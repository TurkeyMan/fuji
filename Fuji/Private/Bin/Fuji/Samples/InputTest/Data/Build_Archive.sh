#!/bin/sh
case "$1" in
  LINUX ) PLATFORM="lnx"
    VALID_PLATFORM=1;;
  OSX ) PLATFORM="osx"
    VALID_PLATFORM=1;;
  PSP ) PLATFORM="psp"
    VALID_PLATFORM=1;;
  PS2 ) PLATFORM="ps2"
    VALID_PLATFORM=1;;
  GC ) PLATFORM="gc"
    VALID_PLATFORM=1;;
  DC ) PLATFORM="dc"
    VALID_PLATFORM=1;;
  PC ) PLATFORM="pc"
    VALID_PLATFORM=1;;
  "" ) echo "No platform specified.\nPlease specify one of:\n  LINUX, OSX, PSP, PS2, PS3, GC, DC, PC";;
  *  ) echo "Unknown platform: '$1'\nPlease specify one of:\n  LINUX, OSX, PSP, PS2, PS3, GC, DC, PC";;
esac

if test -n "$VALID_PLATFORM"
then
  Archiver -$PLATFORM -ini=rkv.ini
fi
