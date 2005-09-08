/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: accessibletexthelper.hxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 02:26:32 $
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

#ifndef COMPHELPER_ACCESSIBLE_TEXT_HELPER_HXX
#define COMPHELPER_ACCESSIBLE_TEXT_HELPER_HXX

#ifndef _COM_SUN_STAR_ACCESSIBILITY_XACCESSIBLETEXT_HPP_
#include <com/sun/star/accessibility/XAccessibleText.hpp>
#endif
#ifndef _COM_SUN_STAR_ACCESSIBILITY_TEXTSEGMENT_HPP_
#include <com/sun/star/accessibility/TextSegment.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_XBREAKITERATOR_HPP_
#include <com/sun/star/i18n/XBreakIterator.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_XCHARACTERCLASSIFICATION_HPP_
#include <com/sun/star/i18n/XCharacterClassification.hpp>
#endif

#ifndef COMPHELPER_ACCESSIBLE_COMPONENT_HELPER_HXX
#include <comphelper/accessiblecomponenthelper.hxx>
#endif
#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef INCLUDED_COMPHELPERDLLAPI_H
#include "comphelper/comphelperdllapi.h"
#endif


//..............................................................................
namespace comphelper
{
//..............................................................................

    //==============================================================================
    // OCommonAccessibleText
    //==============================================================================
    /** base class encapsulating common functionality for the helper classes implementing
        the XAccessibleText
    */
    class COMPHELPER_DLLPUBLIC OCommonAccessibleText
    {
    private:
        ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XBreakIterator >             m_xBreakIter;
        ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XCharacterClassification >   m_xCharClass;

    protected:
        OCommonAccessibleText();
        ~OCommonAccessibleText();

        ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XBreakIterator >             implGetBreakIterator();
        ::com::sun::star::uno::Reference < ::com::sun::star::i18n::XCharacterClassification >   implGetCharacterClassification();
        sal_Bool                                implIsValidBoundary( ::com::sun::star::i18n::Boundary& rBoundary, sal_Int32 nLength );
        virtual sal_Bool                        implIsValidIndex( sal_Int32 nIndex, sal_Int32 nLength );
        virtual sal_Bool                        implIsValidRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex, sal_Int32 nLength );
        virtual ::rtl::OUString                 implGetText() = 0;
        virtual ::com::sun::star::lang::Locale  implGetLocale() = 0;
        virtual void                            implGetSelection( sal_Int32& nStartIndex, sal_Int32& nEndIndex ) = 0;
        virtual void                            implGetGlyphBoundary( ::com::sun::star::i18n::Boundary& rBoundary, sal_Int32 nIndex );
        virtual sal_Bool                        implGetWordBoundary( ::com::sun::star::i18n::Boundary& rBoundary, sal_Int32 nIndex );
        virtual void                            implGetSentenceBoundary( ::com::sun::star::i18n::Boundary& rBoundary, sal_Int32 nIndex );
        virtual void                            implGetParagraphBoundary( ::com::sun::star::i18n::Boundary& rBoundary, sal_Int32 nIndex );
        virtual void                            implGetLineBoundary( ::com::sun::star::i18n::Boundary& rBoundary, sal_Int32 nIndex );

        /** non-virtual versions of the methods
        */
        sal_Unicode SAL_CALL getCharacter( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        sal_Int32 SAL_CALL getCharacterCount() throw (::com::sun::star::uno::RuntimeException);
        ::rtl::OUString SAL_CALL getSelectedText() throw (::com::sun::star::uno::RuntimeException);
        sal_Int32 SAL_CALL getSelectionStart() throw (::com::sun::star::uno::RuntimeException);
        sal_Int32 SAL_CALL getSelectionEnd() throw (::com::sun::star::uno::RuntimeException);
        ::rtl::OUString SAL_CALL getText() throw (::com::sun::star::uno::RuntimeException);
        ::rtl::OUString SAL_CALL getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        ::com::sun::star::accessibility::TextSegment SAL_CALL getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);

    public:

        /** Helper method, that detects the difference between
            two strings and returns the deleted selection and
            the inserted selection if available.

            @returns true if there are differences between the
            two strings and false if both are equal

            @see ::com::sun::star::accessibility::AccessibleEventId
                 ::com::sun::star::accessibility::TextSegment
        */
        static bool implInitTextChangedEvent(
            const rtl::OUString& rOldString,
            const rtl::OUString& rNewString,
            /*out*/ ::com::sun::star::uno::Any& rDeleted,
            /*out*/ ::com::sun::star::uno::Any& rInserted); // throw()
    };


    //==============================================================================
    // OAccessibleTextHelper
    //==============================================================================

    typedef ::cppu::ImplHelper1 <   ::com::sun::star::accessibility::XAccessibleText
                                >   OAccessibleTextHelper_Base;

    /** a helper class for implementing an AccessibleExtendedComponent which at the same time
        supports an XAccessibleText interface
    */
    class COMPHELPER_DLLPUBLIC OAccessibleTextHelper : public OAccessibleExtendedComponentHelper,
                                  public OCommonAccessibleText,
                                  public OAccessibleTextHelper_Base
    {
    protected:
        OAccessibleTextHelper();
        // see the respective base class ctor for an extensive comment on this, please
        OAccessibleTextHelper( IMutex* _pExternalLock );

    public:
        // XInterface
        DECLARE_XINTERFACE( )

        // XTypeProvider
        DECLARE_XTYPEPROVIDER( )

        // XAccessibleText
        virtual sal_Unicode SAL_CALL getCharacter( sal_Int32 nIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        virtual sal_Int32 SAL_CALL getCharacterCount() throw (::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getSelectedText() throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Int32 SAL_CALL getSelectionStart() throw (::com::sun::star::uno::RuntimeException);
        virtual sal_Int32 SAL_CALL getSelectionEnd() throw (::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getText() throw (::com::sun::star::uno::RuntimeException);
        virtual ::rtl::OUString SAL_CALL getTextRange( sal_Int32 nStartIndex, sal_Int32 nEndIndex ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextAtIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBeforeIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::accessibility::TextSegment SAL_CALL getTextBehindIndex( sal_Int32 nIndex, sal_Int16 aTextType ) throw (::com::sun::star::lang::IndexOutOfBoundsException, ::com::sun::star::lang::IllegalArgumentException, ::com::sun::star::uno::RuntimeException);
    };

//..............................................................................
}   // namespace comphelper
//..............................................................................

#endif // COMPHELPER_ACCESSIBLE_TEXT_HELPER_HXX


// -----------------------------------------------------------------------------
//
//  OAccessibleTextHelper is a helper class for implementing the
//  XAccessibleText interface.
//
//  The following methods have a default implementation:
//
//      getCharacter
//      getCharacterCount
//      getSelectedText
//      getSelectionStart
//      getSelectionEnd
//      getText
//      getTextRange
//      getTextAtIndex
//      getTextBeforeIndex
//      getTextBehindIndex
//
//  The following methods must be overriden by derived classes:
//
//      implGetText
//      implGetLocale
//      implGetSelection
//      getCaretPosition
//      setCaretPosition
//      getCharacterAttributes
//      getCharacterBounds
//      getIndexAtPoint
//      setSelection
//      copyText
//
// -----------------------------------------------------------------------------
