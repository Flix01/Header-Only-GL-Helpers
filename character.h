//    _____ _    _                         _
//   / ____| |  | |   /\                  | |
//  | |    | |__| |  /  \   _ __ __ _  ___| |_ ___ _ __
//  | |    |  __  | / /\ \ | '__/ _` |/ __| __/ _ \ '__|
//  | |____| |  | |/ ____ \| | | (_| | (__| ||  __/ |
//   \_____|_|  |_/_/    \_\_|  \__,_|\___|\__\___|_|
//

// https://github.com/Flix01/Header-Only-GL-Helpers
/** MIT License
 *
 * Copyright (c) 2019 Flix (https://github.com/Flix01/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/*
===================
WHAT's THIS:
===================
An header-only, plain C, animated character.

Code should be in ANSI C (-std=c89) with some notable exceptions:
-> single line comments are used.
-> initializer elements can be computable at load time.
-> some C99 math function are used (sinf,cosf,etc.). These can be removed by defining CHA_NO_C99_MATH.
So, to be strict, we should compile the code using -std=c99, but I prefer using -std=gnu89, that allows them.
Also, older versions of MSVC seem to work correctly out of the box with /DCHA_NO_C99_MATH
(even if they're not fully C99 compliant, and/or you can't specify the C dialect to use).

===================
BASIC USAGE:
===================
/*
// Suppose you already have an OpenGL application (that supports the fixed-function-pipeline) working
// Make the following changes, and if everything goes well, you should see something on screen...
// (This is a cut-down version of 'test_character_standalone.c'.
// If you don't support the fixed function pipeline, please see 'test_character.c'.)

... // OpenGL must be available from here

#define CHA_HAS_OPENGL_SUPPORT
#define CHA_USE_VBO                 // optional: it just creates and uses 'vertex buffer objects' (vbo), instead of glBegin(GL_TRIANGLES)/glEnd()
#define CHA_HINT_USE_FFP_VBO        // mandatory if CHA_USE_VBO (otherwise 'vbos' need to know shader attribute locations)
//#define CHA_HINT_USE_VAO          // optional
#define CHARACTER_IMPLEMENTATION
#include <character.h>

static struct cha_character_group* group = NULL;    // global

void InitGL(void) {
    ...
    Character_Init();
    group = Character_CreateGroup(3,3,1.85f,1.75f);
    ...
}
void DestroyGL(void)    {
    ...
    if (group) {Character_DestroyGroup(group);group=NULL;}
    Character_Destroy();
    ...
}
void DrawGL(void)   {
    static float vMatrix[16];
    ...
    chm_Mat4LookAtf(vMatrix,0.f,3.f,15.f,0.f,1.5f,0.f,0.f,1.f,0.f);
    //glLoadMatrixf(vMatrix);
    //glLightfv(GL_LIGHT0,GL_POSITION,lightDirection);  // Important call: the ffp must recalculate internally lightDirectionEyeSpace based on vMatrix [=> every frame]

    // optionally move and aninate characters here (see the demo)... and then call
    cha_character_group_updateMatrices(&group,1,vMatrix,NULL);    // if 'pMatrixNormalizedFrustumPlanes==NULL' there's no frustum culling (you can calculate it in ResizeGL(), see the demo)

    glPushMatrix();
        glLoadIdentity();
        // disable color material if you enabled it
        Character_DrawGroupOpengl(&group,1,0);
        // reenable it if necessary
    glPopMatrix();

    ...
}

// That's all! Easy? Yes.
*/

/*
===================
A POSSIBLE ROADMAP:
===================
-> Move all structs to the declaration section so that they are exposed.
-> Choose the functions to be exposed and expose them (currently this file works only when the implementation is defined).
-> Decide if we need to merge 'character_inl.h' or not.
-> Should we elaborate some higher-level animation API or not? And how? (I've never used programs like Unity 3D and Unreal Engine, so I don't know what kind of API is better for the user).
-> Add other animations.
-> Add other animation 'tools' (for example to play them backwards, or to mirror them in the X direction).

-> Remove all unnecessary chm_ math functions to slim file size a bit.
-> Add dependency to <float.h> for FLT_MAX (currently I hard code it, but better be on the safe side).
-> CHA_HINT_VERTEX_ATTRIBUTE_LOCATION and CHA_HINT_NORMAL_ATTRIBUTE_LOCATION are const... Must we set them at runtime?
*/

#ifndef CHARACTER_H_
#define CHARACTER_H_

#define CHA_VERSION               "0.1 ALPHA"
#define CHA_VERSION_NUM           0001

#ifndef CHA_API_INL
#   define CHA_API_INL __inline
#endif
#ifndef CHA_API_DEC
#   define CHA_API_DEC extern CHA_API_INL
#endif
#ifndef CHA_API_DEF
#   define CHA_API_DEF /* no-op */
#endif
#ifndef CHA_API_PRIV
#   define CHA_API_PRIV CHA_API_INL static
#endif

#ifdef CHA_HAS_OPENGL_SUPPORT
#   ifndef GL_TEXTURE_2D
#       error CHA_HAS_OPENGL_SUPPORT needs access to the OpenGL header files
#   endif
#   ifdef CHA_USE_VBO
#       ifndef CHA_HINT_VERTEX_ATTRIBUTE_LOCATION      /* vertex attribute locations should be made configurable at runtime (at init time) */
#           define CHA_HINT_VERTEX_ATTRIBUTE_LOCATION 0
#   endif
#   ifndef CHA_HINT_NORMAL_ATTRIBUTE_LOCATION
#       define CHA_HINT_NORMAL_ATTRIBUTE_LOCATION 1
#   endif
#   else    /*CHA_USE_VBO*/
#       undef CHA_HINT_USE_VAO
#       undef CHA_HINT_USE_FFP_VBO
#   endif   /*CHA_USE_VBO*/
#else /*CHA_HAS_OPENGL_SUPPORT*/
#   ifdef CHA_USE_VBO
#       undef CHA_USE_VBO
#   endif
#endif /*CHA_HAS_OPENGL_SUPPORT*/

#ifdef CHA_USE_DOUBLE_PRECISION
#   undef CHA_DOUBLE_PRECISION
#   define CHA_DOUBLE_PRECISION
#endif

#ifdef CHA_DOUBLE_PRECISION /* it affects mMatrices (and vMatrices) only. All the rest stays 'float'. 'choat' is CHaracter_flOAT */
#   undef CHA_USE_DOUBLE_PRECISION
#   define CHA_USE_DOUBLE_PRECISION
typedef double choat;
#else
typedef float choat;
#endif

#ifndef CHA_RESTRICT
#   ifdef __restrict
#       define CHA_RESTRICT __restrict
#   else
#       define CHA_RESTRICT /*no-op*/
#   endif
#endif

#if (defined (NDEBUG) || defined (_NDEBUG))
#   undef CHA_NO_ASSERT
#   define CHA_NO_ASSERT
#   undef CHA_NO_STDIO
#   define CHA_NO_STDIO
#   undef CHA_NO_STDLIB
#   define CHA_NO_STDLIB
#endif

CHA_API_DEC void Character_Init(void);
CHA_API_DEC void Character_Destroy(void);

CHA_API_DEC struct cha_character_group* Character_CreateGroup(int num_men,int num_ladies,float men_scaling,float ladies_scaling);
CHA_API_DEC void Character_DestroyGroup(struct cha_character_group* p);


#if (defined(CHA_HAS_OPENGL_SUPPORT) && (!defined(CHA_USE_VBO) || defined(CHA_HINT_USE_FFP_VBO)))
CHA_API_DEC void Character_DrawGroupOpengl(struct cha_character_group*const* pp,int num_group_pointers,int no_materials/*=0*/);
#endif

#include "character_inl.h"

#ifndef CHA_NO_CHARACTER_INSTANCE_NAMES
enum ChaCharacterInstanceManNameEnum {
    CHA_CHARACTER_INSTANCE_MAN_NAME_PETER=0,    CHA_CHARACTER_INSTANCE_MAN_NAME_JOHN,   CHA_CHARACTER_INSTANCE_MAN_NAME_ANDREW, CHA_CHARACTER_INSTANCE_MAN_NAME_JAMES,      CHA_CHARACTER_INSTANCE_MAN_NAME_PHILIP,
    CHA_CHARACTER_INSTANCE_MAN_NAME_THOMAS,     CHA_CHARACTER_INSTANCE_MAN_NAME_MATTHEW,CHA_CHARACTER_INSTANCE_MAN_NAME_SIMON,  CHA_CHARACTER_INSTANCE_MAN_NAME_BARTHOLOMEW,CHA_CHARACTER_INSTANCE_MAN_NAME_JUDE,
    CHA_CHARACTER_INSTANCE_MAN_NAME_MATTHIAS,   CHA_CHARACTER_INSTANCE_MAN_NAME_JOSEPH, CHA_CHARACTER_INSTANCE_MAN_NAME_PAUL,   CHA_CHARACTER_INSTANCE_MAN_NAME_STEPHEN,    CHA_CHARACTER_INSTANCE_MAN_NAME_FRANCIS,
    CHA_CHARACTER_INSTANCE_MAN_NAME_BENEDICT,   CHA_CHARACTER_INSTANCE_MAN_NAME_JOACHIM,CHA_CHARACTER_INSTANCE_MAN_NAME_CHARLES,CHA_CHARACTER_INSTANCE_MAN_NAME_AUGUSTINE,  CHA_CHARACTER_INSTANCE_MAN_NAME_DOMINIC,
    CHA_CHARACTER_INSTANCE_MAN_NAME_COUNT
};
enum ChaCharacterInstanceLadyNameEnum {
    CHA_CHARACTER_INSTANCE_LADY_NAME_MARY=0,    CHA_CHARACTER_INSTANCE_LADY_NAME_ANNE,      CHA_CHARACTER_INSTANCE_LADY_NAME_MARTHA,    CHA_CHARACTER_INSTANCE_LADY_NAME_SUSAN,     CHA_CHARACTER_INSTANCE_LADY_NAME_JOAN,
    CHA_CHARACTER_INSTANCE_LADY_NAME_LUCY,      CHA_CHARACTER_INSTANCE_LADY_NAME_CATHERINE, CHA_CHARACTER_INSTANCE_LADY_NAME_MARGARET,  CHA_CHARACTER_INSTANCE_LADY_NAME_ELIZABETH, CHA_CHARACTER_INSTANCE_LADY_NAME_THERESA,
    CHA_CHARACTER_INSTANCE_LADY_NAME_RITA,      CHA_CHARACTER_INSTANCE_LADY_NAME_MONICA,    CHA_CHARACTER_INSTANCE_LADY_NAME_RACHEL,    CHA_CHARACTER_INSTANCE_LADY_NAME_ROSE,      CHA_CHARACTER_INSTANCE_LADY_NAME_HELENA,
    CHA_CHARACTER_INSTANCE_LADY_NAME_BARBARA,   CHA_CHARACTER_INSTANCE_LADY_NAME_BRIDGET,   CHA_CHARACTER_INSTANCE_LADY_NAME_CHRISTINE, CHA_CHARACTER_INSTANCE_LADY_NAME_ALEXANDRA, CHA_CHARACTER_INSTANCE_LADY_NAME_PRISCILLA,
    CHA_CHARACTER_INSTANCE_LADY_NAME_COUNT
};
#endif

#include <string.h> // memcpy
#include <math.h>

#ifndef CHA_NO_STDIO
#   include <stdio.h> /*fprintf,printf,stderr,FILE*/
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_HALF_PI
#define M_HALF_PI (M_PI/2.0)
#endif
#ifndef M_PIOVER180
#define M_PIOVER180 (3.14159265358979323846/180.0)
#endif
#ifndef M_180OVERPI
#define M_180OVERPI (180.0/3.14159265358979323846)
#endif

#ifdef CHA_NO_C99_MATH
#	define sqrtf(X)		((float)sqrt(X))
#	define floorf(X)	((float)floor(X))
#	define ceilf(X)		((float)ceil(X))
#	define tanf(X)		((float)tan(X))
#	define sinf(X)		((float)sin(X))
#	define cosf(X)		((float)cos(X))
#	define fabsf(X)		((float)abs(X))
#	define atan2f(X,Y)	((float)atan2(X,Y))
#	define fmodf(X,Y)	((float)fmod(X,Y))
#endif

#ifdef CHA_USE_MATH_MACROS
#ifndef CHA_DOUBLE_PRECISION
#   define chm_sin(X)                       sinf(X)
#   define chm_cos(X)                       cosf(X)
#   define chm_Round(X)                     chm_Roundf(X)
#   define chm_Vec3Dot(X,Y)                 chm_Vec3Dotf(X,Y)
#   define chm_Vec3Normalize(X)             chm_Vec3Normalizef(X)
#   define chm_Vec3Cross(X,Y,Z)             chm_Vec3Crossf(X,Y,Z)
#   define chm_Vec3DistSquared(X,Y)         chm_Vec3DistSquaredf(X,Y)
#   define chm_Vec3Dist(X,Y)                chm_Vec3Distf(X,Y)
#   define chm_Mat4Identity(X)              chm_Mat4Identityf(X)
#   define chm_Mat4Copy(X,Y)                chm_Mat4Copyf(X,Y)
#   define chm_Mat4MulUncheckArgs(X,Y,Z)    chm_Mat4MulUncheckArgsf(X,Y,Z)
#   define chm_Mat4Mul(X,Y,Z)               chm_Mat4Mulf(X,Y,Z)
#   define chm_Mat4MulDir(M,DO,X,Y,Z)       chm_Mat4MulDirf(M,DO,X,Y,Z)
#   define chm_Mat4MulPos(M,PO,X,Y,Z)       chm_Mat4MulPosf(M,PO,X,Y,Z)
#   define chm_Mat4LookAt(M,EX,EY,EZ,CX,CY,CZ,UX,UY,UZ)  chm_Mat4LookAtf(M,EX,EY,EZ,CX,CY,CZ,UX,UY,UZ)
#   define chm_Mat4Perspective(M,DFY,AR,N,F)    chm_Mat4Perspectivef(M,DFY,AR,N,F)
#   define chm_Mat4Ortho(M,L,R,B,T,N,F)         chm_Mat4Orthof((M,L,R,B,T,N,F)
#   define chm_Mat4Ortho3D(M,D,DF,AR,N,F)       chm_Mat4Ortho3Df(M,D,DF,AR,N,F)
#   define chm_Mat4Translate(M,X,Y,Z)           chm_Mat4Translatef(M,X,Y,Z)
#   define chm_Mat4Rotate(M,DA,X,Y,Z)           chm_Mat4Rotatef(M,DA,X,Y,Z)
#   define chm_Mat4Scale(M,X,Y,Z)               chm_Mat4Scalef(M,X,Y,Z)
#   define chm_Mat4Invert(X,Y)                  chm_Mat4Invertf(X,Y)
#   define chm_Mat4InvertTransformMatrix(X,Y)   chm_Mat4InvertTransformMatrixf(X,Y)
#   define chm_Mat4InvertTransformMatrixFast(X,Y)   chm_Mat4InvertTransformMatrixFastf(X,Y)
#   define chm_PlaneNormalize(P)                    chm_PlaneNormalizef(P)
#   define chm_GetFrustumPlaneEquations(PE,M,N)    chm_GetFrustumPlaneEquationsf(PE,M,N)
#   define chm_IsOBBVisible(PE,M,AMINX,AMINY,AMINZ,AMAXX,AMAXY,AMAXZ)   chm_IsOBBVisiblef(PE,M,AMINX,AMINY,AMINZ,AMAXX,AMAXY,AMAXZ)
#   define chm_GetFrustumPoints(FP,MINV)                                chm_GetFrustumPointsf(FP,MINV)
#   define chm_GetFrustumAabbCenterAndHalfExtents(CO,HEO,FP)            chm_GetFrustumAabbCenterAndHalfExtentsf(CO,HEO,FP)
#   define chm_GetFrustumRadiusAndCenterDistance(CDO,N,F,FD,AR,CTDN)    chm_GetFrustumRadiusAndCenterDistancef(CDO,N,F,FD,AR,CTDN)
#   define chm_GetFrustumCenterFromCenterDistance(CO,VINV,FDO)          chm_GetFrustumCenterFromCenterDistancef(CO,VINV,FDO)
#   define chm_GetLightViewProjectionMatrixExtra(LVPMO,VINV,C,R,LD,W,H,OPINV,OVO,OFPO,OLVPMOFC) chm_GetLightViewProjectionMatrixExtraf(LVPMO,VINV,C,R,LD,W,H,OPINV,OVO,OFPO,OLVPMOFC)
#   define chm_GetLightViewProjectionMatrix(LVPMO,VINV,C,R,LD,W,H)      chm_GetLightViewProjectionMatrixf(LVPMO,VINV,C,R,LD,W,H)
#   ifndef CHA_NO_STDIO
#       define chm_Mat4_fprintp(M,F,W,P)                chm_Mat4_fprintpf(M,F,W,P)
#       define chm_Mat4_fprint(M,F)                     chm_Mat4_fprintf(M,F)
#       define chm_Mat4_printp(M,W,P)                   chm_Mat4_printpf(M,W,P)
#       define chm_Mat4_print(M)                        chm_Mat4_printf(M)
#   endif
#else /*CHA_DOUBLE_PRECISION*/
#   define chm_sin(X)                       sin(X)
#   define chm_cos(X)                       cos(X)
#   define chm_Round(X)                     chm_Roundd(X)
#   define chm_Vec3Dot(X,Y)                 chm_Vec3Dotd(X,Y)
#   define chm_Vec3Normalize(X)             chm_Vec3Normalized(X)
#   define chm_Vec3Cross(X,Y,Z)             chm_Vec3Crossd(X,Y,Z)
#   define chm_Vec3DistSquared(X,Y)         chm_Vec3DistSquaredd(X,Y)
#   define chm_Vec3Dist(X,Y)                chm_Vec3Distd(X,Y)
#   define chm_Mat4Identity(X)              chm_Mat4Identityd(X)
#   define chm_Mat4Copy(X,Y)                chm_Mat4Copyd(X,Y)
#   define chm_Mat4MulUncheckArgs(X,Y,Z)    chm_Mat4MulUncheckArgsd(X,Y,Z)
#   define chm_Mat4Mul(X,Y,Z)               chm_Mat4Muld(X,Y,Z)
#   define chm_Mat4MulDir(M,DO,X,Y,Z)       chm_Mat4MulDird(M,DO,X,Y,Z)
#   define chm_Mat4MulPos(M,PO,X,Y,Z)       chm_Mat4MulPosd(M,PO,X,Y,Z)
#   define chm_Mat4LookAt(M,EX,EY,EZ,CX,CY,CZ,UX,UY,UZ)  chm_Mat4LookAtd(M,EX,EY,EZ,CX,CY,CZ,UX,UY,UZ)
#   define chm_Mat4Perspective(M,DFY,AR,N,F)    chm_Mat4Perspectived(M,DFY,AR,N,F)
#   define chm_Mat4Ortho(M,L,R,B,T,N,F)         chm_Mat4Orthod((M,L,R,B,T,N,F)
#   define chm_Mat4Ortho3D(M,D,DF,AR,N,F)       chm_Mat4Ortho3Dd(M,D,DF,AR,N,F)
#   define chm_Mat4Translate(M,X,Y,Z)           chm_Mat4Translated(M,X,Y,Z)
#   define chm_Mat4Rotate(M,DA,X,Y,Z)           chm_Mat4Rotated(M,DA,X,Y,Z)
#   define chm_Mat4Scale(M,X,Y,Z)               chm_Mat4Scaled(M,X,Y,Z)
#   define chm_Mat4Invert(X,Y)                  chm_Mat4Invertd(X,Y)
#   define chm_Mat4InvertTransformMatrix(X,Y)   chm_Mat4InvertTransformMatrixd(X,Y)
#   define chm_Mat4InvertTransformMatrixFast(X,Y)   chm_Mat4InvertTransformMatrixFastd(X,Y)
#   define chm_PlaneNormalize(P)                    chm_PlaneNormalized(P)
#   define chm_GetFrustumPlaneEquations(PE,M,N)    chm_GetFrustumPlaneEquationsd(PE,M,N)
#   define chm_IsOBBVisible(PE,M,AMINX,AMINY,AMINZ,AMAXX,AMAXY,AMAXZ)  chm_IsOBBVisibled(PE,M,AMINX,AMINY,AMINZ,AMAXX,AMAXY,AMAXZ)
#   define chm_GetFrustumPoints(FP,MINV)                                chm_GetFrustumPointsd(FP,MINV)
#   define chm_GetFrustumAabbCenterAndHalfExtents(CO,HEO,FP)            chm_GetFrustumAabbCenterAndHalfExtentsd(CO,HEO,FP)
#   define chm_GetFrustumRadiusAndCenterDistance(CDO,N,F,FD,AR,CTDN)    chm_GetFrustumRadiusAndCenterDistanced(CDO,N,F,FD,AR,CTDN)
#   define chm_GetFrustumCenterFromCenterDistance(CO,VINV,FDO)          chm_GetFrustumCenterFromCenterDistanced(CO,VINV,FDO)
#   define chm_GetLightViewProjectionMatrixExtra(LVPMO,VINV,C,R,LD,W,H,OPINV,OVO,OFPO,OLVPMOFC) chm_GetLightViewProjectionMatrixExtrad(LVPMO,VINV,C,R,LD,W,H,OPINV,OVO,OFPO,OLVPMOFC)
#   define chm_GetLightViewProjectionMatrix(LVPMO,VINV,C,R,LD,W,H)      chm_GetLightViewProjectionMatrixd(LVPMO,VINV,C,R,LD,W,H)
#   ifndef CHA_NO_STDIO
#       define chm_Mat4_fprintp(M,F,W,P)                chm_Mat4_fprintpd(M,F,W,P)
#       define chm_Mat4_fprint(M,F)                     chm_Mat4_fprintd(M,F)
#       define chm_Mat4_printp(M,W,P)                   chm_Mat4_printpd(M,W,P)
#       define chm_Mat4_print(M)                        chm_Mat4_printd(M)
#   endif
#endif /*CHA_DOUBLE_PRECISION*/
#endif //CHA_NO_MATH_MACROS

CHA_API_INL float chm_Vec3Dotf(const float* a3,const float* b3) {return a3[0]*b3[0]+a3[1]*b3[1]+a3[2]*b3[2];}
CHA_API_INL double chm_Vec3Dotd(const double* a3,const double* b3) {return a3[0]*b3[0]+a3[1]*b3[1]+a3[2]*b3[2];}
CHA_API_INL void chm_Vec3Normalizef(float* CHA_RESTRICT v3) {
    float len = chm_Vec3Dotf(v3,v3);int i;
    if (len!=0) {len = sqrt(len);for (i=0;i<3;i++) v3[i]/=len;}
}
CHA_API_INL void chm_Vec3Normalized(double* CHA_RESTRICT v3) {
    double len = chm_Vec3Dotd(v3,v3);int i;
    if (len!=0) {len = sqrt(len);for (i=0;i<3;i++) v3[i]/=len;}
}
CHA_API_INL void chm_Vec3Crossf(float* CHA_RESTRICT vOut3,const float* CHA_RESTRICT a3,const float* CHA_RESTRICT b3) {
    vOut3[0] =	a3[1] * b3[2] - a3[2] * b3[1];
    vOut3[1] =	a3[2] * b3[0] - a3[0] * b3[2];
    vOut3[2] =	a3[0] * b3[1] - a3[1] * b3[0];
}
CHA_API_INL void chm_Vec3Crossd(double* CHA_RESTRICT vOut3,const double* CHA_RESTRICT a3,const double* CHA_RESTRICT b3) {
    vOut3[0] =	a3[1] * b3[2] - a3[2] * b3[1];
    vOut3[1] =	a3[2] * b3[0] - a3[0] * b3[2];
    vOut3[2] =	a3[0] * b3[1] - a3[1] * b3[0];
}
CHA_API_INL float chm_Vec3DistSquaredf(const float* CHA_RESTRICT a3,const float* CHA_RESTRICT b3) {
    const float rv[3] = {b3[0]-a3[0],b3[1]-a3[1],b3[2]-a3[2]};
    return rv[0]*rv[0] + rv[1]*rv[1] + rv[2]*rv[2];
}
CHA_API_INL double chm_Vec3DistSquaredd(const double* CHA_RESTRICT a3,const double* CHA_RESTRICT b3) {
    const double rv[3] = {b3[0]-a3[0],b3[1]-a3[1],b3[2]-a3[2]};
    return rv[0]*rv[0] + rv[1]*rv[1] + rv[2]*rv[2];
}
CHA_API_INL float chm_Vec3Distf(const float* CHA_RESTRICT a3,const float* CHA_RESTRICT b3) {
    const float res = chm_Vec3DistSquaredf(a3,b3);
    return res!=0 ? sqrtf(res) : 0;
}
CHA_API_INL double chm_Vec3Distd(const double* CHA_RESTRICT a3,const double* CHA_RESTRICT b3) {
    const double res = chm_Vec3DistSquaredd(a3,b3);
    return res!=0 ? sqrt(res) : 0;
}
CHA_API_INL float chm_Roundf(float number)	{return number < 0.0f ? ceilf(number - 0.5f) : floorf(number + 0.5f);}
CHA_API_INL float chm_Roundd(double number)	{return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);}

CHA_API_INL float* chm_Mat4Identityf(float* CHA_RESTRICT result16) {float* m = result16;m[0]=m[5]=m[10]=m[15]=1.f;m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=0.f;return result16;}
CHA_API_INL double* chm_Mat4Identityd(double* CHA_RESTRICT result16) {double* m = result16;m[0]=m[5]=m[10]=m[15]=1.0;m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=0.0;return result16;}
CHA_API_INL void chm_Mat4Copyf(float* CHA_RESTRICT dst16,const float* CHA_RESTRICT src16) {memcpy(dst16,src16,16*sizeof(float));}
CHA_API_INL void chm_Mat4Copyd(double* CHA_RESTRICT dst16,const double* CHA_RESTRICT src16) {memcpy(dst16,src16,16*sizeof(double));}
CHA_API_DEC float* chm_Mat4MulUncheckArgsf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16);
CHA_API_DEC double* chm_Mat4MulUncheckArgsd(double* CHA_RESTRICT result16,const double* CHA_RESTRICT ml16,const double* CHA_RESTRICT mr16);
CHA_API_DEC float* chm_Mat4Mulf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16);
CHA_API_DEC double* chm_Mat4Muld(double* CHA_RESTRICT result16,const double* CHA_RESTRICT ml16,const double* CHA_RESTRICT mr16);
CHA_API_INL void chm_Mat4MulDirf(const float* CHA_RESTRICT m16,float* CHA_RESTRICT dirOut3,const float dirX,float dirY,float dirZ)  {
    dirOut3[0] = dirX*m16[0] + dirY*m16[4] + dirZ*m16[8];
    dirOut3[1] = dirX*m16[1] + dirY*m16[5] + dirZ*m16[9];
    dirOut3[2] = dirX*m16[2] + dirY*m16[6] + dirZ*m16[10];
}
CHA_API_INL void chm_Mat4MulDird(const double* CHA_RESTRICT m16,double* CHA_RESTRICT dirOut3,const double dirX,double dirY,double dirZ)  {
    dirOut3[0] = dirX*m16[0] + dirY*m16[4] + dirZ*m16[8];
    dirOut3[1] = dirX*m16[1] + dirY*m16[5] + dirZ*m16[9];
    dirOut3[2] = dirX*m16[2] + dirY*m16[6] + dirZ*m16[10];
}
CHA_API_INL void chm_Mat4MulPosf(const float* CHA_RESTRICT m16,float* CHA_RESTRICT posOut3,const float posX,float posY,float posZ)  {
    posOut3[0] = posX*m16[0] + posY*m16[4] + posZ*m16[8] + m16[12];
    posOut3[1] = posX*m16[1] + posY*m16[5] + posZ*m16[9] + m16[13];
    posOut3[2] = posX*m16[2] + posY*m16[6] + posZ*m16[10]+ m16[14];
}
CHA_API_INL void chm_Mat4MulPosd(const double* CHA_RESTRICT m16,double* CHA_RESTRICT posOut3,const double posX,double posY,double posZ)  {
    posOut3[0] = posX*m16[0] + posY*m16[4] + posZ*m16[8] + m16[12];
    posOut3[1] = posX*m16[1] + posY*m16[5] + posZ*m16[9] + m16[13];
    posOut3[2] = posX*m16[2] + posY*m16[6] + posZ*m16[10]+ m16[14];
}
CHA_API_DEC void chm_Mat4LookAtf(float* CHA_RESTRICT mOut16,float eyeX,float eyeY,float eyeZ,float centerX,float centerY,float centerZ,float upX,float upY,float upZ);
CHA_API_DEC void chm_Mat4LookAtd(double* CHA_RESTRICT mOut16,double eyeX,double eyeY,double eyeZ,double centerX,double centerY,double centerZ,double upX,double upY,double upZ);
CHA_API_DEC void chm_Mat4Perspectivef(float* CHA_RESTRICT mOut16,float degfovy,float aspect, float zNear, float zFar);
CHA_API_DEC void chm_Mat4Perspectived(double* CHA_RESTRICT mOut16,double degfovy,double aspect, double zNear, double zFar);
CHA_API_DEC void chm_Mat4Orthof(float* CHA_RESTRICT mOut16,float left,float right, float bottom, float top,float nearVal,float farVal);
CHA_API_DEC void chm_Mat4Orthod(double* CHA_RESTRICT mOut16,double left,double right, double bottom, double top,double nearVal,double farVal);
CHA_API_DEC void chm_Mat4Ortho3Df(float* CHA_RESTRICT mOut16,float cameraTargetDistance,float degfovy,float aspect,float znear,float zfar);
CHA_API_DEC void chm_Mat4Ortho3Dd(double* CHA_RESTRICT mOut16,double cameraTargetDistance,double degfovy,double aspect,double znear,double zfar);
CHA_API_INL float* chm_Mat4Translatef(float* CHA_RESTRICT mInOut16,float x,float y,float z) {int i;for (i=0;i<3;i++) mInOut16[12+i]+=mInOut16[i]*x+mInOut16[4+i]*y+mInOut16[8+i]*z;return mInOut16;}
CHA_API_INL double* chm_Mat4Translated(double* CHA_RESTRICT mInOut16,double x,double y,double z) {int i;for (i=0;i<3;i++) mInOut16[12+i]+=mInOut16[i]*x+mInOut16[4+i]*y+mInOut16[8+i]*z;return mInOut16;}
CHA_API_DEC float* chm_Mat4Rotatef(float* CHA_RESTRICT mInOut16,float degAngle,float x,float y,float z);
CHA_API_DEC double* chm_Mat4Rotated(double* CHA_RESTRICT mInOut16,double degAngle,double x,double y,double z);
CHA_API_INL float* chm_Mat4Scalef(float* CHA_RESTRICT mInOut16,float x,float y,float z) {int i;for (i=0;i<3;i++) {mInOut16[i]*=x;mInOut16[4+i]*=y;mInOut16[8+i]*=z;} return mInOut16;}
CHA_API_INL double* chm_Mat4Scaled(double* CHA_RESTRICT mInOut16,double x,double y,double z) {int i;for (i=0;i<3;i++) {mInOut16[i]*=x;mInOut16[4+i]*=y;mInOut16[8+i]*=z;} return mInOut16;}
CHA_API_DEC int chm_Mat4Invertf(float* CHA_RESTRICT mOut16,const float* CHA_RESTRICT m16);
CHA_API_DEC int chm_Mat4Invertd(double* CHA_RESTRICT mOut16,const double* CHA_RESTRICT m16);
CHA_API_DEC void chm_Mat4InvertTransformMatrixf(float* CHA_RESTRICT mOut16,const float* CHA_RESTRICT m16);
CHA_API_DEC void chm_Mat4InvertTransformMatrixd(double* CHA_RESTRICT mOut16,const double* CHA_RESTRICT m16);
CHA_API_DEC void chm_Mat4InvertTransformMatrixFastf(float* CHA_RESTRICT mOut16,const float* CHA_RESTRICT m16);
CHA_API_DEC void chm_Mat4InvertTransformMatrixFastd(double* CHA_RESTRICT mOut16,const double* CHA_RESTRICT m16);
CHA_API_INL void chm_Mat4Convertd2f(float* CHA_RESTRICT result16,const double* CHA_RESTRICT m16) {int i;for(i = 0; i < 16; i++) result16[i]=(float)m16[i];}
CHA_API_INL void chm_Mat4Convertf2d(double* CHA_RESTRICT result16,const float* CHA_RESTRICT m16) {int i;for(i = 0; i < 16; i++) result16[i]=(double)m16[i];}

