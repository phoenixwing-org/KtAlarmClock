import QtQuick 2.14
import QtQuick.Controls 2.14

KtWindowOver {
    id: over1
    title: qsTr("Over 1")
    Label{
        id: labelMsg
        x:parent.width*0.5
        y:parent.height*0.5
        color: KtAlarmTheme.colorText
        text: "Have a rest! Except work, there are poetry and afar！"
        font.pointSize: KtAlarmTheme.fontPixelNormal

        KtMouseAreaMove{}
    }
}
