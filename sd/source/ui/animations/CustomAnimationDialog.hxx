/*************************************************************************
 *
 *  $RCSfile: CustomAnimationDialog.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: rt $ $Date: 2004-11-26 19:54:26 $
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

#ifndef _SD_CUSTOMANIMATIONDIALOG_HXX
#define _SD_CUSTOMANIMATIONDIALOG_HXX

#ifndef _SD_CUSTOMANIMATIONEFFECT_HXX
#include "CustomAnimationEffect.hxx"
#endif
#ifndef _SD_CUSTOMANIMATIONPRESET_HXX
#include "CustomAnimationPreset.hxx"
#endif

#ifndef _SV_TABDLG_HXX
#include <vcl/tabdlg.hxx>
#endif

#ifndef _SV_LSTBOX_HXX
#include <vcl/lstbox.hxx>
#endif

class TabControl;
class OKButton;
class CancelButton;
class HelpButton;
namespace sd {


// --------------------------------------------------------------------

// property handles
const sal_Int32 nHandleSound = 0;
const sal_Int32 nHandleHasAfterEffect = 1;
const sal_Int32 nHandleIterateType = 2;
const sal_Int32 nHandleIterateInterval = 3;
const sal_Int32 nHandleStart = 4;
const sal_Int32 nHandleBegin = 5;
const sal_Int32 nHandleDuration = 6;
const sal_Int32 nHandleRepeat = 7;
const sal_Int32 nHandleRewind = 8;
const sal_Int32 nHandleEnd = 9;
const sal_Int32 nHandleMasterRel = 10;
const sal_Int32 nHandleDimColor = 11;
const sal_Int32 nHandleMaxParaDepth = 12;
const sal_Int32 nHandlePresetId = 13;
const sal_Int32 nHandleProperty1Type = 14;
const sal_Int32 nHandleProperty1Value = 15;
const sal_Int32 nHandleProperty2Type = 16;
const sal_Int32 nHandleProperty2Value = 17;

const sal_Int32 nHandleAccelerate = 18;
const sal_Int32 nHandleDecelerate = 19;
const sal_Int32 nHandleAutoReverse = 20;
const sal_Int32 nHandleTrigger = 21;

const sal_Int32 nHandleHasText = 22;
const sal_Int32 nHandleTextGrouping = 23;
const sal_Int32 nHandleAnimateForm = 24;
const sal_Int32 nHandleTextGroupingAuto = 25;
const sal_Int32 nHandleTextReverse = 26;

const sal_Int32 nHandleCurrentPage = 27;
const sal_Int32 nHandleSoundURL = 28;
const sal_Int32 nHandleSoundVolumne = 29;
const sal_Int32 nHandleSoundEndAfterSlide = 30;

const sal_Int32 nHandleCommand = 31;

const sal_Int32 nPropertyTypeNone = 0;
const sal_Int32 nPropertyTypeDirection = 1;
const sal_Int32 nPropertyTypeSpokes = 2;
const sal_Int32 nPropertyTypeFirstColor = 3;
const sal_Int32 nPropertyTypeSecondColor = 4;
const sal_Int32 nPropertyTypeZoom = 5;
const sal_Int32 nPropertyTypeFillColor = 6;
const sal_Int32 nPropertyTypeColorStyle = 7;
const sal_Int32 nPropertyTypeFont = 8;
const sal_Int32 nPropertyTypeCharHeight = 9;
const sal_Int32 nPropertyTypeCharColor = 10;
const sal_Int32 nPropertyTypeCharHeightStyle = 11;
const sal_Int32 nPropertyTypeCharDecoration = 12;
const sal_Int32 nPropertyTypeLineColor = 13;
const sal_Int32 nPropertyTypeRotate = 14;
const sal_Int32 nPropertyTypeColor = 15;
const sal_Int32 nPropertyTypeAccelerate = 16;
const sal_Int32 nPropertyTypeDecelerate = 17;
const sal_Int32 nPropertyTypeAutoReverse = 18;
const sal_Int32 nPropertyTypeTransparency = 19;
const sal_Int32 nPropertyTypeFontStyle = 20;
const sal_Int32 nPropertyTypeScale = 21;

// --------------------------------------------------------------------

class PropertySubControl
{
public:
    virtual             ~PropertySubControl();
    virtual             ::com::sun::star::uno::Any getValue() = 0;
    virtual Control*    getControl() = 0;

    static PropertySubControl*
                        create( sal_Int32 nType,
                                ::Window* pParent,
                                const ::com::sun::star::uno::Any& rValue,
                                const rtl::OUString& rPresetId,
                                const Link& rModifyHdl );

};

// --------------------------------------------------------------------

class PropertyControl : public ListBox
{
public:
    PropertyControl( Window* pParent, const ResId& rResId );
    ~PropertyControl();

    void setSubControl( PropertySubControl* pSubControl );
    PropertySubControl* getSubControl() const { return mpSubControl; }

    virtual void Resize();

private:
    PropertySubControl* mpSubControl;
};

// --------------------------------------------------------------------

class CustomAnimationDurationTabPage;
class CustomAnimationEffectTabPage;
class CustomAnimationTextAnimTabPage;
class STLPropertySet;

class CustomAnimationDialog : public TabDialog
{
public:
    CustomAnimationDialog( Window* pParent, STLPropertySet* pSet, USHORT nPage = 0 );
    ~CustomAnimationDialog();

    STLPropertySet* getDefaultSet() { return mpSet; }
    STLPropertySet* getResultSet();

    static STLPropertySet* createDefaultSet();

private:
    STLPropertySet* mpSet;
    STLPropertySet* mpResultSet;

    CustomAnimationEffectPtr mpEffect;
    TabControl* mpTabControl;
    OKButton* mpOKButton;
    CancelButton* mpCancelButton;
    HelpButton* mpHelpButton;

    CustomAnimationDurationTabPage* mpDurationTabPage;
    CustomAnimationEffectTabPage* mpEffectTabPage;
    CustomAnimationTextAnimTabPage* mpTextAnimTabPage;
};

};

#endif // _SD_CUSTOMANIMATIONDIALOG_HXX
