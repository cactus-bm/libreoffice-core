/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: objmisc.cxx,v $
 *
 *  $Revision: 1.88 $
 *
 *  last change: $Author: ihi $ $Date: 2007-04-19 09:28:56 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sfx2.hxx"

#ifndef _INETMSG_HXX //autogen
#include <svtools/inetmsg.hxx>
#endif
#ifndef _SFXENUMITEM_HXX //autogen
#include <svtools/eitem.hxx>
#endif
#ifndef _SFXSTRITEM_HXX //autogen
#include <svtools/stritem.hxx>
#endif
#ifndef _SFXINTITEM_HXX //autogen
#include <svtools/intitem.hxx>
#endif
#include <vos/mutex.hxx>

#ifndef GCC
#endif

#ifndef _COM_SUN_STAR_DOCUMENT_UPDATEDOCMODE_HPP_
#include <com/sun/star/document/UpdateDocMode.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XTYPECONVERTER_HPP_
#include <com/sun/star/script/XTypeConverter.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_FINISHENGINEEVENT_HPP_
#include <com/sun/star/script/FinishEngineEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_INTERRUPTREASON_HPP_
#include <com/sun/star/script/InterruptReason.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XENGINELISTENER_HPP_
#include <com/sun/star/script/XEngineListener.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XDEBUGGING_HPP_
#include <com/sun/star/script/XDebugging.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XINVOKATION_HPP_
#include <com/sun/star/script/XInvocation.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_CONTEXTINFORMATION_HPP_
#include <com/sun/star/script/ContextInformation.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_FINISHREASON_HPP_
#include <com/sun/star/script/FinishReason.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XENGINE_HPP_
#include <com/sun/star/script/XEngine.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_INTERRUPTENGINEEVENT_HPP_
#include <com/sun/star/script/InterruptEngineEvent.hpp>
#endif
#ifndef _COM_SUN_STAR_SCRIPT_XLIBRARYACCESS_HPP_
#include <com/sun/star/script/XLibraryAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_DOCUMENT_MACROEXECMODE_HPP_
#include <com/sun/star/document/MacroExecMode.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_EMBEDSTATES_HPP_
#include <com/sun/star/embed/EmbedStates.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XMODIFIABLE_HPP_
#include <com/sun/star/util/XModifiable.hpp>
#endif

#include <com/sun/star/script/provider/XScript.hpp>
#include <com/sun/star/script/provider/XScriptProvider.hpp>
#include <com/sun/star/script/provider/XScriptProviderSupplier.hpp>

#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/unohlp.hxx>
#endif

#include <com/sun/star/uno/Reference.h>
#include <com/sun/star/uno/Any.h>
#include <com/sun/star/ucb/XContent.hpp>
#include <com/sun/star/task/ErrorCodeRequest.hpp>
#include <svtools/securityoptions.hxx>

#include <comphelper/processfactory.hxx>

#include <com/sun/star/security/XDocumentDigitalSignatures.hpp>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::ucb;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::script;

#ifndef _SB_SBUNO_HXX
#include <basic/sbuno.hxx>
#endif
#ifndef _SB_SBSTAR_HXX
#include <basic/sbstar.hxx>
#endif
#ifndef _SB_BASMGR_HXX
#include <basic/basmgr.hxx>
#endif
#ifndef _VCL_MSGBOX_HXX
#include <vcl/msgbox.hxx>
#endif
#ifndef _SBXCLASS_HXX //autogen
#include <basic/sbx.hxx>
#endif
#ifndef _SFXECODE_HXX
#include <svtools/sfxecode.hxx>
#endif
#ifndef _EHDL_HXX
#include <svtools/ehdl.hxx>
#endif

#include <svtools/pathoptions.hxx>
#include <unotools/ucbhelper.hxx>
#include <tools/inetmime.hxx>
#include <tools/urlobj.hxx>
#include <svtools/inettype.hxx>
#include <osl/file.hxx>
#include <vcl/svapp.hxx>
#include <framework/interaction.hxx>

#include "app.hxx"
#include "appdata.hxx"
#include "request.hxx"
#include "bindings.hxx"
#include "sfxresid.hxx"
#include "docfile.hxx"
#include "docinf.hxx"
#include "docfilt.hxx"
#include "objsh.hxx"
#include "objshimp.hxx"
#include "event.hxx"
#include "fltfnc.hxx"
#include "sfx.hrc"
#include "dispatch.hxx"
#include "viewfrm.hxx"
#include "viewsh.hxx"
#include "ctrlitem.hxx"
#include "arrdecl.hxx"
#include "module.hxx"
#include "macrconf.hxx"
#include "docfac.hxx"
#include "helper.hxx"
#include "doc.hrc"
#include "workwin.hxx"
#include "helpid.hrc"
#include "../appl/app.hrc"
#include "secmacrowarnings.hxx"
#include "sfxdlg.hxx"
#include "appbaslib.hxx"

using namespace ::com::sun::star;

// class SfxHeaderAttributes_Impl ----------------------------------------

class SfxHeaderAttributes_Impl : public SvKeyValueIterator
{
private:
    SfxObjectShell* pDoc;
    SvKeyValueIteratorRef xIter;
    sal_Bool bAlert;

public:
    SfxHeaderAttributes_Impl( SfxObjectShell* pSh ) :
        SvKeyValueIterator(), pDoc( pSh ),
        xIter( pSh->GetMedium()->GetHeaderAttributes_Impl() ),
        bAlert( sal_False ) {}

    virtual sal_Bool GetFirst( SvKeyValue& rKV ) { return xIter->GetFirst( rKV ); }
    virtual sal_Bool GetNext( SvKeyValue& rKV ) { return xIter->GetNext( rKV ); }
    virtual void Append( const SvKeyValue& rKV );

    void ClearForSourceView() { xIter = new SvKeyValueIterator; bAlert = sal_False; }
    void SetAttributes();
    void SetAttribute( const SvKeyValue& rKV );
};

//=========================================================================

sal_uInt16 __READONLY_DATA aTitleMap_Impl[3][2] =
{
                                //  local               remote
    /*  SFX_TITLE_CAPTION   */  {   SFX_TITLE_FILENAME, SFX_TITLE_TITLE },
#ifdef MAC
    /*  SFX_TITLE_PICKLIST  */  {   SFX_TITLE_FILENAME, SFX_TITLE_FULLNAME },
#else
    /*  SFX_TITLE_PICKLIST  */  {   32,                 SFX_TITLE_FULLNAME },
#endif
    /*  SFX_TITLE_HISTORY   */  {   32,                 SFX_TITLE_FULLNAME }
};

//=========================================================================

