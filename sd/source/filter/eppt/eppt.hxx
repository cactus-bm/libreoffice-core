/*************************************************************************
 *
 *  $RCSfile: eppt.hxx,v $
 *
 *  $Revision: 1.38 $
 *
 *  last change: $Author: rt $ $Date: 2004-04-02 13:21:32 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _EPPT_HXX_
#define _EPPT_HXX_
#include <vector>
#ifndef _PptEscherEx_HXX
#include "escherex.hxx"
#endif
#ifndef _SOLAR_H
#include <tools/solar.h>
#endif
#ifndef _SVSTOR_HXX
#include <so3/svstor.hxx>
#endif
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
#ifndef _SV_GRAPH_HXX
#include <vcl/graph.hxx>
#endif
#ifndef _SV_FONTCVT_HXX
#include <vcl/fontcvt.hxx>
#endif
#include <tools/string.hxx>

// ------------------------------------------------------------------------

#include <vcl/mapmod.hxx>
#ifndef _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_FILLSTYLE_HPP_
#include <com/sun/star/drawing/FillStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_LINESTYLE_HPP_
#include <com/sun/star/drawing/LineStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_DASHSTYLE_HPP_
#include <com/sun/star/drawing/DashStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_HATCHSTYLE_HPP_
#include <com/sun/star/drawing/HatchStyle.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_LINEENDTYPE_HPP_
#include <com/sun/star/drawing/LineEndType.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_ALIGNMENT_HPP_
#include <com/sun/star/drawing/Alignment.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_TEXTADJUST_HPP_
#include <com/sun/star/drawing/TextAdjust.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_CIRCLEKIND_HPP_
#include <com/sun/star/drawing/CircleKind.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POLYGONKIND_HPP_
#include <com/sun/star/drawing/PolygonKind.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POLYGONFLAGS_HPP_
#include <com/sun/star/drawing/PolygonFlags.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_XUNIVERSALSHAPEDESCRIPTOR_HPP_
#include <com/sun/star/drawing/XUniversalShapeDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XSHAPEGROUPER_HPP_
#include <com/sun/star/drawing/XShapeGrouper.hpp>
#endif

#ifndef _COM_SUN_STAR_TEXT_XSIMPLETEXT_HPP_
#include <com/sun/star/text/XSimpleText.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XCONNECTORSHAPE_HPP_
#include <com/sun/star/drawing/XConnectorShape.hpp>
#endif

#ifndef _COM_SUN_STAR_DRAWING_BEZIERPOINT_HPP_
#include <com/sun/star/drawing/BezierPoint.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_HATCH_HPP_
#include <com/sun/star/drawing/Hatch.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_LINEDASH_HPP_
#include <com/sun/star/drawing/LineDash.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POLYPOLYGONBEZIERCOORDS_HPP_
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#endif

#ifndef _COM_SUN_STAR_PRESENTATION_XPRESENTATIONSUPPLIER_HPP_
#include <com/sun/star/presentation/XPresentationSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_XCUSTOMPRESENTATIONSUPPLIER_HPP_
#include <com/sun/star/presentation/XCustomPresentationSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XMASTERPAGETARGET_HPP_
#include <com/sun/star/drawing/XMasterPageTarget.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XDRAWPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_XMASTERPAGESSUPPLIER_HPP_
#include <com/sun/star/drawing/XMasterPagesSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_XGRAPHICS_HPP_
#include <com/sun/star/awt/XGraphics.hpp>
#endif

#ifndef _COM_SUN_STAR_TASK_XSTATUSINDICATORSUPPLIER_HPP_
#include <com/sun/star/task/XStatusIndicatorSupplier.hpp>
#endif

#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONEFFECT_HPP_
#include <com/sun/star/presentation/AnimationEffect.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_FADEEFFECT_HPP_
#include <com/sun/star/presentation/FadeEffect.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_CLICKACTION_HPP_
#include <com/sun/star/presentation/ClickAction.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_ANIMATIONSPEED_HPP_
#include <com/sun/star/presentation/AnimationSpeed.hpp>
#endif
#ifndef _COM_SUN_STAR_PRESENTATION_PRESENTATIONRANGE_HPP_
#include <com/sun/star/presentation/PresentationRange.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELDSSUPPLIER_HPP_
#include <com/sun/star/text/XTextFieldsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XTEXTFIELD_HPP_
#include <com/sun/star/text/XTextField.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMED_HPP_
#include <com/sun/star/container/XNamed.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTDESCRIPTOR_HPP_
#include <com/sun/star/awt/FontDescriptor.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXCONTAINER_HPP_
#include <com/sun/star/container/XIndexContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XCONTROLMODEL_HPP_
#include <com/sun/star/awt/XControlModel.hpp>
#endif
#ifndef _COM_SUN_STAR_STYLE_TABSTOP_HPP_
#include <com/sun/star/style/TabStop.hpp>
#endif
#ifndef _MSOCXIMEX_HXX
#include <svx/msocximex.hxx>
#endif

#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSTATE_HPP_
#include <com/sun/star/beans/XPropertyState.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSETINFO_HPP_
#include <com/sun/star/beans/XPropertySetInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTFAMILY_HPP_
#include <com/sun/star/awt/FontFamily.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_FONTPITCH_HPP_
#include <com/sun/star/awt/FontPitch.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_CHARSET_HPP_
#include <com/sun/star/awt/CharSet.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_WRITINGMODE_HPP_
#include <com/sun/star/text/WritingMode.hpp>
#endif

enum PageType { NORMAL = 0, MASTER = 1, NOTICE = 2, UNDEFINED = 3 };

#define EPP_MAINMASTER_PERSIST_KEY      0x80010000
#define EPP_MAINNOTESMASTER_PERSIST_KEY 0x80020000
#define EPP_MAINSLIDE_PERSIST_KEY       0x80030000
#define EPP_MAINNOTES_PERSIST_KEY       0x80040000

#define EPP_Persist_Document            0x80080000
#define EPP_Persist_MainMaster          0x80100000
#define EPP_Persist_MainNotes           0x80200000
#define EPP_Persist_Slide               0x80400000
#define EPP_Persist_Notes               0x80800000
#define EPP_Persist_CurrentPos          0x81000000
#define EPP_Persist_VBAInfoAtom         0x84000000
#define EPP_Persist_ExObj               0x88000000

#define EPP_TEXTSTYLE_NORMAL            0x00000001
#define EPP_TEXTSTYLE_TITLE             0x00000010
#define EPP_TEXTSTYLE_BODY              0x00000100
#define EPP_TEXTSTYLE_TEXT              0x00001000

// PLACEMENT_ID
#define EPP_LAYOUT_TITLESLIDE             0 /* The slide is a title slide                                                             */
#define EPP_LAYOUT_TITLEANDBODYSLIDE      1 /* Title and body slide                                                                   */
#define EPP_LAYOUT_TITLEMASTERSLIDE       2 /* Title master slide                                                                     */
#define EPP_LAYOUT_MASTERSLIDE            3 /* Master slide layout                                                                    */
#define EPP_LAYOUT_MASTERNOTES            4 /* Master notes layout                                                                    */
#define EPP_LAYOUT_NOTESTITLEBODY         5 /* Notes title/body layout                                                                */
#define EPP_LAYOUT_HANDOUTLAYOUT          6 /* Handout layout, therefore it doesn't have placeholders except header, footer, and date */
#define EPP_LAYOUT_ONLYTITLE              7 /* Only title placeholder                                                                 */
#define EPP_LAYOUT_2COLUMNSANDTITLE       8 /* Body of the slide has 2 columns and a title                                            */
#define EPP_LAYOUT_2ROWSANDTITLE          9 /* Slide's body has 2 rows and a title                                                    */
#define EPP_LAYOUT_RIGHTCOLUMN2ROWS      10 /* Body contains 2 columns, right column has 2 rows                                       */
#define EPP_LAYOUT_LEFTCOLUMN2ROWS       11 /* Body contains 2 columns, left column has 2 rows                                        */
#define EPP_LAYOUT_BOTTOMROW2COLUMNS     12 /* Body contains 2 rows, bottom row has 2 columns                                         */
#define EPP_LAYOUT_TOPROW2COLUMN         13 /* Body contains 2 rows, top row has 2 columns                                            */
#define EPP_LAYOUT_4OBJECTS              14 /* 4 objects                                                                              */
#define EPP_LAYOUT_BIGOBJECT             15 /* Big object                                                                             */
#define EPP_LAYOUT_BLANCSLIDE            16 /* Blank slide                                                                            */
#define EPP_LAYOUT_TITLERIGHTBODYLEFT    17 /* Vertical title on the right, body on the left                                          */
#define EPP_LAYOUT_TITLERIGHT2BODIESLEFT 18 /* Vertical title on the right, body on the left split into 2 rows                        */

