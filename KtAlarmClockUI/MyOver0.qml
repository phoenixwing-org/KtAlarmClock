import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import KtAlarmClock 1.0
import "MyOver0.js" as Over0Js


KtWindowOver {
    id: over0
    width:600
    height:400
    visible: true
    color: KtAlarmTheme.colorBackground

    property int counter: 3600
    property int counterForce: 0
    property double forceEndMs: 0
    property int formulaValue: 3300
    property bool isShowFormula: false
    property bool isForced: false

    signal onClockOut(int state)

    title: "Over Window"
    objectName: "over0"
    screenId: 0

    Label {
        id: labelForce
        z: 50
        anchors.centerIn: parent
        width: parent.width
        visible: isForced && counterForce > 0
        color: KtAlarmTheme.colorText
        text: counterForce
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pointSize: KtAlarmTheme.fontPixelLarge
    }

    Item {
        id: centerContent
        anchors.centerIn: parent
        width: imgCoffee.width
        height: imgCoffee.height + clock.height + 10
        visible: !isForced

        Image {
            id: imgCoffee
            anchors.horizontalCenter: parent.horizontalCenter
            width: 100
            height: 100
            sourceSize.height: 100
            sourceSize.width: 100
            source: "qrc:/image/coffee.svg"

            Image {
                id: imgClockIcon
                x: 35
                y: 56
                width: 24
                height: 24
                source: "qrc:/image/alarm-clock.svg"
            }
        }

        MyClock {
            id: clock
            visible: running
            anchors.top: imgCoffee.bottom
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter

            onRunningChanged: Over0Js.afterClockRunningChanged()
            onCounterChanged: Over0Js.afterClockCounterChanged()
        }
    }

    Item {
        id: unlockPanel
        z: 100
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        visible: !isForced
        width: isShowFormula ? 500 : buttonUnlock.width
        height: unlockColumn.implicitHeight

        Column {
            id: unlockColumn
            spacing: 8
            anchors.right: parent.right

            Label {
                id: labelMsg
                width: unlockPanel.width
                horizontalAlignment: Text.AlignRight
                color: KtAlarmTheme.colorText
                text: qsTr("")
                font.pointSize: KtAlarmTheme.fontPixelLarge
            }

            Item {
                id: element
                visible: isShowFormula
                width: labelFormula.width + rectangleResult.width + 20
                height: 50
                anchors.right: parent.right

                Label {
                    id: labelFormula
                    color: KtAlarmTheme.colorText
                    text: qsTr("2200 + 1100 =")
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: KtAlarmTheme.fontPixelLarge
                    anchors.verticalCenter: parent.verticalCenter
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
                    anchors.verticalCenter: parent.verticalCenter

                    TextEdit {
                        id: textEditResult
                        height: 40
                        color: KtAlarmTheme.colorText
                        text: ""
                        anchors.fill: parent
                        anchors.margins: 4
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
                anchors.right: parent.right
                icon.source: "qrc:/image/unlock.svg"
                icon.height: 50
                icon.width: 50
                text: qsTr("Unlock")
                flat: false
                font.pointSize: KtAlarmTheme.fontPixelLarge
                onClicked: Over0Js.unlockPage()
            }
        }
    }

    Timer {
        id: wallClockTimer
        interval: 1000
        running: visible
        repeat: true
        onTriggered: Over0Js.syncWallClocks()
        onRunningChanged: if (running) Over0Js.syncWallClocks()
    }

    Timer {
        id: raiseTimer
        interval: 5000
        running: visible
        repeat: true
        onTriggered: Over0Js.raiseLockScreens()
        onRunningChanged: if (running) Over0Js.raiseLockScreens()
    }

    Timer {
        id: screenWatchdogTimer
        interval: 10000
        running: visible
        repeat: true
        onTriggered: Over0Js.reconcileLockScreens()
        onRunningChanged: if (running) Over0Js.reconcileLockScreens()
    }

    Connections {
        target: Qt.application
        function onStateChanged(state) {
            if (state === Qt.ApplicationActive) {
                Over0Js.syncWallClocks()
                Over0Js.reconcileLockScreens()
                Over0Js.raiseLockScreens()
            }
        }
    }

    onVisibleChanged: {
        if (visible) {
            Over0Js.syncWallClocks()
            Over0Js.reconcileLockScreens()
            Over0Js.raiseLockScreens()
        } else {
            Over0Js.hideSecondaryScreens()
        }
    }

    Component.onCompleted: {
        Over0Js.initialFormula()
    }

    //@disable-check M16
    onClosing: function(closeEvent){
        console.log("over0 .onClosing(), canClose=",canClose)
        if(canClose){
            onClockOut(KtAlarmClock.WorkBreak)
            Over0Js.hideSecondaryScreens()
        }
    }

    onIsForcedChanged: Over0Js.afterIsForceChanged()

    function customHide() {
        Over0Js.customHide()
    }

    function customShow() {
        Over0Js.customShow()
    }
}