CHA_API_DEC void chm_PlaneNormalizef(float* CHA_RESTRICT p4);
CHA_API_DEC void chm_PlaneNormalized(double* CHA_RESTRICT p4);
CHA_API_DEC void chm_GetFrustumPlaneEquationsf(float planeEquationsOut[6][4],const float* CHA_RESTRICT vpMatrix16,int normalizePlanes);
CHA_API_DEC void chm_GetFrustumPlaneEquationsd(double planeEquationsOut[6][4],const double* CHA_RESTRICT vpMatrix16,int normalizePlanes);
CHA_API_DEC int chm_IsOBBVisiblef(const float frustumPlanes[6][4],const float* CHA_RESTRICT mfMatrix16,float aabbMinX,float aabbMinY,float aabbMinZ,float aabbMaxX,float aabbMaxY,float aabbMaxZ);
CHA_API_DEC int chm_IsOBBVisibled(const double frustumPlanes[6][4],const double* CHA_RESTRICT mfMatrix16,double aabbMinX,double aabbMinY,double aabbMinZ,double aabbMaxX,double aabbMaxY,double aabbMaxZ);
// shadow map helpers
CHA_API_DEC void chm_GetFrustumPointsf(float frustumPoints[8][4],const float* CHA_RESTRICT vpMatrixInverse16);
CHA_API_DEC void chm_GetFrustumPointsd(double frustumPoints[8][4],const double* CHA_RESTRICT vpMatrixInverse16);
CHA_API_DEC void chm_GetFrustumAabbCenterAndHalfExtentsf(float* CHA_RESTRICT frustumCenterOut3,float* CHA_RESTRICT frustumHalfExtentsOut3,const float frustumPoints[8][4]);
CHA_API_DEC void chm_GetFrustumAabbCenterAndHalfExtentsd(double* CHA_RESTRICT frustumCenterOut3,double* CHA_RESTRICT frustumHalfExtentsOut3,const double frustumPoints[8][4]);
CHA_API_DEC float chm_GetFrustumRadiusAndCenterDistancef(float* CHA_RESTRICT pFrustumCenterDistanceOut,float cameraNearClippingPlane,float cameraFarClippingPlane,float cameraFovyDeg,float cameraAspectRatio,float cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero);
CHA_API_DEC double chm_GetFrustumRadiusAndCenterDistanced(double* CHA_RESTRICT pFrustumCenterDistanceOut,double cameraNearClippingPlane,double cameraFarClippingPlane,double cameraFovyDeg,double cameraAspectRatio,double cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero);
CHA_API_DEC void chm_GetFrustumCenterFromCenterDistancef(float* CHA_RESTRICT frustumCenterOut3,const float* CHA_RESTRICT cameraVMatrixInverse16,float frustumCenterDistance);
CHA_API_DEC void chm_GetFrustumCenterFromCenterDistanced(double* CHA_RESTRICT frustumCenterOut3,const double* CHA_RESTRICT cameraVMatrixInverse16,double frustumCenterDistance);
CHA_API_DEC void chm_GetLightViewProjectionMatrixExtraf(float* CHA_RESTRICT lvpMatrixOut16
                                                          ,const float* CHA_RESTRICT cameraVMatrixInverse16,const float* CHA_RESTRICT cameraFrustumCenterInWorldSpace,float cameraFrustumRadius
                                                          ,const float*  CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight
                                                          ,const float* CHA_RESTRICT optionalCameraPMatrixInverse16
                                                          ,float* CHA_RESTRICT optionalLightViewportClippingOut4,float optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,float* CHA_RESTRICT optionalLVPMatrixForFrustumCullingUsageOut16   // Highly experimental and untested
                                                          );
CHA_API_DEC void chm_GetLightViewProjectionMatrixExtrad(double* CHA_RESTRICT lvpMatrixOut16
                                                          ,const double* CHA_RESTRICT cameraVMatrixInverse16,const double* CHA_RESTRICT cameraFrustumCenterInWorldSpace,double cameraFrustumRadius
                                                          ,const double*  CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight
                                                          ,const double* CHA_RESTRICT optionalCameraPMatrixInverse16
                                                          ,double* CHA_RESTRICT optionalLightViewportClippingOut4,double optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,double* CHA_RESTRICT optionalLVPMatrixForFrustumCullingUsageOut16   // Highly experimental and untested
                                                          );
CHA_API_DEC void chm_GetLightViewProjectionMatrixf(float* CHA_RESTRICT lvpMatrixOut16
                                                         ,const float* CHA_RESTRICT cameraVMatrixInverse16,const float* CHA_RESTRICT cameraFrustumCenterInWorldSpace,float cameraFrustumRadius
                                                         ,const float* CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight);
CHA_API_DEC void chm_GetLightViewProjectionMatrixd(double* CHA_RESTRICT lvpMatrixOut16
                                                         ,const double* CHA_RESTRICT cameraVMatrixInverse16,const double* CHA_RESTRICT cameraFrustumCenterInWorldSpace,double cameraFrustumRadius
                                                         ,const double* CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight);
// end shadow map helpers

//---------------- single-precision-only math -------------------------------------------------------------
CHA_API_INL void chm_Mat4MulPosWithWDivisionf(const float* CHA_RESTRICT m16,float* CHA_RESTRICT posOut3,const float posX,float posY,float posZ) {
    float w;
    posOut3[0] = posX*m16[0] + posY*m16[4] + posZ*m16[8] + m16[12];
    posOut3[1] = posX*m16[1] + posY*m16[5] + posZ*m16[9] + m16[13];
    posOut3[2] = posX*m16[2] + posY*m16[6] + posZ*m16[10]+ m16[14];
    w          = posX*m16[3] + posY*m16[7] + posZ*m16[11]+ m16[15];
    if (w!=0 && w!=1) {posOut3[0]/=w;posOut3[1]/=w;posOut3[2]/=w;}
}
CHA_API_DEC int chm_UnProject_MvpMatrixInvf(float winX,float winY,float winZ,const float* CHA_RESTRICT mvpMatrixInv16,const int* viewport4,float* objX,float* objY,float* objZ);
CHA_API_DEC void chm_GetRayFromMouseCoordsf(float* rayOriginOut3,float* rayDirOut3,int mouseX,int mouseY,const float* vpMatrixInv,const int* viewport4);
CHA_API_INL float* chm_Mat4Sumf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16)    {int i;for(i=0;i<16;i++) {result16[i]=ml16[i]+mr16[i];}return result16;}
CHA_API_INL float* chm_Mat4SumInPlacef(float* CHA_RESTRICT mInOut16,const float* CHA_RESTRICT m16)    {int i;for(i=0;i<16;i++) {mInOut16[i]+=m16[i];}return mInOut16;}
CHA_API_INL float* chm_Mat4Subf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16)    {int i;for(i=0;i<16;i++) {result16[i]=ml16[i]-mr16[i];}return result16;}
CHA_API_INL float* chm_Mat4SubInPlacef(float* CHA_RESTRICT mInOut16,const float* CHA_RESTRICT m16)    {int i;for(i=0;i<16;i++) {mInOut16[i]-=m16[i];}return mInOut16;}
CHA_API_INL float* chm_Mat4FMAf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16,float sca)  {int i;for(i=0;i<16;i++) {result16[i]=ml16[i]+mr16[i]*sca;}return result16;}
CHA_API_INL float* chm_Mat4FMAInPlacef(float* CHA_RESTRICT mInOut16,const float* CHA_RESTRICT m16,float sca)  {int i;for(i=0;i<16;i++) {mInOut16[i]+=m16[i]*sca;}return mInOut16;}
#ifndef CHA_NO_STDIO
CHA_API_DEC void chm_Mat4_fprintpf(const float* CHA_RESTRICT m16,FILE* stream,int width, int precision);
CHA_API_DEC void chm_Mat4_fprintf(const float* CHA_RESTRICT m16,FILE* stream);
CHA_API_DEC void chm_Mat4_printpf(const float* CHA_RESTRICT m16,int width, int precision);
CHA_API_DEC void chm_Mat4_printf(const float* CHA_RESTRICT m16);
CHA_API_DEC void chm_Mat4_fprintpd(const double* CHA_RESTRICT m16,FILE* stream,int width, int precision);
CHA_API_DEC void chm_Mat4_fprintd(const double* CHA_RESTRICT m16,FILE* stream);
CHA_API_DEC void chm_Mat4_printpd(const double* CHA_RESTRICT m16,int width, int precision);
CHA_API_DEC void chm_Mat4_printd(const double* CHA_RESTRICT m16);
#endif
#ifndef CHA_SLERP_EPSILON
#   define CHA_SLERP_EPSILON (0.0001f)
#endif //CHA_SLERP_EPSILON
CHA_API_INL float chm_QuatLength(const float* CHA_RESTRICT q4) {return sqrtf(q4[0]*q4[0]+q4[1]*q4[1]+q4[2]*q4[2]+q4[3]*q4[3]);}
CHA_API_INL float* chm_QuatNormalized(float* CHA_RESTRICT result4,const float* CHA_RESTRICT q4) {float *r=result4;const float *q=q4;const float len=chm_QuatLength(q4);if (len>0) {r[0]=q[0]/len;r[1]=q[1]/len;r[2]=q[2]/len;r[3]=q[3]/len;} else {r[3]=(float)1;r[0]=r[1]=r[2]=(float)0;} return r;}
CHA_API_INL void chm_QuatNormalize(float* CHA_RESTRICT q4) {const float len=chm_QuatLength(q4);if (len>0) {q4[0]/=len;q4[1]/=len;q4[2]/=len;q4[3]/=len;} else {q4[0]=q4[1]=q4[2]=q4[3]=(float)0;}}
CHA_API_DEC float* chm_QuatSlerpEps(float* CHA_RESTRICT result4,const float* CHA_RESTRICT a4,const float* CHA_RESTRICT b4,float slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/,float eps/*= CHA_SLERP_EPSILON*/);
CHA_API_DEC float* chm_QuatSlerp(float* CHA_RESTRICT result4,const float* CHA_RESTRICT a4,const float* CHA_RESTRICT b4,float slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/);
CHA_API_DEC float* chm_QuatFromMat4(float* CHA_RESTRICT result4,const float* CHA_RESTRICT m16);
CHA_API_DEC float* chm_Mat4SetRotationFromQuat(float* CHA_RESTRICT result16,const float* CHA_RESTRICT q4);
//--------------------------------------------------------------------------



#endif /*CHARACTER_H_ */







#ifdef CHARACTER_IMPLEMENTATION
#ifndef CHA_IMPLEMENTATION_GUARD
#define CHA_IMPLEMENTATION_GUARD


#ifndef CHA_MALLOC
#   undef CHA_NO_STDLIB  /* stdlib cannot be avoided in this case */
#   include <stdlib.h>	 /* malloc/realloc/free */
#   define CHA_MALLOC(X) malloc(X)
#endif
#ifndef CHA_FREE
#   define CHA_FREE(X) free(X)
#endif
#ifndef CHA_REALLOC
#   define CHA_REALLOC(x,y) realloc((x),(y))
#endif
#include <stddef.h> /* size_t */
#ifndef CHA_ASSERT
#   ifdef CHA_NO_ASSERT
#       define CHA_ASSERT(X) /*no-op*/
#   else
#       include <assert.h>
#       define CHA_ASSERT(X) assert((X))
#   endif
#endif
#ifndef CHA_NO_STDLIB
#   include <stdlib.h> /*exit*/
#endif
#include <string.h> /*memcpy,memmove,memset*/

#ifndef __cplusplus
#define CHA_ZERO_INIT {0}
#else
#define CHA_ZERO_INIT {}
#endif

#ifndef CHA_LIGHTNESS
#   define CHA_LIGHTNESS 1.0f
#endif

/* base memory helpers */
CHA_API_PRIV void* cha_malloc(size_t size) {
    void* p = CHA_MALLOC(size);
    if (!p)	{
        CHA_ASSERT(0);	/* No more memory error */
#       ifndef CHA_NO_STDIO
        fprintf(stderr,"CHA_ERROR: cha_malloc(...) failed. Not enough memory.\n");
#       endif
#       ifndef CHA_NO_STDLIB
        exit(1);
#       endif
    }
    return p;
}
CHA_API_PRIV void cha_free(void* p)                         {CHA_FREE(p);}
CHA_API_PRIV void* cha_safe_realloc(void** const ptr, size_t new_size)  {
    void *ptr2 = CHA_REALLOC(*ptr,new_size);
    CHA_ASSERT(new_size!=0);    /* undefined behaviour */
    if (ptr2) *ptr=ptr2;
    else {
        CHA_FREE(*ptr);*ptr=NULL;
        CHA_ASSERT(0);	/* No more memory error */
#       ifndef CHA_NO_STDIO
        fprintf(stderr,"CHA_ERROR: cha_safe_realloc(...) failed. Not enough memory.\n");
#       endif
#       ifndef CHA_NO_STDLIB
        exit(1);
#       endif
    }
    return ptr2;
}

#include <math.h>

#ifdef CHA_USE_SIMD
#if (!defined(__SSE__) && (defined(_MSC_VER) && defined(_M_IX86_FP) && _M_IX86_FP>0))
#define __SSE__		// _MSC_VER does not always define it... (but it always defines __AVX__)
#endif // __SSE__
#ifndef CHA_DOUBLE_PRECISION
#ifdef __SSE__
#include <xmmintrin.h>	// SSE
#endif //__SSE__
#else // CHA_DOUBLE_PRECISION
#ifdef __AVX__
#include <immintrin.h>	// AVX (and everything)
#endif //__AVX__
#endif // CHA_DOUBLE_PRECISION
#endif //CHA_USE_SIMD

extern float chm_Roundf(float number);
extern float chm_Roundd(double number);

extern float chm_Vec3Dotf(const float* a3,const float* b3);
extern double chm_Vec3Dotd(const double* a3,const double* b3);
extern void chm_Vec3Normalizef(float* CHA_RESTRICT v3);
extern void chm_Vec3Normalized(double* CHA_RESTRICT v3);
extern void chm_Vec3Crossf(float* CHA_RESTRICT vOut3,const float* CHA_RESTRICT a3,const float* CHA_RESTRICT b3);
extern void chm_Vec3Crossd(double* CHA_RESTRICT vOut3,const double* CHA_RESTRICT a3,const double* CHA_RESTRICT b3);
extern float chm_Vec3DistSquaredf(const float* CHA_RESTRICT a3,const float* CHA_RESTRICT b3);
extern double chm_Vec3DistSquaredd(const double* CHA_RESTRICT a3,const double* CHA_RESTRICT b3);
extern float chm_Vec3Distf(const float* CHA_RESTRICT a3,const float* CHA_RESTRICT b3);
extern double chm_Vec3Distd(const double* CHA_RESTRICT a3,const double* CHA_RESTRICT b3);

