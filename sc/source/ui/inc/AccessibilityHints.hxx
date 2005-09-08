/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: AccessibilityHints.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 21:05:33 $
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

#ifndef SC_ACCESSIBILITYHINTS_HXX
#define SC_ACCESSIBILITYHINTS_HXX

#ifndef SC_VIEWDATA_HXX
#include "viewdata.hxx"
#endif

#ifndef _COM_SUN_STAR_UNO_XINTERFACE_HPP_
#include <com/sun/star/uno/XInterface.hpp>
#endif

#ifndef _SFXSMPLHINT_HXX
#include <svtools/smplhint.hxx>
#endif
#ifndef _SFXHINT_HXX //autogen
#include <svtools/hint.hxx>
#endif

#define SC_HINT_ACC_SIMPLE_START    SFX_HINT_USER00
#define SC_HINT_ACC_TABLECHANGED    SC_HINT_ACC_SIMPLE_START + 1
#define SC_HINT_ACC_CURSORCHANGED   SC_HINT_ACC_SIMPLE_START + 2
#define SC_HINT_ACC_VISAREACHANGED  SC_HINT_ACC_SIMPLE_START + 3
#define SC_HINT_ACC_ENTEREDITMODE   SC_HINT_ACC_SIMPLE_START + 4
#define SC_HINT_ACC_LEAVEEDITMODE   SC_HINT_ACC_SIMPLE_START + 5
#define SC_HINT_ACC_MAKEDRAWLAYER   SC_HINT_ACC_SIMPLE_START + 6
#define SC_HINT_ACC_WINDOWRESIZED   SC_HINT_ACC_SIMPLE_START + 7

class ScAccWinFocusLostHint : public SfxHint
{
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                xOldAccessible;
public:
                TYPEINFO();
                ScAccWinFocusLostHint(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xOld );
                ~ScAccWinFocusLostHint();

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                GetOldAccessible() const { return xOldAccessible; }
};

class ScAccWinFocusGotHint : public SfxHint
{
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                xNewAccessible;
public:
                TYPEINFO();
                ScAccWinFocusGotHint(
                    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xNew );
                ~ScAccWinFocusGotHint();

    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
                GetNewAccessible() const { return xNewAccessible; }
};

class ScAccGridWinFocusLostHint : public ScAccWinFocusLostHint
{
    ScSplitPos  eOldGridWin;
public:
                TYPEINFO();
                ScAccGridWinFocusLostHint( ScSplitPos eOldGridWin,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xOld );
                ~ScAccGridWinFocusLostHint();

    ScSplitPos  GetOldGridWin() const { return eOldGridWin; }
};

class ScAccGridWinFocusGotHint : public ScAccWinFocusGotHint
{
    ScSplitPos  eNewGridWin;
public:
                TYPEINFO();
                ScAccGridWinFocusGotHint( ScSplitPos eNewGridWin,
                    const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >& xNew );
                ~ScAccGridWinFocusGotHint();

    ScSplitPos  GetNewGridWin() const { return eNewGridWin; }
};

#endif
