/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: optsave.hxx,v $
 *
 *  $Revision: 1.9 $
 *
 *  last change: $Author: rt $ $Date: 2007-07-05 07:47:05 $
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
#ifndef _SVX_OPTSAVE_HXX
#define _SVX_OPTSAVE_HXX

// include ---------------------------------------------------------------

#ifndef _SV_GROUP_HXX
#include <vcl/group.hxx>
#endif
#ifndef _SV_FIELD_HXX
#include <vcl/field.hxx>
#endif
#ifndef _SV_FIXED_HXX
#include <vcl/fixed.hxx>
#endif
#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif
#ifndef _SFXTABDLG_HXX //autogen
#include <sfx2/tabdlg.hxx>
#endif
#ifndef _COM_SUN_STAR_UNO_REFERENCE_H_
#include <com/sun/star/uno/Reference.h>
#endif
#ifndef _SVX_READONLYIMAGE_HXX
#include <readonlyimage.hxx>
#endif

namespace com { namespace sun { namespace star {
  namespace beans {
    struct PropertyValue;
}}}}

// define ----------------------------------------------------------------

#define SfxSaveTabPage SvxSaveTabPage

// class SvxSaveTabPage --------------------------------------------------

struct SvxSaveTabPage_Impl;
class SvxSaveTabPage : public SfxTabPage
{
private:
    FixedLine           aLoadFL;
    CheckBox            aLoadUserSettingsCB;
    CheckBox            aLoadDocPrinterCB;
    FixedLine           aSaveBox;
    CheckBox            aDocInfoBtn;
    ReadOnlyImage       aBackupFI;
    CheckBox            aBackupBtn;
    CheckBox            aAutoSaveBtn;
    NumericField        aAutoSaveEdit;
    FixedText           aMinuteText;
    CheckBox            aNoPrettyPrintingBtn;
    CheckBox            aWarnAlienFormatBtn;

    FixedLine           aRelBox;
    CheckBox            aRelFsysBtn;
    CheckBox            aRelInetBtn;

    FixedLine           aFilterFL;
    FixedText           aApplicationFT;
    ListBox             aApplicationLB;
    FixedText           aFiltersFT;
    ReadOnlyImage       aFiltersFI;
    ListBox             aFiltersLB;
    FixedText           aWarningFT;

    String              sAlienMsg;

    SvxSaveTabPage_Impl* pImpl;

#ifdef _SVX_OPTSAVE_CXX
    DECL_LINK( AutoClickHdl_Impl, CheckBox * );
    DECL_LINK( FilterHdl_Impl, ListBox * );

    bool    AcceptFilter( USHORT nPos );
    void    DetectHiddenControls();
#endif

public:
    SvxSaveTabPage( Window* pParent, const SfxItemSet& rSet );
    virtual ~SvxSaveTabPage();

    static SfxTabPage*  Create( Window* pParent, const SfxItemSet& rAttrSet );

    virtual BOOL        FillItemSet( SfxItemSet& rSet );
    virtual void        Reset( const SfxItemSet& rSet );
};

#endif  // #ifndef _SVX_OPTSAVE_HXX

