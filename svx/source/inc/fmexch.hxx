/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fmexch.hxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: rt $ $Date: 2006-05-04 08:34:38 $
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
#ifndef _SVX_FMEXCH_HXX
#define _SVX_FMEXCH_HXX

#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#ifndef _TRANSFER_HXX
#include <svtools/transfer.hxx>
#endif

#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _LINK_HXX
#include <tools/link.hxx>
#endif

class FmFormShell;
class FmFormPage;
class SvLBoxEntry;

//========================================================================
// Exchange types
#define SVX_FM_FIELD_EXCH           String("SvxFormFieldExch",              sizeof("SvxFormFieldExch"))
#define SVX_FM_CONTROL_EXCH         String("SvxFormExplCtrlExch",           sizeof("SvxFormExplCtrlExch"))
#define SVX_FM_CONTROLS_AS_PATH     String("SvxFormControlsAsPathExchange", sizeof("SvxFormControlsAsPathExchange"))
#define SVX_FM_HIDDEN_CONTROLS      String("SvxFormHiddenControlsExchange", sizeof("SvxFormHiddenControlsExchange"))
#define SVX_FM_FILTER_FIELDS        String("SvxFilterFieldExchange",        sizeof("SvxFilterFieldExchange"))

//========================================================================
class SvTreeListBox;

//........................................................................
namespace svxform
{
//........................................................................

    //====================================================================

    typedef ::std::set< SvLBoxEntry* >  ListBoxEntrySet;

    //====================================================================
    //= OLocalExchange
    //====================================================================
    class OLocalExchange : public TransferableHelper
    {
    private:
        Link                m_aClipboardListener;
        sal_Bool            m_bDragging         : 1;
        sal_Bool            m_bClipboardOwner   : 1;

    public:
        class GrantAccess
        {
            friend class OLocalExchangeHelper;
        };

    public:
        OLocalExchange( );

        sal_Bool    isDragging() const { return m_bDragging; }
        sal_Bool    isClipboardOwner() const { return m_bClipboardOwner; }

        void        startDrag( Window* pWindow, sal_Int8 nDragSourceActions, const GrantAccess& );
        void        copyToClipboard( Window* _pWindow, const GrantAccess& );

        void        setClipboardListener( const Link& _rListener ) { m_aClipboardListener = _rListener; }

        void        clear();

        static  sal_Bool    hasFormat( const DataFlavorExVector& _rFormats, sal_uInt32 _nFormatId );

    protected:
        // XClipboardOwner
        virtual void SAL_CALL lostOwnership( const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::clipboard::XClipboard >& _rxClipboard, const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& _rxTrans ) throw(::com::sun::star::uno::RuntimeException);

        // TransferableHelper
        virtual void        DragFinished( sal_Int8 nDropAction );
        virtual sal_Bool    GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor );

