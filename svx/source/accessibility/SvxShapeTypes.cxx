/*************************************************************************
 *
 *  $RCSfile: SvxShapeTypes.cxx,v $
 *
 *  $Revision: 1.10 $
 *
 *  last change: $Author: rt $ $Date: 2004-12-03 14:12:31 $
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

#include "SvxShapeTypes.hxx"

#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_SHAPE_HXX
#include "AccessibleShape.hxx"
#endif
#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_GRAPHIC_SHAPE_HXX
#include "AccessibleGraphicShape.hxx"
#endif
#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_OLE_SHAPE_HXX
#include "AccessibleOLEShape.hxx"
#endif
#ifndef _SVX_ACCESSIBILITY_ACCESSIBLE_CONTROL_SHAPE_HXX
#include "AccessibleControlShape.hxx"
#endif

namespace accessibility {

AccessibleShape* CreateSvxAccessibleShape (
    const AccessibleShapeInfo& rShapeInfo,
    const AccessibleShapeTreeInfo& rShapeTreeInfo,
    ShapeTypeId nId)
{
    switch (nId)
    {
        case DRAWING_3D_CUBE:
        case DRAWING_3D_EXTRUDE:
        case DRAWING_3D_LATHE:
//BFS01        case DRAWING_3D_POLYGON:
        case DRAWING_3D_SCENE:
        case DRAWING_3D_SPHERE:
        case DRAWING_CAPTION:
        case DRAWING_CLOSED_BEZIER:
        case DRAWING_CLOSED_FREEHAND:
        case DRAWING_CONNECTOR:
        case DRAWING_ELLIPSE:
        case DRAWING_GROUP:
        case DRAWING_LINE:
        case DRAWING_MEASURE:
        case DRAWING_OPEN_BEZIER:
        case DRAWING_OPEN_FREEHAND:
        case DRAWING_PAGE:
        case DRAWING_POLY_POLYGON:
        case DRAWING_POLY_LINE:
        case DRAWING_POLY_POLYGON_PATH:
        case DRAWING_POLY_LINE_PATH:
        case DRAWING_RECTANGLE:
        case DRAWING_TEXT:
        // --> OD 2004-11-29 #i37790# - default accessiblility shape for
        // com::sun::star::drawing::CustomShape
        case DRAWING_CUSTOM:
        // <--
            return new AccessibleShape (rShapeInfo, rShapeTreeInfo);

        case DRAWING_CONTROL:
            return new AccessibleControlShape (rShapeInfo, rShapeTreeInfo);

        case DRAWING_GRAPHIC_OBJECT:
            return new AccessibleGraphicShape (rShapeInfo, rShapeTreeInfo);

        case DRAWING_APPLET:
        case DRAWING_FRAME:
        case DRAWING_OLE:
        case DRAWING_PLUGIN:
            return new AccessibleOLEShape (rShapeInfo, rShapeTreeInfo);

        default:
            return NULL;
    }
}



ShapeTypeDescriptor aSvxShapeTypeList[] = {
    ShapeTypeDescriptor (DRAWING_TEXT,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.TextShape"),
        CreateSvxAccessibleShape),
    ShapeTypeDescriptor (DRAWING_RECTANGLE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.RectangleShape"),
        CreateSvxAccessibleShape),
    ShapeTypeDescriptor (   DRAWING_ELLIPSE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.EllipseShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_CONTROL,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.ControlShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_CONNECTOR,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.ConnectorShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_MEASURE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.MeasureShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_LINE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.LineShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_POLY_POLYGON,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.PolyPolygonShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_POLY_LINE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.PolyLineShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_OPEN_BEZIER,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.OpenBezierShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_CLOSED_BEZIER,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.ClosedBezierShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_OPEN_FREEHAND,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.OpenFreeHandShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_CLOSED_FREEHAND,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.ClosedFreeHandShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_POLY_POLYGON_PATH,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.PolyPolygonPathShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_POLY_LINE_PATH,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.PolyLinePathShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_GRAPHIC_OBJECT,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.GraphicObjectShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_GROUP,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.GroupShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_OLE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.OLE2Shape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_PAGE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.PageShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_CAPTION,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.CaptionShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_FRAME,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.FrameShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_PLUGIN,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.PluginShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_APPLET,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.AppletShape"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_3D_SCENE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.Shape3DSceneObject"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_3D_CUBE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.Shape3DCubeObject"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_3D_SPHERE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.Shape3DSphereObject"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_3D_LATHE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.Shape3DLatheObject"),
        CreateSvxAccessibleShape ),
    ShapeTypeDescriptor (   DRAWING_3D_EXTRUDE,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.Shape3DExtrudeObject"),
        CreateSvxAccessibleShape ),
//BFS01    ShapeTypeDescriptor (   DRAWING_3D_POLYGON,
//BFS01        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.Shape3DPolygonObject"),
//BFS01        CreateSvxAccessibleShape )
    // --> OD 2004-11-29 #i37790# - default accessiblility shape for
    // com::sun::star::drawing::CustomShape
    ShapeTypeDescriptor (   DRAWING_CUSTOM,
        ::rtl::OUString::createFromAscii ("com.sun.star.drawing.CustomShape"),
        CreateSvxAccessibleShape )
    // <--
};


void RegisterDrawShapeTypes (void)
{
    // --> OD 2004-11-26 #i37790#
    ShapeTypeHandler::Instance().AddShapeTypeList ( DRAWING_END, aSvxShapeTypeList);
    // <--
}


} // end of namespace accessibility
