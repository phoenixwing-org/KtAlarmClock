import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts
import QtQuick.Window

Rectangle {
    id: root

    property Window windowTarget
    property real dragPressX: 0
    property real dragPressY: 0

    radius: 14
    color: "#ffffff"
    border.color: "#dee2e6"
    border.width: 1
    focus: true

    implicitWidth: 320
    implicitHeight: 286

    function loadFromModel() {
        workField.value = alarmModel.workSeconds
        breakField.value = alarmModel.breakSeconds
        forceField.value = alarmModel.forceSeconds
    }

    function openActionMenu() {
        actionMenu.x = root.width - actionMenu.width - 16
        actionMenu.y = root.height - 56 - actionMenu.height - 6
        actionMenu.open()
    }

    function clearEditorFocus() {
        forceActiveFocus()
        Qt.inputMethod.hide()
    }

    onVisibleChanged: {
        if (visible)
            loadFromModel()
    }

    Connections {
        target: alarmModel
        function onWorkSecondsChanged() { if (root.visible) workField.value = alarmModel.workSeconds }
        function onBreakSecondsChanged() { if (root.visible) breakField.value = alarmModel.breakSeconds }
        function onForceSecondsChanged() { if (root.visible) forceField.value = alarmModel.forceSeconds }
    }

    FontLoader {
        id: codiconFont

        source: "qrc:/font/codicon-font-v0.0.37-1.ttf"
    }

    component HeaderButton: Button {
        id: button

        property bool chromeless: false
        property color normalColor: "#f8f9fa"
        property color hoverColor: "#eef2f6"
        property color pressedColor: "#e3e8ef"
        property color textColor: "#475467"
        property int contentHorizontalAlignment: Text.AlignHCenter
        property int iconPixelSize: 15

        implicitWidth: 32
        implicitHeight: 32
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0
        leftInset: 0
        rightInset: 0
        topInset: 0
        bottomInset: 0

        contentItem: Text {
            text: button.text
            color: button.textColor
            font.family: codiconFont.name
            font.pixelSize: button.iconPixelSize
            font.weight: Font.DemiBold
            horizontalAlignment: button.contentHorizontalAlignment
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            radius: 11
            color: button.chromeless ? "transparent" : (button.down ? button.pressedColor : (button.hovered ? button.hoverColor : button.normalColor))
            border.color: button.chromeless ? "transparent" : "#d9e2ec"
            border.width: button.chromeless ? 0 : 1
        }
    }

    component PrimaryButton: Button {
        id: button

        implicitHeight: 36

        contentItem: Text {
            text: button.text
            color: "#ffffff"
            font.pixelSize: 12
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            radius: 9
            color: button.down ? "#1864ab" : (button.hovered ? "#1971c2" : "#228be6")
        }
    }

    component LanguageSwitch: Rectangle {
        id: langSwitch

        implicitWidth: 76
        implicitHeight: 36
        radius: 9
        color: "#f1f3f5"
        border.color: "#d9e2ec"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 3
            spacing: 3

            Repeater {
                model: [
                    { label: "EN", locale: "en_US" },
                    { label: "\u4e2d", locale: "zh_CN" }
                ]

                delegate: Item {
                    id: segment

                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    readonly property string labelText: modelData.label
                    readonly property string localeName: modelData.locale
                    readonly property bool selected: alarmModel.locale === localeName

                    Rectangle {
                        anchors.fill: parent
                        radius: 7
                        color: segment.selected ? "#ffffff" : "transparent"
                        border.color: segment.selected ? "#d9e2ec" : "transparent"
                        border.width: segment.selected ? 1 : 0
                    }

                    Text {
                        anchors.centerIn: parent
                        text: segment.labelText
                        color: segment.selected ? "#101828" : "#667085"
                        font.pixelSize: 11
                        font.weight: segment.selected ? Font.DemiBold : Font.Medium
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: alarmController.setLocale(segment.localeName)
                    }
                }
            }
        }
    }

    component ActionMenuItem: Rectangle {
        id: item

        signal triggered()

        property string iconText: ""
        property string text: ""
        property bool destructive: false

        implicitHeight: 30
        Layout.preferredHeight: 30
        radius: 7
        color: rowMouse.containsMouse ? "#f1f3f5" : "transparent"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            spacing: 8

            Text {
                text: item.iconText
                color: item.destructive ? "#c92a2a" : "#536173"
                font.family: codiconFont.name
                font.pixelSize: 13
                Layout.preferredWidth: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                text: item.text
                color: item.destructive ? "#c92a2a" : "#344054"
                font.pixelSize: 11
                Layout.fillWidth: true
                verticalAlignment: Text.AlignVCenter
            }
        }

        MouseArea {
            id: rowMouse

            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                actionMenu.close()
                item.triggered()
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onPressed: root.clearEditorFocus()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 46

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                onPressed: function(mouse) {
                    root.clearEditorFocus()
                    root.dragPressX = mouse.x
                    root.dragPressY = mouse.y
                }
                onPositionChanged: function(mouse) {
                    if (!(mouse.buttons & Qt.LeftButton) || !root.windowTarget)
                        return
                    const currentCenterX = Math.round(root.windowTarget.x + root.windowTarget.width / 2)
                    const currentCenterY = Math.round(root.windowTarget.y + root.windowTarget.height / 2)
                    const next = alarmController.clampWindowPositionOnCurrentScreen(
                        Math.round(root.windowTarget.x + mouse.x - root.dragPressX),
                        Math.round(root.windowTarget.y + mouse.y - root.dragPressY),
                        Math.round(root.windowTarget.width),
                        Math.round(root.windowTarget.height),
                        currentCenterX,
                        currentCenterY
                    )
                    root.windowTarget.x = next.x
                    root.windowTarget.y = next.y
                }
            }

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 18
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 58
                text: qsTr("屏幕闹钟 · 番茄节奏")
                color: "#101828"
                font.pixelSize: 16
                font.weight: Font.Bold
                elide: Text.ElideRight
            }

            HeaderButton {
                anchors.right: parent.right
                anchors.rightMargin: 14
                anchors.verticalCenter: parent.verticalCenter
                chromeless: true
                text: "\uea76"
                textColor: hovered ? "#101828" : "#667085"
                iconPixelSize: 20
                onClicked: alarmController.closeSettings()
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#e9ecef"
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 18
            Layout.rightMargin: 18
            Layout.topMargin: 14
            Layout.bottomMargin: 14
            spacing: 12

            DurationField {
                id: workField
                label: qsTr("工作时长")
                description: qsTr("专注工作的时间")
                from: 60
                to: 5400
                stepSize: 1
            }

            DurationField {
                id: breakField
                label: qsTr("休息时长")
                description: qsTr("建议休息的时间")
                from: 10
                to: 3600
                stepSize: 1
            }

            DurationField {
                id: forceField
                label: qsTr("强制休息")
                description: qsTr("无法跳过的时间")
                from: 0
                to: 3600
                stepSize: 1
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#e9ecef"
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            Layout.leftMargin: 18
            Layout.rightMargin: 18
            spacing: 6

            PrimaryButton {
                text: qsTr("保存并开始")
                Layout.fillWidth: true
                Layout.preferredHeight: 36
                onClicked: alarmController.saveSettingsAndStart(workField.value, breakField.value, forceField.value)
            }

            LanguageSwitch {
                Layout.preferredWidth: 76
                Layout.preferredHeight: 36
            }

            HeaderButton {
                id: moreButton

                Layout.preferredWidth: 22
                Layout.preferredHeight: 36
                implicitWidth: 22
                implicitHeight: 36
                chromeless: true
                text: "\ueb10"
                contentHorizontalAlignment: Text.AlignRight
                textColor: hovered ? "#101828" : "#667085"
                iconPixelSize: 15
                onClicked: root.openActionMenu()
            }
        }
    }

    Popup {
        id: actionMenu

        width: 164
        height: menuColumn.implicitHeight + 12
        modal: false
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        padding: 6

        background: Rectangle {
            radius: 10
            color: "#ffffff"
            border.color: "#dee2e6"
            border.width: 1
        }

        contentItem: ColumnLayout {
            id: menuColumn

            spacing: 2

            ActionMenuItem {
                Layout.fillWidth: true
                iconText: "\uec15"
                text: qsTr("立刻休息")
                onTriggered: alarmController.startBreak()
            }

            ActionMenuItem {
                Layout.fillWidth: true
                iconText: "\ueb8f"
                text: qsTr("后退 1 分钟")
                onTriggered: alarmController.adjustRemaining(60)
            }

            ActionMenuItem {
                Layout.fillWidth: true
                iconText: alarmModel.running ? "\uead1" : "\ueb2c"
                text: alarmModel.running ? qsTr("暂停") : qsTr("开始")
                onTriggered: alarmController.playPause()
            }

            ActionMenuItem {
                Layout.fillWidth: true
                iconText: "\uea9c"
                text: qsTr("前进 1 分钟")
                onTriggered: alarmController.adjustRemaining(-60)
            }

            ActionMenuItem {
                Layout.fillWidth: true
                iconText: "\uea77"
                text: qsTr("下一轮")
                onTriggered: alarmController.nextLoop()
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                Layout.topMargin: 4
                Layout.bottomMargin: 4
                color: "#edf2f7"
            }

            ActionMenuItem {
                Layout.fillWidth: true
                iconText: "\ueb37"
                text: qsTr("恢复默认")
                onTriggered: alarmController.resetSettings()
            }
        }
    }
}
