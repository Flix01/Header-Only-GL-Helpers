// https://github.com/Flix01/Header-Only-GL-Helpers
//
/** MIT License
 *
 * Copyright (c) 2017 Flix (https://github.com/Flix01/)
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

// USAGE:
/*
im_matrix_stack.h does not depend on any OpenGL header.

im_matrix_stack.h implements two kind of methods:
1) Methods starting with im: these are (extended versions of a subset of the) matrix stack methods in immediate mode (=ffp) OpenGL
2) Methods starting with IM: these are some kind of "static" helper methods

TO BE TESTED: When programming in C++ all the methods at point 1) can be accessed with the same syntax of immediate mode (=ffp) OpenGL,
by using MACRO_GL_IMMEDIATE_MODE_MATRIX_STACK_SCOPE in any scope like this:
{
    MACRO_GL_IMMEDIATE_MODE_MATRIX_STACK_SCOPE
    // Now we can use, for example:

    glLoadIdentity(); // and so on
}


//#define IM_MATRIX_STACK_USE_DOUBLE_PRECISION if you want to use doubles.
#define IM_MATRIX_STACK_IMPLEMENTATION  must be defined in one of the source files
*/

#ifndef IM_MATRIX_STACK_H
#define IM_MATRIX_STACK_H

#ifdef __cplusplus
extern "C"	{
#endif

#define IM_MATRIX_STACK_VERSION 0.5

#ifdef IM_MATRIX_STACK_NO_RESTRICT  // please define it globally if the keyword __restrict is not present
#   ifndef __restrict
#       define __restrict /*no-op*/
#   endif
#endif //IM_MATRIX_STACK_NO_RESTRICT

#ifndef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
typedef float  imoat;
#else
typedef double imoat;
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


// ATM these are strictly needed only in plain C (Cpp can init in ctr):
void Im_Init(void);         // Mandatory call in InitGL() (or similar)
void Im_Destroy(void);      // Mandatory call in DestroyGL() (or similar) [Currently it does nothing]


typedef int ImEnum;



#ifndef GL_MATRIX_MODE
#define GL_MATRIX_MODE          0x0BA0
#endif
#ifndef GL_MODELVIEW
#define GL_MODELVIEW            0x1700
#endif
#ifndef GL_PROJECTION
#define GL_PROJECTION           0x1701
#endif
#ifndef GL_TEXTURE
#define GL_TEXTURE				0x1702
#endif
#ifndef GL_MODELVIEW_MATRIX
#define GL_MODELVIEW_MATRIX     0x0BA6
#endif
/*#ifndef GL_MODELVIEW_STACK_DEPTH
#define GL_MODELVIEW_STACK_DEPTH    0x0BA3
#endif*/
#ifndef GL_PROJECTION_MATRIX
#define GL_PROJECTION_MATRIX        0x0BA7
#endif
/*#ifndef GL_PROJECTION_STACK_DEPTH
#define GL_PROJECTION_STACK_DEPTH   0x0BA4
#endif */
#ifndef GL_TEXTURE_MATRIX
#define GL_TEXTURE_MATRIX			0x0BA8
#endif
/*#ifndef GL_TEXTURE_STACK_DEPTH
#define GL_TEXTURE_STACK_DEPTH      0x0BA5
#endif*/

// These IM_ definitions are redundant, but this way we know what is supported and what not
typedef enum {
IM_MATRIX_MODE=GL_MATRIX_MODE,
// Matrix Modes - Start
IM_MODELVIEW=GL_MODELVIEW,
IM_PROJECTION=GL_PROJECTION,
IM_TEXTURE=GL_TEXTURE,

IM_MODELVIEW_MATRIX=GL_MODELVIEW_MATRIX,
IM_PROJECTION_MATRIX=GL_PROJECTION_MATRIX,
IM_TEXTURE_MATRIX=GL_TEXTURE_MATRIX     // Not sure if we'll support this matrix mode inside shaders....
// Matrix Modes - End
} ImEnumTypes;

// Functions that start with "im" (lowercase): [in Cpp these are available as starting with gl in scopes where MACRO_GL_IMMEDIATE_MODE_MATRIX_STACK_SCOPE is defined]
void imMatrixMode(ImEnum mode);
const imoat *imLoadIdentity(void);
const imoat *imLoadMatrix(const imoat m[16]);
const imoat *imMultMatrix(const imoat m[16]);
const imoat *imPreMultMatrix(const imoat m[16]);
void imGetImoatv(ImEnum mode,imoat m[16]);
const imoat *imGetImoatMatrixPtr(void);
const imoat *imGetImoatMatrixPtrFromMode(ImEnum mode);

const imoat* imTranslate(imoat x,imoat y,imoat z);
const imoat* imRotate(imoat degAngle,imoat x,imoat y,imoat z);
const imoat* imScale(imoat x,imoat y,imoat z);

const imoat *imPushMatrix(void);
const imoat *imPopMatrix(void);

const imoat *imuLookAt(imoat eyeX,imoat eyeY,imoat eyeZ,imoat centerX,imoat centerY,imoat centerZ,imoat upX,imoat upY,imoat upZ);
const imoat* imuLookAtYX(imoat* __restrict  cameraMatrixInOut,imoat centerX,imoat centerY,imoat centerZ,imoat minDistanceToCenterAllowed,imoat maxDistanceToCenterAllowed,int invert_xz_axis); // Keeps the camera matrix (with no scaling applied) at position (m[12],m[13],m[14]), when possible, modifies the 3x3 rotation part, converts it into a vMatrix and then returns glMultMatrix(vMatrix);
const imoat *imimLoadCameraMatrix(const imoat* __restrict cameraMatrix); // converts a camera matrix (with no scaling applied) into a vMatrix and then returns glLoadMatrix(vMatrix);
const imoat *imFrustum(imoat left,imoat right, imoat bottom, imoat top,imoat zNear, imoat zFar);
const imoat *imuPerspective(imoat fovy,imoat aspect, imoat zNear, imoat zFar);
const imoat *imuOrtho2D(imoat left,imoat right, imoat bottom, imoat top);
const imoat *imOrtho(imoat left,imoat right, imoat bottom, imoat top,imoat nearVal,imoat farVal);
const imoat *imOrtho3D(imoat cameraTargetDistance,imoat degFOV,imoat aspect,imoat znear,imoat zfar);
int imuProject(imoat objX,imoat objY,imoat objZ,const imoat* __restrict mvMatrix16,const imoat* __restrict pMatrix16,const int* viewport4,imoat* winX,imoat* winY,imoat* winZ);
int imuUnProject(imoat winX,imoat winY,imoat winZ,const imoat* __restrict  mvMatrix16,const imoat* __restrict  pMatrix16,const int* viewport4,imoat* objX,imoat* objY,imoat* objZ);
int imuUnProject4(imoat winX,imoat winY,imoat winZ,imoat clipW,const imoat* __restrict  mvMatrix16,const imoat* __restrict  pMatrix16,const int* viewport4,imoat nearVal,imoat farVal,imoat* objX,imoat* objY,imoat* objZ,imoat* objW);

void imuGetImoatNMatrixFromMvMatrix(imoat* __restrict nMatrix9Out,const imoat* __restrict mvMatrix16);

void imMatrixPrint();
void imMatrixPrintp(int width, int precision);
void imMatrixFPrint(FILE* stream);
void imMatrixFPrintp(FILE* stream,int width, int precision);


// Functions that start with "IM" (uppercase): for advanced users.
int IMInvertMatrix(imoat* __restrict mOut16,const imoat* __restrict m16);       // works with any matrix (with scaling too). Can return 0;
void IMInvertMatrix_Fast(imoat* __restrict mOut16,const imoat* __restrict m16); // works with "mOut16" modelview (or camera) matrices only, without any scaling
void IMSetMatrixRotationFromEulerYXZ(imoat* __restrict mInOut16,imoat Yaw,imoat Pitch, imoat Roll);    //  works with "mInOut16" modelview (or camera) matrices only, without any scaling; the inner 3x3 submatrix will be entirely replaced; angles in radians
int IMGetMatrixRotationEulerYXZ(const imoat* __restrict m16,imoat* Yaw,imoat* Pitch,imoat* Roll);      //  works with "m16" modelview (or camera) matrices only, without any scaling; angles in radians
void IMLookAtYX(imoat* __restrict mInOut16,imoat centerX,imoat centerY,imoat centerZ,imoat min_distance_allowed,imoat max_distance_allowed);   //  works with "mInOut16" camera matrices, without any scaling. Use IMInvertMatrix16_Fast(...) to switch from a camera to a view matrix.
static __inline void IMLookAtYX2D(imoat* __restrict mInOut16,imoat centerX,imoat centerZ,imoat min_distance_allowed,imoat max_distance_allowed)	{
    IMLookAtYX(mInOut16,centerX,mInOut16[13],centerZ,min_distance_allowed,max_distance_allowed);
}
void IMSlerpMatrix(imoat* __restrict mOut16,const imoat* __restrict mStart16,const imoat* __restrict mEnd16,imoat slerpTime_In_0_1);
void IMGetFrustumPlaneEquations(imoat planeEquationsOut[6][4],const imoat* __restrict vpMatrix16,int normalizePlanes); // ax+by+cz+d=0 [xl,xr,yb,yt,zn,zf],normalizePlanes=0 -> no normalization; vpMatrix16 = pMatrix*vMatrix
void IMGetFrustumPoints(imoat frustumPoints[8][4],const imoat* __restrict vpMatrixInverse16); //frustumPoints[?][3]=1 on return; vpMatrixInverse16 = inv(pMatrix*vMatrix) = inv(vMatrix)*inv(pMatrix) = cameraMatrix*inv(pMatrix) = IMInvertMatrix_Fast(vMatrix)*IMInvertMatrix(pMatrix); // Well, this is not correct C code, but should be clear enough
//int IMSlerpMatrixWithVelocities(imoat* __restrict mOut16,const imoat* __restrict mStart16,const imoat* __restrict mEnd16,imoat linearVelocity,imoat angularVelocity,imoat elapsedSecondsSinceLastCall); // returns 1 when done, mOut16 must be refed as mStart16 on the successive call. Last three args must be positive.

static __inline void IMSetMatrixIdentity(imoat* __restrict m16) {
    imoat* m = m16;
    m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=0.f;
    m[0]=m[5]=m[10]=m[15]=1.f;
}
static __inline void IMCopyMatrix(imoat* __restrict dst16,const imoat* __restrict src16) {
    int i;for (i=0;i<16;i++) dst16[i]=src16[i];
}
static __inline void IMMultMatrix(imoat* __restrict dst16,const imoat* __restrict mLeft16,const imoat* __restrict mRight16) {
    imoat* dst = dst16;
    const imoat* m1 = mLeft16;
    const imoat* m2 = mRight16;
    int i,j,j4;
    if (dst==m1) {
        imoat M1[16];IMCopyMatrix(M1,m1);
        IMMultMatrix(dst,M1,m2);
        return;
    }
    else if (dst==m2) {
        imoat M2[16];IMCopyMatrix(M2,m2);
        IMMultMatrix(dst,m1,M2);
        return;
    }
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            j4 = 4*j;
            dst[i+j4] =
                    m1[i]    * m2[0+j4] +
                    m1[i+4]  * m2[1+j4] +
                    m1[i+8]  * m2[2+j4] +
                    m1[i+12] * m2[3+j4];
        }
    }
}
static __inline void IMMatrixInvertXZAxisInPlace(imoat* __restrict mInOut16) {
    imoat* m = mInOut16;
    m[0]=-m[0]; m[8] =-m[8];
    m[1]=-m[1]; m[9] =-m[9];
    m[2]=-m[2]; m[10]=-m[10];

    /*m[0]=-m[0]; m[2]=-m[2];
    m[4]=-m[4]; m[6]=-m[6];
    m[8]=-m[8]; m[10]=-m[10];*/
}
static __inline void IMMatrixInvertXZAxis(imoat* __restrict mOut16,const imoat* __restrict m16) {
    const imoat* m = m16;
    imoat* n = mOut16;
    IMCopyMatrix(n,m);
    IMMatrixInvertXZAxisInPlace(n);
}
static __inline void IMMatrixMulDir16(const imoat* __restrict m16,imoat* __restrict dirOut3,const imoat dirX,imoat dirY,imoat dirZ) {
    //imoat w;
    dirOut3[0] = dirX*m16[0] + dirY*m16[4] + dirZ*m16[8];
    dirOut3[1] = dirX*m16[1] + dirY*m16[5] + dirZ*m16[9];
    dirOut3[2] = dirX*m16[2] + dirY*m16[6] + dirZ*m16[10];
    //w          = dirX*m16[3] + dirY*m16[7] + dirZ*m16[11]; // + m[15] ?
    //if (w!=0 && w!=1) {dirOut3[0]/=w;dirOut3[1]/=w;dirOut3[2]/=w;}
}
static __inline void IMMatrixMulPos16(const imoat* __restrict m16,imoat* __restrict posOut3,const imoat posX,imoat posY,imoat posZ) {
    imoat w;
    posOut3[0] = posX*m16[0] + posY*m16[4] + posZ*m16[8] + m16[12];
    posOut3[1] = posX*m16[1] + posY*m16[5] + posZ*m16[9] + m16[13];
    posOut3[2] = posX*m16[2] + posY*m16[6] + posZ*m16[10]+ m16[14];
    w          = posX*m16[3] + posY*m16[7] + posZ*m16[11]+ m16[15];
    if (w!=0 && w!=1) {posOut3[0]/=w;posOut3[1]/=w;posOut3[2]/=w;}
}

