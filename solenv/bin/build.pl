    :
    eval 'exec perl -S $0 ${1+"$@"}'
        if 0;
#*************************************************************************
#
#   $RCSfile: build.pl,v $
#
#   $Revision: 1.111 $
#
#   last change: $Author: vg $ $Date: 2004-05-07 09:05:25 $
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
# build - build entire project
#

    use Config;
    use POSIX;
    use Cwd;
    use File::Path;

    if (defined $ENV{CWS_WORK_STAMP}) {
        require lib; import lib ("$ENV{SOLARENV}/bin/modules", "$ENV{COMMON_ENV_TOOLS}/modules");
        require Cws; import Cws;
        require Logging; import Logging ;
        require CvsModule; import CvsModule;
        require GenInfoParser; import GenInfoParser;
        require IO::Handle; import IO::Handle;
    };

#### script id #####

    ( $script_name = $0 ) =~ s/^.*\b(\w+)\.pl$/$1/;

    $id_str = ' $Revision: 1.111 $ ';
    $id_str =~ /Revision:\s+(\S+)\s+\$/
      ? ($script_rev = $1) : ($script_rev = "-");

    print "$script_name -- version: $script_rev\n";

#########################
#                       #
#   Globale Variablen   #
#                       #
#########################

    if (defined $ENV{CWS_WORK_STAMP}) {
        if (defined $ENV{VCSID}) {
            $vcsid = $ENV{VCSID};
        } else {
            print_error("Can't determine VCSID. Please use setsolar.", 5);
        };
        $log = Logging->new() if (defined $ENV{CWS_WORK_STAMP});
    };
    $modules_number++;
    $perl = "";
    $remove_commando = "";
    if ( $^O eq 'MSWin32' ) {
        $perl = "$ENV{COMSPEC} -c perl5";
        $remove_commando = "rmdir /S /Q";
        $nul = '> NULL';
    } else {
        use Cwd 'chdir';
        $perl = 'perl';
        $remove_commando = 'rm -rf';
        $nul = '> /dev/null';
    };

    $QuantityToBuild = 0;
# delete $pid when not needed
    %projects_deps_hash = ();   # hash of undependent projects,
                                # that could be built now
    %broken_build = ();         # hash of hashes of the modules,
                                # where build was broken (error occurred)
    %folders_hashes = ();
    %running_children = ();
    $dependencies_hash = 0;
    $cmd_file = '';
    $BuildAllParents = 0;
    $show = 0;
    $deliver = 0;
    %LocalDepsHash = ();
    %BuildQueue = ();
    %PathHash = ();
    %PlatformHash = ();
    %AliveDependencies = ();
    %global_deps_hash = (); # hash of dependencies of the all modules
    %broken_modules_hashes = ();   # hash of modules hashes, which cannot be built further
    @broken_modules_names = ();   # array of modules, which cannot be built further
    @UnresolvedParents = ();
    @dmake_args = ();
    %DeadParents = ();
    $CurrentPrj = '';
    $no_projects = 0;
    $only_dependent = 0;
    $build_from = '';
    $build_from_opt = '';
    $build_since = '';
    $dlv_switch = '';
    $child = 0;
    %processes_hash = ();
    %module_annonced = ();
    $prepare = ''; # prepare for following incompartible build
    $ignore = '';
    @ignored_errors = ();
    %incompartibles = ();
    %force_deliver = ();
    $only_platform = ''; # the only platform to prepare
    $only_common = ''; # the only common output tree to delete when preparing
    %build_modes = ();
    $maximal_processes = 0; # the max number of the processes run
    %modules_types = (); # modules types ('mod', 'img', 'lnk') hash
    %platforms = (); # platforms available or being working with
### main ###

    &get_options;
    &get_build_modes;
    %deliver_env = ();
    if ($prepare) {
        &get_platforms(\%platforms);
        @modules_built = ();

        $deliver_env{'BUILD_SOSL'}++;
        $deliver_env{'COMMON_OUTDIR'}++;
        $deliver_env{'DLLSUFFIX'}++;
        $deliver_env{'GUI'}++;
        $deliver_env{'INPATH'}++;
        $deliver_env{'OFFENV_PATH'}++;
        $deliver_env{'OUTPATH'}++;
        $deliver_env{'L10N_framework'}++;
    };

    $StandDir = &get_stand_dir();
    &provide_consistency if (defined $ENV{CWS_WORK_STAMP});

    $deliver_commando = $ENV{DELIVER};
    $deliver_commando .= ' '. $dlv_switch if ($dlv_switch);
    $ENV{mk_tmp}++;
    %prj_platform = ();
    $check_error_string = '';
    $dmake = '';
    $echo = '';
    $new_line = "\n";
#### main ####

    &get_commands();
    unlink ($cmd_file);
    if ($cmd_file) {
        if (open (CMD_FILE, ">>$cmd_file")) {
            select CMD_FILE;
            $echo = 'echo ';
            $new_line = $echo."\"\"\n";
            print "\@$echo off\npushd\n" if ($ENV{GUI} ne 'UNX');
        } else {
            &print_error ("Cannot open file $cmd_file");
        };
    } elsif ($show) {
        select STDERR;
    };

    print $new_line;

    &BuildAll();
    &cancel_build if (scalar keys %broken_build);
    @TotenEltern = keys %DeadParents;
    if ($#TotenEltern != -1) {
        my ($DeadPrj);
        print $new_line.$new_line;
        print $echo."WARNING! Project(s):\n";
        foreach $DeadPrj (@TotenEltern) {
            print $echo."$DeadPrj\n";
        };
        print $new_line;
        print $echo."not found and couldn't be built. Correct build.lsts.\n";
        print $new_line;
    };
    if (($ENV{GUI} ne 'UNX') && $cmd_file) {
        print "popd\n";
    };
    $ENV{mk_tmp} = '';
    if ($cmd_file) {
        close CMD_FILE;
        print STDOUT "Script $cmd_file generated\n";
    };
    if ($ignore && scalar @ignored_errors) {
        print STDERR "\nERROR: next directories could not be built:\n";
        foreach (@ignored_errors) {
            print STDERR "\t$_\n";
        };
        print STDERR "\nERROR: please check these directories and build the correspondent module(s) anew!!\n\n";
        do_exit(1);
    };
    &finish_logging;
    do_exit(0);


#########################
#                       #
#      Procedures       #
#                       #
#########################

#
# Get dependencies hash of the current and all parent projects
#
    sub GetParentDeps {
        my ($ParentsString, @DepsArray, $Prj, $parent, $prj_link);
        my $prj_dir = shift;
        my $deps_hash = shift;
        $ParentsString = &GetParentsString($prj_dir);
        @DepsArray = &GetDependenciesArray($ParentsString);
        @UnresolvedParents = @DepsArray;
        $$deps_hash{$prj_dir} = [@DepsArray];
        while ($Prj = pop(@UnresolvedParents)) {
            $prj_link = $Prj . '.lnk';
            if (!-d $StandDir.$Prj) {
                if (-e $StandDir.$prj_link) {
                    $Prj = $prj_link;
                } elsif (-l $StandDir.$prj_link) {
                    &print_error("There is no target for link $StandDir$prj_link");
                } elsif (defined $ENV{CWS_WORK_STAMP}) {
                    &checkout_module($Prj, 'image');
                };
            } elsif (defined $ENV{CWS_WORK_STAMP}) {
                &check_module_consistency($Prj);
            };
            my @DepsArray;
            if (!($ParentsString = &GetParentsString($StandDir.$Prj))) {
                $DeadParents{$Prj}++;
                $$deps_hash{$Prj} = [];
                next;
            };
            @DepsArray = &GetDependenciesArray($ParentsString, $Prj);
            $$deps_hash{$Prj} = [@DepsArray];
            foreach $Parent (@DepsArray) {
                if ((!defined($$deps_hash{$Parent})) && (!defined($$deps_hash{$Parent . '.lnk'}))) {
                    push (@UnresolvedParents, $Parent);
                };
            };
        };
        foreach $parent (keys %DeadParents) {
            delete $global_deps_hash{$parent};
            &RemoveFromDependencies($parent, $deps_hash);
        };
        &check_deps_hash($deps_hash);
    };

