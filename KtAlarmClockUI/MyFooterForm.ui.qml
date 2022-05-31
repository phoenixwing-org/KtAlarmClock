import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.14

Item {
    width: 400
    height: 100
    property alias buttonStop: buttonStop
    property alias buttonPause: buttonPause
    property alias buttonStart: buttonStart
    property alias buttonRest: buttonRest
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
            source: "qrc:/image/kt-blue.svg"

            KtMouseAreaMove {}
        }

        RowLayout {
            id: rowLayout
            height: buttonSize
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            KtToolButton {
                id: buttonRest
                text: qsTr("Rest")
                Layout.preferredHeight: buttonSize
                Layout.preferredWidth: buttonSize
                Image {
                    width: iconSize
                    height: iconSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image/coffee.svg"
                    ColorOverlay {
                        anchors.fill: parent
                        source: parent
                        color: KtAlarmTheme.colorIcon
                    }
                }
            }

            KtToolButton {
                id: buttonStart
                text: qsTr("Start")
                Layout.preferredHeight: buttonSize
                Layout.preferredWidth: buttonSize

                Image {
                    width: iconSize
                    height: iconSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image/ic_start.svg"
                    ColorOverlay {
                        anchors.fill: parent
                        source: parent
                        color: KtAlarmTheme.colorIcon
                    }
                }
            }

            KtToolButton {
                id: buttonPause
                text: qsTr("Pause")
                Layout.preferredHeight: buttonSize
                Layout.preferredWidth: buttonSize
                visible: false

                Image {
                    width: iconSize
                    height: iconSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image/ic_pause.svg"
                    ColorOverlay {
                        anchors.fill: parent
                        source: parent
                        color: KtAlarmTheme.colorIcon
                    }
                }
            }

            KtToolButton {
                id: buttonStop
                text: qsTr("Stop")
                Layout.preferredHeight: buttonSize
                Layout.preferredWidth: buttonSize
                Image {

                    width: iconSize
                    height: iconSize
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "image/ic_stop.svg"
                    ColorOverlay {
                        anchors.fill: parent
                        source: parent
                        color: KtAlarmTheme.colorIcon
                    }
                }
            }
        }
    }
}
