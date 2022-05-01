/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        main.cpp
 * @brief       main entry of the application
 */

// Qt
#include <QGuiApplication>
#include <QQmlApplicationEngine>
// KTQ
#include "KTQAlarmClockCmd.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication       app(argc, argv);
    QQmlApplicationEngine engine;
    KTQAlarmClockCmd      cmd;

    // cmd.debug(" Clock");      // debug
    cmd.BuildDialog(&engine); // build diglog

    return app.exec();
}
