import QtQuick 2.0

import "qrc:///"

Flipable{

    objectName: "SimpleCardTypeOne"

    property var kanji: "漢";
    property var num: 0;
    property var word: "word";
    property var desc: "desc";
    property var back_param: ({kanji:kanji,
                               num:num,
                               scale:1.0})
    property var front_param: ({word:word,
                                desc:desc,
                                scale:1.0,
                                band:50})
    property double scale: 1.0

    width: 200*scale
    height: 300*scale

    front: CardTypeOneF{ anchors.fill: parent }
    back: CardTypeOneB{ anchors.fill: parent }

    Component.onCompleted: {
        front.init(front_param);
        back.init(back_param);
    }

    function init( param ){
        kanji = param["kanji"];
        num = param["num"];
        word = param["word"];
        desc = param["desc"];
        front.init(front_param);
        back.init(back_param);
    }

    function getTemplate(){
        console.log("Got simplecardtypeone templ");
        return {kanji:kanji,
                num:num,
                word:word,
                desc:desc};
    }

    id: flipable

    property bool flipped: false

    transform: Rotation{
        id: rotation
        origin.x: flipable.width/2
        origin.y: flipable.height/2
        axis.x: 0
        axis.y: 1
        axis.z: 0
        angle: 0
    }

    states: State{
        name: "back"
        PropertyChanges {
            target: rotation
            angle: 180
        }
        when: flipable.flipped
    }

    transitions: Transition {
        NumberAnimation {
            target: rotation
            property: "angle"
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }

    MouseArea{
        anchors.fill: parent
        onClicked: flipable.flipped = !flipable.flipped
    }

    Connections{
        target: signalContext
        onFlip: flipable.flipped = !flipable.flipped
    }
}
