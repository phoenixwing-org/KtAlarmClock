/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import KtAlarmClock 1.0

MyRightForm {

    // test slot
    function mySlot(index)
    {
        console.log("mySlot("+index+")")
    }
    
    /**
     * update parameter to dialog
     */
    function onUpdateDialog(){
        ktTimeSlideMyWork.value = myAlarmClockParam.WorkTime
        ktTimeSlideMyExercise.value = myAlarmClockParam.WorkBreak
        ktTimeSlideMyForce.value = myAlarmClockParam.TimeForce
    }

    /**
     * get parameter from dialog
     */
    function onUpdateInfos()
    {        
        myAlarmClockParam.WorkTime = ktTimeSlideMyWork.value
        myAlarmClockParam.WorkBreak = ktTimeSlideMyExercise.value
        myAlarmClockParam.TimeForce = ktTimeSlideMyForce.value
    }

    // change Sub Item
    function scrollPage(index) {
        //console.log("scrollPage("+index+")")
        if(0 === index){
            rect.y = - rect0.y
        } else if(1 === index){
            rect.y = - rect1.y
        } else{
            rect.y = - rect2.y
        }
    }
    
    // connect signal onCompleted
    Component.onCompleted: {
        //console.log("MyRight.onCompleted()")
        myAlarmClockParam.sigUpdateInfos.connect(onUpdateInfos)
        myAlarmClockParam.sigUpdateDialog.connect(onUpdateDialog)

        onUpdateDialog()
        //console.log("MyRight.onCompleted()-end")
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
