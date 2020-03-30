// https://github.com/Flix01/Header-Only-GL-Helpers
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
Code should be in ANSI C (-std=c89) with some notable exceptions:
-> single line comments are used.
-> initializer elements can be computable at load time.
-> some C99 math function are used (sinf,cosf,etc.). These can be removed by defining CHA_NO_C99_MATH.
So, to be strict, we should compile the code using -std=c99, but I prefer using -std=gnu89, that allows them.
Also, older versions of MSVC seem to work correctly out of the box with /DCHA_NO_C99_MATH (even if they're not fully C99 compliant, and/or you can't specify the C dialect to use).

// LINUX:
gcc -O2 -std=gnu89 -no-pie -fno-pie test_character_standalone.c -o test_character_standalone -I"../" -lglut -lGL -lX11 -lm
// WINDOWS (here we use the static version of glew, and glut32.lib, that can be replaced by freeglut.lib):
cl /O2 /MT /Tc test_character_standalone.c /D"GLEW_STATIC" /I"../" /link /out:test_character_standalone.exe glut32.lib glew32s.lib opengl32.lib gdi32.lib Shell32.lib comdlg32.lib user32.lib kernel32.lib
// LINUX (for developers: less warnings, but errors when variables are not declared correctly)
gcc -O2 -fno-pie -std=gnu89 -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function -Werror=declaration-after-statement -no-pie  test_character_standalone.c -o test_character_standalone -I"../" -lglut -lGL -lX11 -lm


// IN ADDITION:
By default the source file assumes that every OpenGL-related header is in "GL/".
But you can define in the command-line the correct paths you use in your system
for glut.h, glew.h, etc. with something like:
-DGLUT_PATH=\"Glut/glut.h\"
-DGLEW_PATH=\"Glew/glew.h\"
(this syntax works on Linux, don't know about Windows)
*/

//#define USE_GLEW  // By default it's only defined for Windows builds (but can be defined in Linux/Mac builds too)


#define PROGRAM_NAME "test_character_standalone"

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
#   endif
#endif //__FREEGLUT_STD_H__

#define STR_MACRO(s) #s
#define XSTR_MACRO(s) STR_MACRO(s)


#define CHA_HAS_OPENGL_SUPPORT      /* mandatory */
#define CHA_USE_VBO                 /* optional: it just creates and uses 'vertex buffer objects' (vbo), instead of glBegin(GL_TRIANGLES)/glEnd() */
#define CHA_HINT_USE_FFP_VBO        /* mandatory if CHA_USE_VBO (otherwise 'vbos' need to know shader attribute locations) */
//#define CHA_HINT_USE_VAO          /* optional */
#define CHA_USE_MATH_MACROS         /* mandatory in this demo (to support double precision more easily) */
//#define CHA_DOUBLE_PRECISION        /* optional */
#define CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION  /* optional (experimental and non-robust stuff that does almost nothing in this demo, but it's good for developers) */
#define CHARACTER_IMPLEMENTATION
#include "../character.h"

#ifndef CHA_DOUBLE_PRECISION
#   define  glMultMatrix(M)     glMultMatrixf(M)
#   define  glLoadMatrix(M)     glLoadMatrixf(M)
#else
#   define  glMultMatrix(M)     glMultMatrixd(M)
#   define  glLoadMatrix(M)     glLoadMatrixd(M)
#endif

//#define VISUALIZE_DEPTH_TEXTURE
//#define USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE // Better resolution, but shadow-swimming as the camera rotates (on static objects).
                                                // in camera ortho3d mode [F1], the resolution is further better, but shadow-swimming appears when zooming in-out too.

#define SHADOW_MAP_HEIGHT 1536                  // 1024 2048 (or 1536 if non-power-of-two textures are supported: WebGL1 supports them if no GL_REPEAT and no mipmaps are used; that's OK for us)
#define SHADOW_MAP_WIDTH SHADOW_MAP_HEIGHT      // 1024 2048 (or 1536 if non-power-of-two textures are supported: WebGL1 supports them if no GL_REPEAT and no mipmaps are used; that's OK for us)
//#define SHADOW_MAP_HEIGHT 2048                  // 1024 2048 (or 1536 if non-power-of-two textures are supported: WebGL1 supports them if no GL_REPEAT and no mipmaps are used; that's OK for us)
//#define SHADOW_MAP_WIDTH (SHADOW_MAP_HEIGHT/2)  // 1024 2048 (or 1536 if non-power-of-two textures are supported: WebGL1 supports them if no GL_REPEAT and no mipmaps are used; that's OK for us)
//#define SHADOW_MAP_HEIGHT 1024                  // 1024 2048 (or 1536 if non-power-of-two textures are supported: WebGL1 supports them if no GL_REPEAT and no mipmaps are used; that's OK for us)
//#define SHADOW_MAP_WIDTH (SHADOW_MAP_HEIGHT*2)  // 1024 2048 (or 1536 if non-power-of-two textures are supported: WebGL1 supports them if no GL_REPEAT and no mipmaps are used; that's OK for us)


#define SHADOW_MAP_CLAMP_MODE GL_CLAMP_TO_EDGE // GL_CLAMP or GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER
    //          GL_CLAMP;               // sampling outside of the shadow map gives always shadowed pixels
    //          GL_CLAMP_TO_EDGE;       // sampling outside of the shadow map can give shadowed or unshadowed pixels (it depends on the edge of the shadow map)
    //          GL_CLAMP_TO_BORDER;     // sampling outside of the shadow map gives always non-shadowed pixels (if we set the border color correctly)
#define SHADOW_MAP_FILTER GL_LINEAR // GL_LINEAR or GL_NEAREST (GL_LINEAR is more useful with a sampler2DShadow, that cannot be used with esponential shadow mapping)


//#define CREATE_DEBUG_CONTEXT    // to trap OpenGL errors


#include <stdio.h>
#include <string.h>
//#include <time.h>   /* clock_t */
//#include <ctype.h>  /* tolower */

#ifndef __FREEGLUT_STD_H__
// original glut lacks solid cylinders
// Here we implement it based on the freeGlut source code
/*
 * freeglut_geometry.c
 *
 * Freeglut geometry rendering methods.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
static void fghCircleTable(double **sint,double **cost,const int n)
{
    /*
     * Compute lookup table of cos and sin values forming a cirle
     *
     * Notes:
     *    It is the responsibility of the caller to free these tables
     *    The size of the table is (n+1) to form a connected loop
     *    The last entry is exactly the same as the first
     *    The sign of n can be flipped to get the reverse loop
     */
    int i;

    /* Table size, the sign of n flips the circle direction */

    const int size = abs(n);

    /* Determine the angle between samples */

    const double angle = 2*M_PI/(double)( ( n == 0 ) ? 1 : n );

    /* Allocate memory for n samples, plus duplicate of first entry at the end */

    *sint = (double *) calloc(sizeof(double), size+1);
    *cost = (double *) calloc(sizeof(double), size+1);

    /* Bail out if memory allocation fails, fgError never returns */

    if (!(*sint) || !(*cost))
    {
        free(*sint);
        free(*cost);
        fprintf(stderr,"Failed to allocate memory in fghCircleTable");
    }

    /* Compute cos and sin around the circle */

    (*sint)[0] = 0.0;
    (*cost)[0] = 1.0;

    for (i=1; i<size; i++)
    {
        (*sint)[i] = sin(angle*i);
        (*cost)[i] = cos(angle*i);
    }

    /* Last sample is duplicate of the first */

    (*sint)[size] = (*sint)[0];
    (*cost)[size] = (*cost)[0];
}
void glutSolidCylinder(double radius,double height,double slices,double stacks)
{
    int i,j;

    /* Step in z and radius as stacks are drawn. */

    double z0,z1;
    const double zStep = height / ( ( stacks > 0 ) ? stacks : 1 );

    /* Pre-computed circle */

    double *sint,*cost;
    fghCircleTable(&sint,&cost,-slices);

    /* Cover the base and top */

    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0.0, 0.0, -1.0 );
    glVertex3d(0.0, 0.0,  0.0 );
    for (j=0; j<=slices; j++)
        glVertex3d(cost[j]*radius, sint[j]*radius, 0.0);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3d(0.0, 0.0, 1.0   );
    glVertex3d(0.0, 0.0, height);
    for (j=slices; j>=0; j--)
        glVertex3d(cost[j]*radius, sint[j]*radius, height);
    glEnd();

    /* Do the stacks */

    z0 = 0.0;
    z1 = zStep;

    for (i=1; i<=stacks; i++)
    {
        if (i==stacks)
            z1 = height;

        glBegin(GL_QUAD_STRIP);
        for (j=0; j<=slices; j++ )
        {
            glNormal3d(cost[j],        sint[j],        0.0 );
            glVertex3d(cost[j]*radius, sint[j]*radius, z0  );
            glVertex3d(cost[j]*radius, sint[j]*radius, z1  );
        }
        glEnd();

        z0 = z1; z1 += zStep;
    }

    /* Release sin and cos tables */

    free(sint);
    free(cost);
}
#endif


GLhandleARB gl_helper_LoadShader(const char* buffer, const unsigned int type)
{
    GLhandleARB handle;
    const GLcharARB* files[1];

    // shader Compilation variable
    GLint result;				// Compilation code result
    GLint errorLoglength ;
    char* errorLogText;
    GLsizei actualErrorLogLength;

    handle = glCreateShader(type);
    if (!handle)
    {
        //We have failed creating the vertex shader object.
        printf("Failed creating vertex shader object.\n");
        exit(0);
    }

    files[0] = (const GLcharARB*)buffer;
    glShaderSource(
                handle, //The handle to our shader
                1, //The number of files.
                files, //An array of const char * data, which represents the source code of theshaders
                NULL);

    glCompileShader(handle);

    //Compilation checking.
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);

    // If an error was detected.
    if (!result)
    {
        //We failed to compile.
        printf("Shader failed compilation.\n");

        //Attempt to get the length of our error log.
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &errorLoglength);

        //Create a buffer to read compilation error message
        errorLogText =(char*) malloc(sizeof(char) * errorLoglength);

        //Used to get the final length of the log.
        glGetShaderInfoLog(handle, errorLoglength, &actualErrorLogLength, errorLogText);

        // Display errors.
        printf("%s\n",errorLogText);

        // Free the buffer malloced earlier
        free(errorLogText);
    }

    return handle;
}
GLuint gl_helper_LoadShaderProgramFromSourceWithGeometryShader(const char* vs,const char* gs,const char* fs)	{
    // shader Compilation variable
    GLint result;				// Compilation code result
    GLint errorLoglength ;
    char* errorLogText;
    GLsizei actualErrorLogLength;

    GLhandleARB vertexShaderHandle = 0;
    GLhandleARB geometryShaderHandle = 0;
    GLhandleARB fragmentShaderHandle = 0;
    GLuint programId = 0;

    if (vs) vertexShaderHandle   = gl_helper_LoadShader(vs,GL_VERTEX_SHADER);
    if (gs) geometryShaderHandle   = gl_helper_LoadShader(vs,GL_GEOMETRY_SHADER);
    if (fs) fragmentShaderHandle = gl_helper_LoadShader(fs,GL_FRAGMENT_SHADER);
    if (!vertexShaderHandle || !fragmentShaderHandle) return 0;

    programId = glCreateProgram();

    glAttachShader(programId,vertexShaderHandle);
    if (geometryShaderHandle) glAttachShader(programId,geometryShaderHandle);
    glAttachShader(programId,fragmentShaderHandle);
    glLinkProgram(programId);

    //Link checking.
    glGetProgramiv(programId, GL_LINK_STATUS, &result);

    // If an error was detected.
    if (!result)
    {
        //We failed to compile.
        printf("Program failed to link.\n");

        //Attempt to get the length of our error log.
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &errorLoglength);

        //Create a buffer to read compilation error message
        errorLogText =(char*) malloc(sizeof(char) * errorLoglength);

        //Used to get the final length of the log.
        glGetProgramInfoLog(programId, errorLoglength, &actualErrorLogLength, errorLogText);

        // Display errors.
        printf("%s\n",errorLogText);

        // Free the buffer malloced earlier
        free(errorLogText);
    }

    glDeleteShader(vertexShaderHandle);
    if (geometryShaderHandle) glDeleteShader(geometryShaderHandle);
    glDeleteShader(fragmentShaderHandle);

    return programId;
}
__inline GLuint gl_helper_LoadShaderProgramFromSource(const char* vs,const char* fs)    {return gl_helper_LoadShaderProgramFromSourceWithGeometryShader(vs,0,fs);}



