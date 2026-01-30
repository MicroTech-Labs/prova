/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/

#define DIVHEADER
#include <STDIO.H>
#include <MSXBIOS.H>
#include <CONIO.H>

#include <IO.H>
#include <STDLIB.H>
#include <PROCESS.H>
#include <MALLOC.H>

/* #include <STRING.H> */
extern	char	*strcat(), *strcpy(), *strlen();

extern VOID bzero();

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\JC.H>
#include <A:\MAIN\H-LOCAL\XM.H>
#include <A:\MAIN\H-LOCAL\UTILS.H>

extern char getch();
extern BOOL kbhit();
extern int  strcmp();

/* JC parameters */
static U8 speed = 0;    /* 2400 bps */
static U8 port  = 0;    /* port A */
static U8 timing = 0;   /* z80 */


STATUS GetArgs(_argc, _argv)
int _argc;
char **_argv;
{
    static int argc, i;
    static char **argv;
    static char *p;


    argc = _argc;
    argv = _argv;
/*
    for (i = 0; i < argc; i++)
        printf("Param %d = %s\n",i, argv[i]); 
*/
    for (i = 1; i < argc; i++)
    {
        /* arg not null */

        p = argv[i];
        if (*p++ != '-')
        {
            printf("Param %d not an arg\n", i);
            continue;
        }

        switch (*p++)
        {
            case 0x00:
                printf("Param %d too short\n", i);
                continue;

            case 's':
                speed = (*p) & 0x0F;
                break;

            case 'p':
                port = (*p) & 0x0F;
                break;

            case 't':
                timing = (*p) & 0x0F;
                break;

                case 'f':
                xm_filename = p;
                break;

            default:
                /* print help */
                printf("usage JC [-s] [-p] [-t] [-fFILENAME]\n");
                printf("s: 0 = 2400, 1 = 4800, 2 = 9600, 3 = 19200\n");
                printf("p: 0 = port A, 1 = port B\n");
                printf("t: RESERVED, DO NOT USE\n");    /* 0 = z80 @3.58MHz */
                                                        /* 1 = system timer (UNSUPPORTED) */
                printf("f: save rx to FILENAME (default FRX.BIN)\n");

                return (ERROR);
/*
                printf("unknown option\n");
                break;
*/
        }
    }

    return (OK);
}


STATUS CheckArgs()
{
    if (timing >= 2)
    {
        printf("unsupported t option %u\n", timing);
        return (ERROR);
    }

    if (speed >= 4)
    {
        printf("unsupported s option %u\n", speed);
        return (ERROR);
    }

    if (port >= 2)
    {
        printf("unsupported p option %u\n", port);
        return (ERROR);
    }

    return (OK);
}


STATUS jcConIn(_pDev, _pBuf, _maxlen)
JC_HANDLE _pDev;
char *_pBuf;
size_t _maxlen;
{
    static JC_DATA *pDev;
    static char *pBuf;
    static size_t maxlen;
    static U16 lock;
    static U8 c;
    static size_t len;
    static boolean stayIn;
    static STATUS rc;
    static STATUS s;


    pDev = (JC_DATA *) _pDev;
    pBuf = _pBuf;
    maxlen = _maxlen;

    stayIn = TRUE;
    len = 0;
    while (stayIn)
    {
        s = jcRxC(pDev, &c);
        switch (s)
        {
            case OK:

                switch (c)
                {
                    case 0x1B:  /* esc */
                        rc = JC_REMOTEBRK;
                        stayIn = FALSE;
                        break;

                    case 0x0D: /* return */
                        jcTxS(pDev, C_RET);
                        *(pBuf + len) = 0;
                        rc = OK;
                        stayIn = FALSE;
                        break;

                    case 0x08:  /* backspace */
                        if (len)
                        {
                            jcTxS(pDev, C_BS);
                            len--;
                        }
                        break;

                    default:
                        if (len == maxlen)
                            break;

                        jcTxC(pDev, &c);
                        *(pBuf + len) = c;
                        len++;
                        break;
                }
                break;

            case JC_TMOUT:
                /* terminate string */
                *(pBuf + len) = 0;
                stayIn = FALSE;
                rc = JC_TMOUT;
                break;

            default:        /* case JC_LOCALBRK JC_REMOTEBRK */
                stayIn = FALSE;
                rc = s;
                break;
        }
    }

    return (rc);
}



