/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: xmlxtexp.cxx,v $
 *
 *  $Revision: 1.19 $
 *
 *  last change: $Author: vg $ $Date: 2006-11-21 17:14:32 $
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
#include "precompiled_svx.hxx"

#ifndef _TOOLS_DEBUG_HXX
#include <tools/debug.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif
#ifndef _COM_SUN_STAR_XML_SAX_XDOCUMENTHANDLER_HPP_
#include <com/sun/star/xml/sax/XDocumentHandler.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_DRAWING_POLYPOLYGONBEZIERCOORDS_HPP_
#include <com/sun/star/drawing/PolyPolygonBezierCoords.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_LINEDASH_HPP_
#include <com/sun/star/drawing/LineDash.hpp>
#endif
#ifndef _COM_SUN_STAR_AWT_GRADIENT_HPP_
#include <com/sun/star/awt/Gradient.hpp>
#endif
#ifndef _COM_SUN_STAR_DRAWING_HATCH_HPP_
#include <com/sun/star/drawing/Hatch.hpp>
#endif
#ifndef _COM_SUN_STAR_IO_XACTIVEDATASOURCE_HPP_
#include <com/sun/star/io/XActiveDataSource.hpp>
#endif
#ifndef _COM_SUN_STAR_EMBED_ELEMENTMODES_HPP_
#include <com/sun/star/embed/ElementModes.hpp>
#endif

#ifndef _SFXDOCFILE_HXX
#include <sfx2/docfile.hxx>
#endif

#ifndef _RTL_USTRBUF_HXX_
#include <rtl/ustrbuf.hxx>
#endif

#ifndef _XMLOFF_XMLUCONV_HXX
#include <xmloff/xmluconv.hxx>
#endif

#ifndef _XMLOFF_XMLNMSPE_HXX
#include "xmloff/xmlnmspe.hxx"
#endif

#ifndef _XMLOFF_NMSPMAP_HXX
#include "xmloff/nmspmap.hxx"
#endif

#include "xmloff/xmltoken.hxx"

#ifndef _XMLOFF_XMLMETAE_HXX
#include "xmloff/xmlmetae.hxx"
#endif

#ifndef _XMLOFF_DASHSTYLE_HXX
#include "xmloff/DashStyle.hxx"
#endif

#ifndef _XMLOFF_GRADIENTSTYLE_HXX
#include "xmloff/GradientStyle.hxx"
#endif

#ifndef _XMLOFF_HATCHSTYLE_HXX
#include "xmloff/HatchStyle.hxx"
#endif

#ifndef _XMLOFF_IMAGESTYLE_HXX
#include "xmloff/ImageStyle.hxx"
#endif

#ifndef _XMLOFF_MARKERSTYLE_HXX
#include "xmloff/MarkerStyle.hxx"
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMECONTAINER_HPP_
#include <com/sun/star/container/XNameContainer.hpp>
#endif

#ifndef _COM_SUN_STAR_EMBED_XTRANSACTEDOBJECT_HPP_
#include <com/sun/star/embed/XTransactedObject.hpp>
#endif

#ifndef _COMPHELPER_PROCESSFACTORY_HXX_
#include <comphelper/processfactory.hxx>
#endif

#ifndef _UTL_STREAM_WRAPPER_HXX_
#include <unotools/streamwrap.hxx>
#endif

#ifndef _XMLGRHLP_HXX
#include "xmlgrhlp.hxx"
#endif

#include "xmlxtexp.hxx"

#include <comphelper/storagehelper.hxx>

using namespace com::sun::star;
using namespace com::sun::star::container;
using namespace com::sun::star::document;
using namespace com::sun::star::uno;
using namespace com::sun::star::awt;
using namespace com::sun::star::lang;
using namespace com::sun::star::xml::sax;
using namespace ::xmloff::token;
using namespace ::rtl;
using namespace cppu;

using com::sun::star::embed::XTransactedObject;

class SvxXMLTableEntryExporter
{
public:
    SvxXMLTableEntryExporter( SvXMLExport& rExport ) : mrExport( rExport ) {}
    virtual ~SvxXMLTableEntryExporter();

    virtual void exportEntry( const OUString& rStrName, const Any& rValue ) = 0;

protected:
    SvXMLExport& mrExport;
};

class SvxXMLColorEntryExporter : public SvxXMLTableEntryExporter
{
public:
    SvxXMLColorEntryExporter( SvXMLExport& rExport );
    virtual ~SvxXMLColorEntryExporter();

    virtual void exportEntry( const OUString& rStrName, const Any& rValue );
};

