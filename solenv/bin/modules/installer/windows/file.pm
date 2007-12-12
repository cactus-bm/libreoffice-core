#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: file.pm,v $
#
#   $Revision: 1.17 $
#
#   last change: $Author: kz $ $Date: 2007-12-12 14:54:52 $
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

package installer::windows::file;

use Digest::MD5;
use installer::existence;
use installer::exiter;
use installer::files;
use installer::globals;
use installer::logger;
use installer::pathanalyzer;
use installer::worker;
use installer::windows::idtglobal;
use installer::windows::language;

##########################################################################
# Assigning one cabinet file to each file. This is requrired,
# if cabinet files shall be equivalent to packages.
##########################################################################

sub assign_cab_to_files
{
    my ( $filesref ) = @_;

    my $infoline = "";

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        if ( ! exists(${$filesref}[$i]->{'modules'}) ) { installer::exiter::exit_program("ERROR: No module assignment found for ${$filesref}[$i]->{'gid'} !", "assign_cab_to_files"); }
        my $module = ${$filesref}[$i]->{'modules'};
        # If modules contains a list of modules, only taking the first one.
        if ( $module =~ /^\s*(.*?)\,/ ) { $module = $1; }

        if ( ! exists($installer::globals::allcabinetassigns{$module}) ) { installer::exiter::exit_program("ERROR: No cabinet file assigned to module \"$module\" !", "assign_cab_to_files"); }
        ${$filesref}[$i]->{'assignedcabinetfile'} = $installer::globals::allcabinetassigns{$module};

        # Counting the files in each cabinet file
        if ( ! exists($installer::globals::cabfilecounter{${$filesref}[$i]->{'assignedcabinetfile'}}) )
        {
            $installer::globals::cabfilecounter{${$filesref}[$i]->{'assignedcabinetfile'}} = 1;
        }
        else
        {
            $installer::globals::cabfilecounter{${$filesref}[$i]->{'assignedcabinetfile'}}++;
        }
    }

    # logging the number of files in each cabinet file

    $infoline = "\nCabinet file content:\n";
    push(@installer::globals::logfileinfo, $infoline);
    my $cabfile;
    foreach $cabfile ( sort keys %installer::globals::cabfilecounter )
    {
        $infoline = "$cabfile : $installer::globals::cabfilecounter{$cabfile} files\n";
        push(@installer::globals::logfileinfo, $infoline);
    }

    # assigning startsequencenumbers for each cab file

    my $offset = 1;
    foreach $cabfile ( sort keys %installer::globals::cabfilecounter )
    {
        my $filecount = $installer::globals::cabfilecounter{$cabfile};
        $installer::globals::cabfilecounter{$cabfile} = $offset;
        $offset = $offset + $filecount;

        $installer::globals::lastsequence{$cabfile} = $offset - 1;
    }

    # logging the start sequence numbers

    $infoline = "\nCabinet file start sequences:\n";
    push(@installer::globals::logfileinfo, $infoline);
    foreach $cabfile ( sort keys %installer::globals::cabfilecounter )
    {
        $infoline = "$cabfile : $installer::globals::cabfilecounter{$cabfile}\n";
        push(@installer::globals::logfileinfo, $infoline);
    }

    # logging the last sequence numbers

    $infoline = "\nCabinet file last sequences:\n";
    push(@installer::globals::logfileinfo, $infoline);
    foreach $cabfile ( sort keys %installer::globals::lastsequence )
    {
        $infoline = "$cabfile : $installer::globals::lastsequence{$cabfile}\n";
        push(@installer::globals::logfileinfo, $infoline);
    }
}

##########################################################################
# Assigning sequencenumbers to files. This is requrired,
# if cabinet files shall be equivalent to packages.
##########################################################################

