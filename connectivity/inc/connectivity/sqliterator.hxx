/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: sqliterator.hxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 05:02:07 $
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
#ifndef _CONNECTIVITY_PARSE_SQLITERATOR_HXX_
#define _CONNECTIVITY_PARSE_SQLITERATOR_HXX_

#ifndef _CONNECTIVITY_SQLNODE_HXX
#include "connectivity/sqlnode.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBCX_XTABLESSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XTablesSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBCX_XCOLUMNSSUPPLIER_HPP_
#include <com/sun/star/sdbcx/XColumnsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_XDATABASEMETADATA_HPP_
#include <com/sun/star/sdbc/XDatabaseMetaData.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_SQLWARNING_HPP_
#include <com/sun/star/sdbc/SQLWarning.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _MAP_
#include <map>
#endif
#ifndef _CONNECTIVITY_COMMONTOOLS_HXX_
#include "connectivity/CommonTools.hxx"
#endif
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _CPPUHELPER_WEAK_HXX_
#include <cppuhelper/weak.hxx>
#endif

namespace connectivity
{

    class OSQLParseNode;
    class OSQLParser;

    enum OSQLStatementType {
        SQL_STATEMENT_UNKNOWN,
        SQL_STATEMENT_SELECT,
        SQL_STATEMENT_INSERT,
        SQL_STATEMENT_UPDATE,
        SQL_STATEMENT_DELETE,
        SQL_STATEMENT_ODBC_CALL,
        SQL_STATEMENT_SELECT_COUNT,
        SQL_STATEMENT_CREATE_TABLE
    };

    //==================================================================
    // SbaParseIteratorErrorInfo wird dem Call von aErrorHdl aus SbaParseIterator "ubergeben
    // nErrorCode enth"alt eine Zusatzinformation "uber den Fehler
    // 1    ->      Tabelle nicht gefunden
    // 2    ->      Spalte nicht gefunden
    //==================================================================
    struct OSQLParseIteratorErrorInfo
    {
        sal_uInt16      nErrorCode;     // 1 == Tabelle nicht gefunden, 2 == Spalte nicht gefunden
        ::rtl::OUString     aExpression;    // der Teil-Ausdruck, der das Problem verursacht hat (kann leer sein)
    };

    #define RET_CONTINUE    1   // Parsevorgang fortsetzen
    #define RET_HANDLED     2   // der Fehler wurde schon behandelt, das Parsen soll (mit Status auf Success) abgebrochen werden
    #define RET_BREAK       3   // Abbrechen, Status-Fehlercode setzen
    struct OSQLParseTreeIteratorImpl;

    class OSQLParseTreeIterator
    {
    public:

    private:
        ::com::sun::star::sdbc::SQLWarning  m_aWarning;         // conatins the error while iterating through the statement
        const OSQLParseNode*                m_pParseTree;       // aktueller ParseTree
        const OSQLParser*                   m_pParser;          // if set used for general error messages from the context
        OSQLStatementType                   m_eStatementType;   // Art des Statements
        OSQLTables                          m_aTables;          // Alle Tabellen die im ParseTree und bei der Connection gefunden wurden
        ::vos::ORef<OSQLColumns>            m_aSelectColumns;   // alle Spalten aus dem Select-Clause
        ::vos::ORef<OSQLColumns>            m_aParameters;      // all parameters
        ::vos::ORef<OSQLColumns>            m_aGroupColumns;    // the group by columns
        ::vos::ORef<OSQLColumns>            m_aOrderColumns;    // the order by columns
        ::vos::ORef<OSQLColumns>            m_aCreateColumns;   // the columns for Create table clause
        ::comphelper::UStringMixEqual       m_aCaseEqual;

        OSQLParseTreeIteratorImpl*          m_pImpl;
        ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData>    m_xDatabaseMetaData;

