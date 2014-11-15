/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

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
#include <vcl/virdev.hxx>
#include <vcl/graphicfilter.hxx>
#include <vcl/button.hxx>
#include <vcl/floatwin.hxx>
#include <basegfx/numeric/ftools.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>

#if 0
#  define FIXME_SELF_INTERSECTING_WORKING
#endif

// debugging hook just for us
SAL_DLLPUBLIC css::uno::Sequence< OUString > ImageTree_getAllImageNames();

using namespace css;

class DemoBase :
        public WorkWindow // hide OutputDevice if necessary
{
public:
    DemoBase() : WorkWindow(NULL, WB_APP | WB_STDWORK)
    {
    }
    OutputDevice &getOutDev() { return *this; }
};

enum RenderStyle {
    RENDER_THUMB,    // small view <n> to a page
    RENDER_EXPANDED, // expanded view of this renderer
};

class DemoWin : public DemoBase
{
    Bitmap   maIntroBW;
    BitmapEx maIntro;

    int mnSegmentsX;
    int mnSegmentsY;

    struct RenderContext {
        RenderStyle meStyle;
        bool        mbVDev;
        DemoWin    *mpDemoWin;
    };
    struct RegionRenderer {
    public:
        virtual ~RegionRenderer() {}
        virtual OUString getName() = 0;
        virtual sal_uInt16 getAccelerator() = 0;
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) = 0;
#define RENDER_DETAILS(name,key) \
        virtual OUString getName() SAL_OVERRIDE \
            { return OUString(SAL_STRINGIFY(name)); } \
        virtual sal_uInt16 getAccelerator() SAL_OVERRIDE \
            { return key; }
    };

    std::vector< RegionRenderer * > maRenderers;
    sal_Int32  mnSelectedRenderer;

    void     InitRenderers();

