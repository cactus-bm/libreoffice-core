:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#*************************************************************************
#
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
#
# Copyright 2008 by Sun Microsystems, Inc.
#
# OpenOffice.org - a multi-platform office productivity suite
#
# $RCSfile: config.pl,v $
#
# $Revision: 1.4 $
#
# This file is part of OpenOffice.org.
#
# OpenOffice.org is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3
# only, as published by the Free Software Foundation.
#
# OpenOffice.org is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License version 3 for more details
# (a copy is included in the LICENSE file that accompanied this code).
#
# You should have received a copy of the GNU Lesser General Public License
# version 3 along with OpenOffice.org.  If not, see
# <http://www.openoffice.org/license.html>
# for a copy of the LGPLv3 License.
#
#*************************************************************************

use File::Path;
use File::Copy;

### globals ###

$is_debug = 0;

$gui = $ENV{GUI};
$cygwin = "cygwin";

if ($^O =~ /cygwin/) {
    $gui = $cygwin;
}

if (($gui eq "UNX") or ($gui eq $cygwin)) {
    $pathslash = "/";
}
else
{
    $pathslash = "\\";
}

#### Hardly coded path for
# \share\registry\data\org\openoffice\Office\Jobs.xcu

$jobs_XML_Path = "share" . $pathslash  . "registry" . $pathslash . "data" . $pathslash . "org" . $pathslash . "openoffice" . $pathslash . "Office" .  $pathslash . "Jobs.xcu";

# \user\registry\data\org\openoffice\

$User_Path = "user" . $pathslash  . "registry" . $pathslash . "data" . $pathslash . "org" . $pathslash . "openoffice" . $pathslash;

# \user\registry\data\org\openoffice\Office\

$User_Office_Path = $User_Path . "Office" . $pathslash;


$common_XML = "Common.xcu";
$setup_XML = "Setup.xcu";

### main ###

$idStr = ' $Revision: 1.4 $ ';
$idStr =~ /Revision:\s+(\S+)\s+\$/
  ? ($cpflat2minor_rev = $1) : ($cpflat2minor_rev = "-");

if ( ($#ARGV >= 2) ) {
    $ARGV[0] =~ s/\"//g;
    $ARGV[1] =~ s/\"//g;
    $ARGV[2] =~ s/\"//g;
    chop($ARGV[0]);
    chop($ARGV[1]);
    chop($ARGV[2]);
}

if ( ! ( ($#ARGV >= 2) && $ARGV[0] && $ARGV[1] && $ARGV[2] && (-d $ARGV[2]) ) ) {
    print "Usage: config <installpath> <userinstallpath> <datapath>\n" ;
    exit(1);
}

$installdir = $ARGV[0];
$userinstalldir = $ARGV[1];
$datapath = $ARGV[2];

$fullquickstart_path = $installdir . "program" . $pathslash . "quickstart.exe";

print "patching config ... \n";
if (!-d "$userinstalldir$User_Office_Path") {
    mkpath("$userinstalldir$User_Office_Path", 0, 0777);
}

# copy Common.xcu

print "cp $datapath$common_XML $userinstalldir$User_Office_Path$common_XML\n" if $is_debug;
copy ("$datapath$common_XML", "$userinstalldir$User_Office_Path$common_XML");

# copy Setup.xcu

print "cp $datapath$setup_XML $userinstalldir$User_Path$setup_XML\n" if $is_debug;
copy ("$datapath$setup_XML", "$userinstalldir$User_Path$setup_XML");


# quickstarter loeschen
if ($gui ne "UNX") {
    print "kill $fullquickstart_path \n";
    unlink( $fullquickstart_path ) or die "cannot unlink $fullquickstart_path";
}

#delete joblist
$fullsource_path = $installdir . $jobs_XML_Path;
unlink ($fullsource_path);

exit(0);

