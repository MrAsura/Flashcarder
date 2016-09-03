#include "cardedit.h"
#include "ui_cardedit.h"
#include "cardfactory.h"

#include <QList>
#include <cmath>
#include <algorithm>
#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

CardEdit::CardEdit(QWidget *parent, QString def_dir) :
    QWidget(parent),
    ui(new Ui::CardEdit),
    def_dir_(def_dir),
    cur_json_(),
    non_item_Rows_(),
    is_cardtype_(false),
    cur_file_name_("")
{
    qDebug("CardEdit building...");

    ui->setupUi(this);
}

CardEdit::~CardEdit()
{
    delete ui;
}

void CardEdit::setDir(QString dir)
{
    def_dir_ = dir;
}

void CardEdit::on_newCtBtn_clicked()
{
    //Set up a new cardtype template
    QJsonObject temp;
    QJsonArray array; //Array to  be added
    array.append(QJsonValue("field1"));
    array.append(QJsonValue("field2"));
    array.append(QJsonValue("..."));

    temp.insert( CardFactory::CardTypeID, QJsonValue("New Cardtype"));
    temp.insert( CardFactory::CardTypeFields, QJsonValue(array));
    cur_json_ = QJsonDocument();
    cur_json_.setObject(temp);

    //Disable un-used buttons
    ui->removeCardBtn->setEnabled(false);
    ui->newCardBtn->setEnabled(false);

    is_cardtype_ = true;
    populateListFromJson();
}

void CardEdit::populateListFromJson()
{
    //update non item rows
    QList<int> non_item_rows;

    //RegExp for splitting json to correct parts
    QString singleField = "\n\\s*\"%1\"\\s*:\\s*\"[\\s\\S]*\"\\s*,?\\s*\n"; //Match json field %1
    QString arrayField = "\n\\s*\"%1\"\\s*:\\s*\\[[\\s\\S]*\\]\\s*,?\\s*\n"; //Match a field %1 with a array as value
    QRegExp ctNameExp( singleField.arg(CardFactory::CardTypeID) ); //Match cardtypeName
    QRegExp fNameExp( arrayField.arg(CardFactory::CardTypeFields) ); //Match field names array
    QRegExp cardsExp( arrayField.arg( cardlistFieldName ) ); //Match cardlist
    QRegExp ctIdExp( singleField.arg( cardtypeFieldName ) ); //Match cardstype
    QRegExp endExp("\n\\s*\\}\\s*$"); //Match end brackets
    QRegExp removeLBs("^\n|\n$");

    QString text = cur_json_.toJson();

    //Split text into correct parts
    QList<int> foundInd;
    QStringList items;

    foundInd.push_back(text.indexOf(ctNameExp));
    foundInd.push_back(text.indexOf(fNameExp));
    foundInd.push_back(text.indexOf(endExp));
    foundInd.push_back(text.indexOf(ctIdExp));

    //Match cardlist and save position to split cardlist cards into separate items
    int cardsInd = text.indexOf(cardsExp);
    foundInd.push_back(cardsInd);

    foundInd.removeAll(-1);

    std::sort(foundInd.begin(),foundInd.end(), std::less<int>());

    //Check if a cardsInd exists and split items
    if( cardsInd > -1 )
    {
        //Find end index
        int ind = foundInd.indexOf(cardsInd)+1; //Index in the foundInd list where new values should be added
        non_item_rows << ind-1;

        int cardsEnd = foundInd.at(std::min(ind,foundInd.length()-1));
        int cur = cardsInd;
        bool has_object = false; //Flag for checking if array has atleast one item

        while( cur < cardsEnd )
        {
            cur = text.indexOf(QRegExp("\n\\s*\\{[\\s\\S]*\\},?\\s*\n"),cur+1);

            if( cur <= -1 || cur >= cardsEnd )
            {
                break;
            }

            foundInd.insert(ind++,cur);
            has_object = true;
        }

        //Add ] to list if array has items
        if( has_object )
        {
            int endInd = text.lastIndexOf(QRegExp("\n\\s*\\],?\\s*\n"),cardsEnd);
            //Check that found index is after last found
            if( endInd > cur )
            {
                foundInd.insert(ind,endInd);
                non_item_rows << ind;
            }
        }
    }

    //First part
    int cur_point = foundInd.first();
    int end_point = cur_point;
    items.append(text.mid(0,cur_point).remove(removeLBs));

    for( int ind = 0; ind < foundInd.size()-1; ++ind )
    {
        //Split on foundInd (middle items)
        cur_point = foundInd.at(ind);
        end_point = foundInd.at(ind+1)-cur_point;
        items.append(text.mid(cur_point,end_point).remove(removeLBs));
    }

    //Last part
    items.append(text.mid(foundInd.last()).remove(removeLBs));

    ui->itemList->clear();
    ui->itemList->addItems(items);

}

bool CardEdit::isItem(int row)
{
    if( row <= 0 || row >= ui->itemList->count()-1 )
    {
        return false;
    }
    else if( non_item_Rows_.contains(row) )
    {
        return false;
    }

    return true;
}

void CardEdit::addField(QWidget *parent, QString label, QString field_val)
{
    QWidget* label_w = new QLabel(label+":",ui->labelFields);
    label_w->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    QWidget* field = new QLineEdit(field_val,ui->labelFields);
    field->setObjectName(label+editFieldIdPostfix);

    parent->layout()->addWidget(label_w);
    parent->layout()->addWidget(field);
}

void CardEdit::errorDlg(QString msg)
{
    QMessageBox::warning(this,"Error",msg);
}

