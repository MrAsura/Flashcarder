#include "card.h"

Card::~Card()
{

}

Card::Card(QVariantMap data, QObject *context, QStringList keywords): data_(data), context_(context), keywords_(keywords)
{

}

void Card::setContext(QObject *context)
{
    context_ = context;
}

void Card::setData(QVariantMap data)
{
    data_ = data;
}

void Card::setValue(QString key, QVariant value)
{
    data_[key] = value;
}

void Card::addKeywords(QStringList keywords)
{
    //Concatenate lists
    keywords_ += keywords;
}

void Card::clearKeywords()
{
    keywords_.clear();
}

void Card::display()
{
    //Call the context load function and pass data to it
    //param1: url for qml card type, param2: data for loading the card
    QMetaObject::invokeMethod(context,"load", Q_ARG(QVariant,), Q_ARG(QVariant,QVariant::fromValue(data_)));
}

Card *Card::createCard(QVariantMap data, QObject *context, QStringList keywords)
{
    Card* new_card = new Card(data,context,keywords);
    return new_card;
}
