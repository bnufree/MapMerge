#ifndef ZCHXVECTORMAPFRAMEWORK_H
#define ZCHXVECTORMAPFRAMEWORK_H

#include "zchxmapframe.h"

class glChartCanvas;

namespace qt {
class zchxVectorMapFrameWork : public zchxMapFrameWork
{
    Q_OBJECT
public:
    explicit zchxVectorMapFrameWork(QObject *parent = 0);
    virtual ~zchxVectorMapFrameWork();
    virtual ZCHX::Data::LatLon      Pixel2LatLon(const ZCHX::Data::Point2D& pos);
    virtual ZCHX::Data::Point2D     LatLon2Pixel(const ZCHX::Data::LatLon& ll);
    //更新地图的显示范围
    virtual     void        updateDisplayRange();
    virtual     void        setCenter(double lon, double lat);
    //设定地图数据来源
    virtual     void        setSource(const QString& source, int pos);

    //地图操作接口
    virtual     int         getZoom() const;
    virtual     void        zoomIn();               //放大缩小
    virtual     void        zoomOut();
    virtual     void        pan(int x, int y);     //平移
    virtual     void        setRotateAngle(double ang);
    //地图刷新
    virtual void        update();

    //openCPN的接口对应
    void paintGL();
    void resizeGL(int w, int h);
    void initializeGL();

    //开始地图初始化
    virtual void  initEcdis();
    //
    void  changeS572SENC(const QString& src);
    //显示模式
    virtual void        setDisplayCategory(ZCHX::ZCHX_DISPLAY_CATEGORY category);
    virtual int         getDisplayCategory() const;
    //颜色模式
    virtual void        setColorScheme(ZCHX::ZCHX_COLOR_SCHEME cs);
    virtual int         getColorScheme() const;
    //水深单位
    virtual void        setDepthUnit(ZCHX::DepthUnit unit);
    virtual int         getDepthUnit() const;
    //距离单位
    virtual void        setDistanceUnit(ZCHX::DistanceUnit unit);
    virtual int         getDistanceUnit() const;
    //显示水深
    virtual void        setDepthDisplayStatus(bool sts);
    virtual bool        getDepthDisplayStatus() const;
    //显示文本
    virtual void        setTextDisplayStatus(bool sts);
    virtual bool        getTextDisplayStatus() const ;
    //显示Lights
    virtual void        setLightsDisplayStatus(bool sts);
    virtual bool        getLightsDisplayStatus() const ;
    //显示NavObjects
//    virtual void        setNavObjectsDisplayStatus(bool sts);
//    virtual bool        getNavObjectsDisplayStatus() const ;
    //浅水区
    virtual void        setShallowDepthVal(double val);
    virtual double        getShallowDepthVal() const ;
    //安全区
    virtual void        setSafeDepthVal(double val);
    virtual double        getSafeDepthVal() const ;
    //深水区
    virtual void        setDeepDepthVal(double val);
    virtual double        getDeepDepthVal() const;

private:
    glChartCanvas*          mGLCtrl;

};
}

#endif // ZCHXVECTORMAPFRAMEWORK_H
