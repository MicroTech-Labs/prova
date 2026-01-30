/*
#pragma optimize time
#pragma regalo
#pragma nonrec
*/

#define DIVHEADER
#include <STDIO.H>
#include <MSXBIOS.H>
#include <MALLOC.H>

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\JC.H>
#include <A:\MAIN\H-LOCAL\JCITYPE.H>
#include <A:\MAIN\H-LOCAL\JCIFUNC.H>

extern void opldir();
extern VOID gicini();
/* #include <STRING.H> */
extern  size_t  strlen();

#define TRUE 1
#define FALSE 0

/* TOCUT
#define PSG_SEL 0xA0
#define PSG_WR  0xA1
#define PSG_RD  0xA2
*/

/* TOCUT #define TX  0x01  */      /* PSG R15.0 == IOB.0 -> Joy A.6 rileggibile su PSG R14.4 == IOA.4 quando SEL = 0 (SEL = PSG R15.6 == IOB.6) */ /* JOYSTICK PORT_DEPENDENT */
                        /* PSG R15.2 == IOB.2 -> Joy B.6 rileggibile su PSG R14.4 == IOA.4 quando SEL = 1 */ /* JOYSTICK PORT_DEPENDENT */
/* TOCUT #define RTS 0x02   */     /* PSG R15.1 */ /* JOYSTICK PORT_DEPENDENT */
/* TOCUT #define JOY_SEL_MASK    (0x40) */    /* PSG R15.6 == IOB.6 -> selects Joy port for reading, 0 = Joy A, 1 = Joy B */
/* TOCUT #define RX  0x01 */        /* PSG R14.0 == IOA0 <- Joy A/B.1 */


/*
system timer
1 / 256KHz = 0.00000390625 s.
255680Hz -> 3.911 us
100ms = 100000 us / 3.9us = 25641 system timer ticks

                                  system timer ticks richiesti per 1 bit
 1/1200 = 1 bit ogni 833.333 us -> 213,06
 1/2400 = 1 bit ogni 416.666 us -> 106.53
 1/4800 = 1 bit ogni 208.333 us ->  53.26
 1/9600 = 1 bit ogni 104.166 us ->  26.63
1/19200 = 1 bit ogni  52.083 us ->  13.31
1/38400 = 1 bit ogni  26.041 us ->   6.65
*/

static char *_ver = "0.0.1";
char *jcVer()
{
    return (_ver);
}

