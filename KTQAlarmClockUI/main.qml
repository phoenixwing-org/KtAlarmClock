import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.12
// import QtQuick.Dialogs 1.2 // for pop dialog

Window {
    id: root
    visible: true
    x: myClock.x
    y: 0
    width: 10
    height: 10
    color: "transparent"
    opacity: 1
    flags: Qt.FramelessWindowHint

    property var myWorkBreak: null
    property var myMain: null

    MyClock{
        id: myClock
        visible: true
        x: myMain.x + myMain.width - width
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

    function onChangeSubVisible(index, value){
        //console.log("onChangeSubVisible(" + index + "," + value +")")
        var count = 0;
        if(myWorkBreak.visible) count++
        if(myMain.visible) count++

        // if(myClock.visible) count++

        //console.log("count = " + count)
        //if(count === 0) root.close()
    }

}


