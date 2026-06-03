import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0

Item{
    width: 80
    height: 30
    property int counter: 0
    property int state: KtAlarmClock.None
    property double phaseStartMs: 0
    property int phaseDurationSec: 0
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
            onTriggered: syncFromWallClock()
        }
    }

    function syncFromWallClock() {
        if (!myTimer.running)
            return
        if (state === KtAlarmClock.None || phaseDurationSec <= 0)
            return

        var elapsed = Math.floor((Date.now() - phaseStartMs) / 1000)
        var remaining = phaseDurationSec - elapsed
        if (remaining <= 0) {
            counter = 0
            showTime()
            clockOut()
            return
        }

        counter = remaining
        showTime()
    }

    function clockStart(iState){
        state = iState;
        if(iState === KtAlarmClock.None){
            myTimer.running = false;
            phaseDurationSec = 0
            onClockOut(KtAlarmClock.None)
            return
        }

        phaseDurationSec = counter
        phaseStartMs = Date.now()
        syncFromWallClock()
        myTimer.running = true
    }

    function clockOut(){
        myTimer.running = false;
        onClockOut(state)
    }

    function clockPause(){
        if (myTimer.running) {
            syncFromWallClock()
            phaseDurationSec = counter
        }
        myTimer.running = false
    }

    function showTime(){
        if(counter <= 0){
            label.text = "0:00";
            return
        }
        var m = Math.floor(counter / 60)
        var s = ("00" + counter % 60).slice(-2)
        label.text = m + ":" + s;
    }
}
