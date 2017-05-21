#!/bin/sh -x

binfile=LiftTracker.bin

rm $binfile
particle compile electron --target 0.6.1 ./src --saveTo $binfile
particle flash --usb $binfile
