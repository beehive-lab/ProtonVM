#!/bin/env bash

libOCL=lib
if [ -L ${libOCL} ] ; then
	if [ -e ${libOCL} ] ; then
		echo "lib OK"
	fi
elif [ -e ${my_link} ] ; then
	ln -s src/ lib
fi

inputSize=(32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288)
for s in ${inputSize[@]}; do
    ./build/bin/gpuBenchmark $s
done
