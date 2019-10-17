#ifndef ZCHXTILEMAPFRAMEWORK_H
#define ZCHXTILEMAPFRAMEWORK_H

#include "zchxmapframe.h"

class QPainter;

namespace qt {
class zchxMapLoadThread;


class zchxTileMapFrameWork : public zchxMapFrameWork
{
    Q_OBJECT
public:
    explicit zchxTileMapFrameWork(QObject *parent = 0);
    explicit zchxTileMapFrameWork(double center_lat, double center_lon, int zoom, int width, int height, const QString& source, int pos, int min_zoom, int max_zoom, QObject *parent = 0);
    virtual void updateDisplayRange();
    virtual ZCHX::Data::LatLon      Pixel2LatLon(const ZCHX::Data::Point2D& pos);
    virtual ZCHX::Data::Point2D     LatLon2Pixel(const ZCHX::Data::LatLon& ll);
    void setRotateAngle(double ang);
    virtual void update();
    void    setDisplayImgNumber(bool sts) {mIsDisplayImgNum = sts;}
    void    setPainter(QPainter* painter) {mPainter = painter;}
private:
    ZCHX::Data::Point2D     Mercator2Pixel(const ZCHX::Data::Mercator& mct);
    void        updateEcdis();

signals:

public slots:
    void append(const QPixmap& img, int x, int y);
    void append(const TileImageList& list);
    void clear() {mDataList.clear(); /*update();*/}
    void slotRecvNewMap(double lon, double lat, int zoom, bool sync);
private:
    MapRangeData                mMapRange;      //墨卡托坐标范围  左下最小  右上最大
    zchxMapLoadThread*          mMapThread;
    TileImageList               mDataList;
    double                      mUnitMercatorLength;
    bool                        mIsDisplayImgNum;
    QPainter*                   mPainter;
};
}

#endif // ZCHXTILEMAPFRAMEWORK_H