        void appendWarning(const ::rtl::OUString& _sErrMsg); // append warnings if m_pParser is set
        // F"ugt eine Tabelle in die Map ein
        void                traverseOneTableName(OSQLTables& _rTables,const OSQLParseNode * pTableName, const ::rtl::OUString & rTableRange, const sal_Bool bIsCreateTable);
        void                traverseORCriteria(OSQLParseNode * pSearchCondition);
        void                traverseANDCriteria(OSQLParseNode * pSearchCondition);
        void                traverseOnePredicate(
                                                OSQLParseNode * pColumnRef,
                                                sal_Int32 ePredicateType,
                                                ::rtl::OUString& aValue,
                                                sal_Bool bCompareNull,
                                                OSQLParseNode * pParameter);
        void traverseByColumnNames(const OSQLParseNode* pSelectNode,sal_Bool _bOrder);

        OSQLParseNode *     getTableRef(OSQLTables& _rTables,OSQLParseNode *pTableRef,::rtl::OUString& aTableRange);
        OSQLParseNode *     getQualified_join(OSQLTables& _rTables,OSQLParseNode *pTableRef,::rtl::OUString& aTableRange);
        void                getSelect_statement(OSQLTables& _rTables,OSQLParseNode *pSelect);
        ::rtl::OUString     getUniqueColumnName(const ::rtl::OUString & rColumnName)    const;

        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > findColumn(const OSQLTables& _rTables,const ::rtl::OUString & rColumnName, const ::rtl::OUString & rTableRange);

      protected:
        void setSelectColumnName(::vos::ORef<OSQLColumns>& _rColumns,const ::rtl::OUString & rColumnName,const ::rtl::OUString & rColumnAlias, const ::rtl::OUString & rTableRange,sal_Bool bFkt=sal_False,sal_Int32 _nType = com::sun::star::sdbc::DataType::VARCHAR,sal_Bool bAggFkt=sal_False);
        void appendColumns(::vos::ORef<OSQLColumns>& _rColumns,const ::rtl::OUString& _rTableAlias,const OSQLTable& _rTable);
        // Weitere Member-Variable, die in den "set"-Funktionen zur
        // Verfuegung stehen sollen, koennen in der abgeleiteten Klasse
        // definiert werden und z. B. in deren Konstruktor initialisiert
        // bzw. nach Benutzung der "traverse"-Routinen mit Hilfe weiterer
        // Funktionen abgefragt werden.


      private:
        OSQLParseTreeIterator();        // never implemented

      public:
        OSQLParseTreeIterator(const OSQLParseTreeIterator & rIter);
        OSQLParseTreeIterator(  const ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameAccess>& _xTableSupplier ,
                                const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData>& _xDatabaseMetaData,
                                const OSQLParseNode* pRoot,
                                const OSQLParser* _pParser = NULL);
        ~OSQLParseTreeIterator();

        inline static void * SAL_CALL operator new( size_t nSize ) SAL_THROW( () )
            { return ::rtl_allocateMemory( nSize ); }
        inline static void * SAL_CALL operator new( size_t nSize,void* _pHint ) SAL_THROW( () )
            { return _pHint; }
        inline static void SAL_CALL operator delete( void * pMem ) SAL_THROW( () )
            { ::rtl_freeMemory( pMem ); }
        inline static void SAL_CALL operator delete( void * pMem,void* _pHint ) SAL_THROW( () )
            {  }

        void dispose();
        sal_Bool isCaseSensitive() const { return m_aCaseEqual.isCaseSensitive(); }
        // Der zu analysierende/zu traversierende Parse Tree:
        // bei "Ubergabe von NULL wird der aktuelle Parsetree gel"oscht und der Fehlerstatus gecleared
        void setParseTree(const OSQLParseNode * pNewParseTree);
        void setParser(const OSQLParser* _pParser) { m_pParser = _pParser; }
        const OSQLParseNode * getParseTree() const { return m_pParseTree; };

        // Teilbaueme bei einem select statement
        const OSQLParseNode* getWhereTree() const;
        const OSQLParseNode* getOrderTree() const;
        const OSQLParseNode* getGroupByTree() const;
        const OSQLParseNode* getHavingTree() const;

        const OSQLParseNode* getSimpleWhereTree() const;
        const OSQLParseNode* getSimpleOrderTree() const;
        const OSQLParseNode* getSimpleGroupByTree() const;
        const OSQLParseNode* getSimpleHavingTree() const;

        ::com::sun::star::sdbc::SQLWarning  getWarning() const { return m_aWarning; }
        // Statement-Typ (wird bereits in setParseTree gesetzt):
        OSQLStatementType getStatementType() const { return m_eStatementType; }

