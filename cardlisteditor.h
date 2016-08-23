#ifndef CARDLISTEDITOR_H
#define CARDLISTEDITOR_H

#include <QWidget>

namespace Ui {
class CardlistEditor;
}

class CardlistEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CardlistEditor(QWidget *parent = 0);
    ~CardlistEditor();

private:
    Ui::CardlistEditor *ui;
};

#endif // CARDLISTEDITOR_H
