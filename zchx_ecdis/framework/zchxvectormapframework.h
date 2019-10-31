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
private:
    glChartCanvas*          mGLCtrl;

};
}

#endif // ZCHXVECTORMAPFRAMEWORK_H
