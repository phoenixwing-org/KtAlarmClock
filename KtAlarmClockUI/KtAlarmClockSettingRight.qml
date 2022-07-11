/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14
import KtAlarmClock 1.0

Item {
    id: root
    width: 600
    height: 400
    clip: true
    property var myParam: null

    Rectangle {
        color: "#000000"
        anchors.fill: parent
    }

    // rect width will
    Rectangle {
        id: rect
        width: parent.width
        clip: false
        color: "transparent"
        Rectangle {
            id: rect0
            width: root.width
            height: ktTimeSlideMyWork.height
            anchors.top: rect.top
            color: "#000000"
            radius: 5
            border.width: 0
            border.color: "#ffffff"

            KtTimeSlide {
                id: ktTimeSlideMyWork
                value: 2400
                from: 60
                to: 5400
                stepSize: 300
                title: qsTr("Work:")
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: 5
            }
        }

        Rectangle {
            id: rect1
            anchors.top: rect0.bottom
            width: root.width
            height: ktTimeSlideMyExercise.height
            color: "black"
            border.width: 0
            anchors.topMargin: 2
            border.color: "transparent"

            KtTimeSlide {
                id: ktTimeSlideMyExercise
                value: 600
                from: 10
                to: 3600
                stepSize: 60
                title: qsTr("Sports:")
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: 5
            }
        }

        Rectangle {
            id: rect2
            width: root.width
            color: "black"
            border.width: 0
            anchors.top: rect1.bottom
            anchors.bottomMargin: 0
            anchors.topMargin: 2
            anchors.bottom: parent.bottom
            KtTimeSlide {
                id: ktTimeSlideMyForce
                value: 300
                from: 0
                to: 3600
                stepSize: 10
                title: qsTr("Force:")
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: 5
            }
        }
    }

    // test slot
    function mySlot(index) {
        if (KtAlarmTheme.debugLocate) console.log("mySlot("+index+")")
    }
    
    /**
     * update parameter to dialog
     */
    function onUpdateDialog() {
        if(null == myParam) return
        if (KtAlarmTheme.debugLocate) console.log("KtAlarmClockSettingRight.onUpdateDialog()")
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
        if (KtAlarmTheme.debugLocate) console.log("KtAlarmClockSettingRight.onUpdateInfos()")
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
