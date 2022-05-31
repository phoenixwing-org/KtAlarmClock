import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
Window {
    id:root
    visible: true
    width: 640
    height: 480
    color: KtAlarmTheme.colorPrimary
    property alias footer: footer
    property alias header: header
    property alias center: center
    property var left: null
    property var right: null

    property int mouseRegion: 5
    property int onlyDisplayIconWidth: 450
    property int __menuBarType: 1



    minimumWidth: 400
    minimumHeight: 300

    flags: Qt.FramelessWindowHint


    signal sigWindowSizeChanged()

    /*
     * header
     */
    Item{
        id:header
        z:9999
        height: 40
        anchors.topMargin: mouseRegion

        anchors{
            left: center.left
            right: center.right
            top: parent.top
            margins: 0
        }



        KtMouseAreaMove{
            targetFill: header
            targetMove: root
        }

        Rectangle{
            id: rectangle1
            color: "#333333"
            anchors.fill: parent
            Image{
                id: imageMenus
                source: "image/menus.svg"
                width:(__menuBarType==1)?0:24
                height:24
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: rectangle1.left
                anchors.leftMargin: 5
            }
            Image{
                id: rootIcon
                x: imageMenus.x + imageMenus.width + 5
                y: imageMenus.y
                source: "qrc:/image/kt-blue.svg"
                width:24
                height:24
                sourceSize.height: 24
                sourceSize.width: 24
                KtMouseAreaMove{}
            }
            Text {
                id: name    
                x: imageMenus.x + imageMenus.width + rootIcon.width +15
                text: root.title
                color: KtAlarmTheme.colorTextDlg
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Item {
            id: elementWin
            width:rowLayout1.width
            height: header.height

            RowLayout {
                id: rowLayout1
                height:30

                KtToolButton {
                    id: buttonMin
                    Layout.preferredWidth: 55
                    icon.source: "image/win-min.svg"
                    onClicked: {
                        root.hide()
                    }
                }

                KtToolButton {
                    id: buttonMax
                    Layout.preferredWidth: 55
                    icon.source: "image/win-max.svg"
                    visible: false
                }

                KtToolButton {
                    id: buttonClose
                    Layout.preferredWidth: 55
                    icon.source: "image/win-close.svg"
                    onClicked: {
                        root.close()
                    }
                }
            }
            anchors.right: header.right
            anchors.rightMargin: 5
        }

    }

    Item{
        id:center
        anchors.top: header.bottom
        anchors.topMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: mouseRegion
        anchors.bottom: footer.top
        anchors.bottomMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: mouseRegion

        Rectangle{
            id: rectangle
            color: KtAlarmTheme.colorBorder
            anchors.fill: parent
        }
    }

    Item{
        id:footer
        y: 415
        height: 60
        anchors.left: center.left
        anchors.right: center.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: mouseRegion

        Rectangle{
            color: KtAlarmTheme.colorBorder
            anchors.fill: parent
        }
    }
    Item{
        id:borderArea

        property var color: "transparent"
        anchors{
            fill: parent
        }

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
                    var x1 = root.x + xOffset
                    var width1 = root.width -xOffset
                    root.x = x1
                    root.width = width1
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
                    sigWindowSizeChanged()
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
                    sigWindowSizeChanged()
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
                    sigWindowSizeChanged()
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
                    sigWindowSizeChanged()
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
                    sigWindowSizeChanged()
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
                    sigWindowSizeChanged()
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
                    sigWindowSizeChanged()
                }
                var yOffset = mouse.y-yPosition
                var yHeight = root.height+yOffset
                if(yHeight+root.y<root.maximumHeight && yHeight>root.minimumHeight){
                    root.height = yHeight
                }
            }
        }
    }

}



