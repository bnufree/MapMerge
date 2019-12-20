#include "zchxEcdisProgressWidget.h"
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>

zchxEcdisProgressWidget::zchxEcdisProgressWidget(QWidget *parent) : zchxECdisPopupWidget(parent)
  , mTitle(0)
  , mBar(0)
{
    this->setLayout(new QVBoxLayout);
    mTitle = new QLabel(tr("正在更新地图数据,请稍候..."), this);
    mBar = new QProgressBar(this);
    mBar->setRange(0, 0);
    this->layout()->addWidget(mTitle);
    this->layout()->addWidget(mBar);
//    this->setStyleSheet("background-color:transparent;");
}

void zchxEcdisProgressWidget::setTitle(const QString &title)
{
    mTitle->setText(title);
}

void zchxEcdisProgressWidget::setRange(int min, int max)
{
    mBar->setRange(min, max);
}

void zchxEcdisProgressWidget::setValue(int value)
{
    mBar->setValue(value);
}

