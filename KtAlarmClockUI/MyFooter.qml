import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import KtAlarmClock 1.0

Item {
    id: root
    width: 400
    height: 60
    property alias buttonStart: buttonStart
    property alias buttonBreak: buttonBreak
    property int buttonSize: 48
    property int iconSize: 36

    Rectangle {
        id: rectangle
        radius: 0
        width: 400
        height: 100
        color: "#202020"

        anchors.fill: parent

        RowLayout {
            id: rowLayout
            height: buttonSize
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            KtToolButton {
                id: buttonBreak
                text: qsTr("Rest")
                icon.source: "qrc:/image/coffee.svg"
                icon.width: iconSize
                icon.height: iconSize
                action: actionBreak
            }

            KtToolButton {
                id: buttonBackward
                text: qsTr("play / pause")
                onlyIcon: true
                icon.source: "qrc:/image/kt-backward.svg"
                icon.width: iconSize
                icon.height: iconSize
                action: ActionBackward
                onClicked: KtAlarmTheme.sigAction(KtAlarmClock.ActionBackward)
            }

            KtToolButton {
                id: buttonStart
                text: qsTr("play / pause")
                onlyIcon: true
                icon.source: KtAlarmTheme.loop ? "qrc:/image/pause.svg" : "qrc:/image/play.svg"
                icon.width: iconSize
                icon.height: iconSize
                action: actionPlayPause
            }
            KtToolButton {
                id: buttonForward
                text: qsTr("play / pause")
                onlyIcon: true
                icon.source: "qrc:/image/kt-forward.svg"
                icon.width: iconSize
                icon.height: iconSize
                onClicked: KtAlarmTheme.sigAction(KtAlarmClock.ActionForward)
            }

            KtToolButton {
                id: buttonReset
                onlyIcon: true
                icon.source: "qrc:/image/next.svg"
                icon.width: iconSize
                icon.height: iconSize
                action: actionNextLoop
            }

        }
    }

    Action {
        id: actionPlayPause
        text: "Pay / Pause"
        onTriggered:  KtAlarmTheme.sigAction(KtAlarmClock.ActionPlayPause)
    }

    Action {
        id: actionBreak
        text: "Break"
        onTriggered:  KtAlarmTheme.sigAction(KtAlarmClock.ActionBreak)
    }


    Action {
        id: actionNextLoop
        text: "Next Loop"
        onTriggered:  KtAlarmTheme.sigAction(KtAlarmClock.ActionNextLoop)
    }

}

