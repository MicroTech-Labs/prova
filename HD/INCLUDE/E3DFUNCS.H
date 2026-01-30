#ifndef _E3DFUNCS_H_
#define _E3DFUNCS_H_

#ifdef __cplusplus
extern "C" {
#endif


void iniCoord();
void iniAngle();
/*
E3D_OBJ *ObNw();
E3D_OBJ *ObNw2();
E3D_OBJ *ObNw3();
*/
boolean  ObDel();

void ObTR();
void ObTRV(); 
void ObRRV();
/*
void NmCal();
*/

void ObTA();
void ObRR();
void ObRA();
void ObCal();           /* e3dRecalcObject */
void ObCam();
E3D_CAM * CamNw();
boolean  CamDel();
void CamTR();
void CamTA();
void CamRR();
void CamRA();
void e3DrvI();          /* e3dDrvInstall() */
int  ObGetZ();          /* e3dGetDistance */

void ObRdO();           /* ObjectReadOrientation ADDED_TO_MSX_VERSION */
void ObRdP();           /* ObjectReadPosition ADDED_TO_MSX_VERSION */
void CamWrO();          /* CameraWriteOrientation ADDED_TO_MSX_VERSION */
void CamWrP();          /* CameraWritePosition ADDED_TO_MSX_VERSION */

void CamRdO();          /* CameraWriteOrientation ADDED_TO_MSX_VERSION */
void CamRdP();          /* CameraWritePosition ADDED_TO_MSX_VERSION */

void L2WCal();          /* da E3D_ORI e E3D_POS a coefficienti di rotazione */
void L2WAply();

#ifdef NOT_DEF
boolean ChkExL();
boolean ChkExH();
void GtWdEx();
#endif

boolean Inside();
int Scalar();

void PosCpy();          /* e3d_cpy_v4 */
void OriCpy();          /* e3d_cpy_v4 */

/* supporto per scenario */
E3D_SCN * ScnNew();     /* e3dSceneryCreate */
void ScnDel();          /* e3dSceneryDestroy */
void ScnCam();          /* e3dScenerySetCamera */
void ScnLs();           /* e3dScenerySetLandscape */
void ScnGif();          /* e3dScenerySetGif */
void ScnRes();          /* e3dSceneryResetObjectList */
boolean ScnAdd();       /* e3dSceneryAddObjectToList */
void ScnDrw();          /* e3dSceneryDrawObjectList */
void ScnCal();          /* e3dSceneryRecalcObjects ADDED_TO_MSX_VERSION */


#ifdef __cplusplus
}
#endif

#endif

/*** EOF ***/
