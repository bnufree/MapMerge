#include <QDebug>
#include <QSettings>
#include "zmq.h"
#include "zchxradarpointthread.h"

using namespace ZCHX_RADAR_RECEIVER;

ZCHXRadarPointThread::ZCHXRadarPointThread(const ZCHX_Radar_Setting_Param& param, QObject *parent)
    : ZCHXReceiverThread(param, parent)
{
    qRegisterMetaType<ITF_TrackPoint>("ITF_TrackPoint");
    qRegisterMetaType<ITF_RadarSurfaceTrack>("ITF_RadarSurfaceTrack");
    qRegisterMetaType<QList<ZCHX::Data::ITF_RadarPoint>>("cosnt QList<ZCHX::Data::ITF_RadarPoint>&");
}

void ZCHXRadarPointThread::run()
{
    void *pCtx = NULL;
    void *pSock = NULL;

    //创建context，zmq的socket 需要在context上进行创建
    if((pCtx = zmq_ctx_new()) == NULL)
    {
        return;
    }
    //创建zmq socket ，socket目前有6中属性 ，这里使用SUB方式
    //具体使用方式请参考zmq官方文档（zmq手册）
    if((pSock = zmq_socket(pCtx, ZMQ_SUB)) == NULL)
    {
        zmq_ctx_destroy(pCtx);
        return;
    }
    //绑定地址
    //也就是使用tcp协议进行通信，使用网络端口
//    QString sIP = profile->value("Radar", "IP").toString();
//    QString sPort = profile->value("Radar", "PORT").toString();
//    QString sTopic = profile->value("Radar", "TOPIC").toString();
    QString sIP    = mRadarCommonSettings.m_sIP;
    QString sPort  = mRadarCommonSettings.m_sPort;
    QString sTopic = mRadarCommonSettings.m_sTopic;

    //设置topic 过滤
    QByteArray bTopic = sTopic.toLatin1();
    const char *filter = bTopic.data();
    if(zmq_setsockopt(pSock, ZMQ_SUBSCRIBE, filter, strlen(filter)))
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        return;
    }

    QString sAddress = "tcp://"+sIP+":"+sPort;
    QByteArray byte1 =sAddress.toLocal8Bit();
    const char *pAddr = byte1.data();// = sAddress.toLocal8Bit().data();

    int nFlag = zmq_connect(pSock, pAddr);
    qDebug()<<"---------ZCHXRadarPointThread ip addr------"<<sAddress;
    qDebug()<<"---------ZCHXRadarPointThread topic ------"<<sTopic;
    qDebug()<<" radar --------nFlag:"<<nFlag;

    if(nFlag < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        qDebug()<<" radar zmq_connect() fail!";
        return;
    }

    QByteArray res;
    bool is_connect = false;
    while(!isOver)
    {
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        nFlag = zmq_recvmsg(pSock, &reply, 0);
        int size = zmq_msg_size(&reply);
        res = QByteArray((char*)zmq_msg_data(&reply),(int)(zmq_msg_size(&reply)));
        ITF_RadarSurfaceTrack objRadarSurfaceTrack;
        QList<ZCHX::Data::ITF_RadarPoint> radarPointList;

        //结果分析
        if(objRadarSurfaceTrack.ParseFromString(res.toStdString()))
        {
            if(objRadarSurfaceTrack.trackpoints_size() > 0)
            {
               if (!is_connect)
               {
                   is_connect = true;
                   emit radarConnectStatus(true);
               }

               parseRadarList(objRadarSurfaceTrack, radarPointList);
               emit sendMsg(mRadarCommonSettings.m_sSiteID, radarPointList);
               objRadarSurfaceTrack.Clear();
            }
        }
        zmq_msg_close(&reply);
    }
    zmq_close(pSock);
    zmq_ctx_destroy(pCtx);
}



void ZCHXRadarPointThread::parseRadarList(const ITF_RadarSurfaceTrack &objRadarSurfaceTrack,
                                       QList<ZCHX::Data::ITF_RadarPoint>& radarPointList)
{
    int size = objRadarSurfaceTrack.trackpoints_size();

    for (int i = 0; i < size; i++)
    {
        const ITF_TrackPoint & point = objRadarSurfaceTrack.trackpoints(i);
        ZCHX::Data::ITF_RadarPoint item;
        item.trackNumber              = point.tracknumber();
        item.timeOfDay                      = point.timeofday();
        item.systemAreaCode           = point.systemareacode();
        item.systemIdentificationCode = point.systemidentificationcode();
        item.cartesianPosX            = point.cartesianposx();
        item.cartesianPosY            = point.cartesianposy();
        item.wgs84PosLat              = point.wgs84poslat();
        item.wgs84PosLon             = point.wgs84poslong();
        item.timeOfDay                = point.timeofday();
        item.trackLastReport          = point.tracklastreport();
        item.sigmaX                   = point.sigmax();
        item.sigmaY                   = point.sigmay();
        item.sigmaXY                  = point.sigmaxy();
        item.ampOfPriPlot             = point.ampofpriplot();
        item.cartesianTrkVel_vx        = point.cartesiantrkvel_vx();
        item.cartesianTrkVel_vy        = point.cartesiantrkvel_vy();
        item.cog                      = point.cog();
        item.sog                      = point.sog();
        item.trackType                = ZCHX::Data::CNF(point.tracktype());
        item.fllow              = point.fleetnumber();
        item.status                   = point.status();
        item.extrapolation            = ZCHX::Data::CST(point.extrapolation());
        item.trackPositionCode        = ZCHX::Data::STH(point.trackpositioncode());
        item.diameter                 = point.diameter();

        radarPointList.append(item);
    }
}
