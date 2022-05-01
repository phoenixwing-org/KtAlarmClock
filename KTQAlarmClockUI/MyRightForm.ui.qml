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
        anchors.bottom: parent.bottom
    }


}

/*##^##
Designer {
    D{
        i: 1;anchors_height: 400
    }
}
##^##*/

