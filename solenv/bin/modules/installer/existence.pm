#*************************************************************************
#
#   $RCSfile: existence.pm,v $
#
#   $Revision: 1.3 $
#
#   last change: $Author: rt $ $Date: 2005-04-04 09:59:04 $
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

package installer::existence;

#############################
# Test of existence
#############################

sub exists_in_array
{
    my ($searchstring, $arrayref) = @_;

    my $alreadyexists = 0;

    for ( my $i = 0; $i <= $#{$arrayref}; $i++ )
    {
        if ( ${$arrayref}[$i] eq $searchstring)
        {
            $alreadyexists = 1;
            last;
        }
    }

    return $alreadyexists;
}

sub exists_in_array_of_hashes
{
    my ($searchkey, $searchvalue, $arrayref) = @_;

    my $valueexists = 0;

    for ( my $i = 0; $i <= $#{$arrayref}; $i++ )
    {
        my $hashref = ${$arrayref}[$i];

        if ( $hashref->{$searchkey} eq $searchvalue )
        {
            $valueexists = 1;
            last;
        }
    }

    return $valueexists;
}

#####################################################################
# Returning a specified file as base for the new
# configuration file, defined by its "gid"
#####################################################################

sub get_specified_file
{
    my ($filesarrayref, $searchgid) = @_;

    my $foundfile = 0;
    my $onefile;

    for ( my $i = 0; $i <= $#{$filesarrayref}; $i++ )
    {
        $onefile = ${$filesarrayref}[$i];
        my $filegid = $onefile->{'gid'};

        if ( $filegid eq $searchgid )
        {
            $foundfile = 1;
            last;
        }
    }

    if (!($foundfile))
    {
        installer::exiter::exit_program("ERROR: Could not find file $searchgid in list of files!", "get_specified_file");
    }

    return $onefile;
}

#####################################################################
# Returning a specified file as base for a new file,
# defined by its "Name"
#####################################################################

sub get_specified_file_by_name
{
    my ($filesarrayref, $searchname) = @_;

    my $foundfile = 0;
    my $onefile;

    for ( my $i = 0; $i <= $#{$filesarrayref}; $i++ )
    {
        $onefile = ${$filesarrayref}[$i];
        my $filename = $onefile->{'Name'};

        if ( $filename eq $searchname )
        {
            $foundfile = 1;
            last;
        }
    }

    if (!($foundfile))
    {
        installer::exiter::exit_program("ERROR: Could not find file $searchname in list of files!", "get_specified_file_by_name");
    }

    return $onefile;
}

#####################################################################
# Checking existence of a specific file, defined by its "Name"
#####################################################################

sub filename_exists_in_filesarray
{
    my ($filesarrayref, $searchname) = @_;

    my $foundfile = 0;

    for ( my $i = 0; $i <= $#{$filesarrayref}; $i++ )
    {
        my $onefile = ${$filesarrayref}[$i];
        my $filename = $onefile->{'Name'};

        if ( $filename eq $searchname )
        {
            $foundfile = 1;
            last;
        }
    }

    return $foundfile;
}

#####################################################################
# Checking existence of a specific file, defined by its "gid"
#####################################################################

sub filegid_exists_in_filesarray
{
    my ($filesarrayref, $searchgid) = @_;

    my $foundfile = 0;

    for ( my $i = 0; $i <= $#{$filesarrayref}; $i++ )
    {
        my $onefile = ${$filesarrayref}[$i];
        my $filegid = $onefile->{'gid'};

        if ( $filegid eq $searchgid )
        {
            $foundfile = 1;
            last;
        }
    }

    return $foundfile;
}

1;
