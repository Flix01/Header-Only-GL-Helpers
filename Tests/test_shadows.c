/** License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

// DEPENDENCIES:
/*
-> glut or freeglut (the latter is recommended)
-> glew (Windows only)
*/

// HOW TO COMPILE:
/*
// LINUX:
gcc -O2 -std=gnu89 test_shadows.c -o test_shadows -I"../" -lglut -lGL -lX11 -lm
// WINDOWS (here we use the static version of glew, and glut32.lib, that can be replaced by freeglut.lib):
cl /O2 /MT /Tc test_shadows.c /D"TEAPOT_NO_RESTRICT" /D"DYNAMIC_RESOLUTION_NO_RESTRICT" /D"GLEW_STATIC" /I"../" /link /out:test_shadows.exe glut32.lib glew32s.lib opengl32.lib gdi32.lib Shell32.lib comdlg32.lib user32.lib kernel32.lib
// EMSCRIPTEN:
emcc -O2 -std=gnu89 -fno-rtti -fno-exceptions -o test_shadows.html test_shadows.c -I"./" -I"../" -s LEGACY_GL_EMULATION=0 --closure 1
(for web assembly add: -s WASM=1)

// IN ADDITION:
By default the source file assumes that every OpenGL-related header is in "GL/".
But you can define in the command-line the correct paths you use in your system
for glut.h, glew.h, etc. with something like:
-DGLUT_PATH=\"Glut/glut.h\"
-DGLEW_PATH=\"Glew/glew.h\"
(this syntax works on Linux, don't know about Windows)
*/

//#define USE_GLEW  // By default it's only defined for Windows builds (but can be defined in Linux/Mac builds too)

#ifdef __EMSCRIPTEN__
#	undef USE_GLEW
#endif //__EMSCRIPTEN__

// These path definitions can be passed to the compiler command-line
#ifndef GLUT_PATH
#   define GLUT_PATH "GL/glut.h"    // Mandatory
#endif //GLEW_PATH
#ifndef FREEGLUT_EXT_PATH
#   define FREEGLUT_EXT_PATH "GL/freeglut_ext.h"    // Optional (used only if glut.h comes from the freeglut library)
#endif //GLEW_PATH
#ifndef GLEW_PATH
#   define GLEW_PATH "GL/glew.h"    // Mandatory for Windows only
#endif //GLEW_PATH

#ifdef _WIN32
#	include "windows.h"
#	define USE_GLEW
#endif //_WIN32

#ifdef USE_GLEW
#	include GLEW_PATH
#else //USE_GLEW
#	define GL_GLEXT_PROTOTYPES
#endif //USE_GLEW

#include GLUT_PATH
#ifdef __FREEGLUT_STD_H__
#   ifndef __EMSCRIPTEN__
#       include FREEGLUT_EXT_PATH
#   endif //__EMSCRIPTEN__
#endif //__FREEGLUT_STD_H__


#include <stdio.h>
#include <math.h>
#include <string.h>

#define TEAPOT_CENTER_MESHES_ON_FLOOR           // (Optional) Otherwise meshes are centered in their local aabb center
//#define TEAPOT_INVERT_MESHES_Z_AXIS           // (Optional) Otherwise meshes look in the opposite Z direction
#define TEAPOT_SHADER_SPECULAR                  // (Optional) specular hilights
#define TEAPOT_SHADER_FOG                       // (Optional) fog to remove bad clipping
#define TEAPOT_SHADER_FOG_HINT_FRAMENT_SHADER   // (Optional) better fog quality
#define TEAPOT_SHADER_USE_SHADOW_MAP            // Mandatory for implementing the second shadow mapping step, but can be disabled in this demo
#define TEAPOT_ENABLE_FRUSTUM_CULLING           // (Optional) a bit expensive, and does not cull 100% hidden objects. You'd better test if it works and if it's faster...
#define TEAPOT_IMPLEMENTATION                   // Mandatory in 1 source file (.c or .cpp)
#include "teapot.h"


// "dynamic_resolution.h" implements the first shadow mapping step and optionally dynamic resolution (that by default should keep frame rate > config.dynamic_resolution_target_fps)
//#define DYNAMIC_RESOLUTION_USE_GLSL_VERSION_330   // (Optional) Not sure it's faster...
#define DYNAMIC_RESOLUTION_IMPLEMENTATION           // Mandatory in 1 source file (.c or .cpp)
#include "dynamic_resolution.h"


// Optional local definitions
#define VISUALIZE_DEPTH_TEXTURE // requires the ffp (does not work in WebGL)
//#define TEST_ADD_USER_MESH    // try it!

// Config file handling: basically there's an .ini file next to the
// exe that you can tweak. (it's just an extra)
const char* ConfigFileName = "test_shadows.ini";
typedef struct {
    int fullscreen_width,fullscreen_height;
    int windowed_width,windowed_height;
    int fullscreen_enabled;
    int dynamic_resolution_enabled;
    int dynamic_resolution_target_fps;
    int show_fps;
} Config;
void Config_Init(Config* c) {
    c->fullscreen_width=c->fullscreen_height=0;
    c->windowed_width=960;c->windowed_height=540;
    c->fullscreen_enabled=0;
    c->dynamic_resolution_enabled=1;
    c->dynamic_resolution_target_fps=35;
    c->show_fps = 0;
}
#ifndef __EMSCRIPTEN__
int Config_Load(Config* c,const char* filePath)  {
    FILE* f = fopen(filePath, "rt");
    char ch='\0';char buf[256]="";
    size_t nread=0;
    int numParsedItem=0;
    if (!f)  return -1;
    while ((ch = fgetc(f)) !=EOF)    {
        buf[nread]=ch;
        nread++;
        if (nread>255) {
            nread=0;
            continue;
        }
        if (ch=='\n') {
           buf[nread]='\0';
           if (nread<2 || buf[0]=='[' || buf[0]=='#') {nread = 0;continue;}
           if (nread>2 && buf[0]=='/' && buf[1]=='/') {nread = 0;continue;}
           // Parse
           switch (numParsedItem)    {
               case 0:
               sscanf(buf, "%d %d", &c->fullscreen_width,&c->fullscreen_height);
               break;
               case 1:
               sscanf(buf, "%d %d", &c->windowed_width,&c->windowed_height);
               break;
               case 2:
               sscanf(buf, "%d", &c->fullscreen_enabled);
               break;
               case 3:
               sscanf(buf, "%d", &c->dynamic_resolution_enabled);
               break;
               case 4:
               sscanf(buf, "%d", &c->dynamic_resolution_target_fps);
               break;
               case 5:
               sscanf(buf, "%d", &c->show_fps);
               break;
           }
           nread=0;
           ++numParsedItem;
        }
    }
    fclose(f);
    if (c->windowed_width<=0) c->windowed_width=720;
    if (c->windowed_height<=0) c->windowed_height=405;
    if (c->dynamic_resolution_target_fps<=0) c->dynamic_resolution_target_fps=35;

    return 0;
}
int Config_Save(Config* c,const char* filePath)  {
    FILE* f = fopen(filePath, "wt");
    if (!f)  return -1;
    fprintf(f, "[Size In Fullscreen Mode (zero means desktop size)]\n%d %d\n",c->fullscreen_width,c->fullscreen_height);
    fprintf(f, "[Size In Windowed Mode]\n%d %d\n",c->windowed_width,c->windowed_height);
    fprintf(f, "[Fullscreen Enabled (0 or 1) (CTRL+RETURN)]\n%d\n", c->fullscreen_enabled);
    fprintf(f, "[Dynamic Resolution Enabled (0 or 1) (F1)]\n%d\n", c->dynamic_resolution_enabled);
    fprintf(f, "[Dynamic Resolution Target FPS]\n%d\n", c->dynamic_resolution_target_fps);
    fprintf(f, "[Show FPS (0 or 1) (F2)]\n%d\n", c->show_fps);
    fprintf(f,"\n");
    fclose(f);
    return 0;
}
#endif //__EMSCRIPTEN__
Config config;

// glut has a special fullscreen GameMode that you can toggle with CTRL+RETURN (not in WebGL)
int windowId = 0; 			// window Id when not in fullscreen mode
int gameModeWindowId = 0;	// window Id when in fullscreen mode

// Now we can start with our program

// camera data:
float targetPos[3];         // please set it in resetCamera()
float cameraYaw;            // please set it in resetCamera()
float cameraPitch;          // please set it in resetCamera()
float cameraDistance;       // please set it in resetCamera()
float cameraPos[3];         // Derived value (do not edit)
float vMatrix[16];          // view matrix
float cameraSpeed = 0.5f;   // When moving it

// light data
float lightDirection[3] = {1,2,1.5};// Will be normalized

// pMatrix data:
float pMatrix[16];                  // projection matrix
const float pMatrixFovDeg = 45.f;
const float pMatrixNearPlane = 0.5f;
const float pMatrixFarPlane = 20.0f;

float instantFrameTime = 16.2f;

