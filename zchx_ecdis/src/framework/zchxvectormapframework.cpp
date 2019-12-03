#include "zchxvectormapframework.h"
#include "glChartCanvas.h"
#include "Osenc.h"

extern FILE            *testDump;
using namespace qt;

zchxVectorMapFrameWork::zchxVectorMapFrameWork(QObject *parent) : zchxMapFrameWork(ZCHX::ZCHX_MAP_VECTOR, parent)
{
    mGLCtrl = new glChartCanvas(0);
    connect(mGLCtrl, SIGNAL(signalDBUpdateFinished()), this, SIGNAL(signalDBUpdateFinished()));
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


//地图操作接口
void zchxVectorMapFrameWork::zoomIn()
{
    mGLCtrl->Zoom( 2.0, false );
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
    mGLCtrl->Zoom( 0.25, false );
}

void zchxVectorMapFrameWork::pan(int x, int y)
{
    mGLCtrl->Pan(x, y);
}

void zchxVectorMapFrameWork::setRotateAngle(double ang)
{
    mGLCtrl->RotateDegree(ang);
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


