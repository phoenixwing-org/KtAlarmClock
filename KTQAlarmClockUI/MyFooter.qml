import QtQuick 2.4

MyFooterForm {
    buttonStart.onClicked:{
        myAlarmClockCmd.onStart();
        var component = Qt.createComponent("MyOverPage.qml")
        var obj = component.createObject(this)
        obj.showMyWindow()
    }

    buttonPause.onClicked: {

        console.debug("clicked Pause");
    }

    buttonStop.onClicked:{

        console.debug("clicked Stop");
    }
}
