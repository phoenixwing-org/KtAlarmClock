import QtQuick 2.0
import QtQuick.Controls 2.12

Rectangle{
    id: rectangle
    width: 160
    height: 35
    color: "Black"
    radius: 10
    anchors.fill: parent

    property int counter: 0
    Label{
        id:label
        color: "#e6e6e6"
        text: "00:00"
        anchors.left: parent.left
        anchors.leftMargin: 5
        font.pointSize: 20
        anchors.verticalCenter: parent.verticalCenter
    }

    Timer {
        id: myTimer
        interval: 1000
        running: true
        repeat: true
        onTriggered:{
            counter ++
            label.text = counter;
        }
    }

}
