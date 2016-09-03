import QtQuick 2.0

Item {
    id: cardLoader
    objectName: "CardLoader"

    //Component.onCompleted: {console.log("Loader loaded");printTest();}

    function printTest(){console.log("Test print");}

    //TODO: Allow delayd and instant?

    //Loads the new card but don't display it yet
    property var cur_card;
    property var new_card;
    function load(url, param, load_imidiate)
    {
        load_imidiate = typeof load_imidiate !== 'undefined' ? load_imidiate : true;

        var comp = Qt.createComponent(url);
        param.visible = false; //Hide the new card for now.
        param.enabled = false;
        new_card = comp.createObject(cardLoader,param);
        new_card.anchors.centerIn = cardLoader

        if(new_card === null)
        {
            console.log("Error, failed to load card");
        }
        console.log("Card loaded with parameters:");
        console.log(url);
        //console.log(param);

        if(load_imidiate){
            showNew();
        }
    }

    //Call this functiont to switch to the new card and destroy the old
    function showNew()
    {
        if( new_card === null || typeof new_card === 'undefined' ){
            return; //No card loaded so do nothing
        }

        //cur_card.visible = false; //Is this needed?
        new_card.visible = true;
        new_card.enabled = true;
        cardLoader.width = new_card.width;
        cardLoader.height = new_card.height;

        //console.log(cur_card);
        //console.log(new_card);

        //Make sure cur_card and new_card are not the same,
        //if they are, don't destroy cur card
        if( typeof cur_card !== 'undefined' && cur_card !== null && cur_card !== new_card ){
            cur_card.destroy();
        }
        cur_card = new_card;
        console.log("New card set to visible");
    }
}
