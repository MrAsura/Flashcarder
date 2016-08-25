import QtQuick 2.0

Rectangle {
    id: cardTypeOneF
    objectName: "CardTypeOneF"

    property int scale: 1
    property string word: "Below"
    property string desc: "floor with magic wand below"
    property int band: 50

    function init( param ){
        word = param["word"];
        desc = param["desc"];
        band = param["band"];
        scale = param["scale"];
    }

    //Template for initializing this card type
    function getTemplate(){
        return {word:undefined,
                desc:undefined,
                scale:1,
                band:50};
    }

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
        width: 20
        height: 5
        text: word
        font.pointSize: 14
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
    }

    Rectangle{
        height: band
        color: "transparent"
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        border.width: 2
        border.color: "black"

        Text {
            height: 50
            text: desc
            rotation: 180
            font.bold: false
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.left: parent.left
            anchors.leftMargin: 10
            font.pointSize: 14
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
