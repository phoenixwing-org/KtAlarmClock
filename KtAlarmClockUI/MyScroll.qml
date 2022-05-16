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

Item {
    id: itemTop
    property alias rect2: rect2
    property alias rect1: rect1
    property alias rect0: rect0
    property alias scroll: scroll
    width: 625
    height: 400

    ScrollView {
        id: scroll
        clip: true
        anchors.margins: 0
        anchors.fill: parent
        property int widthTime: 200
        height: 250

        flickableItem.interactive: true

        // rect width will affect scroll view horizontalScrollBar
        Rectangle {
            id: rect
            width: parent.width
            height: scroll.height
            clip: false
            color: "grey"
            Rectangle {
                id: rect0
                width: itemTop.width
                height: ktTimeSlideMyWork.height
                anchors.top: rect.top
                color: "#000000"
                radius: 5
                border.width: 0
                border.color: "#ffffff"

                KtTimeSlide {
                    id: ktTimeSlideMyWork
                    width: widthTime
                    value: 2400
                    from: 5
                    to: 3600
                    stepSize: 300
                    title: qsTr("Work:")
                    onValueChanged: {
                        myAlarmClockParam.WorkTime = value;
                    }
                }
            }

            Rectangle {
                id: rect1
                anchors.top: rect0.bottom
                width: itemTop.width
                height: ktTimeSlideMyExercise.height
                color: "black"
                border.width: 0
                anchors.topMargin: 2
                border.color: "transparent"

                KtTimeSlide {
                    id: ktTimeSlideMyExercise
                    width: widthTime
                    value: 600
                    from: 5
                    to: 1200
                    stepSize: 60
                    title: qsTr("Sports:")
                    onValueChanged: {
                        myAlarmClockParam.WorkBreak = value;
                    }
                }
            }

            Rectangle {
                id: rect2
                width: itemTop.width
                color: "black"
                border.width: 0
                anchors.top: rect1.bottom
                anchors.bottomMargin: 0
                anchors.topMargin: 2
                anchors.bottom: parent.bottom
                KtTimeSlide {
                    id: ktTimeSlideMyForce
                    width: widthTime
                    value: 60
                    from: 0
                    to: 120
                    stepSize: 10
                    title: qsTr("Force:")
                    onValueChanged: {
                        myAlarmClockParam.TimeForce = value;
                    }
                }
            }
        }
    }


    // change Sub Item
    function scrollPage(index)
    {
        console.log("scrollPage("+index+")")
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
}