// custom replacement of gluPerspective(...)
static void Perspective(float res[16],float degfovy,float aspect, float zNear, float zFar) {
    const float eps = 0.0001f;
    float f = 1.f/tan(degfovy*1.5707963268f/180.0); //cotg
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
// custom replacement of glOrtho(...)
static void Ortho(float res[16],float left,float right, float bottom, float top,float nearVal,float farVal) {
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

// custom replacement of gluLookAt(...)
static void LookAt(float m[16],float eyeX,float eyeY,float eyeZ,float centerX,float centerY,float centerZ,float upX,float upY,float upZ)    {
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
static __inline float Vec3Dot(const float v0[3],const float v1[3]) {
    return v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
}
static __inline void Vec3Normalize(float v[3]) {
    float len = Vec3Dot(v,v);int i;
    if (len!=0) {
        len = sqrt(len);
        for (i=0;i<3;i++) v[i]/=len;
    }
}
static __inline void Vec3Cross(float rv[3],const float a[3],const float b[3]) {
    rv[0] =	a[1] * b[2] - a[2] * b[1];
    rv[1] =	a[2] * b[0] - a[0] * b[2];
    rv[2] =	a[0] * b[1] - a[1] * b[0];
}


float current_width=0,current_height=0,current_aspect_ratio=1;  // Not sure when I've used these...
void ResizeGL(int w,int h) {
    current_width = (float) w;
    current_height = (float) h;
    if (current_height!=0) current_aspect_ratio = current_width/current_height;
    if (h>0)	{
        // We set our pMatrix here in ResizeGL(), and we must notify teapot.h about it too.
        Perspective(pMatrix,pMatrixFovDeg,(float)w/(float)h,pMatrixNearPlane,pMatrixFarPlane);
        Teapot_SetProjectionMatrix(pMatrix);
    }

    if (h>0) Dynamic_Resolution_Resize(w,h);    // The dynamic resolution texture (and the shadow map) change their size with this call

    if (w>0 && h>0 && !config.fullscreen_enabled) {
        // On exiting we'll like to save these data back
        config.windowed_width=w;
        config.windowed_height=h;
    }

    glViewport(0,0,w,h);    // This is what people often call in ResizeGL()

}

// Since we have to draw the objects twice (for shadow mapping), it's better to store them in an array.
// We're very lucky because teapot.h provides one for us!
static Teapot_MeshData* allocated_memory = NULL;    // This is allocated in InitGL() and freed in DestroyGL()
static Teapot_MeshData* pMeshData[30];              // This just points to parts of allocated_memory (to avoid multiple allocations)
static const int maxNumMeshData = sizeof(pMeshData)/sizeof(pMeshData[0]);
static int numMeshData = 0; // initial value (see InitGL())
static Teapot_MeshData* pAnimatedMeshData0 = NULL;
static Teapot_MeshData* pAnimatedMeshData1 = NULL;


#ifdef TEST_ADD_USER_MESH
// defined at the bottom
extern void Teapot_Init_User_Mesh_Callback(TeapotMeshEnum meshId,const float** ppverts,int* numVerts,const unsigned short** ppinds,int* numInds);
#endif

/*#define NUM_COLORS 10
static float Colors[NUM_COLORS][3]={
{0.4f,0.4f,0.f},
{0.2f,0.7f,0.4f},
{0.8f,0.2f,0.2f},
{0.4f,1.0f,0.2f},
{0.2f,0.4f,1.f},
{1.0f,0.4f,0.4f},
{1.0f,1.f,0.4f},
{0.8f,0.2f,0.8f},
{0.5f,0.7f,1.0f},
{0.65f,0.65f,0.65f}
};*/


void InitGL(void) {

    // IMPORTANT CALL--------------------------------------------------------
    Dynamic_Resolution_Init(config.dynamic_resolution_target_fps,config.dynamic_resolution_enabled);
    //-----------------------------------------------------------------------
#   ifdef TEST_ADD_USER_MESH
    Teapot_Set_Init_UserMeshCallback(&Teapot_Init_User_Mesh_Callback);  // if used must be called BEFORE Teapot_Init()
#   endif
    // IMPORTANT CALL--------------------------------------------------------
    Teapot_Init();
    //-----------------------------------------------------------------------

    // These are important, but often overlooked OpenGL calls
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Otherwise transparent objects are not displayed correctly
    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);    // Only needed for ffp, when VISUALIZE_DEPTH_TEXTURE is defined

#   ifdef TEAPOT_SHADER_FOG
    // We use fog to prevent ad clipping artifacts, so it just needs the near and far plane
    Teapot_SetFogDistances((pMatrixFarPlane-pMatrixNearPlane)*0.5f,pMatrixFarPlane); // We start the fog at the half point... but it works better nearer when farPlane is far away
    Teapot_SetFogColor(0.3f, 0.6f, 1.0f); // it should be the same as glClearColor()
#   endif

#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    Teapot_SetShadowDarkening(40.f,0.75f);  // First number makes the shadow darker in an uniform way, the second clamps the lower intensity: (40.f,0.75f) default, (0.f,...) -> no shadows
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP

    // This line can change the look of the demo considerably. Try commenting it out!
    Teapot_Enable_ColorMaterial();    // Optional (Teapot_SetColor(...) internally calls Teapot_SetColorAmbient(...); and Teapot_SetColorSpecular(...); when enabled)
    // The truth is that for every object it's tedious to setup 3 colors (even if we'd get better visuals): so this definition is useful.

    // Warning: in this demo we know that the calling order of the callbacks is: InitGL(),ResizeGL(...),DrawGL().
    // That's way we can avoid calling ResizeGL(...) here
    // However in other demos it might be mandatory to call ResizeGL(...) here

    // Allocate meshes
    {
        int i;
        allocated_memory = (Teapot_MeshData*) malloc(maxNumMeshData*sizeof(Teapot_MeshData));
        for (i=0;i<maxNumMeshData;i++) {
            pMeshData[i] = &allocated_memory[i];    // Mandatory Assignment (to split our allocated memory)
            Teapot_MeshData_Clear(pMeshData[i]);    // Reset
        }
    }
    // Initialize meshes
    {
        float mMatrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        int i;Teapot_MeshData* md;
        //-----------------------------------------------------
        i=0;
        // Ground mesh (box)
        md = pMeshData[i++];
        mMatrix[12]=0.0;    mMatrix[13]=-0.25;    mMatrix[14]=0.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,5.f,0.25f,5.f);
        Teapot_MeshData_SetColor(md,0.1f,0.6f,0.1f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CUBIC_GROUND);

        // (teapot)
        pAnimatedMeshData0 = md = pMeshData[i++];   // stored for position animation in DrawGL()
        mMatrix[12]=1.5;    mMatrix[13]=0.0;    mMatrix[14]=-0.5;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,1.f,1.f,1.f);
        Teapot_MeshData_SetColor(md,1.f,0.4f,0.2f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_TEAPOT);
        Teapot_MeshData_SetOutlineEnabled(md,0);

        // (bunny)
        md = pMeshData[i++];
        mMatrix[12]=-0.5;    mMatrix[13]=0.0;    mMatrix[14]=1.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,1.f,1.f,1.f);
        Teapot_MeshData_SetColor(md,0.2f,0.4f,0.9f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_BUNNY);
        Teapot_MeshData_SetOutlineEnabled(md,0);

        // (teapot)
        md = pMeshData[i++];
        mMatrix[12]=0.25;    mMatrix[13]=0.0;    mMatrix[14]=1.5;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,0.5f,0.5f,0.5f);
        Teapot_MeshData_SetColor(md,0.8f,0.6f,0.2f,0.6f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_TEAPOT);

        // (ghost)
        pAnimatedMeshData1 = md = pMeshData[i++];   // stored for position animation in DrawGL()
        mMatrix[12]=-1.5;    mMatrix[13]=0.0;    mMatrix[14]=1.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,0.5f,0.5f,0.5f);
        Teapot_MeshData_SetColor(md,0.3f,0.5f,1.0f,0.5f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_GHOST);

        // (character)
        md = pMeshData[i++];
        mMatrix[12]=-0.375;    mMatrix[13]=0.0;    mMatrix[14]=-0.35;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,1.f,1.f,1.f);
        Teapot_MeshData_SetColor(md,1.0f,0.8f,0.25f,1.0f);
#       ifdef TEST_ADD_USER_MESH
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_USER_00);
#       else
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CHARACTER);
#       endif

        // (torus)
        md = pMeshData[i++];
        mMatrix[12]=0.3;    mMatrix[13]=0.0;    mMatrix[14]=0.5;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,1.f,1.5f,1.25f);
        Teapot_MeshData_SetColor(md,0.5f,0.75f,1.0f,0.65f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_TORUS);

        // (table)
        md = pMeshData[i++];
        Teapot_MeshData_SetScaling(md,0.6f,0.5f,0.5f);
        Teapot_MeshData_SetColor(md,0.5f,0.2f,0.1f,1.0f);
        mMatrix[12]=-1.25;   mMatrix[13]=0.0;    mMatrix[14]=-1.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_TABLE);

        // (chairs)
        {
            int j;
            for (j=0;j<3;j++) {
                md = pMeshData[i++];
                Teapot_MeshData_SetScaling(md,0.5f,0.5f,0.5f);
                Teapot_MeshData_SetColor(md,0.5f,0.2f,0.1f,1.0f);
                mMatrix[13]=0.0;    mMatrix[14]=-1.3;
                mMatrix[12]=-1.25f+(float)(j-1)*0.45;
                Teapot_MeshData_SetMMatrix(md,mMatrix);
                Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CHAIR);
            }
            // With these 2 lines, we can achieve a 180° rotation for free
            mMatrix[8]=-mMatrix[8];mMatrix[9]=-mMatrix[9];mMatrix[10]=-mMatrix[10];
            mMatrix[0]=-mMatrix[0];mMatrix[1]=-mMatrix[1];mMatrix[2]=-mMatrix[2];
            for (j=0;j<3;j++) {
                md = pMeshData[i++];
                Teapot_MeshData_SetScaling(md,0.5f,0.5f,0.5f);
                Teapot_MeshData_SetColor(md,0.5f,0.2f,0.1f,1.0f);
                mMatrix[13]=0.0;    mMatrix[14]=-0.7;
                mMatrix[12]=-1.25f+(float)(j-1)*0.45;
                Teapot_MeshData_SetMMatrix(md,mMatrix);
                Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CHAIR);
            }
            mMatrix[0]=-mMatrix[0];mMatrix[1]=-mMatrix[1];mMatrix[2]=-mMatrix[2];
            mMatrix[8]=-mMatrix[8];mMatrix[9]=-mMatrix[9];mMatrix[10]=-mMatrix[10];
        }

        // (teapot)
        md = pMeshData[i++];
        Teapot_MeshData_SetScaling(md,0.15f,0.15f,0.15f);
        Teapot_MeshData_SetColor(md,0.4f,0.8f,0.2f,1.0f);
        mMatrix[12]=-1.25;   mMatrix[13]=0.5;    mMatrix[14]=-1.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_TEAPOT);


        numMeshData = i;    // Well, we should check that numMeshData<maxNumMeshData
    }

}

void DestroyGL() {
    // Deallocate memory
    int i;
    if (allocated_memory) {free(allocated_memory);allocated_memory=NULL;}
    for (i=0;i<maxNumMeshData;i++) pMeshData[i]=NULL;

    // IMPORTANT CALLS-------------------------------------------------------
    Teapot_Destroy();
    Dynamic_Resolution_Destroy();
    //-----------------------------------------------------------------------
}


//#define TEST_DYN_RES_FOR_SHADOW_MAP   // This just works on my Ubuntu system by adding some wait time every frame to force dynamic resolution to trigger in
#ifdef TEST_DYN_RES_FOR_SHADOW_MAP
#include <unistd.h> // usleep
#endif //TEST_DYN_RES_FOR_SHADOW_MAP

