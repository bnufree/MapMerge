#include "zchxvectormapsettingwidget.h"
#include "ui_zchxvectormapsettingwidget.h"
#include <QFileDialog>
#include "profiles.h"
#include "zchxMapDataUtils.h"
#include <QWidget>

zchxDockWidget::zchxDockWidget(const QString &title, QWidget *parent):
    QDockWidget(title, parent)
{
    setAutoFillBackground(true);
//    setAttribute(Qt::WA_DeleteOnClose);
    if(parent)
    {
        QColor color = parent->palette().background().color();
        qDebug()<<"color:"<<color.name();
        this->setStyleSheet(QString("*{color:black; background-color:%1;}\
                                        QPushButton{border:1px solid black;} \
                                        QLineEdit:focus{background-color:white; color:black;}").arg(color.name()));
    }
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(slotDockLocationChanged(Qt::DockWidgetArea)));

}

zchxDockWidget::~zchxDockWidget()
{
    qDebug()<<"dock delete now";
}

void  zchxDockWidget::slotDockLocationChanged(Qt::DockWidgetArea area)
{
    if(!isFloating())
    {
        setAutoFillBackground(true);
    }

}

zchxVectorMapSettingWidget::zchxVectorMapSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zchxVectorMapSettingWidget),
    mMainWindow(qobject_cast<qt::MainWindow*> (parent))
{
    ui->setupUi(this);
    ui->showLightsChk->setVisible(true);
    ui->showNavObjectChk->setVisible(false);
    ui->distance_unit_frame->setVisible(false);
    ui->colorSchemeCBX->addItem(tr("白天"), ZCHX::ZCHX_COLOR_SCHEME_DAY);
    ui->colorSchemeCBX->addItem(tr("傍晚"), ZCHX::ZCHX_COLOR_SCHEME_DUSK);
    ui->colorSchemeCBX->addItem(tr("夜晚"), ZCHX::ZCHX_COLOR_SCHEME_NIGHT);
    ui->displayCategoryCBX->addItem(tr("基本"), ZCHX::ZCHX_DISPLAY_BASE);
    ui->displayCategoryCBX->addItem(tr("标准"), ZCHX::ZCHX_DISPLAY_STANDARD);
    ui->displayCategoryCBX->addItem(tr("全部"), ZCHX::ZCHX_DISPLAY_ALL);
    ui->distanceUnitCbx->addItem(tr("海里"), ZCHX::Nautical_Miles);
    ui->distanceUnitCbx->addItem(tr("英里"), ZCHX::Statute_Miles);
    ui->distanceUnitCbx->addItem(tr("千米"), ZCHX::Kilometers);
    ui->distanceUnitCbx->addItem(tr("米"), ZCHX::Meters);
    //初始化显示
    ui->shallowDepthTxt->setText(QString::number(mMainWindow->itfToolBarGetShallowDepth()));
    ui->safeDepthTxt->setText(QString::number(mMainWindow->itfToolBarGetSafeDepth()));
    ui->DeepDepthTxt->setText(QString::number(mMainWindow->itfToolBarGetDeepDepth()));
    ui->showDepthTextCHK->setChecked(mMainWindow->itfToolBarGetShowDepth());
    ui->showTextCHK->setChecked(mMainWindow->itfToolBarGetShowText());
    ui->colorSchemeCBX->setCurrentIndex(mMainWindow->itfToolBarGetColorScheme());
    qDebug()<<"category:"<<mMainWindow->itfToolBarGetDispkayCategory();
    ui->displayCategoryCBX->setCurrentIndex(mMainWindow->itfToolBarGetDispkayCategory());
    ui->distanceUnitCbx->setCurrentIndex(mMainWindow->itfToolBarGetDistanceUnit());
    ui->vectorDir->setText(mMainWindow->itfToolBarGetMapSource());
    ui->showLightsChk->setChecked(mMainWindow->itfToolBarGetShowLight());

    //信号槽关联
    connect(ui->colorSchemeCBX, SIGNAL(currentIndexChanged(int)), this, SLOT(slotColorSchemeCBXIndexChanged(int)));
    connect(ui->displayCategoryCBX, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDisplayCategoryCBXIndexChanged(int)));
    connect(ui->distanceUnitCbx, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDistanceUnitCBXIndexChanged(int)));
    connect(ui->showDepthTextCHK, SIGNAL(clicked(bool)), this, SLOT(slotShowDepthTextCHKChanged(bool)));
    connect(ui->showTextCHK, SIGNAL(clicked(bool)), this, SLOT(slotShowTextCHKChanged(bool)));
    connect(ui->showLightsChk, SIGNAL(clicked(bool)), this, SLOT(slotShowLightsCHKChanged(bool)));
    connect(ui->showNavObjectChk, SIGNAL(clicked(bool)), this, SLOT(slotShowNavObjectCHKChanged(bool)));
}

void zchxVectorMapSettingWidget::ReInit()
{
    ui->shallowDepthTxt->setText(QString::number(mMainWindow->itfToolBarGetShallowDepth()));
    ui->safeDepthTxt->setText(QString::number(mMainWindow->itfToolBarGetSafeDepth()));
    ui->DeepDepthTxt->setText(QString::number(mMainWindow->itfToolBarGetDeepDepth()));
    ui->showDepthTextCHK->setChecked(mMainWindow->itfToolBarGetShowDepth());
    ui->showTextCHK->setChecked(mMainWindow->itfToolBarGetShowText());
    ui->colorSchemeCBX->setCurrentIndex(mMainWindow->itfToolBarGetColorScheme());
    qDebug()<<"category:"<<mMainWindow->itfToolBarGetDispkayCategory();
    ui->displayCategoryCBX->setCurrentIndex(mMainWindow->itfToolBarGetDispkayCategory());
    ui->distanceUnitCbx->setCurrentIndex(mMainWindow->itfToolBarGetDistanceUnit());
    ui->vectorDir->setText(mMainWindow->itfToolBarGetMapSource());
    ui->showLightsChk->setChecked(mMainWindow->itfToolBarGetShowLight());
}

