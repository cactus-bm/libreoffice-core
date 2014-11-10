/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#include "openglgdiimpl.hxx"

#include <vcl/gradient.hxx>
#include <salframe.hxx>
#include "salvd.hxx"
#include <basegfx/polygon/b2dpolygontools.hxx>
#include <basegfx/polygon/b2dpolygontriangulator.hxx>

#include <vcl/opengl/OpenGLHelper.hxx>
#include "opengl/salbmp.hxx"

#include <vector>

#define GL_ATTRIB_POS 0
#define GL_ATTRIB_TEX 1

#define glUniformColor(nUniform, nColor, nTransparency)    \
    glUniform4f( nUniform,                                 \
                 ((float) SALCOLOR_RED( nColor )) / 255,   \
                 ((float) SALCOLOR_GREEN( nColor )) / 255, \
                 ((float) SALCOLOR_BLUE( nColor )) / 255,  \
                 (100 - nTransparency) * (1.0 / 100) )

OpenGLSalGraphicsImpl::~OpenGLSalGraphicsImpl()
{
}

void OpenGLSalGraphicsImpl::freeResources()
{
    // Delete shaders, programs and textures if not shared
}

void OpenGLSalGraphicsImpl::Init( SalFrame* pFrame )
{
    mpFrame = pFrame;
}

void OpenGLSalGraphicsImpl::Init(SalVirtualDevice* pVDev)
{
    mpVDev = pVDev;
}

bool OpenGLSalGraphicsImpl::setClipRegion( const vcl::Region& rClip )
{
    const basegfx::B2DPolyPolygon aClip( rClip.GetAsB2DPolyPolygon() );

    /*glEnable(GL_STENCIL_TEST);

    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glDepthMask( GL_FALSE );
    glStencilMask( 0xFF );
    glStencilFunc( GL_NEVER, 1, 0xFF );
    glStencilOp( GL_REPLACE, GL_KEEP, GL_KEEP );

    glClear( GL_STENCIL_BUFFER_BIT );
    DrawPolyPolygon( aClip );

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask( GL_TRUE );
    glStencilMask( 0x00 );
    glStencilFunc(GL_EQUAL, 1, 0xFF);*/

    return true;
}

// get the depth of the device
sal_uInt16 OpenGLSalGraphicsImpl::GetBitCount() const
{
    return 32;
}

// get the width of the device
long OpenGLSalGraphicsImpl::GetGraphicsWidth() const
{
    return GetWidth();
}

inline GLfloat OpenGLSalGraphicsImpl::GetWidth() const
{
    if( mpFrame )
        return mpFrame->maGeometry.nWidth;
    else if (mpVDev)
    {
        long nWidth = 0;
        long nHeight = 0;
        mpVDev->GetSize(nWidth, nHeight);
        return nWidth;
    }
    return 1;
}

inline GLfloat OpenGLSalGraphicsImpl::GetHeight() const
{
    if( mpFrame )
        return mpFrame->maGeometry.nHeight;
    else if (mpVDev)
    {
        long nWidth = 0;
        long nHeight = 0;
        mpVDev->GetSize(nWidth, nHeight);
        return nHeight;
    }
    return 1;
}

// set the clip region to empty
void OpenGLSalGraphicsImpl::ResetClipRegion()
{
    glDisable(GL_STENCIL_TEST);
}

// set the line color to transparent (= don't draw lines)
void OpenGLSalGraphicsImpl::SetLineColor()
{
    if( mnLineColor != SALCOLOR_NONE )
    {
        mnLineColor = SALCOLOR_NONE;
    }
}

// set the line color to a specific color
void OpenGLSalGraphicsImpl::SetLineColor( SalColor nSalColor )
{
    if( mnLineColor != nSalColor )
    {
        mnLineColor = nSalColor;
    }
}

// set the fill color to transparent (= don't fill)
void OpenGLSalGraphicsImpl::SetFillColor()
{
    if( mnFillColor != SALCOLOR_NONE )
    {
        mnFillColor = SALCOLOR_NONE;
    }
}

