/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        main.cpp
 * @brief       main entry of the application
 */

// Qt
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
// kt
#include "ktErrorCode.h"
// Kt
#include "KtAlarmClockCmd.h"

// std
#include <iostream>

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication       app(argc, argv);
    QQmlApplicationEngine engine;
    KtAlarmClockCmd       cmd;

    // qDebug() << argv[ 0 ];
    cmd.setExePath(argv[ 0 ]); // set path

    // cmd.debug(" Clock");      // debug
    ktErrorCode ec = cmd.buildDialog(&engine); // build diglog
    if (KT_FAILED(ec)) {
        QCoreApplication::exit(-1);
    }

    return app.exec();
}
