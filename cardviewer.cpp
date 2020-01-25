#include "cardviewer.h"
#include "ui_cardviewer.h"
#include "global.h"
using global::c_type_id_t;

#include <QQmlContext>
#include <QObject>
#include <QQuickItem>

CardViewer::CardViewer(QWidget *parent, std::shared_ptr<Cardlist> list) :
    QWidget(parent),
    ui(new Ui::CardViewer),
    cardlist_(list),
    card_num_(0),
    card_count_(0),
    flipped_(false)
{
    qDebug("CardViewer building...");

    ui->setupUi(this);

    //Initialize the qml view
    ui->cardViewWidget->rootContext()->setContextProperty("signalContext",this);
    ui->cardViewWidget->setSource(DECK);

    loader_ = ui->cardViewWidget->rootObject()->findChild<QObject*>("CardLoader");

    updateView();
    //Load test card
    //QVariant param = QVariant::fromValue(QVariantMap());
    //QVariant url = QVariant::fromValue(QUrl("qrc:/CardTypeOneF.qml"));
    //QVariant im = QVariant::fromValue(true);
    //QMetaObject::invokeMethod(loader_, global::LOAD_FUNC_NAME, Q_ARG(QVariant, url), Q_ARG(QVariant, param), Q_ARG(QVariant, im));
}

CardViewer::~CardViewer()
{
    delete ui;
}

void CardViewer::resetCardlist()
{
    updateView();
}

void CardViewer::setCardlist(std::shared_ptr<Cardlist> cardlist)
{
    cardlist_ = cardlist;
}

QObject *CardViewer::getContext()
{
    return loader_;
}

void CardViewer::shuffle()
{
    cardlist_->shuffle();
    resetCardlist();
}

void CardViewer::addCards(std::shared_ptr<Cardlist> cardlist)
{
    cardlist_->combine(*cardlist);
    updateView();
}

void CardViewer::updateView()
{
    card_num_ = 0;
    card_count_ = cardlist_->size();

    //Display firts card
    if (card_count_ > 0)
    {
        card_num_ = 1;
        cardlist_->first()->display(flipped_, true);
    }
    else {
        reset();
    }

    updateLabel();
}

void CardViewer::updateLabel()
{
    //Set number of cards
    ui->cardCountLabel->setText(QString("%1 of %2").arg(card_num_).arg(card_count_));
}

void CardViewer::setFlipped(const unsigned &flipped)
{
    flipped_ = flipped;
}

void CardViewer::on_leftBtn_clicked()
{
    //Load card thats on the left if it exists
    if(cardlist_->peek_prev())
    {
        //Prev card was not the current card so we haven't reach the start yet. Show new card
        cardlist_->prev()->display(flipped_); //prev() sets current() to the prev card value
        emit moveRight(); //Shows the transition and new card

        //Update card num label
        card_num_--;
        updateLabel();

        return;
    }
    //First card reached (no prev card) don't do anything
}

void CardViewer::on_rightBtn_clicked()
{
    //Load card thats on the right if it exists
    if(cardlist_->peek_next())
    {
        //Next card was not the current card so we haven't reach the end yet. Show new card
        cardlist_->next()->display(flipped_); //next() sets current() to the next card value
        emit moveLeft(); //Shows the transition and new card

        //Update card num label
        card_num_++;
        updateLabel();

        return;
    }
    //Last card reached (no next card) don't do anything
}

void CardViewer::on_flipBtn_clicked()
{
    emit flip();
}
