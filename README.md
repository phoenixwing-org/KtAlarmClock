# KtAlarmClock

KtAlarmClock is a Qt alarm clock for focused work and enforced breaks. It uses a C++ backend for timing, settings, tray integration, and multi-screen lock orchestration, with QML for the visible UI.

## Current Architecture

| Area | Path | Notes |
| --- | --- | --- |
| App entry | `KtAlarmClock/` | `QApplication` entry point and Windows rc/manifest setup |
| UI/backend module | `KtAlarmClockUI/` | C++ controller, QPC countdown, settings, tray service, QML UI |
| QML UI | `KtAlarmClockUI/res/qml` | Main bubble, settings panel, and lock screen views |
| Images | `KtAlarmClockUI/res/image` | Runtime SVG resources declared directly in CMake |

Technology stack:

- Qt 6: Core, QML, Quick, QuickControls2, Widgets, Svg, LinguistTools
- C++20
- QPC/monotonic countdown as the authoritative time source
- `QSystemTrayIcon` retained behind `KtTrayService`

## Main Features

- Work countdown with pause/resume and remaining-time adjustment.
- Break countdown with forced period support.
- QML main floating bubble and QML settings panel.
- QML lock-screen overlays managed by C++ for primary/secondary screens.
- System tray actions isolated in `KtTrayService`.
- Settings are loaded/saved through a user-local ini file managed by `KtAlarmClockSettings`.
- Runtime translations are generated from `KtAlarmClockUI/res/linguist` and loaded from Qt resources.
- No standalone `.qrc`; image and QML resources are declared in CMake.

## Build

Optional environment variables:

| Variable | Purpose |
| --- | --- |
| `ROOT_DIR` | Output root; defaults to the repository root when unset |
| `KT_ALARM_CLOCK_LOCK_DEBUG` | Set to `1` to use the single-screen lock-screen debug layout |

Example MSVC build:

```bat
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
set ROOT_DIR=C:\path\to\KtAlarmClock-master
cmake -S . -B out\build\debug -G Ninja -DCMAKE_PREFIX_PATH=C:\Qt\6.7.2\msvc2019_64
cmake --build out\build\debug --config Debug
```

Or run the repository helper:

```bat
build.bat
```

`build.bat` cleans `out`, configures a Release Ninja build, runs core tests, and
generates the dist folder under `out\kt\viewer\dist`.
Set `KT_QT_ROOT` to override the Qt installation path; otherwise the helper uses
`QTDIR`, then falls back to `C:\Qt\6.7.2\msvc2019_64`.

Useful CMake options and variables:

| Name | Purpose |
| --- | --- |
| `KT_APP_NAME` | Executable, settings file, and registry key name |
| `KT_APP_DISPLAY_NAME` | User-visible application name |
| `KT_COMPANY` / `KT_DOMAIN` / `KT_DESCRIPTION` | Metadata injected into C++ and Windows version resources |
| `KT_PROJECT_BRANCH` | `debug` or `release`; Debug branch uses console subsystem for easier logs |
| `KT_ENABLE_QML_DEBUG` | Enables `QT_QML_DEBUG` for debug-like configs; off by default |
| `KT_ENABLE_CONSOLE_SUBSYSTEM` | Builds the app executable as a console-subsystem Windows app; defaults to on for debug branches |
| `KT_DIST_SUBSYSTEM` | Subsystem applied to the copied dist executable: `CONSOLE` or `WINDOWS`; defaults to `WINDOWS` |
| `KT_ENABLE_WINDEPLOYQT` | Runs deployment target with `windeployqt`; on by default |
| `KT_DIST_PATH` | Output path for the `KtAlarmClock_dist` target; defaults to `%ROOT_DIR%\kt\viewer\dist` |
| `KT_DIST_CLEANUP` | Removes unused files from `KtAlarmClock_dist`; on by default |
| `KT_DIST_STRIP_OWN_QML_SOURCES` | Removes the app's own QML source files from dist after they are embedded; on by default |

Outputs are configured by `common.cmake` under:

- Debug: `%ROOT_DIR%\kt\viewer\debug`
- Release: `%ROOT_DIR%\kt\viewer\bin`
- Dist: `%ROOT_DIR%\kt\viewer\dist`

On Windows, `KtAlarmClock_deploy` is part of the default build when
`KT_ENABLE_WINDEPLOYQT` is on. It refreshes Qt runtime DLLs, `qt.conf`, Qt QML
modules, and the app's own `qml/KtAlarmClockQml` module beside the executable.
This keeps debug runs from failing on missing files such as
`Qt6QuickControls2d.dll`.

Create a distributable folder with:

```bat
cmake --build out\build\debug --config Debug --target KtAlarmClock_dist
```

## Key Code Paths

- `KtAlarmClockUI/src/source/KtAlarmClockController.cpp`: application state machine and QML command API.
- `KtAlarmClockUI/src/source/KtClockRuntime.cpp`: QPC-backed work countdown runtime.
- `KtAlarmClockUI/src/source/KtLockScreenManager.cpp`: multi-screen QML lock-window orchestration.
- `KtAlarmClockUI/src/source/KtTrayService.cpp`: isolated `QSystemTrayIcon` and `QMenu` integration.
- `KtAlarmClockUI/src/source/KtAlarmClockSettings.cpp`: settings normalization and persistence.
- `KtAlarmClockUI/res/qml/Main.qml`: main QML shell.
- `KtAlarmClockUI/res/qml/LockPrimary.qml` / `LockSecondary.qml`: lock-screen UI.

More notes are in [doc/README.md](doc/README.md).
