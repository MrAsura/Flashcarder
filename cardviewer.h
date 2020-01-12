#ifndef CARDVIEWER_H
#define CARDVIEWER_H

#include <QWidget>

#include <memory>
#include <QUrl>

#include "cardlist.h"

namespace Ui {
class CardViewer;
}

class CardViewer : public QWidget
{
    Q_OBJECT

signals:
    void moveLeft();
    void moveRight();
    void flip();

public:
    explicit CardViewer(QWidget *parent = 0, std::shared_ptr<Cardlist> list = nullptr);
    ~CardViewer();

    void resetCardlist(); //Reset cardlist position and move to the start.
    void setCardlist( std::shared_ptr<Cardlist> cardlist );
    QObject* getContext(); //Get context for cards

    void shuffle(); //Suffle cardlist
    void addCards(std::shared_ptr<Cardlist> cardlist); //Add cardlist to the current list

    void setFlipped(const unsigned &flipped);

public slots:
    void on_leftBtn_clicked();

    void on_rightBtn_clicked();

    void on_flipBtn_clicked();

private:
    Ui::CardViewer *ui;

    void updateView(); //Update view from cardlist_
    void updateLabel(); //Update the card count label

    const QUrl DECK = QUrl("qrc:/Deck.qml"); //The url for the base qml file

    std::shared_ptr<Cardlist> cardlist_;
    QObject* loader_;

    unsigned int card_num_; //Number of current card
    unsigned int card_count_; //Total number of cards

    unsigned flipped_; //If cards should be loaded flipped
};

#endif // CARDVIEWER_H