zchxVectorMapSettingWidget::~zchxVectorMapSettingWidget()
{
    delete ui;
}

void zchxVectorMapSettingWidget::on_mapLoadBtn_clicked()
{
    double lon = ui->centerLon->text().toDouble();
    double lat = ui->centerLat->text().toDouble();
    int zoom = ui->zoomSpinBox->value();
    if(mMainWindow) mMainWindow->setMapCenterAndZoom(lat, lon, zoom);
}

void zchxVectorMapSettingWidget::on_depthSettingApplyBtn_clicked()
{
    if(mMainWindow)
    {
        int shallow = ui->shallowDepthTxt->text().toInt();
        int safe = ui->safeDepthTxt->text().toInt();
        int deep = ui->DeepDepthTxt->text().toInt();
        mMainWindow->itfToolBarSetWaterReferenceDepth(shallow, safe, deep);
    }

}

void zchxVectorMapSettingWidget::on_vectorDirBrowseBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory();
    if(dir.isEmpty()) return;
    ui->vectorDir->setText(dir);
    if(mMainWindow) mMainWindow->itfToolBarSetMapSource(dir);
}

void zchxVectorMapSettingWidget::slotColorSchemeCBXIndexChanged(int index)
{
    ZCHX::ZCHX_COLOR_SCHEME scheme = (ZCHX::ZCHX_COLOR_SCHEME)(ui->colorSchemeCBX->currentData().toInt());
    if(mMainWindow) mMainWindow->itfToolBarSetColorScheme(scheme);
}

void zchxVectorMapSettingWidget::slotDisplayCategoryCBXIndexChanged(int index)
{
    ZCHX::ZCHX_DISPLAY_CATEGORY scheme = (ZCHX::ZCHX_DISPLAY_CATEGORY)(ui->displayCategoryCBX->currentData().toInt());
    if(mMainWindow) mMainWindow->itfToolBarSetDisplayCategory(scheme);
}

void zchxVectorMapSettingWidget::slotDistanceUnitCBXIndexChanged(int index)
{
    if(mMainWindow) mMainWindow->itfToolBarSetDistanceUnit(ZCHX::DistanceUnit(ui->distanceUnitCbx->itemData(index).toInt()));
}

void zchxVectorMapSettingWidget::slotShowDepthTextCHKChanged(bool checked)
{
    if(mMainWindow)mMainWindow->itfToolBarSetShowDepth(checked);
}

void zchxVectorMapSettingWidget::slotShowTextCHKChanged(bool checked)
{
    if(mMainWindow)mMainWindow->itfToolBarSetShowText(checked);
}

void zchxVectorMapSettingWidget::slotShowLightsCHKChanged(bool checked)
{
    if(mMainWindow)mMainWindow->itfToolBarSetShowLight(checked);
}

void zchxVectorMapSettingWidget::slotShowNavObjectCHKChanged(bool checked)
{

}


void zchxVectorMapSettingWidget::on_zoomInBtn_clicked()
{
    if(mMainWindow) mMainWindow->itfToolBarZoomIn();
}

void zchxVectorMapSettingWidget::on_zoomOutBtn_clicked()
{
    if(mMainWindow) mMainWindow->itfToolBarZoomOut();
}

void zchxVectorMapSettingWidget::on_panBtn_clicked()
{
    if(ui->pan_x->value() >0 || ui->pan_y->value() > 0)
    {
        int x = ui->pan_x->value();
        int y = ui->pan_y->value();
        slotPan(x, y);
    }
}

void zchxVectorMapSettingWidget::on_PanLeftBtn_clicked()
{
    slotPan(-10, 0);
}

void zchxVectorMapSettingWidget::on_PanRight_clicked()
{
    slotPan(10, 0);
}

void zchxVectorMapSettingWidget::on_PanUp_clicked()
{
    slotPan(0, 10);
}

void zchxVectorMapSettingWidget::on_PanDown_clicked()
{
    slotPan(0, -10);
}

void zchxVectorMapSettingWidget::on_PanUpLeft_clicked()
{
    slotPan(-10, 10);
}

void zchxVectorMapSettingWidget::on_PanDownLeft_clicked()
{
    slotPan(-10, -10);
}

void zchxVectorMapSettingWidget::on_PanUpRight_clicked()
{
    slotPan(10, 10);
}

void zchxVectorMapSettingWidget::on_PanDownRight_clicked()
{
    slotPan(10, -10);
}

void zchxVectorMapSettingWidget::slotPan(int dx, int dy)
{
    if(mMainWindow) mMainWindow->itfToolBarPan(dx, dy);
}

void zchxVectorMapSettingWidget::on_rotateBtn_clicked()
{
    if(mMainWindow) mMainWindow->itfToolBarRotate(ui->rotate_degree->value());
}

void zchxVectorMapSettingWidget::on_clockwiseRoate_clicked()
{
    if(mMainWindow) mMainWindow->itfToolBarRotateClockwise(5);
}

void zchxVectorMapSettingWidget::on_antiClockwiseRoate_clicked()
{
    if(mMainWindow) mMainWindow->itfToolBarRotateAntiClockwise(5);
}

void zchxVectorMapSettingWidget::on_resetZeroRotate_clicked()
{
    if(mMainWindow) mMainWindow->itfToolBarRotateReset();
}
