/*************************************************************************
 *
 *  $RCSfile: listeneriter.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: obo $ $Date: 2005-04-13 10:20:35 $
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
#ifndef _SVT_LISTENERITER_HXX
#define _SVT_LISTENERITER_HXX

#ifndef INCLUDED_SVLDLLAPI_H
#include "svtools/svldllapi.h"
#endif

#ifndef _RTTI_HXX //autogen
#include <tools/rtti.hxx>
#endif

class SvtListener;
class SvtListenerBase;
class SvtBroadcaster;

//-------------------------------------------------------------------------

class SVL_DLLPUBLIC SvtListenerIter
{
    friend class SvtListenerBase;

    SvtBroadcaster& rRoot;
    SvtListenerBase *pAkt, *pDelNext;

    // for the update of all iterator's, if a listener is added or removed
    // at the same time.
    static SvtListenerIter *pListenerIters;
    SvtListenerIter *pNxtIter;
    TypeId aSrchId;             // fuer First/Next - suche diesen Type

    SVL_DLLPRIVATE static void RemoveListener( SvtListenerBase& rDel,
                                               SvtListenerBase* pNext );

public:
    SvtListenerIter( SvtBroadcaster& );
    ~SvtListenerIter();

    const SvtBroadcaster& GetBroadcaster() const    { return rRoot; }
          SvtBroadcaster& GetBroadcaster()          { return rRoot; }

    SvtListener* GoNext();          // to the next
    SvtListener* GoPrev();          // to the previous

    SvtListener* GoStart();         // to the start of the list
    SvtListener* GoEnd();           // to the end of the list

    SvtListener* GoRoot();          // to the root
    SvtListener* GetCurr() const;   // returns the current

    int IsChanged() const       { return pDelNext != pAkt; }

    SvtListener* First( TypeId nType );
    SvtListener* Next();
};


#endif

