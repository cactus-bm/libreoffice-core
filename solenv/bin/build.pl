:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#*************************************************************************
#
#   $RCSfile: build.pl,v $
#
#   $Revision: 1.17 $
#
#   last change: $Author: vg $ $Date: 2001-05-08 14:04:20 $
#
#   The Contents of this file are made available subject to the terms of
#   either of the following licenses
#
#          - GNU Lesser General Public License Version 2.1
#          - Sun Industry Standards Source License Version 1.1
#
#   Sun Microsystems Inc., October, 2000
#
#   GNU Lesser General Public License Version 2.1
#   =============================================
#   Copyright 2000 by Sun Microsystems, Inc.
#   901 San Antonio Road, Palo Alto, CA 94303, USA
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License version 2.1, as published by the Free Software Foundation.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with this library; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#   MA  02111-1307  USA
#
#
#   Sun Industry Standards Source License Version 1.1
#   =================================================
#   The contents of this file are subject to the Sun Industry Standards
#   Source License Version 1.1 (the "License"); You may not use this file
#   except in compliance with the License. You may obtain a copy of the
#   License at http://www.openoffice.org/license.html.
#
#   Software provided under this License is provided on an "AS IS" basis,
#   WITHOUT WARRUNTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING,
#   WITHOUT LIMITATION, WARRUNTIES THAT THE SOFTWARE IS FREE OF DEFECTS,
#   MERCHANTABLE, FIT FOR A PARTICULAR PURPOSE, OR NON-INFRINGING.
#   See the License for the specific provisions governing your rights and
#   obligations concerning the Software.
#
#   The Initial Developer of the Original Code is: Sun Microsystems, Inc..
#
#   Copyright: 2000 by Sun Microsystems, Inc.
#
#   All Rights Reserved.
#
#   Contributor(s): _______________________________________
#
#
#
#*************************************************************************

#
# build - build entire project
#

use Cwd;

#### script id #####

( $script_name = $0 ) =~ s/^.*\b(\w+)\.pl$/$1/;

$id_str = ' $Revision: 1.17 $ ';
$id_str =~ /Revision:\s+(\S+)\s+\$/
  ? ($script_rev = $1) : ($script_rev = "-");

print "$script_name -- version: $script_rev\n";

#########################
#                       #
#   Globale Variablen   #
#                       #
#########################
$QuantityToBuild = 0;
%LocalDepsHash = ();
%DepsArchive = ();
%BuildQueue = ();
%PathHash = ();
%PlatformHash = ();
%DeadDependencies = ();
%ParentDepsHash = ();
@UnresolvedParents = ();
%DeadParents = ();
$CurrentPrj = "";
$StandDir = GetStandDir();
$QuantityToBuild = GetQuantityToBuild();
$BuildAllParents = HowToBuild();
$ENV{mk_tmp} = "1";

#### main ####

$dmake = GetDmakeCommando();
BuildAll();
@TotenEltern = keys %DeadParents;
if ($#TotenEltern != -1) {
    my ($DeadPrj);
    print "\nWARNING! Project(s):\n\n";
    foreach $DeadPrj (@TotenEltern) {
        print "$DeadPrj\n";
    };
    print "\nnot found and couldn't be built. Correct build.lsts.\n";
};
$ENV{mk_tmp} = "";


#########################
#                       #
#      Procedures       #
#                       #
#########################


#
# Get dependencies hash of the current and all parent projects
#
sub GetParentDeps {
    my ($ParentsString, @DepsArray, $Prj, $Parent, @TotenEltern);
    $ParentsString = GetParentsString(".");
    @DepsArray = GetDependenciesArray($ParentsString);
    @UnresolvedParents = @DepsArray;
    $ParentDepsHash{$CurrentPrj} = [@DepsArray];
    ResolveParentsLoop:
    while ($Prj = pop(@UnresolvedParents)) {
        my (@DepsArray);
        if (!($ParentsString = GetParentsString($StandDir.$Prj))) {
            $DeadParents{$Prj} = 1;
            $ParentDepsHash{$Prj} = [];
            next ResolveParentsLoop;
        };
        @DepsArray = GetDependenciesArray($ParentsString);
        $ParentDepsHash{$Prj} = [@DepsArray];
        foreach $Parent (@DepsArray) {
            if (!defined($ParentDepsHash{$Parent})) {
                push (@UnresolvedParents, $Parent);
            };
        };
    };
    print "\n";
    @TotenEltern = keys %DeadParents;
    foreach $Parent (@TotenEltern) {
        RemoveFromDependencies($Parent, %ParentDepsHash);
    };
};


