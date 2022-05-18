/*
 * ref: https://blog.csdn.net/qq_40602000/article/details/109554364
 */
pragma Singleton
import QtQuick 2.14

QtObject {
    property color colorIcon: debug? "red" : "#2878bb"
    property color colorText: debug? "yellow" : "#2878bb"
    property color colorTextDlg:  debug? "pink" :"#b9d9f0"
    property color colorBackground: "black"
    property color colorBorder: "#2878bb"
    property bool themeDark: true
    property int fontPixelSmall: 8
    property int fontPixelNormal: 12
    property int fontPixelLarge: 20
    property int debug: 0
}
