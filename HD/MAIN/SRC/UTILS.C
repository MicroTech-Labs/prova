
/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/

#define DIVHEADER
#include <STDIO.H>
#include <MSXBIOS.H>
#include <MALLOC.H>
#include <IO.H>

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\XM.H>
#include <A:\MAIN\H-LOCAL\XMP.H>

#include <A:\MAIN\H-LOCAL\JC.H>


extern void opldir();
extern void strcpy();


void hexdump(p, s)
char *p;
size_t s;
{
    while (s--)
        printf("%02x  ", (size_t) (*p++));
    fflush(stdout);
}


U8 CS8(_s, _n)
U8 *_s;
size_t _n;
{
    static char *s;
    static size_t n;
    static char c;

    s = _s;
    n = _n;

    c = 0;
    while (n--)
        c += *s++;
    return (c);
}


/*
 * This	function calculates the	CRC used by the	XMODEM/CRC Protocol
 * The first argument is a pointer to the message block.
 * The second argument is the number of	bytes in the message block.
 * The function	returns	an integer which contains the CRC.
 * The low order 16 bits are the coefficients of the CRC.
 */

int calcrc(_ptr, _n)
char *_ptr;
size_t _n;
{
    static char *ptr;
    static size_t n;
    static int crc;
    static U8 i;

    ptr = _ptr;
    n = _n;

    crc = 0;
    while (n--)
    {
        crc = crc ^ (int) *ptr++ << 8;
        for (i = 0; i < 8; ++i)
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
    }
    
    return (crc & 0xFFFF);
}
