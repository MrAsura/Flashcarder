#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include <cardlist.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionCard_Preview_triggered();

    void on_actionEdit_Create_Dict_triggered();

    void on_actionEdit_Create_Cardlist_triggered();

    void on_actionView_Cards_triggered();

    void on_actionChange_Directory_triggered();

    void on_actionEdit_Create_Cardlists_triggered();

private:
    Ui::MainWindow *ui;

    QString def_dir_; //TODO: Use a proper dir type?
    QString def_type_dir_;
    QStackedWidget* cont_;

    QWidget* makeCardPreview(QWidget *parent);
    QString getNewDir(QString dir_to_ask_for = "data");
    void reloadDir(); //Update files based on def_dir_
    void reloadWidgets(); //Update widgets
    void reloadCardlist(); //Update cardlist
};

#endif // MAINWINDOW_H
