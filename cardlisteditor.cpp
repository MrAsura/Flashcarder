#include "cardlisteditor.h"
#include "ui_cardlisteditor.h"

#include "cardfactory.h"

#include <QDebug>
#include <QStringList>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QQuickItem>

#include <algorithm>

const QString CardlistEditor::FIELDBASEOBJNAME = "fieldBase";
const QString CardlistEditor::FIELDVALBASEOBJNAME = "fieldValBase";
const QString CardlistEditor::FIELDLOOBJNAME = "fieldBaseLo";
const QString CardlistEditor::FIELDVALLOOBJNAME = "fieldVALBaseLo";


CardlistEditor::CardlistEditor(QWidget *parent, QString def_dir) :
    QWidget(parent),
    ui(new Ui::CardlistEditor),
    def_dir_(def_dir),
    cur_file_name_(""),
    cur_file_(nullptr),
    cur_list_saved_(true),
    cur_card_saved_(true),
    cur_list_(QVariantList())
{
    qDebug("CardlistEditor building...");

    ui->setupUi(this);

    //Initialize the combobox here
    //qDebug("Initializing cardlist editor card types to: ");
    //qDebug((char *)CardFactory::getInstance().getCardTypeNames().join(',').data());
    ui->regCTypes->addItems(CardFactory::getInstance().getCardTypeNames());

    //Init preview with a card loader
    ui->previewView->setSource(PREVIEWER);

    //Hide unsaved label
    ui->notSavedLabel->hide();
}

CardlistEditor::~CardlistEditor()
{
    delete cur_file_;
    delete ui;
}

void CardlistEditor::setDir(const QString &def_dir)
{
    def_dir_ = def_dir;
}

void CardlistEditor::on_newListBtn_clicked()
{
    if( !cur_list_saved_ || !cur_card_saved_  )
    {
        int ret_val = openUnsaveWorkDialog();
        if( ret_val == QMessageBox::Cancel ) return;
    }

    //Create a new list and insert a card from the currently selected type
    cur_list_.clear();
    cur_file_name_ = "";
    delete cur_file_;
    cur_file_ = nullptr;
    setCurListSaved(false);

    addNewCardTemplate();

    populateCardlistView();
}

void CardlistEditor::on_newCardBtn_clicked()
{
    //If new card button is pressed when no list exists, generate a new list instead
    if( cur_list_.empty() ){
        on_newListBtn_clicked();
        return;
    }

    //Add a new card template for the currently selected card type
    //New card is added to the second to last row and currently count() is the last index in the new list
    int new_row = ui->curCardlistView->count()-1;
    addRow(new_row, addNewCardTemplate() );

    setCurListSaved(false);

    //Set newly created card as the selected one
    ui->curCardlistView->setCurrentRow(ui->curCardlistView->count()-2);
}

