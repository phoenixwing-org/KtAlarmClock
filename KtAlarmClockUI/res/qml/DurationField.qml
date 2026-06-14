import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

Item {
    id: root

    property int value: 0
    property int from: 0
    property int to: 3600
    property int stepSize: 1
    property string label: ""
    property string description: ""

    Layout.fillWidth: true
    implicitHeight: 38

    function pad2(number) {
        return number < 10 ? "0" + number : "" + number
    }

    function formatted(seconds) {
        const safeSeconds = Math.max(0, seconds)
        const minutes = Math.floor(safeSeconds / 60)
        const restSeconds = safeSeconds % 60
        return pad2(minutes) + ":" + pad2(restSeconds)
    }

    function normalize(candidate) {
        const bounded = Math.max(root.from, Math.min(root.to, candidate))
        const offset = bounded - root.from
        const steps = Math.round(offset / root.stepSize)
        return Math.max(root.from, Math.min(root.to, root.from + steps * root.stepSize))
    }

    function adjust(delta) {
        root.value = normalize(root.value + delta)
    }

    function parseDuration(text) {
        const trimmed = text.trim()
        if (trimmed.length === 0)
            return root.value

        const parts = trimmed.split(":")
        if (parts.length === 1) {
            const seconds = Number.parseInt(parts[0], 10)
            return Number.isNaN(seconds) ? root.value : seconds
        }

        if (parts.length !== 2)
            return root.value

        const minutes = parts[0].length === 0 ? 0 : Number.parseInt(parts[0], 10)
        const secondsPart = parts[1].length === 0 ? 0 : Number.parseInt(parts[1], 10)
        if (Number.isNaN(minutes) || Number.isNaN(secondsPart) || secondsPart > 59)
            return root.value

        return minutes * 60 + secondsPart
    }

    function commitText() {
        root.value = root.normalize(parseDuration(valueInput.text))
        valueInput.text = root.formatted(root.value)
    }

    onValueChanged: {
        if (!valueInput.activeFocus)
            valueInput.text = root.formatted(root.value)
    }

    Component.onCompleted: valueInput.text = root.formatted(root.value)

    RowLayout {
        anchors.fill: parent
        spacing: 8

        ColumnLayout {
            spacing: 1
            Layout.fillWidth: true

            Text {
                text: root.label
                color: "#101828"
                font.pixelSize: 12
                font.weight: Font.DemiBold
                Layout.fillWidth: true
            }

            Text {
                text: root.description
                color: "#667085"
                font.pixelSize: 10
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
        }

        Button {
            id: minusButton

            text: "-"
            enabled: root.value > root.from
            implicitWidth: 32
            implicitHeight: 32
            onClicked: root.adjust(-root.stepSize)

            contentItem: Text {
                text: minusButton.text
                color: minusButton.enabled ? "#101828" : "#98a2b3"
                font.pixelSize: 17
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                radius: 9
                color: minusButton.down ? "#e3e8ef" : (minusButton.hovered ? "#eef2f6" : "#f1f3f5")
                border.color: "#d9dee7"
                border.width: 1
            }
        }

        Rectangle {
            id: valuePill

            Layout.preferredWidth: 76
            Layout.preferredHeight: 32
            radius: 9
            property bool dragging: false
            color: dragging ? "#e7f5ff" : (valueInput.activeFocus ? "#ffffff" : "#eef1f4")
            border.color: dragging || valueInput.activeFocus ? "#74c0fc" : "transparent"
            border.width: dragging || valueInput.activeFocus ? 1 : 0

            TextInput {
                id: valueInput

                anchors.fill: parent
                anchors.leftMargin: 6
                anchors.rightMargin: 6
                text: root.formatted(root.value)
                color: "#101828"
                font.family: "Segoe UI"
                font.pixelSize: 13
                font.weight: Font.DemiBold
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                selectByMouse: true
                activeFocusOnPress: false
                maximumLength: 5
                inputMethodHints: Qt.ImhFormattedNumbersOnly

                validator: RegularExpressionValidator {
                    regularExpression: /[0-9:]{0,5}/
                }

                onActiveFocusChanged: {
                    if (activeFocus)
                        selectAll()
                    else
                        root.commitText()
                }

                Keys.onReturnPressed: function(event) {
                    root.commitText()
                    focus = false
                    event.accepted = true
                }

                Keys.onEnterPressed: function(event) {
                    root.commitText()
                    focus = false
                    event.accepted = true
                }
            }

            MouseArea {
                anchors.fill: parent
                enabled: !valueInput.activeFocus
                acceptedButtons: Qt.LeftButton
                cursorShape: Qt.IBeamCursor

                property real startX: 0
                property int startValue: 0

                onPressed: function(mouse) {
                    startX = mouse.x
                    startValue = root.value
                    valuePill.dragging = false
                }

                onPositionChanged: function(mouse) {
                    if (!(mouse.buttons & Qt.LeftButton))
                        return

                    const delta = Math.round(mouse.x - startX)
                    if (Math.abs(delta) <= 1)
                        return

                    valuePill.dragging = true
                    root.value = root.normalize(startValue + delta)
                }

                onReleased: {
                    if (valuePill.dragging)
                        valueInput.text = root.formatted(root.value)
                    valuePill.dragging = false
                }

                onDoubleClicked: function(mouse) {
                    valuePill.dragging = false
                    valueInput.forceActiveFocus()
                    valueInput.selectAll()
                    mouse.accepted = true
                }
            }
        }

        Button {
            id: plusButton

            text: "+"
            enabled: root.value < root.to
            implicitWidth: 32
            implicitHeight: 32
            onClicked: root.adjust(root.stepSize)

            contentItem: Text {
                text: plusButton.text
                color: plusButton.enabled ? "#101828" : "#98a2b3"
                font.pixelSize: 17
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                radius: 9
                color: plusButton.down ? "#e3e8ef" : (plusButton.hovered ? "#eef2f6" : "#f1f3f5")
                border.color: "#d9dee7"
                border.width: 1
            }
        }
    }

}
