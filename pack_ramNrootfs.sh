#!/bin/bash
echo "Enter .img file filename:"
read imgfile
if [ ! -f "$imgfile" ]; then
    echo "File '$imgfile' does not exist. Aborting."
    exit 1
fi

if [ ! $(find bios/extlinux -name "extlinux" | wc -l) ]; then
    echo "File 'extlinux' not found. Aborting."
    exit 4
fi

if [ ! -d "rootfs" ]; then
    echo "Directory 'rootfs' does not exist. Aborting."
    exit 2
fi
if [ ! -d "initramfs" ]; then
    echo "Directory 'initramfs' does not exist. Aborting."
    exit 3
fi

cd initramfs
    rm initramfs.cpio.gz

    find . -print0 | cpio --null -ov --format=newc > initramfs.cpio
    gzip -9 ./initramfs.cpio
    cp -i initramfs.cpio.gz ../rootfs
cd ..

if [ ! -d "temp" ]; then
    mkdir temp
fi

sudo mount $imgfile temp

sudo ./bios/extlinux/extlinux -i temp
sudo cp -au rootfs/. temp/

sudo umount temp
sudo rm -r temp

echo "Done."
