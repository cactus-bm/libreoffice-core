/*************************************************************************
 *
 *  $RCSfile: nthesdta.hxx,v $
 *  (modified version of spelldta.hxx)
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: hr $ $Date: 2004-02-04 12:59:57 $
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

#ifndef _LINGUISTIC_THESDTA_HXX_
#define _LINGUISTIC_THESDTA_HXX_


#ifndef _COM_SUN_STAR_LINGUISTIC2_XMEANING_HPP_
#include <com/sun/star/linguistic2/XMeaning.hpp>
#endif

#include <tools/solar.h>

#include <uno/lbnames.h>            // CPPU_CURRENT_LANGUAGE_BINDING_NAME macro, which specify the environment type
#include <cppuhelper/implbase1.hxx> // helper for implementations


namespace linguistic
{

class Meaning :
    public cppu::WeakImplHelper1
    <
        ::com::sun::star::linguistic2::XMeaning
    >
{
    ::com::sun::star::uno::Sequence< ::rtl::OUString >  aSyn;   // list of synonyms, may be empty.
    ::rtl::OUString         aTerm;
    INT16               nLanguage;

#if 0
        // this is for future use by a German thesaurus
        BOOL                            bIsGermanPreReform;
#endif

    // disallow copy-constructor and assignment-operator for now
    Meaning(const Meaning &);
    Meaning & operator = (const Meaning &);

public:
#if 0
    Meaning(const ::rtl::OUString &rTerm, INT16 nLang, const PropertyHelper_Thes &rHelper);
#else
    Meaning(const ::rtl::OUString &rTerm, INT16 nLang);
#endif
    virtual ~Meaning();

    // XMeaning
    virtual ::rtl::OUString SAL_CALL
        getMeaning()
            throw(::com::sun::star::uno::RuntimeException);

    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL
        querySynonyms()
            throw(::com::sun::star::uno::RuntimeException);

    // non-interface specific functions
        void    SetSynonyms(
                    const ::com::sun::star::uno::Sequence< ::rtl::OUString >
                    &rSyn );

        void    SetMeaning(const ::rtl::OUString  &rTerm );


};


///////////////////////////////////////////////////////////////////////////

}   // namespace linguistic

#endif


