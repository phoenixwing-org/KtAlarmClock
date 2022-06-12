/**
 * @copyright   Shanghai Kuntai Software Technology Co., Ltd. 2022
 * @license     LGPL 3.0
 * @author      Kuntai
 * @file        MyOver0.js
 * @brief       js functions for MyOver0
 * @note        this can hide these functions as private
 */


/*
 * @brief action after Clock Running Changed
 */
function afterClockRunningChanged() {
    console.log("afterClockRunningChanged()")
    if (clock.running) {
        showMessage("");
        over0.canClose = false
    } else {

        showMessage("Close:");
        // make sure page can be closed
        // do not close it now

        over0.canClose = true
        counterForce = 0
        timeForce.running = false

        // do not set buttonUnlock.visible
    }
}
/*
 * @brief action after Force timer Running Changed
 */
function afterForceRunningChanged() {
    console.log("afterForceRunningChanged()")
    if (force.running) {
        over0.canClose = false
    } else {
        // make sure page can be closed
        // do not close it now

        over0.canClose = true
        counterForce = 0
        timeForce.running = false

        // do not set buttonUnlock.visible
    }
}

/*
 * @brief action after Force timer Running Changed
 */
function afterForceTriggered() {
    console.log("afterForceRunningChanged()")
    over0.counterForce--;

    // console.log("counterForce", over0.counterForce)
    if (over0.counterForce <= 0) force.stop();
}

function changeFooterVisible() {
    if (clock.counter <= 0) {
        showFormula = false
    } else if (over0.counterForce <= 0) {
        showFormula = clock.running
    } else showFormula = false
    if (KtAlarmTheme.debug)
        console.log("showFormula", showFormula, "clock.counter", clock.counter, "clock.running", clock.running, "over0.counterForce", over0.counterForce)
}

/*
 * Show Window
 */
function customHide() {
    over0.visible = false
    over1.visible = false
    over0.canClose = true;
    over1.canClose = true;

    if (clock.running) {
        over0.stopPage0()
    }

    // for over1
    over0.hide()
    over1.hide()
}


/*
 * Show Window
 */
function customShow() {
    over0.visible = false
    over1.visible = false
    over0.screenId = 0
    over1.screenId = 1
    over0.checkoutScreen()
    over1.checkoutScreen()

    //console.log("over1.screenOK = ",over1.screenOK)
    //console.log("over0.screenOK = ",over0.screenOK)

    // if debug, not show all
    let fullScreen = (KtAlarmTheme.debug == 0)
    over0.fullScreen = fullScreen
    over1.fullScreen = fullScreen

    if (over1.screenOK) over1.showOver()
    else over1.hide()

    if (over0.screenOK) {
        over0.canClose = false; // cannot close
        over0.counterForce = over0.timeForce
        showOver0()

        clock.counter = over0.counter;
        clock.clockStart(KtAlarmClock.WorkBreak);
    } else over0.hide()

    changeFooterVisible()
}

function unlockPage() {
    if (KtAlarmTheme.debug)
        console.log("MyOver0.unlockPage()")
    if (over0.counterForce > 0) {
        over0.canClose = false;
        return
    }

    // check whether can close
    let can = false;
    if (KtAlarmTheme.debug)
        console.log("clock.running = ", clock.running)
    if (clock.running) {
        // if running, check answer
        var value = parseInt(textEditResult.text)

        can = (value == formulaValue)

        if (KtAlarmTheme.debug)
            console.log("answer=", formulaValue, "text=", textEditResult.text, "value=", value, "can=", can)
    } else {
        can = true
    }
    over0.canClose = can

    if (KtAlarmTheme.debug)
        console.log("canClose = ", can, over0.canClose)

    textEditResult.text = ""
    if (canClose) {
        showMessage("")
        over.customHide()
        over0.onClockOut(KtAlarmClock.WorkBreak) // clock out from break
    } else {
        showMessage("Result is wrong! Please try agin.")
    }
}

/*
 * Show Window 0
 */
function showOver0() {
    // console.log("MyOver0Js.showOver0()")

    over0.canClose = false
    showMessage("")
    initialFormula()
    if (KtAlarmTheme.debug) {
        flags = Qt.Window
    }

    console.log(counterForce, counter, counterForce, clock.counter)
    if (counterForce > 0 && counterForce < counter) {
        force.start();
    } else force.running = false

    return showOver()
}

function stopPage0() {
    if (KtAlarmTheme.debug)
        console.log("MyOver0.stopPage0()")

    over0.canClose = true;
    if (clock.running) {
        clock.clockPause()
    }
    showMessage("")
}

function initialFormula() {
    let a = Math.floor(1000 * Math.random());
    let b = Math.floor(1000 * Math.random());
    formulaValue = a + b;
    labelFormula.text = a + " + " + b + " ="
}

function showMessage(msg) {
    labelMsg.text = msg
}