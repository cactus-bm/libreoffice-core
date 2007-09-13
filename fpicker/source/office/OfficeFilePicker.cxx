/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: OfficeFilePicker.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: ihi $ $Date: 2007-09-13 16:28:35 $
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
#include "precompiled_fpicker.hxx"

#include "OfficeFilePicker.hxx"

#ifndef _IODLGIMPL_HXX
#include "iodlg.hxx"
#endif

#ifndef _LIST_
#include <list>
#endif
#ifndef _FUNCTIONAL_
#include <functional>
#endif
#ifndef _ALGORITHM_
#include <algorithm>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#define _SVSTDARR_STRINGSDTOR
#include "svtools/svstdarr.hxx"

#ifndef  _COM_SUN_STAR_UNO_ANY_HXX_
#include <com/sun/star/uno/Any.hxx>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_FILEPICKEREVENT_HPP_
#include <com/sun/star/ui/dialogs/FilePickerEvent.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_FILEPREVIEWIMAGEFORMATS_HPP_
#include <com/sun/star/ui/dialogs/FilePreviewImageFormats.hpp>
#endif
#ifndef  _COM_SUN_STAR_UI_DIALOGS_TEMPLATEDESCRIPTION_HPP_
#include <com/sun/star/ui/dialogs/TemplateDescription.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_PROPERTYVALUE_HPP_
#include <com/sun/star/beans/PropertyValue.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_XWINDOW_HPP_
#include <com/sun/star/awt/XWindow.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_STRINGPAIR_HPP_
#include <com/sun/star/beans/StringPair.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif

#ifndef _UNOTOOLS_UCBHELPER_HXX
#include <unotools/ucbhelper.hxx>
#endif
#ifndef INCLUDED_SVTOOLS_PATHOPTIONS_HXX
#include <svtools/pathoptions.hxx>
#endif
#ifndef _COMPHELPER_SEQUENCE_HXX_
#include <comphelper/sequence.hxx>
#endif
#ifndef _CPPUHELPER_TYPEPROVIDER_HXX_
#include <cppuhelper/typeprovider.hxx>
#endif

#ifndef _VOS_MUTEX_HXX_
#include "vos/mutex.hxx"
#endif
#ifndef _SV_APP_HXX
#include "vcl/svapp.hxx"
#endif

// define ----------------------------------------------------------------

#define MAKE_ANY    ::com::sun::star::uno::makeAny

// using ----------------------------------------------------------------

using namespace     ::com::sun::star::container;
using namespace     ::com::sun::star::lang;
using namespace     ::com::sun::star::ui::dialogs;
using namespace     ::com::sun::star::uno;
using namespace     ::com::sun::star::beans;
using namespace     ::com::sun::star::awt;
using namespace     ::utl;

//=====================================================================

//=====================================================================

struct FilterEntry
{
protected:
    ::rtl::OUString     m_sTitle;
    ::rtl::OUString     m_sFilter;

    UnoFilterList       m_aSubFilters;

public:
    FilterEntry( const ::rtl::OUString& _rTitle, const ::rtl::OUString& _rFilter )
        :m_sTitle( _rTitle )
        ,m_sFilter( _rFilter )
    {
    }

    FilterEntry( const ::rtl::OUString& _rTitle, const UnoFilterList& _rSubFilters );

    ::rtl::OUString     getTitle() const { return m_sTitle; }
    ::rtl::OUString     getFilter() const { return m_sFilter; }

    /// determines if the filter has sub filter (i.e., the filter is a filter group in real)
    sal_Bool            hasSubFilters( ) const;

    /** retrieves the filters belonging to the entry
    @return
        the number of sub filters
    */
    sal_Int32           getSubFilters( UnoFilterList& _rSubFilterList );

    // helpers for iterating the sub filters
    const UnoFilterEntry*   beginSubFilters() const { return m_aSubFilters.getConstArray(); }
    const UnoFilterEntry*   endSubFilters() const { return m_aSubFilters.getConstArray() + m_aSubFilters.getLength(); }
};

//=====================================================================

//---------------------------------------------------------------------
FilterEntry::FilterEntry( const ::rtl::OUString& _rTitle, const UnoFilterList& _rSubFilters )
    :m_sTitle( _rTitle )
    ,m_aSubFilters( _rSubFilters )
{
}

//---------------------------------------------------------------------
sal_Bool FilterEntry::hasSubFilters( ) const
{
    return ( 0 < m_aSubFilters.getLength() );
}

