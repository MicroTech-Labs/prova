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
static U8 flowctrl = FC_NONE;
/* XM parameters */
static char *xm_filename = "FRX.DAT";

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

                case 'c':
                flowctrl = (*p) & 0x0F;
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

    if (flowctrl > 8)
    {
        printf("unsupported c option %u\n", flowctrl);
        return (ERROR);
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
    jc_param.flowctrl = flowctrl;
    jc_handle = jcInit(&jc_param);
    if (jc_handle == 0)
        return (ERROR);

#ifdef DEF
    printf("\nFile Receive Xmodem\n");
#endif


#ifdef DEF
    /* xmodem rx file */
    printf("CRC...\n");
    /* first 2 s, next 4 ms */
    jcRxTO(jc_handle, (U8) 2, (U8) 1);
    s = XMRxCRC(xm_filename, jc_handle);
    printf("XMRxCRC() = 0x%02x\n", (U16) s);
    if (XM_UNSUPP == s)
    {
        printf("checksum...\n");
        jcRxTO(jc_handle, (U8) 2, (U8) 1);
        s = XMRxChkSum(xm_filename, jc_handle);
        printf("XMRxChkSum() = 0x%02x\n", (U16) s);
    }
#endif

    /* release resources */
    jcDel(jc_handle);

    return (OK);
}


/* EOF */