class Polygon;
class PptEscherEx;
class XStatusIndicatorRef;

struct PHLayout
{
    sal_Int32   nLayout;
    sal_uInt8   nPlaceHolder[ 8 ];

    sal_uInt8   nUsedObjectPlaceHolder;
    sal_uInt8   nTypeOfTitle;
    sal_uInt8   nTypeOfOutliner;

    BOOL    bTitlePossible;
    BOOL    bOutlinerPossible;
    BOOL    bSecOutlinerPossible;
};

static PHLayout pPHLayout[] =
{
    { EPP_LAYOUT_TITLESLIDE,            0x0d, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x10, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     0x0d, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0e, TRUE, TRUE, TRUE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x0e, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_BLANCSLIDE,            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0e, FALSE, FALSE, FALSE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x0e, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x14, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     0x0d, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x0d, 0x0e, TRUE, FALSE, FALSE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x16, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x0e, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     0x0d, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, FALSE, FALSE },
    { EPP_LAYOUT_RIGHTCOLUMN2ROWS,      0x0d, 0x0e, 0x13, 0x13, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_2ROWSANDTITLE,         0x0d, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_LEFTCOLUMN2ROWS,       0x0d, 0x13, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_TOPROW2COLUMN,         0x0d, 0x13, 0x13, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_2ROWSANDTITLE,         0x0d, 0x0e, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_4OBJECTS,              0x0d, 0x13, 0x13, 0x13, 0x13, 0x00, 0x00, 0x00, 0x13, 0x0d, 0x0e, TRUE, FALSE, FALSE },
    { EPP_LAYOUT_ONLYTITLE,             0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0e, TRUE, FALSE, FALSE },
    { EPP_LAYOUT_BLANCSLIDE,            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0e, FALSE, FALSE, FALSE },
    { EPP_LAYOUT_TITLERIGHT2BODIESLEFT, 0x11, 0x12, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x11, 0x12, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_TITLERIGHTBODYLEFT,    0x11, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x12, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_TITLEANDBODYSLIDE,     0x0d, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x12, TRUE, TRUE, FALSE },
    { EPP_LAYOUT_2COLUMNSANDTITLE,      0x0d, 0x16, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x0d, 0x12, TRUE, TRUE, FALSE }
};