#
# Build everything that should be built
#
    sub BuildAll {
        if ($BuildAllParents) {
            my ($Prj, $PrjDir, $orig_prj);
            &GetParentDeps( $CurrentPrj, \%global_deps_hash);
            &prepare_build_from(\%global_deps_hash) if ($build_from);
            &prepare_incompartible_build(\%global_deps_hash) if ($incompartible);
            if ($build_from_opt || $build_since) {
                &prepare_build_from_opt(\%global_deps_hash);
            };
            $modules_number = scalar keys %global_deps_hash;
            if ($QuantityToBuild) {
                &build_multiprocessing;
                return;
            };
            while ($Prj = &PickPrjToBuild(\%global_deps_hash)) {
                print $new_line;
                my $module_type = &module_classify($Prj);

                &print_annonce($Prj) if ($module_type eq 'lnk');
            &print_annonce($Prj . '.incomp') if ($module_type eq 'img');
            if ($module_type eq 'mod') {
                if (scalar keys %broken_build) {
                    print $echo.    "Skipping project $Prj because of error(s)\n";
                    &RemoveFromDependencies($Prj, \%global_deps_hash);
                    next;
                };
                &print_annonce($Prj);
                $PrjDir = &CorrectPath($StandDir.$Prj);
                &mark_force_deliver($Prj, $PrjDir) if (defined $ENV{CWS_WORK_STAMP});
                 &get_deps_hash($Prj, \%LocalDepsHash);
                 &BuildDependent(\%LocalDepsHash);
                my $deliver_commando = &get_deliver_commando($Prj);
                 if ($cmd_file) {
                     print "$deliver_commando\n";
                 } else {
                     system ("$deliver_commando") if (!$show && ($Prj ne $CurrentPrj) && !$deliver);
                 };
                 print $check_error_string;
            };
            &RemoveFromDependencies($Prj, \%global_deps_hash);
            $no_projects = 0;
        };
    } else {
        &get_deps_hash($CurrentPrj, \%LocalDepsHash);
        &BuildDependent(\%LocalDepsHash);
    };
};

#
# Start build given project
#
sub dmake_dir {
    my ($folder_nick, $BuildDir, $new_BuildDir, $OldBuildDir, $error_code);
    $folder_nick = shift;
    $BuildDir = &CorrectPath($StandDir . $PathHash{$folder_nick});
    if ((!(-d $BuildDir)) && (defined $ENV{CWS_WORK_STAMP})) {
        $OldBuildDir = $BuildDir;
        my $modified_path = $PathHash{$folder_nick};
        $modified_path =~ s/^([^\\\/]+)/$1\.lnk/;
        $BuildDir = &CorrectPath($StandDir . $modified_path);
    };
    my $missing_dir;
    $missing_dir = $OldBuildDir if ($OldBuildDir);
    $missing_dir = $BuildDir if (!$missing_dir);
    &print_error("$missing_dir not found!!\n") if (!(-d $BuildDir));
    if (!(-d $BuildDir)) {
        $new_BuildDir = $BuildDir;
        $new_BuildDir =~ s/_simple//g;
        if ((-d $new_BuildDir)) {
            print("\nTrying $new_BuildDir, $BuildDir not found!!\n");
            $BuildDir = $new_BuildDir;
        } else {
            &print_error("\n$BuildDir not found!!\n");
        }
    }
    if ($cmd_file) {
        print "cd $BuildDir\n";
        print $check_error_string;
        print $echo.$BuildDir."\n";
        print "$dmake\n";
        print $check_error_string;
    } else {
        print "$BuildDir\n";
    };
    &RemoveFromDependencies($folder_nick, \%LocalDepsHash) if (!$child);
    if (!$cmd_file && !$show) {
        chdir $BuildDir;
        cwd();
        $error_code = system ("$dmake");
        if ($error_code && $ignore) {
            push(@ignored_errors, &CorrectPath($StandDir . $PathHash{$folder_nick}));
            $error_code = 0;
        };
    };
    if ($child) {
        my $oldfh = select STDERR;
        $| = 1;
        _exit($? >> 8) if ($? && ($? != -1));
        _exit(0);
    } elsif ($error_code && ($error_code != -1)) {
        &print_error("Error $? occurred while making $BuildDir");
    };
};

