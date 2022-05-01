/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockDlg.h
 * @brief       main dialog class
 */

#ifndef KTQAlarmClockDlg_H
#define KTQAlarmClockDlg_H

#include <QQuickItem>
#include <QString>

// KTQ
#include "KTQAlarmClockParam.h"

/**
 * @brief Class KTQAlarmClockDlg
 */
class KTQAlarmClockDlg : public QQuickItem {
    Q_OBJECT;
    Q_DISABLE_COPY(KTQAlarmClockDlg);

public:
    explicit KTQAlarmClockDlg(QQuickItem *parent = nullptr);
    ~KTQAlarmClockDlg() override;

    // property
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)

public:
    QString getName() const;
    void    setName(const QString &name);

signals:
    void nameChanged();

private:
    QString m_name;
};

#endif // KTQAlarmClockDlg_H
