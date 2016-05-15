#include "dictedit.h"
#include "ui_dictedit.h"

#include <QFileDialog>
#include <QFile>
#include <QByteArray>
#include <QJsonObject>
#include <QStringList>
#include <QDebug>

DictEdit::DictEdit(QWidget *parent, QString def_dir) :
    QWidget(parent), ui(new Ui::DictEdit),  def_dir_(def_dir), cur_file_name_(""), cur_json_(QJsonDocument())
{
    qDebug("DictEdit building...");
    //cur_json_ = QJsonDocument();
    ui->setupUi(this);
}

DictEdit::~DictEdit()
{
    delete ui;
}

void DictEdit::on_loadButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,tr("Load dict or card"), def_dir_,tr( "Dicts (*.dict)"));
    QFile file(file_name);
    file.open(QFile::ReadOnly);

    QByteArray data = file.readAll();
    file.close();

    cur_file_name_ = file_name;

    //Format file data with QJsonDocumentclass
    cur_json_ = QJsonDocument::fromJson(data);
    populateListWithCurrentJson();
}

void DictEdit::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    qDebug("Item change");

    //Update edit fields when focus changes
    //Clear fields if firs or last row
    int row = ui->listWidget->row(current);

    if( isItem(row) )
    {
        ui->TranslationEdit->setEnabled(true);
        ui->WordEdit->setEnabled(true);
    }
    else
    {
        ui->TranslationEdit->setEnabled(false);
        ui->WordEdit->setEnabled(false);
    }

    QPair<QString,QString> item = getItemAt(row);
    ui->TranslationEdit->setText(item.second);
    ui->WordEdit->setText(item.first);
}

//Add a new item to list
void DictEdit::on_ItemBtn_clicked()
{
    addItemToJson("Word","Translation");
    populateListWithCurrentJson();

    ui->listWidget->setCurrentRow( 1 );//New item is added to the first spot
}

//Save current selection changes
void DictEdit::on_AddBtn_clicked()
{
    int row = ui->listWidget->currentRow();
    if( isItem(row) )
    {
        QPair<QString,QString> item = getItemAt(row);
        removeItemFromJson(item.first);

        QString new_word = ui->WordEdit->text();
        QString new_trans = ui->TranslationEdit->text();

        addItemToJson(new_word,new_trans);

        populateListWithCurrentJson();
    }
}

//Remove currently selected item if any
void DictEdit::on_delButton_clicked()
{
    int row = ui->listWidget->currentRow();
    if( isItem(row) )
    {
        QPair<QString,QString> item = getItemAt(row);
        removeItemFromJson(item.first);

        ui->WordEdit->setText("");
        ui->TranslationEdit->setText("");

        populateListWithCurrentJson();
    }
}

bool DictEdit::isItem(int row)
{
    if( row <= 0 || row >= ui->listWidget->count()-1 )
    {
        return false;
    }
    return true;
}

QPair<QString, QString> DictEdit::getItemAt(int row)
{
    //Get current json as object and iterate for # of rows
    //Actual rows begin at row 1 and is equal to begin
    if( !isItem( row ) )
    {
        return QPair<QString,QString>("",""); //Return empty if not a item
    }

    QJsonObject items = cur_json_.object();
    QJsonObject::ConstIterator item = items.begin();
    item += (row-1);

    return QPair<QString,QString>( item.key(), item.value().toString());
}

void DictEdit::populateListWithCurrentJson()
{
    QString text = cur_json_.toJson();
    //Clear current list
    ui->listWidget->clear();

    //Remove last empty lines
    ui->listWidget->addItems(text.split('\n',QString::SkipEmptyParts));
}

void DictEdit::removeItemFromJson(QString key)
{
    QJsonObject json = cur_json_.object();
    json.remove(key);
    cur_json_ = QJsonDocument(json);
}

void DictEdit::addItemToJson(QString key, QString value)
{
    QJsonObject json = cur_json_.object();
    json.insert(key,value);
    cur_json_ = QJsonDocument(json);
}

//Create new list
void DictEdit::on_NewBtn_clicked()
{
    cur_file_name_ = "";
    cur_json_ = QJsonDocument();
    ui->listWidget->clear();
    ui->ItemBtn->clicked();
}

//Save current list
void DictEdit::on_SaveBtn_clicked()
{
    QString file_name = QFileDialog::getSaveFileName(this,tr("Save current list"),def_dir_+cur_file_name_,tr( "Dicts (*.dict)"));

    QFile file(file_name);
    file.open(QFile::WriteOnly);
    file.write(cur_json_.toJson());
    file.close();
}
