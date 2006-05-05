/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: brwbox3.cxx,v $
 *
 *  $Revision: 1.17 $
 *
 *  last change: $Author: rt $ $Date: 2006-05-05 10:24:07 $
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

#ifndef _SVX_BRWBOX_HXX
#include "brwbox.hxx"
#endif
#ifndef _SVTOOLS_ACCESSIBLEBROWSEBOX_HXX
#include "AccessibleBrowseBox.hxx"
#endif
#include "AccessibleBrowseBoxHeaderCell.hxx"
#ifndef _SVTOOLS_ACCESSIBLEBROWSEBOXOBJTYPE_HXX
#include "AccessibleBrowseBoxObjType.hxx"
#endif

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _SV_MULTISEL_HXX
#include <tools/multisel.hxx>
#endif
#ifndef _SFXDATWIN_HXX
#include "datwin.hxx"
#endif
#ifndef _SVTOOLS_BRWIMPL_HXX
#include "brwimpl.hxx"
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLESTATETYPE_HPP_
#include <com/sun/star/accessibility/AccessibleStateType.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_ACCESSIBLEROLE_HPP_
#include <com/sun/star/accessibility/AccessibleRole.hpp>
#endif
#ifndef _SVTOOLS_ACCESSIBILEBROWSEBOXTABLECELL_HXX
#include "AccessibleBrowseBoxTableCell.hxx"
#endif
#ifndef _TOOLKIT_HELPER_VCLUNOHELPER_HXX_
#include <toolkit/helper/vclunohelper.hxx>
#endif

// Accessibility ==============================================================

using ::rtl::OUString;
using ::svt::AccessibleBrowseBox;
using namespace ::com::sun::star::uno;
using ::com::sun::star::accessibility::XAccessible;
using namespace ::com::sun::star::accessibility;

// ============================================================================
namespace svt
{
    using namespace ::com::sun::star::lang;
    using namespace utl;

    Reference< XAccessible > getHeaderCell( BrowseBoxImpl::THeaderCellMap& _raHeaderCells,
                                            sal_Int32 _nPos,
                                            AccessibleBrowseBoxObjType _eType,
                                            const Reference< XAccessible >& _rParent,
                                            BrowseBox& _rBrowseBox)
    {
        Reference< XAccessible > xRet;
        BrowseBoxImpl::THeaderCellMap::iterator aFind = _raHeaderCells.find( _nPos );
        if ( aFind == _raHeaderCells.end() )
        {
            svt::AccessibleBrowseBoxHeaderCell* pNew = new svt::AccessibleBrowseBoxHeaderCell(_nPos,
                                                        _rParent,
                                                        _rBrowseBox,
                                                        NULL,
                                                        _eType);
            pNew->acquire();
            aFind = _raHeaderCells.insert( BrowseBoxImpl::THeaderCellMap::value_type(_nPos,pNew) ).first;
        }
        if ( aFind != _raHeaderCells.end() )
        {
            sal_Int32 nId = aFind->first;
            xRet = aFind->second;
        }
        return xRet;
    }

    // ============================================================================
    // ----------------------------------------------------------------------------
    Reference< XAccessible > BrowseBoxImpl::getAccessibleHeaderBar( AccessibleBrowseBoxObjType _eObjType )
    {
        if ( m_pAccessible && m_pAccessible->isContextAlive() )
            return m_pAccessible->getContext()->getHeaderBar( _eObjType );
        return NULL;
    }

    // ----------------------------------------------------------------------------
    Reference< XAccessible > BrowseBoxImpl::getAccessibleTable( )
    {
        if ( m_pAccessible && m_pAccessible->isContextAlive() )
            return m_pAccessible->getContext()->getTable( );
        return NULL;
    }
}

// ============================================================================

Reference< XAccessible > BrowseBox::CreateAccessible()
{
    Window* pParent = GetAccessibleParentWindow();
    DBG_ASSERT( pParent, "BrowseBox::CreateAccessible - parent not found" );

    Reference< XAccessible > xAccessible = m_pImpl->m_pAccessible;
    if( pParent && !m_pImpl->m_pAccessible)
    {
        Reference< XAccessible > xAccParent = pParent->GetAccessible();
        if( xAccParent.is() )
        {
            m_pImpl->m_pAccessible = new ::svt::AccessibleBrowseBoxAccess( xAccParent, *this );
            xAccessible = m_pImpl->m_pAccessible;
        }
    }
    return xAccessible;
}
// -----------------------------------------------------------------------------

// Children -------------------------------------------------------------------