void CardlistEditor::populateCardlistView()
{
    ui->curCardlistView->setCurrentRow(-1); //Make sure cur_list_ is not indexed when clear triggers a selection change event
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

void CardlistEditor::populateFieldEditArea(const QVariantMap& card)
{
    //Build a new widget that holds the final content
    QWidget* base = new QWidget();
    base->setObjectName(ui->fieldEditArea->widget()->objectName());

    //Base layout
    QVBoxLayout* base_lo = new QVBoxLayout();
    base_lo->setObjectName("fieldEditLo");

    //Populate layout
    QVariant fields = QVariant::fromValue(card);
    insertFields(base_lo, fields);

    //Finally set new widget ( destroys old content )
    base->setLayout(base_lo);
    ui->fieldEditArea->setWidget(base);
}

void CardlistEditor::insertFields(QLayout *lo, QVariant &fields)
{
    //Loop over all the fields and add a new widget for each
    //Field widget contains a lo with a field name label and a text edit for the value
    //If the value is not a simple type (string, int, etc.) recursively call insertFields
    //If fields is a QVariantMap use keys as the field names,
    //but if it is a QVariantList use the index as the field label name.
    if( fields.canConvert<QVariantMap>() )
    {
        QVariantMap field_map = fields.toMap();
        //Loop over fields
        for( QString key: field_map.keys() )
        {
            //Init new field base
            QWidget* field_base = newFieldBase(FNAMEFORMAT.arg(key),FLABELOBJFORMAT.arg(key));

            //Add field's value. Layout already set in field_base
            QWidget* field_val = field_base->findChild<QWidget*>(FIELDVALBASEOBJNAME);
            if( field_val ){
                insertFields(field_val->layout(), field_map[key]);
            }
            else{
                qDebug("Couldn't find field val widget"); //TODO: throw error etc.
            }

            //Put it all together
            lo->addWidget(field_base);
        }

        return;
    }
    else if( fields.canConvert<QVariantList>() )
    {
        QVariantList field_list = fields.toList();
        //Loop over content
        for( int ind = 0; field_list.size(); ind++ )
        {
            //Get new field base
            QWidget* field_base = newFieldBase(FINDNAMEFORMAT.arg(ind),FINDLABELOBJFORMAT.arg(ind));

            //Add field's value. Layout already set in field_base
            QWidget* field_val = field_base->findChild<QWidget*>(FIELDVALBASEOBJNAME);
            if( field_val ){
                insertFields(field_val->layout(), field_list[ind]);
            }
            else{
                qDebug("Couldn't find field val widget"); //TODO: throw error etc.
            }

            //Put it all together
            lo->addWidget(field_base);
        }

        return;
    }

    //If we make it here, fields should contain a simple type
    //Needs to be convertable to string else use empty string
    if ( !fields.canConvert<QString>()) {
        //Should not end here. TODO: throw exception?
        qDebug((QString("Inproper type: ")+ QString(fields.typeName())).toLocal8Bit());
        //return;
    }

    //Insert a line edit to the field with the value given by fields
    QLineEdit* val_edit = new QLineEdit();
    val_edit->setObjectName(FVALEDITOBJNAME);
    val_edit->setText(fields.toString());

    //Connect val edit to saved status
    connect(val_edit, SIGNAL(textEdited(QString)), this, SLOT(set_unsaved_changes_status()));

    //Add to layout
    lo->addWidget(val_edit);
}

QWidget *CardlistEditor::newFieldBase(QString label_text, QString label_obj_name, QString field_obj_name, QString field_val_obj_name, QString field_lo_obj_name, QString field_val_lo_obj_name )
{
    //Create a base widget to hold the current field label and its respective field val widget (in a layout)
    //We add a blank widget to the field val "slot" since the line edit is added later
    QWidget* field_base = new QWidget();
    QWidget* field_val_base = new QWidget();
    QHBoxLayout* field_base_lo = new QHBoxLayout();
    QVBoxLayout* field_val_base_lo = new QVBoxLayout();
    field_base->setObjectName(field_obj_name);
    field_val_base->setObjectName(field_val_obj_name);
    field_base_lo->setObjectName(field_lo_obj_name);
    field_val_base_lo->setObjectName(field_val_lo_obj_name);

    //Add labe with field name and field val
    QLabel* field_name = new QLabel(label_text);
    field_name->setObjectName(label_obj_name);
    field_name->setAlignment(Qt::AlignTop|field_name->alignment());

    field_base_lo->addWidget(field_name);
    field_base_lo->addWidget(field_val_base);

    //Set layouts
    field_val_base->setLayout(field_val_base_lo);
    field_base->setLayout(field_base_lo);

    return field_base;
}

void CardlistEditor::saveFieldValue(QWidget *base, QVariant &fields)
{
    //Loop over all the fields
    //Field widget contains a lo with a field name label and a text edit for the value
    //If the value is not a simple type (string, int, etc.) recursively call insertFields
    //If fields is a QVariantMap use keys as the field names,
    //but if it is a QVariantList use the index as the field label name.
    //A simple value should mean a line edit exists with a value that is placed back to card
    if( fields.canConvert<QVariantMap>() )
    {
        QVariantMap field_map = fields.toMap();
        //Loop over fields
        for( QString key: field_map.keys() )
        {
            //Find base field widget for the field relevant to key-field
            QLabel* field_label = base->findChild<QLabel*>(FLABELOBJFORMAT.arg(key));
            if( field_label ){
                saveFieldValue(field_label->parentWidget(), field_map[key]);
            }
            else{
                qDebug("Couldn't find field label");
            }
        }

        //Need to update fields with the new field map
        fields.setValue(field_map);

        return;
    }
    else if( fields.canConvert<QVariantList>() )
    {
        QVariantList field_list = fields.toList();
        //Loop over content
        for( int ind = 0; field_list.size(); ind++ )
        {
            //Find base field widget for the field relevant to key-field
            QLabel* field_ind_label = base->findChild<QLabel*>(FINDLABELOBJFORMAT.arg(ind));
            if( field_ind_label ){
                saveFieldValue(field_ind_label->parentWidget(), field_list[ind]);
            }
            else{
                qDebug("Couldn't find field ind label");
            }
        }

        //Need to update fields with the new field list
        fields.setValue(field_list);

        return;
    }

    //If we make it here, fields should contain a simple type
    //Find the line edit and get new value. Try to convert to the card type
    QLineEdit* new_val = base->findChild<QLineEdit*>(FVALEDITOBJNAME);
    if(new_val){
        bool ok = true;
        switch (fields.type()) {
        case QVariant::Int:
            fields.setValue(new_val->text().toInt(&ok));
            break;
        case QVariant::String:
            fields.setValue(new_val->text());
            break;
        case QVariant::Double:
            fields.setValue(new_val->text().toDouble(&ok));
            break;
        case QVariant::Bool:
            fields.setValue(new_val->text().toLower()=="true"?true:false);
            break;
        default:
            qDebug("Uncaught type");//TODO: Throw error?
            break;
        }
        if(!ok){
            qDebug("Faild to convert to type: ");
            qDebug(fields.typeName());
        }
    }
    else{
        qDebug("Couldn't find line edit");
    }
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
    msg.setInformativeText("Do you want to continue and lose your progress?");
    msg.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msg.setDefaultButton(QMessageBox::Cancel);
    return msg.exec();
}

bool CardlistEditor::openNewFile(bool open_save_file)
{
    QString file_name = "";
    //TODO: Move file type to factory etc
    if(open_save_file)
    {
        file_name = QFileDialog::getSaveFileName(this,tr("Save current list"),def_dir_+cur_file_name_, "Cardlist (*.cards)");
    }
    else {
        file_name = QFileDialog::getOpenFileName(this,tr("Load cardlist"), def_dir_, "Cardlist (*.cards)");
    }

    if(file_name == "")
    {
        //If filename empty assume cancle
        return false;
    }

    if(cur_file_){
        cur_file_->close();
        delete cur_file_;
    }
    cur_file_ = new QFile(file_name);
    cur_file_name_ = "";

    if(cur_file_->exists())
    {
        if(!cur_file_->open(QFile::ReadWrite | QFile::Text))
        {
            delete cur_file_;
            cur_file_ = nullptr;
            QMessageBox::warning(this, "FlashCarder", "Failed to open file");
            return false;
        }

    }
    else
    {
        delete cur_file_;
        cur_file_ = nullptr;
        QMessageBox::warning(this, "FlashCarder", "File does not exist");
        return false;
    }

    cur_file_name_ = file_name;

    return true;
}

bool CardlistEditor::saveCurList()
{
    bool ret_val = cur_file_->reset();
    ret_val = ret_val && (-1 != cur_file_->write(QJsonDocument::fromVariant(QVariant::fromValue(cur_list_)).toJson()));
    ret_val = ret_val && cur_file_->resize(cur_file_->pos());
    //file.close();
    return ret_val;
}

void CardlistEditor::on_openFileBtn_clicked()
{
    if( !cur_list_saved_ || !cur_card_saved_  )
    {
        int ret_val = openUnsaveWorkDialog();
        if( ret_val == QMessageBox::Cancel ) return;
    }

    if(openNewFile())
    {
        QByteArray data = cur_file_->readAll();
        //cur_file_->close();

        //Format file data with QJsonDocumentclass
        cur_list_ = QJsonDocument::fromJson(data).toVariant().toList();
        populateCardlistView();

        setCurListSaved(true);
    }
}

void CardlistEditor::on_saveFileBtn_clicked()
{
    if(!cur_list_saved_)
    {
        if(!cur_file_)
        {
            if(!openNewFile(true))
            {
                return;
            }
        }
        if(saveCurList())
        {
          setCurListSaved(true);
        }
    }
}

void CardlistEditor::on_saveValBtn_clicked()
{
    int row = ui->curCardlistView->currentRow();
    int ind = row - 1;

    if( ind >= 0 && ind < cur_list_.count())
    {
        //Update val of cur_list_
        saveFieldValue(ui->fieldEditArea->widget(), cur_list_[ind]);
        updatePreview(cur_list_[ind].toMap());

        //Update the row whose value has been changed
        updateRow(row, cur_list_[ind]);

        setCurListSaved(false);
        cur_card_saved_ = true;
    }
}

void CardlistEditor::on_removeBtn_clicked()
{
    //Remove the currently selected item if its not the first or last item (end marks)
    int row = ui->curCardlistView->currentRow();
    int ind = row - 1; //Map row to ind in cur_list_ (first row is not a card)

    if(ind >= 0 && ind < cur_list_.count())
    {
        cur_list_.removeAt(ind);
        removeRow(row);

        if(cur_list_.count() == 0){
            ui->curCardlistView->clear();
            return;
        }
        else {
            setCurListSaved(false);
        }

        if(ind == cur_list_.count()){
            //Need to remove ',' from new last
            updateRow(row - 1, cur_list_.back());
        }
    }
}

void CardlistEditor::on_curCardlistView_itemSelectionChanged()
{
    //TODO: Open a confirmation dialog if fields have been edited but not saved. Need to do somewhere else?

    //Get currently selected card's ind
    int ind = ui->curCardlistView->currentRow() - 1;

    //If selection change triggered by removal, need to decrease ind by one to get correct card
    if(cur_list_.count() < ui->curCardlistView->count() - 2) ind--;

    if( ind >= 0 && ind < cur_list_.count()){
        QVariantMap card = cur_list_[ind].toMap();
        populateFieldEditArea(card);
        updatePreview(card);

        //Disaple editing on the card type field
        QWidget* c_type_id_label = ui->fieldEditArea->widget()->findChild<QWidget*>(FLABELOBJFORMAT.arg(CardFactory::cardTmplIdFieldName));
        if( c_type_id_label ){
            QWidget* c_type_id_val = c_type_id_label->parentWidget()->findChild<QLineEdit*>(FVALEDITOBJNAME);
            if( c_type_id_val ){
                c_type_id_val->setEnabled(false);
            }
        }
    }
    else{
        //No card selected, so set empty widget
        QWidget* empty = new QWidget();
        empty->setObjectName("fieldEditContents");
        ui->fieldEditArea->setWidget(empty);
        updatePreview(QVariantMap());
    }
}

void CardlistEditor::set_unsaved_changes_status()
{
    cur_card_saved_ = false;
}

void CardlistEditor::updatePreview(QVariantMap card)
{
    if(card.empty()){
        //Inproper card. Clear preview
        ui->previewView->rootObject()->setVisible(false);
        return;
    }

    //Load card with the loader in preview
    c_type_id_t type = CardFactory::getInstance().name2id(card[CardFactory::cardTmplIdFieldName].toString());
    QVariant url = QVariant::fromValue(CardFactory::getInstance().getUrl(type));
    QVariant param = card[CardFactory::cardTmplDataFieldName];
    QVariant immediate = QVariant::fromValue(true);
    QVariant flipped = QVariant::fromValue(false);
    QObject* loader = ui->previewView->rootObject();

    QMetaObject::invokeMethod( loader, global::LOAD_FUNC_NAME, Q_ARG(QVariant, url), Q_ARG(QVariant, param), Q_ARG(QVariant, immediate), Q_ARG(QVariant, flipped));

    ui->previewView->rootObject()->setVisible(true);
}

void CardlistEditor::on_saveAsFileBtn_clicked()
{
    if(!openNewFile(true))
    {
        return;
    }
    if(saveCurList())
    {
        setCurListSaved(true);
    }
}

void CardlistEditor::setCurListSaved(bool value)
{
    cur_list_saved_ = value;
    if(value){
        ui->notSavedLabel->hide();
        ui->savedLable->show();
    }
    else{
        ui->notSavedLabel->show();
        ui->savedLable->hide();
    }
}
