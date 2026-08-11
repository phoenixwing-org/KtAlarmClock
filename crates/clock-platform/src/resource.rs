use crate::PlatformResult;
use std::time::Duration;

/// 独立发布进程的一次资源快照。
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct ProcessSample {
    /// 当前驻留物理内存。Windows 为 Working Set，macOS 为 resident size。
    pub resident_bytes: u64,
    /// 更接近应用自身成本的内存。Windows 为 PrivateUsage，macOS 为
    /// `ri_phys_footprint`。
    pub private_bytes: u64,
    /// 进程累计用户态 + 内核态 CPU 时间。
    pub cpu_time: Duration,
}

pub fn sample_process(process_id: u32) -> PlatformResult<ProcessSample> {
    imp::sample(process_id)
}

#[cfg(target_os = "macos")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use std::mem::MaybeUninit;

    pub(super) fn sample(process_id: u32) -> PlatformResult<ProcessSample> {
        let process_id = i32::try_from(process_id)?;
        let mut usage = MaybeUninit::<libc::rusage_info_v4>::zeroed();
        // SAFETY: `usage` points to writable storage of the exact structure requested by
        // RUSAGE_INFO_V4. `proc_pid_rusage` initializes it before returning success.
        let status = unsafe {
            libc::proc_pid_rusage(
                process_id,
                libc::RUSAGE_INFO_V4,
                usage.as_mut_ptr().cast::<libc::rusage_info_t>(),
            )
        };
        if status != 0 {
            return Err(std::io::Error::last_os_error().into());
        }
        // SAFETY: A zero status from proc_pid_rusage means the V4 structure was filled.
        let usage = unsafe { usage.assume_init() };
        Ok(ProcessSample {
            resident_bytes: usage.ri_resident_size,
            private_bytes: usage.ri_phys_footprint,
            cpu_time: Duration::from_nanos(usage.ri_user_time.saturating_add(usage.ri_system_time)),
        })
    }
}

#[cfg(all(test, any(target_os = "macos", target_os = "windows")))]
mod tests {
    use super::*;

    #[test]
    fn current_process_can_be_sampled() {
        let sample = sample_process(std::process::id()).unwrap();
        assert!(sample.resident_bytes > 0);
        assert!(sample.private_bytes > 0);
    }
}

#[cfg(target_os = "windows")]
#[allow(unsafe_code)]
mod imp {
    use super::*;
    use std::mem::size_of;
    use windows::Win32::Foundation::{CloseHandle, FILETIME, HANDLE};
    use windows::Win32::System::ProcessStatus::{
        K32GetProcessMemoryInfo, PROCESS_MEMORY_COUNTERS, PROCESS_MEMORY_COUNTERS_EX,
    };
    use windows::Win32::System::Threading::{
        GetProcessTimes, OpenProcess, PROCESS_QUERY_INFORMATION, PROCESS_VM_READ,
    };

    pub(super) fn sample(process_id: u32) -> PlatformResult<ProcessSample> {
        // SAFETY: The returned handle is owned by ProcessHandle and closed on every path.
        let process = ProcessHandle(unsafe {
            OpenProcess(
                PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                false,
                process_id,
            )?
        });

        let mut memory = PROCESS_MEMORY_COUNTERS_EX {
            cb: size_of::<PROCESS_MEMORY_COUNTERS_EX>() as u32,
            ..Default::default()
        };
        // SAFETY: PROCESS_MEMORY_COUNTERS_EX begins with PROCESS_MEMORY_COUNTERS and `cb`
        // advertises the complete allocation, as required by K32GetProcessMemoryInfo.
        let memory_ok = unsafe {
            K32GetProcessMemoryInfo(
                process.0,
                (&mut memory as *mut PROCESS_MEMORY_COUNTERS_EX).cast::<PROCESS_MEMORY_COUNTERS>(),
                memory.cb,
            )
        };
        memory_ok.ok()?;

        let mut creation = FILETIME::default();
        let mut exit = FILETIME::default();
        let mut kernel = FILETIME::default();
        let mut user = FILETIME::default();
        // SAFETY: All FILETIME pointers are valid writable values for this live handle.
        unsafe { GetProcessTimes(process.0, &mut creation, &mut exit, &mut kernel, &mut user)? };
        let cpu_100ns = file_time_ticks(kernel).saturating_add(file_time_ticks(user));

        Ok(ProcessSample {
            resident_bytes: memory.WorkingSetSize as u64,
            private_bytes: memory.PrivateUsage as u64,
            cpu_time: Duration::from_nanos(cpu_100ns.saturating_mul(100)),
        })
    }

    struct ProcessHandle(HANDLE);

    impl Drop for ProcessHandle {
        fn drop(&mut self) {
            // SAFETY: This wrapper owns the successful OpenProcess result exactly once.
            let _ = unsafe { CloseHandle(self.0) };
        }
    }

    fn file_time_ticks(time: FILETIME) -> u64 {
        (u64::from(time.dwHighDateTime) << 32) | u64::from(time.dwLowDateTime)
    }
}

#[cfg(not(any(target_os = "macos", target_os = "windows")))]
mod imp {
    use super::*;

    pub(super) fn sample(_process_id: u32) -> PlatformResult<ProcessSample> {
        Err("资源采样目前只支持 Windows 和 macOS".into())
    }
}