class SvxXMLLineEndEntryExporter : public SvxXMLTableEntryExporter
{
public:
    SvxXMLLineEndEntryExporter( SvXMLExport& rExport );
    SvxXMLLineEndEntryExporter();
    virtual ~SvxXMLLineEndEntryExporter();

    virtual void exportEntry( const OUString& rStrName, const Any& rValue );
private:
    XMLMarkerStyleExport maMarkerStyle;
};

class SvxXMLDashEntryExporter : public SvxXMLTableEntryExporter
{
public:
    SvxXMLDashEntryExporter( SvXMLExport& rExport );
    virtual ~SvxXMLDashEntryExporter();

    virtual void exportEntry( const OUString& rStrName, const Any& rValue );

private:
    XMLDashStyleExport maDashStyle;
};

class SvxXMLHatchEntryExporter : public SvxXMLTableEntryExporter
{
public:
    SvxXMLHatchEntryExporter( SvXMLExport& rExport );
    virtual ~SvxXMLHatchEntryExporter();

    virtual void exportEntry( const OUString& rStrName, const Any& rValue );
private:
    XMLHatchStyleExport maHatchStyle;
};

class SvxXMLGradientEntryExporter : public SvxXMLTableEntryExporter
{
public:
    SvxXMLGradientEntryExporter( SvXMLExport& rExport );
    virtual ~SvxXMLGradientEntryExporter();

    virtual void exportEntry( const OUString& rStrName, const Any& rValue );
private:
    XMLGradientStyleExport maGradientStyle;
};

class SvxXMLBitmapEntryExporter : public SvxXMLTableEntryExporter
{
public:
    SvxXMLBitmapEntryExporter( SvXMLExport& rExport );
    virtual ~SvxXMLBitmapEntryExporter();

    virtual void exportEntry( const OUString& rStrName, const Any& rValue );

private:
    XMLImageStyle maImageStyle;
};

///////////////////////////////////////////////////////////////////////

// #110680#
SvxXMLXTableExportComponent::SvxXMLXTableExportComponent(
    const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
    const OUString& rFileName,
    const uno::Reference<xml::sax::XDocumentHandler> & rHandler,
    const uno::Reference<container::XNameContainer >& xTable,
    uno::Reference<document::XGraphicObjectResolver >& xGrfResolver )
:   SvXMLExport( xServiceFactory, rFileName, rHandler, NULL, MAP_100TH_MM),
    mxTable( xTable )
{

    _GetNamespaceMap().Add( GetXMLToken(XML_NP_OOO), GetXMLToken(XML_N_OOO), XML_NAMESPACE_OOO );
    _GetNamespaceMap().Add( GetXMLToken(XML_NP_OFFICE), GetXMLToken(XML_N_OFFICE), XML_NAMESPACE_OFFICE );
    _GetNamespaceMap().Add( GetXMLToken(XML_NP_DRAW), GetXMLToken(XML_N_DRAW), XML_NAMESPACE_DRAW );
    _GetNamespaceMap().Add( GetXMLToken(XML_NP_XLINK), GetXMLToken(XML_N_XLINK), XML_NAMESPACE_XLINK );
    _GetNamespaceMap().Add( GetXMLToken(XML_NP_SVG), GetXMLToken(XML_N_SVG),  XML_NAMESPACE_SVG );
    SetGraphicResolver( xGrfResolver );
    setExportFlags( 0 );
}

SvxXMLXTableExportComponent::~SvxXMLXTableExportComponent()
{
}

