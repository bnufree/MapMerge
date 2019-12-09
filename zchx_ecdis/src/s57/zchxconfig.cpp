#include "zchxconfig.h"
#include <QTextCodec>
#include "GL/gl.h"
#include "s52plib.h"
#include "s52utils.h"
#include "_def.h"
#include <QDebug>
#include "OCPNPlatform.h"
#include "CanvasConfig.h"
#include "chartdbs.h"
#include "styles.h"
#include "georef.h"
#include "FontMgr.h"


zchxConfig* zchxConfig::minstance = 0;
zchxConfig::MGarbage zchxConfig::Garbage;

extern glChartCanvas          *glChart;
extern double           g_ChartNotRenderScaleFactor;
extern int              g_restore_stackindex;
extern int              g_restore_dbindex;
extern ArrayOfCDI       g_ChartDirArray;
extern double           vLat, vLon, gLat, gLon;
extern double           kLat, kLon;
extern double           initial_scale_ppm, initial_rotation;
extern ZCHX::ZCHX_COLOR_SCHEME      global_color_scheme;
extern int              g_nDepthUnitDisplay;
extern QString         g_csv_locn;
extern QString         g_SENCPrefix;
extern QString         g_UserPresLibData;
extern QString         gWorldMapLocation;
extern bool             g_bDisplayGrid;         //Flag indicating if grid is to be displayed

extern bool             g_bskew_comp;
extern bool             g_bShowFPS;
extern bool             g_bsmoothpanzoom;
extern bool             g_fog_overzoom;
extern double           g_overzoom_emphasis_base;
extern bool             g_oz_vector_scale;


extern bool             g_bShowOutlines;
extern int              g_iSDMMFormat;
extern int              g_iDistanceFormat;
extern int              g_iSpeedFormat;

extern bool             g_bEnableZoomToCursor;
extern int              g_nCacheLimit;
extern int              g_memCacheLimit;
extern bool             g_bGDAL_Debug;
extern bool             g_bDebugS57;

extern bool             g_bPreserveScaleOnX;
QString         g_locale;

extern bool             g_bFullScreenQuilt;
extern bool             g_bresponsive;
extern bool             g_bGLexpert;
extern int              g_SENC_LOD_pixels;
extern int              g_chart_zoom_modifier;
extern int              g_chart_zoom_modifier_vector;

extern int              g_NMEAAPBPrecision;
extern int              g_default_font_size;
extern int              g_ChartScaleFactor;
extern float            g_ChartScaleFactorExp;
extern zchxGLOptions g_GLOptions;
extern  s52plib          *ps52plib;

/*-------------------------------------------
 * 实例化
---------------------------------------------*/
zchxConfig *zchxConfig::instance()
{
    if ( minstance == 0)
    {
        minstance = new zchxConfig(zchxFuncUtil::getConfigFileName());
    }
    return minstance;
}

zchxConfig::zchxConfig( const QString &LocalFileName ) : QSettings(LocalFileName, QSettings::IniFormat)
{
    mInitFlag = false;
    this->setIniCodec(QTextCodec::codecForName("UTF-8"));
    m_bSkipChangeSetUpdate = false;
}

/*-------------------------------------------
 * 设置默认值
---------------------------------------------*/
void zchxConfig::setDefault(const QString & prefix,const QString &key, const QVariant &def)
{
    BeginGroup(prefix);
    if(value(key).toString().isEmpty())
    {
        setValue(key, def);
    }
    endGroup();
}

void zchxConfig::WriteDefault(const QString &key, const QVariant &def)
{
    if(value(key).toString().isEmpty())
    {
        setValue(key, def);
    }
}

/*-------------------------------------------
 * 设置配置文件值
---------------------------------------------*/
void zchxConfig::setCustomValue(const QString & prefix,const QString & key, const QVariant & value)
{
    BeginGroup(prefix);
    {
        setValue(key, value);
    }
    endGroup();
}
/*-------------------------------------------
 * 返回值
---------------------------------------------*/
QVariant zchxConfig::getCustomValue(const QString& prefix,const QString &keys, const QVariant &defaultValue)
{
    return value(prefix+"/"+keys,defaultValue);
}

int  zchxConfig::getChildCount(const QString& prefix)
{
    return getChildKeys(prefix).size();
}

QStringList zchxConfig::getChildKeys(const QString& prefix)
{
    QStringList keys;
    BeginGroup(prefix);
    keys = childKeys();
    endGroup();
    return keys;
}

//bool zchxConfig::isExpert()
//{
//    return value(COMMON_SEC, OPENGL_EXPERT, false).toBool();
//}

//bool zchxConfig::isUseOpenGL()
//{
//    return value(COMMON_SEC, USE_OPEN_GL, true).toBool();
//}

//quint32 zchxConfig::getRasterFormat()
//{
//    return value(COMMON_SEC, RASTER_FORMAT, GL_RGB).toUInt();
//}

