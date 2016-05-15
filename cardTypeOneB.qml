import QtQuick 2.0

Rectangle {

    id: cardTypeOneB

    property int scale: 1
    property string kanji: "下"
    property int num: 50

    width: 200*scale
    height: 300*scale
    radius: 12
    gradient: Gradient {
        GradientStop {
            position: 0.425
            color: "#ffffff"
        }

        GradientStop {
            position: 0.997
            color: "#afabab"
        }
    }
    border.width: 4

    Text {
        height: 5
        text: kanji
        font.pointSize: 134
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
    }
    Text{
        x: 12
        y: 76
        text: num
        font.pointSize: 12
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        verticalAlignment: Text.AlignVCenter
    }

}
