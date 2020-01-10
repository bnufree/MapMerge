#ifndef ZMQRADARVIDEOTHREAD_H
#define ZMQRADARVIDEOTHREAD_H

#include <QThread>
#include "zchxradarutils.h"
#include <QRunnable>
#include <QMap>



namespace ZCHX_RADAR_RECEIVER
{

class zchxRectVideoFunc : public QRunnable
{
public:
    zchxRectVideoFunc(PROTOBUF_RadarRect* rect = 0) : mRect(rect) {}
    void run();
private:
    PROTOBUF_RadarRect* mRect;
};

class ZCHXRadarRectThread : public ZCHXReceiverThread
{
    Q_OBJECT
public:
    ZCHXRadarRectThread(const ZCHX_RadarRect_Param& param, QObject *parent = 0);
    void run();
public slots:
    int  setFilterTime(int secs) {mFilterSecs = secs;}

private:
    void convertZMQ2ZCHX(/*QList<ZCHX::Data::ITF_RadarRect>& res,*/ const PROTOBUF_RadarRectList& src);
signals:
    void sendVideoMsg(int id, const PROTOBUF_RadarRectList&);
private:
    ZCHX_RadarRect_Param     mRectParam;
    //雷达矩形回波块数据缓存.保存最大分钟内的数据
    QMap<int, PROTOBUF_RadarRect>        mDataMap;
    int                      mFilterSecs;       //输出过滤时间范围内的目标

};
}

#endif // ZMQRADARVIDEOTHREAD_H
