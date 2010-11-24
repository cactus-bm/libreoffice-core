/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#include "apearcfg.hxx"
#include "com/sun/star/uno/Any.hxx"

#include "tools/debug.hxx"
#include "vcl/settings.hxx"
#include "vcl/svapp.hxx"
#include <rtl/logfile.hxx>

#define DEFAULT_LOOKNFEEL   0
#define DEFAULT_DRAGMODE    2
#define DEFAULT_SNAPMODE    0
#define DEFAULT_SCALEFACTOR 100
#define DEFAULT_AAMINHEIGHT 8

using namespace ::rtl;
using namespace ::com::sun::star::uno;

sal_Bool SvtTabAppearanceCfg::bInitialized = sal_False;

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/
SvtTabAppearanceCfg::SvtTabAppearanceCfg()
    :ConfigItem(OUString::createFromAscii("Office.Common/View"))
    ,nDragMode          ( DEFAULT_DRAGMODE )
    ,nScaleFactor       ( DEFAULT_SCALEFACTOR )
    ,nSnapMode          ( DEFAULT_SNAPMODE )
    ,nMiddleMouse       ( MOUSE_MIDDLE_AUTOSCROLL )
#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    ,nAAMinPixelHeight  ( DEFAULT_AAMINHEIGHT )
#endif
    ,bMenuMouseFollow(FALSE)
#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    ,bFontAntialiasing  ( TRUE )
#endif
{
    RTL_LOGFILE_CONTEXT(aLog, "svtools SvtTabAppearanceCfg::SvtTabAppearanceCfg()");

    const Sequence<OUString>& rNames = GetPropertyNames();
    Sequence<Any> aValues = GetProperties(rNames);
    const Any* pValues = aValues.getConstArray();
    DBG_ASSERT(aValues.getLength() == rNames.getLength(), "GetProperties failed");

    if(aValues.getLength() == rNames.getLength())
    {
        for(int nProp = 0; nProp < rNames.getLength(); ++nProp, ++pValues)
        {
            if(pValues->hasValue())
            {
                switch(nProp)
                {
                    case  0: *pValues >>= nScaleFactor; break; //"FontScaling",
                    case  1: *pValues >>= nDragMode; break;   //"Window/Drag",
                    case  2: bMenuMouseFollow = *(sal_Bool*)pValues->getValue(); break; //"Menu/FollowMouse",
                    case  3: *pValues >>= nSnapMode; break; //"Dialog/MousePositioning",
                    case  4: *pValues >>= nMiddleMouse; break; //"Dialog/MiddleMouseButton",
#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
                    case  5: bFontAntialiasing = *(sal_Bool*)pValues->getValue(); break;    // "FontAntialising/Enabled",
                    case  6: *pValues >>= nAAMinPixelHeight; break;                         // "FontAntialising/MinPixelHeight",
#endif
                }
            }
        }
    }
}
/* -----------------------------22.05.01 11:53--------------------------------

 ---------------------------------------------------------------------------*/
SvtTabAppearanceCfg::~SvtTabAppearanceCfg( )
{
}
/* -----------------------------22.05.01 11:54--------------------------------

 ---------------------------------------------------------------------------*/
const Sequence<OUString>& SvtTabAppearanceCfg::GetPropertyNames()
{
    static Sequence<OUString> aNames;
    if(!aNames.getLength())
    {
        static const sal_Char* aPropNames[] =
        {
             "FontScaling"                       //  0
            ,"Window/Drag"                       //  1
            ,"Menu/FollowMouse"                  //  2
            ,"Dialog/MousePositioning"           //  3
            ,"Dialog/MiddleMouseButton"          //  4
#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
            ,"FontAntiAliasing/Enabled"         //  5
            ,"FontAntiAliasing/MinPixelHeight"  //  6
#endif
        };
        const int nCount = sizeof( aPropNames ) / sizeof( aPropNames[0] );
        aNames.realloc(nCount);

        const sal_Char** pAsciiNames = aPropNames;
        OUString* pNames = aNames.getArray();
        for(int i = 0; i < nCount; ++i, ++pNames, ++pAsciiNames)
            *pNames = OUString::createFromAscii( *pAsciiNames );
    }
    return aNames;
}
/* -----------------------------22.05.01 11:54--------------------------------

 ---------------------------------------------------------------------------*/
