/*************************************************************************
 *
 *  $RCSfile: salnativewidgets-gtk.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: hr $ $Date: 2004-10-13 08:57:18 $
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
 *
 ************************************************************************/

#include <plugins/gtk/gtkframe.hxx>
#include <plugins/gtk/gtkdata.hxx>
#include <plugins/gtk/gtkinst.hxx>
#include <plugins/gtk/gtkgdi.hxx>

#include <cstdio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <hash_map>

#ifndef _SV_SALDATA_HXX
#include <saldata.hxx>
#endif

#ifndef _SV_SALDISP_HXX
#include <saldisp.hxx>
#endif


#ifndef _OSL_CONDITN_HXX_
#include <osl/mutex.hxx>
#endif

// initialize statics
BOOL GtkSalGraphics::bThemeChanged = TRUE;

GtkSalGraphics::~GtkSalGraphics()
{
}


using namespace rtl;

/*************************************
 * Cached native widget objects
 *************************************/
static GtkWidget *  gCacheWindow = NULL;
static GtkWidget *  gDumbContainer = NULL;

static GtkWidget *  gBtnWidget          = NULL;
static GtkWidget *  gRadioWidget            = NULL;
static GtkWidget *  gRadioWidgetSibling     = NULL;
static GtkWidget *  gCheckWidget            = NULL;
static GtkWidget *  gScrollHorizWidget      = NULL;
static GtkWidget *  gScrollVertWidget       = NULL;
static GtkWidget *  gArrowWidget            = NULL;
static GtkWidget *  gDropdownWidget     = NULL;
static GtkWidget *  gEditBoxWidget          = NULL;
static GtkWidget *  gSpinButtonWidget       = NULL;
static GtkWidget *  gNotebookWidget     = NULL;
static GtkWidget *  gOptionMenuWidget       = NULL;
static GtkWidget *  gComboWidget            = NULL;
static GtkWidget *  gScrolledWindowWidget   = NULL;

osl::Mutex  * pWidgetMutex;

class NWPixmapCacheList;
static NWPixmapCacheList* gNWPixmapCacheList = NULL;

// Keep a hash table of Widgets->default flags so that we can
// easily and quickly reset each to a default state before using
// them
static std::hash_map<long, guint>   gWidgetDefaultFlags;

static const GtkBorder aDefDefBorder        = { 1, 1, 1, 1 };
static const GtkBorder aDefDefOutsideBorder = { 0, 0, 0, 0 };

// Some GTK defaults
#define MIN_ARROW_SIZE                  11
#define BTN_CHILD_SPACING               1
#define MIN_SPIN_ARROW_WIDTH                6


static void NWEnsureGTKRadio            ( void );
static void NWEnsureGTKButton           ( void );
static void NWEnsureGTKCheck            ( void );
static void NWEnsureGTKScrollbars       ( void );
static void NWEnsureGTKArrow            ( void );
static void NWEnsureGTKEditBox      ( void );
static void NWEnsureGTKSpinButton       ( void );
static void NWEnsureGTKNotebook     ( void );
static void NWEnsureGTKOptionMenu       ( void );
static void NWEnsureGTKCombo            ( void );
static void NWEnsureGTKScrolledWindow   ( void );

static void NWConvertVCLStateToGTKState( ControlState nVCLState, GtkStateType* nGTKState, GtkShadowType* nGTKShadow );
static void NWAddWidgetToCacheWindow( GtkWidget* widget );
static void NWSetWidgetState( GtkWidget* widget, ControlState nState, GtkStateType nGtkState );

static void NWCalcArrowRect( const Rectangle& rButton, Rectangle& rArrow );

/*
 * Individual helper functions
 *
 */

//---
static Rectangle NWGetButtonArea( ControlType nType, ControlPart nPart, Rectangle aAreaRect, ControlState nState,
                                const ImplControlValue& aValue, SalControlHandle& rControlHandle, OUString aCaption );

//---
static Rectangle NWGetEditBoxPixmapRect( ControlType nType, ControlPart nPart, Rectangle aAreaRect, ControlState nState,
                            const ImplControlValue& aValue, SalControlHandle& rControlHandle, OUString aCaption );

static void NWPaintOneEditBox( GdkWindow * gdkDrawable, GdkRectangle *gdkRect,
                               ControlType nType, ControlPart nPart, Rectangle aEditBoxRect,
                               ControlState nState, const ImplControlValue& aValue,
                               SalControlHandle& rControlHandle, OUString aCaption );

//---
static Rectangle NWGetSpinButtonRect( ControlType nType, ControlPart nPart, Rectangle aAreaRect, ControlState nState,
                            const ImplControlValue& aValue, SalControlHandle& rControlHandle, OUString aCaption );

static void NWPaintOneSpinButton( GdkPixmap * pixmap, ControlType nType, ControlPart nPart, Rectangle aAreaRect,
                            ControlState nState, const ImplControlValue& aValue, SalControlHandle& rControlHandle,
                            OUString aCaption );
//---
static Rectangle NWGetComboBoxButtonRect( ControlType nType, ControlPart nPart, Rectangle aAreaRect, ControlState nState,
                            const ImplControlValue& aValue, SalControlHandle& rControlHandle, OUString aCaption );

//---
static Rectangle NWGetListBoxButtonRect( ControlType nType, ControlPart nPart, Rectangle aAreaRect, ControlState nState,
                            const ImplControlValue& aValue, SalControlHandle& rControlHandle, OUString aCaption );

static Rectangle NWGetListBoxIndicatorRect( ControlType nType, ControlPart nPart, Rectangle aAreaRect, ControlState nState,
                            const ImplControlValue& aValue, SalControlHandle& rControlHandle, OUString aCaption );
//---

static Rectangle NWGetScrollButtonRect( ControlPart nPart, Rectangle aAreaRect );
//---

/*********************************************************
 * PixmapCache
 *********************************************************/

// as some native widget drawing operations are pretty slow
// with certain themes (eg tabpages)
// this cache can be used to cache the corresponding pixmap
// see NWPaintGTKTabItem

class NWPixmapCacheData
{
public:
    ControlType m_nType;
    ControlState m_nState;
    Rectangle   m_pixmapRect;
    GdkPixmap*  m_pixmap;

    NWPixmapCacheData()
        { m_pixmap = NULL; };
    ~NWPixmapCacheData()
        { SetPixmap( NULL ); };
    void SetPixmap( GdkPixmap* pPixmap );
};

class NWPixmapCache
{
    int m_size;
    int m_idx;
    NWPixmapCacheData* pData;
public:
    NWPixmapCache();
    ~NWPixmapCache();

    void SetSize( int n)
        { delete [] pData; m_idx = 0; m_size = n; pData = new NWPixmapCacheData[m_size]; }
    int GetSize() { return m_size; }

    BOOL Find( ControlType aType, ControlState aState, const Rectangle& r_pixmapRect, GdkPixmap** pPixmap );
    void Fill( ControlType aType, ControlState aState, const Rectangle& r_pixmapRect, GdkPixmap* pPixmap );

    void ThemeChanged();
};

class NWPixmapCacheList
{
public:
    ::std::vector< NWPixmapCache* > mCaches;

    void AddCache( NWPixmapCache *pCache );
    void RemoveCache( NWPixmapCache *pCache );
    void ThemeChanged();
};

// --- implementation ---

void NWPixmapCacheData::SetPixmap( GdkPixmap* pPixmap )
{
    if( m_pixmap )
        g_object_unref( m_pixmap );

    m_pixmap = pPixmap;

    if( m_pixmap )
        g_object_ref( m_pixmap );
}


NWPixmapCache::NWPixmapCache()
{
    m_idx = 0;
    m_size = 0;
    pData = NULL;
    gNWPixmapCacheList->AddCache(this);
}
NWPixmapCache::~NWPixmapCache()
{
    gNWPixmapCacheList->RemoveCache(this);
    delete[] pData;
}
void NWPixmapCache::ThemeChanged()
{
    // throw away cached pixmaps
    int i;
    for(i=0; i<m_size; i++)
        pData[i].SetPixmap( NULL );
}

BOOL  NWPixmapCache::Find( ControlType aType, ControlState aState, const Rectangle& r_pixmapRect, GdkPixmap** pPixmap )
{
    aState &= ~CTRL_CACHING_ALLOWED; // mask clipping flag
    int i;
    for(i=0; i<m_size; i++)
    {
        if( pData[i].m_nType == aType &&
            pData[i].m_nState == aState &&
            pData[i].m_pixmapRect.getWidth() == r_pixmapRect.getWidth() &&
            pData[i].m_pixmapRect.getHeight() == r_pixmapRect.getHeight() &&
            pData[i].m_pixmap != NULL )
        {
            *pPixmap = pData[i].m_pixmap;
            return TRUE;
        }
    }
    return FALSE;
}

void NWPixmapCache::Fill( ControlType aType, ControlState aState, const Rectangle& r_pixmapRect, GdkPixmap* pPixmap )
{
    if( !(aState & CTRL_CACHING_ALLOWED) )
        return;

    aState &= ~CTRL_CACHING_ALLOWED; // mask clipping flag
    m_idx = (m_idx+1) % m_size; // just wrap
    pData[m_idx].m_nType = aType;
    pData[m_idx].m_nState = aState;
    pData[m_idx].m_pixmapRect = r_pixmapRect;
    pData[m_idx].SetPixmap( pPixmap );
}


void NWPixmapCacheList::AddCache( NWPixmapCache* pCache )
{
    mCaches.push_back( pCache );
}
void NWPixmapCacheList::RemoveCache( NWPixmapCache* pCache )
{
    ::std::vector< NWPixmapCache* >::iterator p;
    p = ::std::find( mCaches.begin(), mCaches.end(), pCache );
    if( p != mCaches.end() )
        mCaches.erase( p );
}
void NWPixmapCacheList::ThemeChanged( )
{
    ::std::vector< NWPixmapCache* >::iterator p = mCaches.begin();
    while( p != mCaches.end() )
    {
        (*p)->ThemeChanged();
        p++;
    }
}


/*********************************************************
 * Make border manipulation easier
 *********************************************************/
inline void NW_gtk_border_set_from_border( GtkBorder& aDst, const GtkBorder * pSrc )
{
    aDst.left       = pSrc->left;
    aDst.top        = pSrc->top;
    aDst.right  = pSrc->right;
    aDst.bottom = pSrc->bottom;
}


/*********************************************************
 * Initialize GTK and local stuff
 *********************************************************/
void GtkData::initNWF( void )
{
    pWidgetMutex = new osl::Mutex;
    gNWPixmapCacheList = new NWPixmapCacheList;
}


/*********************************************************
 * Release GTK and local stuff
 *********************************************************/
void GtkData::deInitNWF( void )
{
    delete pWidgetMutex;
    delete gNWPixmapCacheList;
}

/*
 * IsNativeControlSupported()
 *
 *  Returns TRUE if the platform supports native
 *  drawing of the control defined by nPart
 */
BOOL GtkSalGraphics::IsNativeControlSupported( ControlType nType, ControlPart nPart )
{
    if (
        ((nType==CTRL_PUSHBUTTON)  && (nPart==PART_ENTIRE_CONTROL))     ||
         ((nType==CTRL_RADIOBUTTON) && (nPart==PART_ENTIRE_CONTROL))        ||
        ((nType==CTRL_CHECKBOX)    && (nPart==PART_ENTIRE_CONTROL))     ||
        ((nType==CTRL_SCROLLBAR) &&
                (  (nPart==PART_DRAW_BACKGROUND_HORZ)
                || (nPart==PART_DRAW_BACKGROUND_VERT)
                || (nPart==PART_ENTIRE_CONTROL)
                || (nPart==HAS_THREE_BUTTONS) )                 )   ||
        ((nType==CTRL_EDITBOX) &&
                (  (nPart==PART_ENTIRE_CONTROL)
                || (nPart==HAS_BACKGROUND_TEXTURE) )            )   ||
        ((nType==CTRL_MULTILINE_EDITBOX) &&
                (  (nPart==PART_ENTIRE_CONTROL)
                || (nPart==HAS_BACKGROUND_TEXTURE) )            )   ||
        ((nType==CTRL_SPINBOX) &&
                (  (nPart==PART_ENTIRE_CONTROL)
                || (nPart==PART_ALL_BUTTONS)
                || (nPart==HAS_BACKGROUND_TEXTURE) )            )   ||
        ((nType==CTRL_SPINBUTTONS) &&
                (  (nPart==PART_ENTIRE_CONTROL)
                || (nPart==PART_ALL_BUTTONS)    )               )   ||
        ((nType==CTRL_COMBOBOX) &&
                (  (nPart==PART_ENTIRE_CONTROL)
                || (nPart==HAS_BACKGROUND_TEXTURE)  )           )   ||
        (((nType==CTRL_TAB_ITEM) || (nType==CTRL_TAB_PANE) ||
          (nType==CTRL_TAB_BODY) || (nType==CTRL_FIXEDBORDER)) &&
                (  (nPart==PART_ENTIRE_CONTROL)
                || (nPart==PART_TABS_DRAW_RTL) )                )   ||
        ((nType==CTRL_LISTBOX) &&
                (  (nPart==PART_ENTIRE_CONTROL)
                || (nPart==PART_WINDOW)
                || (nPart==HAS_BACKGROUND_TEXTURE) )            )
       )
        return( TRUE );

    return( FALSE );
}


