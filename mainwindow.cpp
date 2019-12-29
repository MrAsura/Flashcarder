#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dictedit.h"
#include "cardedit.h"
#include "cardfactory.h"
#include "cardlisteditor.h"
#include "cardviewer.h"
#include "global.h"
#include "card.h"
using global::c_type_id_t;

#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QGridLayout>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickWidget>
#include <QQuickItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    def_dir_(""),
    def_type_dir_(""),
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

    //Initialize CardFactory here
    QDir type_dir = QDir(def_dir_+"/types");
    if( !type_dir.exists() ){
        type_dir = QDir(getNewDir("types"));
    }

    def_type_dir_ = type_dir.absolutePath();
    CardFactory::getInstance().initialize(type_dir);

    reloadDir();

    //Use a stacked widget to display different views
    //Widgets should be added in the order they are in the menu
    QStackedWidget* cont = new QStackedWidget(ui->centralWidget);

    cont->addWidget( new CardViewer(cont, std::shared_ptr<Cardlist>(new Cardlist())) ); //View cards
    cont->addWidget( new DictEdit(cont, def_dir_) ); //Dict edit view
    cont->addWidget( new CardEdit(cont,def_dir_) ); //Card edit view
    cont->addWidget( makeCardPreview(cont) ); //Card type preview
    cont->addWidget( new CardlistEditor(cont, def_dir_) );

    cont_ = cont;
    setCentralWidget(cont);

    reloadCardlist();
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

    QQuickWidget* card1 = new QQuickWidget(QUrl("qrc:///CardTypeDict.qml"));
    QQuickWidget* card2 = new QQuickWidget(QUrl("qrc:///CardTypeOneB.qml"));
    QQuickWidget* card3 = new QQuickWidget(QUrl("qrc:///CardTypeOneF.qml"));

    QQuickWidget* deck = new QQuickWidget;
    CardViewer view(parent, std::shared_ptr<Cardlist>(new Cardlist()));// = new CardViewer; //TODO: Leaks memory, fix
    deck->rootContext()->setContextProperty("signalContext", &view);
    deck->setSource(QUrl("qrc:///Deck.qml"));

    QQuickWidget* loader = new QQuickWidget(QUrl("qrc:///CardLoader.qml"));
    QQuickWidget* doublecard = new QQuickWidget(QUrl::fromLocalFile(def_type_dir_+"/CardTypeOne.qml"));

    //ui->centralWidget->layout()->addWidget(card1);
    //ui->centralWidget->layout()->addWidget(card2);
    //ui->centralWidget->layout()->addWidget(card3);

    QWidget* wid = new QWidget(parent);
    wid->setObjectName("CardPreview");

    QGridLayout* lo = new QGridLayout(wid);
    lo->addWidget(card1,1,1,1,1);
    lo->addWidget(card2,2,1,1,1);
    lo->addWidget(card3,2,2,1,1);
    lo->addWidget(deck,1,2,1,1);
    lo->addWidget(loader,2,3,1,1);
    lo->addWidget(doublecard,3,1,1,1);

    //Load a card as a test in the card loader
    QVariant url = "qrc:///CardTypeOneB.qml";
    QVariantMap param;
    param.insert("kanji","+");
    param.insert("num",50);
    QQuickItem* root = loader->rootObject();
    QObject* call_obj = root;
    QMetaObject::invokeMethod(call_obj,global::LOAD_FUNC_NAME, Q_ARG(QVariant,url), Q_ARG(QVariant,QVariant::fromValue(param)), Q_ARG(QVariant,QVariant::fromValue(true)));
    QMetaObject::invokeMethod(call_obj,"printTest");

    //Try loading a card in the deck as well
    root = deck->rootObject();
    call_obj = root->findChild<QObject*>("CardLoader");
    url = QUrl::fromLocalFile(def_type_dir_+"/CardTypeOne.qml");
    QMetaObject::invokeMethod(call_obj,global::LOAD_FUNC_NAME, Q_ARG(QVariant,url), Q_ARG(QVariant,QVariant::fromValue(param)),Q_ARG(QVariant,QVariant::fromValue(true)));

    wid->setLayout(lo);

    return wid;
}

