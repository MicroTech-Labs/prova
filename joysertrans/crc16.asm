uint crc16(byte* data_p, uint length) __naked
{
    //XMODEM CRC calculation
    //https://mdfs.net/Info/Comp/Comms/CRC16.htm
    //"The XMODEM CRC is CRC-16 with a start value of &0000, the end value is not XORed, and uses a polynoimic of 0x1021."

    __asm

    ld b,d
    ld c,e
    ld de,#0

bytelp:
    PUSH BC
    LD A,(HL)         ; Save count, fetch byte from memory

; The following code updates the CRC in DE with the byte in A ---+
    XOR D                     ; XOR byte into CRC top byte
    LD B,#8                   ; Prepare to rotate 8 bits

rotlp:
    SLA E
    ADC A,A             ; Rotate CRC
    JP NC,clear         ; b15 was zero
    LD D,A              ; Put CRC high byte back into D
    LD A,E
    XOR #0x21
    LD E,A              ; CRC=CRC XOR &1021, XMODEM polynomic
    LD A,D
    XOR #0x10           ; And get CRC top byte back into A
clear:
    DEC B               ; Decrement bit counter
    JP NZ,rotlp         ; Loop for 8 bits
    LD D,A              ; Put CRC top byte back into D
; ---------------------------------------------------------------+

    INC HL
    POP BC             ; Step to next byte, get count back
    DEC BC             ; num=num-1
    LD A,B
    OR C
    JP NZ,bytelp  ; Loop until num=0
    RET

    __endasm;
}

