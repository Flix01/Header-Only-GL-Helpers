// https://github.com/Flix01/Header-Only-GL-Helpers
//
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
gcc -O2 -std=gnu89 -no-pie test_teapot.c -o test_teapot -I"../" -lglut -lGL -lX11 -lm
// WINDOWS (here we use the static version of glew, and glut32.lib, that can be replaced by freeglut.lib):
cl /O2 /MT /Tc test_teapot.c /D"TEAPOT_NO_RESTRICT" /D"GLEW_STATIC" /I"../" /link /out:test_teapot.exe glut32.lib glew32s.lib opengl32.lib gdi32.lib Shell32.lib comdlg32.lib user32.lib kernel32.lib
// EMSCRIPTEN:
emcc -O2 -std=gnu89 -fno-rtti -fno-exceptions -o test_teapot.html test_teapot.c -I"./" -I"../" -s LEGACY_GL_EMULATION=0 --closure 1
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

#define TEAPOT_CENTER_MESHES_ON_FLOOR   // (Optional) Otherwise meshes are centered in their local aabb center
//#define TEAPOT_INVERT_MESHES_Z_AXIS     // (Optional) Otherwise meshes look in the opposite Z direction
#define TEAPOT_SHADER_SPECULAR          // (Optional) specular hilights
#define TEAPOT_SHADER_FOG           // (Optional) fog to remove bad clipping
#define TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER // (Optional) better fog quality
//#define TEAPOT_MATRIX_USE_DOUBLE_PRECISION    // (Optional) makes tpoat (teapot_float) be double instead of float
//#define TEAPOT_ENABLE_FRUSTUM_CULLING           // (Optional) a bit expensive, and does not cull 100% hidden objects. You'd better test if it works and if it's faster...
#define TEAPOT_IMPLEMENTATION       // Mandatory in only one c/c++ file
#include "teapot.h"


// Config file handling: basically there's an .ini file next to the
// exe that you can tweak. (it's just an extra)
const char* ConfigFileName = "test_teapot.ini";
typedef struct {
    int fullscreen_width,fullscreen_height;
    int windowed_width,windowed_height;
    int fullscreen_enabled;
    int show_fps;
} Config;
void Config_Init(Config* c) {
    c->fullscreen_width=c->fullscreen_height=0;
    c->windowed_width=960;c->windowed_height=540;
    c->fullscreen_enabled=0;
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
    return 0;
}
int Config_Save(Config* c,const char* filePath)  {
    FILE* f = fopen(filePath, "wt");
    if (!f)  return -1;
    fprintf(f, "[Size In Fullscreen Mode (zero means desktop size)]\n%d %d\n",c->fullscreen_width,c->fullscreen_height);
    fprintf(f, "[Size In Windowed Mode]\n%d %d\n",c->windowed_width,c->windowed_height);
    fprintf(f, "[Fullscreen Enabled (0 or 1) (CTRL+RETURN)]\n%d\n", c->fullscreen_enabled);
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

// About tpoat (teapot_float):
// tpoat is just float or double: normal users should just use float and don't worry
// We use it just to test the (optional) definition: TEAPOT_MATRIX_USE_DOUBLE_PRECISION

// camera data:
tpoat targetPos[3];             // please set it in resetCamera()
tpoat cameraYaw;                // please set it in resetCamera()
tpoat cameraPitch;              // please set it in resetCamera()
tpoat cameraDistance;           // please set it in resetCamera()
tpoat cameraPos[3];             // Derived value (do not edit)
tpoat vMatrix[16];              // view matrix
tpoat cameraSpeed = 0.5f;       // When moving it

// light data
tpoat lightDirection[3] = {1.5,2,2};// Will be normalized

// pMatrix data:
tpoat pMatrix[16];                  // projection matrix
const tpoat pMatrixFovyDeg = 45.f;
const tpoat pMatrixNearPlane = 0.5f;
const tpoat pMatrixFarPlane = 20.0f;

float instantFrameTime = 16.2f;

static __inline float Vec3Dot(const tpoat v0[3],const tpoat v1[3]) {
    return v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2];
}
static __inline void Vec3Normalize(tpoat v[3]) {
    tpoat len = Vec3Dot(v,v);int i;
    if (len!=0) {
        len = sqrt(len);
        for (i=0;i<3;i++) v[i]/=len;
    }
}
static __inline void Vec3Cross(tpoat rv[3],const tpoat a[3],const tpoat b[3]) {
    rv[0] =	a[1] * b[2] - a[2] * b[1];
    rv[1] =	a[2] * b[0] - a[0] * b[2];
    rv[2] =	a[0] * b[1] - a[1] * b[0];
}


