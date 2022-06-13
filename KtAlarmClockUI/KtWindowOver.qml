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

    /*
     * Show Window
     */
    function hideOver(){
        if (KtAlarmTheme.debugLocate) console.log(objectName, ".hideOver()")
        return KtWindowBase.hideBase();
    }

    /*
     * Show Window
     */
    function showOver(){
        if (KtAlarmTheme.debugLocate) console.log(objectName, ".showOver()")

        let ok = checkoutScreen();

        if(!ok) {
           console.log(objectName, ".checkoutScreen() = false")
            root.hide()
            return false;
        }

        x = screen.virtualX
        y = screen.virtualY
        if(fullScreen){
            width = screen.width
            height = screen.height
            root.showFullScreen();
        }
        else{
            root.show()
        }

        if (KtAlarmTheme.debug) debugMsg("showOver")
        return true
    }
    
}

