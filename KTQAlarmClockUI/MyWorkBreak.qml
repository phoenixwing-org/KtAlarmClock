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

    onVisibleChanged: {
        //console.log("MyWorkBreak.onVisibleChanged() visible = " + visible)
        myAlarmClockParam.sigDialogVisibleChange(2, visible);
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            fullScreen = !fullScreen
        }
    }

    MyWorkBreakForm {
        id: myWorkBreakForm
        anchors.fill: parent

        button.onClicked: {
            myAlarmClockParam.sigDialogShow(1, true);
            root.hide()
        }
    }

    function showWindow(){
        //this.visibility = "Maximized"
        //this.flags =Qt.FramelessWindowHint
        this.show()
    }

}

