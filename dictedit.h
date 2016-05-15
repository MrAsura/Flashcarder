#ifndef DICTEDIT_H
#define DICTEDIT_H

#include <QWidget>

#include <QListWidgetItem>
#include <QJsonDocument>
#include <QPair>
#include <QStringList>

namespace Ui {
class DictEdit;
}

class DictEdit : public QWidget
{
    Q_OBJECT

public:
    explicit DictEdit(QWidget *parent = 0, QString def_dir = "");
    ~DictEdit();

private slots:
    void on_loadButton_clicked();

    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *);

    void on_ItemBtn_clicked();

    void on_AddBtn_clicked();

    void on_delButton_clicked();

    void on_NewBtn_clicked();

    void on_SaveBtn_clicked();

private:
    Ui::DictEdit *ui;

    QString def_dir_;
    QString cur_file_name_;
    QJsonDocument cur_json_;

    bool isItem( int row ); //Check if given row is a item row
    QPair<QString,QString> getItemAt( int row );
    void populateListWithCurrentJson();

    void removeItemFromJson( QString key );
    void addItemToJson( QString key, QString value );
};

#endif // DICTEDIT_H
