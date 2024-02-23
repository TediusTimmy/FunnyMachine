Technical Description of the Machine Code
=========================================

This is a technical description of an idealized version of the processor. It's intended for an audience that wants to learn more about the particulars of the binary format of machine code instructions, and anyone intending to implement something that is, more-or-less, the same.

Every machine instruction is two bytes, or sixteen bits, or one word.

The instruction is broken into four groups of four bits. Let us label those groups A, B, C, and D, from the least-significant four bits to most significant four bits. Group A is the opcode of the instruction. The interpretation of the other three groups depends on this opcode. Note that, unless otherwise stated, reserved bits should always be zero, and groups not mentioned to be used by an operation are reserved.

Breakdown by Opcode
-------------------

### 0 - NOP Codes

Group B
* 0000 - NOP
* 0010 - INT (can return no values, C is interrupt #, MSB set - NMI, highest or lowest priority NMI is RESET)
* 0011 - RTI
* 0100 - CLI
* 0101 - ENI
* 0110 - WAIT
* 0111 - HALT
* 1111 - ENDOP (C and D not 15 returned to caller belt)

### 1 - Load

Group B is used to control operation, with each bit being a flag to specify some behavior.
* 0001 - This is a word load (the behavior of an unaligned word load is not defined; in the example implementation, the least-significant bit is ignored)
* 0010 - Scratch Load
* 0100 - Reserved
* 1000 - Reserved

Group C is the belt location in which is the address that will be loaded. (For Scratch Load, it is the scratch location.)

This operation pushes the loaded value onto the belt. Loaded bytes are sign extended.

### 2 - Store

Group B is used to control operation, with each bit being a flag to specify some behavior.
* 0001 - This is a word store (the behavior of an unaligned word store is not defined; in the example implementation, the least-significant bit is ignored)
* 0010 - Scratch Store
* 0100 - Reserved
* 1000 - Reserved

Group C is the belt location in which is the value that will be stored.  
Group D is the belt location in which is the address that will be stored to. (For Scratch Store, it is the scratch location.)

This operation does not modify the belt.

### 3 - Load Immediate

Groups B, C, and D are a signed immediate value.

The immediate value is pushed onto the belt.

### 4 - Load Return (PC-Relative) Address

Groups B, C, and D are a signed immediate value that will be multiplied by two then added to the program counter (at the next instruction).

The computed value is pushed onto the belt.

### 5 - Perform an ALU Operation

Groups C and D are the arguments to the operation.  
Group B is the operation to perform:
* 0 - Add
* 1 - Add with Carry In
* 2 - Subtract with Borrow
* 3 - Subtract
* 4 - Logical Shift Right (note : for the shift and rotate operations, only the least-significant four bits of the argument are used in the shift count)
* 5 - Arithmetic Shift Right
* 6 - Logical Shift Left
* 7 - Load High Nibble
* 8 - AND
* 9 - OR
* 10 - XOR
* 11 - Rotate Right
* 12 - NAND
* 13 - NOR
* 14 - XNOR
* 15 - Rotate Left

The computed value is pushed onto the belt.

### 6 - Software-Defined Operation

Machine swaps to a shadow belt and runs until an ENDOP instruction. Up to two arguments: either argument that is not 15 is pushed onto the callee belt.

### 7 - Conditional Branch

Group B is used to control operation. The most significant bit is reserved. The other three bits constitute a condition number, which indicates what condition is to be evaluated (note that the most significant bit logically negates the condition, so there are actually only four conditions that are tested):
* 0 - Zero
* 1 - Negative
* 2 - Odd
* 3 - Positive (Not Zero AND Not Negative)
* 4 - Not Zero
* 5 - Zero OR Positive
* 6 - Even
* 7 - Zero OR Negative

Group C is the belt location in which is the value to check the condition on.  
Group D is the belt location in which is the new program counter (if the condition is true). The least significant bit of this value is discarded and forced to be zero.

This operation does not modify the belt.

### 8 to 15 - Pick

The three least-significant bits of the opcode are the condition number (see the table in Conditional Branch).  
Group B is the belt location in which is the value to check the condition on.  
If the condition is true, then group C becomes the most recent item on the belt. If it is false, then group D does.
