/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: fmtextcontroldialogs.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 22:57:33 $
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

#ifndef SVX_SOURCE_FORM_FMTEXTCONTROLDIALOGS_HXX
#include "fmtextcontroldialogs.hxx"
#endif

#ifndef _SVX_DIALMGR_HXX
#include "dialmgr.hxx"
#endif
#ifndef _SVX_DIALOGS_HRC
#include "dialogs.hrc"
#endif

#ifndef _SVX_CHARDLG_HXX
//#include "chardlg.hxx"
#endif
#ifndef _SVX_PARAGRPH_HXX
//#include "paragrph.hxx"
#endif
#ifndef _EEITEM_HXX
#include "eeitem.hxx"
#endif
#define ITEMID_TABSTOP      EE_PARA_TABS
#ifndef _SVX_TABSTPGE_HXX
//#include "tabstpge.hxx"
#endif

#include "flagsdef.hxx"
#include <svtools/intitem.hxx>

#include <com/sun/star/uno/Sequence.hxx>

#ifndef _SVTOOLS_CJKOPTIONS_HXX
#include <svtools/cjkoptions.hxx>
#endif

//........................................................................
namespace svx
{
//........................................................................

    //====================================================================
    //= TextControlCharAttribDialog
    //====================================================================
    //--------------------------------------------------------------------
    TextControlCharAttribDialog::TextControlCharAttribDialog( Window* pParent, const SfxItemSet& _rCoreSet, const SvxFontListItem& _rFontList )
        :SfxTabDialog( pParent, SVX_RES( RID_SVXDLG_TEXTCONTROL_CHARATTR ), &_rCoreSet )
        ,m_aFontList( _rFontList )
    {
        FreeResource();

        AddTabPage( RID_SVXPAGE_CHAR_NAME);
        AddTabPage( RID_SVXPAGE_CHAR_EFFECTS);
        AddTabPage( RID_SVXPAGE_CHAR_POSITION);
    }

    //--------------------------------------------------------------------
    TextControlCharAttribDialog::~TextControlCharAttribDialog()
    {
    }

    //--------------------------------------------------------------------
    void TextControlCharAttribDialog::PageCreated( USHORT _nId, SfxTabPage& _rPage )
    {
        SfxAllItemSet aSet(*(GetInputSetImpl()->GetPool()));

        switch( _nId )
        {
            case RID_SVXPAGE_CHAR_NAME:
                aSet.Put (m_aFontList);
                _rPage.PageCreated(aSet);
                break;

            case RID_SVXPAGE_CHAR_EFFECTS:
                aSet.Put (SfxUInt16Item(SID_DISABLE_CTL,DISABLE_CASEMAP));
                _rPage.PageCreated(aSet);
                break;

            case RID_SVXPAGE_CHAR_POSITION:
                aSet.Put( SfxUInt32Item(SID_FLAG_TYPE, SVX_PREVIEW_CHARACTER) );
                _rPage.PageCreated(aSet);
                break;
        }
    }

    //====================================================================
    //= TextControlParaAttribDialog
    //====================================================================
    //--------------------------------------------------------------------
    TextControlParaAttribDialog::TextControlParaAttribDialog( Window* _pParent, const SfxItemSet& _rCoreSet )
        :SfxTabDialog( _pParent, SVX_RES( RID_SVXDLG_TEXTCONTROL_PARAATTR ), &_rCoreSet )
    {
        FreeResource();

        AddTabPage( RID_SVXPAGE_STD_PARAGRAPH );
        AddTabPage( RID_SVXPAGE_ALIGN_PARAGRAPH );

        SvtCJKOptions aCJKOptions;
        if( aCJKOptions.IsAsianTypographyEnabled() )
            AddTabPage( RID_SVXPAGE_PARA_ASIAN );
        else
            RemoveTabPage( RID_SVXPAGE_PARA_ASIAN );

        AddTabPage( RID_SVXPAGE_TABULATOR );
    }

    //--------------------------------------------------------------------
    TextControlParaAttribDialog::~TextControlParaAttribDialog()
    {
    }

    //--------------------------------------------------------------------
    void TextControlParaAttribDialog::PageCreated( USHORT _nId, SfxTabPage& _rPage )
    {
    }

//........................................................................
}   // namespace svx
//........................................................................

