/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.12

Item {
    id: itemTop
    width: 300
    height: 260
    property alias rect2: rect2
    property alias rect1: rect1
    property alias rect0: rect0
    property alias scroll: scroll

    ScrollView {
        id: scroll
        width: 300
        clip: true
        anchors.margins: 5
        anchors.fill: parent

        // rect width will affect scroll view horizontalScrollBar
        Rectangle {
            id: rect
            width: parent.width
            height: rect0.height + rect1.height + rect2.height
            clip: false
            Rectangle {
                id: rect0
                width: itemTop.width
                anchors.top: rect.top
                height: gridLayout0.height
                color: "#000000"
                border.color: "blue"

                GridLayout {
                    id: gridLayout0
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.top: parent.top
                    columns: 3
                    rows: 2

                    Label {
                        id: label
                        Layout.fillWidth: false
                        color: "#ffffff"
                        text: qsTr("Work :")
                        Layout.fillHeight: true
                    }

                    SpinBox {
                        id: spinBoxWorkTime
                        width: 200
                        maximumValue: 86400
                        Layout.preferredWidth: 100
                        Layout.minimumWidth: 80
                        Layout.fillWidth: false
                        value: 60
                        onValueChanged: {
                            myAlarmClockParam.WorkTime = value * 60;
                            console.log("WorkTime = " + myAlarmClockParam.WorkTime)
                        }
                    }
                    Label {
                        color: "#ffffff"
                        text: qsTr("Minute")
                        Layout.fillWidth: true
                    }

                    Label {
                        id: label1
                        Layout.fillWidth: false
                        color: "#ffffff"
                        text: qsTr("Break :")
                        Layout.maximumWidth: 100
                        Layout.preferredWidth: 60
                        Layout.minimumWidth: 60
                        Layout.fillHeight: true
                    }

                    SpinBox {
                        id: spinBoxWorkBreak
                        width: 200
                        maximumValue: 86400
                        value: 10
                        Layout.preferredWidth: 100
                        Layout.fillWidth: false
                        onValueChanged: {
                            myAlarmClockParam.WorkBreak = value * 60;
                            console.log("WorkBreak = " + myAlarmClockParam.WorkBreak)
                        }
                    }
                    Label {
                        color: "#ffffff"
                        text: qsTr("Minute")
                        Layout.fillWidth: true
                    }

                    Label {
                        id: label4
                        color: "#ffffff"
                        text: qsTr("Timer:")
                    }

                    SpinBox {
                        id: spinBoxTimeCounter
                        layer.enabled: false
                        enabled: false
                        value: 0
                        maximumValue: 86400
                        Layout.preferredWidth: 100
                    }
                    Label {
                        color: "#ffffff"
                        text: qsTr("Minute")
                        Layout.fillWidth: true
                    }

                }
            }

            Rectangle {
                id: rect1
                anchors.top: rect0.bottom
                width: itemTop.width
                height: 150
                color: "black"
                anchors.topMargin: 5
                border.color: "blue"
            }

            Rectangle {
                id: rect2
                anchors.top: rect1.bottom
                width: itemTop.width
                height: 300
                color: "black"
                anchors.topMargin: 5
                border.color: "blue"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        scroll.flickableItem.contentY = rect1.y
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
        spinBoxWorkBreak.value = myAlarmClockParam.WorkBreak / 60
        spinBoxWorkTime.value = myAlarmClockParam.WorkTime / 60

        // method 2: connect the signal
        myAlarmClockParam.sigTimeCounter.connect(changeTimeCounter)//void
    }

    function changeTimeCounter(value){
        console.log("changeTimeCounter(" + value + ")")
        spinBoxTimeCounter.value = myAlarmClockParam.TimeCounter / 60

    }

}

/*##^##
Designer {
    D{i:4;anchors_height:46;anchors_width:198}D{i:2;anchors_height:900;anchors_width:530}
D{i:1;anchors_height:263;anchors_width:514}
}
##^##*/
