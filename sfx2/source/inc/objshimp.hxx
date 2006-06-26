/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: objshimp.hxx,v $
 *
 *  $Revision: 1.33 $
 *
 *  last change: $Author: rt $ $Date: 2006-06-26 09:52:23 $
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
#ifndef _SFX_OBJSHIMP_HXX
#define _SFX_OBJSHIMP_HXX

//#include <hash_map>

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif
#ifndef _DATETIME_HXX
#include <tools/datetime.hxx>
#endif

#include <svtools/securityoptions.hxx>
#include "objsh.hxx"
#include "bitset.hxx"

namespace svtools { class AsynchronLink; }

//====================================================================

DBG_NAMEEX(SfxObjectShell)

class SfxViewFrame;
struct MarkData_Impl
{
    String aMark;
    String aUserData;
    SfxViewFrame* pFrame;
};

class SfxFrame;
class SfxDialogLibraryContainer;
class SfxScriptLibraryContainer;
class SfxToolBoxConfig;
class SfxAcceleratorManager;
struct SfxObjectShell_Impl
{
    ::comphelper::EmbeddedObjectContainer* mpObjectContainer;
    SfxAcceleratorManager*  pAccMgr;
    SfxDocumentInfo*    pDocInfo;
    SfxConfigManager*   pCfgMgr;
    BasicManager*       pBasicMgr;          // Doc-BASIC oder 0
    SfxScriptLibraryContainer* pBasicLibContainer;
    SfxDialogLibraryContainer* pDialogLibContainer;
    SfxProgress*        pProgress;
    String              aTitle;
    String              aTempName;
    DateTime            nTime;
    sal_uInt16          nVisualDocumentNumber;
    sal_Int16           nDocumentSignatureState;
    sal_Int16           nScriptingSignatureState;
    sal_Bool            bTemplateConfig:1,
                        bInList:1,          // ob per First/Next erreichbar
                        bClosing:1,         // sal_True w"aehrend Close(), um Benachrichtigungs-Rekursionen zu verhindern
                        bSetInPlaceObj:1,   // sal_True, falls bereits versucht wurde pInPlaceObject zu casten
                        bIsSaving:1,
                        bPasswd:1,
                        bIsTmp:1,
                        bIsNamedVisible:1,
                        bIsTemplate:1,
                        bIsAbortingImport:1,  // Importvorgang soll abgebrochen werden.
                        bImportDone : 1, //Import schon fertig? Fuer AutoReload von Docs.
                        bInPrepareClose : 1,
                        bPreparedForClose : 1,
                        bWaitingForPicklist : 1,// Muss noch in die Pickliste
                        bModuleSearched : 1,
                        bIsBasicDefault : 1,
                        bIsHelpObjSh : 1,
                        bForbidCaching : 1,
                        bForbidReload : 1,
                        bSupportsEventMacros: 1,
                        bLoadingWindows: 1,
                        bBasicInitialized :1,
                        //bHidden :1, // indicates a hidden view shell
                        bIsPrintJobCancelable :1, // Stampit disable/enable cancel button for print jobs ... default = true = enable!
                        bOwnsStorage:1,
                        bNoBaseURL:1,
                        bInitialized:1,
                        bSignatureErrorIsShown:1,
                        bModelInitialized:1, // whether the related model is initialized
                        bPreserveVersions:1,
                        m_bMacroSignBroken:1; // whether the macro signature was explicitly broken

    String              aNewName;  // Der Name, unter dem das Doc gespeichert
                                   // werden soll
    IndexBitSet         aBitSet;
    sal_uInt32               lErr;
    sal_uInt16              nEventId;           // falls vor Activate noch ein
                                            // Open/Create gesendet werden mu/s
    sal_Bool                bDoNotTouchDocInfo;

