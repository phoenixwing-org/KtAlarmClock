import QtQuick
import QtQuick.Window

Window {
    id: root

    width: bubble.implicitWidth
    height: bubble.implicitHeight
    minimumWidth: width
    minimumHeight: height
    maximumWidth: width
    maximumHeight: height
    property bool positioned: false
    x: 0
    y: 0
    visible: positioned && !alarmModel.lockVisible
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool

    Component.onCompleted: {
        const next = alarmController.initialWindowPosition(
            Math.round(root.width),
            Math.round(root.height)
        )
        root.x = next.x
        root.y = next.y
        root.positioned = true
    }

    ClockBubble {
        id: bubble

        anchors.fill: parent
        windowTarget: root
        onContextMenuRequested: function(globalX, globalY) {
            contextMenuWindow.openAt(globalX, globalY)
        }
    }

    Window {
        id: contextMenuWindow

        width: contextMenuPanel.implicitWidth
        height: contextMenuPanel.implicitHeight
        minimumWidth: width
        minimumHeight: height
        maximumWidth: width
        maximumHeight: height
        visible: false
        color: "transparent"
        flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
        transientParent: root

        function openAt(globalX, globalY) {
            width = contextMenuPanel.implicitWidth
            height = contextMenuPanel.implicitHeight
            const next = alarmController.clampWindowPosition(
                globalX,
                globalY,
                Math.round(width),
                Math.round(height)
            )
            x = next.x
            y = next.y
            visible = true
            raise()
            requestActivate()
        }

        onActiveChanged: {
            if (visible && !active)
                visible = false
        }

        BubbleContextMenu {
            id: contextMenuPanel

            anchors.fill: parent
            onRequestClose: contextMenuWindow.visible = false
        }
    }

    Window {
        id: settingsWindow

        width: 320
        height: 286
        minimumWidth: width
        minimumHeight: height
        maximumWidth: width
        maximumHeight: height
        title: "Settings"
        visible: alarmModel.settingsVisible && !alarmModel.lockVisible
        color: "transparent"
        flags: Qt.Tool | Qt.WindowStaysOnTopHint | Qt.FramelessWindowHint
        transientParent: root

        onVisibleChanged: {
            if (!visible)
                return
            const next = alarmController.clampWindowPosition(
                Math.round(root.x),
                Math.round(root.y + root.height + 8),
                Math.round(width),
                Math.round(height)
            )
            x = next.x
            y = next.y
            raise()
            requestActivate()
        }

        SettingsPanel {
            anchors.fill: parent
            visible: settingsWindow.visible
            windowTarget: settingsWindow
        }

        onClosing: function(close) {
            close.accepted = false
            alarmController.closeSettings()
        }
    }
}
