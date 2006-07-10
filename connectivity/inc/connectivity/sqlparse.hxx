/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sqlparse.hxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: obo $ $Date: 2006-07-10 14:16:59 $
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
#ifndef _CONNECTIVITY_SQLPARSE_HXX
#define _CONNECTIVITY_SQLPARSE_HXX

#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _OSL_MUTEX_HXX_ //autogen wg. Mutex
#include <osl/mutex.hxx>
#endif
#ifndef _CONNECTIVITY_SQLNODE_HXX
#include <connectivity/sqlnode.hxx>
#endif
#ifndef YYBISON
#ifndef FLEX_SCANNER
#ifndef BISON_INCLUDED
#define BISON_INCLUDED
#include <sqlbison.hxx>
#endif
#endif
#endif
#ifndef _COM_SUN_STAR_I18N_XCHARACTERCLASSIFICATION_HPP_
#include <com/sun/star/i18n/XCharacterClassification.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_XLOCALEDATA_HPP_
#include <com/sun/star/i18n/XLocaleData.hpp>
#endif
#ifndef CONNECTIVITY_IPARSECONTEXT_HXX
#include "connectivity/IParseContext.hxx"
#endif

#include <map>

// forward declarations
namespace com
{
    namespace sun
    {
        namespace star
        {
            namespace beans
            {
                class XPropertySet;
            }
            namespace util
            {
                class XNumberFormatter;
            }
            namespace lang
            {
                struct Locale;
            }
        }
    }
}
namespace connectivity
{
    class OSQLScanner;

    //==========================================================================
    //= OParseContext
    //==========================================================================
    class OParseContext : public IParseContext
    {
    public:
        OParseContext();

        virtual ~OParseContext();
        // retrieves language specific error messages
        virtual ::rtl::OUString getErrorMessage(ErrorCode _eCodes) const;

        // retrieves language specific keyword strings (only ASCII allowed)
        virtual ::rtl::OString getIntlKeywordAscii(InternationalKeyCode _eKey) const;

        // finds out, if we have an international keyword (only ASCII allowed)
        virtual InternationalKeyCode getIntlKeyCode(const ::rtl::OString& rToken) const;

        // determines the default international setting
        static const ::com::sun::star::lang::Locale& getDefaultLocale();

        /** set's the default locale which should be used when analyzing strings
            <p>If no locale is set, and any method which needs a locale is called, a default
            (en-US) is used.</p>
            <p>If, while parsing, the locale can be obtained from other sources (such as the number format
            set for a table column), the preferred locale is ignored.</p>
        */
        static void setDefaultLocale( const ::com::sun::star::lang::Locale& _rLocale );

        /** get's a locale instance which should be used when parsing in the context specified by this instance
            <p>if this is not overridden by derived classes, it returns the static default locale.</p>
        */
        virtual ::com::sun::star::lang::Locale getPreferredLocale( ) const;
    };

    //==========================================================================
    //= OSQLParser
    //==========================================================================
    /** Parser for SQL92
    */
    class OSQLParser
    {
        friend class OSQLParseNode;
        friend class OSQLInternalNode;
        friend struct SQLParseNodeParameter;

    private:
        typedef ::std::map< sal_uInt32, OSQLParseNode::Rule >   RuleIDMap;
    //  static parts for parsers
        static sal_uInt32           s_nRuleIDs[OSQLParseNode::rule_count + 1];
        static RuleIDMap            s_aReverseRuleIDLookup;
        static OParseContext        s_aDefaultContext;

    //  parts controled by mutex
        //  static ::osl::Mutex         s_aMutex;
        static OSQLScanner*         s_pScanner;
        static OSQLParseNodes*      s_pGarbageCollector;
        static sal_Int32            s_nRefCount;

    // informations on the current parse action
        const IParseContext*        m_pContext;
        OSQLParseNode*              m_pParseTree;   // result from parsing
        ::com::sun::star::lang::Locale* m_pLocale;      // current locale settings for parsing
        ::rtl::OUString                     m_sFieldName;   // current field name for a predicate
        ::rtl::OUString                     m_sErrorMessage;// current error msg

        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
                                    m_xField;       // current field
        ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter >
                                    m_xFormatter;   // current number formatter
        sal_Int32                   m_nFormatKey;   // numberformat, which should be used
        sal_Int32                   m_nDateFormatKey;
        ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >    m_xServiceFactory;
        ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XCharacterClassification> m_xCharClass;
        static ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XLocaleData>       s_xLocaleData;
        ::com::sun::star::uno::Reference< ::com::sun::star::i18n::XLocaleData>      xDummy; // can be deleted after 627

        // convert a string into double trim it to scale of _nscale and than transform it back to string
        ::rtl::OUString stringToDouble(const ::rtl::OUString& _rValue,sal_Int16 _nScale);
        sal_Int16 buildDate(sal_Int32 _nType,OSQLParseNode*& pAppend,OSQLParseNode* pLiteral,OSQLParseNode*& pCompare);

        static ::osl::Mutex& getMutex();

    public:
        // if NULL, a default context will be used
        // the context must live as long as the parser
        OSQLParser(const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _xServiceFactory,const IParseContext* _pContext = NULL);
        ~OSQLParser();

        // Parsing an SQLStatement
        OSQLParseNode* parseTree(::rtl::OUString& rErrorMessage,
                       const ::rtl::OUString& rStatement,
                       sal_Bool bInternational = sal_False);

        // Check a Predicate
        OSQLParseNode* predicateTree(::rtl::OUString& rErrorMessage, const ::rtl::OUString& rStatement,
                       const ::com::sun::star::uno::Reference< ::com::sun::star::util::XNumberFormatter > & xFormatter,
                       const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > & xField);

        // Access to the context
        const IParseContext& getContext() const {return *m_pContext;}

        // TokenIDToStr: Token-Name zu einer Token-Nr.
        static ::rtl::OString TokenIDToStr(sal_uInt32 nTokenID, const IParseContext* pContext = NULL);

        // StrToTokenID: Token-Nr. zu einem Token-Namen.
        // static sal_uInt32 StrToTokenID(const ::rtl::OString & rName);

        // RuleIDToStr gibt den zu einer RuleID gehoerenden ::rtl::OUString zurueck
        // (Leerstring, falls nicht gefunden)
        static ::rtl::OUString RuleIDToStr(sal_uInt32 nRuleID);

        // StrToRuleID berechnet zu einem ::rtl::OUString die RuleID (d.h. ::com::sun::star::sdbcx::Index in yytname)
        // (0, falls nicht gefunden). Die Suche nach der ID aufgrund eines Strings ist
        // extrem ineffizient (sequentielle Suche nach ::rtl::OUString)!
        static sal_uInt32 StrToRuleID(const ::rtl::OString & rValue);

        static OSQLParseNode::Rule RuleIDToRule( sal_uInt32 _nRule );

        // RuleId mit enum, wesentlich effizienter
        static sal_uInt32 RuleID(OSQLParseNode::Rule eRule);
        // compares the _sFunctionName with all known function names and return the DataType of the return value
        static sal_Int32 getFunctionReturnType(const ::rtl::OUString& _sFunctionName, const IParseContext* pContext = NULL);



        void error(sal_Char *fmt);
        int SQLlex();
#ifdef YYBISON
        void setParseTree(OSQLParseNode * pNewParseTree);

        // Is the parse in a special mode?
        // Predicate chack is used to check a condition for a field
        sal_Bool inPredicateCheck() const {return m_xField.is();}
        const ::rtl::OUString& getFieldName() const {return m_sFieldName;}

        void reduceLiteral(OSQLParseNode*& pLiteral, sal_Bool bAppendBlank);
         // does not change the pLiteral argument
        sal_Int16 buildNode(OSQLParseNode*& pAppend,OSQLParseNode* pLiteral,OSQLParseNode*& pCompare);
        // makes a string out of a number, pLiteral will be deleted
        sal_Int16 buildNode_STR_NUM(OSQLParseNode*& pAppend,OSQLParseNode*& pLiteral,OSQLParseNode*& pCompare);
        sal_Int16 buildNode_Date(const double& fValue, sal_Int32 nType, OSQLParseNode*& pAppend,OSQLParseNode* pLiteral,OSQLParseNode*& pCompare);

        sal_Int16 buildComparsionRule(OSQLParseNode*& pAppend,OSQLParseNode* pLiteral);
        // pCompre will be deleted if it is not used
        sal_Int16 buildComparsionRule(OSQLParseNode*& pAppend,OSQLParseNode* pLiteral,OSQLParseNode*& pCompare);

        sal_Int16 buildLikeRule(OSQLParseNode*& pAppend,OSQLParseNode*& pLiteral,const OSQLParseNode* pEscape);
        sal_Int16 buildStringNodes(OSQLParseNode*& pLiteral);
#else
#endif
    };
}


#endif //_CONNECTIVITY_SQLPARSE_HXX
