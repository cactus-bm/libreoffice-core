#*************************************************************************
#
#   $RCSfile: javainstaller.pm,v $
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
package installer::javainstaller;

use Cwd;
use installer::exiter;
use installer::files;
use installer::globals;
use installer::languages;
use installer::pathanalyzer;
use installer::scriptitems;
use installer::systemactions;
use installer::worker;

##############################################################
# Returning a specific language string from the block
# of all translations
##############################################################

sub get_language_string_from_language_block
{
    my ($language_block, $language, $oldstring) = @_;

    my $newstring = "";

    for ( my $i = 0; $i <= $#{$language_block}; $i++ )
    {
        if ( ${$language_block}[$i] =~ /^\s*$language\s*\=\s*\"(.*)\"\s*$/ )
        {
            $newstring = $1;
            last;
        }
    }

    if ( $newstring eq "" )
    {
        $language = "en-US";    # defaulting to english

        for ( my $i = 0; $i <= $#{$language_block}; $i++ )
        {
            if ( ${$language_block}[$i] =~ /^\s*$language\s*\=\s*\"(.*)\"\s*$/ )
            {
                $newstring = $1;
                last;
            }
        }
    }

    return $newstring;
}

##############################################################
# Returning the complete block in all languages
# for a specified string
##############################################################

sub get_language_block_from_language_file
{
    my ($searchstring, $languagefile) = @_;

    my @language_block = ();

    for ( my $i = 0; $i <= $#{$languagefile}; $i++ )
    {
        if ( ${$languagefile}[$i] =~ /^\s*\[\s*$searchstring\s*\]\s*$/ )
        {
            my $counter = $i;

            push(@language_block, ${$languagefile}[$counter]);
            $counter++;

            while (( $counter <= $#{$languagefile} ) && (!( ${$languagefile}[$counter] =~ /^\s*\[/ )))
            {
                push(@language_block, ${$languagefile}[$counter]);
                $counter++;
            }

            last;
        }
    }

    return \@language_block;
}

#######################################################
# Searching for the module name and description in the
# modules collector
#######################################################

sub get_module_name_description
{
    my ($modulesarrayref, $onelanguage, $gid, $type) = @_;

    my $found = 0;

    my $newstring = "";

    for ( my $i = 0; $i <= $#{$modulesarrayref}; $i++ )
    {
        my $onemodule = ${$modulesarrayref}[$i];

        if ( $onemodule->{'gid'} eq $gid )
        {
            my $typestring = $type . " " . "(" . $onelanguage . ")";
            if ( $onemodule->{$typestring} ) { $newstring = $onemodule->{$typestring}; }
            $found = 1;
        }

        if ( $found ) { last; }
    }

    # defaulting to english

    if ( ! $found )
    {
        my $defaultlanguage = "en-US";

        for ( my $i = 0; $i <= $#{$modulesarrayref}; $i++ )
        {
            my $onemodule = ${$modulesarrayref}[$i];

            if ( $onemodule->{'gid'} eq $gid )
            {
                my $typestring = $type . " " . "(" . $defaultlanguage . ")";
                if ( $onemodule->{$typestring} ) { $newstring = $onemodule->{$typestring}; }
                $found = 1;
            }

            if ( $found ) { last; }
        }
    }

    return $newstring;
}

#######################################################
# Setting the productname and productversion
#######################################################

sub set_productname_and_productversion
{
    my ($templatefile, $variableshashref) = @_;

    my $infoline = "\nSetting product name and product version in Java template file\n";
    push( @installer::globals::logfileinfo, $infoline);

    my $productname = $variableshashref->{'PRODUCTNAME'};
    my $productversion = $variableshashref->{'PRODUCTVERSION'};

    for ( my $i = 0; $i <= $#{$templatefile}; $i++ )
    {
        ${$templatefile}[$i] =~ s/\{PRODUCTNAME\}/$productname/g;
        ${$templatefile}[$i] =~ s/\{PRODUCTVERSION\}/$productversion/g;
    }

    $infoline = "End of: Setting product name and product version in Java template file\n\n";
    push( @installer::globals::logfileinfo, $infoline);
}

#######################################################
# Setting the localized Module name and description
#######################################################

sub set_component_name_and_description
{
    my ($templatefile, $modulesarrayref, $onelanguage) = @_;

    my $infoline = "\nSetting component names and description in Java template file\n";
    push( @installer::globals::logfileinfo, $infoline);

    for ( my $i = 0; $i <= $#{$templatefile}; $i++ )
    {
        # OOO_gid_Module_Prg_Wrt_Name
        # OOO_gid_Module_Prg_Wrt_Description

        my $oneline = ${$templatefile}[$i];
        my $oldstring = "";
        my $gid = "";
        my $type = "";

        if ( $oneline =~ /\b(OOO_gid_\w+)\b/ )
        {
            $oldstring = $1;

            $infoline = "Found: $oldstring\n";
            push( @installer::globals::logfileinfo, $infoline);

            if ( $oldstring =~ /^\s*OOO_(gid_\w+)_(\w+?)\s*$/ )
            {
                $gid = $1;
                $type = $2;
            }

            my $newstring = get_module_name_description($modulesarrayref, $onelanguage, $gid, $type);

            $infoline = "\tReplacing (language $onelanguage): OLDSTRING: $oldstring NEWSTRING $newstring\n";
            push( @installer::globals::logfileinfo, $infoline);

            ${$templatefile}[$i] =~ s/$oldstring/$newstring/;   # always substitute, even if $newstring eq ""
        }
    }

    $infoline = "End of: Setting component names and description in Java template file\n\n";
    push( @installer::globals::logfileinfo, $infoline);
}

#######################################################
# Translating the Java file
#######################################################

sub translate_javafile
{
    my ($templatefile, $languagefile, $onelanguage) = @_;

    for ( my $i = 0; $i <= $#{$templatefile}; $i++ )
    {
        my @allstrings = ();

        my $oneline = ${$templatefile}[$i];

        while ( $oneline =~ /\b(OOO_\w+)\b/ )
        {
            my $replacestring = $1;
            push(@allstrings, $replacestring);
            $oneline =~ s/$replacestring//;
        }

        my $oldstring;

        foreach $oldstring (@allstrings)
        {
            my $language_block = get_language_block_from_language_file($oldstring, $languagefile);
            my $newstring = get_language_string_from_language_block($language_block, $onelanguage, $oldstring);

            $newstring =~ s/\"/\\\"/g;  # masquerading the "
            $newstring =~ s/\\\\\"/\\\"/g;  # unmasquerading if \" was converted to \\" (because " was already masked)

            # if (!( $newstring eq "" )) { ${$idtfile}[$i] =~ s/$oldstring/$newstring/; }
            ${$templatefile}[$i] =~ s/$oldstring/$newstring/;   # always substitute, even if $newstring eq ""
        }
    }
}

###########################################################
# changing the language in the include pathes
###########################################################

sub change_language_in_pathes
{
    my ($localincludepatharrayref, $language, $firstlanguage) = @_;

    for ( my $i = 0; $i <= $#{$localincludepatharrayref}; $i++ )
    {
        ${$localincludepatharrayref}[$i] =~ s/\/$firstlanguage\//\/$language\//g;   # /en-US/ -> /de/
    }
}

###########################################################
# Returning the license file name for a defined language
###########################################################

sub get_licensefilesource
{
    my ($language, $firstlanguage, $filesref, $includepatharrayref) = @_;

    my $licensefilename = "LICENSE_" . $language;

    # The different license files for different languages are not in a language specific include path
    # -> the incudepatharray for the first language is sufficient to find all license files
    # my $localincludepatharrayref = installer::worker::copy_array_from_references($includepatharrayref);
    # if ($language ne $firstlanguage) { change_language_in_pathes($localincludepatharrayref, $language, $firstlanguage); }
    # my $licenseref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$licensefilename, $localincludepatharrayref, 0);
    my $licenseref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$licensefilename, $includepatharrayref, 0);
    if ($$licenseref eq "") { installer::exiter::exit_program("ERROR: Could not find License file $licensefilename!", "get_licensefilesource"); }


    my $infoline = "Found licensefile $licensefilename: $$licenseref \n";
    push( @installer::globals::logfileinfo, $infoline);

    return $$licenseref;
}

#######################################################
# Converting the license string into the
# Java specific encoding.
#######################################################

sub convert_licenstring
{
    my ($licensefile, $includepatharrayref, $javadir, $onelanguage) = @_;

    my $licensedir = $javadir . $installer::globals::separator . "license";
    installer::systemactions::create_directory($licensedir);

    # saving the original license file

    my $licensefilename = $licensedir . $installer::globals::separator . "licensefile.txt";
    installer::files::save_file($licensefilename, $licensefile);

    # creating the ulf file from the license file

    $licensefilename = $licensedir . $installer::globals::separator . "licensefile.ulf";
    my @licensearray = ();

    my $section = "\[TRANSLATE\]\n";
    push(@licensearray, $section);

    for ( my $i = 0; $i <= $#{$licensefile}; $i++ )
    {
        my $oneline = ${$licensefile}[$i];

        if ($i == 0) { $oneline =~ s/^\s*\�\�\�//; }

        $oneline =~ s/\s*$//;
        $oneline =~ s/\"/\\\"/g;    # masquerading the "
        $oneline =~ s/\'/\\\'/g;    # masquerading the '

        $oneline =~ s/\$\{/\{/g;    # replacement of variables, only {PRODUCTNAME}, not ${PRODUCTNAME}

        my $ulfstring = $onelanguage . " = " . "\"" . $oneline . "\"\n";
        push(@licensearray, $ulfstring);
    }

    installer::files::save_file($licensefilename, \@licensearray);

    # converting the ulf file to the jlf file with ulfconv

    @licensearray = ();

    my $converter = "ulfconv";

    my $converterref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$converter, $includepatharrayref, 0);
    if ($$converterref eq "") { installer::exiter::exit_program("ERROR: Could not find converter $converter!", "convert_licenstring"); }

    my $infoline = "Found converter file $converter: $$converterref \n";
    push( @installer::globals::logfileinfo, $infoline);

    my $systemcall = "$$converterref $licensefilename |";
    open (CONV, "$systemcall");
    @licensearray = <CONV>;
    close (CONV);

    $licensefilename = $licensedir . $installer::globals::separator . "licensefile.jlf";
    installer::files::save_file($licensefilename, \@licensearray);

    # creating the license string from the jlf file

    $licensestring = "";

    for ( my $i = 1; $i <= $#licensearray; $i++ )   # not the first line!
    {
        my $oneline = $licensearray[$i];
        $oneline =~ s/^\s*$onelanguage\s*\=\s*\"//;
        $oneline =~ s/\"\s*$//;
        $licensestring = $licensestring . $oneline . "\\n";
    }

    $infoline = "Systemcall: $systemcall\n";
    push( @installer::globals::logfileinfo, $infoline);

    if ( $licensestring eq "" )
    {
        $infoline = "ERROR: Could not convert $licensefilename !\n";
        push( @installer::globals::logfileinfo, $infoline);
    }

    return $licensestring;
}

#######################################################
# Adding the license file into the java file
# In the template java file there are two
# occurences of INSTALLSDK_GUI_LICENSE
# and INSTALLSDK_CONSOLE_LICENSE
#######################################################

sub add_license_file_into_javafile
{
    my ( $templatefile, $licensefile, $includepatharrayref, $javadir, $onelanguage ) = @_;

    my $licensestring = convert_licenstring($licensefile, $includepatharrayref, $javadir, $onelanguage);

    # saving the licensestring in an ulf file
    # converting the file using "ulfconv license.ulf"
    # including the new string into the java file

    for ( my $i = 0; $i <= $#{$templatefile}; $i++ )
    {
        ${$templatefile}[$i] =~ s/INSTALLSDK_GUI_LICENSE/$licensestring/;
        ${$templatefile}[$i] =~ s/INSTALLSDK_CONSOLE_LICENSE/$licensestring/;
    }
}

#######################################################
# Executing one system call
#######################################################

sub make_systemcall
{
    my ( $systemcall, $logreturn ) = @_;

    my @returns = ();

    print "... $systemcall ...\n";

    open (REG, "$systemcall");
    while (<REG>) {push(@returns, $_); }
    close (REG);

    my $returnvalue = $?;   # $? contains the return value of the systemcall

    my $infoline = "Systemcall: $systemcall\n";
    push( @installer::globals::logfileinfo, $infoline);

    if ( $logreturn )
    {
        for ( my $j = 0; $j <= $#returns; $j++ ) { push( @installer::globals::logfileinfo, "$returns[$j]"); }
    }

    if ($returnvalue)
    {
        $infoline = "ERROR: $systemcall\n";
        push( @installer::globals::logfileinfo, $infoline);
        $error_occured = 1;
    }
    else
    {
        $infoline = "SUCCESS: $systemcall\n";
        push( @installer::globals::logfileinfo, $infoline);
    }

    return \@returns;
}

#######################################################
# Setting the class path for the Installer SDK
#######################################################

sub set_classpath_for_install_sdk
{
    my ( $directory ) = @_;

    my $installsdk = "";
    my $solarVersion = "";
    my $inPath = "";
    my $updMinorExt = "";

    if ( defined( $ENV{ 'SOLARVERSION' } ) ) { $solarVersion =  $ENV{'SOLARVERSION'}; }
    else { installer::exiter::exit_program("ERROR: Environment variable \"SOLARVERSION\" not set!", "set_classpath_for_install_sdk"); }

    if ( defined( $ENV{ 'INPATH' } ) ) { $inPath =  $ENV{'INPATH'}; }
    else { installer::exiter::exit_program("ERROR: Environment variable \"INPATH\" not set!", "set_classpath_for_install_sdk"); }

    if ( defined( $ENV{ 'UPDMINOREXT' } ) ) { $updMinorExt =  $ENV{'UPDMINOREXT'}; }
    else { installer::exiter::exit_program("ERROR: Environment variable \"UPDMINOREXT\" not set!", "set_classpath_for_install_sdk") if ( ! $ENV{UPDATER} ); }

    $installsdk = $solarVersion .  $installer::globals::separator . $inPath . $installer::globals::separator . "bin" . $updMinorExt;
    $installsdk = $installsdk . $installer::globals::separator . "javainstaller";

    if ( $ENV{'INSTALLSDK_SOURCE'} ) { $installsdk = $ENV{'INSTALLSDK_SOURCE'}; }   # overriding the Install SDK with INSTALLSDK_SOURCE

    # The variable CLASSPATH has to contain:
    # $installsdk/classes:$installsdk/classes/setupsdk.jar:
    # $installsdk/classes/parser.jar:$installsdk/classes/jaxp.jar:
    # $installsdk/classes/ldapjdk.jar:$directory

    my @additional_classpath = ();
    push(@additional_classpath, "$installsdk\/classes");
    push(@additional_classpath, "$installsdk\/installsdk.jar");
    push(@additional_classpath, "$installsdk\/classes\/parser.jar");
    push(@additional_classpath, "$installsdk\/classes\/jaxp.jar");
    push(@additional_classpath, "$directory");

    my $newclasspathstring = "";
    my $oldclasspathstring = "";
    if ( $ENV{'CLASSPATH'} ) { $oldclasspathstring = $ENV{'CLASSPATH'}; }
    else { $oldclasspathstring = "\."; }

    for ( my $i = 0; $i <= $#additional_classpath; $i++ )
    {
        $newclasspathstring = $newclasspathstring . $additional_classpath[$i] . ":";
    }

    $newclasspathstring = $newclasspathstring . $oldclasspathstring;

    $ENV{'CLASSPATH'} = $newclasspathstring;

    my $infoline = "Setting CLASSPATH to $ENV{'CLASSPATH'}\n";
    push( @installer::globals::logfileinfo, $infoline);
}

#######################################################
# Setting the class file name in the Java locale file
#######################################################

sub set_classfilename
{
    my ($templatefile, $classfilename, $searchstring) = @_;

    for ( my $j = 0; $j <= $#{$templatefile}; $j++ )
    {
        if ( ${$templatefile}[$j] =~ /\Q$searchstring\E/ )
        {
            ${$templatefile}[$j] =~ s/$searchstring/$classfilename/;
            last;
        }
    }
}

#######################################################
# Substituting one variable in the xml file
#######################################################

sub replace_one_variable
{
    my ($xmlfile, $variable, $searchstring) = @_;

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        ${$xmlfile}[$i] =~ s/\$\{$searchstring\}/$variable/g;
    }
}

#######################################################
# Substituting the variables in the xml file
#######################################################

sub substitute_variables
{
    my ($xmlfile, $variableshashref) = @_;

    my $key;

    foreach $key (keys %{$variableshashref})
    {
        my $value = $variableshashref->{$key};
        replace_one_variable($xmlfile, $value, $key);
    }
}

##########################################################
# Finding the line number in xml file of a special
# component
##########################################################

sub find_component_line
{
    my ($xmlfile, $componentname) = @_;

    my $linenumber = 0;

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        if ( ${$xmlfile}[$i] =~ /name\s*\=\'\s*$componentname/ )
        {
            $linenumber = $i;
            last;
        }
    }

    return $linenumber;
}

##########################################################
# Removing one package from the xml file
##########################################################

sub remove_package
{
    my ($xmlfile, $packagename) = @_;

    my $searchstring = $packagename;
    if ( $searchstring =~ /\-(\S+?)\s*$/ ) { $searchstring = $1; } # "SUNW%PRODUCTNAME-mailcap" -> "mailcap"

    my $packagestring = "";
    my $namestring = "";

    if ( $installer::globals::issolarispkgbuild )
    {
        $packagestring = "\<pkgunit";
        $namestring = "pkgName";
    }
    elsif ( $installer::globals::islinuxrpmbuild )
    {
        $packagestring = "\<rpmunit";
        $namestring = "rpmUniqueName";
    }

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        if ( ${$xmlfile}[$i] =~ /^\s*\Q$packagestring\E/ )
        {
            # this is a package, but is it the correct one?

            my $do_delete = 0;
            my $linecounter = 1;
            my $startline = $i+1;
            my $line = ${$xmlfile}[$startline];
            if (($line =~ /^\s*\Q$namestring\E\s*\=/) && ($line =~ /\-\Q$searchstring\E/)) { $do_delete = 1; }

            my $endcounter = 0;

            while ((!( $line =~ /\/\>/ )) && ( $startline <= $#{$xmlfile} ))
            {
                $linecounter++;
                $startline++;
                $line = ${$xmlfile}[$startline];
                if (($line =~ /^\s*\Q$namestring\E\s*\=/) && ($line =~ /\-\Q$searchstring\E/)) { $do_delete = 1; }
            }

            $linecounter = $linecounter + 1;

            if ( $do_delete )
            {
                splice(@{$xmlfile},$i, $linecounter);   # removing $linecounter lines, beginning in line $i
                last;
            }
        }
    }
}

##########################################################
# Removing one component from the xml file
##########################################################

sub remove_component
{
    my ($xmlfile, $componentname) = @_;

    my @removed_lines = ();

    push(@removed_lines, "\n");

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        if ( ${$xmlfile}[$i] =~ /name\s*\=\'\s*$componentname/ )
        {
            # Counting the lines till the second "</component>"

            push(@removed_lines, ${$xmlfile}[$i]);
            my $linecounter = 1;
            my $startline = $i+1;
            my $line = ${$xmlfile}[$startline];
            push(@removed_lines, $line);
            my $endcounter = 0;

            while ((!( $line =~ /^\s*\<\/component\>\s*$/ )) && ( $startline <= $#{$xmlfile} ))
            {
                $linecounter++;
                $startline++;
                $line = ${$xmlfile}[$startline];
                push(@removed_lines, $line);
            }

            $linecounter = $linecounter + 2;     # last line and following empty line

            splice(@{$xmlfile},$i, $linecounter);   # removing $linecounter lines, beginning in line $i
            last;
        }
    }

    return \@removed_lines;
}

##########################################################
# If this is an installation set without language packs
# the language pack module can be removed
##########################################################

sub remove_languagepack_from_xmlfile
{
    my ($xmlfile) = @_;

    # Component begins with "<component selected="true" name='module_languagepacks' componentVersion="${PRODUCTVERSION}">"
    # and ends with "</component>" (the second "</component>" !)

    remove_component($xmlfile, "languagepack_DEFAULT");
    remove_component($xmlfile, "languagepack_ONELANGUAGE");
    remove_component($xmlfile, "module_languagepacks");
}

##########################################################
# Duplicating a component
##########################################################

sub duplicate_component
{
    my ( $arrayref ) = @_;

    @newarray = ();

    for ( my $i = 0; $i <= $#{$arrayref}; $i++ )
    {
        push(@newarray, ${$arrayref}[$i]);
    }

    return \@newarray;
}

##########################################################
# Including a component into the xml file
# at a specified line
##########################################################

sub include_component_at_specific_line
{
    my ($xmlfile, $unit, $line) = @_;

    splice(@{$xmlfile},$line, 0, @{$unit});
}

##########################################################
# If this is an installation set with language packs,
# modules for each language pack have to be created
# dynamically
##########################################################

sub duplicate_languagepack_in_xmlfile
{
    my ($xmlfile) = @_;

    my $unit = remove_component($xmlfile, "languagepack_ONELANGUAGE");
    my $startline = find_component_line($xmlfile, "module_languagepacks");
    $startline = $startline + 1;

    for ( my $i = 1; $i <= $#installer::globals::languageproducts; $i++ )   # starting at "1", because "0" (first language) is no languagepack!
    {
        my $onelanguage = $installer::globals::languageproducts[$i];
        my $unitcopy = duplicate_component($unit);

        # replacing string ONELANGUAGE in the unit copy
        for ( my $j = 0; $j <= $#{$unitcopy}; $j++ ) { ${$unitcopy}[$j] =~ s/ONELANGUAGE/$onelanguage/g; }

        # including the unitcopy into the xml file
        include_component_at_specific_line($xmlfile, $unitcopy, $startline);
        $startline = $startline + $#{$unitcopy} + 1;
    }

    # adding the default language as language pack, too
    $unit = remove_component($xmlfile, "languagepack_DEFAULT");
    $startline = find_component_line($xmlfile, "module_languagepacks");
    $startline = $startline + 1;

    $onelanguage = $installer::globals::languageproducts[0];
    $unitcopy = duplicate_component($unit);

    # replacing string DEFAULT in the unit copy
    for ( my $j = 0; $j <= $#{$unitcopy}; $j++ ) { ${$unitcopy}[$j] =~ s/DEFAULT/$onelanguage/g; }

    # including the unitcopy into the xml file
    include_component_at_specific_line($xmlfile, $unitcopy, $startline);
    $startline = $startline + $#{$unitcopy} + 1;
}

#######################################################
# Removing empty packages from xml file. The names
# are stored in @installer::globals::emptypackages
#######################################################

sub remove_empty_packages_in_xmlfile
{
    my ($xmlfile) = @_;

    for ( my $i = 0; $i <= $#installer::globals::emptypackages; $i++ )
    {
        my $packagename = $installer::globals::emptypackages[$i];
        remove_package($xmlfile, $packagename);
        my $infoline = "Removing package $packagename from xml file.\n";
        push( @installer::globals::logfileinfo, $infoline);
    }
}

#######################################################
# Preparing the language packs in the xml file
#######################################################

sub prepare_language_pack_in_xmlfile
{
    my ($xmlfile) = @_;

    if ( ! $installer::globals::is_unix_multi )
    {
        remove_languagepack_from_xmlfile($xmlfile);
    }
    else
    {
        duplicate_languagepack_in_xmlfile($xmlfile);
    }

}

###########################################################
# Removing Ada product from xml file for Solaris x86
###########################################################

sub remove_ada_from_xmlfile
{
    my ($xmlfile) = @_;

    # Component begins with "<component selected='true' name='gid_Module_Optional_Adabas' componentVersion="12">"
    # and ends with "</component>"

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        if ( ${$xmlfile}[$i] =~ /name\s*\=\'\s*gid_Module_Optional_Adabas/ )
        {
            # Counting the lines till "</component>"

            my $linecounter = 1;
            my $startline = $i+1;
            my $line = ${$xmlfile}[$startline];

            while ((!( $line =~ /^\s*\<\/component\>\s*$/ )) && ( $startline <= $#{$xmlfile} ))
            {
                $linecounter++;
                $startline++;
                $line = ${$xmlfile}[$startline];
            }

            $linecounter = $linecounter + 2;     # last line and following empty line

            splice(@{$xmlfile},$i, $linecounter);   # removing $linecounter lines, beginning in line $i
            last;
        }
    }
}

#######################################################################
# Removing w4w filter module from xml file for Solaris x86 and Linux
#######################################################################

sub remove_w4w_from_xmlfile
{
    my ($xmlfile) = @_;

    # Component begins with "<component selected='true' name='gid_Module_Prg_Wrt_Flt_W4w' componentVersion="8">"
    # and ends with "</component>"

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        if ( ${$xmlfile}[$i] =~ /name\s*\=\'\s*gid_Module_Prg_Wrt_Flt_W4w/ )
        {
            # Counting the lines till "</component>"

            my $linecounter = 1;
            my $startline = $i+1;
            my $line = ${$xmlfile}[$startline];

            while ((!( $line =~ /^\s*\<\/component\>\s*$/ )) && ( $startline <= $#{$xmlfile} ))
            {
                $linecounter++;
                $startline++;
                $line = ${$xmlfile}[$startline];
            }

            $linecounter = $linecounter + 2;     # last line and following empty line

            splice(@{$xmlfile},$i, $linecounter);   # removing $linecounter lines, beginning in line $i
            last;
        }
    }
}

###########################################################
# Adding the lowercase variables into the variableshashref
###########################################################

sub add_lowercasevariables_to_allvariableshashref
{
    my ($variableshashref) = @_;

    my $lcvariable = "";

    $lcvariable = lc($variableshashref->{'PRODUCTNAME'});
    $variableshashref->{'LCPRODUCTNAME'} = $lcvariable;

    if ($variableshashref->{'SHORT_PRODUCTEXTENSION'})
    {
        $lcvariable = "\-" . lc($variableshashref->{'SHORT_PRODUCTEXTENSION'}); # including the "-" !
        $variableshashref->{'LCPRODUCTEXTENSION'} = $lcvariable;
    }
    else
    {
        $variableshashref->{'LCPRODUCTEXTENSION'} = "";
    }
}

###########################################################
# Preparing the package subdirectory
###########################################################

sub create_empty_packages
{
    my ( $xmlfile ) = @_;

    if ( $installer::globals::issolarispkgbuild )
    {
        my $path = "";

        for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
        {
            if ( ${$xmlfile}[$i] =~ /pkgRelativePath\s*\=\s*\'(.*?)\'\s*$/ )
            {
                $path = $1;
                installer::systemactions::create_directory_structure($path);
                last;   # only creating one path
            }
        }

        for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
        {
            if ( ${$xmlfile}[$i] =~ /pkgName\s*\=\s*\'(.*?)\'\s*$/ )
            {
                my $pkgname = $1;
                if ( $path ne "" ) { $pkgname = $path . $installer::globals::separator . $pkgname; }
                installer::systemactions::create_directory_structure($pkgname);
            }
        }
    }

    # "-novalidate" does not work for Linux RPMs

    if ( $installer::globals::islinuxrpmbuild )
    {
        for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
        {
            if ( ${$xmlfile}[$i] =~ /rpmPath\s*\=\s*\"(.*?)\"\s*$/ )
            {
                my $rpmpath = $1;
                my $path = "";

                if ( $rpmpath =~ /^\s*(.*)\/(.*?)\s*$/ )
                {
                    $path = $1;
                }

                if ( $path ne "" ) { installer::systemactions::create_directory_structure($path); }

                my $systemcall = "touch $rpmpath";  # creating empty rpm
                system($systemcall);
            }
        }
    }
}

###########################################################
# Reading the archive file name from the xml file
###########################################################

sub get_archivefilename
{
    my ( $xmlfile ) = @_;

    my $archivefilename = "";

    for ( my $j = 0; $j <= $#{$xmlfile}; $j++ )
    {
        if ( ${$xmlfile}[$j] =~ /archiveFileName\s*=\s*\'(.*?)\'/ )
        {
            $archivefilename = $1;
            last;
        }
    }

    return $archivefilename;
}

#######################################################
# Copying the loader locally
#######################################################

sub copy_setup_locally
{
    my ($includepatharrayref, $loadername, $newname) = @_;

    my $loadernameref = installer::scriptitems::get_sourcepath_from_filename_and_includepath(\$loadername, $includepatharrayref, 0);

    if ($$loadernameref eq "") { installer::exiter::exit_program("ERROR: Could not find Java loader $loadername!", "copy_setup_locally"); }

    installer::systemactions::copy_one_file($$loadernameref, $newname);
    my $localcall = "chmod 775 $newname \>\/dev\/null 2\>\&1";
    system($localcall);
}


#######################################################
# Copying the loader into the installation set
#######################################################

sub put_loader_into_installset
{
    my ($installdir, $filename) = @_;

    my $installname = $installdir . $installer::globals::separator . $filename;

    installer::systemactions::copy_one_file($filename, $installname);

    my $localcall = "chmod 775 $installname \>\/dev\/null 2\>\&1";
    system($localcall);
}

#################################################################
# Setting for Solaris the package names in the Java translation
# file. The name is used for the
# This name is displayed tools like prodreg.
# Unfortunately this name in the component is also used
# in the translation template file for the module name
# and module description translations.
#################################################################

sub replace_component_name_in_java_file
{
    my ($alljavafiles, $oldname, $newname) = @_;

    # The new name must not contain white spaces

    $newname =~ s/ /\_/g;

    for ( my $i = 0; $i <= $#{$alljavafiles}; $i++ )
    {
        my $javafilename = ${$alljavafiles}[$i];
        my $javafile = installer::files::read_file($javafilename);

        my $oldstring = "ComponentDescription-" . $oldname;
        my $newstring = "ComponentDescription-" . $newname;

        for ( my $j = 0; $j <= $#{$javafile}; $j++ ) { ${$javafile}[$j] =~ s/\b$oldstring\b/$newstring/; }

        $oldstring = $oldname . "-install-DisplayName";
        $newstring = $newname . "-install-DisplayName";

        for ( my $j = 0; $j <= $#{$javafile}; $j++ ) { ${$javafile}[$j] =~ s/\b$oldstring\b/$newstring/; }

        $oldstring = $oldname . "-uninstall-DisplayName";
        $newstring = $newname . "-uninstall-DisplayName";

        for ( my $j = 0; $j <= $#{$javafile}; $j++ ) { ${$javafile}[$j] =~ s/\b$oldstring\b/$newstring/; }

        installer::files::save_file($javafilename, $javafile);
        $infoline = "Changes in Java file: $javafilename : $oldname \-\> $newname\n";
        push( @installer::globals::logfileinfo, $infoline);
    }
}

#################################################################
# Some module names are not defined in the scp project.
# The names for this modules are searched in the base Java
# translation file.
#################################################################

sub get_module_name_from_basejavafile
{
    my ($componentname, $javatemplateorigfile, $ulffile) = @_;

    my $searchname = $componentname . "-install-DisplayName";
    my $modulename = "";
    my $replacename = "";

    # line content: { "coremodule-install-DisplayName", "OOO_INSTALLSDK_117" },

    for ( my $i = 0; $i <= $#{$javatemplateorigfile}; $i++ )
    {
        if ( ${$javatemplateorigfile}[$i] =~ /\"\s*\Q$searchname\E\s*\"\s*\,\s*\"\s*(.*?)\s*\"\s*\}\s*\,\s*$/ )
        {
            $replacename = $1;
            last;
        }
    }

    if ( $replacename ne "" )
    {
        my $language_block = get_language_block_from_language_file($replacename, $ulffile);
        $modulename = get_language_string_from_language_block($language_block, "en-US", $replacename);
    }

    return $modulename;
}

#################################################################
# Setting for Solaris the package names in the xml file.
# This name is displayed tools like prodreg.
# Unfortunately this name in the component is also used
# in the translation template file for the module name
# and module description translations.
#################################################################

sub replace_component_names
{
    my ($xmlfile, $templatefilename, $modulesarrayref, $javatemplateorigfile, $ulffile) = @_;

    # path in which all java languages files are located

    my $javafilesdir = $templatefilename;
    installer::pathanalyzer::get_path_from_fullqualifiedname(\$javafilesdir);
    my $alljavafiles = installer::systemactions::find_file_with_file_extension("java", $javafilesdir);
    for ( my $i = 0; $i <= $#{$alljavafiles}; $i++ ) { ${$alljavafiles}[$i] = $javafilesdir . ${$alljavafiles}[$i]; }

    # analyzing the xml file

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        my $newstring = "";
        my $componentname = "";

        if ( ${$xmlfile}[$i] =~ /\bcomponent\b.*\bname\s*\=\'\s*(.*?)\s*\'/ )
        {
            $componentname = $1;

            # Getting module name from the scp files in $modulesarrayref

            my $onelanguage = "en-US";
            my $gid = $componentname;
            my $type = "Name";

            my $modulename = "";
            $modulename = get_module_name_description($modulesarrayref, $onelanguage, $gid, $type);

            if ( $modulename eq "" )
            {
                $infoline = "Info: Modulename for $gid not defined in modules collector. Looking in Java ulf file.\n";
                push( @installer::globals::logfileinfo, $infoline);
            }

            if ( $modulename eq "" ) # the modulename can also be set in the Java ulf file
            {
                $modulename = get_module_name_from_basejavafile($componentname, $javatemplateorigfile, $ulffile);
            }

            if ( $modulename ne "" )    # only do something, if the modulename was found
            {
                ${$xmlfile}[$i] =~ s/$componentname/$modulename/;

                $infoline = "Replacement in xml file (Solaris): $componentname \-\> $modulename\n";
                push( @installer::globals::logfileinfo, $infoline);

                # Replacement has to be done in all Java language files
                replace_component_name_in_java_file($alljavafiles, $componentname, $modulename);
            }

            if ( $modulename eq "" ) # the modulename can also be set in the Java ulf file
            {
                $infoline = "WARNING: No replacement in xml file for component: $componentname\n";
                push( @installer::globals::logfileinfo, $infoline);
            }
        }
    }
}

#############################################################################
# Collecting all packages or rpms located in the installation directory
#############################################################################

sub get_all_packages_in_installdir
{
    my ($installdir, $subdir) = @_;

    my $infoline = "";

    my @allrpms = ();   # not needed for Solaris at the moment
    my $allrpms = \@allrpms;

    $installdir =~ s/\Q$installer::globals::separator\E\s*$//;
    my $directory = $installdir . $installer::globals::separator . $subdir;
    $directory =~ s/\Q$installer::globals::separator\E\s*$//;

    if ( $installer::globals::islinuxrpmbuild )
    {
        $allrpms = installer::systemactions::find_file_with_file_extension("rpm", $directory);

        # collecting rpms with the complete path

        for ( my $i = 0; $i <= $#{$allrpms}; $i++ )
        {
            ${$allrpms}[$i] = $directory . $installer::globals::separator . ${$allrpms}[$i];
            $infoline = "Found RPM: ${$allrpms}[$i]\n";
            push( @installer::globals::logfileinfo, $infoline);
        }
    }

    return $allrpms;
}

#######################################################
# Adding the values of the array
#######################################################

sub do_sum
{
    my ( $allnumbers ) = @_;

    my $sum = 0;

    for ( my $i = 0; $i <= $#{$allnumbers}; $i++ )
    {
        $sum = $sum + ${$allnumbers}[$i];
    }

    return $sum;
}

#######################################################
# Setting the filesize for the RPMs in the xml file
#######################################################

sub set_filesize_in_xmlfile
{
    my ($filesize, $rpmname, $xmlfile) = @_;

    my $infoline = "";
    my $foundrpm = 0;
    my $filesizeset = 0;

    for ( my $i = 0; $i <= $#{$xmlfile}; $i++ )
    {
        my $line = ${$xmlfile}[$i];

        # searching for "rpmPath="RPMS/${UNIXPRODUCTNAME}-core01-${PACKAGEVERSION}-${PACKAGEREVISION}.i586.rpm""

        if (( $line =~ /rpmPath\s*=/ ) && ( $line =~ /\Q$rpmname\E\"\s*$/ ))
        {
            $foundrpm = 1;

            my $number = $i;
            $number++;

            while ( ! ( ${$xmlfile}[$number] =~ /\/\>\s*$/ ))
            {
                if ( ${$xmlfile}[$number] =~ /FILESIZEPLACEHOLDER/ )
                {
                    ${$xmlfile}[$number] =~ s/FILESIZEPLACEHOLDER/$filesize/;
                    $filesizeset = 1;
                    $infoline = "Setting filesize for $rpmname : $filesize\n";
                    push( @installer::globals::logfileinfo, $infoline);
                    last;
                }

                $number++;
            }

            last;
        }
    }

    if ( ! $foundrpm )
    {
        $infoline = "ERROR: Did not find $rpmname in xml file !\n";
        push( @installer::globals::logfileinfo, $infoline);
    }

    if ( ! $filesizeset )
    {
        $infoline = "ERROR: Did not set filesize for $rpmname in xml file !\n";
        push( @installer::globals::logfileinfo, $infoline);
    }
}

#######################################################
# Including the file size of the rpms into the
# xml file
#######################################################

sub put_filesize_into_xmlfile
{
    my ($xmlfile, $listofpackages) = @_;

    my $infoline = "";

    for ( my $i = 0; $i <= $#{$listofpackages}; $i++ )
    {
        my $completerpmname = ${$listofpackages}[$i];
        my $rpmname = $completerpmname;
        installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$rpmname);

        my $systemcall = "$installer::globals::rpmcommand -qp --queryformat \"\[\%\{FILESIZES\}\\n\]\" $completerpmname 2\>\&1 |";
        my $rpmout = make_systemcall($systemcall, 0);
        my $filesize = do_sum($rpmout);

        $infoline = "Filesize $rpmname : $filesize\n";
        push( @installer::globals::logfileinfo, $infoline);

        set_filesize_in_xmlfile($filesize, $rpmname, $xmlfile);
    }
}

#######################################################
# Creating the java installer class file dynamically
#######################################################

sub create_java_installer
{
    my ( $installdir, $newdir, $languagestringref, $languagesarrayref, $filesarrayref, $allvariableshashref, $includepatharrayref, $modulesarrayref ) = @_;

    installer::logger::include_header_into_logfile("Creating Java installer:");

    my $infoline = "";

    # collecting all packages or rpms located in the installation directory
    my $listofpackages = get_all_packages_in_installdir($installdir, $newdir);

    # creating the directory
    my $javadir = installer::systemactions::create_directories("javainstaller", $languagestringref);
    $javadir =~ s/\/\s*$//;
    push(@installer::globals::removedirs, $javadir);

    # copying the content from directory install_sdk into the java directory

    my $projectroot = "";
    if ( $ENV{'PRJ'} ) { $projectroot = $ENV{'PRJ'}; }
    else { installer::exiter::exit_program("ERROR: Environment variable PRJ not set", "create_java_installer"); }

    $projectroot =~ s/\/\s*$//;
    my $sourcedir = "$projectroot/inc_global/unix/install_sdk";
    installer::systemactions::copy_complete_directory_without_cvs($sourcedir, $javadir);

    # determining the java template file

    my $templatefilename = $javadir . $installer::globals::separator . "locale/resources/MyResources_template.java";

    # Saving the content of the template file. It is used in the xml files

    my $javatemplateorigfile = installer::files::read_file($templatefilename);

    # determining the ulf language file

    # my $ulffilename = "installsdk.ulf";
    my $ulffilename = "installsdk.jlf";
    $ulffilename = $installer::globals::javalanguagepath . $installer::globals::separator . $ulffilename;
    my $ulffile = installer::files::read_file($ulffilename);

    $infoline = "\nReading ulf file: $ulffilename\n";
    push( @installer::globals::logfileinfo, $infoline);

    $infoline = "Translating the Java template file\n";
    push( @installer::globals::logfileinfo, $infoline);

    # For Unix multi installation sets, $languagesarrayref contains only the first language
    # Therefore the complete @installer::globals::languageproducts has to be used

    my $buildlanguagesref = "";

    if ( $installer::globals::is_unix_multi ) { $buildlanguagesref = \@installer::globals::languageproducts; }
    else { $buildlanguagesref = $languagesarrayref; }

    for ( my $i = 0; $i <= $#{$buildlanguagesref}; $i++ )
    {
        my $onelanguage = ${$buildlanguagesref}[$i];
        my $firstlanguage = ${$buildlanguagesref}[0];

        # replacing all strings in the Java file with content of ulf files

        my $templatefile = installer::files::read_file($templatefilename);

        set_component_name_and_description($templatefile, $modulesarrayref, $onelanguage);
        translate_javafile($templatefile, $ulffile, $onelanguage);

        # adding the license file into the Java file

        my $licensefilesource = get_licensefilesource($onelanguage, $firstlanguage, $filesarrayref, $includepatharrayref);
        my $licensefile = installer::files::read_file($licensefilesource);
        add_license_file_into_javafile($templatefile, $licensefile, $includepatharrayref, $javadir, $onelanguage);

        # setting productname and productversion

        set_productname_and_productversion($templatefile, $allvariableshashref);

        # setting the class name in the java file ( "MyResources_TEMPLATE" -> "MyResources_en" )

        if ( $onelanguage =~ /^\s*(\w+)\-(\w+)\s*$/ ) { $onelanguage = $1; }
        # $onelanguage =~ s/en-US/en/;   # java file name and class name contain only "_en"
        my $classfilename = "MyResources_" . $onelanguage;
        set_classfilename($templatefile, $classfilename, "MyResources_TEMPLATE");

        # saving the new file

        my $newfilename = $templatefilename;
        $newfilename =~ s/_template\.java\s*$/_$onelanguage\.java/;

        installer::files::save_file($newfilename, $templatefile);

        $infoline = "Saving Java file: $newfilename\n";
        push( @installer::globals::logfileinfo, $infoline);
    }

    # renaming one language java file to "MyResources.java"

    my $baselanguage = installer::languages::get_default_language($buildlanguagesref);
    if ( $baselanguage =~ /^\s*(\w+)\-(\w+)\s*$/ ) { $baselanguage = $1; }   # java file name and class name contain only "_en"
    # $baselanguage =~ s/en-US/en/;  # java file name and class name contain only "_en"
    my $baselanguagefilename = $javadir . $installer::globals::separator . "locale/resources/MyResources_" . $baselanguage . "\.java";
    my $basedestfilename = $javadir . $installer::globals::separator . "locale/resources/MyResources.java";
    installer::systemactions::copy_one_file($baselanguagefilename, $basedestfilename);

    # setting the class file name also for the base class

    my $basetemplatefile = installer::files::read_file($basedestfilename);
    my $oldclassfilename = $baselanguagefilename;
    installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$oldclassfilename);
    $oldclassfilename =~ s/\.java//;
    my $newclassfilename = $basedestfilename;
    installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$newclassfilename);
    $newclassfilename =~ s/\.java//;

    set_classfilename($basetemplatefile, $newclassfilename, $oldclassfilename);

    installer::files::save_file($basedestfilename, $basetemplatefile);

    $infoline = "Created base Java file: $basedestfilename\n";
    push( @installer::globals::logfileinfo, $infoline);

    # deleting the template file

    unlink($templatefilename);

    $infoline = "Deleted template Java resource file: $templatefilename\n";
    push( @installer::globals::logfileinfo, $infoline);

    # changing into Java directory

    my $from = cwd();

    chdir($javadir);

    $infoline = "Changing into directory: $javadir\n";
    push( @installer::globals::logfileinfo, $infoline);

    # preparing the xml file

    my $xmlfilename = "";
    my $subdir = "";

    if ( $installer::globals::issolarispkgbuild )
    {
        $xmlfilename = "pkgUnit.xml";
    }
    elsif ( $installer::globals::islinuxrpmbuild )
    {
        $xmlfilename = "rpmUnit.xml";
    }
    else
    {
        installer::exiter::exit_program("ERROR: No platform for Install SDK", "create_java_installer");
    }

    # reading, editing and saving the xmlfile

    my $xmlfile = installer::files::read_file($xmlfilename);
    remove_empty_packages_in_xmlfile($xmlfile);
    prepare_language_pack_in_xmlfile($xmlfile);
    add_lowercasevariables_to_allvariableshashref($allvariableshashref);
    substitute_variables($xmlfile, $allvariableshashref);
    if (( $installer::globals::issolarisx86build ) || ( ! $allvariableshashref->{'ADAPRODUCT'} )) { remove_ada_from_xmlfile($xmlfile); }
    if ( $installer::globals::issolarisx86build || $installer::globals::islinuxbuild ) { remove_w4w_from_xmlfile($xmlfile); }
    replace_component_names($xmlfile, $templatefilename, $modulesarrayref, $javatemplateorigfile, $ulffile);
    if ( $installer::globals::islinuxrpmbuild ) { put_filesize_into_xmlfile($xmlfile, $listofpackages); }
    installer::files::save_file($xmlfilename, $xmlfile);
    $infoline = "Saving xml file: $xmlfilename\n";
    push( @installer::globals::logfileinfo, $infoline);

    # Setting the classpath and starting compiler

    set_classpath_for_install_sdk($javadir);

    # creating class files:
    # language class file, dialog class files, installer class file

    my $jdkpath = "";
    if ( $ENV{'JDKPATH'} ) { $jdkpath = $ENV{'JDKPATH'}; }

    my $javac = "javac";
    if ( $jdkpath ) { $javac = $jdkpath . $installer::globals::separator . $javac; }

    my $systemcall = "$javac locale\/resources\/\*\.java 2\>\&1 |";
    make_systemcall($systemcall, 1);

    $systemcall = "$javac com\/sun\/staroffice\/install\/\*\.java 2\>\&1 |";
    make_systemcall($systemcall, 1);

    # making subdirectory creating empty packages
    create_empty_packages($xmlfile);

    # Copy "jresetup" from solver locally to include it into the classfile
    # Copy "jresetup" from solver to installdir

    my $setupname = "jresetup";
    my $newname = "setup";
    copy_setup_locally($includepatharrayref, $setupname, $newname);

    my $java = "java";
    if ( $jdkpath ) { $java = $jdkpath . $installer::globals::separator . $java; }

    $systemcall = "$java com.sun.setup.builder.InstallBuilder $xmlfilename -novalidate 2\>\&1 |";
    make_systemcall($systemcall, 1);

    # copying the newly created classfile into the installation set

    my $archivefilename = get_archivefilename($xmlfile);
    $archivefilename = $archivefilename . ".class";

    if ( ! -f $archivefilename ) { installer::exiter::exit_program("ERROR: Could not find Java class file $archivefilename!", "create_java_installer"); }

    installer::systemactions::copy_one_file($archivefilename, $installdir);

    # Adding the loader into the installation set. The name of the loader is setup.
    put_loader_into_installset($installdir, $newname);

    chdir($from);

    $infoline = "Changing into directory: $from\n";
    push( @installer::globals::logfileinfo, $infoline);
}

1;
