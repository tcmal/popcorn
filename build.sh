#!/bin/sh
set -e
. ./config.sh

TASK=${1:-install install-headers}

for PROJECT in $PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" $MAKE $TASK)
done
