import QtQuick 2.0

import "qrc:///"

Flipable{

    objectName: "CardTypeOne"

    property var back_param: ({kanji:"下",
                               num:50,
                               scale:1.0})
    property var front_param: ({word:"BelowWWWWWWW",
                                desc:"floor with magic wand below",
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
        front.init(param["front_param"]);
        back.init(param["back_param"]);
        scale = param["scale"];
    }

    function getTemplate(){
        console.log("Got cardtypeone templ");
        return {front_param:front.getTemplate(),
                back_param:back.getTemplate(),
                scale:1};
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
}
