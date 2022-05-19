import QtQuick 2.14
import QtQuick.Controls 2.14

Slider {
    id: root
    value: 0.618
    property int widthSliderHandle: 12

    background: Rectangle {
        x: root.leftPadding
        y: root.topPadding + (root.availableHeight - height) *0.5
        implicitWidth: 250
        implicitHeight: 8
        width: root.availableWidth
        height: implicitHeight
        color: KtAlarmTheme.colorPrimaryD
        radius: height * 0.5

        Rectangle {
            width: root.visualPosition * parent.width
            height: parent.height
            color: KtAlarmTheme.colorPrimary
            radius: background.radius
        }
    }

    handle: Rectangle {
        x: root.leftPadding + root.visualPosition * (root.availableWidth - width)
        y: root.topPadding + (root.availableHeight - height) *0.5
        implicitWidth: widthSliderHandle
        implicitHeight: widthSliderHandle
        color: KtAlarmTheme.colorPrimaryD
        radius: width * 0.5
        border.color: KtAlarmTheme.colorPrimaryU
    }
}
