use clock_platform::sample_process;
use std::error::Error;
use std::fmt::Write as _;
use std::path::{Path, PathBuf};
use std::process::{Child, Command, Stdio};
use std::thread;
use std::time::{Duration, Instant};

const WARM_UP: Duration = Duration::from_secs(4);
const SAMPLE_DURATION: Duration = Duration::from_secs(5);
const SAMPLE_INTERVAL: Duration = Duration::from_millis(250);

#[derive(Debug)]
struct ModeReport {
    mode: &'static str,
    binary_bytes: u64,
    overlay_count: u64,
    overlay_pixels: u64,
    max_resident_bytes: u64,
    max_private_bytes: u64,
    average_cpu_percent: f64,
}

#[test]
#[ignore = "以 stripped Release 子进程采样；使用文档中的专用命令运行"]
fn release_resource_usage() -> Result<(), Box<dyn Error>> {
    if cfg!(debug_assertions) {
        return Err("资源测试必须添加 --release，Debug 数据没有发布意义".into());
    }

    let binary = PathBuf::from(env!("CARGO_BIN_EXE_KtAlarmClock"));
    let scratch = tempfile::tempdir()?;
    let idle = measure_mode(&binary, scratch.path(), "idle", "--resource-test")?;
    let rest = measure_mode(&binary, scratch.path(), "break", "--resource-test-break")?;
    let report = report_json(&idle, &rest);
    let report_path = report_path(&binary);
    std::fs::write(&report_path, &report)?;

    println!("\n{report}");
    println!("资源报告: {}", report_path.display());
    enforce_optional_budget("CLOCK_MAX_BINARY_BYTES", idle.binary_bytes)?;
    enforce_optional_budget("CLOCK_MAX_IDLE_PRIVATE_BYTES", idle.max_private_bytes)?;
    enforce_optional_budget("CLOCK_MAX_BREAK_PRIVATE_BYTES", rest.max_private_bytes)?;
    enforce_optional_cpu_budget("CLOCK_MAX_IDLE_CPU_PERCENT", idle.average_cpu_percent)?;
    Ok(())
}

fn measure_mode(
    binary: &Path,
    scratch: &Path,
    mode: &'static str,
    mode_argument: &str,
) -> Result<ModeReport, Box<dyn Error>> {
    let data_dir = scratch.join(mode);
    let state_path = data_dir.join("resource-state.txt");
    let child = Command::new(binary)
        .arg(mode_argument)
        .arg("--resource-test-seconds=12")
        .env("KT_ALARM_CLOCK_DATA_DIR", &data_dir)
        .env("KT_ALARM_CLOCK_RESOURCE_STATE_PATH", &state_path)
        .stdin(Stdio::null())
        .stdout(Stdio::null())
        .stderr(Stdio::null())
        .spawn()?;
    let mut child = ChildGuard(Some(child));

    thread::sleep(WARM_UP);
    ensure_running(child.child_mut(), mode)?;

    let started = Instant::now();
    let first = sample_process(child.child_mut().id())?;
    let mut last = first;
    let mut max_resident = first.resident_bytes;
    let mut max_private = first.private_bytes;
    while started.elapsed() < SAMPLE_DURATION {
        thread::sleep(SAMPLE_INTERVAL);
        ensure_running(child.child_mut(), mode)?;
        last = sample_process(child.child_mut().id())?;
        max_resident = max_resident.max(last.resident_bytes);
        max_private = max_private.max(last.private_bytes);
    }

    let elapsed = started.elapsed().as_secs_f64();
    let cpu_seconds = last.cpu_time.saturating_sub(first.cpu_time).as_secs_f64();
    let average_cpu_percent = if elapsed > 0.0 {
        cpu_seconds / elapsed * 100.0
    } else {
        0.0
    };

    let status = child.child_mut().wait()?;
    if !status.success() {
        return Err(format!("{mode} 资源测试子进程退出失败: {status}").into());
    }
    child.0 = None;
    let (overlay_count, overlay_pixels) = read_resource_state(&state_path)?;

    Ok(ModeReport {
        mode,
        binary_bytes: std::fs::metadata(binary)?.len(),
        overlay_count,
        overlay_pixels,
        max_resident_bytes: max_resident,
        max_private_bytes: max_private,
        average_cpu_percent,
    })
}

