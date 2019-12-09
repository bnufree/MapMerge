#include "zchxmapwidget.h"
#include "zchxtilemapframework.h"
#include "zchxvectormapframework.h"
#include "map_layer/zchxmaplayermgr.h"
#include "data_manager/zchxdatamgrfactory.h"
#include "draw_manager/zchxdrawtoolutil.h"
#include "zchxroutedatamgr.h"
#include "zchxshipplandatamgr.h"
#include <QPainter>
#include <QDebug>
#include <QDomDocument>
#include "profiles.h"
#include "dialog/zchxmapsourcedialog.h"
#include "zchxEcdisProgressWidget.h"
#include "zchxecdispopupwidget.h"

//#define     DEFAULT_LON         113.093664
//#define     DEFAULT_LAT         22.216150
//#define     DEFAULT_ZOOM        13
using namespace qt;
zchxMapWidget::zchxMapWidget(ZCHX::ZCHX_MAP_TYPE type, QWidget *parent) : QGLWidget(parent),
    m_eTool(DRAWNULL),
    mLastWheelTime(0),
    mCurrentSelectElement(0),
    mUseRightKey(true),
    mFrameWork(0),
    mDrag(0),
    mDisplayImageNum(false),
    mIsMapHidden(false),
    mIsNavigation(false),
    mIsOpenMeet(false),
    mIsRadarTagetTrack(false),
    mIsCameraCombinedTrack(false),
    mIsCameraDisplayWithoutRod(true),
    mCurPluginUserModel(ZCHX::Data::ECDIS_PLUGIN_USE_DISPLAY_MODEL),    
    mRouteDataMgr(new zchxRouteDataMgr(this)),
    mShipPlanDataMgr(new zchxShipPlanDataMgr(this)),
    mToolPtr(0),
    m_targetSizeIndex(0),
    m_traceLenIndex(0),
    m_continueTimeIndex(0),
    mType(type),
    mIsDBUpdateNow(false),
    mDBProgressWidget(0)
{
    this->setMouseTracking(true);
    mZoomLbl = new QLabel(this);
    mZoomLbl->setAutoFillBackground(true);
    mZoomLbl->setStyleSheet("border:1px; color:white; font-size:20pt;");

    bool zoom_visible = false;
#ifdef MyTest
    zoom_visible = true;
#endif
    mZoomLbl->setVisible(zoom_visible);
    QTimer *timer = new QTimer;
    timer->setInterval(Profiles::instance()->value(MAP_INDEX, MAP_UPDATE_INTERVAL).toInt());
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start();    
    //创建地图框架
    double lat = Profiles::instance()->value(MAP_INDEX, MAP_DEFAULT_LAT).toDouble();
    double lon = Profiles::instance()->value(MAP_INDEX, MAP_DEFAULT_LON).toDouble();
    QString     source = Profiles::instance()->value(MAP_INDEX, MAP_URL).toString();
    int     pos = Profiles::instance()->value(MAP_INDEX, MAP_INDEX_START_POS).toInt();
    int zoom = Profiles::instance()->value(MAP_INDEX, MAP_DEFAULT_ZOOM).toInt();
    int min_zoom = Profiles::instance()->value(MAP_INDEX, MAP_MIN_ZOOM).toInt();
    int max_zoom = Profiles::instance()->value(MAP_INDEX, MAP_MAX_ZOOM).toInt();
    if(type == ZCHX::ZCHX_MAP_TILE)
    {
        mFrameWork = new zchxTileMapFrameWork(lat, lon, zoom, width(), height(), source, pos, min_zoom, max_zoom);
    } else
    {
//        mFrameWork = new zchxTileMapFrameWork(lat, lon, zoom, width(), height(), source, pos, min_zoom, max_zoom);
        mFrameWork = new zchxVectorMapFrameWork();
    }
    connect(mFrameWork, SIGNAL(signalDBUpdateFinished()), this, SLOT(slotDBUpdateFinished()));
    connect(mFrameWork, SIGNAL(signalBadChartDirFoundNow()), this, SLOT(slotBadChartDirFoundNow()));
    //地图状态初始化
    releaseDrawStatus();
    //
    //创建数据管理容器
    ZCHX_DATA_FACTORY->setDisplayWidget(this);
    for(int i= ZCHX::DATA_MGR_UNKNOWN + 1; i< ZCHX::DATA_MGR_USER_DEFINE; i = i<<1)
    {
        ZCHX_DATA_FACTORY->createManager(i);
    }
    //数据选择默认为不能选择
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_NONE);

}

zchxMapWidget::~zchxMapWidget()
{
    if(!mFrameWork) delete mFrameWork;
}

void zchxMapWidget::setUseRightKey(bool bUseRightKey)
{
    mUseRightKey = bUseRightKey;
}

void zchxMapWidget::setZoomLabelVisible(bool sts)
{
    mZoomLbl->setVisible(sts);
}

void zchxMapWidget::setImgNumberVisible(bool sts)
{
    mDisplayImageNum = sts;
}



void zchxMapWidget::initializeGL()
{
    glClearColor(0.8, 0.78, 0.5, 1.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH);


    if(mFrameWork) mFrameWork->initEcdis();

}

#if 0
void zchxMapWidget::paintGL()
{
    qDebug()<<"!!!!!!!!!!!";
    QPainter painter(this);
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glPopMatrix();
    painter.endNativePainting();
    if(!mFrameWork) return;
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
    //显示地图
    mFrameWork->updateEcdis(&painter, mDisplayImageNum);
    //显示图元
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        mgr->show(&painter);
    }
    MapLayerMgr::instance()->show(&painter);
    //显示用户的鼠标操作
    if(mToolPtr) mToolPtr->show(&painter);

//    //显示当前的中心点
//    Point2D pnt = mFrameWork->LatLon2Pixel(mCenter);
//    //qDebug()<<pnt.x<<pnt.y;
//    painter.setBrush(QBrush(Qt::red));
//    painter.drawEllipse(pnt.x, pnt.y, 5, 5);
    mZoomLbl->setText(QString("zoom:%1").arg(mFrameWork->Zoom()));
    updateCurrentPos(this->mapFromGlobal(QCursor::pos()));
}
#endif

void zchxMapWidget::resizeGL(int w, int h)
{
    int side = qMin(w, h);
    glViewport((width() - side) / 2, (height() - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1.2, 1.2, -1.2, 1.2, 5.0, 60.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -40.0);
    if(mFrameWork)
    {
        //重新更新地图显示的大小
        mFrameWork->setViewSize(w, h);
        if(mFrameWork->getType() == ZCHX::ZCHX_MAP_VECTOR)
        {
            zchxVectorMapFrameWork* frame = qobject_cast<zchxVectorMapFrameWork*>(mFrameWork);
            if(frame)
            {
                frame->resizeGL(w, h);
            }
        }
    }
    mZoomLbl->setGeometry(10, 10, 100, 60);

    foreach (QWidget* w, mPopWidgetList) {
        QRect rect = w->rect();
        rect.moveCenter(this->rect().center());
        w->move(rect.topLeft());
    }
}

#if 1
void zchxMapWidget::paintEvent(QPaintEvent* e)
{
    if(!mFrameWork) return;
    QPainter painter(this);
    QString color_name = Profiles::instance()->value(MAP_INDEX, MAP_BACK_GROUND).toString();
//    painter.fillRect(this->rect(), QColor(color_name));
    painter.beginNativePainting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    mFrameWork
    if(mFrameWork->getType() == ZCHX::ZCHX_MAP_VECTOR)
    {
        zchxVectorMapFrameWork* frame = qobject_cast<zchxVectorMapFrameWork*>(mFrameWork);
        if(frame)
        {
            frame->paintGL();
        }
    }

    painter.endNativePainting();
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform, true);
    //显示地图
    if(mFrameWork->getType() == ZCHX::ZCHX_MAP_TILE)
    {
        zchxTileMapFrameWork* frame = qobject_cast<zchxTileMapFrameWork*>(mFrameWork);
        if(frame)
        {
            frame->setPainter(&painter);
            frame->setDisplayImgNumber(mDisplayImageNum);
        }
    }
    mFrameWork->update();
    //显示图元
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        mgr->show(&painter);
    }
    MapLayerMgr::instance()->show(&painter);
    //显示用户的鼠标操作
    if(mToolPtr) mToolPtr->show(&painter);