    private:
        void StartDrag( Window* pWindow, sal_Int8 nDragSourceActions, sal_Int32 nDragPointer = DND_POINTER_NONE, sal_Int32 nDragImage = DND_IMAGE_NONE )
        {   // don't allow this base class method to be called from outside
            TransferableHelper::StartDrag(pWindow, nDragSourceActions, nDragPointer, nDragImage);
        }
    };

    //====================================================================
    //= OLocalExchangeHelper
    //====================================================================
    /// a helper for navigator windows (SvTreeListBox'es) which allow DnD within themself
    class OLocalExchangeHelper
    {
    protected:
        Window*             m_pDragSource;
        OLocalExchange*     m_pTransferable;

    public:
        OLocalExchangeHelper( Window* _pDragSource );
        ~OLocalExchangeHelper();

        void        prepareDrag( );

        void        startDrag( sal_Int8 nDragSourceActions );
        void        copyToClipboard( ) const;

        inline  sal_Bool    isDragSource() const { return m_pTransferable && m_pTransferable->isDragging(); }
        inline  sal_Bool    isClipboardOwner() const { return m_pTransferable && m_pTransferable->isClipboardOwner(); }
        inline  sal_Bool    isDataExchangeActive( ) const { return isDragSource() || isClipboardOwner(); }
        inline  void        clear() { if ( isDataExchangeActive() ) m_pTransferable->clear(); }

        void        setClipboardListener( const Link& _rListener ) { if ( m_pTransferable ) m_pTransferable->setClipboardListener( _rListener ); }

    protected:
        virtual OLocalExchange* createExchange() const = 0;

    protected:
        void implReset();
    };

    //====================================================================
    //= OControlTransferData
    //====================================================================
    class OControlTransferData
    {
    private:
        typedef ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Sequence< sal_uInt32 > > ControlPaths;

    private:
        DataFlavorExVector  m_aCurrentFormats;

    protected:
        ListBoxEntrySet     m_aSelectedEntries;
        ControlPaths        m_aControlPaths;
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > >
                            m_aHiddenControlModels;

        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >
                            m_xFormsRoot;       // the root of the forms collection where the entries we represent reside
                                                // this uniquely identifies the page and the document

        SvLBoxEntry*        m_pFocusEntry;

    protected:
        // updates m_aCurrentFormats with all formats we currently could supply
        void    updateFormats( );

    public:
        OControlTransferData( );

        // ctor to construct the data from an arbitrary Transferable (usually clipboard data)
        OControlTransferData(
            const ::com::sun::star::uno::Reference< ::com::sun::star::datatransfer::XTransferable >& _rxTransferable
        );

        inline const DataFlavorExVector&    GetDataFlavorExVector() const;

        void addSelectedEntry( SvLBoxEntry* _pEntry );
        void setFocusEntry( SvLBoxEntry* _pFocusEntry );

        /** notifies the data transfer object that a certain entry has been removed from the owning tree

            In case the removed entry is part of the transfer object's selection, the entry is removed from
            the selection.

            @param  _pEntry
            @return the number of entries remaining in the selection.
        */
        size_t  onEntryRemoved( SvLBoxEntry* _pEntry );

        void setFormsRoot(
            const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >& _rxFormsRoot
            ) { m_xFormsRoot = _rxFormsRoot; }

        void buildPathFormat(SvTreeListBox* pTreeBox, SvLBoxEntry* pRoot);
            // baut aus m_aSelectedEntries m_aControlPaths auf
            // (es wird davon ausgegangen, dass die Eintraege in m_aSelectedEntries sortiert sind in Bezug auf die Nachbar-Beziehung)


        void buildListFromPath(SvTreeListBox* pTreeBox, SvLBoxEntry* pRoot);
            // der umgekehrte Weg : wirft alles aus m_aSelectedEntries weg und baut es mittels m_aControlPaths neu auf

        void addHiddenControlsFormat(const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > > seqInterfaces);
            // fuegt ein SVX_FML_HIDDEN_CONTROLS-Format hinzu und merk sich dafuer die uebergebenen Interfaces
            // (es erfolgt KEINE Ueberpruefung, ob dadurch auch tatsaechlich nur hidden Controls bezeichnet werden, dass muss der
            // Aufrufer sicherstellen)

        SvLBoxEntry*                focused() const { return m_pFocusEntry; }
        const ListBoxEntrySet&      selected() const { return m_aSelectedEntries; }
        ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > >
                                    hiddenControls() const { return m_aHiddenControlModels; }

        ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >
                                getFormsRoot() const { return m_xFormsRoot; }
    };

    //====================================================================
    inline const DataFlavorExVector& OControlTransferData::GetDataFlavorExVector() const
    {
        const_cast< OControlTransferData* >( this )->updateFormats( );
        return m_aCurrentFormats;
    }

    //====================================================================
    //= OControlExchange
    //====================================================================
    class OControlExchange : public OLocalExchange, public OControlTransferData
    {
    public:
        OControlExchange( );

    public:
        static sal_uInt32       getFieldExchangeFormatId( );
        static sal_uInt32       getControlPathFormatId( );
        static sal_uInt32       getHiddenControlModelsFormatId( );

        inline static sal_Bool  hasFieldExchangeFormat( const DataFlavorExVector& _rFormats );
        inline static sal_Bool  hasControlPathFormat( const DataFlavorExVector& _rFormats );
        inline static sal_Bool  hasHiddenControlModelsFormat( const DataFlavorExVector& _rFormats );

    protected:
        virtual sal_Bool    GetData( const ::com::sun::star::datatransfer::DataFlavor& rFlavor );
        virtual void        AddSupportedFormats();
    };

    //====================================================================
    //= OControlExchangeHelper
    //====================================================================
    class OControlExchangeHelper : public OLocalExchangeHelper
    {
    public:
        OControlExchangeHelper(Window* _pDragSource) : OLocalExchangeHelper(_pDragSource) { }

        OControlExchange* operator->() const { return static_cast< OControlExchange* >( m_pTransferable ); }
        OControlExchange& operator*() const { return *static_cast< OControlExchange* >( m_pTransferable ); }

    protected:
        virtual OLocalExchange* createExchange() const;
    };

    //====================================================================
    //====================================================================
    inline sal_Bool OControlExchange::hasFieldExchangeFormat( const DataFlavorExVector& _rFormats )
    {
        return hasFormat( _rFormats, getFieldExchangeFormatId() );
    }

    inline sal_Bool OControlExchange::hasControlPathFormat( const DataFlavorExVector& _rFormats )
    {
        return hasFormat( _rFormats, getControlPathFormatId() );
    }

    inline sal_Bool OControlExchange::hasHiddenControlModelsFormat( const DataFlavorExVector& _rFormats )
    {
        return hasFormat( _rFormats, getHiddenControlModelsFormatId() );
    }

//........................................................................
}   // namespace svxform
//........................................................................

#endif

