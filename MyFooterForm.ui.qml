import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.12

Item {
    width: 400
    height: 200
    property alias buttonStop: buttonStop
    property alias buttonPause: buttonPause
    property alias buttonStart: buttonStart

    Rectangle {
        id: rectangle
        radius: 15
        width: 400
        color: "#7ec687"

        anchors.fill: parent

        RowLayout {
            id: rowLayout
            width: 80
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            ToolButton {
                id: buttonStart
                text: qsTr("Start")
                height: 48
                iconSource: "image/ic_start.svg"
            }

            ToolButton {
                id: buttonPause
                text: qsTr("Pause")
                height: 48
                iconSource: "image/ic_pause.svg"
            }

            ToolButton {
                id: buttonStop
                text: qsTr("Stop")
                height: 48
                iconSource: "image/ic_stop.svg"
            }
        }
    }
}

/*##^##
Designer {
    D{i:2;anchors_height:400;anchors_width:200;anchors_x:8;anchors_y:16}
}
##^##*/

