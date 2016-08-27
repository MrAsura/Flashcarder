#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include "cardlist.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonDocument>
#include <QVariant>
#include <QVariantMap>
#include <QDir>

#include "global.h"

class Card;

class CardFactory
{
public:
    static CardFactory& getInstance();
    void initialize(QDir dir ); //Initialize factory before using

    static const QString CardTypeID;
    static const QString CardTypeFields;

    //Used for generic cardlists
    static const QString cardTmplIdFieldName;
    static const QString cardTmplDataFieldName;
    //Returns the base template for a card.
    //Contains the card type (cardTmplIdFieldName) and
    //the data (cardTmplDataFieldName) for the given type
    //The data is in the form specified by the types template
    static QVariantMap getCardBaseTemplate();

    typedef Card*(*constructor_t)(c_type_id_t, QVariantMap, QObject*, QStringList);

    //Get a card list from a data file
    Cardlist getCardlist( QString filename );

    QString readDataFile( QString filename ); //Read either a xml or a json file and return it as a string
    bool writeDataFile( QString data, QString filename ); //write the given data to a file

    void readCardtypes(QString dir_name);
    QStringList getFields( QString cardtype );

    //Register a custom card type
    //If templ is not given, url is used to get templ by loading the qml
    //If type_id is not given, use the object name in the qml as the type_id. Use object name as the type id name if name is not given.
    void registerType( QUrl url, constructor_t func, c_type_id_t type_id = c_type_id_t(), QVariantMap templ = QVariantMap(), QString name = QString() );

    //Load card templates from qml files (in the cardtype folder etc.).
    void loadCardTemplates(QDir dir );

    QUrl getUrl(c_type_id_t type );

    //Returns human readable identifiers for the card types
    QStringList getCardTypeNames();

    //Switch between card type name and id type
    c_type_id_t name2id( QString name );
    QString id2name(c_type_id_t id );

    QVariantMap getTypeTemplate(c_type_id_t type) const; //Get type template
    QVariantMap getCardTemplate(c_type_id_t type) const; //get type template wrapped in the base template

private:
    CardFactory( CardFactory const& ) = delete;
    void operator= ( CardFactory fac ) = delete;
    CardFactory();

    QMap<QString,QStringList> cardtypes_;

    //Contains card types in string format
    QMap<c_type_id_t, QString> cardtypenames_;
    QMap<c_type_id_t, constructor_t> constructors_;
    QMap<c_type_id_t, QVariantMap> templates_;
    QMap<c_type_id_t, QUrl> urls_; //TODO: add url stuff

    QJsonDocument readJson( QString file ); //Reads a json file and returns it as a document

    void addCardTypeName( c_type_id_t type_id, QString name );
    void addUrl( c_type_id_t type_id, QUrl url );
    void addTemplate( c_type_id_t type_id, QVariantMap templ );
    void addConstructor( c_type_id_t type_id, constructor_t func );
};

#endif // CARDFACTORY_H
