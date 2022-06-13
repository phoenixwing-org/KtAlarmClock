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
        return KtWindowBase.hideBase();
    }

    /*
     * Show Window
     */
    function showOver(){
        // console.log("KtWindowOver.showOver()")
        let ok = checkoutScreen();
        debugMsg("showOver")

        if(!ok) {
            debugMsg("NG ShowOver")
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

        return true
    }
    
}