#
# Build everything that should be built
#
sub BuildAll {
    if ($BuildAllParents) {
        my ($Prj, $PrjDir, $DeadPrj, @TotenEltern);
        GetParentDeps();
        @TotenEltern = keys %DeadParents;
        foreach $DeadPrj (@TotenEltern) {
            delete $ParentDepsHash{$DeadPrj};
            RemoveFromDependencies($DeadPrj, \%ParentDepsHash);
        };
        while ($Prj = PickPrjToBuild(\%ParentDepsHash)) {
            print "\n=============\n";
            print "Building project $Prj\n";
            print   "=============\n";
            $PrjDir = CorrectPath($StandDir.$Prj);
            BuildPrj($PrjDir);
            system ("$ENV{DELIVER}");
            RemoveFromDependencies($Prj, \%ParentDepsHash);
        };
    } else {
        BuildPrj(".");
    };
};


#
# Start build given project
#
sub MakeDir {
    my ($DirToBuild, $BuildDir, $error);
    $DirToBuild = $_[0];
    $BuildDir = CorrectPath($StandDir.$PathHash{$DirToBuild});
    if ($ENV{GUI} eq "UNX") {
        use Cwd 'chdir';
    };
    chdir ($BuildDir);
    print $BuildDir, "\n";
    cwd();
    $error = system ("$dmake");
    if (!$error) {
        RemoveFromDependencies($DirToBuild, \%LocalDepsHash);
    } else {
        print "Error $error occurred while making $BuildDir\n";
        $ENV{mk_tmp} = "";
        exit();
    };
};


#
# Get string (list) of parent projects to build
#
sub GetParentsString {
    my ($PrjDir);
    $PrjDir = $_[0];
    if (!open (PrjBuildFile, $PrjDir."/prj/build.lst")) {
        return "";
    };
    while (<PrjBuildFile>) {
        s/\r\n//;
        if ($_ =~ /([\:]+)([\t | \s]+)/) {
            close PrjBuildFile;
            return $';
        };
    };
    close PrjBuildFile;
    return "NULL";
};

#
# Check if project should be built when all parent projects are built
#
sub HowToBuild {
    my ($i);
    foreach $i (0 .. $#ARGV) {
        if ($ARGV[$i] =~ /^-all/) {
            splice(@ARGV, $i, 1);
            return 1;
        };
    };
    return 0;
}

