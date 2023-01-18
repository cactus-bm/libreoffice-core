#! /bin/bash

echo
rm cactus-libreoffice*.zip
rm lo.*
echo tar does, expanding
./expand.sh
echo tar expanded
echo building release file
. instdir/sdk/settings/dk.mk
VERSION=$(echo $BUILDID | sed 's/\./-/g')
CV=`git tag | sort -V | tail -1`

echo tar
tar -cf lo.tar instdir
echo brotli zip
brotli -Z -v lo.tar
echo zip
NAME=cactus-libreoffice_${VERSION}_${CV}.zip
zip -9 $NAME lo.tar.br
echo built $NAME
du -s --apparent-size lo* cactus*
