/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4

MyLeftForm {

    signal actionClicked(int index)
    toolButtonMySpeech.onClicked: {
        actionClicked(0)
    }
    toolButtonMyClass.onClicked: {
        actionClicked(1)
    }
    toolButtonMySetting.onClicked: {
        actionClicked(2)
    }
}

