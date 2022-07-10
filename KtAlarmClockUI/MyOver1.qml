import QtQuick 2.14
import QtQuick.Controls 2.14

KtWindowOver {
    title: qsTr("Over Window 1")
    visible: true
    objectName: "over1"

    Label{
        id: labelMsg
        x: 10
        y: parent.height -50
        color: KtAlarmTheme.colorText
        font.pointSize: KtAlarmTheme.fontPixelNormal

        text: qsTr("Have a rest! Except work, there are poetry and afar!")

        KtMouseAreaMove{}
    }
}
