<!--
	$Id: drawing.mod,v 1.16 2000-12-05 18:00:47 cl Exp $

   The Contents of this file are made available subject to the terms of
   either of the following licenses

          - GNU Lesser General Public License Version 2.1
          - Sun Industry Standards Source License Version 1.1

   Sun Microsystems Inc., October, 2000

   GNU Lesser General Public License Version 2.1
   =============================================
   Copyright 2000 by Sun Microsystems, Inc.
   901 San Antonio Road, Palo Alto, CA 94303, USA

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License version 2.1, as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
   MA  02111-1307  USA


   Sun Industry Standards Source License Version 1.1
   =================================================
   The contents of this file are subject to the Sun Industry Standards
   Source License Version 1.1 (the "License"); You may not use this file
   except in compliance with the License. You may obtain a copy of the
   License at http://www.openoffice.org/license.html.

   Software provided under this License is provided on an "AS IS" basis,
   WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
   WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
   See the License for the specific provisions governing your rights and
   obligations concerning the Software.

   The Initial Developer of the Original Code is: Sun Microsystems, Inc..

   Copyright: 2000 by Sun Microsystems, Inc.

   All Rights Reserved.

   Contributor(s): _______________________________________

-->

<!ENTITY % points "CDATA" >
<!ENTITY % pathData "CDATA" >
<!ENTITY % gradient-style "(linear|axial|radial|ellipsoid|square|rectangular)" >
<!ENTITY % draw-position "svg:x %coordinate; #IMPLIED svg:y %coordinate; #IMPLIED">
<!ENTITY % draw-end-position "table:end-cell-address %cell-address; #IMPLIED table:end-x %coordinate; #IMPLIED table:end-y %coordinate; #IMPLIED">
<!ENTITY % draw-size "svg:width %coordinate; #IMPLIED svg:height %coordinate; #IMPLIED">
<!ENTITY % draw-transform "svg:transform CDATA #IMPLIED">
<!ENTITY % draw-viewbox "svg:viewBox CDATA #REQUIRED">
<!ENTITY % draw-style-name "draw:style-name %styleName; #IMPLIED presentation:style-name %styleName; #IMPLIED">
<!ENTITY % shapeId "CDATA" >
<!ENTITY % draw-text "(text:p|text:unordered-list|text:ordered-list)*">
<!ENTITY % zindex "draw:z-index %nonNegativeInteger; #IMPLIED">
<!ENTITY % distance "CDATA">

<!-- commont presentation shape attributes -->
<!ENTITY % presentation-style-name "presentation:style-name %styleName; #IMPLIED">
<!ENTITY % presentation-classes "(title|outline|subtitle|text|graphic|object|chart|table|orgchart|page|notes)" >
<!-- ENTITY % presentation-class "presentation:class %presentation-classes; #IMPLIED" -->
<!ENTITY % presentation-class "presentation:class %presentation-classes; #IMPLIED presentation:placeholder (true|false) #IMPLIED presentation:user-transformed (true|false) #IMPLIED">

<!-- Drawing shapes -->
<!ELEMENT draw:rect %draw-text;>
<!ATTLIST draw:rect %draw-position; >
<!ATTLIST draw:rect %draw-end-position; >
<!ATTLIST draw:rect %draw-size; >
<!ATTLIST draw:rect %draw-style-name; >
<!ATTLIST draw:rect %draw-transform; >
<!ATTLIST draw:rect draw:corner-radius %nonNegativeLength; #IMPLIED>
<!ATTLIST draw:rect %zindex;>

<!ELEMENT draw:line %draw-text;>
<!ATTLIST draw:line svg:x1 %length; #REQUIRED>
<!ATTLIST draw:line svg:y1 %length; #REQUIRED>
<!ATTLIST draw:line svg:x2 %length; #REQUIRED>
<!ATTLIST draw:line svg:y2 %length; #REQUIRED>
<!ATTLIST draw:line %draw-style-name; >
<!ATTLIST draw:line %draw-transform; >
<!ATTLIST draw:line %zindex;>
<!ATTLIST draw:line %draw-end-position; >