void DrawGL(void) 
{	
    // We need to calculate the "instantFrameTime", because it's necessary to "dynamic_resolution.h"
    static unsigned begin = 0;
    static unsigned cur_time = 0;
    unsigned elapsed_time,delta_time;
    if (begin==0) begin = glutGet(GLUT_ELAPSED_TIME);
    elapsed_time = glutGet(GLUT_ELAPSED_TIME) - begin;
    delta_time = elapsed_time - cur_time;
    instantFrameTime = (float)delta_time*0.001f;
    cur_time = elapsed_time;

    // view Matrix
    LookAt(vMatrix,cameraPos[0],cameraPos[1],cameraPos[2],targetPos[0],targetPos[1],targetPos[2],0,1,0);
    Teapot_SetViewMatrixAndLightDirection(vMatrix,lightDirection);  // we must notify teapot.h, and we also pass the lightDirection here

    // Animate some objects (
    {
        const float s = sin(cur_time*0.002f);
        const float c = cos(cur_time*0.001f);

        pAnimatedMeshData0->mMatrix[12] = 1.5f + 0.3f*s;
        pAnimatedMeshData0->mMatrix[14] = 0.5f + 1.25f*c;

        pAnimatedMeshData1->mMatrix[12] = -1.5f + 0.5f*c;
        pAnimatedMeshData1->mMatrix[14] = 1.f + 0.25f*s;
    }

    Teapot_MeshData_CalculateMvMatrixFromArray(pMeshData,numMeshData);  // This sets every Teapot_MeshData::mvMatrix

    // Draw to Shadow Map------------------------------------------------------------------------------------------
    {
    // Note: we could just skip this if TEAPOT_SHADER_USE_SHADOW_MAP is not defined,
    // but this part can still be done because it's part of "dynamic_resolution.h",
    // and the shadow map texture can still be created and displayed.

    // We're currently calculating all these matrices every frame. This is obviously wrong.
    // Also: there's no fixed rule I know to calculate these matrices. Feel free to change them!
    static float lpMatrix[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static float lvMatrix[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static float lvpMatrix[16]; // = light_pMatrix*light_vMatrix
    int i;
    //if (lpMatrix[0]==0)
    {
        // This changes with pMatrixFarPlane and pMatrixFovDeg
        const float y = pMatrixFarPlane*tan(pMatrixFovDeg*3.1415/180.0)*0.5f;  // last coefficient is ad-hoc for this demo (in our case it should be 1.0, or maybe 0.5 for free roaming; something like 0.2 for fixed environment and MUCH better shadow quality!)
        const float x = y;
        Ortho(lpMatrix,-x,x,-y,y,pMatrixFarPlane*0.5f,-pMatrixFarPlane*0.5f);
/* POSSIBLE IMPROVEMENTS (https://msdn.microsoft.com/en-us/library/windows/desktop/ee416324(v=vs.85).aspx):
 To calculate the projection, the eight points that make up the view
 frustum are transformed into light space. Next, the minimum and maximum
 values in X and Z are found. These values make up the bounds for an
 orthographic projection [-x,x,-y,y].

 For directional lights, the solution to the moving shadow edges problem is to round the minimum/maximum value in X and Y (that make up the orthographic projection bounds) to pixel size increments. This can be done with a divide operation, a floor operation, and a multiply.
    vLightCameraOrthographicMin /= vWorldUnitsPerTexel;
    vLightCameraOrthographicMin = XMVectorFloor( vLightCameraOrthographicMin );
    vLightCameraOrthographicMin *= vWorldUnitsPerTexel;
    vLightCameraOrthographicMax /= vWorldUnitsPerTexel;
    vLightCameraOrthographicMax = XMVectorFloor( vLightCameraOrthographicMax );
    vLightCameraOrthographicMax *= vWorldUnitsPerTexel;

 The vWorldUnitsPerTexel value is calculated by taking a bound of
 the view frustum, and dividing by the buffer size.

        FLOAT fWorldUnitsPerTexel = fCascadeBound /
        (float)m_CopyOfCascadeConfig.m_iBufferSize;
        vWorldUnitsPerTexel = XMVectorSet( fWorldUnitsPerTexel, fWorldUnitsPerTexel, 						   	0.0f, 0.0f );

Bounding the maximum size of the view frustum results in a looser fit for the orthographic projection.
It is important to note that the texture is 1 pixel larger in width and height when using this technique. This keeps shadow coordinates from indexing outside of the shadow map.

*/
/*
Building the matrix:

How I'd go about the very simple case you have:
You have an orthographic projection for your light-camera. So your frustum is just an Oriented Bounding Box (OBB). That means you can simply feed its world space coordinates (like the width and height) to glm::ortho().

But how do you construct the OBB around the frustum?
Good question. ;D

Here's a simple approach:
First determine the direction of your light (as a normalized vector). Now simply project every vertex of your main-camera's frustum onto that vector and find the nearest and furthest one (simply store the distances along the vector). Now subtract the two distances.
Congratulations! You already have your OBB's depth (Z).
Now repeat that process for the other two vectors. One pointing upwards or downwards (Y) and the other to the right or left (X) relative to your light-camera. Now you have your OBB's orientation (the three vectors) and dimensions. Now simply pass the OBB's dimensions to glm::ortho() and then transform the orthographic matrix so it has the same orientation as your OBB.
You're done. :D

Projecting a point onto a vector:
This step is actually very easy. Just take the dot product between your vector and your point (both stored as vec3).
Example code:

float distance_on_vector = dot(p, vector);

Vector should be normalized, because you need the world-space distance. You don't need the actual position of p in world space (you just need the projected length) to calculate the dimensions of the OBB. That's why the above code is enough.
*/
// https://gamedev.stackexchange.com/questions/73851/how-do-i-fit-the-camera-frustum-inside-directional-light-space
/*
    Calculate the 8 corners of the view frustum in world space. This can be done by using the inverse view-projection matrix to transform the 8 corners of the NDC cube (which in OpenGL is [‒1, 1] along each axis).

    Transform the frustum corners to a space aligned with the shadow map axes. This would commonly be the directional light object's local space. (In fact, steps 1 and 2 can be done in one step by combining the inverse view-projection matrix of the camera with the inverse world matrix of the light.)

    Calculate the bounding box of the transformed frustum corners. This will be the view frustum for the shadow map.

    Pass the bounding box's extents to glOrtho or similar to set up the orthographic projection matrix for the shadow map.

There are a couple caveats with this basic approach. First, the Z bounds for the shadow map will be tightly fit around the view frustum, which means that objects outside the view frustum, but between the view frustum and the light, may fall outside the shadow frustum. This could lead to missing shadows. To fix this, depth clamping can be enabled so that objects in front of the shadow frustum will be rendered with clamped Z instead of clipped. Alternatively, the Z-near of the shadow frustum can be pushed out to ensure any possible shadowers are included.

The bigger issue is that this produces a shadow frustum that continuously changes size and position as the camera moves around. This leads to shadows "swimming", which is a very distracting artifact. In order to fix this, it's common to do the following additional two steps:

    Fix the overall size of the frustum based on the longest diagonal of the camera frustum. This ensures that the camera frustum can fit into the shadow frustum in any orientation. Don't allow the shadow frustum to change size as the camera rotates.

    Discretize the position of the frustum, based on the size of texels in the shadow map. In other words, if the shadow map is 1024×1024, then you only allow the frustum to move around in discrete steps of 1/1024th of the frustum size. (You also need to increase the size of the frustum by a factor of 1024/1023, to give room for the shadow frustum and view frustum to slip against each other.)

If you do these, the shadow will remain rock solid in world space as the camera moves around. (It won't remain solid if the camera's FOV, near or far planes are changed, though.)

As a bonus, if you do all the above, you're well on your way to implementing cascaded shadow maps, which are "just" a set of shadow maps calculated from the view frustum as above, but using different view frustum near and far plane values to place each shadow map.
*/
    }
    //if (lmvMatrix[15]==0)
    {
        // This changes with lightDirection, pMatrixFarPlane, targetPos (= camera target)
        const float distance =  pMatrixFarPlane*0.1f;
        const float shadowTargetPos[3] = {targetPos[0],0.f,targetPos[2]};   // We keep it at y=0
        const float lpos[3] = {shadowTargetPos[0]-lightDirection[0]*distance,
                               shadowTargetPos[1]-lightDirection[1]*distance,
                               shadowTargetPos[2]-lightDirection[2]*distance};
        LookAt(lvMatrix,lpos[0],lpos[1],lpos[2],shadowTargetPos[0],shadowTargetPos[1],shadowTargetPos[2],0,1,0);

        Teapot_Helper_MultMatrix(lvpMatrix,lpMatrix,lvMatrix);
    }


    Dynamic_Resolution_Bind_Shadow();   // Binds the shadow map FBO and its shader program
    glClear(GL_DEPTH_BUFFER_BIT);
    Dynamic_Resolution_Shadow_Set_VpMatrix(lvpMatrix);  // lvpMatrix is good if we can use mMatrix below. If we MUST use mvMatrix below, here we must pass (lvpMatrix * cameraViewMatrixInverse). Please see Dynamic_Resolution_MultMatrix(...) and Teapot_GetViewMatrixInverse(...) methods.
    Teapot_LowLevel_BindVertexBufferObject();
    for (i=1;i<numMeshData;i++) {
        // We can skip i==0: ground plane does not cast any shadow
        const Teapot_MeshData* md = pMeshData[i];
        if (md->color[3]>=0.5f && md->meshId<TEAPOT_MESH_PIVOT3D)
        {
            // We could enable frustum culling here too...
            // but we don't... it's something manual...
            // (we must extract the shadow frustum planes and then call Teapot_Helper_IsVisible(...).
            // Please see Teapot_Helper_GetFrustumPlaneEquations(...);)

            if (md->meshId==TEAPOT_MESH_CAPSULE) {
                // unluckily TEAPOT_MESH_CAPSULE is special... sorry!
                // We don't want to draw "scaled" capsules. Instead we want to regenerate valid capsules, reinterpreting Teapot_SetScaling(...)
#               if (!defined(TEAPOT_NO_MESH_CYLINDER) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_UP) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_DOWN))
                const float height = md->scaling[1];
                const float diameter = (md->scaling[0]+md->scaling[2])*0.5f;
                const float yAxis[3] = {md->mMatrix[4],md->mMatrix[5],md->mMatrix[6]};
                float tmp,mat[16];
                float origin[3] = {md->mMatrix[12],md->mMatrix[13],md->mMatrix[14]};
                float center[3];Teapot_GetMeshAabbCenter(TEAPOT_MESH_CYLINDER_LATERAL_SURFACE,center);
                center[1]=-center[1];   //
                origin[1]-=diameter*center[1];

                memcpy(mat,md->mMatrix,sizeof(mat));
                mat[12] = origin[0];    mat[13] = origin[1];    mat[14] = origin[2];
                Dynamic_Resolution_Shadow_Set_Scaling(diameter,height,diameter);
                Dynamic_Resolution_Shadow_Set_MMatrix(mat); // mMatrix here (or mvMatrix if we had called above Dynamic_Resolution_Shadow_Set_VpMatrix(lCombined * cameraViewMatrixInverse);)
                Teapot_LowLevel_DrawElements(TEAPOT_MESH_CYLINDER_LATERAL_SURFACE);

                // TEAPOT_MESH_HALF_SPHERE_UP and TEAPOT_MESH_HALF_SPHERE_DOWN are not affected by TEAPOT_CENTER_MESHES_ON_FLOOR
                Dynamic_Resolution_Shadow_Set_Scaling(diameter,diameter,diameter);
                tmp = height*(0.5f-center[1]);
                mat[12] = origin[0] + yAxis[0]*tmp;
                mat[13] = origin[1] + yAxis[1]*tmp;
                mat[14] = origin[2] + yAxis[2]*tmp;
                Dynamic_Resolution_Shadow_Set_MMatrix(mat); // mMatrix here (or mvMatrix if we had called above Dynamic_Resolution_Shadow_Set_VpMatrix(lCombined * cameraViewMatrixInverse);)
                Teapot_LowLevel_DrawElements(TEAPOT_MESH_HALF_SPHERE_UP);

                tmp = -height*(0.5f+center[1]);
                mat[12] = origin[0] + yAxis[0]*tmp;
                mat[13] = origin[1] + yAxis[1]*tmp;
                mat[14] = origin[2] + yAxis[2]*tmp;
                Dynamic_Resolution_Shadow_Set_MMatrix(mat); // mMatrix here (or mvMatrix if we had called above Dynamic_Resolution_Shadow_Set_VpMatrix(lCombined * cameraViewMatrixInverse);)
                Teapot_LowLevel_DrawElements(TEAPOT_MESH_HALF_SPHERE_DOWN);
#               endif // !defined(...)
                continue;
            }

            Dynamic_Resolution_Shadow_Set_MMatrix(md->mMatrix); // mMatrix here (or mvMatrix if we had called above Dynamic_Resolution_Shadow_Set_VpMatrix(lvpMatrix * cameraViewMatrixInverse);)
            Dynamic_Resolution_Shadow_Set_Scaling(md->scaling[0],md->scaling[1],md->scaling[2]);
            Teapot_LowLevel_DrawElements(md->meshId);
        }
    }
    Teapot_LowLevel_UnbindVertexBufferObject();
    Dynamic_Resolution_Unbind_Shadow();   // Unbinds the shadow map FBO and its shader program

#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    Teapot_SetShadowVpMatrix(lvpMatrix);    // Needed for the second shadowing pass (enabled with the definition: TEAPOT_SHADER_USE_SHADOW_MAP). Note that here we can't pass (lvpMatrix * cameraViewMatrixInverse), but just lvpMatrix (because the multiplication happens internally).
    Teapot_SetShadowMapFactor(Dynamic_Resolution_GetShadowMapDynResFactor());   // The shadow map has dynamic resolution too. That means that in the shader used in "teapot.h" there's an additional float uniform that must be updated from "dynamic_resolution.h"
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP
    }


    // Render to framebuffer---------------------------------------------------------------------------------------
    Dynamic_Resolution_Bind();  // This defaults to nothing if we don't use dynamic resolution (-> it's for free: we can draw inside it as usual)

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Teapot_PreDraw();

#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    glBindTexture(GL_TEXTURE_2D,Dynamic_Resolution_Get_Shadow_Texture_ID());    // For the 2nd part of the shadow map algorithm
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP

    // We can add an aabb frame around any object
    /*{
        const Teapot_MeshData* md = pAnimatedMeshData0;
        static float aabb[3];
        Teapot_GetMeshAabbExtents(md->meshId,aabb);
        Teapot_SetColor(0.4,0.4,0,1);
        Teapot_SetScaling(aabb[0]*md->scaling[0],aabb[1]*md->scaling[1],aabb[2]*md->scaling[2]);
        glLineWidth(4.f);
        // Hp) We have already md->mvMatrix. We do have it, because we're calling Teapot_DrawMulti_Mv(...) below.
        // Otherwise, we can just call Teapot_Draw(md->mMatrix,TEAPOT_MESHLINES_CUBE_EDGES),
        // Or better we can move this snippet below Teapot_DrawMulti(pMeshData,numMeshData,1),
        // because Teapot_DrawMulti(...) internally sets all the mvMatrices and then calls Teapot_DrawMulti_Mv(pMeshData,numMeshData,1).
        Teapot_Draw_Mv(md->mvMatrix,TEAPOT_MESHLINES_CUBE_EDGES);
    }*/

    // Here we draw all our pMeshData
    Teapot_DrawMulti_Mv(pMeshData,numMeshData,1);    // Here we don't use Teapot_DrawMulti(...) cause we got the MvMatrices already (see Teapot_MeshData_CalculateMvMatrixFromArray(pMeshData,numMeshData); above)
                                                     // This way we could have used mvMatrices in the shadow map creation (we haven't done it AFAIR)
                                                     // Please note that to handle transparent objects correctly, it can change the object order (see last argument). So to detect an object, just store pointers and don't realloc the initial buffer (see that we have maxNumMeshData>=numMeshData)

    // We can add a pivot at the camera target point
    /*{
        static float mMatrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        mMatrix[12]=targetPos[0];mMatrix[13]=targetPos[1];mMatrix[14]=targetPos[2];
        Teapot_SetColor(1,1,0.5,1); // Just the color of the center of the pivot
        Teapot_SetScaling(0.4,0.4,0.4);
        //Teapot_Enable_MeshOutline();  // This does not work together with glDisable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        Teapot_Draw(mMatrix,TEAPOT_MESH_PIVOT3D);
        glEnable(GL_DEPTH_TEST);
        //Teapot_Disable_MeshOutline();
    }*/

#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    glBindTexture(GL_TEXTURE_2D,0);
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP

    Teapot_PostDraw();
    
	Dynamic_Resolution_Unbind();	

    // Draw to screen at current resolution_factor: ----------------------------------------------------------------
    glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDepthMask(GL_FALSE);
    Dynamic_Resolution_Render(instantFrameTime);    // Mandatory
    glEnable(GL_DEPTH_TEST);glEnable(GL_CULL_FACE);glDepthMask(GL_TRUE);
    //--------------------------------------------------------------------------------------------------------------


    if (config.show_fps) {
        if ((elapsed_time/1000)%2==0)   {
            printf("%s\n",Dynamic_Resolution_GetInfoString());fflush(stdout);
            config.show_fps=0;
        }
    }


#   ifdef VISUALIZE_DEPTH_TEXTURE
#   ifndef __EMSCRIPTEN__
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D,Dynamic_Resolution_Get_Shadow_Texture_ID());
        glColor4f(1,1,1,0.9f);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(-1,    -1);
        glTexCoord2f(1,0);glVertex2f(-0.25*current_aspect_ratio, -1);
        glTexCoord2f(1,1);glVertex2f(-0.25*current_aspect_ratio, -0.25/current_aspect_ratio);
        glTexCoord2f(0,1);glVertex2f(-1,    -0.25/current_aspect_ratio);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);
        glDisable(GL_BLEND);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
    }
