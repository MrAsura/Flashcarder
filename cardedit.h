#ifndef CARDEDIT_H
#define CARDEDIT_H

#include <QWidget>
#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>

namespace Ui {
class CardEdit;
}

class CardEdit : public QWidget
{
    Q_OBJECT

public:
    explicit CardEdit(QWidget *parent = 0, QString def_dir = "");
    ~CardEdit();

public slots:
    void setDir( QString dir );

private slots:
    void on_newCtBtn_clicked();

    void on_newClBtn_clicked();

    void on_newCardBtn_clicked();

    void on_itemList_currentRowChanged(int currentRow);

    void on_saveValueBtn_clicked();

    void on_removeCardBtn_clicked();

    void on_loadFileBtn_clicked();

    void on_saveFileBtn_clicked();

private:
    Ui::CardEdit *ui;
    QString def_dir_;
    QJsonDocument cur_json_;

    QList<int> non_item_Rows_;//Contains row that dont contain items
    bool is_cardtype_;
    QString cur_file_name_;

    const QString cardtypeFieldName = "cardTypeID";
    const QString cardlistFieldName = "cards";
    const QString editFieldIdPostfix = "Field";

    void populateListFromJson();

    bool isItem( int row ); //Return if row contains an item or not

    //Make the field and label of a field  and add them to parent
    void addField( QWidget* parent, QString label, QString field_val );

    void errorDlg( QString msg ); //Opens a error dialog

};

#endif // CARDEDIT_H
