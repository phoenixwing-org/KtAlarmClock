#ifndef KtTranslationManager_H
#define KtTranslationManager_H

#include "KtAlarmClockUI.h"

#include <QObject>
#include <QString>
#include <QTranslator>

class ExportedByKtAlarmClockUI KtTranslationManager : public QObject {
    Q_OBJECT

public:
    explicit KtTranslationManager(QObject* parent = nullptr);

    bool install(const QString& localeName = QString());
    QString locale() const;

    static QString normalize_locale(const QString& localeName);

private:
    QString     locale_;
    QTranslator translator_;
};

#endif // KtTranslationManager_H
