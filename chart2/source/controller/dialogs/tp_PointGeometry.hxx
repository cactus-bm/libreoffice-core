/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: tp_PointGeometry.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: vg $ $Date: 2007-05-22 17:47:22 $
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
#ifndef _CHART2_TP_POINTGEOMETRY_HXX
#define _CHART2_TP_POINTGEOMETRY_HXX

// header for SfxTabPage
#ifndef _SFXTABDLG_HXX
#include <sfx2/tabdlg.hxx>
#endif

//.............................................................................
namespace chart
{
//.............................................................................

class BarGeometryResources;
class SchLayoutTabPage : public SfxTabPage
{
public:
    SchLayoutTabPage(Window* pParent, const SfxItemSet& rInAttrs);
    virtual ~SchLayoutTabPage();

    static SfxTabPage* Create(Window* pParent, const SfxItemSet& rInAttrs);
    virtual BOOL FillItemSet(SfxItemSet& rOutAttrs);
    virtual void Reset(const SfxItemSet& rInAttrs);

private:
    BarGeometryResources*   m_pGeometryResources;
};

//.............................................................................
} //namespace chart
//.............................................................................

#endif

