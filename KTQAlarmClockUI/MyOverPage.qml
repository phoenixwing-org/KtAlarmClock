import QtQuick 2.4
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    id: myOverPageWindow
    width: 500
    height: 400
    opacity: 0.8
    visible: false
    visibility : "Maximized"          //最大化
    //flags : Qt.FramelessWindowHint

    MyOverPageForm {
        anchors.fill: parent

        button.onClicked: {
            myOverPageWindow.close()
        }
    }

    function showMyWindow(){
        this.visibility = "Maximized"
        this.flags =Qt.FramelessWindowHint
        this.show()
    }

}