sub assign_sequencenumbers_to_files
{
    my ( $filesref ) = @_;

    my %directaccess = ();
    my %allassigns = ();

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        my $onefile = ${$filesref}[$i];

        # Keeping order in cabinet files
        # -> collecting all files in one cabinet file
        # -> sorting files and assigning numbers

        # Saving counter $i for direct access into files array
        # "destination" of the file is a unique identifier ('Name' is not unique!)
        if ( exists($directaccess{$onefile->{'destination'}}) ) { installer::exiter::exit_program("ERROR: 'destination' at file not unique: $onefile->{'destination'}", "assign_sequencenumbers_to_files"); }
        $directaccess{$onefile->{'destination'}} = $i;

        my $cabfilename = $onefile->{'assignedcabinetfile'};
        # collecting files in cabinet files
        if ( ! exists($allassigns{$cabfilename}) )
        {
            my %onecabfile = ();
            $onecabfile{$onefile->{'destination'}} = 1;
            $allassigns{$cabfilename} = \%onecabfile;
        }
        else
        {
            $allassigns{$cabfilename}->{$onefile->{'destination'}} = 1;
        }
    }

    # Sorting each hash and assigning numbers
    # The destination of the file determines the sort order, not the filename!
    my $cabfile;
    foreach $cabfile ( sort keys %allassigns )
    {
        my $counter = $installer::globals::cabfilecounter{$cabfile};
        my $dest;
        foreach $dest ( sort keys %{$allassigns{$cabfile}} ) # <- sorting the destination!
        {
            my $directaccessnumber = $directaccess{$dest};
            ${$filesref}[$directaccessnumber]->{'assignedsequencenumber'} = $counter;
            $counter++;
        }
    }
}

###############################################
# Generating the component name from a file
###############################################

sub get_file_component_name
{
    my ($fileref, $filesref) = @_;

    # In this function exists the rule to create components from files
    # Rule:
    # Two files get the same componentid, if:
    # both have the same destination directory.
    # both have the same "gid" -> both were packed in the same zip file
    # All other files are included into different components!

    # my $componentname = $fileref->{'gid'} . "_" . $fileref->{'Dir'};

    # $fileref->{'Dir'} is not sufficient! All files in a zip file have the same $fileref->{'Dir'},
    # but can be in different subdirectories.
    # Solution: destination=share\Scripts\beanshell\Capitalise\capitalise.bsh
    # in which the filename (capitalise.bsh) has to be removed and all backslashes (slashes) are
    # converted into underline.

    my $destination = $fileref->{'destination'};
    installer::pathanalyzer::get_path_from_fullqualifiedname(\$destination);
    $destination =~ s/\\/\_/g;
    $destination =~ s/\//\_/g;
    $destination =~ s/\_\s*$//g;    # removing ending underline

    my $componentname = $fileref->{'gid'} . "__" . $destination;

    # Files with different languages, need to be packed into different components.
    # Then the installation of the language specific component is determined by a language condition.

    if ( $fileref->{'ismultilingual'} )
    {
        my $officelanguage = $fileref->{'specificlanguage'};
        $componentname = $componentname . "_" . $officelanguage;
    }

    $componentname = lc($componentname);    # componentnames always lowercase

    $componentname =~ s/\-/\_/g;            # converting "-" to "_"
    $componentname =~ s/\./\_/g;            # converting "-" to "_"

    # Attention: Maximum length for the componentname is 72

    $componentname =~ s/gid_file_/g_f_/g;
    $componentname =~ s/_extra_/_e_/g;
    $componentname =~ s/_config_/_c_/g;
    $componentname =~ s/_org_openoffice_/_o_o_/g;
    $componentname =~ s/_program_/_p_/g;
    $componentname =~ s/_typedetection_/_td_/g;
    $componentname =~ s/_linguistic_/_l_/g;
    $componentname =~ s/_module_/_m_/g;
    $componentname =~ s/_optional_/_opt_/g;
    $componentname =~ s/_packages/_pack/g;
    $componentname =~ s/_menubar/_mb/g;
    $componentname =~ s/_common_/_cm_/g;
    $componentname =~ s/_export_/_exp_/g;
    $componentname =~ s/_table_/_tb_/g;
    $componentname =~ s/_sofficecfg_/_sc_/g;
    $componentname =~ s/_startmodulecommands_/_smc_/g;
    $componentname =~ s/_drawimpresscommands_/_dic_/g;
    $componentname =~ s/_basiccommands_/_bac_/g;
    $componentname =~ s/_basicidecommands_/_baic_/g;
    $componentname =~ s/_genericcommands_/_genc_/g;
    $componentname =~ s/_bibliographycommands_/_bibc_/g;

    # All this is not necessary for files, which have the flag ASSIGNCOMPOMENT

    my $styles = "";
    if ( $fileref->{'Styles'} ) { $styles = $fileref->{'Styles'}; }
    if ( $styles =~ /\bASSIGNCOMPOMENT\b/ ) { $componentname = get_component_from_assigned_file($fileref->{'AssignComponent'}, $filesref); }

    return $componentname;
}