// Config file handling: basically there's an .ini file next to the
// exe that you can tweak. (it's just an extra)
static const char* ConfigFileName = PROGRAM_NAME".ini";
typedef struct {
    int fullscreen_width,fullscreen_height;
    int windowed_width,windowed_height;
    int fullscreen_enabled;
    int show_fps;
    int use_camera_ortho3d_projection_matrix;
} Config;
void Config_Init(Config* c) {
    c->fullscreen_width=c->fullscreen_height=0;
    c->windowed_width=960;c->windowed_height=540;
    c->fullscreen_enabled=0;
    c->show_fps = 0;
    c->use_camera_ortho3d_projection_matrix = 0;
}
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
               case 4:
               sscanf(buf, "%d", &c->use_camera_ortho3d_projection_matrix);
               break;
           }
           nread=0;
           ++numParsedItem;
        }
    }
    fclose(f);
    if (c->windowed_width<=0) c->windowed_width=720;
    if (c->windowed_height<=0) c->windowed_height=405;
#   ifdef __EMSCRIPTEN__
    c->fullscreen_enabled = 0;
#   endif
    return 0;
}
int Config_Save(Config* c,const char* filePath)  {
    FILE* f = fopen(filePath, "wt");
    if (!f)  return -1;
    fprintf(f, "[Size In Fullscreen Mode (zero means desktop size)]\n%d %d\n",c->fullscreen_width,c->fullscreen_height);
    fprintf(f, "[Size In Windowed Mode]\n%d %d\n",c->windowed_width,c->windowed_height);
    fprintf(f, "[Fullscreen Enabled (0 or 1) (CTRL+RETURN)]\n%d\n", c->fullscreen_enabled);
    fprintf(f, "[Show FPS (0 or 1) (F2)]\n%d\n", c->show_fps);
    fprintf(f, "[Use camera ortho3d projection matrix (0 or 1) (F1)]\n%d\n", c->use_camera_ortho3d_projection_matrix);
    fprintf(f,"\n");
    fclose(f);
    return 0;
}

static Config config;

// glut has a special fullscreen GameMode that you can toggle with CTRL+RETURN (not in WebGL)
static int windowId = 0; 			// window Id when not in fullscreen mode
static int gameModeWindowId = 0;	// window Id when in fullscreen mode

// Now we can start with our program

// camera data:
static choat targetPos[3];         // please set it in resetCamera()
static choat cameraYaw;            // please set it in resetCamera()
static choat cameraPitch;          // please set it in resetCamera()
static choat cameraDistance;       // please set it in resetCamera()
static choat cameraPos[3];         // Derived value (do not edit)
static choat vMatrix[16];          // view matrix
static choat cameraSpeed = 0.5;    // When moving it

// light data
static choat lightYaw = (choat)(M_PI*0.425);
static choat lightPitch = (choat)(M_PI*0.235);   // must be copied to resetLight() too
static choat lightDirection[4] = {0,1,0,0};      // Derived value (do not edit) [lightDirection[3]==0]

// pMatrix data:
static float pMatrix[16],pMatrixInverse[16];    // projection matrix
static float pMatrixFovyDeg = 45.;      //
static float pMatrixNearPlane = 0.5;    //
static float pMatrixFarPlane = 35.;    //
static choat cameraFrustumCenterDistance = 0;
static choat cameraFrustumRadius = 0;

static float pMatrixNormalizedFrustumPlanes[6][4]; // used for frustum culling: warning: must be float!

const GLfloat globalBackgroundColor[4]={0.5f,0.7f,1.0f,1.0f};

static float instantFrameTime = 16.2; // in seconds
static float totalFrameTime = 0.f;     // in seconds

static int isJustSwitchingFullscreen = 0;

static struct cha_character_group* group = NULL;
static struct cha_character_instance* g_character_instance_selected = NULL;


#ifndef __EMSCRIPTEN__
static void GlutDraw3DPivot(const choat* cameraTargetPosition3,GLuint* pOptionalDisplayListBase);   /* optional */
#endif //__EMSCRIPTEN__


static const char* ShadowPassVertexShader[] = {
    "   void main() {\n"
    "       gl_Position = ftransform();\n"
    "   }\n"

};
static const char* ShadowPassFragmentShader[] = {
    "   void main() {\n"
    "       //gl_FragColor =  gl_Color;\n"
    "   }\n"
};
typedef struct {
    GLuint fbo;
    GLuint textureId;
    GLuint program;
} ShadowPass;

ShadowPass shadowPass;
void InitShadowPass()	{
    ShadowPass* sp = &shadowPass;
    sp->program = gl_helper_LoadShaderProgramFromSource(*ShadowPassVertexShader,*ShadowPassFragmentShader);

    // create depth texture
    glGenTextures(1, &sp->textureId);
    glBindTexture(GL_TEXTURE_2D, sp->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, SHADOW_MAP_FILTER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, SHADOW_MAP_FILTER);
#   ifndef __EMSCRIPTEN__
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
#   else //__EMSCRIPTEN__
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 0);
#   undef SHADOW_MAP_CLAMP_MODE
#   define SHADOW_MAP_CLAMP_MODE GL_CLAMP_TO_EDGE
#   endif //__EMSCRIPTEN__
    if (SHADOW_MAP_CLAMP_MODE==GL_CLAMP_TO_BORDER)  {
        const GLfloat border[] = {1.0f,1.0f,1.0f,0.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    }
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, SHADOW_MAP_CLAMP_MODE );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, SHADOW_MAP_CLAMP_MODE );
    glBindTexture(GL_TEXTURE_2D, 0);

    // create depth fbo
    glGenFramebuffers(1, &sp->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, sp->fbo);
#   ifndef __EMSCRIPTEN__
    glDrawBuffer(GL_NONE); // Instruct openGL that we won't bind a color texture with the currently bound FBO
    glReadBuffer(GL_NONE);
#   endif //__EMSCRIPTEN__
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,sp->textureId, 0);
    {
        //Does the GPU support current FBO configuration?
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status!=GL_FRAMEBUFFER_COMPLETE) printf("glCheckFramebufferStatus(...) FAILED for shadowPass.fbo.\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void DestroyShadowPass()	{
    ShadowPass* sp=&shadowPass;
    if (sp->program) {glDeleteProgram(sp->program);sp->program=0;}
    if (sp->fbo) {glDeleteBuffers(1,&sp->fbo);sp->fbo=0;}
    if (sp->textureId) {glDeleteTextures(1,&sp->textureId);}
}

