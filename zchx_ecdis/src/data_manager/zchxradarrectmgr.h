#ifndef ZCHXRADARRECTDMGR_H
#define ZCHXRADARRECTDMGR_H

#include "zchxtemplatedatamgr.h"

namespace qt {
class zchxRadarRectMgr : public zchxTemplateDataMgr<RadarRectGlowElement, ZCHX::Data::ITF_RadarRect>
{
    Q_OBJECT
public:
    explicit zchxRadarRectMgr(zchxMapWidget* w, QObject *parent = 0);
    void    setIsDisplay(bool sts);
    void    SetRadarDisplayInfo(bool showRadarLabel, int targetSizeIndex, int traceLenIndex, int continueTimeIndex); //设置雷达显示方式
    void    setRadarRect(int radarSiteId, QList<ZCHX::Data::ITF_RadarRect> rectList);
    void    show(QPainter *painter);
    void    makePixmap(QPixmap* pic);

signals:

public slots:

private:
//    bool isRadarDisplayByTargetSize(const ZCHX::Data::ITF_RadarRect & data);
//    bool isRadarDisplayByTraceLen(const ZCHX::Data::ITF_RadarRect & data);
//    bool isRadarDisplayByContinueTime(const ZCHX::Data::ITF_RadarRect & data);

    QMap<int, QList<ZCHX::Data::ITF_RadarRect> > mRectMap;
    bool m_showRadarLabel;
    int m_targetSizeIndex;
    int m_traceLenIndex;
    int m_continueTimeIndex;
    QMutex m_mutex;
};
}

#endif // ZCHXRADARRECTDMGR_H
