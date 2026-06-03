import QtQuick 2.14
import QtQuick.Controls 2.14

KtWindowOver {
    id: root
    title: qsTr("Over Window Secondary")
    visible: true
    objectName: "overSecondary"
    signal sendClose()

    property bool isForced: false
    property int counterForce: 0

    Label {
        anchors.centerIn: parent
        width: parent.width - 40
        horizontalAlignment: Text.AlignHCenter
        color: KtAlarmTheme.colorText
        font.pointSize: KtAlarmTheme.fontPixelNormal
        text: qsTr("Have a rest! Except work, there are poetry and afar!")
        wrapMode: Text.WordWrap
        visible: !isForced
    }

    Label {
        anchors.centerIn: parent
        color: KtAlarmTheme.colorText
        font.pointSize: KtAlarmTheme.fontPixelLarge
        text: counterForce
        visible: isForced && counterForce > 0
    }

    KtToolButton {
        z: 100
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        visible: !isForced
        icon.source: "qrc:/image/unlock.svg"
        icon.height: 50
        icon.width: 50
        text: qsTr("Unlock")
        flat: false
        font.pointSize: KtAlarmTheme.fontPixelLarge
        onClicked: sendClose()
    }
}
