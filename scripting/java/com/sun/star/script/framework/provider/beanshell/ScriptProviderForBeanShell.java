/*************************************************************************
*
*  $RCSfile: ScriptProviderForBeanShell.java,v $
*
*  $Revision: 1.4 $
*
*  last change: $Author: svesik $ $Date: 2004-04-19 23:11:14 $
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
package com.sun.star.script.framework.provider.beanshell;

import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XMultiServiceFactory;
import com.sun.star.lang.XSingleServiceFactory;
import com.sun.star.registry.XRegistryKey;
import com.sun.star.comp.loader.FactoryHelper;
import com.sun.star.lang.XTypeProvider;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XInitialization;
import com.sun.star.uno.AnyConverter;
import com.sun.star.uno.Type;
import com.sun.star.uno.Any;

import com.sun.star.beans.XPropertySet;

import com.sun.star.lang.IllegalArgumentException;
import com.sun.star.lang.WrappedTargetException;
import com.sun.star.reflection.InvocationTargetException;
import com.sun.star.script.CannotConvertException;

import java.util.Properties;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.Map;
import java.io.*;

import java.net.URL;
import java.net.MalformedURLException;
import java.net.URLDecoder;

import bsh.BshClassManager;
import bsh.Interpreter;

import drafts.com.sun.star.script.provider.XScriptContext;
import drafts.com.sun.star.script.provider.XScriptProvider;
import drafts.com.sun.star.script.provider.XScript;

import com.sun.star.script.framework.provider.*;
import com.sun.star.script.framework.log.*;
import com.sun.star.script.framework.container.ScriptMetaData;

public class ScriptProviderForBeanShell
{
    public static class _ScriptProviderForBeanShell extends ScriptProvider
    {
        private static ScriptEditorForBeanShell myScriptEditorForBeanShell;

        public _ScriptProviderForBeanShell(XComponentContext ctx)
        {
            super (ctx, "BeanShell");
        }

        public XScript getScript( /*IN*/String scriptURI )
            throws com.sun.star.uno.RuntimeException,
                   com.sun.star.lang.IllegalArgumentException
        {
            ScriptMetaData scriptData = getScriptData( scriptURI );
            if ( scriptData == null )
            {
                throw new com.sun.star.uno.RuntimeException(
                    "Cannot find script for URI: " + scriptURI );
            }
            else
            {
                ScriptImpl script = new ScriptImpl( m_xContext, scriptData, m_xInvocationContext );
                return script;
            }
        }

        public boolean hasScriptEditor()
        {
            return true;
        }

        public ScriptEditor getScriptEditor()
        {
            return ScriptEditorForBeanShell.getEditor();
        }
    }

    /**
     * Returns a factory for creating the service.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     *
     * @param  implName      the name of the implementation for which a service is desired
     * @param  multiFactory  the service manager to be used if needed
     * @param  regKey        the registryKey
     * @return               returns a <code>XSingleServiceFactory</code> for creating
     *                          the component
     * @see                  com.sun.star.comp.loader.JavaLoader
     */
    public static XSingleServiceFactory __getServiceFactory( String implName,
            XMultiServiceFactory multiFactory,
            XRegistryKey regKey )
    {
        XSingleServiceFactory xSingleServiceFactory = null;

        if ( implName.equals( ScriptProviderForBeanShell._ScriptProviderForBeanShell.class.getName() ) )
        {
            xSingleServiceFactory = FactoryHelper.getServiceFactory(
                ScriptProviderForBeanShell._ScriptProviderForBeanShell.class,
                "drafts.com.sun.star.script.ScriptProviderForBeanShell",
                multiFactory,
                regKey );
        }

        return xSingleServiceFactory;
    }


    /**
     * Writes the service information into the given registry key.
     * This method is called by the <code>JavaLoader</code>
     * <p>
     *
     * @param  regKey  the registryKey
     * @return         returns true if the operation succeeded
     * @see            com.sun.star.comp.loader.JavaLoader
     */
    public static boolean __writeRegistryServiceInfo( XRegistryKey regKey )
    {
        String impl = "com.sun.star.scripting.runtime.beanshell." +
            "ScriptProviderForBeanShell$_ScriptProviderForBeanShell";

        String service = "drafts.com.sun.star.script.provider." +
            "ScriptProviderForBeanShell";

        if (FactoryHelper.writeRegistryServiceInfo(impl, service, regKey)) {
            try {
                // code below is commented out because we want this to happen
                // as part of the install, this will have to be done
                // programatically during the install as registration of java components
                // is not fully supported in setup. It should work somewhat like c++ somewhat like c++ registration in install
/*                XRegistryKey newKey = regKey.createKey(impl + "/UNO/SINGLETONS/drafts.com.sun.star.script.provider.theScriptProviderForBeanShell");
                newKey.setStringValue(service); */
                return true;
            }
            catch (Exception ex) {
                System.err.println(
                    "Error registering ScriptProviderForBeanShell: " + ex);
            }
        }
        return false;
    }
}

