#*************************************************************************
#
#   $RCSfile: profiles.pm,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2005-01-31 10:46:36 $
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

package installer::profiles;

use installer::converter;
use installer::existence;
use installer::exiter;
use installer::files;
use installer::globals;
use installer::logger;
use installer::remover;
use installer::systemactions;

#############################
# Profiles
#############################

#######################################################
# Sorting the content of a profile
#######################################################

sub sorting_profile
{
    my ($profilesref) = @_;

    my @profile = ();
    my @definedsections = ();

    for ( my $i = 0; $i <= $#{$profilesref}; $i++ )
    {
        my $line = ${$profilesref}[$i];

        if ( $line =~ /^\s*(\[.*\])\s*$/ )  # this is a section (every second line)
        {
            my $section = $1;

            if (!(installer::existence::exists_in_array($section, \@definedsections)))
            {
                my $sectionline = $section . "\n";
                push(@definedsections, $section);
                push(@profile, $sectionline);

                for ( my $j = 0; $j <= $#{$profilesref}; $j++ )
                {
                    my $oneline = ${$profilesref}[$j];
                    installer::remover::remove_leading_and_ending_whitespaces(\$oneline);

                    if ( $oneline eq $section )
                    {
                        my $nextline = ${$profilesref}[$j+1];
                        push(@profile, $nextline);
                    }
                }
            }
        }
    }

    return \@profile;
}

#####################################################################
# Adding the newly created profile into the file list
#####################################################################

sub add_profile_into_filelist
{
    my ($filesarrayref, $oneprofile, $completeprofilename) = @_;

    my %profile = ();

    # Taking the base data from the "gid_File_Lib_Vcl"

    my $vclgid = "gid_File_Lib_Vcl";
    my $vclfile = installer::existence::get_specified_file($filesarrayref, $vclgid);

    # copying all base data
    installer::converter::copy_item_object($vclfile, \%profile);

    # and overriding all new values

    $profile{'ismultilingual'} = 0;
    $profile{'sourcepath'} = $completeprofilename;
    $profile{'Name'} = $oneprofile->{'Name'};
    $profile{'UnixRights'} = "444";
    $profile{'gid'} = $oneprofile->{'gid'};
    $profile{'Dir'} = $oneprofile->{'Dir'};
    $profile{'destination'} = $oneprofile->{'destination'};
    $profile{'Styles'} = "";
    if ( $oneprofile->{'Styles'} ) { $profile{'Styles'} = $oneprofile->{'Styles'}; }
    $profile{'modules'} = $oneprofile->{'ModuleID'};    # Profiles can only be added completely to a module

    push(@{$filesarrayref}, \%profile);
}

###################################################
# Including Windows line ends in ini files
# Profiles on Windows shall have \r\n line ends
###################################################

sub include_windows_lineends
{
    my ($onefile) = @_;

    for ( my $i = 0; $i <= $#{$onefile}; $i++ )
    {
        ${$onefile}[$i] =~ s/\r?\n$/\r\n/;
    }
}

####################################
# Create profiles
####################################

sub create_profiles
{
    my ($profilesref, $profileitemsref, $filesarrayref, $languagestringref) = @_;

    my $infoline;

    my $profilesdir = installer::systemactions::create_directories("profiles", $languagestringref);

    installer::logger::include_header_into_logfile("Creating profiles:");

    # Attention: The module dependencies from ProfileItems have to be ignored, because
    # the Profile has to be installed completely with all of its content and the correct name.
    # Only complete profiles can belong to a specified module, but not ProfileItems!

    # iterating over all files

    for ( my $i = 0; $i <= $#{$profilesref}; $i++ )
    {
        my $oneprofile = ${$profilesref}[$i];
        my $dir = $oneprofile->{'Dir'};
        if ( $dir eq "PREDEFINED_CONFIGDIR" ) { next; }     # ignoring the profile sversion file

        my $profilegid = $oneprofile->{'gid'};
        my $profilename = $oneprofile->{'Name'};

        my @onefile = ();
        my $profileempty = 1;

        for ( my $j = 0; $j <= $#{$profileitemsref}; $j++ )
        {
            my $oneprofileitem = ${$profileitemsref}[$j];
            my $profileid = $oneprofileitem->{'ProfileID'};

            if ( $profileid eq $profilegid )
            {
                my $section = $oneprofileitem->{'Section'};
                my $key = $oneprofileitem->{'Key'};
                my $value = $oneprofileitem->{'Value'};
                my $order = $oneprofileitem->{'Order'}; # ignoring order at the moment

                my $line = "[" . $section . "]" . "\n";
                push(@onefile, $line);
                $line = $key . "=" . $value . "\n";
                push(@onefile, $line);

                $profileempty = 0;
            }
        }

        if ( $profileempty ) { next; }  # ignoring empty profiles

        # Sorting the array @onefile
        my $onefileref = sorting_profile(\@onefile);

        if ( $installer::globals::iswin && $installer::globals::plat =~ /cygwin/i)      # Windows line ends only for Cygwin
        {
            include_windows_lineends($onefileref);
        }

        # Saving the profile as a file
        $completeprofilename = $profilesdir . $installer::globals::separator . $profilename;

        installer::files::save_file($completeprofilename, $onefileref);

        # Adding the file to the filearray
        # Some data are set now, others are taken from the file "soffice.exe" ("soffice.bin")
        add_profile_into_filelist($filesarrayref, $oneprofile, $completeprofilename);

        $infoline = "Created Profile: $completeprofilename\n";
        push( @installer::globals::logfileinfo, $infoline);
    }

    $infoline = "\n";
    push( @installer::globals::logfileinfo, $infoline);
}


1;
