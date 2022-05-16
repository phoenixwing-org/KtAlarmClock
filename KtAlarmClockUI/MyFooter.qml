import QtQuick 2.14

MyFooterForm {
    buttonStart.onClicked:{
        myAlarmClockCmd.onStart();
    }

    buttonPause.onClicked: {

        console.debug("clicked Pause");
        myAlarmClockCmd.onStart();
    }

    buttonStop.onClicked:{

        console.debug("clicked Stop");
        myAlarmClockParam.sigClockStart(4); //4:stop
    }
}
