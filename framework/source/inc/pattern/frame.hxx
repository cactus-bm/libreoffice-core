/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: frame.hxx,v $
 *
 *  $Revision: 1.4 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:33:23 $
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

#ifndef __FRAMEWORK_PATTERN_FRAME_HXX_
#define __FRAMEWORK_PATTERN_FRAME_HXX_

//_______________________________________________
// own includes

#ifndef __FRAMEWORK_GENERAL_H_
#include <general.h>
#endif

//_______________________________________________
// interface includes

#ifndef _COM_SUN_STAR_FRAME_XFRAME_HPP_
#include <com/sun/star/frame/XFrame.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XCONTROLLER_HPP_
#include <com/sun/star/frame/XController.hpp>
#endif

#ifndef _COM_SUN_STAR_FRAME_XMODEL_HPP_
#include <com/sun/star/frame/XModel.hpp>
#endif

#ifndef _COM_SUN_STAR_LANG_XCOMPONENT_HPP_
#include <com/sun/star/lang/XComponent.hpp>
#endif

#ifndef _COM_SUN_STAR_UTIL_XCLOSEABLE_HPP_
#include <com/sun/star/util/XCloseable.hpp>
#endif

//_______________________________________________
// other includes

//_______________________________________________
// namespaces

#ifndef css
namespace css = ::com::sun::star;
#endif

namespace framework{
    namespace pattern{
        namespace frame{

//_______________________________________________
// definitions

//-----------------------------------------------
inline css::uno::Reference< css::frame::XModel > extractFrameModel(const css::uno::Reference< css::frame::XFrame >& xFrame)
{
    css::uno::Reference< css::frame::XModel >      xModel;
    css::uno::Reference< css::frame::XController > xController;
    if (xFrame.is())
        xController = xFrame->getController();
    if (xController.is())
        xModel = xController->getModel();
    return xModel;
}

//-----------------------------------------------
/** @short  close (or dispose) the given resource.

    @descr  It try to close the given resource first.
            Delegating of the ownership can be influenced from
            outside. If closing isnt possible (because the
            needed interface isnt available) dispose() is tried instead.
            Al possible exception are handled inside.
            So the user of this method has to look for the return value only.

    @attention  The given resource will not be cleared.
                But later using of it can produce an exception!

    @param  xResource
            the object, which should be closed here.

    @param  bDelegateOwnerShip
            used at the XCloseable->close() method to define
            the right owner in case closing failed.

    @return [bool]
            TRUE if closing failed.
 */
inline sal_Bool closeIt(const css::uno::Reference< css::uno::XInterface >& xResource         ,
                       sal_Bool                                     bDelegateOwnerShip)
{
    css::uno::Reference< css::util::XCloseable > xClose  (xResource, css::uno::UNO_QUERY);
    css::uno::Reference< css::lang::XComponent > xDispose(xResource, css::uno::UNO_QUERY);

    try
    {
        if (xClose.is())
            xClose->close(bDelegateOwnerShip);
        else
        if (xDispose.is())
            xDispose->dispose();
        else
            return sal_False;
    }
    catch(const css::util::CloseVetoException&)
        { return sal_False; }
    catch(const css::lang::DisposedException&)
        {} // disposed is closed is ...
    catch(const css::uno::RuntimeException&)
        { throw; } // shouldnt be suppressed!
    catch(const css::uno::Exception&)
        { return sal_False;  } // ??? We defined to return a boolen value instead of throwing exceptions ...
                               // (OK: RuntimeExceptions shouldnt be catched inside the core ..)

    return sal_True;
}

        } // namespace frame
    } // namespace pattern
} // namespace framework

#endif // __FRAMEWORK_PATTERN_FRAME_HXX_