static const char* DefaultPassVertexShader[] = {
    "uniform mat4 u_biasedShadowMvpMatrix;\n"   // (*) Actually it's already multiplied with vMatrixInverse (in C code, so that the multiplication can be easily done with doubles)
    "uniform vec4 u_fogDistances;\n"
    "uniform vec3 u_fogColor;\n"
    "varying float v_fog;\n"
    "varying vec4 v_shadowCoord;\n"
    "varying vec4 v_ambient;\n"
    "varying vec4 v_diffuse;\n"
    "varying vec4 v_specular;\n"
    "\n"
    "void main()	{\n"
    "   gl_Position = ftransform();\n"
    "\n"
    "   vec4 vertexScaledEyeSpace = gl_ModelViewMatrix* gl_Vertex;"
#   ifndef DONT_USE_NORMAL_MATRIX_IN_SHADERS
    "	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);\n"    // Here we have gl_NormalMatrix for free, but in general we must calculate it and pass it to the shader
#   else
    // https://lxjk.github.io/2017/10/01/Stop-Using-Normal-Matrix.html
    // [this code should be almost 100% equivalent to using a normal matrix  (3 dot product to get squared scale from model matrix, 3 reciprocal and multiply per vertex vs nothing but gl_NormalMatrix calculation in C++ and passing it as an uniform).]
    // But the author says: 'For non-uniform scale this method is FASTER then sending over a normal matrix to shader or doing inverse transpose in shader.'
    // Well, I'm not sure (doesn't it depend on the number of vertices? And are uniforms so slow?). In any case it's a good thing to stop writing code for the normal matrix in C++!
    "   vec3 normal_coeff=vec3(1.0/dot(gl_ModelViewMatrix[0].xyz,gl_ModelViewMatrix[0].xyz),1.0/dot(gl_ModelViewMatrix[1].xyz,gl_ModelViewMatrix[1].xyz),1.0/dot(gl_ModelViewMatrix[2].xyz,gl_ModelViewMatrix[2].xyz));\n"
    "   //vec3 normal = normalize(mat3(gl_ModelViewMatrix)*(gl_Normal*normal_coeff));\n"
    "	vec3 normal = normalize(vec3(gl_ModelViewMatrix * vec4(gl_Normal.xyz*normal_coeff,0.0)));\n"    // same as line above if we don't have the mat3 cast (or if this is faster... and it would be useful to know it!)
#   endif
    "\n"
//#   define CARTOON_STYLE    // but it should be applied to characters only...
#   ifdef CARTOON_STYLE
    "   const vec3 zAxis = vec3(0.0,0.0,1.0);\n"
    "   normal = dot(normal,zAxis)>0.05?zAxis:(0.2*normal);\n"
#   endif
    "\n"
    "	vec3 lightVector = /*normalize(*/gl_LightSource[0].position.xyz/*)*/;\n"   // already in eye space
    "	float nxDir = max(0.0, dot(normal, lightVector));\n"
    "   \n"
    "   v_ambient = gl_FrontLightProduct[0].ambient;\n"
    "   v_ambient = clamp(v_ambient,0.0,1.0);"
    "	v_diffuse = gl_FrontLightProduct[0].diffuse*nxDir;\n"
    "   v_diffuse = clamp(v_diffuse,0.0,1.0);\n"
    "   vec3 E = normalize(-vertexScaledEyeSpace.xyz);\n"
    "   vec3 halfVector = normalize(lightVector + E);\n"
    "   float nxHalf = max(0.005,dot(normal, halfVector));\n"
    "   v_specular = gl_FrontLightProduct[0].specular * pow(nxHalf,gl_FrontMaterial.shininess);\n"
    "   //v_specular = vec4(clamp(gl_FrontLightProduct[0].specular*10.0,0.0,1.0)*halfVector,1.0);\n"
    "   v_specular = clamp(v_specular,0.0,1.0);\n"
    "\n"
    "	//gl_FrontColor = gl_Color;\n"
    "\n"
    "   v_fog = 1.0 - min((u_fogDistances.y+vertexScaledEyeSpace.z)*u_fogDistances.w,1.0);\n"
    "\n"
    "   v_shadowCoord = u_biasedShadowMvpMatrix*vertexScaledEyeSpace;\n"  // vertexScaledEyeSpace=gl_ModelViewMatrix* gl_Vertex;    (*) This value must NOT depend on the camera at all! But since we don't pass a 'naked' mMatrix in shaders (not robust to double precision usage), we dress it in a mvMatrix. So here we're passing a mMatrix from camera space to light space (through a mvMatrix).
    "}\n"                                                                           // (the bias just converts clip space to texture space)
};
static const char* DefaultPassFragmentShader[] = {
    "uniform sampler2D u_shadowMap;\n"
    "uniform vec2 u_shadowDarkening;\n" // .x = fDarkeningFactor [10.0-80.0], .y = min value clamp [0.0-1.0]
    "uniform vec3 u_fogColor;\n"
    "varying float v_fog;\n"
    "varying vec4 v_shadowCoord;\n"
    "varying vec4 v_ambient;\n"
    "varying vec4 v_diffuse;\n"
    "varying vec4 v_specular;\n"
    "\n"
    "void main() {\n"
    "	float shadowFactor = 1.0;\n"
    "	vec4 shadowCoordinateWdivide = v_shadowCoord/v_shadowCoord.w;\n"
    "   shadowFactor = clamp(exp(u_shadowDarkening.x*(texture2D(u_shadowMap,(shadowCoordinateWdivide.st)).r - shadowCoordinateWdivide.z)),u_shadowDarkening.y,1.0);\n"
    "//   shadowFactor = clamp(   exp(u_shadowDarkening.x*texture2D(u_shadowMap,(shadowCoordinateWdivide.st)).r) *   exp(-u_shadowDarkening.x*shadowCoordinateWdivide.z),u_shadowDarkening.y,1.0);\n"
    "	gl_FragColor.rgb = mix(gl_FrontLightModelProduct.sceneColor.rgb + v_ambient.rgb + \n"
    "   ((v_diffuse.rgb /*gl_Color.rgb*/+v_specular.rgb) * shadowFactor),u_fogColor,v_fog);\n"
    "   gl_FragColor.a=v_diffuse.a;\n"  // mmmh, changing this does nothing
    "   \n"
    //    TEST: we could store materials as an int[4] (packing ambient,diffuse,specular,shininess: the latter with full 32-bit range), instead of using 12 floats + 1 more float for shininess
    //    [or we can keep floats in a single array: uniform vec4 material_data[4]; with 3 padding floats at the end. This is for using a single glUniform4fv(...) call per material]
    //    Integers are better if we want to compare them on the CPU to avoid useless glUniform4iv(...) calls, but we lose precision and we must decode them on the GPU.
    //"   const float inv255 = 1.0/255.0;\n"
    //"   int u_col = (((int)(1.0*255.0)) | ((int)(1.0*255.0)<<8) | ((int)(0.0*255.0)<<16) | ((int)(1.0*255.0)<<24));\n"    // on the CPU
    //"   vec4 rgba = vec4(float(u_col&0xFF)*inv255,float((u_col>>8)&0xFF)*inv255,float((u_col>>16)&0xFF)*inv255,float((u_col>>24)&0xFF)*inv255);\n"
    //"   gl_FragColor=rgba;\n"
    "   \n"
    "}\n"
};
typedef struct {
    GLuint program;
    GLint uniform_location_biasedShadowMvpMatrix;
    GLint uniform_location_shadowMap;
    GLint uniform_location_shadowDarkening;
    GLint uniform_location_fogColor,uniform_location_fogDistances,uniform_location_inv_scaling_squared;

} DefaultPass;

DefaultPass defaultPass;
void InitDefaultPass()	{
    DefaultPass* dp = &defaultPass;float fog_start,fog_end;
    dp->program = gl_helper_LoadShaderProgramFromSource(*DefaultPassVertexShader,*DefaultPassFragmentShader);
    dp->uniform_location_biasedShadowMvpMatrix = glGetUniformLocation(dp->program,"u_biasedShadowMvpMatrix");
    dp->uniform_location_shadowMap = glGetUniformLocation(dp->program,"u_shadowMap");
    dp->uniform_location_shadowDarkening = glGetUniformLocation(dp->program,"u_shadowDarkening");
    dp->uniform_location_fogColor = glGetUniformLocation(dp->program,"u_fogColor");
    dp->uniform_location_fogDistances = glGetUniformLocation(dp->program,"u_fogDistances");
    dp->uniform_location_inv_scaling_squared = glGetUniformLocation(dp->program,"u_inv_scaling_squared");

    glUseProgram(dp->program);
    glUniform1i(dp->uniform_location_shadowMap,0);
    glUniform2f(dp->uniform_location_shadowDarkening,40.0,0.75);	// Default values are (40.0f,0.75f) in [0-80] and [0-1]
    //glUniformMatrix4fv(dp->uniform_location_biasedShadowMvpMatrix, 1 /*only setting 1 matrix*/, GL_FALSE /*transpose?*/, Matrix);
    // We use fog to prevent ad clipping artifacts, so it just needs the near and far plane
    fog_start = (pMatrixFarPlane-pMatrixNearPlane)*0.75f;    // We start the fog at the half point... but it works better nearer when farPlane is far away
    fog_end = pMatrixFarPlane;
    glUniform4f(dp->uniform_location_fogDistances,fog_start,fog_end,fog_end-fog_start,1.0/(fog_end-fog_start));
    glUniform3f(dp->uniform_location_fogColor,globalBackgroundColor[0], globalBackgroundColor[1], globalBackgroundColor[2]); // it should be the same as glClearColor()
    glUniform4f(dp->uniform_location_inv_scaling_squared,1.f,1.f,1.f,1.f);
    glUseProgram(0);
}
void DestroyDefaultPass()	{
    DefaultPass* dp = &defaultPass;
    if (dp->program) {glDeleteProgram(dp->program);dp->program=0;}
}

static choat current_width=0,current_height=0,current_aspect_ratio=1;  // Not sure when I've used these...
void ResizeGL(int w,int h) {
    current_width = (choat) w;
    current_height = (choat) h;
    if (current_height!=(choat)0) current_aspect_ratio = current_width/current_height;
    if (h>0)	{
        // We set our pMatrix
        if (!config.use_camera_ortho3d_projection_matrix)
            chm_Mat4Perspectivef(pMatrix,pMatrixFovyDeg,(float)w/(float)h,pMatrixNearPlane,pMatrixFarPlane);
        else
            chm_Mat4Ortho3Df(pMatrix,cameraDistance,pMatrixFovyDeg,(float)w/(float)h,pMatrixNearPlane,pMatrixFarPlane);

#       ifndef __EMSCRIPTEN__
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(pMatrix);
        glMatrixMode(GL_MODELVIEW);
#       endif //__EMSCRIPTEN__
        

//        DefaultPassSetPMatrix(pMatrix,1);

//#       ifdef USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE
        chm_Mat4Invertf(pMatrixInverse,pMatrix);    // pMatrixInverse is mandatory only when USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE is defined (but can be used for better shadow frustum culling in any case)
//#       endif //USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE

        // Needed for shadow mapping and frustum culling ('cameraFrustumRadius' and 'cameraFrustumCenterDistance' depend only by pMatrix-related data)
        cameraFrustumRadius = chm_GetFrustumRadiusAndCenterDistance(&cameraFrustumCenterDistance,pMatrixNearPlane,pMatrixFarPlane,pMatrixFovyDeg,current_aspect_ratio,config.use_camera_ortho3d_projection_matrix?cameraDistance:0);

        //printf("cameraFrustumRadius=%1.4f cameraFrustumCenterDistance=%1.4f\n",cameraFrustumRadius,cameraFrustumCenterDistance);
        chm_GetFrustumPlaneEquationsf(pMatrixNormalizedFrustumPlanes,pMatrix,1);


    }


    if (w>0 && h>0 && !config.fullscreen_enabled) {
        // On exiting we'll like to save these data back
        config.windowed_width=w;
        config.windowed_height=h;
    }

    glViewport(0,0,w,h);    // This is what people often call in ResizeGL()

}

void InitGL(void) {
    char windowTitle[1024+4096] = PROGRAM_NAME".c";
    size_t i;
    const float globalAmbientLightModel[4] = {0.2f,0.2f,0.2f,1.0f};
    const float lightAmbientColor[4] = {0.2f,0.2f,0.2f,1.0f};
    const float lightDiffuseColor[4] = {1.0f,1.0f,1.0f,1.0f};
    const float lightSpecularColor[4] = {1.f,1.f,1.f,1.0f};

    // These are important, but often overlooked OpenGL calls
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(globalBackgroundColor[0],globalBackgroundColor[1],globalBackgroundColor[2],globalBackgroundColor[3]);
    glEnable(GL_TEXTURE_2D);

    // ffp stuff
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0,GL_AMBIENT,lightAmbientColor);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightDiffuseColor);
    glLightfv(GL_LIGHT0,GL_SPECULAR,lightSpecularColor);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,globalAmbientLightModel);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);

    glLineWidth(1.f);glEnable(GL_LINE_SMOOTH);
    glPolygonOffset(2.0, 4);

#   ifndef __EMSCRIPTEN__
    glutSetWindowTitle(windowTitle);
    glDisable(GL_LIGHTING);
