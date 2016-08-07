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
#include <QQuickItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    def_dir_(""),
    cont_()
{
    ui->setupUi(this);

    qDebug() << (QDir::currentPath() + "/data");

    if( QDir(QDir::currentPath() + "/data").exists() )
    {
        //If a data dir exist use it as the default dir
        def_dir_ = QDir::currentPath() + "/data";
    }
    else
    {
        def_dir_ = getNewDir();
    }

    reloadDir();

    //Use a stacked widget to display different views
    //Widgets should be added in the order they are in the menu
    QStackedWidget* cont = new QStackedWidget(ui->centralWidget);
    QWidget* cardView = new QWidget(cont);
    cardView->setObjectName("CardView");
    cont->addWidget( cardView ); //View cards TODO: replace with proper widget?
    cont->addWidget( new DictEdit(cont, def_dir_) ); //Dict edit view
    cont->addWidget( new CardEdit(cont,def_dir_) ); //Card edit view
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
    QQuickWidget* test = new QQuickWidget(QUrl("qrc:///deck.qml"));
    QQuickWidget* loader = new QQuickWidget(QUrl("qrc:///cardLoader.qml"));

    //ui->centralWidget->layout()->addWidget(card1);
    //ui->centralWidget->layout()->addWidget(card2);
    //ui->centralWidget->layout()->addWidget(card3);

    QWidget* wid = new QWidget(parent);
    wid->setObjectName("CardPreview");

    QGridLayout* lo = new QGridLayout(wid);
    lo->addWidget(card1,1,1,1,1);
    lo->addWidget(card2,2,1,1,1);
    lo->addWidget(card3,2,2,1,1);
    lo->addWidget(test,1,2,1,1);
    lo->addWidget(loader,2,3,1,1);

    //Load a card as a test in the card loader
    QVariant url = "qrc:///cardTypeOneB.qml";
    QVariantMap param;
    param.insert("kanji","+");
    param.insert("num",50);
    QQuickItem* root = loader->rootObject();
    QObject* call_obj = root;
    QMetaObject::invokeMethod(call_obj,"load", Q_ARG(QVariant,url), Q_ARG(QVariant,QVariant::fromValue(param)));
    QMetaObject::invokeMethod(call_obj,"printTest");

    wid->setLayout(lo);

    return wid;
}

QString MainWindow::getNewDir()
{
    return QFileDialog::getExistingDirectory(this, tr("Choose data directory"),
                                             "../",
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
}

void MainWindow::reloadDir()
{
    //Reset current stuff
    ui->menuDictionaries->clear();

    //Read cardtypes
    CardFactory::getInstance().readCardtypes( def_dir_ );

    //Insert .dict files to the menu list
    QDir dir(def_dir_);
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
}

void MainWindow::reloadWidgets()
{
    //TODO: Add other widgets.
    cont_->findChild<DictEdit*>("DictEdit")->setDir(def_dir_);
    cont_->findChild<CardEdit*>("CardEdit")->setDir(def_dir_);
}

void MainWindow::on_actionView_Cards_triggered()
{
    cont_->setCurrentWidget( cont_->findChild<QWidget*>("CardView") );
}

void MainWindow::on_actionChange_Directory_triggered()
{
    //ask for a new dir and refresh
    def_dir_ = getNewDir();
    reloadDir();
    reloadWidgets();
}
