/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KTQAlarmClockOverDlg.h
 * @brief       main entry
*/

#include <QApplication>
#include "KTQAlarmClockDlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KTQAlarmClockDlg mainDlg;
    mainDlg.setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    mainDlg.show();
    mainDlg.move(mainDlg.x() + 400, 0);

    return a.exec();
}
