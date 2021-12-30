#!/bin/sh


os=`uname`

if test "$os" = Darwin
then
   /bin/cp Makefile.inc.MacOSX Makefile.inc 
elif test "$os" = Linux
then
   /bin/cp Makefile.inc.Linux Makefile.inc 
fi
