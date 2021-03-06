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
There are two ways to define a label:
* Normal labels `label:`
* Local labels `@label:`

Note that the `@` won't be part of the label name with local labels. Local
labels are only valid between two normal labels, and can't be used outside of
them. The purpose of local labels is to allow the reuse of common label names
in sections of assembly: for example `loop`. Labels resolve to the relative
location of the label with respect to the next instruction. This is how they
are used in LRA and the BR opcodes.

## Other syntax

### Symbols
Symbols are for implementing named constants in code:
`symbol = value`

Symbols can only reference other symbols in their creation. There isn't the
infrastructure to allow them to depend on labels.

### Assembler directives
`.dw 1234, $1234`
Define word data. Values greater than a word will cause a warning.
`.assert expression`
Evaluate expression and generate an error message should it be zero/false.
Assert expressions only work with labels that are already defined.
`.db "string", 12, 12, "other"`
Define byte data. This data will be word-aligned, and the end of the
directive will be padded to a word. Strings are not zero terminated. Values
greater than a byte will cause a warning.
`.org value`
Move the location of the next output to a specific absolute location.
This location MUST be word aligned, or a warning will be output and the
location rounded towards zero to a word address.  
Org expressions only work with labels that are already defined.

### Expressions
Any result, label, or symbol can be used in expressions that do math.
Also, there is the special "variable" `*`, which represents the current
write location: where the next word will be added to the image.

Operator precedence:
* `` + - ~ < > ` ``
* `* / % << >> >>>`
* `+ -`
* `& | ^`
* `= <> >= <= > <`
* `||`
* `&&`
* `!`

The first line is all unary operators. The `+` operator is absolute value,
because unary plus is dumb. The `<` operator takes the low byte of an
expression, and the `>` operator takes the high byte. This is a 16-bit
architecture. The `` ` `` operator converts a label from the relative
location with respect to the next instruction to the absolute offset within
the current 8KB block. The `~` operator is bit-wise negation, while the `!`
operator is logical negation.

The `>>>` operator is a logical shift right. Both truncate their operand to
sixteen bits first.

Remember that `%10` is ALWAYS "the number 2" and NEVER "mod 10". The space
is necessary `% 10`. Also, div and mod by zero will probably crash the
assembler.

Finally, `&&` and `||` short-circuit. Not that it should matter at this time.
