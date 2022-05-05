import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.12
// import QtQuick.Dialogs 1.2 // for pop dialog

Window {
    id: root
    x: myMain.x
    y: 0
    width: myClock.width
    height: myClock.height

    visible: true
    color: "transparent"
    opacity: 1
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    MyClock{
        id: myClock
        timeMax: 4
        visible: true
        x: 0
        y: 0
    }

    MyWorkBreak{
        id:myWorkBreak
    }

    MyMain{
        id:myMain
        x:Screen.width - width - 100
        y:100
    }

    onVisibleChanged: {
        onChangeSubVisible(0,visible)
    }

    // connect signal onCompleted
    Component.onCompleted: {
        // signal Visible Change
        myAlarmClockParam.sigDialogVisibleChange.connect(onChangeSubVisible)
        // signal Show sub dialog
        myAlarmClockParam.sigDialogShow.connect(onShowDialog)
        myAlarmClockParam.sigClockStart.connect(onClockStart)
        
        myAlarmClockParam.sigClockOut.connect(onClockTimeout);

        myClock.sigClockOut.connect(onClockTimeout)
    }

    function onClockStart(state){
        if (1 === state){
            myClock.timeMax = myAlarmClockParam.WorkTime;
            myMain.hide();
            myWorkBreak.hide();
            myClock.onClockStart(state);
        } else if (3 === state){
            myWorkBreak.showWindow();
            myMain.show();
        }
    }

    /**
     * @brief Show sub dialog
     */
    function onShowDialog(index, value){
        console.log("onShowDialog(" + index + "," + value +")")
        switch(index) {
        case 1:
            if(value){
                myMain.show();
            }
            else{
                myMain.hide();
            }
            break;
        case 2:
            if(value){
                myWorkBreak.showWindow();
            }
            else{
                myWorkBreak.hide();
            }
            break;
        default:

        }
    }

    // on state change
    function onClockTimeout(state){
        console.log("onClockTimeout("+state+")")
        switch(state) {
        case 1:
            myAlarmClockParam.sigDialogShow(2, true);
            break;
        case 3:
            myAlarmClockParam.sigDialogShow(1, true);
            break;
        case 4:
        }
    }

    function onChangeSubVisible(index, value){
        console.log("onChangeSubVisible(" + index + "," + value +")")
        var count = 0;
        if(myWorkBreak.visible) count++
        if(myMain.visible) count++

        // if(myClock.visible) count++

        console.log("count = " + count)
        //if(count < 1) root.close()
    }
}
