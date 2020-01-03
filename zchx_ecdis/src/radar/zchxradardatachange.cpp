#include "ZCHXRadarDataChange.h"
#include "zchxradarrectthread.h"
#include "zchxradarpointthread.h"
#include "zchxradarechothread.h"
#include<QDebug>

using namespace ZCHX_RADAR_RECEIVER;


ZCHXRadarDataChange::ZCHXRadarDataChange(QObject *parent)
    : QObject(parent)
{

}

ZCHXRadarDataChange::~ZCHXRadarDataChange()
{
    stop();
}

void ZCHXRadarDataChange::stop()
{
    foreach (ZCHXReceiverThread* thread, mThreadList) {
        thread->setIsOver(true);
    }
}

void ZCHXRadarDataChange::appendRadarVideo(const ZCHX_Radar_Setting_Param &param)
{
    if(mRadarVideoList.contains(param)) return;
    ZCHXRadarEchoThread* thread = new ZCHXRadarEchoThread(param);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(sendMsg(int,double,double,double,int,int,int,QByteArray,QByteArray))
            , this, SIGNAL(sendRadarVideo(int,double,double,double,int,int,int,QByteArray,QByteArray)));
    mThreadList.append(thread);
    thread->start();
    mRadarVideoList.append(param);
}


void ZCHXRadarDataChange::appendRadarPoint(const ZCHX_Radar_Setting_Param &param)
{
    if(mRadarPointList.contains(param)) return;
    ZCHXRadarPointThread* thread = new ZCHXRadarPointThread(param);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(sendMsg(int,QList<ZCHX::Data::ITF_RadarPoint>)), this, SIGNAL(sendRadarPoint(QList<ZCHX::Data::ITF_RadarPoint>)));
    mThreadList.append(thread);
    thread->start();
    mRadarPointList.append(param);
}

void ZCHXRadarDataChange::appendRadarPointList(const QList<ZCHX_Radar_Setting_Param>& list)
{
    foreach (ZCHX_Radar_Setting_Param param, list) {
        appendRadarPoint(param);
    }
}


void ZCHXRadarDataChange::appendRadarRect(const ZCHX_RadarRect_Param &param)
{
    if(mRadarRectList.contains(param)) return;
    ZCHXRadarRectThread* thread = new ZCHXRadarRectThread(param);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(sendVideoMsg(int, QList<ZCHX::Data::ITF_RadarRect>)), this, SIGNAL(sendRadarRect(int,QList<ZCHX::Data::ITF_RadarRect>)));
    mThreadList.append(thread);
    thread->start();
    mRadarRectList.append(param);
}

void ZCHXRadarDataChange::appendRadarRectList(const QList<ZCHX_RadarRect_Param>& list)
{
    foreach (ZCHX_RadarRect_Param param, list) {
        appendRadarRect(param);
    }
}
