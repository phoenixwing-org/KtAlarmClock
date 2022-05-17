import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12

Item {
    width: 400
    height: 100
    property alias buttonStop: buttonStop
    property alias buttonPause: buttonPause
    property alias buttonStart: buttonStart

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
            width: 48
            height: 48
            sourceSize.height: 48
            sourceSize.width: 48
            source: "qrc:/image/kt-blue.svg"

            KtMouseAreaMove{}
        }

        RowLayout {
            id: rowLayout
            width: 80
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            KtToolButton {
                id: buttonStart
                text: qsTr("Start")
                height: 48
                icon.source: "image/ic_start.svg"
                icon.height: 48
                icon.width: 48
            }

            KtToolButton {
                id: buttonPause
                text: qsTr("Pause")
                height: 48
                visible: false
                icon.source: "image/ic_pause.svg"
                icon.height: 48
                icon.width: 48
            }

            KtToolButton {
                id: buttonStop
                text: qsTr("Stop")
                height: 48
                icon.source: "image/ic_stop.svg"
                icon.height: 48
                icon.width: 48
            }
        }
    }
}



