/*
label "strategiche"
USE_LANDSCAPE
*/

#ifndef _E3DTYPES_H_
#define _E3DTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Use Parallel Vdp */
#define UPV
#define MM 1
#define mm 0

/* typedef float   DATA; */
typedef E3D_COORD       E3D_POS[3];
typedef E3D_ANGLE       E3D_ORI[3];
typedef U8              VRTX_OFST;      /* T_NVx DEPENDENT */
typedef U8    T_NVx;

typedef void    (* E3D_FP)();

/*
#define PI          ((DATA) 3.141592653589793L)
#define GRADO_RAD   ((DATA) PI / (DATA) 180L)
*/

/* costanti usate per accedere ai campi di E3D_POS */
#define C_X     0
#define C_Y     1
#define C_Z     2

/* costanti usate per accedere ai campi di E3D_ORI */
#define A_ALFA  0
#define A_BETA  1
#define A_GAMMA 2

/*
typedef int COLOR_TYPE;
#define FLAT    0
#define RGB     1
*/

typedef char T_RASTER;
#define WF_RR   ((T_RASTER) 0x01)
#define SLD_RR  ((T_RASTER) 0x02)
#define LS_RR   ((T_RASTER) 0x04)


#define BB_NO   (0x00)      /* no bounding box */
#define BB_CUBE (0x01)      /* Cube */
#define BB_SVL  (0x02)      /* Scan Vertex List */

/* Clearing Tecnique */
#define CT_NO   (0x00)      /* no clearing required */
#define CT_AEP  (0x01)      /* Assert Every Polygon (Primitive) */
#define CT_AO   (0x02)      /* Assert Object */

typedef struct _E3D_CAM
{
    char    a;
} E3D_CAM;


typedef struct _E3D_OBJ
{
    char    a;
} E3D_OBJ;


typedef struct _E3D_SCN
{
    char    a;
} E3D_SCN;


#define M_X1    0
/*
#define M_Y1    1
#define M_Z1    2

#define M_X2    3
#define M_Y2    4
#define M_Z2    5

#define M_X3    6
#define M_Y3    7
#define M_Z3    8
*/
#define M_TX    0
/*
#define M_TY    1
#define M_TZ    2
*/

typedef struct _M44
{
    NORM      rotate[9];
    E3D_COORD translate[3];     /* E3D_POS DEPENDENT */
} M44;

typedef NORM    E3D_VERSOR[3];


typedef struct _OBJ_PARAM
{
    T_NVx     nVx;
    E3D_POS   *pVxLt;
    VRTX_OFST *pVxOfst;
    T_COLOR   *pColor;
    T_RASTER  rr;
    U8        ct;
    boolean   cf;           /* color fading/se S7 test per DONT_CLIP */
    boolean   tstVw;
    char      *pName;
    E3D_COORD siZ;
    U8        BndBox;
    boolean   pyZS;

} OBJ_PARAM;


#ifdef __cplusplus
}
#endif

#endif

/*** EOF ***/