// set the fill color to a specific color, shapes will be
// filled accordingly
void OpenGLSalGraphicsImpl::SetFillColor( SalColor nSalColor )
{
    if( mnFillColor != nSalColor )
    {
        mnFillColor = nSalColor;
    }
}

// enable/disable XOR drawing
void OpenGLSalGraphicsImpl::SetXORMode( bool /*bSet*/, bool /*bInvertOnly*/ )
{
}

// set line color for raster operations
void OpenGLSalGraphicsImpl::SetROPLineColor( SalROPColor /*nROPColor*/ )
{
}

// set fill color for raster operations
void OpenGLSalGraphicsImpl::SetROPFillColor( SalROPColor /*nROPColor*/ )
{
}

bool OpenGLSalGraphicsImpl::CreateSolidProgram( void )
{
    mnSolidProgram = OpenGLHelper::LoadShaders( "solidVertexShader", "solidFragmentShader" );
    if( mnSolidProgram == 0 )
        return false;

    SAL_INFO( "vcl.opengl", "Solid Program Created" );
    glBindAttribLocation( mnSolidProgram, GL_ATTRIB_POS, "position" );
    mnColorUniform = glGetUniformLocation( mnSolidProgram, "color" );
    return true;
}

bool OpenGLSalGraphicsImpl::CreateTextureProgram( void )
{
    mnTextureProgram = OpenGLHelper::LoadShaders( "textureVertexShader", "textureFragmentShader" );
    if( mnTextureProgram == 0 )
        return false;

    glBindAttribLocation( mnTextureProgram, GL_ATTRIB_POS, "position" );
    glBindAttribLocation( mnTextureProgram, GL_ATTRIB_TEX, "tex_coord_in" );
    mnSamplerUniform = glGetUniformLocation( mnTextureProgram, "sampler" );
    return true;
}

bool OpenGLSalGraphicsImpl::CreateMaskedTextureProgram( void )
{
    mnMaskedTextureProgram = OpenGLHelper::LoadShaders( "maskedTextureVertexShader", "maskedTextureFragmentShader" );
    if( mnMaskedTextureProgram == 0 )
        return false;

    glBindAttribLocation( mnTextureProgram, GL_ATTRIB_POS, "position" );
    glBindAttribLocation( mnTextureProgram, GL_ATTRIB_TEX, "tex_coord_in" );
    mnMaskedSamplerUniform = glGetUniformLocation( mnMaskedTextureProgram, "sampler" );
    mnMaskSamplerUniform = glGetUniformLocation( mnMaskedTextureProgram, "mask" );
    return true;
}

bool OpenGLSalGraphicsImpl::CreateMaskProgram( void )
{
    mnMaskedTextureProgram = OpenGLHelper::LoadShaders( "maskVertexShader", "maskFragmentShader" );
    if( mnMaskedTextureProgram == 0 )
        return false;

    glBindAttribLocation( mnTextureProgram, GL_ATTRIB_POS, "position" );
    glBindAttribLocation( mnTextureProgram, GL_ATTRIB_TEX, "tex_coord_in" );
    mnMaskUniform = glGetUniformLocation( mnMaskProgram, "sampler" );
    mnMaskColorUniform = glGetUniformLocation( mnMaskProgram, "mask" );
    return true;
}

void OpenGLSalGraphicsImpl::BeginSolid( SalColor nColor, sal_uInt8 nTransparency )
{
    if( mnSolidProgram == 0 )
    {
        glClearColor( 1, 1, 1, 1 );
        glClear( GL_COLOR_BUFFER_BIT );
        if( !CreateSolidProgram() )
            return;
    }

    glUseProgram( mnSolidProgram );
    glUniformColor( mnColorUniform, nColor, nTransparency );
}

void OpenGLSalGraphicsImpl::BeginSolid( SalColor nColor )
{
    BeginSolid( nColor, 0 );
}

void OpenGLSalGraphicsImpl::EndSolid( void )
{
    glUseProgram( 0 );
}

