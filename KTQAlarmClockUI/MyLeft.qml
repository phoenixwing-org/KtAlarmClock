/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4

MyLeftForm {

    signal sigScrollPage(int index)

    actionMyWork.onTriggered: {
        sigScrollPage(0)
    }
    actionMyClass.onTriggered: {
        sigScrollPage(1)
    }
    actionSet.onTriggered: {
        sigScrollPage(2)
    }

    onOnlyIconChanged: {
        if (onlyIcon){
            width = minimunWidth;
        }
        else{
            width = defaultWidth;
        }
    }

    function displayOnlyIcon()
    {
        onlyIcon = true;
    }

    function displayTextBesideIcon()
    {
        //width = ktLeft.defaultWidth;
        onlyIcon = false;
    }

}

