#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: globals.pm,v $
#
#   $Revision: 1.7 $
#
#   last change: $Author: obo $ $Date: 2005-12-21 12:49:37 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************


package par2script::globals;

############################################
# Global settings
############################################

BEGIN
{
    $prog="par2script";

    $includepathlist = "";
    $scriptname = "";
    $parfilelistorig = "";
    $parfilelist = "";

    @allitems = ("Installation", "ScpAction", "HelpText", "Directory", "DataCarrier", "StarRegistry", "File",
                 "Shortcut", "Custom", "Procedure", "Module", "Profile", "ProfileItem",
                 "Folder", "FolderItem", "RegistryItem", "StarRegistryItem", "WindowsCustomAction");

    @items_with_moduleid = ("Profile", "ProfileItem",
                             "FolderItem", "RegistryItem", "StarRegistryItem");

    $logging = 0;
    $logfilename = "logfile.log";   # the default logfile name for global errors
    @logfileinfo = ();

    $multidefinitionerror = 0;
    @multidefinitiongids = ();

    $plat = $^O;

    if (( $plat =~ /MSWin/i ) || (( $plat =~ /cygwin/i ) && ( $ENV{'USE_SHELL'} eq "4nt" )))
    {
        $separator = "\\";
        $pathseparator = "\;";
        $isunix = 0;
        $iswin = 1;
    }
    else
    {
        $separator = "/";
        $pathseparator = "\:";
        $isunix = 1;
        $iswin = 0;
    }

    $islinux = 0;
    $issolaris = 0;

    if ( $plat =~ /linux/i ) { $islinux = 1; }
    if ( $plat =~ /solaris/i ) { $issolaris = 1; }

}

1;