Reference< XAccessible > BrowseBox::CreateAccessibleCell( sal_Int32 _nRow, sal_uInt16 _nColumnPos )
{
    // BBINDEX_TABLE must be the table
    OSL_ENSURE(m_pImpl->m_pAccessible,"Invalid call: Accessible is null");

    return new svt::AccessibleBrowseBoxTableCell(
        m_pImpl->getAccessibleTable(),
        *this,
        NULL,
        _nRow,
        _nColumnPos
    );
}
// -----------------------------------------------------------------------------

Reference< XAccessible > BrowseBox::CreateAccessibleRowHeader( sal_Int32 _nRow )
{
    return svt::getHeaderCell(
            m_pImpl->m_aRowHeaderCellMap,
            _nRow,
            svt::BBTYPE_ROWHEADERCELL,
            m_pImpl->getAccessibleHeaderBar(svt::BBTYPE_ROWHEADERBAR),
            *this);
}
// -----------------------------------------------------------------------------

Reference< XAccessible > BrowseBox::CreateAccessibleColumnHeader( sal_uInt16 _nColumnPos )
{
    return svt::getHeaderCell(
            m_pImpl->m_aColHeaderCellMap,
            _nColumnPos,
            svt::BBTYPE_COLUMNHEADERCELL,
            m_pImpl->getAccessibleHeaderBar(svt::BBTYPE_COLUMNHEADERBAR),
            *this);
}
// -----------------------------------------------------------------------------

sal_Int32 BrowseBox::GetAccessibleControlCount() const
{
    return 0;
}
// -----------------------------------------------------------------------------

Reference< XAccessible > BrowseBox::CreateAccessibleControl( sal_Int32 nIndex )
{
    DBG_ASSERT( FALSE, "BrowseBox::CreateAccessibleControl: to be overwritten!" );
    return NULL;
}
// -----------------------------------------------------------------------------

// Conversions ----------------------------------------------------------------

sal_Bool BrowseBox::ConvertPointToCellAddress(
        sal_Int32& rnRow, sal_uInt16& rnColumnPos, const Point& rPoint )
{
    //! TODO has to be checked
    rnRow = GetRowAtYPosPixel(rPoint.Y());
    rnColumnPos = GetColumnAtXPosPixel(rPoint.X());
    return rnRow != BROWSER_INVALIDID && rnColumnPos != BROWSER_INVALIDID;
}
// -----------------------------------------------------------------------------

sal_Bool BrowseBox::ConvertPointToRowHeader( sal_Int32& rnRow, const Point& rPoint )
{
    rnRow = GetRowAtYPosPixel(rPoint.Y());
    //  USHORT nColumnId = GetColumnAtXPosPixel(rPoint.X());
    return rnRow != BROWSER_INVALIDID;// && nColumnId == 0;
}
// -----------------------------------------------------------------------------

sal_Bool BrowseBox::ConvertPointToColumnHeader( sal_uInt16& _rnColumnPos, const Point& _rPoint )
{
    _rnColumnPos = GetColumnAtXPosPixel(_rPoint.X());
    return _rnColumnPos != BROWSER_INVALIDID;
}
// -----------------------------------------------------------------------------

sal_Bool BrowseBox::ConvertPointToControlIndex( sal_Int32& _rnIndex, const Point& _rPoint )
{
    //! TODO has to be checked
    sal_Int32 nRow = 0;
    sal_uInt16 nColumn = 0;
    sal_Bool bRet = ConvertPointToCellAddress(nRow,nColumn,_rPoint);
    if ( bRet )
        _rnIndex = nRow * ColCount() + nColumn;

    return bRet;
}
// -----------------------------------------------------------------------------

// Object data and state ------------------------------------------------------

OUString BrowseBox::GetAccessibleName( ::svt::AccessibleBrowseBoxObjType eObjType,sal_Int32 _nPosition ) const
{
    OUString aRetText;
    switch( eObjType )
    {
        case ::svt::BBTYPE_BROWSEBOX:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "BrowseBox" ) );
            break;
        case ::svt::BBTYPE_TABLE:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "Table" ) );
            break;
        case ::svt::BBTYPE_ROWHEADERBAR:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "RowHeaderBar" ) );
            break;
        case ::svt::BBTYPE_COLUMNHEADERBAR:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "ColumnHeaderBar" ) );
            break;
        case ::svt::BBTYPE_TABLECELL:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "TableCell" ) );
