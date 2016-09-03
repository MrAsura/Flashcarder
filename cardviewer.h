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

public:
    explicit CardViewer(QWidget *parent = 0, std::shared_ptr<Cardlist> list = nullptr);
    ~CardViewer();

    void setCardlist( std::shared_ptr<Cardlist> cardlist );

private:
    Ui::CardViewer *ui;

    const QUrl DECK = QUrl("qrc:///Deck.qml"); //The url for the base qml file

    std::shared_ptr<Cardlist> cardlist_;
    QObject* loader_;
};

#endif // CARDVIEWER_H