int zchxConfig::loadMyConfig()
{
    g_NMEAAPBPrecision = 3;
    g_GLOptions.m_bUseAcceleratedPanning = true;
    g_GLOptions.m_GLPolygonSmoothing = true;
    g_GLOptions.m_GLLineSmoothing = true;
    g_GLOptions.m_iTextureDimension = 512;
    g_GLOptions.m_iTextureMemorySize = 128;
    if(!g_bGLexpert){
        g_GLOptions.m_iTextureMemorySize = qMax(128, g_GLOptions.m_iTextureMemorySize);
        g_GLOptions.m_bTextureCompressionCaching = g_GLOptions.m_bTextureCompression;
    }

    g_restore_dbindex = -1;
    g_ChartNotRenderScaleFactor = 1.5;
    g_SENC_LOD_pixels = 2;
    g_bFullScreenQuilt = 1;
    g_bPreserveScaleOnX = 1;

    //    Reasonable starting point
    vLat = START_LAT;                   // display viewpoint
    vLon = START_LON;
    gLat = START_LAT;                   // GPS position, as default
    gLon = START_LON;
    initial_scale_ppm = .0003;        // decent initial value
    initial_rotation = 0;


    // Load the raw value, with no defaults, and no processing
    int ret_Val = LoadMyConfigRaw();

    //  Perform any required post processing and validation
    if(!ret_Val){
        g_ChartScaleFactorExp = zchxFuncUtil::getChartScaleFactorExp( g_ChartScaleFactor );



        if(!g_bGLexpert){
            g_GLOptions.m_iTextureMemorySize = qMax(128, g_GLOptions.m_iTextureMemorySize);
            g_GLOptions.m_bTextureCompressionCaching = g_GLOptions.m_bTextureCompression;
        }

        g_chart_zoom_modifier = qMin(g_chart_zoom_modifier,5);
        g_chart_zoom_modifier = qMax(g_chart_zoom_modifier,-5);
        g_chart_zoom_modifier_vector = qMin(g_chart_zoom_modifier_vector,5);
        g_chart_zoom_modifier_vector = qMax(g_chart_zoom_modifier_vector,-5);
    }

    if(!mInitFlag) mInitFlag = true;
    return ret_Val;
}

QVariant zchxConfig::Read(const QString &key, ParamType type, void *ret, const QVariant& def)
{
    QVariant val = value(key, def);
    if(ret)
    {
        switch (type) {
        case PARAM_BOOL:
            *(bool*)ret = val.toBool();
            break;
        case PARAM_INT:
            *(int*)ret = val.toInt();
            break;
        case PARAM_DOUBLE:
            *(double*)ret = val.toDouble();
            break;
        case PARAM_FLOAT:
            *(float*)ret = val.toFloat();
            break;
        case PARAM_STRING:
        {
            QString res = val.toString();
            res.replace("\\", "/");
            *(QString*)ret = res;
            break;
        }
        case PARAM_STRINGLIST:
            *(QStringList*)ret = val.toStringList();
            break;
        default:
            break;
        }
    }

    return val;
}

