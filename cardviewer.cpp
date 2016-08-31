#include "cardviewer.h"
#include "ui_cardviewer.h"

CardViewer::CardViewer(QWidget *parent, std::shared_ptr<Cardlist> list) :
    QWidget(parent),
    ui(new Ui::CardViewer),
    cardlist_(list)
{
    ui->setupUi(this);
}

CardViewer::~CardViewer()
{
    delete ui;
}

void CardViewer::setCardlist(std::shared_ptr<Cardlist> cardlist)
{
    cardlist_ = cardlist;
}