struct SOParagraph
{
    sal_Bool                bDepth;
    sal_Bool                bExtendedParameters;
    sal_uInt32              nParaFlags;
    sal_Int16               nBulletFlags;
    String                  sPrefix;
    String                  sSuffix;
    String                  sGraphicUrl;            // String auf eine Graphic
    Size                    aBuGraSize;
    sal_uInt32              nNumberingType;         // in wirlichkeit ist dies ein SvxEnum
    sal_uInt32              nHorzAdjust;
    sal_uInt32              nBulletColor;
    sal_Int32               nBulletOfs;
    sal_Int16               nStartWith;             // Start der nummerierung
    sal_Int16               nTextOfs;
    sal_Int16               nBulletRealSize;        // GroessenVerhaeltnis in Proz
    sal_Int16               nDepth;                 // aktuelle tiefe
    sal_Unicode             cBulletId;              // wenn Numbering Type == CharSpecial
    ::com::sun::star::awt::FontDescriptor       aFontDesc;

    sal_Bool                bExtendedBulletsUsed;
    sal_uInt16              nBulletId;
    sal_uInt32              nMappedNumType;

    SOParagraph()
    {
        nDepth = 0;
        bDepth = TRUE;
        bExtendedParameters = FALSE;
        nParaFlags = 0;
        nBulletFlags = 0;
        bExtendedBulletsUsed = FALSE;
        nBulletId = 0xffff;
    };
};

// ------------------------------------------------------------------------

class EscherGraphicProvider;
class PPTExBulletProvider
{
    friend struct PPTExParaSheet;

    protected :

        SvMemoryStream          aBuExPictureStream;
        SvMemoryStream          aBuExOutlineStream;
        SvMemoryStream          aBuExMasterStream;

        EscherGraphicProvider*  pGraphicProv;

    public :

        sal_uInt16              GetId( const ByteString& rUniqueId, Size& rGraphicSize );

                                PPTExBulletProvider();
                                ~PPTExBulletProvider();
};

class SoundEntry
{
        sal_uInt32              nFileSize;
        String                  aSoundURL;

        String                  ImplGetName() const;
        String                  ImplGetExtension() const;

    public :

        sal_Bool                IsSameURL( const String& rURL ) const;
        sal_uInt32              GetFileSize( ) const { return nFileSize; };

                                SoundEntry( const String& rSoundURL );

        // returns the size of a complete SoundContainer
        sal_uInt32              GetSize( sal_uInt32 nId ) const;
        void                    Write( SvStream& rSt, sal_uInt32 nId );
};
class SoundCollection : private List
{
        const SoundEntry*       ImplGetByIndex( sal_uInt32 nId ) const;

    public:

                                SoundCollection() {}
                                ~SoundCollection();

        sal_uInt32              GetId( const String& );

        // returns the size of a complete SoundCollectionContainer
        sal_uInt32              GetSize() const;
        void                    Write( SvStream& rSt );
};

struct FontCollectionEntry
{
        String                  Name;
        double                  Scaling;
        sal_Int16               Family;
        sal_Int16               Pitch;
        sal_Int16               CharSet;

        String                  Original;
        sal_Bool                bIsConverted;

        FontCollectionEntry( const String& rName, sal_Int16 nFamily, sal_Int16 nPitch, sal_Int16 nCharSet ) :
                            Family  ( nFamily ),
                            Pitch   ( nPitch ),
                            CharSet ( nCharSet ),
                            Scaling ( 1.0 ),
                            Original( rName )
                            {
                                ImplInit( rName );
                            };

        FontCollectionEntry( const String& rName ) :
                            Original( rName ),
                            Scaling ( 1.0 )
                            {
                                ImplInit( rName );
                            };
        ~FontCollectionEntry();

