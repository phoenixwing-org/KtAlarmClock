/**
* @copyright Shanghai Kuntai Software Technology Co., Ltd. 2022
* @license LGPL 3.0
* @author Kuntai
*/
import QtQuick 2.4

MyRightForm {

    // change Sub Item
    function changeScrollRect(index)
    {
        console.log("MyRightForm.changeScrollRect("+index+")")
        if(0 === index){
            myScroll.scroll.flickableItem.contentY = myScroll.rect0.y
        } else if(1 === index){
            myScroll.scroll.flickableItem.contentY = myScroll.rect1.y
        } else{
            myScroll.scroll.flickableItem.contentY = myScroll.rect2.y
        }
    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
