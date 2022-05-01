#ifndef KTQAlarmClockCmd_H
#define KTQAlarmClockCmd_H

#include <QQuickItem>

class KTQAlarmClockCmd : public QQuickItem {
    Q_OBJECT;
    Q_DISABLE_COPY(KTQAlarmClockCmd);

public:
    explicit KTQAlarmClockCmd(QQuickItem *parent = nullptr);
    ~KTQAlarmClockCmd() override;
};

#endif // KTQAlarmClockCmd_H
