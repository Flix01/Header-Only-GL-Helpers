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
gcc -O2 -std=gnu89 test_matrix_stack.c -o test_matrix_stack -I"../" -lglut -lGL -lX11 -lm
// WINDOWS (here we use the static version of glew, and glut32.lib, that can be replaced by freeglut.lib):
cl /O2 /MT /Tc test_matrix_stack.c /D"TEAPOT_NO_RESTRICT" /D"GLEW_STATIC" /I"../" /link /out:test_matrix_stack.exe glut32.lib glew32s.lib opengl32.lib gdi32.lib Shell32.lib comdlg32.lib user32.lib kernel32.lib
// EMSCRIPTEN:
emcc -O2 -std=gnu89 -fno-rtti -fno-exceptions -o test_matrix_stack.html test_matrix_stack.c -I"./" -I"../" -s LEGACY_GL_EMULATION=0 --closure 1
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

#define USE_DOUBLE_PRECISION
#ifdef USE_DOUBLE_PRECISION
#   define TEAPOT_MATRIX_USE_DOUBLE_PRECISION       // (Optional) makes tpoat (teapot_float) be double instead of float
#   define IM_MATRIX_STACK_USE_DOUBLE_PRECISION     // (Optional) makes imoat (im_matrix_stack_float) be double instead of float
#endif //USE_DOUBLE_PRECISION

#define TEAPOT_CENTER_MESHES_ON_FLOOR   // (Optional) Otherwise meshes are centered in their local aabb center
//#define TEAPOT_INVERT_MESHES_Z_AXIS     // (Optional) Otherwise meshes look in the opposite Z direction
//#define TEAPOT_SHADER_SPECULAR          // (Optional) specular hilights
//#define TEAPOT_SHADER_FOG           // (Optional) fog to remove bad clipping
//#define TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER // (Optional) better fog quality
//#define TEAPOT_ENABLE_FRUSTUM_CULLING           // (Optional) a bit expensive, and does not cull 100% hidden objects. You'd better test if it works and if it's faster...
#define TEAPOT_IMPLEMENTATION       // Mandatory in only one c/c++ file
#include "teapot.h"


#define IM_MATRIX_STACK_IMPLEMENTATION       // Mandatory in only one c/c++ file
#include "im_matrix_stack.h"


// Config file handling: basically there's an .ini file next to the
// exe that you can tweak. (it's just an extra)
const char* ConfigFileName = "test_matrix_stack.ini";
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
tpoat cameraSpeed = 0.5f;       // When moving it

// light data
tpoat lightDirection[3] = {1.5,2,2};// Will be normalized

// pMatrix data:
tpoat pMatrix[16];                  // projection matrix
const tpoat pMatrixFovDeg = 45.f;
const tpoat pMatrixNearPlane = 0.5f;
const tpoat pMatrixFarPlane = 20.0f;

float instantFrameTime = 16.2f;

// No more helper methods here!

int current_width=0,current_height=0;  // Not sure when I've used these...
void ResizeGL(int w,int h) {
    current_width = w;
    current_height = h;
    if (h>0)	{
        // We set our pMatrix here in ResizeGL(), and we must notify teapot.h about it too.
        imMatrixMode(GL_PROJECTION);
        imLoadIdentity();
        imuPerspective(pMatrixFovDeg,(tpoat)w/(tpoat)h,pMatrixNearPlane,pMatrixFarPlane);
        imGetImoatv(GL_PROJECTION,pMatrix); // We can retrieve it if we need to store pMatrix.
        Teapot_SetProjectionMatrix(pMatrix);
        imMatrixMode(GL_MODELVIEW);
    }

    if (w>0 && h>0 && !config.fullscreen_enabled) {
        // On exiting we'll like to save these data back
        config.windowed_width=w;
        config.windowed_height=h;
    }

    glViewport(0,0,w,h);    // This is what people often call in ResizeGL()

}


