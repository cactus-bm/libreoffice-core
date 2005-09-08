#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: inifile.pm,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:18:28 $
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

package installer::windows::inifile;

use installer::existence;
use installer::files;
use installer::globals;
use installer::windows::idtglobal;

####################################################
# Setting the profile for a special profileitem
####################################################

sub get_profile_for_profileitem
{
    my ($profileid, $filesref) = @_;

    my $profile = installer::existence::get_specified_file($filesref, $profileid);

    return $profile;
}

####################################################
# Checking whether profile is included in patch
####################################################

sub profile_has_patch_flag
{
    my ($profile) = @_;

    my $in_patch = 0;

    my $styles = "";
    if ( $profile->{'Styles'} ) { $styles = $profile->{'Styles'}; }
    if ( $styles =~ /\bPATCH\b/ ) { $in_patch = 1; }

    return $in_patch;
}

####################################################
# Checking whether profile is part of product
####################################################

sub file_is_part_of_product
{
    my ($profilegid, $filesref) = @_;

    my $part_of_product = 0;

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        $onefile = ${$filesref}[$i];
        my $filegid = $onefile->{'gid'};

        if ( $filegid eq $profilegid )
        {
            $part_of_product = 1;
            last;
        }
    }

    return $part_of_product;
}

###########################################################################################################
# Creating the file IniFile.idt dynamically
# Content:
# IniFile\tFileName\tDirProperty\tSection\tKey\tValue\tAction\tComponent_
###########################################################################################################

sub create_inifile_table
{
    my ($inifiletableentries, $filesref, $basedir) = @_;

    my @inifiletable = ();

    installer::windows::idtglobal::write_idt_header(\@inifiletable, "inifile");

    for ( my $i = 0; $i <= $#{$inifiletableentries}; $i++ )
    {
        my $profileitem = ${$inifiletableentries}[$i];

        my $profileid = $profileitem->{'ProfileID'};

        # Is this profile part of the product? This is not sure, for example in patch process.
        # If the profile is not part of the product, this ProfileItem must be ignored.

        if ( ! file_is_part_of_product($profileid, $filesref) ) { next; }

        my $profile = get_profile_for_profileitem($profileid, $filesref);

        if (( $installer::globals::patch ) && ( ! profile_has_patch_flag($profile) )) { next; }

        my %inifile = ();

        $inifile{'IniFile'} = $profileitem->{'Inifiletablekey'};
        $inifile{'FileName'} = $profile->{'Name'};
        $inifile{'DirProperty'} = $profile->{'uniquedirname'};
        $inifile{'Section'} = $profileitem->{'Section'};
        $inifile{'Key'} = $profileitem->{'Key'};
        $inifile{'Value'} = $profileitem->{'Value'};
        $inifile{'Action'} = $profileitem->{'Inifiletableaction'};
        $inifile{'Component_'} = $profile->{'componentname'};

        my $oneline = $inifile{'IniFile'} . "\t" . $inifile{'FileName'} . "\t" . $inifile{'DirProperty'} . "\t"
                 . $inifile{'Section'} . "\t" . $inifile{'Key'} . "\t" . $inifile{'Value'} . "\t"
                 . $inifile{'Action'} . "\t" . $inifile{'Component_'} . "\n";

        push(@inifiletable, $oneline);
    }

    # Saving the file

    my $inifiletablename = $basedir . $installer::globals::separator . "IniFile.idt";
    installer::files::save_file($inifiletablename ,\@inifiletable);
    my $infoline = "Created idt file: $inifiletablename\n";
    push(@installer::globals::logfileinfo, $infoline);

}

1;
