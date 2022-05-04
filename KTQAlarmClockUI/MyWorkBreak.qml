import QtQuick 2.4
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id: root
    width: 500
    height: 400
    opacity: 1
    visible: false

    onVisibleChanged: {
        //console.log("MyWorkBreak.onVisibleChanged() visible = " + visible)
        myAlarmClockParam.sigDialogVisibleChange(2, visible);
    }

    MyWorkBreakForm {
        anchors.fill: parent

        button.onClicked: {
            myAlarmClockParam.sigDialogShow(1, true);
            root.hide()
        }
    }

    function showWindow(){
        this.visibility = "Maximized"
        this.flags =Qt.FramelessWindowHint
        this.show()
    }

}

