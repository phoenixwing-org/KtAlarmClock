import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import KtAlarmClock 1.0
import QtGraphicalEffects 1.14
import "MyOver0.js" as Over0Js


KtWindowOver {
    id: over0
    width:600
    height:400
    color: KtAlarmTheme.colorBackground

    property int counter: 3600
    property int counterForce: 0
    property int formulaValue: 3300
    property bool canClose: false
    property bool isShowFormula: false
    property bool isForced: false
    
    signal onClockOut(int state)

    //modality: Qt.WindowModal //Block other windows
    title: "Over Window"
    screenId: 0

    Label {
        id: labelMsg
        color: KtAlarmTheme.colorText
        text: qsTr("")
        anchors.horizontalCenter: footer.horizontalCenter
        anchors.bottom: footer.top
        anchors.bottomMargin: 5
        font.pointSize: KtAlarmTheme.fontPixelLarge
    }

    /**
     * @brief footer counter label
     * @note visible is only controlled by isForced
     */
    Rectangle {
        id: footer
        x: (over0.width - width) * 0.5
        y: over0.height - 100
        width: isShowFormula? 500 : buttonUnlock.width
        height: 80
        color: KtAlarmTheme.colorBackground
        visible: !isForced

        KtMouseAreaMove{}

        Item {
            id: element
            visible: isShowFormula
            width: labelFormula.width + rectangleResult.width + 20
            height: 50
            clip: false
            anchors.horizontalCenter: footer.horizontalCenter
            anchors.verticalCenter: footer.verticalCenter

            Label {
                id: labelFormula
                color: KtAlarmTheme.colorText
                text: qsTr("2200 + 1100 =")
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: KtAlarmTheme.fontPixelLarge

                anchors.verticalCenter: element.verticalCenter
            }

            Rectangle {
                id: rectangleResult
                width: 80
                height: 40
                anchors.left: labelFormula.right
                anchors.leftMargin: 10
                color: "#112a3f"
                border.color: KtAlarmTheme.colorBorder
                border.width: 1
                visible: true

                anchors.verticalCenter: element.verticalCenter
                TextEdit {
                    id: textEditResult
                    x: 159
                    y: 15
                    height: 40
                    color: KtAlarmTheme.colorText
                    text: ""
                    anchors.rightMargin: 5
                    anchors.leftMargin: 0
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.left: parent.left
                    font.family: "Arial"
                    font.pixelSize: 30
                    wrapMode: Text.NoWrap
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        KtToolButton {
            id: buttonUnlock
            icon.source: "qrc:/image/unlock.svg"
            icon.height: 50
            icon.width: 50
            visible: true
            text: qsTr("Unlock")
            flat: false
            font.pointSize: KtAlarmTheme.fontPixelLarge
            
            anchors.verticalCenter: footer.verticalCenter
            anchors.right: footer.right
            anchors.rightMargin: 5
            onClicked: Over0Js.unlockPage()
        }
    }

    /**
     * @brief time counter label
     * @note visible is only controlled by counterForce > 0
     */
    Label {
        id: labelForce
        width: parent.width
        visible: counterForce>0
        color: KtAlarmTheme.colorText
        text: counterForce
        anchors.horizontalCenter: footer.horizontalCenter
        anchors.verticalCenter: footer.verticalCenter
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: KtAlarmTheme.fontPixelNormal
    }

    Image {
        id: imgCoffee
        x: (over0.width -width ) * 0.5
        y: (over0.height - height) * 0.5
        width: 100
        height: 100
        sourceSize.height: 100
        sourceSize.width: 100
        source: "qrc:/image/coffee.svg"
        KtMouseAreaMove{}

        Image {
            id: imgClock
            x: 35
            y: 56
            width: 24
            height: 24
            source: "qrc:/image/alarm-clock.svg"
            KtMouseAreaMove{}
        }
    }

    /**
     * @brief clock item
     * @note visible is only controlled by running
     */
    MyClock{
        id: clock
        visible: running
        x: imgCoffee.x + 5
        y: imgCoffee.y + imgCoffee.height + 10
        KtMouseAreaMove{}

        onRunningChanged: Over0Js.afterClockRunningChanged()
        onCounterChanged: Over0Js.afterClockCounterChanged()
    }

    MyOver1 {
        id: over1
        canClose: over0.canClose
        title: qsTr("Over Window 1")
        screenId: 1
    }


    Component.onCompleted: {
        Over0Js.initialFormula()
    }

    //@disable-check M16
    onClosing: function(closeEvent){
        closeEvent.accepted = canClose
    }
    
    onVisibleChanged: {
        //if over0 hide, hide the second one, duplicate command
        if(!visible) over1.hide();
    }

    onIsForcedChanged: Over0Js.afterIsForceChanged()
    
    function customHide() {
        Over0Js.customHide()
    }   

    function customShow() {
        Over0Js.customShow()
    }
    
}
