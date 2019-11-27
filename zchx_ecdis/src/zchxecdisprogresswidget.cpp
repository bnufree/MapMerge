#include "zchxecdisprogresswidget.h"
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>

zchxEcdisProgressWidget::zchxEcdisProgressWidget(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
    this->setLayout(new QVBoxLayout);
    this->layout()->addWidget(new QLabel(tr("正在更新地图数据,请稍候..."), this));
    QProgressBar* bar = new QProgressBar(this);
    bar->setRange(0, 0);
    this->layout()->addWidget(bar);
//    this->setStyleSheet("background-color:transparent;");
}

void zchxEcdisProgressWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
