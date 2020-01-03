#include "zmq.h"
#include "zchxradarechothread.h"
#include <QDebug>
#include <QFile>
#include <QSettings>

using namespace ZCHX_RADAR_RECEIVER;

ZCHXRadarEchoThread::ZCHXRadarEchoThread(const ZCHX_Radar_Setting_Param& param, QObject *parent)
    : ZCHXReceiverThread(param, parent)
{
    qRegisterMetaType<ITF_VideoFrame>("ITF_VideoFrame");
    qRegisterMetaType<Map_RadarVideo>("Map_RadarVideo");
    m_lastUpdateRadarEchoTime = 0;
    m_videoFrameMap.clear();
}

void ZCHXRadarEchoThread::run()
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
//    QString sIP = profile->value("Radar_video_1", "IP").toString();
//    QString sPort = profile->value("Radar_video_1", "PORT").toString();
//    QString sTopic = profile->value("Radar_video_1", "TOPIC").toString();
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
    qDebug()<<"---------Echo ip addr------"<<sAddress;
    qDebug()<<"---------Echo topic ------"<<sTopic;
    qDebug()<<" Echo --------nFlag:"<<nFlag;
    if(nFlag < 0)
    {
        zmq_close(pSock);
        zmq_ctx_destroy(pCtx);
        qDebug()<<" echo zmq_connect() fail!";
        return;
    }

    unsigned char *pBufferRec = NULL;
    ITF_VideoFrame objVideoFrame;
    while(!isOver)
    {
        //qDebug()<<"---start echo recv-------------";
        bool bOk = false;
        zmq_msg_t reply;
        zmq_msg_init(&reply);
        zmq_recvmsg(pSock, &reply, 0);//0表示非阻塞
        qint64 size = zmq_msg_size(&reply);
        if(size<15)
            continue;
        //qDebug()<<"echo recv, size :"<<size;
        QByteArray res = QByteArray((char*)zmq_msg_data(&reply),(int)(zmq_msg_size(&reply)));

        pBufferRec = (unsigned char*)zmq_msg_data(&reply);
        //qDebug()<<"echo recv, data :"<<res;

        if(objVideoFrame.ParseFromArray(pBufferRec,size))
        {
           //qDebug()<<"回波数据:";
           dealRadarEchoData(objVideoFrame);

       }
//        else
//        {
//            qDebug()<<"echo recv, data :"<<res;
//        }
       zmq_msg_close(&reply);
    }
    zmq_close(pSock);
    zmq_ctx_destroy(pCtx);
}


void ZCHXRadarEchoThread::dealRadarEchoData(const ITF_VideoFrame &objVideoFrame)
{
    double dCentreLon = objVideoFrame.longitude();
    double dCentreLat = objVideoFrame.latitude();
    int uDisplayType = 1;//1回波-2余辉
    int uLoopNum = objVideoFrame.loopnum();
    double dDistance = objVideoFrame.radius();
    int uCurIndex = objVideoFrame.curindex();
    QByteArray objPixmap = QByteArray(objVideoFrame.imagedata().c_str(), objVideoFrame.imagedata().length());
    QByteArray prePixmap = QByteArray(objVideoFrame.curimagedata().c_str(), objVideoFrame.curimagedata().length());

    emit sendMsg(mRadarCommonSettings.m_sSiteID, dCentreLon, dCentreLat, dDistance, uDisplayType, uLoopNum, uCurIndex, objPixmap, prePixmap);
}
