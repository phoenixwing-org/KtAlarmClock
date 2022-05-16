/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClockDlg.h
 * @brief       main dialog class
 */

#ifndef KtAlarmClockDlg_H
#define KtAlarmClockDlg_H

#include <QQuickItem>
#include <QString>

// Kt
#include "KtAlarmClockParam.h"

/**
 * @brief Class KtAlarmClockDlg
 */
class KtAlarmClockDlg : public QQuickItem {
    Q_OBJECT;
    Q_DISABLE_COPY(KtAlarmClockDlg);

public:
    explicit KtAlarmClockDlg(QQuickItem* parent = nullptr);
    ~KtAlarmClockDlg() override;

    // property
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)

public:
    QString getName() const;
    void    setName(const QString& name);

signals:
    void nameChanged();

private:
    QString m_name;
};

#endif // KtAlarmClockDlg_H
