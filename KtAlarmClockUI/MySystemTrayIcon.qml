import QtQuick 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0
import Qt.labs.platform 1.1

SystemTrayIcon {
    id:trayIcon
    visible: true
    icon.source: "qrc:/image/kt.svg"

    signal sigAction(int index)

    menu: Menu {
        MenuItem {
            text: qsTr("Have a rest right now")
            iconSource: "qrc:/image/coffee.svg"
            onTriggered: sigAction(Kt.ActionBreak)
        }
        MenuItem {
            text: qsTr("Setting")
            iconSource: "qrc:/image/menus.svg"
            onTriggered: {
                sigAction(Kt.ActionMainDlg)
            }
        }

        MenuSeparator{}
        MenuItem {
            text: qsTr("Help")
            iconSource: "qrc:/image/kt.svg"
            onTriggered: {
                trayIcon.showMessage("锟钛闹钟", "可以定时休息、工作\n"+
                                     "http://www.kuntaisoft.cn")
            }
        }

        MenuSeparator{}
        MenuItem {
            text: qsTr("Quit Kt Clock")
            iconSource: "qrc:/image/cancel.svg"
            onTriggered: sigAction(Kt.ActionClose)
        }
    }
    
    onActivated: {}
    onMessageClicked: console.log("Message clicked")
    Component.onCompleted:{
    }
}
