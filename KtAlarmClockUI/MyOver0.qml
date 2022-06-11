import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import KtAlarmClock 1.0
import QtGraphicalEffects 1.14

KtWindowOver {
    id: root
    width:600
    height:400
    color: KtAlarmTheme.colorBackground

    property int timeForce: 0
    property int timeMax: 3600
    property int counterForce: 0
    property int formulaValue: 3300
    property bool canClose: false
    property bool showFormula: counterForce<=0
    
    signal onClockOut(int state)

    //modality: Qt.WindowModal //Block other windows
    title: "First Over"
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

    Rectangle {
        id: footer
        x: (root.width - width) * 0.5
        y: root.height - 100
        width: 500
        height: 80
        color: "#111111"
        visible: showFormula

        KtMouseAreaMove{}

        Item {
            id: element
            visible: counterForce <= 0
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
            visible: showFormula
            text: qsTr("Unlock")
            flat: false
            font.pointSize: KtAlarmTheme.fontPixelLarge
            
            anchors.verticalCenter: footer.verticalCenter
            anchors.right: footer.right
            anchors.rightMargin: 5
            onClicked: unlockPage()
        }

        Label {
            id: labelForce
            width: parent.width
            visible: true
            color: KtAlarmTheme.colorText
            text: counterForce
            anchors.horizontalCenter: buttonUnlock.horizontalCenter
            anchors.verticalCenter: buttonUnlock.verticalCenter
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: KtAlarmTheme.fontPixelNormal
        }
    }

    Image {
        id: imgCoffee
        x: root.width/2
        y: root.height * 0.667
        width: 100
        height: 100
        sourceSize.height: 100
        sourceSize.width: 100
        source: "qrc:/image/coffee.svg"
        KtMouseAreaMove{}

    }

    Image {
        id: imgClock
        x: imgCoffee.x + 35
        y: imgCoffee.y + 56
        z: 100
        width: 24
        height: 24
        source: "qrc:/image/alarm-clock.svg"
        KtMouseAreaMove{}
    }

    MyClock{
        id: clock
        timeMax: 4
        visible: true
        x: root.width * 0.5
        y: root.height * 0.5
        KtMouseAreaMove{}
    }

    MyOver1 {
        id: over1
        canClose: root.canClose
        title: qsTr("Over 1 window")
        screenId: 1
    }


    Component.onCompleted: {
        initialFormula()
    }

    //@disable-check M16
    onClosing: function(closeEvent){
        closeEvent.accepted = canClose
        // console.log("MyOver0.closeEvent.accepted =",closeEvent.accepted)
        if(!canClose){
            console.log("MyOver0.closeEvent.accepted =",closeEvent.accepted)
        }
    }

    //@disable-check M204
    Timer {
        id: timerForce
        interval: 1000
        running: false
        repeat: true
        onTriggered:{
            root.counterForce--
            //console.log("counterForce", root.counterForce)
            if(root.counterForce <= 0) {
                stop();
                labelForce.visible = false
            }
        }
    }
    /*
     * Show Window
     */
    function customHide(){
        root.visible = false
        root.canClose = true;

        if(clock.clockRunning){
            root.stopPage0()
        }

        // for over1
        root.hide()
        over1.hide();
    }


    /*
     * Show Window
     */
    function customShow(){
        root.visible = true
        visible = true
        root.screenId = 0
        over1.screenId = 1
        root.checkoutScreen()
        over1.checkoutScreen()
        //console.log("over1.screenOK = ",over1.screenOK)
        //console.log("root.screenOK = ",root.screenOK)

        if(over1.screenOK){
            over1.showOver();
        }

        if(root.screenOK){
            root.canClose = false;// cannot close
            root.counterForce = root.timeForce
            root.showOver0()

            root.clock.timeMax = root.timeMax;
            root.clock.clockStart(KtAlarmClock.WorkBreak);
            return
        }
    }
    function unlockPage(){
        // console.log("MyOver0.unlockPage()")
        if (root.counterForce > 0){
            root.canClose = false;
            return
        }

        // check whether can close
        let can = false;
        if(clock.running){
            // if running, check answer
            var value = parseInt(textEditResult.text)
            can =  (value == root.formulaValue)
        } else{
            can = true
        }
        root.canClose = can

        console.log("canClose = ",value, root.canClose)

        root.textEditResult.text = ""
        if(canClose){
            showMessage("")
            if (clock.running) clock.clockPause()
            root.onClockOut(KtAlarmClock.WorkBreak) // clock out from break
        }
        else{
            showMessage("Result is wrong! Please try agin.")
        }
    }

    /*
     * Show Window 0
     */
    function showOver0(){
        //console.log("MyOver0.showOver0()")
        
        root.canClose = false;
        showMessage("")
        initialFormula();
        if(KtAlarmTheme.debug){
            flags= Qt.Window
        }

        if(counterForce > 0 && counterForce < clock.timeMax){
            labelForce.visible = true
            timerForce.start();
        } else{
            labelForce.visible = false
        }

        return showOver()
    }

    function stopPage0(){
        console.log("MyOver0.stopPage0()")

        root.canClose = true;
        if(clock.clockRunning){
            clock.clockPause()
        }
        showMessage("")
    }

    function initialFormula(){
        let a = Math.floor(1000 * Math.random());
        let b = Math.floor(1000 * Math.random());
        formulaValue = a + b;
        labelFormula.text = a + " + " + b + " ="
    }

    function showMessage(msg){
        labelMsg.text = msg
    }
    
    onVisibleChanged: {
        //if root hide, hide the second one
        if(!visible) over1.hide();
    }
    
}
