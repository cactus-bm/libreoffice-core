/*************************************************************************
 *
 *  $RCSfile: fcomp.cxx,v $
 *
 *  $Revision: 1.12 $
 *
 *  last change: $Author: oj $ $Date: 2001-05-07 10:37:52 $
 *
 *  The Contents of this file are made available subject to the terms of
 *  either of the following licenses
 *
 *         - GNU Lesser General Public License Version 2.1
 *         - Sun Industry Standards Source License Version 1.1
 *
 *  Sun Microsystems Inc., October, 2000
 *
 *  GNU Lesser General Public License Version 2.1
 *  =============================================
 *  Copyright 2000 by Sun Microsystems, Inc.
 *  901 San Antonio Road, Palo Alto, CA 94303, USA
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License version 2.1, as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *  MA  02111-1307  USA
 *
 *
 *  Sun Industry Standards Source License Version 1.1
 *  =================================================
 *  The contents of this file are subject to the Sun Industry Standards
 *  Source License Version 1.1 (the "License"); You may not use this file
 *  except in compliance with the License. You may obtain a copy of the
 *  License at http://www.openoffice.org/license.html.
 *
 *  Software provided under this License is provided on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING,
 *  WITHOUT LIMITATION, WARRANTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
 *  MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
 *  See the License for the specific provisions governing your rights and
 *  obligations concerning the Software.
 *
 *  The Initial Developer of the Original Code is: Sun Microsystems, Inc.
 *
 *  Copyright: 2000 by Sun Microsystems, Inc.
 *
 *  All Rights Reserved.
 *
 *  Contributor(s): _______________________________________
 *
 *
 ************************************************************************/

#ifndef _CONNECTIVITY_FILE_FCOMP_HXX_
#include "file/fcomp.hxx"
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _CONNECTIVITY_SQLPARSE_HXX
#include "connectivity/sqlparse.hxx"
#endif
#ifndef _CONNECTIVITY_FILE_FANALYZER_HXX_
#include "file/fanalyzer.hxx"
#endif
#ifndef _COM_SUN_STAR_SDBC_XCOLUMNLOCATE_HPP_
#include <com/sun/star/sdbc/XColumnLocate.hpp>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include "connectivity/dbexception.hxx"
#endif
#define CONNECTIVITY_PROPERTY_NAME_SPACE file
#ifndef _CONNECTIVITY_PROPERTYIDS_HXX_
#include "propertyids.hxx"
#endif

using namespace connectivity;
using namespace connectivity::file;
using namespace com::sun::star::uno;
using namespace com::sun::star::sdbc;
using namespace ::com::sun::star::container;

DBG_NAME(OPredicateCompiler);
//------------------------------------------------------------------
OPredicateCompiler::OPredicateCompiler(OSQLAnalyzer* pAnalyzer)//,OCursor& rCurs)
                     // : m_rCursor(rCurs)
                     : m_pAnalyzer(pAnalyzer)
                     , m_bORCondition(FALSE)
                     , m_nParamCounter(0)
{
    DBG_CTOR(OPredicateCompiler,NULL);
}

//------------------------------------------------------------------
OPredicateCompiler::~OPredicateCompiler()
{
    Clean();
    DBG_DTOR(OPredicateCompiler,NULL);
}
// -----------------------------------------------------------------------------
void OPredicateCompiler::dispose()
{
    Clean();
    m_orgColumns        = NULL;
    m_aParameterColumns = NULL;
    m_xIndexes          = NULL;
}
//------------------------------------------------------------------
//  inline OCursor& OPredicateCompiler::Cursor() const {return m_rCursor;}
//------------------------------------------------------------------
void OPredicateCompiler::Clean()
{
    for(OCodeList::reverse_iterator aIter = m_aCodeList.rbegin(); aIter != m_aCodeList.rend();++aIter)
    {
        delete *aIter;
        m_aCodeList.pop_back();
    }
    //  m_aCodeList.clear();
}

