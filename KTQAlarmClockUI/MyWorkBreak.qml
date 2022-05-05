import QtQuick 2.4
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id: root
    x:0
    y:0
    width: fullScreen ? Screen.width : 640 // desktopAvailableWidth
    height: fullScreen ? Screen.height : 480 //desktopAvailableHeight
    property bool fullScreen: true

    visible: false
    //fullScreen ? Qt.FramelessWindowHint : Qt.Window
    flags: Qt.FramelessWindowHint

    MyWorkBreakForm {
        id: myWorkBreakForm
        anchors.fill: parent

        button.onClicked: {
            myAlarmClockParam.sigDialogShow(1, true);
            root.hide()
        }
    }

    MyClock {
        id: myClock
        y: 0
        visible: true
        timeMax: 8
        anchors.horizontalCenter: parent.horizontalCenter
    }

    // connect signal onCompleted
    Component.onCompleted: {
        myClock.sigClockOut.connect(onClockTimeout)
    }

    function showWindow(){
        //this.visibility = "Maximized"
        //this.flags =Qt.FramelessWindowHint
        myClock.timeMax = myAlarmClockParam.WorkBreak
        myClock.onClockStart(3) // 3: break
        root.show()
    }

    function onClockTimeout(state){
        root.hide()
        myAlarmClockParam.sigClockOut(state)
    }
    
    onVisibleChanged: {
        //console.log("MyWorkBreak.onVisibleChanged() visible = " + visible)
        myAlarmClockParam.sigDialogVisibleChange(2, visible);
    }

}

