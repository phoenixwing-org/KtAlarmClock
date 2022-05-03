import QtQuick 2.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

Item {

    width: 400
    height: 300
    property alias button: button

    Rectangle {
        id: rectangle
        color: "black"
        anchors.fill: parent
        opacity: 0.8

        Button {
            id: button
            text: qsTr("Close")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:200;anchors_width:300}
}
##^##*/

