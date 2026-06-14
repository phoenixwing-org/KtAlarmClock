#include "KtTranslationManager.h"

#include "KtAlarmClockSettings.h"

#include <QCoreApplication>
#include <QLocale>

KtTranslationManager::KtTranslationManager(QObject* parent)
    : QObject(parent)
    , locale_()
    , translator_(this) {
}

bool KtTranslationManager::install(const QString& localeName) {
    const QString normalizedLocale = normalize_locale(localeName);

    if (!locale_.isEmpty())
        QCoreApplication::removeTranslator(&translator_);

    const QString resourcePath =
        QStringLiteral(":/i18n/%1_%2.qm").arg(QStringLiteral(PROJECT_APP_NAME), normalizedLocale);
    const bool loaded = translator_.load(resourcePath);
    if (loaded)
        QCoreApplication::installTranslator(&translator_);

    locale_ = normalizedLocale;
    QLocale::setDefault(QLocale(locale_));
    return loaded;
}

QString KtTranslationManager::locale() const {
    return locale_;
}

QString KtTranslationManager::normalize_locale(const QString& localeName) {
    return KtAlarmClockSettings::normalize_locale(localeName);
}
