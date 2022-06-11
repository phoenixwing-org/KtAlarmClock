import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0
import Qt.labs.platform 1.1
/**
 * @brief root window
 */
Window {
    id: root
    x: mainDlg.x + mainDlg.width - clock.width - 5
    y: 2

    width: clock.width
    height: clock.height
    property bool canClose: false

    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    // control move
    KtMouseAreaMove{
        targetFill: root.contentItem
        targetMove: root
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        // 该属性设置为false的话，则鼠标的进入 离开 移动不能捕获到
        hoverEnabled: false
        onClicked: {
            if(mouse.button===Qt.RightButton) popMenu.show()
        }
    }
    
    // clock item
    MyClock{
        id: clock
        counter: -100
    }

    // over window
    MyOver0{
        id:over
    }

    // main window
    MyMain{
        id:mainDlg
    }

    // main pop menu
    MyWindowMenu{
        id: popMenu
        x: root.x - width + root.width
        y: root.y + root.height
    }

    // tray icon
    MySystemTrayIcon {
        id:trayIcon
    }

    Connections {
        target: KtAlarmTheme
        onWorkStepChanged: afterWorkStepChanged
    }

    //@disable-check M16
    onClosing: function(closeEvent){
        closeEvent.accepted = root.canClose //accept the close
        //if(!canClose)
        //    console.log("main.closeEvent.accepted = false")
    }

    // connect signal onCompleted
    Component.onCompleted: {
        //console.log("main.onCompleted()")
        if(KtAlarmTheme.debug){
            myAlarmClockParam.WorkBreak = 10
            myAlarmClockParam.TimeForce = 2
            myAlarmClockParam.WorkTime = 15
        }

        //root.show();
        let x = Math.floor(Screen.width - mainDlg.width - 100)
        mainDlg.x = x;

        // signal Show sub dialog
        
        // signal for clock
        clock.onClockOut.connect(clockTimeout)
        over.onClockOut.connect(clockTimeout)

        // for Action
        trayIcon.onAction.connect(runCommand)
        popMenu.onAction.connect(runCommand)
        KtAlarmTheme.onAction.connect(runCommand);
        myAlarmClockParam.onAction.connect(runCommand)

        // signal for param and cmd
        myAlarmClockCmd.setAutoStart(true)
        
        //console.log("main.onCompleted()-end")
    }

    function afterWorkStepChanged(){

        console.log("main:afterWorkStepChanged(), workStep=",KtAlarmTheme.workStep)
        let showClock = false
        let showOver = false

        if( KtAlarmTheme.workStep == KtAlarmClock.WorkTime){
            showClock = true
        }
        else if( KtAlarmTheme.workStep == KtAlarmClock.WorkBreak){
            showOver = true
        }

        if(clock.visible !== showClock) {
            console.log("change clock.visible to ", showClock)
            clock.visible = showClock
        }
        if(over.visible !== showOver) {
            console.log("change over.visible  to ", showOver)
            over.visible = showOver
            if(!showOver){
                over.customHide();
            }
        }
    }

    function closeAllWindows(){

        KtAlarmTheme.workStep = KtAlarmClock.None

        // can close
        root.canClose = true;
        over.canClose = true;

        //hide
        trayIcon.hide()
        mainDlg.close()
        root.close()        
    }

    function clockStart(state){
        KtAlarmTheme.workStep = state
        //myAlarmClockParam.dump() // dump

        switch(state){
        case KtAlarmClock.WorkBreak:
            clock.clockPause()
            over.timeMax= myAlarmClockParam.WorkBreak;
            over.timeForce= myAlarmClockParam.TimeForce;
            over.customShow();
            return
        case KtAlarmClock.WorkTime:
            clock.timeMax = clock.counter;
            clock.clockStart(state);
            break;
        default:
            clock.clockPause()
        }
    }

    // command after clock time out
    function clockTimeout(state){
        // on state change
        // console.log("clockTimeout("+state+")")
        switch(state) {
        case KtAlarmClock.WorkTime:
            myAlarmClockParam.onUpdateInfos() // get infos
            clockStart(KtAlarmClock.WorkBreak) // break
            break;
        case KtAlarmClock.WorkBreak:
            myAlarmClockParam.onUpdateInfos() // get infos

            clock.counter = myAlarmClockParam.WorkTime; // reset
            clockStart(KtAlarmClock.WorkTime) // work time
            break;
        default:
            clockStart(KtAlarmClock.None) // None
            break;
        }
    }

    /**
     * @brief Action signal treatment
     */
    function runCommand(index){
        console.log("main.runCommand(" + index + ")")
        switch(index) {
        case KtAlarmClock.ActionPlayPause:
            if(clock.counter <= -100){
                // first time to paly
                KtAlarmTheme.loop = true

                myAlarmClockParam.onUpdateInfos() // get infos
                clock.counter = myAlarmClockParam.WorkTime; // reset
            } else{
                // change state
                KtAlarmTheme.loop = !(KtAlarmTheme.loop)
            }

            console.log("To : loop = " + KtAlarmTheme.loop + ", counter = ", clock.counter)
            if(KtAlarmTheme.loop){
                clockStart(KtAlarmClock.WorkTime);
            }
            else{
                // change to pause
                clock.clockPause()
            }

            break;
        case KtAlarmClock.ActionBreak:
            myAlarmClockParam.onUpdateInfos() // get infos
            KtAlarmTheme.loop = true
            clockStart(KtAlarmClock.WorkBreak)
            break;
        case KtAlarmClock.ActionNextLoop:
            KtAlarmTheme.loop = true
            
            myAlarmClockParam.onUpdateInfos() // get infos
            clock.counter = myAlarmClockParam.WorkTime; // reset
            clockStart(KtAlarmClock.WorkTime);
            break;
        case KtAlarmClock.ActionForward:
            KtAlarmTheme.loop = true
            clock.counter -= 60; // 60s
            clockStart(KtAlarmClock.WorkTime);
            break;
        case KtAlarmClock.ActionBackward:
            KtAlarmTheme.loop = true
            clock.counter += 60; // 60s
            clockStart(KtAlarmClock.WorkTime);
            break;
        case KtAlarmClock.ActionMainDlg:
            mainDlg.show()
            break;
        case KtAlarmClock.ActionClose:
            closeAllWindows()
            break;
        case KtAlarmClock.ActionHelp:
            break;
        case KtAlarmClock.ActionKtWeb:
            break;
        default:
        }
    }
}
