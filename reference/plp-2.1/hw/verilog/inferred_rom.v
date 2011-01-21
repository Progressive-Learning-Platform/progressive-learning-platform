
module inferred_rom(clka, clkb, ena, enb, addra, addrb, doa, dob);
        input clka, clkb;
        input ena, enb;
        input [8:0] addra, addrb;
        output reg [31:0] doa, dob;
        reg [31:0] RAM [511:0];

        always @(negedge clka) begin
                if (ena) begin
                        doa <= RAM[addra];
                end
        end

        always @(negedge clkb) begin
                if (enb) begin
                        dob <= RAM[addrb];
                end
        end

        initial begin
		RAM[0] = 32'h080000e8;
		RAM[1] = 32'h00000000;
		RAM[2] = 32'h4f206272;
		RAM[3] = 32'h61766520;
		RAM[4] = 32'h6e657720;
		RAM[5] = 32'h776f726c;
		RAM[6] = 32'h64207468;
		RAM[7] = 32'h61742068;
		RAM[8] = 32'h61732073;
		RAM[9] = 32'h75636820;
		RAM[10] = 32'h70656f70;
		RAM[11] = 32'h6c652069;
		RAM[12] = 32'h6e206974;
		RAM[13] = 32'h2e204c65;
		RAM[14] = 32'h74277320;
		RAM[15] = 32'h73746172;
		RAM[16] = 32'h74206174;
		RAM[17] = 32'h206f6e63;
		RAM[18] = 32'h652e0000;
		RAM[19] = 32'h706c7032;
		RAM[20] = 32'h2e310000;
		RAM[21] = 32'h73746172;
		RAM[22] = 32'h74696e67;
		RAM[23] = 32'h206d656d;
		RAM[24] = 32'h6f727920;
		RAM[25] = 32'h74657374;
		RAM[26] = 32'h2e2e2e00;
		RAM[27] = 32'h646f6e65;
		RAM[28] = 32'h2e000000;
		RAM[29] = 32'h3c08f060;
		RAM[30] = 32'h03e00008;
		RAM[31] = 32'h8d020000;
		RAM[32] = 32'h3c08f060;
		RAM[33] = 32'h03e00008;
		RAM[34] = 32'had040000;
		RAM[35] = 32'h3c08f060;
		RAM[36] = 32'had000000;
		RAM[37] = 32'h8d090000;
		RAM[38] = 32'h0124502a;
		RAM[39] = 32'h140afffd;
		RAM[40] = 32'h00000000;
		RAM[41] = 32'h03e00008;
		RAM[42] = 32'h00000000;
		RAM[43] = 32'h3c08f020;
		RAM[44] = 32'h03e00008;
		RAM[45] = 32'had040000;
		RAM[46] = 32'h3c08f020;
		RAM[47] = 32'h03e00008;
		RAM[48] = 32'h8d020000;
		RAM[49] = 32'h3c08f050;
		RAM[50] = 32'h03e00008;
		RAM[51] = 32'h8d020000;
		RAM[52] = 32'h3c08f050;
		RAM[53] = 32'h03e00008;
		RAM[54] = 32'h8d020004;
		RAM[55] = 32'h3c08f000;
		RAM[56] = 32'h8d090004;
		RAM[57] = 32'h31290002;
		RAM[58] = 32'h1120fffd;
		RAM[59] = 32'h00000000;
		RAM[60] = 32'h8d020008;
		RAM[61] = 32'had090000;
		RAM[62] = 32'h03e00008;
		RAM[63] = 32'h00000000;
		RAM[64] = 32'h3c08f000;
		RAM[65] = 32'h8d090004;
		RAM[66] = 32'h31290001;
		RAM[67] = 32'h1120fffd;
		RAM[68] = 32'h00000000;
		RAM[69] = 32'had04000c;
		RAM[70] = 32'had090000;
		RAM[71] = 32'h03e00008;
		RAM[72] = 32'h00000000;
		RAM[73] = 32'h001fc825;
		RAM[74] = 32'h0004c025;
		RAM[75] = 32'h8f040000;
		RAM[76] = 32'h340800ff;
		RAM[77] = 32'h01044824;
		RAM[78] = 32'h00094e00;
		RAM[79] = 32'h00042202;
		RAM[80] = 32'h01045024;
		RAM[81] = 32'h000a5400;
		RAM[82] = 32'h00042202;
		RAM[83] = 32'h01045824;
		RAM[84] = 32'h000b5a00;
		RAM[85] = 32'h00042202;
		RAM[86] = 32'h01242025;
		RAM[87] = 32'h01442025;
		RAM[88] = 32'h01642025;
		RAM[89] = 32'h10800009;
		RAM[90] = 32'h00000000;
		RAM[91] = 32'h340f0004;
		RAM[92] = 32'h0c000040;
		RAM[93] = 32'h25efffff;
		RAM[94] = 32'h00042202;
		RAM[95] = 32'h1480fffc;
		RAM[96] = 32'h00000000;
		RAM[97] = 32'h11e0ffe9;
		RAM[98] = 32'h27180004;
		RAM[99] = 32'h03200008;
		RAM[100] = 32'h00000000;
		RAM[101] = 32'h001fc025;
		RAM[102] = 32'h00047825;
		RAM[103] = 32'h3c0e0000;
		RAM[104] = 32'h35ce0030;
		RAM[105] = 32'h3c0d0000;
		RAM[106] = 32'h35ad0031;
		RAM[107] = 32'h3c0c8000;
		RAM[108] = 32'h358c0000;
		RAM[109] = 32'h3c0b0000;
		RAM[110] = 32'h356b0021;
		RAM[111] = 32'h01ec4024;
		RAM[112] = 32'h000f7840;
		RAM[113] = 32'h256bffff;
		RAM[114] = 32'h11600005;
		RAM[115] = 32'h00000000;
		RAM[116] = 32'h11000005;
		RAM[117] = 32'h00000000;
		RAM[118] = 32'h08000080;
		RAM[119] = 32'h00000000;
		RAM[120] = 32'h03000008;
		RAM[121] = 32'h00000000;
		RAM[122] = 32'h3c040000;
		RAM[123] = 32'h34840030;
		RAM[124] = 32'h0c000040;
		RAM[125] = 32'h00000000;
		RAM[126] = 32'h0800006f;
		RAM[127] = 32'h00000000;
		RAM[128] = 32'h3c040000;
		RAM[129] = 32'h34840031;
		RAM[130] = 32'h0c000040;
		RAM[131] = 32'h00000000;
		RAM[132] = 32'h0800006f;
		RAM[133] = 32'h00000000;
		RAM[134] = 32'h3c08f010;
		RAM[135] = 32'h03e00008;
		RAM[136] = 32'h8d020000;
		RAM[137] = 32'h03e00008;
		RAM[138] = 32'h340200c0;
		RAM[139] = 32'h03e00008;
		RAM[140] = 32'h340200f9;
		RAM[141] = 32'h03e00008;
		RAM[142] = 32'h340200a4;
		RAM[143] = 32'h03e00008;
		RAM[144] = 32'h340200b0;
		RAM[145] = 32'h03e00008;
		RAM[146] = 32'h34020099;
		RAM[147] = 32'h03e00008;
		RAM[148] = 32'h34020092;
		RAM[149] = 32'h03e00008;
		RAM[150] = 32'h34020082;
		RAM[151] = 32'h03e00008;
		RAM[152] = 32'h340200f8;
		RAM[153] = 32'h03e00008;
		RAM[154] = 32'h34020080;
		RAM[155] = 32'h03e00008;
		RAM[156] = 32'h34020090;
		RAM[157] = 32'h03e00008;
		RAM[158] = 32'h34020088;
		RAM[159] = 32'h03e00008;
		RAM[160] = 32'h34020083;
		RAM[161] = 32'h03e00008;
		RAM[162] = 32'h340200a7;
		RAM[163] = 32'h03e00008;
		RAM[164] = 32'h340200a1;
		RAM[165] = 32'h03e00008;
		RAM[166] = 32'h34020086;
		RAM[167] = 32'h03e00008;
		RAM[168] = 32'h3402008e;
		RAM[169] = 32'h3c08f0a0;
		RAM[170] = 32'h03e00008;
		RAM[171] = 32'had040000;
		RAM[172] = 32'h001f4825;
		RAM[173] = 32'h00045025;
		RAM[174] = 32'h340b00ff;
		RAM[175] = 32'h0c0000c5;
		RAM[176] = 32'h014b2024;
		RAM[177] = 32'h00026025;
		RAM[178] = 32'h000a5202;
		RAM[179] = 32'h0c0000c5;
		RAM[180] = 32'h014b2024;
		RAM[181] = 32'h00026825;
		RAM[182] = 32'h000a5202;
		RAM[183] = 32'h0c0000c5;
		RAM[184] = 32'h014b2024;
		RAM[185] = 32'h00027025;
		RAM[186] = 32'h000a5202;
		RAM[187] = 32'h0c0000c5;
		RAM[188] = 32'h014b2024;
		RAM[189] = 32'h00022600;
		RAM[190] = 32'h000e7400;
		RAM[191] = 32'h000d6a00;
		RAM[192] = 32'h008e2025;
		RAM[193] = 32'h008d2025;
		RAM[194] = 32'h008c2025;
		RAM[195] = 32'h080000a9;
		RAM[196] = 32'h0009f825;
		RAM[197] = 32'h34080000;
		RAM[198] = 32'h1088ffc2;
		RAM[199] = 32'h34080001;
		RAM[200] = 32'h1088ffc2;
		RAM[201] = 32'h34080002;
		RAM[202] = 32'h1088ffc2;
		RAM[203] = 32'h34080003;
		RAM[204] = 32'h1088ffc2;
		RAM[205] = 32'h34080004;
		RAM[206] = 32'h1088ffc2;
		RAM[207] = 32'h34080005;
		RAM[208] = 32'h1088ffc2;
		RAM[209] = 32'h34080006;
		RAM[210] = 32'h1088ffc2;
		RAM[211] = 32'h34080007;
		RAM[212] = 32'h1088ffc2;
		RAM[213] = 32'h34080008;
		RAM[214] = 32'h1088ffc2;
		RAM[215] = 32'h34080009;
		RAM[216] = 32'h1088ffc2;
		RAM[217] = 32'h3408000a;
		RAM[218] = 32'h1088ffc2;
		RAM[219] = 32'h3408000b;
		RAM[220] = 32'h1088ffc2;
		RAM[221] = 32'h3408000c;
		RAM[222] = 32'h1088ffc2;
		RAM[223] = 32'h3408000d;
		RAM[224] = 32'h1088ffc2;
		RAM[225] = 32'h3408000e;
		RAM[226] = 32'h1088ffc2;
		RAM[227] = 32'h3408000f;
		RAM[228] = 32'h1088ffc2;
		RAM[229] = 32'h00000000;
		RAM[230] = 32'h03e00008;
		RAM[231] = 32'h340200ff;
		RAM[232] = 32'h0c000034;
		RAM[233] = 32'h00000000;
		RAM[234] = 32'h00029025;
		RAM[235] = 32'h3c040000;
		RAM[236] = 32'h34840008;
		RAM[237] = 32'h0c000049;
		RAM[238] = 32'h00000000;
		RAM[239] = 32'h3404000d;
		RAM[240] = 32'h0c000040;
		RAM[241] = 32'h00000000;
		RAM[242] = 32'h3404000a;
		RAM[243] = 32'h0c000040;
		RAM[244] = 32'h00000000;
		RAM[245] = 32'h3c048cc7;
		RAM[246] = 32'h34848cff;
		RAM[247] = 32'h0c0000a9;
		RAM[248] = 32'h00000000;
		RAM[249] = 32'h001298c2;
		RAM[250] = 32'h3415ff00;
		RAM[251] = 32'h34140010;
		RAM[252] = 32'h340800ff;
		RAM[253] = 32'h12a8000b;
		RAM[254] = 32'h00000000;
		RAM[255] = 32'h00152025;
		RAM[256] = 32'h0c00002b;
		RAM[257] = 32'h2694ffff;
		RAM[258] = 32'h00132025;
		RAM[259] = 32'h0c000023;
		RAM[260] = 32'h0015a842;
		RAM[261] = 32'h1680fff6;
		RAM[262] = 32'h00000000;
		RAM[263] = 32'h0800010f;
		RAM[264] = 32'h00000000;
		RAM[265] = 32'h3c0424f9;
		RAM[266] = 32'h3484ffff;
		RAM[267] = 32'h0c0000a9;
		RAM[268] = 32'h00000000;
		RAM[269] = 32'h080000ff;
		RAM[270] = 32'h00000000;
		RAM[271] = 32'h0c000086;
		RAM[272] = 32'h34100001;
		RAM[273] = 32'h12020009;
		RAM[274] = 32'h00000000;
		RAM[275] = 32'h34100002;
		RAM[276] = 32'h1202004a;
		RAM[277] = 32'h00000000;
		RAM[278] = 32'h34100004;
		RAM[279] = 32'h1202003f;
		RAM[280] = 32'h00000000;
		RAM[281] = 32'h080000f5;
		RAM[282] = 32'h00000000;
		RAM[283] = 32'h34100061;
		RAM[284] = 32'h34110064;
		RAM[285] = 32'h3412006a;
		RAM[286] = 32'h34130076;
		RAM[287] = 32'h34140066;
		RAM[288] = 32'h0000a825;
		RAM[289] = 32'h0c000037;
		RAM[290] = 32'h00000000;
		RAM[291] = 32'h00022025;
		RAM[292] = 32'h0c00002b;
		RAM[293] = 32'h00000000;
		RAM[294] = 32'h10500017;
		RAM[295] = 32'h1051001d;
		RAM[296] = 32'h10520024;
		RAM[297] = 32'h10530027;
		RAM[298] = 32'h00000000;
		RAM[299] = 32'h08000121;
		RAM[300] = 32'h00000000;
		RAM[301] = 32'h001fb025;
		RAM[302] = 32'h0c000037;
		RAM[303] = 32'h00000000;
		RAM[304] = 32'h0002be00;
		RAM[305] = 32'h0c000037;
		RAM[306] = 32'h00000000;
		RAM[307] = 32'h00024400;
		RAM[308] = 32'h02e8b825;
		RAM[309] = 32'h0c000037;
		RAM[310] = 32'h00000000;
		RAM[311] = 32'h00024200;
		RAM[312] = 32'h02e8b825;
		RAM[313] = 32'h0c000037;
		RAM[314] = 32'h00000000;
		RAM[315] = 32'h02e2b825;
		RAM[316] = 32'h02c00008;
		RAM[317] = 32'h00171025;
		RAM[318] = 32'h0c00012d;
		RAM[319] = 32'h00000000;
		RAM[320] = 32'h0002a825;
		RAM[321] = 32'h0c000040;
		RAM[322] = 32'h00142025;
		RAM[323] = 32'h08000121;
		RAM[324] = 32'h00000000;
		RAM[325] = 32'h0c00012d;
		RAM[326] = 32'h00000000;
		RAM[327] = 32'haea20000;
		RAM[328] = 32'h26b50004;
		RAM[329] = 32'h0c000040;
		RAM[330] = 32'h00142025;
		RAM[331] = 32'h08000121;
		RAM[332] = 32'h00000000;
		RAM[333] = 32'h0c000040;
		RAM[334] = 32'h00142025;
		RAM[335] = 32'h02a00008;
		RAM[336] = 32'h00000000;
		RAM[337] = 32'h3c040000;
		RAM[338] = 32'h3484004c;
		RAM[339] = 32'h0c000049;
		RAM[340] = 32'h00000000;
		RAM[341] = 32'h08000121;
		RAM[342] = 32'h00000000;
		RAM[343] = 32'h3c081000;
		RAM[344] = 32'h35080000;
		RAM[345] = 32'h3c090000;
		RAM[346] = 32'h35290001;
		RAM[347] = 32'h3c0af040;
		RAM[348] = 32'h354a0000;
		RAM[349] = 32'had480004;
		RAM[350] = 32'had490000;
		RAM[351] = 32'h3c040000;
		RAM[352] = 32'h34840054;
		RAM[353] = 32'h0c000049;
		RAM[354] = 32'h00000000;
		RAM[355] = 32'h0c000040;
		RAM[356] = 32'h3404000d;
		RAM[357] = 32'h0c000040;
		RAM[358] = 32'h3404000a;
		RAM[359] = 32'h3c101000;
		RAM[360] = 32'h36100000;
		RAM[361] = 32'h3c111100;
		RAM[362] = 32'h36310000;
		RAM[363] = 32'h3c12dead;
		RAM[364] = 32'h3652beef;
		RAM[365] = 32'hae100000;
		RAM[366] = 32'h26100004;
		RAM[367] = 32'h1611fffd;
		RAM[368] = 32'h00000000;
		RAM[369] = 32'h3c101000;
		RAM[370] = 32'h36100000;
		RAM[371] = 32'h8e130000;
		RAM[372] = 32'h1613000a;
		RAM[373] = 32'h00000000;
		RAM[374] = 32'h26100004;
		RAM[375] = 32'h1611fffb;
		RAM[376] = 32'h00000000;
		RAM[377] = 32'h3c040000;
		RAM[378] = 32'h3484006c;
		RAM[379] = 32'h0c000049;
		RAM[380] = 32'h00000000;
		RAM[381] = 32'h0800018b;
		RAM[382] = 32'h00000000;
		RAM[383] = 32'h0c000065;
		RAM[384] = 32'h00102025;
		RAM[385] = 32'h0c000040;
		RAM[386] = 32'h3404003a;
		RAM[387] = 32'h0c000065;
		RAM[388] = 32'h00132025;
		RAM[389] = 32'h0c000040;
		RAM[390] = 32'h3404000d;
		RAM[391] = 32'h0c000040;
		RAM[392] = 32'h3404000a;
		RAM[393] = 32'h08000176;
		RAM[394] = 32'h00000000;
		RAM[395] = 32'h0800018b;
		RAM[396] = 32'h00000000;
		RAM[397] = 32'h00000000;
		RAM[398] = 32'h00000000;
		RAM[399] = 32'h00000000;
		RAM[400] = 32'h00000000;
		RAM[401] = 32'h00000000;
		RAM[402] = 32'h00000000;
		RAM[403] = 32'h00000000;
		RAM[404] = 32'h00000000;
		RAM[405] = 32'h00000000;
		RAM[406] = 32'h00000000;
		RAM[407] = 32'h00000000;
		RAM[408] = 32'h00000000;
		RAM[409] = 32'h00000000;
		RAM[410] = 32'h00000000;
		RAM[411] = 32'h00000000;
		RAM[412] = 32'h00000000;
		RAM[413] = 32'h00000000;
		RAM[414] = 32'h00000000;
		RAM[415] = 32'h00000000;
		RAM[416] = 32'h00000000;
		RAM[417] = 32'h00000000;
		RAM[418] = 32'h00000000;
		RAM[419] = 32'h00000000;
		RAM[420] = 32'h00000000;
		RAM[421] = 32'h00000000;
		RAM[422] = 32'h00000000;
		RAM[423] = 32'h00000000;
		RAM[424] = 32'h00000000;
		RAM[425] = 32'h00000000;
		RAM[426] = 32'h00000000;
		RAM[427] = 32'h00000000;
		RAM[428] = 32'h00000000;
		RAM[429] = 32'h00000000;
		RAM[430] = 32'h00000000;
		RAM[431] = 32'h00000000;
		RAM[432] = 32'h00000000;
		RAM[433] = 32'h00000000;
		RAM[434] = 32'h00000000;
		RAM[435] = 32'h00000000;
		RAM[436] = 32'h00000000;
		RAM[437] = 32'h00000000;
		RAM[438] = 32'h00000000;
		RAM[439] = 32'h00000000;
		RAM[440] = 32'h00000000;
		RAM[441] = 32'h00000000;
		RAM[442] = 32'h00000000;
		RAM[443] = 32'h00000000;
		RAM[444] = 32'h00000000;
		RAM[445] = 32'h00000000;
		RAM[446] = 32'h00000000;
		RAM[447] = 32'h00000000;
		RAM[448] = 32'h00000000;
		RAM[449] = 32'h00000000;
		RAM[450] = 32'h00000000;
		RAM[451] = 32'h00000000;
		RAM[452] = 32'h00000000;
		RAM[453] = 32'h00000000;
		RAM[454] = 32'h00000000;
		RAM[455] = 32'h00000000;
		RAM[456] = 32'h00000000;
		RAM[457] = 32'h00000000;
		RAM[458] = 32'h00000000;
		RAM[459] = 32'h00000000;
		RAM[460] = 32'h00000000;
		RAM[461] = 32'h00000000;
		RAM[462] = 32'h00000000;
		RAM[463] = 32'h00000000;
		RAM[464] = 32'h00000000;
		RAM[465] = 32'h00000000;
		RAM[466] = 32'h00000000;
		RAM[467] = 32'h00000000;
		RAM[468] = 32'h00000000;
		RAM[469] = 32'h00000000;
		RAM[470] = 32'h00000000;
		RAM[471] = 32'h00000000;
		RAM[472] = 32'h00000000;
		RAM[473] = 32'h00000000;
		RAM[474] = 32'h00000000;
		RAM[475] = 32'h00000000;
		RAM[476] = 32'h00000000;
		RAM[477] = 32'h00000000;
		RAM[478] = 32'h00000000;
		RAM[479] = 32'h00000000;
		RAM[480] = 32'h00000000;
		RAM[481] = 32'h00000000;
		RAM[482] = 32'h00000000;
		RAM[483] = 32'h00000000;
		RAM[484] = 32'h00000000;
		RAM[485] = 32'h00000000;
		RAM[486] = 32'h00000000;
		RAM[487] = 32'h00000000;
		RAM[488] = 32'h00000000;
		RAM[489] = 32'h00000000;
		RAM[490] = 32'h00000000;
		RAM[491] = 32'h00000000;
		RAM[492] = 32'h00000000;
		RAM[493] = 32'h00000000;
		RAM[494] = 32'h00000000;
		RAM[495] = 32'h00000000;
		RAM[496] = 32'h00000000;
		RAM[497] = 32'h00000000;
		RAM[498] = 32'h00000000;
		RAM[499] = 32'h00000000;
		RAM[500] = 32'h00000000;
		RAM[501] = 32'h00000000;
		RAM[502] = 32'h00000000;
		RAM[503] = 32'h00000000;
		RAM[504] = 32'h00000000;
		RAM[505] = 32'h00000000;
		RAM[506] = 32'h00000000;
		RAM[507] = 32'h00000000;
		RAM[508] = 32'h00000000;
		RAM[509] = 32'h00000000;
		RAM[510] = 32'h00000000;
		RAM[511] = 32'h00000000;

	end
endmodule