#   endif //__EMSCRIPTEN__
#   endif //VISUALIZE_DEPTH_TEXTURE

#   ifdef TEST_DYN_RES_FOR_SHADOW_MAP
    {
    static unsigned waitTime = 30000;
    usleep(waitTime);
    if (Dynamic_Resolution_GetShadowMapDynResFactor()<=0.2f) waitTime=25000;
    }
#   endif //TEST_DYN_RES_FOR_SHADOW_MAP

}

static void GlutDestroyWindow(void);
static void GlutCreateWindow();

void GlutCloseWindow(void)  {
#ifndef __EMSCRIPTEN__
Config_Save(&config,ConfigFileName);
#endif //__EMSCRIPTEN__
}

void GlutNormalKeys(unsigned char key, int x, int y) {
    const int mod = glutGetModifiers();
    switch (key) {
#	ifndef __EMSCRIPTEN__	
    case 27: 	// esc key
        Config_Save(&config,ConfigFileName);
        GlutDestroyWindow();
#		ifdef __FREEGLUT_STD_H__
        glutLeaveMainLoop();
#		else
        exit(0);
#		endif
        break;
    case 13:	// return key
    {
        if (mod&GLUT_ACTIVE_CTRL) {
            config.fullscreen_enabled = gameModeWindowId ? 0 : 1;
            GlutDestroyWindow();
            GlutCreateWindow();
        }
    }
        break;
#endif //__EMSCRIPTEN__	
    }

}

static void updateCameraPos() {
    const float distanceY = sin(cameraPitch)*cameraDistance;
    const float distanceXZ = cos(cameraPitch)*cameraDistance;
    cameraPos[0] = targetPos[0] + sin(cameraYaw)*distanceXZ;
    cameraPos[1] = targetPos[1] + distanceY;
    cameraPos[2] = targetPos[2] + cos(cameraYaw)*distanceXZ;
}

static void resetCamera() {
    // You can set the initial camera position here through:
    targetPos[0]=0; targetPos[1]=0; targetPos[2]=0; // The camera target point
    cameraYaw = 2*M_PI;                             // The camera rotation around the Y axis
    cameraPitch = M_PI*0.125f;                      // The camera rotation around the XZ plane
    cameraDistance = 5;                             // The distance between the camera position and the camera target point

    updateCameraPos();
}