#
# Getting hashes of all internal dependencies and additional
# infos for given project
#
sub BuildPrj {
    my ($dummy, $PrjToBuild);
    $PrjToBuild = $_[0];
    if ($ENV{GUI} eq "UNX") {
        use Cwd 'chdir';
    };
    chdir $PrjToBuild;
    cwd();

    open (PrjBuildFile, "prj/build.lst");
    BuildLstLoop:
    while (<PrjBuildFile>) {
        s/\r\n//;
        if ($_ =~ /nmake/) {
            my ($Platform, $Dependencies, $Dir, $DirAlias, @Array);
            $Dependencies = $';
            $dummy = $`;
            $dummy =~ /(\S+)(\s+)(\S+)/;
            $Dir = $3;
            $Dependencies =~ /(\w+)/; #/(\t\-\t)(\w+)/; #(\t)(\S+)(\s)/;
            $Platform = $1;
            $Dependencies = $';
            while ($Dependencies =~ /,(\w+)/) {
                $Dependencies = $';
            };
            $Dependencies =~ /(\s+)(\S+)(\s+)/;
            $DirAlias = $2;
            if (!CheckPlatform($Platform)) {
                $DeadDependencies{$DirAlias} = 1;
                next BuildLstLoop;
            };
            $PlatformHash{$DirAlias} = 1;
            $Dependencies = $';
            @Array = GetDependenciesArray($Dependencies);
            $LocalDepsHash{$DirAlias} = [@Array];
            $BuildQueue{$DirAlias} = 1;
            $PathHash{$DirAlias} = $Dir;
        };
    };
    close PrjBuildFile;
    %DepsArchive = %LocalDepsHash;
    foreach $Dir (keys %DeadDependencies) {
        if (!IsHashNative($Dir)) {
            RemoveFromDependencies($Dir, \%LocalDepsHash);
            delete $DeadDependencies{$Dir};
        };
    };
    BuildDependent();
};


#
# Convert path from abstract (with '\' and/or '/' delimiters)
# to system-dependent
#
sub CorrectPath {
    $_ = $_[0];
    if ($ENV{GUI} eq "UNX") {
        s/\\/\//g;
    } elsif (   ($ENV{GUI} eq "WNT") ||
                ($ENV{GUI} eq "WIN") ||
                ($ENV{GUI} eq "MACOSX") ||
                ($ENV{GUI} eq "OS2")) {
        s/\//\\/g;
    } else {
        $ENV{mk_tmp} = "";
        die "\nNo environment set\n";
    };
    return $_;
};


#
# Get platform-dependent dmake commando
#
sub GetDmakeCommando {
    my ($dmake, $arg);

    # Setting alias for dmake
    $dmake = "dmake";
    #if (defined $ENV{PROFULLSWITCH}) {
    #   $dmake .= " ".$ENV{PROFULLSWITCH};
    #};
    while ($arg = pop(@ARGV)) {
        $dmake .= " "."$arg";
    };
    return $dmake;
};


#
# Procedure returns quantity of folders to be built syncronously
#
sub GetQuantityToBuild {
    my ($i);
    foreach $i (0 .. $#ARGV) {
        if ($ARGV[$i] =~ /^-PP/) {
            splice(@ARGV, $i, 1);
            return $';
        };
    };
};


#
# Procedure prooves if current dir is a root dir of the drive
#
sub IsRootDir {
    my ($Dir);
    $Dir = $_[0];
    if (        (($ENV{GUI} eq "UNX") ||
                 ($ENV{GUI} eq "MACOSX")) &&
                ($Dir eq "\/")) {
        return 1;
    } elsif (   (($ENV{GUI} eq "WNT") ||
                 ($ENV{GUI} eq "WIN") ||
                 ($ENV{GUI} eq "OS2")) &&
                ($Dir =~ /\S:\/$/)) {
        return 1;
    } else {
        return 0;
    };
};


#
# Procedure retrieves list of projects to be built from build.lst
#
sub GetStandDir {
    my ($StandDir);
DirLoop:
    do {
        $StandDir = cwd();
        if (open(PrjBuildFile, "prj/build.lst")) {
            $StandDir =~ /(\w+$)/;
            $StandDir = $`;
            $CurrentPrj = $1;
            close(PrjBuildFile);
            return $StandDir;
        } elsif (IsRootDir($StandDir)) {
            $ENV{mk_tmp} = "";
            die "Found no project to build\n"
        };
    }
    while (chdir '..');
};


#
# Build the entire project according to queue of dependencies
#
sub BuildDependent {
    my ($Dir);
    while ($Dir = PickPrjToBuild(\%LocalDepsHash)) {
        MakeDir($Dir);
        $Dir = "";
    };
};


#
# Picks project which can be build now from hash and deletes it from hash
#
sub PickPrjToBuild {
    my ($Prj, $DepsHash);
    $DepsHash = $_[0];
    $Prj = FindIndepPrj($DepsHash);
    delete $$DepsHash{$Prj};
    #print "$Prj\n";
    return $Prj;
};