static __inline void IMSetMatrixIdentity9(imoat* __restrict m9) {
    imoat* m = m9;
    m[1]=m[2]=m[3]=m[5]=m[6]=m[7]=0.f;
    m[0]=m[4]=m[8]=1.f;
}
static __inline void IMCopyMatrix9(imoat* __restrict dst9,const imoat* __restrict src9) {
    int i;for (i=0;i<16;i++) dst9[i]=src9[i];
}
static __inline void IMMultMatrix9(imoat* __restrict dst9,const imoat* __restrict mLeft9,const imoat* __restrict mRight9) {
    imoat* dst = dst9;
    const imoat* m1 = mLeft9;
    const imoat* m2 = mRight9;
    int i,j,j3;
    if (dst==m1) {
        imoat M1[9];IMCopyMatrix9(M1,m1);
        IMMultMatrix9(dst,M1,m2);
        return;
    }
    else if (dst==m2) {
        imoat M2[9];IMCopyMatrix9(M2,m2);
        IMMultMatrix9(dst,m1,M2);
        return;
    }
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            j3 = 3*j;
            dst[i+j3] =
                    m1[i]    * m2[0+j3] +
                    m1[i+3]  * m2[1+j3] +
                    m1[i+6]  * m2[2+j3];
        }
    }
}

static __inline void IMSetQuaternionIdentity(imoat* __restrict q4) {
    imoat* q = q4;q[0]=q[1]=q[2]=0;q[3]=1;
}
void IMCopyQuaternion(imoat* __restrict dst4,const imoat* __restrict src4);
void IMGetQuaternionFromMatrixRotation(imoat* __restrict q4,const imoat* __restrict m16);
void IMSetMatrixRotationFromQuaternion(imoat* __restrict mInOut16,const imoat* __restrict q4);
void IMSlerpQuaternion(imoat* __restrict qOut4,const imoat* __restrict qStart4,const imoat* __restrict qEnd4,imoat t);
//int IMSlerpQuaternionWithAngularVelocity(imoat* __restrict qOut4,const imoat* __restrict qStart4,const imoat* __restrict qEnd4,imoat angularVelocity,imoat elapsedSecondsSinceLastCall); // returns 1 when done, qOut4 must be refed as qStart4 on the successive call. Last two args must be positive.

static __inline void IMPlaneNormalize(imoat* __restrict p4) {
    int i;imoat len = p4[0]*p4[0]+p4[1]*p4[1]+p4[2]*p4[2];
    if (len!=0) {
        len = sqrt(len);
        for (i=0;i<4;i++) p4[i]/=len;
    }
}
static __inline imoat IMPlanePointDistance(const imoat* __restrict plane4,const imoat* __restrict pt3) {
    imoat rv = plane4[3];
    int i;for (i=0;i<3;i++) rv+=plane4[i]*pt3[i];
    return rv;
}

static __inline imoat IMVector3Dot(const imoat v0[3],const imoat v1[3]) {
    return v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
}
static __inline void IMVector3Normalize(imoat v[3]) {
    imoat len = IMVector3Dot(v,v);int i;
    if (len!=0) {
        len = sqrt(len);
        for (i=0;i<3;i++) v[i]/=len;
    }
}
static __inline void IMVector3Cross(imoat rv[3],const imoat a[3],const imoat b[3]) {
    rv[0] =	a[1] * b[2] - a[2] * b[1];
    rv[1] =	a[2] * b[0] - a[0] * b[2];
    rv[2] =	a[0] * b[1] - a[1] * b[0];
}


int IMIsVisible(const imoat frustumPlanes[6][4],const imoat*__restrict mfMatrix16,imoat aabbMinX,imoat aabbMinY,imoat aabbMinZ,imoat aabbMaxX,imoat aabbMaxY,imoat aabbMaxZ);

static __inline void IMConvertMatrixd2f16(float* __restrict result16,const double* __restrict m16) {int i;for(i = 0; i < 16; i++) result16[i]=(float)m16[i];}
static __inline void IMConvertMatrixf2d16(double* __restrict result16,const float* __restrict m16) {int i;for(i = 0; i < 16; i++) result16[i]=(double)m16[i];}
static __inline void IMConvertMatrixd2f9(float* __restrict result9,const double* __restrict m9) {int i;for(i = 0; i < 9; i++) result9[i]=(float)m9[i];}
static __inline void IMConvertMatrixf2d9(double* __restrict result9,const float* __restrict m9) {int i;for(i = 0; i < 9; i++) result9[i]=(double)m9[i];}
static __inline void IMConvertQuaterniond2f(float* __restrict result4,const double* __restrict q4) {int i;for(i = 0; i < 4; i++) result4[i]=(float)q4[i];}
static __inline void IMConvertQuaternionf2d(double* __restrict result4,const float* __restrict q4) {int i;for(i = 0; i < 4; i++) result4[i]=(double)q4[i];}


static __inline const imoat *imLoadMatrixf(const float m[16]) {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    double md[16];IMConvertMatrixf2d16(md,m);return imLoadMatrix(md);
#else
    return imLoadMatrix(m);
#endif
}
static __inline const imoat *imLoadMatrixd(const double m[16]) {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    return imLoadMatrix(m);
#else
    float mf[16];IMConvertMatrixd2f16(mf,m);return imLoadMatrix(mf);
#endif
}
static __inline const imoat *imMultMatrixf(const float m[16])   {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    double md[16];IMConvertMatrixf2d16(md,m);return imMultMatrix(md);
#else
    return imMultMatrix(m);
#endif
}
static __inline const imoat *imMultMatrixd(const double m[16])   {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    return imMultMatrix(m);
#else
    float mf[16];IMConvertMatrixd2f16(mf,m);return imMultMatrix(mf);
#endif
}
static __inline const imoat *imPreMultMatrixf(const float m[16]) {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    double md[16];IMConvertMatrixf2d16(md,m);return imPreMultMatrix(md);
#else
    return imPreMultMatrix(m);
#endif
}
static __inline const imoat *imPreMultMatrixd(const double m[16]) {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    return imPreMultMatrix(m);
#else
    float mf[16];IMConvertMatrixd2f16(mf,m);return imPreMultMatrix(mf);
#endif
}
static __inline const imoat* imTranslatef(float x,float y,float z) {return imTranslate(x,y,z);}
static __inline const imoat* imTranslated(double x,float y,float z) {return imTranslate(x,y,z);}
static __inline const imoat* imRotatef(float degAngle,float x,float y,float z) {return imRotate(degAngle,x,y,z);}
static __inline const imoat* imRotated(double degAngle,double x,double y,double z) {return imRotate(degAngle,x,y,z);}
static __inline const imoat* imScalef(float x,float y,float z) {return imScale(x,y,z);}
static __inline const imoat* imScaled(double x,double y,double z) {return imScale(x,y,z);}
static __inline void imGetFloatv(ImEnum mode,float m[16]) {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    const double* md=imGetImoatMatrixPtrFromMode(mode);IMConvertMatrixd2f16(m,md);
#else
    imGetImoatv(mode,m);
#endif
}
static __inline void imGetDoublev(ImEnum mode,double m[16]) {
#ifdef IM_MATRIX_STACK_USE_DOUBLE_PRECISION
    imGetImoatv(mode,m);
#else
    const float* mf=imGetImoatMatrixPtrFromMode(mode);IMConvertMatrixf2d16(m,mf);
#endif
}


#ifdef __cplusplus
}

