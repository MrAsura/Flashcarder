#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include "cardlist.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonDocument>
#include <QVariant>

class CardFactory
{
public:
    static CardFactory& getInstance();

    static const QString CardTypeID;
    static const QString CardTypeFields;

    typedef QString c_type_id_t;
    typedef Card*(*constructor_t)(QVariantMap);

    //Get a card list from a data file
    Cardlist getCardlist( QString filename );

    QString readDataFile( QString filename ); //Read either a xml or a json file and return it as a string
    bool writeDataFile( QString data, QString filename ); //write the given data to a file

    void readCardtypes(QString dir_name);
    QStringList getFields( QString cardtype );

    //Register a custom card type
    void registerType( c_type_id_t type_id, constructor_t func, QVariantMap templ);

    //Load card templates from qml files (in the cardtype folder etc.).
    void loadCardTemplates();

private:
    CardFactory( CardFactory const& ) = delete;
    void operator= ( CardFactory fac ) = delete;
    CardFactory();

    QMap<QString,QStringList> cardtypes_;

    QMap<c_type_id_t, constructor_t> constructors_;
    QMap<c_type_id_t, QVariantMap> templates_;
    QMap<c_type_id_t, QUrl> urls_; //TODO: add url stuff

    QJsonDocument readJson( QString file ); //Reads a json file and returns it as a document

    void addTemplate( c_type_id_t type_id, QVariantMap templ );
};

#endif // CARDFACTORY_H