int current_width=0,current_height=0;  // Not sure when I've used these...
void ResizeGL(int w,int h) {
    current_width = w;
    current_height = h;
    if (h>0)	{
        // We set our pMatrix here in ResizeGL(), and we must notify teapot.h about it too.
        Teapot_Helper_Perspective(pMatrix,pMatrixFovyDeg,(tpoat)w/(tpoat)h,pMatrixNearPlane,pMatrixFarPlane);
        Teapot_SetProjectionMatrix(pMatrix);
    }

    if (w>0 && h>0 && !config.fullscreen_enabled) {
        // On exiting we'll like to save these data back
        config.windowed_width=w;
        config.windowed_height=h;
    }

    glViewport(0,0,w,h);    // This is what people often call in ResizeGL()

}


void InitGL(void) {    
    // IMPORTANT CALL--------------------------------------------------------
    Teapot_Init();
    //-----------------------------------------------------------------------

    // These are important, but often overlooked, OpenGL calls
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Otherwise transparent objects are not displayed correctly
    glClearColor(0.3f, 0.6f, 1.0f, 1.0f);

#   ifdef TEAPOT_SHADER_FOG
    // We use fog to prevent ad clipping artifacts, so it just needs the near and far plane
    Teapot_SetFogDistances((pMatrixFarPlane-pMatrixNearPlane)*0.5f,pMatrixFarPlane); // We start the fog at the half point... but it works better nearer when farPlane is far away
    Teapot_SetFogColor(0.3f, 0.6f, 1.0f); // it should be the same as glClearColor()
#   endif

    // This line can change the look of the demo considerably. Try commenting it out!
    Teapot_Enable_ColorMaterial();    // Optional (Teapot_SetColor(...) internally calls Teapot_SetColorAmbient(...); and Teapot_SetColorSpecular(...); when enabled)
    // The truth is that for every object it's tedious to setup 3 colors (even if we'd get better visuals): so this definition is useful.

    // Warning: in this demo we know that the calling order of the callbacks is: InitGL(),ResizeGL(...),DrawGL().
    // That's why we can avoid calling ResizeGL(...) here
    // However in other demos it might be mandatory to call ResizeGL(...) here

}

void DestroyGL() {
    // IMPORTANT CALL--------------------------------------------------------
    Teapot_Destroy();
    // ----------------------------------------------------------------------
}


void DrawGL(void) 
{	
    // We need to calculate the "instantFrameTime" for smooth camera movement and
    // we also need stuff to calculate FPS.
    const unsigned timeNow = glutGet(GLUT_ELAPSED_TIME);
    static unsigned begin = 0;
    static unsigned cur_time = 0;
    unsigned elapsed_time,delta_time;
    static unsigned long frames = 0;
    static unsigned fps_time_start = 0;
    static float FPS = 60;
    // We need this because we'll keep modifying it to pass all the model matrices
    static tpoat mMatrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};

    // Here are some FPS calculations
    if (begin==0) begin = timeNow;
    elapsed_time = timeNow - begin;
    delta_time = elapsed_time - cur_time;    
    instantFrameTime = (float)delta_time*0.001f;
    cur_time = elapsed_time;
    if (fps_time_start==0) fps_time_start = timeNow;
    if ((++frames)%20==0) {
        // We update our FPS every 20 frames (= it's not instant FPS)
        FPS = (20*1000.f)/(float)(timeNow - fps_time_start);
        fps_time_start = timeNow;
    }

    // view Matrix
    Teapot_Helper_LookAt(vMatrix,cameraPos[0],cameraPos[1],cameraPos[2],targetPos[0],targetPos[1],targetPos[2],0,1,0);
    Teapot_SetViewMatrixAndLightDirection(vMatrix,lightDirection);  // we must notify teapot.h, and we also pass the lightDirection here

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    Teapot_PreDraw();   // From now on we can call all the Teapot_XXX methods below

    // Ground mesh (box)
    mMatrix[12]=0.0;    mMatrix[13]=-0.25;    mMatrix[14]=0.0;
#   ifndef TEAPOT_CENTER_MESHES_ON_FLOOR
    mMatrix[13]=-0.125;