namespace immediate_mode {
inline void glMatrixMode(ImEnum mode) {imMatrixMode(mode);}
inline const imoat *glLoadIdentity(void) {return imLoadIdentity();}
inline const imoat *glLoadMatrix(const imoat m[16]) {return imLoadMatrix(m);}
inline const imoat *glMultMatrix(const imoat m[16]) {return imMultMatrix(m);}
inline const imoat *glPreMultMatrix(const imoat m[16]) {return imPreMultMatrix(m);}
inline void glGetImoatv(ImEnum mode,imoat m[16]) {return imGetImoatv(mode,m);}
inline const imoat *glGetImoatMatrixPtr(void) {return imGetImoatMatrixPtr();}
inline const imoat *glGetImoatMatrixPtrFromMode(ImEnum mode) {return imGetImoatMatrixPtrFromMode(mode);}

inline const imoat* glTranslate(imoat x,imoat y,imoat z) {return imTranslate(x,y,z);}
inline const imoat* glRotate(imoat degAngle,imoat x,imoat y,imoat z) {return imRotate(degAngle,x,y,z);}
inline const imoat* glScale(imoat x,imoat y,imoat z) {return imScale(x,y,z);}

inline const imoat *glPushMatrix(void) {return imPushMatrix();}
inline const imoat *glPopMatrix(void) {return imPopMatrix();}

inline const imoat *gluLookAt(imoat eyeX,imoat eyeY,imoat eyeZ,imoat centerX,imoat centerY,imoat centerZ,imoat upX,imoat upY,imoat upZ) {return imuLookAt(eyeX,eyeY,eyeZ,centerX,centerY,centerZ,upX,upY,upZ);}
inline const imoat *gluLookAtYX(imoat* __restrict  cameraMatrixInOut,imoat centerX,imoat centerY,imoat centerZ,imoat minDistanceToCenterAllowed,imoat maxDistanceToCenterAllowed,int invert_xz_axis) {return imuLookAtYX(cameraMatrixInOut,centerX,centerY,centerZ,minDistanceToCenterAllowed,maxDistanceToCenterAllowed,invert_xz_axis);}
inline const imoat *glimLoadCameraMatrix(const imoat* __restrict cameraMatrix16) {return imimLoadCameraMatrix(cameraMatrix16);}
inline const imoat *glFrustum(imoat left,imoat right, imoat bottom, imoat top,imoat zNear, imoat zFar) {return imFrustum(left,right,bottom,top,zNear,zFar);}
inline const imoat *gluPerspective(imoat fovy,imoat aspect, imoat zNear, imoat zFar) {return imuPerspective(fovy,aspect,zNear,zFar);}
inline const imoat *gluOrtho2D(imoat left,imoat right, imoat bottom, imoat top) {return imuOrtho2D(left,right,bottom,top);}
inline const imoat *glOrtho(imoat left,imoat right, imoat bottom, imoat top,imoat nearVal,imoat farVal) {return imOrtho(left,right,bottom,top,nearVal,farVal);}
inline const imoat *glOrtho3D(imoat cameraTargetDistance,imoat degFOV,imoat aspect,imoat znear,imoat zfar) {return imOrtho3D(cameraTargetDistance,degFOV,aspect,znear,zfar);}
inline int gluProject(imoat objX,imoat objY,imoat objZ,const imoat* __restrict mvMatrix16,const imoat* __restrict pMatrix16,const int* viewport4,imoat* winX,imoat* winY,imoat* winZ) {
    return imuProject(objX,objY,objZ,mvMatrix16,pMatrix16,viewport4,winX,winY,winZ);
}
inline int gluUnProject(imoat winX,imoat winY,imoat winZ,const imoat* __restrict mvMatrix16,const imoat* __restrict pMatrix16,const int* viewport4,imoat* objX,imoat* objY,imoat* objZ) {
    return imuUnProject(winX,winY,winZ,mvMatrix16,pMatrix16,viewport4,objX,objY,objZ);
}
inline int gluUnProject4(imoat winX,imoat winY,imoat winZ,imoat clipW,const imoat* __restrict mvMatrix16,const imoat* __restrict pMatrix16,const int* viewport4,imoat nearVal,imoat farVal,imoat* objX,imoat* objY,imoat* objZ,imoat* objW) {
    return imuUnProject4(winX,winY,winZ,clipW,mvMatrix16,pMatrix16,viewport4,nearVal,farVal,objX,objY,objZ,objW);
}

inline void gluGetImoatNMatrixFromMvMatrix(imoat* __restrict nMatrix9Out,const imoat* __restrict mvMatrix16) {imuGetImoatNMatrixFromMvMatrix(nMatrix9Out,mvMatrix16);}

inline const imoat *glLoadMatrixf(const float m[16]) {return imLoadMatrixf(m);}
inline const imoat *glLoadMatrixd(const double m[16]) {return imLoadMatrixd(m);}
inline const imoat *glMultMatrixf(const float m[16]) {return imMultMatrixf(m);}
inline const imoat *glMultMatrixd(const double m[16]) {return imMultMatrixd(m);}
inline const imoat *glPreMultMatrixf(const float m[16]) {return imPreMultMatrixf(m);}
inline const imoat *glPreMultMatrixd(const double m[16]) {return imPreMultMatrixd(m);}
inline const imoat* glTranslatef(float x,float y,float z) {return imTranslatef(x,y,z);}
inline const imoat* glTranslated(double x,float y,float z) {return imTranslated(x,y,z);}
inline const imoat* glRotatef(float degAngle,float x,float y,float z) {return imRotatef(degAngle,x,y,z);}
inline const imoat* glRotated(double degAngle,double x,double y,double z) {return imRotated(degAngle,x,y,z);}
inline const imoat* glScalef(float x,float y,float z) {return imScalef(x,y,z);}
inline const imoat* glScaled(double x,double y,double z) {return imScaled(x,y,z);}
inline void glGetFloatv(ImEnum mode,float m[16]) {imGetFloatv(mode,m);}
inline void glGetDoublev(ImEnum mode,double m[16]) {imGetDoublev(mode,m);}

inline void glMatrixPrint() {imMatrixPrint();}
inline void glMatrixPrintp(int width, int precision) {imMatrixPrintp(width,precision);}
inline void glMatrixFPrint(FILE* stream)    {imMatrixFPrint(stream);}
inline void glMatrixFPrintp(FILE* stream,int width, int precision) {imMatrixFPrintp(stream,width,precision);}

} // namespace immediate_mode

#define MACRO_GL_IMMEDIATE_MODE_MATRIX_STACK_SCOPE   using namespace immediate_mode;   \
                                        using immediate_mode::glMatrixMode;     \
                                        using immediate_mode::glLoadIdentity;     \
                                        using immediate_mode::glLoadMatrix;     \
                                        using immediate_mode::glMultMatrix;     \
                                        using immediate_mode::glPreMultMatrix;     \
                                        using immediate_mode::glGetImoatv;     \
                                        using immediate_mode::glGetImoatMatrixPtr;     \
                                        using imeediate_mode::glGetImoatMatrixPtrFromMode; \
                                        using immediate_mode::glTranslatef;     \
                                        using immediate_mode::glRotatef;     \
                                        using immediate_mode::glScalef;     \
                                        using immediate_mode::glPushMatrix;     \
                                        using immediate_mode::glPopMatrix;     \
                                        using immediate_mode::gluLookAt;     \
                                        using immediate_mode::gluLookAtYX;     \
                                        using immediate_mode::glimLoadCameraMatrix;     \
                                        using immediate_mode::glFrustum;     \
                                        using immediate_mode::gluPerspective;     \
                                        using immediate_mode::gluOrtho2D;    \
                                        using immediate_mode::glOrtho;         \
                                        using immediate_mode::glOrtho3D;         \
                                        using immediate_mode::gluProject;         \
                                        using immediate_mode::gluUnProject;         \
                                        using immediate_mode::gluUnProject4;    \
                                        using immediate_mode::glMatrixPrint;    \
                                        using immediate_mode::glMatrixPrintp;         \
                                        using immediate_mode::glMatrixFPrint;         \
                                        using immediate_mode::glMatrixFPrintp;        \
                                        using immediate_mode::gluGetImoatNMatrixFromMvMatrix;       \
                                        using immediate_mode::glLoadMatrixf;     \
                                        using immediate_mode::glLoadMatrixd;     \
                                        using immediate_mode::glMultMatrixf;     \
                                        using immediate_mode::glMultMatrixd;     \
                                        using immediate_mode::glPreMultMatrixf;     \
                                        using immediate_mode::glPreMultMatrixd;     \
                                        using immediate_mode::glTranslatef;     \
                                        using immediate_mode::glTranslated;     \
                                        using immediate_mode::glRotatef;     \
                                        using immediate_mode::glRotated;     \
                                        using immediate_mode::glScalef;     \
                                        using immediate_mode::glScaled;     \
                                        using immediate_mode::glGetFloatv;     \
                                        using immediate_mode::glGetDoublev;

#endif //__cplusplus

#endif //IM_MATRIX_STACK_H

#ifdef IM_MATRIX_STACK_IMPLEMENTATION

#ifndef IM_MATRIX_STACK_IMPLEMENTATION_H    // Additional guard
#define IM_MATRIX_STACK_IMPLEMENTATION_H


#include <math.h>
#include <stdio.h>  // fprint

