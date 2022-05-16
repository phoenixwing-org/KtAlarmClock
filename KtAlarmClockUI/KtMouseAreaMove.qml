import QtQuick 2.14
import QtQuick.Controls 2.14

MouseArea {
    id:rect
    property var targetMove: parent
    property var targetFill: parent

    // private property begin with __
    property int __dx: 0
    property int __dy: 0

    onPressed: {
        if (targetMove === null) return;
        // calculate the delta position
        // use rect.mapToGlobal to change to global position
        var globalPosition = rect.mapToGlobal(mouse.x, mouse.y)
        __dx = targetMove.x - globalPosition.x
        __dy = targetMove.y - globalPosition.y

        //console.log("globalPosition = ",globalPosition.x,globalPosition.y)
        //console.log("(dx dy) =",__dx,__dy, "mouse=",mouse.x,mouse.y,"target =",targetMove.x,targetMove.y)
    }

    onTargetFillChanged: {
        rect.anchors.fill= targetFill
    }

    onPositionChanged: {
        if (targetMove === null) return;

        var globalPosition = rect.mapToGlobal(mouse.x, mouse.y)
        var x1 = globalPosition.x + __dx
        var y1 = globalPosition.y + __dy
        if ( x1 !== targetMove.x || y1 !== targetMove.y){
            targetMove.x = x1
            targetMove.y = y1
            //console.log("(dx dy) =",__dx,__dy, "mouse=",mouse.x,mouse.y,"target =",targetMove.x,targetMove.y)
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
