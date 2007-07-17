/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: animationtransformnode.cxx,v $
 *
 *  $Revision: 1.7 $
 *
 *  last change: $Author: obo $ $Date: 2007-07-17 14:48:11 $
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
#include "precompiled_slideshow.hxx"

// must be first
#include <canvas/debug.hxx>
#include <canvas/verbosetrace.hxx>
#include <com/sun/star/animations/AnimationTransformType.hpp>

#include "animationtransformnode.hxx"
#include "animationfactory.hxx"
#include "activitiesfactory.hxx"

using namespace com::sun::star;

namespace slideshow {
namespace internal {

void AnimationTransformNode::dispose()
{
    mxTransformNode.clear();
    AnimationBaseNode::dispose();
}

AnimationActivitySharedPtr AnimationTransformNode::createActivity() const
{
    ActivitiesFactory::CommonParameters aParms( fillCommonParameters() );

    const sal_Int16 nTransformType( mxTransformNode->getTransformType() );

    const AttributableShapeSharedPtr& rShape( getShape() );

    switch( nTransformType )
    {
    default:
        ENSURE_AND_THROW(
            false, "AnimationTransformNode::createTransformActivity(): "
            "Unknown transform type" );

    case animations::AnimationTransformType::TRANSLATE:
        // FALLTHROUGH intended
    case animations::AnimationTransformType::SCALE:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createPairPropertyAnimation(
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize(),
                nTransformType ),
            getXAnimateNode() );

    case animations::AnimationTransformType::ROTATE:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createNumberPropertyAnimation(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("Rotate") ),
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize() ),
            getXAnimateNode() );

    case animations::AnimationTransformType::SKEWX:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createNumberPropertyAnimation(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("SkewX") ),
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize() ),
            getXAnimateNode() );

    case animations::AnimationTransformType::SKEWY:
        return ActivitiesFactory::createAnimateActivity(
            aParms,
            AnimationFactory::createNumberPropertyAnimation(
                ::rtl::OUString(
                    RTL_CONSTASCII_USTRINGPARAM("SkewY") ),
                rShape,
                getContext().mpSubsettableShapeManager,
                getSlideSize() ),
            getXAnimateNode() );
    }
}

} // namespace internal
} // namespace slideshow
