/*************************************************************************
 *
 *  $RCSfile: acceleratorconfigurationwriter.cxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-09-20 10:09:42 $
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

#ifndef _FRAMEWORK_XML_ACCELERATORCONFIGURATIONWRITER_HXX_
#include <xml/acceleratorconfigurationwriter.hxx>
#endif

//_______________________________________________
// own includes

#ifndef __FRAMEWORK_XML_ATTRIBUTELIST_HXX
#include <xml/attributelist.hxx>
#endif

#ifndef __FRAMEWORK_ACCELERATORCONST_H_
#include <acceleratorconst.h>
#endif

#ifndef __FRAMEWORK_THREADHELP_READGUARD_HXX_
#include <threadhelp/readguard.hxx>
#endif

//_______________________________________________
// interface includes

#ifndef _COM_SUN_STAR_XML_SAX_XEXTENDEDDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XExtendedDocumentHandler.hpp>
#endif

#ifndef _COM_SUN_STAR_XML_SAX_XATTRIBUTELIST_HPP_
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#endif

#ifndef _COM_SUN_STAR_AWT_KEYMODIFIER_HPP_
#include <com/sun/star/awt/KeyModifier.hpp>
#endif

//_______________________________________________
// other includes

#ifndef _SV_SVAPP_HXX
#include <vcl/svapp.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

//_______________________________________________
// namespace

namespace framework{


//-----------------------------------------------
AcceleratorConfigurationWriter::AcceleratorConfigurationWriter(const AcceleratorCache&                                       rContainer,
                                                               const css::uno::Reference< css::xml::sax::XDocumentHandler >& xConfig   )
    : ThreadHelpBase(&Application::GetSolarMutex())
    , m_rContainer  (rContainer                   )
    , m_xConfig     (xConfig                      )
{
}

//-----------------------------------------------
AcceleratorConfigurationWriter::~AcceleratorConfigurationWriter()
{
}

//-----------------------------------------------
void AcceleratorConfigurationWriter::flush()
{
    // SAFE -> ----------------------------------
    ReadGuard aReadLock(m_aLock);

    css::uno::Reference< css::xml::sax::XDocumentHandler >         xCFG        = m_xConfig;
    css::uno::Reference< css::xml::sax::XExtendedDocumentHandler > xExtendedCFG(m_xConfig, css::uno::UNO_QUERY_THROW);

    aReadLock.unlock();
    // <- SAFE ----------------------------------

    // prepare attribute list
    AttributeListImpl*                                   pAttribs = new AttributeListImpl;
    css::uno::Reference< css::xml::sax::XAttributeList > xAttribs(static_cast< css::xml::sax::XAttributeList* >(pAttribs), css::uno::UNO_QUERY);

    pAttribs->addAttribute(AL_XMLNS_ACCEL, ATTRIBUTE_TYPE_CDATA, NS_XMLNS_ACCEL);
    pAttribs->addAttribute(AL_XMLNS_XLINK, ATTRIBUTE_TYPE_CDATA, NS_XMLNS_XLINK);

    // generate xml
    xCFG->startDocument();

    xExtendedCFG->unknown(DOCTYPE_ACCELERATORS);
    xCFG->ignorableWhitespace(::rtl::OUString());

    xCFG->startElement(AL_ELEMENT_ACCELERATORLIST, xAttribs);
    xCFG->ignorableWhitespace(::rtl::OUString());

    // TODO think about threadsafe using of cache
    AcceleratorCache::TKeyList                 lKeys = m_rContainer.getAllKeys();
    AcceleratorCache::TKeyList::const_iterator pKey  ;
    for (  pKey  = lKeys.begin();
           pKey != lKeys.end()  ;
         ++pKey                 )
    {
        const css::awt::KeyEvent& rKey     = *pKey;
        const ::rtl::OUString&    rCommand = m_rContainer.getCommandByKey(rKey);
        impl_ts_writeKeyCommandPair(rKey, rCommand, xCFG);
    }

    /* TODO write key-command list
    std::vector< SfxAcceleratorConfigItem>::const_iterator p;
    for ( p = m_aWriteAcceleratorList.begin(); p != m_aWriteAcceleratorList.end(); p++ )
        WriteAcceleratorItem( *p );
    */

    xCFG->ignorableWhitespace(::rtl::OUString());
    xCFG->endElement(AL_ELEMENT_ACCELERATORLIST);
    xCFG->ignorableWhitespace(::rtl::OUString());
    xCFG->endDocument();
}

//-----------------------------------------------
void AcceleratorConfigurationWriter::impl_ts_writeKeyCommandPair(const css::awt::KeyEvent&                                     aKey    ,
                                                                 const ::rtl::OUString&                                        sCommand,
                                                                 const css::uno::Reference< css::xml::sax::XDocumentHandler >& xConfig )
{
    AttributeListImpl*                                   pAttribs = new AttributeListImpl;
    css::uno::Reference< css::xml::sax::XAttributeList > xAttribs (static_cast< css::xml::sax::XAttributeList* >(pAttribs) , css::uno::UNO_QUERY_THROW);

    ::rtl::OUString sKey = m_rKeyMapping->mapCodeToIdentifier(aKey.KeyCode);
    // TODO check if key is empty!

    pAttribs->addAttribute(AL_ATTRIBUTE_KEYCODE, ATTRIBUTE_TYPE_CDATA, sKey    );
    pAttribs->addAttribute(AL_ATTRIBUTE_URL    , ATTRIBUTE_TYPE_CDATA, sCommand);

    if ((aKey.Modifiers & css::awt::KeyModifier::SHIFT) == css::awt::KeyModifier::SHIFT)
        pAttribs->addAttribute(AL_ATTRIBUTE_MOD_SHIFT, ATTRIBUTE_TYPE_CDATA, ::rtl::OUString::createFromAscii("true"));

    if ((aKey.Modifiers & css::awt::KeyModifier::MOD1) == css::awt::KeyModifier::MOD1)
        pAttribs->addAttribute(AL_ATTRIBUTE_MOD_MOD1, ATTRIBUTE_TYPE_CDATA, ::rtl::OUString::createFromAscii("true"));

    if ((aKey.Modifiers & css::awt::KeyModifier::MOD2) == css::awt::KeyModifier::MOD2)
        pAttribs->addAttribute(AL_ATTRIBUTE_MOD_MOD2, ATTRIBUTE_TYPE_CDATA, ::rtl::OUString::createFromAscii("true"));

    xConfig->ignorableWhitespace(::rtl::OUString());
    xConfig->startElement(AL_ELEMENT_ITEM, xAttribs);
    xConfig->ignorableWhitespace(::rtl::OUString());
    xConfig->endElement(AL_ELEMENT_ITEM);
    xConfig->ignorableWhitespace(::rtl::OUString());
}

} // namespace framework
