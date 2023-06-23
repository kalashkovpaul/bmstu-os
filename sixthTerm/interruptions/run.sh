#!/bin/bash
set -Eeuxo pipefail

make
trap 'make clean' EXIT

sudo insmod tasklet.ko
sudo insmod myworkqueue.ko

cat /proc/interrupts

sudo dmesg -wH | grep 'Tasklet\|WorkQueue'

sudo rmmod myworkqueue.ko
sudo rmmod tasklet.ko
