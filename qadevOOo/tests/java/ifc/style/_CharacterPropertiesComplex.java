/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: _CharacterPropertiesComplex.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:01:18 $
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

package ifc.style;

import lib.MultiPropertyTest;

/**
* Testing <code>com.sun.star.style.CharacterPropertiesComplex</code>
* service properties :
* <ul>
*  <li><code> CharHeightComplex</code></li>
*  <li><code> CharWeightComplex</code></li>
*  <li><code> CharFontNameComplex</code></li>
*  <li><code> CharFontStyleNameComplex</code></li>
*  <li><code> CharFontFamilyComplex</code></li>
*  <li><code> CharFontCharSetComplex</code></li>
*  <li><code> CharFontPitchComplex</code></li>
*  <li><code> CharPostureComplex</code></li>
*  <li><code> CharLocaleComplex</code></li>
* </ul> <p>
* Properties testing is automated by <code>lib.MultiPropertyTest</code>.
* @see com.sun.star.style.CharacterPropertiesComplex
*/
public class _CharacterPropertiesComplex extends MultiPropertyTest {

    public void _CharWeightComplex() {
        testProperty("CharWeightComplex", new Float(com.sun.star.awt.FontWeight.BOLD),
            new Float(com.sun.star.awt.FontWeight.THIN)) ;
    }

    public void _CharPostureComplex() {
        testProperty("CharPostureComplex", com.sun.star.awt.FontSlant.ITALIC,
            com.sun.star.awt.FontSlant.NONE) ;
    }

    /**
    * Forces environment recreation.
    */
    protected void after() {
        disposeEnvironment();
    }

} //finish class _CharacterPropertiesComplex

