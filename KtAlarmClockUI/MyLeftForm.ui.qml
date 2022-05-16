
/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12

Item {
    id: root
    width: defaultWidth
    height: 300

    property bool onlyIcon: false
    property int minimumWidth: 50
    property int defaultWidth: 200
    property alias actionMyWork: actionMyWork
    property alias actionMyExercise: actionMyExercise
    property alias actionMyForce: actionMyForce

    Rectangle {
        id: rect
        color: "#242628"
        anchors.fill: parent
    }

    ToolBar {
        id: toolbar
        width: parent.width
        background: Rectangle {
            color: "#242628"
        }

        ColumnLayout {
            width: parent.width
            transformOrigin: Item.Left

            KtToolButton {
                id: toolButtonMyWork
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/book-opened.svg"
                font.pointSize: 14
                action: actionMyWork
            }

            KtToolButton {
                id: toolButtonMyExercise
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/bike.svg"
                font.pointSize: 14
                action: actionMyExercise
            }

            KtToolButton {
                id: toolButtonMyForce
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/cancel.svg"
                font.pointSize: 14
                action: actionMyForce
            }
        }
    }

    Action {
        id: actionMyWork
        text: "&Work Time"
        shortcut: StandardKey.New
    }

    Action {
        id: actionMyExercise
        text: "&Exercise time"
        shortcut: StandardKey.Open
    }
    Action {
        id: actionMyForce
        text: "&Force"
        shortcut: StandardKey.Save
    }
}