    private :

        FontCollectionEntry() {};

        void ImplInit( const String& rName );
};

class VirtualDevice;
class FontCollection : private List
{
        VirtualDevice* pVDev;
    public :
                    FontCollection();
                    ~FontCollection();

        sal_uInt32  GetId( FontCollectionEntry& rFontDescriptor );
        sal_uInt32  GetCount() const { return List::Count(); };
        const FontCollectionEntry*                      GetById( sal_uInt32 nId );
        FontCollectionEntry&    GetLast() { return *(FontCollectionEntry*)List::Last(); };
};

// ------------------------------------------------------------------------

#define PPTEX_STYLESHEETENTRYS  9

enum PPTExTextAttr
{
    ParaAttr_BulletOn,
    ParaAttr_BuHardFont,
    ParaAttr_BuHardColor,
    ParaAttr_BuHardHeight,
    ParaAttr_BulletChar,
    ParaAttr_BulletFont,
    ParaAttr_BulletHeight,
    ParaAttr_BulletColor,
    ParaAttr_Adjust,
    ParaAttr_LineFeed,
    ParaAttr_UpperDist,
    ParaAttr_LowerDist,
    ParaAttr_TextOfs,
    ParaAttr_BulletOfs,
    ParaAttr_DefaultTab,
    ParaAttr_AsianLB_1,
    ParaAttr_AsianLB_2,
    ParaAttr_AsianLB_3,
    ParaAttr_BiDi,
    CharAttr_Bold,
    CharAttr_Italic,
    CharAttr_Underline,
    CharAttr_Shadow,
    CharAttr_Strikeout,
    CharAttr_Embossed,
    CharAttr_Font,
    CharAttr_AsianOrComplexFont,
    CharAttr_Symbol,
    CharAttr_FontHeight,
    CharAttr_FontColor,
    CharAttr_Escapement
};

struct PPTExCharLevel
{
    sal_uInt16      mnFlags;
    sal_uInt16      mnFont;
    sal_uInt16      mnAsianOrComplexFont;
    sal_uInt16      mnFontHeight;
    sal_uInt16      mnEscapement;
    sal_uInt32      mnFontColor;
};

struct PPTExCharSheet
{
                PPTExCharLevel  maCharLevel[ 5 ];

                PPTExCharSheet( int nInstance );

                void    SetStyleSheet( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > &,
                                        FontCollection& rFontCollection, int nLevel );
                void    Write( SvStream& rSt, PptEscherEx* pEx, sal_uInt16 nLev, sal_Bool bFirst, sal_Bool bSimpleText,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & rPagePropSet );

};

struct PPTExParaLevel
{
    sal_Bool        mbIsBullet;
    sal_uInt16      mnBulletChar;
    sal_uInt16      mnBulletFont;
    sal_uInt16      mnBulletHeight;
    sal_uInt32      mnBulletColor;

    sal_uInt16      mnAdjust;
    sal_uInt16      mnLineFeed;
    sal_uInt16      mnUpperDist;
    sal_uInt16      mnLowerDist;
    sal_uInt16      mnTextOfs;
    sal_uInt16      mnBulletOfs;
    sal_uInt16      mnDefaultTab;

    sal_Bool        mbExtendedBulletsUsed;
    sal_uInt16      mnBulletId;
    sal_uInt16      mnBulletStart;
    sal_uInt32      mnMappedNumType;
    sal_uInt32      mnNumberingType;
    sal_uInt16      mnAsianSettings;
    sal_uInt16      mnBiDi;
};

struct PPTExParaSheet
{
                PPTExBulletProvider& rBuProv;

                sal_uInt32  mnInstance;

                PPTExParaLevel  maParaLevel[ 5 ];
                PPTExParaSheet( int nInstance, sal_uInt16 nDefaultTab, PPTExBulletProvider& rProv );

                void    SetStyleSheet( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > &,
                                        FontCollection& rFontCollection, int nLevel, const PPTExCharLevel& rCharLevel );
                void    Write( SvStream& rSt, PptEscherEx* pEx, sal_uInt16 nLev, sal_Bool bFirst, sal_Bool bSimpleText,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & rPagePropSet );
};

class PPTExStyleSheet
{

    public :

                PPTExCharSheet*     mpCharSheet[ PPTEX_STYLESHEETENTRYS ];
                PPTExParaSheet*     mpParaSheet[ PPTEX_STYLESHEETENTRYS ];

                PPTExStyleSheet( sal_uInt16 nDefaultTab, PPTExBulletProvider& rBuProv );
                ~PPTExStyleSheet();

                PPTExParaSheet& GetParaSheet( int nInstance ) { return *mpParaSheet[ nInstance ]; };
                PPTExCharSheet& GetCharSheet( int nInstance ) { return *mpCharSheet[ nInstance ]; };

