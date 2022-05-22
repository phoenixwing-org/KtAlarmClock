import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0

Window {
    id: root
    width: clock.width
    height: clock.height
    property alias myClockTimeMax: clock.timeMax
    property alias myClockVisible: clock.visible
    property alias clock: clock

    property var target: null

    property bool canClose: true
    color: "transparent"
    visible:  true 
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    MyClock{
        id: clock
        visible: true
        x: 0
        y: 0
        KtMouseAreaMove{
            id:clockMove
            targetFill: parent
            targetMove: root

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            // 该属性设置为false的话，则鼠标的进入 离开 移动不能捕获到
            hoverEnabled: false
            onClicked: {
                if(mouse.button===Qt.RightButton){
                    let x1 = root.x - menuTest.width + root.width
                    menuTest.x = x1
                    menuTest.y = root.y + root.height+5
                    menuTest.show()
                }
            }

        }
    }

    MyWindowMenu{
        id:menuTest
        visible: false
        onSigAction:{
            myAlarmClockParam.sigAction(index)
        }
    }

    //@disable-check M16
    onClosing: function(closeEvent){
        closeEvent.accepted = canClose
        if(!canClose){
            console.log("MyClockWindow.closeEvent.accepted = false")
        }
    }
}