####################################################################
# Returning the component name for a defined file gid.
# This is necessary for files with flag ASSIGNCOMPOMENT
####################################################################

sub get_component_from_assigned_file
{
    my ($gid, $filesref) = @_;

    my $onefile = installer::existence::get_specified_file($filesref, $gid);
    my $componentname = "";
    if ( $onefile->{'componentname'} ) { $componentname = $onefile->{'componentname'}; }
    else { installer::exiter::exit_program("ERROR: No component defined for file: $gid", "get_component_from_assigned_file"); }

    return $componentname;
}

####################################################################
# Generating the special filename for the database file File.idt
# Sample: CONTEXTS, CONTEXTS1
# This name has to be unique.
# In most cases this is simply the filename.
####################################################################

sub generate_unique_filename_for_filetable
{
    my ($fileref, $component) = @_;

    # This new filename has to be saved into $fileref, because this is needed to find the source.
    # The filename sbasic.idx/OFFSETS is changed to OFFSETS, but OFFSETS is not unique.
    # In this procedure names like OFFSETS5 are produced. And exactly this string has to be added to
    # the array of all files.

    my $uniquefilename = "";
    my $counter = 0;

    if ( $fileref->{'Name'} ) { $uniquefilename = $fileref->{'Name'}; }

    installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$uniquefilename); # making /registry/schema/org/openoffice/VCL.xcs to VCL.xcs

    if (( $installer::globals::prepare_winpatch ) && ( exists($installer::globals::savedmapping{"$component/$uniquefilename"}) ))
    {
        # If we have a FTK mapping for this component/file, use it.
        $installer::globals::savedmapping{"$component/$uniquefilename"} =~ m/^(.*);/;
        $uniquefilename = $1;
         $lcuniquefilename = lc($uniquefilename);
        $installer::globals::alluniquefilenames{$uniquefilename} = 1;
        $installer::globals::alllcuniquefilenames{$lcuniquefilename} = 1;
        return $uniquefilename;
    }

    $uniquefilename =~ s/\-/\_/g;       # no "-" allowed
    $uniquefilename =~ s/\@/\_/g;       # no "@" allowed
    $uniquefilename =~ s/\$/\_/g;       # no "$" allowed
    $uniquefilename =~ s/^\s*\./\_/g;       # no "." at the beginning allowed allowed
    $uniquefilename =~ s/^\s*\d/\_d/g;      # no number at the beginning allowed allowed (even file "0.gif", replacing to "_d.gif")
    $uniquefilename =~ s/org_openoffice_/ooo_/g;    # shorten the unique file name

    my $lcuniquefilename = lc($uniquefilename); # only lowercase names

    my $newname = 0;

    if ( ! exists($installer::globals::alllcuniquefilenames{$lcuniquefilename}) &&
         ! exists($installer::globals::savedrevmapping{$lcuniquefilename}) )
    {
        $installer::globals::alluniquefilenames{$uniquefilename} = 1;
        $installer::globals::alllcuniquefilenames{$lcuniquefilename} = 1;
        $newname = 1;
    }

    if ( ! $newname )
    {
        # adding a number until the name is really unique: OFFSETS, OFFSETS1, OFFSETS2, ...
        # But attention: Making "abc.xcu" to "abc1.xcu"

        my $uniquefilenamebase = $uniquefilename;

        do
        {
            $counter++;

            if ( $uniquefilenamebase =~ /\./ )
            {
                $uniquefilename = $uniquefilenamebase;
                $uniquefilename =~ s/\./$counter\./;
            }
            else
            {
                $uniquefilename = $uniquefilenamebase . $counter;
            }

            $newname = 0;
            $lcuniquefilename = lc($uniquefilename);    # only lowercase names

            if ( ! exists($installer::globals::alllcuniquefilenames{$lcuniquefilename}) &&
                 ! exists($installer::globals::savedrevmapping{$lcuniquefilename}) )
            {
                $installer::globals::alluniquefilenames{$uniquefilename} = 1;
                $installer::globals::alllcuniquefilenames{$lcuniquefilename} = 1;
                $newname = 1;
            }
        }
        until ( $newname )
    }

    return $uniquefilename;
}

