/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4

Item {
    id: element
    width: 600
    height: 400
    property alias myScroll: myScroll

    MyScroll {
        id: myScroll
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottomMargin: rectBottom.border.height
        anchors.bottom: rectBottom.top
    }

    Rectangle {
        id: rectBottom
        x: 0
        y: 140
        width: parent.width
        height: 54
        radius: 20
        gradient: Gradient {
        GradientStop {
            position: 0
            color: "#d4dfe7"
        }

        GradientStop {
            position: 1
            color: "#ff000000"
        }
    }
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 0
    border.color: "pink"
    RowLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        ToolButton {
            id: toolButtonStart
            width: 90
            height: 36
            text: "Start"
            layer.effect: toolButtonStart
            layer.enabled: false
            layer.smooth: true
            isDefault: true
            iconName: ""
            iconSource: "qrc: images/Start.png"
        }

        ToolButton {
            id: toolButtonPause
            text: "Pause"
            iconSource: "qrc: images/Pause.png"
        }

        ToolButton {
            id: toolButton2
            text: "Stop"
            iconSource: "qrc: images/Reset.png"
        }
    }
}
}

/*##^##
Designer {
    D{
        i: 1;anchors_height: 400
    }
}
##^##*/

