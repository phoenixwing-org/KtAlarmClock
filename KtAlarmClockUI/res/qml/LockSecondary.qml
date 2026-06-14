import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    signal unlockRequested()

    property string forceText: ""
    property bool forced: true
    property int screenId: 0
    property bool unlockVisible: false

    color: "#05080b"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 0

        Item {
            Layout.fillHeight: true
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 14

            Item {
                Layout.preferredWidth: 128
                Layout.preferredHeight: 128
                Layout.alignment: Qt.AlignHCenter

                Image {
                    anchors.centerIn: parent
                    source: "qrc:/image/coffee.svg"
                    sourceSize.width: 78
                    sourceSize.height: 78
                    fillMode: Image.PreserveAspectFit
                }
            }

            Text {
                text: root.forced ? qsTr("休息一下") : qsTr("准备好就回来")
                color: "#eef6ff"
                font.pixelSize: 24
                font.weight: Font.DemiBold
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignHCenter
            }

            Item {
                Layout.preferredWidth: 280
                Layout.preferredHeight: 58
                Layout.alignment: Qt.AlignHCenter

                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 6
                    opacity: root.forced ? 1 : 0
                    scale: root.forced ? 1 : 0.96
                    visible: opacity > 0.01

                    Behavior on opacity {
                        NumberAnimation { duration: 220; easing.type: Easing.OutCubic }
                    }
                    Behavior on scale {
                        NumberAnimation { duration: 220; easing.type: Easing.OutCubic }
                    }

                    Text {
                        text: root.forceText.length > 0 ? root.forceText : "0:00"
                        color: "#46aef6"
                        font.family: "Segoe UI"
                        font.pixelSize: 20
                        font.weight: Font.Medium
                        horizontalAlignment: Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }
                }

                SlideUnlock {
                    anchors.centerIn: parent
                    visible: opacity > 0.01
                    opacity: root.unlockVisible ? 1 : 0
                    scale: root.unlockVisible ? 1 : 0.96
                    text: qsTr("滑动恢复")
                    completedText: "松开继续"
                    onAccepted: root.unlockRequested()

                    Behavior on opacity {
                        NumberAnimation { duration: 240; easing.type: Easing.OutCubic }
                    }
                    Behavior on scale {
                        NumberAnimation { duration: 240; easing.type: Easing.OutBack }
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
