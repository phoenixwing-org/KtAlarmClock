/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @file        KtAlarmClockSettings.cpp
 */
#include "KtAlarmClockSettings.h"

#include "KtAlarmClock.h"
#include "KtAlarmClockParam.h"

#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QSettings>
#include <QStandardPaths>
#include <QString>

namespace {
constexpr auto kSettingsPathEnv = "KT_ALARM_CLOCK_SETTINGS_PATH";
constexpr auto kWorkTimeKey      = "WorkTime";
constexpr auto kWorkBreakKey     = "WorkBreak";
constexpr auto kTimeForceKey     = "TimeForce";
constexpr auto kLocaleKey        = "Locale";

int valid_or_default(int value, const KtDurationBounds& bounds) {
    if (value < bounds.minimum || value > bounds.maximum)
        return bounds.fallback;
    return value;
}

QSettings open_settings() {
    const QFileInfo info(KtAlarmClockSettings::file_path());
    QDir().mkpath(info.absolutePath());
    return QSettings(info.absoluteFilePath(), QSettings::IniFormat);
}

QString default_locale_name() {
    const QLocale systemLocale;
    if (systemLocale.language() == QLocale::Chinese)
        return QStringLiteral("zh_CN");
    return QStringLiteral("en_US");
}
} // namespace

//------------------------------------------------------
void KtAlarmClockSettings::apply_defaults(KtAlarmClockParam& param) {
    param.WorkTime    = DefaultWorkTime;
    param.WorkBreak   = DefaultWorkBreak;
    param.TimeForce   = DefaultTimeForce;
    param.WorkStep    = KtAlarmClock::None;
    param.TimeCounter = 0;
    param.TimeTotal   = param.WorkTime + param.WorkBreak;
}

//------------------------------------------------------
QString KtAlarmClockSettings::file_path() {
    const QByteArray overridePath = qgetenv(kSettingsPathEnv);
    if (!overridePath.isEmpty())
        return QString::fromLocal8Bit(overridePath);

    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (dirPath.isEmpty())
        dirPath = QDir::home().filePath(QStringLiteral(".") + QStringLiteral(PROJECT_APP_NAME));

    return QDir(dirPath).filePath(QStringLiteral(PROJECT_APP_NAME) + QStringLiteral(".ini"));
}

//------------------------------------------------------
QString KtAlarmClockSettings::normalize_locale(const QString& localeName) {
    const QString normalized = localeName.trimmed().replace(QLatin1Char('-'), QLatin1Char('_'));
    if (normalized == QStringLiteral("zh_CN") || normalized == QStringLiteral("en_US"))
        return normalized;
    return default_locale_name();
}

//------------------------------------------------------
QString KtAlarmClockSettings::read_locale() {
    QSettings setting = open_settings();
    return normalize_locale(setting.value(QString::fromLatin1(kLocaleKey), default_locale_name()).toString());
}

//------------------------------------------------------
void KtAlarmClockSettings::normalize(KtAlarmClockParam& param) {
    param.WorkTime  = valid_or_default(param.WorkTime, work_time_bounds());
    param.WorkBreak = valid_or_default(param.WorkBreak, work_break_bounds());

    if (param.TimeForce < time_force_bounds().minimum)
        param.TimeForce = DefaultTimeForce;
    if (param.TimeForce > param.WorkBreak)
        param.TimeForce = param.WorkBreak;
    if (param.TimeForce > time_force_bounds().maximum)
        param.TimeForce = time_force_bounds().maximum;

    if (param.WorkStep < KtAlarmClock::None || param.WorkStep > KtAlarmClock::WorkBreak)
        param.WorkStep = KtAlarmClock::None;
    if (param.TimeCounter < 0)
        param.TimeCounter = 0;

    param.TimeTotal = param.WorkTime + param.WorkBreak;
}

//------------------------------------------------------
void KtAlarmClockSettings::read(KtAlarmClockParam& param) {
    QSettings setting = open_settings();

    param.WorkTime  = setting.value(QString::fromLatin1(kWorkTimeKey), param.WorkTime).toInt();
    param.WorkBreak = setting.value(QString::fromLatin1(kWorkBreakKey), param.WorkBreak).toInt();
    param.TimeForce = setting.value(QString::fromLatin1(kTimeForceKey), param.TimeForce).toInt();
    normalize(param);
}

//------------------------------------------------------
void KtAlarmClockSettings::write_locale(const QString& localeName) {
    QSettings setting = open_settings();
    setting.setValue(QString::fromLatin1(kLocaleKey), normalize_locale(localeName));
    setting.sync();
}

//------------------------------------------------------
void KtAlarmClockSettings::write(KtAlarmClockParam& param) {
    normalize(param);

    QSettings setting = open_settings();
    setting.setValue(QString::fromLatin1(kWorkBreakKey), param.WorkBreak);
    setting.setValue(QString::fromLatin1(kTimeForceKey), param.TimeForce);
    setting.setValue(QString::fromLatin1(kWorkTimeKey), param.WorkTime);
    setting.sync();
}