#   endif

    Character_Init();
    group = Character_CreateGroup(3,3,1.85f,1.75f,  0.0115f,1,0.04f);
    CHA_ASSERT(group && group->num_men>0 && group->num_ladies>0);   // they are used in the demo

    InitShadowPass();
    InitDefaultPass();

    // Please note that after InitGL(), this implementation calls ResizeGL(...,...).
    // If you copy/paste this code you can call it explicitly...

    // leg-strtching to make character taller
    //cha_character_instance_set_vertical_stretching(&group->instances[1]/*&group->men[CHA_CHARACTER_INSTANCE_MAN_NAME_PETER]*/,0.5f);

    /*{   // optional:
        struct cha_character_instance* peter = &group->instances[CHA_CHARACTER_INSTANCE_MAN_NAME_PETER];
        const float specular_yellow[4]= {0.9f,0.9f,0.f,1.f};
        const float diffuse_yellow[4] = {0.7f,0.7f,0.f,1.f};
        const float ambient_yellow[4] = {0.2f,0.2f,0.f,1.f};
        struct cha_material* tshirt = &peter->materials[CHA_MATERIAL_NAME_T_SHIRT];
        for (i=0;i<4;i++) {
            tshirt->amb[i]=ambient_yellow[i];
            tshirt->dif[i]=diffuse_yellow[i];
            tshirt->spe[i]=specular_yellow[i];
        }
    }

    {   // optional
        struct cha_character_instance* mary = &group->instances[CHA_CHARACTER_INSTANCE_LADY_NAME_MARY];
        const float diffuse_red[4] = {0.7f,0.15f,0.15f,1.f};
        const float diffuse_blue[4] = {0.25f,0.25f,0.7f,1.f};
        struct cha_material* mary_tshirt = &mary->materials[CHA_MATERIAL_NAME_T_SHIRT];
        struct cha_material* mary_trousers = &mary->materials[CHA_MATERIAL_NAME_TROUSERS];
        for (i=0;i<4;i++) {
            mary_tshirt->dif[i]=diffuse_red[i];
            mary_trousers->dif[i]=diffuse_blue[i];
        }
        // well, we should change amb,and spe too...
    }*/

}

void DestroyGL() {

    DestroyShadowPass();
    DestroyDefaultPass();

    g_character_instance_selected = NULL;
    if (group) {Character_DestroyGroup(group);group=NULL;}
    Character_Destroy();
}

void glDrawBoneCallback(const struct cha_mesh_instance* mi,int bone_idx,const float* mvMatrix16,float length,void* userData)   {
    (void)mi;(void)bone_idx;(void)userData;
    // we draw the bone (using display lists here would speed up things a lot)
    glPushMatrix();
    glLoadMatrixf(mvMatrix16); /* this will stay single-precision */
    glRotatef(-90.f,1.f,0.f,0.f);
    glRotatef(45.f/*+roll*/,0.f,0.f,1.f);   /* 'roll' is probably already embedded in the matrices */
    glPushMatrix();
    glutSolidSphere(length*0.1f,4,4);
    glTranslatef(0.f,0.f,length*0.25f);
    glPushMatrix();
    glutSolidCone(length*0.2f,length*0.75f,4,1);
    glScalef(1.f,1.f,-1.f);
    glFrontFace(GL_BACK);
    glutSolidCone(length*0.2f,length*0.25f,4,1);
    glFrontFace(GL_FRONT);
    glPopMatrix();
    glTranslatef(0.f,0.f,length*0.75f);
    glutSolidSphere(length*0.05f,4,4);
    glPopMatrix();
    glPopMatrix();

}
void cha_mesh_instance_draw_armature_opengl(const struct cha_mesh_instance* mi)  {
    CHA_ASSERT(mi->armature);
    {
        const unsigned deselected_mask = CHA_BONE_MASK_INVERSE(mi->selected_bone_mask);
        const unsigned   selected_mask = mi->selected_bone_mask;
        glLineWidth(1.f);
        glDepthMask(GL_FALSE);glDisable(GL_DEPTH_TEST);
        //glEnable(GL_BLEND);
        if (deselected_mask) {
            glColor4f(0.8f,0.8f,0.f,1.f);
            cha_mesh_instance_draw_armature(mi,deselected_mask,&glDrawBoneCallback,NULL);
        }
        if (  selected_mask) {
            glColor4f(0.4f,0.4f,0.8f,1.f);
            cha_mesh_instance_draw_armature(mi,  selected_mask,&glDrawBoneCallback,NULL);
        }
        cha_mesh_instance_draw_armature(mi,  selected_mask,&glDrawBoneCallback,NULL);
        //glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        if (deselected_mask) {
            glColor4f(0.6f,0.4f,0.f,1.f);
            cha_mesh_instance_draw_armature(mi,deselected_mask,&glDrawBoneCallback,NULL);
        }
        if (  selected_mask) {
            glColor4f(0.2f,0.2f,0.4f,1.f);
            cha_mesh_instance_draw_armature(mi,  selected_mask,&glDrawBoneCallback,NULL);
        }
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor4f(0.8f,0.8f,0.8f,1.f);
        glEnable(GL_DEPTH_TEST);glDepthMask(GL_TRUE);
    }
}

extern void DrawGeometry(int isShadowPass);

void CharacterGroupMoveAndAnimate(float totalTimeSeconds,float frameTimeSeconds) {

    int i;
    struct cha_character_instance* peter = &group->men[CHA_CHARACTER_INSTANCE_MAN_NAME_PETER];
    struct cha_character_instance* mary = &group->ladies[CHA_CHARACTER_INSTANCE_LADY_NAME_MARY];
    struct cha_mesh_instance* peter_mi = &peter->mesh_instances[CHA_MESH_NAME_BODY];
    struct cha_mesh_instance* mary_mi = &mary->mesh_instances[CHA_MESH_NAME_BODY];
    const struct cha_armature* armature = peter_mi->armature;
    float character_animation_walk_run_mix_in_0_1 = sinf(totalTimeSeconds*0.1f)+0.5f;//  will be clamped to [0,1] later
    float character_animation_speed = 1.f;//    /* in [-1,3]*/1.0f;

    CHA_ASSERT(group && group->num_men>=1 && group->num_ladies>=1);

    CHA_ASSERT(armature->num_bones>0 && armature->bones[0].parent_idx==-1);
    CHA_ASSERT(mary_mi->armature==peter_mi->armature);

    /* clamp 'character_animation_run_walk_mix' in [0,1] */
    if (character_animation_walk_run_mix_in_0_1<0.f)   {
        character_animation_speed = 1.f-(character_animation_walk_run_mix_in_0_1*character_animation_walk_run_mix_in_0_1);
        //character_animation_speed = 1.f+character_animation_walk_run_mix_in_0_1;
        if (character_animation_speed<0.5f) character_animation_speed=0.5f;
        character_animation_walk_run_mix_in_0_1 = 0.f;
    }
    else if (character_animation_walk_run_mix_in_0_1>1.f)  {
        character_animation_speed = character_animation_walk_run_mix_in_0_1;
        if (character_animation_speed>1.2f) character_animation_speed=1.2f;
        character_animation_walk_run_mix_in_0_1 = 1.f;
    }
    else character_animation_speed = 1.f;

    CHA_ASSERT((character_animation_walk_run_mix_in_0_1==0.f && character_animation_speed<=1.f) ||
               (character_animation_walk_run_mix_in_0_1==1.f && character_animation_speed>=1.f) ||
               (character_animation_speed==1.f));


    chm_Mat4Identity(peter->mMatrixIn);


#   if 1
    {   // TEST: manual animating some bones
        struct cha_mesh_instance* mi = mary_mi;
        /*const float amount = 20.f*sinf(totalTimeSeconds*2.f);int j;
        for (j=1;j<4;j++)   {
            float* m = &mi->pose_matrices[CHA_BONE_SPACE_BONE][j*16];
            struct cha_mesh_instance_pose_data* pose_data = &mi->pose_data[j];
            chm_Mat4ClearRotation(m);
            chm_Mat4Rotate(m,amount,0.f,0.f,1.f);
            pose_data->rot_dirty = 2;   // 2 means that we have updated 'pose_matrices[CHA_BONE_SPACE_BONE]'; 1 that we have updated 'pose_data' directly
        }*/
        /*if (CHA_BONE_NAME_LOWERARM_WRIST_L>0)    {
            float* m = &mi->pose_matrices[CHA_BONE_SPACE_BONE][CHA_BONE_NAME_LOWERARM_WRIST_L*16];
            struct cha_mesh_instance_pose_data* pose_data = &mi->pose_data[CHA_BONE_NAME_LOWERARM_WRIST_L];
            m[13]=0.75f;
            pose_data->tra_dirty = 2;
        }*/
        // test CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION (to remove)
        if (CHA_BONE_NAME_ROOT>-1)    {
            float* m = &mi->pose_matrices[CHA_BONE_SPACE_BONE][CHA_BONE_NAME_ROOT*16];
            struct cha_mesh_instance_pose_data* pose_data = &mi->pose_data[CHA_BONE_NAME_ROOT];
            const float amount = sinf(totalTimeSeconds*0.5f);
            //chm_Mat4Rotate(m,5.f*amount,0.f,1.f,0.f);pose_data->rot_dirty = 2;
            m[14]=5.f*amount;  // y vertical
            //m[13]=2.f*amount;  // y vertical
            pose_data->tra_dirty = 2;
            //chm_Mat4Rotate(mary->mMatrix,360.f*amount,0.f,1.f,0.f);
        }
    }
#   elif 1
    {   // TEST: skeletal animation
        struct cha_mesh_instance* mi = mary_mi;
        const float animation_speed = 1.f;
        const float additional_time_to_get_to_first_frame = 0.5f*animation_speed;    // this value MUST be constant through all the animation time (i.e. if 'animation_speed' is not constant it MUST be removed from here).
        const float animation_time=totalTimeSeconds*animation_speed;      // good only for when 'animation_speed' is constant through all the animation time
        //static float animation_time=0;animation_time+=frameTimeSeconds*animation_speed;   // good always

        cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,CHA_ARMATURE_ACTION_NAME_FALL_DOWN,animation_time,additional_time_to_get_to_first_frame,0);
        //cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,CHA_ARMATURE_ACTION_NAME_RUN_CYCLE,animation_time,additional_time_to_get_to_first_frame,0);

    }
#   endif
#   if 1
    /* code used to make peter walk/run (in place) */
    {   // TEST: skeletal animation
        struct cha_mesh_instance* mi = peter_mi;
        const float additional_time_to_get_to_first_frame = 1.0f;    // seconds
        static float animation_time = 0.f;
        animation_time+= (frameTimeSeconds)*character_animation_speed;
        cha_mesh_instance_calculate_bone_space_pose_matrices_from_action_ex(mi,CHA_ARMATURE_ACTION_NAME_CYCLE_RUN,animation_time,additional_time_to_get_to_first_frame,character_animation_walk_run_mix_in_0_1,CHA_ARMATURE_ACTION_NAME_CYCLE_WALK,0,-1,0);//CHA_BONE_MASK_ARMS_AND_HANDS);
        //cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,CHA_ARMATURE_ACTION_NAME_CYCLE_WALK,animation_time,additional_time_to_get_to_first_frame,0);
    }