extern void chm_Mat4Convertd2f(float* CHA_RESTRICT result16,const double* CHA_RESTRICT m16);
extern void chm_Mat4Convertf2d(double* CHA_RESTRICT result16,const float* CHA_RESTRICT m16);
extern float* chm_Mat4Identityf(float* CHA_RESTRICT result16);
extern double* chm_Mat4Identityd(double* CHA_RESTRICT result16);
extern void chm_Mat4Copyf(float* CHA_RESTRICT dst16,const float* CHA_RESTRICT src16);
extern void chm_Mat4Copyd(double* CHA_RESTRICT dst16,const double* CHA_RESTRICT src16);
CHA_API_DEF float* chm_Mat4MulUncheckArgsf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16) {
    int i,i4;
#	if (defined(CHA_USE_SIMD) && defined(__SSE__))
    __m128 row1 = _mm_loadu_ps(&ml16[0]);
    __m128 row2 = _mm_loadu_ps(&ml16[4]);
    __m128 row3 = _mm_loadu_ps(&ml16[8]);
    __m128 row4 = _mm_loadu_ps(&ml16[12]);
    for(i=0; i<4; i++) {
        i4 = 4*i;
        __m128 brod1 = _mm_set1_ps(mr16[i4]);
        __m128 brod2 = _mm_set1_ps(mr16[i4 + 1]);
        __m128 brod3 = _mm_set1_ps(mr16[i4 + 2]);
        __m128 brod4 = _mm_set1_ps(mr16[i4 + 3]);
        __m128 row = _mm_add_ps(
                    _mm_add_ps(
                        _mm_mul_ps(brod1, row1),
                        _mm_mul_ps(brod2, row2)),
                    _mm_add_ps(
                        _mm_mul_ps(brod3, row3),
                        _mm_mul_ps(brod4, row4)));
        _mm_storeu_ps(&result16[i4], row);
    }
#	else
    /* reference implementation */
    float mri4plus0,mri4plus1,mri4plus2,mri4plus3;
    for(i = 0; i < 4; i++) {
        i4=4*i;mri4plus0=mr16[i4];mri4plus1=mr16[i4+1];mri4plus2=mr16[i4+2];mri4plus3=mr16[i4+3];
        result16[  i4] = ml16[0]*mri4plus0 + ml16[4]*mri4plus1 + ml16[ 8]*mri4plus2 + ml16[12]*mri4plus3;
        result16[1+i4] = ml16[1]*mri4plus0 + ml16[5]*mri4plus1 + ml16[ 9]*mri4plus2 + ml16[13]*mri4plus3;
        result16[2+i4] = ml16[2]*mri4plus0 + ml16[6]*mri4plus1 + ml16[10]*mri4plus2 + ml16[14]*mri4plus3;
        result16[3+i4] = ml16[3]*mri4plus0 + ml16[7]*mri4plus1 + ml16[11]*mri4plus2 + ml16[15]*mri4plus3;
    }
#	endif //CHA_DOUBLE_PRECISION
    return result16;
}
CHA_API_DEF double* chm_Mat4MulUncheckArgsd(double* CHA_RESTRICT result16,const double* CHA_RESTRICT ml16,const double* CHA_RESTRICT mr16) {
    int i,i4;
#if (defined(CHA_USE_SIMD) && defined(__AVX__))
    __m256d row1 = _mm256_loadu_pd(&ml16[0]);
    __m256d row2 = _mm256_loadu_pd(&ml16[4]);
    __m256d row3 = _mm256_loadu_pd(&ml16[8]);
    __m256d row4 = _mm256_loadu_pd(&ml16[12]);
    for(i=0; i<4; i++) {
        i4 = 4*i;
        __m256d brod1 = _mm256_set1_pd(mr16[i4]);
        __m256d brod2 = _mm256_set1_pd(mr16[i4 + 1]);
        __m256d brod3 = _mm256_set1_pd(mr16[i4 + 2]);
        __m256d brod4 = _mm256_set1_pd(mr16[i4 + 3]);
        __m256d row = _mm256_add_pd(
                    _mm256_add_pd(
                        _mm256_mul_pd(brod1, row1),
                        _mm256_mul_pd(brod2, row2)),
                    _mm256_add_pd(
                        _mm256_mul_pd(brod3, row3),
                        _mm256_mul_pd(brod4, row4)));
        _mm256_storeu_pd(&result16[i4], row);
    }
#	else
    /* reference implementation */
    double mri4plus0,mri4plus1,mri4plus2,mri4plus3;
    for(i = 0; i < 4; i++) {
        i4=4*i;mri4plus0=mr16[i4];mri4plus1=mr16[i4+1];mri4plus2=mr16[i4+2];mri4plus3=mr16[i4+3];
        result16[  i4] = ml16[0]*mri4plus0 + ml16[4]*mri4plus1 + ml16[ 8]*mri4plus2 + ml16[12]*mri4plus3;
        result16[1+i4] = ml16[1]*mri4plus0 + ml16[5]*mri4plus1 + ml16[ 9]*mri4plus2 + ml16[13]*mri4plus3;
        result16[2+i4] = ml16[2]*mri4plus0 + ml16[6]*mri4plus1 + ml16[10]*mri4plus2 + ml16[14]*mri4plus3;
        result16[3+i4] = ml16[3]*mri4plus0 + ml16[7]*mri4plus1 + ml16[11]*mri4plus2 + ml16[15]*mri4plus3;
    }
#	endif
    return result16;
}
CHA_API_DEF float* chm_Mat4Mulf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16) {
    if (result16==ml16) {
        float ML16[16];chm_Mat4Copyf(ML16,ml16);
        return chm_Mat4MulUncheckArgsf(result16,ML16,mr16);
    }
    else if (result16==mr16) {
        float MR16[16];chm_Mat4Copyf(MR16,mr16);
        return chm_Mat4MulUncheckArgsf(result16,ml16,MR16);
    }
    return chm_Mat4MulUncheckArgsf(result16,ml16,mr16);
}
CHA_API_DEF double* chm_Mat4Muld(double* CHA_RESTRICT result16,const double* CHA_RESTRICT ml16,const double* CHA_RESTRICT mr16) {
    if (result16==ml16) {
        double ML16[16];chm_Mat4Copyd(ML16,ml16);
        return chm_Mat4MulUncheckArgsd(result16,ML16,mr16);
    }
    else if (result16==mr16) {
        double MR16[16];chm_Mat4Copyd(MR16,mr16);
        return chm_Mat4MulUncheckArgsd(result16,ml16,MR16);
    }
    return chm_Mat4MulUncheckArgsd(result16,ml16,mr16);
}
extern void chm_Mat4MulDirf(const float* CHA_RESTRICT m16,float* CHA_RESTRICT dirOut3,const float dirX,float dirY,float dirZ);
extern void chm_Mat4MulDird(const double* CHA_RESTRICT m16,double* CHA_RESTRICT dirOut3,const double dirX,double dirY,double dirZ);
extern void chm_Mat4MulPosf(const float* CHA_RESTRICT m16,float* CHA_RESTRICT posOut3,const float posX,float posY,float posZ);
extern void chm_Mat4MulPosd(const double* CHA_RESTRICT m16,double* CHA_RESTRICT posOut3,const double posX,double posY,double posZ);
CHA_API_DEF void chm_Mat4LookAtf(float* CHA_RESTRICT mOut16,float eyeX,float eyeY,float eyeZ,float centerX,float centerY,float centerZ,float upX,float upY,float upZ)   {
    float* m = mOut16;
    const float eps = 0.0001f;

    float F[3] = {eyeX-centerX,eyeY-centerY,eyeZ-centerZ};
    float length = F[0]*F[0]+F[1]*F[1]+F[2]*F[2];	// length2 now
    float up[3] = {upX,upY,upZ};

    float S[3] = {up[1]*F[2]-up[2]*F[1],up[2]*F[0]-up[0]*F[2],up[0]*F[1]-up[1]*F[0]};
    float U[3] = {F[1]*S[2]-F[2]*S[1],F[2]*S[0]-F[0]*S[2],F[0]*S[1]-F[1]*S[0]};

    if (length==0) length = eps;
    length = sqrt(length);
    F[0]/=length;F[1]/=length;F[2]/=length;

    length = S[0]*S[0]+S[1]*S[1]+S[2]*S[2];if (length==0) length = eps;
    length = sqrt(length);
    S[0]/=length;S[1]/=length;S[2]/=length;

    length = U[0]*U[0]+U[1]*U[1]+U[2]*U[2];if (length==0) length = eps;
    length = sqrt(length);
    U[0]/=length;U[1]/=length;U[2]/=length;

    m[0] = S[0];
    m[1] = U[0];
    m[2] = F[0];
    m[3]= 0;

    m[4] = S[1];
    m[5] = U[1];
    m[6] = F[1];
    m[7]= 0;

    m[8] = S[2];
    m[9] = U[2];
    m[10]= F[2];
    m[11]= 0;

    m[12] = -S[0]*eyeX -S[1]*eyeY -S[2]*eyeZ;
    m[13] = -U[0]*eyeX -U[1]*eyeY -U[2]*eyeZ;
    m[14]= -F[0]*eyeX -F[1]*eyeY -F[2]*eyeZ;
    m[15]= 1;
}
CHA_API_DEF void chm_Mat4LookAtd(double* CHA_RESTRICT mOut16,double eyeX,double eyeY,double eyeZ,double centerX,double centerY,double centerZ,double upX,double upY,double upZ) {
    double* m = mOut16;
    const double eps = 0.0001;

    double F[3] = {eyeX-centerX,eyeY-centerY,eyeZ-centerZ};
    double length = F[0]*F[0]+F[1]*F[1]+F[2]*F[2];	// length2 now
    double up[3] = {upX,upY,upZ};

    double S[3] = {up[1]*F[2]-up[2]*F[1],up[2]*F[0]-up[0]*F[2],up[0]*F[1]-up[1]*F[0]};
    double U[3] = {F[1]*S[2]-F[2]*S[1],F[2]*S[0]-F[0]*S[2],F[0]*S[1]-F[1]*S[0]};

    if (length==0) length = eps;
    length = sqrt(length);
    F[0]/=length;F[1]/=length;F[2]/=length;

    length = S[0]*S[0]+S[1]*S[1]+S[2]*S[2];if (length==0) length = eps;
    length = sqrt(length);
    S[0]/=length;S[1]/=length;S[2]/=length;

    length = U[0]*U[0]+U[1]*U[1]+U[2]*U[2];if (length==0) length = eps;
    length = sqrt(length);
    U[0]/=length;U[1]/=length;U[2]/=length;

    m[0] = S[0];
    m[1] = U[0];
    m[2] = F[0];
    m[3]= 0;

    m[4] = S[1];
    m[5] = U[1];
    m[6] = F[1];
    m[7]= 0;

    m[8] = S[2];
    m[9] = U[2];
    m[10]= F[2];
    m[11]= 0;

    m[12] = -S[0]*eyeX -S[1]*eyeY -S[2]*eyeZ;
    m[13] = -U[0]*eyeX -U[1]*eyeY -U[2]*eyeZ;
    m[14]= -F[0]*eyeX -F[1]*eyeY -F[2]*eyeZ;
    m[15]= 1;
}
CHA_API_DEF void chm_Mat4Perspectivef(float* CHA_RESTRICT mOut16,float degfovy,float aspect, float zNear, float zFar)   {
    float* res = mOut16;
    const float eps = 0.0001f;
    float f = 1.f/tanf(degfovy*1.5707963268f/180.0f); //cotg
    float Dfn = (zFar-zNear);
    if (Dfn==0) {zFar+=eps;zNear-=eps;Dfn=zFar-zNear;}
    if (aspect==0) aspect = 1.f;

    res[0]  = f/aspect;
    res[1]  = 0;
    res[2]  = 0;
    res[3]  = 0;

    res[4]  = 0;
    res[5]  = f;
    res[6]  = 0;
    res[7] = 0;

    res[8]  = 0;
    res[9]  = 0;
    res[10] = -(zFar+zNear)/Dfn;
    res[11] = -1;

    res[12]  = 0;
    res[13]  = 0;
    res[14] = -2.f*zFar*zNear/Dfn;
    res[15] = 0;
}
CHA_API_DEF void chm_Mat4Perspectived(double* CHA_RESTRICT mOut16,double degfovy,double aspect, double zNear, double zFar)  {
    double* res = mOut16;
    const double eps = 0.0001;
    double f = 1.f/tan(degfovy*1.5707963268/180.0); //cotg
    double Dfn = (zFar-zNear);
    if (Dfn==0) {zFar+=eps;zNear-=eps;Dfn=zFar-zNear;}
    if (aspect==0) aspect = 1.f;

    res[0]  = f/aspect;
    res[1]  = 0;
    res[2]  = 0;
    res[3]  = 0;

    res[4]  = 0;
    res[5]  = f;
    res[6]  = 0;
    res[7] = 0;

    res[8]  = 0;
    res[9]  = 0;
    res[10] = -(zFar+zNear)/Dfn;
    res[11] = -1;

    res[12]  = 0;
    res[13]  = 0;
    res[14] = -2.f*zFar*zNear/Dfn;
    res[15] = 0;
}
CHA_API_DEF void chm_Mat4Orthof(float* CHA_RESTRICT mOut16,float left,float right, float bottom, float top,float nearVal,float farVal)  {
    float* res = mOut16;
    const float eps = 0.0001f;
    float Drl = (right-left);
    float Dtb = (top-bottom);
    float Dfn = (farVal-nearVal);
    if (Drl==0) {right+=eps;left-=eps;Drl=right-left;}
    if (Dtb==0) {top+=eps;bottom-=eps;Dtb=top-bottom;}
    if (Dfn==0) {farVal+=eps;nearVal-=eps;Dfn=farVal-nearVal;}

    res[0]  = 2.f/Drl;
    res[1]  = 0;
    res[2]  = 0;
    res[3] = 0;

    res[4]  = 0;
    res[5]  = 2.f/Dtb;
    res[6]  = 0;
    res[7] = 0;

    res[8]  = 0;
    res[9]  = 0;
    res[10] = -2.f/Dfn;
    res[11] = 0;

    res[12]  = -(right+left)/Drl;
    res[13]  = -(top+bottom)/Dtb;
    res[14] = (farVal+nearVal)/Dfn;
    res[15] = 1;
}
CHA_API_DEF void chm_Mat4Ortho3Df(float* CHA_RESTRICT mOut16,float cameraTargetDistance,float degfovy,float aspect,float znear,float zfar)	{
    // Warning: this function might be WRONG! Use it at your own risk!
    const float FOVTG=tanf(degfovy*(float)(3.14159265358979323846/360.0));
    float y=cameraTargetDistance*FOVTG;//=(zfar-znear)*0.5f;
    //float y=(cameraTargetDistance<zfar?cameraTargetDistance:zfar)*FOVTG;  // or maybe this?
    float x=y*aspect;
    //chm_Mat4Orthof(mOut16, -x, x, -y, y, znear, zfar);  // I thought this was correct
    //chm_Mat4Orthof(mOut16, -x, x, -y, y, -zfar, znear);  // But this works better in my test-case
    chm_Mat4Orthof(mOut16, -x, x, -y, y, -zfar, -znear);  // Or maybe this?
}
CHA_API_DEF void chm_Mat4Orthod(double* CHA_RESTRICT mOut16,double left,double right, double bottom, double top,double nearVal,double farVal)   {
    double* res = mOut16;
    const double eps = 0.0001;
    double Drl = (right-left);
    double Dtb = (top-bottom);
    double Dfn = (farVal-nearVal);
    if (Drl==0) {right+=eps;left-=eps;Drl=right-left;}
    if (Dtb==0) {top+=eps;bottom-=eps;Dtb=top-bottom;}
    if (Dfn==0) {farVal+=eps;nearVal-=eps;Dfn=farVal-nearVal;}

    res[0]  = 2.f/Drl;
    res[1]  = 0;
    res[2]  = 0;
    res[3] = 0;

    res[4]  = 0;
    res[5]  = 2.f/Dtb;
    res[6]  = 0;
    res[7] = 0;

    res[8]  = 0;
    res[9]  = 0;
    res[10] = -2.f/Dfn;
    res[11] = 0;

    res[12]  = -(right+left)/Drl;
    res[13]  = -(top+bottom)/Dtb;
    res[14] = (farVal+nearVal)/Dfn;
    res[15] = 1;
}
CHA_API_DEF void chm_Mat4Ortho3Dd(double* CHA_RESTRICT mOut16,double cameraTargetDistance,double degfovy,double aspect,double znear,double zfar)	{
    // Warning: this function might be WRONG! Use it at your own risk!
    const double FOVTG=tanf(degfovy*(double)(3.14159265358979323846/360.0));
    double y=cameraTargetDistance*FOVTG;//=(zfar-znear)*0.5f;
    //double y=(cameraTargetDistance<zfar?cameraTargetDistance:zfar)*FOVTG;  // or maybe this?
    double x=y*aspect;
    //chm_Mat4Orthod(mOut16, -x, x, -y, y, znear, zfar);  // I thought this was correct
    //chm_Mat4Orthod(mOut16, -x, x, -y, y, -zfar, znear);  // But this works better in my test-case
    chm_Mat4Orthod(mOut16, -x, x, -y, y, -zfar, -znear);  // Or maybe this?
}
extern float* chm_Mat4Translatef(float* CHA_RESTRICT mInOut16,float x,float y,float z);
extern double* chm_Mat4Translated(double* CHA_RESTRICT mInOut16,double x,double y,double z);
CHA_API_DEF float* chm_Mat4Rotatef(float* CHA_RESTRICT mInOut16,float degAngle,float x,float y,float z) {
    const float angle = degAngle*M_PIOVER180;
    const float c = cosf(angle);
    const float s = sinf(angle);
    float len = x*x+y*y+z*z;
    if (len<0.999f || len>1.001f) {len=sqrtf(len);x/=len;y/=len;z/=len;}
    {
        const float omc = (float)1-c, xs = x*s,ys = y*s,zs = z*s;int i;
        const float xy = x*y,xz = x*z,yz = y*z, xx = x*x,yy = y*y,zz = z*z;
        const float rot[9] = {  /* 3x3 rotation matrix */
            c+xx*omc,   xy*omc+zs,  xz*omc-ys,
            xy*omc-zs,  c+yy*omc,   yz*omc+xs,
            xz*omc+ys,  yz*omc-xs,  c+zz*omc
        };
        const float* m = mInOut16;
        for (i=0;i<3;i++)   {   /* manual 3x3 rotation */
            const float mi=m[i],mi4=m[i+4],mi8=m[i+8];  /* we cannot use references here */
            mInOut16[i]     = mi*rot[0] + mi4*rot[1] + mi8*rot[2];
            mInOut16[4+i]   = mi*rot[3] + mi4*rot[4] + mi8*rot[5];
            mInOut16[8+i]   = mi*rot[6] + mi4*rot[7] + mi8*rot[8];
        }
        return mInOut16;
    }

}
CHA_API_DEF double* chm_Mat4Rotated(double* CHA_RESTRICT mInOut16,double degAngle,double x,double y,double z)   {
    const double angle = degAngle*M_PIOVER180;
    const double c = cos(angle);
    const double s = sin(angle);
    double len = x*x+y*y+z*z;
    if (len<0.9999 || len>1.0001) {len=sqrt(len);x/=len;y/=len;z/=len;}
    {
        const double omc = (double)1-c, xs = x*s,ys = y*s,zs = z*s;int i;
        const double xy = x*y,xz = x*z,yz = y*z, xx = x*x,yy = y*y,zz = z*z;
        const double rot[9] = {  /* 3x3 rotation matrix */
            c+xx*omc,   xy*omc+zs,  xz*omc-ys,
            xy*omc-zs,  c+yy*omc,   yz*omc+xs,
            xz*omc+ys,  yz*omc-xs,  c+zz*omc
        };
        const double* m = mInOut16;
        for (i=0;i<3;i++)   {   /* manual 3x3 rotation */
            const double mi=m[i],mi4=m[i+4],mi8=m[i+8];  /* we cannot use references here */
            mInOut16[i]     = mi*rot[0] + mi4*rot[1] + mi8*rot[2];
            mInOut16[4+i]   = mi*rot[3] + mi4*rot[4] + mi8*rot[5];
            mInOut16[8+i]   = mi*rot[6] + mi4*rot[7] + mi8*rot[8];
        }
        return mInOut16;
    }

}
extern float* chm_Mat4Scalef(float* CHA_RESTRICT mInOut16,float x,float y,float z);
extern double* chm_Mat4Scaled(double* CHA_RESTRICT mInOut16,double x,double y,double z);
CHA_API_DEF int chm_Mat4Invertf(float* CHA_RESTRICT mOut16,const float* CHA_RESTRICT m16)   {
    const float* m = m16;
    float* n = mOut16;

    float m00 = m[0],  m10 = m[1],  m20 = m[2],  m30 = m[3];
    float m01 = m[4],  m11 = m[5],  m21 = m[6],  m31 = m[7];
    float m02 = m[8],  m12 = m[9],  m22 = m[10], m32 = m[11];
    float m03 = m[12], m13 = m[13], m23 = m[14], m33 = m[15];

    float v0 = m20 * m31 - m21 * m30;
    float v1 = m20 * m32 - m22 * m30;
    float v2 = m20 * m33 - m23 * m30;
    float v3 = m21 * m32 - m22 * m31;
    float v4 = m21 * m33 - m23 * m31;
    float v5 = m22 * m33 - m23 * m32;

    float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    float det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
    if (det==0) return 0;
    {
        float invDet = 1 / det;

        float d00 = t00 * invDet;
        float d10 = t10 * invDet;
        float d20 = t20 * invDet;
        float d30 = t30 * invDet;

        float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;
        {
            float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            v0 = m21 * m10 - m20 * m11;
            v1 = m22 * m10 - m20 * m12;
            v2 = m23 * m10 - m20 * m13;
            v3 = m22 * m11 - m21 * m12;
            v4 = m23 * m11 - m21 * m13;
            v5 = m23 * m12 - m22 * m13;
            {
                float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
                float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
                float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
                float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

                n[0] =d00; n[1] =d10; n[2] =d20; n[3] =d30;
                n[4] =d01; n[5] =d11; n[6] =d21; n[7] =d31;
                n[8] =d02; n[9] =d12; n[10]=d22; n[11]=d32;
                n[12]=d03; n[13]=d13; n[14]=d23; n[15]=d33;
            }
        }
    }
    return 1;
}
CHA_API_DEF int chm_Mat4Invertd(double* CHA_RESTRICT mOut16,const double* CHA_RESTRICT m16) {
    const double* m = m16;
    double* n = mOut16;

    double m00 = m[0],  m10 = m[1],  m20 = m[2],  m30 = m[3];
    double m01 = m[4],  m11 = m[5],  m21 = m[6],  m31 = m[7];
    double m02 = m[8],  m12 = m[9],  m22 = m[10], m32 = m[11];
    double m03 = m[12], m13 = m[13], m23 = m[14], m33 = m[15];

    double v0 = m20 * m31 - m21 * m30;
    double v1 = m20 * m32 - m22 * m30;
    double v2 = m20 * m33 - m23 * m30;
    double v3 = m21 * m32 - m22 * m31;
    double v4 = m21 * m33 - m23 * m31;
    double v5 = m22 * m33 - m23 * m32;

    double t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    double t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    double t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    double t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    double det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
    if (det==0) return 0;
    {
        double invDet = 1 / det;

        double d00 = t00 * invDet;
        double d10 = t10 * invDet;
        double d20 = t20 * invDet;
        double d30 = t30 * invDet;

        double d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        double d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        double d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        double d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;
        {
            double d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            double d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            double d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            double d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            v0 = m21 * m10 - m20 * m11;
            v1 = m22 * m10 - m20 * m12;
            v2 = m23 * m10 - m20 * m13;
            v3 = m22 * m11 - m21 * m12;
            v4 = m23 * m11 - m21 * m13;
            v5 = m23 * m12 - m22 * m13;
            {
                double d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
                double d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
                double d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
                double d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

                n[0] =d00; n[1] =d10; n[2] =d20; n[3] =d30;
                n[4] =d01; n[5] =d11; n[6] =d21; n[7] =d31;
                n[8] =d02; n[9] =d12; n[10]=d22; n[11]=d32;
                n[12]=d03; n[13]=d13; n[14]=d23; n[15]=d33;
            }
        }
    }
    return 1;
}
CHA_API_DEF void chm_Mat4InvertTransformMatrixf(float* CHA_RESTRICT mOut16,const float* CHA_RESTRICT m16)   {
    // It works only for translation + rotation + scaling, and only
    // when rotation can be represented by an unit quaternion
    const float* m = m16;
    float* n = mOut16;int i,i4,i4p1,i4p2;
    const float T[3] = {-m[12],-m[13],-m[14]};
    // Step 1. Transpose the 3x3 submatrix
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step 2. Adjust scaling
    for (i=0;i<3;i++)	{
        float rsi;
        i4=4*i;i4p1=i4+1;i4p2=i4p1+1;
        rsi = m[i4]*m[i4]+m[i4p1]*m[i4p1]+m[i4p2]*m[i4p2];
        if (rsi>1.e-8f) rsi=(float)1.0/rsi;
        else rsi=(float)1.0;
        n[i]*=rsi;n[i+4]*=rsi;n[i+8]*=rsi;
    }
    // Step 3. Adjust translation
    n[12]=T[0]*n[0] + T[1]*n[4] +T[2]*n[8];
    n[13]=T[0]*n[1] + T[1]*n[5] +T[2]*n[9];
    n[14]=T[0]*n[2] + T[1]*n[6] +T[2]*n[10];
}
CHA_API_DEF void chm_Mat4InvertTransformMatrixd(double* CHA_RESTRICT mOut16,const double* CHA_RESTRICT m16) {
    // It works only for translation + rotation + scaling, and only
    // when rotation can be represented by an unit quaternion
    const double* m = m16;
    double* n = mOut16;int i,i4,i4p1,i4p2;
    const double T[3] = {-m[12],-m[13],-m[14]};
    // Step 1. Transpose the 3x3 submatrix
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step 2. Adjust scaling
    for (i=0;i<3;i++)	{
        double rsi;
        i4=4*i;i4p1=i4+1;i4p2=i4p1+1;
        rsi = m[i4]*m[i4]+m[i4p1]*m[i4p1]+m[i4p2]*m[i4p2];
        if (rsi>1.e-8f) rsi=(double)1.0/rsi;
        else rsi=(double)1.0;
        n[i]*=rsi;n[i+4]*=rsi;n[i+8]*=rsi;
    }
    // Step 3. Adjust translation
    n[12]=T[0]*n[0] + T[1]*n[4] +T[2]*n[8];
    n[13]=T[0]*n[1] + T[1]*n[5] +T[2]*n[9];
    n[14]=T[0]*n[2] + T[1]*n[6] +T[2]*n[10];
}
CHA_API_DEF void chm_Mat4InvertTransformMatrixFastf(float* CHA_RESTRICT mOut16,const float* CHA_RESTRICT m16)   {
    // It works only for translation + rotation, and only
    // when rotation can be represented by an unit quaternion
    // scaling is discarded
    const float* m = m16;
    float* n = mOut16;
    const float T[3] = {-m[12],-m[13],-m[14]};
    // Step 1. Transpose the 3x3 submatrix
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step2. Adjust translation
    n[12]=T[0]*n[0] + T[1]*n[4] +T[2]*n[8];
    n[13]=T[0]*n[1] + T[1]*n[5] +T[2]*n[9];
    n[14]=T[0]*n[2] + T[1]*n[6] +T[2]*n[10];
}
CHA_API_DEF void chm_Mat4InvertTransformMatrixFastd(double* CHA_RESTRICT mOut16,const double* CHA_RESTRICT m16) {
    // It works only for translation + rotation, and only
    // when rotation can be represented by an unit quaternion
    // scaling is discarded
    const double* m = m16;
    double* n = mOut16;
    const double T[3] = {-m[12],-m[13],-m[14]};
    // Step 1. Transpose the 3x3 submatrix
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step2. Adjust translation
    n[12]=T[0]*n[0] + T[1]*n[4] +T[2]*n[8];
    n[13]=T[0]*n[1] + T[1]*n[5] +T[2]*n[9];
    n[14]=T[0]*n[2] + T[1]*n[6] +T[2]*n[10];
}
CHA_API_DEF void chm_PlaneNormalizef(float* CHA_RESTRICT p4) {
    float len = p4[0]*p4[0]+p4[1]*p4[1]+p4[2]*p4[2];
    if (len!=0) {int i;len = sqrt(len);for (i=0;i<4;i++) p4[i]/=len;}
}
CHA_API_DEF void chm_PlaneNormalized(double* CHA_RESTRICT p4) {
    double len = p4[0]*p4[0]+p4[1]*p4[1]+p4[2]*p4[2];
    if (len!=0) {int i;len = sqrt(len);for (i=0;i<4;i++) p4[i]/=len;}
}
CHA_API_DEF void chm_GetFrustumPlaneEquationsf(float planeEquationsOut[6][4],const float* CHA_RESTRICT vpMatrix16,int normalizePlanes)   {
    // ax+by+cz+d=0 [xl,xr,yb,yt,zn,zf],normalizePlanes=0 -> no normalization
    float m00 = vpMatrix16[0], m01 = vpMatrix16[4], m02 = vpMatrix16[8],  m03 = vpMatrix16[12];
    float m10 = vpMatrix16[1], m11 = vpMatrix16[5], m12 = vpMatrix16[9],  m13 = vpMatrix16[13];
    float m20 = vpMatrix16[2], m21 = vpMatrix16[6], m22 = vpMatrix16[10], m23 = vpMatrix16[14];
    float m30 = vpMatrix16[3], m31 = vpMatrix16[7], m32 = vpMatrix16[11], m33 = vpMatrix16[15];
    float* p = NULL;
    p = &planeEquationsOut[0][0];   // Left
    p[0] = m30+m00; p[1] = m31+m01; p[2] = m32+m02; p[3] = m33+m03;if (normalizePlanes) chm_PlaneNormalizef(p);
    p = &planeEquationsOut[1][0];   // Right
    p[0] = m30-m00; p[1] = m31-m01; p[2] = m32-m02; p[3] = m33-m03;if (normalizePlanes) chm_PlaneNormalizef(p);
    p = &planeEquationsOut[2][0];   // Bottom
    p[0] = m30+m10; p[1] = m31+m11; p[2] = m32+m12; p[3] = m33+m13;if (normalizePlanes) chm_PlaneNormalizef(p);
    p = &planeEquationsOut[3][0];   // Top
    p[0] = m30-m10; p[1] = m31-m11; p[2] = m32-m12; p[3] = m33-m13;if (normalizePlanes) chm_PlaneNormalizef(p);
    p = &planeEquationsOut[4][0];   // Near
    p[0] = m30+m20; p[1] = m31+m21; p[2] = m32+m22; p[3] = m33+m23;if (normalizePlanes) chm_PlaneNormalizef(p);
    p = &planeEquationsOut[5][0];   // Far
    p[0] = m30-m20; p[1] = m31-m21; p[2] = m32-m22; p[3] = m33-m23;if (normalizePlanes) chm_PlaneNormalizef(p);
}
CHA_API_DEF void chm_GetFrustumPlaneEquationsd(double planeEquationsOut[6][4],const double* CHA_RESTRICT vpMatrix16,int normalizePlanes)   {
    // ax+by+cz+d=0 [xl,xr,yb,yt,zn,zf],normalizePlanes=0 -> no normalization
    double m00 = vpMatrix16[0], m01 = vpMatrix16[4], m02 = vpMatrix16[8],  m03 = vpMatrix16[12];
    double m10 = vpMatrix16[1], m11 = vpMatrix16[5], m12 = vpMatrix16[9],  m13 = vpMatrix16[13];
    double m20 = vpMatrix16[2], m21 = vpMatrix16[6], m22 = vpMatrix16[10], m23 = vpMatrix16[14];
    double m30 = vpMatrix16[3], m31 = vpMatrix16[7], m32 = vpMatrix16[11], m33 = vpMatrix16[15];
    double* p = NULL;
    p = &planeEquationsOut[0][0];   // Left
    p[0] = m30+m00; p[1] = m31+m01; p[2] = m32+m02; p[3] = m33+m03;if (normalizePlanes) chm_PlaneNormalized(p);
    p = &planeEquationsOut[1][0];   // Right
    p[0] = m30-m00; p[1] = m31-m01; p[2] = m32-m02; p[3] = m33-m03;if (normalizePlanes) chm_PlaneNormalized(p);
    p = &planeEquationsOut[2][0];   // Bottom
    p[0] = m30+m10; p[1] = m31+m11; p[2] = m32+m12; p[3] = m33+m13;if (normalizePlanes) chm_PlaneNormalized(p);
    p = &planeEquationsOut[3][0];   // Top
    p[0] = m30-m10; p[1] = m31-m11; p[2] = m32-m12; p[3] = m33-m13;if (normalizePlanes) chm_PlaneNormalized(p);
    p = &planeEquationsOut[4][0];   // Near
    p[0] = m30+m20; p[1] = m31+m21; p[2] = m32+m22; p[3] = m33+m23;if (normalizePlanes) chm_PlaneNormalized(p);
    p = &planeEquationsOut[5][0];   // Far
    p[0] = m30-m20; p[1] = m31-m21; p[2] = m32-m22; p[3] = m33-m23;if (normalizePlanes) chm_PlaneNormalized(p);
}
CHA_API_DEF int chm_IsOBBVisiblef(const float frustumPlanes[6][4],const float*CHA_RESTRICT mfMatrix16,float aabbMinX,float aabbMinY,float aabbMinZ,float aabbMaxX,float aabbMaxY,float aabbMaxZ) {
    // It "should" performs AABB test. mfMatrix16 is the matrix M so that:
    // F*M = mvpMatrix (F being the matrix used to extract the frustum planes).
    // Here we use: F=pMatrix and M=mvMatrix, but it could be: F=vpMatrix and M=mMatrix too.
    int i;const float zero = (float)0;
    // Start OBB => AABB transformation based on: http://dev.theomader.com/transform-bounding-boxes/
    float aabb[6];const float* m = mfMatrix16;
    for (i=0;i<3;i++)   {
        const float a0i=m[i]*aabbMinX,b0i=m[i]*aabbMaxX,a3i=m[4+i]*aabbMinY,b3i=m[4+i]*aabbMaxY,a6i=m[8+i]*aabbMinZ,b6i=m[8+i]*aabbMaxZ,m12i = m[12+i];
        float vmin,vmax;
        if (a0i<b0i)    {vmin = (a0i+m12i); vmax = (b0i+m12i);}
        else            {vmin = (b0i+m12i); vmax = (a0i+m12i);}
        if (a3i<b3i)    {vmin+= a3i;        vmax+= b3i;}
        else            {vmin+= b3i;        vmax+= a3i;}
        if (a6i<b6i)    {vmin+= a6i;        vmax+= b6i;}
        else            {vmin+= b6i;        vmax+= a6i;}
        aabb[i] = vmin;aabb[3+i] = vmax;
    }
    // Tip: From now on 'aabb' must be constant
    // End OBB => AABB transformation based on: http://dev.theomader.com/transform-bounding-boxes/
    {
        for(i=0; i < 6; i++) {
            const float *pl = &frustumPlanes[i][0];
            const int p[3] = {3*(int)(pl[0]>zero),3*(int)(pl[1]>zero),3*(int)(pl[2]>zero)};   // p[j] = 0 or 3
            const float dp = pl[0]*aabb[p[0]] + pl[1]*aabb[p[1]+1] + pl[2]*aabb[p[2]+2] + pl[3];
            if (dp < 0) return 0;
        }
    }
    return 1;
}
CHA_API_DEF int chm_IsOBBVisibled(const double frustumPlanes[6][4],const double* CHA_RESTRICT mfMatrix16,double aabbMinX,double aabbMinY,double aabbMinZ,double aabbMaxX,double aabbMaxY,double aabbMaxZ) {
    // It "should" performs AABB test. mfMatrix16 is the matrix M so that:
    // F*M = mvpMatrix (F being the matrix used to extract the frustum planes).
    // Here we use: F=pMatrix and M=mvMatrix, but it could be: F=vpMatrix and M=mMatrix too.
    int i;const double zero = (double)0;
    // Start OBB => AABB transformation based on: http://dev.theomader.com/transform-bounding-boxes/
    double aabb[6];const double* m = mfMatrix16;
    for (i=0;i<3;i++)   {
        const double a0i=m[i]*aabbMinX,b0i=m[i]*aabbMaxX,a3i=m[4+i]*aabbMinY,b3i=m[4+i]*aabbMaxY,a6i=m[8+i]*aabbMinZ,b6i=m[8+i]*aabbMaxZ,m12i = m[12+i];
        double vmin,vmax;
        if (a0i<b0i)    {vmin = (a0i+m12i); vmax = (b0i+m12i);}
        else            {vmin = (b0i+m12i); vmax = (a0i+m12i);}
        if (a3i<b3i)    {vmin+= a3i;        vmax+= b3i;}
        else            {vmin+= b3i;        vmax+= a3i;}
        if (a6i<b6i)    {vmin+= a6i;        vmax+= b6i;}
        else            {vmin+= b6i;        vmax+= a6i;}
        aabb[i] = vmin;aabb[3+i] = vmax;
    }
    // Tip: From now on 'aabb' must be constant
    // End OBB => AABB transformation based on: http://dev.theomader.com/transform-bounding-boxes/
    {
        for(i=0; i < 6; i++) {
            const double *pl = &frustumPlanes[i][0];
            const int p[3] = {3*(int)(pl[0]>zero),3*(int)(pl[1]>zero),3*(int)(pl[2]>zero)};   // p[j] = 0 or 3
            const double dp = pl[0]*aabb[p[0]] + pl[1]*aabb[p[1]+1] + pl[2]*aabb[p[2]+2] + pl[3];
            if (dp < 0) return 0;
        }
    }
    return 1;
}
// shadow map helpers
CHA_API_DEF void chm_GetFrustumPointsf(float frustumPoints[8][4],const float* CHA_RESTRICT vpMatrixInverse16)    {
    int i,j;const float v[8][4] = {{-1, -1, -1, 1},{-1,  1, -1, 1},{ 1,  1, -1, 1},{ 1, -1, -1, 1},{-1, -1, 1, 1},{-1,  1, 1, 1},{ 1,  1, 1, 1},{ 1, -1, 1, 1}};
    for (i = 0; i < 8; i++) {
        float w,* fPoint = frustumPoints[i],posX=v[i][0],posY=v[i][1],posZ=v[i][2];
        for (j=0;j<4;j++)   fPoint[j] = posX*vpMatrixInverse16[j] + posY*vpMatrixInverse16[4+j] + posZ*vpMatrixInverse16[8+j] + vpMatrixInverse16[12+j];   // Mat4MulPos
        w = fPoint[3];if (w!=(float)0 && w!=(float)1) {fPoint[0]/=w;fPoint[1]/=w;fPoint[2]/=w;} fPoint[3]=(float)1;
    }
}
CHA_API_DEF void chm_GetFrustumAabbCenterAndHalfExtentsf(float* CHA_RESTRICT frustumCenterOut3,float* CHA_RESTRICT frustumHalfExtentsOut3,const float frustumPoints[8][4])    {
    float vmin[3] = {frustumPoints[0][0],frustumPoints[0][1],frustumPoints[0][2]};
    float vmax[3] = {vmin[0],vmin[1],vmin[2]};
    int i,j;
    for (i = 1; i < 8; i++) {
        for (j = 0; j < 3; j++) {
            if      (vmin[j] > frustumPoints[i][j]) vmin[j] = frustumPoints[i][j];
            else if (vmax[j] < frustumPoints[i][j]) vmax[j] = frustumPoints[i][j];
        }
    }
    for (j = 0; j < 3; j++) {
        if (frustumCenterOut3)      frustumCenterOut3[j] = (vmin[j]+vmax[j])*(float)0.5;
        if (frustumHalfExtentsOut3) frustumHalfExtentsOut3[j] = (vmax[j]-vmin[j])*(float)0.5;
    }
}
CHA_API_DEF float chm_GetFrustumRadiusAndCenterDistancef(float* CHA_RESTRICT pFrustumCenterDistanceOut,float cameraNearClippingPlane,float cameraFarClippingPlane,float cameraFovyDeg,float cameraAspectRatio,float cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero)    {
    // returns the frustum radius (by value) and the scalar (positive) distance from the the camera eye to the frustum center (by 'pFrustumCenterDistanceOut').
    // 'cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero': the arg name is correct when the result is passed to 'chm_GetLightViewProjectionMatrix' functions.
    // Otherwise simply set it when using an ortho3D camera frustum (that needs a camera-target distance), or just set it to zero.
    float radius,frustumCenterDistance;
    float tanFovDiagonalSquared = tanf(cameraFovyDeg*(float)(3.14159265358979323846/360.0)); // At this point this is just TANFOVY
    const float halfNearFarClippingPlane = (float)0.5*(cameraFarClippingPlane+cameraNearClippingPlane);
    if (cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero>cameraFarClippingPlane) cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero = 0;  // Not needed
    // Get frustumCenter and radius
    if (cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero<=0)  {
        // camera perspective mode here
        tanFovDiagonalSquared*=tanFovDiagonalSquared;
        tanFovDiagonalSquared*=((float)1+cameraAspectRatio*cameraAspectRatio);
        frustumCenterDistance = halfNearFarClippingPlane*((float)1+tanFovDiagonalSquared);
        if (frustumCenterDistance > cameraFarClippingPlane) frustumCenterDistance = cameraFarClippingPlane;
        radius = (tanFovDiagonalSquared*cameraFarClippingPlane*cameraFarClippingPlane) + (cameraFarClippingPlane-frustumCenterDistance)*(cameraFarClippingPlane-frustumCenterDistance); // This is actually radiusSquared
    }
    else {
        // camera ortho3d mode here
        const float y=cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero*tanFovDiagonalSquared;
        const float x=y*cameraAspectRatio;
        const float halfClippingPlaneDistance = (float)(0.5)*(cameraFarClippingPlane-cameraNearClippingPlane);
        frustumCenterDistance = halfNearFarClippingPlane;
        radius = x*x+y*y; // This is actually radiusXYSquared
        radius = radius + halfClippingPlaneDistance*halfClippingPlaneDistance;// This is actually radiusSquared
    }
    radius = sqrtf(radius);
    if (pFrustumCenterDistanceOut) *pFrustumCenterDistanceOut = frustumCenterDistance;
    return radius;
}
CHA_API_DEF void chm_GetFrustumCenterFromCenterDistancef(float* CHA_RESTRICT frustumCenterOut3,const float* CHA_RESTRICT cameraVMatrixInverse16,float frustumCenterDistance)    {
    // 'frustumCenterOut3' is in world space (mandatory arg).
    // 'cameraVMatrixInverse16' must be orthonormal (no scaling).
    const float cameraPosition3[3] = {cameraVMatrixInverse16[12],cameraVMatrixInverse16[13],cameraVMatrixInverse16[14]};
    const float cameraForwardDirection3[3] = {-cameraVMatrixInverse16[8],-cameraVMatrixInverse16[9],-cameraVMatrixInverse16[10]};
    int i;if (frustumCenterOut3)  {for (i=0;i<3;i++) frustumCenterOut3[i] = cameraPosition3[i] + cameraForwardDirection3[i]*frustumCenterDistance;}
}
CHA_API_DEF void chm_GetLightViewProjectionMatrixExtraf(float* CHA_RESTRICT lvpMatrixOut16
                                                          ,const float* CHA_RESTRICT cameraVMatrixInverse16,const float* CHA_RESTRICT cameraFrustumCenterInWorldSpace,float cameraFrustumRadius
                                                          ,const float*  CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight
                                                          ,const float* CHA_RESTRICT optionalCameraPMatrixInverse16
                                                          ,float* CHA_RESTRICT optionalLightViewportClippingOut4,float optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,float* CHA_RESTRICT optionalLVPMatrixForFrustumCullingUsageOut16   // Highly experimental and untested
                                                          )  {
    // 'optionalPMatrixInverse16' is required only if you need to retrieve (one or more of) the arguments that follow them (otherwise their value is untouched).
    // 'normalizedLightDirection3' must be in world space.
    float lpMatrix[16],lvMatrix[16],lvpMatrixFallback[16];
    const float texelIncrement = shadowMapWidth>=shadowMapHeight ? (float) 1.0/(float)shadowMapHeight : (float) 1.0/(float)shadowMapWidth;
    const float cameraLeftDirection3[3] = {-cameraVMatrixInverse16[0],-cameraVMatrixInverse16[1],-cameraVMatrixInverse16[2]};
    float dotCameraXDirectionLightDirection = 0, lightUpVector3[3]={0,1,0};
    int i;if (lvpMatrixOut16==0) lvpMatrixOut16=lvpMatrixFallback;
    if (lvpMatrixOut16!=lvpMatrixFallback && shadowMapWidth!=shadowMapHeight)    {
        // User wants stable shadow mapping, but there are additional requirements if shadowMapWidth!=shadowMapHeight [usually setting them to power of two fixes them]
        const unsigned short r = (shadowMapWidth<shadowMapHeight) ? (shadowMapHeight%shadowMapWidth) : (shadowMapWidth%shadowMapHeight);
        CHA_ASSERT(r==0);
    }
    // Shadow swimming happens when: 1) camera translates; 2) camera rotates; 3) objects move or rotate
    // AFAIK Shadow swimming (3) can't be fixed in any way
    if (texelIncrement>0)   cameraFrustumRadius = ceilf(cameraFrustumRadius/texelIncrement)*texelIncrement;      // This 'should' fix Shadow swimming (1)  [Not sure code is correct!]
    // Get light matrices
    chm_Mat4Orthof(lpMatrix,-cameraFrustumRadius,cameraFrustumRadius,-cameraFrustumRadius,cameraFrustumRadius,0,-(float)2*cameraFrustumRadius);
    if (shadowMapWidth!=shadowMapHeight)    {
        if (lvpMatrixOut16!=lvpMatrixFallback)  {
            // User wants stable shadow mapping... (with popping artifacts)
            // Good (stable with popping artifacts)
            dotCameraXDirectionLightDirection = fabsf(chm_Vec3Dotf(cameraLeftDirection3,normalizedLightDirection3));
            if (dotCameraXDirectionLightDirection>(float)0.5) {lightUpVector3[0]=0;lightUpVector3[1]=0;lightUpVector3[2]=1;}
        }
        else {/* User wants unstable shadow mapping */ chm_Vec3Crossf(lightUpVector3,cameraLeftDirection3,normalizedLightDirection3);}
    }
    chm_Mat4LookAtf(lvMatrix,
            cameraFrustumCenterInWorldSpace[0]-normalizedLightDirection3[0]*cameraFrustumRadius,   // eye[0]
            cameraFrustumCenterInWorldSpace[1]-normalizedLightDirection3[1]*cameraFrustumRadius,   // eye[1]
            cameraFrustumCenterInWorldSpace[2]-normalizedLightDirection3[2]*cameraFrustumRadius,   // eye[2]
            cameraFrustumCenterInWorldSpace[0],cameraFrustumCenterInWorldSpace[1],cameraFrustumCenterInWorldSpace[2],     // target
            lightUpVector3[0],lightUpVector3[1],lightUpVector3[2]                                                   // up (people that cares about wasted texture space can probably change it)
            );
    chm_Mat4Mulf(lvpMatrixOut16,lpMatrix,lvMatrix);// Get output
    // This 'should' fix Shadow swimming (2) [Not sure code is correct!]
    if (texelIncrement>0)   {
        float shadowOrigin[4]   = {0,0,0,1};
        float roundedOrigin[4]  = {0,0,0,0};
        float roundOffset[4]    = {0,0,0,0};
        float texelCoefficient = texelIncrement*(float)2;
        chm_Mat4MulPosf(lvpMatrixOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
        for (i = 0; i < 2; i++) {// Or i<3 ?
            shadowOrigin[i]/= texelCoefficient;
            roundedOrigin[i] = chm_Roundf(shadowOrigin[i]);
            roundOffset[i] = roundedOrigin[i] - shadowOrigin[i];
            roundOffset[i]*=  texelCoefficient;
        }
        lvpMatrixOut16[12]+= roundOffset[0];
        lvpMatrixOut16[13]+= roundOffset[1];
    }
    // Extra stuff
    if (optionalCameraPMatrixInverse16) {
        float cameraVPMatrixInv[16],cameraVPMatrixInverseAdjusted[16];float frustumPoints[8][4];
        float minVal[3],maxVal[3],tmp;int j;
        chm_Mat4Mulf(cameraVPMatrixInv,cameraVMatrixInverse16,optionalCameraPMatrixInverse16); // vMatrixInverse16 needs an expensive chm_Mat4Invert(...) to be calculated. Here we can exploit the property of the product of 2 inverse matrices.
        // If we call chm_GetFrustumPoints(frustumPoints,cameraVPMatrixInv) we find the frustum corners in world space

        chm_Mat4Mulf(cameraVPMatrixInverseAdjusted,lvpMatrixOut16,cameraVPMatrixInv);  // This way we 'should' get all points in the [-1,1] light NDC space (or not?)

        chm_GetFrustumPointsf(frustumPoints,cameraVPMatrixInverseAdjusted);

        if (optionalCameraFrustumPointsInNDCLightSpaceOut) {
            for (i=0;i<8;i++)   {
                for (j=0;j<4;j++)   {
                    optionalCameraFrustumPointsInNDCLightSpaceOut[i][j] = frustumPoints[i][j];
                }
            }
        }

        // Calculate 'minVal' and 'maxVal' based on 'frustumPoints'
        for (i=0;i<3;i++)   minVal[i]=maxVal[i]=frustumPoints[0][i];
        for (i=1;i<8;i++)   {
            for (j=0;j<3;j++)   {   // We will actually skip the z component later...
                tmp = frustumPoints[i][j];
                if      (minVal[j]>tmp) minVal[j]=tmp;
                else if (maxVal[j]<tmp) maxVal[j]=tmp;
            }
            //fprintf(stderr,"frustumPoints[%d]={%1.4f,%1.4f,%1.4f}\n",i,frustumPoints[i][0], frustumPoints[i][1], frustumPoints[i][2]);
        }

        if (optionalLightViewportClippingOut4)   {
            const float zero_point_five = (float) 0.5;
            optionalLightViewportClippingOut4[0] = minVal[0]*zero_point_five+zero_point_five;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[1] = minVal[1]*zero_point_five+zero_point_five;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[2] = (maxVal[0]-minVal[0])*zero_point_five;    // extent x in [0,1]
            optionalLightViewportClippingOut4[3] = (maxVal[1]-minVal[1])*zero_point_five;    // extent y in [0,1]

            for (i=0;i<4;i++)   {
               optionalLightViewportClippingOut4[i]/=texelIncrement;    // viewport is in [0,texture_size]
               // mmmh, this is probably wrong when textureWidth!=textureHeight... TO CHECK!
            }
        }
        if (optionalLVPMatrixForFrustumCullingUsageOut16)   {
            const int attemptToFixSwimming = (lvpMatrixOut16==lvpMatrixFallback) ? 1 : 0;   // Only if we don't want lvpMatrixOut16
            float minmaxXY[4]={minVal[0]*cameraFrustumRadius,maxVal[0]*cameraFrustumRadius,minVal[1]*cameraFrustumRadius,maxVal[1]*cameraFrustumRadius};
            if (attemptToFixSwimming && texelIncrement>0)   {
                for (i=0;i<4;i++) {
                    // This 'should' fix Shadow swimming (1) in the 'Stable Shadow Mapping Technique'
                    // Not sure it works here too...
                    if (minmaxXY[i]>=0) minmaxXY[i] = ceilf(minmaxXY[i]/texelIncrement)*texelIncrement;
                    else                minmaxXY[i] = -ceilf(-minmaxXY[i]/texelIncrement)*texelIncrement;
                }
            }
            chm_Mat4Orthof(optionalLVPMatrixForFrustumCullingUsageOut16,
                         minmaxXY[0],minmaxXY[1],
                         minmaxXY[2],minmaxXY[3],
                         0,-(float)2*cameraFrustumRadius                      // For z, we just copy chm_Ortho(lpMatrix,...)
                         );
            chm_Mat4Mulf(optionalLVPMatrixForFrustumCullingUsageOut16,optionalLVPMatrixForFrustumCullingUsageOut16,lvMatrix);
            // This 'should' fix Shadow swimming (2) in the 'Stable Shadow Mapping Technique'. Not here, because the shadow viewport stretches when the camera rotates. We try anyway...
            if (attemptToFixSwimming && texelIncrement>0)   {
                float shadowOrigin[4]   = {0,0,0,1};
                float roundedOrigin[4]  = {0,0,0,0};
                float roundOffset[4]    = {0,0,0,0};
                float texelCoefficient = texelIncrement*(float)2;
                chm_Mat4MulPosf(optionalLVPMatrixForFrustumCullingUsageOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
                for (i = 0; i < 2; i++) {// Or i<3 ?
                    shadowOrigin[i]/= texelCoefficient;
                    roundedOrigin[i] = chm_Roundf(shadowOrigin[i]);
                    roundOffset[i] = roundedOrigin[i] - shadowOrigin[i];
                    roundOffset[i]*=  texelCoefficient;
                }
                optionalLVPMatrixForFrustumCullingUsageOut16[12]+= roundOffset[0];
                optionalLVPMatrixForFrustumCullingUsageOut16[13]+= roundOffset[1];
            }
        }
    }

}
CHA_API_DEF void chm_GetLightViewProjectionMatrixf(float* CHA_RESTRICT lvpMatrixOut16
                                                         ,const float* CHA_RESTRICT cameraVMatrixInverse16,const float* CHA_RESTRICT cameraFrustumCenterInWorldSpace,float cameraFrustumRadius
                                                         ,const float* CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight)  {
    // 'normalizedLightDirection3' must be in world space.
    chm_GetLightViewProjectionMatrixExtraf(lvpMatrixOut16,cameraVMatrixInverse16,cameraFrustumCenterInWorldSpace,cameraFrustumRadius,normalizedLightDirection3,shadowMapWidth,shadowMapHeight,0,0,0,0);
}

CHA_API_DEF void chm_GetFrustumPointsd(double frustumPoints[8][4],const double* CHA_RESTRICT vpMatrixInverse16)    {
    int i,j;const double v[8][4] = {{-1, -1, -1, 1},{-1,  1, -1, 1},{ 1,  1, -1, 1},{ 1, -1, -1, 1},{-1, -1, 1, 1},{-1,  1, 1, 1},{ 1,  1, 1, 1},{ 1, -1, 1, 1}};
    for (i = 0; i < 8; i++) {
        double w,* fPoint = frustumPoints[i],posX=v[i][0],posY=v[i][1],posZ=v[i][2];
        for (j=0;j<4;j++)   fPoint[j] = posX*vpMatrixInverse16[j] + posY*vpMatrixInverse16[4+j] + posZ*vpMatrixInverse16[8+j] + vpMatrixInverse16[12+j];   // Mat4MulPos
        w = fPoint[3];if (w!=(double)0 && w!=(double)1) {fPoint[0]/=w;fPoint[1]/=w;fPoint[2]/=w;} fPoint[3]=(double)1;
    }
}
CHA_API_DEF void chm_GetFrustumAabbCenterAndHalfExtentsd(double* CHA_RESTRICT frustumCenterOut3,double* CHA_RESTRICT frustumHalfExtentsOut3,const double frustumPoints[8][4])    {
    double vmin[3] = {frustumPoints[0][0],frustumPoints[0][1],frustumPoints[0][2]};
    double vmax[3] = {vmin[0],vmin[1],vmin[2]};
    int i,j;
    for (i = 1; i < 8; i++) {
        for (j = 0; j < 3; j++) {
            if      (vmin[j] > frustumPoints[i][j]) vmin[j] = frustumPoints[i][j];
            else if (vmax[j] < frustumPoints[i][j]) vmax[j] = frustumPoints[i][j];
        }
    }
    for (j = 0; j < 3; j++) {
        if (frustumCenterOut3)      frustumCenterOut3[j] = (vmin[j]+vmax[j])*(double)0.5;
        if (frustumHalfExtentsOut3) frustumHalfExtentsOut3[j] = (vmax[j]-vmin[j])*(double)0.5;
    }
}
CHA_API_DEF double chm_GetFrustumRadiusAndCenterDistanced(double* CHA_RESTRICT pFrustumCenterDistanceOut,double cameraNearClippingPlane,double cameraFarClippingPlane,double cameraFovyDeg,double cameraAspectRatio,double cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero)    {
    // returns the frustum radius (by value) and the scalar (positive) distance from the the camera eye to the frustum center (by 'pFrustumCenterDistanceOut').
    // 'cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero': the arg name is correct when the result is passed to 'chm_GetLightViewProjectionMatrix' functions.
    // Otherwise simply set it when using an ortho3D camera frustum (that needs a camera-target distance), or just set it to zero.
    double radius,frustumCenterDistance;
    double tanFovDiagonalSquared = tan(cameraFovyDeg*(double)(3.14159265358979323846/360.0)); // At this point this is just TANFOVY
    const double halfNearFarClippingPlane = (double)0.5*(cameraFarClippingPlane+cameraNearClippingPlane);
    if (cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero>cameraFarClippingPlane) cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero = 0;  // Not needed
    // Get frustumCenter and radius
    if (cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero<=0)  {
        // camera perspective mode here
        tanFovDiagonalSquared*=tanFovDiagonalSquared;
        tanFovDiagonalSquared*=((double)1+cameraAspectRatio*cameraAspectRatio);
        frustumCenterDistance = halfNearFarClippingPlane*((double)1+tanFovDiagonalSquared);
        if (frustumCenterDistance > cameraFarClippingPlane) frustumCenterDistance = cameraFarClippingPlane;
        radius = (tanFovDiagonalSquared*cameraFarClippingPlane*cameraFarClippingPlane) + (cameraFarClippingPlane-frustumCenterDistance)*(cameraFarClippingPlane-frustumCenterDistance); // This is actually radiusSquared
    }
    else {
        // camera ortho3d mode here
        const double y=cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero*tanFovDiagonalSquared;
        const double x=y*cameraAspectRatio;
        const double halfClippingPlaneDistance = (double)(0.5)*(cameraFarClippingPlane-cameraNearClippingPlane);
        frustumCenterDistance = halfNearFarClippingPlane;
        radius = x*x+y*y; // This is actually radiusXYSquared
        radius = radius + halfClippingPlaneDistance*halfClippingPlaneDistance;// This is actually radiusSquared
    }
    radius = sqrt(radius);
    if (pFrustumCenterDistanceOut) *pFrustumCenterDistanceOut = frustumCenterDistance;
    return radius;
}
CHA_API_DEF void chm_GetFrustumCenterFromCenterDistanced(double* CHA_RESTRICT frustumCenterOut3,const double* CHA_RESTRICT cameraVMatrixInverse16,double frustumCenterDistance)    {
    // 'frustumCenterOut3' is in world space (mandatory arg).
    // 'cameraVMatrixInverse16' must be orthonormal (no scaling).
    const double cameraPosition3[3] = {cameraVMatrixInverse16[12],cameraVMatrixInverse16[13],cameraVMatrixInverse16[14]};
    const double cameraForwardDirection3[3] = {-cameraVMatrixInverse16[8],-cameraVMatrixInverse16[9],-cameraVMatrixInverse16[10]};
    int i;if (frustumCenterOut3)  {for (i=0;i<3;i++) frustumCenterOut3[i] = cameraPosition3[i] + cameraForwardDirection3[i]*frustumCenterDistance;}
}
CHA_API_DEF void chm_GetLightViewProjectionMatrixExtrad(double* CHA_RESTRICT lvpMatrixOut16
                                                          ,const double* CHA_RESTRICT cameraVMatrixInverse16,const double* CHA_RESTRICT cameraFrustumCenterInWorldSpace,double cameraFrustumRadius
                                                          ,const double*  CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight
                                                          ,const double* CHA_RESTRICT optionalCameraPMatrixInverse16
                                                          ,double* CHA_RESTRICT optionalLightViewportClippingOut4,double optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,double* CHA_RESTRICT optionalLVPMatrixForFrustumCullingUsageOut16   // Highly experimental and untested
                                                          )  {
    // 'optionalPMatrixInverse16' is required only if you need to retrieve (one or more of) the arguments that follow them (otherwise their value is untouched).
    // 'normalizedLightDirection3' must be in world space.
    double lpMatrix[16],lvMatrix[16],lvpMatrixFallback[16];
    const double texelIncrement = shadowMapWidth>=shadowMapHeight ? (double) 1.0/(double)shadowMapHeight : (double) 1.0/(double)shadowMapWidth;
    const double cameraLeftDirection3[3] = {-cameraVMatrixInverse16[0],-cameraVMatrixInverse16[1],-cameraVMatrixInverse16[2]};
    double dotCameraXDirectionLightDirection = 0, lightUpVector3[3]={0,1,0};
    int i;if (lvpMatrixOut16==0) lvpMatrixOut16=lvpMatrixFallback;
    if (lvpMatrixOut16!=lvpMatrixFallback && shadowMapWidth!=shadowMapHeight)    {
        const unsigned short r = (shadowMapWidth<shadowMapHeight) ? (shadowMapHeight%shadowMapWidth) : (shadowMapWidth%shadowMapHeight);
        CHA_ASSERT(r==0);   // User wants stable shadow mapping, but there are additional requirements if shadowMapWidth!=shadowMapHeight [usually setting them to power of two fixes them]
    }
    // Shadow swimming happens when: 1) camera translates; 2) camera rotates; 3) objects move or rotate
    // AFAIK Shadow swimming (3) can't be fixed in any way
    if (texelIncrement>0)   cameraFrustumRadius = ceil(cameraFrustumRadius/texelIncrement)*texelIncrement;      // This 'should' fix Shadow swimming (1)  [Not sure code is correct!]
    // Get light matrices
    chm_Mat4Orthod(lpMatrix,-cameraFrustumRadius,cameraFrustumRadius,-cameraFrustumRadius,cameraFrustumRadius,0,-(double)2*cameraFrustumRadius);
    if (shadowMapWidth!=shadowMapHeight)    {
        if (lvpMatrixOut16!=lvpMatrixFallback)  {
            // User wants stable shadow mapping... (with popping artifacts)
            // Good (stable with popping artifacts)
            dotCameraXDirectionLightDirection = fabs(chm_Vec3Dotd(cameraLeftDirection3,normalizedLightDirection3));
            if (dotCameraXDirectionLightDirection>(double)0.5) {lightUpVector3[0]=0;lightUpVector3[1]=0;lightUpVector3[2]=1;}
        }
        else {/* User wants unstable shadow mapping */ chm_Vec3Crossd(lightUpVector3,cameraLeftDirection3,normalizedLightDirection3);}
    }
    chm_Mat4LookAtd(lvMatrix,
            cameraFrustumCenterInWorldSpace[0]-normalizedLightDirection3[0]*cameraFrustumRadius,   // eye[0]
            cameraFrustumCenterInWorldSpace[1]-normalizedLightDirection3[1]*cameraFrustumRadius,   // eye[1]
            cameraFrustumCenterInWorldSpace[2]-normalizedLightDirection3[2]*cameraFrustumRadius,   // eye[2]
            cameraFrustumCenterInWorldSpace[0],cameraFrustumCenterInWorldSpace[1],cameraFrustumCenterInWorldSpace[2],     // target
            lightUpVector3[0],lightUpVector3[1],lightUpVector3[2]                                                   // up (people that cares about wasted texture space can probably change it)
            );
    chm_Mat4Muld(lvpMatrixOut16,lpMatrix,lvMatrix);// Get output
    // This 'should' fix Shadow swimming (2) [Not sure code is correct!]
    if (texelIncrement>0)   {
        double shadowOrigin[4]   = {0,0,0,1};
        double roundedOrigin[4]  = {0,0,0,0};
        double roundOffset[4]    = {0,0,0,0};
        double texelCoefficient = texelIncrement*(double)2;
        chm_Mat4MulPosd(lvpMatrixOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
        for (i = 0; i < 2; i++) {// Or i<3 ?
            shadowOrigin[i]/= texelCoefficient;
            roundedOrigin[i] = chm_Roundd(shadowOrigin[i]);
            roundOffset[i] = roundedOrigin[i] - shadowOrigin[i];
            roundOffset[i]*=  texelCoefficient;
        }
        lvpMatrixOut16[12]+= roundOffset[0];
        lvpMatrixOut16[13]+= roundOffset[1];
    }
    // Extra stuff
    if (optionalCameraPMatrixInverse16) {
        double cameraVPMatrixInv[16],cameraVPMatrixInverseAdjusted[16];double frustumPoints[8][4];
        double minVal[3],maxVal[3],tmp;int j;
        chm_Mat4Muld(cameraVPMatrixInv,cameraVMatrixInverse16,optionalCameraPMatrixInverse16); // vMatrixInverse16 needs an expensive chm_Mat4Invert(...) to be calculated. Here we can exploit the property of the product of 2 inverse matrices.
        // If we call chm_GetFrustumPoints(frustumPoints,cameraVPMatrixInv) we find the frustum corners in world space

        chm_Mat4Muld(cameraVPMatrixInverseAdjusted,lvpMatrixOut16,cameraVPMatrixInv);  // This way we 'should' get all points in the [-1,1] light NDC space (or not?)

        chm_GetFrustumPointsd(frustumPoints,cameraVPMatrixInverseAdjusted);

        if (optionalCameraFrustumPointsInNDCLightSpaceOut) {
            for (i=0;i<8;i++)   {
                for (j=0;j<4;j++)   {
                    optionalCameraFrustumPointsInNDCLightSpaceOut[i][j] = frustumPoints[i][j];
                }
            }
        }

        // Calculate 'minVal' and 'maxVal' based on 'frustumPoints'
        for (i=0;i<3;i++)   minVal[i]=maxVal[i]=frustumPoints[0][i];
        for (i=1;i<8;i++)   {
            for (j=0;j<3;j++)   {   // We will actually skip the z component later...
                tmp = frustumPoints[i][j];
                if      (minVal[j]>tmp) minVal[j]=tmp;
                else if (maxVal[j]<tmp) maxVal[j]=tmp;
            }
            //fprintf(stderr,"frustumPoints[%d]={%1.4f,%1.4f,%1.4f}\n",i,frustumPoints[i][0], frustumPoints[i][1], frustumPoints[i][2]);
        }

        if (optionalLightViewportClippingOut4)   {
            const double zero_point_five = (double) 0.5;
            optionalLightViewportClippingOut4[0] = minVal[0]*zero_point_five+zero_point_five;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[1] = minVal[1]*zero_point_five+zero_point_five;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[2] = (maxVal[0]-minVal[0])*zero_point_five;    // extent x in [0,1]
            optionalLightViewportClippingOut4[3] = (maxVal[1]-minVal[1])*zero_point_five;    // extent y in [0,1]

            for (i=0;i<4;i++)   {
               optionalLightViewportClippingOut4[i]/=texelIncrement;    // viewport is in [0,texture_size]
               // mmmh, this is probably wrong when textureWidth!=textureHeight... TO CHECK!
            }
        }
        if (optionalLVPMatrixForFrustumCullingUsageOut16)   {
            const int attemptToFixSwimming = (lvpMatrixOut16==lvpMatrixFallback) ? 1 : 0;   // Only if we don't want lvpMatrixOut16
            double minmaxXY[4]={minVal[0]*cameraFrustumRadius,maxVal[0]*cameraFrustumRadius,minVal[1]*cameraFrustumRadius,maxVal[1]*cameraFrustumRadius};
            if (attemptToFixSwimming && texelIncrement>0)   {
                for (i=0;i<4;i++) {
                    // This 'should' fix Shadow swimming (1) in the 'Stable Shadow Mapping Technique'
                    // Not sure it works here too...
                    if (minmaxXY[i]>=0) minmaxXY[i] = ceil(minmaxXY[i]/texelIncrement)*texelIncrement;
                    else                minmaxXY[i] = -ceil(-minmaxXY[i]/texelIncrement)*texelIncrement;
                }
            }
            chm_Mat4Orthod(optionalLVPMatrixForFrustumCullingUsageOut16,
                         minmaxXY[0],minmaxXY[1],
                         minmaxXY[2],minmaxXY[3],
                         0,-(double)2*cameraFrustumRadius                      // For z, we just copy chm_Ortho(lpMatrix,...)
                         );
            chm_Mat4Muld(optionalLVPMatrixForFrustumCullingUsageOut16,optionalLVPMatrixForFrustumCullingUsageOut16,lvMatrix);
            // This 'should' fix Shadow swimming (2) in the 'Stable Shadow Mapping Technique'. Not here, because the shadow viewport stretches when the camera rotates. We try anyway...
            if (attemptToFixSwimming && texelIncrement>0)   {
                double shadowOrigin[4]   = {0,0,0,1};
                double roundedOrigin[4]  = {0,0,0,0};
                double roundOffset[4]    = {0,0,0,0};
                double texelCoefficient = texelIncrement*(double)2;
                chm_Mat4MulPosd(optionalLVPMatrixForFrustumCullingUsageOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
                for (i = 0; i < 2; i++) {// Or i<3 ?
                    shadowOrigin[i]/= texelCoefficient;
                    roundedOrigin[i] = chm_Roundd(shadowOrigin[i]);
                    roundOffset[i] = roundedOrigin[i] - shadowOrigin[i];
                    roundOffset[i]*=  texelCoefficient;
                }
                optionalLVPMatrixForFrustumCullingUsageOut16[12]+= roundOffset[0];
                optionalLVPMatrixForFrustumCullingUsageOut16[13]+= roundOffset[1];
            }
        }
    }

}
CHA_API_DEF void chm_GetLightViewProjectionMatrixd(double* CHA_RESTRICT lvpMatrixOut16
                                                         ,const double* CHA_RESTRICT cameraVMatrixInverse16,const double* CHA_RESTRICT cameraFrustumCenterInWorldSpace,double cameraFrustumRadius
                                                         ,const double* CHA_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight)  {
    // 'normalizedLightDirection3' must be in world space.
    chm_GetLightViewProjectionMatrixExtrad(lvpMatrixOut16,cameraVMatrixInverse16,cameraFrustumCenterInWorldSpace,cameraFrustumRadius,normalizedLightDirection3,shadowMapWidth,shadowMapHeight,0,0,0,0);
}
// end shadow map helpers

//---------------- single-precision-only math -------------------------------------------------------------
extern void chm_Mat4MulPosWithWDivisionf(const float* CHA_RESTRICT m16,float* CHA_RESTRICT posOut3,const float posX,float posY,float posZ);
CHA_API_DEF int chm_UnProject_MvpMatrixInvf(float winX,float winY,float winZ,const float* CHA_RESTRICT mvpMatrixInv16,const int* viewport4,float* objX,float* objY,float* objZ)    {
    // To compute the coordinates objX objY objZ , gluUnProject multiplies the normalized device coordinates by the inverse of model * proj as follows:
    // [objX objY objZ W] = INV(P*M) * ⁢ [2*(winX-view[0])/view[2]-1    2*(winY-view[1])/view[3]-1  2*winZ-1    1]
    // INV denotes matrix inversion. W is an unused variable, included for consistent matrix notation.
   const float *invpm = mvpMatrixInv16;
   // normalized device coords
   const float v[4] = {
      2*(winX-(float)viewport4[0])/(float)viewport4[2]-1,
      2*(winY-(float)viewport4[1])/(float)viewport4[3]-1,
      2*winZ-1,
      1
   };
   float objW = 0;
   // calculate obj
   *objX =  v[0]*invpm[0] + v[1]*invpm[4] + v[2]*invpm[8]  + v[3]*invpm[12];
   *objY =  v[0]*invpm[1] + v[1]*invpm[5] + v[2]*invpm[9]  + v[3]*invpm[13];
   *objZ =  v[0]*invpm[2] + v[1]*invpm[6] + v[2]*invpm[10] + v[3]*invpm[14];
    objW =  v[0]*invpm[3] + v[1]*invpm[7] + v[2]*invpm[11] + v[3]*invpm[15];
   /* *objX =  v[0]*invpm[0] + v[1]*invpm[1] + v[2]*invpm[2]  + v[3]*invpm[3];
   *objY =  v[0]*invpm[4] + v[1]*invpm[5] + v[2]*invpm[6]  + v[3]*invpm[7];
   *objZ =  v[0]*invpm[8] + v[1]*invpm[9] + v[2]*invpm[10] + v[3]*invpm[11];*/
   //objW  =  v[0]*invpm[12] + v[1]*invpm[13] + v[2]*invpm[14] + v[3]*invpm[15];
   if (objW!=0 && objW!=1) {(*objX)/=objW;(*objY)/=objW;(*objZ)/=objW;}
   return 1;
}
CHA_API_DEF void chm_GetRayFromMouseCoordsf(float* rayOriginOut3,float* rayDirOut3,int mouseX,int mouseY,const float* vpMatrixInv,const int* viewport4) {
    // rayOriginOut3 and rayDirOut3 are in world space
    float rayOrigin[3] = {0,0,0};
    float rayDir[3] = {0,0,-1};
    int i;
    // Find rayOrigin and rayDir (world space)
    //chm_MultMatrix(vpMatrixInv,TIS.pMatrix,TIS.vMatrix);
    //chm_InvertMatrix(vpMatrixInv,vpMatrixInv);
    chm_UnProject_MvpMatrixInvf(mouseX,viewport4[3]-mouseY-1,0.0,vpMatrixInv,viewport4,&rayOrigin[0],&rayOrigin[1],&rayOrigin[2]);
    chm_UnProject_MvpMatrixInvf(mouseX,viewport4[3]-mouseY-1,1.0,vpMatrixInv,viewport4,&rayDir[0],&rayDir[1],&rayDir[2]);
    for (i=0;i<3;i++) rayDir[i]-=rayOrigin[i];
    chm_Vec3Normalizef(rayDir);
    //printf("rayOrigin={%1.2f,%1.2f,%1.2f} rayDir={%1.2f,%1.2f,%1.2f}\n",rayOrigin[0],rayOrigin[1],rayOrigin[2],rayDir[0],rayDir[1],rayDir[2]);
    for (i=0;i<3;i++) {
        if (rayDirOut3) rayDirOut3[i] = rayDir[i];
        if (rayOriginOut3) rayOriginOut3[i] = rayOrigin[i];
    }
}
extern float* chm_Mat4Sumf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16);
extern float* chm_Mat4SumInPlacef(float* CHA_RESTRICT mInOut16,const float* CHA_RESTRICT m16);
extern float* chm_Mat4Subf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16);
extern float* chm_Mat4SubInPlacef(float* CHA_RESTRICT mInOut16,const float* CHA_RESTRICT m16);
extern float* chm_Mat4FMAf(float* CHA_RESTRICT result16,const float* CHA_RESTRICT ml16,const float* CHA_RESTRICT mr16,float sca);
extern float* chm_Mat4FMAInPlacef(float* CHA_RESTRICT mInOut16,const float* CHA_RESTRICT m16,float sca);
#ifndef CHA_NO_STDIO
CHA_API_DEF void chm_Mat4_fprintpf(const float* CHA_RESTRICT m16,FILE* stream,int width, int precision)  {
    int w = width, p = precision, r;
    CHA_ASSERT(m16);
    for(r = 0; r < 4; r++) {
        fprintf(stream, "| %*.*f %*.*f %*.*f %*.*f |\n",
            w, p, m16[r], w, p, m16[r+4], w, p, m16[r+8], w, p, m16[r+12]
        );
    }
}
CHA_API_DEF void chm_Mat4_fprintf(const float* CHA_RESTRICT m16,FILE* stream) {chm_Mat4_fprintpf(m16,stream, 6, 2);}
CHA_API_DEF void chm_Mat4_printpf(const float* CHA_RESTRICT m16,int width, int precision)  {chm_Mat4_fprintpf(m16,stdout, width, precision);}
CHA_API_DEF void chm_Mat4_printf(const float* CHA_RESTRICT m16) {chm_Mat4_fprintpf(m16,stdout, 6, 2);}
CHA_API_DEF void chm_Mat4_fprintpd(const double* CHA_RESTRICT m16,FILE* stream,int width, int precision)  {
    int w = width, p = precision, r;
    CHA_ASSERT(m16);
    for(r = 0; r < 4; r++) {
        fprintf(stream, "| %*.*f %*.*f %*.*f %*.*f |\n",
            w, p, m16[r], w, p, m16[r+4], w, p, m16[r+8], w, p, m16[r+12]
        );
    }
}
CHA_API_DEF void chm_Mat4_fprintd(const double* CHA_RESTRICT m16,FILE* stream) {chm_Mat4_fprintpd(m16,stream, 6, 2);}
CHA_API_DEF void chm_Mat4_printpd(const double* CHA_RESTRICT m16,int width, int precision)  {chm_Mat4_fprintpd(m16,stdout, width, precision);}
CHA_API_DEF void chm_Mat4_printd(const double* CHA_RESTRICT m16) {chm_Mat4_fprintpd(m16,stdout, 6, 2);}
#endif
extern float chm_QuatLength(const float* CHA_RESTRICT q4);
extern float* chm_QuatNormalized(float* CHA_RESTRICT result4,const float* CHA_RESTRICT q4);
extern void chm_QuatNormalize(float* CHA_RESTRICT q4);
CHA_API_DEF float* chm_QuatSlerpEps(float* CHA_RESTRICT result4,const float* CHA_RESTRICT a4,const float* CHA_RESTRICT b4,float slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/,float eps/*= CHA_SLERP_EPSILON*/)  {
    // Adapted from OgraMath (www.ogre3d.org AFAIR)

    //const int normalizeQOutAfterLerp = 1;            // When using Lerp instead of Slerp qOut should be normalized. However some users prefer setting eps small enough so that they can leave the Lerp as it is.
    //const float eps = CHA_SLERP_EPSILON;              // In [0 = 100% Slerp,1 = 100% Lerp] Faster but less precise with bigger epsilon (Lerp is used instead of Slerp more often). Users should tune it to achieve a performance boost.
    const float one = (float)1;
    const float *qStart=a4;
    float qEnd[4]={b4[0],b4[1],b4[2],b4[3]};
    float* qOut=result4;

    float fCos = qStart[0] * qEnd[0] + qStart[1] * qEnd[1] + qStart[2] * qEnd[2] + qStart[3] * qEnd[3];

    // Do we need to invert rotation?
    if(fCos < 0)	//Originally it was if(fCos < (float)0 && shortestPath)
        {fCos = -fCos;qEnd[0] = -qEnd[0];qEnd[1] = -qEnd[1];qEnd[2] = -qEnd[2];qEnd[3] = -qEnd[3];}

    if( fCos < one - eps)	// Originally if was "Ogre::Math::Abs(fCos)" instead of "fCos", but we know fCos>0, because we have hard coded shortestPath=true
    {
        // Standard case (slerp)
#       ifndef CHA_QUAT_SLERP_USE_ACOS_AND_SIN_INSTEAD_OF_ATAN2_AND_SQRT
        // Ogre::Quaternion uses this branch by default
        float fSin = sqrtf(one - fCos*fCos);
        float fAngle = atan2f(fSin, fCos);
#       else //CHA_QUAT_SLERP_USE_ACOS_AND_SIN_INSTEAD_OF_ATAN2_AND_SQRT
        // Possible replacement of the two lines above
        // (it's hard to tell if they're faster, but my instinct tells me I should trust atan2 better than acos (geometry geeks needed here...)):
        // But probably sin(...) is faster than (sqrt + 1 subtraction and mult)
        float fAngle = acosf(fCos);
        float fSin = sinf(fAngle);
#       endif //CHA_QUAT_SLERP_USE_ACOS_AND_SIN_INSTEAD_OF_ATAN2_AND_SQRT

        const float fInvSin = one / fSin;
        const float fCoeff0 = sin((one - slerpTime_In_0_1) * fAngle) * fInvSin;
        const float fCoeff1 = sin(slerpTime_In_0_1 * fAngle) * fInvSin;

        //qOut =  fCoeff0 * qStart + fCoeff1 * qEnd; //Avoided for maximum portability and conversion of the code
        qOut[0] = (fCoeff0 * qStart[0] + fCoeff1 * qEnd[0]);
        qOut[1] = (fCoeff0 * qStart[1] + fCoeff1 * qEnd[1]);
        qOut[2] = (fCoeff0 * qStart[2] + fCoeff1 * qEnd[2]);
        qOut[3] = (fCoeff0 * qStart[3] + fCoeff1 * qEnd[3]);
    } else
    {
        // There are two situations:
        // 1. "qStart" and "qEnd" are very close (fCos ~= +1), so we can do a linear
        //    interpolation safely.
        // 2. "qStart" and "qEnd" are almost inverse of each other (fCos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here.
        // IMPORTANT: CASE 2 can't happen anymore because we have hardcoded "shortestPath = true" and now fCos > 0

        const float fCoeff0 = one - slerpTime_In_0_1;
        const float fCoeff1 = slerpTime_In_0_1;

        //qOut =  fCoeff0 * qStart + fCoeff1 * qEnd; //Avoided for maximum portability and conversion of the code
        qOut[0] = (fCoeff0 * qStart[0] + fCoeff1 * qEnd[0]);
        qOut[1] = (fCoeff0 * qStart[1] + fCoeff1 * qEnd[1]);
        qOut[2] = (fCoeff0 * qStart[2] + fCoeff1 * qEnd[2]);
        qOut[3] = (fCoeff0 * qStart[3] + fCoeff1 * qEnd[3]);
        if (normalizeResult4AfterLerp)  chm_QuatNormalize(qOut);
    }

    return qOut;
}
CHA_API_DEF float* chm_QuatSlerp(float* CHA_RESTRICT result4,const float* CHA_RESTRICT a4,const float* CHA_RESTRICT b4,float slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/)  {return chm_QuatSlerpEps(result4,a4,b4,slerpTime_In_0_1,normalizeResult4AfterLerp,CHA_SLERP_EPSILON);}
CHA_API_DEF float* chm_QuatFromMat4(float* CHA_RESTRICT result4,const float* CHA_RESTRICT m16)  {
    // this code is glm based
    float* q=result4;const float* m=m16;
    float *qx=&q[0],*qy=&q[1],*qz=&q[2],*qw=&q[3];
    const float c00=m[0],c01=m[1],c02=m[2], c10=m[4],c11=m[5],c12=m[6], c20=m[8],c21=m[9],c22=m[10];

    float fourXSquaredMinus1 = c00 - c11 - c22, fourYSquaredMinus1 = c11 - c00 - c22;
    float fourZSquaredMinus1 = c22 - c00 - c11, fourWSquaredMinus1 = c00 + c11 + c22;
    float biggestVal,mult,fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    int biggestIndex = 0;

    if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourXSquaredMinus1;biggestIndex = 1;}
    if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourYSquaredMinus1;biggestIndex = 2;}
    if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourZSquaredMinus1;biggestIndex = 3;}

    biggestVal = sqrtf(fourBiggestSquaredMinus1 + (float)1) * (float)0.5;
    mult = (float)0.25 / biggestVal;

    switch  (biggestIndex)    {
    case 0:
        *qw = biggestVal; *qx = (c12 - c21) * mult; *qy = (c20 - c02) * mult; *qz = (c01 - c10) * mult;
        break;
    case 1:
        *qw = (c12 - c21) * mult; *qx = biggestVal; *qy = (c01 + c10) * mult; *qz = (c20 + c02) * mult;
        break;
    case 2:
        *qw = (c20 - c02) * mult; *qx = (c01 + c10) * mult; *qy = biggestVal; *qz = (c12 + c21) * mult;
        break;
    case 3:
        *qw = (c01 - c10) * mult; *qx = (c20 + c02) * mult; *qy = (c12 + c21) * mult; *qz = biggestVal;
        break;

    default:					// Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
        //CHA_ASSER(1);
        *qx=*qy=*qz=(float)0;*qw=(float)1;
        break;
    }
    return result4;
}
CHA_API_DEF float* chm_Mat4SetRotationFromQuat(float* CHA_RESTRICT result16,const float* CHA_RESTRICT q4)    {
    // this code is glm based
    const float one =(float)1,two=(float)2;
    float* m=result16;const float* q=q4;
    const float qx=q[0],qy=q[1],qz=q[2],qw=q[3];
    float *c00=&m[0],*c01=&m[1],*c02=&m[2], *c10=&m[4],*c11=&m[5],*c12=&m[6], *c20=&m[8],*c21=&m[9],*c22=&m[10];

    float qxx = (qx * qx), qyy = (qy * qy), qzz = (qz * qz);
    float qxz = (qx * qz), qxy = (qx * qy), qyz = (qy * qz);
    float qwx = (qw * qx), qwy = (qw * qy), qwz = (qw * qz);

    *c00 = one - two * (qyy +  qzz); *c01 = two * (qxy + qwz); *c02 = two * (qxz - qwy);
    *c10 = two * (qxy - qwz); *c11 = one - two * (qxx +  qzz); *c12 = two * (qyz + qwx);
    *c20 = two * (qxz + qwy); *c21 = two * (qyz - qwx); *c22 = one - two * (qxx +  qyy);

    return result16;
}
//--------------------------------------------------------------------------

