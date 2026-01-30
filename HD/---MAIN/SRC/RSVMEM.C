#define DIVHEADER
#include <STDIO.H>
#include <MSXBIOS.H>
#include <CONIO.H>
#include <IO.H>
#include <STDLIB.H>
#include <PROCESS.H>
#include <BDOSFUNC.H>
/* from PROCESS.C */
char _execgo();

typedef	char	 U8;
typedef	int      S16;
typedef	unsigned U16;
typedef	unsigned uint;
typedef	VOID     void;
typedef BOOL     boolean;

extern void putkb();

#define _F349   (*((size_t *) 0xF349))
#define _F34B   (*((size_t *) 0xF34B))

void fork()
{
    U8      pid;
 	XREG	r;


    printf("fork");
    /* get ID */
    r.bc = ((unsigned)_FORK);
    callxx(BDOS, &r);
	if (r.af >> 8)
    {
        printf("ERROR 0x%04x\n", (r.af >> 8));
		return(ERROR);
    }
    pid = r.bc >> 8;
    printf("pid = 0x%02x\n", (U16) pid);
}


void join0()
{
 	XREG	r;


    printf("join 0");

    r.bc = ((unsigned)_JOIN);
    callxx(BDOS, &r);
	if (r.af >> 8)
    {
        printf("ERROR 0x%04x\n", (r.af >> 8));
        printf("retry join 0\n");
        r.bc = ((unsigned)_JOIN);
        callxx(BDOS, &r);
    }
}

STATUS main(_argc, _argv)
int _argc;
char **_argv;
{
    U16     v;
	XREG	r;


    /* step back 0xF349 and 0xF34B pointers */
    v = _F349;
    printf("0xF349 was 0x%04x\n", v);
    v = v - (U16) 64;
    _F349 = v;
    _F34B = v;
    printf("now 0x%04x\n", v);

    /* clear keyboard buffer*/
    putkb("");

    /* _JOIN with pid 0 */
    join0();

    /* load "MSXDOS2.SYS" at 0x100, set LOAD_FLAG to 0, jp to 0x100*/
    _execgo("MSXDOS2.SYS", (char) 0);

    /* should not get here*/
    return (OK);
}
