/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: richtextvclcontrol.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2006-09-17 00:01:17 $
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
#include "precompiled_forms.hxx"

#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTVCLCONTROL_HXX
#include "richtextvclcontrol.hxx"
#endif
#ifndef FORMS_SOURCE_RICHTEXT_RICHTEXTIMPLCONTOL_HXX
#include "richtextimplcontrol.hxx"
#endif

#ifndef _SFXITEMPOOL_HXX
#include <svtools/itempool.hxx>
#endif
#ifndef _SFXITEMSET_HXX
#include <svtools/itemset.hxx>
#endif
#ifndef _SVTOOLS_LANGUAGEOPTIONS_HXX
#include <svtools/languageoptions.hxx>
#endif
#if OSL_DEBUG_LEVEL > 0
    #ifndef _TOOLS_TEMPFILE_HXX
    #include <tools/tempfile.hxx>
    #endif
    #ifndef _UNTOOLS_UCBSTREAMHELPER_HXX
    #include <unotools/ucbstreamhelper.hxx>
    #endif
    #ifndef _SV_MSGBOX_HXX
    #include <vcl/msgbox.hxx>
    #endif
    #ifndef _FILEDLGHELPER_HXX
    #include <sfx2/filedlghelper.hxx>
    #endif
    #ifndef _URLOBJ_HXX
    #include <tools/urlobj.hxx>
    #endif
#endif

#ifndef _SVX_SCRIPTTYPEITEM_HXX
#include <svx/scripttypeitem.hxx>
#endif
#ifndef _MyEDITENG_HXX
#include <svx/editeng.hxx>
#endif
#ifndef _MyEDITVIEW_HXX
#include <svx/editview.hxx>
#endif
#ifndef _EEITEM_HXX
#include <svx/eeitem.hxx>
#endif

#define ITEMID_FONT         EE_CHAR_FONTINFO
#ifndef _SVX_FONTITEM_HXX
#include <svx/fontitem.hxx>
#endif
#define ITEMID_FONTHEIGHT   EE_CHAR_FONTHEIGHT
#ifndef _SVX_FHGTITEM_HXX
#include <svx/fhgtitem.hxx>
#endif

#include <memory>

//........................................................................
namespace frm
{
//........................................................................

    using namespace ::com::sun::star::awt;

    //====================================================================
    //= RichTextControl
    //====================================================================
    //--------------------------------------------------------------------
    RichTextControl::RichTextControl( RichTextEngine* _pEngine, Window* _pParent, WinBits _nStyle,
        ITextAttributeListener* _pTextAttribListener, ITextSelectionListener* _pSelectionListener )
        :Control( _pParent, implInitStyle( _nStyle ) )
        ,m_pImpl( NULL )
    {
        implInit( _pEngine, _pTextAttribListener, _pSelectionListener );
    }

    //--------------------------------------------------------------------
    void RichTextControl::implInit( RichTextEngine* _pEngine, ITextAttributeListener* _pTextAttribListener, ITextSelectionListener* _pSelectionListener )
    {
        m_pImpl = new RichTextControlImpl( this, _pEngine, _pTextAttribListener, _pSelectionListener );
        SetCompoundControl( TRUE );
    }

    //--------------------------------------------------------------------
    RichTextControl::~RichTextControl( )
    {
        delete m_pImpl;
    }

    //--------------------------------------------------------------------
    AttributeState RichTextControl::getState( AttributeId _nAttributeId ) const
    {
        return m_pImpl->getAttributeState( _nAttributeId );
    }

    //--------------------------------------------------------------------
    void RichTextControl::executeAttribute( AttributeId _nAttributeId, const SfxPoolItem* _pArgument )
    {
        SfxItemSet aToApplyAttributes( getView().GetEmptyItemSet() );
        if ( !m_pImpl->executeAttribute( getView().GetAttribs(), aToApplyAttributes, _nAttributeId, _pArgument, m_pImpl->getSelectedScriptType() ) )
        {
            OSL_ENSURE( sal_False, "RichTextControl::executeAttribute: cannot handle the given attribute!" );
            return;
        }

        applyAttributes( aToApplyAttributes );
    }