#if OSL_DEBUG_LEVEL > 1
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( " [" ) );
            aRetText += OUString::valueOf(sal_Int32(GetCurRow()));
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( "," ) );
            aRetText += OUString::valueOf(sal_Int32(GetCurColumnId()));
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( "]" ) );
#endif
            break;
        case ::svt::BBTYPE_ROWHEADERCELL:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "RowHeaderCell" ) );
#if OSL_DEBUG_LEVEL > 1
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( " [" ) );
            aRetText += OUString::valueOf(sal_Int32(GetCurRow()));
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( "," ) );
            aRetText += OUString::valueOf(sal_Int32(GetCurColumnId()));
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( "]" ) );
#endif
            break;
        case ::svt::BBTYPE_COLUMNHEADERCELL:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "ColumnHeaderCell" ) );
#if OSL_DEBUG_LEVEL > 1
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( " [" ) );
            aRetText += OUString::valueOf(sal_Int32(GetCurRow()));
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( "," ) );
            aRetText += OUString::valueOf(sal_Int32(GetCurColumnId()));
            aRetText += OUString( RTL_CONSTASCII_USTRINGPARAM( "]" ) );
#endif
            break;
        default:
            OSL_ENSURE(0,"BrowseBox::GetAccessibleName: invalid enum!");
    }
    return aRetText;
}
// -----------------------------------------------------------------------------

OUString BrowseBox::GetAccessibleDescription( ::svt::AccessibleBrowseBoxObjType eObjType,sal_Int32 _nPosition ) const
{
    OUString aRetText;
    switch( eObjType )
    {
        case ::svt::BBTYPE_BROWSEBOX:
            aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "BrowseBox description" ) );
            break;
        case ::svt::BBTYPE_TABLE:
            //  aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "TABLE description" ) );
            break;
        case ::svt::BBTYPE_ROWHEADERBAR:
            //  aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "ROWHEADERBAR description" ) );
            break;
        case ::svt::BBTYPE_COLUMNHEADERBAR:
            //  aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "COLUMNHEADERBAR description" ) );
            break;
        case ::svt::BBTYPE_TABLECELL:
            //  aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "TABLECELL description" ) );
            break;
        case ::svt::BBTYPE_ROWHEADERCELL:
            //  aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "ROWHEADERCELL description" ) );
            break;
        case ::svt::BBTYPE_COLUMNHEADERCELL:
            //  aRetText = OUString( RTL_CONSTASCII_USTRINGPARAM( "COLUMNHEADERCELL description" ) );
            break;
    }
    return aRetText;
}
// -----------------------------------------------------------------------------

OUString BrowseBox::GetRowDescription( sal_Int32 nRow ) const
{
    return OUString();
}
// -----------------------------------------------------------------------------

OUString BrowseBox::GetColumnDescription( sal_uInt16 _nColumn ) const
{
    return OUString( GetColumnTitle( GetColumnId( _nColumn ) ) );
}

// -----------------------------------------------------------------------------

