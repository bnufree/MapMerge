#ifndef RADARELE_H
#define RADARELE_H

#include "IDrawElement.hpp"
#include <QtCore>

namespace qt
{
class  RadarPointElement: public Element
{
public:
    RadarPointElement(const double &lat, const double &lon, zchxMapWidget* frame);
    RadarPointElement(const ZCHX::Data::ITF_RadarPoint &ele, zchxMapWidget* frame);
    RadarPointElement(const RadarPointElement& pt);

    enum RADAR_SHARE{
        Radar_Rect = 0,
        Radar_Ellipse,
    };

    std::pair<double, double> getPoint();

    const ZCHX::Data::ITF_RadarPoint& getData() const;
    void setData(const ZCHX::Data::ITF_RadarPoint& data);

    const std::vector<std::pair<double, double> > &getPath() const;
    void setPath(const std::vector<std::pair<double, double> > &path);

    RADARTYPE getRadarType() const;
    void setRadarType(const RADARTYPE &type);

    bool getShan() const;
    void setShan(bool shan);

    void setDrawAsAis(bool sts) {mDrawAsAis = sts;}
    bool getDrawAsAis() const {return mDrawAsAis;}

    bool  getDrawShapeAsRect() const {return mRadarShapeAsRect;}

    //0不闪，1闪
    uint getStatus() const;
    void setStatus(const uint &value);

    bool getNeedDrawBox() const;
    void setNeedDrawBox(bool needDrawBox);

    void updateGeometry(QPointF, qreal){}
    virtual void initFromSettings();
    //目标描绘
    void drawElement(QPainter *painter);
    //绘制轨迹点
    void drawTrack(QPainter *painter);
    void drawText(QPainter *painter, QPointF pos, int sideLen);
    void clicked(bool isDouble);
    void showToolTip(const QPoint &pos);

    void setShowRadarLabel(bool showRadarLabel);
private:
    int getSideLen();

    std::vector<std::pair<double, double>> m_path;
    RADARTYPE    m_radar_type;
    ZCHX::Data::ITF_RadarPoint m_data;
    bool m_shan;
    bool m_needDrawBox;
    uint m_status; //0不闪，1闪
    bool mRadarShapeAsRect; //true:矩形,false:圆形
    bool mDrawAsAis;  //是否将目标画为一个船舶图标
    bool m_showRadarLabel;
};

class  RadarAreaElement: public Element
{
public:
    explicit RadarAreaElement(double radarY,double radarX,int centerLineAngel,int radius, int maxScanRangeANgle, int numberofChannele, int maxWakePointsNumber, zchxMapWidget* v);
    explicit RadarAreaElement(const ZCHX::Data::ITF_RadarArea &ele, zchxMapWidget* v);
    double radarX() const;
    void setRadarX(double radarX);

    double radarY() const;
    void setRadarY(double radarY);

    int centerLineAngel() const;
    void setCenterLineAngel(int centerLineAngel);

    int radius() const;
    void setRadius(int radius);

    int maxScanRangeANgle() const;
    void setMaxScanRangeANgle(int maxScanRangeANgle);

    int numberofChannele() const;
    void setNumberofChannele(int numberofChannele);

    int maxWakePointsNumber() const;
    void setMaxWakePointsNumber(int maxWakePointsNumber);

    ZCHX::Data::ITF_RadarArea data() const;
    //指定点是否在雷达扫描区域内
    bool contains(zchxMapFrameWork *framework, double angleFromNorth, double x, double y) const;
    QPolygonF getShapePnts(zchxMapFrameWork *framework, double angleFromNorth);

    void updateGeometry(QPointF, qreal){}
private:
    double m_radarX;
    double m_radarY;
    int m_centerLineAngel;
    int m_radius;
    int m_maxScanRangeANgle;
    int m_numberofChannele;
    int m_maxWakePointsNumber;
    ZCHX::Data::ITF_RadarArea m_data;
};
}

#endif // RADARELE_H
