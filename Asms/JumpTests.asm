         LDI jump3
         LDI `jump3
         LRA jump1
         RET 0
         NOP
         NOP
         NOP
jump1:
         BRA jump2
         NOP
         NOP
         NOP
jump2:   NOP
jump3:   NOP


ldi $BFF
add $0, $0
add $0, $0
add $0, $0
add $0, $0
ldi $F
add $0, $1
ldi $48
stb $0, $1
lra $0
retz $f, $0
