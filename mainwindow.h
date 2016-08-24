#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

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
    QStackedWidget* cont_;

    QWidget* makeCardPreview(QWidget *parent);
    QString getNewDir();
    void reloadDir(); //Update files based on def_dir_
    void reloadWidgets(); //Update widgets
};

#endif // MAINWINDOW_H
