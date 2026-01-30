#ifndef __MSXBIOS_H_
#define __MSXBIOS_H_

typedef unsigned NAT;

/* NOT_USED
typedef struct _regs {
    TINY	f;
    TINY	a;
    NAT	bc;
    NAT	de;
    NAT	hl;
} REGS;
*/
#define	fnkstr(n)   ((char *)0xf86f+16*(n))
/* NOT_USED
#define	msx2()      (*(TINY *)0xfaf8)

VOID    calbio();
VOID    calbas();
VOID    calsub();
VOID    calslt();
VOID    callx();
*/

VOID    screen();
NAT     rnd();
char    chget();
VOID    cls();
VOID    di();
VOID    ei();
VOID    disscr();
VOID    enascr();

/* NOT_USED
TINY    rdslt();
VOID    wrslt();
VOID    inifnk();
VOID    gicini();
VOID    sound();
TINY    rdpsg();
BOOL    chsns();
VOID    chput();
BOOL    lptout();
BOOL    lptstt();
char    *pinlin();
char    *inlin();
TINY    breakx();
VOID    beep();
VOID    locate();
VOID    erafnk();
VOID    dspfnk();
TINY    gtstck();
BOOL    gttrig();
TINY    gtpad();
TINY    gtpdl();
VOID    chgsnd();
TINY    snsmat();
VOID    kilbuf();
*/

#endif


