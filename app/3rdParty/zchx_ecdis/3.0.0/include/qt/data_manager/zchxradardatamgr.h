#ifndef ZCHXRADARDATAMGR_H
#define ZCHXRADARDATAMGR_H

#include "zchxecdisdatamgr.h"
#include <QMutex>

namespace qt {
class zchxRadarDataMgr : public zchxEcdisDataMgr
{
    Q_OBJECT
public:
    explicit zchxRadarDataMgr(zchxMapWidget* w, QObject *parent = 0);
    void    show(QPainter* painter);
    bool    updateActiveItem(const QPoint& pt);
    Element* selectItem(const QPoint &pt);

    void    SetRadarDisplayInfo(int targetSizeIndex, int traceLenIndex, int continueTimeIndex); //设置雷达显示方式
    void    setPickUpRadarPoint(QString id);          //设定当前选中的雷达
    void    setRadarPointData(int radarSiteId, const QList<ZCHX::Data::ITF_RadarPoint> &data);
    void    setHistoryRadarPointData(const std::vector<RadarPointElement> &data);
    void    showRadarPoint(QPainter* painter);

    //雷达扫描区域
    void    setRadarAreaData(const std::vector<RadarAreaElement> &data);

    //鼠标右键菜单
    QList<QAction*> getRightMenuActions(const QPoint &pt);
signals:


public slots:
    void setRealTimeTraces();
    void invokeLinkageSpot();
    void setConcern();
    void setPictureInPicture();

private:    
    bool isRadarDisplayByTargetSize(const ZCHX::Data::ITF_RadarPoint & data,
                                           int targetSizeIndex);

    QMap<int, QMap<QString, std::shared_ptr<RadarPointElement> > >     m_RadarPointMap; // 双雷达
//    QMap<QString, std::shared_ptr<RadarPointElement>>     m_RadarPoint;               //实时雷达目标数据
    std::vector<RadarPointElement>     m_HistoryRadarPoint;        //雷达历史点数据
    QMap<int, double>                               m_pRadarPointHistory;       //雷达的转向数据(历史数据使用)
    QMutex              mDataMutex;

    //雷达扫描区域点数据
    std::vector<RadarAreaElement>      m_RadarArea;            //雷达扫描区域数据
    int m_targetSizeIndex;
    int m_traceLenIndex;
    int m_continueTimeIndex;
};
}

#endif // ZCHXRADARDATAMGR_H
