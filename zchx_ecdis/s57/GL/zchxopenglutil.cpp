#include "zchxopenglutil.h"

PFNGLGENFRAMEBUFFERSEXTPROC zchxOpenGlUtil::s_glGenFramebuffers = 0;
PFNGLGENRENDERBUFFERSEXTPROC zchxOpenGlUtil::s_glGenRenderbuffers = 0;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC zchxOpenGlUtil::s_glFramebufferTexture2D = 0;
PFNGLBINDFRAMEBUFFEREXTPROC zchxOpenGlUtil::s_glBindFramebuffer = 0;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC zchxOpenGlUtil::s_glFramebufferRenderbuffer = 0;
PFNGLRENDERBUFFERSTORAGEEXTPROC zchxOpenGlUtil::s_glRenderbufferStorage = 0;
PFNGLBINDRENDERBUFFEREXTPROC zchxOpenGlUtil::s_glBindRenderbuffer = 0;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC zchxOpenGlUtil::s_glCheckFramebufferStatus = 0;
PFNGLDELETEFRAMEBUFFERSEXTPROC zchxOpenGlUtil::s_glDeleteFramebuffers = 0;
PFNGLDELETERENDERBUFFERSEXTPROC zchxOpenGlUtil::s_glDeleteRenderbuffers = 0;

PFNGLCOMPRESSEDTEXIMAGE2DPROC zchxOpenGlUtil::s_glCompressedTexImage2D = 0;
PFNGLGETCOMPRESSEDTEXIMAGEPROC zchxOpenGlUtil::s_glGetCompressedTexImage = 0;

//      Vertex Buffer Object (VBO) support
PFNGLGENBUFFERSPROC zchxOpenGlUtil::s_glGenBuffers = 0;
PFNGLBINDBUFFERPROC zchxOpenGlUtil::s_glBindBuffer = 0;
PFNGLBUFFERDATAPROC zchxOpenGlUtil::s_glBufferData = 0;
PFNGLDELETEBUFFERSPROC zchxOpenGlUtil::s_glDeleteBuffers = 0;

zchxOpenGlUtil::zchxOpenGlUtil(QObject *parent) : QObject(parent)
{

}



GLboolean zchxOpenGlUtil::QueryExtension( const char *extName )
{
    /*
     ** Search for extName in the extensions string. Use of strstr()
     ** is not sufficient because extension names can be prefixes of
     ** other extension names. Could use strtok() but the constant
     ** string returned by glGetString might be in read-only memory.
     */
    char *p;
    char *end;
    int extNameLen;

    extNameLen = strlen( extName );

    p = (char *) glGetString( GL_EXTENSIONS );
    if( NULL == p ) {
        return GL_FALSE;
    }

    end = p + strlen( p );

    while( p < end ) {
        int n = strcspn( p, " " );
        if( ( extNameLen == n ) && ( strncmp( extName, p, n ) == 0 ) ) {
            return GL_TRUE;
        }
        p += ( n + 1 );
    }
    return GL_FALSE;
}




GenericFunction zchxOpenGlUtil::ocpnGetProcAddress(const char *addr, const char *extension)
{
    char addrbuf[256];
    if(!extension)
        return (GenericFunction)NULL;
    //  If this is an extension entry point,
    //  We look explicitly in the extensions list to confirm
    //  that the request is actually supported.
    // This may be redundant, but is conservative, and only happens once per session.
    if(extension && strlen(extension)){
        QString s_extension = QString::fromUtf8(&addr[2]);
        QString s_family = QString::fromUtf8(extension);
        s_extension.insert(0, "_");
        s_extension.insert(0, s_family);
        s_extension.insert(0, "GL_");

        if(!QueryExtension( s_extension.toUtf8().data() )){
            return (GenericFunction)NULL;
        }
    }

    snprintf(addrbuf, sizeof addrbuf, "%s%s", addr, extension);
    return (GenericFunction)systemGetProcAddress(addrbuf);

}

bool  b_glEntryPointsSet;