#
# Get string (list) of parent projects to build
#
sub GetParentsString {
    my ($PrjDir);
    $PrjDir = shift;
    $PrjDir = '.' if ($PrjDir eq $CurrentPrj);
    return '' if (!open (BUILD_LST, $PrjDir.'/prj/build.lst'));
    while (<BUILD_LST>) {
        if ($_ =~ /#/) {
            if ($`) {
                $_ = $`;
            } else {
                next;
            };
        };
        s/\r\n//;
        if ($_ =~ /\:+\s+/) {
            close BUILD_LST;
            return &pick_for_build_type($');
        };
    };
    close BUILD_LST;
    return 'NULL';
};

#
# get folders' platform infos
#
sub get_prj_platform {
    my ($prj_alias, $line);
    while(<BUILD_LST>) {
        s/\r\n//;
        $line++;
        if ($_ =~ /nmake/) {
            if ($' =~ /\s+-\s+(\w+)[,\S+]*\s+(\S+)/ ) {
                my $platform = $1;
                my $alias = $2;
                &print_error ("There is no correct alias set in the line $line!") if ($alias eq 'NULL');
                &mark_platform($alias, $platform);
            } else {
                &print_error("Misspelling in line: \n$_");
            };
        };
    };
    seek(BUILD_LST, 0, 0);
};

#
# Getting hashes of all internal dependencies and additional
# infos for given project
#
sub get_deps_hash {
    my ($dummy, $module_to_build, $module_path);
    %DeadDependencies = ();
    $module_to_build = shift;
    $module_path = &CorrectPath($StandDir.$module_to_build);
    my $dependencies_hash = shift;
    chdir $module_path;
    cwd();
    if ($deliver) {
        if ($cmd_file) {
            print "$deliver_commando\n";
        } else {
            system ("$deliver_commando") if (!$show);
        };
        return;
    };
    open (BUILD_LST, 'prj/build.lst');
    &get_prj_platform;
    while (<BUILD_LST>) {
        if ($_ =~ /#/o) {
            next if (!$`);
            $_ = $`;
        };
        s/\r\n//;
        if ($_ =~ /nmake/o) {
            my ($Platform, $Dependencies, $Dir, $DirAlias, @Array);
            $Dependencies = $';
            $dummy = $`;
            $dummy =~ /(\S+)\s+(\S+)/o;
            $Dir = $2;
            $Dependencies =~ /(\w+)/o;
            $Platform = $1;
            $Dependencies = $';
            while ($Dependencies =~ /,(\w+)/o) {
                $Dependencies = $';
            };
            $Dependencies =~ /\s+(\S+)\s+/o;
            $DirAlias = $1;
            if (!&CheckPlatform($Platform)) {
                $DeadDependencies{$DirAlias}++;
                next;
            };
            $PlatformHash{$DirAlias}++;
            $Dependencies = $';
            &print_error("$module_to_build/prj/build.lst has wrong written dependencies string:\n$_\n") if (!$Dependencies);
            @Array = &GetDependenciesArray($Dependencies);
            $$dependencies_hash{$DirAlias} = [@Array];
            $BuildQueue{$DirAlias}++;
            if ($Dir =~ /(\\|\/)/o) {
                $Dir = $module_to_build . $1 . $';
            } else {$Dir = $module_to_build;};
            $PathHash{$DirAlias} = $Dir;
        };
    };
    close BUILD_LST;
    foreach $Dir (keys %DeadDependencies) {
        next if defined $AliveDependencies{$Dir};
        if (!&IsHashNative($Dir)) {
            &RemoveFromDependencies($Dir, $dependencies_hash);
            delete $DeadDependencies{$Dir};
        };
    };
    &check_deps_hash($dependencies_hash);
};

#
# mark platform in order to proof if alias is used according to specs
#
sub mark_platform {
    my $prj_alias = shift;
    if (exists $prj_platform{$prj_alias}) {
        $prj_platform{$prj_alias} = 'all';
    } else {
        $prj_platform{$prj_alias} = shift;
    };
};

#
# Convert path from abstract (with '\' and/or '/' delimiters)
# to system-independent
#
sub CorrectPath {
    $_ = shift;
    if ( ($^O eq 'MSWin32') && (!defined $ENV{SHELL})) {
        s/\//\\/g;
    } else {;
        s/\\/\//g;
    };
    return $_;
};


sub check_dmake {
#print "Checking dmake...";
    if (open(DMAKEVERSION, "dmake -V |")) {
        my @dmake_version = <DMAKEVERSION>;
        close DMAKEVERSION;
#       if ($dmake_version[0] =~ /^dmake\s\-\sCopyright\s\(c\)/) {
#           print " Using version $1\n" if ($dmake_version[0] =~ /Version\s(\d+\.*\d*)/);
#        };
        return;
    };
    &print_error('dmake - no such file or directory');
};

#
# Get platform-dependent commands
#
sub get_commands {
    my $arg = '';
    # Setting alias for dmake
    $dmake = 'dmake';
    &check_dmake;

    if ($cmd_file) {
        if ($ENV{GUI} eq 'UNX') {
            $check_error_string = "if \"\$?\" != \"0\" exit\n";
        } else {
            $check_error_string = "if \"\%?\" != \"0\" quit\n";
        };
    };

    while ($arg = pop(@dmake_args)) {
        $dmake .= ' '.$arg;
    };
};

#
# Procedure prooves if current dir is a root dir of the drive
#
sub IsRootDir {
    my ($Dir);
    $Dir = shift;
    if (        (($ENV{GUI} eq 'UNX') ||
                 ($ENV{GUI} eq 'MACOSX')) &&
                ($Dir eq '/')) {
        return 1;
    } elsif (   (($ENV{GUI} eq 'WNT') ||
                 ($ENV{GUI} eq 'WIN') ||
                 ($ENV{GUI} eq 'OS2')) &&
                ($Dir =~ /\S:\/$/)) {
        return 1;
    } else {
        return 0;
    };
};

#
# Procedure retrieves list of projects to be built from build.lst
#
sub get_stand_dir {
    if (!(defined $ENV{GUI})) {
        $ENV{mk_tmp} = '';
        die "No environment set\n";
    };
    my $StandDir;
    do {
        $StandDir = cwd();
        if (open(BUILD_LST, 'prj/build.lst')) {
            $StandDir =~ /([\.\w]+$)/;
            $StandDir = $`;
            $CurrentPrj = $1;
            close(BUILD_LST);
            return $StandDir;
        } elsif (&IsRootDir($StandDir)) {
            $ENV{mk_tmp} = '';
            &print_error ('Found no project to build');
        };
    }
    while (chdir '..');
};

#
# Picks project which can be build now from hash and deletes it from hash
#
sub PickPrjToBuild {
    my ($Prj, $DepsHash);
    $DepsHash = shift;
    &handle_dead_children if ($QuantityToBuild);
    $Prj = &FindIndepPrj($DepsHash);
    delete $$DepsHash{$Prj};
    return $Prj;
};

#
# Make a decision if the project should be built on this platform
#
sub CheckPlatform {
    my $Platform = shift;
    return 1 if ($Platform eq 'all');
    return 1 if (($ENV{GUI} eq 'WIN') && ($Platform eq 'w'));
    return 1 if (($ENV{GUI} eq 'UNX') && ($Platform eq 'u'));
    return 1 if (($ENV{GUI} eq 'MAC') && ($Platform eq 'm'));
    return 1 if (($ENV{GUI} eq 'OS2') && ($Platform eq 'p'));
    return 1 if (($ENV{GUI} eq 'WNT') &&
                       (($Platform eq 'w') || ($Platform eq 'n')));
    return 0;
};

#
# Remove project to build ahead from dependencies and make an array
# of all from given project dependent projects
#
sub RemoveFromDependencies {
    my ($ExclPrj, $i, $Prj, $Dependencies);
    $ExclPrj = shift;
    my $ExclPrj_orig = '';
    $ExclPrj_orig = $` if ($ExclPrj =~ /\.lnk$/o);
    $Dependencies = shift;
    foreach $Prj (keys %$Dependencies) {
        foreach $i (0 .. $#{$$Dependencies{$Prj}}) {
            if ((${$$Dependencies{$Prj}}[$i] eq $ExclPrj) ||
                (${$$Dependencies{$Prj}}[$i] eq $ExclPrj_orig)) {
                splice (@{$$Dependencies{$Prj}}, $i, 1);
                $i = 0;
                last;
            };
        };
    };
};


#
# Check the hash for consistency
#
sub check_deps_hash {
    my $deps_hash_ref = shift;
    return if (!scalar keys %$deps_hash_ref);
    my %deps_hash = %$deps_hash_ref;
    my $consistent;
    foreach $key (keys %$deps_hash_ref) {
        $deps_hash{$key} = [@{$$deps_hash_ref{$key}}];
    };

    do {
        $consistent = '';
        foreach $key (keys %deps_hash) {
            @value_array = @{$deps_hash{$key}};
            if ($#value_array == -1) {
                &RemoveFromDependencies($key, \%deps_hash);
                delete $deps_hash{$key};
                $consistent = 1;
            };
        };
    } while ($consistent && (scalar keys %deps_hash));
    return if ($consistent);
    print STDERR "Fatal error:";
    foreach (keys %deps_hash) {
        print STDERR "\n\t$_ depends on: ";
        foreach my $i (@{$deps_hash{$_}}) {
            print STDERR (' ', $i);
        };
    };
    if ($child) {
        my $oldfh = select STDERR;
        $| = 1;
        _do_exit(1);
    } else {
        &print_error ("There are dead or circular dependencies\n");
    };
};

#
# Find undependent project
#
sub FindIndepPrj {
    my ($Prj, @Prjs, @PrjDeps, $Dependencies, $i);
    my $children = &children_number;
    return '' if ($children && ($children >= $QuantityToBuild));
    $Dependencies = shift;
    @Prjs = keys %$Dependencies;
    if ($#Prjs != -1) {
        foreach $Prj (@Prjs) {
            next if (&IsHashNative($Prj));
            @PrjDeps = @{$$Dependencies{$Prj}};
            return $Prj if ($#PrjDeps == -1);
        };
        return '';
    } else {
        $no_projects = 1;
        return '';
    };
};

#
# Check if given entry is HASH-native, that is not a user-defined data
#
sub IsHashNative {
    my $Prj = shift;
    return 1 if ($Prj =~ /^HASH\(0x[\d | a | b | c | d | e | f]{6,}\)/);
    return 0;
};

#
# Getting array of dependencies from the string given
#
sub GetDependenciesArray {
    my ($DepString, @Dependencies, $ParentPrj, $prj, $string);
    @Dependencies = ();
    $DepString = shift;
    $string = $DepString;
    $prj = shift;
    while ($DepString !~ /^NULL/o) {
        &print_error("Project $prj has wrong written dependencies string:\n $string") if (!$DepString);
        $DepString =~ /(\S+)\s*/o;
        $ParentPrj = $1;
        $DepString = $';
        if ($ParentPrj =~ /\.(\w+)$/o) {
            $ParentPrj = $`;
            if (($prj_platform{$ParentPrj} ne $1) &&
                ($prj_platform{$ParentPrj} ne 'all')) {
                &print_error ("$ParentPrj\.$1 is a wrong dependency identifier!\nCheck if it is platform dependent");
            };
            $AliveDependencies{$ParentPrj}++ if (&CheckPlatform($1));
            push(@Dependencies, $ParentPrj);
        } else {
            if ((exists($prj_platform{$ParentPrj})) &&
                ($prj_platform{$ParentPrj} ne 'all') ) {
                &print_error("$ParentPrj is a wrong used dependency identifier!\nCheck if it is platform dependent");
            };
            push(@Dependencies, $ParentPrj);
        };
    };
    return @Dependencies;
};


