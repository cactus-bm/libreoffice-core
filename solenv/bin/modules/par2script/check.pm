#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: check.pm,v $
#
#   $Revision: 1.6 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:24:49 $
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


package par2script::check;

use par2script::existence;
use par2script::globals;
use par2script::work;

################################
# Checks of the setup script
################################

########################################################
# Checking if all defined directories are needed
########################################################

sub check_needed_directories
{
    my ($setupscript) = @_;

    print "Checking directories ... ";

    my $allfiles = par2script::work::get_all_items_from_script($setupscript, "File");
    my $alldirs = par2script::work::get_all_items_from_script($setupscript, "Directory");

    # checking if all defined directories are needed

    for ( my $i = 0; $i <= $#{$alldirs}; $i++ )
    {
        my $onedir = ${$alldirs}[$i];

        # I. directory has create flag

        my $styles = "";
        if ( $onedir->{'Styles'} ) { $styles = $onedir->{'Styles'} }

        if ( $styles =~ /\bCREATE\b/ ) { next; }    # this is okay

        my $dirgid = $onedir->{'gid'};
        my $directory_required = 0;

        # II. there are files in the directory

        for ( my $j = 0; $j <= $#{$allfiles}; $j++ )
        {
            my $onefile = ${$allfiles}[$j];


            my $filedir = "";
            if ( $onefile->{'Dir'} ) { $filedir = $onefile->{'Dir'} }
            my $filenetdir = "";
            if ( $onefile->{'NetDir'} )  { $filedir = $onefile->{'NetDir'} }

            if (( $dirgid eq $filedir ) || ( $dirgid eq $filenetdir ))
            {
                $directory_required = 1;
                last;
            }
        }

        if ( $directory_required ) { next; };

        # III. the directory is parent for another directory

        for ( my $j = 0; $j <= $#{$alldirs}; $j++ )
        {
            my $onedir = ${$alldirs}[$j];

            my $parentid = $onedir->{'ParentID'};

            if ( $dirgid eq $parentid )
            {
                $directory_required = 1;
                last;
            }
        }

        if (! $directory_required)
        {
            my $infoline = "INFO: Directory definition $dirgid is superfluous\n";
            # print $infoline;
            push(@par2script::globals::logfileinfo, $infoline);
        }
    }

    print "Done\n";
}

##################################################
# Checking if the directories in the file
# definitions are defined.
##################################################

sub check_directories_in_item_definitions
{
    my ($setupscript, $item) = @_;

    print "Checking $item ... ";

    my $allfiles = par2script::work::get_all_items_from_script($setupscript, $item);
    my $alldirectorygids = par2script::work::get_all_gids_from_script($setupscript, "Directory");

    for ( my $i = 0; $i <= $#{$allfiles}; $i++ )
    {
        my $onefile = ${$allfiles}[$i];

        my $filedir = $onefile->{'Dir'};

        # checking if $filedir is not empty or not defined

        if ( $filedir eq "" ) { die "ERROR: No directory defined for file: $onefile->{'gid'}!\n"; }

        if (( $filedir eq "PD_PROGDIR" ) || ( $filedir =~ /PREDEFINED_/ )) { next; }

        # now checking if this directoryid exists

        if ( ! par2script::existence::exists_in_array($filedir, $alldirectorygids) )
        {
            die "ERROR: Directory $filedir in $item $onefile->{'gid'} not defined!\n";
        }
    }

    print "Done\n";
}

########################################################
# Checking for all Items, that know their modules,
# whether these modules exist. If not, this is a
# script error.
########################################################

sub check_module_existence
{
    my ($script) = @_;

    print "Checking modules ... ";

    my $allmodulegids = par2script::work::get_all_gids_from_script($script, "Module");

    for ( my $i = 0; $i <= $#par2script::globals::items_with_moduleid; $i++ )
    {
        my $oneitem = $par2script::globals::items_with_moduleid[$i];

        my $allitems = par2script::work::get_all_items_from_script($script, $oneitem);

        for ( my $j = 0; $j <= $#{$allitems}; $j++ )
        {
            my $oneitem = ${$allitems}[$j];

            my $moduleid = $oneitem ->{'ModuleID'};

            if ( $moduleid eq "" )  { die "ERROR: No ModuleID defined for item: $oneitem->{'gid'}!\n"; }

            # now checking if this moduleid exists

            if ( ! par2script::existence::exists_in_array($moduleid, $allmodulegids) )
            {
                die "ERROR: ModuleID $moduleid in $oneitem->{'gid'} not defined!\n";
            }
        }
    }

    print "Done\n";
}

########################################################
# If the StarRegistry is not defined in the script,
# it has to be removed from the file definition.
########################################################

sub check_registry_at_files
{
    my ($script) = @_;

    print "Checking StarRegistry ... ";

    my $allstarregistrygids = par2script::work::get_all_gids_from_script($script, "StarRegistry");
    my $isfile = 0;

    for ( my $i = 0; $i <= $#{$script}; $i++ )
    {
        if ( ${$script}[$i] =~ /^\s*File\s+\w+\s*$/ ) { $isfile = 1; }

        if (( $isfile ) && ( ${$script}[$i] =~ /^\s*End\s*$/i )) { $isfile = 0; }

        if (( $isfile ) && ( ${$script}[$i] =~ /^\s*RegistryID\s+\=\s+(\w+)\s*\;\s*$/ ))
        {
            my $registry = $1;
            my $isdefinedregistry = 0;

            for ( my $j = 0; $j <= $#{$allstarregistrygids}; $j++ )
            {
                if ( $registry eq ${$allstarregistrygids}[$j] ) { $isdefinedregistry = 1; }
            }

            # if no definition was found, the line "RegistryID = ..." can be removed

            if (! $isdefinedregistry) { splice(@{$script}, $i, 1); }
        }
    }

    print "Done\n";
}

########################################################
# File, Shortcut, Directory, Procedure must not
# contain a ModuleID
########################################################

sub check_moduleid_at_items
{
    my ($script) = @_;

    print "Checking module assignments ... ";

    my @items = ("File", "Directory", "Shortcut", "Procedure");

    for ( my $i = 0; $i <= $#items; $i++ )
    {
        my $oneitem = $items[$i];
        my $isitem = 0;

        for ( my $j = 0; $j <= $#{$script}; $j++ )
        {
            if ( ${$script}[$j] =~ /^\s*$oneitem\s+(\w+)\s*$/ )
            {
                $gid = $1;
                $isitem = 1;
            }

            if (( $isitem ) && ( ${$script}[$j] =~ /^\s*End\s*$/i )) { $isitem = 0; }

            if (( $isitem ) && ( ${$script}[$j] =~ /^\s*ModuleID\s+\=/ ))
            {
                die "\nERROR: ModuleID assigned to $gid! No module assignment to $oneitem!\n\n";
            }
        }
    }

    print "Done\n";
}

########################################################
# File, Shortcut, Directory, Procedure must not
# contain a ModuleID
########################################################

sub check_semicolon
{
    my ($script) = @_;

    print "Checking syntax ... ";

    for ( my $i = 0; $i <= $#par2script::globals::allitems; $i++ )
    {
        my $oneitem = $par2script::globals::allitems[$i];

        if ( $oneitem eq "Procedure" ) { next; }    # no syntax check for Procedure
        if ( $oneitem eq "Custom" ) { next; }       # no syntax check for Custom
        if ( $oneitem eq "Module" ) { next; }       # no syntax check for Module

        my $isinsideitem = 0;
        my $gid = "";
        my $isstartline = 0;

        for ( my $j = 0; $j <= $#{$script}; $j++ )
        {
            my $scriptline = ${$script}[$j];

            if ( $isstartline )
            {
                $isstartline = 0;
                $isinsideitem = 1;
            }

            if ( $scriptline =~ /^\s*$oneitem\s+(\w+)\s*$/ )
            {
                $gid = $1;
                $isstartline = 1;
            }

            if (( $isinsideitem ) && ( $scriptline =~ /^\s*End\s*$/i ))
            {
                $isinsideitem = 0;
            }

            # checking semicolon
            if ( ($isinsideitem) && (!($scriptline =~ /\;\s*$/)) ) { die "\nERROR: Syntax error (missing semicolon) in $gid:\n$scriptline\n\n"; }
            if ( ($isinsideitem) && ($scriptline =~ /\;\;\s*$/) ) { die "\nERROR: Syntax error (double semicolon) in $gid:\n$scriptline\n\n"; }
        }
    }

    print "Done\n";
}

1;