                void            SetStyleSheet( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > &,
                                                FontCollection& rFontCollection, int nInstance, int nLevel );
                sal_Bool        IsHardAttribute( sal_uInt32 nInstance, sal_uInt32 nLevel, PPTExTextAttr eAttr, sal_uInt32 nValue );

                sal_uInt32      SizeOfTxCFStyleAtom() const;
                void            WriteTxCFStyleAtom( SvStream& rSt );
};


struct EPPTHyperlink
{
    String      aURL;
    sal_uInt32  nType;      // bit 0-7 : type       ( 1: click action to a slide )
                            //                      ( 2: hyperlink url )
                            // bit 8-23: index
                            // bit 31  : hyperlink is attached to a shape

    EPPTHyperlink( const String rURL, sal_uInt32 nT ) :
        aURL        ( rURL ),
        nType       ( nT ){};
};

enum PPTExOleObjEntryType
{
    NORMAL_OLE_OBJECT, OCX_CONTROL
};

struct PPTExOleObjEntry
{
    PPTExOleObjEntryType    eType;
    sal_uInt32              nOfsA;          // offset to the EPP_ExOleObjAtom in mpExEmbed (set at creation)
    sal_uInt32              nOfsB;          // offset to the EPP_ExOleObjStg

    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel >    xControlModel;
    ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >       xShape;

    PPTExOleObjEntry( PPTExOleObjEntryType eT, sal_uInt32 nOfs ) :
        eType   ( eT ),
        nOfsA   ( nOfs ) {};
};

struct TextRuleEntry
{
    int                 nPageNumber;
    SvMemoryStream*     pOut;

    TextRuleEntry( int nPg ) :
        nPageNumber( nPg ),
        pOut ( NULL ){};

    ~TextRuleEntry() { delete pOut; };
};

// ------------------------------------------------------------------------

struct GroupEntry
{
        sal_uInt32                  mnCurrentPos;
        sal_uInt32                  mnCount;
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess >           mXIndexAccess;
                                GroupEntry( ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & rIndex )
                                {
                                    mXIndexAccess = rIndex;
                                    mnCount =mXIndexAccess->getCount();
                                    mnCurrentPos = 0;
                                };
                                GroupEntry( sal_uInt32 nCount )
                                {
                                    mnCount = nCount;
                                    mnCurrentPos = 0;
                                };
                                ~GroupEntry(){};
};

// ------------------------------------------------------------------------

class GroupTable
{
    protected:

        sal_uInt32              mnIndex;
        sal_uInt32              mnCurrentGroupEntry;
        sal_uInt32              mnMaxGroupEntry;
        sal_uInt32              mnGroupsClosed;
        GroupEntry**            mpGroupEntry;

        void                    ImplResizeGroupTable( sal_uInt32 nEntrys );

    public:

        sal_uInt32              GetCurrentGroupIndex() const { return mnIndex; };
        sal_Int32               GetCurrentGroupLevel() const { return mnCurrentGroupEntry - 1; };
        ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > &
                                GetCurrentGroupAccess() const { return mpGroupEntry[  mnCurrentGroupEntry - 1 ]->mXIndexAccess; };
        sal_uInt32              GetGroupsClosed();
        void                    SkipCurrentGroup();
        void                    ResetGroupTable( sal_uInt32 nCount );
        void                    ClearGroupTable();
        sal_Bool                EnterGroup( ::com::sun::star::uno::Reference< ::com::sun::star::container::XIndexAccess > & rIndex );
        sal_Bool                GetNextGroupEntry();
                                GroupTable();
                                ~GroupTable();
};

class PropValue
{
    protected :

        ::com::sun::star::uno::Any                              mAny;

        ::com::sun::star::uno::Reference
            < ::com::sun::star::beans::XPropertySet >           mXPropSet;

        sal_Bool    ImplGetPropertyValue( const String& rString );
        sal_Bool    ImplGetPropertyValue( const ::com::sun::star::uno::Reference
                        < ::com::sun::star::beans::XPropertySet > &, const String& );

    public :

        static sal_Bool GetPropertyValue(
                ::com::sun::star::uno::Any& rAny,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > &,
                        const String& rPropertyName,
                            sal_Bool bTestPropertyAvailability = sal_False );

        static ::com::sun::star::beans::PropertyState GetPropertyState(
                    const ::com::sun::star::uno::Reference < ::com::sun::star::beans::XPropertySet > &,
                        const String& rPropertyName );
};

class PropStateValue : public PropValue
{
    protected :

        ::com::sun::star::beans::PropertyState                  ePropState;
        ::com::sun::star::uno::Reference
            < ::com::sun::star::beans::XPropertyState >         mXPropState;

        sal_Bool    ImplGetPropertyValue( const String& rString, sal_Bool bGetPropertyState = TRUE );

};

// ------------------------------------------------------------------------

struct FieldEntry;
class PortionObj : public PropStateValue
{

    friend class ParagraphObj;

    protected :

