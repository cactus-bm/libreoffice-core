#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: registry.pm,v $
#
#   $Revision: 1.6 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:20:54 $
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

package installer::windows::registry;

use installer::files;
use installer::globals;
use installer::worker;
use installer::windows::idtglobal;

#####################################################
# Generating the component name from a registryitem
#####################################################

sub get_registry_component_name
{
    my ($registryref) = @_;

    # In this function exists the rule to create components from registryitems
    # Rule:
    # The componentname can be directly taken from the ModuleID.
    # All registryitems belonging to one module can get the same component.

    my $componentname = "";

    if ( $registryref->{'ModuleID'} ) { $componentname = $registryref->{'ModuleID'}; }

    $componentname =~ s/\\/\_/g;
    $componentname =~ s/\//\_/g;
    $componentname =~ s/\-/\_/g;
    $componentname =~ s/\_\s*$//g;

    $componentname = lc($componentname);    # componentnames always lowercase

    # Attention: Maximum length for the componentname is 72

    $componentname =~ s/gid_module_/g_m_/g;
    $componentname =~ s/_optional_/_o_/g;
    $componentname =~ s/_javafilter_/_jf_/g;

    $componentname = $componentname . "_registry";  # identifying this component as registryitem component

    return $componentname;
}

##############################################################
# Returning identifier for registry table.
##############################################################

sub get_registry_identifier
{
    my ($registry) = @_;

    my $identifier = "";

    if ( $registry->{'gid'} ) { $identifier = $registry->{'gid'}; }

    $identifier = lc($identifier);  # always lower case

    # Attention: Maximum length is 72

    $identifier =~ s/gid_regitem_/g_r_/;
    $identifier =~ s/_soffice_/_s_/;
    $identifier =~ s/_clsid_/_c_/;
    $identifier =~ s/_currentversion_/_cv_/;
    $identifier =~ s/_microsoft_/_ms_/;
    $identifier =~ s/_staroffice_/_so_/;
    $identifier =~ s/_classpath_/_cp_/;
    $identifier =~ s/__/_/g;

    # Saving this in the registry collector

    $registry->{'uniquename'} = $identifier;

    return $identifier;
}

##################################################################
# Returning root value for registry table.
##################################################################

sub get_registry_root
{
    my ($registry) = @_;

    my $rootvalue = 0;  # Default: Parent is KKEY_CLASSES_ROOT
    my $scproot = "";

    if ( $registry->{'ParentID'} ) { $scproot = $registry->{'ParentID'}; }

    if ( $scproot eq "PREDEFINED_HKEY_CLASSES_ROOT" ) { $rootvalue = 0; }

    if ( $scproot eq "PREDEFINED_HKEY_LOCAL_MACHINE" ) { $rootvalue = -1; }

    return $rootvalue;
}

##############################################################
# Returning key for registry table.
##############################################################

sub get_registry_key
{
    my ($registry, $allvariableshashref) = @_;

    my $key = "";

    if ( $registry->{'Subkey'} ) { $key = $registry->{'Subkey'}; }

    if ( $key =~ /\%/ ) { $key = installer::worker::replace_variables_in_string($key, $allvariableshashref); }

    return $key;
}

##############################################################
# Returning name for registry table.
##############################################################

sub get_registry_name
{
    my ($registry) = @_;

    my $name = "";

    if ( $registry->{'Name'} ) { $name = $registry->{'Name'}; }

    return $name;
}

##############################################################
# Returning value for registry table.
##############################################################

sub get_registry_value
{
    my ($registry) = @_;

    my $value = "";

    if ( $registry->{'Value'} ) { $value = $registry->{'Value'}; }

    $value =~ s/\\\"/\"/g;  # no more masquerading of '"'
    $value =~ s/\<progpath\>/\[INSTALLLOCATION\]/;
    $value =~ s/\[INSTALLLOCATION\]\\/\[INSTALLLOCATION\]/; # removing "\" after "[INSTALLLOCATION]"

    return $value;
}

##############################################################
# Returning component for registry table.
##############################################################

sub get_registry_component
{
    my ($registry) = @_;

    # All registry items belonging to one module can
    # be included into one component

    my $componentname = get_registry_component_name($registry);

    # saving componentname in the registryitem collector

    $registry->{'componentname'} = $componentname;

    return $componentname;
}

######################################################
# Creating the file Registry.idt dynamically
# Content:
# Registry Root Key Name Value Component_
######################################################

sub create_registry_table
{
    my ($registryref, $allregistrycomponentsref, $basedir, $languagesarrayref, $allvariableshashref) = @_;

    for ( my $m = 0; $m <= $#{$languagesarrayref}; $m++ )
    {
        my $onelanguage = ${$languagesarrayref}[$m];

        my @registrytable = ();

        installer::windows::idtglobal::write_idt_header(\@registrytable, "registry");

        for ( my $i = 0; $i <= $#{$registryref}; $i++ )
        {
            my $oneregistry = ${$registryref}[$i];

            # Controlling the language!
            # Only language independent folderitems or folderitems with the correct language
            # will be included into the table

            if (! (!(( $oneregistry->{'ismultilingual'} )) || ( $oneregistry->{'specificlanguage'} eq $onelanguage )) )  { next; }

            my %registry = ();

            $registry{'Registry'} = get_registry_identifier($oneregistry);
            $registry{'Root'} = get_registry_root($oneregistry);
            $registry{'Key'} = get_registry_key($oneregistry, $allvariableshashref);
            $registry{'Name'} = get_registry_name($oneregistry);
            $registry{'Value'} = get_registry_value($oneregistry);
            $registry{'Component_'} = get_registry_component($oneregistry);

            # Collecting all components
            if (!(installer::existence::exists_in_array($registry{'Component_'}, $allregistrycomponentsref)))
            {
                push(@{$allregistrycomponentsref}, $registry{'Component_'});
            }

            my $oneline = $registry{'Registry'} . "\t" . $registry{'Root'} . "\t" . $registry{'Key'} . "\t"
                        . $registry{'Name'} . "\t" . $registry{'Value'} . "\t" . $registry{'Component_'} . "\n";

            push(@registrytable, $oneline);
        }

        # Saving the file

        my $registrytablename = $basedir . $installer::globals::separator . "Registry.idt" . "." . $onelanguage;
        installer::files::save_file($registrytablename ,\@registrytable);
        my $infoline = "Created idt file: $registrytablename\n";
        push(@installer::globals::logfileinfo, $infoline);
    }
}

1;