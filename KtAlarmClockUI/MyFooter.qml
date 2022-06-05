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

        Image {
            id: ktLogo
            x: 5
            y: 5
            z: 9998
            width: buttonSize
            height: buttonSize
            sourceSize.height: buttonSize
            sourceSize.width: buttonSize
            source: "qrc:/image/kt.svg"

            KtMouseAreaMove {}
        }

        RowLayout {
            id: rowLayout
            height: buttonSize
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            KtToolButton {
                id: buttonBreak
                text: qsTr("Rest")
                Layout.preferredHeight: buttonSize
                Layout.preferredWidth: buttonSize
                Image {
                    width: iconSize
                    height: iconSize
                    sourceSize.height: iconSize
                    sourceSize.width: iconSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:/image/coffee.svg"
                }

                action: actionBreak
            }

            KtToolButton {
                id: buttonStart
                text: qsTr("Start")
                Layout.preferredHeight: buttonSize
                Layout.preferredWidth: buttonSize

                Image {
                    width: iconSize
                    height: iconSize
                    sourceSize.height: iconSize
                    sourceSize.width: iconSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: KtAlarmTheme.loop ? "qrc:/image/pause.svg" : "qrc:/image/play.svg"
                }
                action: actionPlayPause
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

