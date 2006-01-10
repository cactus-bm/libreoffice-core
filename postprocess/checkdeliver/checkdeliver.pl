:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: checkdeliver.pl,v $
#
#   $Revision: 1.7 $
#
#   last change: $Author: rt $ $Date: 2006-01-10 16:41:19 $
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
#
#
# checkdeliver.pl - compare delivered files on solver with those on SRC_ROOT
#

use strict;
use Getopt::Long;
use File::stat;

#### globals #####

my $err       = 0;
my $srcdir    = '';
my $solverdir = '';
my $platform  = '';
my $milestoneext = '';
my @exceptionmodlist = ("postprocess", "instset.*native"); # modules not yet delivered

#### main #####

print "checkdeliver.pl - checking delivered binaries\n";

get_globals();                                  # get global variables
my $deliverlists_ref = get_deliver_lists();     # get deliver log files
foreach my $listfile ( @$deliverlists_ref ) {
    $err += check( $listfile );                 # check delivered files
}
print "OK\n" if ( ! $err );
exit $err;

#### subroutines ####

sub get_globals
# set global variables using environment variables and command line options
{
    my $help;

    # set global variables according to environnment
    $platform      = $ENV{INPATH};
    $srcdir        = $ENV{SOLARSRC};
    $solverdir     = $ENV{SOLARVERSION};
    $milestoneext  = $ENV{UPDMINOREXT};

    # override environment with command line options
    GetOptions('help' => \$help,
               'p=s'  => \$platform
    ) or usage (1);

    if ( $help ) {
        usage(0);
    }

    #do some sanity checks
    if ( ! ( $platform && $srcdir && $solverdir ) ) {
        die "Error: please set environment\n";
    }
    if ( ! -d $srcdir ) {
        die "Error: cannot find source directory '$srcdir'\n";
    }
    if ( ! -d $solverdir ) {
        die "Error: cannot find solver directory '$solverdir'\n";
    }
}

sub get_deliver_lists
# find deliver log files on solver
{
    my @files;
    my $pattern = "$solverdir/$platform/inc";
    $pattern .= "$milestoneext" if ( $milestoneext );
    $pattern .= "/*/deliver.log";

    if ( $^O =~ /cygwin/i && $ENV{'USE_SHELL'} eq "4nt" )
    {   # glob from cygwin's perl needs $pattern to use only slashes.
        # (DOS style path are OK as long as slashes are used.)
        $pattern =~ s/\\/\//g;
    }

    @files = glob( $pattern );
    # do not check modules not yet built
    foreach my $exceptionpattern ( @exceptionmodlist ) {
        @files = grep ! /\/$exceptionpattern\//, @files;
    }
    if ( ! @files ) {
        die "Error: cannot find deliver log files";
    }
    return \@files;
}

sub check
# reads deliver log file given as parameter and compares pairs of files listed there.
{
    my $listname = shift;
    my $error = 0;
    my %delivered;
    my $module;
    my $islinked = 0;
    if ( $listname =~ /\/(\w+?)\/deliver.log$/o ) {
        $module = $1;
    } else {
        print STDERR "Error: cannot determine module name from \'$listname\'\n";
        return 1;
    }
    # read deliver log file
    open( DELIVERLOG, "< $listname" ) or die( "Error: cannot open file \'$listname\'\n$!");
    foreach ( <DELIVERLOG> ) {
        next if ( /^LINK / );
        # For now we concentrate on binaries, located in 'bin' or 'lib'.
        # It probably is a good idea to check all files but this requires some
        # d.lst cleanup which is beyond the current scope. -> TODO
        next if ( ! / $module\/$platform\/[bl]i[nb]\// );
        chomp;
        if ( /^\w+? (\S+) (\S+)\s*$/o ) {
            $delivered{$1} = $2;
        } else {
            print STDERR "Warning: cannot parse \'$listname\' line\n\'$_\'\n";
        }
    }
    close( DELIVERLOG );
    # on CWS modules not added can exist as links. For windows it may happen that these
    # links cannot be resolved (when working with nfs mounts). This prevents checking,
    # but is not an error.
    if ( $ENV{CWS_WORK_STAMP} ) {
        if ( ! -d "$srcdir/$module" ) {
            if ( -e "$srcdir/$module.lnk/prj/d.lst") {
                # module is linked, link can be resolved
                $islinked = 1;
            } else {
                # print "Cannot find module '$module', no checking\n";
                return $error;
            }
        }
    }
    # compare all delivered files with their origin
    # no strict 'diff' allowed here, as deliver may alter files (hedabu, strip, ...)
    foreach my $file ( sort keys %delivered ) {
        my $ofile = "$srcdir/$file";
        my $sfile = "$solverdir/$delivered{$file}";
        # on CWS modules may exist as link only, named <module>.lnk
        if ( $islinked ) {
            $ofile =~ s/\/$module\//\/$module.lnk\//;
        }
        if ( $milestoneext ) {
            # deliver log files do not contain milestone extension on solver
            $sfile =~ s/\/$platform\/(...)\//\/$platform\/$1$milestoneext\//;
        }
        # compare files, not directories or links
        next if ( -d $ofile );
        next if ( -l $sfile );

        if ( -e $ofile && -e $sfile ) {
            my $orgfile_stats = stat($ofile);
            my $delivered_stats = stat($sfile);
            if ( $platform =~ /^wntmsci/ ) {
                # For windows 'rebase' may alter file dates, but file
                # size should keep constant.
                if ( $orgfile_stats->size ne $delivered_stats->size ) {
                    print STDERR "\nError: ";
                    print STDERR "file sizes differ for $ofile $sfile\n";
                    $error ++;
                }
            } else {
                # Unix like platforms strip binaries. Stripping reduces file
                # size but does not change modification time.
                if ( ( $orgfile_stats->mtime - $delivered_stats->mtime ) gt 1 ) {
                    print STDERR "Error: ";
                    print STDERR "file dates differ for $ofile $sfile\n";
                    $error ++;
                }
            }
        } else {
            print STDERR "Error: no such file '$ofile'\n" if ( ! -e $ofile );
            print STDERR "Error: no such file '$sfile'\n" if ( ! -e $sfile );
            $error ++;
        }
    }
    if ( $error ) {
        print STDERR "Errors found: Module '$module' not delivered correctly?\n\n";
    }
    return $error;
}

sub usage
# print usage message and exit
{
    my $retval = shift;
    print STDERR "Usage: checkdeliver.pl [-h] [-p <platform>]\n";
    print STDERR "Compares delivered files on solver with original ones on SRC_ROOT\n";
    print STDERR "Options:\n";
    print STDERR "    -h              print this usage message\n";
    print STDERR "    -p platform     specify platform\n";

    exit $retval;
}
