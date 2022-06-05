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
        onSigAction:{
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
        myAlarmClockParam.sigDialogShow.connect(onShowDialog)
        myAlarmClockParam.sigClockStart.connect(onClockStart)
        
        myAlarmClockParam.sigClockOut.connect(onClockTimeout);
        myAlarmClockParam.sigAction.connect(onSigAction);
        KtAlarmTheme.sigAction.connect(onSigAction);

        clock.sigClockOut.connect(onClockTimeout)
        trayIcon.sigAction.connect(onSigAction)

        myAlarmClockCmd.setAutoStart(true);
        
        //console.log("main.onCompleted()-end")
    }

    function closeAllWindows(){

        KtAlarmTheme.clockStep = KtAlarmClock.None

        // can close
        root.canClose = true;
        overItem.canClose = true;

        //hide
        trayIcon.hide()
        overItem.customHide()
        mainDlg.hide()
        root.close()
        
    }

    function onClockStart(state){
        KtAlarmTheme.clockStep = state
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
            clock.onClockStart(state);
            overItem.customHide();
            break;
        default:
            clock.onClockPause()
            overItem.customHide();
        }
    }

    // on state change
    function onClockTimeout(state){
        // console.log("onClockTimeout("+state+")")
        switch(state) {
        case KtAlarmClock.WorkTime:
            myAlarmClockParam.sigUpdateInfos() // get infos
            onClockStart(KtAlarmClock.WorkBreak) // break
            break;
        case KtAlarmClock.WorkBreak:
            myAlarmClockParam.sigUpdateInfos() // get infos

            clock.counter = myAlarmClockParam.WorkTime; // reset
            onClockStart(KtAlarmClock.WorkTime) // work time
            break;
        default:
            onClockStart(KtAlarmClock.None) // None
            break;
        }
    }

    /**
     * @brief Show sub dialog
     */
    function onShowDialog(index, value){
        //console.log("onShowDialog(" + index + "," + value +")")
        switch(index) {
        case KtAlarmClock.DlgMain:
            if(value){
                mainDlg.show();
            }
            else{
                onClockStart()
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
    function onSigAction(index){
        console.log("main.onSigAction(" + index + ")")
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
                onClockStart(KtAlarmClock.WorkTime);
            }
            else{
                // change to pause
                clock.onClockPause()
                overItem.customHide();
            }

            break;
        case KtAlarmClock.ActionBreak:
            myAlarmClockParam.sigUpdateInfos() // get infos
            KtAlarmTheme.loop = true
            onClockStart(KtAlarmClock.WorkBreak)
            break;
        case KtAlarmClock.ActionNextLoop:
            KtAlarmTheme.loop = true
            
            myAlarmClockParam.sigUpdateInfos() // get infos
            clock.counter = myAlarmClockParam.WorkTime; // reset
            onClockStart(KtAlarmClock.WorkTime);
            break;
        case KtAlarmClock.ActionMainDlg:
            mainDlg.show()
            break;
        case KtAlarmClock.ActionClose:
            closeAllWindows()
            break;
        case KtAlarmClock.ActionHelp:
            break;
        case KtAlarmClock.ActionMainDlg:
            break;
        case KtAlarmClock.ActionKtWeb:
            break;
        default:
        }
    }
}
