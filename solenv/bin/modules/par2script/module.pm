#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: module.pm,v $
#
#   $Revision: 1.5 $
#
#   last change: $Author: rt $ $Date: 2005-09-08 09:26:47 $
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


package par2script::module;

use par2script::converter;
use par2script::work;

###########################################
# Removing undefined gids automatically
# from modules
###########################################

sub remove_from_modules
{
    my ($script) = @_;

    # if these 4 gids are not defined, they are automatically removed from the module

    my $allfilegids = par2script::work::get_all_gids_from_script($script, "File");
    my $allproceduregids = par2script::work::get_all_gids_from_script($script, "Procedure");
    my $allcustomgids = par2script::work::get_all_gids_from_script($script, "Custom");
    my $alldirectorygids = par2script::work::get_all_gids_from_script($script, "Directory");

    for ( my $i = 0; $i <= $#{$script}; $i++ )
    {
        my $oneline = ${$script}[$i];

        my $modulename;

        if ( $oneline =~ /^\s*Module\s+(\w+)\s*$/ )
        {
            $modulegid = $1;
            next;
        }

        if ( $oneline =~ /^\s*(\w+?)\s*\=\s*\((.*?)\)\s*\;\s*$/ )
        {
            my $key = $1;
            my $allassigneditemsstring = $2;
            my $defineditemgids;

            if ( $key eq "Files" ) { $defineditemgids = $allfilegids; }
            elsif ( $key eq "Dirs" ) { $defineditemgids = $alldirectorygids; }
            elsif ( $key eq "Procedures" ) { $defineditemgids = $allproceduregids; }
            elsif ( $key eq "Customs" ) { $defineditemgids = $allcustomgids; }
            else { next; }  # for instance "Styles"

            my $allassigneditems = par2script::converter::convert_stringlist_into_array(\$allassigneditemsstring, ",");

            for ( my $j = 0; $j <= $#{$allassigneditems}; $j++ )
            {
                my $oneassigneditem = ${$allassigneditems}[$j];

                # is there a definition for this assigned item?

                my $itemisdefined = 0;

                for ( my $k = 0; $k <= $#{$defineditemgids}; $k++ )
                {
                    if ( $oneassigneditem eq ${$defineditemgids}[$k] )
                    {
                        $itemisdefined = 1;
                        last;
                    }
                }

                if (! $itemisdefined)
                {
                    my $infoline = "WARNING: Removing $oneassigneditem from Module $modulegid\n";
                    # print $infoline;
                    push(@par2script::globals::logfileinfo, $infoline);

                    ${$script}[$i] =~ s/\b$oneassigneditem\b//;
                    ${$script}[$i] =~ s/\,\s*\,/\,/;
                    ${$script}[$i] =~ s/\(\s*\,\s*/\(/;
                    ${$script}[$i] =~ s/\s*\,\s*\)/\)/;

                    if (( ${$script}[$i] =~ /\(\s*\,\s*\)/ ) || ( ${$script}[$i] =~ /\(\s*\)/ ))
                    {
                        # this line can be removed
                        splice(@{$script}, $i, 1);
                        $i--;       # decreasing the counter!
                    }
                }
            }
        }
    }
}

###########################################
# Adding the collectors
# to the root module
###########################################

sub create_rootmodule
{
    my ($rootmodule, $allitemgids, $itemname) = @_;

    if ( $#{$allitemgids} > -1 )
    {
        my $oneline = "\t$itemname \= \(";

        for ( my $i = 0; $i <= $#{$allitemgids}; $i++ )
        {
            my $onegid = ${$allitemgids}[$i];

            my $infoline = "WARNING: Adding $onegid to root module\n";
            # print $infoline;

            if ($oneline eq "") { $oneline = "\t\t\t\t"; }

            $oneline .= $onegid;

            if ( $i == $#{$allitemgids} ) { $oneline .= "\)\;"; }
            else { $oneline .= "\,"; }

            if ( length($oneline) > 100 )
            {
                $oneline .= "\n";
                push(@{$rootmodule}, $oneline);
                $oneline = "";
            }

        }

        if (! $oneline =~ /^\s*$/ )
        {
            $oneline .= "\n";
            push(@{$rootmodule}, $oneline);
        }
    }
}

######################################################
# Splitting one long line into several short lines
######################################################

sub make_multiliner
{
    my ($itemname, $allgidstring) = @_;

    my @newblock = ();

    my $allitemgids = par2script::converter::convert_stringlist_into_array(\$allgidstring, ",");

    if ( $#{$allitemgids} > -1 )
    {
        my $oneline = "\t$itemname \= \(";

        for ( my $i = 0; $i <= $#{$allitemgids}; $i++ )
        {
            my $onegid = ${$allitemgids}[$i];

            if ($oneline eq "") { $oneline = "\t\t\t\t"; }

            $oneline .= $onegid;

            if ( $i == $#{$allitemgids} ) { $oneline .= "\)\;"; }
            else { $oneline .= "\,"; }

            if ( length($oneline) > 100 )
            {
                $oneline .= "\n";
                push(@newblock, $oneline);
                $oneline = "";
            }
        }

        if (! $oneline =~ /^\s*$/ )
        {
            $oneline .= "\n";
            push(@newblock, $oneline);
        }
    }

    return \@newblock;
}

###################################################
# Shorten the lines that belong to modules, if
# the length of the line is greater 100
###################################################

sub shorten_lines_at_modules
{
    my ($script) = @_;

    my $ismoduleblock = 0;

    for ( my $i = 0; $i <= $#{$script}; $i++ )
    {
        my $oneline = ${$script}[$i];

        if ( $oneline =~ /^\s*Module\s+\w+\s*$/ ) { $ismoduleblock = 1; }
        if (( $oneline =~ /^\s*End\s*$/ ) && ( $ismoduleblock )) { $ismoduleblock = 0; }

        if ( $ismoduleblock )
        {
            if (( $oneline =~ /^\s*(\w+)\s*\=\s*\((.*)\)\s*\;\s*$/ ) && ( length($oneline) > 100 ))
            {
                # this line has to be splitted in several lines
                my $item = $1;
                my $allgidstring = $2;

                my $multilines = make_multiliner($item, $allgidstring);
                splice(@{$script}, $i, 1, @{$multilines});
            }
        }
    }
}

###########################################
# Adding defined gids automatically
# to the root module
###########################################

sub add_to_root_module
{
    my ($script) = @_;

    my $rootmodulestartline = "";

    # if these 4 gids are defined and not added to another module, they are automatically added to the root module

    my $allfilegids = par2script::work::get_all_gids_from_script($script, "File");
    my $allproceduregids = par2script::work::get_all_gids_from_script($script, "Procedure");
    my $allcustomgids = par2script::work::get_all_gids_from_script($script, "Custom");
    my $alldirectorygids = par2script::work::get_all_gids_from_script($script, "Directory");

    for ( my $i = 0; $i <= $#{$script}; $i++ )
    {
        my $oneline = ${$script}[$i];

        if (( $oneline =~ /^\s*Module\s+\w+\s*$/ ) && ( $rootmodulestartline eq "" ))   # the first module is the root module
        {
            $rootmodulestartline = $i;
        }

        if ( $oneline =~ /^\s*(\w+?)\s*\=\s*\((.*?)\)\s*\;\s*$/ )
        {
            my $key = $1;
            my $allassigneditemsstring = $2;
            my $defineditemgids;

            if ( $key eq "Files" ) { $defineditemgids = $allfilegids; }
            elsif ( $key eq "Dirs" ) { $defineditemgids = $alldirectorygids; }
            elsif ( $key eq "Procedures" ) { $defineditemgids = $allproceduregids; }
            elsif ( $key eq "Customs" ) { $defineditemgids = $allcustomgids; }
            else { next; }  # for instance "Styles"

            my $allassigneditems = par2script::converter::convert_stringlist_into_array(\$allassigneditemsstring, ",");

            for ( my $j = 0; $j <= $#{$allassigneditems}; $j++ )
            {
                my $oneassigneditem = ${$allassigneditems}[$j];

                # this can be removed for the list for the root module

                for ( my $k = 0; $k <= $#{$defineditemgids}; $k++ )
                {
                    if ( $oneassigneditem eq ${$defineditemgids}[$k] )
                    {
                        splice(@{$defineditemgids}, $k, 1);
                        last;
                    }
                }
            }
        }
    }

    # Now the four collectors contain only gids, that have to be added to the root module
    # The module begins at $rootmodulestartline

    for ( my $i = $rootmodulestartline; $i <= $#{$script}; $i++ )
    {
        my $oneline = ${$script}[$i];

        if ( $oneline =~ /^\s*End\s*$/ ) { last; }

        if ( $oneline =~ /^\s*(\w+)\s+\=\s+\((.*)\)\s*\;\s*$/ )
        {
            my $key = $1;
            my $allassigneditemsstring = $2;
            my $defineditemgids;

            if ( $key eq "Files" ) { $defineditemgids = $allfilegids; }
            elsif ( $key eq "Dirs" ) { $defineditemgids = $alldirectorygids; }
            elsif ( $key eq "Procedures" ) { $defineditemgids = $allproceduregids; }
            elsif ( $key eq "Customs" ) { $defineditemgids = $allcustomgids; }
            else { next; }  # for instance "Styles"

            my $allassigneditems = par2script::converter::convert_stringlist_into_array(\$allassigneditemsstring, "\,");

            # adding the root module items to the collectors

            for ( my $j = 0; $j <= $#{$allassigneditems}; $j++ )
            {
                push(@{$defineditemgids}, ${$allassigneditems}[$j]);
            }

            # then the existing lines can be removed

            splice(@{$script}, $i, 1);
            $i--;       # decreasing the counter!
        }
    }

    # creation of the new block

    my @rootmodule = ();
    my $rootmoduleref = \@rootmodule;

    create_rootmodule($rootmoduleref, $alldirectorygids, "Dirs");
    create_rootmodule($rootmoduleref, $allfilegids, "Files");
    create_rootmodule($rootmoduleref, $allproceduregids, "Procedures");
    create_rootmodule($rootmoduleref, $allcustomgids, "Customs");

    # and finally the new blocks can be inserted into the root module

    my $insertline;

    for ( my $i = $rootmodulestartline; $i <= $#{$script}; $i++ )
    {
        if ( ${$script}[$i] =~ /^\s*End\s*$/i )
        {
            $insertline = $i;
            last;
        }
    }

    splice(@{$script}, $insertline, 0, @{$rootmoduleref});

}

1;
