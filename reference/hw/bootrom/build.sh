#!/bin/sh
java -jar ../../sw/PLPTool/dist/PLPTool.jar -plp bootloader.plp -c fload.asm  ../../sw/libplp/libplp_timer.asm ../../sw/libplp/libplp_leds.asm ../../sw/libplp/libplp_plpid.asm ../../sw/libplp/libplp_uart.asm ../../sw/libplp/libplp_switches.asm ../../sw/libplp/libplp_sseg.asm
java -jar ../../sw/PLPTool/dist/PLPTool.jar -plp bootloader.plp -a  
tar -xf bootloader.plp plp.hex
python ../scripts/plpromgen.py plp.hex
cp inferred_rom.v ../verilog
