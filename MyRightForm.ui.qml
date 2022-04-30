
/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

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
                icon.source: "qrc:/image/play.svg"
                layer.effect: toolButtonStart
                layer.enabled: false
                layer.smooth: true
            }

            ToolButton {
                id: toolButtonPause
                text: "Pause"
                icon.source: "qrc:/image/pause.svg"
            }

            ToolButton {
                id: toolButtonReset
                text: "Stop"
                icon.source: "qrc:/image/stop.svg"
            }
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:400}
}
##^##*/

