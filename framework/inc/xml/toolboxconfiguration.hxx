#ifndef __FRAMEWORK_XML_TOOLBOXCONFIGURATION_HXX_
#define __FRAMEWORK_XML_TOOLBOXCONFIGURATION_HXX_

#ifndef _SVARRAY_HXX
#include <svtools/svarray.hxx>
#endif
#ifndef _SV_BITMAP_HXX
#include <vcl/bitmap.hxx>
#endif
#ifndef _STRING_HXX
#include <tools/string.hxx>
#endif
#ifndef _STREAM_HXX
#include <tools/stream.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX
#include <vcl/toolbox.hxx>
#endif
#ifndef _SV_TOOLBOX_HXX
#include <vcl/toolbox.hxx>
#endif

// #110897#
#ifndef _COM_SUN_STAR_LANG_XMULTISERVICEFACTORY_HPP_
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#endif

namespace framework
{

struct ToolBoxItemDescriptor
{
    Bitmap* pBmp;                       // Bitmap ptr not used by xml configuration
    String  aBitmapName;                // bitmap name => use to find correct bmp file
    String  aItemText;                  // label for this toolbox item
    String  aURL;                       // URL command to dispatch
    USHORT  nId;                        // internal id not used by xml configuration
    USHORT  nItemBits;                  // properties for this toolbox item (WinBits)
    USHORT  nItemType;                  // toolbox item type (BUTTON, SPACE, BREAK, SEPARATOR)
    USHORT  nVisible;                   // toolbox item visible?
    USHORT  nWidth;                     // width of a toolbox window (edit field, etc.)
    USHORT  nUserDef;                   // user defined toolbox item (1=yes/0=no)
    String  aHelpId;                    // A help id associated with this toolbox item

    public:

        ToolBoxItemDescriptor() : pBmp( 0 )
                                 ,nId( 0 )
                                 ,nItemBits( 0 )
                                 ,nItemType( (USHORT)TOOLBOXITEM_SPACE )
                                 ,nVisible( sal_True )
                                 ,nWidth( 0 )
                                 ,nUserDef( sal_False ) {}
};

typedef ToolBoxItemDescriptor* ToolBoxItemDescriptorPtr;
SV_DECL_PTRARR_DEL( ToolBoxDescriptor, ToolBoxItemDescriptorPtr, 10, 2)

struct ToolBoxLayoutItemDescriptor
{
    String                      aName;          // Unique name of the toolbox ( Objectbar, Toolbar etc. )
    String                      aUserName;      // Userspecified name for this toolbar
    Point                       aFloatingPos;   // Position in floating mode
    USHORT                      nFloatingLines; // Number of lines in floating mode
    USHORT                      nLines;         // Number of lines in docking mode
    WindowAlign                 eAlign;         // Aligned position in docking mode
    BOOL                        bVisible;       // Visible or not
    BOOL                        bFloating;      // Floating mode on/off
    ButtonType                  eType;          // text, symbol or text+symbol

    ToolBoxLayoutItemDescriptor() : nFloatingLines( 0 )
                                    ,nLines( 1 )
                                    ,eAlign( WINDOWALIGN_LEFT )
                                    ,bVisible( sal_False )
                                    ,bFloating( sal_False )
                                    ,eType( BUTTON_SYMBOL ) {}
};

typedef ToolBoxLayoutItemDescriptor* ToolBoxLayoutItemDescriptorPtr;
SV_DECL_PTRARR_DEL( ToolBoxLayoutDescriptor, ToolBoxLayoutItemDescriptorPtr, 10, 2)

class ToolBoxConfiguration
{
    public:
        // #110897#
        static sal_Bool LoadToolBox(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
            SvStream& rInStream, ToolBoxDescriptor& aItems );

        // #110897#
        static sal_Bool StoreToolBox(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
            SvStream& rOutStream, const ToolBoxDescriptor& aItems );

        // #110897#
        static sal_Bool LoadToolBoxLayout(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
            SvStream& rInStream, ToolBoxLayoutDescriptor& aItems );

        // #110897#
        static sal_Bool StoreToolBoxLayout(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& xServiceFactory,
            SvStream& rOutStream, ToolBoxLayoutDescriptor& aItems );
};

} // namespace framework

#endif // __FRAMEWORK_XML_TOOLBOXCONFIGURATION_HXX_
