#ifndef ZCHXVECTORMAPFRAMEWORK_H
#define ZCHXVECTORMAPFRAMEWORK_H

#include "zchxmapframe.h"
namespace qt {
class zchxVectorMapFrameWork : public zchxMapFrameWork
{
    Q_OBJECT
public:
    explicit zchxVectorMapFrameWork(QObject *parent = 0);
    virtual ZCHX::Data::LatLon      Pixel2LatLon(const ZCHX::Data::Point2D& pos);
    virtual ZCHX::Data::Point2D     LatLon2Pixel(const ZCHX::Data::LatLon& ll);
    //更新地图的显示范围
    virtual     void        updateDisplayRange();
    //设定地图数据来源
    virtual     void        setSource(const QString& source, int pos);

    //地图操作接口
    virtual     void        zoomIn();               //放大缩小
    virtual     void        zoomOut();
    virtual     void        pan(int x, int y);     //平移
    virtual     void        setRotateAngle(double ang);
    //地图刷新
    virtual void        update();

};
}

#endif // ZCHXVECTORMAPFRAMEWORK_H