void zchxOpenGlUtil::GetglEntryPoints( OCPN_GLCaps *pcaps )
{

    // the following are all part of framebuffer object,
    // according to opengl spec, we cannot mix EXT and ARB extensions
    // (I don't know that it could ever happen, but if it did, bad things would happen)

#ifndef __OCPN__ANDROID__
    const char *extensions[] = {"", "ARB", "EXT", 0 };
#else
    const char *extensions[] = {"OES", 0 };
#endif

    unsigned int n_ext = (sizeof extensions) / (sizeof *extensions);

    unsigned int i;
    for(i=0; i<n_ext; i++) {
        if((pcaps->m_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)
            ocpnGetProcAddress( "glGenFramebuffers", extensions[i])))
            break;
    }

    if(i<n_ext){
        pcaps->m_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC)
            ocpnGetProcAddress( "glGenRenderbuffers", extensions[i]);
        pcaps->m_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)
            ocpnGetProcAddress( "glFramebufferTexture2D", extensions[i]);
        pcaps->m_glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)
            ocpnGetProcAddress( "glBindFramebuffer", extensions[i]);
        pcaps->m_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)
            ocpnGetProcAddress( "glFramebufferRenderbuffer", extensions[i]);
        pcaps->m_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEEXTPROC)
            ocpnGetProcAddress( "glRenderbufferStorage", extensions[i]);
        pcaps->m_glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC)
            ocpnGetProcAddress( "glBindRenderbuffer", extensions[i]);
        pcaps->m_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)
            ocpnGetProcAddress( "glCheckFramebufferStatus", extensions[i]);
        pcaps->m_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC)
            ocpnGetProcAddress( "glDeleteFramebuffers", extensions[i]);
        pcaps->m_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC)
            ocpnGetProcAddress( "glDeleteRenderbuffers", extensions[i]);

        //VBO
        pcaps->m_glGenBuffers = (PFNGLGENBUFFERSPROC)
            ocpnGetProcAddress( "glGenBuffers", extensions[i]);
        pcaps->m_glBindBuffer = (PFNGLBINDBUFFERPROC)
            ocpnGetProcAddress( "glBindBuffer", extensions[i]);
        pcaps->m_glBufferData = (PFNGLBUFFERDATAPROC)
            ocpnGetProcAddress( "glBufferData", extensions[i]);
        pcaps->m_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)
            ocpnGetProcAddress( "glDeleteBuffers", extensions[i]);


    }

    //  Retry VBO entry points with all extensions
    if(0 == pcaps->m_glGenBuffers){
        for( i=0; i<n_ext; i++) {
            if((pcaps->m_glGenBuffers = (PFNGLGENBUFFERSPROC)ocpnGetProcAddress( "glGenBuffers", extensions[i])) )
                break;
        }

        if( i < n_ext ){
            pcaps->m_glBindBuffer = (PFNGLBINDBUFFERPROC) ocpnGetProcAddress( "glBindBuffer", extensions[i]);
            pcaps->m_glBufferData = (PFNGLBUFFERDATAPROC) ocpnGetProcAddress( "glBufferData", extensions[i]);
            pcaps->m_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) ocpnGetProcAddress( "glDeleteBuffers", extensions[i]);
        }
    }


#ifndef __OCPN__ANDROID__
    for(i=0; i<n_ext; i++) {
        if((pcaps->m_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)
            ocpnGetProcAddress( "glCompressedTexImage2D", extensions[i])))
            break;
    }

    if(i<n_ext){
        pcaps->m_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)
            ocpnGetProcAddress( "glGetCompressedTexImage", extensions[i]);
    }
#else
    pcaps->m_glCompressedTexImage2D =          glCompressedTexImage2D;
#endif

}





void zchxOpenGlUtil::GetglEntryPoints( void )
{
    b_glEntryPointsSet = true;

    // the following are all part of framebuffer object,
    // according to opengl spec, we cannot mix EXT and ARB extensions
    // (I don't know that it could ever happen, but if it did, bad things would happen)

#ifndef __OCPN__ANDROID__
    const char *extensions[] = {"", "ARB", "EXT", 0 };
#else
    const char *extensions[] = {"OES", 0 };
#endif

    unsigned int n_ext = (sizeof extensions) / (sizeof *extensions);

    unsigned int i;
    for(i=0; i<n_ext; i++) {
        if((s_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)
            ocpnGetProcAddress( "glGenFramebuffers", extensions[i])))
            break;
    }

    if(i<n_ext){
        s_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC)
            ocpnGetProcAddress( "glGenRenderbuffers", extensions[i]);
        s_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)
            ocpnGetProcAddress( "glFramebufferTexture2D", extensions[i]);
        s_glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)
            ocpnGetProcAddress( "glBindFramebuffer", extensions[i]);
        s_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)
            ocpnGetProcAddress( "glFramebufferRenderbuffer", extensions[i]);
        s_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEEXTPROC)
            ocpnGetProcAddress( "glRenderbufferStorage", extensions[i]);
        s_glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC)
            ocpnGetProcAddress( "glBindRenderbuffer", extensions[i]);
        s_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)
            ocpnGetProcAddress( "glCheckFramebufferStatus", extensions[i]);
        s_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC)
            ocpnGetProcAddress( "glDeleteFramebuffers", extensions[i]);
        s_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC)
            ocpnGetProcAddress( "glDeleteRenderbuffers", extensions[i]);

        //VBO
        s_glGenBuffers = (PFNGLGENBUFFERSPROC)
            ocpnGetProcAddress( "glGenBuffers", extensions[i]);
        s_glBindBuffer = (PFNGLBINDBUFFERPROC)
            ocpnGetProcAddress( "glBindBuffer", extensions[i]);
        s_glBufferData = (PFNGLBUFFERDATAPROC)
            ocpnGetProcAddress( "glBufferData", extensions[i]);
        s_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)
            ocpnGetProcAddress( "glDeleteBuffers", extensions[i]);


    }

    //  Retry VBO entry points with all extensions
    if(0 == s_glGenBuffers){
        for( i=0; i<n_ext; i++) {
            if((s_glGenBuffers = (PFNGLGENBUFFERSPROC)ocpnGetProcAddress( "glGenBuffers", extensions[i])) )
                break;
        }

        if( i < n_ext ){
            s_glBindBuffer = (PFNGLBINDBUFFERPROC) ocpnGetProcAddress( "glBindBuffer", extensions[i]);
            s_glBufferData = (PFNGLBUFFERDATAPROC) ocpnGetProcAddress( "glBufferData", extensions[i]);
            s_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) ocpnGetProcAddress( "glDeleteBuffers", extensions[i]);
        }
    }


#ifndef __OCPN__ANDROID__
    for(i=0; i<n_ext; i++) {
        if((s_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)
            ocpnGetProcAddress( "glCompressedTexImage2D", extensions[i])))
            break;
    }

    if(i<n_ext){
        s_glGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGEPROC)
            ocpnGetProcAddress( "glGetCompressedTexImage", extensions[i]);
    }
#else
    s_glCompressedTexImage2D =          glCompressedTexImage2D;
#endif

}
