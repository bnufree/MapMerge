#include "zchxvectormapframework.h"


using namespace qt;

zchxVectorMapFrameWork::zchxVectorMapFrameWork(QObject *parent) : zchxMapFrameWork(ZCHX::ZCHX_MAP_VECTOR, parent)
{

}

ZCHX::Data::LatLon  zchxVectorMapFrameWork::Pixel2LatLon(const ZCHX::Data::Point2D& pos)
{

}

ZCHX::Data::Point2D  zchxVectorMapFrameWork::LatLon2Pixel(const ZCHX::Data::LatLon& ll)
{

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

}

void zchxVectorMapFrameWork::zoomOut()
{

}

void zchxVectorMapFrameWork::pan(int x, int y)
{

}

void zchxVectorMapFrameWork::setRotateAngle(double ang)
{

}

//地图刷新
void zchxVectorMapFrameWork::update()
{

}
