/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        KtAlarmClock.js
 * @brief       js functions for main window
 */

/**
 * @brief action after main Completed
 */
function afterCompleted() {
    if (KtAlarmTheme.debugLocate) console.log("main .afterCompleted()")
    if (KtAlarmTheme.debug) {
        myAlarmClockParam.WorkBreak = 10
        myAlarmClockParam.TimeForce = 2
        myAlarmClockParam.WorkTime = 15
    }

    //let x = Math.floor(Screen.width - 300)
    root.x = 600;

    // signal for clock
    clock.onClockOut.connect(clockTimeout)

    // for Action
    trayIcon.onAction.connect(MainJs.runCommand)
   KtAlarmTheme.onAction.connect(MainJs.runCommand);
    myAlarmClockParam.onAction.connect(MainJs.runCommand)

    //console.log("main.onCompleted()-end")
}

function afterWorkStepChanged() {

    if (KtAlarmTheme.debugLocate)
        console.log("main .afterWorkStepChanged(), workStep=", KtAlarmTheme.workStep)
}

function closeAllWindows() {
    if (KtAlarmTheme.debugLocate) console.log("main .closeAllWindows()")
    KtAlarmTheme.workStep = KtAlarmClock.None

    // can close
    root.canClose = true;

    unloadOverDlg()

    //hide
    trayIcon.hide()
    unloadSettingDlg()
    root.close()
}


function loadPopMenu() {
    if(KtAlarmTheme.debugLocate) console.log("MainJs.loadPopMenu()");

    var component = Qt.createComponent("qrc:/MyWindowMenu.qml")
    let popDlg = component.createObject(root)
    if(popDlg == null){
        console.log("Error to load popDlg");
        return
    }
    popDlg.x = root.x - width + root.width
    popDlg.y = root.y + root.height

    popDlg.show();
    
    // destroy when close
    popDlg.onClosing.connect(function(){
        if (KtAlarmTheme.debugLocate) console.log("popDlg.onClosing.connect(function())");
        popDlg.destroy()
    });
    popDlg.onAction.connect(MainJs.runCommand)
    

}

function loadSettingDlg() {
    if(KtAlarmTheme.debugLocate) console.log("MainJs.loadSettingDlg()");
    unloadSettingDlg()
    
    var component = Qt.createComponent("qrc:/MyMain.qml")
    let dlg = component.createObject(root)
    if(dlg == null){
        console.log("Error to load SettingDlg");
        return
    }
    dlg.myParam = myAlarmClockParam
    
    // destroy when close
    dlg.onClosing.connect(unloadSettingDlg);

    dlg.x = root.x
    dlg.y = root.y + root.height

    if(dlg.y < 30) dlg.y =30
    dlg.show();
    settingDlg = dlg
}

function unloadSettingDlg(){
    if(null !== settingDlg){
        settingDlg.myParam = null
        if(KtAlarmTheme.debugLocate) console.log("MainJs.unloadSettingDlg()");
        settingDlg.destroy();
        settingDlg = null;
    }
}

function loadOverDlg() {
    if(KtAlarmTheme.debugLocate) console.log("MainJs.loadOverDlg()");
    loaderOver0.source = "qrc:/MyOver0.qml"
    let over = loaderOver0.item
    over.onClockOut.connect(clockTimeout)
    over.counter = myAlarmClockParam.WorkBreak;
    over.counterForce = myAlarmClockParam.TimeForce;
    over.customShow();
}

function unloadOverDlg() {
    if (null == loaderOver0.item) return
    if (KtAlarmTheme.debugLocate) console.log("loaderOver0 unload");
    loaderOver0.item.canClose = true
    loaderOver0.source = ""
}

function clockStart(state) {
    if (KtAlarmTheme.debugLocate) console.log("main .clockStart(" + state + ")");
    //myAlarmClockParam.dump() // dump
    KtAlarmTheme.workStep = state

    switch (state) {
        case KtAlarmClock.WorkBreak:
            clock.clockPause()
            // make sure hide in Mac system
            unloadSettingDlg()
            root.hide()
            loadOverDlg()
            return
        case KtAlarmClock.WorkTime:
            root.show()
            unloadOverDlg() // duplicate hide
            clock.clockStart(state);
            break;
        default:
            root.show()
            unloadOverDlg() // duplicate hide
            clock.clockPause()
    }

}

// command after clock time out
function clockTimeout(state) {
    if (KtAlarmTheme.debugLocate) console.log("main .clockTimeout(" + state + ")")

    // on state change
    switch (state) {
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
function runCommand(index) {
    if (KtAlarmTheme.debugLocate) console.log("main .runCommand(" + index + ")")

    switch (index) {
        case KtAlarmClock.ActionPlayPause:
            root.show()
            clock.visible = true
            if (clock.counter <= -100) {
                // first time to paly
                KtAlarmTheme.loop = true

                myAlarmClockParam.onUpdateInfos() // get infos
                clock.counter = myAlarmClockParam.WorkTime; // reset
            } else {
                // change state
                KtAlarmTheme.loop = !(KtAlarmTheme.loop)
            }

            console.log("To : loop = " + KtAlarmTheme.loop + ", counter = ", clock.counter)
            if (KtAlarmTheme.loop) {
                clockStart(KtAlarmClock.WorkTime);
            } else {
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
            loadSettingDlg()
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