void OpenGLSalGraphicsImpl::BeginInvert( void )
{
    glEnable( GL_BLEND );
    glBlendFunc( GL_ONE_MINUS_DST_COLOR, GL_ZERO );
    BeginSolid( MAKE_SALCOLOR( 255, 255, 255 ) );
}

void OpenGLSalGraphicsImpl::EndInvert( void )
{
    EndSolid();
    glDisable( GL_BLEND );
}

void OpenGLSalGraphicsImpl::DrawPoint( long nX, long nY )
{
    GLushort pPoint[2];

    pPoint[0] = nX;
    pPoint[1] = GetHeight() - nY;

    glEnableVertexAttribArray( GL_ATTRIB_POS );
    glVertexAttribPointer( GL_ATTRIB_POS, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, pPoint );
    glDrawArrays( GL_POINTS, 0, 1 );
    glDisableVertexAttribArray( GL_ATTRIB_POS );
}

void OpenGLSalGraphicsImpl::DrawLine( long nX1, long nY1, long nX2, long nY2 )
{
    GLfloat pPoints[4];

    pPoints[0] = (2 * nX1) / GetWidth() - 1.0;
    pPoints[1] = (2 * (GetHeight() - nY1)) / GetHeight() - 1.0;
    pPoints[2] = (2 * nX2) / GetWidth() - 1.0;;
    pPoints[3] = (2 * (GetHeight() - nY2)) / GetHeight() - 1.0;

    glEnableVertexAttribArray( GL_ATTRIB_POS );
    glVertexAttribPointer( GL_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, pPoints );
    glDrawArrays( GL_LINES, 0, 2 );
    glDisableVertexAttribArray( GL_ATTRIB_POS );
}

void OpenGLSalGraphicsImpl::DrawLines( sal_uInt32 nPoints, const SalPoint* pPtAry, bool bClose )
{
    std::vector<GLfloat> aPoints(nPoints * 2);
    sal_uInt32 i, j;

    for( i = 0, j = 0; i < nPoints; i++ )
    {
        aPoints[j++] = (2 * pPtAry[i].mnX) / GetWidth()  - 1.0;
        aPoints[j++] = (2 * pPtAry[i].mnY) / GetHeight() - 1.0;
    }

    glEnableVertexAttribArray( GL_ATTRIB_POS );
    glVertexAttribPointer( GL_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, &aPoints[0] );
    if( bClose )
        glDrawArrays( GL_LINE_LOOP, 0, nPoints );
    else
        glDrawArrays( GL_LINE_STRIP, 0, nPoints );
    glDisableVertexAttribArray( GL_ATTRIB_POS );
}

void OpenGLSalGraphicsImpl::DrawConvexPolygon( sal_uInt32 nPoints, const SalPoint* pPtAry )
{
    std::vector<GLfloat> aVertices(nPoints * 2);
    sal_uInt32 i, j;

    for( i = 0, j = 0; i < nPoints; i++, j += 2 )
    {
        aVertices[j] = (2 * pPtAry[i].mnX) / GetWidth() - 1.0;
        aVertices[j+1] = (2 * pPtAry[i].mnY) / GetHeight() - 1.0;
    }

    glEnableVertexAttribArray( GL_ATTRIB_POS );
    glVertexAttribPointer( GL_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, &aVertices[0] );
    glDrawArrays( GL_TRIANGLE_FAN, 0, nPoints );
    glDisableVertexAttribArray( GL_ATTRIB_POS );
}

void OpenGLSalGraphicsImpl::DrawRect( long nX, long nY, long nWidth, long nHeight )
{
    long nX1( nX );
    long nY1( GetHeight() - nY );
    long nX2( nX + nWidth );
    long nY2( GetHeight() - nY - nHeight );
    const SalPoint aPoints[] = { { nX1, nY2 }, { nX1, nY1 },
                                 { nX2, nY1 }, { nX2, nY2 }};

    DrawConvexPolygon( 4, aPoints );
}

