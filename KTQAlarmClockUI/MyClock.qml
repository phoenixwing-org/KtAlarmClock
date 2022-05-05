import QtQuick 2.0
import QtQuick.Window 2.14
import QtQuick.Controls 2.12

Item{
    width: 150
    height: 50
    property int counter: 0
    property int timeMax: 3600
    property int state: 0

    signal sigClockOut(int state)

    Rectangle{
        id: rectangle
        color: "Black"
        radius: 10
        anchors.fill: parent

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
                if(counter <= 0) onClockOut();
                counter --
                showTime();
            }
        }
    }

    function onClockStart(iState){
        counter = timeMax;
        state = iState;
        showTime();
        myTimer.running = true;
    }

    function onClockOut(){
        myTimer.running = false;
        sigClockOut(state)
    }

    function showTime(){
        if(counter <= 0){
            label.text = "0:00";
            return
        }
        var m = Math.floor(counter / 60)
        var s = ("00" +counter % 60).slice(-2)
        label.text = m + ":" + s;
    }
}
