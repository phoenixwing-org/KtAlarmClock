/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import QtQuick.Layouts 1.12
// Controls 2.14 do not have flickableItem
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.14

Rectangle {
    id: root
    width: 625
    height: 400
    color: KtAlarmTheme.colorBackground
    property alias rect2: rect2
    property alias rect1: rect1
    property alias rect0: rect0
    property alias scroll: scroll

    ScrollView {
        id: scroll
        clip: true
        anchors.margins: 0
        anchors.fill: parent
        height: 250

        flickableItem.interactive: true

        // rect width will affect scroll view horizontalScrollBar
        Rectangle {
            id: rect
            width: parent.width
            height: scroll.height
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
                    from: 5
                    to: 5400
                    stepSize: 300
                    title: qsTr("Work:")
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    onValueChanged: {
                        myAlarmClockParam.WorkTime = value;
                    }
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
                    onValueChanged: {
                        myAlarmClockParam.WorkBreak = value;
                    }
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
                    onValueChanged: {
                        myAlarmClockParam.TimeForce = value;
                    }
                }
            }
        }
    }


    // change Sub Item
    function scrollPage(index) {
        // console.log("scrollPage("+index+")")
        if(0 === index){
            scroll.flickableItem.contentY = rect0.y
        } else if(1 === index){
            scroll.flickableItem.contentY = rect1.y
        } else{
            scroll.flickableItem.contentY = rect2.y
        }
    }
    
    // connect signal onCompleted
    Component.onCompleted: {
        // method 1: bind the property
        ktTimeSlideMyWork.value = myAlarmClockParam.WorkTime
        ktTimeSlideMyExercise.value = myAlarmClockParam.WorkBreak
        ktTimeSlideMyForce.value = myAlarmClockParam.TimeForce
    }
 
    /**
     * update parameter information
     */
    function updateInfor()
    {        
        myAlarmClockParam.WorkTime = ktTimeSlideMyWork.value
        myAlarmClockParam.WorkBreak = ktTimeSlideMyExercise.value
        myAlarmClockParam.TimeForce = ktTimeSlideMyForce.value
    }
}





/*##^##
Designer {
    D{i:4;anchors_width:615}
}
##^##*/
