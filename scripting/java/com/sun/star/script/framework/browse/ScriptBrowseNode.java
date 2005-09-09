/*************************************************************************
 *
 *  OpenOffice.org - a multi-platform office productivity suite
 *
 *  $RCSfile: ScriptBrowseNode.java,v $
 *
 *  $Revision: 1.14 $
 *
 *  last change: $Author: rt $ $Date: 2005-09-09 01:56:07 $
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
package com.sun.star.script.framework.browse;

import com.sun.star.script.browse.XBrowseNode;
import com.sun.star.script.browse.BrowseNodeTypes;
import com.sun.star.script.provider.XScriptContext;

import com.sun.star.beans.PropertyAttribute;
import com.sun.star.lib.uno.helper.PropertySet;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Any;
import com.sun.star.uno.Type;
import com.sun.star.uno.XComponentContext;
import com.sun.star.uno.UnoRuntime;

import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.ucb.XSimpleFileAccess;
import com.sun.star.beans.XIntrospectionAccess;
import com.sun.star.script.XInvocation;

import com.sun.star.lang.NoSupportException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.reflection.InvocationTargetException;
import com.sun.star.container.NoSuchElementException;
import com.sun.star.container.ElementExistException;

import java.io.File;
import java.util.*;
import javax.swing.JOptionPane;

import com.sun.star.script.framework.log.LogUtils;
import com.sun.star.script.framework.provider.ScriptProvider;
import com.sun.star.script.framework.browse.DialogFactory;
import com.sun.star.script.framework.container.*;

public class ScriptBrowseNode extends PropertySet
    implements XBrowseNode, XInvocation
{
    private ScriptProvider provider;

    private Parcel parent;
    private String name;
    public String uri;
    public String description;
    public boolean editable  = false;
    public boolean deletable = false;
    public boolean renamable = false;

    public ScriptBrowseNode( ScriptProvider provider, Parcel parent,
        String name )
    {
        this.provider = provider;
        this.name = name;
        this.parent = parent;
        ScriptMetaData data = null;
        XSimpleFileAccess xSFA = null;
        XComponentContext xCtx = provider.getScriptingContext().getComponentContext();
        XMultiComponentFactory xFac = xCtx.getServiceManager();
        try
        {
            data = (ScriptMetaData)parent.getByName( name );
            xSFA = ( XSimpleFileAccess)
                UnoRuntime.queryInterface( XSimpleFileAccess.class,
                    xFac.createInstanceWithContext(
                        "com.sun.star.ucb.SimpleFileAccess",
                        xCtx ) );
        }

        // TODO fix exception types to be caught here, should we rethrow?
        catch (  Exception e )
        {
            LogUtils.DEBUG("** caught exception getting script data for " + name + " ->" + e.toString() );
        }

        uri = data.getShortFormScriptURL();
        description = data.getDescription();

        if (provider.hasScriptEditor() == true)
        {

            this.editable  = true;
            try
            {
                if ( !parent.isUnoPkg() &&
                    !xSFA.isReadOnly( parent.getPathToParcel() ) )
                {
                    this.deletable = true;
                    this.renamable = true;
                }
            }
            // TODO propagate errors
            catch ( Exception e )
            {
                LogUtils.DEBUG("Caught exception in creation of ScriptBrowseNode");
                LogUtils.DEBUG( LogUtils.getTrace(e));
            }

        }

        registerProperty("Deletable", new Type(boolean.class),
            (short)0, "deletable");
        registerProperty("Editable", new Type(boolean.class),
            (short)0, "editable");
        registerProperty("Renamable", new Type(boolean.class),
            (short)0, "renamable");
        registerProperty("URI", new Type(String.class),
            (short)0, "uri");
        registerProperty("Description", new Type(String.class),
            (short)0, "description");
    }


    public String getName() {
        return name;
    }

    public XBrowseNode[] getChildNodes() {
        return new XBrowseNode[0];
    }

    public boolean hasChildNodes() {
        return false;
    }

    public short getType() {
        return BrowseNodeTypes.SCRIPT;
    }

    public String toString() {
        return getName();
    }

    public void updateURI( Parcel p ) {
        parent = p;
        ScriptMetaData data = null;
        try
        {
            data = (ScriptMetaData)parent.getByName( name );
        }

        // TODO fix exception types to be caught here, should we rethrow?
        catch (  Exception e )
        {
            LogUtils.DEBUG("** caught exception getting script data for " + name + " ->" + e.toString() );
        }
        uri = data.getShortFormScriptURL();
    }
    // implementation of XInvocation interface
    public XIntrospectionAccess getIntrospection() {
        return null;
    }

    public Object invoke(String aFunctionName, Object[] aParams,
                         short[][] aOutParamIndex, Object[][] aOutParam)
        throws com.sun.star.lang.IllegalArgumentException,
               com.sun.star.script.CannotConvertException,
               com.sun.star.reflection.InvocationTargetException
    {
        // Initialise the out paramters - not used but prevents error in
        // UNO bridge
        aOutParamIndex[0] = new short[0];
        aOutParam[0] = new Object[0];

        Any result = new Any(new Type(Boolean.class), Boolean.TRUE);

        if (aFunctionName.equals("Editable"))
        {
            if (!editable)
            {
                NoSupportException nse = new NoSupportException(
                    aFunctionName + " is not editable " );

                throw new InvocationTargetException(
                    "Scripting framework error editing script", null, nse );
            }

            XScriptContext ctxt =  provider.getScriptingContext();
            ScriptMetaData data = null;
            try
            {
                data = (ScriptMetaData)parent.getByName( name );
            }
            catch ( NoSuchElementException nse )
            {
                throw new com.sun.star.lang.IllegalArgumentException(
                    name + " does not exist or can't be found " );
            }
            catch (  com.sun.star.lang.WrappedTargetException wte )
            {
                // rethrow
                throw new InvocationTargetException(
                    "Scripting framework editing script ",
                        null, wte.TargetException );
            }

            provider.getScriptEditor().edit(ctxt, data);
        }
        else if (aFunctionName.equals("Deletable"))
        {
            if (!deletable)
            {
                NoSupportException nse = new NoSupportException(
                    aFunctionName + " is not supported for this node" );

                throw new InvocationTargetException(
                      "Scripting framework error deleting script", null, nse );
            }
            try
            {
                parent.removeByName( name );
                result = new Any(new Type(Boolean.class), Boolean.TRUE);
            }
            catch ( NoSuchElementException nse )
            {
                throw new com.sun.star.lang.IllegalArgumentException(
                    name + " does not exist or can't be found " );
            }
            catch ( WrappedTargetException wte )
            {
                // rethrow
                throw new InvocationTargetException(
                    "Scripting framework deleting script ",
                        null, wte.TargetException );
            }

        }
        else if (aFunctionName.equals("Renamable"))
        {
            result = new Any(new Type(XBrowseNode.class), new XBrowseNode[0]);
            if (!renamable)
            {
                NoSupportException nse = new NoSupportException(
                    aFunctionName + " is not supported for this node" );

                throw new InvocationTargetException(
                    "Scripting framework error renaming script", null, nse );
            }

            try
            {
                String newName = (String) AnyConverter.toString(aParams[0]);
                ScriptMetaData oldData = (ScriptMetaData)parent.getByName( name );
                oldData.loadSource();
                String oldSource = oldData.getSource();

                LogUtils.DEBUG("Create renamed script");
                String languageName =
                    newName + "." + provider.getScriptEditor().getExtension();
                String language = provider.getName();

                ScriptEntry entry = new ScriptEntry(
                    language, languageName, languageName, "", new HashMap() );

                ScriptMetaData data = new ScriptMetaData(
                    parent, entry, oldSource );

                parent.insertByName( languageName, data );

                LogUtils.DEBUG("Now remove old script");
                parent.removeByName( name );

                uri = data.getShortFormScriptURL();
                name = languageName;
                result = new Any(new Type(XBrowseNode.class), this);
            }
            catch ( NoSuchElementException nse )
            {
                throw new com.sun.star.lang.IllegalArgumentException(
                    name + " does not exist or can't be found " );
            }
            catch ( ElementExistException eee )
            {
                // rethrow
                throw new InvocationTargetException(
                    "Scripting framework error renaming script ",
                        null, eee );
            }
            catch ( WrappedTargetException wte )
            {
                // rethrow
                throw new InvocationTargetException(
                    "Scripting framework rename script ",
                        null, wte.TargetException );
            }
        }
        else {
            throw new com.sun.star.lang.IllegalArgumentException(
                "Function " + aFunctionName + " not supported.");
        }

        return result;
    }

    public void setValue(String aPropertyName, Object aValue)
        throws com.sun.star.beans.UnknownPropertyException,
               com.sun.star.script.CannotConvertException,
               com.sun.star.reflection.InvocationTargetException
    {
    }

    public Object getValue(String aPropertyName)
        throws com.sun.star.beans.UnknownPropertyException
    {
        return null;
    }

    public boolean hasMethod(String aName) {
        return false;
    }

    public boolean hasProperty(String aName) {
        return false;
    }
}
