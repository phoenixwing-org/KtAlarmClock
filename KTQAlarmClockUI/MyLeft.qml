/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4

MyLeftForm {

    signal sigScrollPage(int index)
    
    toolButtonMyWork.onClicked: {
        sigScrollPage(0)
    }
    toolButtonMyClass.onClicked: {
        sigScrollPage(1)
    }
    toolButtonMySetting.onClicked: {
        sigScrollPage(2)
    }
}

