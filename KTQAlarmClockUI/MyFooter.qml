import QtQuick 2.4

MyFooterForm {
    buttonStart.onClicked:{
        myAlarmClockCmd.onStart();
    }

    buttonPause.onClicked: {

        console.debug("clicked Pause");
    }

    buttonStop.onClicked:{

        console.debug("clicked Stop");
    }


}