int zchxConfig::LoadMyConfigRaw( bool bAsTemplate )
{

    int read_int;
    QString val;

    BeginGroup("Settings" );

    // Some undocumented values
    Read("NCacheLimit", PARAM_INT, &g_nCacheLimit );




    int mem_limit = 0;
    Read("MEMCacheLimit",PARAM_INT, &mem_limit );
    if(mem_limit > 0)
        g_memCacheLimit = mem_limit * 1024;       // convert from MBytes to kBytes


    Read("DebugGDAL", PARAM_BOOL,&g_bGDAL_Debug );
    Read("DebugS57", PARAM_BOOL, &g_bDebugS57 );

    Read("DefaultFontSize",PARAM_INT, &g_default_font_size );

    Read("ChartObjectScaleFactor", PARAM_INT,  &g_ChartScaleFactor );


\

    Read("SkewToNorthUp",   PARAM_BOOL, &g_bskew_comp );

    Read("ShowFPS",   PARAM_BOOL, &g_bShowFPS );
    Read("NMEAAPBPrecision",  PARAM_INT,  &g_NMEAAPBPrecision );

    /* opengl options */
#ifdef ocpnUSE_GL
    if(!bAsTemplate ){
        Read("OpenGLExpert",  PARAM_BOOL, &g_bGLexpert, false );
        Read("UseAcceleratedPanning", PARAM_BOOL, &g_GLOptions.m_bUseAcceleratedPanning, true );
        Read("GPUTextureCompression", PARAM_BOOL, &g_GLOptions.m_bTextureCompression);
        Read("GPUTextureCompressionCaching", PARAM_BOOL, &g_GLOptions.m_bTextureCompressionCaching);
        Read("PolygonSmoothing", PARAM_BOOL, &g_GLOptions.m_GLPolygonSmoothing);
        Read("LineSmoothing", PARAM_BOOL, &g_GLOptions.m_GLLineSmoothing);
        Read("GPUTextureDimension", PARAM_INT, &g_GLOptions.m_iTextureDimension );
        Read("GPUTextureMemSize", PARAM_INT, &g_GLOptions.m_iTextureMemorySize );
    }
#endif




    Read("InitialStackIndex", PARAM_INT, &g_restore_stackindex );
    Read("InitialdBIndex", PARAM_INT, &g_restore_dbindex );

    Read("ChartNotRenderScaleFactor", PARAM_DOUBLE, &g_ChartNotRenderScaleFactor );
    Read("ResponsiveGraphics", PARAM_BOOL, &g_bresponsive );

    Read("ZoomDetailFactor", PARAM_INT, &g_chart_zoom_modifier );
    Read("ZoomDetailFactorVector", PARAM_INT, &g_chart_zoom_modifier_vector );


    Read("SENC_LOD_Pixels", PARAM_INT, &g_SENC_LOD_pixels, 2 );

    Read("ShowGrid", PARAM_BOOL, &g_bDisplayGrid );
    Read("ShowChartOutlines", PARAM_BOOL, &g_bShowOutlines );
    Read("SDMMFormat", PARAM_INT, &g_iSDMMFormat ); //0 = "Degrees, Decimal minutes"), 1 = "Decimal degrees", 2 = "Degrees,Minutes, Seconds"

    Read("DistanceFormat", PARAM_INT, &g_iDistanceFormat ); //0 = "Nautical miles"), 1 = "Statute miles", 2 = "Kilometers", 3 = "Meters"
    Read("SpeedFormat", PARAM_INT, &g_iSpeedFormat ); //0 = "kts"), 1 = "mph", 2 = "km/h", 3 = "m/s"


   Read("FullScreenQuilt", PARAM_BOOL, &g_bFullScreenQuilt );
    Read("PreserveScaleOnX", PARAM_BOOL, &g_bPreserveScaleOnX );

    Read("Locale", PARAM_STRING, &g_locale );


    endGroup();

    BeginGroup("Settings/GlobalState"  );

    read_int = -1;
    Read("S52_DEPTH_UNIT_SHOW", PARAM_INT, &read_int );   // default is metres
    if(read_int >= 0){
        read_int = qMax(read_int, 0);        // qualify value
        read_int = qMin(read_int, 2);
        g_nDepthUnitDisplay = read_int;
    }

    endGroup();
    QString strpres("PresentationLibraryData");
    QString valpres;
    BeginGroup("Directories");
    Read( strpres, PARAM_STRING, &valpres );       // Get the File name
    if(!valpres.isEmpty())
        g_UserPresLibData = valpres;

    QString strs("SENCFileLocation" );
    QString vals;
    Read( strs, PARAM_STRING, &vals );       // Get the Directory name
    if(!vals.isEmpty())
        g_SENCPrefix = vals;

    Read("BasemapDir", PARAM_STRING, &gWorldMapLocation );
    endGroup();


    BeginGroup("Settings/GlobalState");

    Read("nColorScheme", PARAM_INT, &read_int, 1 );
    global_color_scheme = (ZCHX::ZCHX_COLOR_SCHEME) read_int;
    QString st;

    double st_lat = 0.0, st_lon = 0.0;
    Read("VPLatLon", PARAM_STRING, &st );
    if(st.trimmed().length())
    {
        sscanf(st.toUtf8().data(), "%lf,%lf", &st_lat, &st_lon );

        //    Sanity check the lat/lon...both have to be reasonable.
        if( fabs( st_lon ) < 360. ) {
            while( st_lon < -180. )
                st_lon += 360.;

            while( st_lon > 180. )
                st_lon -= 360.;

            vLon = st_lon;
        }

        if( fabs( st_lat ) < 90.0 ) vLat = st_lat;

        QString s;
        s.sprintf("Setting Viewpoint Lat/Lon %g, %g", vLat, vLon );
        qDebug()<< s;

    }

    double st_view_scale, st_rotation;
    if( Read("VPScale", PARAM_STRING, &st ).toString().size() > 0 ) {
        sscanf( st.toUtf8().data(), "%lf", &st_view_scale );
        //    Sanity check the scale
        st_view_scale = fmax ( st_view_scale, .001/32 );
        st_view_scale = fmin ( st_view_scale, 4 );
        initial_scale_ppm = st_view_scale;
    }

    if( Read("VPRotation", PARAM_STRING, &st ).toString().size() > 0 ) {
        sscanf( st.toUtf8().data(), "%lf", &st_rotation );
        //    Sanity check the rotation
        st_rotation = fmin ( st_rotation, 360 );
        st_rotation = fmax ( st_rotation, 0 );
        initial_rotation = st_rotation * PI / 180.;
    }

    QString sll;
    double lat, lon;
    if( Read("OwnShipLatLon", PARAM_STRING, &sll ).toString().size() > 0 ) {
        sscanf( sll.toUtf8().data(), "%lf,%lf", &lat, &lon );

        //    Sanity check the lat/lon...both have to be reasonable.
        if( fabs( lon ) < 360. ) {
            while( lon < -180. )
                lon += 360.;

            while( lon > 180. )
                lon -= 360.;

            gLon = lon;
        }

        if( fabs( lat ) < 90.0 ) gLat = lat;

    }
    endGroup();

    //    Fonts

    //  Load the persistent Auxiliary Font descriptor Keys
    BeginGroup("Settings/AuxFontKeys" );
    QStringList list = childKeys();
    for( int i=0; i<list.size(); i++ ) {
        QString strk = list[i];
        QString kval;
        Read( strk, PARAM_STRING, &kval );
        if(kval.size() == 0) continue;
        bool bNewKey = FontMgr::Get().AddAuxKey(kval);
        if(!bAsTemplate && !bNewKey) {
            remove(strk);
        }
    }
    endGroup();;

    BeginGroup("Settings/QTFonts" );

    QString str;
    QStringList deleteList;
    list = childKeys();
    for( int i=0; i<list.size(); i++ ) {
        str = list[i];
        QString pval;
        Read( str, PARAM_STRING,  &pval );
        if( str.startsWith("Font" ) )
        {
            // Convert pre 3.1 setting. Can't delete old entries from inside the
            // GetNextEntry() loop, so we need to save those and delete outside.
            deleteList.append( str );
            QString oldKey = pval.left(pval.indexOf(':'));
            str = FontMgr::GetFontConfigKey( oldKey );
        }

        if( pval.isEmpty() || pval.startsWith((":")) )
        {
            deleteList.append( str );
        } else
        {
            FontMgr::Get().LoadFontNative( str, pval );
        }
    }

    for( unsigned int i=0; i<deleteList.count(); i++ )
    {
        remove(deleteList[i] );
    }
    deleteList.clear();


    //     //    Multicanvas Settings
    //     LoadCanvasConfigs();

    BeginGroup("Settings/Others");


    g_bEnableZoomToCursor = false;
    Read("EnableZoomToCursor", PARAM_BOOL, &g_bEnableZoomToCursor );
    Read("NavObjectFileName", PARAM_STRING, &m_sNavObjSetFile );

    endGroup();

    return ( 0 );
}