/*
    IN  JC_PARAM * = parametri funzionamento
    OUT JC_HANDLE = id istanza driver
                    0 se ko
    ACT
*/
JC_HANDLE jcInit(_pParam)
JC_PARAM *_pParam;
{
    static JC_PARAM *pParam;
    static JC_DATA *pData;
    static U8 td;
    static U8 rd;
    static U8 rdh;
    static U16 rxTmOut;
    static unsigned int limit;
    int i;
    U16 lock;
    static U16 val;
    static U8 low, high;


    pParam = _pParam;

    pData = (JC_DATA *) malloc((U16) sizeof(JC_DATA));
    if (pData == NULL)
    {
        /* ERR out of memory */
        return 0;
    }
    opldir(&pData->param, pParam, sizeof(JC_PARAM));

#ifdef NOT_DEF
    printf("Tx delay? ");
	i = scanf("%u", &limit);

	if (i != 1)
	{
        /* ERR bad value */
		return 0;
	}
    td = (U8) limit;

    printf("Rx delay? ");
	i = scanf("%u", &limit);

	if (i != 1)
	{
        /* ERR bad value */
		return 0;
	}
    rd = (U8) limit;
#endif

    switch (pParam->speed)
    {
        case 0: td = 101; rd = 106; rdh = rd / 2; break;  /*  2400 */
        case 1: td =  48; rd =  51; rdh = rd / 2; break;  /*  4800 */
        case 2: td =  22; rd =  25; rdh = rd / 2; break;  /*  9600 */
        case 3: td =   8; rd =  11; rdh = rd / 2; break;  /* 19200 */
        default: return 0;
    }

    pData->txDelay   = td;
    pData->rxDelay   = rd;
    pData->rxDelayHalf = rdh;

#ifdef NOT_DEF
    /* TODO support for system timer */
    switch (pParam->timing)
    {
        case 0:     /* z80 */
            break;
        case 1:     /* system timer */
            break;
    }
#endif

    if (pData->param.port == 0)
    {
        pData->txAndMask  = 0xFE; /* 0b11111110 */
        pData->txOrMask   = 0x01; /* 0b00000001 */
        pData->rtsAndMask = 0xFD; /* 0b11111101 */
        pData->rtsOrMask  = 0x02; /* 0b00000010 */
        pData->dtrAndMask = 0xEF; /* 0b11101111 */
        pData->dtrOrMask  = 0x10; /* 0b00010000 */
    }
    else
    {
        pData->txAndMask  = 0xFB; /* 0b11111011 */
        pData->txOrMask   = 0x04; /* 0b00000100 */
        pData->rtsAndMask = 0xF7; /* 0b11110111 */
        pData->rtsOrMask  = 0x08; /* 0b00001000 */
        pData->dtrAndMask = 0xDF; /* 0b11011111 */
        pData->dtrOrMask  = 0x20; /* 0b00100000 */
    }
    /* on readable signals, AND mask is to get value of the corresponding bit */
    pData->rxAndMask  = 0x01;
    pData->ctsAndMask = 0x02;
    pData->dsrAndMask = 0x04;

    /* INNER_LOOP DEPENDENT */

    /* 1s / 16us = 62500 */
    val = 62500;     /* 1 sec */
    low = (U8) (val & 0xFF);
    val--;
    high = (U8) (val >> 8);
    high++;
    pData->rxTmOutL1st = ((U16) high << 8) | ((U16) low);

    /* 4ms / 16us = 250 */
    val = 250;       /* 4 ms */
    low = (U8) (val & 0xFF);
    val--;
    high = (U8) (val >> 8);
    high++;
    pData->rxTmOutLNxt = ((U16) high << 8) | ((U16) low);

    pData->rxTmOutH1st = 1;
    pData->rxTmOutHNxt = 1;

    pData->disKbdCtr = 0;
    pData->disKbdRld = 0;

    /* _FOOT_PRINT_ */
    gicini();

    lock = jcLock(pData);

    /* WARNING: bios isr performs a scan of keyboard and joystick, setting:
        - dtr  Low
        - rts High
        - tx  High
    */
    dtrH();
    rtsH();
    txH();
    jcUnlock(lock);

end:

    return ((JC_HANDLE) pData);
}

/*
    IN  JC_HANDLE = id istanza driver
        U8 = first char rx timeout with 1 second resolution
        U8 = next chars rx timeout with 4 ms resolution
*/
STATUS jcRxTO(_pDev, _first, _next)
JC_HANDLE _pDev;
U8 _first;
U8 _next;
{
    static JC_DATA *pData;
    static U8 first;
    static U8 next;
    static U16 val;
    static U8 low, high;

    pData = (JC_DATA *) _pDev;
    first = _first;
    next = _next;

    /* INNER_LOOP DEPENDENT */

    pData->rxTmOutH1st = first;
    pData->rxTmOutHNxt = next;

    val = 62500;     /* 1 sec */
    low = (U8) (val & 0xFF);
    val--;
    high = (U8) (val >> 8);
    high++;
    pData->rxTmOutL1st = ((U16) high << 8) | ((U16) low);

    val = 250;       /* 4 ms */
    low = (U8) (val & 0xFF);
    val--;
    high = (U8) (val >> 8);
    high++;
    pData->rxTmOutLNxt = ((U16) high << 8) | ((U16) low);

    return (OK);
}


/*
    IN  JC_HANDLE = id istanza driver
        U8 = keyboard scan timeout value, resolution is ~4ms
            0 keyboard scan is always active
    OUT STATUS
    ACT set Keyboard Scan TimeOut, keyboard activity in this period is ignored by jc receive routine
*/
STATUS jcKSTO(_pDev, _rld)
JC_HANDLE _pDev;
U8 _rld;
{
    /* INNER_LOOP DEPENDENT */

    ((JC_DATA *) _pDev)->disKbdRld = _rld;

    return (OK);
}

/*
    IN  JC_HANDLE = id istanza driver
    OUT
    ACT
*/
STATUS jcDel(_p)
JC_HANDLE _p;
{
    if (_p)
        free((char *) _p);

    gicini();

    return (OK);
}