#
# Getting current directory list
#
sub GetDirectoryList {
    my ($Path);
    $Path = shift;
    opendir(CurrentDirList, $Path);
    @DirectoryList = readdir(CurrentDirList);
    closedir(CurrentDirList);
    return @DirectoryList;
};

sub finish_logging {
    return if ($show || (!defined $log));
    my $message = shift;
    $message = 'SUCCESS.'  if (!$message);
    $message .= " Built $modules_number modules.";
    $log->end_log_extended($script_name,$vcsid,$message);
};

sub print_error {
    my $message = shift;
    $modules_number -= scalar keys %global_deps_hash;
    $modules_number -= 1;
    &finish_logging("FAILURE: " . $message);
    print STDERR "\nERROR: $message\n";
    $ENV{mk_tmp} = '';
    close CMD_FILE if ($cmd_file);
    unlink ($cmd_file);
    do_exit(1) if (!$child);
};

sub usage {
    print STDERR "\nbuild\n";
    print STDERR "Syntax:   build    [--all|-a[:prj_name]]|[--from|-f prj_name1[:prj_name2] [prj_name3 [...]]]|[--since|-c prj_name] [--with_branches|-b]|[--prepare|-p][:platform]] [--deliver|-d [--dlv_switch deliver_switch]]] [-P processes] [--show|-s] [--help|-h] [--file|-F] [--ignore|-i] [--version|-V] [--mode|-m OOo[,SO[,EXT]] [-- dmake_options] \n";
    print STDERR "Example:  build --from sfx2\n";
    print STDERR "              - build projects including current one from sfx2\n";
    print STDERR "Example:  build --all:sfx2\n";
    print STDERR "              - the same as --all, but skip all projects that have been already built when using \"--all\" switch before sfx2\n";
    print STDERR "Keys:     --all       - build all projects from very beginning till current one\n";
    print STDERR "      --from      - build all projects dependent from the specified (including it) till current one\n";
    print STDERR "      --mode OOo      - build only projects needed for OpenOffice.org\n";
    print STDERR "      --prepare- clear all projects for incompartible build from prj_name till current one [for platform] (cws version)\n";
    print STDERR "      --with_branches- build all projects in neighbour branches and current branch starting from actual project\n";
    print STDERR "      --since     - build all projects beginning from the specified till current one (the same as \"--all:prj_name\", but skipping prj_name)\n";
    print STDERR "      --show      - show what is going to be built\n";
    print STDERR "      --file      - generate command file file_name\n";
    print STDERR "      --deliver   - only deliver, no build (usable for \'-all\' and \'-from\' keys)\n";
    print STDERR "      -P          - start multiprocessing build, with number of processes passed (UNIXes only)\n";
    print STDERR "      --dlv_switch    - use deliver with the switch specified\n";
    print STDERR "      --help      - print help info\n";
    print STDERR "      --ignore    - force tool to ignore errors\n";
    print STDERR "Default:          - build current project\n";
    print STDERR "Keys that are not listed above would be passed to dmake\n";
};

