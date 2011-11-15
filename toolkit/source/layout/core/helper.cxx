/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2000, 2010 Oracle and/or its affiliates.
 *
 * OpenOffice.org - a multi-platform office productivity suite
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

#include "helper.hxx"

#include <assert.h>
#include <list>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/XWindow.hpp>
#include <com/sun/star/awt/VclWindowPeerAttribute.hpp>
#include <toolkit/awt/vclxwindow.hxx>
#include <tools/debug.hxx>

#include "proplist.hxx"

namespace layoutimpl
{
using namespace com::sun::star;
using rtl::OUString;

}

#include "bin.hxx"
#include "box.hxx"
#include "dialogbuttonhbox.hxx"
#include "flow.hxx"
#include "localized-string.hxx"
#include "table.hxx"

namespace layoutimpl
{

oslModule WidgetFactory::mSfx2Library = 0;
WindowCreator WidgetFactory::mSfx2CreateWidget = 0;

uno::Reference <awt::XLayoutContainer> WidgetFactory::createContainer (OUString const& name)
{
    uno::Reference< awt::XLayoutContainer > xPeer;

    if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "hbox" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new HBox() );
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "vbox" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new VBox() );
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "table" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new Table() );
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "flow" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new Flow() );
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "bin" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new Bin() );
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "min-size" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new MinSize() );
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "align" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new Align() );
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "dialogbuttonhbox" ) ) )
        xPeer = uno::Reference< awt::XLayoutContainer >( new DialogButtonHBox() );

    return xPeer;
}

uno::Reference <awt::XLayoutConstrains> WidgetFactory::toolkitCreateWidget (uno::Reference <awt::XToolkit> xToolkit, uno::Reference <uno::XInterface> xParent, OUString const& name, long properties)
{
    uno::Reference< awt::XLayoutConstrains > xPeer;
    bool bToplevel = !xParent.is();

    // UNO Control Widget
    awt::WindowDescriptor desc;
    if ( bToplevel )
        desc.Type = awt::WindowClass_TOP;
    else
    {
        desc.Type = awt::WindowClass_SIMPLE;

        uno::Reference< awt::XWindowPeer > xWinParent( xParent, uno::UNO_QUERY );
        assert( xParent.is() );
        assert( xWinParent.is() );
        /*
          With the new three layer instarr/rpath feature, when
          prepending toolkit/unxlngx6.pro/lib or $SOLARVER/lib to
          LD_LIBRARY_PATH, VCLXWindow::GetImplementation returns 0x0
          vclxtoolkit::ImplCreateWindow failing to create any widget;
          although it succeeds here.

          While developing, one now must copy libtlx.so to
          $OOO_INSTALL_PREFIX/openoffice.org/basis3.0/program/libtklx.so
          each time.
        */
        VCLXWindow* parentComponent = VCLXWindow::GetImplementation( xWinParent );
        if ( !parentComponent )
            throw uno::RuntimeException(
                OUString(RTL_CONSTASCII_USTRINGPARAM("parent has no implementation")),
                uno::Reference< uno::XInterface >() );
        desc.Parent = xWinParent;
    }

    desc.ParentIndex = 0;
    // debugging help ...
    desc.Bounds.X = 0;
    desc.Bounds.Y = 0;
    desc.Bounds.Width = 300;
    desc.Bounds.Height = 200;

    desc.WindowAttributes = properties;
    desc.WindowServiceName = name;

    uno::Reference< awt::XWindowPeer > xWinPeer;
    try
    {
        OSL_TRACE("Asking toolkit: %s", OUSTRING_CSTR( desc.WindowServiceName ) );
        xWinPeer = xToolkit->createWindow( desc );
        if ( !xWinPeer.is() )
            throw uno::RuntimeException(
                OUString( RTL_CONSTASCII_USTRINGPARAM( "Cannot create peer" ) ),
                uno::Reference< uno::XInterface >() );
        xPeer = uno::Reference< awt::XLayoutConstrains >( xWinPeer, uno::UNO_QUERY );
    }
    catch( uno::Exception & )
    {
        OSL_TRACE( "Warning: %s is not a recognized type", OUSTRING_CSTR( name ) );
        return uno::Reference< awt::XLayoutConstrains >();
    }

    return xPeer;
}

