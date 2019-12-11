#include "zchxvectormapsettingwidget.h"
#include "ui_zchxvectormapsettingwidget.h"

zchxVectorMapSettingWidget::zchxVectorMapSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zchxVectorMapSettingWidget),
    mMainWindow(qobject_cast<qt::MainWindow*> (parent))
{
    ui->setupUi(this);
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
    ui->shallowDepthTxt->setText(QString::number(mMainWindow->itfGetShallowDepth()));
    ui->safeDepthTxt->setText(QString::number(mMainWindow->itfGetSafeDepth()));
    ui->DeepDepthTxt->setText(QString::number(mMainWindow->itfGetDeepDepth()));
    ui->showDepthTextCHK->setChecked(mMainWindow->itfGetShowDepth());
    ui->showTextCHK->setChecked(mMainWindow->itfGetShowText());
    ui->colorSchemeCBX->setCurrentIndex(mMainWindow->itfGetColorScheme() - 1);
    qDebug()<<"category:"<<mMainWindow->itfGetDispkayCategory();
    ui->displayCategoryCBX->setCurrentIndex(mMainWindow->itfGetDispkayCategory());
    ui->distanceUnitCbx->setCurrentIndex(mMainWindow->itfGetDistanceUnit());
    ui->vectorDir->setText(mMainWindow->itfGetMapSource());

    //信号槽关联
    connect(ui->colorSchemeCBX, SIGNAL(currentIndexChanged(int)), this, SLOT(slotColorSchemeCBXIndexChanged(int)));
    connect(ui->displayCategoryCBX, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDisplayCategoryCBXIndexChanged(int)));
    connect(ui->distanceUnitCbx, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDistanceUnitCBXIndexChanged(int)));
    connect(ui->showDepthTextCHK, SIGNAL(clicked(bool)), this, SLOT(slotShowDepthTextCHKChanged(bool)));
    connect(ui->showTextCHK, SIGNAL(clicked(bool)), this, SLOT(slotShowTextCHKChanged(bool)));
    connect(ui->showLightsChk, SIGNAL(clicked(bool)), this, SLOT(slotShowDepthTextCHKChanged(bool)));
    connect(ui->showNavObjectChk, SIGNAL(clicked(bool)), this, SLOT(slotShowNavObjectCHKChanged(bool)));
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
        mMainWindow->itfSetShallowDepth(ui->shallowDepthTxt->text().toDouble());
        mMainWindow->itfSetSafeDepth(ui->safeDepthTxt->text().toDouble());
        mMainWindow->itfSetDeepDepth(ui->DeepDepthTxt->text().toDouble());
    }

}

void zchxVectorMapSettingWidget::on_vectorDirBrowseBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory();
    if(dir.isEmpty()) return;
    ui->vectorDir->setText(dir);
    if(mMainWindow) mMainWindow->itfSetMapSource(dir);
}

void zchxVectorMapSettingWidget::slotColorSchemeCBXIndexChanged(int index)
{
    ZCHX::ZCHX_COLOR_SCHEME scheme = (ZCHX::ZCHX_COLOR_SCHEME)(ui->colorSchemeCBX->currentData().toInt());
    if(mMainWindow) mMainWindow->itfSetColorScheme(scheme);
}

void zchxVectorMapSettingWidget::slotDisplayCategoryCBXIndexChanged(int index)
{
    ZCHX::ZCHX_DISPLAY_CATEGORY scheme = (ZCHX::ZCHX_DISPLAY_CATEGORY)(ui->displayCategoryCBX->currentData().toInt());
    if(mMainWindow) mMainWindow->itfSetDisplayCategory(scheme);
}

void zchxVectorMapSettingWidget::slotDistanceUnitCBXIndexChanged(int index)
{
    if(mMainWindow) mMainWindow->itfSetDistanceUnit(ZCHX::DistanceUnit(ui->distanceUnitCbx->itemData(index).toInt()));
}

void zchxVectorMapSettingWidget::slotShowDepthTextCHKChanged(bool checked)
{
    if(mMainWindow)mMainWindow->itfSetShowDepth(checked);
}

void zchxVectorMapSettingWidget::slotShowTextCHKChanged(bool checked)
{
    if(mMainWindow)mMainWindow->itfSetShowText(checked);
}

void zchxVectorMapSettingWidget::slotShowLightsCHKChanged(bool checked)
{

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