void zchxConfig::LoadS57Config()
{
#if 1
    if( !ps52plib )  return;
    BeginGroup("Settings/GlobalState");
    ps52plib->SetShowS57Text( Read("bShowS57Text", PARAM_BOOL, 0, true).toBool() );
    ps52plib->SetShowS57ImportantTextOnly(Read("bShowS57ImportantTextOnly", PARAM_BOOL, 0, 0).toBool() );
    ps52plib->SetShowLdisText(Read("bShowLightDescription", PARAM_BOOL, 0, 0).toBool());
    ps52plib->SetExtendLightSectors(Read("bExtendLightSectors", PARAM_BOOL, 0, 0).toBool());
    ps52plib->SetDisplayCategory((ZCHX::ZCHX_DISPLAY_CATEGORY) Read("nDisplayCategory", PARAM_INT, 0, ZCHX::ZCHX_DISPLAY_STANDARD).toInt() );
    ps52plib->m_nSymbolStyle = (LUPname) Read("nSymbolStyle", PARAM_INT,  0, (enum _LUPname) PAPER_CHART).toInt();
    ps52plib->m_nBoundaryStyle = (LUPname) Read("nBoundaryStyle", PARAM_INT, 0, PLAIN_BOUNDARIES).toInt();
    ps52plib->m_bShowSoundg = Read("bShowSoundg", PARAM_BOOL, 0, 1).toBool();
    ps52plib->m_bShowMeta =  Read("bShowMeta", PARAM_BOOL, 0, 0).toBool();
    ps52plib->m_bUseSCAMIN = Read("bUseSCAMIN", PARAM_BOOL, 0, 1).toBool();
    ps52plib->m_bShowAtonText =  Read("bShowAtonText",PARAM_BOOL, 0, 1).toBool();
    ps52plib->m_bDeClutterText = Read("bDeClutterText", PARAM_BOOL, 0,0).toBool();
    ps52plib->m_bShowNationalTexts = Read("bShowNationalText", PARAM_BOOL, 0, 1).toBool();

    double dval = Read("S52_MAR_SAFETY_CONTOUR", PARAM_DOUBLE, 0, 8.0).toDouble();
    S52_setMarinerParam( S52_MAR_SAFETY_CONTOUR, dval );
    S52_setMarinerParam( S52_MAR_SAFETY_DEPTH, dval ); // Set safety_contour and safety_depth the same

    dval = Read("S52_MAR_SHALLOW_CONTOUR",PARAM_DOUBLE, 0, 3.0).toDouble();
    S52_setMarinerParam( S52_MAR_SHALLOW_CONTOUR, dval );

    dval = Read("S52_MAR_DEEP_CONTOUR", PARAM_DOUBLE,0, 10.0).toDouble();
    S52_setMarinerParam(S52_MAR_DEEP_CONTOUR, dval );

    dval = Read("S52_MAR_TWO_SHADES", PARAM_DOUBLE, 0, 0.0).toDouble();
    S52_setMarinerParam(S52_MAR_TWO_SHADES, dval );

    ps52plib->UpdateMarinerParams();
    int read_int = Read("S52_DEPTH_UNIT_SHOW", PARAM_INT,0, 1).toInt();
    read_int = qMax(read_int, 0);        // qualify value
    read_int = qMin(read_int, 2);
    ps52plib->m_nDepthUnitDisplay = /*read_int*/1;
    g_nDepthUnitDisplay = read_int;

    //    S57 Object Class Visibility
    endGroup();
    OBJLElement *pOLE;
    QString section = "Settings/ObjectFilter";
    int iOBJMax = getChildCount(section);
    if( iOBJMax ) {
        QStringList keys = getChildKeys(section);
        BeginGroup(section);
        foreach (QString key, keys) {
            long val = Read(key, PARAM_INT,0).toLongLong();
            bool bNeedNew = false;
            QString sObj;
            if(key.startsWith("viz"))
            {
                sObj = key.mid(3);
                for( unsigned int iPtr = 0; iPtr <  ps52plib->pOBJLArray->count(); iPtr++ ) {
                    pOLE = (OBJLElement *) (  ps52plib->pOBJLArray->at( iPtr ) );
                    if( !strncmp( pOLE->OBJLName, sObj.toUtf8().data(), 6 ) ) {
                        pOLE->nViz = val;
                        bNeedNew = false;
                        break;
                    }
                }
                if( bNeedNew ) {
                    pOLE = (OBJLElement *) calloc( sizeof(OBJLElement), 1 );
                    memcpy( pOLE->OBJLName, sObj.toUtf8().data(), OBJL_NAME_LEN );
                    pOLE->nViz = 1;
                    ps52plib->pOBJLArray->append((void *) pOLE );
                }
            }
        }
        endGroup();
    }
#endif
}