//---------------------------------------------------------------------
sal_Int32 FilterEntry::getSubFilters( UnoFilterList& _rSubFilterList )
{
    _rSubFilterList = m_aSubFilters;
    return m_aSubFilters.getLength();
}

// struct ElementEntry_Impl ----------------------------------------------

struct ElementEntry_Impl
{
    sal_Int16       m_nElementID;
    sal_Int16       m_nControlAction;
    Any         m_aValue;
    rtl::OUString       m_aLabel;
    sal_Bool        m_bEnabled      : 1;

    sal_Bool        m_bHasValue     : 1;
    sal_Bool        m_bHasLabel     : 1;
    sal_Bool        m_bHasEnabled   : 1;

                    ElementEntry_Impl( sal_Int16 nId );

    void            setValue( const Any& rVal ) { m_aValue = rVal; m_bHasValue = sal_True; }
    void            setAction( sal_Int16 nAction ) { m_nControlAction = nAction; }
    void            setLabel( const rtl::OUString& rVal ) { m_aLabel = rVal; m_bHasLabel = sal_True; }
    void            setEnabled( sal_Bool bEnabled ) { m_bEnabled = bEnabled; m_bHasEnabled = sal_True; }
};

ElementEntry_Impl::ElementEntry_Impl( sal_Int16 nId )
    : m_nElementID( nId )
    , m_nControlAction( 0 )
    , m_bEnabled( sal_False )
    , m_bHasValue( sal_False )
    , m_bHasLabel( sal_False )
    , m_bHasEnabled( sal_False )
{}

//------------------------------------------------------------------------------------
void SvtFilePicker::prepareExecute()
{
    // set the default directory
    // --**-- doesn't match the spec yet
    if ( m_aDisplayDirectory.getLength() > 0 || m_aDefaultName.getLength() > 0 )
    {
        if ( m_aDisplayDirectory.getLength() > 0 )
        {

            INetURLObject aPath( m_aDisplayDirectory );
            if ( m_aDefaultName.getLength() > 0 )
            {
                aPath.insertName( m_aDefaultName );
                getDialog()->SetHasFilename( true );
            }
            getDialog()->SetPath( aPath.GetMainURL( INetURLObject::NO_DECODE ) );
        }
        else if ( m_aDefaultName.getLength() > 0 )
        {
            getDialog()->SetPath( m_aDefaultName );
            getDialog()->SetHasFilename( true );
        }
    }
    else
    {
        // Default-Standard-Dir setzen
        INetURLObject aStdDirObj( SvtPathOptions().GetWorkPath() );
        getDialog()->SetPath( aStdDirObj.GetMainURL( INetURLObject::NO_DECODE ) );
    }

    // set the control values and set the control labels, too
    if ( m_pElemList && !m_pElemList->empty() )
    {
        ::svt::OControlAccess aAccess( getDialog(), getDialog()->GetView() );

        ElementList::iterator aListIter;
        for ( aListIter = m_pElemList->begin();
              aListIter != m_pElemList->end(); ++aListIter )
        {
            ElementEntry_Impl& rEntry = *aListIter;
            if ( rEntry.m_bHasValue )
                aAccess.setValue( rEntry.m_nElementID, rEntry.m_nControlAction, rEntry.m_aValue );
            if ( rEntry.m_bHasLabel )
                aAccess.setLabel( rEntry.m_nElementID, rEntry.m_aLabel );
            if ( rEntry.m_bHasEnabled )
                aAccess.enableControl( rEntry.m_nElementID, rEntry.m_bEnabled );
        }

        getDialog()->updateListboxLabelSizes();
    }

    if ( m_pFilterList && !m_pFilterList->empty() )
    {
        for (   FilterList::iterator aListIter = m_pFilterList->begin();
                aListIter != m_pFilterList->end();
                ++aListIter
            )
        {
            if ( aListIter->hasSubFilters() )
            {   // it's a filter group
                UnoFilterList aSubFilters;
                aListIter->getSubFilters( aSubFilters );

                getDialog()->AddFilterGroup( aListIter->getTitle(), aSubFilters );
             }
            else
                // it's a single filter
                getDialog()->AddFilter( aListIter->getTitle(), aListIter->getFilter() );
        }
    }

    // set the default filter
    if ( m_aCurrentFilter.getLength() > 0 )
        getDialog()->SetCurFilter( m_aCurrentFilter );

}