<!ELEMENT draw:polyline %draw-text; >
<!ATTLIST draw:polyline %draw-position; >
<!ATTLIST draw:polyline %draw-size; >
<!ATTLIST draw:polyline %draw-viewbox; >
<!ATTLIST draw:polyline svg:points %points; #REQUIRED>
<!ATTLIST draw:polyline %draw-style-name; >
<!ATTLIST draw:polyline %draw-transform; >
<!ATTLIST draw:polyline %zindex;>
<!ATTLIST draw:polyline %draw-end-position; >

<!ELEMENT draw:polygon %draw-text; >
<!ATTLIST draw:polygon %draw-position; >
<!ATTLIST draw:polygon %draw-end-position; >
<!ATTLIST draw:polygon %draw-size; >
<!ATTLIST draw:polygon %draw-viewbox; >
<!ATTLIST draw:polygon svg:points %points; #REQUIRED >
<!ATTLIST draw:polygon %draw-style-name; >
<!ATTLIST draw:polygon %draw-transform; >
<!ATTLIST draw:polygon %zindex;>

<!ELEMENT draw:path %draw-text; >
<!ATTLIST draw:path %draw-position;>
<!ATTLIST draw:path %draw-end-position; >
<!ATTLIST draw:path %draw-size; >
<!ATTLIST draw:path %draw-viewbox; >
<!ATTLIST draw:path svg:d %pathData; #REQUIRED >
<!ATTLIST draw:path %draw-style-name; >
<!ATTLIST draw:path %draw-transform; >
<!ATTLIST draw:path %zindex;>

<!ELEMENT draw:circle %draw-text; >
<!ATTLIST draw:circle svg:cx %length; #REQUIRED >
<!ATTLIST draw:circle svg:cy %length; #REQUIRED >
<!ATTLIST draw:circle svg:r %nonNegativeLength; #REQUIRED >
<!ATTLIST draw:circle %draw-style-name; >
<!ATTLIST draw:circle %draw-transform; >
<!ATTLIST draw:circle %zindex;>
<!ATTLIST draw:circle %draw-end-position; >

<!ELEMENT draw:ellipse %draw-text; >
<!ATTLIST draw:ellipse svg:cx %length; #REQUIRED >
<!ATTLIST draw:ellipse svg:cy %length; #REQUIRED >
<!ATTLIST draw:ellipse svg:rx %length; #REQUIRED >
<!ATTLIST draw:ellipse svg:ry %length; #REQUIRED >
<!ATTLIST draw:ellipse %draw-style-name; >
<!ATTLIST draw:ellipse %draw-transform; >
<!ATTLIST draw:ellipse %zindex;>
<!ATTLIST draw:ellipse %draw-end-position; >

<!ELEMENT draw:connector %draw-text;>
<!ATTLIST draw:connector draw:type (standard|lines|line|curve) "standard">
<!ATTLIST draw:connector draw:line-skew CDATA #IMPLIED>
<!ATTLIST draw:connector %draw-style-name;>
<!ATTLIST draw:connector svg:x1 %coordinate; #REQUIRED>
<!ATTLIST draw:connector svg:y1 %coordinate; #REQUIRED>
<!ATTLIST draw:connector svg:x2 %coordinate; #REQUIRED>
<!ATTLIST draw:connector svg:y2 %coordinate; #REQUIRED>
<!ATTLIST draw:connector draw:start-shape %shapeId; #IMPLIED>
<!ATTLIST draw:connector draw:start-glue-point %integer; #IMPLIED>
<!ATTLIST draw:connector draw:end-shape %shapeId; #IMPLIED>
<!ATTLIST draw:connector draw:end-glue-point %integer; #IMPLIED>
<!ATTLIST draw:connector %zindex;>
<!ATTLIST draw:connector %draw-end-position; >

<!ELEMENT draw:g (%shapes;)* >
<!ATTLIST draw:g %draw-transform; >
<!ATTLIST draw:g %zindex;>
<!ATTLIST draw:g %draw-end-position; >

