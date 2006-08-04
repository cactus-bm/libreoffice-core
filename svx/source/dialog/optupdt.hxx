/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: optupdt.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: ihi $ $Date: 2006-08-04 09:48:40 $
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

#ifndef _SVX_OPTUPDT_HXX
#define _SVX_OPTUPDT_HXX

// include ---------------------------------------------------------------

#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEREPLACE_HPP_
#include <com/sun/star/container/XNameReplace.hpp>
#endif

// class SvxPathTabPage --------------------------------------------------

class SvxOnlineUpdateTabPage : public SfxTabPage
{
private:
    FixedLine           m_aOptionsLine;
    CheckBox            m_aAutoCheckCheckBox;
    RadioButton         m_aEveryDayButton;
    RadioButton         m_aEveryWeekButton;
    RadioButton         m_aEveryMonthButton;
    PushButton          m_aCheckNowButton;
    CheckBox            m_aAutoDownloadCheckBox;
    FixedText           m_aDestPathLabel;
    FixedText           m_aDestPath;
    PushButton          m_aChangePathButton;

    DECL_LINK(  FileDialogHdl_Impl, PushButton* ) ;
    DECL_LINK(  CheckNowHdl_Impl, PushButton* ) ;
    DECL_LINK(  AutoCheckHdl_Impl, CheckBox* ) ;
    DECL_LINK(  AutoDownloadHdl_Impl, CheckBox* ) ;

    ::com::sun::star::uno::Reference< ::com::sun::star::container::XNameReplace > m_xUpdateAccess;

public:
    SvxOnlineUpdateTabPage( Window* pParent, const SfxItemSet& rSet );
    ~SvxOnlineUpdateTabPage();

    static SfxTabPage*      Create( Window* pParent, const SfxItemSet& rSet );

    virtual BOOL            FillItemSet( SfxItemSet& rSet );
    virtual void            Reset( const SfxItemSet& rSet );
    virtual void            FillUserData();
};


#endif

