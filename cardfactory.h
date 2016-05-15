#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include "cardlist.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QJsonDocument>

class CardFactory
{
public:
    static CardFactory& getInstance();

    static const QString CardTypeID;
    static const QString CardTypeFields;

    //Get a card list from a data file
    Cardlist getCardlist( QString filename );

    QString readDataFile( QString filename ); //Read either a xml or a json file and return it as a string
    bool writeDataFile( QString data, QString filename ); //write the given data to a file

    void readCardtypes(QString dir_name);
    QStringList getFields( QString cardtype );

private:
    CardFactory( CardFactory const& ) = delete;
    void operator= ( CardFactory fac ) = delete;
    CardFactory();

    QMap<QString,QStringList> cardtypes_;

    QJsonDocument readJson( QString file ); //Reads a json file and returns it as a document
};

#endif // CARDFACTORY_H
