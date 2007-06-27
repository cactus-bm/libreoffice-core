/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: datwin.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: hr $ $Date: 2007-06-27 21:08:23 $
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

#ifndef _SFXDATWIN_HXX
#define _SFXDATWIN_HXX

#ifndef _BRWBOX_HXX
#include <svtools/brwbox.hxx>
#endif

#ifndef _BRWHEAD_HXX
#include <svtools/brwhead.hxx>
#endif

#ifndef _TIMER_HXX //autogen
#include <vcl/timer.hxx>
#endif
#ifndef _IMAGE_HXX //autogen
#include <vcl/image.hxx>
#endif
#ifndef _LIST_HXX //autogen
#include <tools/list.hxx>
#endif
#ifndef _TRANSFER_HXX
#include <svtools/transfer.hxx>
#endif

//===================================================================

#define MIN_COLUMNWIDTH  2
#define DRAG_CRITICAL    4

DECLARE_LIST( RectangleList, Rectangle* )

//===================================================================

class ButtonFrame
{
    Rectangle   aRect;
    Rectangle   aInnerRect;
    String      aText;
    BOOL        bPressed;
    BOOL        bCurs;
    BOOL        bAbbr;
    BOOL        m_bDrawDisabled;

public:
               ButtonFrame( const Point& rPt, const Size& rSz,
                            const String &rText,
                            BOOL bPress = FALSE,
                            BOOL bCursor = FALSE,
                            BOOL bAbbreviate = TRUE,
                            BOOL _bDrawDisabled = FALSE)
                :aRect( rPt, rSz )
                ,aInnerRect( Point( aRect.Left()+1, aRect.Top()+1 ),
                            Size( aRect.GetWidth()-2, aRect.GetHeight()-2 ) )
                ,aText(rText)
                ,bPressed(bPress)
                ,bCurs(bCursor)
                ,bAbbr(bAbbreviate)
                ,m_bDrawDisabled(_bDrawDisabled)
            {
            }

    void    Draw( OutputDevice& rDev );
};

//===================================================================

class BrowserColumn
{
    USHORT              _nId;
    ULONG               _nOriginalWidth;
    ULONG               _nWidth;
    Image               _aImage;
    String              _aTitle;
    BOOL                _bFrozen;
    HeaderBarItemBits   _nFlags;

public:
                        BrowserColumn( USHORT nItemId, const Image &rImage,
                                        const String& rTitle, ULONG nWidthPixel, const Fraction& rCurrentZoom,
                                        HeaderBarItemBits nFlags );
    virtual            ~BrowserColumn();

    USHORT              GetId() const { return _nId; }

    ULONG               Width() { return _nWidth; }
    Image&              GetImage() { return _aImage; }
    String&             Title() { return _aTitle; }
    HeaderBarItemBits&  Flags() { return _nFlags; }

    BOOL                IsFrozen() const { return _bFrozen; }
    void                Freeze( BOOL bFreeze = TRUE ) { _bFrozen = bFreeze; }

    virtual void        Draw( BrowseBox& rBox, OutputDevice& rDev,
                              const Point& rPos, BOOL bCurs  );

    void                SetWidth(ULONG nNewWidthPixel, const Fraction& rCurrentZoom);
    void                ZoomChanged(const Fraction& rNewZoom);
};

//===================================================================

