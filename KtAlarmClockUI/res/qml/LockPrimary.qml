import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Effects
import QtQuick.Layouts

Rectangle {
    id: root

    signal answerSubmitted(string answer, bool requireFormula)
    signal debugExitRequested()
    signal debugSizeToggleRequested()

    property bool debugControlsVisible: false
    property string debugSizeText: "Half"
    property bool forced: true
    property string forceText: ""
    property string formulaText: ""
    readonly property bool formulaEntryActive: formulaUnlock.active
    property bool formulaVisible: false
    property int screenId: 0
    property int unlockFailureSerial: 0
    property bool unlockVisible: false

    color: "#05080b"
    focus: true

    function prepareForShow() {
        unlockVisible = false
        formulaVisible = false
        formulaUnlock.reset()
        unlockSlider.reset()
        forceActiveFocus()
    }

    Keys.onPressed: function(event) {
        if (event.key === Qt.Key_Escape && formulaUnlock.active) {
            formulaUnlock.close()
            event.accepted = true
        }
    }

    Item {
        id: restScene

        anchors.fill: parent
        opacity: formulaUnlock.active ? 0.30 : 1
        scale: formulaUnlock.active ? 0.985 : 1

        Behavior on opacity {
            NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
        }
        Behavior on scale {
            NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 0

            RowLayout {
                visible: root.debugControlsVisible
                Layout.fillWidth: true
                spacing: 10

                Button {
                    id: debugExitButton

                    text: qsTr("Debug Exit")
                    onClicked: root.debugExitRequested()

                    background: Rectangle {
                        radius: 8
                        color: debugExitButton.down ? "#522528" : (debugExitButton.hovered ? "#693036" : "#341b1e")
                        border.color: "#ff8a8a"
                        border.width: 1
                    }

                    contentItem: Text {
                        text: debugExitButton.text
                        color: "#ffd8d8"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    id: debugSizeButton

                    text: root.debugSizeText
                    onClicked: root.debugSizeToggleRequested()

                    background: Rectangle {
                        radius: 8
                        color: debugSizeButton.down ? "#274766" : (debugSizeButton.hovered ? "#315b82" : "#17283a")
                        border.color: "#58c7ff"
                        border.width: 1
                    }

                    contentItem: Text {
                        text: debugSizeButton.text
                        color: "#d9f1ff"
                        font.pixelSize: 13
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }

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
                    Layout.preferredWidth: 300
                    Layout.preferredHeight: 58
                    Layout.alignment: Qt.AlignHCenter

                    ColumnLayout {
                        id: forceCountdown

                        anchors.centerIn: parent
                        spacing: 8
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
                        id: unlockSlider

                        anchors.centerIn: parent
                        visible: opacity > 0.01
                        opacity: root.unlockVisible ? 1 : 0
                        scale: root.unlockVisible ? 1 : 0.96
                        text: root.formulaVisible ? qsTr("滑动验证") : qsTr("滑动恢复")
                        completedText: root.formulaVisible ? "松开输入答案" : "松开恢复工作"
                        onAccepted: {
                            if (root.formulaVisible)
                                formulaUnlock.open(true)
                            else
                                root.answerSubmitted("", false)
                        }

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

    MultiEffect {
        anchors.fill: restScene
        source: restScene
        visible: formulaUnlock.active
        blurEnabled: true
        blur: 0.9
        opacity: formulaUnlock.active ? 1 : 0
    }

    FormulaUnlock {
        id: formulaUnlock

        anchors.fill: parent
        formulaText: root.formulaText
        failureSerial: root.unlockFailureSerial
        onSubmitted: function(answer, requireFormula) {
            root.answerSubmitted(answer, requireFormula)
        }
    }

    onUnlockVisibleChanged: {
        if (!unlockVisible)
            formulaUnlock.close()
    }

    Component.onCompleted: forceActiveFocus()
}