//-----------------------------------------------------------------------------
IMPL_LINK( SvtFilePicker, DialogClosedHdl, Dialog*, pDlg )
{
    if ( m_xDlgClosedListener.is() )
    {
        sal_Int16 nRet = static_cast< sal_Int16 >( pDlg->GetResult() );
        ::com::sun::star::ui::dialogs::DialogClosedEvent aEvent( *this, nRet );
        m_xDlgClosedListener->dialogClosed( aEvent );
        m_xDlgClosedListener.clear();
    }
    return 0;
}

//------------------------------------------------------------------------------------
// SvtFilePicker
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
WinBits SvtFilePicker::getWinBits( WinBits& rExtraBits )
{
    // set the winbits for creating the filedialog
    WinBits nBits = 0L;
    rExtraBits = 0L;

    // set the standard bits acording to the service name
    if ( m_nServiceType == TemplateDescription::FILEOPEN_SIMPLE )
    {
        nBits = WB_OPEN;
    }
    else if ( m_nServiceType == TemplateDescription::FILESAVE_SIMPLE )
    {
        nBits = WB_SAVEAS;
    }
#if SUPD>639
    else if ( m_nServiceType == TemplateDescription::FILESAVE_AUTOEXTENSION )
    {
        nBits = WB_SAVEAS;
        rExtraBits = SFX_EXTRA_AUTOEXTENSION;
    }
#endif
    else if ( m_nServiceType == TemplateDescription::FILESAVE_AUTOEXTENSION_PASSWORD )
    {
        nBits = WB_SAVEAS | SFXWB_PASSWORD;
        rExtraBits = SFX_EXTRA_AUTOEXTENSION;
    }
    else if ( m_nServiceType == TemplateDescription::FILESAVE_AUTOEXTENSION_PASSWORD_FILTEROPTIONS )
    {
        nBits = WB_SAVEAS | SFXWB_PASSWORD;
        rExtraBits = SFX_EXTRA_AUTOEXTENSION | SFX_EXTRA_FILTEROPTIONS;
    }
    else if ( m_nServiceType == TemplateDescription::FILESAVE_AUTOEXTENSION_TEMPLATE )
    {
        nBits = WB_SAVEAS;
        rExtraBits = SFX_EXTRA_AUTOEXTENSION | SFX_EXTRA_TEMPLATES;
    }
    else if ( m_nServiceType == TemplateDescription::FILESAVE_AUTOEXTENSION_SELECTION )
    {
        nBits = WB_SAVEAS;
        rExtraBits = SFX_EXTRA_AUTOEXTENSION | SFX_EXTRA_SELECTION;
    }

    else if ( m_nServiceType == TemplateDescription::FILEOPEN_LINK_PREVIEW_IMAGE_TEMPLATE )
    {
        nBits = WB_OPEN;
        rExtraBits = SFX_EXTRA_INSERTASLINK | SFX_EXTRA_SHOWPREVIEW | SFX_EXTRA_IMAGE_TEMPLATE;
    }
    else if ( m_nServiceType == TemplateDescription::FILEOPEN_PLAY )
    {
        nBits = WB_OPEN;
        rExtraBits = SFX_EXTRA_PLAYBUTTON;
    }
    else if ( m_nServiceType == TemplateDescription::FILEOPEN_READONLY_VERSION )
    {
        nBits = WB_OPEN | SFXWB_READONLY;
        rExtraBits = SFX_EXTRA_SHOWVERSIONS;
    }
    else if ( m_nServiceType == TemplateDescription::FILEOPEN_LINK_PREVIEW )
    {
        nBits = WB_OPEN;
        rExtraBits = SFX_EXTRA_INSERTASLINK | SFX_EXTRA_SHOWPREVIEW;
    }
    if ( m_bMultiSelection && ( ( nBits & WB_OPEN ) == WB_OPEN ) )
        nBits |= SFXWB_MULTISELECTION;

    return nBits;
}

//------------------------------------------------------------------------------------
void SvtFilePicker::notify( sal_Int16 _nEventId, sal_Int16 _nControlId )
{
    if ( !m_xListener.is() )
        return;

    FilePickerEvent aEvent( *this, _nControlId );

    switch ( _nEventId )
    {
        case FILE_SELECTION_CHANGED:
            m_xListener->fileSelectionChanged( aEvent );
            break;
        case DIRECTORY_CHANGED:
            m_xListener->directoryChanged( aEvent );
            break;
        case HELP_REQUESTED:
            m_xListener->helpRequested( aEvent );
            break;
        case CTRL_STATE_CHANGED:
            m_xListener->controlStateChanged( aEvent );
            break;
        case DIALOG_SIZE_CHANGED:
            m_xListener->dialogSizeChanged();
            break;
        default:
            DBG_ERRORFILE( "SvtFilePicker::notify(): Unknown event id!" );
            break;
    }
}

