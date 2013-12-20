/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_OSL_DETAIL_COMPONENT_DECLARATIONS_H
#define INCLUDED_OSL_DETAIL_COMPONENT_DECLARATIONS_H

#ifdef DISABLE_DYNLOADING

#ifdef __cplusplus
extern "C" {
#endif

void * analysis_component_getFactory( const char * , void * , void * );
void * animcore_component_getFactory( const char * , void * , void * );
void * avmedia_component_getFactory( const char * , void * , void * );
void * basprov_component_getFactory( const char * , void * , void * );
void * bootstrap_component_getFactory( const char * , void * , void * );
void * chartcore_component_getFactory( const char * , void * , void * );
void * comphelp_component_getFactory( const char * , void * , void * );
void * configmgr_component_getFactory( const char * , void * , void * );
void * cui_component_getFactory( const char * , void * , void * );
void * date_component_getFactory( const char * , void * , void * );
void * dba_component_getFactory( const char * , void * , void * );
void * dbaxml_component_getFactory( const char * , void * , void * );
void * deployment_component_getFactory( const char * , void * , void * );
void * dlgprov_component_getFactory( const char * , void * , void * );
void * embobj_component_getFactory( const char * , void * , void * );
void * emboleobj_component_getFactory( const char * , void * , void * );
void * evtatt_component_getFactory( const char * , void * , void * );
void * filterconfig1_component_getFactory( const char * , void * , void * );
void * frm_component_getFactory( const char * , void * , void * );
void * fsstorage_component_getFactory( const char * , void * , void * );
void * fwk_component_getFactory( const char * , void * , void * );
void * fwl_component_getFactory( const char * , void * , void * );
void * fwm_component_getFactory( const char * , void * , void * );
void * hwp_component_getFactory( const char * , void * , void * );
void * hyphen_component_getFactory( const char * , void * , void * );
void * i18npool_component_getFactory( const char * , void * , void * );
void * i18nsearch_component_getFactory( const char * , void * , void * );
void * introspection_component_getFactory( const char * , void * , void * );
void * lng_component_getFactory( const char * , void * , void * );
void * lnth_component_getFactory( const char * , void * , void * );
void * localebe1_component_getFactory( const char * , void * , void * );
void * oox_component_getFactory( const char * , void * , void * );
void * package2_component_getFactory( const char * , void * , void * );
void * pricing_component_getFactory( const char * , void * , void * );
void * protocolhandler_component_getFactory( const char * , void * , void * );
void * reflection_component_getFactory( const char * , void * , void * );
void * sb_component_getFactory( const char * , void * , void * );
void * sc_component_getFactory( const char * , void * , void * );
void * scd_component_getFactory( const char * , void * , void * );
void * scfilt_component_getFactory( const char * , void * , void * );
void * scriptframe_component_getFactory( const char * , void * , void * );
void * sd_component_getFactory( const char * , void * , void * );
void * sdd_component_getFactory( const char * , void * , void * );
void * sfx_component_getFactory( const char * , void * , void * );
void * sm_component_getFactory( const char * , void * , void * );
void * smd_component_getFactory( const char * , void * , void * );
void * sot_component_getFactory( const char * , void * , void * );
void * spell_component_getFactory( const char * , void * , void * );
void * spl_component_getFactory( const char * , void * , void * );
void * stocservices_component_getFactory( const char * , void * , void * );
void * stringresource_component_getFactory( const char * , void * , void * );
void * svgfilter_component_getFactory( const char * , void * , void * );
void * svl_component_getFactory( const char * , void * , void * );
void * svt_component_getFactory( const char * , void * , void * );
void * svx_component_getFactory( const char * , void * , void * );
void * svxcore_component_getFactory( const char * , void * , void * );
void * sw_component_getFactory( const char * , void * , void * );
void * swd_component_getFactory( const char * , void * , void * );
void * t602filter_component_getFactory( const char * , void * , void * );
void * textfd_component_getFactory( const char * , void * , void * );
void * tk_component_getFactory( const char * , void * , void * );
void * ucb_component_getFactory( const char * , void * , void * );
void * ucpexpand1_component_getFactory( const char * , void * , void * );
void * ucpfile_component_getFactory( const char * , void * , void * );
void * ucppkg1_component_getFactory( const char * , void * , void * );
void * unordf_component_getFactory( const char * , void * , void * );
void * unoxml_component_getFactory( const char * , void * , void * );
void * utl_component_getFactory( const char * , void * , void * );
void * uui_component_getFactory( const char * , void * , void * );
void * vbaevents_component_getFactory( const char * , void * , void * );
void * vbaswobj_component_getFactory( const char * , void * , void * );
void * vcl_component_getFactory( const char * , void * , void * );
void * wpftdraw_component_getFactory( const char * , void * , void * );
void * wpftwriter_component_getFactory( const char * , void * , void * );
void * writerfilter_component_getFactory( const char * , void * , void * );
void * xmlfd_component_getFactory( const char * , void * , void * );
void * xmlsecurity_component_getFactory( const char * , void * , void * );
void * xo_component_getFactory( const char * , void * , void * );
void * xof_component_getFactory( const char * , void * , void * );
void * xstor_component_getFactory( const char * , void * , void * );

void * com_sun_star_comp_extensions_xml_sax_FastParser( void * , void * );
void * com_sun_star_comp_extensions_xml_sax_ParserExpat( void * , void * );
void * com_sun_star_comp_stoc_DLLComponentLoader( void * , void * );
void * com_sun_star_comp_stoc_ImplementationRegistration( void * , void * );
void * com_sun_star_comp_stoc_NestedRegistry( void * , void * );
void * com_sun_star_comp_stoc_ORegistryServiceManager( void * , void * );
void * com_sun_star_comp_stoc_OServiceManager( void * , void * );
void * com_sun_star_comp_stoc_OServiceManagerWrapper( void * , void * );
void * com_sun_star_comp_stoc_SimpleRegistry( void * , void * );
void * com_sun_star_extensions_xml_sax_Writer( void * , void * );
void * com_sun_star_security_comp_stoc_AccessController( void * , void * );
void * com_sun_star_security_comp_stoc_FilePolicy( void * , void * );
void * com_sun_star_comp_Draw_GraphicExporter_implementation_getFactory( void * , void * );
void * com_sun_star_comp_Svx_GraphicExportHelper_implementation_getFactory( void * , void * );
void * com_sun_star_comp_Svx_GraphicImportHelper_implementation_getFactory( void * , void * );
void * com_sun_star_comp_gallery_GalleryThemeProvider_implementation_getFactory( void * , void * );
void * com_sun_star_comp_graphic_PrimitiveFactory2D_implementation_getFactory( void * , void * );
void * com_sun_star_comp_svx_Impl_FindbarDispatcher_implementation_getFactory( void * , void * );
void * com_sun_star_comp_svx_RecoveryUI_implementation_getFactory( void * , void * );
void * com_sun_star_drawing_EnhancedCustomShapeEngine_implementation_getFactory( void * , void * );
void * com_sun_star_drawing_SvxShapeCollection_implementation_getFactory( void * , void * );
void * com_sun_star_drawing_SvxUnoColorTable_implementation_getFactory( void * , void * );
void * com_sun_star_svx_DownSearchToolboxController_implementation_getFactory( void * , void * );
void * com_sun_star_svx_FindTextToolboxController_implementation_getFactory( void * , void * );
void * com_sun_star_svx_MatchCaseToolboxController_implementation_getFactory( void * , void * );
void * com_sun_star_svx_FindAllToolboxController_implementation_getFactory( void * , void * );
void * com_sun_star_svx_FontHeightToolBoxController_implementation_getFactory( void * , void * );
void * com_sun_star_svx_UpSearchToolboxController_implementation_getFactory( void * , void * );
void * com_sun_star_svx_ExitFindbarToolboxController_implementation_getFactory( void * , void * );
void * org_apache_openoffice_comp_svx_sidebar_PanelFactory_implementation_getFactory( void * , void * );

#ifdef __cplusplus
}
#endif

#endif /* DISABLE_DYNLOADING */

#endif // INCLUDED_OSL_DETAIL_COMPONENT_DECLARATIONS_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
