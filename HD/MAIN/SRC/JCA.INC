; _JCA_INC_
; ATTENZIONE: DEVE ESSERE COERENTE CON JC.H E JCP.H

JC_OK        equ     0       ; OK DEPENDENT
JC_TMOUT     equ     1
JC_RXERR     equ     2
JC_REMOTEBRK equ     3
JC_LOCALBRK  equ     4
JC_LOCALKBD  equ     5
JC_CANNOT_TX equ     6

;BIT_NONE    equ    0      ; #define FC_NONE      (0)
BIT_RTR_CTS  equ    0      ; #define FC_RTR_CTS   (1)

; JC_DATA DEPENDENT

txDelay     equ     0
rxDelay     equ     txDelay + 1
rxDelayHalf equ     rxDelay + 1
txAndMask   equ     rxDelayHalf + 1
txOrMask    equ     txAndMask + 1
rxAndMask   equ     txOrMask + 1
rxTmOutL1st equ     rxAndMask + 1
rxTmOutH1st equ     rxTmOutL1st + 2

rxTmOutLNxt equ     rxTmOutH1st + 1
rxTmOutHNxt equ     rxTmOutLNxt + 2

rxMissing   equ     rxTmOutHNxt + 1

rtsAndMask  equ     rxMissing + 2
rtsOrMask   equ     rtsAndMask + 1
ctsAndMask  equ     rtsOrMask + 1
dsrAndMask  equ     ctsAndMask + 1
dtrAndMask  equ     dsrAndMask + 1
dtrOrMask   equ     dtrAndMask + 1

disKbdCtr   equ     dtrOrMask + 1
disKbdRld   equ     disKbdCtr + 1

param_ofst  equ     disKbdRld + 1

; JC_PARAM DEPENDENT

port        equ     param_ofst
speed       equ     port + 1
timing      equ     speed + 1
flowctrl    equ     timing + 1


PSG_SEL equ 0A0H
PSG_WR  equ 0A1H
PSG_RD  equ 0A2H
; TOCUT RX      equ  01H
JC_OR_MASK    equ     (040H)
JC_AND_MASK   equ     (NOT JC_OR_MASK)


; ***
;   USE AF
m_capsLedOn macro
	in a,(0AAH)
	and 0BFH
	out (0AAH),a
    endm

; ***
;   USE AF
m_capsLedOff macro
	in a,(0AAH)
	or 040H
	out (0AAH),a
    endm

;   USE A
m_swToRx macro
    ld  a,14
    out (PSG_SEL),a
    endm

;   USE A
m_swToTx macro
    ld  a,15
    out (PSG_SEL),a
    endm

; ***
;   USE AF
m_rtsL macro
    in  a,(PSG_RD)
    and (ix + rtsAndMask)    ; A with rts = 0
    out (PSG_WR),a
    endm

; ***
;   USE AF
m_rtsH macro
    in  a,(PSG_RD)
    or (ix + rtsOrMask)     ; A with rts = 1
    out (PSG_WR),a
    endm

; ***
;   USE AF
m_dtrL macro
    in  a,(PSG_RD)
    and (ix + dtrAndMask)
    out (PSG_WR),a
    endm

; ***
;   USE AF
m_dtrH macro
    in  a,(PSG_RD)
    or (ix + dtrOrMask)
    out (PSG_WR),a
    endm

; EOF

