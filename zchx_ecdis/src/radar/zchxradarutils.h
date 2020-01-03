#ifndef ZCHXRADARUTILS_H
#define ZCHXRADARUTILS_H

#include "../src/zchxutils.hpp"
#include <QThread>

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
