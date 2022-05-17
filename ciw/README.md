CallIfWhile
===========

This is a high-level low-level language to target the architecture. It's like the Pascal version of B.

It's not the most useful of things, but it is a lot easier than assembly. As it stands, between the inefficient code and 8K target, one can only write about a 150 lines in the program. The forty line main loop in the BF interpreter is a few operations shy of generating an error message because the loop is too large for how do-loop handles branches. It's almost 2048 instructions.

This "compiler" outputs assembly to be assembled by the assembler. Assemble! Because it outputs assembly, there are some decisions that it doesn't make regarding branch distances that make 2048 instructions the longest branch it will correctly output. Messages from the assembler are probably an indication of bad code generation. Also note that, unlike the assembler, the compiler will die on the first error. Hopefully, the message is useful to you.

The only data type of the compiler is the sixteen-bit word. It will not operate on bytes. Also, the only input number base is ten: there are no binary or hex constants.

# Program
A program consists of three sections, in this order:
* Constants
* Globals
* Functions

## Constants
Constant declarations look like so:
```
   const number1 = 5
   const number2 := 7 << number1
   const number3 <- 1 + 8 * 5 + 1
```
Note that assignment can be achieved with either `=`, `:=`, or `<-`. It will compute the value of the expression before going further (division by zero will probably crash the compiler).

## Globals
Globals look like so:
```
   dim variable1; @200 variable2; variable3 [100]; @40960 screen [2000]
```
They are separated by semi-colons, and there can be as many `dim` lines as one likes. I only put one in the example. The third variable is an array with one hundred elements. Note that only single-dimensional arrays are supported.  
The second and fourth examples may need explanation: they are "homed" variables. The at sign tells the compiler to not allocate memory for the variable, instead expect it at some fixed address. Do not "home" a variable to an odd memory location. It will not be treated correctly. And not just because the underlying architecture doesn't support unaligned reads. As a hack, the compiler puts meaningful information into that bit of a variable's address. Also, do not home a variable to an address less than 128, for the same reason. The compiler is FULL of hacks.  
Note that there is no bounds-checking code output and no bounds-checking is performed. The array length of homed variables is unused, but the declaration tells the compiler to treat the variable as an array. Also, while the compiler will allow you to declare a zero-element array, I'm not sure if it is something I wish to continue allowing.

## Functions
Finally, you have your function declarations. There is a special function called `program` that will be the first function to be executed. As an implementation detail, the compiler compiles functions in alphabetical order. That may be useful should you need to relocate functions to get the assembly to work.
```
   declare toCallLater (args; or; not)

   function program (argument; or; more)
      <variables>
      <statements>
   end function
```
Again, variables need to be declared before any executable statements. Function arguments are word values: arrays cannot be passed to functions (correctly). As functions need to be declared before use, the `declare` for allows you to tell the compiler that a function exists before writing the code. This is useful for organizing code, or making mutually-recursive functions.

### Variables
```
   dim variable1; variable2[10]; static variable3; static variable4[10]; static @ 300 variable5; static @ 302 variable6 [49]
```
These are normal local variables to a function. Static variables are, essentially, global variables that can only be accessed in this function. As global variables, they can be homed.

### Statements
#### Assignment
We finally get to the meat-and-potatoes of executing code. Firstly, there is the ever-imperative assignment statement.
```
   variable1 = 3
   variable2 := 2 << variable1
   variable3[variable2] <- 1 + 8 * 5 + 1
```
It has three forms because ... no reason. Form two was stolen from Pascal, and forms two and three help drive home the concept that one is evaluating an expression and jamming it into a memory location and not making some metaphysical statement about the equivalence of two things.  
Also note that for array `array`, `array := 5` and `array[0] := 5` is the same, for now. Don't depend on it.

#### Call
```
   call function (12; 13; 14)
```
This is just what it advertises: calling a function as a statement.
##### Return
```
   return 15
```
This returns from the current function. A blank `return` is equivalent to `return 0`. Note that the compiler doesn't place an implicit `return` at the end of a function. Forgetting to `return` is a catastrophic runtime error.
##### Tailcall
```
   tailcall 13; 14; 15
```
This is the same as `return function(13;14;15)`, but this doesn't create a new stack frame. The Pong program makes use of this because I wasn't done implementing do-loop when I wrote it. It's an alternative way of looping.

#### If
```
   if <condition> then
      <statements>
   else if <condition> then
      <statements>
   else
      <statements>
   end if

   if <condition> then <statement> else <statement>
```
One can use `fi` instead of `end if`. The `else` is always optional. The single-line form only allows one `then` and one `else` statement.

