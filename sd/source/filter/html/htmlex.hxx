/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: htmlex.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: kz $ $Date: 2006-12-12 16:42:51 $
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

#ifndef _SD_HTMLEX_HXX
#define _SD_HTMLEX_HXX

#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_UCB_XSIMPLEFILEACCESS_HPP_
#include <com/sun/star/ucb/XSimpleFileAccess.hpp>
#endif

#ifndef _SV_GDIMTF_HXX //autogen
#include <vcl/gdimtf.hxx>
#endif
#ifndef _SFXITEMSET_HXX //autogen
#include <svtools/itemset.hxx>
#endif
#ifndef _SD_RESLTN_HXX
#include "resltn.hxx"       // enum PublishingResolution
#endif
#ifndef _SV_COLRDLG_HXX
#include <svtools/colrdlg.hxx>
#endif
#ifndef _EHDL_HXX //autogen
#include <svtools/ehdl.hxx>
#endif

#include "strings.hrc"

#ifndef SD_DRAW_DOC_SHELL_HXX
#include "DrawDocShell.hxx"
#endif

#ifndef SD_WINDOW_HXX
#include "Window.hxx"
#endif

#ifndef SD_VIEW_SHELL_HXX
#include "ViewShell.hxx"
#endif

#ifndef INC_ASSCLASS
#include "assclass.hxx"
#endif

#ifndef _SD_RESID_HXX
#include "sdresid.hxx"
#endif

#ifndef _SD_PUBDLG_HXX
#include "pubdlg.hxx"
#endif

#define NUM_BUTTONS 12

#define PUB_LOWRES_WIDTH    640
#define PUB_LOWRES_HEIGHT   480
#define PUB_MEDRES_WIDTH    800
#define PUB_MEDRES_HEIGHT   600
#define PUB_HIGHRES_WIDTH   1024
#define PUB_HIGHRES_HEIGHT  768

#define HtmlButtonThemaStr = "private://gallery/hidden/HtmlExportButtons";

class List;
class SfxProgress;
class SdrOutliner;
class SdPage;
class HtmlState;
class SdrTextObj;
class SdrPage;
class SdDrawDocument;

namespace sd {
class View;
}

class HtmlErrorContext : public ErrorContext
{
private:
    USHORT  mnResId;
    String  maURL1;
    String  maURL2;

public:
                    HtmlErrorContext(Window *pWin=0);
                    ~HtmlErrorContext() {};

    virtual BOOL    GetString( ULONG nErrId, String& rCtxStr );

    void            SetContext( USHORT nResId );
    void            SetContext( USHORT nResId, const String& rURL );
    void            SetContext( USHORT nResId, const String& rURL1, const String& rURL2 );
};

// =====================================================================
// this class exports an Impress Document as a HTML Presentation
// =====================================================================
class HtmlExport
{
    String maPath;

    SdDrawDocument* mpDoc;
    ::sd::DrawDocShell* mpDocSh;

    HtmlErrorContext meEC;

    HtmlPublishMode meMode;
    SfxProgress* mpProgress;
    bool mbImpress;
    USHORT mnSdPageCount;
    USHORT mnPagesWritten;
    bool mbContentsPage;
    INT16 mnButtonThema;
    UINT16 mnWidthPixel;
    UINT16 mnHeightPixel;
    PublishingFormat meFormat;
    bool mbHeader;
    bool mbNotes;
    bool mbFrames;
    bool mbKiosk;
    String maIndex;
    String maEMail;
    String maAuthor;
    String maHomePage;
    String maInfo;
    INT16 mnCompression;
    String maDocFileName;
    String maFramePage;
    String mDocTitle;
    bool mbDownload;

    bool mbAutoSlide;
    UINT32  mnSlideDuration;
    bool mbSlideSound;
    bool mbEndless;

    bool mbUserAttr;            // die folgenden Farben werden fuer das <body>
    Color maTextColor;          // tag genutzt, wenn mbUserAttr true ist
    Color maBackColor;
    Color maLinkColor;
    Color maVLinkColor;
    Color maALinkColor;
    Color maFirstPageColor;
    bool mbDocColors;