// numVerts is the number of vertices (= number of 3 floats)
// numInds is the number of triangle indices (= 3 * num triangles)
// pNormsOut must be the same size as pVerts
CHA_API_PRIV void cha_calculate_vertex_normals(const float* pVerts,int numVerts,int vertsStrideInNumComponents,const unsigned short* pInds,int numInds,float* pNormsOut,int normsStrideInNumComponents)   {
    if (!pVerts || !pNormsOut || !pInds || numInds<3 || numVerts<3) return;
    // Calculate vertex normals
    {
        int i,f,iSz;float v21[3],v01[3],vcr[3];float len;
        if (vertsStrideInNumComponents<3)  vertsStrideInNumComponents = 3;
        if (normsStrideInNumComponents<3)  normsStrideInNumComponents = 3;
        for (i=0,iSz=numVerts*normsStrideInNumComponents;i<iSz;i+=normsStrideInNumComponents)	{pNormsOut[i]=pNormsOut[i+1]=pNormsOut[i+2]=0.f;}

        for (f = 0;f < numInds;f=f+3)	{
            const float* v0 = &pVerts[vertsStrideInNumComponents*(int)pInds[f]];
            const float* v1 = &pVerts[vertsStrideInNumComponents*(int)pInds[f+1]];
            const float* v2 = &pVerts[vertsStrideInNumComponents*(int)pInds[f+2]];

            float* n0 = &pNormsOut[normsStrideInNumComponents*(int)pInds[f]];
            float* n1 = &pNormsOut[normsStrideInNumComponents*(int)pInds[f+1]];
            float* n2 = &pNormsOut[normsStrideInNumComponents*(int)pInds[f+2]];

            v21[0] = v2[0]-v1[0];   v21[1] = v2[1]-v1[1];   v21[2] = v2[2]-v1[2];
            v01[0] = v0[0]-v1[0];   v01[1] = v0[1]-v1[1];   v01[2] = v0[2]-v1[2];

            //vcr = v21.cross(v01);
            vcr[0] = v21[1] * v01[2] - v21[2] * v01[1];
            vcr[1] = v21[2] * v01[0] - v21[0] * v01[2];
            vcr[2] = v21[0] * v01[1] - v21[1] * v01[0];

            n0[0]+=vcr[0];  n0[1]+=vcr[1];  n0[2]+=vcr[2];
            n1[0]+=vcr[0];  n1[1]+=vcr[1];  n1[2]+=vcr[2];
            n2[0]+=vcr[0];  n2[1]+=vcr[1];  n2[2]+=vcr[2];
        }
        for (i=0,iSz=numVerts*normsStrideInNumComponents;i<iSz;i+=normsStrideInNumComponents)	{
            float* n = &pNormsOut[i];
            len = n[0]*n[0]+n[1]*n[1]+n[2]*n[2];
            if (len>0.00000000001f)
            {len = sqrt(len);n[0]/=len;n[1]/=len;n[2]/=len;}
            else {
                n[0]=0.f;n[1]=1.f;n[2]=0.f;
            }
        }
    }
}

