#ifndef _JC_H_
#define _JC_H_

/* JC return codes*/
#define JC_OK           OK
#define JC_TMOUT     ((STATUS) 1)
#define JC_RXERR     ((STATUS) 2)
/* remote host requests termination, returned only by ConIn */
#define JC_REMOTEBRK ((STATUS) 3)
/* not used */
#define JC_LOCALBRK  ((STATUS) 4)
/* local keyboard activity */
#define JC_LOCALKBD  ((STATUS) 5)
/* transmission not possible */
#define JC_CANNOT_TX ((STATUS) 6)

typedef unsigned int JC_HANDLE;

/*
    port    0 = joystick port A
            1 = joystick port B
    speed   0 =  2400 baud
            1 =  4800 baud
            2 =  9600 baud
            3 = 19200 baud
            4 = 38400 baud
    timing 0 = z80 @ 3,5MHz without system timer
            1 = z80 @ 3,5MHz with system timer
            2 = fast(er) cpu with system timer
    flowctrl
            0x00 = FC_NONE no flow control (BIT_NONE)
            0x01 = FC_RTR_CTS (DTE-DTE)    (BIT_RTR_CTS)
*/
#define FC_NONE      (0)
#define FC_RTR_CTS   (1)

typedef struct _JC_PARAM
{
    U8 port;
    U8 speed;
    U8 timing;
    U8 flowctrl;

} JC_PARAM;

extern char *     jcVer();
extern JC_HANDLE  jcInit();
extern STATUS     jcDel();

extern STATUS     jcTxC();
extern STATUS     jcRxC();

extern STATUS     jcTxS();
extern STATUS     jcTxBuf();
extern STATUS     jcRxBuf();

extern STATUS     jcRxTO();
extern STATUS     jcKSTO();
extern size_t     jcRxMiss();

extern VOID       jcRTS();
extern BOOL       jcCTS();
extern BOOL       jcDSR();
extern VOID       jcDTR();

extern char *C_RET;
extern char *C_BS;

#endif

/* EOF */
