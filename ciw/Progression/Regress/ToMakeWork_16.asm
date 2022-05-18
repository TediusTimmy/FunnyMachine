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
function_decider:
    ; Variable arg : 2
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 4
        LD  0
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 4
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        ADD 0, 1
        LD  0
        ST  0, nsp
    ; Constant 0 : 2
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; < 2
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        SUB 0, 1
        BRN 0, 4
        ADD 15, 15
        BRA 2
        NOT 15
        ST  0, nsp - 1
    ; If 2
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LRA auto_2_else
        RETZ 1, 0
    ; Constant 127 : 3
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 127
        ST  0, nsp
    ; Return 3
 @two   LDI 2
        LD  0
 @rv    LD  0
 @rvl   LDI 6
 @bpa   LDI 4
 @pbp   LD  bpa
        ADD pbp, rvl
        ST  rv, 0
 @bp    ADD pbp, two
        ST  bp, two
 @nbp   LD  pbp
        ST  nbp, bpa
 @ra    ADD pbp, two
        LDI 4
        ADD ra, 0
        ST  0, two
        LD  ra
        RET 0
        LRA auto_3_fi
        RET 0
auto_2_else:
    ; Variable arg : 4
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 4
        LD  0
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 4
        ST  0, nsp
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        ADD 0, 1
        LD  0
        ST  0, nsp
    ; Constant 0 : 4
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; > 4
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LD  nsp
        SUB 0, 1
        BRP 0, 4
        ADD 15, 15
        BRA 2
        NOT 15
        ST  0, nsp - 1
    ; If 0
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
        LD  sp
        LRA auto_4_else
        RETZ 1, 0
    ; Constant -127 : 5
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI -127
        ST  0, nsp
    ; Return 5
 @two   LDI 2
        LD  0
 @rv    LD  0
 @rvl   LDI 6
 @bpa   LDI 4
 @pbp   LD  bpa
        ADD pbp, rvl
        ST  rv, 0
 @bp    ADD pbp, two
        ST  bp, two
 @nbp   LD  pbp
        ST  nbp, bpa
 @ra    ADD pbp, two
        LDI 4
        ADD ra, 0
        ST  0, two
        LD  ra
        RET 0
        LRA auto_5_fi
        RET 0
auto_4_else:
    ; Constant 0 : 7
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Return 7
 @two   LDI 2
        LD  0
 @rv    LD  0
 @rvl   LDI 6
 @bpa   LDI 4
 @pbp   LD  bpa
        ADD pbp, rvl
        ST  rv, 0
 @bp    ADD pbp, two
        ST  bp, two
 @nbp   LD  pbp
        ST  nbp, bpa
 @ra    ADD pbp, two
        LDI 4
        ADD ra, 0
        ST  0, two
        LD  ra
        RET 0
auto_5_fi:
auto_3_fi:
function_program:
    ; Call 12
    ; Function call decider : return value 12
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Function call decider : arguments 12
    ; Constant 0 : 12
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Function call decider : call 12
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
        LRA auto_6_ret
        ST  0, ra
        LRA function_decider
        RET 0
auto_6_ret:
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
    ; Call 13
    ; Function call decider : return value 13
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Function call decider : arguments 13
    ; Constant 12 : 13
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 12
        ST  0, nsp
    ; Function call decider : call 13
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
        LRA auto_7_ret
        ST  0, ra
        LRA function_decider
        RET 0
auto_7_ret:
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
    ; Call 14
    ; Function call decider : return value 14
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Function call decider : arguments 14
    ; Constant -12 : 14
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI -12
        ST  0, nsp
    ; Function call decider : call 14
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
        LRA auto_8_ret
        ST  0, ra
        LRA function_decider
        RET 0
auto_8_ret:
 @two   LDI 2
 @sp    LD  0
 @nsp   ADD sp, two
        ST  nsp, two
    ; Constant 0 : 15
 @two   LDI 2
 @sp    LD  0
 @nsp   SUB sp, two
        ST  nsp, two
        LDI 0
        ST  0, nsp
    ; Return 15
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
        LDI 2
        ADD ra, 0
        ST  0, two
        LD  ra
        RET 0
