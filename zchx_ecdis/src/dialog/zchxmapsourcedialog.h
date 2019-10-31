#ifndef ZCHXMAPSOURCEDIALOG_H
#define ZCHXMAPSOURCEDIALOG_H

#include <QDialog>

namespace Ui {
class zchxMapSourceDialog;
}

class zchxMapSourceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit zchxMapSourceDialog(QWidget *parent = 0);
    ~zchxMapSourceDialog();
    QString getUrl() const {return mUrl;}
    int     getPos() const {return mPos;}

private slots:
    void on_browse_clicked();

    void on_pos_currentIndexChanged(int index);

    void on_ok_clicked();

    void on_cancel_clicked();

private:
    Ui::zchxMapSourceDialog *ui;
    QString         mUrl;
    int             mPos;
};

#endif // ZCHXMAPSOURCEDIALOG_H
