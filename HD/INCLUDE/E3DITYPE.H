#ifndef _E3DITYPE_H_
#define _E3DITYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* costanti usate nelle clip mask */
#define CLPNO ((char) 0x00)
#define CLPXL ((char) 0x01)
#define CLPXH ((char) 0x02)
#define CLPYL ((char) 0x04)
#define CLPYH ((char) 0x08)
#define CLPZL ((char) 0x10)
#define CLPZH ((char) 0x20)

/*
typedef uint    SC;
#define MINSC  (0)
#define MAXSC  (65535)
*/
typedef U8    SC;
#define MINSC  (0)
#define MAXSC  (255)

#define MAX_INV MAXSC


typedef U8    T_NPy;

/* costanti x dimensionamento array coinvolti in clipping e rasterizzazione */
#define MaxVxInPy   (8)                 /* (8) per G3D - 20 per 64x48 challenge , 4IL */
#define MaxVxInClPy (MaxVxInPy + 4)     /* TODEBUG + 4 quale verifica "geometrica" ha? */

/* queste costanti non sono valide per landscape */
#define MaxPyInOb   (10)        /* (10) per G3D - 16 per 64x48 challenge , 4IL */
#define MaxVxInOb   (MaxVxInPy * MaxPyInOb)
#define MaxVxInClOb (MaxVxInClPy * MaxPyInOb)

/* numero massimo di vertici necessari per disegnare un qualunque oggetto (normale o landscape) */
/*               il massimo tra: oggetti normali    e           landscape */
#define MaxVxInAnyOb    (MaxVxInClOb * 2)       /* , (NUM_OF_LANDSCAPE_TRIANGLES * MaxVxInClPy))) */

/* numero massimo di vertici necessari per disegnare un qualunque oggetto (normale o landscape) */
/*               il massimo tra: oggetti normali    e           landscape */
#define MaxPyInAnyOb    (MaxPyInOb)         /* , NUM_OF_LANDSCAPE_TRIANGLES)) */

/* definizione del numero massimo di oggetti (escluso il landscape) che possono essere contenuti in uno E3D_SCN */
#define MaxObjInScn     (60)  /* (60) per G3D - 3 per 64x48 challenge */
/*
4inline:
        49 pezzi nella board
         1 pezzo di gioco
         1 board
         1 starfield
         7 next
        --
        59

64x48 challenge:
     1 + 1 pezzo
     1 starfield
    --
     3

*/

/* numero massimo di oggetti "figli" */
#define MaxObSn (3)

#define SclDef  (64)

/* dimensione massima del "nome" di un oggetto (0 finale escluso) */
#define MaxObNmLn       (5)

typedef struct _PX3D
{
    int       x;
    int       y;
    int       z;
} PX3D;


typedef struct _PT2D
{
    SC x;
    SC y;

} PT2D;


typedef struct _PY_DSCR
{
    T_NVx nVx;              /* PY_DSCR DEPENDENT: do not move! */
    VRTX_OFST *pVxO;        /* PY_DSCR DEPENDENT: do not move! */
    T_NVx nPp;
    PT2D *p2DPp;
    T_COLOR color;
    T_COLOR clrFad;
    E3D_COORD z;

} PY_DSCR;


typedef struct _VECTOR
{
    E3D_POS p;
    E3D_ORI o;
} VECTOR;



#ifdef FLAT_8
typedef struct _H_POLY
{
    U8  startX;
    U8  stopX;
} H_POLY;
#else
typedef struct _H_POLY
{
    int       startX;
    int       stopX;
} H_POLY;
#endif

/* bitmap per campo flag di OBJ e CAM */
#define F_T     0x01
#define F_R     0x02
#define F_CAL   0x03        /* F_T | F_R */
#define F_CAM   0x04
#define F_PERS  0x08
#define F_PP    0x10

#define F_D     0x20        /* SMART_CAMERA */

#define F_G     0x40        /* SMART_GAMMA */

typedef struct _CAM
{
    VECTOR cntr;
    U8     flag;
    M44    matrix;
    E3D_POS dt;             /* Differential Translation */
} CAM;


/* per permettere alla struttura di contenere un riferimento a se stessa */
/*
Vx = vertex
Py = poly
Lt = list
*/
struct _OBJ
{
    VECTOR   cntr;
    E3D_POS  cntrWdPos;
    E3D_POS  cntrVwPos; /*cercare tutti gli eventuali riferimenti gia' presenti,
    ma commentati, nel codice */
    U8       flag;
    boolean  tstVw;
    T_NVx    nVx;
    E3D_POS  *pVxLtOb;        /* Object: applicando scalingFactor si ottiene... */
    E3D_POS  *pVxLtSd;        /* Scaled: applicando cntr (rotaz/transl locale ed
                                         eventualmente rotaz/transl ereditata si ottiene... */
    E3D_POS  *pVxLtWd;        /* World: applicando camera si ottiene... */
    E3D_POS  *pVxLtVw;        /* View: applicando prospettiva si ottiene... */
    E3D_POS  *pVxLtSn;        /* Screen: */

    E3D_POS  *pNmLtSd;
    E3D_POS  *pNmLtWd;
    E3D_POS  *pNmLtVw;
    T_NPy    nPy;
    PY_DSCR  *pPolyDescr;
    char     raster;
    uint     bndOb;             /* boundObject */
    uint     bndSd;             /* boundScaled */
    U8       bndBox;
    char     clpMsk;
    U8       ct;                /* clearing tecnique */
    boolean  cf;                /* color fading/se S7 test per DONT_CLIP  */
    NORM     rflx;              /* reflex */
    int      sclN;
    int      sclD;
    /* char     name[MaxObNmLn + 1]; */
    char     *pName;

    boolean  enDsply;
    boolean  pyZS;              /* enable primitives Z sorting */
    boolean  vcOk;
    CAM      *lstCam;

    T_NPy    nPyDefStrm;
    PY_DSCR  **pPyDefStrm;
    T_NPy    nPyWkStrm;
    PY_DSCR  **pPyWkStrm;

/* SUPERIMPOSE */
    E3D_COORD siZ;    /* SuperImpose at Z coordinate */
};

typedef struct _OBJ OBJ;

typedef struct _SCN_Z_SORT
{
    int z;
    OBJ *pObj;
} SCN_Z_SORT;

typedef struct _SCN
{
    U8          nObj;
    E3D_OBJ *   objLst[MaxObjInScn];
    E3D_GIF *   pGif;
    E3D_CAM *   pCam;
    E3D_OBJ *   pLs;
    boolean     obZS;
/* NOT_USED
    E3D_OBJ *        pWallpaper;
    E3D_OBJ *        pHorizontPlane;
*/
} SCN;


#ifdef FLAT_SN
extern H_POLY *     yC;
extern H_POLY *     pyC;
/* TOCUT
extern H_POLY *     yCDefault;
extern H_POLY *     pyCDef;
*/
#endif

#ifdef FLAT_NORM
manca adattamento
extern H_POLY       yC[WSY];
extern H_POLY *     pyC;
extern H_POLY       yCDefault[WSY];
extern H_POLY *     pyCDef;
#endif

extern SC          yCMin;
extern SC          yCMinDefault;
extern SC          yCMax;
extern SC          yCMaxDefault;

#ifdef __cplusplus
}
#endif

#endif

/*** EOF ***/
