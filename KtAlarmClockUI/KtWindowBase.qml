import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14

Window {
    id: root
    property bool canClose: false
    property bool fullScreen: false //true
    property int screenId: 0 //Screens id
    property bool screenOK: true

    //@disable-check M16
    onClosing: function(closeEvent){
        closeEvent.accepted = canClose
    }

    function checkoutScreen(){
        screenOK = (screenId>=0 && screenId < Qt.application.screens.length);
        if(!screenOK) return screenOK;

        root.screen = Qt.application.screens[screenId]
        return true;
    }

    /*
     * Show Window
     */
    function hideBase(){
        console.log("KtWindowBase.hideBase()")
        canClose = true
        root.hide()
        return true;
    }

    /*
     * Show Window
     */
    function showBase(){
        console.log("KtWindowBase.showBase()")
        let ok = checkoutScreen();
        if(!ok) {
            root.hide()
            return false;
        }

        if(fullScreen)
            root.showFullScreen()
        else
            root.show();
    }
}

