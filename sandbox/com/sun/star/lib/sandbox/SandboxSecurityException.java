/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: SandboxSecurityException.java,v $
 *
 *  $Revision: 1.3 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-08 17:09:17 $
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

package com.sun.star.lib.sandbox;

/**
 * An applet security exception.
 *
 * @version     1.6, 11/23/96
 * @author  Arthur van Hoff
 */
public class SandboxSecurityException extends SecurityException {
    private String key = null;
    private Object msgobj[] = null;

    public SandboxSecurityException(String name) {
        super(name);
        this.key = name;
    }

    public SandboxSecurityException(String name, String arg) {
        this(name);
        msgobj = new Object[1];
        msgobj[0] = (Object)arg;
    }

    public SandboxSecurityException(String name, String arg1, String arg2) {
        this(name);
        msgobj = new Object[2];
        msgobj[0] = (Object)arg1;
        msgobj[1] = (Object)arg2;
    }


    //The loading of the static member  amh caused a deadlock:
    //Thread 1 holds a monitor on SandboxSecuritymethod and tries to get hold of
    //a lock of the class loader.
    //Thread 2 is the finalizer which holds the class loader monitor an needs the
    //SandboxSecurity monitor.
    //Therefor the method getLocalizedMessage and the member amh have been removed.
//    private static SandboxMessageHandler amh = new SandboxMessageHandler("appletsecurityexception");

}