    String   maHTMLExtension;
    String** mpHTMLFiles;
    String** mpImageFiles;
    String** mpPageNames;
    String** mpTextFiles;

    String maExportPath;            // Das Ausgabeverzeichnes bzw. die URL
    String maIndexUrl;
    String maURLPath;
    String maCGIPath;
    PublishingScript meScript;

    const String maHTMLHeader;

    SdrTextObj* GetLayoutTextObject(SdrPage* pPage);

    void SetDocColors( SdPage* pPage = NULL );

    bool    CreateImagesForPresPages();
    bool    CreateHtmlTextForPresPages();
    bool    CreateHtmlForPresPages();
    bool    CreateContentPage();
    void    CreateFileNames();
    bool    CreateBitmaps();
    bool    CreateOutlinePages();
    bool    CreateFrames();
    bool    CreateNotesPages();
    bool    CreateNavBarFrames();

    bool    CreateASPScripts();
    bool    CreatePERLScripts();
    bool    CreateImageFileList();
    bool    CreateImageNumberFile();

    bool    checkForExistingFiles();
    bool    checkFileExists( ::com::sun::star::uno::Reference< ::com::sun::star::ucb::XSimpleFileAccess >& xFileAccess, String const & aFileName );

    ULONG   CreateBitmap( ULONG nThemeId, INT16 nImage, const String& aName ) const;
    void    SmoothBitmap( BitmapEx& aBmp, Color aBackCol ) const;
    String getDocumentTitle();
    bool    SavePresentation();

    String  CreateLink( const String& aLink, const String& aText,
                        const String& aTarget = String()) const;
    String  CreateImage( const String& aImage, const String& aAltText, INT16 nWidth = -1, INT16 nHeight = -1 ) const;
    String  CreateNavBar( USHORT nSdPage, bool bIsText ) const;
    String  CreateBodyTag() const;

    String  ParagraphToHTMLString( SdrOutliner* pOutliner, ULONG nPara, const Color& rBackgroundColor );
    String  TextAttribToHTMLString( SfxItemSet* pSet, HtmlState* pState, const Color& rBackgroundColor );

    String  CreateTextForTitle( SdrOutliner* pOutliner, SdPage* pPage, const Color& rBackgroundColor );
    String  CreateTextForPage( SdrOutliner* pOutliner, SdPage* pPage, bool bHeadLine, const Color& rBackgroundColor );
    String  CreateTextForNotesPage( SdrOutliner* pOutliner, SdPage* pPage, bool bHeadLine, const Color& rBackgroundColor );

    String  CreateHTMLCircleArea( ULONG nRadius, ULONG nCenterX,
                                  ULONG nCenterY, const String& rHRef ) const;
    String  CreateHTMLPolygonArea( const ::basegfx::B2DPolyPolygon& rPolyPoly, Size aShift, double fFactor, const String& rHRef ) const;
    String  CreateHTMLRectArea( const Rectangle& rRect,
                                const String& rHRef ) const;

    String  CreatePageURL( USHORT nPgNum );

    String InsertSound( const String& rSoundFile );
    bool CopyFile( const String& rSourceFile, const String& rDestPath );
    bool CopyScript( const String& rPath, const String& rSource, const String& rDest, bool bUnix = false );

    void InitProgress( USHORT nProgrCount );
    void ResetProgress();

    String WriteMetaCharset() const;

    void InitExportParameters( const com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >& rParams);
    void ExportHtml();
    void ExportKiosk();
    void ExportWebCast();

    List aSpecialObjects;
    void HideSpecialObjects( SdPage* pPage );
    void ShowSpecialObjects();

    bool WriteHtml( const String& rFileName, bool bAddExtension, const String& rHtmlData );
    String GetButtonName( USHORT nButton ) const;

 public:
     HtmlExport( rtl::OUString aPath, const com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >& rParams, SdDrawDocument* pExpDoc, ::sd::DrawDocShell* pDocShell );
    virtual ~HtmlExport();

    static String   ColorToHTMLString( Color aColor );
    static String   StringToHTMLString( const String& rString );
    static String   StringToURL( const String& rURL );
};

#endif // _SD_HTMLEX_HXX
