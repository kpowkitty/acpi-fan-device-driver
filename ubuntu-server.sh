#!/usr/bin/env bash

name="ubuntu-server" # NAME QCOW2 IMAGE THIS
path="/home/bee/vm/csc415/" # ROOT TO THESE FILES
dist="ubuntu-22.04.5-desktop-amd64.iso" # ISO IMAGE
bios_path="/usr/share/edk2-ovmf" # ROOT OF DRIVE FILES

CPU_TYPE="host"
CPU_FEATURES="vmware-cpuid-freq=on,hv-passthrough"
MACHINE_FEATURES="accel=kvm,kernel_irqchip=on"
MY_OPTIONS=""

MEM="4G"
CPU_SOCKETS="1"
CPU_CORES="1"
CPU_THREADS="2"

qemu-system-x86_64 -enable-kvm \
	-machine q35,"$MACHINE_FEATURES" \
	-cpu "$CPU_TYPE",kvm=on,vendor=GenuineIntel,"$CPU_FEATURES","$MY_OPTIONS" \
	-smp "$CPU_THREADS",cores="$CPU_CORES",sockets="$CPU_SOCKETS" \
	-m "$MEM" \
    -drive file=${bios_path}/OVMF_CODE.fd,readonly=on,format=raw,if=pflash \
    -drive file=${bios_path}/OVMF_VARS.fd,format=raw,if=pflash \
    -drive file=${path}/${name}.qcow2,id=hdd,format=qcow2,if=virtio,discard=unmap \
    -device virtio-net,netdev=vmnic -netdev user,id=vmnic \
    -usbdevice tablet \
	-vga virtio \
	-boot c \
	-acpitable file=${path}/fake_fans.aml \
    "$@"
