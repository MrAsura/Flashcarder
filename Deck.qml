import QtQuick 2.0

import "qrc:///"

Item { //Base of the deck
    width: 400
    height: 200

    objectName: "DeckBase"
    id: deckBase

    //Add a frame for debugging
    Rectangle{
        anchors.fill: parent
        opacity: 0.5
        color: "grey"
        border.width: 2
    }

    MouseArea{
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            if( mouse.button == Qt.RightButton ){
                cardHolder.rightShiftActivated();
            } else{
                cardHolder.leftShiftActivated();
            }

            console.log("click click");
        }
    }

    Item { //Area that will contain the card and the loader

        id: cardHolder

        CardLoader{
            id: cardLoader
            anchors.centerIn: parent
        }

        //Fit to the size of the cardLoader (and its card)
        width: cardLoader.width
        height: cardLoader.height
        anchors.verticalCenter: parent.verticalCenter

        //color: "black"

        state: "visible"

        //Connect card animation to signals emitted by the context
        Connections{
            target: signalContext
            onMoveLeft: cardHolder.leftShift()
            onMoveRight: cardHolder.rightShift()
            onReset: cardHolder.reset()
        }

        signal leftShiftActivated();
        signal rightShiftActivated();

        Component.onCompleted: {
            cardHolder.leftShiftActivated.connect(signalContext.on_rightBtn_clicked);
            cardHolder.rightShiftActivated.connect(signalContext.on_leftBtn_clicked);
        }

        //Functions for making left/right transitions
        function leftShift(){
            console.log("Move Left");
            //Performe the leftward movement of the card
            if(cardHolder.state === "visible" ){
                cardHolder.state = "exit_left";
            }
        }
        function rightShift(){
            console.log("Move Right");
            if(cardHolder.state === "visible" ){
                cardHolder.state = "exit_right";
            }
        }
        function reset(){
            console.log("Reset")
            if(cardLoader.cur_card !== null && typeof cardLoader.cur_card !== 'undefined') cardLoader.cur_card.destroy();
        }

        function showNewCard(){
            console.log("Show New card");
            cardLoader.showNew();
        }

        //property int count: 1;



        states: [
            State {
                name: "visible"
                PropertyChanges {
                    target: cardHolder
//                    color: "black"
                    x: deckBase.width/2 - cardHolder.width/2
                }
            },
            State {
                name: "exit_left"
                PropertyChanges {
                    target: cardHolder
//                    color: "green"
                    x: -cardHolder.width
                    //count: cardHolder.count + 1
                }
            },
            State{
                name: "switch_left"
                PropertyChanges {
                    target: cardHolder
                    x: deckBase.width
                }
            },
            State{
                name: "switch_right"
                PropertyChanges {
                    target: cardHolder
                    x: -cardHolder.width
                }
            },
            State{
                name: "exit_right"
                PropertyChanges {
                    target: cardHolder
//                    color: "green"
                    x: deckBase.width
                    //count: cardHolder.count + 1
                }
            }
        ]

        transitions: [
            Transition {
                from: "visible"
                to: "exit_left"
                SequentialAnimation{
                    NumberAnimation {
                        target: cardHolder
                        property: "x"
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    ScriptAction{
                        script: {
                            cardHolder.showNewCard();
                            cardHolder.state = "switch_left";
                        }
                    }
                }
            },
            Transition {
                from: "visible"
                to: "exit_right"
                SequentialAnimation{
                    NumberAnimation{
                        target: cardHolder
                        property: "x"
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    ScriptAction{
                        script: {
                            cardHolder.showNewCard();
                            cardHolder.state = "switch_right";
                        }
                    }
                }
            },
            Transition {
                from: "exit_left"
                to: "switch_left"
                ScriptAction{
                    script: cardHolder.state = "visible"
                }
            },
            Transition {
                from: "exit_right"
                to: "switch_right"
                ScriptAction{
                    script: cardHolder.state = "visible"
                }
            },
            Transition {
                from: "switch_right,switch_left"
                to: "visible"

                NumberAnimation {
                    target: cardHolder
                    property: "x"
                    duration: 1000
                    easing.type: Easing.InOutQuad
                }
            }
        ]
        /*
        transitions: [
            Transition {
                from: "visible"
                to: "exit_left,exit_right"
                reversible: false
                SequentialAnimation{
                    ParallelAnimation{
                        ColorAnimation{
                            target: cardHolder
                            duration: 1000
                            easing.type: Easing.InOutQuad
                        }
                        PathAnimation{
                            duration: 1000
                            path: Path{
                                PathLine{
                                    x: par.width
                                }
                                PathLine{}
                            }
                        }

//                        NumberAnimation {
//                            target: cardHolder
//                            property: "x"
//                            duration: 1000
//                            easing.type: Easing.InOutQuad
//                        }
                    }
//                    ScriptAction{
//                        script: {
//                            if( cardHolder.state == "exit_left" ){
//                                cardHolder.state = "exit_right"
//                            }
//                            else{
//                                cardHolder.state = "exit_left"
//                            }
//                        }
//                    }
                }
            },
            Transition {
                from: "exit_right,exit_left"
                to: "visible"
                reversible: false
                ParallelAnimation{
                    ColorAnimation{
                        target: cardHolder
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        target: cardHolder
                        property: "x"
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                }
            },
            Transition {
                from: "exit_right"
                to: "exit_left"
                reversible: true
                PauseAnimation {
                    duration: 200
                }
                ScriptAction{
                    script:{
                        console.log("to visible",cardHolder.state);
                        cardHolder.state = "visible";
                    }
                }
            }
        ]*/
    }
}
