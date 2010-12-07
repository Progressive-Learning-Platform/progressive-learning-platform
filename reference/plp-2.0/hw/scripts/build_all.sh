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

sh scripts/build_linux.sh