struct cha_armature_action_key_frame {float x,y,z,w,time;};    /* w is used only in rotation key frames */
/* Never used (TO REMOVE) */
#if 0
static int cha_armature_action_key_frame_cmp(const void* kfv0,const void* kfv1) {
    const struct cha_armature_action_key_frame* kf0 = (const struct cha_armature_action_key_frame*) kfv0;
    const struct cha_armature_action_key_frame* kf1 = (const struct cha_armature_action_key_frame*) kfv1;
    return kf1->time>kf0->time?-1:(kf1->time<kf0->time?1:0);
}
#endif
CHA_API_PRIV int cha_armature_action_key_frames_binary_search(const struct cha_armature_action_key_frame* keys,int num_items,const float time_to_search)  {
    /* returns values in [0,num_items], 'num_items' included! */
    int first=0, last=num_items-1;
    int mid;int cmp;
    float key_time = 0.f;
    if (num_items==0) return 0;  /* otherwise match will be 1 */
    while (first <= last) {
        mid = (first + last) / 2;
        key_time = keys[mid].time;
        cmp = key_time>time_to_search?-1:(key_time<time_to_search?1:0);
        if (cmp>0) first = mid + 1;
        else if (cmp<0) {
            if (mid==0) return 0;
            last = mid - 1;
        }
        else {CHA_ASSERT(mid<num_items); return mid;}
    }
    CHA_ASSERT(mid<num_items);
    return cmp>0 ? (mid+1) : mid;
}
struct cha_armature_action_bone_key_frame_stream {
    int num_translation_key_frames;
    struct cha_armature_action_key_frame* translation_key_frames;  /* w is ignored here */
    int num_rotation_key_frames;
    struct cha_armature_action_key_frame* rotation_key_frames;    /* w is used here */
};
void cha_armature_action_bone_key_frame_stream_init(struct cha_armature_action_bone_key_frame_stream* p,int num_rotation_key_frames,const struct cha_armature_action_key_frame* rotation_key_frames,int num_translation_key_frames,const struct cha_armature_action_key_frame* translation_key_frames)    {
    memset(p,0,sizeof(*p));
    p->num_translation_key_frames = num_translation_key_frames;
    if (p->num_translation_key_frames)  {
        p->translation_key_frames = (struct cha_armature_action_key_frame*) cha_malloc(p->num_translation_key_frames*sizeof(struct cha_armature_action_key_frame));
        memcpy(p->translation_key_frames,translation_key_frames,p->num_translation_key_frames*sizeof(struct cha_armature_action_key_frame));
    }
    p->num_rotation_key_frames = num_rotation_key_frames;
    if (p->num_rotation_key_frames)  {
        p->rotation_key_frames = (struct cha_armature_action_key_frame*) cha_malloc(p->num_rotation_key_frames*sizeof(struct cha_armature_action_key_frame));
        memcpy(p->rotation_key_frames,rotation_key_frames,p->num_rotation_key_frames*sizeof(struct cha_armature_action_key_frame));
    }
}
void cha_armature_action_bone_key_frame_stream_destroy(struct cha_armature_action_bone_key_frame_stream* p)    {
    if (p->translation_key_frames) {cha_free(p->translation_key_frames);p->translation_key_frames=NULL;}
    p->num_translation_key_frames=0;
    if (p->rotation_key_frames) {cha_free(p->rotation_key_frames);p->rotation_key_frames=NULL;}
    p->num_rotation_key_frames=0;
}

struct cha_armature_action {
    char name[128];
    int idx;
    float min_frame_time,max_frame_time;
    float duration;
    float ticks_per_second;
    float num_ticks;
    int looping;
    unsigned bone_mask; /* of all the bones affected by this action */
#   ifdef CHA_ARMATURE_ACTION_USER_CODE
    CHA_ARMATURE_ACTION_USER_CODE
#   endif
};



enum ChaBoneSpace {
    CHA_BONE_SPACE_BONE=0,
    CHA_BONE_SPACE_ARMATURE=1,
    CHA_BONE_SPACE_SKINNING=2,
    CHA_BONE_SPACE_GRABBING=3
};
struct cha_armature_bone_space_data {
    float head[3],tail[3]; /* pos of tip and base of the bone */
    float roll;  /* bone roll (radians) around the head/tail axis; */
    float matrix[16],matrix_inv[16];
};
struct cha_armature_bone {
    char name[128];
    struct cha_armature_bone_space_data data[2];    /* */
    float length;   /* = sqrt(Sum((head[i]-tail[i])*(head[i]-tail[i]))) [using 'arm_head' and 'arm_tail' gives the same result] */
    float post_pose_armature_space_matrix[16];  /* internally used to speed up calculation of CHA_BONE_SPACE_GRABBING pose_matrices */

    int idx;        /* index inside cha_armature::bones */
    int parent_idx; /* -1 if root bone */
    int mirror_idx; /* == idx if no mirror bone */
    int num_children;
    int* children_idx;

    struct cha_armature_action_bone_key_frame_stream** key_frame_streams;   /* one stream per action (size: num_actions) */
#   ifdef CHA_ARMATURE_BONE_USER_CODE
    CHA_ARMATURE_BONE_USER_CODE
#   endif
};
struct cha_armature {
    char name[128];
    int idx;        /* index inside a global array */
    int num_bones;
    struct cha_armature_bone* bones; /* size -> num_bones */
    int num_actions;
    struct cha_armature_action* actions; /* size -> num_actions (but their key_frames are inside 'bones') */
#   ifdef CHA_ARMATURE_USER_CODE
    CHA_ARMATURE_USER_CODE
#   endif
};
/*-------- GLOBALS ------------------------------------------------*/
const int gNumCharacterArmatures = CHA_ARMATURE_NAME_COUNT;
struct cha_armature gCharacterArmatures[CHA_ARMATURE_NAME_COUNT] = CHA_ZERO_INIT;
/*-----------------------------------------------------------------*/
void cha_armature_init(struct cha_armature* p,const char* name,int armature_idx,int num_bones,const char** bone_names,const int* bone_parent_idx,const int* bone_mirror_idx,const struct cha_armature_bone_space_data* bone_data_pair_per_bone)    {
    int i=0,j=0,k=0;size_t len;float tmp[16];//const float mId16[16]={1,0,0,0,   0,1,0,0,    0,0,1,0,    0,0,0,1};
    const struct cha_armature_bone_space_data* p_bone_data_pair = bone_data_pair_per_bone;
    memset(p,0,sizeof(*p));
    CHA_ASSERT(armature_idx>=0 && armature_idx<CHA_ARMATURE_NAME_COUNT && name);
    len = strlen(name);
    if (len<127) strcpy(p->name,name);
    else {strncpy(p->name,name,127);p->name[127]='\0';}
    p->idx = armature_idx;
    p->num_bones = num_bones;
    p->bones = (struct cha_armature_bone*) cha_malloc(p->num_bones*sizeof(struct cha_armature_bone));
    CHA_ASSERT(CHA_BONE_NAME_ROOT==0);  /* 'root' must be the first bone */
    for (i=0;i<p->num_bones;i++)    {
        struct cha_armature_bone* b = &p->bones[i];
        const char* name = bone_names[i];
        const int parent_idx = bone_parent_idx[i];
        const int mirror_idx = bone_mirror_idx[i];
        float length[2]={0.f,0.f};
        memset(b,0,sizeof(*b));
        len = strlen(name);
        if (len<127) strcpy(b->name,name);
        else {strncpy(b->name,name,127);b->name[127]='\0';}
        b->idx = i;
        b->parent_idx = parent_idx;
        CHA_ASSERT(b->parent_idx>=-1 && b->parent_idx<p->num_bones && b->parent_idx!=b->idx);
        CHA_ASSERT(b->parent_idx<b->idx);   /* this way we can just loop bones (without recursion) in some cases */
        b->mirror_idx = mirror_idx;
        CHA_ASSERT(b->mirror_idx>=-1 && b->mirror_idx<p->num_bones);
        memcpy(b->data,p_bone_data_pair,2*sizeof(struct cha_armature_bone_space_data));
        p_bone_data_pair+=2;
        /* calculate p->length */
        for (j=0;j<2;j++)   {
            struct cha_armature_bone_space_data* d = &b->data[j];
            length[j]=0.f;
            for (k=0;k<3;k++)   {length[j]+=(d->head[k]-d->tail[k])*(d->head[k]-d->tail[k]);}
            length[j]=sqrtf(length[j]);
        }
        CHA_ASSERT(fabs(length[0]-length[1])<0.0001f);
        b->length = (length[0]+length[1])*0.5f;
    }
    /* discover p->num_children */
    for (i=0;i<p->num_bones;i++)    {
        struct cha_armature_bone* b = &p->bones[i];
        if (b->parent_idx>=0) ++p->bones[b->parent_idx].num_children;
    }
    /* allocate p->children_idx */
    for (i=0;i<p->num_bones;i++)    {
        struct cha_armature_bone* b = &p->bones[i];
        if (b->num_children) {
            b->children_idx = (int*) cha_malloc(b->num_children*sizeof(int));
            for (j=0;j<b->num_children;j++) b->children_idx[j] = -1;
        }
    }
    /* fill p->children_idx */
    for (i=0;i<p->num_bones;i++)    {
        struct cha_armature_bone* b = &p->bones[i];
        CHA_ASSERT(b->idx == i);
        if (b->parent_idx>=0) {
            struct cha_armature_bone* parent = &p->bones[b->parent_idx];
            int ok = 0;
            for (j=0;j<parent->num_children;j++) {
                CHA_ASSERT(parent->children_idx[j]!=i);
                if (parent->children_idx[j] == -1)  {
                    parent->children_idx[j]=i;
                    ok = 1;break;
                }
            }
            CHA_ASSERT(ok);
        }
    }

    /* here we:
       -> modify b->data[CHA_BONE_SPACE_BONE].matrix to include the translation part that is missing,
          so that now it is the 'off_bone' matrix referenced in the docs
       -> also, calculate the two 'matrix_inv' here
       -> and, finally, calculate post_pose_armature_space_matrix
    */
    for (i=0;i<p->num_bones;i++)    {
        struct cha_armature_bone* b = &p->bones[i];
        struct cha_armature_bone_space_data* dbs = &b->data[CHA_BONE_SPACE_BONE];
        struct cha_armature_bone_space_data* das = &b->data[CHA_BONE_SPACE_ARMATURE];
        float* m = dbs->matrix;
        float* ppm = b->post_pose_armature_space_matrix;
        CHA_ASSERT(m[12]==0.f && m[13]==0.f && m[14]==0.f); /* it was a mat3x3 in Blender (unlike the mat4x4 dbs->matrix)! */
        m[12] = dbs->head[0];
        m[13] = dbs->head[1]+(b->parent_idx>=CHA_BONE_NAME_ROOT?p->bones[b->parent_idx].length:0.f);
//        m[13] = dbs->head[1]+(b->parent_idx>CHA_BONE_NAME_ROOT?p->bones[b->parent_idx].length:0.f);
        m[14] = dbs->head[2];
        chm_Mat4InvertTransformMatrixf(dbs->matrix_inv,dbs->matrix); /* but we can use the faster version too */
        chm_Mat4InvertTransformMatrixf(das->matrix_inv,das->matrix);
        /* we calculate 'ppm' here */
        ppm[0]=ppm[5]=ppm[10]=ppm[15]=1.f;if (i!=CHA_BONE_NAME_ROOT) ppm[13]=b->length; /* we should exclude 'ppm[13]=b->length' when 'i==CHA_BONE_NAME_ROOT' if the head of our root bone is 'grounded', not the tail */
        chm_Mat4Copyf(tmp,b->data[CHA_BONE_SPACE_ARMATURE].matrix_inv);
        tmp[12]=tmp[13]=tmp[14]=0.f;   /* we just need the rotation part of 'tmp' */
        chm_Mat4Mulf(ppm,ppm,tmp);
        if (i!=CHA_BONE_NAME_ROOT) chm_Mat4Rotatef(ppm,90.f,1.f,0.f,0.f);   /* not sure why this constant is necessary */
//#       define TEST_TO_REMOVE
#       ifdef TEST_TO_REMOVE
        // we test that: b->arm_space.matrix = b->parent->arm_space.matrix * b->off_bone; that now translates in:
        // b->data[CHA_BONE_SPACE_ARMATURE].matrix = b->parent->data[CHA_BONE_SPACE_ARMATURE].matrix * b->data[CHA_BONE_SPACE_BONE].matrix;

        {
            float arm_matrix[16];int j,ok;
            const float identity16[16]={1,0,0,0,    0,1,0,0,    0,0,1,0,    0,0,0,1};
            const float* arm_space_matrix = b->data[CHA_BONE_SPACE_ARMATURE].matrix;
            const float* bone_space_matrix = b->data[CHA_BONE_SPACE_BONE].matrix;
            const float* arm_space_parent_matrix = b->parent_idx>=0?p->bones[b->parent_idx].data[CHA_BONE_SPACE_ARMATURE].matrix:identity16;
            chm_Mat4Mul(arm_matrix,arm_space_parent_matrix,bone_space_matrix);
            /* we must check that: arm_matrix[16]==arm_space_matrix[16] */
            ok = 1;
            for (j=0;j<16;j++)  {
                const float v0 = arm_space_matrix[j];
                const float v1 = arm_matrix[j];
                if (fabs(v0-v1)>0.0001f) {ok=0;break;}
            }
            if (!ok)    {
                printf("cha_armature_init(...): off_bone test failed at bone[%d][\"%s\"]:\n",i,b->name);
                chm_Mat4_print(arm_space_matrix);
                chm_Mat4_print(arm_matrix);
            }
        }
#       endif

    }

}
void cha_armature_destroy(struct cha_armature* p)   {
    int i,j;
    if (p->num_actions) {
        for (i=0;i<p->num_bones;i++)   {
            struct cha_armature_bone* b = &p->bones[i];
            if (b->key_frame_streams)   {
                for (j=0;j<p->num_actions;j++)  {
                    struct cha_armature_action_bone_key_frame_stream* kfstream = b->key_frame_streams[j];
                    if (kfstream)   {
                        cha_armature_action_bone_key_frame_stream_destroy(kfstream);
                        cha_free(kfstream);
                    }
                }
                cha_free(b->key_frame_streams);b->key_frame_streams=NULL;
            }
        }
    }
    if (p->actions) {cha_free(p->actions);p->actions=NULL;}
    p->num_actions=0;
    for (i=0;i<p->num_bones;i++)   {
        struct cha_armature_bone* b = &p->bones[i];
        if (b->children_idx)    {cha_free(b->children_idx);b->children_idx=NULL;}
        b->num_children = 0;

    }
    if (p->bones) {cha_free(p->bones);p->bones=NULL;}
    p->num_bones=0;p->name[0]='\0';p->idx=-1;
}
void cha_armature_add_actions(struct cha_armature* p,int num_actions,const char** action_names,const int* num_key_frames_arranged,const struct cha_armature_action_key_frame* key_frames,int num_key_frames,float frs_sec /* default is 24.f */)   {
    int i,j,k;
    const struct cha_armature_action_key_frame* pkf = key_frames;
    CHA_ASSERT(!p->actions && p->num_actions==0);
    p->num_actions = num_actions;
    p->actions = (struct cha_armature_action*) cha_malloc(p->num_actions*sizeof(struct cha_armature_action));
    memset(p->actions,0,p->num_actions*sizeof(struct cha_armature_action));
    for (i=0;i<p->num_actions;i++)  {
        struct cha_armature_action* act = &p->actions[i];
        const char* name = action_names[i];
        size_t len = strlen(name);
        act->idx = i;
        if (len<127) strcpy(act->name,name);
        else {strncpy(act->name,name,127);act->name[127]='\0';}    
        if (len>1)  {
            if (strstr(act->name,"cycle") || strstr(act->name,"Cycle") || strstr(act->name,"CYCLE") ||
                strstr(act->name,"loop") || strstr(act->name,"Loop") || strstr(act->name,"LOOP")    ) act->looping = 1;
            /* We should REMOVE this if/else block */
            if ((act->name[0]=='W' || act->name[0]=='w') && (strncmp(&act->name[1],"alk",3)==0 || strncmp(&act->name[1],"ALK",3)==0))  {
                /* it's the walk cycle*/
                act->looping=1;
                CHA_ASSERT(i==CHA_ARMATURE_ACTION_NAME_CYCLE_WALK);
            }
            else if ((act->name[0]=='R' || act->name[0]=='r') && (strncmp(&act->name[1],"un",2)==0 || strncmp(&act->name[1],"UN",2)==0))  {
                /* it's the run cycle*/
                act->looping=1;
                CHA_ASSERT(i==CHA_ARMATURE_ACTION_NAME_CYCLE_RUN);
            }
        }
    }
    /* allocate and reset all action-related fields inside each bone */
    CHA_ASSERT(p->bones && p->num_bones>0);
    for (i=0;i<p->num_bones;i++)    {
        struct cha_armature_bone* b = &p->bones[i];
        CHA_ASSERT(!b->key_frame_streams);
        b->key_frame_streams = (struct cha_armature_action_bone_key_frame_stream**) cha_malloc(p->num_actions*sizeof(struct cha_armature_action_bone_key_frame_stream*));
        memset(b->key_frame_streams,0,p->num_actions*sizeof(struct cha_armature_action_bone_key_frame_stream*));
        for (j=0;j<p->num_actions;j++)  {
            struct cha_armature_action* act = &p->actions[j];
            const int num_translation_key_frames = i!=0 ? 0 : num_key_frames_arranged[(p->num_bones+1)*j+i];
            const int num_rotation_key_frames = num_key_frames_arranged[(p->num_bones+1)*j+i+1];

            struct cha_armature_action_bone_key_frame_stream* kf_stream;
            /*printf("Bone[%d]Action[%d]: num_translation_key_frames=%d; num_rotation_key_frames=%d\n",i,j,num_translation_key_frames,num_rotation_key_frames);*/

            CHA_ASSERT(b->key_frame_streams[j]==NULL);
            b->key_frame_streams[j] = (struct cha_armature_action_bone_key_frame_stream*) cha_malloc(sizeof(struct cha_armature_action_bone_key_frame_stream));
            kf_stream = b->key_frame_streams[j];
            memset(kf_stream,0,sizeof(struct cha_armature_action_bone_key_frame_stream));
            kf_stream->num_translation_key_frames = num_translation_key_frames;
            if (kf_stream->num_translation_key_frames)  {
                kf_stream->translation_key_frames = (struct cha_armature_action_key_frame*) cha_malloc(kf_stream->num_translation_key_frames*sizeof(struct cha_armature_action_key_frame));
                memset(kf_stream->translation_key_frames,0,kf_stream->num_translation_key_frames*sizeof(struct cha_armature_action_key_frame));
             }
            kf_stream->num_rotation_key_frames = num_rotation_key_frames;
            if (kf_stream->num_rotation_key_frames)  {
                kf_stream->rotation_key_frames = (struct cha_armature_action_key_frame*) cha_malloc(kf_stream->num_rotation_key_frames*sizeof(struct cha_armature_action_key_frame));
                memset(kf_stream->rotation_key_frames,0,kf_stream->num_rotation_key_frames*sizeof(struct cha_armature_action_key_frame));
            }
            if (kf_stream->num_translation_key_frames || kf_stream->num_rotation_key_frames)    {
                act->bone_mask|=(1<<(unsigned)j);
            }
        }
    }
    /* fill them all with proper key-frame streams */
    for (j=0;j<p->num_actions;j++)  {
        struct cha_armature_action* act = &p->actions[j];
        float min_frame_time=100000000.f,max_frame_time=0.f;
        int num_total_key_frames=0; /* of action 'act' (we could store it, if it's useful) */
        for (i=0;i<p->num_bones;i++)    {
            struct cha_armature_bone* b = &p->bones[i];
            struct cha_armature_action_bone_key_frame_stream* kf_stream = b->key_frame_streams[j];
            float last_time_check = 0.f;
            for (k=0;k<kf_stream->num_translation_key_frames;k++)   {
                const struct cha_armature_action_key_frame *ckf = pkf++;
                struct cha_armature_action_key_frame *kf = &kf_stream->translation_key_frames[k];
                CHA_ASSERT(ckf);
                CHA_ASSERT(last_time_check<ckf->time);
                memcpy(kf,ckf,sizeof(struct cha_armature_action_key_frame));     /* fill kf */
                last_time_check = ckf->time;
                if (max_frame_time<ckf->time) max_frame_time=ckf->time;
                if (min_frame_time>ckf->time) min_frame_time=ckf->time;
            }
            num_total_key_frames+=kf_stream->num_translation_key_frames;
            /*qsort(kf_stream->translation_key_frames,sizeof(struct cha_armature_action_key_frame),kf_stream->num_translation_key_frames,&cha_armature_action_key_frame_cmp);*/
            last_time_check = 0.f;
            for (k=0;k<kf_stream->num_rotation_key_frames;k++)   {
                const struct cha_armature_action_key_frame *ckf = pkf++;
                struct cha_armature_action_key_frame *kf = &kf_stream->rotation_key_frames[k];
                CHA_ASSERT(ckf);
                CHA_ASSERT(last_time_check<ckf->time);
                memcpy(kf,ckf,sizeof(struct cha_armature_action_key_frame));     /* fill kf */
                last_time_check = ckf->time;
                if (max_frame_time<ckf->time) max_frame_time=ckf->time;
                if (min_frame_time>ckf->time) min_frame_time=ckf->time;
            }
            num_total_key_frames+=kf_stream->num_rotation_key_frames;
            /*qsort(kf_stream->rotation_key_frames,sizeof(struct cha_armature_action_key_frame),kf_stream->num_rotation_key_frames,&cha_armature_action_key_frame_cmp);            */
        }

        act->ticks_per_second = frs_sec > 0 ? frs_sec : 24;
        if (num_total_key_frames>0) {
            act->min_frame_time = min_frame_time;
            act->max_frame_time = max_frame_time;
            act->num_ticks = act->max_frame_time-act->min_frame_time+1.f;   /* this is because key_frame_time is usually the 'frame tick index', but... is this correct? Why do actions usually start at key_frame 1? */
            act->duration = act->num_ticks/act->ticks_per_second;
        }
    }

    {
        const int cnt = (int)(pkf-key_frames);
        CHA_ASSERT(cnt==num_key_frames);
    }
}
void cha_armature_display(struct cha_armature* p) {
    int i,j;
    CHA_ASSERT(strlen(p->name)>0);
    printf("\t%d) name: \"%s\"\tnum_bones=%d\n",p->idx,p->name,p->num_bones);
    if (p->bones)   {
        /* small safety check */
        for (i=0;i<p->num_bones;i++) {
            const struct cha_armature_bone* b = &p->bones[i];
            CHA_ASSERT(strlen(b->name)>0);
            CHA_ASSERT(i==b->idx);
        }
        /* end small safety check */
        for (i=0;i<p->num_actions;i++)  {
            const struct cha_armature_action* a = &p->actions[i];
            printf("\t\taction[%d][\"%s\"]={num_ticks:%1.3ff,ticks_per_sec=%1.3ff,duration=%1.3ff};\n",i,a->name,a->num_ticks,a->ticks_per_second,a->duration);
        }
        printf("\n");
        for (i=0;i<p->num_bones;i++) {
            const struct cha_armature_bone* b = &p->bones[i];
            const struct cha_armature_bone_space_data* da = &b->data[0];
            const struct cha_armature_bone_space_data* db = &b->data[1];
            printf("\t\tbone[%d][\"%s\"]:\t[len=%1.5f]\tparent_idx=[%d][\"%s\"]\tmirror_idx=[%d][%s]\t",i,b->name,b->length,b->parent_idx,b->parent_idx>=0?p->bones[b->parent_idx].name:"NULL",b->mirror_idx,(b->mirror_idx>=0 && b->mirror_idx!=b->idx)?p->bones[b->mirror_idx].name:"NULL");
            for (j=0;j<b->num_children;j++) printf("children_idx[%d]=[%d][\"%s\"]; ",j,b->children_idx[j],b->children_idx[j]>=0?p->bones[b->children_idx[j]].name:"NULL");
            printf("\n\t\t\tdata[ARMATURE_SPACE]:\thead={%1.5ff,%1.5ff,%1.5ff};\ttail={%1.5ff,%1.5ff,%1.5ff};\troll=%1.5ff\n",
                   da->head[0],da->head[1],da->head[2],
                    da->tail[0],da->tail[1],da->tail[2],
                    da->roll);
            printf("\t\t\tmat[16]:\n");chm_Mat4_printf(da->matrix);printf("\n");
            printf("\t\t\tmat_inv[16]:\n");chm_Mat4_printf(da->matrix_inv);printf("\n");

            printf("\t\t\tdata[BONE_SPACE]:\thead={%1.5ff,%1.5ff,%1.5ff};\ttail={%1.5ff,%1.5ff,%1.5ff};\troll=%1.5ff\n",
                   db->head[0],db->head[1],db->head[2],
                    db->tail[0],db->tail[1],db->tail[2],
                    db->roll);

            printf("\t\t\tmat[16]:\n");chm_Mat4_printf(db->matrix);printf("\n");
            printf("\t\t\tmat[16]:\n");chm_Mat4_printf(db->matrix_inv);printf("\n");

            if (b->key_frame_streams)   {
                for (j=0;j<p->num_actions;j++)  {
                    const struct cha_armature_action_bone_key_frame_stream* kfstream = b->key_frame_streams[j];
                    if (kfstream && (kfstream->num_rotation_key_frames || kfstream->num_translation_key_frames))   {
                        int k;
                        printf("\t\t\tkey_frame_stream_for_action[%d][%s]:\n",j,p->actions[j].name);
                        if (kfstream->num_translation_key_frames)   {
                            printf("\t\t\t\ttra_keys: {[%d]:\t",kfstream->num_translation_key_frames);
                            for (k=0;k<kfstream->num_translation_key_frames;k++)    {
                                const struct cha_armature_action_key_frame* kf = &kfstream->translation_key_frames[k];
                                printf("{%1.3ff,%1.3ff,%1.3ff;%1.3ff}",kf->x,kf->y,kf->z,kf->time);
                                if (k!=kfstream->num_translation_key_frames-1) printf(",");
                                if (k%8==0) printf("\n\t\t\t\t");
                            }
                            printf("};\n");
                        }
                        if (kfstream->num_rotation_key_frames)   {
                            printf("\t\t\t\trot_keys: {[%d]:\t",kfstream->num_rotation_key_frames);
                            for (k=0;k<kfstream->num_rotation_key_frames;k++)    {
                                const struct cha_armature_action_key_frame* kf = &kfstream->rotation_key_frames[k];
                                printf("{%1.3ff,%1.3ff,%1.3ff,%1.3ff;%1.3ff}",kf->x,kf->y,kf->z,kf->w,kf->time);
                                if (k!=kfstream->num_rotation_key_frames-1) printf(",");
                                if (k%6==0) printf("\n\t\t\t\t");
                            }
                            printf("};\n");
                        }
                    }
                }

            }
        }

    }
}

struct cha_mesh_part {
    short material_idx;
    int inds_start,inds_count;
#   ifdef CHA_MESH_PART_USER_CODE
    CHA_MESH_PART_USER_CODE
#   endif
};
void cha_mesh_part_init(struct cha_mesh_part* p,short material_idx,int inds_start,int inds_count)  {
    memset(p,0,sizeof(*p));
    p->material_idx = material_idx;
    p->inds_start = inds_start;
    p->inds_count = inds_count;
}
void cha_mesh_part_destroy(struct cha_mesh_part* p)    {p->material_idx=-1;p->inds_start=p->inds_count=0;}
struct cha_mesh_shape_key {
    char name[128];
    float *verts,*norms;int num_verts;      /* 'num_verts' must match the total num_mesh of the mesh (and it's the same for all shape keys) */
#   ifdef CHA_USE_VBO
    GLuint vbo,vao; /* 'vbo' with 'verts' 'norms'; ibo with 'inds', but used only for NON-animated shape_keys */
#   endif
#   ifdef CHA_MESH_SHAPE_KEY_USER_CODE
    CHA_MESH_SHAPE_KEY_USER_CODE
#   endif
};
void cha_mesh_shape_key_init(struct cha_mesh_shape_key* p,const char* name,const float* verts,int num_verts,const unsigned short* inds,int num_inds)  {
    const size_t len = strlen(name);
    memset(p,0,sizeof(*p));
    if (len<127) strcpy(p->name,name);
    else {strncpy(p->name,name,127);p->name[127]='\0';}
    p->num_verts = num_verts;
    p->verts = (float*) cha_malloc(3*p->num_verts*sizeof(float));
    memcpy(p->verts,verts,3*p->num_verts*sizeof(float));
    p->norms = (float*) cha_malloc(3*p->num_verts*sizeof(float));
    cha_calculate_vertex_normals(p->verts,p->num_verts,3,inds,num_inds,p->norms,3);
}
void cha_mesh_shape_key_destroy(struct cha_mesh_shape_key* p)    {
    if (p->verts) {cha_free(p->verts);p->verts = NULL;}
    if (p->norms) {cha_free(p->norms);p->norms = NULL;}
    p->num_verts=0;p->name[0] = '\0';
#   ifdef CHA_USE_VBO
    if (p->vbo) {glDeleteBuffers(1,&p->vbo);p->vbo=0;}
#   ifdef CHA_HINT_USE_VAO
    if (p->vao) {glDeleteVertexArrays(1,&p->vao);p->vao=0;}
#   endif
#   endif
}
struct cha_mesh_vertex_weight {
    int bone_idx;     /* -1 means that this vertex has less of 3 weights and we can skip the following weights of this vertex */
    float weight;
};


struct cha_mesh {
    char name[128];
    int idx;    /* index of this mesh into a global array */
    unsigned short* inds;int num_inds;
    float* verts;float* norms;int num_verts;
    struct cha_mesh_shape_key* shape_keys;int num_shape_keys;int shape_keys_type_static;    /* 'static' = non-animated. Used when the first shape key is not named "Basis" */
    struct cha_mesh_part* parts;int num_parts; /* these are basically mesh indices per material */
    /* the following fields are used only by the 'man' mesh (armature-related stuff) */
    struct cha_mesh_vertex_weight* weights;int num_weights;  /* num_weights==num_verts*3, because we use a maximum of 3 weights per vertex */
    unsigned* weight_bone_masks;    /* size=num_verts. Each mask marks the bones that influence each vertex (unsigned is 32-bit -> so max 32 bones). Used in software skinning to skip transforming unmodified vertices. */
    float aabb_min[3],aabb_max[3];  /* based on untouched 'verts' */
    float aabb_center[3],aabb_half_extents[3];

    /* the following are WIP */
    int armature_idx;       /* when >=0: 'weights' if present */
    int parent_bone_idx;    /* when >=0: mesh is placed according to this bone, belonging to the armature of 'parent_mesh_id', that must be >=0 too. TODO: 'armature_idx'>=0 for this mesh is probably not supported. */
    int parent_mesh_idx;    /* when >=0 mesh if transformed relatively to this mesh (note that 'parent_bone_idx' can be -1) */
    int num_child_meshes;int* child_mesh_idx; /* size=num_children */
    float parent_offset_matrix[16];  /* used when some valid 'parent' is set */
    float parent_offset_matrix_link[16];int parent_offset_matrix_link_present;  /* currently just used as a hack for displaying 2 eyes with a single eye mesh */

#   ifdef CHA_USE_VBO
    GLuint vbo,vao,ibo; /* 'vbo' with 'verts' 'norms'; ibo with 'inds' */
#   endif

#   ifdef CHA_MESH_USER_CODE
    CHA_MESH_USER_CODE
#   endif
};

