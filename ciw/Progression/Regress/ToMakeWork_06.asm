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
        LDI $8
        OR  1, 0
        INC 15
        STB 0, 1 ; B008 <- 1
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
        LDI 2
 @sp    SUB bp, 0
        ST  sp, sa
        LRA auto_1
        ST  0, ra
        LRA function_program
        RET 0
auto_1:
    ; Program exit : halt machine 
        LDI $BFF
        LDI 4
        SHL 1, 0
        LDI $F
        OR  1, 0
        ST  0, 0
function_program:
    ; Constant 40960 : 3
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 2560
        LDI 4
        SHL 1, 0
        ST  0, nsp
    ; Assignment to writeL 3
 @four  LDI 4
 @bp    LD  0
        LDI -2
 @ldr   ADD bp, 0
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        ST  0, ldr
    ; Constant 72 : 4
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 72
        ST  0, nsp
    ; Constant 7 : 4
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 7
        ST  0, nsp
    ; Constant 8 : 4
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 8
        ST  0, nsp
    ; << 4
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        SHL 0, 1
        ST  0, nsp
    ; \/ 4
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        OR  0, 1
        ST  0, nsp
    ; Assignment [] 
    ; Constant 0 : 4
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Assignment [] to writeL 4
 @four  LDI 4
 @bp    LD  0
        LDI -2
 @ldr   ADD bp, 0
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LD  ldr
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
    ; Constant 73 : 5
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 73
        ST  0, nsp
    ; Constant 7 : 5
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 7
        ST  0, nsp
    ; Constant 8 : 5
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 8
        ST  0, nsp
    ; << 5
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        SHL 0, 1
        ST  0, nsp
    ; \/ 5
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        OR  0, 1
        ST  0, nsp
    ; Assignment [] 
    ; Constant 1 : 5
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 1
        ST  0, nsp
    ; Assignment [] to writeL 5
 @four  LDI 4
 @bp    LD  0
        LDI -2
 @ldr   ADD bp, 0
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LD  ldr
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
    ; TailCall Arguments 6
    ; TailCall 6
        LRA function_program
        RET 0