#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: globals.pm,v $
#
#   $Revision: 1.73 $
#
#   last change: $Author: kz $ $Date: 2007-09-06 09:51:38 $
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

package installer::globals;

############################################
# Global settings
############################################

BEGIN
{
    $prog="make_installer.pl";

    @noMSLocaleLangs = ( "br", "dz", "km", "nr", "ns", "rw", "ss", "st", "tg", "ts", "tn", "ve", "xh", "zu" , "ne" , "bn", "bn-BD", "bn-IN", "lo", "cy" , "ku" , "as-IN" , "te-IN" , "ml-IN" , "mr-IN", "ur-IN", "ta-IN", "or-IN" , "ti-ER" , "eo" , "ka" ,"ga" , "uk"  );

    $ziplistname = "";
    $pathfilename = "";
    $setupscriptname = "";
    $headerfilename = "";
    $shellscriptsfilename = "";
    $product = "";
    $languagelist = "";
    $added_english = 0;
    $set_office_start_language = 0;
    $solarjavaset = 0;

    $destdir = "";
    $rootpath = "";

    $productextension = "";
    @languageproducts = ();
    $build = "";
    $minor = "";
    $lastminor = "";
    $compiler = "";
    $pro = 0;
    $dounzip = 1;
    $languages_defined_in_productlist = 0;
    $setupscript_defined_in_productlist = 0;
    $services_rdb_created = 0;
    $servicesrdb_can_be_created = 0;
    $islinux = 0;
    $issolaris = 0;
    $iswindowsbuild = 0;
    $islinuxbuild = 0;
    $islinuxrpmbuild = 0;
    $issolarisbuild = 0;
    $issolarispkgbuild = 0;
    $issolarissparcbuild = 0;
    $issolarisx86build = 0;
    $isfreebsdpkgbuild = 0;
    $unpackpath = "";
    $idttemplatepath = "";
    $idtlanguagepath = "";
    $packjobref = "";
    $buildid = "Not set";
    $guidcounter = 1000; # for uniqueness of guids
    $fontsfolder = "FontsFolder";
    $fontsfoldername = "Fonts";
    $fontsdirparent = "";
    $fontsdirname = "";
    $fontsdirhostname = "truetype";
    $officefolder = "OfficeFolder";
    $officemenufolder = "OfficeMenuFolder";
    $startupfolder = "StartupFolder";
    $programfilesfolder = "ProgramFilesFolder";
    $templatefolder = "TemplateFolder";
    $templatefoldername = "Templates";
    $programmenufolder = "ProgramMenuFolder";
    $adafolder = "adabas";
    $encodinglistname = "msi-encodinglist.txt";
    $msiencoding = "";  # hash reference for msi encodings
    $msilanguage = "";  # hash reference for msi languages LCID
    $sofficeiconadded = 0;
    $temppath = "";
    $globaltempdirname = "ooopackaging";
    $cyg_temppath = "";
    $temppathdefined = 0;
    $jdstemppathdefined = 0;
    $packageversion = 1;
    $packagerevision = 1;
    $rpmcommand = "";
    $rpmquerycommand = "";
    $debian = "";
    $installertypedir = "";
    $controlledmakecabversion = "5";
    $saved_packages_path = "";
    $max_lang_length = 50;
    $globalblock = "Globals";
    $rootmodulegid = "";

    $is_special_epm = 0;
    $epm_in_path = 0;
    $epm_path = "";
    $epmoutpath = "";
    $upx_in_path = 0;
    $upxfile = "";
    $simple = 0;
    $simpledefaultuserdir = "\$ORIGIN/..";
    $call_epm = 1;
    $packageformat = "";
    $packagename = "";
    $packagelist = "";
    $addpackagelist = "";
    $is_unix_multi = 0;
    $unixmultipath = "";
    $unixmultipath_orig = "";
    $alllanguagesinproductarrayref = "";
    $shiptestdirectory = "";
    $makelinuxlinkrpm = 0;
    $linuxlinkrpmprocess = 0;
    $add_required_package = "";
    $linuxrespin = 0;
    @linuxpatchfiles = ();
    $linuxlibrarybaselevel = "1";
    $linuxlibrarypatchlevel = "1.1";
    @linuxlinks = ();
    @linkrpms = ();

    $strip = 1;
    $solarjava = 0;
    $jdklib = "";
    $jrepath = "";

    $globallogging = 0;
    $logfilename = "logfile.log";   # the default logfile name for global errors
    @logfileinfo = ();
    @errorlogfileinfo = ();
    @globallogfileinfo = ();
    $exitlog = "";
    $globalinfo_copied = 0;
    $quiet = 0;

    $debug = 0;
    $debugfilename = "debug.txt";
    $checksumfilename = "checksum.txt";
    @functioncalls = ();

    $ismultilingual = 0;
    @multilanguagemodules = ();
    $languagemodulesparent = "gid_Module_Langpack_Base";
    $languagemodulesbase = "gid_Module_Root_";
    %alluniquefilenames = ();
    %alllcuniquefilenames = ();
    %uniquefilenamesequence = ();
    %dependfilenames = ();
    $isopensourceproduct = 1;
    $manufacturer = "";
    $longmanufacturer = "";
    $sundirname = "Sun";
    $codefilename = "codes.txt";
    $componentfilename = "components.txt";
    $productcode = "";
    $upgradecode = "";
    $msiproductversion = "";
    $created_new_component_guid = 0;
    @allddffiles = ();
    $infodirectory = "";
    $currentcontent = "";

    $createdxpddefaultlang = 0;
    $xpddir = "";
    $productxpdfile = "setup.xpd";
    $xpd_files_prepared = 0;
    $defaultlanguage = "";
    # @emptyxpdparents = ();
    @createdxpdfiles = ();
    @allxpdfiles = ();
    $isxpdplatform = 0;
    $javalanguagepath = "";
    $javasettozero = 0;
    $addlicensefile = 1;
    $addsystemintegration = 0;
    $addjavainstaller = 0;
    $added_directories = 0;
    $makedownload = 1;
    $makejds = 1;
    $jdsexcludefilename = "";
    $jds_language_controlled = 0;
    $correct_jds_language = 0;
    $islanguagepackinunixmulti = 0;
    @installsetfiles = ();
    @binarytableonlyfiles = ();
    @allscpactions = ();
    $languagepackaddon = "LanguagePack";
    $patchaddon = "Patch";
    $ooodownloadfilename = "";
    $downloadfilename = "";
    $downloadfileextension = "";
    $oooversionstring = "";

    $is_copy_only_project = 0;
    $is_simple_packager_project = 0;
    $addchildprojects = 0;
    $languagepack = 0;
    $tab = 0;
    $patch = 0;
    $patchincludepath = "";
    $refresh_includepathes = 0;
    $patchfilelistname = "patchfilelist.txt";
    @patchfilecollector = ();
    $nopatchfilecollector = "";
    @userregistrycollector = ();
    $addeduserregitrykeys = 0;

    $unomaxservices = 25;
    $javamaxservices = 15;

    $one_cab_file = 0;
    $fix_number_of_cab_files = 0;
    $cab_file_per_component = 0;
    $cabfilecompressionlevel = 2;
    $number_of_cabfiles = 4;    # only for $fix_number_of_cab_files = 1
    $include_cab_in_msi = 0;
    $use_packages_for_cabs = 1;
    $msidatabasename = "";
    $prepare_winpatch = 0;
    $previous_idt_dir = "";
    $updatepack = 0;

    $saveinstalldir = "";
    $csp_installdir = "";       # global installdir of createsimplepackage() in simplepackage.pm
    $csp_installlogdir = "";    # global installlogdir of createsimplepackage() in simplepackage.pm
    $csp_languagestring = "";   # global languagestring of createsimplepackage() in simplepackage.pm
    $localinstalldirset = 0;
    $localinstalldir = "";

    $adafilename = "";
    $javafilename = "";
    $javafilename2 = "";
    $javafilename3 = "";
    $javafile = "";
    $adafile = "";
    $urefile = "";

    $subdir = "";
    $postprocess_specialepm = 0;
    $postprocess_standardepm = 0;

    $starttime = "";

    @solarispatchscripts = ("checkinstall", "copyright", "patch_checkinstall", "patch_postinstall", "postinstall", "preinstall", "i.none");
    @solarispatchfiles = (".diPatch", "patchinfo");
    @environmentvariables = ( "SOLARVERSION", "GUI", "WORK_STAMP", "OUTPATH", "LOCAL_OUT", "LOCAL_COMMON_OUT" );
    @packagelistitems = ("module", "solarispackagename", "packagename", "copyright", "vendor", "description" );
    @regcompjars = ( "unoil.jar", "java_uno.jar", "ridl.jar", "jurt.jar", "juh.jar", "xmerge.jar", "commonwizards.jar" );
    @regcompregisterlibs = ( "javavm.uno", "javaloader.uno", "uriproc.uno" );
    @languagepackfeature =();
    @featurecollector =();
    $msiassemblyfiles = "";
    $nsisfilename = "Nsis.mlf";
    $linuxlinkrpms = "gid_Module_Root_Files_3,gid_Module_Root_Files_4,gid_Module_Root_Files_5";
    $extensioninstalldir = "gid_Dir_Share_Extension_Install";
    @languagenames = ();
    @requiredpackages = ();
    %componentconditions = ();

    %allcabinets = ();
    %allcabinetassigns = ();
    %cabfilecounter = ();
    %lastsequence = ();

    @removedirs = ();
    @jdsremovedirs = ();
    @emptypackages = ();
    %fontpackageexists = ();

    $plat = $^O;

    if (( $plat =~ /MSWin/i ) || (( $plat =~ /cygwin/i ) && ( $ENV{'USE_SHELL'} eq "4nt" )))
    {
        $unzippath = "unzip.exe";           # Has to be in the path: r:\btw\unzip.exe
        $zippath= "zip.exe";                # Has to be in the path: r:\btw\zip.exe
        $checksumfile = "so_checksum.exe";
        $unopkgfile = "unopkg.exe";
        if ( $plat =~ /cygwin/i )
        {
            $separator = "/";
            $pathseparator = "\:";
            $quote = "\'";
        }
        else
        {
            $separator = "\\";
            $pathseparator = "\;";
            $quote = "\"";
        }
        $libextension = "\.dll";
        $isunix = 0;
        $iswin = 1;
        $wrapcmd = "";
        %savedmapping = ();
        %savedrevmapping = ();
        %savedrev83mapping = ();
        %saved83dirmapping = ();
    }
    elsif (( $plat =~ /cygwin/i ) && ( $ENV{'USE_SHELL'} ne "4nt" ))
    {
        $unzippath = "unzip";               # Has to be in the path: /usr/bin/unzip
        $zippath = "zip";                   # Has to be in the path: /usr/bin/zip
        $checksumfile = "so_checksum";
        $unopkgfile = "unopkg.exe";
        $separator = "/";
        $pathseparator = "\:";
        $libextension = "\.dll";
        $quote = "\'";
        $isunix = 0;
        $iswin = 1;
        $wrapcmd = $ENV{'WRAPCMD'}." -env ";
        %savedmapping = ();
        %savedrevmapping = ();
        %savedrev83mapping = ();
        %saved83dirmapping = ();
    }
    else
    {
        $unzippath = "unzip";               # Has to be in the path: /usr/bin/unzip
        $zippath = "zip";                   # Has to be in the path: /usr/bin/zip
        $checksumfile = "so_checksum";
        $unopkgfile = "unopkg";
        $separator = "/";
        $pathseparator = "\:";
        if ( $plat =~ /darwin/i )
        {
            $libextension = "\.dylib";
        }
        else
        {
            $libextension = "\.so";
        }
        $quote = "\'";
        $isunix = 1;
        $iswin = 0;
        $wrapcmd = "";
    }

    if ( $plat =~ /linux/i ) { $islinux = 1; }
    if ( $plat =~ /solaris/i ) { $issolaris = 1; }

    # ToDo: Needs to be expanded for additional platforms

}

1;
