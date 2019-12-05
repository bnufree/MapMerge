#ifndef ZCHXECDISPOPUPWIDGET_H
#define ZCHXECDISPOPUPWIDGET_H

#include <QWidget>

class zchxECdisPopupWidget : public QWidget
{
    Q_OBJECT
public:
    explicit zchxECdisPopupWidget(QWidget *parent = 0);
    ~zchxECdisPopupWidget();

protected:
    void paintEvent(QPaintEvent *event);

signals:
    void  signalAbouttoClose();

public slots:
};


class zchxVectorMapSourceWidget : public zchxECdisPopupWidget
{
    Q_OBJECT
public:
    explicit zchxVectorMapSourceWidget(const QString& msg, QWidget *parent = 0);

signals:
    void  signalSelDir();
public slots:
    void  slotSelDir();
};

#endif // ZCHXECDISPOPUPWIDGET_H