void OpenGLSalGraphicsImpl::DrawPolygon( sal_uInt32 nPoints, const SalPoint* pPtAry )
{
    ::basegfx::B2DPolygon aPolygon;

    for( sal_uInt32 i = 0; i < nPoints; i++ )
        aPolygon.append( ::basegfx::B2DPoint( pPtAry[i].mnX, pPtAry[i].mnY ) );
    aPolygon.setClosed( true );

    if( ::basegfx::tools::isConvex( aPolygon ) )
    {
        if( nPoints > 2L )
        {
            DrawConvexPolygon( nPoints, pPtAry );
        }
    }
    else
    {
        const ::basegfx::B2DPolygon& aResult(
            ::basegfx::triangulator::triangulate( aPolygon ) );
        std::vector<GLfloat> aVertices(aResult.count() * 2);
        sal_uInt32 j( 0 );

        float nHeight = GetHeight();
        float nWidth = GetWidth();
        for( sal_uInt32 i = 0; i < aResult.count(); i++ )
        {
            const ::basegfx::B2DPoint& rPt( aResult.getB2DPoint(i) );
            aVertices[j++] = 2 * rPt.getX() / nWidth - 1.0f;
            aVertices[j++] = 1.0f - 2 * rPt.getY() / nHeight;
        }

        glEnableVertexAttribArray( GL_ATTRIB_POS );
        glVertexAttribPointer( GL_ATTRIB_POS, 2, GL_FLOAT, GL_FALSE, 0, &aVertices[0] );
        glDrawArrays( GL_TRIANGLES, 0, aResult.count() );
        glDisableVertexAttribArray( GL_ATTRIB_POS );
    }
}

void OpenGLSalGraphicsImpl::DrawPolyPolygon( const basegfx::B2DPolyPolygon& pPolyPolygon )
{
    sal_uInt32 i, j;
    ::std::vector< GLushort > pVertices;

    for( i = 0; i < pPolyPolygon.count(); i++ )
    {
        const basegfx::B2DPolygon& pPolygon( pPolyPolygon.getB2DPolygon( i ) );
        const ::basegfx::B2DPolygon& aResult(
            ::basegfx::triangulator::triangulate( pPolygon ) );

        for( j = 0; j < aResult.count(); j++ )
        {
            const ::basegfx::B2DPoint& rPt( aResult.getB2DPoint( j ) );
            pVertices.push_back( rPt.getX() );
            pVertices.push_back( rPt.getY() );
        }
    }

    glEnableVertexAttribArray( GL_ATTRIB_POS );
    glVertexAttribPointer( GL_ATTRIB_POS, 2, GL_UNSIGNED_SHORT, GL_FALSE, 0, pVertices.data() );
    glDrawArrays( GL_TRIANGLES, 0, pVertices.size() / 2 );
    glDisableVertexAttribArray( GL_ATTRIB_POS );
}

void OpenGLSalGraphicsImpl::DrawTextureRect( const Size& rSize, const SalTwoRect& rPosAry )
{
    GLfloat aTexCoord[8];

    aTexCoord[0] = aTexCoord[2] = rPosAry.mnSrcX / (double) rSize.Width();
    aTexCoord[4] = aTexCoord[6] = (rPosAry.mnSrcX + rPosAry.mnSrcWidth) / (double) rSize.Width();
    aTexCoord[3] = aTexCoord[5] = (rSize.Height() - rPosAry.mnSrcY) / (double) rSize.Height();
    aTexCoord[1] = aTexCoord[7] = (rSize.Height() - rPosAry.mnSrcY - rPosAry.mnSrcHeight) / (double) rSize.Height();

    glEnableVertexAttribArray( GL_ATTRIB_TEX );
    glVertexAttribPointer( GL_ATTRIB_TEX, 2, GL_FLOAT, GL_FALSE, 0, aTexCoord );

    DrawRect( rPosAry.mnDestX, rPosAry.mnDestY, rPosAry.mnDestWidth, rPosAry.mnDestHeight );

    glDisableVertexAttribArray( GL_ATTRIB_TEX );
}

