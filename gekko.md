# IBM Gekko

![gekko_block](/imgstore/gekko_block.jpg)

## Features

High-performance, superscalar microprocessor:
- As many as four instructions can be fetched from the instruction cache per clock cycle
- As many as two instructions can be dispatched per clock
- As many as six instructions can execute per clock (including two integer instructions)
- Single-clock-cycle execution for most instructions

Six independent execution units and two register files:
- BPU featuring both static and dynamic branch prediction
	- 64-entry (16-set, four-way set-associative) branch target instruction cache (BTIC), a cache of branch instructions that have been encountered in branch/loop code sequences. If a target instruction is in the BTIC, it is fetched into the instruction queue a cycle sooner than it can be made available from the instruction cache. Typically, if a
fetch access hits the BTIC, it provides the first two instructions in the target stream.
	- 512-entry branch history table (BHT) with two bits per entry for four levels of prediction—not-taken, strongly not-taken, taken, strongly taken
	- Branch instructions that do not update the count register (CTR) or link register (LR) are removed from the instruction stream.
- Two integer units (IUs) that share thirty-two GPRs for integer operands
	- IU1 can execute any integer instruction
	- IU2 can execute all integer instructions except multiply and divide instructions (multiply, divide, shift, rotate, arithmetic, and logical instructions). Most instructions that execute in the IU2 take one cycle to execute. The IU2 has a single-entry reservation station
- Three-stage FPU
	- Fully IEEE 754-1985-compliant FPU for both single- and double-precision operations
	- Supports paired single-precision floating point arithematic instruction set extension
	- Supports non-IEEE mode for time-critical operations
	- Hardware support for denormalized numbers
	- Two-entry reservation station
	- Thirty-two 64-bit FPRs for single-, paired single- or double-precision operands.
	- Two-stage LSU
	- Two-entry reservation station
	- Single-cycle, pipelined cache access
	- Dedicated adder performs EA calculations
	- Performs alignment and precision conversion for floating-point data
	- Performs alignment and sign extension for integer data
	- Three-entry store queue
	- Supports both big- and little-endian modes
	- Supports data type conversion with indexed scaling.
- SRU handles miscellaneous instructions
	- Executes CR logical and Move to/Move from SPR instructions (mtspr and mfspr)
	- Single-entry reservation station

Rename buffers:
	- Six GPR rename buffers
	- Six FPR rename buffers
	- Condition register buffering supports two CR writes per clock	

Completion unit:
	- The completion unit retires an instruction from the six-entry reorder buffer (completion queue) when all instructions ahead of it have been completed, the instruction has finished execution, and no exceptions are pending.
	- Guarantees sequential programming model (precise exception model)
	- Monitors all dispatched instructions and retires them in order
	- Tracks unresolved branches and flushes instructions from the mispredicted branch
	- Retires as many as two instructions per clock

Separate on-chip instruction and data caches (Harvard architecture):
	- 32-Kbyte, eight-way set-associative instruction and data caches
	- Pseudo least-recently-used (PLRU) replacement algorithm
	- 32-byte (eight-word) cache block
	- Physically indexed/physical tags. (Note that the PowerPC architecture refers to physical address space as real address space.)
	- Cache write-back or write-through operation programmable on a per-page or per-block basis
	- Instruction cache can provide four instructions per clock; data cache can provide two words per clock
	- Caches can be disabled in software
	- Caches can be locked in software
	- Data cache coherency (MEI) maintained in hardware
	- The critical double word is made available to the requesting unit when it is burst into the line-fill buffer. The cache is nonblocking, so it can be accessed during this operation.
	- Data cache can be partitioned as a four-way 16 Kbyte normal cache and a four-way 16-Kbyte locked cache.

On-chip 1:1 L2 cache:
	- 256 Kbyte on-chip ECC SRAMs
	- On-chip 2-way set-associative tag memory

DMA engine:
	- 15 entry DMA command queue.
	- Each DMA command can transfer up to 4 Kbyte data in 32 byte increment.

Write gather pipe:
	- 128 byte circular FIFO buffer.
	- Non-cacheable stores to a specified address are gathered for burst transaction transfer.

Separate memory management units (MMUs) for instructions and data:
	- 52-bit virtual address; 32-bit physical address
	- Address translation for 4-Kbyte pages, variable-sized blocks, and 256-Mbyte segments
	- Memory programmable as write-back/write-through, cacheable/noncacheable, and coherency enforced/coherency not enforced on a page or block basis
	- Separate IBATs and DBATs (four each) also defined as SPRs
	- Separate instruction and data translation lookaside buffers (TLBs)
	- Both TLBs are 128-entry, two-way set associative, and use LRU replacement algorithm
	- TLBs are hardware-reloadable (that is, the page table search is performed in hardware.

Bus interface features include the following:
	- Selectable bus-to-core clock frequency ratios of 2x, 2.5x, 3x, 3.5x, 4x, 4.5x ... 8x and 10x. (2x to 8x, all half-clock multipliers in-between)
	- A 64-bit, split-transaction external data bus with burst transfers
	- Support for address pipelining and limited out-of-order bus transactions
	- Single-entry load queue
	- Single-entry instruction fetch queue
	- Two-entry L1 cache castout queue
	- No-DRTRY mode eliminates the DRTRY signal from the qualified bus grant. This allows the forwarding of data during load operations to the internal core one bus cycle sooner than if the use of DRTRY is enabled.

Multiprocessing support features include the following:
	- Hardware-enforced, three-state cache coherency protocol (MEI) for data cache.
	- Load/store with reservation instruction pair for atomic memory references, semaphores, and other multiprocessor operations

Power and thermal management:
	- Three static modes, doze, nap, and sleep, progressively reduce power dissipation:
		- Doze—All the functional units are disabled except for the time base/decrementer registers and the bus snooping logic.
		- Nap—The nap mode further reduces power consumption by disabling bus snooping, leaving only the time base register and the PLL in a powered state.
		- Sleep—All internal functional units are disabled, after which external system logic may disable the PLL and SYSCLK.
	- Thermal management facility provides software-controllable thermal management. Thermal management is performed through the use of three supervisor-level registers and an Gekko-specific thermal management exception.
	- Instruction cache throttling provides control of instruction fetching to limit power consumption.

Performance monitor can be used to help debug system designs and improve software efficiency:
	- In-system testability and debugging features through JTAG boundary-scan capability

## Signals

![gekko_signals](/imgstore/gekko_signals.jpg)