####################################################################
# Generating the special file column for the database file File.idt
# Sample: NAMETR~1.TAB|.nametranslation.table
# The first part has to be 8.3 conform.
####################################################################

sub generate_filename_for_filetable
{
    my ($fileref, $shortnamesref) = @_;

    my $returnstring = "";

    my $filename = $fileref->{'Name'};

    installer::pathanalyzer::make_absolute_filename_to_relative_filename(\$filename);   # making /registry/schema/org/openoffice/VCL.xcs to VCL.xcs

    my $shortstring;
    if (( $installer::globals::prepare_winpatch ) && ( exists($installer::globals::savedmapping{"$fileref->{'componentname'}/$filename"}) ))
    {
        $installer::globals::savedmapping{"$fileref->{'componentname'}/$filename"} =~ m/.*;(.*)/;
        if ($1 ne '')
        {
            $shortstring = $1;
        }
        else
        {
            $shortstring = installer::windows::idtglobal::make_eight_three_conform_with_hash($filename, "file", $shortnamesref);
        }
    }
    else
    {
        $shortstring = installer::windows::idtglobal::make_eight_three_conform_with_hash($filename, "file", $shortnamesref);
    }

    if ( $shortstring eq $filename ) { $returnstring = $filename; } # nothing changed
    else {$returnstring = $shortstring . "\|" . $filename; }

    return $returnstring;
}

#########################################
# Returning the filesize of a file
#########################################

sub get_filesize
{
    my ($fileref) = @_;

    my $file = $fileref->{'sourcepath'};

    my $filesize;

    if ( -f $file ) # test of existence. For instance services.rdb does not always exist
    {
        $filesize = ( -s $file );   # file size can be "0"
    }
    else
    {
        $filesize = -1;
    }

    return $filesize;
}

#############################################
# Returning the file version, if required
# Sample: "8.0.1.8976";
#############################################

sub get_fileversion
{
    my ($fileref, $allvariables) = @_;

    my $fileversion = "";

    if ( $allvariables->{'USE_FILEVERSION'} )
    {
        if ( ! $allvariables->{'LIBRARYVERSION'} ) { installer::exiter::exit_program("ERROR: USE_FILEVERSION is set, but not LIBRARYVERSION", "get_fileversion"); }
        $fileversion = $allvariables->{'LIBRARYVERSION'} . "\." . $installer::globals::buildid;
    }

    if ( $installer::globals::prepare_winpatch ) { $fileversion = ""; } # Windows patches do not allow this version

    return $fileversion;
}

#############################################
# Returning the sequence for a file
#############################################

