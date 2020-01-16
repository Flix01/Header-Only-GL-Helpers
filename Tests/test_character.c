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
gcc -O3 -std=gnu89 -DNDEBUG -no-pie test_character.c -o test_character -I"../" -lglut -lGL -lX11 -lm
// WINDOWS (here we use the static version of glew, and glut32.lib, that can be replaced by freeglut.lib):
cl /O2 /MT /Tc test_character.c /D"TEAPOT_NO_RESTRICT" /D"DYNAMIC_RESOLUTION_NO_RESTRICT" /D"GLEW_STATIC" /I"../" /link /out:test_character.exe glut32.lib glew32s.lib opengl32.lib gdi32.lib Shell32.lib comdlg32.lib user32.lib kernel32.lib
// EMSCRIPTEN:
emcc -O2 -std=gnu89 -fno-rtti -fno-exceptions -o test_character.html test_character.c -I"./" -I"../" -s LEGACY_GL_EMULATION=0 --closure 1
(for PCF shadows 2x2 add: -D"DYNAMIC_RESOLUTION_SHADOW_USE_PCF=2" -s USE_WEBGL2=1)
(for web assembly add:    -s WASM=1)

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


#include <stdio.h>  // printf
#include <math.h>
#include <string.h>


//#define DEMO_USE_DOUBLE_PRECISION       /* optional */

#ifdef DEMO_USE_DOUBLE_PRECISION
#   define DYNAMIC_RESOLUTION_USE_DOUBLE_PRECISION
#   define TEAPOT_USE_DOUBLE_PRECISION
#   define CHA_USE_DOUBLE_PRECISION
#endif


// "dynamic_resolution.h" implements the first shadow mapping step and optionally dynamic resolution (that by default should keep frame rate > config.dynamic_resolution_target_fps)
//#define DYNAMIC_RESOLUTION_USE_GLSL_VERSION_330       // (Optional) Not sure it's faster...
//#define DYNAMIC_RESOLUTION_SHADOW_MAP_SIZE_FORCE_POT    // There are other definitions that affect the shadow map resolution. Please see dynamic_resolution.h.
//#define DYNAMIC_RESOLUTION_SHADOW_MAP_RECTANGULAR       // more flickering but slightly better resolution
//#define DYNAMIC_RESOLUTION_SHADOW_MAP_SIZE_MULTIPLIER   2.0 // 1.5 default value [better resolution, but more expensive]
//#define DYNAMIC_RESOLUTION_SHADOW_MAP_DISABLED        // test-only
#if (!defined(__EMSCRIPTEN__) && !defined(DYNAMIC_RESOLUTION_SHADOW_USE_PCF))
//#   define DYNAMIC_RESOLUTION_SHADOW_USE_PCF 4    // Optional [but expensive] Percentage Closing Filter Shadows (for this to work emscripten needs in the command-line: -s USE_WEBGL2=1 )
#endif //__EMSCRIPTEN__
#define DYNAMIC_RESOLUTION_IMPLEMENTATION           // Mandatory in 1 source file (.c or .cpp)
#include "dynamic_resolution.h"

#define TEAPOT_CENTER_MESHES_ON_FLOOR           // (Optional) Otherwise meshes are centered in their local aabb center
//#define TEAPOT_INVERT_MESHES_Z_AXIS           // (Optional) Otherwise meshes look in the opposite Z direction
//#define TEAPOT_SHADER_SPECULAR                  // (Optional) specular hilights
#define TEAPOT_SHADER_FOG                       // (Optional) fog to remove bad clipping
#define TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER	// (Optional) better fog quality
#define TEAPOT_SHADER_USE_SHADOW_MAP            // Mandatory for implementing the second shadow mapping step, but can be disabled in this demo
#define TEAPOT_ENABLE_FRUSTUM_CULLING           // (Optional) a bit expensive, and does not cull 100% hidden objects. You'd better test if it works and if it's faster...
//#define TEAPOT_SHADER_NORMALIZE_NORMALS      // (Optional)
//#define TEAPOT_SHADER_USE_ACCURATE_NORMALS      // (Optional) a bit slower [it replaces TEAPOT_SHADER_USE_NORMAL_MATRIX from version 1.2]. Only relevant when using non-uniform scaling and/or scaling embedded in matrices.
//#define TEAPOT_SHADER_HINT_ACCURATE_NORMALS_GPU // (Optional) used only if TEAPOT_SHADER_USE_ACCURATE_NORMALS is defined. Don't know if it's faster or not (1 less glUniform3f(...) call on the CPU, but more calculations in the vertex shader)
//#define TEAPOT_USE_LEGACY_CODE
#define TEAPOT_IMPLEMENTATION                   // Mandatory in 1 source file (.c or .cpp)
#include "teapot.h"


#define CHA_HAS_OPENGL_SUPPORT  /* for vbo support */
#define CHA_USE_VBO
#define CHA_USE_MATH_MACROS
//#define CHA_HINT_USE_VAO  /* on emscripten it needs WebGL2 */
//#define CHA_ALLOW_ROOT_ONLY_POSE_OPTIMIZATION
#define CHA_LIGHTNESS 0.55f      /* [must be <=1.f] Well, this depends on the shader we use (in teapot.h). You can try to tweak this value. [And for some odd reasons, lightness depends on the scaling specified in Character_CreateGroup(...)] */
#define CHARACTER_IMPLEMENTATION
#include "character.h"

static struct cha_character_group* group = NULL;

// Optional local definitions
//#define VISUALIZE_DEPTH_TEXTURE // requires the ffp (does not work in WebGL)