uno::Reference< awt::XLayoutConstrains >
WidgetFactory::createWidget (uno::Reference< awt::XToolkit > xToolkit, uno::Reference< uno::XInterface > xParent, OUString const& name, long properties)
{
    uno::Reference< awt::XLayoutConstrains > xPeer;

    xPeer = uno::Reference <awt::XLayoutConstrains> (createContainer (name), uno::UNO_QUERY);
    if ( xPeer.is() )
        return xPeer;

    xPeer = implCreateWidget (xParent, name, properties);
    if (xPeer.is ())
        return xPeer;

#define FIXED_INFO 1
#if FIXED_INFO
    OUString tName = name;
    // FIXME
    if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "fixedinfo" ) ) )
        tName = OUString(RTL_CONSTASCII_USTRINGPARAM("fixedtext"));
    xPeer = toolkitCreateWidget (xToolkit, xParent, tName, properties);
#else
    xPeer = toolkitCreateWidget (xToolkit, xParent, name, properties);
#endif

    return xPeer;
}

PropHelper::PropHelper() : LockHelper()
                         , cppu::OPropertySetHelper( maBrdcstHelper )
                         , pHelper( NULL )
{
}

void
PropHelper::addProp (const char *pName, sal_Int32 nNameLen, rtl_TextEncoding e,
                     uno::Type aType, void *pPtr)
{
    // this sucks rocks for effiency ...
    PropDetails aDetails;
    aDetails.aName = rtl::OUString::intern( pName, nNameLen, e );
    aDetails.aType = aType;
    aDetails.pValue = pPtr;
    maDetails.push_back( aDetails );
}

cppu::IPropertyArrayHelper & SAL_CALL
PropHelper::getInfoHelper()
{
    if ( ! pHelper )
    {
        uno::Sequence< beans::Property > aProps( maDetails.size() );
        for ( unsigned int i = 0; i < maDetails.size(); i++)
        {
            aProps[i].Name = maDetails[i].aName;
            aProps[i].Type = maDetails[i].aType;
            aProps[i].Handle = i;
            aProps[i].Attributes = 0;
        }
        pHelper = new cppu::OPropertyArrayHelper( aProps, false /* fixme: faster ? */ );

    }
    return *pHelper;
}

sal_Bool SAL_CALL
PropHelper::convertFastPropertyValue(
    uno::Any & rConvertedValue,
    uno::Any & rOldValue,
    sal_Int32 nHandle,
    const uno::Any& rValue )
    throw (lang::IllegalArgumentException)
{
    OSL_ASSERT( nHandle >= 0 && nHandle < (sal_Int32) maDetails.size() );

    // FIXME: no Any::getValue ...
    getFastPropertyValue( rOldValue, nHandle );
    if ( rOldValue != rValue )
    {
        rConvertedValue = rValue;
        return sal_True; // changed
    }
    else
    {
        rConvertedValue.clear();
        rOldValue.clear();
    }
    return sal_False;
}


void SAL_CALL
PropHelper::setFastPropertyValue_NoBroadcast( sal_Int32 nHandle,
                                              const uno::Any& rValue )
    throw (uno::Exception)
{
    OSL_ASSERT( nHandle >= 0 && nHandle < (sal_Int32) maDetails.size() );

    const PropDetails &rInfo = maDetails[ nHandle ];

    uno_type_assignData( rInfo.pValue, rInfo.aType.getTypeLibType(),
                         rValue.pData, rValue.pType,
                         0, 0, 0 );

    if ( mpListener )
        mpListener->propertiesChanged();
}

void SAL_CALL
PropHelper::getFastPropertyValue( uno::Any& rValue,
                                  sal_Int32 nHandle ) const
{
    OSL_ASSERT( nHandle >= 0 && nHandle < (sal_Int32) maDetails.size() );
    const PropDetails &rInfo = maDetails[ nHandle ];
    rValue.setValue( rInfo.pValue, rInfo.aType );
}

::com::sun::star::uno::Any
PropHelper::queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException)
{
    return OPropertySetHelper::queryInterface( rType );
}

