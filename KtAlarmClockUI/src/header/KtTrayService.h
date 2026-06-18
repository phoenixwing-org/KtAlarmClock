#ifndef KtTrayService_H
#define KtTrayService_H

#include "KtAlarmClockUI.h"

#include <QObject>
#include <QPoint>
#include <QString>
#include <QSystemTrayIcon>

class QAction;
class QMenu;

class ExportedByKtAlarmClockUI KtTrayService : public QObject {
    Q_OBJECT

public:
    explicit KtTrayService(QObject* parent = nullptr);
    ~KtTrayService() override;

    void hide();
    void retranslate();
    void setDebugLocate(bool enabled);
    void setForbidden(bool forbidden);
    void setRemainingText(const QString& text);
    void setRunning(bool running);
    void showContextMenuAt(const QPoint& globalPos);

private:
    void build_menu();
    void emit_if_allowed(void (KtTrayService::*signalMethod)());
    void update_menu_state();
    void update_tooltip();

signals:
    void breakRequested();
    void helpRequested();
    void nextLoopRequested();
    void playPauseRequested();
    void quitRequested();
    void settingsRequested();
    void websiteRequested();

private slots:
    void on_tray_activated(QSystemTrayIcon::ActivationReason reason);

private:
    QSystemTrayIcon* trayIcon_;
    QMenu*           menu_;
    QAction*         titleAction_;
    QAction*         playPauseAction_;
    QAction*         breakAction_;
    QAction*         nextLoopAction_;
    QAction*         settingsAction_;
    QAction*         helpAction_;
    QAction*         websiteAction_;
    QAction*         quitAction_;
    QString          remainingText_;
    bool             running_;
    bool             debugLocate_;
    bool             forbidden_;
};

#endif // KtTrayService_H
