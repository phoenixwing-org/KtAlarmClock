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
        if(KtAlarmTheme.debug){
            myAlarmClockParam.WorkBreak = 5
            myAlarmClockParam.TimeForce = 1
            myAlarmClockParam.WorkTime = 5
        }

        root.show();
        var x = Math.floor(Screen.width - mainDlg.width - 100)
        mainDlg.x = x;

        // signal Show sub dialog
        myAlarmClockParam.sigDialogShow.connect(onShowDialog)
        myAlarmClockParam.sigClockStart.connect(onClockStart)
        
        myAlarmClockParam.sigClockOut.connect(onClockTimeout);
        myAlarmClockParam.sigAction.connect(onSigAction);

        clock.sigClockOut.connect(onClockTimeout)
        trayIcon.sigAction.connect(onSigAction)

        // start clock:
        onClockStart(Kt.WorkTime);

        myAlarmClockCmd.SetAutoStart(true);
    }

    function closeAllWindows(){

        KtAlarmTheme.clockStep = Kt.None

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
        switch(state){
        case Kt.WorkBreak:
            clock.onClockPause()
            mainDlg.updateInfor()
            overItem.timeMax= myAlarmClockParam.WorkBreak;
            overItem.timeForce= myAlarmClockParam.TimeForce;
            overItem.customShow();
            return
        case Kt.WorkTime:
            //mainDlg.updateInfor()
            clock.timeMax = myAlarmClockParam.WorkTime;
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
        case Kt.WorkTime:
            onClockStart(Kt.WorkBreak) // break
            break;
        case Kt.WorkBreak:
            onClockStart(Kt.WorkTime) // work time
            break;
        default:
            onClockStart(Kt.None) // None
            break;
        }
    }

    /**
     * @brief Show sub dialog
     */
    function onShowDialog(index, value){
        //console.log("onShowDialog(" + index + "," + value +")")
        switch(index) {
        case Kt.DlgMain:
            if(value){
                mainDlg.show();
            }
            else{
                mainDlg.hide();
            }
            break;
        case Kt.DlgBreak:
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
        // console.log("main.onSigAction(" + index + ")")
        if ( KtAlarmTheme.clockStep == Kt.WorkBreak){
            return
        }
        switch(index) {
        case Kt.ActionBreak:
            onClockStart(Kt.WorkBreak)
            break;
        case Kt.ActionMainDlg:
            mainDlg.show()
            mainDlg.raise()
            mainDlg.requestActivate()
            break;
        case Kt.ActionClose:
            closeAllWindows()
            break;
        default:
        }
    }
}