uno::Reference <beans::XPropertySetInfo> SAL_CALL PropHelper::getPropertySetInfo () throw (uno::RuntimeException)
{
    return css::uno::Reference <css::beans::XPropertySetInfo> (createPropertySetInfo (getInfoHelper ()));
}

} // namespace layoutimpl

#include <awt/vclxbutton.hxx>
#include <awt/vclxdialog.hxx>
#include <awt/vclxfixedline.hxx>
#include <awt/vclxplugin.hxx>
#include <awt/vclxscroller.hxx>
#include <awt/vclxsplitter.hxx>
#include <awt/vclxtabcontrol.hxx>
#include <awt/vclxtabpage.hxx>
#include <toolkit/awt/vclxtoolkit.hxx>
#include <toolkit/awt/vclxwindow.hxx>
#include <vcl/button.hxx>
#include <vcl/dialog.hxx>
#include <vcl/fixed.hxx>
#include <vcl/tabctrl.hxx>
#include <vcl/tabpage.hxx>
#include <vcl/unohelp.hxx>

#include <layout/layout.hxx>
#include <toolkit/awt/vclxwindows.hxx>
#include <vcl/lstbox.hxx>
#include <vcl.hxx>

#include <typeinfo>

namespace layoutimpl
{

uno::Reference <awt::XLayoutConstrains> WidgetFactory::implCreateWidget (uno::Reference <uno::XInterface> xParent, OUString name, long attributes)
{
    Window* parent = 0;

    if (VCLXWindow* parentComponent = VCLXWindow::GetImplementation (xParent))
        parent = parentComponent->GetWindow ();

    VCLXWindow* component = 0;
    Window* window = 0;
    if (!window)
        window = layoutCreateWindow (&component, parent, name, attributes);

    uno::Reference <awt::XLayoutConstrains> reference;
    if (window)
    {
        window->SetCreatedWithToolkit( sal_True );
        if ( component )
            component->SetCreatedWithToolkit( true );
        reference = component;
        window->SetComponentInterface( component );
        if ( attributes & awt::WindowAttribute::SHOW )
            window->Show();
    }

    return reference;
}

Window* WidgetFactory::layoutCreateWindow (VCLXWindow** component, Window *parent, OUString const& name, long& attributes)
{
    Window* window = 0;

    if (0)
    {
        ;
    }
    if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "dialog" ) ) )
    {
        if ( parent == NULL )
            parent = DIALOG_NO_PARENT;
        window = new Dialog( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXDialog();

        attributes ^= awt::WindowAttribute::SHOW;
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "modaldialog" ) ) )
    {
        if ( parent == NULL )
            parent = DIALOG_NO_PARENT;
        window = new ModalDialog( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXDialog();

        attributes ^= awt::WindowAttribute::SHOW;
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "modelessdialog" ) ) )
    {
        if ( parent == NULL )
            parent = DIALOG_NO_PARENT;
        window = new ModelessDialog (parent, ImplGetWinBits (attributes, 0));
        *component = new layoutimpl::VCLXDialog();

        attributes ^= awt::WindowAttribute::SHOW;
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "sfxdialog" ) ) )
    {
        if ( parent == NULL )
            parent = DIALOG_NO_PARENT;
        window = new ClosingDialog (parent, ImplGetWinBits (attributes, 0));
        *component = new layoutimpl::VCLXDialog();

        attributes ^= awt::WindowAttribute::SHOW;
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "sfxmodaldialog" ) ) )
    {
        if ( parent == NULL )
            parent = DIALOG_NO_PARENT;
        window = new ClosingModalDialog( parent,
                                         ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXDialog();

        attributes ^= awt::WindowAttribute::SHOW;
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "sfxmodelessdialog" ) ) )
    {
        if ( parent == NULL )
            parent = DIALOG_NO_PARENT;
        window = new ClosingModelessDialog (parent, ImplGetWinBits (attributes, 0));
        *component = new layoutimpl::VCLXDialog();

        attributes ^= awt::WindowAttribute::SHOW;
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "tabcontrol" ) ) )
    {
        window = new TabControl( parent, ImplGetWinBits( attributes, WINDOW_TABCONTROL ) );
        *component = new layoutimpl::VCLXTabControl();
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "scroller" ) ) )
    {
        // used FixedImage because I just want some empty non-intrusive widget
        window = new FixedImage( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXScroller();
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "hsplitter" ) ) || name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "vsplitter" ) ) )
    {
        window = new FixedImage( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXSplitter( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "hsplitter" ) ) );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "hfixedline" ) ) || name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "vfixedline" ) ) )
    {
        WinBits nStyle = ImplGetWinBits( attributes, 0 );
        nStyle ^= WB_HORZ;
        if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "hfixedline" ) ) )
            nStyle |= WB_HORZ;
        else
            nStyle |= WB_VERT;
        window = new FixedLine( parent, nStyle );
        *component = new layoutimpl::VCLXFixedLine();
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "okbutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXOKButton( window );
        window->SetType (WINDOW_OKBUTTON);
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "cancelbutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXCancelButton( window );
        window->SetType (WINDOW_CANCELBUTTON);
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "yesbutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXYesButton( window );
        window->SetType (WINDOW_OKBUTTON);
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "nobutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        window->SetType (WINDOW_CANCELBUTTON);
        *component = new layoutimpl::VCLXNoButton( window );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "retrybutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXRetryButton( window );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "ignorebutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXIgnoreButton( window );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "resetbutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXResetButton( window );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "applybutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXApplyButton( window );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "helpbutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXHelpButton( window );
        window->SetType (WINDOW_HELPBUTTON);
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "morebutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXMoreButton( window );
        window->SetType (WINDOW_MOREBUTTON);
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "advancedbutton" ) ) )
    {
        window = new PushButton( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::VCLXAdvancedButton( window );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "plugin" ) ) )
    {
        window = new Control( parent, ImplGetWinBits( attributes, 0 ) );
#ifndef __SUNPRO_CC
        OSL_TRACE( "%s: parent=%p (%s)", __FUNCTION__, parent, typeid( *parent ).name() );
#endif
        *component = new layoutimpl::VCLXPlugin( window, ImplGetWinBits( attributes, 0 ) );
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "tabpage" ) ) )
    {
        if (layout::TabPage::global_parent)
            parent = layout::TabPage::global_parent;
        layout::TabPage::global_parent = 0;
        //window = new TabPage( parent, ImplGetWinBits( attributes, 0 ) );
        attributes ^= awt::WindowAttribute::SHOW;
        WinBits nStyle = ImplGetWinBits( attributes, 0 );
        nStyle |= WB_HIDE;

        if (!parent)
        {
            window = new Dialog( parent, nStyle );
            *component = new VCLXDialog();
        }
        else
        {
            window = new TabPage( parent, nStyle );
            *component = new VCLXTabPage( window );
        }
    }
    else if ( name.equalsAsciiL( RTL_CONSTASCII_STRINGPARAM( "string" ) ) )
    {
        // FIXME: move <string>s.text to simple map<string> in root?
        attributes &= ~awt::WindowAttribute::SHOW;
        window = new Window( parent, ImplGetWinBits( attributes, 0 ) );
        *component = new layoutimpl::LocalizedString();
    }
    else if (name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("svxfontlistbox"))
             || name.equalsAsciiL(RTL_CONSTASCII_STRINGPARAM("svxlanguagebox")))
    {
        window = new ListBox (parent, ImplGetWinBits (attributes, 0));
        *component = new VCLXListBox ();
    }
    return window;
}

} // namespace layoutimpl

// Avoid polluting the rest of the code with vcl linkage pieces ...

#include <vcl/imagerepository.hxx>
#include <vcl/bitmapex.hxx>
#include <vcl/graph.hxx>

namespace layoutimpl
{

uno::Reference< graphic::XGraphic > loadGraphic( const char *pName )
{
    BitmapEx aBmp;

    OUString aStr( pName, strlen( pName ), RTL_TEXTENCODING_ASCII_US );
    if ( aStr.compareToAscii( ".uno:" ) == 0 )
        aStr = aStr.copy( 5 ).toAsciiLowerCase();

    if ( !vcl::ImageRepository::loadImage( OUString::createFromAscii( pName ), aBmp, true, true ) )
        return uno::Reference< graphic::XGraphic >();

    return Graphic( aBmp ).GetXGraphic();
}

} // namespace layoutimpl

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