/*
 * HitTestNativeControl()
 *
 *  bIsInside is set to TRUE if aPos is contained within the
 *  given part of the control, whose bounding region is
 *  given by rControlRegion (in VCL frame coordinates).
 *
 *  returns whether bIsInside was really set.
 */
BOOL GtkSalGraphics::hitTestNativeControl( ControlType      nType,
                                ControlPart     nPart,
                                const Region&       rControlRegion,
                                const Point&        aPos,
                                SalControlHandle&   rControlHandle,
                                BOOL&           rIsInside )
{
    if ( ( nType == CTRL_SCROLLBAR ) &&
         ( ( nPart == PART_BUTTON_UP ) ||
           ( nPart == PART_BUTTON_DOWN ) ||
           ( nPart == PART_BUTTON_LEFT ) ||
           ( nPart == PART_BUTTON_RIGHT ) ) )
    {
        NWEnsureGTKScrollbars();

        // Grab some button style attributes
        gboolean has_forward;
        gboolean has_forward2;
        gboolean has_backward;
        gboolean has_backward2;

        gtk_widget_style_get( gScrollHorizWidget, "has-forward-stepper", &has_forward,
                                        "has-secondary-forward-stepper", &has_forward2,
                                        "has-backward-stepper", &has_backward,
                                           "has-secondary-backward-stepper", &has_backward2, NULL );
        Rectangle aForward;
        Rectangle aBackward;

        rIsInside = FALSE;

        ControlPart nCounterPart;
        if ( nPart == PART_BUTTON_UP )
            nCounterPart = PART_BUTTON_DOWN;
        else if ( nPart == PART_BUTTON_DOWN )
            nCounterPart = PART_BUTTON_UP;
        else if ( nPart == PART_BUTTON_LEFT )
            nCounterPart = PART_BUTTON_RIGHT;
        else if ( nPart == PART_BUTTON_RIGHT )
            nCounterPart = PART_BUTTON_LEFT;

        aBackward = NWGetScrollButtonRect( nPart, rControlRegion.GetBoundRect() );
        aForward = NWGetScrollButtonRect( nCounterPart, rControlRegion.GetBoundRect() );

        if ( has_backward && has_forward2 )
        {
            if ( ( nPart == PART_BUTTON_UP ) || ( nPart == PART_BUTTON_DOWN ) )
                aBackward.setHeight( aBackward.getHeight() / 2 );
            else
                aBackward.setWidth( aBackward.getWidth() / 2 );

            if ( nPart == PART_BUTTON_DOWN )
                aBackward.Move( 0, aBackward.getHeight() / 2 );
            else if ( nPart == PART_BUTTON_RIGHT )
                aBackward.Move( aBackward.getWidth() / 2, 0 );
        }

        if ( has_backward2 && has_forward )
        {
            if ( ( nPart == PART_BUTTON_UP ) || ( nPart == PART_BUTTON_DOWN ) )
                aForward.setHeight( aForward.getHeight() / 2 );
            else
                aForward.setWidth( aForward.getWidth() / 2 );

            if ( nPart == PART_BUTTON_DOWN )
                aForward.Move( 0, aForward.getHeight() / 2 );
            else if ( nPart == PART_BUTTON_RIGHT )
                aForward.Move( aForward.getWidth() / 2, 0 );
        }

        if ( ( nPart == PART_BUTTON_UP ) || ( nPart == PART_BUTTON_LEFT ) )
        {
            if ( has_backward )
                rIsInside |= aBackward.IsInside( aPos );
            if ( has_backward2 )
                rIsInside |= aForward.IsInside( aPos );
        }
        else
        {
            if ( has_forward )
                rIsInside |= aBackward.IsInside( aPos );
            if ( has_forward2 )
                rIsInside |= aForward.IsInside( aPos );
        }
        return ( TRUE );
    }

    if( IsNativeControlSupported(nType, nPart) )
    {
        rIsInside = rControlRegion.IsInside( aPos );
        return( TRUE );
    }
    else
    {
        return( FALSE );
    }
}


/*
 * DrawNativeControl()
 *
 *  Draws the requested control described by nPart/nState.
 *
 *  rControlRegion: The bounding region of the complete control in VCL frame coordinates.
 *  aValue:         An optional value (tristate/numerical/string)
 *  rControlHandle: Carries platform dependent data and is maintained by the SalFrame implementation.
 *  aCaption:   A caption or title string (like button text etc)
 */
