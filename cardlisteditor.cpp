#include "cardlisteditor.h"
#include "ui_cardlisteditor.h"

#include "cardfactory.h"

#include <QDebug>
#include <QStringList>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>

#include <algorithm>

CardlistEditor::CardlistEditor(QWidget *parent, QString def_dir) :
    QWidget(parent),
    ui(new Ui::CardlistEditor),
    def_dir_(def_dir),
    cur_file_name_(""),
    cur_list_saved_(true),
    cur_list_(QVariantList())
{
    ui->setupUi(this);

    //Initialize the combobox here
    //qDebug("Initializing cardlist editor card types to: ");
    //qDebug((char *)CardFactory::getInstance().getCardTypeNames().join(',').data());
    ui->regCTypes->addItems(CardFactory::getInstance().getCardTypeNames());
}

CardlistEditor::~CardlistEditor()
{
    delete ui;
}

void CardlistEditor::setDir(const QString &def_dir)
{
    def_dir_ = def_dir;
}

void CardlistEditor::on_newListBtn_clicked()
{
    if( !cur_list_saved_ )
    {
        int ret_val = openUnsaveWorkDialog();
        if( ret_val == QMessageBox::Cancel ) return;
    }

    //Create a new list and insert a card from the currently selected type
    cur_list_.clear();
    cur_file_name_ = "";
    cur_list_saved_ = false;

    addNewCardTemplate();

    populateCardlistView();
}

void CardlistEditor::on_newCardBtn_clicked()
{
    //Add a new card template for the currently selected card type
    //New card is added to the second to last row and currently count() is the last index in the new list
    int new_row = ui->curCardlistView->count()-1;
    addRow(new_row, addNewCardTemplate() );

    cur_list_saved_ = false;

    //Set newly created card as the selected one
    ui->curCardlistView->setCurrentRow(ui->curCardlistView->count()-2);
}

void CardlistEditor::populateCardlistView()
{
    ui->curCardlistView->clear();

    //TODO: Try to make it so it doesn't breake if qt's json indenting etc. changes
    QByteArray list_json = QJsonDocument::fromVariant(QVariant::fromValue(cur_list_)).toJson();
    list_json.remove(list_json.size()-1,1); //Remove last newline

    //Find beg inds of each separate card
    QList<int> ind_list;
    ind_list.push_back(0); //Push first ind.

    for( QVariant card: cur_list_){
        QByteArray card_json = QJsonDocument::fromVariant(card).toJson();
        card_json.push_front('\n'); //So that prev part is only taken to the newline (no extra)
        card_json = card_json.left(card_json.size()-1).split('\n').join("\n"+INDENT);
        ind_list.push_back(list_json.indexOf(card_json,ind_list.last())+1); //Ind needs to point to the next char after \n
    }

    //Sort inds just in case
    std::sort(ind_list.begin(),ind_list.end(),std::less<int>());

    //Split list_json according to ind_list
    QStringList rows;
    for( int i = 0; i < ind_list.size()-1; i++ ){
        //We need to exclude newlines between row borders
        rows << list_json.mid( ind_list.at(i), ind_list.at(i+1) - ind_list.at(i) - 1); //-1 excludes \n from end
    }

    //Still need to handle the end of the string
    //Last char should be ] so exclude it and add it as it's on row
    rows << list_json.mid( ind_list.last(), list_json.size() - ind_list.last() - 2);
    rows << list_json.right(1);

    //Add rows to the view area and disable selection of the first and last row (the [] part of the json array)
    ui->curCardlistView->addItems(rows);
    QListWidgetItem* item = ui->curCardlistView->item(0);
    item->setFlags( item->flags() & ~Qt::ItemIsSelectable );
    item = ui->curCardlistView->item(ui->curCardlistView->count()-1);
    item->setFlags( item->flags() & ~Qt::ItemIsSelectable );
}

