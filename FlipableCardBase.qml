import QtQuick 2.0

Flipable{

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
