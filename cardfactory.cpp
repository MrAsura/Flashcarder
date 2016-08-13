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

void CardFactory::registerType(CardFactory::c_type_id_t type_id, CardFactory::constructor_t func, QVariantMap templ)
{
    constructors_.insert(type_id,func);
    addTemplate(type_id,templ);
}

void CardFactory::addTemplate(CardFactory::c_type_id_t type_id, QVariantMap templ)
{
    templates_.insert(type_id, templ);
}

void CardFactory::loadCardTemplates()
{
    //TODO: load card templates from external qmls

    //Load build in card types. Use the constructor for the base card for types loaded from qmls
    QQmlEngine engine; //Create gml engine for getting templates from the qml files

    QQmlComponent comp(&engine, QUrl("qrc:///cardTypeDict.qml"));
    QObject* call_obj = comp.create();

    QVariantMap templ;

    QMetaObject::invokeMethod(call_obj,"getTemplate", Q_RETURN_ARG(QVariantMap,templ));
    registerType("cardTypeDict",Card::createCard,templ);
    delete call_obj;

    comp.loadUrl(QUrl("qrc:///cardTypeOneB.qml"));
    call_obj = comp.create();

    QMetaObject::invokeMethod(call_obj,"getTemplate", Q_RETURN_ARG(QVariantMap,templ));
    registerType("cardTypeOneB",&Card::createCard,templ);
    delete call_obj;

    comp.loadUrl(QUrl("qrc:///cardTypeOneF.qml"));
    call_obj = comp.create();

    QMetaObject::invokeMethod(call_obj,"getTemplate", Q_RETURN_ARG(QVariantMap,templ));
    registerType("cardTypeOneF",&Card::createCard,templ);
    delete call_obj;
}