void SfxObjectShell::AbortImport()
{
    pImp->bIsAbortingImport = sal_True;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsAbortingImport() const
{
    return pImp->bIsAbortingImport;
}

//-------------------------------------------------------------------------

#if SUPD<604
void SfxObjectShell::NotifyReloadAvailable()
{
}
#endif

//-------------------------------------------------------------------------

SfxDocumentInfo& SfxObjectShell::GetDocInfo()
{
    if( !pImp->pDocInfo )
    {
        pImp->pDocInfo = new SfxDocumentInfo;
        pImp->pDocInfo->SetReadOnly( IsReadOnly() );
    }

    return *pImp->pDocInfo;
}

//-------------------------------------------------------------------------

void SfxObjectShell::FlushDocInfo()
{
    if ( IsLoading() )
        return;

    SetModified(sal_True);
    SfxDocumentInfo &rInfo = GetDocInfo();
    Broadcast( SfxDocumentInfoHint( &rInfo ) );
    SetAutoLoad( INetURLObject(rInfo.GetReloadURL()),
        rInfo.GetReloadDelay() * 1000, rInfo.IsReloadEnabled() );
/*
    // bitte beachten:
    // 1. Titel in DocInfo aber nicht am Doc (nach HTML-Import)
    //  => auch am Doc setzen
    // 2. Titel in DocInfo leer (Briefumschlagsdruck)
    //  => nicht am Doc setzen, da sonst "unbenanntX" daraus wird
    String aDocInfoTitle = GetDocInfo().GetTitle();
    if ( aDocInfoTitle.Len() )
        SetTitle( aDocInfoTitle );
    else
    {
        pImp->aTitle.Erase();
        SetNamedVisibility_Impl();
        if ( GetMedium() )
        {
            SfxShell::SetName( GetTitle(SFX_TITLE_APINAME) );
            Broadcast( SfxSimpleHint(SFX_HINT_TITLECHANGED) );
        }
    }*/
}

//-------------------------------------------------------------------------

void SfxObjectShell::SetError(sal_uInt32 lErr)
{
    if(pImp->lErr==ERRCODE_NONE)
        pImp->lErr=lErr;
}

//-------------------------------------------------------------------------

sal_uInt32 SfxObjectShell::GetError() const
{
    return ERRCODE_TOERROR(GetErrorCode());
}

//-------------------------------------------------------------------------

sal_uInt32 SfxObjectShell::GetErrorCode() const
{
    sal_uInt32 lError=pImp->lErr;
    if(!lError && GetMedium())
        lError=GetMedium()->GetErrorCode();
//REMOVE        if(!lError && HasStorage())
//REMOVE            lError= GetStorage()->GetErrorCode();
    return lError;
}

//-------------------------------------------------------------------------

void SfxObjectShell::ResetError()
{
    pImp->lErr=0;
    SfxMedium * pMed = GetMedium();
    if( pMed )
        pMed->ResetError();
//REMOVE        SvStorage *pStor= HasStorage() ? GetStorage() : 0;
//REMOVE        if( pStor )
//REMOVE            pStor->ResetError();
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsTemplate() const
{
    return pImp->bIsTemplate;
}

//-------------------------------------------------------------------------

void SfxObjectShell::SetTemplate(sal_Bool bIs)
{
    pImp->bIsTemplate=bIs;
    SfxFilterMatcher aMatcher( GetFactory().GetFactoryName() );
    SfxFilterMatcherIter aIter( &aMatcher, SFX_FILTER_TEMPLATEPATH );
    SfxMedium* pMed = GetMedium();
    if( pMed ) pMed->SetFilter( aIter.First() );
}

//-------------------------------------------------------------------------

void SfxObjectShell::EnableSetModified( sal_Bool bEnable )
{
#ifdef DBG_UTIL
    if ( bEnable == pImp->m_bEnableSetModified )
        DBG_WARNING( "SFX_PERSIST: EnableSetModified 2x mit dem gleichen Wert gerufen" );
#endif
    pImp->m_bEnableSetModified = bEnable;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsEnableSetModified() const
{
    return pImp->m_bEnableSetModified && !IsReadOnly();
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsModified()
{
    if ( pImp->m_bIsModified )
        return sal_True;

    if ( !pImp->m_xDocStorage.is() )
    {
        // if the document still has no storage and is not set to be modified explicitly it is not modified
        return sal_False;
    }

    uno::Sequence < ::rtl::OUString > aNames = GetEmbeddedObjectContainer().GetObjectNames();
    for ( sal_Int32 n=0; n<aNames.getLength(); n++ )
    {
        uno::Reference < embed::XEmbeddedObject > xObj = GetEmbeddedObjectContainer().GetEmbeddedObject( aNames[n] );
        OSL_ENSURE( xObj.is(), "An empty entry in the embedded objects list!\n" );
        if ( xObj.is() )
        {
            try
            {
                sal_Int32 nState = xObj->getCurrentState();
                if ( nState != embed::EmbedStates::LOADED )
                {
                    uno::Reference< util::XModifiable > xModifiable( xObj->getComponent(), uno::UNO_QUERY );
                    if ( xModifiable.is() && xModifiable->isModified() )
                        return sal_True;
                }
            }
            catch( uno::Exception& )
            {}
        }
    }

    return sal_False;
}

//-------------------------------------------------------------------------

void SfxObjectShell::SetModified( sal_Bool bModifiedP )
{
#ifdef DBG_UTIL
    if ( !bModifiedP && !IsEnableSetModified() )
        DBG_WARNING( "SFX_PERSIST: SetModified( sal_False ), obwohl IsEnableSetModified() == sal_False" )
#endif

    if( !IsEnableSetModified() )
        return;

    if( pImp->m_bIsModified != bModifiedP )
    {
        pImp->m_bIsModified = bModifiedP;
        ModifyChanged();
    }

//REMOVE        pImp->m_aModifiedTime = Time();
}

//-------------------------------------------------------------------------

void SfxObjectShell::ModifyChanged()

/*  [Beschreibung]

    Diese virtuelle Methode wird aus der virtuellen Basisklasse SvPersist
    gerufen, wenn sich das Modified-Flag ge"andert hat. Diese Querverbindung
    ist notwendig, da aus einem Zweig einer virtuellen Vererbung nicht
    quer un den anderen gerufen werden kann.
*/

{
    if ( pImp->bClosing )
        // SetModified aus dem dispose des Models!
        return;

    {DBG_CHKTHIS(SfxObjectShell, 0);}
    SfxObjectShell *pDoc;
    for ( pDoc = SfxObjectShell::GetFirst(); pDoc;
          pDoc = SfxObjectShell::GetNext(*pDoc) )
        if( pDoc->IsModified() )
            break;

    SfxViewFrame* pViewFrame = SfxViewFrame::Current();
    if ( pViewFrame )
        pViewFrame->GetBindings().Invalidate( SID_SAVEDOCS );


    Invalidate( SID_SIGNATURE );
    Invalidate( SID_MACRO_SIGNATURE );
    Broadcast( SfxSimpleHint( SFX_HINT_TITLECHANGED ) );    // xmlsec05, signed state might change in title...

    SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_MODIFYCHANGED, this ) );
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsReadOnlyUI() const

/*  [Beschreibung]

    Liefert sal_True, wenn das Dokument fuer die UI wie r/o behandelt werden
    soll. Dieses ist unabhaengig vom tatsaechlichen r/o, welches per
    <IsReadOnly()> erfragbar ist.
*/

{
    return pImp->bReadOnlyUI;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsReadOnlyMedium() const

/*  [Beschreibung]

    Liefert sal_True, wenn das Medium r/o ist bzw. r/o geoeffnet wurde.
*/

{
    if ( !pMedium )
        return sal_True;
    return pMedium->IsReadOnly();
}

//-------------------------------------------------------------------------

void SfxObjectShell::SetReadOnlyUI( sal_Bool bReadOnly )

/*  [Beschreibung]

    Schaltet das Dokument in einen r/o bzw. r/w Zustand ohne es neu
    zu laden und ohne die Open-Modi des Mediums zu aendern.
*/

{
    sal_Bool bWasRO = IsReadOnly();
    pImp->bReadOnlyUI = bReadOnly;
    if ( bWasRO != IsReadOnly() )
    {
        Broadcast( SfxSimpleHint(SFX_HINT_MODECHANGED) );
        if ( pImp->pDocInfo )
            pImp->pDocInfo->SetReadOnly( IsReadOnly() );
    }
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsReadOnly() const
{
    return pImp->bReadOnlyUI || IsReadOnlyMedium();
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsInModalMode() const
{
    return pImp->bModalMode || pImp->bRunningMacro;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::HasModalViews() const
{
    SfxViewFrame* pFrame = SfxViewFrame::GetFirst( this );
    while( pFrame )
    {
        if ( pFrame->IsInModalMode() )
            return sal_True;

        pFrame = SfxViewFrame::GetNext( *pFrame, this );
    }

    return sal_False;
}

//-------------------------------------------------------------------------

void SfxObjectShell::SetMacroMode_Impl( sal_Bool bModal )
{
    if ( !pImp->bRunningMacro != !bModal )
    {
        pImp->bRunningMacro = bModal;
        Broadcast( SfxSimpleHint( SFX_HINT_MODECHANGED ) );
    }
}

//-------------------------------------------------------------------------

void SfxObjectShell::SetModalMode_Impl( sal_Bool bModal )
{
    // nur Broadcasten wenn modifiziert, sonst ggf. Endlosrekursion
    if ( !pImp->bModalMode != !bModal )
    {
        // zentral mitz"ahlen
        sal_uInt16 &rDocModalCount = SFX_APP()->Get_Impl()->nDocModalMode;
        if ( bModal )
            ++rDocModalCount;
        else
            --rDocModalCount;

        // umschalten
        pImp->bModalMode = bModal;
        Broadcast( SfxSimpleHint( SFX_HINT_MODECHANGED ) );
    }
}

//--------------------------------------------------------------------

Size SfxObjectShell::GetFirstPageSize()
{
    return GetVisArea(ASPECT_THUMBNAIL).GetSize();
}


//--------------------------------------------------------------------

IndexBitSet& SfxObjectShell::GetNoSet_Impl()
{
    return pImp->aBitSet;
}

//--------------------------------------------------------------------
// changes the title of the document

void SfxObjectShell::SetTitle
(
    const String& rTitle        // der neue Titel des Dokuments
)

/*  [Beschreibung]

    Mit dieser Methode kann der Titel des Dokuments gesetzt werden.
    Dieser entspricht initial dem kompletten Dateinamen. Ein Setzen
    des Titels wirkt jedoch nicht zu"uck auf den Dateinamen; er wird
    jedoch in den Caption-Bars der MDI-Fenster angezeigt.
*/

{
    DBG_CHKTHIS(SfxObjectShell, 0);

    // nix zu tun?
    if ( ( HasName() && pImp->aTitle == rTitle ) ||
         ( !HasName() && GetTitle() == rTitle ) )
        return;

    SfxApplication *pSfxApp = SFX_APP();
#if 0
    // wird 'unbenannt#' als Titel gesetzt
    String aNoName(SfxResId(STR_NONAME));
    if ( rTitle.Match(aNoName) <= aNoName.Len() )
    {
        // er ist es selbst => ignorieren
        pSfxApp->ReleaseIndex(pImp->nVisualDocumentNumber);
        pImp->bIsNamedVisible=0;
    }
#endif

    // ggf. die unbenannt-Nummer freigeben
    if ( pImp->bIsNamedVisible && USHRT_MAX != pImp->nVisualDocumentNumber )
    {
        pSfxApp->ReleaseIndex(pImp->nVisualDocumentNumber);
        pImp->bIsNamedVisible = 0;
    }

    // Title setzen
    pImp->aTitle = rTitle;
//  Wieso denn in der DocInfo?
//  GetDocInfo().SetTitle( rTitle );
//  FlushDocInfo();

    // Benachrichtigungen
    if ( GetMedium() )
    {
        SfxShell::SetName( GetTitle(SFX_TITLE_APINAME) );
        Broadcast( SfxSimpleHint(SFX_HINT_TITLECHANGED) );
    }
}

//--------------------------------------------------------------------

#if OSL_DEBUG_LEVEL > 1
String X(const String &rRet)
{
    if ( !rRet.Len() )
        return DEFINE_CONST_UNICODE( "-empty-" );
    return rRet;
}
#else
#define X(ret) ret
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
String SfxObjectShell::GetTitle
(
    sal_uInt16  nMaxLength      /*  0 (default)
                                der Titel selbst, so wie er ist

                                1 (==SFX_TITLE_FILENAME)
                                liefert den logischen Dateinamen ohne Pfad
                                (unter WNT je nach Systemeinstellung ohne
                                Extension)

                                2 (==SFX_TITLE_FULLNAME)
                                liefert den mit komplettem logischen Dateinamen
                                mit Pfad (remote => ::com::sun::star::util::URL)

                                3 (==SFX_TITLE_APINAME)
                                liefert den logischen Dateinamen ohne Pfad
                                und Extension

                                4 (==SFX_TITLE_DETECT)
                                liefert den kompletten Titel, falls noch
                                nicht gesetzt wird aber aus DocInfo oder
                                dem Namen des Medium erzeugt

                                5 (==SFX_TITLE_CAPTION)
                                liefert den Titel so, wie MB ihn heute in
                                der CaptionBar anzeigen m"ochte

                                6 (==SFX_TITLE_PICKLIST)
                                liefert den Titel so, wie MB ihn heute in
                                der PickList anzeigen m"ochte

                                7 (==SFX_TITLE_HISTORY)
                                liefert den Titel so, wie MB ihn heute in
                                der History anzeigen m"ochte

                                10 bis USHRT_MAX
                                liefert maximal 'nMaxLength' Zeichen vom logischen
                                Dateinamen inkl. Pfad (remote => ::com::sun::star::util::URL)
                                */
) const

/*  [Beschreibung]

    Liefert den Titel bzw. logischen Dateinamen des Dokuments, je nach
    'nMaxLength'.

    Falls der Dateiname mit Pfad verwendet wird, wird die Namensk"urzung durch
    Ersetzung eines oder mehrerer Directory-Namen durch "..." durchgef"uhrt,
    URLs werden z.Zt. immer komplett geliefert.
*/

{
//    if ( GetCreateMode() == SFX_CREATE_MODE_EMBEDDED )
//        return String();
    SfxMedium *pMed = GetMedium();
    if ( IsLoading() )
        return String();

    if ( !nMaxLength && pImp->pDocInfo )
    {
        String aTitle = pImp->pDocInfo->GetTitle();
        if ( aTitle.Len() )
            return aTitle;
    }

    // Titel erzeugen?
    if ( SFX_TITLE_DETECT == nMaxLength && !pImp->aTitle.Len() )
    {
        static sal_Bool bRecur = sal_False;
        if ( bRecur )
            return DEFINE_CONST_UNICODE( "-not available-" );
        bRecur = sal_True;

        String aTitle;
        SfxObjectShell *pThis = (SfxObjectShell*) this;

        if ( pMed )
        {
            SFX_ITEMSET_ARG( pMed->GetItemSet(), pNameItem, SfxStringItem, SID_DOCINFO_TITLE, sal_False );
            if ( pNameItem )
                aTitle = pNameItem->GetValue();
        }

        if ( !aTitle.Len() )
        {
            // evtl. ist Titel aus DocInfo verwendbar
            //aTitle = pThis->GetDocInfo().GetTitle();
            //aTitle.EraseLeadingChars();
            //aTitle.EraseTrailingChars();

            if ( !aTitle.Len() )
                // sonst wie SFX_TITLE_FILENAME
                aTitle = GetTitle( SFX_TITLE_FILENAME );
        }

        pThis->SetTitle( aTitle );
        bRecur = sal_False;
        return X(aTitle);
    }
    else if (SFX_TITLE_APINAME == nMaxLength )
        return X(GetAPIName());

    // Sonderfall Vorlagen:
    if( IsTemplate() && pImp->aTitle.Len() &&
         ( nMaxLength == SFX_TITLE_CAPTION || nMaxLength == SFX_TITLE_PICKLIST ) )
        return X(pImp->aTitle);

    // Picklist/Caption wird gemappt
    if ( pMed && ( nMaxLength == SFX_TITLE_CAPTION || nMaxLength == SFX_TITLE_PICKLIST ) )
    {
        // Wenn ein spezieller Titel beim "Offnen mitgegebent wurde;
        // wichtig bei URLs, die INET_PROT_FILE verwenden, denn bei denen
        // wird der gesetzte Titel nicht beachtet.
        // (s.u., Auswertung von aTitleMap_Impl)
        SFX_ITEMSET_ARG( pMed->GetItemSet(), pNameItem, SfxStringItem, SID_DOCINFO_TITLE, sal_False );
        if ( pNameItem )
            return X( pNameItem->GetValue() );
    }

    // noch unbenannt?
    DBG_ASSERT( !HasName() || pMed, "HasName() aber kein Medium?!?" );
    if ( !HasName() || !pMed )
    {
        // schon Titel gesezt?
        if ( pImp->aTitle.Len() )
            return X(pImp->aTitle);

        // mu\s es durchnumeriert werden?
        String aNoName( SfxResId( STR_NONAME ) );
        if ( pImp->bIsNamedVisible )
            // Nummer hintenanh"angen
            aNoName += String::CreateFromInt32( pImp->nVisualDocumentNumber );

        // Dokument hei\st vorerst 'unbenannt#'
        return X(aNoName);
    }

    const INetURLObject& aURL = INetURLObject( pMed->GetName() );
    if ( nMaxLength >= SFX_TITLE_CAPTION && nMaxLength <= SFX_TITLE_HISTORY )
    {
        sal_uInt16 nRemote;
        if( !pMed || aURL.GetProtocol() == INET_PROT_FILE )
            nRemote = 0;
        else
            nRemote = 1;
        nMaxLength = aTitleMap_Impl[nMaxLength-SFX_TITLE_CAPTION][nRemote];
    }

    // lokale Datei?
    if ( aURL.GetProtocol() == INET_PROT_FILE )
    {
        String aName( aURL.HasMark() ? INetURLObject( aURL.GetURLNoMark() ).PathToFileName() : aURL.PathToFileName() );

//      if ( nMaxLength > SFX_TITLE_MAXLEN )
//          return X( DirEntry( aName ).GetFull( FSYS_STYLE_HOST, sal_False, nMaxLength ) );
//      else
        if ( nMaxLength == SFX_TITLE_FULLNAME )
            return X( aName );

        if ( !pImp->aTitle.Len() )
        {
            if ( nMaxLength == SFX_TITLE_FILENAME )
                return X( aURL.getName( INetURLObject::LAST_SEGMENT,
                                        true, INetURLObject::DECODE_WITH_CHARSET ) );

            // sonst Titel aus Dateiname generieren
            pImp->aTitle = aURL.getBase( INetURLObject::LAST_SEGMENT,
                                         true, INetURLObject::DECODE_WITH_CHARSET );
        }
    }
    else
    {
        // ::com::sun::star::util::URL-Versionen
        if ( nMaxLength >= SFX_TITLE_MAXLEN )
        {
            String aComplete( pMed->GetName() );
            if( aComplete.Len() > nMaxLength )
            {
                String aRet( DEFINE_CONST_UNICODE( "..." ) );
                aRet += aComplete.Copy( aComplete.Len() - nMaxLength + 3, nMaxLength - 3 );
                return X( aRet );
            }
            else
                return X( pMed->GetName() );
        }
        else if ( nMaxLength == SFX_TITLE_FILENAME )
        {
            //String aName( aURL.GetLastName() );
            String aName( aURL.GetBase() );
            aName = INetURLObject::decode( aName, INET_HEX_ESCAPE, INetURLObject::DECODE_WITH_CHARSET );
            if( !aName.Len() )
                aName = aURL.GetURLNoPass();
            return X(aName);
        }
        else if ( nMaxLength == SFX_TITLE_FULLNAME )
            return X(aURL.GetMainURL( INetURLObject::DECODE_TO_IURI ));

        // ggf. Titel aus Dateiname generieren
        if ( !pImp->aTitle.Len() )
            pImp->aTitle = aURL.GetBase();

        // workaround for the case when the name can not be retrieved from URL by INetURLObject
        if ( !pImp->aTitle.Len() )
            pImp->aTitle = aURL.GetMainURL( INetURLObject::DECODE_WITH_CHARSET );
    }

    // ganzer Titel
    return X(pImp->aTitle);
}

//--------------------------------------------------------------------

void SfxObjectShell::InvalidateName()

/*  [Beschreibung]

    Ermittelt den Titel des Dokuments neu aus 'unbenannt', DocInfo-Titel
    bzw. Dateinamen. Wird nach Laden aus Template oder SaveAs ben"otigt.
*/

{
    // Title neu erzeugen
    pImp->aTitle.Erase();
//  pImp->nVisualDocumentNumber = USHRT_MAX;
    GetTitle( SFX_TITLE_DETECT );
    SetName( GetTitle( SFX_TITLE_APINAME ) );

    // Benachrichtigungen
    Broadcast( SfxSimpleHint(SFX_HINT_TITLECHANGED) );
}

//--------------------------------------------------------------------

void SfxObjectShell::SetNamedVisibility_Impl()
{
    if ( !pImp->bIsNamedVisible )
    {
        // Nummer verpassen
        pImp->bIsNamedVisible = sal_True;
        // ggf. neue Nummer verpassen
        if ( !HasName() && USHRT_MAX == pImp->nVisualDocumentNumber && !pImp->aTitle.Len() )
        {
            pImp->nVisualDocumentNumber = SFX_APP()->GetFreeIndex();
            Broadcast( SfxSimpleHint(SFX_HINT_TITLECHANGED) );
        }
    }

    SetName( GetTitle(SFX_TITLE_APINAME) );
}

void SfxObjectShell::SetNoName()
{
    bHasName = 0;
    bIsTmp = sal_True;
    GetModel()->attachResource( ::rtl::OUString(), GetModel()->getArgs() );
}

//--------------------------------------------------------------------

void SfxObjectShell::MemoryError()
{
}

//--------------------------------------------------------------------

SfxProgress* SfxObjectShell::GetProgress() const
{
    return pImp->pProgress;
}

//--------------------------------------------------------------------

void SfxObjectShell::SetProgress_Impl
(
    SfxProgress *pProgress  /*  zu startender <SfxProgress> oder 0, falls
                                der Progress zur"uckgesetzt werden soll */
)

/*  [Beschreibung]

    Interne Methode zum setzen oder zur"ucksetzen des Progress-Modes
    f"ur diese SfxObjectShell.
*/

{
    DBG_ASSERT( ( !pImp->pProgress && pProgress ) ||
                ( pImp->pProgress && !pProgress ),
                "Progress activation/deacitivation mismatch" );
    pImp->pProgress = pProgress;
}

//--------------------------------------------------------------------

void SfxObjectShell::PostActivateEvent_Impl( SfxViewFrame* pFrame )
{
    SfxApplication* pSfxApp = SFX_APP();
    if ( !pSfxApp->IsDowning() && !IsLoading() && (!pFrame || !pFrame->GetFrame()->IsClosing_Impl() ) )
    {
        SFX_ITEMSET_ARG( pMedium->GetItemSet(), pHiddenItem, SfxBoolItem, SID_HIDDEN, sal_False );
        if ( !pHiddenItem || !pHiddenItem->GetValue() )
        {
            sal_uInt16 nId = pImp->nEventId;
            pImp->nEventId = 0;
            if ( nId )
                pSfxApp->NotifyEvent(SfxEventHint( nId, this ), sal_False);
        }
    }
}

//--------------------------------------------------------------------

void SfxObjectShell::SetActivateEvent_Impl(sal_uInt16 nId )
{
    if ( GetFactory().GetFlags() & SFXOBJECTSHELL_HASOPENDOC )
        pImp->nEventId = nId;
}

//--------------------------------------------------------------------

void SfxObjectShell::RegisterTransfer( SfxMedium& rMedium )
/*  [Beschreibung ]
    Alle Medien, die aufgesetzt werden, um Teile eines Dokumentes zu
    laden, muessen an der zugehoerigen SfxObjectShell angemeldet
    werden. So kann dokumentweise abgebrochen werden.  */
{
    rMedium.SetCancelManager_Impl( GetMedium()->GetCancelManager_Impl() );
    rMedium.SetReferer( GetMedium()->GetName() );
}

//-------------------------------------------------------------------------

void SfxObjectShell::PrepareReload( )
/*  [Beschreibung ]
    Wird vor dem Reload gerufen und gibt die Moeglichkeit,
    etwaige Caches zu leeren. */
{
}

//-------------------------------------------------------------------------

void SfxObjectShell::LockAutoLoad( sal_Bool bLock )

/*  Verhindert ein evtl. eintreffendes AutoLoad. Wird auch vor AutoLoad
    eines umgebenden FrameSet beruecksichtigt.
*/

{
    if ( bLock )
        ++pImp->nAutoLoadLocks;
    else
        --pImp->nAutoLoadLocks;
}

//-------------------------------------------------------------------------

// kann nach frame.cxx gemoved werden, wenn 358+36x-Stand gemerged sind

sal_Bool SfxFrame::IsAutoLoadLocked_Impl() const
{
    // sein einges Doc gelockt?
    const SfxObjectShell* pObjSh = GetCurrentDocument();
    if ( !pObjSh || !pObjSh->IsAutoLoadLocked() )
        return sal_False;

    // seine Childs gelockt?
    for ( sal_uInt16 n = GetChildFrameCount(); n--; )
        if ( !GetChildFrame(n)->IsAutoLoadLocked_Impl() )
            return sal_False;

    // sonst ist AutoLoad erlaubt
    return sal_True;
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsAutoLoadLocked() const

/*  Liefert, ob ein eintreffendes AutoLoad ausgefuehrt werden darf. Wird auch
    vor AutoLoad eines umgebenden FrameSet beruecksichtigt.
*/

{
    return !IsReadOnly() || pImp->nAutoLoadLocks > 0;
}

//-------------------------------------------------------------------------
void SfxObjectShell::BreakMacroSign_Impl( sal_Bool bBreakMacroSign )
{
    pImp->m_bMacroSignBroken = bBreakMacroSign;
}

//-------------------------------------------------------------------------
void SfxObjectShell::CheckMacrosOnLoading_Impl()
{
    const SfxFilter* pFilter = pMedium->GetFilter();
    sal_Bool bHasStorage = IsPackageStorageFormat_Impl( *pMedium );

    if ( GetError() != ERRCODE_NONE )
        return;

    if ( SvtSecurityOptions().IsMacroDisabled() )
    {
        // no macro should be executed at all
        pImp->bMacroDisabled = sal_True;
        pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
    }
    else
    {
        sal_Bool bHasMacros = sal_False;

        if ( bHasStorage && ( !pFilter || !( pFilter->GetFilterFlags() & SFX_FILTER_STARONEFILTER ) ) )
        {
            uno::Reference< embed::XStorage > xStorage = pMedium->GetStorage();
            if ( xStorage.is() )
                bHasMacros = StorageHasMacros( xStorage );
            else
                SetError( ERRCODE_IO_GENERAL );
        }

        if ( !bHasMacros )
            bHasMacros = HasMacrosLib_Impl();

        if ( bHasMacros )
        {
            AdjustMacroMode( String() ); // if macros are disabled the message will be shown here
            if ( SvtSecurityOptions().GetMacroSecurityLevel() > 2
                && MacroExecMode::NEVER_EXECUTE == pImp->nMacroMode && pMedium )
            {
                  UseInteractionToHandleError( pMedium->GetInteractionHandler(), ERRCODE_SFX_DOCUMENT_MACRO_DISABLED );
            }
        }
        else if ( !pImp->bMacroDisabled )
        {
            // if macros will be added by the user later, the security check is obsolete
            pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE_NO_WARN;
        }
    }

    // MAV: the code below is moved here since this is the only central place where the object shell is visible
    //      in case of pick list for example OpenDocExec_Impl() is not used.

    // xmlsec05, check with SFX team
    // Check if there is a broken signature...
    // After EA change to interaction handler...
    if ( !pImp->bSignatureErrorIsShown
    && GetDocumentSignatureState() == SIGNATURESTATE_SIGNATURES_BROKEN )
    {
        if ( pMedium
          && UseInteractionToHandleError( pMedium->GetInteractionHandler(), ERRCODE_SFX_BROKENSIGNATURE ) )
            pImp->bSignatureErrorIsShown = sal_True;

        pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
    }
}

//-------------------------------------------------------------------------
void SfxObjectShell::SetAutoLoad(
    const INetURLObject& rUrl, sal_uInt32 nTime, sal_Bool bReload )
/*  [Beschreibung ]
    Hiermit wird automatisches Laden der Url rUrl nTime
    Millisekunden nach Aufruf von FinishedLoading angefordert. bReload
    bestimmt, ob das Dokument aus dem Cache geladen werden soll oder
    nicht.  */
{
    if ( pImp->pReloadTimer )
        DELETEZ(pImp->pReloadTimer);
    if ( bReload )
    {
        pImp->pReloadTimer = new AutoReloadTimer_Impl(
                                rUrl.GetMainURL( INetURLObject::DECODE_TO_IURI ),
                                nTime, bReload, this );
        pImp->pReloadTimer->Start();
    }
}

sal_Bool SfxObjectShell::IsLoadingFinished() const
{
    return ( pImp->nLoadedFlags == SFX_LOADED_ALL );
}

void impl_addToModelCollection(const css::uno::Reference< css::frame::XModel >& xModel);
void SfxObjectShell::InitOwnModel_Impl()
{
    if ( !pImp->bModelInitialized )
    {
        SFX_ITEMSET_ARG( pMedium->GetItemSet(), pSalvageItem, SfxStringItem, SID_DOC_SALVAGE, sal_False);
        if ( pSalvageItem )
        {
            pImp->aTempName = pMedium->GetPhysicalName();
            pMedium->GetItemSet()->ClearItem( SID_DOC_SALVAGE );
            pMedium->GetItemSet()->ClearItem( SID_FILE_NAME );
            pMedium->GetItemSet()->Put( SfxStringItem( SID_FILE_NAME, pMedium->GetOrigURL() ) );
        }
        else
        {
            pMedium->GetItemSet()->ClearItem( SID_PROGRESS_STATUSBAR_CONTROL );
            pMedium->GetItemSet()->ClearItem( SID_DOCUMENT );
        }

        pMedium->GetItemSet()->ClearItem( SID_REFERER );
        uno::Reference< frame::XModel >  xModel ( GetModel(), uno::UNO_QUERY );
        if ( xModel.is() )
        {
            ::rtl::OUString aURL = GetMedium()->GetOrigURL();
            SfxItemSet *pSet = GetMedium()->GetItemSet();
            if ( !GetMedium()->IsReadOnly() )
                pSet->ClearItem( SID_INPUTSTREAM );
            uno::Sequence< beans::PropertyValue > aArgs;
            TransformItems( SID_OPENDOC, *pSet, aArgs );
            xModel->attachResource( aURL, aArgs );
            impl_addToModelCollection(xModel);
        }

        pImp->bModelInitialized = sal_True;
    }
}

void SfxObjectShell::FinishedLoading( sal_uInt16 nFlags )
{
    sal_Bool bSetModifiedTRUE = sal_False;
    SFX_ITEMSET_ARG( pMedium->GetItemSet(), pSalvageItem, SfxStringItem, SID_DOC_SALVAGE, sal_False );
    if( ( nFlags & SFX_LOADED_MAINDOCUMENT ) && !(pImp->nLoadedFlags & SFX_LOADED_MAINDOCUMENT )
        && !(pImp->nFlagsInProgress & SFX_LOADED_MAINDOCUMENT ))
    {
        pImp->nFlagsInProgress |= SFX_LOADED_MAINDOCUMENT;
        ((SfxHeaderAttributes_Impl*)GetHeaderAttributes())->SetAttributes();
        pImp->bImportDone = sal_True;
        if( !IsAbortingImport() )
            PositionView_Impl();

        // Salvage
        if ( pSalvageItem )
            bSetModifiedTRUE = sal_True;

        if ( !IsEnableSetModified() )
            EnableSetModified( sal_True );

        if( !bSetModifiedTRUE && IsEnableSetModified() )
            SetModified( sal_False );

        CheckMacrosOnLoading_Impl();

        bHasName = sal_True; // the document is loaded, so the name should already available
        GetTitle( SFX_TITLE_DETECT );
        InitOwnModel_Impl();
        pImp->nFlagsInProgress &= ~SFX_LOADED_MAINDOCUMENT;
    }

    if( ( nFlags & SFX_LOADED_IMAGES ) && !(pImp->nLoadedFlags & SFX_LOADED_IMAGES )
        && !(pImp->nFlagsInProgress & SFX_LOADED_IMAGES ))
    {
        pImp->nFlagsInProgress |= SFX_LOADED_IMAGES;
        SfxDocumentInfo& rInfo = GetDocInfo();
        SetAutoLoad( INetURLObject(rInfo.GetReloadURL()),
            rInfo.GetReloadDelay() * 1000, rInfo.IsReloadEnabled() );
        if( !bSetModifiedTRUE && IsEnableSetModified() )
            SetModified( sal_False );
        Invalidate( SID_SAVEASDOC );
        pImp->nFlagsInProgress &= ~SFX_LOADED_IMAGES;
    }

    pImp->nLoadedFlags |= nFlags;

    if ( !pImp->nFlagsInProgress )
    {
        // in case of reentrance calls the first called FinishedLoading() call on the stack
        // should do the notification, in result the notification is done when all the FinishedLoading() calls are finished

        if ( bSetModifiedTRUE )
            SetModified( sal_True );
        else
            SetModified( sal_False );

        if ( (pImp->nLoadedFlags & SFX_LOADED_MAINDOCUMENT ) && (pImp->nLoadedFlags & SFX_LOADED_IMAGES ) )
        {
            // closing the streams on loading should be under control of SFX!
            // if a readonly medium has storage then it's stream is already based on temporary file
            DBG_ASSERT( pMedium->IsOpen(), "Don't close the medium when loading documents!" );
            if( !(pMedium->GetOpenMode() & STREAM_WRITE) && !pMedium->HasStorage_Impl() )
                // don't lock file opened read only
                pMedium->CloseInStream();
        }

        pImp->bInitialized = sal_True;
        SFX_APP()->NotifyEvent( SfxEventHint( SFX_EVENT_LOADFINISHED, this ) );

        // Title is not available until loading has finished
        Broadcast( SfxSimpleHint( SFX_HINT_TITLECHANGED ) );
        if ( pImp->nEventId )
            PostActivateEvent_Impl(SfxViewFrame::GetFirst(this));
    }
}

//-------------------------------------------------------------------------

void SfxObjectShell::PositionView_Impl()
{
    MarkData_Impl *pMark = Get_Impl()->pMarkData;
    if( pMark )
    {
        SfxViewShell* pSh = pMark->pFrame->GetViewShell();
        if( pMark->aUserData.Len() )
            pSh->ReadUserData( pMark->aUserData, sal_True );
        else if( pMark->aMark.Len() )
            pSh->JumpToMark( pMark->aMark );
        DELETEZ( Get_Impl()->pMarkData );
    }
}

//-------------------------------------------------------------------------

sal_Bool SfxObjectShell::IsLoading() const
/*  [Beschreibung ]
    Wurde bereits FinishedLoading aufgerufeb? */
{
    return !( pImp->nLoadedFlags & SFX_LOADED_MAINDOCUMENT );
}

//-------------------------------------------------------------------------

void SfxObjectShell::CancelTransfers()
/*  [Beschreibung ]
    Hier koennen Transfers gecanceled werden, die nicht mit
    RegisterTransfer registiert wurden */
{
    GetMedium()->CancelTransfers();
    if( ( pImp->nLoadedFlags & SFX_LOADED_ALL ) != SFX_LOADED_ALL )
    {
        AbortImport();
        if( IsLoading() )
            FinishedLoading( SFX_LOADED_ALL );

/*
        SfxViewFrame* pFrame = SfxViewFrame::GetFirst( this );
        while( pFrame )
        {
            pFrame->CancelTransfers();
            pFrame = SfxViewFrame::GetNext( *pFrame, this );
        }*/
    }
}

//-------------------------------------------------------------------------

AutoReloadTimer_Impl::AutoReloadTimer_Impl(
    const String& rURL, sal_uInt32 nTime, sal_Bool bReloadP, SfxObjectShell* pSh )
    : aUrl( rURL ), bReload( bReloadP ), pObjSh( pSh )
{
    SetTimeout( nTime );
}

//-------------------------------------------------------------------------

void AutoReloadTimer_Impl::Timeout()
{
    SfxViewFrame *pFrame = SfxViewFrame::GetFirst( pObjSh );

    if ( pFrame )
    {
        // momentan nicht m"oglich/sinnvoll?
        if ( !pObjSh->CanReload_Impl() || pObjSh->IsAutoLoadLocked() || Application::IsUICaptured() )
        {
            // erneuten Versuch erlauben
            Start();
            return;
        }

        SfxAllItemSet aSet( SFX_APP()->GetPool() );
        aSet.Put( SfxBoolItem( SID_AUTOLOAD, sal_True ) );
        if ( aUrl.Len() )
            aSet.Put(  SfxStringItem( SID_FILE_NAME, aUrl ) );
        SfxRequest aReq( SID_RELOAD, 0, aSet );
        pObjSh->Get_Impl()->pReloadTimer = 0;
        delete this;
        pFrame->ExecReload_Impl( aReq );
        return;
    }

    pObjSh->Get_Impl()->pReloadTimer = 0;
    delete this;
}

SfxModule* SfxObjectShell::GetModule() const
{
    return GetFactory().GetModule();
}

sal_Bool SfxObjectShell::IsBasic(
    const String & rCode, SbxObject * pVCtrl )
{
    if( !rCode.Len() ) return sal_False;
    if( !pImp->bIsBasicDefault )
        return sal_False;
    return SfxMacroConfig::IsBasic( pVCtrl, rCode, GetBasicManager() );
}

ErrCode SfxObjectShell::CallBasic( const String& rMacro,
    const String& rBasic, SbxObject* pVCtrl, SbxArray* pArgs,
    SbxValue* pRet )
{
    SfxApplication* pApp = SFX_APP();
    if( pApp->GetName() != rBasic )
    {
        AdjustMacroMode( String() );
        if( pImp->nMacroMode == MacroExecMode::NEVER_EXECUTE )
            return ERRCODE_IO_ACCESSDENIED;
    }

    pApp->EnterBasicCall();
    BasicManager *pMgr = GetBasicManager();
    if( pApp->GetName() == rBasic )
        pMgr = pApp->GetBasicManager();
    ErrCode nRet = SfxMacroConfig::Call( pVCtrl, rMacro, pMgr, pArgs, pRet );
    pApp->LeaveBasicCall();
    return nRet;
}

ErrCode SfxObjectShell::Call( const String & rCode, sal_Bool bIsBasicReturn, SbxObject * pVCtrl )
{
    ErrCode nErr = ERRCODE_NONE;
    if ( bIsBasicReturn )
        CallBasic( rCode, String(), pVCtrl );
    return nErr;
}

// perhaps rename to CallScript once we get rid of the existing CallScript
// and Call, CallBasic, CallStarBasic methods
ErrCode SfxObjectShell::CallXScript( const String& rScriptURL,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >&
            aParams,
        ::com::sun::star::uno::Any& aRet,
        ::com::sun::star::uno::Sequence< sal_Int16 >& aOutParamIndex,
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >&
            aOutParam)
{
    OSL_TRACE( "in CallXScript" );
    ErrCode nErr = ERRCODE_NONE;

    bool bCaughtException = FALSE;
    ::com::sun::star::uno::Any aException;

    // security check if it's not an application script?
    // or if it's a document script??
    if( rScriptURL.Search( UniString::CreateFromAscii( "location=document" ) )
            > 0 )
    {
        AdjustMacroMode( String() );
        if( pImp->nMacroMode == MacroExecMode::NEVER_EXECUTE ) {
            return ERRCODE_IO_ACCESSDENIED;
        }
    }

    try
    {
        Reference< provider::XScriptProviderSupplier > xSPS =
            Reference< provider::XScriptProviderSupplier >
                ( GetModel(), UNO_QUERY_THROW );

        Reference< provider::XScriptProvider > xScriptProvider =
            xSPS->getScriptProvider();

        if( !xScriptProvider.is() )
        {
            OSL_TRACE( "CallXScript: no ScriptProvider" );
            throw RuntimeException(::rtl::OUString(), Reference< XInterface >());
        }

        ::rtl::OUString oScriptURL( rScriptURL.GetBuffer() );
        Reference< provider::XScript > xScript =
            xScriptProvider->getScript( oScriptURL );

        if( !xScript.is() )
        {
            OSL_TRACE( "CallXScript: no Script" );
            throw RuntimeException(::rtl::OUString(), Reference< XInterface >());
        }
        OSL_TRACE( "CallXScript, got Script, about to invoke");
        OSL_TRACE( "CallXScript, number of params is: %d", aParams.getLength() );
        aRet = xScript->invoke( aParams, aOutParamIndex, aOutParam );
        OSL_TRACE( "CallXScript, invoke is finished");
    }
    // Use the errors from basic for the time being
    catch ( ::com::sun::star::uno::RuntimeException& rte )
    {
        OSL_TRACE( "CallXScript: exception rte" );

        aException = makeAny( rte );
        bCaughtException = TRUE;
        nErr = ERRCODE_BASIC_INTERNAL_ERROR;
    }
    catch ( provider::ScriptFrameworkErrorException& ite )
    {
        OSL_TRACE( "CallXScript: exception ite" );

        aException = makeAny( ite );
        bCaughtException = TRUE;
        nErr = ERRCODE_BASIC_INTERNAL_ERROR;
    }
    catch ( ::com::sun::star::reflection::InvocationTargetException& ite )
    {
        OSL_TRACE( "CallXScript: exception ite" );

        aException = makeAny( ite );
        bCaughtException = TRUE;
        nErr = ERRCODE_BASIC_INTERNAL_ERROR;
    }

    if ( bCaughtException )
    {
        SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();

        if ( pFact != NULL )
        {
            VclAbstractDialog* pDlg =
                pFact->CreateScriptErrorDialog( GetDialogParent(), aException );

            if ( pDlg != NULL )
            {
                pDlg->Execute();
                delete pDlg;
            }
        }
    }

    OSL_TRACE( "leaving CallXScript" );
    return nErr;
}

extern ::com::sun::star::uno::Any sbxToUnoValue( SbxVariable* pVar );

//-------------------------------------------------------------------------
namespace {
    using namespace ::com::sun::star::uno;

    //.....................................................................
    static SbxArrayRef lcl_translateUno2Basic( const void* _pAnySequence )
    {
        SbxArrayRef xReturn;
        if ( _pAnySequence )
        {
            // in real it's a sequence of Any (by convention)
            const Sequence< Any >* pArguments = static_cast< const Sequence< Any >* >( _pAnySequence );

            // do we have arguments ?
            if ( pArguments->getLength() )
            {
                // yep
                xReturn = new SbxArray;
                String sEmptyName;

                // loop through the sequence
                const Any* pArg     =           pArguments->getConstArray();
                const Any* pArgEnd  = pArg  +   pArguments->getLength();

                for ( sal_uInt16 nArgPos=1; pArg != pArgEnd; ++pArg, ++nArgPos )
                    // and create a Sb object for every Any
                    xReturn->Put( GetSbUnoObject( sEmptyName, *pArg ), nArgPos );
            }
        }
        return xReturn;
    }
    //.....................................................................
    void lcl_translateBasic2Uno( const SbxVariableRef& _rBasicValue, void* _pAny )
    {
        if ( _pAny )
            *static_cast< Any* >( _pAny ) = sbxToUnoValue( _rBasicValue );
    }
}
//-------------------------------------------------------------------------
ErrCode SfxObjectShell::CallStarBasicScript( const String& _rMacroName, const String& _rLocation,
    const void* _pArguments, void* _pReturn )
{
    OSL_TRACE("in CallSBS");
    ::vos::OClearableGuard aGuard( Application::GetSolarMutex() );

    // the arguments for the call
    SbxArrayRef xMacroArguments = lcl_translateUno2Basic( _pArguments );

    // the return value
    SbxVariableRef xReturn = _pReturn ? new SbxVariable : NULL;

    // the location (document or application)
    String sMacroLocation;
    if ( _rLocation.EqualsAscii( "application" ) )
        sMacroLocation = SFX_APP()->GetName();
#ifdef DBG_UTIL
    else
        DBG_ASSERT( _rLocation.EqualsAscii( "document" ),
            "SfxObjectShell::CallStarBasicScript: invalid (unknown) location!" );
#endif

    // call the script
    ErrCode eError = CallBasic( _rMacroName, sMacroLocation, NULL, xMacroArguments, xReturn );

    // translate the return value
    lcl_translateBasic2Uno( xReturn, _pReturn );

    // outta here
    return eError;
}

//-------------------------------------------------------------------------
ErrCode SfxObjectShell::CallScript(
    const String & rScriptType,
    const String & rCode,
    const void *pArgs,
    void *pRet
)
{
    ::vos::OClearableGuard aGuard( Application::GetSolarMutex() );
    ErrCode nErr = ERRCODE_NONE;
    if( rScriptType.EqualsAscii( "StarBasic" ) )
    {
        // the arguments for the call
        SbxArrayRef xMacroArguments = lcl_translateUno2Basic( pArgs );

        // the return value
        SbxVariableRef xReturn = pRet ? new SbxVariable : NULL;

        // call the script
        nErr = CallBasic( rCode, String(), NULL, xMacroArguments, xReturn );

        // translate the return value
        lcl_translateBasic2Uno( xReturn, pRet );

        // did this fail because the method was not found?
        if ( nErr == ERRCODE_BASIC_PROC_UNDEFINED )
        {   // yep-> look in the application BASIC module
            nErr = CallBasic( rCode, SFX_APP()->GetName(), NULL, xMacroArguments, xReturn );
        }
    }
    else if( rScriptType.EqualsAscii( "JavaScript" ) )
    {
        DBG_ERROR( "JavaScript not allowed" );
        return 0;
    }
    else
    {
        DBG_ERROR( "StarScript not allowed" );
    }
    return nErr;
}

SfxFrame* SfxObjectShell::GetSmartSelf( SfxFrame* pSelf, SfxMedium& /*rMedium*/ )
{
    return pSelf;
}

SfxObjectShellFlags SfxObjectShell::GetFlags() const
{
    if( pImp->eFlags == SFXOBJECTSHELL_UNDEFINED )
        pImp->eFlags = GetFactory().GetFlags();
    return pImp->eFlags;
}

void SfxObjectShell::SetFlags( SfxObjectShellFlags eFlags )
{
    pImp->eFlags = eFlags;
}

/*
void SfxObjectShell::SetBaseURL( const String& rURL )
{
    pImp->aBaseURL = rURL;
    pImp->bNoBaseURL = FALSE;
}

const String& SfxObjectShell::GetBaseURLForSaving() const
{
    if ( pImp->bNoBaseURL )
        return String();
    return GetBaseURL();
}

const String& SfxObjectShell::GetBaseURL() const
{
    if ( pImp->aBaseURL.Len() )
        return pImp->aBaseURL;
    return pMedium->GetBaseURL();
}

void SfxObjectShell::SetEmptyBaseURL()
{
    pImp->bNoBaseURL = TRUE;
}
*/
String SfxObjectShell::QueryTitle( SfxTitleQuery eType ) const
{
    String aRet;

    switch( eType )
    {
        case SFX_TITLE_QUERY_SAVE_NAME_PROPOSAL:
        {
            SfxMedium* pMed = GetMedium();
            const INetURLObject aObj( pMed->GetName() );
            aRet = aObj.GetMainURL( INetURLObject::DECODE_TO_IURI );
            if ( !aRet.Len() )
                aRet = GetTitle( SFX_TITLE_CAPTION );
            break;
        }
    }
    return aRet;
}

void SfxHeaderAttributes_Impl::SetAttributes()
{
    bAlert = sal_True;
    SvKeyValue aPair;
    for( sal_Bool bCont = xIter->GetFirst( aPair ); bCont;
         bCont = xIter->GetNext( aPair ) )
        SetAttribute( aPair );
}

void SfxHeaderAttributes_Impl::SetAttribute( const SvKeyValue& rKV )
{
    String aValue = rKV.GetValue();
    if( rKV.GetKey().CompareIgnoreCaseToAscii( "refresh" ) == COMPARE_EQUAL && rKV.GetValue().Len() )
    {
        sal_uInt32 nTime = aValue.GetToken(  0, ';' ).ToInt32() ;
        String aURL = aValue.GetToken( 1, ';' );
        aURL.EraseTrailingChars().EraseLeadingChars();
        SfxDocumentInfo& rInfo = pDoc->GetDocInfo();
        if( aURL.Copy(0, 4).CompareIgnoreCaseToAscii( "url=" ) == COMPARE_EQUAL )
        {
            INetURLObject aObj;
            INetURLObject( pDoc->GetMedium()->GetName() ).GetNewAbsURL( aURL.Copy( 4 ), &aObj );
            rInfo.SetReloadURL( aObj.GetMainURL( INetURLObject::NO_DECODE ) );
        }
        rInfo.EnableReload( sal_True );
        rInfo.SetReloadDelay( nTime );
        pDoc->FlushDocInfo();
    }
    else if( rKV.GetKey().CompareIgnoreCaseToAscii( "expires" ) == COMPARE_EQUAL )
    {
        DateTime aDateTime;
        if( INetRFC822Message::ParseDateField( rKV.GetValue(), aDateTime ) )
        {
            aDateTime.ConvertToLocalTime();
            pDoc->GetMedium()->SetExpired_Impl( aDateTime );
        }
        else
        {
//          DBG_ERROR( "Schlechtes ::com::sun::star::util::DateTime fuer Expired" );
            pDoc->GetMedium()->SetExpired_Impl( Date( 1, 1, 1970 ) );
        }
    }
    else if( rKV.GetKey().CompareIgnoreCaseToAscii( "content-type" ) == COMPARE_EQUAL )
    {
        ::rtl::OString sContent = ::rtl::OUStringToOString( aValue, RTL_TEXTENCODING_ASCII_US );
        ByteString sType, sSubType;
        INetContentTypeParameterList aParameters;

        if( INetContentTypes::parse( sContent, sType, sSubType, &aParameters ) )
        {
            const INetContentTypeParameter * pCharset = aParameters.find("charset");
            if (pCharset != 0)
                pDoc->GetMedium()->SetCharset( pCharset->m_sValue );
        }
    }
}

void SfxHeaderAttributes_Impl::Append( const SvKeyValue& rKV )
{
    xIter->Append( rKV );
    if( bAlert ) SetAttribute( rKV );
}

SvKeyValueIterator* SfxObjectShell::GetHeaderAttributes()
{
    if( !pImp->xHeaderAttributes.Is() )
    {
        DBG_ASSERT( pMedium, "Kein Medium" );
        pImp->xHeaderAttributes = new SfxHeaderAttributes_Impl( this );
    }
    return ( SvKeyValueIterator*) &pImp->xHeaderAttributes;
}

void SfxObjectShell::ClearHeaderAttributesForSourceViewHack()
{
    ((SfxHeaderAttributes_Impl*)GetHeaderAttributes())
        ->ClearForSourceView();
}


void SfxObjectShell::SetHeaderAttributesForSourceViewHack()
{
    ((SfxHeaderAttributes_Impl*)GetHeaderAttributes())
        ->SetAttributes();
}

void SfxObjectShell::StartLoading_Impl()
{
    pImp->nLoadedFlags = 0;
    pImp->bModelInitialized = sal_False;
}

sal_Bool SfxObjectShell::IsPreview() const
{
    if ( !pMedium )
        return sal_False;

    sal_Bool bPreview = sal_False;
    SFX_ITEMSET_ARG( pMedium->GetItemSet(), pFlags, SfxStringItem, SID_OPTIONS, sal_False);
    if ( pFlags )
    {
        // Werte auf einzelne Items verteilen
        String aFileFlags = pFlags->GetValue();
        aFileFlags.ToUpperAscii();
        if ( STRING_NOTFOUND != aFileFlags.Search( 'B' ) )
            bPreview = sal_True;
    }

    if ( !bPreview )
    {
        SFX_ITEMSET_ARG( pMedium->GetItemSet(), pItem, SfxBoolItem, SID_PREVIEW, sal_False);
        if ( pItem )
            bPreview = pItem->GetValue();
    }

    return bPreview;
}

sal_Bool SfxObjectShell::IsSecure()
{
    // Wenn globale Warnung an ist, nach Secure-Referer-Liste gehen
    String aReferer = GetMedium()->GetName();
    if ( !aReferer.Len() )
    {
        // bei neuen Dokumenten das Template als Referer nehmen
        String aTempl( GetDocInfo().GetTemplateFileName() );
        if ( aTempl.Len() )
            aReferer = INetURLObject( aTempl ).GetMainURL( INetURLObject::NO_DECODE );
    }

    INetURLObject aURL( "macro:" );
    if ( !aReferer.Len() )
        // empty new or embedded document
        return sal_True;

        SvtSecurityOptions aOpt;

    if( aOpt.GetBasicMode() == eALWAYS_EXECUTE )
        return sal_True;

    if( aOpt.GetBasicMode() == eNEVER_EXECUTE )
        return sal_False;

    if ( aOpt.IsSecureURL( aURL.GetMainURL( INetURLObject::NO_DECODE ), aReferer ) )
    //if ( SvtSecurityOptions().IsSecureURL( aURL.GetMainURL( INetURLObject::NO_DECODE ), aReferer ) )
    {
        if ( GetMedium()->GetContent().is() )
        {
            Any aAny( ::utl::UCBContentHelper::GetProperty( aURL.GetMainURL( INetURLObject::NO_DECODE ), String( RTL_CONSTASCII_USTRINGPARAM("IsProtected")) ) );
            sal_Bool bIsProtected = FALSE;
            if ( ( aAny >>= bIsProtected ) && bIsProtected )
                return sal_False;
            else
                return sal_True;
        }
        else
            return sal_True;
    }
    else
        return sal_False;
}

void SfxObjectShell::SetWaitCursor( BOOL bSet ) const
{
    for( SfxViewFrame* pFrame = SfxViewFrame::GetFirst( this ); pFrame; pFrame = SfxViewFrame::GetNext( *pFrame, this ) )
    {
        if ( bSet )
            pFrame->GetFrame()->GetWindow().EnterWait();
        else
            pFrame->GetFrame()->GetWindow().LeaveWait();
    }
}

String SfxObjectShell::GetAPIName() const
{
    INetURLObject aURL( GetMedium()->GetName() );
    String aName( aURL.GetBase() );
    if( !aName.Len() )
        aName = aURL.GetURLNoPass();
    if ( !aName.Len() )
        aName = GetTitle( SFX_TITLE_DETECT );
    return aName;
}

void SfxObjectShell::Invalidate( USHORT nId )
{
    for( SfxViewFrame* pFrame = SfxViewFrame::GetFirst( this ); pFrame; pFrame = SfxViewFrame::GetNext( *pFrame, this ) )
        Invalidate_Impl( pFrame->GetBindings(), nId );
}

// nMacroMode == -1 : uninitialized
// other values as in /com/sun/star/document/MacroExecMode.hxx

void SfxObjectShell::AdjustMacroMode( const String& /*rScriptType*/ )
{
    if( pImp->nMacroMode < 0 )
    {
        SFX_ITEMSET_ARG( pMedium->GetItemSet(), pMacroModeItem, SfxUInt16Item, SID_MACROEXECMODE, sal_False);
        pImp->nMacroMode = pMacroModeItem ? pMacroModeItem->GetValue() : MacroExecMode::NEVER_EXECUTE;
    }

    // --> PB 2004-11-09 #i35190#
    // xmlsec05, check with SFX team
    // After EA change to interaction handler...
    if ( !pImp->bSignatureErrorIsShown && GetDocumentSignatureState() == SIGNATURESTATE_SIGNATURES_BROKEN )
    {
        // if the signature is broken, show here the warning before
        // the macro warning
        if ( pMedium
          && UseInteractionToHandleError( pMedium->GetInteractionHandler(), ERRCODE_SFX_BROKENSIGNATURE ) )
            pImp->bSignatureErrorIsShown = sal_True;

        pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
    }
    // <--

    if ( SvtSecurityOptions().IsMacroDisabled() )
    {
        // no macro should be executed at all
        pImp->bMacroDisabled = sal_True;
        pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;

        if ( !pImp->bMacroDisabledMessageIsShown && pMedium
          && UseInteractionToHandleError( pMedium->GetInteractionHandler(), ERRCODE_SFX_MACROS_SUPPORT_DISABLED ) )
        {
            pImp->bMacroDisabledMessageIsShown = sal_True;
        }

        return;
    }

    // get setting from configuration if required
    sal_Int16 nAutoConformation = 0;
    if ( pImp->nMacroMode == MacroExecMode::USE_CONFIG
      || pImp->nMacroMode == MacroExecMode::USE_CONFIG_REJECT_CONFIRMATION
      || pImp->nMacroMode == MacroExecMode::USE_CONFIG_APPROVE_CONFIRMATION )
    {
           SvtSecurityOptions aOpt;
        switch( aOpt.GetMacroSecurityLevel() )
        {
            case 3:
                pImp->nMacroMode = MacroExecMode::FROM_LIST_NO_WARN;
                break;
            case 2:
                pImp->nMacroMode = MacroExecMode::FROM_LIST_AND_SIGNED_WARN;
                break;
            case 1:
                pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE;
                break;
            case 0:
                pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE_NO_WARN;
                break;
            default:
                OSL_ENSURE( sal_False, "Unexpected macro security level!\n" );
                pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
        }

        if ( pImp->nMacroMode == MacroExecMode::USE_CONFIG_REJECT_CONFIRMATION )
            nAutoConformation = -1;
        else if ( pImp->nMacroMode == MacroExecMode::USE_CONFIG_APPROVE_CONFIRMATION )
            nAutoConformation = 1;
    }

    if ( pImp->nMacroMode == MacroExecMode::NEVER_EXECUTE
      || pImp->nMacroMode == MacroExecMode::ALWAYS_EXECUTE_NO_WARN )
        return;

    try
    {
        uno::Reference< security::XDocumentDigitalSignatures > xSignatures(
            comphelper::getProcessServiceFactory()->createInstance(
                rtl::OUString( RTL_CONSTASCII_USTRINGPARAM ( "com.sun.star.security.DocumentDigitalSignatures" ) ) ),
            uno::UNO_QUERY );
        String aReferer;

        // get document location from medium name and check whether it is a trusted one
        if ( xSignatures.is() )
        {
            ::rtl::OUString aLocation;
            aReferer = GetMedium()->GetName();
            if ( !aReferer.Len() )
            {
                // for documents made from a template: get the name of the template
                aReferer = GetDocInfo().GetTemplateFileName();
            }
            INetURLObject aURLReferer( aReferer );
            if ( aURLReferer.removeSegment() )
                aLocation = aURLReferer.GetMainURL( INetURLObject::NO_DECODE );

            if ( aLocation.getLength() && xSignatures->isLocationTrusted( aLocation ) )
            {
                pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE_NO_WARN;
                return;
            }
        }

        // at this point it is clear that the document is not in the secure location
        if ( pImp->nMacroMode == MacroExecMode::FROM_LIST_NO_WARN )
        {
            pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
            return;
        }

        // check whether the document is signed with trusted certificate
        if ( xSignatures.is() && pImp->nMacroMode != MacroExecMode::FROM_LIST )
        {
            ::com::sun::star::uno::Sequence< security::DocumentSignatureInformation > aScriptingSignatureInformations;
            uno::Reference < embed::XStorage > xStore = GetMedium()->GetLastCommitReadStorage_Impl();
            sal_uInt16 nSignatureState = GetScriptingSignatureState();

            if ( nSignatureState != SIGNATURESTATE_NOSIGNATURES && pImp->m_bMacroSignBroken )
            {
                // if there is a macro signature it must be handled as broken
                nSignatureState = SIGNATURESTATE_SIGNATURES_BROKEN;
            }

            if ( nSignatureState == SIGNATURESTATE_SIGNATURES_BROKEN )
            {
                if ( pImp->nMacroMode != MacroExecMode::FROM_LIST_AND_SIGNED_NO_WARN )
                {
                    if ( pMedium
                      && UseInteractionToHandleError( pMedium->GetInteractionHandler(), ERRCODE_SFX_BROKENSIGNATURE ) )
                        pImp->bSignatureErrorIsShown = sal_True;

                    pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
                    return;
                }
            }
            else if ( (    nSignatureState == SIGNATURESTATE_SIGNATURES_OK
                        || nSignatureState == SIGNATURESTATE_SIGNATURES_NOTVALIDATED )
                      && xStore.is() )
                aScriptingSignatureInformations =
                    xSignatures->verifyScriptingContentSignatures( xStore, uno::Reference< io::XInputStream >() );

            sal_Int32 nNumOfInfos = aScriptingSignatureInformations.getLength();

            // from now on aReferer is the system file path
            // aReferer = INetURLObject::decode( aReferer, '%', INetURLObject::DECODE_WITH_CHARSET );
            ::rtl::OUString aSystemFileURL;
            if ( osl::FileBase::getSystemPathFromFileURL( aReferer, aSystemFileURL ) == osl::FileBase::E_None )
                aReferer = aSystemFileURL;

            if ( nNumOfInfos )
            {
                for ( sal_Int32 nInd = 0; nInd < nNumOfInfos; nInd++ )
                    if ( xSignatures->isAuthorTrusted( aScriptingSignatureInformations[nInd].Signer ) )
                    {
                        pImp->nMacroMode = MacroExecMode::ALWAYS_EXECUTE_NO_WARN;
                        return;
                    }

                if ( pImp->nMacroMode != MacroExecMode::FROM_LIST_AND_SIGNED_NO_WARN )
                {
                    MacroWarning aDlg( GetDialogParent(), true );
                    aDlg.SetDocumentURL( aReferer );
                    if( nNumOfInfos > 1 )
                        aDlg.SetStorage( xStore, aScriptingSignatureInformations );
                    else
                        aDlg.SetCertificate( aScriptingSignatureInformations[ 0 ].Signer );
                    USHORT nRet = aDlg.Execute();
                    pImp->nMacroMode = ( nRet == RET_OK ) ? MacroExecMode::ALWAYS_EXECUTE_NO_WARN : MacroExecMode::NEVER_EXECUTE;
                    return;
                }
            }
            else if( pImp->nMacroMode == MacroExecMode::USE_CONFIG )
            {
                MacroWarning aWarning( GetDialogParent(), false );
                aWarning.SetDocumentURL( aReferer );
                if( aWarning.Execute() != RET_OK )
                    return;
            }
        }

        // at this point it is clear that the document is neither in secure location nor signed with trusted certificate
        if ( pImp->nMacroMode == MacroExecMode::FROM_LIST_AND_SIGNED_NO_WARN
          || pImp->nMacroMode == MacroExecMode::FROM_LIST_AND_SIGNED_WARN )
        {
            if ( pImp->nMacroMode == MacroExecMode::FROM_LIST_AND_SIGNED_WARN && pMedium )
            {
                  UseInteractionToHandleError( pMedium->GetInteractionHandler(), ERRCODE_SFX_DOCUMENT_MACRO_DISABLED );
            }
               pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
            return;
        }
    }
    catch ( uno::Exception& )
    {
        if ( pImp->nMacroMode == MacroExecMode::FROM_LIST_NO_WARN
          || pImp->nMacroMode == MacroExecMode::FROM_LIST_AND_SIGNED_WARN
          || pImp->nMacroMode == MacroExecMode::FROM_LIST_AND_SIGNED_NO_WARN )
        {
            pImp->nMacroMode = MacroExecMode::NEVER_EXECUTE;
            return;
        }
    }

    // conformation is required
    sal_Bool bSecure = sal_False;
    if ( !nAutoConformation )
    {
        String aReferer = GetMedium()->GetName();
        if ( !aReferer.Len() )
            aReferer = GetDocInfo().GetTemplateFileName();
        ::rtl::OUString aSystemFileURL;
        if ( osl::FileBase::getSystemPathFromFileURL( aReferer, aSystemFileURL ) == osl::FileBase::E_None )
            aReferer = aSystemFileURL;

        MacroWarning aWarning( GetDialogParent(), false );
        aWarning.SetDocumentURL( aReferer );
        bSecure = ( aWarning.Execute() == RET_OK );
    }
    else
        bSecure = ( nAutoConformation > 0 );

    pImp->nMacroMode = bSecure ? MacroExecMode::ALWAYS_EXECUTE_NO_WARN : MacroExecMode::NEVER_EXECUTE;
}

sal_Int16 SfxObjectShell::GetMacroMode()
{
    return pImp->nMacroMode;
}

Window* SfxObjectShell::GetDialogParent( SfxMedium* pLoadingMedium )
{
    Window* pWindow = 0;
    SfxItemSet* pSet = pLoadingMedium ? pLoadingMedium->GetItemSet() : GetMedium()->GetItemSet();
    SFX_ITEMSET_ARG( pSet, pUnoItem, SfxUnoAnyItem, SID_FILLFRAME, FALSE );
    if ( pUnoItem )
    {
        uno::Reference < frame::XFrame > xFrame;
        pUnoItem->GetValue() >>= xFrame;
        if ( xFrame.is() )
            pWindow = VCLUnoHelper::GetWindow( xFrame->getContainerWindow() );
    }

    if ( !pWindow )
    {
        SfxFrame* pFrame = 0;
        SFX_ITEMSET_ARG( pSet, pFrameItem, SfxFrameItem, SID_DOCFRAME, FALSE );
        if( pFrameItem && pFrameItem->GetFrame() )
            // get target frame from ItemSet
            pFrame = pFrameItem->GetFrame();
        else
        {
            // try the current frame
            SfxViewFrame* pView = SfxViewFrame::Current();
            if ( !pView || pView->GetObjectShell() != this )
                // get any visible frame
                pView = SfxViewFrame::GetFirst(this);
            if ( pView )
                pFrame = pView->GetFrame();
        }

        if ( pFrame )
            // get topmost window
            pWindow = VCLUnoHelper::GetWindow( pFrame->GetFrameInterface()->getContainerWindow() );
    }

    if ( pWindow )
    {
        // this frame may be invisible, show it if it is allowed
        SFX_ITEMSET_ARG( pSet, pHiddenItem, SfxBoolItem, SID_HIDDEN, sal_False );
        if ( !pHiddenItem || !pHiddenItem->GetValue() )
        {
            pWindow->Show();
            pWindow->ToTop();
        }
    }

    return pWindow;
}

String SfxObjectShell::UpdateTitle( SfxMedium* pMed, USHORT nDocViewNumber )
{
    // Titel des Fensters
    String aTitle;
    if ( pMed )
    {
        INetURLObject aTmp( pMed->GetName() );
        aTitle = aTmp.getName( INetURLObject::LAST_SEGMENT, true, INetURLObject::DECODE_WITH_CHARSET );
    }
    else
    {
        pMed = GetMedium();
        aTitle = GetTitle(SFX_TITLE_CAPTION);
        String aName(aTitle);
        if ( nDocViewNumber )
        {
            aName += ':';
            aName += String::CreateFromInt32( nDocViewNumber );
        }
    }

    if ( pMed )
    {
        SFX_ITEMSET_ARG( pMed->GetItemSet(), pRepairedDocItem, SfxBoolItem, SID_REPAIRPACKAGE, sal_False );
        if ( pRepairedDocItem && pRepairedDocItem->GetValue() )
            aTitle += String( SfxResId(STR_REPAIREDDOCUMENT) );
    }

    if ( IsReadOnlyUI() || pMed && pMed->IsReadOnly() )
        aTitle += String( SfxResId(STR_READONLY) );
    return aTitle;
}

void SfxObjectShell::SetCreateMode_Impl( SfxObjectCreateMode nMode )
{
    eCreateMode = nMode;
}

BOOL SfxObjectShell::IsInPlaceActive()
{
    if ( eCreateMode != SFX_CREATE_MODE_EMBEDDED )
        return FALSE;

    SfxViewFrame* pFrame = SfxViewFrame::GetFirst( this );
    return pFrame && pFrame->GetFrame()->IsInPlace();
}

BOOL SfxObjectShell::IsUIActive()
{
    if ( eCreateMode != SFX_CREATE_MODE_EMBEDDED )
        return FALSE;

    SfxViewFrame* pFrame = SfxViewFrame::GetFirst( this );
    return pFrame && pFrame->GetFrame()->IsInPlace() && pFrame->GetFrame()->GetWorkWindow_Impl()->IsVisible_Impl();
}

void SfxObjectShell::UIActivate( BOOL )
{
}

void SfxObjectShell::InPlaceActivate( BOOL )
{
}

BOOL SfxObjectShell::HasMacrosLib_Impl() const
{
    Reference< XLibraryContainer > xContainer( const_cast< SfxObjectShell* >( this )->GetBasicContainer() );
    BOOL bHasMacros = xContainer.is();
    try
    {
        if ( bHasMacros )
        {
            // a library container exists; check if it's empty

            // if there are libraries except "Standard" library
            // we assume that they are not empty (because they have been created by the user)
            if ( !xContainer->hasElements() )
                bHasMacros = sal_False;
            else
            {
                ::rtl::OUString aStdLibName( RTL_CONSTASCII_USTRINGPARAM( "Standard" ) );
                uno::Sequence< ::rtl::OUString > aElements = xContainer->getElementNames();
                if ( aElements.getLength() )
                {
                    if ( aElements.getLength() > 1 || !aElements[0].equals( aStdLibName ) )
                        bHasMacros = sal_True;
                    else
                    {
                        // usually a "Standard" library is always present (design)
                        // for this reason we must check if it's empty
                        //
                        // Note: Since #i73229#, this is not true anymore. There's no default
                        // "Standard" lib anymore. Wouldn't it be time to get completely
                        // rid of the "Standard" thingie - this shouldn't be necessary
                        // anymore, should it?
                        // 2007-01-25 / frank.schoenheit@sun.com
                        uno::Reference < container::XNameAccess > xLib;
                        uno::Any aAny = xContainer->getByName( aStdLibName );
                        aAny >>= xLib;
                        if ( xLib.is() )
                            bHasMacros = xLib->hasElements();
                    }
                }
            }
        }
    }
    catch( uno::Exception& )
    {
    }

    return bHasMacros;
}

BOOL SfxObjectShell::HasMacrosStor_Impl() const
{
    sal_Bool bHasMacros = sal_False;
    if ( pImp->m_xDocStorage.is() )
        bHasMacros = StorageHasMacros( pImp->m_xDocStorage );

    return bHasMacros;
}

BOOL SfxObjectShell::StorageHasMacros( const uno::Reference< embed::XStorage >& xStorage )
{
    sal_Bool bHasMacros = sal_False;

    if ( xStorage.is() )
    {
        try
        {
            bHasMacros =
                ( xStorage->hasByName( ::rtl::OUString::createFromAscii("Basic") )
                    && xStorage->isStorageElement( ::rtl::OUString::createFromAscii("Basic") ) )
                ||  ( xStorage->hasByName( ::rtl::OUString::createFromAscii("Scripts") )
                    && xStorage->isStorageElement( ::rtl::OUString::createFromAscii("Scripts") ) );
        }
        catch ( uno::Exception& )
        {
            OSL_ASSERT( "Something is wrong with the checked storage!\n" );
        }
    }

    return bHasMacros;
}

sal_Bool SfxObjectShell::UseInteractionToHandleError(
                    const uno::Reference< task::XInteractionHandler >& xHandler,
                    sal_uInt32 nError )
{
    sal_Bool bResult = sal_False;

    if ( xHandler.is() )
    {
        try
        {
            uno::Any aInteraction;
            uno::Sequence< uno::Reference< task::XInteractionContinuation > > lContinuations(2);
            ::framework::ContinuationAbort* pAbort = new ::framework::ContinuationAbort();
            ::framework::ContinuationApprove* pApprove = new ::framework::ContinuationApprove();
            lContinuations[0] = uno::Reference< task::XInteractionContinuation >(
                                 static_cast< task::XInteractionContinuation* >( pAbort ), uno::UNO_QUERY );
            lContinuations[1] = uno::Reference< task::XInteractionContinuation >(
                                 static_cast< task::XInteractionContinuation* >( pApprove ), uno::UNO_QUERY );

            task::ErrorCodeRequest aErrorCode;
            aErrorCode.ErrCode = nError;
            aInteraction <<= aErrorCode;

            ::framework::InteractionRequest* pRequest = new ::framework::InteractionRequest(aInteraction,lContinuations);
            uno::Reference< task::XInteractionRequest > xRequest(
                             static_cast< task::XInteractionRequest* >( pRequest ),
                             uno::UNO_QUERY);

            xHandler->handle(xRequest);
            bResult = pAbort->isSelected();
        }
        catch( uno::Exception& )
        {}
    }

    return bResult;
}