#   endif
    Teapot_SetScaling(8.f,0.25f,12.f);
    Teapot_SetColor(0.1f,0.6f,0.1f,1.0f);
    Teapot_Draw(mMatrix,TEAPOT_MESH_CUBIC_GROUND);

    Teapot_SetScaling(1.f,1.f,1.f);

    // (teapot)
    mMatrix[12]=0.5;    mMatrix[13]=0.0;    mMatrix[14]=-1.5;
    Teapot_SetColor(1.f,0.4f,0.2f,1.0f);
    Teapot_Draw(mMatrix,TEAPOT_MESH_TEAPOT);

    // (bunny)
    mMatrix[12]=-0.5;    mMatrix[13]=0.0;    mMatrix[14]=1.0;
    Teapot_SetColor(0.2f,0.4f,0.9f,1.0f);
    Teapot_Draw(mMatrix,TEAPOT_MESH_BUNNY);

    Teapot_SetScaling(0.5f,0.5f,0.5f);

    // (teapot)
    mMatrix[12]=0.5;    mMatrix[13]=0.0;    mMatrix[14]=1.5;
    Teapot_SetColor(0.8f,0.6f,0.2f,1.0f);
    Teapot_Draw(mMatrix,TEAPOT_MESH_TEAPOT);

    // (bunny)
    mMatrix[12]=-1.5;    mMatrix[13]=0.0;    mMatrix[14]=1.0;
    Teapot_SetColor(0.3f,0.5f,1.0f,1.0f);
    Teapot_Draw(mMatrix,TEAPOT_MESH_BUNNY);

    // (transparency needs: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);)
    glEnable(GL_BLEND);
    mMatrix[12]=-0.5;    mMatrix[13]=0.0;    mMatrix[14]=-0.5;
    Teapot_SetColor(0.95f,0.95f,0.2f,0.65f);
    Teapot_Draw(mMatrix,TEAPOT_MESH_CYLINDER);
    glDisable(GL_BLEND);

    // (capsule)
    Teapot_SetScaling(0.25f,0.5f,0.25f);
    // For capsules, Teapot_SetScaling(x,y,z) is interpreted this way:
    // diameter = (x+z)/2; cylinderHeight = y. So the total height is: (cylinderHeight + diameter)
    // This was made to force uniform scaling of the two half spheres.
    mMatrix[12]=0.5;    mMatrix[13]=0.0;    mMatrix[14]=0.5;
    Teapot_SetColor(0.5f,0.75f,1.0f,1.0f);
    Teapot_Draw(mMatrix,TEAPOT_MESH_CAPSULE);


    Teapot_SetColor(0.5f,0.2f,0.1f,1.0f);

    // (chairs)
    Teapot_SetScaling(0.25f,0.25f,0.25f);
    mMatrix[13]=0.0;    mMatrix[14]=1.25;
    mMatrix[12]= 1.3;Teapot_Draw(mMatrix,TEAPOT_MESH_CHAIR);
    mMatrix[12]= 1.5;Teapot_Draw(mMatrix,TEAPOT_MESH_CHAIR);
    mMatrix[12]= 1.7;Teapot_Draw(mMatrix,TEAPOT_MESH_CHAIR);

    // With these 2 lines, we can achieve a 180° rotation for free
    mMatrix[8]=-mMatrix[8];mMatrix[9]=-mMatrix[9];mMatrix[10]=-mMatrix[10];
    mMatrix[0]=-mMatrix[0];mMatrix[1]=-mMatrix[1];mMatrix[2]=-mMatrix[2];
    mMatrix[13]=0.0;    mMatrix[14]=1.55;
    mMatrix[12]= 1.3;Teapot_Draw(mMatrix,TEAPOT_MESH_CHAIR);
    mMatrix[12]= 1.5;Teapot_Draw(mMatrix,TEAPOT_MESH_CHAIR);
    mMatrix[12]= 1.7;Teapot_Draw(mMatrix,TEAPOT_MESH_CHAIR);
    mMatrix[0]=-mMatrix[0];mMatrix[1]=-mMatrix[1];mMatrix[2]=-mMatrix[2];
    mMatrix[8]=-mMatrix[8];mMatrix[9]=-mMatrix[9];mMatrix[10]=-mMatrix[10];


    // (table)
    Teapot_SetScaling(0.3f,0.25f,0.25f);
    mMatrix[12]=1.5;   mMatrix[13]=0.0;    mMatrix[14]=1.4;
    Teapot_Draw(mMatrix,TEAPOT_MESH_TABLE);


    /*{ // All meshes
#       define NUM_COLORS 10
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
        };
        int i;const int HALF_TEAPOT_MESH_COUNT = TEAPOT_MESH_COUNT/2+1;
        Teapot_SetScaling(0.5,0.5,0.5);
        for (i=0;i<TEAPOT_MESH_COUNT;i++)   {
            const int ci = i%NUM_COLORS;
            Teapot_SetColor(Colors[ci][0],Colors[ci][1],Colors[ci][2],1.f);
            mMatrix[12]=-2.5+(i<HALF_TEAPOT_MESH_COUNT ? 0 : 5);
            mMatrix[13]=0.0;
            if (i==0)                           mMatrix[14]=-HALF_TEAPOT_MESH_COUNT*0.15f;
            else if (i==HALF_TEAPOT_MESH_COUNT) mMatrix[14]=-HALF_TEAPOT_MESH_COUNT*0.50f;
            else mMatrix[14]+=0.8f;
            Teapot_Draw(mMatrix,i);
        }
        // Please note that, unlike all the other meshes, the last two meshes: TEAPOT_MESH_HALF_SPHERE_UP and TEAPOT_MESH_HALF_SPHERE_DOWN
        // are always centered in the virtual center of their full sphere (regardless of the TEAPOT_CENTER_MESHES_ON_FLOOR definition):
        // they're there mainly for internal use when drawing: TEAPOT_MESH_CAPSULE
    }*/

    /*{   // Test Teapot_Helper_DrawArmatureBone(...)
        const tpoat length = 1.f;
        Teapot_Helper_IdentityMatrix(mMatrix);
        mMatrix[12]=0;mMatrix[14]=2;mMatrix[13]=0;
        Teapot_Helper_RotateMatrix(mMatrix,20,0,0,1);
        Teapot_Helper_RotateMatrix(mMatrix,30,1,0,0);

        Teapot_LowLevel_StartDisablingLighting();
        glDepthMask(GL_FALSE);glDisable(GL_DEPTH_TEST);
        Teapot_SetColorAmbient(0.8f,0.8f,0.f);//0.75f);
        Teapot_Helper_DrawArmatureBone(mMatrix,length);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.f);glEnable(GL_LINE_SMOOTH);  // not present in teapot.h (can be used). Better in InitGL().
        //glPolygonOffset(2.0, 4);                  // this probably interferes with teapot.h
        glEnable(GL_POLYGON_OFFSET_LINE);
        Teapot_SetColorAmbient(0.6f,0.4f,0.f);
        Teapot_Helper_DrawArmatureBone(mMatrix,length);
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);glDepthMask(GL_TRUE);
        Teapot_LowLevel_StopDisablingLighting();

        // reset
        Teapot_Helper_IdentityMatrix(mMatrix);
        Teapot_SetScaling(1.f,1.f,1.f);
    }*/



    Teapot_PostDraw();  // We can't call the Teapot_XXX methods used above anymore
    