void OpenGLSalGraphicsImpl::DrawTexture( GLuint nTexture, const Size& rSize, const SalTwoRect& pPosAry )
{
    if( mnTextureProgram == 0 )
    {
        if( !CreateTextureProgram() )
            return;
    }

    glUseProgram( mnTextureProgram );
    glUniform1i( mnSamplerUniform, 0 );
    glActiveTexture( GL_TEXTURE0 );
    CHECK_GL_ERROR();
    glBindTexture( GL_TEXTURE_2D, nTexture );

    DrawTextureRect( rSize, pPosAry );
    CHECK_GL_ERROR();

    glBindTexture( GL_TEXTURE_2D, 0 );
    glUseProgram( 0 );
}

void OpenGLSalGraphicsImpl::DrawTextureWithMask( GLuint nTexture, GLuint nMask, const SalTwoRect& /*pPosAry*/ )
{
    if( mnMaskedTextureProgram == 0 )
    {
        if( !CreateMaskedTextureProgram() )
            return;
    }

    glUseProgram( mnMaskedTextureProgram );
    glUniform1i( mnMaskedSamplerUniform, 0 );
    glUniform1i( mnMaskSamplerUniform, 1 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, nTexture );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, nMask );

    //DrawTextureRect( pPosAry );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    glUseProgram( 0 );
}

void OpenGLSalGraphicsImpl::DrawMask( GLuint nMask, SalColor nMaskColor, const SalTwoRect& /*pPosAry*/ )
{
    if( mnMaskProgram == 0 )
    {
        if( !CreateMaskProgram() )
            return;
    }

    glUseProgram( mnMaskProgram );
    glUniformColor( mnMaskColorUniform, nMaskColor, 0 );
    glUniform1i( mnMaskUniform, 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, nMask );

    //DrawTextureRect( pPosAry );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, 0 );
    glUseProgram( 0 );
}


// draw --> LineColor and FillColor and RasterOp and ClipRegion
void OpenGLSalGraphicsImpl::drawPixel( long nX, long nY )
{
    SAL_INFO( "vcl.opengl", "::drawPixel" );
    if( mnLineColor != SALCOLOR_NONE )
    {
        maContext.makeCurrent();
        BeginSolid( mnLineColor );
        DrawPoint( nX, nY );
        EndSolid();
    }
}

void OpenGLSalGraphicsImpl::drawPixel( long nX, long nY, SalColor nSalColor )
{
    SAL_INFO( "vcl.opengl", "::drawPixel" );
    if( nSalColor != SALCOLOR_NONE )
    {
        maContext.makeCurrent();
        BeginSolid( nSalColor );
        DrawPoint( nX, nY );
        EndSolid();
    }
}

void OpenGLSalGraphicsImpl::drawLine( long nX1, long nY1, long nX2, long nY2 )
{
    SAL_INFO( "vcl.opengl", "::drawLine" );
    if( mnLineColor != SALCOLOR_NONE )
    {
        maContext.makeCurrent();
        BeginSolid( mnLineColor );
        DrawLine( nX1, nY1, nX2, nY2 );
        EndSolid();
    }
}

void OpenGLSalGraphicsImpl::drawRect( long nX, long nY, long nWidth, long nHeight )
{
    SAL_INFO( "vcl.opengl", "::drawRect" );
    maContext.makeCurrent();
    glViewport( 0, 0, GetWidth(), GetHeight() );

    if( mnFillColor != SALCOLOR_NONE )
    {
        BeginSolid( mnFillColor );
        DrawRect( nX, nY, nWidth, nHeight );
        EndSolid();
    }

    if( mnLineColor != SALCOLOR_NONE )
    {
        const long nX1( nX );
        const long nY1( nY );
        const long nX2( nX + nWidth - 1 );
        const long nY2( nY + nHeight - 1 );
        const SalPoint aPoints[] = { { nX1, nY1 }, { nX2, nY1 },
                                     { nX2, nY2 }, { nX1, nY2 } };

        BeginSolid( mnLineColor );
        DrawLines( 4, aPoints, true );
        EndSolid();
    }
}