BOOL GtkSalGraphics::drawNativeControl( ControlType nType,
                            ControlPart nPart,
                            const Region& rControlRegion,
                            ControlState nState,
                            const ImplControlValue& aValue,
                            SalControlHandle& rControlHandle,
                            OUString aCaption )
{
    BOOL            returnVal = FALSE;
    int             nDepth = GetSalData()->GetDisplay()->GetVisual()->GetDepth();

    // get a GC with current clipping region set
    GC          gc = SelectFont();

    // theme changed ?
    if( GtkSalGraphics::bThemeChanged )
    {
        // invalidate caches
        gNWPixmapCacheList->ThemeChanged();
        GtkSalGraphics::bThemeChanged = FALSE;
    }

    if ( pWidgetMutex->acquire() )
    {
        if ( (nType==CTRL_PUSHBUTTON) && (nPart==PART_ENTIRE_CONTROL) )
        {
            returnVal = NWPaintGTKButton( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( (nType==CTRL_RADIOBUTTON) && (nPart==PART_ENTIRE_CONTROL) )
        {
            returnVal = NWPaintGTKRadio( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( (nType==CTRL_CHECKBOX) && (nPart==PART_ENTIRE_CONTROL) )
        {
            returnVal = NWPaintGTKCheck( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( (nType==CTRL_SCROLLBAR) && ((nPart==PART_DRAW_BACKGROUND_HORZ) || (nPart==PART_DRAW_BACKGROUND_VERT)) )
        {
            returnVal = NWPaintGTKScrollbar( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( ((nType==CTRL_EDITBOX) && ((nPart==PART_ENTIRE_CONTROL) || (nPart==HAS_BACKGROUND_TEXTURE)) )
                || ((nType==CTRL_SPINBOX) && (nPart==HAS_BACKGROUND_TEXTURE))
                || ((nType==CTRL_COMBOBOX) && (nPart==HAS_BACKGROUND_TEXTURE))
                || ((nType==CTRL_LISTBOX) && (nPart==HAS_BACKGROUND_TEXTURE)) )
        {
            returnVal = NWPaintGTKEditBox( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( ((nType==CTRL_MULTILINE_EDITBOX) && ((nPart==PART_ENTIRE_CONTROL) || (nPart==HAS_BACKGROUND_TEXTURE)) ) )
        {
            returnVal = NWPaintGTKEditBox( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( ((nType==CTRL_SPINBOX) || (nType==CTRL_SPINBUTTONS))
                && ((nPart==PART_ENTIRE_CONTROL) || (nPart==PART_ALL_BUTTONS)) )
        {
            returnVal = NWPaintGTKSpinBox( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( (nType == CTRL_COMBOBOX) && (nPart==PART_ENTIRE_CONTROL) )
        {
            returnVal = NWPaintGTKComboBox( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }
        else if ( (nType==CTRL_TAB_ITEM) || (nType==CTRL_TAB_PANE) || (nType==CTRL_TAB_BODY) || (nType==CTRL_FIXEDBORDER) )
        {
            if ( nType == CTRL_TAB_BODY )
                returnVal = TRUE;
            else
                returnVal = NWPaintGTKTabItem( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption);
        }
        else if ( (nType==CTRL_LISTBOX) && ((nPart==PART_ENTIRE_CONTROL) || (nPart==PART_WINDOW)) )
        {
            returnVal = NWPaintGTKListBox( nType, nPart, rControlRegion, nState, aValue, rControlHandle, aCaption );
        }

        pWidgetMutex->release();
    }

    return( returnVal );
}


/*
 * DrawNativeControlText()
 *
 *  OPTIONAL.  Draws the requested text for the control described by nPart/nState.
 *     Used if text not drawn by DrawNativeControl().
 *
 *  rControlRegion: The bounding region of the complete control in VCL frame coordinates.
 *  aValue:         An optional value (tristate/numerical/string)
 *  rControlHandle: Carries platform dependent data and is maintained by the SalFrame implementation.
 *  aCaption:   A caption or title string (like button text etc)
 */
BOOL GtkSalGraphics::drawNativeControlText( ControlType nType,
                                ControlPart nPart,
                                const Region& rControlRegion,
                                ControlState nState,
                                const ImplControlValue& aValue,
                                SalControlHandle& rControlHandle,
                                OUString aCaption )
{
    return( FALSE );
}


/*
 * GetNativeControlRegion()
 *
 *  If the return value is TRUE, rNativeBoundingRegion
 *  contains the TRUE bounding region covered by the control
 *  including any adornment, while rNativeContentRegion contains the area
 *  within the control that can be safely drawn into without drawing over
 *  the borders of the control.
 *
 *  rControlRegion: The bounding region of the control in VCL frame coordinates.
 *  aValue:     An optional value (tristate/numerical/string)
 *  rControlHandle: Carries platform dependent data and is maintained by the SalFrame implementation.
 *  aCaption:       A caption or title string (like button text etc)
 */
BOOL GtkSalGraphics::getNativeControlRegion(  ControlType nType,
                                ControlPart nPart,
                                const Region& rControlRegion,
                                ControlState nState,
                                const ImplControlValue& aValue,
                                SalControlHandle& rControlHandle,
                                OUString aCaption,
                                Region &rNativeBoundingRegion,
                                Region &rNativeContentRegion )
{
    BOOL returnVal = FALSE;

    if ( pWidgetMutex->acquire() )
    {
        if ( (nType==CTRL_PUSHBUTTON) && (nPart==PART_ENTIRE_CONTROL)
                && (rControlRegion.GetBoundRect().GetWidth() > 16)
                && (rControlRegion.GetBoundRect().GetHeight() > 16) )
        {
            rNativeBoundingRegion = NWGetButtonArea( nType, nPart, rControlRegion.GetBoundRect(),
                                     nState, aValue, rControlHandle, aCaption );
            rNativeContentRegion = rControlRegion;

            returnVal = TRUE;
        }
        if ( (nType==CTRL_SPINBOX) && ((nPart==PART_BUTTON_UP) || (nPart==PART_BUTTON_DOWN)) )
        {
            rNativeBoundingRegion = NWGetSpinButtonRect( nType, nPart, rControlRegion.GetBoundRect(),
                                        nState, aValue, rControlHandle, aCaption );
            rNativeContentRegion = rNativeBoundingRegion;

            returnVal = TRUE;
        }
        if ( (nType==CTRL_COMBOBOX) && (nPart==PART_BUTTON_DOWN) )
        {
            rNativeBoundingRegion = NWGetComboBoxButtonRect( nType, nPart, rControlRegion.GetBoundRect(), nState,
                                        aValue, rControlHandle, aCaption );
            rNativeContentRegion = rNativeBoundingRegion;

            returnVal = TRUE;
        }
        if ( (nType==CTRL_SPINBOX) && ((nPart==PART_BUTTON_UP) || (nPart==PART_BUTTON_DOWN)) )
        {
            SpinbuttonValue *   pSpinVal = (SpinbuttonValue *)(aValue.getOptionalVal());

            rNativeBoundingRegion = NWGetSpinButtonRect( nType, nPart, rControlRegion.GetBoundRect(), nState,
                                    aValue, rControlHandle, aCaption );
            rNativeContentRegion = rNativeBoundingRegion;

            returnVal = TRUE;
        }
        if ( (nType==CTRL_LISTBOX) && ((nPart==PART_BUTTON_DOWN) || (nPart==PART_SUB_EDIT)) )
        {
            rNativeBoundingRegion = NWGetListBoxButtonRect( nType, nPart, rControlRegion.GetBoundRect(), nState,
                                        aValue, rControlHandle, aCaption );
            rNativeContentRegion = rNativeBoundingRegion;

            returnVal = TRUE;
        }
        if ( (nType==CTRL_SCROLLBAR) && ((nPart==PART_BUTTON_LEFT) || (nPart==PART_BUTTON_RIGHT) ||
                                         (nPart==PART_BUTTON_UP) || (nPart==PART_BUTTON_DOWN)  ) )
        {
            rNativeBoundingRegion = NWGetScrollButtonRect( nPart, rControlRegion.GetBoundRect() );
            rNativeContentRegion = rNativeBoundingRegion;

            returnVal = TRUE;
        }

        pWidgetMutex->release();
    }

    return( returnVal );
}


/************************************************************************
 * Individual control drawing functions
 ************************************************************************/
BOOL GtkSalGraphics::NWPaintGTKButton( ControlType nType, ControlPart nPart,
            const Region& rControlRegion, ControlState nState, const ImplControlValue& aValue,
            SalControlHandle& rControlHandle, OUString aCaption )
{
    GdkPixmap * pixmap = NULL;
    Rectangle       pixmapRect;
    Rectangle       buttonRect;
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    gboolean        interiorFocus;
    gint            focusWidth;
    gint            focusPad;
    BOOL            bDrawFocus = TRUE;
    gint            x, y, w, h;
    GtkBorder       aDefBorder;
    GtkBorder       aDefOutsideBorder;
    GtkBorder*      pBorder;
    GtkBorder*      pOutBorder;
    GdkRectangle    clipRect;

    NWEnsureGTKButton();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    BOOL bUseWindow = (GetNumCliprects() == 1);

    // Find the overall bounding rect of the buttons's drawing area,
    // plus its actual draw rect excluding adornment
    pixmapRect = rControlRegion.GetBoundRect();
    x = y = 0;
    w = pixmapRect.getWidth();
    h = pixmapRect.getHeight();

    // Grab some button style attributes
    gtk_widget_style_get( gBtnWidget,   "focus-line-width", &focusWidth,
                                "focus-padding",    &focusPad,
                                 "interior_focus",  &interiorFocus,
                                "default_border",   &pBorder,
                                "default_outside_border", &pOutBorder, NULL );

    // Make sure the border values exist, otherwise use some defaults
    if ( pBorder )
    {
        NW_gtk_border_set_from_border( aDefBorder, pBorder );
        gtk_border_free( pBorder );
    }
    else NW_gtk_border_set_from_border( aDefBorder, &aDefDefBorder );

    if ( pOutBorder )
    {
        NW_gtk_border_set_from_border( aDefOutsideBorder, pOutBorder );
        gtk_border_free( pOutBorder );
    }
    else NW_gtk_border_set_from_border( aDefOutsideBorder, &aDefDefOutsideBorder );

    // If the button is too small, don't ever draw focus or grab more space
    if ( (w < 16) || (h < 16) )
        bDrawFocus = FALSE;

    if( !bUseWindow )
    {
        pixmap = NWGetPixmapFromScreen( pixmapRect );
        if ( !pixmap )
            return( FALSE );
    }
    else
    {
        clipRect.x = GetBoundingCliprect().Left();
        clipRect.y = GetBoundingCliprect().Top();
        clipRect.width = GetBoundingCliprect().getWidth();
        clipRect.height = GetBoundingCliprect().getHeight();
        x = pixmapRect.Left();
        y = pixmapRect.Top();
    }

    // set up references to correct drawable and cliprect
    GdkDrawable* const &gdkDrawable = GDK_DRAWABLE( bUseWindow ? GetGdkWindow() : pixmap );
    GdkRectangle* const &gdkRect    = bUseWindow ? &clipRect : NULL;

    NWSetWidgetState( gBtnWidget, nState, stateType );

    // Buttons must paint opaque since some themes have alpha-channel enabled buttons
    gtk_paint_flat_box( gBtnWidget->style, gdkDrawable, GTK_STATE_NORMAL, GTK_SHADOW_NONE,
                        gdkRect, gBtnWidget, "base", x, y, w, h );

    if ( (nState & CTRL_STATE_DEFAULT) && (GTK_BUTTON(gBtnWidget)->relief == GTK_RELIEF_NORMAL) )
    {
        gtk_paint_box( gBtnWidget->style, gdkDrawable, GTK_STATE_NORMAL, GTK_SHADOW_IN,
                       gdkRect, gBtnWidget, "buttondefault", x, y, w, h );
    }

    if ( (nState & CTRL_STATE_DEFAULT) && bDrawFocus )
    {
        x += aDefBorder.left;
        y += aDefBorder.top;
        w -= aDefBorder.left + aDefBorder.right;
        h -= aDefBorder.top + aDefBorder.bottom;
    }

    if ( !interiorFocus && bDrawFocus )
    {
        x += focusWidth + focusPad;
        y += focusWidth + focusPad;
        w -= 2 * (focusWidth + focusPad);
        h -= 2 * (focusWidth + focusPad);
    }

    if ( (GTK_BUTTON(gBtnWidget)->relief != GTK_RELIEF_NONE)
        || (nState & CTRL_STATE_PRESSED)
        || (nState & CTRL_STATE_ROLLOVER) )
    {
        gtk_paint_box( gBtnWidget->style, gdkDrawable, stateType, shadowType,
                       gdkRect, gBtnWidget, "button", x, y, w, h );
    }

    // Draw focus rect
#if 0 // VCL draws focus rects
    if ( (nState & CTRL_STATE_FOCUSED) && (nState & CTRL_STATE_ENABLED) && bDrawFocus )
    {
        if (interiorFocus)
        {
            x += gBtnWidget->style->xthickness + focusPad;
            y += gBtnWidget->style->ythickness + focusPad;
            w -= 2 * (gBtnWidget->style->xthickness + focusPad);
            h -=  2 * (gBtnWidget->style->xthickness + focusPad);
        }
        else
        {
            x -= focusWidth + focusPad;
            y -= focusWidth + focusPad;
            w += 2 * (focusWidth + focusPad);
            h += 2 * (focusWidth + focusPad);
        }
        if ( !interiorFocus )
            gtk_paint_focus( gBtnWidget->style, gdkDrawable, stateType, gdkRect,
                             gBtnWidget, "button", x, y, w, h );
    }
#endif

    if( !bUseWindow )
    {
        if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
        {
            g_object_unref( pixmap );
            return( FALSE );
        }
        g_object_unref( pixmap );
    }

    return( TRUE );
}

static Rectangle NWGetButtonArea( ControlType nType, ControlPart nPart, Rectangle aAreaRect, ControlState nState,
                            const ImplControlValue& aValue, SalControlHandle& rControlHandle, OUString aCaption )
{
    gboolean        interiorFocus;
    gint            focusWidth;
    gint            focusPad;
    GtkBorder       aDefBorder;
    GtkBorder       aDefOutsideBorder;
    GtkBorder * pBorder;
    GtkBorder * pOutBorder;
    BOOL            bDrawFocus = TRUE;
    Rectangle       aRect;
    gint            x, y, w, h;

    NWEnsureGTKButton();
    gtk_widget_style_get( gBtnWidget,   "focus-line-width", &focusWidth,
                                "focus-padding",    &focusPad,
                                 "interior_focus",  &interiorFocus,
                                "default_border",   &pBorder,
                                "default_outside_border", &pOutBorder, NULL );

    // Make sure the border values exist, otherwise use some defaults
    if ( pBorder )
    {
        NW_gtk_border_set_from_border( aDefBorder, pBorder );
        gtk_border_free( pBorder );
    }
    else NW_gtk_border_set_from_border( aDefBorder, &aDefDefBorder );

    if ( pOutBorder )
    {
        NW_gtk_border_set_from_border( aDefOutsideBorder, pOutBorder );
        gtk_border_free( pOutBorder );
    }
    else NW_gtk_border_set_from_border( aDefOutsideBorder, &aDefDefOutsideBorder );

    x = aAreaRect.getX();
    y = aAreaRect.getY();
    w = aAreaRect.getWidth();
    h = aAreaRect.getHeight();

    // If the button is too small, don't ever draw focus or grab more space
    if ( (w < 16) || (h < 16) )
        bDrawFocus = FALSE;

    if ( (nState & CTRL_STATE_DEFAULT) && bDrawFocus )
    {
        x -= aDefBorder.left;
        y -= aDefBorder.top;
        w += aDefBorder.left + aDefBorder.right;
        h += aDefBorder.top + aDefBorder.bottom;
    }

    aRect.setX( x );
    aRect.setY( y );
    aRect.setWidth( w );
    aRect.setHeight( h );

    return( aRect );
}

//-------------------------------------

BOOL GtkSalGraphics::NWPaintGTKRadio( ControlType nType, ControlPart nPart,
                                      const Region& rControlRegion, ControlState nState,
                                      const ImplControlValue& aValue, SalControlHandle& rControlHandle,
                                      OUString aCaption )
{
#define RADIO_BUTTON_MINIMUMSIZE   14 // found by experiment, TODO: should be checked at runtime if possible

    GdkPixmap * pixmap = NULL;
    Rectangle       pixmapRect;
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    BOOL            isChecked = (aValue.getTristateVal()==BUTTONVALUE_ON);
    gboolean        interiorFocus;
    gint            focusWidth;
    gint            focusPad;
    gint            x, y;
    GdkRectangle    clipRect;

    NWEnsureGTKButton();
    NWEnsureGTKRadio();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    BOOL bUseWindow = (GetNumCliprects() == 1);

    // Find the overall bounding rect of the control
    pixmapRect = rControlRegion.GetBoundRect();
    x = y = 0;

    // assure minimum button size required by gtk+ to avoid clipping
    if( pixmapRect.getWidth() < RADIO_BUTTON_MINIMUMSIZE )
        pixmapRect.setWidth( RADIO_BUTTON_MINIMUMSIZE );
    if( pixmapRect.getHeight() < RADIO_BUTTON_MINIMUMSIZE )
        pixmapRect.setHeight( RADIO_BUTTON_MINIMUMSIZE );

    // Set the shadow based on if checked or not so we get a freakin checkmark.
    shadowType = isChecked ? GTK_SHADOW_IN : GTK_SHADOW_OUT;
    NWSetWidgetState( gRadioWidget, nState, stateType );
    NWSetWidgetState( gRadioWidgetSibling, nState, stateType );

    // GTK enforces radio groups, so that if we don't have 2 buttons in the group,
    // the single button will always be active.  So we have to have 2 buttons.
    if (!isChecked)
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(gRadioWidgetSibling), TRUE );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(gRadioWidget), isChecked );

    if( !bUseWindow )
    {
        pixmap = NWGetPixmapFromScreen( pixmapRect );
        if ( !pixmap )
            return( FALSE );
        x = y = 0;
    }
    else
    {
        clipRect.x = GetBoundingCliprect().Left();
        clipRect.y = GetBoundingCliprect().Top();
        clipRect.width = GetBoundingCliprect().getWidth();
        clipRect.height = GetBoundingCliprect().getHeight();
        x = pixmapRect.Left();
        y = pixmapRect.Top();
    }

    // set up references to correct drawable and cliprect
    GdkDrawable* const &gdkDrawable = GDK_DRAWABLE( bUseWindow ? GetGdkWindow() : pixmap );
    GdkRectangle* const &gdkRect    = bUseWindow ? &clipRect : NULL;

    gtk_paint_option( gRadioWidget->style, gdkDrawable, stateType, shadowType,
                      gdkRect, gRadioWidget, "radiobutton",
                      x, y, pixmapRect.getWidth(), pixmapRect.getHeight() );

    if( !bUseWindow )
    {
        if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
        {
            g_object_unref( pixmap );
            return( FALSE );
        }
        g_object_unref( pixmap );
    }

    return( TRUE );
}

//-------------------------------------

BOOL GtkSalGraphics::NWPaintGTKCheck( ControlType nType, ControlPart nPart,
                                      const Region& rControlRegion, ControlState nState,
                                      const ImplControlValue& aValue,
                                      SalControlHandle& rControlHandle, OUString aCaption )
{
#define CHECKBOX_MINIMUMSIZE   14 // found by experiment, TODO: should be checked at runtime if possible

    GdkPixmap * pixmap = NULL;
    Rectangle       pixmapRect;
    Rectangle       btnBoundRect;
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    BOOL            isChecked = (aValue.getTristateVal()==BUTTONVALUE_ON) ? TRUE : FALSE;
    gint            ctrlSize;
    GdkRectangle    clipRect;
    gint            x,y;

    NWEnsureGTKButton();
    NWEnsureGTKCheck();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    BOOL bUseWindow = (GetNumCliprects() == 1);

    // Find the overall bounding rect of the control
    pixmapRect = rControlRegion.GetBoundRect();

    // assure minimum button size required by gtk+ to avoid clipping
    if( pixmapRect.getWidth() < CHECKBOX_MINIMUMSIZE )
        pixmapRect.setWidth( CHECKBOX_MINIMUMSIZE );
    if( pixmapRect.getHeight() < CHECKBOX_MINIMUMSIZE )
        pixmapRect.setHeight( CHECKBOX_MINIMUMSIZE );

    btnBoundRect = pixmapRect;

    // Set the shadow based on if checked or not so we get a checkmark.
    shadowType = isChecked ? GTK_SHADOW_IN : GTK_SHADOW_OUT;
    NWSetWidgetState( gCheckWidget, nState, stateType );
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON(gCheckWidget), isChecked );

    if( !bUseWindow )
    {
        pixmap = NWGetPixmapFromScreen( pixmapRect );
        if ( !pixmap )
            return( FALSE );
        x = btnBoundRect.getX() - pixmapRect.getX();
        y = btnBoundRect.getY() - pixmapRect.getY();
    }
    else
    {
        clipRect.x = GetBoundingCliprect().Left();
        clipRect.y = GetBoundingCliprect().Top();
        clipRect.width = GetBoundingCliprect().getWidth();
        clipRect.height = GetBoundingCliprect().getHeight();
        x = btnBoundRect.Left();
        y = btnBoundRect.Top();
    }

    // set up references to correct drawable and cliprect
    GdkDrawable* const &gdkDrawable = GDK_DRAWABLE( bUseWindow ? GetGdkWindow() : pixmap );
    GdkRectangle* const &gdkRect    = bUseWindow ? &clipRect : NULL;

    gtk_paint_check( gCheckWidget->style, gdkDrawable, stateType, shadowType,
                     gdkRect, gCheckWidget, "checkbutton",
                     x, y, btnBoundRect.getWidth(), btnBoundRect.getHeight() );

    if( !bUseWindow )
    {
        if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
        {
            g_object_unref( pixmap );
            return( FALSE );
        }
        g_object_unref( pixmap );
    }
    return( TRUE );
}

//-------------------------------------
static void NWCalcArrowRect( const Rectangle& rButton, Rectangle& rArrow )
{
    // Size the arrow appropriately
    rArrow.setWidth ( rButton.getWidth()  / 2 );
    rArrow.setHeight( rButton.getHeight() / 2 );

    rArrow.setX( rButton.getX() + ( rButton.getWidth()  - rArrow.getWidth()  ) / 2 );
    rArrow.setY( rButton.getY() + ( rButton.getHeight() - rArrow.getHeight() ) / 2 );
}

BOOL GtkSalGraphics::NWPaintGTKScrollbar( ControlType nType, ControlPart nPart,
                                          const Region& rControlRegion, ControlState nState,
                                          const ImplControlValue& aValue, SalControlHandle& rControlHandle,
                                          OUString aCaption )
{
    ScrollbarValue* pScrollbarVal = (ScrollbarValue *)(aValue.getOptionalVal());
    GdkPixmap   *   pixmap = NULL;
    Rectangle       pixmapRect;
    Rectangle       scrollbarRect;
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    GtkScrollbar *  scrollbarWidget;
    GtkStyle *  style;
    GtkAdjustment* scrollbarValues = NULL;
    GtkOrientation  scrollbarOrientation;
    Rectangle       thumbRect = pScrollbarVal->maThumbRect;
    Rectangle       button11BoundRect = pScrollbarVal->maButton1Rect;   // backward
    Rectangle       button22BoundRect = pScrollbarVal->maButton2Rect;   // forward
    Rectangle       button12BoundRect = pScrollbarVal->maButton1Rect;   // secondary forward
    Rectangle       button21BoundRect = pScrollbarVal->maButton2Rect;   // secondary backward
    GtkArrowType    button1Type;                                        // backward
    GtkArrowType    button2Type;                                        // forward
    gchar *     scrollbarTagH = (gchar *) "hscrollbar";
    gchar *     scrollbarTagV = (gchar *) "vscrollbar";
    gchar *     scrollbarTag = NULL;
    Rectangle       arrowRect;
    gint            slider_width = 0;
    gint            stepper_size = 0;
    gint            stepper_spacing = 0;
    gint            trough_border = 0;
    gint            min_slider_length = 0;
    gint            vShim = 0;
    gint            hShim = 0;
    gint            x,y;
    GdkRectangle    clipRect;

    NWEnsureGTKButton();
    NWEnsureGTKScrollbars();
    NWEnsureGTKArrow();

    // as multiple paints are required for the scrollbar
    // painting them directly to the window flickers
    BOOL bUseWindow = FALSE; //(GetNumCliprects() == 1);

    // Find the overall bounding rect of the control
    pixmapRect = rControlRegion.GetBoundRect();
    pixmapRect.setWidth( pixmapRect.getWidth() + 1 );
    pixmapRect.setHeight( pixmapRect.getHeight() + 1 );
    scrollbarRect = pixmapRect;

    if ( (scrollbarRect.getWidth() <= 1) || (scrollbarRect.getHeight() <= 1) )
        return( TRUE );

    // Grab some button style attributes
    gtk_widget_style_get( gScrollHorizWidget, "slider_width", &slider_width,
                                      "stepper_size", &stepper_size,
                                      "trough_border", &trough_border,
                                      "stepper_spacing", &stepper_spacing,
                                      "min_slider_length", &min_slider_length, NULL );
    gboolean has_forward;
    gboolean has_forward2;
    gboolean has_backward;
    gboolean has_backward2;

    gtk_widget_style_get( gScrollHorizWidget, "has-forward-stepper", &has_forward,
                                      "has-secondary-forward-stepper", &has_forward2,
                                      "has-backward-stepper", &has_backward,
                                         "has-secondary-backward-stepper", &has_backward2, NULL );
    gint magic = trough_border ? 1 : 0;
    gint nFirst = 0;

    if ( has_backward )  nFirst  += 1;
    if ( has_forward2 )  nFirst  += 1;

    if ( nPart == PART_DRAW_BACKGROUND_HORZ )
    {
        unsigned int sliderHeight = slider_width + (trough_border * 2);
        vShim = (pixmapRect.getHeight() - sliderHeight) / 2;

        if ( sliderHeight < scrollbarRect.getHeight() );
        {
            scrollbarRect.Move( 0, vShim );
            scrollbarRect.setHeight( sliderHeight );
        }

        scrollbarWidget = GTK_SCROLLBAR( gScrollHorizWidget );
        scrollbarOrientation = GTK_ORIENTATION_HORIZONTAL;
        scrollbarTag = scrollbarTagH;
        button1Type = GTK_ARROW_LEFT;
        button2Type = GTK_ARROW_RIGHT;

        if ( has_backward )
        {
            button12BoundRect.Move( stepper_size - trough_border,
                                    (scrollbarRect.getHeight() - slider_width) / 2 );
        }

        button11BoundRect.Move( trough_border, (scrollbarRect.getHeight() - slider_width) / 2 );
        button11BoundRect.setHeight( slider_width );
        button11BoundRect.setWidth( stepper_size );
        button12BoundRect.setHeight( slider_width );
        button12BoundRect.setWidth( stepper_size );

        if ( has_backward2 )
        {
            button22BoundRect.Move( stepper_size+(trough_border+1)/2, (scrollbarRect.getHeight() - slider_width) / 2 );
            button21BoundRect.Move( (trough_border+1)/2, (scrollbarRect.getHeight() - slider_width) / 2 );
        }
        else
        {
            button22BoundRect.Move( (trough_border+1)/2, (scrollbarRect.getHeight() - slider_width) / 2 );
        }

        button21BoundRect.setHeight( slider_width );
        button21BoundRect.setWidth( stepper_size );
        button22BoundRect.setHeight( slider_width );
        button22BoundRect.setWidth( stepper_size );

        thumbRect.setHeight( slider_width );
        // Make sure the thumb is at least the default width (so we don't get tiny thumbs),
        // but if the VCL gives us a size smaller than the theme's default thumb size,
        // honor the VCL size
#if 0
        if ( (thumbRect.getWidth() < min_slider_length)
            && ((scrollbarRect.getWidth()-button1BoundRect.getWidth()-button2BoundRect.getWidth()) > min_slider_length) )
            thumbRect.setWidth( min_slider_length );
#endif

        thumbRect.setWidth( thumbRect.getWidth() + magic );
        // Center vertically in the track
        thumbRect.Move( 0, (scrollbarRect.getHeight() - slider_width) / 2 );
    }
    else
    {
        unsigned int sliderWidth = slider_width + (trough_border * 2);
        hShim = (pixmapRect.getWidth() - sliderWidth) / 2;

        if ( sliderWidth < scrollbarRect.getWidth() );
        {
            scrollbarRect.Move( hShim, 0 );
            scrollbarRect.setWidth( sliderWidth );
        }

        scrollbarWidget = GTK_SCROLLBAR( gScrollVertWidget );
        scrollbarOrientation = GTK_ORIENTATION_VERTICAL;
        scrollbarTag = scrollbarTagV;
        button1Type = GTK_ARROW_UP;
        button2Type = GTK_ARROW_DOWN;

        if ( has_backward )
        {
            button12BoundRect.Move( (scrollbarRect.getWidth() - slider_width) / 2,
                                    stepper_size + trough_border );
        }
        button11BoundRect.Move( (scrollbarRect.getWidth() - slider_width) / 2, trough_border );
        button11BoundRect.setHeight( stepper_size );
        button11BoundRect.setWidth( slider_width );
        button12BoundRect.setHeight( stepper_size );
        button12BoundRect.setWidth( slider_width );

        if ( has_backward2 )
        {
            button22BoundRect.Move( (scrollbarRect.getWidth() - slider_width) / 2, stepper_size+(trough_border+1)/2 );
            button21BoundRect.Move( (scrollbarRect.getWidth() - slider_width) / 2, (trough_border+1)/2 );
        }
        else
        {
            button22BoundRect.Move( (scrollbarRect.getWidth() - slider_width) / 2, (trough_border+1)/2 );
        }

        button21BoundRect.setHeight( stepper_size );
        button21BoundRect.setWidth( slider_width );
        button22BoundRect.setHeight( stepper_size );
        button22BoundRect.setWidth( slider_width );

        thumbRect.setWidth( slider_width );
#if 0
        // Make sure the thumb is at least the default width (so we don't get tiny thumbs),
        // but if the VCL gives us a size smaller than the theme's default thumb size,
        // honor the VCL size
        if ( (thumbRect.getHeight() < min_slider_length)
            && ((scrollbarRect.getHeight()-button1BoundRect.getHeight()-button2BoundRect.getHeight()) > min_slider_length) )
            thumbRect.setHeight( min_slider_length );
#endif

        thumbRect.setHeight( thumbRect.getHeight() + magic );
        // Center horizontally in the track
        thumbRect.Move( (scrollbarRect.getWidth() - slider_width) / 2, 0 );
    }

    BOOL has_slider = ! ( thumbRect.IsEmpty() );

    scrollbarValues = gtk_range_get_adjustment( GTK_RANGE(scrollbarWidget) );
    if ( scrollbarValues == NULL )
        scrollbarValues = GTK_ADJUSTMENT( gtk_adjustment_new(0, 0, 0, 0, 0, 0) );
    if ( nPart == PART_DRAW_BACKGROUND_HORZ )
    {
        scrollbarValues->lower = pScrollbarVal->mnMin;
        scrollbarValues->upper = pScrollbarVal->mnMax;
        scrollbarValues->value = pScrollbarVal->mnCur;
        scrollbarValues->page_size = scrollbarRect.getWidth() / 2;
    }
    else
    {
        scrollbarValues->lower = pScrollbarVal->mnMin;
        scrollbarValues->upper = pScrollbarVal->mnMax;
        scrollbarValues->value = pScrollbarVal->mnCur;
        scrollbarValues->page_size = scrollbarRect.getHeight() / 2;
    }
    gtk_adjustment_changed( scrollbarValues );

    if( !bUseWindow )
    {
        pixmap = NWGetPixmapFromScreen( pixmapRect );
        if ( !pixmap )
            return( FALSE );
        x = y = 0;
    }
    else
    {
        clipRect.x = GetBoundingCliprect().Left();
        clipRect.y = GetBoundingCliprect().Top();
        clipRect.width = GetBoundingCliprect().getWidth();
        clipRect.height = GetBoundingCliprect().getHeight();
        x = pixmapRect.Left();
        y = pixmapRect.Top();
    }
    // set up references to correct drawable and cliprect
    GdkDrawable* const &gdkDrawable = GDK_DRAWABLE( bUseWindow ? GetGdkWindow() : pixmap );
    GdkRectangle* const &gdkRect    = bUseWindow ? &clipRect : NULL;

    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );
    NWSetWidgetState( GTK_WIDGET(scrollbarWidget), nState, stateType );
    NWSetWidgetState( gBtnWidget, nState, stateType );
    style = GTK_WIDGET( scrollbarWidget )->style;

    // ----------------- TROUGH
    gtk_paint_flat_box( gBtnWidget->style, gdkDrawable,
                        GTK_STATE_NORMAL, GTK_SHADOW_NONE, gdkRect,
                        gBtnWidget, "base", x, y,
                        pixmapRect.getWidth(), pixmapRect.getHeight() );
    gtk_paint_box( style, gdkDrawable, GTK_STATE_ACTIVE, GTK_SHADOW_IN,
                   gdkRect, GTK_WIDGET(scrollbarWidget), "trough",
                   x+(scrollbarRect.getX() - pixmapRect.getX()),
                   y+(scrollbarRect.getY()-pixmapRect.getY()),
                   scrollbarRect.getWidth(), scrollbarRect.getHeight() );

    if ( nState & CTRL_STATE_FOCUSED )
    {
        gtk_paint_focus( style, gdkDrawable, GTK_STATE_ACTIVE,
                         gdkRect, GTK_WIDGET(scrollbarWidget), "trough",
                         x+(scrollbarRect.getX() - pixmapRect.getX()),
                         y+(scrollbarRect.getY()-pixmapRect.getY()),
                         scrollbarRect.getWidth(), scrollbarRect.getHeight() );
    }

    // ----------------- THUMB
    if ( has_slider )
    {
        NWConvertVCLStateToGTKState( pScrollbarVal->mnThumbState, &stateType, &shadowType );
        if ( pScrollbarVal->mnThumbState & CTRL_STATE_PRESSED )  stateType = GTK_STATE_PRELIGHT;
        gtk_paint_slider( style, gdkDrawable, stateType, GTK_SHADOW_OUT,
                        gdkRect, GTK_WIDGET(scrollbarWidget), "slider",
                        x+hShim+thumbRect.getX(), y+vShim+thumbRect.getY(),
                        thumbRect.getWidth(), thumbRect.getHeight(), scrollbarOrientation );
    }
    // ----------------- BUTTON 1 //
    if ( has_backward )
    {
        NWConvertVCLStateToGTKState( pScrollbarVal->mnButton1State, &stateType, &shadowType );
        if ( stateType == GTK_STATE_INSENSITIVE )   stateType = GTK_STATE_NORMAL;
        gtk_paint_box( style, gdkDrawable, stateType, shadowType,
                       gdkRect, GTK_WIDGET(scrollbarWidget), "stepper",
                       x+hShim+button11BoundRect.getX(), y+vShim+button11BoundRect.getY(),
                       button11BoundRect.getWidth(), button11BoundRect.getHeight() );
        // ----------------- ARROW 1
        NWCalcArrowRect( button11BoundRect, arrowRect );
        gtk_paint_arrow( style, gdkDrawable, stateType, shadowType,
                         gdkRect, GTK_WIDGET(scrollbarWidget), scrollbarTag, button1Type, TRUE,
                         x+hShim+arrowRect.getX(), y+vShim+arrowRect.getY(),
                         arrowRect.getWidth(), arrowRect.getHeight() );
    }
    if ( has_forward2 )
    {
        NWConvertVCLStateToGTKState( pScrollbarVal->mnButton2State, &stateType, &shadowType );
        if ( stateType == GTK_STATE_INSENSITIVE )   stateType = GTK_STATE_NORMAL;
        gtk_paint_box( style, gdkDrawable, stateType, shadowType,
                       gdkRect, GTK_WIDGET(scrollbarWidget), "stepper",
                       x+hShim+button12BoundRect.getX(), y+vShim+button12BoundRect.getY(),
                       button12BoundRect.getWidth(), button12BoundRect.getHeight() );
        // ----------------- ARROW 1
        NWCalcArrowRect( button12BoundRect, arrowRect );
        gtk_paint_arrow( style, gdkDrawable, stateType, shadowType,
                         gdkRect, GTK_WIDGET(scrollbarWidget), scrollbarTag, button2Type, TRUE,
                         x+hShim+arrowRect.getX(), y+vShim+arrowRect.getY(),
                         arrowRect.getWidth(), arrowRect.getHeight() );
    }
    // ----------------- BUTTON 2
    if ( has_backward2 )
    {
        NWConvertVCLStateToGTKState( pScrollbarVal->mnButton1State, &stateType, &shadowType );
        if ( stateType == GTK_STATE_INSENSITIVE )   stateType = GTK_STATE_NORMAL;
        gtk_paint_box( style, gdkDrawable, stateType, shadowType, gdkRect,
                       GTK_WIDGET(scrollbarWidget), "stepper",
                       x+hShim+button21BoundRect.getX(), y+vShim+button21BoundRect.getY(),
                       button21BoundRect.getWidth(), button21BoundRect.getHeight() );
        // ----------------- ARROW 2
        NWCalcArrowRect( button21BoundRect, arrowRect );
        gtk_paint_arrow( style, gdkDrawable, stateType, shadowType,
                         gdkRect, GTK_WIDGET(scrollbarWidget), scrollbarTag, button1Type, TRUE,
                         x+hShim+arrowRect.getX(), y+vShim+arrowRect.getY(),
                         arrowRect.getWidth(), arrowRect.getHeight() );
    }
    if ( has_forward )
    {
        NWConvertVCLStateToGTKState( pScrollbarVal->mnButton2State, &stateType, &shadowType );
        if ( stateType == GTK_STATE_INSENSITIVE )   stateType = GTK_STATE_NORMAL;
        gtk_paint_box( style, gdkDrawable, stateType, shadowType, gdkRect,
                       GTK_WIDGET(scrollbarWidget), "stepper",
                       x+hShim+button22BoundRect.getX(), y+vShim+button22BoundRect.getY(),
                       button22BoundRect.getWidth(), button22BoundRect.getHeight() );
        // ----------------- ARROW 2
        NWCalcArrowRect( button22BoundRect, arrowRect );
        gtk_paint_arrow( style, gdkDrawable, stateType, shadowType,
                         gdkRect, GTK_WIDGET(scrollbarWidget), scrollbarTag, button2Type, TRUE,
                         x+hShim+arrowRect.getX(), y+vShim+arrowRect.getY(),
                         arrowRect.getWidth(), arrowRect.getHeight() );
    }

    if( !bUseWindow )
    {
        if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
        {
            g_object_unref( pixmap );
            return( FALSE );
        }
        g_object_unref( pixmap );
    }
    return( TRUE );
}

//---

static Rectangle NWGetScrollButtonRect( ControlPart nPart, Rectangle aAreaRect )
{
    gint slider_width;
    gint stepper_size;
    gint stepper_spacing;
    gint trough_border;

    NWEnsureGTKScrollbars();

    // Grab some button style attributes
    gtk_widget_style_get( gScrollHorizWidget, "slider-width", &slider_width,
                                      "stepper-size", &stepper_size,
                                      "trough-border", &trough_border,
                                         "stepper-spacing", &stepper_spacing, NULL );

    gboolean has_forward;
    gboolean has_forward2;
    gboolean has_backward;
    gboolean has_backward2;

    gtk_widget_style_get( gScrollHorizWidget, "has-forward-stepper", &has_forward,
                                      "has-secondary-forward-stepper", &has_forward2,
                                      "has-backward-stepper", &has_backward,
                                         "has-secondary-backward-stepper", &has_backward2, NULL );
    gint       buttonWidth;
    gint       buttonHeight;
    Rectangle  buttonRect;

    gint nFirst = 0;
    gint nSecond = 0;

    if ( has_forward )   nSecond += 1;
    if ( has_forward2 )  nFirst  += 1;
    if ( has_backward )  nFirst  += 1;
    if ( has_backward2 ) nSecond += 1;

    if ( ( nPart == PART_BUTTON_UP ) || ( nPart == PART_BUTTON_DOWN ) )
    {
        buttonWidth = slider_width + 2 * trough_border;
        buttonHeight = stepper_size + trough_border + stepper_spacing;
    }
    else
    {
        buttonWidth = stepper_size + trough_border + stepper_spacing;
        buttonHeight = slider_width + 2 * trough_border;
    }

    if ( nPart == PART_BUTTON_UP )
    {
        buttonHeight *= nFirst;
        buttonHeight -= 1;
        buttonRect.setX( aAreaRect.getX() );
        buttonRect.setY( aAreaRect.getY() );
    }
    else if ( nPart == PART_BUTTON_LEFT )
    {
        buttonWidth *= nFirst;
        buttonWidth -= 1;
        buttonRect.setX( aAreaRect.getX() );
        buttonRect.setY( aAreaRect.getY() );
    }
    else if ( nPart == PART_BUTTON_DOWN )
    {
        buttonHeight *= nSecond;
        buttonRect.setX( aAreaRect.getX() );
        buttonRect.setY( aAreaRect.getY() + aAreaRect.getHeight() - buttonHeight );
    }
    else if ( nPart == PART_BUTTON_RIGHT )
    {
        buttonWidth *= nSecond;
        buttonRect.setX( aAreaRect.getX() + aAreaRect.getWidth() - buttonWidth );
        buttonRect.setY( aAreaRect.getY() );
    }

    buttonRect.setWidth( buttonWidth );
    buttonRect.setHeight( buttonHeight );

    return( buttonRect );
}

//-------------------------------------

BOOL GtkSalGraphics::NWPaintGTKEditBox( ControlType nType, ControlPart nPart,
                                        const Region& rControlRegion, ControlState nState,
                                        const ImplControlValue& aValue, SalControlHandle& rControlHandle,
                                        OUString aCaption )
{
    GdkPixmap * pixmap;
    Rectangle       pixmapRect;

    // Find the overall bounding rect of the buttons's drawing area,
    // plus its actual draw rect excluding adornment
    pixmapRect = NWGetEditBoxPixmapRect( nType, nPart, rControlRegion.GetBoundRect(),
                    nState, aValue, rControlHandle, aCaption );

    pixmap = NWGetPixmapFromScreen( pixmapRect );
    if ( !pixmap )
        return( FALSE );

    // NWPaintOneEditBox() requires pixmap local
    Rectangle       aEditBoxRect = rControlRegion.GetBoundRect();
    aEditBoxRect.setX( rControlRegion.GetBoundRect().getX() - pixmapRect.getX() );
    aEditBoxRect.setY( rControlRegion.GetBoundRect().getY() - pixmapRect.getY() );

    NWPaintOneEditBox( pixmap, NULL, nType, nPart, aEditBoxRect, nState, aValue, rControlHandle, aCaption );

    if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
    {
        g_object_unref( pixmap );
        return( FALSE );
    }

    g_object_unref( pixmap );
    return( TRUE );
}


/* Take interior/exterior focus into account and return
 * the bounding rectangle of the edit box including
 * any focus requirements.
 */
static Rectangle NWGetEditBoxPixmapRect(ControlType         nType,
                                ControlPart         nPart,
                                Rectangle               aAreaRect,
                                ControlState            nState,
                                const ImplControlValue& aValue,
                                SalControlHandle&       rControlHandle,
                                OUString                aCaption )
{
    Rectangle       pixmapRect = aAreaRect;
    gboolean        interiorFocus;
    gint            focusWidth;

    NWEnsureGTKEditBox();

    // Grab some entry style attributes
    gtk_widget_style_get( gEditBoxWidget,   "focus-line-width", &focusWidth,
                                     "interior-focus",  &interiorFocus, NULL );

    if ( !interiorFocus )
    {
        pixmapRect.Move( -(focusWidth), -(focusWidth) );
        pixmapRect.setWidth( pixmapRect.getWidth() + (2*(focusWidth)) );
        pixmapRect.setHeight( pixmapRect.getHeight() + (2*(focusWidth)) );
    }

    return( pixmapRect );
}


/* Paint a GTK Entry widget into the specified GdkPixmap.
 * All coordinates should be local to the Pixmap, NOT
 * screen/window coordinates.
 */
static void NWPaintOneEditBox(  GdkWindow * gdkDrawable,
                                GdkRectangle *  gdkRect,
                            ControlType         nType,
                            ControlPart         nPart,
                            Rectangle               aEditBoxRect,
                            ControlState            nState,
                            const ImplControlValue& aValue,
                            SalControlHandle&       rControlHandle,
                            OUString                aCaption )
{
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    GtkWidget      *widget;
    gboolean        interiorFocus;
    gint            focusWidth;

    NWEnsureGTKButton();
    NWEnsureGTKEditBox();
    NWEnsureGTKSpinButton();
    NWEnsureGTKCombo();
    NWEnsureGTKScrolledWindow();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    shadowType = GTK_SHADOW_IN;

    switch ( nType )
    {
        case CTRL_COMBOBOX:
        case CTRL_SPINBOX:
            widget = gSpinButtonWidget;
            break;

        case CTRL_MULTILINE_EDITBOX:
            widget = gScrolledWindowWidget;
            break;
//      case CTRL_COMBOBOX:
//          widget = gComboWidget;
//          break;

        default:
            widget = gEditBoxWidget;
            break;
    }

    NWSetWidgetState( gBtnWidget, nState, stateType );
    NWSetWidgetState( widget, nState, stateType );

    // Blueprint needs to paint entry_bg with a Button widget, not an Entry widget to get
    // a nice white (or whatever default color) background
    if ( stateType == GTK_STATE_PRELIGHT )
        stateType = GTK_STATE_NORMAL;
    gtk_paint_flat_box( gBtnWidget->style, gdkDrawable, stateType, GTK_SHADOW_NONE,
                        gdkRect, gBtnWidget, "entry_bg",
                        aEditBoxRect.getX(), aEditBoxRect.getY(),
                        aEditBoxRect.getWidth(), aEditBoxRect.getHeight() );

    gtk_paint_shadow( widget->style, gdkDrawable, GTK_STATE_NORMAL, shadowType,
                      gdkRect, widget, "entry",
                      aEditBoxRect.getX(), aEditBoxRect.getY(),
                      aEditBoxRect.getWidth(), aEditBoxRect.getHeight() );

#if 0   // vcl draws focus rects
    // Grab some entry style attributes
    gtk_widget_style_get( gEditBoxWidget,   "focus-line-width", &focusWidth,
                                     "interior-focus",  &interiorFocus, NULL );
    // Draw focus rect
    if ( nState & CTRL_STATE_FOCUSED )
    {
        if ( !interiorFocus )
        {
            Rectangle aFocusRect = aEditBoxRect;

            aFocusRect.Move( -(focusWidth), -(focusWidth) );
            aFocusRect.setWidth( aFocusRect.getWidth() + (2*(focusWidth)) );
            aFocusRect.setHeight( aFocusRect.getHeight() + (2*(focusWidth)) );

            gtk_paint_focus( widget->style, gdkDrawable, stateType, gdkRect, widget, "entry",
                aFocusRect.getX(), aFocusRect.getY(), aFocusRect.getWidth(), aFocusRect.getHeight() );
        }
    }
#endif
}



//-------------------------------------

BOOL GtkSalGraphics::NWPaintGTKSpinBox( ControlType nType, ControlPart nPart,
                                        const Region& rControlRegion, ControlState nState,
                                        const ImplControlValue& aValue,
                                        SalControlHandle& rControlHandle, OUString aCaption )
{
    GdkPixmap   *       pixmap;
    Rectangle           pixmapRect;
    GtkStateType        stateType;
    GtkShadowType       shadowType;
    SpinbuttonValue *   pSpinVal = (SpinbuttonValue *)(aValue.getOptionalVal());
    Rectangle           upBtnRect;
    ControlPart     upBtnPart = PART_BUTTON_UP;
    ControlState        upBtnState = CTRL_STATE_ENABLED;
    Rectangle           downBtnRect;
    ControlPart     downBtnPart = PART_BUTTON_DOWN;
    ControlState        downBtnState = CTRL_STATE_ENABLED;

    NWEnsureGTKButton();
    NWEnsureGTKSpinButton();
    NWEnsureGTKArrow();

    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    if ( pSpinVal )
    {
        upBtnPart = pSpinVal->mnUpperPart;
        upBtnState = pSpinVal->mnUpperState;

        downBtnPart = pSpinVal->mnLowerPart;
        downBtnState = pSpinVal->mnLowerState;
    }

    // CTRL_SPINBUTTONS pass their area in pSpinVal, not in rControlRegion
    if ( nType == CTRL_SPINBUTTONS )
    {
        if ( !pSpinVal )
        {
            fprintf( stderr, "Tried to draw CTRL_SPINBUTTONS, but the SpinButtons data structure didn't exist!\n" );
            return( false );
        }
        pixmapRect = pSpinVal->maUpperRect;
        pixmapRect.Union( pSpinVal->maLowerRect );
    }
    else
        pixmapRect = rControlRegion.GetBoundRect();


    pixmap = NWGetPixmapFromScreen( pixmapRect );
    if ( !pixmap )
        return( FALSE );

    upBtnRect = NWGetSpinButtonRect( nType, upBtnPart, pixmapRect, upBtnState, aValue, rControlHandle, aCaption );
    downBtnRect = NWGetSpinButtonRect( nType, downBtnPart, pixmapRect, downBtnState, aValue, rControlHandle, aCaption );

    if ( (nType==CTRL_SPINBOX) && (nPart!=PART_ALL_BUTTONS) )
    {
        // Draw an edit field for SpinBoxes and ComboBoxes
        Rectangle aEditBoxRect( pixmapRect );
        aEditBoxRect.setWidth( upBtnRect.getX() - pixmapRect.getX() );
        aEditBoxRect.setX( 0 );
        aEditBoxRect.setY( 0 );

        NWPaintOneEditBox( pixmap, NULL, nType, nPart, aEditBoxRect, nState, aValue, rControlHandle, aCaption );
    }

    NWSetWidgetState( gSpinButtonWidget, nState, stateType );
    gtk_widget_style_get( gSpinButtonWidget, "shadow_type", &shadowType, NULL );

    if ( shadowType != GTK_SHADOW_NONE )
    {
        Rectangle       shadowRect( upBtnRect );

        shadowRect.Union( downBtnRect );
        gtk_paint_box( gSpinButtonWidget->style, pixmap, GTK_STATE_NORMAL, shadowType, NULL,
            gSpinButtonWidget, "spinbutton",
            (shadowRect.getX() - pixmapRect.getX()), (shadowRect.getY() - pixmapRect.getY()),
            shadowRect.getWidth(), shadowRect.getHeight() );
    }

    NWPaintOneSpinButton( pixmap, nType, upBtnPart, pixmapRect, upBtnState, aValue, rControlHandle, aCaption );
    NWPaintOneSpinButton( pixmap, nType, downBtnPart, pixmapRect, downBtnState, aValue, rControlHandle, aCaption );

    if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
    {
        g_object_unref( pixmap );
        return( FALSE );
    }

    g_object_unref( pixmap );
    return( TRUE );
}

//---

static Rectangle NWGetSpinButtonRect(   ControlType         nType,
                                ControlPart         nPart,
                                Rectangle           aAreaRect,
                                ControlState            nState,
                                const ImplControlValue& aValue,
                                SalControlHandle&       rControlHandle,
                                OUString                aCaption )
{
    gint            buttonSize;
    Rectangle       buttonRect;

    NWEnsureGTKSpinButton();

    buttonSize = MAX( PANGO_PIXELS( pango_font_description_get_size(GTK_WIDGET(gSpinButtonWidget)->style->font_desc) ),
                   MIN_SPIN_ARROW_WIDTH );
    buttonSize -= buttonSize % 2 - 1; /* force odd */
    buttonRect.setWidth( buttonSize + 2 * gSpinButtonWidget->style->xthickness );
    buttonRect.setX( aAreaRect.getX() + (aAreaRect.getWidth() - buttonRect.getWidth()) );
    if ( nPart == PART_BUTTON_UP )
    {
        buttonRect.Top() = aAreaRect.getY();
        buttonRect.Bottom() = buttonRect.Top() + (aAreaRect.getHeight() / 2);
    }
    else
    {
        buttonRect.Top() = aAreaRect.getY() + (aAreaRect.getHeight() / 2);
        buttonRect.Bottom() = aAreaRect.Bottom(); // cover area completely
    }

    return( buttonRect );
}

//---

static void NWPaintOneSpinButton(   GdkPixmap   *           pixmap,
                            ControlType         nType,
                            ControlPart         nPart,
                            Rectangle               aAreaRect,
                            ControlState            nState,
                            const ImplControlValue& aValue,
                            SalControlHandle&       rControlHandle,
                            OUString                aCaption )
{
    Rectangle           buttonRect;
    GtkStateType        stateType;
    GtkShadowType       shadowType;
    Rectangle           arrowRect;
    gint                arrowSize;

    NWEnsureGTKSpinButton();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    buttonRect = NWGetSpinButtonRect( nType, nPart, aAreaRect, nState, aValue, rControlHandle, aCaption );

    NWSetWidgetState( gSpinButtonWidget, nState, stateType );
    gtk_paint_box( gSpinButtonWidget->style, pixmap, stateType, shadowType, NULL, gSpinButtonWidget,
            (nPart == PART_BUTTON_UP) ? "spinbutton_up" : "spinbutton_down",
            (buttonRect.getX() - aAreaRect.getX()), (buttonRect.getY() - aAreaRect.getY()),
            buttonRect.getWidth(), buttonRect.getHeight() );

    arrowSize = (buttonRect.getWidth() - (2 * gSpinButtonWidget->style->xthickness)) - 4;
    arrowSize -= arrowSize % 2 - 1; /* force odd */
    arrowRect.setWidth( arrowSize );
    arrowRect.setHeight( arrowRect.getWidth() );
    arrowRect.setX( buttonRect.getX() + (buttonRect.getWidth() - arrowRect.getWidth()) / 2 );
    if ( nPart == PART_BUTTON_UP )
        arrowRect.setY( buttonRect.getY() + (buttonRect.getHeight() - arrowRect.getHeight()) / 2 + 1);
    else
        arrowRect.setY( buttonRect.getY() + (buttonRect.getHeight() - arrowRect.getHeight()) / 2 - 1);

    gtk_paint_arrow( gSpinButtonWidget->style, pixmap, GTK_STATE_NORMAL, GTK_SHADOW_OUT, NULL, gSpinButtonWidget,
            "spinbutton", (nPart == PART_BUTTON_UP) ? GTK_ARROW_UP : GTK_ARROW_DOWN, TRUE,
            (arrowRect.getX() - aAreaRect.getX()), (arrowRect.getY() - aAreaRect.getY()),
            arrowRect.getWidth(), arrowRect.getHeight() );
}


//-------------------------------------

BOOL GtkSalGraphics::NWPaintGTKComboBox( ControlType nType, ControlPart nPart,
                                         const Region& rControlRegion, ControlState nState,
                                         const ImplControlValue& aValue,
                                         SalControlHandle& rControlHandle, OUString aCaption )
{
    GdkPixmap   *   pixmap = NULL;
    Rectangle       pixmapRect;
    Rectangle       buttonRect;
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    Rectangle       arrowRect;
    gint            arrowSize;
    gint            x,y;
    GdkRectangle    clipRect;

    NWEnsureGTKButton();
    NWEnsureGTKArrow();
    NWEnsureGTKCombo();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    BOOL bUseWindow = (GetNumCliprects() == 1);

    // Find the overall bounding rect of the buttons's drawing area,
    // plus its actual draw rect excluding adornment
    pixmapRect = rControlRegion.GetBoundRect();

    if( !bUseWindow )
    {
        pixmap = NWGetPixmapFromScreen( pixmapRect );
        if ( !pixmap )
            return( FALSE );
        x = y = 0;
    }
    else
    {
        clipRect.x = GetBoundingCliprect().Left();
        clipRect.y = GetBoundingCliprect().Top();
        clipRect.width = GetBoundingCliprect().getWidth();
        clipRect.height = GetBoundingCliprect().getHeight();
        x = pixmapRect.Left();
        y = pixmapRect.Top();
    }

    // set up references to correct drawable and cliprect
    GdkDrawable* const &gdkDrawable = GDK_DRAWABLE( bUseWindow ? GetGdkWindow() : pixmap );
    GdkRectangle* const &gdkRect    = bUseWindow ? &clipRect : NULL;

    buttonRect = NWGetComboBoxButtonRect( nType, nPart, pixmapRect, nState, aValue, rControlHandle, aCaption );

    Rectangle       aEditBoxRect( pixmapRect );
    aEditBoxRect.setWidth( pixmapRect.getWidth() - buttonRect.getWidth() );
    aEditBoxRect.setX( x );
    aEditBoxRect.setY( y );

    NWPaintOneEditBox( gdkDrawable, gdkRect, nType, nPart, aEditBoxRect,
                       nState, aValue, rControlHandle, aCaption );

    NWSetWidgetState( gBtnWidget, nState, stateType );
    NWSetWidgetState( gComboWidget, nState, stateType );
    NWSetWidgetState( gArrowWidget, nState, stateType );

    // Buttons must paint opaque since some themes have alpha-channel enabled buttons
    gtk_paint_flat_box( gBtnWidget->style, gdkDrawable, GTK_STATE_NORMAL, GTK_SHADOW_NONE,
                        gdkRect, gBtnWidget, "base",
                        x+(buttonRect.getX() - pixmapRect.getX()),
                        y+(buttonRect.getY() - pixmapRect.getY()),
                        buttonRect.getWidth(), buttonRect.getHeight() );

    gtk_paint_box( GTK_COMBO(gComboWidget)->button->style, gdkDrawable, stateType, shadowType,
                   gdkRect, GTK_COMBO(gComboWidget)->button, "button",
                   x+(buttonRect.getX() - pixmapRect.getX()),
                   y+(buttonRect.getY() - pixmapRect.getY()),
                   buttonRect.getWidth(), buttonRect.getHeight() );

#define ARROW_EXTENT        0.7
    arrowRect.setWidth( (gint)(MIN_ARROW_SIZE * ARROW_EXTENT) );
    arrowRect.setHeight( (gint)(MIN_ARROW_SIZE * ARROW_EXTENT) );
    arrowRect.setX( buttonRect.getX() + (gint)((buttonRect.getWidth() - arrowRect.getWidth()) / 2)  );
    arrowRect.setY( buttonRect.getY() + (gint)((buttonRect.getHeight() - arrowRect.getHeight()) / 2)  );

    gtk_paint_arrow( gArrowWidget->style, gdkDrawable, stateType, shadowType,
            gdkRect, gArrowWidget, "arrow", GTK_ARROW_DOWN, TRUE,
            x+(arrowRect.getX() - pixmapRect.getX()), y+(arrowRect.getY() - pixmapRect.getY()),
            arrowRect.getWidth(), arrowRect.getHeight() );

    if( !bUseWindow )
    {
        if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
        {
            g_object_unref( pixmap );
            return( FALSE );
        }
        g_object_unref( pixmap );
    }

    return( TRUE );
}

//----

static Rectangle NWGetComboBoxButtonRect(   ControlType         nType,
                                    ControlPart         nPart,
                                    Rectangle               aAreaRect,
                                    ControlState            nState,
                                    const ImplControlValue& aValue,
                                    SalControlHandle&       rControlHandle,
                                    OUString                aCaption )
{
    Rectangle   aButtonRect;
    gint        nArrowWidth;
    gint        nFocusWidth;
    gint        nFocusPad;

    NWEnsureGTKArrow();

    // Grab some button style attributes
    gtk_widget_style_get( gDropdownWidget,  "focus-line-width", &nFocusWidth,
                                    "focus-padding",    &nFocusPad, NULL );

    nArrowWidth = MIN_ARROW_SIZE + (GTK_MISC(gArrowWidget)->xpad * 2);
    aButtonRect.setY( aAreaRect.getY() );
    aButtonRect.setHeight( aAreaRect.getHeight() );
    aButtonRect.setWidth( nArrowWidth + ((BTN_CHILD_SPACING + gDropdownWidget->style->xthickness) * 2)
                        + (2 * (nFocusWidth+nFocusPad)) );
    aButtonRect.setX( aAreaRect.getX() + aAreaRect.getWidth() - aButtonRect.getWidth() );

    return( aButtonRect );
}

//-------------------------------------



BOOL GtkSalGraphics::NWPaintGTKTabItem( ControlType nType, ControlPart nPart,
                                        const Region& rControlRegion, ControlState nState,
                                        const ImplControlValue& aValue,
                                        SalControlHandle& rControlHandle, OUString aCaption )
{
    GdkPixmap * pixmap;
    Rectangle       pixmapRect;
    Rectangle       tabRect;
    TabitemValue *  pTabitemValue = (TabitemValue *)(aValue.getOptionalVal());
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    static NWPixmapCache aCacheItems;
    static NWPixmapCache aCachePage;

    if( !aCacheItems.GetSize() )
        aCacheItems.SetSize( 20 );
    if( !aCachePage.GetSize() )
        aCachePage.SetSize( 1 );

    if ( !pTabitemValue && (nType==CTRL_TAB_ITEM) )
    {
        fprintf( stderr, "NWPaintGTKTabItem() received a NULL TabitemValue.  Cannot draw native tab\n" );
        return( false );
    }

    NWEnsureGTKButton();
    NWEnsureGTKNotebook();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    // Find the overall bounding rect of the buttons's drawing area,
    // plus its actual draw rect excluding adornment
    pixmapRect = rControlRegion.GetBoundRect();
    if ( nType == CTRL_TAB_ITEM )
    {
        if ( !pTabitemValue->isFirst() )
        {
            // GTK+ tabs overlap on the right edge (the top tab obscures the
            // left edge of the tab right "below" it, so adjust the rectangle
            // to draw tabs slightly large so the overlap happens
            pixmapRect.Move( -2, 0 );
            pixmapRect.setWidth( pixmapRect.getWidth() + 2 );
        }
        if ( nState & CTRL_STATE_SELECTED )
        {
            // In GTK+, the selected tab is 2px taller than all other tabs
            pixmapRect.setY( pixmapRect.getY() - 2 );
            pixmapRect.setHeight( pixmapRect.getHeight() + 3 );
            tabRect = pixmapRect;
            // Only draw over 1 pixel of the tab pane that this tab is drawn on top of.
            tabRect.setHeight( tabRect.getHeight() - 1 );
        }
        else
            tabRect = pixmapRect;

        // Allow the tab to draw a right border if needed
        tabRect.setWidth( tabRect.getWidth() - 1 );
    }

    if( nType == CTRL_TAB_ITEM )
    {
        if( aCacheItems.Find( nType, nState, pixmapRect, &pixmap ) )
            return NWRenderPixmapToScreen( pixmap, pixmapRect );
    }
    else
    {
        if( aCachePage.Find( nType, nState, pixmapRect, &pixmap ) )
            return NWRenderPixmapToScreen( pixmap, pixmapRect );
    }


//  gtk_widget_set_state( gNotebookWidget, stateType );

    pixmap = NWGetPixmapFromScreen( pixmapRect );
    if ( !pixmap )
        return( FALSE );

    NWSetWidgetState( gNotebookWidget, nState, stateType );
    switch( nType )
    {
        case CTRL_TAB_BODY:
            break;

        case CTRL_FIXEDBORDER:
        case CTRL_TAB_PANE:
            gtk_paint_box_gap( gNotebookWidget->style, pixmap, GTK_STATE_NORMAL, GTK_SHADOW_OUT, NULL, gNotebookWidget,
                (char *)"notebook", 0, 0, pixmapRect.getWidth(), pixmapRect.getHeight(), GTK_POS_TOP, 0, 0 );
            break;

        case CTRL_TAB_ITEM:
            stateType = ( nState & CTRL_STATE_SELECTED ) ? GTK_STATE_NORMAL : GTK_STATE_ACTIVE;

            gtk_paint_extension( gNotebookWidget->style, pixmap, stateType, GTK_SHADOW_OUT, NULL, gNotebookWidget,
                (char *)"tab", (tabRect.getX() - pixmapRect.getX()), (tabRect.getY() - pixmapRect.getY()),
                tabRect.getWidth(), tabRect.getHeight(), GTK_POS_BOTTOM );

            if ( nState & CTRL_STATE_SELECTED )
            {
                gtk_paint_flat_box( gNotebookWidget->style, pixmap, stateType, GTK_SHADOW_NONE, NULL, gNotebookWidget,
                    (char *)"base", 0, (pixmapRect.getHeight() - 2), pixmapRect.getWidth(), 1 );
            }
            break;

        default:
            break;
    }

    // Crux seems to think it can make the pane without a left edge
    if ( nType == CTRL_FIXEDBORDER )
        pixmapRect.Move( 1, 0 );

    // cache data
    if( GetGtkFrame()->getVisibilityState() == GDK_VISIBILITY_UNOBSCURED )
    {
        if( nType == CTRL_TAB_ITEM )
            aCacheItems.Fill( nType, nState, pixmapRect, pixmap );
        else
            aCachePage.Fill( nType, nState, pixmapRect, pixmap );
    }

    if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
    {
        g_object_unref( pixmap );
        return( FALSE );
    }

    g_object_unref( pixmap );
    return( TRUE );
}

//-------------------------------------

BOOL GtkSalGraphics::NWPaintGTKListBox( ControlType nType, ControlPart nPart,
                                        const Region& rControlRegion, ControlState nState,
                                        const ImplControlValue& aValue,
                                        SalControlHandle& rControlHandle, OUString aCaption )
{
    GdkPixmap   *   pixmap = NULL;
    Rectangle       pixmapRect;
    Rectangle       widgetRect;
    Rectangle       aIndicatorRect;
    GtkStateType    stateType;
    GtkShadowType   shadowType;
    gint            bInteriorFocus;
    gint            nFocusLineWidth;
    gint            nFocusPadding;
    gint            x,y;
    GdkRectangle    clipRect;

    NWEnsureGTKButton();
    NWEnsureGTKOptionMenu();
    NWEnsureGTKScrolledWindow();
    NWConvertVCLStateToGTKState( nState, &stateType, &shadowType );

    BOOL bUseWindow = (GetNumCliprects() == 1);

    // Find the overall bounding rect of the buttons's drawing area,
    // plus its actual draw rect excluding adornment
    pixmapRect = rControlRegion.GetBoundRect();
    if ( nPart == PART_WINDOW )
    {
        // Make the widget a _bit_ bigger
        pixmapRect.setX( pixmapRect.getX() - 1 );
        pixmapRect.setY( pixmapRect.getY() - 1 );
        pixmapRect.setWidth( pixmapRect.getWidth() + 2 );
        pixmapRect.setHeight( pixmapRect.getHeight() + 2 );
    }

    widgetRect = pixmapRect;

    if( !bUseWindow )
    {
        pixmap = NWGetPixmapFromScreen( pixmapRect );
        if ( !pixmap )
            return( FALSE );
        x = y = 0;
    }
    else
    {
        clipRect.x = GetBoundingCliprect().Left();
        clipRect.y = GetBoundingCliprect().Top();
        clipRect.width = GetBoundingCliprect().getWidth();
        clipRect.height = GetBoundingCliprect().getHeight();
        x = pixmapRect.Left();
        y = pixmapRect.Top();
    }

    // set up references to correct drawable and cliprect
    GdkDrawable* const &gdkDrawable = GDK_DRAWABLE( bUseWindow ? GetGdkWindow() : pixmap );
    GdkRectangle* const &gdkRect    = bUseWindow ? &clipRect : NULL;

    NWSetWidgetState( gBtnWidget, nState, stateType );
    NWSetWidgetState( gOptionMenuWidget, nState, stateType );
    NWSetWidgetState( gScrolledWindowWidget, nState, stateType );

    if ( nPart != PART_WINDOW )
    {
        gtk_widget_style_get( gOptionMenuWidget,
            "interior_focus",   &bInteriorFocus,
            "focus_line_width", &nFocusLineWidth,
            "focus_padding",    &nFocusPadding,
            NULL);
        // Listboxes must paint opaque since some themes have alpha-channel enabled bodies
        gtk_paint_flat_box( gBtnWidget->style, gdkDrawable, GTK_STATE_NORMAL, GTK_SHADOW_NONE,
                            gdkRect, gBtnWidget, "base", x, y,
                            pixmapRect.getWidth(), pixmapRect.getHeight() );
        gtk_paint_box( gOptionMenuWidget->style, gdkDrawable, stateType, shadowType, gdkRect,
                       gOptionMenuWidget, "optionmenu",
                       x+(widgetRect.getX() - pixmapRect.getX()),
                       y+(widgetRect.getY() - pixmapRect.getY()),
                       widgetRect.getWidth(), widgetRect.getHeight() );
        aIndicatorRect = NWGetListBoxIndicatorRect( nType, nPart, widgetRect, nState,
                                                    aValue, rControlHandle, aCaption );
        gtk_paint_tab( gOptionMenuWidget->style, gdkDrawable, stateType, shadowType, gdkRect,
                       gOptionMenuWidget, "optionmenutab",
                       x+(aIndicatorRect.getX() - pixmapRect.getX()),
                       y+(aIndicatorRect.getY() - pixmapRect.getY()),
                       aIndicatorRect.getWidth(), aIndicatorRect.getHeight() );
    }
    else
    {
        shadowType = GTK_SHADOW_IN;

        gtk_paint_shadow( gScrolledWindowWidget->style, gdkDrawable, GTK_STATE_NORMAL, shadowType,
            gdkRect, gScrolledWindowWidget, "scrolled_window",
            x+(widgetRect.getX() - pixmapRect.getX()), y+(widgetRect.getY() - pixmapRect.getY()),
            widgetRect.getWidth(), widgetRect.getHeight() );
    }

    if( !bUseWindow )
    {
        if( !NWRenderPixmapToScreen(pixmap, pixmapRect) )
        {
            g_object_unref( pixmap );
            return( FALSE );
        }
    g_object_unref( pixmap );
    }

    return( TRUE );
}

//----

static Rectangle NWGetListBoxButtonRect( ControlType    nType,
                                         ControlPart    nPart,
                                         Rectangle      aAreaRect,
                                         ControlState   nState,
                                         const ImplControlValue&    aValue,
                                         SalControlHandle&          rControlHandle,
                                         OUString       aCaption )
{
    Rectangle       aPartRect;
    GtkRequisition *pIndicatorSize = NULL;
    GtkBorder      *pIndicatorSpacing = NULL;
    gint            width = 13; // GTK+ default
    gint            right = 5;  // GTK+ default
    gint            nButtonAreaWidth = 0;
    gint            xthickness = 0;

    NWEnsureGTKOptionMenu();

    gtk_widget_style_get( gOptionMenuWidget,
            "indicator_size",   &pIndicatorSize,
            "indicator_spacing",&pIndicatorSpacing, NULL);

    if ( pIndicatorSize )
        width = pIndicatorSize->width;

    if ( pIndicatorSpacing )
        right = pIndicatorSpacing->right;

    aPartRect.setHeight( aAreaRect.getHeight() );
    aPartRect.setY( aAreaRect.getY() );

    xthickness = gOptionMenuWidget->style->xthickness;
    nButtonAreaWidth = width + right + (xthickness * 2);
    switch( nPart )
    {
        case PART_BUTTON_DOWN:
            aPartRect.setWidth( nButtonAreaWidth );
            aPartRect.setX( aAreaRect.getX() + aAreaRect.getWidth() - aPartRect.getWidth() );
            break;

        case PART_SUB_EDIT:
            aPartRect.setWidth( aAreaRect.getWidth() - nButtonAreaWidth - xthickness );
            aPartRect.setX( aAreaRect.getX() + xthickness );
            break;

        default:
            aPartRect.setWidth( aAreaRect.getWidth() );
            aPartRect.setX( aAreaRect.getX() );
            break;
    }

    if ( pIndicatorSize )
        g_free( pIndicatorSize );
    if ( pIndicatorSpacing )
        g_free( pIndicatorSpacing );

    return( aPartRect );
}

//----

static Rectangle NWGetListBoxIndicatorRect( ControlType         nType,
                                    ControlPart         nPart,
                                    Rectangle               aAreaRect,
                                    ControlState            nState,
                                    const ImplControlValue& aValue,
                                    SalControlHandle&       rControlHandle,
                                    OUString                aCaption )
{
    Rectangle       aIndicatorRect;
    GtkRequisition *pIndicatorSize = NULL;
    GtkBorder      *pIndicatorSpacing = NULL;
    gint            width = 13; // GTK+ default
    gint            height = 13;    // GTK+ default
    gint            right = 5;  // GTK+ default

    NWEnsureGTKOptionMenu();

    gtk_widget_style_get( gOptionMenuWidget,
            "indicator_size",   &pIndicatorSize,
            "indicator_spacing",&pIndicatorSpacing, NULL);

    if ( pIndicatorSize )
    {
        width = pIndicatorSize->width;
        height = pIndicatorSize->height;
    }

    if ( pIndicatorSpacing )
        right = pIndicatorSpacing->right;

    aIndicatorRect.setWidth( width );
    aIndicatorRect.setHeight( height );
    aIndicatorRect.setX( aAreaRect.getX() + aAreaRect.getWidth() - width - right - gOptionMenuWidget->style->xthickness );
    aIndicatorRect.setY( aAreaRect.getY() + ((aAreaRect.getHeight() - height) / 2) );

    // If height is odd, move the indicator down 1 pixel
    if ( aIndicatorRect.getHeight() % 2 )
        aIndicatorRect.setY( aIndicatorRect.getY() + 1 );

    if ( pIndicatorSize )
        g_free( pIndicatorSize );
    if ( pIndicatorSpacing )
        g_free( pIndicatorSpacing );

    return( aIndicatorRect );
}


/************************************************************************
 * Create a GdkPixmap filled with the contents of an area of an Xlib window
 ************************************************************************/

GdkPixmap* GtkSalGraphics::NWGetPixmapFromScreen( Rectangle srcRect )
{
    // Create a new pixmap to hold the composite of the window background and the control
    GdkPixmap * pPixmap     = gdk_pixmap_new( NULL, srcRect.getWidth(), srcRect.getHeight(),
                                              GetSalData()->GetDisplay()->GetVisual()->GetDepth() );
    GdkGC *  pPixmapGC  = gdk_gc_new( pPixmap );

    if( !pPixmap || !pPixmapGC )
    {
        if ( pPixmap )
            g_object_unref( pPixmap );
        if ( pPixmapGC )
            g_object_unref( pPixmapGC );
        fprintf( stderr, "salnativewidgets-gtk.cxx: could not get valid pixmap from screen\n" );
        return( NULL );
    }

    // Copy the background of the screen into a composite pixmap
    XCopyArea( GetXDisplay(), GetDrawable(), gdk_x11_drawable_get_xid(pPixmap), gdk_x11_gc_get_xgc(pPixmapGC),
               srcRect.getX(), srcRect.getY(), srcRect.getWidth(), srcRect.getHeight(), 0, 0 );

    g_object_unref( pPixmapGC );
    return( pPixmap );
}




/************************************************************************
 * Copy an alpha pixmap to screen using a gc with clipping
 ************************************************************************/

BOOL GtkSalGraphics::NWRenderPixmapToScreen( GdkPixmap* pPixmap, Rectangle dstRect )
{
    // The GC can't be null, otherwise we'd have no clip region
    if( SelectFont() == NULL )
    {
        fprintf(stderr, "salnativewidgets.cxx: no valid GC\n" );
        return( FALSE );
    }

    if ( !pPixmap )
        return( FALSE );

    // Copy the background of the screen into a composite pixmap
    XCopyArea( GetXDisplay(), GDK_DRAWABLE_XID(pPixmap), GetDrawable(), SelectFont(),
               0, 0, dstRect.getWidth(), dstRect.getHeight(), dstRect.getX(), dstRect.getY() );

    X11SalGraphics::YieldGraphicsExpose( GetXDisplay(), NULL, GetDrawable() );

    return( TRUE );
}


/************************************************************************
 * State conversion
 ************************************************************************/
static void NWConvertVCLStateToGTKState( ControlState nVCLState,
            GtkStateType* nGTKState, GtkShadowType* nGTKShadow )
{
    *nGTKShadow = GTK_SHADOW_OUT;
    *nGTKState = GTK_STATE_INSENSITIVE;

    if ( nVCLState & CTRL_STATE_ENABLED )
    {
        if ( nVCLState & CTRL_STATE_PRESSED )
        {
            *nGTKState = GTK_STATE_ACTIVE;
            *nGTKShadow = GTK_SHADOW_IN;
        }
        else if ( nVCLState & CTRL_STATE_ROLLOVER )
        {
            *nGTKState = GTK_STATE_PRELIGHT;
            *nGTKShadow = GTK_SHADOW_OUT;
        }
        else
        {
            *nGTKState = GTK_STATE_NORMAL;
            *nGTKShadow = GTK_SHADOW_OUT;
        }
    }
}

/************************************************************************
 * Set widget flags
 ************************************************************************/
static void NWSetWidgetState( GtkWidget* widget, ControlState nState, GtkStateType nGtkState )
{
    // Set to default state, then build up from there
    GTK_WIDGET_UNSET_FLAGS( widget, GTK_HAS_DEFAULT );
    GTK_WIDGET_UNSET_FLAGS( widget, GTK_HAS_FOCUS );
    GTK_WIDGET_UNSET_FLAGS( widget, GTK_SENSITIVE );
    GTK_WIDGET_SET_FLAGS( widget, gWidgetDefaultFlags[(long)widget] );

    if ( nState & CTRL_STATE_DEFAULT )
        GTK_WIDGET_SET_FLAGS( widget, GTK_HAS_DEFAULT );
    if ( !GTK_IS_TOGGLE_BUTTON(widget) && (nState & CTRL_STATE_FOCUSED) )
        GTK_WIDGET_SET_FLAGS( widget, GTK_HAS_FOCUS );
    if ( nState & CTRL_STATE_ENABLED )
        GTK_WIDGET_SET_FLAGS( widget, GTK_SENSITIVE );
    gtk_widget_set_state( widget, nGtkState );
}

/************************************************************************
 * Widget ensure functions - make sure cached objects are valid
 ************************************************************************/

//-------------------------------------

static void NWAddWidgetToCacheWindow( GtkWidget* widget )
{
    if ( !gCacheWindow || !gDumbContainer )
    {
        if ( !gCacheWindow )
            gCacheWindow = gtk_window_new( GTK_WINDOW_TOPLEVEL );
        if ( !gDumbContainer )
            gDumbContainer = gtk_fixed_new();
        gtk_container_add( GTK_CONTAINER(gCacheWindow), gDumbContainer );
        gtk_widget_realize( gDumbContainer );
        gtk_widget_realize( gCacheWindow );
    }

    gtk_container_add( GTK_CONTAINER(gDumbContainer), widget );
    gtk_widget_realize( widget );
    gtk_widget_ensure_style( widget );

    // Store widget's default flags
    gWidgetDefaultFlags[ (long)widget ] = GTK_WIDGET_FLAGS( widget );
}

//-------------------------------------

static void NWEnsureGTKButton( void )
{
    if ( !gBtnWidget )
    {
        gBtnWidget = gtk_button_new_with_label( "" );
        NWAddWidgetToCacheWindow( gBtnWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKRadio( void )
{
    if ( !gRadioWidget || !gRadioWidgetSibling )
    {
        gRadioWidget = gtk_radio_button_new( NULL );
        gRadioWidgetSibling = gtk_radio_button_new_from_widget( GTK_RADIO_BUTTON(gRadioWidget) );
        NWAddWidgetToCacheWindow( gRadioWidget );
        NWAddWidgetToCacheWindow( gRadioWidgetSibling );
    }
}

//-------------------------------------

static void NWEnsureGTKCheck( void )
{
    if ( !gCheckWidget )
    {
        gCheckWidget = gtk_check_button_new();
        NWAddWidgetToCacheWindow( gCheckWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKScrollbars( void )
{
    if ( !gScrollHorizWidget )
    {
        gScrollHorizWidget = gtk_hscrollbar_new( NULL );
        NWAddWidgetToCacheWindow( gScrollHorizWidget );
    }

    if ( !gScrollVertWidget )
    {
        gScrollVertWidget = gtk_vscrollbar_new( NULL );
        NWAddWidgetToCacheWindow( gScrollVertWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKArrow( void )
{
    if ( !gArrowWidget || !gDropdownWidget )
    {
        gDropdownWidget = gtk_toggle_button_new();
        NWAddWidgetToCacheWindow( gDropdownWidget );
        gArrowWidget = gtk_arrow_new( GTK_ARROW_DOWN, GTK_SHADOW_OUT );
        gtk_container_add( GTK_CONTAINER(gDropdownWidget), gArrowWidget );
        gtk_widget_set_rc_style( gArrowWidget );
        gtk_widget_realize( gArrowWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKEditBox( void )
{
    if ( !gEditBoxWidget )
    {
        gEditBoxWidget = gtk_entry_new();
        NWAddWidgetToCacheWindow( gEditBoxWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKSpinButton( void )
{
    if ( !gSpinButtonWidget )
    {
        GtkAdjustment *adj = GTK_ADJUSTMENT( gtk_adjustment_new(0, 0, 0, 0, 0, 0) );
        gSpinButtonWidget = gtk_spin_button_new( adj, 1, 2 );
        NWAddWidgetToCacheWindow( gSpinButtonWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKNotebook( void )
{
    if ( !gNotebookWidget )
    {
        gNotebookWidget = gtk_notebook_new();
        NWAddWidgetToCacheWindow( gNotebookWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKOptionMenu( void )
{
    if ( !gOptionMenuWidget )
    {
        gOptionMenuWidget = gtk_option_menu_new();
        NWAddWidgetToCacheWindow( gOptionMenuWidget );
    }
}

//-------------------------------------

static void NWEnsureGTKCombo( void )
{
    if ( !gComboWidget )
    {
        gComboWidget = gtk_combo_new();
        NWAddWidgetToCacheWindow( gComboWidget );
        // Must realize the ComboBox's button widget, since GTK
        // does not do this for us in GtkCombo::gtk_widget_realize()
        gtk_widget_realize( GTK_COMBO(gComboWidget)->button );
    }
}

//-------------------------------------

static void NWEnsureGTKScrolledWindow( void )
{
    if ( !gScrolledWindowWidget )
    {
        GtkAdjustment *hadj = GTK_ADJUSTMENT( gtk_adjustment_new(0, 0, 0, 0, 0, 0) );
        GtkAdjustment *vadj = GTK_ADJUSTMENT( gtk_adjustment_new(0, 0, 0, 0, 0, 0) );

        gScrolledWindowWidget = gtk_scrolled_window_new( hadj, vadj );
        NWAddWidgetToCacheWindow( gScrolledWindowWidget );
    }
}