        void            ImplClear();
        void            ImplConstruct( PortionObj& rPortionObj );
        sal_uInt32      ImplGetTextField( ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & rXTextRangeRef,
                            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & rXPropSetRef, String& rURL );
        sal_uInt32      ImplCalculateTextPositions( sal_uInt32 nCurrentTextPosition );
        void            ImplGetPortionValues( FontCollection& rFontCollection, sal_Bool bGetPropStateValue = FALSE );

    public :

        ::com::sun::star::beans::PropertyState  meCharColor;
        ::com::sun::star::beans::PropertyState  meCharHeight;
        ::com::sun::star::beans::PropertyState  meFontName;
        ::com::sun::star::beans::PropertyState  meAsianOrComplexFont;
        ::com::sun::star::beans::PropertyState  meCharEscapement;
        sal_uInt16      mnCharAttrHard;

        sal_uInt32      mnCharColor;
        sal_uInt16      mnCharAttr;
        sal_uInt16      mnCharHeight;
        sal_uInt16      mnFont;
        sal_uInt16      mnAsianOrComplexFont;
        sal_Int16       mnCharEscapement;

        sal_uInt32      mnTextSize;
        sal_Bool        mbLastPortion;

        sal_uInt16*     mpText;
        FieldEntry*     mpFieldEntry;

                        PortionObj( ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > & rXTextRangeRef,
                                        sal_Bool bLast, FontCollection& rFontCollection );
                        PortionObj( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & rXPropSetRef,
                                        FontCollection& rFontCollection );
                        PortionObj( PortionObj& rPortionObj );
                        ~PortionObj();

        void            Write( SvStream* pStrm, sal_Bool bLast );
        sal_uInt32      Count() const { return mnTextSize; };

        PortionObj&     operator=( PortionObj& rPortionObj );
};

struct ParaFlags
{
    sal_Bool    bFirstParagraph : 1;
    sal_Bool    bLastParagraph  : 1;

                    ParaFlags() { bFirstParagraph = TRUE; bLastParagraph = FALSE; };
};

class ParagraphObj : public List, public PropStateValue, public SOParagraph
{
    friend class TextObj;
    friend struct PPTExParaSheet;

        MapMode         maMapModeSrc;
        MapMode         maMapModeDest;

    protected :

        void            ImplConstruct( ParagraphObj& rParagraphObj );
        void            ImplClear();
        sal_uInt32      ImplCalculateTextPositions( sal_uInt32 nCurrentTextPosition );
        ::com::sun::star::awt::Size         ImplMapSize( const ::com::sun::star::awt::Size& rSize );
        void            ImplGetParagraphValues( PPTExBulletProvider& rBuProv, sal_Bool bGetPropStateValue = FALSE );
        void            ImplGetNumberingLevel( PPTExBulletProvider& rBuProv, sal_Int16 nDepth, sal_Bool bGetPropStateValue = FALSE );

    public :

        ::com::sun::star::uno::Sequence< ::com::sun::star::style::TabStop > maTabStop;

        sal_uInt32          mnTextSize;

        sal_Bool            mbIsBullet;
        sal_Bool            mbFirstParagraph;
        sal_Bool            mbLastParagraph;

        ::com::sun::star::beans::PropertyState  meBullet;
        ::com::sun::star::beans::PropertyState  meTextAdjust;
        ::com::sun::star::beans::PropertyState  meLineSpacing;
        ::com::sun::star::beans::PropertyState  meLineSpacingTop;
        ::com::sun::star::beans::PropertyState  meLineSpacingBottom;
        ::com::sun::star::beans::PropertyState  meForbiddenRules;
        ::com::sun::star::beans::PropertyState  meParagraphPunctation;
        ::com::sun::star::beans::PropertyState  meBiDi;

        sal_uInt16                              mnTextAdjust;
        sal_Int16                               mnLineSpacing;
        sal_Int16                               mnLineSpacingTop;
        sal_Int16                               mnLineSpacingBottom;
        sal_Bool                                mbForbiddenRules;
        sal_Bool                                mbParagraphPunctation;
        sal_uInt16                              mnBiDi;

                        ParagraphObj( ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextContent > & rXTextContentRef,
                            ParaFlags, FontCollection& rFontCollection,
                                PPTExBulletProvider& rBuProv );
                        ParagraphObj( ParagraphObj& rParargraphObj );
                        ParagraphObj( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & rXPropSetRef,
                                        PPTExBulletProvider& rBuProv );

        void            CalculateGraphicBulletSize( sal_uInt16 nFontHeight );
                        ~ParagraphObj();

        void            Write( SvStream* pStrm );
        sal_uInt32          Count() const { return mnTextSize; };

        ParagraphObj&   operator=( ParagraphObj& rParagraphObj );
};

struct ImplTextObj
{
    sal_uInt32      mnRefCount;
    sal_uInt32      mnTextSize;
    int             mnInstance;
    List*           mpList;
    sal_Bool        mbHasExtendedBullets;
    sal_Bool        mbFixedCellHeightUsed;