#if 0
bool zchxConfig::LoadLayers(QString &path)
{
    QStringList file_array;
    wxDir dir;
    Layer *l;
    dir.Open( path );
    if( dir.IsOpened() ) {
        QString filename;
        bool cont = dir.GetFirst( &filename );
        while( cont ) {
            file_array.Clear();
            filename.Prepend( wxFileName::GetPathSeparator() );
            filename.Prepend( path );
            wxFileName f( filename );
            size_t nfiles = 0;
            if( f.GetExt().IsSameAs( wxT("gpx") ) )
                file_array.Add( filename); // single-gpx-file layer
            else{
                if(wxDir::Exists( filename ) ){
                    wxDir dir( filename );
                    if( dir.IsOpened() ){
                        nfiles = dir.GetAllFiles( filename, &file_array, wxT("*.gpx") );      // layers subdirectory set
                    }
                }
            }

            if( file_array.GetCount() ){
                l = new Layer();
                l->m_LayerID = ++g_LayerIdx;
                l->m_LayerFileName = file_array[0];
                if( file_array.GetCount() <= 1 )
                    wxFileName::SplitPath( file_array[0], NULL, NULL, &( l->m_LayerName, NULL, NULL );
                else
                    wxFileName::SplitPath( filename, NULL, NULL, &( l->m_LayerName, NULL, NULL );

                bool bLayerViz = g_bShowLayers;

                l->m_bHasVisibleNames = wxCHK_UNDETERMINED;

                l->m_bIsVisibleOnChart = bLayerViz;

                QString laymsg;
                laymsg.Printf( wxT("New layer %d: %s"), l->m_LayerID, l->m_LayerName.c_str() );
                ZCHX_LOGMSG( laymsg );

                pLayerList->Insert( l );

                //  Load the entire file array as a single layer

                for( unsigned int i = 0; i < file_array.GetCount(); i++ ) {
                    QString file_path = file_array[i];

                    if( ::wxFileExists( file_path ) ) {
                        NavObjectCollection1 *pSet = new NavObjectCollection1;
                        pSet->load_file(file_path.fn_str());
                        long nItems = pSet->LoadAllGPXObjectsAsLayer(l->m_LayerID, bLayerViz, l->m_bHasVisibleNames);
                        l->m_NoOfItems += nItems;
                        l->m_LayerType = _("Persistent");

                        QString objmsg;
                        objmsg.Printf( wxT("Loaded GPX file %s with %ld items."), file_path.c_str(), nItems );
                        ZCHX_LOGMSG( objmsg );

                        delete pSet;
                    }
                }
            }

            cont = dir.GetNext( &filename );
        }
    }
    g_bLayersLoaded = true;

    return true;
}
#endif

bool zchxConfig::LoadChartDirArray( ArrayOfCDI &ChartDirArray )
{
    //    Chart Directories
    BeginGroup("ChartDirectories" );
    QStringList keys = allKeys();
    int iDirMax = keys.size();
    if(iDirMax > 0) ChartDirArray.clear();
    int nAdjustChartDirs = 0;
    int iDir = 0;
    for( int i=0; i<iDirMax; i++ ) {
        QString str = keys[i];
        QString val;
        Read(str, PARAM_STRING, &val);
        QString dirname( val );
        if( !dirname.isEmpty() )
        {

            /*     Special case for first time run after Windows install with sample chart data...
   We desire that the sample configuration file opencpn.ini should not contain any
   installation dependencies, so...
   Detect and update the sample [ChartDirectories] entries to point to the Shared Data directory
   For instance, if the (sample) opencpn.ini file should contain shortcut coded entries like:

   [ChartDirectories]
   ChartDir1=SampleCharts\\MaptechRegion7

   then this entry will be updated to be something like:
   ChartDir1=c:\Program Files\opencpn\SampleCharts\\MaptechRegion7

   */
            if( dirname.indexOf("SampleCharts" )  == 0 ) // only update entries starting with "SampleCharts"
            {
                nAdjustChartDirs++;
                remove(str );
                QString new_dir = dirname.mid(dirname.indexOf("SampleCharts" ) );
                new_dir.insert(0, zchxFuncUtil::getDataDir() + "/" );
                dirname = new_dir;
            }

            ChartDirInfo cdi;
            cdi.fullpath = dirname.left(dirname.indexOf( '^' ));
            cdi.magic_number = dirname.mid(dirname.indexOf('^') + 1);

            ChartDirArray.append(cdi );
            iDir++;
        }
    }
    endGroup();

    if( nAdjustChartDirs ) UpdateChartDirs( ChartDirArray );
    return true;
}





bool zchxConfig::UpdateChartDirs( ArrayOfCDI& dir_array )
{
    QString str_buf;

    BeginGroup("ChartDirectories" );
    int iDirMax = childKeys().size();
    if( iDirMax ) {
        QStringList keys = childKeys();
        foreach (QString key, keys) {
            remove(key);
        }
    }

    iDirMax = dir_array.count();

    for( int iDir = 0; iDir < iDirMax; iDir++ ) {
        ChartDirInfo cdi = dir_array[iDir];

        QString dirn = cdi.fullpath;
        dirn.append(("^") );
        dirn.append( cdi.magic_number );

        str_buf.sprintf("ChartDir%d", iDir + 1 );
        setValue(str_buf, dirn);
    }

    endGroup();
    return true;
}

#if 0
void zchxConfig::CreateConfigGroups( ChartGroupArray *pGroupArray )
{
    if( !pGroupArray ) return;

    SetPath("/Groups" ) );
    Write("GroupCount", (int) pGroupArray->GetCount() );

    for( unsigned int i = 0; i < pGroupArray->GetCount(); i++ ) {
        ChartGroup *pGroup = pGroupArray->Item( i );
        QString s;
        s.Printf( _T("Group%d"), i + 1 );
        s.Prepend("/Groups/" ) );
        SetPath( s );

        Write("GroupName", pGroup->m_group_name );
        Write("GroupItemCount", (int) pGroup->m_element_array.size() );

        for( unsigned int j = 0; j < pGroup->m_element_array.size(); j++ ) {
            QString sg;
            sg.Printf( _T("Group%d/Item%d"), i + 1, j );
            sg.Prepend("/Groups/" ) );
            SetPath( sg );
            Write("IncludeItem", pGroup->m_element_array[j]->m_element_name );

            QString t;
            QStringList u = pGroup->m_element_array[j]->m_missing_name_array;
            if( u.GetCount() ) {
                for( unsigned int k = 0; k < u.GetCount(); k++ ) {
                    t += u[k];
                    t += _T(";");
                }
                Write("ExcludeItems", t );
            }
        }
    }
}

void zchxConfig::DestroyConfigGroups( void )
{
    DeleteGroup("/Groups" ) );  //zap
}

void zchxConfig::LoadConfigGroups( ChartGroupArray *pGroupArray )
{
    SetPath("/Groups" ) );
    unsigned int group_count;
    Read("GroupCount", (int *) &group_count, 0 );

    for( unsigned int i = 0; i < group_count; i++ ) {
        ChartGroup *pGroup = new ChartGroup;
        QString s;
        s.Printf( _T("Group%d"), i + 1 );
        s.Prepend("/Groups/" ) );
        SetPath( s );

        QString t;
        Read("GroupName", &t );
        pGroup->m_group_name = t;

        unsigned int item_count;
        Read("GroupItemCount", (int *) &item_count );
        for( unsigned int j = 0; j < item_count; j++ ) {
            QString sg;
            sg.Printf( _T("Group%d/Item%d"), i + 1, j );
            sg.Prepend("/Groups/" ) );
            SetPath( sg );

            QString v;
            Read("IncludeItem", &v );
            ChartGroupElement *pelement = new ChartGroupElement{v};
            pGroup->m_element_array.emplace_back( pelement );

            QString u;
            if( Read("ExcludeItems", &u ) ) {
                if( !u.IsEmpty() ) {
                    QStringTokenizer tk( u, _T(";") );
                    while( tk.HasMoreTokens() ) {
                        QString token = tk.GetNextToken();
                        pelement->m_missing_name_array.Add( token );
                    }
                }
            }
        }
        pGroupArray->Add( pGroup );
    }

}