//------------------------------------------------------------------------------------
namespace {
    //................................................................................
    struct FilterTitleMatch : public ::std::unary_function< FilterEntry, bool >
    {
    protected:
        const ::rtl::OUString& rTitle;

    public:
        FilterTitleMatch( const ::rtl::OUString& _rTitle ) : rTitle( _rTitle ) { }

        //............................................................................
        bool operator () ( const FilterEntry& _rEntry )
        {
            sal_Bool bMatch;
            if ( !_rEntry.hasSubFilters() )
                // a real filter
                bMatch = ( _rEntry.getTitle() == rTitle );
            else
                // a filter group -> search the sub filters
                bMatch =
                    _rEntry.endSubFilters() != ::std::find_if(
                        _rEntry.beginSubFilters(),
                        _rEntry.endSubFilters(),
                        *this
                    );

            return bMatch ? true : false;
        }
        bool operator () ( const UnoFilterEntry& _rEntry )
        {
            return _rEntry.First == rTitle ? true : false;
        }
    };
}

//------------------------------------------------------------------------------------
sal_Bool SvtFilePicker::FilterNameExists( const ::rtl::OUString& rTitle )
{
    sal_Bool bRet = sal_False;

    if ( m_pFilterList )
        bRet =
            m_pFilterList->end() != ::std::find_if(
                m_pFilterList->begin(),
                m_pFilterList->end(),
                FilterTitleMatch( rTitle )
            );

    return bRet;
}

//------------------------------------------------------------------------------------
sal_Bool SvtFilePicker::FilterNameExists( const UnoFilterList& _rGroupedFilters )
{
    sal_Bool bRet = sal_False;

    if ( m_pFilterList )
    {
        const UnoFilterEntry* pStart = _rGroupedFilters.getConstArray();
        const UnoFilterEntry* pEnd = pStart + _rGroupedFilters.getLength();
        for ( ; pStart != pEnd; ++pStart )
            if ( m_pFilterList->end() != ::std::find_if( m_pFilterList->begin(), m_pFilterList->end(), FilterTitleMatch( pStart->First ) ) )
                break;

        bRet = pStart != pEnd;
    }

    return bRet;
}

//------------------------------------------------------------------------------------
void SvtFilePicker::ensureFilterList( const ::rtl::OUString& _rInitialCurrentFilter )
{
    if ( !m_pFilterList )
    {
        m_pFilterList = new FilterList;

        // set the first filter to the current filter
        if ( ! m_aCurrentFilter.getLength() )
            m_aCurrentFilter = _rInitialCurrentFilter;
    }
}

//------------------------------------------------------------------------------------
// class SvtFilePicker
//------------------------------------------------------------------------------------
SvtFilePicker::SvtFilePicker( const Reference < XMultiServiceFactory >& xFactory )
    :OCommonPicker( xFactory )
    ,m_pFilterList      ( NULL )
    ,m_pElemList        ( NULL )
    ,m_bMultiSelection  ( sal_False )
    ,m_nServiceType     ( TemplateDescription::FILEOPEN_SIMPLE )
{
}

SvtFilePicker::~SvtFilePicker()
{
    if ( m_pFilterList && !m_pFilterList->empty() )
        m_pFilterList->erase( m_pFilterList->begin(), m_pFilterList->end() );
    delete m_pFilterList;

    if ( m_pElemList && !m_pElemList->empty() )
        m_pElemList->erase( m_pElemList->begin(), m_pElemList->end() );
    delete m_pElemList;
}

//------------------------------------------------------------------------------------
sal_Int16 SvtFilePicker::implExecutePicker( )
{
    getDialog()->SetFileCallback( this );

    prepareExecute();

    getDialog()->EnableAutocompletion( TRUE );
    // now we are ready to execute the dialog
    sal_Int16 nRet = getDialog()->Execute();

    // the execution of the dialog yields, so it is possible the at this point the window or the dialog is closed
    if ( getDialog() )
        getDialog()->SetFileCallback( NULL );

    return nRet;
}

