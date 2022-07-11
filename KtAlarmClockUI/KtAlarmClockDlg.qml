import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0
import Qt.labs.platform 1.1
import "KtAlarmClock.js" as MainJs

/**
 * @brief root window
 */
Window {
    id: root
    y: 2

    width: clock.width
    height: clock.height
    property bool canClose: false
    property var settingDlg: null

    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    // control move
    KtMouseAreaMove{
        targetFill: root.contentItem
        targetMove: root
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        // 该属性设置为false的话，则鼠标的进入 离开 移动不能捕获到
        hoverEnabled: false
        onClicked: {
            if(mouse.button===Qt.RightButton) MainJs.loadPopMenu()
        }
    }
    
    // clock item
    MyClock{
        id: clock
        visible: !MyOver0.visible
        counter: -100
    }

    // tray icon
    MySystemTrayIcon {
        id:trayIcon
    }
    
    // loader for over0
    Loader {
        id: loaderOver0
    }

    Connections {
        target: KtAlarmTheme
        onWorkStepChanged: MainJs.afterWorkStepChanged
    }

    //@disable-check M16
    onClosing: function(closeEvent){
        closeEvent.accepted = root.canClose //accept the clos
        console.log("main .onClosing(), canClose=",canClose)

    }

    // connect signal onCompleted
    Component.onCompleted: MainJs.afterCompleted()
}
