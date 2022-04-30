
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
    property alias toolButtonMySpeech: toolButtonMySpeech
    property alias toolButtonMyClass: toolButtonMyClass
    property alias toolButtonMySetting: toolButtonMySetting

    ToolBar {
        id: toolbar
        width: parent.width
        height: 200
        ColumnLayout {
            anchors.fill: parent
            ToolButton {
                id: toolButtonMySpeech
                transformOrigin: Item.Left
                Layout.fillWidth: true
                icon.source: "qrc:/image/volume-loud.svg"
                action: actionMySpeech
            }
            ToolButton {
                id: toolButtonMyClass
                Layout.fillWidth: true
                icon.source: "qrc:/image/book-opened.svg"
                action: actionMyClass
            }
            ToolButton {
                id: toolButtonMySetting
                Layout.fillWidth: true
                icon.source: "qrc:/image/settings.svg"
                action: actionSet
            }
        }
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
