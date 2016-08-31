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
const QString CardFactory::cardTmplKeywordFieldName = "keywords";

CardFactory::CardFactory(): cardtypes_()
{
    //Load card types
    //loadCardTemplates();
}

bool CardFactory::isRegisteredType(c_type_id_t type_id)
{
    return cardtypenames_.contains(type_id);
}

QVariantMap CardFactory::getTypeTemplate(c_type_id_t type) const
{
    return templates_[type];
}

QVariantMap CardFactory::getCardTemplate(c_type_id_t type) const
{
    QVariantMap tmpl = getCardBaseTemplate();
    tmpl[cardTmplIdFieldName] = id2name(type);
    tmpl[cardTmplDataFieldName] = getTypeTemplate(type);
    return tmpl;
}

QJsonDocument CardFactory::readJson(QString filename)
{
    QFile json(filename);
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
        //QVariantList def_val_keywords;
        tmpl.insert(cardTmplIdFieldName,def_val_id);
        tmpl.insert(cardTmplDataFieldName,def_val_data);
        //tmpl.insert(cardTmplKeywordFieldName, def_val_keywords);
    }
    return tmpl;
}

std::shared_ptr<Cardlist> CardFactory::getCardlist(QString filename, QObject* context)
{
    //Read json data from file and use values to initialize cards
    QJsonDocument cardlist = readJson(filename);

    std::shared_ptr<Cardlist> cardlist_ptr( new Cardlist() );

    if( cardlist.isArray() )
    {
        //Loop over cards in cardlist
        for( QJsonValueRef item: cardlist.array() )
        {
            if(!item.isObject()) continue; //Card needs to be an object

            QJsonObject card = item.toObject();
            c_type_id_t type_id = name2id(card[cardTmplIdFieldName].toString());
            //Check if type_id is a known type
            if( !isRegisteredType(type_id) )
            {
                continue;
            }

            QVariantMap data = QVariant(card).toMap();
            cardlist_ptr->addCard(Cardlist::card_ptr(getConstructor(type_id)(type_id, data, context, QStringList())));
        }
    }
    else{
        //TODO: Do error stuff here
        qDebug("File did not contain a valid Json Array");
    }
    return cardlist_ptr;
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

void CardFactory::registerType( QUrl url, constructor_t func, c_type_id_t type_id , QVariantMap templ , QString name )
{
    if( templ.empty() || name.isEmpty() ){
        //Load template from url
        //Load build in card types. Use the constructor for the base card for types loaded from qmls
        QQmlEngine engine; //Create gml engine for getting templates from the qml files

        QQmlComponent comp(&engine, url);
        QObject* call_obj = comp.create();

        if(templ.empty()){
            QVariant ret_val;
            QMetaObject::invokeMethod(call_obj,"getTemplate", Q_RETURN_ARG(QVariant, ret_val));
            templ = ret_val.toMap();
            //QMetaObject::invokeMethod(call_obj,"testPrint");
        }
        if(name.isEmpty()){
            name = call_obj->objectName();
        }
        delete call_obj;
    }
    //If name is not set, attempt to cast type_id to string
    name = name.isEmpty()&&type_id!=c_type_id_t() ? QVariant(type_id).toString() : name;
    type_id = type_id==c_type_id_t()&&!name.isEmpty() ? c_type_id_t(name) : type_id;
    if( type_id == c_type_id_t() || name.isEmpty() ){
        //Error setting name/type_id
        qDebug("Error getting type id/name"); //TODO: Throw error etc.?
    }

    addUrl(type_id, url);
    addConstructor(type_id,func);
    addCardTypeName(type_id, name);
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

CardFactory::constructor_t CardFactory::getConstructor(c_type_id_t type_id)
{
    return constructors_[type_id];
}

void CardFactory::loadCardTemplates( QDir dir )
{
    //load card templates from external qmls
    dir.setNameFilters(QStringList("*.qml"));
    dir.setFilter(QDir::Files|QDir::Readable);
    QStringList type_qmls = dir.entryList();

    for( QString type_qml: type_qmls){
        //c_type_id_t type_id = type_qml.split('/').last().left(type_qml.size()-4); //Use the name of the qml file as the type id
        registerType(QUrl::fromLocalFile(dir.absoluteFilePath(type_qml)), &Card::createCard );
    }

    //Register build in types here
    registerType(QUrl("qrc:///CardTypeDict.qml"), &Card::createCard );
    registerType(QUrl("qrc:///CardTypeOneF.qml"), &Card::createCard );
    registerType(QUrl("qrc:///CardTypeOneB.qml"), &Card::createCard );
}

QUrl CardFactory::getUrl(c_type_id_t type)
{
    return urls_[type];
}

QStringList CardFactory::getCardTypeNames()
{
    return QStringList(cardtypenames_.values());
}

c_type_id_t CardFactory::name2id(QString name) const
{
    return cardtypenames_.key(name);
}

QString CardFactory::id2name(c_type_id_t id) const
{
    return cardtypenames_.value(id);
}