    //--------------------------------------------------------------------
    void RichTextControl::applyAttributes( const SfxItemSet& _rAttributesToApply )
    {
        // apply
        if ( HasChildPathFocus() )
            getView().HideCursor();

        BOOL bOldUpdateMode = getEngine().GetUpdateMode();   // TODO: guard?
        getEngine().SetUpdateMode( FALSE );

        getView().SetAttribs( _rAttributesToApply );

        getEngine().SetUpdateMode( bOldUpdateMode );
        getView().Invalidate();

        if ( HasChildPathFocus() )
            getView().ShowCursor();

        m_pImpl->updateAllAttributes();
            // TODO: maybe we should have a list of attributes which need to be updated
            // (the handler for the just executed attribute should know)
    }

    //--------------------------------------------------------------------
    void RichTextControl::enableAttributeNotification( AttributeId _nAttributeId, ITextAttributeListener* _pListener )
    {
        m_pImpl->enableAttributeNotification( _nAttributeId, _pListener );
    }

    //--------------------------------------------------------------------
    void RichTextControl::disableAttributeNotification( AttributeId _nAttributeId )
    {
        m_pImpl->disableAttributeNotification( _nAttributeId );
    }

    //--------------------------------------------------------------------
    bool RichTextControl::isMappableSlot( SfxSlotId _nSlotId )
    {
        switch ( _nSlotId )
        {
            case SID_ATTR_PARA_ADJUST_LEFT:
            case SID_ATTR_PARA_ADJUST_CENTER:
            case SID_ATTR_PARA_ADJUST_RIGHT:
            case SID_ATTR_PARA_ADJUST_BLOCK:
            case SID_SET_SUPER_SCRIPT:
            case SID_SET_SUB_SCRIPT:
            case SID_ATTR_PARA_LINESPACE_10:
            case SID_ATTR_PARA_LINESPACE_15:
            case SID_ATTR_PARA_LINESPACE_20:
            case SID_ATTR_PARA_LEFT_TO_RIGHT:
            case SID_ATTR_PARA_RIGHT_TO_LEFT:
            case SID_TEXTDIRECTION_TOP_TO_BOTTOM:
            case SID_TEXTDIRECTION_LEFT_TO_RIGHT:
            case SID_ATTR_CHAR_LATIN_FONT:
            case SID_ATTR_CHAR_LATIN_FONTHEIGHT:
            case SID_ATTR_CHAR_LATIN_LANGUAGE:
            case SID_ATTR_CHAR_LATIN_POSTURE:
            case SID_ATTR_CHAR_LATIN_WEIGHT:
                return true;
        }
        return false;
    }

    //--------------------------------------------------------------------
    void RichTextControl::convert( const SfxPoolItem& _rItem, FontDescriptor& _rUnoFont )
    {
        OSL_ENSURE( _rItem.ISA( SvxFontItem ), "RichTextControl::convert: invalid item type!" );

        _rUnoFont.Name    = static_cast< const SvxFontItem& >( _rItem ).GetFamilyName();
        _rUnoFont.Family  = static_cast< const SvxFontItem& >( _rItem ).GetFamily();
        _rUnoFont.Pitch   = static_cast< const SvxFontItem& >( _rItem ).GetPitch();
        _rUnoFont.CharSet = static_cast< const SvxFontItem& >( _rItem ).GetCharSet();
    }

    //--------------------------------------------------------------------
    void RichTextControl::convert( const FontDescriptor& _rUnoFont, SfxPoolItem*& _rpFontItem )
    {
        SvxFontItem* pFontItem = new SvxFontItem( SID_ATTR_CHAR_FONT );
        pFontItem->GetFamilyName() = _rUnoFont.Name;
        pFontItem->GetFamily()     = (FontFamily)_rUnoFont.Family;
        pFontItem->GetPitch()      = (FontPitch)_rUnoFont.Pitch;
        pFontItem->GetCharSet()    = _rUnoFont.CharSet;

        _rpFontItem = pFontItem;
    }

