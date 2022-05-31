
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
    property alias actionDefault: actionDefault
    property alias actionSave: actionSave

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
                width: defaultWidth
                transformOrigin: Item.Left
                icon.source: "qrc:/image/book-opened.svg"
                font.pointSize: KtAlarmTheme.fontPixelNormal

                action: actionMyWork
            }

            KtToolButton {
                id: toolButtonMyExercise
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/bike.svg"
                font.pointSize: KtAlarmTheme.fontPixelNormal
                action: actionMyExercise
            }

            KtToolButton {
                id: toolButtonMyForce
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/cancel.svg"
                font.pointSize: KtAlarmTheme.fontPixelNormal
                action: actionMyForce
            }

            KtToolButton {
                id: toolButtonDefault
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/KtCloudLoad.svg"
                font.pointSize: KtAlarmTheme.fontPixelNormal
                action: actionDefault
            }

            KtToolButton {
                id: toolButtonSave
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/KtSave.svg"
                font.pointSize: KtAlarmTheme.fontPixelNormal
                action: actionSave
            }
        }
    }

    Action {
        id: actionMyWork
        text: "工作时长"
        shortcut: StandardKey.New
    }

    Action {
        id: actionMyExercise
        text: "休息时长"
        shortcut: StandardKey.Open
    }
    Action {
        id: actionMyForce
        text: "强制休息时长"
        shortcut: StandardKey.Save
    }
    
    Action {
        id: actionDefault
        text: "恢复默认设置"
    }  

    Action {
        id: actionSave
        text: "保存当前设置"
    }
}
