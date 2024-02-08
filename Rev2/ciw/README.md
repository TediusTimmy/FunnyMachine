CallIfWhile
===========

Changes:
* This is a stack implementation that uses SDOs to squeeze the generated source code. Rather than the 8 opcodes per normal operation, it uses 3. However, it takes 13 cycles to execute that same code: there's a 4 cycle penalty for entering (and exiting) two SDOs and 1 cycle because we double load the stack pointer (because they happen in different SDOs).
* Multiplication, division, and modulus are (not yet) implemented in software.
* Fixed `tailcall`
