:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#*************************************************************************
#
#   $RCSfile: mkout.pl,v $
#
#   $Revision: 1.1 $
#
#   last change: $Author: hr $ $Date: 2001-04-19 15:43:15 $
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
# mkout - create output tree
#

use Cwd;
use Getopt::Std;
use File::Path;

#### script id #####

( $script_name = $0 ) =~ s/^.*\W(\w+)$/$1/;

$id_str = ' $Revision: 1.1 $ ';
$id_str =~ /Revision:\s+(\S+)\s+\$/
  ? ($script_rev = $1) : ($script_rev = "-");

print "$script_name -- version: $script_rev\n";

#### globals ####

$is_debug   = 0;

$base_dir   = 0;            # path to module base directory
$dir_mode   = 0755;         # default directory creation mode

$envpath    = 0;            # platform/product combination
$opt_r      = 0;            # create 'remote' subdirs

%sub_dirs   = (
#               dirname    remote(yes/no)
                'bin'   => 1,
                'class' => 0,
                'inc'   => 0,
                'lib'   => 1,
                'misc'  => 1,
                'obj'   => 1,
                'res'   => 1,
                'slb'   => 1,
                'slo'   => 1,
                'srs'   => 1
            );

#### main ####

parse_options();
init_globals();
create_dirs();

exit(0);

#### subroutines #####

sub parse_options {
    my $rc;

    $rc = getopts('r');

    if ( !$rc || $#ARGV > 0 ) {
        usage();
        exit(1);
    }
    $envpath = $ARGV[0] if defined($ARGV[0]);
}

sub init_globals {
    my $umask;
    $base_dir =  get_base();
    print "Base_Diri=$base_dir\n" if $is_debug;

    $umask = umask();
    if ( defined($umask) ) {
        $dir_mode = 0777 - $umask;
    }
    $envpath = $ENV{INPATH} unless $envpath;

    if ( !$envpath ) {
        print_error("can't determine platform/environment");
        exit(3);
    }
    print "Platform/Environment: $envpath\n" if $is_debug;
}

sub get_base {
    # a module base dir contains a subdir 'prj'
    # which in turn contains a file 'd.lst'
    my (@field, $base, $dlst);
    my $path = cwd();

    @field = split(/\//, $path);

    while ( $#field != -1 ) {
        $base = join('/', @field);
        $dlst = $base . '/prj/d.lst';
        last if -e $dlst;
        pop @field;
    }

    if ( $#field == -1 ) {
        print_error("can't determine module");
        exit(2);
    }
    else {
        return $base;
    }
}

sub create_dirs {
    foreach $dir ( keys %sub_dirs ) {
        $path = $base_dir . '/' . $envpath . '/' . $dir;
        if ( $opt_r && $sub_dirs{$dir} ) {
            $path .= "/remote";
        }
        mkpath($path, 0, $dir_mode);
        print "Create path: $path\n" if $is_debug;
    }
}

sub print_error {
    my $message = shift;

    print STDERR "$script_name: ERROR: $message\n";
}

sub usage {
    print STDERR "Usage:\n$script_name [-r] [platform/environment]\n";
    print STDERR "Options:\n  -r create 'remote' directories\n";
}

# vim: set ts=4 shiftwidth=4 expandtab syntax=perl:
