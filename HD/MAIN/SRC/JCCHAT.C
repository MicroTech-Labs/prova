/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/

#define DIVHEADER
#include <STDIO.H>
#include <MSXBIOS.H>
#include <CONIO.H>
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
extern VOID bzero();
#define JIFFY   (*((size_t *) 0xFC9E))
#define SCNCNT  (*((U8 *) 0xF3F6))

extern void JCKBD();

/* #define JCCHAT_DEBUG */

/* JC parameters */
static U8 speed = 3;    /* 19200 bps */
static U8 port  = 0;    /* port A */
static U8 timing = 0;   /* z80 */
static U8 flowctrl = FC_NONE;
static char *filename = "JC.RX";

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


/*
    OUT A = 0 when no key is pressed
        A = 1 when any key is pressed    
*/
BOOL kc()
{
    static U8 *pNewKey;
    static U8 r;


    for (r = 0, pNewKey = (U8 *) 0xFBE5; r < 11; r++, pNewKey++)
        if (*pNewKey != 0xFF)
            return TRUE;
    return FALSE;
}


/* max copy & paste length */
#define MAX_CP_LEN     1024

STATUS chat(_jc_handle)
JC_HANDLE _jc_handle;
{
    static JC_HANDLE *jc_handle;
    static U8 c;
    static size_t len;
    static boolean stayIn;
    static STATUS rc;
    static STATUS s;
    static size_t cpLen;
    static char cpBuf[MAX_CP_LEN];
    static char *p;
    static char txBuf[2];
    static U16 scnCnt;
    static U16 start;
    static U16 i;
    static boolean ktest;
    static U8 kkey;


    jc_handle = (JC_HANDLE *) _jc_handle;
    txBuf[1] = (char) 0;


    for (;;)
    {
        /* loop waiting for joyCOM rx */
        stayIn = TRUE;
        while (stayIn)
        {
            s = jcRxBuf(jc_handle, cpBuf, (size_t) MAX_CP_LEN);
            cpLen = (size_t) MAX_CP_LEN - (size_t) jcRxMiss(jc_handle);

#ifdef JCCHAT_DEBUG
            printf("jcRxBuf = 0x%02x cpLen = %u\n", (U16) s, (U16) cpLen);
#endif
            if (s == JC_RXERR)
            {
                /* rx error, at the moment drop any received byte */
                return (JC_RXERR);
            }
            else if (s == JC_TMOUT && cpLen == 0)
            {
                /* timeout and nothing received */
                return (JC_TMOUT);
            }
            else
            {
                /* here cpLen != 0 (s may be JC_TMOUT or JC_OK or JC_LOCALKBD) */

                /* print received chars (1 at a time), if any */
                p = cpBuf;
                while (cpLen--)
                {
                    c = *p++;
/*                    printf("0x%02x ", (U16) c); */
                    printf("%c", (U16) c);
                }
/*                printf("\n"); */
                fflush(stdout);

                /* waitkey */
#ifdef JCCHAT_DEBUG
                while (!kbhit())
                    ;
                c = getch();
#endif
                if (s == JC_LOCALKBD)
                {
                    /* user keyboard activity */
                    stayIn = FALSE;
                }
            }
        }

        /* loop waiting for kbd */
        do
        {
            if (kbhit())
            {
                c = getch();
                switch (c)
                {
                    case 27:
                        return (JC_LOCALBRK);

                    case '1':

                        scnCnt = SCNCNT;

                        for (i = 0; i < 5; i++)
                        {
                            /* disable keyboard and joystick scan in bios isr */
                            SCNCNT=0;
                            printf("DTR ON\n");                            
                            jcDTR(jc_handle, (BOOL) TRUE);
                            start = JIFFY;
                            while ((JIFFY - start) < 60)
                                ;

                            SCNCNT=0;
                            printf("DTR OFF\n");
                            jcDTR(jc_handle, (BOOL) FALSE);
                            start = JIFFY;
                            while ((JIFFY - start) < 60)
                                ;
                        }
                        /* restore scan in bios isr */
                        SCNCNT = scnCnt;

                        break;

                    case '2':

                        scnCnt = SCNCNT;
                        /* disable keyboard and joystick scan in bios isr */
                        SCNCNT=0;
                        ktest = TRUE;
                        while (ktest)
                        {
                            start = JIFFY;
                            while (JIFFY == start)
                                ;
                            JCKBD();
                            SCNCNT=0;
                            if (kbhit())
                            {
                                kkey = getch();
                                printf("%c", (U16) kkey);
                                fflush(stdout);
                                if (kkey == '0')
                                    ktest = FALSE;
                            }
                        }
                        /* restore scan in bios isr */
                        SCNCNT = scnCnt;

                        break;
                }


/*                printf("tx 0x%02x", (U16) c); */
/*
                printf("%c", (U16) c);
                fflush(stdout);
*/
                txBuf[0] = c;
                jcTxS(jc_handle, txBuf);
            }
        }
        while (kc());
    }

    return (rc);
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
    jc_param.flowctrl = flowctrl;
    jc_handle = jcInit(&jc_param);
    if (jc_handle == 0)
        return (ERROR);

    printf("jcchat\n");

    jcRxTO(jc_handle, (U8) 60, (U8) 1);

    s = chat(jc_handle);
    printf("chat = 0x%02x\n", (U16) s);

    /* release resources */
    jcDel(jc_handle);

    return (OK);
}


/* EOF */

