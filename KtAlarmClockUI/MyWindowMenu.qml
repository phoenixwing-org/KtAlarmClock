import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.0
import KtAlarmClock 1.0

Window {
    id: root
    x:0
    y:0
    width: 240
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
            Layout.preferredWidth: 100
            visible: true
            icon.source: "image/bike.svg"
            text: "Have a sports now!"
            transformOrigin: Item.Left
            Layout.fillWidth: true
            display: Qt.ToolButtonTextBesideIcon
            onClicked: {
                sigAction(Kt.ActionBreak)
                root.hide()
            }
        }

        KtToolButton {
            id: buttonSetting
            Layout.preferredHeight: 40
            Layout.preferredWidth: 100
            visible: true
            text: "Open Setting Dialog"
            transformOrigin: Item.Left
            Layout.fillWidth: true
            icon.source: "image/grid.svg"
            display: Qt.ToolButtonTextBesideIcon
            onClicked: {
                sigAction(Kt.ActionMainDlg)
                root.hide()
            }
        }
    }
    onActiveFocusItemChanged: {
        if (!activeFocusItem) {
            root.visible = false
        }
    }

}
