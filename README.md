# ACPI Fan Device Driver (Linux)

A **kernel-level device driver** for managing fan speed through ACPI.  
Users can turn fans on/off and control fan speed via this driver.

---

## Features

- Turn fans on or off programmatically  
- Adjust fan speed dynamically  
- Works with virtualized ACPI device trees  

---

## Prerequisites

- Linux development environment with kernel headers
- QEMU (or another VM that supports ACPI table injection)  
- `acpica-tools` for verifying ACPI tables  

---

## Build and Test Instructions

```bash
# Clone the repository
git clone git@github.com:kpowkitty/acpi-fan-device-driver.git
cd acpi-fan-device-driver

# Setup VM with virtualized ACPI table
# (Example script provided in repo for QEMU using -acpitable)

# Verify ACPI table loaded inside VM
ls /sys/firmware/acpi/tables/
# Should show 'SSDT'

cat /sys/firmware/acpi/tables/SSDT > ssdt.bin
iasl -d ssdt.bin
# Look for signatures 'VIRTIO' and 'FAKEFANS'
# Example of a successful decompiled table is in /Test

# Build kernel module
cd Module
make clean
make
sudo insmod virtfan.ko

# Run tests
cd ../Test
sudo make clean
sudo make run
```

---

Please see `writeup.pdf` for more information on the design process of this driver.