void GlutSpecialKeys(int key,int x,int y)
{
    const int mod = glutGetModifiers();
    if (!(mod&GLUT_ACTIVE_CTRL))	{
        switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            cameraYaw+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_LEFT ? -4.0f : 4.0f);
            if (cameraYaw>M_PI) cameraYaw-=2*M_PI;
            else if (cameraYaw<=-M_PI) cameraYaw+=2*M_PI;
            updateCameraPos();		break;
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            cameraPitch+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_UP ? 2.f : -2.f);
            if (cameraPitch>M_PI-0.001f) cameraPitch=M_PI-0.001f;
            else if (cameraPitch<-M_PI*0.05f) cameraPitch=-M_PI*0.05f;
            updateCameraPos();
            break;
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            cameraDistance+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_PAGE_DOWN ? 25.0f : -25.0f);
            if (cameraDistance<1.f) cameraDistance=1.f;
            updateCameraPos();
            break;
        case GLUT_KEY_F1:
            config.dynamic_resolution_enabled = !config.dynamic_resolution_enabled;
            printf("dynamic_resolution_enabled: %s.\n",config.dynamic_resolution_enabled?"ON":"OFF");fflush(stdout);
            Dynamic_Resolution_ToggleEnabled();
            break;
        case GLUT_KEY_F2:
            config.show_fps = !config.show_fps;
            //printf("showFPS: %s.\n",config.show_fps?"ON":"OFF");fflush(stdout);
            break;
        case GLUT_KEY_HOME:
            // Reset the camera
            resetCamera();
            break;
        }
    }
    else if (mod&GLUT_ACTIVE_CTRL) {
        switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        {
            // Here we move targetPos and cameraPos at the same time

            // We must find a pivot relative to the camera here (ignoring Y)
            float forward[3] = {targetPos[0]-cameraPos[0],0,targetPos[2]-cameraPos[2]};
            float up[3] = {0,1,0};
            float left[3];

            Vec3Normalize(forward);
            Vec3Cross(left,up,forward);
            {
                float delta[3] = {0,0,0};int i;
                if (key==GLUT_KEY_LEFT || key==GLUT_KEY_RIGHT) {
                    float amount = instantFrameTime*cameraSpeed*(key==GLUT_KEY_RIGHT ? -25.0f : 25.0f);
                    for (i=0;i<3;i++) delta[i]+=amount*left[i];
                }
                else {
                    float amount = instantFrameTime*cameraSpeed*(key==GLUT_KEY_DOWN ? -25.0f : 25.0f);
                    for ( i=0;i<3;i++) delta[i]+=amount*forward[i];
                }
                for ( i=0;i<3;i++) {
                    targetPos[i]+=delta[i];
                    cameraPos[i]+=delta[i];
                }
            }
        }
        break;
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            // We use world space coords here.
            targetPos[1]+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_PAGE_DOWN ? -25.0f : 25.0f);
            if (targetPos[1]<-50.f) targetPos[1]=-50.f;
            else if (targetPos[1]>500.f) targetPos[1]=500.f;
            updateCameraPos();
        break;
        }
    }
}

void GlutMouse(int a,int b,int c,int d) {

}

// Note that we have used GlutFakeDrawGL() so that at startup
// the calling order is: InitGL(),ResizeGL(...),DrawGL()
// Also note that glutSwapBuffers() must NOT be called inside DrawGL()
static void GlutDrawGL(void)		{DrawGL();glutSwapBuffers();}
static void GlutIdle(void)			{glutPostRedisplay();}
static void GlutFakeDrawGL(void) 	{glutDisplayFunc(GlutDrawGL);}
void GlutDestroyWindow(void) {
    if (gameModeWindowId || windowId)	{
        DestroyGL();

        if (gameModeWindowId) {
#           ifndef __EMSCRIPTEN__
            glutLeaveGameMode();
#           endif
            gameModeWindowId = 0;
        }
        if (windowId) {
            glutDestroyWindow(windowId);
            windowId=0;
        }
    }
}
void GlutCreateWindow() {
    GlutDestroyWindow();
#   ifndef __EMSCRIPTEN__
    if (config.fullscreen_enabled)	{
        char gms[16]="";
        if (config.fullscreen_width>0 && config.fullscreen_height>0)	{
            sprintf(gms,"%dx%d:32",config.fullscreen_width,config.fullscreen_height);
            glutGameModeString(gms);
            if (glutGameModeGet (GLUT_GAME_MODE_POSSIBLE)) gameModeWindowId = glutEnterGameMode();
            else config.fullscreen_width=config.fullscreen_height=0;
        }
        if (gameModeWindowId==0)	{
            const int screenWidth = glutGet(GLUT_SCREEN_WIDTH);
            const int screenHeight = glutGet(GLUT_SCREEN_HEIGHT);
            sprintf(gms,"%dx%d:32",screenWidth,screenHeight);
            glutGameModeString(gms);
            if (glutGameModeGet (GLUT_GAME_MODE_POSSIBLE)) gameModeWindowId = glutEnterGameMode();
        }
    }
#   endif
    if (!gameModeWindowId) {
        config.fullscreen_enabled = 0;
        glutInitWindowPosition(100,100);
        glutInitWindowSize(config.windowed_width,config.windowed_height);
        windowId = glutCreateWindow("test_shadows.c");
    }

    glutKeyboardFunc(GlutNormalKeys);
    glutSpecialFunc(GlutSpecialKeys);
    glutMouseFunc(GlutMouse);
    glutIdleFunc(GlutIdle);
    glutReshapeFunc(ResizeGL);
    glutDisplayFunc(GlutFakeDrawGL);
#   ifdef __FREEGLUT_STD_H__
#   ifndef __EMSCRIPTEN__
    glutWMCloseFunc(GlutCloseWindow);
#   endif //__EMSCRIPTEN__
#   endif //__FREEGLUT_STD_H__

#ifdef USE_GLEW
    {
        GLenum err = glewInit();
        if( GLEW_OK != err ) {
            fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err) );
            return;
        }
    }
#endif //USE_GLEW


    InitGL();

}


int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#ifndef __EMSCRIPTEN__
    //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
#ifdef __FREEGLUT_STD_H__
    glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;
#endif //__FREEGLUT_STD_H__
#endif //__EMSCRIPTEN__

    Config_Init(&config);
#ifndef __EMSCRIPTEN__
    Config_Load(&config,ConfigFileName);
#endif //__EMSCRIPTEN__

    GlutCreateWindow();

    //OpenGL info
    printf("\nGL Vendor: %s\n", glGetString( GL_VENDOR ));
    printf("GL Renderer : %s\n", glGetString( GL_RENDERER ));
    printf("GL Version (string) : %s\n",  glGetString( GL_VERSION ));
    printf("GLSL Version : %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ));
    //printf("GL Extensions:\n%s\n",(char *) glGetString(GL_EXTENSIONS));

    printf("\nKEYS:\n");
    printf("AROW KEYS + PAGE_UP/PAGE_DOWN:\tmove camera (optionally with CTRL down)\n");
    printf("HOME KEY:\t\t\treset camera\n");
    printf("F1:\t\t\ttoggle dynamic resolution on/off\n");
#	ifndef __EMSCRIPTEN__
    printf("CTRL+RETURN:\t\ttoggle fullscreen on/off\n");
#	endif //__EMSCRIPTEN__
    printf("F2:\t\t\tdisplay FPS\n");
    printf("\n");

    resetCamera();  // Mandatory

    glutMainLoop();


    return 0;
}


