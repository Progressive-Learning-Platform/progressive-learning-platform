/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */


module arbiter(rst, clk, cpu_daddr, cpu_bus_data, bus_cpu_data, cpu_drw, cpu_iaddr, bus_cpu_inst, mod_leds_leds, mod_uart_txd, mod_uart_rxd, mod_switches_switches, mod_sseg_an, mod_sseg_display, cpu_stall, mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub, mod_sram_data, mod_sram_addr);
	input clk, rst;

	/* cpu i/o */
	input cpu_drw; /* data read/write. cpu_drw = 0 is read */
	input [31:0] cpu_daddr, cpu_iaddr; /* word aligned */
	input [31:0] cpu_bus_data; /* cpu data coming from the cpu */
	output [31:0] bus_cpu_data, bus_cpu_inst; /* cpu data going to the cpu */

	/* module i/o */
	output [7:0] mod_leds_leds;
	input mod_uart_rxd;
	output mod_uart_txd;
	input [7:0] mod_switches_switches;
	output [3:0] mod_sseg_an;
	output [7:0] mod_sseg_display;
	output cpu_stall, mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub;
	inout [15:0] mod_sram_data;
	output [23:1] mod_sram_addr;

	/* effective address calculation for the modules */
	wire [7:0] imod, dmod;
	wire [31:0] ieff_addr, deff_addr;
	mm imm(cpu_iaddr, imod, ieff_addr);
	mm dmm(cpu_daddr, dmod, deff_addr);

	/* instruction and data enable wires */
	wire mod0_ie = imod == 0;
	wire mod1_ie = imod == 1;
	wire mod2_ie = imod == 2;
	wire mod3_ie = imod == 3;
	wire mod4_ie = imod == 4;
	wire mod5_ie = imod == 5;
	wire mod6_ie = imod == 6;
	wire mod7_ie = imod == 7;
	wire mod8_ie = imod == 8;
	wire mod9_ie = imod == 9;
	wire mod10_ie = imod == 10;

	wire mod0_de = dmod == 0;
	wire mod1_de = dmod == 1;
	wire mod2_de = dmod == 2;
	wire mod3_de = dmod == 3;
	wire mod4_de = dmod == 4;
	wire mod5_de = dmod == 5;
	wire mod6_de = dmod == 6;
	wire mod7_de = dmod == 7;
	wire mod8_de = dmod == 8;
	wire mod9_de = dmod == 9;
	wire mod10_de = dmod == 10;

	/* module instantiations */
	/* 0 */ mod_rom		rom_t		(rst, clk, mod0_ie, mod0_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data);
	///* 1 */ mod_ram 	ram_t		(rst, clk, mod1_ie, mod1_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data);
	/* 1 */ mod_sram	ram_t		(rst, clk, mod1_ie, mod1_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data, cpu_stall, mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub, mod_sram_data, mod_sram_addr);
	/* 2 */ mod_uart	uart_t		(rst, clk, mod2_ie, mod2_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data, mod_uart_txd, mod_uart_rxd);
	/* 3 */ mod_switches 	switches_t 	(rst, clk, mod3_ie, mod3_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data, mod_switches_switches);
	/* 4 */ mod_leds	leds_t    	(rst, clk, mod4_ie, mod4_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data, mod_leds_leds);

	/* 8 */ mod_plpid	plpid_t   	(rst, clk, mod8_ie, mod8_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data);
	/* 9 */ mod_timer	timer_t	  	(rst, clk, mod9_ie, mod9_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data);
	/* 10 */ mod_sseg	sseg_t		(rst, clk, mod10_ie, mod10_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, bus_cpu_inst, bus_cpu_data, mod_sseg_an, mod_sseg_display);
endmodule
