#ifndef _GIFCOLOR_H_
#define _GIFCOLOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef S8
/* versione screen 8                                G,R,B (3,3,2) */
#define BLACK           0x00
#define LGREEN          0xE5    /* verde chiaro     7,1,1 */
#define BLUE            0x03    /* blu              0,0,3 */
#define LBLUE           0x4B    /* blu chiaro       2,2,3 */

#define RED             0x1C    /* rosso            0,7,0 */
#define RED2            0x39    /* rosso scuro      1,6,1 */
#define MAGENTA         0x56    /* rosso magenta    2,5,2 */
#define RED3            0x73    /* rosso chiaro     3,4,3 */

#define SENAPE          0xB4    /* senape           5,5,0 */
#define CYAN            0x93    /* azzurro chiaro   4,4,3 */
#define YELLOW          0xFC    /* giallo */
#define GREEN           0xE0    /* verde            7,0,0 */

#define VIOLET          0x0F    /* viola            0,3,3 */
#define GRAY            0x6F    /* grigio */
#define WHITE           0xFF
#endif


#ifdef S7
/* versione palette 16 colori */
#define BLACK           1
#define LGREEN          2   /* verde chiaro */
#define BLUE            4   /* blu */
#define LBLUE           5   /* blu chiaro */

#define MAGENTA         6   /* rosso magenta */
#define CYAN            7   /* azzurro chiaro*/
#define RED             8   /* rosso */
#define YELLOW          10  /* giallo senape */

#define GREEN           12  /* verde */
#define VIOLET          13  /* viola */
#define GRAY            14  /* grigio */
#define WHITE           15
#endif


#ifdef __cplusplus
}
#endif

#endif
