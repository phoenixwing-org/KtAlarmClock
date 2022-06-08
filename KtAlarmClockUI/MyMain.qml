/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import KtAlarmClock 1.0

KtWindow{
    id: root
    visible: true
    width: 480
    height: 480

    title: qsTr("护眼闹钟")

    left:ktLeft
    right: ktRight

    // Left
    MyLeft{
        id: ktLeft
        y: ktRight.y

        parent: center
        height: ktRight.height
        anchors.left: parent.left
        anchors.leftMargin: 0
    }

    MyRight{
        id: ktRight
        parent: center
        anchors.left: ktLeft.right
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
    }

    MyFooter{
        id:ktfooter
        parent: footer

        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
    }

    // connect signal onCompleted
    Component.onCompleted: {
        //console.log("MyMain.onCompleted()")
        ktLeft.sigScrollPage.connect(ktRight.scrollPage)// pass index value
        root.sigWindowSizeChanged.connect(root.autoDisplayLeftMenu)
        //console.log("MyMain.onCompleted()-end")
    }

    function autoDisplayLeftMenu()
    {
        var onlyIcon = (root.width <= root.onlyDisplayIconWidth);
        if ( onlyIcon != ktLeft.onlyIcon){
            ktLeft.onlyIcon = onlyIcon;
            root.__menuBarType = onlyIcon? 0:1
        }
    }
}
