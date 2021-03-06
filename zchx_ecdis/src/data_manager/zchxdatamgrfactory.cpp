#include "zchxdatamgrfactory.h"

namespace qt {
zchxDataMgrFactory* zchxDataMgrFactory::minstance = 0;
zchxDataMgrFactory::MGarbage zchxDataMgrFactory::Garbage;

zchxDataMgrFactory::zchxDataMgrFactory() : mWidget(0)
{

}

zchxDataMgrFactory::~zchxDataMgrFactory()
{

}

//zchxDataMgrFactory* zchxDataMgrFactory::instance()
//{
//    if(minstance == 0)
//    {
//        minstance = new zchxDataMgrFactory;
//    }
//    return minstance;
//}

void zchxDataMgrFactory::setDisplayWidget(zchxMapWidget *w)
{
    mWidget = w;
}

void zchxDataMgrFactory::createManager(int type)
{
    switch (type) {
    case ZCHX::DATA_MGR_AIS_SITE:
        mMgrList[type] = std::shared_ptr<zchxAisSiteDataMgr>(new zchxAisSiteDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_AIDTO_NAVIGATION:
        mMgrList[type] = std::shared_ptr<zchxAidtoNavigationDataMgr>(new zchxAidtoNavigationDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_AIS:
        mMgrList[type] = std::shared_ptr<zchxAisDataMgr>(new zchxAisDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_RADAR_SITE:
        mMgrList[type] = std::shared_ptr<zchxRadarSiteDataMgr>(new zchxRadarSiteDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_RADAR:
        mMgrList[type] = std::shared_ptr<zchxRadarDataMgr>(new zchxRadarDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_CAMERA:
        mMgrList[type] = std::shared_ptr<zchxCameraDataMgr>(new zchxCameraDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_CAMERA_VIEW:
        mMgrList[type] = std::shared_ptr<zchxCameraViewDataMgr>(new zchxCameraViewDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_VIDEO_TARGET:
        mMgrList[type] = std::shared_ptr<zchxVideoTargetDataMgr>(new zchxVideoTargetDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_ROD:
        mMgrList[type] = std::shared_ptr<zchxRodDataMgr>(new zchxRodDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_WARNING_ZONE:
        mMgrList[type] = std::shared_ptr<zchxWarningZoneDataMgr>(new zchxWarningZoneDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_COAST:
        mMgrList[type] = std::shared_ptr<zchxCoastDataMgr>(new zchxCoastDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_SEABEDIPLINE:
        mMgrList[type] = std::shared_ptr<zchxSeabedPipLineDataMgr>(new zchxSeabedPipLineDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_CHANNEL:
        mMgrList[type] = std::shared_ptr<zchxChannelDataMgr>(new zchxChannelDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_STRUCTURE:
        mMgrList[type] = std::shared_ptr<zchxStructureDataMgr>(new zchxStructureDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_AREANET:
        mMgrList[type] = std::shared_ptr<zchxAreaNetDataMgr>(new zchxAreaNetDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_MOOR:
        mMgrList[type] = std::shared_ptr<zchxMooringDataMgr>(new zchxMooringDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_CARDMOUTH:
        mMgrList[type] = std::shared_ptr<zchxCardMouthDataMgr>(new zchxCardMouthDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_STATISTCLINE:
        mMgrList[type] = std::shared_ptr<zchxStatistcLineDataMgr>(new zchxStatistcLineDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_LOCAL_MARK:
        mMgrList[type] = std::shared_ptr<zchxLocalMarkDataMgr>(new zchxLocalMarkDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_DANGEROUS:
        mMgrList[type] = std::shared_ptr<zchxDangerousDataMgr>(new zchxDangerousDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_PASTROLSTATION:
        mMgrList[type] = std::shared_ptr<zchxPastrolStationDataMgr>(new zchxPastrolStationDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_ISLANDLINE:
        mMgrList[type] = std::shared_ptr<zchxIslandLineDataMgr>(new zchxIslandLineDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_NET_GRID:
        mMgrList[type] = std::shared_ptr<zchxNetGridDataMgr>(new zchxNetGridDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_SHIPALARM_ASCEND:
        mMgrList[type] = std::shared_ptr<zchxShipAlarmAscendDataMgr>(new zchxShipAlarmAscendDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_RADAR_RECT:
        mMgrList[type] = std::shared_ptr<zchxRadarRectMgr>(new zchxRadarRectMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_RADAR_VIDEO:
        mMgrList[type] = std::shared_ptr<zchxRadarVideoMgr>(new zchxRadarVideoMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_RADAR_FEATURE_ZONE:
        mMgrList[type] = std::shared_ptr<zchxRadarFeatureZoneDataMgr>(new zchxRadarFeatureZoneDataMgr(mWidget));
        break;
    case ZCHX::DATA_MGR_NAVIMARK:
        mMgrList[type] = std::shared_ptr<zchxNaviMarkDataMgr>(new zchxNaviMarkDataMgr(mWidget));
        break;
    default:
        break;
    }
    return;
}

std::shared_ptr<zchxEcdisDataMgr> zchxDataMgrFactory::getManager(int type)
{
    if(!mMgrList.contains(type)) {
        createManager(type);
    }
    std::shared_ptr<zchxEcdisDataMgr> mgr = mMgrList.value(type, 0);
    Q_ASSERT(mgr);
    return mgr;
}

QList<std::shared_ptr<zchxEcdisDataMgr>> zchxDataMgrFactory::getManagers() const
{
    return mMgrList.values();
}

bool zchxDataMgrFactory::removeDataMgr(std::shared_ptr<zchxEcdisDataMgr> mgr)
{
    if(!mgr) return false;
    return mMgrList.remove(mgr->getType()) != 0;
}

bool zchxDataMgrFactory::appendDataMgr(std::shared_ptr<zchxEcdisDataMgr> mgr)
{
    if(!mgr) return false;
    if(mMgrList.contains(mgr->getType())) return true;
    mMgrList[mgr->getType()] = mgr;
    return true;
}

zchxAisDataMgr* zchxDataMgrFactory::getAisDataMgr()
{
    return static_cast<zchxAisDataMgr*>(getManager(ZCHX::DATA_MGR_AIS).get());
}

zchxAisSiteDataMgr *zchxDataMgrFactory::getAisSiteMgr()
{
    return static_cast<zchxAisSiteDataMgr*>(getManager(ZCHX::DATA_MGR_AIS_SITE).get());
}

zchxAidtoNavigationDataMgr *zchxDataMgrFactory::getAidtoNavigationMgr()
{
    return static_cast<zchxAidtoNavigationDataMgr*>(getManager(ZCHX::DATA_MGR_AIDTO_NAVIGATION).get());
}

zchxRadarDataMgr* zchxDataMgrFactory::getRadarDataMgr()
{
    return static_cast<zchxRadarDataMgr*>(getManager(ZCHX::DATA_MGR_RADAR).get());
}

zchxRadarSiteDataMgr *zchxDataMgrFactory::getRadarSiteMgr()
{
    return static_cast<zchxRadarSiteDataMgr*>(getManager(ZCHX::DATA_MGR_RADAR_SITE).get());
}

zchxCameraDataMgr* zchxDataMgrFactory::getCameraDataMgr()
{
    return static_cast<zchxCameraDataMgr*>(getManager(ZCHX::DATA_MGR_CAMERA).get());
}

zchxRodDataMgr* zchxDataMgrFactory::getRodDataMgr()
{
    return static_cast<zchxRodDataMgr*>(getManager(ZCHX::DATA_MGR_ROD).get());
}

zchxCameraViewDataMgr* zchxDataMgrFactory::getCameraViewMgr()
{
    return static_cast<zchxCameraViewDataMgr*>(getManager(ZCHX::DATA_MGR_CAMERA_VIEW).get());
}

zchxVideoTargetDataMgr* zchxDataMgrFactory::getVideoDataMgr()
{
    return static_cast<zchxVideoTargetDataMgr*>(getManager(ZCHX::DATA_MGR_VIDEO_TARGET).get());
}

zchxPastrolStationDataMgr* zchxDataMgrFactory::getPastrolStationMgr()
{
    return static_cast<zchxPastrolStationDataMgr*>(getManager(ZCHX::DATA_MGR_PASTROLSTATION).get());
}

zchxWarningZoneDataMgr* zchxDataMgrFactory::getWarningZoneMgr()
{
    return static_cast<zchxWarningZoneDataMgr*>(getManager(ZCHX::DATA_MGR_WARNING_ZONE).get());
}

zchxCoastDataMgr*   zchxDataMgrFactory::getCoastMgr()
{
    return static_cast<zchxCoastDataMgr*>(getManager(ZCHX::DATA_MGR_COAST).get());
}

zchxSeabedPipLineDataMgr* zchxDataMgrFactory::getSeabedPiplineMgr()
{
    return static_cast<zchxSeabedPipLineDataMgr*>(getManager(ZCHX::DATA_MGR_SEABEDIPLINE).get());
}

zchxChannelDataMgr* zchxDataMgrFactory::getChannelMgr()
{
    return static_cast<zchxChannelDataMgr*>(getManager(ZCHX::DATA_MGR_CHANNEL).get());
}

zchxStructureDataMgr*   zchxDataMgrFactory::getStructureMgr()
{
    return static_cast<zchxStructureDataMgr*>(getManager(ZCHX::DATA_MGR_STRUCTURE).get());
}

zchxAreaNetDataMgr* zchxDataMgrFactory::getAreanetMgr()
{
    return static_cast<zchxAreaNetDataMgr*>(getManager(ZCHX::DATA_MGR_AREANET).get());
}

zchxMooringDataMgr* zchxDataMgrFactory::getMooringMgr()
{
    return static_cast<zchxMooringDataMgr*>(getManager(ZCHX::DATA_MGR_MOOR).get());
}

zchxCardMouthDataMgr*   zchxDataMgrFactory::getCardmouthMgr()
{
    return static_cast<zchxCardMouthDataMgr*>(getManager(ZCHX::DATA_MGR_CARDMOUTH).get());
}

zchxStatistcLineDataMgr*   zchxDataMgrFactory::getStatistcLineMgr()
{
    return static_cast<zchxStatistcLineDataMgr*>(getManager(ZCHX::DATA_MGR_STATISTCLINE).get());
}

zchxLocalMarkDataMgr*   zchxDataMgrFactory::getLocalmarkMgr()
{
    return static_cast<zchxLocalMarkDataMgr*>(getManager(ZCHX::DATA_MGR_LOCAL_MARK).get());
}

zchxDangerousDataMgr*   zchxDataMgrFactory::getDangerousMgr()
{
    return static_cast<zchxDangerousDataMgr*>(getManager(ZCHX::DATA_MGR_DANGEROUS).get());
}

zchxIslandLineDataMgr*   zchxDataMgrFactory::getIslandlineMgr()
{
    return static_cast<zchxIslandLineDataMgr*>(getManager(ZCHX::DATA_MGR_ISLANDLINE).get());
}

zchxNetGridDataMgr*   zchxDataMgrFactory::getNetGridMgr()
{
    return static_cast<zchxNetGridDataMgr*>(getManager(ZCHX::DATA_MGR_NET_GRID).get());
}

zchxShipAlarmAscendDataMgr*   zchxDataMgrFactory::getShipAlarmAscendMgr()
{
    return static_cast<zchxShipAlarmAscendDataMgr*>(getManager(ZCHX::DATA_MGR_SHIPALARM_ASCEND).get());
}

zchxRadarRectMgr* zchxDataMgrFactory::getRadarRectMgr()
{
    return static_cast<zchxRadarRectMgr*>(getManager(ZCHX::DATA_MGR_RADAR_RECT).get());
}

zchxRadarVideoMgr* zchxDataMgrFactory::getRadarVideoMgr()
{
    return static_cast<zchxRadarVideoMgr*>(getManager(ZCHX::DATA_MGR_RADAR_VIDEO).get());
}

zchxRadarFeatureZoneDataMgr* zchxDataMgrFactory::getRadarFeatureZoneMgr()
{
    return static_cast<zchxRadarFeatureZoneDataMgr*>(getManager(ZCHX::DATA_MGR_RADAR_FEATURE_ZONE).get());
}

zchxNaviMarkDataMgr *zchxDataMgrFactory::getNaviMarkDataMgr()
{
    return static_cast<zchxNaviMarkDataMgr*>(getManager(ZCHX::DATA_MGR_NAVIMARK).get());
}

}