//------------------------------------------------------------------------------------
SvtFileDialog* SvtFilePicker::implCreateDialog( Window* _pParent )
{
    WinBits nExtraBits;
    WinBits nBits = getWinBits( nExtraBits );

    return new SvtFileDialog( _pParent, nBits, nExtraBits );
}

//------------------------------------------------------------------------------------
// disambiguate XInterface
//------------------------------------------------------------------------------------
IMPLEMENT_FORWARD_XINTERFACE2( SvtFilePicker, OCommonPicker, SvtFilePicker_Base )

//------------------------------------------------------------------------------------
// disambiguate XTypeProvider
//------------------------------------------------------------------------------------
IMPLEMENT_FORWARD_XTYPEPROVIDER2( SvtFilePicker, OCommonPicker, SvtFilePicker_Base )

//------------------------------------------------------------------------------------
// XExecutableDialog functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::setTitle( const ::rtl::OUString& _rTitle ) throw (RuntimeException)
{
    OCommonPicker::setTitle( _rTitle );
}

//------------------------------------------------------------------------------------
sal_Int16 SAL_CALL SvtFilePicker::execute(  ) throw (RuntimeException)
{
    return OCommonPicker::execute();
}

//------------------------------------------------------------------------------------
// XAsynchronousExecutableDialog functions
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::setDialogTitle( const ::rtl::OUString& _rTitle ) throw (RuntimeException)
{
    setTitle( _rTitle );
}

//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::startExecuteModal( const Reference< ::com::sun::star::ui::dialogs::XDialogClosedListener >& xListener ) throw (RuntimeException)
{
    m_xDlgClosedListener = xListener;
    prepareDialog();
    prepareExecute();
    getDialog()->EnableAutocompletion( TRUE );
    getDialog()->StartExecuteModal( LINK( this, SvtFilePicker, DialogClosedHdl ) );
}

//------------------------------------------------------------------------------------
// XFilePicker functions
//------------------------------------------------------------------------------------

void SAL_CALL SvtFilePicker::setMultiSelectionMode( sal_Bool bMode ) throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    m_bMultiSelection = bMode;
}

void SAL_CALL SvtFilePicker::setDefaultName( const rtl::OUString& aName ) throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    m_aDefaultName = aName;
}

void SAL_CALL SvtFilePicker::setDisplayDirectory( const rtl::OUString& aDirectory )
    throw( IllegalArgumentException, RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    m_aDisplayDirectory = aDirectory;
}

rtl::OUString SAL_CALL SvtFilePicker::getDisplayDirectory() throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( getDialog() )
    {
        rtl::OUString aPath = getDialog()->GetPath();

        // #97148# ----
        if( m_aOldHideDirectory == aPath )
            return m_aOldDisplayDirectory;
        m_aOldHideDirectory = aPath;

        // #102204# -----
        if( !getDialog()->ContentIsFolder( aPath ) )
        {
            INetURLObject aFolder( aPath );
            aFolder.CutLastName();
            aPath = aFolder.GetMainURL( INetURLObject::NO_DECODE );
        }
        m_aOldDisplayDirectory = aPath;
        return aPath;
    }
    else
        return m_aDisplayDirectory;
}

Sequence< rtl::OUString > SAL_CALL SvtFilePicker::getFiles() throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( ! getDialog() )
    {
        Sequence< rtl::OUString > aEmpty;
        return aEmpty;
    }

    // if there is more than one path we have to return the path to the
    // files first and then the list of the selected entries

    SvStringsDtor* pPathList = getDialog()->GetPathList();
    USHORT i, nCount = pPathList->Count();
    USHORT nTotal = nCount > 1 ? nCount+1: nCount;

    Sequence< rtl::OUString > aPath( nTotal );

    if ( nCount == 1 )
        aPath[0] = rtl::OUString( *pPathList->GetObject( 0 ) );
    else if ( nCount > 1 )
    {
        INetURLObject aObj( *pPathList->GetObject( 0 ) );
        aObj.removeSegment();
        aPath[0] = aObj.GetMainURL( INetURLObject::NO_DECODE );

        for ( i = 0; i < nCount; /* i++ is done below */ )
        {
            aObj.SetURL( *pPathList->GetObject(i++) );
            aPath[i] = aObj.getName();
        }
    }

    delete pPathList;
    return aPath;
}

//------------------------------------------------------------------------------------
// XFilePickerControlAccess functions
//------------------------------------------------------------------------------------

