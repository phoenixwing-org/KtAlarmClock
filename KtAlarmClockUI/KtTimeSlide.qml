import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.0

Rectangle {
    id: root
    width: 200
    height: 120
    color: "black"

    property int value: 0 // seconds
    property int from: 0 // seconds
    property int to: 86400 // seconds
    property int stepSize: 10 // seconds
    property string title: ""


    ColumnLayout {
        id: rootLayout
        height: 108
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        spacing: 5

        Item{
            id:elementTitle

            Layout.fillHeight: true
            Layout.fillWidth: true
            Label {
                id: label0
                height: 16
                color: KtAlarmTheme.colorTextDlg
                text: title
                anchors.verticalCenter: parent.verticalCenter
                Layout.fillWidth: false
                Layout.fillHeight: true
                font.pointSize: KtAlarmTheme.fontPixelNormal
                Layout.maximumWidth: 80
                Layout.preferredWidth: 60
                Layout.minimumWidth: 60
                verticalAlignment: Text.AlignVCenter
            }
            Label {
                id: labelTime
                x: 144
                y: 13
                height: 16
                color: KtAlarmTheme.colorTextDlg
                text: "0:00"

                anchors.bottom: elementTitle.bottom
                anchors.bottomMargin: 2
                anchors.right: elementTitle.right
                anchors.rightMargin: 5

                font.pointSize: KtAlarmTheme.fontPixelNormal
                verticalAlignment: Text.AlignVCenter
            }
        }

        KtSpinBox{
            id: ktSpinBox
            value: root.value
            from: root.from
            to: root.to
            stepSize: root.stepSize

            width: root.width
            Layout.fillHeight: false
            Layout.fillWidth: true
            onValueChanged: {
                if(root.value != ktSpinBox.value){
                    let v1 = ktSpinBox.value
                    root.value = v1
                }
            }
        }

        KtSlider{
            id: slider
            width: parent.width
            Layout.fillWidth: true
            Layout.preferredHeight: 31
            value: root.value
            smooth: true
            scale: 1
            font.pointSize: KtAlarmTheme.fontPixelSmall
            snapMode: Slider.NoSnap
            stepSize: root.stepSize
            from: root.from
            to: root.to

            onPositionChanged: {
                //console.log("onPositionChanged,slider.value=",value)
                if(root.value != slider.value){
                    let v1 = slider.value
                    root.value = v1
                }
            }
        }
    }

    onValueChanged: {
        var m = Math.floor(value / 60)
        var s = ("00" +value % 60).slice(-2)
        labelTime.text = m + ":" + s;
    }
}