public:
    DemoWin() : DemoBase()
              , mnSegmentsX(4)
              , mnSegmentsY(3)
              , mnSelectedRenderer(-1)
              , mpButton(NULL)
              , mpButtonWin(NULL)
              , mnBounceX(1)
              , mnBounceY(1)
    {
        if (!Application::LoadBrandBitmap("intro", maIntro))
            Application::Abort("Failed to load intro image");

        maIntroBW = maIntro.GetBitmap();
        maIntroBW.Filter(BMP_FILTER_EMBOSS_GREY);

        InitRenderers();
    }

    OUString getRendererList();
    void     selectRenderer(const OUString &rName);

    // Bouncing windows on click ...
    PushButton     *mpButton;
    FloatingWindow *mpButtonWin;
    AutoTimer       maBounce;
    int             mnBounceX, mnBounceY;
    DECL_LINK(BounceTimerCb, void *);

    virtual void MouseButtonDown(const MouseEvent& rMEvt) SAL_OVERRIDE;
    virtual void KeyInput( const KeyEvent& rKEvt ) SAL_OVERRIDE;

    virtual void Paint(const Rectangle& rRect) SAL_OVERRIDE
    {
        fprintf(stderr, "DemoWin::Paint(%ld,%ld,%ld,%ld)\n", rRect.getX(), rRect.getY(), rRect.getWidth(), rRect.getHeight());
        drawToDevice(getOutDev(), false);
    }

    static std::vector<Rectangle> partition(OutputDevice &rDev, int nX, int nY)
    {
        Rectangle r;
        std::vector<Rectangle> aRegions;

        // Make small cleared area for these guys
        Size aSize(rDev.GetOutputSizePixel());
        long nBorderSize = aSize.Width() / 32;
        long nBoxWidth = (aSize.Width() - nBorderSize*(nX+1)) / nX;
        long nBoxHeight = (aSize.Height() - nBorderSize*(nY+1)) / nY;
        for (int y = 0; y < nY; y++)
        {
            for (int x = 0; x < nX; x++)
            {
                r.SetPos(Point(nBorderSize + (nBorderSize + nBoxWidth) * x,
                               nBorderSize + (nBorderSize + nBoxHeight) * y));
                r.SetSize(Size(nBoxWidth, nBoxHeight));
                aRegions.push_back(r);
            }
        }

        return aRegions;
    }

    static void clearRects(OutputDevice &rDev, std::vector<Rectangle> &rRects)
    {
        for (size_t i = 0; i < rRects.size(); i++)
        {
            // knock up a nice little border
            rDev.SetLineColor(COL_GRAY);
            rDev.SetFillColor(COL_LIGHTGRAY);
            if (i % 2)
            {
                int nBorderSize = rRects[i].GetWidth() / 5;
                rDev.DrawRect(rRects[i], nBorderSize, nBorderSize);
            }
            else
                rDev.DrawRect(rRects[i]);
        }
    }

    void drawBackground(OutputDevice &rDev, Rectangle r)
    {
        Gradient aGradient;
        aGradient.SetStartColor(COL_BLUE);
        aGradient.SetEndColor(COL_GREEN);
        aGradient.SetStyle(GradientStyle_LINEAR);
        rDev.DrawGradient(r, aGradient);
    }

    struct DrawLines : public RegionRenderer
    {
        RENDER_DETAILS(lines,KEY_L)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            if (rCtx.meStyle == RENDER_EXPANDED)
            {
                sal_uInt16 nOldAA = rDev.GetAntialiasing();
                rDev.SetAntialiasing(ANTIALIASING_ENABLE_B2DDRAW);

                std::vector<Rectangle> aRegions(DemoWin::partition(rDev, 4, 4));
                DemoWin::clearRects(rDev, aRegions);

#if 0 // FIXME: get this through to the backend ...
                double nTransparency[] = {
                    1.0, 1.0, 1.0, 1.0,
                    0.8, 0.8, 0.8, 0.8,
                    0.5, 0.5, 0.5, 0.5,
                    0.1, 0.1, 0.1, 0.1
                };
#endif
                drawing::LineCap eLineCaps[] = {
                    drawing::LineCap_BUTT, drawing::LineCap_ROUND, drawing::LineCap_SQUARE, drawing::LineCap_BUTT,
                    drawing::LineCap_BUTT, drawing::LineCap_ROUND, drawing::LineCap_SQUARE, drawing::LineCap_BUTT,
                    drawing::LineCap_BUTT, drawing::LineCap_ROUND, drawing::LineCap_SQUARE, drawing::LineCap_BUTT,
                    drawing::LineCap_BUTT, drawing::LineCap_ROUND, drawing::LineCap_SQUARE, drawing::LineCap_BUTT
                };
                ::basegfx::B2DLineJoin eJoins[] = {
                    basegfx::B2DLINEJOIN_NONE,  basegfx::B2DLINEJOIN_MIDDLE, basegfx::B2DLINEJOIN_BEVEL,  basegfx::B2DLINEJOIN_MITER,
                    basegfx::B2DLINEJOIN_ROUND, basegfx::B2DLINEJOIN_NONE,   basegfx::B2DLINEJOIN_MIDDLE, basegfx::B2DLINEJOIN_BEVEL,
                    basegfx::B2DLINEJOIN_MITER, basegfx::B2DLINEJOIN_ROUND,  basegfx::B2DLINEJOIN_NONE,   basegfx::B2DLINEJOIN_MIDDLE,
                    basegfx::B2DLINEJOIN_BEVEL, basegfx::B2DLINEJOIN_MITER,  basegfx::B2DLINEJOIN_ROUND,  basegfx::B2DLINEJOIN_NONE
                };
                double aLineWidths[] = {
                    10.0, 15.0, 20.0, 10.0,
                    10.0, 15.0, 20.0, 10.0,
                    10.0, 15.0, 20.0, 10.0,
                     0.1,  1.0, 10.0, 50.0
                };
                for (size_t i = 0; i < aRegions.size(); i++)
                {
                    static const struct {
                        double nX, nY;
                    } aPoints[] = {
                        { 0.2, 0.2 }, { 0.8, 0.3 }, { 0.7, 0.8 }
                    };
                    rDev.SetLineColor(Color(COL_BLACK));
                    basegfx::B2DPolygon aPoly;
                    Rectangle aSub(aRegions[i]);
                    for (size_t j = 0; j < SAL_N_ELEMENTS(aPoints); j++)
                    {
                        aPoly.append(basegfx::B2DPoint(aSub.Left() + aSub.GetWidth() * aPoints[j].nX,
                                                       aSub.Top()  + aSub.GetHeight() * aPoints[j].nY));
                    }
                    rDev.DrawPolyLine(aPoly, aLineWidths[i], eJoins[i], eLineCaps[i]);

                    // Half of them not-anti-aliased ..
                    if (i > aRegions.size()/2)
                        rDev.SetAntialiasing(nOldAA);
                }
            }
            else
            {
                rDev.SetFillColor(Color(COL_LIGHTRED));
                rDev.SetLineColor(Color(COL_BLACK));
                rDev.DrawRect(r);

                for(int i=0; i<r.GetHeight(); i+=15)
                    rDev.DrawLine(Point(r.Left(), r.Top()+i), Point(r.Right(), r.Bottom()-i));
                for(int i=0; i<r.GetWidth(); i+=15)
                    rDev.DrawLine(Point(r.Left()+i, r.Bottom()), Point(r.Right()-i, r.Top()));

                // Should draw a white-line across the middle
                Color aLastPixel(COL_WHITE);
                Point aCenter((r.Left() + r.Right())/2 - 4,
                              (r.Top() + r.Bottom())/2 - 4);
                for(int i=0; i<8; i++)
                {
                    rDev.DrawPixel(aCenter, aLastPixel);
                    aLastPixel = rDev.GetPixel(aCenter);
                    aCenter.Move(1,1);
                }
            }
        }
    };

    struct DrawText : public RegionRenderer
    {
        RENDER_DETAILS(text,KEY_T)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &) SAL_OVERRIDE
        {
            rDev.SetTextColor(Color(COL_BLACK));
            vcl::Font aFont(OUString("Times"), Size(0, 25));
            rDev.SetFont(aFont);
            rDev.DrawText(r, OUString("Click any rect to zoom"));
        }
    };

    struct DrawCheckered : public RegionRenderer
    {
        RENDER_DETAILS(checks,KEY_C)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            if (rCtx.meStyle == RENDER_EXPANDED)
            {
                std::vector<Rectangle> aRegions(DemoWin::partition(rDev, 2, 2));
                for (size_t i = 0; i < aRegions.size(); i++)
                {
                    vcl::Region aRegion;
                    Rectangle aSub(aRegions[i]);
                    Rectangle aSmaller(aSub);
                    aSmaller.Move(10,10);
                    aSmaller.setWidth(aSmaller.getWidth()-20);
                    aSmaller.setHeight(aSmaller.getHeight()-24);
                    switch (i) {
                    case 0:
                        aRegion = vcl::Region(aSub);
                        break;
                    case 1:
                        aRegion = vcl::Region(aSmaller);
                        aRegion.XOr(aSub);
                        break;
                    case 2:
                    {
                        Polygon aPoly(aSub);
                        aPoly.Rotate(aSub.Center(), 450);
                        aPoly.Clip(aSmaller);
                        aRegion = vcl::Region(aPoly);
                        break;
                    }
                    case 3:
                    {
                        tools::PolyPolygon aPolyPoly;
                        sal_Int32 nTW = aSub.GetWidth()/6;
                        sal_Int32 nTH = aSub.GetHeight()/6;
                        Rectangle aTiny(Point(4, 4), Size(nTW*2, nTH*2));
                        aPolyPoly.Insert(Polygon(aTiny));
                        aTiny.Move(nTW*3, nTH*3);
                        aPolyPoly.Insert(Polygon(aTiny));
                        aTiny.Move(nTW, nTH);
                        aPolyPoly.Insert(Polygon(aTiny));

                        aRegion = vcl::Region(aPolyPoly);
                        break;
                    }
                    } // switch
                    rDev.SetClipRegion(aRegion);
                    rDev.DrawCheckered(aSub.TopLeft(), aSub.GetSize());
                    rDev.SetClipRegion();
                }
            }
            else
            {
                rDev.DrawCheckered(r.TopLeft(), r.GetSize());
            }
        }
    };

    struct DrawPoly : public RegionRenderer
    {
        RENDER_DETAILS(poly,KEY_P)
        DrawCheckered maCheckered;
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            maCheckered.RenderRegion(rDev, r, rCtx);

            long nDx = r.GetWidth()/20;
            long nDy = r.GetHeight()/20;
            Rectangle aShrunk(r);
            aShrunk.Move(nDx, nDy);
            aShrunk.SetSize(Size(r.GetWidth()-nDx*2,
                                 r.GetHeight()-nDy*2));
            Polygon aPoly(aShrunk);
            tools::PolyPolygon aPPoly(aPoly);
            rDev.SetLineColor(Color(COL_RED));
            rDev.SetFillColor(Color(COL_RED));
            // This hits the optional 'drawPolyPolygon' code-path
            rDev.DrawTransparent(aPPoly, 64);
        }
    };

    struct DrawEllipse : public RegionRenderer
    {
        RENDER_DETAILS(ellipse,KEY_E)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &) SAL_OVERRIDE
        {
            rDev.SetLineColor(Color(COL_RED));
            rDev.SetFillColor(Color(COL_GREEN));
            rDev.DrawEllipse(r);
        }
    };

    struct DrawGradient : public RegionRenderer
    {
        RENDER_DETAILS(gradient,KEY_G)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            if (rCtx.meStyle == RENDER_EXPANDED)
            {
                std::vector<Rectangle> aRegions(DemoWin::partition(rDev, 5, 4));
                sal_uInt32 nStartCols[] = {
                    COL_RED, COL_RED, COL_RED, COL_GREEN, COL_GREEN,
                    COL_BLUE, COL_BLUE, COL_BLUE, COL_CYAN, COL_CYAN,
                    COL_BLACK, COL_LIGHTGRAY, COL_WHITE, COL_BLUE, COL_CYAN,
                    COL_WHITE, COL_WHITE, COL_WHITE, COL_BLACK, COL_BLACK
                };
                sal_uInt32 nEndCols[] = {
                    COL_WHITE, COL_WHITE, COL_WHITE, COL_BLACK, COL_BLACK,
                    COL_RED, COL_RED, COL_RED, COL_GREEN, COL_GREEN,
                    COL_GRAY, COL_GRAY, COL_LIGHTGRAY, COL_LIGHTBLUE, COL_LIGHTCYAN,
                    COL_BLUE, COL_BLUE, COL_BLUE, COL_CYAN, COL_CYAN
                };
                GradientStyle eStyles[] = {
                    GradientStyle_LINEAR, GradientStyle_AXIAL, GradientStyle_RADIAL, GradientStyle_ELLIPTICAL, GradientStyle_SQUARE,
                    GradientStyle_RECT, GradientStyle_FORCE_EQUAL_SIZE, GradientStyle_LINEAR, GradientStyle_RADIAL, GradientStyle_LINEAR,
                    GradientStyle_LINEAR, GradientStyle_AXIAL, GradientStyle_RADIAL, GradientStyle_ELLIPTICAL, GradientStyle_SQUARE,
                    GradientStyle_RECT, GradientStyle_FORCE_EQUAL_SIZE, GradientStyle_LINEAR, GradientStyle_RADIAL, GradientStyle_LINEAR
                };
                sal_uInt16 nAngles[] = {
                    0, 0, 0, 0, 0,
                    15, 30, 45, 60, 75,
                    90, 120, 135, 160, 180,
                    0, 0, 0, 0, 0
                };
                sal_uInt16 nBorders[] = {
                    0, 0, 0, 0, 0,
                    1, 10, 100, 10, 1,
                    0, 0, 0, 0, 0,
                    1, 10, 20, 10, 1,
                    0, 0, 0, 0, 0
                };
                DemoWin::clearRects(rDev, aRegions);
                assert(aRegions.size() <= SAL_N_ELEMENTS(nStartCols));
                assert(aRegions.size() <= SAL_N_ELEMENTS(nEndCols));
                assert(aRegions.size() <= SAL_N_ELEMENTS(eStyles));
                assert(aRegions.size() <= SAL_N_ELEMENTS(nAngles));
                assert(aRegions.size() <= SAL_N_ELEMENTS(nBorders));
                for (size_t i = 0; i < aRegions.size(); i++)
                {
                    Rectangle aSub = aRegions[i];
                    Gradient aGradient;
                    aGradient.SetStartColor(Color(nStartCols[i]));
                    aGradient.SetEndColor(Color(nEndCols[i]));
                    aGradient.SetStyle(eStyles[i]);
                    aGradient.SetAngle(nAngles[i]);
                    aGradient.SetBorder(nBorders[i]);
                    rDev.DrawGradient(aSub, aGradient);
                }
            }
            else
            {
                Gradient aGradient;
                aGradient.SetStartColor(COL_YELLOW);
                aGradient.SetEndColor(COL_RED);
                //          aGradient.SetAngle(45);
                aGradient.SetStyle(GradientStyle_RECT);
                aGradient.SetBorder(r.GetSize().Width()/20);
                rDev.DrawGradient(r, aGradient);
            }
        }
    };

    struct DrawBitmap : public RegionRenderer
    {
        RENDER_DETAILS(bitmap,KEY_B)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            Bitmap aBitmap(rCtx.mpDemoWin->maIntroBW);
            aBitmap.Scale(r.GetSize(), BMP_SCALE_BESTQUALITY);
            rDev.DrawBitmap(r.TopLeft(), aBitmap);
        }
    };

    struct DrawBitmapEx : public RegionRenderer
    {
        RENDER_DETAILS(bitmapex,KEY_X)
        DrawCheckered maCheckered;
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            maCheckered.RenderRegion(rDev, r, rCtx);

            BitmapEx aBitmap(rCtx.mpDemoWin->maIntro);
            aBitmap.Scale(r.GetSize(), BMP_SCALE_BESTQUALITY);
            AlphaMask aSemiTransp(aBitmap.GetSizePixel());
            aSemiTransp.Erase(64);
            rDev.DrawBitmapEx(r.TopLeft(), BitmapEx(aBitmap.GetBitmap(),
                                                    aSemiTransp));
        }
    };

    struct DrawPolyPolygons : public RegionRenderer
    {
        RENDER_DETAILS(polypoly,KEY_N)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &) SAL_OVERRIDE
        {
            struct {
                double nX, nY;
            } aPoints[] = { { 0.1, 0.1 }, { 0.9, 0.9 },
#ifdef FIXME_SELF_INTERSECTING_WORKING
                            { 0.9, 0.1 }, { 0.1, 0.9 },
                            { 0.1, 0.1 }
#else
                            { 0.1, 0.9 }, { 0.5, 0.5 },
                            { 0.9, 0.1 }, { 0.1, 0.1 }
#endif
            };

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
                    rDev.SetLineColor(Color(COL_YELLOW));
                    rDev.SetFillColor(Color(COL_BLACK));
                    rDev.DrawPolygon(aPoly);

                    // now move and add to the polypolygon
                    aPoly.Move(0, r.GetHeight()/2);
                    aPolyPoly.Insert(aPoly);
                }
            }
            rDev.SetLineColor(Color(COL_LIGHTRED));
            rDev.SetFillColor(Color(COL_GREEN));
            rDev.DrawTransparent(aPolyPoly, 50);
        }
    };

    struct DrawToVirtualDevice : public RegionRenderer
    {
        RENDER_DETAILS(vdev,KEY_V)
        enum RenderType {
            RENDER_AS_BITMAP,
            RENDER_AS_OUTDEV,
            RENDER_AS_BITMAPEX,
            RENDER_AS_ALPHA_OUTDEV
        };

        void SizeAndRender(OutputDevice &rDev, Rectangle r, RenderType eType,
                           const RenderContext &rCtx)
        {
            VirtualDevice *pNested;

            if ((int)eType < RENDER_AS_BITMAPEX)
                pNested = new VirtualDevice(rDev);
            else
                pNested = new VirtualDevice(rDev,0,0);

            pNested->SetOutputSizePixel(r.GetSize());
            Rectangle aWhole(Point(0,0), r.GetSize());

            // mini me
            rCtx.mpDemoWin->drawToDevice(*pNested, true);

            if (eType == RENDER_AS_BITMAP)
            {
                Bitmap aBitmap(pNested->GetBitmap(Point(0,0),aWhole.GetSize()));
                rDev.DrawBitmap(r.TopLeft(), aBitmap);
            }
            else if (eType == RENDER_AS_BITMAPEX)
            {
                BitmapEx aBitmapEx(pNested->GetBitmapEx(Point(0,0),aWhole.GetSize()));
                rDev.DrawBitmapEx(r.TopLeft(), aBitmapEx);
            }
            else if (eType == RENDER_AS_OUTDEV ||
                     eType == RENDER_AS_ALPHA_OUTDEV)
            {
                rDev.DrawOutDev(r.TopLeft(), r.GetSize(),
                                aWhole.TopLeft(), aWhole.GetSize(),
                                *pNested);
            }
            delete pNested;
        }
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            // avoid infinite recursion
            if (rCtx.mbVDev)
                return;

            if (rCtx.meStyle == RENDER_EXPANDED)
            {
                std::vector<Rectangle> aRegions(DemoWin::partition(rDev, 2, 2));
                DemoWin::clearRects(rDev, aRegions);

                RenderType eRenderTypes[] = { RENDER_AS_BITMAP, RENDER_AS_OUTDEV,
                                              RENDER_AS_BITMAPEX, RENDER_AS_ALPHA_OUTDEV };
                for (size_t i = 0; i < aRegions.size(); i++)
                    SizeAndRender(rDev, aRegions[i], eRenderTypes[i], rCtx);
            }
            else
                SizeAndRender(rDev, r, RENDER_AS_BITMAP, rCtx);
        }
    };

    struct DrawIcons : public RegionRenderer
    {
        RENDER_DETAILS(icons,KEY_I)

        std::vector<OUString> maIconNames;
        std::vector<BitmapEx> maIcons;
        bool bHasLoadedAll;
        DrawIcons() : bHasLoadedAll(false)
        {
            // a few icons to start with
            const char *pNames[] = {
                "cmd/lc_openurl.png",
                "cmd/lc_newdoc.png",
                "cmd/lc_choosemacro.png",
                "cmd/lc_save.png",
                "cmd/lc_saveas.png",
                "cmd/lc_importdialog.png",
                "cmd/lc_sendmail.png",
                "cmd/lc_editdoc.png",
                "cmd/lc_print.png",
                "cmd/lc_combobox.png",
                "cmd/lc_insertformcombo.png",
                "cmd/lc_printpreview.png",
                "cmd/lc_cut.png",
                "cmd/lc_copy.png",
                "cmd/lc_paste.png",
                "cmd/sc_autopilotmenu.png",
                "cmd/lc_formatpaintbrush.png",
                "cmd/lc_undo.png",
                "cmd/lc_redo.png",
                "cmd/lc_marks.png",
                "cmd/lc_fieldnames.png",
                "cmd/lc_hyperlinkdialog.png",
              };
            for (size_t i = 0; i < SAL_N_ELEMENTS(pNames); i++)
            {
                maIconNames.push_back(OUString::createFromAscii(pNames[i]));
                maIcons.push_back(BitmapEx(maIconNames[i]));
            }
        }

        void LoadAllImages()
        {
            if (bHasLoadedAll)
                return;
            bHasLoadedAll = true;

            css::uno::Sequence< OUString > aAllIcons = ImageTree_getAllImageNames();
            for (sal_Int32 i = 0; i < aAllIcons.getLength(); i++)
            {
                maIconNames.push_back(aAllIcons[i]);
                maIcons.push_back(BitmapEx(aAllIcons[i]));
            }
        }

        void doDrawIcons(OutputDevice &rDev, Rectangle r, bool bExpanded)
        {
            long nMaxH = 0, nVPos = 0;
            Point p(r.TopLeft());
            size_t nToRender = maIcons.size();

            if (!bExpanded && maIcons.size() > 64)
                nToRender = 64;
            for (size_t i = 0; i < nToRender; i++)
            {
                Size aSize(maIcons[i].GetSizePixel());
//              sAL_DEBUG("Draw icon '" << maIconNames[i] << "'");

                if (!i % 4)
                    rDev.DrawBitmapEx(p, maIcons[i]);
                else
                {
                    basegfx::B2DHomMatrix aTransform;
                    aTransform.scale(aSize.Width(), aSize.Height());
                    switch (i % 4)
                    {
                    case 2:
                        aTransform.shearX((double)((i >> 2) % 8) / 8);
                        aTransform.shearY((double)((i >> 4) % 8) / 8);
                        break;
                    case 3:
                        aTransform.translate(-aSize.Width()/2, -aSize.Height()/2);
                        aTransform.rotate(i);
                        if (i & 0x100)
                        {
                            aTransform.shearX((double)((i >> 2) % 8) / 8);
                            aTransform.shearY((double)((i >> 4) % 8) / 8);
                        }
                        aTransform.translate(aSize.Width()/2,  aSize.Height()/2);
                        break;
                    default:
                        aTransform.translate(-aSize.Width()/2, -aSize.Height()/2);
                        aTransform.rotate(2 * F_2PI * i / nToRender);
                        aTransform.translate(aSize.Width()/2,  aSize.Height()/2);
                        break;
                    }
                    aTransform.translate(p.X(), p.Y());
                    rDev.DrawTransformedBitmapEx(aTransform, maIcons[i]);
                }

                // next position
                p.Move(aSize.Width(), 0);
                if (aSize.Height() > nMaxH)
                    nMaxH = aSize.Height();
                if (p.X() >= r.Right()) // wrap to next line
                {
                    nVPos += nMaxH;
                    nMaxH = 0;
                    p = Point(r.Left(), r.Top() + nVPos);
                }
                if (p.Y() >= r.Bottom()) // re-start at top
                {
                    p = r.TopLeft();
                    nVPos = 0;
                }
            }
        }

        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &rCtx) SAL_OVERRIDE
        {
            if (rCtx.meStyle == RENDER_EXPANDED)
            {
                LoadAllImages();
                doDrawIcons(rDev, r, true);
            }
            else
            {
                doDrawIcons(rDev, r, false);
            }
        }
    };

    struct FetchDrawBitmap : public RegionRenderer
    {
        RENDER_DETAILS(fetchdraw,KEY_F)
        virtual void RenderRegion(OutputDevice &rDev, Rectangle r,
                                  const RenderContext &) SAL_OVERRIDE
        {
            Bitmap aBitmap(rDev.GetBitmap(Point(0,0),rDev.GetOutputSizePixel()));
            aBitmap.Scale(r.GetSize(), BMP_SCALE_BESTQUALITY);
            rDev.DrawBitmap(r.TopLeft(), aBitmap);
        }
    };

    void drawToDevice(OutputDevice &rDev, bool bVDev)
    {
        RenderContext aCtx;
        aCtx.mbVDev = bVDev;
        aCtx.mpDemoWin = this;

        Rectangle aWholeWin(Point(0,0), rDev.GetOutputSizePixel());

        drawBackground(rDev, aWholeWin);

        if (!bVDev /* want everything in the vdev */ &&
            mnSelectedRenderer >= 0)
        {
            aCtx.meStyle = RENDER_EXPANDED;
            maRenderers[mnSelectedRenderer]->RenderRegion(rDev, aWholeWin, aCtx);
        }
        else
        {
            aCtx.meStyle = RENDER_THUMB;
            std::vector<Rectangle> aRegions(DemoWin::partition(rDev, mnSegmentsX, mnSegmentsY));
            DemoWin::clearRects(rDev, aRegions);
            for (size_t i = 0; i < maRenderers.size(); i++)
                maRenderers[i]->RenderRegion(rDev, aRegions[i], aCtx);
        }
    }
};

