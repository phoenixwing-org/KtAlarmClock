import QtQuick 2.14
import QtQuick.Controls 2.14

ToolButton {
    id:root
    property bool onlyIcon: true
    display: onlyIcon ? Qt.ToolButtonIconOnly : Qt.ToolButtonTextBesideIcon

    //icon
    icon.color: "transparent"

    //@disable-check M17
    palette.buttonText:"#FFFFFF"

    background: Rectangle{
        color: root.hovered? "#555555":"transparent"
        border.width: 0
    }
}
