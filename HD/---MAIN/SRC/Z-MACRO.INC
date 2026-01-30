DELAY_TX macro
    ld  b,c     ; reload delay counter
    djnz $
    endm

DELAY_RX macro
    ld  b,c
    djnz $
    endm

DELAY_RX_HALF macro
    ld  b,d
    djnz $
    endm

LD_IYL_N macro NUM
	; ld iyl,n
	defb 0FDh
	ld l,NUM
    endm

OR_IYL macro
    defb 0FDh
    or l
    endm

DEC_IYL macro
    defb 0FDh
    dec l
    endm

; EOF
