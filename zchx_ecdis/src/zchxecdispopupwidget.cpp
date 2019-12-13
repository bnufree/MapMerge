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
    this->setFixedHeight(80);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setLayout(new QVBoxLayout);
    this->layout()->addWidget(new QLabel(msg, this));


    QPushButton *btn = new QPushButton(QStringLiteral("重新设定"), this);
    btn->setStyleSheet("QPushButton{border:1px solid gray; min-height:30px; width:100px;} QPushButton:hover{color:blue;}");
    btn->setFlat(true);
    this->layout()->addWidget(btn);
    this->layout()->setAlignment(btn, Qt::AlignCenter);
    connect(btn, SIGNAL(clicked(bool)), this, SLOT(slotSelDir()));
}

void zchxVectorMapSourceWidget::slotSelDir()
{
    emit signalSelDir();
    emit signalAbouttoClose();
}
