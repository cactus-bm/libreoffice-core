#*************************************************************************
#
#   $RCSfile: featurecomponent.pm,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2005-01-31 10:49:22 $
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

package installer::windows::featurecomponent;

use installer::converter;
use installer::existence;
use installer::exiter;
use installer::files;
use installer::globals;
use installer::windows::idtglobal;

#################################################################################
# Collecting all pairs of features and components from the files collector
#################################################################################

sub create_featurecomponent_table_from_files_collector
{
    my ($featurecomponenttableref, $filesref) = @_;

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        my $onefile = ${$filesref}[$i];

        my $filecomponent = $onefile->{'componentname'};
        my $filemodules = $onefile->{'modules'};

        if ( $filecomponent eq "" )
        {
            installer::exiter::exit_program("ERROR: No component defined for file $onefile->{'Name'}", "create_featurecomponent_table_from_files_collector");
        }
        if ( $filemodules eq "" )
        {
            installer::exiter::exit_program("ERROR: No modules found for file $onefile->{'Name'}", "create_featurecomponent_table_from_files_collector");
        }

        my $filemodulesarrayref = installer::converter::convert_stringlist_into_array(\$filemodules, ",");

        for ( my $j = 0; $j <= $#{$filemodulesarrayref}; $j++ )
        {
            my %featurecomponent = ();

            my $onemodule = ${$filemodulesarrayref}[$j];
            $onemodule =~ s/\s*$//;
            $featurecomponent{'Feature'} = $onemodule;
            $featurecomponent{'Component'} = $filecomponent;

            # Attention: Features are renamed, because the maximum length is 38.
            # But in the files collector ($filesref), the original names are saved.

            installer::windows::idtglobal::shorten_feature_gid(\$featurecomponent{'Feature'});

            $oneline = "$featurecomponent{'Feature'}\t$featurecomponent{'Component'}\n";

            # control of uniqueness

            if (! installer::existence::exists_in_array($oneline, $featurecomponenttableref))
            {
                push(@{$featurecomponenttableref}, $oneline);
            }
        }
    }
}

#################################################################################
# Collecting all pairs of features and components from the registry collector
#################################################################################

sub create_featurecomponent_table_from_registry_collector
{
    my ($featurecomponenttableref, $registryref) = @_;

    for ( my $i = 0; $i <= $#{$registryref}; $i++ )
    {
        my $oneregistry = ${$registryref}[$i];

        my $registrycomponent = $oneregistry->{'componentname'};
        my $registrymodule = $oneregistry->{'ModuleID'};

        if ( $registrycomponent eq "" )
        {
            installer::exiter::exit_program("ERROR: No component defined for registry $oneregistry->{'gid'}", "create_featurecomponent_table_from_registry_collector");
        }
        if ( $registrymodule eq "" )
        {
            installer::exiter::exit_program("ERROR: No modules found for registry $oneregistry->{'gid'}", "create_featurecomponent_table_from_registry_collector");
        }

        my %featurecomponent = ();

        $featurecomponent{'Feature'} = $registrymodule;
        $featurecomponent{'Component'} = $registrycomponent;

        # Attention: Features are renamed, because the maximum length is 38.
        # But in the files collector ($filesref), the original names are saved.

        installer::windows::idtglobal::shorten_feature_gid(\$featurecomponent{'Feature'});

        $oneline = "$featurecomponent{'Feature'}\t$featurecomponent{'Component'}\n";

        # control of uniqueness

        if (! installer::existence::exists_in_array($oneline, $featurecomponenttableref))
        {
            push(@{$featurecomponenttableref}, $oneline);
        }
    }
}

#################################################################################
# Collecting all feature that are listed in the featurecomponent table.
#################################################################################

sub collect_all_feature
{
    my ($featurecomponenttable) = @_;

    my @allfeature = ();

    for ( my $i = 3; $i <= $#{$featurecomponenttable}; $i++ )   # beginning in line 4
    {
        my $oneline = ${$featurecomponenttable}[$i];

        if ( $oneline =~ /^\s*(\S+)\s+(\S+)\s*$/ )
        {
            my $feature = $1;

            if (! installer::existence::exists_in_array($feature, \@allfeature)) { push(@allfeature, $feature); }
        }
    }

    return \@allfeature;
}

#################################################################################
# On Win98 and Win Me there seems to be the problem, that maximum 817
# components can be added to a feature. Even if Windows Installer 2.0
# is used.
#################################################################################

sub check_number_of_components_at_feature
{
    my ($featurecomponenttable) = @_;

    my $infoline = "\nChecking number of components at features. Maximum is 817 (for Win 98 and Win Me)\n";
    push(@installer::globals::logfileinfo, $infoline);

    my $allfeature = collect_all_feature($featurecomponenttable);

    for ( my $i = 0; $i <= $#{$allfeature}; $i++ )
    {
        my $onefeature = ${$allfeature}[$i];
        my $featurecomponents = 0;

        for ( my $j = 0; $j <= $#{$featurecomponenttable}; $j++ )
        {
            if ( ${$featurecomponenttable}[$j] =~ /^\s*\Q$onefeature\E\s+(\S+)\s*$/ ) { $featurecomponents++; }
        }

        if ( $featurecomponents > 816 )
        {
            installer::exiter::exit_program("ERROR: More than 816 components ($featurecomponents) at feature $onefeature. This causes problems on Win 98 and Win Me!", "check_number_of_components_at_feature");
        }

        # Logging the result

        $infoline = "Number of components at feature $onefeature : $featurecomponents\n";
        push(@installer::globals::logfileinfo, $infoline);
    }

    $infoline = "\n";
    push(@installer::globals::logfileinfo, $infoline);
}

#################################################################################
# Creating the file FeatureC.idt dynamically
# Content:
# Feature Component
#################################################################################

sub create_featurecomponent_table
{
    my ($filesref, $registryref, $basedir) = @_;

    my @featurecomponenttable = ();
    my $infoline;

    installer::windows::idtglobal::write_idt_header(\@featurecomponenttable, "featurecomponent");

    # This is the first time, that features and componentes are related
    # Problem: How about created profiles, configurationfiles, services.rdb
    # -> simple solution: putting them all to the root module
    # Otherwise profiles and configurationfiles cannot be created the way, they are now created
    # -> especially a problem for the configurationfiles! # ToDo
    # Very good: All ProfileItems belong to the root
    # services.rdb belongs to the root anyway.

    # At the moment only the files are related to components (and the files know their modules).
    # The component for each file is written into the files collector $filesinproductlanguageresolvedarrayref

    create_featurecomponent_table_from_files_collector(\@featurecomponenttable, $filesref);

    create_featurecomponent_table_from_registry_collector(\@featurecomponenttable, $registryref);

    # Additional components have to be added here

    # Checking, whether there are more than 817 components at a feature

    check_number_of_components_at_feature(\@featurecomponenttable);

    # Saving the file

    my $featurecomponenttablename = $basedir . $installer::globals::separator . "FeatureC.idt";
    installer::files::save_file($featurecomponenttablename ,\@featurecomponenttable);
    $infoline = "Created idt file: $featurecomponenttablename\n";
    push(@installer::globals::logfileinfo, $infoline);

}

1;