/*
    
*/
STATUS jcRxC(_pDev, _pRx)
JC_HANDLE _pDev;
char *_pRx;
{
    static JC_DATA *pDev;
    static char *pRx;
    static STATUS s;
    static U16 lock;


    pDev = (JC_DATA *) _pDev;
    pRx = _pRx;

    lock = jcLock(pDev);
    s = z0Rx(pRx, (size_t) 1);
    jcUnlock(lock);

    return (s);
}


STATUS jcRxBuf(_pDev, _pBuf, _maxlen)
JC_HANDLE _pDev;
char *_pBuf;
size_t _maxlen;
{
    static JC_DATA *pDev;
    static char *pBuf;
    static STATUS s;
    static U16 lock;
    static size_t maxlen;


    pDev = (JC_DATA *) _pDev;
    pBuf = _pBuf;
    maxlen = _maxlen;

    lock = jcLock(pDev);
    s = z0Rx(pBuf, (size_t) maxlen);
    jcUnlock(lock);

    return (s);
}


void jcTxC(_pDev, _pTx)
JC_HANDLE _pDev;
char *_pTx;
{
    static JC_DATA *pDev;
    static char *pTx;
    static U16 lock;
    static STATUS s;


    pDev = (JC_DATA *) _pDev;
    pTx = _pTx;

    lock = jcLock(pDev);
    s = z0Tx(pTx, (size_t) 1);
    jcUnlock(lock);

    return (s);
}


STATUS jcTxBuf(_handle, _buf, _len)
JC_HANDLE _handle;
char *_buf;
size_t _len;
{
    static JC_DATA *handle;
    static char *buf;
    static size_t len;    
    static U16 lock;
    static STATUS s;

    handle = (JC_DATA *) _handle;
    buf = _buf;
    len = _len;

    lock = jcLock(handle);
    s = z0Tx(buf, (size_t) len);
    jcUnlock(lock);

    return (s);
}

STATUS jcTxS(_handle, _str)
JC_HANDLE _handle;
char *_str;
{
    return (jcTxBuf(_handle, _str, strlen(_str)));
}

/* ORIGINALE */
#ifdef NOT_DEF
STATUS jcTxS(_pDev, _pTx)
JC_HANDLE _pDev;
char *_pTx;
{
    static JC_DATA *pDev;
    static char *pTx;
    static U16 lock;
    static STATUS s;


    pDev = (JC_DATA *) _pDev;
    pTx = _pTx;

    s = JC_OK;

    lock = jcLock(pDev);
    while ((*pTx) && (s == JC_OK))
    {
        s = z0Tx(pTx, (size_t) 1);
        pTx++;
    }
    jcUnlock(lock);

    return (s);
}
#endif

static char crlf[] = "\r\n";     /* { 0x0D, 0x0A, 0x00 }; */    /* should be equivalent to "\r\n" */
static char cr[]   = "\r";       /* { 0x0D, 0x00 }; */    /* should be equivalent to "\r\n" */
char *C_RET = crlf;

char bsspbs[] = "\b \b";     /* { 0x09, ' ', 0x09, 0x00}; */
char *C_BS = bsspbs;


size_t jcRxMiss(_pDev)
JC_HANDLE _pDev;
{
    return (size_t) ((JC_DATA *) _pDev)->rxMissing;
}


VOID jcRTS(_handle, _s)
JC_HANDLE _handle;
BOOL _s;
{
    static U16 lock;
    static BOOL s;


    s = _s;
    lock = jcLock((JC_DATA *) _handle);
    s ? rtsL() : rtsH();
    jcUnlock(lock);
}


BOOL jcCTS(_handle)
JC_HANDLE _handle;
{
    static U16 lock;
    static BOOL retValue;


    lock = jcLock((JC_DATA *) _handle);
    retValue = (BOOL) cts();
    jcUnlock(lock);
    return (retValue);
}

BOOL jcDSR(_handle)
JC_HANDLE _handle;
{
    static U16 lock;
    static BOOL retValue;


    lock = jcLock((JC_DATA *) _handle);
    retValue = (BOOL) dsr();
    jcUnlock(lock);
    return (retValue);
}

VOID jcDTR(_handle, _s)
JC_HANDLE _handle;
BOOL _s;
{
    static U16 lock;
    static BOOL s;


    s = _s;
    lock = jcLock((JC_DATA *) _handle);
    s ? dtrL() : dtrH();
    jcUnlock(lock);
}

/* EOF */
