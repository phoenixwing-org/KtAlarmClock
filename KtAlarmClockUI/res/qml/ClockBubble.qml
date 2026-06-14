import QtQuick
import QtQuick.Window

Rectangle {
    id: root

    signal contextMenuRequested(int globalX, int globalY)

    property Window windowTarget
    property real pressX: 0
    property real pressY: 0

    implicitWidth: Math.max(80, timeLabel.implicitWidth + 20)
    implicitHeight: 34

    radius: height / 2
    color: "#222222"

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: function(mouse) {
            if (mouse.button === Qt.RightButton) {
                if (root.windowTarget)
                    root.contextMenuRequested(
                        Math.round(root.windowTarget.x + mouse.x),
                        Math.round(root.windowTarget.y + mouse.y)
                    )
                return
            }
            root.pressX = mouse.x
            root.pressY = mouse.y
        }
        onPositionChanged: function(mouse) {
            if ((mouse.buttons & Qt.LeftButton) && root.windowTarget) {
                const currentCenterX = Math.round(root.windowTarget.x + root.windowTarget.width / 2)
                const currentCenterY = Math.round(root.windowTarget.y + root.windowTarget.height / 2)
                const next = alarmController.clampWindowPositionOnCurrentScreen(
                    Math.round(root.windowTarget.x + mouse.x - root.pressX),
                    Math.round(root.windowTarget.y + mouse.y - root.pressY),
                    Math.round(root.windowTarget.width),
                    Math.round(root.windowTarget.height),
                    currentCenterX,
                    currentCenterY
                )
                root.windowTarget.x = next.x
                root.windowTarget.y = next.y
            }
        }
    }

    Text {
        id: timeLabel

        anchors.centerIn: parent
        text: alarmModel.remainingText
        color: alarmModel.forbidden ? "#ffb454" : "#46aef6"
        font.family: "Segoe UI"
        font.pixelSize: 16
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
