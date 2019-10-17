#include "zchxtilemapframework.h"
#include "profiles.h"
#include "zchxmaploadthread.h"
#include <QPainter>

using namespace qt;

zchxTileMapFrameWork::zchxTileMapFrameWork(QObject *parent)
  : zchxMapFrameWork(ZCHX::ZCHX_MAP_TILE, parent)
  , mIsDisplayImgNum(false)
  , mMapThread(0)
  , mPainter(0)
{

}

zchxTileMapFrameWork::zchxTileMapFrameWork(double center_lat, double center_lon, int zoom, int width, int height, const QString& source, int pos,  int min_zoom, int max_zoom, QObject *parent)
    : zchxMapFrameWork(ZCHX::ZCHX_MAP_TILE, parent)
    , mIsDisplayImgNum(false)
    , mMapThread(0)
    , mPainter(0)
{
    setMinZoom(min_zoom);
    setMaxZoom(max_zoom);
    setSource(source, pos);
    setCenter(center_lon, center_lat);
    setZoom(zoom);
    setViewSize(width, height);

    mMapThread = new zchxMapLoadThread;
    connect(mMapThread, SIGNAL(signalSendCurPixmap(QPixmap,int,int)), this, SLOT(append(QPixmap,int,int)));
    connect(mMapThread, SIGNAL(signalSendNewMap(double, double, int, bool)), this, SLOT(slotRecvNewMap(double,double,int, bool)));
    connect(mMapThread, SIGNAL(signalSendImgList(TileImageList)), this, SLOT(append(TileImageList)));
    mMapThread->start();
}

void zchxTileMapFrameWork::updateDisplayRange()
{
    mUnitMercatorLength = zchxMapDataUtils::calResolution(getZoom());
    if(mViewWidth == 0 || mViewHeight == 0) return;
    Profiles::instance()->setValue(MAP_INDEX, MAP_DEFAULT_LAT, mCenter.lat);
    Profiles::instance()->setValue(MAP_INDEX, MAP_DEFAULT_LON, mCenter.lon);
    Profiles::instance()->setValue(MAP_INDEX, MAP_DEFAULT_ZOOM, getZoom());
    //计算当前中心经纬度对应的墨卡托坐标
    ZCHX::Data::Mercator center_mct = zchxMapDataUtils::wgs84LonlatToMercator(mCenter);
    //计算当前视窗对应的墨卡托坐标的显示范围
    mMapRange.mLowerLeft.mX = center_mct.mX - mUnitMercatorLength * mViewWidth / 2.0;
    mMapRange.mLowerLeft.mY = center_mct.mY - mUnitMercatorLength * mViewHeight / 2.0;
    mMapRange.mTopRight.mX = center_mct.mX + mUnitMercatorLength * mViewWidth / 2.0;
    mMapRange.mTopRight.mY = center_mct.mY + mUnitMercatorLength * mViewHeight / 2.0;

    MapLoadSetting setting;
    setting.mMapRange = mMapRange;
    setting.mSourceUrl = mSourceUrl;
    setting.mTilePos = mIndexStartPos;
    setting.mResolution = mUnitMercatorLength;
    setting.mZoom = getZoom();
    setting.mCenter = mCenter;
    if(mMapThread) mMapThread->appendTask(setting);
}

void zchxTileMapFrameWork::setRotateAngle(double ang)
{
    mRotateAngle = 0.0;
}

ZCHX::Data::Point2D zchxTileMapFrameWork::Mercator2Pixel(const ZCHX::Data::Mercator &mct)
{
    double x = mct.mX - mMapRange.mLowerLeft.mX;
    double y = mct.mY - mMapRange.mTopRight.mY;
    ZCHX::Data::Point2D res;
    res.x = x / mUnitMercatorLength + mOffset.width();
    res.y = 0 - y / mUnitMercatorLength + mOffset.height();
    return res;
}


void zchxTileMapFrameWork::slotRecvNewMap(double lon, double lat, int zoom, bool sync)
{
    emit signalSendCurMapinfo(lat, lon, zoom);
    if(!sync) clear();
}

void zchxTileMapFrameWork::append(const QPixmap &img, int x, int y)
{
    mDataList.append(TileImage(img, x, y));
}

void zchxTileMapFrameWork::append(const TileImageList &list)
{
    mDataList = list;
    setOffSet(0, 0);
}

ZCHX::Data::Point2D zchxTileMapFrameWork::LatLon2Pixel(const ZCHX::Data::LatLon &ll)
{
    //获取当前经纬度对应的墨卡托坐标
    ZCHX::Data::Mercator mct = zchxMapDataUtils::wgs84LonlatToMercator(ll);
    //通过墨卡托坐标换算屏幕坐标
    return Mercator2Pixel(mct);
}


ZCHX::Data::LatLon zchxTileMapFrameWork::Pixel2LatLon(const ZCHX::Data::Point2D& pos)
{
    double x = pos.x;
    double y = mViewHeight - pos.y;
    //获取当前指定位置对应的墨卡托坐标
    ZCHX::Data::Mercator target;
    target.mX = mMapRange.mLowerLeft.mX + mUnitMercatorLength * x;
    target.mY = mMapRange.mLowerLeft.mY + mUnitMercatorLength * y;

    return zchxMapDataUtils::mercatorToWgs84LonLat(target);
}

void zchxTileMapFrameWork::update()
{
    updateEcdis();
}

void zchxTileMapFrameWork::updateEcdis()
{
    if(!mPainter) return;
    foreach(TileImage data, mDataList)
    {
        int x = data.mPosX + mOffset.width();
        int y = data.mPosY + mOffset.height();
        mPainter->drawPixmap(x, y, data.mImg);
        if(mIsDisplayImgNum)mPainter->drawText(x, y, data.mName);
    }
}
