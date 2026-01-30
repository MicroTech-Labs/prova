#ifndef _XMP_H_
#define _XMP_H_

/* #define XMDBG */

#define	ACK	0x06
#define CAN	0x18
#define EOT	0x04
#define NAK	0x15
#define SOH	0x01
#define STX	0x02

void resErr();

#ifdef XMDBG
extern void peBadSeq();
extern void peUndRun();
extern void peTmOut();
extern void peProt();
extern void peStop();
extern void peChkCRC();
extern void peReTx();
#endif

extern void txAck();

#endif

/* EOF */
