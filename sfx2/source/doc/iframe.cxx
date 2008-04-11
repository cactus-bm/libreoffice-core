/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: iframe.cxx,v $
 * $Revision: 1.11 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/

// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sfx2.hxx"

#include "iframe.hxx"
#include <sfx2/sfxdlg.hxx>
#include <sfx2/sfxsids.hrc>
#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>

#include <tools/urlobj.hxx>
#include <tools/debug.hxx>
#include <rtl/ustring.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <svtools/miscopt.hxx>
#include <vcl/window.hxx>

using namespace ::com::sun::star;

namespace sfx2
{

class IFrameWindow_Impl : public Window
{
    uno::Reference < frame::XFrame > mxFrame;
    sal_Bool                bActive;
    sal_Bool                bBorder;

public:
    IFrameWindow_Impl( Window *pParent,
                       sal_Bool bHasBorder,
                       WinBits nWinBits = 0 );

public:
    void            SetBorder( sal_Bool bNewBorder = sal_True );
    sal_Bool        HasBorder() const { return bBorder; }
};

IFrameWindow_Impl::IFrameWindow_Impl( Window *pParent, sal_Bool bHasBorder, WinBits nWinBits )
    : Window( pParent, nWinBits | WB_CLIPCHILDREN | WB_NODIALOGCONTROL | WB_DOCKBORDER )
    , bActive(sal_False)
    , bBorder(bHasBorder)
{
    if ( !bHasBorder )
        SetBorderStyle( WINDOW_BORDER_NOBORDER );
    else
        SetBorderStyle( WINDOW_BORDER_NORMAL );
    //SetActivateMode( ACTIVATE_MODE_GRABFOCUS );
}

void IFrameWindow_Impl::SetBorder( sal_Bool bNewBorder )
{
    if ( bBorder != bNewBorder )
    {
        Size aSize = GetSizePixel();
        bBorder = bNewBorder;
        if ( bBorder )
            SetBorderStyle( WINDOW_BORDER_NORMAL );
        else
            SetBorderStyle( WINDOW_BORDER_NOBORDER );
        if ( GetSizePixel() != aSize )
            SetSizePixel( aSize );
    }
}

#define PROPERTY_UNBOUND 0

SfxItemPropertyMap aIFramePropertyMap_Impl[] =
{
    { "FrameIsAutoBorder",    17, 1, &::getBooleanCppuType(), PROPERTY_UNBOUND, 0 },
    { "FrameIsAutoScroll",    17, 2, &::getBooleanCppuType(), PROPERTY_UNBOUND, 0 },
    { "FrameIsBorder",        13, 3, &::getBooleanCppuType(), PROPERTY_UNBOUND, 0 },
    { "FrameIsScrollingMode", 20, 4, &::getBooleanCppuType(), PROPERTY_UNBOUND, 0 },
    { "FrameMarginHeight",    17, 5, &::getCppuType( (sal_Int32*)0 ), PROPERTY_UNBOUND, 0 },
    { "FrameMarginWidth",     16, 6, &::getCppuType( (sal_Int32*)0 ), PROPERTY_UNBOUND, 0 },
    { "FrameName",             9, 7, &::getCppuType((const ::rtl::OUString*)0), PROPERTY_UNBOUND, 0 },
    { "FrameURL",              8, 8, &::getCppuType((const ::rtl::OUString*)0), PROPERTY_UNBOUND, 0 },
    {0,0,0,0,0,0}
};

SFX_IMPL_XSERVICEINFO( IFrameObject, "com.sun.star.embed.SpecialEmbeddedObject", "com.sun.star.comp.sfx2.IFrameObject" )
SFX_IMPL_SINGLEFACTORY( IFrameObject );

IFrameObject::IFrameObject( const uno::Reference < lang::XMultiServiceFactory >& rFact )
    : mxFact( rFact )
    , maPropSet( aIFramePropertyMap_Impl )
{
}

IFrameObject::~IFrameObject()
{
}


void SAL_CALL IFrameObject::initialize( const uno::Sequence< uno::Any >& aArguments ) throw ( uno::Exception, uno::RuntimeException )
{
    if ( aArguments.getLength() )
        aArguments[0] >>= mxObj;
}

sal_Bool SAL_CALL IFrameObject::load(
    const uno::Sequence < com::sun::star::beans::PropertyValue >& /*lDescriptor*/,
    const uno::Reference < frame::XFrame >& xFrame )
throw( uno::RuntimeException )
{
    if ( SvtMiscOptions().IsPluginsEnabled() )
    {
        DBG_ASSERT( !mxFrame.is(), "Frame already existing!" );
        Window* pParent = VCLUnoHelper::GetWindow( xFrame->getContainerWindow() );
        IFrameWindow_Impl* pWin = new IFrameWindow_Impl( pParent, maFrmDescr.IsFrameBorderOn() );
        pWin->SetSizePixel( pParent->GetOutputSizePixel() );
        pWin->SetBackground();
        pWin->Show();

        uno::Reference < awt::XWindow > xWindow( pWin->GetComponentInterface(), uno::UNO_QUERY );
        xFrame->setComponent( xWindow, uno::Reference < frame::XController >() );

        // we must destroy the IFrame before the parent is destroyed
        xWindow->addEventListener( this );

        mxFrame = uno::Reference< frame::XFrame >( mxFact->createInstance( ::rtl::OUString::createFromAscii( "com.sun.star.frame.Frame" ) ),
                    uno::UNO_QUERY );
        uno::Reference < awt::XWindow > xWin( pWin->GetComponentInterface(), uno::UNO_QUERY );
        mxFrame->initialize( xWin );
        mxFrame->setName( maFrmDescr.GetName() );

        uno::Reference< frame::XDispatchProvider > xProv( mxFrame, uno::UNO_QUERY );

        util::URL aTargetURL;
        aTargetURL.Complete = ::rtl::OUString( maFrmDescr.GetURL().GetMainURL( INetURLObject::NO_DECODE ) );
        uno::Reference < util::XURLTransformer > xTrans( mxFact->createInstance( rtl::OUString::createFromAscii("com.sun.star.util.URLTransformer" )), uno::UNO_QUERY );
        xTrans->parseStrict( aTargetURL );

        uno::Sequence < beans::PropertyValue > aProps(2);
        aProps[0].Name = ::rtl::OUString::createFromAscii("PluginMode");
        aProps[0].Value <<= (sal_Int16) 2;
        aProps[1].Name = ::rtl::OUString::createFromAscii("ReadOnly");
        aProps[1].Value <<= (sal_Bool) sal_True;
        uno::Reference < frame::XDispatch > xDisp = xProv->queryDispatch( aTargetURL, ::rtl::OUString::createFromAscii("_self"), 0 );
        if ( xDisp.is() )
            xDisp->dispatch( aTargetURL, aProps );

        return TRUE;
    }

    return FALSE;
}

void SAL_CALL IFrameObject::cancel() throw( com::sun::star::uno::RuntimeException )
{
    try
    {
        uno::Reference < util::XCloseable > xClose( mxFrame, uno::UNO_QUERY );
        if ( xClose.is() )
            xClose->close( sal_True );
        mxFrame = 0;
    }
    catch ( uno::Exception& )
    {}
}

void SAL_CALL IFrameObject::close( sal_Bool /*bDeliverOwnership*/ ) throw( com::sun::star::util::CloseVetoException, com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL IFrameObject::addCloseListener( const com::sun::star::uno::Reference < com::sun::star::util::XCloseListener >& ) throw( com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL IFrameObject::removeCloseListener( const com::sun::star::uno::Reference < com::sun::star::util::XCloseListener >& ) throw( com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL IFrameObject::disposing( const com::sun::star::lang::EventObject& ) throw (com::sun::star::uno::RuntimeException)
{
    cancel();
}

uno::Reference< beans::XPropertySetInfo > SAL_CALL IFrameObject::getPropertySetInfo() throw( ::com::sun::star::uno::RuntimeException )
{
    return maPropSet.getPropertySetInfo();
}

void SAL_CALL IFrameObject::setPropertyValue(const ::rtl::OUString& aPropertyName, const uno::Any& aAny)
    throw ( beans::UnknownPropertyException, beans::PropertyVetoException, lang::IllegalArgumentException, lang::WrappedTargetException, uno::RuntimeException)
{
    if ( aPropertyName.equalsAscii("FrameURL") )
    {
        ::rtl::OUString aURL;
        aAny >>= aURL;
        maFrmDescr.SetURL( String(aURL) );
    }
    else if ( aPropertyName.equalsAscii("FrameName") )
    {
        ::rtl::OUString aName;
        if ( aAny >>= aName )
            maFrmDescr.SetName( aName );
    }
    else if ( aPropertyName.equalsAscii("FrameIsAutoScroll") )
    {
        sal_Bool bIsAutoScroll = sal_Bool();
        if ( (aAny >>= bIsAutoScroll) && bIsAutoScroll )
            maFrmDescr.SetScrollingMode( ScrollingAuto );
    }
    else if ( aPropertyName.equalsAscii("FrameIsScrollingMode") )
    {
        sal_Bool bIsScroll = sal_Bool();
        if ( aAny >>= bIsScroll )
            maFrmDescr.SetScrollingMode( bIsScroll ? ScrollingYes : ScrollingNo );
    }
    else if ( aPropertyName.equalsAscii("FrameIsBorder") )
    {
        sal_Bool bIsBorder = sal_Bool();
        if ( aAny >>= bIsBorder )
            maFrmDescr.SetFrameBorder( bIsBorder );
    }
    else if ( aPropertyName.equalsAscii("FrameIsAutoBorder") )
    {
        sal_Bool bIsAutoBorder = sal_Bool();
        if ( (aAny >>= bIsAutoBorder) )
        {
            BOOL bBorder = maFrmDescr.IsFrameBorderOn();
            maFrmDescr.ResetBorder();
            if ( bIsAutoBorder )
                maFrmDescr.SetFrameBorder( bBorder );
        }
    }
    else if ( aPropertyName.equalsAscii("FrameMarginWidth") )
    {
        sal_Int32 nMargin = 0;
        Size aSize = maFrmDescr.GetMargin();
        if ( aAny >>= nMargin )
        {
            aSize.Width() = nMargin;
            maFrmDescr.SetMargin( aSize );
        }
    }
    else if ( aPropertyName.equalsAscii("FrameMarginHeight") )
    {
        sal_Int32 nMargin = 0;
        Size aSize = maFrmDescr.GetMargin();
        if ( aAny >>= nMargin )
        {
            aSize.Height() = nMargin;
            maFrmDescr.SetMargin( aSize );
        }
    }
    else
         throw beans::UnknownPropertyException();
}

uno::Any SAL_CALL IFrameObject::getPropertyValue(const ::rtl::OUString& aPropertyName)
        throw ( beans::UnknownPropertyException, lang::WrappedTargetException, uno::RuntimeException)
{
    uno::Any aAny;
    if ( aPropertyName.equalsAscii("FrameURL") )
    {
        aAny <<= ::rtl::OUString( maFrmDescr.GetURL().GetMainURL( INetURLObject::NO_DECODE ) );
    }
    else if ( aPropertyName.equalsAscii("FrameName") )
    {
        aAny <<= ::rtl::OUString( maFrmDescr.GetName() );
    }
    else if ( aPropertyName.equalsAscii("FrameIsAutoScroll") )
    {
        sal_Bool bIsAutoScroll = ( maFrmDescr.GetScrollingMode() == ScrollingAuto );
        aAny <<= bIsAutoScroll;
    }
    else if ( aPropertyName.equalsAscii("FrameIsScrollingMode") )
    {
        sal_Bool bIsScroll = ( maFrmDescr.GetScrollingMode() == ScrollingYes );
        aAny <<= bIsScroll;
    }
    else if ( aPropertyName.equalsAscii("FrameIsBorder") )
    {
        sal_Bool bIsBorder = maFrmDescr.IsFrameBorderOn();
        aAny <<= bIsBorder;
    }
    else if ( aPropertyName.equalsAscii("FrameIsAutoBorder") )
    {
        sal_Bool bIsAutoBorder = !maFrmDescr.IsFrameBorderSet();
        aAny <<= bIsAutoBorder;
    }
    else if ( aPropertyName.equalsAscii("FrameMarginWidth") )
    {
        aAny <<= (sal_Int32 ) maFrmDescr.GetMargin().Width();
    }
    else if ( aPropertyName.equalsAscii("FrameMarginHeight") )
    {
        aAny <<= (sal_Int32 ) maFrmDescr.GetMargin().Height();
    }
    else
         throw beans::UnknownPropertyException();
    return aAny;
}

void SAL_CALL IFrameObject::addPropertyChangeListener(const ::rtl::OUString&, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener > & ) throw( ::com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL IFrameObject::removePropertyChangeListener(const ::rtl::OUString&, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertyChangeListener > & ) throw( ::com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL IFrameObject::addVetoableChangeListener(const ::rtl::OUString&, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener > & ) throw( ::com::sun::star::uno::RuntimeException )
{
}

void SAL_CALL IFrameObject::removeVetoableChangeListener(const ::rtl::OUString&, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XVetoableChangeListener > & ) throw( ::com::sun::star::uno::RuntimeException )
{
}

::sal_Int16 SAL_CALL IFrameObject::execute() throw (::com::sun::star::uno::RuntimeException)
{
    SfxAbstractDialogFactory* pFact = SfxAbstractDialogFactory::Create();
    VclAbstractDialog* pDlg = pFact->CreateEditObjectDialog( NULL, SID_INSERT_FLOATINGFRAME, mxObj );
    if ( pDlg )
        pDlg->Execute();
    return 0;
}

void SAL_CALL IFrameObject::setTitle( const ::rtl::OUString& ) throw (::com::sun::star::uno::RuntimeException)
{
}

}
