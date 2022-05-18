import QtQuick 2.14
import QtQuick.Controls 2.14

KtWindowOver {
    title: qsTr("Second window")
    Label{
        id: labelMsg2
        x:parent.width*0.5
        y:parent.height*0.5
        color: KtAlarmTheme.colorText
        text: "休息吧！\r\n除了工作，还有诗歌和远方！";
        font.pointSize: KtAlarmTheme.fontPixelNormal

        KtMouseAreaMove{}
    }
}