void OpenGLSalGraphicsImpl::drawPolyLine( sal_uInt32 nPoints, const SalPoint* pPtAry )
{
    SAL_INFO( "vcl.opengl", "::drawPolyLine" );
    maContext.makeCurrent();

    if( mnLineColor != SALCOLOR_NONE && nPoints > 1 )
    {
        BeginSolid( mnLineColor );
        DrawLines( nPoints, pPtAry, false );
        EndSolid();
    }
}

void OpenGLSalGraphicsImpl::drawPolygon( sal_uInt32 nPoints, const SalPoint* pPtAry )
{
    SAL_INFO( "vcl.opengl", "::drawPolygon" );
    if( nPoints == 0 )
        return;
    if( nPoints == 1 )
    {
        drawPixel( pPtAry[0].mnX, pPtAry[0].mnY );
        return;
    }
    if( nPoints == 2 )
    {
        drawLine( pPtAry[0].mnX, pPtAry[0].mnY,
                  pPtAry[1].mnX, pPtAry[1].mnY );
        return;
    }

    maContext.makeCurrent();

    if( mnFillColor != SALCOLOR_NONE )
    {
        BeginSolid( mnFillColor );
        DrawPolygon( nPoints, pPtAry );
        EndSolid();
    }

    if( mnLineColor != SALCOLOR_NONE )
    {
        BeginSolid( mnLineColor );
        DrawLines( nPoints, pPtAry, true );
        EndSolid();
    }
}

void OpenGLSalGraphicsImpl::drawPolyPolygon( sal_uInt32 nPoly, const sal_uInt32* pPoints, PCONSTSALPOINT* pPtAry )
{
    SAL_INFO( "vcl.opengl", "::drawPolyPolygon" );
    if( nPoly <= 0 )
        return;

    maContext.makeCurrent();

    if( mnFillColor != SALCOLOR_NONE )
    {
        BeginSolid( mnFillColor );
        for( sal_uInt32 i = 0; i < nPoly; i++ )
            DrawPolygon( pPoints[i], pPtAry[i] );
        EndSolid();
    }

    if( mnLineColor != SALCOLOR_NONE )
    {
        // TODO Use glMultiDrawElements or primitive restart
        BeginSolid( mnLineColor );
        for( sal_uInt32 i = 0; i < nPoly; i++ )
            DrawLines( pPoints[i], pPtAry[i], true );
        EndSolid();
    }
}

bool OpenGLSalGraphicsImpl::drawPolyPolygon( const ::basegfx::B2DPolyPolygon&, double /*fTransparency*/ )
{
    return false;
}

bool OpenGLSalGraphicsImpl::drawPolyLine(
            const ::basegfx::B2DPolygon&,
            double /*fTransparency*/,
            const ::basegfx::B2DVector& /*rLineWidths*/,
            basegfx::B2DLineJoin,
            com::sun::star::drawing::LineCap)
{
    return false;
}

bool OpenGLSalGraphicsImpl::drawPolyLineBezier(
            sal_uInt32 /*nPoints*/,
            const SalPoint* /*pPtAry*/,
            const sal_uInt8* /*pFlgAry*/ )
{
    return false;
}

bool OpenGLSalGraphicsImpl::drawPolygonBezier(
            sal_uInt32 /*nPoints*/,
            const SalPoint* /*pPtAry*/,
            const sal_uInt8* /*pFlgAry*/ )
{
    return false;
}

bool OpenGLSalGraphicsImpl::drawPolyPolygonBezier(
            sal_uInt32 /*nPoly*/,
            const sal_uInt32* /*pPoints*/,
            const SalPoint* const* /*pPtAry*/,
            const sal_uInt8* const* /*pFlgAry*/ )
{
    return NULL;
}

// CopyArea --> No RasterOp, but ClipRegion
void OpenGLSalGraphicsImpl::copyArea(
            long /*nDestX*/, long /*nDestY*/,
            long /*nSrcX*/, long /*nSrcY*/,
            long /*nSrcWidth*/, long /*nSrcHeight*/,
            sal_uInt16 /*nFlags*/ )
{
    SAL_INFO( "vcl.opengl", "::copyArea" );
}