#ifdef __cplusplus
extern "C"	{
#endif


#define IM_MATRIX_STACK_NUM_MATRIX_MODES 3

#ifndef IM_MATRIX_STACK_MODELVIEW_DEPTH
#   define IM_MATRIX_STACK_MODELVIEW_DEPTH 16
#endif
#ifndef IM_MATRIX_STACK_PROJECTION_DEPTH
#   define IM_MATRIX_STACK_PROJECTION_DEPTH 8
#endif
#ifndef IM_MATRIX_STACK_TEXTURE_DEPTH
#   define IM_MATRIX_STACK_TEXTURE_DEPTH 4
#endif

#define IM_MATRIX_STACK_MAX_DEPTH IM_MATRIX_STACK_MODELVIEW_DEPTH
#if (IM_MATRIX_STACK_PROJECTION_DEPTH>IM_MATRIX_STACK_MAX_DEPTH)
#undef IM_MATRIX_STACK_MAX_DEPTH
#define IM_MATRIX_STACK_MAX_DEPTH IM_MATRIX_STACK_PROJECTION_DEPTH
#endif
#if (IM_MATRIX_STACK_TEXTURE_DEPTH>IM_MATRIX_STACK_MAX_DEPTH)
#undef IM_MATRIX_STACK_MAX_DEPTH
#define IM_MATRIX_STACK_MAX_DEPTH IM_MATRIX_STACK_TEXTURE_DEPTH
#endif

#ifdef __cplusplus
struct Im_Matrix_Stack_Internal
#else
typedef struct
#endif
{
imoat* matrixStacks[IM_MATRIX_STACK_NUM_MATRIX_MODES][IM_MATRIX_STACK_MAX_DEPTH];
int matrixStackDepths[IM_MATRIX_STACK_NUM_MATRIX_MODES];

imoat matrixStackModelview[IM_MATRIX_STACK_MODELVIEW_DEPTH][16];
imoat matrixStackProjection[IM_MATRIX_STACK_PROJECTION_DEPTH][16];
imoat matrixStackTexture[IM_MATRIX_STACK_TEXTURE_DEPTH][16];
int matrixStackIndices[IM_MATRIX_STACK_NUM_MATRIX_MODES];
int matrixMode; // -IM_MODELVIEW (starts at zero)

#ifdef __cplusplus
Im_Matrix_Stack_Internal();
#endif
}
#ifndef __cplusplus
Im_Matrix_Stack_Internal
#endif
;

static void Im_Internal_Reset(Im_Matrix_Stack_Internal* im) {
    int i,j,k,depth;

    im->matrixStackDepths[0] = IM_MATRIX_STACK_MODELVIEW_DEPTH;
    im->matrixStackDepths[1] = IM_MATRIX_STACK_PROJECTION_DEPTH;
    im->matrixStackDepths[2] = IM_MATRIX_STACK_TEXTURE_DEPTH;

    im->matrixMode = 0;
    for (k=0;k<IM_MATRIX_STACK_NUM_MATRIX_MODES;k++) {
        im->matrixStackIndices[k]=0;
        depth = im->matrixStackDepths[k];
        for (j=0;j<depth;j++)
        {
            if (k==0)       im->matrixStacks[k][j] = &im->matrixStackModelview[j][0];
            else if (k==1)  im->matrixStacks[k][j] = &im->matrixStackProjection[j][0];
            else if (k==2)  im->matrixStacks[k][j] = &im->matrixStackTexture[j][0];
            else continue;
            for (i=0;i<16;i++) {im->matrixStacks[k][j][i]=0.f;}
            im->matrixStacks[k][j][0]=im->matrixStacks[k][j][5]=im->matrixStacks[k][j][10]=im->matrixStacks[k][j][15]=1.f;
        }
    }
}
static Im_Matrix_Stack_Internal IMMS;
#ifdef __cplusplus
Im_Matrix_Stack_Internal::Im_Matrix_Stack_Internal() {Im_Internal_Reset(this);}
#endif


int IMInvertMatrix(imoat* __restrict mOut16,const imoat* __restrict m16) {
    const imoat* m = m16;
    imoat* n = mOut16;

    imoat m00 = m[0],  m10 = m[1],  m20 = m[2],  m30 = m[3];
    imoat m01 = m[4],  m11 = m[5],  m21 = m[6],  m31 = m[7];
    imoat m02 = m[8],  m12 = m[9],  m22 = m[10], m32 = m[11];
    imoat m03 = m[12], m13 = m[13], m23 = m[14], m33 = m[15];

    imoat v0 = m20 * m31 - m21 * m30;
    imoat v1 = m20 * m32 - m22 * m30;
    imoat v2 = m20 * m33 - m23 * m30;
    imoat v3 = m21 * m32 - m22 * m31;
    imoat v4 = m21 * m33 - m23 * m31;
    imoat v5 = m22 * m33 - m23 * m32;

    imoat t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    imoat t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    imoat t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    imoat t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    imoat det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
    if (det==0) return 0;
    {
        imoat invDet = 1 / det;

        imoat d00 = t00 * invDet;
        imoat d10 = t10 * invDet;
        imoat d20 = t20 * invDet;
        imoat d30 = t30 * invDet;

        imoat d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        imoat d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        imoat d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        imoat d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;
        {
            imoat d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            imoat d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            imoat d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            imoat d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            v0 = m21 * m10 - m20 * m11;
            v1 = m22 * m10 - m20 * m12;
            v2 = m23 * m10 - m20 * m13;
            v3 = m22 * m11 - m21 * m12;
            v4 = m23 * m11 - m21 * m13;
            v5 = m23 * m12 - m22 * m13;
            {
                imoat d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
                imoat d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
                imoat d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
                imoat d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

                n[0] =d00; n[1] =d10; n[2] =d20; n[3] =d30;
                n[4] =d01; n[5] =d11; n[6] =d21; n[7] =d31;
                n[8] =d02; n[9] =d12; n[10]=d22; n[11]=d32;
                n[12]=d03; n[13]=d13; n[14]=d23; n[15]=d33;
            }
        }
    }
    return 1;
}
void IMInvertMatrix_Fast(imoat* __restrict mOut16,const imoat* __restrict m16)	{
    // It works only for translation + rotation, and only
    // when rotation can be represented by an unit quaternion
    // scaling is discarded
    const imoat* m = m16;
    imoat* n = mOut16;
    const imoat T[3] = {-m[12],-m[13],-m[14]};
    imoat w;
    // Step 1. Transpose the 3x3 submatrix
    /* 0   4   8   12
       1   5   9   13
       2   6   10  14 */
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step2. Adjust translation
    /*n[12]=T[0]*n[0] + T[1]*n[1] + T[2]*n[2];
    n[13]=T[0]*n[4] + T[1]*n[5] + T[2]*n[6];
    n[14]=T[0]*n[8] + T[1]*n[9] + T[2]*n[10];*/
    //w    =T[0]*n[12]+ T[1]*n[13]+ T[2]*n[14];
    n[12]=T[0]*n[0] + T[1]*n[4] +T[2]*n[8];
    n[13]=T[0]*n[1] + T[1]*n[5] +T[2]*n[9];
    n[14]=T[0]*n[2] + T[1]*n[6] +T[2]*n[10];
    w    =T[0]*n[3] + T[1]*n[7] +T[2]*n[11];
    if (w!=0 && w!=1) {n[12]/=w;n[13]/=w;n[14]/=w;} // These last 2 lines are not strictly necessary AFAIK
}
void IMSetMatrixRotationFromEulerYXZ(imoat* __restrict mInOut16,imoat Yaw,imoat Pitch, imoat Roll) {
    imoat* m = mInOut16;

    const imoat fCosY=cos(Yaw);const imoat fSinY=sin(Yaw);
    //const imoat kYMat[9] = {fCosY,0,fSinY,  0,1,0,  -fSinY,0,fCosY};  // To transpose all these matrices we just need to invert th sin
    const imoat kYMat[9] = {fCosY,0,-fSinY,  0,1,0,  fSinY,0,fCosY};

    const imoat fCosP=cos(Pitch);const imoat fSinP=sin(Pitch);
    //const imoat kXMat[9] = {1,0,0,  0,fCosP,-fSinP, 0,fSinP,fCosP};
    const imoat kXMat[9] = {1,0,0,0,fCosP,fSinP,0,-fSinP,fCosP};

    const imoat fCosR=cos(Roll);const imoat fSinR=sin(Roll);
    //const imoat kZMat[9] = {fCosR,-fSinR,0, fSinR,fCosR,0,  0,0,1};
    const imoat kZMat[9] = {fCosR,fSinR,0, -fSinR,fCosR,0,  0,0,1};

    imoat n[18];

    // m3_mul(kYMat,m3_mul(kXMat,kZMat));
    IMMultMatrix9(&n[9],kXMat,kZMat);
    IMMultMatrix9(n,kYMat,&n[9]);

    //IMMultMatrix9(&n[9],kYMat,kXMat);
    //IMMultMatrix9(n,&n[9],kZMat);

    // Map n back to m
    m[0]=n[0];  m[4]=n[3];  m[8] =n[6];
    m[1]=n[1];  m[5]=n[4];  m[9] =n[7];
    m[2]=n[2];  m[6]=n[5];  m[10]=n[8];
}
int IMGetMatrixRotationEulerYXZ(const imoat* __restrict m16,imoat* Yaw,imoat* Pitch,imoat* Roll) {
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy
    const imoat* m = m16;
    imoat m00=m[0], m01=m[4], m02=m[8], m10=m[1], m11=m[5], m12=m[9], m22=m[10];


    *Pitch = asin(-m12);
    if ( *Pitch < M_HALF_PI )
    {
        if ( *Pitch > -M_HALF_PI )
        {
            *Yaw = atan2(m02,m22);
            *Roll = atan2(m10,m11);
            return 1;
        }
        else
        {
            // WARNING.  Not a unique solution.
            imoat fRmY = atan2(-m01,m00);
            *Roll = 0;  // any angle works
            *Yaw = *Roll - fRmY;
            return 0;
        }
    }
    else
    {
        // WARNING.  Not a unique solution.
        imoat fRpY = atan2(-m01,m00);
        *Roll = 0;  // any angle works
        *Yaw = fRpY - *Roll;
        return 0;
    }
}
/**
 * Makes "mInOut16" look at "to", using the YXZ axis order.
 * It must be "mInOut16"!=NULL,"min_distance_allowed">=0,"max_distance_allowed">0
 * "mInOut16" must have no scaling applied (its mat3 will be completely re-set)
 * if min_distance_allowed==max_distance_allowed no distance is kept.
 * Note that "mInOut16" can't be used as a camera view matrix: but we can use Im_InvertMatrix16_Fast(...) for it.
*/
void IMLookAtYX(imoat* __restrict mInOut16,imoat centerX,imoat centerY,imoat centerZ,imoat min_distance_allowed,imoat max_distance_allowed)	{
    imoat* m = mInOut16;
    imoat D[3];imoat Dxz2,Dxz,AY,AX;
    imoat T[3] = {m[12],m[13],m[14]};// translation part
    D[0]=centerX-T[0];D[1]=centerY-T[1];D[2]=centerZ-T[2];
    Dxz2 = D[0]*D[0]+D[2]*D[2];
    Dxz = sqrt(Dxz2);
    AY = atan2(D[0],D[2]);
    AX = -atan2(D[1],Dxz);
    IMSetMatrixRotationFromEulerYXZ(m,AY,AX,0);
    if (min_distance_allowed<0) min_distance_allowed=0;
    if (max_distance_allowed<0) max_distance_allowed=0;
    if (min_distance_allowed<max_distance_allowed)	{
        const imoat distance=sqrt(Dxz2+D[1]*D[1]);
        const imoat zAxis[3] = {m[8],m[9],m[10]};    //zAxis
        if (distance<min_distance_allowed)  {
            //m4_set_translation(matrix,v3_add(T,v3_muls(zAxis,distance-min_distance_allowed)));
            const imoat distanceDelta = distance-min_distance_allowed;
            m[12] = T[0] + zAxis[0]*distanceDelta;
            m[13] = T[1] + zAxis[1]*distanceDelta;
            m[14] = T[2] + zAxis[2]*distanceDelta;
        }
        else if (distance>max_distance_allowed) {
            //m4_set_translation(matrix,v3_add(T,v3_muls(zAxis,distance-max_distance_allowed)));
            const imoat distanceDelta = distance-max_distance_allowed;
            m[12] = T[0] + zAxis[0]*distanceDelta;
            m[13] = T[1] + zAxis[1]*distanceDelta;
            m[14] = T[2] + zAxis[2]*distanceDelta;
        }
    }
}

void IMCopyQuaternion(imoat* __restrict dst4,const imoat* __restrict src4) {int i;for (i=0;i<4;i++) dst4[i]=src4[i];}
void IMGetQuaternionFromMatrixRotation(imoat* __restrict q4,const imoat* __restrict m16)   {
    const imoat* m = m16;
    imoat* q = q4;
    imoat m00=m[0], m10=m[1], m20=m[2];
    imoat m01=m[4], m11=m[5], m21=m[6];
    imoat m02=m[8], m12=m[9], m22=m[10];
    /*imoat m00=m[0], m10=m[4], m20=m[8];
    imoat m01=m[1], m11=m[5], m21=m[9];
    imoat m02=m[2], m12=m[6], m22=m[10]; */
    imoat trace = m00 + m11 + m22;
    if (trace > 0.f) {
        imoat s = sqrt(trace + 1.0f);
        q[3]=(s * 0.5f);s = 0.5f / s;
        q[0]=((m12 - m21) * s);
        q[1]=((m20 - m02) * s);
        q[2]=((m01 - m10) * s);
    }
    else {
        // i,j,k are all 0,1 or 2
        int i = m00 < m11 ?
            (m11 < m22 ? 2 : 1) :
            (m00 < m22 ? 2 : 0);
        int j = (i + 1) % 3;
        int k = (i + 2) % 3;
        /*imoat s = sqrt(m[i][i] - m[j][j] - m[k][k] + 1.0f);
        q[i] = s * 0.5f;s = 0.5f / s;
        q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;*/
        // >>> m[i][j] = m[j*4+i] because m[2][1] = m21 = m[6]
        int i4 = 4*i, j4 = 4*j, k4 = 4*k;
        imoat s = sqrt(m[i4+i] - m[j4+j] - m[k4+k] + 1.0f);
        q[i] = s * 0.5f;s = 0.5f / s;
        q[3] = (m[k4+j] - m[j4+k]) * s;
        q[j] = (m[j4+i] + m[i4+j]) * s;
        q[k] = (m[k4+i] + m[i4+k]) * s;
        /*imoat s = sqrt(m[i4+i] - m[j4+j] - m[k4+k] + 1.0f);
        q[i] = s * 0.5f;s = 0.5f / s;
        q[3] = (m[k+j4] - m[j+k4]) * s;
        q[j] = (m[j+i4] + m[i+j4]) * s;
        q[k] = (m[k+i4] + m[i+k4]) * s;*/
    }
}
void IMSetMatrixRotationFromQuaternion(imoat* __restrict mInOut16,const imoat* __restrict q4) {
    imoat* m = mInOut16;
    const imoat* q = q4;
    imoat d = q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
    if (d == 0.0f) {m[0]=m[5]=m[10]=1;m[1]=m[2]=m[4]=m[6]=m[8]=m[9]=0;return;}
    {
    imoat s = 2.0f / d;
    imoat xs = q[0] * s,   ys = q[1] * s,   zs = q[2] * s;
    imoat wx = q[3] * xs,  wy = q[3] * ys,  wz = q[3] * zs;
    imoat xx = q[0] * xs,  xy = q[0] * ys,  xz = q[0] * zs;
    imoat yy = q[1] * ys,  yz = q[1] * zs,  zz = q[2] * zs;

    m[0] = 1.0f - (yy + zz); m[1] = xy - wz;           m[2] = xz + wy;
    m[4] = xy + wz;          m[5] = 1.0f - (xx + zz);  m[6] = yz - wx;
    m[8] = xz - wy;          m[9] = yz + wx;           m[10] = 1.0f - (xx + yy);

    /*m[0] = 1.0f - (yy + zz); m[4] = xy - wz;           m[8] = xz + wy;
    m[1] = xy + wz;          m[5] = 1.0f - (xx + zz);  m[9] = yz - wx;
    m[2] = xz - wy;          m[6] = yz + wx;           m[10] = 1.0f - (xx + yy); */
    }
}
void IMSlerpQuaternion(imoat* __restrict qOut4,const imoat* __restrict qStart4,const imoat* __restrict qEnd4,imoat t)   {
    if (qOut4==qStart4) {
        imoat qs[4];if (qOut4==qEnd4) return;IMCopyQuaternion(qs,qOut4);
        IMSlerpQuaternion(qOut4,qs,qEnd4,t);
        return;
    }
    if (qOut4==qEnd4) {
        imoat qe[4];IMCopyQuaternion(qe,qOut4);
        IMSlerpQuaternion(qOut4,qStart4,qe,t);
        return;
    }

    {
    const int normalizeQOutAfterLerp = 1;            // When using Lerp instead of Slerp qOut should be normalized. However some users prefer setting eps small enough so that they can leave the Lerp as it is.
    const imoat eps=0.0001f;                      	 // In [0 = 100% Slerp,1 = 100% Lerp] Faster but less precise with bigger epsilon (Lerp is used instead of Slerp more often). Users should tune it to achieve a performance boost.
    const int useAcosAndSinInsteadOfAtan2AndSqrt = 0;// Another possible minimal Speed vs Precision tweak (I suggest just changing it here and not in the caller code)

    const imoat* qStart = qStart4;
    imoat qEnd[4] = {qEnd4[0],qEnd4[1],qEnd4[2],qEnd4[3]};
    imoat* qOut = qOut4;
    imoat fCos;int i;
    fCos = qStart[0] * qEnd[0] + qStart[1] * qEnd[1] + qStart[2] * qEnd[2] + qStart[3] * qEnd[3];

    // Code based on Ogre::Quaternion (MIT licensed). Hardcoded "shortestPath=true" and simplified code.
    // Do we need to invert rotation?
    if(fCos < 0)	//Originally it was if(fCos < static_cast < Real >(0.0) && shortestPath)
    {
        fCos = -fCos;
        //qEnd[0] = -qEnd[0];qEnd[1] = -qEnd[1];qEnd[2] = -qEnd[2];qEnd[3] = -qEnd[3];
        for (i=0;i<4;i++) qEnd[i] = -qEnd[i];       // is this faster (auto-vectorization) ?
    }

    if( fCos < 1.f - eps)	// Originally if was "Ogre::Math::Abs(fCos)" instead of "fCos", but we know fCos>0, because we have hard coded shortestPath=true
    {
        // Standard case (slerp)
        imoat fSin,fAngle;
        if (!useAcosAndSinInsteadOfAtan2AndSqrt)	{
            // Ogre::Quaternion uses this branch by default
            fSin = sqrt(1.f - fCos*fCos);
            fAngle = atan2(fSin, fCos);
        }
        else	{
            // Possible replacement of the two lines above
            // (it's hard to tell if they're faster, but my instinct tells me I should trust atan2 better than acos (geometry geeks needed here...)):
            // But probably sin(...) is faster than (sqrt + 1 subtraction and mult)
            fAngle = acos(fCos);
            fSin = sin(fAngle);
        }

        {
        const imoat fInvSin = 1.f / fSin;
        const imoat fCoeff0 = sin((1.f - t) * fAngle) * fInvSin;
        const imoat fCoeff1 = sin(t * fAngle) * fInvSin;

        //qOut =  fCoeff0 * qStart + fCoeff1 * qEnd; //Avoided for maximum portability and conversion of the code
        //qOut[0] = (fCoeff0 * qStart[0] + fCoeff1 * qEnd[0]);
        //qOut[1] = (fCoeff0 * qStart[1] + fCoeff1 * qEnd[1]);
        //qOut[2] = (fCoeff0 * qStart[2] + fCoeff1 * qEnd[2]);
        //qOut[3] = (fCoeff0 * qStart[3] + fCoeff1 * qEnd[3]);
        for (i=0;i<4;i++) qOut[i] = (fCoeff0 * qStart[i] + fCoeff1 * qEnd[i]);
        }
    } else
    {
        // There are two situations:
        // 1. "qStart" and "qEnd" are very close (fCos ~= +1), so we can do a linear
        //    interpolation safely.
        // 2. "qStart" and "qEnd" are almost inverse of each other (fCos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here.
        // IMPORTANT: CASE 2 can't happen anymore because we have hardcoded "shortestPath = true" and now fCos > 0

        const imoat fCoeff0 = 1.f - t;
        const imoat fCoeff1 = t;

        //qOut =  fCoeff0 * qStart + fCoeff1 * qEnd; //Avoided for maximum portability and conversion of the code
        //qOut[0] = (fCoeff0 * qStart[0] + fCoeff1 * qEnd[0]);
        //qOut[1] = (fCoeff0 * qStart[1] + fCoeff1 * qEnd[1]);
        //qOut[2] = (fCoeff0 * qStart[2] + fCoeff1 * qEnd[2]);
        //qOut[3] = (fCoeff0 * qStart[3] + fCoeff1 * qEnd[3]);
        for (i=0;i<4;i++) qOut[i] = (fCoeff0 * qStart[i] + fCoeff1 * qEnd[i]);
        if (normalizeQOutAfterLerp)  {
            imoat d = qOut[0]*qOut[0]+qOut[1]*qOut[1]+qOut[2]*qOut[2]+qOut[3]*qOut[3];
            if (d!=0) {
                d=sqrt(d);
                //qOut[0]/=d;qOut[1]/=d;qOut[2]/=d;qOut[3]/=d;
                for (i=0;i<4;i++) qOut[i]/=d;
            }
        }
    }

    }
}
/*int IMSlerpQuaternionWithAngularVelocity(imoat* __restrict qOut4,const imoat* __restrict qStart4,const imoat* __restrict qEnd4,imoat angularVelocity,imoat elapsedSecondsSinceLastCall)   {
    if (angularVelocity<=0) {
        return IMSlerpQuaternionWithAngularVelocity(qOut4,qEnd4,qStart4,-angularVelocity,elapsedSecondsSinceLastCall);
    }
    if (qOut4==qStart4) {
        imoat qs[4];if (qOut4==qEnd4) return 1;IMCopyQuaternion(qs,qOut4);
        return IMSlerpQuaternionWithAngularVelocity(qOut4,qs,qEnd4,angularVelocity,elapsedSecondsSinceLastCall);
    }
    if (qOut4==qEnd4) {
        imoat qe[4];IMCopyQuaternion(qe,qOut4);
        return IMSlerpQuaternionWithAngularVelocity(qOut4,qStart4,qe,angularVelocity,elapsedSecondsSinceLastCall);
    }

    {
    const imoat eps=0.0001f;                      	 // When it's reaching qEnd4 (through eps), make qOut4 = qEnd and return true.
    const int useAcosAndSinInsteadOfAtan2AndSqrt = 0;// Another possible minimal Speed vs Precision tweak (I suggest just changing it here and not in the caller code)

    const imoat* qStart = qStart4;
    imoat qEnd[4] = {qEnd4[0],qEnd4[1],qEnd4[2],qEnd4[3]};
    imoat* qOut = qOut4;
    imoat fCos;int i;
    fCos = qStart[0] * qEnd[0] + qStart[1] * qEnd[1] + qStart[2] * qEnd[2] + qStart[3] * qEnd[3];

    // Code based on Ogre::Quaternion (MIT licensed). Hardcoded "shortestPath=true" and simplified code.
    // Do we need to invert rotation?
    if(fCos < 0)	//Originally it was if(fCos < static_cast < Real >(0.0) && shortestPath)
    {
        fCos = -fCos;
        //qEnd[0] = -qEnd[0];qEnd[1] = -qEnd[1];qEnd[2] = -qEnd[2];qEnd[3] = -qEnd[3];
        for (i=0;i<4;i++) qEnd[i] = -qEnd[i];       // is this faster (auto-vectorization) ?
    }

    if( fCos < 1.f - eps)	// Originally if was "Ogre::Math::Abs(fCos)" instead of "fCos", but we know fCos>0, because we have hard coded shortestPath=true
    {
        // Standard case (slerp)
        imoat fSin,fAngle;
        if (!useAcosAndSinInsteadOfAtan2AndSqrt)	{
            // Ogre::Quaternion uses this branch by default
            fSin = sqrt(1.f - fCos*fCos);
            fAngle = atan2(fSin, fCos);
        }
        else	{
            // Possible replacement of the two lines above
            // (it's hard to tell if they're faster, but my instinct tells me I should trust atan2 better than acos (geometry geeks needed here...)):
            // But probably sin(...) is faster than (sqrt + 1 subtraction and mult)
            fAngle = acos(fCos);
            fSin = sin(fAngle);
        }

        if (fAngle<0) {printf("fAngle < 0 [%1.3f]: Error\n",fAngle*M_180OVERPI);exit(1);}
        // Now we have fAngle, and our angularVelocity = D(angle)/D(time) -> t = angularVelocity/D(angle)
        {
        const imoat Dangle = angularVelocity * elapsedSecondsSinceLastCall;
        imoat t;
        if (Dangle-eps>=fAngle) {for (i=0;i<4;i++) qOut[i]=qEnd[i];return 1;}

        t = Dangle/fAngle;
        const imoat fInvSin = 1.f / fSin;
        const imoat fCoeff0 = sin((1.f - t) * fAngle) * fInvSin;
        const imoat fCoeff1 = sin(t * fAngle) * fInvSin;

        //qOut =  fCoeff0 * qStart + fCoeff1 * qEnd; //Avoided for maximum portability and conversion of the code
        //qOut[0] = (fCoeff0 * qStart[0] + fCoeff1 * qEnd[0]);
        //qOut[1] = (fCoeff0 * qStart[1] + fCoeff1 * qEnd[1]);
        //qOut[2] = (fCoeff0 * qStart[2] + fCoeff1 * qEnd[2]);
        //qOut[3] = (fCoeff0 * qStart[3] + fCoeff1 * qEnd[3]);
        for (i=0;i<4;i++) qOut[i] = (fCoeff0 * qStart[i] + fCoeff1 * qEnd[i]);
        }
        return 0;
    } else
    {
        // Finished. We can return 1.
        for (i=0;i<4;i++) qOut[i]=qEnd[i];return 1;    // End
    }

    }
}*/
void IMSlerpMatrix(imoat* __restrict mOut16,const imoat* __restrict mStart16,const imoat* __restrict mEnd16,imoat slerpTime_In_0_1)    {
    if (slerpTime_In_0_1<=0)        {if (mOut16!=mStart16) IMCopyMatrix(mOut16,mStart16);return;}
    else if (slerpTime_In_0_1>=1)   {if (mOut16!=mEnd16)   IMCopyMatrix(mOut16,mEnd16);  return;}


    {
        if (1)  {
            // Lerp positions
            const imoat T1[3] = {mStart16[12],mStart16[13],mStart16[14]};
            const imoat T2[3] = {mEnd16[12],mEnd16[13],mEnd16[14]};
            const imoat t = slerpTime_In_0_1;
            const imoat one_minus_t = 1.f-t;

            //mOut16[12] = T1[0]*one_minus_t + T2[0]*t;
            //mOut16[13] = T1[1]*one_minus_t + T2[1]*t;
            //mOut16[14] = T1[2]*one_minus_t + T2[2]*t;
            int i;for (i=0;i<3;i++) mOut16[12+i] = T1[i]*one_minus_t + T2[i]*t;
        }
        if (1)  {
            // Slerp rotations
            imoat q[12];
            IMGetQuaternionFromMatrixRotation(q,mStart16);
            IMGetQuaternionFromMatrixRotation(&q[4],mEnd16);
            IMSlerpQuaternion(&q[8],q,&q[4],slerpTime_In_0_1);
            IMSetMatrixRotationFromQuaternion(mOut16,&q[8]);
        }

    }
}
/*
int IMSlerpMatrixWithVelocities(imoat* mOut16, const imoat* mStart16, const imoat* mEnd16, imoat linearVelocity, imoat angularVelocity, imoat elapsedSecondsSinceLastCall)    {
    int rv = 0,i;
    {
        if (1)  {
            // Lerp positions
            const imoat TS[3] = {mStart16[12],mStart16[13],mStart16[14]};
            const imoat TE[3] = {mEnd16[12],mEnd16[13],mEnd16[14]};
            imoat Delta[3] = {TE[0]-TS[0],TE[1]-TS[1],TE[2]-TS[2]};
            imoat len = Delta[0]*Delta[0]+Delta[1]*Delta[1]+Delta[2]*Delta[2];

            const imoat *T1 = TS;const imoat *T2 = TE;
            imoat vel = linearVelocity;
            if (vel<0) {vel=-vel;T1=TE;T2=TS;}
            if (len<0.000001f) {for (i=0;i<3;i++) mOut16[12+i] = T2[i];++rv;}
            else {
                len = sqrt(len);
                {
                    imoat Dlen = linearVelocity * elapsedSecondsSinceLastCall;
                    if (Dlen-0.001f>=len) {for (i=0;i<3;i++) mOut16[12+i] = T2[i];++rv;}
                    else {
                        const imoat t = Dlen/len;
                        const imoat one_minus_t = 1.f-t;

                        for (i=0;i<3;i++) mOut16[12+i] = T1[i]*one_minus_t + T2[i]*t;
                    }
                }
            }
        }
        if (1)  {
            // Slerp rotations
            imoat q[12];
            IMGetQuaternionFromMatrixRotation(q,mStart16);
            IMGetQuaternionFromMatrixRotation(&q[4],mEnd16);
            rv+=IMSlerpQuaternionWithAngularVelocity(&q[8],q,&q[4],angularVelocity,elapsedSecondsSinceLastCall);
            IMSetMatrixRotationFromQuaternion(mOut16,&q[8]);
        }

    }
    return (rv==2) ? 1 : 0;
}
*/
void IMGetFrustumPlaneEquations(imoat planeEquationsOut[6][4],const imoat* __restrict vpMatrix16,int normalizePlanes)  {
    // ax+by+cz+d=0 [xl,xr,yb,yt,zn,zf],normalizePlanes=0 -> no normalization
    imoat m00 = vpMatrix16[0], m01 = vpMatrix16[4], m02 = vpMatrix16[8],  m03 = vpMatrix16[12];
    imoat m10 = vpMatrix16[1], m11 = vpMatrix16[5], m12 = vpMatrix16[9],  m13 = vpMatrix16[13];
    imoat m20 = vpMatrix16[2], m21 = vpMatrix16[6], m22 = vpMatrix16[10], m23 = vpMatrix16[14];
    imoat m30 = vpMatrix16[3], m31 = vpMatrix16[7], m32 = vpMatrix16[11], m33 = vpMatrix16[15];
    imoat* p = NULL;
    p = &planeEquationsOut[0][0];   // Left
    p[0] = m30+m00; p[1] = m31+m01; p[2] = m32+m02; p[3] = m33+m03;if (normalizePlanes) IMPlaneNormalize(p);
    p = &planeEquationsOut[1][0];   // Right
    p[0] = m30-m00; p[1] = m31-m01; p[2] = m32-m02; p[3] = m33-m03;if (normalizePlanes) IMPlaneNormalize(p);
    p = &planeEquationsOut[2][0];   // Bottom
    p[0] = m30+m10; p[1] = m31+m11; p[2] = m32+m12; p[3] = m33+m13;if (normalizePlanes) IMPlaneNormalize(p);
    p = &planeEquationsOut[3][0];   // Top
    p[0] = m30-m10; p[1] = m31-m11; p[2] = m32-m12; p[3] = m33-m13;if (normalizePlanes) IMPlaneNormalize(p);
    p = &planeEquationsOut[4][0];   // Near
    p[0] = m30+m20; p[1] = m31+m21; p[2] = m32+m22; p[3] = m33+m23;if (normalizePlanes) IMPlaneNormalize(p);
    p = &planeEquationsOut[5][0];   // Far
    p[0] = m30-m20; p[1] = m31-m21; p[2] = m32-m22; p[3] = m33-m23;if (normalizePlanes) IMPlaneNormalize(p);
}

void IMGetFrustumPoints(imoat frustumPoints[8][4],const imoat* __restrict vpMatrixInverse16)    {
    const imoat v[8][4] = {{-1, -1, -1, 1},{-1,  1, -1, 1},{ 1,  1, -1, 1},{ 1, -1, -1, 1},{-1, -1, 1, 1},{-1,  1, 1, 1},{ 1,  1, 1, 1},{ 1, -1, 1, 1}};
    int i;
    for (i = 0; i < 8; i++) {
        IMMatrixMulPos16(vpMatrixInverse16,frustumPoints[i],v[i][0],v[i][1],v[i][2]);
        frustumPoints[i][3]=1;
    }
}

void imMatrixMode(ImEnum mode)  {    
    if (mode>=IM_MODELVIEW && mode<=IM_TEXTURE) {IMMS.matrixMode = mode-IM_MODELVIEW;return;}
    if (mode==IM_MODELVIEW_MATRIX)  {IMMS.matrixMode = 0;return;}
    if (mode==IM_PROJECTION_MATRIX)  {IMMS.matrixMode = 1;return;}
    if (mode==IM_TEXTURE_MATRIX)  {IMMS.matrixMode = 2;return;}
}
const imoat * imLoadIdentity(void) {
    imoat* m = &IMMS.matrixStacks[IMMS.matrixMode][IMMS.matrixStackIndices[IMMS.matrixMode]][0];
    IMSetMatrixIdentity(m);
    return m;
}
const imoat * imLoadMatrix(const imoat m[16]) {
    imoat* mi = &IMMS.matrixStacks[IMMS.matrixMode][IMMS.matrixStackIndices[IMMS.matrixMode]][0];
    IMCopyMatrix(mi,m);
    return mi;
}
const imoat* imMultMatrix(const imoat m[16]) {
    imoat* miPrev = &IMMS.matrixStacks[IMMS.matrixMode][IMMS.matrixStackIndices[IMMS.matrixMode]][0];
    imoat miNext[16];
    IMMultMatrix(miNext,miPrev,m);
    IMCopyMatrix(miPrev,miNext);
    return miPrev;
}
const imoat* imPreMultMatrix(const imoat m[16]) {
    imoat* miPrev = &IMMS.matrixStacks[IMMS.matrixMode][IMMS.matrixStackIndices[IMMS.matrixMode]][0];
    imoat miNext[16];
    IMMultMatrix(miNext,m,miPrev);
    IMCopyMatrix(miPrev,miNext);
    return miPrev;
}

const imoat* imTranslate(imoat x,imoat y,imoat z)  {
    const imoat m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1};
    return imMultMatrix(m);
}

