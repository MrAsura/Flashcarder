#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include "cardlist.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonDocument>
#include <QVariant>
#include <QVariantMap>

#include "global.h"

class Card;

class CardFactory
{
public:
    static CardFactory& getInstance();

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
    void registerType( c_type_id_t type_id, constructor_t func, QUrl url, QVariantMap templ = QVariantMap());

    //Load card templates from qml files (in the cardtype folder etc.).
    void loadCardTemplates();

    QUrl getUrl(c_type_id_t type );

private:
    CardFactory( CardFactory const& ) = delete;
    void operator= ( CardFactory fac ) = delete;
    CardFactory();

    QMap<QString,QStringList> cardtypes_;

    QMap<c_type_id_t, constructor_t> constructors_;
    QMap<c_type_id_t, QVariantMap> templates_;
    QMap<c_type_id_t, QUrl> urls_; //TODO: add url stuff

    QJsonDocument readJson( QString file ); //Reads a json file and returns it as a document

    void addUrl( c_type_id_t type_id, QUrl url );
    void addTemplate( c_type_id_t type_id, QVariantMap templ );
    void addConstructor( c_type_id_t type_id, constructor_t func );
};

#endif // CARDFACTORY_H