#   endif

    /* code used to make all characters blink and speak */
    {
        const float sinBlink = sinf(totalTimeSeconds)-0.98f;
        const float amountBiink = sinBlink<0.f?0.f:(sinBlink*50.f); /* must be in [0,1] */
        const float sinSpeak = sinf(totalTimeSeconds*8.f);
        const float amountSpeak = 0.5f+0.5f*sinSpeak;               /* must be in [0,1] */
        for (i=0;i<group->num_instances;i++)  {
            {
                /* Manual eye shape-key animation (blink) */
                struct cha_mesh_instance* mi = &group->instances[i].mesh_instances[CHA_MESH_NAME_EYE];

                mi->shk_values[CHA_EYE_SHAPE_KEY_NAME_CLOSED] = amountBiink;
                mi->shk_values_dirty=1;
            }
            {
                /* Manual mouth shape-key animation (speak) */
                struct cha_mesh_instance* mi = &group->instances[i].mesh_instances[CHA_MESH_NAME_MOUTH];

                mi->shk_values[CHA_MOUTH_SHAPE_KEY_NAME_OPEN] = amountSpeak;
                mi->shk_values_dirty=1;
            }
        }
    }

    /* code used to make peter walk around: */
#   if 1
    {
        choat* m = peter->mMatrixIn;  // peter
        const float scalingY = peter->scaling[1];
        static float amount = 0.f;
        const float speed_mix_factor = (character_animation_walk_run_mix_in_0_1*0.5f)+0.5f; // maps [0,1] to [.5,1]
        const float radius = 7.f, angular_speed = character_animation_speed * speed_mix_factor * scalingY / radius; // the latter is the scaling
        float cos_val=1.f,sin_val=0.f;
        amount += frameTimeSeconds*1.2f*angular_speed;  /* tune hard-coded coefficient to tune the skating-effect */
        cos_val = cosf(amount); sin_val = sinf(amount);
        m[0] = cos_val;m[1] = 0.f;m[2] = sin_val;
        chm_Vec3Cross(&m[8],&m[0],&m[4]);
        m[12]+=radius*cos_val;
        m[14]+=radius*sin_val;
    }
#   endif

    /* code used to perform animations on characters different from peter and mary */
#   if 1
    {
        const struct cha_armature* armature = peter_mi->armature;   /* all characters share the same armature */
        int anim_idx = 0;
        CHA_ASSERT(armature);
        if (anim_idx<armature->num_actions && armature->num_actions>4) {
            for (i=0;i<group->num_instances;i++)    {
                struct cha_character_instance* inst = &group->instances[i];
                if (inst!=peter && inst!=mary)  {
                    while (anim_idx==CHA_ARMATURE_ACTION_NAME_CYCLE_RUN || anim_idx==CHA_ARMATURE_ACTION_NAME_CYCLE_WALK    // we skip these (already shown by peter)
                           || anim_idx==CHA_ARMATURE_ACTION_NAME_POSE_HANDS_OPEN || anim_idx==CHA_ARMATURE_ACTION_NAME_POSE_HANDS_CLOSED  // we skip these (affect hands only, not very noticeable)
                           ) anim_idx=(anim_idx+1)%armature->num_actions;
                    if (inst->active && !inst->culled)  {
                        struct cha_mesh_instance* mi = &inst->mesh_instances[CHA_MESH_NAME_BODY];
                        const struct cha_armature_action* action = &armature->actions[anim_idx];
                        const float animation_speed = 1.f;
                        const float additional_time_to_get_to_first_frame = 0.5f*animation_speed;    // this value MUST be constant through all the animation time (i.e. if 'animation_speed' is not constant it MUST be removed from here).
                        const float repeat_time = additional_time_to_get_to_first_frame+action->duration+10.f;
                        float animation_time=totalTimeSeconds*animation_speed;      // good only for when 'animation_speed' is constant through all the animation time
                        //static float animation_time=0;animation_time+=frameTimeSeconds*animation_speed;   // good always
                        if (!action->looping && animation_time>repeat_time) animation_time=fmodf(animation_time,repeat_time);

                        cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,anim_idx,animation_time,additional_time_to_get_to_first_frame,0);
                    }
                    ++anim_idx;anim_idx%=armature->num_actions;
                }
            }
        }
    }
#   endif

    /* alternative code used to perform animations on characters different from peter and mary */
#   if 0
    {
        const struct cha_armature* armature = peter_mi->armature;   /* all characters share the same armature */
        CHA_ASSERT(armature);
        if (armature->num_actions>2) {
            const float animation_speed = 1.f;
            const float additional_time_to_get_to_first_frame = 2.5f*animation_speed;    // this value MUST be constant through all the animation time (i.e. if 'animation_speed' is not constant it MUST be removed from here).
            const float repeat_time = 5.f;
            float animation_time=totalTimeSeconds*animation_speed;      // good only for when 'animation_speed' is constant through all the animation time
            const int anim_idx_delta = (int)(animation_time/repeat_time);
            animation_time=fmodf(animation_time,repeat_time);
            for (i=0;i<group->num_instances;i++)    {
                struct cha_character_instance* inst = &group->instances[i];
                if (inst!=peter && inst!=mary)  {
                    int anim_idx = (i+anim_idx_delta)%armature->num_actions;
                    while (anim_idx==CHA_ARMATURE_ACTION_NAME_CYCLE_RUN || anim_idx==CHA_ARMATURE_ACTION_NAME_CYCLE_WALK) anim_idx=(anim_idx+1)%armature->num_actions;   // we skip these (already shown by peter)
                    if (inst->active && !inst->culled)  {
                        struct cha_mesh_instance* mi = &inst->mesh_instances[CHA_MESH_NAME_BODY];
                        cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,anim_idx,animation_time,additional_time_to_get_to_first_frame,0);
                    }
                }
            }
        }
    }
#   endif

}


void DrawGL(void)
{
    static unsigned begin = 0;
    static unsigned cur_time = 0;
    // All the things about time are just used to display FPS (F2)
    // or to move objects around (NOT for shadow)
    unsigned elapsed_time,delta_time;

    // These two instead are necessary for shadow mapping
    static choat vMatrixInverse[16];            // view Matrix inverse (it's the camera matrix).
    static choat vilvpMatrix[16];               // = light_pMatrix*light_vMatrix*camera_vMatrixInverse  [The latter was not used in test_shadows.c, but I now believe this is the correct way of doing it]

    static choat cameraFrustumCenter[3]={0,0,0};  // im world (=model) space

    // Just some time stuff here
    if (begin==0) begin = (unsigned) glutGet(GLUT_ELAPSED_TIME);
    elapsed_time = (unsigned) glutGet(GLUT_ELAPSED_TIME);
    if (elapsed_time<begin) begin=elapsed_time;
    elapsed_time-=begin;
    delta_time = elapsed_time;
    if (delta_time<cur_time) cur_time=delta_time;
    delta_time-=cur_time;
    instantFrameTime = (float)delta_time*0.001f;
    totalFrameTime = (float)elapsed_time*0.001f;
    cur_time = elapsed_time;


    // view Matrix
    chm_Mat4LookAt(vMatrix,cameraPos[0],cameraPos[1],cameraPos[2],targetPos[0],targetPos[1],targetPos[2],0,1,0);
    glLoadMatrix(vMatrix);
#   ifndef CHA_DOUBLE_PRECISION
    glLightfv(GL_LIGHT0,GL_POSITION,lightDirection);    // Important: the ffp must recalculate internally lightDirectionEyeSpace based on vMatrix [=> every frame]
#   else
    {
        const float lightDirectionf[3]={lightDirection[0],lightDirection[1],lightDirection[2]};
        glLightfv(GL_LIGHT0,GL_POSITION,lightDirectionf);    // Important: the ffp must recalculate internally lightDirectionEyeSpace based on vMatrix [=> every frame]
    }
#   endif
    glLoadIdentity();

    // view Matrix inverse (it's the camera matrix). Used twice below (and very important to keep in any case).
    chm_Mat4InvertTransformMatrixFast(vMatrixInverse,vMatrix);    // We can use Helper_InvertMatrixFast(...) instead of Helper_InvertMatrix(...) here [No scaling inside and no projection matrix]


    // The 2 scalars (cameraFrustumCenterDistance and cameraFrustumRadius) have been calculated in ResizeGL()
    chm_GetFrustumCenterFromCenterDistance(cameraFrustumCenter,vMatrixInverse,cameraFrustumCenterDistance);


    // MANDATORY CALLS
    CharacterGroupMoveAndAnimate(totalFrameTime,instantFrameTime);
    cha_character_group_updateMatrices(&group,1,vMatrix,pMatrixNormalizedFrustumPlanes);    /* if 'pMatrixNormalizedFrustumPlanes==NULL' there's no frustum culling */
    // -----------------------------------------------------------------


    // Draw to Shadow Map------------------------------------------------------------------------------------------
    {

#       ifndef USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE
        chm_GetLightViewProjectionMatrix(vilvpMatrix,
                                             vMatrixInverse,cameraFrustumCenter,cameraFrustumRadius,
                                             lightDirection,SHADOW_MAP_WIDTH,SHADOW_MAP_HEIGHT);
#       else //USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE
        {
            const choat *pMatInv;
#           ifdef CHA_DOUBLE_PRECISION
            choat pMatrixInvd[16];chm_Mat4Convertf2d(pMatrixInvd,pMatrixInverse);pMatInv=pMatrixInvd;
#           else
            pMatInv=pMatrixInverse;
#           endif
            chm_GetLightViewProjectionMatrixExtra(0,
                                             vMatrixInverse,cameraFrustumCenter,cameraFrustumRadius,
                                             lightDirection,SHADOW_MAP_WIDTH,SHADOW_MAP_HEIGHT,
                                             pMatInv,    // Mandatory when we need to retrieve arguments that follow it
                                             0,0,
                                             vilvpMatrix);        // Technically this was provided as an 'lvpMatrix for optimal frustum culling usage' in the 'Stable Shadow Mapping' case (but can be used to replace it too)
        }
#       endif //USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE

        /* 'chm_GetLightViewProjectionMatrix(...)' gives us 'lvpMatrix', so we miss 'vMatrixInverse' */
        chm_Mat4Mul(vilvpMatrix,vilvpMatrix,vMatrixInverse);   /* with this line we can use inst->mvMatrixOut (instead of inst->mMatrixOut, that is missing) */

        // Draw to shadow map texture
        glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();glMatrixMode(GL_MODELVIEW);        // We'll set the combined light view-projection matrix in GL_MODELVIEW (do you know that it's the same?)
        glBindFramebuffer(GL_FRAMEBUFFER, shadowPass.fbo);
        glViewport(0, 0, SHADOW_MAP_WIDTH,SHADOW_MAP_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_CLAMP);
        glUseProgram(shadowPass.program);            // we can just use glUseProgram(0) here
        glPushMatrix();
        glLoadMatrix(vilvpMatrix); // we load both (light) projection and view matrices here (it's the same after all)
        DrawGeometry(1);
        glPopMatrix();
        glUseProgram(0);
        glDisable(GL_DEPTH_CLAMP);
        glCullFace(GL_BACK);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);

    }


    // Draw world
    {
        // biasedShadowMvpMatrix is used only in the DefaultPass:
        static choat bias[16] = {0.5,0,0,0, 0,0.5,0,0,  0,0,0.5,0,    0.5,0.5,0.5,1}; // Moving from unit cube in NDC [-1,1][-1,1][-1,1] to [0,1][0,1][0,1] (x and y are texCoords; z is the depth range, [0,1] by default in window coordinates)
        static choat biasedShadowMvpMatrix[16];     // multiplied per vMatrixInverse
        chm_Mat4Mul(biasedShadowMvpMatrix,bias,vilvpMatrix); // test_shadows.c multiplies per vMatrixInverse only here (maybe because in the shadow pass it used mMatrix: no matter, since 'mlvpMatrix' can be both: 'lvpMatrix*mMatrix' and 'vilvpMatrix*mvMatrix'). We did it before, so we can use mvMatrix everywhere.

        // Draw to world
        glViewport(0, 0,(GLsizei) current_width,(GLsizei) current_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_LIGHTING);
        glBindTexture(GL_TEXTURE_2D,shadowPass.textureId);
        glUseProgram(defaultPass.program);
#       ifndef CHA_DOUBLE_PRECISION
        glUniformMatrix4fv(defaultPass.uniform_location_biasedShadowMvpMatrix, 1 /*only setting 1 matrix*/, GL_FALSE /*transpose?*/,biasedShadowMvpMatrix);
#       else
        {
            float tmp[16];chm_Mat4Convertd2f(tmp,biasedShadowMvpMatrix);
            glUniformMatrix4fv(defaultPass.uniform_location_biasedShadowMvpMatrix, 1 /*only setting 1 matrix*/, GL_FALSE /*transpose?*/,tmp);
        }
#       endif
        DrawGeometry(0);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D,0);
        glDisable(GL_LIGHTING);
    }

    // Draw armature
    if (g_character_instance_selected)  {
        const struct cha_mesh_instance* mi = &g_character_instance_selected->mesh_instances[CHA_MESH_NAME_BODY];
        cha_mesh_instance_draw_armature_opengl(mi);
    }

    // Draw 3d pivot
    glPushMatrix();
    glMultMatrix(vMatrix);
    GlutDraw3DPivot(targetPos,NULL);    /* works with mMatrix */
    glPopMatrix();


    if (config.show_fps && instantFrameTime>0) {
        if ((elapsed_time/1000)%2==0)   {
            printf("FPS=%1.0f\n",1./instantFrameTime);fflush(stdout);
            config.show_fps=0;
        }
    }