sal_Bool SvxXMLXTableExportComponent::save( const OUString& rURL, const uno::Reference<container::XNameContainer >& xTable ) throw()
{
    uno::Reference < embed::XStorage > xStorage;
    SfxMedium* pMedium = NULL;
    sal_Bool bRet = sal_False;

    Reference< XGraphicObjectResolver > xGrfResolver;
    SvXMLGraphicHelper* pGraphicHelper = 0;

    try
    {
        do
        {
            uno::Reference < io::XOutputStream > xOut;
            uno::Reference < io::XStream > xStream;

            sal_Bool bNeedStorage = xTable->getElementType() == ::getCppuType((const OUString*)0);

            uno::Reference< lang::XMultiServiceFactory> xServiceFactory( ::comphelper::getProcessServiceFactory() );
            if( !xServiceFactory.is() )
            {
                DBG_ERROR( "got no service manager" );
                return FALSE;
            }

            uno::Reference< uno::XInterface > xWriter( xServiceFactory->createInstance( OUString( RTL_CONSTASCII_USTRINGPARAM( "com.sun.star.xml.sax.Writer" ) ) ) );
            if( !xWriter.is() )
            {
                DBG_ERROR( "com.sun.star.xml.sax.Writer service missing" );
                return FALSE;
            }

            uno::Reference<xml::sax::XDocumentHandler>  xHandler( xWriter, uno::UNO_QUERY );

            if( bNeedStorage )
            {
                xStorage =
                  ::comphelper::OStorageHelper::GetStorageFromURL( rURL, embed::ElementModes::WRITE | embed::ElementModes::TRUNCATE );

                if( !xStorage.is() )
                {
                    DBG_ERROR( "no storage!" );
                    break;
                }

                OUString sMetaName( RTL_CONSTASCII_USTRINGPARAM( "Content.xml" ) );
                xStream = xStorage->openStreamElement( sMetaName, embed::ElementModes::WRITE );
                pGraphicHelper = SvXMLGraphicHelper::Create( xStorage, GRAPHICHELPER_MODE_WRITE );
                xGrfResolver = pGraphicHelper;
                xOut = xStream->getOutputStream();
            }
            else
            {
                pMedium = new SfxMedium( rURL, STREAM_WRITE | STREAM_TRUNC, TRUE );
                pMedium->IsRemote();

                SvStream* pStream = pMedium->GetOutStream();
                if( NULL == pStream )
                {
                    DBG_ERROR( "no output stream!" );
                    break;
                }

                xOut = new utl::OOutputStreamWrapper( *pStream );
            }

            uno::Reference<io::XActiveDataSource> xMetaSrc( xWriter, uno::UNO_QUERY );
            xMetaSrc->setOutputStream( xOut );

            const OUString aName;

            // #110680#
            // SvxXMLXTableExportComponent aExporter( aName, xHandler, xTable, xGrfResolver );
            SvxXMLXTableExportComponent aExporter( xServiceFactory, aName, xHandler, xTable, xGrfResolver );

            bRet = aExporter.exportTable();

        }
        while( 0 );

        if( pGraphicHelper )
            SvXMLGraphicHelper::Destroy( pGraphicHelper );

        if( xStorage.is() )
        {
            Reference< XTransactedObject > xTrans( xStorage, UNO_QUERY );
            if( xTrans.is() )
                xTrans->commit();

            Reference< XComponent > xComp( xStorage, UNO_QUERY );
            if( xComp.is() )
                xStorage->dispose();
        }
    }
    catch( uno::Exception& )
    {
        bRet = sal_False;
    }

    if( pMedium )
    {
        pMedium->Commit();
        delete pMedium;
    }

    return bRet;
}

sal_Bool SvxXMLXTableExportComponent::exportTable() throw()
{
    sal_Bool bRet = sal_False;

    try
    {
        GetDocHandler()->startDocument();

        // export namespaces
        sal_uInt16 nPos = GetNamespaceMap().GetFirstKey();
        while( USHRT_MAX != nPos )
        {
            GetAttrList().AddAttribute( GetNamespaceMap().GetAttrNameByKey( nPos ), GetNamespaceMap().GetNameByKey( nPos ) );
            nPos = GetNamespaceMap().GetNextKey( nPos );
        }

        do
        {
            if( !mxTable.is() )
                break;

            char const* pEleName;
            Type aExportType = mxTable->getElementType();
            SvxXMLTableEntryExporter* pExporter = NULL;

            if( aExportType == ::getCppuType((const sal_Int32*)0) )
            {
                pExporter = new SvxXMLColorEntryExporter(*this);
                pEleName = "color-table";
            }
            else if( aExportType == ::getCppuType((const drawing::PolyPolygonBezierCoords*)0) )
            {
                pExporter = new SvxXMLLineEndEntryExporter(*this);
                pEleName = "marker-table";
            }
            else if( aExportType == ::getCppuType((const drawing::LineDash*)0) )
            {
                pExporter = new SvxXMLDashEntryExporter(*this);
                pEleName = "dash-table";
            }
            else if( aExportType == ::getCppuType((const drawing::Hatch*)0) )
            {
                pExporter = new SvxXMLHatchEntryExporter(*this);
                pEleName = "hatch-table";
            }
            else if( aExportType == ::getCppuType((const awt::Gradient*)0))
            {
                pExporter = new SvxXMLGradientEntryExporter(*this);
                pEleName = "gradient-table";
            }
            else if( aExportType == ::getCppuType((const OUString*)0))
            {
                pExporter = new SvxXMLBitmapEntryExporter(*this);
                pEleName = "bitmap-table";
            }
            else
            {
                DBG_ERROR( "unknown type for export");
                break;
            }

            SvXMLElementExport aElem( *this, XML_NAMESPACE_OOO, pEleName, sal_True, sal_True );

            Sequence< OUString > aNames = mxTable->getElementNames();
            const sal_Int32 nCount = aNames.getLength();
            const OUString* pNames = aNames.getConstArray();
            Any aAny;

            sal_Int32 nIndex;
            for( nIndex = 0; nIndex < nCount; nIndex++, pNames++ )
            {
                aAny = mxTable->getByName( *pNames );
                pExporter->exportEntry( *pNames, aAny );
            }

            bRet = sal_True;
        }
        while(0);

        GetDocHandler()->endDocument();
    }
    catch( Exception e )
    {
        bRet = sal_False;
    }

    return bRet;
}

