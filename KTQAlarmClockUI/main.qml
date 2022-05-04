import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.12

Window {
    id: root
    visible: true
    x:800
    y:50
    width: 150
    height: 100
    color: "transparent"
    opacity: 1
    flags: "CustomizeWindowHint"

    property var myWorkBreak: null
    property var myMain: null

    MyClock{
        id: myClock
    }

    onVisibleChanged: {
        onChangeSubVisible(0,visible)
    }

    // connect signal onCompleted
    Component.onCompleted: {
        var component = Qt.createComponent("MyWorkBreak.qml")
        myWorkBreak = component.createObject(this)
        myWorkBreak.y = 200
        //myWorkBreak.show()

        component = Qt.createComponent("MyMain.qml")
        myMain = component.createObject(this)
        myMain.x = myWorkBreak.x + 500
        myMain.y = 200

        // signal Visible Change
        myAlarmClockParam.sigVisibleChange.connect(onChangeSubVisible)
        // signal Show sub dialog
        myAlarmClockParam.sigShowDialog.connect(onShowDialog)
    }

    /**
     * @brief Show sub dialog
     */
    function onShowDialog(index, value){
        console.log("onShowDialog(" + index + "," + value +")")
        switch(index) {
        case 1:
            myMain.visible = value
            break;
        case 2:
            if(value)
                myWorkBreak.showWindow();
            else
                myWorkBreak.visible = false;
            break;
        default:

        }
    }

    function onChangeSubVisible(index, value){
        console.log("onChangeSubVisible(" + index + "," + value +")")
        var count = 0;
        if(myWorkBreak.visible) count++
        if(myMain.visible) count++
        // if(myClock.visible) count++

        console.log("count = " + count)
        if(count === 0) root.close()
    }

}


