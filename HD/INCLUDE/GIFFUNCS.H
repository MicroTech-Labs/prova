#ifndef _GIFFUNCS_H_
#define _GIFFUNCS_H_

#ifdef __cplusplus
extern "C" {
#endif


void GifCB();                   /* void gifClearBuffer(E3D_GIF *pGif, boolean f) */
void GifSB();                   /* SwapBuffers */
void GifCF();                   /* gifColorFlat */
void GifLO();                   /* gifLogicalOperation */

/* ritornano costanti di schermo e/o finestra */
uint Focus();
uint Depth();
uint Wsxh();
uint Wsyh();

#ifdef __cplusplus
}
#endif

#endif
