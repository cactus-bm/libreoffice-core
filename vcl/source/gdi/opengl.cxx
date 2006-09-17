/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: opengl.cxx,v $
 *
 *  $Revision: 1.13 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 12:06:09 $
 *
 *  The Contents of this file are made available subject to
 *  the terms of GNU Lesser General Public License Version 2.1.
 *
 *
 *    GNU Lesser General Public License Version 2.1
 *    =============================================
 *    Copyright 2005 by Sun Microsystems, Inc.
 *    901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License version 2.1, as published by the Free Software Foundation.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with this library; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *    MA  02111-1307  USA
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_vcl.hxx"

#include <svsys.h>
#include <window.hxx>

#ifndef _SV_OUTDEV_HXX
#include <outdev.hxx>
#endif
#ifndef _SV_SALOGL_HXX
#include <salogl.hxx>
#endif
#ifndef _SV_OPENGL_HXX
#include <opengl.hxx>
#endif
#ifndef _SV_SALGDI_HXX
#include <salgdi.hxx>
#endif
#ifndef _SV_SVDATA_HXX
#include <svdata.hxx>
#endif
#ifndef _SV_SALINST_HXX
#include <salinst.hxx>
#endif
#ifndef _SV_WINDOW_H
#include <window.h>
#endif


#include <svapp.hxx>
#include <vos/mutex.hxx>

// -----------
// - Defines -
// -----------

// -----------------------
// - Fnc-Pointer-Typedef -
// -----------------------