//    //显示当前的中心点
//    Point2D pnt = mFrameWork->LatLon2Pixel(mCenter);
//    //qDebug()<<pnt.x<<pnt.y;
//    painter.setBrush(QBrush(Qt::red));
//    painter.drawEllipse(pnt.x, pnt.y, 5, 5);
    if(mZoomLbl->isVisible())
    {
        mZoomLbl->setText(tr("zoom:%1").arg(mFrameWork->getZoom()));
        int width_t = QFontMetrics(mZoomLbl->font()).width(mZoomLbl->text());
        mZoomLbl->setMinimumWidth(width_t);
    }
    updateCurrentPos(this->mapFromGlobal(QCursor::pos()));
}

#endif

#if 0

void zchxMapWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    QSize size = e->size();
    if(size.width() > 0 && size.height() > 0)
    {
        if(mFrameWork)
        {
            //重新更新地图显示的大小
            mFrameWork->SetViewSize(size.width(), size.height());
        }
    }

    mZoomLbl->setGeometry(10, 10, 100, 60);
}

# endif

bool zchxMapWidget::IsLeftButton(Qt::MouseButtons buttons)
{
    return buttons & Qt::LeftButton;
}

bool zchxMapWidget::IsLeftButton(QMouseEvent * e)
{
    return IsLeftButton(e->button()) || IsLeftButton(e->buttons());
}

bool zchxMapWidget::IsRightButton(Qt::MouseButtons buttons)
{
    return buttons & Qt::RightButton;
}

bool zchxMapWidget::IsRightButton(QMouseEvent * e)
{
    return IsRightButton(e->button()) || IsRightButton(e->buttons());
}

bool zchxMapWidget::IsRotation(QMouseEvent * e)
{
    return e->modifiers() & Qt::ControlModifier;
}

bool zchxMapWidget::IsRouting(QMouseEvent * e)
{
    return e->modifiers() & Qt::ShiftModifier;
}

bool zchxMapWidget::IsLocationEmulation(QMouseEvent * e)
{
    return e->modifiers() & Qt::AltModifier;
}

void zchxMapWidget::setActiveDrawElement(const ZCHX::Data::Point2D &pos, bool dbClick)
{
    //检查当前的地图模式,如果是纯显示模式,重置当前的目标图元选择;编辑模式则保持不变
    //if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL == mCurPluginUserModel) return;
    //重置选择的目标为空
    setCurrentSelectedItem(0);

    //检查各个数据管理类,获取当前选择的目标
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        if(mgr->updateActiveItem(pos.toPoint())){
            Element* ele = getCurrentSelectedElement();
            if(ele)
            {
                ele->clicked(dbClick);
                if(dbClick){
                    setETool2DrawPickup();
                }
            }
            break;
        }
    }
}
//目标跟踪的情况,横琴使用.雷达,AIS,目标跟踪;空白点:热点视频显示
void zchxMapWidget::setSelectedCameraTrackTarget(const ZCHX::Data::Point2D &pos)
{
    if(ZCHX::Data::ECDIS_PICKUP_NONE == mCurPickupType) return;
    //检查当前选中的东西
    Element *ele = 0;
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        ele = mgr->selectItem(pos.toPoint());
        if(ele) break;
    }
    ZCHX::Data::ITF_CameraTrackTarget target;
    double lat,lon;
    QPointF e_pos = pos.toPointF();
    zchxUtilToolLL4CurPoint(e_pos,lat,lon);
    target.id = "";
    target.type = 3;
    target.lat = lat;
    target.lon = lon;
    if(ele){
        if(ele->getElementType() == ZCHX::Data::ELE_RADAR_POINT)
        {
            //选中雷达点(radar)轨迹元素
            RadarPointElement *item = static_cast<RadarPointElement*>(ele);
            if(!item) return;
            ZCHX::Data::ITF_RadarPoint radar = item->getData();
            target.id = QString::number(radar.trackNumber);
            target.lon = radar.getLon();
            target.lat = radar.getLat();
            target.type = 2;
        } else if(ele->getElementType() == ZCHX::Data::ELE_AIS)
        {
            //选中AIS
            AisElement *item = static_cast<AisElement*>(ele);
            if(!item) return;
            ZCHX::Data::ITF_AIS ais = item->data();
            target.id = ais.id;
            target.lon = ais.lon;
            target.lat = ais.lat;
            target.type = 1;
        }
    }

    emit signalIsEcdisCameraTrackTarget(target);
}

void zchxMapWidget::setPickUpNavigationTarget(const ZCHX::Data::Point2D &pos)
{
    if(ZCHX::Data::ECDIS_PICKUP_AIS !=  mCurPickupType) return;
    Element *ele = 0;
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        if(mgr->getType() != ZCHX::DATA_MGR_AIS) continue;
        ele = mgr->selectItem(pos.toPoint());
        if(ele) break;
    }
    if(!ele) return;
    AisElement *item = static_cast<AisElement*>(ele);
    ZCHX_DATA_FACTORY->getAisDataMgr()->setFocusID(item->data().id);
    emit signalIsSelected4TrackRadarOrbit(item->data(), true);
}

void zchxMapWidget::getPointNealyCamera(const ZCHX::Data::Point2D &pos)
{
    if(ZCHX::Data::ECDIS_PICKUP_RADARPOINT !=  mCurPickupType) return;
    Element *ele = 0;
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        if(mgr->getType() != ZCHX::DATA_MGR_RADAR) continue;
        ele = mgr->selectItem(pos.toPoint());
        if(ele) break;
    }

    ZCHX::Data::LatLon ll = zchxUtilToolLL4CurPoint(pos.toPointF());
    int trackNum = 0;
    if(ele)
    {
        RadarPointElement *item = static_cast<RadarPointElement*>(ele);
        if(!item) return;
        trackNum = item->getData().trackNumber;
        ll.lat = item->getData().getLat();
        ll.lon = item->getData().getLon();
    }

    emit signalSendPointNealyCamera(trackNum, ll.lat,ll.lon);
}



