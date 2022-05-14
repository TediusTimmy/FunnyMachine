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
        LDI 0
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
    ; Constant 13 : 3
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 13
        ST  0, nsp
    ; Assignment to variable 3
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LDI 128
        LD  sp
        ST  0, 1
    ; Variable variable : 4
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 128
        LD  0
        ST  0, nsp
    ; Return 4
 @two   LDI 2
        LD  0
 @rv    LD  0
 @rvl   LDI 4
 @bpa   LDI 4
 @pbp   LD  bpa
        ADD pbp, rvl
        ST  rv, 0
 @bp    ADD pbp, two
        ST  bp, two
 @nbp   LD  pbp
        ST  nbp, bpa
 @ra    ADD pbp, two
        LDI 0
        ADD ra, 0
        ST  0, two
        LD  ra
        RET 0
