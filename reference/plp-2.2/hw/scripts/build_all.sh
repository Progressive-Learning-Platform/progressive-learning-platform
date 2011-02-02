#!/bin/sh

echo "==========================================="
echo "Build bootrom image"
echo "==========================================="

cd bootrom
sh build.sh
cd ..

echo "==========================================="
echo "Build hardware"
echo "==========================================="

sh scripts/build_1200k_linux.sh
sh scripts/build_500k_linux.sh
