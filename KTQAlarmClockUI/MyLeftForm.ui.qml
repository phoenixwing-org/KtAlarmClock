
/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    width: defaultWidth
    height: 400

    property int minimunWidth: 50
    property int defaultWidth: 150
    property alias toolButtonMyWork: toolButtonMyWork
    property alias toolButtonMyClass: toolButtonMyClass
    property alias toolButtonMySetting: toolButtonMySetting

    Rectangle{
        id:rect
        color: "#000000"
        anchors.fill: parent

    }

    ToolBar {
        id: toolbar
        width: parent.width
        background: Rectangle{
            color: "#000000"
        }

        ColumnLayout {
            anchors.fill: parent

            ToolButton {
                id: toolButtonMyWork
                transformOrigin: Item.Left
                Layout.fillWidth: true
                display:"TextBesideIcon"
                icon.source: "qrc:/image/volume-loud.svg"
                icon.color: "transparent"
                palette.buttonText:"#FFFFFF"

                background: Rectangle{
                    color: "#000000"
                    border.color: "#000000"
                    border.width: 0
                }
                action: actionMyWork
            }

            ToolButton {
                id: toolButtonMyClass
                Layout.fillWidth: true
                display:"TextBesideIcon"
                icon.source: "qrc:/image/book-opened.svg"
                icon.color: "transparent"
                palette.buttonText:"#FFFFFF"

                background: Rectangle{
                    color: "#000000"
                    border.width: 0;
                    border.color: "#000000";
                }

                action: actionMyClass
            }

            ToolButton {
                id: toolButtonMySetting
                Layout.fillWidth: true
                display:"TextBesideIcon"
                icon.source: "qrc:/image/settings.svg"
                icon.color: "transparent"
                palette.buttonText:"#FFFFFF"

                background: Rectangle{
                    color: "#000000"
                    border.width: 0;
                    border.color: "#000000";
                }

                action: actionSet
            }




        }
    }

    Action {
        id: actionMyWork
        text: "&My Work"
        shortcut: StandardKey.New
    }

    Action {
        id: actionMyClass
        text: "&My Class"
        shortcut: StandardKey.Open
    }
    Action {
        id: actionSet
        text: "&Setting"
        shortcut: StandardKey.Save
    }
}
