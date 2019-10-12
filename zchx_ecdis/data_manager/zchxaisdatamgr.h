#ifndef ZCHXAISDATAMGR_H
#define ZCHXAISDATAMGR_H

#include "zchxecdisdatamgr.h"
#include <QMutex>
#include <QRunnable>

namespace qt {
class zchxAisDataMgr;
class zchxAisDraw : public QThread
{
    Q_OBJECT
public:
    zchxAisDraw(zchxAisDataMgr* mgr, const QSize& size, const QList<std::shared_ptr<AisElement>>& list);
    void run();

private:
    int                 mWidth;
    int                 mHeight;
    QList<std::shared_ptr<AisElement>>  mList;
    zchxAisDataMgr*  mMgr;
};

class zchxAisDataMgr : public zchxEcdisDataMgr
{
    Q_OBJECT
public:
    explicit    zchxAisDataMgr(zchxMapWidget* w, QObject *parent = 0);
    void        show(QPainter* painter);
    bool        updateActiveItem(const QPoint& pt);
    Element*    selectItem(const QPoint &pt);
    void    removeConcern(const QString& id);    
    void    removeRealtimeTailTrack(const QString& id);
    void    removeHistoryTrack(const QString& id);

    //单船历史轨迹数据更新
    bool setSingleAisData(QString id, const QList<ZCHX::Data::ITF_AIS> &data, bool force);
    void removeAisHistoryData(QString id);
    void clearHistoryTrackSel();

    //实时数据管理
    void SetPickUpAisInfo(QString id);          //设定当前选中的船舶ID
    void setAisData(const QList<ZCHX::Data::ITF_AIS> &data, bool check = true);
    void clearDisplayData() {m_aisMap.clear();}
    void setConsAisData(const ZCHX::Data::ITF_AIS &aisdata);  //施工船显示.施工船强制显示为图片
    //历史回放数据
    void setHistoryAisData(const QList<ZCHX::Data::ITF_AIS> &data);
    void setClearHistoryData(bool states); //true:清空历史回放数据; false:清空实时的历史轨迹数据
    //实时尾迹数据
    bool setRealtimeTailTrack(const QString& id, const QList<ZCHX::Data::ITF_AIS> &data);
    //Ais数据显示配置
    void SetEnableShipTag(int val);
    void setHistoryTrackStyle(const QString &color, const int lineWidth); //历史尾迹样式
    void setPrepushTrackStyle(const QString &color, const int lineWidth); //预推轨迹样式
    //鼠标右键菜单
    QList<QAction*> getRightMenuActions(const QPoint &pt);
    //船舶相机更新
    void updateCamera(const QList<ZCHX::Data::ITF_CameraDev>& cam);
public slots:
    //添加图片信息
    void setPixmap(const QPixmap& map);
    QPixmap  getPixmap();
signals:
    //船队
    void signalFleet(const ZCHX::Data::ITF_AIS& data);    
    //黑白名单
    void signalCreateBlackOrWhiteList(const QString& id, int type);
    //CPA
    void signalCreateCPATrack(const QString& id);

public slots:
    void slotOpenCameraList();
    void setPictureInPicture();
    void setFleet();
    void setSimulationExtrapolation();
    void setHistoryTraces();
    void setRealTimeTraces();
    void setBlackList();
    void setWhiteList();
    void invokeLinkageSpot();
    void sendPTZLocation();
    void setCPATrack();
    void setConcern();



private:
    QMutex              mDataMutex;
    QMutex              mPixMutex;
public:
    QHash<QString, std::shared_ptr<AisElement>>    m_aisData;              //船舶模拟数据
    QHash<QString, std::shared_ptr<AisElement>>    m_aisMap;               //实时AIS对象
    //船舶的轨迹合并到了船舶图元,这里暂且注释掉
    QString             mSelHistoryTrackID;
    int                 mSelHistoryPointIndex;
    //QMap<QString, QList<ZCHX::Data::ITF_AIS>>                   m_aisTraceMap;          //船舶历史轨迹
    //QMap<QString, int>                                          m_aisIndexMap;          //当前船舶历史轨迹线上放大显示的轨迹点下标
    QHash<QString, std::shared_ptr<AisElement>>    m_historyAisMap;        //AIS历史对象,历史回放时使用
    int                                                         mShipTagDisplayMode;    //配置船舶AIStag的显示配置
    QString                                                     m_sHistoryTrackStyle;
    int                                                         m_iHistoryTrackWidth;
    QString                                                     m_sPrepushTrackStyle;
    int                                                         m_iPrepushTrackWidth;
    QList<ZCHX::Data::ITF_AIS>                                  m_pShipSiumtionExtrapolation;
    QPixmap                                                     mCurPixmap;

};
}

#endif // zchxAisDataMgr_H
