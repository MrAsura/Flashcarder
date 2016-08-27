#include "cardfactory.h"

#include <QByteArray>
#include <QFile>

#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QQmlEngine>
#include <QQmlComponent>

const QString CardFactory::CardTypeID = "cardtypeName";
const QString CardFactory::CardTypeFields = "fieldNames";
const QString CardFactory::cardTmplIdFieldName = "cardType";
const QString CardFactory::cardTmplDataFieldName = "data";

CardFactory::CardFactory(): cardtypes_()
{
    //Load card types
    //loadCardTemplates();
}

QVariantMap CardFactory::getTypeTemplate(c_type_id_t type) const
{
    return templates_[type];
}

QVariantMap CardFactory::getCardTemplate(c_type_id_t type) const
{
    QVariantMap tmpl = getCardBaseTemplate();
    tmpl[cardTmplIdFieldName] = type;
    tmpl[cardTmplDataFieldName] = getTypeTemplate(type);
    return tmpl;
}

QJsonDocument CardFactory::readJson(QString file)
{
    QFile json(file);
    QJsonDocument json_doc;

    json.open(QFile::ReadOnly);
    json_doc = QJsonDocument::fromJson(json.readAll());

    return json_doc;
}

void CardFactory::addCardTypeName(c_type_id_t type_id, QString name)
{
    cardtypenames_.insert(type_id,name);
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

void CardFactory::initialize(QDir dir)
{
    //Handle initialization here
    loadCardTemplates(dir);
}

QVariantMap CardFactory::getCardBaseTemplate()
{
    static QVariantMap tmpl;
    if( tmpl.empty() ){
        c_type_id_t def_val_id;
        QVariantMap def_val_data;
        tmpl.insert(cardTmplIdFieldName,def_val_id);
        tmpl.insert(cardTmplDataFieldName,def_val_data);
    }
    return tmpl;
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

void CardFactory::registerType(c_type_id_t type_id, CardFactory::constructor_t func, QUrl url, QVariantMap templ , QString name)
{
    addUrl(type_id, url);
    addConstructor(type_id,func);

    //If name is not set, attempt to cast type_id to string
    name = name.isEmpty() ? QVariant(type_id).toString() : name;
    addCardTypeName(type_id, name);

    if( templ.empty() ){
        //Load template from url
        //Load build in card types. Use the constructor for the base card for types loaded from qmls
        QQmlEngine engine; //Create gml engine for getting templates from the qml files

        QQmlComponent comp(&engine, url);
        QObject* call_obj = comp.create();

        QVariant ret_val;
        QMetaObject::invokeMethod(call_obj,"getTemplate", Q_RETURN_ARG(QVariant, ret_val));
        templ = ret_val.toMap();
        //QMetaObject::invokeMethod(call_obj,"testPrint");

        delete call_obj;
    }

    addTemplate(type_id,templ);
}

void CardFactory::addTemplate(c_type_id_t type_id, QVariantMap templ)
{
    templates_.insert(type_id, templ);
}

void CardFactory::addConstructor(c_type_id_t type_id, CardFactory::constructor_t func)
{
    constructors_.insert(type_id,func);
}

void CardFactory::loadCardTemplates( QDir dir )
{
    //load card templates from external qmls
    dir.setNameFilters(QStringList("*.qml"));
    dir.setFilter(QDir::Files|QDir::Readable);
    QStringList type_qmls = dir.entryList();

    for( QString type_qml: type_qmls){
        c_type_id_t type_id = type_qml.split('/').last().left(type_qml.size()-4); //Use the name of the qml file as the type id
        registerType(type_id, &Card::createCard, QUrl::fromLocalFile(dir.absoluteFilePath(type_qml)));
    }

    //Register build in types here
    registerType("CardTypeDict",&Card::createCard,QUrl("qrc:///CardTypeDict.qml"));
    registerType("CardTypeOneB",&Card::createCard,QUrl("qrc:///CardTypeOneB.qml"));
    registerType("CardTypeOneF",&Card::createCard,QUrl("qrc:///CardTypeOneF.qml"));
}

QUrl CardFactory::getUrl(c_type_id_t type)
{
    return urls_[type];
}

QStringList CardFactory::getCardTypeNames()
{
    return QStringList(cardtypenames_.values());
}

c_type_id_t CardFactory::name2id(QString name)
{
    return cardtypenames_.key(name);
}

QString CardFactory::id2name(c_type_id_t id)
{
    return cardtypenames_.value(id);
}