sub get_sequence_for_file
{
    my ($number, $onefile) = @_;

    my $sequence = "";

    if (( $onefile->{'assignedsequencenumber'} ) && ( $installer::globals::use_packages_for_cabs ))
    {
        $sequence = $onefile->{'assignedsequencenumber'};
    }
    else
    {
        $sequence = $number;
        # my $sequence = $number + 1;

        # Idea: Each component is packed into a cab file.
        # This requires that all files in one cab file have sequences directly follwing each other,
        # for instance from 1456 to 1466. Then in the media table the LastSequence for this cab file
        # is 1466.
        # Because all files belonging to one component are directly behind each other in the file
        # collector, it is possible to use simply an increasing number as sequence value.
        # If files belonging to one component are not directly behind each other in the files collector
        # this mechanism will no longer work.
    }

    return $sequence;
}

#############################################
# Returning the Windows language of a file
#############################################

sub get_language_for_file
{
    my ($fileref) = @_;

    my $language = "";

    if ( $fileref->{'specificlanguage'} ) { $language = $fileref->{'specificlanguage'}; }

    if (!($language eq ""))
    {
        $language = installer::windows::language::get_windows_language($language);
    }

    return $language;
}

####################################################################
# Creating a new KeyPath for components in TemplatesFolder.
####################################################################

sub generate_registry_keypath
{
    my ($onefile) = @_;

    my $keypath = $onefile->{'Name'};
    $keypath =~ s/\.//g;
    $keypath = lc($keypath);
    $keypath = "userreg_" . $keypath;

    return $keypath;
}

############################################
# Creating the file File.idt dynamically
############################################

