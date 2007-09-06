#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: feature.pm,v $
#
#   $Revision: 1.19 $
#
#   last change: $Author: kz $ $Date: 2007-09-06 09:54:50 $
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

package installer::windows::feature;

use installer::existence;
use installer::exiter;
use installer::files;
use installer::globals;
use installer::sorter;
use installer::worker;
use installer::windows::idtglobal;
use installer::windows::language;

##############################################################
# Returning the gid for a feature.
# Attention: Maximum length
##############################################################

sub get_feature_gid
{
    my ($onefeature) = @_;

    my $gid = "";

    if ( $onefeature->{'gid'} ) { $gid = $onefeature->{'gid'}; }

    # Attention: Maximum feature length is 38!
    installer::windows::idtglobal::shorten_feature_gid(\$gid);

    return $gid
}

##############################################################
# Returning the gid of the parent.
# Attention: Maximum length
##############################################################

sub get_feature_parent
{
    my ($onefeature) = @_;

    my $parentgid = "";

    if ( $onefeature->{'ParentID'} ) { $parentgid = $onefeature->{'ParentID'}; }

    # The modules, hanging directly below the root, have to be root modules.
    # Only then it is possible to make the "real" root module invisible by
    # setting the display to "0".

    if ( $parentgid eq $installer::globals::rootmodulegid ) { $parentgid = ""; }

    # Attention: Maximum feature length is 38!
    installer::windows::idtglobal::shorten_feature_gid(\$parentgid);

    return $parentgid
}

##############################################################
# Returning the display for a feature.
# 0: Feature is not shown
# odd: subfeatures are shown
# even:  subfeatures are not shown
##############################################################

sub get_feature_display
{
    my ($onefeature) = @_;

    my $display;
    my $parentid = "";

    if ( $onefeature->{'ParentID'} ) { $parentid = $onefeature->{'ParentID'}; }

    if ( $parentid eq "" )
    {
        $display = "0";                                 # root module is not visible
    }
    elsif ( $onefeature->{'gid'} eq "gid_Module_Prg")   # program module shows subfeatures
    {
        $display = "1";                                 # root module shows subfeatures
    }
    else
    {
        $display = "2";                                 # all other modules do not show subfeatures
    }

    # special case: Feature is below root module and has flag "HIDDEN_ROOT" -> $display is 0

    my $styles = "";
    if ( $onefeature->{'Styles'} ) { $styles = $onefeature->{'Styles'}; }

    if (( $parentid eq $installer::globals::rootmodulegid ) && ( $styles =~ /\bHIDDEN_ROOT\b/ ))
    {
        $display = "0";
    }

    # Special handling for c05office. No program module visible.
    if (( $onefeature->{'gid'} eq "gid_Module_Prg" ) && ( $installer::globals::product =~ /c05office/i )) { $display = "0"; }

    if ( $installer::globals::languagepack ) { $display = "0"; }     # making all feature invisible!

    return $display
}

##############################################################
# Returning the level for a feature.
##############################################################

sub get_feature_level
{
    my ($onefeature) = @_;

    my $level = "20";   # the default

    my $localdefault = "";

    if ( $onefeature->{'Default'} ) { $localdefault = $onefeature->{'Default'}; }

    if ( $localdefault eq "NO" )    # explicitely set Default = "NO"
    {
        $level = "200";             # deselected in default installation, base is 100
        if ( $installer::globals::patch ) { $level = "20"; }
    }

    # special handling for Java and Ada

    if ( $onefeature->{'Name'} =~ /java/i || $onefeature->{'Name'} =~ /adabas/i ) { $level = $level + 40; }

    # if FeatureLevel is defined in scp, this will be used

    if ( $onefeature->{'FeatureLevel'} ) { $level = $onefeature->{'FeatureLevel'}; }

    return $level
}

##############################################################
# Returning the directory for a feature.
##############################################################

sub get_feature_directory
{
    my ($onefeature) = @_;

    my $directory;

    $directory = "INSTALLLOCATION";

    return $directory
}

##############################################################
# Returning the directory for a feature.
##############################################################

sub get_feature_attributes
{
    my ($onefeature) = @_;

    my $attributes;

    # No advertising of features and no leaving on network.
    # Feature without parent must not have the "2"

    my $parentgid = "";
    if ( $onefeature->{'ParentID'} ) { $parentgid = $onefeature->{'ParentID'}; }

    if (( $parentgid eq "" ) || ( $parentgid eq $installer::globals::rootmodulegid )) { $attributes = "8"; }
    else { $attributes = "10"; }

    return $attributes
}

#####################################################################
# For multilingual installation sets, the language dependent files
# are advised to the language feature.
#####################################################################

