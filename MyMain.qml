/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 640
    height: 550
    title: qsTr("Kt Alarm Clock")

    // Left
    MyLeft{
        id: ktLeft
        height: parent.height
    }

    MyRight{
        id: ktRight
        height: 550
        anchors.left: ktLeft.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.leftMargin: 0
    }

    // connect signal onCompleted
    Component.onCompleted: {
        ktLeft.actionClicked.connect(ktRight.changeScrollRect)// pass index value
    }

}
