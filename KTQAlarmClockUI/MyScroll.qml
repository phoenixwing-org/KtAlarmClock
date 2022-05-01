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
    width: 530
    height: 300
    property alias rect2: rect2
    property alias rect1: rect1
    property alias rect0: rect0
    property alias scroll: scroll

    ScrollView {
        id: scroll
        anchors.fill: parent

        // rect width will affect scroll view horizontalScrollBar
        Rectangle {
            id: rect
            width: parent.width
            height: rect0.height + rect1.height + rect2.height
            Rectangle {
                id: rect0
                width: itemTop.width
                anchors.top: rect.top
                height: 300
                color: "red"
                border.color: "blue"
            }

            Rectangle {
                id: rect1
                anchors.top: rect0.bottom
                width: itemTop.width
                height: 300
                color: "green"
                border.color: "blue"
            }

            Rectangle {
                id: rect2
                anchors.top: rect1.bottom
                width: itemTop.width
                height: 300
                color: "orange"
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
}

/*##^##
Designer {
    D{
        i: 2;anchors_height: 900;anchors_width: 530
    }D{i: 1;anchors_height: 263;anchors_width: 514}
}
##^##*/
