Technical Description of the Machine Code
=========================================

This is a technical description of an idealized version of the processor. It's intended for an audience that wants to learn more about the particulars of the binary format of machine code instructions, and anyone intending to implement something that is, more-or-less, the same.

Every machine instruction is two bytes, or sixteen bits, or one word.

The instruction is broken into four groups of four bits. Let us label those groups A, B, C, and D, from the least-significant four bits to most significant four bits. Group A is the opcode of the instruction. The interpretation of the other three groups depends on this opcode. Note that, unless otherwise stated, reserved bits should always be zero.

Breakdown by Opcode
-------------------

* 0 - NOP

No operation is performed by this opcode.

* 1 - Load

Group B is used to control operation, with each bit being a flag to specify some behavior.

** 0001 - This is a word load (the behavior of an unaligned word load is not defined; in the example implementation, the least-significant bit is ignored)
** 0010 - Reserved
** 0100 - Reserved
** 1000 - Reserved
Group C is the belt location in which is the address that will be loaded.

This operation pushes the loaded value onto the belt.

* 2 - Store

Group B is used to control operation, with each bit being a flag to specify some behavior.

** 0001 - This is a word store (the behavior of an unaligned word store is not defined; in the example implementation, the least-significant bit is ignored)
** 0010 - Reserved
** 0100 - Reserved
** 1000 - Reserved
Group C is the belt location in which is the value that will be stored.  
Group D is the belt location in which is the address that will be stored to.

This operation does not modify the belt.

* 3 - Load Immediate

Groups B, C, and D are a signed immediate value.

The immediate value is pushed onto the belt.

* 4 - Load Return (PC-Relative) Address

Groups B, C, and D are a signed immediate value that will be multiplied by two then added to the program counter (at the next instruction).

The computed value is pushed onto the belt.

* 5 - Perform an ALU Operation

Groups C and D are the arguments to the operation.  
Group B is the operation to perform:

** 0 - Add
** 1 - Add with Carry In
** 2 - Subtract with Borrow
** 3 - Subtract
** 4 - Logical Shift Left (note : for the shift and rotate operations, only the least-significant four bits of the argument are used in the shift count)
** 5 - Arithmetic Shift Left (yes, this is the same as a logical shift)
** 6 - Logical Shift Right
** 7 - Arithmetic Shift Right
** 8 - AND
** 9 - OR
** 10 - XOR
** 11 - Rotate Left
** 12 - NAND
** 13 - NOR
** 14 - XNOR
** 15 - Rotate Right

The computed value is pushed onto the belt.

* 6 - Implementation Defined

The actual implementation of this operation is left up to the implementer.

The example implementation: Perform Extended ALU Operation  
Groups C and D are the arguments to the operation.  
Group B is the operation to perform, with the most-significant bit reserved:

** 0 - BCD Add
** 1 - BCD Add with Carry In
** 2 - BCD Subtract with Borrow
** 3 - BCD Subtract
** 4 - Signed Multiplication
** 5 - Unsigned Multiplication
** 6 - Signed Division (if the divisor is zero, then the result is 0 with remainder dividend)
** 7 - Unsigned Division

For the example, either one or two values are pushed onto the belt: for BCD operations, the result; for multiplication, the high word followed by the low word; for division, the quotient followed by the remainder.

* 7 - Conditional Branch

Group B is used to control operation. The most significant bit is reserved. The other three bits constitute a condition number, which indicates what condition is to be evaluated (note that the most significant bit logically negates the condition, so there are actually only four conditions that are tested):

** 0 - Zero
** 1 - Negative
** 2 - Odd
** 3 - Positive (Not Zero AND Not Negative)
** 4 - Not Zero
** 5 - Zero OR Positive
** 6 - Even
** 7 - Zero OR Negative

Group C is the belt location in which is the value to check the condition on.  
Group D is the belt location in which is the new program counter (if the condition is true). The least significant bit of this value is discarded and forced to be zero.

This operation does not modify the belt.

* 8 to 15 - Conditional Relative Branch using Immediate

The three least-significant bits of the opcode are the condition number (see the table in Conditional Branch).  
Group B is the belt location in which is the value to check the condition on.  
Groups C and D are a signed immediate value that will be multiplied by two then added to the program counter (at the next instruction) to make the new program counter (if the condition is true).

This operation does not modify the belt.
