import QtQuick 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0

Item {
    id: root
    width: 10
    height: 10
    property bool fullScreen: false //true
    property alias clockPosition: clockPosition
    property int timeForce: 0
    property int timeMax: 3600
    property bool canClose: false
    property int debug: 0
    property bool running: value

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
    }

    MyOver1 {
        id: over1
        title: qsTr("Over 1 window")
        screenId: 1
    }

    // connect signal onCompleted
    Component.onCompleted: {
         over0.myClock.sigClockOut.connect(onClockTimeout)
    }

    onRunningChanged: {
        if(root.running){
            customShow()
        } else{
            customHide()
        }
    }

    // on state change
    function onClockTimeout(state){
        root.customHide()
        myAlarmClockParam.sigClockOut(Kt.WorkBreak)
    }
    /*
     * Show Window
     */
    function customHide(){
        root.visible = false
        root.canClose = true;
        over0.canClose = true;

        over0.running = false
        over1.canClose = true;
        over0.hide()
        over1.hide();
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
            over0.running = root.running
            over0.showOver0()

            over0.myClock.timeMax = root.timeMax;
            over0.myClock.onClockStart(Kt.WorkBreak);
            return
        }
    }

}

