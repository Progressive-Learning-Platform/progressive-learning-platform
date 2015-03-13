# Timing Speedup of PLP #

Documentation and research toward speeding up the PLP system to 50MHz (20 ns).


## Timing Research ##
### Nexys 3 ###
The decision has been made that due to small size and routing capabilities that the target board would be moved from the Nexys2 to the Nexys3. The larger space on the Nexys3 is expected to reduce routing delays significantly.

After running a timing report for the Nexys3 with high effort in the place and route phase the design easily meets the previous 50MHz goal even with the forwarding logic and debounce module which both presented problems on the Nexys2.

The design however is unable to run at native clock speed of 100MHz.  In the first 200 of the 2000 failing endpoints the failures are ocuring 50% in the branch calculations, 25% in memory accesses during the MEM stage, and the remaining 25% in the RAM to MEM/WB pipeline. All of these failed endpoints have delays >65% due to routing time.

After working with different constraints it appears that the design cannot meet the goal of 100MHz.


### Nexys 2 ###
The 32-bit x 32-bit multiply in the EX stage seems to be the absolute limiting factor. If speed requirements are to be met it cannot be included in the design in its current form.

Another critical failure point occurs when branch computation must be performed. In order to determine if a branch is taken or not, a subtraction must be performed in the EX stage. The result is then used to determine the PC value in the current IF stage. This cause huge delay since the EX stage must complete before the IF stage can successfully begin. This critical path is as follows:

(EX) Forwarding Logic calculated, ALU inputs are selected, alu\_func is set, alu\_r is calculated, branch control signal is set, (IF) flush signal set, mux to decide what is latched, register latches


Many of the delays also come from the debounce module which should give no timing constaraints as its only signal is reset which is not clock dependent. These delays can safely be ignored.

Memory access paths have a different timing requirement since the signal begins in the CPU on the rising edge and must be in the RAM at the falling edge. This means all memory paths must complete in half of a period.

The failing paths are almost entirely due to routing time. In many cases upwards of 6ns is lost in the arbiter on memory access paths.