typedef void ( __OPENGL_CALL *OGLFncClearDepth )( GLclampd fDepth );
typedef void ( __OPENGL_CALL *OGLFncDepthFunc )( GLenum fFunc );
typedef void ( __OPENGL_CALL *OGLFncEnable )( GLenum eCap );
typedef void ( __OPENGL_CALL *OGLFncDisable )( GLenum eCap );
typedef void ( __OPENGL_CALL *OGLFncDepthMask )( GLboolean bFlag );
typedef void ( __OPENGL_CALL *OGLFncShadeModel )( GLenum eMode );
typedef void ( __OPENGL_CALL *OGLFncEdgeFlag )( GLboolean bFlag );
typedef void ( __OPENGL_CALL *OGLFncClear )( GLbitfield nMask );
typedef void ( __OPENGL_CALL *OGLFncFlush )( void );
typedef void ( __OPENGL_CALL *OGLFncFinish )( void );
typedef void ( __OPENGL_CALL *OGLFncViewport )( GLint nX, GLint nY, GLsizei nWidth, GLsizei nHeight );
typedef void ( __OPENGL_CALL *OGLFncBegin )( GLenum eMode );
typedef void ( __OPENGL_CALL *OGLFncEnd )( void );
typedef void ( __OPENGL_CALL *OGLFncVertex3dv )( const GLdouble *fV );
typedef void ( __OPENGL_CALL *OGLFncNormal3dv )( const GLdouble *fV );
typedef void ( __OPENGL_CALL *OGLFncColor4ub )( GLubyte cRed, GLubyte cGreen, GLubyte cBlue, GLubyte cAlpha );
typedef void ( __OPENGL_CALL *OGLFncMaterialfv )( GLenum eFace, GLenum ePNname, const GLfloat *fParams );
typedef void ( __OPENGL_CALL *OGLFncMaterialf )( GLenum eFace, GLenum ePName, GLfloat fParam );
typedef void ( __OPENGL_CALL *OGLFncLightModelfv )( GLenum ePNname, const GLfloat *fParams );
typedef void ( __OPENGL_CALL *OGLFncLightModelf )( GLenum ePname, GLfloat fParam );
typedef void ( __OPENGL_CALL *OGLFncLightfv )( GLenum eLight, GLenum ePNname, const GLfloat *fParams );
typedef void ( __OPENGL_CALL *OGLFncLightf )( GLenum eLight, GLenum ePname, GLfloat fParam );
typedef void ( __OPENGL_CALL *OGLFncPolygonMode )( GLenum eFace, GLenum eMode );
typedef void ( __OPENGL_CALL *OGLFncCullFace )( GLenum eMode );
typedef void ( __OPENGL_CALL *OGLFncPointSize )( GLfloat fSize );
typedef void ( __OPENGL_CALL *OGLFncLineWidth )( GLfloat fWidth );
typedef void ( __OPENGL_CALL *OGLFncMatrixMode )( GLenum eMode );
typedef void ( __OPENGL_CALL *OGLFncLoadMatrixd )( const GLdouble *fM );
typedef void ( __OPENGL_CALL *OGLFncTexCoord2dv )( const GLdouble *pParams );
typedef void ( __OPENGL_CALL *OGLFncTexCoord3dv )( const GLdouble *fV );
typedef void ( __OPENGL_CALL *OGLFncTexImage1D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
typedef void ( __OPENGL_CALL *OGLFncTexImage2D )( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
typedef void ( __OPENGL_CALL *OGLFncCopyTexImage1D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border );
typedef void ( __OPENGL_CALL *OGLFncCopyTexImage2D )( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
typedef void ( __OPENGL_CALL *OGLFncCopyTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
typedef void ( __OPENGL_CALL *OGLFncCopyTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
typedef void ( __OPENGL_CALL *OGLFncPixelTransferf )( GLenum pname, GLfloat param );
typedef void ( __OPENGL_CALL *OGLFncPixelTransferi )( GLenum pname, GLint param );
typedef void ( __OPENGL_CALL *OGLFncGetTexLevelParameterfv )( GLenum target, GLint level, GLenum pname, GLfloat *params );
typedef void ( __OPENGL_CALL *OGLFncGetTexLevelParameteriv )( GLenum target, GLint level, GLenum pname, GLint *params );
typedef void ( __OPENGL_CALL *OGLFncGetTexParameterfv )( GLenum target, GLenum pname, GLfloat *params );
typedef void ( __OPENGL_CALL *OGLFncGetTexParameteriv )( GLenum target, GLenum pname, GLint *params );
typedef void ( __OPENGL_CALL *OGLFncTexSubImage1D )( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels );
typedef void ( __OPENGL_CALL *OGLFncTexSubImage2D )( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
typedef void ( __OPENGL_CALL *OGLFncPixelStoref )( GLenum pname, GLfloat param );
typedef void ( __OPENGL_CALL *OGLFncPixelStorei )( GLenum pname, GLint param );
typedef void ( __OPENGL_CALL *OGLFncGenTextures )( GLsizei n, GLuint *textures );
typedef GLboolean ( __OPENGL_CALL *OGLFncIsTexture )( GLuint texture );
typedef void ( __OPENGL_CALL *OGLFncBindTexture )( GLenum target, GLuint texture );
typedef void ( __OPENGL_CALL *OGLFncDeleteTextures )( GLsizei n, const GLuint *textures );
typedef GLboolean ( __OPENGL_CALL *OGLFncAreTexturesResident )( GLsizei n, const GLuint *textures, GLboolean *residences );
typedef void ( __OPENGL_CALL *OGLFncPrioritizeTextures )( GLsizei n, const GLuint *textures, const GLclampf *priorities );
typedef void ( __OPENGL_CALL *OGLFncTexEnvf )( GLenum target, GLenum pname, GLfloat param );
typedef void ( __OPENGL_CALL *OGLFncTexEnvfv )( GLenum target, GLenum pname, const GLfloat *params );
typedef void ( __OPENGL_CALL *OGLFncTexEnvi )( GLenum target, GLenum pname, GLint param );
typedef void ( __OPENGL_CALL *OGLFncTexEnviv )( GLenum target, GLenum pname, const GLint *params );
typedef void ( __OPENGL_CALL *OGLFncTexParameterf )( GLenum target, GLenum pname, GLfloat param );
typedef void ( __OPENGL_CALL *OGLFncTexParameterfv )( GLenum target, GLenum pname, const GLfloat *params );
typedef void ( __OPENGL_CALL *OGLFncTexParameteri )( GLenum target, GLenum pname, GLint param );
typedef void ( __OPENGL_CALL *OGLFncTexParameteriv )( GLenum target, GLenum pname, const GLint *params );
typedef void ( __OPENGL_CALL *OGLFncTexGend )( GLenum coord, GLenum pname, GLdouble param );
typedef void ( __OPENGL_CALL *OGLFncTexGendv )( GLenum coord, GLenum pname, const GLdouble *params );
typedef void ( __OPENGL_CALL *OGLFncTexGenf )( GLenum coord, GLenum pname, GLfloat param );
typedef void ( __OPENGL_CALL *OGLFncTexGenfv )( GLenum coord, GLenum pname, const GLfloat *params );
typedef void ( __OPENGL_CALL *OGLFncTexGeni )( GLenum coord, GLenum pname, GLint param );
typedef void ( __OPENGL_CALL *OGLFncTexGeniv )( GLenum coord, GLenum pname, const GLint *params );
typedef void ( __OPENGL_CALL *OGLFncGetIntegerv )( GLenum pname, GLint *params );
typedef void ( __OPENGL_CALL *OGLFncPolygonOffset ) ( GLfloat factor, GLfloat units );
typedef void ( __OPENGL_CALL *OGLFncScissor ) ( GLint x, GLint y, GLsizei width, GLsizei height );
typedef void ( __OPENGL_CALL *OGLFncEnableClientState ) ( GLenum array );
typedef void ( __OPENGL_CALL *OGLFncDisableClientState ) ( GLenum array );
typedef void ( __OPENGL_CALL *OGLFncVertexPointer ) ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
typedef void ( __OPENGL_CALL *OGLFncColorPointer ) ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
typedef void ( __OPENGL_CALL *OGLFncIndexPointer ) ( GLenum type, GLsizei stride, const GLvoid *pointer );
typedef void ( __OPENGL_CALL *OGLFncNormalPointer ) ( GLenum type, GLsizei stride, const GLvoid *pointer );
typedef void ( __OPENGL_CALL *OGLFncTexCoordPointer ) ( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
typedef void ( __OPENGL_CALL *OGLFncEdgeFlagPointer ) ( GLsizei stride, const GLvoid *pointer );
typedef void ( __OPENGL_CALL *OGLFncArrayElement ) ( GLint i );
typedef void ( __OPENGL_CALL *OGLFncDrawElements ) ( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );
typedef void ( __OPENGL_CALL *OGLFncDrawArrays ) ( GLenum mode, GLint first, GLsizei count );
typedef void ( __OPENGL_CALL *OGLFncInterleavedArrays ) ( GLenum format, GLsizei stride, const GLvoid *pointer );
typedef void ( __OPENGL_CALL *OGLFncLoadIdentity ) ();
typedef void ( __OPENGL_CALL *OGLFncBlendFunc ) ( GLenum sfactor, GLenum dfactor );

// ----------
// - Macros -
// ----------

#define PGRAPHICS mpOutDev->mpGraphics

#define OGL_INIT() (mpOGL && (mpOutDev->mpGraphics || mpOutDev->ImplGetGraphics()))

#define INIT_OGLFNC( FncName ) static OGLFnc##FncName pImplOpenGLFnc##FncName = NULL
#define GET_OGLFNC_GL( FncName )                                                    \
pImplOpenGLFnc##FncName = (OGLFnc##FncName) mpOGL->GetOGLFnc( "gl" #FncName );    \
if( !pImplOpenGLFnc##FncName ) bRet = FALSE;

// ----------
// - OpenGL -
// ----------

static BOOL bImplOpenGLFncPtrInitialized = FALSE;

INIT_OGLFNC( ClearDepth );
INIT_OGLFNC( DepthFunc );
INIT_OGLFNC( Enable );
INIT_OGLFNC( Disable );
INIT_OGLFNC( DepthMask );
INIT_OGLFNC( ShadeModel );
INIT_OGLFNC( EdgeFlag );
INIT_OGLFNC( Clear );
INIT_OGLFNC( Flush );
INIT_OGLFNC( Finish );
INIT_OGLFNC( Viewport );
INIT_OGLFNC( Begin );
INIT_OGLFNC( End );
INIT_OGLFNC( Vertex3dv );
INIT_OGLFNC( Normal3dv );
INIT_OGLFNC( Color4ub );
INIT_OGLFNC( Materialfv );
INIT_OGLFNC( Materialf );
INIT_OGLFNC( LightModelfv );
INIT_OGLFNC( LightModelf );
INIT_OGLFNC( Lightfv );
INIT_OGLFNC( Lightf );
INIT_OGLFNC( PolygonMode );
INIT_OGLFNC( CullFace );
INIT_OGLFNC( PointSize );
INIT_OGLFNC( LineWidth );
INIT_OGLFNC( MatrixMode );
INIT_OGLFNC( LoadMatrixd );
INIT_OGLFNC( TexCoord2dv );
INIT_OGLFNC( TexCoord3dv );
INIT_OGLFNC( TexImage1D );
INIT_OGLFNC( TexImage2D );
INIT_OGLFNC( CopyTexImage1D );
INIT_OGLFNC( CopyTexImage2D );
INIT_OGLFNC( CopyTexSubImage1D );
INIT_OGLFNC( CopyTexSubImage2D );
INIT_OGLFNC( PixelTransferf );
INIT_OGLFNC( PixelTransferi );
INIT_OGLFNC( GetTexLevelParameterfv );
INIT_OGLFNC( GetTexLevelParameteriv );
INIT_OGLFNC( GetTexParameterfv );
INIT_OGLFNC( GetTexParameteriv );
INIT_OGLFNC( TexSubImage1D );
INIT_OGLFNC( TexSubImage2D );
INIT_OGLFNC( PixelStoref );
INIT_OGLFNC( PixelStorei );
INIT_OGLFNC( GenTextures );
INIT_OGLFNC( IsTexture );
INIT_OGLFNC( BindTexture );
INIT_OGLFNC( DeleteTextures );
INIT_OGLFNC( AreTexturesResident );
INIT_OGLFNC( PrioritizeTextures );
INIT_OGLFNC( TexEnvf );
INIT_OGLFNC( TexEnvfv );
INIT_OGLFNC( TexEnvi );
INIT_OGLFNC( TexEnviv );
INIT_OGLFNC( TexParameterf );
INIT_OGLFNC( TexParameterfv );
INIT_OGLFNC( TexParameteri );
INIT_OGLFNC( TexParameteriv );
INIT_OGLFNC( TexGend );
INIT_OGLFNC( TexGendv );
INIT_OGLFNC( TexGenf );
INIT_OGLFNC( TexGenfv );
INIT_OGLFNC( TexGeni );
INIT_OGLFNC( TexGeniv );
INIT_OGLFNC( GetIntegerv );
INIT_OGLFNC( PolygonOffset );
INIT_OGLFNC( Scissor );

INIT_OGLFNC( EnableClientState );
INIT_OGLFNC( DisableClientState );
INIT_OGLFNC( VertexPointer );
INIT_OGLFNC( ColorPointer );
INIT_OGLFNC( IndexPointer );
INIT_OGLFNC( NormalPointer );
INIT_OGLFNC( TexCoordPointer );
INIT_OGLFNC( EdgeFlagPointer );
INIT_OGLFNC( ArrayElement );
INIT_OGLFNC( DrawElements );
INIT_OGLFNC( DrawArrays );
INIT_OGLFNC( InterleavedArrays );

INIT_OGLFNC( LoadIdentity );
INIT_OGLFNC( BlendFunc );

// ------------------------------------------------------------------------

BOOL OpenGL::ImplInitFncPointers()
{
    BOOL bRet = TRUE;

    GET_OGLFNC_GL( ClearDepth );
    GET_OGLFNC_GL( DepthFunc );
    GET_OGLFNC_GL( Enable );
    GET_OGLFNC_GL( Disable );
    GET_OGLFNC_GL( DepthMask );
    GET_OGLFNC_GL( ShadeModel );
    GET_OGLFNC_GL( EdgeFlag );
    GET_OGLFNC_GL( Clear );
    GET_OGLFNC_GL( Flush );
    GET_OGLFNC_GL( Finish );
    GET_OGLFNC_GL( Viewport );
    GET_OGLFNC_GL( Begin );
    GET_OGLFNC_GL( End );
    GET_OGLFNC_GL( Vertex3dv );
    GET_OGLFNC_GL( Normal3dv );
    GET_OGLFNC_GL( Color4ub );
    GET_OGLFNC_GL( Materialfv );
    GET_OGLFNC_GL( Materialf );
    GET_OGLFNC_GL( LightModelfv );
    GET_OGLFNC_GL( LightModelf  );
    GET_OGLFNC_GL( Lightfv );
    GET_OGLFNC_GL( Lightf );
    GET_OGLFNC_GL( PolygonMode  );
    GET_OGLFNC_GL( CullFace );
    GET_OGLFNC_GL( PointSize );
    GET_OGLFNC_GL( LineWidth );
    GET_OGLFNC_GL( MatrixMode );
    GET_OGLFNC_GL( LoadMatrixd  );
    GET_OGLFNC_GL( TexCoord2dv  );
    GET_OGLFNC_GL( TexCoord3dv  );
    GET_OGLFNC_GL( TexImage1D );
    GET_OGLFNC_GL( TexImage2D );
    GET_OGLFNC_GL( CopyTexImage1D );
    GET_OGLFNC_GL( CopyTexImage2D );
    GET_OGLFNC_GL( CopyTexSubImage1D );
    GET_OGLFNC_GL( CopyTexSubImage2D );
    GET_OGLFNC_GL( PixelTransferf );
    GET_OGLFNC_GL( PixelTransferi );
    GET_OGLFNC_GL( GetTexLevelParameterfv );
    GET_OGLFNC_GL( GetTexLevelParameteriv );
    GET_OGLFNC_GL( GetTexParameterfv );
    GET_OGLFNC_GL( GetTexParameteriv );
    GET_OGLFNC_GL( TexSubImage1D );
    GET_OGLFNC_GL( TexSubImage2D );
    GET_OGLFNC_GL( PixelStoref );
    GET_OGLFNC_GL( PixelStorei );
    GET_OGLFNC_GL( GenTextures );
    GET_OGLFNC_GL( IsTexture );
    GET_OGLFNC_GL( BindTexture );
    GET_OGLFNC_GL( DeleteTextures );
    GET_OGLFNC_GL( AreTexturesResident );
    GET_OGLFNC_GL( PrioritizeTextures );
    GET_OGLFNC_GL( TexEnvf );
    GET_OGLFNC_GL( TexEnvfv );
    GET_OGLFNC_GL( TexEnvi );
    GET_OGLFNC_GL( TexEnviv );
    GET_OGLFNC_GL( TexParameterf );
    GET_OGLFNC_GL( TexParameterfv );
    GET_OGLFNC_GL( TexParameteri );
    GET_OGLFNC_GL( TexParameteriv );
    GET_OGLFNC_GL( TexGend );
    GET_OGLFNC_GL( TexGendv );
    GET_OGLFNC_GL( TexGenf );
    GET_OGLFNC_GL( TexGenfv );
    GET_OGLFNC_GL( TexGeni );
    GET_OGLFNC_GL( TexGeniv );
    GET_OGLFNC_GL( GetIntegerv );
    GET_OGLFNC_GL( PolygonOffset );
    GET_OGLFNC_GL( Scissor );

    GET_OGLFNC_GL( EnableClientState );
    GET_OGLFNC_GL( DisableClientState );
    GET_OGLFNC_GL( VertexPointer );
    GET_OGLFNC_GL( ColorPointer );
    GET_OGLFNC_GL( IndexPointer );
    GET_OGLFNC_GL( NormalPointer );
    GET_OGLFNC_GL( TexCoordPointer );
    GET_OGLFNC_GL( EdgeFlagPointer );
    GET_OGLFNC_GL( ArrayElement );
    GET_OGLFNC_GL( DrawElements );
    GET_OGLFNC_GL( DrawArrays );
    GET_OGLFNC_GL( InterleavedArrays );

    GET_OGLFNC_GL( LoadIdentity );
    GET_OGLFNC_GL( BlendFunc );

    return bRet;
}

BOOL OpenGL::mbNoOGL = FALSE;

// ------------------------------------------------------------------------

OpenGL::OpenGL( OutputDevice* pOutDev ) :
            mpOutDev( pOutDev )
{
    ImplInit();
}

// ------------------------------------------------------------------------

OpenGL::~OpenGL()
{
    delete mpOGL;
}

// ------------------------------------------------------------------------

void OpenGL::ImplInit()
{
    if( ( PGRAPHICS || mpOutDev->ImplGetGraphics() ) && ! mbNoOGL )
    {
        mpOGL = ImplGetSVData()->mpDefInst->CreateSalOpenGL( PGRAPHICS );

        if ( !mpOGL->IsValid() || (!bImplOpenGLFncPtrInitialized && !ImplInitFncPointers()) )
        {
            delete mpOGL;
            mpOGL = NULL;
        }
        else
            bImplOpenGLFncPtrInitialized = TRUE;
    }
    else
        mpOGL = NULL;
}

// ------------------------------------------------------------------------

void OpenGL::SetConnectOutputDevice( OutputDevice* pOutDev )
{
    delete mpOGL;
    mpOutDev = pOutDev;
    ImplInit();
}

// ------------------------------------------------------------------------

void OpenGL::ClearDepth( GLclampd fDepth )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncClearDepth( fDepth );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::DepthFunc( GLenum eFunc )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncDepthFunc( eFunc );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Enable( GLenum eCap )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncEnable( eCap );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Disable( GLenum eCap )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncDisable( eCap );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::DepthMask( GLboolean bFlag )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncDepthMask( bFlag );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::ShadeModel( GLenum eMode )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncShadeModel( eMode );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::EdgeFlag( GLboolean bFlag )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncEdgeFlag( bFlag );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Clear( GLbitfield nMask )
{
    if( OGL_INIT() )
    {
#if defined UNX && ! defined MACOSX
        mpOGL->StartScene( PGRAPHICS );
#endif
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncClear( nMask );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Flush()
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncFlush();
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Finish()
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncFinish();
#if defined UNX && ! defined MACOSX
        mpOGL->StopScene();
#endif
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Viewport( GLint nX, GLint nY, GLsizei nWidth, GLsizei nHeight )
{
    if( OGL_INIT() )
    {
        long nOutHeight;

        if( mpOutDev->GetOutDevType() == OUTDEV_WINDOW )
            nOutHeight = ( (Window*) mpOutDev )->ImplGetFrameWindow()->mnOutHeight;
        else
            nOutHeight = mpOutDev->mnOutHeight;

        mpOGL->OGLEntry( PGRAPHICS );

        // --- RTL --- mirror viewport coordinates
        if( mpOutDev->ImplHasMirroredGraphics() )
        {
            long lx = nX + mpOutDev->mnOutOffX;
            long lwidth = nWidth;
            mpOutDev->mpGraphics->mirror( lx, lwidth, mpOutDev );
            nX = lx - mpOutDev->mnOutOffX;
        }
        pImplOpenGLFncViewport( nX + mpOutDev->mnOutOffX,
                      nOutHeight - nY - nHeight - mpOutDev->mnOutOffY,
                      nWidth, nHeight );

        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Begin( GLenum eMode )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncBegin( eMode );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::End()
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncEnd();
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Vertex3dv( const GLdouble* fVar )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncVertex3dv( fVar );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Normal3dv( const GLdouble* fVar )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncNormal3dv( fVar );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Color4ub( GLubyte cRed, GLubyte cGreen, GLubyte cBlue, GLubyte cAlpha )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncColor4ub( cRed, cGreen, cBlue, cAlpha );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Materialfv( GLenum eFace, GLenum ePName, const GLfloat *fParams )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncMaterialfv( eFace, ePName, fParams );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Materialf( GLenum eFace, GLenum ePName, GLfloat fParam )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncMaterialf( eFace, ePName, fParam );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::LightModelfv( GLenum ePName, const GLfloat *fParams )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncLightModelfv( ePName, fParams );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::LightModelf( GLenum ePName, GLfloat fParam )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncLightModelf( ePName, fParam );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Lightfv( GLenum eLight, GLenum ePName, const GLfloat *fParams )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncLightfv( eLight, ePName, fParams );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Lightf( GLenum eLight, GLenum ePName, GLfloat fParam )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncLightf( eLight, ePName, fParam );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::PolygonMode( GLenum eFace, GLenum eMode )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPolygonMode( eFace, eMode );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::CullFace( GLenum eMode )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncCullFace( eMode );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::PointSize( GLfloat fSize )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPointSize( fSize );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::LineWidth( GLfloat fWidth )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncLineWidth( fWidth );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::MatrixMode( GLenum eMode )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncMatrixMode( eMode );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::LoadMatrixd( const GLdouble *fM )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncLoadMatrixd( fM );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexCoord2dv( const GLdouble *pParams )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexCoord2dv( pParams );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexCoord3dv( const GLdouble *fVar )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexCoord3dv( fVar );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexImage1D( target, level, internalformat, width, border, format, type, pixels );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::CopyTexImage1D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncCopyTexImage1D( target, level, internalFormat, x, y, width, border );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::CopyTexImage2D( GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncCopyTexImage2D( target, level, internalFormat, x, y, width, height, border );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::CopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncCopyTexSubImage1D( target, level, xoffset, x, y, width );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::CopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncCopyTexSubImage2D( target, level, xoffset, yoffset, x, y, width, height );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::PixelTransferf( GLenum pname, GLfloat param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPixelTransferf( pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::PixelTransferi( GLenum pname, GLint param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPixelTransferi( pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::GetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncGetTexLevelParameterfv( target, level, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::GetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncGetTexLevelParameteriv( target, level, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::GetTexParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncGetTexParameterfv( target, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::GetTexParameteriv( GLenum target, GLenum pname, GLint *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncGetTexParameteriv( target, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexSubImage1D( target, level, xoffset, width, format, type, pixels );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::PixelStoref( GLenum pname, GLfloat param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPixelStoref( pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::PixelStorei( GLenum pname, GLint param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPixelStorei( pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::GenTextures( GLsizei n, GLuint *textures )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncGenTextures( n, textures );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

GLboolean OpenGL::IsTexture( GLuint texture )
{
    GLboolean bRet = FALSE;

    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        bRet = pImplOpenGLFncIsTexture( texture );
        mpOGL->OGLExit( PGRAPHICS );
    }
    return bRet;
}

// ------------------------------------------------------------------------

void OpenGL::BindTexture( GLenum target, GLuint texture )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncBindTexture( target, texture );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::DeleteTextures( GLsizei n, const GLuint *textures )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncDeleteTextures( n, textures );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

GLboolean OpenGL::AreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences )
{
    GLboolean bRet = FALSE;
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        bRet = pImplOpenGLFncAreTexturesResident( n, textures, residences );
        mpOGL->OGLExit( PGRAPHICS );
    }
    return bRet;
}

// ------------------------------------------------------------------------

void OpenGL::PrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPrioritizeTextures( n, textures, priorities );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexEnvf( GLenum target, GLenum pname, GLfloat param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexEnvf( target, pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexEnvfv( GLenum target, GLenum pname, const GLfloat *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexEnvfv( target, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexEnvi( GLenum target, GLenum pname, GLint param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexEnvi( target, pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexEnviv( GLenum target, GLenum pname, const GLint *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexEnviv( target, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexParameterf( GLenum target, GLenum pname, GLfloat param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexParameterf( target, pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexParameterfv( target, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexParameteri( GLenum target, GLenum pname, GLint param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexParameteri( target, pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexParameteriv( GLenum target, GLenum pname, const GLint *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexParameteriv( target, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexGend( GLenum coord, GLenum pname, GLdouble param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexGend( coord, pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexGendv( GLenum coord, GLenum pname, const GLdouble *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexGendv( coord, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexGenf( GLenum coord, GLenum pname, GLfloat param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexGenf( coord, pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexGenfv( GLenum coord, GLenum pname, const GLfloat *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexGenfv( coord, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexGeni( GLenum coord, GLenum pname, GLint param )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexGeni( coord, pname, param );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexGeniv( GLenum coord, GLenum pname, const GLint *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexGeniv( coord, pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::GetIntegerv( GLenum pname, GLint *params )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncGetIntegerv( pname, params );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::PolygonOffset( GLfloat factor, GLfloat units )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncPolygonOffset( factor, units );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::Scissor( GLint nX, GLint nY, GLsizei nWidth, GLsizei nHeight )
{
    if( OGL_INIT() )
    {
        long nOutHeight;

        if( mpOutDev->GetOutDevType() == OUTDEV_WINDOW )
            nOutHeight = ( (Window*) mpOutDev )->ImplGetFrameWindow()->mnOutHeight;
        else
            nOutHeight = mpOutDev->mnOutHeight;

        mpOGL->OGLEntry( PGRAPHICS );

        // --- RTL --- mirror scissor coordinates
        if( mpOutDev->ImplHasMirroredGraphics() )
        {
            long lx = nX + mpOutDev->mnOutOffX;
            long lwidth = nWidth;
            mpOutDev->mpGraphics->mirror( lx, lwidth, mpOutDev );
            nX = lx - mpOutDev->mnOutOffX;
        }
        pImplOpenGLFncScissor( nX + mpOutDev->mnOutOffX,
                     nOutHeight - nY - nHeight - mpOutDev->mnOutOffY,
                     nWidth, nHeight );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::EnableClientState( GLenum array )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncEnableClientState( array );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::DisableClientState( GLenum array )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncDisableClientState( array );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::VertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncVertexPointer( size, type, stride, pointer );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::ColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncColorPointer( size, type, stride, pointer );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::IndexPointer( GLenum type, GLsizei stride, const GLvoid *pointer )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncIndexPointer( type, stride, pointer );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::NormalPointer( GLenum type, GLsizei stride, const GLvoid *pointer )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncNormalPointer( type, stride, pointer );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::TexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncTexCoordPointer( size, type, stride, pointer );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::EdgeFlagPointer( GLsizei stride, const GLvoid *pointer )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncEdgeFlagPointer( stride, pointer );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::ArrayElement( GLint i )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncArrayElement( i );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::DrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncDrawElements( mode, count, type, indices );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::DrawArrays( GLenum mode, GLint first, GLsizei count )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncDrawArrays( mode, first, count );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::InterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncInterleavedArrays( format, stride, pointer );
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::LoadIdentity( )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncLoadIdentity();
        mpOGL->OGLExit( PGRAPHICS );
    }
}

// ------------------------------------------------------------------------

void OpenGL::BlendFunc( GLenum sfactor, GLenum dfactor )
{
    if( OGL_INIT() )
    {
        mpOGL->OGLEntry( PGRAPHICS );
        pImplOpenGLFncBlendFunc( sfactor, dfactor);
        mpOGL->OGLExit( PGRAPHICS );
    }
}

