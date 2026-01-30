/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/

#define DIVHEADER
#include <STDIO.H>
#include <MSXBIOS.H>
#define	fnkstr(n)   ((char *)0xf86f+16*(n))
#include <CONIO.H>

#include <ctype.h>

/*
extern char getch();
extern BOOL kbhit();
*/
/* #include <STRING.H> */
extern	char *strcpy();
/* #include <IO.H> */
extern	int	read(), write();

#include <MYTYPES.H>
#include <A:\MAIN\H-LOCAL\JC.H>
#include <A:\MAIN\H-LOCAL\JCIFUNC.H>
#include <A:\MAIN\H-LOCAL\UTILS.H>
extern void opldir();
extern VOID bzero();
#define JIFFY   (*((size_t *) 0xFC9E))
#define SCNCNT  (*((U8 *) 0xF3F6))
/*
    resolution is 4ms
    5 * 4ms ~=  20ms
    32 * 4ms ~= 120ms
*/
#define ECHO_DELAY  32
extern void JCKBD();
extern BOOL KEYPRESS();

/* #define JCTEST_DEBUG */

/* JC parameters */
static U8 speed = 3;    /* 19200 bps */
static U8 port  = 0;    /* port A */
static U8 timing = 0;   /* z80 */
static U8 flowctrl = FC_NONE;
static char *filename = "JC.RX";

/* global vars */
static U8 scnCnt;
static U16 jiffy;
static char saveFK[16*10];
char *FK1 = "\xC9\0";     /* 201 */
char *FK2 = "\xCA\0";     /* 202 */
char *FK3 = "\xCB\0";     /* 203 */
char *FK4 = "\xCC\0";     /* 204 */
char *FK5 = "\xCD\0";     /* 205 */
char *FK6 = "\xCE\0";     /* 206 */

/* max copy & paste length */
#define MAX_CP_LEN     4096
static char cpBuf[MAX_CP_LEN + 1];      /* +1 for terminator */
static char txBuf[2];
static BOOL hexMode = FALSE;

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

            case 'c':
                flowctrl = (*p) & 0x0F;
                break;

            default:
                /* print help */
                printf("arguments:\n");
                printf("-s: 0 = 2400, 1 = 4800, 2 = 9600, 3 = 19200\n");
                printf("-p: 0 = port A, 1 = port B\n");
                printf("-c: 0 = no flow control, 1 = RTS/CTS\n");
                printf("-t: RESERVED, DO NOT USE\n");    /* 0 = z80 @3.58MHz */
                                                        /* 1 = system timer (UNSUPPORTED) */
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

    if (flowctrl > 8)
    {
        printf("unsupported c option %u\n", flowctrl);
        return (ERROR);
    }

    return (OK);
}


void jcLoop()
{
    if (jiffy != JIFFY)
    {
        /* time is good for keyboard scan */
        JCKBD();
        jiffy = JIFFY;
        SCNCNT = 0;
    }
}

void show(_p, _len)
char *_p;
size_t _len;
{
    static char *p;
    static size_t len;
    static U8 c;
    
    p = _p;
    len = _len;
    
    if (len)
    {
        while (len--)
        {
            c = *p++;
            if (hexMode)
            {
                printf("0x%02x ", (U16) c);
            }
            else
            {
                printf("%c", (U16) c);
            }
            jcLoop();            
        }
        fflush(stdout);
    }
}


