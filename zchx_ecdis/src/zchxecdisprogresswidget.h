#ifndef ZCHXECDISPROGRESSWIDGET_H
#define ZCHXECDISPROGRESSWIDGET_H

#include <QWidget>

class zchxEcdisProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit zchxEcdisProgressWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
};

#endif // ZCHXECDISPROGRESSWIDGET_H