IMPL_LINK_NOARG(DemoWin,BounceTimerCb)
{
    mpButton->Check(mnBounceX>0);
    mpButton->SetPressed(mnBounceY>0);

    Point aCur = mpButtonWin->GetPosPixel();
    static const int nMovePix = 10;
    aCur.Move(mnBounceX * nMovePix, mnBounceX * nMovePix);
    Size aWinSize = GetSizePixel();
    if (aCur.X() <= 0 || aCur.X() >= aWinSize.Width())
        mnBounceX *= -1;
    if (aCur.Y() <= 0 || aCur.Y() >= aWinSize.Height())
        mnBounceX *= -1;
    mpButtonWin->SetPosPixel(aCur);

    // All smoke and mirrors to test sub-region invalidation underneath
    Rectangle aRect(aCur, mpButtonWin->GetSizePixel());
    Invalidate(aRect);
    return 0;
}

void DemoWin::KeyInput(const KeyEvent &rKEvt)
{
    sal_uInt16 nCode = rKEvt.GetKeyCode().GetCode();

    // click to zoom out
    if (mnSelectedRenderer >= 0)
    {
        if (nCode == KEY_ESCAPE || nCode == KEY_BACKSPACE)
        {
            mnSelectedRenderer = -1;
            Invalidate();
            return;
        }
    }
    else
    {
        for (size_t i = 0; i < maRenderers.size(); i++)
        {
            if (nCode == maRenderers[i]->getAccelerator())
            {
                mnSelectedRenderer = i;
                Invalidate();
                return;
            }
        }
    }
}