void zchxMapWidget::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    if(IsLeftButton(e))
    {
        //先更新当前鼠标点击的位置信息,给地图移动做准备
        updateCurrentPos(e->pos());
        mPressPnt = e->pos();
        qDebug()<<"left mouse press event:"<<e->pos();
        //检查不同的情况进行处理,地图的其他操作优先处理
        if(isActiveETool) {
            //获取当前点的经纬度
            ZCHX::Data::LatLon ll =zchxUtilToolLL4CurPoint(e->pos());
            switch (m_eTool) {
            case DRAWMEASUREAREA:
            case DRAWDIRANGLE:
            case DRAWDISTANCE:
            case ZONEDRAWRADAR:
            case ZONEDRAW:
            case CHANNELMANAGER:
            case MOORINGMANAGER:
            case CARDMOUTHMANAGER:
            case STATISTCLINEMANAGER:
            {
                if(mToolPtr) mToolPtr->appendPoint(e->pos());
                break;
            }
            case DRAWPICKUP:
            case COMMONZONESELECT:
            case ZONESELECT:
            case CHANNELSELECT:
            case MOORINGSELECT:
            case CARDMOUTHSELECT:
            case STATISTCLINESELECT:
            {
                setActiveDrawElement(e->pos(), false);
                if(m_eTool != DRAWPICKUP)
                {
                    if(mToolPtr)
                    {
                        mToolPtr->setElement((MoveElement*)getCurrentSelectedElement());
                        if(mToolPtr->element()) mToolPtr->element()->setActivePathPoint(-1);
                    }
                }
                break;
            }
            case CARDMOUTHMOVECTRL:
            case STATISTCLINEMOVECTRL:
            case CHANNELMOVECTRL:
            case ZONEMOVECTRL:
            case MOORINGMOVECTRL:
            {
                if(mToolPtr)
                {
                    mToolPtr->selectCtrlPoint(e->pos());
                }
                break;
            }
            case ZONEMOVE:
            case CHANNELMOVE:
            case MOORINGMOVE:
            case CARDMOUTHMOVE:
            case STATISTCLINEMOVE:
            {
                if(mToolPtr && mToolPtr->element()) mToolPtr->setStartMove(true);
                break;
            }
            case ZONEADDCTRL:
            case CHANNELADDCTRL:
            case MOORINGADDCTRL:
            case CARDMOUTHADDCTRL:
            case STATISTCLINEADDCTRL:
            {
                if(mToolPtr && mToolPtr->element()) mToolPtr->addCtrlPoint(e->pos());
                break;
            }
            case ZONEDELCTRL:
            case CHANNELDELCTRL:
            case MOORINGDELCTRL:
            case CARDMOUTHDELCTRL:
            case STATISTCLINEDELCTRL:
            {
                if(mToolPtr && mToolPtr->element()) mToolPtr->delCtrlPoint(e->pos());
                break;
            }
            case CAMERATEACK:
            {
                setSelectedCameraTrackTarget(e->pos());
                break;
            }
            case TRACKTARGET:
            {
                setPickUpNavigationTarget(e->pos());
                break;
            }

            case DRAWGPS:
            {
                getPointNealyCamera(e->pos());
                break;
            }
            case DRAWNULL:
            {
                releaseDrawStatus();
                break;
            }
            case DRAWLOCALMARK:
            {
                if(mToolPtr){
                    mToolPtr->appendPoint(e->pos());
                    mToolPtr->endDraw();
                }
                break;
            }
            case PICKUPPTZ:
            {
                emit signalSendPTZLocation(ll.lat, ll.lon);
                break;
            }
            case DRAWCAMERANETGRID:
            {
                if(mToolPtr)
                {
                    mToolPtr->appendPoint(e->pos());
                    if(mToolPtr->getPointSize() == 2)
                    {
                        mToolPtr->endDraw();
                        releaseDrawStatus();
                    }
                }
                break;
            }

#if 0


            case ROUTELINEDRAW:
            {
                m_eToolPoints.push_back(ll);
                //将路由数据点转换成点列保存，在绘制路由菜单结束后，清除
                QStringList pntslist;
                foreach (ms::LatLon pnt, m_eToolPoints) {
                    pntslist.append(QString::number(pnt.lon, 'f', 6));
                    pntslist.append(QString::number(pnt.lat, 'f', 6));
                }
                std::string res = pntslist.join(",").toStdString();
                Settings::Set("RoutePath", res);
                break;
            }
            case SHIPPLANDRAW:
            {
                m_eToolPoints.push_back(ll);
                //将路由数据点转换成点列保存，在绘制路由菜单结束后，清除
                QStringList pntslist;
                foreach (ms::LatLon pnt, m_eToolPoints) {
                    pntslist.append(QString::number(pnt.lon, 'f', 6));
                    pntslist.append(QString::number(pnt.lat, 'f', 6));
                }
                std::string res = pntslist.join(",").toStdString();
                Settings::Set("PlanPath", res);
                break;
            }
            case SHIPPLANSELECT:
            {
                shipPlanEditSelect(pt);
                //发送新的船舶计划数据,以便更新
                break;
            }
            case SHIPPLANMOVECTRL:
            {
                m_bShipPlanLineMove = shipPlanEditSelectCtrlPoint(pt);
                break;
            }
            case SHIPPLANEDELCTRL:
            {
                shipPlanLineEditDelCtrlPoint(pt);
                break;
            }
            case SHIPPLANADDCTRL:
            {
                m_eToolPoints.push_back(ll);
                //将路由数据点转换成点列保存，在绘制路由菜单结束后，清除
                QStringList pntslist;
                foreach (ms::LatLon pnt, m_eToolPoints) {
                    pntslist.append(QString::number(pnt.lon, 'f', 6));
                    pntslist.append(QString::number(pnt.lat, 'f', 6));
                }
                std::string res = pntslist.join(",").toStdString();
                Settings::Set("PlanAddPnt", res);

                shipPlanLineEditAddCtrlPoint(e->pos());
                break;
            }
            case SHIPPLANINSERTCTRL:
            {
                shipPlanLineEditInsertCtrlPoint(e->pos());
                break;
            }
                //------------------
            case ROUTELINEELECT:
            {
                routeLineEditSelect(pt);
                //发送新的数据,以便更新
                break;
            }
            case ROUTEINSERTCTRL:
            {
                routeLineEditInsertCtrlPoint(e->pos());
                break;
            }
            case ROUTEMOVECTRL:
            {
                m_bRouteLineMove = routeLineEditSelectCtrlPoint(pt);
                //m_bRouteLineMove = true;
                break;
            }
            case ROUTEADDCTRL:
            {
                m_eToolPoints.push_back(ll);
                //将路由数据点转换成点列保存，在绘制路由菜单结束后，清除
                QStringList pntslist;
                foreach (ms::LatLon pnt, m_eToolPoints) {
                    pntslist.append(QString::number(pnt.lon, 'f', 6));
                    pntslist.append(QString::number(pnt.lat, 'f', 6));
                }
                std::string res = pntslist.join(",").toStdString();
                Settings::Set("RouteAddPnt", res);
                routeLineEditAddCtrlPoint(e->pos());
                break;
            }
            case ROUTEDELCTRL:
            {
                routeLineEditDelCtrlPoint(pt);
                break;
            }
            case COASTDATALINEDRAW:
            {
                m_eToolPoints.push_back(ll);
                break;
            }
            case SEABEDPIPELINEDRAW:
            {
                m_eToolPoints.push_back(ll);
                break;
            }
            case STRUCTUREPOINTDRAW:
            {
                m_eToolPoint = ll;
                break;
            }
            case AREANETZONEDRAW:
            {
                m_eToolPoints.push_back(ll);
                break;
            }







                //环岛线操作
            case ISLANDLINEDRAW:
            {
                m_eToolPoints.push_back(ll);
                break;
            }
            case ISLANDLINESELECT:
            {
                islandLineEditSelect(pt);
                //发送新的防区数据,以便更新
                break;
            }
            case ISLANDLINEMOVE:
            {
                zoneIsMove = true;
                break;
            }
            case ISLANDLINEMOVECTRL:
            {
                islandLineEditSelectCtrlPoint(pt);
                zoneIsMove = true;
                break;
            }
            case ISLANDLINEADDCTRL:
            {
                islandLineEditAddCtrlPoint(pt);
                break;
            }
            case ISLANDLINEDELCTRL:
            {
                islandLineEditDelCtrlPoint(pt);
                break;
            }
            case ARESELECTD:
            {
                m_leftMousePress = true;
                m_eToolPoints.clear();
                m_eToolPoints.push_back(ll);
                break;
            }
            case ROUTEORCROSSPICKUP://拾取路由或者交越点
            {
                routeOrCrossPickup(pt);
                break;
            }
            case SHIPSIMULATIONDRAW:
            {
                m_eToolPoints.push_back(ll);
                break;
            }
            case CUSTOMFLOWLINE://只允许有2个点
            {
                if(m_eToolPoints.size()<2)
                    m_eToolPoints.push_back(ll);
                else
                {
                    ms::LatLon FristPos = m_eToolPoints.at(0);
                    m_eToolPoints.clear();
                    m_eToolPoints.push_back(FristPos);
                    m_eToolPoints.push_back(ll);
                }
                break;
            }
            case LOCALMARKPOSTION:
            {
                emit signalSendLocalMarkPos(ll.lat, ll.lon);
                break;
            }
            case FIXEFREFERENCEPOINT:
            {
                emit signalSendReferencePos(ll.lat, ll.lon);
                break;
            }

#endif
            }
            // IF END

        }

        else
        {
            /************要求在拾取时，也可以移动地图时使用这样的方式*************/
            //                if(m_eTool == DRAWPICKUP)
            //                {

            //                    setActiveDrawElement(pt);
            ////                    releaseDrawStatus();//选择完毕后，释放
            //                }
            //                else
            //                {
            //                    setCursor(Qt::OpenHandCursor);
            //                    update();
            //                }
            setCursor(Qt::OpenHandCursor);
        }
        //=======zxl end===========================@}
        //zxl禁用别人的
        //            mView->framework()->TouchEssvent(GetTouchEvent(e, df::TouchEvent::TOUCH_DOWN));
        //            setCursor(Qt::OpenHandCursor);
    }
    else if(IsRightButton(e) && mUseRightKey)
    {
        qDebug()<<"right mouse press event:"<<e->pos();
        QMenu menu;
        if(mToolPtr) {
            menu.addActions(mToolPtr->getRightMenuActions(e->pos()));
        } else {
            bool bShowOtherRightKeyMenu = Profiles::instance()->value(MAP_INDEX, MAP_DISPLAY_MENU, false).toBool();

            if(curUserModel() == ZCHX::Data::ECDIS_PLUGIN_USE_DISPLAY_MODEL )
            {
                menu.addAction(tr("平移"),this,SLOT(releaseDrawStatus()));
                //处于显示模式时.对各个数据对象进行检查,如果当前选择了目标,且当前鼠标位置在对应的目标范围内,则弹出目标对应的菜单,否则只显示基本的右键菜单
                foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
                    QList<QAction*> list =  mgr->getRightMenuActions(e->pos());
                    if(list.size() > 0)
                    {
                        menu.addActions(list);
                        break;
                    }
                }

                if(bShowOtherRightKeyMenu && menu.actions().size() == 1)
                {

                    //                menu.addAction(tr("截屏"),this,SIGNAL(signalScreenShot()));
                    if(m_eTool != DRAWPICKUP) menu.addAction(tr("目标点选"),this, SLOT(setETool2DrawPickup()));
                    //                menu.addAction(tr("框选"),this,SLOT(selectAnRegion()));
                    //                menu.addAction(tr("船舶模拟"),this,SLOT(setShipSimulation()));
                    //                menu.addAction(tr("关注点"),this,SLOT(setLocationMark()));
                    //                menu.addAction(tr("固定参考点"),this,SLOT(setFixedReferencePoint()));
                    menu.addAction(tr("热点"),this,SLOT(invokeHotSpot()));
#ifdef MyTest
                    menu.addAction(tr("参数设定"), this, SIGNAL(signalSetParam()));
#endif
//                    menu.addAction(tr("设定地图数据源"), this, SLOT(resetMapSource()));
//                    menu.addAction(tr("地图数据转换"), this, SLOT(changeS572Senc()));
                }

            } else
            {
#if 0
                //处于编辑模式时,则弹出对应操作的右键菜单
                //绘制路由线时候，增加右键结束绘制
                if ROUTELINEDRAW:
                {
                    menu.addAction(tr("End draw"),  this,   SLOT(routeLineRightKeyOKSlot()));
                    menu.addAction(tr("Cancel"),    this,   SLOT(routeLineRightKeyCancelSlot()));
                }
                if(isActiveETool && (m_eTool == ROUTEINSERTCTRL ||
                                     m_eTool == ROUTEMOVECTRL ||
                                     m_eTool == ROUTEDELCTRL ||
                                     m_eTool == ROUTEADDCTRL))
                {
                    menu.addAction(tr("End edit"),this,SLOT(routeLineRightKeyOKSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(routeLineRightKeyCancelSlot()));
                }
                if(isActiveETool && (m_eTool == SHIPPLANINSERTCTRL ||
                                     m_eTool == SHIPPLANMOVECTRL ||
                                     m_eTool == SHIPPLANEDELCTRL ||
                                     m_eTool == SHIPPLANADDCTRL))
                {
                    menu.addAction(tr("End edit"),this,SLOT(shipPlanLineRightKeyOkSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(shipPlanLineRightKeyCancelSlot()));
                }
                if(isActiveETool && m_eTool == SHIPPLANDRAW)
                {
                    menu.addAction(tr("End draw"),this,SLOT(shipPlanLineRightKeyOkSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(shipPlanLineRightKeyCancelSlot()));
                }
                if(isActiveETool && m_eTool == COASTDATALINEDRAW)
                {
                    menu.addAction(tr("End draw"),this,SLOT(coastDataRightKeyOkSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(coastDataRightKeyCancelSlot()));
                }
                if(isActiveETool && m_eTool == SEABEDPIPELINEDRAW)
                {
                    menu.addAction(tr("End draw"),this,SLOT(seabedPipeLineRightKeyOkSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(seabedPipeLineRightKeyCancelSlot()));
                }
                if(isActiveETool && m_eTool == STRUCTUREPOINTDRAW)
                {
                    menu.addAction(tr("End draw"),this,SLOT(structureRightKeyOkSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(structureRightKeyCancelSlot()));
                    return;
                }
                if(isActiveETool && m_eTool == AREANETZONEDRAW)
                {
                    menu.addAction(tr("End draw"),this,SLOT(areaNetRightKeyOkSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(areaNetRightKeyCancelSlot()));
                }


                if (isActiveETool && m_eTool == SHIPSIMULATIONDRAW)
                {

                    menu.addAction(tr("End draw"),this,SLOT(shipSlumtionLineRightKeyOKSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(shipSlumtionLineRightKeyCancelSlot()));
                }
                if (isActiveETool && m_eTool == CUSTOMFLOWLINE)
                {
                    menu.addAction(tr("End draw"),this,SLOT(customFlowLineRightKeyOKSlot()));
                    menu.addAction(tr("Cancel"),this,SLOT(customFlowLineRightKeyCancelSlot()));
                    return;
                }
#endif
            }

        }
        menu.exec(QCursor::pos());
    }
}

void zchxMapWidget::mouseReleaseEvent(QMouseEvent *e)
{
    updateCurrentPos(e->pos());
    //更新点
    if(mToolPtr) {
        mToolPtr->updateOldPath();
    } else {
        if(mDrag)
        {
            mDrag = false;
            //mDx = 0;
            //mDy = 0;
            QPoint pnt = e->pos();
            if(mFrameWork) mFrameWork->pan(mPressPnt.x()- pnt.x(), mPressPnt.y() - pnt.y());

        } else
        {

            mFrameWork->setCenter(mPressPnt);
        }
    }
    e->accept();
}

void zchxMapWidget::mouseDoubleClickEvent(QMouseEvent *e)
{
    //检查当前是否正在进行鼠标操作,双击结束鼠标操作
    if(mToolPtr)
    {
        mToolPtr->endDraw();
    } else
    {
        //更新当前点的经纬度
        updateCurrentPos(e->pos());
        //检查当前是否选择了船舶或者雷达目标
        setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TARGET);
        setActiveDrawElement(e->pos(), true);
        //地图移动到当前点作为中心点
        mFrameWork->setCenter(ZCHX::Data::Point2D(e->pos()));
    }
    e->accept();
}

void zchxMapWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons() & Qt::LeftButton)
    {
        //当前鼠标按住左键移动拖动地图
        if(!mToolPtr)
        {
            mDrag = true;
            QPoint pnt = e->pos();
            if(mType == ZCHX::ZCHX_MAP_TILE)
            {
                mFrameWork->setOffSet(pnt.x() - mPressPnt.x(),pnt.y() - mPressPnt.y());
            } else
            {
                int dx = mPressPnt.x() - pnt.x();
                int dy = mPressPnt.y() - pnt.y();
                bool enable = (dx >= 5 || dy >= 5);
                if(/*enable*/1)
                {
                    mFrameWork->pan(mPressPnt.x() - pnt.x(), mPressPnt.y() - pnt.y());
                    mPressPnt = pnt;
                }
            }
        }
        //update();

    } else
    {
        //单纯地移动鼠标
        updateCurrentPos(e->pos());
        switch(m_eTool)
        {
        //画网格
        case DRAWCAMERANETGRID:
        {
            if(mToolPtr && mToolPtr->getPointSize() >= 1) mToolPtr->appendPoint(e->pos());
            break;
        }
            //移动区域
        case ZONEMOVE:
        case CHANNELMOVE:
        case MOORINGMOVE:
        case CARDMOUTHMOVE:
        case STATISTCLINEMOVE:
        {
            if(mToolPtr){
                double lonOff,latOff;
                lonOff = latOff = 0;
                zchxUtilToolGetDis4Point(mPressPnt, e->pos(),latOff,lonOff);
                mToolPtr->moveWithOffset(latOff, lonOff);
            }
            break;
        }

        case ZONEMOVECTRL:
        case CHANNELMOVECTRL:
        case MOORINGMOVECTRL:
        case CARDMOUTHMOVECTRL:
        case STATISTCLINEMOVECTRL:
        {
            if(mToolPtr) mToolPtr->changeCtrlPoint(e->pos());
            break;
        }

        default:
            break;
        }
    }
    e->accept();
}

void zchxMapWidget::updateCurrentPos(const QPoint &p)
{
    //取得当前的屏幕坐标
    ZCHX::Data::Point2D pnt(p);
    //获取当前位置对应的经纬度坐标
    if(!mFrameWork) return;
    ZCHX::Data::LatLon ll = mFrameWork->Pixel2LatLon(pnt);
    emit signalDisplayCurPos(ll.lon, ll.lat);
}

void zchxMapWidget::autoChangeCurrentStyle()
{
    if(!Profiles::instance()->value(MAP_INDEX, MAP_STYLE_AUTO_CHANGE, false).toBool()) return;
    //获取设定的白天,黄昏,夜晚的时间,根据当前时间自动设定地图颜色模式
    QString cur_str = QTime::currentTime().toString("hhmmss");
    if(cur_str < Profiles::instance()->value(MAP_INDEX, MAP_DAY_TIME).toString())
    {
        setMapStyle(MapStyleEcdisNight);
    } else if(cur_str < Profiles::instance()->value(MAP_INDEX, MAP_DUSK_TIME).toString())
    {
        setMapStyle(MapStyleEcdisDayBright);
    } else
    {
        setMapStyle(MapStyleEcdisDayDUSK);
    }

}

void zchxMapWidget::setCurZoom(int zoom)
{
    if(mFrameWork) mFrameWork->setZoom(zoom);
}


int  zchxMapWidget::zoom() const
{
    if(!mFrameWork) return 0;
    return mFrameWork->getZoom();
}
void zchxMapWidget::setCenterLL(const ZCHX::Data::LatLon& pnt )
{
    if(mFrameWork) mFrameWork->setCenter(pnt);
}

void zchxMapWidget::setCenterAndZoom(const ZCHX::Data::LatLon &ll, int zoom)
{
    if(mFrameWork) mFrameWork->setCenterAndZoom(ll, zoom);
}

void zchxMapWidget::setCenterAtTargetLL(double lat, double lon)
{
    int zoom = Profiles::instance()->value(MAP_INDEX, MAP_DEFAULT_TARGET_ZOOM).toInt();
    setCenterAndZoom(ZCHX::Data::LatLon(lat, lon), zoom);
}

ZCHX::Data::LatLon zchxMapWidget::centerLatLon() const
{
    if(mFrameWork) return mFrameWork->getCenter();
    return ZCHX::Data::LatLon();
}

void zchxMapWidget::wheelEvent(QWheelEvent *e)
{
//    qDebug()<<__FUNCTION__<<__LINE__<<e->delta()<<e->angleDelta().x()<<e->angleDelta().y()<<e->phase();
    if(QDateTime::currentMSecsSinceEpoch() - mLastWheelTime >= 1* 1000)
    {
        if(e->delta() > 0)
        {
            //放大
            if(mFrameWork) mFrameWork->zoomIn();
        } else
        {
            //缩小
            if(mFrameWork) mFrameWork->zoomOut();
        }
        mLastWheelTime = QDateTime::currentMSecsSinceEpoch();
    }
    e->accept();
}

void zchxMapWidget::setSource(const QString& source, int pos)
{
    if(isDBUpdateNow()) return;
    if(mFrameWork)
    {
        //瓦片图不需要数据解析  不需要进度条显示
        if(mType == ZCHX::ZCHX_MAP_VECTOR)
        {
            mIsDBUpdateNow = true;
#if 0
            if(!mDBProgressWidget)
            {
                mDBProgressDlg = new QProgressDialog(this);
                mDBProgressDlg->setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);
                mDBProgressDlg->setRange(0, 0);
                mDBProgressDlg->setWindowTitle(tr("数据更新"));
                mDBProgressDlg->setLabel(new QLabel(tr("正在更新地图数据,请稍候...")));
                QPushButton *cancelBtn = new QPushButton(tr("取消"), mDBProgressDlg);
                cancelBtn->setFlat(true);
                mDBProgressDlg->setCancelButton(cancelBtn);
            }
            if(mDBProgressDlg)
            {
                QRect rect = mDBProgressDlg->rect();
                rect.moveCenter(this->rect().center());
                mDBProgressDlg->move(rect.topLeft());
                mDBProgressDlg->show();
            }
#endif
            if(!mDBProgressWidget)
            {
                mDBProgressWidget = new zchxEcdisProgressWidget(this);
                connect(mDBProgressWidget, SIGNAL(signalAbouttoClose()), this, SLOT(slotPopupWidgetAbouttoClose()));
            }
            QRect rect = mDBProgressWidget->rect();
            rect.moveCenter(this->rect().center());
            mDBProgressWidget->move(rect.topLeft());
            mDBProgressWidget->show();
        }
        mFrameWork->setSource(source, pos);

    }
}

QString zchxMapWidget::getSource() const
{
    if(!mFrameWork) return QString();
    return mFrameWork->getSource();
}

void zchxMapWidget::setScaleControl(QScaleSlider * pScale)
{

}

void zchxMapWidget::setMapStyle(MapStyle mapStyle)
{
    if(mFrameWork) mFrameWork->setMapStyle(mapStyle);
}

double zchxMapWidget::zchxUtilToolAngle4north()
{
    if(mFrameWork) return mFrameWork->getRotateAngle();
    return 0.0;
}

void   zchxMapWidget::zchxUtilToolSetAngle4north(double ang)
{
    if(mFrameWork) mFrameWork->setRotateAngle(ang);
}

void zchxMapWidget::setMapRotateAtLL(double lat, double lon, double ang)
{
    if(mFrameWork)
    {
        mFrameWork->setCenter(lon, lat);
        mFrameWork->setRotateAngle(ang);
    }
}

void zchxMapWidget::resetMapRotate(double lat, double lon)
{
    if(mFrameWork)
    {
        mFrameWork->setCenter(lon, lat);
        mFrameWork->setRotateAngle(0);
    }
}

void zchxMapWidget::reset()
{
    double lat = Profiles::instance()->value(MAP_INDEX, MAP_DEFAULT_LAT).toDouble();
    double lon = Profiles::instance()->value(MAP_INDEX, MAP_DEFAULT_LON).toDouble();
    double zoom = Profiles::instance()->value(MAP_INDEX, MAP_DEFAULT_ZOOM).toInt();
    setCenterAndZoom(ZCHX::Data::LatLon(lat, lon), zoom);
}

void zchxMapWidget::setStyleAutoChange(bool val)
{
    Profiles::instance()->setValue(MAP_INDEX, MAP_STYLE_AUTO_CHANGE, val);
}

void zchxMapWidget::setGISDayBrightTime(const QTime &t)
{
    Profiles::instance()->setValue(MAP_INDEX, MAP_DAY_TIME, t.toString("hhmmss"));
}

void zchxMapWidget::setGISDuskTime(const QTime &t)
{
    Profiles::instance()->setValue(MAP_INDEX, MAP_DUSK_TIME, t.toString("hhmmss"));
}

void zchxMapWidget::setGISNightTime(const QTime &t)
{
    Profiles::instance()->setValue(MAP_INDEX, MAP_NIGHT_TIME, t.toString("hhmmss"));
}

void zchxMapWidget::setWarnColorAlphaStep(int val)
{
    Profiles::instance()->setValue(MAP_INDEX, WARN_FLAH_COLOR_ALPHA, val);
}

int zchxMapWidget::getWarnColorAlphaStep()
{
    return Profiles::instance()->value(MAP_INDEX, WARN_FLAH_COLOR_ALPHA).toInt();
}

zchxRouteDataMgr* zchxMapWidget::getRouteDataMgr()
{
    return mRouteDataMgr;
}

zchxShipPlanDataMgr* zchxMapWidget::getShipPlanDataMgr()
{
    return mShipPlanDataMgr;
}

bool zchxMapWidget::zchxUtilToolPoint4CurWindow(double lat, double lon, QPointF &p)
{
    ZCHX::Data::Point2D pos = mFrameWork->LatLon2Pixel(lat, lon);
    p.setX(pos.x);
    p.setY(pos.y);
    return true;
}

bool zchxMapWidget::zchxUtilToolLL4CurPoint(const QPointF &p, double &lat, double &lon)
{
    ZCHX::Data::Point2D pos(p.x(), p.y());
    ZCHX::Data::LatLon ll = mFrameWork->Pixel2LatLon(pos);
    lat = ll.lat;
    lon = ll.lon;
    return true;
}

ZCHX::Data::LatLon zchxMapWidget::zchxUtilToolLL4CurPoint(const QPointF &p)
{
    return mFrameWork->Pixel2LatLon(ZCHX::Data::Point2D(p));
}

void zchxMapWidget::zchxUtilToolGetDis4Point(QPointF star, QPointF end, double &latoff, double &lonoff)
{
    ZCHX::Data::LatLon endll = mFrameWork->Pixel2LatLon(ZCHX::Data::Point2D(end.x(), end.y()));
    ZCHX::Data::LatLon startll = mFrameWork->Pixel2LatLon(ZCHX::Data::Point2D(star.x(), star.y()));
    lonoff = endll.lon - startll.lon;
    latoff = endll.lat - startll.lat;
}

uint zchxMapWidget::zchxUtilToolCurZoom()
{
    return mFrameWork->getZoom();
}

void zchxMapWidget::zchxSetMapMinScale(int minVisibleZoom)
{
    if(mFrameWork)mFrameWork->setMinZoom(minVisibleZoom);
}

void zchxMapWidget::zchxSetMapMaxScale(int maxVisibleZoom)
{
    if(mFrameWork) mFrameWork->setMaxZoom(maxVisibleZoom);
}

ZCHX::Data::ECDIS_PLUGIN_USE_MODEL zchxMapWidget::curUserModel() const
{
    return mCurPluginUserModel;
}

void zchxMapWidget::setCurPluginUserModel(const ZCHX::Data::ECDIS_PLUGIN_USE_MODEL &curUserModel)
{
    if(mCurPluginUserModel == ZCHX::Data::ECDIS_PLUGIN_USE_MODEL::ECDIS_PLUGIN_USE_DISPLAY_MODEL)
    {
        //releaseDrawStatus();
    }
    mCurPluginUserModel = curUserModel;
}

ZCHX::Data::ECDIS_PICKUP_TYPEs zchxMapWidget::getCurPickupType() const
{
    return mCurPickupType;
}

void zchxMapWidget::setCurPickupType(const ZCHX::Data::ECDIS_PICKUP_TYPEs &curPickupType)
{
    mCurPickupType = curPickupType;
    //设定各个数据类是否可以选择
    //编辑模式下不显示图元
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        mgr->setPickUpAvailable(mgr->getType() & curPickupType);
    }
}

Element* zchxMapWidget::getCurrentSelectedElement()
{
    return mCurrentSelectElement;
}

void zchxMapWidget::setCurrentSelectedItem(Element* item)
{
    mCurrentSelectElement = item;
}

//GPS数据接口
void zchxMapWidget::setGPSDataList(std::list<std::shared_ptr<ZCHX::Data::GPSPoint> > list)
{
    QMutexLocker locker(&m_gpsTracksMutex);
    m_gpsTracks = list;
}

void zchxMapWidget::setGPSData(std::shared_ptr<ZCHX::Data::GPSPoint> data)
{
    QMutexLocker locker(&m_gpsTracksMutex);

    //查找是否需要更新数据
    std::list<std::shared_ptr<ZCHX::Data::GPSPoint> >::iterator it = m_gpsTracks.begin();
    it = std::find_if(it, m_gpsTracks.end(), [data](std::shared_ptr<ZCHX::Data::GPSPoint> one) -> bool {
            return (data->imei == one->imei);
});

    //更新数据
    if(it != m_gpsTracks.end())
    {
        std::shared_ptr<ZCHX::Data::GPSPoint> old = *it;
        old.reset(data.get());
    }
    else //添加数据
    {
        m_gpsTracks.push_back(data);
    }
}

void zchxMapWidget::clearGPSData()
{
    QMutexLocker locker(&m_gpsTracksMutex);
    m_gpsTracks.clear();
}

void zchxMapWidget::setIsWarningType(bool bWarningType)
{
    m_bHaveWarningType = bWarningType;
}

void zchxMapWidget::setFleet(const QMap<QString, ZCHX::Data::ITF_Fleet> &fleetMap)
{
    QList<ZCHX::Data::ITF_DangerousCircle> list;
    // 设置危险圈
    QMap<QString, ZCHX::Data::ITF_Fleet>::const_iterator fleetIt = fleetMap.begin();
    for (; fleetIt != fleetMap.end(); ++fleetIt)
    {
        ZCHX::Data::ITF_Fleet fleetInfo = *fleetIt;
        if (fleetInfo.dangerCircleRadius > 0)
        {
            ZCHX::Data::ITF_DangerousCircle circle = {fleetIt.key(), {0, 0}, 0, fleetInfo.dangerCircleRadius};
            list.push_back(circle);
        }
    }
    ZCHX_DATA_FACTORY->getDangerousMgr()->setData(list);
}

void zchxMapWidget::ScalePlus()
{
    if(mFrameWork) mFrameWork->zoomIn();
}

void zchxMapWidget::ScaleMinus()
{
    if(mFrameWork) mFrameWork->zoomOut();
}

void zchxMapWidget::ShowAll()
{
    //设置地图的最小模式
    if(mFrameWork) mFrameWork->zoom2Min();
}

//地图工作模式
//平移
void zchxMapWidget::releaseDrawStatus()
{
    //地图重新进入平移状态
    isActiveETool = false;
    m_eTool = DRAWNULL;
    setCursor(Qt::OpenHandCursor);
    setCurPluginUserModel(ZCHX::Data::ECDIS_PLUGIN_USE_DISPLAY_MODEL);
    //当前没有活动的图元
    setCurrentSelectedItem(0);
    emit signalMapIsRoaming();
    releaseDrawTool();
}

void zchxMapWidget::selectAnRegion()
{
    m_eTool = ARESELECTD;
    isActiveETool = true;
    setCursor(Qt::ArrowCursor);
}

void zchxMapWidget::setLocationMark()
{
    m_eTool = LOCALMARKPOSTION;
    isActiveETool = true;
    setCursor(Qt::ArrowCursor);
    emit signalShowLocationMarkOrReferencePos(true);
}

void zchxMapWidget::setFixedReferencePoint()
{
    m_eTool = FIXEFREFERENCEPOINT;
    isActiveETool = true;
    setCursor(Qt::ArrowCursor);
    emit signalShowLocationMarkOrReferencePos(false);
}

void zchxMapWidget::setETool2DrawPickup()
{
    m_eTool = DRAWPICKUP;
    isActiveETool = true; //拾取时是否允许移动海图 true 不允许，false 允许
    setCursor(Qt::ArrowCursor);
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TARGET );
}

