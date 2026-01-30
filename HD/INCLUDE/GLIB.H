#ifndef __GLIB_H_
#define __GLIB_H_

/* 
#define	c_dpage		(*(TINY *)0xfaf5)
#define	c_apage		(*(TINY *)0xfaf6)
#define	c_fore		(*(TINY *)0xf3e9)
#define c_back		(*(TINY *)0xf3ea)
#define	c_bord		(*(TINY *)0xf3eb)
#define	c_lastx		(*(NAT  *)0xfcb7)
#define	c_lasty		(*(NAT  *)0xfcb9)
#define c_screen	(*(TINY *)0xfcaf)
#define c_sprite	(*(TINY *)0xf3e0 & 3)
#define c_xmax		(gtxmax())
#define c_ymax		(gtymax())
*/

#define	PSET	(TINY)0

/*
#define AND	(TINY)1
#define OR	(TINY)2
#define XOR	(TINY)3
#define PRESET	(TINY)4
#define TPSET	(TINY)8
#define TAND	(TINY)9
#define TOR	(TINY)10
#define TXOR	(TINY)11
#define TPRESET (TINY)12
*/

VOID    ginit();
VOID	color();
VOID	boxfill();
/*
VOID	boxline();
VOID	line();
VOID	pset();
*/
VOID	setpg();
VOID	inispr();
VOID	cpyv2v();
/*
VOID    setwrt();
VOID    outvdp();
*/

/*
VOID    interlace();
NAT     gtxmax();
NAT     gtymax();
VOID    setrd();
TINY    invdp();
TINY    vpeek();
VOID	vpoke();
VOID	filvrm();
VOID	ldirmv();
VOID	ldirvm();
*/
VOID	wrtvdp();
TINY	rdvdp();
/*
TINY	rdvsts();
VOID	iniplt();
VOID	rstplt();
NAT     getplt();
VOID	setplt();
VOID	circle();
VOID	paint();
TINY	point();
NAT     calpat();
NAT     calatr();
VOID	sprite();
VOID	colspr();
VOID	putspr();
VOID	cpyv2m();
VOID	cpym2v();
VOID	totext();
VOID	grpprt();
VOID	knjprt();
VOID	glocate();
NAT     vramsize();
*/


#endif

