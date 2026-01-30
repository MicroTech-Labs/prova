#ifndef _GIFITYPE_H_
#define _GIFITYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _T_RCT
{
    U8  n;
    U8  xl;       /* SC DEPENDENT */
    U8  yl;       /* SC DEPENDENT */
    U8  xh;       /* SC DEPENDENT */
    U8  yh;       /* SC DEPENDENT */

} T_RCT;


typedef struct _GIF
{
    GIF_PARAM param;

    uint    ssx;
    uint    ssy;
/*
    uint    ssxh;
    uint    ssyh;
*/
    uint    wox;
    uint    woy;
/*
    uint    fugaX;
    uint    fugaY;
*/
    uint    focus;
    uint    depth;

    uint    wsxh;
    uint    wsyh;
/* TO_CUT
    DATA    kViewX;
    DATA    kViewY;
*/
    boolean first;

    NORM   *pfzRate;
    void   *pfzMA;
    NORM    kvx;
    NORM    kvy;

    /* scacchi
    boolean rstCpy;
    boolean rstFil;
    char    rstPg;
    uint    rstX;
    uint    rstY;
    */

    U8  Hkx;       /* SC DEPENDENT */
    U8  Hky;       /* SC DEPENDENT */

    U8  scxl;       /* SC DEPENDENT */
    U8  scyl;       /* SC DEPENDENT */
    U8  scxh;       /* SC DEPENDENT */
    U8  scyh;       /* SC DEPENDENT */

    T_RCT *rctPtr[2];
    T_RCT *rctBase[2];
    uint rctN;

    T_COLOR clearClr;
} GIF;


#ifdef __cplusplus
}
#endif

#endif
