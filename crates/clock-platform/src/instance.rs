use crate::PlatformResult;
use std::fs::{File, OpenOptions};
use std::io::ErrorKind;
#[cfg(target_os = "windows")]
use std::io::{Read, Seek, SeekFrom, Write};
#[cfg(target_os = "windows")]
use std::net::{Ipv4Addr, SocketAddrV4, TcpListener, TcpStream};
#[cfg(unix)]
use std::os::unix::net::{UnixListener, UnixStream};
use std::path::Path;
#[cfg(unix)]
use std::path::PathBuf;
use std::thread;
use std::time::Duration;

const ACTIVATION_RETRIES: usize = 10;
const ACTIVATION_RETRY_DELAY: Duration = Duration::from_millis(20);

#[derive(Debug)]
pub enum InstanceRole {
    Primary(SingleInstance),
    Secondary,
}

/// 持有用户配置目录中的文件锁，并接收“激活已有实例”。
///
/// 文件锁随进程退出自动释放。Unix 使用同目录 Unix socket；Windows 将随机
/// localhost 端口写进已加锁的文件，避免固定端口冲突。
#[derive(Debug)]
pub struct SingleInstance {
    _lock_file: File,
    #[cfg(target_os = "windows")]
    listener: TcpListener,
    #[cfg(unix)]
    listener: UnixListener,
    #[cfg(unix)]
    socket_path: PathBuf,
}

impl SingleInstance {
    pub fn acquire(lock_path: &Path) -> PlatformResult<InstanceRole> {
        if let Some(parent) = lock_path.parent() {
            std::fs::create_dir_all(parent)?;
        }

        let mut lock_file = OpenOptions::new()
            .create(true)
            .read(true)
            .write(true)
            .truncate(false)
            .open(lock_path)?;

        match lock_file.try_lock() {
            Ok(()) => Self::become_primary(lock_file, lock_path),
            Err(std::fs::TryLockError::WouldBlock) => {
                notify_primary(lock_path, &mut lock_file);
                Ok(InstanceRole::Secondary)
            }
            Err(std::fs::TryLockError::Error(error)) => Err(error.into()),
        }
    }

    #[cfg(target_os = "windows")]
    fn become_primary(mut lock_file: File, _lock_path: &Path) -> PlatformResult<InstanceRole> {
        let listener = TcpListener::bind(SocketAddrV4::new(Ipv4Addr::LOCALHOST, 0))?;
        listener.set_nonblocking(true)?;
        let port = listener.local_addr()?.port();

        lock_file.set_len(0)?;
        lock_file.seek(SeekFrom::Start(0))?;
        write!(lock_file, "{port}\n")?;
        lock_file.sync_data()?;

        Ok(InstanceRole::Primary(Self {
            _lock_file: lock_file,
            listener,
        }))
    }

    #[cfg(unix)]
    fn become_primary(lock_file: File, lock_path: &Path) -> PlatformResult<InstanceRole> {
        let socket_path = lock_path.with_extension("sock");
        if socket_path.exists() {
            std::fs::remove_file(&socket_path)?;
        }
        let listener = UnixListener::bind(&socket_path)?;
        listener.set_nonblocking(true)?;
        Ok(InstanceRole::Primary(Self {
            _lock_file: lock_file,
            listener,
            socket_path,
        }))
    }

    /// 消耗当前排队的连接；任一成功连接都表示第二实例请求显示主窗口。
    pub fn take_activation_request(&self) -> PlatformResult<bool> {
        let mut activated = false;
        loop {
            match self.listener.accept() {
                Ok((_stream, _address)) => activated = true,
                Err(error) if error.kind() == ErrorKind::WouldBlock => return Ok(activated),
                Err(error) => return Err(error.into()),
            }
        }
    }
}

#[cfg(unix)]
impl Drop for SingleInstance {
    fn drop(&mut self) {
        let _ = std::fs::remove_file(&self.socket_path);
    }
}

#[cfg(target_os = "windows")]
fn notify_primary(_lock_path: &Path, lock_file: &mut File) {
    for _ in 0..ACTIVATION_RETRIES {
        let mut port_text = String::new();
        if lock_file.seek(SeekFrom::Start(0)).is_ok()
            && lock_file.read_to_string(&mut port_text).is_ok()
            && port_text
                .trim()
                .parse::<u16>()
                .ok()
                .and_then(|port| {
                    TcpStream::connect_timeout(
                        &SocketAddrV4::new(Ipv4Addr::LOCALHOST, port).into(),
                        Duration::from_millis(50),
                    )
                    .ok()
                })
                .is_some()
        {
            return;
        }
        thread::sleep(ACTIVATION_RETRY_DELAY);
    }
}

#[cfg(unix)]
fn notify_primary(lock_path: &Path, _lock_file: &mut File) {
    let socket_path = lock_path.with_extension("sock");
    for _ in 0..ACTIVATION_RETRIES {
        if UnixStream::connect(&socket_path).is_ok() {
            return;
        }
        thread::sleep(ACTIVATION_RETRY_DELAY);
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn second_instance_notifies_primary() {
        let directory = tempfile::tempdir().unwrap();
        let lock_path = directory.path().join("instance.lock");
        let InstanceRole::Primary(primary) = SingleInstance::acquire(&lock_path).unwrap() else {
            panic!("第一个实例必须取得锁");
        };

        assert!(matches!(
            SingleInstance::acquire(&lock_path).unwrap(),
            InstanceRole::Secondary
        ));
        assert!(primary.take_activation_request().unwrap());
        assert!(!primary.take_activation_request().unwrap());
    }

    #[test]
    fn stale_lock_file_can_be_reused() {
        let directory = tempfile::tempdir().unwrap();
        let lock_path = directory.path().join("instance.lock");
        std::fs::write(&lock_path, "1\n").unwrap();

        assert!(matches!(
            SingleInstance::acquire(&lock_path).unwrap(),
            InstanceRole::Primary(_)
        ));
    }
}
