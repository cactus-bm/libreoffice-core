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
rm -r -f  xdg
rm -r -f  xpdfimport
#rm -r -f  xslt
rm -r -f  dtd
rm -r -f extensions
rm config/images*

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
rm libchartcorelo.so
rm libchartcontrollerlo.so
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

echo adding extra libraries
for LIB in libssl3.so libsmime3.so libnss3.so libfontconfig.so.1 libavahi-client.so.3.2.9 libavahi-common.so.3.5.3
do
    if [[ ! -f $LIB ]]
    then
        echo installing $LIB
        cp /usr/lib64/$LIB .
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

