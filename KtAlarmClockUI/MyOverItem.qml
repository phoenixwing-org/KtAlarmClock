import QtQuick 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0

Item {
    id: root
    width: 10
    height: 10
    property int timeForce: 0
    property int timeMax: 3600
    property bool canClose: false

    visible: true

    MyOver0 {
        id: over0
        screenId: 0
        title: qsTr("Over 0 window")
        Item {
            id: clockPosition
            width: 120
            height: 30
            anchors.horizontalCenter: parent.horizontalCenter
        }
        onVisibleChanged: {
            //if over0 hide, hide the second one
            if(!visible) over1.hide();
        }

        onClockRunningChanged: {
            // console.log("MyOver0.onClockRunningChanged = ", clockRunning)
            if(!clockRunning){
                root.canClose = true
                root.customHide()
                myAlarmClockParam.sigClockOut(KtAlarmClock.WorkBreak)
            }
        }
    }

    MyOver1 {
        id: over1
        title: qsTr("Over 1 window")
        screenId: 1
    }

    /*
     * Show Window
     */
    function customHide(){
        root.visible = false
        root.canClose = true;
        over0.canClose = root.canClose;
        over1.canClose = root.canClose;

        over1.hide();
        if(over0.clockRunning){
            over0.stopPage0()
        } else{
            over0.hide()
        }
    }

    /*
     * Show Window
     */
    function customShow(){
        root.visible = true
        visible = true
        over0.screenId = 0
        over1.screenId = 1
        over0.checkoutScreen()
        over1.checkoutScreen()
        //console.log("over1.screenOK = ",over1.screenOK)
        //console.log("over0.screenOK = ",over0.screenOK)

        if(over1.screenOK){
            over1.showOver();
        }

        if(over0.screenOK){
            over0.canClose = false;// cannot close
            over0.counterForce = root.timeForce
            over0.showOver0()

            over0.myClock.timeMax = root.timeMax;
            over0.myClock.actClockStart(KtAlarmClock.WorkBreak);
            return
        }
    }

}

