#include "cardfactory.h"

#include <QByteArray>
#include <QFile>
#include <QDir>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QQmlEngine>
#include <QQmlComponent>

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

void CardFactory::addUrl(c_type_id_t type_id, QUrl url)
{
    urls_.insert(type_id, url);
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

void CardFactory::registerType(c_type_id_t type_id, CardFactory::constructor_t func, QUrl url, QVariantMap templ )
{
    addUrl(type_id, url);
    addConstructor(type_id,func);
    if( templ.empty() ){
        //Load template from url
        //Load build in card types. Use the constructor for the base card for types loaded from qmls
        QQmlEngine engine; //Create gml engine for getting templates from the qml files

        QQmlComponent comp(&engine, url);
        QObject* call_obj = comp.create();

        QMetaObject::invokeMethod(call_obj,"getTemplate", Q_RETURN_ARG(QVariantMap,templ));
        addTemplate(type_id, templ);

        delete call_obj;
    }
    else {
        addTemplate(type_id,templ);
    }
}

void CardFactory::addTemplate(c_type_id_t type_id, QVariantMap templ)
{
    templates_.insert(type_id, templ);
}

void CardFactory::addConstructor(c_type_id_t type_id, CardFactory::constructor_t func)
{
    constructors_.insert(type_id,func);
}

void CardFactory::loadCardTemplates()
{
    //TODO: load card templates from external qmls

    registerType("cardTypeDict",&Card::createCard,QUrl("qrc:///cardTypeDict.qml"));
    registerType("cardTypeOneB",&Card::createCard,QUrl("qrc:///cardTypeOneB.qml"));
    registerType("cardTypeOneF",&Card::createCard,QUrl("qrc:///cardTypeOneF.qml"));

}

QUrl CardFactory::getUrl(c_type_id_t type)
{
    return urls_[type];
}
