import QtQuick 2.14
import QtQuick.Controls 2.14

SpinBox {
    id: control
    stepSize: 1
    to: 86400
    value: 45
    editable: true

    contentItem: TextInput {
        z: 2
        text: control.textFromValue(control.value, control.locale)

        font: control.font
        color: KtAlarmTheme.colorTextDlg
        selectionColor: "#ffffff"
        selectedTextColor: "#000000"
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        padding: 5


        readOnly: !control.editable
        validator: control.validator
        inputMethodHints: Qt.ImhFormattedNumbersOnly
    }

    up.indicator: Rectangle {
        x: control.mirrored ? 0 : parent.width - width
        height: parent.height
        implicitWidth: 40
        implicitHeight: 40
        color: control.up.pressed ? "#3e3837" : "#000000"
        border.color: enabled ? KtAlarmTheme.colorBorder : "#112a3f"
        border.width: 1

        Text {
            text: "+"
            color: KtAlarmTheme.colorTextDlg
            font.pixelSize: control.font.pixelSize * 2
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    down.indicator: Rectangle {
        x: control.mirrored ? parent.width - width : 0
        height: parent.height
        implicitWidth: 40
        implicitHeight: 40
        color: control.down.pressed ? "#3e3837" : "#000000"
        border.color: enabled ? KtAlarmTheme.colorBorder : "#112a3f"
        border.width: 1


        Text {
            text: "-"
            font.pixelSize: control.font.pixelSize * 2
            color: KtAlarmTheme.colorTextDlg
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    background: Rectangle {
        implicitWidth: 140
        border.color: KtAlarmTheme.colorBorder
        color: "#000000"
        border.width: 1
    }
}
