import QtQuick 2.14
import KtAlarmClock 1.0

MyFooterForm {
    buttonStart.onClicked:{
        myAlarmClockCmd.onStart(KtAlarmClock.WorkTime);
    }

    buttonPause.onClicked: {
        console.log("buttonPause.onClicked")
    }

    buttonStop.onClicked:{
        myAlarmClockCmd.onStart(KtAlarmClock.None);
    }
    buttonRest.onClicked:{
        myAlarmClockCmd.onStart(KtAlarmClock.WorkBreak);
    }
}

