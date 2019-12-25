#ifndef ZCHXVECTORMAPSETTINGWIDGET_H
#define ZCHXVECTORMAPSETTINGWIDGET_H

#include <QWidget>
#include "mainwindow.h"
#include <QDockWidget>

class zchxDockWidget:public QDockWidget
{
    Q_OBJECT

public :
    explicit zchxDockWidget(const QString& title, QWidget* parent = NULL);
    ~zchxDockWidget();
protected:
//    void closeEvent(QCloseEvent* event) {hide();}
public slots:
    void slotDockLocationChanged(Qt::DockWidgetArea area);
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
    void ReInit();

private slots:
    void on_mapLoadBtn_clicked();

    void on_depthSettingApplyBtn_clicked();

    void on_vectorDirBrowseBtn_clicked();

    void on_zoomInBtn_clicked();

    void on_zoomOutBtn_clicked();

    void on_panBtn_clicked();

    void on_PanLeftBtn_clicked();

    void on_PanRight_clicked();

    void on_PanUp_clicked();

    void on_PanDown_clicked();

    void on_PanUpLeft_clicked();

    void on_PanDownLeft_clicked();

    void on_PanUpRight_clicked();

    void on_PanDownRight_clicked();

    void on_rotateBtn_clicked();

    void on_clockwiseRoate_clicked();

    void on_antiClockwiseRoate_clicked();

    void on_resetZeroRotate_clicked();

public slots:
    void slotColorSchemeCBXIndexChanged(int index);
    void slotDisplayCategoryCBXIndexChanged(int index);
    void slotDistanceUnitCBXIndexChanged(int index);
    void slotShowDepthTextCHKChanged(bool checked);
    void slotShowTextCHKChanged(bool checked);
    void slotShowLightsCHKChanged(bool checked);
    void slotShowNavObjectCHKChanged(bool checked);
    void slotPan(int dx, int dy);

private:
    Ui::zchxVectorMapSettingWidget *ui;
    qt::MainWindow*                 mMainWindow;
};

#endif // ZCHXVECTORMAPSETTINGWIDGET_H
