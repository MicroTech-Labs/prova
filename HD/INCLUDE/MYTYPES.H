#ifndef __MYTYPES_H_
#define __MYTYPES_H_


typedef	char	 U8;
typedef	int      S16;
typedef	unsigned U16;
typedef	unsigned uint;
typedef U16 (* T_FP)();

/* per compatibilita' con Visual C */
typedef	VOID     void;
typedef BOOL     boolean;

typedef U8       E3D_ANGLE;
typedef S16      E3D_COORD;
typedef S16      NORM;

#define MAXINT  (32767)
#define MININT  (-32768)
/*
#define MINU8   (0)
#define MAXU8   (255)
*/

#define MSX
#define USE_GRP

#define DEF

#define FLAT_SN
/* #define FLAT_NORM */
#ifdef FLAT_SN
#define FLAT_8
#endif

#endif

