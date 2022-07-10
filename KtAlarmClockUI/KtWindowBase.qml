import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14

Window {
    id: root
    property bool canClose: true
    property bool fullScreen: false //true
    property int screenId: 0 //Screens id
    property bool screenOK: true
    visible:false
        
    onVisibleChanged: {
        debugMsg("onVisibleChanged ") 
    }

    //@disable-check M16
    onClosing: function(closeEvent){
        console.log(objectName, ".Base.onClosing(), canClose=",canClose)
        closeEvent.accepted = canClose
    }

    function checkoutScreen(){
        if (KtAlarmTheme.debugLocate) console.log(objectName,".checkoutScreen()")
        screenOK = (screenId>=0 && screenId < Qt.application.screens.length);
        if(!screenOK) return screenOK;

        root.screen = Qt.application.screens[screenId]
        return true;
    }

    /*
     * Show Window
     */
    function hideBase(){
        console.log(objectName,".hideBase()")
        canClose = true
        root.hide()
        return true;
    }

    /*
     * Show Window
     */
    function showBase(){
        console.log(objectName,".showBase()")
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
    
    function debugMsg(iMsg = ""){
        console.log(objectName, iMsg + ", {visible:", visible, ", rect: (",x,",",y,",",width,",",height,")",
            ", canClose:", canClose, ",screenOK:",screenOK,
            ", screenId:",screenId, "}")
    }
}