sub init_logging {
    return if ($show || (!defined $log));
    my $parameter_list = '';
    foreach (@ARGV) {$parameter_list .= "$_\;"};
    $parameter_list = $` if ($parameter_list =~ /;$/o);

    my $childws  = $ENV{CWS_WORK_STAMP};
    my $masterws = $ENV{WORK_STAMP};
    return if (!defined( $childws ) || !defined( $masterws ));
    $log->start_log_extended($script_name, $parameter_list, $masterws, $childws);
};

#
# Get all options passed
#
sub get_options {
    my $arg;
    &init_logging;
    while ($arg = shift @ARGV) {
        $arg =~ /^-P$/          and $QuantityToBuild = shift @ARGV  and next;
        $arg =~ /^-P(\d+)$/         and $QuantityToBuild = $1 and next;
        $arg =~ /^--all$/       and $BuildAllParents = 1            and next;
        $arg =~ /^-a$/      and $BuildAllParents = 1            and next;
        $arg =~ /^--show$/      and $show = 1                       and next;
        $arg =~ /^-s$/      and $show = 1                       and next;
        $arg =~ /^--deliver$/   and $deliver = 1                    and next;
        $arg =~ /^-d$/  and $deliver = 1                    and next;
        $arg =~ /^--dlv_switch$/    and $dlv_switch = &get_switch_options   and next;
        $arg =~ /^--file$/      and $cmd_file = shift @ARGV         and next;
        $arg =~ /^-F$/      and $cmd_file = shift @ARGV         and next;

        $arg =~ /^--with_branches$/     and $BuildAllParents = 1
                                and $build_from = shift @ARGV       and next;
        $arg =~ /^-b$/      and $BuildAllParents = 1
                                and $build_from = shift @ARGV       and next;

        $arg =~ /^--all:(\S+)$/ and $BuildAllParents = 1
                                and $build_from_opt = $1            and next;
        $arg =~ /^-a:(\S+)$/ and $BuildAllParents = 1
                                and $build_from_opt = $1            and next;
        if ($arg =~ /^--from$/ || $arg =~ /^-f$/) {
                                    $BuildAllParents = 1;
                                    &get_incomp_projects;
                                    next;
        };
        $arg =~ /^--prepare$/   and $prepare = 1 and next;
        $arg =~ /^-p$/          and $prepare = 1 and next;
        $arg =~ /^--prepare:/   and $prepare = 1 and $only_platform = $' and next;
        $arg =~ /^-p:/          and $prepare = 1 and $only_platform = $' and next;
        $arg =~ /^--since$/     and $BuildAllParents = 1
                                and $build_since = shift @ARGV      and next;
        $arg =~ /^-c$/      and $BuildAllParents = 1
                                and $build_since = shift @ARGV      and next;
        $arg =~ /^-s$/          and $BuildAllParents = 1
                                and $build_since = shift @ARGV      and next;
        $arg =~ /^--help$/      and &usage                          and do_exit(0);
        $arg =~ /^-h$/      and &usage                          and do_exit(0);
        $arg =~ /^--ignore$/        and $ignore = 1                         and next;
        $arg =~ /^-i$/      and $ignore = 1                         and next;
        $arg =~ /^--version$/   and do_exit(0);
        $arg =~ /^-V$/          and do_exit(0);
        $arg =~ /^-m$/          and &get_modes      and next;
        $arg =~ /^--mode$/      and &get_modes      and next;
        if ($arg =~ /^--$/) {
            &get_dmake_args;
            next;
        };
        push (@dmake_args, $arg);
    };
    &print_error('Switches --with_branches and --all collision') if ($build_from && $build_from_opt);

    &print_error('Switches --with_branches and --since collision') if ($build_from && $build_since);
    $cmd_file = '' if ($show);
    if (($ENV{GUI} eq 'WNT') && $QuantityToBuild) {
        $QuantityToBuild = 0;
        &print_error('-P switch is disabled for windows!\n');
    };
    $incompartible = scalar keys %incompartibles;
    if ($prepare && !$incompartible) {
        &print_error("--prepare is for use with --from switch only!\n");
    };
    if ($QuantityToBuild && $ignore) {
        &print_error("Cannot ignore errors in multiprocessing build");
    };
#    if ($incompartible && (!defined $ENV{CWS_WORK_STAMP})) {
#       print "-incomp_from switch is implemented for cws only!\n";
#        print "Ignored...";
#        $incompartible = '';
#    };
    if ($only_platform) {
        $only_common = 'common';
        $only_common .= '.pro' if ($only_platform =~ /\.pro$/);
    };
    @ARGV = @dmake_args;
};

sub get_dmake_args {
    my $arg;
    while ($arg = shift @ARGV) {
        next if ($arg =~ /^--$/);
        push (@dmake_args, $arg);
    };
};

#
# get all options without '-'
#
sub get_switch_options {
    my $string = '';
    my $option = '';
    while ($option = shift @ARGV) {
        if (!($option =~ /^-+/)) {
            $string .= '-' . $option;
            $string .= ' ';
        } else {
            unshift(@ARGV, $option);
            last;
        };
    };
    $string =~ s/\s$//;
    return $string;
};

#
# cancel build when one of children has error exit code
#
sub cancel_build {
    $modules_number -= scalar keys %global_deps_hash;
    my $log_string = 'FAILURE. Build is broken in modules: ';
    if ($BuildAllParents) {
        $modules_number -= scalar @broken_modules_names;
        print STDERR "\n";
        print STDERR scalar @broken_modules_names;
        print STDERR " module(s): ";
        foreach (@broken_modules_names) {
            print STDERR "\n\t$_";
            $log_string .= " $_";
            &RemoveFromDependencies($_, \%global_deps_hash);
        };
        &finish_logging($log_string);
        print STDERR "\nneed(s) to be rebuilt\n\nReason(s):\n\n";
        foreach (keys %broken_build) {
            print STDERR "ERROR: error " . $broken_build{$_} . " occurred while making $_\n";
        };
        print STDERR "\nAttention: if you build and deliver the above module(s) you may prolongue your build from module " . &PickPrjToBuild(\%global_deps_hash) . "\n";
    } else {
        &finish_logging($log_string . $CurrentPrj);
        kill 9 => -$$;
    };
    print STDERR "\n";
    do_exit(1);
};

#
# Function for storing error in multiprocessing AllParents build
#
sub store_error {
    my ($pid, $error_code) = @_;
    my $child_nick = $processes_hash{$pid};
    $broken_modules_hashes{$folders_hashes{$child_nick}}++;
    $broken_build{&CorrectPath($StandDir . $PathHash{$child_nick})} = $error_code;
    &cancel_build if (!$BuildAllParents);
};

#
# child handler (clears (or stores info about) the terminated child)
#
sub handle_dead_children {
    my $pid = 0;
    if (($pid = waitpid( -1, &WNOHANG)) > 0) {
        &store_error($pid, $?) if ($?);
        &clear_from_child($pid);
    };
    while(&children_number() >= $QuantityToBuild) {
        sleep 1;
        &handle_dead_children;
    };
};

sub clear_from_child {
    my $pid = shift;
    my $child_nick = $processes_hash{$pid};
      &RemoveFromDependencies($child_nick,
                            $folders_hashes{$child_nick});
    $running_children{$folders_hashes{$child_nick}}--;
    delete $processes_hash{$pid};
    $only_dependent = 0;
};

#
# Build the entire project according to queue of dependencies
#
sub BuildDependent {
    $dependencies_hash = shift;
    my $pid = 0;
    my $child_nick = '';
    $running_children{$dependencies_hash} = 0 if (!defined $running_children{$dependencies_hash});
    while ($child_nick = &PickPrjToBuild($dependencies_hash)) {
        if (($QuantityToBuild)) { # multyprocessing not for $BuildAllParents (-all etc)!!
            do {
                &handle_dead_children;
                return if (defined $broken_modules_hashes{$dependencies_hash});
                # start current child & all
                # that could be started now
                &start_child($child_nick) if ($child_nick);
                $child_nick = &PickPrjToBuild($dependencies_hash);
                while ($only_dependent) {
                    return if ($BuildAllParents);
                    sleep 1;
                    $child_nick = &PickPrjToBuild($dependencies_hash);
                };
            } while (!$no_projects);
            return if ($BuildAllParents);
            &handle_dead_children while (&children_number());
            &mp_success_exit;
        } else {
            &dmake_dir($child_nick);
        };
        $child_nick = '';
    };
};

sub children_number {
    return scalar keys %processes_hash;
};

sub start_child {
    my $child_nick = shift;
    my $pid;
    my $children_running;
    my $oldfh = select STDOUT;
    $| = 1;
    if ($pid = fork) { # parent
        select $oldfh;
        $processes_hash{$pid} = $child_nick;
        $children_running = &children_number;
        print 'Running processes: ', $children_running, "\n";
        $maximal_processes = $children_running if ($children_running > $maximal_processes);
        $folders_hashes{$child_nick} = $dependencies_hash;
        $running_children{$dependencies_hash}++;
#       sleep(1) if ($BuildAllParents);
    } elsif (defined $pid) { # child
        select $oldfh;
        $child = 1;
        &dmake_dir($child_nick);
        do_exit(1);
    };
};

#
# Build everything that should be built multiprocessing version
#
sub build_multiprocessing {
    my $Prj;
    my @build_queue = ();       # array, containing queue of projects
                                # to build
    do {
        while ($Prj = &PickPrjToBuild(\%global_deps_hash)) {
            my $module_type = &module_classify($Prj);

            if ($module_type eq 'lnk') {
                &print_annonce($Prj);
                &RemoveFromDependencies($Prj, \%global_deps_hash);
                next;
            };

            if ($module_type eq 'img') {
                &print_annonce($Prj . '.incomp');
                &RemoveFromDependencies($Prj, \%global_deps_hash);
                next;
            }
            &mark_force_deliver($Prj, &CorrectPath($StandDir.$Prj)) if (defined $ENV{CWS_WORK_STAMP});
            push @build_queue, $Prj;
            $projects_deps_hash{$Prj} = {};
            &get_deps_hash($Prj, $projects_deps_hash{$Prj});
        };
        sleep(1) if (!$Prj);
        &build_actual_queue(\@build_queue);
        if (scalar keys %broken_modules_hashes) {
            do {
                sleep(1);
                &handle_dead_children;
                &build_actual_queue(\@build_queue);
            } while (&children_number());
            &cancel_build;
        };
    } while (scalar (keys %global_deps_hash));
    # Let all children finish their work
    &cancel_build if (scalar keys %broken_build);
    &mp_success_exit;
};

sub mp_success_exit {
    print STDERR "\nMultiprocessing build is finished\n";
    print STDERR "Maximal number of processes run: $maximal_processes\n";
    do_exit(0);
};

#
# Here the built queue is built as long as possible
#
sub build_actual_queue {
    my $build_queue = shift;
    my $i = 0;
    do {
        while ($i <= (scalar(@$build_queue) - 1)) {
            $Prj = $$build_queue[$i];
            if (defined $broken_modules_hashes{$projects_deps_hash{$Prj}}) {
                push (@broken_modules_names, $Prj);
                splice (@$build_queue, $i, 1);
                next;
            };
            &annonce_module($Prj) if (!(defined $module_annonced{$Prj}));
            $only_dependent = 0;
            $no_projects = 0;
            &BuildDependent($projects_deps_hash{$Prj});
            if ($no_projects &&
                !$running_children{$projects_deps_hash{$Prj}} &&
                !defined $broken_modules_hashes{$projects_deps_hash{$Prj}})
            {
                chdir(&CorrectPath($StandDir.$Prj));
                system (&get_deliver_commando($Prj)) if (!$show && ($Prj ne $CurrentPrj));
                &RemoveFromDependencies($Prj, \%global_deps_hash);
                splice (@$build_queue, $i, 1);
                next;
            };
            $i++;
            &handle_dead_children;
        };
        $i = 0;
    } while (!&are_all_dependent($build_queue));
};

#
# Print announcement for module just started
#
sub annonce_module {
    my $Prj = shift;
    &print_annonce($Prj);
    $module_annonced{$Prj}++;
};

sub print_annonce {
    my $Prj = shift;
    my $text;
    if ($Prj =~ /\.lnk$/o) {
        $text = "Skipping link to $`\n";
    } elsif ($Prj =~ /\.incomp$/o) {
        $text = "Skipping incomplete $`\n";
    } else {
        $text = "Building project $Prj\n";
    };
    print $echo . "=============\n";
    print $echo . $text;
    print $echo . "=============\n";
};