const imoat* imRotate(imoat degAngle,imoat x,imoat y,imoat z)  {
    const imoat angle = degAngle*M_PIOVER180;
    const imoat c = cos(angle);
    const imoat s = sin(angle);
    imoat len = x*x+y*y+z*z;
    if (len<0.999f || len>1.001f) {len=sqrt(len);x/=len;y/=len;z/=len;}
    {
        const imoat m[16] = {
            c + x*x*(1-c),  y*x*(1-c)+z*s,    z*x*(1-c)-y*s,    0,
            x*y*(1-c) - z*s,  c + y*y*(1-c),      z*y*(1-c) + x*s,    0,
            x*z*(1-c) + y*s,  y*z*(1-c) - x*s,    c + z*z*(1-c),      0,
            0,              0,                  0,                  1};
        return imMultMatrix(m);
    }
}

const imoat* imScale(imoat x,imoat y,imoat z)  {
    const imoat m[16] = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
    return imMultMatrix(m);
}

const imoat*  imPushMatrix(void) {
    int* pind = &IMMS.matrixStackIndices[IMMS.matrixMode];
    const imoat* miPrev = &IMMS.matrixStacks[IMMS.matrixMode][*pind][0];
    imoat* miNext=0;
    if ((*pind)+1<IMMS.matrixStackDepths[IMMS.matrixMode]) {
        ++(*pind);
        miNext = &IMMS.matrixStacks[IMMS.matrixMode][*pind][0];
        IMCopyMatrix(miNext,miPrev);
    }
    return &IMMS.matrixStacks[IMMS.matrixMode][*pind][0];
}
const imoat* imPopMatrix(void) {
    int* pind = &IMMS.matrixStackIndices[IMMS.matrixMode];
    if ((*pind)>0) {--(*pind);}
    return &IMMS.matrixStacks[IMMS.matrixMode][*pind][0];
}

