#ifndef CARDLISTEDITOR_H
#define CARDLISTEDITOR_H

#include <QWidget>
#include <QString>
#include <QVariantList>
#include <QLayout>

#include "global.h"

namespace Ui {
class CardlistEditor;
}

class CardlistEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CardlistEditor(QWidget *parent = 0, QString def_dir = "");
    ~CardlistEditor();

    void setDir(const QString &def_dir);

private slots:
    void on_newListBtn_clicked();

    void on_newCardBtn_clicked();

    void on_openFileBtn_clicked();

    void on_saveFileBtn_clicked();

    void on_saveValBtn_clicked();

    void on_removeBtn_clicked();

    void on_curCardlistView_itemSelectionChanged();

    void set_unsaved_changes_status(); //Sets cur_card_saved_ to false

private:
    Ui::CardlistEditor *ui;

    QString def_dir_;
    QString cur_file_name_;
    bool cur_list_saved_; //Track if progress has been saved
    bool cur_card_saved_; //Check if currently selected card has altered values

    //Holds the current list in QVariantList form
    QVariantList cur_list_;

    const QByteArray INDENT = "    ";
    const QString FLABELOBJFORMAT = "%1Label";
    const QString FNAMEFORMAT = "%1: ";
    const QString FINDLABELOBJFORMAT = "indLabel%1";
    const QString FINDNAMEFORMAT = "Index %1: ";
    const QString FVALEDITOBJNAME = "fieldValEdit";
    static const QString FIELDBASEOBJNAME;
    static const QString FIELDVALBASEOBJNAME;
    static const QString FIELDLOOBJNAME;
    static const QString FIELDVALLOOBJNAME;

    //Populate view from cur_list_ (clear old list)
    void populateCardlistView();

    //Functions for manipulating the cardlistview
    void addRow(int row , const QVariant &card);
    void updateRow(int row , const QVariant &card);
    void removeRow( int row );

    //Add fields based on the given card to the field edit area
    void populateFieldEditArea(const QVariantMap &card);
    void insertFields( QLayout* lo, QVariant &fields );
    QWidget* newFieldBase(QString label_text, QString label_obj_name,
                          QString field_obj_name = FIELDBASEOBJNAME,
                          QString field_val_obj_name = FIELDVALBASEOBJNAME,
                          QString field_lo_obj_name = FIELDLOOBJNAME ,
                          QString field_val_lo_obj_name = FIELDVALLOOBJNAME);

    void saveFieldValue(QWidget *base, QVariant &fields); //Save values from the field line edits back to card.

    //Return the type id for the current combo box selection
    c_type_id_t getCurSelectedType();

    //Append a card template of a given type to the cur_list_ return reference to it
    QVariant &addNewCardTemplate(c_type_id_t type = c_type_id_t());

    int openUnsaveWorkDialog();
};

#endif // CARDLISTEDITOR_H
