#ifndef ZCHXRADARUTILS_H
#define ZCHXRADARUTILS_H

#include <QThread>
#include "ZCHXRadarVideo.pb.h"
#include "ZCHXRadar.pb.h"

typedef com::zhichenhaixin::proto::singleVideoBlock  PROTOBUF_SingleVideoBlock;
typedef com::zhichenhaixin::proto::singleVideoBlocks PROTOBUF_SingleVideoBlockList;

typedef com::zhichenhaixin::proto::RadarHistoryRect  PROTOBUF_RadarHistoryRect;
typedef com::zhichenhaixin::proto::RadarHistoryRects PROTOBUF_RadarHistoryRectList;

typedef com::zhichenhaixin::proto::RadarRect  PROTOBUF_RadarRect;
typedef com::zhichenhaixin::proto::RadarRects PROTOBUF_RadarRectList;

typedef com::zhichenhaixin::proto::TrackPoint PROTOBUF_TrackPoint;
typedef com::zhichenhaixin::proto::RadarSurfaceTrack PROTOBUF_RadarSurfaceTrack;

namespace ZCHX_RADAR_RECEIVER
{
    struct ZCHX_Radar_Setting_Param
    {
        QString     m_sIP;
        QString     m_sPort;
        int          m_sSiteID;
        QString     m_sTopic;

        bool operator ==(const ZCHX_Radar_Setting_Param& other)
        {
            return m_sIP == other.m_sIP
                    && m_sPort == other.m_sPort
                    && m_sTopic == other.m_sTopic;
        }
    };

struct ZCHX_RadarRect_Param
{
    ZCHX_Radar_Setting_Param    mSetting;
    QString     m_sCurColor;
    QString     m_sHistoryColor;
    QString     m_sEdgeColor;
    QString     m_sHistoryBackgroundColor;

    bool operator ==(const ZCHX_RadarRect_Param& other)
    {
        return mSetting == other.mSetting;
    }
};

class ZCHXReceiverThread : public QThread
{
    Q_OBJECT
public:
    explicit ZCHXReceiverThread(const ZCHX_Radar_Setting_Param& param, QObject *parent = 0)
             :isOver(false)
             ,mRadarCommonSettings(param)
             ,QThread(parent) {}
    virtual void run() {}
    bool getIsOver() const {return isOver;}
    void setIsOver(bool value) {isOver = value;}

protected:
    ZCHX_Radar_Setting_Param mRadarCommonSettings;
    bool  isOver;
};
}





#endif // ZCHXRADARUTILS_H