void imGetImoatv(ImEnum mode,imoat m[16])   {
    const imoat* mi;
    if (mode>=IM_MODELVIEW && mode<=IM_TEXTURE) mode-=IM_MODELVIEW;
    else if (mode==IM_MODELVIEW_MATRIX)  mode = 0;
    else if (mode==IM_PROJECTION_MATRIX)  mode = 1;
    else if (mode==IM_TEXTURE_MATRIX)  mode = 2;
    else return;

    mi = &IMMS.matrixStacks[mode][IMMS.matrixStackIndices[mode]][0];
    IMCopyMatrix(m,mi);
}
const imoat *imGetImoatMatrixPtr(void) {
    return &IMMS.matrixStacks[IMMS.matrixMode][IMMS.matrixStackIndices[IMMS.matrixMode]][0];
}
const imoat *imGetImoatMatrixPtrFromMode(ImEnum mode) {
    if (mode>=IM_MODELVIEW && mode<=IM_TEXTURE) mode-=IM_MODELVIEW;
    else if (mode==IM_MODELVIEW_MATRIX)  mode = 0;
    else if (mode==IM_PROJECTION_MATRIX)  mode = 1;
    else if (mode==IM_TEXTURE_MATRIX)  mode = 2;
    else return 0;

    return &IMMS.matrixStacks[mode][IMMS.matrixStackIndices[mode]][0];
}

