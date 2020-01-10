#ifndef DIRECTRADARTHREAD_H
#define DIRECTRADARTHREAD_H

#include <QThread>
#include "zchxradarutils.h"

namespace ZCHX_RADAR_RECEIVER
{
class ZCHXRadarPointThread : public ZCHXReceiverThread
{
    Q_OBJECT
public:
    explicit ZCHXRadarPointThread(const ZCHX_Radar_Setting_Param& param, QObject *parent = 0);
    void run();

signals:
    void sendMsg(int, const PROTOBUF_RadarSurfaceTrack &);
    void radarConnectStatus(bool value);
private:
    void parseRadarList(const ITF_RadarSurfaceTrack &objRadarSurfaceTrack,
                        QList<ZCHX::Data::ITF_RadarPoint>& radarPointList);
};
}

#endif // DIRECTRADARTHREAD_H