void BrowseBox::FillAccessibleStateSet(
        ::utl::AccessibleStateSetHelper& rStateSet,
        ::svt::AccessibleBrowseBoxObjType eObjType ) const
{
    switch( eObjType )
    {
        case ::svt::BBTYPE_BROWSEBOX:
        case ::svt::BBTYPE_TABLE:

            rStateSet.AddState( AccessibleStateType::FOCUSABLE );
            if ( HasFocus() )
                rStateSet.AddState( AccessibleStateType::FOCUSED );
            if ( IsActive() )
                rStateSet.AddState( AccessibleStateType::ACTIVE );
            if ( GetUpdateMode() )
                rStateSet.AddState( AccessibleStateType::EDITABLE );
            if ( IsEnabled() )
            {
                rStateSet.AddState( AccessibleStateType::ENABLED );
                rStateSet.AddState( AccessibleStateType::SENSITIVE );
            }
            if ( IsReallyVisible() )
                rStateSet.AddState( AccessibleStateType::VISIBLE );
            if ( eObjType == ::svt::BBTYPE_TABLE )
                rStateSet.AddState( AccessibleStateType::MANAGES_DESCENDANTS );

            break;
        case ::svt::BBTYPE_ROWHEADERBAR:
            rStateSet.AddState( AccessibleStateType::FOCUSABLE );
            rStateSet.AddState( AccessibleStateType::VISIBLE );
            if ( GetSelectRowCount() )
                rStateSet.AddState( AccessibleStateType::FOCUSED );
            rStateSet.AddState( AccessibleStateType::MANAGES_DESCENDANTS );
            break;
        case ::svt::BBTYPE_COLUMNHEADERBAR:
            rStateSet.AddState( AccessibleStateType::FOCUSABLE );
            rStateSet.AddState( AccessibleStateType::VISIBLE );
            if ( GetSelectColumnCount() )
                rStateSet.AddState( AccessibleStateType::FOCUSED );
            rStateSet.AddState( AccessibleStateType::MANAGES_DESCENDANTS );
            break;
        case ::svt::BBTYPE_TABLECELL:
            {
                sal_Int32 nCurRow = GetCurRow();
                sal_uInt16 nCurColumn = GetCurColumnId();
                if ( IsFieldVisible(nCurRow,nCurColumn) )
                    rStateSet.AddState( AccessibleStateType::VISIBLE );
                if ( !IsFrozen( nCurColumn ) )
                    rStateSet.AddState( AccessibleStateType::FOCUSABLE );
                rStateSet.AddState( AccessibleStateType::TRANSIENT );
            }
            break;
        case ::svt::BBTYPE_ROWHEADERCELL:
        case ::svt::BBTYPE_COLUMNHEADERCELL:
            OSL_ENSURE(0,"Illegal call here!");
            break;
    }
}
// -----------------------------------------------------------------------
void BrowseBox::FillAccessibleStateSetForCell( ::utl::AccessibleStateSetHelper& _rStateSet,
                                               sal_Int32 _nRow, sal_uInt16 _nColumnPos ) const
{
    //! TODO check if the state is valid for table cells
    if ( IsCellVisible( _nRow, _nColumnPos ) )
        _rStateSet.AddState( AccessibleStateType::VISIBLE );
    if ( GetCurrRow() == _nRow && GetCurrColumn() == _nColumnPos )
        _rStateSet.AddState( AccessibleStateType::FOCUSED );
    else // only transient when column is not focused
        _rStateSet.AddState( AccessibleStateType::TRANSIENT );
}
// -----------------------------------------------------------------------------

void BrowseBox::GrabTableFocus()
{
    GrabFocus();
}
// -----------------------------------------------------------------------------
String BrowseBox::GetCellText(long _nRow, USHORT _nColId) const
{
    DBG_ASSERT(0,"This method has to be implemented by the derived classes! BUG!!");
    return String();
}

// -----------------------------------------------------------------------------
void BrowseBox::commitHeaderBarEvent(sal_Int16 nEventId,
        const Any& rNewValue, const Any& rOldValue, sal_Bool _bColumnHeaderBar )
{
    if ( isAccessibleAlive() )
        m_pImpl->m_pAccessible->getContext()->commitHeaderBarEvent( nEventId,
            rNewValue, rOldValue, _bColumnHeaderBar );
}

// -----------------------------------------------------------------------------
void BrowseBox::commitTableEvent( sal_Int16 _nEventId, const Any& _rNewValue, const Any& _rOldValue )
{
    if ( isAccessibleAlive() )
        m_pImpl->m_pAccessible->getContext()->commitTableEvent( _nEventId, _rNewValue, _rOldValue );
}
// -----------------------------------------------------------------------------
void BrowseBox::commitBrowseBoxEvent( sal_Int16 _nEventId, const Any& _rNewValue, const Any& _rOldValue )
{
    if ( isAccessibleAlive() )
        m_pImpl->m_pAccessible->getContext()->commitEvent( _nEventId, _rNewValue, _rOldValue);
}

// -----------------------------------------------------------------------------
::svt::AccessibleBrowseBoxAccess*  BrowseBox::getBrowseBoxAccessible()
{
    return m_pImpl->m_pAccessible;
}