// CopyBits and DrawBitmap --> RasterOp and ClipRegion
// CopyBits() --> pSrcGraphics == NULL, then CopyBits on same Graphics
void OpenGLSalGraphicsImpl::copyBits( const SalTwoRect& rPosAry, SalGraphics* /*pSrcGraphics*/ )
{
    // TODO Check if SalGraphicsImpl is the same
    const bool bSameGraphics( false );

    SAL_INFO( "vcl.opengl", "::copyBits" );
    if( bSameGraphics &&
        (rPosAry.mnSrcWidth == rPosAry.mnDestWidth) &&
        (rPosAry.mnSrcHeight == rPosAry.mnDestHeight))
    {
        // short circuit if there is nothing to do
        if( (rPosAry.mnSrcX == rPosAry.mnDestX) &&
            (rPosAry.mnSrcY == rPosAry.mnDestY))
            return;
        // use copyArea() if source and destination context are identical
        copyArea( rPosAry.mnDestX, rPosAry.mnDestY, rPosAry.mnSrcX, rPosAry.mnSrcY,
            rPosAry.mnSrcWidth, rPosAry.mnSrcHeight, 0 );
        return;
    }

    // TODO Copy from one FBO to the other (glBlitFramebuffer)
}

void OpenGLSalGraphicsImpl::drawBitmap( const SalTwoRect& rPosAry, const SalBitmap& rSalBitmap )
{
    const OpenGLSalBitmap& rBitmap = static_cast<const OpenGLSalBitmap&>(rSalBitmap);
    GLuint nTexture = rBitmap.GetTexture( maContext );
    const Size aSize = rSalBitmap.GetSize();

    SAL_INFO( "vcl.opengl", "::drawBitmap" );
    maContext.makeCurrent();
    glViewport( 0, 0, GetWidth(), GetHeight() );
    DrawTexture( nTexture, aSize, rPosAry );
}

void OpenGLSalGraphicsImpl::drawBitmap(
            const SalTwoRect& /*rPosAry*/,
            const SalBitmap& /*rSalBitmap*/,
            SalColor /*nTransparentColor*/ )
{
    OSL_FAIL( "::DrawBitmap with transparent color not supported" );
}

void OpenGLSalGraphicsImpl::drawBitmap(
            const SalTwoRect& rPosAry,
            const SalBitmap& rSalBitmap,
            const SalBitmap& rMaskBitmap )
{
    const OpenGLSalBitmap& rBitmap = static_cast<const OpenGLSalBitmap&>(rSalBitmap);
    const OpenGLSalBitmap& rMask = static_cast<const OpenGLSalBitmap&>(rMaskBitmap);
    const GLuint nTexture( rBitmap.GetTexture( maContext ) );
    const GLuint nMask( rMask.GetTexture( maContext ) );

    SAL_INFO( "vcl.opengl", "::drawBitmap" );
    maContext.makeCurrent();
    DrawTextureWithMask( nTexture, nMask, rPosAry );
}

void OpenGLSalGraphicsImpl::drawMask(
            const SalTwoRect& rPosAry,
            const SalBitmap& rSalBitmap,
            SalColor nMaskColor )
{
    const OpenGLSalBitmap& rBitmap = static_cast<const OpenGLSalBitmap&>(rSalBitmap);
    const GLuint nTexture( rBitmap.GetTexture( maContext ) );

    SAL_INFO( "vcl.opengl", "::drawMask" );
    maContext.makeCurrent();
    DrawMask( nTexture, nMaskColor, rPosAry );
}

SalBitmap* OpenGLSalGraphicsImpl::getBitmap( long nX, long nY, long nWidth, long nHeight )
{
    OpenGLSalBitmap* pBitmap = new OpenGLSalBitmap;
    SAL_INFO( "vcl.opengl", "::getBitmap " << nX << "," << nY <<
              " " << nWidth << "x" << nHeight );
    if( !pBitmap->Create( maContext, nX, nY, nWidth, nHeight ) )
    {
        delete pBitmap;
        pBitmap = NULL;
    }
    return pBitmap;
}

