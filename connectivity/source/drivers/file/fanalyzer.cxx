/*************************************************************************
 *
 *  $RCSfile: fanalyzer.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: hr $ $Date: 2003-03-19 16:38:24 $
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
#ifndef _CONNECTIVITY_FILE_FANALYZER_HXX_
#include "file/fanalyzer.hxx"
#endif
#ifndef _CONNECTIVITY_SQLPARSE_HXX
#include "connectivity/sqlparse.hxx"
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif
#ifndef _COMPHELPER_EXTRACT_HXX_
#include <comphelper/extract.hxx>
#endif
#ifndef _CONNECTIVITY_SQLNODE_HXX
#include "connectivity/sqlnode.hxx"
#endif


using namespace ::connectivity;
using namespace ::connectivity::file;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::container;

typedef ::std::list<OEvaluateSet*>      OEvaluateSetList;
DBG_NAME( file_OSQLAnalyzer )
//------------------------------------------------------------------
OSQLAnalyzer::OSQLAnalyzer()
               : m_aCompiler(this)
               , m_aInterpreter(m_aCompiler)
{
    DBG_CTOR( file_OSQLAnalyzer, NULL );
}

// -----------------------------------------------------------------------------
OSQLAnalyzer::~OSQLAnalyzer()
{
    DBG_DTOR( file_OSQLAnalyzer, NULL );
}

// -----------------------------------------------------------------------------
void OSQLAnalyzer::setIndexes(const Reference< XNameAccess>& _xIndexes)
{
    m_aCompiler.m_xIndexes = _xIndexes;
}
//------------------------------------------------------------------
void OSQLAnalyzer::start(OSQLParseNode* pSQLParseNode)
{
    m_aCompiler.start(pSQLParseNode);
}

//------------------------------------------------------------------
::std::vector<sal_Int32>* OSQLAnalyzer::bindResultRow(OValueRow _pRow)
{
    OCodeList& rCodeList    = m_aCompiler.m_aCodeList;
    // Zaehlen, wieviele Kriterien
    // wenn nur ein Kriterium, und das entsprechende Feld ist indiziert
    // dann wird der Index verwendet

    OEvaluateSetList    aEvaluateSetList;
    OEvaluateSet*       pEvaluateSet = NULL;
    ::std::vector<sal_Int32>*   pKeySet = NULL;

    for (OCodeList::iterator aIter = rCodeList.begin(); aIter != rCodeList.end(); ++aIter)
    {
        OOperandAttr* pAttr = PTR_CAST(OOperandAttr,(*aIter));
        if (pAttr)
        {
            if (pAttr->isIndexed() && !m_aCompiler.hasORCondition())
            {
                OCode* pCode1 = *(aIter + 1);
                OCode* pCode2 = *(aIter + 2);

                if (PTR_CAST(OOperand,pCode1))
                    pEvaluateSet = pAttr->preProcess(PTR_CAST(OBoolOperator,pCode2), PTR_CAST(OOperand,pCode1));
                else
                    pEvaluateSet = pAttr->preProcess(PTR_CAST(OBoolOperator,pCode1));
            }

            if (pEvaluateSet)
            {
                aEvaluateSetList.push_back(pEvaluateSet);
                pEvaluateSet = NULL;
            }
            pAttr->bindValue(_pRow);
        }
    }

    // Keyset erzeugen mit kleinster Liste
    if(!aEvaluateSetList.empty())
    {
        // welche Liste hat den kleinsten count ?
        OEvaluateSetList::iterator i = aEvaluateSetList.begin();
        pEvaluateSet = *(i);
        for(++i; i != aEvaluateSetList.end();++i)
        {
            OEvaluateSet*   pEvaluateSetComp = (*i);
            for(OEvaluateSet::reverse_iterator j = pEvaluateSet->rbegin(); j != pEvaluateSet->rend(); ++j)
            {
                if (pEvaluateSetComp->find(j->second) != pEvaluateSetComp->end())
                    pEvaluateSet->erase(j->second);
            }
        }
        pKeySet = new ::std::vector<sal_Int32>(pEvaluateSet->size());
        sal_Int32 k=0;
        for(OEvaluateSet::iterator j = pEvaluateSet->begin(); j != pEvaluateSet->end(); ++j,++k)
        {
            (*pKeySet)[k] = j->second;
        }

        // alle loeschen
        for(i = aEvaluateSetList.begin(); i != aEvaluateSetList.end();++i)
            delete (*i);
    }

    return pKeySet;
}

//------------------------------------------------------------------
void OSQLAnalyzer::describeParam(::vos::ORef<OSQLColumns> rParameterColumns)
{
    OCodeList& rCodeList    = m_aCompiler.m_aCodeList;
    OCodeStack aCodeStack;

    if (!rCodeList.size())
        return;     // kein Pr�dikat
    if (!rParameterColumns->size())
        return; // keine Parameter

    // Anlegen von Columns, die eine genauere Beschreibung f�r die enthalten
    ::vos::ORef<OSQLColumns> aNewParamColumns = new OSQLColumns(*rParameterColumns);


    // Anlegen einer Testzeile, wird ben�tigt um die Parameter zu beschreiben
    OValueRow aParameterRow  = new OValueVector(rParameterColumns->size());
    bindParameterRow(aParameterRow);

    OValueRow aTestRow = new OValueVector(Reference< XIndexAccess>(m_aCompiler.getOrigColumns(),UNO_QUERY)->getCount());
    delete bindResultRow(aTestRow);                 // Binden der Attribute an die Values

    for(OCodeList::iterator aIter = rCodeList.begin(); aIter != rCodeList.end(); ++aIter)
    {
        OOperand* pOperand = PTR_CAST(OOperand,(*aIter));
        OOperator* pOperator = PTR_CAST(OOperator,(*aIter));
        if (pOperand)
            aCodeStack.push(pOperand);
        else
        {
            if (pOperator->getRequestedOperands() == 2)     // bei zwei Operatoren ist es m�glich
            {                                               // einen Parameter weiter zu spezifizieren
                OOperandParam *pParam  = PTR_CAST(OOperandParam,aCodeStack.top());
                if (pParam)  // Anpassen des ParameterTyps, wenn der linke Operand ein Attribut ist
                {
                    OOperandAttr *pLeft  = PTR_CAST(OOperandAttr,*(rCodeList.end() - 2));
                    if (pLeft)
                    {
                        Reference< XPropertySet> xCol;
                        Reference< XIndexAccess>(m_aCompiler.getOrigColumns(),UNO_QUERY)->getByIndex(pLeft->getRowPos()) >>= xCol;
                        OSL_ENSURE(xCol.is(), "Ung�ltige Struktur");
                        pParam->describe(xCol, aNewParamColumns);
                    }
                }
            }
            pOperator->Exec(aCodeStack);
        }
    }
    OOperand* pOperand = aCodeStack.top();
    aCodeStack.pop();

    OSL_ENSURE(aCodeStack.size() == 0, "StackFehler");
    OSL_ENSURE(pOperand, "StackFehler");
    if (IS_TYPE(OOperandResult,pOperand))
        delete pOperand;
    else
        OSL_ENSURE(0,"Illegal here!");

    rParameterColumns = aNewParamColumns;
    //  m_aCompiler.setParameterColumns(rParameterColumns);
}

// -----------------------------------------------------------------------------
OOperandAttr* OSQLAnalyzer::createOperandAttr(sal_Int32 _nPos,
                                              const Reference< XPropertySet>& _xCol,
                                              const Reference< XNameAccess>& _xIndexes)
{
    return new OOperandAttr(_nPos,_xCol);
}
// -----------------------------------------------------------------------------
BOOL OSQLAnalyzer::hasRestriction() const
{
    return m_aCompiler.hasCode();
}



