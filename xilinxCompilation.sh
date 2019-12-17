#!/usr/bin/env bash

PLATFORM=xilinx_kcu1500_dynamic_5_0

function compileGlobalInterpreter {
    mkdir -p buildGlobal
    cd buildGlobal
    xocc -c -s -o interpreter.xo -t hw --platform $PLATFORM ../interpreter.cl
    xocc -l -s -O3 -j12 -o mykerinterpreternel.xclbin -t hw --platform $PLATFORM interpreter.xo
    cd -
}

function compileLocalInterpreter {
    mkdir -p buildLocal
    cd buildLocal
    xocc -c -s -o interpreterLocal.xo -t hw --platform $PLATFORM ../interpreterLocal.cl
    xocc -l -s -O3 -j12 -o mykerinterpreternelLocal.xclbin -t hw --platform $PLATFORM interpreterLocal.xo
    cd -
}

function compilePrivateInterpreter {
    mkdir -p buildPrivate
    cd buildPrivate
    xocc -c -s -o interpreterPrivate.xo -t hw --platform $PLATFORM ../interpreterPrivate.cl
    xocc -l -s -O3 -j12 -o mykerinterpreternelPrivate.xclbin -t hw --platform $PLATFORM interpreterPrivate.xo
    cd -
}

compileGlobalInterpreter
compileLocalInterpreter
compilePrivateInterpreter