                    ImplTextObj( int nInstance );
                    ~ImplTextObj();
};

class TextObj
{
        ImplTextObj*    mpImplTextObj;
        void            ImplCalculateTextPositions();

    public :
                        TextObj( ::com::sun::star::uno::Reference< ::com::sun::star::text::XSimpleText > &
                                    rXText, int nInstance, FontCollection& rFontCollection, PPTExBulletProvider& rBuProv );
                        TextObj( TextObj& rTextObj );
                        ~TextObj();

        void            Write( SvStream* pStrm );

        ParagraphObj*   First(){ return (ParagraphObj*)mpImplTextObj->mpList->First(); };
        ParagraphObj*   Next(){ return(ParagraphObj*)mpImplTextObj->mpList->Next(); };
        sal_uInt32      Count() const { return mpImplTextObj->mnTextSize; };
        int             GetInstance() const { return mpImplTextObj->mnInstance; };
        sal_Bool        HasExtendedBullets(){ return mpImplTextObj->mbHasExtendedBullets; };

        TextObj&        operator=( TextObj& rTextObj );
};

// ------------------------------------------------------------------------
class PPTWriter : public GroupTable, public PropValue, public PPTExBulletProvider
{
        sal_Bool                        mbStatus;
        sal_uInt32                      mnStatMaxValue;
        sal_uInt32                      mnLatestStatValue;
        std::vector< PPTExStyleSheet* > maStyleSheetList;
        PPTExStyleSheet*                mpStyleSheet;

        EscherGraphicProvider*          mpGraphicProvider;
        Fraction                        maFraction;
        MapMode                         maMapModeSrc;
        MapMode                         maMapModeDest;
        ::com::sun::star::awt::Size     maDestPageSize;
        ::com::sun::star::awt::Size     maNotesPageSize;
        PageType                        meLatestPageType;
        List                            maSlideNameList;

        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >                 mXModel;
        ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator >        mXStatusIndicator;
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPagesSupplier >   mXDrawPagesSupplier;
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XMasterPagesSupplier > mXMasterPagesSupplier;
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPages >           mXDrawPages;
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >            mXDrawPage;
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >           mXPagePropSet;
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >           mXBackgroundPropSet;
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShapes >              mXShapes;
        ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape >               mXShape;
        ::com::sun::star::uno::Reference< ::com::sun::star::text::XSimpleText >             mXText;             // TextRef des globalen Text
        ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor >             mXCursor;
        ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange >              mXCursorText;       // TextRef des Teilstuecks des Cursors
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >           mXCursorPropSet;    // die Properties des Teilstueckes
        ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextField >              mXTextField;
        ::com::sun::star::awt::Size         maSize;
        ::com::sun::star::awt::Point        maPosition;
        Rectangle           maRect;
        ByteString          mType;
        sal_Bool            mbPresObj;
        sal_Bool            mbEmptyPresObj;
        sal_Bool            mbStatusIndicator;
        sal_Int32           mnAngle;
        sal_uInt32          mnTextStyle;

        sal_Bool            mbFontIndependentLineSpacing;
        sal_uInt32          mnTextSize;

        SvStorageRef        mrStg;
        SvStorageStream*    mpCurUserStrm;
        SvStorageStream*    mpStrm;
        SvStorageStream*    mpPicStrm;
        PptEscherEx*        mpPptEscherEx;

        List                maExOleObj;
        sal_uInt32          mnVBAOleOfs;
        SvMemoryStream*     mpVBA;
        sal_uInt32          mnExEmbed;
        SvMemoryStream*     mpExEmbed;

        sal_uInt32          mnPages;            // anzahl einzelner Slides ( ohne masterpages & notes & handout )
        sal_uInt32          mnMasterPages;      //
        sal_uInt32          mnDrawings;         // anzahl Slides +  masterpages + notes +  handout
        sal_uInt32          mnPagesWritten;
        sal_uInt32          mnUniqueSlideIdentifier;
        sal_uInt32          mnTxId;             // Identifier determined by the HOST (PP) ????
        sal_uInt32          mnDiaMode;          // 0 -> manuell
                                                // 1 -> halbautomatisch
                                                // 2 -> automatisch


        sal_uInt32          mnShapeMasterTitle;
        sal_uInt32          mnShapeMasterBody;

        List                maTextRuleList;     // TextRuleEntry's
        List                maHyperlink;
        FontCollection      maFontCollection;
        SoundCollection     maSoundCollection;

        PHLayout&           ImplGetLayout( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPropSet ) const;
        void                ImplWriteExtParaHeader( SvMemoryStream& rSt, sal_uInt32 nRef, sal_uInt32 nInstance, sal_uInt32 nSlideId );