        // Die "traverse"-Routinen durchlaufen bestimmte Teile des Parse Tree
        // und rufen fuer die erkannten Konstrukte die virtuellen "set"-Routinen
        // auf und uebergeben diesen die erkannten Informationen als Parameter.
        //
        // Der Parse Tree muss in einer bestimmten Form vorliegen:
        //      SELECT [<tablerange>.]<columnname>, [<tablerange>.]<columnname>, ...
        //        FROM <tablename> [<tablerange>], <tablename> [<tablerange>], ...
        //       WHERE (
        //                  <predicate>
        //              AND <predicate>
        //              ...
        //             )
        //          OR (
        //              ...
        //             )
        //          ...
        //       ORDER BY <columname>, <columnname>, ...
        //
        // (die Klammern sind optional bzw. zusaetzliche Klammern stoeren nicht, aber
        // es duerfen nur mehrere AND-Bedingungen, die ihrerseits mit OR verknuepft sind,
        // auftreten).
        //
        //
        // <predicate> kann sein:
        //          [<tablerange>.]<columnname> <operator> <value>
        //          [<tablerange>.]<columnname> LIKE <value>
        //          [<tablerange>.]<columnname> NOT LIKE <value>
        //          [<tablerange>.]<columnname> IS NULL
        //          [<tablerange>.]<columnname> IS NOT NULL
        //
        // <operator> kann sein:
        //          "="
        //          "<>"
        //          "<"
        //          "<="
        //          ">"
        //          ">="
        //
        // <value> kann auch ein Parameter sein, in diesem Fall enthaelt
        // das Argument "ParameterName" der "set"-Funktionen den Namen des Parameters
        // (ohne fuehrenden Doppelpunkt) bzw. "?" bei unbenannten Parametern.
        //
        // <columnname> in der Select-Klausel kann auch "*" oder "COUNT(*)" sein.
        //
        //
        // Wenn das Statement NICHT diese Form hat, oder wenn eine der "set"-Routinen
        // den IteratorStatus != IsSuccessful() hinterlaesst, wird die weitere Analyse
        // des Parse Tree abgebrochen. Ansonsten liefert "Status().IsSuccessful() == TRUE".

        void traverseTableNames(OSQLTables& _rTables);
        virtual void setTableName(const ::rtl::OUString & rTableName, const ::rtl::OUString & rCatalogName, const ::rtl::OUString& rSchemaName,
                              const ::rtl::OUString & rTableRange);
        // [TableName enthaelt immer einen Namen, TableRange ist, falls angegeben, die "Range"-
        // Variable (eine Art Alias-Name fuer den TableName), falls nicht angegeben, identisch
        // zum TableName. SchemaName ist leer, wenn nicht angegeben.]

        void traverseSelectColumnNames(const OSQLParseNode* pSelectNode);
        // [TableRange kann leer sein, wenn nicht angegeben]

        void traverseCreateColumns(const OSQLParseNode* pSelectNode); //traverse columns for "create table" statement

        void traverseOrderByColumnNames(const OSQLParseNode* pSelectNode);
        virtual void setOrderByColumnName(const ::rtl::OUString & rColumnName, const ::rtl::OUString & rTableRange, sal_Bool bAscending);

        void traverseGroupByColumnNames(const OSQLParseNode* pSelectNode);
        virtual void setGroupByColumnName(const ::rtl::OUString & rColumnName, const ::rtl::OUString & rTableRange);
        // [TableRange kann leer sein, wenn nicht angegeben]

