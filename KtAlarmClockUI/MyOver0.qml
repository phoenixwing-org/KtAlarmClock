import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.12
import KtAlarmClock 1.0
import QtGraphicalEffects 1.14

KtWindowOver {
    id: root
    width:600
    height:400
    property alias myClock: myClock
    readonly property alias clockRunning: myClock.clockRunning

    property alias labelFormula: labelFormula
    property alias labelMsg: labelMsg
    property alias textEditResult: textEditResult
    property int counterForce: 0
    property int formulaValue: 3300
    property bool canClose: false
    property bool showFormula: counterForce<=0
    
    //modality: Qt.WindowModal //Block other windows
    title: "First Over"

    Rectangle {
        id: rectangle
        width: root.width
        height: root.height
        color: KtAlarmTheme.colorBackground
        anchors.fill: parent
        opacity: 1

        Label {
            id: labelMsg
            y: 189
            color: KtAlarmTheme.colorText
            text: qsTr("")
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: footer.top
            anchors.bottomMargin: 0
            font.pointSize: KtAlarmTheme.fontPixelLarge
        }

        Image {
            x: root.width/2
            y: root.height/2
            width: 100
            height: 100
            sourceSize.height: 100
            sourceSize.width: 100
            source: "qrc:/image/coffee.svg"
            ColorOverlay {
                 anchors.fill: parent
                 source: parent
                 color: KtAlarmTheme.colorIcon
             }

            KtMouseAreaMove{}
        }

        Rectangle {
            id: footer
            width: parent.width
            height: 80
            color: "#111111"
            anchors.bottom: parent.bottom

            Label {
                id: kt
                x: 80
                color: KtAlarmTheme.colorText
                text: qsTr("上海锟钛软件科技有限公司")
                anchors.verticalCenter: footer.verticalCenter
                font.pointSize: KtAlarmTheme.fontPixelNormal
            }

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
                id: buttonStop
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
                anchors.horizontalCenter: buttonStop.horizontalCenter
                anchors.verticalCenter: buttonStop.verticalCenter
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: KtAlarmTheme.fontPixelNormal
            }

            Image {
                id: ktLogo
                x: 5
                y: 5
                z: 100
                width: 70
                height: 70
                sourceSize.height: 70
                sourceSize.width: 70
                source: "qrc:/image/kt-blue.svg"
                KtMouseAreaMove{}
            }
        }
    }

    MyClock{
        id: myClock
        timeMax: 4
        visible: true
        x: root.width/2
        y: 0
        KtMouseAreaMove{}
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

    onActiveFocusItemChanged: {
        //console.log("MyOver0.activeFocusItem is changed to  ", activeFocusItem)

        // clockRunning and not active, user want to escape the lock

        // console.log("clockRunning =  ", clockRunning)
        // console.log("clockRunning && !activeFocusItem =  ", (clockRunning && !activeFocusItem))
        //if(clockRunning && !activeFocusItem){
        //    console.log("I am in (clockRunning && !activeFocusItem)")
        //   / this.hide();
        //    //console.log("MyOver0: Try to Run showOver()  ")
        //    
        //    //root.showFullScreen()
        //}
    }

    function unlockPage(){
        // console.log("MyOver0.unlockPage()")
        if (root.counterForce > 0){
            root.canClose = false;
            return
        }

        // check answer
        var value = parseInt(textEditResult.text)
        root.canClose = (value == root.formulaValue)
        // console.log("canClose = ",value, root.canClose)

        root.textEditResult.text = ""
        if(canClose){
            showMessage("")
            myClock.onClockPause()
            myAlarmClockParam.sigClockOut(KtAlarmClock.WorkBreak) // clock out from break
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

        if(counterForce > 0 && counterForce < myClock.timeMax){
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
        if(clockRunning){
            myClock.onClockPause()
        }
        if(visible){
            showMessage("")
            this.hide()
        }
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
}
