/*************************************************************************
 *
 *  $RCSfile: bibbeam.cxx,v $
 *
 *  $Revision: 1.6 $
 *
 *  last change: $Author: fs $ $Date: 2001-10-22 07:31:41 $
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

#ifndef _OSL_MUTEX_HXX_
#include <osl/mutex.hxx>
#endif
#ifndef _URLOBJ_HXX
#include <tools/urlobj.hxx>
#endif
#ifndef _TOOLKIT_UNOHLP_HXX
#include <toolkit/helper/vclunohelper.hxx>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _COM_SUN_STAR_AWT_POSSIZE_HPP_
#include <com/sun/star/awt/PosSize.hpp>
#endif
#ifndef _COM_SUN_STAR_FRAME_XDISPATCH_HPP_
#include <com/sun/star/frame/XDispatch.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XURLTRANSFORMER_HPP_
#include <com/sun/star/util/XURLTransformer.hpp>
#endif

#ifndef __EXTENSIONS_INC_EXTENSIO_HRC__
#include "extensio.hrc"
#endif

#ifndef _SV_LSTBOX_HXX //autogen wg. form::component::ListBox
#include <vcl/lstbox.hxx>
#endif

#ifndef _SV_EDIT_HXX //autogen wg. Edit
#include <vcl/edit.hxx>
#endif

#ifndef _TOOLS_DEBUG_HXX //autogen wg. DBG_ASSERT
#include <tools/debug.hxx>
#endif

#ifndef ADRBEAM_HXX
#include "bibbeam.hxx"
#endif
#include "toolbar.hrc"
#ifndef ADRRESID_HXX
#include "bibresid.hxx"
#endif
#ifndef _BIB_DATMAN_HXX
#include "datman.hxx"
#endif

using namespace rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::awt;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;

#define C2U(cChar) OUString::createFromAscii(cChar)

#define PROPERTY_FRAME                      1
#define ID_TOOLBAR                          1
#define ID_GRIDWIN                          2

//.........................................................................
namespace bib
{
//.........................................................................

    using namespace ::com::sun::star::uno;
    using namespace ::com::sun::star::awt;

    //=====================================================================
    //= BibGridwin
    //=====================================================================
    class BibGridwin
                :public DockingWindow
    {
    private:
            Reference< XWindow >            m_xGridWin;
            Reference< XControlModel >      m_xGridModel;
            Reference< XControl >           m_xControl;
            Reference< XControlContainer >  m_xControlContainer;

    protected:

            virtual void        Resize();

    public:

            BibGridwin(Window* pParent, WinBits nStyle = WB_3DLOOK );
            ~BibGridwin();

            void createGridWin(const Reference< XControlModel > & xDbForm);
            void changeGridModel(const Reference< XControlModel > & xGModel);
            void disposeGridWin();

            const Reference< XControlContainer >& getControlContainer() const { return m_xControlContainer; }
    };

    //---------------------------------------------------------------------
    BibGridwin::BibGridwin( Window* _pParent, WinBits _nStyle )
        :DockingWindow( _pParent, _nStyle )
    {
        m_xControlContainer = VCLUnoHelper::CreateControlContainer(this);
    }

    //---------------------------------------------------------------------
    BibGridwin::~BibGridwin()
    {
    }

    //---------------------------------------------------------------------
    void BibGridwin::Resize()
    {
        if(m_xGridWin.is())
        {
            ::Size aSize = GetOutputSizePixel();
            m_xGridWin->setPosSize(0, 0, aSize.Width(),aSize.Height(), PosSize::SIZE);
        }
    }

    //---------------------------------------------------------------------
    void BibGridwin::createGridWin(const uno::Reference< XControlModel > & xGModel)
    {
        m_xGridModel = xGModel;

        if( m_xControlContainer.is())
        {
            uno::Reference< lang::XMultiServiceFactory >  xMgr = comphelper::getProcessServiceFactory();

            if ( m_xGridModel.is() && xMgr.is())
            {
                uno::Reference< XPropertySet >  xPropSet( m_xGridModel, UNO_QUERY );

                if ( xPropSet.is() && m_xGridModel.is() )
                {
                    uno::Any aAny = xPropSet->getPropertyValue( C2U("DefaultControl") );
                    rtl::OUString aControlName;
                    aAny >>= aControlName;

                    m_xControl = Reference< XControl > (xMgr->createInstance( aControlName ), UNO_QUERY );
                    DBG_ASSERT( m_xControl.is(), "no GridControl created" )
                    if ( m_xControl.is() )
                        m_xControl->setModel( m_xGridModel );
                }

                if ( m_xControl.is() )
                {
                    // Peer als Child zu dem FrameWindow
                    m_xControlContainer->addControl(C2U("GridControl"), m_xControl);
                    m_xGridWin=uno::Reference< XWindow > (m_xControl, UNO_QUERY );
                    m_xGridWin->setVisible( sal_True );
                    m_xControl->setDesignMode( sal_True );
                        // initially switch on the desing mode - switch it off _after_ loading the form
                        // 17.10.2001 - 93107 - frank.schoenheit@sun.com

                    ::Size aSize = GetOutputSizePixel();
                    m_xGridWin->setPosSize(0, 0, aSize.Width(),aSize.Height(), PosSize::POSSIZE);
                }
            }
        }
    }

    //---------------------------------------------------------------------
    void BibGridwin::disposeGridWin()
    {
        if ( m_xControl.is() )
        {
            m_xControlContainer->removeControl( m_xControl );
            m_xControl->dispose();
        }
    }

    //---------------------------------------------------------------------
    void BibGridwin::changeGridModel(const uno::Reference< XControlModel > & xGModel)
    {
        m_xGridModel = xGModel;

        if ( m_xControl.is() )
            m_xControl->setModel( m_xGridModel );
    }

    //---------------------------------------------------------------------
    BibBeamer::BibBeamer( Window* _pParent, BibDataManager* _pDM, WinBits _nStyle )
        :SplitWindow( _pParent, _nStyle | WB_NOSPLITDRAW )
        ,pDatMan( _pDM )
        ,pToolBar( NULL )
        ,pGridWin( NULL )
    {
        createToolBar();
        createGridWin();
        if ( pDatMan )
            pDatMan->SetToolbar(pToolBar);
        pGridWin->Show();

        if ( pDatMan )
            connectForm( pDatMan );
    }

    //---------------------------------------------------------------------
    BibBeamer::~BibBeamer()
    {
        if ( isFormConnected() )
            disconnectForm();

        if ( m_xToolBarRef.is() )
            m_xToolBarRef->dispose();

        if ( pToolBar )
        {
            if ( pDatMan )
                pDatMan->SetToolbar(0);

            DELETEZ( pToolBar );
        }

        if ( pGridWin )
        {
            pGridWin->disposeGridWin();
            DELETEZ( pGridWin );
        }

    }

    //---------------------------------------------------------------------
    void BibBeamer::createToolBar()
    {
        pToolBar= new BibToolBar(this);
        ::Size aSize=pToolBar->GetSizePixel();
        InsertItem(ID_TOOLBAR, pToolBar, aSize.Height(), 0, 0, SWIB_FIXED );
        if ( m_xController.is() )
            pToolBar->SetXController( m_xController );
    }

    //---------------------------------------------------------------------
    void BibBeamer::createGridWin()
    {
        pGridWin = new BibGridwin(this,0);

        InsertItem(ID_GRIDWIN, pGridWin, 40, 1, 0, SWIB_RELATIVESIZE );

        pGridWin->createGridWin( pDatMan->updateGridModel() );
    }

    //---------------------------------------------------------------------
    Reference< XControlContainer > BibBeamer::getControlContainer()
    {
        Reference< XControlContainer > xReturn;
        if ( pGridWin )
            xReturn = pGridWin->getControlContainer();
        return xReturn;
    }

    //---------------------------------------------------------------------
    void BibBeamer::SetXController(const uno::Reference< frame::XController > & xCtr)
    {
        m_xController = xCtr;

        if ( pToolBar )
            pToolBar->SetXController( m_xController );

    }

//.........................................................................
}   // namespace bib
//.........................................................................
