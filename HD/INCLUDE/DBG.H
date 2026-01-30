#ifndef _DBG_H_
#define _DBG_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DBG_PARAM
{
    char fn[12];        /* name + ext + 0 */
    U16 tsLut[4];       /* T_MEMSEG DEPENDENT */
    T_FP pDlCall;       /* DlCall entry point */
    U16 dl1;            /* T_DL_ID DEPENDENT */
    U16 dl2;            /* T_DL_ID DEPENDENT */
    U16 dl3;            /* T_DL_ID DEPENDENT */

} DBG_PARAM;


#ifdef __cplusplus
}
#endif

#endif

/*** EOF ***/
