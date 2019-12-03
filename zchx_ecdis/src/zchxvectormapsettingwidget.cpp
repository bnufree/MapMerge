#include "zchxvectormapsettingwidget.h"
#include "ui_zchxvectormapsettingwidget.h"

zchxVectorMapSettingWidget::zchxVectorMapSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::zchxVectorMapSettingWidget),
    mMainWindow(qobject_cast<qt::MainWindow*> (parent))
{
    ui->setupUi(this);
    ui->colorSchemeCBX->addItem(tr("白天"), ZCHX::ZCHX_COLOR_DAY);
    ui->colorSchemeCBX->addItem(tr("傍晚"), ZCHX::ZCHX_COLOR_DUSK);
    ui->colorSchemeCBX->addItem(tr("夜晚"), ZCHX::ZCHX_COLOR_NIGHT);
    ui->displayCategoryCBX->addItem(tr("基本"), ZCHX::ZCHX_DISPLAY_BASE);
    ui->displayCategoryCBX->addItem(tr("标准"), ZCHX::ZCHX_DISPLAY_STANDARD);
    ui->displayCategoryCBX->addItem(tr("全部"), ZCHX::ZCHX_DISPLAY_ALL);
    ui->distanceUnitCbx->addItem(tr("海里"), ZCHX::Nautical_Miles);
    ui->distanceUnitCbx->addItem(tr("英里"), ZCHX::Statute_Miles);
    ui->distanceUnitCbx->addItem(tr("千米"), ZCHX::Kilometers);
    ui->distanceUnitCbx->addItem(tr("米"), ZCHX::Meters);
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

void zchxVectorMapSettingWidget::on_colorSchemeCBX_currentIndexChanged(int index)
{
    ZCHX::ZCHX_COLOR_SCHEME scheme = (ZCHX::ZCHX_COLOR_SCHEME)(ui->colorSchemeCBX->currentData().toInt());
    if(mMainWindow) mMainWindow->itfSetColorScheme(scheme);
}

void zchxVectorMapSettingWidget::on_displayCategoryCBX_currentIndexChanged(int index)
{
    ZCHX::ZCHX_DISPLAY_CATEGORY scheme = (ZCHX::ZCHX_DISPLAY_CATEGORY)(ui->displayCategoryCBX->currentData().toInt());
    if(mMainWindow) mMainWindow->itfSetDisplayCategory(scheme);
}

void zchxVectorMapSettingWidget::on_distanceUnitCbx_currentIndexChanged(int index)
{

}

void zchxVectorMapSettingWidget::on_showDepthTextCHK_clicked(bool checked)
{
}

void zchxVectorMapSettingWidget::on_showTextCHK_clicked(bool checked)
{

}

void zchxVectorMapSettingWidget::on_showLightsChk_clicked(bool checked)
{

}

void zchxVectorMapSettingWidget::on_showNavObjectChk_clicked(bool checked)
{

}
