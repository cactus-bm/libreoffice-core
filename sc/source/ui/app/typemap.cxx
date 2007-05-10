/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: typemap.cxx,v $
 *
 *  $Revision: 1.16 $
 *
 *  last change: $Author: kz $ $Date: 2007-05-10 16:54:48 $
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

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"


#include <svx/svxids.hrc>
#include "sc.hrc"
#include "scitems.hxx"



#define ITEMID_DBTYPE           0
























#include <sfx2/msg.hxx>
#include <svtools/stritem.hxx>
#include <svtools/slstitm.hxx>
#include <sfx2/objitem.hxx>
#include <svx/fontitem.hxx>
#include <svx/hlnkitem.hxx>
#include <svx/srchitem.hxx>
#include <svx/postattr.hxx>
#include <svx/postitem.hxx>
#include <sfx2/tplpitem.hxx>
#include <svx/zoomitem.hxx>
#include <svx/brshitem.hxx>
#include <svx/wghtitem.hxx>
#include <svx/fhgtitem.hxx>
#include <svx/colritem.hxx>
#include <svx/boxitem.hxx>
#include <svx/bolnitem.hxx>
#include <svtools/ptitem.hxx>
#include <svx/sizeitem.hxx>
#include <svx/algitem.hxx>
#include <svx/clipfmtitem.hxx>
#include <svx/udlnitem.hxx>
#include <svx/xlnstit.hxx>
#include <svx/xlnedit.hxx>
#include <svx/xfillit0.hxx>
#include <svx/xflclit.hxx>
#include <svx/xgrad.hxx>
#include <svx/xflgrit.hxx>
#include <svx/xflhtit.hxx>
#include <svx/xbtmpit.hxx>
#include <svx/xlineit0.hxx>
#include <svx/xlndsit.hxx>
#include <svx/xlnwtit.hxx>
#include <svx/xlnclit.hxx>
#include <svx/xtextit0.hxx>
#include <svx/xftadit.hxx>
#include <svx/xftdiit.hxx>
#include <svx/xftstit.hxx>
#include <svx/xftmrit.hxx>
#include <svx/xftouit.hxx>
#include <svx/xftshit.hxx>
#include <svx/xftshcit.hxx>
#include <svx/xftshxy.hxx>
#include <svx/xftsfit.hxx>
#include <svx/langitem.hxx>
#include <svx/grafctrl.hxx>
#include <svx/shdditem.hxx>
#include <svx/shaditem.hxx>
#include <svx/cntritem.hxx>
#include <svx/crsditem.hxx>
#include <svx/brkitem.hxx>
#include <svx/charreliefitem.hxx>
#include <svx/rotmodit.hxx>
#include <svx/drawitem.hxx>
#include <svtools/ilstitem.hxx>
#include <svtools/globalnameitem.hxx>
#include <svx/chrtitem.hxx>

// #i25616#
#include <svx/sdshitm.hxx>

#include <svtools/aeitem.hxx>
#include <avmedia/mediaitem.hxx>
#include "attrib.hxx"

#define SvxDrawToolItem             SfxAllEnumItem
#define SvxDrawAlignItem            SfxAllEnumItem
#define SvxChooseControlItem        SfxEnumItem
#define avmedia_MediaItem           ::avmedia::MediaItem

#define SFX_TYPEMAP
#include "scslots.hxx"
