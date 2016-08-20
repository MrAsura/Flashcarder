#include "card.h"
#include "cardfactory.h"

#include <QUrl>

Card::~Card()
{

}

Card::Card(c_type_id_t type, QVariantMap data, QObject *context, QStringList keywords): type_(type), data_(data), context_(context), keywords_(keywords)
{

}

void Card::setContext(QObject *context)
{
    context_ = context;
}

void Card::setData(const QVariantMap &data)
{
    data_ = data;
}

void Card::setValue(const QString &key, const QVariant &value)
{
    data_[key] = value;
}

QVariant Card::getValue(const QString &key)
{
    return data_[key];
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
    QMetaObject::invokeMethod(context_,"load", Q_ARG(QUrl,CardFactory::getInstance().getUrl(type_)), Q_ARG(QVariant,QVariant::fromValue(data_)));
}

QStringList Card::keywords()
{
    return keywords_;
}

Card *Card::createCard(c_type_id_t type, QVariantMap data, QObject *context, QStringList keywords)
{
    Card* new_card = new Card(type, data, context, keywords);
    return new_card;
}

QString Card::getType() const
{
    return type_;
}

void Card::setType(const c_type_id_t &type)
{
    type_ = type;
}

QVariantMap Card::getData() const
{
    return data_;
}

QObject *Card::getContext() const
{
    return context_;
}
