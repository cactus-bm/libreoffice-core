/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SimpleLayoutWrapper.cxx,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 06:32:04 $
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

#include "SimpleLayoutWrapper.hxx"

#include <vcl/window.hxx>

namespace sd { namespace toolpanel {

SimpleLayoutWrapper::SimpleLayoutWrapper (
    ::std::auto_ptr< ::Window> pWindow,
    int nPreferredWidth,
    int nPreferredHeight,
    bool bIsResizable)
    : mpWindow (pWindow),
      mnPreferredWidth(nPreferredWidth),
      mnPreferredHeight(nPreferredHeight),
      mbIsResizable (bIsResizable)
{
}




SimpleLayoutWrapper::~SimpleLayoutWrapper (void)
{
}




Size SimpleLayoutWrapper::GetPreferredSize (void)
{
    return Size (mnPreferredWidth, mnPreferredHeight);
}




int SimpleLayoutWrapper::GetPreferredWidth (int nHeight)
{
    return mnPreferredWidth;
}




int SimpleLayoutWrapper::GetPreferredHeight (int nWidth)
{
    return mnPreferredHeight;
}




bool SimpleLayoutWrapper::IsResizable (void)
{
    return mbIsResizable;
}




::Window* SimpleLayoutWrapper::GetWindow (void)
{
    return mpWindow.get();
}


} } // end of namespace ::sd::toolpanel

