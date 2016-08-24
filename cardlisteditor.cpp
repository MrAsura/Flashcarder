#include "cardlisteditor.h"
#include "ui_cardlisteditor.h"

#include "cardfactory.h"

#include <QDebug>
#include <QStringList>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

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
    addNewCardTemplate();

    cur_list_saved_ = false;

    populateCardlistView();
}

void CardlistEditor::populateCardlistView()
{
    ui->curCardlistView->clear();

    //TODO: do a proper population
    QJsonDocument json = QJsonDocument::fromVariant(QVariant::fromValue(cur_list_));
    QByteArray temp = json.toJson();
    ui->curCardlistView->addItem(QString(temp));
}

c_type_id_t CardlistEditor::getCurSelectedType()
{
    return CardFactory::getInstance().name2id(ui->regCTypes->currentText());
}

void CardlistEditor::addNewCardTemplate(c_type_id_t type)
{
    type = type == c_type_id_t() ? getCurSelectedType() : type;
    cur_list_.insert(cur_list_.end(),CardFactory::getInstance().getCardTemplate(type));
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
    cur_list_saved_ = false;
}

void CardlistEditor::on_removeBtn_clicked()
{
    cur_list_saved_ = false;
}
