#!/usr/bin/env sh

export CXX=g++
export CC=gcc

mkdir build && cd build

cmake \
    -DENABLE_SANITIZER_ADDRESS=ON \
    -DENABLE_SANITIZER_UNDEFINED=ON \
    ..

cmake --build .
