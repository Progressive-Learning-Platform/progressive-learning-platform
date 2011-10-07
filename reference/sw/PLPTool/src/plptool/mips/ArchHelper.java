/*
    Copyright 2010-2011 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mips;

/**
 *
 * @author wira
 */
public class ArchHelper {
    public static String getQuickReferenceString() {
        String str = "";

        str += "<a name=\"top\" /><h1>Quick Reference Card</h1>";

        str += "<p>Instructions: <a href=\"#r\">R-type</a> " +
               "<a href=\"#i\">I-type</a> " +
               "<a href=\"#j\">J-type</a> " +
               "</p>";
        str += "<p><a href=\"#p\">Pseudo Instructions</a></p>";
        str += "<p><a href=\"#d\">Assembler Directives</a></p>";
        str += "<p><a href=\"#regs\">Registers Usage</a></p>";
        str += "<p><a href=\"#mmap\">I/O Memory Map</a></p>";

        str += "<a name=\"r\" /><h1>R-type Instructions</h1>";
        str += "<table border=1 width=\"100%\">";

        String[][] data = new String[13][2];

        data[0][0] = "<font color=blue><b>addu</b></font> $rd, $rs, $rt";      data[0][1] = "rd = rs + rt";
        data[1][0] = "<font color=blue><b>subu</b></font> $rd, $rs, $rt";      data[1][1] = "rd = rs - rt";

        data[2][0] = "<font color=blue><b>and</b></font>  $rd, $rs, $rt";      data[2][1] = "rd = rs &amp; rt";
        data[3][0] = "<font color=blue><b>or</b></font>   $rd, $rs, $rt";      data[3][1] = "rd = rs | rt";
        data[4][0] = "<font color=blue><b>nor</b></font>  $rd, $rs, $rt";      data[4][1] = "rd = ~(rs | rt)";
        data[5][0] = "<font color=blue><b>mullo</b></font> $rd, $rs, $rt";     data[5][1] = "rd = (rs * rt) &amp; 0xffffffff";
        data[6][0] = "<font color=blue><b>mulhi</b></font> $rd, $rs, $rt";     data[6][1] = "rd = (rs * rt) &gt;&gt; 32";
        data[7][0] = "<font color=blue><b>slt</b></font>  $rd, $rs, $rt";      data[7][1] = "rd = (rs &lt; rt) ? 1 : 0";
        data[8][0] = "<font color=blue><b>sltu</b></font> $rd, $rs, $rt";      data[8][1] = "rd = (rs &lt; rt) ? 1 : 0";
        data[9][0] = "<font color=blue><b>sll</b></font>  $rd, $rt, shamt";    data[9][1] = "rd = rt &lt;&lt; shamt";
        data[10][0] = "<font color=blue><b>srl</b></font>  $rd, $rt, shamt";   data[10][1] = "rd = rt &gt;&gt; shamt";
        data[11][0] = "<font color=blue><b>jr</b></font> $rs";                 data[11][1] = "PC = rs";
        data[12][0] = "<font color=blue><b>jalr</b></font> $rd, $rs";          data[12][1] = "rd = PC + 4; PC = rs";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"i\" /><h1>I-type Instructions</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[10][2];

        data[0][0] = "<font color=blue><b>addiu</b></font> $rt, $rs, imm";     data[0][1] = "rt = rs + SignExtend(imm)";
        data[1][0] = "<font color=blue><b>andi</b></font>  $rt, $rs, imm";     data[1][1] = "rt = rs & ZeroExtend(imm)";
        data[2][0] = "<font color=blue><b>ori</b></font>   $rt, $rs, imm";     data[2][1] = "rt = rs | ZeroExtend(imm)";
        data[3][0] = "<font color=blue><b>slti</b></font>  $rt, $rs, imm";     data[3][1] = "rt = (rs &lt; SignExtend(imm)) ? 1 : 0";
        data[4][0] = "<font color=blue><b>sltiu</b></font> $rt, $rs, imm";     data[4][1] = "rt = (rs &lt; SignExtend(imm)) ? 1 : 0";
        data[5][0] = "<font color=blue><b>lui</b></font>   $rt, imm";          data[5][1] = "rt = imm &lt;&lt; 16";
        data[6][0] = "<font color=blue><b>lw</b></font>    $rt, imm($rs)";     data[6][1] = "rt = mem[SignExtend(imm) + rs]";
        data[7][0] = "<font color=blue><b>sw</b></font>    $rt, imm($rs)";     data[7][1] = "mem[SignExtend(imm) + rs] = rt";
        data[8][0] = "<font color=blue><b>beq</b></font>   $rt, $rs, label";   data[8][1] = "if (rt == rs) PC = PC + 4 + imm";
        data[9][0] = "<font color=blue><b>bne</b></font>   $rt, $rs, label";   data[9][1] = "if (rt != rs) PC = PC + 4 + imm";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"j\" /><h1>J-type Instructions</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[2][2];

        data[0][0] = "<font color=blue><b>j</b></font>     label";             data[0][1] = "PC = label";
        data[1][0] = "<font color=blue><b>jal</b></font>   label";             data[1][1] = "ra = PC + 4; PC = label";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"p\" /><h1>Pseudo-operations</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[7][2];

        data[0][0] = "<font color=blue><b>nop</b></font>";                     data[0][1] = "sll $0, $0, 0";
        data[1][0] = "<font color=blue><b>b</b></font>     label";             data[1][1] = "beq $0, $0, label";
        data[2][0] = "<font color=blue><b>move</b></font>  $rd, $rs";          data[2][1] = "or  $rd, $0, $rs";
        data[3][0] = "<font color=blue><b>li</b></font>    $rd, imm32";        data[3][1] = "lui $rd, imm32 &gt;&gt; 16<br />";
                                                                               data[3][1] += "ori $rd, $rd, imm32 & 0xffff";
        data[4][0] = "<font color=blue><b>li</b></font>    $rd, label";        data[4][1] = "lui $rd, label[31:16]<br />";
                                                                               data[4][1] += "ori $rd, $rd, label[15:0]";
        data[5][0] = "<font color=blue><b>push</b></font>  $rt";               data[5][1] = "addiu $sp, $sp, -4<br />";
                                                                               data[5][1] += "sw $rt, 4($sp)";
        data[6][0] = "<font color=blue><b>pop</b></font>   $rt";               data[6][1] = "lw $rt, 4($sp)<br />";
                                                                               data[6][1] += "addiu $sp, $sp, 4";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"d\" /><h1>Assembler Directives</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[6][2];

        data[0][0] = ".org <i>address</i>";                data[0][1] = "Place subsequent statements starting from <i>address</i>";
        data[1][0] = "<i>label</i>:";                data[1][1] = "Label current memory location as <i>label</i>";
        data[2][0] = ".word <i>value</i>";           data[2][1] = "Write 32-bit <i>value</i> to the current address";
        data[3][0] = ".ascii \"<i>string</i>\"";     data[3][1] = "Place <i>string</i> starting from the current address";
        data[4][0] = ".asciiz \"<i>string</i>\"";    data[4][1] = "Place null-terminated <i>string</i> starting from the current address";
        data[5][0] = ".space <i>value</i>";          data[5][1] = "Reserve <i>value</i> words starting from the current address";


        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"regs\" /><h1>Registers Usage Guide</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[12][2];

        data[0][0] = "$0, $zero";               data[0][1] = "The zero register";
        data[1][0] = "$1, $at";                 data[1][1] = "Assembler temporary";
        data[2][0] = "$2-$3, $v0-$v1";          data[2][1] = "Return values";
        data[3][0] = "$4-$7, $a0-$a3";          data[3][1] = "Function arguments";
        data[4][0] = "$8-$17, $t0-$t9";         data[4][1] = "Temporaries";
        data[5][0] = "$18-$25, $s0-$s7";        data[5][1] = "Saved temporaries";
        data[6][0] = "$26, $i0";                data[6][1] = "Interrupt vector";
        data[7][0] = "$27, $i1";                data[7][1] = "Interrupt return address";
        data[8][0] = "$28, $gp";                data[8][1] = "Global pointer";
        data[9][0] = "$29, $sp";                data[9][1] = "Stack pointer";
        data[10][0] = "$30, $fp";               data[10][1] = "Frame pointer";
        data[11][0] = "$31, $ra";               data[11][1] = "Return address";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"mmap\" /><h1>I/O Memory Map</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[11][2];

        data[0][0] = "0x00000000";              data[0][1] = "Boot ROM";
        data[1][0] = "0x10000000";              data[1][1] = "RAM";
        data[2][0] = "0xf0000000";              data[2][1] = "UART";
        data[3][0] = "0xf0100000";              data[3][1] = "Switches";
        data[4][0] = "0xf0200000";              data[4][1] = "LEDs";
        data[5][0] = "0xf0300000";              data[5][1] = "GPIO";
        data[6][0] = "0xf0400000";              data[6][1] = "VGA";
        data[7][0] = "0xf0500000";              data[7][1] = "PLPID";
        data[8][0] = "0xf0600000";              data[8][1] = "Timer";
        data[9][0] = "0xf0a00000";              data[9][1] = "Seven Segments";
        data[10][0] = "0xf0700000";             data[10][1] = "Interrupt Controller";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        return str;
    }
}