//------------------------------------------------------------------
void OPredicateCompiler::start(OSQLParseNode* pSQLParseNode)
{
    if (!pSQLParseNode)
        return;

    m_nParamCounter = 0;
    // Parse Tree analysieren (je nach Statement-Typ)
    // und Zeiger auf WHERE-Klausel setzen:
    OSQLParseNode * pWhereClause = NULL;
    OSQLParseNode * pOrderbyClause = NULL;

    if (SQL_ISRULE(pSQLParseNode,select_statement))
    {
        DBG_ASSERT(pSQLParseNode->count() >= 4,"OFILECursor: Fehler im Parse Tree");

        OSQLParseNode * pTableExp = pSQLParseNode->getChild(3);
        DBG_ASSERT(pTableExp != NULL,"Fehler im Parse Tree");
        DBG_ASSERT(SQL_ISRULE(pTableExp,table_exp)," Fehler im Parse Tree");
        DBG_ASSERT(pTableExp->count() == 5,"Fehler im Parse Tree");

        pWhereClause    = pTableExp->getChild(1);
        pOrderbyClause  = pTableExp->getChild(4);
    }
    else if (SQL_ISRULE(pSQLParseNode,update_statement_searched))
    {
        DBG_ASSERT(pSQLParseNode->count() == 5,"OFILECursor: Fehler im Parse Tree");
        pWhereClause = pSQLParseNode->getChild(4);
    }
    else if (SQL_ISRULE(pSQLParseNode,update_statement_positioned))
    {
        // nyi
        DBG_ERROR("OPredicateCompiler: update positioned nyi");
    }
    else if (SQL_ISRULE(pSQLParseNode,delete_statement_searched))
    {
        DBG_ASSERT(pSQLParseNode->count() == 4,"Fehler im Parse Tree");
        pWhereClause = pSQLParseNode->getChild(3);
    }
    else if (SQL_ISRULE(pSQLParseNode,delete_statement_positioned))
    {
        // nyi
        DBG_ERROR("OPredicateCompiler: positioned nyi");
    }
    else
            // Anderes Statement. Keine Selektionskriterien.
        return;

    if (SQL_ISRULE(pWhereClause,where_clause))
    {
        // Wenn es aber eine where_clause ist, dann darf sie nicht leer sein:
        DBG_ASSERT(pWhereClause->count() == 2,"OFILECursor: Fehler im Parse Tree");

        OSQLParseNode * pComparisonPredicate = pWhereClause->getChild(1);
        DBG_ASSERT(pComparisonPredicate != NULL,"OFILECursor: Fehler im Parse Tree");

        execute(pComparisonPredicate );
    }
    else
    {
        // Die Where Clause ist meistens optional, d. h. es koennte sich auch
        // um "optional_where_clause" handeln.
        DBG_ASSERT(SQL_ISRULE(pWhereClause,opt_where_clause),"OPredicateCompiler: Fehler im Parse Tree");
    }
}