#   ifdef VISUALIZE_DEPTH_TEXTURE
#       ifndef __EMSCRIPTEN__
    {
        const float shadowmap_ratio = (float) SHADOW_MAP_WIDTH/(float)SHADOW_MAP_HEIGHT;
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDepthMask(GL_FALSE);

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glColor3f(1,1,1);
        //glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D,shadowPass.textureId);
        glEnable(GL_COLOR_MATERIAL);
        glColor4f(1,1,1,0.9f);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(-1,    -1);
        glTexCoord2f(0,1);glVertex2f(-0.25f*current_aspect_ratio/shadowmap_ratio, -1.f);
        glTexCoord2f(1,1);glVertex2f(-0.25f*current_aspect_ratio/shadowmap_ratio, -0.25f/current_aspect_ratio);
        glTexCoord2f(1,0);glVertex2f(-1.f,    -0.25f/current_aspect_ratio);
        glEnd();
        glBindTexture(GL_TEXTURE_2D,0);
        glColor4f(1,1,1,1);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_BLEND);
        //glEnable(GL_LIGHTING);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
    }
#       endif //__EMSCRIPTEN__
#   endif //VISUALIZE_DEPTH_TEXTURE

}

void DrawGeometry(int isShadowPass) {
/*
    This method is called twice, once with isShadowPass==1 and once with isShadowPass==0.
    However, when we call it with isShadowPass==1 in DrawGL(), we do:

    glPushMatrix();
        glLoadMatrix(vilvpMatrix); // we load both (light) projection and view matrices here (it's the same after all)
        DrawGeometry(1);
    glPopMatrix();

    This is bad, because if we use  glLoadMatrix(...) here, we erase glLoadMatrix(vilvpMatrix), and the
    shadow will be wrong.

    So for now we just take care of NOT using glLoadMatrix(...) here at all, we just do:
    glMultMatrix(mvMatrix); all the time

*/

    choat m[16];
    glPushMatrix();

    // characters
    glPushMatrix();
    {
        glDisable(GL_COLOR_MATERIAL);
        Character_DrawGroupOpengl(&group,1,isShadowPass?1:0);
        glEnable(GL_COLOR_MATERIAL);

    }
    glPopMatrix();

#   if 1
     // test hand-grabbing objects
    glPushMatrix();
    {
        const int bone_idx = CHA_BONE_NAME_HAND1_R;     // we can use any bone name here
        const int inst_idx = CHA_CHARACTER_INSTANCE_LADY_NAME_MARY;
        const struct cha_character_instance* inst = &group->ladies[inst_idx];
        const struct cha_mesh_instance* mi = &inst->mesh_instances[CHA_MESH_NAME_BODY];
        const float* mGrab = &mi->pose_matrices[CHA_BONE_SPACE_GRABBING][16*bone_idx];
        CHA_ASSERT(inst_idx>=0 && inst_idx<2);

        // Basically, if bone_idx!=CHA_BONE_NAME_ROOT (why should you need to place an object in the root bone?),
        // you simply have to multiply mi->mvMatrix and mGrab
        glMultMatrixf(mi->mvMatrix);
#       ifdef CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION        
        if (bone_idx!=CHA_BONE_NAME_ROOT) glMultMatrixf(mGrab);
        else glRotatef(90.f,1.f,0.f,0.f);
#       else
        glMultMatrixf(mGrab);
#       endif

        // draw an object (without any offset transform here)
        glScalef(0.75f,0.25f,0.75f);
        glColor3f(1.f,0.f,0.f);
        glutSolidCube(0.15f);
        glPushMatrix();
        glTranslatef(0,-0.075f-0.1f,0.f);
        glScalef(1.25f,2.f,1.25f);
        glColor3f(0.f,1.f,0.f);
        glutSolidSphere(0.05f,8,8);
        glPopMatrix();
        glTranslatef(0.f,0.f,0.075f);
        glRotatef(90.f,0.f,0.f,1.f);
        glScalef(0.75f,0.75f,0.025f);
        glColor3f(0.f,0.f,1.f);
        glutSolidCylinder(0.1,2.,8,8);
    }
    glPopMatrix();
#   endif

    // draw bounding box (for the mouse-selected character)
    if (!isShadowPass && g_character_instance_selected)
    {
        const struct cha_character_instance* inst = g_character_instance_selected;
        const struct cha_mesh_instance* mi = &inst->mesh_instances[CHA_MESH_NAME_BODY];
        const struct cha_mesh* mesh = mi->mesh;

        if (inst->active && !inst->culled)  {
            // Here we just draw the aabb around the character
            float mv[16];
            chm_Mat4Copyf(mv,inst->mvMatrixOut);
            // Now 'mv' is in the origin of 'mesh->aabb_min/mesh->aabb_max'

            glPushMatrix();
                // glutWireCube needs the matrix to be placed at 'mesh->aabb_center', so we must translate it
                // We use glutWireCube(1.f), so we must scale the matrix in the three dimensions with the extents of our aabb
                chm_Mat4Translatef(mv,mesh->aabb_center[0],mesh->aabb_center[1],mesh->aabb_center[2]);
                chm_Mat4Scalef(mv,mesh->aabb_half_extents[0]*2.f,mesh->aabb_half_extents[1]*2.f,mesh->aabb_half_extents[2]*2.f);
                glMultMatrixf(mv);

                glColor4f(0.5f,0.25f,0.f,1.f);
                glLineWidth(4.f);glutWireCube(1.f);
                glColor4f(0.8f,0.8f,0.8f,1.f);
            glPopMatrix();
        }

#       if 0
        /* test: draw the aabb of a single bone */
        {
            const int bone_idx = CHA_BONE_NAME_UPPERARM_LOWERARM_L;
            const struct cha_armature_bone* b = &mi->armature->bones[bone_idx];
            const float* bone_matrix = &mi->pose_matrices[CHA_BONE_SPACE_ARMATURE][bone_idx*16];
            const float aabbMin[3]={-0.1f*b->length,0.f,-0.1f*b->length};
            const float aabbMax[3]={0.1f*b->length,b->length,0.1f*b->length};
            float aabbCenter[3],aabbExtents[3];
            float mv[16];int i;
            for (i=0;i<3;i++) {
                aabbExtents[i] = aabbMax[i]-aabbMin[i];
                aabbCenter[i] = (aabbMax[i]+aabbMin[i])*0.5f;
            }
            chm_Mat4Mulf(mv,mi->mvMatrix,bone_matrix);
            glPushMatrix();
                // glutWireCube needs the matrix to be placed at 'mesh->aabb_center', so we must translate it
                // We use glutWireCube(1.f), so we must scale the matrix in the three dimensions with the extents of our aabb
                chm_Mat4Translatef(mv,aabbCenter[0],aabbCenter[1],aabbCenter[2]);
                chm_Mat4Scalef(mv,aabbExtents[0],aabbExtents[1],aabbExtents[2]);
                glMultMatrixf(mv);

                glColor4f(0.5f,0.5f,0.8f,1.f);
                glDisable(GL_DEPTH_TEST);
                glLineWidth(10.f);glutWireCube(1.f);
                glEnable(GL_DEPTH_TEST);
                glColor4f(0.8f,0.8f,0.8f,1.f);
            glPopMatrix();

        }
#       endif

    }

    glMultMatrix(vMatrix);  /* from now on we use mMatrix instead of mvMatrix */

    // ground
    glPushMatrix();
    {
        chm_Mat4Identity(m);
        m[13]=/*-4.4f*/-0.6f;
        chm_Mat4Scale(m,1.5f,.1f,1.5f);

        glMultMatrix(m);
        glColor4f(.3f,0.6f,.3f,1.f);
        glutSolidCube(12.f);
    }
    glPopMatrix();

    // house
    glPushMatrix();
    {
        // white cube
        chm_Mat4Identity(m);
        m[12]=-3.75f;
        m[13]=1.5f;
        m[14]=-1.f;
        chm_Mat4Scale(m,1.f,1.f,1.5f);
        glMultMatrix(m);
        glColor4f(1.f,1.f,1.f,1.f);
        glutSolidCube(3.f);

        // red roof
        chm_Mat4Identity(m);
        chm_Mat4Translate(m,0.f,2.275f,-1.75f);
        chm_Mat4Scale(m,0.8f,0.5f,1.0f);
        chm_Mat4Rotate(m,-30.f,0.0f,0.0f,1.0f);
        glMultMatrix(m);
        glColor4f(1.f,0.f,0.f,1.f);
        glutSolidCylinder(3.f,3.5f,3,1);

    }
    glPopMatrix();

    glPopMatrix();
}

