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
    cardlist_(list)
{
    qDebug("CardViewer building...");

    ui->setupUi(this);

    //Initialize the qml view
    ui->cardViewWidget->rootContext()->setContextProperty("signalContext",this);
    ui->cardViewWidget->setSource(DECK);

    loader_ = ui->cardViewWidget->rootObject()->findChild<QObject*>("CardLoader");

    //Load test card
    QVariant param = QVariant::fromValue(QVariantMap());
    QVariant url = QVariant::fromValue(QUrl("qrc:/CardTypeOneF.qml"));
    QVariant im = QVariant::fromValue(true);
    QMetaObject::invokeMethod(loader_, global::LOAD_FUNC_NAME, Q_ARG(QVariant, url), Q_ARG(QVariant, param), Q_ARG(QVariant, im));
}

CardViewer::~CardViewer()
{
    delete ui;
}

void CardViewer::setCardlist(std::shared_ptr<Cardlist> cardlist)
{
    cardlist_ = cardlist;
}
