#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dictedit.h"
#include "cardedit.h"
#include "cardfactory.h"

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QGridLayout>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    def_dir_(""),
    cont_()
{
    ui->setupUi(this);

    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Choose data directory"),
                                                    "../",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    def_dir_ = dir_name;

    //Read cardtypes
    CardFactory::getInstance().readCardtypes( dir_name );

    //Insert .dict files to the menu list
    QDir dir(dir_name);
    dir.setFilter( QDir::Files);
    dir.setNameFilters(QStringList("*.dict"));
    QStringList dicts = dir.entryList();

    for( QString dict: dicts)
    {
        QAction* act = new QAction(dict,ui->menuDictionaries);
        act->setCheckable(true);
        ui->menuDictionaries->addAction(act);
    }

    ui->menuDictionaries->addSeparator();

    dir.setNameFilters(QStringList("*.cards"));
    dicts = dir.entryList();

    for( QString dict: dicts)
    {
        QAction* act = new QAction(dict,ui->menuDictionaries);
        act->setCheckable(true);
        ui->menuDictionaries->addAction(act);
    }

    //Use a stacked widget to display different views
    //Widgets hould be added in the order they are in the menu
    QStackedWidget* cont = new QStackedWidget(ui->centralWidget);
    QWidget* cardView = new QWidget(cont);
    cardView->setObjectName("CardView");
    cont->addWidget( cardView ); //View cards
    cont->addWidget( new DictEdit(cont, dir_name) ); //Dict edit view
    cont->addWidget( new CardEdit(cont,dir_name) ); //Card edit view
    cont->addWidget( makeCardPreview(cont) ); //Card type preview

    cont_ = cont;
    setCentralWidget(cont);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionCard_Preview_triggered()
{
    cont_->setCurrentWidget( cont_->findChild<QWidget*>("CardPreview") );
}

void MainWindow::on_actionEdit_Create_Dict_triggered()
{
//    qDebug("Activate edit");
//    ui->menuDictionaries->setEnabled(false);
//    DictEdit* cw = new DictEdit(this, def_dir_);
//    setCentralWidget(cw);
    cont_->setCurrentWidget( cont_->findChild<QWidget*>("DictEdit") );
}

void MainWindow::on_actionEdit_Create_Cardlist_triggered()
{
//    ui->menuDictionaries->setEnabled(false);
//    CardEdit* ce = new CardEdit(this, def_dir_);
//    setCentralWidget(ce);
    cont_->setCurrentWidget( cont_->findChild<QWidget*>("CardEdit") );
}

QWidget *MainWindow::makeCardPreview( QWidget* parent )
{
    //Preview the card qmls
//    QQmlEngine* engine = new QQmlEngine(this);
//    QQmlComponent component( engine, QUrl("qrc///cards/cardTypeDict.qml"), this);
//    component.loadUrl( QUrl("qrc///cards/cardTypeDict.qml") );
//    auto error = component.errorString();

//    QObject* object = component.create();
//    setCentralWidget( qobject_cast<QWidget*>(object) );

    QQuickWidget* card1 = new QQuickWidget(QUrl("qrc:///cardTypeDict.qml"));
    QQuickWidget* card2 = new QQuickWidget(QUrl("qrc:///cardTypeOneB.qml"));
    QQuickWidget* card3 = new QQuickWidget(QUrl("qrc:///cardTypeOneF.qml"));

    //ui->centralWidget->layout()->addWidget(card1);
    //ui->centralWidget->layout()->addWidget(card2);
    //ui->centralWidget->layout()->addWidget(card3);

    QWidget* wid = new QWidget(parent);
    wid->setObjectName("CardPreview");

    QGridLayout* lo = new QGridLayout(wid);
    lo->addWidget(card1,1,1,1,1);
    lo->addWidget(card2,2,1,1,1);
    lo->addWidget(card3,2,2,1,1);

    wid->setLayout(lo);

    return wid;
}

void MainWindow::on_actionView_Cards_triggered()
{
    cont_->setCurrentWidget( cont_->findChild<QWidget*>("CardView") );
}