#
# Make a decision if the project should be built on this platform
#
sub CheckPlatform {
    my ($Platform);
    $Platform = $_[0];
    if ($Platform eq "all") {
        return 1;
    } elsif (($ENV{GUI} eq "WNT") &&
                            (($Platform eq "w") || ($Platform eq "n"))) {
        return 1;
    } elsif (($ENV{GUI} eq "WIN") && ($Platform eq "w")) {
        return 1;
    } elsif (($ENV{GUI} eq "UNX") && ($Platform eq "u")) {
        return 1;
    } elsif (($ENV{GUI} eq "MACOSX") && ($Platform eq "m")) {
        return 1;
    } elsif (($ENV{GUI} eq "OS2") && ($Platform eq "p")) {
        return 1;
    };
    return 0;
};


#
# Remove project to build ahead from dependencies and make an array
# of all from given project dependent projects
#
sub RemoveFromDependencies {
    my ($ExclPrj, $i, $Prj, $Dependencies);
    $ExclPrj = $_[0];
    $Dependencies = $_[1];
    foreach $Prj (keys %$Dependencies) {
        PrjDepsLoop:
        foreach $i (0 .. $#{$$Dependencies{$Prj}}) {
            #print $Prj, " $i ", ${$$Dependencies{$Prj}}[$i], "\n";
            if (${$$Dependencies{$Prj}}[$i] eq $ExclPrj) {
                splice (@{$$Dependencies{$Prj}}, $i, 1);
                $i = 0;
                last PrjDepsLoop;
            };
        };
    };
};


#
# Find undependent project
#
sub FindIndepPrj {
    my ($Prj, @Prjs, @PrjDeps, $Dependencies, $i);
    $Dependencies = $_[0];
    @Prjs = keys %$Dependencies;
    if ($#Prjs != -1) {
        PrjLoop:
        foreach $Prj (@Prjs) {
            if (IsHashNative($Prj)) {
                next PrjLoop;
            };
            @PrjDeps = @{$$Dependencies{$Prj}};
            if ($#PrjDeps == -1) {
                return $Prj;
            };
        };
        # If there are only dependent projects in hash - generate error
        print "\nError: projects";
        DeadPrjLoop:
        foreach $Prj (keys %$Dependencies) {
            if (IsHashNative($Prj)) {
                next DeadPrjLoop;
            };
            $i = 0;
            print "\n", $Prj, " depends on:";
            foreach $i (0 .. $#{$$Dependencies{$Prj}}) {
                print " ", ${$$Dependencies{$Prj}}[$i];
            };
        };
        print "\nhave dead or circular dependencies\n\n";
        $ENV{mk_tmp} = "";
        exit ();
    };
};



#
# Check if given entry is HASH-native, that is not a user-defined data
#
sub IsHashNative {
    my ($Prj);
    $Prj = $_[0];
    if ($Prj =~ /^HASH\(0x[\d | a | b | c | d | e | f]{6,}\)/) {
        return 1;
    } else {
        return 0;
    };
};


#
# Getting array of dependencies from the string given
#
sub GetDependenciesArray {
    my ($DepString, @Dependencies, $ParentPrj);
    @Dependencies = ();
    $DepString = $_[0];
    while (!($DepString =~ /^NULL/)) {
        $DepString =~ /(\S+)(\s+)/;
        $ParentPrj = $1;
        $DepString = $';
        if ($ParentPrj =~ /(\S+)(\.)(\w)/) {
            $ParentPrj = $1;
            if (CheckPlatform($3)) {
                push(@Dependencies, $ParentPrj);
            };
        } else {
            push(@Dependencies, $ParentPrj);
        };
    };
    return @Dependencies;
};


#
# Getting current directory list
#
sub GetDirectoryList {
    my ($Path);
    $Path = $_[0];
    opendir(CurrentDirList, $Path);
    @DirectoryList = readdir(CurrentDirList);
    closedir(CurrentDirList);
    return @DirectoryList;
};