STATUS test(_jc_handle)
JC_HANDLE _jc_handle;
{
    static JC_HANDLE *jc_handle;
    static U8 c;
    static boolean stayRx;
    static boolean stayTx;    
    static STATUS s;
    static size_t cpLen;
    static size_t len;
    static char *p;
    static U16 start;
    static U16 i;
    static U16 lock;
    static char *pTx;
    static size_t lenTx;


    jc_handle = (JC_HANDLE *) _jc_handle;
    txBuf[1] = (char) 0;

    jcRxTO(jc_handle, (U8) 60, (U8) 2);     /* 60s, 8ms */

    /* no wait for echo */
    jcKSTO(jc_handle, (U8) 0);

    /* ints are enabled */
    /* intsOn = TRUE; */

    pTx = NULL;
    lenTx = 0;

    for (;;)
    {
        /* loop waiting for joyCOM rx */
        stayRx = TRUE;
        while (stayRx)
        {
/*
            if (intsOn)
            {
                intsOn = FALSE;
                lock = jcLock(jc_handle);
            }
*/
            if (lenTx)
            {
                lock = jcLock(jc_handle);

                z0TxNS(pTx, lenTx);
                s = z0Rx(cpBuf, (size_t) MAX_CP_LEN);

                jcUnlock(lock);                
                lenTx = 0;
                pTx = NULL;
            }
            else
            {
                lock = jcLock(jc_handle);

                s = z0Rx(cpBuf, (size_t) MAX_CP_LEN);

                jcUnlock(lock);                
            }

/*
            intsOn = TRUE;
            jcUnlock(lock);
*/
            cpLen = (size_t) MAX_CP_LEN - (size_t) jcRxMiss(jc_handle);

            /* JCTEST_DEBUG */
            /* printf("s=%u cpLen=%u\n", (U16) s, (U16) cpLen); */

            if (s == JC_RXERR)
            {
                /* print good received chars */
                show(cpBuf, cpLen);

                /* print ERROR indication */
                printf("XX");
                fflush(stdout);

                /* return (JC_RXERR); */
            }
            else if (s == JC_TMOUT && cpLen == 0)
            {
                /* timeout and nothing received */

                return (JC_TMOUT);
            }
            else if (s == JC_TMOUT || s == JC_OK)
            {
                /* here cpLen != 0 */
                /* print received chars (1 at a time), if any */
                show(cpBuf, cpLen);
            }
            else if (s == JC_LOCALKBD)
            {
                /* user activity: first of all scan keyboard */
                /* jcLoop(); */
                JCKBD();

                /* here cpLen != 0 */
                /* print received chars (1 at a time), if any */
                show(cpBuf, cpLen);

                /* time to switch to tx */
                stayRx = FALSE;
            }
            else
            {
/* JCTEST_DEBUG */
                printf("UNEXPECTED s=0x%02x cpLen=%u\n", (U16) s, (U16) cpLen);
            }
        }

        stayTx = TRUE;
        while (stayTx)
        {
            if (kbhit())
            {
                c = getch();
                switch (c)
                {
                    case 27:
                        return (JC_LOCALBRK);

                    case 0xC9:
                        printf("RTS ON\n");
                        jcRTS(jc_handle, (BOOL) TRUE);
                        jcKSTO(jc_handle, (U8) ECHO_DELAY);
                        stayTx = FALSE;
                        break;

                    case 0xCA:
                        printf("RTS OFF\n");
                        jcRTS(jc_handle, (BOOL) FALSE);
                        jcKSTO(jc_handle, (U8) ECHO_DELAY);
                        stayTx = FALSE;
                        break;

                    case 0xCB:
                        printf("DTR ON\n");
                        jcDTR(jc_handle, (BOOL) TRUE);
                        jcKSTO(jc_handle, (U8) ECHO_DELAY);
                        stayTx = FALSE;
                        break;

                    case 0xCC:
                        printf("DTR OFF\n");
                        jcDTR(jc_handle, (BOOL) FALSE);
                        jcKSTO(jc_handle, (U8) ECHO_DELAY);
                        stayTx = FALSE;
                        break;
                        
                    case 0xCD:
                        printf("CTS %s\n", jcCTS(jc_handle) ? "ON" : "OFF");
                        printf("DSR %s\n", jcDSR(jc_handle) ? "ON" : "OFF");
/*
                            printf("RTS OFF\n");
                            jcRTS(jc_handle, (BOOL) FALSE);
                            start = JIFFY;
                            while ((JIFFY - start) < 60)
                                jcLoop();
*/
                        jcKSTO(jc_handle, (U8) 0);
                        stayTx = FALSE;
                        break;

                    case 0xCE:
                        hexMode = !hexMode;
                        jcKSTO(jc_handle, (U8) ECHO_DELAY);
                        stayTx = FALSE;
                        break;

                        default:
                        txBuf[0] = c;
                        /* wait for echo */
                        jcKSTO(jc_handle, (U8) ECHO_DELAY);

                        /* time to switch to tx */
                        stayTx = FALSE;
                        pTx = txBuf;
                        lenTx = 1;
/*
                        intsOn = FALSE;
                        lock = jcLock(jc_handle);
                        z0TxNS(txBuf, (size_t) 1);
*/
/*
                        z0Tx(txBuf, (size_t) 1);
*/
                }
            }
            else
            {
                /* some keyboard activity detected, but no char in buffer */
                /* wait for a char until there is some key pressed */

                jcLoop();

                if (! kbhit())
                {
                    /* no char in buffer */                    
/*
                    printf("-");
                    fflush(stdout);
*/
                    if (! KEYPRESS())
                    {
                        /* no key pressed, time to switch to rx */
                        jcKSTO(jc_handle, (U8) 0);
                        stayTx = FALSE;
                    }
                }
            }
        }
    }
    return (OK);
}


STATUS main(_argc, _argv)
int _argc;
char **_argv;
{
    static boolean stayIn;
    static JC_HANDLE jc_handle;
    static JC_PARAM jc_param;
    static char c;    
    static STATUS s;


    printf("JC driver %s\n", jcVer());

    /* check parameters */
    if (OK != GetArgs((int) _argc, _argv))
        return (ERROR);
    if (OK != CheckArgs())
        return (ERROR);

    /* ISR_SCAN disable keyboard and joystick scan in bios isr */
    scnCnt = SCNCNT;
    jiffy = JIFFY;
    SCNCNT = 0;

    /* init and create jc driver handle */
    bzero(&jc_param, (size_t) sizeof(jc_param));
    jc_param.port = port;
    jc_param.speed = speed;
    jc_param.timing = timing;
    jc_param.flowctrl = flowctrl;
    jc_handle = jcInit(&jc_param);
    if (jc_handle == 0)
    {
        SCNCNT = scnCnt;
        return (ERROR);
    }

    printf("jctest 0.0.3\n");

    /* save function key definitions */
    opldir(saveFK, fnkstr(1), sizeof(saveFK));
    strcpy(fnkstr(1), FK1);
    strcpy(fnkstr(2), FK2);
    strcpy(fnkstr(3), FK3);
    strcpy(fnkstr(4), FK4);
    strcpy(fnkstr(5), FK5);    

    s = test(jc_handle);
    printf("test = 0x%02x\n", (U16) s);

    /* restore function key definitions */
    opldir(fnkstr(1), saveFK, sizeof(saveFK));

    /* release resources */
    jcDel(jc_handle);

    /* ISR_SCAN enable keyboard and joystick scan in bios isr */
    SCNCNT = 1;

    return (OK);
}


/* EOF */

