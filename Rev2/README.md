Instruction Set Revision Two
============================

I finally implemented a revision to the ISA that I've been sitting on for a while now.

## The Instructions:
* No Operation (REALLY!)
* Load from memory
* Store to memory
* Load immediate
* Load return address
* Do something with the ALU
* Software-Defined Operation
* Conditional Return
* Conditional Move

## As Assembly Mnemonics:
* NOP
* INT    - Software invoke Interrupt
* RTI    - Return from Interrupt
* CLI    - Disable Interrupts
* ENI    - Enable Interrupts
* WAIT   - Wait for interrupt
* HALT   - Wait for reset
* INTER  - Interrogate state
* ENDOP  - End of Software-Defined Operation
* LD     - Load Word
* LDB    - Load Byte
* LDS    - Load from Scratchpad
* ST     - Store Word
* STB    - Store Byte
* STS    - Store to Scratchpad
* LDI    - Load Immediate (12-bit, sign-extended)
* LRA    - Load Return Address (12-bit, sign-extended)
* ADD    - Add
* ADC    - Add with the carry-in flag set
* SBB    - Subtract with the borrow-in flag set
* SUB    - Subtract
* SHL    - Shift Left
* SHR    - Shift Right
* ASR    - Arithmetic (Signed) Shift Right
* AND    - Logical And
* OR     - Logical Or
* XOR    - Logical Xor
* NAND   - Logical Nand
* NOR    - Logical Nor
* XNOR   - Logical Xnor
* ROL    - Rotate Left
* LHN    - Load High Nibble (the high nibble of the opcode becomes the high nibble of the belt value)
* SDO0   - Software-defined operation 0
* SDO1   - Software-defined operation 1
* SDO2   - Software-defined operation 2
* SDO3   - Software-defined operation 3
* SDO4   - Software-defined operation 4
* SDO5   - Software-defined operation 5
* SDO6   - Software-defined operation 6
* SDO7   - Software-defined operation 7
* SDO8   - Software-defined operation 8
* SDO9   - Software-defined operation 9
* SDOA   - Software-defined operation 10
* SDOB   - Software-defined operation 11
* SDOC   - Software-defined operation 12
* SDOD   - Software-defined operation 13
* SDOE   - Software-defined operation 14
* SDOF   - Software-defined operation 15
* RETZ   - Return (indirect branch) if Zero
* RETN   - Return if Negative
* RETO   - Return if Odd
* RETP   - Return if Positive (not Zero and not Negative)
* RETC   - Return if not zero
* RETZP  - Return if Zero or Positive
* RETE   - Return if Even
* RETZN  - Return if Zero or Negative
* PICKZ  - Pick if Zero
* PICKN  - Pick if Negative
* PICKO  - Pick if Odd
* PICKP  - Pick if Positive
* PICK   - Pick if not zero
* PICKZP - Pick if Zero or Positive
* PICKE  - Pick if Even
* PICKZN - Pick if Zero or Negative

## Synthetic Instructions
* DEC   - Decrement
* INC   - Increment
* NEG   - Two's Complement Negation
* NOT   - Logical Not
* RET   - Return Always

## New Notes

### SEX
All byte reads are now automatically sign extended, so this operation is not necessary.

### SDOx
The top 32 words of memory are now: 16 interrupt vectors and 16 software-defined operation vectors. When an SDO operation is executed, the machine switches to a shadow belt, drops whichever or both of the two arguments weren't belt location 15, and executes until ENDOP. While in SDO mode, SDO opcodes are ignored. This allows one to program their own opcodes. Call-if-while uses this to have stack-oriented opcodes.

### PICK
Pick takes three arguments: if the first argument is true when evaluated by the PICK opcode's condition, then the second argument is dropped on the belt, else the third. It's basically a conditional move that allows for branchless programming.
