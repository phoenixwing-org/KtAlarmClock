/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        main.js
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
    popMenu.onAction.connect(MainJs.runCommand)
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

    overUnload()

    //hide
    trayIcon.hide()
    mainDlg.close()
    root.close()
}

function overLoad() {
    console.log("overLoad()");
    loadOver0.source = "qrc:/MyOver0.qml"
    let over = loadOver0.item
    over.onClockOut.connect(clockTimeout)
    over.counter = myAlarmClockParam.WorkBreak;
    over.counterForce = myAlarmClockParam.TimeForce;
    over.customShow();
}

function overUnload() {
    if (null == loadOver0.item) return
    if (KtAlarmTheme.debugLocate) console.log("loadOver0 unload");
    loadOver0.item.canClose = true
    loadOver0.source = ""
}

function clockStart(state) {
    if (KtAlarmTheme.debugLocate) console.log("main .clockStart(" + state + ")");
    //myAlarmClockParam.dump() // dump
    KtAlarmTheme.workStep = state

    switch (state) {
        case KtAlarmClock.WorkBreak:
            clock.clockPause()
            // make sure hide in Mac system
            mainDlg.hide()
            root.hide()
            overLoad()
            return
        case KtAlarmClock.WorkTime:
            root.show()
            overUnload() // duplicate hide
            clock.clockStart(state);
            break;
        default:
            root.show()
            overUnload() // duplicate hide
            clock.clockPause()
    }

}

// command after clock time out
function clockTimeout(state) {
    if (KtAlarmTheme.debugLocate) console.log("main .clockTimeout(" + state + ")")

    // on state change
    switch (state) {
        case KtAlarmClock.WorkTime:
            mainDlg.hide()
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
