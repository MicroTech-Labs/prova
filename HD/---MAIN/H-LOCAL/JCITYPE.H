#ifndef _JCITYPE_H_
#define _JCITYPE_H_

typedef struct _JC_DATA
{
    U8  txDelay;
    U8  rxDelay;
    U8  rxDelayHalf;

    U8  txAndMask;
    U8  txOrMask;

    U8  rxAndMask;
    U16 rxTmOutL1st;    /* timeout on first reception */
    U8  rxTmOutH1st;
    U16 rxTmOutLNxt;    /* timeout on next reception(s) */
    U8  rxTmOutHNxt;
    U16 rxMissing;

    U8  rtsAndMask;
    U8  rtsOrMask;

    U8  ctsAndMask;
    U8  dsrAndMask;

    U8  dtrAndMask;
    U8  dtrOrMask;

    U8  disKbdCtr;      /* disable keyboard scan counter */
    U8  disKbdRld;      /* disable keyboard scan reload */

    JC_PARAM param;

} JC_DATA;

#endif

/* EOF */
