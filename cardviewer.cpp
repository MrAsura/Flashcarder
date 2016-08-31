#include "cardviewer.h"
#include "ui_cardviewer.h"

CardViewer::CardViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardViewer)
{
    ui->setupUi(this);
}

CardViewer::~CardViewer()
{
    delete ui;
}
