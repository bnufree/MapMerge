#include "zchxecdispopupwidget.h"
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>

zchxECdisPopupWidget::zchxECdisPopupWidget(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
}

zchxECdisPopupWidget::~zchxECdisPopupWidget()
{
    qDebug()<<"disconstruct"<<this;
}


void zchxECdisPopupWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

zchxVectorMapSourceWidget::zchxVectorMapSourceWidget(const QString& msg, QWidget *parent) : zchxECdisPopupWidget(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setLayout(new QVBoxLayout);
    this->layout()->addWidget(new QLabel(msg, this));

    QPushButton *btn = new QPushButton(QStringLiteral("重新设定地图数据目录"), this);
    btn->setFlat(true);
    this->layout()->addWidget(btn);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(slotSelDir()));
}

void zchxVectorMapSourceWidget::slotSelDir()
{
    emit signalSelDir();
    emit signalAbouttoClose();
}
