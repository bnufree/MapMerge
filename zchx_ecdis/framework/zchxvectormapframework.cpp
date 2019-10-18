#include "zchxvectormapframework.h"
#include "glChartCanvas.h"


using namespace qt;

zchxVectorMapFrameWork::zchxVectorMapFrameWork(QObject *parent) : zchxMapFrameWork(ZCHX::ZCHX_MAP_VECTOR, parent)
{
    mGLCtrl = new glChartCanvas(0);
}

zchxVectorMapFrameWork::~zchxVectorMapFrameWork()
{
    if(mGLCtrl) delete mGLCtrl;
}

ZCHX::Data::LatLon  zchxVectorMapFrameWork::Pixel2LatLon(const ZCHX::Data::Point2D& pos)
{
    double lat, lon;
    mGLCtrl->getLLOfPix(lat, lon, pos.x, pos.y);
    return ZCHX::Data::LatLon(lat, lon);
}

ZCHX::Data::Point2D  zchxVectorMapFrameWork::LatLon2Pixel(const ZCHX::Data::LatLon& ll)
{
    int x ,y;
    mGLCtrl->getPixcelOfLL(x, y, ll.lat, ll.lon);
    return ZCHX::Data::Point2D(x, y);
}

//更新地图的显示范围
void zchxVectorMapFrameWork::updateDisplayRange()
{

}

//设定地图数据来源
void zchxVectorMapFrameWork::setSource(const QString& source, int pos)
{

}

//地图操作接口
void zchxVectorMapFrameWork::zoomIn()
{
    mGLCtrl->Zoom( 2.0, false );
}

void zchxVectorMapFrameWork::zoomOut()
{
    mGLCtrl->Zoom( 0.25, false );
}

void zchxVectorMapFrameWork::pan(int x, int y)
{
    mGLCtrl->Pan(x, y);
}

void zchxVectorMapFrameWork::setRotateAngle(double ang)
{
    mGLCtrl->RotateDegree(ang);
}

//地图刷新
void zchxVectorMapFrameWork::update()
{

}

void zchxVectorMapFrameWork::paintGL()
{
    mGLCtrl->paintGL();
}

void zchxVectorMapFrameWork::resizeGL(int w, int h)
{
    mGLCtrl->resizeGL(w, h);
}

void zchxVectorMapFrameWork::initializeGL()
{
    mGLCtrl->initializeGL();
}