void CardlistEditor::addRow(int row, const QVariant& card)
{
    QByteArray card_json = QJsonDocument::fromVariant(card).toJson();

    //Check if row is last card row, if not we need to add a ',' else just remove \n
    //No row added yet so compensate row ind
    if( row == ui->curCardlistView->count() - 1 ){
        card_json.remove(card_json.size()-1,1); //Remove \n from the end
        //Also need to add ',' to prev last item
        QString new_text = ui->curCardlistView->item(row-1)->text();
        new_text.push_back(',');
        ui->curCardlistView->item(row-1)->setText(new_text);
    }
    else{
        card_json[card_json.size()-1] = ',';
    }
    //Need to align indentation
    card_json = card_json.split('\n').join("\n"+INDENT);
    card_json.push_front(INDENT); //Indent first bracket

    ui->curCardlistView->insertItem(row,card_json);
}

void CardlistEditor::updateRow(int row, const QVariant& card)
{
    QByteArray card_json = QJsonDocument::fromVariant(card).toJson();

    //Check if row is last card row, if not we need to add a ',' else just remove \n
    if( row == ui->curCardlistView->count() - 2 ){
        card_json.remove(card_json.size()-1,1); //Remove \n from the end
    }
    else{
        card_json[card_json.size()-1] = ',';
    }
    //Need to align indentation
    card_json = card_json.split('\n').join("\n"+INDENT);
    card_json.push_front(INDENT); //Indent first bracket

    ui->curCardlistView->item(row)->setText( card_json );
}

void CardlistEditor::removeRow(int row)
{
    QListWidgetItem* item = ui->curCardlistView->item(row);
    delete item;
}

c_type_id_t CardlistEditor::getCurSelectedType()
{
    return CardFactory::getInstance().name2id(ui->regCTypes->currentText());
}

QVariant& CardlistEditor::addNewCardTemplate(c_type_id_t type)
{
    type = type == c_type_id_t() ? getCurSelectedType() : type;
    cur_list_.push_back(CardFactory::getInstance().getCardTemplate(type));
    return cur_list_.last();
}

int CardlistEditor::openUnsaveWorkDialog()
{
    QMessageBox msg;
    msg.setText("There are unsaved changes in the current list.");
    msg.setInformativeText("Do you wan't to continue and lose your progress?");
    msg.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    return msg.exec();
}

void CardlistEditor::on_openFileBtn_clicked()
{
    if( !cur_list_saved_ )
    {
        int ret_val = openUnsaveWorkDialog();
        if( ret_val == QMessageBox::Cancel ) return;
    }

    QString file_name = QFileDialog::getOpenFileName(this,tr("Load cardlist"), def_dir_,tr( "Cardlist (*.cards)"));
    QFile file(file_name);
    file.open(QFile::ReadOnly);

    QByteArray data = file.readAll();
    file.close();

    cur_file_name_ = file_name;

    //Format file data with QJsonDocumentclass
    cur_list_ = QJsonDocument::fromJson(data).toVariant().toList();
    populateCardlistView();

    cur_list_saved_ = true;
}

void CardlistEditor::on_saveFileBtn_clicked()
{
    //TODO: Move file type to factory etc
    QString file_ending = "Cardlist (*.cards)";

    QString file_name = QFileDialog::getSaveFileName(this,tr("Save current list"),def_dir_+cur_file_name_, file_ending);

    QFile file(file_name);
    file.open(QFile::WriteOnly);
    file.write(QJsonDocument::fromVariant(QVariant::fromValue(cur_list_)).toJson());
    file.close();

    cur_list_saved_ = true;
}

void CardlistEditor::on_saveValBtn_clicked()
{
    int row = ui->curCardlistView->currentRow();

    //Update the row whose value has been changed
    //updateRow(row);

    cur_list_saved_ = false;
}

void CardlistEditor::on_removeBtn_clicked()
{
    //Remove the currently selected item
    int row = ui->curCardlistView->currentRow();
    int ind = row - 1; //Map row to ind in cur_list_ (first row is not a card)

    cur_list_.removeAt(ind);
    removeRow(row);

    cur_list_saved_ = false;
}