void DemoWin::MouseButtonDown(const MouseEvent& rMEvt)
{
    // click to zoom out
    if (mnSelectedRenderer >= 0)
    {
        mnSelectedRenderer = -1;
        Invalidate();
        return;
    }

    // click on a region to zoom into it
    std::vector<Rectangle> aRegions(partition(*this, mnSegmentsX, mnSegmentsY));
    for (size_t i = 0; i < aRegions.size(); i++)
    {
        if (aRegions[i].IsInside(rMEvt.GetPosPixel()))
        {
            mnSelectedRenderer = i;
            Invalidate();
            return;
        }
    }

    // otherwise bounce floating windows
    if (!mpButton)
    {
        mpButtonWin = new FloatingWindow(this);
        mpButton = new PushButton(mpButtonWin);
        mpButton->SetSymbol(SymbolType::HELP);
        mpButton->SetText("PushButton demo");
        mpButton->SetPosSizePixel(Point(0,0), mpButton->GetOptimalSize());
        mpButton->Show();
        mpButtonWin->SetPosSizePixel(Point(0,0), mpButton->GetOptimalSize());
        mpButtonWin->Show();
        mnBounceX = 1; mnBounceX = 1;
        maBounce.SetTimeoutHdl(LINK(this,DemoWin,BounceTimerCb));
        maBounce.SetTimeout(55);
        maBounce.Start();
    }
    else
    {
        maBounce.Stop();
        delete mpButtonWin;
        mpButtonWin = NULL;
        mpButton = NULL;
    }
}

