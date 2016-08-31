#ifndef CARDVIEWER_H
#define CARDVIEWER_H

#include <QWidget>

#include <memory>

#include "cardlist.h"

namespace Ui {
class CardViewer;
}

class CardViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CardViewer(QWidget *parent = 0, std::shared_ptr<Cardlist> list = nullptr);
    ~CardViewer();

    void setCardlist( std::shared_ptr<Cardlist> cardlist );

private:
    Ui::CardViewer *ui;

    std::shared_ptr<Cardlist> cardlist_;
};

#endif // CARDVIEWER_H
