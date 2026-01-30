#ifndef _XM_H_
#define _XM_H_

typedef unsigned int XM_HANDLE;

/* output codes */
#define XM_TMOUT   ((STATUS) 0x10)
#define XM_RXERR   ((STATUS) 0x11)
#define XM_CAN     ((STATUS) 0x12)
#define XM_UNSUPP  ((STATUS) 0x13)

typedef struct _XM_PARAM
{
    U8 nAttempts;

} XM_PARAM;

extern STATUS     XMSend();
extern STATUS     XMRxChkSum();
extern STATUS     XMRxCRC();
extern STATUS     YMRx();

#endif

/* EOF */
