/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    width: 250
    height: 400

    ToolBar {
        id: toolbar
        width: parent.width
        height: 200
        ColumnLayout {
            anchors.fill: parent
            ToolButton {
                Layout.fillWidth: true
                action: actionMySpeech
            }
            ToolButton {
                Layout.fillWidth: true
                action: actionMyClass
            }
            ToolButton {
                Layout.fillWidth: true
                action: actionSet
            }
        }
    }

    Text {
        id: element
        text: qsTr("Left")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        anchors.top: toolbar.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.topMargin: 0
        font.pixelSize: 22
    }

    Action {
        id: actionMySpeech
        text: "&My Speech"
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
