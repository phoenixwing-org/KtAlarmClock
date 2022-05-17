import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import KtAlarmClock 1.0
import Qt.labs.platform 1.1

Window {
    id: root
    x: 10
    y: 10
    width: 50
    height: 50
    property bool canClose: false
    property int debug: 0 // or or 1

    visible: true
    color: "transparent"
    opacity: 1
    flags: Qt.FramelessWindowHint

    MyClockWindow {
        id: clockDlg
        x: mainDlg.x + mainDlg.width - width
        y: 0
        visible: !overItem.visible 
        canClose: root.canClose
        debug: root.debug
        target: mainDlg
    }

    MyOverItem{
        id:overItem
        visible: false
        debug: root.debug
    }

    MyMain{
        id:mainDlg
        x:100
        y:100        
        visible:false
        debug: root.debug

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
        if(!canClose)
            console.log("main.closeEvent.accepted = false")
    }

    // connect signal onCompleted
    Component.onCompleted: {
        if(debug){
            myAlarmClockParam.WorkBreak = 5
            myAlarmClockParam.TimeForce = 1
            myAlarmClockParam.WorkTime = 5
        }

        clockDlg.show();
        var x = Math.floor(Screen.width - mainDlg.width - 100)
        mainDlg.x = x;

        // signal Show sub dialog
        myAlarmClockParam.sigDialogShow.connect(onShowDialog)
        myAlarmClockParam.sigClockStart.connect(onClockStart)
        
        myAlarmClockParam.sigClockOut.connect(onClockTimeout);
        myAlarmClockParam.sigAction.connect(onSigAction);

        clockDlg.clock.sigClockOut.connect(onClockTimeout)
        trayIcon.sigAction.connect(onSigAction)

        // start clock:
        onClockStart(Kt.WorkTime);

        myAlarmClockCmd.SetAutoStart(true);
    }

    function closeAllWindows(){
        // can close
        root.canClose = true;
        overItem.canClose = true;
        clockDlg.canClose = true;

        //hide
        trayIcon.hide()
        overItem.customHide()
        mainDlg.hide()
        clockDlg.hide()
        root.close()
        
    }

    function onClockStart(state){

        switch(state){
        case Kt.WorkBreak:
            clockDlg.clock.onClockPause()
            overItem.timeMax= myAlarmClockParam.WorkBreak;
            overItem.timeForce= myAlarmClockParam.TimeForce;
            overItem.customShow();
            return
        case Kt.WorkTime:
            console.log("overItem.visible", overItem.visible)
            clockDlg.clock.timeMax = myAlarmClockParam.WorkTime;
            clockDlg.clock.onClockStart(state);
            overItem.customHide();
            break;
        default:
            clockDlg.clock.onClockPause()
            overItem.customHide();
        }
    }

    // on state change
    function onClockTimeout(state){
        console.log("onClockTimeout("+state+")")
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
