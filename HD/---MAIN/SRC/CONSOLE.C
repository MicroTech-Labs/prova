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
extern	int atoi();

/* #include <STRING.H> */
extern	VOID	sprintf(.);

#include <PROCESS.H>
#include <MALLOC.H>

/* #include <STRING.H> */
extern	char	*strcat(), *strcpy();
extern  size_t  strlen();

#include <BDOSFUNC.H>

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\JC.H>
#include <A:\MAIN\H-LOCAL\XM.H>
#include <A:\MAIN\H-LOCAL\UTILS.H>

extern char getch();
extern BOOL kbhit();
extern int  strcmp();

extern void putkb();
extern VOID bzero();

/* JC parameters */
static U8 speed = 3;    /* 19200 bps */
static U8 port  = 0;    /* port A */
static U8 timing = 0;   /* z80 */
static U8 flowctrl = FC_NONE;
static char *pStdOut = NULL; /* filename of previous command output */

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
/*
                case 'f':
                xm_filename = p;
                break;
*/
            case 'o':
                pStdOut = "out";
                break;

            default:
                /* print help */
                printf("arguments:\n");
                printf("-s: 0 = 2400, 1 = 4800, 2 = 9600, 3 = 19200\n");
                printf("-p: 0 = port A, 1 = port B\n");
                printf("-c: 0 = no flow control, 1 = RTS/CTS\n");
                printf("-t: RESERVED, DO NOT USE\n");    /* 0 = z80 @3.58MHz */
                                                        /* 1 = system timer (UNSUPPORTED) */
/*
                printf("f: save rx to FILENAME (default FRX.BIN)\n");
*/
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



/* max copy & paste length */
#define MAX_CP_LEN      256

STATUS ConIn(_jc_handle, _pBuf, _maxlen)
JC_HANDLE _jc_handle;
char *_pBuf;
size_t _maxlen;
{
    static JC_HANDLE *jc_handle;
    static char *pBuf;
    static size_t maxlen;
    static U8 c;
    static size_t len;
    static boolean stayIn;
    static STATUS rc;
    static STATUS s;
    static size_t cpLen;
    static char cpBuf[MAX_CP_LEN];
    static char *p;


    jc_handle = (JC_HANDLE *) _jc_handle;
    pBuf = _pBuf;
    maxlen = _maxlen;

    stayIn = TRUE;
    len = 0;
    while (stayIn)
    {
        s = jcRxBuf(jc_handle, cpBuf, (size_t) MAX_CP_LEN);
        cpLen = (size_t) MAX_CP_LEN - (size_t) jcRxMiss(jc_handle);

        if (s == JC_RXERR)
        {
            /* rx error, at the moment drop any received byte */
            /* terminate string */
            *(pBuf + len) = 0;
            rc = JC_RXERR;
            stayIn = FALSE;
        }
        else if (s == JC_TMOUT && cpLen == 0)
        {
            /* timeout and nothing received */
            *(pBuf + len) = 0;
            rc = JC_TMOUT;
            stayIn = FALSE;
        }
        else
        {
            /* here cpLen != 0 (s may be JC_TMOUT or JC_OK) */
            /* elaborate 1 char at a time */
            p = cpBuf;
            while (cpLen--)
            {
                c = *p++;
                switch (c)
                {
                    case 0x1B:  /* esc */
                        *(pBuf + len) = 0;
                        rc = JC_REMOTEBRK;
                        cpLen = 0;          /* exit from internal loop */
                        stayIn = FALSE;     /* exit from external loop */
                        break;

                    case 0x0D: /* return */
                        jcTxS(jc_handle, C_RET);
                        *(pBuf + len) = 0;
                        rc = OK;
                        cpLen = 0;          /* exit from internal loop */
                        stayIn = FALSE;     /* exit from external loop */
                        break;

                    case 0x08:  /* backspace */
                        if (len)
                        {
                            jcTxS(jc_handle, C_BS);
                            len--;
                        }
                        break;

                    default:
                        if (len == maxlen)
                            break;

                        jcTxC(jc_handle, &c);
                        *(pBuf + len) = c;
                        len++;
                }
            }
        }
    }

    return (rc);
}


STATUS Prompt(_pDev, _pBuf, _maxlen)
JC_HANDLE _pDev;
char *_pBuf;
size_t _maxlen;
{
    static JC_HANDLE pDev;
    static char *pBuf;
    static size_t maxlen;
    static STATUS s;


    pDev = _pDev;
    pBuf = _pBuf;
    maxlen = _maxlen;

    jcTxS(pDev, "> ");
    s = ConIn(pDev, pBuf, maxlen);

    return (s);
}