void DemoWin::InitRenderers()
{
    maRenderers.push_back(new DrawLines());
    maRenderers.push_back(new DrawText());
    maRenderers.push_back(new DrawPoly());
    maRenderers.push_back(new DrawEllipse());
    maRenderers.push_back(new DrawCheckered());
    maRenderers.push_back(new DrawBitmapEx());
    maRenderers.push_back(new DrawBitmap());
    maRenderers.push_back(new DrawGradient());
    maRenderers.push_back(new DrawPolyPolygons());
    maRenderers.push_back(new DrawToVirtualDevice());
    maRenderers.push_back(new DrawIcons());
    maRenderers.push_back(new FetchDrawBitmap());
}

OUString DemoWin::getRendererList()
{
    OUStringBuffer aBuf;
    for (size_t i = 0; i < maRenderers.size(); i++)
    {
        aBuf.append(maRenderers[i]->getName());
        aBuf.append(' ');
    }
    return aBuf.makeStringAndClear();
}

void DemoWin::selectRenderer(const OUString &rName)
{
    for (size_t i = 0; i < maRenderers.size(); i++)
    {
        if (maRenderers[i]->getName() == rName)
        {
            mnSelectedRenderer = i;
            Invalidate();
            return;
        }
    }
}

class DemoApp : public Application
{
    int showHelp(DemoWin &rWin)
    {
        fprintf(stderr,"vcldemo - a VCL test app\n");
        fprintf(stderr,"  --help            - print this text\n");
        fprintf(stderr,"  --show <renderer> - start with a given renderer, options are:\n");
        OUString aRenderers(rWin.getRendererList());
        fprintf(stderr,"         %s\n\n",
                rtl::OUStringToOString(aRenderers, RTL_TEXTENCODING_UTF8).getStr());
        return 0;
    }

public:
    DemoApp() {}

