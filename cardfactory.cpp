#include "cardfactory.h"

#include <QByteArray>
#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>

const QString CardFactory::CardTypeID = "cardtypeName";
const QString CardFactory::CardTypeFields = "fieldNames";

CardFactory::CardFactory(): cardtypes_()
{
}

QJsonDocument CardFactory::readJson(QString file)
{
    QFile json(file);
    QJsonDocument json_doc;

    json.open(QFile::ReadOnly);
    json_doc = QJsonDocument::fromJson(json.readAll());

    return json_doc;
}


CardFactory &CardFactory::getInstance()
{
    static CardFactory fac;
    return fac;
}

void CardFactory::readCardtypes(QString dir_name)
{
    //Find cardtypes
    QDir dir(dir_name);
    dir.setFilter( QDir::Files);
    dir.setNameFilters(QStringList("*.ctype"));
    QStringList ctypes = dir.entryList();

    for( QString ctype: ctypes)
    {
        QJsonDocument type_info = readJson(dir_name + "/" + ctype);
        QJsonObject obj = type_info.object();

        QString type_name = obj.value(CardTypeID).toString();
        QVariantList type_fields_var = obj.value(CardTypeFields).toArray().toVariantList();
        QStringList field_names;

        for( QVariant field: type_fields_var )
        {
            field_names << field.toString();
        }

        cardtypes_.insert(type_name,field_names);
    }
}

QStringList CardFactory::getFields(QString cardtype)
{
    return cardtypes_.value(cardtype);
}
