import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0

Item{
    width: 80
    height: 30
    property int counter: 0
    property int state: KtAlarmClock.None
    readonly property alias running: myTimer.running

    signal onClockOut(int state)

    Rectangle{
        id: rectangle
        color: "#222222"
        radius: 15
        anchors.fill: parent

        Label{
            id:label
            color: KtAlarmTheme.colorIconLight
            text: "00:00"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left
            anchors.leftMargin: 5
            font.pointSize: KtAlarmTheme.fontPixelNormal
            anchors.verticalCenter: parent.verticalCenter
        }

        Timer {
            id: myTimer
            interval: 1000
            running: false
            repeat: true
            onTriggered:{
                if(counter <= 0) clockOut();
                else counter --
                showTime();
            }
        }
    }

    function clockStart(iState){
        state = iState;
        if(iState === KtAlarmClock.None){
            myTimer.running = false;
            onClockOut(KtAlarmClock.None)
            return;
        }

        showTime();
        myTimer.running = true;
    }

    function clockOut(){
        myTimer.running = false;
        onClockOut(state)
    }

    function clockPause(){
        myTimer.running = false;
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