// methods without content:
void SvxXMLXTableExportComponent::_ExportAutoStyles() {}
void SvxXMLXTableExportComponent::_ExportMasterStyles() {}
void SvxXMLXTableExportComponent::_ExportContent() {}

///////////////////////////////////////////////////////////////////////

SvxXMLTableEntryExporter::~SvxXMLTableEntryExporter()
{
}

///////////////////////////////////////////////////////////////////////

SvxXMLColorEntryExporter::SvxXMLColorEntryExporter( SvXMLExport& rExport )
: SvxXMLTableEntryExporter( rExport )
{
}

SvxXMLColorEntryExporter::~SvxXMLColorEntryExporter()
{
}

void SvxXMLColorEntryExporter::exportEntry( const OUString& rStrName, const Any& rValue )
{
    mrExport.AddAttribute( XML_NAMESPACE_DRAW, XML_NAME, rStrName );

    sal_Int32 nColor = 0;
    rValue >>= nColor;

    OUStringBuffer aOut;
    mrExport.GetMM100UnitConverter().convertColor( aOut, nColor );
    mrExport.AddAttribute( XML_NAMESPACE_DRAW, XML_COLOR, aOut.makeStringAndClear() );

    SvXMLElementExport aElem( mrExport, XML_NAMESPACE_DRAW, XML_COLOR, sal_True, sal_True );
}

///////////////////////////////////////////////////////////////////////

SvxXMLLineEndEntryExporter::SvxXMLLineEndEntryExporter( SvXMLExport& rExport )
: SvxXMLTableEntryExporter( rExport ), maMarkerStyle( rExport )
{
}

SvxXMLLineEndEntryExporter::~SvxXMLLineEndEntryExporter()
{
}

void SvxXMLLineEndEntryExporter::exportEntry( const OUString& rStrName, const Any& rValue )
{
    maMarkerStyle.exportXML( rStrName, rValue );
}

///////////////////////////////////////////////////////////////////////

SvxXMLDashEntryExporter::SvxXMLDashEntryExporter( SvXMLExport& rExport )
: SvxXMLTableEntryExporter( rExport ), maDashStyle( rExport )
{
}

SvxXMLDashEntryExporter::~SvxXMLDashEntryExporter()
{
}

void SvxXMLDashEntryExporter::exportEntry( const OUString& rStrName, const Any& rValue )
{
    maDashStyle.exportXML( rStrName, rValue );
}

///////////////////////////////////////////////////////////////////////

SvxXMLHatchEntryExporter::SvxXMLHatchEntryExporter( SvXMLExport& rExport )
: SvxXMLTableEntryExporter( rExport ), maHatchStyle( rExport )
{
}

SvxXMLHatchEntryExporter::~SvxXMLHatchEntryExporter()
{
}

void SvxXMLHatchEntryExporter::exportEntry( const OUString& rStrName, const Any& rValue )
{
    maHatchStyle.exportXML( rStrName, rValue );
}

///////////////////////////////////////////////////////////////////////

SvxXMLGradientEntryExporter::SvxXMLGradientEntryExporter( SvXMLExport& rExport )
: SvxXMLTableEntryExporter( rExport ), maGradientStyle( rExport )
{
}

SvxXMLGradientEntryExporter::~SvxXMLGradientEntryExporter()
{
}

void SvxXMLGradientEntryExporter::exportEntry( const OUString& rStrName, const Any& rValue )
{
    maGradientStyle.exportXML( rStrName, rValue );
}

///////////////////////////////////////////////////////////////////////

SvxXMLBitmapEntryExporter::SvxXMLBitmapEntryExporter( SvXMLExport& rExport )
: SvxXMLTableEntryExporter( rExport )
{
}

SvxXMLBitmapEntryExporter::~SvxXMLBitmapEntryExporter()
{
}

void SvxXMLBitmapEntryExporter::exportEntry( const OUString& rStrName, const Any& rValue )
{
    maImageStyle.exportXML( rStrName, rValue, mrExport );
}
