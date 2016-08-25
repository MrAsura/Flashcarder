#ifndef CARDLISTEDITOR_H
#define CARDLISTEDITOR_H

#include <QWidget>
#include <QString>
#include <QVariantList>

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

private:
    Ui::CardlistEditor *ui;

    QString def_dir_;
    QString cur_file_name_;
    bool cur_list_saved_; //Track if progress has been saved

    //Holds the current list in QVariantList form
    QVariantList cur_list_;

    const QByteArray INDENT = "    ";

    //Populate view from cur_list_ (clear old list)
    void populateCardlistView();
    //Functions for manipulating the cardlistview
    void addRow(int row , const QVariant &card);
    void updateRow(int row , const QVariant &card);
    void removeRow( int row );

    //Return the type id for the current combo box selection
    c_type_id_t getCurSelectedType();

    //Append a card template of a given type to the cur_list_ return reference to it
    QVariant &addNewCardTemplate(c_type_id_t type = c_type_id_t());

    int openUnsaveWorkDialog();
};

#endif // CARDLISTEDITOR_H
