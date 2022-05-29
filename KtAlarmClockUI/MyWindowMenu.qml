import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.0
import KtAlarmClock 1.0

Window {
    id: root
    x:0
    y:0
    width: 200
    height: layout.height+5
    flags: Qt.Window | Qt.FramelessWindowHint
    color: "black"

    signal sigAction(int index)

    ColumnLayout {
        id: layout
        x: 0
        y: 0
        width: parent.width

        KtToolButton {
            id: buttonBreak
            Layout.preferredHeight: 40
            visible: true
            icon.source: "qrc:/image/bike.svg"
            text: "立刻休息"
            transformOrigin: Item.Left
            display: Qt.ToolButtonTextBesideIcon
            onClicked: {
                sigAction(KtAlarmClock.ActionBreak)
                root.hide()
            }
        }

        KtToolButton {
            id: buttonSetting
            Layout.preferredHeight: 40
            visible: true
            text: "显示设置界面"
            transformOrigin: Item.Left
            icon.source: "qrc:/image/grid.svg"
            display: Qt.ToolButtonTextBesideIcon
            onClicked: {
                sigAction(KtAlarmClock.ActionMainDlg)
                root.hide()
            }
        }
        KtToolButton {
            Layout.preferredHeight: 40
            visible: true
            text: "在线帮助"
            transformOrigin: Item.Left
            icon.source: "qrc:/image/alarm-clock.svg"
            display: Qt.ToolButtonTextBesideIcon
            onClicked: {
                root.hide()
                Qt.openUrlExternally("https://gitee.com/kuntaisoft/KtAlarmClock/wikis/Home")
            }
        }
        KtToolButton {
            Layout.preferredHeight: 40
            visible: true
            text: "访问锟钛网站"
            transformOrigin: Item.Left
            icon.source: "qrc:/image/kt.svg"
            display: Qt.ToolButtonTextBesideIcon
            onClicked: {
                root.hide()
                Qt.openUrlExternally("http://www.kuntaisoft.cn")
            }
        }
    }
    onActiveFocusItemChanged: {
        if (!activeFocusItem) {
            root.visible = false
        }
    }

}
