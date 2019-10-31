#include "zchxmapframe.h"
#include <QDebug>
#include <QPainter>

using namespace qt;

zchxMapFrameWork::zchxMapFrameWork(ZCHX::ZCHX_MAP_TYPE type, QObject *parent) : QObject(parent)
  , mType(type)
  , mViewWidth(0)
  , mViewHeight(0)
  , mSourceUrl("")
  , mIndexStartPos(0)
  , mStyle(MapStyleEcdisDayBright)
  , mMinZoom(0)
  , mMaxZoom(0)
  , mOffset(0, 0)
{

}



zchxMapFrameWork::~zchxMapFrameWork()
{
     qDebug()<<__FUNCTION__<<__LINE__;
}

void zchxMapFrameWork::setOffSet(int offset_x, int offset_y)
{
    mOffset.setWidth(offset_x);
    mOffset.setHeight(offset_y);
}

void zchxMapFrameWork::setViewSize(int width, int height)
{
    mViewHeight = height;
    mViewWidth = width;
    updateDisplayRange();
}

void zchxMapFrameWork::setSource(const QString& source, int pos)
{
    if(mSourceUrl != source || mIndexStartPos != pos)
    {
        mSourceUrl = source;
        mIndexStartPos = pos;
        updateDisplayRange();
    }
}

void zchxMapFrameWork::setZoom(int zoom)
{
    if(zoom < mMinZoom || zoom > mMaxZoom) return;
    mCurZoom = zoom;    
    updateDisplayRange();
}

void zchxMapFrameWork::setCenter(double lon, double lat)
{
    mCenter.lat = lat;
    mCenter.lon = lon;
    //重新计算当前视窗的显示范围
    updateDisplayRange();
}

void zchxMapFrameWork::setCenterAndZoom(const ZCHX::Data::LatLon &ll, int zoom)
{
    mCenter = ll;
    setZoom(zoom);
}

void zchxMapFrameWork::pan(int x, int y)
{
    setCenter(ZCHX::Data::Point2D(0.5*mViewWidth + x, 0.5*mViewHeight + y));
}

void zchxMapFrameWork::setCenter(const ZCHX::Data::Point2D &point)
{
    setCenter(Pixel2LatLon(point));
}


ZCHX::Data::Point2D zchxMapFrameWork::LatLon2Pixel(double lat, double lon)
{
    return LatLon2Pixel(ZCHX::Data::LatLon(lat, lon));
}


void zchxMapFrameWork::zoomIn()
{
    int zoom = mCurZoom;
    setZoom(++zoom);
}

void zchxMapFrameWork::zoomOut()
{
    int zoom = mCurZoom;
    setZoom(--zoom);
}


PPATH zchxMapFrameWork::convert2QtPonitList(const GPATH& path)
{
    PPATH pts;
    for(int i = 0; i < path.size(); ++i)
    {
        GPNT ll = path[i];
        pts.push_back(LatLon2Pixel(ll.first,ll.second).toPointF());
    }
    return pts;
}