void imuGetImoatNMatrixFromMvMatrix(imoat* __restrict nMatrix9Out,const imoat* __restrict mvMatrix16)  {
    const imoat* m = mvMatrix16;
    imoat* n = nMatrix9Out;

    // We need to invert mvMatrix[9] before and then trnspose it on the fly--------
    // Create shorthands to access matrix members
    imoat m00 = m[0],  m10 = m[1],  m20 = m[2];
    imoat m01 = m[4],  m11 = m[5],  m21 = m[6];
    imoat m02 = m[8],  m12 = m[9],  m22 = m[10];

#   ifndef IM_MATRIX_STACK_GETNMATRIX_NO_OPTIMIZE
    // Optional optimization:
    const imoat sqcol0 = m00*m00 + m10*m10 + m20*m20;
    const imoat sqcol1 = m01*m01 + m11*m11 + m21*m21;
    const imoat sqcol2 = m02*m02 + m12*m12 + m22*m22;
    const imoat eps = 0.0001;
    if (fabs(sqcol0-sqcol1)<eps && fabs(sqcol0-sqcol2)<eps && fabs(sqcol1-sqcol2)<eps)  {
        //save a lot of expesive calculations and just set nMatrix to mvMatrix.3x3 (but it needs be renormalized in the shader anyway)
        n[0]=m00; n[1]=m10; n[2]=m20;
        n[3]=m01; n[4]=m11; n[5]=m21;
        n[6]=m02; n[7]=m12; n[8]=m22;
    }
    else
#   endif //IM_MATRIX_STACK_GETNMATRIX_NO_OPTIMIZE
    {

        // Calculate cofactor matrix
        imoat c00 =   m11*m22 - m12*m21,   c10 = -(m01*m22 - m02*m21),  c20 =   m01*m12 - m02*m11;
        imoat c01 = -(m10*m22 - m12*m20),  c11 =   m00*m22 - m02*m20,   c21 = -(m00*m12 - m02*m10);
        imoat c02 =   m10*m21 - m11*m20,   c12 = -(m00*m21 - m01*m20),  c22 =   m00*m11 - m01*m10;

        imoat i00,i10,i20, i01,i11,i21, i02,i12,i22;

        // Caclculate the determinant by using the already calculated determinants
        // in the cofactor matrix.
        // Second sign is already minus from the cofactor matrix.
        imoat det = m00*c00 + m10*c10 + m20 * c20;
        if (fabs(det) < 0.00001)    {
            // return identity
            n[0]=n[4]=n[8]=1;
            n[1]=n[2]=n[3]=n[5]=n[6]=n[7]=0;
            return;
        }

        // Calcuate inverse by dividing the transposed cofactor matrix by the determinant.
        i00 = c00 / det;  i10 = c01 / det;  i20 = c02 / det;
        i01 = c10 / det;  i11 = c11 / det;  i21 = c12 / det;
        i02 = c20 / det;  i12 = c21 / det;  i22 = c22 / det;

        // Transpose it on the fly
        n[0] = i00; n[1] = i01; n[2] = i02;
        n[3] = i10; n[4] = i11; n[5] = i12;
        n[6] = i20; n[7] = i21; n[8] = i22;
    }

}


const imoat *imuLookAt(imoat eyeX,imoat eyeY,imoat eyeZ,imoat centerX,imoat centerY,imoat centerZ,imoat upX,imoat upY,imoat upZ)    {
    imoat m[16];
    const imoat eps = 0.0001f;

    imoat F[3] = {eyeX-centerX,eyeY-centerY,eyeZ-centerZ};
    imoat length = F[0]*F[0]+F[1]*F[1]+F[2]*F[2];	// length2 now
    imoat up[3] = {upX,upY,upZ};

    imoat S[3] = {up[1]*F[2]-up[2]*F[1],up[2]*F[0]-up[0]*F[2],up[0]*F[1]-up[1]*F[0]};
    imoat U[3] = {F[1]*S[2]-F[2]*S[1],F[2]*S[0]-F[0]*S[2],F[0]*S[1]-F[1]*S[0]};

    if (length==0) length = eps;
    length = sqrt(length);
    F[0]/=length;F[1]/=length;F[2]/=length;

    length = S[0]*S[0]+S[1]*S[1]+S[2]*S[2];if (length==0) length = eps;
    length = sqrt(length);
    S[0]/=length;S[1]/=length;S[2]/=length;

    length = U[0]*U[0]+U[1]*U[1]+U[2]*U[2];if (length==0) length = eps;
    length = sqrt(length);
    U[0]/=length;U[1]/=length;U[2]/=length;

    /*
                S0	S1	S2  0		1	0	0	-ex
                U0	U1	U2	0   *   0	1	0	-ey
                F0	F1	F2  0		0	0	1	-ez
                0	0	0	1		0	0	0	1

            */
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

    return imMultMatrix(m);
}

const imoat* imuLookAtYX(imoat* __restrict cameraMatrix16InOut,imoat centerX,imoat centerY,imoat centerZ,imoat minDistanceToCenterAllowed,imoat maxDistanceToCenterAllowed,int invert_xz_axis)   {
    imoat* m = cameraMatrix16InOut;
    imoat vMatrix[16];  // Probably we could save this tmp var, if we call imLoadMatrix at the end...
    IMLookAtYX(m,centerX,centerY,centerZ,minDistanceToCenterAllowed,maxDistanceToCenterAllowed);
    if (!invert_xz_axis) IMMatrixInvertXZAxisInPlace(m);  // This is necessary to invert the camera convention so that we can use for all objects (camera included): +X = left, +Y = up, +Z = forward
    IMInvertMatrix_Fast(vMatrix,m);
    if (!invert_xz_axis) IMMatrixInvertXZAxisInPlace(m);  // Undo previous axis inversion
    return imMultMatrix(vMatrix);
}
const imoat *imimLoadCameraMatrix(const imoat* __restrict cameraMatrix16) {
    imoat* m = (imoat*) cameraMatrix16;
    imoat* mi = &IMMS.matrixStacks[IMMS.matrixMode][IMMS.matrixStackIndices[IMMS.matrixMode]][0];
    IMMatrixInvertXZAxisInPlace(m);  // This is necessary to invert the camera convention so that we can use for all objects (camera included): +X = left, +Y = up, +Z = forward
    IMInvertMatrix_Fast(mi,m);
    IMMatrixInvertXZAxisInPlace(m);  // Undo previous axis inversion
    return mi;
}


// Typically, the matrix mode is GL_PROJECTION, and (left,bottom,-nearVal) and (right,top,-nearVal) specify the points on the near clipping plane that are mapped to the lower left and upper right corners of the window, assuming that the eye is located at (0, 0, 0).
// -farVal specifies the location of the far clipping plane.
// Both nearVal and farVal must be positive (glFrustum(...) seems to return identity() otherwise).
const imoat* imFrustum(imoat left,imoat right, imoat bottom, imoat top,imoat zNear, imoat zFar) {
    imoat res[16];
    const imoat eps = 0.0001f;
    imoat Drl = (right-left);
    imoat Dtb = (top-bottom);
    imoat Dfn = (zFar-zNear);
    if (zNear<=0 || zFar<=0) return &IMMS.matrixStacks[IMMS.matrixMode][IMMS.matrixStackIndices[IMMS.matrixMode]][0];   // to mimic glFrustum(...)
    if (Drl==0) {right+=eps;left-=eps;Drl=right-left;}
    if (Dtb==0) {top+=eps;bottom-=eps;Dtb=top-bottom;}
    if (Dfn==0) {zFar+=eps;zNear-=eps;Dfn=zFar-zNear;}

    res[0]  = 2.f*zNear/Drl;
    res[1]  = 0;
    res[2]  = 0;
    res[3] = 0;

    res[4]  = 0;
    res[5]  = 2.f*zNear/Dtb;
    res[6]  = 0;
    res[7] = 0;

    res[8]  = (right+left)/Drl;
    res[9]  = (top+bottom)/Dtb;
    res[10] = -(zFar+zNear)/Dfn;
    res[11] = -1;

    res[12]  = 0;
    res[13]  = 0;
    res[14] = -2.f*zFar*zNear/Dfn;
    res[15] = 0;

    return imMultMatrix(res);
}

const imoat *imuPerspective(imoat fovy,imoat aspect, imoat zNear, imoat zFar) {
    imoat res[16];
    const imoat eps = 0.0001f;
    imoat f = 1.f/tan(fovy*1.5707963268f/180.0); //cotg
    imoat Dfn = (zFar-zNear);
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

    return imMultMatrix(res);
}

const imoat* imuOrtho2D(imoat left,imoat right, imoat bottom, imoat top) {
    imoat res[16];
    const imoat eps = 0.0001f;
    imoat Drl = (right-left);
    imoat Dtb = (top-bottom);
    if (Drl==0) {right+=eps;left-=eps;Drl=right-left;}
    if (Dtb==0) {top+=eps;bottom-=eps;Dtb=top-bottom;}

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
    res[10] = -1;
    res[11] = 0;

    res[12]  = -(right+left)/Drl;
    res[13]  = -(top+bottom)/Dtb;
    res[14] = 0;
    res[15] = 1;

    return imMultMatrix(res);
}


// Typically, the matrix mode is GL_PROJECTION, and (left, bottom, -nearVal) and (right,top,-nearVal)
// specify the points on the near clipping plane that are mapped to the lower left and upper right corners of the window, respectively, assuming that the eye is located at (0, 0, 0).
// -farVal pecifies the location of the far clipping plane.
// Both nearVal and farVal can be either positive or negative.
const imoat* imOrtho(imoat left,imoat right, imoat bottom, imoat top,imoat nearVal,imoat farVal) {
    imoat res[16];
    const imoat eps = 0.0001f;
    imoat Drl = (right-left);
    imoat Dtb = (top-bottom);
    imoat Dfn = (farVal-nearVal);
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

    return imMultMatrix(res);
}
/**
 * Untested, by it might be useful in 3d editors that can switch from prjective and ortho mode:
 * basically in ortho mode we must change the projection matrix
 * every time the distance between the camera and the camera target changes.
*/
const imoat * imOrtho3D(imoat cameraTargetDistance,imoat degFOV,imoat aspect,imoat znear,imoat zfar)	{
    const imoat FOVTG=tan(degFOV*1.5707963268f/180.0);
    imoat y=cameraTargetDistance*FOVTG;//=(zfar-znear)*0.5f;
    imoat x=y*aspect;
    return imOrtho( -x, x, -y, y, znear, zfar);
}

