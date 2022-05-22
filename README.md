Emulation Project Thing
=======================

Firstly: the files here are licensed under various different terms. See the file you want to know about to find out more. This is really a smorgasbord project of hacks and duct tape.

# History

In early 2021, I had finally heard about the Commander X16 (https://www.commanderx16.com/) and I wanted to make a virtual machine for a similar-style architecture. I had been watching Ben Eater videos (https://eater.net/), and wanted the result to be hardware-realistic. I have also been pursuing the logical implications of Belt architectures (https://millcomputing.com/). So, I wasn't making a 6502-based thing. It was going to have a custom processor, but be situated so that some of the hardware COULD be real.

I worked on that for a couple of months and then stalled out. By then, though, I had the foundation: the memory map (which is inspired by the Commander X16 memory map (which itself is inspired by the memory map for one of Commodore's machines, I think)), the 16 bit data bus with ALL its strangeness, and the eventual graphics processor (TODO : That's currently planned for Emu3).

In December, I watched some of jdah's videos (https://github.com/jdah/jdh-8) and started working on an 8-bit belt machine. I iterated on a few designs, and then decided to incorporate the stuff from earlier in the year into it. What's the point of an 8-bit architecture when fourteen of your sixteen instructions are two bytes anyway. So, I then iterated on some designs that resulted in what you are looking at now. I intend to improve the emulator until it is capable of a more reasonable experience. I think it's still worth showing at the point that it can print out "Hello, World!" and stop.

# What is a belt machine?

What is a RISC machine? Bear with me: I have to agree with Karl Popper here that the best way to answer this question is to look at the problems that are trying to be solved. RISC was developed as a response to overly-complicated "CISC" ISAs. So, they were going to make computers cheaper by making instructions easy to decode. RISC made processor instructions that were trivial for hardware to decode. And the difficulties associated with using those simpler instructions would be swept under the rug by better compilers.

The thing is, with modern hardware, instruction decode isn't that much of a bottleneck. Modern processors are dominated by one thing: determining which instruction to execute next. To be clearer: a program is a sequence of instructions, and we can number each instruction: the hard part in processors today is figuring out what the number of the instruction to execute next is. And there are sub-problems related with this: branch prediction, does an instruction tell me to jump to some other number instead of the next number; and dependency management, does this instruction have all of the data it needs to execute. The second question only makes sense when you realize that modern computers will look several instructions ahead and execute the instructions that it can before they are supposed to be (something that adds to the dependency problem, and something that can only be done because instruction decode isn't that serious of a problem).

The individuals at Mill Computing were looking at that problem when they devised the belt machine. They expose how long it takes to run instructions so that the compiler can know when the dependent data will be present in the result stream (the belt) and can insert an instruction that depends on that result. In addition, the compiler knows and can output all of the simultaneous instructions that the processor can begin in any cycle. (And the difficulties associated with using these instructions will be swept under the rug by better compilers.)

So, just as RISC processors moved complicated sequences of instructions from processors to compilers, belt processors move complicated data dependency from processors to compilers. The Mill computer processor will also address that branch prediction problem, in its own way (but I'm not going to make any use of how they do so: address does not mean solve).

# The Processor

It's a 16 bit RISC-belt processor. The RISC design is for simplicity in decoding, and the belt design allows me to save instruction bits by not having to encode a destination. It has nine instructions, but it's more like forty-six. Some goals: memory-mapped IO, no stack (but able to implement one), no flags register (and no flags bits in the belt), position-independent code (it didn't start like that, but the segmentation of the memory map makes it a good idea), and binary-coded decimal instructions.

I was disinclined to put in a multiply and divide instruction. They're there, but I don't like them. And divide is not implemented to be useful in arbitrary-precision arithmetic. Maybe I'll do that later?
Also, the branch instructions were added at the last minute so that branches wouldn't pollute the belt so much. Using LRA and RET can do everything the branches do, but with better range (at the cost of dropping an intermediate).

### Update

I made a change to the logic: all of the synthetic instructions are now zero-based logic, and the belt has been expanded to 15 entries. The "always -1 constant" has been removed.

## The Instructions:
* No Operation
* Load from memory
* Store to memory
* Load immediate
* Load return address
* Do something with the ALU
* Do something else
* Conditional Return
* Conditional immediate-relative branch

## As Assembly Mnemonics:
* NOP
* LD    - Load Word
* LDB   - Load Byte
* ST    - Store Word
* STB   - Store Byte
* LDI   - Load Immediate (12-bit, sign-extended)
* LRA   - Load Return Address (12-bit, sign-extended)
* ADD   - Add
* ADC   - Add with the carry-in flag set
* SUB   - Subtract
* SBB   - Subtract with the borrow-in flag set
* SHL   - Shift Left
* SHR   - Shift Right
* ASR   - Arithmetic (Signed) Shift Right
* AND   - Logical And
* OR    - Logical Or
* XOR   - Logical Xor
* NAND  - Logical Nand
* NOR   - Logical Nor
* XNOR  - Logical Xnor
* ROL   - Rotate Left
* ROR   - Rotate Right
* BADD  - BCD Add
* BADC  - BCD Add with the carry-in flag set
* BSUB  - BCD Subtract
* BSBB  - BCD Subtract with the borrow-in flag set
* MUL   - Signed Multiply
* UMUL  - Unsigned Multiply
* DIV   - Signed Division
* UDIV  - Unsigned Division
* RETZ  - Return (indirect branch) if Zero
* RETN  - Return if Negative
* RETO  - Return if Odd
* RETP  - Return if Positive (not Zero and not Negative)
* RETC  - Return if not zero
* RETZP - Return if Zero or Positive
* RETE  - Return if Even
* RETZN - Return if Zero or Negative
* BRZ   - Branch if Zero (8-bit, sign-extended, immediate-relative jump)
* BRN   - Branch if Negative
* BRO   - Branch if Odd
* BRP   - Branch if Positive
* BR    - Branch if not zero
* BRZP  - Branch if Zero or Positive
* BRE   - Branch if Even
* BRZN  - Branch if Zero or Negative

## Synthetic Instructions
* DEC   - Decrement
* INC   - Increment
* NEG   - Two's Complement Negation
* NOT   - Logical Not
* BRA   - Branch Always
* RET   - Return Always

## Notes on Instructions:
### LD
Word reads on unaligned addresses are incorrect: the least-significant bit is dropped, and it becomes a word read on the forcibly aligned address.
### ST
Word writes on unaligned addresses are incorrect: the least-significant bit is dropped, and it becomes a word write on the forcibly aligned address.  
The first argument is the value to store. The second argument is the address to store it at. (Write it down before I get it wrong AGAIN.)
### LRA
The address that this is relative to is the next instruction. The program counter has already been incremented when the offset is computed. This was one of the last instructions I added, while I was thinking about what would be needed to implement a stack. At first, I thought that an indirect jump would be enough (what is now RET). Given that ROM is banked, however, one cannot assume the return address. You can't just put the return address on the stack without assuming the bank the code is executing in. So, I made an instruction to compute the return address from a call. At that point, all of the branches became superfluous: LRA could be used with a conditional RET to do all of that, with a better offset range. I didn't want to pollute the belt with a bunch of jump addresses, though, so the branches made a comeback.  
Later note : So far, I have call and return working in CallIfWhile. At this point, I think I made the right opcodes. It's just frustrating to synthesize a stack.
### ADC and SBB
There isn't a flags register. So, instead, these instructions hard-code the carry/borrow in flag. Just use branches and logic to determine if a carry out occurred.
### SHL, SHR, ASR
Only the 4 least significant bits of the shift amount are used. Shifts by negative amounts don't work as you probably think they should.
### MUL and UMUL
This drops the least significant word, then the most significant word onto the belt.
### DIV and UDIV
This drops the quotient and then the remainder onto the belt. If the divisor is zero, then the quotient is zero and the remainder is the dividend.

## Notes on Synthetic Instructions (and How the Belt Works):
At this point, I need to describe how the belt works. This belt is implemented as a FIFO ring buffer. The destination of all operations that generate a result is implicitly the back of the queue. Belt locations are referenced relative to the back of the queue. Belt location 0 is the most recently generated result. This ring buffer retains the last 15 results (0 to 14). Belt location 14 is the least recently generated result (it was first into the queue) and it will be replaced by the next result (it is the first out). Location 15 has a special value: zero. There is a balance between having useful values available, and belt depth, and I'm not sure if I've hit it.
### DEC
This is just subtracting belt location 15 from some other belt location with the borrow in flag set.
### INC
This is just adding belt location 15 to some other belt location with the carry in flag set.
### NEG
This is just subtracting some belt location from belt location 15.
### NOT
This is just NORing belt location 15 with some other belt location.
### BRA
This is a conditional jump if location 15 is zero (and location 15 is hard-coded to be zero). Don't frown at my childish jokes. I did decide that SEX was unnecessary, though I could fit it in with the BCD instructions.
### RET
This is a conditional jump if location 15 is zero.

## Postmortem
This is a list of nice-to-haves. I will collect them here, before either deciding against them or implementing them.  
I think that I have firmly decided against all of these. Changing to zero-based logic and adding a belt location is my last tweak of the ISA.
### LDN
Load nybble. An instruction to compliment LDI that takes a belt location, shifts it left by four, and then uses the second argument slot of the instruction to fill in those four bits.  
Given that the second argument slot is the 4 most-significant bits, it could replace those bits in the argument with those bits in the instruction. Hmm....
### PICK
It will be very hard to incorporate PICK, because it is a three argument conditional operator. It is conditional, so there needs to be a belt location to query. If true, copy the first argument. If false, copy the second argument. This is a Mill conditional operation, and it needs operations like this because of how hard it is to keep the belt consistent. I'd have to drop the branch instructions for PICK. It's a tough call.  
Maybe, move BRCC family into RET, and if the high bit of operation select is set, then argument 2 is sign extended into a 4-bit immediate.... The farthest that Pong branches is 12 instructions....  
Looking at the CallIfWhile compiler: I have three places where BR is used for a short jump and could be replaced, and seven places where pick would actually allow more space efficient code by removing the branches (and not doing weird things with the result count).
### MIN / MAX
An instruction to do this would be nice. Writing conditional code is a pain, because testing for certain conditions changes the belt.
### CBW or CBTW or MOVSX or EXT or SEX
The Pong game doesn't store up/down as -1/1 because it can't sign extend a byte to a word. And where it does store -1/1, it does a store followed by a load to truncate the high bits.
### Rescue
Some instruction that allows me to grab multiple belt entries and retain them or rearrange them. Or, some means of limiting the impact certain ephemeral results: the destination computed for a conditional branch, or a 12-bit immediate that I'm going to immediately extend to 16 bits.

#### Other pie in the sky instructions:
### LDS and STS
Load from and store to scratchpad. Have a 16 register "scratchpad", which are registers. They don't change constantly. Data could be "stored" in them.
### Address belt
If LRA dropped on a separate belt, and RET used that belt for the destination, and I had a load/store for that belt, then I could get rid of BRCC.

# The Memory Map

CPU Memory is broken into eight, 8KB banks:
* 0000-1FFF RAM
* 2000-3FFF RAM
* 4000-5FFF RAM
* 6000-7FFF RAM
* 8000-9FFF RAM
* A000-AFFF VRAM
* B000-BFFF System
* C000-DFFF ROM
* E000-FFFF ROM

Bank 5 is broken into two banks: a 4KB VRAM bank, and a 4KB System bank. Everything that isn't memory-mapped in the System area is backed by RAM bank FF.

There are 256, 8KB banks of RAM that can be selected. Bank FF is special and you should use it with care. There are 256, 8KB banks of ROM. If the ROM read in is less than 1 MB, then the top 1MB of ROM space is RAM. There are 256, 4KB banks of VRAM. When the system starts, all bank registers are zero, so the machine looks as though it has 8KB of RAM mirrored 5 times, 4KB of VRAM, 4KB of System area, and 8KB of ROM mirrored twice.

## The System Area
TODO: put in things as I implement them, aiming to document Emu3.

B000-B008 - Bank registers. Readable and writable, control where the banks are pointing.  
B009 - VRAM bus locked. Reads of VRAM and writes to VRAM while this is 1 will fail.  
B010-B020 - DMA controllers. See description in Emu2.  
B100 - Screen frame : roughly a 1/30 sec timer  
B101 - Screen seconds : rolling count of 30 frame intervals  
BFFF - Writing to this memory location will stop the emulator.

## The VRAM memory map

Currently, there is nothing in VRAM.  
I have a design for the bottom 4KB of VRAM for Emu2 : See Emu2.  
I have a design for the top 512KB of VRAM for Emu3 : See Emu3.

# The emulator(s)

## EMU1

This emulator has memory-mapped IO to output characters to the screen. Outputting characters to BFFE will cause them to be printed to the screen. Also, writing to BFFF will stop the emulation. As of originally committing, it didn't even handle the memory banks correctly, but when I implemented that for the other emulators, this one gained that ability.

## EMU2

This is as finished as I want to make it. It has a more fully featured screen device (80x25 and 16 colors), and a keyboard.

### VRAM
0000-0FA0 : Screen device. This is 80x25, 16 color foreground and background. It is redrawn unconditionally 30 times a second.

### System Area
The halt command is still there. The print character has been removed.

B008-B000 - Bank registers. Readable and writable, control where the banks are pointing.  
B009 - VRAM bus locked. Reads of VRAM and writes to VRAM while this is 1 will fail.  
B010-B020 - DMA controllers. See description.  
B100 - Screen frame : roughly a 1/30 sec timer  
B101 - Screen seconds : rolling count of 30 frame intervals  
BFFC - Keyboard input present (1 if the current character has not been read before, 0 if you've already read it)  
BFFD - Keyboard character (the last character read by the keyboard)

### DMA Controllers
The DMA controllers move banks of data from one bus to another. They move 32 bytes per instruction. When the VRAM bus is locked, they will block until it is unlocked, and then finish. There is no way to transfer less than a complete bank (4K or 8K) of data.
* DMA 1 - ROM to RAM
* DMA 2 - ROM to VRAM
* DMA 3 - RAM to VRAM
* DMA 4 - VRAM to RAM

Each DMA controller has 4 control bytes:
* Control register. This is zero if inactive, one if a transfer is running, and two if a transfer is being initiated. Write a two to this byte to start a transfer.
* Destination Bank.
* Source Bank.
* Bank count. For transfers to/from VRAM, this is a count of 4K banks. For ROM to RAM transfers, this is in 8K banks.

## EMU3

This is in progress. This is planned to have a sprite engine and sound engine. I'm using Javidx9's Pixel Game Engine (https://github.com/OneLoneCoder/olcPixelGameEngine), as I have experience with it.

### System Area
Changes from Emu2:

BFFC - Key pressed request (write the key code of the key you are interested in to this address)  
BFFD - Key pressed result (get result from this address: 1 currently pressed, 0 not pressed)

### VRAM
Bank 128 - 191 : Sprite memory
Bank 192 - 223 : Background memory
Bank 224 - 239 : Palette memory
Bank 240 - 243 : Background 1
Bank 244 - 247 : Background 2
Bank 248 - 251 : Background 3
Bank 252 - 255 : Object Memory (Actually only takes up the first half of Bank 252)

Sprite memory is 4096 tiles, which are 8x8 pixels and 256 colors.  
Background memory is 512 tiles, which are 16x16 and 256 colors.  
Palette memory is 64 palettes of 256 RGBA colors. (Note that color 0 is always transparent, even though it has a palette entry, and the alpha channel is ignored: RGBA just looks better than RGB plus padding.)

Backgrounds:
* 80x60 tiles in two arrays (because I added the other capability last)
* First array is 2 bytes per tile:
  * Low 9 bits select tile
  * High 6 bits select palette
* Second array is at the last bank, 2 bits per tile (left shifts extract data):
  * Vertical mirror
  * Horizontal mirror

Objects:
* 256 objects of 8 bytes
  * X Position (2 bytes, top-left is 0,0 right and down are positive)
  * Y Position (2 bytes)
  * Pattern (2 bytes)
    * 12 bit tile selector
    * 2 bit vertical size (00 - 1 tile, 01 - 2 tiles, 10 - 4 tiles, 11 - 8 tiles)
    * 2 bit horizontal size
  * Attributes (2 bytes, only 10 bits used)
    * Vertical mirror
    * Horizontal mirror
    * Priority (2 bits : 11 - in front, 10 - between BG1 and BG2, 01 - between BG2 and BG3, 00 - behind BG3)
    * Palette (6 bits)