<!ELEMENT draw:page-thumbnail EMPTY>
<!ATTLIST draw:page-thumbnail %draw-position; >
<!ATTLIST draw:page-thumbnail %draw-size; >
<!ATTLIST draw:page-thumbnail %draw-style-name; >
<!ATTLIST draw:page-thumbnail %presentation-class; >
<!ATTLIST draw:page-thumbnail %zindex;>
<!ATTLIST draw:page-thumbnail %draw-end-position; >

<!ELEMENT draw:caption %draw-text;>
<!ATTLIST draw:caption %draw-position; >
<!ATTLIST draw:caption %draw-end-position; >
<!ATTLIST draw:caption %draw-size; >
<!ATTLIST draw:caption %draw-style-name; >
<!ATTLIST draw:caption %draw-transform; >
<!ATTLIST draw:caption draw:caption-point-x %coordinate; #IMPLIED>
<!ATTLIST draw:caption draw:caption-point-y %coordinate; #IMPLIED>
<!ATTLIST draw:caption %zindex;>

<!ELEMENT draw:measure %draw-text;>
<!ATTLIST draw:measure svg:x1 %coordinate; #REQUIRED>
<!ATTLIST draw:measure svg:y1 %coordinate; #REQUIRED>
<!ATTLIST draw:measure svg:x2 %coordinate; #REQUIRED>
<!ATTLIST draw:measure svg:y2 %coordinate; #REQUIRED>
<!ATTLIST draw:measure %draw-end-position; >
<!ATTLIST draw:measure %draw-style-name; >
<!ATTLIST draw:measure %draw-transform; >
<!ATTLIST draw:measure %zindex;>

<!-- graphic style elements -->
<!ELEMENT draw:gradient EMPTY >
<!ATTLIST draw:gradient draw:name %styleName; #REQUIRED>
<!ATTLIST draw:gradient draw:style %gradient-style; #REQUIRED>
<!ATTLIST draw:gradient draw:cx %coordinate; #IMPLIED>
<!ATTLIST draw:gradient draw:cy %coordinate; #IMPLIED>
<!ATTLIST draw:gradient draw:start-color %color; #IMPLIED>
<!ATTLIST draw:gradient draw:end-color %color; #IMPLIED>
<!ATTLIST draw:gradient draw:start-intensity %percentage; #IMPLIED>
<!ATTLIST draw:gradient draw:end-intensity %percentage; #IMPLIED>
<!ATTLIST draw:gradient draw:angle %integer; #IMPLIED>
<!ATTLIST draw:gradient draw:border %percentage; #IMPLIED>

<!ELEMENT draw:hatch EMPTY >
<!ATTLIST draw:hatch draw:name %styleName; #REQUIRED>
<!ATTLIST draw:hatch draw:style (single|double|triple) #REQUIRED >
<!ATTLIST draw:hatch draw:color %color; #IMPLIED>
<!ATTLIST draw:hatch draw:distance %length; #IMPLIED>
<!ATTLIST draw:hatch draw:rotation %integer; #IMPLIED>

<!ELEMENT draw:fill-image EMPTY >
<!ATTLIST draw:fill-image draw:name %styleName; #REQUIRED>
<!ATTLIST draw:fill-image xlink:href %uriReference; #REQUIRED>
<!ATTLIST draw:fill-image xlink:type (simple) #IMPLIED>
<!ATTLIST draw:fill-image xlink:show (embed) #IMPLIED>
<!ATTLIST draw:fill-image xlink:actuate (onLoad) #IMPLIED>
<!ATTLIST draw:fill-image svg:width %length; #IMPLIED>
<!ATTLIST draw:fill-image svg:height %length; #IMPLIED>

