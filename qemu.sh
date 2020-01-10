#!/bin/sh
set -e
. ./build.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -kernel build/boot/popcorn.kernel -no-reboot -no-shutdown -s -S
