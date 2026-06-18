#ifndef KtClockRuntime_H
#define KtClockRuntime_H

#include "KtMonotonicCountdown.h"
#include "KtAlarmClockUI.h"

#include <QObject>

class QTimer;

class ExportedByKtAlarmClockUI KtClockRuntime : public QObject {
    Q_OBJECT

public:
    explicit KtClockRuntime(QObject* parent = nullptr);
    ~KtClockRuntime() override;

    int counter() const;
    void pause();
    int phase() const;
    bool running() const;

    void start(int phase, int counterSec);
    void sync_from_monotonic_clock();

private:
    void emit_updated();

signals:
    void timeout(int phase);
    void updated(int phase, int counterSec, bool running);

private slots:
    void on_tick();

private:
    KtMonotonicCountdown clock_;
    QTimer* timer_;
};

#endif // KtClockRuntime_H
