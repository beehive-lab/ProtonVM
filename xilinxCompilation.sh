#!/usr/bin/env bash

mkdir -p build
cd build 

PLATFORM=xilinx_kcu1500_dynamic_5_0

## First compilation
xocc -c -s -o interpreter.xo -t hw --platform $PLATFORM ../interpreter.cl

## Linking and bitstream generation
xocc -l -s -o mykerinterpreternel.xclbin -t hw --platform $PLATFORM interpreter.xo

cd -
