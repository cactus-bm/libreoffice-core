#*************************************************************************
#
#   $RCSfile: macro.pl,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: kz $ $Date: 2005-01-13 19:19:33 $
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

my $completelangiso_var = $ENV{completelangiso_var};
my $outfile = "";

if ( !defined $completelangiso_var) {
    print STDERR "ERROR: No language defined!\n";
    exit 1;
}

startup_check();
if ( "$completelangiso_var" eq "$lastcompletelangiso_var" ) {
    print STDERR "No new languages. Keeping old file\n";
    exit 0;
}

my @completelangiso = split " +", $completelangiso_var;

open OUTFILE, ">$outfile" or die "$0 ERROR: cannot open $outfile for writing!\n";

print OUTFILE "// generated file, do not edit\n\n";
print OUTFILE "// languages used for last time generation\n";
print OUTFILE "// completelangiso: $completelangiso_var\n\n";
write_ALL_LANG();
write_OTHER_LANGS();
write_DIR_ISOLANGUAGE_ALL_LANG_2();
write_DIR_ISOLANGUAGE_ALL_LANG();
write_DIR_IDENT_ALL_LANG();
write_EXTRA_ALL_LANG();
write_EXTRA_IDENT_ALL_LANG();
write_RESFILE_ALL_LANG();
write_SHORT_RESFILE_ALL_LANG();
write_README_ALL_LANG();
write_README_TXT_ALL_LANG();
write_VALUE_ALL_LANG_LETTER_DIR();
write_KEY_ALL_LANG_LETTER();
write_FILE_ALL_LANG_LETTER();

close OUTFILE;


sub write_ALL_LANG
{
    print OUTFILE "#define ALL_LANG(ident, resid) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tident ($lang) = resid";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_OTHER_LANGS
{
    print OUTFILE "#define OTHER_LANGS ";
    foreach $lang (@completelangiso) {
        next if ( $lang eq "en-US");
        print OUTFILE "\\\n\tDosName ($lang) = \"$lang\"";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
        print OUTFILE "\n\n";
}

sub write_DIR_ISOLANGUAGE_ALL_LANG_2
{
    print OUTFILE "#define DIR_ISOLANGUAGE_ALL_LANG_2 ";
    print OUTFILE "\\\n\tDosName (en-US) = \"en-US\"; \\\n\t";
    print OUTFILE "OTHER_LANGS";
        print OUTFILE "\n\n";
}

sub write_DIR_ISOLANGUAGE_ALL_LANG
{
    print OUTFILE "#define DIR_ISOLANGUAGE_ALL_LANG ";
    print OUTFILE "\\\n\tDosName (en-US) = \"en\"; \\\n\t";
    print OUTFILE "OTHER_LANGS";
        print OUTFILE "\n\n";
}

sub write_DIR_IDENT_ALL_LANG
{
    print OUTFILE "#define DIR_IDENT_ALL_LANG(name) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tDosName ($lang) = STRING(name)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_EXTRA_ALL_LANG
{
    print OUTFILE "#define EXTRA_ALL_LANG(name) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tName ($lang) = EXTRAFILENAME(name,_$lang)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_EXTRA_IDENT_ALL_LANG
{
    print OUTFILE "#define EXTRA_IDENT_ALL_LANG(name) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tName ($lang) = STRING(name)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_RESFILE_ALL_LANG
{
    print OUTFILE "#define RESFILE_ALL_LANG(name) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tName ($lang) = RESFILENAME(name,$lang)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_SHORT_RESFILE_ALL_LANG
{
    print OUTFILE "#define SHORT_RESFILE_ALL_LANG(name) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tName ($lang) = SHORTRESFILENAME(name,$lang)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_README_ALL_LANG
{
    print OUTFILE "#define README_ALL_LANG(key, name) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tkey ($lang) = READMEFILENAME(name,_$lang)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_README_TXT_ALL_LANG
{
    print OUTFILE "#define README_TXT_ALL_LANG(key, name, ext) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tkey ($lang) = READMETXTFILENAME(name,_$lang,ext)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

# FIXME: Not used at all in OOo?, #i38597#
sub write_VALUE_ALL_LANG_LETTER_DIR
{
    print OUTFILE "#define VALUE_ALL_LANG_LETTER_DIR ";
    print OUTFILE "\\\n\tValue (en-US) = STRING(en);";
    foreach $lang (@completelangiso) {
        next if ( $lang eq "en-US");
        print OUTFILE "\\\n\tValue ($lang) = STRING($lang)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_KEY_ALL_LANG_LETTER
{
    print OUTFILE "#define KEY_ALL_LANG_LETTER ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tKey ($lang) = STRING($lang)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub write_FILE_ALL_LANG_LETTER
{
    print OUTFILE "#define FILE_ALL_LANG_LETTER(name, ext) ";
    foreach $lang (@completelangiso) {
        print OUTFILE "\\\n\tName ($lang) = CONFIGLANGFILENAME(name,$lang,ext)";
        print OUTFILE "; " if ( $lang ne $completelangiso[$#completelangiso]);
    }
    print OUTFILE "\n\n";
}

sub startup_check
{
    my $i;
    for ( $i=0; $i <= $#ARGV; $i++) {
        if ( "$ARGV[$i]" eq "-o" ) {
            if ( defined $ARGV[ $i + 1] ) {
                $outfile = $ARGV[ $i + 1];
            } else {
                usage();
            }
        }
    }
    usage() if $i<2;
    usage() if "$outfile" eq "";
    if ( -f "$outfile" ) {
        open OLDFILE, "$outfile" or die "$0 - ERROR: $outfiles exists but isn't readable.\n";
        while ( $line = <OLDFILE> ) {
            if ( $line =~ /^\/\/.*completelangiso:/ ) {
                $lastcompletelangiso_var = $line;
                chomp $lastcompletelangiso_var;
                $lastcompletelangiso_var =~ s/^\/\/.*completelangiso:\s*//;
                last;
            }

        }
        close OLDFILE;
    }
}

sub usage
{
    print STDERR "Generate language dependend macros use in *.scp files\n";
    print STDERR "perl $0 -o outputfile\n";
    exit  1;
}
