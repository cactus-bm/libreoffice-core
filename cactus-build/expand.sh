#!/bin/bash

cp -r ../instdir .
cd instdir
rm -r -f help
rm -r -f readmes
rm -r -f libreofficedev
TOTAL_SIZE=`du -s --apparent-size . | awk '{print $1}'`
SHARE_SIZE=`du -s --apparent-size share | awk '{print $1}'`
PROGRAM_SIZE=`du -s --apparent-size program | awk '{print $1}'`
echo
echo total is $TOTAL_SIZE of which share size is $SHARE_SIZE program size is $PROGRAM_SIZE
echo Removing libraries from share
cd share
rm -r -f  Scripts
rm -r -f  autocorr
rm -r -f  autotext
rm -r -f  calc
rm -r -f  fingerprint
rm -r -f  firebird
rm -r -f  gallery
rm -r -f  libreofficekit
rm -r -f  pallette
rm -r -f  template
rm -r -f  theme_definitions
rm -r -f  tipoftheday
rm -r -f  toolbarmode
rm -r -f  wizards
rm -r -f  wordbook
#rm -r -f  xdg
rm -r -f  xpdfimport
rm -r -f  dtd
rm -r -f extensions
rm config/images*

# ------

# rm -r -f  xslt

echo Removing libraries from program
cd ..
cd program
rm JREProperties.class
rm -r -f __pycache__
rm -r -f classes
rm *.png
rm libjava*
rm libldap*
rm libofficebean*
rm mailmerge*
rm msgbox*
rm officehelp*
rm -r -f wizards
rm xpdfimport
rm -r -f opencl*
rm -r -f shell
rm java*
rm libC*
rm libEng*
rm libLang*

# ------------------ test
#
rm libOsi*
rm libpython3.8.so.1.0
rm libpython3.8.so
rm libmwaw-0.3-lo.so.3
rm liblocaledata_others.so
rm libpostgresql-sdbc-impllo.so
rm libdbulo.so
rm -r -f python-core-3.8.16
rm libwps-0.4-lo.so.4
rm libvbaswobjlo.so
rm libvbaobjlo.so
rm libdbalo.so
# rm libchartcorelo.so
# rm libchartcontrollerlo.so
rm libmysqlclo.so
rm libdict_*.so
rm libodbclo.so
rm libPresenterScreenlo.so
rm libwpftwriterlo.so
rm libucpcmis1lo.so
rm liblocaledata_euro.so
rm libetonyek-0.1-lo.so.1
rm libfbclient.so.2
rm libwpd-0.10-lo.so.10
rm libodfgen-0.1-lo.so.1
rm liblwpftlo.so
rm libbasegfxlo.so
rm libdbaselo.so
rm libfirebird_sdbclo.so
rm libcomphelper.so
rm librptlo.so
rm libjdbclo.so

# ----

# rm libsdlo.so
# rm libscfiltlo.so
# rm libpdfiumlo.so
# rm libooxlo.so
# rm libsclo.so
# rm libswlo.so
# rm libclucene.so
# rm libwpftdrawlo.so
# rm libicuuc.so.72
# rm libicui18n.so.72
# rm libicudata.so.72
# rm libmergedlo.so
# rm libslideshowlo.so
# rm libuno_cppuhelpergcc3.so.3

#------------

#rm libsmdlo.so*
#rm libuuresolverlo.so*
#rm libinvocadaptlo.so*
#rm libmozbootstraplo.so*
#rm libstoragefdlo.so*
#rm libjuhx.so*
#rm libmigrationoo3lo.so*
#rm libwriterperfectlo.so*
#rm libtextconversiondlgslo.so*
#rm libsdbc2.so*
#rm libwpftcalclo.so*
#rm libinvocationlo.so*
#rm libsdbtlo.so*
#rm libt602filterlo.so*
#rm libsolverlo.so*
#rm libsaxlo.so*
#rm libdbpool2.so*
#rm libabplo.so*
#rm libbinaryurplo.so*
#rm libdlgprovlo.so*
#rm libscriptframe.so*
#rm libtextconv_dict.so*
#rm libucpftp1.so*
#rm libmysql_jdbclo.so*
#rm libcached1.so*
#rm libcalclo.so*
#rm libpdffilterlo.so*
#rm libiolo.so*
#rm libdeploymentgui.so*
#rm libtllo.so*
#rm libflatlo.so*

#----------------------

rm libevoablo.so*
rm libucpdav1.so*
rm libbiblo.so*
rm libmsformslo.so*
rm libpdfimportlo.so*
rm libucbhelper.so*
rm libbootstraplo.so*
rm libcairocanvaslo.so*
rm libscuilo.so*
rm libsvgfilterlo.so*
rm libpcrlo.so*
rm libacclo.so*
rm libslideshowlo.so*
rm libsmlo.so*
rm libwpftdrawlo.so*
rm libcuilo.so*

echo adding extra libraries
for LIB in libdbus-1.so libssl3.so libsmime3.so libnss3.so libavahi-client.so libavahi-common.so\
           libsystemd.so libdw.so libdw-0.176.so libelf.so libelf-0.176.so libgcrypt.so libgpg-error.so liblz4.so libsystemd.so\
           libfontconfig.so
do
    if [[ ! -f $LIB ]]
    then
        echo copying ${LIB}
        ( cd /usr/lib64; tar -cf - ${LIB}*) | tar -xvf -
    fi
done
cd ..
cd ..
# This first off run needs to be done, it configures
# some things that modify data in the file structure
# If this was not done here then the first run
# after installing this data would abort without
# doing anything useful (except the configuration)
# it may be better to do this after installation
# as an installation uuid is created, doing it here
# means that all installation from this program
# will have the same identifier, for our purposes
# this is unlikely to matter.
echo "Doing config run"
LD_LIBRAY_PATH=`pwd`/instdir/program:$LD_LIBRARY_PATH
echo "1,2,3\n4,5,6\na,b,c" > x.csv
instdir/program/soffice.bin \
        --headless \
        --norestore \
        --invisible \
        --nodefault \
        --nofirststartwizard \
        --nolockcheck \
        --nologo \
        --convert-to "html" \
        x.csv

echo
TOTAL_SIZE=`du -s --apparent-size instdir | awk '{print $1}'`
SHARE_SIZE=`du -s --apparent-size instdir/share | awk '{print $1}'`
PROGRAM_SIZE=`du -s --apparent-size instdir/program | awk '{print $1}'`
echo
echo total is $TOTAL_SIZE of which share size is $SHARE_SIZE program size is $PROGRAM_SIZE
echo
echo
echo
# vim:set shiftwidth=4 softtabstop=4 expandtab:

