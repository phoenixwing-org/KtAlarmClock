import QtQuick 2.14
import KtAlarmClock 1.0

MyFooterForm {
    buttonStart.onClicked:{
        myAlarmClockCmd.onStart();
    }

    buttonPause.onClicked: {
        myAlarmClockCmd.onStart();
    }

    buttonStop.onClicked:{
        myAlarmClockParam.sigClockStart(4); //4:stop
    }
    buttonRest.onClicked:{
        myAlarmClockParam.sigAction(Kt.ActionBreak);
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
