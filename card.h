#ifndef CARD_H
#define CARD_H
//Base class for a card.
//TODO: Implement cur virtual methods
#include <QStringList>
#include <QVariant>

#include "global.h"
using global::c_type_id_t;

class Card //: QObject
{
    //Q_OBJECT //Needles overhead to declare as a Q_Object when works without it?

public:
    ~Card();
    Card( c_type_id_t type, QVariantMap data, QObject* context, QStringList keywords);

    //Context is used for displaying the card
    void setContext(QObject* context);

    void setData( const QVariantMap &data );
    void setValue( const QString &key, const QVariant &value);
    QVariant getValue( const QString &key );

    void addKeywords(QStringList keywords );
    void clearKeywords();

    void display(bool immediate = true); //Display the card

    QStringList keywords(); //Return keywords associated with the card. Can be used to find a specific card.

    //Creates an instance of this card
    static Card* createCard(c_type_id_t type, QVariantMap data, QObject* context = nullptr, QStringList keywords = QStringList() );

    QString getType() const;
    void setType(const c_type_id_t &type);

    QVariantMap getData() const;

    QObject *getContext() const;

signals:
    //Define a display signal that is sent when the card should be displayed
    //void displayCard(QVariant url, QVariant data );

private:

    c_type_id_t type_;
    QVariantMap data_;
    QObject* context_;
    QStringList keywords_;
};

#endif // CARD_H
