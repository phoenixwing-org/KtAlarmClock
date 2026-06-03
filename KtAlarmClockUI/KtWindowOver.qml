import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14

KtWindowBase {
    id: root
    x:50
    y:50
    width: 500
    height: 600
    color: "black"
    fullScreen: true

    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    function hideOver(){
        if (KtAlarmTheme.debugLocate) console.log(objectName, ".hideOver()")
        return hideBase();
    }

    function showOver(){
        if (KtAlarmTheme.debugLocate) console.log(objectName, ".showOver()")

        if (!applyScreenGeometry()) {
            console.log(objectName, ".applyScreenGeometry() = false")
            hide()
            return false
        }

        if (fullScreen)
            showFullScreen()
        else
            show()

        raiseToFront()

        if (KtAlarmTheme.debug) debugMsg("showOver")
        return true
    }
}
