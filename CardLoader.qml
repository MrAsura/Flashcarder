import QtQuick 2.0

Item {
    id: cardLoader
    objectName: "CardLoader"

    //Component.onCompleted: {console.log("Loader loaded");printTest();}

    function printTest(){console.log("Test print");}
    function load(url, param){
        var comp = Qt.createComponent(url);
        var card = comp.createObject(cardLoader,param);
        if(card === null){
            console.log("Error, failed to load card");
        }
        console.log("Card loaded with parameters:");
        console.log(url);
        console.log(param);
    }

}

