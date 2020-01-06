import QtQuick 2.0

Rectangle{

    id: dictCard
    objectName: "CardTypeDict"

    property string word: "word"
    property string ex: "ex"
    property double scale: 1.0

    function init( param ){
        word = param["word"];
        ex = param["ex"];
        scale = param["scale"];
    }

    //Template for initializing this card type
    function getTemplate(){
        console.log("Got template");
        return {word:"word",
                ex:"",
                scale:1.0};
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
        id:main_text
        text: word
        font.pointSize: 28
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
    Text{
        text: ex
        anchors.left: main_text.left
        anchors.bottom: main_text.top
        font.pointSize: 16
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }
}