sub are_all_dependent {
    my $build_queue = shift;
    my $folder = '';
    foreach my $prj (@$build_queue) {
        $folder = &FindIndepPrj($projects_deps_hash{$prj});
        return '' if ($folder);
    };
    return '1';
};

#
# Procedure checks out module or its image ($prj_name/prj)
#
sub checkout_module {
    my $prj_name = shift;
    my $image = shift;
    my $cws = Cws->new();
    $cws->child($ENV{CWS_WORK_STAMP});
    $cws->master($ENV{WORK_STAMP});
    my $cvs_module = &get_cvs_module($cws, $prj_name);
    &print_error("Cannot get cvs_module!!") if (!$cvs_module);
    my $parent_work_stamp = $ENV{WORK_STAMP};
    $parent_work_stamp .= '_' . $ENV{UPDMINOR} if (defined $ENV{UPDMINOR});

    $cvs_module->verbose(1);
    $cvs_module->{MODULE} .= '/prj' if ($image);
    if ($show) {
        print "Checking out $prj_name...\n";
        return;
    };
    $cvs_module->checkout($StandDir, $parent_work_stamp, '');
    # Quick hack, should not be there
    # if Heiner's Cws has error handling
    if (!-d &CorrectPath($StandDir.$prj_name)) {
        $cvs_module->checkout($StandDir, '', '');
        if (!-d &CorrectPath($StandDir.$prj_name)) {
            &print_error ("Cannot checkout $prj_name. Check if you have login to server");
        };
    };
};

#
# Procedure defines if the local directory is a
# complete module, an image or a link
# return values: lnk link
#                img incomplete (image)
#                mod complete (module)
#
sub module_classify {
    my $Prj = shift;
    return 'lnk' if ($Prj =~ /\.lnk$/o);
    opendir DIRHANDLE, $StandDir.$Prj;
    my @dir_content = readdir(DIRHANDLE);
    closedir(DIRHANDLE);
    # Check if there only 2 entries: CVS & prj
    # dirty, but must work
    if (scalar(@dir_content) <= 4) {
        foreach (@dir_content) {
            return 'mod' if ( ($_ ne 'CVS') &&
                            ($_ ne 'prj') &&
                            (!(/^\.+$/o))   );
        };
        return 'img';
    };
    return 'mod';
};

#
# This procedure provides consistency for cws
# for optimized build (ie in case of -with_branches, -all:prj_name
# and -since switches)
#
sub provide_consistency {
    &check_dir;
    foreach $var_ref (\$build_from, \$build_from_opt, \$build_since) {
        if ($$var_ref) {
            return if (-d $StandDir.$$var_ref);
            $$var_ref .= '.lnk' and return if (-d $StandDir.$$var_ref.'.lnk');
            my $current_dir = cwd();
            &checkout_module($$var_ref, 'image');
            chdir $current_dir;
            cwd();
            return;
        };
    };
};

#
# Retrieve CvsModule object for passed module.
# (Heiner's proprietary :)
#
sub get_cvs_module
{
    my $cws    = shift;
    my $module = shift;

    my $cvs_module = CvsModule->new();
    my ($method, $vcsid, $server, $repository) = get_cvs_root($cws, $module);

    return undef if  !($method && $vcsid && $server && $repository);

    $cvs_module->module($module);
    $cvs_module->cvs_method($method);
    $cvs_module->vcsid($vcsid);
    $cvs_module->cvs_server($server);
    $cvs_module->cvs_repository($repository);

    return $cvs_module;
};

#
# Try to get cvs coordinates via module link
#
sub get_link_cvs_root{
    my $module = shift;
    my $cvs_root_file = $StandDir.$module.'.lnk'.'/CVS/Root';
    if (!open(CVS_ROOT, $cvs_root_file)) {
        print STDERR "Attention: cannot read $cvs_root_file!!\n";
        return '';
    };
    my @cvs_root = <CVS_ROOT>;
    close CVS_ROOT;
    $cvs_root[0] =~ s/[\r\n]+//o;
    return $cvs_root[0] if (!($cvs_root[0] =~ /\^\s*$/));
    return '';
};

#
# Find out which CVS server holds the module, returns
# the elements of CVSROOT.
# (Heiner's proprietary)
#
sub get_cvs_root
{
    my $cws    = shift;
    my $module = shift;
    my $cvsroot = &get_link_cvs_root($module);
    if (!$cvsroot) {
        my $master = $cws->master();

        my $workspace_lst = get_workspace_lst();
        my $workspace_db = GenInfoParser->new();
        my $success = $workspace_db->load_list($workspace_lst);
        if ( !$success ) {
            print_error("Can't load workspace list '$workspace_lst'.", 4);
        }

        my $key = "$master/drives/o:/projects/$module/scs";
        $cvsroot = $workspace_db->get_value($key);

        if ( !$cvsroot  ) {
            print_error("No such module '$module' for '$master' in workspace database.", 0);
            return (undef, undef, undef, undef);
        }
    };
    my ($dummy1, $method, $user_at_server, $repository) = split(/:/, $cvsroot);
    my ($dummy2, $server) = split(/@/, $user_at_server);

    if ( ! ($method && $server && $repository ) ) {
        print_error("Can't determine CVS server for module '$module'.", 0);
        return (undef, undef, undef, undef);
    }

    return ($method, $vcsid, $server, $repository);
};

#
# Get the workspace list ('stand.lst'), either from 'localini'
# or, if this is not possible, from 'globalini.
# (Heiner's proprietary :)
#
sub get_workspace_lst
{
    my $home;
    if ( $^O eq 'MSWin32' ) {
        $home = $ENV{TEMP};
    }
    else {
        $home = $ENV{HOME};
    }
    my $localini = "$home/localini";
    if ( ! -f "$localini/stand.lst" ) {
        my $globalini = get_globalini();
        return "$globalini/stand.lst";
    }
    return "$localini/stand.lst";
}