#ifdef TEST_ADD_USER_MESH
void Teapot_Init_User_Mesh_Callback(TeapotMeshEnum meshId,const float** ppverts,int* numVerts,const unsigned short** ppinds,int* numInds) {
    switch ((int)meshId) {
    case TEAPOT_MESH_USER_00:   {
        // Susanne model (copyright by Blender)
        // Tip: almost all the other meshes are 1 unit tall and centered in (0,0,0)
        static const float verts[] = {
            0.222222,0.083333,0.388889,-0.222222,0.083333,0.388889,0.253968,0.047619,0.349206,-0.253968,0.047619,0.349206,0.277778,0.027778,0.293651,-0.277778,0.027778,0.293651,0.178571,-0.011905,0.313492,-0.178571,-0.011905,0.313492,
            0.178571,0.015873,0.365079,-0.178571,0.015873,0.365079,0.178571,0.067460,0.396825,-0.178571,0.067460,0.396825,0.138889,0.083333,0.404762,-0.138889,0.083333,0.404762,0.103175,0.047619,0.376984,-0.103175,0.047619,0.376984,
            0.079365,0.027778,0.329365,-0.079365,0.027778,0.329365,0.039683,0.123016,0.333333,-0.039683,0.123016,0.333333,0.071429,0.123016,0.376984,-0.071429,0.123016,0.376984,0.123016,0.123016,0.404762,-0.123016,0.123016,0.404762,
            0.138889,0.166667,0.404762,-0.138889,0.166667,0.404762,0.103175,0.198413,0.376984,-0.103175,0.198413,0.376984,0.079365,0.222222,0.329365,-0.079365,0.222222,0.329365,0.178571,0.261905,0.313492,-0.178571,0.261905,0.313492,
            0.178571,0.230159,0.365079,-0.178571,0.230159,0.365079,0.178571,0.182540,0.396825,-0.178571,0.182540,0.396825,0.222222,0.166667,0.388889,-0.222222,0.166667,0.388889,0.253968,0.198413,0.349206,-0.253968,0.198413,0.349206,
            0.277778,0.222222,0.293651,-0.277778,0.222222,0.293651,0.317460,0.123016,0.285714,-0.317460,0.123016,0.285714,0.285714,0.123016,0.341270,-0.285714,0.123016,0.341270,0.238095,0.123016,0.384921,-0.238095,0.123016,0.384921,
            0.242063,0.123016,0.392857,-0.242063,0.123016,0.392857,0.226190,0.170635,0.396825,-0.226190,0.170635,0.396825,0.178571,0.190476,0.408730,-0.178571,0.190476,0.408730,0.134921,0.170635,0.416667,-0.134921,0.170635,0.416667,
            0.115079,0.123016,0.416667,-0.115079,0.123016,0.416667,0.134921,0.079365,0.416667,-0.134921,0.079365,0.416667,0.178571,0.123016,0.420635,-0.178571,0.123016,0.420635,0.178571,0.059524,0.408730,-0.178571,0.059524,0.408730,
            0.226190,0.079365,0.396825,-0.226190,0.079365,0.396825,0.000000,0.218254,0.376984,0.000000,0.178571,0.416667,0.000000,-0.345238,0.373016,0.000000,-0.162698,0.396825,0.000000,-0.095238,0.404762,0.000000,-0.392857,0.365079,
            0.000000,0.206349,0.305556,0.000000,0.289683,0.289683,0.000000,0.456349,-0.277778,0.000000,0.285714,-0.432540,0.000000,0.035714,-0.420635,0.000000,-0.194444,-0.178571,0.103175,-0.095238,0.285714,-0.103175,-0.095238,0.285714,
            0.158730,-0.222222,0.289683,-0.158730,-0.222222,0.289683,0.178571,-0.353175,0.289683,-0.178571,-0.353175,0.289683,0.186508,-0.452381,0.269841,-0.186508,-0.452381,0.269841,0.166667,-0.480159,0.265873,-0.166667,-0.480159,0.265873,
            0.091270,-0.492063,0.281746,-0.091270,-0.492063,0.281746,0.000000,-0.500000,0.293651,0.222222,-0.071429,0.269841,-0.222222,-0.071429,0.269841,0.321429,-0.019841,0.273809,-0.321429,-0.019841,0.273809,0.420635,0.075397,0.226190,
            -0.420635,0.075397,0.226190,0.436508,0.218254,0.301587,-0.436508,0.218254,0.301587,0.361111,0.246032,0.317460,-0.361111,0.246032,0.317460,0.250000,0.305556,0.349206,-0.250000,0.305556,0.349206,0.162698,0.384921,0.373016,
            -0.162698,0.384921,0.373016,0.079365,0.365079,0.384921,-0.079365,0.365079,0.384921,0.031746,0.250000,0.380952,-0.031746,0.250000,0.380952,0.083333,0.210317,0.392857,-0.083333,0.210317,0.392857,0.063492,0.154762,0.388889,
            -0.063492,0.154762,0.388889,0.103175,0.047619,0.376984,-0.103175,0.047619,0.376984,0.190476,0.007937,0.357143,-0.190476,0.007937,0.357143,0.250000,0.031746,0.341270,-0.250000,0.031746,0.341270,0.317460,0.095238,0.329365,
            -0.317460,0.095238,0.329365,0.325397,0.150794,0.329365,-0.325397,0.150794,0.329365,0.305556,0.190476,0.337302,-0.305556,0.190476,0.337302,0.218254,0.222222,0.365079,-0.218254,0.222222,0.365079,0.126984,0.238095,0.384921,
            -0.126984,0.238095,0.384921,0.000000,-0.388889,0.373016,0.055556,-0.365079,0.373016,-0.055556,-0.365079,0.373016,0.059524,-0.424603,0.361111,-0.059524,-0.424603,0.361111,0.031746,-0.448413,0.353175,-0.031746,-0.448413,0.353175,
            0.000000,-0.452381,0.349206,0.000000,-0.099206,0.380952,0.000000,-0.071429,0.376984,0.051587,-0.075397,0.376984,-0.051587,-0.075397,0.376984,0.063492,-0.115079,0.380952,-0.063492,-0.115079,0.380952,0.043651,-0.146825,0.376984,
            -0.043651,-0.146825,0.376984,0.202381,-0.023810,0.341270,-0.202381,-0.023810,0.341270,0.313492,0.027778,0.317460,-0.313492,0.027778,0.317460,0.369048,0.103175,0.305556,-0.369048,0.103175,0.305556,0.376984,0.190476,0.333333,
            -0.376984,0.190476,0.333333,0.349206,0.210317,0.369048,-0.349206,0.210317,0.369048,0.222222,0.277778,0.404762,-0.222222,0.277778,0.404762,0.158730,0.325397,0.424603,-0.158730,0.325397,0.424603,0.103175,0.313492,0.432540,
            -0.103175,0.313492,0.432540,0.051587,0.218254,0.428571,-0.051587,0.218254,0.428571,0.063492,-0.051587,0.412698,-0.063492,-0.051587,0.412698,0.107143,-0.226190,0.361111,-0.107143,-0.226190,0.361111,0.126984,-0.357143,0.349206,
            -0.126984,-0.357143,0.349206,0.134921,-0.416667,0.337302,-0.134921,-0.416667,0.337302,0.119048,-0.464286,0.321429,-0.119048,-0.464286,0.321429,0.083333,-0.472222,0.321429,-0.083333,-0.472222,0.321429,0.000000,-0.480159,0.325397,
            0.000000,0.023810,0.369048,0.000000,0.107143,0.388889,0.166667,0.242063,0.376984,-0.166667,0.242063,0.376984,0.083333,0.071429,0.380952,-0.083333,0.071429,0.380952,0.067460,0.107143,0.384921,-0.067460,0.107143,0.384921,
            0.059524,-0.349206,0.373016,-0.059524,-0.349206,0.373016,0.039683,-0.226190,0.380952,-0.039683,-0.226190,0.380952,0.000000,-0.226190,0.380952,0.000000,-0.166667,0.376984,0.047619,-0.138889,0.396825,-0.047619,-0.138889,0.396825,
            0.067460,-0.115079,0.404762,-0.067460,-0.115079,0.404762,0.055556,-0.067460,0.396825,-0.055556,-0.067460,0.396825,0.019841,-0.063492,0.396825,-0.019841,-0.063492,0.396825,0.000000,-0.103175,0.420635,0.023810,-0.075397,0.412698,
            -0.023810,-0.075397,0.412698,0.047619,-0.079365,0.412698,-0.047619,-0.079365,0.412698,0.055556,-0.115079,0.420635,-0.055556,-0.115079,0.420635,0.039683,-0.126984,0.408730,-0.039683,-0.126984,0.408730,0.000000,-0.146825,0.408730,
            0.130952,-0.158730,0.281746,-0.130952,-0.158730,0.281746,0.083333,-0.123016,0.361111,-0.083333,-0.123016,0.361111,0.091270,-0.158730,0.361111,-0.091270,-0.158730,0.361111,0.119048,-0.126984,0.281746,-0.119048,-0.126984,0.281746,
            0.000000,-0.444444,0.349206,0.023810,-0.440476,0.349206,-0.023810,-0.440476,0.349206,0.047619,-0.416667,0.361111,-0.047619,-0.416667,0.361111,0.047619,-0.376984,0.369048,-0.047619,-0.376984,0.369048,0.000000,-0.396825,0.333333,
            0.047619,-0.380952,0.337302,-0.047619,-0.380952,0.337302,0.047619,-0.412698,0.325397,-0.047619,-0.412698,0.325397,0.023810,-0.432540,0.321429,-0.023810,-0.432540,0.321429,0.000000,-0.436508,0.321429,0.087302,0.111111,0.396825,
            -0.087302,0.111111,0.396825,0.095238,0.079365,0.392857,-0.095238,0.079365,0.392857,0.170635,0.218254,0.384921,-0.170635,0.218254,0.384921,0.138889,0.214286,0.392857,-0.138889,0.214286,0.392857,0.214286,0.202381,0.392857,
            -0.214286,0.202381,0.392857,0.285714,0.178571,0.353175,-0.285714,0.178571,0.353175,0.297619,0.146825,0.349206,-0.297619,0.146825,0.349206,0.293651,0.099206,0.345238,-0.293651,0.099206,0.345238,0.242063,0.051587,0.365079,
            -0.242063,0.051587,0.365079,0.190476,0.031746,0.376984,-0.190476,0.031746,0.376984,0.115079,0.055556,0.396825,-0.115079,0.055556,0.396825,0.091270,0.150794,0.396825,-0.091270,0.150794,0.396825,0.107143,0.190476,0.396825,
            -0.107143,0.190476,0.396825,0.119048,0.182540,0.384921,-0.119048,0.182540,0.384921,0.099206,0.150794,0.384921,-0.099206,0.150794,0.384921,0.123016,0.063492,0.384921,-0.123016,0.063492,0.384921,0.190476,0.043651,0.369048,
            -0.190476,0.043651,0.369048,0.234127,0.059524,0.357143,-0.234127,0.059524,0.357143,0.277778,0.107143,0.341270,-0.277778,0.107143,0.341270,0.281746,0.142857,0.341270,-0.281746,0.142857,0.341270,0.269841,0.170635,0.345238,
            -0.269841,0.170635,0.345238,0.210317,0.198413,0.380952,-0.210317,0.198413,0.380952,0.142857,0.202381,0.388889,-0.142857,0.202381,0.388889,0.170635,0.206349,0.380952,-0.170635,0.206349,0.380952,0.103175,0.087302,0.380952,
            -0.103175,0.087302,0.380952,0.099206,0.115079,0.380952,-0.099206,0.115079,0.380952,0.055556,0.234127,0.309524,-0.055556,0.234127,0.309524,0.099206,0.337302,0.313492,-0.099206,0.337302,0.313492,0.170635,0.349206,0.301587,
            -0.170635,0.349206,0.301587,0.246032,0.281746,0.281746,-0.246032,0.281746,0.281746,0.345238,0.230159,0.250000,-0.345238,0.230159,0.250000,0.404762,0.206349,0.234127,-0.404762,0.206349,0.234127,0.392857,0.083333,0.190476,
            -0.392857,0.083333,0.190476,0.305556,0.000000,0.210317,-0.305556,0.000000,0.210317,0.222222,-0.047619,0.238095,-0.222222,-0.047619,0.238095,0.000000,0.456349,0.146825,0.000000,0.500000,-0.039683,0.000000,-0.099206,-0.341270,
            0.000000,-0.234127,0.095238,0.000000,-0.496032,0.234127,0.000000,-0.408730,0.174603,0.000000,-0.289683,0.162698,0.000000,-0.246032,0.142857,0.432540,0.119048,0.027778,-0.432540,0.119048,0.027778,0.436508,0.162698,-0.023810,
            -0.436508,0.162698,-0.023810,0.392857,0.134921,-0.222222,-0.392857,0.134921,-0.222222,0.234127,0.222222,-0.357143,-0.234127,0.222222,-0.357143,0.373016,-0.023810,0.035714,-0.373016,-0.023810,0.035714,0.301587,-0.063492,-0.083333,
            -0.301587,-0.063492,-0.083333,0.325397,-0.003968,-0.218254,-0.325397,-0.003968,-0.218254,0.170635,0.027778,-0.337302,-0.170635,0.027778,-0.337302,0.119048,-0.178571,0.206349,-0.119048,-0.178571,0.206349,0.091270,-0.210317,0.130952,
            -0.091270,-0.210317,0.130952,0.146825,-0.361111,0.194444,-0.146825,-0.361111,0.194444,0.126984,-0.253968,0.198413,-0.126984,-0.253968,0.198413,0.166667,-0.464286,0.202381,-0.166667,-0.464286,0.202381,0.071429,-0.384921,0.186508,
            -0.071429,-0.384921,0.186508,0.063492,-0.273809,0.182540,-0.063492,-0.273809,0.182540,0.083333,-0.480159,0.222222,-0.083333,-0.480159,0.222222,0.111111,-0.142857,0.218254,-0.111111,-0.142857,0.218254,0.107143,-0.115079,0.238095,
            -0.107143,-0.115079,0.238095,0.103175,-0.087302,0.253968,-0.103175,-0.087302,0.253968,0.107143,-0.198413,0.083333,-0.107143,-0.198413,0.083333,0.150794,-0.158730,-0.134921,-0.150794,-0.158730,-0.134921,0.174603,-0.075397,-0.273809,
            -0.174603,-0.075397,-0.273809,0.230159,0.440476,-0.194444,-0.230159,0.440476,-0.194444,0.230159,0.472222,-0.035714,-0.230159,0.472222,-0.035714,0.230159,0.432540,0.119048,-0.230159,0.432540,0.119048,0.234127,0.265873,0.218254,
            -0.234127,0.265873,0.218254,0.369048,0.206349,0.170635,-0.369048,0.206349,0.170635,0.321429,0.230159,0.142857,-0.321429,0.230159,0.142857,0.325397,0.357143,0.027778,-0.325397,0.357143,0.027778,0.404762,0.285714,0.063492,
            -0.404762,0.285714,0.063492,0.404762,0.313492,-0.059524,-0.404762,0.313492,-0.059524,0.325397,0.380952,-0.099206,-0.325397,0.380952,-0.099206,0.325397,0.345238,-0.226190,-0.325397,0.345238,-0.226190,0.404762,0.273809,-0.182540,
            -0.404762,0.273809,-0.182540,0.313492,0.166667,-0.297619,-0.313492,0.166667,-0.297619,0.246032,0.011905,-0.277778,-0.246032,0.011905,-0.277778,0.416667,0.166667,-0.103175,-0.416667,0.166667,-0.103175,0.206349,-0.087302,0.075397,
            -0.206349,-0.087302,0.075397,0.218254,-0.099206,-0.107143,-0.218254,-0.099206,-0.107143,0.452381,0.206349,-0.119048,-0.452381,0.206349,-0.119048,0.392857,-0.071429,-0.063492,-0.392857,-0.071429,-0.063492,0.527778,-0.051587,-0.166667,
            -0.527778,-0.051587,-0.166667,0.650794,0.027778,-0.218254,-0.650794,0.027778,-0.218254,0.686508,0.162698,-0.214286,-0.686508,0.162698,-0.214286,0.626984,0.257936,-0.214286,-0.626984,0.257936,-0.214286,0.519841,0.242063,-0.158730,
            -0.519841,0.242063,-0.158730,0.515873,0.210317,-0.146825,-0.515873,0.210317,-0.146825,0.603175,0.222222,-0.198413,-0.603175,0.222222,-0.198413,0.642857,0.146825,-0.206349,-0.642857,0.146825,-0.206349,0.615079,0.039683,-0.206349,
            -0.615079,0.039683,-0.206349,0.523809,-0.019841,-0.154762,-0.523809,-0.019841,-0.154762,0.420635,-0.035714,-0.067460,-0.420635,-0.035714,-0.067460,0.468254,0.182540,-0.111111,-0.468254,0.182540,-0.111111,0.480159,0.154762,-0.146825,
            -0.480159,0.154762,-0.146825,0.448413,-0.011905,-0.107143,-0.448413,-0.011905,-0.107143,0.527778,0.000000,-0.186508,-0.527778,0.000000,-0.186508,0.603175,0.047619,-0.226190,-0.603175,0.047619,-0.226190,0.626984,0.126984,-0.226190,
            -0.626984,0.126984,-0.226190,0.595238,0.182540,-0.222222,-0.595238,0.182540,-0.222222,0.519841,0.174603,-0.182540,-0.519841,0.174603,-0.182540,0.428571,0.146825,-0.107143,-0.428571,0.146825,-0.107143,0.424603,0.087302,-0.138889,
            -0.424603,0.087302,-0.138889,0.384921,0.047619,-0.138889,-0.384921,0.047619,-0.138889,0.416667,0.043651,-0.138889,-0.416667,0.043651,-0.138889,0.428571,0.007937,-0.138889,-0.428571,0.007937,-0.138889,0.412698,-0.007937,-0.138889,
            -0.412698,-0.007937,-0.138889,0.369048,0.000000,-0.035714,-0.369048,0.000000,-0.035714,0.365079,-0.011905,-0.087302,-0.365079,-0.011905,-0.087302,0.365079,0.019841,-0.095238,-0.365079,0.019841,-0.095238,0.404762,0.103175,-0.107143,
            -0.404762,0.103175,-0.107143,0.452381,0.123016,-0.134921,-0.452381,0.123016,-0.134921,0.452381,0.119048,-0.162698,-0.452381,0.119048,-0.162698,0.412698,-0.007937,-0.162698,-0.412698,-0.007937,-0.162698,0.432540,0.007937,-0.162698,
            -0.432540,0.007937,-0.162698,0.420635,0.039683,-0.162698,-0.420635,0.039683,-0.162698,0.388889,0.047619,-0.162698,-0.388889,0.047619,-0.162698,0.428571,0.087302,-0.162698,-0.428571,0.087302,-0.162698,0.527778,0.166667,-0.210317,
            -0.527778,0.166667,-0.210317,0.603175,0.174603,-0.246032,-0.603175,0.174603,-0.246032,0.638889,0.123016,-0.250000,-0.638889,0.123016,-0.250000,0.615079,0.043651,-0.246032,-0.615079,0.043651,-0.246032,0.531746,0.000000,-0.214286,
            -0.531746,0.000000,-0.214286,0.448413,-0.007937,-0.134921,-0.448413,-0.007937,-0.134921,0.484127,0.146825,-0.174603,-0.484127,0.146825,-0.174603,0.452381,0.055556,-0.166667,-0.452381,0.055556,-0.166667,0.476190,0.031746,-0.170635,
            -0.476190,0.031746,-0.170635,0.507936,0.063492,-0.186508,-0.507936,0.063492,-0.186508,0.488095,0.087302,-0.178571,-0.488095,0.087302,-0.178571,0.515873,0.119048,-0.190476,-0.515873,0.119048,-0.190476,0.535714,0.095238,-0.194444,
            -0.535714,0.095238,-0.194444,0.563492,0.107143,-0.198413,-0.563492,0.107143,-0.198413,0.551587,0.138889,-0.198413,-0.551587,0.138889,-0.198413,0.519841,0.222222,-0.246032,-0.519841,0.222222,-0.246032,0.634921,0.238095,-0.277778,
            -0.634921,0.238095,-0.277778,0.694444,0.150794,-0.253968,-0.694444,0.150794,-0.253968,0.666667,0.027778,-0.269841,-0.666667,0.027778,-0.269841,0.527778,-0.043651,-0.250000,-0.527778,-0.043651,-0.250000,0.400794,-0.063492,-0.166667,
            -0.400794,-0.063492,-0.166667,0.436508,0.194444,-0.194444,-0.436508,0.194444,-0.194444};
        static const unsigned short inds[] = {
            46,2,44,3,47,45,44,4,42,5,45,43,2,6,4,7,3,5,0,8,2,9,1,3,10,14,8,15,11,9,8,16,6,17,9,7,20,16,14,21,17,19,12,20,14,21,13,15,22,26,20,27,23,21,26,18,20,27,19,29,32,28,26,
            33,29,31,34,26,24,35,27,33,36,32,34,37,33,39,38,30,32,39,31,41,44,40,38,45,41,43,46,38,36,47,39,45,36,48,46,37,49,51,34,50,36,35,51,53,24,52,34,25,53,55,22,54,24,23,55,57,22,58,56,59,23,57,
            12,62,58,63,13,59,10,64,62,65,11,63,0,48,64,49,1,65,60,64,48,49,65,61,62,64,60,61,65,63,60,58,62,63,59,61,60,56,58,59,57,61,60,54,56,57,55,61,60,52,54,55,53,61,60,50,52,53,51,61,60,48,50,
            51,49,61,173,90,88,174,90,175,171,88,86,172,89,174,84,171,86,172,85,87,82,169,84,170,83,85,80,167,82,168,81,83,78,145,163,146,79,164,93,145,91,94,146,148,93,149,147,150,94,148,97,149,95,98,150,152,99,151,97,100,152,154,
            101,153,99,102,154,156,101,157,155,158,102,156,105,157,103,106,158,160,107,159,105,108,160,162,66,161,107,66,162,67,127,161,109,128,162,160,127,157,159,158,128,160,155,178,125,156,179,158,153,125,123,154,126,156,151,123,121,152,124,154,149,121,119,
            150,122,152,147,119,117,148,120,150,145,117,115,146,118,148,163,115,113,164,116,146,113,176,163,176,114,164,161,111,109,162,112,67,111,177,182,177,112,183,180,177,176,181,177,183,134,175,173,175,135,174,132,173,171,174,133,172,132,169,130,133,170,172,
            165,184,167,185,166,168,130,167,184,168,131,185,189,186,143,189,187,188,186,68,184,187,68,188,130,68,129,131,68,185,141,190,143,191,142,144,139,192,141,193,140,142,196,139,138,197,140,195,70,138,137,70,138,197,143,69,189,144,69,191,190,207,69,
            191,207,206,70,199,196,200,70,197,196,201,194,202,197,195,201,192,194,202,193,204,192,205,190,206,193,191,203,199,198,204,200,202,198,205,203,206,198,204,138,163,176,164,138,176,139,210,163,211,140,164,143,210,141,144,211,213,186,212,143,187,213,166,
            208,165,80,209,166,213,214,212,208,215,213,211,78,210,214,211,79,215,129,221,130,129,222,71,132,221,219,222,133,220,134,219,217,220,135,218,136,217,216,218,136,216,217,230,216,218,230,229,217,226,228,227,218,229,219,224,226,225,220,227,71,224,221,
            71,225,223,223,228,224,229,223,225,224,228,226,227,229,225,182,233,231,234,183,232,111,231,253,232,112,254,111,255,109,112,256,254,113,233,180,114,234,252,113,249,251,250,114,252,115,247,249,248,116,250,117,245,247,246,118,248,119,243,245,244,120,246,
            123,243,121,124,244,242,125,241,123,126,242,240,125,235,239,236,126,240,178,237,235,238,179,236,127,255,237,256,128,238,255,275,237,256,276,258,235,275,277,276,236,278,235,273,239,236,274,278,239,271,241,240,272,274,243,271,269,272,244,270,243,267,245,
            244,268,270,247,267,265,268,248,266,247,263,249,248,264,266,249,261,251,250,262,264,233,261,279,262,234,280,255,259,257,260,256,258,253,281,259,282,254,260,231,279,281,280,232,282,66,283,72,284,66,72,107,285,283,286,108,284,103,285,105,104,286,288,
            101,287,103,102,288,290,99,289,101,100,290,292,99,293,291,294,100,292,95,293,97,96,294,296,95,297,295,298,96,296,93,299,297,300,94,298,308,337,307,308,338,328,307,335,306,307,336,338,306,339,305,306,340,336,88,305,339,305,89,340,86,339,333,
            340,87,334,84,333,329,334,85,330,82,329,331,330,83,332,329,337,331,338,330,332,333,335,329,334,336,340,331,327,325,332,328,338,80,331,325,332,81,326,341,214,208,342,215,344,325,208,80,326,209,342,214,345,78,215,346,344,345,91,78,346,92,300,
            323,303,76,324,303,352,351,77,303,352,77,350,77,347,304,348,77,304,304,327,308,328,304,308,327,341,325,328,342,348,295,317,309,318,296,310,315,76,75,316,76,324,357,302,301,358,302,356,302,353,74,354,302,74,74,315,75,316,74,75,291,361,363,
            362,292,364,363,367,365,368,364,366,365,369,371,370,366,372,371,375,373,376,372,374,377,375,313,378,376,374,315,373,377,374,316,378,353,371,373,372,354,374,355,365,371,366,356,372,357,363,365,364,358,366,291,359,289,292,360,364,359,301,73,360,301,358,
            283,287,289,288,284,290,283,359,73,360,284,73,72,283,73,73,284,72,295,361,293,296,362,310,309,367,361,368,310,362,311,369,367,370,312,368,375,381,313,376,382,370,349,383,347,350,384,386,383,319,317,384,320,386,297,383,317,384,298,318,299,341,383,
            342,300,384,341,347,383,384,348,342,299,345,343,344,346,300,321,377,313,322,378,380,377,323,315,378,324,380,385,321,319,386,322,380,351,385,349,352,386,380,323,379,351,352,380,324,387,401,399,388,402,414,399,403,397,404,400,398,403,395,397,404,396,406,
            405,393,395,406,394,408,407,391,393,408,392,410,391,411,389,412,392,390,409,417,411,418,410,412,407,419,409,420,408,410,423,407,405,424,408,422,425,405,403,426,406,424,427,403,401,428,404,426,401,415,427,416,402,428,319,441,317,320,442,444,389,443,319,
            390,444,412,309,441,311,442,310,312,381,413,387,414,382,388,411,439,443,440,412,444,445,439,437,446,440,444,433,437,435,438,434,436,447,433,431,448,434,446,447,449,429,448,450,432,429,415,413,430,416,450,311,429,381,430,312,382,441,447,311,442,448,446,
            441,443,445,446,444,442,415,451,475,452,416,476,431,451,449,432,452,462,431,459,461,460,432,462,435,459,433,436,460,458,437,457,435,438,458,456,437,453,455,454,438,456,439,473,453,474,440,454,427,475,463,476,428,464,425,463,465,464,426,466,423,465,467,
            466,424,468,423,469,421,424,470,468,421,471,419,422,472,470,419,473,417,420,474,472,455,477,457,456,478,480,479,483,477,480,484,482,483,487,485,488,484,486,487,491,485,488,492,490,463,485,491,486,464,492,483,475,451,484,476,486,461,483,451,462,484,478,
            457,461,459,462,458,460,473,455,453,474,456,480,471,479,473,480,472,474,487,471,469,488,472,482,489,469,467,490,470,488,465,489,467,490,466,468,463,491,465,466,492,464,391,503,501,504,392,502,393,501,499,502,394,500,393,497,395,394,498,500,395,495,397,
            396,496,498,397,493,399,398,494,496,399,505,387,400,506,494,501,505,493,502,506,504,493,499,501,500,494,502,495,497,499,500,498,496,381,505,313,382,506,388,313,503,321,504,314,322,319,503,389,504,320,390,46,0,2,3,1,47,44,2,4,5,3,45,
            2,8,6,7,9,3,0,10,8,9,11,1,10,12,14,15,13,11,8,14,16,17,15,9,20,18,16,21,15,17,12,22,20,21,23,13,22,24,26,27,25,23,26,28,18,27,21,19,32,30,28,33,27,29,34,32,26,35,25,27,36,38,32,
            37,35,33,38,40,30,39,33,31,44,42,40,45,39,41,46,44,38,47,37,39,36,50,48,37,47,49,34,52,50,35,37,51,24,54,52,25,35,53,22,56,54,23,25,55,22,12,58,59,13,23,12,10,62,63,11,13,10,0,64,65,1,11,
            0,46,48,49,47,1,173,175,90,174,89,90,171,173,88,172,87,89,84,169,171,172,170,85,82,167,169,170,168,83,80,165,167,168,166,81,78,91,145,146,92,79,93,147,145,94,92,146,93,95,149,150,96,94,97,151,149,98,96,150,99,153,151,
            100,98,152,101,155,153,102,100,154,101,103,157,158,104,102,105,159,157,106,104,158,107,161,159,108,106,160,66,67,161,66,108,162,127,159,161,128,110,162,127,178,157,158,179,128,155,157,178,156,126,179,153,155,125,154,124,126,151,153,123,152,122,124,
            149,151,121,150,120,122,147,149,119,148,118,120,145,147,117,146,116,118,163,145,115,164,114,116,113,180,176,176,181,114,161,67,111,162,110,112,111,67,177,177,67,112,180,182,177,181,176,177,134,136,175,175,136,135,132,134,173,174,135,133,132,171,169,
            133,131,170,165,186,184,185,187,166,130,169,167,168,170,131,189,188,186,189,144,187,186,188,68,187,185,68,130,184,68,131,129,68,141,192,190,191,193,142,139,194,192,193,195,140,196,194,139,197,138,140,70,196,138,143,190,69,144,189,69,190,205,207,
            191,69,207,70,198,199,200,198,70,196,199,201,202,200,197,201,203,192,202,195,193,192,203,205,206,204,193,203,201,199,204,198,200,198,207,205,206,207,198,138,139,163,164,140,138,139,141,210,211,142,140,143,212,210,144,142,211,186,165,212,187,144,213,
            208,212,165,209,81,166,214,210,212,215,209,213,78,163,210,211,164,79,129,71,221,129,131,222,132,130,221,222,131,133,134,132,219,220,133,135,136,134,217,218,135,136,217,228,230,218,216,230,217,219,226,227,220,218,219,221,224,225,222,220,71,223,224,
            71,222,225,223,230,228,229,230,223,182,180,233,234,181,183,111,182,231,232,183,112,111,253,255,112,110,256,113,251,233,114,181,234,113,115,249,250,116,114,115,117,247,248,118,116,117,119,245,246,120,118,119,121,243,244,122,120,123,241,243,124,122,244,
            125,239,241,126,124,242,125,178,235,236,179,126,178,127,237,238,128,179,127,109,255,256,110,128,255,257,275,256,238,276,235,237,275,276,238,236,235,277,273,236,240,274,239,273,271,240,242,272,243,241,271,272,242,244,243,269,267,244,246,268,247,245,267,
            268,246,248,247,265,263,248,250,264,249,263,261,250,252,262,233,251,261,262,252,234,255,253,259,260,254,256,253,231,281,282,232,254,231,233,279,280,234,232,66,107,283,284,108,66,107,105,285,286,106,108,103,287,285,104,106,286,101,289,287,102,104,288,
            99,291,289,100,102,290,99,97,293,294,98,100,95,295,293,96,98,294,95,93,297,298,94,96,93,91,299,300,92,94,308,327,337,308,307,338,307,337,335,307,306,336,306,335,339,306,305,340,88,90,305,305,90,89,86,88,339,340,89,87,84,86,333,
            334,87,85,82,84,329,330,85,83,329,335,337,338,336,330,333,339,335,334,330,336,331,337,327,332,326,328,80,82,331,332,83,81,341,343,214,342,209,215,325,341,208,326,81,209,214,343,345,215,79,346,345,299,91,346,79,92,323,351,303,324,76,303,
            351,349,77,352,303,77,77,349,347,348,350,77,304,347,327,328,348,304,327,347,341,328,326,342,295,297,317,318,298,296,315,323,76,316,75,76,357,355,302,358,301,302,302,355,353,354,356,302,74,353,315,316,354,74,291,293,361,362,294,292,363,361,367,
            368,362,364,365,367,369,370,368,366,371,369,375,376,370,372,377,373,375,378,314,376,315,353,373,374,354,316,353,355,371,372,356,354,355,357,365,366,358,356,357,359,363,364,360,358,291,363,359,292,290,360,359,357,301,360,73,301,283,285,287,288,286,284,
            283,289,359,360,290,284,295,309,361,296,294,362,309,311,367,368,312,310,311,381,369,370,382,312,375,369,381,376,314,382,349,385,383,350,348,384,383,385,319,384,318,320,297,299,383,384,300,298,299,343,341,342,344,300,321,379,377,322,314,378,377,379,323,
            378,316,324,385,379,321,386,320,322,351,379,385,352,350,386,387,413,401,388,400,402,399,401,403,404,402,400,403,405,395,404,398,396,405,407,393,406,396,394,407,409,391,408,394,392,391,409,411,412,410,392,409,419,417,418,420,410,407,421,419,420,422,408,
            423,421,407,424,406,408,425,423,405,426,404,406,427,425,403,428,402,404,401,413,415,416,414,402,319,443,441,320,318,442,389,411,443,390,320,444,309,317,441,442,318,310,381,429,413,414,430,382,411,417,439,440,418,412,445,443,439,446,438,440,433,445,437,
            438,446,434,447,445,433,448,432,434,447,431,449,448,430,450,429,449,415,430,414,416,311,447,429,430,448,312,441,445,447,442,312,448,415,449,451,452,450,416,431,461,451,432,450,452,431,433,459,460,434,432,435,457,459,436,434,460,437,455,457,438,436,458,
            437,439,453,454,440,438,439,417,473,474,418,440,427,415,475,476,416,428,425,427,463,464,428,426,423,425,465,466,426,424,423,467,469,424,422,470,421,469,471,422,420,472,419,471,473,420,418,474,455,479,477,456,458,478,479,481,483,480,478,484,483,481,487,
            488,482,484,487,489,491,488,486,492,463,475,485,486,476,464,483,485,475,484,452,476,461,477,483,462,452,484,457,477,461,462,478,458,473,479,455,474,454,456,471,481,479,480,482,472,487,481,471,488,470,472,489,487,469,490,468,470,465,491,489,490,492,466,
            391,389,503,504,390,392,393,391,501,502,392,394,393,499,497,394,396,498,395,497,495,396,398,496,397,495,493,398,400,494,399,493,505,400,388,506,501,503,505,502,494,506,493,495,499,500,496,494,381,387,505,382,314,506,313,505,503,504,506,314,319,321,503,
            504,322,320};
        // We use "static" here because they must survive until Teapot_Init(...) returns.
        *ppverts = verts;
        *numVerts = (int)(sizeof(verts)/(sizeof(verts[0])*3));
        *ppinds = inds;
        *numInds = (int)(sizeof(inds)/sizeof(inds[0]));
    }
        break;
    }
}
#endif //TEST_ADD_USER_MESH
