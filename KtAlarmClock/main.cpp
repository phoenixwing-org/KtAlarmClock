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

// Kt
#include "KtAlarmClockCmd.h"

// std
#include <iostream>

int main(int argc, char* argv[]) {
    // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QGuiApplication       app(argc, argv);
    QQmlApplicationEngine engine;
    KtAlarmClockCmd       cmd;

    // qDebug() << argv[ 0 ];
    cmd.setExePath(argv[ 0 ]); // set path

    // cmd.debug(" Clock");      // debug
    int code = cmd.buildDialog(&engine); // build dialog
    if (code) {
        QCoreApplication::exit(-1);
    }

    return app.exec();
}
