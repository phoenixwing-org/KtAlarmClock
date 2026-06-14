import QtQuick

Rectangle {
    id: root

    signal requestClose()

    implicitWidth: 188
    implicitHeight: menuColumn.implicitHeight + 14

    radius: 10
    color: "#ffffff"
    border.color: "#dee2e6"
    border.width: 1

    FontLoader {
        id: codiconFont

        source: "qrc:/font/codicon-font-v0.0.37-1.ttf"
    }

    component MenuItem: Rectangle {
        id: item

        signal triggered()

        property bool destructive: false
        property bool enabled: true
        property string iconText: ""
        property string text: ""

        width: menuColumn.width
        height: 32
        radius: 7
        color: item.enabled && rowMouse.containsMouse ? "#f1f3f5" : "transparent"
        opacity: item.enabled ? 1 : 0.42

        Row {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 9

            Text {
                text: item.iconText
                color: item.destructive ? "#c92a2a" : "#526070"
                font.family: codiconFont.name
                font.pixelSize: 14
                width: 17
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                text: item.text
                color: item.destructive ? "#c92a2a" : "#253041"
                font.pixelSize: 12
                font.weight: Font.Medium
                width: parent.width - 26
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }
        }

        MouseArea {
            id: rowMouse

            anchors.fill: parent
            hoverEnabled: true
            cursorShape: item.enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
            onClicked: {
                if (!item.enabled)
                    return
                root.requestClose()
                item.triggered()
            }
        }
    }

    component Separator: Item {
        width: menuColumn.width
        height: 9

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            height: 1
            color: "#edf2f7"
        }
    }

    Column {
        id: menuColumn

        anchors.fill: parent
        anchors.margins: 7
        spacing: 2

        MenuItem {
            enabled: !alarmModel.forbidden
            iconText: alarmModel.running ? "\uead1" : "\ueb2c"
            text: alarmModel.running ? qsTr("暂停") : qsTr("开始")
            onTriggered: alarmController.playPause()
        }

        MenuItem {
            enabled: !alarmModel.forbidden
            iconText: "\uec15"
            text: qsTr("立刻休息")
            onTriggered: alarmController.startBreak()
        }

        MenuItem {
            enabled: !alarmModel.forbidden
            iconText: "\uea77"
            text: qsTr("下一轮")
            onTriggered: alarmController.nextLoop()
        }

        Separator {}

        MenuItem {
            enabled: !alarmModel.forbidden
            iconText: "\ueb8f"
            text: qsTr("后退 1 分钟")
            onTriggered: alarmController.adjustRemaining(60)
        }

        MenuItem {
            enabled: !alarmModel.forbidden
            iconText: "\uea9c"
            text: qsTr("前进 1 分钟")
            onTriggered: alarmController.adjustRemaining(-60)
        }

        Separator {}

        MenuItem {
            enabled: !alarmModel.forbidden
            iconText: "\ueb52"
            text: qsTr("设置")
            onTriggered: alarmController.openSettings()
        }

        MenuItem {
            destructive: true
            iconText: "\uea76"
            text: qsTr("退出")
            onTriggered: alarmController.quit()
        }
    }
}
