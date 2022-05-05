
/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: root
    width: defaultWidth
    height: 400

    property bool onlyIcon: false
    property int minimunWidth: 50
    property int defaultWidth: 200
    property alias actionMyWork: actionMyWork
    property alias actionMyClass: actionMyClass
    property alias actionSet: actionSet

    Rectangle {
        id: rect
        color: "#000000"
        anchors.fill: parent
    }

    ToolBar {
        id: toolbar
        width: parent.width
        background: Rectangle {
            color: "#000000"
        }

        ColumnLayout {
            transformOrigin: Item.Left

            KtToolButton {
                id: toolButtonMyWork
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/volume-loud.svg"
                action: actionMyWork
            }

            KtToolButton {
                id: toolButtonMyClass
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/book-opened.svg"
                action: actionMyClass
            }

            KtToolButton {
                id: toolButtonMySetting
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/settings.svg"
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