static void GlutDestroyWindow(void);
static void GlutCreateWindow(void);

void GlutCloseWindow(void)  {Config_Save(&config,ConfigFileName);}

void GlutNormalKeys(unsigned char key, int x, int y) {
    const int mod = glutGetModifiers();
    (void)x;(void)y;
    switch (key) {
    case 27: 	// esc key
        Config_Save(&config,ConfigFileName);
        GlutDestroyWindow();
#       ifdef __FREEGLUT_EXT_H__
        glutLeaveMainLoop();
#	else
        exit(0);
#	endif
        break;
    case 13:	// return key
    {
#       ifndef __EMSCRIPTEN__
        if (mod&GLUT_ACTIVE_CTRL) {
            config.fullscreen_enabled = gameModeWindowId ? 0 : 1;
            isJustSwitchingFullscreen = 1;
            GlutDestroyWindow();
            GlutCreateWindow();
            isJustSwitchingFullscreen = 0;
        }
#       endif //__EMSCRIPTEN__
    }
        break;
    case 'c':
        /*isCullFaceDisabled=!isCullFaceDisabled;
        printf("Key 'c': cull face: %s\n",isCullFaceDisabled?"DISABLED":"ENABLED");fflush(stdout);*/
        break;
    case 's':
        /*isShadowingDisabled=!isShadowingDisabled;
        printf("Key 's': shadowing: %s\n",isShadowingDisabled?"DISABLED":"ENABLED");fflush(stdout);*/
        break;
    case 'l':
        /*if (rv && rv->num_lods>1) {
            if (++numLodDisplayed>=(int)rv->num_lods) numLodDisplayed-=(int)rv->num_lods;
            printf("Key 'l': LOD: %d\n",numLodDisplayed);fflush(stdout);
        }*/
        break;
    }




}

static void updateCameraPos() {
    const choat distanceY = chm_sin(cameraPitch)*cameraDistance;
    const choat distanceXZ = chm_cos(cameraPitch)*cameraDistance;
    cameraPos[0] = targetPos[0] + chm_sin(cameraYaw)*distanceXZ;
    cameraPos[1] = targetPos[1] + distanceY;
    cameraPos[2] = targetPos[2] + chm_cos(cameraYaw)*distanceXZ;
}

static void updateDirectionalLight() {
    const choat distanceY = chm_sin(lightPitch);
    const choat distanceXZ = chm_cos(lightPitch);
    lightDirection[0] = chm_sin(lightYaw)*distanceXZ;
    lightDirection[1] = distanceY;
    lightDirection[2] = chm_cos(lightYaw)*distanceXZ;
    chm_Vec3Normalize(lightDirection);
    lightDirection[3]=0.;
}

static void resetCamera() {
    // You can set the initial camera position here through:
    targetPos[0]=0; targetPos[1]=1.5; targetPos[2]=0; // The camera target point
    cameraYaw = (choat)(2*M_PI);                             // The camera rotation around the Y axis
    cameraPitch = (choat)(M_PI*0.125);                      // The camera rotation around the XZ plane
    cameraDistance = 15;                             // The distance between the camera position and the camera target point

    updateCameraPos();
    if (config.use_camera_ortho3d_projection_matrix)    ResizeGL((int)current_width,(int)current_height); // Needed because in Helper_Orho3D(...) cameraTargetDistance changes
}

static void resetLight() {
    lightYaw = (choat)(M_PI*0.15);
    lightPitch = (choat)(M_PI*0.25);
    updateDirectionalLight();
}

void GlutSpecialKeys(int key,int x,int y)
{
    const int mod = glutGetModifiers();
    (void)x;(void)y;
    if (!(mod&GLUT_ACTIVE_CTRL) && !(mod&GLUT_ACTIVE_SHIFT))	{
        switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            cameraYaw+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_LEFT ? (choat)-4.0 : (choat)4.0);
            if (cameraYaw>(choat)M_PI) cameraYaw-=2*M_PI;
            else if (cameraYaw<=-(choat)M_PI) cameraYaw+=2*M_PI;
            updateCameraPos();		break;
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            cameraPitch+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_UP ? (choat)2 : (choat)-2);
            if (cameraPitch>(choat)(M_PI-0.001)) cameraPitch=(choat)(M_PI-0.001);
            else if (cameraPitch<-(choat)(M_PI*0.1)) cameraPitch=-(choat)(M_PI*0.1);
            updateCameraPos();
            break;
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            cameraDistance+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_PAGE_DOWN ? (choat)25.0 : (choat)-25.0);
            if (cameraDistance<pMatrixNearPlane+(choat)0.55) cameraDistance=pMatrixNearPlane+(choat)0.55;
            updateCameraPos();
            if (config.use_camera_ortho3d_projection_matrix)    ResizeGL((int)current_width,(int)current_height); // Needed because in Helper_Orho3D(...) cameraTargetDistance changes
            break;
        case GLUT_KEY_F2:
            config.show_fps = !config.show_fps;
            //printf("showFPS: %s.\n",config.show_fps?"ON":"OFF");fflush(stdout);
            break;
        case GLUT_KEY_F1:
            config.use_camera_ortho3d_projection_matrix = !config.use_camera_ortho3d_projection_matrix;
            //printf("camera ortho mode: %s.\n",config.use_camera_ortho3d_projection_matrix?"ON":"OFF");fflush(stdout);
            ResizeGL((int)current_width,(int)current_height);
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
            choat forward[3] = {targetPos[0]-cameraPos[0],0,targetPos[2]-cameraPos[2]};
            choat up[3] = {0,1,0};
            choat left[3];

            chm_Vec3Normalize(forward);
            chm_Vec3Cross(left,up,forward);
            {
                choat delta[3] = {0,0,0};int i;
                if (key==GLUT_KEY_LEFT || key==GLUT_KEY_RIGHT) {
                    choat amount = instantFrameTime*cameraSpeed*(key==GLUT_KEY_RIGHT ? (choat)-25.0 : (choat)25.0);
                    for (i=0;i<3;i++) delta[i]+=amount*left[i];
                }
                else {
                    choat amount = instantFrameTime*cameraSpeed*(key==GLUT_KEY_DOWN ? (choat)-25.0 : (choat)25.0);
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
            targetPos[1]+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_PAGE_DOWN ? (choat)-25.0f : (choat)25.0f);
            if (targetPos[1]<(choat)-50.) targetPos[1]=(choat)-50.;
            else if (targetPos[1]>(choat)500.) targetPos[1]=(choat)500.;
            updateCameraPos();
            if (config.use_camera_ortho3d_projection_matrix)    ResizeGL((int)current_width,(int)current_height); // Needed because in Helper_Orho3D(...) cameraTargetDistance changes
        break;
        }
    }
    else if (mod&GLUT_ACTIVE_SHIFT)	{
        switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            lightYaw+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_LEFT ? (choat)-4.0 : (choat)4.0);
            if (lightYaw>(choat)M_PI) lightYaw-=2*M_PI;
            else if (lightYaw<=-(choat)M_PI) lightYaw+=2*M_PI;
            updateDirectionalLight();
            break;
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            lightPitch+= instantFrameTime*cameraSpeed*( (key==GLUT_KEY_UP || key==GLUT_KEY_PAGE_UP) ? (choat)2. : (choat)-2.);
            if (lightPitch>(choat)(M_PI-0.001)) lightPitch=(choat)(M_PI-0.001);
            else if (lightPitch<-(choat)(M_PI*0.05)) lightPitch=-(choat)(M_PI*0.05);
            updateDirectionalLight();
            break;
        case GLUT_KEY_HOME:
            // Reset the light
            resetLight();
            break;
        }
    }
}

void GlutMouse(int button,int state,int x,int y) {
    const int mods =  glutGetModifiers();
    if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)   {
        const int viewport[4] = {0,0,(int)current_width,(int)current_height};
        static struct cha_character_instance* selected_inst = NULL;
        selected_inst = cha_character_group_GetInstanceUnderMousef(&group,1,x,y,viewport,pMatrixInverse,NULL);
        if (!selected_inst) {g_character_instance_selected=selected_inst;}
        else    {
            if (g_character_instance_selected!=selected_inst)  {
#               ifdef CHA_CHARACTER_GROUP_INIT_NUM_COLORS   /* dbg only stuff (to see if default colors need to be changed) */
                const int idx = selected_inst->group_idx;
                const int is_man = idx<group->num_men?1:0;
                const int mtl_idx = (is_man ? (idx+(CHA_CHARACTER_GROUP_INIT_NUM_COLORS/2+1)) : (idx-group->num_men))%CHA_CHARACTER_GROUP_INIT_NUM_COLORS;
                printf("mtl_idx=%d\n",mtl_idx);fflush(stdout);
#               endif
                g_character_instance_selected = selected_inst;
            }
            else if (g_character_instance_selected) {
                /* (optional) find bone under mouse */
                struct cha_mesh_instance* mi = &g_character_instance_selected->mesh_instances[CHA_MESH_NAME_BODY];
                const int bone_index_under_mouse = cha_mesh_instance_GetBoneUnderMousef(mi,x,y,viewport,pMatrixInverse,NULL);
                if (bone_index_under_mouse!=CHA_BONE_NAME_COUNT)    {
                    unsigned bone_mask_under_mouse = 1U<<bone_index_under_mouse;
                    if (mods&GLUT_ACTIVE_SHIFT) bone_mask_under_mouse = cha_armature_getBoneSubTreeMask(mi->armature,bone_mask_under_mouse);
                    if (mods&GLUT_ACTIVE_CTRL) mi->selected_bone_mask^=bone_mask_under_mouse;
                    else mi->selected_bone_mask=bone_mask_under_mouse;
                    //printf("bone_index_under_mouse = %d\n",bone_index_under_mouse);fflush(stdout);
                }
            }
        }
    }
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
            glutLeaveGameMode();
            gameModeWindowId = 0;
        }
        if (windowId) {
            glutDestroyWindow(windowId);
            windowId=0;
        }
    }
}