void zchxMapWidget::setETool2DrawTrackTarget()
{
    m_eTool = TRACKTARGET;
    isActiveETool = true; //拾取时是否允许移动海图 true 不允许，false 允许
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_AIS);
    setCursor(Qt::ArrowCursor);
    //    update();
}

void zchxMapWidget::setETool2DrawCameraTrackTarget()
{
    m_eTool = CAMERATEACK;
    isActiveETool = true;//拾取时是否允许移动海图 true 不允许，false 允许
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_AIS | ZCHX::Data::ECDIS_PICKUP_RADARPOINT | ZCHX::Data::ECDIS_PICKUP_CAMERAVIDEOWARN);
    setCursor(Qt::ArrowCursor);
}

void zchxMapWidget::setETool2DrawGps()
{
    m_eTool = DRAWGPS;
    isActiveETool = true; //是否允许移动海图 true 不允许，false 允许
    setCursor(Qt::ArrowCursor);
}

void zchxMapWidget::setETool2DrawRouteOrCross()
{
    m_eTool = ROUTEORCROSSPICKUP;
    isActiveETool = true; //拾取时是否允许移动海图 true 不允许，false 允许
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_ROUTEANDCROSS);
    setCursor(Qt::ArrowCursor);
}

void zchxMapWidget::setETool2DrawDistance()
{
    m_eTool = DRAWDISTANCE;
    isActiveETool =true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2DrawDirAngle()
{
    m_eTool = DRAWDIRANGLE;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2DrawNull()
{
    m_eTool = DRAWNULL;
}

void zchxMapWidget::initDrawTool()
{
    if(mToolPtr && mToolPtr->getType() != m_eTool) mToolPtr.reset();
    if(!mToolPtr)
    {
        switch(m_eTool){
        case DRAWMEASUREAREA:
            mToolPtr = std::shared_ptr<zchxDrawAreaTool>(new zchxDrawAreaTool(this));
            break;
        case DRAWDISTANCE:
            mToolPtr = std::shared_ptr<zchxDrawDistanceTool>(new zchxDrawDistanceTool(this));
            break;
        case DRAWDIRANGLE:
            mToolPtr = std::shared_ptr<zchxDrawAngleTool>(new zchxDrawAngleTool(this));
            break;
        case CHANNELMANAGER:
            mToolPtr = std::shared_ptr<zchxDrawChannelZoneTool>(new zchxDrawChannelZoneTool(this));
            break;
        case MOORINGMANAGER:
            mToolPtr = std::shared_ptr<zchxDrawMooringZoneTool>(new zchxDrawMooringZoneTool(this));
            break;
        case CARDMOUTHMANAGER:
            mToolPtr = std::shared_ptr<zchxDrawCardMouthTool>(new zchxDrawCardMouthTool(this));
            break;
        case STATISTCLINEMANAGER:
            mToolPtr = std::shared_ptr<zchxDrawStatistcLineTool>(new zchxDrawStatistcLineTool(this));
            break;
        case ZONEDRAW:
            mToolPtr = std::shared_ptr<zchxDrawWarningZoneTool>(new zchxDrawWarningZoneTool(this));
            break;
        case COMMONZONESELECT:
        case ZONESELECT:
        case CHANNELSELECT:
        case MOORINGSELECT:
        case CARDMOUTHSELECT:
        case STATISTCLINESELECT:
            mToolPtr = std::shared_ptr<zchxEditZoneTool>(new zchxEditZoneTool(this, m_eTool, 4));
            break;
        case DRAWCAMERANETGRID:
            mToolPtr = std::shared_ptr<zchxDrawCameraNetGridTool>(new zchxDrawCameraNetGridTool(this));
            break;
        default:
            break;
        }

    }
    if(mToolPtr)mToolPtr->startDraw();
}

void zchxMapWidget::releaseDrawTool()
{
    if(mToolPtr) mToolPtr.reset();
}

void zchxMapWidget::setETool2DrawArea()
{
    m_eTool = DRAWMEASUREAREA;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2SelectCommonZONE()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = COMMONZONESELECT;
    isActiveETool = true;
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_COMMONZONE);
    setCursor(Qt::ArrowCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2DrawRadarZONE()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ZONEDRAWRADAR;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setRadarDisplayInfo(int targetSizeIndex, int traceLenIndex, int continueTimeIndex)
{
    m_targetSizeIndex = targetSizeIndex;
    m_traceLenIndex = traceLenIndex;
    m_continueTimeIndex = continueTimeIndex;
}

void zchxMapWidget::setETool2DrawZONE()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ZONEDRAW;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2SelectZONE()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ZONESELECT;
    isActiveETool = true;
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_WARRINGZONE);
    setCursor(Qt::ArrowCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2moveZONE()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ZONEMOVE;
    isActiveETool = true;
    setCursor(Qt::SizeAllCursor);
}

void zchxMapWidget::setETool2ctrlZONE()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ZONEMOVECTRL;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2addCtrlZONE()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ZONEADDCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
}

void zchxMapWidget::setETool2delCtrlZONE()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ZONEDELCTRL;
    isActiveETool = true;
    setCursor(Qt::ForbiddenCursor);
}


