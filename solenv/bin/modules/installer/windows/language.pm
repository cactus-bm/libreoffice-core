#*************************************************************************
#
#   $RCSfile: language.pm,v $
#
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

package installer::windows::language;

use installer::exiter;

####################################################
# Determining the Windows language (LCID)
# English: 1033
####################################################

sub get_windows_language
{
    my ($language) = @_;

    my $windowslanguage = "";

    if ( $installer::globals::msilanguage->{$language} ) { $windowslanguage = $installer::globals::msilanguage->{$language}; }

    if ( $windowslanguage eq "" ) { installer::exiter::exit_program("ERROR: Unknown language $language in function get_windows_language", "get_windows_language"); }

    return $windowslanguage;
}

####################################################
# Determining the Windows language ANSI-Codepage
# English: 1252
####################################################

sub get_windows_encoding
{
    my ($language) = @_;

    my $windowsencoding = "";

    if ( $installer::globals::msiencoding->{$language} ) { $windowsencoding = $installer::globals::msiencoding->{$language}; }

    if ( $windowsencoding eq "" ) { installer::exiter::exit_program("ERROR: Unknown language $language in function get_windows_encoding", "get_windows_encoding"); }

    if ( $windowsencoding eq "0" ) { $windowsencoding = "65001"; }  # languages with "0" have to be available in UTF-8 (65001)

    # Asian multilingual installation sets need a code neutral Windows Installer database -> $windowsencoding = 0
    if (( $language eq "en-US" ) && (( $installer::globals::product =~ /suitemulti/i ) || ( $installer::globals::product =~ /officemulti/i ))) { $windowsencoding = "0"; }

    return $windowsencoding;
}

1;