void SAL_CALL SvtFilePicker::setValue( sal_Int16 nElementID,
                                       sal_Int16 nControlAction,
                                       const Any& rValue )
    throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( getDialog() )
    {
        ::svt::OControlAccess aAccess( getDialog(), getDialog()->GetView() );
        aAccess.setValue( nElementID, nControlAction, rValue );
    }
    else
    {
        if ( !m_pElemList )
            m_pElemList = new ElementList;

        sal_Bool bFound = sal_False;
        ElementList::iterator aListIter;

        for ( aListIter = m_pElemList->begin();
              aListIter != m_pElemList->end(); ++aListIter )
        {
            ElementEntry_Impl& rEntry = *aListIter;
            if ( ( rEntry.m_nElementID == nElementID ) &&
                 ( !rEntry.m_bHasValue || ( rEntry.m_nControlAction == nControlAction ) ) )
            {
                rEntry.setAction( nControlAction );
                rEntry.setValue( rValue );
                bFound = sal_True;
            }
        }

        if ( !bFound )
        {
            ElementEntry_Impl aNew( nElementID );
            aNew.setAction( nControlAction );
            aNew.setValue( rValue );
            m_pElemList->insert( m_pElemList->end(), aNew );
        }
    }
}

//------------------------------------------------------------------------------------

Any SAL_CALL SvtFilePicker::getValue( sal_Int16 nElementID, sal_Int16 nControlAction )
    throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    Any      aAny;

    // execute() called?
    if ( getDialog() )
    {
        ::svt::OControlAccess aAccess( getDialog(), getDialog()->GetView() );
        aAny = aAccess.getValue( nElementID, nControlAction );
    }
    else if ( m_pElemList && !m_pElemList->empty() )
    {
        ElementList::iterator aListIter;
        for ( aListIter = m_pElemList->begin();
              aListIter != m_pElemList->end(); ++aListIter )
        {
            ElementEntry_Impl& rEntry = *aListIter;
            if ( ( rEntry.m_nElementID == nElementID ) &&
                 ( rEntry.m_bHasValue ) &&
                 ( rEntry.m_nControlAction == nControlAction ) )
            {
                aAny = rEntry.m_aValue;
                break;
            }
        }
    }

    return aAny;
}


//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::setLabel( sal_Int16 nLabelID, const rtl::OUString& rValue )
    throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( getDialog() )
    {
        ::svt::OControlAccess aAccess( getDialog(), getDialog()->GetView() );
        aAccess.setLabel( nLabelID, rValue );
    }
    else
    {
        if ( !m_pElemList )
            m_pElemList = new ElementList;

        sal_Bool bFound = sal_False;
        ElementList::iterator aListIter;

        for ( aListIter = m_pElemList->begin();
              aListIter != m_pElemList->end(); ++aListIter )
        {
            ElementEntry_Impl& rEntry = *aListIter;
            if ( rEntry.m_nElementID == nLabelID )
            {
                rEntry.setLabel( rValue );
                bFound = sal_True;
            }
        }

        if ( !bFound )
        {
            ElementEntry_Impl aNew( nLabelID );
            aNew.setLabel( rValue );
            m_pElemList->insert( m_pElemList->end(), aNew );
        }
    }
}

//------------------------------------------------------------------------------------
rtl::OUString SAL_CALL SvtFilePicker::getLabel( sal_Int16 nLabelID )
    throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    rtl::OUString aLabel;

    if ( getDialog() )
    {
        ::svt::OControlAccess aAccess( getDialog(), getDialog()->GetView() );
        aLabel = aAccess.getLabel( nLabelID );
    }
    else if ( m_pElemList && !m_pElemList->empty() )
    {
        ElementList::iterator aListIter;
        for ( aListIter = m_pElemList->begin();
              aListIter != m_pElemList->end(); ++aListIter )
        {
            ElementEntry_Impl& rEntry = *aListIter;
            if ( rEntry.m_nElementID == nLabelID )
            {
                if ( rEntry.m_bHasLabel )
                    aLabel = rEntry.m_aLabel;
                break;
            }
        }
    }

    return aLabel;
}