void InitGL(void) {    
    // IMPORTANT CALLS-------------------------------------------------------
    Im_Matrix_Stack_Init();
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
    // IMPORTANT CALLS--------------------------------------------------------
    Teapot_Destroy();
    Im_Matrix_Stack_Destroy();
    // ----------------------------------------------------------------------
}


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
    // We will use these to move the little bunny:
    const float sn = sin(cur_time*0.002f);
    const float cs = cos(cur_time*0.001f);
    imoat littleBunnyPos[3];    // The big bunny needs to know wher the little one is


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
    imMatrixMode(GL_MODELVIEW);
    imLoadIdentity();
    // Here we don't store the vMatrix, so we can directly chain the gluLookAt(...) call,
    // because imuLookAt(...), as many other calls, returns a const imoat*
    Teapot_SetViewMatrixAndLightDirection(
                imuLookAt(cameraPos[0],cameraPos[1],cameraPos[2],targetPos[0],targetPos[1],targetPos[2],0,1,0),
                lightDirection);  // we must notify teapot.h, and we also pass the lightDirection here
    // Moreover, we could have stored the pointer returned by imuLookAt(...), or more generally by imGetImoatMatrixPtr()):
    // it points to the memory spot inside the stack. So if we "scope" the rest of the
    // code with some imPushMatrix()/imPopMatrix(), it should stay valid.

    imPushMatrix(); // Optional scope

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    Teapot_PreDraw();   // From now on we can call all the Teapot_XXX methods below

    // Ground mesh (box)
    imPushMatrix();
    imTranslate(0,-0.25,0); // Note that we can use imTranslatef(...) or imTranslated(...) as well
    Teapot_SetScaling(8.f,0.25f,12.f);
    Teapot_SetColor(0.1f,0.6f,0.1f,1.0f);
    Teapot_Draw_Mv(imGetImoatMatrixPtr(),TEAPOT_MESH_CUBIC_GROUND); // imGetImoatMatrixPtr() returns a ptr to the current stack matrix space of the current matrix mode
    imPopMatrix();

    Teapot_SetScaling(1.f,1.f,1.f);

    // (teapot)
    imPushMatrix();
    imLoadIdentity();   // New: Test: imLoadIdentity() necessary only if we call Teapot_Draw(...) instead of Teapot_Draw_Mv(...) here
    imTranslate(0.5,0.0,-2.5);
    imRotate(cur_time*0.02f,0,1,0);    // Of course we can use imRotatef(...), imRotated(...) as well
    Teapot_SetColor(1.f,0.4f,0.2f,1.0f);
    Teapot_Draw(imGetImoatMatrixPtr(),TEAPOT_MESH_TEAPOT);
    imPopMatrix();


    Teapot_SetScaling(0.5f,0.5f,0.5f);

    // (teapot)
    imPushMatrix();
    imTranslate(0.5,0.0,2.5);
    Teapot_SetColor(0.8f,0.6f,0.2f,1.0f);
    Teapot_Draw_Mv(imGetImoatMatrixPtr(),TEAPOT_MESH_TEAPOT);
    imPopMatrix();

    // (little bunny)
    imPushMatrix();
    littleBunnyPos[0]=3.5*cs;littleBunnyPos[1]=0.0;littleBunnyPos[2]=2.0*sn;
    imTranslate(littleBunnyPos[0],littleBunnyPos[1],littleBunnyPos[2]);
    Teapot_SetColor(0.3f,0.5f,1.0f,1.0f);
    Teapot_Draw_Mv(imGetImoatMatrixPtr(),TEAPOT_MESH_BUNNY);
    imPopMatrix();

    // (transparency needs: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);)
    imPushMatrix();
    glEnable(GL_BLEND);
    imTranslate(-2.5,0.0,-3.5);
    Teapot_SetColor(0.95f,0.95f,0.2f,0.65f);
    Teapot_Draw_Mv(imGetImoatMatrixPtr(),TEAPOT_MESH_CYLINDER);
    glDisable(GL_BLEND);
    imPopMatrix();

    // (capsule)
    imPushMatrix();
    Teapot_SetScaling(0.45f,0.75f,0.45f);
    // For capsules, Teapot_SetScaling(x,y,z) is interpreted this way:
    // diameter = (x+z)/2; cylinderHeight = y. So the total height is: (cylinderHeight + diameter)
    // This was made to force uniform scaling of the two half spheres.
    imTranslate(2.25,0.0,1.0);
    Teapot_SetColor(0.5f,0.75f,1.0f,1.0f);
    Teapot_Draw_Mv(imGetImoatMatrixPtr(),TEAPOT_MESH_CAPSULE);
    imPopMatrix();

    // (big bunny)
    imPushMatrix();
    Teapot_SetScaling(1.f,1.f,1.f);
    Teapot_SetColor(0.2f,0.4f,0.9f,1.0f);
    imLoadIdentity();   // We MUST work with mMatrix now
    imTranslate(-1.75,0.0,0.0); // (*)
    // This makes the mMatrix Yaw look at the X and Z point (if you need pitch too, call IMLookAtYX(...), but it's the same in this demo because of TEAPOT_CENTER_MESHES_ON_FLOOR)
    IMLookAtYX2D((imoat*) imGetImoatMatrixPtr(),littleBunnyPos[0],littleBunnyPos[2],
            0.5,
            2.5
            );  // Last 2 args can be 0,0 if you just need rotation around (*)
    // IMLookAtYX(),IMLookAtYX2D() need no scaling INSIDE the matrix (first argument)
    Teapot_Draw(imGetImoatMatrixPtr(),TEAPOT_MESH_BUNNY);   // Not Teapot_Draw_Mv(...) here
    imPopMatrix();


    Teapot_PostDraw();  // We can't call the Teapot_XXX methods used above anymore
    imPopMatrix();  // Optional scope

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
    cameraPitch = M_PI*0.14f;                       // The camera rotation around the XZ plane
    cameraDistance = 6;                             // The distance between the camera position and the camera target point

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

            IMVector3Normalize(forward);
            IMVector3Cross(left,up,forward);
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
        windowId = glutCreateWindow("test_matrix_stack.c");
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



