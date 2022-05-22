/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.14

MyRightForm {

    // test slot
    function mySlot(index)
    {
        console.log("mySlot("+index+")")
    }
        
    /**
     * update parameter information
     */
    function updateInfor()
    {
        myScroll.updateInfor()
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
