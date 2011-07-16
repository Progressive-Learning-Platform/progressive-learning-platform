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


module arbiter(rst, clk, cpu_daddr, cpu_bus_data, bus_cpu_data, cpu_drw, cpu_iaddr, bus_cpu_inst, int, int_ack, mod_leds_leds, mod_uart_txd, mod_uart_rxd, mod_switches_switches, mod_sseg_an, mod_sseg_display, cpu_stall, mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub, mod_sram_data, mod_sram_addr, mod_vga_rgb, mod_vga_hs, mod_vga_vs, mod_gpio_gpio, i_button);
	input clk, rst, int_ack;
	output int; 

	/* cpu i/o */
	input [1:0] cpu_drw; /* data read/write. cpu_drw = 00 is nop, 01 is write, 10 is read */
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
	output [7:0] mod_vga_rgb;
	output mod_vga_hs, mod_vga_vs;
	inout [15:0] mod_gpio_gpio;
	input i_button;

	/* inter-module interconnect */
	wire [31:0] mod_vga_sram_addr;
	wire [31:0] mod_vga_sram_data;
	wire mod_vga_sram_read;
	wire mod_vga_sram_rdy;

	/* interrupt interconnect */
	wire i_timer, i_uart;

	/* pmc hardware interconnect */
	wire pmc_cache_miss_I, pmc_cache_miss_D, pmc_cache_access_I, pmc_cache_access_D, pmc_uart_recv, pmc_uart_send;

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
	wire modA_ie = imod == 10;
	wire modB_ie = imod == 11;

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
	wire modA_de = dmod == 10;
	wire modB_de = dmod == 11;

	/* the bus muxes */
	wire [31:0] mod0_inst, mod0_data;
	wire [31:0] mod1_inst, mod1_data;
	wire [31:0] mod2_inst, mod2_data;
	wire [31:0] mod3_inst, mod3_data;
	wire [31:0] mod4_inst, mod4_data;
	wire [31:0] mod5_inst, mod5_data;
	wire [31:0] mod6_inst, mod6_data;
	wire [31:0] mod7_inst, mod7_data;
	wire [31:0] mod8_inst, mod8_data;
	wire [31:0] mod9_inst, mod9_data;
	wire [31:0] modA_inst, modA_data;
	wire [31:0] modB_inst, modB_data;

	assign bus_cpu_inst = 
		mod0_ie ? mod0_inst :
		mod1_ie ? mod1_inst :
		mod2_ie ? mod2_inst :
		mod3_ie ? mod3_inst :
		mod4_ie ? mod4_inst :
		mod5_ie ? mod5_inst :
		mod6_ie ? mod6_inst :
		mod7_ie ? mod7_inst :
		mod8_ie ? mod8_inst :
		mod9_ie ? mod9_inst : 
		modA_ie ? modA_inst : 
		modB_ie ? modB_inst : 0;

	assign bus_cpu_data = 
		mod0_de ? mod0_data :
		mod1_de ? mod1_data :
		mod2_de ? mod2_data :
		mod3_de ? mod3_data :
		mod4_de ? mod4_data :
		mod5_de ? mod5_data :
		mod6_de ? mod6_data :
		mod7_de ? mod7_data :
		mod8_de ? mod8_data :
		mod9_de ? mod9_data : 
		modA_de ? modA_data : 
		modB_de ? modB_data : 0;

	/* module instantiations */
	/* 0 */ mod_rom		rom_t		(rst, clk, mod0_ie, mod0_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod0_inst, mod0_data);
	/* 1 */ mod_memory_hierarchy	ram_t		(rst, clk, mod1_ie, mod1_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod1_inst, mod1_data, cpu_stall, mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub, mod_sram_data, mod_sram_addr, mod_vga_sram_data, mod_vga_sram_addr, mod_vga_sram_read, mod_vga_sram_rdy, pmc_cache_miss_I, pmc_cache_miss_D, pmc_cache_access_I, pmc_cache_access_D);
	/* 2 */ mod_uart	uart_t		(rst, clk, mod2_ie, mod2_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod2_inst, mod2_data, mod_uart_txd, mod_uart_rxd, i_uart, pmc_uart_recv, pmc_uart_send);
	/* 3 */ mod_switches 	switches_t 	(rst, clk, mod3_ie, mod3_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod3_inst, mod3_data, mod_switches_switches);
	/* 4 */ mod_leds	leds_t    	(rst, clk, mod4_ie, mod4_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod4_inst, mod4_data, mod_leds_leds);
	/* 5 */ mod_gpio	gpio_t		(rst, clk, mod5_ie, mod5_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod5_inst, mod5_data, mod_gpio_gpio);
	/* 6 */ mod_vga		vga_t		(rst, clk, mod6_ie, mod6_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod6_inst, mod6_data, mod_vga_rgb, mod_vga_hs, mod_vga_vs, mod_vga_sram_data, mod_vga_sram_addr, mod_vga_sram_read, mod_vga_sram_rdy);
	/* 7 */ mod_plpid	plpid_t   	(rst, clk, mod7_ie, mod7_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod7_inst, mod7_data);
	/* 8 */ mod_timer	timer_t	  	(rst, clk, mod8_ie, mod8_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod8_inst, mod8_data, i_timer);
	/* 9 */ mod_sseg	sseg_t		(rst, clk, mod9_ie, mod9_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, mod9_inst, mod9_data, mod_sseg_an, mod_sseg_display);
	/* 10 */mod_interrupt	interrupt_t	(rst, clk, modA_ie, modA_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, modA_inst, modA_data, int, int_ack, i_timer, i_uart, i_button);
	/* 11 */mod_pmc		pmc_t		(rst, clk, modB_ie, modB_de, ieff_addr, deff_addr, cpu_drw, cpu_bus_data, modB_inst, modB_data, int, pmc_cache_miss_I, pmc_cache_miss_D, pmc_cache_access_I, pmc_cache_access_D, pmc_uart_recv, pmc_uart_send);
endmodule