QString MainWindow::getNewDir(QString dir_to_ask_for)
{
    return QFileDialog::getExistingDirectory(this, "Choose "+dir_to_ask_for+" directory",
                                             "../",
                                             QFileDialog::ShowDirsOnly
                                             | QFileDialog::DontResolveSymlinks);
}

void MainWindow::reloadDir()
{
    //Reset current card list
    for (QAction *action : ui->menuDictionaries->actions())
    {
        if (!action->isSeparator() && action->objectName() != "actionShuffle")
        {
            ui->menuDictionaries->removeAction(action);
        }
    }
    //ui->menuDictionaries->clear();

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
        act->setData(QDir::toNativeSeparators(def_dir_ + "/" + dict));
        act->setCheckable(true);
        ui->menuDictionaries->addAction(act);
    }

    ui->menuDictionaries->addSeparator();

    dir.setNameFilters(QStringList("*.cards"));
    dicts = dir.entryList();

    for( QString dict: dicts)
    {
        QAction* act = new QAction(dict,ui->menuDictionaries);
        act->setData(QDir::toNativeSeparators(def_dir_ + "/" + dict));
        act->setCheckable(true);
        ui->menuDictionaries->addAction(act);
    }
}

void MainWindow::reloadWidgets()
{
    //TODO: Add other widgets. Error checking
    cont_->findChild<DictEdit*>("DictEdit")->setDir(def_dir_);
    cont_->findChild<CardEdit*>("CardEdit")->setDir(def_dir_);
    cont_->findChild<CardlistEditor*>("CardlistEditor")->setDir(def_dir_);
}

void MainWindow::reloadCardlist()
{
    //Get proper cardlist from the selected cards
    std::shared_ptr<Cardlist> new_list( new Cardlist() );
    for( QAction* action : ui->menuDictionaries->actions())
    {
        if(action->isChecked() && !action->isSeparator() && action->objectName() != "Shuffle")
        {
            new_list->combine(*addCards(action));
        }
    }

    cont_->findChild<CardViewer*>("CardViewer")->setCardlist(new_list);
    cont_->findChild<CardViewer*>("CardViewer")->resetCardlist();
}

std::shared_ptr<Cardlist> MainWindow::addCards(QAction *action)
{
    QObject *context = cont_->findChild<CardViewer*>("CardViewer")->getContext();
    std::shared_ptr<Cardlist> list = CardFactory::getInstance().getCardlist(action->data().toString(), context);
//    if(card_cache_.contains(action->objectName()))
//    {
//        //Remove existing cards before adding new
//        removeCards(action);
//    }
/*    QList<Cardlist::card_ptr> cache_list;
    list->reset();
    do{
        cache_list.append(list->current());
    } while (list->current() != list->next());
    list->reset();
    card_cache_[action->objectName()] = cache_list;*/
    return list;
}

void MainWindow::removeCards(QAction *action)
{
    //Todo: Add a more fancy card management cheme
    if(!action->isChecked())
    {
        reloadCardlist(); //Bruteforce method
    }
}

void MainWindow::on_actionView_Cards_triggered()
{
    cont_->setCurrentWidget( cont_->findChild<QWidget*>("CardViewer") );
}

void MainWindow::on_actionChange_Directory_triggered()
{
    //ask for a new dir and refresh
    def_dir_ = getNewDir();
    reloadDir();
    reloadWidgets();
}

void MainWindow::on_actionEdit_Create_Cardlists_triggered()
{
    //Switch to cardlist editor widget
    cont_->setCurrentWidget(cont_->findChild<QWidget*>("CardlistEditor"));
}

void MainWindow::on_actionShuffle_toggled(bool is_shuffled)
{
    if(is_shuffled){
        cont_->findChild<CardViewer*>("CardViewer")->shuffle();
    }
    else
    {
        //Reload selected cards
        reloadCardlist();
    }
}

void MainWindow::on_menuDictionaries_triggered(QAction *action)
{
    if(!action->isSeparator() && action->objectName() != "actionShuffle")
    {
        qDebug("Dict menu action triggered");
        if(action->isChecked())
        {
            //addCards(action);
            reloadCardlist();
        }
        else
        {
            removeCards(action);
        }
    }
}
