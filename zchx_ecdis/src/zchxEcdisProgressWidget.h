#ifndef ZCHXECDISPROGRESSWIDGET_H
#define ZCHXECDISPROGRESSWIDGET_H

#include <zchxecdispopupwidget.h>

class QLabel;
class QProgressBar;
class zchxEcdisProgressWidget : public zchxECdisPopupWidget
{
    Q_OBJECT
public:
    explicit zchxEcdisProgressWidget(QWidget *parent = 0);

signals:

public slots:
    void    setTitle(const QString& title);
    void    setRange(int min, int max);
    void    setValue(int value);

private:
    QLabel  *mTitle;
    QProgressBar *mBar;
};

#endif // ZCHXECDISPROGRESSWIDGET_H
