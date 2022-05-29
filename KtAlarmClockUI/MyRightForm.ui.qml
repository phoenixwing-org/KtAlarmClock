
/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14

Item {
    id: root
    width: 600
    height: 400
    clip: true
    property alias rect: rect
    property alias rect0: rect0
    property alias rect1: rect1
    property alias rect2: rect2
    property alias ktTimeSlideMyWork: ktTimeSlideMyWork
    property alias ktTimeSlideMyExercise: ktTimeSlideMyExercise
    property alias ktTimeSlideMyForce: ktTimeSlideMyForce

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
                from: 5
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
}

/*##^##
Designer {
    D{i:1;invisible:true}
}
##^##*/

