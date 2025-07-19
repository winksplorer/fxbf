#!/bin/bash

doas mount /dev/sdb1 /mnt
doas rm /mnt/fxbf.g1a
doas cp program.bf /mnt/program.bf
doas cp fxbf.g1a /mnt/fxbf.g1a
doas umount /mnt
udisksctl power-off -b /dev/sdb

# "-I/home/wink/.local/share/fxsdk/sysroot/lib/gcc/sh3eb-elf/14.1.0/include",
# "-I/home/wink/.local/share/fxsdk/sysroot/sh3eb-elf/include",