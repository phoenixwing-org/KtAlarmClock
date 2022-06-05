/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import KtAlarmClock 1.0

Item {
    id: root
    width: defaultWidth
    height: 300

    property bool onlyIcon: false
    property int minimumWidth: 50
    property int defaultWidth: 200


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
                id: toolButtonReset
                onlyIcon: root.onlyIcon
                width: root.width
                transformOrigin: Item.Left
                icon.source: "qrc:/image/next.svg"
                font.pointSize: KtAlarmTheme.fontPixelNormal
                action: ActionNextLoop
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
        text: "上课读书"
        shortcut: StandardKey.New
        onTriggered: {
            sigScrollPage(0)
        }
    }

    Action {
        id: actionMyExercise
        text: "下课休息"
        shortcut: StandardKey.Open
        onTriggered: {
            sigScrollPage(1)
        }
    }
    Action {
        id: actionMyForce
        text: "强制休息"
        shortcut: StandardKey.Save
        onTriggered: {
                sigScrollPage(2)
            }
    }

    Action {
        id: actionDefault
        text: "默认设置"
        onTriggered: {
                myAlarmClockParam.setDefault()
                myAlarmClockParam.sigUpdateDialog()
            }

    }

    Action {
        id: actionSave
        text: "保存设置"
        onTriggered: {
                myAlarmClockParam.sigUpdateInfos()
                myAlarmClockParam.registerWrite()
            }
    }

    Action {
        id: actionNextLoop
        text: "从头再来"
    }

    Action {
        id: actionHelp
        text: "在线帮助"
    }

    Action {
        id: actionKtWeb
        text: "锟钛网站"
    }

    signal sigScrollPage(int index)


    onOnlyIconChanged: {
        if (onlyIcon){
            width = minimumWidth
        }
        else{
            width = defaultWidth
        }
    }

    function displayOnlyIcon()
    {
        onlyIcon = true;
    }

    function displayTextBesideIcon()
    {
        //width = ktLeft.defaultWidth;
        onlyIcon = false;
    }

}

