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
    height: 480
    minimumWidth:400
    minimumHeight: 300
    title: qsTr("Kt Alarm Clock")

    // Left
    MyLeft{
        id: ktLeft
        anchors.left: parent.left
        anchors.right: ktRight.left
        anchors.bottom: ktfooter.top
        anchors.top: parent.top
    }

    MyRight{
        id: ktRight
        anchors.left: ktLeft.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: ktfooter.top
        anchors.leftMargin: 0
    }

    MyFooter{
        id:ktfooter
        height:60
        width: parent.width
        anchors.left: parent.left
        anchors.bottom: parent.bottom


    }


    // connect signal onCompleted
    Component.onCompleted: {
        ktLeft.actionClicked.connect(ktRight.changeScrollRect)// pass index value
    }

}
