#define DIVHEADER
#include <STDIO.H>

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\XMP.H>

extern char msg_nak;
extern char msg_ack;
extern char msg_C;

extern U8 m_TmOut;
extern U8 m_Stop;
extern U8 m_Prot;
extern U8 m_UnderRun;
extern U8 m_BadSeq;
extern U8 m_ChkCRC;

extern U8 e_TmOut;      /* timeout waiting for start bit */
extern U8 e_Stop;       /* stop bit error */
extern U8 e_Prot;       /* protocol error */
extern U8 e_UnderRun;   /* underrun error (received less bytes than expected) */
extern U8 e_BadSeq;     /* sequence number error */
extern U8 e_ChkCRC;     /* checksum/CRC error */

