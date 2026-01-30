
#ifndef _E3DPARAM_H_
#define _E3DPARAM_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _E3D_PARAM
{
    E3D_FP pDlCall;
    U16 dl1;            /* T_DL_ID DEPENDENT */
    U16 dl2;            /* T_DL_ID DEPENDENT */
    U16 dl3;            /* T_DL_ID DEPENDENT */
    U16 pSysHeap;       /* HEAP DEPENDENT */
    U16 pP0Heap;        /* HEAP DEPENDENT */
    void *pTmrLst;      /* T_ST DEPENDENT */
    void *dummy0;

} E3D_PARAM;


#ifdef __cplusplus
}
#endif

#endif

/*** EOF ***/