void zchxConfig::LoadCanvasConfigs( bool bApplyAsTemplate )
{
    int n_canvas;
    QString s;
    canvasConfig *pcc;

    SetPath("/Canvas" ) );

    //  If the canvas config has never been set/persisted, use the global settings
    if(!HasEntry("CanvasConfig" ))){

        pcc = new canvasConfig(0);
        pcc->LoadFromLegacyConfig( this );
        g_canvasConfigArray.Add(pcc);

        return;
    }

    Read("CanvasConfig", (int *)&g_canvasConfig, 0 );

    // Do not recreate canvasConfigs when applying config dynamically
    if(g_canvasConfigArray.GetCount() == 0){     // This is initial load from startup
        s.Printf( _T("/Canvas/CanvasConfig%d"), 1 );
        SetPath( s );
        canvasConfig *pcca = new canvasConfig(0);
        LoadConfigCanvas(pcca, bApplyAsTemplate);
        g_canvasConfigArray.Add(pcca);

        s.Printf( _T("/Canvas/CanvasConfig%d"), 2 );
        SetPath( s );
        pcca = new canvasConfig(1);
        LoadConfigCanvas(pcca, bApplyAsTemplate);
        g_canvasConfigArray.Add(pcca);
    } else {         // This is a dynamic (i.e. Template) load
        canvasConfig *pcca = g_canvasConfigArray[0];
        s.Printf( _T("/Canvas/CanvasConfig%d"), 1 );
        SetPath( s );
        LoadConfigCanvas(pcca, bApplyAsTemplate);

        if(g_canvasConfigArray.GetCount() > 1){
            canvasConfig *pcca = g_canvasConfigArray[1];
            s.Printf( _T("/Canvas/CanvasConfig%d"), 2 );
            SetPath( s );
            LoadConfigCanvas(pcca, bApplyAsTemplate);
        } else {
            s.Printf( _T("/Canvas/CanvasConfig%d"), 2 );
            SetPath( s );
            pcca = new canvasConfig(1);
            LoadConfigCanvas(pcca, bApplyAsTemplate);
            g_canvasConfigArray.Add(pcca);
        }
    }
}

