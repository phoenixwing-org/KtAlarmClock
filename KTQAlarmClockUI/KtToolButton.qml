import QtQuick 2.4
import QtQuick.Controls 2.12

ToolButton {
    width: 150
    height: 35
    property bool onlyIcon: true
    display: onlyIcon ? Qt.ToolButtonIconOnly : Qt.ToolButtonTextBesideIcon

    //icon.source: ""
    icon.color: "transparent"

    palette.buttonText:"#FFFFFF"

    background: Rectangle{
        color: "transparent"
        // border.color: "#202020"
        border.width: 0
    }
}