sub change_modules_in_filescollector
{
    my ($filesref) = @_;

    push(@installer::globals::multilanguagemodules, $installer::globals::languagemodulesparent);

    my $onefile;

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        $onefile =  ${$filesref}[$i];

        # Searching for multilingual files

        if ( $onefile->{'ismultilingual'} )
        {
            my $officelanguage = $onefile->{'specificlanguage'};
            $officelanguage =~ s/-/_/; # "en-US" to "en_US" (the latter is used in scp)
            $feature = $installer::globals::languagemodulesbase . $officelanguage;   # "gid_Module_Root_en_US"
            $onefile->{'modules'} = $feature;   # assigning the new language feature !

            # Collecting all new feature
            if ( ! installer::existence::exists_in_array($feature, \@installer::globals::multilanguagemodules) )
            {
                push(@installer::globals::multilanguagemodules, $feature);
            }
        }
    }
}

##############################################################
# Creating the feature for the language packs.
##############################################################

sub add_language_pack_feature
{
    my ($featuretableref, $languagemodules, $onelanguage) = @_;

    for ( my $i = 0; $i <= $#installer::globals::languagepackfeature; $i++ )
    {
        my $gid = $installer::globals::languagepackfeature[$i];

        # $languagemodules contains all modules with flag LANGUAGEMODULE
        # Now it is time to find the correct feature using the gid.

        my $onefeature = installer::worker::find_item_by_gid($languagemodules, $gid);
        if ( $onefeature eq "" ) { installer::exiter::exit_program("ERROR: Language feature not found: $gid !", "add_language_pack_feature"); }

        my %feature = ();

        $feature{'feature'} = get_feature_gid($onefeature);
        $feature{'feature_parent'} = "";
        $feature{'Title'} = $onefeature->{'Name'};
        $feature{'Description'} = $onefeature->{'Description'};
        $feature{'Display'} = "1";
        $feature{'Level'} = get_feature_level($onefeature);
        $feature{'Directory_'} =  "INSTALLLOCATION";
        $feature{'Attributes'} =  "8";

        my $oneline = $feature{'feature'} . "\t" . $feature{'feature_parent'} . "\t" . $feature{'Title'} . "\t"
                    . $feature{'Description'} . "\t" . $feature{'Display'} . "\t" . $feature{'Level'} . "\t"
                    . $feature{'Directory_'} . "\t" . $feature{'Attributes'} . "\n";

        push(@{$featuretableref}, $oneline);

        # collecting all feature in global feature collector
        if ( ! installer::existence::exists_in_array($feature{'feature'}, \@installer::globals::featurecollector) )
        {
            push(@installer::globals::featurecollector, $feature{'feature'});
        }

        # $onelanguage = "de"
        # $gid = "gm_Langpack_de"
        # -> only include if this is the language specific language name
        my $comparelanguage = $onelanguage;
        $comparelanguage =~ s/-/_/;
        if ( $gid =~ /\_$comparelanguage\s*$/) { push(@installer::globals::languagenames, $feature{'Title'}); } # saving all titles in the global variable!
    }

}

##################################################################
# Creating the feature for the multilingual installation sets.
##################################################################

sub add_multilingual_features
{
    my ($featuretableref, $languagemodules, $onelanguage) = @_;

    # Adding the new dynamic language modules, collected in change_modules_in_filescollector

    for ( my $i = 0; $i <= $#installer::globals::multilanguagemodules; $i++ )
    {
        my $gid = $installer::globals::multilanguagemodules[$i];

        # $languagemodules contains all modules with flag LANGUAGEMODULE
        # Now it is time to find the correct feature using the gid.
        my $onefeature = installer::worker::find_item_by_gid($languagemodules, $gid);
        if ( $onefeature eq "" ) { installer::exiter::exit_program("ERROR: Language feature not found: $gid !", "add_multilingual_features"); }

        my $deselectable = 1;   # feature can be deselected
        if (( $i == 0 ) || ( $i == 1 )) { $deselectable = 0; } # Toplevel module and english cannot be deselected

        my %feature = ();

        $feature{'feature'} = get_feature_gid($onefeature);
        $feature{'feature_parent'} = get_feature_parent($onefeature);
        $feature{'Title'} = $onefeature->{'Name'};
        $feature{'Description'} = $onefeature->{'Description'};
        $feature{'Display'} = get_feature_display($onefeature);
        $feature{'Level'} = get_feature_level($onefeature);
        $feature{'Directory_'} = get_feature_directory($onefeature);
        $feature{'Attributes'} = get_feature_attributes($onefeature);

        if ( $deselectable == 0 ) { $feature{'Attributes'} = $feature{'Attributes'} + 16; } # making feature not deselectable

        $oneline = $feature{'feature'} . "\t" . $feature{'feature_parent'} . "\t" . $feature{'Title'} . "\t"
                    . $feature{'Description'} . "\t" . $feature{'Display'} . "\t" . $feature{'Level'} . "\t"
                    . $feature{'Directory_'} . "\t" . $feature{'Attributes'} . "\n";

        push(@{$featuretableref}, $oneline);

        # collecting all feature in global feature collector
        if ( ! installer::existence::exists_in_array($feature{'feature'}, \@installer::globals::featurecollector) )
        {
            push(@installer::globals::featurecollector, $feature{'feature'});
        }
    }
}

#################################################################################
# Replacing one variable in one files
#################################################################################

sub replace_one_variable
{
    my ($translationfile, $variable, $searchstring) = @_;

    for ( my $i = 0; $i <= $#{$translationfile}; $i++ )
    {
        ${$translationfile}[$i] =~ s/\%$searchstring/$variable/g;
    }
}

#################################################################################
# Replacing the variables in the feature names and descriptions
#################################################################################

sub replace_variables
{
    my ($translationfile, $variableshashref) = @_;

    foreach $key (keys %{$variableshashref})
    {
        my $value = $variableshashref->{$key};
        replace_one_variable($translationfile, $value, $key);
    }
}

#################################################################################
# Collecting the feature recursively.
#################################################################################

sub collect_modules_recursive
{
    my ($modulesref, $parentid, $feature, $directaccess, $directgid, $directparent, $directsortkey, $sorted) = @_;

    my @allchildren = ();
    my $childrenexist = 0;

    # Collecting children from Module $parentid

    my $modulegid;
    foreach $modulegid ( keys %{$directparent})
    {
        if ( $directparent->{$modulegid} eq $parentid )
        {
            my %childhash = ( "gid" => "$modulegid", "Sortkey" => "$directsortkey->{$modulegid}");
            push(@allchildren, \%childhash);
            $childrenexist = 1;
        }
    }

    # Sorting children

    if ( $childrenexist )
    {
        # Sort children
        installer::sorter::sort_array_of_hashes_numerically(\@allchildren, "Sortkey");

        # Adding children to new array
        my $childhashref;
        foreach $childhashref ( @allchildren )
        {
            my $gid = $childhashref->{'gid'};

            # Saving all lines, that have this 'gid'

            my $unique;
            foreach $unique ( keys %{$directgid} )
            {
                if ( $directgid->{$unique} eq $gid )
                {
                    push(@{$feature}, ${$modulesref}[$directaccess->{$unique}]);
                    if ( $sorted->{$unique} == 1 ) { installer::exiter::exit_program("ERROR: Sorting feature failed! \"$unique\" already sorted.", "sort_feature"); }
                    $sorted->{$unique} = 1;
                }
            }

            collect_modules_recursive($modulesref, $gid, $feature, $directaccess, $directgid, $directparent, $directsortkey, $sorted);
        }
    }
}

#################################################################################
# Sorting the feature in specified order. Evaluated is the key "Sortkey", that
# is set in scp2 projects.
# The display order of modules in Windows Installer is dependent from the order
# in the idt file. Therefore the order of the modules array has to be adapted
# to the Sortkey order, before the idt file is created.
#################################################################################

sub sort_feature
{
    my ($modulesref) = @_;

    my @feature = ();

    my %directaccess = ();
    my %directparent = ();
    my %directgid = ();
    my %directsortkey = ();
    my %sorted = ();

    for ( my $i = 0; $i <= $#{$modulesref}; $i++ )
    {
        my $onefeature = ${$modulesref}[$i];

        my $uniquekey = $onefeature->{'uniquekey'};
        my $modulegid = $onefeature->{'gid'};

        $directaccess{$uniquekey} = $i;

        $directgid{$uniquekey} = $onefeature->{'gid'};

        # ParentID and Sortkey are not saved for the 'uniquekey', but only for the 'gid'

        if ( $onefeature->{'ParentID'} ) { $directparent{$modulegid} = $onefeature->{'ParentID'}; }
        else { $directparent{$modulegid} = ""; }

        if ( $onefeature->{'Sortkey'} ) { $directsortkey{$modulegid} = $onefeature->{'Sortkey'}; }
        else { $directsortkey{$modulegid} = "9999"; }

        # Bookkeeping:
        $sorted{$uniquekey} = 0;
    }

    # Searching all feature recursively, beginning with ParentID = ""
    my $parentid = "";
    collect_modules_recursive($modulesref, $parentid, \@feature, \%directaccess, \%directgid, \%directparent, \%directsortkey, \%sorted);

    # Bookkeeping
    my $modulekey;
    foreach $modulekey ( keys %sorted )
    {
        if ( $sorted{$modulekey} == 0 )
        {
            my $infoline = "Warning: Module \"$modulekey\" could not be sorted. Added to the end of the module array.\n";
            push(@installer::globals::logfileinfo, $infoline);
            push(@feature, ${$modulesref}[$directaccess{$modulekey}]);
        }
    }

    return \@feature;
}

#################################################################################
# Adding a unique key to the modules array. The gid is not unique for
# multilingual modules. Only the combination from gid and specific language
# is unique. Uniqueness is required for sorting mechanism.
#################################################################################

sub add_uniquekey
{
    my ( $modulesref ) = @_;

    for ( my $i = 0; $i <= $#{$modulesref}; $i++ )
    {
        my $uniquekey = ${$modulesref}[$i]->{'gid'};
        if ( ${$modulesref}[$i]->{'specificlanguage'} ) { $uniquekey = $uniquekey . "_" . ${$modulesref}[$i]->{'specificlanguage'}; }
        ${$modulesref}[$i]->{'uniquekey'} = $uniquekey;
    }
}

#################################################################################
# Creating the file Feature.idt dynamically
# Content:
# Feature Feature_Parent Title Description Display Level Directory_ Attributes
#################################################################################

sub create_feature_table
{
    my ($modulesref, $basedir, $languagesarrayref, $allvariableshashref) = @_;

    for ( my $m = 0; $m <= $#{$languagesarrayref}; $m++ )
    {
        my $onelanguage = ${$languagesarrayref}[$m];

        my $infoline;

        my @featuretable = ();
        my @languagefeatures = ();

        installer::windows::idtglobal::write_idt_header(\@featuretable, "feature");

        for ( my $i = 0; $i <= $#{$modulesref}; $i++ )
        {
            my $onefeature = ${$modulesref}[$i];

            # Java and Ada only, if the correct settings are set
            my $styles = "";
            if ( $onefeature->{'Styles'} ) { $styles = $onefeature->{'Styles'}; }
            if (( $styles =~ /\bJAVAMODULE\b/ ) && ( ! ($allvariableshashref->{'JAVAPRODUCT'} ))) { next; }
            if (( $styles =~ /\bADAMODULE\b/ ) && ( ! ($allvariableshashref->{'ADAPRODUCT'} ))) { next; }

            # Controlling the language!
            # Only language independent feature or feature with the correct language will be included into the table

            if (! (!(( $onefeature->{'ismultilingual'} )) || ( $onefeature->{'specificlanguage'} eq $onelanguage )) )  { next; }

            my $languagefeature = 0;
            if ( $styles =~ /\bLANGUAGEMODULE\b/ )
            {
                push(@languagefeatures, $onefeature);
                $languagefeature = 1;
            }

            my %feature = ();

            $feature{'feature'} = get_feature_gid($onefeature);
            $feature{'feature_parent'} = get_feature_parent($onefeature);
            # if ( $onefeature->{'ParentID'} eq "" ) { $feature{'feature_parent'} = ""; }   # Root has no parent
            $feature{'Title'} = $onefeature->{'Name'};
            $feature{'Description'} = $onefeature->{'Description'};
            $feature{'Display'} = get_feature_display($onefeature);
            $feature{'Level'} = get_feature_level($onefeature);
            $feature{'Directory_'} = get_feature_directory($onefeature);
            $feature{'Attributes'} = get_feature_attributes($onefeature);

            my $oneline = $feature{'feature'} . "\t" . $feature{'feature_parent'} . "\t" . $feature{'Title'} . "\t"
                    . $feature{'Description'} . "\t" . $feature{'Display'} . "\t" . $feature{'Level'} . "\t"
                    . $feature{'Directory_'} . "\t" . $feature{'Attributes'} . "\n";

            if ( ! $languagefeature )
            {
                push(@featuretable, $oneline);
                # collecting all feature in global feature collector
                if ( ! installer::existence::exists_in_array($feature{'feature'}, \@installer::globals::featurecollector) )
                {
                    push(@installer::globals::featurecollector, $feature{'feature'});
                }
            }
        }

        if ( $installer::globals::languagepack ) { add_language_pack_feature(\@featuretable, \@languagefeatures, $onelanguage); }

        if (( $installer::globals::ismultilingual ) && ( ! $installer::globals::languagepack ) && ( ! $allvariableshashref->{'DONTUSELANGUAGEPACKFEATURE'} )) { add_multilingual_features(\@featuretable, \@languagefeatures, $onelanguage); }

        # Saving the file

        my $featuretablename = $basedir . $installer::globals::separator . "Feature.idt" . "." . $onelanguage;
        installer::files::save_file($featuretablename ,\@featuretable);
        $infoline = "Created idt file: $featuretablename\n";
        push(@installer::globals::logfileinfo, $infoline);
    }

}

1;