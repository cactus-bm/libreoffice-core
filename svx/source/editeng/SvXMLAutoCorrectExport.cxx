/*************************************************************************
 *
 *  $RCSfile: SvXMLAutoCorrectExport.cxx,v $
 *
 *  $Revision: 1.1 $
 *
 *  last change: $Author: mtg $ $Date: 2001-02-09 17:56:59 $
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
 *  Contributor(s): Martin Gallwey (gallwey@sun.com)
 *
 *
 ************************************************************************/
#ifndef _SV_XMLAUTOCORRECTEXPORT_HXX
#include <SvXMLAutoCorrectExport.hxx>
#endif
#define _SVSTDARR_STRINGSISORTDTOR
#define _SVSTDARR_STRINGSDTOR
#include <svtools/svstdarr.hxx>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star;
using namespace ::rtl;
sal_Char __READONLY_DATA sXML_np__block_list[] = "_block-list";

SvXMLAutoCorrectExport::SvXMLAutoCorrectExport( const SvxAutocorrWordList *  pNewAutocorr_List, const rtl::OUString &rFileName,
          com::sun::star::uno::Reference< com::sun::star::xml::sax::XDocumentHandler> &rHandler)
: pAutocorr_List ( pNewAutocorr_List),
  SvXMLExport(rFileName, rHandler)
{
    _GetNamespaceMap().AddAtIndex( XML_NAMESPACE_BLOCKLIST_IDX, sXML_np_block_list,
                                  sXML_n_block_list, XML_NAMESPACE_BLOCKLIST );
}

sal_uInt32 SvXMLAutoCorrectExport::exportDoc(const sal_Char *pClass)
{
    GetDocHandler()->startDocument();

    sal_uInt16 nPos = _GetNamespaceMap().GetIndexByKey( XML_NAMESPACE_BLOCKLIST );
    AddAttribute ( USHRT_MAX, _GetNamespaceMap().GetAttrNameByIndex ( nPos ),
                              _GetNamespaceMap().GetNameByIndex ( nPos ) );
    {
        SvXMLElementExport aRoot (*this, XML_NAMESPACE_BLOCKLIST, sXML_block_list, sal_True, sal_True);
        sal_uInt16 nBlocks= pAutocorr_List->Count();
        for ( sal_uInt16 i = 0; i < nBlocks; i++)
        {
            SvxAutocorrWord* p = pAutocorr_List->GetObject(i);

            AddAttribute( XML_NAMESPACE_BLOCKLIST,
                          sXML_abbreviated_name,
                          OUString(p->GetShort()));
            AddAttribute( XML_NAMESPACE_BLOCKLIST,
                          sXML_name,
                          OUString(p->IsTextOnly() ? p->GetLong() : p->GetShort()));

            SvXMLElementExport aBlock( *this, XML_NAMESPACE_BLOCKLIST, sXML_block, sal_True, sal_True);
        }
    }
    GetDocHandler()->endDocument();
    return 0;
}

SvXMLExceptionListExport::SvXMLExceptionListExport( const SvStringsISortDtor &rNewList, const rtl::OUString &rFileName,
          com::sun::star::uno::Reference< com::sun::star::xml::sax::XDocumentHandler> &rHandler)
: rList ( rNewList),
  SvXMLExport(rFileName, rHandler)
{
    _GetNamespaceMap().AddAtIndex( XML_NAMESPACE_BLOCKLIST_IDX, sXML_np_block_list,
                                  sXML_n_block_list, XML_NAMESPACE_BLOCKLIST );
}

sal_uInt32 SvXMLExceptionListExport::exportDoc(const sal_Char *pClass)
{
    GetDocHandler()->startDocument();

    sal_uInt16 nPos = _GetNamespaceMap().GetIndexByKey( XML_NAMESPACE_BLOCKLIST );
    AddAttribute ( USHRT_MAX, _GetNamespaceMap().GetAttrNameByIndex ( nPos ),
                              _GetNamespaceMap().GetNameByIndex ( nPos ) );
    {
        SvXMLElementExport aRoot (*this, XML_NAMESPACE_BLOCKLIST, sXML_block_list, sal_True, sal_True);
        sal_uInt16 nBlocks= rList.Count();
        for ( sal_uInt16 i = 0; i < nBlocks; i++)
        {
            AddAttribute( XML_NAMESPACE_BLOCKLIST,
                          sXML_abbreviated_name,
                          OUString( *rList[i] ) );
            SvXMLElementExport aBlock( *this, XML_NAMESPACE_BLOCKLIST, sXML_block, sal_True, sal_True);
        }
    }
    GetDocHandler()->endDocument();
    return 0;
}
