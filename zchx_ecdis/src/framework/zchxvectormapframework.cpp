#include "zchxvectormapframework.h"
#include "glChartCanvas.h"
#include "Osenc.h"

extern FILE            *testDump;
using namespace qt;

zchxVectorMapFrameWork::zchxVectorMapFrameWork(QObject *parent) : zchxMapFrameWork(ZCHX::ZCHX_MAP_VECTOR, parent)
{
//    initGL();
}

zchxVectorMapFrameWork::zchxVectorMapFrameWork(double center_lat, double center_lon, int zoom, int width, int height, const QString& source, int min_zoom, int max_zoom, QObject *parent)
    : zchxMapFrameWork(ZCHX::ZCHX_MAP_VECTOR, parent)
{
    mMinZoom = min_zoom;
    mMaxZoom = max_zoom;
    mSourceUrl = source;
    mCenter.lat = center_lat;
    mCenter.lon = center_lon;
    mViewHeight = height;
    mViewWidth = width;
    initGL(center_lat, center_lon, zoom, width, height, source);
}

void zchxVectorMapFrameWork::initGL(double lat, double lon,  int zoom, int width, int height, const QString& chartDir)
{
    mGLCtrl = new glChartCanvas(lat, lon, zchxMapDataUtils::calResolutionOfPPM(zoom), width, height, chartDir);
    connect(mGLCtrl, SIGNAL(signalDBUpdateFinished()), this, SIGNAL(signalDBUpdateFinished()));
    connect(mGLCtrl, SIGNAL(signalBadChartDirFoundNow()), this, SIGNAL(signalBadChartDirFoundNow()));
    connect(mGLCtrl, SIGNAL(signalSendProcessBarText(QString)), this, SIGNAL(signalSendProcessBarText(QString)));
    connect(mGLCtrl, SIGNAL(signalSendProcessRange(int,int)), this, SIGNAL(signalSendProcessRange(int,int)));
    connect(mGLCtrl, SIGNAL(signalSendProcessVal(int)), this, SIGNAL(signalSendProcessVal(int)));
}


zchxVectorMapFrameWork::~zchxVectorMapFrameWork()
{
    if(mGLCtrl) delete mGLCtrl;
}

ZCHX::Data::LatLon  zchxVectorMapFrameWork::Pixel2LatLon(const ZCHX::Data::Point2D& pos)
{
    double lat, lon;
    mGLCtrl->getLLOfPix(lat, lon, pos.x, pos.y);
    return ZCHX::Data::LatLon(lat, lon);
}

ZCHX::Data::Point2D  zchxVectorMapFrameWork::LatLon2Pixel(const ZCHX::Data::LatLon& ll)
{
    int x ,y;
    mGLCtrl->getPixcelOfLL(x, y, ll.lat, ll.lon);
    return ZCHX::Data::Point2D(x, y);
}

//更新地图的显示范围
void zchxVectorMapFrameWork::updateDisplayRange()
{
//    int x ,y;
//    mGLCtrl->getPixcelOfLL(x, y, mCenter.lat, mCenter.lon);
//    mGLCtrl->Pan( x - mGLCtrl->GetVP().pixWidth() / 2, y - mGLCtrl->GetVP().pixHeight() / 2 );
}

void zchxVectorMapFrameWork::setCenter(double lon, double lat)
{
    zchxMapFrameWork::setCenter(lon, lat);
    int x ,y;
    mGLCtrl->getPixcelOfLL(x, y, mCenter.lat, mCenter.lon);
    mGLCtrl->Pan( x - mGLCtrl->GetVP().pixWidth() / 2, y - mGLCtrl->GetVP().pixHeight() / 2 );
}

void zchxVectorMapFrameWork::setCenterAndZoom(const ZCHX::Data::LatLon &ll, int zoom)
{
    if(zoom <= 0)
    {
        mGLCtrl->setViewCenterAndZoom(ll.lat, ll.lon);
    } else
    {
        double resolution = zchxMapDataUtils::calResolution(zoom);
        double ppm = 1 / resolution;
        mGLCtrl->setViewCenterAndZoom(ll.lat, ll.lon, ppm);
    }
}

void zchxVectorMapFrameWork::setZoom(int zoom)
{
//    zchxMapFrameWork::setZoom(zoom);
    //计算层级对应的分辨率
    double resolution = zchxMapDataUtils::calResolution(zoom);
    double ppm = 1 / resolution;
    mGLCtrl->setViewScalePPM(ppm);

}

//地图操作接口
void zchxVectorMapFrameWork::zoomIn()
{
    mGLCtrl->Zoom( 2.0, true );
}

int  zchxVectorMapFrameWork::getZoom() const
{
    double ppm = mGLCtrl->getViewScalePPM();
    double mpp = 1 / ppm;
    int zoom = zchxMapDataUtils::calZoomByResolution(mpp);
//    qDebug()<<"ppm"<<ppm<<" mmp:"<<mpp<<" zoom:"<<zoom;
    return zoom;
}

