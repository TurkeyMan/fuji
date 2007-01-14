#!/bin/sh
#echo "your moma $1\n"
#Archiver.exe -pc -ini=rkv.ini
FUJI_PLATFORM="LINUX"
case "$FUJI_PLATFORM" in
  LINUX ) PLATFORM="lnx";;
  *     ) echo "unknown platform: $FUJI_PLATFORM";;
esac
Archiver -$PLATFORM -ini=rkv.ini
#Archiver -"$1" -ini=rkv.ini
#pause