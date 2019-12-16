#ifndef RADARRECTELE_H
#define RADARRECTELE_H

#include "IDrawElement.hpp"
#include <QtCore>

struct      zchxPolygon
{
    QPolygon polygon;
    QColor   color;
    zchxPolygon (const QPolygon& poly, const QColor& rcolor) {polygon = poly; color = rcolor;}
};

typedef     QList<zchxPolygon>  QPolygonList;

namespace qt {
class RadarRectGlowElement : public Element
{
public:
    explicit RadarRectGlowElement(const ZCHX::Data::ITF_RadarRect& data, zchxMapWidget* frame);

    const ZCHX::Data::ITF_RadarRect &data() const;
    void setData(const ZCHX::Data::ITF_RadarRect& rect);
    void drawElement(QPainter *painter);
    void drawOutline(QPainter *painter, const QPointF& center, double in, double out);
    std::string name () const {return QString::number(mRect.rectNumber).toLatin1().data();}

private:
    int  getRectHight(double refer_dis);
    void calculateRect(double& angle, double& length, const ZCHX::Data::ITF_RadarHistoryRect &rect);
    void drawRadarTracks(QPainter* painter);
    void calculateSkipRect(ZCHX::Data::ITF_RadarHistoryRectList& list, const ZCHX::Data::ITF_RadarHistoryRect& pre, const ZCHX::Data::ITF_RadarHistoryRect& cur);
    void drawRadarRect(QPainter* painter,
                       const QColor& targetColor,
                       const QColor& edgeColor,
                       const QColor& backgroundColor,
                       const ZCHX::Data::ITF_RadarHistoryRect &rect,
                       bool  round = false,
                       QPolygon* side = 0);
    void drawRadarRect(const QPointF & leftTopPos, const QPointF & bottomRightPos, QPainter *painter);
    bool isRadarDisplayByTargetSize(const ZCHX::Data::ITF_RadarRect & data,
                                    int targetSizeIndex);
    bool isRadarDisplayByTraceLen(const ZCHX::Data::ITF_RadarRect & data,
                                  int traceLenIndex );
    bool isRadarDisplayByContinueTime(const ZCHX::Data::ITF_RadarRect & data,
                                      int continueTimeIndex);

    ZCHX::Data::ITF_RadarRect mRect;
    static  int             mMaxRectLength;
    static  int             mMaxGapOf2Rect;
    QList<ZCHX::Data::LatLon>           mInsertPointList;
    QList<ZCHX::Data::LatLon>           mOrigonPointList;
    double                  mHistoryPixelLength;
    double                  mMaxRectLenthAtZoom;            //目标在当前等级下的最大长度
    QMutex m_mutex;
};

}

#endif // RADARRECTELE_H