//------------------------------------------------------------------
OOperand* OPredicateCompiler::execute(OSQLParseNode* pPredicateNode)
{
    OOperand* pOperand = NULL;
    if (pPredicateNode->count() == 3 &&                         // Ausdruck is geklammert
        SQL_ISPUNCTUATION(pPredicateNode->getChild(0),"(") &&
        SQL_ISPUNCTUATION(pPredicateNode->getChild(2),")"))
    {
        execute(pPredicateNode->getChild(1));
    }
    else if ((SQL_ISRULE(pPredicateNode,search_condition) || (SQL_ISRULE(pPredicateNode,boolean_term)))
                            &&          // AND/OR-Verknuepfung:
             pPredicateNode->count() == 3)
    {
        execute(pPredicateNode->getChild(0));                           // Bearbeiten des linken Zweigs
        execute(pPredicateNode->getChild(2));                           // Bearbeiten des rechten Zweigs

        if (SQL_ISTOKEN(pPredicateNode->getChild(1),OR))                // OR-Operator
        {
            m_aCodeList.push_back(new OOp_OR());
            m_bORCondition = sal_True;
        }
        else if (SQL_ISTOKEN(pPredicateNode->getChild(1),AND))      // AND-Operator
            m_aCodeList.push_back(new OOp_AND());
        else
        {
            DBG_ERROR("OPredicateCompiler: Fehler im Parse Tree");
        }
    }
    else if (SQL_ISRULE(pPredicateNode,comparison_predicate))
    {
        execute_COMPARE(pPredicateNode);
    }
    else if (SQL_ISRULE(pPredicateNode,like_predicate))
    {
        execute_LIKE(pPredicateNode);
    }
    else if (SQL_ISRULE(pPredicateNode,test_for_null))
    {
        execute_ISNULL(pPredicateNode);
    }
    else if(SQL_ISRULE(pPredicateNode,num_value_exp))
    {
        execute(pPredicateNode->getChild(0));                           // Bearbeiten des linken Zweigs
        execute(pPredicateNode->getChild(2));                           // Bearbeiten des rechten Zweigs
        if (SQL_ISPUNCTUATION(pPredicateNode->getChild(1),"+"))
        {
            m_aCodeList.push_back(new OOp_ADD());
        }
        else if (SQL_ISPUNCTUATION(pPredicateNode->getChild(1),"-"))
            m_aCodeList.push_back(new OOp_SUB());
        else
        {
            DBG_ERROR("OPredicateCompiler: Fehler im Parse Tree num_value_exp");
        }
    }
    else if(SQL_ISRULE(pPredicateNode,term))
    {
        execute(pPredicateNode->getChild(0));                           // Bearbeiten des linken Zweigs
        execute(pPredicateNode->getChild(2));                           // Bearbeiten des rechten Zweigs
        if (SQL_ISPUNCTUATION(pPredicateNode->getChild(1),"*"))
        {
            m_aCodeList.push_back(new OOp_MUL());
        }
        else if (SQL_ISPUNCTUATION(pPredicateNode->getChild(1),"/"))
            m_aCodeList.push_back(new OOp_DIV());
        else
        {
            DBG_ERROR("OPredicateCompiler: Fehler im Parse Tree num_value_exp");
        }
    }
    else
        pOperand = execute_Operand(pPredicateNode);                     // jetzt werden nur einfache Operanden verarbeitet

    return pOperand;
}

