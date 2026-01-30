#ifndef _E3DIFUNC_H_
#define _E3DIFUNC_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void    LSRaster();     /* RasterPipeSegment */
extern void    WFRaster();     /* e3dWireFrameRaster <- forse e3dRasterPipeWireFrame*/
extern void    FLRaster();     /* e3dRasterPipeFlat */
extern boolean ChkVwP();       /* CheckViewPipe */
extern boolean ChkVwSg();      /* Check View Segment */
extern boolean GetMsk();       /* GetMask */
extern void    FndIS();        /* FindIntersection */
extern T_NVx   ClpPp();        /* ClipPipe */
extern T_NVx   ClpXYZ();       /* ClipPipeXYZ */
extern void    PpPers();       /* ApplyPerspectiveToPipe */
extern T_NVx   Pp2Sp();        /* poly pipe to segment pipe */
extern T_NVx   ClpSp();        /* clip segment pipe */
extern void    e3dError();     /* unrecoverable error */
extern int     GetSgBar();     /* Get Segment Baricenter */

#ifdef __cplusplus
}
#endif

#endif

/*** EOF ***/
