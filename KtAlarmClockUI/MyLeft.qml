/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14

MyLeftForm {

    signal sigScrollPage(int index)

    actionMyWork.onTriggered: {
        sigScrollPage(0)
    }
    actionMyExercise.onTriggered: {
        sigScrollPage(1)
    }
    actionMyForce.onTriggered: {
        sigScrollPage(2)
    }

    onOnlyIconChanged: {
        if (onlyIcon){
            width = minimumWidth;
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