        sal_uInt32          ImplProgBinaryTag( SvStream* pOutStrm = NULL );
        sal_uInt32          ImplProgBinaryTagContainer( SvStream* pOutStrm = NULL, SvMemoryStream* pBinTag = NULL );
        sal_uInt32          ImplProgTagContainer( SvStream* pOutStrm = NULL, SvMemoryStream* pBinTag = NULL );
        sal_uInt32          ImplOutlineViewInfoContainer( SvStream* pOutStrm = NULL );
        sal_uInt32          ImplSlideViewInfoContainer( sal_uInt32 nInstance, SvStream* pOutStrm = NULL );
        sal_uInt32          ImplVBAInfoContainer( SvStream* pOutStrm = NULL );
        sal_uInt32          ImplDocumentListContainer( SvStream* pOutStrm = NULL );
        sal_uInt32          ImplMasterSlideListContainer( SvStream* pOutStrm = NULL );

    protected:

        sal_Bool            ImplCreateSummaryInformation();
        sal_Bool            ImplCreateDocumentSummaryInformation();
        sal_Bool            ImplCreateCurrentUserStream();
        void                ImplCreateHeaderFooterStrings( SvStream& rOut,
                                ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPagePropSet );
        void                ImplCreateHeaderFooters( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& rXPagePropSet );
        sal_Bool            ImplCreateDocument();
        sal_Bool            ImplCreateHyperBlob( SvMemoryStream& rStream );
        sal_uInt32          ImplInsertBookmarkURL( const String& rBookmark, const sal_uInt32 nType,
            const String& rStringVer0, const String& rStringVer1, const String& rStringVer2, const String& rStringVer3 );
        sal_Bool            ImplCreateMaster( sal_uInt32 nPageNum );
        sal_Bool            ImplCreateMainNotes();
        sal_Bool            ImplCreateSlide( sal_uInt32 nPageNum );
        sal_Bool            ImplCreateNotes( sal_uInt32 nPageNum );
        void                ImplWriteBackground( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & rXBackgroundPropSet );
        void                ImplWriteVBA( SvMemoryStream* pVBA );
        void                ImplWriteOLE( sal_uInt32 nCnvrtFlags );
        sal_Bool            ImplWriteAtomEnding();

        sal_Bool            ImplInitSOIface();
        sal_Bool            ImplSetCurrentStyleSheet( sal_uInt32 nPageNum );
        sal_Bool            ImplGetPageByIndex( sal_uInt32 nIndex, PageType );
        sal_Bool            ImplGetShapeByIndex( sal_uInt32 nIndex, sal_Bool bGroup = FALSE );
        sal_uInt32          ImplGetMasterIndex( PageType ePageType );
        void                ImplFlipBoundingBox( EscherPropertyContainer& rPropOpt );
        sal_Bool            ImplGetText();
        sal_Bool            ImplCreatePresentationPlaceholder( const sal_Bool bMaster, const PageType PageType,
                                const sal_uInt32 StyleInstance, const sal_uInt8 PlaceHolderId );
        sal_Bool            ImplGetEffect( const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > &,
                                ::com::sun::star::presentation::AnimationEffect& eEffect,
                                ::com::sun::star::presentation::AnimationEffect& eTextEffect,
                                sal_Bool& bHasSound );
        void                ImplWriteObjectEffect( SvStream& rSt,
                                ::com::sun::star::presentation::AnimationEffect eEffect,
                                ::com::sun::star::presentation::AnimationEffect eTextEffect,
                                sal_uInt16 nOrder );
        void                ImplWriteClickAction( SvStream& rSt, ::com::sun::star::presentation::ClickAction eAction );
        sal_Bool            ImplGetStyleSheets();
        void                ImplWriteParagraphs( SvStream& rOutStrm, TextObj& rTextObj );
        void                ImplWritePortions( SvStream& rOutStrm, TextObj& rTextObj );
        void                ImplWriteTextStyleAtom( SvStream& rOut, int nTextInstance, sal_uInt32 nAtomInstance,
                                TextRuleEntry* pTextRule, SvStream& rExtBu );
        void                ImplWritePage( const PHLayout& rLayout,
                                                EscherSolverContainer& rSolver,
                                                    PageType ePageType,
                                                        sal_Bool bMaster,
                                                            int nPageNumber = 0 );

        ::com::sun::star::awt::Point        ImplMapPoint( const ::com::sun::star::awt::Point& );
        ::com::sun::star::awt::Size         ImplMapSize( const ::com::sun::star::awt::Size& );
        Rectangle                           ImplMapRectangle( const ::com::sun::star::awt::Rectangle& );

        sal_Bool                            ImplCloseDocument();        // die font-, hyper-, Soundliste wird geschrieben ..
        void                                ImplWriteCString( SvStream&, const String&, sal_uInt32 nInstance = 0 );

    public:
                                PPTWriter( SvStorageRef& rSvStorage,
                                            ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > & rModel,
                                            ::com::sun::star::uno::Reference< ::com::sun::star::task::XStatusIndicator > & rStatInd,
                                                SvMemoryStream* pVBA, sal_uInt32 nCnvrtFlags );

                                ~PPTWriter();

        sal_Bool                IsValid() const { return mbStatus; };
};


#endif
