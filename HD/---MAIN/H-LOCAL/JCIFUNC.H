#ifndef _JCIFUNC_H_
#define _JCIFUNC_H_

extern U16    jcLock();
extern void   jcUnlock();

extern void psgStart();
extern void psgStop();

/*
all these functions do not need JC_HANDLE
must be inkoved inside a code section between jcLock and jcUnlock calls
code between jcLock and jcUnlock is a jc _CritSect (critical section):
interrupts are disabled, IX and alt regs are used by jc driver
*/
extern void   txH();
extern void   rtsH();
extern void   rtsL();
extern BOOL   cts();
extern void   dtrH();
extern void   dtrL();
extern BOOL   dsr();
extern STATUS z0Tx();
extern STATUS z0TxNS();
extern STATUS z0Rx();

#endif

/* EOF */
