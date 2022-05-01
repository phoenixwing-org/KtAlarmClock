import QtQuick 2.4

MyFooterForm {
    buttonStart.onClicked:{
        console.debug("clicked Start");

    }

    buttonPause.onClicked: {

        console.debug("clicked Pause");
    }

    buttonStop.onClicked:{

        console.debug("clicked Stop");
    }


}