// Config file handling: basically there's an .ini file next to the
// exe that you can tweak. (it's just an extra)
const char* ConfigFileName = "test_character.ini";
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
choat targetPos[3];         // please set it in resetCamera()
choat cameraYaw;            // please set it in resetCamera()
choat cameraPitch;          // please set it in resetCamera()
choat cameraDistance;       // please set it in resetCamera()
choat cameraPos[3];         // Derived value (do not edit)
choat vMatrix[16];          // view matrix
choat cameraSpeed = 0.5;   // When moving it

// light data
choat lightYaw = M_PI*0.225f,lightPitch = M_PI*0.25f;   // Please edit them in resetLight() too
choat lightDirection[4] = {0,1,0,0};                    // Derived value (do not edit) [lightDirection[3]==0]

// pMatrix data:
float pMatrix[16];                  // projection matrix
const float pMatrixFovyDeg = 45.;
const float pMatrixNearPlane = 0.5;
const float pMatrixFarPlane = 35.;
float pMatrixInv[16];               // inverse projection matrix (test)
float pMatrixNormalizedFrustumPlanes[6][4]; // necessary for character.h frustum culling (and must be float)

float instantFrameTime = 16.2f; // in seconds
float totalFrameTime = 0.f;     // in seconds

float current_width=0,current_height=0,current_aspect_ratio=1;  // Not sure when I've used these...
void ResizeGL(int w,int h) {
    current_width = (float) w;
    current_height = (float) h;
    if (current_height!=0) current_aspect_ratio = current_width/current_height;
    if (h>0)	{
        // We set our pMatrix here in ResizeGL(), and we must notify teapot.h about it too.
        chm_Mat4Perspectivef(pMatrix,pMatrixFovyDeg,(float)w/(float)h,pMatrixNearPlane,pMatrixFarPlane);
        Teapot_SetProjectionMatrixf(pMatrix);
        chm_Mat4Invertf(pMatrixInv,pMatrix);

        chm_GetFrustumPlaneEquationsf(pMatrixNormalizedFrustumPlanes,pMatrix,1);
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
static Teapot_MeshData* pMeshData[12];              // This just points to parts of allocated_memory (to avoid multiple allocations)
static const int maxNumMeshData = sizeof(pMeshData)/sizeof(pMeshData[0]);
static int numMeshData = 0; // initial value (see InitGL())

static Teapot_MeshData* pMouseSelectedMeshData = NULL;
static struct cha_character_instance* mouseSelectedCharacterInstance = NULL;



void InitGL(void) {

    // IMPORTANT CALL--------------------------------------------------------
    Dynamic_Resolution_Init(config.dynamic_resolution_target_fps,config.dynamic_resolution_enabled);
    //-----------------------------------------------------------------------
    // IMPORTANT CALL--------------------------------------------------------
    Teapot_Init();
    //-----------------------------------------------------------------------
    // IMPORTANT_CALL--------------------------------------------------------
    Character_Init();
    group = Character_CreateGroup(3,3,1.85f,1.75f);
    //-----------------------------------------------------------------------
    // (don't copy this)
    CHA_ASSERT(group && group->num_men>0 && group->num_ladies>0);   // they are used in the demo
    CHA_ASSERT(CHA_CHARACTER_INSTANCE_LADY_NAME_MARY==0);
    group->ladies[CHA_CHARACTER_INSTANCE_LADY_NAME_MARY].mesh_instances[CHA_MESH_NAME_BODY].selected_bone_mask = CHA_BONE_MASK_SPINE;   // we'll use it in the manual animation later


    // These are important, but often overlooked OpenGL calls
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Otherwise transparent objects are not displayed correctly
    glClearColor(0.5f,0.7f,1.0f,1.0f);
    glEnable(GL_TEXTURE_2D);    // Only needed for ffp, when VISUALIZE_DEPTH_TEXTURE is defined

    glEnable(GL_LINE_SMOOTH);

#   ifdef TEAPOT_SHADER_FOG
    // We use fog to prevent ad clipping artifacts, so it just needs the near and far plane
    Teapot_SetFogDistances((pMatrixFarPlane-pMatrixNearPlane)*0.75f,pMatrixFarPlane); // We start the fog at 3/4
    Teapot_SetFogColor(0.5f,0.7f,1.0f); // it should be the same as glClearColor()
#   endif

#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    Teapot_SetShadowDarkening(40.f,0.75f);  // First number makes the shadow darker in an uniform way, the second clamps the lower intensity: (40.f,0.75f) default, (...,1.0f) -> no shadows
                                            // [However if TEAPOT_SHADER_SHADOW_MAP_PCF>0, then (0.1f/first_number) is the shadow bias (or 0.0f if zero)].
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP

    // This line can change the look of the demo considerably. Try commenting it out!
    Teapot_Enable_ColorMaterial();    // Optional (Teapot_SetColor(...) internally calls Teapot_SetColorAmbient(...); and Teapot_SetColorSpecular(...); when enabled)
    // The truth is that for every object it's tedious to setup 3 colors (even if we'd get better visuals): so this definition is useful.

    // Warning: in this demo we know that the calling order of the callbacks is: InitGL(),ResizeGL(...),DrawGL().
    // That's why we can avoid calling ResizeGL(...) here
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
        choat mMatrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        int i;Teapot_MeshData* md;
        CHA_ASSERT(sizeof(choat)==sizeof(tpoat) && sizeof(choat)==sizeof(droat));
        //-----------------------------------------------------
        i=0;
        // Ground mesh (box)
        md = pMeshData[i++];
        mMatrix[12]=0.0;    mMatrix[13]=-0.6;    mMatrix[14]=0.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,1.5f*12.f,0.6f,1.5f*12.f);
        Teapot_MeshData_SetColor(md,0.3f,0.6f,0.3f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CUBIC_GROUND);

        // (house)
        md = pMeshData[i++];
        mMatrix[12]=-3.75;    mMatrix[13]=0.0;    mMatrix[14]=-1.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,3.f,3.f,4.5f);
        Teapot_MeshData_SetColor(md,1.f,1.f,1.f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CUBE);

        // (roof)
        md = pMeshData[i++];
        mMatrix[12]=-3.75;    mMatrix[13]=3.0;    mMatrix[14]=-1.0;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,4.f,1.5f,5.5f);
        Teapot_MeshData_SetColor(md,1.f,0.f,0.f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_ROOF);

        // (cylinder)
        md = pMeshData[i++];
        mMatrix[12]=-1.65;    mMatrix[13]=0;    mMatrix[14]=-1.5;
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,0.4f,2.5f,0.4f);
        Teapot_MeshData_SetColor(md,0.3f,0.5f,1.f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CYLINDER);

        /*// back wall (box)
        md = pMeshData[i++];
        mMatrix[12]=0.0;    mMatrix[13]=1.5;    mMatrix[14]=-9.0;
        Teapot_Helper_RotateMatrix(mMatrix,90,1,0,0);
        Teapot_MeshData_SetMMatrix(md,mMatrix);
        Teapot_MeshData_SetScaling(md,1.5f*12.f,0.4f,3.f);
        Teapot_MeshData_SetColor(md,1.f,1.f,1.f,1.0f);
        Teapot_MeshData_SetMeshId(md,TEAPOT_MESH_CUBIC_GROUND);
        Teapot_Helper_IdentityMatrix(mMatrix);*/

        numMeshData = i;
        CHA_ASSERT(numMeshData<maxNumMeshData);  // Well, we should check that numMeshData<maxNumMeshData
    }

}

void DestroyGL() {
    // Deallocate memory
    int i;
    if (allocated_memory) {free(allocated_memory);allocated_memory=NULL;}
    for (i=0;i<maxNumMeshData;i++) pMeshData[i]=NULL;

    // IMPORTANT CALLS-------------------------------------------------------
    if (group) {Character_DestroyGroup(group);group=NULL;}
    Character_Destroy();
    Teapot_Destroy();
    Dynamic_Resolution_Destroy();
    //-----------------------------------------------------------------------

    // don't forget to reset pointers (mandatory if we switch from/to fullscreen)
    pMouseSelectedMeshData = NULL;
    mouseSelectedCharacterInstance = NULL;
}


//#define TEST_DYN_RES_FOR_SHADOW_MAP   // This just works on my Ubuntu system by adding some wait time every frame to force dynamic resolution to trigger in
#ifdef TEST_DYN_RES_FOR_SHADOW_MAP
#include <unistd.h> // usleep
#endif //TEST_DYN_RES_FOR_SHADOW_MAP


/* The following 4 functions are necessary to 'bind' character.h to teapot.h (that draws geometry) and to 'dynamic_resolution.h' (that draws the shadow map) */
void my_mesh_instance_draw_callback_opengl(const struct cha_mesh_instance* mi,const float* mvMatrix16,int no_materials_and_normals/*=0*/,void* userData) {
    const struct cha_mesh* mesh = mi->mesh;
    const GLuint vbo = mi->vbo ? mi->vbo : (mi->static_shk_index>=0 ? mesh->shape_keys[mi->static_shk_index].vbo : mesh->vbo);
    const GLuint ibo = mesh->ibo;
    const GLuint vao = mi->vao ? mi->vao : (mi->static_shk_index>=0 ? mesh->shape_keys[mi->static_shk_index].vao : mesh->vao);
    int i;
    (void)userData;

    CHA_ASSERT(vbo && ibo);

#   ifdef CHA_HINT_USE_VAO
    glBindVertexArray(vao);
#   else
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glVertexAttribPointer(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
#   endif

    if (!no_materials_and_normals)  {
#       ifdef TEAPOT_SHADER_USE_SHADOW_MAP
        Teapot_LowLevel_SetMvMatrixUniformWithShadowSupportFloat(mvMatrix16);
#       else
        Teapot_LowLevel_SetMvMatrixUniformFloat(mvMatrix16);
#       endif

#       ifndef CHA_HINT_USE_VAO
        glVertexAttribPointer(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*3*mesh->num_verts));
#       endif

        for (i=0;i<mesh->num_parts;i++) {
            const struct cha_mesh_part* part = &mesh->parts[i];
            const int indsStart = part->inds_start;
            const int indsCount = part->inds_count;
            const struct cha_material* pMat = &mi->parent_character_instance->materials[part->material_idx];
            CHA_ASSERT(indsCount%3==0);
#           ifdef TEAPOT_SHADER_SPECULAR
            Teapot_SetColorAmbientDiffuseAndSpecular(pMat->amb,pMat->dif,pMat->spe);    /* shininess is in pMat->spe[3] */
#           else
            Teapot_SetColorAmbientAndDiffuse(pMat->amb,pMat->dif);
#           endif
            glDrawElements(GL_TRIANGLES, indsCount, GL_UNSIGNED_SHORT,(const void *) (indsStart*sizeof(unsigned short)));
        }
    }
    else    {
        Dynamic_Resolution_Shadow_Set_MMatrixFloat(mvMatrix16); // mMatrix here (or mvMatrix if we called:  Dynamic_Resolution_Shadow_Set_VpMatrix(lightPvMatrix * cameraVMatrixInv); and we DID it)

        /* 'no_materials_and_normals': faster code for depth-only rendering (shadow map here) */
        glDrawElements(GL_TRIANGLES, mesh->num_inds, GL_UNSIGNED_SHORT,(const void *) (0));
    }
}
void additionalObjectsShadowPassDraw(void* userData)  {
    const choat* lpvMatrix = (choat*) userData;
    choat tmp[16];
    CHA_ASSERT(userData);
    //chm_Mat4InvertTransformMatrixFast(tmp,vMatrix);    /* tmp = vMatrixInverse */
    Teapot_GetViewMatrixInverse(tmp);                   /* teapot_h has vMatrixInverse already stored... */
    chm_Mat4Mul(tmp,lpvMatrix,tmp);
    Dynamic_Resolution_Shadow_Set_VpMatrix(tmp); /* all this so that we can just use 'mvMatrix' instead of 'mMatrix' for shadows in 'cha_mesh_instance_draw_callback_opengl(...)' */
    Dynamic_Resolution_Shadow_Set_Scaling(1,1,1);   /* we reset it */

#   ifndef CHA_HINT_USE_VAO
    glEnableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
    //glEnableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);    // normals are not necessary for drawing shadows    (well, 'vao' uses them...)
#   endif
    cha_character_group_draw(&group,1,1,0,&my_mesh_instance_draw_callback_opengl,NULL);
#   ifndef CHA_HINT_USE_VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
    //glDisableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);   // normals are not necessary for drawing shadows    (well, 'vao' uses them...)
#   else
    glBindVertexArray(0);
#   endif
    Dynamic_Resolution_Shadow_Set_VpMatrix(lpvMatrix);  /* clean-up */
}
/* Of course drawing the character armature could be entirely skipped... */
void glDrawBoneCallback(const struct cha_mesh_instance* mi,int bone_idx,const float* mvMatrix16,float length,void* userData) {
    (void)mi;(void)bone_idx;(void)userData;
#   ifndef TEAPOT_USE_DOUBLE_PRECISION
    Teapot_Helper_DrawArmatureBone_Mv(mvMatrix16,length);
#   else
    {
    tpoat tmp[16];chm_Mat4Convertf2d(tmp,mvMatrix16);
    Teapot_Helper_DrawArmatureBone_Mv(tmp,length);
    }
#   endif
}
void cha_mesh_instance_draw_armature_opengl(const struct cha_mesh_instance* mi)  {
    CHA_ASSERT(mi->armature);
    glLineWidth(1.f);
    {
        const unsigned deselected_mask = CHA_BONE_MASK_INVERSE(mi->selected_bone_mask);
        const unsigned   selected_mask = mi->selected_bone_mask;
        Teapot_LowLevel_StartDisablingLighting();
        glDepthMask(GL_FALSE);glDisable(GL_DEPTH_TEST);
        if (deselected_mask) {
            Teapot_SetColorAmbient(0.8f,0.8f,0.f);//0.75f);
            cha_mesh_instance_draw_armature(mi,deselected_mask,&glDrawBoneCallback,NULL);
        }
        if (  selected_mask) {
            Teapot_SetColorAmbient(0.4f,0.4f,0.8f);//0.75f);
            cha_mesh_instance_draw_armature(mi,  selected_mask,&glDrawBoneCallback,NULL);
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_POLYGON_OFFSET_LINE);
        if (deselected_mask) {
            Teapot_SetColorAmbient(0.6f,0.4f,0.f);
            cha_mesh_instance_draw_armature(mi,deselected_mask,&glDrawBoneCallback,NULL);
        }
        if (  selected_mask) {
            Teapot_SetColorAmbient(0.2f,0.2f,0.4f);
            cha_mesh_instance_draw_armature(mi,  selected_mask,&glDrawBoneCallback,NULL);
        }
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);glDepthMask(GL_TRUE);
        Teapot_LowLevel_StopDisablingLighting();
    }
}


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
        const float amount = 20.f*sinf(totalTimeSeconds*2.f);int j;
        for (j=0;j<CHA_BONE_NAME_COUNT;j++)   {
            // in InitGL() we set the initial value of 'mi->selected_bone_mask',
            // so that, when user (de)selects bones, the manual animation changes
            if (mi->selected_bone_mask&(1U<<j))    {
            float* m = &mi->pose_matrices[CHA_BONE_SPACE_BONE][j*16];
            struct cha_mesh_instance_pose_data* pose_data = &mi->pose_data[j];
            chm_Mat4Identityf(m);
            chm_Mat4Rotatef(m,amount,0.f,0.f,1.f);
            pose_data->rot_dirty = 2;   // 2 means that we have updated 'pose_matrices[CHA_BONE_SPACE_BONE]'; 1 that we have updated 'pose_data' directly        
            }
        }
        /*if (CHA_BONE_NAME_LOWERARM_WRIST_L>0)    {
            float* m = &mi->pose_matrices[CHA_BONE_SPACE_BONE][CHA_BONE_NAME_LOWERARM_WRIST_L*16];
            struct cha_mesh_instance_pose_data* pose_data = &mi->pose_data[CHA_BONE_NAME_LOWERARM_WRIST_L];
            m[13]=0.75f;
            pose_data->tra_dirty = 2;
        }*/
        // test CHA_ROOT__SOMETHING (to remove)
        /*if (CHA_BONE_NAME_ROOT>-1)    {
            float* m = &mi->pose_matrices[CHA_BONE_SPACE_BONE][CHA_BONE_NAME_ROOT*16];
            struct cha_mesh_instance_pose_data* pose_data = &mi->pose_data[CHA_BONE_NAME_ROOT];
            const float amount = sinf(totalTimeSeconds*0.5f);
            //chm_Mat4Rotate(m,5.f*amount,0.f,1.f,0.f);pose_data->rot_dirty = 2;
            m[14]=5.f*amount;  // y vertical
            //m[13]=2.f*amount;  // y vertical
            pose_data->tra_dirty = 2;
            //chm_Mat4Rotate(mary->mMatrix,360.f*amount,0.f,1.f,0.f);
        }*/
        //mi->pose_bone_mask = 0;
        //cha_mesh_instance_update_bone_matrices(mi);
        //cha_mesh_instance_update_vertices(mi);
        /*if (last_bone_mask!=mi->pose_bone_mask) {
            last_bone_mask=mi->pose_bone_mask;
            fprintf(stderr,"mi->pose_bone_mask=%u",mi->pose_bone_mask);
            if (mi->pose_bone_mask==CHA_BONE_MASK_ALL) fprintf(stderr," (=CHA_BONE_MASK_ALL)");
            fprintf(stderr,";\n");
        }*/
    }