    //--------------------------------------------------------------------
    sal_uInt32 RichTextControl::convertFontHeight( const SfxPoolItem& _rItem )
    {
        OSL_ENSURE( _rItem.ISA( SvxFontHeightItem ), "RichTextControl::convert: invalid item type!" );
        return static_cast< const SvxFontHeightItem& >( _rItem ).GetHeight();
    }

    //--------------------------------------------------------------------
    SfxPoolItem* RichTextControl::convertFontHeight( sal_uInt32 _nUnoFontHeight )
    {
        SvxFontHeightItem* pItem = new SvxFontHeightItem( _nUnoFontHeight );
        pItem->SetProp( 100, SFX_MAPUNIT_POINT );
        return pItem;
    }

    //--------------------------------------------------------------------
    void RichTextControl::Resize()
    {
        m_pImpl->layoutWindow();
        Invalidate();
    }

    //--------------------------------------------------------------------
    void RichTextControl::GetFocus()
    {
        getViewport().GrabFocus();
    }

    //--------------------------------------------------------------------
    WinBits RichTextControl::implInitStyle( WinBits nStyle )
    {
        if ( !( nStyle & WB_NOTABSTOP ) )
            nStyle |= WB_TABSTOP;
        return nStyle;
    }

    //--------------------------------------------------------------------
    void RichTextControl::StateChanged( StateChangedType _nStateChange )
    {
        if ( _nStateChange == STATE_CHANGE_STYLE )
        {
            SetStyle( implInitStyle( GetStyle() ) );
            m_pImpl->notifyStyleChanged();
        }
        else if ( _nStateChange == STATE_CHANGE_ZOOM )
        {
            m_pImpl->notifyZoomChanged();
        }
        else if ( _nStateChange == STATE_CHANGE_INITSHOW )
        {
            m_pImpl->notifyInitShow();
        }
        Control::StateChanged( _nStateChange );
    }

    //--------------------------------------------------------------------
    long RichTextControl::PreNotify( NotifyEvent& _rNEvt )
    {
        if ( IsWindowOrChild( _rNEvt.GetWindow() ) )
        {
            if ( EVENT_KEYINPUT == _rNEvt.GetType() )
            {
                const KeyEvent* pKeyEvent = _rNEvt.GetKeyEvent();

                sal_uInt16 nCode = pKeyEvent->GetKeyCode().GetCode();
                sal_Bool   bShift = pKeyEvent->GetKeyCode().IsShift();
                sal_Bool   bCtrl = pKeyEvent->GetKeyCode().IsMod1();
                sal_Bool   bAlt = pKeyEvent->GetKeyCode().IsMod2();
                if ( ( KEY_TAB == nCode ) && bCtrl && !bAlt )
                {
                    // Ctrl-Tab is used to step out of the control
                    // -> build a new key event without the Ctrl-key, and let the very base class handle it
                    KeyCode aNewCode( KEY_TAB, bShift, sal_False, sal_False );
                    KeyEvent aNewEvent( pKeyEvent->GetCharCode(), aNewCode );
                    Control::KeyInput( aNewEvent );
                    return 1;   // handled
                }

#if OSL_DEBUG_LEVEL > 0
                if  (   (   ( KEY_F12 == nCode )
                        ||  ( KEY_F11 == nCode )
                        )
                    &&  bCtrl
                    &&  bAlt
                    )
                {
                    bool bLoad = KEY_F11 == nCode;
                    struct
                    {
                        const sal_Char* pDescription;
                        const sal_Char* pExtension;
                        EETextFormat    eFormat;
                    } aExportFormats[] =
                    {
                        { "OASIS OpenDocument (*.xml)", "*.xml", EE_FORMAT_XML },
                        { "HyperText Markup Language (*.html)", "*.html", EE_FORMAT_HTML },
                        { "Rich Text format (*.rtf)", "*.rtf", EE_FORMAT_RTF },
                        { "Text (*.txt)", "*.txt", EE_FORMAT_TEXT }
                    };

                    ::sfx2::FileDialogHelper aFP( bLoad ? ::sfx2::FILEOPEN_SIMPLE : ::sfx2::FILESAVE_AUTOEXTENSION, 0, this );

                    for ( size_t i = 0; i < sizeof( aExportFormats ) / sizeof( aExportFormats[ 0 ] ); ++i )
                    {
                        aFP.AddFilter(
                            String::CreateFromAscii( aExportFormats[i].pDescription ),
                            String::CreateFromAscii( aExportFormats[i].pExtension ) );
                    }
                    ErrCode nResult = aFP.Execute();
                    if ( nResult == 0 )
                    {
                        String sFileName = aFP.GetPath();
                        SvStream* pStream = ::utl::UcbStreamHelper::CreateStream(
                            sFileName, ( bLoad ? STREAM_READ : STREAM_WRITE | STREAM_TRUNC ) | STREAM_SHARE_DENYALL
                        );
                        if ( pStream )
                        {
                            EETextFormat eFormat = EE_FORMAT_XML;
                            String sFilter = aFP.GetCurrentFilter();
                            for ( size_t i = 0; i < sizeof( aExportFormats ) / sizeof( aExportFormats[ 0 ] ); ++i )
                            {
                                if ( sFilter.EqualsAscii( aExportFormats[i].pDescription ) )
                                {
                                    eFormat = aExportFormats[i].eFormat;
                                    break;
                                }
                            }
                            if ( bLoad )
                            {
                                INetURLObject aURL( sFileName );
                                aURL.removeSegment();
                                getEngine().Read( *pStream, aURL.GetMainURL( INetURLObject::NO_DECODE ), eFormat );
                            }
                            else
                            {
                                getEngine().Write( *pStream, eFormat );
                            }
                        }
                        DELETEZ( pStream );
                    }
                    return 1;   // handled
                }
#endif
            }
        }
        return Control::PreNotify( _rNEvt );
    }

