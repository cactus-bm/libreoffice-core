:
eval 'exec perl -wS $0 ${1+"$@"}'
    if 0;
#*************************************************************************
#
#   $RCSfile: guw.pl,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: hjs $ $Date: 2002-07-17 15:45:02 $
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
#   Contributor(s): Volker Quetschke <quetschke@scytek.de>
#
#
#
#*************************************************************************
# Description: ??

# Attention!!! Avoid dos-style lineendings (\r\n) in parameter files (@filename)
# No, replace_cyg takes care for this.

#---------------------------------------------------------------------------
# global vars
@params = ();

# set debug mode here:
#$debug="true";

#---------------------------------------------------------------------------
# Define known parameter exceptions
%knownpara = ( 'echo', [ '/TEST', 'QQQ', 'CCC' ],
               'lib', [ 'OUT:', 'EXTRACT:','out:', 'def:', 'machine:' ],
               'link', [ 'BASE:', 'DEBUG', 'DLL', 'LIBPATH', 'MACHINE:',
                         'MAP', 'NODEFAULTLIB', 'OPT', 'PDB', 'RELEASE',
                         'SUBSYSTEM', 'STACK', 'out:', 'map:', 'ENTRY:',
                         'implib:', 'def' ],
               'regcomp', [ '-env:', 'vnd.sun.star.expand:' ],
               'regmerge', [ '/UCR' ] );

#---------------------------------------------------------------------------
# procedures

#----------------------------------------------------------
# Function name: WinFormat
# Description:   Format variables to Windows Format.
# Arguments:     1. Variable (string)
# Return value:  Reformatted String
#----------------------------------------------------------
sub WinFormat {
  my $variable = shift @_;
  my( $d1, $d2 );
  $variable =~ s/(\$\w+)/$1/eeg ; # expand the variables
  $variable =~ s/(\$\w+)/$1/eeg ; # expand the variables twice!
  $variable =~ s/:/;/g;
  $variable =~ s/([;]|\A)(\w);/$1$2:/g; # get back the drives
  # Search for posix path ;entry; and replace with cygpath -w entry
#  while ( $variable =~ /(?:[;]|\A)((?:\/[\w\.~]+)+(?:[;]|\Z))/ ) { # Normal paths
  while ( $variable =~ /(?:[;]|\A)((?:\/[\w\.\-~]+)+(?:[;]|\Z))/ ) { # Normal paths
    if ( defined $debug ) { print(STDERR "WinFormat:\nnormal path:\n$variable\n");};
    $d1 = $1 ;
    chomp( $d2 = qx{cygpath -w "$d1"} ) ;
    $variable =~ s/$d1/$d2/ ;
  }
  while ( $variable =~ /(?:-\w)((?:\/[\w\.\-~]+)+(?:\s|\Z))/ ) { # Include paths
    if ( defined $debug ) { print(STDERR "WinFormat:\ninclude path:\n$variable\n");};
    $d1 = $1 ;
    chomp( $d2 = qx{cygpath -w "$d1"} ) ;
    $variable =~ s/$d1/$d2/ ;
  }
  $variable =~ s/\//\\/g; # Remaining \ come from e.g.: ../foo/baa
  $variable =~ s/^\\$/\//g; # a single "/" needs to be preserved

  if ( defined $debug ) { print(STDERR "WinFormat:\nresult:\n$variable\n");};
  return $variable;
}

#----------------------------------------------------------
# Function name: replace_cyg
# Description:   Process all arguments and change them to Windows Format.
# Arguments:     Reference to array with arguments
# Return value:  -
#----------------------------------------------------------
sub replace_cyg {
    my $args = shift;
    my @cmd_file;
    foreach my $para ( @$args )
      {
        if ( $para =~ "^@" ) {
          # it's a command file
          if ( defined $debug ) { print(STDERR "----------------------------\n");};
          $para =~ s/^@//;
          $filename = $para;
          if ( defined $debug ) { print(STDERR "filename = $filename \n");};
          # open this command file for reading
          open(CMD, "$filename");
          while ( <CMD> ) {
            # Remove DOS lineendings. Bug in Cygwin / Perl?
            $_ =~ s/\r//g;
            # Fill all tokens into array
            push( @cmd_file, split(/[ \t\n]+/,$_));
          }
          close(CMD);
          # reformat all tokens
          replace_cyg(\@cmd_file);
          if ( defined $debug ) { print(STDERR "Tokens processed:\n");};
          foreach $i (@cmd_file) {
            if ( defined $debug ) { print(STDERR "!".$i."!\n");};
          }
          # open this filename for writing (truncate) Textmode?
          open(CMD, '>', $filename);
          # write all tokens back into this file
          print(CMD join(' ', @cmd_file));
          close(CMD);
          # convert filename to dos style
          $para = WinFormat( $para );
          if ( defined $debug ) { print(STDERR "----------------------------\n");};
          $para = "@".$para;
        } else {
          # it's just a parameter
          if ( defined $debug ) { print(STDERR "\nParameter:---${para}---\n");};
          # If $tmp1 is empty then $para is a parameter.
          my $is_no_para = 1;
          foreach $i (@{$knownpara{$command}}) {
            if( $para =~ /$i/ ) {
              $is_no_para = 0;
              if ( defined $debug ) { print(STDERR "Is parameter exception:${para}:\n" );};
              last;
            }
          }
          if( $is_no_para ) {
            $para = WinFormat($para);
          }
          if ( defined $debug ) { print(STDERR "Converted line:${para}:\n" );};
        } # else
      } # foreach loop
  }

#---------------------------------------------------------------------------
# main
@params = @ARGV;

$command = shift(@params);
print( STDERR "Command: $command\n" );

replace_cyg(\@params);
if ( defined $debug ) { print(STDERR "\n---------------------\nExecute: $command @params\n");};
#print( STDERR "$command", @params);
exec( "$command", @params);
