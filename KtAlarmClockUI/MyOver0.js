/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        MyOver0.js
 * @brief       js functions for MyOver0
 */

.import "LockScreenManager.js" as LockScreen

/*
 * @brief action after Clock Running Changed
 */
function afterClockRunningChanged() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .afterClockRunningChanged() :", clock.running)

    showMessage("");
    if (clock.running) {
        over0.canClose = false
    } else {
        // make sure page can be closed
        // do not close it now

        over0.canClose = true
        isForced = false
        isShowFormula = false

        // do not set buttonUnlock.visible
    }
}

/*
 * @brief action after Force timer Running Changed
 */
function afterIsForceChanged() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .afterIsForceChanged() :", isForced)
    if (isForced) {
        over0.canClose = false
        isShowFormula = false
    } else {
        // make sure page can be closed
        // do not close it now

        over0.canClose = true
        counterForce = 0
        forceEndMs = 0

        // do not set :
        // buttonUnlock.visible, isShowFormula
    }
}

/*
 * @brief action after clock counter Changed
 */
function afterClockCounterChanged() {
    if (!over0.visible) {
        if (KtAlarmTheme.debugLocate) console.log("over0.visible = false, then exit afterClockCounterChanged()")
        return;
    }
    if (KtAlarmTheme.debugLocate) console.log("over0 .afterClockCounterChanged()")
    updateFormulaVisibility()
}

/*
 * @brief sync TimeForce countdown from wall clock
 */
function syncForceFromWallClock() {
    if (!over0.visible)
        return

    if (forceEndMs <= 0) {
        updateFormulaVisibility()
        return
    }

    var remaining = Math.ceil((forceEndMs - Date.now()) / 1000)
    if (remaining <= 0) {
        isForced = false
        counterForce = 0
        forceEndMs = 0
    } else {
        isForced = true
        counterForce = remaining
    }
    updateFormulaVisibility()
}

function updateFormulaVisibility() {
    if (isForced) {
        isShowFormula = false
    } else if (clock.running) {
        isShowFormula = true
    } else {
        isShowFormula = false
    }
}

/*
 * @brief calibrate break clock and force timer (e.g. after resume from sleep)
 */
function syncWallClocks() {
    if (clock.running)
        clock.syncFromWallClock()
    syncForceFromWallClock()
}

function hideSecondaryScreens() {
    LockScreen.hideAll()
}

function reconcileLockScreens() {
    if (!over0.visible)
        return
    LockScreen.reconcileScreens(over0, unlockPage)
}

function raiseLockScreens() {
    if (!over0.visible)
        return
    LockScreen.raiseAll(over0)
}

function customHide() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .customHide()")
    over0.canClose = true
    hideSecondaryScreens()
    over0.hide()
}


/*
 * Show Window
 */
function customShow() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .customShow()")

    over0.fullScreen = (KtAlarmTheme.debug == 0)
    clock.counter = over0.counter
    showOver0()
    reconcileLockScreens()
    raiseLockScreens()
}

function unlockPage() {
    if (KtAlarmTheme.debugLocate) console.log("over0.unlockPage()")
    if (isForced) {
        over0.canClose = false;
        return
    }

    // check whether can close
    var can = false;
    if (clock.running) {
        var value = parseInt(textEditResult.text)
        can = (value == formulaValue)

        if (KtAlarmTheme.debug)
            console.log("answer=", formulaValue, "text=", textEditResult.text, "value=", value, "can=", can)
    } else {
        can = true
    }
    over0.canClose = can

    textEditResult.text = ""
    if (canClose) {
        showMessage("")
        over0.onClockOut(KtAlarmClock.WorkBreak)
    } else {
        showMessage("Wrong answer!")
    }
}

/*
 * Show Window 0
 */
function showOver0() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .showOver0()")

    over0.canClose = false
    showMessage("")
    initialFormula()

    if (KtAlarmTheme.debug) {
        flags = Qt.Window
    }

    if (over0.counterForce > 0) {
        if (over0.counterForce >= over0.counter) {
            over0.counterForce = 0
            isForced = false
            forceEndMs = 0
        } else {
            forceEndMs = Date.now() + over0.counterForce * 1000
            isForced = true
        }
    } else {
        isForced = false
        forceEndMs = 0
    }

    clock.clockStart(KtAlarmClock.WorkBreak);
    syncWallClocks()

    var ok = showOver()
    if (ok)
        raiseLockScreens()
    return ok
}

function initialFormula() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .initialFormula()")
    let a = Math.floor(1000 * Math.random());
    let b = Math.floor(1000 * Math.random());
    formulaValue = a + b;
    labelFormula.text = a + " + " + b + " ="
}

function showMessage(msg) {
    labelMsg.text = msg
}
