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
    if (KtAlarmTheme.debugLocate) console.log("over0 .afterClockRunningChanged() :", clock.running)

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
    if (KtAlarmTheme.debugLocate) console.log("over0 .afterIsForceChanged() :", isForced)
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
    if (!over0.visible) {
        if (KtAlarmTheme.debugLocate) console.log("over0.visible = false, then exit afterClockCounterChanged()")
        return;
    }
    if (KtAlarmTheme.debugLocate) console.log("over0 .afterClockCounterChanged()")

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
}

/*
 * Show Window
 */
function customHide() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .customHide()")
    over0.canClose = true; // Do not change over 1's canClose
    over0.hide() //hide
    loaderOver1.sourceComponent = null // unload over 1
}

/*
 * Show Window
 */
function customShow() {
    if (KtAlarmTheme.debugLocate) console.log("over0 .customShow()")

    // if debug, not show all
    over0.fullScreen = (KtAlarmTheme.debug == 0) 
    clock.counter = over0.counter;//set parameter first 
    showOver0()// then show Over page

    // show over 1? debug or screens length more than one
    if (KtAlarmTheme.debug || Qt.application.screens.length>1) {
        loaderOver1.sourceComponent = compOver1;
        let comp = loaderOver1.item
        comp.screenId = KtAlarmTheme.debug? 0 : 1 

        if(null !== comp) {
            comp.checkoutScreen();
            comp.showOver();
            // debug for position
            if(KtAlarmTheme.debug !== 0) {
                comp.x = over0.width
                comp.color= "grey"
            }
        } else {
            console.log("loader: over 1 is null")
        }
    }
    
}

function unlockPage() {
    if (KtAlarmTheme.debugLocate) console.log("over0.unlockPage()")
    if (isForced) {
        over0.canClose = false;
        return
    }

    // check whether can close
    let can = false;
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

    textEditResult.text = ""
    if (canClose) {
        showMessage("")
        over0.onClockOut(KtAlarmClock.WorkBreak) // clock out from break
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
    if (KtAlarmTheme.debugLocate) console.log("over0 .initialFormula()")
    let a = Math.floor(1000 * Math.random());
    let b = Math.floor(1000 * Math.random());
    formulaValue = a + b;
    labelFormula.text = a + " + " + b + " ="
}

function showMessage(msg) {
    labelMsg.text = msg
}