void zchxMapWidget::setETool2Draw4CoastDataLine()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = COASTDATALINEDRAW;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2Draw4SeabedPipeLineLine()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = SEABEDPIPELINEDRAW;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2Draw4StructurePoint()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = STRUCTUREPOINTDRAW;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2Draw4AreaNetZone()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = AREANETZONEDRAW;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2Draw4ChannelArea()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CHANNELMANAGER;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2SelectChannel()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CHANNELSELECT;
    isActiveETool = true;
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_CHANNELZONE);
    setCursor(Qt::ArrowCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2moveChannel()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CHANNELMOVE;
    isActiveETool = true;
    setCursor(Qt::SizeAllCursor);
}

void zchxMapWidget::setETool2ctrlChannel()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CHANNELMOVECTRL;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2addCtrlChannel()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CHANNELADDCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
}

void zchxMapWidget::setETool2delCtrlChannel()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CHANNELDELCTRL;
    isActiveETool = true;
    setCursor(Qt::ForbiddenCursor);
}

void zchxMapWidget::setETool2Draw4MooringArea()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = MOORINGMANAGER;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2SelectMooring()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = MOORINGSELECT;
    isActiveETool = true;
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_MOORINGZONE);
    setCursor(Qt::ArrowCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2moveMooring()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = MOORINGMOVE;
    isActiveETool = true;
    setCursor(Qt::SizeAllCursor);
}

