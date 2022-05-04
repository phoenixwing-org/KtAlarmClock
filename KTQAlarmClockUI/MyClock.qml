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
        running: false
        repeat: true
        onTriggered:{
            counter ++
            label.text = "00:0" + counter;
            if(counter >= 5){
                onClockState(2)
                running = false;
            }

        }
    }

    // on state change
    function onClockState(state){
        switch(state) {
        case 1:
            counter = 0;
            myTimer.running = true;
            break;
        case 2:
            myTimer.running = false;
            myAlarmClockParam.sigDialogShow(2, true);
            break;
        default:

        }
    }

    // connect signal onCompleted
    Component.onCompleted: {
        myAlarmClockParam.sigClockState.connect(onClockState)
    }

}
