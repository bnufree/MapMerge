#ifndef ZCHXOPENGLUTIL_H
#define ZCHXOPENGLUTIL_H

#include <QObject>
#include "GL/gl.h"

typedef class{
  public:
    QString Renderer;
    GLenum TextureRectangleFormat;

    bool bOldIntel;
    bool bCanDoVBO;
    bool bCanDoFBO;

    //      Vertex Buffer Object (VBO) support
    PFNGLGENBUFFERSPROC                 m_glGenBuffers;
    PFNGLBINDBUFFERPROC                 m_glBindBuffer;
    PFNGLBUFFERDATAPROC                 m_glBufferData;
    PFNGLDELETEBUFFERSPROC              m_glDeleteBuffers;

    //      Frame Buffer Object (FBO) support
    PFNGLGENFRAMEBUFFERSEXTPROC         m_glGenFramebuffers;
    PFNGLGENRENDERBUFFERSEXTPROC        m_glGenRenderbuffers;
    PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    m_glFramebufferTexture2D;
    PFNGLBINDFRAMEBUFFEREXTPROC         m_glBindFramebuffer;
    PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC m_glFramebufferRenderbuffer;
    PFNGLRENDERBUFFERSTORAGEEXTPROC     m_glRenderbufferStorage;
    PFNGLBINDRENDERBUFFEREXTPROC        m_glBindRenderbuffer;
    PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  m_glCheckFramebufferStatus;
    PFNGLDELETEFRAMEBUFFERSEXTPROC      m_glDeleteFramebuffers;
    PFNGLDELETERENDERBUFFERSEXTPROC     m_glDeleteRenderbuffers;

    PFNGLCOMPRESSEDTEXIMAGE2DPROC       m_glCompressedTexImage2D;
    PFNGLGETCOMPRESSEDTEXIMAGEPROC      m_glGetCompressedTexImage;


}OCPN_GLCaps;

typedef void (*GenericFunction)(void);
#define systemGetProcAddress(ADDR) wglGetProcAddress(ADDR)

class zchxOpenGlUtil : public QObject
{
    Q_OBJECT
public:
    explicit zchxOpenGlUtil(QObject *parent = 0);
    static void GetglEntryPoints( OCPN_GLCaps *pcaps );
    static GLboolean QueryExtension( const char *extName );
    static GenericFunction ocpnGetProcAddress(const char *addr, const char *extension);
    static void GetglEntryPoints( void );


//    static GLboolean glIsRenderBuffersExt(GLuint renderbuffer);
//    static void glBindRenderBufferExt(GLenum target, GLuint renderbuffer);
//    static void glDeleteRenderBuffersExt(GLsizei n, const GLuint *renderbuffers);
//    static void glGenRenderBuffersExt(GLsizei n, GLuint *renderbuffers);
//    static void glRenderBufferStorageExt(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
//    static GLboolean glIsFrameBufferExt(GLuint framebuffer);
//    static void glBindFrameBufferExt(GLenum target, GLuint framebuffer);
//    static void glDeleteFrameBuffersExt(GLsizei n, const GLuint *framebuffers);
//    static void glGenFrameBuffersExt(GLsizei n, GLuint *framebuffers);
//    static GLenum glCheckFrameBufferStatusExt(GLenum target);
//    static void glFrameBufferTexture1DExt(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
//    static void glFrameBufferTexture2DExt(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
//    static void glFrameBufferTexture3DExt(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
//    static void glFrameBufferRenderBufferExt(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);

//private:

    static PFNGLGENFRAMEBUFFERSEXTPROC         s_glGenFramebuffers;
    static PFNGLGENRENDERBUFFERSEXTPROC        s_glGenRenderbuffers;
    static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC    s_glFramebufferTexture2D;
    static PFNGLBINDFRAMEBUFFEREXTPROC         s_glBindFramebuffer;
    static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC s_glFramebufferRenderbuffer;
    static PFNGLRENDERBUFFERSTORAGEEXTPROC     s_glRenderbufferStorage;
    static PFNGLBINDRENDERBUFFEREXTPROC        s_glBindRenderbuffer;
    static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC  s_glCheckFramebufferStatus;
    static PFNGLDELETEFRAMEBUFFERSEXTPROC      s_glDeleteFramebuffers;
    static PFNGLDELETERENDERBUFFERSEXTPROC     s_glDeleteRenderbuffers;

    static PFNGLCOMPRESSEDTEXIMAGE2DPROC s_glCompressedTexImage2D;
    static PFNGLGETCOMPRESSEDTEXIMAGEPROC s_glGetCompressedTexImage;

    //      Vertex Buffer Object (VBO) support
    static PFNGLGENBUFFERSPROC                 s_glGenBuffers;
    static PFNGLBINDBUFFERPROC                 s_glBindBuffer;
    static PFNGLBUFFERDATAPROC                 s_glBufferData;
    static PFNGLDELETEBUFFERSPROC              s_glDeleteBuffers;

signals:

public slots:
};

#endif // ZCHXOPENGLUTIL_H