void zchxVectorMapFrameWork::zoomOut()
{
    mGLCtrl->Zoom( /*0.25*/0.5, true );
}

void zchxVectorMapFrameWork::pan(int x, int y)
{
    mGLCtrl->Pan(x, y);
}

void zchxVectorMapFrameWork::setRotateAngle(double ang)
{
    mGLCtrl->RotateDegree(ang);
}

double zchxVectorMapFrameWork::getRotateAngle()
{
    return mGLCtrl->getViewRotate();
}

//地图刷新
void zchxVectorMapFrameWork::update()
{    

}

void zchxVectorMapFrameWork::paintGL()
{
    mGLCtrl->paintGL();
}

void zchxVectorMapFrameWork::resizeGL(int w, int h)
{
    mViewWidth = w;
    mViewHeight = h;
    mGLCtrl->resizeGL(w, h);
}

void zchxVectorMapFrameWork::initializeGL()
{
    mGLCtrl->initializeGL();
}

void zchxVectorMapFrameWork::initEcdis()
{
    if(mGLCtrl) mGLCtrl->slotStartLoadEcdis();
}

void zchxVectorMapFrameWork::setSource(const QString &source, int pos)
{
    ChartDirInfo info;
    info.fullpath = source;
    ArrayOfCDI cdi;
    cdi.append(info);
    if(mGLCtrl) mGLCtrl->UpdateChartDatabaseInplace(cdi);
}

void zchxVectorMapFrameWork::changeS572SENC(const QString &src)
{
    if(!testDump) testDump = fopen("test.txt", "w");
    if(mGLCtrl) mGLCtrl->changeS572SENC(src);
    fclose(testDump);
}

void zchxVectorMapFrameWork::setDisplayCategory(ZCHX::ZCHX_DISPLAY_CATEGORY category)
{
    if(mGLCtrl->GetENCDisplayCategory() == category) return;
    mGLCtrl->SetENCDisplayCategory(category);
}

int zchxVectorMapFrameWork::getDisplayCategory() const
{
    return mGLCtrl->GetENCDisplayCategory();
}

int  zchxVectorMapFrameWork::getColorScheme() const
{
    return mGLCtrl->GetColorScheme();
}

void zchxVectorMapFrameWork::setColorScheme(ZCHX::ZCHX_COLOR_SCHEME scheme)
{
    if(mGLCtrl->GetColorScheme() == scheme) return;
    mGLCtrl->SetColorScheme(scheme);
}

void zchxVectorMapFrameWork::setDepthUnit(ZCHX::DepthUnit unit)
{
    mGLCtrl->setDepthUnit(unit);
}

int zchxVectorMapFrameWork::getDepthUnit() const
{
    return mGLCtrl->getDepthUnit();
}



void zchxVectorMapFrameWork::setShallowDepthVal(double val)
{
    mGLCtrl->setShallowDepth(val);
}

double zchxVectorMapFrameWork::getShallowDepthVal() const
{
    return mGLCtrl->getShallowDepth();
}

void zchxVectorMapFrameWork::setSafeDepthVal(double val)
{
    mGLCtrl->setSafeDepth(val);
}

double zchxVectorMapFrameWork::getSafeDepthVal() const
{
    return mGLCtrl->getSafeDepth();
}

void zchxVectorMapFrameWork::setDeepDepthVal(double val)
{
    mGLCtrl->setDeepDepth(val);
}

double zchxVectorMapFrameWork::getDeepDepthVal() const
{
    return mGLCtrl->getDeepDepth();
}

void zchxVectorMapFrameWork::setWaterReferenceDepth(int shallow, int safe, int deep)
{
    mGLCtrl->setWaterReferenceDepth(shallow, safe, deep);
}


void zchxVectorMapFrameWork::setDistanceUnit(ZCHX::DistanceUnit unit)
{
    mGLCtrl->setDistanceUnit(unit);
}

int zchxVectorMapFrameWork::getDistanceUnit() const
{
    return mGLCtrl->getDistanceUnit();
}

void zchxVectorMapFrameWork::setDepthDisplayStatus(bool sts)
{
    mGLCtrl->SetShowENCDepth(sts);
}

bool zchxVectorMapFrameWork::getDepthDisplayStatus() const
{
    return mGLCtrl->GetShowENCDepth();
}

void zchxVectorMapFrameWork::setTextDisplayStatus(bool sts)
{
    mGLCtrl->SetShowENCText(sts);
}

bool zchxVectorMapFrameWork::getTextDisplayStatus() const
{
    return mGLCtrl->GetShowENCText();
}

void zchxVectorMapFrameWork::setLightsDisplayStatus(bool sts)
{
    mGLCtrl->SetShowENCLights(sts);
}

bool zchxVectorMapFrameWork::getLightsDisplayStatus() const
{
    return mGLCtrl->GetShowENCLights();
}


void zchxVectorMapFrameWork::setGridDisplayStatus(bool sts)
{
    mGLCtrl->SetShowGrid(sts);
}

bool zchxVectorMapFrameWork::getGridDisplayStatus() const
{
    return mGLCtrl->GetShowGrid();
}


