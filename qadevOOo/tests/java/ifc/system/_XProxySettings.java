/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

package ifc.system;

import java.util.Map;

import lib.MultiMethodTest;
import lib.Status;
import lib.StatusException;

import com.sun.star.system.XProxySettings;

/**
 * Tests <code>com.sun.star.system.XProxySettings</code> interface. The result
 * of each method is compared with expected settings which is specified by the
 * caller of the tests via object relation "XProxySettings.proxaSettings". That
 * should be a HashMap containing the following keys:
 * <ul>
 *   <li>ftpProxyAddress</li>
 *   <li>ftpProxyPort</li>
 *   <li>gopherProxyAddress</li>
 *   <li>gopherProxyPort</li>
 *   <li>httpProxyAddress</li>
 *   <li>httpProxyPort</li>
 *   <li>httpsProxyAddress</li>
 *   <li>httpsProxyPort</li>
 *   <li>socksProxyAddress</li>
 *   <li>socksProxyPort</li>
 *   <li>proxyBypassAddress</li>
 *   <li>proxyEnabled</li>
 * </ul>.
 * Each value for a key, should be a String specifying the correct result of
 * the corresponding getXXX() method of XProxySettings interface.
 * <p>If the object relation is not specified the test failes.
 *
 */
public class _XProxySettings extends MultiMethodTest {
    public XProxySettings oObj;

    /**
     * Contains a HashMap with correct results of the tested methods. See
     * the class description.
     */
    Map expectedProxies;

    /**
     * Checks that the "XProxySettings.proxySettings" object relation is
     * specified and stores its value to <code>expectedProxies</code> field.
     *
     * @throws StatusException is the object relation is not specified.
     *
     * @see #expectedProxies
     */
    public void before() {
        expectedProxies = (Map)tEnv.getObjRelation(
                "XProxySettings.proxySettings");

        if (expectedProxies == null) {
            throw new StatusException(
                    Status.failed("Expected proxy settings are not specified"));
        }
    }

