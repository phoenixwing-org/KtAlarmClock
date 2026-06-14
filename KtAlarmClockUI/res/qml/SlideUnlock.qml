import QtQuick

Item {
    id: root

    signal accepted()

    property string text: qsTr("滑动解锁")
    property string completedText: qsTr("松开继续")
    property bool enabled: true
    property real progress: 0

    implicitWidth: 236
    implicitHeight: 48

    function finish() {
        if (!root.enabled)
            return
        root.progress = 1
        root.accepted()
        resetTimer.restart()
    }

    function reset() {
        resetTimer.stop()
        root.progress = 0
    }

    Timer {
        id: resetTimer
        interval: 450
        onTriggered: root.progress = 0
    }

    Behavior on progress {
        NumberAnimation {
            duration: 180
            easing.type: Easing.OutCubic
        }
    }

    Rectangle {
        id: track

        anchors.fill: parent
        radius: height / 2
        color: "#eef2f7"
        border.color: "#d8e1ea"
        border.width: 1
        opacity: root.enabled ? 1.0 : 0.55

        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: handle.x + handle.width
            radius: height / 2
            color: "#d0ebff"
            opacity: 0.8
        }

        Text {
            anchors.centerIn: parent
            text: root.progress > 0.82 ? root.completedText : root.text
            color: "#475467"
            font.pixelSize: 13
            font.weight: Font.DemiBold
            opacity: 1.0 - Math.min(root.progress, 0.75)
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle {
            id: handle

            width: 40
            height: 40
            radius: 20
            x: 4 + root.progress * (track.width - width - 8)
            y: 4
            color: "#228be6"
            border.color: "#1c7ed6"
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "\u203a"
                color: "#ffffff"
                font.pixelSize: 26
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            MouseArea {
                id: dragArea

                anchors.fill: parent
                enabled: root.enabled
                cursorShape: Qt.PointingHandCursor

                property real startX: 0
                property real startProgress: 0

                onPressed: function(mouse) {
                    startX = mouse.x
                    startProgress = root.progress
                }

                onPositionChanged: function(mouse) {
                    if (!(mouse.buttons & Qt.LeftButton))
                        return
                    const travel = track.width - handle.width - 8
                    root.progress = Math.max(0, Math.min(1, startProgress + (mouse.x - startX) / travel))
                }

                onReleased: {
                    if (root.progress >= 0.88)
                        root.finish()
                    else
                        root.progress = 0
                }
            }
        }
    }
}
