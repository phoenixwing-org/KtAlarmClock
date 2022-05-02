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
// kt
#include "ktErrorCode.h"
// KTQ
#include "KTQAlarmClockCmd.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication       app(argc, argv);
    QQmlApplicationEngine engine;
    KTQAlarmClockCmd      cmd;

    // cmd.debug(" Clock");      // debug
    ktErrorCode ec = cmd.BuildDialog(&engine); // build diglog
    if (KT_FAILED(ec)) {
        QCoreApplication::exit(-1);
    }

    return app.exec();
}