#
# Procedure clears up module for incompartible build
#
sub ensure_clear_module {
    my $Prj = shift;
    my $module_type = shift;
    if ($module_type eq 'mod') {
        &clear_module($$Prj);
        my $lnk_name = $$Prj.'.lnk';
        if (-e ($StandDir.$lnk_name)) {
            print "Last checkout for $$Prj seems to be interrupted...\n";
            print "Check it out again...\n";
            $module_type = 'lnk';
            $$Prj = $lnk_name;
        } else {
            return;
        };
    };
    if ($module_type eq 'lnk') {
        print "\nBreaking link $$Prj...\n";
        return if ($show);
        $$Prj =~ /\.lnk$/o;
        my $new_name = $`;
        &checkout_module($new_name);
        my $action = '';
        if ( $^O eq 'MSWin32' ) {
            if(!rename("$StandDir$$Prj", "$StandDir$new_name.backup.lnk")) {
                $action = 'rename';
            };
        } else {
            if(!unlink $StandDir.$$Prj) {
                $action = 'remove';
            }
        };
        &print_error("Cannot $action $StandDir$$Prj. Please $action it manually") if ($action);
    } else {
        print "Checking out consistent " . $$Prj . "...\n";
        &checkout_module ($$Prj) if (!$show);
    };
};

#
# Procedure removes output tree from the module (without common trees)
#
sub clear_module {
    my $Prj = shift;
    print "Removing module's $Prj output trees...\n";
    print "\n" and return if ($show);
    opendir DIRHANDLE, $StandDir.$Prj;
    my @dir_content = readdir(DIRHANDLE);
    closedir(DIRHANDLE);
    foreach (@dir_content) {
        next if (/^\.+$/);
        my $dir = &CorrectPath($StandDir.$Prj.'/'.$_);
        if ((!-d $dir.'/CVS') && &is_output_tree($dir)) {
            #print "I would delete $dir\n";
            rmtree("$dir", 0, 1);
            if (defined $SIG{__WARN__}) {
                &print_error($SIG{__WARN__}) if ($SIG{__WARN__} ne '');
            };
        };
    };
};

#
# Figure out if the directory is an output tree
#
sub is_output_tree {
    my $dir = shift;
    $dir =~ /([\w\d\.]+)$/;
    $_ = $1;
    return '1' if (defined $platforms{$_});
    if ($only_common) {
        return '1' if ($_ eq $only_common);
    } else {
        return '1' if (/^common$/);
        return '1' if (/^common\.pro$/);
    };
    return '';
};

#
# This precedure checks if the module is consistent
# (criterion: presence of prj/build.lst file)
#
sub check_module_consistency {
    my $Prj = shift;
    my $prj_buld_lst = $StandDir.$Prj.'/prj/build.lst';
    if (!-f $prj_buld_lst) {
        &checkout_module($Prj, 'image');
        if (!-f $prj_buld_lst) {
            print "Cannot checkout consistent $Prj\n";
            # For Ause: Uncomment following line and
            # remove the upper one
            #&print_error "Cannot checkout consistent $Prj";
        };
    };
};

sub check_modules {
    my $modules_ref = shift;
    my @images = ();
    foreach my $module (@$modules_ref) {
        my $type = &module_classify($module);
        push(@images, $module) if ($type eq 'img');
        next if (scalar @images);
        $modules_types{$module} = $type;
    };
    if (scalar @images) {
        &print_error("You have incomplete modules: " . "@images\n" . "\nThis can cause incompatible binaries in the output tree. Please check these modules out or copy them with copyprj");
    };
};

#
# Removes projects which it is not necessary to build
# in incompartible build
#
sub prepare_incompartible_build {
    my ($prj, $deps_hash);
    $deps_hash = shift;
    foreach (keys %incompartibles) {
        my $incomp_prj = $_;
        $incomp_prj .= '.lnk' if (!defined $$deps_hash{$_});
        delete $incompartibles{$_};
        $incompartibles{$incomp_prj} = $$deps_hash{$incomp_prj};
        delete $$deps_hash{$incomp_prj};
    }
    while ($prj = &PickPrjToBuild($deps_hash)) {
        &RemoveFromDependencies($prj, $deps_hash);
        &RemoveFromDependencies($prj, \%incompartibles);
    };
    foreach (keys %incompartibles) {
        $$deps_hash{$_} = $incompartibles{$_};
    };
    if ($build_from_opt) {
        &prepare_build_from_opt($deps_hash);
        delete $$deps_hash{$build_from_opt};
    };
    @modules_built = keys %$deps_hash;
    &check_modules(\@modules_built);
    &clear_delivered if ($prepare);
    print "\n";
    foreach $prj (keys %$deps_hash) {
        my $prj_lnk = '';
        my $module_type = $modules_types{$prj};
        $prj_lnk = $prj if ($module_type =~ /\.lnk$/o);
        if ($prepare) {
            &ensure_clear_module(\$prj, $module_type);
        } else {
            my $message;
            if ($module_type ne 'mod') {
                $message = "$prj is not a complete module!";
#           } elsif (-d &CorrectPath($StandDir.$prj.'/'. $ENV{INPATH})) {
#               $message = "$prj contains old output tree!";
            };
#&print_error("$message Prepare workspace with -prepare switch!") if ($message);
        };
        if ($prj_lnk) {
            $$deps_hash{$prj} = $$deps_hash{$prj_lnk};
            delete $$deps_hash{$prj_lnk};
        };
    };
    if ($build_from_opt) {
        $$deps_hash{$build_from_opt} = [];
        $build_from_opt = '';
    };
    print "\nPreparation finished\n\n" and  do_exit(0) if ($prepare);
};

#
# Removes projects which it is not necessary to build
# with -with_branches switch
#
sub prepare_build_from {
    my ($prj, $deps_hash);
    $deps_hash = shift;
    my %from_deps_hash = ();   # hash of dependencies of the -from project
    &GetParentDeps( $StandDir.$build_from,\%from_deps_hash);
    foreach $prj (keys %from_deps_hash) {
        delete $$deps_hash{$prj};
        &RemoveFromDependencies($prj, $deps_hash);
    };
};

#
# Removes projects which it is not necessary to build
# with --all:prj_name or --since switch
#
sub prepare_build_from_opt {
    my ($prj, $deps_hash, $border_prj);
    $deps_hash = shift;
    $border_prj = $build_from_opt if ($build_from_opt);
    $border_prj = $build_since if ($build_since);
    while ($prj = &PickPrjToBuild($deps_hash)) {
        $orig_prj = '';
        $orig_prj = $` if ($prj =~ /\.lnk$/o);
        if (($border_prj ne $prj) &&
            ($border_prj ne $orig_prj)) {
            &RemoveFromDependencies($prj, $deps_hash);
            next;
        } else {
            if ($build_from_opt) {
                $$deps_hash{$prj} = [];
            } else {
                &RemoveFromDependencies($prj, $deps_hash);
            };
            return;
        };
    };
};

