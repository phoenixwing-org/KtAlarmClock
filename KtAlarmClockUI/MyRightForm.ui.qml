
/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.14

Item {
    id: element
    width: 600
    height: 400
    property alias myScroll: myScroll

    Rectangle {
        color: "#000000"
        anchors.fill: parent
    }

    MyScroll {
        id: myScroll
        width: parent.width
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
    }
}

/*##^##
Designer {
    D{i:1;anchors_height:400}D{i:2;anchors_height:400;anchors_width:400;anchors_x:76;anchors_y:48}
}
##^##*/