SalColor OpenGLSalGraphicsImpl::getPixel( long nX, long nY )
{
    char pixel[3];

    maContext.makeCurrent();
    glReadPixels( nX, nY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    return MAKE_SALCOLOR( pixel[0], pixel[1], pixel[2] );
}

// invert --> ClipRegion (only Windows or VirDevs)
void OpenGLSalGraphicsImpl::invert(
            long nX, long nY,
            long nWidth, long nHeight,
            SalInvert nFlags)
{
    // TODO Figure out what are those:
    //   * SAL_INVERT_50 (50/50 pattern?)
    //   * SAL_INVERT_TRACKFRAME (dash-line rectangle?)

    maContext.makeCurrent();

    if( nFlags & SAL_INVERT_TRACKFRAME )
    {

    }
    else if( nFlags & SAL_INVERT_50 )
    {

    }
    else // just invert
    {
        BeginInvert();
        DrawRect( nX, nY, nWidth, nHeight );
        EndInvert();
    }
}

void OpenGLSalGraphicsImpl::invert( sal_uInt32 nPoints, const SalPoint* pPtAry, SalInvert nFlags )
{
    maContext.makeCurrent();

    if( nFlags & SAL_INVERT_TRACKFRAME )
    {

    }
    else if( nFlags & SAL_INVERT_50 )
    {

    }
    else // just invert
    {
        BeginInvert();
        DrawPolygon( nPoints, pPtAry );
        EndInvert();
    }
}

bool OpenGLSalGraphicsImpl::drawEPS(
            long /*nX*/, long /*nY*/,
            long /*nWidth*/, long /*nHeight*/,
            void* /*pPtr*/,
            sal_uLong /*nSize*/ )
{
    return false;
}

/** Render bitmap with alpha channel

    @param rSourceBitmap
    Source bitmap to blit

    @param rAlphaBitmap
    Alpha channel to use for blitting

    @return true, if the operation succeeded, and false
    otherwise. In this case, clients should try to emulate alpha
    compositing themselves
 */
bool OpenGLSalGraphicsImpl::drawAlphaBitmap(
            const SalTwoRect& rPosAry,
            const SalBitmap& rSalBitmap,
            const SalBitmap& rAlphaBitmap )
{
    const OpenGLSalBitmap& rBitmap = static_cast<const OpenGLSalBitmap&>(rSalBitmap);
    const OpenGLSalBitmap& rAlpha = static_cast<const OpenGLSalBitmap&>(rAlphaBitmap);
    const GLuint nTexture( rBitmap.GetTexture( maContext ) );
    const GLuint nAlpha( rAlpha.GetTexture( maContext ) );

    maContext.makeCurrent();
    DrawTextureWithMask( nTexture, nAlpha, rPosAry );
    return true;
}

/** draw transformed bitmap (maybe with alpha) where Null, X, Y define the coordinate system */
bool OpenGLSalGraphicsImpl::drawTransformedBitmap(
            const basegfx::B2DPoint& /*rNull*/,
            const basegfx::B2DPoint& /*rX*/,
            const basegfx::B2DPoint& /*rY*/,
            const SalBitmap& /*rSourceBitmap*/,
            const SalBitmap* /*pAlphaBitmap*/)
{
    return false;
}

/** Render solid rectangle with given transparency

    @param nTransparency
    Transparency value (0-255) to use. 0 blits and opaque, 255 a
    fully transparent rectangle
 */
bool OpenGLSalGraphicsImpl::drawAlphaRect(
                long nX, long nY,
                long nWidth, long nHeight,
                sal_uInt8 nTransparency )
{
    if( mnFillColor != SALCOLOR_NONE && nTransparency < 100 )
    {
        BeginSolid( mnFillColor, nTransparency );
        DrawRect( nX, nY, nWidth, nHeight );
        EndSolid();
    }

    return true;
}

bool OpenGLSalGraphicsImpl::drawGradient(const tools::PolyPolygon& /*rPolygon*/,
        const Gradient& /*rGradient*/)
{
    return false;
}

bool OpenGLSalGraphicsImpl::swapBuffers()
{
    maContext.swapBuffers();
    return true;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