STATUS WaitBuf(_jc_handle, _pBuf, _maxlen)
JC_HANDLE _jc_handle;
char *_pBuf;
size_t _maxlen;
{
    static JC_HANDLE jc_handle;    
    static char *pBuf;
    static size_t maxlen;
    static STATUS s;


    jc_handle = _jc_handle;
    pBuf = _pBuf;
    maxlen = _maxlen;

    printf("waiting...\n");
    s = jcRxBuf(jc_handle, pBuf, maxlen);

    printf("jcRxBuf = 0x%02x\n", (size_t) s);
    printf("rxMissing = %u\n", (size_t) jcRxMiss(jc_handle));
    hexdump((char *) pBuf, maxlen);
    printf("\n");

    return (s);
}


STATUS step(s)
char *s;
{
    printf(s);
    fflush(stdout);
}

#define JIFFY   (*((size_t *) 0xFC9E))
void wait(n)
size_t n;
{
    size_t start = JIFFY;

	while ((JIFFY - start) < n)
        ;
}

STATUS main(_argc, _argv)
int _argc;
char **_argv;
{
    static boolean stayIn;
    static JC_HANDLE jc_handle;
    static JC_PARAM jc_param;
    static XM_HANDLE *pXM;
    static XM_PARAM xmparam;
    static STATUS s, s2;    
    static char cmdBuf[256];
    static char c;    
    
    /* check parameters */
    if (OK != GetArgs((int) _argc, _argv))
        return (ERROR);
    if (OK != CheckArgs())
        return (ERROR);

    /* init and create jc driver handle */
    bzero(&jc_param, (size_t) sizeof(jc_param));

    jc_param.port = port;
    jc_param.speed = speed;
    jc_param.timing = timing;
    jc_handle = jcInit(&jc_param);
    if (jc_handle == 0)
        return (ERROR);

#ifdef DEF
    printf("\njc-srv\n");
    jcTxS(jc_handle, C_RET);
    jcTxS(jc_handle, "jc-srv");
    jcTxS(jc_handle, C_RET);
#endif

#ifdef NOT_DEF
    printf("tx test - ESC to quit\n");
    /* legge tastiera e trasmette tasto premuto */
    stayIn = TRUE;
    while (stayIn)
    {
        while (!kbhit())
            ;
        c = getch();
        if (c == 27)
            break;
        cmdBuf[0] = c;
        cmdBuf[1] = (char) 0;
        jcTxS(jc_handle, cmdBuf);
    }
#endif

#ifdef NOT_DEF
    printf("DEBUG MODE - Rx ESC to quit\n");
    stayIn = TRUE;
    while (stayIn)
    {
        s = jcRxC(jc_handle, &c);
        if (OK == s)
        {
            printf("%c ", (char) c);
            fflush(stdout);
            /* echo */
            jcTxS(jc_handle, &c);

            if (c == 27)
                stayIn = FALSE;

/*
            for (s = 0; s < 8; s++)
            {
                (c & 0x80) ? printf("1") : printf("0");
                c <<= 1;
            }
            printf("\n");
*/
/*            fflush(stdout); */
        }
        else
        {
            printf("s = 0x%02x\n", (size_t) s);
        }
    }
#endif

#ifdef NOT_DEF

    /* legge tastiera e trasmette tasto premuto */
    printf("rx and tx - ESC to quit\n");
    stayIn = TRUE;
    while (stayIn)
    {
        if (OK == jcRxC(jc_handle, &c))
        {
            printf("%c ", (char) c);
            fflush(stdout);
            /* echo */
            jcTxS(jc_handle, &c);

            if (c == 27)
                stayIn = FALSE;
        }
    }
#endif

#ifdef NOT_DEF
    /* get msg from host */
    /* first 3 s, next 1000 ms */
    jcRxTO(jc_handle, (U8) 3, (U8) 250);
    WaitBuf(jc_handle, cmdBuf, (size_t) 128);
#endif

#ifdef NOT_DEF
    /* xmodem rx file */
    printf("file ymodem rx...\n");
    /* first 2 s, next 4 ms */
    jcRxTO(jc_handle, (U8) 2, (U8) 1);
    s = YMRx(jc_handle);
    printf("YMRx() = 0x%02x\n", (U16) s);
#endif

    /* release resources */
    jcDel(jc_handle);

    return (OK);
}


/* EOF */

