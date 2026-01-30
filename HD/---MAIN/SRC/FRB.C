/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/

#define DIVHEADER
#include <STDIO.H>
#include <MSXBIOS.H>
#include <CONIO.H>

extern VOID bzero();

/* #include <STRING.H> */
extern	char *strcpy();

/* #include <IO.H> */
extern	int	read(), write();

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\JC.H>
#include <A:\MAIN\H-LOCAL\JCIFUNC.H>

#include <A:\MAIN\H-LOCAL\UTILS.H>

extern char getch();
extern BOOL kbhit();

/* #define FRB_DEBUG */

/* JC parameters */
static U8 speed = 3;    /* 19200 bps */
static U8 port  = 0;    /* port A */
static U8 timing = 0;   /* z80 */
static U8 flowctrl = FC_NONE;
/* FRB parameters */
static char *filename = "FRB.DAT";

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
                filename = p;
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
                printf("-f<filename>: default FRB.DAT\n");                
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


FILE *fileOpen(_fn)
char *_fn;
{
    char f[128+1];   /* nome file in pagina 3 */
    char m[2+1];     /* mode in pagina 3 */
    FILE *fp;


    strcpy(f, _fn);
    strcpy(m, "WB");
    fp = fopen(f, m);
    if (fp == NULL)
    {
        printf("ERROR: cann't open %s\n", _fn);
    }
    return (fp);
}


/* max copy & paste length */
#define MAX_CP_LEN     1024

STATUS frb(_jc_handle)
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
    static size_t rxTotal;
    static FILE *fp;


    jc_handle = (JC_HANDLE *) _jc_handle;
    txBuf[1] = (char) 0;
    rxTotal = 0;

    if (NULL == (fp = fileOpen(filename)))
        return (ERROR);

    for (;;)
    {
        /* loop waiting or joyCom rx */
        stayIn = TRUE;
        while (stayIn)
        {
            s = jcRxBuf(jc_handle, cpBuf, (size_t) MAX_CP_LEN);
            cpLen = (size_t) MAX_CP_LEN - (size_t) jcRxMiss(jc_handle);

#ifdef FRB_DEBUG
            printf("jcRxBuf = 0x%02x cpLen = %u\n", (U16) s, (U16) cpLen);
#endif
            if (s == JC_RXERR)
            {
                /* rx error, at the moment drop any received byte */
                printf("rx error, %u bytes received\n", (U16) rxTotal);
                fclose(fp);
                return (JC_RXERR);
            }
            else if (s == JC_TMOUT && cpLen == 0)
            {
                /* timeout and nothing received in last rx session */
                printf("end of transfer, %u bytes received\n", (U16) rxTotal);
                fclose(fp);
                return (JC_TMOUT);
            }
            else
            {
                /* here cpLen != 0 (s may be JC_TMOUT or JC_OK or JC_LOCALKBD) */
                rxTotal += cpLen;
/* FRB_DEBUG */
                printf("%u/%u\n", (U16) cpLen, (U16) rxTotal);
                /* printf("."); */
                /* fflush(stdout); */
                write(fileno(fp), cpBuf, (size_t) cpLen);
#ifdef NOT_DEF
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
#endif                
                /* waitkey */
#ifdef FRB_DEBUG
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
                if (c == 27)
                {
                    printf("user break, rxTotal = %u\n", (U16) rxTotal);
                    fclose(fp);
                    return (JC_LOCALBRK);
                }

/*                printf("tx 0x%02x", (U16) c); */
/*
                printf("%c", (U16) c);
                fflush(stdout);
*/
/*
                txBuf[0] = c;
                jcTxS(jc_handle, txBuf);
*/                
            }
        }
        while (kc());
    }
    fclose(fp);

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

    printf("File Receive Binary\n");

    jcRxTO(jc_handle, (U8) 5, (U8) 1);

    s = frb(jc_handle);
    printf("frb = 0x%02x\n", (U16) s);

    /* release resources */
    jcDel(jc_handle);

    return (OK);
}


/* EOF */

