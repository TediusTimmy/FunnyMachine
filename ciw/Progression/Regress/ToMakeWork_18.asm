    ; Program initialization : set SP and BP to 9FFE 
        LDI $9FF
 @bpa   LDI 4
        SHL 1, 0
        LDI $E
 @isp   OR  1, 0
 @spa   LDI 2
        ST  isp, bpa
        ST  isp, spa
    ; Program initialization : identity map RAM and ROM banks 
        LDI $B00
        LDI 4
        SHL 1, 0
        LDI $7
        OR  1, 0
        INC 15
        STB 0, 1 ; B007 <- 1
        SUB 1, 2
        INC 0
        STB 2, 0 ; B001 <- 1
        INC 0
        INC 3
        STB 0, 1 ; B002 <- 2
        INC 1
        INC 1
        STB 0, 1 ; B003 <- 3
        INC 1
        INC 1
        STB 0, 1 ; B004 <- 4
    ; Function call program : return value 0
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Function call program : arguments 0
    ; Function call program : call 0
 @sa    LDI 2
 @sp    LD  0
 @ra    SUB 0, 1
 @bp    SUB 0, 2
 @ba    LDI 4
 @obp   LD  0
        ST  obp, bp
        ST  bp, ba
        LDI 0
 @sp    SUB bp, 0
        ST  sp, sa
        LRA auto_1_ret
        ST  0, ra
        LRA function_program
        RET 0
auto_1_ret:
    ; Program exit : halt machine 
        LDI $BFF
        LDI 4
        SHL 1, 0
        LDI $F
        OR  1, 0
        ST  0, 0
function_program:
    ; Constant 1864 : 6
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 1864
        ST  0, nsp
    ; Assignment [] 
    ; Constant 0 : 6
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Assignment [] to writeL 6
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 2560
        LDI 4
        SHL 1, 0
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LDI 1
        SHL 1, 0
        LD  sp
        ADD 0, 1
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LD  sp
        ST  1, 0
    ; Constant 1893 : 7
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 1893
        ST  0, nsp
    ; Assignment [] 
    ; Constant 1 : 7
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 1
        ST  0, nsp
    ; Assignment [] to writeL 7
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 2560
        LDI 4
        SHL 1, 0
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LDI 1
        SHL 1, 0
        LD  sp
        ADD 0, 1
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LD  sp
        ST  1, 0
    ; Constant 1900 : 8
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 1900
        ST  0, nsp
    ; Assignment [] 
    ; Constant 2 : 8
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 2
        ST  0, nsp
    ; Assignment [] to writeL 8
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 2560
        LDI 4
        SHL 1, 0
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LDI 1
        SHL 1, 0
        LD  sp
        ADD 0, 1
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LD  sp
        ST  1, 0
    ; Constant 1900 : 9
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 1900
        ST  0, nsp
    ; Assignment [] 
    ; Constant 3 : 9
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 3
        ST  0, nsp
    ; Assignment [] to writeL 9
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 2560
        LDI 4
        SHL 1, 0
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LDI 1
        SHL 1, 0
        LD  sp
        ADD 0, 1
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LD  sp
        ST  1, 0
    ; Constant 1903 : 10
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 1903
        ST  0, nsp
    ; Assignment [] 
    ; Constant 4 : 10
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 4
        ST  0, nsp
    ; Assignment [] to writeL 10
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 2560
        LDI 4
        SHL 1, 0
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LDI 1
        SHL 1, 0
        LD  sp
        ADD 0, 1
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  nsp
        LD  sp
        ST  1, 0
    ; TailCall Arguments 11
    ; TailCall 11
        LRA function_program
        RET 0
.assert * <= $1FFE
   .org $1FFE
   .dw  $C000
   .db "Hello", 0, 0
   .dw 72, 101, 108, 111