/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <rtl/bootstrap.hxx>
#include <comphelper/processfactory.hxx>
#include <cppuhelper/bootstrap.hxx>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/registry/XSimpleRegistry.hpp>
#include <com/sun/star/ucb/UniversalContentBroker.hpp>

#include <vcl/vclmain.hxx>

#include <tools/urlobj.hxx>
#include <tools/stream.hxx>
#include <vcl/svapp.hxx>
#include <vcl/pngread.hxx>
#include <vcl/wrkwin.hxx>
#include <vcl/graphicfilter.hxx>

#if 0
#  define FIXME_ALPHA_WORKING
#  define FIXME_ROUNDED_RECT_WORKING
#  define FIXME_DRAW_TRANSPARENT_WORKING
#endif

using namespace css;

class DemoWin : public WorkWindow
{
    Bitmap   maIntroBW;
    BitmapEx maIntro;

public:
    DemoWin() : WorkWindow( NULL, WB_APP | WB_STDWORK)
    {
        // Needed to find images
        OUString aPath;
        rtl::Bootstrap::get("SYSBINDIR", aPath);
#ifdef FIXME_THIS_FAILS
        rtl::Bootstrap::set("BRAND_BASE_DIR", aPath + "/..");
        if (Application::LoadBrandBitmap("intro", maIntro))
            Application::Abort("Failed to load intro image");
#else
        aPath = aPath + "/intro.png";
        SvFileStream aFileStream( aPath, STREAM_READ );
        GraphicFilter aGraphicFilter(false);
        Graphic aGraphic;
        if (aGraphicFilter.ImportGraphic(aGraphic, aPath, aFileStream) != 0)
            Application::Abort("Failed to load intro image: " + aPath);
        maIntro = aGraphic.GetBitmapEx();
#endif
        maIntroBW = maIntro.GetBitmap();
        maIntroBW.Filter( BMP_FILTER_EMBOSS_GREY );
    }

    virtual void Paint( const Rectangle& rRect ) SAL_OVERRIDE;

    std::vector<Rectangle> partitionAndClear(int nX, int nY);

    void drawBackground()
    {
        Rectangle r(Point(0,0), GetSizePixel());
        Gradient aGradient;
        aGradient.SetStartColor(COL_BLUE);
        aGradient.SetEndColor(COL_GREEN);
        aGradient.SetStyle(GradientStyle_LINEAR);
//        aGradient.SetBorder(r.GetSize().Width()/20);
        DrawGradient(r, aGradient);
    }

    void drawRadialLines(Rectangle r)
    {
        SetFillColor(Color(COL_LIGHTRED));
        SetLineColor(Color(COL_BLACK));
        DrawRect( r );

        // FIXME: notice these appear reflected at the bottom not the top.
        for(int i=0; i<r.GetHeight(); i+=15)
            DrawLine( Point(r.Left(), r.Top()+i), Point(r.Right(), r.Bottom()-i) );
        for(int i=0; i<r.GetWidth(); i+=15)
            DrawLine( Point(r.Left()+i, r.Bottom()), Point(r.Right()-i, r.Top()) );

        // Should draw a white-line across the middle
        Color aLastPixel( COL_WHITE );
        Point aCenter((r.Left() + r.Right())/2 - 4,
                      (r.Top() + r.Bottom())/2 - 4);
        for(int i=0; i<8; i++)
        {
            DrawPixel(aCenter, aLastPixel);
            aLastPixel = GetPixel(aCenter);
            aCenter.Move(1,1);
        }
    }

    void drawText(Rectangle r)
    {
        SetTextColor( Color( COL_BLACK ) );
        vcl::Font aFont( OUString( "Times" ), Size( 0, 25 ) );
        SetFont( aFont );
        DrawText( r, OUString( "Just a simple text" ) );
    }