void zchxMapWidget::setETool2ctrlMooring()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = MOORINGMOVECTRL;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2addCtrlMooring()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = MOORINGADDCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
}

void zchxMapWidget::setETool2delCtrlMooring()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = MOORINGDELCTRL;
    isActiveETool = true;
    setCursor(Qt::ForbiddenCursor);
}

void zchxMapWidget::setETool2Draw4CardMouthArea()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CARDMOUTHMANAGER;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2SelectCardMouth()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CARDMOUTHSELECT;
    isActiveETool = true;
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_CARDMOUTHZONE);
    setCursor(Qt::ArrowCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2moveCardMouth()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CARDMOUTHMOVE;
    isActiveETool = true;
    setCursor(Qt::SizeAllCursor);
}

void zchxMapWidget::setETool2ctrlCardMouth()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CARDMOUTHMOVECTRL;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2addCtrlCardMouth()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CARDMOUTHADDCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
}

void zchxMapWidget::setETool2delCtrlCardMouth()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = CARDMOUTHDELCTRL;
    isActiveETool = true;
    setCursor(Qt::ForbiddenCursor);
}

void zchxMapWidget::setETool2Draw4StatistcLineArea()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = STATISTCLINEMANAGER;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2SelectStatistcLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = STATISTCLINESELECT;
    isActiveETool = true;
    setCurPickupType(ZCHX::Data::ECDIS_PICKUP_TYPE::ECDIS_PICKUP_STATISTCLINEZONE);
    setCursor(Qt::ArrowCursor);
    initDrawTool();
}

