#ifndef CARDVIEWER_H
#define CARDVIEWER_H

#include <QWidget>

namespace Ui {
class CardViewer;
}

class CardViewer : public QWidget
{
    Q_OBJECT

public:
    explicit CardViewer(QWidget *parent = 0);
    ~CardViewer();

private:
    Ui::CardViewer *ui;
};

#endif // CARDVIEWER_H
