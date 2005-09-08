#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: removefile.pm,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:21:26 $
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

package installer::windows::removefile;

use installer::files;
use installer::globals;
use installer::windows::idtglobal;

########################################################################
# Returning the FileKey for a folderitem for removefile table.
########################################################################

sub get_removefile_filekey
{
    my ($folderitem) = @_;

    # returning the unique identifier

    my $identifier = "remove_" . $folderitem->{'directory'};

    $identifier = lc($identifier);

    return $identifier;
}

########################################################################
# Returning the Component for a folderitem for removefile table.
########################################################################

sub get_removefile_component
{
    my ($folderitem) = @_;

    return $folderitem->{'component'};
}

########################################################################
# Returning the FileName for a folderitem for removefile table.
########################################################################

sub get_removefile_filename
{
    my ($folderitem) = @_;

     # return nothing: The assigned directory will be removed

    return "";
}

########################################################################
# Returning the DirProperty for a folderitem for removefile table.
########################################################################

sub get_removefile_dirproperty
{
    my ($folderitem) = @_;

    return $folderitem->{'directory'};
}

########################################################################
# Returning the InstallMode for a folderitem for removefile table.
########################################################################

sub get_removefile_installmode
{
    my ($folderitem) = @_;

    # always returning "2": The file is only removed, if the assigned
    # component is removed. Name: msidbRemoveFileInstallModeOnRemove

    return 2;
}

###########################################################################################################
# Creating the file RemoveFi.idt dynamically
# Content:
# FileKey Component_ FileName DirProperty InstallMode
###########################################################################################################

sub create_removefile_table
{
    my ($folderitemsref, $basedir) = @_;

    my @removefiletable = ();

    installer::windows::idtglobal::write_idt_header(\@removefiletable, "removefile");

    # Only the directories created for the FolderItems have to be deleted
    # with the information in the table RemoveFile

    my @directorycollector = ();

    for ( my $i = 0; $i <= $#{$folderitemsref}; $i++ )
    {
        my $onelink = ${$folderitemsref}[$i];

        if ( $onelink->{'used'} == 0 ) { next; }

        if ( installer::existence::exists_in_array($onelink->{'directory'}, \@directorycollector)) { next; }

        push(@directorycollector, $onelink->{'directory'});

        my %removefile = ();

        $removefile{'FileKey'} = get_removefile_filekey($onelink);
        $removefile{'Component_'} = get_removefile_component($onelink);
        $removefile{'FileName'} = get_removefile_filename($onelink);
        $removefile{'DirProperty'} = get_removefile_dirproperty($onelink);
        $removefile{'InstallMode'} = get_removefile_installmode($onelink);

        my $oneline = $removefile{'FileKey'} . "\t" . $removefile{'Component_'} . "\t" . $removefile{'FileName'} . "\t"
                    . $removefile{'DirProperty'} . "\t" . $removefile{'InstallMode'} . "\n";

        push(@removefiletable, $oneline);
    }

    # Saving the file

    my $removefiletablename = $basedir . $installer::globals::separator . "RemoveFi.idt";
    installer::files::save_file($removefiletablename ,\@removefiletable);
    my $infoline = "Created idt file: $removefiletablename\n";
    push(@installer::globals::logfileinfo, $infoline);

}

1;