#ifdef CREATE_DEBUG_CONTEXT
#ifdef __cplusplus
extern "C"
#endif
void GLDebugMessageCallback(GLenum source, GLenum type,
    GLuint id, GLenum severity,GLsizei length, const GLchar *msg,const void *userParam)
{
    char sourceStr[32],typeStr[32],severityStr[32];
    const char *sourceFmt = "UNDEFINED(0x%04X)",*typeFmt = "UNDEFINED(0x%04X)",*severityFmt = "UNDEFINED";

    switch(source)

    {
    case GL_DEBUG_SOURCE_API_ARB:             sourceFmt = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:   sourceFmt = "WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceFmt = "SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:     sourceFmt = "THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:     sourceFmt = "APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER_ARB:           sourceFmt = "OTHER"; break;
    }

    snprintf(sourceStr, 32, sourceFmt, source);

    switch(type)
    {

    case GL_DEBUG_TYPE_ERROR_ARB:               typeFmt = "ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: typeFmt = "DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  typeFmt = "UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY_ARB:         typeFmt = "PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:         typeFmt = "PERFORMANCE"; break;
    case GL_DEBUG_TYPE_OTHER_ARB:               typeFmt = "OTHER"; break;
    }
    snprintf(typeStr, 32, typeFmt, type);


    switch(severity)
    {
    case GL_DEBUG_SEVERITY_HIGH_ARB:   severityFmt = "HIGH";   break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB: severityFmt = "MEDIUM"; break;
    case GL_DEBUG_SEVERITY_LOW_ARB:    severityFmt = "LOW"; break;
    }

    snprintf(severityStr, 32, severityFmt, severity);

    fprintf(stderr,"OpenGL: %s [source=%s type=%s severity=%s(%d) id=%d]\n",msg, sourceStr, typeStr, severityStr,severity, id);

    if (strcmp(severityFmt,"UNDEFINED")!=0) {
        fprintf(stderr,"BREAKPOINT TRIGGERED\n");   /* set breakpoint here */
    }
}
#endif //CREATE_DEBUG_CONTEXT

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
        char windowTitle[1024] = PROGRAM_NAME".c";
        strcat(windowTitle," - shadowMap");
#       ifdef USE_UNSTABLE_SHADOW_MAPPING_TECHNIQUE
        strcat(windowTitle," [unstable]");
#       endif
#       ifndef __cplusplus
        strcat(windowTitle,": "XSTR_MACRO(SHADOW_MAP_WIDTH)"x"XSTR_MACRO(SHADOW_MAP_HEIGHT)"");
#       endif
        config.fullscreen_enabled = 0;

#       ifdef CREATE_DEBUG_CONTEXT
        glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);
#       endif

        glutInitWindowPosition(100,100);
        glutInitWindowSize(config.windowed_width,config.windowed_height);
        windowId = glutCreateWindow(windowTitle);

#       ifdef CREATE_DEBUG_CONTEXT
        glDebugMessageCallback(GLDebugMessageCallback,NULL);    // last is userParam
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glEnable(GL_DEBUG_OUTPUT);   // use glDisable(GL_DEBUG_OUTPUT); at runtime to disable it

        // Test:
        //glEnable(GL_DEPTH); // instead of glEnable(GL_DEPTH_TEST); => Produces: GL_INVALID_ENUM error generated. <cap> enum is invalid; expected GL_ALPHA_TEST, GL_BLEND, GL_COLOR_MATERIAL, GL_CULL_FACE, GL_DEPTH_TEST, GL_DITHER, GL_FOG, etc. (136 others).
#       endif
    }

    glutKeyboardFunc(GlutNormalKeys);
    glutSpecialFunc(GlutSpecialKeys);
    glutMouseFunc(GlutMouse);
    glutIdleFunc(GlutIdle);
    glutReshapeFunc(ResizeGL);
    glutDisplayFunc(GlutFakeDrawGL);
#   ifdef __FREEGLUT_EXT_H__
    glutWMCloseFunc(GlutCloseWindow);
#   endif //__FREEGLUT_EXT_H__

#ifdef USE_GLEW
    {
        GLenum err = glewInit();
        if( GLEW_OK != err ) {
            fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err) );
            return;
        }
    }
#endif //USE_GLEW

    if (!isJustSwitchingFullscreen) {
        //OpenGL info
        printf("\nGL Vendor: %s\n", glGetString( GL_VENDOR ));
        printf("GL Renderer : %s\n", glGetString( GL_RENDERER ));
        printf("GL Version (string) : %s\n",  glGetString( GL_VERSION ));
        printf("GLSL Version : %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ));
        //printf("GL Extensions:\n%s\n",(char *) glGetString(GL_EXTENSIONS));

        printf("\nKEYS:\n");
        printf("AROW KEYS + PAGE_UP/PAGE_DOWN:\tmove camera (optionally with CTRL down)\n");
        printf("HOME KEY:\t\t\treset camera\n");
        printf("ARROW KEYS + SHIFT:\tmove directional light\n");
        printf("F1:\t\t\ttoggle camera ortho mode on and off\n");
        printf("F2:\t\t\tdisplay FPS to console\n");
        printf("LMB:\t\t\tselect a character (displaying its aabb and its armature)\n");
        printf("LMB(+-CTRL,SHIFT):\t(de)select bones in a selected character (displaying them in a different color)\n");
        printf("CTRL+RETURN:\t\ttoggle fullscreen on/off (if supported)\n");

        printf("\n");fflush(stdout);

        resetCamera();  // Mandatory
        resetLight();   // Mandatory
    }

    InitGL();

}


int main(int argc, char** argv)
{   
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
#ifdef __FREEGLUT_EXT_H__
    glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;
#endif //__FREEGLUT_EXT_H__

    Config_Init(&config);
    Config_Load(&config,ConfigFileName);


    GlutCreateWindow();


    glutMainLoop();


    return 0;
}

/** optional: draws 3d pivot at camera target position.
 * can be sped up using display lists if used with pOptionalDisplayListBase!=NULL && *pOptionalDisplayListBase=0, but
 * then the lists must be freed using: if (*pOptionalDisplayListBase) {glDeleteLists(*pOptionalDisplayListBase,8);*pOptionalDisplayListBase=0;}
 * [The point of having 8 lists instead of a single one is just to ease code to allow its rendering without using any display list at all]
*/
#ifndef __EMSCRIPTEN__
void GlutDraw3DPivot(const choat* cameraTargetPosition3,GLuint* pOptionalDisplayListBase) {
    unsigned int curDisplayListIndex=0;
    const int mustGenerateLists = (pOptionalDisplayListBase && *pOptionalDisplayListBase==0) ? 1 : 0;
    const int mustDrawGeometry = (!pOptionalDisplayListBase || *pOptionalDisplayListBase==0) ? 1 : 0;
    const int mustCallLists = (pOptionalDisplayListBase && *pOptionalDisplayListBase!=0) ? 1 : 0;

    if (mustGenerateLists) *pOptionalDisplayListBase = glGenLists(8);

    if (cameraTargetPosition3)  {
         // camera target pos:
         // Center
         glColor3f(0,0,0);
         glPushMatrix();
         glTranslatef(cameraTargetPosition3[0],cameraTargetPosition3[1],cameraTargetPosition3[2]);
         glPushMatrix();
         if (pOptionalDisplayListBase)   {
             if (mustCallLists) glCallList(*pOptionalDisplayListBase+curDisplayListIndex);
             else if (mustGenerateLists) glNewList(*pOptionalDisplayListBase+curDisplayListIndex,GL_COMPILE_AND_EXECUTE);
             ++curDisplayListIndex;
         }
         if (mustDrawGeometry) glutSolidSphere(0.04,8,8);
         if (mustGenerateLists) glEndList();

         // X Axis
         glPushMatrix();
         glColor3f(1,0,0);
         glRotatef(90,0,1,0);
         if (pOptionalDisplayListBase)   {
             if (mustCallLists) glCallList(*pOptionalDisplayListBase+curDisplayListIndex);
             else if (mustGenerateLists) glNewList(*pOptionalDisplayListBase+curDisplayListIndex,GL_COMPILE_AND_EXECUTE);
             ++curDisplayListIndex;
         }
         if (mustDrawGeometry) glutSolidCylinder(0.04,0.25,8,8);
         if (mustGenerateLists) glEndList();
         glTranslatef(0,0,0.25);
         if (pOptionalDisplayListBase)   {
             if (mustCallLists) glCallList(*pOptionalDisplayListBase+curDisplayListIndex);
             else if (mustGenerateLists) glNewList(*pOptionalDisplayListBase+curDisplayListIndex,GL_COMPILE_AND_EXECUTE);
             ++curDisplayListIndex;
         }
         if (mustDrawGeometry) glutSolidCone(0.06,0.1,8,8);
         if (mustGenerateLists) glEndList();
         glPopMatrix();

         // Y Axis
         glPushMatrix();
         glColor3f(0,1,0);
         glRotatef(-90,1,0,0);
         if (pOptionalDisplayListBase)   {
             if (mustCallLists) glCallList(*pOptionalDisplayListBase+curDisplayListIndex);
             else if (mustGenerateLists) glNewList(*pOptionalDisplayListBase+curDisplayListIndex,GL_COMPILE_AND_EXECUTE);
             ++curDisplayListIndex;
         }
         if (mustDrawGeometry) glutSolidCylinder(0.04,0.25,8,8);
         if (mustGenerateLists) glEndList();
         glTranslatef(0,0,0.25);
         if (pOptionalDisplayListBase)   {
             if (mustCallLists) glCallList(*pOptionalDisplayListBase+curDisplayListIndex);
             else if (mustGenerateLists) glNewList(*pOptionalDisplayListBase+curDisplayListIndex,GL_COMPILE_AND_EXECUTE);
             ++curDisplayListIndex;
         }
         if (mustDrawGeometry) glutSolidCone(0.06,0.1,8,8);
         if (mustGenerateLists) glEndList();
         glPopMatrix();

         // Z Axis
         glPushMatrix();
         glColor3f(0,0,1);
         if (pOptionalDisplayListBase)   {
             if (mustCallLists) glCallList(*pOptionalDisplayListBase+curDisplayListIndex);
             else if (mustGenerateLists) glNewList(*pOptionalDisplayListBase+curDisplayListIndex,GL_COMPILE_AND_EXECUTE);
             ++curDisplayListIndex;
         }
         if (mustDrawGeometry) glutSolidCylinder(0.04,0.25,8,8);
         if (mustGenerateLists) glEndList();
         glTranslatef(0,0,0.25);
         if (pOptionalDisplayListBase)   {
             if (mustCallLists) glCallList(*pOptionalDisplayListBase+curDisplayListIndex);
             else if (mustGenerateLists) glNewList(*pOptionalDisplayListBase+curDisplayListIndex,GL_COMPILE_AND_EXECUTE);
             ++curDisplayListIndex;
         }
         if (mustDrawGeometry) glutSolidCone(0.06,0.1,8,8);
         if (mustGenerateLists) glEndList();
         glPopMatrix();

         glPopMatrix();
         glPopMatrix();
         // End camera target position
     }
}
#endif //__EMSCRIPTEN__