//------------------------------------------------------------------
OOperand* OPredicateCompiler::execute_COMPARE(OSQLParseNode* pPredicateNode)  throw(SQLException, RuntimeException)
{
    DBG_ASSERT(pPredicateNode->count() == 3,"OFILECursor: Fehler im Parse Tree");

    if (!(SQL_ISRULE(pPredicateNode->getChild(0),column_ref) ||
          pPredicateNode->getChild(2)->getNodeType() == SQL_NODE_STRING ||
          pPredicateNode->getChild(2)->getNodeType() == SQL_NODE_INTNUM ||
          pPredicateNode->getChild(2)->getNodeType() == SQL_NODE_APPROXNUM ||
          SQL_ISTOKEN(pPredicateNode->getChild(2),TRUE) ||
          SQL_ISTOKEN(pPredicateNode->getChild(2),FALSE) ||
          SQL_ISRULE(pPredicateNode->getChild(2),parameter) ||
          // odbc date
          (SQL_ISRULE(pPredicateNode->getChild(2),set_fct_spec) && SQL_ISPUNCTUATION(pPredicateNode->getChild(2)->getChild(0),"{"))))
    {
        ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Statement to complex"),NULL);
        return NULL;
    }

    OSQLPredicateType ePredicateType;
    OSQLParseNode *pPrec = pPredicateNode->getChild(1);

    if (pPrec->getNodeType() == SQL_NODE_EQUAL)
        ePredicateType = SQL_PRED_EQUAL;
    else if (pPrec->getNodeType() == SQL_NODE_NOTEQUAL)
        ePredicateType = SQL_PRED_NOTEQUAL;
    else if (pPrec->getNodeType() == SQL_NODE_LESS)
        ePredicateType = SQL_PRED_LESS;
    else if (pPrec->getNodeType() == SQL_NODE_LESSEQ)
        ePredicateType = SQL_PRED_LESSOREQUAL;
    else if (pPrec->getNodeType() == SQL_NODE_GREATEQ)
        ePredicateType = SQL_PRED_GREATEROREQUAL;
    else if (pPrec->getNodeType() == SQL_NODE_GREAT)
        ePredicateType = SQL_PRED_GREATER;

    OOperand* pOb = execute(pPredicateNode->getChild(0));
    OOperand* pOperand  = execute(pPredicateNode->getChild(2));

    OBoolOperator* pOperator = new OOp_COMPARE(ePredicateType);
    //pOb->PreProcess(pOperator, pOperand);

    m_aCodeList.push_back(pOperator);

    // wenn es sich um eine Vergleichsoperation auf datum/Zeit handelt, dann
    // erfolgt jetzt bereits eine Umwandlung fuer die Konstante
//  if (pOb)
//  {
//      switch (pPredicateNode->getChild(2)->getNodeType())
//      {
//          case SQL_NODE_STRING:
//          {
//              OOperandConst* pConst = PTR_CAST(OOperandConst,m_aCodeList[m_aCodeList.size() - 2]);
//              switch (pOb->getDBType())
//              {
//                  case DataType::DECIMAL:
//                  case DataType::NUMERIC:
//                  case DataType::REAL:
//                  case DataType::DOUBLE:
//                  case DataType::TIMESTAMP:
//                  case DataType::DATE:
//                  case DataType::TIME:
//                  {
//                      try
//                      {
//                          pConst->setValue(makeAny(pConst->getValue().getDouble()));
//                      }
//                      catch( Exception&)
//                      {
//                          ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Datatype mismatch"),NULL);
//                      }
//                  }   break;
//                  case DataType::TINYINT:
//                  case DataType::SMALLINT:
//                  case DataType::INTEGER:
//                  case DataType::BIT:
//                      ;
//                      ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Datatype mismatch"),NULL);
//              }
//          }
//      }
//  }
    return NULL;
}

//------------------------------------------------------------------
OOperand* OPredicateCompiler::execute_LIKE(OSQLParseNode* pPredicateNode) throw(SQLException, RuntimeException)
{
    DBG_ASSERT(pPredicateNode->count() >= 4,"OFILECursor: Fehler im Parse Tree");

    if (!SQL_ISRULE(pPredicateNode->getChild(0),column_ref))
    {
        ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Invalid Statement"),NULL);
        return NULL;
    }

    OSQLPredicateType ePredicateType;
    OSQLParseNode *pAtom;
    OSQLParseNode *pOptEscape;
    sal_Unicode cEscape = L'\0';

    if (pPredicateNode->count() == 5)
        ePredicateType = SQL_PRED_NOTLIKE;
    else
        ePredicateType = SQL_PRED_LIKE;

    pAtom       = pPredicateNode->getChild(pPredicateNode->count()-2);
    pOptEscape  = pPredicateNode->getChild(pPredicateNode->count()-1);

    if (!(pAtom->getNodeType() == SQL_NODE_STRING || SQL_ISRULE(pAtom,parameter)))
    {
        ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Invalid Statement"),NULL);
        return NULL;
    }
    if (pOptEscape->count() != 0)
    {
        if (pOptEscape->count() != 2)
        {
            ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Invalid Statement"),NULL);
            return NULL;
        }
        OSQLParseNode *pEscNode = pOptEscape->getChild(1);
        if (pEscNode->getNodeType() != SQL_NODE_STRING)
        {
            ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Invalid Statement"),NULL);
            return NULL;
        }
        else
            cEscape = pEscNode->getTokenValue().toChar();
    }

    OOperand* pOb = execute(pPredicateNode->getChild(0));
    OOperand* pOperand  = execute(pAtom);
    OBoolOperator* pOperator = (ePredicateType == SQL_PRED_LIKE)
                                                    ? new OOp_LIKE(cEscape)
                                                    : new OOp_NOTLIKE(cEscape);
    m_aCodeList.push_back(pOperator);
    return NULL;
}