#ifdef NOT_DEF
int spawn(cmd)
char *cmd;
{
    char *argv[10];
    char *fn;
    char buf[128];
    U8 pid;
    static FILE *fp;
    static int retValue;
	XREG	r;


    /* get ID */
    r.bc = ((unsigned)_FORK);
    callxx(BDOS, &r);
	if (r.af >> 8)
		return(ERROR);
    pid = r.bc >> 8;
    printf("pid = 0x%02x\n", (U16) pid);
/*
    printf("clear JC\n");
    putenv("JC=");
*/
    fn = "JC.BAT";
    fp = fopen(fn, "W");
    if (fp == NULL)
    {
        printf("cannot open %s\n", fn);
        return (ERROR);
    }

    strcpy(buf, cmd);
    strcat(buf, "\n");
    retValue = fwrite(buf, (int) 1, (int) strlen(buf), (FILE *) fp);

    sprintf(buf, "set JC=%u\n", (U16) pid);
/*    strcpy(buf, "set JC=JOIN\n");
*/
    retValue = fwrite(buf, (int) 1, (int) strlen(buf), (FILE *) fp);

    strcpy(buf, "CONSOLE\n");
    retValue = fwrite(buf, (int) 1, (int) strlen(buf), (FILE *) fp);
/*
    printf("retValue = %d\n", retValue);
    printf("ferror = %s\n", ferror(fp) ? "TRUE" : "FALSE");
*/
    fclose(fp);

    argv[0] = "JC.BAT";
    argv[1] = "";
    execvp("COMMAND2.COM", argv);

    return 0;
}
#endif

/*
    IN char * = 0 terminated string (final \n already removed)
*/
int wrjcbat(cmd)
char *cmd;
{
    char *fn;
    char buf[256];
    static FILE *fp;


    fn = "JC.BAT";
    fp = fopen(fn, "W");
    if (fp == NULL)
    {
        printf("cannot open %s\n", fn);
        return (ERROR);
    }

    strcpy(buf, ">out ");
    strcat(buf, cmd);
    strcat(buf, "\n");
    fwrite(buf, (int) 1, (int) strlen(buf), (FILE *) fp);

    strcpy(buf, "CONSOLE -o\n");
    fwrite(buf, (int) 1, (int) strlen(buf), (FILE *) fp);

    fclose(fp);
    return 0;
}

STATUS sendRaw(_jc_handle, _pStdOut)
JC_HANDLE _jc_handle;
char *_pStdOut;
{
    static JC_HANDLE jc_handle;
    static char *pStdOut;
    char buf[256];
    FILE *fp;
    int n;
    boolean stayIn;


    if (_jc_handle == 0 || _pStdOut == NULL)
        return OK;

    jc_handle = _jc_handle;
    pStdOut = _pStdOut;

    fp = fopen(pStdOut, "R");
    if (fp == NULL)
        return (ERROR);

    stayIn = TRUE;
    while (stayIn == TRUE)
    {
        n = fread(buf, (int) 1, (int) 1, (FILE *) fp);
        if (n)
            jcTxC(jc_handle, &buf[0]);
            /* printf("%c", buf[0]); */
        else
        {
            stayIn = FALSE;
            /* fflush(stdout); */
        }
    }

    fclose(fp);
    return 0;
}


