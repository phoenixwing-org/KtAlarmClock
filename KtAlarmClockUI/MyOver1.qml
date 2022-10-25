import QtQuick 2.14
import QtQuick.Controls 2.14

KtWindowOver {
    title: qsTr("Over Window 1")
    visible: true
    objectName: "over1"
    signal sendClose();
    
    KtToolButton {
        x: 20
        y:20
        icon.source: "qrc:/image/unlock.svg"
        icon.height: 50
        icon.width: 50
        text: qsTr("Unlock")
        flat: false
        font.pointSize: KtAlarmTheme.fontPixelLarge
        
        anchors.verticalCenter: footer.verticalCenter
        anchors.right: footer.right
        anchors.rightMargin: 5
        onClicked: sendClose();
    }
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
