#include "zchxEcdisProgressWidget.h"
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>

zchxEcdisProgressWidget::zchxEcdisProgressWidget(QWidget *parent) : zchxECdisPopupWidget(parent)
{
    this->setLayout(new QVBoxLayout);
    this->layout()->addWidget(new QLabel(tr("正在更新地图数据,请稍候..."), this));
    QProgressBar* bar = new QProgressBar(this);
    bar->setRange(0, 0);
    this->layout()->addWidget(bar);
//    this->setStyleSheet("background-color:transparent;");
}

