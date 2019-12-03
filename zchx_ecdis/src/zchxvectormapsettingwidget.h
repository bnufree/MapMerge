#ifndef ZCHXVECTORMAPSETTINGWIDGET_H
#define ZCHXVECTORMAPSETTINGWIDGET_H

#include <QWidget>
#include "mainwindow.h"
#include <QDockWidget>

class zchxDockWidget:public QDockWidget
{
public :
    explicit zchxDockWidget(const QString& title, QWidget* parent = NULL) : QDockWidget(title, parent) {}
protected:
    void closeEvent(QCloseEvent* event) {hide();}
};

namespace Ui {
class zchxVectorMapSettingWidget;
}

class zchxVectorMapSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit zchxVectorMapSettingWidget(QWidget *parent = 0);
    ~zchxVectorMapSettingWidget();

private slots:
    void on_mapLoadBtn_clicked();

    void on_depthSettingApplyBtn_clicked();

    void on_vectorDirBrowseBtn_clicked();

    void on_colorSchemeCBX_currentIndexChanged(int index);

    void on_displayCategoryCBX_currentIndexChanged(int index);

    void on_distanceUnitCbx_currentIndexChanged(int index);

    void on_showDepthTextCHK_clicked(bool checked);

    void on_showTextCHK_clicked(bool checked);

    void on_showLightsChk_clicked(bool checked);

    void on_showNavObjectChk_clicked(bool checked);

private:
    Ui::zchxVectorMapSettingWidget *ui;
    qt::MainWindow*                 mMainWindow;
};

#endif // ZCHXVECTORMAPSETTINGWIDGET_H