fn ensure_running(child: &mut Child, mode: &str) -> Result<(), Box<dyn Error>> {
    if let Some(status) = child.try_wait()? {
        Err(format!("{mode} 资源测试子进程过早退出: {status}").into())
    } else {
        Ok(())
    }
}

struct ChildGuard(Option<Child>);

impl ChildGuard {
    fn child_mut(&mut self) -> &mut Child {
        self.0.as_mut().expect("资源测试子进程已经被取走")
    }
}

impl Drop for ChildGuard {
    fn drop(&mut self) {
        if let Some(child) = self.0.as_mut() {
            let _ = child.kill();
            let _ = child.wait();
        }
    }
}

fn report_json(idle: &ModeReport, rest: &ModeReport) -> String {
    let mut output = String::new();
    writeln!(&mut output, "{{").unwrap();
    writeln!(&mut output, "  \"schema_version\": 1,").unwrap();
    writeln!(&mut output, "  \"platform\": \"{}\",", std::env::consts::OS).unwrap();
    writeln!(&mut output, "  \"profile\": \"release\",").unwrap();
    writeln!(&mut output, "  \"renderer\": \"software\",").unwrap();
    writeln!(&mut output, "  \"samples\": [").unwrap();
    write_mode_json(&mut output, idle, true);
    write_mode_json(&mut output, rest, false);
    writeln!(&mut output, "  ]").unwrap();
    writeln!(&mut output, "}}").unwrap();
    output
}

fn write_mode_json(output: &mut String, report: &ModeReport, comma: bool) {
    writeln!(output, "    {{").unwrap();
    writeln!(output, "      \"mode\": \"{}\",", report.mode).unwrap();
    writeln!(output, "      \"binary_bytes\": {},", report.binary_bytes).unwrap();
    writeln!(output, "      \"overlay_count\": {},", report.overlay_count).unwrap();
    writeln!(
        output,
        "      \"overlay_pixels\": {},",
        report.overlay_pixels
    )
    .unwrap();
    writeln!(
        output,
        "      \"max_resident_bytes\": {},",
        report.max_resident_bytes
    )
    .unwrap();
    writeln!(
        output,
        "      \"max_private_bytes\": {},",
        report.max_private_bytes
    )
    .unwrap();
    writeln!(
        output,
        "      \"average_cpu_percent\": {:.4}",
        report.average_cpu_percent
    )
    .unwrap();
    writeln!(output, "    }}{}", if comma { "," } else { "" }).unwrap();
}

fn report_path(binary: &Path) -> PathBuf {
    binary
        .parent()
        .and_then(Path::parent)
        .unwrap_or_else(|| Path::new("target"))
        .join("resource-usage.json")
}

fn read_resource_state(path: &Path) -> Result<(u64, u64), Box<dyn Error>> {
    let source = std::fs::read_to_string(path)?;
    let value = |name: &str| -> Result<u64, Box<dyn Error>> {
        let prefix = format!("{name}=");
        let value = source
            .lines()
            .find_map(|line| line.strip_prefix(&prefix))
            .ok_or_else(|| format!("资源状态缺少 {name}"))?;
        Ok(value.parse()?)
    };
    Ok((value("overlay_count")?, value("overlay_pixels")?))
}

fn enforce_optional_budget(name: &str, actual: u64) -> Result<(), Box<dyn Error>> {
    let Some(limit) = std::env::var_os(name) else {
        return Ok(());
    };
    let limit: u64 = limit.to_string_lossy().parse()?;
    if actual > limit {
        Err(format!("{name} 超限: actual={actual}, limit={limit}").into())
    } else {
        Ok(())
    }
}

fn enforce_optional_cpu_budget(name: &str, actual: f64) -> Result<(), Box<dyn Error>> {
    let Some(limit) = std::env::var_os(name) else {
        return Ok(());
    };
    let limit: f64 = limit.to_string_lossy().parse()?;
    if actual > limit {
        Err(format!("{name} 超限: actual={actual:.4}, limit={limit:.4}").into())
    } else {
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn report_is_machine_readable_without_locale_numbers() {
        let sample = ModeReport {
            mode: "idle",
            binary_bytes: 10,
            overlay_count: 1,
            overlay_pixels: 2,
            max_resident_bytes: 20,
            max_private_bytes: 30,
            average_cpu_percent: 0.125,
        };
        let report = report_json(&sample, &sample);
        assert!(report.contains("\"average_cpu_percent\": 0.1250"));
        assert!(!report.contains("0,125"));
    }
}