void  SvtTabAppearanceCfg::Commit()
{
    const Sequence<OUString>& rNames = GetPropertyNames();
    Sequence<Any> aValues(rNames.getLength());
    Any* pValues = aValues.getArray();

    const Type& rType = ::getBooleanCppuType();
    for(int nProp = 0; nProp < rNames.getLength(); nProp++)
    {
        switch(nProp)
        {
            case  0: pValues[nProp] <<= nScaleFactor; break;            // "FontScaling",
            case  1: pValues[nProp] <<= nDragMode; break;               //"Window/Drag",
            case  2: pValues[nProp].setValue(&bMenuMouseFollow, rType); break; //"Menu/FollowMouse",
            case  3: pValues[nProp] <<= nSnapMode; break;               //"Dialog/MousePositioning",
            case  4: pValues[nProp] <<= nMiddleMouse; break;               //"Dialog/MiddleMouseButton",
#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
            case  5: pValues[nProp].setValue(&bFontAntialiasing, rType); break; // "FontAntialising/Enabled",
            case  6: pValues[nProp] <<= nAAMinPixelHeight; break;               // "FontAntialising/MinPixelHeight",
#endif
        }
    }
    PutProperties(rNames, aValues);
}

void SvtTabAppearanceCfg::Notify( const com::sun::star::uno::Sequence< rtl::OUString >& )
{
}

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

void SvtTabAppearanceCfg::SetDragMode  ( USHORT nSet )
{
    nDragMode = nSet;
    SetModified();
}

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

void SvtTabAppearanceCfg::SetScaleFactor ( USHORT nSet )
{
    nScaleFactor = nSet;
    SetModified();
}

/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

void SvtTabAppearanceCfg::SetSnapMode ( USHORT nSet )
{
    nSnapMode = nSet;
    SetModified();
}
/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/
void SvtTabAppearanceCfg::SetMiddleMouseButton ( USHORT nSet )
{
    nMiddleMouse = nSet;
    SetModified();
}
/*--------------------------------------------------------------------
     Beschreibung:
 --------------------------------------------------------------------*/

void SvtTabAppearanceCfg::SetApplicationDefaults ( Application* pApp )
{
    AllSettings   hAppSettings = pApp->GetSettings();
    StyleSettings hAppStyle    = hAppSettings.GetStyleSettings();

    // Look & Feel

    // SetStandard...Styles() resets the UseSystemUIFonts flag,
    // but we don't want to change it now, so save the flag before ...
    BOOL bUseSystemUIFonts = hAppStyle.GetUseSystemUIFonts();
    hAppStyle.SetStandardStyles();
    // and set it here
    hAppStyle.SetUseSystemUIFonts( bUseSystemUIFonts );

    // Screen and ScreenFont Scaling

    hAppStyle.SetScreenZoom( nScaleFactor );
    hAppStyle.SetScreenFontZoom( nScaleFactor );

#if defined( UNX ) || defined ( FS_PRIV_DEBUG )
    // font anti aliasing
    hAppStyle.SetAntialiasingMinPixelHeight( nAAMinPixelHeight );
    hAppStyle.SetDisplayOptions( bFontAntialiasing ? 0 : DISPLAY_OPTION_AA_DISABLE );
#endif

    // Mouse Snap

    MouseSettings hMouseSettings = hAppSettings.GetMouseSettings();
    ULONG         nMouseOptions  = hMouseSettings.GetOptions();

    nMouseOptions &=  ! (MOUSE_OPTION_AUTOCENTERPOS | MOUSE_OPTION_AUTODEFBTNPOS);

    switch ( nSnapMode )
    {
    case SnapToButton:
        nMouseOptions |= MOUSE_OPTION_AUTODEFBTNPOS;
        break;
    case SnapToMiddle:
        nMouseOptions |= MOUSE_OPTION_AUTOCENTERPOS;
        break;
    case NoSnap:
    default:
        break;
    }
    hMouseSettings.SetOptions(nMouseOptions);
    hMouseSettings.SetMiddleButtonAction(nMiddleMouse);

    // Merge and Publish Settings

    ULONG nFollow = hMouseSettings.GetFollow();
    if(bMenuMouseFollow)
        nFollow |= MOUSE_FOLLOW_MENU;
    else
        nFollow &= ~MOUSE_FOLLOW_MENU;
    hMouseSettings.SetFollow( nFollow );

    hAppSettings.SetMouseSettings( hMouseSettings );

    hAppSettings.SetStyleSettings( hAppStyle );
    pApp->MergeSystemSettings    ( hAppSettings );      // Allow system-settings to apply
    pApp->SystemSettingsChanging ( hAppSettings, NULL );// Allow overruling of system-settings
                                                        //is concerned with window drag

    pApp->SetSettings ( hAppSettings );
}



