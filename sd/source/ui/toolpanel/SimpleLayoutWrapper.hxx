/*************************************************************************
 *
 *  $RCSfile: SimpleLayoutWrapper.hxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: kz $ $Date: 2005-03-18 16:55:52 $
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

#ifndef SD_TASKPANE_SIMPLE_LAYOUT_WRAPPER_HXX
#define SD_TASKPANE_SIMPLE_LAYOUT_WRAPPER_HXX

#include "taskpane/ILayoutableWindow.hxx"

#include <memory>

namespace sd { namespace toolpanel {


/** Simple wrapper arround a regular VCL window that implements the
    methods that return the preferred size of the window.  They are
    given once to the constructor and are not modified by the wrapper.
*/
class SimpleLayoutWrapper
    : public virtual ILayoutableWindow
{
public:
    SimpleLayoutWrapper (
        ::std::auto_ptr< ::Window> pWindow,
        int nPreferredWidth,
        int nPreferredHeight,
        bool bIsResizable);
    virtual ~SimpleLayoutWrapper (void);

    virtual Size GetPreferredSize (void);
    virtual int GetPreferredWidth (int nHeight);
    virtual int GetPreferredHeight (int nWidth);
    virtual bool IsResizable (void);
    virtual ::Window* GetWindow (void);

private:
    ::std::auto_ptr< ::Window> mpWindow;
    int mnPreferredWidth;
    int mnPreferredHeight;
    bool mbIsResizable;
};

} } // end of namespace ::sd::toolpanel

#endif