void zchxMapWidget::setETool2moveStatistcLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = STATISTCLINEMOVE;
    isActiveETool = true;
    setCursor(Qt::SizeAllCursor);
}

void zchxMapWidget::setETool2ctrlStatistcLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = STATISTCLINEMOVECTRL;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2addCtrlStatistcLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = STATISTCLINEADDCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
}

void zchxMapWidget::setETool2delCtrlStatistcLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = STATISTCLINEDELCTRL;
    isActiveETool = true;
    setCursor(Qt::ForbiddenCursor);
}

void zchxMapWidget::setETool2Draw4IslandLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ISLANDLINEDRAW;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    //    update();
}

void zchxMapWidget::setETool2Select4IslandLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ISLANDLINESELECT;
    isActiveETool = true;
    setCursor(Qt::ArrowCursor);
    //    update();
}

void zchxMapWidget::setETool2move4IslandLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool =  ISLANDLINEMOVE;
    isActiveETool = true;
    setCursor(Qt::SizeAllCursor);
    //    update();
}

void zchxMapWidget::setETool2moveCtrlPoint4IslandLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ISLANDLINEMOVECTRL;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    //    update();
}

void zchxMapWidget::setETool2addCtrlPoint4IslandLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ISLANDLINEADDCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
    //    update();
}

