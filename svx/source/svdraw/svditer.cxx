/*************************************************************************
 *
 *  $RCSfile: svditer.cxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2004-04-02 14:12:49 $
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

#ifndef _SVDITER_HXX
#include "svditer.hxx"
#endif

#ifndef _SVDPAGE_HXX
#include "svdpage.hxx"
#endif

#ifndef _SVDOGRP_HXX
#include "svdogrp.hxx"
#endif

#ifndef _SVDOBJ_HXX
#include "svdobj.hxx"
#endif

// #99190#
#ifndef _E3D_SCENE3D_HXX
#include "scene3d.hxx"
#endif

SdrObjListIter::SdrObjListIter(const SdrObjList& rObjList, SdrIterMode eMode, BOOL bReverse)
:   maObjList(1024, 64, 64),
    mnIndex(0L),
    mbReverse(bReverse)
{
    ImpProcessObjectList(rObjList, eMode);
    Reset();
}

SdrObjListIter::SdrObjListIter( const SdrObject& rObj, SdrIterMode eMode, BOOL bReverse )
:   maObjList(1024, 64, 64),
    mnIndex(0L),
    mbReverse(bReverse)
{
    if ( rObj.ISA( SdrObjGroup ) )
        ImpProcessObjectList(*rObj.GetSubList(), eMode);
    else
        maObjList.Insert( (void*)&rObj, LIST_APPEND );
    Reset();
}

void SdrObjListIter::ImpProcessObjectList(const SdrObjList& rObjList, SdrIterMode eMode)
{
    for(sal_uInt32 a(0L); a < rObjList.GetObjCount(); a++)
    {
        SdrObject* pObj = rObjList.GetObj(a);
        sal_Bool bIsGroup(pObj->IsGroupObject());

        // #99190# 3D objects are no group objects, IsGroupObject()
        // only tests if pSub is not null ptr :-(
        if(bIsGroup && pObj->ISA(E3dObject) && !pObj->ISA(E3dScene))
            bIsGroup = sal_False;

        if(eMode != IM_DEEPNOGROUPS || !bIsGroup)
            maObjList.Insert(pObj, LIST_APPEND);

        if(bIsGroup && eMode != IM_FLAT)
            ImpProcessObjectList(*pObj->GetSubList(), eMode);
    }
}

