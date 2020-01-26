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
    cont_(),
    is_shuffled_(false)
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
    QMetaObject::invokeMethod(call_obj,global::LOAD_FUNC_NAME, Q_ARG(QVariant,url), Q_ARG(QVariant,QVariant::fromValue(param)), Q_ARG(QVariant,QVariant::fromValue(true)),Q_ARG(QVariant,QVariant::fromValue(false)));
    QMetaObject::invokeMethod(call_obj,"printTest");

    //Try loading a card in the deck as well
    root = deck->rootObject();
    call_obj = root->findChild<QObject*>("CardLoader");
    url = QUrl::fromLocalFile(def_type_dir_+"/CardTypeOne.qml");
    QMetaObject::invokeMethod(call_obj,global::LOAD_FUNC_NAME, Q_ARG(QVariant,url), Q_ARG(QVariant,QVariant::fromValue(param)),Q_ARG(QVariant,QVariant::fromValue(true)),Q_ARG(QVariant,QVariant::fromValue(false)));

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

bool MainWindow::populateMenu(QMenu *menu, QDir &path)
{
    //Insert files to the menu list
    bool files_added = addFiles(menu, path);

    //Insert Directories
    bool dirs_added = addDirs(menu, path);

    return files_added || dirs_added;
}

bool MainWindow::addDirs(QMenu *parent, QDir &dirs)
{
    QFileInfoList dir_names = dirs.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    bool files_added = false;
    for(QFileInfo dir: dir_names)
    {
        QMenu *new_menu = parent->addMenu(dir.fileName());
        new_menu->setObjectName("actionMenu");
        QDir new_dir(dir.absoluteFilePath());

        if(populateMenu(new_menu, new_dir)) {
            files_added = true;
        }
        else
        {
            parent->removeAction(new_menu->menuAction());
        }
    }
    return files_added;
}

bool MainWindow::addFiles(QMenu *menu, QDir &files)
{
    QStringList type_list[] = {QStringList("*.dict"), QStringList("*.cards")};
    bool files_added = false;

    //Add select all button for selecting sub actions
    QAction *select = menu->addAction("Select All");
    select->setObjectName("actionSelectAll");
    select->setCheckable(true);
    menu->addSeparator();
    //connect(ui->actionClear, &QAction::triggered, select, &QAction::setChecked);

    for(QStringList &types: type_list)
    {
        files.setNameFilters(types);
        QFileInfoList entries = files.entryInfoList(types, QDir::Files);

        for(QFileInfo &entry: entries)
        {
            QAction* act = menu->addAction(entry.fileName());
            act->setData(entry.absoluteFilePath());
            act->setCheckable(true);
            files_added = true;

            //Add connections
            // Connect to clear and select all
            //connect(select, &QAction::triggered, act, &QAction::trigger);
            //connect(act, &QAction::triggered, select, &QAction::setChecked);
            //connect(ui->actionClear, &QAction::triggered, act, &QAction::setChecked);
        }
        if(entries.size() > 0) menu->addSeparator();
    }
    return files_added;
}

void MainWindow::reloadDir()
{
    //Reset current card list
    for (QAction *action : ui->menuDictionaries->actions())
    {
        if (action->objectName() != "actionShuffle" && action->objectName() != "actionFlipped" && action->objectName() != "actionClear")
        {
            ui->menuDictionaries->removeAction(action);
        }
    }
    //ui->menuDictionaries->clear();
    ui->menuDictionaries->addSeparator();

    //Read cardtypes
    CardFactory::getInstance().readCardtypes( def_dir_ );

    QDir dir(def_dir_);
    populateMenu(ui->menuDictionaries, dir);
}

void MainWindow::reloadWidgets()
{
    //TODO: Add other widgets. Error checking
    cont_->findChild<DictEdit*>("DictEdit")->setDir(def_dir_);
    cont_->findChild<CardEdit*>("CardEdit")->setDir(def_dir_);
    cont_->findChild<CardlistEditor*>("CardlistEditor")->setDir(def_dir_);
}

void MainWindow::recursiveAddCards(QMenu *menu, std::shared_ptr<Cardlist> list)
{
    for( QAction* action : menu->actions())
    {
        if(action->isChecked() && action->objectName() != "actionShuffle" && action->objectName() != "actionFlipped" && action->objectName() != "actionSelectAll")
        {
            list->combine(*addCards(action));
        }
        else if(action->menu())
        {
            recursiveAddCards(action->menu(), list);
        }
    }
}

void MainWindow::reloadCardlist()
{
    //Get proper cardlist from the selected cards
    std::shared_ptr<Cardlist> new_list( new Cardlist() );
    recursiveAddCards(ui->menuDictionaries, new_list);

    cont_->findChild<CardViewer*>("CardViewer")->setCardlist(new_list);
    cont_->findChild<CardViewer*>("CardViewer")->resetCardlist();
    if(is_shuffled_) cont_->findChild<CardViewer*>("CardViewer")->shuffle();
}

void MainWindow::recursiveSetActionChecked(QMenu *menu, bool checked)
{
    for(QAction *action: menu->actions())
    {
        if(action->isCheckable() && !action->isSeparator() && action->objectName() != "actionShuffle" && action->objectName() != "actionFlipped")
        {
            action->setChecked(checked);
        }
        else if(action->menu())
        {
            recursiveSetActionChecked(action->menu(), checked);
        }
    }
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
    is_shuffled_ = is_shuffled;
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
    if(!action->isSeparator() && action->objectName() != "actionShuffle" && action->objectName() != "actionFlipped")
    {
        qDebug("Dict menu action triggered");
        if(action->objectName() == "actionClear")
        {
            //Clear all the selections
            recursiveSetActionChecked(ui->menuDictionaries, false);
            reloadCardlist(); //Need to reset the cardlist
        }
        else if(action->objectName() == "actionSelectAll")
        {
            recursiveSetActionChecked(qobject_cast<QMenu*>(action->parentWidget()), action->isChecked());
            reloadCardlist();
        }
        else if(action->isChecked())
        {
            cont_->findChild<CardViewer*>("CardViewer")->addCards(addCards(action));
            if(is_shuffled_) cont_->findChild<CardViewer*>("CardViewer")->shuffle();
        }
        else
        {
            //Un-check the select all if it is checked
            QWidget *parent = action->parentWidget();
            while(parent->findChild<QAction*>("actionSelectAll"))
            {
                parent->findChild<QAction*>("actionSelectAll")->setChecked(false);
                parent = parent->parentWidget();
            }

            removeCards(action);
        }
    }
}

void MainWindow::on_actionFlipped_toggled(bool flipped)
{
    cont_->findChild<CardViewer*>("CardViewer")->setFlipped(flipped);
}