void zchxMapWidget::setETool2delCtrlPoint4IslandLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = ISLANDLINEDELCTRL;
    isActiveETool = true;
    setCursor(Qt::ForbiddenCursor);
    //    update();
}

void zchxMapWidget::setETool2DrawShipPlanLine()
{
    //强制进入编辑状态
    mCurPluginUserModel = ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL;

    //将所有船舶的选择清空
    if(mCurrentSelectElement->getElementType() == ZCHX::Data::ELE_PLAN_LINE)
    {
        setCurrentSelectedItem(0);
    }
    emit signalNoShipPlanSelected();
    m_eTool = SHIPPLANDRAW;// ROUTELINEDRAW
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2SelectShipPlanLine()
{
    mCurPluginUserModel = ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL;
    m_eTool = SHIPPLANSELECT;
    isActiveETool = true;
    setCursor(Qt::ArrowCursor);
}

void zchxMapWidget::setETool2insertCtrlPointShipPlanLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool =  SHIPPLANINSERTCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
}

void zchxMapWidget::setETool2moveCtrlPointShipPlanLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = SHIPPLANMOVECTRL;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}

void zchxMapWidget::setETool2addCtrlPointShipPlanLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = SHIPPLANADDCTRL;
    isActiveETool = true;
    setCursor(Qt::PointingHandCursor);
}

void zchxMapWidget::setETool2delCtrlPointShipPlanLine()
{
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = SHIPPLANEDELCTRL;
    isActiveETool = true;
    setCursor(Qt::ForbiddenCursor);
}

void zchxMapWidget::setETool2DrawLocalMark()
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = DRAWLOCALMARK;
    isActiveETool = true;
    QCursor cursor(QPixmap(":/mouseCursor/mousecursor/positionMark.svg"),-1,-1);
    setCursor(cursor);
}

void zchxMapWidget::setETool2PickUpPTZ()
{
    m_eTool = PICKUPPTZ;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
}


void zchxMapWidget::setCurrentProjectID(int id)
{
    mCurrentProjectID = id;
}

void zchxMapWidget::setETool2DrawCameraNetGrid(const QSizeF& size, const QString& camera)
{
    //在编辑模式下使用
    if(ZCHX::Data::ECDIS_PLUGIN_USE_EDIT_MODEL != mCurPluginUserModel) return;
    m_eTool = DRAWCAMERANETGRID;
    isActiveETool = true;
    setCursor(Qt::CrossCursor);
    initDrawTool();
    zchxDrawCameraNetGridTool* ptr = static_cast<zchxDrawCameraNetGridTool*> (mToolPtr.get());
    if(ptr) ptr->setCameraGridParam(camera, size);
}

void zchxMapWidget::invokeHotSpot()
{
    if(!mFrameWork) return;
    ZCHX::Data::LatLon ll = mFrameWork->Pixel2LatLon(mPressPnt);
    ZCHX::Data::ITF_CloudHotSpot data;
    data.fllow = ZCHX::Data::ITF_CloudHotSpot::FLLOW_TYPE_TURN;
    data.mode = ZCHX::Data::ITF_CloudHotSpot::MODE_HANDLE;
    data.targetNumber = "";
    data.targetType = 0;
    data.targetLon = ll.lon;
    data.targetLat = ll.lat;
    qDebug()<<"hot spot:"<<mPressPnt<<"lon- lat:"<<FLOAT_STRING(ll.lon, 10)<<FLOAT_STRING(ll.lat, 10);
    emit signalInvokeHotSpot(data);
}

void zchxMapWidget::resetMapSource()
{
    if(mType == ZCHX::ZCHX_MAP_TILE)
    {
        zchxMapSourceDialog* dlg = new zchxMapSourceDialog;
        if(dlg->exec() == QDialog::Accepted)
        {
            if(!dlg->getUrl().isEmpty())
            {
                mFrameWork->setSource(dlg->getUrl(), dlg->getPos());
            }
        }
        dlg->close();

    } else
    {
        QString url = QFileDialog::getExistingDirectory();
        if(url.isEmpty()) return;
        mFrameWork->setSource(url, 0);
    }
}

void zchxMapWidget::changeS572Senc()
{
    if(mType == ZCHX::ZCHX_MAP_VECTOR)
    {
        zchxVectorMapFrameWork* frame = qobject_cast<zchxVectorMapFrameWork*>(mFrameWork);
        if(!frame) return;
        QString url = QFileDialog::getOpenFileName();
        if(url.isEmpty()) return;
        frame->changeS572SENC(url);
    }
}

//图元tooltip显示
bool zchxMapWidget::event(QEvent *e)
{
    if(e->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEve = static_cast<QHelpEvent *>(e);
        if(helpEve)
        {
            setHoverDrawElement(helpEve->pos());
            //setHoverDrawElement(m2::PointD(L2D(helpEve->x()), L2D(helpEve->y())));
        }
        else
        {
            QToolTip::hideText();
            e->ignore();
        }
        return true;
    }
    return QWidget::event(e);
}

void zchxMapWidget::setHoverDrawElement(const ZCHX::Data::Point2D &pos)
{
    //检查各个数据管理类,获取当前选择的目标
    foreach (std::shared_ptr<zchxEcdisDataMgr> mgr, ZCHX_DATA_FACTORY->getManagers()) {
        Element *ele = mgr->selectItem(pos.toPoint());
        if(ele) ele->showToolTip(mapToGlobal(pos.toPoint()));
    }
}

void zchxMapWidget::slotDBUpdateFinished()
{
    if(mDBProgressWidget)
    {
        mDBProgressWidget->signalAbouttoClose();
        mDBProgressWidget = 0;
    }
    mIsDBUpdateNow = false;
}

void zchxMapWidget::slotBadChartDirFoundNow()
{
    qDebug()<<"!!!!!!!!!!!!!!";
    zchxVectorMapSourceWidget* widget = new zchxVectorMapSourceWidget(QStringLiteral("当前地图数据目录异常,请确认"), this);
    connect(widget, SIGNAL(signalSelDir()), this, SLOT(slotResetSourceFromDlg()));
    connect(widget, SIGNAL(signalAbouttoClose()), this, SLOT(slotPopupWidgetAbouttoClose()));
    QRect rect = widget->rect();
    rect.moveCenter(this->rect().center());
    widget->move(rect.topLeft());
    widget->show();
    mPopWidgetList.append(widget);
}

void zchxMapWidget::slotResetSourceFromDlg()
{
    QString dir = QFileDialog::getExistingDirectory();
    if(dir.isEmpty()) return;
    setSource(dir, 0);
}

void zchxMapWidget::slotPopupWidgetAbouttoClose()
{
    QWidget* w = qobject_cast<QWidget*> (sender());
    if(w)
    {
        slotRemovePopupWidget(w);
    }
}

void zchxMapWidget::slotRemovePopupWidget(QWidget *w)
{
    mPopWidgetList.removeOne(w);
    if(w) w->close();
}












