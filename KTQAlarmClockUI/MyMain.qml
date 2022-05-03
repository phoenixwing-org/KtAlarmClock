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
    id: root
    visible: true
    width: 640
    height: 480
    minimumWidth:100
    minimumHeight: 100
    title: qsTr("Kt Alarm Clock")


    property int  mouseRegion: 5
    property int onlyDispalyIconWidth:450

    MouseArea {
        id:leftX
        width: root.mouseRegion
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        cursorShape: Qt.SizeHorCursor
        property int xPosition: 0
        onPressed: {
            xPosition = mouse.x
        }

        onPositionChanged: {
            var xOffset = mouse.x-xPosition
            if(root.x+xOffset>0 && root.width-xOffset>root.minimumWidth){
                root.x = root.x+xOffset
                root.width = root.width-xOffset
                root.autoDisplayLeftMenu()
            }
        }
    }



    MouseArea{
        id:rightX
        width: root.mouseRegion
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        anchors.rightMargin: 0
        cursorShape: Qt.SizeHorCursor
        property int xPosition: 0
        onPressed: {
            xPosition = mouse.x
        }

        onPositionChanged: {
            var xOffset = mouse.x-xPosition
            var xWidth = root.width+xOffset
            if(xWidth+root.x<root.maximumWidth && xWidth>root.minimumWidth){
                root.width = xWidth
                root.autoDisplayLeftMenu()
            }
        }
    }

    MouseArea{
        id:topY
        height: root.mouseRegion
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        cursorShape: Qt.SizeVerCursor
        property int yPosition: 0
        onPressed: {
            yPosition = mouse.y
        }

        onPositionChanged: {
            var yOffset = mouse.y-yPosition
            if(root.y+yOffset>0 && root.height-yOffset>root.minimumHeight){
                root.y = root.y+yOffset
                root.height = root.height-yOffset
                root.autoDisplayLeftMenu()
            }
        }
    }

    MouseArea{
        id:bottomY
        height: root.mouseRegion
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        cursorShape: Qt.SizeVerCursor
        property int yPosition: 0
        onPressed: {
            yPosition = mouse.y
        }

        onPositionChanged: {
            var yOffset = mouse.y-yPosition
            var yHeight = root.height+yOffset
            if(yHeight+root.y<root.maximumHeight && yHeight>root.minimumHeight){
                root.height = yHeight
                root.autoDisplayLeftMenu()
            }
        }
    }

    MouseArea{
        id:leftTopCorner
        width: root.mouseRegion
        height: root.mouseRegion
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.leftMargin: 0
        cursorShape: Qt.SizeFDiagCursor
        property int xPosition: 0
        property int yPosition: 0
        onPressed: {
            xPosition = mouse.x
            yPosition = mouse.y
        }

        onPositionChanged: {
            var xOffset = mouse.x-xPosition
            if(root.x+xOffset>0 && root.width-xOffset>root.minimumWidth){
                root.x = root.x+xOffset
                root.width = root.width-xOffset
                root.autoDisplayLeftMenu()
            }
            var yOffset = mouse.y-yPosition
            if(root.y+yOffset>0 && root.height-yOffset>root.minimumHeight){
                root.y = root.y+yOffset
                root.height = root.height-yOffset
            }
        }
    }

    MouseArea{
        id:rightTopCorner
        width: root.mouseRegion
        height: root.mouseRegion
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.rightMargin: 0
        cursorShape: Qt.SizeBDiagCursor
        property int xPosition: 0
        property int yPosition: 0
        onPressed: {
            xPosition = mouse.x
            yPosition = mouse.y
        }

        onPositionChanged: {
            var xOffset = mouse.x-xPosition
            var xWidth = root.width+xOffset
            if(xWidth+root.x<root.maximumWidth && xWidth>root.minimumWidth){
                root.width = xWidth
                root.autoDisplayLeftMenu()
            }
            var yOffset = mouse.y-yPosition
            if(root.y+yOffset>0 && root.height-yOffset>root.minimumHeight){
                root.y = root.y+yOffset
                root.height = root.height-yOffset
            }
        }
    }

    MouseArea{
        id:leftBottomCorner
        width: root.mouseRegion
        height: root.mouseRegion
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        cursorShape: Qt.SizeBDiagCursor
        property int xPosition: 0
        property int yPosition: 0
        onPressed: {
            xPosition = mouse.x
            yPosition = mouse.y
        }
        onPositionChanged: {
            var xOffset = mouse.x-xPosition
            if(root.x+xOffset>0 && root.width-xOffset>root.minimumWidth){
                root.x = root.x+xOffset
                root.width = root.width-xOffset
                root.autoDisplayLeftMenu()
            }

            var yOffset = mouse.y-yPosition
            var yHeight = root.height+yOffset

            if(yHeight+root.y<root.maximumHeight && yHeight>root.minimumHeight){
                root.height = yHeight
            }
        }
    }




    MouseArea{
        id:rightBottomCorner
        width: root.mouseRegion
        height: root.mouseRegion
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        cursorShape: Qt.SizeFDiagCursor
        property int xPosition: 0
        property int yPosition: 0
        onPressed: {
            xPosition = mouse.x
            yPosition = mouse.y
        }
        onPositionChanged: {
            var xOffset = mouse.x-xPosition
            var xWidth = root.width+xOffset
            if(xWidth+root.x<root.maximumWidth && xWidth>root.minimumWidth){
                root.width = xWidth
                root.autoDisplayLeftMenu()
            }
            var yOffset = mouse.y-yPosition
            var yHeight = root.height+yOffset
            if(yHeight+root.y<root.maximumHeight && yHeight>root.minimumHeight){
                root.height = yHeight
            }
        }
    }

    // Left
    MyLeft{
        id: ktLeft
        height: ktRight.height

        function displayOnlyIcon()
        {
            width = ktLeft.minimunWidth;
            toolButtonMyClass.display = Qt.ToolButtonIconOnly
            toolButtonMySetting.display = Qt.ToolButtonIconOnly
            toolButtonMyWork.display = Qt.ToolButtonIconOnly

        }

        function displayTextBesideIcon()
        {
            width = ktLeft.defaultWidth;
            toolButtonMyClass.display = Qt.ToolButtonTextBesideIcon
            toolButtonMySetting.display = Qt.ToolButtonTextBesideIcon
            toolButtonMyWork.display = Qt.ToolButtonTextBesideIcon
        }

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
        ktLeft.sigScrollPage.connect(ktRight.myScroll.scrollPage)// pass index value
    }




    function autoDisplayLeftMenu()
    {
        if(root.width <= root.onlyDispalyIconWidth)
        {
            ktLeft.displayOnlyIcon()
        }else{
            ktLeft.displayTextBesideIcon()
        }
    }

}
