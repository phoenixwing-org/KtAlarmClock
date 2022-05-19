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
        color: control.up.pressed ? "#3e3837" : KtAlarmTheme.colorBackground
        border.color: KtAlarmTheme.colorBorder
        border.width: 1

        Text {
            text: "+"
            color: enabled?KtAlarmTheme.colorTextDlg:KtAlarmTheme.colorBackground
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
<<<<<<< HEAD
        color: control.down.pressed ? "#3e3837" : KtAlarmTheme.colorBackground
        border.color: KtAlarmTheme.colorBorder
=======
        color: control.down.pressed ? "#3e3837" : "#000000"
        border.color: enabled ? KtAlarmTheme.colorBorder : "#112a3f"
>>>>>>> master
        border.width: 1

        Text {
            text: "-"
            font.pixelSize: control.font.pixelSize * 2
<<<<<<< HEAD
            color: enabled?KtAlarmTheme.colorTextDlg:KtAlarmTheme.colorBackground
=======
            color: KtAlarmTheme.colorTextDlg
>>>>>>> master
            anchors.fill: parent
            fontSizeMode: Text.Fit
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    background: Rectangle {
        implicitWidth: 140
        border.color: KtAlarmTheme.colorBorder
<<<<<<< HEAD
        color: KtAlarmTheme.colorBackground
=======
        color: "#000000"
>>>>>>> master
        border.width: 1
    }
}