//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::enableControl( sal_Int16 nElementID, sal_Bool bEnable )
    throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( getDialog() )
    {
        ::svt::OControlAccess aAccess( getDialog(), getDialog()->GetView() );
        aAccess.enableControl( nElementID, bEnable );
    }
    else
    {
        if ( !m_pElemList )
            m_pElemList = new ElementList;

        sal_Bool bFound = sal_False;
        ElementList::iterator aListIter;

        for ( aListIter = m_pElemList->begin();
              aListIter != m_pElemList->end(); ++aListIter )
        {
            ElementEntry_Impl& rEntry = *aListIter;
            if ( rEntry.m_nElementID == nElementID )
            {
                rEntry.setEnabled( bEnable );
                bFound = sal_True;
            }
        }

        if ( !bFound )
        {
            ElementEntry_Impl aNew( nElementID );
            aNew.setEnabled( bEnable );
            m_pElemList->insert( m_pElemList->end(), aNew );
        }
    }
}

//------------------------------------------------------------------------------------
// XFilePickerNotifier functions
//------------------------------------------------------------------------------------

void SAL_CALL SvtFilePicker::addFilePickerListener( const Reference< XFilePickerListener >& xListener ) throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    m_xListener = xListener;
}

//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::removeFilePickerListener( const Reference< XFilePickerListener >& ) throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    m_xListener.clear();
}

//------------------------------------------------------------------------------------
// XFilePreview functions
//------------------------------------------------------------------------------------

Sequence< sal_Int16 > SAL_CALL SvtFilePicker::getSupportedImageFormats()
    throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    Sequence< sal_Int16 > aFormats( 1 );

    aFormats[0] = FilePreviewImageFormats::BITMAP;

    return aFormats;
}

//------------------------------------------------------------------------------------
sal_Int32 SAL_CALL SvtFilePicker::getTargetColorDepth() throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    sal_Int32 nDepth = 0;

    if ( getDialog() )
        nDepth = getDialog()->getTargetColorDepth();

    return nDepth;
}

//------------------------------------------------------------------------------------
sal_Int32 SAL_CALL SvtFilePicker::getAvailableWidth() throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    sal_Int32 nWidth = 0;

    if ( getDialog() )
        nWidth = getDialog()->getAvailableWidth();

    return nWidth;
}

//------------------------------------------------------------------------------------
sal_Int32 SAL_CALL SvtFilePicker::getAvailableHeight() throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    sal_Int32 nHeigth = 0;

    if ( getDialog() )
        nHeigth = getDialog()->getAvailableHeight();

    return nHeigth;
}

//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::setImage( sal_Int16 aImageFormat, const Any& rImage )
    throw ( IllegalArgumentException, RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( getDialog() )
        getDialog()->setImage( aImageFormat, rImage );
}

//------------------------------------------------------------------------------------
sal_Bool SAL_CALL SvtFilePicker::setShowState( sal_Bool bShowState )
    throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    sal_Bool bRet = sal_False;

    if ( getDialog() )
        bRet = getDialog()->setShowState( bShowState );

    return bRet;
}

//------------------------------------------------------------------------------------
sal_Bool SAL_CALL SvtFilePicker::getShowState() throw ( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    sal_Bool bRet = sal_False;

    if ( getDialog() )
        bRet = getDialog()->getShowState();

    return bRet;
}

//------------------------------------------------------------------------------------
// XFilterGroupManager functions
//------------------------------------------------------------------------------------

void SAL_CALL SvtFilePicker::appendFilterGroup( const ::rtl::OUString& sGroupTitle,
                                                const Sequence< StringPair >& aFilters )
    throw ( IllegalArgumentException, RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );

    // check the names
    if ( FilterNameExists( aFilters ) )
        // TODO: a more precise exception message
        throw IllegalArgumentException();

    // ensure that we have a filter list
    ::rtl::OUString sInitialCurrentFilter;
    if ( aFilters.getLength() )
        sInitialCurrentFilter = aFilters[0].First;
    ensureFilterList( sInitialCurrentFilter );

    // append the filter
    m_pFilterList->insert( m_pFilterList->end(), FilterEntry( sGroupTitle, aFilters ) );
}

//------------------------------------------------------------------------------------
// XFilterManager functions
//------------------------------------------------------------------------------------

void SAL_CALL SvtFilePicker::appendFilter( const rtl::OUString& aTitle,
                                           const rtl::OUString& aFilter )
    throw( IllegalArgumentException, RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    // check the name
    if ( FilterNameExists( aTitle ) )
        // TODO: a more precise exception message
        throw IllegalArgumentException();

    // ensure that we have a filter list
    ensureFilterList( aTitle );

    // append the filter
    m_pFilterList->insert( m_pFilterList->end(), FilterEntry( aTitle, aFilter ) );
}

