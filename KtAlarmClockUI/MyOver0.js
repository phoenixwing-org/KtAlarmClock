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
    if (KtAlarmTheme.debug)
        console.log("afterClockRunningChanged() :", clock.running)

    if (clock.running) {
        showMessage("");
        over0.canClose = false
    } else {

        showMessage("Close:");
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
    console.log("afterIsForceChanged() :", isForced)
    if (isForced) {
        over0.canClose = false
        isShowFormula = false
    } else {
        // make sure page can be closed
        // do not close it now

        over0.canClose = true
        counterForce = 0

        // do not set :
        // buttonUnlock.visible, isShowFormula
    }
}

/*
 * @brief action after clock counter Changed
 */
function afterClockCounterChanged() {
    if (!over0.visible) return;

    // if forced, change counterForce...
    if (isForced && counterForce > 0) {
        counterForce--;
        if (counterForce < 1) isForced = false;
    }

    // change isShowFormula
    if (isForced) {
        isShowFormula = false
    } else if (clock.running) {
        isShowFormula = true
    } else isShowFormula = false

    if (over0.visible) {
        if (!over1.visible) over1.showOver()
    }
    // debug
    //if (KtAlarmTheme.debug)
    //    console.log("afterClockCounterChanged: ",
    //        clock.running, isShowFormula,
    //        clock.counter, counterForce)
}

/*
 * Show Window
 */
function customHide() {
    console.log("over0.customHide()")

    // Do not change over1.canClose
    over0.canClose = true;
    clock.running = false

    // for over 0 and 1
    over0.hide();
    over1.hide()
}


/*
 * Show Window
 */
function customShow() {
    console.log("over0.customShow()")
        // if debug, not show all
    let fullScreen = (KtAlarmTheme.debug == 0)
    over0.fullScreen = fullScreen

    let showOver1 = false;
    if (over0.screenOK) {

        //set parameter first 
        clock.counter = over0.counter;

        // then show Over page
        showOver0()

        // SET FOR OVER 1
        showOver1 = over1.screenOK


    } else {
        over0.hide()
    }

    if (showOver1) over1.showOver();
    //else over1.hide();

    // debug
    if (KtAlarmTheme.debug) {
        console.log("afterClockCounterChanged: running, isShowFormula, counter, counterForce");
    }

    //afterClockCounterChanged()
}

function unlockPage() {
    if (KtAlarmTheme.debug)
        console.log("MyOver0.unlockPage()")
    if (isForced) {
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

    if (counterForce > 0) {
        isForced = true
        if (counterForce >= counter) {
            counterForce = 0; // do not counter the force
        }
    } else isForced = false

    clock.clockStart(KtAlarmClock.WorkBreak);

    return showOver()
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