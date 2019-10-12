#ifndef RADARRECTELE_H
#define RADARRECTELE_H

#include "IDrawElement.hpp"
#include <QtCore>

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
    void drawRadarRect(const QPointF & leftTopPos, const QPointF & bottomRightPos, QPainter *painter);
    bool isRadarDisplayByTargetSize(const ZCHX::Data::ITF_RadarRect & data,
                                    int targetSizeIndex);
    bool isRadarDisplayByTraceLen(const ZCHX::Data::ITF_RadarRect & data,
                                  int traceLenIndex );
    bool isRadarDisplayByContinueTime(const ZCHX::Data::ITF_RadarRect & data,
                                      int continueTimeIndex);

    ZCHX::Data::ITF_RadarRect mRect;
};

}

#endif // RADARRECTELE_H