class ScriptImpl implements XScript
{
    private ScriptMetaData metaData;
    private XComponentContext m_xContext;
    private XMultiComponentFactory m_xMultiComponentFactory;
    private Object m_oInvokeContext;

    ScriptImpl( XComponentContext ctx, ScriptMetaData metaData, Object oInvokeContext ) throws com.sun.star.uno.RuntimeException
    {
        this.metaData = metaData;
        this.m_xContext = ctx;
        this.m_oInvokeContext = oInvokeContext;

        try
        {
            this.m_xMultiComponentFactory = m_xContext.getServiceManager();
        }
        catch ( Exception e )
        {
            LogUtils.DEBUG( LogUtils.getTrace( e ) );
            throw new com.sun.star.uno.RuntimeException(
                "Error constructing  ScriptImpl [beanshell]: "
                + e.getMessage() );
        }

        LogUtils.DEBUG("ScriptImpl [beanshell] script data = " + metaData );
    }
        /**
         *                          documentStorageID and document reference
         *                          for use in script name resolving
         *
         * @param aParams           All parameters; pure, out params are
         *                          undefined in sequence, i.e., the value
         *                          has to be ignored by the callee
         *
         * @param aOutParamIndex    Out indices
         *
         * @param aOutParam         Out parameters
         *
         * @returns                 The value returned from the function
         *                          being invoked
         *
         * @throws IllegalArgumentException If there is no matching script name
         *
         * @throws CannotConvertException   If args do not match or cannot
         *                                  be converted the those of the
         *                                  invokee
         *
         * @throws InvocationTargetException If the running script throws
         *                                   an exception this information
         *                                   is captured and rethrown as
         *                                   this exception type.
         */

        public Object invoke( /*IN*/Object[] aParams,
                            /*OUT*/short[][] aOutParamIndex,
                            /*OUT*/Object[][] aOutParam )
            throws IllegalArgumentException, CannotConvertException,
                InvocationTargetException
        {
            // Initialise the out paramters - not used at the moment
            aOutParamIndex[0] = new short[0];
            aOutParam[0] = new Object[0];


            ClassLoader cl = null;
            try {
                cl = ClassLoaderFactory.getURLClassLoader( metaData );
            }
            catch (Exception e)
            {
                throw new InvocationTargetException(e.getMessage());
            }
            Interpreter interpreter = new Interpreter();

            interpreter.getNameSpace().clear();
            interpreter.setClassLoader(cl);

            try {
                interpreter.set("context",
                    ScriptContext.createContext(m_oInvokeContext,
                        m_xContext, m_xMultiComponentFactory));

                interpreter.set("ARGUMENTS", aParams);
            }
            catch (bsh.EvalError e) {
                throw new InvocationTargetException(e.getMessage());
            }

            try {
                String source = null;
                Object result = null;

                ScriptEditorForBeanShell editor =
                    ScriptEditorForBeanShell.getEditor(
                        metaData.getSourceURL() );

                if (editor != null && editor.isModified())
                {
                    source = editor.getText();
                }
                else
                {
                    source = metaData.getSource();

                }

                if ( source == null || source.length() == 0 )
                    throw new InvocationTargetException(
                        "Couldn't read script: " + metaData.getSourceURL()  );

                result = interpreter.eval( source );

                if (result == null)
                    return new Any(new Type(), null);
                return result;
            }
            catch (Exception ex) {
                throw new InvocationTargetException(ex.getMessage());
            }
        }

}