//------------------------------------------------------------------------------------
void SAL_CALL SvtFilePicker::setCurrentFilter( const rtl::OUString& aTitle )
    throw( IllegalArgumentException, RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    if ( ! FilterNameExists( aTitle ) )
        throw IllegalArgumentException();

    m_aCurrentFilter = aTitle;

    if ( getDialog() )
        getDialog()->SetCurFilter( aTitle );
}

//------------------------------------------------------------------------------------
rtl::OUString SAL_CALL SvtFilePicker::getCurrentFilter()
    throw( RuntimeException )
{
    checkAlive();

    ::vos::OGuard aGuard( Application::GetSolarMutex() );
    rtl::OUString aFilter = getDialog() ? rtl::OUString( getDialog()->GetCurFilter() ) :
                                            rtl::OUString( m_aCurrentFilter );
    return aFilter;
}


//------------------------------------------------------------------------------------
// XInitialization functions
//------------------------------------------------------------------------------------

void SAL_CALL SvtFilePicker::initialize( const Sequence< Any >& _rArguments )
    throw ( Exception, RuntimeException )
{
    checkAlive();

    m_nServiceType = TemplateDescription::FILEOPEN_SIMPLE;

    if ( _rArguments.getLength() == 1 )
    {
        // compatibility: one argument, type sal_Int16 , specifies the service type
        if ( _rArguments.getLength() )
        {
            if ( _rArguments[0] >>= m_nServiceType )
                return;
        }
    }

    // let the base class analyze the sequence (will call into implHandleInitializationArgument)
    OCommonPicker::initialize( _rArguments );
}

//-------------------------------------------------------------------------
sal_Bool SvtFilePicker::implHandleInitializationArgument( const ::rtl::OUString& _rName, const Any& _rValue ) SAL_THROW( ( Exception, RuntimeException ) )
{
    if ( _rName.equalsAscii( "TemplateDescription" ) )
    {
        m_nServiceType = TemplateDescription::FILEOPEN_SIMPLE;
        OSL_VERIFY( _rValue >>= m_nServiceType );
        return sal_True;
    }
    return OCommonPicker::implHandleInitializationArgument( _rName, _rValue );
}

//------------------------------------------------------------------------------------
// XServiceInfo
//------------------------------------------------------------------------------------

/* XServiceInfo */
rtl::OUString SAL_CALL SvtFilePicker::getImplementationName() throw( RuntimeException )
{
    return impl_getStaticImplementationName();
}

/* XServiceInfo */
sal_Bool SAL_CALL SvtFilePicker::supportsService( const rtl::OUString& sServiceName ) throw( RuntimeException )
{
    Sequence< rtl::OUString > seqServiceNames = getSupportedServiceNames();
    const rtl::OUString* pArray = seqServiceNames.getConstArray();
    for ( sal_Int32 i = 0; i < seqServiceNames.getLength(); i++ )
    {
        if ( sServiceName == pArray[i] )
        {
            return sal_True ;
        }
    }
    return sal_False ;
}

/* XServiceInfo */
Sequence< rtl::OUString > SAL_CALL SvtFilePicker::getSupportedServiceNames() throw( RuntimeException )
{
    return impl_getStaticSupportedServiceNames();
}

/* Helper for XServiceInfo */
Sequence< rtl::OUString > SvtFilePicker::impl_getStaticSupportedServiceNames()
{
    Sequence< rtl::OUString > seqServiceNames( 1 );
    rtl::OUString* pArray = seqServiceNames.getArray();
    pArray[0] = rtl::OUString::createFromAscii( "com.sun.star.ui.dialogs.OfficeFilePicker" );
    return seqServiceNames ;
}

/* Helper for XServiceInfo */
rtl::OUString SvtFilePicker::impl_getStaticImplementationName()
{
    return rtl::OUString::createFromAscii( "com.sun.star.svtools.OfficeFilePicker" );
}

/* Helper for registry */
Reference< XInterface > SAL_CALL SvtFilePicker::impl_createInstance(
    const Reference< XComponentContext >& rxContext) throw( Exception )
{
    Reference< XMultiServiceFactory > xServiceManager (rxContext->getServiceManager(), UNO_QUERY_THROW);
    return Reference< XInterface >( *new SvtFilePicker( xServiceManager ) );
}
