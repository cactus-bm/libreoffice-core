/*************************************************************************
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Copyright 2008 by Sun Microsystems, Inc.
 *
 * OpenOffice.org - a multi-platform office productivity suite
 *
 * $RCSfile: BuildEnvTools.java,v $
 *
 * $Revision: 1.2 $
 *
 * This file is part of OpenOffice.org.
 *
 * OpenOffice.org is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenOffice.org is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenOffice.org.  If not, see
 * <http://www.openoffice.org/license.html>
 * for a copy of the LGPLv3 License.
 *
 ************************************************************************/
package helper;

import java.io.File;
import java.io.PrintWriter;
import lib.TestParameters;
import share.LogWriter;
import util.*;

/**
 * This class support you to execute some shell commands in a buld environment. At ervery call of commands
 * a build environment was created and the commands will be executed.
 *
 */
public class BuildEnvTools {

    private final TestParameters param;
    private final LogWriter log;
    private final boolean mDebug;
    private final String mEnvSet;
    private final String mPlatform;
    private final String mShell;
    private final String mVersion;

    /**
     * This constructor creates an instance of BuildEncTools. It is verifying for all neccesarry
     * parameters in <CODE>TestParameters</CODE> This must be:
     * <ul>
     * <li>OOO_EnvSet: This parameter must ponit to the script to create a build environment like
     *     '$SRC_ROOT/SolarisX86Env.Set.sh' or '%SRC_ROOT\\winenv.set.sh'.
            * In case you are able to use \'setsolar\' or \'setcws\' just type \'setsolar\' value.
     * </li>
     * <li>OperatingSystem: Fill this parameter with an operating system like unxsols, unxsoli, unxlngi or wntmsci.
     * </li>
     * <li> Shell: Fill this parameter with a shell which can start 'OOO_ENVSET' f.e '/bin/tcsh'
     *      or 'c:\\myShell\\myShell.exe'
     * </li>
     * <li>Version: You have to fill this parameter with information of the version
     *              to test like 'src680_m243' or 'cws_unoapi
     * </li>
     * <li>COMP_ENV: You have to fill this parameter with information of the version of the compiler like
     *               unxsols4, unxsoli4, unxlngi6 oder wintmsci12
     * @param param
     * @param log
     * @throws helper.ParameterNotFoundException
     */
    public BuildEnvTools(TestParameters param, LogWriter log) throws ParameterNotFoundException {
        this.param = param;
        this.log = log;
        mDebug = param.getBool(PropertyName.DEBUG_IS_ACTIVE);

        boolean error = false;

        String msg = "\nERROR: the following parameter must be set before executing the test:\n\n";

        mEnvSet = (String) param.get(PropertyName.OOO_ENVSET);
        if (mEnvSet == null) {
            msg += PropertyName.OOO_ENVSET + "\nThis parameter must ponit to the script to create a build " +
                "environment like '$SRC_ROOT/SolarisX86Env.Set.sh' or '%SRC_ROOT\\winenv.set.sh'\n" +
                "In case you are able to use \'setsolar\' or \'setcws\' just type \'setsolar\' value.\n\n ";
            error = true;
        }

        mPlatform = (String) param.get(PropertyName.OPERATING_SYSTEM);
        if (mDebug) {
            log.println("### " + mPlatform);
        }
        if ((mPlatform == null) || ((!mPlatform.equals(PropertyName.UNXSOLS)) &&
            (!mPlatform.equalsIgnoreCase(PropertyName.UNXSOLI)) && (!mPlatform.equals(PropertyName.UNXLNGI)) &&
            (!mPlatform.equals(PropertyName.WNTMSCI)))) {

            msg += PropertyName.OPERATING_SYSTEM + "\nFill this parameter with an operating system like unxsols," +
                " unxsoli, unxlngi or wntmsci.  \n\n";
            error = true;
        }

        mShell = (String) param.get(PropertyName.SHELL);
        if (mShell == null) {
            msg += PropertyName.SHELL + "\nFill this parameter with a shell which can start 'OOO_ENVSET'" +
                "\n\t/bin/tcsh c:\\myShell\\myShell.exe\n\n";
            error = true;
        }

        mVersion = (String) param.get(PropertyName.VERSION);
        if (mVersion == null) {
            msg += PropertyName.VERSION + "\nYou have to fill this parameter with information of the version" +
                "to test like 'src680_m243' or 'cws_unoapi'";
            error = true;
        }

        final String mCompiler = (String) param.get(PropertyName.COMP_ENV);
        if (mCompiler == null) {
            msg += PropertyName.COMP_ENV + "\nYou have to fill this parameter with information of the version" +
                "of the compiler like unxsols4, unxsoli4, unxlngi6 oder wintmsci12";
            error = true;
        }


        if (error) {
            throw new ParameterNotFoundException(msg);
        }
    }