    void drawPoly(Rectangle r) // pretty
    {
        drawCheckered(r);

        long nDx = r.GetWidth()/20;
        long nDy = r.GetHeight()/20;
        Rectangle aShrunk(r);
        aShrunk.Move(nDx, nDy);
        aShrunk.SetSize(Size(r.GetWidth()-nDx*2,
                             r.GetHeight()-nDy*2));
        Polygon aPoly(aShrunk);
        tools::PolyPolygon aPPoly(aPoly);
        SetLineColor(Color(COL_RED));
        SetFillColor(Color(COL_RED));
        // This hits the optional 'drawPolyPolygon' code-path
        DrawTransparent(aPPoly, 64);
    }
    void drawEllipse(Rectangle r)
    {
        SetLineColor(Color(COL_RED));
        SetFillColor(Color(COL_GREEN));
        DrawEllipse(r);
    }
    void drawCheckered(Rectangle r)
    {
        DrawCheckered(r.TopLeft(), r.GetSize());
    }
    void drawGradient(Rectangle r)
    {
        Gradient aGradient;
        aGradient.SetStartColor(COL_YELLOW);
        aGradient.SetEndColor(COL_RED);
//        aGradient.SetAngle(45);
        aGradient.SetStyle(GradientStyle_RECT);
        aGradient.SetBorder(r.GetSize().Width()/20);
        DrawGradient(r, aGradient);
    }
    void drawBitmap(Rectangle r)
    {
        Bitmap aBitmap(maIntroBW);
        aBitmap.Scale(r.GetSize(), BMP_SCALE_BESTQUALITY);
        DrawBitmap(r.TopLeft(), aBitmap);
    }
    void drawBitmapEx(Rectangle r)
    {
        drawCheckered(r);

        BitmapEx aBitmap(maIntro);
        aBitmap.Scale(r.GetSize(), BMP_SCALE_BESTQUALITY);
#ifdef FIXME_ALPHA_WORKING
        AlphaMask aSemiTransp(aBitmap.GetSizePixel());
        aSemiTransp.Erase(64);
        DrawBitmapEx(r.TopLeft(), BitmapEx(aBitmap.GetBitmap(),
                                           aSemiTransp));
#else
        DrawBitmapEx(r.TopLeft(), aBitmap);
#endif
    }
    void drawPolyPolgons(Rectangle r)
    {
        struct {
            double nX, nY;
        } aPoints[] = { { 0.1, 0.1 }, { 0.9, 0.9 },
                        { 0.9, 0.1 }, { 0.1, 0.9 },
                        { 0.1, 0.1 } };

        tools::PolyPolygon aPolyPoly;
        // Render 4x polygons & aggregate into another PolyPolygon
        for (int x = 0; x < 2; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                Rectangle aSubRect(r);
                aSubRect.Move(x * r.GetWidth()/3, y * r.GetHeight()/3);
                aSubRect.SetSize(Size(r.GetWidth()/2, r.GetHeight()/4));
                Polygon aPoly(SAL_N_ELEMENTS(aPoints));
                for (size_t v = 0; v < SAL_N_ELEMENTS(aPoints); v++)
                {
                    aPoly.SetPoint(Point(aSubRect.Left() +
                                         aSubRect.GetWidth() * aPoints[v].nX,
                                         aSubRect.Top() +
                                         aSubRect.GetHeight() * aPoints[v].nY),
                                   v);
                }
                SetLineColor(Color(COL_YELLOW));
                SetFillColor(Color(COL_BLACK));
                DrawPolygon(aPoly);

                // now move and add to the polypolygon
                aPoly.Move(0, r.GetHeight()/2);
                aPolyPoly.Insert(aPoly);
            }
        }
        SetLineColor(Color(COL_LIGHTRED));
        SetFillColor(Color(COL_GREEN));
#ifdef FIXME_DRAW_TRANSPARENT_WORKING
        DrawTransparent(aPolyPoly, 50);
#else
        DrawPolyPolygon(aPolyPoly);
#endif
    }
    void fetchDrawBitmap(Rectangle r)
    {
        // FIXME: should work ...
        Bitmap aBitmap(GetBitmap(Point(0,0),GetSizePixel()));
        aBitmap.Scale(r.GetSize(), BMP_SCALE_BESTQUALITY);
        DrawBitmap(r.TopLeft(), aBitmap);
    }
};

std::vector<Rectangle> DemoWin::partitionAndClear(int nX, int nY)
{
    Rectangle r;
    std::vector<Rectangle> aRegions;

    // Make small cleared area for these guys
    Size aSize(GetSizePixel());
    long nBorderSize = aSize.Width() / 32;
    long nBoxWidth = (aSize.Width() - nBorderSize*(nX+1)) / nX;
    long nBoxHeight = (aSize.Height() - nBorderSize*(nY+1)) / nY;
    for (int y = 0; y < nY; y++ )
    {
        for (int x = 0; x < nX; x++ )
        {
            r.SetPos(Point(nBorderSize + (nBorderSize + nBoxWidth) * x,
                           nBorderSize + (nBorderSize + nBoxHeight) * y));
            r.SetSize(Size(nBoxWidth, nBoxHeight));

            // knock up a nice little border
            SetLineColor(COL_GRAY);
            SetFillColor(COL_LIGHTGRAY);
            if ((x + y) % 2)
                DrawRect(r);
            else
            {
#ifdef FIXME_ROUNDED_RECT_WORKING
                DrawRect(r, nBorderSize, nBorderSize);
#else
                DrawRect(r);
#endif
            }

            aRegions.push_back(r);
        }
    }

    return aRegions;
}

void DemoWin::Paint( const Rectangle& rRect )
{
    fprintf(stderr, "DemoWin::Paint(%ld,%ld,%ld,%ld)\n", rRect.getX(), rRect.getY(), rRect.getWidth(), rRect.getHeight());

    drawBackground();

    std::vector<Rectangle> aRegions(partitionAndClear(4,3));

    drawRadialLines(aRegions[0]);
    drawText(aRegions[1]);
    drawPoly(aRegions[2]);
    drawEllipse(aRegions[3]);
    drawCheckered(aRegions[4]);
    drawBitmapEx(aRegions[5]);
    drawBitmap(aRegions[6]);
    drawGradient(aRegions[7]);
    drawPolyPolgons(aRegions[8]);
    // last - thumbnail all the above
    fetchDrawBitmap(aRegions[9]);
}

class DemoApp : public Application
{
public:
    DemoApp() {}

    virtual int Main() SAL_OVERRIDE
    {
        DemoWin aMainWin;
        aMainWin.SetText( "Interactive VCL demo" );
        aMainWin.Show();
        Application::Execute();
        return 0;
    }

protected:
    uno::Reference<lang::XMultiServiceFactory> xMSF;
    void Init() SAL_OVERRIDE
    {
        try
        {
            uno::Reference<uno::XComponentContext> xComponentContext
                = ::cppu::defaultBootstrap_InitialComponentContext();
            xMSF = uno::Reference<lang::XMultiServiceFactory>
                ( xComponentContext->getServiceManager(), uno::UNO_QUERY );
            if( !xMSF.is() )
                Application::Abort("Bootstrap failure - no service manager");

            ::comphelper::setProcessServiceFactory( xMSF );
        }
        catch (const uno::Exception &e)
        {
            Application::Abort("Bootstrap exception " + e.Message);
        }
    }
    void DeInit() SAL_OVERRIDE
    {
        uno::Reference< lang::XComponent >(
            comphelper::getProcessComponentContext(),
        uno::UNO_QUERY_THROW )-> dispose();
        ::comphelper::setProcessServiceFactory( NULL );
    }
};

void vclmain::createApplication()
{
    static DemoApp aApp;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
