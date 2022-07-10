/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import KtAlarmClock 1.0

MyRightForm {
    property var myParam: null

    // test slot
    function mySlot(index) {
        if (KtAlarmTheme.debugLocate) console.log("mySlot("+index+")")
    }
    
    /**
     * update parameter to dialog
     */
    function onUpdateDialog() {
        if(null == myParam) return
        if (KtAlarmTheme.debugLocate) console.log("MyRight.onUpdateDialog()")
        if (KtAlarmTheme.debugLocate) myParam.dump()
        ktTimeSlideMyWork.value = myParam.WorkTime
        ktTimeSlideMyExercise.value = myParam.WorkBreak
        ktTimeSlideMyForce.value = myParam.TimeForce
    }

    /**
     * get parameter from dialog
     */
    function onUpdateInfos() {
        if(null == myParam) return
        if (KtAlarmTheme.debugLocate) console.log("MyRight.onUpdateInfos()")
        myParam.WorkTime = ktTimeSlideMyWork.value
        myParam.WorkBreak = ktTimeSlideMyExercise.value
        myParam.TimeForce = ktTimeSlideMyForce.value
        if (KtAlarmTheme.debugLocate)  myParam.dump()
    }

    // change Sub Item
    function scrollPage(index) {
        if (KtAlarmTheme.debugLocate) console.log("scrollPage("+index+")")
        if(0 === index){
            rect.y = - rect0.y
        } else if(1 === index){
            rect.y = - rect1.y
        } else{
            rect.y = - rect2.y
        }
    }
}
