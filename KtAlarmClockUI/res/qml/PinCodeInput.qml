import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Layouts

FocusScope {
    id: root

    signal completed(string code)

    property int digits: 4
    property color accentColor: "#228be6"
    property alias text: hiddenInput.text

    implicitWidth: row.implicitWidth
    implicitHeight: row.implicitHeight

    function clear() {
        hiddenInput.text = ""
    }

    function forceInputFocus() {
        hiddenInput.forceActiveFocus()
    }

    RowLayout {
        id: row

        anchors.fill: parent
        spacing: 10

        Repeater {
            model: root.digits

            Rectangle {
                Layout.preferredWidth: 48
                Layout.preferredHeight: 56
                radius: 12
                color: "#f8f9fa"
                border.color: hiddenInput.activeFocus && index === hiddenInput.text.length ? root.accentColor : "#d9dee7"
                border.width: hiddenInput.activeFocus && index === hiddenInput.text.length ? 2 : 1

                Text {
                    anchors.centerIn: parent
                    text: index < hiddenInput.text.length ? hiddenInput.text[index] : ""
                    color: "#101828"
                    font.pixelSize: 22
                    font.weight: Font.DemiBold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    onClicked: root.forceInputFocus()
                }
            }
        }
    }

    TextInput {
        id: hiddenInput

        anchors.fill: parent
        opacity: 0
        focus: true
        inputMethodHints: Qt.ImhDigitsOnly
        validator: RegularExpressionValidator {
            regularExpression: /^[0-9]{0,4}$/
        }
        maximumLength: root.digits

        onTextChanged: {
            if (text.length === root.digits) {
                const answer = text
                root.completed(answer)
            }
        }
    }
}