sub get_modes {
    my $option = '';
    while ($option = shift @ARGV) {
        if ($option =~ /^-+/) {
            unshift(@ARGV, $option);
            return;
        } else {
            if ($option =~ /,/) {
                $build_modes{$`}++;
                unshift(@ARGV, $') if ($');
            } else {$build_modes{$option}++;};
        };
    };
    $build_modes{$option}++;
};

sub get_incomp_projects {
    my $option = '';
    while ($option = shift @ARGV) {
        if ($option =~ /^-+/) {
            unshift(@ARGV, $option);
            return;
        } else {
            if ($option =~ /(:)/) {
                $option = $`;
                &print_error("-from switch collision") if ($build_from_opt);
                $build_from_opt = $';
            };
            $incompartibles{$option}++;
        };
    };
};

sub get_platforms {
    my $platforms_ref = shift;
    if ($only_platform) {
        $$platforms_ref{$only_platform}++;
        return;
    };
    my $solver = $ENV{SOLARVERSION};
    my ($iserverbin, @platforms_conf);
    $iserverbin = "i_server -d ";
    $iserverbin .= $ENV{SOLAR_ENV_ROOT} . '/b_server/config/stand.lst -i ';
    my $workstamp = $ENV{WORK_STAMP};
    @platforms_conf = `$iserverbin $workstamp/Environments -l`;
    if ( $platforms_conf[0]  =~ /Environments/ ) {
        shift @platforms_conf;
    }

    foreach (@platforms_conf) {
        s/\s//g;
        my $s_path = $solver . '/' .  $_;
        $$platforms_ref{$_}++ if (-e $s_path);
    };
    &print_error("There is no platform found!!") if (!scalar keys %platforms);
};

#
# This procedure clears solver from delivered
# by the modules to be build
#
sub clear_delivered {
    print "Clearing up delivered\n";
    my %backup_vars;
    foreach my $platform (keys %platforms) {
        print "\nRemoving delivered for $platform\n";
        my %solar_vars = ();
        &read_ssolar_vars($platform, \%solar_vars);
        foreach (keys %solar_vars) {
            if (!defined $backup_vars{$_}) {
                $backup_vars{$_} = $ENV{$_};
            };
            $ENV{$_} = $solar_vars{$_};
        };
        my $undeliver = "$deliver_commando -delete $nul";
        foreach my $module (@modules_built) {
            my $module_path = &CorrectPath($StandDir.$module);
            print "Removing delivered from module $module\n";
            next if ($show);
            my $current_dir = cwd();
            chdir($module_path.'.lnk') or chdir($module_path);
            if (system($undeliver)) {
                $ENV{$_} = $backup_vars{$_} foreach (keys %backup_vars);
                &print_error("Cannot run: $undeliver");
            }
            chdir $current_dir;
            cwd();
        };
    };
    $ENV{$_} = $backup_vars{$_} foreach (keys %backup_vars);
};

#
# Run setsolar for given platform and
# write all variables needed in %solar_vars hash
#
sub read_ssolar_vars {
    my ($setsolar, $entries_file, $tmp_file);
    $setsolar = $ENV{ENV_ROOT} . '/etools/setsolar.pl';
    my ($platform, $solar_vars) = @_;
    if ( $^O eq 'MSWin32' ) {
        $tmp_file = $ENV{TEMP} . "\\solar.env.$$.tmp";
    } else {
        $setsolar = '/net/jumbo.germany/cvs/buildenv/etools/setsolar.pl' if ! -e $setsolar;
        $tmp_file = $ENV{HOME} . "/.solar.env.$$.tmp";
    };
    print_error('There is no setsolar found') if !-e $setsolar;
    my $pro = "";
    if ($platform =~ /\.pro$/) {
        $pro = "-pro";
        $platform = $`;
    };
    my $param = "-$ENV{WORK_STAMP} $pro $platform";
    my $ss_comando = "$perl $setsolar -file $tmp_file $param $nul";
    $entries_file = '/CVS/Entries';
    if (system($ss_comando)) {
        unlink $tmp_file;
        &print_error("Cannot run commando:\n$ss_comando");
    };
    &get_solar_vars($solar_vars, $tmp_file);
};

#
# read variables to hash
#
sub get_solar_vars {
    my ($solar_vars, $file) = @_;
    my ($var, $value);
    open SOLARTABLE, "<$file" or die "can�t open solarfile $file";
    while(<SOLARTABLE>) {
        s/\r\n//o;
        next if(!/^\w+\s+(\w+)/o);
        next if (!defined $deliver_env{$1});
        $var = $1;
        if ( $^O eq 'MSWin32' ) {
            /$var=(\S+)$/o;
            $value = $1;
        } else {
            /\'(\S+)\'$/o;
            $value = $1;
        };
        $$solar_vars{$var} = $value;
    };
    close SOLARTABLE;
    unlink $file;
}

#
# Procedure checks out the module when we're
# in link
#
sub checkout_current_module {
    my $module_name = shift;
    my $link_name = $module_name . '.lnk';
    chdir $StandDir;
    cwd();
    print "\nBreaking link to module $module_name";
    &checkout_module($module_name);
    if (!-d $module_name) {
        &print_error("Cannot checkout $module_name");
    };
    my $action;
    if ( $^O eq 'MSWin32' ) {
        $action = 'rename' if (!rename($link_name,
                                $module_name.'.backup.lnk'));
    } else {
        $action = 'remove' if (!unlink $link_name);
    };
    &print_error("Cannot $action $link_name. Please $action it manually") if ($action);
    chdir $module_name;
    cwd();
};

sub check_dir {
    my $start_dir = cwd();
    my @dir_entries = split(/[\\\/]/, $start_dir);
    my $current_module = $dir_entries[$#dir_entries];
    $current_module = $` if ($current_module =~ /(\.lnk)$/);
    my $link_name = $ENV{SRC_ROOT}.'/'.$current_module.'.lnk';
    if ( $^O eq 'MSWin32' ) {
        $start_dir =~ s/\\/\//go;
        $link_name =~ s/\\/\//go;
        if (lc($start_dir) eq lc($link_name)) {
            &checkout_current_module($current_module);
        };
    } elsif ((-l $link_name) && (chdir $link_name)) {
        if ($start_dir eq cwd()) {
            # we're dealing with link => fallback to SRC_ROOT under UNIX
            $StandDir = $ENV{SRC_ROOT}.'/';
            &checkout_current_module($current_module);
            return;
        } else {
            chdir $start_dir;
            cwd();
        };
    };
};

sub mark_force_deliver {
    my ($module_name, $module_path) = @_;
#   my $cws_tag_string = 'Tcws_' . lc($ENV{WORK_STAMP}.'_'.$ENV{CWS_WORK_STAMP});
    my $cvs_tag_file = $module_path . '/CVS/Tag';
    return if (!open CVSTAG, "<$cvs_tag_file");
    my @tag = <CVSTAG>;
    close CVSTAG;
    $tag[0] =~ /^(\S+)/o;
    $force_deliver{$module_name}++ if ($1 =~ /^Tcws_/o);
};

sub get_deliver_commando {
    my $module_name = shift;
    return $deliver_commando if (!defined $force_deliver{$module_name});
    return $deliver_commando . ' -force';
};

#
# Store all available build modi in %build_modes
#
sub get_build_modes {
    return if (scalar keys %build_modes);
    if (defined $ENV{BUILD_TYPE}) {
        if ($ENV{BUILD_TYPE} =~ /\s+/o) {
            my @build_modes = split (/\s+/, $ENV{BUILD_TYPE});
            $build_modes{$_}++ foreach (@build_modes);
        } else {
            $build_modes{$ENV{BUILD_TYPE}}++;
        };
        return;
    };
    # Default build modes(for OpenOffice.org)
    $build_modes{'OOo'}++;
    $build_modes{'EXT'}++;
};

#
# pick only the modules, that should be built for
# build types from %build_modes
#
sub pick_for_build_type {
    my $modules = shift;
    my @mod_array = split(/\s+/, $modules);
    my $new_modules = '';
    foreach (@mod_array) {
        if (/(\w+):(\S+)/o) {
            $new_modules .= $2 . ' ' if (defined $build_modes{$1});
            next;
        };
        $new_modules .= $_ . ' '
    };
    return $new_modules;
};

sub do_exit {
    my $exit_code = shift;
    if ($exit_code) {
#        &finish_logging("error occured");
    } else {
#        &finish_logging;
    };
    exit($exit_code);
};