class BrowserDataWin
            :public Control
            ,public DragSourceHelper
            ,public DropTargetHelper
{
public:
    BrowserHeader*  pHeaderBar;     // only for BROWSER_HEADERBAR_NEW
    Window*         pEventWin;      // Window of forwarded events
    ScrollBarBox*   pCornerWin;     // Window in the corner btw the ScrollBars
    BOOL*           pDtorNotify;
    AutoTimer       aMouseTimer;    // recalls MouseMove on dragging out
    MouseEvent      aRepeatEvt;     // a MouseEvent to repeat
    Point           aLastMousePos;  // verhindert pseudo-MouseMoves

    String          aRealRowCount;  // zur Anzeige im VScrollBar

    RectangleList   aInvalidRegion; // invalidated Rectangles during !UpdateMode
    FASTBOOL        bInPaint;       // TRUE while in Paint
    FASTBOOL        bInCommand;     // TRUE while in Command
    FASTBOOL        bNoScrollBack;  // nur vorwaerts scrollen
    FASTBOOL        bNoHScroll;     // kein horizontaler Scrollbar
    FASTBOOL        bNoVScroll;     // no vertical scrollbar
    FASTBOOL        bAutoHScroll;   // autohide horizontaler Scrollbar
    FASTBOOL        bAutoVScroll;   // autohide horizontaler Scrollbar
    FASTBOOL        bUpdateMode;    // nicht SV-UpdateMode wegen Invalidate()
    FASTBOOL        bAutoSizeLastCol;// last column always fills up window
    FASTBOOL        bHighlightAuto; // new auto-highlight by SetFont() etc.
    FASTBOOL        bResizeOnPaint; // outstanding resize-event
    FASTBOOL        bUpdateOnUnlock;    // Update() while locked
    FASTBOOL        bInUpdateScrollbars;    // Rekursionsschutz
    FASTBOOL        bHadRecursion;          // Rekursion war aufgetreten
    FASTBOOL        bOwnDataChangedHdl;     // dont change colors in DataChanged
    FASTBOOL        bCallingDropCallback;   // we're in a callback to AcceptDrop or ExecuteDrop curently
    USHORT          nUpdateLock;    // lock count, dont call Control::Update()!
    short           nCursorHidden;  // new conuter for DoHide/ShowCursor

    long            m_nDragRowDividerLimit;
    long            m_nDragRowDividerOffset;

public:
                    BrowserDataWin( BrowseBox* pParent );
                    ~BrowserDataWin();

    virtual void    DataChanged( const DataChangedEvent& rDCEvt );
    virtual void    Paint( const Rectangle& rRect );
    virtual void    RequestHelp( const HelpEvent& rHEvt );
    virtual void    Command( const CommandEvent& rEvt );
    virtual void    MouseButtonDown( const MouseEvent& rEvt );
    virtual void    MouseMove( const MouseEvent& rEvt );
                    DECL_LINK( RepeatedMouseMove, void * );

    virtual void    MouseButtonUp( const MouseEvent& rEvt );
    virtual void    KeyInput( const KeyEvent& rEvt );
    virtual void    Tracking( const TrackingEvent& rTEvt );

    // DropTargetHelper overridables
    virtual sal_Int8 AcceptDrop( const AcceptDropEvent& rEvt );
    virtual sal_Int8 ExecuteDrop( const ExecuteDropEvent& rEvt );

    // DragSourceHelper overridables
    virtual void    StartDrag( sal_Int8 _nAction, const Point& _rPosPixel );


    BrowseEvent     CreateBrowseEvent( const Point& rPosPixel );
    void            Repaint();
    BrowseBox*      GetParent() const
                         { return (BrowseBox*) Window::GetParent(); }
    const String&   GetRealRowCount() const { return aRealRowCount; }

    void            SetUpdateMode( BOOL bMode );
    FASTBOOL        GetUpdateMode() const { return bUpdateMode; }
    void            EnterUpdateLock() { ++nUpdateLock; }
    void            LeaveUpdateLock();
    void            Update();
    void            DoOutstandingInvalidations();
    void            Invalidate( USHORT nFlags = 0 );
    void            Invalidate( const Rectangle& rRect, USHORT nFlags = 0 );
    void            Invalidate( const Region& rRegion, USHORT nFlags = 0 )
                    { Control::Invalidate( rRegion, nFlags ); }

protected:
    void            StartRowDividerDrag( const Point& _rStartPos );
    BOOL            ImplRowDividerHitTest( const BrowserMouseEvent& _rEvent );
};

//-------------------------------------------------------------------

inline void BrowserDataWin::Repaint()
{
    if ( GetUpdateMode() )
        Update();
    Paint( Rectangle( Point(), GetOutputSizePixel() ) );
}

//===================================================================

class BrowserScrollBar: public ScrollBar
{
    ULONG           _nTip;
    ULONG           _nLastPos;
    BrowserDataWin* _pDataWin;

public:
                    BrowserScrollBar( Window* pParent, WinBits nStyle,
                                      BrowserDataWin *pDataWin )
                    :   ScrollBar( pParent, nStyle ),
                        _nTip( 0 ),
                        _nLastPos( ULONG_MAX ),
                        _pDataWin( pDataWin )
                    {}
                    //ScrollBar( Window* pParent, const ResId& rResId );

    virtual void    Tracking( const TrackingEvent& rTEvt );
    virtual void    EndScroll();
};

//===================================================================

void InitSettings_Impl( Window *pWin,
         BOOL bFont = TRUE, BOOL bForeground = TRUE, BOOL bBackground = TRUE );

//===================================================================

#ifdef DBG_MI

void DoLog_Impl( const BrowseBox *pThis, const char *pWhat, const char *pWho );
#define LOG(pThis,what,who) DoLog_Impl(pThis,what,who)

#else

#define LOG(pThis,what,who)

#endif


#endif

