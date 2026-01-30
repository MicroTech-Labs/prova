#ifndef _GIFIFUNC_H_
#define _GIFIFUNC_H_

#ifdef __cplusplus
extern "C" {
#endif


void GifLine();                 /* void GifLine(GIF *pGif, int x, int y) */
/*  specifiche per disegnare poligoni con linee orizzontali */
void GifHSet();
void GifHLn();
void GifLO();


#ifdef __cplusplus
}
#endif

#endif