    AutoReloadTimer_Impl *pReloadTimer;
    MarkData_Impl*      pMarkData;
    sal_uInt16              nLoadedFlags;
    sal_uInt16              nFlagsInProgress;
    String              aMark;
    Size                aViewSize;          // wird leider vom Writer beim
    sal_Bool                bInFrame;           // HTML-Import gebraucht
    sal_Bool                bModalMode;
    sal_Bool                bRunningMacro;
    sal_Bool                bReloadAvailable;
    sal_uInt16              nAutoLoadLocks;
    SfxModule*              pModule;
    SfxFrame*               pFrame;
    SfxToolBoxConfig*       pTbxConfig;
    SfxEventConfigItem_Impl* pEventConfig;
    SfxObjectShellFlags     eFlags;
    svtools::AsynchronLink* pCloser;
    String                  aBaseURL;
    sal_Bool                bReadOnlyUI;
    SvRefBaseRef            xHeaderAttributes;
    sal_Bool                bHiddenLockedByAPI;
    sal_Bool                bInCloseEvent;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel >             xModel;
    sal_uInt16              nStyleFilter;
    sal_Int16                nMacroMode;
    sal_Bool                bDisposing;

    sal_Bool                bMacroDisabled;
    sal_Bool                bMacroDisabledMessageIsShown;

    sal_Bool                m_bEnableSetModified;
    sal_Bool                m_bIsModified;

    Rectangle               m_aVisArea;
    MapUnit                 m_nMapUnit;

    sal_Bool                m_bCreateTempStor;
    ::com::sun::star::uno::Reference< ::com::sun::star::embed::XStorage > m_xDocStorage;

    sal_Bool                m_bIsInit;


    SfxObjectShell_Impl() :
              mpObjectContainer(0),
              pAccMgr(0),
              pDocInfo ( 0),
              pCfgMgr( 0),
              pBasicMgr( 0),
              pBasicLibContainer( 0 ),
              pDialogLibContainer( 0 ),
              pProgress( 0),
              nTime(),
              nVisualDocumentNumber( USHRT_MAX),
              nDocumentSignatureState( SIGNATURESTATE_UNKNOWN ),
              nScriptingSignatureState( SIGNATURESTATE_UNKNOWN ),
              bTemplateConfig( sal_False),
              bInList( sal_False),
              bClosing( sal_False),
              bSetInPlaceObj( sal_False),
              bIsSaving( sal_False),
              bPasswd( sal_False),
              bIsTmp( sal_False),
              bIsNamedVisible( sal_False),
              bIsTemplate(sal_False),
              bIsAbortingImport ( sal_False),
              bImportDone ( sal_False),
              bInPrepareClose( sal_False ),
              bPreparedForClose( sal_False ),
              bWaitingForPicklist( sal_False ),
              bModuleSearched( sal_False ),
              bIsBasicDefault( sal_True ),
              bIsHelpObjSh( sal_False ),
              bForbidCaching( sal_False ),
              bForbidReload( sal_False ),
              bSupportsEventMacros( sal_True ),
              bLoadingWindows( sal_False ),
              bBasicInitialized( sal_False ),
//              bHidden( sal_False ),
              bIsPrintJobCancelable( sal_True ),
              bOwnsStorage( sal_True ),
              bNoBaseURL( sal_False ),
              bInitialized( sal_False ),
              bSignatureErrorIsShown( sal_False ),
              bModelInitialized( sal_False ),
              bPreserveVersions( sal_True ),
        lErr(ERRCODE_NONE),
        nEventId ( 0),
        bDoNotTouchDocInfo( sal_False ),
        pReloadTimer ( 0),
        pMarkData( 0 ),
        nLoadedFlags ( SFX_LOADED_MAINDOCUMENT ),
        nFlagsInProgress( 0 ),
        bInFrame( sal_False ),
        bModalMode( sal_False ),
              bRunningMacro( sal_False ),
              bReloadAvailable( sal_False ),
        nAutoLoadLocks( 0 ),
              pModule( 0 ),
        pFrame( 0 ),
              pTbxConfig( 0 ),
        pEventConfig(NULL),
        eFlags( SFXOBJECTSHELL_UNDEFINED ),
        pCloser( 0 ),
        bReadOnlyUI( sal_False ),
        bHiddenLockedByAPI( sal_False ),
              bInCloseEvent( sal_False )
              , nStyleFilter( 0 )
              , nMacroMode( -1 )
        , bDisposing( sal_False )
        , bMacroDisabled( sal_False )
        , bMacroDisabledMessageIsShown( sal_False )
        , m_bEnableSetModified( sal_True )
        , m_bIsModified( sal_False )
        , m_nMapUnit( MAP_100TH_MM )
        , m_bCreateTempStor( sal_False )
        , m_bIsInit( sal_False )
    {}
    ~SfxObjectShell_Impl();

};

#endif

