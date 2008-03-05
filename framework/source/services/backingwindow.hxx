/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: backingwindow.hxx,v $
 *
 *  $Revision: 1.2 $
 *
 *  last change: $Author: kz $ $Date: 2008-03-05 17:23:33 $
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

#ifndef FRAMEWORK_BACKINGWINDOW_HXX
#define FRAMEWORK_BACKINGWINDOW_HXX

#include "rtl/ustring.hxx"

#include "vcl/button.hxx"
#include "vcl/fixed.hxx"
#include "vcl/bitmapex.hxx"
#include "vcl/toolbox.hxx"

#include "svtools/moduleoptions.hxx"

#include "com/sun/star/frame/XDispatchProvider.hpp"
#include "com/sun/star/frame/XDesktop.hpp"
#include "com/sun/star/frame/XTerminateListener.hpp"
#include "com/sun/star/document/XEventListener.hpp"
#include "com/sun/star/document/XEventBroadcaster.hpp"
#include "com/sun/star/util/XURLTransformer.hpp"
#include "com/sun/star/ui/dialogs/XFilePicker.hpp"
#include "com/sun/star/ui/dialogs/XFilePickerControlAccess.hpp"
#include "com/sun/star/ui/dialogs/XFilterManager.hpp"
#include "com/sun/star/ui/dialogs/XFolderPicker.hpp"
#include "com/sun/star/ui/dialogs/TemplateDescription.hpp"
#include "com/sun/star/ui/dialogs/ExecutableDialogResults.hpp"

#include <set>


namespace framework
{
        // To get the transparent mouse-over look, the closer is actually a toolbox
    // overload DataChange to handle style changes correctly
    class DecoToolBox : public ToolBox
    {
        Size maMinSize;

        using Window::ImplInit;
    public:
                DecoToolBox( Window* pParent, WinBits nStyle = 0 );
                DecoToolBox( Window* pParent, const ResId& rResId );

        void    DataChanged( const DataChangedEvent& rDCEvt );

        void    calcMinSize();
        Size    getMinSize();
    };

    class BackingWindow : public Window
    {
        com::sun::star::uno::Reference<com::sun::star::frame::XDesktop>                  mxDesktop;
        com::sun::star::uno::Reference<com::sun::star::frame::XDispatchProvider >        mxDesktopDispatchProvider;
        com::sun::star::uno::Reference<com::sun::star::document::XEventBroadcaster>      mxBroadcaster;

        FixedText                       maWelcome;
        Size                            maWelcomeSize;
        FixedText                       maProduct;
        Size                            maProductSize;
        FixedText                       maWriterText;
        ImageButton                     maWriterButton;
        FixedText                       maCalcText;
        ImageButton                     maCalcButton;
        FixedText                       maImpressText;
        ImageButton                     maImpressButton;
        FixedText                       maDrawText;
        ImageButton                     maDrawButton;
        FixedText                       maDBText;
        ImageButton                     maDBButton;
        FixedText                       maOpenText;
        ImageButton                     maOpenButton;
        FixedText                       maTemplateText;
        ImageButton                     maTemplateButton;

        DecoToolBox                     maToolbox;

        BitmapEx                        maBackgroundLeft;
        BitmapEx                        maBackgroundMiddle;
        BitmapEx                        maBackgroundRight;

        String                          maWelcomeString;
        String                          maProductString;
        String                          maCreateString;
        String                          maOpenString;
        String                          maTemplateString;

        Font                            maTextFont;
        Rectangle                       maControlRect;

        long                            mnColumnWidth[2];
        Color                           maLabelTextColor;
        Color                           maWelcomeTextColor;

        Size                            maButtonImageSize;

        static const long nBtnPos = 240;
        static const int nItemId_Extensions = 1;
        static const int nItemId_Reg = 2;
        static const int nItemId_Info = 3;
        static const int nShadowTop = 32;
        static const int nShadowLeft = 35;
        static const int nShadowRight = 45;
        static const int nShadowBottom = 50;

        void loadImage( const ResId& i_rId, ImageButton& i_rButton );

        void layoutButtonAndText( const char* i_pURL, int nColumn, const std::set<rtl::OUString>& i_rURLS,
                                  SvtModuleOptions& i_rOpt, SvtModuleOptions::EModule i_eMod,
                                  ImageButton& i_rBtn, FixedText& i_rText,
                                  const String& i_rStr = String()
                                  );

        bool executeFileOpen();
        void dispatchURL( const rtl::OUString& i_rURL,
                          const rtl::OUString& i_rTarget = rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "_default" ) ),
                          const com::sun::star::uno::Reference< com::sun::star::frame::XDispatchProvider >& i_xProv = com::sun::star::uno::Reference< com::sun::star::frame::XDispatchProvider >(),
                          const com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >& = com::sun::star::uno::Sequence< com::sun::star::beans::PropertyValue >()
                          );

        DECL_LINK( ClickHdl, Button* );
        DECL_LINK( ToolboxHdl, void* );
        public:
        BackingWindow( Window* pParent );
        ~BackingWindow();

        virtual void        Paint( const Rectangle& rRect );
        virtual void        Resize();
    };

}

#endif
