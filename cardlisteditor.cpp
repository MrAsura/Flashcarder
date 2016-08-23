#include "cardlisteditor.h"
#include "ui_cardlisteditor.h"

CardlistEditor::CardlistEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardlistEditor)
{
    ui->setupUi(this);
}

CardlistEditor::~CardlistEditor()
{
    delete ui;
}