void CardEdit::on_newClBtn_clicked()
{
    //Set up a new cardlist template
    QJsonObject temp;
    temp.insert( cardtypeFieldName, QJsonValue("Cardtype name"));
    temp.insert( cardlistFieldName, QJsonValue(QJsonArray()));
    cur_json_ = QJsonDocument();
    cur_json_.setObject(temp);

    //Disable un-used buttons
    ui->removeCardBtn->setEnabled(true);
    ui->newCardBtn->setEnabled(true);

    populateListFromJson();
    is_cardtype_ = false;
}

void CardEdit::on_newCardBtn_clicked()
{
    QJsonObject json = cur_json_.object();
    QJsonArray cards = json.value(cardlistFieldName).toArray();

    QJsonObject new_item;
    QStringList fields = CardFactory::getInstance().getFields(json.value(cardtypeFieldName).toString());

    //Check if valid cardtype
    if( fields == QStringList() )
    {
        errorDlg("Un-Registered Cardtype");
        return;
    }

    for( QString field: fields)
    {
        new_item.insert(field,QJsonValue("Value"));
    }

    cards.append(QJsonValue(new_item));
    json.insert(cardlistFieldName,QJsonValue(cards));
    cur_json_.setObject(json);

    populateListFromJson();
}

void CardEdit::on_itemList_currentRowChanged(int currentRow)
{
    QList<QWidget*> children = ui->labelFields->findChildren<QWidget*>();
    for( QWidget* child : children )
    {
        delete child;
    }

    //Check tat row has an item
    if( isItem(currentRow) )
    {
        //Find out the selected fieldname and value
        QJsonObject::ConstIterator it = cur_json_.object().constBegin();
        it += currentRow-1; //If iterator is advanced too much will go over "end()"
        QJsonObject json = cur_json_.object();

        //Check if selected is info field or card
        if( currentRow-1 >= json.count() )
        {
            //Card Item
            QJsonObject fields = json.value(cardlistFieldName).toArray().at(currentRow-json.count()-1).toObject();
            QStringList f_names = fields.keys();

            //Iterate over all the keys
            for( QString name: f_names )
            {
                addField(ui->labelFields,name,fields.value(name).toString());
            }

        }
        else if( it.key() == CardFactory::CardTypeID || it.key() == cardtypeFieldName )
        {
            //Cardtype name or id
            addField(ui->labelFields,it.key(),it.value().toString());
        }
        else if( it.key() == CardFactory::CardTypeFields )
        {
            //Get string from val array
            QString fields = "";
            QJsonArray array = it.value().toArray();

            for( QVariant field: array.toVariantList() )
            {
                if( fields.length() > 0 )
                {
                    fields += ", ";
                }

                fields += field.toString();
            }

            addField(ui->labelFields,it.key(),fields);
        }
    }

}

void CardEdit::on_saveValueBtn_clicked()
{
    //update value
    int row = ui->itemList->currentRow();

    if( isItem(row) )
    {
        //Field names are json field name + editFieldIdPostfix
        //Find children and iterate over them
        QList<QLineEdit*> fields = ui->labelFields->findChildren<QLineEdit*>(QRegExp("[\\s\\S]*"+editFieldIdPostfix));
        QJsonObject json = cur_json_.object();
        QJsonObject value = json;
        bool is_card = false;
        int index = 0;

        //Determine selected item
        if( json.begin()+row-1 == json.end() )
        {
            //Selected item is a card
            index = row-json.count()-1;
            value = json.value(cardlistFieldName).toArray().at(index).toObject();
            is_card = true;
        }

        for( QLineEdit* field: fields )
        {
            QString field_name = field->objectName().remove(editFieldIdPostfix);

            if(field_name == CardFactory::CardTypeFields)
            {
                //Array field
                QString text = field->text().remove(' ');
                value.insert(field_name,QJsonArray::fromStringList(text.split(',',QString::SkipEmptyParts)));
            }
            else
            {
                //String field
                value.insert(field_name,field->text());
            }
        }

        if( is_card )
        {
            QJsonArray arr = json.value(cardlistFieldName).toArray();
            arr.replace(index,QJsonValue(value));
            json.insert(cardlistFieldName,QJsonValue(arr));
        }
        else
        {
            json = value;
        }

        cur_json_.setObject(json);

        populateListFromJson();
    }
}

void CardEdit::on_removeCardBtn_clicked()
{
    QJsonObject json = cur_json_.object();
    int row = ui->itemList->currentRow();

    if( isItem(row) && row > json.count() )
    {
        //Selected item is a card
        int index = row-json.count()-1;

        QJsonArray arr = json.value(cardlistFieldName).toArray();
        arr.removeAt(index);
        json.insert(cardlistFieldName,QJsonValue(arr));

        cur_json_.setObject(json);

        populateListFromJson();
    }
}

void CardEdit::on_loadFileBtn_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,tr("Load cardlist or cardtype"), def_dir_,tr( "Cardlist (*.cards);; Cardtype (*.ctype)"));
    QFile file(file_name);
    file.open(QFile::ReadOnly);

    QByteArray data = file.readAll();
    file.close();

    cur_file_name_ = file_name;

    if( file_name.contains(".ctype") )
    {
        is_cardtype_ = true;
    }
    else
    {
        is_cardtype_ = false;
    }

    //Format file data with QJsonDocumentclass
    cur_json_ = QJsonDocument::fromJson(data);
    populateListFromJson();
}

void CardEdit::on_saveFileBtn_clicked()
{
    QString file_ending = is_cardtype_?"Cardtype (*.ctype)":"Cardlist (*.cards)";

    QString file_name = QFileDialog::getSaveFileName(this,tr("Save current list"),def_dir_+cur_file_name_, file_ending);

    QFile file(file_name);
    file.open(QFile::WriteOnly);
    file.write(cur_json_.toJson());
    file.close();

    if( is_cardtype_ )
    {
        CardFactory::getInstance().readCardtypes(def_dir_);
    }
}
