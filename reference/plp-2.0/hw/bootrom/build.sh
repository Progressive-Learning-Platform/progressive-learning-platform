#!/bin/sh

java -jar ../../sw/PLPTool/dist/PLPTool.jar -af fload.asm bootloader
tar -xf bootloader.plp plp.hex
python ../scripts/plpromgen.py plp.hex
cp inferred_rom.v ../verilog
