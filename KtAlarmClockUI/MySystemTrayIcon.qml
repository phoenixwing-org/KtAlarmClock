import QtQuick 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0
import Qt.labs.platform 1.1

SystemTrayIcon {
    id:trayIcon
    visible: true
    icon.source: "qrc:/image/kt-blue.svg"

    signal sigAction(int index)

    menu: Menu {
        MenuItem {
            text: qsTr("锟钛护眼闹钟")
            iconSource: "qrc:/image/kt.svg"
            onTriggered: {
                if(checkForbiden()) return
                trayIcon.showMessage("锟钛闹钟", "可以定时休息、工作\n"+
                                     "http://www.kuntaisoft.cn")
            }
        }
        
        MenuSeparator{}
        MenuItem {
            text: qsTr("立刻休息")
            iconSource: "qrc:/image/coffee.svg"
            onTriggered: {
                if(checkForbiden()) return
                sigAction(KtAlarmClock.ActionBreak)
            }
        }
        MenuItem {
            text: qsTr("显示设置界面")
            iconSource: "qrc:/image/menus.svg"
            onTriggered: {
                if(checkForbiden()) return
                sigAction(KtAlarmClock.ActionMainDlg)
            }
        }

        MenuSeparator{}
        MenuItem {
            text: qsTr("在线帮助")
            onTriggered: {
                if(checkForbiden()) return
                Qt.openUrlExternally("https://gitee.com/kuntaisoft/KtAlarmClock/wikis/Home")
            }
        }

        MenuItem {
            text: qsTr("访问锟钛网站")
            onTriggered: {
                if(checkForbiden()) return
                Qt.openUrlExternally("http://www.kuntaisoft.cn")
            }
        }

        MenuSeparator{}
        MenuItem {
            text: qsTr("退出")
            iconSource: "qrc:/image/cancel.svg"
            onTriggered:{
                if(checkForbiden()) return
                sigAction(KtAlarmClock.ActionClose)
            }
        }
    }
    
    onActivated: {}
    onMessageClicked: {}
    Component.onCompleted:{
    }

    function checkForbiden(){
        if(KtAlarmTheme.debug ===1) return false
        return (KtAlarmTheme.workStep == KtAlarmClock.WorkBreak)
    }
}
