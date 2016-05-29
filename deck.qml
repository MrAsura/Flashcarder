import QtQuick 2.0

Rectangle {
    width: 200
    height: 200

    id: par

    Rectangle{
        width: 50
        height: 50
        color: "black"
        id: rect

        state: "visible"

        property int count: 1

        MouseArea{
            anchors.fill: parent
            onClicked: {
                rect.state = rect.count % 2 == 0 ? "exit_left" : "exit_right"
                rect.count = rect.count + 1
                console.log("click click")
            }
        }

        states: [
            State {
                name: "visible"
                PropertyChanges {
                    target: rect
                    color: "black"
                    x: par.width/2
                }
            },
            State {
                name: "exit_left"
                PropertyChanges {
                    target: rect
                    color: "green"
                    x: -rect.width
                    //count: rect.count + 1
                }
            },
            State{
                name: "switch_left"
                PropertyChanges {
                    target: rect
                    x: par.width
                }
            },
            State{
                name: "switch_right"
                PropertyChanges {
                    target: rect
                    x: -rect.width
                }
            },
            State{
                name: "exit_right"
                PropertyChanges {
                    target: rect
                    color: "green"
                    x: par.width
                    //count: rect.count + 1
                }
            }
        ]

        transitions: [
            Transition {
                from: "visible"
                to: "exit_left"
                SequentialAnimation{
                    NumberAnimation {
                        target: rect
                        property: "x"
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    ScriptAction{
                        script: rect.state = "switch_left"
                    }
                }
            },
            Transition {
                from: "visible"
                to: "exit_right"
                SequentialAnimation{
                    NumberAnimation{
                        target: rect
                        property: "x"
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    ScriptAction{
                        script: rect.state = "switch_right"
                    }
                }
            },
            Transition {
                from: "exit_left"
                to: "switch_left"
                ScriptAction{
                    script: rect.state = "visible"
                }
            },
            Transition {
                from: "exit_right"
                to: "switch_right"
                ScriptAction{
                    script: rect.state = "visible"
                }
            },
            Transition {
                from: "switch_right,switch_left"
                to: "visible"

                NumberAnimation {
                    target: rect
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
                            target: rect
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
//                            target: rect
//                            property: "x"
//                            duration: 1000
//                            easing.type: Easing.InOutQuad
//                        }
                    }
//                    ScriptAction{
//                        script: {
//                            if( rect.state == "exit_left" ){
//                                rect.state = "exit_right"
//                            }
//                            else{
//                                rect.state = "exit_left"
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
                        target: rect
                        duration: 1000
                        easing.type: Easing.InOutQuad
                    }
                    NumberAnimation {
                        target: rect
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
                        console.log("to visible",rect.state);
                        rect.state = "visible";
                    }
                }
            }
        ]*/
    }
}
