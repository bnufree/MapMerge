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

    virtual int         getZoom() const {return mCurZoom;}
    int         getDrawScale() const {return getZoom();}
    virtual void        setZoom(int zoom) ;
    void        setMinZoom(int zoom) {mMinZoom = zoom;}
    void        setMaxZoom(int zoom) {mMaxZoom = zoom;}
    int         getMinZoom() const {return mMinZoom;}
    int         getMaxZoom() const {return mMaxZoom;}
    void        zoom2Min() {setZoom(mMinZoom);}

    void        setViewSize(int width, int height);
    QSize       getViewSize() const {return QSize(mViewWidth, mViewHeight);}

    virtual void        setCenterAndZoom(const ZCHX::Data::LatLon& ll, int zoom);
    virtual void        setCenter(double lon, double lat);
    void        setCenter(const ZCHX::Data::Point2D& point);
    void        setCenter(const ZCHX::Data::LatLon& ll) {setCenter(ll.lon, ll.lat);}
    ZCHX::Data::LatLon      getCenter() const {return mCenter;}

    //更新地图的显示范围
    virtual     void        updateDisplayRange() {}


    //地图操作接口
    virtual     void        zoomIn();               //放大缩小
    virtual     void        zoomOut();
    virtual     void        pan(int x, int y);     //平移
    virtual     double                  getRotateAngle() {return mRotateAngle;}
    virtual     void        setRotateAngle(double ang) {mRotateAngle = ang;}



    //mapStyle
    void        setMapStyle(MapStyle style) {mStyle = style;}
    MapStyle    getMapStyle() const {return mStyle;}

    //
    PPATH convert2QtPonitList(const GPATH &path);
    //地图刷新
    virtual void        update() {}
    virtual void        paintGL() {}
    virtual void        initEcdis() {}

    //地图参数设定
    //设定地图数据来源
    virtual     void        setSource(const QString& source, int pos);
    virtual     QString     getSource() const {return mSourceUrl;}

    //显示模式
    virtual void        setDisplayCategory(ZCHX::ZCHX_DISPLAY_CATEGORY category) {}
    virtual int         getDisplayCategory() const {return -1;}
    //颜色模式
    virtual void        setColorScheme(ZCHX::ZCHX_COLOR_SCHEME cs) {}
    virtual int         getColorScheme() const {return -1;}
    //水深单位
    virtual void        setDepthUnit(ZCHX::DepthUnit unit) {}
    virtual int         getDepthUnit() const {return -1;}
    //距离单位
    virtual void        setDistanceUnit(ZCHX::DistanceUnit unit) {}
    virtual int         getDistanceUnit() const {return -1;}
    //显示水深
    virtual void        setDepthDisplayStatus(bool sts) {}
    virtual bool        getDepthDisplayStatus() const {return false;}
    //显示文本
    virtual void        setTextDisplayStatus(bool sts) {}
    virtual bool        getTextDisplayStatus() const {return false;}
    //显示Lights
    virtual void        setLightsDisplayStatus(bool sts) {}
    virtual bool        getLightsDisplayStatus() const {return false;}
    //显示NavObjects
    virtual void        setNavObjectsDisplayStatus(bool sts) {}
    virtual bool        getNavObjectsDisplayStatus() const {return false;}
    //浅水区
    virtual void        setShallowDepthVal(double val) {}
    virtual double        getShallowDepthVal() const {return 0;}
    //安全区
    virtual void        setSafeDepthVal(double val) {}
    virtual double        getSafeDepthVal() const {return 0;}
    //深水区
    virtual void        setDeepDepthVal(double val) {}
    virtual double        getDeepDepthVal() const {return 0;}
    //显示网格
    virtual void        setGridDisplayStatus(bool isDisplay) {}
    virtual bool        getGridDisplayStatus() const  {return false;}

signals:
    void        signalSendCurMapinfo(double center_lat, double center_lon, int zoom);
    void        signalDBUpdateFinished();
    void        signalBadChartDirFoundNow();
    void        signalSendProcessBarText(const QString& text);
    void        signalSendProcessRange(int min, int max);
    void        signalSendProcessVal(int val);
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