sub create_files_table
{
    my ($filesref, $allfilecomponentsref, $basedir, $allvariables) = @_;

    installer::logger::include_timestamp_into_logfile("Performance Info: File Table start");

    # Structure of the files table:
    # File Component_ FileName FileSize Version Language Attributes Sequence
    # In this function, all components are created.
    #
    # $allfilecomponentsref is empty at the beginning

    my $infoline;

    my @allfiles = ();
    my @filetable = ();
    my @filehashtable = ();
    my %allfilecomponents = ();
    my $counter = 0;

    if ( $^O =~ /cygwin/i ) { installer::worker::generate_cygwin_pathes($filesref); }

    # The filenames must be collected because of uniqueness
    # 01-44-~1.DAT, 01-44-~2.DAT, ...
    # my @shortnames = ();
    my %shortnames = ();

    installer::windows::idtglobal::write_idt_header(\@filetable, "file");
    installer::windows::idtglobal::write_idt_header(\@filehashtable, "filehash");

    for ( my $i = 0; $i <= $#{$filesref}; $i++ )
    {
        my %file = ();

        my $onefile = ${$filesref}[$i];

        my $styles = "";
        if ( $onefile->{'Styles'} ) { $styles = $onefile->{'Styles'}; }
        if (( $styles =~ /\bJAVAFILE\b/ ) && ( ! ($allvariables->{'JAVAPRODUCT'} ))) { next; }
        if (( $styles =~ /\bADAFILE\b/ ) && ( ! ($allvariables->{'ADAPRODUCT'} ))) { next; }

        $file{'Component_'} = get_file_component_name($onefile, $filesref);
        $file{'File'} = generate_unique_filename_for_filetable($onefile, $file{'Component_'});

        $onefile->{'uniquename'} = $file{'File'};
        $onefile->{'componentname'} = $file{'Component_'};

        # Collecting all components
        # if (!(installer::existence::exists_in_array($file{'Component_'}, $allfilecomponentsref))) { push(@{$allfilecomponentsref}, $file{'Component_'}); }

        if ( ! exists($allfilecomponents{$file{'Component_'}}) ) { $allfilecomponents{$file{'Component_'}} = 1; }

        $file{'FileName'} = generate_filename_for_filetable($onefile, \%shortnames);

        $file{'FileSize'} = get_filesize($onefile);

        $file{'Version'} = get_fileversion($onefile, $allvariables);

        $file{'Language'} = get_language_for_file($onefile);

        if ( $styles =~ /\bDONT_PACK\b/ ) { $file{'Attributes'} = "8192"; }
        else { $file{'Attributes'} = "16384"; }

        # $file{'Attributes'} = "16384";    # Sourcefile is packed
        # $file{'Attributes'} = "8192";     # Sourcefile is unpacked

        $counter++;
        $file{'Sequence'} = get_sequence_for_file($counter, $onefile);

        $onefile->{'sequencenumber'} = $file{'Sequence'};

        my $oneline = $file{'File'} . "\t" . $file{'Component_'} . "\t" . $file{'FileName'} . "\t"
                . $file{'FileSize'} . "\t" . $file{'Version'} . "\t" . $file{'Language'} . "\t"
                . $file{'Attributes'} . "\t" . $file{'Sequence'} . "\n";

        push(@filetable, $oneline);

        push(@allfiles, $onefile);

        # Collecting all component conditions
        if ( $onefile->{'ComponentCondition'} )
        {
            if ( ! exists($installer::globals::componentcondition{$file{'Component_'}}))
            {
                $installer::globals::componentcondition{$file{'Component_'}} = $onefile->{'ComponentCondition'};
            }
        }

        if ( $installer::globals::prepare_winpatch )
        {
            my $path = $onefile->{'sourcepath'};
            if ( $^O =~ /cygwin/i ) { $path = $onefile->{'cyg_sourcepath'}; }

            open(FILE, $path) or die "ERROR: Can't open $path for creating file hash";
            binmode(FILE);
            my $hashinfo = pack("l", 20);
            $hashinfo .= Digest::MD5->new->addfile(*FILE)->digest;

            my @i = unpack ('x[l]l4', $hashinfo);
            $oneline = $file{'File'} . "\t" .
                "0" . "\t" .
                $i[0] . "\t" .
                $i[1] . "\t" .
                $i[2] . "\t" .
                $i[3] . "\n";
            push (@filehashtable, $oneline);
        }

        # Saving the sequence number in a hash with uniquefilename as key.
        # This is used for better performance in "save_packorder"
        $installer::globals::uniquefilenamesequence{$onefile->{'uniquename'}} = $onefile->{'sequencenumber'};

        # Special handling for files in PREDEFINED_OSSHELLNEWDIR. These components
        # need as KeyPath a RegistryItem in HKCU
        my $destdir = "";
        if ( $onefile->{'Dir'} ) { $destdir = $onefile->{'Dir'}; }

        if (( $destdir =~ /\bPREDEFINED_OSSHELLNEWDIR\b/ ) || ( $onefile->{'needs_user_registry_key'} ))
        {
            my $keypath = generate_registry_keypath($onefile);
            $onefile->{'userregkeypath'} = $keypath;
            push(@installer::globals::userregistrycollector, $onefile);
            $installer::globals::addeduserregitrykeys = 1;
        }
    }

    # putting content from %allfilecomponents to $allfilecomponentsref for later usage
    foreach $localkey (keys %allfilecomponents ) { push( @{$allfilecomponentsref}, $localkey); }

    my $filetablename = $basedir . $installer::globals::separator . "File.idt";
    installer::files::save_file($filetablename ,\@filetable);
    $infoline = "\nCreated idt file: $filetablename\n";
    push(@installer::globals::logfileinfo, $infoline);

    installer::logger::include_timestamp_into_logfile("Performance Info: File Table end");

    my $filehashtablename = $basedir . $installer::globals::separator . "MsiFileHash.idt";
    installer::files::save_file($filehashtablename ,\@filehashtable);
    $infoline = "\nCreated idt file: $filehashtablename\n";
    push(@installer::globals::logfileinfo, $infoline);

    return \@allfiles;
}

1;
