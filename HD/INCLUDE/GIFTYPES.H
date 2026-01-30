#ifndef _GIFTYPES_H_
#define _GIFTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* TODO le union non piacciono al vs */
/*typedef uint    T_COLOR;*/
typedef U8      T_COLOR;

/*
    screen mode selector, mutually exclusive,
    if changed verify coherency with corresponding labels in .mac files
*/
#define S7
/* #define S8 */

#define	SSX     (256)     /* raccomandato che sia pari */
#define	SSY     (192)     /* raccomandato che sia pari, deve essere > 1 (usato per carbon copy ldir) */


typedef struct _XGIF
{
    char a;
} E3D_GIF;

typedef struct _GIF_PARAM
{
    uint ssx;
    uint ssy;
    uint wox;
    uint woy;
    uint f;
    uint fu;
    uint d;
    uint wsxh;
    uint wsyh;
    uint rctN;
    boolean drwWin;

} GIF_PARAM;

#ifdef __cplusplus
}
#endif

#endif