//------------------------------------------------------------------
OOperand* OPredicateCompiler::execute_ISNULL(OSQLParseNode* pPredicateNode) throw(SQLException, RuntimeException)
{
    if (!SQL_ISRULE(pPredicateNode->getChild(0),column_ref))
    {
        ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Invalid Statement"),NULL);
        return NULL;
    }

    DBG_ASSERT(pPredicateNode->count() >= 3,"OFILECursor: Fehler im Parse Tree");
    DBG_ASSERT(SQL_ISTOKEN(pPredicateNode->getChild(1),IS),"OFILECursor: Fehler im Parse Tree")

    OSQLPredicateType ePredicateType;
    if (SQL_ISTOKEN(pPredicateNode->getChild(2),NOT))
        ePredicateType = SQL_PRED_ISNOTNULL;
    else
        ePredicateType = SQL_PRED_ISNULL;

    OOperand* pOb = execute(pPredicateNode->getChild(0));
    OBoolOperator* pOperator = (ePredicateType == SQL_PRED_ISNULL) ?
                                new OOp_ISNULL() : new OOp_ISNOTNULL();

    //pOb->PreProcess(pOperator);


    m_aCodeList.push_back(pOperator);
    return NULL;
}
//------------------------------------------------------------------
OOperand* OPredicateCompiler::execute_Operand(OSQLParseNode* pPredicateNode) throw(SQLException, RuntimeException)
{
    OOperand* pOperand = NULL;

    if (SQL_ISRULE(pPredicateNode,column_ref))
    {
        ::rtl::OUString aColumnName;
        if (pPredicateNode->count() == 1)
        {
            aColumnName = pPredicateNode->getChild(0)->getTokenValue();
        }
        else if (pPredicateNode->count() == 3)
        {
            ::rtl::OUString aTableName = pPredicateNode->getChild(0)->getTokenValue();
            if(SQL_ISRULE(pPredicateNode->getChild(2),column_val))
                aColumnName = pPredicateNode->getChild(2)->getChild(0)->getTokenValue();
            else
                aColumnName = pPredicateNode->getChild(2)->getTokenValue();
        }

        if(!m_orgColumns->hasByName(aColumnName))
        {
            ::rtl::OUString sMsg = ::rtl::OUString::createFromAscii("Column not found: ");
            sMsg += aColumnName;
            throw SQLException(sMsg,NULL,SQLSTATE_GENERAL,1000,Any());
        }
        ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet> xCol;
        try
        {
            if (m_orgColumns->getByName(aColumnName) >>= xCol)
            {
                pOperand = m_pAnalyzer->createOperandAttr(Reference< XColumnLocate>(m_orgColumns,UNO_QUERY)->findColumn(aColumnName),xCol,m_xIndexes);
            }
            else
            {// Column existiert nicht im Resultset
                ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Invalid Statement"),NULL);
            }
        }
        catch(Exception &)
        {
            OSL_ENSURE(0,"OPredicateCompiler::execute_Operand Exception");
        }
    }
    else if (SQL_ISRULE(pPredicateNode,parameter))
    {
        pOperand = new OOperandParam(pPredicateNode, ++m_nParamCounter);
    }
    else if (pPredicateNode->getNodeType() == SQL_NODE_STRING ||
             pPredicateNode->getNodeType() == SQL_NODE_INTNUM ||
             pPredicateNode->getNodeType() == SQL_NODE_APPROXNUM ||
             pPredicateNode->getNodeType() == SQL_NODE_NAME ||
             SQL_ISTOKEN(pPredicateNode,TRUE) ||
             SQL_ISTOKEN(pPredicateNode,FALSE) ||
             SQL_ISRULE(pPredicateNode,parameter))
    {
        pOperand = new OOperandConst(*pPredicateNode, pPredicateNode->getTokenValue());
    }
    else if((pPredicateNode->count() == 2) &&
            (SQL_ISPUNCTUATION(pPredicateNode->getChild(0),"+") || SQL_ISPUNCTUATION(pPredicateNode->getChild(0),"-")) &&
            pPredicateNode->getChild(1)->getNodeType() == SQL_NODE_INTNUM)
    { // falls -1 bzw. +1 vorhanden ist
        ::rtl::OUString aValue(pPredicateNode->getChild(0)->getTokenValue());
        aValue += pPredicateNode->getChild(1)->getTokenValue();
        pOperand = new OOperandConst(*pPredicateNode->getChild(1), aValue);
    }
    else if(SQL_ISRULE(pPredicateNode,set_fct_spec) && SQL_ISPUNCTUATION(pPredicateNode->getChild(0),"{"))
    {
        const OSQLParseNode* pODBCNode = pPredicateNode->getChild(1);
        const OSQLParseNode* pODBCNodeChild = pODBCNode->getChild(0);

        // Odbc Date or time
        if (pODBCNodeChild->getNodeType() == SQL_NODE_KEYWORD && (
            SQL_ISTOKEN(pODBCNodeChild,D) ||
            SQL_ISTOKEN(pODBCNodeChild,T) ||
            SQL_ISTOKEN(pODBCNodeChild,TS) ))
        {
            pOperand = new OOperandConst(*pODBCNode->getChild(1), pODBCNode->getChild(1)->getTokenValue());

//          // setting the Date
//          try
//          {
//              pOperand->setValue(makeAny(pOperand->getValue().getDouble()));
//          }
//          catch( Exception & )
//          {
//              OSL_ENSURE(0,"OPredicateCompiler::execute_Operand Exception");
//              ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Datatype mismatch"),NULL);
//          }
        }
        else
            ;
            ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Statement to complex"),NULL);
    }
    else
    {
        ::dbtools::throwGenericSQLException(::rtl::OUString::createFromAscii("Statement to complex"),NULL);
    }
    if (pOperand)
        m_aCodeList.push_back(pOperand);
    return pOperand;
}

////////////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------
OPredicateInterpreter::~OPredicateInterpreter()
{
    while(!m_aStack.empty())
    {
        delete m_aStack.top();
        m_aStack.pop();
    }
    //  m_aStack.clear();
}

//------------------------------------------------------------------
sal_Bool OPredicateInterpreter::evaluate(OCodeList& rCodeList)
{
    static sal_Bool bResult;

    OCodeList::iterator aIter = rCodeList.begin();
    if (!(*aIter))
        return sal_True;        // kein Pr�dikat

    for(;aIter != rCodeList.end();++aIter)
    {
        OOperand* pOperand = PTR_CAST(OOperand,(*aIter));
        if (pOperand)
            m_aStack.push(pOperand);
        else
            ((OOperator *)(*aIter))->Exec(m_aStack);
    }

    OOperand* pOperand = m_aStack.top();
    m_aStack.pop();

    DBG_ASSERT(m_aStack.size() == 0, "StackFehler");
    DBG_ASSERT(pOperand, "StackFehler");

    bResult = pOperand->isValid();
    if (IS_TYPE(OOperandResult,pOperand))
        delete pOperand;
    return bResult;
}
// -----------------------------------------------------------------------------



