/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     Apache-2.0
 * @author      Kuntai
 * @file        main.cpp
 * @brief       main entry of the application
 */

// Qt
#include <QApplication>
#include <QDebug>

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#include <QCoreApplication>
#endif

// Kt
#include "KtAlarmClockCmd.h"
#include "KtAlarmClockUI.h"

// std
#include <iostream>

int main(int argc, char* argv[]) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough); // 150%/175% 等小数缩放更平滑
#endif
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication    app(argc, argv);
    KtAlarmClockCmd cmd;

    // qDebug() << argv[ 0 ];
    cmd.setExePath(argv[ 0 ]); // set path

    // cmd.debug(" Clock");      // debug
    const int code = cmd.build();
    if (code == KT_S_ALREADY_RUNNING)
        return 0;
    if (code != KT_S_OK)
        return -1;

    return app.exec();
}
