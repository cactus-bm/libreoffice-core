#*************************************************************************
#
#   $RCSfile: files.pm,v $
#
#   $Revision: 1.4 $
#
#   last change: $Author: hr $ $Date: 2004-12-15 09:54:41 $
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

package installer::files;

use installer::exiter;
use installer::logger;

############################################
# File Operations
############################################

sub check_file
{
    my ($arg) = @_;

    if ( $installer::globals::debug ) { installer::logger::debuginfo("installer::files::check_file : $arg"); }

    if(!( -f $arg ))
    {
        installer::exiter::exit_program("ERROR: Cannot find file $arg", "check_file");
    }
}

sub read_file
{
    my ($localfile) = @_;

    if ( $installer::globals::debug ) { installer::logger::debuginfo("installer::files::read_file : $localfile"); }

    open( IN, "<$localfile" ) || installer::exiter::exit_program("ERROR: Cannot open file $localfile for reading", "read_file");
    my @localfile = <IN>;
    close( IN );

    return \@localfile;
}

###########################################
# Saving files, arrays and hashes
###########################################

sub save_file
{
    my ($savefile, $savecontent) = @_;

    if ( $installer::globals::debug ) { installer::logger::debuginfo("installer::files::save_file : $savefile : $#{$savecontent}"); }

    open( OUT, ">$savefile" ) || installer::exiter::exit_program("ERROR: Cannot open file $savefile for writing", "save_file");
    print OUT @{$savecontent};
    close( OUT);
}

sub save_hash
{
    my ($savefile, $hashref) = @_;

    if ( $installer::globals::debug ) { installer::logger::debuginfo("installer::files::save_hash : $savefile"); }

    my @printcontent = ();

    my $itemkey;

    foreach $itemkey ( keys %{$hashref} )
    {
        my $line = "";
        my $itemvalue = $hashref->{$itemkey};
        $line = $itemkey . "=" . $itemvalue . "\n";
        push(@printcontent, $line);
    }

    open( OUT, ">$savefile" ) || installer::exiter::exit_program("ERROR: Cannot open file $savefile for writing", "save_hash");
    print OUT @printcontent;
    close( OUT);
}

sub save_array_of_hashes
{
    my ($savefile, $arrayref) = @_;

    if ( $installer::globals::debug ) { installer::logger::debuginfo("installer::files::save_array_of_hashes : $savefile : $#{$arrayref}"); }

    my @printcontent = ();

    for ( my $i = 0; $i <= $#{$arrayref}; $i++ )
    {
        my $line = "";
        my $hashref = ${$arrayref}[$i];
        my $itemkey;

        foreach $itemkey ( keys %{$hashref} )
        {
            my $itemvalue = $hashref->{$itemkey};
            $line = $line . $itemkey . "=" . $itemvalue . "\t";
        }

        $line = $line . "\n";

        push(@printcontent, $line);
    }

    open( OUT, ">$savefile" ) || installer::exiter::exit_program("ERROR: Cannot open file $savefile for writing", "save_array_of_hashes");
    print OUT @printcontent;
    close( OUT);
}

###########################################
# Binary file operations
###########################################

sub read_binary_file
{
    my ($filename) = @_;

    if ( $installer::globals::debug ) { installer::logger::debuginfo("installer::files::read_binary_file : $filename"); }

    my $file;

    open( IN, "<$filename" ) || installer::exiter::exit_program("ERROR: Cannot open file $filename for reading", "read_binary_file");
    binmode IN;
    seek IN, 0, 2;
    my $length = tell IN;
    seek IN, 0, 0;
    read IN, $file, $length;
    close IN;

    return $file;
}

sub save_binary_file
{
    my ($file, $filename) = @_;

    if ( $installer::globals::debug ) { installer::logger::debuginfo("installer::files::save_binary_file : $filename"); }

    open( OUT, ">$filename" ) || installer::exiter::exit_program("ERROR: Cannot open file $filename for writing", "save_binary_file");
    binmode OUT;
    print OUT $file;
    close OUT;
}

1;
