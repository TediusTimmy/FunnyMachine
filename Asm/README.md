The Assembler
=============

Note: this was written in a short amount of time, and it doesn't do a good
job of error handling. I don't doubt that you're able to break it, but, if
used correctly, it should give you correct results.

Usage
-----
`asm <input file> <output file>`

Syntax
------

## Basic Syntax for assembly lines:
[label definition] [result definition] opcode

### Opcode
This is just the mnemonic, from the upper README file. Nothing's different.
Note that addressing is implicit, because each operation only supports one
addressing mode. Places where you would want to put results do not accept
labels, and places where you would want to put labels do not accept results.

### Result definition
Naming results is so very important with this architecture to make sense of
things. The assembly system gives three ways to name the result of an operation:
* `@` - Assign this name to the result, overwriting any previous name.
* `#` - Assign this name to the result, warning if the previous result by this name is still on the belt.
* `%` - Assign this name to the result, warning if the previous result by this name has fallen off of the belt.

The last two are for the programmer to check their understanding of what is going on: they are diagnostic in nature.

### Label definition
There is currently only one way to define labels, and it is not fully
implemented, but two are planned.
* Normal labels `label:`
* Local labels `@label:`

Note that the `@` won't be part of the label name with local labels. Local
labels are only valid between two normal labels, and can't be used outside of
them. The purpose of local labels is to allow the reuse of common label names
in sections of assembly: for example `loop`. Also note that, currently,
labels cannot be referenced before they are defined. It's a one-pass
assembler, and I haven't added that infrastructure.

## Other syntax

### Symbols
Symbols aren't implemented, but are intended to have the syntax
`symbol = value`. They are for implementing named constants in code.

### Assembler directives
There also aren't any assembler directives. I'm not sure what to add. There
definitely needs to be a way to declare data, and possibly conditional
assembly. Assembler directives will have the syntax `.directive and stuff`.