    /**
     * Executes the given commands in OOo-Environment shell.
     * @param commands
     * @param workDir
     * @param shortWait
     * @return the processHandler of the commands
     * @see helper.ProcessHandler
     */
    public ProcessHandler runCommandsInEnvironmentShell(String[] commands, File workDir, boolean shortWait) {

        final String[] cmdLines = getCmdLinesWithCommand(commands);
        final ProcessHandler pHdl = new ProcessHandler(cmdLines, (PrintWriter) log, workDir, shortWait, param);
        pHdl.runCommand();
        return pHdl;
    }

    private String getSetSolarCmd() {
        String cmd = null;

        if (mVersion.startsWith("cws_")) {
            cmd = "setcws " + mVersion.substring(4, mVersion.length());
        } else {
            final String mCompiler = (String) param.get(PropertyName.COMP_ENV);

            // mVersion looks like src680_m243
            final String[] versions = mVersion.split("_");

            cmd = "setsolar -" + versions[0] + " -ver " + versions[1] + " -jdk14  -pro " + mCompiler;
        }
        return cmd;
    }

    public String getEnvCmd() {

        String cmd = null;

        log.println("prepare command for platform " + mPlatform);

        if (mEnvSet.equals("setsolar")) {
            cmd = getSetSolarCmd();
//            if (mPlatform.equals(PropertyName.WNTMSCI)) {
//            //cmd = "call " + cmd;
//            }
        } else {
            cmd = "source " + mEnvSet;
        }
        return cmd;
    }

    public String getSrcRoot() {

        String sSrcRoot = (String) param.get(PropertyName.SRC_ROOT);

        if (sSrcRoot == null) {
            final String cmd = getEnvCmd();
            String[] cmdLines = null;

            if (mPlatform.equals(PropertyName.WNTMSCI)) {
                cmdLines = new String[]{mShell, "/C", "" + cmd + " ^ echo SRC_ROOT=%SRC_ROOT"};
            } else {
                cmdLines = new String[]{mShell, "-c ", cmd + " ; echo \"SRC_ROOT=$SRC_ROOT\""};
            }

            final ProcessHandler procHdl = new ProcessHandler(cmdLines, (PrintWriter) log, null, true, param);
            procHdl.runCommand();

            if (mDebug) {
                log.println("---> Output of command:");
                log.println(procHdl.getOutputText());
                log.println("<--- Output of command:");
                log.println("---> Error output of command");
                log.println(procHdl.getErrorText());
                log.println("<--- Error output of command");
            }
            final String output = procHdl.getOutputText();
            final String[] outs = output.split("\n");

            for (int i = 0; i < outs.length; i++) {
                final String line = outs[i];
                if (line.startsWith("SRC_ROOT")) {
                    sSrcRoot = getEnvValue(line);
                }
            }
        }
        return sSrcRoot;
    }

    private String[] getCmdLinesWithCommand(String[] commands) {

        final String envcmd = getEnvCmd();
        String[] cmdLines = null;
        log.println("prepare command for platform " + mPlatform);

        String seperator = "";
        if (mPlatform.equals(PropertyName.WNTMSCI)) {
            seperator = "^";
        } else {
            seperator = ";";
        }

        String command = "";
        for (int i = 0; i < commands.length; i++) {
            command += seperator + commands[i];
        }

        if (mPlatform.equals(PropertyName.WNTMSCI)) {
            cmdLines = new String[]{mShell, "/C", "\"" + envcmd + command + "\""};
        } else {
            cmdLines = new String[]{mShell, "-c ", envcmd + command};
        }
        return cmdLines;
    }

    private String getEnvValue(String line) {
        final String[] split = line.split("=");
        return split[1];
    }
}