// -----------------------------------------------------------------------------
sal_Bool BrowseBox::isAccessibleAlive( ) const
{
    return ( NULL != m_pImpl->m_pAccessible ) && m_pImpl->m_pAccessible->isContextAlive();
}
// -----------------------------------------------------------------------------
// IAccessibleTableProvider
// -----------------------------------------------------------------------------
sal_Int32 BrowseBox::GetCurrRow() const
{
    return GetCurRow();
}
// -----------------------------------------------------------------------------
sal_uInt16 BrowseBox::GetCurrColumn() const
{
    return GetColumnPos( GetCurColumnId() );
}
// -----------------------------------------------------------------------------
sal_Bool BrowseBox::HasRowHeader() const
{
    return ( GetColumnId( 0 ) == 0 ); // HandleColumn == RowHeader
}
// -----------------------------------------------------------------------------
sal_Bool BrowseBox::IsCellFocusable() const
{
    return sal_True;
}
// -----------------------------------------------------------------------------
sal_Bool BrowseBox::GoToCell( sal_Int32 _nRow, sal_uInt16 _nColumn )
{
    return GoToRowColumnId( _nRow, GetColumnId( _nColumn ) );
}
// -----------------------------------------------------------------------------
void BrowseBox::SelectColumn( sal_uInt16 _nColumn, sal_Bool _bSelect )
{
    SelectColumnPos( _nColumn, _bSelect );
}
// -----------------------------------------------------------------------------
sal_Bool BrowseBox::IsColumnSelected( long _nColumn ) const
{
    return ( pColSel && (0 <= _nColumn) && (_nColumn <= 0xFFF) ) ?
        pColSel->IsSelected( static_cast< sal_uInt16 >( _nColumn ) ) :
        sal_False;
}
// -----------------------------------------------------------------------------
sal_Int32 BrowseBox::GetSelectedRowCount() const
{
    return GetSelectRowCount();
}
// -----------------------------------------------------------------------------
sal_Int32 BrowseBox::GetSelectedColumnCount() const
{
    const MultiSelection* pColSel = GetColumnSelection();
    return pColSel ? pColSel->GetSelectCount() : 0;
}
// -----------------------------------------------------------------------------
void BrowseBox::GetAllSelectedRows( ::com::sun::star::uno::Sequence< sal_Int32 >& _rRows ) const
{
    sal_Int32 nCount = GetSelectRowCount();
    if( nCount )
    {
        _rRows.realloc( nCount );
        _rRows[ 0 ] = const_cast< BrowseBox* >( this )->FirstSelectedRow();
        for( sal_Int32 nIndex = 1; nIndex < nCount; ++nIndex )
            _rRows[ nIndex ] = const_cast< BrowseBox* >( this )->NextSelectedRow();
        DBG_ASSERT( const_cast< BrowseBox* >( this )->NextSelectedRow() == BROWSER_ENDOFSELECTION,
                    "BrowseBox::GetAllSelectedRows - too many selected rows found" );
    }
}
// -----------------------------------------------------------------------------
void BrowseBox::GetAllSelectedColumns( ::com::sun::star::uno::Sequence< sal_Int32 >& _rColumns ) const
{
    const MultiSelection* pColSel = GetColumnSelection();
    sal_Int32 nCount = GetSelectedColumnCount();
    if( pColSel && nCount )
    {
        _rColumns.realloc( nCount );

        sal_Int32 nIndex = 0;
        sal_uInt32 nRangeCount = pColSel->GetRangeCount();
        for( sal_uInt32 nRange = 0; nRange < nRangeCount; ++nRange )
        {
            const Range& rRange = pColSel->GetRange( nRange );
            // loop has to include aRange.Max()
            for( sal_Int32 nCol = rRange.Min(); nCol <= rRange.Max(); ++nCol )
            {
                DBG_ASSERT( nIndex < nCount,
                    "GetAllSelectedColumns - range overflow" );
                _rColumns[ nIndex ] = nCol;
                ++nIndex;
            }
        }
    }
}
// -----------------------------------------------------------------------------
sal_Bool BrowseBox::IsCellVisible( sal_Int32 _nRow, sal_uInt16 _nColumnPos ) const
{
    return IsFieldVisible( _nRow, GetColumnId( _nColumnPos ) );
}
// -----------------------------------------------------------------------------
String BrowseBox::GetAccessibleCellText(long _nRow, USHORT _nColPos) const
{
    return GetCellText( _nRow, GetColumnId( _nColPos ) );
}

// -----------------------------------------------------------------------------
BOOL BrowseBox::GetGlyphBoundRects( const Point& rOrigin, const String& rStr, int nIndex, int nLen, int nBase, MetricVector& rVector )
{
    return Control::GetGlyphBoundRects( rOrigin, rStr, nIndex, nLen, nBase, rVector );
}
// -----------------------------------------------------------------------------
Rectangle BrowseBox::GetWindowExtentsRelative( Window *pRelativeWindow )
{
    return Control::GetWindowExtentsRelative( pRelativeWindow );
}
// -----------------------------------------------------------------------------
void BrowseBox::GrabFocus()
{
    Control::GrabFocus();
}
// -----------------------------------------------------------------------------
Reference< XAccessible > BrowseBox::GetAccessible( BOOL bCreate )
{
    return Control::GetAccessible( bCreate );
}
// -----------------------------------------------------------------------------
Window* BrowseBox::GetAccessibleParentWindow() const
{
    return Control::GetAccessibleParentWindow();
}
// -----------------------------------------------------------------------------
Window* BrowseBox::GetWindow() const
{
    return const_cast< BrowseBox* >( this );
}