        // Bei Selektionskriterien werden (selbst bei einem einfachen Praedikat)
        // folgende "set"-Funktionen in der angegebenen Reihenfolge aufgerufen:
        //
        //          setORCriteriaPre
        //          |
        //          |   setANDCriteriaPre
        //          |   |   setPredicate
        //          |   |   [weitere setPredicate-Aufrufe] ...
        //          |   setANDCriteriaPost
        //          |
        //          |   ... [weitere setANDCriteriaPre/Post-Paare und darin setPredicate-Aufrufe]
        //          |
        //          setORCriteriaPost
        //
        // Die AND-Verknuepfungen sind also implizit ODER-Verknuepft. setORCriteriaPre und
        // setORCriteriaPost werden jeweils nur ein einziges Mal aufgerufen (sind also
        // eigentlich ziemlich ueberfluessig, da man diese Aktionen auch vor bzw. nach dem
        // traverse-Aufruf erledigen kann)!
        //
        void traverseSelectionCriteria(const OSQLParseNode* pSelectNode);
        virtual void setORCriteriaPre();
        virtual void setORCriteriaPost();
        virtual void setANDCriteriaPre();
        virtual void setANDCriteriaPost();
        virtual void setPredicate(const ::rtl::OUString & rColumnName,
                                  const ::rtl::OUString & rTableRange,
                                  sal_Int32 ePredicateType,
                                  const ::rtl::OUString & rValue,
                                  const ::rtl::OUString & rParameterName);


        // Erweiterung auf UPDATE- und INSERT-Statement ... (nyi):
        void traverseAssignments();
        virtual void setAssign(const ::rtl::OUString & rColumnName,
                               const ::rtl::OUString & rValue, sal_Bool bsetNull,
                               const ::rtl::OUString & rParameterName);

        // Alle "traverse"-Routinen hintereinander aufrufen. Je nach Statement-Typ:
        // Bei UPDATE und INSERT-Statement nur traverseTableNames und traverseAssignments,
        // bei DELETE nur traverseTableNames und bei SELECT-Statement
        // in der Reihenfolge: traverseTableNames, traverseSelectColumnNames,
        // traverseOrderByColumnNames, traverseSelectionCriteria.
        // Bricht bei irgendwelchen Fehlern sofort ab und liefert entsprechenden
        // Status.
        void traverseAll();

        // Die TableRangeMap enth"alt alle Tabellen unter dem zugeh"origen Rangenamen der zuerst gefunden wird
        const OSQLTables& getTables() const { return m_aTables;}

        ::vos::ORef<OSQLColumns> getSelectColumns() const { return m_aSelectColumns;}
        ::vos::ORef<OSQLColumns> getGroupColumns() const { return m_aGroupColumns;}
        ::vos::ORef<OSQLColumns> getOrderColumns() const { return m_aOrderColumns;}
        ::vos::ORef<OSQLColumns> getParameters()    const { return m_aParameters; }
        ::vos::ORef<OSQLColumns> getCreateColumns() const { return m_aCreateColumns;}

        /** return the columname and the table range
            @param  _pColumnRef
                The column ref parse node.
            @param  _rColumnName
                The column name to be set.
            @param  _rTableRange
                The table range to be set.
        */
        void getColumnRange(    const OSQLParseNode* _pColumnRef,
                                ::rtl::OUString &_rColumnName,
                                ::rtl::OUString& _rTableRange) const;

        /** return the alias name of a column
            @param  _pDerivedColumn
                The parse node where SQL_ISRULE(_pDerivedColumn,derived_column) must be true
            @return
                The alias name of the column or an empty string.
        */
        static ::rtl::OUString getColumnAlias(const OSQLParseNode* _pDerivedColumn);
        // gibt den Columnnamen und die Tablerange (falls vorhanden) zur"uck

        /** return the columname and the table range
            @param  _pColumnRef
                The column ref parse node.
            @param  _xMetaData
                The database meta data.
            @param  _rColumnName
                The column name to be set.
            @param  _rTableRange
                The table range to be set.
        */
        static void getColumnRange( const OSQLParseNode* _pColumnRef,
                                    const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XDatabaseMetaData>& _xMetaData,
                                    ::rtl::OUString &_rColumnName,
                                    ::rtl::OUString& _rTableRange);

        // Ermittelt fuer eine Funktion, Spalten den zugehoeren TableRange,
        // wenn nicht eindeutig, dann leer
        sal_Bool getColumnTableRange(const OSQLParseNode* pNode, ::rtl::OUString &rTableRange) const;

        // return true when the tableNode is a rule like catalog_name, schema_name or table_name
        sal_Bool isTableNode(const OSQLParseNode* _pTableNode) const;
    };
}

#endif // _CONNECTIVITY_PARSE_SQLITERATOR_HXX_

