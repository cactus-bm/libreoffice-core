/*************************************************************************
 *
 *  $RCSfile: saldisp.hxx,v $
 *
 *  $Revision: 1.18 $
 *
 *  last change: $Author: kz $ $Date: 2003-08-25 13:54:50 $
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

#ifndef _SV_SALDISP_HXX
#define _SV_SALDISP_HXX

// -=-= exports =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
struct  SalAppXResource;
class   SalDisplay;
class   SalColormap;
class   SalColormapRef;
class   SalTrueColorConverter;
class   SalVisual;
class   SalXLib;
class   SalImageList;
class   SalBitmapList;

// -=-= #includes =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef _SALUNX_H
#include <salunx.h>
#endif
#ifndef _SV_SALGTYPE_HXX
#include <salgtype.hxx>
#endif
#ifndef _SV_PTRSTYLE_HXX
#include <ptrstyle.hxx>
#endif
#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif

#include <vector>
#ifndef _GEN_HXX
#include <tools/gen.hxx>
#endif

// -=-= forwards -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class   BitmapPalette;
class   SalImage;
class   SalBitmap;
class   SalFrameData;
class   ColorMask;
class   SalSystemData;

namespace vcl_sal { class WMAdaptor; }

#ifndef _XSHM_H_
struct XShmSegmentInfo;
#endif

// -=-= typedefs -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
typedef struct _oslMutexImpl *oslThreadMutex;

DECLARE_LIST( SalImageList, SalImage* )
DECLARE_LIST( SalBitmapList,SalBitmap*)

// -=-= #defines -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define PROPERTY_SUPPORT_WM_SetPos              0x00000001
#define PROPERTY_SUPPORT_WM_Screen              0x00000002
#define PROPERTY_SUPPORT_WM_Parent_Pixmap_None  0x00000004
#define PROPERTY_SUPPORT_WM_ClientPos           0x00000008
#define PROPERTY_SUPPORT_XSetClipMask           0x00000010  // for bitmap ops.
#define PROPERTY_SUPPORT_3ButtonMouse           0x00000020

#define PROPERTY_BUG_XA_FAMILY_NAME_nil         0x00001000
#define PROPERTY_BUG_XCopyArea_GXxor            0x00002000  // from window
#define PROPERTY_BUG_Stipple                    0x00004000  // 0/1 inverted
#define PROPERTY_BUG_Tile                       0x00008000  // Recreate the
                                            // dither brush each time
#define PROPERTY_BUG_FillPolygon_Tile           0x00010000  // always Toggle Fillstyle
#define PROPERTY_BUG_DrawLine                   0x00020000  // a DrawLine is one point to short
#define PROPERTY_BUG_CopyPlane_RevertBWPixel    0x00040000  // revert fg and bg for xcopyplane
#define PROPERTY_BUG_CopyArea_OnlySmallSlices   0x00080000
#define PROPERTY_BUG_Bitmap_Bit_Order           0x00100000

#define PROPERTY_FEATURE_Maximize               0x01000000
#define PROPERTY_FEATURE_SharedMemory           0x02000000
#define PROPERTY_FEATURE_TrustedSolaris         0x04000000

#define PROPERTY_DEFAULT                        0x00000FCB

// ------------------------------------------------------------------------
// server vendor

typedef enum  {
    vendor_none = 0,
    vendor_attachmate,
    vendor_excursion,
    vendor_hp,
    vendor_hummingbird,
    vendor_ibm,
    vendor_sco,
    vendor_sgi,
    vendor_sun,
    vendor_xfree,
    vendor_xinside,
    vendor_xprinter,
    vendor_unknown
} srv_vendor_t;

extern "C" srv_vendor_t sal_GetServerVendor( Display *p_display );

// -=-= SalWM =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
enum SalWM { olwm,      // Open Look
             mwm,       // Motif
             kwm,       // KDE Desktop Environment
             FourDwm,   // SGI
             vuewm,     // HP
             dtwm,      // CDE
             winmgr,    // Oracle NC
             twm,
             fvwm,      // ...
             pmwm,      // SCO
             otherwm };

// -=-= SalRGB -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// MSB/Bigendian Sicht (SalColor == RGB, r=0xFF0000, g=0xFF00, b=0xFF)

enum SalRGB { RGB,  RBG,
              GBR,  GRB,
              BGR,  BRG,
              RGBA, RBGA,
              GBRA, GRBA,
              BGRA, BRGA,
              other };

// -=-= SalVisual =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class SalVisual : public XVisualInfo
{
    STDAPI( SalVisual );

    SalRGB          eRGBMode_;
    int             nRedShift_;
    int             nGreenShift_;
    int             nBlueShift_;
    int             nRedBits_;
    int             nGreenBits_;
    int             nBlueBits_;
public:
                            ~SalVisual();
                            SalVisual( const XVisualInfo* pXVI );

    inline  VisualID        GetVisualId() const { return visualid; }
    inline  Visual         *GetVisual() const { return visual; }
    inline  int             GetClass() const { return c_class; }
    inline  int             GetDepth() const { return depth; }
    inline  SalRGB          GetMode() const { return eRGBMode_; }

            Pixel           GetTCPixel( SalColor nColor ) const;
            SalColor        GetTCColor( Pixel nPixel ) const;
            BOOL            Convert( int &n0, int &n1, int &n2, int &n3 ); // 32bit
             BOOL           Convert( int &n0, int &n1, int &n2 ); // 24bit
};

// -=-= SalColormap/SalColormapRef=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
class SalColormap : public SvRefBase
{
    STDAPI( SalColormap );

    SalDisplay     *pDisplay_;
    Colormap        hColormap_;
    SalColor       *pPalette_;          // Pseudocolor
    SalVisual      *pVisual_;
    USHORT         *pLookupTable_;      // Pseudocolor: 12bit reduction
    Pixel           nWhitePixel_;
    Pixel           nBlackPixel_;
    Pixel           nUsed_;             // Pseudocolor

    void            GetPalette();
    void            GetLookupTable();
public:
    SalColormap( SalDisplay          *pSalDisplay,
                 Colormap             hColormap );
    SalColormap( const BitmapPalette &rpPalette );
    SalColormap( USHORT               nDepth );
    SalColormap();
    virtual                 ~SalColormap();

    inline  Colormap        GetXColormap() const { return hColormap_; }
    inline  SalDisplay     *GetDisplay() const { return pDisplay_; }
    inline  Display        *GetXDisplay() const;
    inline  SalVisual      *GetVisual() const;
    inline  Visual         *GetXVisual() const;
    inline  Pixel           GetWhitePixel() const { return nWhitePixel_; }
    inline  Pixel           GetBlackPixel() const { return nBlackPixel_; }
    inline  Pixel           GetUsed() const { return nUsed_; }
    inline  int             GetClass() const;

    BOOL            GetXPixels( XColor  &rColor,
                                int      r,
                                int      g,
                                int      b ) const;
    inline  BOOL            GetXPixel( XColor  &rColor,
                                       int      r,
                                       int      g,
                                       int      b ) const;
    Pixel           GetPixel( SalColor nColor ) const;
    SalColor        GetColor( Pixel nPixel ) const;
    void            SetPalette( const BitmapPalette &rPalette );
};

SV_DECL_IMPL_REF( SalColormap )

// -=-= SalXLib =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
typedef int(*YieldFunc)(int fd, void* data);
struct YieldEntry;

class SalXLib
{
    STDAPI( SalXLib );

    timeval         Timeout_;
    ULONG           nTimeoutMS_;
    int             pTimeoutFDS_[2];

    int             nStateOfYield_;
    BOOL            bWasXError_;
    BOOL            bIgnoreXErrors_;
    int                         nIgnoreErrorLevel;

    int             nFDs_;
    fd_set          aReadFDS_;
    fd_set          aExceptionFDS_;
    YieldEntry      *pYieldEntries_;

public:
    SalXLib();
    ~SalXLib();
    void            Init( int *pArgc, char *ppArgv[] );

    void            Yield( BOOL bWait );
    void            Wakeup();

    void            Insert( int fd, void* data,
                            YieldFunc   pending,
                            YieldFunc   queued,
                            YieldFunc   handle );
    void            Remove( int fd );

    void            XError( Display *pDisp, XErrorEvent *pEvent );
    inline  void                    PushErrorTrap() { nIgnoreErrorLevel++; }
    inline  void                    PopErrorTrap() { nIgnoreErrorLevel--; }
    inline  BOOL            WasXError() const { return bWasXError_; }
    inline  BOOL            GetIgnoreXErrors() const { return bIgnoreXErrors_; }
    inline  void            SetIgnoreXErrors( BOOL b )
    { bIgnoreXErrors_ = b; bWasXError_ = FALSE; }

    inline  void            StartTimer( ULONG nMS );
    inline  void            StopTimer();

    bool            CheckTimeout( bool bExecuteTimers = true );
};

// -=-= SalXEvent =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
struct SalXEvent
{
             SalXEvent      *pNext_;                // Stack
            XEvent          event_;
};

// -=-= SalDisplay -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

class SalI18N_InputMethod;
class SalI18N_KeyboardExtension;
class XlfdStorage;
class ExtendedFontStruct;
class ExtendedXlfd;
class AttributeProvider;
class SalUnicodeConverter;
class SalConverterCache;

DECLARE_LIST( SalFontCache, ExtendedFontStruct* )

extern "C" {
    struct SnDisplay;
    struct SnLauncheeContext;
};

class SalDisplay
{
    STDAPI( SalDisplay );

    SalXLib        *pXLib_;
    SalI18N_InputMethod         *mpInputMethod;
    SalI18N_KeyboardExtension   *mpKbdExtension;

    AttributeProvider           *mpFactory;
    XlfdStorage                 *mpFontList;
    const ExtendedXlfd          *mpFallbackFactory;

    SalSystemData  *mpSalSystemData;
    // the one to get create and destroy notify events

    Display        *pDisp_;             // X Display
    Screen         *pScreen_;           // XDefaultScreenOfDisplay
    int             nScreen_;           // XDefaultScreen
    SalVisual      *pRootVisual_;       // default visual of screen
    XLIB_Window     hRootWindow_;
    Size            aSize_;             // Screen Size [pixel]
    Pair            aResolution_;       // [dpi]
    bool            mbExactResolution;
    ULONG           nMaxRequestSize_;   // [byte]

    srv_vendor_t    meServerVendor;
    SalWM           eWindowManager_;
    ULONG           nProperties_;       // PROPERTY_SUPPORT, BUG, FEATURE
    BOOL            bLocal_;            // Server==Client? Init
    // in SalDisplay::IsLocal()
    BOOL            mbLocalIsValid;     // bLocal_ is valid ?
    ULONG           nImageDepths_;      // Supported Image Depths
    ULONG           nSharedImages_;     // Supports MIT-SHM Extension
    // until x bytes

    int             nStateOfYield_;
    int             nStateOfSendEvent_;
    oslThreadMutex  hEventGuard_;
    SalXEvent      *pEventQueue_;       // threaded user event queue
    SalXEvent      *pDispatchStack_;    // Dispatch/Yield

    XLIB_Cursor     aPointerCache_[POINTER_COUNT];
    SalFrameData   *pCapture_;

    // GDI
    SalVisual      *pVisual_;           // Visual
    XLIB_Window     hRefWindow_;
    GC              pMonoGC_;
    GC              pCopyGC_;
    GC              pAndInvertedGC_;
    GC              pAndGC_;
    GC              pOrGC_;
    GC              pStippleGC_;
    Pixmap          hInvert50_;
    SalColormapRef  xColor_;

    SalFontCache   *pFontCache_;

    int             nBeepVolume_;       // Sound

    // Keyboard
    BOOL            bNumLockFromXS_;    // Num Lock handled by X Server
    int             nNumLockIndex_;     // modifier index in modmap
    int             nNumLockMask_;      // keyevent state mask for
    KeySym          nShiftKeySym_;      // first shift modifier
    KeySym          nCtrlKeySym_;       // first control modifier
    KeySym          nMod1KeySym_;       // first mod1 modifier
    ByteString      m_aKeyboardName;

    SalBitmapList   Bitmaps_;           // to destroy bitmap resources;

    SalImageList    SharedImages_;

    ::vcl_sal::WMAdaptor*   m_pWMAdaptor;

    bool            m_bXinerama;
    std::vector< Rectangle > m_aXineramaScreens;

    struct SnDisplay           *m_pSnDisplay;
    struct SnLauncheeContext   *m_pSnLauncheeContext;

    void            DestroyFontCache();
    long            Dispatch( XEvent *pEvent );
    void            InitXinerama();
public:
    static  SalDisplay     *GetSalDisplay( Display* display );
    static  BOOL            BestVisual( Display     *pDisp,
                                        int          nScreen,
                                        XVisualInfo &rVI );

                            SalDisplay( Display* pDisp,
                                        Visual* pVisual = NULL,
                                        Colormap aColMap = None);

    ~SalDisplay();

    void            Init( Colormap hXColmap, const XVisualInfo* pXVI );

    BOOL            IsEvent();
    void            SendEvent( Atom         aEvent,
                               void        *pData,
                               XLIB_Window  hReceiver = 0 ) const;
    void            SendEvent( Atom         aEvent,
                               UINT32      *pData = 0,
                               XLIB_Window  hReceiver = 0 ) const;
    void            Yield( BOOL bWait );
    void            PrintInfo() const;

    void            PrintEvent( const ByteString &rComment,
                                XEvent       *pEvent ) const;

    void            AddFontPath( const ByteString &rPath ) const;
    XlfdStorage*    GetXlfdList();
    ExtendedFontStruct*
    GetFont( const ExtendedXlfd *pFont,
             int nPixelSize, sal_Bool bVertical );
    const ExtendedXlfd*
    GetFallbackFactory()
    { return mpFallbackFactory; }

    void            Beep() const;

    void            ModifierMapping();
    String          GetKeyNameFromKeySym( KeySym keysym ) const;
    XubString       GetKeyName( USHORT nKeyCode ) const;
    USHORT          GetKeyCode( KeySym keysym, char*pcPrintable ) const;
    KeySym          GetKeySym( XKeyEvent      *pEvent,
                               unsigned char  *pPrintable,
                               int            *pLen,
                               Status         *pStatus,
                               XIC = NULL ) const;

    XLIB_Cursor     GetPointer( int ePointerStyle );
    int             CaptureMouse( SalFrameData *pCapture );

    BOOL            IsLocal();
    inline  void            Insert( SalBitmap *pBitmap );
    inline  void            Remove( SalBitmap *pBitmap );

    inline  SalImage       *Seek( SalImage *pImage );
    inline  void            Insert( SalImage *pImage );
    inline  void            Remove( SalImage *pImage );
    void            Remove( XEvent   *pEvent );

    XLIB_Window     GetRootWindow() const { return hRootWindow_; }
    XLIB_Window     GetDrawable() const { return hRefWindow_; }
    Display        *GetDisplay() const { return pDisp_; }
    int             GetScreenNumber() const { return nScreen_; }
    srv_vendor_t    GetServerVendor() const { return meServerVendor; }
    void            SetServerVendor() { meServerVendor = sal_GetServerVendor(pDisp_); }
    BOOL            IsDisplay() const { return !!pXLib_; }
    GC              GetMonoGC() const { return pMonoGC_; }
    GC              GetCopyGC() const { return pCopyGC_; }
    GC              GetAndInvertedGC() const { return pAndInvertedGC_; }
    GC              GetAndGC() const { return pAndGC_; }
    GC              GetOrGC() const { return pOrGC_; }
    GC              GetStippleGC() const { return pStippleGC_; }
    GC              GetGC( USHORT nDepth ) const;
    Pixmap          GetInvert50() const { return hInvert50_; }
    SalColormap    &GetColormap() const { return *xColor_; }
    SalVisual      *GetVisual() const { return pVisual_; }
    SalVisual      *GetRootVisual() const { return pRootVisual_; }
    const Size     &GetScreenSize() const { return aSize_; }
    const Pair     &GetResolution() const { return aResolution_; }
    bool            GetExactResolution() const { return mbExactResolution; }
    ULONG           GetProperties() const { return nProperties_; }
    ULONG           GetMaxRequestSize() const { return nMaxRequestSize_; }
    ULONG           GetImageDepths() const { return nImageDepths_; }
    ULONG           SupportsShm() const { return nSharedImages_; }
    void            DisableShm() { nSharedImages_ /= 2; } // = 0
    void            GetScreenFontResolution( long& rDPIX, long& rDPIY ) const;

    BOOL            MouseCaptured( const SalFrameData *pFrameData ) const
    { return pCapture_ == pFrameData; }
    SalFrameData*   GetCaptureFrame() const
    { return pCapture_; }
    SalXLib*         GetXLib() { return pXLib_; }

    SalI18N_InputMethod*            GetInputMethod()  { return mpInputMethod;  }
    SalI18N_KeyboardExtension*  GetKbdExtension() { return mpKbdExtension; }
    void            SetInputMethod( SalI18N_InputMethod *pInputMethod )
    { mpInputMethod = pInputMethod; }
    void            SetKbdExtension(SalI18N_KeyboardExtension *pKbdExtension)
    { mpKbdExtension = pKbdExtension; }
    const char* GetKeyboardName( BOOL bRefresh = FALSE );
    ::vcl_sal::WMAdaptor* getWMAdaptor() const { return m_pWMAdaptor; }
    bool            IsXinerama() const { return m_bXinerama; }
    const std::vector< Rectangle >& GetXineramaScreens() const { return m_aXineramaScreens; }
};

// -=-= inlines =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

inline void SalDisplay::Remove( SalBitmap *pBitmap )
{ Bitmaps_.Remove( pBitmap ); }

inline void SalDisplay::Insert( SalBitmap *pBitmap )
{ Bitmaps_.Insert( pBitmap ); }

inline SalImage *SalDisplay::Seek( SalImage *pImage )
{ return SharedImages_.Seek( pImage ); }

inline void SalDisplay::Remove( SalImage *pImage )
{ SharedImages_.Remove( pImage ); }

inline void SalDisplay::Insert( SalImage *pImage )
{ SharedImages_.Insert( pImage ); }

inline GC SalDisplay::GetGC( USHORT nDepth ) const
{ return 1 == nDepth
         ? pMonoGC_
         : pVisual_->GetDepth() == nDepth
           ? pCopyGC_
           : NULL; }

inline  Display *SalColormap::GetXDisplay() const
{ return pDisplay_->GetDisplay(); }

inline SalVisual *SalColormap::GetVisual() const
{ return pVisual_ ? pVisual_ : pDisplay_->GetVisual(); }

inline Visual *SalColormap::GetXVisual() const
{ return GetVisual()->GetVisual(); }

inline int SalColormap::GetClass() const
{ return pVisual_ ? pVisual_->GetClass() : PseudoColor; }

/*----------------------------------------------------------
 keep track of correct size of the initial window
 */
// get foreign key names
namespace vcl_sal {
    String getKeysymReplacementName(
        const char* pKeyboard,
        KeySym nSymbol );
}


#endif // _SV_SALDISP_HXX