#### While
##### Do-Loop
```
   : label : do
      <statements>
   loop

   do until <condition>
      <statements>
   loop

   do
      <statements>
   loop while <condition>
```
This is a VERY versatile loop. The condition check can be either `until` or `while` and can be either at the top of the loop, the bottom of the loop, or IN the loop (to be described later). The loop can be labeled so that `break` and `continue` can jump out of nested loops, like Java.
##### Break
```
   break

   break : label :
```
Terminates execution of either the current loop, or the named loop.
##### Continue
```
   continue

   continue : label :
```
Terminates execution of the current iteration of either the current loop, or the named loop. Execution begins at either the top of the loop, before the loop control condition has been evaluated, or at the bottom of the loop, if there is a loop control condition there, before that condition is evaluated. If the loop has two conditions, both are evaluated.
##### While
```
   while <condition>

   while : label : <condition>
```
The easiest way to describe this is syntactic sugar for `if 0 == <condition> then break`. Note that in-loop `while`/`until` statements don't effect the behavior of continue. An example may help:
```
   do
      continue
   loop while 0

   do
      continue
      while 0
   loop
```
The first loop ends immediately. The second loop runs infinitely.
##### Until
```
   until <condition>

   until : label : <condition>
```
The easiest way to describe this is syntactic sugar for `if <condition> then break`. All of the `while` notes apply to `until`.

#### Asm
```
   asm " NOP ; inline assembly "
```
This was tacked-on to help the developer write more efficient code. So, CallIfWhile can provide structure, while assembly actually does the work. For instance, Pong's inefficient screen clearing function is a ten instruction loop that executes two thousand times. It almost doesn't fit between video ram lockouts.

### Expressions

Operator precedence:
* `+ - ! not`
* `* / %`
* `+ -`
* `<< >> >>> <<> >><`
* `>= <= > <`
* `= == <> != #`
* `&& and then`
* `|| or else`
* `& | ? and or xor /\ \/`

The unary `+` operator is absolute value. I've stated my opinions on it elsewhere.  
The `!` and `not` operators are bitwise-not. As such `-1` is used for TRUE and `0` for FALSE. As shown in the `while` description, sometimes `not <expression>` does not do what you expect.  
Bitwise operators: `>>>` unsigned shift right, `<<>` rotate left, and `>><` rotate right.  
The `=` and `==` are both equality, and `!=`, `<>`, and `#` are not equals. Only the relational operators return `-1` for TRUE and `0` for FALSE: boolean operations assume that these are the values used.  
The usage of `and then` and `or else` is stolen from Ada; and, yes, the operators are `and then` and `or else`.  
The `?` is `xor`, and the names of each may be used. Also `/\` is `and` and `\/` is `or`: did you know that these bigraphs were how the meaningless-to-English backslash was introduced to ASCII?

Note that it is generally more efficient to have a constant `const neg_one = -1` than to have a `-1` in your code.

### Extra
#### Comments
```
# MUST begin with a #
     // This is a comment
  (* Pascal-style *)
```
Pascal-style comments don't nest. Not that they do in Pascal, either.

#### Line continuation
```
   if 5 = 5 \
      then
   fi
```
If the last character of a line is `\` then the newline is discarded and the next line continues the current line. It doesn't work to extend a `#` comment.

#### Semicolons
Why do I do something no other language does and use semicolons to separate things? The language is full of "good ideas" from other languages, so what happened here? Consider the following:
```
   call sin(1.25)
   call sin(1,25)
```
DB14's main data types included a floating-point data type. Most programming languages use the comma to separate arguments, and that poses a problem for case two: is it a call to "sin" with two arguments? Or, is it a call to "sin" that tries to use the European decimal symbol? DB14 used the semicolon to separate arguments so that numbers could use the European decimal symbol.

### Memory Map
The initialization routine identity-maps the next four banks of RAM and second bank of ROM. The stack pointer is located at RAM address two. The base pointer is located at RAM address four. Globals start at RAM address 128 and grow up. The stack pointer is initialized to the end of bank five (40958), and grows down. The memory area between the base pointer and globals is not accessible by the language: all addresses in that range are interpreted by the compiler as function argument addresses (ie, relative to the base pointer). You need to write assembly to reach that memory.

Currently, the second bank of ROM is unused, and it will probably be a BAD THING to write a program whose code exceeds that limit. A use is intended, but the assembler needs to be augmented to support it.
