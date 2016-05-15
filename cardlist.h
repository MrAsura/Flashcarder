#ifndef CARDLIST_H
#define CARDLIST_H

#include "card.h"

#include <memory>
#include <qstring.h>
#include <QStringList>
#include <vector>
#include <utility>
#include <random>

//Collection class that holds a collection of cards.
//Iterates throug available cards.
class Cardlist
{
public:
    typedef std::shared_ptr<Card> card_ptr;

    Cardlist();
    Cardlist( card_ptr card );
    ~Cardlist();

    Cardlist* reset(); //current() will return the first card in the container after call to reset(). Return pointer to object to support chaining
    Cardlist* suffle(); //re-orders cards to a random order and current() returns the first card. Return pointer to object to support chaining

    card_ptr current(); //return the current card.
    card_ptr next(); //Call to next returns the card after current() (or the last card) and next call to current() returns the retured card
    card_ptr prev(); //Returns the card retured by current() before last call to next() (or the first card)
    card_ptr first(); //Return first card and current() returns it also
    card_ptr last(); //Returns last card and current() returns it also

    Cardlist* combine( Cardlist& list ); //Combine two lists. Return pointer to object to support chaining
    Cardlist* addCard( card_ptr card ); //Add a card to the end of the list. Return pointer to object to support chaining

    std::vector<card_ptr> findKeyword( QStringList keywords ); //find cards that match given keywords

    unsigned int size(); //Return the number of cards in the list

private:

    //Internal data structure
    struct Container{
        Container* next;
        Container* prev;
        card_ptr card;
        Container( card_ptr c ){
            next = nullptr;
            prev = nullptr;
            card = c;
        }
    };

    void link(Container* first, Container* second); //Links last to point to first and viceverca

    //Get pairs that are linked to form a new order for size containers. fl will contain the first and last index
    std::vector<std::pair<int,int>> getOrder(unsigned int size , std::pair<int, int> &fl);

    unsigned int size_;
    Container* first_;
    Container* last_;
    Container* current_;

    std::default_random_engine e_;
};

#endif // CARDLIST_H