    virtual int Main() SAL_OVERRIDE
    {
        try
        {
            DemoWin aMainWin;

            for (sal_Int32 i = 0; i < GetCommandLineParamCount(); i++)
            {
                bool bLast = i == GetCommandLineParamCount() - 1;
                OUString aArg = GetCommandLineParam(i);
                if (aArg == "--help" || aArg == "-h")
                    return showHelp(aMainWin);
                if (aArg == "--show")
                {
                    if (bLast)
                        return showHelp(aMainWin);
                    else
                        aMainWin.selectRenderer(GetCommandLineParam(++i));
                }
            }

            aMainWin.SetText("Interactive VCL demo");
            aMainWin.Show();
            Application::Execute();
        }
        catch (const css::uno::Exception& e)
        {
            SAL_WARN("vcl.app", "Fatal exception: " << e.Message);
            return 1;
        }
        catch (const std::exception& e)
        {
            SAL_WARN("vcl.app", "Fatal exception: " << e.what());
            return 1;
        }
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
                (xComponentContext->getServiceManager(), uno::UNO_QUERY);
            if(!xMSF.is())
                Application::Abort("Bootstrap failure - no service manager");

            ::comphelper::setProcessServiceFactory(xMSF);
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
        uno::UNO_QUERY_THROW)-> dispose();
        ::comphelper::setProcessServiceFactory(NULL);
    }
};

void vclmain::createApplication()
{
    static DemoApp aApp;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
