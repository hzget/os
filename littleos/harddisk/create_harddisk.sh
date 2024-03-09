#!/bin/bash

# create a harddisk with a filesystem that supports FAT16
# and copy a txt file to it

rm -rf boot.bin harddisk.img
nasm -f bin boot.asm -o boot.bin
dd if=boot.bin >> harddisk.img
dd if=/dev/zero bs=1048576 count=16 >> harddisk.img
sudo mount -t vfat harddisk.img /mnt
sudo cp hello.txt /mnt/
sudo cp ../bin/* /mnt/
sudo umount /mnt
