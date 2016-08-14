// Functions as the front and the back of a simple Dict based card

import QtQuick 2.0

Rectangle{

    id: dictCard

    property string word: "word"
    property int scale: 1

    function init( param ){
        word = param["word"];
    }

    //Template for initializing this card type
    function getTemplate(){
        return {word:undefined,
                scale:1};
    }

    border.color: "black"

    width: 300*scale
    height: 150*scale
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

    Text{
        text: word
        font.pointSize: 32
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}