#   elif 1
    {   // TEST: skeletal animation
        struct cha_mesh_instance* mi = mary_mi;
        const float animation_speed = 1.f;
        const float additional_time_to_get_to_first_frame = 0.5f*animation_speed;    // this value MUST be constant through all the animation time (i.e. if 'animation_speed' is not constant it MUST be removed from here).
        const float animation_time=totalTimeSeconds*animation_speed;      // good only for when 'animation_speed' is constant through all the animation time
        //static float animation_time=0;animation_time+=frameTimeSeconds*animation_speed;   // good always

        cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,CHA_ARMATURE_ACTION_NAME_FALL_DOWN,animation_time,additional_time_to_get_to_first_frame);
        //cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,CHA_ARMATURE_ACTION_NAME_RUN_CYCLE,animation_time,additional_time_to_get_to_first_frame);

    }
#   endif
#   if 1
    {   // TEST: skeletal animation
        struct cha_mesh_instance* mi = peter_mi;
        const float additional_time_to_get_to_first_frame = 1.0f;    // seconds
        static float animation_time = 0.f;
        animation_time+= (frameTimeSeconds)*character_animation_speed;
        cha_mesh_instance_calculate_bone_space_pose_matrices_from_action_ex(mi,CHA_ARMATURE_ACTION_NAME_CYCLE_RUN,animation_time,additional_time_to_get_to_first_frame,character_animation_walk_run_mix_in_0_1,CHA_ARMATURE_ACTION_NAME_CYCLE_WALK,0,-1,0);//CHA_BONE_MASK_ARMS_AND_HANDS);
        //cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,CHA_ARMATURE_ACTION_NAME_CYCLE_WALK,animation_time,additional_time_to_get_to_first_frame);
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
        if (anim_idx<armature->num_actions && armature->num_actions>2) {
            for (i=0;i<group->num_instances;i++)    {
                struct cha_character_instance* inst = &group->instances[i];
                if (inst!=peter && inst!=mary)  {
                    while (anim_idx==CHA_ARMATURE_ACTION_NAME_CYCLE_RUN || anim_idx==CHA_ARMATURE_ACTION_NAME_CYCLE_WALK) anim_idx=(anim_idx+1)%armature->num_actions;   // we skip these (already shown by peter)
                    if (inst->active && !inst->culled)  {
                        struct cha_mesh_instance* mi = &inst->mesh_instances[CHA_MESH_NAME_BODY];
                        const struct cha_armature_action* action = &armature->actions[anim_idx];
                        const float animation_speed = 1.f;
                        const float additional_time_to_get_to_first_frame = 0.5f*animation_speed;    // this value MUST be constant through all the animation time (i.e. if 'animation_speed' is not constant it MUST be removed from here).
                        const float repeat_time = additional_time_to_get_to_first_frame+action->duration+10.f;
                        float animation_time=totalTimeSeconds*animation_speed;      // good only for when 'animation_speed' is constant through all the animation time
                        //static float animation_time=0;animation_time+=frameTimeSeconds*animation_speed;   // good always
                        if (!action->looping && animation_time>repeat_time) animation_time=fmodf(animation_time,repeat_time);

                        cha_mesh_instance_calculate_bone_space_pose_matrices_from_action(mi,anim_idx,animation_time,additional_time_to_get_to_first_frame);
                    }
                    ++anim_idx;anim_idx%=armature->num_actions;
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
    unsigned elapsed_time,delta_time;


    // time calculations
    if (begin==0) begin = glutGet(GLUT_ELAPSED_TIME);
    elapsed_time = glutGet(GLUT_ELAPSED_TIME);
    if (elapsed_time<begin) begin=elapsed_time;
    elapsed_time-=begin;
    delta_time = elapsed_time;
    if (delta_time<cur_time) cur_time=delta_time;
    delta_time-=cur_time;
    instantFrameTime = (float)delta_time*0.001f;
    totalFrameTime = (float)elapsed_time*0.001f;
    cur_time = elapsed_time;



    // view Matrix
    Teapot_Helper_LookAt(vMatrix,cameraPos[0],cameraPos[1],cameraPos[2],targetPos[0],targetPos[1],targetPos[2],0,1,0);
    Teapot_SetViewMatrixAndLightDirection(vMatrix,lightDirection);  // we must notify teapot.h, and we also pass the lightDirection here


    Teapot_MeshData_CalculateMvMatrixFromArray(pMeshData,numMeshData);  // This sets every Teapot_MeshData::mvMatrix

    // IMPORTANT: well, see their code to understand why these are necessary
    CharacterGroupMoveAndAnimate(totalFrameTime,instantFrameTime);
    cha_character_group_updateMatrices(&group,1,vMatrix,pMatrixNormalizedFrustumPlanes);
    // -----------------------------------------------------------------


#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    // Draw to Shadow Map------------------------------------------------------------------------------------------
    {
    static choat lvpMatrix[16]; // = light_pMatrix*light_vMatrix

//#   define TEST_AUTOMATIC_LVPMATRIX_CALCULATION
#   ifndef TEST_AUTOMATIC_LVPMATRIX_CALCULATION
    // We're currently calculating all these matrices every frame. This is obviously wrong.
    // Also: there's no fixed rule I know to calculate these matrices. Feel free to change them!
    static choat lpMatrix[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    static choat lvMatrix[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    const choat texelIncrement = Dynamic_Resolution_GetShadowMapTexelIncrement();   // intended to alleviate shadow swimming (but it does not seem to work...)

    //if (lpMatrix[0]==0)
    {
        // This changes with pMatrixFarPlane and pMatrixFovDeg
        choat y = pMatrixFarPlane*tan(pMatrixFovyDeg*3.1415/180.0)*0.3f;  // last coefficient is ad-hoc for this demo (in our case it should be 1.0, or maybe 0.5 for free roaming; something like 0.2 for fixed environment and MUCH better shadow quality!)
        choat x = y;
        y = ceil(y/texelIncrement)*texelIncrement;x=y;  // intended to alleviate shadow swimming... (?)
        Teapot_Helper_Ortho(lpMatrix,-x,x,-y,y,pMatrixFarPlane*0.5f,-pMatrixFarPlane*0.5f);
    }
    //if (lmvMatrix[15]==0)
    {
        // This changes with lightDirection, pMatrixFarPlane, targetPos (= camera target)
        const choat distance =  pMatrixFarPlane*0.1f;
        const choat shadowTargetPos[3] = {targetPos[0],0.f,targetPos[2]};   // We keep it at y=0
        const choat lpos[3] = {shadowTargetPos[0]-lightDirection[0]*distance,
                               shadowTargetPos[1]-lightDirection[1]*distance,
                               shadowTargetPos[2]-lightDirection[2]*distance};
        Teapot_Helper_LookAt(lvMatrix,lpos[0],lpos[1],lpos[2],shadowTargetPos[0],shadowTargetPos[1],shadowTargetPos[2],0,1,0);

        Teapot_Helper_MultMatrix(lvpMatrix,lpMatrix,lvMatrix);
    }
#   else //TEST_AUTOMATIC_LVPMATRIX_CALCULATION
    {
        // This eats too much shadow map resolution... see also: https://github.com/Flix01/Tiny-OpenGL-Shadow-Mapping-Examples
        float frustumRadius,frustumCenterDistance;choat frustumCenter3[3];
        const float cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero = 0.f;
        frustumRadius = chm_GetFrustumRadiusAndCenterDistancef(&frustumCenterDistance,pMatrixNearPlane,pMatrixFarPlane,pMatrixFovyDeg,current_aspect_ratio,cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero);
        chm_GetFrustumCenterFromCenterDistance(frustumCenter3,Teapot_GetViewMatrixInverseConstReference(),frustumCenterDistance);
        chm_GetLightViewProjectionMatrix(lvpMatrix,Teapot_GetViewMatrixInverseConstReference(),frustumCenter3,frustumRadius,Teapot_GetNormalizedLightDirectionConstReference(),Dynamic_Resolution_GetShadowMapTextureWidth(),Dynamic_Resolution_GetShadowMapTextureHeight());
    }
#   endif //TEST_AUTOMATIC_LVPMATRIX_CALCULATION

    // There is also a version that takes lpMatrix and lvMatrix and multiplies them
    // and another version that takes lvpMatrix and its frustum planes and performs frustum culling
    // (try it, but it's untested and probably slower in many cases)
    Teapot_HiLevel_DrawMulti_ShadowMap_Vp(pMeshData,numMeshData,lvpMatrix,0.5f,&additionalObjectsShadowPassDraw,(void*)lvpMatrix);
    //Teapot_HiLevel_DrawMulti_ShadowMap_Vp(pMeshData,numMeshData,lvpMatrix,0.5f,NULL,(void*)NULL);
    // The HiLevel function above uses parts of dynamic_resolution_h too, but you can unwrap it and use low-level functions as well (see its code).

    // Most noticeably, it wraps functions like:

    // Dynamic_Resolution_Bind_Shadow();   // Binds the shadow map FBO and its shader program
    // Teapot_LowLevel_BindVertexBufferObject();
    // [...] Teapot_LowLevel_DrawElements(meshId);
    // Teapot_LowLevel_UnbindVertexBufferObject();
    // Dynamic_Resolution_Unbind_Shadow();

    // And also it sets some uniforms to shaders in both dynamic_resolution_h and teapot_h
    // And finally it also calls:
    // glBindTexture(GL_TEXTURE_2D,Dynamic_Resolution_Get_Shadow_Texture_ID());    // For the 2nd part of the shadow map algorithm
    }
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP



    // Render to framebuffer---------------------------------------------------------------------------------------
    Dynamic_Resolution_Bind();  // This defaults to nothing if we don't use dynamic resolution (-> it's for free: we can draw inside it as usual)

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Teapot_PreDraw();

    // We can add an aabb frame around any object (here we use mouse selection)
    if (pMouseSelectedMeshData) {
        const Teapot_MeshData* md = pMouseSelectedMeshData; //pAnimatedMeshData0;
        float scaling[3],aabb[3];

        // Fill 'scaling':
        Teapot_GetMeshAabbExtents(md->meshId,aabb);
        if (md->meshId==TEAPOT_MESH_CAPSULE)    {
            // Sorry, but capsules are special (Teapot_SetScaling(...) does not scale them, because we want the two half-spheres to be always regular)
            const float sphereScaling = (md->scaling[0]+md->scaling[2])*0.5;
            scaling[0]=aabb[0]*sphereScaling;scaling[2]=aabb[2]*sphereScaling;
            aabb[1]*=md->scaling[1];aabb[1]-=1.0*(md->scaling[1]-sphereScaling);
            scaling[1] = aabb[1];
        }
        else {int i;for (i=0;i<3;i++) scaling[i]=aabb[i]*md->scaling[i];}

        // Apply scaling, set color and line width, and draw an AABB around the shape:
        Teapot_SetScaling(scaling[0],scaling[1],scaling[2]);
        Teapot_SetColor(0.4,0.4,0,1);
        glLineWidth(4.f);
        // Hp) We have already md->mvMatrix. We do have it, because we're calling Teapot_DrawMulti_Mv(...) below.
        // Otherwise, we can just call Teapot_Draw(md->mMatrix,TEAPOT_MESHLINES_CUBE_EDGES),
        // Or better we can move this snippet below Teapot_DrawMulti(pMeshData,numMeshData,1),
        // because Teapot_DrawMulti(...) internally sets all the mvMatrices and then calls Teapot_DrawMulti_Mv(pMeshData,numMeshData,1).
        Teapot_Draw_Mv(md->mvMatrix,TEAPOT_MESHLINES_CUBE_EDGES);
    }
    if (mouseSelectedCharacterInstance)   {
        const struct cha_mesh_instance* mi = &mouseSelectedCharacterInstance->mesh_instances[CHA_MESH_NAME_BODY];
        const struct cha_mesh* mesh = mi->mesh;
        float mv[16];
        chm_Mat4Copyf(mv,mouseSelectedCharacterInstance->mvMatrixOut);
#       ifdef TEAPOT_CENTER_MESHES_ON_FLOOR
        chm_Mat4Translatef(mv,0.f,-mesh->aabb_half_extents[1],0.f);
#       endif

        // Here we are in the origin of mesh->aabbMin/aabbMax (but not in mesh->aabb_center)
        chm_Mat4Translatef(mv,mesh->aabb_center[0],mesh->aabb_center[1],mesh->aabb_center[2]);
        Teapot_SetScaling(mesh->aabb_half_extents[0]*2.f,mesh->aabb_half_extents[1]*2.f,mesh->aabb_half_extents[2]*2.f);
        Teapot_SetColor(0.4,0.4,0,1);
        glLineWidth(4.f);
        // Hp) We have already mvMatrix.
        // Otherwise, we can just call Teapot_Draw(mMatrix,TEAPOT_MESHLINES_CUBE_EDGES),
        // Or better we can move this snippet below Teapot_DrawMulti(pMeshData,numMeshData,1),
        // because Teapot_DrawMulti(...) internally sets all the mvMatrices and then calls Teapot_DrawMulti_Mv(pMeshData,numMeshData,1).
        Teapot_Draw_MvFloat(mv,TEAPOT_MESHLINES_CUBE_EDGES);
    }

    // We can add a pivot at the camera target point
    {
        static choat mMatrix[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        mMatrix[12]=targetPos[0];mMatrix[13]=targetPos[1];mMatrix[14]=targetPos[2];
        Teapot_SetColor(0.5,0.5,0,1); // the color of the center of the pivot (and for the pivot mesh only there's a hack that transfers the brightness of this color to the whole mesh)
        Teapot_SetScaling(0.4,0.4,0.4);
        // Tip: We can change the outline color/params with Teapot_Set_MeshOutline_XXX(...) methods
        Teapot_Enable_MeshOutline();  // This does not work together with glDisable(GL_DEPTH_TEST);
        //glDisable(GL_DEPTH_TEST);
        Teapot_Draw(mMatrix,TEAPOT_MESH_PIVOT3D);
        //glEnable(GL_DEPTH_TEST);
        Teapot_Disable_MeshOutline();
    }

    // Here we draw the characters:
    Teapot_LowLevel_UnbindVertexBufferObjectAndDisableVertexAttributes(1,1); // we use the 'teapot' shader, but we must 'unbind' the teapot_h 'vbo'
    Teapot_Disable_ColorMaterial();
    Teapot_SetScaling(1,1,1);   // reset Teapot scaling
#   ifndef CHA_HINT_USE_VAO
    glEnableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
    glEnableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
#   endif
    cha_character_group_draw(&group,1,0,0,&my_mesh_instance_draw_callback_opengl,NULL);
#   ifndef CHA_HINT_USE_VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(CHA_HINT_VERTEX_ATTRIBUTE_LOCATION);
    glDisableVertexAttribArray(CHA_HINT_NORMAL_ATTRIBUTE_LOCATION);
#   else
    glBindVertexArray(0);
#   endif
    Teapot_Enable_ColorMaterial();
    Teapot_LowLevel_BindVertexBufferObjectAndEnableVertexAttributes(1,1);



    // Here we draw all our pMeshData
    Teapot_DrawMulti_Mv(pMeshData,numMeshData,1);    // Here we don't use Teapot_DrawMulti(...) because we got the mvMatrices already (see Teapot_MeshData_CalculateMvMatrixFromArray(pMeshData,numMeshData); above)
                                                     // This way we could have used mvMatrices in the shadow map creation (we haven't done it AFAIR)
                                                     // Please note that to handle transparent objects correctly, it can change the object order (see last argument). So to detect an object, just store pointers and don't realloc the initial buffer (see that we have maxNumMeshData>=numMeshData)
    if (mouseSelectedCharacterInstance) {
        cha_mesh_instance_draw_armature_opengl(&mouseSelectedCharacterInstance->mesh_instances[CHA_MESH_NAME_BODY]);
    }


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
#       if TEAPOT_SHADER_SHADOW_MAP_PCF>1
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        //glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
#       endif
        glColor4f(1,1,1,0.9f);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex2f(-1,    -1);
        glTexCoord2f(1,0);glVertex2f(-0.25*current_aspect_ratio, -1);
        glTexCoord2f(1,1);glVertex2f(-0.25*current_aspect_ratio, -0.25/current_aspect_ratio);
        glTexCoord2f(0,1);glVertex2f(-1,    -0.25/current_aspect_ratio);
        glEnd();
#       if TEAPOT_SHADER_SHADOW_MAP_PCF>1
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        //glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY );
#       endif
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
    const float distanceY = sin(cameraPitch)*cameraDistance;
    const float distanceXZ = cos(cameraPitch)*cameraDistance;
    cameraPos[0] = targetPos[0] + sin(cameraYaw)*distanceXZ;
    cameraPos[1] = targetPos[1] + distanceY;
    cameraPos[2] = targetPos[2] + cos(cameraYaw)*distanceXZ;
}

static void updateDirectionalLight() {
    const float distanceY = sin(lightPitch);
    const float distanceXZ = cos(lightPitch);
    lightDirection[0] = sin(lightYaw)*distanceXZ;
    lightDirection[1] = distanceY;
    lightDirection[2] = cos(lightYaw)*distanceXZ;
    Teapot_Helper_Vector3Normalize(lightDirection);
    lightDirection[3]=0.f;
}

static void resetCamera() {
    // You can set the initial camera position here through:
    targetPos[0]=0; targetPos[1]=1.5; targetPos[2]=0; // The camera target point
    cameraYaw = 2*M_PI;                             // The camera rotation around the Y axis
    cameraPitch = M_PI*0.125f;                      // The camera rotation around the XZ plane
    cameraDistance = 15;                             // The distance between the camera position and the camera target point

    updateCameraPos();
}

static void resetLight() {
    lightYaw = M_PI*0.15f;
    lightPitch = M_PI*0.25f;
    updateDirectionalLight();
}

void GlutSpecialKeys(int key,int x,int y)
{
    const int mod = glutGetModifiers();
    (void)(x);(void)(y);    // -Wunused-parameter
    if (!(mod&GLUT_ACTIVE_CTRL) && !(mod&GLUT_ACTIVE_SHIFT))	{
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
            choat forward[3] = {targetPos[0]-cameraPos[0],0,targetPos[2]-cameraPos[2]};
            choat up[3] = {0,1,0};
            choat left[3];

            Teapot_Helper_Vector3Normalize(forward);
            Teapot_Helper_Vector3Cross(left,up,forward);
            {
                choat delta[3] = {0,0,0};int i;
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
    else if (mod&GLUT_ACTIVE_SHIFT)	{
        switch (key) {
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            lightYaw+= instantFrameTime*cameraSpeed*(key==GLUT_KEY_LEFT ? -4.0f : 4.0f);
            if (lightYaw>M_PI) lightYaw-=2*M_PI;
            else if (lightYaw<=-M_PI) lightYaw+=2*M_PI;
            updateDirectionalLight();
            break;
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            lightPitch+= instantFrameTime*cameraSpeed*( (key==GLUT_KEY_UP || key==GLUT_KEY_PAGE_UP) ? 2.f : -2.f);
            if (lightPitch>M_PI-0.001f) lightPitch=M_PI-0.001f;
            else if (lightPitch<-M_PI*0.05f) lightPitch=-M_PI*0.05f;
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
        const int viewport[4] = {0,0,current_width,current_height};
        choat distanceMesh = 0.0;float distanceChar = 0.f;
        struct cha_character_instance* selected_inst = NULL;
        pMouseSelectedMeshData = Teapot_MeshData_GetMeshUnderMouse(pMeshData,numMeshData,x,y,viewport,&distanceMesh);
        selected_inst = cha_character_group_GetInstanceUnderMousef(&group,1,x,y,viewport,pMatrixInv,&distanceChar);
        if (selected_inst && pMouseSelectedMeshData) {
            if (distanceChar<=distanceMesh) pMouseSelectedMeshData=NULL;
            else selected_inst=NULL;
        }
        if (!selected_inst) {mouseSelectedCharacterInstance=selected_inst;}
        else    {
            if (mouseSelectedCharacterInstance!=selected_inst)  {
#               ifdef CHA_CHARACTER_GROUP_INIT_NUM_COLORS   /* dbg only stuff (to see if default colors need to be changed) */
                const int idx = selected_inst->group_idx;
                const int is_man = idx<group->num_men?1:0;
                const int mtl_idx = (is_man ? (idx+(CHA_CHARACTER_GROUP_INIT_NUM_COLORS/2+1)) : (idx-group->num_men))%CHA_CHARACTER_GROUP_INIT_NUM_COLORS;
                printf("mtl_idx=%d\n",mtl_idx);fflush(stdout);
#               endif
                mouseSelectedCharacterInstance=selected_inst;
            }
            else if (mouseSelectedCharacterInstance) {
                /* (optional) find bone under mouse */
                struct cha_mesh_instance* mi = &mouseSelectedCharacterInstance->mesh_instances[CHA_MESH_NAME_BODY];
                const int bone_index_under_mouse = cha_mesh_instance_GetBoneUnderMousef(mi,x,y,viewport,pMatrixInv,NULL);
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
        windowId = glutCreateWindow("test_character.c");
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
    printf("ARROW KEYS + SHIFT:\tmove directional light\n");
    printf("F1:\t\t\ttoggle dynamic resolution on/off\n");
    printf("F2:\t\t\tdisplay FPS\n");
    printf("LMB:\t\t\tselect a character (displaying its aabb and its armature)\n");
    printf("LMB(+-CTRL,SHIFT):\t(de)select bones in a selected character (displaying them in a different color)\n");
#	ifndef __EMSCRIPTEN__
    printf("CTRL+RETURN:\t\ttoggle fullscreen on/off (if supported)\n");
#	endif //__EMSCRIPTEN__
    printf("\n");fflush(stdout);

    resetCamera();  // Mandatory
    resetLight();   // Mandatory

    glutMainLoop();


    return 0;
}

