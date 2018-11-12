// https://github.com/Flix01/Header-Only-GL-Helpers
//
/* LICENSE: Made by @Flix01. MIT license on my part.
 * But I don't know the license of the original Teapot mesh data,
 * and of the Bunny and Torus meshes (these two from the Bullet Physics Engine's GImpact Old Demo)
 * I've made the other meshes myself.
 * You can disable some meshes with something like TEAPOT_NO_MESH_BUNNY (never tested).
 *
 * However:
 * ->   The teapot mesh data is based on the data contained in the freeglut library (see glutSolidTeapot(...)),
 *      with some modifications to display it better when GL_CULL_FACE is enabled. AFAIK, the freeglut library has a liberal
 *      license that is compatible with MIT.
 * ->   The bunny and torus meshes are based on the BunnyMesh.h and TorusMesh.h contained in the GImpactTestDemo that was first
 *      available from here: http://gimpact.sourceforge.net. GIMPACT itself is licensed under either LGPL, BSD or ZLib-style license.
 *      However there is no note about the license of the two mesh data...
*/

/* WHAT'S THIS?
 * A plain C (--std=gnu89) header-only file that can be used to display the teapot mesh (and other meshes),
 * in a (modern) openGL program, with a very simple API and no other dependencies (everything is in the teapot.h file).
*/

/* USAGE:
 * Please see the comments of the functions below and try to follow their order when you call them.
 * Define TEAPOT_IMPLEMENTATION in one of your .c (or .cpp) files before the inclusion of this file.
*/
// OPTIONAL DEFINITIONS:
//
//#define TEAPOT_CENTER_MESHES_ON_FLOOR   // By default meshes are centered in their model location (usually their aabb center)
//#define TEAPOT_INVERT_MESHES_Z_AXIS     // Makes meshes look in the opposite Z direction
//
//#define TEAPOT_SHADER_SPECULAR // Adds specular light component in the shader and Teapot_SetColorSpecular(...) function
//#define TEAPOT_SHADER_FOG // Adds linear fog in the shader and Teapot_SetFogColor(...) Teapot_SetFogDistances(...) functions
//#define TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER // Changes the fog quality a bit (used only when TEAPOT_SHADER_FOG is defined)
//#define TEAPOT_SHADER_USE_NORMAL_MATRIX  // (slow) accurate normal calculations when the model(view) matrix has some scaling applied (instead of using Teapot_SetScaling(...)).
//                                            But, overall, I think that leaving this definition out and just using Teapot_SetScaling(...) and orthonormal mvMatrices (= no glScalef(...)),
//                                            is accurate enough, faster, and easier to use too. NOTE that you probably need to CHANGE something in your code if you switch this definition on/off.
//#define TEAPOT_SHADER_USE_SHADOW_MAP      // Eases shadow mapping by supporting the second pass of the algorithm.
//#define TEAPOT_SHADER_SHADOW_MAP_PCF 4    // (optional, but needs a value>0, otherwise will be set to zero). Basically when TEAPOT_SHADER_USE_SHADOW_MAP is defined, PCF filter used (dynamic_resolution.h can automatically set this value when DYNAMIC_RESOLUTION_SHADOW_USE_PCF is used).
//                                          // Warning: when TEAPOT_SHADER_SHADOW_MAP_PCF is used with emscripten, it needs: -s USE_WEBGL2=1
//
//#define TEAPOT_MAX_NUM_USER_MESH_VERTICES (3000)  // (experimental) default is 1000. Needed when, using Teapot_Set_Init_UserMeshCallback(...), you overflow the number of vertices. Warning: the max number of vertices for ALL meshes (= embedded + user) can't be bigger than 65535, because the indices are unsigned short.
//#define TEAPOT_MAX_NUM_USER_MESH_INDICES (15000)  // (experimental) default is 5000. Needed when, using Teapot_Set_Init_UserMeshCallback(...), you overflow the number of indices.
//
//#define TEAPOT_ENABLE_FRUSTUM_CULLING     // (experimental) it does not cull 100% objects, and performance might be slower rather than faster...
//
//#define TEAPOT_USE_OPENMP                 // (experimental) ATM is only used in Teapot_MeshData_CalculateMvMatrixFromArray(...) and never tested => one more dependency and no gain: DO NOT USE!
//
//#define TEAPOT_USE_SIMD					// (experimental) speeds up Teapot_Helper_MultMatrix(...) using SIMD (about 1.5x-2x when compiled with -O3 -DNDEBUG -march=native), Requires -msse (OR -mavx when using double precision).
//
//#define TEAPOT_MESHDATA_HAS_MMATRIX_PTR   // (untested) handy when using Teapot_MeshData + some kind of physic engine that already stores a mMatrix16 somewhere.

#ifndef TEAPOT_H_
#define TEAPOT_H_


#ifdef __cplusplus
extern "C" {
#endif

#ifndef TEAPOT_VERSION
#   define TEAPOT_VERSION 1.1
#endif //TEAPOT_VERSION


/* The __restrict and __restrict__ keywords are recognized in both C, at all language levels, and C++, at LANGLVL(EXTENDED).*/
#ifdef TEAPOT_NO_RESTRICT  // please define it globally if the keyword __restrict is not present
#   ifndef __restrict   
#       define __restrict /*no-op*/
#   endif
#endif //TEAPOT_NO_RESTRICT

#ifdef TEAPOT_USE_DOUBLE_PRECISION	// from version TEAPOT_VERSION 1.1: TEAPOT_MATRIX_USE_DOUBLE_PRECISION and  TEAPOT_USE_DOUBLE_PRECISION are the same
#undef TEAPOT_MATRIX_USE_DOUBLE_PRECISION
#define TEAPOT_MATRIX_USE_DOUBLE_PRECISION
#endif //TEAPOT_USE_DOUBLE_PRECISION

#ifndef TEAPOT_MATRIX_USE_DOUBLE_PRECISION
typedef float  tpoat;       /* short form of tEApOT_FLoat */
#else
typedef double tpoat;
#undef TEAPOT_USE_DOUBLE_PRECISION
#define TEAPOT_USE_DOUBLE_PRECISION
#endif

#ifdef TEAPOT_USE_SIMD
#if (!defined(__SSE__) && (defined(_MSC_VER) && defined(_M_IX86_FP) && _M_IX86_FP>0))
#define __SSE__		// _MSC_VER does not always define it... (but it always defines __AVX__)
#endif // __SSE__
#ifndef TEAPOT_MATRIX_USE_DOUBLE_PRECISION
#ifdef __SSE__
#include <xmmintrin.h>	// SSE
#endif //__SSE__
#else // TEAPOT_MATRIX_USE_DOUBLE_PRECISION
#ifdef __AVX__
#include <immintrin.h>	// AVX (and everything)
#endif //__AVX__
#endif // TEAPOT_MATRIX_USE_DOUBLE_PRECISION
#endif //TEAPOT_USE_SIMD

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

#ifndef TEAPOT_SHADER_SHADOW_MAP_PCF
#   ifdef DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#       define TEAPOT_SHADER_SHADOW_MAP_PCF DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#   else //DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#       define TEAPOT_SHADER_SHADOW_MAP_PCF 0
#   endif //DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#endif //TEAPOT_SHADER_SHADOW_MAP_PCF

typedef enum {
    TEAPOT_MESH_TEAPOT=0,
    TEAPOT_MESH_ARROW,
    TEAPOT_MESH_BUNNY,
    TEAPOT_MESH_CAR,                    // TEAPOT_NO_MESH_CAR_WHEELS can be defined to strip wheels
    TEAPOT_MESH_CHAIR,
    TEAPOT_MESH_CHARACTER,
    TEAPOT_MESH_GHOST,
    TEAPOT_MESH_SKITTLE,
    TEAPOT_MESH_SLEDGE,
    TEAPOT_MESH_TABLE,
    TEAPOT_MESH_TORUS,
    TEAPOT_MESH_USER_00,                // TEAPOT_MESH_USER_XX meshes must be entered through Teapot_Set_Init_UserMeshCallback(...) before calling Teapot_Init(...)
    TEAPOT_MESH_USER_01,
    TEAPOT_MESH_USER_02,
    TEAPOT_MESH_USER_03,
    TEAPOT_MESH_USER_04,
    TEAPOT_MESH_USER_05,
    TEAPOT_MESH_USER_06,
    TEAPOT_MESH_USER_07,
    TEAPOT_MESH_USER_08,
    TEAPOT_MESH_USER_09,
    TEAPOT_MESH_USER_10,
    TEAPOT_MESH_USER_11,
    TEAPOT_MESH_USER_12,
    TEAPOT_MESH_USER_13,
    TEAPOT_MESH_USER_14,
    TEAPOT_MESH_USER_15,
    TEAPOT_MESH_USER_16,
    TEAPOT_MESH_USER_17,
    TEAPOT_MESH_USER_18,
    TEAPOT_MESH_USER_19,
    TEAPOT_MESH_USER_20,
    TEAPOT_MESH_USER_21,
    TEAPOT_MESH_USER_22,
    TEAPOT_MESH_USER_23,
    TEAPOT_MESH_USER_24,
    TEAPOT_MESH_USER_25,
    TEAPOT_MESH_USER_26,
    TEAPOT_MESH_USER_27,
    TEAPOT_MESH_USER_28,
    TEAPOT_MESH_USER_29,
    TEAPOT_MESH_CUBE,
    TEAPOT_MESH_CUBIC_GROUND,           // Top face is made by 5x5 vertices
    TEAPOT_MESH_PLANE_X,
    TEAPOT_MESH_PLANE_Y,
    TEAPOT_MESH_PLANE_Z,
    TEAPOT_MESH_CYLINDER,
    TEAPOT_MESH_CONE1,
    TEAPOT_MESH_CONE2,                  // Better quality
    TEAPOT_MESH_PYRAMID,
    TEAPOT_MESH_ROOF,
    TEAPOT_MESH_SPHERE1,
    TEAPOT_MESH_SPHERE2,                // Better quality
    TEAPOT_MESH_CAPSULE,                // Warning: this is special: itsHeight=scaling.y+(scaling.x+scaling.z)*0.5f; itsDiameter=(scaling.x+scaling.z)*0.5f; (this avoids non-uniform scaling)
    TEAPOT_MESH_CYLINDER_LATERAL_SURFACE,
    TEAPOT_MESH_HALF_SPHERE_UP,         // Warning: These is not centered when TEAPOT_CENTER_MESHES_ON_FLOOR is defined
    TEAPOT_MESH_HALF_SPHERE_DOWN,       // Warning: These is not centered when TEAPOT_CENTER_MESHES_ON_FLOOR is defined
    TEAPOT_MESH_PIVOT3D,                // Warning: These is not centered when TEAPOT_CENTER_MESHES_ON_FLOOR is defined
    TEAPOT_MESH_TEXT_X,                 // Warning: These is not centered when TEAPOT_CENTER_MESHES_ON_FLOOR is defined
    TEAPOT_MESH_TEXT_Y,                 // Warning: These is not centered when TEAPOT_CENTER_MESHES_ON_FLOOR is defined
    TEAPOT_MESH_TEXT_Z,                 // Warning: These is not centered when TEAPOT_CENTER_MESHES_ON_FLOOR is defined
    // GL_LINES starts here:
    TEAPOT_MESHLINES_CUBE_EDGES,        // Experimental GL_LINES
    TEAPOT_MESH_COUNT
} TeapotMeshEnum;
#define TEAPOT_FIRST_MESHLINES_INDEX (TEAPOT_MESHLINES_CUBE_EDGES)
#define TEAPOT_LAST_CONCAVE_MESH_INDEX (TEAPOT_MESH_TORUS)

// This callback can be useful when users want to add some simple meshes: just verts + inds, all sharing the same color, and nothing else [= NO TEXTURE!]
typedef void (*TeapotInitUserMeshCallback)(TeapotMeshEnum meshId,const float** ppverts,int* numVerts,const unsigned short** ppinds,int* numInds); // numVerts is the number of vertices (each vertex is 3 floats)
void Teapot_Set_Init_UserMeshCallback(TeapotInitUserMeshCallback callback); // (Optional/Advanced Users) to be called before Teapot_Init(void)

// This callback can be useful when users need to do something with the verts/inds of the meshes
typedef void (*TeapotInitCallback)(TeapotMeshEnum meshId,const float* pverts,int numVerts,const unsigned short* pinds,int numInds); // numVerts is the number of vertices (each vertex is 3 floats)
void Teapot_Set_Init_Callback(TeapotInitCallback callback); // (Optional/Advanced Users) to be called before Teapot_Init(void)

void Teapot_Init(void);     // In your InitGL() method
void Teapot_Destroy(void);  // In your DestroyGL() method (cleanup)

// In your InitGL() and ResizeGL() methods:
void Teapot_SetProjectionMatrix(const tpoat pMatrix[16]);   // Sets the projection matrix [Warning: it calls glUseProgram(0); at the end => call it outside Teapot_PreDraw()/Teapot_PostDraw()]
#ifdef TEAPOT_SHADER_FOG
void Teapot_SetFogColor(float R, float G, float B);  // it should be the same as glClearColor() [Warning: it calls glUseProgram(0); at the end => call it outside Teapot_PreDraw()/Teapot_PostDraw()]
void Teapot_SetFogDistances(float startDistance,float endDistance); // endDistance should be equal to the far clipping plane [Warning: it calls glUseProgram(0); at the end => call it outside Teapot_PreDraw()/Teapot_PostDraw()]
#endif //TEAPOT_SHADER_FOG

// In your DrawGL() method:
void Teapot_SetViewMatrixAndLightDirection(const tpoat vMatrix[16],tpoat lightDirectionWorldSpace[3]);    // vMatrix CAN'T HAVE any scaling! Sets the camera view matrix (= gluLookAt matrix) and the directional light in world space [Warning: it calls glUseProgram(0); at the end => call it outside Teapot_PreDraw()/Teapot_PostDraw()]

// 6 Helper Functions. If used, they must be called AFTER Teapot_SetViewMatrixAndLightDirection(...)
void Teapot_GetViewMatrix(tpoat* res16);                                    // camera view matrix (16 tpoat)
const tpoat* Teapot_GetViewMatrixConstReference();                          // camera view matrix (16 tpoat)
void Teapot_GetNormalizedLightDirection(tpoat* res3);                       // directional light in world space (3 tpoat)
const tpoat* Teapot_GetNormalizedLightDirectionConstReference();            // directional light in world space (3 tpoat)
void Teapot_GetNormalizedLightDirectionInViewSpace(tpoat* res3);            // directional light in view space (3 tpoat)
const tpoat* Teapot_GetNormalizedLightDirectionInViewSpaceConstReference(); // directional light in view space (3 tpoat)

#ifdef TEAPOT_SHADER_USE_SHADOW_MAP
void Teapot_SetShadowVpMatrix(const tpoat unbiasedShadowVpMatrix[16]);  // MANDATORY: Must be called AFTER Teapot_SetViewMatrixAndLightDirection(...) Warning: Do NOT call inside Teapot_PreDraw()/Teapot_PostDraw(): it sets glUseProgram(0) before exiting.
void Teapot_SetShadowDarkening(float darkeningFactorIn_0_80,float shadowMinIntensityIn_0_1);    // Optional. Default values are (40.0f,0.75f). Warning: Do NOT call inside Teapot_PreDraw()/Teapot_PostDraw(): it sets glUseProgram(0) before exiting.
void Teapot_GetViewMatrixInverse(tpoat* res16);				// Helper: (it's the camera matrix). Must be called AFTER Teapot_SetViewMatrixAndLightDirection(...)
const tpoat* Teapot_GetViewMatrixInverseConstReference();   // Helper: (it's the camera matrix). Must be called AFTER Teapot_SetViewMatrixAndLightDirection(...)
#endif //TEAPOT_SHADER_USE_SHADOW_MAP

//------------------------------------------------------------------------------------
void Teapot_PreDraw(void);  // sets program and buffers for drawing
//------------------------------------------------------------------------------------

void Teapot_SetColor(float R, float G, float B, float A);     // Optional (last set is used). A<1.0 requires glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); and glEnable(GL_BLEND); to make transparency work
void Teapot_SetColorAmbient(float R, float G, float B);       // Optional (last set is used).
#ifdef TEAPOT_SHADER_SPECULAR
void Teapot_SetColorSpecular(float R, float G, float B, float SHI); // Optional (last set is used). If SHI<0, last valid call value is used
#endif //TEAPOT_SHADER_SPECULAR

void Teapot_SetScaling(float scalingX,float scalingY,float scalingZ);   // Optional (last set is used): it scales all the vertices, leaving the normals unaffected (a good perfomance/quality compromise)

// There are two Teapot_Draw functions:
void Teapot_Draw(const tpoat mMatrix[16],TeapotMeshEnum meshId);        // (optionally repeat this call for multiple draws)
void Teapot_Draw_Mv(const tpoat mvMatrix[16],TeapotMeshEnum meshId);    // (optionally repeat this call for multiple draws) Note that this is not the model matrix, but: Teapot_Helper_MultMatrix(mvMatrix,vMatrix,mMatrix)

// There are two Teapot_DrawMulti functions:
typedef struct _Teapot_MeshData {
#   ifdef TEAPOT_MESHDATA_HAS_MMATRIX_PTR
    const tpoat* mMatrix;   // Input for Teapot_DrawMulti(...) [Useful if your physic engine already provides a tpoat mMatrix[16] somewhere]
#   else
    tpoat mMatrix[16];      // Input for Teapot_DrawMulti(...)
#   endif
    tpoat mvMatrix[16];     // Output for Teapot_DrawMulti(...), but Input if Teapot_DrawMulti_Mv(...) is used [Teapot_Helper_MultMatrix(mvMatrix,vMatrix,mMatrix)];
    // Input:
    TeapotMeshEnum meshId;
    float scaling[3];
    float color[4];
    float colorAmbient[3];  // Skipped when Teapot_Color_Material is enabled
    float colorSpecular[4]; // Skipped when Teapot_Color_Material is enabled. Used only when TEAPOT_SHADER_SPECULAR is defined
    int outlineEnabled;     // 0 or 1
    void* userPtr;          // yours
#   ifdef __cplusplus
    _Teapot_MeshData();
    virtual ~_Teapot_MeshData() {}  // In case we want to inherit from it in C++
#   endif
}  Teapot_MeshData;
void Teapot_MeshData_Clear(Teapot_MeshData* md);
#ifndef TEAPOT_MESHDATA_HAS_MMATRIX_PTR
void Teapot_MeshData_SetMMatrix(Teapot_MeshData* md,const tpoat* mMatrix16);
#endif //TEAPOT_MESHDATA_HAS_MMATRIX_PTR
void Teapot_MeshData_SetMvMatrix(Teapot_MeshData* md,const tpoat* mvMatrix16);
static __inline void Teapot_MeshData_SetScaling(Teapot_MeshData* md,float scalingX,float scalingY,float scalingZ) {md->scaling[0]=scalingX;md->scaling[1]=scalingY;md->scaling[2]=scalingZ;}
static __inline void Teapot_MeshData_SetColor(Teapot_MeshData* md,float R,float G,float B,float A) {md->color[0]=R;md->color[1]=G;md->color[2]=B;md->color[3]=A;}
static __inline void Teapot_MeshData_SetColorAmbient(Teapot_MeshData* md,float R,float G,float B) {md->colorAmbient[0]=R;md->colorAmbient[1]=G;md->colorAmbient[2]=B;}
#ifdef TEAPOT_SHADER_SPECULAR
static __inline void Teapot_MeshData_SetColorSpecular(Teapot_MeshData* md,float R, float G, float B, float SHI) {md->colorSpecular[0]=R;md->colorSpecular[1]=G;md->colorSpecular[2]=B;md->colorSpecular[3]=SHI;}
#endif //TEAPOT_SHADER_SPECULAR
static __inline void Teapot_MeshData_SetOutlineEnabled(Teapot_MeshData* md,int meshOutlineEnabled) {md->outlineEnabled=meshOutlineEnabled;}
static __inline void Teapot_MeshData_SetMeshId(Teapot_MeshData* md,TeapotMeshEnum meshId) {md->meshId = meshId;}
void Teapot_MeshData_CalculateMvMatrix(Teapot_MeshData* md);    // From mMatrix (called internally when Teapot_DrawMulti(...) is used)
void Teapot_MeshData_CalculateMvMatrixFromArray(Teapot_MeshData** meshes,int numMeshes);  // From mMatrix (called internally when Teapot_DrawMulti(...) is used)
// I think the following function works only if no scaling is inside any Teapot_MeshData::mMatrix (But Teapot_MeshData_SetScaling(...) is OK):
Teapot_MeshData* Teapot_MeshData_GetMeshUnderMouse(Teapot_MeshData* const* meshes,int numMeshes,int mouseX,int mouseY,const int* viewport4,tpoat* pOptionalDistanceOut);

void Teapot_DrawMulti(Teapot_MeshData** meshes,int numMeshes,int mustSortObjectsForTransparency);  // 'mustSortObjectsForTransparency' requires glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); and  glDisable(GL_BLEND); At the end it restores glDisable(GL_BLEND); if used.
void Teapot_DrawMulti_Mv(Teapot_MeshData* const* meshes,int numMeshes,int mustSortObjectsForTransparency);  // Same as above, but use it only if you set or calculate all the Teapot_MeshData::mvMatrix[16] manually

//----------------------------------------------------------------------------------------
void Teapot_PostDraw(void); // unsets program and buffers for drawing
//----------------------------------------------------------------------------------------

void Teapot_GetMeshAabbCenter(TeapotMeshEnum meshId,float center[3]);
void Teapot_GetMeshAabbHalfExtents(TeapotMeshEnum meshId,float halfAabb[3]);
void Teapot_GetMeshAabbExtents(TeapotMeshEnum meshId,float aabb[3]);
void Teapot_GetMeshAabbMinAndMax(TeapotMeshEnum meshId,float aabbMin[3],float aabbMax[3]);


void Teapot_Enable_ColorMaterial(void);     // (*)
void Teapot_Disable_ColorMaterial(void);
int Teapot_Get_ColorMaterial_Enabled(void);    // returns 0 or 1

// (*) When Teapot_Color_Material is enabled, the only change is that:
/*  Teapot_SetColor(float R, float G, float B, float A)
    internally adds the following calls:
    Teapot_SetColorAmbient(...);
    Teapot_SetColorSpecular(...);
    with values derived from R,G,B,A.
*/

void Teapot_Enable_MeshOutline(void);       // Needs glEnable(GL_CULL_FACE). Adds an outline around the mesh.
void Teapot_Disable_MeshOutline(void);
int Teapot_Get_MeshOutline_Enabled(void);    // returns 0 or 1
void Teapot_Set_MeshOutline_Color(float R,float G, float B, float A);	// A<1.0 requires glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); and glEnable(GL_BLEND); to make transparency work
void Teapot_Set_MeshOutline_Scaling(float scalingGreaterOrEqualThanOne);    // default is 1.015f
void Teapot_Set_MeshOutline_Params(float polygonOffsetSlope, float polygonOffsetConstant);  // defaults are -1.f, -250.f

static __inline tpoat* Teapot_Helper_IdentityMatrix(tpoat* __restrict result16) {
    tpoat* m = result16;
    m[0]=m[5]=m[10]=m[15]=1;
    m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=0;
    return result16;
}
static __inline void Teapot_Helper_CopyMatrix(tpoat* __restrict dst16,const tpoat* __restrict src16) {
    int i;for (i=0;i<16;i++) dst16[i]=src16[i];
}
static __inline tpoat* Teapot_Helper_MultMatrixUncheckArgs(tpoat* __restrict result16,const tpoat* __restrict ml16,const tpoat* __restrict mr16) {	
	int i,i4;   
#	if (defined(TEAPOT_USE_SIMD) && !defined(TEAPOT_MATRIX_USE_DOUBLE_PRECISION) && defined(__SSE__))
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
#	elif (defined(TEAPOT_USE_SIMD) && defined(TEAPOT_MATRIX_USE_DOUBLE_PRECISION) && defined(__AVX__))
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
#	else //MOL_USE_DOUBLE_PRECISION
	/* reference implementation */	
	tpoat mri4plus0,mri4plus1,mri4plus2,mri4plus3;
	for(i = 0; i < 4; i++) {
        i4=4*i;mri4plus0=mr16[i4];mri4plus1=mr16[i4+1];mri4plus2=mr16[i4+2];mri4plus3=mr16[i4+3];
		result16[  i4] = ml16[0]*mri4plus0 + ml16[4]*mri4plus1 + ml16[ 8]*mri4plus2 + ml16[12]*mri4plus3;
		result16[1+i4] = ml16[1]*mri4plus0 + ml16[5]*mri4plus1 + ml16[ 9]*mri4plus2 + ml16[13]*mri4plus3;
		result16[2+i4] = ml16[2]*mri4plus0 + ml16[6]*mri4plus1 + ml16[10]*mri4plus2 + ml16[14]*mri4plus3;
		result16[3+i4] = ml16[3]*mri4plus0 + ml16[7]*mri4plus1 + ml16[11]*mri4plus2 + ml16[15]*mri4plus3;
    }	
#	endif //MOL_USE_DOUBLE_PRECISION
	return result16;
}
static __inline tpoat* Teapot_Helper_MultMatrix(tpoat* __restrict result16,const tpoat* __restrict ml16,const tpoat* __restrict mr16) {
    if (result16==ml16) {
        tpoat ML16[16];Teapot_Helper_CopyMatrix(ML16,ml16);
        return Teapot_Helper_MultMatrixUncheckArgs(result16,ML16,mr16);
    }
    else if (result16==mr16) {
        tpoat MR16[16];Teapot_Helper_CopyMatrix(MR16,mr16);
        return Teapot_Helper_MultMatrixUncheckArgs(result16,ml16,MR16);
    }
    return Teapot_Helper_MultMatrixUncheckArgs(result16,ml16,mr16);
}

void Teapot_Helper_LookAt(tpoat* __restrict mOut16,tpoat eyeX,tpoat eyeY,tpoat eyeZ,tpoat centerX,tpoat centerY,tpoat centerZ,tpoat upX,tpoat upY,tpoat upZ);
void Teapot_Helper_Perspective(tpoat* __restrict mOut16,tpoat degfovy,tpoat aspect, tpoat zNear, tpoat zFar);
void Teapot_Helper_Ortho(tpoat* __restrict mOut16,tpoat left,tpoat right, tpoat bottom, tpoat top,tpoat nearVal,tpoat farVal);
tpoat* Teapot_Helper_TranslateMatrix(tpoat* __restrict mInOut16,tpoat x,tpoat y,tpoat z);
tpoat* Teapot_Helper_RotateMatrix(tpoat* __restrict mInOut16,tpoat degAngle,tpoat x,tpoat y,tpoat z);
tpoat* Teapot_Helper_ScaleMatrix(tpoat* __restrict mInOut16,tpoat x,tpoat y,tpoat z);
int Teapot_Helper_InvertMatrix(tpoat* __restrict mOut16,const tpoat* __restrict m16);
void Teapot_Helper_InvertMatrixFast(tpoat* __restrict mOut16,const tpoat* __restrict m16);
void Teapot_Helper_GetFrustumPlaneEquations(tpoat planeEquationsOut[6][4],const tpoat* __restrict vpMatrix16,int normalizePlanes);
void Teapot_Helper_GetFrustumPoints(tpoat frustumPoints[8][4],const tpoat* __restrict vpMatrixInverse16);   // frustumPoints[i][3]==1
// 'optionalPMatrixInverse16' is required only if you need to retrieve (one or more of) the arguments that follow it (otherwise their value is untouched).
void Teapot_Helper_GetLightViewProjectionMatrixExtra(tpoat* __restrict lvpMatrixOut16,
                                                          const tpoat* __restrict cameraVMatrixInverse16,
                                                          tpoat cameraNearClippingPlane,tpoat cameraFarClippingPlane,tpoat cameraFovyDeg,tpoat cameraAspectRatio,
                                                          const tpoat*  __restrict normalizedLightDirection3, tpoat texelIncrement
                                                          ,tpoat* __restrict optionalSphereCenterOut,tpoat* __restrict optionalSphereRadiiSquaredOut
                                                          ,const tpoat* __restrict optionalCameraPMatrixInverse16
                                                          ,tpoat* __restrict optionalLightViewportClippingOut4,tpoat optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,tpoat* __restrict optionalLVPMatrixForFrustumCullingUsageOut16
                                                          );
// This function can be used to auto-calculate the light vpMatrix (for a directional light),that fits a camera frustum
static __inline void  Teapot_Helper_GetLightViewProjectionMatrix(tpoat* __restrict lvpMatrixOut16,
                                                          const tpoat* __restrict cameraVMatrixInverse16,
                                                          tpoat cameraNearClippingPlane,tpoat cameraFarClippingPlane,tpoat cameraFovyDeg,tpoat cameraAspectRatio,
                                                          const tpoat*  __restrict normalizedLightDirection3, tpoat texelIncrement)  {
     Teapot_Helper_GetLightViewProjectionMatrixExtra(lvpMatrixOut16,cameraVMatrixInverse16,cameraNearClippingPlane,cameraFarClippingPlane,cameraFovyDeg,cameraAspectRatio,normalizedLightDirection3,texelIncrement,0,0,0,0,0,0);
}
static __inline void Teapot_Helper_MatrixMulDir(const tpoat* __restrict m16,tpoat* __restrict dirOut3,const tpoat dirX,tpoat dirY,tpoat dirZ) {
    //tpoat w;
    dirOut3[0] = dirX*m16[0] + dirY*m16[4] + dirZ*m16[8];
    dirOut3[1] = dirX*m16[1] + dirY*m16[5] + dirZ*m16[9];
    dirOut3[2] = dirX*m16[2] + dirY*m16[6] + dirZ*m16[10];
    //w          = dirX*m16[3] + dirY*m16[7] + dirZ*m16[11]; // + m[15] ?
    //if (w!=0 && w!=1) {dirOut3[0]/=w;dirOut3[1]/=w;dirOut3[2]/=w;}
}
static __inline void Teapot_Helper_MatrixMulPos(const tpoat* __restrict m16,tpoat* __restrict posOut3,const tpoat posX,tpoat posY,tpoat posZ) {
    tpoat w;
    posOut3[0] = posX*m16[0] + posY*m16[4] + posZ*m16[8] + m16[12];
    posOut3[1] = posX*m16[1] + posY*m16[5] + posZ*m16[9] + m16[13];
    posOut3[2] = posX*m16[2] + posY*m16[6] + posZ*m16[10]+ m16[14];
    w          = posX*m16[3] + posY*m16[7] + posZ*m16[11]+ m16[15];
    if (w!=0 && w!=1) {posOut3[0]/=w;posOut3[1]/=w;posOut3[2]/=w;}
}
static __inline void Teapot_Helper_NormalizePlane(tpoat* __restrict p4) {
    tpoat len = p4[0]*p4[0]+p4[1]*p4[1]+p4[2]*p4[2];
    if (len!=0) {
        int i;len = sqrt(len);
        for (i=0;i<4;i++) p4[i]/=len;
    }
}
static __inline tpoat Teapot_Helper_Vector3Dot(const tpoat* a3,const tpoat* b3) {return a3[0]*b3[0]+a3[1]*b3[1]+a3[2]*b3[2];}
static __inline void Teapot_Helper_Vector3Normalize(tpoat* __restrict v3) {
    tpoat len = Teapot_Helper_Vector3Dot(v3,v3);int i;
    if (len!=0) {len = sqrt(len);for (i=0;i<3;i++) v3[i]/=len;}
}
static __inline void Teapot_Helper_Vector3Cross(tpoat* __restrict vOut3,const tpoat* __restrict a3,const tpoat* __restrict b3) {
    vOut3[0] =	a3[1] * b3[2] - a3[2] * b3[1];
    vOut3[1] =	a3[2] * b3[0] - a3[0] * b3[2];
    vOut3[2] =	a3[0] * b3[1] - a3[1] * b3[0];
}
static __inline tpoat Teapot_Helper_Vector3DistSquared(const tpoat* __restrict a3,const tpoat* __restrict b3) {
    const tpoat rv[3] = {b3[0]-a3[0],b3[1]-a3[1],b3[2]-a3[2]};
    return rv[0]*rv[0] + rv[1]*rv[1] + rv[2]*rv[2];
}
static __inline tpoat Teapot_Helper_Vector3Dist(const tpoat* __restrict a3,const tpoat* __restrict b3) {
    const tpoat res = Teapot_Helper_Vector3DistSquared(a3,b3);
    return res!=0 ? sqrt(res) : 0;
}
static __inline tpoat Teapot_Helper_Round(tpoat number)	{return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);}


// It "should" performs AABB test. mfMatrix16 is the matrix M so that: F*M = mvpMatrix (F being the matrix used to extract the frustum planes). Here we use: F=pMatrix and M=mvMatrix, but it could be: F=vpMatrix and M=mMatrix too.
int Teapot_Helper_IsVisible(const tpoat frustumPlanes[6][4],const tpoat*__restrict mfMatrix16,tpoat aabbMinX,tpoat aabbMinY,tpoat aabbMinZ,tpoat aabbMaxX,tpoat aabbMaxY,tpoat aabbMaxZ);
int Teapot_Helper_UnProject_MvpMatrixInv(tpoat winX,tpoat winY,tpoat winZ,const tpoat* __restrict mvpMatrixInv16,const int* viewport4,tpoat* objX,tpoat* objY,tpoat* objZ);
// Maps the specified window coordinates into object coordinates using mvMatrix16, pMatrix16, and viewport4.
// The result is stored in objX, objY, and objZ. A return value of 1 indicates success; a return value of 0 indicates failure.
// viewport4 Specifies the current viewport (as from a glGetIntegerv call).
static __inline int Teapot_Helper_UnProject(tpoat winX,tpoat winY,tpoat winZ,const tpoat* __restrict mvMatrix16,const tpoat* __restrict pMatrix16,const int* viewport4,tpoat* objX,tpoat* objY,tpoat* objZ) {
    tpoat mvpMatrixInv[16];Teapot_Helper_MultMatrix(mvpMatrixInv,pMatrix16,mvMatrix16);Teapot_Helper_InvertMatrix(mvpMatrixInv,mvpMatrixInv);
    return Teapot_Helper_UnProject_MvpMatrixInv(winX,winY,winZ,mvpMatrixInv,viewport4,objX,objY,objZ);
}

static __inline void Teapot_Helper_ConvertMatrixd2f16(float* __restrict result16,const double* __restrict m16) {int i;for(i = 0; i < 16; i++) result16[i]=(float)m16[i];}
static __inline void Teapot_Helper_ConvertMatrixf2d16(double* __restrict result16,const float* __restrict m16) {int i;for(i = 0; i < 16; i++) result16[i]=(double)m16[i];}
static __inline void Teapot_Helper_ConvertMatrixd2f9(float* __restrict result9,const double* __restrict m9) {int i;for(i = 0; i < 9; i++) result9[i]=(float)m9[i];}
static __inline void Teapot_Helper_ConvertMatrixf2d9(double* __restrict result9,const float* __restrict m9) {int i;for(i = 0; i < 9; i++) result9[i]=(double)m9[i];}


// Low-level API (use it at your own risk, or never use it at all!)

// These can be used to replace Teapot_PreDraw()
void Teapot_LowLevel_EnableVertexAttributes(int enableVertexAttribArray,int enableNormalAttribArray); // args are bool variables (0 or 1)
void Teapot_LowLevel_BindVertexBufferObject(void);
void Teapot_LowLevel_BindShaderProgram(void);

void Teapot_LowLevel_StartDisablingLighting(void);      // Use Teapot_SetAmbientColor() to set the color then (but ALPHA is still set through last call to Teapot_SetColor(...))
void Teapot_LowLevel_SetMvMatrixUniform(const tpoat mvMatrix[16]);   // This just sets the uniform matrix
void Teapot_LowLevel_DrawElements(TeapotMeshEnum meshId);   // This just calls glDrawElements(...)
void Teapot_LowLevel_StopDisablingLighting(void);

// These can be used to replace Teapot_PostDraw()
void Teapot_LowLevel_UnbindShaderProgram(void);
void Teapot_LowLevel_UnbindVertexBufferObject(void);
void Teapot_LowLevel_DisableVertexAttributes(int disableVertexAttribArray,int disableNormalAttribArray); // args are bool variables (0 or 1)

#if (defined(DYNAMIC_RESOLUTION_H) && defined(TEAPOT_SHADER_USE_SHADOW_MAP))
#if ((defined(DYNAMIC_RESOLUTION_USE_DOUBLE_PRECISION) && defined(TEAPOT_USE_DOUBLE_PRECISION)) || (!defined(DYNAMIC_RESOLUTION_USE_DOUBLE_PRECISION) && !defined(TEAPOT_USE_DOUBLE_PRECISION)))
// lpvMatrix16 = lpMatrix16 * lvMatrix16 (light projection and view matrices)
void Teapot_HiLevel_DrawMulti_ShadowMap_Vp(Teapot_MeshData* const* pMeshData,int numMeshData,const tpoat* lvpMatrix16, float transparent_threshold);
// Not always faster (overhead + no BVH). Anyway use: Teapot_Helper_GetFrustumPlaneEquations(vpMatrixFrustumPlaneEquations,lvpMatrix16,0);
void Teapot_HiLevel_DrawMulti_ShadowMap_Vp_WithFrustumCulling(Teapot_MeshData* const* pMeshData,int numMeshData,const tpoat* lvpMatrix16, const tpoat lvpMatrixFrustumPlaneEquations[6][4], float transparent_threshold);
static __inline void Teapot_HiLevel_DrawMulti_ShadowMap(Teapot_MeshData* const* pMeshData,int numMeshData,const tpoat* lpMatrix16, const tpoat* lvMatrix16, float transparent_threshold) {
    tpoat lpvMatrix16[16];Teapot_Helper_MultMatrix(lpvMatrix16,lpMatrix16,lvMatrix16);
    Teapot_HiLevel_DrawMulti_ShadowMap_Vp(pMeshData,numMeshData,lpvMatrix16,transparent_threshold);
}
#endif
#endif



#ifdef __cplusplus
}
#endif

#endif //TEAPOT_H_


#ifdef TEAPOT_IMPLEMENTATION

#ifndef TEAPOT_IMPLEMENTATION_H
#define TEAPOT_IMPLEMENTATION_H // Additional guard

#include <math.h>   // sqrt
#include <stdlib.h> // qsort
#ifdef TEAPOT_USE_OPENMP
#include <omp.h>
#endif //TEAPOT_USE_OPENMP

#ifndef TEAPOT_SHADER_SHADOW_MAP_PCF
#   ifdef DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#       define TEAPOT_SHADER_SHADOW_MAP_PCF DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#   else //DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#       define TEAPOT_SHADER_SHADOW_MAP_PCF 0
#   endif //DYNAMIC_RESOLUTION_SHADOW_USE_PCF
#endif //TEAPOT_SHADER_SHADOW_MAP_PCF

#ifndef STR_MACRO
#   define STR_MACRO(s) #s
#endif //STR_MACRO
#ifndef XSTR_MACRO
#   define XSTR_MACRO(s) STR_MACRO(s)
#endif //XSTR_MACRO


#ifdef __cplusplus
extern "C" {
#endif

__inline static void Teapot_Helper_GlUniformMatrix4v(GLint location,GLsizei count,GLboolean transpose,const tpoat* value) {
    const float* fvalue = NULL;
#   ifndef TEAPOT_MATRIX_USE_DOUBLE_PRECISION
    fvalue = value;
#   else
    float val[16];Teapot_Helper_ConvertMatrixd2f16(val,value);fvalue=val;
#   endif
    glUniformMatrix4fv(location,count,transpose,fvalue);
}
__inline static void Teapot_Helper_GlUniformMatrix3v(GLint location,GLsizei count,GLboolean transpose,const tpoat* value) {
    const float* fvalue = NULL;
#   ifndef TEAPOT_MATRIX_USE_DOUBLE_PRECISION
    fvalue = value;
#   else
    float val[9];Teapot_Helper_ConvertMatrixd2f9(val,value);fvalue=val;
#   endif
    glUniformMatrix3fv(location,count,transpose,fvalue);
}

__inline static void Teapot_Helper_GlUniform3v(GLint location,GLsizei count,const tpoat* value) {
    const float* fvalue = NULL;
#   ifndef TEAPOT_MATRIX_USE_DOUBLE_PRECISION
    fvalue = value;
#   else
    const float val[3] = {(float)value[0],(float)value[1],(float)value[2]};fvalue = val;
#   endif
    glUniform3fv(location,count,fvalue);
}


static const char* TeapotVS[] = {
#   if (defined(__EMSCRIPTEN__) && defined(TEAPOT_SHADER_USE_SHADOW_MAP) && (TEAPOT_SHADER_SHADOW_MAP_PCF>0))
    "#version 300 es\n"         // emscripten needs: -s USE_WEBGL2=1
    "#define attribute in\n"
    "#define varying out\n"
#   endif //__EMSCRIPTEN__ && TEAPOT_SHADER_USE_SHADOW_MAP && TEAPOT_SHADER_SHADOW_MAP_PCF
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "attribute vec4 a_vertex;\n"
    "attribute vec3 a_normal;\n"
    "uniform mat4 u_mvMatrix;\n"
    "uniform mat4 u_pMatrix;\n"
#   ifdef TEAPOT_SHADER_USE_NORMAL_MATRIX
    "uniform mat3 u_nMatrix;\n"
#   endif //TEAPOT_SHADER_USE_NORMAL_MATRIX
    "uniform vec4 u_scaling;\n"
    "uniform vec3 u_lightVector;\n"
    "uniform vec4 u_color;\n"
    "uniform vec4 u_colorAmbient;\n"
#   ifdef TEAPOT_SHADER_SPECULAR
    "uniform vec4 u_colorSpecular;\n"   // R,G,B,SHININESS
#   endif //TEAPOT_SHADER_SPECULAR
#   ifdef TEAPOT_SHADER_FOG
    "uniform vec4 u_fogDistances;\n"
#   ifndef TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "uniform vec3 u_fogColor;\n"
#   else //TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "varying float v_fog;\n"
#   endif // TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
#   endif // TEAPOT_SHADER_FOG
#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    "uniform mat4 u_biasedShadowMvpMatrix;\n"
    "varying vec4 v_shadowCoord;\n"
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP
    "varying vec4 v_color;\n"
    "\n"
    "void main()	{\n"
#   ifndef TEAPOT_SHADER_USE_NORMAL_MATRIX
    "   vec3 normalEyeSpace = vec3(u_mvMatrix * vec4(a_normal, 0.0));\n"
#   else //TEAPOT_SHADER_USE_NORMAL_MATRIX
    //"   vec3 normalEyeSpace = u_nMatrix * a_normal;\n"
    "   vec3 normalEyeSpace = normalize(u_nMatrix * a_normal);\n"
#   endif //TEAPOT_SHADER_USE_NORMAL_MATRIX
    "   float fDot = max(0.0, dot(normalEyeSpace,u_lightVector));\n"
    "   vec4 vertexScaledWorldSpace = a_vertex * u_scaling;\n"
    "   vec4 vertexScaledEyeSpace = u_mvMatrix*vertexScaledWorldSpace;\n"
    "   //vertexScaledEyeSpace.xyz/vertexScaledEyeSpace.w;\n"                 // is this necessary ?
#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    "   v_shadowCoord = u_biasedShadowMvpMatrix*vertexScaledWorldSpace;\n"
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP
#   ifndef TEAPOT_SHADER_SPECULAR
    "   v_color = vec4(u_colorAmbient.rgb + u_color.rgb*(fDot*u_colorAmbient.a),u_color.a);\n"
#   else  // TEAPOT_SHADER_SPECULAR
    "   vec3 E = normalize(-vertexScaledEyeSpace.xyz);\n"
    "   vec3 halfVector = normalize(u_lightVector + E);\n"
    "   float nxHalf = max(0.005,dot(normalEyeSpace, halfVector));\n"
    "   vec3 specularColor = u_colorSpecular.rgb * pow(nxHalf,u_colorSpecular.a);\n"
    "   v_color = vec4(u_colorAmbient.rgb + (u_color.rgb*fDot+specularColor.rgb)*u_colorAmbient.a,u_color.a);\n"
#   endif // TEAPOT_SHADER_SPECULAR
#   ifdef TEAPOT_SHADER_FOG
#   ifdef TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "  v_fog = \n"
#   else //TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "   float v_fog = \n"
#   endif //TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "   1.0 - (u_fogDistances.y-abs(vertexScaledEyeSpace.z))*u_fogDistances.w;\n"
#   ifndef TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "  v_color.rgb = mix(v_color.rgb,u_fogColor.rgb,v_fog);\n"
#   endif //TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
#   endif //TEAPOT_SHADER_FOG
    "   gl_Position = u_pMatrix * vertexScaledEyeSpace;\n"
    "}\n"
};
// 0.2 is the global ambient factor

static const char* TeapotFS[] = {
#   if (defined(__EMSCRIPTEN__) && defined(TEAPOT_SHADER_USE_SHADOW_MAP) && (TEAPOT_SHADER_SHADOW_MAP_PCF>0))
    "#version 300 es\n"         // emscripten needs: -s USE_WEBGL2=1
    "#define varying in\n"
    "#define shadow2D texture\n"
    "#define gl_FragColor FragColor\n"
#   endif //__EMSCRIPTEN__ && TEAPOT_SHADER_USE_SHADOW_MAP && TEAPOT_SHADER_SHADOW_MAP_PCF
    "#define TABSSQRT " XSTR_MACRO(TEAPOT_SHADER_SHADOW_MAP_PCF) "\n"
    "#ifdef GL_ES\n"
    "precision mediump float;\n"
    "precision lowp int;\n"
    "#endif\n"
    "varying vec4 v_color;\n"
#   ifdef TEAPOT_SHADER_FOG
#   ifdef TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "uniform vec3 u_fogColor;\n"
    "varying float v_fog;\n"
#   endif //TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
#   endif
#   if (defined(__EMSCRIPTEN__) && defined(TEAPOT_SHADER_USE_SHADOW_MAP) && (TEAPOT_SHADER_SHADOW_MAP_PCF>0))
    "out vec4 gl_FragColor;\n"
#   endif //__EMSCRIPTEN__ && TEAPOT_SHADER_USE_SHADOW_MAP && TEAPOT_SHADER_SHADOW_MAP_PCF
#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
#       if (TEAPOT_SHADER_SHADOW_MAP_PCF<1)
    "       uniform sampler2D u_shadowMap;\n"
#       else    // TEAPOT_SHADER_SHADOW_MAP_PCF
#           ifdef __EMSCRIPTEN__
    "       uniform lowp sampler2DShadow u_shadowMap;\n"
#           else //__EMSCRIPTEN__
    "       uniform sampler2DShadow u_shadowMap;\n"
#           endif //__EMSCRIPTEN__
#       endif   // TEAPOT_SHADER_SHADOW_MAP_PCF
    "uniform vec2 u_shadowDarkening;\n" // .x = fDarkeningFactor [10.0-80.0], .y = min value clamp [0.0-1.0]
    "uniform vec4 u_ambientColor;\n"
    "uniform float u_shadowMapFactor; // in [0,1]: default: 1\n"
    "uniform vec2 u_shadowMapTexelIncrement;\n"
    "varying vec4 v_shadowCoord;\n"
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP
    "\n"
    "void main() {\n"
    "float shadowFactor = 1.0;\n"
#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    "vec4 shadowCoordinateWdivide = v_shadowCoord/v_shadowCoord.w;\n"
    "\n"
#       if (TEAPOT_SHADER_SHADOW_MAP_PCF<1)
    "       shadowFactor = clamp(exp(u_shadowDarkening.x*(1.0-u_ambientColor.a)*(texture2D(u_shadowMap,(shadowCoordinateWdivide.st*u_shadowMapFactor)).r - shadowCoordinateWdivide.z)),u_shadowDarkening.y,1.0);\n"
#       elif (TEAPOT_SHADER_SHADOW_MAP_PCF==1)
    "       shadowFactor = shadow2D(u_shadowMap,vec3(shadowCoordinateWdivide.st,shadowCoordinateWdivide.z-u_shadowDarkening.x));\n"
    "       shadowFactor = u_shadowDarkening.y + (1.0-u_shadowDarkening.y)*shadowFactor;\n"
#       else //TEAPOT_SHADER_SHADOW_MAP_PCF
    "       shadowFactor=0.0;\n"
    "       float biasedShadowCoordinateZ = shadowCoordinateWdivide.z-u_shadowDarkening.x;\n;"
#           if (TEAPOT_SHADER_SHADOW_MAP_PCF== (TEAPOT_SHADER_SHADOW_MAP_PCF/2)*2)  // even
    "           const float edgeVal = 0.5+float((TABSSQRT-1)/2);\n"
    "           const float startVal = -edgeVal;\n"
    "           const float endVal = edgeVal+0.5;\n"    // we use +0.5 and < instead of <= in the for loop (more robust)
#           else // TEAPOT_SHADER_SHADOW_MAP_PCF odd
    "           const float edgeVal = float((TABSSQRT-1)/2);\n"
    "           const float startVal = -edgeVal;\n"
    "           const float endVal = edgeVal+0.5;\n"    // we use +0.5 and < instead of <= in the for loop (more robust)
#           endif // TEAPOT_SHADER_SHADOW_MAP_PCF
    "       float x,y;shadowFactor=0.0;\n;"
    "       for (y=startVal; y<endVal; y+=1.0)   {\n"
    "           for (x=startVal; x<endVal; x+=1.0)   {\n"
    "               shadowFactor += shadow2D(u_shadowMap,vec3(shadowCoordinateWdivide.st+vec2(x*u_shadowMapTexelIncrement.x,y*u_shadowMapTexelIncrement.y),biasedShadowCoordinateZ));\n"
    "           }\n"
    "       }\n"
    "       shadowFactor/=float(TABSSQRT*TABSSQRT);\n"
    "       shadowFactor = u_shadowDarkening.y + (1.0-u_shadowDarkening.y)*shadowFactor;\n"
#       endif //TEAPOT_SHADER_SHADOW_MAP_PCF
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP
#   ifdef TEAPOT_SHADER_FOG
#   ifdef TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
    "   //float v_fog = 1.0 - (u_fogDistances.y-gl_FragCoord.z/gl_FragCoord.w)*u_fogDistances.w;\n" // Best quality but expensive...
    "   gl_FragColor = vec4(mix(v_color.rgb*shadowFactor,u_fogColor.rgb,v_fog).rgb,v_color.a);\n"
    "   return;\n"
#   endif //TEAPOT_SHADER_FOG_HINT_FRAGMENT_SHADER
#   endif //TEAPOT_SHADER_FOG
    "    gl_FragColor = vec4(v_color.rgb*shadowFactor,v_color.a);\n"
    "}\n"
};


typedef struct {
    GLuint programId;
    GLuint vertexBuffer,elementBuffer;
    int startInds[TEAPOT_MESH_COUNT];
    int numInds[TEAPOT_MESH_COUNT];
    float halfExtents[TEAPOT_MESH_COUNT][3];
    float centerPoint[TEAPOT_MESH_COUNT][3];
    float aabbMin[TEAPOT_MESH_COUNT][3];
    float aabbMax[TEAPOT_MESH_COUNT][3];
    GLint aLoc_vertex,aLoc_normal;
    GLint uLoc_mvMatrix,uLoc_pMatrix,uLoc_nMatrix,uLoc_scaling,
    uLoc_lightVector,uLoc_color,uLoc_colorAmbient,uLoc_colorSpecular,
    uLoc_fogColor,uLoc_fogDistances,
    uLoc_biasedShadowMvpMatrix,uLoc_shadowMap,uLoc_shadowDarkening,uLoc_shadowMapFactor,uLoc_shadowMapTexelIncrement;

    tpoat lightDirectionWorldSpace[3];
    tpoat lightDirectionViewSpace[3];
    tpoat pMatrix[16];
    tpoat pMatrixFrustum[6][4];
    tpoat vMatrix[16];
    float scaling[3];

    float color[4];
    float colorAmbient[4];
    float colorSpecular[4];

    tpoat vMatrixInverse[16];
    tpoat biasedShadowVpMatrix[16]; // actually what we store here is: biasedShadowVpMatrix * vMatrixInverse (so that we must multiply it per mvMatrix, instead of mMatrix)
    float shadowDarkening,shadowClamp;

    int colorMaterialEnabled;
    int meshOutlineEnabled;
    float colorMeshOutline[4];
    float scalingMeshOutline;
    float polygonOffsetSlope;
    float polygonOffsetConstant;
} Teapot_Inner_Struct;
static Teapot_Inner_Struct TIS;
static TeapotInitCallback gTeapotInitCallback=NULL;
static TeapotInitUserMeshCallback gTeapotInitUserMeshCallback=NULL;

static __inline GLuint Teapot_LoadShaderProgramFromSource(const char* vs,const char* fs);
void Teapot_Helper_LookAt(tpoat* __restrict mOut16,tpoat eyeX,tpoat eyeY,tpoat eyeZ,tpoat centerX,tpoat centerY,tpoat centerZ,tpoat upX,tpoat upY,tpoat upZ)    {
    tpoat* m = mOut16;
    const tpoat eps = 0.0001;

    tpoat F[3] = {eyeX-centerX,eyeY-centerY,eyeZ-centerZ};
    tpoat length = F[0]*F[0]+F[1]*F[1]+F[2]*F[2];	// length2 now
    tpoat up[3] = {upX,upY,upZ};

    tpoat S[3] = {up[1]*F[2]-up[2]*F[1],up[2]*F[0]-up[0]*F[2],up[0]*F[1]-up[1]*F[0]};
    tpoat U[3] = {F[1]*S[2]-F[2]*S[1],F[2]*S[0]-F[0]*S[2],F[0]*S[1]-F[1]*S[0]};

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
}
void Teapot_Helper_Perspective(tpoat* __restrict mOut16,tpoat degfovy,tpoat aspect, tpoat zNear, tpoat zFar) {
    tpoat* res = mOut16;
    const tpoat eps = 0.0001;
    tpoat f = 1.f/tan(degfovy*1.5707963268f/180.0); //cotg
    tpoat Dfn = (zFar-zNear);
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
void Teapot_Helper_Ortho(tpoat* __restrict mOut16,tpoat left,tpoat right, tpoat bottom, tpoat top,tpoat nearVal,tpoat farVal) {
    tpoat* res = mOut16;
    const tpoat eps = 0.0001;
    tpoat Drl = (right-left);
    tpoat Dtb = (top-bottom);
    tpoat Dfn = (farVal-nearVal);
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
tpoat* Teapot_Helper_TranslateMatrix(tpoat* __restrict mInOut16,tpoat x,tpoat y,tpoat z)  {
    const tpoat m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1};
    return Teapot_Helper_MultMatrix(mInOut16,mInOut16,m);
}
tpoat* Teapot_Helper_RotateMatrix(tpoat* __restrict mInOut16,tpoat degAngle,tpoat x,tpoat y,tpoat z)  {
    const tpoat angle = degAngle*M_PIOVER180;
    const tpoat c = cos(angle);
    const tpoat s = sin(angle);
    tpoat len = x*x+y*y+z*z;
    if (len<0.999f || len>1.001f) {len=sqrt(len);x/=len;y/=len;z/=len;}
    {
        const tpoat m[16] = {
            c + x*x*(1-c),  y*x*(1-c)+z*s,    z*x*(1-c)-y*s,    0,
            x*y*(1-c) - z*s,  c + y*y*(1-c),      z*y*(1-c) + x*s,    0,
            x*z*(1-c) + y*s,  y*z*(1-c) - x*s,    c + z*z*(1-c),      0,
            0,              0,                  0,                  1};
        return Teapot_Helper_MultMatrix(mInOut16,mInOut16,m);
    }
}
tpoat* Teapot_Helper_ScaleMatrix(tpoat* __restrict mInOut16,tpoat x,tpoat y,tpoat z)  {
    const tpoat m[16] = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
    return Teapot_Helper_MultMatrix(mInOut16,mInOut16,m);
}
int Teapot_Helper_InvertMatrix(tpoat* __restrict mOut16,const tpoat* __restrict m16)	{
    const tpoat* m = m16;
    tpoat* n = mOut16;

    tpoat m00 = m[0],  m10 = m[1],  m20 = m[2],  m30 = m[3];
    tpoat m01 = m[4],  m11 = m[5],  m21 = m[6],  m31 = m[7];
    tpoat m02 = m[8],  m12 = m[9],  m22 = m[10], m32 = m[11];
    tpoat m03 = m[12], m13 = m[13], m23 = m[14], m33 = m[15];

    tpoat v0 = m20 * m31 - m21 * m30;
    tpoat v1 = m20 * m32 - m22 * m30;
    tpoat v2 = m20 * m33 - m23 * m30;
    tpoat v3 = m21 * m32 - m22 * m31;
    tpoat v4 = m21 * m33 - m23 * m31;
    tpoat v5 = m22 * m33 - m23 * m32;

    tpoat t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    tpoat t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    tpoat t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    tpoat t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    tpoat det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
    if (det==0) return 0;
    {
        tpoat invDet = 1 / det;

        tpoat d00 = t00 * invDet;
        tpoat d10 = t10 * invDet;
        tpoat d20 = t20 * invDet;
        tpoat d30 = t30 * invDet;

        tpoat d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        tpoat d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        tpoat d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        tpoat d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;
        {
            tpoat d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            tpoat d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            tpoat d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            tpoat d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            v0 = m21 * m10 - m20 * m11;
            v1 = m22 * m10 - m20 * m12;
            v2 = m23 * m10 - m20 * m13;
            v3 = m22 * m11 - m21 * m12;
            v4 = m23 * m11 - m21 * m13;
            v5 = m23 * m12 - m22 * m13;
            {
                tpoat d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
                tpoat d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
                tpoat d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
                tpoat d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

                n[0] =d00; n[1] =d10; n[2] =d20; n[3] =d30;
                n[4] =d01; n[5] =d11; n[6] =d21; n[7] =d31;
                n[8] =d02; n[9] =d12; n[10]=d22; n[11]=d32;
                n[12]=d03; n[13]=d13; n[14]=d23; n[15]=d33;
            }
        }
    }
    return 1;
}
void Teapot_Helper_InvertMatrixFast(tpoat* __restrict mOut16,const tpoat* __restrict m16)	{
    // It works only for translation + rotation, and only
    // when rotation can be represented by an unit quaternion
    // scaling is discarded
    const tpoat* m = m16;
    tpoat* n = mOut16;
    const tpoat T[3] = {-m[12],-m[13],-m[14]};
    tpoat w;
    // Step 1. Transpose the 3x3 submatrix
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step2. Adjust translation
    n[12]=T[0]*n[0] + T[1]*n[4] +T[2]*n[8];
    n[13]=T[0]*n[1] + T[1]*n[5] +T[2]*n[9];
    n[14]=T[0]*n[2] + T[1]*n[6] +T[2]*n[10];
    w    =T[0]*n[3] + T[1]*n[7] +T[2]*n[11];
    if (w!=0 && w!=1) {n[12]/=w;n[13]/=w;n[14]/=w;} // These last 2 lines are not strictly necessary AFAIK
}
void Teapot_Helper_GetFrustumPlaneEquations(tpoat planeEquationsOut[6][4],const tpoat* __restrict vpMatrix16,int normalizePlanes)   {
    // ax+by+cz+d=0 [xl,xr,yb,yt,zn,zf],normalizePlanes=0 -> no normalization
    tpoat m00 = vpMatrix16[0], m01 = vpMatrix16[4], m02 = vpMatrix16[8],  m03 = vpMatrix16[12];
    tpoat m10 = vpMatrix16[1], m11 = vpMatrix16[5], m12 = vpMatrix16[9],  m13 = vpMatrix16[13];
    tpoat m20 = vpMatrix16[2], m21 = vpMatrix16[6], m22 = vpMatrix16[10], m23 = vpMatrix16[14];
    tpoat m30 = vpMatrix16[3], m31 = vpMatrix16[7], m32 = vpMatrix16[11], m33 = vpMatrix16[15];
    tpoat* p = NULL;
    p = &planeEquationsOut[0][0];   // Left
    p[0] = m30+m00; p[1] = m31+m01; p[2] = m32+m02; p[3] = m33+m03;if (normalizePlanes) Teapot_Helper_NormalizePlane(p);
    p = &planeEquationsOut[1][0];   // Right
    p[0] = m30-m00; p[1] = m31-m01; p[2] = m32-m02; p[3] = m33-m03;if (normalizePlanes) Teapot_Helper_NormalizePlane(p);
    p = &planeEquationsOut[2][0];   // Bottom
    p[0] = m30+m10; p[1] = m31+m11; p[2] = m32+m12; p[3] = m33+m13;if (normalizePlanes) Teapot_Helper_NormalizePlane(p);
    p = &planeEquationsOut[3][0];   // Top
    p[0] = m30-m10; p[1] = m31-m11; p[2] = m32-m12; p[3] = m33-m13;if (normalizePlanes) Teapot_Helper_NormalizePlane(p);
    p = &planeEquationsOut[4][0];   // Near
    p[0] = m30+m20; p[1] = m31+m21; p[2] = m32+m22; p[3] = m33+m23;if (normalizePlanes) Teapot_Helper_NormalizePlane(p);
    p = &planeEquationsOut[5][0];   // Far
    p[0] = m30-m20; p[1] = m31-m21; p[2] = m32-m22; p[3] = m33-m23;if (normalizePlanes) Teapot_Helper_NormalizePlane(p);
}
void Teapot_Helper_GetFrustumPoints(tpoat frustumPoints[8][4],const tpoat* __restrict vpMatrixInverse16)    {
    const tpoat v[8][4] = {{-1, -1, -1, 1},{-1,  1, -1, 1},{ 1,  1, -1, 1},{ 1, -1, -1, 1},{-1, -1, 1, 1},{-1,  1, 1, 1},{ 1,  1, 1, 1},{ 1, -1, 1, 1}};
    int i;for (i = 0; i < 8; i++) {
        Teapot_Helper_MatrixMulPos(vpMatrixInverse16,frustumPoints[i],v[i][0],v[i][1],v[i][2]);
        frustumPoints[i][3]=1;
    }
}
static __inline void Teapot_Helper_GetFrustumAabbCenterAndHalfExtents(tpoat* __restrict frustumCenterOut3,tpoat* __restrict frustumHalfExtentsOut3,const tpoat frustumPoints[8][4])    {
    tpoat vmin[3] = {frustumPoints[0][0],frustumPoints[0][1],frustumPoints[0][2]};
    tpoat vmax[3] = {vmin[0],vmin[1],vmin[2]};
    int i,j;
    for (i = 1; i < 8; i++) {
        for (j = 0; j < 3; j++) {
            if      (vmin[j] > frustumPoints[i][j]) vmin[j] = frustumPoints[i][j];
            else if (vmax[j] < frustumPoints[i][j]) vmax[j] = frustumPoints[i][j];
        }
    }
    for (j = 0; j < 3; j++) {
        if (frustumCenterOut3)      frustumCenterOut3[j] = (vmin[j]+vmax[j])*0.5;
        if (frustumHalfExtentsOut3) frustumHalfExtentsOut3[j] = (vmax[j]-vmin[j])*0.5;
    }
}
// 'optionalPMatrixInverse16' is required only if you need to retrieve (one or more of) the arguments that follow it (otherwise their value is untouched).
void Teapot_Helper_GetLightViewProjectionMatrixExtra(tpoat* __restrict lvpMatrixOut16,
                                                          const tpoat* __restrict cameraVMatrixInverse16,
                                                          tpoat cameraNearClippingPlane,tpoat cameraFarClippingPlane,tpoat cameraFovyDeg,tpoat cameraAspectRatio,
                                                          const tpoat*  __restrict normalizedLightDirection3, tpoat texelIncrement
                                                          ,tpoat* __restrict optionalSphereCenterOut,tpoat* __restrict optionalSphereRadiiSquaredOut
                                                          ,const tpoat* __restrict optionalCameraPMatrixInverse16
                                                          ,tpoat* __restrict optionalLightViewportClippingOut4,tpoat optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,tpoat* __restrict optionalLVPMatrixForFrustumCullingUsageOut16   // Highly experimental and untested
                                                          )  {
    const tpoat cameraPosition3[3] = {cameraVMatrixInverse16[12],cameraVMatrixInverse16[13],cameraVMatrixInverse16[14]};
    const tpoat cameraForwardDirection3[3] = {-cameraVMatrixInverse16[8],-cameraVMatrixInverse16[9],-cameraVMatrixInverse16[10]};
    tpoat frustumCenter[3] = {0,0,0};tpoat radius = 0;
    tpoat lpMatrix[16],lvMatrix[16],lvpMatrixFallback[16];
    int i;

    // Get frustumCenter and radius
    tpoat frustumCenterDistance;
    tpoat tanFovDiagonalSquared = tan(cameraFovyDeg*3.14159265358979323846/360.0); // 0.5*M_PI/180.0
    const tpoat halfNearFarClippingPlane = 0.5*(cameraFarClippingPlane+cameraNearClippingPlane);
    if (lvpMatrixOut16==0) lvpMatrixOut16=lvpMatrixFallback;    // AFAIK from the caller point of view it's still lvpMatrixOut16==0, isn't it?
    tanFovDiagonalSquared*=tanFovDiagonalSquared;
    tanFovDiagonalSquared*=(1.0+cameraAspectRatio*cameraAspectRatio);
    frustumCenterDistance = halfNearFarClippingPlane*(1.0+tanFovDiagonalSquared);
    if (frustumCenterDistance > cameraFarClippingPlane) frustumCenterDistance = cameraFarClippingPlane;
    for (i=0;i<3;i++) frustumCenter[i] = cameraPosition3[i]+cameraForwardDirection3[i]*frustumCenterDistance;
    radius = (tanFovDiagonalSquared*cameraFarClippingPlane*cameraFarClippingPlane) + (cameraFarClippingPlane-frustumCenterDistance)*(cameraFarClippingPlane-frustumCenterDistance);
    if (optionalSphereCenterOut)        *optionalSphereCenterOut = frustumCenterDistance;
    if (optionalSphereRadiiSquaredOut)  *optionalSphereRadiiSquaredOut = radius;
    radius = sqrt(radius);
    //fprintf(stderr,"radius=%1.4f frustumCenterDistance=%1.4f nearPlane=%1.4f farPlane = %1.4f\n",radius,frustumCenterDistance,cameraNearClippingPlane,cameraFarClippingPlane);

    // For people trying to save texture space it's:  halfNearFarClippingPlane <= frustumCenterDistance <= cameraFarClippingPlane
    // When frustumCenterDistance == cameraFarClippingPlane, then frustumCenter is on the far clip plane (and half the texture space gets wasted).
    // when frustumCenterDistance == halfNearFarClippingPlane, then we're using an ortho projection matrix, and frustumCenter is in the middle of the near and far plane (no texture space gets wasted).
    // in all the other cases the space wasted can go from zero to half texture

    // Shadow swimming happens when: 1) camera translates; 2) camera rotates; 3) objects move or rotate
    // AFAIK Shadow swimming (3) can't be fixed in any way
    if (texelIncrement>0)   radius = ceil(radius/texelIncrement)*texelIncrement;      // This 'should' fix Shadow swimming (1)  [Not sure code is correct!]

    // Get light matrices
    Teapot_Helper_Ortho(lpMatrix,-radius,radius,-radius,radius,0,-2.0*radius);
    Teapot_Helper_LookAt(lvMatrix,
            frustumCenter[0]-normalizedLightDirection3[0]*radius,   // eye[0]
            frustumCenter[1]-normalizedLightDirection3[1]*radius,   // eye[1]
            frustumCenter[2]-normalizedLightDirection3[2]*radius,   // eye[2]
            frustumCenter[0],frustumCenter[1],frustumCenter[2],     // target
            0,1,0                                                   // up (people that cares about wasted texture space can probably change it)
            );
    // Get output
    Teapot_Helper_MultMatrix(lvpMatrixOut16,lpMatrix,lvMatrix);

    // This 'should' fix Shadow swimming (2) [Not sure code is correct!]
    if (texelIncrement>0)   {
        tpoat shadowOrigin[4]   = {0,0,0,1};
        tpoat roundedOrigin[4]  = {0,0,0,0};
        tpoat roundOffset[4]    = {0,0,0,0};
        tpoat texelCoefficient = texelIncrement*2.0;
        Teapot_Helper_MatrixMulPos(lvpMatrixOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
        for (i = 0; i < 2; i++) {// Or i<3 ?
            shadowOrigin[i]/= texelCoefficient;
            roundedOrigin[i] = Teapot_Helper_Round(shadowOrigin[i]);
            roundOffset[i] = roundedOrigin[i] - shadowOrigin[i];
            roundOffset[i]*=  texelCoefficient;
        }
        lvpMatrixOut16[12]+= roundOffset[0];
        lvpMatrixOut16[13]+= roundOffset[1];
    }

    // Debug stuff
    //fprintf(stderr,"radius=%1.5f frustumCenter={%1.5f,%1.5f,%1.5f}\n",radius,frustumCenter[0],frustumCenter[1],frustumCenter[2]);

    // Extra stuff [Not sure code is correct: the returned viewport seems too big!]
    if (optionalCameraPMatrixInverse16) {
        int j;
        tpoat cameraVPMatrixInv[16],cameraVPMatrixInverseAdjusted[16];tpoat frustumPoints[8][4];
        tpoat minVal[3],maxVal[3],tmp;
        Teapot_Helper_MultMatrix(cameraVPMatrixInv,cameraVMatrixInverse16,optionalCameraPMatrixInverse16); // vMatrixInverse16 needs an expensive Helper_InvertMatrix(...) to be calculated. Here we can exploit the property of the product of 2 invertse matrices.
        // If we call Teapot_Helper_GetFrustumPoints(frustumPoints,cameraVPMatrixInv) we find the frustum corners in world space

        Teapot_Helper_MultMatrix(cameraVPMatrixInverseAdjusted,lvpMatrixOut16,cameraVPMatrixInv);  // This way we 'should' get all points in the [-1,1] light NDC space (or not?)

        Teapot_Helper_GetFrustumPoints(frustumPoints,cameraVPMatrixInverseAdjusted);

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
            optionalLightViewportClippingOut4[0] = minVal[0]*0.5+0.5;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[1] = minVal[1]*0.5+0.5;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[2] = (maxVal[0]-minVal[0])*0.5;    // extent x in [0,1]
            optionalLightViewportClippingOut4[3] = (maxVal[1]-minVal[1])*0.5;    // extent y in [0,1]

            for (i=0;i<4;i++)   {
               optionalLightViewportClippingOut4[i]/=texelIncrement;    // viewport is in [0,texture_size]
            }

            /*optionalLightViewportClippingOut4[0] = floor(optionalLightViewportClippingOut4[0]);
            optionalLightViewportClippingOut4[1] = floor(optionalLightViewportClippingOut4[1]);
            optionalLightViewportClippingOut4[2] = ceil(optionalLightViewportClippingOut4[2]);
            optionalLightViewportClippingOut4[3] = ceil(optionalLightViewportClippingOut4[3]);*/

            //fprintf(stderr,"viewport={%1.4f,%1.4f,%1.4f,%1.4f}\n",optionalLightViewportClippingOut4[0],optionalLightViewportClippingOut4[1],optionalLightViewportClippingOut4[2],optionalLightViewportClippingOut4[3]);
        }

        if (optionalLVPMatrixForFrustumCullingUsageOut16)   {
            const int attemptToFixSwimming = (lvpMatrixOut16==lvpMatrixFallback) ? 1 : 0;   // Only if we don't want lvpMatrixOut16
            float minmaxXY[4]={minVal[0]*radius,maxVal[0]*radius,minVal[1]*radius,maxVal[1]*radius};
            if (attemptToFixSwimming && texelIncrement>0)   {
                for (i=0;i<4;i++) {
                    // This 'should' fix Shadow swimming (1) in the 'Stable Shadow Mapping Technique'
                    // Not sure it works here too...
                    if (minmaxXY[i]>=0) minmaxXY[i] = ceil(minmaxXY[i]/texelIncrement)*texelIncrement;
                    else                minmaxXY[i] = -ceil(-minmaxXY[i]/texelIncrement)*texelIncrement;
                }
            }
            Teapot_Helper_Ortho(optionalLVPMatrixForFrustumCullingUsageOut16,
                         minmaxXY[0],minmaxXY[1],
                         minmaxXY[2],minmaxXY[3],
                         0,-2.0*radius                      // For z, we just copy Helper_Ortho(lpMatrix,...)
                         );
            Teapot_Helper_MultMatrix(optionalLVPMatrixForFrustumCullingUsageOut16,optionalLVPMatrixForFrustumCullingUsageOut16,lvMatrix);
            // This 'should' fix Shadow swimming (2) in the 'Stable Shadow Mapping Technique'
            // Not here, because the shadow viewport stretches when the camera rotates
            // We try anyway...
            if (attemptToFixSwimming && texelIncrement>0)   {
                tpoat shadowOrigin[4]   = {0,0,0,1};
                tpoat roundedOrigin[4]  = {0,0,0,0};
                tpoat roundOffset[4]    = {0,0,0,0};
                tpoat texelCoefficient = texelIncrement*2.0;
                Teapot_Helper_MatrixMulPos(optionalLVPMatrixForFrustumCullingUsageOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
                for (i = 0; i < 2; i++) {// Or i<3 ?
                    shadowOrigin[i]/= texelCoefficient;
                    roundedOrigin[i] = Teapot_Helper_Round(shadowOrigin[i]);
                    roundOffset[i] = roundedOrigin[i] - shadowOrigin[i];
                    roundOffset[i]*=  texelCoefficient;
                }
                optionalLVPMatrixForFrustumCullingUsageOut16[12]+= roundOffset[0];
                optionalLVPMatrixForFrustumCullingUsageOut16[13]+= roundOffset[1];
            }
        }
    }

}






static __inline void Teapot_Helper_Min3(tpoat* __restrict res3,const tpoat* a3,const tpoat* b3) {
    int i;for (i=0;i<3;i++) res3[i]=a3[i]<b3[i]?a3[i]:b3[i];
}
static __inline void Teapot_Helper_Max3(tpoat* __restrict res3,const tpoat* a3,const tpoat* b3) {
    int i;for (i=0;i<3;i++) res3[i]=a3[i]>b3[i]?a3[i]:b3[i];
}
int Teapot_Helper_IsVisible(const tpoat frustumPlanes[6][4],const tpoat*__restrict mfMatrix16,tpoat aabbMinX,tpoat aabbMinY,tpoat aabbMinZ,tpoat aabbMaxX,tpoat aabbMaxY,tpoat aabbMaxZ) {
    // It "should" performs AABB test. mfMatrix16 is the matrix M so that: F*M = mvpMatrix (F being the matrix used to extract the frustum planes). Here we use: F=pMatrix and M=mvMatrix, but it could be: F=vpMatrix and M=mMatrix too.
    int i;
    // AABB transformation by: http://dev.theomader.com/transform-bounding-boxes/
    const tpoat* m = mfMatrix16;
    const tpoat a[9] = {m[0]*aabbMinX,m[1]*aabbMinX,    m[2]*aabbMinX, m[4]*aabbMinY,m[5]*aabbMinY,m[6]*aabbMinY,   m[8]*aabbMinZ,m[9]*aabbMinZ,m[10]*aabbMinZ};
    const tpoat b[9] = {m[0]*aabbMaxX,m[1]*aabbMaxX,    m[2]*aabbMaxX, m[4]*aabbMaxY,m[5]*aabbMaxY,m[6]*aabbMaxY,   m[8]*aabbMaxZ,m[9]*aabbMaxZ,m[10]*aabbMaxZ};
    tpoat buf[18];
    Teapot_Helper_Min3(&buf[0], &a[0],&b[0]);
    Teapot_Helper_Min3(&buf[3], &a[3],&b[3]);
    Teapot_Helper_Min3(&buf[6], &a[6],&b[6]);
    Teapot_Helper_Max3(&buf[9], &a[0],&b[0]);
    Teapot_Helper_Max3(&buf[12],&a[3],&b[3]);
    Teapot_Helper_Max3(&buf[15],&a[6],&b[6]);
    {
        const tpoat aabb[6] = {
            buf[0]+buf[ 3]+buf[ 6]+m[12], buf[ 1]+buf[ 4]+buf[ 7]+m[13], buf[ 2]+buf[ 5]+buf[ 8]+m[14],
            buf[9]+buf[12]+buf[15]+m[12], buf[10]+buf[13]+buf[16]+m[13], buf[11]+buf[14]+buf[17]+m[14]
        };

        // End AABB transformation

        // FAST VERSION
        {
            for(i=0; i < 6; i++) {
                const tpoat *pl = &frustumPlanes[i][0];
                const int p[3] = {3*(int)(pl[0]>0.0f),3*(int)(pl[1]>0.0f),3*(int)(pl[2]>0.0f)};   // p[j] = 0 or 3
                const tpoat dp = pl[0]*aabb[p[0]] + pl[1]*aabb[p[1]+1] + pl[2]*aabb[p[2]+2] + pl[3];
                if (dp < 0) return 0;
            }
        }

        // MUCH SLOWER VERSION
        /*{
        const tpoat aabb0=aabb[0],aabb1=aabb[1],aabb2=aabb[2];
        const tpoat aabb3=aabb[3],aabb4=aabb[4],aabb5=aabb[5];
        for(i=0; i < 6; i++)    {
            int sum = 0;
            const tpoat *pl = &frustumPlanes[i][0];
            const tpoat plx=pl[0],ply=pl[1],plz=pl[2],plw=pl[3];
            sum += (plx*aabb0+ply*aabb1+plz*aabb2+plw)<0?1:0;
            sum += (plx*aabb3+ply*aabb1+plz*aabb2+plw)<0?1:0;
            sum += (plx*aabb0+ply*aabb4+plz*aabb2+plw)<0?1:0;
            sum += (plx*aabb3+ply*aabb4+plz*aabb2+plw)<0?1:0;
            sum += (plx*aabb0+ply*aabb1+plz*aabb5+plw)<0?1:0;
            sum += (plx*aabb3+ply*aabb1+plz*aabb5+plw)<0?1:0;
            sum += (plx*aabb0+ply*aabb4+plz*aabb5+plw)<0?1:0;
            sum += (plx*aabb3+ply*aabb4+plz*aabb5+plw)<0?1:0;
            if (sum==8) return 0;
        }
    }*/

        // Furthermore we still have a lot of false positives in both cases
    }
    return 1;
}
int Teapot_Helper_UnProject_MvpMatrixInv(tpoat winX,tpoat winY,tpoat winZ,const tpoat* __restrict mvpMatrixInv16,const int* viewport4,tpoat* objX,tpoat* objY,tpoat* objZ)    {
    // To compute the coordinates objX objY objZ , gluUnProject multiplies the normalized device coordinates by the inverse of model * proj as follows:
    // [objX objY objZ W] = INV(P*M) * ⁢ [2*(winX-view[0])/view[2]-1    2*(winY-view[1])/view[3]-1  2*winZ-1    1]
    // INV denotes matrix inversion. W is an unused variable, included for consistent matrix notation.
   const tpoat *invpm = mvpMatrixInv16;
   // normalized device coords
   const tpoat v[4] = {
      2*(winX-(tpoat)viewport4[0])/(tpoat)viewport4[2]-1,
      2*(winY-(tpoat)viewport4[1])/(tpoat)viewport4[3]-1,
      2*winZ-1,
      1
   };
   tpoat objW = 0;
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
Teapot_MeshData* Teapot_MeshData_GetMeshUnderMouse(Teapot_MeshData* const* meshes,int numMeshes,int mouseX,int mouseY,const int* viewport4,tpoat* pOptionalDistanceOut) {
    Teapot_MeshData* rv = 0;
    tpoat vpMatrixInv[16];
    tpoat rayOrigin[3] = {0,0,0};
    tpoat rayDir[3] = {0,0,-1};
    tpoat intersection_distance = 0;
    int i,j;
    if (pOptionalDistanceOut) *pOptionalDistanceOut=intersection_distance;
    if (!meshes || numMeshes<=0 || !viewport4) return rv;

    // 1) Find rayOrigin and rayDir (world space)
    Teapot_Helper_MultMatrix(vpMatrixInv,TIS.pMatrix,TIS.vMatrix);
    Teapot_Helper_InvertMatrix(vpMatrixInv,vpMatrixInv);
    Teapot_Helper_UnProject_MvpMatrixInv(mouseX,viewport4[3]-mouseY-1,0.0,vpMatrixInv,viewport4,&rayOrigin[0],&rayOrigin[1],&rayOrigin[2]);
    Teapot_Helper_UnProject_MvpMatrixInv(mouseX,viewport4[3]-mouseY-1,1.0,vpMatrixInv,viewport4,&rayDir[0],&rayDir[1],&rayDir[2]);
    for (i=0;i<3;i++) rayDir[i]-=rayOrigin[i];
    Teapot_Helper_Vector3Normalize(rayDir);
    //printf("rayOrigin={%1.2f,%1.2f,%1.2f} rayDir={%1.2f,%1.2f,%1.2f}\n",rayOrigin[0],rayOrigin[1],rayOrigin[2],rayDir[0],rayDir[1],rayDir[2]);

    // 2) Loop all meshes and find OBB vs ray intersection
    // Code based on: http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/ (WTFPL Public Licence)
    for (i=0;i<numMeshes;i++)   {
        const Teapot_MeshData* md = meshes[i];
        const tpoat* obbMatrix = md->mMatrix;
        const TeapotMeshEnum meshId = md->meshId;
        tpoat tMin = 0;tpoat tMax = 1000000000000;
        int noCollisionDetected = 0;
        tpoat obbPosDelta[3] =  {obbMatrix[12]-rayOrigin[0],obbMatrix[13]-rayOrigin[1],obbMatrix[14]-rayOrigin[2]};        
        //const float scaling[3] = {meshId==TEAPOT_MESH_CAPSULE ? ((md->scaling[0]+md->scaling[2])*0.5) : md->scaling[0],md->scaling[1],meshId==TEAPOT_MESH_CAPSULE ? ((md->scaling[0]+md->scaling[2])*0.5) : md->scaling[2]};
        const float* scaling = md->scaling;
        tpoat aabbMin[3] = {TIS.aabbMin[meshId][0]*scaling[0],TIS.aabbMin[meshId][1]*scaling[1],TIS.aabbMin[meshId][2]*scaling[2]};
        tpoat aabbMax[3] = {TIS.aabbMax[meshId][0]*scaling[0],TIS.aabbMax[meshId][1]*scaling[1],TIS.aabbMax[meshId][2]*scaling[2]};
        //Teapot_Helper_Vector3Normalize(obbPosDelta);
        //for (j=0;j<3;j++) obbPosDelta[i]=-obbPosDelta[i];

        if (md->meshId==TEAPOT_MESH_CAPSULE) {
            // Sorry, but capsules are special
            const float sphereScaling = (md->scaling[0]+md->scaling[2])*0.5;
#           ifndef TEAPOT_CENTER_MESHES_ON_FLOOR
            aabbMin[1]+=0.5*(md->scaling[1]-2.0*sphereScaling);
            aabbMax[1]-=0.5*(md->scaling[1]-2.0*sphereScaling);
#           else //TEAPOT_CENTER_MESHES_ON_FLOOR
            aabbMax[1]-=0.5*(md->scaling[1]-2.0*sphereScaling);
#           endif //TEAPOT_CENTER_MESHES_ON_FLOOR
            //printf("TEAPOT_MESH_CAPSULE: aabbMin(%1.3f,%1.3f,%1.3f) aabbMax(%1.3f,%1.3f,%1.3f) md->scaling[1]=%1.3f sphereScaling=%1.3f TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_DOWN][1]=%1.3f\n",aabbMin[0],aabbMin[1],aabbMin[2],aabbMax[0],aabbMax[1],aabbMax[2],md->scaling[1],sphereScaling,TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_DOWN][1]);
        }

        for (j=0;j<3;j++)   {
            if (!noCollisionDetected)   {
                int j4 = 4*j;
                // Test intersection with the 2 planes perpendicular to the OBB's j axis
                const tpoat axis[3] = {obbMatrix[j4],obbMatrix[j4+1],obbMatrix[j4+2]};
                const tpoat e = Teapot_Helper_Vector3Dot(axis, obbPosDelta);
                const tpoat f = Teapot_Helper_Vector3Dot(rayDir, axis);

                //if ( abs(f) > 0.001)  // @Flix: the reference code used this (but it does not work for me; so maybe my selection does not work with a projection ortho matrix...)
                {
                    // Standard case
                    // t1 and t2 now contain distances betwen ray origin and ray-plane intersections:
                    tpoat t1 = (e+aabbMin[j])/f; // Intersection with the "left" plane
                    tpoat t2 = (e+aabbMax[j])/f; // Intersection with the "right" plane
                    // We want t1 to represent the nearest intersection, so if it's not the case, invert t1 and t2
                    if (t1>t2)  {tpoat w=t1;t1=t2;t2=w;}
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
            rv = (Teapot_MeshData*) md;
        }
    }

    if (pOptionalDistanceOut) *pOptionalDistanceOut=intersection_distance;
    return rv;
}


void Teapot_SetViewMatrixAndLightDirection(const tpoat vMatrix[16],tpoat lightDirectionWorldSpace[3])   {
    tpoat len = lightDirectionWorldSpace[0]*lightDirectionWorldSpace[0]+lightDirectionWorldSpace[1]*lightDirectionWorldSpace[1]+lightDirectionWorldSpace[2]*lightDirectionWorldSpace[2];
    if (len<0.999f || len>1.001f) {len = sqrt(len);lightDirectionWorldSpace[0]/=len;lightDirectionWorldSpace[1]/=len;lightDirectionWorldSpace[2]/=len;}
    TIS.lightDirectionWorldSpace[0] = lightDirectionWorldSpace[0];
    TIS.lightDirectionWorldSpace[1] = lightDirectionWorldSpace[1];
    TIS.lightDirectionWorldSpace[2] = lightDirectionWorldSpace[2];

    // lightDirectionViewSpace = v3_norm(m4_mul_dir(vMatrix,light_direction));
    Teapot_Helper_CopyMatrix(TIS.vMatrix,vMatrix);
#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    Teapot_Helper_InvertMatrixFast(TIS.vMatrixInverse,TIS.vMatrix);
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP*/
    //int i;for (i=0;i<16;i++) TIS.vMatrix[i]=vMatrix[i];	// what's faster ?
    TIS.lightDirectionViewSpace[0] = lightDirectionWorldSpace[0]*TIS.vMatrix[0] + lightDirectionWorldSpace[1]*TIS.vMatrix[4] + lightDirectionWorldSpace[2]*TIS.vMatrix[8];
    TIS.lightDirectionViewSpace[1] = lightDirectionWorldSpace[0]*TIS.vMatrix[1] + lightDirectionWorldSpace[1]*TIS.vMatrix[5] + lightDirectionWorldSpace[2]*TIS.vMatrix[9];
    TIS.lightDirectionViewSpace[2] = lightDirectionWorldSpace[0]*TIS.vMatrix[2] + lightDirectionWorldSpace[1]*TIS.vMatrix[6] + lightDirectionWorldSpace[2]*TIS.vMatrix[10];

    glUseProgram(TIS.programId);
    Teapot_Helper_GlUniform3v(TIS.uLoc_lightVector,1,TIS.lightDirectionViewSpace);
    glUseProgram(0);
}
void Teapot_SetProjectionMatrix(const tpoat pMatrix[16])    {
    Teapot_Helper_CopyMatrix(TIS.pMatrix,pMatrix);
#   ifdef TEAPOT_ENABLE_FRUSTUM_CULLING
    Teapot_Helper_GetFrustumPlaneEquations(TIS.pMatrixFrustum,TIS.pMatrix,0);   // Last arg can probably be 0...
#   endif
    glUseProgram(TIS.programId);
    Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_pMatrix, 1 /*only setting 1 matrix*/, GL_FALSE /*transpose?*/, pMatrix);
    glUseProgram(0);
}

void Teapot_GetViewMatrix(tpoat* res16) {Teapot_Helper_CopyMatrix(res16,TIS.vMatrix);}
const tpoat* Teapot_GetViewMatrixConstReference() {return TIS.vMatrix;}
void Teapot_GetNormalizedLightDirection(tpoat* res3)    {int i;for (i=0;i<3;i++) res3[i]=TIS.lightDirectionWorldSpace[i];}
const tpoat* Teapot_GetNormalizedLightDirectionConstReference() {return TIS.lightDirectionWorldSpace;}
void Teapot_GetNormalizedLightDirectionInViewSpace(tpoat* res3)     {int i;for (i=0;i<3;i++) res3[i]=TIS.lightDirectionViewSpace[i];}
const tpoat* Teapot_GetNormalizedLightDirectionInViewSpaceConstReference() {return TIS.lightDirectionViewSpace;}

#ifdef TEAPOT_SHADER_USE_SHADOW_MAP
void Teapot_SetShadowVpMatrix(const tpoat unbiasedShadowVpMatrix[16])    {
    static tpoat bias[16] = {0.5,0,0,0, 0,0.5,0,0,  0,0,0.5,0,    0.5,0.5,0.5,1}; // Moving from unit cube [-1,1] to [0,1]

    Teapot_Helper_MultMatrix(TIS.biasedShadowVpMatrix,bias,unbiasedShadowVpMatrix);
    Teapot_Helper_MultMatrix(TIS.biasedShadowVpMatrix,TIS.biasedShadowVpMatrix,TIS.vMatrixInverse);
}
void Teapot_GetViewMatrixInverse(tpoat* res16)  {Teapot_Helper_CopyMatrix(res16,TIS.vMatrixInverse);}
const tpoat* Teapot_GetViewMatrixInverseConstReference() {return TIS.vMatrixInverse;}
void Teapot_SetShadowDarkening(float darkeningFactorIn_0_80,float shadowMinIntensityIn_0_1)    {
    TIS.shadowDarkening = darkeningFactorIn_0_80; TIS.shadowClamp = shadowMinIntensityIn_0_1;
#   if TEAPOT_SHADER_SHADOW_MAP_PCF>0
    TIS.shadowDarkening = TIS.shadowDarkening==0.0 ? 0.0 : (0.1/TIS.shadowDarkening);
#   endif //TEAPOT_SHADER_SHADOW_MAP_PCF>0
    glUseProgram(TIS.programId);
    glUniform2f(TIS.uLoc_shadowDarkening,TIS.shadowDarkening,TIS.shadowClamp);
    glUseProgram(0);
}
void Teapot_SetShadowMapFactor(float shadowMapResolutionFactorIn_0_1)    {
    glUseProgram(TIS.programId);
    glUniform1f(TIS.uLoc_shadowMapFactor,shadowMapResolutionFactorIn_0_1);
    glUseProgram(0);
}
void Teapot_SetShadowMapTexelIncrement(float shadowMapTexelIncrementX,float shadowMapTexelIncrementY)    {
    glUseProgram(TIS.programId);
    glUniform2f(TIS.uLoc_shadowMapTexelIncrement,shadowMapTexelIncrementX,shadowMapTexelIncrementY);
    glUseProgram(0);
}
#endif //TEAPOT_SHADER_USE_SHADOW_MAP

#ifdef TEAPOT_SHADER_FOG
void Teapot_SetFogColor(float R, float G, float B)  {
    glUseProgram(TIS.programId);
    glUniform3f(TIS.uLoc_fogColor,R,G,B);
    glUseProgram(0);
}
void Teapot_SetFogDistances(float startDistance,float endDistance)  {
    glUseProgram(TIS.programId);
    glUniform4f(TIS.uLoc_fogDistances,startDistance,endDistance,endDistance-startDistance,1.0/(endDistance-startDistance));
    glUseProgram(0);
}
#endif //TEAPOT_SHADER_FOG

void Teapot_LowLevel_EnableVertexAttributes(int enableVertexAttribArray,int enableNormalAttribArray) {
    if (enableVertexAttribArray) glEnableVertexAttribArray(TIS.aLoc_vertex);
    if (enableNormalAttribArray) glEnableVertexAttribArray(TIS.aLoc_normal);
}
void Teapot_LowLevel_BindVertexBufferObject(void) {
    glBindBuffer(GL_ARRAY_BUFFER, TIS.vertexBuffer);
    glVertexAttribPointer(TIS.aLoc_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0);
    glVertexAttribPointer(TIS.aLoc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TIS.elementBuffer);
}
void Teapot_LowLevel_BindShaderProgram(void) {
    glUseProgram(TIS.programId);
}
void Teapot_LowLevel_UnbindShaderProgram(void) {
    glUseProgram(0);
}
void Teapot_LowLevel_UnbindVertexBufferObject(void) {
    glBindBuffer(GL_ARRAY_BUFFER,0);
}
void Teapot_LowLevel_DisableVertexAttributes(int disableVertexAttribArray,int disableNormalAttribArray) {
    if (disableVertexAttribArray) glDisableVertexAttribArray(TIS.aLoc_vertex);
    if (disableNormalAttribArray) glDisableVertexAttribArray(TIS.aLoc_normal);
}

void Teapot_PreDraw(void)   {
    if (TIS.programId)  {
        glEnableVertexAttribArray(TIS.aLoc_vertex);
        glEnableVertexAttribArray(TIS.aLoc_normal);
        glBindBuffer(GL_ARRAY_BUFFER, TIS.vertexBuffer);
        glVertexAttribPointer(TIS.aLoc_vertex, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, 0);
        glVertexAttribPointer(TIS.aLoc_normal, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(sizeof(float)*3));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TIS.elementBuffer);

        glUseProgram(TIS.programId);
    }
}

void Teapot_SetScaling(float scalingX, float scalingY, float scalingZ)  {
    TIS.scaling[0]=scalingX;TIS.scaling[1]=scalingY;TIS.scaling[2]=scalingZ;
    glUniform4f(TIS.uLoc_scaling,scalingX,scalingY,scalingZ,1.f);
}
void Teapot_SetColor(float R,float G,float B,float A)  {
    TIS.color[0]=R;TIS.color[1]=G;TIS.color[2]=B;TIS.color[3]=A;
    glUniform4fv(TIS.uLoc_color,1,TIS.color);
    if (TIS.colorMaterialEnabled) {
        const float ambFac = 0.25f;
        Teapot_SetColorAmbient(R*ambFac,G*ambFac,B*ambFac);
#       ifdef TEAPOT_SHADER_SPECULAR
        {
            const float speFac = 0.8f * A;
            Teapot_SetColorSpecular(R*speFac,G*speFac,B*speFac,-1.f*A);
        }
#       endif //TEAPOT_SHADER_SPECULAR
    }
}
void Teapot_SetColorAmbient(float R,float G,float B)  {
    TIS.colorAmbient[0]=R;TIS.colorAmbient[1]=G;TIS.colorAmbient[2]=B;
    glUniform4fv(TIS.uLoc_colorAmbient,1,TIS.colorAmbient);
}
#ifdef TEAPOT_SHADER_SPECULAR
void Teapot_SetColorSpecular(float R,float G,float B,float SHI) {
    TIS.colorSpecular[0]=R;TIS.colorSpecular[1]=G;TIS.colorSpecular[2]=B;
    if (SHI>0) TIS.colorSpecular[3]=SHI;
    glUniform4f(TIS.uLoc_colorSpecular,R,G,B,TIS.colorSpecular[3]);
}
#endif //TEAPOT_SHADER_SPECULAR

void Teapot_Enable_ColorMaterial(void) {TIS.colorMaterialEnabled = 1;}
void Teapot_Disable_ColorMaterial(void) {TIS.colorMaterialEnabled = 0;}
int Teapot_Get_ColorMaterial_Enabled(void) {return TIS.colorMaterialEnabled;}

void Teapot_Enable_MeshOutline(void) {TIS.meshOutlineEnabled = 1;}
void Teapot_Disable_MeshOutline(void) {TIS.meshOutlineEnabled = 0;}
int Teapot_Get_MeshOutline_Enabled(void) {return TIS.meshOutlineEnabled;}
void Teapot_Set_MeshOutline_Color(float R,float G, float B, float A)	{
    TIS.colorMeshOutline[0]=R;TIS.colorMeshOutline[1]=G;TIS.colorMeshOutline[2]=B;
    if (A>=0) TIS.colorMeshOutline[3]=A;
}
void Teapot_Set_MeshOutline_Scaling(float scalingGreaterOrEqualThanOne) {
    TIS.scalingMeshOutline = scalingGreaterOrEqualThanOne;
}
void Teapot_Set_MeshOutline_Params(float polygonOffsetSlope,float polygonOffsetConstant)    {
    TIS.polygonOffsetSlope = polygonOffsetSlope;
    TIS.polygonOffsetConstant = polygonOffsetConstant;
}


void Teapot_Draw_Mv(const tpoat mvMatrix[16], TeapotMeshEnum meshId)    {
    if (meshId==TEAPOT_MESH_COUNT) return;
    else if (meshId == TEAPOT_MESH_CAPSULE)  {
        // We don't want to draw "scaled" capsules. Instead we want to regenerate valid capsules, reinterpreting Teapot_SetScaling(...)
#       if (!defined(TEAPOT_NO_MESH_CYLINDER) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_UP) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_DOWN))
        const tpoat height = TIS.scaling[1];
        const tpoat diameter = (TIS.scaling[0]+TIS.scaling[2])*0.5f;
        const float pushScaling[3] = {TIS.scaling[0],TIS.scaling[1],TIS.scaling[2]};
        const tpoat yAxis[3] = {mvMatrix[4],mvMatrix[5],mvMatrix[6]};
        const tpoat centerY = -TIS.centerPoint[TEAPOT_MESH_CYLINDER_LATERAL_SURFACE][1];
        tpoat tmp = diameter*centerY;
        const tpoat origin[3] = {mvMatrix[12]-mvMatrix[4]*tmp,mvMatrix[13]-mvMatrix[5]*tmp,mvMatrix[14]-mvMatrix[6]*tmp};
        tpoat mat[16];Teapot_Helper_CopyMatrix(mat,mvMatrix);
        mat[12] = origin[0];    mat[13] = origin[1];    mat[14] = origin[2];

        // Tip: it should be better to cull TEAPOT_MESH_CAPSULE here
        // and skip its child shapes in the frustum culling code below

        // TEAPOT_MESH_CYLINDER_LATERAL_SURFACE
        Teapot_SetScaling(diameter,height,diameter);
        Teapot_Draw_Mv(mat,TEAPOT_MESH_CYLINDER_LATERAL_SURFACE);

        // TEAPOT_MESH_HALF_SPHERE_UP and TEAPOT_MESH_HALF_SPHERE_DOWN are not affected by TEAPOT_CENTER_MESHES_ON_FLOOR
        Teapot_SetScaling(diameter,diameter,diameter);
        tmp = height*(0.5f-centerY);
        mat[12] = origin[0] + yAxis[0]*tmp;
        mat[13] = origin[1] + yAxis[1]*tmp;
        mat[14] = origin[2] + yAxis[2]*tmp;
        Teapot_Draw_Mv(mat,TEAPOT_MESH_HALF_SPHERE_UP);

        tmp = -height*(0.5f+centerY);
        mat[12] = origin[0] + yAxis[0]*tmp;
        mat[13] = origin[1] + yAxis[1]*tmp;
        mat[14] = origin[2] + yAxis[2]*tmp;
        Teapot_Draw_Mv(mat,TEAPOT_MESH_HALF_SPHERE_DOWN);

        Teapot_SetScaling(pushScaling[0],pushScaling[1],pushScaling[2]);
#       endif // !defined(...)
        return;
    }

#   ifdef TEAPOT_ENABLE_FRUSTUM_CULLING
    if (meshId<TEAPOT_MESH_TEXT_X || meshId>TEAPOT_MESH_TEXT_Z) {
        const float scaling[3] = {TIS.scaling[0],TIS.scaling[1],TIS.scaling[2]};
        const float aabbMin[3] = {TIS.aabbMin[meshId][0]*scaling[0],TIS.aabbMin[meshId][1]*scaling[1],TIS.aabbMin[meshId][2]*scaling[2]};
        const float aabbMax[3] = {TIS.aabbMax[meshId][0]*scaling[0],TIS.aabbMax[meshId][1]*scaling[1],TIS.aabbMax[meshId][2]*scaling[2]};
        //tpoat matrix[16];Teapot_Helper_InvertMatrixFast(matrix,mvMatrix);

        if (!Teapot_Helper_IsVisible(TIS.pMatrixFrustum,
                                     mvMatrix,
                                     aabbMin[0],aabbMin[1],aabbMin[2],
                                     aabbMax[0],aabbMax[1],aabbMax[2]))
                                     {
            //fprintf(stderr,"MeshId=%d\n",meshId);
            return;
        }
    }
#   endif //TEAPOT_ENABLE_FRUSTUM_CULLING

#   ifdef TEAPOT_SHADER_USE_NORMAL_MATRIX
    {
        const tpoat* m = mvMatrix;
        tpoat nMatrix3x3Out[9];
        tpoat* n = nMatrix3x3Out;
        // We need to invert mvMatrix[9] before and then trnspose it on the fly--------
        // Create shorthands to access matrix members
        tpoat m00 = m[0],  m10 = m[1],  m20 = m[2];
        tpoat m01 = m[4],  m11 = m[5],  m21 = m[6];
        tpoat m02 = m[8],  m12 = m[9],  m22 = m[10];
        /*tpoat m00 = m[0],  m10 = m[4],  m20 = m[8];
        tpoat m01 = m[1],  m11 = m[5],  m21 = m[9];
        tpoat m02 = m[2],  m12 = m[6],  m22 = m[10];*/

#       ifndef TEAPOT_SHADER_USE_NORMAL_MATRIX_NO_OPTIMIZE
        // Optional optimization:
        const tpoat sqcol0 = m00*m00 + m10*m10 + m20*m20;
        const tpoat sqcol1 = m01*m01 + m11*m11 + m21*m21;
        const tpoat sqcol2 = m02*m02 + m12*m12 + m22*m22;
        const tpoat eps = 0.0001;
        if (fabs(sqcol0-sqcol1)<eps && fabs(sqcol0-sqcol2)<eps && fabs(sqcol1-sqcol2)<eps)  {
            //save a lot of expesive calculations and just set nMatrix to mvMatrix.3x3 (will be renormalized in the shader anyway)
            n[0]=m00; n[1]=m10; n[2]=m20;
            n[3]=m01; n[4]=m11; n[5]=m21;
            n[6]=m02; n[7]=m12; n[8]=m22;
        }
        else
#       endif //TEAPOT_SHADER_USE_NORMAL_MATRIX_NO_OPTIMIZE
        {
            // Calculate cofactor matrix
            tpoat c00 =   m11*m22 - m12*m21,   c10 = -(m01*m22 - m02*m21),  c20 =   m01*m12 - m02*m11;
            tpoat c01 = -(m10*m22 - m12*m20),  c11 =   m00*m22 - m02*m20,   c21 = -(m00*m12 - m02*m10);
            tpoat c02 =   m10*m21 - m11*m20,   c12 = -(m00*m21 - m01*m20),  c22 =   m00*m11 - m01*m10;

            tpoat i00,i10,i20, i01,i11,i21, i02,i12,i22;

            // Caclculate the determinant by using the already calculated determinants
            // in the cofactor matrix.
            // Second sign is already minus from the cofactor matrix.
            tpoat det = m00*c00 + m10*c10 + m20 * c20;
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
            /*n[0] = i00; n[3] = i01; n[6] = i02;
            n[1] = i10; n[4] = i11; n[7] = i12;
            n[2] = i20; n[5] = i21; n[8] = i22;*/
        }
        Teapot_Helper_GlUniformMatrix3v(TIS.uLoc_nMatrix, 1 /*only setting 1 matrix*/, GL_FALSE /*transpose?*/, nMatrix3x3Out);
    }
#   endif //TEAPOT_SHADER_USE_NORMAL_MATRIX


#   ifdef TEAPOT_SHADER_USE_SHADOW_MAP
    {
    tpoat tmp[16];
    Teapot_Helper_MultMatrix(tmp,TIS.biasedShadowVpMatrix,mvMatrix);
    Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_biasedShadowMvpMatrix,1,GL_FALSE,tmp);
    }
#   endif //TEAPOT_SHADER_USE_SHADOW_MAP

    Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE,mvMatrix);

    if (meshId<TEAPOT_FIRST_MESHLINES_INDEX) {
        if (meshId == TEAPOT_MESH_PIVOT3D) {
#           ifndef TEAPOT_NO_MESH_PIVOT3D
            const float pushColorAmbient[4] = {TIS.colorAmbient[0],TIS.colorAmbient[1],TIS.colorAmbient[2],TIS.colorAmbient[3]};
            const float pushScaling[3] = {TIS.scaling[0],TIS.scaling[1],TIS.scaling[2]};
            const tpoat T[3] = {mvMatrix[12],mvMatrix[13],mvMatrix[14]};
            tpoat mv[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, T[0],T[1],T[2],1};
            const tpoat charScale = 0.1f;
            const tpoat cp[3]={0.56f*TIS.scaling[0],0.05f*TIS.scaling[1],0.f*TIS.scaling[2]};

            // [ 0-30] 30 tri-faces: Center   (0.875,0.875,0.000)
            // [30-54] 24 tri-faces: ArrowX   (0.875,0.000,0.000)
            // [54-78] 24 tri-faces: ArrowY   (0.000,0.875,0.000)
            // [78-102]24 tri-faces: Arrow>   (0.000,0.000,0.875)
            int startInds = TIS.startInds[meshId];
            const int numInds[4] = {90,72,72,72};
            float brightness = (0.35f*TIS.color[0]+0.5f*TIS.color[1]+0.15f*TIS.color[2])*1.25f;
            if (brightness>1.f) brightness=1.f;

            if (TIS.meshOutlineEnabled && TIS.colorMeshOutline[3]>0)
            {
                const int mustUsePolygonOffset = (TIS.polygonOffsetSlope!=0 && TIS.polygonOffsetConstant!=0) ? 1 : 0;
                const float opacity = TIS.color[3]<1 ? (TIS.colorMeshOutline[3]>TIS.color[3]?(TIS.color[3]*0.7f):(TIS.colorMeshOutline[3]*0.7f)) : TIS.colorMeshOutline[3];

                if (mustUsePolygonOffset) {glEnable(GL_POLYGON_OFFSET_FILL);glPolygonOffset( TIS.polygonOffsetSlope, TIS.polygonOffsetConstant);}
                glFrontFace(GL_CW);
                Teapot_SetScaling(pushScaling[0]*TIS.scalingMeshOutline,pushScaling[1]*TIS.scalingMeshOutline,pushScaling[2]*TIS.scalingMeshOutline);
                glUniform4f(TIS.uLoc_colorAmbient,TIS.colorMeshOutline[0],TIS.colorMeshOutline[1],TIS.colorMeshOutline[2],0);
                glUniform4f(TIS.uLoc_color,TIS.color[0],TIS.color[1],TIS.color[2],opacity);

                //Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE, mvMatrix);
                glDrawElements(GL_TRIANGLES,TIS.numInds[meshId],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[meshId]*sizeof(unsigned short)));
                if (TIS.color[3]<opacity) glUniform4f(TIS.uLoc_color,TIS.color[0],TIS.color[1],TIS.color[2],TIS.color[3]);
                glFrontFace(GL_CCW);
                if (mustUsePolygonOffset) glDisable(GL_POLYGON_OFFSET_FILL);
            }

            //glUniform4f(TIS.uLoc_colorAmbient,0.875,0.875,0,0);
            glUniform4f(TIS.uLoc_colorAmbient,TIS.color[0],TIS.color[1],TIS.color[2],0);
            glDrawElements(GL_TRIANGLES,numInds[0],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
            startInds+=numInds[0];

            glUniform4f(TIS.uLoc_colorAmbient,0.875*brightness,0,0,0);
            glDrawElements(GL_TRIANGLES,numInds[1],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
            startInds+=numInds[1];

            glUniform4f(TIS.uLoc_colorAmbient,0,0.875*brightness,0,0);
            glDrawElements(GL_TRIANGLES,numInds[2],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
            startInds+=numInds[2];

            glUniform4f(TIS.uLoc_colorAmbient,0,0,0.875*brightness,0);
            glDrawElements(GL_TRIANGLES,numInds[3],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
            startInds+=numInds[3];
            //if (startInds!=TIS.startInds[meshId]+TIS.numInds[meshId]) {printf("ERROR: startInds(=%d)!=TIS.startInds[%d](=%d)+TIS.numInds[%d](=%d)\n",startInds,(int)meshId,TIS.startInds[meshId],(int)meshId,TIS.numInds[meshId]);exit(1);}

            Teapot_SetScaling(pushScaling[0]*charScale,pushScaling[1]*charScale,pushScaling[2]*charScale);
#           ifndef TEAPOT_NO_MESH_TEXT_X
            glUniform4f(TIS.uLoc_colorAmbient,0.875,0,0,0);
            mv[12] = T[0] + cp[0]*mvMatrix[0] + cp[1]*mvMatrix[4] + cp[2]*mvMatrix[8];
            mv[13] = T[1] + cp[0]*mvMatrix[1] + cp[1]*mvMatrix[5] + cp[2]*mvMatrix[9];
            mv[14] = T[2] + cp[0]*mvMatrix[2] + cp[1]*mvMatrix[6] + cp[2]*mvMatrix[10];
            Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE, mv);
            glDrawElements(GL_TRIANGLES,TIS.numInds[TEAPOT_MESH_TEXT_X],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[TEAPOT_MESH_TEXT_X]*sizeof(unsigned short)));
#           endif //TEAPOT_NO_MESH_TEXT_X
#           ifndef TEAPOT_NO_MESH_TEXT_Y
            glUniform4f(TIS.uLoc_colorAmbient,0,0.875,0,0);
            mv[12] = T[0] + cp[1]*mvMatrix[0] + cp[0]*mvMatrix[4] + cp[2]*mvMatrix[8];
            mv[13] = T[1] + cp[1]*mvMatrix[1] + cp[0]*mvMatrix[5] + cp[2]*mvMatrix[9];
            mv[14] = T[2] + cp[1]*mvMatrix[2] + cp[0]*mvMatrix[6] + cp[2]*mvMatrix[10];
            Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE, mv);
            glDrawElements(GL_TRIANGLES,TIS.numInds[TEAPOT_MESH_TEXT_Y],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[TEAPOT_MESH_TEXT_Y]*sizeof(unsigned short)));
#           endif //TEAPOT_NO_MESH_TEXT_Y
#           ifndef TEAPOT_NO_MESH_TEXT_Z
            glUniform4f(TIS.uLoc_colorAmbient,0,0.0,875,0);
            mv[12] = T[0] + cp[2]*mvMatrix[0] + cp[1]*mvMatrix[4] + cp[0]*mvMatrix[8];
            mv[13] = T[1] + cp[2]*mvMatrix[1] + cp[1]*mvMatrix[5] + cp[0]*mvMatrix[9];
            mv[14] = T[2] + cp[2]*mvMatrix[2] + cp[1]*mvMatrix[6] + cp[0]*mvMatrix[10];
            Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE, mv);
            glDrawElements(GL_TRIANGLES,TIS.numInds[TEAPOT_MESH_TEXT_Z],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[TEAPOT_MESH_TEXT_Z]*sizeof(unsigned short)));
#           endif //TEAPOT_NO_MESH_TEXT_Z


            Teapot_SetScaling(pushScaling[0],pushScaling[1],pushScaling[2]);
            Teapot_SetColorAmbient(pushColorAmbient[0],pushColorAmbient[1],pushColorAmbient[2]);
#           endif //TEAPOT_NO_MESH_PIVOT3D
        }
        else    {
            if (TIS.meshOutlineEnabled && TIS.colorMeshOutline[3]>0)
            {
                const float pushColorAmbient[4] = {TIS.colorAmbient[0],TIS.colorAmbient[1],TIS.colorAmbient[2],TIS.colorAmbient[3]};
                const float pushScaling[3] = {TIS.scaling[0],TIS.scaling[1],TIS.scaling[2]};
                const int mustUsePolygonOffset = (TIS.polygonOffsetSlope!=0 && TIS.polygonOffsetConstant!=0) ? 1 : 0;
                const float opacity = TIS.color[3]<1 ? (TIS.colorMeshOutline[3]>TIS.color[3]?(TIS.color[3]*0.7f):(TIS.colorMeshOutline[3]*0.7f)) : TIS.colorMeshOutline[3];
                int numInds = TIS.numInds[meshId];
#               ifdef TEAPOT_NO_MESH_CAR_WHEELS
                if (meshId==TEAPOT_MESH_CAR) numInds-=360;
#               endif

                if (mustUsePolygonOffset) {glEnable(GL_POLYGON_OFFSET_FILL);glPolygonOffset( TIS.polygonOffsetSlope, TIS.polygonOffsetConstant);}
                glFrontFace(GL_CW);
#               ifdef TEAPOT_CENTER_MESHES_ON_FLOOR
                if (meshId<TEAPOT_MESH_HALF_SPHERE_UP && TIS.scalingMeshOutline>1)  {
                    tpoat mv[16];int i;
                    const float cpY = 2.f*TIS.halfExtents[meshId][1]*pushScaling[1]*(1.0f-TIS.scalingMeshOutline)*0.5f;
                    for (i=0;i<16;i++) mv[i] = mvMatrix[i];
                    mv[12]+= cpY*mvMatrix[4];
                    mv[13]+= cpY*mvMatrix[5];
                    mv[14]+= cpY*mvMatrix[6];
                    Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE, mv);
                }
#               endif
                Teapot_SetScaling(pushScaling[0]*TIS.scalingMeshOutline,pushScaling[1]*TIS.scalingMeshOutline,pushScaling[2]*TIS.scalingMeshOutline);
                glUniform4f(TIS.uLoc_colorAmbient,TIS.colorMeshOutline[0],TIS.colorMeshOutline[1],TIS.colorMeshOutline[2],0);
                glUniform4f(TIS.uLoc_color,TIS.color[0],TIS.color[1],TIS.color[2],opacity);

                glDrawElements(GL_TRIANGLES,numInds,GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[meshId]*sizeof(unsigned short)));
                glFrontFace(GL_CCW);
                if (mustUsePolygonOffset) glDisable(GL_POLYGON_OFFSET_FILL);

                // This draws a wireframe mesh over the mesh (and works)
                /*glEnable(  GL_POLYGON_OFFSET_LINE );
        glPolygonOffset( -2.5f, -2.5f );
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glUniform4f(TIS.uLoc_colorAmbient,TIS.colorMeshOutline[0],TIS.colorMeshOutline[1],TIS.colorMeshOutline[2],0);
        glUniform4f(TIS.uLoc_color,TIS.color[0],TIS.color[1],TIS.color[2],TIS.colorMeshOutline[3]);
        Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE, mvMatrix);
        glDrawElements(GL_TRIANGLES,TIS.numInds[meshId],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[meshId]*sizeof(unsigned short)));
        glDisable(GL_POLYGON_OFFSET_LINE);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );*/



                Teapot_SetScaling(pushScaling[0],pushScaling[1],pushScaling[2]);
                Teapot_SetColorAmbient(pushColorAmbient[0],pushColorAmbient[1],pushColorAmbient[2]);
                if (TIS.color[3]<opacity) glUniform4f(TIS.uLoc_color,TIS.color[0],TIS.color[1],TIS.color[2],TIS.color[3]);
#               ifdef TEAPOT_CENTER_MESHES_ON_FLOOR
                Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix,1,GL_FALSE, mvMatrix);
#               endif
            }
            switch (meshId) {
            case TEAPOT_MESH_CAR:   {
                int startInds = TIS.startInds[meshId];
                const int numInds[3] = {156,24,         // Chassis, Glass, Black
#               ifdef TEAPOT_NO_MESH_CAR_WHEELS
                204                                     // Black:	204 inds
#               else
                564                                     // Wheels:	360 inds
#               endif
                };
                const float pushColor[4] = {TIS.color[0],TIS.color[1],TIS.color[2],TIS.color[3]};

                glDrawElements(GL_TRIANGLES,numInds[0],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[0];

                Teapot_SetColor(0.074895f,0.62175f,0.64f,pushColor[3]);
                glDrawElements(GL_TRIANGLES,numInds[1],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[1];

                Teapot_SetColor(0.f,0.f,0.f,pushColor[3]);
                glDrawElements(GL_TRIANGLES,numInds[2],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[2];
                //if (startInds!=TIS.startInds[meshId]+TIS.numInds[meshId]) {printf("ERROR: startInds(=%d)!=TIS.startInds[%d](=%d)+TIS.numInds[%d](=%d)\n",startInds,(int)meshId,TIS.startInds[meshId],(int)meshId,TIS.numInds[meshId]);exit(1);}

                Teapot_SetColor(pushColor[0],pushColor[1],pushColor[2],pushColor[3]);
            }
            break;
            case TEAPOT_MESH_CHARACTER:   {
                int startInds = TIS.startInds[meshId];
                const int numInds[3] = {327,216,183};       // Outfit, Black, Skin
                const float pushColor[4] = {TIS.color[0],TIS.color[1],TIS.color[2],TIS.color[3]};

                glDrawElements(GL_TRIANGLES,numInds[0],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[0];

                Teapot_SetColor(0.02f,0.02f,0.02f,pushColor[3]);
                glDrawElements(GL_TRIANGLES,numInds[1],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[1];

                Teapot_SetColor(0.740000f,0.562612f,0.399372f,pushColor[3]);
                glDrawElements(GL_TRIANGLES,numInds[2],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[2];
                //if (startInds!=TIS.startInds[meshId]+TIS.numInds[meshId]) {printf("ERROR: startInds(=%d)!=TIS.startInds[%d](=%d)+TIS.numInds[%d](=%d)\n",startInds,(int)meshId,TIS.startInds[meshId],(int)meshId,TIS.numInds[meshId]);exit(1);}

                Teapot_SetColor(pushColor[0],pushColor[1],pushColor[2],pushColor[3]);
            }
            break;
            case TEAPOT_MESH_GHOST:   {
                int startInds = TIS.startInds[meshId];
                const int numInds[3] = {558,48,24};       // Color, White, Black
                const float pushColor[4] = {TIS.color[0],TIS.color[1],TIS.color[2],TIS.color[3]};

                glDrawElements(GL_TRIANGLES,numInds[0],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[0];

                Teapot_SetColor(0.9f,0.9f,0.9f,pushColor[3]);
                glDrawElements(GL_TRIANGLES,numInds[1],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[1];

                Teapot_SetColor(0.02f,0.02f,0.02f,pushColor[3]);
                glDrawElements(GL_TRIANGLES,numInds[2],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[2];
                //if (startInds!=TIS.startInds[meshId]+TIS.numInds[meshId]) {printf("ERROR: startInds(=%d)!=TIS.startInds[%d](=%d)+TIS.numInds[%d](=%d)\n",startInds,(int)meshId,TIS.startInds[meshId],(int)meshId,TIS.numInds[meshId]);exit(1);}

                Teapot_SetColor(pushColor[0],pushColor[1],pushColor[2],pushColor[3]);
            }
            break;
            case TEAPOT_MESH_SLEDGE: {
                int startInds = TIS.startInds[meshId];
                const int numInds[2] = {68*3,144*3};
                const float pushColor[4] = {TIS.color[0],TIS.color[1],TIS.color[2],TIS.color[3]};

                glDrawElements(GL_TRIANGLES,numInds[0],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[0];

                Teapot_SetColor(0.9f,0.9f,0.f,pushColor[3]);
                glDrawElements(GL_TRIANGLES,numInds[1],GL_UNSIGNED_SHORT,(const void*) (startInds*sizeof(unsigned short)));
                startInds+=numInds[1];
                //if (startInds!=TIS.startInds[meshId]+TIS.numInds[meshId]) {printf("ERROR: startInds(=%d)!=TIS.startInds[%d](=%d)+TIS.numInds[%d](=%d)\n",startInds,(int)meshId,TIS.startInds[meshId],(int)meshId,TIS.numInds[meshId]);exit(1);}

                Teapot_SetColor(pushColor[0],pushColor[1],pushColor[2],pushColor[3]);
            }            
            break;
            default:
                glDrawElements(GL_TRIANGLES,TIS.numInds[meshId],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[meshId]*sizeof(unsigned short)));
            break;
            }
        }
    }
    else {
        const float pushColorAmbient[4] = {TIS.colorAmbient[0],TIS.colorAmbient[1],TIS.colorAmbient[2],TIS.colorAmbient[3]};
        glUniform4f(TIS.uLoc_colorAmbient,TIS.color[0],TIS.color[1],TIS.color[2],0);
        glDrawElements(GL_LINES,TIS.numInds[meshId],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[meshId]*sizeof(unsigned short)));
        Teapot_SetColorAmbient(pushColorAmbient[0],pushColorAmbient[1],pushColorAmbient[2]);
    }

}

void Teapot_Draw(const tpoat mMatrix[16], TeapotMeshEnum meshId) {
    tpoat mvMatrix[16]; // mvMatrix = vMatrix * mMatrix;
    if (meshId==TEAPOT_MESH_COUNT) return;
    Teapot_Helper_MultMatrix(mvMatrix,TIS.vMatrix,mMatrix);
    Teapot_Draw_Mv(mvMatrix,meshId);
}

void Teapot_LowLevel_StartDisablingLighting(void) {
    TIS.colorAmbient[0]=TIS.colorAmbient[1]=TIS.colorAmbient[2]=0.8f;TIS.colorAmbient[3]=0.f;
    glUniform4fv(TIS.uLoc_colorAmbient,1,TIS.colorAmbient);
}

void Teapot_LowLevel_SetMvMatrixUniform(const tpoat mvMatrix[16])   {
    Teapot_Helper_GlUniformMatrix4v(TIS.uLoc_mvMatrix, 1 /*only setting 1 matrix*/, GL_FALSE /*transpose?*/, mvMatrix);
}
void Teapot_LowLevel_DrawElements(TeapotMeshEnum meshId)    {
    glDrawElements(meshId<TEAPOT_FIRST_MESHLINES_INDEX ? GL_TRIANGLES : GL_LINES,TIS.numInds[meshId],GL_UNSIGNED_SHORT,(const void*) (TIS.startInds[meshId]*sizeof(unsigned short)));
}

void Teapot_LowLevel_StopDisablingLighting(void) {
    TIS.colorAmbient[0]=TIS.colorAmbient[1]=TIS.colorAmbient[2]=0.25f;TIS.colorAmbient[3]=1.f;
    glUniform4fv(TIS.uLoc_colorAmbient,1,TIS.colorAmbient);
}

void Teapot_PostDraw(void)  {
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDisableVertexAttribArray(TIS.aLoc_vertex);
    glDisableVertexAttribArray(TIS.aLoc_normal);
}


static int _Teapot_MeshData_Comparer(const void* pmd0,const void* pmd1) {
    const Teapot_MeshData* md0 = *((const Teapot_MeshData* const*) (pmd0));
    const Teapot_MeshData* md1 = *((const Teapot_MeshData* const*) (pmd1));
    if (md0->color[3]<1.f)   {
        if (md1->color[3]<1.f) {
            //return fabsf(md1->mvMatrix[14])-fabsf(md0->mvMatrix[14]);
            return (md1->mvMatrix[12]*md1->mvMatrix[12]+md1->mvMatrix[13]*md1->mvMatrix[13]+md1->mvMatrix[14]*md1->mvMatrix[14]) -
                    (md0->mvMatrix[12]*md0->mvMatrix[12]+md0->mvMatrix[13]*md0->mvMatrix[13]+md0->mvMatrix[14]*md0->mvMatrix[14]);
        }
        else return 1;
    }
    else if (md1->color[3]<1.f) return -1;
    return 0;
}

void Teapot_MeshData_Clear(Teapot_MeshData* md) {
    Teapot_Helper_IdentityMatrix(md->mvMatrix);
    md->color[0]=md->color[1]=md->color[2]=0.75f;md->color[3]=1.f;
    md->colorAmbient[0]=md->colorAmbient[1]=md->colorAmbient[2]=0.25f;
    md->colorSpecular[0]=md->colorSpecular[1]=md->colorSpecular[2]=0.8f;md->colorSpecular[3]=20.f;
    md->scaling[0]=md->scaling[1]=md->scaling[2]=1.f;
    md->outlineEnabled = 0;md->userPtr=0;
}
#ifdef __cplusplus
_Teapot_MeshData::_Teapot_MeshData() {Teapot_MeshData_Clear(this);}
#endif
void Teapot_MeshData_SetMvMatrix(Teapot_MeshData* md, const tpoat* mvMatrix16) {Teapot_Helper_CopyMatrix(md->mvMatrix,mvMatrix16);}
#ifndef TEAPOT_MESHDATA_HAS_MMATRIX_PTR
void Teapot_MeshData_SetMMatrix(Teapot_MeshData* md, const tpoat* mMatrix16) {Teapot_Helper_CopyMatrix(md->mMatrix,mMatrix16);}
#endif //TEAPOT_MESHDATA_HAS_MMATRIX_PTR
void Teapot_MeshData_CalculateMvMatrix(Teapot_MeshData* md) {Teapot_Helper_MultMatrixUncheckArgs(md->mvMatrix,TIS.vMatrix,md->mMatrix);}

#ifndef TEAPOT_USE_OPENMP
void Teapot_MeshData_CalculateMvMatrixFromArray(Teapot_MeshData** meshes,int numMeshes) {int i;if (!meshes || numMeshes<=0) return;for (i=0;i<numMeshes;i++) Teapot_MeshData_CalculateMvMatrix(meshes[i]);}
#else //TEAPOT_USE_OPENMP
void Teapot_MeshData_CalculateMvMatrixFromArray(Teapot_MeshData** meshes,int numMeshes) {
    int i;if (!meshes || numMeshes<=0) return;
#   pragma omp parallel
    for (i=0;i<numMeshes;i++) {
        Teapot_MeshData* md = meshes[i];
        Teapot_Helper_MultMatrix(md->mvMatrix,TIS.vMatrix,md->mMatrix);
    }
}
#endif //TEAPOT_USE_OPENMP

void Teapot_DrawMulti(Teapot_MeshData** meshes,int numMeshes,int mustSortObjectsForTransparency) {
    Teapot_MeshData_CalculateMvMatrixFromArray(meshes,numMeshes);
    Teapot_DrawMulti_Mv(meshes,numMeshes,mustSortObjectsForTransparency);
}
void Teapot_DrawMulti_Mv(Teapot_MeshData* const* meshes,int numMeshes,int mustSortObjectsForTransparency)  {
    if (!meshes || numMeshes<=0) return;
    if (mustSortObjectsForTransparency) qsort((void*)meshes,numMeshes,sizeof(Teapot_MeshData*),_Teapot_MeshData_Comparer);
    {
        const int pushMeshOutlineEnabled = TIS.meshOutlineEnabled;
        int i,startTransparentObjects=mustSortObjectsForTransparency?0:-1;
        for (i=0;i<numMeshes;i++) {
            const Teapot_MeshData* md = meshes[i];
            TIS.meshOutlineEnabled = md->outlineEnabled;
            if (!TIS.colorMaterialEnabled)  {
                Teapot_SetColorAmbient(md->colorAmbient[0],md->colorAmbient[1],md->colorAmbient[2]);
#               ifdef TEAPOT_SHADER_SPECULAR
                Teapot_SetColorSpecular(md->colorSpecular[0],md->colorSpecular[1],md->colorSpecular[2],md->colorSpecular[3]);
#               endif //TEAPOT_SHADER_SPECULAR
            }
            Teapot_SetColor(md->color[0],md->color[1],md->color[2],md->color[3]);
            Teapot_SetScaling(md->scaling[0]==0?1:md->scaling[0],md->scaling[1]==0?1:md->scaling[1],md->scaling[2]==0?1:md->scaling[2]);
            if (md->color[3]<1.f && startTransparentObjects==0) {
                startTransparentObjects=1;
                glEnable(GL_BLEND);
            }
            if (md->color[3]!=0) Teapot_Draw_Mv(md->mvMatrix,md->meshId);
        }
        if (startTransparentObjects==1) {
            glDisable(GL_BLEND);
        }
        TIS.meshOutlineEnabled = pushMeshOutlineEnabled;
    }
}


// Loading shader function
static __inline GLhandleARB Teapot_LoadShader(const char* buffer, const unsigned int type)
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

static __inline GLuint Teapot_LoadShaderProgramFromSource(const char* vs,const char* fs)	{
    // shader Compilation variable
    GLint result;				// Compilation code result
    GLint errorLoglength ;
    char* errorLogText;
    GLsizei actualErrorLogLength;

    GLhandleARB vertexShaderHandle;
    GLhandleARB fragmentShaderHandle;
    GLuint programId = 0;

    vertexShaderHandle   = Teapot_LoadShader(vs,GL_VERTEX_SHADER);
    fragmentShaderHandle = Teapot_LoadShader(fs,GL_FRAGMENT_SHADER);
    if (!vertexShaderHandle || !fragmentShaderHandle) return 0;

    programId = glCreateProgram();

    glAttachShader(programId,vertexShaderHandle);
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
    glDeleteShader(fragmentShaderHandle);

    return programId;
}

// numVerts is the number of vertices (= number of 3 floats)
// numInds is the number of triangle indices (= 3 * num triangles)
// pNormsOut must be the same size as pVerts
__inline static void CalculateVertexNormals(const float* pVerts,int numVerts,int vertsStrideInNumComponents,const unsigned short* pInds,int numInds,float* pNormsOut,int normsStrideInNumComponents)   {
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
            if (len>0.00001f)   {len = sqrt(len);n[0]/=len;n[1]/=len;n[2]/=len;}
            else {n[0]=0.f;n[1]=1.f;n[2]=0.f;}
        }
    }
}

// numVerts is the number of vertices (= number of 3 floats)
__inline static void GetAabbHalfExtentsAndCenter(const float* pVerts,const int numVerts,int vertsStrideInNumComponents,float halfExtentsOut[3],float centerPointOut[3])	{
    float minValue[3];float maxValue[3];float tempVert[3];int i,i3;
    if (vertsStrideInNumComponents<3) vertsStrideInNumComponents=3;
    if (pVerts && numVerts>0)	{
        minValue[0]=maxValue[0]=pVerts[0];minValue[1]=maxValue[1]=pVerts[1];minValue[2]=maxValue[2]=pVerts[2];
    }
    else minValue[0]=maxValue[0]=minValue[1]=maxValue[1]=minValue[2]=maxValue[2]=0;
    for (i=1; i<numVerts; i++)	{
        i3 = i*vertsStrideInNumComponents;tempVert[0]=pVerts[i3];tempVert[1]=pVerts[i3+1];tempVert[2]=pVerts[i3+2];
        if (minValue[0]>tempVert[0])        minValue[0]=tempVert[0];
        else if (maxValue[0]<tempVert[0])   maxValue[0]=tempVert[0];
        if (minValue[1]>tempVert[1])        minValue[1]=tempVert[1];
        else if (maxValue[1]<tempVert[1])   maxValue[1]=tempVert[1];
        if (minValue[2]>tempVert[2])        minValue[2]=tempVert[2];
        else if (maxValue[2]<tempVert[2])   maxValue[2]=tempVert[2];
    }
    for (i=0;i<3;i++) {
        halfExtentsOut[i]=(maxValue[i]-minValue[i])*0.5;
        centerPointOut[i]=(maxValue[i]+minValue[i])*0.5;
    }
}

void Teapot_Destroy(void) {
    if (TIS.vertexBuffer) {
        glDeleteBuffers(1,&TIS.vertexBuffer);
        TIS.vertexBuffer = 0;
    }
    if (TIS.elementBuffer) {
        glDeleteBuffers(1,&TIS.elementBuffer);
        TIS.elementBuffer = 0;
    }
    if (TIS.programId) {
        glDeleteProgram(TIS.programId);TIS.programId=0;
    }
}

static void AddMeshVertsAndInds(float* totVerts,const int MAX_TOTAL_VERTS,int* numTotVerts,int totVertsStrideInNumComponents,unsigned short* totInds,const int MAX_TOTAL_INDS,int* numTotInds,
                                const float* verts,int numVerts,const unsigned short* inds,int numInds,TeapotMeshEnum meshId) {
    int i,i3;
    float* pTotVerts = &totVerts[(*numTotVerts)*totVertsStrideInNumComponents];
    unsigned short* pTotInds = &totInds[*numTotInds];
    if (*numTotVerts+numVerts>MAX_TOTAL_VERTS) {
        if (meshId>=TEAPOT_MESH_USER_00 && meshId<TEAPOT_MESH_CUBE)
            fprintf(stderr,"Error in teapot.h: not enough user mesh vertices [TEAPOT_MAX_NUM_MESH_VERTS+TEAPOT_MAX_NUM_USER_MESH_VERTICES must be at least: %d (you can set it in the Teapot_Init(void) implementation or just define a bigger TEAPOT_MAX_NUM_USER_MESH_VERTICES)\n",*numTotVerts+numVerts);
        else
            fprintf(stderr,"Error in teapot.h: TEAPOT_MAX_NUM_MESH_VERTS must be at least: %d (you can set it in the Teapot_Init(void) implementation)\n",*numTotVerts+numVerts);
        exit(0);
    }
    if (*numTotInds+numInds>MAX_TOTAL_INDS) {
        if (meshId>=TEAPOT_MESH_USER_00 && meshId<TEAPOT_MESH_CUBE)
            fprintf(stderr,"Error: not enough user mesh indices [TEAPOT_MAX_NUM_MESH_INDS+TEAPOT_MAX_NUM_USER_MESH_INDICES must be at least: %d (you can set it in the Teapot_Init(void) implementation or just define a bigger TEAPOT_MAX_NUM_USER_MESH_INDICES)\n",*numTotInds+numInds);
        else
            fprintf(stderr,"Error: TEAPOT_MAX_NUM_MESH_INDS must be at least: %d (you can set it in the Teapot_Init(void) implementation)\n",*numTotInds+numInds);
        exit(0);
    }
    TIS.startInds[meshId] = *numTotInds;
    TIS.numInds[meshId] = numInds;

    // HERE we can do stuff on verts/inds---------------------------------------
    for (i=0;i<numVerts;i++) {
        i3=i*3;
        pTotVerts[0] = verts[i3];
        pTotVerts[1] = verts[i3+1];
#       ifndef  TEAPOT_INVERT_MESHES_Z_AXIS
        pTotVerts[2] = verts[i3+2];
#       else //TEAPOT_INVERT_MESHES_Z_AXIS
        pTotVerts[2] = -verts[i3+2];
#       endif //TEAPOT_INVERT_MESHES_Z_AXIS

        pTotVerts+=totVertsStrideInNumComponents;
    }

    if (meshId<TEAPOT_FIRST_MESHLINES_INDEX)    {
        for (i=0;i<numInds;i+=3) {
            pTotInds[0] = inds[i]+(*numTotVerts);
#           ifndef  TEAPOT_INVERT_MESHES_Z_AXIS
            pTotInds[1] = inds[i+1]+(*numTotVerts);
            pTotInds[2] = inds[i+2]+(*numTotVerts);
#           else //TEAPOT_INVERT_MESHES_Z_AXIS
            pTotInds[2] = inds[i+1]+(*numTotVerts);
            pTotInds[1] = inds[i+2]+(*numTotVerts);
#           endif //TEAPOT_INVERT_MESHES_Z_AXIS
            pTotInds+=3;
        }
    }
    else {
        for (i=0;i<numInds;i+=2) {
            pTotInds[0] = inds[i]+(*numTotVerts);
            pTotInds[1] = inds[i+1]+(*numTotVerts);
            pTotInds+=2;
        }
    }
    //---------------------------------------------------------------------------

    GetAabbHalfExtentsAndCenter(&totVerts[(*numTotVerts)*totVertsStrideInNumComponents],numVerts,6,&TIS.halfExtents[meshId][0],&TIS.centerPoint[meshId][0]);

    /*fprintf(stderr,"%d) centerPoint:%1.2f,%1.2f,%1.2f   halfExtents:%1.2f,%1.2f,%1.2f\n",meshId,
            TIS.centerPoint[meshId][0],TIS.centerPoint[meshId][1],TIS.centerPoint[meshId][2],
            TIS.halfExtents[meshId][0],TIS.halfExtents[meshId][1],TIS.halfExtents[meshId][2]
            );*/

#   ifdef TEAPOT_CENTER_MESHES_ON_FLOOR
    //if (meshId<=TEAPOT_MESH_SPHERE2)
    //if (meshId!=TEAPOT_MESH_CYLINDER_LATERAL_SURFACE && meshId!=TEAPOT_MESH_HALF_SPHERE_UP && meshId!=TEAPOT_MESH_HALF_SPHERE_DOWN && meshId!=TEAPOT_MESH_PIVOT3D)
    if (meshId<TEAPOT_MESH_CYLINDER_LATERAL_SURFACE || meshId>=TEAPOT_FIRST_MESHLINES_INDEX)
    {
        pTotVerts = &totVerts[(*numTotVerts)*totVertsStrideInNumComponents];
        for (i=1;i<numVerts*totVertsStrideInNumComponents;i+=totVertsStrideInNumComponents) {
            pTotVerts[i] += TIS.halfExtents[meshId][1];
        }
    }
    TIS.centerPoint[meshId][1]+= TIS.halfExtents[meshId][1];
#   endif //TEAPOT_CENTER_MESHES_ON_FLOOR

    {
        float center[3];float halfAabb[3];
        for (i=0;i<3;i++) {
            center[i] = TIS.centerPoint[meshId][i];
            halfAabb[i] = TIS.halfExtents[meshId][i];
            TIS.aabbMin[meshId][i] = center[i] - halfAabb[i];
            TIS.aabbMax[meshId][i] = center[i] + halfAabb[i];
        }
    }

    *numTotVerts+=numVerts;
    *numTotInds+=numInds;

    if (gTeapotInitCallback) gTeapotInitCallback(meshId,verts,numVerts,inds,numInds);
}


void Teapot_GetMeshAabbCenter(TeapotMeshEnum meshId,float center[3]) {
    center[0] = TIS.centerPoint[meshId][0];center[1] = TIS.centerPoint[meshId][1];center[2] = TIS.centerPoint[meshId][2];
}
void Teapot_GetMeshAabbHalfExtents(TeapotMeshEnum meshId,float halfAabb[3]) {
    halfAabb[0] = TIS.halfExtents[meshId][0];halfAabb[1] = TIS.halfExtents[meshId][1];halfAabb[2] = TIS.halfExtents[meshId][2];
}
void Teapot_GetMeshAabbExtents(TeapotMeshEnum meshId,float aabb[3]) {
    aabb[0] = TIS.halfExtents[meshId][0]*2;aabb[1] = TIS.halfExtents[meshId][1]*2;aabb[2] = TIS.halfExtents[meshId][2]*2;
}
void Teapot_GetMeshAabbMinAndMax(TeapotMeshEnum meshId,float aabbMin[3],float aabbMax[3])  {
    aabbMin[0] = TIS.aabbMin[meshId][0];aabbMin[1] = TIS.aabbMin[meshId][1];aabbMin[2] = TIS.aabbMin[meshId][2];
    aabbMax[0] = TIS.aabbMax[meshId][0];aabbMax[1] = TIS.aabbMax[meshId][1];aabbMax[2] = TIS.aabbMax[meshId][2];
}

void Teapot_Set_Init_Callback(TeapotInitCallback callback)  {gTeapotInitCallback = callback;}

void Teapot_Set_Init_UserMeshCallback(TeapotInitUserMeshCallback callback) {gTeapotInitUserMeshCallback = callback;}

#if (defined(DYNAMIC_RESOLUTION_H) && defined(TEAPOT_SHADER_USE_SHADOW_MAP))
#if ((defined(DYNAMIC_RESOLUTION_USE_DOUBLE_PRECISION) && defined(TEAPOT_USE_DOUBLE_PRECISION)) || (!defined(DYNAMIC_RESOLUTION_USE_DOUBLE_PRECISION) && !defined(TEAPOT_USE_DOUBLE_PRECISION)))
static void Teapot_MeshData_HiLevel_DrawMulti_ShadowMap_Vp_Internal(Teapot_MeshData* const* pMeshData,int numMeshData,const tpoat* lvpMatrix16,const tpoat lvpMatrixFrustumPlaneEquations[6][4],float transparent_threshold, int use_frustum_culling)
{
    int i;
    Dynamic_Resolution_Bind_Shadow();   // Binds the shadow map FBO and its shader program
    glClear(GL_DEPTH_BUFFER_BIT);
    Dynamic_Resolution_Shadow_Set_VpMatrix(lvpMatrix16);  // lvpMatrix16 is good if we can use mMatrix below. If we MUST use mvMatrix below, here we must pass (lvpMatrix * cameraViewMatrixInverse). Please see Dynamic_Resolution_MultMatrix(...) and Teapot_GetViewMatrixInverse(...) methods.
    Teapot_LowLevel_BindVertexBufferObject();
    for (i=0;i<numMeshData;i++) {
        const Teapot_MeshData* md = pMeshData[i];
        TeapotMeshEnum meshId = md->meshId;
        if (md->color[3]>=transparent_threshold && meshId<TEAPOT_MESH_PIVOT3D)
        {
            if (use_frustum_culling) {
            // We could enable frustum culling here too...
            // but we don't... it's something manual...
            // (we must extract the shadow frustum planes and then call Teapot_Helper_IsVisible(...).
            // Please see Teapot_Helper_GetFrustumPlaneEquations(...);)
            // Not sure this works for meshId==TEAPOT_MESH_CAPSULE...

                if (meshId<TEAPOT_MESH_TEXT_X || meshId>TEAPOT_MESH_TEXT_Z) {
                    const float* scaling = md->scaling;
                    const tpoat aabbMin[3] = {TIS.aabbMin[meshId][0]*scaling[0],TIS.aabbMin[meshId][1]*scaling[1],TIS.aabbMin[meshId][2]*scaling[2]};
                    const tpoat aabbMax[3] = {TIS.aabbMax[meshId][0]*scaling[0],TIS.aabbMax[meshId][1]*scaling[1],TIS.aabbMax[meshId][2]*scaling[2]};

                    if (!Teapot_Helper_IsVisible(lvpMatrixFrustumPlaneEquations,
                                                 md->mMatrix,
                                                 aabbMin[0],aabbMin[1],aabbMin[2],
                                                 aabbMax[0],aabbMax[1],aabbMax[2]))
                    {
                        //fprintf(stderr,"MeshId=%d\n",meshId);
                        continue;
                    }
                }
            }


            if (meshId==TEAPOT_MESH_CAPSULE) {
                // unluckily TEAPOT_MESH_CAPSULE is special... sorry!
                // We don't want to draw "scaled" capsules. Instead we want to regenerate valid capsules, reinterpreting Teapot_SetScaling(...)
#               if (!defined(TEAPOT_NO_MESH_CYLINDER) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_UP) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_DOWN))
                const float height = md->scaling[1];
                const float diameter = (md->scaling[0]+md->scaling[2])*0.5f;
                const float yAxis[3] = {md->mMatrix[4],md->mMatrix[5],md->mMatrix[6]};
                tpoat mat[16];
                float tmp,center[3],origin[3];				
				Teapot_GetMeshAabbCenter(TEAPOT_MESH_CYLINDER_LATERAL_SURFACE,center);                
				tmp = (diameter*center[1]);

                origin[0] = md->mMatrix[12]+md->mMatrix[4]*tmp;
				origin[1] = md->mMatrix[13]-md->mMatrix[5]*tmp;
				origin[2] = md->mMatrix[14]-md->mMatrix[6]*tmp;

                center[1]=-center[1];   //
                //origin[1]-=diameter*center[1];

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

            // (Opt) Simplify meshes
            if (meshId==TEAPOT_MESH_SPHERE2)    meshId=TEAPOT_MESH_SPHERE1;
            else if (meshId==TEAPOT_MESH_CONE2) meshId=TEAPOT_MESH_CONE1;
            else if (meshId==TEAPOT_MESH_CUBIC_GROUND)  meshId=TEAPOT_MESH_CUBE;
            // End (Opt)

            Dynamic_Resolution_Shadow_Set_MMatrix(md->mMatrix); // mMatrix here (or mvMatrix if we had called above Dynamic_Resolution_Shadow_Set_VpMatrix(lvpMatrix * cameraViewMatrixInverse);)
            Dynamic_Resolution_Shadow_Set_Scaling(md->scaling[0],md->scaling[1],md->scaling[2]);
            Teapot_LowLevel_DrawElements(meshId);
        }
    }
    Teapot_LowLevel_UnbindVertexBufferObject();
    Dynamic_Resolution_Unbind_Shadow();   // Unbinds the shadow map FBO and its shader program

    Teapot_SetShadowVpMatrix(lvpMatrix16);    // Needed for the second shadowing pass (enabled with the definition: TEAPOT_SHADER_USE_SHADOW_MAP). Note that here we can't pass (lvpMatrix * cameraViewMatrixInverse), but just lvpMatrix (because the multiplication happens internally).
    Teapot_SetShadowMapFactor(Dynamic_Resolution_GetShadowMapDynResFactor());   // The shadow map has dynamic resolution too. That means that in the shader used in "teapot.h" there's an additional float uniform that must be updated from "dynamic_resolution.h"
    Teapot_SetShadowMapTexelIncrement(Dynamic_Resolution_GetShadowMapTexelIncrement(),Dynamic_Resolution_GetShadowMapTexelIncrement());
    glBindTexture(GL_TEXTURE_2D,Dynamic_Resolution_Get_Shadow_Texture_ID());
}
void Teapot_HiLevel_DrawMulti_ShadowMap_Vp(Teapot_MeshData* const* pMeshData,int numMeshData,const tpoat* lvpMatrix16, float transparent_threshold)  {
    static tpoat dummy[6][4];
    Teapot_MeshData_HiLevel_DrawMulti_ShadowMap_Vp_Internal(pMeshData,numMeshData,lvpMatrix16,dummy,transparent_threshold,0);
}
void Teapot_HiLevel_DrawMulti_ShadowMap_Vp_WithFrustumCulling(Teapot_MeshData* const* pMeshData,int numMeshData,const tpoat* lvpMatrix16,const tpoat lvpMatrixFrustumPlaneEquations[6][4], float transparent_threshold) {
    Teapot_MeshData_HiLevel_DrawMulti_ShadowMap_Vp_Internal(pMeshData,numMeshData,lvpMatrix16,lvpMatrixFrustumPlaneEquations,transparent_threshold,1);
}
#endif
#endif

void Teapot_Init(void) {
    int i,j;
    TIS.colorMaterialEnabled = 0;
    TIS.meshOutlineEnabled = 0;
    Teapot_Set_MeshOutline_Color(0,0,0,1);
    //Teapot_Set_MeshOutline_Scaling(1.0f);Teapot_Set_MeshOutline_Params(-2.0f,-250.f);
    Teapot_Set_MeshOutline_Scaling(1.015f);Teapot_Set_MeshOutline_Params(-1.0f,-250.f);
    TIS.vertexBuffer = TIS.elementBuffer = 0;
    for (i=0;i<TEAPOT_MESH_COUNT;i++) {
        TIS.startInds[i] = TIS.numInds[i] = 0;
        TIS.halfExtents[i][0]=TIS.halfExtents[i][1]=TIS.halfExtents[i][2]=0;
        TIS.centerPoint[i][0]=TIS.centerPoint[i][1]=TIS.centerPoint[i][2]=0;
    }

    TIS.programId =  Teapot_LoadShaderProgramFromSource(*TeapotVS,*TeapotFS);
    if (!TIS.programId) return;

    TIS.aLoc_vertex = glGetAttribLocation(TIS.programId, "a_vertex");
    TIS.aLoc_normal = glGetAttribLocation(TIS.programId, "a_normal");
    TIS.uLoc_mvMatrix = glGetUniformLocation(TIS.programId,"u_mvMatrix");
    TIS.uLoc_pMatrix = glGetUniformLocation(TIS.programId,"u_pMatrix");
    TIS.uLoc_nMatrix = glGetUniformLocation(TIS.programId,"u_nMatrix");
    TIS.uLoc_scaling = glGetUniformLocation(TIS.programId,"u_scaling");
    TIS.uLoc_lightVector = glGetUniformLocation(TIS.programId,"u_lightVector");
    TIS.uLoc_color = glGetUniformLocation(TIS.programId,"u_color");
    TIS.uLoc_colorAmbient = glGetUniformLocation(TIS.programId,"u_colorAmbient");
    TIS.uLoc_colorSpecular = glGetUniformLocation(TIS.programId,"u_colorSpecular");
    TIS.uLoc_fogColor = glGetUniformLocation(TIS.programId,"u_fogColor");
    TIS.uLoc_fogDistances = glGetUniformLocation(TIS.programId,"u_fogDistances");
    TIS.uLoc_biasedShadowMvpMatrix = glGetUniformLocation(TIS.programId,"u_biasedShadowMvpMatrix");
    TIS.uLoc_shadowMap = glGetUniformLocation(TIS.programId,"u_shadowMap");
    TIS.uLoc_shadowDarkening = glGetUniformLocation(TIS.programId,"u_shadowDarkening");
    TIS.uLoc_shadowMapFactor = glGetUniformLocation(TIS.programId,"u_shadowMapFactor");
    TIS.uLoc_shadowMapTexelIncrement = glGetUniformLocation(TIS.programId,"u_shadowMapTexelIncrement");


    /*
    if (TIS.aLoc_vertex<0) printf("Error: TIS.aLoc_vertex not found\n");
    //else printf("TIS.aLoc_vertex = %d\n",TIS.aLoc_vertex);
    if (TIS.aLoc_normal<0) printf("Error: TIS.aLoc_normal not found\n");
    //else printf("TIS.aLoc_normal = %d\n",TIS.aLoc_normal);
    if (TIS.uLoc_mvMatrix<0) printf("Error: TIS.uLoc_mvMatrix not found\n");
    if (TIS.uLoc_pMatrix<0) printf("Error: TIS.uLoc_pMatrix not found\n");
    if (TIS.uLoc_nMatrix<0) printf("Error: TIS.uLoc_nMatrix not found\n");
    if (TIS.uLoc_scaling<0) printf("Error: TIS.uLoc_scaling not found\n");
    if (TIS.uLoc_lightVector<0) printf("Error: TIS.uLoc_lightVector not found\n");
    if (TIS.uLoc_color<0) printf("Error: TIS.uLoc_color not found\n");
    if (TIS.uLoc_colorAmbient<0) printf("Error: TIS.uLoc_colorAmbient not found\n");
    if (TIS.uLoc_colorSpecular<0) printf("Error: TIS.uLoc_colorSpecular not found\n");
    if (TIS.uLoc_fogColor<0) printf("Error: TIS.uLoc_fogColor not found\n");
    if (TIS.uLoc_fogDistances<0) printf("Error: TIS.uLoc_fogDistances not found\n");
    if (TIS.uLoc_biasedShadowMvpMatrix<0) printf("Error: TIS.uLoc_biasedShadowMvpMatrix not found\n");
    if (TIS.uLoc_shadowMap<0) printf("Error: TIS.uLoc_shadowMap not found\n");
    if (TIS.uLoc_shadowDarkening<0) printf("Error: TIS.uLoc_shadowDarkening not found\n");
    if (TIS.uLoc_shadowMapFactor<0) printf("Error: TIS.uLoc_shadowMapFactor not found\n");
    */

    {
        tpoat one[4]={1.f,1.f,1.f,1.f};
        const tpoat pMatrix[16] = {1.3580,0.0000,0.0000,0.0000, 0.0000,2.4142,0.0000,0.0000, 0.0000,0.0000,-1.0004,-1.0000, 0.0000,0.0000,-0.2000,0.0000};
        const tpoat vMatrix[16] = {1.0000,0.0000,0.0000,0.0000, 0.0000,0.8944,0.4472,0.0000, 0.0000,-0.4472,0.8944,0.0000, -0.0000,0.0000,-22.3607,1.0000};
        Teapot_SetProjectionMatrix(pMatrix);    // m4_perspective(45.0f, 16.f/9.f,0.1f,500.f);
        Teapot_SetViewMatrixAndLightDirection(vMatrix,one);   // m4_look_at(0,10,20,0,0,0,0,1,0);
#       ifdef TEAPOT_SHADER_FOG
        Teapot_SetFogColor(0.45f,0.65f,0.95f);
        Teapot_SetFogDistances(50.f,200.f);
#       endif //TEAPOT_SHADER_FOG
#       ifdef TEAPOT_SHADER_USE_SHADOW_MAP
        Teapot_SetShadowDarkening(40.f,0.75f);
        Teapot_SetShadowMapFactor(1.f);
        Teapot_SetShadowMapTexelIncrement(1.f/512.f,1.f/512.f);
#       endif // TEAPOT_SHADER_USE_SHADOW_MAP

        glUseProgram(TIS.programId);
#       ifdef TEAPOT_SHADER_USE_SHADOW_MAP
        glUniform1i(TIS.uLoc_shadowMap,0);
#       endif // TEAPOT_SHADER_USE_SHADOW_MAP
        Teapot_SetScaling(1,1,1);
        Teapot_SetColor(0.75,0.75,0.75,1);
        TIS.colorAmbient[3]=1;Teapot_SetColorAmbient(0.25,0.25,0.25);
#       ifdef TEAPOT_SHADER_SPECULAR
        Teapot_SetColorSpecular(0.8,0.8,0.8,20.0);
#       endif //TEAPOT_SHADER_SPECULAR
        glUseProgram(0);
    }

    {        
#       ifndef TEAPOT_MAX_NUM_MESH_VERTS
#           define TEAPOT_MAX_NUM_MESH_VERTS  (2796)
#       endif //TEAPOT_MAX_NUM_MESH_VERTS
#       ifndef TEAPOT_MAX_NUM_MESH_INDS
#           define TEAPOT_MAX_NUM_MESH_INDS   (14148)
#       endif //TEAPOT_MAX_NUM_MESH_INDS

#       ifndef TEAPOT_MAX_NUM_USER_MESH_VERTICES
#           define TEAPOT_MAX_NUM_USER_MESH_VERTICES (1000)
#       endif //TEAPOT_MAX_NUM_USER_MESH_VERTICES

#       ifndef TEAPOT_MAX_NUM_USER_MESH_INDICES
#           define TEAPOT_MAX_NUM_USER_MESH_INDICES (5000)
#       endif //TEAPOT_MAX_NUM_USER_MESH_INDICES

        float totVerts[(TEAPOT_MAX_NUM_MESH_VERTS+TEAPOT_MAX_NUM_USER_MESH_VERTICES)*3*2];    // last x2 is because we use a single VBO with interleaved vertices and normals
        float* norms=&totVerts[0]+3;		    // pointer to the 1st normal
        unsigned short totInds[TEAPOT_MAX_NUM_MESH_INDS+TEAPOT_MAX_NUM_USER_MESH_INDICES];


#       ifdef TEAPOT_NO_MESH_PLANE_BACK_FACES
#           define TEAPOT_NO_MESH_PLANE_X_BACK_FACE
#           define TEAPOT_NO_MESH_PLANE_Y_BACK_FACE
#           define TEAPOT_NO_MESH_PLANE_Z_BACK_FACE
#       endif //TEAPOT_NO_MESH_PLANE_BACK_FACES

        int numTotVerts = 0;
        int numTotInds = 0;

        for (i=0;i<TEAPOT_FIRST_MESHLINES_INDEX;i++)    {
            switch (i)  {
            case TEAPOT_MESH_TEAPOT: {
#               ifndef TEAPOT_NO_MESH_TEAPOT
                {
                    const float A=0.0,B=0.250700,C=0.058345,D=0.074009,E=0.275580,F=0.499902,G=0.077794,H=0.323109,I=0.114399,J=0.400222,K=0.446657,L=0.283874,M=0.147190,N=0.354356,O=0.492338,P=0.301895,Q=0.280998,R=0.451085,S=0.018991,T=0.370789,U=0.074010,V=0.267919,W=0.496550,X=0.043050,Y=0.513757,Z=0.267918;
                    const float a=0.370788,b=0.027141,c=0.063789,d=0.048620,e=0.128358,f=0.046348,g=0.536983,h=0.049096,ia=0.079879,j=0.103696,k=0.368219,l=0.228473,m=0.062807,n=0.083726,o=0.048364,p=0.026758,q=0.108685,r=0.032211,s=0.260808,t=0.045581,u=0.471781,v=0.490958,w=0.612983,x=0.061079,y=0.038897,z=0.237484;
                    const float A0=0.429588,B0=0.637900,C0=0.558163,D0=0.480410,E0=0.503343,F0=0.271410,G0=0.624195,H0=0.214158,I0=0.452603,J0=0.265579,K0=0.387397,L0=0.088490,M0=0.588064,N0=0.044214,O0=0.413288,P0=0.250207,Q0=0.478425,R0=0.414636,S0=0.283835,T0=0.176415,U0=0.368787,V0=0.202521,W0=0.156909,X0=0.263135,Y0=0.446530,Z0=0.112000;
                    const float a0=0.111957,b0=0.343671,c0=0.203557,d0=0.145520,e0=0.189988,f0=0.061915,g0=0.115900,h0=0.042806,i0=0.319123,j0=0.692272,k0=0.125662,l0=0.767659,m0=0.117986,n0=0.116488,o0=1.041151,p0=0.018590,q0=0.854095,r0=0.049531,s0=0.131858,t0=1.008747,u0=1.059384,v0=0.094931,w0=0.034629,x0=0.081440,y0=0.997242,z0=0.810641;
                    const float A1=0.803313,B1=1.095039,C1=0.060940,D1=0.970112,E1=0.110297,F1=1.023730,G1=0.147227,H1=1.030483,I1=0.838979,J1=1.063723,K1=0.879919,L1=0.081769,M1=0.719234,N1=0.131416,O1=0.355378,P1=0.319747,Q1=0.154533,R1=0.154384,S1=0.284114,T1=0.720126,U1=0.896075,V1=0.171145,W1=0.266920,X1=0.887997,Y1=0.238750,Z1=0.229259;
                    const float a1=0.064436,b1=0.226999,c1=0.747979,d1=0.210578,e1=0.327100,f1=1.007859,g1=0.168850,h1=0.371574,i1=0.984833,j1=0.146072,k1=0.961806,l1=0.123294,m1=1.111246,n1=1.073557,o1=0.023758,p1=0.136816,q1=0.037991,r1=0.189126,s1=0.069148,t1=0.157746,u1=0.012093,v1=0.071513,w1=0.137798,x1=0.123105,y1=0.101150,z1=0.037948;
                    const float A2=0.122375,B2=0.100767,C2=0.177705,D2=0.182692,E2=0.157735,F2=0.153887,G2=0.120356,H2=0.118223,I2=0.046868,J2=0.185966,K2=0.245114,L2=0.393132,M2=0.357844,N2=0.250426,O2=0.442796,P2=0.276530,Q2=0.230920,R2=0.337143,S2=0.186008,T2=0.024913,U2=0.219531,V2=0.135925,W2=0.449471,X2=0.102407,Y2=0.209826,Z2=0.399768;
                    const float a2=0.082900,b2=0.128511,c2=0.285240,d2=0.995435,e2=1.078663,f2=0.275955,g2=1.006984,h2=0.273301,i2=0.051861,j2=0.935303,k2=0.270646,l2=1.071804,m2=0.268800,n2=0.265816,o2=0.267151,p2=0.060775,q2=0.919067,r2=0.265503,s2=0.264755,t2=1.034532,u2=0.963226,v2=0.064827,w2=0.891919,x2=0.005870,y2=0.010219,z2=0.112367;
                    const float A3=0.047251,B3=0.025643,C3=0.011202,D3=0.117770,E3=0.029795,F3=0.027661,G3=0.009717,H3=0.897548,I3=0.034674,J3=0.029684,K3=1.057518,L3=0.270483,M3=0.996652,N3=0.268149,O3=0.042949,P3=0.935787,Q3=0.429333,R3=0.277568,S3=0.554421,T3=0.711911,U3=0.536239,V3=0.270676,W3=0.521536,X3=0.264421,Y3=0.514310,Z3=0.397772;
                    const float a3=0.416948,b3=0.484152,c3=0.355578,d3=0.163476,e3=0.664480,f3=0.263996,g3=0.429763,h3=0.313386,i3=0.261345,j3=0.518619,k3=0.281743,l3=0.605054,m3=0.355755,n3=0.545629,o3=0.334817,p3=0.404416,q3=0.564967,r3=0.311493,s3=0.503599,t3=0.288169,u3=0.461407,v3=0.447526,w3=0.154464,x3=0.582094,y3=0.462973,z3=0.514055;
                    const float A4=0.176198,B4=0.355751,C4=0.462226,D4=0.344437,E4=0.637469,F4=0.196665,G4=0.550184,H4=0.378594,I4=0.191571,J4=0.676635,K4=0.563890,L4=0.140150,M4=0.190410,N4=0.197401,O4=0.691490,P4=0.440301,Q4=0.442230,R4=0.324215,S4=0.338876,T4=0.187336,U4=0.662075,V4=0.592626,W4=0.339588,X4=0.526614,Y4=0.698204,Z4=0.552434;
                    const float a4=0.183714,b4=0.113328,c4=0.440304,d4=0.115979,e4=0.269662,f4=0.131388,g4=0.793542,h4=0.132790,i4=0.621799,j4=0.157551,k4=0.804127,l4=0.264867,m4=0.338877,n4=0.159368,o4=0.609914,p4=0.462233,q4=0.814712,r4=0.185946,s4=0.598031,t4=0.372519,u4=0.366292,v4=0.187337,w4=0.116815,x4=0.121573,y4=0.919601,z4=0.144829;
                    const float A5=0.031203,B5=0.168085,C5=0.944857,D5=0.122658,E5=0.388220,F5=0.190412,G5=0.484044,H5=0.373518,I5=0.447531,J5=0.116403,K5=0.344440,L5=0.196667,M5=0.270429;

                    const float verts[] = {
                        0.057040,W,-D,A,F,-D,h0,W,-A5,-0.057040,W,-D,-h0,W,-A5,A,W,-0.131049,-h0,W,-w4,A,W,-0.016969,A,B,0.410034,v4,E,u4,e0,B,t4,m4,E,l4,b0,B,e4,Y0,B,d4,c4,E,b4,G5,B,-U,A,L,0.411113,A,E,0.403281,F5,L,H5,I5,L,J5,0.485129,L,-D,0.477295,E,-U,A,E,0.427050,L5,E,E5,K5,L,M5,p4,E,D5,0.501065,
                        E,-D,A,B,0.444610,c0,B,p3,m3,E,k3,Q0,B,0.129547,j3,B,-U,c4,E,-i3,Y0,B,-f3,m4,E,-0.412885,b0,B,-0.417680,e0,B,-0.520540,v4,E,-Y3,A,B,-0.558052,I5,L,-X3,F5,L,-W3,A,L,-0.559132,A,E,-0.551299,p4,E,-V3,K5,L,-0.418447,L5,E,-U3,A,E,-0.575070,Q0,B,-R3,m3,E,-g3,c0,B,-Z4,A,B,-V4,-T4,E,-Y3,-e0,B,-0.520539,-S4,E,
                        -0.412886,-b0,B,-0.417681,-Y0,B,-f3,-P4,E,-i3,-G5,B,-D,-M4,L,-W3,-v3,L,-X3,-0.485122,L,-D,-0.477292,E,-D,-F4,E,-U3,-D4,L,-0.418449,-C4,E,-V3,-0.501058,E,-D,-c0,B,-Z4,-B4,E,-g3,-Q0,B,-R3,-j3,B,-D,-P4,E,b4,-Y0,B,d4,-S4,E,l4,-b0,B,e4,-e0,B,t4,-T4,E,u4,-v3,L,J5,-M4,L,H5,-C4,E,D5,-D4,L,M5,-F4,E,E5,-Q0,B,0.129548,
                        -B4,E,k3,-c0,B,p3,A,I,0.508084,l,I,y3,k,B,0.294209,g,I,w3,x3,I,-D,A,-S,0.563460,P0,-S,z3,O0,I,0.339279,M0,-S,A4,E4,-S,-U,A,-M,0.602624,J0,-M,G4,I0,-S,H4,G0,-M,I4,J4,-M,-U,A,-V,0.617480,F0,-V,K4,D0,-M,0.406400,B0,-V,N4,O4,-V,-U,g,I,-0.302483,k,B,-Q4,l,I,-0.610992,A,I,-0.656103,M0,-S,-R4,O0,I,-0.487296,
                        P0,-S,-U4,A,-S,-0.711478,G0,-M,-W4,I0,-S,-X4,J0,-M,-Y4,A,-M,-0.750645,B0,-Z,-0.345420,D0,-M,-S3,F0,-Z,-T3,A,-Z,-0.765501,-l,I,-0.610991,-k,B,-Q4,-g,I,-0.302482,-x3,I,-D,-P0,-S,-U4,-O0,I,-0.487297,-M0,-S,-R4,-E4,-S,-D,-J0,-M,-Y4,-I0,-S,-X4,-G0,-M,-W4,-J4,-M,-D,-F0,-Z,-T3,-D0,-M,-S3,-B0,-Z,-0.345419,-O4,
                        -V,-D,-g,I,w3,-k,B,0.294210,-l,I,y3,-M0,-S,A4,-O0,I,0.339277,-P0,-S,z3,-G0,-M,I4,-I0,-S,H4,-J0,-M,G4,-B0,-V,N4,-D0,-M,0.406401,-F0,-V,K4,s,-T,0.538971,u,-T,Z3,v,-V,a3,w,-T,0.186797,A,-T,0.590469,z,-K,b3,A0,-K,c3,C0,-K,d3,e3,-T,-U,A,-K,0.531045,H0,-F,Q3,K0,-F,h3,E0,-F,L4,l3,-K,-D,A,-F,0.471620,A,-F,-U,
                        n3,-F,-D,w,-T,-o3,u,-a,-0.545791,v,-Z,-q3,s,-a,-0.686992,C0,-K,-r3,A0,-K,-s3,z,-K,-0.632173,A,-a,-0.738490,E0,-F,-t3,K0,-F,-u3,H0,-F,-0.577353,A,-K,-0.679063,A,-F,-0.619640,-s,-a,-0.686991,-u,-a,-0.545790,-v,-Z,-q3,-w,-T,-o3,-z,-K,-0.632172,-A0,-K,-s3,-C0,-K,-r3,-e3,-T,-D,-H0,-F,-0.577351,-K0,-F,-u3,
                        -E0,-F,-t3,-l3,-K,-D,-n3,-F,-D,-w,-T,0.186799,-u,-T,Z3,-v,-V,a3,-s,-T,0.538973,-C0,-K,d3,-A0,-K,c3,-z,-K,b3,-E0,-F,L4,-K0,-F,h3,-H0,-F,Q3,A,0.120708,-0.627202,C,f4,-g4,C,h4,-i4,G,j4,-k4,G,n4,-o4,C,a4,-q4,C,r4,-s4,A,0.195607,-0.819523,A,0.198028,-V4,A,0.111001,-0.908121,A,0.119495,-0.788730,C,x4,-y4,G,
                        z4,-C5,C,B5,-D1,A,0.087947,-0.982133,C,v0,-y0,G,E1,-H1,C,k0,-J1,A,0.178655,-0.981592,A,X,-1.007523,C,X,-F1,G,X,-u0,C,X,-B1,A,0.132647,-1.078832,-C,r4,-s4,-C,a4,-q4,-G,n4,-o4,-G,j4,-k4,-C,h4,-i4,-C,f4,-g4,-C,B5,-D1,-G,z4,-C5,-C,x4,-y4,-C,k0,-J1,-G,E1,-H1,-C,v0,-y0,-C,X,-B1,-G,X,-u0,-C,X,-F1,A,-0.027854,
                        -0.994017,C,-w0,-t0,G,-r0,-o0,C,-a1,-n1,A,X,-m1,A,-0.112939,-0.951339,C,-l1,-k1,G,-j1,-i1,C,-g1,-f1,A,-0.071209,-1.088285,A,-0.197773,-0.876247,C,-d1,-K1,G,-Y1,-X1,C,-W1,-U1,A,-0.179203,-1.018327,C,-S1,-0.743002,G,-P1,-0.731119,C,-O1,-M1,A,-0.279725,-0.899747,-C,-a1,-n1,-G,-r0,-o0,-C,-w0,-t0,-C,-g1,-f1,
                        -G,-j1,-i1,-C,-l1,-k1,-C,-W1,-U1,-G,-Y1,-X1,-C,-d1,-K1,-C,-O1,-M1,-G,-P1,-0.731117,-C,-S1,-0.743001,A,-0.265781,-0.751779,A,-0.086943,Y,g0,-L1,j0,e,-N1,Y,Q1,-R1,T1,V1,-Z1,Y,g0,-b1,c1,e,-e1,Y,A,-0.260004,0.760646,A,-h1,Y,A,-0.048762,0.679605,L0,o1,l0,m0,-p0,A1,L0,-C1,I1,A,-0.080189,0.855188,A,0.043007,
                        0.751450,x,G1,z0,x0,s0,q0,x,n0,H3,A,0.109502,0.917311,A,B,0.859502,A,0.154213,0.790893,d,B,w2,v2,B,u2,d,B,t2,-e,-e1,Y,-g0,-b1,c1,-V1,-Z1,Y,-Q1,-R1,T1,-e,-N1,Y,-g0,-L1,j0,-L0,-C1,I1,-m0,-p0,A1,-L0,o1,l0,-x,n0,H3,-x0,s0,q0,-x,G1,z0,-d,B,t2,-v2,B,u2,-d,B,w2,A,s2,0.884354,t,r2,q2,p2,o2,d2,t,m2,l2,A,B,1.066949,
                        A,0.269440,0.902720,y,k2,j2,i2,h2,g2,y,f2,e2,A,0.269549,1.106517,r,n2,P3,O3,N3,M3,r,L3,K3,A,0.271544,1.085177,A,0.277162,m1,A,s2,0.908128,A,0.112402,0.864452,-t,m2,l2,-p2,o2,d2,-t,r2,q2,-y,f2,e2,-i2,h2,g2,-y,k2,j2,-r,L3,K3,-O3,N3,M3,-r,n2,P3,A,O,0.043759,N0,R,J3,f,O,I3,ia,R,x2,n,O,G3,q,O,-F3,j,R,-E3,
                        D3,O,-D,A,R,0.038358,p,J,-C3,o,J,-B3,m,J,-A3,z2,R,-D,A,J,-0.005940,b,N,-y2,h,N,-T2,c,N,-I2,0.068069,J,-D,j,R,-H2,q,O,-G2,ia,R,-F2,n,O,-E2,f,O,-D2,N0,R,-C2,A,O,-0.191779,m,J,-B2,o,J,-A2,p,J,-p1,A,R,-0.186376,c,N,-y1,h,N,-x1,b,N,-w1,A,J,-0.142078,-N0,R,-C2,-f,O,-D2,-ia,R,-F2,-n,O,-E2,-q,O,-G2,-j,R,-H2,
                        -D3,O,-D,-p,J,-p1,-o,J,-A2,-m,J,-B2,-z2,R,-D,-b,N,-w1,-h,N,-x1,-c,N,-y1,-0.068067,J,-D,-j,R,-E3,-q,O,-F3,-ia,R,x2,-n,O,G3,-f,O,I3,-N0,R,J3,-m,J,-A3,-o,J,-B3,-p,J,-C3,-c,N,-I2,-h,N,-T2,-b,N,-y2,A,H,0.083737,A,N,-0.004860,f0,H,v1,d0,H,-u1,t1,H,-D,s1,N,-D,A,P,0.211231,a0,P,r1,Z0,H,q1,X0,P,z1,c2,P,-D,A,Q,
                        0.325758,W0,Q,0.294775,V0,P,b2,U0,Q,a2,Z2,Q,-U,A,B,0.375459,T0,B,0.340624,S0,Q,Y2,R0,B,X2,W2,B,-U,d0,H,-V2,f0,H,-U2,A,H,-0.231756,A,N,-0.143158,X0,P,-J2,Z0,H,-S2,a0,P,-R2,A,P,-0.359249,U0,Q,-Q2,V0,P,-P2,W0,Q,-O2,A,Q,-0.473776,R0,B,-N2,S0,Q,-M2,T0,B,-0.488646,A,B,-0.523477,-f0,H,-U2,-d0,H,-V2,-t1,H,-D,
                        -s1,N,-D,-a0,P,-R2,-Z0,H,-S2,-X0,P,-J2,-c2,P,-D,-W0,Q,-O2,-V0,P,-P2,-U0,Q,-Q2,-Z2,Q,-D,-T0,B,-0.488645,-S0,Q,-M2,-R0,B,-N2,-W2,B,-D,-d0,H,-u1,-f0,H,v1,-X0,P,z1,-Z0,H,q1,-a0,P,r1,-U0,Q,a2,-V0,P,b2,-W0,Q,0.294777,-R0,B,X2,-S0,Q,Y2,-T0,B,0.340625,h0,W,-w4,-i0,B,-L2,-i0,B,K2,i0,B,K2,i0,B,-L2,A,-h1,-0.713830
                    };

                    const unsigned short inds[] = {
                        0,1,2,1,3,4,1,5,6,7,1,4,8,9,10,10,11,12,11,13,12,14,15,13,16,9,17,18,11,9,11,19,14,14,20,21,22,18,16,23,24,18,24,25,19,19,26,20,27,23,22,28,29,23,29,30,25,25,31,26,15,32,33,
                        33,34,35,34,36,35,37,38,36,20,32,21,39,34,32,34,40,37,37,41,42,26,39,20,43,44,39,44,45,40,40,46,41,31,43,26,47,48,43,48,49,45,45,50,46,38,51,52,52,53,54,53,55,54,56,57,55,41,51,42,58,53,51,
                        53,59,56,56,60,61,46,58,41,62,63,58,63,64,59,59,65,60,50,62,46,66,67,62,67,68,64,64,69,65,57,70,71,71,72,73,72,74,73,75,8,74,60,70,61,76,72,70,72,77,75,75,16,17,65,76,60,78,79,76,79,80,77,
                        77,22,16,69,78,65,81,82,78,82,83,80,80,27,22,84,28,27,85,86,28,86,87,30,30,88,31,89,85,84,90,91,85,91,92,87,87,93,88,94,90,89,95,96,90,96,97,92,92,98,93,99,95,94,100,101,95,101,102,97,97,103,98,
                        88,47,31,104,105,47,105,106,49,49,107,50,93,104,88,108,109,104,109,110,106,106,111,107,98,108,93,112,113,108,113,114,110,110,115,111,103,112,98,116,117,112,117,118,114,114,119,115,107,66,50,120,121,66,121,122,68,68,123,69,111,120,107,
                        124,125,120,125,126,122,122,127,123,115,124,111,128,129,124,129,130,126,126,131,127,119,128,115,132,133,128,133,134,130,130,135,131,123,81,69,136,137,81,137,138,83,83,84,27,127,136,123,139,140,136,140,141,138,138,89,84,131,139,127,142,143,139,
                        143,144,141,141,94,89,135,142,131,145,146,142,146,147,144,144,99,94,99,148,100,100,149,150,149,102,150,151,103,102,152,153,148,148,154,149,154,151,149,155,156,151,157,158,153,153,159,154,159,155,154,160,161,155,162,163,158,158,163,159,159,163,160,
                        160,163,164,103,165,116,116,166,167,166,118,167,168,119,118,156,169,165,165,170,166,170,168,166,171,172,168,161,173,169,169,174,170,174,171,170,175,176,171,164,163,173,173,163,174,174,163,175,175,163,177,119,178,132,132,179,180,179,134,180,181,135,134,
                        172,182,178,178,183,179,183,181,179,184,185,181,176,186,182,182,187,183,187,184,183,188,189,184,177,163,186,186,163,187,187,163,188,188,163,190,135,191,145,145,192,193,192,147,193,194,99,147,185,195,191,191,196,192,196,194,192,197,152,194,189,198,195,
                        195,199,196,199,197,196,200,157,197,190,163,198,198,163,199,199,163,200,200,163,162,201,202,203,203,204,205,205,206,207,207,208,209,210,202,211,212,204,202,213,206,204,214,208,206,215,212,210,216,213,212,217,214,213,218,219,214,220,216,215,221,217,216,
                        222,218,217,223,224,218,208,225,209,226,227,225,228,229,227,230,201,229,208,231,226,226,232,228,228,233,230,230,210,211,219,234,231,231,235,232,232,236,233,233,215,210,224,237,234,234,238,235,235,239,236,236,220,215,240,221,220,241,222,221,242,223,222,
                        243,244,223,245,241,240,246,242,241,247,243,242,248,249,243,250,246,245,251,247,246,252,248,247,253,254,248,255,252,251,256,253,252,257,258,253,244,259,237,237,260,238,238,261,239,239,240,220,249,262,259,259,263,260,260,264,261,261,245,240,254,265,262,
                        262,266,263,263,267,264,264,250,245,258,268,265,265,269,266,266,270,267,267,271,250,272,273,274,274,275,276,276,277,278,278,279,280,281,282,273,273,283,275,275,284,277,277,285,279,286,287,282,282,288,283,283,289,284,284,290,285,291,287,292,293,288,287,
                        294,289,288,295,290,289,279,296,280,297,298,296,299,300,298,301,272,300,285,297,279,302,299,297,303,301,299,304,281,301,290,302,285,305,303,302,306,304,303,307,286,304,290,308,305,305,309,306,306,310,307,307,291,292,311,293,291,312,294,293,313,295,294,
                        314,315,295,316,312,311,317,313,312,318,314,313,319,320,314,316,321,317,317,322,318,318,323,319,319,324,325,326,327,321,321,327,322,322,327,323,323,327,324,315,328,308,308,329,309,309,330,310,310,311,291,320,331,328,328,332,329,329,333,330,330,316,311,
                        324,331,325,334,332,331,335,333,332,336,316,333,324,327,334,334,327,335,335,327,336,336,327,326,337,338,339,339,340,341,340,342,341,343,344,342,345,346,338,338,347,340,347,343,340,348,349,343,350,351,346,346,352,347,352,348,347,353,354,348,344,355,356,
                        356,357,358,357,359,358,360,361,359,349,362,355,355,363,357,363,360,357,364,365,360,354,366,362,362,367,363,367,364,363,368,369,364,361,370,371,371,372,373,372,374,373,375,376,374,365,377,370,370,378,372,378,375,372,379,380,375,369,381,377,377,382,378,
                        382,379,378,383,384,379,376,385,386,386,387,388,387,389,388,390,337,389,380,391,385,385,392,387,392,390,387,393,345,390,384,394,391,391,395,392,395,393,392,396,350,393,397,351,398,399,352,351,352,400,353,353,401,402,403,399,397,404,405,399,405,406,400,
                        400,407,401,408,404,403,409,410,404,410,411,406,406,412,407,413,409,408,414,415,409,415,416,411,411,417,412,401,366,402,418,367,366,367,419,368,368,420,421,407,418,401,422,423,418,423,424,419,419,425,420,412,422,407,426,427,422,427,428,424,424,429,425,
                        417,426,412,430,431,426,431,432,428,428,433,429,420,381,421,434,382,381,382,435,383,383,436,437,425,434,420,438,439,434,439,440,435,435,441,436,429,438,425,442,443,438,443,444,440,440,445,441,433,442,429,446,447,442,447,448,444,444,449,445,436,394,437,
                        450,395,394,395,451,396,396,397,398,441,450,436,452,453,450,453,454,451,451,403,397,445,452,441,455,456,452,456,457,454,454,408,403,449,455,445,458,459,455,459,460,457,457,413,408,0,356,461,359,5,461,342,0,2,7,339,2,389,7,4,3,386,4,
                        374,3,6,5,371,6,2,341,342,461,358,359,6,373,374,4,388,389,271,251,250,38,446,433,52,462,446,462,55,448,448,57,449,57,458,449,71,463,458,463,74,460,460,8,413,8,414,413,10,464,414,464,13,416,416,15,417,15,430,417,33,465,430,
                        465,36,432,432,38,433,0,461,1,1,6,3,1,461,5,7,2,1,8,17,9,10,9,11,11,14,13,14,21,15,16,18,9,18,24,11,11,24,19,14,19,20,22,23,18,23,29,24,24,29,25,19,25,26,27,28,23,28,86,29,29,86,30,
                        25,30,31,15,21,32,33,32,34,34,37,36,37,42,38,20,39,32,39,44,34,34,44,40,37,40,41,26,43,39,43,48,44,44,48,45,40,45,46,31,47,43,47,105,48,48,105,49,45,49,50,38,42,51,52,51,53,53,56,55,56,61,57,
                        41,58,51,58,63,53,53,63,59,56,59,60,46,62,58,62,67,63,63,67,64,59,64,65,50,66,62,66,121,67,67,121,68,64,68,69,57,61,70,71,70,72,72,75,74,75,17,8,60,76,70,76,79,72,72,79,77,75,77,16,65,78,76,
                        78,82,79,79,82,80,77,80,22,69,81,78,81,137,82,82,137,83,80,83,27,84,85,28,85,91,86,86,91,87,30,87,88,89,90,85,90,96,91,91,96,92,87,92,93,94,95,90,95,101,96,96,101,97,92,97,98,99,100,95,100,150,101,
                        101,150,102,97,102,103,88,104,47,104,109,105,105,109,106,49,106,107,93,108,104,108,113,109,109,113,110,106,110,111,98,112,108,112,117,113,113,117,114,110,114,115,103,116,112,116,167,117,117,167,118,114,118,119,107,120,66,120,125,121,121,125,122,
                        68,122,123,111,124,120,124,129,125,125,129,126,122,126,127,115,128,124,128,133,129,129,133,130,126,130,131,119,132,128,132,180,133,133,180,134,130,134,135,123,136,81,136,140,137,137,140,138,83,138,84,127,139,136,139,143,140,140,143,141,138,141,89,
                        131,142,139,142,146,143,143,146,144,141,144,94,135,145,142,145,193,146,146,193,147,144,147,99,99,152,148,100,148,149,149,151,102,151,156,103,152,157,153,148,153,154,154,155,151,155,161,156,157,162,158,153,158,159,159,160,155,160,164,161,103,156,165,
                        116,165,166,166,168,118,168,172,119,156,161,169,165,169,170,170,171,168,171,176,172,161,164,173,169,173,174,174,175,171,175,177,176,119,172,178,132,178,179,179,181,134,181,185,135,172,176,182,178,182,183,183,184,181,184,189,185,176,177,186,182,186,187,
                        187,188,184,188,190,189,135,185,191,145,191,192,192,194,147,194,152,99,185,189,195,191,195,196,196,197,194,197,157,152,189,190,198,195,198,199,199,200,197,200,162,157,201,211,202,203,202,204,205,204,206,207,206,208,210,212,202,212,213,204,213,214,206,
                        214,219,208,215,216,212,216,217,213,217,218,214,218,224,219,220,221,216,221,222,217,222,223,218,223,244,224,208,226,225,226,228,227,228,230,229,230,211,201,208,219,231,226,231,232,228,232,233,230,233,210,219,224,234,231,234,235,232,235,236,233,236,215,
                        224,244,237,234,237,238,235,238,239,236,239,220,240,241,221,241,242,222,242,243,223,243,249,244,245,246,241,246,247,242,247,248,243,248,254,249,250,251,246,251,252,247,252,253,248,253,258,254,255,256,252,256,257,253,257,466,258,244,249,259,237,259,260,
                        238,260,261,239,261,240,249,254,262,259,262,263,260,263,264,261,264,245,254,258,265,262,265,266,263,266,267,264,267,250,258,466,268,265,268,269,266,269,270,267,270,271,272,281,273,274,273,275,276,275,277,278,277,279,281,286,282,273,282,283,275,283,284,
                        277,284,285,286,292,287,282,287,288,283,288,289,284,289,290,291,293,287,293,294,288,294,295,289,295,315,290,279,297,296,297,299,298,299,301,300,301,281,272,285,302,297,302,303,299,303,304,301,304,286,281,290,305,302,305,306,303,306,307,304,307,292,286,
                        290,315,308,305,308,309,306,309,310,307,310,291,311,312,293,312,313,294,313,314,295,314,320,315,316,317,312,317,318,313,318,319,314,319,325,320,316,326,321,317,321,322,318,322,323,319,323,324,315,320,328,308,328,329,309,329,330,310,330,311,320,325,331,
                        328,331,332,329,332,333,330,333,316,324,334,331,334,335,332,335,336,333,336,326,316,337,345,338,339,338,340,340,343,342,343,349,344,345,350,346,338,346,347,347,348,343,348,354,349,350,398,351,346,351,352,352,353,348,353,402,354,344,349,355,356,355,357,
                        357,360,359,360,365,361,349,354,362,355,362,363,363,364,360,364,369,365,354,402,366,362,366,367,367,368,364,368,421,369,361,365,370,371,370,372,372,375,374,375,380,376,365,369,377,370,377,378,378,379,375,379,384,380,369,421,381,377,381,382,382,383,379,
                        383,437,384,376,380,385,386,385,387,387,390,389,390,345,337,380,384,391,385,391,392,392,393,390,393,350,345,384,437,394,391,394,395,395,396,393,396,398,350,397,399,351,399,405,352,352,405,400,353,400,401,403,404,399,404,410,405,405,410,406,400,406,407,
                        408,409,404,409,415,410,410,415,411,406,411,412,413,414,409,414,464,415,415,464,416,411,416,417,401,418,366,418,423,367,367,423,419,368,419,420,407,422,418,422,427,423,423,427,424,419,424,425,412,426,422,426,431,427,427,431,428,424,428,429,417,430,426,
                        430,465,431,431,465,432,428,432,433,420,434,381,434,439,382,382,439,435,383,435,436,425,438,434,438,443,439,439,443,440,435,440,441,429,442,438,442,447,443,443,447,444,440,444,445,433,446,442,446,462,447,447,462,448,444,448,449,436,450,394,450,453,395,
                        395,453,451,396,451,397,441,452,450,452,456,453,453,456,454,451,454,403,445,455,452,455,459,456,456,459,457,454,457,408,449,458,455,458,463,459,459,463,460,457,460,413,0,344,356,359,361,5,342,344,0,7,337,339,389,337,7,3,376,386,374,376,3,
                        5,361,371,2,339,341,461,356,358,6,371,373,4,386,388,271,255,251,38,52,446,52,54,462,462,54,55,448,55,57,57,71,458,71,73,463,463,73,74,460,74,8,8,10,414,10,12,464,464,12,13,416,13,15,15,33,430,33,35,465,465,35,36,
                        432,36,38};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_BUNNY: {
#               ifndef TEAPOT_NO_MESH_BUNNY
                {
                    const float A=0.499120,B=0.499905,C=0.499116,D=0.320756,E=0.499121,F=0.683914,G=0.497743,H=0.499114;

                    const float verts[] = {
                        -0.005326,-0.147601,0.625450,-0.005546,-0.139797,0.663273,-0.060312,-0.097053,0.658934,0.049399,-0.097219,0.659606,-0.005563,-0.067678,F,0.124974,0.032568,0.537897,0.118797,0.064318,0.519466,0.119200,0.058246,0.544451,-0.134000,0.032960,0.536311,-0.113666,0.008720,0.555753,-0.120334,
                        0.037640,0.562773,0.110999,0.037291,0.564190,0.104331,0.008391,0.557089,-0.128230,0.058620,0.542936,-0.118585,0.003714,0.539146,0.122726,0.015638,0.514850,0.118861,0.050850,0.497373,-0.127502,0.064690,0.517958,-0.131521,0.016022,0.513293,0.109437,0.003370,0.540543,-0.127335,0.051222,0.495865,0.136776,
                        0.292841,0.384904,0.130910,0.285500,0.342629,0.137174,0.325360,0.311767,0.146967,0.352588,0.353097,-0.168932,0.414408,0.301237,-0.190199,0.434949,0.244993,-0.154896,0.355935,0.289227,-0.136779,0.285904,0.340990,-0.143141,0.293264,0.383189,-0.152761,0.353040,0.351262,-0.142543,0.325782,0.310054,0.163935,
                        0.413905,0.303276,0.149871,0.355475,0.291093,0.185951,0.434380,0.247296,0.145330,0.175559,0.427091,0.162452,0.267513,0.325697,-0.152564,0.176009,0.425267,-0.168166,0.268013,0.323672,-0.188663,0.394252,0.241677,0.184333,0.393688,0.243960,0.180278,0.327873,0.268389,-0.185106,0.328425,0.266151,0.061550,
                        0.153321,-0.530988,0.076663,0.176933,-0.572437,0.002538,0.212315,-0.569922,0.002236,0.177176,-0.529208,0.080589,0.084343,-0.678480,0.031129,0.041548,-0.679584,0.062379,0.140834,-0.657305,0.003036,0.157284,-0.664987,0.002973,0.046359,-0.681928,-0.025224,0.041634,-0.679930,-0.056445,0.141014,-0.658033,
                        -0.120976,-0.030224,-0.630149,-0.127903,0.029817,-0.584012,-0.119430,0.032701,-0.657161,0.002554,-0.026922,-0.631542,-0.035336,-0.028018,-0.631098,-0.064204,0.036900,-0.677897,-0.074832,-0.029121,-0.630623,0.097432,0.108237,-0.598019,0.090001,0.113064,-0.545099,0.070066,0.036697,-0.677075,0.079915,-0.029355,
                        -0.629675,0.040432,-0.028132,-0.630634,-0.074564,0.084577,-0.679430,-0.085522,0.113329,-0.546173,-0.092319,0.108523,-0.599181,-0.071657,0.177157,-0.573346,0.125021,0.032332,-0.655664,0.126047,-0.030598,-0.628636,0.132589,0.029424,-0.582416,-0.057125,0.153501,-0.531715,-0.281047,0.054272,-0.326657,-0.289679,
                        0.075437,-0.212368,-0.184106,0.209299,-0.222753,-0.171339,0.186400,-0.322165,-0.164703,0.199932,-0.140366,-0.141192,0.159338,0.045313,-0.001190,0.252155,0.048661,-0.000037,0.290949,-0.130016,-0.174526,0.138986,-0.454281,0.001191,0.264893,-0.337106,0.001862,0.222300,-0.457061,0.000730,0.296135,-0.254187,
                        -0.245046,0.065927,-0.121983,-0.234104,0.053911,0.023500,-0.237908,-0.066725,-0.482486,-0.152235,-0.126037,-0.633994,-0.115314,0.126627,0.123694,-0.001876,0.182151,0.143412,-0.177588,0.026660,0.144913,0.244173,0.065189,-0.118987,0.289939,0.074562,-0.208819,0.184905,0.208743,-0.220494,0.164466,0.199435,
                        -0.138349,0.241050,-0.067448,-0.479553,0.157061,-0.126503,-0.632100,0.177949,0.138453,-0.452124,0.250122,-0.177727,0.123656,0.196050,-0.199430,-0.000811,0.242635,-0.129938,0.083742,0.199003,-0.230305,-0.062507,0.205738,-0.233363,-0.137763,0.257917,-0.108345,-0.126910,0.257603,-0.102724,-0.024105,0.232539,
                        -0.433881,-0.339609,0.265654,-0.475224,-0.259346,0.199283,-0.466998,-0.425337,0.127566,-0.205535,0.333254,-0.003891,-0.259620,0.363483,-0.003352,-0.295387,0.266658,0.127772,-0.237416,0.274235,0.062728,-0.328777,-0.050642,0.079827,-0.336149,-0.105039,0.000198,-0.352302,-0.327120,0.085926,-0.441323,-0.322447,
                        0.051360,-0.327746,-0.172376,0.265290,-0.471302,-0.169849,0.260760,-0.451076,-0.240370,0.233744,-0.407344,-0.163808,0.039899,-0.336571,-0.049210,-0.001117,-0.356544,-0.113517,0.124334,-0.391360,0.208957,0.155125,-0.296774,0.183402,0.075680,-0.352388,0.192189,0.190897,-0.498582,0.269451,0.177128,-0.498543,
                        0.306824,0.139141,-0.498427,0.335966,0.082624,-0.498260,0.314472,0.035934,-0.498119,0.240176,-0.004366,-0.186127,0.459205,0.046942,-0.187283,0.463321,0.032034,-0.177715,0.509900,-0.004648,-0.171651,0.508707,0.171696,0.040099,0.442045,0.171679,0.023802,0.447166,0.205390,-0.001310,0.416503,-0.005120,-0.173144,
                        0.585529,0.013197,-0.184091,0.553572,0.013787,-0.166117,0.596733,0.051115,-0.156804,0.622179,0.087701,-0.110714,0.636777,0.086082,-0.063101,0.636396,0.040761,-0.012835,0.656814,0.097011,-0.015912,0.594033,0.103024,0.048542,0.597409,0.079517,0.058508,0.617072,0.114271,0.037621,0.577971,0.104448,0.001223,
                        0.574035,0.137325,0.075374,0.462314,0.135830,0.245557,0.453480,0.105194,0.163531,0.489091,0.129234,0.050857,0.488433,0.213282,0.499256,0.194086,0.181515,0.485771,0.329617,0.056237,0.236086,0.451189,0.070471,0.277036,0.436495,0.035845,0.268199,0.377224,0.109290,0.371312,0.238490,0.106326,0.333005,0.243748,
                        0.084608,0.361313,0.316120,0.162810,0.326732,0.446023,0.181065,0.406156,0.399669,0.100215,0.385059,0.367970,0.083064,0.332158,0.405988,0.162999,0.177038,0.389596,0.152055,0.108612,0.410181,0.068624,0.180391,0.326281,0.063008,0.220484,0.309278,0.193006,-0.051877,0.198542,0.173332,0.026130,0.147062,0.188527,
                        0.048564,0.289780,0.053456,-0.499274,-0.439014,0.082043,-0.497446,-0.321138,0.063861,-0.443539,-0.440244,0.057765,-0.499291,-0.221558,0.074159,-0.499343,-0.147787,0.081066,-0.453984,-0.149058,0.064705,-0.443079,-0.222830,0.192162,-0.232395,-0.481292,0.144180,-0.247934,-0.603770,0.079513,-0.499353,-0.503828,
                        0.083082,-0.442371,-0.508676,0.133881,-0.457891,-0.519984,0.131514,-0.499512,-0.515121,0.168328,-0.414539,-0.424309,0.121277,-0.405106,-0.471814,0.103696,-0.341721,-0.571274,0.152673,-0.349502,-0.528593,-0.166915,-0.414033,-0.426363,-0.183343,-0.325647,-0.444820,-0.149789,-0.349046,-0.530445,-0.177186,
                        -0.419475,-0.406013,-0.062350,-0.443349,-0.441016,-0.052129,-H,-0.439660,-0.077391,-C,-0.504788,-0.080728,-0.442124,-0.509679,-0.095674,-0.121240,-F,0.002334,-0.101605,-0.614195,0.001762,-0.232855,-0.552971,-0.035458,-0.238852,-0.628699,-0.038556,-0.123049,-0.673901,-0.191261,-A,-0.429739,-0.259685,-E,
                        -0.264573,-0.266438,-0.474421,-0.262604,-0.198014,-0.466397,-0.427770,-0.181108,-A,-0.475449,-0.129250,-0.499118,-0.516718,-0.082152,-0.497198,-0.322144,-0.194459,0.049143,0.287434,-0.080790,0.125935,0.247442,-0.198028,0.249360,0.290466,-0.097024,0.269580,0.274922,-0.068671,0.220683,0.308471,-0.113626,
                        0.371648,0.237124,-0.218670,0.426640,0.178958,-0.216681,B,0.191454,-0.077521,0.277260,0.435589,-0.169818,0.327235,0.443986,-0.143178,0.245979,0.451772,-0.074616,0.180607,0.325404,-0.003350,0.235827,0.397391,-0.042197,0.268317,0.376746,-0.064074,0.312509,0.346878,-0.089572,0.332419,0.404930,-0.187264,
                        0.406712,0.397413,-0.186617,0.486327,0.327363,-0.134306,0.006169,0.507234,-0.159285,0.109082,0.408274,-0.145295,0.075801,0.460583,-0.169769,0.177541,0.387557,-0.106873,-0.015605,0.592785,-0.114013,0.001553,0.572697,-0.121838,-0.013177,0.545736,-0.117158,-0.036849,0.557649,-0.089512,0.135121,0.574423,
                        -0.062717,0.054062,0.636786,-0.005158,0.073721,0.652625,-0.004641,0.158550,0.589155,-0.098373,-0.110433,0.635637,-0.096606,-0.062825,0.635277,-0.061751,-0.156634,0.621487,-0.023078,-0.184037,0.553350,-0.024143,-0.166059,0.596501,-0.188526,0.095747,0.328633,-0.213025,-0.000678,0.413941,-0.179522,0.040630,
                        0.439894,-0.055724,-0.187128,0.462692,-0.103550,-0.155305,0.463592,-0.118620,-0.146677,0.402488,-0.056350,-0.186626,0.411631,-0.090513,-0.497747,0.312389,-0.042917,-0.497749,0.238670,-0.036598,-0.406081,0.250420,-0.132928,-0.367311,0.269885,-0.147290,-G,0.333189,-0.235495,-0.406635,-0.166682,-0.261705,
                        -0.450287,-0.243570,-0.267159,-0.470498,-0.173109,-0.083152,-0.453736,-0.150064,-0.065855,-0.442881,-0.223629,-0.113383,-0.368585,-0.222894,-0.146576,-0.394012,-0.112895,-0.052782,-0.327589,-0.173014,-0.081421,-0.303510,-0.150225,-0.207285,-0.232739,-0.140292,-0.201464,-0.229700,-0.064958,-0.190682,-0.218929,
                        -0.018404,-0.199174,-0.198833,-0.003231,-0.230331,-0.106290,0.041787,-0.260143,-0.101942,-0.027276,-0.191371,-0.430642,0.159888,-0.203467,-0.239652,0.055062,-0.197084,-0.334939,0.019019,-0.180289,-0.497752,0.178537,-0.279229,-0.255122,-0.280191,-D,-0.102652,-0.235287,-0.311994,-0.120316,-0.342527,-0.246580,
                        -0.129199,0.080746,-0.254700,-0.176965,0.120565,-0.209549,-0.145738,0.236471,-0.198126,-0.051286,0.196146,-0.244589,-0.269896,-0.374270,-0.189501,-0.231819,-0.483629,-0.160800,-0.296297,0.181467,-0.196407,-0.403209,0.254206,-0.184918,-0.497746,0.303584,-0.185459,-0.227219,0.005551,-0.170806,-0.360279,
                        -0.187845,-0.085849,-0.441064,-0.323499,-0.000554,-0.341289,-0.201640,-0.082095,-0.335904,-0.106031,-0.081638,-0.352150,0.191226,-0.002377,-0.369012,0.089170,-0.002885,-0.345467,0.178067,-0.134384,-0.237019,0.272630,-0.020657,-0.431673,0.169698,-0.041415,-0.497750,0.206240,-0.004097,-0.199710,0.411820,
                        -0.041360,-0.177604,0.509452,-0.004808,-0.173531,0.534301,-0.115960,-0.103451,0.601792,-0.096219,-0.152051,0.588691,-0.051387,-0.012696,0.656250,-0.089438,0.058764,0.616037,-0.112733,0.048868,0.596087,-0.123773,0.037980,0.576514,-0.113229,0.163861,0.487752,-0.131533,0.073086,0.546432,-0.136044,0.077254,
                        0.522242,-0.211983,0.331671,0.229941,-0.063591,0.236266,0.450455,-0.115513,0.419500,0.320538,-0.121951,0.421298,0.268127,-0.091861,0.148905,0.281343,-0.205977,-0.122629,0.270677,-0.134805,-0.205139,0.331647,-0.162760,-0.122236,0.387249,-0.201695,-0.042184,0.348775,-0.187861,-0.456200,-0.473480,-0.140069,
                        -0.247505,-0.605510,-0.088689,-0.241529,-0.642742,-0.100270,-0.341413,-0.572523,-0.071600,-0.395452,-0.467013,-0.119259,-0.404743,-0.473287,-0.056064,-0.337101,-0.558613,0.178331,-0.420012,-0.403836,0.185247,-0.326204,-0.442564,0.101130,-0.121538,-0.682709,0.043888,-0.123174,-0.673396,0.039887,-0.238966,
                        -0.628237,0.258851,-B,-0.261398,0.192454,-0.499700,-0.427389,0.258475,-B,-0.171901,0.231474,-0.499824,-0.071233,0.155526,-0.499591,-0.044702,0.075588,0.125699,0.248400,0.182229,0.095186,0.330904,0.117384,0.420936,0.269592,0.058219,0.312324,0.347628,0.127325,0.076856,0.523855,0.122505,0.072703,0.547987,
                        0.120612,0.097839,0.566866,0.052531,0.053888,0.637492,0.105723,-0.103785,0.603150,0.119919,-0.079262,0.568615,0.107661,-0.037189,0.559026,0.085996,-0.152326,0.589806,0.034008,-0.166103,0.563629,0.169029,-0.095405,0.506545,0.146009,-0.024881,0.514847,0.155089,-0.122716,0.389195,0.128765,-0.134192,0.486288,
                        0.094848,-0.155604,0.464807,0.110692,-0.147023,0.403893,0.088450,-0.391918,0.036306,0.193277,-0.335528,0.021410,0.174032,-0.498539,0.181729,0.014720,-0.431978,0.170936,0.029748,-0.406432,0.251850,0.125950,-0.367953,0.272492,0.144211,-0.394451,-0.111114,0.219802,-0.388971,-0.241891,0.112443,-0.368926,
                        -0.221511,0.079791,-0.303753,-0.149237,0.182143,-0.227774,0.007802,0.199506,-0.240260,0.057530,0.185545,-0.431463,0.163220,0.189721,-0.456770,-0.471168,0.203650,-0.146362,0.239001,0.199729,-0.123242,0.273161,D,-0.103621,-0.231358,0.313254,-0.121260,-0.338699,0.282642,0.053421,-0.323205,0.111627,0.126285,
                        0.125085,0.138561,0.158916,0.047026,0.231414,0.053208,0.026350,0.189507,-0.404043,0.257592,0.151897,-0.274032,-0.046093,0.177514,-0.234453,-0.021832,0.034829,-0.498116,0.207730,0.048193,-0.186784,0.412271,0.080332,0.134865,0.575463,0.112559,-0.013531,0.547172,0.125556,0.005776,0.508825,0.215205,0.425985,
                        0.181616,0.110299,0.419159,0.321922,0.207605,0.331037,0.232510,0.086311,0.148636,0.282434,0.000638,-0.386198,-0.407391,0.073573,-0.395671,-0.466124,0.238375,-0.470133,-0.075830,0.093277,-0.241804,-0.641628,0.059337,-0.337275,-0.557907,-0.131431,-0.457491,-0.521608,-0.076398,-C,-0.148709,-0.059102,-H,
                        -0.222274,-0.106097,0.385370,0.366706,-0.089928,0.361577,0.315051,-0.110842,0.333333,0.242419,-0.137599,0.051260,0.486799,-0.179616,0.024333,0.445015,-0.043957,-0.165985,0.563151,-0.178054,-0.094880,0.504419,-0.154924,-0.024427,0.513004,-0.137663,-0.133790,0.484657,-0.092618,-0.391644,0.035196,-0.042856,
                        -0.336447,-0.049716,-0.220543,-0.388305,-0.244588,-0.232218,-0.433180,-0.342455,-0.154695,-0.273569,-0.047971,-0.130604,-0.390724,0.206373,-0.259216,-0.107564,-0.130077,-0.198228,-G,0.266045,-0.180486,-0.233912,-0.024024,-0.274469,-0.376654,-0.315745,-0.241394,-0.469408,-0.078767,-0.165775,-0.460771,
                        -0.047984,-0.065642,-0.328583,-0.051428,-0.129660,-0.078885,0.567087,-0.109942,-0.128015,0.528800,-0.129796,0.098218,0.565333,-0.186505,0.317381,0.419948,-0.004005,0.215761,0.499482,-0.002819,0.227905,0.308580,0.182862,-0.499670,-0.473220,0.194734,-0.042783,0.351201,0.091917,0.269294,0.276078,0.132048,
                        0.104912,0.522245,-0.005419,-0.006328,0.675304,0.072536,-0.153572,0.515188,0.100524,-0.128333,0.530089,-0.001521,-0.349590,-0.045783,0.274630,-0.377483,-0.312382,0.169459,-0.360793,-0.185761,0.226933,-0.106981,0.044587,0.173288,0.185880,-0.320055,0.245793,-0.270637,-0.371267,0.279321,-0.255966,-0.276771,
                        0.187685,-0.219501,-0.016086,0.179761,0.316827,0.422191,-0.002602,0.168040,0.258478,-0.002406,0.150857,0.222217,0.192662,0.248769,0.292859,0.162412,-0.461266,-0.045974,0.001109,-0.332804,-0.470974,-0.260405,-E,-0.175078,-0.234640,-A,-0.074087,-0.140663,0.105325,0.520575,-0.081849,-0.153339,0.514243,-0.159022,
                        -C,-0.046627};

                    const unsigned short inds[] = {
                        0,1,2,3,4,1,2,1,4,0,3,1,5,6,7,8,9,10,5,7,11,5,11,12,8,10,13,8,14,9,5,15,16,8,13,17,8,18,14,5,16,6,5,19,15,8,17,20,8,20,18,5,12,19,21,22,23,21,23,24,25,26,27,
                        28,29,30,28,30,31,32,33,34,35,36,22,27,31,30,27,30,25,37,28,38,23,33,32,23,32,24,35,22,21,27,26,39,37,29,28,33,40,34,22,36,41,22,41,23,42,31,27,42,27,39,38,28,31,38,31,42,23,41,40,23,40,33,
                        43,44,45,43,45,46,47,48,49,50,51,52,50,52,53,54,55,56,51,57,58,51,58,52,54,59,60,61,47,49,61,49,44,61,44,43,43,62,61,63,64,65,63,65,48,52,59,66,67,68,56,67,56,55,66,68,69,66,69,53,70,71,63,
                        71,64,63,44,49,50,44,50,45,61,62,72,61,72,70,47,63,48,56,66,59,56,68,66,53,69,45,53,45,50,54,56,59,51,50,49,51,49,48,70,47,61,70,63,47,66,53,52,69,73,46,69,46,45,68,73,69,68,67,73,60,59,52,
                        60,52,58,57,51,48,57,48,65,71,70,72,74,75,76,74,76,77,78,79,80,78,80,81,82,77,83,82,83,84,73,67,82,77,76,85,77,85,83,86,87,79,86,79,78,88,74,82,54,89,88,54,88,82,54,82,55,75,86,78,75,78,76,
                        79,90,91,79,91,80,82,84,46,82,46,73,67,55,82,76,78,81,76,81,85,87,92,90,87,90,79,74,77,82,93,94,95,93,95,96,97,98,71,97,71,72,97,72,99,100,101,102,103,104,105,103,105,106,107,108,109,110,111,112,110,112,113,
                        114,115,103,116,117,118,119,120,121,115,114,122,115,122,123,124,125,126,127,128,129,127,129,130,127,130,131,132,133,134,132,134,135,136,137,138,139,140,141,142,143,3,3,144,145,146,147,148,149,146,150,151,152,153,151,35,152,150,12,11,150,11,149,
                        16,15,154,32,34,155,32,155,156,157,158,159,160,161,162,163,164,165,163,165,166,167,168,169,167,169,170,171,172,173,174,175,117,174,117,176,177,178,179,177,179,180,98,97,181,98,181,182,183,184,185,183,185,186,187,188,189,187,189,190,191,192,193,
                        191,194,192,195,196,197,195,197,198,54,60,199,200,201,202,200,202,203,204,205,206,204,206,207,204,208,209,204,209,197,204,197,196,205,204,196,205,196,210,92,211,212,213,214,215,216,217,218,219,220,221,222,215,223,219,224,225,219,225,226,30,227,228,
                        30,228,25,14,18,229,230,231,37,230,37,232,233,234,235,233,235,236,237,238,239,237,239,240,241,2,242,243,0,2,244,139,245,230,246,247,230,247,248,249,250,251,249,251,252,253,254,255,253,255,256,253,256,257,258,259,260,261,262,263,261,263,264,
                        265,266,267,268,269,270,268,270,271,268,271,272,273,274,275,275,276,273,277,278,279,280,281,282,280,282,283,192,284,279,192,279,285,280,283,92,274,286,281,259,206,260,256,287,288,256,288,257,270,269,289,290,267,277,291,263,262,292,123,293,294,295,296,
                        112,297,286,298,255,254,298,254,299,249,252,300,300,132,249,301,135,302,301,302,244,241,303,304,2,305,242,233,306,307,307,308,233,231,309,221,221,37,231,13,310,311,234,9,14,234,14,235,42,39,217,42,217,312,313,223,224,215,214,225,215,225,224,
                        218,228,314,218,314,315,211,246,316,211,316,212,317,318,319,317,319,320,208,204,207,208,207,321,57,200,203,199,60,58,199,58,203,322,323,324,322,324,193,325,326,324,325,324,327,328,190,329,174,176,184,174,184,183,71,330,64,201,200,331,201,331,332,
                        333,334,109,333,109,108,334,333,175,334,175,174,175,333,335,175,335,336,175,336,337,175,337,178,175,178,177,172,338,173,169,168,339,160,340,155,166,152,163,166,158,152,170,169,223,159,158,166,159,166,341,164,24,32,164,32,156,6,16,154,6,154,342,
                        151,136,168,343,344,147,343,147,149,146,148,345,346,347,348,349,142,141,349,141,350,351,138,137,351,137,352,353,354,355,353,355,356,357,358,359,360,361,362,360,362,124,357,295,122,363,121,364,363,364,365,118,292,116,115,366,103,367,368,358,369,368,125,
                        107,370,328,371,171,372,373,374,375,373,375,94,62,43,99,96,95,85,96,85,81,376,377,80,376,80,91,99,72,62,106,105,93,106,93,378,368,100,125,107,109,370,379,369,124,379,124,362,380,381,367,366,104,103,118,180,365,107,364,121,107,121,120,
                        122,114,357,296,126,125,361,360,382,361,382,131,133,300,383,133,132,300,135,134,140,135,140,302,349,143,142,143,144,3,345,384,240,345,240,239,150,146,348,150,348,385,151,168,167,151,167,35,15,19,385,15,385,386,34,40,387,34,387,155,165,388,162,
                        223,159,170,388,165,156,161,160,389,339,390,169,172,376,338,176,391,392,336,335,119,336,119,393,370,187,328,71,98,330,332,394,189,332,189,395,328,187,190,326,191,193,326,193,324,198,197,209,198,209,396,88,89,322,88,322,285,397,398,262,397,262,261,
                        210,196,195,210,195,291,283,211,92,230,232,215,230,215,222,227,220,226,227,226,399,216,400,401,224,219,313,26,25,228,26,228,218,20,402,229,9,234,308,9,308,10,403,402,231,403,231,248,308,234,233,238,237,306,233,303,241,233,241,242,243,304,404,
                        243,404,245,247,405,406,247,406,403,407,319,251,407,251,250,408,299,276,294,296,112,408,409,295,410,411,259,410,259,258,291,265,263,266,293,268,412,275,289,273,287,256,273,256,413,411,194,321,321,207,411,282,317,283,414,272,87,414,87,86,278,414,86,
                        278,86,75,274,281,270,274,270,289,279,284,277,415,287,273,415,273,276,289,269,416,410,290,277,410,277,417,418,419,264,418,264,258,409,420,293,409,293,123,408,295,298,408,298,299,420,408,275,420,275,412,319,407,405,406,405,421,406,421,236,244,245,404,
                        421,422,304,421,304,303,236,421,303,233,238,306,423,310,308,423,308,307,231,230,248,20,17,311,20,311,402,37,424,29,313,425,223,214,401,400,214,400,225,316,222,426,228,227,399,215,232,213,252,318,111,318,251,319,208,321,396,208,396,209,398,210,291,
                        398,291,262,279,74,88,321,191,326,321,326,396,284,194,417,188,392,395,188,395,189,394,182,190,394,190,189,64,330,331,64,331,65,57,65,331,334,427,370,334,370,109,110,372,428,110,428,353,339,173,338,339,338,390,156,155,340,156,340,388,429,170,159,
                        429,159,341,157,159,223,40,41,389,40,389,387,12,150,385,12,385,19,342,343,7,384,153,157,385,348,352,344,430,153,344,153,384,3,145,431,3,431,4,142,3,0,350,432,433,138,339,136,354,433,432,354,432,355,428,173,339,428,339,138,126,360,124,
                        295,434,122,435,364,107,118,365,364,118,364,436,103,380,114,128,127,379,107,120,108,125,100,371,125,371,113,106,378,172,106,172,437,375,99,438,172,378,377,172,377,376,94,375,438,94,438,95,439,329,181,439,181,374,100,102,171,100,171,371,373,440,439,
                        368,369,358,369,359,358,101,441,103,101,103,106,101,106,437,118,104,366,180,179,363,180,363,365,121,393,119,361,130,129,361,129,362,130,361,131,355,133,383,355,383,356,339,168,136,140,350,141,433,347,346,433,346,349,346,146,144,346,144,143,345,148,384,
                        149,147,146,154,137,136,154,136,151,157,240,384,15,386,154,35,21,442,163,442,164,388,340,162,443,390,338,443,338,444,389,445,429,389,429,161,376,444,338,176,117,116,176,116,391,337,336,393,337,393,446,201,332,395,201,395,447,188,185,184,188,184,392,
                        328,329,439,391,325,327,391,327,447,323,202,327,323,327,324,199,89,54,321,194,191,448,449,418,448,418,260,391,195,325,212,90,92,230,222,246,222,316,246,218,315,216,219,226,220,215,224,223,399,400,314,39,26,218,39,218,217,229,235,14,231,402,311,
                        231,311,450,229,402,403,307,306,237,307,237,423,242,305,238,2,241,243,404,304,422,247,403,248,249,132,135,249,135,301,257,288,415,257,415,254,257,254,253,298,286,413,409,123,434,418,258,260,116,265,291,420,412,268,111,318,297,111,297,112,206,259,411,
                        267,268,272,267,272,414,281,280,270,193,192,285,193,285,322,92,271,280,282,297,318,282,318,317,413,286,274,289,416,412,412,416,269,412,269,268,266,265,292,261,264,419,293,266,292,294,298,295,112,286,294,299,254,415,299,415,276,451,250,249,451,249,301,
                        139,0,245,304,243,241,305,2,4,305,4,431,450,423,237,450,237,309,406,236,235,313,309,237,17,13,311,37,221,220,37,220,424,232,37,38,232,38,213,399,226,225,399,225,400,223,426,222,217,216,312,444,91,90,318,252,251,205,448,260,205,260,206,
                        57,203,58,323,199,203,323,203,202,411,417,194,436,104,118,435,328,439,187,370,185,187,185,188,97,375,374,175,177,180,175,180,117,370,427,186,370,186,185,110,353,356,111,110,383,170,429,445,160,155,387,426,169,390,161,429,341,161,341,162,157,223,425,
                        21,24,164,164,442,21,11,7,343,11,343,149,352,137,154,430,151,153,144,146,345,348,146,346,140,134,432,140,432,350,351,347,433,351,433,354,351,353,428,351,428,138,382,357,359,126,295,360,292,366,115,179,446,363,366,292,118,381,380,103,381,103,441,
                        358,380,367,125,124,369,104,440,373,104,373,105,102,101,437,329,190,182,329,182,181,95,438,83,95,83,85,377,96,81,377,81,80,438,99,84,438,84,83,43,46,84,43,84,99,437,172,102,172,171,102,113,371,372,113,372,110,120,119,108,379,362,129,
                        379,129,128,101,367,441,436,440,104,118,117,180,115,123,292,126,296,295,125,113,112,125,112,296,131,382,359,131,359,127,355,432,134,355,134,133,139,141,0,349,346,143,144,345,145,148,147,344,148,344,384,386,352,154,154,151,430,154,430,342,152,35,442,
                        152,442,163,36,445,389,36,389,41,162,340,160,426,390,443,165,164,156,170,445,167,111,383,300,372,171,173,372,173,428,174,183,186,174,186,427,174,427,334,392,184,176,178,337,446,178,446,179,97,374,181,98,182,394,98,394,330,435,439,440,392,391,447,
                        392,447,395,194,284,192,396,326,325,396,325,198,202,201,447,202,447,327,449,452,419,449,419,418,291,195,391,291,391,116,90,212,444,213,312,401,213,401,214,316,443,444,316,444,212,314,400,315,220,227,424,29,227,30,29,424,227,229,18,20,425,313,237,
                        425,237,240,406,235,229,450,311,310,450,310,423,238,305,431,238,431,239,245,0,243,301,244,404,301,404,451,421,405,407,421,407,422,319,405,247,319,247,320,276,275,408,255,298,413,255,413,256,408,420,409,258,264,263,258,263,410,265,116,292,268,267,266,
                        275,274,289,413,274,273,277,267,414,277,414,278,280,271,270,279,278,75,279,75,74,87,272,271,87,271,92,281,286,297,281,297,282,411,207,206,288,287,415,420,268,293,263,265,290,263,290,410,417,411,410,295,409,434,298,294,286,211,320,247,211,247,246,
                        422,407,250,422,250,451,302,139,244,422,451,404,236,303,233,238,233,242,450,309,231,403,406,229,13,10,308,13,308,310,221,309,313,221,313,219,213,38,42,213,42,312,400,216,315,426,443,316,399,314,228,401,312,216,111,300,252,283,317,320,283,320,211,
                        210,398,397,210,397,452,210,452,449,210,449,448,210,448,205,195,198,325,452,397,261,452,261,419,88,285,279,322,89,199,322,199,323,417,277,284,290,265,267,107,328,435,330,394,332,330,332,331,200,57,331,335,333,108,335,108,119,110,356,383,376,91,444,
                        387,389,160,223,169,426,166,165,162,166,162,341,35,167,445,35,445,36,153,152,158,153,158,157,7,6,342,157,425,240,352,386,385,342,430,344,342,344,343,145,345,239,145,239,431,141,142,0,350,433,349,302,140,139,351,352,348,351,348,347,353,351,354,
                        357,114,380,357,380,358,295,357,382,295,382,360,122,434,123,446,393,121,446,121,363,436,364,435,436,435,440,367,381,441,379,127,359,379,359,369,439,374,373,100,368,367,100,367,101,105,373,94,105,94,93,375,97,99,378,93,96,378,96,377};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CAR: {
#               ifndef TEAPOT_NO_MESH_CAR
                {
                    const float A=0.586117,B=0.764500,C=0.139795,D=0.633952,E=0.435170,F=0.499940,G=0.070295,H=0.390093,I=0.021560,J=1.137814,K=0.695000,L=0.487295,M=0.255417,N=0.230569,O=0.220484,P=0.110637,Q=0.380008,R=1.068362,S=0.581955,T=0.422249,U=0.114814,V=0.332379,W=0.512885,X=1.236972,Y=0.559726,Z=0.346988;
                    const float a=0.555333,b=0.235089,c=0.324959,d=0.766557,e=0.495763,f=1.041836,g=1.046013,h=0.926081,ia=0.200948,j=1.157299,k=0.936166,l=0.070397,m=1.198862,n=0.070012,o=0.374362,p=0.731834,q=0.495762,r=0.305288,s=0.618666,t=0.507028,u=0.417159,v=0.535115,w=0.621747,x=0.553880,y=0.891358,z=0.490655;
                    const float A0=0.656710,B0=0.621987,C0=0.901443,D0=0.660887,E0=0.741919,F0=0.285498,G0=0.626164,H0=1.007113,I0=1.011290,J0=0.776642,K0=0.194076,L0=1.036586,M0=0.555601,N0=0.259412,O0=0.851362,P0=0.479854,Q0=0.207819,R0=0.948566,S0=0.556399,T0=0.577563,U0=0.533104,V0=0.172821,W0=0.390990,X0=0.177727,Y0=0.103029,Z0=0.256360;
                    const float a0=0.0,b0=0.980582,c0=0.555041,d0=0.556959,e0=0.614906,f0=0.643977,g0=0.205278,h0=1.190948,i0=0.556679,j0=0.555321,k0=0.561991,l0=1.131783,m0=0.550009;

                    const float verts[] = {
                        -0.556399,N0,-X,M0,N0,-X,M0,Q0,-R0,-S0,Q0,-R0,W,C,-s,V,E,-t,T,E,-u,S,C,-v,-w,G,-T0,-w,G,U0,-S,C,z,-S,C,-v,o,L,V0,o,L,-W0,X0,F,-Y0,F0,L,Z0,a0,F,0.074698,-F0,L,Z0,T,E,b,-V,E,-t,-F0,L,-P0,F0,L,-P0,-X0,F,-Y0,-o,L,-W0,-o,L,V0,a0,F,-0.280755,c0,K0,-L0,-d0,K0,-L0,W,C,Y,a,G,e0,w,G,U0,
                        S,C,z,-T,E,b,-V,E,c,a,G,-f0,w,G,-T0,-a,G,e0,-W,C,Y,-T,E,-u,V,E,c,-W,C,-s,-a,G,-f0,M0,g0,-h0,-S0,g0,-h0,-i0,ia,-j,j0,ia,-j,-D,-M,-m,-K,-M,-J,-K,-I,-J,-D,-I,-m,-D,-Z,-J,D,-M,-m,D,-Z,-J,D,-M,X,K,-M,R,K,-I,R,D,-I,X,-K,-I,R,-K,-M,R,D,n,-J,D,-I,-m,-D,n,-J,D,-Z,R,-D,-Z,R,-D,-M,X,-D,-I,X,-D,n,R,D,n,R,K,-M,-J,
                        K,-I,-J,-i0,ia,-j,-k0,l,-l0,m0,l,-l0,j0,ia,-j,c0,K0,-L0,m0,l,-b0,-d0,K0,-L0,-k0,l,-b0,-S,C,-v,-S,C,z,-T,E,b,-T,E,-u,-W,C,-s,-V,E,-t,V,E,-t,W,C,-s,V,E,c,-V,E,c,-W,C,Y,W,C,Y,S,C,-v,T,E,-u,T,E,b,S,C,z,-A,-H,-f,-A,-F,-h,-B,-F,-h,-B,-H,-f,B,-U,-k,B,-N,-g,A,-N,-g,A,-U,-k,-A,-e,-d,-A,-Q,-A0,-B,-Q,-A0,-B,-e,
                        -d,-B,-P,-J0,-B,-N,-g,-A,-N,-g,B,-H,-f,B,-F,-h,A,-F,-h,A,-H,-f,-A,-Q,I0,-A,-O,H0,-B,-O,H0,-B,-Q,I0,B,-Q,I0,B,-O,H0,A,-O,H0,A,-Q,I0,-A,-H,G0,-A,-F,E0,-B,-F,E0,-B,-H,G0,-A,-O,-D0,-B,-O,-D0,B,-P,-J0,A,-P,-J0,-A,-P,-J0,-A,-U,-k,-B,-U,-k,B,-H,G0,B,-F,E0,A,-F,E0,A,-H,G0,-A,-e,C0,-B,-e,C0,B,-O,-D0,A,-O,-D0,
                        B,-N,B0,A,-N,B0,-A,-P,y,-B,-P,y,x,-r,-O0,-A,-N,B0,-x,-r,0.816638,-x,-r,-O0,A,-q,-d,A,-Q,-A0,B,-P,y,B,-U,p,A,-U,p,A,-P,y,-B,-N,B0,-A,-U,p,B,-q,-d,B,-q,C0,x,-r,0.816639,-B,-U,p,B,-Q,-A0,A,-q,C0};

                    const unsigned short inds[] = {
                        47,49,46,46,52,50,54,56,53,48,58,57,60,61,59,62,50,52,58,50,63,63,64,58,65,66,57,50,47,46,61,49,48,62,54,53,67,56,55,52,51,68,59,69,60,51,69,68,66,59,61,70,72,73,74,72,75,76,71,70,60,46,49,
                        65,67,66,48,66,61,57,64,65,68,62,52,53,63,62,65,53,56,76,75,77,55,68,69,55,59,67,47,48,49,46,51,52,54,55,56,48,47,58,60,49,61,62,63,50,58,47,50,51,60,69,66,67,59,70,71,72,74,73,72,76,77,71,
                        60,51,46,65,56,67,48,57,66,57,58,64,68,54,62,53,64,63,65,64,53,76,74,75,55,54,68,55,69,59,78,80,81,83,85,82,86,88,89,90,92,93,78,79,80,83,84,85,86,87,88,90,91,92,1,3,0,4,6,7,8,10,11,
                        12,13,14,15,16,17,6,12,18,20,5,19,22,23,24,21,20,25,3,26,27,25,13,21,22,17,16,23,25,20,28,30,31,5,13,6,32,17,24,34,7,35,9,37,10,23,19,38,15,14,16,39,31,18,15,18,12,38,40,11,10,33,32,
                        11,41,8,38,24,23,1,43,42,16,25,22,37,29,28,26,2,1,0,27,44,45,43,44,6,13,12,38,32,24,1,42,45,45,26,1,44,43,0,0,3,27,35,31,30,40,34,41,17,39,15,1,2,3,4,5,6,8,9,10,20,21,5,
                        3,2,26,25,14,13,22,24,17,23,22,25,28,29,30,5,21,13,32,33,17,34,4,7,9,36,37,23,20,19,15,12,14,39,28,31,15,39,18,38,19,40,10,37,33,11,40,41,1,0,43,16,14,25,37,36,29,45,42,43,35,7,31,
                        40,4,34,17,33,39,95,97,94,99,101,98,103,105,102,106,96,104,94,107,108,110,112,109,114,116,113,118,120,117,122,124,121,125,104,103,98,128,127,130,106,129,133,135,132,113,137,136,127,139,138,132,141,140,142,115,114,100,112,144,145,146,121,
                        94,147,95,108,131,130,144,148,149,146,122,121,151,153,150,147,103,102,143,123,116,142,146,155,113,146,114,98,156,109,102,96,95,140,152,151,151,157,132,112,111,144,152,141,158,129,147,130,146,142,114,144,139,128,158,119,153,158,135,134,120,119,158,
                        111,148,144,132,140,151,151,150,118,118,117,157,157,133,132,143,159,154,154,124,123,123,137,116,116,115,143,108,147,94,103,147,125,138,149,160,155,143,142,122,146,136,160,148,156,101,100,144,145,159,155,136,123,122,147,129,125,129,126,125,157,134,133,
                        158,161,120,128,101,144,150,119,118,156,111,110,117,161,157,121,154,145,130,147,108,149,139,144,146,113,136,158,134,161,155,146,145,153,152,158,141,135,158,95,147,102,109,99,98,98,127,138,138,160,156,156,110,109,106,131,107,107,97,96,96,105,104,
                        104,126,106,109,100,99,95,96,97,99,100,101,103,104,105,106,107,96,94,97,107,110,111,112,114,115,116,118,119,120,122,123,124,125,126,104,98,101,128,130,131,106,133,134,135,113,116,137,127,128,139,132,135,141,142,143,115,108,107,131,151,152,153,
                        143,154,123,98,138,156,102,105,96,140,141,152,151,118,157,138,139,149,155,159,143,160,149,148,145,154,159,136,137,123,129,106,126,157,161,134,150,153,119,156,148,111,117,120,161,121,124,154,109,112,100};

                    // Chassis:	156 inds
                    // Glass:	24 inds
                    // Black:	204 inds
                    // Wheels:	360 inds

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CHAIR: {
#               ifndef TEAPOT_NO_MESH_CHAIR
                {
                    const float A=0.243105,B=0.010308,C=0.500213,D=0.178277,E=0.173974,F=0.233475,G=0.164448,H=0.058928,I=0.232517,J=0.226072,K=0.224925,L=0.158547,M=0.165405,N=0.243106,O=0.306060,P=0.157401,Q=0.157400,R=0.475903,S=0.156668,T=0.199886,U=0.478605,V=0.165406;
                    const float verts[] = {
                        0.243105,B,-A,I,B,-A,A,H,-A,I,B,-J,A,H,-E,A,B,A,D,H,-A,A,C,-E,D,O,-N,A,C,-A,-A,H,-A,-F,B,-A,-A,B,-A,-D,O,-A,-D,H,-A,-F,B,P,-A,B,N,-A,H,-E,I,B,P,A,H,A,I,B,A,D,H,-E,-A,H,N,G,B,A,-D,H,-E,-F,B,N,-F,B,K,G,B,-A,G,B,-J,-V,B,-A,-V,B,-J,-F,B,-J,I,B,K,G,B,K,-M,B,N,-M,B,K,G,B,-L,-M,B,-L,
                        -F,-C,-J,G,B,P,-M,B,P,-M,-C,-L,-F,B,-L,-F,-C,Q,-M,-C,K,I,B,-L,G,-C,-J,I,-C,-L,G,-C,Q,I,-C,K,I,-C,Q,G,-C,K,I,-C,-J,G,-C,-L,-M,-C,Q,-F,-C,K,-V,-C,-J,-F,-C,-L,-A,C,-A,-D,O,-E,-A,C,-E,D,O,-E,-D,R,-E,-S,C,-E,-D,R,-A,-T,C,-E,-S,C,-A,-D,U,-A,-T,C,-A,-D,U,-E,D,R,-A,D,U,-A,S,C,-A,D,R,-E,T,C,-A,S,C,-E,D,U,-E,T,
                        C,-E};

                    const unsigned short inds[] = {
                        0,1,2,0,3,1,0,4,5,2,1,6,4,0,2,2,7,4,2,8,9,10,11,12,10,13,14,12,15,16,12,17,10,18,0,5,5,4,19,19,20,5,19,21,22,22,23,19,22,17,16,22,24,17,16,25,22,16,26,25,16,17,12,
                        1,3,27,27,3,28,27,28,29,27,6,1,29,30,11,29,28,30,29,14,27,11,30,31,11,31,12,11,14,29,20,19,23,20,32,5,20,33,32,23,22,34,23,35,33,23,33,20,34,22,25,34,35,23,34,26,35,25,26,34,30,36,37,
                        30,38,31,37,39,40,37,41,30,40,42,37,40,33,35,40,43,15,35,44,40,31,38,42,42,12,31,42,40,15,42,41,37,15,12,42,15,43,26,26,16,15,26,44,35,3,0,45,3,46,28,45,0,18,45,47,3,18,5,32,18,36,45,
                        18,48,39,32,49,18,28,46,36,36,30,28,36,18,39,36,47,45,39,37,36,39,48,33,33,40,39,33,49,32,50,18,49,50,48,18,49,33,51,49,48,50,51,33,48,51,48,49,52,3,47,52,46,3,47,36,53,47,46,52,53,36,46,
                        53,46,47,54,40,44,54,43,40,44,26,55,44,43,54,55,26,43,55,43,44,56,30,41,56,38,30,41,42,57,41,38,56,57,42,38,57,38,41,17,58,10,17,59,60,4,21,19,4,61,21,6,27,14,6,61,8,6,8,2,14,11,10,
                        14,24,6,14,13,24,21,6,24,21,61,6,24,22,21,24,13,59,24,59,17,62,63,64,64,65,62,64,63,66,64,67,68,64,66,67,58,17,60,58,13,10,58,65,68,60,65,58,60,69,65,70,71,72,70,72,73,73,74,70,73,72,75,
                        73,76,77,7,2,9,7,61,4,7,74,77,9,74,7,9,71,74,61,7,76,61,13,8,61,75,59,8,72,71,13,58,67,13,61,59,13,66,8,59,63,69,65,69,62,69,60,59,69,63,62,63,59,75,68,65,64,68,67,58,67,66,13,
                        66,63,72,74,71,70,71,9,8,72,8,66,77,74,73,77,76,7,76,75,61,75,72,63,75,76,73};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CHARACTER: {
#               ifndef TEAPOT_NO_MESH_CHARACTER
                {
                    const float A=0.135638,B=0.056183,C=0.037627,D=0.057922,E=0.001879,F=0.063700,G=0.031989,H=0.0,I=0.097337,J=0.058172,K=0.442050,L=0.499956,M=0.052284,N=0.117274,O=0.414724,P=0.292279,Q=0.231831,R=0.292392,S=0.157148,T=0.161858,U=0.019598,V=0.037291,W=0.153785,X=0.436659,Y=0.018152,Z=0.049762;
                    const float a=0.346368,b=0.066755,c=0.054776,d=0.060140,e=0.049959,f=0.047816,g=0.214279,h=0.042051,ia=0.476966,j=0.059062,k=0.057227,l=0.061592,m=0.088939,n=0.039146,o=0.331938,p=0.058195,q=0.022252,r=0.069962,s=0.176355,t=0.217334,u=0.143020,v=0.011237,w=0.107649,x=0.275367,y=0.058811,z=0.083640;
                    const float A0=0.257492,B0=0.024361,C0=0.220169,D0=0.149928,E0=0.161373,F0=0.162004,G0=0.043486,H0=0.097250,I0=0.111190,J0=0.070007,K0=0.070009,L0=0.065958,M0=0.354711,N0=0.086852,O0=0.043677,P0=0.079404,Q0=0.012148,R0=0.087117,S0=0.319132,T0=0.065551,U0=0.326527,V0=0.067643,W0=0.374231,X0=0.093643,Y0=0.374357,Z0=0.065614;
                    const float a0=0.414893,b0=0.042578,c0=0.374359,d0=0.343797,e0=0.065942,f0=0.105018,g0=0.040501,h0=0.113207,i0=0.312358,j0=0.175027,k0=0.055688,l0=0.477693,m0=0.079405,n0=0.374229,o0=0.155027,p0=0.063972,q0=0.081661,r0=0.312357,s0=0.477692,t0=0.055689,u0=0.032442;
                    const float verts[] = {
                        -v,-m,Z0,v,-m,-X0,v,-m,Z0,H,R,b0,-J0,R,-Z,K0,R,-Z,w,x,-D,y,P,Q0,w,x,C,-B,-J,z,-A,-J,G,-F,A0,-f0,B0,P,-p0,F,A0,-f0,-F,A0,q0,-y,P,Q0,-w,x,C,-y,P,-u0,-w,x,-D,-B0,P,-p0,-u,D0,C,u,D0,C,E0,F0,-D,E0,F0,C,F,j0,-h0,u,D0,-D,y,P,-u0,-F,j0,-h0,-E0,F0,-D,-u,D0,-D,F,E,H0,F,o0,R0,F,A0,q0,B0,P,O0,-B0,P,O0,-B,-J,-I0,
                        -v,-m,-X0,-A,-J,-G0,B,-J,-I0,A,-J,-G0,B,-J,z,A,-J,G,-A,-K,G,-B,-K,G,A,-K,-M,B,-K,-M,-F,E,H0,-F,o0,R0,A,-K,G,B,-K,G,-B,-K,-M,-A,-K,-M,-F,E,-N,W,E,C,F,E,-N,W,E,-D,-W,E,C,-W,E,-D,-E0,F0,C,C0,g,C,-C0,g,-D,-Q,-I,-D,Q,-I,-D,T,-I,-D,-Q,-I,C,-T,-I,C,Q,-I,C,C0,g,-D,-C0,g,C,-T,-I,-D,T,-I,C,-P0,W0,f,-t0,s0,e,-0.105053,
                        0.439534,-g0,-d,i0,h,-J0,R,-Z,k0,l0,e,m0,n0,f,0.105052,0.439533,-g0,0.000001,L,-0.034255,K0,R,-Z,H,0.395718,-0.095346,-A,-J,-G0,-F,E,-N,-B,-J,-I0,-A,-J,G,-F,E,H0,-W,E,C,A,-J,-G0,W,E,C,A,-J,G,F,E,H0,B,-J,z,-W,E,-D,F,E,-N,W,E,-D,B,-J,-I0,-B,-J,z,-A,-K,-M,-A,-L,G,-A,-K,G,A,-K,-M,B,-L,-M,B,-K,-M,-B,-L,G,
                        -A,-L,-M,-B,-L,-M,A,-L,G,B,-L,G,-B,-K,-M,A,-ia,N,B,-L,N,A,-L,N,A,-L,-M,-B,-ia,N,-A,-L,N,-B,-L,N,B,-K,G,A,-K,G,-B,-K,G,-A,-ia,N,d,r0,h,-U,X,j,-V,O,k,-U,O,l,U,X,j,V,O,k,V,X,c,-n,o,p,H,S0,T0,H,U0,V0,n,o,p,B,-ia,N,-V,X,c,U,O,l,H,M0,N0,q,Y0,r,H,a0,L0,Y,a,b,-q,c0,r,H,M0,N0,H,a0,L0,-Y,a,b,H,M0,N0,H,d0,e0,Y,
                        a,b,-Y,a,b,-d,i0,h,H,R,b0,H,0.300543,0.059667,n,o,p,d,r0,h,m0,n0,f,-P0,W0,f,-q,c0,r,-V,O,k,k0,l0,e,-U,X,j,U,X,j,K0,R,-Z,-Q,-I,C,-s,-S,C,-T,-I,C,T,-I,-D,s,-S,C,T,-I,C,t,-S,C,s,-S,-D,t,-S,-D,-s,-S,-D,-t,-S,C,-t,-S,-D,Q,-I,C,Q,-I,-D,-T,-I,-D,-Q,-I,-D,H,O,L0,-n,o,p,-J0,R,-Z,H,S0,T0,q,Y0,r,V,O,k,V,X,c,U,O,
                        l,-U,O,l,-t0,s0,e,-V,X,c,-Y,a,b,H,U0,V0,H,d0,e0,Y,a,b};

                    const unsigned short inds[] = {
                        0,1,2,3,4,5,6,7,8,0,9,10,11,12,13,14,15,16,16,17,18,11,17,19,20,14,16,21,22,23,24,6,25,26,13,12,27,13,24,20,28,29,30,21,31,32,7,33,34,12,17,32,34,14,27,18,11,35,36,37,1,35,38,
                        1,38,39,40,2,41,0,42,43,34,33,7,7,26,12,12,19,17,17,15,34,1,44,45,40,0,2,46,31,47,39,48,44,37,42,10,2,45,49,0,50,36,2,48,41,36,51,37,47,32,14,52,29,27,32,21,8,53,25,21,52,24,54,
                        54,25,55,56,29,57,46,20,56,16,58,20,23,8,21,59,6,8,60,16,18,6,22,25,28,18,29,28,61,60,22,62,63,58,64,65,66,67,59,23,66,59,61,68,60,63,23,22,58,69,28,0,36,1,6,26,7,11,19,12,14,34,15,
                        16,15,17,11,18,17,20,47,14,21,25,22,24,13,6,26,6,13,27,11,13,20,58,28,30,53,21,32,8,7,34,7,12,32,33,34,27,29,18,1,36,35,0,10,42,1,39,44,40,9,0,46,30,31,39,41,48,37,51,42,2,1,45,
                        0,43,50,2,49,48,36,50,51,47,31,32,52,57,29,32,31,21,53,55,25,52,27,24,54,24,25,56,20,29,46,47,20,16,68,58,23,59,8,59,67,6,60,68,16,6,67,22,28,60,18,28,69,61,22,67,62,58,68,64,66,62,67,
                        23,70,66,61,64,68,63,70,23,58,65,69,71,72,73,74,71,75,76,77,78,72,76,79,71,73,75,77,80,78,76,78,79,72,79,73,75,73,81,80,75,81,78,80,81,79,78,81,73,79,81,82,83,84,85,86,87,88,89,90,90,91,92,
                        82,87,93,88,94,95,84,94,96,92,86,97,98,99,100,101,102,103,104,105,106,102,107,108,109,104,106,110,111,112,101,107,113,114,115,116,103,108,117,109,105,98,116,99,104,112,108,107,117,110,118,100,114,119,104,114,116,107,110,112,99,120,100,
                        77,121,80,122,123,124,125,126,127,128,129,130,130,129,131,82,93,83,85,97,86,88,95,89,90,89,91,82,85,87,88,96,94,84,83,94,92,91,86,98,105,99,101,113,102,104,99,105,102,113,107,109,119,104,110,132,111,101,118,107,114,120,115,
                        103,102,108,109,106,105,116,115,99,112,111,108,117,132,110,100,120,114,104,119,114,107,118,110,99,115,120,117,108,132,108,111,132,122,133,123,125,134,126,135,136,137,136,135,138,139,140,141,140,139,142,143,144,145,144,143,146,147,148,149,150,151,152,
                        153,154,155,156,157,158,151,148,159,160,161,162,163,164,165,166,167,168,169,170,171,172,168,173,162,169,174,165,166,172,175,170,160,174,171,175,173,167,163,176,158,157,153,147,177,149,148,151,148,147,178,150,149,151,177,149,179,180,152,181,152,182,181,
                        158,176,183,180,183,176,154,184,155,157,185,186,153,186,185,156,158,182,176,157,184,187,188,189,188,190,189,154,177,187,150,180,190,156,185,157,160,170,161,163,167,164,166,164,167,169,161,170,172,166,168,162,161,169,165,164,166,175,171,170,174,169,171,
                        173,168,167,150,179,149,177,147,149,152,156,182,180,181,183,154,176,184,153,155,186,187,177,188,188,150,190,154,153,177,150,152,180};

                    // Cylinder_Cylinder_Outfit_Cylinder_Cylinder_Outfit_Outfit:	327 inds
                    // Cylinder_Cylinder_Outfit_Cylinder_Cylinder_Outfit_Blackt:	216 inds
                    // Cylinder_Cylinder_Outfit_Cylinder_Cylinder_Outfit_Skinkt:	183 inds

                    // numVerts = 191; numInds =  726

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_GHOST: {
#               ifndef TEAPOT_NO_MESH_GHOST
                {
                    const float A=0.443444,B=0.060890,C=0.360372,D=0.204377,E=0.500124,F=0.470069,G=0.338300,H=0.351984,I=0.385886,J=0.047027,K=0.324393,L=0.049136,M=0.078509,N=0.241513,O=0.241932,P=0.063165,Q=0.324957,R=0.075139,S=0.386557,T=0.136031,U=0.205520,V=0.182713,W=0.276049,X=0.217348,Y=0.328377,Z=0.204325;
                    const float a=0.137819,b=0.274444,c=0.326468,d=0.168321,e=0.096578,f=0.224602,g=0.339617,h=0.072009,ia=0.370454,j=0.289004,k=0.312869,l=0.211033,m=0.074861,n=0.367955,o=0.080077,p=0.393590,q=0.076563,r=0.393882,s=0.076639,t=0.332986,u=0.332655,v=0.334933,w=0.313118,x=0.207248,y=0.394274,z=0.221687;
                    const float A0=0.220206,B0=0.185115,C0=0.191287,D0=0.224378,E0=0.065999,F0=0.069096,G0=0.0,H0=0.132265;
                    const float verts[] = {
                        -0.069096,-H,-g,h,-E,-ia,C0,-H,-j,k,-E,-l,g,-H,-F0,ia,-E,h,j,-H,C0,l,-E,k,F0,-H,g,-h,-E,ia,-C0,-H,j,-k,-E,l,-g,-H,F0,-ia,-E,-h,-j,-H,-0.191288,-l,-E,-k,-G0,E,-G0,-m,-G,-n,-o,B,-p,q,-F,-r,s,B,-y,x,-G,-w,z,B,-v,u,-F,-D0,t,B,-f,n,-G,-m,p,B,-o,r,-F,q,y,B,s,w,-G,x,v,B,z,D0,-F,u,f,B,
                        t,m,-G,n,o,B,p,-q,-F,r,-s,B,y,-x,-G,w,-z,B,v,-u,-F,0.224379,-t,B,f,-n,-G,m,-p,B,o,-r,-F,-q,-y,B,-s,-w,-G,-x,-v,B,-0.221688,-D0,-F,-u,-f,B,-t,-M,D,-I,-E0,C,-K,-L,A,-N,J,A,-O,P,C,-Q,R,D,-S,T,A,-U,V,C,-W,X,D,-Y,Z,A,-a,b,C,-B0,c,D,-A0,N,A,-L,K,C,-E0,I,D,-M,O,A,J,Q,C,P,S,D,R,U,A,T,W,C,V,Y,D,X,a,A,Z,B0,C,b,
                        A0,D,c,L,A,N,E0,C,K,M,D,I,-J,A,O,-P,C,Q,-R,D,S,-T,A,U,-V,C,W,-X,D,Y,-Z,A,a,-b,C,B0,-c,D,A0,-N,A,L,-K,C,0.065998,-I,D,M,-O,A,-J,-Q,C,-P,-S,D,-R,-U,A,-T,-W,C,-V,-Y,D,-X,-a,A,-Z,-0.185114,C,-b,-0.220205,D,-c,0.150856,H0,0.359751,-0.147711,H0,0.361054,0.114683,d,0.372819,0.115466,e,0.376671,0.187729,e,0.346368,
                        0.185531,d,0.343109,-0.182530,d,0.344715,-0.184699,e,0.347993,-0.112175,e,0.377664,-0.111425,d,0.373805};

                    const unsigned short inds[] = {
                        12,0,4,7,8,6,10,8,9,12,10,11,14,12,13,0,14,15,1,2,0,3,4,2,5,6,4,20,49,54,17,20,19,22,54,57,20,21,19,60,22,57,21,24,23,26,60,63,24,25,23,28,63,66,25,28,27,30,66,69,28,29,27,
                        32,69,72,29,32,31,32,33,31,36,75,78,33,36,35,36,37,35,40,81,84,37,40,39,42,84,87,40,41,39,44,87,90,41,44,43,46,90,93,44,45,43,48,93,96,45,48,47,18,96,49,48,17,47,8,35,9,8,31,33,6,31,7,
                        6,27,29,4,27,5,4,23,25,2,23,3,2,19,21,0,19,1,0,47,17,14,47,15,14,43,45,12,43,13,12,39,41,10,39,11,10,35,37,16,51,94,50,94,51,49,95,50,16,94,91,94,92,91,96,92,95,16,91,88,91,89,88,
                        92,90,89,16,88,85,89,85,88,89,87,86,16,85,82,86,82,85,87,83,86,16,82,79,83,79,82,84,80,83,16,79,76,80,76,79,81,77,80,16,76,73,77,73,76,78,74,77,16,73,70,74,70,73,75,71,74,16,70,67,71,67,70,
                        72,68,71,16,67,64,68,64,67,68,66,65,16,64,61,65,61,64,65,63,62,16,61,58,61,59,58,63,59,62,16,58,55,59,55,58,60,56,59,16,55,52,56,52,55,56,54,53,16,52,51,52,50,51,53,49,50,4,6,8,8,10,12,
                        12,14,0,0,2,4,4,8,12,20,18,49,17,18,20,22,20,54,20,22,21,60,24,22,21,22,24,26,24,60,24,26,25,28,26,63,25,26,28,30,28,66,28,30,29,32,30,69,29,30,32,32,34,33,36,34,75,33,34,36,36,38,37,
                        40,38,81,37,38,40,42,40,84,40,42,41,44,42,87,41,42,44,46,44,90,44,46,45,48,46,93,45,46,48,18,48,96,48,18,17,8,33,35,8,7,31,6,29,31,6,5,27,4,25,27,4,3,23,2,21,23,2,1,19,0,17,19,
                        0,15,47,14,45,47,14,13,43,12,41,43,12,11,39,10,37,39,10,9,35,50,95,94,49,96,95,94,95,92,96,93,92,91,92,89,92,93,90,89,86,85,89,90,87,86,83,82,87,84,83,83,80,79,84,81,80,80,77,76,81,78,77,
                        77,74,73,78,75,74,74,71,70,75,72,71,71,68,67,72,69,68,68,65,64,68,69,66,65,62,61,65,66,63,61,62,59,63,60,59,59,56,55,60,57,56,56,53,52,56,57,54,52,53,50,53,54,49,81,106,78,78,105,36,36,104,38,
                        72,99,102,72,101,32,34,101,100,75,100,99,81,104,103,81,103,106,78,106,105,36,105,104,72,75,99,72,102,101,34,32,101,75,34,100,81,38,104,103,104,98,99,100,97,100,101,97,101,102,97,102,99,97,104,105,98,105,106,98,106,103,98};

                    // Cylinder_Cylinder_Color:	558 inds
                    // Cylinder_Cylinder_White:	48 inds
                    // Cylinder_Cylinder_Black:	24 inds

                    // numVerts = 107; numInds =  630

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_SKITTLE: {
#               ifndef TEAPOT_NO_MESH_SKITTLE
                {
                    const float A=0.068164,B=0.028235,C=0.500199,D=0.146066,E=0.060503,F=0.067338,G=0.252662,H=0.045625,I=0.293162,J=0.108954,K=0.041616,L=0.372116,M=0.150572,N=0.292958,O=0.244034,P=0.134676,Q=0.451275,R=0.000000;
                    const float verts[] = {
                        0.028235,-C,A,A,-C,B,A,-C,-B,B,-C,-A,-B,-C,-A,-A,-C,-B,-A,-C,B,-B,-C,A,R,L,-M,J,N,-F,-K,O,-F,-P,L,-F,-K,C,-F,J,Q,-F,K,O,F,-J,N,F,-J,Q,F,K,C,F,P,L,F,R,L,M,B,G,A,B,-C,A,A,G,B,A,-C,B,A,G,-B,A,-C,-B,B,G,-A,B,-C,-A,-B,G,-A,-B,-C,-A,-A,G,-B,-A,-C,-B,-A,G,B,-A,-C,B,-B,G,A,-B,-C,A,E,H,
                        D,E,-I,D,D,-I,E,D,H,E,D,-I,-E,D,H,-E,E,-I,-D,E,H,-D,-E,-I,-D,-E,H,-D,-D,-I,-E,-D,H,-E,-D,-I,E,-D,H,E,-E,-I,D,-E,H,D};

                    const unsigned short inds[] = {
                        6,3,2,8,9,10,9,8,13,8,10,11,8,11,12,8,12,13,9,13,18,10,9,14,11,10,15,12,11,16,13,12,17,9,18,14,10,14,15,11,15,16,12,16,17,13,17,18,14,18,19,15,14,19,16,15,19,17,16,19,18,17,19,
                        21,38,37,23,40,38,25,42,40,27,44,42,29,46,44,31,48,46,48,35,50,35,37,50,26,30,22,51,20,34,51,37,36,49,34,32,48,51,49,47,32,30,46,49,47,45,30,28,44,47,45,43,28,26,42,45,43,24,43,26,40,43,41,
                        39,24,22,38,41,39,36,22,20,36,38,39,2,1,0,0,7,6,6,5,4,4,3,6,2,0,6,21,23,38,23,25,40,25,27,42,27,29,44,29,31,46,31,33,48,48,33,35,35,21,37,34,20,22,22,24,26,26,28,30,30,32,34,
                        34,22,30,51,36,20,51,50,37,49,51,34,48,50,51,47,49,32,46,48,49,45,47,30,44,46,47,43,45,28,42,44,45,24,41,43,40,42,43,39,41,24,38,40,41,36,39,22,36,37,38};

                    // numVerts = 52; numInds =  240

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_SLEDGE: {
#               ifndef TEAPOT_NO_MESH_SLEDGE
                {
                    const float A=0.194319,B=0.354346,C=0.214322,D=0.500085,E=0.404831,F=0.439738,G=0.296856,H=0.417552,I=0.560433,J=0.400068,K=1.108146,L=0.844569,M=1.007324,N=0.270446,O=0.498414,P=0.822383,Q=0.714364,R=0.047627,S=0.606433,T=0.247837,U=0.343091,V=0.453996,W=0.358741;
                    const float verts[] = {
                        0.354346,-C,F,B,-E,G,B,-C,G,A,-C,F,A,-E,F,-A,-C,F,-A,-E,G,-A,-C,G,-B,-C,F,-B,-E,F,A,-C,G,-B,-C,G,B,-C,-H,B,-E,-I,B,-C,-I,A,-C,-H,A,-E,-H,-A,-C,-H,-A,-E,-I,-A,-C,-I,-B,-C,-H,-B,-E,-H,A,-C,-I,-B,-C,-I,B,-E,-H,A,-E,-K,A,-E,-I,B,-D,-H,A,-D,-I,-A,-E,-H,-B,-E,-K,-B,-E,-I,-A,-D,-H,-B,
                        -D,-I,B,-E,F,A,-E,G,B,-D,F,B,-E,L,A,-D,G,-A,-E,F,-B,-E,G,-A,-D,F,-A,-E,L,-B,-D,G,B,-D,-I,B,-D,-K,A,-D,-H,-A,-D,-I,-A,-D,-K,-B,-D,-H,B,-D,G,A,-D,L,A,-D,F,-A,-D,G,-B,-D,L,-B,-D,F,-B,-D,-K,-A,-E,-K,A,-D,-K,B,-E,-K,-A,-D,L,-B,-W,M,-B,-E,L,-A,-V,M,B,-D,L,A,-W,M,A,-E,L,B,-V,M,-B,-V,M,-B,-U,K,-A,-W,M,-A,-T,
                        K,A,-V,M,A,-U,K,B,-W,M,B,-T,K,-A,-U,K,-B,-T,K,B,-U,K,A,-T,K,-J,-C,S,J,-C,S,-J,D,S,-A,-C,F,J,D,S,-N,D,-Q,-J,D,-P,-J,-C,F,N,D,O,J,-C,-H,J,D,-P,J,-C,F,B,-C,F,-J,-C,-P,J,-C,-P,-J,-C,-I,-B,-C,-I,-J,-C,-H,J,-C,-I,-A,-C,-I,-N,D,O,-N,-R,O,N,D,-Q,N,-R,-Q,N,-R,O,-N,-R,-Q,-J,-C,G,-B,-C,F,-B,-C,G,-B,-C,-H,J,-C,G,
                        B,-C,G,B,-C,-H,B,-C,-I,A,-C,F,A,-C,G,-A,-C,G,A,-C,-H,-A,-C,-H,A,-C,-I};

                    const unsigned short inds[] = {
                        80,81,82,80,83,81,82,81,84,82,85,86,82,87,80,84,88,82,84,89,90,81,91,84,81,92,91,93,90,94,93,95,86,93,96,95,86,90,93,86,85,90,86,97,82,90,88,84,90,98,94,94,99,93,100,82,88,100,85,82,100,101,85,
                        88,90,102,88,101,100,102,90,85,102,103,88,85,103,102,101,88,104,104,88,103,104,103,101,103,85,105,105,85,101,105,101,103,87,82,106,87,107,80,87,108,107,106,82,97,106,108,87,106,109,108,97,86,95,97,109,106,97,96,109,95,96,97,
                        91,92,110,110,84,91,110,92,111,110,111,89,89,84,110,89,111,112,89,112,98,98,90,89,98,112,113,98,113,94,92,81,114,114,81,83,83,80,107,83,115,114,115,83,116,115,112,111,116,117,115,108,118,116,112,115,117,117,116,118,118,108,109,
                        118,119,117,119,94,113,119,118,99,99,94,119,96,93,99,0,1,2,3,4,0,5,6,7,8,9,5,2,1,10,10,4,3,7,6,11,11,9,8,12,13,14,15,16,12,17,18,19,20,21,17,14,13,22,22,16,15,19,18,23,23,21,20,
                        13,12,24,13,25,26,24,12,16,24,1,27,24,27,13,16,22,26,16,1,24,26,22,13,26,28,16,26,25,28,18,17,29,18,30,31,29,17,21,29,6,32,29,32,18,21,23,31,21,6,29,31,23,18,31,33,21,31,30,33,1,0,34,
                        1,16,35,34,0,4,34,36,1,34,37,36,4,10,35,4,37,34,35,10,1,35,16,38,35,38,4,6,5,39,6,21,40,39,5,9,39,41,6,39,42,41,9,11,40,9,42,39,40,11,6,40,21,43,40,43,9,44,13,27,44,45,13,
                        27,28,44,27,38,46,46,16,28,46,28,27,46,38,16,28,45,44,47,18,32,47,48,18,32,33,47,32,43,49,49,21,33,49,33,32,49,43,21,33,48,47,50,1,36,50,27,1,36,38,50,36,51,52,52,4,38,52,38,36,52,51,4,
                        38,27,50,53,6,41,53,32,6,41,43,53,41,54,55,55,9,43,55,43,41,55,54,9,43,32,53,48,33,56,56,33,30,56,30,48,30,18,57,57,18,48,57,48,30,45,28,58,58,28,25,58,25,45,25,13,59,59,13,45,59,45,25,
                        54,41,60,54,61,62,60,41,42,60,63,54,42,9,62,42,63,60,62,9,54,62,61,42,51,36,64,51,65,66,64,36,37,64,67,51,37,4,66,37,67,64,66,4,51,66,65,37,68,54,63,68,61,54,68,69,61,63,42,70,63,69,68,
                        70,42,61,70,71,63,61,71,70,72,51,67,72,65,51,72,73,65,67,37,74,67,73,72,74,37,65,74,75,67,65,75,74,69,63,76,76,63,71,76,71,69,71,61,77,77,61,69,77,69,71,73,67,78,78,67,75,78,75,73,75,65,79,
                        79,65,73,79,73,75};

                    // Upperpart: 68*3 inds
                    // Lowerpart: 144*3 inds

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_TABLE: {
#               ifndef TEAPOT_NO_MESH_TABLE
                {
                    const float A=0.423227,B=1.157334,C=1.058837,D=0.500178,E=0.615603,F=0.541731,G=0.443234,H=1.231206,I=0.500177;
                    const float verts[] = {
                        -1.231206,A,E,-H,D,-E,-H,A,-E,-B,A,E,-H,I,E,-B,A,F,-C,A,E,H,I,E,H,A,-E,H,D,-E,B,A,E,H,A,E,B,A,-G,-B,A,-G,-B,A,-E,C,A,-E,B,A,-E,B,A,-F,-C,A,F,C,A,E,C,A,F,B,A,F,-C,A,-E,-B,A,-F,-C,A,-F,C,A,-F,-B,A,G,-C,-D,F,-B,-D,G,-C,A,G,-C,-D,-G,-C,A,-G,-B,-D,-F,C,A,G,C,A,-G,B,-D,F,C,-D,G,B,A,
                        G,B,-D,-G,C,-D,-F,B,-D,-F,C,-D,-G,B,-D,G,C,-D,F,-C,-D,-F,-B,-D,-G,-C,-D,G,-B,-D,F};

                    const unsigned short inds[] = {
                        0,1,2,0,3,4,0,5,3,1,0,4,4,6,7,7,1,4,7,8,9,7,10,11,11,8,7,11,12,8,13,0,2,1,14,2,9,1,7,9,15,1,8,16,9,8,17,16,3,5,6,6,4,3,6,5,18,6,18,19,19,7,6,
                        19,18,20,19,20,10,10,7,19,10,20,21,10,21,11,14,1,22,14,23,2,14,24,23,22,1,15,22,24,14,22,25,24,15,9,16,15,25,22,15,17,25,16,17,15,5,0,26,5,27,18,26,0,13,26,28,5,13,2,23,13,29,26,
                        13,30,31,23,32,13,18,27,29,29,13,31,29,20,18,29,28,26,31,33,29,31,30,24,24,34,31,24,32,23,20,29,33,20,35,21,33,31,34,33,36,20,34,24,25,34,37,33,34,38,12,25,39,34,21,35,37,37,11,21,37,34,12,
                        37,36,33,12,11,37,12,38,17,17,8,12,17,39,25,39,17,40,40,17,38,40,38,39,38,34,41,41,34,39,41,39,38,36,37,42,42,37,35,42,35,36,35,20,43,43,20,36,43,36,35,32,24,44,44,24,30,44,30,32,30,13,45,
                        45,13,32,45,32,30,28,29,46,46,29,27,46,27,28,27,5,47,47,5,28,47,28,27};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_TORUS: {
#               ifndef TEAPOT_NO_MESH_TORUS
                {
                    const float A=0.059127,B=0.095730,C=0.0,D=0.321378,E=0.300261,F=0.185627,G=0.375276,H=0.216598,I=0.418717,J=0.241738,K=0.238520,L=0.214788,M=0.275726,N=0.248374,O=0.321982,P=0.290005,Q=0.359389,R=0.188644,S=0.259637,T=0.218207,U=0.323590,V=0.305088,W=0.319166,X=0.033586,Y=0.369042,Z=0.038815;
                    const float a=0.430985,b=0.045250,c=0.480861,d=0.050479,e=0.160488,f=0.099149,g=0.352953,h=0.114634,ia=0.412080,j=0.133941,k=0.459945,l=0.149427,m=0.277938,n=0.350540,o=0.066769,p=0.313937,q=0.077227,r=0.363009,s=0.254609,t=0.090099,u=0.423745,v=0.100556,w=0.441845,x=0.196688,y=0.395790,z=0.176175;
                    const float A0=0.339076,B0=0.151036,C0=0.284172,D0=0.391164,E0=0.130522,F0=0.473017,G0=0.293222,H0=0.320775,I0=0.150835,J0=0.300060,K0=0.031575,L0=0.483676,M0=0.433398,N0=0.371053,O0=0.491721,P0=0.104579,Q0=0.093316,R0=0.286988,S0=0.052490,T0=0.499966,U0=0.478246,V0=0.244151,W0=0.336462,X0=0.373668,Y0=0.295435,Z0=0.406851;
                    const float a0=0.122679,b0=0.275525,c0=0.201917,d0=0.224241,e0=0.251391,f0=0.435409,g0=0.204532,h0=0.459342,i0=0.062747,j0=0.261246,k0=0.155460,l0=0.295032,m0=0.301669,n0=0.177382,o0=0.177381;
                    const float verts[] = {
                        0.502782,C,C,L0,C,-A,M0,C,-B,N0,C,-B,H0,C,-A,m0,C,C,H0,C,A,N0,C,B,M0,C,B,L0,C,A,O0,P0,C,F0,v,-A,u,t,-B,r,q,-B,p,o,-A,l0,i0,C,p,o,A,r,q,B,u,t,B,F0,v,A,h0,g0,C,w,x,-A,y,z,-B,A0,B0,-B,G0,E0,-A,b0,a0,C,G0,E0,A,A0,B0,B,y,z,B,w,x,A,Z0,Y0,C,D0,C0,-A,n,s,-B,E,T,-B,S,R,-A,V0,n0,C,S,R,A,
                        E,T,B,n,s,B,D0,C0,A,W0,X0,C,U,Q,-A,P,O,-B,N,M,-B,L,K,-A,c0,d0,C,L,K,A,N,M,B,P,O,B,U,Q,A,e0,f0,C,J,I,-A,H,G,-B,F,D,-B,e,m,-A,I0,j0,C,e,m,A,F,D,B,H,G,B,J,I,A,k0,U0,C,l,k,-A,j,ia,-B,h,g,-B,f,V,-A,Q0,R0,C,f,V,A,h,g,B,j,ia,B,l,k,A,S0,T0,C,d,c,-A,b,a,-B,Z,Y,-B,X,W,-A,K0,J0,C,X,W,A,Z,Y,B,b,a,B,d,c,A,-S0,T0,
                        C,-d,c,-A,-b,a,-B,-Z,Y,-B,-X,W,-A,-K0,J0,C,-X,W,A,-Z,Y,B,-b,a,B,-d,c,A,-k0,U0,C,-l,k,-A,-j,ia,-B,-h,g,-B,-f,V,-A,-Q0,R0,C,-f,V,A,-h,g,B,-j,ia,B,-l,k,A,-e0,f0,C,-J,I,-A,-H,G,-B,-F,D,-B,-e,m,-A,-I0,j0,C,-e,m,A,-F,D,B,-H,G,B,-J,I,A,-W0,X0,C,-U,Q,-A,-P,O,-B,-N,M,-B,-L,K,-A,-c0,d0,C,-L,K,A,-N,M,B,-P,O,B,-U,
                        Q,A,-Z0,Y0,C,-D0,C0,-A,-n,s,-B,-E,T,-B,-S,R,-A,-V0,n0,C,-S,R,A,-E,T,B,-n,s,B,-D0,C0,A,-h0,g0,C,-w,x,-A,-y,z,-B,-A0,B0,-B,-G0,E0,-A,-b0,a0,C,-G0,E0,A,-A0,B0,B,-y,z,B,-w,x,A,-O0,P0,C,-F0,v,-A,-u,t,-B,-r,q,-B,-p,o,-A,-l0,i0,C,-p,o,A,-r,q,B,-u,t,B,-F0,v,A,-0.502782,C,C,-L0,C,-A,-M0,C,-B,-N0,C,-B,-H0,C,-A,
                        -m0,C,C,-H0,C,A,-N0,C,B,-M0,C,B,-L0,C,A,-O0,-P0,C,-F0,-v,-A,-u,-t,-B,-r,-q,-B,-p,-o,-A,-l0,-i0,C,-p,-o,A,-r,-q,B,-u,-t,B,-F0,-v,A,-h0,-g0,C,-w,-x,-A,-y,-z,-B,-A0,-B0,-B,-G0,-E0,-A,-b0,-a0,C,-G0,-E0,A,-A0,-B0,B,-y,-z,B,-w,-x,A,-Z0,-Y0,C,-D0,-C0,-A,-n,-s,-B,-E,-T,-B,-S,-R,-A,-V0,-o0,C,-S,-R,A,-E,-T,B,-n,
                        -s,B,-D0,-C0,A,-W0,-X0,C,-U,-Q,-A,-P,-O,-B,-N,-M,-B,-L,-K,-A,-c0,-d0,C,-L,-K,A,-N,-M,B,-P,-O,B,-U,-Q,A,-e0,-f0,C,-J,-I,-A,-H,-G,-B,-F,-D,-B,-e,-m,-A,-I0,-j0,C,-e,-m,A,-F,-D,B,-H,-G,B,-J,-I,A,-k0,-U0,C,-l,-k,-A,-j,-ia,-B,-h,-g,-B,-f,-V,-A,-Q0,-R0,C,-f,-V,A,-h,-g,B,-j,-ia,B,-l,-k,A,-S0,-T0,C,-d,-c,-A,-b,
                        -a,-B,-Z,-Y,-B,-X,-W,-A,-K0,-J0,C,-X,-W,A,-Z,-Y,B,-b,-a,B,-d,-c,A,S0,-T0,C,d,-c,-A,b,-a,-B,Z,-Y,-B,X,-W,-A,K0,-J0,C,X,-W,A,Z,-Y,B,b,-a,B,d,-c,A,k0,-U0,C,l,-k,-A,j,-ia,-B,h,-g,-B,f,-V,-A,Q0,-R0,C,f,-V,A,h,-g,B,j,-ia,B,l,-k,A,e0,-f0,C,J,-I,-A,H,-G,-B,F,-D,-B,e,-m,-A,I0,-j0,C,e,-m,A,F,-D,B,H,-G,B,J,-I,A,
                        W0,-X0,C,U,-Q,-A,P,-O,-B,N,-M,-B,L,-K,-A,c0,-d0,C,L,-K,A,N,-M,B,P,-O,B,U,-Q,A,Z0,-Y0,C,D0,-C0,-A,n,-s,-B,E,-T,-B,S,-R,-A,V0,-o0,C,S,-R,A,E,-T,B,n,-s,B,D0,-C0,A,h0,-g0,C,w,-x,-A,y,-z,-B,A0,-B0,-B,G0,-E0,-A,b0,-a0,C,G0,-E0,A,A0,-B0,B,y,-z,B,w,-x,A,O0,-P0,C,F0,-v,-A,u,-t,-B,r,-q,-B,p,-o,-A,l0,-i0,C,p,-o,
                        A,r,-q,B,u,-t,B,F0,-v,A};

                    const unsigned short inds[] = {
                        0,1,11,1,2,12,2,3,13,3,4,14,4,5,15,5,6,16,6,7,17,7,8,18,8,9,19,9,0,10,10,11,21,11,12,22,12,13,23,13,14,24,14,15,25,15,16,26,16,17,27,17,18,28,18,19,29,19,10,20,20,21,31,
                        21,22,32,22,23,33,23,24,34,24,25,35,25,26,36,26,27,37,27,28,38,28,29,39,29,20,30,30,31,41,31,32,42,32,33,43,33,34,44,34,35,45,35,36,46,36,37,47,37,38,48,38,39,49,39,30,40,40,41,51,41,42,52,
                        42,43,53,43,44,54,44,45,55,45,46,56,46,47,57,47,48,58,48,49,59,49,40,50,50,51,61,51,52,62,52,53,63,53,54,64,54,55,65,55,56,66,56,57,67,57,58,68,58,59,69,59,50,60,60,61,71,61,62,72,62,63,73,
                        63,64,74,64,65,75,65,66,76,66,67,77,67,68,78,68,69,79,69,60,70,70,71,81,71,72,82,72,73,83,73,74,84,74,75,85,75,76,86,76,77,87,77,78,88,78,79,89,79,70,80,80,81,91,81,82,92,82,83,93,83,84,94,
                        84,85,95,85,86,96,86,87,97,87,88,98,88,89,99,89,80,90,90,91,101,91,92,102,92,93,103,93,94,104,94,95,105,95,96,106,96,97,107,97,98,108,98,99,109,99,90,100,100,101,111,101,102,112,102,103,113,103,104,114,104,105,115,
                        105,106,116,106,107,117,107,108,118,108,109,119,109,100,110,110,111,121,111,112,122,112,113,123,113,114,124,114,115,125,115,116,126,116,117,127,117,118,128,118,119,129,119,110,120,120,121,131,121,122,132,122,123,133,123,124,134,124,125,135,125,126,136,
                        126,127,137,127,128,138,128,129,139,129,120,130,130,131,141,131,132,142,132,133,143,133,134,144,134,135,145,135,136,146,136,137,147,137,138,148,138,139,149,139,130,140,140,141,151,141,142,152,142,143,153,143,144,154,144,145,155,145,146,156,146,147,157,
                        147,148,158,148,149,159,149,140,150,150,151,161,151,152,162,152,153,163,153,154,164,154,155,165,155,156,166,156,157,167,157,158,168,158,159,169,159,150,160,160,161,171,161,162,172,162,163,173,163,164,174,164,165,175,165,166,176,166,167,177,167,168,178,
                        168,169,179,169,160,170,170,171,181,171,172,182,172,173,183,173,174,184,174,175,185,175,176,186,176,177,187,177,178,188,178,179,189,179,170,180,180,181,191,181,182,192,182,183,193,183,184,194,184,185,195,185,186,196,186,187,197,187,188,198,188,189,199,
                        189,180,190,190,191,201,191,192,202,192,193,203,193,194,204,194,195,205,195,196,206,196,197,207,197,198,208,198,199,209,199,190,200,200,201,211,201,202,212,202,203,213,203,204,214,204,205,215,205,206,216,206,207,217,207,208,218,208,209,219,209,200,210,
                        210,211,221,211,212,222,212,213,223,213,214,224,214,215,225,215,216,226,216,217,227,217,218,228,218,219,229,219,210,220,220,221,231,221,222,232,222,223,233,223,224,234,224,225,235,225,226,236,226,227,237,227,228,238,228,229,239,229,220,230,230,231,241,
                        231,232,242,232,233,243,233,234,244,234,235,245,235,236,246,236,237,247,237,238,248,238,239,249,239,230,240,240,241,251,241,242,252,242,243,253,243,244,254,244,245,255,245,246,256,246,247,257,247,248,258,248,249,259,249,240,250,250,251,261,251,252,262,
                        252,253,263,253,254,264,254,255,265,255,256,266,256,257,267,257,258,268,258,259,269,259,250,260,260,261,271,261,262,272,262,263,273,263,264,274,264,265,275,265,266,276,266,267,277,267,268,278,268,269,279,269,260,270,270,271,281,271,272,282,272,273,283,
                        273,274,284,274,275,285,275,276,286,276,277,287,277,278,288,278,279,289,279,270,280,280,281,291,281,282,292,282,283,293,283,284,294,284,285,295,285,286,296,286,287,297,287,288,298,288,289,299,289,280,290,290,291,1,291,292,2,292,293,3,293,294,4,
                        294,295,5,295,296,6,296,297,7,297,298,8,298,299,9,299,290,0,0,11,10,1,12,11,2,13,12,3,14,13,4,15,14,5,16,15,6,17,16,7,18,17,8,19,18,9,10,19,10,21,20,11,22,21,12,23,22,13,24,23,14,25,24,
                        15,26,25,16,27,26,17,28,27,18,29,28,19,20,29,20,31,30,21,32,31,22,33,32,23,34,33,24,35,34,25,36,35,26,37,36,27,38,37,28,39,38,29,30,39,30,41,40,31,42,41,32,43,42,33,44,43,34,45,44,35,46,45,
                        36,47,46,37,48,47,38,49,48,39,40,49,40,51,50,41,52,51,42,53,52,43,54,53,44,55,54,45,56,55,46,57,56,47,58,57,48,59,58,49,50,59,50,61,60,51,62,61,52,63,62,53,64,63,54,65,64,55,66,65,56,67,66,
                        57,68,67,58,69,68,59,60,69,60,71,70,61,72,71,62,73,72,63,74,73,64,75,74,65,76,75,66,77,76,67,78,77,68,79,78,69,70,79,70,81,80,71,82,81,72,83,82,73,84,83,74,85,84,75,86,85,76,87,86,77,88,87,
                        78,89,88,79,80,89,80,91,90,81,92,91,82,93,92,83,94,93,84,95,94,85,96,95,86,97,96,87,98,97,88,99,98,89,90,99,90,101,100,91,102,101,92,103,102,93,104,103,94,105,104,95,106,105,96,107,106,97,108,107,98,109,108,
                        99,100,109,100,111,110,101,112,111,102,113,112,103,114,113,104,115,114,105,116,115,106,117,116,107,118,117,108,119,118,109,110,119,110,121,120,111,122,121,112,123,122,113,124,123,114,125,124,115,126,125,116,127,126,117,128,127,118,129,128,119,120,129,
                        120,131,130,121,132,131,122,133,132,123,134,133,124,135,134,125,136,135,126,137,136,127,138,137,128,139,138,129,130,139,130,141,140,131,142,141,132,143,142,133,144,143,134,145,144,135,146,145,136,147,146,137,148,147,138,149,148,139,140,149,140,151,150,
                        141,152,151,142,153,152,143,154,153,144,155,154,145,156,155,146,157,156,147,158,157,148,159,158,149,150,159,150,161,160,151,162,161,152,163,162,153,164,163,154,165,164,155,166,165,156,167,166,157,168,167,158,169,168,159,160,169,160,171,170,161,172,171,
                        162,173,172,163,174,173,164,175,174,165,176,175,166,177,176,167,178,177,168,179,178,169,170,179,170,181,180,171,182,181,172,183,182,173,184,183,174,185,184,175,186,185,176,187,186,177,188,187,178,189,188,179,180,189,180,191,190,181,192,191,182,193,192,
                        183,194,193,184,195,194,185,196,195,186,197,196,187,198,197,188,199,198,189,190,199,190,201,200,191,202,201,192,203,202,193,204,203,194,205,204,195,206,205,196,207,206,197,208,207,198,209,208,199,200,209,200,211,210,201,212,211,202,213,212,203,214,213,
                        204,215,214,205,216,215,206,217,216,207,218,217,208,219,218,209,210,219,210,221,220,211,222,221,212,223,222,213,224,223,214,225,224,215,226,225,216,227,226,217,228,227,218,229,228,219,220,229,220,231,230,221,232,231,222,233,232,223,234,233,224,235,234,
                        225,236,235,226,237,236,227,238,237,228,239,238,229,230,239,230,241,240,231,242,241,232,243,242,233,244,243,234,245,244,235,246,245,236,247,246,237,248,247,238,249,248,239,240,249,240,251,250,241,252,251,242,253,252,243,254,253,244,255,254,245,256,255,
                        246,257,256,247,258,257,248,259,258,249,250,259,250,261,260,251,262,261,252,263,262,253,264,263,254,265,264,255,266,265,256,267,266,257,268,267,258,269,268,259,260,269,260,271,270,261,272,271,262,273,272,263,274,273,264,275,274,265,276,275,266,277,276,
                        267,278,277,268,279,278,269,270,279,270,281,280,271,282,281,272,283,282,273,284,283,274,285,284,275,286,285,276,287,286,277,288,287,278,289,288,279,280,289,280,291,290,281,292,291,282,293,292,283,294,293,284,295,294,285,296,295,286,297,296,287,298,297,
                        288,299,298,289,290,299,290,1,0,291,2,1,292,3,2,293,4,3,294,5,4,295,6,5,296,7,6,297,8,7,298,9,8,299,0,9};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CUBE: {
#               ifndef TEAPOT_NO_MESH_CUBE
                {
                    const float A=0.5;
                    const float verts[] = {-A,-A,-A,-A,-A,A,A,-A,-A,A,-A,A,-A,A,-A,-A,A,A,A,A,-A,A,A,A,-A,-A,A,-A,A,A,A,-A,A,A,A,A,-A,-A,-A,-A,-A,A,-A,A,-A,-A,A,A,-A,-A,-A,-A,A,-A,A,-A,-A,A,A,-A,A,-A,-A,A,-A,A,A,A,-A,A,A,A};

                    const unsigned short inds[] = {
                        3,0,2,5,6,4,9,10,11,13,14,12,19,16,17,23,20,22,3,1,0,5,7,6,9,8,10,13,15,14,19,18,16,23,21,20};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CUBIC_GROUND: {
#               ifndef TEAPOT_NO_MESH_CUBIC_GROUND
                {
                    const float A=0.5,B=0.3,C=0.1;
                    const float verts[] = {
                        A,-A,A,-A,-A,-A,A,-A,-A,-A,A,A,A,A,-A,-A,A,-A,-A,A,A,A,-A,A,A,A,A,-A,-A,A,-A,A,-A,-A,-A,-A,A,A,-A,-A,-A,-A,-A,A,-A,A,A,A,A,-A,-A,A,A,-A,-A,-A,A,A,A,A,-A,-A,A,-A,A,A,A,-A,-A,A,-A,A,-A,A,B,-A,A,C,-A,A,-C,-A,A,-B,A,A,-B,A,A,-C,A,A,C,A,A,B,-B,A,A,-C,A,A,C,A,A,B,A,A,B,A,-A,C,A,-A,
                        -C,A,-A,-B,A,-A,-B,A,B,-B,A,C,-B,A,-C,-B,A,-B,-C,A,B,-C,A,C,-C,A,-C,-C,A,-B,C,A,B,C,A,C,C,A,-C,C,A,-B,B,A,B,B,A,C,B,A,-C,B,A,-B};

                    const unsigned short inds[] = {
                        0,1,2,6,7,8,9,10,11,12,13,14,15,16,17,0,18,1,6,20,7,9,21,10,12,22,13,15,23,16,28,36,55,43,5,27,47,39,43,51,38,47,55,37,51,32,24,3,40,25,24,41,26,25,42,27,26,33,40,32,44,41,40,
                        45,42,41,46,43,42,34,44,33,48,45,44,49,46,45,50,47,46,35,48,34,52,49,48,53,50,49,54,51,50,19,52,35,31,53,52,30,54,53,29,55,54,28,4,36,43,39,5,47,38,39,51,37,38,55,36,37,32,40,24,40,41,25,
                        41,42,26,42,43,27,33,44,40,44,45,41,45,46,42,46,47,43,34,48,44,48,49,45,49,50,46,50,51,47,35,52,48,52,53,49,53,54,50,54,55,51,19,31,52,31,30,53,30,29,54,29,28,55};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_PLANE_X: {
#               ifndef TEAPOT_MESH_PLANE_X
                {
                    const float A=0.5;const float verts[] = {0,-A,-A,0,-A,A,0,A,A,0,A,-A};
                    const unsigned short inds[] = {
                        3,2,0,2,1,0    // Front
#                       ifndef TEAPOT_NO_MESH_PLANE_X_BACK_FACE
                        ,0,1,2,2,3,0   // Back
#                       endif //TEAPOT_NO_MESH_PLANE_X_BACK_FACE
                    };
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_PLANE_Y: {
#               ifndef TEAPOT_MESH_PLANE_Y
                {
                    const float A=0.5;const float verts[] = {-A,0,-A,   A,0,-A, A,0,A,  -A,0,A};
                    const unsigned short inds[] = {
                        3,2,0,2,1,0     // Front
#                       ifndef TEAPOT_NO_MESH_PLANE_Y_BACK_FACE
                        ,0,1,2,2,3,0    // Back
#                       endif //TEAPOT_NO_MESH_PLANE_Y_BACK_FACE
                    };
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_PLANE_Z: {
#               ifndef TEAPOT_MESH_PLANE_Z
                {
                    const float A=0.5;const float verts[] = {-A,-A,0,A,-A,0,A,A,0,-A,A,0};
                    const unsigned short inds[] = {
                        0,1,2,2,3,0     // Front
#                       ifndef TEAPOT_NO_MESH_PLANE_Z_BACK_FACE
                        ,3,2,0,2,1,0    // Back
#                       endif //TEAPOT_NO_MESH_PLANE_Z_BACK_FACE
                    };
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CYLINDER: {
#               ifndef TEAPOT_NO_MESH_CYLINDER
                {
                    const float A=0.5,B=0.365914,C=0.133934,D=0.499848,E=0.499847,F=0.133933,G=0.0;
                    const float verts[] = {
                        G,-A,G,-C,-A,-D,C,-A,-D,B,-A,-B,D,-A,-C,D,-A,C,B,-A,B,C,-A,D,-C,-A,D,-B,-A,B,-E,-A,C,-D,-A,-F,-B,-A,-B,G,A,G,-C,A,-D,C,A,-D,B,A,-B,D,A,-C,D,A,C,B,A,B,C,A,D,-C,A,D,-B,A,B,-E,A,C,-D,A,-F,-B,A,-B,-C,-A,-D,-C,A,-D,C,-A,-D,C,A,-D,B,-A,-B,B,A,-B,D,-A,-C,D,A,-C,D,-A,C,D,A,C,B,-A,B,
                        B,A,B,C,-A,D,C,A,D,-C,-A,D,-C,A,D,-B,-A,B,-B,A,B,-E,-A,C,-E,A,C,-D,-A,-F,-D,A,-F,-B,-A,-B,-B,A,-B};

                    const unsigned short inds[] = {
                        0,1,2,0,2,3,0,3,4,0,4,5,0,5,6,0,6,7,0,7,8,0,8,9,0,9,10,0,10,11,0,11,12,0,12,1,13,15,14,13,16,15,13,17,16,13,18,17,13,19,18,13,20,19,13,21,20,13,22,21,13,23,22,
                        13,24,23,13,25,24,13,14,25,27,28,26,29,30,28,31,32,30,33,34,32,35,36,34,37,38,36,39,40,38,41,42,40,43,44,42,45,46,44,47,48,46,49,26,48,27,29,28,29,31,30,31,33,32,33,35,34,35,37,36,37,39,38,
                        39,41,40,41,43,42,43,45,44,45,47,46,47,49,48,49,27,26};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CONE1: {
#               ifndef TEAPOT_NO_MESH_CONE
                {
                    const float A=0.5,B=0.499955,C=0.365992,D=0.133962,E=0.133963,F=0.0,G=0.365993;
                    const float verts[] = {F,A,F,C,-A,-C,B,-A,-D,D,-A,-B,-E,-A,-B,-G,-A,-C,-B,-A,-D,-B,-A,E,-C,-A,C,-D,-A,B,E,-A,B,C,-A,C,B,-A,E,F,-A,F,D,-A,-B,C,-A,-C,B,-A,-D,B,-A,E,C,-A,C,E,-A,B,-D,-A,B,-C,-A,C,-B,-A,E,-B,-A,-D,-G,-A,-C,-E,-A,-B};

                    const unsigned short inds[] = {
                        1,0,2,3,0,1,4,0,3,5,0,4,6,0,5,7,0,6,8,0,7,9,0,8,10,0,9,11,0,10,12,0,11,2,0,12,13,14,15,13,15,16,13,16,17,13,17,18,13,18,19,13,19,20,13,20,21,13,21,22,13,22,23,
                        13,23,24,13,24,25,13,25,14};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CONE2: {
#               ifndef TEAPOT_NO_MESH_CONE2
                {
                    const float A=0.5,B=0.0,C=0.499955,D=0.365992,E=0.239978,F=0.064302,G=0.133962,H=0.133963,I=0.175676,J=0.365993;
                    const float verts[] = {
                        I,B,-I,B,A,B,E,B,-F,F,B,-E,-F,B,-E,-0.175677,B,-I,-E,B,-F,-E,-B,F,-I,-B,I,-F,-B,E,F,-B,E,I,-B,I,E,-B,F,B,-A,-B,G,-A,-C,D,-A,-D,C,-A,-G,C,-A,H,D,-A,D,H,-A,C,-G,-A,C,-D,-A,D,-C,-A,H,-C,-A,-G,-J,-A,-D,-H,-A,-C,C,-A,-G,C,-A,H,D,-A,D,H,-A,C,-G,-A,C,-D,-A,D,-C,-A,H,-C,-A,-G,-J,
                        -A,-D,-H,-A,-C,G,-A,-C,D,-A,-D};

                    const unsigned short inds[] = {
                        0,1,2,3,1,0,4,1,3,5,1,4,6,1,5,7,1,6,8,1,7,9,1,8,10,1,9,11,1,10,12,1,11,2,1,12,13,14,15,13,15,16,13,16,17,13,17,18,13,18,19,13,19,20,13,20,21,13,21,22,13,22,23,
                        13,23,24,13,24,25,13,25,14,26,12,27,12,28,27,28,10,29,10,30,29,9,31,30,31,7,32,7,33,32,33,5,34,5,35,34,4,36,35,3,37,36,37,2,26,26,2,12,12,11,28,28,11,10,10,9,30,9,8,31,31,8,7,
                        7,6,33,33,6,5,5,4,35,4,3,36,3,0,37,37,0,2};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_PYRAMID: {
#               ifndef TEAPOT_NO_MESH_PYRAMID
                {
                    const float A=0.5,B=0.0;
                    const float verts[] = {A,-A,A,-A,-A,-A,A,-A,-A,-A,-A,A,B,A,B,-A,-A,-A,-A,-A,A,B,A,B,-A,-A,A,A,-A,A,B,A,B,A,-A,-A,-A,-A,-A,B,A,B,A,-A,A,A,-A,-A};

                    const unsigned short inds[] = {
                        0,1,2,3,4,5,0,6,1,7,8,9,10,11,12,13,14,15};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_ROOF: {
#               ifndef TEAPOT_NO_MESH_ROOF
                {
                    const float A=0.5,B=0.0;
                    const float verts[] = {A,-A,A,-A,-A,-A,A,-A,-A,B,A,A,A,-A,A,-A,-A,A,B,A,-A,-A,-A,-A,B,A,-A,-A,-A,-A,B,A,A,A,-A,-A,B,A,-A,-A,-A,A,-A,-A,A,B,A,A,A,-A,-A,A,-A,A};

                    const unsigned short inds[] = {
                        0,1,2,5,6,7,10,11,12,0,13,1,3,14,4,5,15,6,8,16,9,10,17,11};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_SPHERE1: {
#               ifndef TEAPOT_NO_MESH_SPHERE1
                {
                    const float A=0.0,B=0.223610,C=0.154506,D=0.249998,E=0.425327,F=0.262868,G=0.404508,H=0.293893,I=0.475529,J=0.344095,K=0.262869,L=0.404506,M=0.131434,N=0.212661,O=0.081228,P=0.425325,Q=0.138194,R=0.262863,S=0.361804,T=0.5,U=0.223608,V=0.447213,W=0.425324,X=0.262865,Y=0.425326;
                    const float verts[] = {
                        A,A,-T,S,-R,-B,-Q,-P,-B,-V,A,-U,-Q,P,-B,S,R,-B,Q,-P,B,-S,-R,B,-S,R,B,Q,P,B,V,-A,U,A,-A,T,-O,-D,-E,N,-C,-E,M,-L,-K,W,A,-F,N,C,-E,-X,A,-Y,-J,-D,-F,-O,D,-E,-J,D,-F,M,L,-K,I,-C,-A,I,C,A,A,-T,-A,H,-G,-A,-I,-C,-A,-H,-G,-A,-H,G,A,-I,C,A,H,G,A,A,T,A,J,-D,F,-M,-L,K,-W,-A,F,-M,L,K,J,D,
                        F,O,-D,E,X,-A,Y,-N,-C,E,-N,C,E,O,D,E};

                    const unsigned short inds[] = {
                        0,13,12,1,13,15,0,12,17,0,17,19,0,19,16,1,15,22,2,14,24,3,18,26,4,20,28,5,21,30,1,22,25,2,24,27,3,26,29,4,28,31,5,30,23,6,32,37,7,33,39,8,34,40,9,35,41,10,36,38,38,41,11,
                        38,36,41,36,9,41,41,40,11,41,35,40,35,8,40,40,39,11,40,34,39,34,7,39,39,37,11,39,33,37,33,6,37,37,38,11,37,32,38,32,10,38,23,36,10,23,30,36,30,9,36,31,35,9,31,28,35,28,8,35,29,34,8,
                        29,26,34,26,7,34,27,33,7,27,24,33,24,6,33,25,32,6,25,22,32,22,10,32,30,31,9,30,21,31,21,4,31,28,29,8,28,20,29,20,3,29,26,27,7,26,18,27,18,2,27,24,25,6,24,14,25,14,1,25,22,23,10,
                        22,15,23,15,5,23,16,21,5,16,19,21,19,4,21,19,20,4,19,17,20,17,3,20,17,18,3,17,12,18,12,2,18,15,16,5,15,13,16,13,0,16,12,14,2,12,13,14,13,1,14};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_SPHERE2: {
#               ifndef TEAPOT_NO_MESH_SPHERE2
                {
                    const float A=0.0,B=0.125576,C=0.251151,D=0.361805,E=0.138198,F=0.138199,G=0.361806,H=0.223610,I=0.154506,J=0.073809,K=0.125575,L=0.483975,M=0.425327,N=0.249998,O=0.358282,P=0.344095,Q=0.081229,R=0.447215,S=0.5,T=0.223608,U=0.212661,V=0.425324,W=0.223605,X=0.262868,Y=0.475528,Z=0.081228;
                    const float a=0.430349,b=0.343579,c=0.477711,d=0.077608,e=0.221429,f=0.119427,g=0.478313,h=0.251149,ia=0.340858,j=0.293893,k=0.404508,l=0.218003,m=0.432094,n=0.131431,o=0.319097,p=0.131432,q=0.475529,r=0.262864,s=0.069099,t=0.026395,u=0.180902,v=0.430902,w=0.361804,x=0.425325,y=0.138194,z=0.116411;
                    const float A0=0.328760,B0=0.262863,C0=0.038803,D0=0.119426,E0=0.101590,F0=0.304773,G0=0.221428,H0=0.328759,I0=0.265970,J0=0.340856,K0=0.131434,L0=0.241986,M0=0.262869,N0=0.014820,O0=0.432092,P0=0.406365,Q0=0.147619,R0=0.251150,S0=0.404506,T0=0.415525,U0=0.473607,V0=0.335409,W0=0.335410,X0=0.212662,Y0=0.404509,Z0=0.069100;
                    const float a0=0.154509,b0=0.447214,c0=0.212660,d0=0.262865,e0=0.483974,f0=0.180901,g0=0.344093,h0=0.293890,i0=0.125573,j0=0.293892,k0=0.475527,l0=0.425326,m0=0.447213,n0=0.328757,o0=0.376721,p0=0.180900,q0=0.344092,r0=0.293891,s0=0.293889,t0=0.335408;
                    const float verts[] = {
                        A,A,-S,w,-B0,-H,-y,-x,-H,-m0,A,-T,-y,x,-H,w,B0,-H,y,-x,H,-w,-B0,H,-w,B0,H,y,x,H,m0,-A,T,A,-A,S,-z,-O,-A0,-Z,-N,-M,-C0,-D0,-L,E0,-J,-L,U,-I,-M,F0,-G0,-H0,I0,-J0,-C,K0,-S0,-M0,-N0,-O0,-C,P0,Q0,-R0,V,A,-X,P0,-Q0,-R0,E0,J,-L,U,I,-M,F0,G0,-H0,-o0,A,-n0,-d0,A,-l0,-i0,A,-e0,-L0,-O,
                        -C,-P,-N,-X,-T0,-f,-h,-z,O,-A0,-Z,N,-M,-C0,D0,-L,-T0,f,-h,-P,N,-X,-L0,O,-C,-N0,O0,-C,K0,S0,-M0,I0,J0,-C,g,-J,K,q,-I,-A,a,-e,-K,a,e,-K,q,I,A,g,J,K,d,-c,B,A,-S,-A,-d,-c,-B,b,-ia,-B,j,-k,-A,l,-m,B,-a,-e,K,-q,-I,-A,-g,-J,-K,-l,-m,-B,-j,-k,-A,-b,-ia,B,-b,ia,B,-j,k,A,-l,m,-B,-g,J,-K,-q,I,A,-a,e,K,l,m,B,j,k,
                        A,b,ia,-B,-d,c,-B,A,S,A,d,c,B,T0,-f,h,P,-N,X,L0,-O,C,N0,-O0,C,-K0,-S0,M0,-I0,-J0,C,-P0,-Q0,R0,-V,-A,X,-P0,Q0,R0,-I0,J0,C,-K0,S0,M0,N0,O0,C,L0,O,C,P,N,X,T0,f,h,C0,-D0,L,Z,-N,M,z,-O,A0,o0,-A,n0,d0,-A,l0,i0,-A,e0,-E0,-J,L,-U,-I,M,-F0,-G0,H0,-E0,J,L,-U,I,M,-F0,G0,H0,C0,D0,L,Z,N,M,z,O,A0,p0,n,R,o,p,D,W,r,
                        G,-s,c0,R,-t,q0,G,-u,s0,G,-W,-A,R,-t0,Q,D,-t0,-Q,D,-s,-c0,R,-u,-s0,G,-t,-q0,G,p0,-n,R,W,-r,G,o,-p,D,v,U,E,0.404510,r0,A,W0,P,F,-s,Y,F,-0.154508,Y,-A,-T,V,F,-U0,Q,E,-S,-A,A,-0.473606,-Q,E,-T,-V,E,-a0,-Y,-A,-Z0,-Y,E,W0,-P,E,Y0,-r0,-0.000001,v,-X0,0.138197,a0,Y,A,T,V,-F,Z0,k0,-F,-Y0,j0,A,-V0,P,-F,-v,X0,
                        -E,-Y0,-j0,-A,-v,-X0,-E,-W0,-P,-F,a0,-Y,-A,Z0,-k0,-F,T,-V,-F,S,A,A,U0,-Q,-E,U0,Q,-E,u,h0,-G,0.069098,U,-R,t,g0,-D,-W,r,-G,-f0,n,-b0,-o,n,-D,-o,-p,-D,-f0,-p,-b0,-W,-r,-D,V0,Z,-D,V0,-Q,-D,W,-A,-b0,t,-g0,-G,s,-c0,-R,u,-h0,-D};

                    const unsigned short inds[] = {
                        0,15,14,1,17,23,0,14,29,0,29,35,0,35,24,1,23,44,2,20,50,3,32,56,4,38,62,5,41,68,1,44,51,2,50,57,3,56,63,4,62,69,5,68,45,6,74,89,7,77,95,8,80,98,9,83,101,10,86,90,92,99,11,
                        91,102,92,90,103,91,92,102,99,102,100,99,91,103,102,103,104,102,102,104,100,104,101,100,90,86,103,86,85,103,103,85,104,85,84,104,104,84,101,84,9,101,99,96,11,100,105,99,101,106,100,99,105,96,105,97,96,100,106,105,106,107,105,
                        105,107,97,107,98,97,101,83,106,83,82,106,106,82,107,82,81,107,107,81,98,81,8,98,96,93,11,97,108,96,98,109,97,96,108,93,108,94,93,97,109,108,109,110,108,108,110,94,110,95,94,98,80,109,80,79,109,109,79,110,79,78,110,
                        110,78,95,78,7,95,93,87,11,94,111,93,95,112,94,93,111,87,111,88,87,94,112,111,112,113,111,111,113,88,113,89,88,95,77,112,77,76,112,112,76,113,76,75,113,113,75,89,75,6,89,87,92,11,88,114,87,89,115,88,87,114,92,
                        114,91,92,88,115,114,115,116,114,114,116,91,116,90,91,89,74,115,74,73,115,115,73,116,73,72,116,116,72,90,72,10,90,47,86,10,46,117,47,45,118,46,47,117,86,117,85,86,46,118,117,118,119,117,117,119,85,119,84,85,45,68,118,
                        68,67,118,118,67,119,67,66,119,119,66,84,66,9,84,71,83,9,70,120,71,69,121,70,71,120,83,120,82,83,70,121,120,121,122,120,120,122,82,122,81,82,69,62,121,62,61,121,121,61,122,61,60,122,122,60,81,60,8,81,65,80,8,
                        64,123,65,63,124,64,65,123,80,123,79,80,64,124,123,124,125,123,123,125,79,125,78,79,63,56,124,56,55,124,124,55,125,55,54,125,125,54,78,54,7,78,59,77,7,58,126,59,57,127,58,59,126,77,126,76,77,58,127,126,127,128,126,
                        126,128,76,128,75,76,57,50,127,50,49,127,127,49,128,49,48,128,128,48,75,48,6,75,53,74,6,52,129,53,51,130,52,53,129,74,129,73,74,52,130,129,130,131,129,129,131,73,131,72,73,51,44,130,44,43,130,130,43,131,43,42,131,
                        131,42,72,42,10,72,66,71,9,67,132,66,68,133,67,66,132,71,132,70,71,67,133,132,133,134,132,132,134,70,134,69,70,68,41,133,41,40,133,133,40,134,40,39,134,134,39,69,39,4,69,60,65,8,61,135,60,62,136,61,60,135,65,
                        135,64,65,61,136,135,136,137,135,135,137,64,137,63,64,62,38,136,38,37,136,136,37,137,37,36,137,137,36,63,36,3,63,54,59,7,55,138,54,56,139,55,54,138,59,138,58,59,55,139,138,139,140,138,138,140,58,140,57,58,56,32,139,
                        32,31,139,139,31,140,31,30,140,140,30,57,30,2,57,48,53,6,49,141,48,50,142,49,48,141,53,141,52,53,49,142,141,142,143,141,141,143,52,143,51,52,50,20,142,20,19,142,142,19,143,19,18,143,143,18,51,18,1,51,42,47,10,
                        43,144,42,44,145,43,42,144,47,144,46,47,43,145,144,145,146,144,144,146,46,146,45,46,44,23,145,23,22,145,145,22,146,22,21,146,146,21,45,21,5,45,26,41,5,25,147,26,24,148,25,26,147,41,147,40,41,25,148,147,148,149,147,
                        147,149,40,149,39,40,24,35,148,35,34,148,148,34,149,34,33,149,149,33,39,33,4,39,33,38,4,34,150,33,35,151,34,33,150,38,150,37,38,34,151,150,151,152,150,150,152,37,152,36,37,35,29,151,29,28,151,151,28,152,28,27,152,
                        152,27,36,27,3,36,27,32,3,28,153,27,29,154,28,27,153,32,153,31,32,28,154,153,154,155,153,153,155,31,155,30,31,29,14,154,14,13,154,154,13,155,13,12,155,155,12,30,12,2,30,21,26,5,22,156,21,23,157,22,21,156,26,
                        156,25,26,22,157,156,157,158,156,156,158,25,158,24,25,23,17,157,17,16,157,157,16,158,16,15,158,158,15,24,15,0,24,12,20,2,13,159,12,14,160,13,12,159,20,159,19,20,13,160,159,160,161,159,159,161,19,161,18,19,14,15,160,
                        15,16,160,160,16,161,16,17,161,161,17,18,17,1,18};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_ARROW: {
#               ifndef TEAPOT_NO_MESH_ARROW
                {
                    const float A=0.500001,B=0.099617,C=0.021451,D=0.080053,E=0.058602,F=0.187664,G=0.050284,H=0.100574,I=0.137380,J=0.0,K=0.137378;
                    const float verts[] = {
                        -K,-B,-K,-J,-A,J,-F,-B,-G,-G,-B,-F,G,-B,-F,I,-B,-K,F,-B,-G,F,-B,G,I,-B,I,G,-B,F,-G,-B,F,-K,-B,I,-F,-B,G,-J,-B,J,-G,-B,-F,-I,-B,-K,-F,-B,-G,-F,-B,G,-I,-B,I,-G,-B,F,G,-B,F,I,-B,I,F,-B,G,F,-B,-G,I,-B,-K,G,-B,-F,J,A,-J,C,A,-D,-C,A,-D,-E,A,-E,-D,A,-C,-D,A,C,-E,A,E,-C,A,D,C,A,
                        D,E,A,E,D,A,C,D,A,-C,E,A,-E,C,-H,-D,-C,A,-D,C,A,-D,-C,-H,-D,-E,A,-E,-E,-H,-E,-D,A,-C,-D,-H,-C,-D,A,C,-D,-H,C,-E,A,E,-E,-H,E,-C,A,D,-C,-H,D,C,A,D,C,-H,D,E,A,E,E,-H,E,D,A,C,D,-H,C,D,A,-C,D,-H,-C,E,A,-E,E,-H,-E};

                    const unsigned short inds[] = {
                        0,1,2,3,1,0,4,1,3,5,1,4,6,1,5,7,1,6,8,1,7,9,1,8,10,1,9,11,1,10,12,1,11,2,1,12,13,14,15,13,15,16,13,16,17,13,17,18,13,18,19,13,19,20,13,20,21,13,21,22,13,22,23,
                        13,23,24,13,24,25,13,25,14,26,27,28,26,28,29,26,29,30,26,30,31,26,31,32,26,32,33,26,33,34,26,34,35,26,35,36,26,36,37,26,37,38,26,38,27,39,40,41,42,43,40,44,45,43,46,47,45,48,49,47,50,51,49,
                        52,53,51,54,55,53,56,57,55,58,59,57,60,61,59,62,41,61,39,42,40,42,44,43,44,46,45,46,48,47,48,50,49,50,52,51,52,54,53,54,56,55,56,58,57,58,60,59,60,62,61,62,39,41};

                    // Probably wrong. We Must do a AddMeshVertsAndIndsLines()
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_CYLINDER_LATERAL_SURFACE: {
#               ifndef TEAPOT_NO_MESH_CYLINDER
                {
                    TIS.startInds[i] = TIS.startInds[TEAPOT_MESH_CYLINDER]+72;
                    TIS.numInds[i] = TIS.numInds[TEAPOT_MESH_CYLINDER]-72;
                    for (j=0;j<3;j++) {
                        TIS.halfExtents[i][j] = TIS.halfExtents[TEAPOT_MESH_CYLINDER][j];
                        TIS.centerPoint[i][j] = TIS.centerPoint[TEAPOT_MESH_CYLINDER][j];
                        TIS.aabbMin[i][j] = TIS.aabbMin[TEAPOT_MESH_CYLINDER][j];
                        TIS.aabbMax[i][j] = TIS.aabbMax[TEAPOT_MESH_CYLINDER][j];
                    }
                }
#               endif
            }
                break;                               
            case TEAPOT_MESH_HALF_SPHERE_UP: {
#               ifndef TEAPOT_MESH_HALF_SPHERE_UP
                {
                    const float A=0.0,B=0.461940,C=0.353553,D=0.191342,E=0.151083,F=0.361376,G=0.124227,H=0.214853,I=0.463769,J=0.428467,K=0.151924,L=0.327934,M=0.082221,N=0.177477,O=0.007646,P=0.511063,Q=0.007064,R=0.472161,S=0.229541,T=0.279164,U=0.299910,V=0.364746,W=0.324620,X=0.394798,Y=0.046372,Z=0.190021;
                    const float a=0.085685,b=0.111952,c=0.458751,d=0.121176,e=0.496548,f=0.312665,g=0.075143,h=0.353519,ia=0.098179,j=0.191323,k=0.106268,l=0.499951,m=0.119652,n=0.154731,o=0.221087,p=0.285906,q=0.373554,r=0.404332,s=0.174938,t=0.087493,u=0.323243,v=0.161666,w=0.422337,x=0.211227,y=0.457134,z=0.228631;
                    const float A0=0.195575,B0=0.002926;
                    const float verts[] = {
                        0.040667,B,-j,g,C,-h,ia,D,-0.461895,k,-A,-l,-A,0.5,-A,m,B,-n,o,C,-p,0.288864,D,-q,f,-A,-r,s,B,-t,u,C,-v,w,D,-x,y,-A,-z,A0,B,-B0,F,C,-0.005407,R,D,-Q,P,-A,-O,N,B,M,L,C,K,J,D,0.198498,I,-A,H,G,B,E,S,C,T,U,D,V,W,-A,X,Y,B,Z,a,C,0.351112,b,D,c,d,-A,e,-0.040667,B,j,-g,C,h,-ia,D,0.461894,
                        -k,-A,l,-m,B,n,-o,C,p,-0.288865,D,q,-f,-A,r,-s,B,t,-u,C,v,-w,D,x,-y,-A,z,-A0,B,B0,-F,C,0.005406,-R,D,Q,-P,-A,O,-N,B,-M,-L,C,-K,-J,D,-0.198499,-I,-A,-H,-G,B,-E,-S,C,-T,-U,D,-V,-W,-A,-X,-Y,B,-Z,-a,C,-0.351113,-b,D,-c,-d,-A,-e};

                    const unsigned short inds[] = {
                        1,7,2,0,4,5,2,8,3,1,5,6,5,4,9,7,12,8,5,10,6,6,11,7,11,16,12,10,13,14,10,15,11,9,4,13,14,19,15,13,4,17,15,20,16,13,18,14,18,23,19,17,4,21,19,24,20,17,22,18,24,27,28,
                        22,25,26,22,27,23,21,4,25,28,31,32,25,30,26,26,31,27,25,4,29,31,34,35,29,4,33,32,35,36,30,33,34,34,39,35,33,4,37,36,39,40,33,38,34,39,44,40,38,41,42,38,43,39,37,4,41,42,45,46,42,47,43,
                        41,4,45,43,48,44,46,51,47,45,4,49,47,52,48,45,50,46,49,4,53,51,56,52,50,53,54,50,55,51,56,2,3,53,1,54,54,2,55,53,4,0,1,6,7,2,7,8,1,0,5,7,11,12,5,9,10,6,10,11,11,15,16,
                        10,9,13,10,14,15,14,18,19,15,19,20,13,17,18,18,22,23,19,23,24,17,21,22,24,23,27,22,21,25,22,26,27,28,27,31,25,29,30,26,30,31,31,30,34,32,31,35,30,29,33,34,38,39,36,35,39,33,37,38,39,43,44,
                        38,37,41,38,42,43,42,41,45,42,46,47,43,47,48,46,50,51,47,51,52,45,49,50,51,55,56,50,49,53,50,54,55,56,55,2,53,0,1,54,1,2};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_HALF_SPHERE_DOWN: {
#               ifndef TEAPOT_MESH_HALF_SPHERE_DOWN
                {
                    const float A=0.0,B=0.461940,C=0.191342,D=0.353553,E=0.353554,F=0.124227,G=0.002926,H=0.214853,I=0.463769,J=0.428467,K=0.151924,L=0.082221,M=0.177477,N=0.511063,O=0.007064,P=0.472161,Q=0.361376,R=0.007646,S=0.151083,T=0.229541,U=0.279164,V=0.299910,W=0.364746,X=0.324620,Y=0.394798,Z=0.046372;
                    const float a=0.190021,b=0.458751,c=0.121176,d=0.496548,e=0.373554,f=0.075143,g=0.353519,h=0.098179,ia=0.106268,j=0.499951,k=0.191323,l=0.154731,m=0.221087,n=0.285906,o=0.312665,p=0.404332,q=0.174938,r=0.228631,s=0.195575,t=0.457134,u=0.211227,v=0.422337,w=0.161666,x=0.323243,y=0.087493;
                    const float verts[] = {
                        -0.040667,-B,-k,-f,-D,-g,-h,-C,-0.461895,-ia,A,-j,-A,-0.5,-A,-0.119652,-B,-l,-m,-D,-n,-0.288865,-C,-e,-o,A,-p,-q,-B,-y,-x,-D,-w,-v,-C,-u,-t,A,-r,-s,-B,-G,-Q,-D,-0.005407,-P,-C,-O,-N,A,-R,-M,-B,L,-0.327935,-D,K,-J,-C,0.198498,-I,A,H,-F,-B,S,-T,-D,U,-V,-C,W,-X,A,Y,-Z,-B,a,-0.085685,
                        -D,0.351112,-0.111953,-C,b,-c,A,d,0.040667,-B,k,f,-D,g,h,-C,0.461894,ia,-A,j,0.119651,-B,l,m,-D,n,0.288864,-C,e,o,-A,p,q,-B,y,x,-E,w,v,-C,u,t,-A,r,s,-B,G,Q,-E,0.005406,P,-C,O,N,-A,R,M,-B,-L,0.327934,-E,-K,J,-C,-0.198499,I,-A,-H,F,-B,-S,T,-D,-U,V,-C,-W,X,-A,-Y,Z,-B,-a,0.085684,-D,-0.351113,0.111952,-C,
                        -b,c,-A,-d};

                    const unsigned short inds[] = {
                        1,7,2,0,4,5,2,8,3,1,5,6,5,4,9,7,12,8,5,10,6,6,11,7,11,16,12,10,13,14,10,15,11,9,4,13,14,19,15,13,4,17,15,20,16,13,18,14,18,23,19,17,4,21,19,24,20,17,22,18,24,27,28,
                        22,25,26,22,27,23,21,4,25,28,31,32,25,30,26,26,31,27,25,4,29,31,34,35,29,4,33,32,35,36,30,33,34,34,39,35,33,4,37,36,39,40,33,38,34,39,44,40,38,41,42,38,43,39,37,4,41,42,45,46,42,47,43,
                        41,4,45,43,48,44,46,51,47,45,4,49,47,52,48,45,50,46,49,4,53,51,56,52,50,53,54,50,55,51,56,2,3,53,1,54,54,2,55,53,4,0,1,6,7,2,7,8,1,0,5,7,11,12,5,9,10,6,10,11,11,15,16,
                        10,9,13,10,14,15,14,18,19,15,19,20,13,17,18,18,22,23,19,23,24,17,21,22,24,23,27,22,21,25,22,26,27,28,27,31,25,29,30,26,30,31,31,30,34,32,31,35,30,29,33,34,38,39,36,35,39,33,37,38,39,43,44,
                        38,37,41,38,42,43,42,41,45,42,46,47,43,47,48,46,50,51,47,51,52,45,49,50,51,55,56,50,49,53,50,54,55,56,55,2,53,0,1,54,1,2};

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;            
            case TEAPOT_MESH_CAPSULE: {
#               if (!defined(TEAPOT_NO_MESH_CYLINDER) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_UP) && !defined(TEAPOT_NO_MESH_HALF_SPHERE_DOWN))
                {
                    TIS.startInds[i] = 0;
                    TIS.numInds[i] = 0;
                    for (j=0;j<3;j++) {
                        TIS.halfExtents[i][j] = TIS.halfExtents[TEAPOT_MESH_CYLINDER][j];
                        TIS.centerPoint[i][j] = TIS.centerPoint[TEAPOT_MESH_CYLINDER][j];
                    }
                    if (TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_UP][1]==0)      TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_UP][1] = 0.25;
                    if (TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_DOWN][1]==0)    TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_DOWN][1] = 0.25;
                    TIS.halfExtents[i][1] += TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_UP][1]+TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_DOWN][1];
                    {
                        for (j=0;j<3;j++) {
                            TIS.aabbMin[i][j] = TIS.centerPoint[i][j] - TIS.halfExtents[i][j];
                            TIS.aabbMax[i][j] = TIS.centerPoint[i][j] + TIS.halfExtents[i][j];
                        }
                        //printf("CAPSULE: aabbMin(%1.3f,%1.3f,%1.3f) aabbMax(%1.3f,%1.3f,%1.3f) [%1.3f,%1.3f]\n",TIS.aabbMin[i][0],TIS.aabbMin[i][1],TIS.aabbMin[i][2],TIS.aabbMax[i][0],TIS.aabbMax[i][1],TIS.aabbMax[i][2],TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_UP][1],TIS.halfExtents[TEAPOT_MESH_HALF_SPHERE_DOWN][1]);
                    }
#                   ifdef TEAPOT_CENTER_MESHES_ON_FLOOR
                    TIS.centerPoint[i][1]-=TIS.halfExtents[i][1];
#                   endif //TEAPOT_CENTER_MESHES_ON_FLOOR
                }
#               endif
            }
                break;
            case TEAPOT_MESH_PIVOT3D: {
#               ifndef TEAPOT_NO_MESH_PIVOT3D
                {
                    const float A=0.0,B=0.024000,C=0.041569,D=0.350739,E=0.1,F=0.092881,G=0.053625,H=0.048000,I=0.350000,J=0.107250,K=0.500739;
                    const float verts[] = {
                        E,-A,-H,E,C,-B,E,C,B,E,-A,H,E,-C,B,E,-C,-B,H,-A,E,B,C,E,-B,C,E,-H,-A,E,-B,-C,E,B,-C,E,H,E,-A,B,E,-C,-B,E,-C,-H,E,A,-B,E,C,B,E,C,-B,-A,-C,-H,A,A,-B,-C,B,-0.024500,-C,-B,0.025000,-C,-B,D,-A,-A,D,-A,J,D,F,G,D,F,-G,D,-A,-J,D,-F,-G,D,-F,G,K,-A,-A,D,-A,J,D,F,G,D,F,-G,D,-A,-J,D,-F,
                        -G,D,-F,G,I,-A,-H,E,-A,-H,I,C,-B,E,C,-B,I,C,B,E,C,B,I,-A,H,E,-A,H,I,-C,B,E,-C,B,I,-C,-B,E,-C,-B,-A,-A,D,-J,-A,D,-G,F,D,G,F,D,J,-A,D,G,-F,D,-G,-F,D,-A,-A,K,-J,-A,D,-G,F,D,G,F,D,J,-A,D,G,-F,D,-G,-F,D,H,-A,I,H,-A,E,B,C,I,B,C,E,-B,C,I,-B,C,E,-H,-A,I,-H,-A,E,-B,-C,I,-B,-C,E,B,-C,I,B,-C,E,-A,D,A,-J,D,A,-G,
                        D,-F,G,D,-F,J,D,A,G,D,F,-G,D,F,-A,K,A,-J,D,A,-G,D,-F,G,D,-F,J,D,A,G,D,F,-G,D,F,H,I,A,H,E,-A,B,I,-C,B,E,-C,-B,I,-C,-B,E,-C,-H,I,A,-H,E,A,-B,I,C,-B,E,C,B,I,C,B,E,C};

                    const unsigned short inds[] = {
                        12,17,2,2,1,12,12,1,13,17,8,7,3,2,17,17,6,3,6,4,3,8,15,9,1,0,13,7,6,17,13,18,14,14,19,15,15,19,9,5,4,22,11,10,20,18,21,19,21,22,20,21,18,22,22,0,5,21,20,19,9,20,10,
                        11,22,4,17,16,8,6,11,4,8,16,15,13,0,18,14,18,19,22,18,0,9,19,20,11,20,22,23,24,25,23,25,26,23,26,27,23,27,28,23,28,29,23,29,24,31,30,32,32,30,33,33,30,34,34,30,35,35,30,36,36,30,31,
                        38,39,37,40,41,39,41,44,43,44,45,43,45,48,47,48,37,47,38,40,39,40,42,41,41,42,44,44,46,45,45,46,48,48,38,37,75,76,77,75,77,78,75,78,79,75,79,80,75,80,81,75,81,76,83,82,84,84,82,85,85,82,86,
                        86,82,87,87,82,88,88,82,83,90,91,89,92,93,91,93,96,95,96,97,95,97,100,99,100,89,99,90,92,91,92,94,93,93,94,96,96,98,97,97,98,100,100,90,89,49,50,51,49,51,52,49,52,53,49,53,54,49,54,55,49,55,50,
                        57,56,58,58,56,59,59,56,60,60,56,61,61,56,62,62,56,57,64,65,63,66,67,65,67,70,69,70,71,69,71,74,73,74,63,73,64,66,65,66,68,67,67,68,70,70,72,71,71,72,74,74,64,63};

                    // [ 0-30] 30 tri-faces: Center   (0.875,0.875,0.000)
                    // [30-54] 24 tri-faces: ArrowX   (0.875,0.000,0.000)
                    // [54-78] 24 tri-faces: ArrowY   (0.000,0.875,0.000)
                    // [78-102]24 tri-faces: Arrow>   (0.000,0.000,0.875)

                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_TEXT_X: {
#               ifndef TEAPOT_NO_MESH_TEXT_X
                {
                    const float A=0.0,B=0.498480,C=0.503688,D=0.005208,E=0.298902,F=0.548886,G=0.002046;
                    const float verts[] = {
                        -0.127038,D,A,-E,B,A,-F,B,A,-G,0.154752,A,0.538098,B,A,0.283650,B,A,0.125178,D,A,-F,-C,-A,-G,-0.144336,-A,0.567114,-C,-A,-E,-C,-A,0.312666,-C,-0.000000};

                    const unsigned short inds[] = {
                        0,1,2,0,3,1,3,4,5,3,6,4,0,6,3,7,6,0,7,8,6,8,9,6,7,10,8,11,9,8};

                    // Probably wrong. We Must do a AddMeshVertsAndIndsLines()
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_TEXT_Y: {
#               ifndef TEAPOT_NO_MESH_TEXT_Y
                {
                    const float A=0.0,B=0.504562,C=0.497344;
                    const float verts[] = {-0.063080,-0.016311,-A,-0.170796,B,A,-0.319828,B,A,0.007747,0.131245,-A,0.326468,B,A,0.178912,B,A,-0.142760,-C,-A,-0.288841,-C,-A};

                    const unsigned short inds[] = {
                        0,1,2,0,3,1,3,4,5,3,6,4,0,6,3,7,6,0};

                    // Probably wrong. We Must do a AddMeshVertsAndIndsLines()
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            case TEAPOT_MESH_TEXT_Z: {
#               ifndef TEAPOT_NO_MESH_TEXT_Z
                {
                    const float A=0.0,B=0.322419,C=0.500899,D=0.322340,E=0.500820,F=0.434766;
                    const float verts[] = {-0.421938,B,A,0.432535,C,A,-0.421938,C,A,0.086730,B,A,-0.100674,-D,A,-0.444248,-E,A,F,-D,A,F,-E,0.000000};

                    const unsigned short inds[] = {
                        0,1,2,0,3,1,3,4,1,5,4,3,5,6,4,5,7,6};

                    // Probably wrong. We Must do a AddMeshVertsAndIndsLines()
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            }
        }

        if (gTeapotInitUserMeshCallback)    {
            const int lastUserMeshIndex = TEAPOT_MESH_USER_00+(TEAPOT_MESH_CUBE-TEAPOT_MESH_USER_00);
            for (i=TEAPOT_MESH_USER_00;i<lastUserMeshIndex;i++) {
                const float* pVerts=NULL;const unsigned short* pInds=NULL;
                int numVerts=0,numInds=0;
                gTeapotInitUserMeshCallback((TeapotMeshEnum) i,&pVerts,&numVerts,&pInds,&numInds);
                if (pVerts && pInds && numVerts>3 && numInds>3) {
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS+TEAPOT_MAX_NUM_USER_MESH_VERTICES,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS+TEAPOT_MAX_NUM_USER_MESH_INDICES,&numTotInds,pVerts,numVerts,pInds,numInds,(TeapotMeshEnum)i);
                }
            }
        }

        //printf("Teapot_init(): numTotVerts = %d numTotInds = %d",numTotVerts,numTotInds);

        CalculateVertexNormals(totVerts,numTotVerts,6,totInds,numTotInds,norms,6);

        for (i=TEAPOT_FIRST_MESHLINES_INDEX;i<TEAPOT_MESH_COUNT;i++)    {
            switch (i)  {
            case TEAPOT_MESHLINES_CUBE_EDGES: {
#               ifndef TEAPOT_NO_MESHLINES_CUBE_EDGES
                {
                    const float D = 0.5f;
                    const float verts[] = {
                        -D,-D,-D,   // 0 RDB
                        -D,-D, D,   // 1 RDF
                        -D, D,-D,   // 2 RTB
                        -D, D, D,   // 3 RTF
                         D,-D,-D,   // 4 LDB,
                         D,-D, D,   // 5 LDF
                         D, D,-D,   // 6 LTB
                         D, D, D    // 7 LTF
                         };

                    const unsigned short inds[] = {
                        0,1, 0,2, 1,3, 2,3,  // R
                        4,5, 4,6, 5,7, 6,7, // L
                        0,4, 1,5, 2,6, 3,7};

                    // Probably wrong. We Must do a AddMeshVertsAndIndsLines()
                    AddMeshVertsAndInds(totVerts,TEAPOT_MAX_NUM_MESH_VERTS+TEAPOT_MAX_NUM_USER_MESH_VERTICES,&numTotVerts,6,totInds,TEAPOT_MAX_NUM_MESH_INDS+TEAPOT_MAX_NUM_USER_MESH_INDICES,&numTotInds,verts,sizeof(verts)/(sizeof(verts[0])*3),inds,sizeof(inds)/sizeof(inds[0]),(TeapotMeshEnum)i);
                }
#               endif
            }
                break;
            }
        }

        glGenBuffers(1, &TIS.vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, TIS.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*numTotVerts, totVerts, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &TIS.elementBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TIS.elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short)*numTotInds, totInds, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

}

#ifdef __cplusplus
}
#endif

#endif //TEAPOT_IMPLEMENTATION_H
#endif //TEAPOT_IMPLEMENTATION