<!ELEMENT draw:transparency EMPTY>
<!ATTLIST draw:transparency draw:name %styleName; #REQUIRED>
<!ATTLIST draw:transparency draw:style %gradient-style; #REQUIRED>
<!ATTLIST draw:transparency svg:cx %coordinate; #IMPLIED>
<!ATTLIST draw:transparency svg:cy %coordinate; #IMPLIED>
<!ATTLIST draw:transparency draw:start %percentage; #IMPLIED>
<!ATTLIST draw:transparency draw:end %percentage; #IMPLIED>
<!ATTLIST draw:transparency draw:angle %integer; #IMPLIED>
<!ATTLIST draw:transparency draw:border %percentage; #IMPLIED>

<!ELEMENT draw:marker EMPTY>
<!ATTLIST draw:marker draw:name %styleName; #REQUIRED>
<!ATTLIST draw:marker %draw-viewbox; >
<!ATTLIST draw:marker svg:d %pathData; #REQUIRED>

<!ELEMENT draw:stroke-dash EMPTY>
<!ATTLIST draw:stroke-dash draw:name %styleName; #REQUIRED>
<!ATTLIST draw:stroke-dash draw:style (rect|round) #IMPLIED>
<!ATTLIST draw:stroke-dash draw:dots1 %integer; #IMPLIED>
<!ATTLIST draw:stroke-dash draw:dots1-length %length; #IMPLIED>
<!ATTLIST draw:stroke-dash draw:dots2 %integer; #IMPLIED>
<!ATTLIST draw:stroke-dash draw:dots2-length %length; #IMPLIED>
<!ATTLIST draw:stroke-dash draw:distance %length; #IMPLIED>

<!-- stroke attributes -->
<!ATTLIST style:properties draw:stroke (none|dash|solid) #IMPLIED>
<!ATTLIST style:properties draw:stroke-dash CDATA #IMPLIED>
<!ATTLIST style:properties svg:stroke-width %length; #IMPLIED>
<!ATTLIST style:properties svg:stroke-color %color; #IMPLIED>
<!ATTLIST style:properties draw:marker-start %styleName; #IMPLIED>
<!ATTLIST style:properties draw:marker-end %styleName; #IMPLIED>
<!ATTLIST style:properties draw:marker-start-width %length; #IMPLIED>
<!ATTLIST style:properties draw:marker-end-width %length; #IMPLIED>
<!ATTLIST style:properties draw:marker-start-center %boolean; #IMPLIED>
<!ATTLIST style:properties draw:marker-end-center %boolean; #IMPLIED>
<!ATTLIST style:properties svg:stroke-opacity %floatOrPercentage; #IMPLIED>
<!ATTLIST style:properties svg:stroke-linejoin (miter|round|bevel|middle|none|inherit) #IMPLIED>

<!-- fill attributes -->
<!ATTLIST style:properties draw:fill (none|solid|bitmap|gradient|hatch) #IMPLIED>
<!ATTLIST style:properties draw:fill-color %color; #IMPLIED>
<!ATTLIST style:properties draw:fill-gradient-name %styleName; #IMPLIED>
<!ATTLIST style:properties draw:gradient-step-count CDATA #IMPLIED>
<!ATTLIST style:properties draw:fill-hatch-name %styleName; #IMPLIED>
<!ATTLIST style:properties draw:fill-image-name %styleName; #IMPLIED>
<!ATTLIST style:properties style:repeat (no-repeat|repeat|stretch) #IMPLIED>
<!ATTLIST style:properties draw:fill-image-width %length; #IMPLIED>
<!ATTLIST style:properties draw:fill-image-height %length; #IMPLIED>
<!ATTLIST style:properties draw:refX %percentage; #IMPLIED>
<!ATTLIST style:properties draw:refY %percentage; #IMPLIED>
<!ATTLIST style:properties draw:tile-repeat-offset CDATA #IMPLIED>
<!ENTITY % noneOrFloatOrPercentage "CDATA">
<!ATTLIST style:properties draw:transparency %percentage; #IMPLIED>
<!ATTLIST style:properties draw:transparency-name %styleName; #IMPLIED>

