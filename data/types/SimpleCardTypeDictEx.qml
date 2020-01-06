import QtQuick 2.0

import "qrc:///"

FlipableCardBase{

    objectName: "SimpleCardTypeDictEx"

    property var word: "word";
    property var ex: "";
    property var desc: "desc";
    property var back_param: ({word:desc,
                               scale:1.0})
    property var front_param: ({word:word,
                                ex:ex,
                                scale:1.0})
    property double scale: 1.0

    front: CardTypeDictEx{ anchors.fill: parent }
    back: CardTypeDict{ anchors.fill: parent }

    width: front.width*scale
    height: front.height*scale

    Component.onCompleted: {
        front.init(front_param);
        back.init(back_param);
    }

    function init( param ){
        ex = param["ex"];
        word = param["word"];
        desc = param["desc"];
        front.init(front_param);
        back.init(back_param);
    }

    function getTemplate(){
        console.log("Got simplecardtypdictex templ");
        return {ex:ex,
                word:word,
                desc:desc};
    }
}

