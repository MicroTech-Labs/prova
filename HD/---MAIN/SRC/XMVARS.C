#define DIVHEADER
#include <STDIO.H>

#include <MYTYPES.H>

#include <A:\MAIN\H-LOCAL\XM.H>
#include <A:\MAIN\H-LOCAL\XMP.H>

char msg_nak = NAK;
char msg_ack = ACK;
char msg_C = 'C';

U8 m_TmOut = 10;
U8 m_Stop = 5;
U8 m_Prot = 5;
U8 m_UnderRun = 5;
U8 m_BadSeq = 5;
U8 m_ChkCRC = 5;

U8 e_TmOut;      /* timeout waiting for start bit */
U8 e_Stop;       /* stop bit error */
U8 e_Prot;       /* protocol error */
U8 e_UnderRun;   /* underrun error (received less bytes than expected) */
U8 e_BadSeq;     /* sequence number error */
U8 e_ChkCRC;     /* checksum/CRC error */

