#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: directory.pm,v $
#
#   $Revision: 1.15 $
#
#   last change: $Author: rt $ $Date: 2005-10-18 08:26:58 $
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

package installer::windows::directory;

use installer::files;
use installer::globals;
use installer::pathanalyzer;
use installer::windows::idtglobal;

##############################################################
# Adding unique directory names to the directory collection
##############################################################

sub create_unique_directorynames
{
    my ($directoryref) = @_;

    for ( my $i = 0; $i <= $#{$directoryref}; $i++ )
    {
        my $onedir = ${$directoryref}[$i];
        my $uniquename = $onedir->{'HostName'};
        # get_path_from_fullqualifiedname(\$uniqueparentname);
        # making /registry/schema/org/openoffice/VCL.xcs to VCL.xcs

        $uniquename =~ s/\_//g;                 # removing existing underlines
        $uniquename =~ s/\.//g;                 # removing dots in directoryname
        $uniquename =~ s/\Q$installer::globals::separator\E/\_/g;   # replacing slash and backslash with underline

        my $uniqueparentname = $uniquename;

        if ( $uniqueparentname =~ /^\s*(.*)\_(.*?)\s*$/ )   # the underline is now the separator
        {
            $uniqueparentname = $1;
        }
        else
        {
            $uniqueparentname = "INSTALLLOCATION";
        }

        $uniquename =~ s/\-/\_/g;           # making "-" to "_"
        $uniqueparentname =~ s/\-/\_/g;     # making "-" to "_"

        $onedir->{'uniquename'} = $uniquename;
        $onedir->{'uniqueparentname'} = $uniqueparentname;

    }
}

#####################################################
# Creating the defaultdir for the file Director.idt
#####################################################

sub create_defaultdir_directorynames
{
    my ($directoryref) = @_;

    my @shortnames = ();

    for ( my $i = 0; $i <= $#{$directoryref}; $i++ )
    {
        my $onedir = ${$directoryref}[$i];
        my $hostname = $onedir->{'HostName'};
        installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$hostname);   # making program/classes to classes
        my $shortstring = installer::windows::idtglobal::make_eight_three_conform($hostname, "dir", \@shortnames);

        my $defaultdir;

        if ( $shortstring eq $hostname )
        {
            $defaultdir = $hostname;
        }
        else
        {
            $defaultdir = $shortstring . "|" . $hostname;
        }

        $onedir->{'defaultdir'} = $defaultdir;

        my $fontdir = "";
        if ( $onedir->{'Dir'} ) { $fontdir = $onedir->{'Dir'}; }

        my $fontdefaultdir = "";
        if ( $onedir->{'defaultdir'} ) { $fontdefaultdir = $onedir->{'defaultdir'}; }

        if (( $fontdir eq "PREDEFINED_OSSYSTEMFONTDIR" ) && ( $fontdefaultdir eq $installer::globals::fontsdirhostname ))
        {
            $installer::globals::fontsdirname = $onedir->{'defaultdir'};
            $installer::globals::fontsdirparent = $onedir->{'uniqueparentname'};
        }
    }
}

###############################################
# Fill content into the directory table
###############################################

sub create_directorytable_from_collection
{
    my ($directorytableref, $directoryref) = @_;

    for ( my $i = 0; $i <= $#{$directoryref}; $i++ )
    {
        my $onedir = ${$directoryref}[$i];
        my $hostname = $onedir->{'HostName'};
        my $dir = "";

        if ( $onedir->{'Dir'} ) { $dir = $onedir->{'Dir'}; }

        if (( $dir eq "PREDEFINED_PROGDIR" ) && ( $hostname eq "" )) { next; }  # removing files from root directory

        my $oneline = $onedir->{'uniquename'} . "\t" . $onedir->{'uniqueparentname'} . "\t" . $onedir->{'defaultdir'} . "\n";

        push(@{$directorytableref}, $oneline);
    }
}

###############################################
# Defining the root installation structure
###############################################

sub add_root_directories
{
    my ($directorytableref, $allvariableshashref) = @_;

    my $oneline = "TARGETDIR\t\tSourceDir\n";
    push(@{$directorytableref}, $oneline);

    my $sourcediraddon = "";
    if (($installer::globals::addchildprojects) || ($installer::globals::patch)) { $sourcediraddon = "\:\."; }

    if (!($installer::globals::product =~ /ada/i )) # the following directories not for ada products
    {
        $oneline = "$installer::globals::programfilesfolder\tTARGETDIR\t.\n";
        push(@{$directorytableref}, $oneline);

        # my $manufacturer = $installer::globals::manufacturer;
        # my $shortmanufacturer = installer::windows::idtglobal::make_eight_three_conform($manufacturer, "dir");    # third parameter not used
        # $shortmanufacturer =~ s/\s/\_/g;                                  # changing empty space to underline

        my $productname = $allvariableshashref->{'PRODUCTNAME'};
        my $productversion = $allvariableshashref->{'PRODUCTVERSION'};
        my $productkey = $productname . " " . $productversion;

        if ( $allvariableshashref->{'NOVERSIONINDIRNAME'} ) { $productkey = $productname; }

        my $shortproductkey = installer::windows::idtglobal::make_eight_three_conform($productkey, "dir");      # third parameter not used
        $shortproductkey =~ s/\s/\_/g;                                  # changing empty space to underline

        if ( $allvariableshashref->{'SUNDIR'} )
        {
            $oneline = "sundirectory\t$installer::globals::programfilesfolder\t$installer::globals::sundirname$sourcediraddon\n";
            push(@{$directorytableref}, $oneline);

            $oneline = "INSTALLLOCATION\tsundirectory\t$shortproductkey|$productkey$sourcediraddon\n";
            push(@{$directorytableref}, $oneline);
        }
        else
        {
            $oneline = "INSTALLLOCATION\t$installer::globals::programfilesfolder\t$shortproductkey|$productkey$sourcediraddon\n";
            push(@{$directorytableref}, $oneline);
        }

        $oneline = "$installer::globals::programmenufolder\tTARGETDIR\t.\n";
        push(@{$directorytableref}, $oneline);

        $oneline = "$installer::globals::officemenufolder\t$installer::globals::programmenufolder\t$shortproductkey|$productkey\n";
        push(@{$directorytableref}, $oneline);

        $oneline = "$installer::globals::startupfolder\tTARGETDIR\t.\n";
        push(@{$directorytableref}, $oneline);

        $oneline = "$installer::globals::templatefolder\tTARGETDIR\t$installer::globals::templatefoldername\n";
        push(@{$directorytableref}, $oneline);

        if ( $installer::globals::fontsdirname )
        {
            $oneline = "$installer::globals::fontsfolder\t$installer::globals::fontsdirparent\t$installer::globals::fontsfoldername\:$installer::globals::fontsdirname\n";
        }
        else
        {
            $oneline = "$installer::globals::fontsfolder\tTARGETDIR\t$installer::globals::fontsfoldername\n";
        }

        push(@{$directorytableref}, $oneline);
    }

    if ($installer::globals::product =~ /ada/i )    # the following directories only for ada products
    {
        $oneline = "INSTALLLOCATION\tTARGETDIR\t$installer::globals::adafolder\n";
        push(@{$directorytableref}, $oneline);
    }

}

###############################################
# Creating the file Director.idt dynamically
###############################################

sub create_directory_table
{
    my ($directoryref, $basedir, $allvariableshashref) = @_;

    # Structure of the directory table:
    # Directory Directory_Parent DefaultDir
    # Directory is a unique identifier
    # Directory_Parent is the unique identifier of the parent
    # DefaultDir is .:APPLIC~1|Application Data with
    # Before ":" : [sourcedir]:[destdir] (not programmed yet)
    # After ":" : 8+3 and not 8+3 the destination directory name

    my @directorytable = ();
    my $infoline;

    create_unique_directorynames($directoryref);
    create_defaultdir_directorynames($directoryref);    # only destdir!
    installer::windows::idtglobal::write_idt_header(\@directorytable, "directory");
    add_root_directories(\@directorytable, $allvariableshashref);
    create_directorytable_from_collection(\@directorytable, $directoryref);

    # Saving the file

    my $directorytablename = $basedir . $installer::globals::separator . "Director.idt";
    installer::files::save_file($directorytablename ,\@directorytable);
    $infoline = "Created idt file: $directorytablename\n";
    push(@installer::globals::logfileinfo, $infoline);

}

1;
