/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include <boost/current_function.hpp>
#include <canvas/canvastools.hxx>

#include <comphelper/anytostring.hxx>
#include <cppuhelper/exc_hlp.hxx>

#include <basegfx/point/b2dpoint.hxx>
#include <basegfx/vector/b2dvector.hxx>

#include <com/sun/star/rendering/XCanvas.hpp>
#include <com/sun/star/geometry/IntegerSize2D.hpp>
#include "com/sun/star/uno/Reference.hxx"

#include "pointersymbol.hxx"
#include "eventmultiplexer.hxx"

#include <o3tl/compat_functional.hxx>
#include <algorithm>


using namespace com::sun::star;

namespace slideshow {
namespace internal {

const sal_Int32 LEFT_BORDER_SPACE  = 10;
const sal_Int32 LOWER_BORDER_SPACE = 10;

PointerSymbolSharedPtr PointerSymbol::create( const uno::Reference<rendering::XBitmap>& xBitmap,
                                        ScreenUpdater&                            rScreenUpdater,
                                        EventMultiplexer&                         rEventMultiplexer,
                                        const UnoViewContainer&                   rViewContainer )
{
    PointerSymbolSharedPtr pRet(
        new PointerSymbol( xBitmap,
                           rScreenUpdater,
                           rViewContainer ));

    rEventMultiplexer.addViewHandler( pRet );

    return pRet;
}

PointerSymbol::PointerSymbol( uno::Reference<rendering::XBitmap> const &   xBitmap,
                        ScreenUpdater&                               rScreenUpdater,
                        const UnoViewContainer&                      rViewContainer ) :
    mxBitmap(xBitmap),
    maViews(),
    mrScreenUpdater( rScreenUpdater ),
    mbVisible(false)
{
    std::for_each( rViewContainer.begin(),
                   rViewContainer.end(),
                   boost::bind( &PointerSymbol::viewAdded,
                                this,
                                _1 ));
}

void PointerSymbol::setVisible( const bool bVisible )
{
    if( mbVisible != bVisible )
    {
        mbVisible = bVisible;

        ViewsVecT::const_iterator       aIter( maViews.begin() );
        ViewsVecT::const_iterator const aEnd ( maViews.end() );
        while( aIter != aEnd )
        {
            if( aIter->second )
            {
                if( bVisible )
                    aIter->second->show();
                else
                    aIter->second->hide();
            }

            ++aIter;
        }

        // sprites changed, need a screen update for this frame.
        mrScreenUpdater.requestImmediateUpdate();
    }
}

basegfx::B2DPoint PointerSymbol::calcSpritePos( UnoViewSharedPtr const & rView ) const
{
    const uno::Reference<rendering::XBitmap> xBitmap( rView->getCanvas()->getUNOCanvas(),
                                                      uno::UNO_QUERY_THROW );
    const geometry::IntegerSize2D realSize( xBitmap->getSize() );

    return basegfx::B2DPoint(
        // pos.X pos.Y are given in 0..1, beginning from the upper left corner of the currentSlide.
        std::min<sal_Int32>( 0, LEFT_BORDER_SPACE ),
        std::max<sal_Int32>( 0, realSize.Height * 1 - mxBitmap->getSize().Height
                                                - LOWER_BORDER_SPACE ) );
}

basegfx::B2DPoint PointerSymbol::calcSpritePos(
    UnoViewSharedPtr const & rView, const ::com::sun::star::geometry::RealPoint2D pos) const
{
    const uno::Reference<rendering::XBitmap> xBitmap( rView->getCanvas()->getUNOCanvas(),
                                                      uno::UNO_QUERY_THROW );
    const geometry::IntegerSize2D realSize( xBitmap->getSize() );

    const geometry::IntegerSize2D realTranslationOffset ( rView->getTranslationOffset() );


    basegfx::B2DPoint newPos(
        realTranslationOffset.Width + (realSize.Width - 2 * realTranslationOffset.Width) * pos.X,
        realTranslationOffset.Height + (realSize.Height - 2 * realTranslationOffset.Height) * pos.Y);


    return newPos;
}

void PointerSymbol::viewAdded( const UnoViewSharedPtr& rView )
{
    cppcanvas::CustomSpriteSharedPtr sprite;

    try
    {
        const geometry::IntegerSize2D spriteSize( mxBitmap->getSize() );
        sprite = rView->createSprite( basegfx::B2DVector( spriteSize.Width,
                                                          spriteSize.Height ),
                                      1000.0 ); // sprite should be in front of all
                                                // other sprites
        rendering::ViewState viewState;
        canvas::tools::initViewState( viewState );
        rendering::RenderState renderState;
        canvas::tools::initRenderState( renderState );
        sprite->getContentCanvas()->getUNOCanvas()->drawBitmap(
            mxBitmap, viewState, renderState );

        sprite->setAlpha( 0.9 );
        sprite->movePixel( calcSpritePos( rView ) );
        if( mbVisible )
            sprite->show();
    }
    catch( uno::Exception& )
    {
        OSL_FAIL( OUStringToOString(
                        comphelper::anyToString( cppu::getCaughtException() ),
                        RTL_TEXTENCODING_UTF8 ).getStr() );
    }

    maViews.push_back( ViewsVecT::value_type( rView, sprite ) );
}

void PointerSymbol::viewRemoved( const UnoViewSharedPtr& rView )
{
    maViews.erase(
        std::remove_if(
            maViews.begin(), maViews.end(),
            boost::bind(
                std::equal_to<UnoViewSharedPtr>(),
                rView,
                // select view:
                boost::bind( o3tl::select1st<ViewsVecT::value_type>(), _1 ) ) ),
        maViews.end() );
}

void PointerSymbol::viewChanged( const UnoViewSharedPtr& rView )
{
    // find entry corresponding to modified view
    ViewsVecT::iterator aModifiedEntry(
        std::find_if(
            maViews.begin(),
            maViews.end(),
            boost::bind(
                std::equal_to<UnoViewSharedPtr>(),
                rView,
                // select view:
                boost::bind( o3tl::select1st<ViewsVecT::value_type>(), _1 ))));

    OSL_ASSERT( aModifiedEntry != maViews.end() );
    if( aModifiedEntry == maViews.end() )
        return;

    if( aModifiedEntry->second )
        aModifiedEntry->second->movePixel(
            calcSpritePos(aModifiedEntry->first) );
}

void PointerSymbol::viewsChanged()
{
    // reposition sprites on all views
    ViewsVecT::const_iterator       aIter( maViews.begin() );
    ViewsVecT::const_iterator const aEnd ( maViews.end() );
    while( aIter != aEnd )
    {
        if( aIter->second )
            aIter->second->movePixel(
                calcSpritePos( aIter->first ));
        ++aIter;
    }
}

void PointerSymbol::viewsChanged(const ::com::sun::star::geometry::RealPoint2D pos)
{
    // reposition sprites on all views
    ViewsVecT::const_iterator       aIter( maViews.begin() );
    ViewsVecT::const_iterator const aEnd ( maViews.end() );
    while( aIter != aEnd )
    {
        if( aIter->second ) {
            aIter->second->movePixel(
                calcSpritePos( aIter->first, pos ));
            mrScreenUpdater.notifyUpdate();
            mrScreenUpdater.commitUpdates();
        }
        ++aIter;
    }
}

} // namespace internal
} // namespace slideshow

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
