#! /bin/bash

echo
rm cactus-libreoffice*.zip
rm lo.*
echo filtering ../instdir to instdir
./expand.sh
echo program copied
echo building release file
VERSION=$(cat ../configure |
          awk -F= '/PACKAGE_VERSION=/{if (!v){print $2;v=1}}' |
          sed "s/'//g" |
          sed 's/\./-/g' |
          sed 's/^\(\([0-9]\+-\)\{3\}[0-9]\+\).*$/\1/')
CV=`git tag | sort -V | tail -1`

echo Libreoffice build version is $VERSION
echo Cactus build version is $CV
echo tar
tar -cf lo.tar instdir
echo compress with brotli
brotli -Z -v lo.tar
echo zip
NAME=cactus-libreoffice_${VERSION}_${CV}.zip
zip $NAME lo.tar.br
echo built $NAME
du -sh --apparent-size lo* cactus*
# vim:set shiftwidth=4 softtabstop=4 expandtab:
