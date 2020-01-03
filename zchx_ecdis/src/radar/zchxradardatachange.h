#ifndef ZCHXRADARVIDEODATACHANGE_H
#define ZCHXRADARVIDEODATACHANGE_H
#include <QObject>
#include "zchxradarutils.h"


namespace ZCHX_RADAR_RECEIVER {


class ZCHXRadarDataChange : public QObject
{
    Q_OBJECT
public:
    explicit ZCHXRadarDataChange(QObject *parent = 0);
    ~ZCHXRadarDataChange();
    void stop();
    void appendRadarRect(const ZCHX_RadarRect_Param& param);
    void appendRadarRectList(const QList<ZCHX_RadarRect_Param>& list);
    void appendRadarPoint(const ZCHX_Radar_Setting_Param& param);
    void appendRadarPointList(const QList<ZCHX_Radar_Setting_Param>& list);
    void appendRadarVideo(const ZCHX_Radar_Setting_Param &param);
signals:
    void sendRadarRect(int id, const QList<ZCHX::Data::ITF_RadarRect>&);
    void sendRadarPoint(int id, const QList<ZCHX::Data::ITF_RadarPoint> &);
    void sendRadarVideo(int siteID, double lon, double lat, double dis, int type, int loop, int curIndex, const QByteArray& objPixmap, const QByteArray& prePixMap);


private:
    QList<ZCHXReceiverThread*> mThreadList;
    QList<ZCHX_RadarRect_Param>        mRadarRectList;
    QList<ZCHX_Radar_Setting_Param>        mRadarPointList;
    QList<ZCHX_Radar_Setting_Param>        mRadarVideoList;
};
}

#endif // ZCHXRADARVIDEODATACHANGE_H