    /**
     * Calls <code>getFtpProxyAddress()</code> and verifies its result with
     * "ftpProxyAddress" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getFtpProxyAddress() {
        String ftpProxyAddress = oObj.getFtpProxyAddress();
        String expectedFtpProxyAddress = (String)
                expectedProxies.get("ftpProxyAddress");

        log.println("getFtpProxyAddress");
        log.println("getting: "+ftpProxyAddress);
        log.println("expected: "+expectedFtpProxyAddress);

        tRes.tested("getFtpProxyAddress()",
                ftpProxyAddress.equals(expectedFtpProxyAddress));
    }

    /**
     * Calls <code>getFtpProxyPort()</code> and verifies its result with
     * "ftpProxyPort" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getFtpProxyPort() {
        String ftpProxyPort = oObj.getFtpProxyPort();
        String expectedFtpProxyPort = (String)
                expectedProxies.get("ftpProxyPort");

        log.println("getFtpProxyPort");
        log.println("getting: "+ftpProxyPort);
        log.println("expected: "+expectedFtpProxyPort);

        tRes.tested("getFtpProxyPort()",
                ftpProxyPort.equals(expectedFtpProxyPort));
    }

    /**
     * Calls <code>getGopherProxyAddress()</code> and verifies its result with
     * "gopherProxyAddress" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getGopherProxyAddress() {
        String gopherProxyAddress = oObj.getGopherProxyAddress();
        String expectedGopherProxyAddress = (String)
                expectedProxies.get("gopherProxyAddress");

        tRes.tested("getGopherProxyAddress()",
                gopherProxyAddress.equals(expectedGopherProxyAddress));
    }

    /**
     * Calls <code>getGopherProxyPort()</code> and verifies its result with
     * "gopherProxyPort" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getGopherProxyPort() {
        String gopherProxyPort = oObj.getGopherProxyPort();
        String expectedGopherProxyPort = (String)
                expectedProxies.get("gopherProxyPort");

        tRes.tested("getGopherProxyPort()",
                gopherProxyPort.equals(expectedGopherProxyPort));
    }

    /**
     * Calls <code>getHttpProxyAddress()</code> and verifies its result with
     * "httpProxyAddress" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getHttpProxyAddress() {
        String httpProxyAddress = oObj.getHttpProxyAddress();
        String expectedHttpProxyAddress = (String)
                expectedProxies.get("httpProxyAddress");

        log.println("getHttpProxyAddress");
        log.println("getting: "+httpProxyAddress);
        log.println("expected: "+expectedHttpProxyAddress);

        tRes.tested("getHttpProxyAddress()",
                httpProxyAddress.equals(expectedHttpProxyAddress));
    }

    /**
     * Calls <code>getHttpProxyPort()</code> and verifies its result with
     * "httpProxyPort" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getHttpProxyPort() {
        String httpProxyPort = oObj.getHttpProxyPort();
        String expectedHttpProxyPort = (String)
                expectedProxies.get("httpProxyPort");

        log.println("getHttpProxyPort");
        log.println("getting: "+httpProxyPort);
        log.println("expected: "+expectedHttpProxyPort);

        tRes.tested("getHttpProxyPort()",
                httpProxyPort.equals(expectedHttpProxyPort));
    }

    /**
     * Calls <code>getHttpsProxyAddress()</code> and verifies its result with
     * "httpsProxyAddress" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getHttpsProxyAddress() {
        String httpsProxyAddress = oObj.getHttpsProxyAddress();
        String expectedHttpsProxyAddress = (String)
                expectedProxies.get("httpsProxyAddress");

        log.println("getHttpsProxyAddress");
        log.println("getting: "+httpsProxyAddress);
        log.println("expected: "+expectedHttpsProxyAddress);

        tRes.tested("getHttpsProxyAddress()",
                httpsProxyAddress.equals(expectedHttpsProxyAddress));
    }

    /**
     * Calls <code>getHttpsProxyPort()</code> and verifies its result with
     * "httpsProxyPort" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getHttpsProxyPort() {
        String httpsProxyPort = oObj.getHttpsProxyPort();
        String expectedHttpsProxyPort = (String)
                expectedProxies.get("httpsProxyPort");

        log.println("getHttpsProxyPort");
        log.println("getting: "+httpsProxyPort);
        log.println("expected: "+expectedHttpsProxyPort);

        tRes.tested("getHttpsProxyPort()",
                httpsProxyPort.equals(expectedHttpsProxyPort));
    }

    /**
     * Calls <code>getProxyBypassAddress()</code> and verifies its result with
     * "proxyBypassAddress" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getProxyBypassAddress() {
        String proxyBypassAddress = oObj.getProxyBypassAddress();
        String expectedProxyBypassAddress = (String)
                expectedProxies.get("proxyBypassAddress");

        log.println("getProxyBypassAddress");
        log.println("getting: "+proxyBypassAddress);
        log.println("expected: "+expectedProxyBypassAddress);

        tRes.tested("getProxyBypassAddress()",
                proxyBypassAddress.equals(expectedProxyBypassAddress));
    }

    /**
     * Calls <code>getSocksProxyAddress()</code> and verifies its result with
     * "socksProxyAddress" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getSocksProxyAddress() {
        String socksProxyAddress = oObj.getSocksProxyAddress();
        String expectedSocksProxyAddress = (String)
                expectedProxies.get("socksProxyAddress");

        log.println("getSocksProxyAddress");
        log.println("getting: "+socksProxyAddress);
        log.println("expected: "+expectedSocksProxyAddress);

        tRes.tested("getSocksProxyAddress()",
                socksProxyAddress.equals(expectedSocksProxyAddress));
    }

    /**
     * Calls <code>getSocksProxyPort()</code> and verifies its result with
     * "socksProxyPort" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _getSocksProxyPort() {
        String socksProxyPort = oObj.getSocksProxyPort();
        String expectedSocksProxyPort = (String)
                expectedProxies.get("socksProxyPort");

        log.println("getSocksProxyPort");
        log.println("getting: "+socksProxyPort);
        log.println("expected: "+expectedSocksProxyPort);

        tRes.tested("getSocksProxyPort()",
                socksProxyPort.equals(expectedSocksProxyPort));
    }

    /**
     * Calls <code>isProxyEnabled()</code> and verifies its result with
     * "proxyEnabled" value of <code>expectedProxies</code>.
     *
     * @see #expectedProxies
     */
    public void _isProxyEnabled() {
        boolean proxyEnabled = oObj.isProxyEnabled();

        String proxyEnabledStr = (String)expectedProxies.get("proxyEnabled");
        boolean expected = proxyEnabledStr != null
                && proxyEnabledStr.equalsIgnoreCase("true");

        tRes.tested("isProxyEnabled()", proxyEnabled == expected);
    }
}