// transforms the specified object coordinates into window coordinates using mvMatrix16, pMatrix16, and viewport4.
// The result is stored in winX, winY, and winZ. A return value of 1 indicates success, a return value of 0 indicates failure.
// viewport4 Specifies the current viewport (as from a glGetIntegerv call).
int imuProject(imoat objX,imoat objY,imoat objZ,const imoat* __restrict mvMatrix16,const imoat* __restrict pMatrix16,const int* viewport4,imoat* winX,imoat* winY,imoat* winZ)    {
    // To compute the coordinates, let v = objX objY objZ 1.0 represented as a matrix with 4 rows and 1 column.
    // Then gluProject computes v ″ as follows:
    // v ″ = P × M × v
    // where P is the current projection matrix proj and M is the current modelview matrix model
    // (both represented as 4 × 4 matrices in column-major order).
    const imoat objW = 1;
    // v ″ = P × M × v = P × (M × v) AFAIK
    const imoat mv[4] = {
        objX*mvMatrix16[0] + objY*mvMatrix16[4] + objZ*mvMatrix16[8]  + objW*mvMatrix16[12],
        objX*mvMatrix16[1] + objY*mvMatrix16[5] + objZ*mvMatrix16[9]  + objW*mvMatrix16[13],
        objX*mvMatrix16[2] + objY*mvMatrix16[6] + objZ*mvMatrix16[10] + objW*mvMatrix16[14],
        objX*mvMatrix16[3] + objY*mvMatrix16[7] + objZ*mvMatrix16[11] + objW*mvMatrix16[15]
    };
    imoat pmv[4] = {
        mv[0]*pMatrix16[0] + mv[1]*pMatrix16[4] + mv[2]*pMatrix16[8]  + mv[3]*pMatrix16[12],
        mv[0]*pMatrix16[1] + mv[1]*pMatrix16[5] + mv[2]*pMatrix16[9]  + mv[3]*pMatrix16[13],
        mv[0]*pMatrix16[2] + mv[1]*pMatrix16[6] + mv[2]*pMatrix16[10] + mv[3]*pMatrix16[14],
        mv[0]*pMatrix16[3] + mv[1]*pMatrix16[7] + mv[2]*pMatrix16[11] + mv[3]*pMatrix16[15] // (*)
    };

    if(pmv[3]==0.0) return 0;
    pmv[0]/=pmv[3];pmv[1]/=pmv[3];pmv[2]/=pmv[3];

    // The window coordinates are then computed as follows:
    *winX = (imoat)viewport4[0] + (imoat)viewport4[2]*(pmv[0]*0.5+0.5);
    *winY = (imoat)viewport4[1] + (imoat)viewport4[3]*(pmv[1]*0.5+0.5);
    *winZ = pmv[2]*0.5+0.5;

    return 1;
}

// maps the specified window coordinates into object coordinates using mvMatrix16, pMatrix16, and viewport4.
// The result is stored in objX, objY, and objZ. A return value of 1 indicates success; a return value of 0 indicates failure.
// viewport4 Specifies the current viewport (as from a glGetIntegerv call).
int imuUnProject(imoat winX,imoat winY,imoat winZ,const imoat* __restrict mvMatrix16,const imoat* __restrict pMatrix16,const int* viewport4,imoat* objX,imoat* objY,imoat* objZ)    {
// imuUnProject4 is equivalent to imuUnProject when clipW is 1, nearVal is 0, and farVal is 1.

// To compute the coordinates objX objY objZ , gluUnProject multiplies the normalized device coordinates by the inverse of model * proj as follows:
// [objX objY objZ W] = INV(P*M) * ⁢ [2*(winX-view[0])/view[2]-1    2*(winY-view[1])/view[3]-1  2*winZ-1    1]
// INV denotes matrix inversion. W is an unused variable, included for consistent matrix notation.
   imoat pm[16],invpm[16];
   // normalized device coords
   const imoat v[4] = {
      2*(winX-(imoat)viewport4[0])/(imoat)viewport4[2]-1,
      2*(winY-(imoat)viewport4[1])/(imoat)viewport4[3]-1,
      2*winZ-1,
      1
   };
   imoat objW = 0;
   // calculate pm:
   IMMultMatrix(pm,pMatrix16,mvMatrix16);

   // calculate invpm:
   IMInvertMatrix(invpm,pm);

   // calculate obj
   *objX =  v[0]*invpm[0] + v[1]*invpm[4] + v[2]*invpm[8]  + v[3]*invpm[12];
   *objY =  v[0]*invpm[1] + v[1]*invpm[5] + v[2]*invpm[9]  + v[3]*invpm[13];
   *objZ =  v[0]*invpm[2] + v[1]*invpm[6] + v[2]*invpm[10] + v[3]*invpm[14];
    objW =  v[0]*invpm[3] + v[1]*invpm[7] + v[2]*invpm[11] + v[3]*invpm[15];

   /* *objX =  v[0]*invpm[0] + v[1]*invpm[1] + v[2]*invpm[2]  + v[3]*invpm[3];
   *objY =  v[0]*invpm[4] + v[1]*invpm[5] + v[2]*invpm[6]  + v[3]*invpm[7];
   *objZ =  v[0]*invpm[8] + v[1]*invpm[9] + v[2]*invpm[10] + v[3]*invpm[11];*/
   //objW  =  v[0]*invpm[12] + v[1]*invpm[13] + v[2]*invpm[14] + v[3]*invpm[15];
   if (objW!=0 && objW!=1) {
       (*objX)/=objW;(*objY)/=objW;(*objZ)/=objW;
   }

   return 1;
}

// maps the specified window coordinatesi: winX, winY, and winZ and its clip w coordinate clipW
// into object coordinates objX objY objZ objW using mvMatrix16, pMatrix16, and viewport4.
// clipW can be other than 1 as for vertices in glFeedbackBuffer when data type GLU_4D_COLOR_TEXTURE is returned.
// This also handles the case where the nearVal and farVal planes are different from the default, 0 and 1, respectively.
// A return value of 1 indicates success; a return value of 0 indicates failure.
// viewport4 Specifies the current viewport (as from a glGetIntegerv call).
int imuUnProject4(imoat winX, imoat winY, imoat winZ, imoat clipW, const imoat* __restrict mvMatrix16, const imoat* __restrict pMatrix16, const int* viewport4, imoat nearVal, imoat farVal, imoat* objX, imoat* objY, imoat* objZ, imoat* objW)    {
// [objX objY objZ objW] = INV(P*M) * ⁢ [2*(winX-view[0])/view[2]-1    2*(winY-view[1])/view[3]-1  2*(winZ-nearVal)/(farVal-nearVal)-1    clipW]
// imuUnProject4 is equivalent to imuUnProject when clipW is 1, nearVal is 0, and farVal is 1.
   imoat pm[16],invpm[16];
   // normalized device coords
   const imoat v[4] = {
      2*(winX-(imoat)viewport4[0])/(imoat)viewport4[2]-1,
      2*(winY-(imoat)viewport4[1])/(imoat)viewport4[3]-1,
      2*(winZ-nearVal)/(farVal-nearVal)-1,
      clipW
   };
   // calculate pm:
   IMMultMatrix(pm,pMatrix16,mvMatrix16);

   // calculate invpm:
   IMInvertMatrix(invpm,pm);

   // calculate obj
   *objX =  v[0]*invpm[0] + v[1]*invpm[4] + v[2]*invpm[8]  + v[3]*invpm[12];
   *objY =  v[0]*invpm[1] + v[1]*invpm[5] + v[2]*invpm[9]  + v[3]*invpm[13];
   *objZ =  v[0]*invpm[2] + v[1]*invpm[6] + v[2]*invpm[10] + v[3]*invpm[14];
   *objW =  v[0]*invpm[3] + v[1]*invpm[7] + v[2]*invpm[11] + v[3]*invpm[15];
   return 1;
}

static __inline void IMHelperMin3(imoat* __restrict res3,const imoat* a3,const imoat* b3) {
    int i;for (i=0;i<3;i++) res3[i]=a3[i]<b3[i]?a3[i]:b3[i];
}
static __inline void IMHelperMax3(imoat* __restrict res3,const imoat* a3,const imoat* b3) {
    int i;for (i=0;i<3;i++) res3[i]=a3[i]>b3[i]?a3[i]:b3[i];
}
int IMIsVisible(const imoat frustumPlanes[6][4],const imoat*__restrict mfMatrix16,imoat aabbMinX,imoat aabbMinY,imoat aabbMinZ,imoat aabbMaxX,imoat aabbMaxY,imoat aabbMaxZ)    {
    // It "should" performs AABB test. mfMatrix16 is the matrix M so that: F*M = mvpMatrix (F being the matrix used to extract the frustum planes). Here we use: F=pMatrix and M=mvMatrix, but it could be: F=vpMatrix and M=mMatrix too.
    int i;
    // AABB transformation by: http://dev.theomader.com/transform-bounding-boxes/
    const imoat* m = mfMatrix16;
    const imoat a[9] = {m[0]*aabbMinX,m[1]*aabbMinX,    m[2]*aabbMinX, m[4]*aabbMinY,m[5]*aabbMinY,m[6]*aabbMinY,   m[8]*aabbMinZ,m[9]*aabbMinZ,m[10]*aabbMinZ};
    const imoat b[9] = {m[0]*aabbMaxX,m[1]*aabbMaxX,    m[2]*aabbMaxX, m[4]*aabbMaxY,m[5]*aabbMaxY,m[6]*aabbMaxY,   m[8]*aabbMaxZ,m[9]*aabbMaxZ,m[10]*aabbMaxZ};
    imoat buf[18];
    IMHelperMin3(&buf[0], &a[0],&b[0]);
    IMHelperMin3(&buf[3], &a[3],&b[3]);
    IMHelperMin3(&buf[6], &a[6],&b[6]);
    IMHelperMax3(&buf[9], &a[0],&b[0]);
    IMHelperMax3(&buf[12],&a[3],&b[3]);
    IMHelperMax3(&buf[15],&a[6],&b[6]);
    {
        const imoat aabb[6] = {
            buf[0]+buf[ 3]+buf[ 6]+m[12], buf[ 1]+buf[ 4]+buf[ 7]+m[13], buf[ 2]+buf[ 5]+buf[ 8]+m[14],
            buf[9]+buf[12]+buf[15]+m[12], buf[10]+buf[13]+buf[16]+m[13], buf[11]+buf[14]+buf[17]+m[14]
        };
        // End AABB transformation

        {
            for(i=0; i < 6; i++) {
                const imoat *pl = &frustumPlanes[i][0];
                const int p[3] = {3*(int)(pl[0]>0.0f),3*(int)(pl[1]>0.0f),3*(int)(pl[2]>0.0f)};   // p[j] = 0 or 3
                const imoat dp = pl[0]*aabb[p[0]] + pl[1]*aabb[p[1]+1] + pl[2]*aabb[p[2]+2] + pl[3];
                if (dp < 0) return 0;
            }
        }
    }
    return 1;
}


void imMatrixPrint() {imMatrixFPrintp(stdout,0, 4);}
void imMatrixPrintp(int width, int precision) {imMatrixFPrintp(stdout,width, precision);}
void imMatrixFPrint(FILE* stream) {imMatrixFPrintp(stream, 0, 4);}
void imMatrixFPrintp(FILE* stream,int width, int precision) {
    int w = width, p = precision, r;
    const imoat* m = imGetImoatMatrixPtr();
    fprintf(stream, "{");
    for(r = 0; r < 16; r++) {fprintf(stream,"%*.*f",w,p,m[r]);if (r!=15) fprintf(stream,",");}
    fprintf(stream, "};\n");
}


void Im_Matrix_Stack_Init(void)  {
    Im_Internal_Reset(&IMMS);
}
void Im_Matrix_Stack_Destroy(void)   {

}

#ifdef __cplusplus
}
#endif

#endif //IM_MATRIX_STACK_IMPLEMENTATION_H
#endif //IM_MATRIX_STACK_IMPLEMENTATION