struct cha_material_names {char names[CHA_MATERIAL_NAME_COUNT][128];};
/*-------- GLOBALS ------------------------------------------------*/
const int gNumCharacterMeshes = CHA_MESH_NAME_COUNT;
struct cha_mesh gCharacterMeshes[CHA_MESH_NAME_COUNT] = CHA_ZERO_INIT;
struct cha_material_names gMaterialNames;
/*-----------------------------------------------------------------*/
void cha_mesh_destroy(struct cha_mesh* p) {
    int i;
    p->name[0]='\0';p->idx=-1;

    if (p->num_weights) {cha_free(p->weights);p->weights=NULL;}
    p->num_weights=0;
    if (p->weight_bone_masks) {cha_free(p->weight_bone_masks);p->weight_bone_masks=NULL;}

    if (p->shape_keys)  {
        for (i=0;i<p->num_shape_keys;i++) cha_mesh_shape_key_destroy(&p->shape_keys[i]);
        cha_free(p->shape_keys);p->shape_keys=NULL;
    }
    p->num_shape_keys=0;

    if (p->parts)  {
        for (i=0;i<p->num_parts;i++) cha_mesh_part_destroy(&p->parts[i]);
        cha_free(p->parts);p->parts=NULL;
    }
    p->num_parts=0;

    if (p->inds) {cha_free(p->inds);p->inds = NULL;}
    p->num_inds=0;

    if (p->verts) {cha_free(p->verts);p->verts = NULL;}
    if (p->norms) {cha_free(p->norms);p->norms = NULL;}
    p->num_verts=0;

    if (p->child_mesh_idx) {cha_free(p->child_mesh_idx);p->child_mesh_idx=NULL;}
    p->num_child_meshes=0;

#   ifdef CHA_USE_VBO
    if (p->vbo) {glDeleteBuffers(1,&p->vbo);p->vbo=0;}
    if (p->ibo) {glDeleteBuffers(1,&p->ibo);p->ibo=0;}
#   ifdef CHA_HINT_USE_VAO
    if (p->vao) {glDeleteVertexArrays(1,&p->vao);p->vao=0;}
#   endif
#   endif
}
void cha_mesh_display(struct cha_mesh* p) {
    int i,j;
    printf("\t%d)\"%s\": num_verts=%d (aabb_center={%1.3ff,%1.3ff,%1.3ff};aabb_half_extents={%1.3ff,%1.3ff,%1.3ff};)\n",p->idx,p->name,p->num_verts,p->aabb_center[0],p->aabb_center[1],p->aabb_center[2],p->aabb_half_extents[0],p->aabb_half_extents[1],p->aabb_half_extents[2]);
    if (p->num_shape_keys)   {
        for (i=0;i<p->num_shape_keys;i++) {
            const struct cha_mesh_shape_key* sk = &p->shape_keys[i];
            CHA_ASSERT(strlen(sk->name)>0);
            printf("\t\tshapeKeys[%d]: \"%s\"\n",i,sk->name);
        }
    }
    for (i=0;i<p->num_parts;i++) {
        const struct cha_mesh_part* part = &p->parts[i];
        CHA_ASSERT(part->material_idx>=0);
        CHA_ASSERT(part->inds_count%3==0);
        printf("\t\tparts[%d]: matIdx=%d num_triangles=%d\n",i,part->material_idx,part->inds_count/3);
    }
    if (p->num_weights) {
        int minBoneIdx=10000000,maxBoneIdx=-100000000;float sum=0.f;
        printf("\t\tmesh has: %d bone weights (3*%d).",p->num_weights,p->num_verts);
        CHA_ASSERT(p->num_weights=p->num_verts*3);    /* we use 3 weights per vertex */
        for (i=0;i<p->num_verts;i++) {
            sum = 0.f;
            for (j=0;j<3;j++)   {
                const struct cha_mesh_vertex_weight* w = &p->weights[3*i+j];
                if (w->bone_idx>=0) {
                    sum+=w->weight;
                    if (minBoneIdx>w->bone_idx) minBoneIdx=w->bone_idx;
                    else if (maxBoneIdx<w->bone_idx) maxBoneIdx=w->bone_idx;
                }
            }
            CHA_ASSERT(sum>0.999f && sum<1.0001f);    /* weights must be normalized */
        }
        printf(" boneIdx are in [%d,%d].\n",minBoneIdx,maxBoneIdx);
    }
    if (p->num_child_meshes)    {
        printf("\t\tmesh has %d child meshes: {",p->num_child_meshes);
        for (i=0;i<p->num_child_meshes;i++) {
            j = p->child_mesh_idx[i];
            if (j<=0 || j>=CHA_MESH_NAME_COUNT) printf("%d",j);
            printf("\"%s\"",gCharacterMeshes[j].name);
            if (i!=p->num_child_meshes-1) printf(",");
        }
        printf("};\n");
    }
}
void cha_mesh_init(struct cha_mesh* p,const char* name,int mesh_idx,const float* verts,int num_verts,const unsigned short* inds,int num_inds) {
    size_t len;
    const float id16[16] = {1.f,0.f,0.f,0.f,    0.f,1.f,0.f,0.f,    0.f,0.f,1.f,0.f,    0.f,0.f,0.f,1.f};
    memset(p,0,sizeof(*p));
    CHA_ASSERT(mesh_idx>=0 && mesh_idx<CHA_MESH_NAME_COUNT && name);
    len = strlen(name);
    if (len<127) strcpy(p->name,name);
    else {strncpy(p->name,name,127);p->name[127]='\0';}
    p->idx = mesh_idx;
    p->armature_idx=p->parent_bone_idx=p->parent_mesh_idx=-1;
    p->parent_offset_matrix[0]=p->parent_offset_matrix[5]=p->parent_offset_matrix[10]=p->parent_offset_matrix[15]=1.f;  /* identity matrix */
    p->num_inds = num_inds;
    p->inds = (unsigned short*) cha_malloc(p->num_inds*sizeof(unsigned short));
    memcpy(p->inds,inds,p->num_inds*sizeof(unsigned short));
    p->num_verts = num_verts;
    p->verts = (float*) cha_malloc(3*p->num_verts*sizeof(float));
    memcpy(p->verts,verts,3*p->num_verts*sizeof(float));
    p->norms = (float*) cha_malloc(3*p->num_verts*sizeof(float));
    cha_calculate_vertex_normals(p->verts,p->num_verts,3,p->inds,p->num_inds,p->norms,3);
    memcpy(p->parent_offset_matrix,id16,16*sizeof(float));
    memcpy(p->parent_offset_matrix_link,id16,16*sizeof(float));
    if (p->verts && p->num_verts)   {
        int i,k;
        for (k=0;k<3;k++) p->aabb_min[k]=p->aabb_max[k]=p->verts[k];
        for (i=1;i<p->num_verts;i++)    {
            const float* v = &p->verts[3*i];
            for (k=0;k<3;k++) {
                const float vk = v[k];
                if (p->aabb_min[k]>vk) p->aabb_min[k]=vk;
                else if (p->aabb_max[k]<vk) p->aabb_max[k]=vk;
            }
        }
        for (k=0;k<3;k++) {
            p->aabb_center[k]=(p->aabb_max[k]+p->aabb_min[k])*0.5f;
            p->aabb_half_extents[k]=(p->aabb_max[k]-p->aabb_min[k])*0.5f;
        }
    }
#   ifdef CHA_USE_VBO
    {
        const size_t verts_size_in_bytes = sizeof(float)*3*p->num_verts;
        CHA_ASSERT(p->verts && p->norms && p->num_verts);
        CHA_ASSERT(p->inds && p->num_inds);
        CHA_ASSERT(p->vbo==0 && p->ibo==0 && p->vao==0);

#       ifdef CHA_HINT_USE_VAO
        glGenVertexArrays(1,&p->vao);
        glBindVertexArray(p->vao);
#       endif
        glGenBuffers(1, &p->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, p->vbo);
        glBufferData(GL_ARRAY_BUFFER, 2*verts_size_in_bytes, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verts_size_in_bytes, p->verts);
        glBufferSubData(GL_ARRAY_BUFFER, verts_size_in_bytes, verts_size_in_bytes, p->norms);

#       ifndef CHA_HINT_USE_FFP_VBO
        glEnableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
        glVertexAttribPointer(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
        glVertexAttribPointer(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)(verts_size_in_bytes));
#       else
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, (void*)0);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, (void*)verts_size_in_bytes);
#       endif

        glGenBuffers(1, &p->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*p->num_inds, p->inds, GL_STATIC_DRAW);

#       ifdef CHA_HINT_USE_VAO
        glBindVertexArray(0);
#       endif

#       ifndef CHA_HINT_USE_FFP_VBO
        glDisableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
        glDisableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
#       else
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
#       endif

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

    }