void zchxConfig::LoadConfigCanvas( canvasConfig *cConfig, bool bApplyAsTemplate )
{
#if 0
    QString st;
    double st_lat, st_lon;

    if(!bApplyAsTemplate){
        //    Reasonable starting point
        cConfig->iLat = START_LAT;     // display viewpoint
        cConfig->iLon = START_LON;

        if( Read("canvasVPLatLon", &st ) ) {
            sscanf( st.toUtf8().data( wxConvUTF8, "%lf,%lf", &st_lat, &st_lon );

            //    Sanity check the lat/lon...both have to be reasonable.
            if( fabs( st_lon ) < 360. ) {
                while( st_lon < -180. )
                    st_lon += 360.;

                while( st_lon > 180. )
                    st_lon -= 360.;

                cConfig->iLon = st_lon;
            }

            if( fabs( st_lat ) < 90.0 )
                cConfig->iLat = st_lat;
        }

        cConfig->iScale = .0003; // decent initial value
        cConfig->iRotation = 0;

        double st_view_scale;
        if( Read( QString("canvasVPScale" ), &st ) ) {
            sscanf( st.toUtf8().data( wxConvUTF8, "%lf", &st_view_scale );
            //    Sanity check the scale
            st_view_scale = fmax ( st_view_scale, .001/32 );
            st_view_scale = fmin ( st_view_scale, 4 );
            cConfig->iScale = st_view_scale;
        }

        double st_rotation;
        if( Read( QString("canvasVPRotation" ), &st ) ) {
            sscanf( st.toUtf8().data( wxConvUTF8, "%lf", &st_rotation );
            //    Sanity check the rotation
            st_rotation = fmin ( st_rotation, 360 );
            st_rotation = fmax ( st_rotation, 0 );
            cConfig->iRotation = st_rotation * PI / 180.;
        }

        Read("canvasInitialdBIndex", &cConfig->DBindex, 0 );
        Read("canvasbFollow", &cConfig->bFollow, 0 );

        Read("canvasCourseUp", &cConfig->bCourseUp, 0 );
        Read("canvasLookahead", &cConfig->bLookahead, 0 );
    }

    Read("ActiveChartGroup", &cConfig->GroupID, 0 );

    // Special check for group selection when applied as template
    if(cConfig->GroupID && bApplyAsTemplate){
        if( cConfig->GroupID > (int) g_pGroupArray->GetCount() )
            cConfig->GroupID = 0;
    }

    Read("canvasShowTides", &cConfig->bShowTides, 0 );
    Read("canvasShowCurrents", &cConfig->bShowCurrents, 0 );


    Read("canvasQuilt", &cConfig->bQuilt, 1 );
    Read("canvasShowGrid", &cConfig->bShowGrid, 0 );
    Read("canvasShowOutlines", &cConfig->bShowOutlines, 0 );
    Read("canvasShowDepthUnits", &cConfig->bShowDepthUnits, 0 );

    Read("canvasShowAIS", &cConfig->bShowAIS, 1 );
    Read("canvasAttenAIS", &cConfig->bAttenAIS, 0 );

    // ENC options
    Read("canvasShowENCText", &cConfig->bShowENCText, 1 );
    Read("canvasENCDisplayCategory", &cConfig->nENCDisplayCategory, STANDARD );
    Read("canvasENCShowDepths", &cConfig->bShowENCDepths, 1 );
    Read("canvasENCShowBuoyLabels", &cConfig->bShowENCBuoyLabels, 1 );
    Read("canvasENCShowLightDescriptions", &cConfig->bShowENCLightDescriptions, 1 );
    Read("canvasENCShowLights", &cConfig->bShowENCLights, 1 );


    int sx, sy;
    Read("canvasSizeX", &sx, 0 );
    Read("canvasSizeY", &sy, 0 );
    cConfig->canvasSize = wxSize(sx, sy);

#endif


}


void zchxConfig::SaveCanvasConfigs( )
{
#if 0
    SetPath("/Canvas" ) );
    Write("CanvasConfig", (int )g_canvasConfig );

    QString s;
    canvasConfig *pcc;

    switch( g_canvasConfig ){

    case 0:
    default:

        s.Printf( _T("/Canvas/CanvasConfig%d"), 1 );
        SetPath( s );

        if(g_canvasConfigArray.GetCount() > 0 ){
            pcc = g_canvasConfigArray.Item(0);
            if(pcc){
                SaveConfigCanvas(pcc);
            }
        }
        break;

    case 1:

        if(g_canvasConfigArray.GetCount() > 1 ){

            s.Printf( _T("/Canvas/CanvasConfig%d"), 1 );
            SetPath( s );
            pcc = g_canvasConfigArray.Item(0);
            if(pcc){
                SaveConfigCanvas(pcc);
            }

            s.Printf( _T("/Canvas/CanvasConfig%d"), 2 );
            SetPath( s );
            pcc = g_canvasConfigArray.Item(1);
            if(pcc){
                SaveConfigCanvas(pcc);
            }
        }
        break;

    }
#endif
}


void zchxConfig::SaveConfigCanvas( canvasConfig *cConfig )
{
#if 0
    QString st1;

    if(cConfig->canvas){
        ViewPort vp = cConfig->canvas->GetVP();

        if( vp.IsValid() ) {
            st1.Printf("%10.4f,%10.4f", vp.clat, vp.clon );
            Write("canvasVPLatLon", st1 );
            st1.Printf("%g", vp.view_scale_ppm );
            Write("canvasVPScale", st1 );
            st1.Printf("%i", ((int)(vp.rotation * 180 / PI)) % 360 );
            Write("canvasVPRotation", st1 );
        }

        int restore_dbindex = 0;
        ChartStack *pcs = cConfig->canvas->GetpCurrentStack();
        if(pcs)
            restore_dbindex = pcs->GetCurrentEntrydbIndex();
        if( cConfig->canvas->GetQuiltMode())
            restore_dbindex = cConfig->canvas->GetQuiltReferenceChartIndex();
        Write("canvasInitialdBIndex", restore_dbindex );

        Write("canvasbFollow", cConfig->canvas->m_bFollow );
        Write("ActiveChartGroup", cConfig->canvas->m_groupIndex );

        Write("canvasToolbarConfig", cConfig->canvas->GetToolbarConfigString() );
        Write("canvasShowToolbar", 0 );  //cConfig->canvas->GetToolbarEnable() );

        Write("canvasQuilt", cConfig->canvas->GetQuiltMode() );
        Write("canvasShowGrid", cConfig->canvas->GetShowGrid() );
        Write("canvasShowOutlines", cConfig->canvas->GetShowOutlines() );
        Write("canvasShowDepthUnits", cConfig->canvas->GetShowDepthUnits() );

        Write("canvasShowAIS", cConfig->canvas->GetShowAIS() );
        Write("canvasAttenAIS", cConfig->canvas->GetAttenAIS() );

        Write("canvasShowTides", cConfig->canvas->GetbShowTide() );
        Write("canvasShowCurrents", cConfig->canvas->GetbShowCurrent() );

        // ENC options
        Write("canvasShowENCText", cConfig->canvas->GetShowENCText() );
        Write("canvasENCDisplayCategory", cConfig->canvas->GetENCDisplayCategory() );
        Write("canvasENCShowDepths", cConfig->canvas->GetShowENCDepth() );
        Write("canvasENCShowBuoyLabels", cConfig->canvas->GetShowENCBuoyLabels() );
        Write("canvasENCShowLightDescriptions", cConfig->canvas->GetShowENCLightDesc() );
        Write("canvasENCShowLights", cConfig->canvas->GetShowENCLights() );

        Write("canvasCourseUp", cConfig->canvas->GetCourseUP() );
        Write("canvasLookahead", cConfig->canvas->GetLookahead() );


        int width = cConfig->canvas->GetSize().x;
        //  if(cConfig->canvas->IsPrimaryCanvas()){
        //      width = qMax(width, gFrame->GetClientSize().x / 10);
        //  }
        //  else{
        //      width = qMin(width, gFrame->GetClientSize().x  * 9 / 10);
        //  }

        Write("canvasSizeX", width );
        Write("canvasSizeY", cConfig->canvas->GetSize().y );

    }
#endif
}

#endif


void zchxConfig::DeleteGroup(const QString &group)
{
    BeginGroup(group);
    QStringList keys = childKeys();
    foreach (QString key, keys) {
        remove(key);
    }
    endGroup();
}

void zchxConfig::BeginGroup(const QString &g)
{
    if(!group().isEmpty()) endGroup();
    beginGroup(g);
}

void zchxConfig::UpdateSettings()
{
    //    Global options and settings
    BeginGroup("Settings");

    Write("DefaultFontSize", g_default_font_size );
    Write("ShowGrid", g_bDisplayGrid );
    Write("ShowChartOutlines", g_bShowOutlines );
    Write("SDMMFormat", g_iSDMMFormat );
    Write("ChartObjectScaleFactor", g_ChartScaleFactor );
    Write("SkewToNorthUp", g_bskew_comp );
    Write("ShowFPS", g_bShowFPS );

    Write("ZoomDetailFactor", g_chart_zoom_modifier );
    Write("ZoomDetailFactorVector", g_chart_zoom_modifier_vector );

    Write("FogOnOverzoom", g_fog_overzoom );
    Write("OverzoomVectorScale", g_oz_vector_scale );
    Write("OverzoomEmphasisBase", g_overzoom_emphasis_base );
    Write("UseAcceleratedPanning", g_GLOptions.m_bUseAcceleratedPanning );
    Write("GPUTextureCompression", g_GLOptions.m_bTextureCompression);
    Write("GPUTextureCompressionCaching", g_GLOptions.m_bTextureCompressionCaching);
    Write("GPUTextureDimension", g_GLOptions.m_iTextureDimension );
    Write("GPUTextureMemSize", g_GLOptions.m_iTextureMemorySize );
    Write("PolygonSmoothing", g_GLOptions.m_GLPolygonSmoothing);
    Write("LineSmoothing", g_GLOptions.m_GLLineSmoothing);
    Write("SmoothPanZoom", g_bsmoothpanzoom );



    Write("PreserveScaleOnX", g_bPreserveScaleOnX );

    Write("InitialStackIndex", g_restore_stackindex );
    Write("InitialdBIndex", g_restore_dbindex );

    Write("NMEAAPBPrecision", g_NMEAAPBPrecision );
        Write("DistanceFormat", g_iDistanceFormat );
        Write("SpeedFormat", g_iSpeedFormat );
    Write("ResponsiveGraphics", g_bresponsive );


    Write("Locale", g_locale );


    //    S57 Object Filter Settings
    endGroup();
#if 0
    BeginGroup("Settings/ObjectFilter");

    if(  ps52plib ) {
        for( unsigned int iPtr = 0; iPtr <  ps52plib->pOBJLArray->count(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) (  ps52plib->pOBJLArray->at( iPtr ) );

            QString st1("viz");
            char name[7];
            strncpy( name, pOLE->OBJLName, 6 );
            name[6] = 0;
            st1.append( QString::fromUtf8(name) );
            Write( st1, pOLE->nViz );
        }
    }
    endGroup();
#endif

    //    Global State

    BeginGroup("Settings/GlobalState");

    QString st1;

    //     if( cc1 ) {
    //  ViewPort vp = cc1->GetVP();
    //
    //  if( vp.IsValid() ) {
    //      st1.Printf("%10.4f,%10.4f", vp.clat, vp.clon );
    //      Write("VPLatLon", st1 );
    //      st1.Printf("%g", vp.view_scale_ppm );
    //      Write("VPScale", st1 );
    //      st1.Printf("%i", ((int)(vp.rotation * 180 / PI)) % 360 );
    //      Write("VPRotation", st1 );
    //  }
    //     }

    st1.sprintf("%10.4f, %10.4f", gLat, gLon );
    Write("OwnShipLatLon", st1 );
    Write("nColorScheme", global_color_scheme );

    Write("S52_DEPTH_UNIT_SHOW", g_nDepthUnitDisplay );

    endGroup();

    BeginGroup("Settings/GlobalState");
#if 0
    if(  ps52plib ) {
        Write("bShowS57Text",  ps52plib->GetShowS57Text() );
        Write("bShowS57ImportantTextOnly",  ps52plib->GetShowS57ImportantTextOnly() );
        Write("nDisplayCategory", ps52plib->GetDisplayCategory() );
        Write("nSymbolStyle", (int)  ps52plib->m_nSymbolStyle );
        Write("nBoundaryStyle", (int)  ps52plib->m_nBoundaryStyle );

        Write("bShowSoundg",  ps52plib->m_bShowSoundg );
        Write("bShowMeta",  ps52plib->m_bShowMeta );
        Write("bUseSCAMIN",  ps52plib->m_bUseSCAMIN );
        Write("bShowAtonText",  ps52plib->m_bShowAtonText );
        Write("bShowLightDescription",  ps52plib->m_bShowLdisText );
        Write("bExtendLightSectors",  ps52plib->m_bExtendLightSectors );
        Write("bDeClutterText",  ps52plib->m_bDeClutterText );
        Write("bShowNationalText",  ps52plib->m_bShowNationalTexts );

        Write("S52_MAR_SAFETY_CONTOUR", S52_getMarinerParam( S52_MAR_SAFETY_CONTOUR ) );
        Write("S52_MAR_SHALLOW_CONTOUR", S52_getMarinerParam( S52_MAR_SHALLOW_CONTOUR ) );
        Write("S52_MAR_DEEP_CONTOUR", S52_getMarinerParam( S52_MAR_DEEP_CONTOUR ) );
        Write("S52_MAR_TWO_SHADES", S52_getMarinerParam( S52_MAR_TWO_SHADES ) );
        Write("S52_DEPTH_UNIT_SHOW",  ps52plib->m_nDepthUnitDisplay );
    }
#endif
    endGroup();
    BeginGroup("Directories");
    Write("S57DataLocation", ("") );
    Write("BasemapDir", gWorldMapLocation );
    endGroup();


    //    Fonts

    //  Store the persistent Auxiliary Font descriptor Keys
    BeginGroup("Settings/AuxFontKeys" );

    QStringList keyArray = FontMgr::Get().GetAuxKeyArray();
    for(unsigned int i=0 ; i <  keyArray.count() ; i++){
        QString key;
        key.sprintf("Key%i", i);
        QString keyval = keyArray[i];
        Write( key, keyval );
    }
    endGroup();

    QString font_path;
    font_path = ("Settings/QTFonts" );
    DeleteGroup(font_path);
    BeginGroup(font_path );

    int nFonts = FontMgr::Get().GetNumFonts();

    for( int i = 0; i < nFonts; i++ ) {
        QString cfstring(FontMgr::Get().GetConfigString(i));
        QString valstring = FontMgr::Get().GetFullConfigDesc( i );
        Write( cfstring, valstring );
    }
    endGroup();


    BeginGroup("Settings/Others" );


    Write("EnableZoomToCursor", g_bEnableZoomToCursor );
    endGroup();
//    SaveCanvasConfigs();
}



zchxConfig::~zchxConfig()
{
    qDebug()<<"now destruct here";
}






