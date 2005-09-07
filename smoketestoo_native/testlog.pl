:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: testlog.pl,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: rt $ $Date: 2005-09-07 21:29:26 $
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

### globals ###

$is_debug = 0;

@logfiles_list = ("swlog.dat",
                "smalog.dat",
                "shptlog.dat",
                "sdrwlog.dat",
                "sdlog.dat",
                "sclog.dat",
        "javalog.dat"
);

$dont_kill ="dont_deinstall";
$error_str = "error";
$tests_complete = "---";
$gui = $ENV{GUI};
$inpath = $ENV{INPATH};
$cygwin = "cygwin";

if ($^O =~ /cygwin/) {
        $gui = $cygwin;
}

if (($gui eq "UNX") or ($gui eq $cygwin)) {
    $pathslash = "/";
}
else
{
    $pathslash = "\\";
}

$misc = $inpath . $pathslash . "misc" . $pathslash;
$okfile = $misc . "ok.bat";
umask (02);

### sub routines ###

sub test_logfile {
    my ($file) = shift;
    my ($line, $failed, $complete);

    $failed = 0;
    $complete = 0;

    if (! -e $file) {
        print "$file: error: file is missing\n";
        return (0);
    }

    open TABLE, "<$file" or die "Error: can�t open log file $file]";

    while(<TABLE>) {
        $line = $_;
        chomp $line;
        if ( $line =~ /$error_str/ ) {
             print "$file: $line\n";
             $failed = 1;
        }
        if ( $line =~ /$tests_complete/ ) {
             $complete = 1;
             print "$file: $line\n" if $is_debug;
        }
        else
        {
             print "$file: $line\n" if $is_debug;
        }
    }

    close TABLE;

    print "$failed $complete\n" if $is_debug;

    if (!$complete) {
        print "$file: error: the test was not complete!\n"
    }

    if (!$failed && $complete) {
        print "true\n" if $is_debug;
        return (1);
    }
    else
    {
        print "false\n" if $is_debug;
        return (0);
    }
}


### main ###

$idStr = ' $Revision: 1.2 $ ';
$idStr =~ /Revision:\s+(\S+)\s+\$/
  ? ($cpflat2minor_rev = $1) : ($cpflat2minor_rev = "-");

print "TestLog -- Version: $cpflat2minor_rev\n";

if (-e $okfile) {
        unlink ($okfile);
}

if (  ($#ARGV >-1) && ($#ARGV < 1) ) {
    $ARGV[0] =~ s/\"//g;
}

if ( ! ( ($#ARGV < 1) && $ARGV[0] && (-d $ARGV[0]) ) ) {
    print "Error! Usage: testlog <log_directory>\n" ;
    exit(1);
}

$logfiledir = $ARGV[0];
$is_testerror = 0;

print "@logfiles_list\n" if $is_debug;

for $i (0..$#logfiles_list) {
    $current_file = $logfiledir . $pathslash . $logfiles_list[$i];
    if (! test_logfile ($current_file)) {
        $is_testerror = 1;
    }
}

# write file to prevent deinstallation of office
if ($is_testerror) {
    $dont_del_file = $logfiledir . $pathslash . $dont_kill;
    open (ERRFILE, ">$dont_del_file");
    print ERRFILE "dont delete flag";
    close (ERRFILE);
}
else {
    print "$okfile\n" if $is_debug;
    open( OKFILE, ">$okfile");
    print OKFILE "echo ok!\n";
    close( OKFILE );
    chmod (0775, "$okfile");
}

exit($is_testerror);