    //--------------------------------------------------------------------
    long RichTextControl::Notify( NotifyEvent& _rNEvt )
    {
        long nDone = 0;
        if ( _rNEvt.GetType() == EVENT_COMMAND )
        {
            const CommandEvent& rEvent = *_rNEvt.GetCommandEvent();
            nDone = m_pImpl->HandleCommand( rEvent );
        }
        return nDone ? nDone : Control::Notify( _rNEvt );
    }

    //--------------------------------------------------------------------
    void RichTextControl::Draw( OutputDevice* _pDev, const Point& _rPos, const Size& _rSize, ULONG _nFlags )
    {
        m_pImpl->Draw( _pDev, _rPos, _rSize, _nFlags );
    }

    //--------------------------------------------------------------------
    EditView& RichTextControl::getView()
    {
        return *m_pImpl->getView( RichTextControlImpl::GrantAccess() );
    }

    //--------------------------------------------------------------------
    const EditView& RichTextControl::getView() const
    {
        return *m_pImpl->getView( RichTextControlImpl::GrantAccess() );
    }

    //--------------------------------------------------------------------
    EditEngine& RichTextControl::getEngine() const
    {
        return *m_pImpl->getEngine( RichTextControlImpl::GrantAccess() );
    }

    //--------------------------------------------------------------------
    Window& RichTextControl::getViewport() const
    {
        return *m_pImpl->getViewport( RichTextControlImpl::GrantAccess() );
    }

    //--------------------------------------------------------------------
    void RichTextControl::SetReadOnly( bool _bReadOnly )
    {
        m_pImpl->SetReadOnly( _bReadOnly );
    }

    //--------------------------------------------------------------------
    bool RichTextControl::IsReadOnly() const
    {
        return m_pImpl->IsReadOnly();
    }

    //--------------------------------------------------------------------
    void RichTextControl::SetBackgroundColor( )
    {
        m_pImpl->SetBackgroundColor( );
    }

    //--------------------------------------------------------------------
    void RichTextControl::SetBackgroundColor( const Color& _rColor )
    {
        m_pImpl->SetBackgroundColor( _rColor );
    }

    //--------------------------------------------------------------------
    void RichTextControl::SetHideInactiveSelection( bool _bHide )
    {
        m_pImpl->SetHideInactiveSelection( _bHide );
    }

    //--------------------------------------------------------------------
    bool RichTextControl::GetHideInactiveSelection() const
    {
        return m_pImpl->GetHideInactiveSelection( );
    }

//........................................................................
}   // namespace frm
//........................................................................


