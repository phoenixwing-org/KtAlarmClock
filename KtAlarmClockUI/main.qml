import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0
import Qt.labs.platform 1.1

Window {
    id: root
    x: mainDlg.x + mainDlg.width - clock.width - 5
    y: 2
    width: clock.width
    height: clock.height
    property bool canClose: false

    visible: true
    color: "transparent"
    opacity: 1
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    KtMouseAreaMove{
        targetFill: root.contentItem
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

    MyWindowMenu{
        id:menuTest
        visible: false
        actAction:{
            myAlarmClockParam.sigAction(index)
        }
    }

    MyClock{
        id: clock
        x: 0
        y: 0
        counter: -100
    }

    MyOverItem{
        id:overItem
        visible: false
    }

    MyMain{
        id:mainDlg
        x:100
        y:100
        visible:false

        //@disable-check M16
        onClosing: function(closeEvent){
            closeEvent.accepted = canClose
            if(!canClose)hide()
        }
    }

    MySystemTrayIcon {
        id:trayIcon
    }

    Connections {
        target: KtAlarmTheme
        onWorkStepChanged: actWorkStepChanged
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

        root.show();
        var x = Math.floor(Screen.width - mainDlg.width - 100)
        mainDlg.x = x;

        // signal Show sub dialog
        myAlarmClockParam.sigDialogShow.connect(actShowDialog)
        myAlarmClockParam.sigClockStart.connect(actClockStart)
        
        myAlarmClockParam.sigClockOut.connect(actClockTimeout);
        myAlarmClockParam.sigAction.connect(actAction);
        KtAlarmTheme.sigAction.connect(actAction);

        clock.sigClockOut.connect(actClockTimeout)
        trayIcon.sigAction.connect(actAction)

        myAlarmClockCmd.setAutoStart(true);
        
        //console.log("main.onCompleted()-end")
    }

    function actWorkStepChanged(){

        console.log("main:actWorkStepChanged(), workStep=",KtAlarmTheme.workStep)
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
        if(overItem.visible !== showOver) {
            console.log("change over.visible  to ", showOver)
            overItem.visible = showOver
            if(!showOver){
                overItem.customHide();
            }
        }
    }

    function closeAllWindows(){

        KtAlarmTheme.workStep = KtAlarmClock.None

        // can close
        root.canClose = true;
        overItem.canClose = true;

        //hide
        trayIcon.hide()
        mainDlg.hide()
        root.close()
        
    }

    function actClockStart(state){
        KtAlarmTheme.workStep = state
        //myAlarmClockParam.dump() // dump

        switch(state){
        case KtAlarmClock.WorkBreak:
            clock.onClockPause()
            overItem.timeMax= myAlarmClockParam.WorkBreak;
            overItem.timeForce= myAlarmClockParam.TimeForce;
            overItem.customShow();
            return
        case KtAlarmClock.WorkTime:
            clock.timeMax = clock.counter;
            clock.actClockStart(state);
            break;
        default:
            clock.onClockPause()
        }
    }

    // on state change
    function actClockTimeout(state){
        // console.log("actClockTimeout("+state+")")
        switch(state) {
        case KtAlarmClock.WorkTime:
            myAlarmClockParam.sigUpdateInfos() // get infos
            actClockStart(KtAlarmClock.WorkBreak) // break
            break;
        case KtAlarmClock.WorkBreak:
            myAlarmClockParam.sigUpdateInfos() // get infos

            clock.counter = myAlarmClockParam.WorkTime; // reset
            actClockStart(KtAlarmClock.WorkTime) // work time
            break;
        default:
            actClockStart(KtAlarmClock.None) // None
            break;
        }
    }

    /**
     * @brief Show sub dialog
     */
    function actShowDialog(index, value){
        //console.log("actShowDialog(" + index + "," + value +")")
        switch(index) {
        case KtAlarmClock.DlgMain:
            if(value){
                mainDlg.show();
            }
            else{
                actClockStart()
                mainDlg.hide();
            }
            break;
        case KtAlarmClock.DlgBreak:
            if(value){
                overItem.customShow();
            }
            else{
                overItem.customHide();
            }
            break;
        default:

        }
    }

    /**
     * @brief Action signal treatment
     */
    function actAction(index){
        console.log("main.actAction(" + index + ")")
        switch(index) {
        case KtAlarmClock.ActionPlayPause:
            if(clock.counter <= -100){
                // first time to paly
                KtAlarmTheme.loop = true

                myAlarmClockParam.sigUpdateInfos() // get infos
                clock.counter = myAlarmClockParam.WorkTime; // reset
            } else{
                // change state
                KtAlarmTheme.loop = !(KtAlarmTheme.loop)
            }

            console.log("To : loop = " + KtAlarmTheme.loop + ", counter = ", clock.counter)
            if(KtAlarmTheme.loop){
                actClockStart(KtAlarmClock.WorkTime);
            }
            else{
                // change to pause
                clock.onClockPause()
            }

            break;
        case KtAlarmClock.ActionBreak:
            myAlarmClockParam.sigUpdateInfos() // get infos
            KtAlarmTheme.loop = true
            actClockStart(KtAlarmClock.WorkBreak)
            break;
        case KtAlarmClock.ActionNextLoop:
            KtAlarmTheme.loop = true
            
            myAlarmClockParam.sigUpdateInfos() // get infos
            clock.counter = myAlarmClockParam.WorkTime; // reset
            actClockStart(KtAlarmClock.WorkTime);
            break;
        case KtAlarmClock.ActionForward:
            KtAlarmTheme.loop = true
            clock.counter -= 60; // 60s
            actClockStart(KtAlarmClock.WorkTime);
            break;
        case KtAlarmClock.ActionBackward:
            KtAlarmTheme.loop = true
            clock.counter += 60; // 60s
            actClockStart(KtAlarmClock.WorkTime);
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