<!-- graphic attributes -->
<!ATTLIST style:properties draw:color-mode (greyscale|mono|watermark|standard) #IMPLIED>
<!ATTLIST style:properties draw:luminance %percentage; #IMPLIED>
<!ATTLIST style:properties draw:contrast %percentage; #IMPLIED>
<!ATTLIST style:properties draw:gamma %percentage; #IMPLIED>
<!ATTLIST style:properties draw:red %percentage; #IMPLIED>
<!ATTLIST style:properties draw:green %percentage; #IMPLIED>
<!ATTLIST style:properties draw:blue %percentage; #IMPLIED>

<!-- shadow attributes -->
<!ATTLIST style:properties draw:shadow (visible|hidden) #IMPLIED>
<!ATTLIST style:properties draw:shadow-offset-x %length; #IMPLIED>
<!ATTLIST style:properties draw:shadow-offset-y %length; #IMPLIED>
<!ATTLIST style:properties draw:shadow-color %color; #IMPLIED>
<!ATTLIST style:properties draw:shadow-transparency CDATA #IMPLIED>

<!-- connector attributes -->
<!ATTLIST style:properties draw:start-line-spacing-horizontal %distance; #IMPLIED>
<!ATTLIST style:properties draw:start-line-spacing-vertical %distance; #IMPLIED>
<!ATTLIST style:properties draw:end-line-spacing-horizontal %distance; #IMPLIED>
<!ATTLIST style:properties draw:end-line-spacing-vertical %distance; #IMPLIED>

<!-- measure attributes -->
<!ATTLIST style:properties draw:line-distance %distance; #IMPLIED>
<!ATTLIST style:properties draw:guide-overhang %distance; #IMPLIED>
<!ATTLIST style:properties draw:guide-distance %distance; #IMPLIED>
<!ATTLIST style:properties draw:start-guide %distance; #IMPLIED>
<!ATTLIST style:properties draw:end-guide %distance; #IMPLIED>
<!ATTLIST style:properties draw:placing (below|atop) #IMPLIED>
<!ATTLIST style:properties draw:parallel %boolean; #IMPLIED>
<!ATTLIST style:properties draw:text-position-vertical (left|center|right|auto) #IMPLIED>
<!ATTLIST style:properties draw:text-position-horizontal (top|center|bottom|auto) #IMPLIED>

<!-- Drawing page -->
<!ELEMENT draw:page ((%shapes;)*,presentation:notes?)>
<!ATTLIST draw:page draw:name %string; #IMPLIED>
<!ATTLIST draw:page draw:style-name %styleName; #IMPLIED>
<!ATTLIST draw:page draw:master-page-name %styleName; #REQUIRED>
<!ATTLIST draw:page presentation:presentation-page-layout-name %styleName; #IMPLIED>

<!-- Presentation notes -->
<!ELEMENT presentation:notes (%shapes;)*>


<!-- presentation page layouts -->
<!ELEMENT style:presentation-page-layout (presentation:placeholder)* >
<!ATTLIST style:presentation-page-layout style:name %styleName; #REQUIRED>
<!ELEMENT presentation:placeholder EMPTY >
<!ATTLIST presentation:placeholder presentation:object (title|outline|subtitle|text|graphic|object|chart|orgchart|page|notes|handout) #REQUIRED>
<!ATTLIST presentation:placeholder svg:x %coordinateOrPercentage; #REQUIRED>
<!ATTLIST presentation:placeholder svg:y %coordinateOrPercentage; #REQUIRED>
<!ATTLIST presentation:placeholder svg:width %lengthOrPercentage; #REQUIRED>
<!ATTLIST presentation:placeholder svg:height %lengthOrPercentage; #REQUIRED>

