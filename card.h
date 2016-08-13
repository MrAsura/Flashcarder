#ifndef CARD_H
#define CARD_H
//Base class for a card.
//TODO: Implement cur virtual methods
#include <QStringList>
#include <QVariant>

class Card
{
public:
    virtual ~Card() = 0;
    virtual void display() = 0; //Display the card
    virtual void flip() = 0; //Display now show the other side of the card (if it has one)
    virtual QStringList keywords() = 0; //Return keywords associated with the card. Can be used to find a specific card.

    static Card* createCard(QVariant data);
};

#endif // CARD_H
