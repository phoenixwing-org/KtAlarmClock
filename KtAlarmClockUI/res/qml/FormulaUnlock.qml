import QtQuick
import QtQuick.Controls.Basic

Item {
    id: root

    signal submitted(string answer, bool requireFormula)

    property bool active: false
    property int failureSerial: 0
    property string formulaText: ""
    property bool requireFormula: true
    property bool transitionsEnabled: true

    visible: opacity > 0
    opacity: active ? 1 : 0

    function open(shouldRequireFormula) {
        requireFormula = shouldRequireFormula
        active = true
        pin.clear()
        Qt.callLater(pin.forceInputFocus)
    }

    function close() {
        active = false
        pin.clear()
    }

    function reset() {
        transitionsEnabled = false
        active = false
        requireFormula = true
        clearAfterReject.stop()
        shakeAnimation.stop()
        cardShake.x = 0
        pin.clear()
        Qt.callLater(function() {
            transitionsEnabled = true
        })
    }

    function reject() {
        if (!active)
            return
        shakeAnimation.restart()
        clearAfterReject.restart()
    }

    onFailureSerialChanged: {
        if (active && failureSerial > 0)
            reject()
    }

    Behavior on opacity {
        enabled: root.transitionsEnabled

        NumberAnimation {
            duration: 180
            easing.type: Easing.OutCubic
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#05080b"
        opacity: 0.58
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.close()
    }

    Rectangle {
        id: card

        width: Math.min(parent.width - 64, 430)
        height: 272
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: 0
        radius: 24
        color: "#ffffff"
        border.color: "#d9dee7"
        border.width: 1
        scale: root.active ? 1 : 0.96
        transform: Translate {
            id: cardShake

            x: 0
        }

        Behavior on scale {
            enabled: root.transitionsEnabled

            NumberAnimation {
                duration: 200
                easing.type: Easing.OutBack
            }
        }

        SequentialAnimation {
            id: shakeAnimation

            NumberAnimation { target: cardShake; property: "x"; to: -10; duration: 38; easing.type: Easing.OutQuad }
            NumberAnimation { target: cardShake; property: "x"; to: 10; duration: 58; easing.type: Easing.InOutQuad }
            NumberAnimation { target: cardShake; property: "x"; to: -7; duration: 52; easing.type: Easing.InOutQuad }
            NumberAnimation { target: cardShake; property: "x"; to: 5; duration: 44; easing.type: Easing.InOutQuad }
            NumberAnimation { target: cardShake; property: "x"; to: 0; duration: 48; easing.type: Easing.OutCubic }
        }

        Timer {
            id: clearAfterReject

            interval: 300
            repeat: false
            onTriggered: {
                pin.clear()
                pin.forceInputFocus()
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: pin.forceInputFocus()
        }

        Column {
            anchors.fill: parent
            anchors.leftMargin: 30
            anchors.rightMargin: 30
            anchors.topMargin: 32
            anchors.bottomMargin: 30
            spacing: 0

            Text {
                text: qsTr("请验证一下")
                color: "#101828"
                font.pixelSize: 21
                font.weight: Font.Bold
                width: parent.width
                height: implicitHeight
                horizontalAlignment: Text.AlignHCenter
            }

            Item {
                width: parent.width
                height: 24
            }

            Rectangle {
                width: parent.width
                height: 72
                radius: 18
                color: "#f1f3f5"
                border.color: "#edf0f4"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: root.formulaText
                    color: "#101828"
                    font.family: "Segoe UI"
                    font.pixelSize: 32
                    font.weight: Font.Bold
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Item {
                width: parent.width
                height: 26
            }

            Item {
                width: parent.width
                height: pin.implicitHeight

                PinCodeInput {
                    id: pin

                    anchors.horizontalCenter: parent.horizontalCenter
                    onCompleted: function(code) {
                        root.submitted(code, root.requireFormula)
                    }
                }
            }
        }
    }
}