<!-- presentation page attributes -->
<!ATTLIST style:properties presentation:transition-type (manual|automatic|semi-automatic) #IMPLIED >
<!ATTLIST style:properties presentation:transition-style (none|fade-from-left|fade-from-top|fade-from-right|fade-from-bottom|fade-to-center|fade-from-center|move-from-left|move-from-top|move-from-right|move-from-bottom|roll-from-left|roll-from-right|roll-from-bottom|vertical-stripes|horizontal-stripes|clockwise|counterclockwise|fade-from-upperleft|fade-from-upperright|fade-from-lowerleft|fade-from-lowerright|close-vertical|close-horizontal|open-vertical|open-horizontal|spiralin-left|spiralin-right|spiralout-left|spiralout-right|dissolve|wavyline-from-left|wavyline-from-top|wavyline-from-right|wavyline-from-bottom|random|stretch-from-left|stretch-from-top|stretch-from-right|stretch-from-bottom|vertical-lines|horizontal-lines) #IMPLIED >
<!ATTLIST style:properties presentation:transition-speed (slow|medium|fast) #IMPLIED >
<!ATTLIST style:properties presentation:duration %timeDuration; #IMPLIED>
<!ATTLIST style:properties presentation:visibility (visible|hidden) #IMPLIED>

<!ELEMENT presentation:sound EMPTY>
<!ATTLIST presentation:sound xlink:href %uriReference; #REQUIRED>
<!ATTLIST presentation:sound xlink:type (simple) #IMPLIED>
<!ATTLIST presentation:sound xlink:show (embed) #IMPLIED>
<!ATTLIST presentation:sound xlink:actuate (onLoad) #IMPLIED>

<!-- text boxes -->
<!ELEMENT draw:text-box (text:h|text:p|text:ordered-list|
		text:unordered-list|table:table|chart:chart|
		draw:a|draw:text-box|draw:image)*>
<!ATTLIST draw:text-box %draw-style-name;>
<!ATTLIST draw:text-box draw:name %string; #IMPLIED>
<!ATTLIST draw:text-box draw:chain-next-name %string; #IMPLIED>

<!ATTLIST draw:text-box text:anchor-type %anchorType; #IMPLIED>
<!ATTLIST draw:text-box text:anchor-page-number %positiveInteger; #IMPLIED>
<!ATTLIST draw:text-box %draw-position;>
<!ATTLIST draw:text-box svg:width %lengthOrPercentage; #IMPLIED>
<!ATTLIST draw:text-box svg:height %lengthOrPercentage; #IMPLIED>
<!ATTLIST draw:text-box fo:min-height %lengthOrPercentage; #IMPLIED>
<!ATTLIST draw:text-box %zindex;>
<!ATTLIST draw:text-box %presentation-class; >
<!ATTLIST draw:text-box %draw-transform; >

<!-- image -->
<!ELEMENT draw:image (svg:desc?)>
<!ATTLIST draw:image %draw-style-name;>
<!ATTLIST draw:image draw:name %string; #IMPLIED>
<!ATTLIST draw:image xlink:href %uriReference; #REQUIRED>
<!ATTLIST draw:image xlink:type (simple) #FIXED "simple">
<!ATTLIST draw:image xlink:show (embed) "embed">
<!ATTLIST draw:image xlink:actuate (onLoad) "onLoad">
<!ATTLIST draw:image draw:filter-name %string; #IMPLIED>
<!ATTLIST draw:image text:anchor-type %anchorType; #IMPLIED>
<!ATTLIST draw:image text:anchor-page-number %positiveInteger; #IMPLIED>
<!ATTLIST draw:image %draw-position;>
<!ATTLIST draw:image svg:width %lengthOrPercentage; #IMPLIED>
<!ATTLIST draw:image svg:height %lengthOrPercentage; #IMPLIED>
<!ATTLIST draw:image %presentation-class; >
<!ATTLIST draw:image %zindex;>

<!ELEMENT svg:desc (#PCDATA)>

<!-- hyperlink -->
<!ELEMENT draw:a (draw:image|draw:text-box)>
<!ATTLIST draw:a xlink:href %uriReference; #REQUIRED>
<!ATTLIST draw:a xlink:type (simple) #FIXED "simple">
<!ATTLIST draw:a xlink:show (new|replace) #IMPLIED>
<!ATTLIST draw:a xlink:actuate (onRequest) "onRequest">
<!ATTLIST draw:a office:name %string; #IMPLIED>
<!ATTLIST draw:a office:target-frame-name %string; #IMPLIED>
<!ATTLIST draw:a office:server-map %boolean; "false">