#ifdef NOT_DEF
/* FORK, scrittura JC.BAT, JOIN */
STATUS main_orig(_argc, _argv)
int _argc;
char **_argv;
{
    static boolean stayIn;
    static JC_HANDLE jc_handle;
    static JC_PARAM jc_param;
    static STATUS s;    
    static char cmdBuf[256];
    static int i;
    char *ev_jc;
    U8 pid;
	XREG	r;


    /* check parameters */
    if (OK != GetArgs((int) _argc, _argv))
        return (ERROR);
    if (OK != CheckArgs())
        return (ERROR);

    /* read JC envvar */   
    ev_jc = getenv("JC");
    if (ev_jc == NULL)
    {
        printf("error reading JC\n");
        exit(ERROR);
    }
    else if (strlen(ev_jc) != 0)
    {
        printf("JC=%s\n", (U16) ev_jc);
        pid = atoi(ev_jc);
        printf("pid=%u\n", (U16) pid);
        
        r.bc = ((unsigned) pid * 256 + (unsigned)_JOIN);
        callxx(BDOS, &r);
        printf("JOIN=0x%02x\n", (U16) r.af >> 8);

        /* clear JC */
        putenv("JC=");
    }
    else
        printf("JC not exists");

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
    printf("\nstart console...\n");
    jcTxS(jc_handle, C_RET);
    jcTxS(jc_handle, "start console...");
    jcTxS(jc_handle, C_RET);
#endif

    stayIn = TRUE;
    while (stayIn)
    {
#ifdef DEF
        printf("enter .COM name (only 1 word):\n");
        i = scanf("%s", cmdBuf);
        if (i != 1)
        {
            printf("bad value\n");
            return (ERROR);
        }
        printf("cmdBuf %s\n", cmdBuf);
        s = OK;
#else
        /* get msg from host */
        /* first 3 s, next 1000 ms */
        printf("enter command:\n");
        jcRxTO(jc_handle, (U8) 3, (U8) 250);
        s = Prompt(jc_handle, cmdBuf, sizeof(cmdBuf)-1);
#endif
        if (s == OK)
        {
            /* spawn process */
            if (strcmp(cmdBuf, "exit") == 0)
                stayIn = FALSE;
            else
            {
                printf("spawning command: '%s'\n", cmdBuf);
                spawn(cmdBuf);
            }
        }
    }
    
    /* release resources */
    jcDel(jc_handle);

    return (OK);
}
#endif

#ifdef NOT_DEF
/* keyboard buffer = cmd + console */
STATUS main(_argc, _argv)
int _argc;
char **_argv;
{
    static boolean stayIn;
    static JC_HANDLE jc_handle;
    static JC_PARAM jc_param;
    static STATUS s;    
    static char cmdBuf[256];
    static int i;
    char *ev_jc;
    U8 pid;
	XREG	r;

    stayIn = TRUE;
    while (stayIn)
    {

        printf("enter .COM name (only 1 word):\n");
        i = scanf("%s", cmdBuf);
        if (i != 1)
        {
            printf("bad value\n");
            return (ERROR);
        }
        s = OK;
        if (s == OK)
        {
            /* spawn process */
            if (strcmp(cmdBuf, "exit") == 0)
                stayIn = FALSE;
            else
            {
                strcat(cmdBuf,"\r");
                strcat(cmdBuf,"console\r");
                printf("cmdBuf %s\n", cmdBuf);

                putkb(cmdBuf);
                stayIn = FALSE;
            }
        }
    }
    /* release resources */
    jcDel(jc_handle);

    return (OK);
}
#endif

/* keyboard buffer = jc.bat
jc.bat
    comando
    console
*/
#define MAX_NAME_SZ 32
STATUS main(_argc, _argv)
int _argc;
char **_argv;
{
    static boolean stayIn;
    static JC_HANDLE jc_handle;
    static JC_PARAM jc_param;
    static STATUS s;    
    static char cmd[MAX_NAME_SZ];
    static int i;
    size_t len;
    char *ev_jc;
    U8 pid;
	XREG	r;


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

    stayIn = TRUE;
    while (stayIn)
    {
        if (pStdOut != NULL)
        {
            sendRaw(jc_handle, pStdOut);
            pStdOut = NULL;
        }

#ifdef NOT_DEF        
        printf("enter command (max len %u):\n", (U16) MAX_NAME_SZ);

        /* Get the name, with size limit. */
        fgets(cmd, MAX_NAME_SZ, stdin);
        len = strlen(cmd);
        printf("len %u\n", (U16) len);

        /* Remove trailing newline, if there. */
        if ((len > 0) && (cmd[len - 1] == '\n'))
            cmd[len - 1] = '\0';
#else
        /* get msg from host */

        /* first 20 sec, next 4 ms: to support clipboard copy & paste */
        jcRxTO(jc_handle, (U8) 20, (U8) 1);
        s = Prompt(jc_handle, cmd, MAX_NAME_SZ-1);
#endif
        if (strlen(cmd) > 0)
        {
            printf("cmd = %s\n", cmd);
            /* spawn process */
            if (strcmp(cmd, "exit") == 0)
                stayIn = FALSE;
            else
            {
                if (wrjcbat(cmd) != 0)
                    exit(ERROR);

                putkb("jc.bat\r");
                stayIn = FALSE;
            }
        }
    }
    /* release resources */
    jcDel(jc_handle);

    return (OK);
}

/* EOF */