if (config.show_fps) {
    if ((elapsed_time/1000)%2==0)   {
        char tmp[64]="";
        sprintf(tmp,"FPS: %1.0f (%dx%d)\n",FPS,current_width,current_height);
        printf("%s",tmp);fflush(stdout);
        config.show_fps=0;
    }
}

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
    (void)(x);(void)(y);    // -Wunused-parameter
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
    const tpoat distanceY = sin(cameraPitch)*cameraDistance;
    const tpoat distanceXZ = cos(cameraPitch)*cameraDistance;
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
    (void)(x);(void)(y);    // -Wunused-parameter
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
        case GLUT_KEY_F2:
            config.show_fps = !config.show_fps;
            //printf("showFPS: %s.\n",config.show_fps?"ON":"OFF");
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
            tpoat forward[3] = {targetPos[0]-cameraPos[0],0,targetPos[2]-cameraPos[2]};
            tpoat up[3] = {0,1,0};
            tpoat left[3];

            Vec3Normalize(forward);
            Vec3Cross(left,up,forward);
            {
                tpoat delta[3] = {0,0,0};int i;
                if (key==GLUT_KEY_LEFT || key==GLUT_KEY_RIGHT) {
                    tpoat amount = instantFrameTime*cameraSpeed*(key==GLUT_KEY_RIGHT ? -25.0f : 25.0f);
                    for (i=0;i<3;i++) delta[i]+=amount*left[i];
                }
                else {
                    tpoat amount = instantFrameTime*cameraSpeed*(key==GLUT_KEY_DOWN ? -25.0f : 25.0f);
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

void GlutMouse(int button,int state,int x,int y) {
    (void)(button);(void)(state);(void)(x);(void)(y);    // -Wunused-parameter
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
        windowId = glutCreateWindow("test_teapot.c");
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
#	ifndef __EMSCRIPTEN__
    printf("CTRL+RETURN:\t\ttoggle fullscreen on/off\n");
#	endif //__EMSCRIPTEN__
    printf("F2:\t\t\tdisplay FPS\n");
    printf("\n");


    resetCamera();  // Mandatory

    glutMainLoop();


    return 0;
}



