#ifndef ZCHXMAPFRAMEWORK_H
#define ZCHXMAPFRAMEWORK_H

#include <QObject>
#include <QPointF>
#include "zchxMapDataUtils.h"

namespace qt {
class zchxMapFrameWork : public QObject
{
    Q_OBJECT
public:
    explicit zchxMapFrameWork(ZCHX::ZCHX_MAP_TYPE type, QObject* parent = 0);
    virtual ~zchxMapFrameWork();
    ZCHX::ZCHX_MAP_TYPE getType() const {return mType;}

    virtual ZCHX::Data::LatLon      Pixel2LatLon(const ZCHX::Data::Point2D& pos) = 0;
    virtual ZCHX::Data::Point2D     LatLon2Pixel(const ZCHX::Data::LatLon& ll) = 0;
    ZCHX::Data::Point2D     LatLon2Pixel(double lat, double lon);

    void        setOffSet(int offset_x, int offset_y);
    QSize       getOffset() const {return mOffset;}

    int         getZoom() const {return mCurZoom;}
    int         getDrawScale() const {return getZoom();}
    virtual void        setZoom(int zoom) ;
    void        setMinZoom(int zoom) {mMinZoom = zoom;}
    void        setMaxZoom(int zoom) {mMaxZoom = zoom;}
    int         getMinZoom() const {return mMinZoom;}
    int         getMaxZoom() const {return mMaxZoom;}
    void        zoom2Min() {setZoom(mMinZoom);}

    void        setViewSize(int width, int height);
    QSize       getViewSize() const {return QSize(mViewWidth, mViewHeight);}

    void        setCenterAndZoom(const ZCHX::Data::LatLon& ll, int zoom);
    void        setCenter(double lon, double lat);
    void        setCenter(const ZCHX::Data::Point2D& point);
    void        setCenter(const ZCHX::Data::LatLon& ll) {setCenter(ll.lon, ll.lat);}
    ZCHX::Data::LatLon      getCenter() const {return mCenter;}

    //更新地图的显示范围
    virtual     void        updateDisplayRange() {}
    //设定地图数据来源
    virtual     void        setSource(const QString& source, int pos);

    //地图操作接口
    virtual     void        zoomIn();               //放大缩小
    virtual     void        zoomOut();
    virtual     void        pan(int x, int y);     //平移
    double                  getRotateAngle() {return mRotateAngle;}
    virtual     void        setRotateAngle(double ang) {mRotateAngle = ang;}



    //mapStyle
    void        setMapStyle(MapStyle style) {mStyle = style;}
    MapStyle    getMapStyle() const {return mStyle;}

    //
    PPATH convert2QtPonitList(const GPATH &path);
    //地图刷新
    virtual void        update() {}
    virtual void        paintGL() {}

signals:
    void        signalSendCurMapinfo(double center_lat, double center_lon, int zoom);
public slots:


protected:
    ZCHX::ZCHX_MAP_TYPE     mType;
    int                     mCurZoom;
    int                     mMinZoom;
    int                     mMaxZoom;
    int                     mViewWidth;
    int                     mViewHeight;
    ZCHX::Data::LatLon      mCenter;
    QString                 mSourceUrl;    
    int                     mIndexStartPos;
    MapStyle                mStyle;
    double                  mRotateAngle;
    QSize                   mOffset;

};
}

#endif // ZCHXMAPFRAMEWORK_H