#   endif
}
void cha_mesh_add_mesh_parts(struct cha_mesh* p,int num_parts,const int* material_idx_array,const int* num_inds_per_material_array)   {
    /* Warning: 'num_inds_per_material_array' must contain 'num_parts+1' elements; 'material_idx_array' must contain 'num_parts' elements */
    int i;CHA_ASSERT(p->num_parts==0 && !p->parts && p->verts && p->inds);
    p->num_parts = num_parts;p->parts = (struct cha_mesh_part*) cha_malloc(p->num_parts*sizeof(struct cha_mesh_part));
    for (i=0;i<p->num_parts;i++)    cha_mesh_part_init(&p->parts[i],material_idx_array[i],num_inds_per_material_array[i],num_inds_per_material_array[i+1]-num_inds_per_material_array[i]);
}
void cha_mesh_add_shape_keys(struct cha_mesh* p,int num_shape_keys,const char** shape_key_names,const float* shape_key_verts)   {
    /* Warning: 'shape_key_verts' must be the address of the first element of a float_array[num_shape_keys][3*mesh->num_verts] */
    int i;const float* p_shape_key_verts = shape_key_verts;
    CHA_ASSERT(num_shape_keys>0 && p->num_shape_keys==0 && !p->shape_keys && p->verts && p->inds && !p->shape_keys_type_static);
    p->shape_keys_type_static = (num_shape_keys==1 || (strcmp(shape_key_names[0],"Basis")!=0 && strcmp(shape_key_names[0],"basis")!=0)) ? 1 : 0;
    p->num_shape_keys = num_shape_keys;
    p->shape_keys = (struct cha_mesh_shape_key*) cha_malloc(p->num_shape_keys*sizeof(struct cha_mesh_shape_key));
    for (i=0;i<p->num_shape_keys;i++) {
        cha_mesh_shape_key_init(&p->shape_keys[i],shape_key_names[i],p_shape_key_verts,p->num_verts,p->inds,p->num_inds);
        p_shape_key_verts+=(size_t)(p->num_verts*3);
    }
    if (!p->shape_keys_type_static) {
        /* make shape keys relative */
        int j=0;const struct cha_mesh_shape_key* basis = &p->shape_keys[0];
        CHA_ASSERT(p->num_shape_keys>1 && p->shape_keys);
        CHA_ASSERT(strcmp(basis->name,"Basis")==0); /* all shape keys will be relative to the first shape key, named 'Basis' */
        for (i=1;i<p->num_shape_keys;i++)   {
            struct cha_mesh_shape_key* sk = &p->shape_keys[i];
            const float *bv = basis->verts, *bn = basis->norms;
            float *v = sk->verts, *n = sk->norms;
            for (j=0;j<3*sk->num_verts;j++)   {
                (*v++) -= *bv++;
                (*n++) -= *bn++;
            }
        }
    }
#   ifdef CHA_USE_VBO
    else    {
            /* build shape keys vbo and vao */
            const size_t verts_size_in_bytes = sizeof(float)*3*p->num_verts;
            CHA_ASSERT(p->shape_keys && p->num_shape_keys>0);
            for (i=0;i<p->num_shape_keys;i++)   {
                struct cha_mesh_shape_key* sk = &p->shape_keys[i];
                CHA_ASSERT(sk->vbo==0);

#               ifdef CHA_HINT_USE_VAO
                glGenVertexArrays(1,&sk->vao);
                glBindVertexArray(sk->vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->ibo);
#               endif

                glGenBuffers(1, &sk->vbo);
                glBindBuffer(GL_ARRAY_BUFFER, sk->vbo);
                glBufferData(GL_ARRAY_BUFFER, 2*verts_size_in_bytes, NULL, GL_STATIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, verts_size_in_bytes, sk->verts);
                glBufferSubData(GL_ARRAY_BUFFER, verts_size_in_bytes, verts_size_in_bytes, sk->norms);

#               ifndef CHA_HINT_USE_FFP_VBO
                glEnableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
                glVertexAttribPointer(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
                glEnableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
                glVertexAttribPointer(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)(verts_size_in_bytes));
#               else
                glEnableClientState(GL_VERTEX_ARRAY);
                glVertexPointer(3, GL_FLOAT, 0, (void*)0);
                glEnableClientState(GL_NORMAL_ARRAY);
                glNormalPointer(GL_FLOAT, 0, (void*)verts_size_in_bytes);
#               endif

#               ifdef CHA_HINT_USE_VAO
                glBindVertexArray(0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#               endif

#               ifndef CHA_HINT_USE_FFP_VBO
                glDisableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
                glDisableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
#               else
                glDisableClientState(GL_VERTEX_ARRAY);
                glDisableClientState(GL_NORMAL_ARRAY);
#               endif

                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
    }
#   endif
}
void cha_mesh_set_parent_bone(struct cha_mesh* p,int parent_mesh_idx,int parent_bone_idx,const float* parent_offset_matrix16,const float* parent_offset_matrix16_link) {
    struct cha_mesh* parent_mesh;
    const struct cha_armature* parent_armature;
    int parent_armature_idx;
    CHA_ASSERT(parent_mesh_idx>-1 && parent_mesh_idx<CHA_MESH_NAME_COUNT && parent_mesh_idx!=p->idx);
    parent_mesh = &gCharacterMeshes[parent_mesh_idx];
    parent_armature_idx = parent_mesh->armature_idx;
    CHA_ASSERT(parent_armature_idx>-1 && parent_armature_idx<CHA_ARMATURE_NAME_COUNT);
    parent_armature = &gCharacterArmatures[parent_armature_idx];
    CHA_ASSERT(parent_bone_idx>-1 && parent_bone_idx<parent_armature->num_bones);
    CHA_ASSERT(p->parent_bone_idx==-1 && p->parent_mesh_idx==-1);
    CHA_ASSERT(p->armature_idx==-1);    /* Not implemented yet */
    p->parent_mesh_idx = parent_mesh_idx;
    p->parent_bone_idx = parent_bone_idx;
    if (parent_offset_matrix16) memcpy(p->parent_offset_matrix,parent_offset_matrix16,16*sizeof(float));
    if (parent_offset_matrix16_link) memcpy(p->parent_offset_matrix_link,parent_offset_matrix16_link,16*sizeof(float));
    p->parent_offset_matrix_link_present = parent_offset_matrix16_link ? 1 : 0;
    /* update parent_mesh->child_mesh_idx*/
    {
        int i;for (i=0;i<parent_mesh->num_child_meshes;i++) {CHA_ASSERT(parent_mesh->child_mesh_idx[i]!=p->idx);}
        cha_safe_realloc((void** const)&parent_mesh->child_mesh_idx,(parent_mesh->num_child_meshes+1)*sizeof(int));
        parent_mesh->child_mesh_idx[parent_mesh->num_child_meshes] = p->idx;
        ++parent_mesh->num_child_meshes;
    }
}
void cha_mesh_set_parent_mesh(struct cha_mesh* p,int parent_mesh_idx,const float* parent_offset_matrix16,const float* parent_offset_matrix16_link) {
    struct cha_mesh* parent_mesh;    
    CHA_ASSERT(parent_mesh_idx>-1 && parent_mesh_idx<CHA_MESH_NAME_COUNT && parent_mesh_idx!=p->idx);
    parent_mesh = &gCharacterMeshes[parent_mesh_idx];
    CHA_ASSERT(p->parent_bone_idx==-1 && p->parent_mesh_idx==-1);
    CHA_ASSERT(p->armature_idx==-1);    /* Not implemented yet */
    p->parent_mesh_idx = parent_mesh_idx;
    if (parent_offset_matrix16) memcpy(p->parent_offset_matrix,parent_offset_matrix16,16*sizeof(float));
    if (parent_offset_matrix16_link) memcpy(p->parent_offset_matrix_link,parent_offset_matrix16_link,16*sizeof(float));
    p->parent_offset_matrix_link_present = parent_offset_matrix16_link ? 1 : 0;
    /* update parent_mesh->child_mesh_idx*/
    {
        int i;for (i=0;i<parent_mesh->num_child_meshes;i++) {CHA_ASSERT(parent_mesh->child_mesh_idx[i]!=p->idx);}
        cha_safe_realloc((void** const)&parent_mesh->child_mesh_idx,(parent_mesh->num_child_meshes+1)*sizeof(int));
        parent_mesh->child_mesh_idx[parent_mesh->num_child_meshes] = p->idx;
        ++parent_mesh->num_child_meshes;
    }
}
void cha_mesh_swap_parent_offset_matrices(struct cha_mesh* p)  {
    float tmp[16];memcpy(tmp,p->parent_offset_matrix,16*sizeof(float));
    memcpy(p->parent_offset_matrix,p->parent_offset_matrix_link,16*sizeof(float));
    memcpy(p->parent_offset_matrix_link,tmp,16*sizeof(float));
}
void cha_mesh_add_armature_weights(struct cha_mesh* p,int armature_idx,const struct cha_mesh_vertex_weight* weights,int num_weights)
{
    int i,j;
    const float aabb_scaling[3]={1.2f,6.f,1.f};  /* y is front, z is up here */
    const float aabb_max_z_scaling = 1.75f;     /* well, to cover a character with arms up we need this */
    CHA_ASSERT(!p->weights && p->num_weights==0);
    CHA_ASSERT(num_weights==p->num_verts*3);    /* we use 3 weights per vertex */    
    p->num_weights = num_weights;
    p->weights = (struct cha_mesh_vertex_weight*) cha_malloc(p->num_weights*sizeof(struct cha_mesh_vertex_weight));
    memcpy(p->weights,weights,p->num_weights*sizeof(struct cha_mesh_vertex_weight));
    CHA_ASSERT(p->armature_idx==-1 && p->parent_bone_idx==-1 && p->parent_mesh_idx==-1);
    /*CHA_ASSERT(armature_idx>=0);*/
    p->armature_idx=armature_idx;
    CHA_ASSERT(!p->weight_bone_masks);
    p->weight_bone_masks = (unsigned*) cha_malloc(p->num_verts*sizeof(unsigned));
    memset(p->weight_bone_masks,0,p->num_verts*sizeof(unsigned));
    for (i=0;i<p->num_verts;i++)    {
        unsigned* mask = &p->weight_bone_masks[i];
        const struct cha_mesh_vertex_weight* weights = &p->weights[3*i];
        *mask=0;
        for (j=0;j<3;j++)   {
            const int bi = weights[j].bone_idx;
            CHA_ASSERT(bi<32);  /* mask is 32-bit */
            if (bi>=0) (*mask)|=(1<<((unsigned)bi));}
    }
    /* enlarge aabb a bit to take armature poses into account */
    p->aabb_max[2]*=aabb_max_z_scaling;
    for (i=0;i<3;i++) {
        p->aabb_min[i]*=aabb_scaling[i];p->aabb_max[i]*=aabb_scaling[i];
        p->aabb_center[i]=(p->aabb_max[i]+p->aabb_min[i])*0.5f;
        p->aabb_half_extents[i]=(p->aabb_max[i]-p->aabb_min[i])*0.5f;
    }
}

struct cha_material {
    float amb[4],dif[4],spe[3],shi; /* 12 floats in total */
#   ifdef CHA_MATERIAL_USER_CODE
    CHA_MATERIAL_USER_CODE
#   endif
};
void cha_material_names_init(struct cha_material_names* p,const char* material_names[CHA_MATERIAL_NAME_COUNT])  {
    int i;size_t len;memset(p,0,sizeof(*p));
    for (i=0;i<CHA_MATERIAL_NAME_COUNT;i++) {
        char* dst = &p->names[i][0];
        const char* src = material_names[i];
        len = strlen(src);
        if (len<127) strcpy(dst,src);
        else {strncpy(dst,src,127);dst[127]='\0';}
    }
}
void cha_material_init(struct cha_material* p,const float amb[4],const float dif[4],const float spe[3],float shi) {
    memset(p,0,sizeof(*p));
    memcpy(p->amb,amb,4*sizeof(float));memcpy(p->dif,dif,4*sizeof(float));
    memcpy(p->spe,spe,3*sizeof(float));p->shi = shi;
}
void cha_material_display(const struct cha_material* p) {
    CHA_ASSERT(p);
    printf("amb[4] = {%1.4f,%1.4f,%1.4f,%1.4f};\n",p->amb[0],p->amb[1],p->amb[2],p->amb[3]);
    printf("dif[4] = {%1.4f,%1.4f,%1.4f,%1.4f};\n",p->dif[0],p->dif[1],p->dif[2],p->dif[3]);
    printf("spe[3] = {%1.4f,%1.4f,%1.4f}, shi = %1.4f;\n",p->spe[0],p->spe[1],p->spe[2],p->shi);
}

#include "character_inl.h"

struct cha_mesh_instance_pose_data {
    float tra[3],rot[4];
    int tra_dirty,rot_dirty;    /* 1 ==> modified (pose_matrix must be updated from pose_data (==this)) 2 ==> outdated (pose_data (==this) must be updated by pose_matrix[in bone space]) */
#   ifdef CHA_MESH_INSTANCE_POSE_DATA_USER_CODE
    CHA_MESH_INSTANCE_POSE_DATA_USER_CODE
#   endif
};
struct cha_character_instance;  /* forward declaration */
struct cha_mesh_instance {
    const struct cha_mesh* mesh; /* non-owned reference */
    const struct cha_armature* armature;   /* non-owned reference (set only if this armature DEFORMS this mesh) */

    float mvMatrix[16],mvMatrix_link[16];   /* if these are full mMatrices (model matrices), they could be in double precision */

    /* pose stuff (if 'armature'!=NULL) */
    float* pose_matrices[4];   /* size = armature->num_bones*16. */
    struct cha_mesh_instance_pose_data* pose_data;       /* size = armature->num_bones */
    unsigned pose_bone_mask;    /* bone mask of the currently affected bones */

    /* skeletal animation stuff (if 'armature'!=NULL) */
    float *verts,*norms;            /* size=mesh->num_verts; buffer for the animated verts/norms */

    /* shape keys animation stuff (if mesh->shape_keys!=NULL) */
    int shk_values_dirty;                  /* when manually modifying any 'shk_values', we must turn on this flag */
    float* shk_values;              /* size=mesh->num_shape_keys, each is in [0.f,1.f] */
    float *verts_shk,*norms_shk;    /* size=mesh->num_verts; buffer for the animated verts/norms */
    int static_shk_index;           /* -1 if static shape keys are not used */

    int culled;
    struct cha_character_instance* parent_character_instance;

#   ifdef CHA_USE_VBO
    GLuint vbo,vao; /* 'vbo' with 'verts' 'norms'; ibo with 'inds', but used only for NON-animated shape_keys */
#   endif

#   ifdef CHA_MESH_INSTANCE_USER_CODE
    CHA_MESH_INSTANCE_USER_CODE
#   endif
};
void cha_mesh_instance_reset_pose(struct cha_mesh_instance* p)   {
    int i,j;
    CHA_ASSERT(p->armature && p->pose_matrices[0] && p->pose_matrices[1] && p->pose_matrices[2] && p->pose_matrices[3] && p->pose_data);
    memset(p->pose_data,0,p->armature->num_bones*sizeof(struct cha_mesh_instance_pose_data));
    for (i=0;i<4;i++)   memset(p->pose_matrices[i],0,p->armature->num_bones*16*sizeof(float));
    /* reset all values */
    for (j=0;j<p->armature->num_bones;j++)  {
        const struct cha_armature_bone* b = &p->armature->bones[j];
        struct cha_mesh_instance_pose_data* d = &p->pose_data[j];
        d->rot[3]=1.f;
        for (i=0;i<4;i++)   {
            float* m16 = &p->pose_matrices[i][16*j];
            if (i==CHA_BONE_SPACE_ARMATURE) memcpy(m16,b->data[CHA_BONE_SPACE_ARMATURE].matrix,16*sizeof(float));
            else if (i==CHA_BONE_SPACE_GRABBING) {
                chm_Mat4MulUncheckArgsf(m16,&p->pose_matrices[CHA_BONE_SPACE_ARMATURE][16*j],b->post_pose_armature_space_matrix);
                if (j==CHA_BONE_NAME_ROOT) {
                    CHA_ASSERT(p->mesh);
                    //chm_Mat4Rotate(m16,180.f,1.f,0.f,0.f);
                    //chm_Mat4Translate(m16,-p->mesh->aabb_center[0],-p->mesh->aabb_center[1]-b->length,-p->mesh->aabb_center[2]);
                    //chm_Mat4Translate(m16,0.f,b->length,0.f);
                }
            }
            else {m16[0]=m16[5]=m16[10]=m16[15]=1.f;}
        }
    }
}
void cha_mesh_instance_reset_shape_key_pose(struct cha_mesh_instance* p)    {
    const struct cha_mesh* mesh = p->mesh;
    CHA_ASSERT(p->shk_values && mesh && mesh->num_shape_keys && !mesh->shape_keys_type_static);
    memset(p->shk_values,0,mesh->num_shape_keys*sizeof(float));
    memcpy(p->verts_shk,mesh->verts,mesh->num_verts*3*sizeof(float));
    memcpy(p->norms_shk,mesh->norms,mesh->num_verts*3*sizeof(float));
}
void cha_mesh_instance_init(struct cha_mesh_instance* p,const struct cha_mesh* mesh,int optional_static_shape_key_idx)    {
    int i;
    const float id16[16] = {1.f,0.f,0.f,0.f,    0.f,1.f,0.f,0.f,    0.f,0.f,1.f,0.f,    0.f,0.f,0.f,1.f};
    CHA_ASSERT(mesh);
    memset(p,0,sizeof(*p));
    p->mesh = mesh;
    p->static_shk_index = mesh->shape_keys_type_static ? optional_static_shape_key_idx : -1;
    CHA_ASSERT(p->static_shk_index<mesh->num_shape_keys);
    memcpy(p->mvMatrix,id16,16*sizeof(float));
    if (mesh->armature_idx>-1)  {
        /* has skeletal animation */
        const struct cha_mesh_shape_key* sk = (mesh->shape_keys_type_static && p->static_shk_index>-1) ? &mesh->shape_keys[p->static_shk_index] : NULL;
        const float* pv = sk ? sk->verts : mesh->verts;
        const float* pn = sk ? sk->norms : mesh->norms;
        CHA_ASSERT(mesh->parent_mesh_idx==-1);
        CHA_ASSERT(mesh->parent_bone_idx==-1);
        CHA_ASSERT(mesh->armature_idx<CHA_ARMATURE_NAME_COUNT);
        p->armature = &gCharacterArmatures[mesh->armature_idx];
        /* we must allocate and reset the bone_matrices */
        for (i=0;i<4;i++)   {p->pose_matrices[i] = (float*) cha_malloc(p->armature->num_bones*16*sizeof(float));}
        p->pose_data = (struct cha_mesh_instance_pose_data*) cha_malloc(p->armature->num_bones*sizeof(struct cha_mesh_instance_pose_data));
        cha_mesh_instance_reset_pose(p);
        /* we must add space for the deformed verts/norms */
        p->verts = (float*) cha_malloc(p->mesh->num_verts*3*sizeof(float));
        memcpy(p->verts,pv,p->mesh->num_verts*3*sizeof(float));
        p->norms = (float*) cha_malloc(p->mesh->num_verts*3*sizeof(float));
        memcpy(p->norms,pn,p->mesh->num_verts*3*sizeof(float));
    }
    if (mesh->num_shape_keys>0 && (!mesh->shape_keys_type_static || mesh->armature_idx>-1)) {
        /* we must add space for the deformed shape-keys verts/norms */
        p->verts_shk = (float*) cha_malloc(p->mesh->num_verts*3*sizeof(float));
        p->norms_shk = (float*) cha_malloc(p->mesh->num_verts*3*sizeof(float));
        if (!mesh->shape_keys_type_static || p->static_shk_index<0)  {
            memcpy(p->verts_shk,mesh->verts,p->mesh->num_verts*3*sizeof(float));
            memcpy(p->norms_shk,mesh->norms,p->mesh->num_verts*3*sizeof(float));
            if (!mesh->shape_keys_type_static)  {
                p->shk_values = (float*) cha_malloc(p->mesh->num_shape_keys*sizeof(float));
                memset(p->shk_values,0,p->mesh->num_shape_keys*sizeof(float));
            }
        }
        else {
            const struct cha_mesh_shape_key* sk;
            CHA_ASSERT(p->static_shk_index>=0 && p->static_shk_index<mesh->num_shape_keys);
            sk = &mesh->shape_keys[p->static_shk_index];
            memcpy(p->verts_shk,sk->verts,p->mesh->num_verts*3*sizeof(float));
            memcpy(p->norms_shk,sk->norms,p->mesh->num_verts*3*sizeof(float));
        }
    }
#   ifdef CHA_USE_VBO
    if (p->armature || (p->verts_shk && !mesh->shape_keys_type_static)){
        const int num_verts = p->mesh->num_verts;
        const size_t verts_size_in_bytes = sizeof(float)*3*num_verts;
        const float* cverts = p->verts ? p->verts : p->verts_shk;
        const float* cnorms = p->norms ? p->norms : p->norms_shk;

        CHA_ASSERT(cverts && cnorms && num_verts);
        CHA_ASSERT(p->vbo==0 && p->vao==0);

#       ifdef CHA_HINT_USE_VAO
        glGenVertexArrays(1,&p->vao);
        glBindVertexArray(p->vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,p->mesh->ibo);
#       endif

        glGenBuffers(1, &p->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, p->vbo);
        glBufferData(GL_ARRAY_BUFFER, 2*verts_size_in_bytes, NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verts_size_in_bytes, cverts);
        glBufferSubData(GL_ARRAY_BUFFER, verts_size_in_bytes, verts_size_in_bytes, cnorms);

#       ifndef CHA_HINT_USE_FFP_VBO
        glEnableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
        glVertexAttribPointer(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
        glVertexAttribPointer(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)(verts_size_in_bytes));
#       else
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, (void*)0);
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, (void*)verts_size_in_bytes);
#       endif

#       ifdef CHA_HINT_USE_VAO
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
#       endif

#       ifndef CHA_HINT_USE_FFP_VBO
        glDisableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
        glDisableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
#       else
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
#       endif

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#   endif

}
void cha_mesh_instance_destroy(struct cha_mesh_instance* p)    {
    int i;
    for (i=0;i<4;i++)   {if (p->pose_matrices[i]) {cha_free(p->pose_matrices[i]);p->pose_matrices[i]=NULL;}}
    if (p->pose_data)   {cha_free(p->pose_data);p->pose_data=NULL;}
    if (p->verts) {cha_free(p->verts);p->verts=NULL;}
    if (p->norms) {cha_free(p->norms);p->norms=NULL;}
    if (p->verts_shk) {cha_free(p->verts_shk);p->verts_shk=NULL;}
    if (p->norms_shk) {cha_free(p->norms_shk);p->norms_shk=NULL;}
    if (p->shk_values)  {cha_free(p->shk_values);p->shk_values=NULL;}
    p->armature = NULL;p->mesh = NULL;
#   ifdef CHA_USE_VBO
    if (p->vbo) {glDeleteBuffers(1,&p->vbo);p->vbo=0;}
#   ifdef CHA_HINT_USE_VAO
    if (p->vao) {glDeleteVertexArrays(1,&p->vao);p->vao=0;}
#   endif
#   endif
}
void cha_mesh_instance_draw_armature(const struct cha_mesh_instance* mi,void (*drawArmatureBoneCallback)(const struct cha_mesh_instance* mi,int bone_idx,const float* mvMatrix16,float length,void* userData),void* userData)  {
    const struct cha_armature* p = mi->armature;
    int i;
//#   if 1
    const int allowed_root_opt =
#   ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
    1;
#   else
    0;
#   endif
    float mv[16];
    if (!p || !drawArmatureBoneCallback) return;
    CHA_ASSERT(p && drawArmatureBoneCallback);

    for (i=0;i<p->num_bones;i++)    {
        const struct cha_armature_bone* b = &p->bones[i];
        const float* gMatrix = &mi->pose_matrices[CHA_BONE_SPACE_ARMATURE][b->idx*16];

        if (mi->pose_matrices
                && (!allowed_root_opt || (!(mi->pose_bone_mask==CHA_BONE_MASK_ROOT && i==0)))
                ) {
#           ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
            chm_Mat4MulUncheckArgsf(mv,mi->mvMatrix,gMatrix);
#           else
            chm_Mat4MulUncheckArgsf(mv,mi->mvMatrix,gMatrix);
#           endif
        }
        else {
#           ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
            chm_Mat4Copyf(mv,mi->mvMatrix);    /* Why it's rotated ?*/
            chm_Mat4Rotatef(mv,90.f,1,0,0);
#           else
            chm_Mat4MulUncheckArgsf(mv,mi->mvMatrix,gMatrix);
#           endif
        }
        drawArmatureBoneCallback(mi,i,mv,b->length,userData);
    }
/*#else
    if (mi->pose_matrices) {
        float m[16];
        int start_bone_idx = 0;
#       ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
        if (mi->pose_bone_mask==CHA_BONE_MASK_ROOT) start_bone_idx = 1; // root bone position would be wrong, so we skip it
#       endif
        if (!p || !drawArmatureBoneCallback) return;
        CHA_ASSERT(p && drawArmatureBoneCallback);

        for (i=start_bone_idx;i<p->num_bones;i++)    {
            const struct cha_armature_bone* b = &p->bones[i];
            chm_Mat4Mul(m,mi->mvMatrix,&mi->pose_matrices[CHA_BONE_SPACE_ARMATURE][b->idx*16]);
            drawArmatureBoneCallback(mi,i,m,b->length,userData);
        }
    }
#endif*/
}

void cha_slerp_callback(float* rot_out4,const float* rot_start4,const float* rot_end4,float factor) {
    const int normalize_quat_lerp = 0;
    const float slerp_eps = 0.001f;
    CHA_ASSERT(factor>=0.f && factor<=1.0f);
    chm_QuatSlerpEps(rot_out4,rot_start4,rot_end4,factor,normalize_quat_lerp,slerp_eps);
}
void cha_lerp_callback(float* tra_out3,const float* tra_start3,const float* tra_end3,float factor) {
    int j;
    CHA_ASSERT(factor>=0.f && factor<=1.0);
    for (j=0;j<3;j++) tra_out3[j] = tra_start3[j] + (tra_end3[j] - tra_start3[j]) * factor;
}
int cha_mesh_instance_interpolate_key_frames_from_action_step(const struct cha_armature_action* action,
                                                               float animation_time /* relative */,float additional_time_to_get_to_first_frame,int is_first_loop,
                                                               const struct cha_armature_action_key_frame* keys,const int sz,
                                                               const float* vin,float* vout,int vcomponents,
                                                               void (*lerp_callback)(float*,const float*,const float*,float)
                                                               )
{
    /* returns 1 if the animation pose is already in place (we can skip converting it to matrix) */
    const int looping=action->looping;/*action->flags&CHA_ARMATURE_ACTION_FLAG_LOOPING;*/
    const float *start,*end;float delta_time,factor;
    int pos_index=sz-1,next_pos_index,j;

    CHA_ASSERT(action && keys && sz>0 && vin && vout && ((vcomponents>=3 && lerp_callback==&cha_lerp_callback) || (vcomponents<=4 && lerp_callback==&cha_slerp_callback)));

    if (animation_time<keys[0].time && (is_first_loop || !looping))    {
        start = vin;end = &keys[0].x;
        if (animation_time<0.f) factor = (animation_time+additional_time_to_get_to_first_frame)/(additional_time_to_get_to_first_frame+keys[0].time);
        else {
            if (additional_time_to_get_to_first_frame && (is_first_loop /*|| !looping*/)) factor = 1.f;   // hack! (required to fix the mixed walk/run animation)
            else factor = animation_time/keys[0].time;
        }
        lerp_callback(vout,start,end,factor);
        //fprintf(stderr,"animation_time=%1.5f factor=%1.5f\n",animation_time,factor);
        return 0;
    }
    CHA_ASSERT(animation_time>=0.f);
    if (!looping && animation_time>=keys[sz-1].time)    {
        const int value_is_already_there = memcmp(vout,&keys[sz-1].x,vcomponents*sizeof(float))==0;
        memcpy(vout,&keys[sz-1].x,vcomponents*sizeof(float));
        return value_is_already_there;
    }
    /*if (sz==1 || (is_first_loop && animation_time<keys[0].time))    {
        if (!is_first_loop || animation_time>=keys[0].time) {
            const int value_is_already_there = memcmp(vout,&keys[0].x,vcomponents*sizeof(float))==0;
            memcpy(vout,&keys[0].x,vcomponents*sizeof(float));
            return value_is_already_there;
        }
        else {
            start = vin;end = &keys[0].x;
            factor = animation_time/keys[0].time;
            lerp_callback(vout,start,end,factor);
        }
        return 0;
    }*/


    if (sz<3)   {
        /* linear search */
        for (j = 0; j < sz; j++) {if (animation_time < keys[j].time) {pos_index = j-1;break;}}
        // pos_index in [-1,sz-1]   // The latter is because it's inited to sz-1
    }
    else    {
        /* binary search */
        pos_index = cha_armature_action_key_frames_binary_search(&keys[0],sz,animation_time)-1;
    }
    CHA_ASSERT(pos_index>=-1 && pos_index<=sz-1);

    /* perform slerp here */
    if (pos_index==-1)	{
        CHA_ASSERT(!is_first_loop);
        /* we should 'continue' the previous slerp here */
        CHA_ASSERT(looping);
        CHA_ASSERT(animation_time<=keys[0].time && animation_time + action->max_frame_time>=keys[sz-1].time);
        start = &keys[sz-1].x;  end=&keys[0].x;
        delta_time = keys[0].time + action->max_frame_time - keys[sz-1].time;
        factor = (animation_time + action->max_frame_time - keys[sz-1].time) / delta_time;
        lerp_callback(vout,start,end,factor);
        return 0;
    }

    next_pos_index = (pos_index + 1);
    /*if (next_pos_index==sz && !looping) {
        memcpy(vout,&keys[sz-1].x,vcomponents*sizeof(float));
        return;
    }
    else*/ if (next_pos_index>=sz) {
        CHA_ASSERT(looping);
        //CHA_ASSERT(animation_time<=keys[0].time);    // NOPE!
        next_pos_index=0;
        delta_time = keys[0].time + action->max_frame_time - keys[sz-1].time;
        factor = (animation_time - keys[sz-1].time) / delta_time;
        /* this lerp will 'continue' later in the brach: if (pos_index==-1) */
    }
    else    {
        delta_time = keys[next_pos_index].time - keys[pos_index].time;
        factor = (animation_time - keys[pos_index].time) / delta_time;
    }
    start = &keys[pos_index].x;
    end = &keys[next_pos_index].x;
    CHA_ASSERT(next_pos_index < sz);
    lerp_callback(vout,start,end,factor);
    return 0;
}

// TODO: Add a time_to_first_frame, and add some kind of higher level system which stores animation start time
float cha_mesh_instance_calculate_bone_space_pose_matrices_from_action_ex(struct cha_mesh_instance* p,int action_index,float global_animation_time,float additional_time_to_get_to_first_frame,
                                                                          float action_weight_in_mix_mode_in_0_1 /*=1.f*/,int mix_action_idx/*=-1*/,
                                                                          int bone_start_idx/*=0*/,int num_bones_to_process/*=-1*/,unsigned bone_exclude_mask/*=0*/)
{
    const struct cha_armature* armature = p->armature;
    const struct cha_armature_action* action;
    struct cha_mesh_instance_pose_data* pose_data_list = p->pose_data;   /* size = armature->num_bones */
    float* pose_matrix_list = p->pose_matrices[CHA_BONE_SPACE_BONE];    /*  size = armature->num_bones*16. */
    float animation_time=0.f;
    int i,j,looping=0,is_first_loop = 0;
    int must_mix_with_another_action = 0;
    CHA_ASSERT(armature && action_index>=0 && armature->num_actions>action_index);
    if (bone_start_idx<0) bone_start_idx=0;
    if (num_bones_to_process<0) num_bones_to_process=armature->num_bones-bone_start_idx;
    if (bone_exclude_mask) {CHA_ASSERT(bone_start_idx+num_bones_to_process<=32);}    /* 'bone_idx_exclude_mask' (32 bit) can't be used with more than 32 bones */
    CHA_ASSERT(pose_data_list && pose_matrix_list);
    action = &armature->actions[action_index];
    CHA_ASSERT(action);
    looping = action->looping;/*action->flags&CHA_ARMATURE_ACTION_FLAG_LOOPING;*/
    CHA_ASSERT(action_weight_in_mix_mode_in_0_1>=0.f && action_weight_in_mix_mode_in_0_1<=1.f);
    CHA_ASSERT(additional_time_to_get_to_first_frame>=0.f);

    if (mix_action_idx>=0) {
        CHA_ASSERT(mix_action_idx<armature->num_actions);
        /* ok, but we set 'must_mix_with_another_action' only
        if 'merge_factor_in_0_1' is in (0.01f,0.99f) so we can skip a lot of code */
        if (action_weight_in_mix_mode_in_0_1<=0.01f) {
            action_index = mix_action_idx;mix_action_idx = -1;
            action = &armature->actions[action_index];   /* switch to the other action and just play it fully */
            looping = action->looping;/*action->flags&CHA_ARMATURE_ACTION_FLAG_LOOPING;*/
        }
        else if (action_weight_in_mix_mode_in_0_1<0.99f) must_mix_with_another_action = 1;
    }

    if (!must_mix_with_another_action) {
        int can_skip_dirty_flag = 0;
        const float additional_time = additional_time_to_get_to_first_frame*action->ticks_per_second;
        animation_time = global_animation_time*action->ticks_per_second - additional_time;
        if (looping)    {
            is_first_loop = (animation_time<action->max_frame_time)?1:0;
            if (animation_time<0.f) {CHA_ASSERT(is_first_loop);}
            else animation_time = fmodf(animation_time,action->max_frame_time);
        }
        //else fprintf(stderr,"animation_time=%1.5f additional_time=%1.5f\n",animation_time,additional_time);

        for (i=bone_start_idx;i<num_bones_to_process;i++) {
            const struct cha_armature_bone* b = &armature->bones[i];
            const struct cha_armature_action_bone_key_frame_stream* stream = b->key_frame_streams[action_index];
            struct cha_mesh_instance_pose_data* pose_data = &pose_data_list[i];
            float* pose_matrix = &pose_matrix_list[16*i];
            /* we must fill 'pose_data' from key frames inside 'stream' and then convert 'pose_data' to 'pose_matrix' */
            if (!(bone_exclude_mask&(1<<i)) && stream) {
                if (stream->num_rotation_key_frames) {
                    float* rot_out = pose_data->rot;    /* used also as input in some cases */
                    const int sz = stream->num_rotation_key_frames;
                    const struct cha_armature_action_key_frame* keys = stream->rotation_key_frames;
                    if (pose_data->rot_dirty>=2) {chm_QuatFromMat4(pose_data->rot,pose_matrix);pose_data->rot_dirty=0;}
                    can_skip_dirty_flag = cha_mesh_instance_interpolate_key_frames_from_action_step(action,animation_time,additional_time,is_first_loop,keys,sz,
                                                                              rot_out,rot_out,
                                                                              4,&cha_slerp_callback);
                    pose_data->rot_dirty=can_skip_dirty_flag?0:1;
                    /* Here 'pose_data' rot should be OK */
                    //chm_Mat4SetRotationFromQuat(pose_matrix,rot_out);
                }
                if (stream->num_translation_key_frames) {
                    float* tra_out = pose_data->tra;    /* used also as input in some cases */
                    const int sz = stream->num_translation_key_frames;
                    const struct cha_armature_action_key_frame* keys = stream->translation_key_frames;
                    if (pose_data->tra_dirty>=2) {for (j=0;j<3;j++) pose_data->tra[j]=pose_matrix[12+j];pose_data->tra_dirty=0;}
                    can_skip_dirty_flag = cha_mesh_instance_interpolate_key_frames_from_action_step(action,animation_time,additional_time,is_first_loop,keys,sz,
                                                                              tra_out,tra_out,
                                                                              3,&cha_lerp_callback);
                    pose_data->tra_dirty=can_skip_dirty_flag?0:1;   /* tra is not as expensive as rot */
                    /* Here 'pose_data' tra should be OK */
                    //for (j=0;j<3;j++) pose_matrix[12+j] = tra_out[j];
                }
            }
        }
    }

    else {
        /* trying to merge the two actions based on 'merge_factor_in_0_1' */
        const int idx[2] = {mix_action_idx,action_index};
        const struct cha_armature_action* actions[2] = {&armature->actions[idx[0]],&armature->actions[idx[1]]};
        const int loopings[2] = {actions[0]->looping,actions[1]->looping};
        const float additional_times[2] = {additional_time_to_get_to_first_frame*actions[0]->ticks_per_second,additional_time_to_get_to_first_frame*actions[1]->ticks_per_second};
        float animation_times[2] = {global_animation_time*actions[0]->ticks_per_second - additional_times[0],global_animation_time*actions[1]->ticks_per_second - additional_times[1]};
        const int is_first_loops[2] = {loopings[0]?((animation_times[0]<actions[0]->max_frame_time)?1:0):0,  loopings[1]?((animation_times[1]<actions[1]->max_frame_time)?1:0):0};
        if (loopings[0] && animation_times[0]>=0.f) animation_times[0] = fmodf(animation_times[0],actions[0]->max_frame_time);
        if (loopings[1] && animation_times[1]>=0.f) animation_times[1] = fmodf(animation_times[1],actions[1]->max_frame_time);

        for (i=bone_start_idx;i<num_bones_to_process;i++) {
            const struct cha_armature_bone* b = &armature->bones[i];
            const struct cha_armature_action_bone_key_frame_stream* streams[2] = {b->key_frame_streams[idx[0]],b->key_frame_streams[idx[1]]};
            struct cha_mesh_instance_pose_data* pose_data = &pose_data_list[i];
            float* pose_matrix = &pose_matrix_list[16*i];
            float rot[2][4]={{0,0,0,1},{0,0,0,1}};
            float tra[2][3]={{0,0,0},{0,0,0}};
            float* rot_in = pose_data->rot;
            float* tra_in = pose_data->tra;
            int ok;

            if (!(bone_exclude_mask&(1<<i))) {
                ok = 0;
                for (j=0;j<2;j++)   {
                    const struct cha_armature_action_bone_key_frame_stream* stream = streams[j];
                    /* we must fill 'pose_data' from key frames inside 'stream' and then convert 'pose_data' to 'pose_matrix' */
                    if (stream && stream->num_rotation_key_frames) {
                        const int sz = stream->num_rotation_key_frames;
                        const struct cha_armature_action_key_frame* keys = stream->rotation_key_frames;
                        if (pose_data->rot_dirty>=2) {chm_QuatFromMat4(pose_data->rot,pose_matrix);pose_data->rot_dirty=0;}
                        cha_mesh_instance_interpolate_key_frames_from_action_step(actions[j],animation_times[j],additional_times[j],is_first_loops[j],keys,sz,
                                                                                  rot_in,&rot[j][0],
                                4,&cha_slerp_callback);
                        ++ok;

                    }
                    if (ok) {
                        /* we have 2 'rot' to merge into 'rot_in' */
                        /*cha_slerp_callback(rot_in,&rot[0][0],&rot[1][0],action_weight_in_mix_mode_in_0_1);
                        pose_data->rot_dirty=1;*/
                        float rot_tmp[4];int can_skip_value;
                        cha_slerp_callback(rot_tmp,&rot[0][0],&rot[1][0],action_weight_in_mix_mode_in_0_1);
                        can_skip_value = (memcmp(rot_in,rot_tmp,4*sizeof(float))==0)?1:0;
                        memcpy(rot_in,rot_tmp,4*sizeof(float));
                        pose_data->rot_dirty=can_skip_value?0:1;
                        /* Here 'pose_data' rot should be OK */
                        //chm_Mat4SetRotationFromQuat(pose_matrix,rot_in);
                    }
                }

                ok = 0;
                for (j=0;j<2;j++)   {
                    const struct cha_armature_action_bone_key_frame_stream* stream = streams[j];
                    if (stream && stream->num_translation_key_frames) {
                        const int sz = stream->num_translation_key_frames;
                        const struct cha_armature_action_key_frame* keys = stream->translation_key_frames;
                        if (pose_data->tra_dirty>=2) {for (j=0;j<3;j++) pose_data->tra[j]=pose_matrix[12+j];pose_data->tra_dirty=0;}
                        cha_mesh_instance_interpolate_key_frames_from_action_step(actions[j],animation_times[j],additional_times[j],is_first_loops[j],keys,sz,
                                                                                  tra_in,&tra[j][0],
                                3,&cha_lerp_callback);
                        ++ok;
                    }
                }
                if (ok) {
                    /* we have 2 'tra' to merge into 'tra_in' */
                    /*cha_lerp_callback(tra_in,&tra[0][0],&tra[1][0],action_weight_in_mix_mode_in_0_1);
                    pose_data->tra_dirty=1;*/
                    float tra_tmp[3];int can_skip_value;
                    cha_lerp_callback(tra_tmp,&tra[0][0],&tra[1][0],action_weight_in_mix_mode_in_0_1);
                    can_skip_value = (memcmp(tra_in,tra_tmp,3*sizeof(float))==0)?1:0;
                    memcpy(tra_in,tra_tmp,3*sizeof(float));
                    pose_data->tra_dirty=can_skip_value?0:1;
                    /* Here 'pose_data' tra should be OK */
                    //for (j=0;j<3;j++) pose_matrix[12+j] = tra_in[j];
                }

            }
        }
    }

    return animation_time;  /* returns fmod(global_animation_time,action->max_frame_time) for looping animations */
}
CHA_API_INL float cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(struct cha_mesh_instance* p,int action_index,float global_animation_time,float additional_time_to_get_to_first_frame)  {
    return cha_mesh_instance_calculate_bone_space_pose_matrices_from_action_ex(p,action_index,global_animation_time,additional_time_to_get_to_first_frame,1.f,-1,0,-1,0);
}
void cha_mesh_instance_update_bone_matrix(struct cha_mesh_instance* p,int bone_idx,int recursive) {
    //p->pose_bone_mask = 0;    /* well, this must be reset BEFORE calling this method (multiple times) */
    if (p->armature)    {
        /* calculates all:
           -> p->pose_matrices[CHA_BONE_SPACE_BONE] from p->pose_data (only if necessary)
           -> p->pose_matrices[CHA_BONE_SPACE_ARMATURE] from p->pose_matrices[CHA_BONE_SPACE_BONE]
           -> p->pose_matrices[CHA_BONE_SPACE_SKINNING] from p->pose_matrices[CHA_BONE_SPACE_ARMATURE]
           -> p->pose_matrices[CHA_BONE_SPACE_GRABBING] from p->pose_matrices[CHA_BONE_SPACE_ARMATURE]

           To get the CHA_BONE_SPACE_ARMATURE:
           CHA_BONE_SPACE_ARMATURE = CHA_BONE_SPACE_SKINNING * b->data[CHA_BONE_SPACE_ARMATURE].matrix;
           so that: CHA_BONE_SPACE_SKINNING = CHA_BONE_SPACE_ARMATURE * b->data[CHA_BONE_SPACE_ARMATURE].matrix_inv;
        */
        int i;
        const struct cha_armature* armature = p->armature;
        const struct cha_armature_bone* b = &armature->bones[bone_idx];
        struct cha_mesh_instance_pose_data* pose_data = &p->pose_data[bone_idx];
        float* pose_matrix_bone_space = &p->pose_matrices[CHA_BONE_SPACE_BONE][16*bone_idx];
        float* pose_matrix_armature_space = &p->pose_matrices[CHA_BONE_SPACE_ARMATURE][16*bone_idx];
        float* pose_matrix_skinning_space = &p->pose_matrices[CHA_BONE_SPACE_SKINNING][16*bone_idx];
        float* pose_matrix_grabbing_space = &p->pose_matrices[CHA_BONE_SPACE_GRABBING][16*bone_idx];
        const int can_skip_bone = (pose_data->rot_dirty==0 || pose_data->rot_dirty==3) &&
                                    (pose_data->tra_dirty==0 || pose_data->tra_dirty==3);
        CHA_ASSERT(bone_idx<armature->num_bones);
#       ifndef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
        if (!can_skip_bone || (b->parent_idx>=CHA_BONE_NAME_ROOT && (p->pose_bone_mask&(1<<((unsigned)b->parent_idx)))))
#       else
        if (!can_skip_bone || (b->parent_idx>CHA_BONE_NAME_ROOT && (p->pose_bone_mask&(1<<((unsigned)b->parent_idx)))))
#       endif
        {
            p->pose_bone_mask|=(1<<b->idx);
            if (pose_data->rot_dirty>0 && pose_data->rot_dirty<3)    {
                CHA_ASSERT(pose_data->rot_dirty==1 || pose_data->rot_dirty==2);
                if (pose_data->rot_dirty==1) {
                    chm_Mat4SetRotationFromQuat(pose_matrix_bone_space,pose_data->rot);
                    pose_data->rot_dirty=0;
                }
                else {
                    //chm_QuatFromMat4(pose_data->rot,pose_matrix_bone_space); /* well, this is strictly necessary if we play an animation after the manual pose_matrix change, not now */
                    pose_data->rot_dirty=3; /* we use the private value 3 to defer 'chm_QuatFromMat4(...)' */
                }
            }
            if (pose_data->tra_dirty>0 && pose_data->tra_dirty<3)    {
                CHA_ASSERT(pose_data->tra_dirty==1 || pose_data->tra_dirty==2);
                if (pose_data->tra_dirty==1) {
                    for (i=0;i<3;i++) pose_matrix_bone_space[12+i]=pose_data->tra[i];
                    pose_data->tra_dirty=0;
                }
                else {
                    //for (i=0;i<3;i++) pose_data->tra[i]=pose_matrix_bone_space[12+i]; /* well, this is strictly necessary if we play an animation after the manual pose_matrix change, not now */
                    pose_data->tra_dirty=3; /* we use the private value 3 to defer the line above only when necessary */
                }

            }

            /* what we're doing here is to update our 'pose_matrix_armature_space' using 'pose informations' */
            chm_Mat4MulUncheckArgsf(pose_matrix_armature_space,b->data[CHA_BONE_SPACE_BONE].matrix,pose_matrix_bone_space);
            if (b->parent_idx>=CHA_BONE_NAME_ROOT)
            {
                const float* parent_pose_matrix_armature_space = &p->pose_matrices[CHA_BONE_SPACE_ARMATURE][16*b->parent_idx];
                chm_Mat4Mulf(pose_matrix_armature_space,parent_pose_matrix_armature_space,pose_matrix_armature_space);
                if (!recursive) {CHA_ASSERT(b->parent_idx<bone_idx);}
            }

            if (recursive)  {
                for (i=0;i<b->num_children;i++) {
                    const int child_bone_idx = b->children_idx[i];
                    cha_mesh_instance_update_bone_matrix(p,child_bone_idx,recursive);
                }
            }

            /* Here we have 'pose_matrix_armature_space', but we want to store 'pose_matrix_skinning_space' too:
            we must 'remove' from it the 'bind pose', represented by 'b->data[CHA_BONE_SPACE_ARMATURE]': */
            chm_Mat4MulUncheckArgsf(pose_matrix_skinning_space,pose_matrix_armature_space,b->data[CHA_BONE_SPACE_ARMATURE].matrix_inv);

            /* Here we calculate 'pose_matrix_grabbing_space' */
            chm_Mat4MulUncheckArgsf(pose_matrix_grabbing_space,pose_matrix_armature_space,b->post_pose_armature_space_matrix);
            /*if (bone_idx==CHA_BONE_NAME_ROOT)    {
                CHA_ASSERT(p->mesh);
                //chm_Mat4Rotate(pose_matrix_grabbing_space,180.f,1.f,0.f,0.f);
                //chm_Mat4Translate(pose_matrix_grabbing_space,-p->mesh->aabb_center[0],-p->mesh->aabb_center[1]-b->length,-p->mesh->aabb_center[2]);
                //chm_Mat4Translate(pose_matrix_grabbing_space,0.f,b->length,0.f);
            }*/
        }
   }
}
CHA_API_PRIV /*CHA_API_INL*/ void cha_mesh_instance_update_bone_matrices(struct cha_mesh_instance* p,int start_bone_idx,int end_bone_idx) {
    //p->pose_bone_mask = start_bone_pose_mask;
    if (p->armature)    {
        int bone_idx;
        //cha_mesh_instance_update_bone_matrix(p,0,1);
        if (start_bone_idx<0) start_bone_idx=0;
        if (end_bone_idx<0) end_bone_idx=p->armature->num_bones-1;
        CHA_ASSERT(start_bone_idx<p->armature->num_bones && end_bone_idx<p->armature->num_bones && start_bone_idx<=p->armature->num_bones);
        for (bone_idx=start_bone_idx;bone_idx<=end_bone_idx;bone_idx++)    {
            cha_mesh_instance_update_bone_matrix(p,bone_idx,0);
        }
    }
}
void cha_mesh_instance_update_vertices(struct cha_mesh_instance* p)    {
    float* pverts = NULL,* pnorms = NULL;
    if (p->mesh->shape_keys && p->shk_values_dirty && !p->mesh->shape_keys_type_static)    {
        int i,j,k;
        const struct cha_mesh* mesh = p->mesh;
        const struct cha_mesh_shape_key* shk0 = &mesh->shape_keys[0];
        float weight_sum = 0.f;
        CHA_ASSERT(p->shk_values && p->verts_shk && p->norms_shk);
        CHA_ASSERT(mesh->num_verts==shk0->num_verts);

        memcpy(p->verts_shk,shk0->verts,mesh->num_verts*3*sizeof(float));
        memcpy(p->norms_shk,shk0->norms,mesh->num_verts*3*sizeof(float));
        for (i=1;i<mesh->num_shape_keys;i++)    {
            const float weight = p->shk_values[i];
            CHA_ASSERT(weight>=0.f);
            if (weight>0.f) {
                const struct cha_mesh_shape_key* shki = &mesh->shape_keys[i];
                CHA_ASSERT(shki->num_verts == mesh->num_verts);
                weight_sum+=weight;
                for (j=0;j<mesh->num_verts;j++) {
                    int j3 = j*3;
                    float *v=&p->verts_shk[j3],*n=&p->norms_shk[j3];
                    const float *cv=&shki->verts[j3],*cn=&shki->norms[j3];
                    for (k=0;k<3;k++)   {
                        v[k]+=weight*cv[k];
                        n[k]+=weight*cn[k];
                    }
                }
            }
        }
        p->shk_values_dirty = 0;
        weight_sum = fabs(weight_sum);
        if (weight_sum>0.01f && weight_sum<0.99f)   {
            /* renormalize if necessary */
            for (j=0;j<mesh->num_verts;j++) chm_Vec3Normalizef(&p->norms_shk[j*3]);
        }
        else if (weight_sum<0.01f)  {
            /* use standard mesh->verts, instead of shape key 0 ["Basic"] */
            memcpy(p->verts_shk,mesh->verts,mesh->num_verts*3*sizeof(float));
            memcpy(p->norms_shk,mesh->norms,mesh->num_verts*3*sizeof(float));
        }
        pverts = p->verts_shk;pnorms = p->norms_shk;
    }
    /* here we can calculate skeletal animated verts/norms, based on p->pose_matrices[CHA_BONE_SPACE_SKINNING] */
    if (p->armature
            && p->pose_bone_mask>0             // Dbg code can comment this out
#       ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
            && p->pose_bone_mask>CHA_BONE_MASK_ROOT // > or != ?
#       endif
            )    {
#       define NUM_WEIGHT_PER_VERTEX 3   /* hard coded */
        int i,j,k;
        const struct cha_mesh* mesh = p->mesh;
        const float* cverts = p->verts_shk ? p->verts_shk : mesh->verts;
        const float* cnorms = p->norms_shk ? p->norms_shk : mesh->verts;

        //float mRootGrabInv[16];chm_Mat4Invert(mRootGrabInv,&p->pose_matrices[CHA_BONE_SPACE_GRABBING][16*CHA_BONE_NAME_ROOT]);

        CHA_ASSERT(mesh->num_weights==mesh->num_verts*NUM_WEIGHT_PER_VERTEX);
        CHA_ASSERT(mesh->weights && mesh->weight_bone_masks);
        CHA_ASSERT(p->verts && p->norms);
        if (p->pose_bone_mask==0) p->pose_bone_mask = CHA_BONE_MASK_ALL;
        if (p->pose_bone_mask==CHA_BONE_MASK_ALL)   {
            /* optimization */
            memset(p->verts,0,3*mesh->num_verts*sizeof(float));
            memset(p->norms,0,3*mesh->num_verts*sizeof(float));
        }
        for (i=0;i<mesh->num_verts;i++) {
            const int i3 = 3*i;
            const int inw = NUM_WEIGHT_PER_VERTEX*i;
            float *v = &p->verts[i3], *n = &p->norms[i3];
            const float *vc = &cverts[i3], *nc = &cnorms[i3];
            const unsigned vert_bone_mask = mesh->weight_bone_masks[i];
            float wsum=0.f;
            const int bone_mask_ok = (p->pose_bone_mask&vert_bone_mask)?1:0;
            if (bone_mask_ok) {
                if (p->pose_bone_mask!=CHA_BONE_MASK_ALL)   {
                    memset(v,0,3*sizeof(float));
                    memset(n,0,3*sizeof(float));
                }

                for (j=0;j<NUM_WEIGHT_PER_VERTEX;j++)   {
                    const struct cha_mesh_vertex_weight* w = &mesh->weights[inw+j];
                    if (w->bone_idx<0) break;
                    CHA_ASSERT(w->bone_idx!=0); /* root is not a deform-bone */
                    wsum+=w->weight;
                    {
                        /* Here there are 3 optimizations that can be done:
                           1) In an old .pdf I've read, they didn't sum 3 chm_Mat4MulPos(...),
                              but they created new matrix instead, and then used this matrix to
                              multiply both the vertex and the normal (and the tangent if present)
                              [source:] https://software.intel.com/sites/default/files/m/d/4/1/d/8/293750.pdf
                              Fast Skinning    March 21st 2005    J.M.P. van Waveren  © 2005, Id Software, Inc.
                           2) We can speed up chm_Mat4MulPos(...) to use SIMD, and/or rearrange all the structs
                              (using a preprocessing and postprocessing step) to make them more SIMD friendly
                           3) We can simply support hardware skinning, so that we can reach the maximum performance
                           Of course we're strong supporters of all the 'slow skinning' techniques, so we don't implement
                           any of these bad speed hacks.
                        */
                        const float* pose_matrix = &p->pose_matrices[CHA_BONE_SPACE_SKINNING][16*w->bone_idx];
                        {
                            float tmp[3];
                            chm_Mat4MulPosf(pose_matrix,tmp,vc[0],vc[1],vc[2]);
                            for (k=0;k<3;k++) v[k]+=tmp[k]*w->weight;

                            /*float tmp[3];float tmp2[3];
                            chm_Mat4MulPosf(pose_matrix,tmp,vc[0],vc[1],vc[2]);
                            chm_Mat4MulPosf(mRootGrabInv,tmp2,tmp[0],tmp[1],tmp[2]);
                            for (k=0;k<3;k++) v[k]+=tmp2[k]*w->weight;*/
                        }
                        {
                            float tmp[3];
                            chm_Mat4MulDirf(pose_matrix,tmp,nc[0],nc[1],nc[2]);
                            for (k=0;k<3;k++) n[k]+=tmp[k]*w->weight;

                            /*float tmp[3];float tmp2[3];
                            chm_Mat4MulDirf(pose_matrix,tmp,nc[0],nc[1],nc[2]);
                            chm_Mat4MulDirf(mRootGrabInv,tmp2,tmp[0],tmp[1],tmp[2]);
                            for (k=0;k<3;k++) n[k]+=tmp2[k]*w->weight;*/
                        }
                    }
                }
                //chm_Vec3Normalizef(n);    /* optional */
                CHA_ASSERT(fabs(wsum-1.f)<0.001f);
                //memset(p->norms,0,3*mesh->num_verts*sizeof(float));   // no normals
            }
        }
#       undef NUM_WEIGHT_PER_VERTEX
        pverts = p->verts;pnorms = p->norms;
    }

#   ifdef CHA_USE_VBO
    if (pverts && pnorms && p->vbo)   {
        // TODO: we don't need this if character is not visible (but probably we don't need to call this method in first place in that case)
        const int num_verts = p->mesh->num_verts;
        const size_t verts_size_in_bytes = sizeof(float)*3*num_verts;
        CHA_ASSERT(p->vbo>0);
        glBindBuffer(GL_ARRAY_BUFFER, p->vbo);

        glBufferData(GL_ARRAY_BUFFER, 2*verts_size_in_bytes, NULL, GL_STREAM_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, verts_size_in_bytes, pverts);
        glBufferSubData(GL_ARRAY_BUFFER, verts_size_in_bytes, verts_size_in_bytes, pnorms);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#   endif
}


struct cha_character_instance {
    char name[128];
#   ifdef CHA_CHARACTER_INSTANCE_HAS_MMATRIX_PTR
    choat* mMatrixIn;         // yours    ->  write-only
#   else
    choat mMatrixIn[16];      // yours    ->  write-only
#   endif
    choat mMatrixOut[16];  // mMatrixOut = mMatrixIn + scaling + rotation(.blend2gl)  -> read-only
    float mvMatrixOut[16]; // mvMatrixOut = vMatrix * mMatrixOut (always float) -> read-only
    float mvMatrixRootBoneOut[16];  // an mvMatrixOut that takes armature root bone animation into account (mesh_instances[0].mesh->aabbMin/Max)
    float scaling[3];         // read/write
    int active;            // yours (it excludes the instance when 0)
    int culled;            // read-only
    int num_materials;struct cha_material* materials;
    int num_meshes;struct cha_mesh_instance* mesh_instances;   /* size = num_meshes == CHA_MESH_NAME_COUNT */
    int group_idx;                              // index inside 'parent_group'
    struct cha_character_group* parent_group;
#   ifdef CHA_CHARACTER_INSTANCE_USER_CODE
    CHA_CHARACTER_INSTANCE_USER_CODE
#   endif
};
void cha_character_instance_init(struct cha_character_instance* p,const char* name,int gender_type_female)  {
    const float id[16]={1,0,0,0,    0,1,0,0,    0,0,1,0,    0,0,0,1};
    int i;const size_t len = strlen(name);
    memset(p,0,sizeof(*p));
    if (len<127) strcpy(p->name,name);
    else {strncpy(p->name,name,127);p->name[127]='\0';}
    p->group_idx = -1; p->parent_group = NULL;
    p->active = 1;
    if (p->mMatrixIn) memcpy(p->mMatrixIn,id,16*sizeof(float));
    memcpy(p->mvMatrixOut,id,16*sizeof(float));
    p->scaling[0]=p->scaling[1]=p->scaling[2]=1.f;
    p->num_materials = CHA_MATERIAL_NAME_COUNT;
    p->materials = (struct cha_material*) cha_malloc(p->num_materials*sizeof(struct cha_material));
    cha_materials_reset(p->materials);
    //for (i=0;i<p->num_materials;i++) memcpy(&p->materials[i],&gMaterials[i][0],sizeof(struct cha_material));
    p->num_meshes = CHA_MESH_NAME_COUNT;
    p->mesh_instances = (struct cha_mesh_instance*) cha_malloc(p->num_meshes*sizeof(struct cha_mesh_instance));
    /*memset(p->mesh_instances,0,p->num_meshes*sizeof(struct cha_mesh_instance));*/
    for (i=0;i<CHA_MESH_NAME_COUNT;i++) {
        const int gender_type = (gender_type_female && (i==CHA_MESH_NAME_BODY || i==CHA_MESH_NAME_HEAD)) ? 0 : -1;
        cha_mesh_instance_init(&p->mesh_instances[i],&gCharacterMeshes[i],gender_type);
        p->mesh_instances[i].parent_character_instance=p;
    }
}
void cha_character_instance_destroy(struct cha_character_instance* p)   {
    if (p->mesh_instances)  {
        int i;for(i=0;i<p->num_meshes;i++)  {
            struct cha_mesh_instance* mi = &p->mesh_instances[i];
            if (mi) cha_mesh_instance_destroy(mi);
        }
        cha_free(p->mesh_instances);p->mesh_instances=NULL;
    }
    if (p->materials)   {cha_free(p->materials);p->materials=NULL;}
    p->num_materials=0;
    p->num_meshes=0;    
    p->name[0]='\0';
    p->group_idx=-1;
    p->parent_group=NULL;
}


void cha_character_instance_draw(const struct cha_character_instance* inst,int no_materials/*=0*/,int mesh_name_mask_to_exclude/*=0*/,void (*mesh_instance_draw_callback)(const struct cha_mesh_instance* mi,const float* mvMatrix16,int no_materials/*=0*/,void* user_data/*=NULL*/),void* user_data) {
    int mi_idx;int use_parent_offset_matrix_link = 0;
    CHA_ASSERT(mesh_instance_draw_callback);
    if (no_materials) mesh_name_mask_to_exclude|=CHA_MESH_MASK_MOUTH_AND_EYE;
    for (mi_idx=0;mi_idx<inst->num_meshes;mi_idx++)    {
        const struct cha_mesh_instance* mi = &inst->mesh_instances[mi_idx];
        if (!mi->culled)    {
            const int mi_idx_mask = (1<<(unsigned)mi_idx);
            CHA_ASSERT(mi->mesh);
            if (!(mesh_name_mask_to_exclude&mi_idx_mask)) {
                (*mesh_instance_draw_callback)(mi,use_parent_offset_matrix_link ? mi->mvMatrix_link : mi->mvMatrix,no_materials,user_data);
                if (mi->mesh->parent_offset_matrix_link_present && mi->mesh->parent_mesh_idx>=0)   {
                    /* trick to process the other eye here */
                    if (use_parent_offset_matrix_link)  use_parent_offset_matrix_link=0;
                    else    {use_parent_offset_matrix_link=1;--mi_idx;}
                }
            }
        }
    }
}

struct cha_character_group {
    struct cha_character_instance* instances;int num_instances; /* these gets allocated */
    struct cha_character_instance *men,* ladies;int num_men,num_ladies;   /* these are just references into the 'instances' array */
};


void cha_character_group_updateMatrices(struct cha_character_group** pp,int num_group_pointers,const choat* CHA_RESTRICT vMatrix,const float pMatrixNormalizedFrustumPlanesOrNull[6][4])  {
    /* inst->mMatrixOut = inst->mMatrixIn + scaling + rotation(.blend2gl);  -> read-only */
    int gi,i,k,l;choat tm[16]={1,0,0,0,  0,0,-1,0,   0,1,0,0,    0,0,0,1};
#   ifdef CHA_DEBUG_FRUSTUM_CULLING
    int num_culled_instances[CHA_MESH_NAME_COUNT+1]=CHA_ZERO_INIT;
    static int num_culled_instances_last[CHA_MESH_NAME_COUNT+1]=CHA_ZERO_INIT;
#   endif
#   ifdef CHA_DOUBLE_PRECISION
    double mvMatrixd[16];const int aabb_idx_map[3]={0,2,1};
#   endif
    for (gi=0;gi<num_group_pointers;gi++)  {
        struct cha_character_group* g = pp[gi];
        for (i=0;i<g->num_instances;i++)    {
            struct cha_character_instance* inst = &g->instances[i];
            if (inst->active)   {
                inst->culled = 0;
                tm[0]=inst->scaling[0];tm[6]=-inst->scaling[2];tm[9]=inst->scaling[1];
#               ifdef CHA_DOUBLE_PRECISION
                chm_Mat4MulUncheckArgsd(inst->mMatrixOut,inst->mMatrixIn,tm); // (with so many zeros we can do better...)
#               else
                chm_Mat4MulUncheckArgsf(inst->mMatrixOut,inst->mMatrixIn,tm); // (with so many zeros we can do better...)
#               endif
                if (vMatrix)    {
                    struct cha_mesh_instance* mi = &inst->mesh_instances[CHA_MESH_NAME_BODY];
                    const struct cha_mesh* mesh = mi->mesh;
                    const struct cha_armature_bone* b = &mi->armature->bones[CHA_BONE_NAME_ROOT];
                    const float* gMatrix = &mi->pose_matrices[CHA_BONE_SPACE_GRABBING][CHA_BONE_NAME_ROOT*16];
                    int use_parent_offset_matrix_link = 0;
#                   ifdef CHA_DOUBLE_PRECISION
                    chm_Mat4MulUncheckArgsd(mvMatrixd,vMatrix,inst->mMatrixOut); // doubles
                    // Here we must convert mvMatrixd to float, but we can lose precision...
                    // But we cull out values that exceed the FLT_MAX boundaries (hoping that user frustum is not so long)
                    for (k=0;k<3;k++)   {
#                       ifndef FLT_MAX
#                           define CHA_FLT_MAX (340282346638528859811704183484516925440.0)
#                       else
#                           define CHA_FLT_MAX FLT_MAX  /* well, I didn't want to include <float.h> */
#                       endif
                        const double val = mvMatrixd[12+k];
                        const double limit = (double)CHA_FLT_MAX - (double)(mesh->aabb_half_extents[aabb_idx_map[k]]*inst->scaling[k])*2.0;    // the map is because: aabb_half_extents[1] is Z and aabb_half_extents[2] is Y
                        if (val>limit || val<-limit)    {inst->culled=mi->culled=1;break;}
                    }
                    chm_Mat4Convertd2f(inst->mvMatrixOut,mvMatrixd);    // well here we lose all the precision
                    if (inst->culled)   {
                        chm_Mat4Copyf(inst->mvMatrixRootBoneOut,inst->mvMatrixOut);  // wrong, but better than nothing
                        continue;
                    }
#                   else
                    chm_Mat4MulUncheckArgsf(inst->mvMatrixOut,vMatrix,inst->mMatrixOut); // always floats
#                   endif

                    mi->pose_bone_mask=0;
                    cha_mesh_instance_update_bone_matrix(mi,0,0);   // updates root bone animation only (if necessary) and modifies mi->pose_bone_mask.


//#                   ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
                    chm_Mat4MulUncheckArgsf(inst->mvMatrixRootBoneOut,inst->mvMatrixOut,gMatrix);
//#                   else
//                    chm_Mat4MulUncheckArgsf(inst->mvMatrixRootBoneOut,mi->mvMatrix,gMatrix);
//#                   endif

                    // in the line below: inst->mvMatrixOut works, but mi->mvMatrix does not work. Why?
                    // There's still a Z offset (b->length) that must be appended here, why?
                    chm_Mat4Translatef(inst->mvMatrixRootBoneOut,0.f,b->length,0.f);

                    if (pMatrixNormalizedFrustumPlanesOrNull)   {
                        inst->culled = mi->culled = chm_IsOBBVisiblef(pMatrixNormalizedFrustumPlanesOrNull,inst->mvMatrixRootBoneOut,mesh->aabb_min[0],mesh->aabb_min[1],mesh->aabb_min[2],mesh->aabb_max[0],mesh->aabb_max[1],mesh->aabb_max[2]) ? 0 : 1;
                        if (inst->culled) {
#                            ifdef CHA_DEBUG_FRUSTUM_CULLING
                            ++num_culled_instances[0];
#                           endif
                            continue;
                        }
                    }

                    cha_mesh_instance_update_bone_matrices(mi,1,-1); // updates bone animations (except root) (if necessary) and modifies mi->pose_bone_mask.
                    for (l=0;l<inst->num_meshes;l++)    {
                        struct cha_mesh_instance* mi = &inst->mesh_instances[l];
                        const struct cha_mesh* mesh = mi->mesh;
                        float* mv = use_parent_offset_matrix_link ? (float*) mi->mvMatrix_link : (float*) mi->mvMatrix;
                        CHA_ASSERT(mesh);
                        mi->culled = 0;
                        //--------------------------------------------------------------------
                        if (mesh->parent_mesh_idx>=0) {
                            /* Here we calculate and store 'mi->offset_transform' */
                            const struct cha_mesh_instance* pmi = &inst->mesh_instances[mesh->parent_mesh_idx];
                            if (mesh->parent_bone_idx>=CHA_BONE_NAME_ROOT)
                            {
                                CHA_ASSERT(pmi->armature && mesh->parent_bone_idx!=CHA_BONE_NAME_ROOT);
                                chm_Mat4MulUncheckArgsf(mv,pmi->mvMatrix,&pmi->pose_matrices[CHA_BONE_SPACE_GRABBING][16*mesh->parent_bone_idx]); // or just (pmi->)mMatrix ?
                            }
                            else memcpy(mv,pmi->mvMatrix,16*sizeof(float)); /* Warning: 'mesh_name_mask_to_exclude' could exclude calculation of parent 'pmi->offset_transform' before this line */
                            chm_Mat4Mulf(mv,mv,use_parent_offset_matrix_link ? mesh->parent_offset_matrix_link : mesh->parent_offset_matrix);
                        }
                        else memcpy(mv,inst->mvMatrixOut,16*sizeof(float));
#                       ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
                        if (mi->armature && mi->pose_bone_mask==CHA_BONE_MASK_ROOT) {
                            chm_Mat4Mulf(mv,mv,&mi->pose_matrices[CHA_BONE_SPACE_GRABBING][16*CHA_BONE_NAME_ROOT]);  // Basically the same as inst->mvMatrixRootBoneOut, but without the bone length translation
                        }
#                       endif
                        //--------------------------------------------------------------------                        
                        if (l==CHA_MESH_NAME_EYE || l==CHA_MESH_NAME_MOUTH) {
                            if (mesh->parent_mesh_idx>=0 && inst->mesh_instances[mesh->parent_mesh_idx].culled)   {
                                CHA_ASSERT(mesh->parent_mesh_idx<l);
                                mi->culled = 1;
                            }
                            else mi->culled = (mv[4]*mv[12]+mv[5]*mv[13]+mv[6]*mv[14]<0.0f) ? 1 : 0; // zAxis is &mv[4] for us
                        }
#                       ifndef CHA_NO_FRUSTUM_CULLING_ON_CHARACTER_SUBPARTS
                        else if (l!=CHA_MESH_NAME_BODY && pMatrixNormalizedFrustumPlanesOrNull) {
                            CHA_ASSERT(!mi->armature);
                            mi->culled = chm_IsOBBVisiblef(pMatrixNormalizedFrustumPlanesOrNull,mv,mesh->aabb_min[0],mesh->aabb_min[1],mesh->aabb_min[2],mesh->aabb_max[0],mesh->aabb_max[1],mesh->aabb_max[2]) ? 0 : 1;
                        }
#                       endif
                        if (!mi->culled) {
                            //----------------------------------------------------------------------
                            if (mesh->parent_offset_matrix_link_present && mesh->parent_mesh_idx>=0)   {
                                /* trick to process the other eye here */
                                if (use_parent_offset_matrix_link)  use_parent_offset_matrix_link=0;
                                else    {use_parent_offset_matrix_link=1;--l;continue;}
                            }
                            //----------------------------------------------------------------------
                            cha_mesh_instance_update_vertices(mi);  // mi->pose_bone_mask is used here.
                        }
#                       ifdef CHA_DEBUG_FRUSTUM_CULLING
                        else ++num_culled_instances[l+1];
#                       endif
                    }
                }
            }
        }
    }
#   ifdef CHA_DEBUG_FRUSTUM_CULLING
    if (num_culled_instances[0]!=num_culled_instances_last[0])    {fprintf(stderr,"num_culled_instances = %d;\n",num_culled_instances[0]);num_culled_instances_last[0]=num_culled_instances[0];}
    for (i=1;i<CHA_MESH_NAME_COUNT+1;i++)   {if (num_culled_instances[i]!=num_culled_instances_last[i])    {fprintf(stderr,"num_culled_meshes[\"%s\"] = %d;\n",gCharacterMeshes[i-1].name,num_culled_instances[i]);num_culled_instances_last[i]=num_culled_instances[i];}}
#   endif
}
void cha_character_group_draw(struct cha_character_group*const* pp,int num_group_pointers,int no_materials/*=0*/,int mesh_name_mask_to_exclude/*=0*/,void (*mesh_instance_draw_callback)(const struct cha_mesh_instance* mi,const float* mvMatrix16,int no_materials/*=0*/,void* user_data/*=NULL*/),void* user_data) {
    int gi,i;
    CHA_ASSERT(mesh_instance_draw_callback);
    for (gi=0;gi<num_group_pointers;gi++)  {
        const struct cha_character_group* g = pp[gi];
        for (i=0;i<g->num_instances;i++)    {
            const struct cha_character_instance* inst = &g->instances[i];
            if (inst->active)   {
                cha_character_instance_draw(inst,no_materials,mesh_name_mask_to_exclude,mesh_instance_draw_callback,user_data);
            }
        }
    }
}
struct cha_character_instance* cha_character_group_GetInstanceUnderMouseFromRayf(struct cha_character_group* const* pp,int num_group_pointers,const float* rayOrigin3,const float* rayDir3,float* CHA_RESTRICT pOptionalDistanceOut) {
    // rayOrigin3 and rayDirection3 must be in eye space (i.e. camera space: ray must be retrieved using pMatrixInv and NOT pvMatrixInv; infact here we use mvMatrix, NOT mMatrix)
    struct cha_character_instance* rv = 0;
    float intersection_distance = 0;
    int gi,i,j;
    if (pOptionalDistanceOut) *pOptionalDistanceOut=intersection_distance;
    if (!pp || num_group_pointers<=0) return rv;

    // Loop all meshes and find OBB vs ray intersection
    // Code based on: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/ (WTFPL Public Licence)
    for (gi=0;gi<num_group_pointers;gi++)   {
        const struct cha_character_group* p = pp[gi];
        for (i=0;i<p->num_instances;i++)   {
            struct cha_character_instance* inst = &p->instances[i];
            const struct cha_mesh_instance* mi = &inst->mesh_instances[CHA_MESH_NAME_BODY];
            const struct cha_mesh* mesh = mi->mesh;
            const float* obbMatrix = inst->mvMatrixRootBoneOut;
            float tMin = 0;float tMax = (float)1000000000000;
            int noCollisionDetected = 0;
            float obbPosDelta[3] =  {obbMatrix[12]-rayOrigin3[0],obbMatrix[13]-rayOrigin3[1],obbMatrix[14]-rayOrigin3[2]};
            if (!inst->active || inst->culled) continue;
            for (j=0;j<3;j++)   {
                if (!noCollisionDetected)   {
                    int j4 = 4*j;
                    // Test intersection with the 2 planes perpendicular to the OBB's j axis
                    float axis[3] = {obbMatrix[j4],obbMatrix[j4+1],obbMatrix[j4+2]},e,f;
                    float sca = chm_Vec3Dotf(axis,axis);
                    float aabbMinj = mesh->aabb_min[j];
                    float aabbMaxj = mesh->aabb_max[j];
                    if (sca<(float)0.00009 || sca>(float)1.00001) {
                        sca = sqrtf(sca);
                        aabbMinj*=sca;aabbMaxj*=sca;
                        sca=(float)1/sca;axis[0]*=sca;axis[1]*=sca;axis[2]*=sca;
                    }
                    e = chm_Vec3Dotf(axis, obbPosDelta);
                    f = chm_Vec3Dotf(rayDir3, axis);
                    //if ( abs(f) > 0.001)  // @Flix: the reference code used this (but it does not work for me; so maybe my selection does not work with a projection ortho matrix...)
                    {
                        // Standard case
                        // t1 and t2 now contain distances betwen ray origin and ray-plane intersections:
                        float t1 = (e+aabbMinj)/f; // Intersection with the "left" plane
                        float t2 = (e+aabbMaxj)/f; // Intersection with the "right" plane
                        // We want t1 to represent the nearest intersection, so if it's not the case, invert t1 and t2
                        if (t1>t2)  {float w=t1;t1=t2;t2=w;}
                        if (t2 < tMax)    tMax = t2;
                        if (t1 > tMin)    tMin = t1;
                        // And here's the trick :
                        // If "far" is closer than "near", then there is NO intersection.
                        // See the images in the tutorials for the visual explanation.
                        if (tMin > tMax) noCollisionDetected=1;

                    }
                    /*else    {
                    // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
                    if(-e+aabbMin[j] > 0.0 || -e+aabbMax[j] < 0.0) noCollisionDetected=1;
                }*/
                }
            }

            if (!noCollisionDetected && (intersection_distance<=0 || intersection_distance>tMin))   {
                intersection_distance = tMin;
                rv = inst;
            }
        }
    }

    if (pOptionalDistanceOut) *pOptionalDistanceOut=intersection_distance;
    return rv;
}
struct cha_character_instance* cha_character_group_GetInstanceUnderMousef(struct cha_character_group* const* pp,int num_group_pointers,int mouseX,int mouseY,const int* viewport4,const float* CHA_RESTRICT pMatrixInv,float* CHA_RESTRICT pOptionalDistanceOut) {
    float ray_origin[3] = {0,0,0};
    float ray_dir[3] = {0,0,-1};
    chm_GetRayFromMouseCoordsf(ray_origin,ray_dir,mouseX,mouseY,pMatrixInv,viewport4);
    return cha_character_group_GetInstanceUnderMouseFromRayf(pp,num_group_pointers,ray_origin,ray_dir,pOptionalDistanceOut);
}

void cha_private_copy_material_from_color(struct cha_material* m,unsigned rgba,float shi) {
    if (rgba==0) return;    /* keep last */
    else    {
        const float sca = 1.f/255.f;
#       ifndef CHA_BIG_ENDIAN
        float d[4] = {(float)((rgba>>24)&0xFF)*sca,(float)((rgba>>16)&0xFF)*sca,(float)((rgba>>8)&0xFF)*sca,(float)((rgba)&0xFF)*sca};
#       else    /* ??? */
        float d[4] = {(float)((rgba>>8)&0xFF)*sca,(float)((rgba>>16)&0xFF)*sca,(float)((rgba>>24)&0xFF)*sca,(float)((rgba)&0xFF)*sca};
#       endif
        float a[4],s[3];int i;
        if (d[3]<0.001f) d[3]=1.f;
        for (i=0;i<3;i++)   {
            d[i]*=CHA_LIGHTNESS; if (d[i]>1.f) d[i]=1.f;
            a[i]=d[i]*0.2f;s[i]=d[i]*0.5f;if (s[i]>=1.f) s[i]=1.f;
        }
        a[3]=1.f;
        memcpy(&m->amb[0],a,sizeof(float)*4);
        memcpy(&m->dif[0],d,sizeof(float)*4);
        memcpy(&m->spe[0],s,sizeof(float)*3);
        if (shi>0.0000000001f) m->shi = shi;
        //cha_material_display(m);
    }
}

void cha_character_group_init(struct cha_character_group* p,int num_men,int num_ladies,float men_scaling,float ladies_scaling) {
    const char* male_names[]={"Peter","John","Andrew","James","Philip","Thomas","Matthew","Simon","Bartholomew","Jude",
                              "Matthias","Joseph","Paul","Stephen","Francis","Benedict","Joachim","Charles","Augustine","Dominic"};
    const char* female_names[]={"Mary","Anne","Martha","Susan","Joan","Lucy","Catherine","Margaret","Elizabeth","Theresa",
                               "Rita","Monica","Rachel","Rose","Helena","Barbara","Bridget","Christine","Alexandra","Priscilla"};    
    const size_t num_male_names = sizeof(male_names)/sizeof(male_names[0]);
    const size_t num_female_names = sizeof(female_names)/sizeof(female_names[0]);

#   define CHA_CHARACTER_GROUP_INIT_NUM_COLORS 20
    //const char* gMaterialNames[CHA_MATERIAL_NAME_COUNT] = {"hair","irid","lips","pupil","shoes-belt","skin","t-shirt","teeth","trousers","white_eye"};
    // 0 means: keep default material (and shininess<=0 too)    
    const unsigned colors[CHA_MATERIAL_NAME_COUNT][CHA_CHARACTER_GROUP_INIT_NUM_COLORS]={
        /* CHA_MATERIAL_NAME_HAIR */        {0,0x774315ff,0xEBB100FF,0xAA98A9FF,0x46000aFF,     0x967117FF,0x644117FF,0xFFDB00FF,0x050505FF,0x6F4E37FF,
                                            0x3d2b1fff,0x480607ff,0x81613Cff,0x893f45ff,0x5d3954ff,       0x8b0000ff,0x696969ff,0x534b4fff,0x101010ff,0x202020ff},
        /* CHA_MATERIAL_NAME_IRID */        {0,0x007ba7ff,0x3d2b1fff,0x007fffff,0x800000ff,     0x507D2AFF,0x6F4E37FF,0x5B92E5FF,0x00A86BFF,0x4166F5FF,
                                            0xcd7f32ff,0x6f4e37ff,0x592720ff,0xa3c1adff,0x007aa5ff,       0x800020ff,0x08e8deff,0x6699ccff,0xa4c639ff,0x9966ccff},
        /* CHA_MATERIAL_NAME_LIPS */        {0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0},
        /* CHA_MATERIAL_NAME_PUPIL*/        {0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0},
        /* CHA_MATERIAL_NAME_SHOES_BELT*/   {0,0x800000ff,0xaf4035ff,0x967117ff,0x0f0f0fff,     0x704214FF,0xA2ADD0FF,0x882D17FF,0x738678FF,0x7E5E60FF,
                                            0xB5B35Cff,0x673107ff,0x3D484Aff,0x002147ff,0xE12C2Cff,       0x2A2F23ff,0xcc8899ff,0x436B95ff,0x996515ff,0xBEBEBEff},
        /* CHA_MATERIAL_NAME_SKIN*/         {0,0,0xfc89acff,0xdeaa88ff,0,     0xF6ADC6FF,0,0xF2BDCDFF,0xDEA5A4FF,0x644117FF,
                                            0xe18e96ff,0xe18e96ff,0,0,0,       0,0x836953ff,0xf6adc6ff,0xe18e96ff,0},
        /* CHA_MATERIAL_NAME_T_SHIRT*/      {0,0x035096ff,0x004953ff,0xae0c00ff,0xff7518ff,     0x014421FF,0xFFD300FF,0x007FFFFF,0xD9E650ff,0x1164B4FF,
                                            0x008000ff,0x09A650ff,0x4166B5FF,0x004953ff,0xFF7E00ff,       0x800000ff,0x841B2Dff,0x3B3C36ff,0xFFAA1Dff,0x88540Bff},
        /* CHA_MATERIAL_NAME_TEETH*/        {0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0},
        /* CHA_MATERIAL_NAME_TROUSERS*/     {0,0x003153ff,0x004040ff,0x0038a8ff,0xc2b280ff,     0xE9D66BFF,0x2F4F4FFF,0x008000FF,0xE6E6FAFF,0x73C2FBFF,
                                            0x800000ff,0x88540Bff,0x305933ff,0x36454Fff,0x232B2Bff,       0x9E1B32ff,0x536878ff,0x008000ff,0x4A5D23ff,0x3C1414ff},
        /* CHA_MATERIAL_NAME_WHITE_EYE*/    {0,0,0,0,0, 0,0,0,0,0,  0,0,0,0,0, 0,0,0,0,0}
    };
    const float shininesses[CHA_MATERIAL_NAME_COUNT] = {
        /* CHA_MATERIAL_NAME_HAIR */        0,
        /* CHA_MATERIAL_NAME_IRID */        0,
        /* CHA_MATERIAL_NAME_LIPS */        0,
        /* CHA_MATERIAL_NAME_PUPIL*/        0,
        /* CHA_MATERIAL_NAME_SHOES_BELT*/   0,
        /* CHA_MATERIAL_NAME_SKIN*/         0,
        /* CHA_MATERIAL_NAME_T_SHIRT*/      0,
        /* CHA_MATERIAL_NAME_TEETH*/        0,
        /* CHA_MATERIAL_NAME_TROUSERS*/     0,
        /* CHA_MATERIAL_NAME_WHITE_EYE*/    0
    };

    const choat id16[16] = {1,0,0,0,    0,1,0,0,    0,0,1,0,    0,0,0,1},step_angle=2.399892449;
    choat step_radius = men_scaling*0.4,angle=0,radius=0;
    int i,j;const int num_instances = num_men+num_ladies;
    if (men_scaling<=0.f) men_scaling=1.f;
    if (ladies_scaling<=0.f) ladies_scaling=1.f;
    memset(p,0,sizeof(*p));
    CHA_ASSERT(num_men>=0 && num_ladies>=0 && num_instances>0);
    CHA_ASSERT(num_male_names==num_female_names && num_male_names==20);
#   ifndef CHA_NO_CHARACTER_INSTANCE_NAMES
    CHA_ASSERT((int)CHA_CHARACTER_INSTANCE_MAN_NAME_COUNT==(int)CHA_CHARACTER_INSTANCE_LADY_NAME_COUNT && CHA_CHARACTER_INSTANCE_MAN_NAME_COUNT==num_male_names);
#   endif
    p->num_instances=num_instances;
    p->instances = (struct cha_character_instance*) cha_malloc(p->num_instances*sizeof(struct cha_character_instance));
    p->num_men=num_men;p->num_ladies=num_ladies;
    if (p->num_men)     p->men=p->instances;
    if (p->num_ladies)  p->ladies=&p->instances[num_men];
    if (p->num_instances<5) step_radius+=step_radius*0.15f*p->num_instances;
    for (i=0;i<p->num_instances;i++)    {
        const int is_man = i<p->num_men?1:0;
        const char* name = is_man ? male_names[i%num_male_names] : female_names[(i-p->num_men)%num_female_names];
        struct cha_character_instance* inst = &p->instances[i];
        choat* m = inst->mMatrixIn;
        const int interleaved_idx = is_man ?
                    (i<p->num_ladies ? (i*2) : (p->num_ladies*2)+(i-p->num_ladies)) :
                    (i-p->num_men)<p->num_men ? (((i-p->num_men)*2+1)) : (((p->num_men)*2+(i-2*p->num_men)))
                    ;
        const int mtl_idx = (is_man ? (i+(CHA_CHARACTER_GROUP_INIT_NUM_COLORS/2+1)) : (i-p->num_men))%CHA_CHARACTER_GROUP_INIT_NUM_COLORS;
        const choat sina = sin(angle),cosa = cos(angle);
        cha_character_instance_init(inst,name,i<p->num_men?0:1);
        inst->parent_group = p;inst->group_idx=i;

        if (is_man) {inst->scaling[0]=inst->scaling[1]=inst->scaling[2]=men_scaling;}
        else {inst->scaling[0]=inst->scaling[1]=inst->scaling[2]=ladies_scaling;}
        // positions
        CHA_ASSERT(interleaved_idx<p->num_instances);
#       ifdef CHA_DOUBLE_PRECISION
        chm_Mat4Copyd(m,id16);
#       else
        chm_Mat4Copyf(m,id16);
#       endif
        angle = interleaved_idx*step_angle;
        radius = interleaved_idx*step_radius;
        m[12]=radius*cosa;
        m[14]=radius*sina;
        // materials
        CHA_ASSERT(mtl_idx<CHA_CHARACTER_GROUP_INIT_NUM_COLORS);
        //printf("mtl_idx=%d\n",mtl_idx);
        for (j=0;j<CHA_MATERIAL_NAME_COUNT;j++) {
            cha_private_copy_material_from_color(&inst->materials[j],colors[j][mtl_idx],shininesses[j]);
        }

    }
    cha_character_group_updateMatrices(&p,1,NULL,NULL);

}
void cha_character_group_destroy(struct cha_character_group* p)   {
    if (p->instances) {
        int i;for (i=0;i<p->num_instances;i++) {
            struct cha_character_instance* inst = &p->instances[i];
            cha_character_instance_destroy(inst);
        }
        cha_free(p->instances);p->instances=NULL;
    }
    p->men=p->ladies=NULL;
    p->num_instances=p->num_men=p->num_ladies=0;
}




CHA_API_DEF void Character_Init(void) {

    cha_armature_init_body(&gCharacterArmatures[CHA_ARMATURE_NAME_BODY]);
    cha_mesh_init_body(&gCharacterMeshes[CHA_MESH_NAME_BODY]);
    cha_mesh_init_head(&gCharacterMeshes[CHA_MESH_NAME_HEAD]);
    cha_mesh_init_eye(&gCharacterMeshes[CHA_MESH_NAME_EYE]);
    cha_mesh_init_mouth(&gCharacterMeshes[CHA_MESH_NAME_MOUTH]);

    cha_materials_init_names(&gMaterialNames);   /* this doesn't need destruction */

    /*cha_armature_display(&gCharacterArmatures[CHA_ARMATURE_NAME_MAN]);*/
    /*for (i=0;i<CHA_MESH_NAME_COUNT;i++) cha_mesh_display(&gCharacterMeshes[i]);*/
    /*cha_mesh_display(&gCharacterMeshes[CHA_MESH_NAME_MAN]);*/
    /*cha_mesh_display(&gCharacterMeshes[CHA_MESH_NAME_EYE]);
    cha_mesh_display(&gCharacterMeshes[CHA_MESH_NAME_MOUTH]);*/

    CHA_ASSERT(CHA_BONE_MASK_ALL==(1<<gCharacterArmatures[CHA_ARMATURE_NAME_BODY].num_bones)-1);
}
CHA_API_DEF void Character_Destroy(void)  {
    int i;       
    for (i=0;i<CHA_MESH_NAME_COUNT;i++) cha_mesh_destroy(&gCharacterMeshes[i]);
    for (i=0;i<CHA_ARMATURE_NAME_COUNT;i++) cha_armature_destroy(&gCharacterArmatures[i]);
}

CHA_API_DEF struct cha_character_group* Character_CreateGroup(int num_men,int num_ladies,float men_scaling,float ladies_scaling) {
    struct cha_character_group* p = (struct cha_character_group*) cha_malloc(sizeof(struct cha_character_group));
    cha_character_group_init(p,num_men,num_ladies,men_scaling,ladies_scaling);return p;
}
CHA_API_DEF void Character_DestroyGroup(struct cha_character_group* p) {
    if (p)    {cha_character_group_destroy(p);cha_free(p);}
}

#if (defined(CHA_HAS_OPENGL_SUPPORT) && (!defined(CHA_USE_VBO) || defined(CHA_HINT_USE_FFP_VBO)))
void cha_mesh_instance_draw_callback_opengl(const struct cha_mesh_instance* mi,const float* mvMatrix16,int no_materials/*=0*/,void* user_data);
CHA_API_DEF void Character_DrawGroupOpengl(struct cha_character_group*const* pp,int num_group_pointers,int no_materials/*=0*/)  {
    const float zero[4]={0,0,0,1};int i;
#   ifdef CHA_USE_VBO
#       ifndef CHA_HINT_USE_VAO
#           ifndef CHA_HINT_USE_FFP_VBO
#               error CHA_HINT_USE_FFP_VBO must be defined
#           endif
        glEnableClientState(GL_VERTEX_ARRAY);
        if (!no_materials)  glEnableClientState(GL_NORMAL_ARRAY);
#       endif
#   endif
    cha_character_group_draw(pp,num_group_pointers,no_materials,0,&cha_mesh_instance_draw_callback_opengl,NULL);
#   ifdef CHA_USE_VBO
#       ifdef CHA_HINT_USE_VAO
        glBindVertexArray(0);
#       else
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
#       endif
#   endif
        glMaterialfv(GL_FRONT,GL_SPECULAR,zero);
}
#endif

#ifdef CHA_HAS_OPENGL_SUPPORT
#   ifndef CHA_USE_VBO
void cha_mesh_instance_draw_callback_opengl(const struct cha_mesh_instance* mi,const float* mvMatrix16,int no_materials/*=0*/,void* user_data) {
    /* no 'vbo' here: we must use glBegin(GL_TRIANGLES)/glEnd() */
    const struct cha_mesh* mesh = mi->mesh;
    const unsigned short *inds = mesh->inds;
    const float *verts = mi->verts ? mi->verts : (mi->verts_shk ? mi->verts_shk : mesh->verts);
    const float *norms = mi->norms ? mi->norms : (mi->norms_shk ? mi->norms_shk : mesh->norms);
    int i,j;(void)user_data;

    glPushMatrix();
    glMultMatrixf(mvMatrix16);
    if (!no_materials)  {
        //glBegin(GL_TRIANGLES);
        for (i=0;i<mesh->num_parts;i++) {
            const struct cha_mesh_part* part = &mesh->parts[i];

            const int indsStart = part->inds_start;
            const int indsCount = part->inds_count;
            const int indsEnd = indsStart+indsCount;
            unsigned short i0;            const float *v0;
            const float *n0;
            const struct cha_material* pMat;

            CHA_ASSERT(indsCount%3==0);

            /* Technically glMaterialfv calls could be placed inside glBegin()/glEnd(),
            but when using shaders (and we do it for shadows), that doesn't seem work. */
            pMat = &mi->parent_character_instance->materials[part->material_idx];//gMaterials[part->material_idx];
            glMaterialfv(GL_FRONT,GL_AMBIENT,pMat->amb);
            glMaterialfv(GL_FRONT,GL_DIFFUSE,pMat->dif);
            glMaterialfv(GL_FRONT,GL_SPECULAR,pMat->spe);   /* shi is in pMat->spe[3]. However 'glMaterialf' docs say: 'While the ambient, diffuse, specular and emission material parameters all have alpha components, only the diffuse alpha component is used in the lighting computation.' */
            glMaterialf(GL_FRONT,GL_SHININESS,pMat->spe[3]);


            glBegin(GL_TRIANGLES);
            for (j=indsStart;j<indsEnd;j++)  {
                i0 = inds[j];
                v0 = &verts[i0*3];
                n0 = &norms[i0*3];
                glNormal3fv(n0);
                glVertex3fv(v0);
            }
            glEnd();
        }
        //glEnd();
    }
    else    {
        /* 'no_materials': faster code for depth-only rendering */
        glBegin(GL_TRIANGLES);
        /*for (i=0;i<mesh->num_parts;i++) {
            const struct cha_mesh_part* part = &mesh->parts[i];
            const int indsStart = part->inds_start;
            const int indsCount = part->inds_count;
            const int indsEnd = indsStart+indsCount;
            CHA_ASSERT(indsCount%3==0);
            for (j=indsStart;j<indsEnd;j++)  glVertex3fv(&verts[inds[j]*3]);
        }*/
        for (j=0;j<mesh->num_inds;j++)  glVertex3fv(&verts[inds[j]*3]);
        glEnd();
    }
    glPopMatrix();
}
#   elif defined(CHA_HINT_USE_FFP_VBO)
void cha_mesh_instance_draw_callback_opengl(const struct cha_mesh_instance* mi,const float* mvMatrix16,int no_materials/*=0*/,void* userData) {
    /* 'vbo' support here */
    const struct cha_mesh* mesh = mi->mesh;
    const GLuint vbo = mi->vbo ? mi->vbo : (mi->static_shk_index>=0 ? mesh->shape_keys[mi->static_shk_index].vbo : mesh->vbo);
    const GLuint ibo = mesh->ibo;
    const GLuint vao = mi->vao ? mi->vao : (mi->static_shk_index>=0 ? mesh->shape_keys[mi->static_shk_index].vao : mesh->vao);
    int i;(void)userData;

    CHA_ASSERT(vbo && ibo);

    // Two lines to comment out:
//    if (mesh->idx==CHA_MESH_NAME_MAN || mesh->idx==CHA_MESH_NAME_HEAD)  {CHA_ASSERT(mesh->shape_keys && mesh->shape_keys[0].vbo);}
//    else if (mesh->shape_keys) {CHA_ASSERT(!mesh->shape_keys[0].vbo);}

#   ifdef CHA_HINT_USE_VAO
    glBindVertexArray(vao);
#   else
#       ifndef CHA_HINT_USE_FFP_VBO
#           error CHA_HINT_USE_FFP_VBO must be defined
#       endif
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glVertexPointer(3, GL_FLOAT, 0, (void*)0);
#   endif

    glPushMatrix();
    glMultMatrixf(mvMatrix16);
    if (no_materials)  {
        glDrawElements(GL_TRIANGLES, mesh->num_inds, GL_UNSIGNED_SHORT,(const void *) (0));
    }
    else {
#       ifndef CHA_HINT_USE_VAO
        glNormalPointer(GL_FLOAT, 0, (void*)(3*sizeof(float)*mesh->num_verts));
#       endif
        for (i=0;i<mesh->num_parts;i++) {
            const struct cha_mesh_part* part = &mesh->parts[i];
            const int indsStart = part->inds_start;
            const int indsCount = part->inds_count;
            const struct cha_material* pMat = &mi->parent_character_instance->materials[part->material_idx];
            CHA_ASSERT(indsCount%3==0);
            glMaterialfv(GL_FRONT,GL_AMBIENT,pMat->amb);
            glMaterialfv(GL_FRONT,GL_DIFFUSE,pMat->dif);
            glMaterialfv(GL_FRONT,GL_SPECULAR,pMat->spe);   /* shi is in pMat->spe[3]. However 'glMaterialf' docs say: 'While the ambient, diffuse, specular and emission material parameters all have alpha components, only the diffuse alpha component is used in the lighting computation.' */
            glMaterialf(GL_FRONT,GL_SHININESS,pMat->spe[3]);

            glDrawElements(GL_TRIANGLES, indsCount, GL_UNSIGNED_SHORT,(const void *) (indsStart*sizeof(unsigned short)));
        }
    }
    glPopMatrix();
}
#   endif
#endif /*CHA_HAS_OPENGL_SUPPORT */
/* ===============================================================================================*/








#endif /* CHA_IMPLEMENTATION_GUARD */
#endif /* CHARACTER_IMPLEMENTATION */
