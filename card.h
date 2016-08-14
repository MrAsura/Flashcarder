#ifndef CARD_H
#define CARD_H
//Base class for a card.
//TODO: Implement cur virtual methods
#include <QStringList>
#include <QVariant>

class Card
{
public:
    ~Card();
    Card(QVariantMap data, QObject* context, QStringList keywords);
    //Context is used for displaying the card
    void setContext(QObject* context);
    void setData( QVariantMap data );
    void setValue( QString key, QVariant value);
    void addKeywords(QStringList keywords );
    void clearKeywords();
    void display(); //Display the card

    QStringList keywords(); //Return keywords associated with the card. Can be used to find a specific card.

    //Creates an instance of this card
    static Card* createCard(QVariantMap data, QObject* context = nullptr, QStringList keywords = QStringList() );

private:
    QVariantMap data_;
    QObject* context_;
    QStringList keywords_;
};

#endif // CARD_H
