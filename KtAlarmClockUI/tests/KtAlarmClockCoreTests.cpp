#include "KtAlarmClock.h"
#include "KtClockRuntime.h"
#include "KtAlarmClockParam.h"
#include "KtAlarmClockSettings.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QThread>
#include <QTemporaryDir>

#include <cstdio>

namespace {

int fail(const char* expression, int line) {
    std::fprintf(stderr, "CHECK failed at line %d: %s\n", line, expression);
    std::fflush(stderr);
    qCritical() << "CHECK failed at line" << line << ":" << expression;
    return 1;
}

#define CHECK(expression)              \
    do {                               \
        if (!(expression))             \
            return fail(#expression, __LINE__); \
    } while (false)

int test_settings_normalize() {
    KtAlarmClockParam param;
    param.WorkTime = -1;
    param.WorkBreak = 999999;
    param.TimeForce = 999999;
    param.WorkStep = 99;
    param.TimeCounter = -5;

    KtAlarmClockSettings::normalize(param);

    CHECK(param.WorkTime == KtAlarmClockSettings::DefaultWorkTime);
    CHECK(param.WorkBreak == KtAlarmClockSettings::DefaultWorkBreak);
    CHECK(param.TimeForce == param.WorkBreak);
    CHECK(param.WorkStep == KtAlarmClock::None);
    CHECK(param.TimeCounter == 0);
    CHECK(param.TimeTotal == param.WorkTime + param.WorkBreak);

    param.WorkTime = 60;
    param.WorkBreak = 120;
    param.TimeForce = 120;
    KtAlarmClockSettings::normalize(param);

    CHECK(param.WorkTime == 60);
    CHECK(param.WorkBreak == 120);
    CHECK(param.TimeForce == 120);

    return 0;
}

int test_clock_runtime() {
    KtClockRuntime runtime;
    runtime.start(KtAlarmClock::WorkTime, 2);

    CHECK(runtime.running());
    CHECK(runtime.phase() == KtAlarmClock::WorkTime);
    CHECK(runtime.counter() <= 2);
    CHECK(runtime.counter() > 0);

    QThread::msleep(1100);
    runtime.sync_from_monotonic_clock();

    CHECK(runtime.counter() <= 1);
    CHECK(runtime.counter() >= 0);

    runtime.pause();
    const int pausedCounter = runtime.counter();
    QThread::msleep(300);
    runtime.sync_from_monotonic_clock();

    CHECK(!runtime.running());
    CHECK(runtime.counter() == pausedCounter);

    return 0;
}

int test_settings_persistence() {
    QTemporaryDir tempDir;
    CHECK(tempDir.isValid());

    const QString settingsPath =
        QDir(tempDir.path()).filePath(QStringLiteral(PROJECT_APP_NAME) + QStringLiteral(".ini"));
    qputenv("KT_ALARM_CLOCK_SETTINGS_PATH", settingsPath.toLocal8Bit());

    KtAlarmClockParam saved;
    saved.WorkTime = 600;
    saved.WorkBreak = 300;
    saved.TimeForce = 120;
    KtAlarmClockSettings::write(saved);

    CHECK(QFileInfo::exists(settingsPath));

    KtAlarmClockParam loaded;
    KtAlarmClockSettings::read(loaded);

    CHECK(loaded.WorkTime == 600);
    CHECK(loaded.WorkBreak == 300);
    CHECK(loaded.TimeForce == 120);
    CHECK(loaded.TimeTotal == 900);

    KtAlarmClockSettings::write_locale(QStringLiteral("en-US"));
    CHECK(KtAlarmClockSettings::read_locale() == QStringLiteral("en_US"));

    KtAlarmClockSettings::write_locale(QStringLiteral("zh_CN"));
    CHECK(KtAlarmClockSettings::read_locale() == QStringLiteral("zh_CN"));

    qunsetenv("KT_ALARM_CLOCK_SETTINGS_PATH");
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    QCoreApplication::setOrganizationName(QStringLiteral(PROJECT_COMPANY));
    QCoreApplication::setOrganizationDomain(QStringLiteral(PROJECT_DOMAIN));
    QCoreApplication::setApplicationName(QStringLiteral(PROJECT_APP_NAME) + QStringLiteral("CoreTests"));

    QCoreApplication app(argc, argv);

    if (const int result = test_settings_normalize(); result != 0)
        return result;
    if (const int result = test_settings_persistence(); result != 0)
        return result;
    if (const int result = test_clock_runtime(); result != 0)
        return result;

    qInfo() << "KtAlarmClockCoreTests passed";
    return 0;
}
