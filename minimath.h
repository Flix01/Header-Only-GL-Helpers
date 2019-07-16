//                __ ____    __ ___ ______
//               |  '    \  |  '   `      \_
//              _|   __   |_|   __    __   |      _   _
//    _ __ ___ (_)  |  |  (_)  |  |  |  |  | __ _| |_| |__
//   | '_ ` _ \| |  |  |  | |  |  |  |  |  |/ _` | __| '_ \_
//   | | | | | | |  |  |  | |  |  |  |  |  | (_| | |_| | | |
//   |_| |_| |_|_|__|  |__|_|__|  |__|  |__|\__,_|\__|_| |_|
//

// https://github.com/Flix01/Header-Only-GL-Helpers
/**  MIT License
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

Some functions are ported from
-> the OgreMath library (www.ogre3d.org) - MIT licensed.
-> the repository https://github.com/juj/MathGeoLib - Apache 2 license
-> the glm library (https://github.com/g-truc/glm) - licensed under the Happy Bunny License (Modified MIT) or the MIT License.
-> Eric's Blog: https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html

================
GETTING STARTED:
================
-> minimath.h is a stb-like header-only column-major mini math library.
   That means that MINIMATH_IMPLEMENTATION must be defined in a single c or c++ file before including minimath.h.

-> The main type used is "nmoat" (miNiMath flOAT): if NM_DOUBLE_PRECISION is defined it's a double, otherwise it's a float.

-> There are 3 levels of API that can be used:
  <0> [C/C++] Functions starting with "nmh_" (e.g. nmh_sin(...)).
      These functions are simply low-level math functions.
      (example: nmh_sin(...) defaults to sinf(...), or to sin(...) when NM_DOUBLE_PRECISION is defined).
  <1> [C/C++] Functions starting with "nm_" (e.g. nm_Mat4Mul(nmoat* result16,const nmoat* ml16,const nmoat* mr16)):
      Although this API is not type-safe (because we can do: nmoat v2[2];nm_Mat4Mul(v2,...) without any compiler error),
      it's the preferred API to use: it's non-intrusive because it works on plain C arrays.
      You can even disable everything else by defining NM_NO_MATH_STRUCTS.
  <2> [C/C++] Structs vec2,vec3,vec4,quat,mat3,mat4 and functions starting with vec2_,vec3_,vec4_,quat_,mat3_,mat4_, plus
      functions like make_vec2(...),[...],make_mat4(...):
      This API is type-safe (example: mat4 dst,a,b;[fill a and b here];mat4_mul(&dst,&a,&b);).
  <3> [C++ only] Structs vec2,vec3,vec4,quat,mat3,mat4 and their methods.
      Example: mat4 a,b;mat4 c = a*b;
  <4> [C++ only (experimental)] A PARTIAL glm library (https://github.com/g-truc/glm) compatibility layer, that can be
      activated using the definition NM_BEST_GLM_COMPATIBILITY. Example file "glm_test.cpp":
      #define NM_BEST_GLM_COMPATIBILITY
      #define MINIMATH_IMPLEMENTATION
      #include <minimath.h>
      glm::mat4 camera(float Translate, glm::vec2 const & Rotate)   {
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
        glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -Translate));
        View = glm::rotate(View, Rotate.y, glm::vec3(-1.0f, 0.0f, 0.0f));
        View = glm::rotate(View, Rotate.x, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
        return Projection * View * Model;
      }
      int main(int argc,char* argv[])	{
        glm::mat4 result = camera(25.f,glm::vec2(60.f,30.f));
        return 1;
      }

=======================
SOME USEFUL DEFINITIONS
=======================
NM_DOUBLE_PRECISION                  minimath.h works with doubles instead of floats.
NM_NO_MATH_STRUCTS                   only API <0> and <1> are available (good choice!).
NM_BEST_GLM_COMPATIBILITY            [C++ only] enables API <4>.
NM_GLM_MATRIX_SUBSCRIPT_OPERATOR     [C++ only: already defined by NM_BEST_GLM_COMPATIBILITY] It affects the C++ [] operator of mat3 and mat4, making it work like in glm (not the best choice IMO: better use the col[col][row] field explicitly).
NM_HAS_CFG                           if too many NM_ defs pollute your project settings, just define NM_HAS_CFG, create "minimath_cfg.h" and put all the other NM_ defs there.
NM_NAMESPACE                         [C++ only] [needs a name] wraps API <2> and <3> with namespace NM_NAMESPACE (and if <4> is enabled, it overwrites the "glm" namespace).
NM_NO_CPP_CTRS                       [C++ only] with this, to init structs you can't use C++ ctrs anymore, but now you can use C-style: vec2 v2={{{1,2}}}; [vec2 v2={1,2}; works with a warning] Tip: vec2 v2=make_vec2(1,2); WORKS in ANY case.
NM_NM_NO_C_INITIALIZATION            [C only or C++ with NM_NO_CPP_CTRS] some old compiler does not even support C-style initialization (because of anonymous sub-structs). With this def we can still use something like: vec2 v2=make_vec2(1,2);
NM_USE_SIMD                          [Experimental] together with the correct compilation flags, this def enables SIMD acceleration for a VERY FEW functions (and I've not even sure that this actually speeds up code in optimized release mode).
NM_ALIGN_STRUCTS                     [Experimental] it aligns all struct (intended to speed up NM_USE_SIMD a bit, but it's not required by NM_USE_SIMD).
NM_COMPILER_SUPPORTS_NONPOD_ANONYMOUS_MEMBERS [C++ only][Experimental] mat3 and mat4 structs can have, in some cases, handy anonymous fields: vec3 xAxis,yAxis,zAxis(,and tra for mat4). This is only possible if vec3 is a POD-type (always true in C, or with NM_NO_CPP_CTRS in C++). This def tries to see if your compiler can keep vec3 non-POD and still add the 'axis' fields.
NM_USE_MEMCPY                        [impl includes <string.h>] It uses memcpy instead of a for loop to copy arrays.
NM_NO_STDIO                          [excludes <stdio.h> from header] It disables all the "print" functions.
NM_NO_ASSERT                         Disables NM_ASSERTs
NM_ASSERT_ON_BAD_QUATERNION_CTR      the quat struct C++ constructor, and the make_quat(...) function (but NOT the C-style init: quat q={{{x,y,z,w}}};), take w as first argument (like glm). To avoid common errors this def can be defined.

========
IN CODE:
========
NM_ASSERT(some bool cond);           Default assert macro
NM_STRUCT_SIZE_CHECK;                It asserts if the structs have strange sizes

=====
TIPS:
=====
-> To get a raw nmoat pointer of a struct, you can just use the "e" field for vec/quat and the "m" field for mat3/4 (ex: vec3 v;nmoat* vp=v.e; mat4 m;nmoat* mp=m.m;).

=====
TODO:
=====
-> Find and fix all errors.
-> Use a unique and consistent naming convention for all functions.
-> Find and add missing functions, and make all accessible from the 3 APIs.
-> Avoid implementing <3> or <4> using <2>, but whenever possible always use <1> in implementations.
-> Document functions better.
-> Provide examples, or tests. Check correctness of everything.

*/
#ifndef MINIMATH_H_
#define MINIMATH_H_

#define NM_VERSION               "1.0 WIP"
#define NM_VERSION_NUM           0002

#ifdef NM_HAS_CFG
#   include "minimath_cfg.h"
#endif

#ifndef NM_API_DEF
#define NM_API_DEF /* no-op */
#endif

#ifndef NM_RESTRICT
#define NM_RESTRICT __restrict
// __restrict (C99), otherwise try: restrict (C11) or: /* no-op */
#endif

#ifndef NM_API_INL
#define NM_API_INL __inline
// C99, otherwise try: __inline or /* no-op */
/* Hint: To be conformant with the C standard, every pure (= not 'static' or 'extern') inline definition
in the .h file, must be matched with a corresponding 'extern' declaration in the implementation file.
Otherwise under some compiler settings we can have 'undefined reference' errors */
#endif

#ifndef NM_API_STATIC
#define NM_API_STATIC static
#endif

#ifndef NM_API_EXT
#define NM_API_EXT extern
#endif

#ifndef NM_API_EXT_INL
#define NM_API_EXT_INL NM_API_EXT NM_API_INL
/* On GCC: The combination of inline and extern has almost the effect of a macro.
 * The way to use it is to put a function definition in a header file with these keywords,
 * and put another copy of the definition (lacking inline and extern) in a library file.
 * The definition in the header file causes most calls to the function to be inlined.
 * If any uses of the function remain, they refer to the single copy in the library. */
#endif

#ifndef NM_API_DEF_EXT
#define NM_API_DEF_EXT NM_API_DEF NM_API_EXT
#endif

#ifndef NM_API_DEF_EXT_INL
#   ifndef __cplusplus
#       define NM_API_DEF_EXT_INL NM_API_DEF NM_API_EXT_INL
#   else
#       define NM_API_DEF_EXT_INL NM_API_DEF
#   endif
#endif

#ifndef NM_API_DEF_INL
#define NM_API_DEF_INL NM_API_DEF NM_API_INL
#endif

#ifndef NM_API_EXPLICIT
#define NM_API_EXPLICIT explicit
#endif

#ifndef NM_API_INL_STATIC
#define NM_API_INL_STATIC NM_API_INL NM_API_STATIC
#endif


#if (defined(NM_USE_DOUBLE_PRECISION) || defined(NM_DOUBLE_PRECISION))
#   undef NM_DOUBLE_PRECISION
#   define NM_DOUBLE_PRECISION
#   undef NM_USE_DOUBLE_PRECISION
#   define NM_USE_DOUBLE_PRECISION
#endif

#ifdef NM_DOUBLE_PRECISION
typedef double nmoat;
#else
typedef float nmoat;
#endif

#ifndef NM_SLERP_EPSILON
#   define NM_SLERP_EPSILON ((nmoat)0.0001)
#endif //NM_SLERP_EPSILON
#ifndef NM_EPSILON
#   define NM_EPSILON ((nmoat)0.0000001)
#endif

#ifdef NM_ALIGN_STRUCTS
#   ifdef NM_DOUBLE_PRECISION
#       define NM_STRUCT_ALIGNMENT_AMOUNT   (32)
#   else
#       define NM_STRUCT_ALIGNMENT_AMOUNT   (16)
#   endif
#   ifdef _MSC_VER
#       define NM_PRE_ALIGN(X)     __declspec(align(X))
#       define NM_POST_ALIGN(X)    /* no-op */
#   else //_MSC_VER
#       define NM_PRE_ALIGN(X)     /* no-op */
#       define NM_POST_ALIGN(X)     __attribute__((aligned(X)))
#   endif //_MSC_VER
#   define NM_PRE_ALIGN_DEFAULT     NM_PRE_ALIGN(NM_STRUCT_ALIGNMENT_AMOUNT)
#   define NM_POST_ALIGN_DEFAULT    NM_POST_ALIGN(NM_STRUCT_ALIGNMENT_AMOUNT)
#else //NM_ALIGN_STRUCTS
#   define NM_PRE_ALIGN(X)     /* no-op */
#   define NM_POST_ALIGN(X)     /* no-op */
#   define NM_PRE_ALIGN_DEFAULT     /* no-op */
#   define NM_POST_ALIGN_DEFAULT    /* no-op */
#endif //NM_ALIGN_STRUCTS

//--------------------------------------------------

#include <math.h>
#ifndef M_PI
#define M_PI ((nmoat)3.14159265358979323846)
#endif
#ifndef M_HALF_PI
#define M_HALF_PI ((nmoat)(3.14159265358979323846/2.0))
#endif
#ifndef M_PIOVER180
#define M_PIOVER180 ((nmoat)(3.14159265358979323846/180.0))
#endif
#ifndef M_180OVERPI
#define M_180OVERPI ((nmoat)(180.0/3.14159265358979323846))
#endif
#ifndef M_DEG2RAD
#define M_DEG2RAD(X)   (X*(nmoat)M_PIOVER180)
#endif
#ifndef M_RAD2DEG
#define M_RAD2DEG(X)   (X*(nmoat)M_180OVERPI)
#endif

#ifndef NM_NO_STDIO
#include <stdio.h>  // printf(...), FILE*
#endif

//#include <stddef.h> /* size_t */

#ifdef NM_BEST_GLM_COMPATIBILITY
#   ifndef NM_NAMESPACE
#       define NM_NAMESPACE glm
#   endif
#   undef NM_GLM_MATRIX_SUBSCRIPT_OPERATOR
#   define NM_GLM_MATRIX_SUBSCRIPT_OPERATOR
#endif //NM_BEST_GLM_COMPATIBILITY


#ifndef __cplusplus
#   undef NM_NO_CPP_CTRS
#   define NM_NO_CPP_CTRS
#   ifdef NM_NAMESPACE
#       undef NM_NAMESPACE
#       warning NM_NAMESPACE will be ignored and undefined (plain C compilation).
#   endif //NM_NAMESPACE
#else //__cplusplus
#endif //__cplusplus

#ifndef NM_ASSERT
#   if (!defined(NDEBUG) && !defined(_NDEBUG) && !defined(NM_NO_ASSERT))
#       ifndef __cplusplus
#           include <assert.h>
#       else //__cplusplus
#           include <cassert>
#       endif //__cplusplus
#   define NM_ASSERT(X) assert(X)
#   else
#       define NM_ASSERT(X) /* no-op */
#   endif
#endif //NM_ASSERT

#ifdef NM_WARNING_SUPPRESSION
#   if (defined(__GNUC__) || defined(__MINGW__) || defined(__clang__))
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wmissing-braces"
#   elif defined(_MSC_VER)
#       pragma warning( push )
//#     pragma warning (disable: 4127)     // condition expression is constant
//#     pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#       pragme warning (disable: 4190)     // "-Wreturn-type-c-linkage"
#   endif
#endif // NM_WARNING_SUPPRESSION

#ifdef __cplusplus
extern "C"	{
#endif //__cplusplus

NM_API_DEF_EXT_INL nmoat nmh_radians(nmoat deg);
NM_API_DEF_EXT_INL nmoat nmh_degrees(nmoat rad);
NM_API_DEF_EXT_INL nmoat nmh_sin(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_cos(nmoat v);
NM_API_DEF_EXT_INL void nmh_sincos(nmoat v,nmoat* NM_RESTRICT s,nmoat* NM_RESTRICT c);
NM_API_DEF_EXT_INL nmoat nmh_tan(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_asin(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_acos(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_atan(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_atan2(nmoat y,nmoat x);
NM_API_DEF_EXT_INL nmoat nmh_sqrt(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_fabs(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_fmod(nmoat a,nmoat b);
NM_API_DEF_EXT_INL nmoat nmh_pow(nmoat x,nmoat y);
NM_API_DEF_EXT_INL nmoat nmh_round(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_ceil(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_floor(nmoat v);
NM_API_DEF_EXT_INL nmoat nmh_fractional_part(nmoat v);


NM_API_DEF_EXT_INL int nm_AreEqualEps(const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,int num_nmoats,nmoat eps);
NM_API_DEF_EXT_INL int nm_AreEqual(const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,int num_nmoats);


NM_API_DEF_EXT_INL nmoat* nm_Vec2Copy(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT src2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2Add(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2Sub(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2Mul(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2Div(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2MulSca(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec2DivSca(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec2Neg(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2SAdd(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2SSub(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2SMul(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2SDiv(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2SMulSca(nmoat* NM_RESTRICT dst2,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec2SDivSca(nmoat* NM_RESTRICT dst2,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec2SNeg(nmoat* NM_RESTRICT dst2);
NM_API_DEF_EXT_INL nmoat nm_Vec2Length2(const nmoat* NM_RESTRICT a2);
NM_API_DEF_EXT_INL nmoat nm_Vec2Length(const nmoat* NM_RESTRICT a2);
NM_API_DEF_EXT_INL void nm_Vec2Normalize(nmoat* NM_RESTRICT v2);
NM_API_DEF_EXT_INL nmoat* nm_Vec2Normalized(nmoat* NM_RESTRICT vOut2,const nmoat* NM_RESTRICT v2);
NM_API_DEF_EXT_INL nmoat nm_Vec2DistSquared(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2);
NM_API_DEF_EXT_INL nmoat nm_Vec2Dist(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2);

NM_API_DEF_EXT_INL nmoat* nm_Vec3Copy(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT src3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3Add(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3Sub(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3Mul(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3Div(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3MulSca(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec3DivSca(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec3Neg(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3SAdd(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3SSub(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3SMul(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3SDiv(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3SMulSca(nmoat* NM_RESTRICT dst3,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec3SDivSca(nmoat* NM_RESTRICT dst3,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec3SNeg(nmoat* NM_RESTRICT dst3);
NM_API_DEF_EXT_INL nmoat nm_Vec3Dot(const nmoat* a3,const nmoat* b3);
NM_API_DEF_EXT_INL nmoat nm_Vec3Length2(const nmoat* NM_RESTRICT a3);
NM_API_DEF_EXT_INL nmoat nm_Vec3Length(const nmoat* NM_RESTRICT a3);
NM_API_DEF_EXT_INL void nm_Vec3Normalize(nmoat* NM_RESTRICT v3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3Normalized(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT v3);
NM_API_DEF_EXT_INL nmoat* nm_Vec3Cross(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);
NM_API_DEF_EXT_INL nmoat nm_Vec3DistSquared(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);
NM_API_DEF_EXT_INL nmoat nm_Vec3Dist(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);

NM_API_DEF_EXT_INL nmoat* nm_Vec4Copy(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT src4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4Add(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4Sub(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4Mul(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4Div(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4MulSca(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec4DivSca(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec4Neg(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4SAdd(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4SSub(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4SMul(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4SDiv(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4SMulSca(nmoat* NM_RESTRICT dst4,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec4SDivSca(nmoat* NM_RESTRICT dst4,nmoat s);
NM_API_DEF_EXT_INL nmoat* nm_Vec4SNeg(nmoat* NM_RESTRICT dst4);
NM_API_DEF_EXT_INL nmoat nm_Vec4Length2(const nmoat* NM_RESTRICT a4);
NM_API_DEF_EXT_INL nmoat nm_Vec4Length(const nmoat* NM_RESTRICT a4);
NM_API_DEF_EXT_INL void nm_Vec4Normalize(nmoat* NM_RESTRICT v4);
NM_API_DEF_EXT_INL nmoat* nm_Vec4Normalized(nmoat* NM_RESTRICT vOut4,const nmoat* NM_RESTRICT v4);
NM_API_DEF_EXT_INL nmoat nm_Vec4DistSquared(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);
NM_API_DEF_EXT_INL nmoat nm_Vec4Dist(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);

NM_API_DEF_EXT_INL nmoat* nm_QuatCopy(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT src4);
NM_API_DEF_EXT_INL nmoat nm_QuatLength2(const nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat nm_QuatLength(const nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat* nm_QuatLerp(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat t,int normalizeQOutAfterLerp/*=1*/);
NM_API_DEF_EXT_INL nmoat* nm_QuatSlerpEps(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/,nmoat eps/*=NM_SLERP_EPSILON*/);
NM_API_DEF_EXT_INL nmoat* nm_QuatSlerp(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/);
NM_API_DEF_EXT_INL void nm_QuatNormalize(nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat* nm_QuatNormalized(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat nm_QuatDot(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);
NM_API_DEF_EXT_INL nmoat* nm_QuatNegated(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT src4);
NM_API_DEF_EXT_INL void nm_QuatNegate(nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat* nm_QuatMul(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);
NM_API_DEF_EXT_INL nmoat* nm_QuatMul3Quats(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,const nmoat* NM_RESTRICT c4);
NM_API_DEF_EXT_INL nmoat* nm_QuatConjugated(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL void nm_QuatConjugate(nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat* nm_QuatInverted(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL void nm_QuatInverse(nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat* nm_QuatMulVec3(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT q4,const nmoat* NM_RESTRICT vIn3);
NM_API_DEF_EXT_INL nmoat* nm_QuatMulVec3Inv(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT q4,const nmoat* NM_RESTRICT vIn3);
NM_API_DEF_EXT_INL nmoat* nm_QuatFromAngleAxis(nmoat* NM_RESTRICT qOut4,nmoat rfAngle,nmoat rkAxisX,nmoat rkAxisY,nmoat rkAxisZ);
NM_API_DEF_EXT_INL nmoat* nm_QuatFromAngleAxisVec3(nmoat* NM_RESTRICT qOut4,nmoat rfAngle,const nmoat* NM_RESTRICT rkAxis3);
NM_API_DEF_EXT_INL void nm_QuatToAngleAxis(const nmoat* NM_RESTRICT q4,nmoat* NM_RESTRICT rfAngleOut1,nmoat* NM_RESTRICT rkAxisOut3);
NM_API_DEF_EXT_INL nmoat* nm_QuatFromLookAtYX(nmoat* NM_RESTRICT qOut4,nmoat sourcePosX,nmoat sourcePosY,nmoat sourcePosZ,nmoat targetPosX,nmoat targetPosY,nmoat targetPosZ);
NM_API_DEF_EXT_INL nmoat* nm_QuatFromEuler(nmoat* NM_RESTRICT qOut4,nmoat fAngleX,nmoat fAngleY,nmoat fAngleZ,int eulerRotationMode);



NM_API_DEF_EXT_INL nmoat* nm_Mat3Copy(nmoat* NM_RESTRICT dst9,const nmoat* NM_RESTRICT src9);
NM_API_DEF_EXT_INL nmoat* nm_Mat3Identity(nmoat* NM_RESTRICT result9);
NM_API_DEF_EXT_INL nmoat* nm_Mat3Mul(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT ml9,const nmoat* NM_RESTRICT mr9);
NM_API_DEF_EXT_INL nmoat* nm_Mat3Mul_NoCheck(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT ml9,const nmoat* NM_RESTRICT mr9);

NM_API_DEF_EXT_INL nmoat* nm_Mat3Mul3(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9,const nmoat* NM_RESTRICT c9);
NM_API_DEF_EXT_INL nmoat* nm_Mat3Mul3_NoCheck(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9,const nmoat* NM_RESTRICT c9);

NM_API_DEF_EXT_INL nmoat* nm_QuatFromMat3(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT m9);
NM_API_DEF_EXT_INL nmoat* nm_Mat3FromQuat(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT q4);

NM_API_DEF_EXT_INL void nm_Mat3TransposeInPlace(nmoat* NM_RESTRICT m9);
NM_API_DEF_EXT_INL nmoat* nm_Mat3Transpose(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT m9);

NM_API_DEF_EXT_INL int nm_Mat3Invert(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT m9);
NM_API_DEF_EXT_INL int nm_Mat3InvertInPlace(nmoat* NM_RESTRICT m9);

NM_API_DEF_EXT_INL const nmoat* nm_Mat3SlerpEps(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT mStart9,const nmoat* NM_RESTRICT mEnd9,nmoat slerpTime_In_0_1,nmoat eps);
NM_API_DEF_EXT_INL const nmoat* nm_Mat3Slerp(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT mStart9,const nmoat* NM_RESTRICT mEnd9,nmoat slerpTime_In_0_1);

NM_API_DEF_EXT_INL nmoat* nm_Mat3FromEulerXYZ(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat3FromEulerXZY(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat3FromEulerYXZ(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat3FromEulerYZX(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat3FromEulerZXY(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat3FromEulerZYX(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z);

NM_API_DEF_EXT_INL int nm_Mat3ToEulerXYZ(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat3ToEulerXZY(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat3ToEulerYXZ(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat3ToEulerYZX(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat3ToEulerZXY(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat3ToEulerZYX(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z);

NM_API_DEF_EXT_INL void nm_Mat3FromEuler(nmoat* m9,nmoat x,nmoat y,nmoat z,int eulerRotationMode);
NM_API_DEF_EXT_INL int nm_Mat3ToEuler(const nmoat* m9,nmoat* x,nmoat* y,nmoat* z,int eulerRotationMode);


NM_API_DEF_EXT_INL nmoat* nm_Mat4Copy(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT src16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4Identity(nmoat* NM_RESTRICT result16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4Mul(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT ml16,const nmoat* NM_RESTRICT mr16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4Mul_NoCheck(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT ml16,const nmoat* NM_RESTRICT mr16);

NM_API_DEF_EXT_INL void nm_Mat4LookAt(nmoat* NM_RESTRICT mOut16,nmoat eyeX,nmoat eyeY,nmoat eyeZ,nmoat centerX,nmoat centerY,nmoat centerZ,nmoat upX,nmoat upY,nmoat upZ);
NM_API_DEF_EXT_INL void nm_Mat4Perspective(nmoat* NM_RESTRICT mOut16,nmoat degfovy,nmoat aspect, nmoat zNear, nmoat zFar);
NM_API_DEF_EXT_INL void nm_Mat4Ortho(nmoat* NM_RESTRICT mOut16,nmoat left,nmoat right, nmoat bottom, nmoat top,nmoat nearVal,nmoat farVal);
NM_API_DEF_EXT_INL void nm_Mat4Ortho3D(nmoat* NM_RESTRICT mOut16,nmoat cameraTargetDistance,nmoat degfovy,nmoat aspect,nmoat znear,nmoat zfar);

NM_API_DEF_EXT_INL int nm_Mat4Project(nmoat objX,nmoat objY,nmoat objZ,const nmoat* NM_RESTRICT mvMatrix16,const nmoat* NM_RESTRICT pMatrix16,const int* viewport4,nmoat* NM_RESTRICT winX,nmoat* NM_RESTRICT winY,nmoat* NM_RESTRICT winZ);
NM_API_DEF_EXT_INL int nm_Mat4UnProjectMvpInv(nmoat winX,nmoat winY,nmoat winZ,const nmoat* NM_RESTRICT mvpMatrixInv16,const int* viewport4,nmoat* NM_RESTRICT objX,nmoat* NM_RESTRICT objY,nmoat* NM_RESTRICT objZ);
NM_API_DEF_EXT_INL int nm_Mat4UnProject(nmoat winX,nmoat winY,nmoat winZ,const nmoat* NM_RESTRICT mvMatrix16, const nmoat* NM_RESTRICT pMatrix16, const int* viewport4,nmoat* NM_RESTRICT objX,nmoat* NM_RESTRICT objY,nmoat* NM_RESTRICT objZ);
NM_API_DEF_EXT_INL int nm_Mat4UnProject4MvpInv(nmoat winX,nmoat winY,nmoat winZ,nmoat clipW,const nmoat* NM_RESTRICT mvpMatrixInv16,const int* viewport4,nmoat nearVal,nmoat farVal,nmoat* NM_RESTRICT objX,nmoat* NM_RESTRICT objY,nmoat* NM_RESTRICT objZ,nmoat* NM_RESTRICT objW);
NM_API_DEF_EXT_INL int nm_Mat4UnProject4(nmoat winX, nmoat winY, nmoat winZ, nmoat clipW, const nmoat* NM_RESTRICT mvMatrix16, const nmoat* NM_RESTRICT pMatrix16, const int* viewport4, nmoat nearVal, nmoat farVal, nmoat* NM_RESTRICT objX, nmoat* NM_RESTRICT objY, nmoat* NM_RESTRICT objZ, nmoat* NM_RESTRICT objW);

NM_API_DEF_EXT_INL nmoat* nm_Mat4Translate(nmoat* NM_RESTRICT mInOut16,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat4Rotate(nmoat* NM_RESTRICT mInOut16,nmoat degAngle,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat4Scale(nmoat* NM_RESTRICT mInOut16,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL void nm_Mat4TransposeInPlace(nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4Transpose(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL int nm_Mat4Invert(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL int nm_Mat4InvertInPlace(nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4InvertTransformMatrixWithoutScaling(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4InvertTransformMatrix(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL void nm_Mat4InvertXZAxisInPlace(nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4InvertXZAxis(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16);

NM_API_DEF_EXT_INL void nm_Mat4MulDir(const nmoat* NM_RESTRICT m16,nmoat* NM_RESTRICT dirOut3,const nmoat dirX,nmoat dirY,nmoat dirZ);
NM_API_DEF_EXT_INL void nm_Mat4MulPos(const nmoat* NM_RESTRICT m16,nmoat* NM_RESTRICT posOut3,const nmoat posX,nmoat posY,nmoat posZ);

NM_API_DEF_EXT_INL int nm_AreMat4EqualEps(const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16,nmoat eps);
NM_API_DEF_EXT_INL int nm_AreMat4Equal(const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16);

NM_API_DEF_EXT_INL nmoat* nm_QuatFromMat4(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT m16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4SetRotationFromQuat(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT q4);
NM_API_DEF_EXT_INL nmoat* nm_Mat3GetFromMat4(nmoat* NM_RESTRICT dst9,const nmoat* NM_RESTRICT src16);
NM_API_DEF_EXT_INL void nm_Mat4SetRotationFromMat3(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT src9);

NM_API_DEF_EXT_INL const nmoat* nm_Mat4FromMRotScaTra(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT rot9,nmoat scaX,nmoat scaY,nmoat scaZ,nmoat traX,nmoat traY,nmoat traZ);
NM_API_DEF_EXT_INL const nmoat* nm_Mat4FromQRotScaTra(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT rot4,nmoat scaX,nmoat scaY,nmoat scaZ,nmoat traX,nmoat traY,nmoat traZ);
NM_API_DEF_EXT_INL const nmoat* nm_Mat4SlerpEps(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT mStart16,const nmoat* NM_RESTRICT mEnd16,nmoat slerpTime_In_0_1,nmoat eps);
NM_API_DEF_EXT_INL const nmoat* nm_Mat4Slerp(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT mStart16,const nmoat* NM_RESTRICT mEnd16,nmoat slerpTime_In_0_1);

NM_API_DEF_EXT_INL nmoat* nm_Mat4Mul3(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16,const nmoat* NM_RESTRICT c16);
NM_API_DEF_EXT_INL nmoat* nm_Mat4Mul3_NoCheck(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16,const nmoat* NM_RESTRICT c16);

NM_API_DEF_EXT_INL nmoat* nm_Mat4FromEulerXYZ(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat4FromEulerXZY(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat4FromEulerYXZ(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat4FromEulerYZX(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat4FromEulerZXY(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z);
NM_API_DEF_EXT_INL nmoat* nm_Mat4FromEulerZYX(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z);

NM_API_DEF_EXT_INL int nm_Mat4ToEulerXYZ(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat4ToEulerXZY(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat4ToEulerYXZ(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat4ToEulerYZX(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat4ToEulerZXY(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int nm_Mat4ToEulerZYX(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z);

NM_API_DEF_EXT_INL void nm_Mat4FromEuler(nmoat* m16,nmoat x,nmoat y,nmoat z,int eulerRotationMode);
NM_API_DEF_EXT_INL int nm_Mat4ToEuler(const nmoat* m16,nmoat* x,nmoat* y,nmoat* z,int eulerRotationMode);

NM_API_DEF_EXT_INL const nmoat* nm_Mat4LookAtYX(nmoat* NM_RESTRICT mInOut16,nmoat lookAtX,nmoat lookAtY,nmoat lookAtZ,nmoat minDistanceAllowed/*=0*/,nmoat maxDistanceAllowed/*=0*/,nmoat pitchLimit/* = 1.483486111*/);
NM_API_DEF_EXT_INL const nmoat* nm_Mat4LookAtYX2D(nmoat* NM_RESTRICT mInOut16,nmoat lookAtX,nmoat lookAtZ,nmoat minDistanceAllowed/*=0*/,nmoat maxDistanceAllowed/*=0*/,nmoat pitchLimit/* = 1.483486111*/);

NM_API_DEF_EXT_INL int nm_AreMat3EqualEps(const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9,nmoat eps);
NM_API_DEF_EXT_INL int nm_AreMat3Equal(const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9);

NM_API_DEF_EXT_INL int nm_AreQuatEqualEps(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat eps);
NM_API_DEF_EXT_INL int nm_AreQuatEqual(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);

NM_API_DEF_EXT_INL int nm_AreVec4EqualEps(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat eps);
NM_API_DEF_EXT_INL int nm_AreVec4Equal(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4);

NM_API_DEF_EXT_INL int nm_AreVec3EqualEps(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3,nmoat eps);
NM_API_DEF_EXT_INL int nm_AreVec3Equal(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3);

NM_API_DEF_EXT_INL int nm_AreVec2EqualEps(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2,nmoat eps);
NM_API_DEF_EXT_INL int nm_AreVec2Equal(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2);

NM_API_DEF_EXT_INL int nm_AreVec3EqualEps3(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3,const nmoat* NM_RESTRICT eps3);
NM_API_DEF_EXT_INL int nm_AreVec3EqualEpsRadius(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3,nmoat epsRadiusSquared);
NM_API_DEF_EXT_INL int nm_AreVec2EqualEps2(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2,const nmoat* NM_RESTRICT eps2);
NM_API_DEF_EXT_INL int nm_AreVec2EqualEpsRadius(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2,nmoat epsRadiusSquared);

#ifndef NM_NO_STDIO
NM_API_DEF_EXT_INL void nm_Vec2_fprintp(const nmoat* NM_RESTRICT v2,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void nm_Vec2_fprint(const nmoat* NM_RESTRICT v2,FILE* stream);
NM_API_DEF_EXT_INL void nm_Vec2_printp(const nmoat* NM_RESTRICT v2,int width, int precision);

NM_API_DEF_EXT_INL void nm_Vec3_fprintp(const nmoat* NM_RESTRICT v3,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void nm_Vec3_fprint(const nmoat* NM_RESTRICT v3,FILE* stream);
NM_API_DEF_EXT_INL void nm_Vec3_printp(const nmoat* NM_RESTRICT v3,int width, int precision);
NM_API_DEF_EXT_INL void nm_Vec3_print(const nmoat* NM_RESTRICT v3);

NM_API_DEF_EXT_INL void nm_Vec4_fprintp(const nmoat* NM_RESTRICT v4,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void nm_Vec4_fprint(const nmoat* NM_RESTRICT v4,FILE* stream);
NM_API_DEF_EXT_INL void nm_Vec4_printp(const nmoat* NM_RESTRICT v4,int width, int precision);
NM_API_DEF_EXT_INL void nm_Vec4_print(const nmoat* NM_RESTRICT v4);

NM_API_DEF_EXT_INL void nm_Quat_fprintp(const nmoat* NM_RESTRICT q4,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void nm_Quat_fprint(const nmoat* NM_RESTRICT q4,FILE* stream);
NM_API_DEF_EXT_INL void nm_Quat_printp(const nmoat* NM_RESTRICT q4,int width, int precision);
NM_API_DEF_EXT_INL void nm_Quat_print(const nmoat* NM_RESTRICT q4);

NM_API_DEF_EXT_INL void nm_Mat3_fprintp(const nmoat* NM_RESTRICT m9,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void nm_Mat3_fprint(const nmoat* NM_RESTRICT m9,FILE* stream);
NM_API_DEF_EXT_INL void nm_Mat3_printp(const nmoat* NM_RESTRICT m9,int width, int precision);
NM_API_DEF_EXT_INL void nm_Mat3_print(const nmoat* NM_RESTRICT m9);

NM_API_DEF_EXT_INL void nm_Mat4_fprintp(const nmoat* NM_RESTRICT m16,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void nm_Mat4_fprint(const nmoat* NM_RESTRICT m16,FILE* stream);
NM_API_DEF_EXT_INL void nm_Mat4_printp(const nmoat* NM_RESTRICT m16,int width, int precision);
NM_API_DEF_EXT_INL void nm_Mat4_print(const nmoat* NM_RESTRICT m16);
#endif //NM_NO_STDIO


NM_API_DEF_EXT_INL void nm_PlaneNormalize(nmoat* NM_RESTRICT p4);
NM_API_DEF_EXT_INL nmoat nm_PlanePointDistance(const nmoat* NM_RESTRICT plane4,const nmoat* NM_RESTRICT pt3);


// returns the frustum radius (by value) and the scalar (positive) distance from the the camera eye to the frustum center (by 'pFrustumCenterDistanceOut').
// 'cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero': the arg name is correct when the result is passed to 'nm_GetLightViewProjectionMatrix' functions.
// Otherwise simply set it when using an ortho3D camera frustum (that needs a camera-target distance), or just set it to zero.
NM_API_DEF_EXT_INL nmoat nm_GetFrustumRadiusAndCenterDistance(nmoat* NM_RESTRICT pFrustumCenterDistanceOut,nmoat cameraNearClippingPlane,nmoat cameraFarClippingPlane,nmoat cameraFovyDeg,nmoat cameraAspectRatio,nmoat cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero);
// 'frustumCenterOut3' is in world space (mandatory arg).
// 'cameraVMatrixInverse16' must be orthonormal (no scaling).
NM_API_DEF_EXT_INL void nm_GetFrustumCenterFromCenterDistance(nmoat* NM_RESTRICT frustumCenterOut3,const nmoat* NM_RESTRICT cameraVMatrixInverse16,nmoat frustumCenterDistance);
// 'optionalPMatrixInverse16' is required only if you need to retrieve (one or more of) the arguments that follow them (otherwise their value is untouched).
// 'normalizedLightDirection3' must be in world space.
NM_API_DEF_EXT_INL void nm_GetLightViewProjectionMatrixExtra(nmoat* NM_RESTRICT lvpMatrixOut16
                                                          ,const nmoat* NM_RESTRICT cameraVMatrixInverse16,const nmoat* NM_RESTRICT cameraFrustumCenterInWorldSpace,nmoat cameraFrustumRadius
                                                          ,const nmoat*  NM_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight
                                                          ,const nmoat* NM_RESTRICT optionalCameraPMatrixInverse16
                                                          ,nmoat* NM_RESTRICT optionalLightViewportClippingOut4,nmoat optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,nmoat* NM_RESTRICT optionalLVPMatrixForFrustumCullingUsageOut16   // Highly experimental and untested
                                                          );
// 'normalizedLightDirection3' must be in world space.
NM_API_DEF_EXT_INL void nm_GetLightViewProjectionMatrix(nmoat* NM_RESTRICT lvpMatrixOut16
                                                         ,const nmoat* NM_RESTRICT cameraVMatrixInverse16,const nmoat* NM_RESTRICT cameraFrustumCenterInWorldSpace,nmoat cameraFrustumRadius
                                                         ,const nmoat* NM_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight);
NM_API_DEF_EXT_INL void nm_GetFrustumPlaneEquations(nmoat planeEquationsOut[6][4],const nmoat* NM_RESTRICT vpMatrix16,int normalizePlanes);
NM_API_DEF_EXT_INL int nm_FrustumOBBVisibilityTest(const nmoat frustumPlanes[6][4],const nmoat* NM_RESTRICT mfMatrix16,nmoat aabbMinX,nmoat aabbMinY,nmoat aabbMinZ,nmoat aabbMaxX,nmoat aabbMaxY,nmoat aabbMaxZ);
NM_API_DEF_EXT_INL int nm_FrustumBSVisibilityTest(const nmoat normalizedFrustumPlanes[6][4],const nmoat* NM_RESTRICT mfMatrix16,nmoat bsCenterX,nmoat bsCenterY,nmoat bsCenterZ,nmoat bsRadius,const nmoat* NM_RESTRICT optionalFrustumCenterInMfSpace3,nmoat optionalFrustumRadius);
NM_API_DEF_EXT_INL int nm_IsAabbVisible(const nmoat normalizedFrustumPlanes[6][4],const nmoat* NM_RESTRICT mfMatrix16,const nmoat* NM_RESTRICT aabbMin3,const nmoat* NM_RESTRICT aabbMax3,nmoat aabbRadius,nmoat scalingX,nmoat scalingY,nmoat scalingZ,const nmoat* NM_RESTRICT optionalFrustumCenterInMfSpace3,nmoat optionalFrustumRadius);


// These two function used to be implementation-private, and are now exposed (TODO: find out what they do and document them)
NM_API_DEF_EXT_INL void nm_GetFrustumPoints(nmoat frustumPoints[8][4],const nmoat* NM_RESTRICT vpMatrixInverse16);
NM_API_DEF_EXT_INL void nm_GetFrustumAabbCenterAndHalfExtents(nmoat* NM_RESTRICT frustumCenterOut3,nmoat* NM_RESTRICT frustumHalfExtentsOut3,const nmoat frustumPoints[8][4]);

// these defs starts at 1 to match an enum order used by Blender
// (note that when NM_NO_MATH_STRUCTS is defined, enum EulerRotationMode is not present)
#define NM_EULER_XYZ 1
#define NM_EULER_XZY 2
#define NM_EULER_YXZ 3
#define NM_EULER_YZX 4
#define NM_EULER_ZXY 5
#define NM_EULER_ZYX 6


#ifdef __cplusplus
} // extern "C"
#endif

#ifdef NM_NAMESPACE
namespace NM_NAMESPACE  {
#endif //NM_NAMESPACE


#ifndef NM_NO_MATH_STRUCTS

// Warning: enum starts at 1 to match an enum order used by Blender
 enum EulerRotationMode {
    EULER_XYZ = NM_EULER_XYZ,  // 1
    EULER_XZY = NM_EULER_XZY,  // 2
    EULER_YXZ = NM_EULER_YXZ,  // 3
    EULER_YZX = NM_EULER_YZX,  // 4
    EULER_ZXY = NM_EULER_ZXY,  // 5
    EULER_ZYX = NM_EULER_ZYX   // 6
    //AXIS_ANGLE = -1       // (these are used by Blender too)
    //QUATERNION_WXYZ = 0
};
typedef enum EulerRotationMode EulerRotationMode;

NM_API_DEF_EXT_INL nmoat radians(nmoat deg);
NM_API_DEF_EXT_INL nmoat degrees(nmoat rad);

// ---------------
// vec2 definition
// ---------------
#ifdef NM_NO_CPP_CTRS
#	ifndef NM_NO_C_INITIALIZATION
#   	define NM_RETURN_VEC2_CTR(X,Y)   return (vec2){{{(X),(Y)}}};
#	else //NM_NO_C_INITIALIZATION
#		define NM_RETURN_VEC2_CTR(X,Y)	  {vec2 r;r.x=X;r.y=Y;return r;}
#	endif //NM_NO_C_INITIALIZATION
#else
#   define NM_RETURN_VEC2_CTR(X,Y)   return vec2(X,Y);
#endif


NM_PRE_ALIGN_DEFAULT struct vec2 {
    // if we had to use alignas(...), we should have put it here (AFAIK it's the same)
    union	{
        struct {nmoat x,y;};
        nmoat e[2];
    };
#	ifdef __cplusplus
#   ifndef NM_NO_CPP_CTRS
    NM_API_INL vec2() : x(0),y(0) {}
    NM_API_INL vec2(nmoat v) : x(v),y(v) {}
    NM_API_INL vec2(nmoat _x,nmoat _y) : x(_x),y(_y) {}
    NM_API_INL vec2(const vec2& o) : x(o.x),y(o.y) {}
#   endif //NM_NO_CPP_CTRS

#   ifndef NM_NO_STDIO
    void fprintp(FILE* stream,int width, int precision) const;
    void fprint(FILE* stream) const;
    void printp(int width, int precision) const;
    void print() const;
#   endif //NM_NO_STDIO

    //NM_API_INL nmoat operator[](int i) const {NM_ASSERT(i>=0 && i<2);return *(((nmoat*)&x)+i);}
    //NM_API_INL nmoat& operator[](int i) {NM_ASSERT(i>=0 && i<2);return *(((nmoat*)&x)+i);}
    NM_API_INL const nmoat& operator[](int i) const {NM_ASSERT(i>=0 && i<2);return e[i];}
    NM_API_INL nmoat& operator[](int i) {NM_ASSERT(i>=0 && i<2);return e[i];}
    NM_API_INL const vec2& operator=(const vec2& o) {x=o.x;y=o.y;return *this;}
    NM_API_INL const vec2& operator=(nmoat s) {x=s;y=s;return *this;}

    NM_API_INL vec2 operator+(const vec2& o) const	{NM_RETURN_VEC2_CTR(x+o.x,y+o.y)}
    NM_API_INL void operator+=(const vec2& o) {x+=o.x;y+=o.y;}
    NM_API_INL vec2 operator+(nmoat s) const	{NM_RETURN_VEC2_CTR(x+s,y+s)}
    NM_API_INL void operator+=(nmoat s) {x+=s;y+=s;}

    NM_API_INL vec2 operator-(const vec2& o) const	{NM_RETURN_VEC2_CTR(x-o.x,y-o.y)}
    NM_API_INL void operator-=(const vec2& o) {x-=o.x;y-=o.y;}
    NM_API_INL vec2 operator-(nmoat s) const	{NM_RETURN_VEC2_CTR(x-s,y-s)}
    NM_API_INL void operator-=(nmoat s) {x-=s;y-=s;}
    NM_API_INL vec2 operator-() const	{NM_RETURN_VEC2_CTR(-x,-y)}

    NM_API_INL vec2 operator*(const vec2& o) const	{NM_RETURN_VEC2_CTR(x*o.x,y*o.y)}
    NM_API_INL void operator*=(const vec2& o) {x*=o.x;y*=o.y;}
    NM_API_INL vec2 operator*(nmoat s) const	{NM_RETURN_VEC2_CTR(x*s,y*s)}
    NM_API_INL void operator*=(nmoat s) {x*=s;y*=s;}

    NM_API_INL vec2 operator/(const vec2& o) const	{NM_RETURN_VEC2_CTR(x/o.x,y/o.y)}
    NM_API_INL void operator/=(const vec2& o) {x/=o.x;y/=o.y;}
    NM_API_INL vec2 operator/(nmoat s) const {NM_RETURN_VEC2_CTR(x/s,y/s)}
    NM_API_INL void operator/=(nmoat s) {x/=s;y/=s;}

    NM_API_INL nmoat dot(const vec2& o) const {return x*o.x+y*o.y;}

    NM_API_INL nmoat  length2()  const  {return x*x+y*y;}
    NM_API_INL nmoat  length()  const  {return nmh_sqrt(x*x+y*y);}
    NM_API_INL void normalize() {const nmoat len=length();if (len>0) {x/=len;y/=len;} else {x=y=(nmoat)0;}}
    NM_API_INL vec2 normalized() const	{const nmoat len=length();if (len>0) NM_RETURN_VEC2_CTR(x/len,y/len) else NM_RETURN_VEC2_CTR(0,0)}

    NM_API_INL vec2 lerp(const vec2& o,nmoat t) const {nmoat ct = (nmoat)1-t;NM_RETURN_VEC2_CTR(x*ct+o.x*t,y*ct+o.y*t)}

    NM_API_INL vec2 proj(const vec2& onto) const {return onto*this->dot(onto)/onto.dot(onto);}
    NM_API_INL nmoat angle_between(const vec2& o) const {return nmh_acos(this->dot(o)/(this->length()*o.length()));}



#	endif //__cplusplus
} NM_POST_ALIGN_DEFAULT;

#ifndef __cplusplus
typedef struct vec2 vec2;
#endif
NM_API_DEF_INL vec2 make_vec2(nmoat _x, nmoat _y) {NM_RETURN_VEC2_CTR(_x,_y)}

#ifndef NM_NO_STDIO
NM_API_DEF_EXT_INL void vec2_fprintp(const vec2* v2,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void vec2_fprint(const vec2* v2,FILE* stream);
NM_API_DEF_EXT_INL void vec2_printp(const vec2* v2,int width, int precision);
NM_API_DEF_EXT_INL void vec2_print(const vec2* v2);
#ifdef __cplusplus
NM_API_INL void vec2::fprintp(FILE* stream,int width, int precision) const {vec2_fprintp(this,stream,width,precision);}
NM_API_INL void vec2::fprint(FILE* stream) const   {vec2_fprint(this,stream);}
NM_API_INL void vec2::printp(int width, int precision) const {vec2_printp(this,width,precision);}
NM_API_INL void vec2::print() const    {vec2_print(this);}
#endif //__cplusplus
#endif //NM_NO_STDIO
//NM_API_DEF_EXT_INL vec2 vec2_copy(const vec2* src);
NM_API_DEF_EXT_INL vec2 vec2_add(const vec2* a,const vec2* b);
NM_API_DEF_EXT_INL vec2 vec2_sub(const vec2* a,const vec2* b);
NM_API_DEF_EXT_INL vec2 vec2_neg(const vec2* src);
NM_API_DEF_EXT_INL vec2 vec2_mul(const vec2* a,const vec2* b);
NM_API_DEF_EXT_INL vec2 vec2_mulsca(const vec2* src,nmoat f);
NM_API_DEF_EXT_INL vec2 vec2_div(const vec2* a,const vec2* b);
NM_API_DEF_EXT_INL vec2 vec2_divsca(const vec2* src,nmoat f);
NM_API_DEF_EXT_INL const vec2* vec2_sadd(vec2* dst,const vec2* src);
NM_API_DEF_EXT_INL const vec2* vec2_ssub(vec2* dst,const vec2* src);
NM_API_DEF_EXT_INL const vec2* vec2_inv(vec2* dst);
NM_API_DEF_EXT_INL const vec2* vec2_smul(vec2* dst,const vec2* src);
NM_API_DEF_EXT_INL const vec2* vec2_smulsca(vec2* dst,nmoat f);
NM_API_DEF_EXT_INL const vec2* vec2_sdiv(vec2* dst,const vec2* src);
NM_API_DEF_EXT_INL const vec2* vec2_sdivsca(vec2* dst,nmoat f);
NM_API_DEF_EXT_INL nmoat vec2_dot(const vec2* a,const vec2* b);
NM_API_DEF_EXT_INL nmoat vec2_length2(const vec2* a);
NM_API_DEF_EXT_INL nmoat vec2_length(const vec2* a);
NM_API_DEF_EXT_INL const vec2* vec2_normalize(vec2* dst);
NM_API_DEF_EXT_INL vec2 vec2_normalized(const vec2* src);
NM_API_DEF_EXT_INL vec2 vec2_lerp(const vec2* a,const vec2* b,nmoat t);
NM_API_DEF_EXT_INL vec2 vec2_proj(const vec2* src,const vec2* onto);
NM_API_DEF_EXT_INL nmoat vec2_angle_between(const vec2* a,const vec2* b);
NM_API_DEF_EXT_INL int vec2_areequaleps(const vec2* a,const vec2* b,nmoat eps);
NM_API_DEF_EXT_INL int vec2_areequal(const vec2* a,const vec2* b);


// ---------------
// vec3 definition
// ---------------
#ifdef NM_NO_CPP_CTRS
#	ifndef NM_NO_C_INITIALIZATION
#   	define NM_RETURN_VEC3_CTR(X,Y,Z)   return (vec3){{{(X),(Y),(Z)}}};
#	else //NM_NO_C_INITIALIZATION
#		define NM_RETURN_VEC3_CTR(X,Y,Z)	  {vec3 r;r.x=X;r.y=Y;r.z=Z;return r;}
#	endif //NM_NO_C_INITIALIZATION
#else
#   define NM_RETURN_VEC3_CTR(X,Y,Z)   return vec3(X,Y,Z);
#endif

#undef NM_VEC3_MUST_BE_POD
#undef NM_QUERY_MAT3_VEC3_AXIS_VECTORS
#undef NM_QUERY_MAT4_VEC3_AXIS_VECTORS
#if (defined(NM_NO_CPP_CTRS) || defined(NM_COMPILER_SUPPORTS_NONPOD_ANONYMOUS_MEMBERS))
#   ifndef NM_NO_AXIS_VECTORS_IN_STRUCT_MAT3
#       define NM_QUERY_MAT3_VEC3_AXIS_VECTORS
#   endif //NM_NO_AXIS_VECTORS_IN_STRUCT_MAT3
#   if (!defined(NM_NO_AXIS_VECTORS_IN_STRUCT_MAT4) && !defined(NM_AXIS_VEC4_IN_STRUCT_MAT4))
#       define NM_QUERY_MAT4_VEC3_AXIS_VECTORS
#   endif
#endif
#if (defined(NM_QUERY_MAT3_VEC3_AXIS_VECTORS) || defined(NM_QUERY_MAT4_VEC3_AXIS_VECTORS))
#   define NM_VEC3_MUST_BE_POD
#endif

NM_PRE_ALIGN_DEFAULT struct vec3 {
    union	{
        struct {nmoat x,y,z;};
        nmoat e[3];
    };
#	ifdef __cplusplus
#   ifndef NM_NO_CPP_CTRS
    NM_API_INL vec3() : x(0),y(0),z(0) {}
    NM_API_INL vec3(nmoat v) : x(v),y(v),z(v) {}
    NM_API_INL vec3(nmoat _x,nmoat _y,nmoat _z) : x(_x),y(_y),z(_z) {}
    NM_API_INL vec3(const vec2& o,nmoat _z=(nmoat)0) : x(o.x),y(o.y),z(_z) {}
    NM_API_INL vec3(const vec3& o) : x(o.x),y(o.y),z(o.z) {}
#   endif //NM_NO_CPP_CTRS

#   if (!defined(NM_NO_CPP_CTRS) || !defined(NM_VEC3_MUST_BE_POD))
    // These are placed inside the def block, because, in some (rare) cases,
    // vec3 can be used inside an anonymous sub-member in mat3 or mat4 (and it must be POD)
    NM_API_INL const vec3& operator=(const vec3& o) {x=o.x;y=o.y;z=o.z;return *this;}
    NM_API_INL const vec3& operator=(const vec2& o) {x=o.x;y=o.y;return *this;}
    NM_API_INL const vec3& operator=(nmoat s) {x=y=z=s;return *this;}
#   endif // !NM_NO_CPP_CTRS || !NM_VEC3_MUST_BE_POD

#   ifndef NM_NO_STDIO
    void fprintp(FILE* stream,int width, int precision) const;
    void fprint(FILE* stream) const;
    void printp(int width, int precision) const;
    void print() const;
#   endif //NM_NO_STDIO

    NM_API_INL const nmoat& operator[](int i) const {NM_ASSERT(i>=0 && i<3);return e[i];}
    NM_API_INL nmoat& operator[](int i) {NM_ASSERT(i>=0 && i<3);return e[i];}

    NM_API_INL vec3 operator+(const vec3& o) const	{NM_RETURN_VEC3_CTR(x+o.x,y+o.y,z+o.z)}
    NM_API_INL void operator+=(const vec3& o) {x+=o.x;y+=o.y;z+=o.z;}
    NM_API_INL vec3 operator+(nmoat s) const	{NM_RETURN_VEC3_CTR(x+s,y+s,z+s)}
    NM_API_INL void operator+=(nmoat s) {x+=s;y+=s;z+=s;}

    NM_API_INL vec3 operator-(const vec3& o) const	{NM_RETURN_VEC3_CTR(x-o.x,y-o.y,z-o.z)}
    NM_API_INL void operator-=(const vec3& o) {x-=o.x;y-=o.y;z-=o.z;}
    NM_API_INL vec3 operator-(nmoat s) const	{NM_RETURN_VEC3_CTR(x-s,y-s,z-s)}
    NM_API_INL void operator-=(nmoat s) {x-=s;y-=s;z-=s;}
    NM_API_INL vec3 operator-() const	{NM_RETURN_VEC3_CTR(-x,-y,-z)}

    NM_API_INL vec3 operator*(const vec3& o) const	{NM_RETURN_VEC3_CTR(x*o.x,y*o.y,z*o.z)}
    NM_API_INL void operator*=(const vec3& o) {x*=o.x;y*=o.y;z*=o.z;}
    NM_API_INL vec3 operator*(nmoat s) const	{NM_RETURN_VEC3_CTR(x*s,y*s,z*s)}
    NM_API_INL void operator*=(nmoat s) {x*=s;y*=s;z*=s;}

    NM_API_INL vec3 operator/(const vec3& o) const	{NM_RETURN_VEC3_CTR(x/o.x,y/o.y,z/o.z)}
    NM_API_INL void operator/=(const vec3& o) {x/=o.x;y/=o.y;z/=o.z;}
    NM_API_INL vec3 operator/(nmoat s) const {NM_RETURN_VEC3_CTR(x/s,y/s,z/s)}
    NM_API_INL void operator/=(nmoat s) {x/=s;y/=s;z/=s;}

    NM_API_INL nmoat dot(const vec3& o) const {return x*o.x+y*o.y+z*o.z;}
    NM_API_INL vec3 cross(const vec3& o) const {NM_RETURN_VEC3_CTR(y*o.z-z*o.y,z*o.x-x*o.z,x*o.y-y*o.x)}

    NM_API_INL nmoat  length2()  const  {return x*x+y*y+z*z;}
    NM_API_INL nmoat  length()  const  {return nmh_sqrt(x*x+y*y+z*z);}
    NM_API_INL void normalize() {const nmoat len=length();if (len>0) {x/=len;y/=len;z/=len;} else {x=y=z=(nmoat)0;}}
    NM_API_INL vec3 normalized() const	{const nmoat len=length();if (len>0) NM_RETURN_VEC3_CTR(x/len,y/len,z/len) else NM_RETURN_VEC3_CTR(0,0,0)}

    NM_API_INL vec3 lerp(const vec3& o,nmoat t) const {nmoat ct = (nmoat)1-t;NM_RETURN_VEC3_CTR(x*ct+o.x*t,y*ct+o.y*t,z*ct+o.z*t)}

    NM_API_INL vec3 proj(const vec3& onto) const {return onto*this->dot(onto)/onto.dot(onto);}
    NM_API_INL nmoat angle_between(const vec3& o) const {return nmh_acos(this->dot(o)/(this->length()*o.length()));}

#	endif //__cplusplus
} NM_POST_ALIGN_DEFAULT;

typedef struct vec3 vec3;
NM_API_DEF_INL vec3 make_vec3(nmoat _x, nmoat _y, nmoat _z) {NM_RETURN_VEC3_CTR(_x,_y,_z)}

#ifndef NM_NO_STDIO
NM_API_DEF_EXT_INL void vec3_fprintp(const vec3* v3,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void vec3_fprint(const vec3* v3,FILE* stream);
NM_API_DEF_EXT_INL void vec3_printp(const vec3* v3,int width, int precision);
NM_API_DEF_EXT_INL void vec3_print(const vec3* v3);
#ifdef __cplusplus
NM_API_INL void vec3::fprintp(FILE* stream,int width, int precision) const {vec3_fprintp(this,stream,width,precision);}
NM_API_INL void vec3::fprint(FILE* stream) const   {vec3_fprint(this,stream);}
NM_API_INL void vec3::printp(int width, int precision) const {vec3_printp(this,width,precision);}
NM_API_INL void vec3::print() const    {vec3_print(this);}
#endif //__cplusplus
#endif //NM_NO_STDIO
//NM_API_DEF_EXT_INL vec3 vec3_copy(const vec3* src);
NM_API_DEF_EXT_INL vec3 vec3_add(const vec3* a,const vec3* b);
NM_API_DEF_EXT_INL vec3 vec3_sub(const vec3* a,const vec3* b);
NM_API_DEF_EXT_INL vec3 vec3_neg(const vec3* src);
NM_API_DEF_EXT_INL vec3 vec3_mul(const vec3* a,const vec3* b);
NM_API_DEF_EXT_INL vec3 vec3_mulsca(const vec3* src,nmoat f);
NM_API_DEF_EXT_INL vec3 vec3_div(const vec3* a,const vec3* b);
NM_API_DEF_EXT_INL vec3 vec3_divsca(const vec3* src,nmoat f);
NM_API_DEF_EXT_INL const vec3* vec3_sadd(vec3* dst,const vec3* src);
NM_API_DEF_EXT_INL const vec3* vec3_ssub(vec3* dst,const vec3* src);
NM_API_DEF_EXT_INL const vec3* vec3_inv(vec3* dst);
NM_API_DEF_EXT_INL const vec3* vec3_smul(vec3* dst,const vec3* src);
NM_API_DEF_EXT_INL const vec3* vec3_smulsca(vec3* dst,nmoat f);
NM_API_DEF_EXT_INL const vec3* vec3_sdiv(vec3* dst,const vec3* src);
NM_API_DEF_EXT_INL const vec3* vec3_sdivsca(vec3* dst,nmoat f);
NM_API_DEF_EXT_INL nmoat vec3_dot(const vec3* a,const vec3* b);
NM_API_DEF_EXT_INL vec3 vec3_cross(const vec3* a,const vec3* b);
NM_API_DEF_EXT_INL nmoat vec3_length3(const vec3* a);
NM_API_DEF_EXT_INL nmoat vec3_length(const vec3* a);
NM_API_DEF_EXT_INL const vec3* vec3_normalize(vec3* dst);
NM_API_DEF_EXT_INL vec3 vec3_normalized(const vec3* src);
NM_API_DEF_EXT_INL vec3 vec3_lerp(const vec3* a,const vec3* b,nmoat t);
NM_API_DEF_EXT_INL vec3 vec3_proj(const vec3* src,const vec3* onto);
NM_API_DEF_EXT_INL nmoat vec3_angle_between(const vec3* a,const vec3* b);
NM_API_DEF_EXT_INL int vec3_areequaleps(const vec3* a,const vec3* b,nmoat eps);
NM_API_DEF_EXT_INL int vec3_areequal(const vec3* a,const vec3* b);



// ---------------
// vec4 definition
// ---------------
#ifdef NM_NO_CPP_CTRS
#	ifndef NM_NO_C_INITIALIZATION
#   	define NM_RETURN_VEC4_CTR(X,Y,Z,W)   return (vec4){{{(X),(Y),(Z),(W)}}};
#	else //NM_NO_C_INITIALIZATION
#		define NM_RETURN_VEC4_CTR(X,Y,Z,W)	  {vec4 r;r.x=X;r.y=Y;r.z=Z;r.w=W;return r;}
#	endif //NM_NO_C_INITIALIZATION
#else
#   define NM_RETURN_VEC4_CTR(X,Y,Z,W)   return vec4(X,Y,Z,W);
#endif

#undef NM_VEC4_MUST_BE_POD
#undef NM_QUERY_MAT4_VEC4_AXIS_VECTORS
#if (defined(NM_NO_CPP_CTRS) || defined(NM_COMPILER_SUPPORTS_NONPOD_ANONYMOUS_MEMBERS))
#   if (!defined(NM_NO_AXIS_VECTORS_IN_STRUCT_MAT4) && defined(NM_AXIS_VEC4_IN_STRUCT_MAT4))
#       define NM_QUERY_MAT4_VEC4_AXIS_VECTORS
#       define NM_VEC4_MUST_BE_POD
#   endif
#endif
#if (defined(NM_QUERY_MAT4_VEC3_AXIS_VECTORS) && defined(NM_QUERY_MAT4_VEC4_AXIS_VECTORS))
#   error NM_QUERY_MAT4_VEC3_AXIS_VECTORS and NM_QUERY_MAT4_VEC4_AXIS_VECTORS should be mutually exclusive.
#endif

NM_PRE_ALIGN_DEFAULT struct vec4 {
    union	{
        struct {nmoat x,y,z,w;};
        struct {nmoat r,g,b,a;};
        nmoat e[4];
    };
#	ifdef __cplusplus
#   ifndef NM_NO_CPP_CTRS
    NM_API_INL vec4() : x(0),y(0),z(0),w(0) {}
    NM_API_INL vec4(nmoat v) : x(v),y(v),z(v),w(v) {}
    NM_API_INL vec4(nmoat _x,nmoat _y,nmoat _z,nmoat _w) : x(_x),y(_y),z(_z),w(_w) {}
    NM_API_INL vec4(const vec2& o,nmoat _z=(nmoat)0,nmoat _w=(nmoat)0) : x(o.x),y(o.y),z(_z),w(_w) {}
    NM_API_INL vec4(nmoat _x,nmoat _y,const vec2& zw) : x(_x),y(_y),z(zw.x),w(zw.y) {}
    NM_API_INL vec4(const vec2& xy,const vec2& zw) : x(xy.x),y(xy.y),z(zw.x),w(zw.y) {}
    NM_API_INL vec4(const vec3& o,nmoat _w=(nmoat)0) : x(o.x),y(o.y),z(o.z),w(_w) {}
    NM_API_INL vec4(const vec4& o) : x(o.x),y(o.y),z(o.z),w(o.w) {}
#   endif //NM_NO_CPP_CTRS

#   if (!defined(NM_NO_CPP_CTRS) || !defined(NM_VEC4_MUST_BE_POD))
    // These are placed inside this def block, because, in some (very rare) cases,
    // vec4 can be used inside an anonymous sub-struct in mat4
    NM_API_INL const vec4& operator=(const vec4& o) {x=o.x;y=o.y;z=o.z;w=o.w;return *this;}
    NM_API_INL const vec4& operator=(const vec2& o) {x=o.x;y=o.y;return *this;}
    NM_API_INL const vec4& operator=(const vec3& o) {x=o.x;y=o.y;z=o.z;return *this;}
    NM_API_INL const vec4& operator=(nmoat s) {x=y=z=w=s;return *this;}
#   endif // !NM_NO_CPP_CTRS || !NM_VEC4_MUST_BE_POD

#   ifndef NM_NO_STDIO
    void fprintp(FILE* stream,int width, int precision) const;
    void fprint(FILE* stream) const;
    void printp(int width, int precision) const;
    void print() const;
#   endif //NM_NO_STDIO

    NM_API_INL const nmoat& operator[](int i) const {NM_ASSERT(i>=0 && i<4);return e[i];}
    NM_API_INL nmoat& operator[](int i) {NM_ASSERT(i>=0 && i<4);return e[i];}

    NM_API_INL vec4 operator+(const vec4& o) const	{NM_RETURN_VEC4_CTR(x+o.x,y+o.y,z+o.z,w+o.w)}
    NM_API_INL void operator+=(const vec4& o) {x+=o.x;y+=o.y;z+=o.z;w+=o.w;}
    NM_API_INL vec4 operator+(nmoat s) const	{NM_RETURN_VEC4_CTR(x+s,y+s,z+s,w+s)}
    NM_API_INL void operator+=(nmoat s) {x+=s;y+=s;z+=s;w+=s;}

    NM_API_INL vec4 operator-(const vec4& o) const	{NM_RETURN_VEC4_CTR(x-o.x,y-o.y,z-o.z,w-o.w)}
    NM_API_INL void operator-=(const vec4& o) {x-=o.x;y-=o.y;z-=o.z;w-=o.w;}
    NM_API_INL vec4 operator-(nmoat s) const	{NM_RETURN_VEC4_CTR(x-s,y-s,z-s,w-s)}
    NM_API_INL void operator-=(nmoat s) {x-=s;y-=s;z-=s;w-=s;}
    NM_API_INL vec4 operator-() const	{NM_RETURN_VEC4_CTR(-x,-y,-z,-w)}

    NM_API_INL vec4 operator*(const vec4& o) const	{NM_RETURN_VEC4_CTR(x*o.x,y*o.y,z*o.z,w*o.w)}
    NM_API_INL void operator*=(const vec4& o) {x*=o.x;y*=o.y;z*=o.z;w*=o.w;}
    NM_API_INL vec4 operator*(nmoat s) const	{NM_RETURN_VEC4_CTR(x*s,y*s,z*s,w*s)}
    NM_API_INL void operator*=(nmoat s) {x*=s;y*=s;z*=s;w*=s;}

    NM_API_INL vec4 operator/(const vec4& o) const	{NM_RETURN_VEC4_CTR(x/o.x,y/o.y,z/o.z,w/o.w)}
    NM_API_INL void operator/=(const vec4& o) {x/=o.x;y/=o.y;z/=o.z;w/=o.w;}
    NM_API_INL vec4 operator/(nmoat s) const {NM_RETURN_VEC4_CTR(x/s,y/s,z/s,w/s)}
    NM_API_INL void operator/=(nmoat s) {x/=s;y/=s;z/=s;w/=s;}

    NM_API_INL nmoat dot(const vec4& o) const {return x*o.x+y*o.y+z*o.z+w*o.w;}

    NM_API_INL nmoat  length2()  const  {return x*x+y*y+z*z+w*w;}
    NM_API_INL nmoat  length()  const  {return nmh_sqrt(x*x+y*y+z*z+w*w);}
    NM_API_INL void normalize() {const nmoat len=length();if (len>0) {x/=len;y/=len;z/=len;w/=len;} else {x=y=z=w=(nmoat)0;}}
    NM_API_INL vec4 normalized() const	{const nmoat len=length();if (len>0) NM_RETURN_VEC4_CTR(x/len,y/len,z/len,w/len) else NM_RETURN_VEC4_CTR(0,0,0,0)}

    NM_API_INL vec4 lerp(const vec4& o,nmoat t) const {nmoat ct = (nmoat)1-t;NM_RETURN_VEC4_CTR(x*ct+o.x*t,y*ct+o.y*t,z*ct+o.z*t,w*ct+o.w*t)}

    NM_API_INL vec4 proj(const vec4& onto) const {return onto*this->dot(onto)/onto.dot(onto);}

    NM_API_INL /*NM_API_EXPLICIT*/ operator vec3() const {NM_RETURN_VEC3_CTR(x,y,z)}    // dangerous? But NM_API_EXPLICIT can be used only in C++11

#	endif //__cplusplus
} NM_POST_ALIGN_DEFAULT;

typedef struct vec4 vec4;
NM_API_DEF_INL vec4 make_vec4(nmoat _x, nmoat _y, nmoat _z, nmoat _w) {NM_RETURN_VEC4_CTR(_x,_y,_z,_w)}

#ifndef NM_NO_STDIO
NM_API_DEF_EXT_INL void vec4_fprintp(const vec4* v4,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void vec4_fprint(const vec4* v4,FILE* stream);
NM_API_DEF_EXT_INL void vec4_printp(const vec4* v4,int width, int precision);
NM_API_DEF_EXT_INL void vec4_print(const vec4* v4);
#ifdef __cplusplus
NM_API_INL void vec4::fprintp(FILE* stream,int width, int precision) const {vec4_fprintp(this,stream,width,precision);}
NM_API_INL void vec4::fprint(FILE* stream) const   {vec4_fprint(this,stream);}
NM_API_INL void vec4::printp(int width, int precision) const {vec4_printp(this,width,precision);}
NM_API_INL void vec4::print() const    {vec4_print(this);}
#endif //__cplusplus
#endif //NM_NO_STDIO
//NM_API_DEF_EXT_INL vec4 vec4_copy(const vec4* src);
NM_API_DEF_EXT_INL vec4 vec4_add(const vec4* a,const vec4* b);
NM_API_DEF_EXT_INL vec4 vec4_sub(const vec4* a,const vec4* b);
NM_API_DEF_EXT_INL vec4 vec4_neg(const vec4* src);
NM_API_DEF_EXT_INL vec4 vec4_mul(const vec4* a,const vec4* b);
NM_API_DEF_EXT_INL vec4 vec4_mulsca(const vec4* src,nmoat f);
NM_API_DEF_EXT_INL vec4 vec4_div(const vec4* a,const vec4* b);
NM_API_DEF_EXT_INL vec4 vec4_divsca(const vec4* src,nmoat f);
NM_API_DEF_EXT_INL const vec4* vec4_sadd(vec4* dst,const vec4* src);
NM_API_DEF_EXT_INL const vec4* vec4_ssub(vec4* dst,const vec4* src);
NM_API_DEF_EXT_INL const vec4* vec4_inv(vec4* dst);
NM_API_DEF_EXT_INL const vec4* vec4_smul(vec4* dst,const vec4* src);
NM_API_DEF_EXT_INL const vec4* vec4_smulsca(vec4* dst,nmoat f);
NM_API_DEF_EXT_INL const vec4* vec4_sdiv(vec4* dst,const vec4* src);
NM_API_DEF_EXT_INL const vec4* vec4_sdivsca(vec4* dst,nmoat f);
NM_API_DEF_EXT_INL nmoat vec4_dot(const vec4* a,const vec4* b);
NM_API_DEF_EXT_INL nmoat vec4_length2(const vec4* a);
NM_API_DEF_EXT_INL nmoat vec4_length(const vec4* a);
NM_API_DEF_EXT_INL const vec4* vec4_normalize(vec4* dst);
NM_API_DEF_EXT_INL vec4 vec4_normalized(const vec4* src);
NM_API_DEF_EXT_INL vec4 vec4_lerp(const vec4* a,const vec4* b,nmoat t);
NM_API_DEF_EXT_INL vec4 vec4_proj(const vec4* src,const vec4* onto);
NM_API_DEF_EXT_INL int vec4_areequaleps(const vec4* a,const vec4* b,nmoat eps);
NM_API_DEF_EXT_INL int vec4_areequal(const vec4* a,const vec4* b);



// ---------------
// quat definition
// ---------------
#ifdef NM_ASSERT_ON_BAD_QUATERNION_CTR
// w is the first component, not the last
#   define NM_ASSERT_QUAT_CTR(W,X,Y,Z) NM_ASSERT(!((Z)==1 && (X)==0 && (Y)==0 && (Z)==0))
#else //NM_ASSERT_ON_BAD_QUATERNION_CTR
#   define NM_ASSERT_QUAT_CTR(W,X,Y,Z) /* no-op */
#endif //NM_ASSERT_ON_BAD_QUATERNION_CTR
#ifdef NM_NO_CPP_CTRS
#	ifndef NM_NO_C_INITIALIZATION
#   	define NM_RETURN_QUAT_CTR(W,X,Y,Z)   return (quat){{{(X),(Y),(Z),(W)}}};
#	else //NM_NO_C_INITIALIZATION
#		define NM_RETURN_QUAT_CTR(W,X,Y,Z)	  {quat r;r.x=X;r.y=Y;r.z=Z;r.w=W;return r;}
#	endif //NM_NO_C_INITIALIZATION
#else
#   define NM_RETURN_QUAT_CTR(W,X,Y,Z)   return quat(W,X,Y,Z);
#endif

NM_PRE_ALIGN_DEFAULT struct quat {
    union	{
        struct {nmoat x,y,z,w;};
        nmoat e[4];
    };
#	ifdef __cplusplus
#   ifndef NM_NO_CPP_CTRS
    NM_API_INL quat() : x(0),y(0),z(0),w(1) {}
    NM_API_INL quat(nmoat _w,nmoat _x,nmoat _y,nmoat _z) : x(_x),y(_y),z(_z),w(_w) {NM_ASSERT_QUAT_CTR(w,x,y,z);}
    NM_API_INL quat(const quat& o) : x(o.x),y(o.y),z(o.z),w(o.w) {}
    NM_API_INL quat(nmoat angle,const vec3& axis) {fromAngleAxis(angle,axis);}
#   endif //NM_NO_CPP_CTRS

#   ifndef NM_NO_STDIO
    void fprintp(FILE* stream,int width, int precision) const;
    void fprint(FILE* stream) const;
    void printp(int width, int precision) const;
    void print() const;
#   endif //NM_NO_STDIO

    NM_API_INL const nmoat& operator[](int i) const {NM_ASSERT(i>=0 && i<4);return e[i];}
    NM_API_INL nmoat& operator[](int i) {NM_ASSERT(i>=0 && i<4);return e[i];}
    NM_API_INL const quat& operator=(const quat& o) {x=o.x;y=o.y;z=o.z;w=o.w;return *this;}
    NM_API_INL const quat& operator=(nmoat s) {x=y=z=w=s;return *this;}

    NM_API_INL quat operator+(const quat& o) const	{NM_RETURN_QUAT_CTR(x+o.x,y+o.y,z+o.z,w+o.w)}
    NM_API_INL void operator+=(const quat& o) {x+=o.x;y+=o.y;z+=o.z;w+=o.w;}
    NM_API_INL quat operator+(nmoat s) const	{NM_RETURN_QUAT_CTR(x+s,y+s,z+s,w+s)}
    NM_API_INL void operator+=(nmoat s) {x+=s;y+=s;z+=s;w+=s;}

    NM_API_INL quat operator-(const quat& o) const	{NM_RETURN_QUAT_CTR(x-o.x,y-o.y,z-o.z,w-o.w)}
    NM_API_INL void operator-=(const quat& o) {x-=o.x;y-=o.y;z-=o.z;w-=o.w;}
    NM_API_INL quat operator-(nmoat s) const	{NM_RETURN_QUAT_CTR(x-s,y-s,z-s,w-s)}
    NM_API_INL void operator-=(nmoat s) {x-=s;y-=s;z-=s;w-=s;}
    NM_API_INL quat operator-() const	{NM_RETURN_QUAT_CTR(-x,-y,-z,-w)}

    NM_API_INL quat operator*(const quat& o) const	{NM_RETURN_QUAT_CTR(
                                w * o.w - x * o.x - y * o.y - z * o.z,
                                w * o.x + x * o.w + y * o.z - z * o.y,
                                w * o.y + y * o.w + z * o.x - x * o.z,
                                w * o.z + z * o.w + x * o.y - y * o.x
                    )}
    NM_API_INL void operator*=(const quat& o) {*this=*this*o;}
    NM_API_INL quat operator*(nmoat s) const	{NM_RETURN_QUAT_CTR(x*s,y*s,z*s,w*s)}
    NM_API_INL void operator*=(nmoat s) {x*=s;y*=s;z*=s;w*=s;}

    NM_API_INL quat operator/(const quat& o) const	{NM_RETURN_QUAT_CTR(x/o.x,y/o.y,z/o.z,w/o.w)}
    NM_API_INL void operator/=(const quat& o) {x/=o.x;y/=o.y;z/=o.z;w/=o.w;}
    NM_API_INL quat operator/(nmoat s) const {NM_RETURN_QUAT_CTR(x/s,y/s,z/s,w/s)}
    NM_API_INL void operator/=(nmoat s) {x/=s;y/=s;z/=s;w/=s;}

    NM_API_INL nmoat dot(const quat& o) const {return x*o.x+y*o.y+z*o.z+w*o.w;}

    NM_API_INL nmoat  length2()  const  {return x*x+y*y+z*z+w*w;}
    NM_API_INL nmoat  length()  const  {return nmh_sqrt(x*x+y*y+z*z+w*w);}
    NM_API_INL void normalize() {const nmoat len=length();if (len>(nmoat)0.000001) {x/=len;y/=len;z/=len;w/=len;} else {x=y=z=(nmoat)0;w=(nmoat)1;}}
    NM_API_INL quat normalized() const	{const nmoat len=length();if (len>(nmoat)0.000001) NM_RETURN_QUAT_CTR(x/len,y/len,z/len,w/len) else NM_RETURN_QUAT_CTR(1,0,0,0)}

    NM_API_INL quat lerp(const quat& o,nmoat t,bool normalizeQOutAfterLerp=true) const {nmoat ct = (nmoat)1-t;quat rv;rv.x=x*ct+o.x*t;rv.y=y*ct+o.y*t;rv.z=z*ct+o.z*t;rv.w=w*ct+o.w*t;if (normalizeQOutAfterLerp) rv.normalize();return rv;}
    quat slerp(const quat& o,nmoat slerpTime_In_0_1,bool normalizeQOutAfterLerp=true,nmoat eps=nmoat(0.0001)) const;

    NM_API_INL quat conjugated() const {NM_RETURN_QUAT_CTR(w, -x, -y, -z)}
    NM_API_INL void conjugate() {x=-x;y=-y;z=-z;}
    NM_API_INL quat inverted() const	{return conjugated()/dot(*this);}
    NM_API_INL void inverse() {conjugate();(*this)/=dot(*this);}

    NM_API_INL vec3 operator*(const vec3& v) const    {
        vec3 QuatVector; {QuatVector.x=x;QuatVector.y=y;QuatVector.z=z;}
        const vec3 uv(QuatVector.cross(v));
        const vec3 uuv(QuatVector.cross(uv));

        return v + ((uv * w) + uuv) * (nmoat)2;
    }
    NM_API_INL vec3 mulInv(const vec3& v) const    {return inverted()*v;}

    void fromAngleAxis(nmoat rfAngle,const vec3& rkAxis);
    void toAngleAxis(nmoat& rfAngle,vec3& rkAxis) const;
#	endif //__cplusplus
} NM_POST_ALIGN_DEFAULT;

typedef struct quat quat;
NM_API_DEF_INL quat make_quat(nmoat _w, nmoat _x, nmoat _y, nmoat _z) {NM_ASSERT_QUAT_CTR(w,x,y,z);NM_RETURN_QUAT_CTR(_w,_x,_y,_z)}
NM_API_DEF_INL quat make_quat_from_vec3(nmoat _w, vec3* v) {NM_ASSERT_QUAT_CTR(_w,v->x,v->y,v->z);NM_RETURN_QUAT_CTR(_w,v->x,v->y,v->z)}

#ifndef NM_NO_STDIO
NM_API_DEF_EXT_INL void quat_fprintp(const quat* q,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void quat_fprint(const quat* q,FILE* stream);
NM_API_DEF_EXT_INL void quat_printp(const quat* q,int width, int precision);
NM_API_DEF_EXT_INL void quat_print(const quat* q);
#ifdef __cplusplus
NM_API_INL void quat::fprintp(FILE* stream,int width, int precision) const {quat_fprintp(this,stream,width,precision);}
NM_API_INL void quat::fprint(FILE* stream) const   {quat_fprint(this,stream);}
NM_API_INL void quat::printp(int width, int precision) const {quat_printp(this,width,precision);}
NM_API_INL void quat::print() const    {quat_print(this);}
#endif //__cplusplus
#endif //NM_NO_STDIO
//NM_API_DEF_EXT_INL quat quat_copy(const quat* src);
NM_API_DEF_EXT_INL quat quat_add(const quat* a,const quat* b);
NM_API_DEF_EXT_INL quat quat_sub(const quat* a,const quat* b);
NM_API_DEF_EXT_INL quat quat_neg(const quat* src);
NM_API_DEF_EXT_INL quat quat_mul3quats(const quat* a,const quat* b,const quat* c);
NM_API_DEF_EXT_INL quat quat_mul(const quat* a,const quat* b);
NM_API_DEF_EXT_INL quat quat_mulsca(const quat* src,nmoat f);
NM_API_DEF_EXT_INL quat quat_div(const quat* a,const quat* b);
NM_API_DEF_EXT_INL quat quat_divsca(const quat* src,nmoat f);
NM_API_DEF_EXT_INL const quat* quat_sadd(quat* dst,const quat* src);
NM_API_DEF_EXT_INL const quat* quat_ssub(quat* dst,const quat* src);
NM_API_DEF_EXT_INL const quat* quat_inv(quat* dst);
NM_API_DEF_EXT_INL const quat* quat_smul(quat* dst,const quat* src);
NM_API_DEF_EXT_INL const quat* quat_smulsca(quat* dst,nmoat f);
NM_API_DEF_EXT_INL const quat* quat_sdiv(quat* dst,const quat* src);
NM_API_DEF_EXT_INL const quat* quat_sdivsca(quat* dst,nmoat f);
NM_API_DEF_EXT_INL nmoat quat_dot(const quat* a,const quat* b);
NM_API_DEF_EXT_INL vec3 quat_crossvec(const quat* a,const vec3* b);
NM_API_DEF_EXT_INL nmoat quat_length2(const quat* a);
NM_API_DEF_EXT_INL nmoat quat_length(const quat* a);
NM_API_DEF_EXT_INL const quat* quat_normalize(quat* dst);
NM_API_DEF_EXT_INL quat quat_normalized(const quat* src);
NM_API_DEF_EXT_INL quat quat_lerp(const quat* a,const quat* b,nmoat t,int normalizeQOutAfterLerp/*=1*/);
NM_API_DEF_EXT_INL quat quat_slerp(const quat* a,const quat* b,nmoat slerpTime_In_0_1,int normalizeQOutAfterLerp/*=1*/,nmoat eps/*=(nmoat)0.0001*/);
NM_API_DEF_EXT_INL quat quat_conjugated(const quat* q);
NM_API_DEF_EXT_INL void quat_conjugate(quat* q);
NM_API_DEF_EXT_INL quat quat_inverted(const quat* q);
NM_API_DEF_EXT_INL void quat_inverse(quat* q);
NM_API_DEF_EXT_INL vec3 quat_mulvec(const quat* q,const vec3* v);
NM_API_DEF_EXT_INL vec3 quat_mulvecInv(const quat* q,const vec3* v);
NM_API_DEF_EXT_INL quat quat_fromAngleAxis(nmoat rfAngle,const vec3* rkAxis);
NM_API_DEF_EXT_INL void quat_toAngleAxis(const quat* q,nmoat* rfAngleOut,vec3* rkAxisOut);
NM_API_DEF_EXT_INL quat quat_fromLookAtYX(const vec3* sourcePosition,const vec3* targetPosition);
NM_API_DEF_EXT_INL quat quat_FromEuler(nmoat fAngleX,nmoat fAngleY,nmoat fAngleZ,EulerRotationMode eulerRotationMode);
NM_API_DEF_EXT_INL int quat_areequaleps(const quat* a,const quat* b,nmoat eps);
NM_API_DEF_EXT_INL int quat_areequal(const quat* a,const quat* b);


#ifdef __cplusplus
NM_API_INL quat quat::slerp(const quat& o,nmoat slerpTime_In_0_1,bool normalizeQOutAfterLerp,nmoat eps) const   {return quat_slerp(this,&o,slerpTime_In_0_1,normalizeQOutAfterLerp?1:0,eps);}
NM_API_INL void quat::fromAngleAxis(nmoat rfAngle,const vec3& rkAxis)  {*this=quat_fromAngleAxis(rfAngle,&rkAxis);}
NM_API_INL void quat::toAngleAxis(nmoat& rfAngle,vec3& rkAxis) const   {quat_toAngleAxis(this,&rfAngle,&rkAxis);}
#endif //__cplusplus



// ---------------
// mat3 definition
// ---------------
#ifdef NM_NO_CPP_CTRS
#	ifndef NM_NO_C_INITIALIZATION
#   	define NM_RETURN_MAT3_CTR(m00,m10,m20,m01,m11,m21,m02,m12,m22)   return (mat3){{{m00,m10,m20,m01,m11,m21,m02,m12,m22}}};
#	else //NM_NO_C_INITIALIZATION
#		define NM_RETURN_MAT3_CTR(m00,m10,m20,m01,m11,m21,m02,m12,m22)	  {mat3 m;m.m[0]=m00;m.m[1]=m10;m.m[2]=m20;m.m[3]=m01;m.m[4]=m11;m.m[5]=m21;m.m[6]=m02;m.m[7]=m12;m.m[8]=m22;return m;}
#	endif //NM_NO_C_INITIALIZATION
#else
#   define NM_RETURN_MAT3_CTR(m00,m10,m20,m01,m11,m21,m02,m12,m22)   return mat3(m00,m10,m20,m01,m11,m21,m02,m12,m22);
#endif
NM_PRE_ALIGN_DEFAULT struct mat3 {
    union {
        nmoat m[9];         // column-major order: e.g. m[3],m[4],m[5] are the Y axis rotation part of the matrix
        nmoat col[3][3];	// e.g. col[1] is: col[1][0]=m[3]  col[1][1]=m[4]  col[1][2]=m[5]
        struct {nmoat m00,m10,m20,m01,m11,m21,m02,m12,m22;};  // m[r,c] == col[c][r]
/*      [source: https://stackoverflow.com/questions/146452/what-are-pod-types-in-c]
        POD stands for Plain Old Data - that is, a class (whether defined with the keyword struct or the keyword class)
        without constructors, destructors and virtual members functions.

        Wikipedia's article on POD [http://en.wikipedia.org/wiki/Plain_Old_Data_Structures]
        goes into a bit more detail and defines it as:
        A Plain Old Data Structure in C++ is an aggregate class that:
        -> contains only PODS as members
        -> has no user-defined destructor
        -> no user-defined copy assignment operator
        -> and no nonstatic members of pointer-to-member type.

        [also see: https://stackoverflow.com/questions/4178175/what-are-aggregates-and-pods-and-how-why-are-they-special/4178176#4178176]
*/
#       if (defined (NM_NO_CPP_CTRS) || defined(NM_COMPILER_SUPPORTS_NONPOD_ANONYMOUS_MEMBERS))    // otherwise: minimath.h: error: member ‘vec3 mat3::<unnamed union>::<unnamed struct>::xAxis’ with constructor (or with copy assignment operator) not allowed in anonymous aggregate
#       ifndef NM_NO_AXIS_VECTORS_IN_STRUCT_MAT3
        struct {vec3 xAxis,yAxis,zAxis;};
#       endif //NM_NO_AXIS_IN_STRUCT_MAT3
#       endif //NM_NO_CPP_CTRS || NM_COMPILER_SUPPORTS_NONPOD_ANONYMOUS_MEMBERS

        // mat3::print() displays:
        /*
                     xAxis                yAxis                zAxis
                       |                    |                    |
                       v                    v                    v
         |  m[0]==col[0][0]==m00  m[3]==col[1][0]==m01  m[6]==col[2][0]==m02  |
         |  m[1]==col[0][1]==m10  m[4]==col[1][1]==m11  m[7]==col[2][1]==m12  |
         |  m[2]==col[0][2]==m20  m[5]==col[1][2]==m21  m[8]==col[2][2]==m22  |

        */
    };

#	ifdef __cplusplus
#   ifndef NM_NO_CPP_CTRS
    NM_API_INL NM_API_EXPLICIT mat3(const nmoat* m9) {int t;for (t=0;t<9;t++) m[t]=m9[t];/*memcpy(m,m9,9*sizeof(nmoat));*/}
    NM_API_INL mat3(nmoat m00,nmoat m10, nmoat m20, nmoat m01, nmoat m11, nmoat m21, nmoat m02, nmoat m12, nmoat m22) {
        nmoat* p = m;
        *p++ = m00; *p++ = m10; *p++ = m20;
        *p++ = m01; *p++ = m11; *p++ = m21;
        *p++ = m02; *p++ = m12; *p++ = m22;
    }
    NM_API_INL mat3() {m[0]=m[4]=m[8]=(nmoat)1;m[1]=m[2]=m[3]=m[5]=m[6]=m[7]=(nmoat)0;}
    NM_API_INL mat3(nmoat d) {m[0]=m[4]=m[8]=d;m[1]=m[2]=m[3]=m[5]=m[6]=m[7]=(nmoat)0;}
    NM_API_INL mat3(const mat3& o) {int t;for (t=0;t<9;t++) m[t]=o.m[t];/**this=o;*//*memcpy(m,o.m,9*sizeof(nmoat));*/}
    NM_API_INL NM_API_EXPLICIT mat3(const quat& o) {setQuaternionToMatrixRotation(o);}
#   endif //NM_NO_CPP_CTRS

#   ifndef NM_NO_STDIO
    void fprintp(FILE* stream,int width, int precision) const;
    void fprint(FILE* stream) const;
    void printp(int width, int precision) const;
    void print() const;
#   endif //NM_NO_STDIO


    NM_API_INL const mat3& operator=(const mat3& o) {int t;for (t=0;t<9;t++) m[t]=o.m[t];/*memcpy(m,o.m,9*sizeof(nmoat));*/return *this;}
#   ifdef NM_GLM_MATRIX_SUBSCRIPT_OPERATOR
    NM_API_INL const nmoat* operator[](int i) const {NM_ASSERT(i>=0 && i<3);return col[i];}
    NM_API_INL nmoat* operator[](int i) {NM_ASSERT(i>=0 && i<3);return col[i];}
#   else //NM_GLM_MATRIX_SUBSCRIPT_OPERATOR
    NM_API_INL nmoat operator[](int i) const {NM_ASSERT(i>=0 && i<9);return m[i];}
    NM_API_INL nmoat& operator[](int i) {NM_ASSERT(i>=0 && i<9);return m[i];}
#   endif //NM_GLM_MATRIX_SUBSCRIPT_OPERATOR

    NM_API_INL mat3 operator*(const mat3& o) const  {mat3 res;nm_Mat3Mul_NoCheck(res.m,m,o.m);return res;}
    NM_API_INL const mat3& operator*=(const mat3& o) {*this = (*this)*o;return *this;}

    quat getQuaternionFromMatrixRotation() const;
    void setQuaternionToMatrixRotation(const quat& q);

    NM_API_INL mat3 transposed() const	{NM_RETURN_MAT3_CTR(m[0],m[3],m[6],m[1],m[4],m[7],m[2],m[5],m[8])}
    NM_API_INL void transpose() {*this=transposed();}
    NM_API_INL mat3 inverted() const	{mat3 o;nm_Mat3Invert(o.m,m);return o;}
    NM_API_INL int inverse() {return nm_Mat3InvertInPlace(m);}

    mat3 slerp(const mat3& o,nmoat slerpTime_In_0_1) const;

    NM_API_INL vec3 mulDir(nmoat dirX,nmoat dirY,nmoat dirZ) const {NM_RETURN_VEC3_CTR(dirX*m[0] + dirY*m[3] + dirZ*m[6], dirX*m[1] + dirY*m[4] + dirZ*m[7], dirX*m[2] + dirY*m[5] + dirZ*m[8])}
    NM_API_INL vec3 operator*(const vec3& o) const {NM_RETURN_VEC3_CTR(o.x*m[0] + o.y*m[3] + o.z*m[6], o.x*m[1] + o.y*m[4] + o.z*m[7], o.x*m[2] + o.y*m[5] + o.z*m[8])}

    NM_API_STATIC void FromEuler(mat3& m,nmoat x,nmoat y, nmoat z,EulerRotationMode mode);

#   endif //__cplusplus


} NM_POST_ALIGN_DEFAULT;


typedef struct mat3 mat3;
NM_API_DEF_INL mat3 make_mat3(nmoat m00,nmoat m10,nmoat m20,nmoat m01,nmoat m11,nmoat m21,nmoat m02,nmoat m12,nmoat m22) {NM_RETURN_MAT3_CTR(m00,m10,m20,m01,m11,m21,m02,m12,m22)}
NM_API_DEF_INL mat3 make_mat3_from_row_major(nmoat m00,nmoat m01,nmoat m02,nmoat m10,nmoat m11,nmoat m12,nmoat m20,nmoat m21,nmoat m22) {NM_RETURN_MAT3_CTR(m00,m10,m20,m01,m11,m21,m02,m12,m22)}
NM_API_DEF_INL mat3 mat3_identity() {NM_RETURN_MAT3_CTR(1,0,0,0,1,0,0,0,1)}


#ifndef NM_NO_STDIO
NM_API_DEF_EXT_INL void mat3_fprintp(const mat3* m,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void mat3_fprint(const mat3* m,FILE* stream);
NM_API_DEF_EXT_INL void mat3_printp(const mat3* m,int width, int precision);
NM_API_DEF_EXT_INL void mat3_print(const mat3* m);
#ifdef __cplusplus
NM_API_INL void mat3::fprintp(FILE* stream,int width, int precision) const {mat3_fprintp(this,stream,width,precision);}
NM_API_INL void mat3::fprint(FILE* stream) const   {mat3_fprint(this,stream);}
NM_API_INL void mat3::printp(int width, int precision) const {mat3_printp(this,width,precision);}
NM_API_INL void mat3::print() const    {mat3_print(this);}
#endif //__cplusplus
#endif //NM_NO_STDIO
//NM_API_DEF_EXT_INL mat3 mat3_copy(const quat* src);
NM_API_DEF_EXT_INL const mat3* mat3_mul(mat3* dst,const mat3* a,const mat3* b);
NM_API_DEF_EXT_INL const mat3* mat3_mul_nocheck(mat3* dst,const mat3* a,const mat3* b);
NM_API_DEF_EXT_INL const mat3* mat3_mul3(mat3* dst,const mat3* a,const mat3* b,const mat3* c);
NM_API_DEF_EXT_INL const mat3* mat3_mul3_nocheck(mat3* dst,const mat3* a,const mat3* b,const mat3* c);

NM_API_DEF_EXT_INL quat mat3_getQuaternionFromMatrixRotation(const mat3* src);
NM_API_DEF_EXT_INL void mat3_setQuaternionToMatrixRotation(mat3* dst,const quat* q);

NM_API_DEF_EXT_INL const mat3* mat3_transposed(mat3* dst,const mat3* src);
NM_API_DEF_EXT_INL void mat3_transpose(mat3* m);
NM_API_DEF_EXT_INL const mat3* mat3_inverted(mat3* dst,const mat3* src);
NM_API_DEF_EXT_INL int mat3_invert(mat3* m);

NM_API_DEF_EXT_INL vec3 mat3_muldir(const mat3* m,nmoat dirX,nmoat dirY,nmoat dirZ);
NM_API_DEF_EXT_INL vec3 mat3_muldirvec(const mat3* m,const vec3* o);

NM_API_DEF_EXT_INL void mat3_slerp(mat3* m,const mat3* mStart,const mat3* mEnd,nmoat slerpTime_In_0_1);
NM_API_DEF_EXT_INL void mat3_fromEulerXYZ(mat3* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat3_fromEulerXZY(mat3* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat3_fromEulerYXZ(mat3* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat3_fromEulerYZX(mat3* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat3_fromEulerZXY(mat3* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat3_fromEulerZYX(mat3* m,nmoat x,nmoat y, nmoat z);

NM_API_DEF_EXT_INL int mat3_toEulerXYZ(const mat3* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat3_toEulerXZY(const mat3* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat3_toEulerYXZ(const mat3* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat3_toEulerYZX(const mat3* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat3_toEulerZXY(const mat3* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat3_toEulerZYX(const mat3* m,nmoat* x,nmoat* y,nmoat* z);

NM_API_DEF_EXT_INL void mat3_fromEuler(mat3* m,nmoat x,nmoat y,nmoat z,EulerRotationMode mode);
NM_API_DEF_EXT_INL int mat3_toEuler(const mat3* m,nmoat* x,nmoat* y,nmoat* z,EulerRotationMode mode);

NM_API_DEF_EXT_INL int mat3_areequaleps(const mat3* a,const mat3* b,nmoat eps);
NM_API_DEF_EXT_INL int mat3_areequal(const mat3* a,const mat3* b);


#ifdef __cplusplus
NM_API_INL quat mat3::getQuaternionFromMatrixRotation() const  {return mat3_getQuaternionFromMatrixRotation(this);}
NM_API_INL void mat3::setQuaternionToMatrixRotation(const quat& q)  {mat3_setQuaternionToMatrixRotation(this,&q);}
NM_API_INL mat3 mat3::slerp(const mat3& o,nmoat slerpTime_In_0_1) const {mat3 rv;mat3_slerp(&rv,this,&o,slerpTime_In_0_1);return rv;}
NM_API_INL void mat3::FromEuler(mat3& m,nmoat x,nmoat y, nmoat z,EulerRotationMode mode) {return mat3_fromEuler(&m,x,y,z,mode);}
#endif //__cplusplus



// ---------------
// mat4 definition
// ---------------
#ifdef NM_NO_CPP_CTRS
#	ifndef NM_NO_C_INITIALIZATION
#   	define NM_RETURN_MAT4_CTR(m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,m32,m03,m13,m23,m33)   return (mat4){{{m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,m32,m03,m13,m23,m33}}};
#	else //NM_NO_C_INITIALIZATION
#		define NM_RETURN_MAT4_CTR(m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,m32,m03,m13,m23,m33)	  {mat4 m;m.m[0]=m00;m.m[1]=m10;m.m[2]=m20;m.m[3]=m30;  m.m[4]=m01;m.m[5]=m11;m.m[6]=m21;m.m[7]=m31;    m.m[8]=m02;m.m[9]=m12;m.m[10]=m22;m.m[11]=m32;    m.m[12]=m03;m.m[13]=m13;m.m[14]=m23;m.m[15]=m33;  return m;}
#	endif //NM_NO_C_INITIALIZATION
#else
#   define NM_RETURN_MAT4_CTR(m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,m32,m03,m13,m23,m33)   return mat4(m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,m32,m03,m13,m23,m33);
#endif
NM_PRE_ALIGN_DEFAULT struct mat4 {
    union {
        nmoat m[16];		// column-major order: e.g. m[12],m[13],m[14] are the translation part of the matrix
        nmoat col[4][4];	// e.g. col[3] is the translation part: col[3][0]=m[12]  col[3][1]=m[13]  col[3][2]=m[14]
        struct {nmoat m00,m10,m20,m30,  m01,m11,m21,m31,    m02,m12,m22,m32,    m03,m13,m23,m33;};  // m[r,c] == col[c][r]
#       if (defined(NM_NO_CPP_CTRS) || defined(NM_COMPILER_SUPPORTS_NONPOD_ANONYMOUS_MEMBERS))    // otherwise: minimath.h: error: member ‘vec3 mat4::<unnamed union>::<unnamed struct>::xAxis’ with constructor (or with copy operator) not allowed in anonymous aggregate
#           ifndef NM_NO_AXIS_VECTORS_IN_STRUCT_MAT4
#               ifndef NM_AXIS_VEC4_IN_STRUCT_MAT4
#                   ifndef NM_ALIGN_STRUCTS
        struct {vec3 xAxis;nmoat xAxisW;vec3 yAxis;nmoat yAxisW;vec3 zAxis;nmoat zAxisW;vec3 tra;nmoat traW;};
#                   else //NM_ALIGN_STRUCTS
        struct {vec3 xAxis,yAxis,zAxis,tra;};   // when vec3 is aligned: sizeof(vec3)=4*sizeof(nmoat)
#                   endif //NM_ALIGN_STRUCTS
#               else //NM_AXIS_VEC4_IN_STRUCT_MAT4
        struct {vec4 xAxis,yAxis,zAxis,tra;};
#               endif //NM_AXIS_VEC4_IN_STRUCT_MAT4
#           endif //NM_NO_AXIS_IN_STRUCT_MAT4
#       endif //NM_NO_CPP_CTRS || NM_COMPILER_SUPPORTS_NONPOD_ANONYMOUS_MEMBERS

        // mat4::print() displays something like:
        /*
                     xAxis                 yAxis                 zAxis                 trasl
                       |                     |                     |                     |
                       v                     v                     v                     v
         |  m[0]==col[0][0]==m00  m[4]==col[1][0]==m01  m[8] ==col[2][0]==m02  m[12]==col[3][0]==m03  |
         |  m[1]==col[0][1]==m10  m[5]==col[1][1]==m11  m[9] ==col[2][1]==m12  m[13]==col[3][1]==m13  |
         |  m[2]==col[0][2]==m20  m[6]==col[1][2]==m21  m[10]==col[2][2]==m22  m[14]==col[3][2]==m23  |
         |  ----------------------------------------------------------------------------------------  |
         |  m[3]==col[0][3]==m30  m[7]==col[1][3]==m31  m[11]==col[2][3]==m32  m[15]==col[3][3]==m33  |
        */
    };

#	ifdef __cplusplus
#   ifndef NM_NO_CPP_CTRS
    NM_API_INL NM_API_EXPLICIT mat4(const nmoat* m16) {int t;for (t=0;t<16;t++) m[t]=m16[t];/*memcpy(m,m16,16*sizeof(nmoat));*/}
    NM_API_INL mat4(nmoat m00,nmoat m10, nmoat m20, nmoat m30, nmoat m01, nmoat m11, nmoat m21, nmoat m31, nmoat m02, nmoat m12, nmoat m22, nmoat m32, nmoat m03, nmoat m13, nmoat m23, nmoat m33) {
        nmoat* p = m;
        *p++ = m00; *p++ = m10; *p++ = m20; *p++ = m30;
        *p++ = m01; *p++ = m11; *p++ = m21; *p++ = m31;
        *p++ = m02; *p++ = m12; *p++ = m22; *p++ = m32;
        *p++ = m03; *p++ = m13; *p++ = m23; *p++ = m33;
    }
    NM_API_INL mat4() {m[0]=m[5]=m[10]=m[15]=(nmoat)1;m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=(nmoat)0;}
    NM_API_INL mat4(nmoat d) {m[0]=m[5]=m[10]=m[15]=d;m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=(nmoat)0;}
    NM_API_INL mat4(const mat4& o) {int t;for (t=0;t<16;t++) m[t]=o.m[t];/**this=o;*//*memcpy(m,o.m,16*sizeof(nmoat));*/}
    NM_API_INL NM_API_EXPLICIT mat4(const quat& o) {setQuaternionToMatrixRotation(o);m[3]=m[7]=m[11]=m[12]=m[13]=m[14]=(nmoat)0;m[15]=(nmoat)1;}
    NM_API_INL NM_API_EXPLICIT mat4(const mat3& o) {setRotationFromMat3(o);m[3]=m[7]=m[11]=m[12]=m[13]=m[14]=(nmoat)0;m[15]=(nmoat)1;}
#   endif //NM_NO_CPP_CTRS

#   ifndef NM_NO_STDIO
    void fprintp(FILE* stream,int width, int precision) const;
    void fprint(FILE* stream) const;
    void printp(int width, int precision) const;
    void print() const;
#   endif //NM_NO_STDIO


    NM_API_INL const mat4 operator=(const mat4& o) {int t;for (t=0;t<16;t++) m[t]=o.m[t];/*memcpy(m,o.m,16*sizeof(nmoat));*/return *this;}
#   ifdef NM_GLM_MATRIX_SUBSCRIPT_OPERATOR
    NM_API_INL const nmoat* operator[](int i) const {NM_ASSERT(i>=0 && i<4);return col[i];}
    NM_API_INL nmoat* operator[](int i) {NM_ASSERT(i>=0 && i<4);return col[i];}
#   else //NM_GLM_MATRIX_SUBSCRIPT_OPERATOR
    NM_API_INL nmoat operator[](int i) const {NM_ASSERT(i>=0 && i<16);return m[i];}
    NM_API_INL nmoat& operator[](int i) {NM_ASSERT(i>=0 && i<16);return m[i];}
#   endif //NM_GLM_MATRIX_SUBSCRIPT_OPERATOR

    NM_API_INL mat4 operator*(const mat4& o) const  {mat4 res;nm_Mat4Mul_NoCheck(res.m,m,o.m);return res;}
    NM_API_INL const mat4& operator*=(const mat4& o) {*this = (*this)*o;return *this;}


    mat4& translate(nmoat x,nmoat y,nmoat z);
    mat4& rotate(nmoat degAngle,nmoat x,nmoat y,nmoat z);
    mat4& scale(nmoat x,nmoat y,nmoat z);

    quat getQuaternionFromMatrixRotation() const;
    void setQuaternionToMatrixRotation(const quat& q);
    NM_API_INL void getRotationToMat3(mat3& o) const {int i;for (i=0;i<3;i++) for (int j=0;j<3;j++) o.col[i][j]=col[i][j];}
    NM_API_INL void setRotationFromMat3(const mat3& m) {int i;for (i=0;i<3;i++) for (int j=0;j<3;j++) col[i][j]=m.col[i][j];}


    NM_API_INL mat4 transposed() const	{NM_RETURN_MAT4_CTR(m[0],m[4],m[8],m[12],m[1],m[5],m[9],m[13],m[2],m[6],m[10],m[14],m[3],m[7],m[11],m[15])}
    NM_API_INL void transpose() {*this=transposed();}

    mat4 inverted() const;
    int invert();

    mat4 invertedFast() const;
    void invertFast();

    void invertXZAxis();
    mat4 invertedXZAxis() const;
    //NM_API_INL vec3 mulDir(T dirX,nmoat dirY,nmoat dirZ) const {return vec3(dirX*m[0] + dirY*m[4] + dirZ*m[8], dirX*m[1] + dirY*m[5] + dirZ*m[9], dirX*m[2] + dirY*m[6] + dirZ*m[10]);}
    //vec3 mulPos(T posX,nmoat posY,nmoat posZ) const;

    NM_API_INL vec4 operator*(const vec4& o) const {NM_RETURN_VEC4_CTR(o.x*m[0] + o.y*m[4] + o.z*m[8] + o.w*m[12],o.x*m[1] + o.y*m[5] + o.z*m[9] + o.w*m[13],o.x*m[2] + o.y*m[6] + o.z*m[10]+ o.w*m[14],o.x*m[3] + o.y*m[7] + o.z*m[11]+ o.w*m[15])}

    mat4 slerp(const mat4& o,nmoat slerpTime_In_0_1) const;

    NM_API_INL vec3 mulDir(nmoat dirX,nmoat dirY,nmoat dirZ) const {NM_RETURN_VEC3_CTR(dirX*m[0] + dirY*m[3] + dirZ*m[6], dirX*m[1] + dirY*m[4] + dirZ*m[7], dirX*m[2] + dirY*m[5] + dirZ*m[8])}
    NM_API_INL vec3 operator*(const vec3& o) const {NM_RETURN_VEC3_CTR(o.x*m[0] + o.y*m[3] + o.z*m[6], o.x*m[1] + o.y*m[4] + o.z*m[7], o.x*m[2] + o.y*m[5] + o.z*m[8])}


#   endif //__cplusplus

} NM_POST_ALIGN_DEFAULT;

typedef struct mat4 mat4;
NM_API_DEF_INL mat4 make_mat4(nmoat m00,nmoat m10, nmoat m20, nmoat m30, nmoat m01, nmoat m11, nmoat m21, nmoat m31, nmoat m02, nmoat m12, nmoat m22, nmoat m32, nmoat m03, nmoat m13, nmoat m23, nmoat m33) {NM_RETURN_MAT4_CTR(m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,m32,m03,m13,m23,m33)}
NM_API_DEF_INL mat4 make_mat4_from_row_major(nmoat m00,nmoat m01,nmoat m02,nmoat m03,   nmoat m10,nmoat m11,nmoat m12,nmoat m13,    nmoat m20,nmoat m21,nmoat m22,nmoat m23,    nmoat m30,nmoat m31,nmoat m32,nmoat m33) {NM_RETURN_MAT4_CTR(m00,m10,m20,m30,m01,m11,m21,m31,m02,m12,m22,m32,m03,m13,m23,m33)}
NM_API_DEF_INL mat4 mat4_identity() {NM_RETURN_MAT4_CTR(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1)}


#ifndef NM_NO_STDIO
NM_API_DEF_EXT_INL void mat4_fprintp(const mat4* m,FILE* stream,int width, int precision);
NM_API_DEF_EXT_INL void mat4_fprint(const mat4* m,FILE* stream);
NM_API_DEF_EXT_INL void mat4_printp(const mat4* m,int width, int precision);
NM_API_DEF_EXT_INL void mat4_print(const mat4* m);
#ifdef __cplusplus
NM_API_INL void mat4::fprintp(FILE* stream,int width, int precision) const {mat4_fprintp(this,stream,width,precision);}
NM_API_INL void mat4::fprint(FILE* stream) const   {mat4_fprint(this,stream);}
NM_API_INL void mat4::printp(int width, int precision) const {mat4_printp(this,width,precision);}
NM_API_INL void mat4::print() const    {mat4_print(this);}
#endif //__cplusplus
#endif //NM_NO_STDIO
//NM_API_DEF_EXT_INL mat4 mat4_copy(const quat* src);
NM_API_DEF_EXT_INL const mat4* mat4_mul(mat4* dst,const mat4* a,const mat4* b);
NM_API_DEF_EXT_INL const mat4* mat4_mul_nocheck(mat4* dst,const mat4* a,const mat4* b);
NM_API_DEF_EXT_INL const mat4* mat4_mul3(mat4* dst,const mat4* a,const mat4* b,const mat4* c);
NM_API_DEF_EXT_INL const mat4* mat4_mul3_nocheck(mat4* dst,const mat4* a,const mat4* b,const mat4* c);

NM_API_DEF_EXT_INL quat mat4_getQuaternionFromMatrixRotation(const mat4* src);
NM_API_DEF_EXT_INL void mat4_setQuaternionToMatrixRotation(mat4* dst,const quat* q);
NM_API_DEF_EXT_INL const mat3* mat4_getMat3FromMatrixRotation(mat3* dst,const mat4* src);
NM_API_DEF_EXT_INL void mat4_setMat3ToMatrixRotation(mat4* dst,const mat3* src);


NM_API_DEF_EXT_INL const mat4* mat4_transposed(mat4* dst,const mat4* src);
NM_API_DEF_EXT_INL void mat4_transpose(mat4* m);
NM_API_DEF_EXT_INL const mat4* mat4_inverted(mat4* dst,const mat4* src);
NM_API_DEF_EXT_INL int mat4_invert(mat4* m);
NM_API_DEF_EXT_INL const mat4* mat4_invertedFast(mat4* dst,const mat4* src);
NM_API_DEF_EXT_INL void mat4_invertFast(mat4* m);
NM_API_DEF_EXT_INL const mat4* mat4_invertedXZAxis(mat4* dst,const mat4* src);
NM_API_DEF_EXT_INL void mat4_invertXZAxis(mat4* m);

NM_API_DEF_EXT_INL vec3 mat4_muldir(const mat4* m,nmoat dirX,nmoat dirY,nmoat dirZ);
NM_API_DEF_EXT_INL vec3 mat4_muldirvec(const mat4* m,const vec3* dir);

NM_API_DEF_EXT_INL vec4 mat4_mulpos(const mat4* m,nmoat posX,nmoat posY,nmoat posZ,nmoat posW/*=(nmoat)1*/);
NM_API_DEF_EXT_INL vec4 mat4_mulposvec(const mat4* m,const vec4* pos);

// A few static methods:
NM_API_DEF_EXT_INL const mat4* mat4_lookAt(mat4* m,nmoat eyeX,nmoat eyeY,nmoat eyeZ,nmoat centerX,nmoat centerY,nmoat centerZ,nmoat upX,nmoat upY,nmoat upZ);
NM_API_DEF_EXT_INL const mat4* mat4_perspective(mat4* m,nmoat degfovy,nmoat aspect, nmoat zNear, nmoat zFar);
NM_API_DEF_EXT_INL const mat4* mat4_ortho(mat4* m,nmoat left,nmoat right, nmoat bottom, nmoat top,nmoat nearVal,nmoat farVal);

NM_API_DEF_EXT_INL const mat4* mat4_translate(mat4* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL const mat4* mat4_rotate(mat4* m,nmoat degAngle,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL const mat4* mat4_scale(mat4* m,nmoat x,nmoat y, nmoat z);


NM_API_DEF_EXT_INL const mat4* mat4_fromRotScaTra(mat4* m,const mat3* rot,const vec3* sca,const vec3* tra);
NM_API_DEF_EXT_INL const mat4* mat4_lookAtYX(mat4* m,const vec3* lookAt,nmoat minDistance/*=0*/,nmoat maxDistance/*=0*/,nmoat pitchLimit/* = 1.483486111*/);

NM_API_DEF_EXT_INL const mat4* mat4_slerp(mat4* m,const mat4* mStart,const mat4* mEnd,nmoat slerpTime_In_0_1);

NM_API_DEF_EXT_INL void mat4_fromEulerXYZ(mat4* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat4_fromEulerXZY(mat4* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat4_fromEulerYXZ(mat4* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat4_fromEulerYZX(mat4* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat4_fromEulerZXY(mat4* m,nmoat x,nmoat y, nmoat z);
NM_API_DEF_EXT_INL void mat4_fromEulerZYX(mat4* m,nmoat x,nmoat y, nmoat z);

NM_API_DEF_EXT_INL int mat4_toEulerXYZ(const mat4* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat4_toEulerXZY(const mat4* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat4_toEulerYXZ(const mat4* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat4_toEulerYZX(const mat4* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat4_toEulerZXY(const mat4* m,nmoat* x,nmoat* y,nmoat* z);
NM_API_DEF_EXT_INL int mat4_toEulerZYX(const mat4* m,nmoat* x,nmoat* y,nmoat* z);

NM_API_DEF_EXT_INL void mat4_fromEuler(mat4* m,nmoat x,nmoat y,nmoat z,EulerRotationMode mode);
NM_API_DEF_EXT_INL int mat4_toEuler(const mat4* m,nmoat* x,nmoat* y,nmoat* z,EulerRotationMode mode);

NM_API_DEF_EXT_INL int mat4_areequaleps(const mat4* a,const mat4* b,nmoat eps);
NM_API_DEF_EXT_INL int mat4_areequal(const mat4* a,const mat4* b);

#ifdef __cplusplus
NM_API_INL quat mat4::getQuaternionFromMatrixRotation() const  {return mat4_getQuaternionFromMatrixRotation(this);}
NM_API_INL void mat4::setQuaternionToMatrixRotation(const quat& q)  {mat4_setQuaternionToMatrixRotation(this,&q);}
NM_API_INL mat4 mat4::slerp(const mat4& o,nmoat slerpTime_In_0_1) const {mat4 rv;mat4_slerp(&rv,this,&o,slerpTime_In_0_1);return rv;}
NM_API_INL mat4 mat4::inverted() const	{mat4 o;mat4_inverted(&o,this);return o;}
NM_API_INL int mat4::invert() {return mat4_invert(this);}
NM_API_INL mat4 mat4::invertedFast() const {mat4 o;mat4_invertedFast(&o,this);return o;}
NM_API_INL void mat4::invertFast() {mat4_invertedFast(this,this);}
NM_API_INL void mat4::invertXZAxis()  {mat4_invertXZAxis(this);}
NM_API_INL mat4 mat4::invertedXZAxis() const {mat4 o=*this;mat4_invertXZAxis(&o);return o;}
NM_API_INL mat4& mat4::translate(nmoat x,nmoat y,nmoat z) {nm_Mat4Translate(m,x,y,z);return *this;}
NM_API_INL mat4& mat4::rotate(nmoat degAngle,nmoat x,nmoat y,nmoat z) {nm_Mat4Rotate(m,degAngle,x,y,z);return *this;}
NM_API_INL mat4& mat4::scale(nmoat x,nmoat y,nmoat z) {nm_Mat4Scale(m,x,y,z);return *this;}
#endif //__cplusplus


// NM_STRUCT_SIZE_CHECK docs:
/*
                                            -----------------------------------------
                                            |                sizeof:                |
 * -----------------------------------------|---------------------------------------|
 * | NM_DOUBLE_PRECISION | NM_ALIGN_STRUCTS | vec2 | vec3 | vec4/quat | mat3 | mat4 |
 * |---------------------|------------------|------|------|-----------|------|------|
 * |         NO          |        NO        |  8   |  12  |    16     |  36  |  64  |
 * |---------------------|------------------|------|------|-----------|------|------|
 * |         NO          |   YES (16 bit)   |  16* |  16* |    16     |  48* |  64  |
 * |---------------------|------------------|------|------|-----------|------|------|
 * |         YES         |        NO        |  16  |  24  |    32     |  72  | 128  |
 * |---------------------|------------------|------|------|-----------|------|------|
 * |         YES         |   YES (32 bit)   |  32* |  32* |    32     |  96* | 128  |
 * ----------------------------------------------------------------------------------

 So what changes with NM_ALIGN_STRUCTS is something like:
 NO DOUBLE PRECISION -> sizeof(vec2)=sizeof(vec3):=16 and sizeof(mat3):=48
 DOUBLE PRECISION    -> sizeof(vec2)=sizeof(vec3):=32 and sizeof(mat3):=96
 This breaks code like this: vec3 v[4];int i;nmoat* p=v[0].e;for (i=0;i<12;i++) *p++=(nmoat)10;
*/
#ifndef NM_ALIGN_STRUCTS
#   ifndef NM_NAMESPACE
#       define NM_STRUCT_SIZE_CHECK NM_ASSERT(sizeof(vec2)==2*sizeof(nmoat) && sizeof(vec3)==3*sizeof(nmoat) && sizeof(vec4)==4*sizeof(nmoat) && sizeof(quat)==4*sizeof(nmoat) && sizeof(mat3)==9*sizeof(nmoat) && sizeof(mat4)==16*sizeof(nmoat))
#   else
#       define NM_STRUCT_SIZE_CHECK NM_ASSERT(sizeof(NM_NAMESPACE::vec2)==2*sizeof(nmoat) && sizeof(NM_NAMESPACE::vec3)==3*sizeof(nmoat) && sizeof(NM_NAMESPACE::vec4)==4*sizeof(nmoat) && sizeof(NM_NAMESPACE::quat)==4*sizeof(nmoat) && sizeof(NM_NAMESPACE::mat3)==9*sizeof(nmoat) && sizeof(NM_NAMESPACE::mat4)==16*sizeof(nmoat))
#   endif
#else //NM_ALIGN_STRUCTS
#   ifndef NM_NAMESPACE
#       define NM_STRUCT_SIZE_CHECK NM_ASSERT(sizeof(vec2)==4*sizeof(nmoat) && sizeof(vec3)==4*sizeof(nmoat) && sizeof(vec4)==4*sizeof(nmoat) && sizeof(quat)==4*sizeof(nmoat) && sizeof(mat3)==12*sizeof(nmoat) && sizeof(mat4)==16*sizeof(nmoat))
#   else
#       define NM_STRUCT_SIZE_CHECK NM_ASSERT(sizeof(NM_NAMESPACE::vec2)==4*sizeof(nmoat) && sizeof(NM_NAMESPACE::vec3)==4*sizeof(nmoat) && sizeof(NM_NAMESPACE::vec4)==4*sizeof(nmoat) && sizeof(NM_NAMESPACE::quat)==4*sizeof(nmoat) && sizeof(NM_NAMESPACE::mat3)==12*sizeof(nmoat) && sizeof(NM_NAMESPACE::mat4)==16*sizeof(nmoat))
#   endif
#endif //NM_ALIGN_STRUCTS

//==================================================================================================
#ifdef __cplusplus
#   ifndef NM_NO_GLM_COMPATIBILITY
NM_API_INL nmoat* value_ptr(vec2& o) {return &o.x;}
NM_API_INL const nmoat* value_ptr(const vec2& o) {return &o.x;}
NM_API_INL int isEqual(const vec2& a,const vec2& b,nmoat eps=NM_EPSILON) {return vec2_areequaleps(&a,&b,eps);}

NM_API_INL nmoat* value_ptr(vec3& o) {return &o.x;}
NM_API_INL const nmoat* value_ptr(const vec3& o) {return &o.x;}
NM_API_INL vec3 normalize(const vec3& o) {return o.normalized();}
NM_API_INL nmoat dot(const vec3& a,const vec3& b) {return a.dot(b);}
NM_API_INL vec3 cross(const vec3& a,const vec3& b) {return a.cross(b);}
NM_API_INL int isEqual(const vec3& a,const vec3& b,nmoat eps=NM_EPSILON) {return vec3_areequaleps(&a,&b,eps);}

NM_API_INL nmoat* value_ptr(vec4& o) {return &o.x;}
NM_API_INL const nmoat* value_ptr(const vec4& o) {return &o.x;}
NM_API_INL int isEqual(const vec4& a,const vec4& b,nmoat eps=NM_EPSILON) {return vec4_areequaleps(&a,&b,eps);}

NM_API_INL nmoat* value_ptr(quat& o) {return &o.x;}
NM_API_INL const nmoat* value_ptr(const quat& o) {return &o.x;}
NM_API_INL void quatSlerp(quat& output,nmoat slerpTime_In_0_1,const quat& qStart,const quat& qEnd,bool normalizeQOutAfterLerp=true,nmoat eps=(nmoat)0.0001) {output = qStart.slerp(qEnd,slerpTime_In_0_1,normalizeQOutAfterLerp,eps);}
NM_API_INL void quatLerp(quat& output,nmoat slerpTime_In_0_1,const quat& qStart,const quat& qEnd,bool normalizeQOutAfterLerp=true) {output = qStart.lerp(qEnd,slerpTime_In_0_1,normalizeQOutAfterLerp);}
NM_API_INL void fromAngleAxis(quat& output,nmoat radAngle,const vec3& axis) {return output.fromAngleAxis(radAngle,axis);}
NM_API_INL int isEqual(const quat& a,const quat& b,nmoat eps=NM_EPSILON) {return quat_areequaleps(&a,&b,eps);/*isEqual(a.x,b.x,eps) && isEqual(a.y,b.y,eps) && isEqual(a.z,b.z,eps) && isEqual(a.w,b.w,eps);*/}
NM_API_INL quat conjugate(const quat& q) {return q.conjugated();}
NM_API_INL quat inverse(const quat& q) {return q.inverted();}
NM_API_INL quat normalize(const quat& q) {return q.normalized();}

NM_API_INL nmoat* value_ptr(mat3& o) {return o.m;}
NM_API_INL const nmoat* value_ptr(const mat3& o) {return o.m;}
NM_API_INL mat3 transpose(const mat3& o) {return o.transposed();}
NM_API_INL mat3 inverse(const mat3& o) {return o.inverted();}
NM_API_INL mat3 mat3_cast(const quat& q) {
#   ifndef NM_NO_CPP_CTRS
    return mat3(q);
#   else
    mat3 m;m.setQuaternionToMatrixRotation(q);return m;
#   endif
}
NM_API_INL quat quat_cast(const mat3& o) {return o.getQuaternionFromMatrixRotation();}
NM_API_INL int isEqual(const mat3& a,const mat3& b,nmoat eps=NM_EPSILON) {return mat3_areequaleps(&a,&b,eps);}

NM_API_INL nmoat* value_ptr(mat4& o) {return o.m;}
NM_API_INL const nmoat* value_ptr(const mat4& o) {return o.m;}
NM_API_INL mat4 transpose(const mat4& o) {return o.transposed();}
NM_API_INL mat4 inverse(const mat4& o) {return o.inverted();}
NM_API_INL mat3 mat3_cast(const mat4& o) {mat3 tmp;o.getRotationToMat3(tmp);return tmp;}
NM_API_INL mat4 mat4_cast(const quat& q) {
#   ifndef NM_NO_CPP_CTRS
    return mat4(q);
#   else
    mat4 tmp;tmp.setQuaternionToMatrixRotation(q);return tmp;
#   endif
}
NM_API_INL quat quat_cast(const mat4& o) {return o.getQuaternionFromMatrixRotation();}
NM_API_INL mat4 perspective(nmoat radfovy,nmoat aspect, nmoat zNear, nmoat zFar) {mat4 m;mat4_perspective(&m,nmh_degrees(radfovy),aspect,zNear,zFar);return m;}
NM_API_INL mat4 ortho(nmoat left,nmoat right, nmoat bottom, nmoat top,nmoat nearVal,nmoat farVal) {mat4 m;mat4_ortho(&m,left,right,bottom,top,nearVal,farVal);return m;}
NM_API_INL mat4 lookAt(const vec3& eye,const vec3& center,const vec3& up) {mat4 m;mat4_lookAt(&m,eye.x,eye.y,eye.z,center.x,center.y,center.z,up.x,up.y,up.z);return m;}
NM_API_INL mat4 rotate(const mat4& m,nmoat radAngle,const vec3& axis) {mat4 o=m;o.rotate(nmh_degrees(radAngle),axis.x,axis.y,axis.z);return o;}
NM_API_INL mat4 translate(const mat4& m,const vec3& offset) {mat4 o=m;o.translate(offset.x,offset.y,offset.z);return o;}
NM_API_INL mat4 scale(const mat4& m,const vec3& scaling) {mat4 o=m;o.scale(scaling.x,scaling.y,scaling.z);return o;}
NM_API_INL void FromRotScaTra(mat4& output,const mat3& rot,const vec3& sca,const vec3& tra) {mat4_fromRotScaTra(&output,&rot,&sca,&tra);}
NM_API_INL int isEqual(const mat4& a,const mat4& b,nmoat eps=NM_EPSILON) {return mat4_areequaleps(&a,&b,eps);}
#   endif //NM_NO_GLM_COMPATIBILITY
#endif //__cplusplus
//========================================================================================================


#undef NM_RETURN_VEC2_CTR
#undef NM_RETURN_VEC3_CTR
#undef NM_RETURN_VEC4_CTR
#undef NM_RETURN_QUAT_CTR
#undef NM_RETURN_MAT3_CTR
#undef NM_RETURN_MAT4_CTR

#else   //NM_NO_MATH_STRUCTS
#   define NM_STRUCT_SIZE_CHECK /* no-op */
#endif //NM_NO_MATH_STRUCTS

#ifdef NM_NAMESPACE
}
#endif //NM_NAMESPACE


#ifdef NM_WARNING_SUPPRESSION
#   if (defined(__GNUC__) || defined(__MINGW__) || defined(__clang__))
#       pragma GCC diagnostic pop
#   elif defined(_MSC_VER)
#       pragma warning( pop )
#   endif
#endif // NM_WARNING_SUPPRESSION


#endif //MINIMATH_H_














//=====================================================================
//
// IMPLEMENTATION
//
//======================================================================

#ifdef MINIMATH_IMPLEMENTATION
#ifndef MINIMATH_IMPLEMENTATION_GUARD
#define MINIMATH_IMPLEMENTATION_GUARD

#ifndef NM_API_IMPL
#define NM_API_IMPL /* no-op */
#endif

#ifndef NM_API_PRIV
#ifdef __cplusplus
#define NM_API_PRIV /* no-op */
#else
#define NM_API_PRIV static
#endif
#endif

#ifndef NM_API_PRIV_INL
#define NM_API_PRIV_INL NM_API_PRIV NM_API_INL
#endif

#ifndef NM_API_PRIV_STATIC
#define NM_API_PRIV_STATIC NM_API_PRIV NM_API_STATIC
#endif

#ifndef NM_API_PRIV_STATIC_INL
#define NM_API_PRIV_STATIC_INL NM_API_PRIV_INL NM_API_STATIC
#endif



#ifndef NM_API_IMPL_EXT
#define NM_API_IMPL_EXT NM_API_IMPL NM_API_EXT
#endif

#ifdef NM_USE_MEMCPY
#   include <string.h> // memcpy
#   define NM_COPY_MACRO(NMOATP_DST,NMOATP_SRC,NUM_NMOATS_TO_COPY)  memcpy((NMOATP_DST),(NMOATP_SRC),(NUM_NMOATS_TO_COPY)*sizeof(nmoat))
#else //NM_USE_MEMCPY
#   define NM_COPY_MACRO(NMOATP_DST,NMOATP_SRC,NUM_NMOATS_TO_COPY)  {int i;for(i=0;i<(NUM_NMOATS_TO_COPY);i++) NMOATP_DST[i]=NMOATP_SRC[i];}
#endif //NM_USE_MEMCPY

#ifdef NM_WARNING_SUPPRESSION
#   if (defined(__GNUC__) || defined(__MINGW__) || defined(__clang__))
#       pragma GCC diagnostic push
#       pragma GCC diagnostic ignored "-Wmissing-braces"
#   elif defined(_MSC_VER)
#       pragma warning( push )
//#     pragma warning (disable: 4127)     // condition expression is constant
//#     pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#       pragme warning (disable: 4190)     // "-Wreturn-type-c-linkage"
#   endif
#endif // NM_WARNING_SUPPRESSION

#ifdef NM_USE_SIMD
#	if (!defined(__SSE__) && (defined(_MSC_VER) && defined(_M_IX86_FP) && _M_IX86_FP>0))
#		define __SSE__		// _MSC_VER does not always define it...
#	endif // __SSE__
#	ifndef NM_DOUBLE_PRECISION
#		ifdef __SSE__
#		include <xmmintrin.h>	// SSE
#		endif //__SSE__
#		ifdef __SSE3__
#		include <pmmintrin.h>	// SSE3 _mm_hadd_ps
#		endif //__SSE__
#		ifdef __SSE4_1__
#		include <smmintrin.h>	// SSE4.1 _mm_blendv_ps
#		endif

//====================================================================
#	define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))
    // vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#	define VecSwizzle(vec, x,y,z,w)           _mm_shuffle_ps(vec, vec, MakeShuffleMask(x,y,z,w))
#	define VecSwizzle1(vec, x)                _mm_shuffle_ps(vec, vec, MakeShuffleMask(x,x,x,x))
    // special swizzle
#	define VecSwizzle_0101(vec)               _mm_movelh_ps(vec, vec)
#	define VecSwizzle_2323(vec)               _mm_movehl_ps(vec, vec)
#	define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#	define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)
    // return (vec1[x], vec1[y], vec2[z], vec2[w])
#	define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
    // special shuffle
#	define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#	define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)
//---------------------------------------------------------------------
// for column major matrix
// we use __m128 to represent 2x2 matrix as A = | A0  A2 |
//                                              | A1  A3 |
// 2x2 column major Matrix multiply A*B
NM_API_PRIV_INL __m128 Mat2Mul(__m128 vec1, __m128 vec2)    {
    return
        _mm_add_ps(_mm_mul_ps(                     vec1, VecSwizzle(vec2, 0,0,3,3)),
                   _mm_mul_ps(VecSwizzle(vec1, 2,3,0,1), VecSwizzle(vec2, 1,1,2,2)));
}
// 2x2 column major Matrix adjugate multiply (A#)*B
NM_API_PRIV_INL __m128 Mat2AdjMul(__m128 vec1, __m128 vec2) {
    return
        _mm_sub_ps(_mm_mul_ps(VecSwizzle(vec1, 3,0,3,0), vec2),
                   _mm_mul_ps(VecSwizzle(vec1, 2,1,2,1), VecSwizzle(vec2, 1,0,3,2)));

}
// 2x2 column major Matrix multiply adjugate A*(B#)
NM_API_PRIV_INL __m128 Mat2MulAdj(__m128 vec1, __m128 vec2) {
    return
        _mm_sub_ps(_mm_mul_ps(                     vec1, VecSwizzle(vec2, 3,3,0,0)),
                   _mm_mul_ps(VecSwizzle(vec1, 2,3,0,1), VecSwizzle(vec2, 1,1,2,2)));
}
//=====================================================================

#	else // NM_DOUBLE_PRECISION
#		ifdef __AVX__
#		include <immintrin.h>	// AVX (and everything)
#		endif //__AVX__

//======================================================================
#	define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))
    // vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#	define VecSwizzle(vec, x,y,z,w)           _mm256_permute4x64_pd(vec,MakeShuffleMask(x,y,z,w))  // AVX2!	//_mm256_shuffle_pd(vec, vec, MakeShuffleMask(x,y,z,w))
/* For AVX1 fallback: one can perform an arbitrary permutation of __m256d in at most 4 instruction in the current generation of AVX: one VPERM2F128, two VSHUFPD, and one VBLENDPD. Incoming AVX2 in Haswell is much more capable, allowing for an arbitrary permutation in one instruction (VPERMPD I think).
*/
#	define VecSwizzle1(vec, x)                VecSwizzle(vec,x,x,x,x)
    // special swizzle
#	define VecSwizzle_0101(vec)               VecSwizzle(vec,0,1,0,1) // _mm256_movelh_pd(vec, vec)
#	define VecSwizzle_2323(vec)               VecSwizzle(vec,2,3,2,3) // _mm256_movehl_pd(vec, vec)
#	define VecSwizzle_0022(vec)               _mm256_moveldup_pd(vec)
#	define VecSwizzle_1133(vec)               _mm256_movehdup_pd(vec)
    // return (vec1[x], vec1[y], vec2[z], vec2[w])
//#	define VecShuffle(vec1, vec2, x,y,z,w)    _mm256_shuffle_pd(vec1, vec2, MakeShuffleMask(x,y,z,w))	// WRONG!
    // special shuffle
//#	define VecShuffle_0101(vec1, vec2)        VecShuffle(vec1, vec2, 0,1,0,1) // _mm256_movelh_pd(vec1, vec2)	// WRONG!
//#	define VecShuffle_2323(vec1, vec2)        VecShuffle(vec1, vec2, 2,3,2,3) // _mm256_movehl_pd(vec2, vec1)	// WRONG!
//----------------------------------------------------------------------
// for column major matrix
// we use __m256d to represent 2x2 matrix as A = | A0  A2 |
//                                              | A1  A3 |
// 2x2 column major Matrix multiply A*B
NM_API_PRIV_INL __m256d Mat2Mul(__m256d vec1, __m256d vec2)  {
    return
        _mm256_add_pd(_mm256_mul_pd(                     vec1, VecSwizzle(vec2, 0,0,3,3)),
                   _mm256_mul_pd(VecSwizzle(vec1, 2,3,0,1), VecSwizzle(vec2, 1,1,2,2)));
}
// 2x2 column major Matrix adjugate multiply (A#)*B
NM_API_PRIV_INL __m256d Mat2AdjMul(__m256d vec1, __m256d vec2)   {
    return
        _mm256_sub_pd(_mm256_mul_pd(VecSwizzle(vec1, 3,0,3,0), vec2),
                   _mm256_mul_pd(VecSwizzle(vec1, 2,1,2,1), VecSwizzle(vec2, 1,0,3,2)));

}
// 2x2 column major Matrix multiply adjugate A*(B#)
NM_API_PRIV_INL __m256d Mat2MulAdj(__m256d vec1, __m256d vec2)   {
    return
        _mm256_sub_pd(_mm256_mul_pd(                     vec1, VecSwizzle(vec2, 3,3,0,0)),
                   _mm256_mul_pd(VecSwizzle(vec1, 2,3,0,1), VecSwizzle(vec2, 1,1,2,2)));
}
//======================================================================

#	endif //NM_DOUBLE_PRECISION
#	ifndef NM_ALIGN_STRUCTS
#		define NM_MM_LOAD_PS(X) _mm_loadu_ps(X)
#		define NM_MM256_LOAD_PD(X) _mm256_loadu_pd(X)
#		define NM_MM_STORE_PS(X,Y) _mm_storeu_ps(X,Y)
#		define NM_MM256_STORE_PD(X,Y) _mm256_storeu_pd(X,Y)
#	else //NM_ALIGN_STRUCTS
#		define NM_MM_LOAD_PS(X) _mm_load_ps(X)
#		define NM_MM256_LOAD_PD(X) _mm256_load_pd(X)
#		define NM_MM_STORE_PS(X,Y) _mm_store_ps(X,Y)
#		define NM_MM256_STORE_PD(X,Y) _mm256_store_pd(X,Y)
#	endif //NM_ALIGN_STRUCTS
#endif //NM_USE_SIMD

#ifdef __cplusplus
extern "C"	{
#endif //__cplusplus


NM_API_IMPL nmoat nmh_radians(nmoat deg) {return M_DEG2RAD(deg);}
NM_API_IMPL nmoat nmh_degrees(nmoat rad) {return M_RAD2DEG(rad);}

#ifndef NM_DOUBLE_PRECISION
NM_API_IMPL nmoat nmh_sin(nmoat v)  {return sinf(v);}
NM_API_IMPL nmoat nmh_cos(nmoat v)  {return cosf(v);}
NM_API_IMPL void nmh_sincos(nmoat v,nmoat* NM_RESTRICT s,nmoat* NM_RESTRICT c)  {
#   if (defined(GNU_SOURCE) || defined(NM_HAS_SINCOS))
    return sincosf(v,s,c);
#   else
    *s=sinf(v);*c=cosf(v);
#   endif
}
NM_API_IMPL nmoat nmh_tan(nmoat v) {return tanf(v);}
NM_API_IMPL nmoat nmh_asin(nmoat v)   {return asinf(v);}
NM_API_IMPL nmoat nmh_acos(nmoat v)   {return acosf(v);}
NM_API_IMPL nmoat nmh_atan(nmoat v)   {return atanf(v);}
NM_API_IMPL nmoat nmh_atan2(nmoat y,nmoat x)  {return atan2f(y,x);}
NM_API_IMPL nmoat nmh_sqrt(nmoat v) {return sqrtf(v);}
NM_API_IMPL nmoat nmh_fabs(nmoat v) {return fabsf(v);}
NM_API_IMPL nmoat nmh_fmod(nmoat a,nmoat b) {return fmodf(a,b);}
NM_API_IMPL nmoat nmh_pow(nmoat x,nmoat y) {return powf(x,y);}
NM_API_IMPL nmoat nmh_round(nmoat v)    {return v < 0.0f ? ceilf(v - 0.5f) : floorf(v + 0.5f);}
NM_API_IMPL nmoat nmh_ceil(nmoat v) {return ceilf(v);}
NM_API_IMPL nmoat nmh_floor(nmoat v) {return floorf(v);}
#else //NM_DOUBLE_PRECISION
NM_API_IMPL nmoat nmh_sin(nmoat v)  {return sin(v);}
NM_API_IMPL nmoat nmh_cos(nmoat v)  {return cos(v);}
NM_API_IMPL void nmh_sincos(nmoat v,nmoat* NM_RESTRICT s,nmoat* NM_RESTRICT c)  {
#   if (defined(GNU_SOURCE) || defined(NM_HAS_SINCOS))
    return sincos(v,s,c);
#   else
    *s=sin(v);*c=cos(v);
#   endif
}
NM_API_IMPL nmoat nmh_tan(nmoat v) {return tan(v);}
NM_API_IMPL nmoat nmh_asin(nmoat v)   {return asin(v);}
NM_API_IMPL nmoat nmh_acos(nmoat v)   {return acos(v);}
NM_API_IMPL nmoat nmh_atan(nmoat v)   {return atan(v);}
NM_API_IMPL nmoat nmh_atan2(nmoat y,nmoat x)  {return atan2(y,x);}
NM_API_IMPL nmoat nmh_sqrt(nmoat v) {return sqrt(v);}
NM_API_IMPL nmoat nmh_fabs(nmoat v) {return fabs(v);}
NM_API_IMPL nmoat nmh_fmod(nmoat a,nmoat b) {return fmod(a,b);}
NM_API_IMPL nmoat nmh_pow(nmoat x,nmoat y) {return pow(x,y);}
NM_API_IMPL nmoat nmh_round(nmoat v)    {return v < 0.0 ? ceil(v - 0.5) : floor(v + 0.5);}
NM_API_IMPL nmoat nmh_ceil(nmoat v) {return ceil(v);}
NM_API_IMPL nmoat nmh_floor(nmoat v) {return floor(v);}
#endif // NM_DOUBLE_PRECISION
NM_API_IMPL nmoat nmh_fractional_part(nmoat v) {return nmh_fmod(v,(nmoat)1.0);}

// private functions here ---------------------------------------------------
#ifdef NEVER
NM_API_PRIV_INL int nm_priv_IsEqualEps(nmoat a,nmoat b,nmoat eps) {return nmh_fabs(a-b)<eps?1:0;}
NM_API_PRIV_INL int nm_priv_IsEqual(nmoat a,nmoat b) {return nmh_fabs(a-b)<NM_EPSILON?1:0;}

NM_API_PRIV_INL nmoat* nm_priv_Copy(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT src,const int num_nmoats) {int i;NM_ASSERT(num_nmoats>0);for(i=0;i<num_nmoats;i++) dst[i]=src[i];return dst;}
NM_API_PRIV_INL nmoat nm_priv_Dot(const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats) {nmoat r=a[0]*b[0];int i;NM_ASSERT(num_nmoats>0);for(i=1;i<num_nmoats;i++) r+=a[i]*b[i];return r;}
NM_API_PRIV_INL nmoat nm_priv_Distance2(const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats) {nmoat t=(a[0]-b[0]);nmoat r=t*t;int i;NM_ASSERT(num_nmoats>0);for(i=1;i<num_nmoats;i++) {t=a[i]-b[i];r+=t*t;}return r;}
NM_API_PRIV_INL nmoat nm_priv_Distance(const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats) {return nmh_sqrt(nm_priv_Distance2(a,b,num_nmoats));}
NM_API_PRIV_INL nmoat nm_priv_Length2(const nmoat* NM_RESTRICT v,const int num_nmoats) {nmoat t=v[0];nmoat r=t*t;int i;NM_ASSERT(num_nmoats>0);for(i=1;i<num_nmoats;i++) {t=v[i];r+=t*t;} return r;}
NM_API_PRIV_INL nmoat nm_priv_Length(const nmoat* NM_RESTRICT v,const int num_nmoats) {return nm_priv_Length2(v,num_nmoats);}
NM_API_PRIV_INL nmoat* nm_priv_Normalized(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT src,const int num_nmoats,const nmoat* NM_RESTRICT defaultValue) {
    int i;nmoat len=nm_priv_Length2(src,num_nmoats);
    if (len>NM_EPSILON) {len=nmh_sqrt(len);for (i=0;i<num_nmoats;i++) dst[i]=src[i]/len;}
    else if (defaultValue)  {for (i=0;i<num_nmoats;i++) dst[i]=defaultValue[i];}
    else {const nmoat dv = (nmoat)0;for (i=0;i<num_nmoats;i++) dst[i]=dv;}
    return dst;
}
NM_API_PRIV_INL void nm_priv_Normalize(nmoat* NM_RESTRICT v,const int num_nmoats,const nmoat* NM_RESTRICT defaultValue) {
    int i;nmoat len=nm_priv_Length2(v,num_nmoats);
    if (len>NM_EPSILON) {len=nmh_sqrt(len);for (i=0;i<num_nmoats;i++) v[i]/=len;}
    else if (defaultValue) {for (i=0;i<num_nmoats;i++) v[i]=defaultValue[i];}
    else  {const nmoat dv = (nmoat)0;for (i=0;i<num_nmoats;i++) v[i]=dv;}
}
NM_API_IMPL nmoat* nm_priv_Lerp(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats,nmoat t) {
    int i;nmoat ct = (nmoat)1-t;
    NM_ASSERT(num_nmoats>0);
    for (i=0;i<num_nmoats;i++) dst[i] = a[i]*ct+b[i]*t;
    return dst;
}
#endif //NEVER
NM_API_IMPL nmoat* nm_priv_VectorAdd(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=a[i]+b[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorSub(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=a[i]-b[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorMul(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=a[i]*b[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorDiv(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=a[i]/b[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorMulSca(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,nmoat s,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=a[i]*s;return dst;}
NM_API_IMPL nmoat* nm_priv_VectorDivSca(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,nmoat s,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=a[i]/s;return dst;}
NM_API_IMPL nmoat* nm_priv_VectorNeg(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=-a[i];return dst;}

NM_API_IMPL nmoat* nm_priv_VectorSAdd(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]+=a[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorSSub(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]-=a[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorSMul(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]*=a[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorSDiv(nmoat* NM_RESTRICT dst,const nmoat* NM_RESTRICT a,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]/=a[i];return dst;}
NM_API_IMPL nmoat* nm_priv_VectorSMulSca(nmoat* NM_RESTRICT dst,nmoat s,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]*=s;return dst;}
NM_API_IMPL nmoat* nm_priv_VectorSDivSca(nmoat* NM_RESTRICT dst,nmoat s,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]/=s;return dst;}
NM_API_IMPL nmoat* nm_priv_VectorSNeg(nmoat* NM_RESTRICT dst,const int num_nmoats) {int i;for (i=0;i<num_nmoats;i++) dst[i]=-dst[i];return dst;}
// end private functions ----------------------------------------------------

NM_API_IMPL int nm_AreEqualEps(const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,int num_nmoats,nmoat eps) {int i;NM_ASSERT(num_nmoats>0);for (i=0;i<num_nmoats;i++) {if (nmh_fabs(a[i]-b[i])>eps) return 0;} return 1;}
NM_API_IMPL int nm_AreEqual(const nmoat* NM_RESTRICT a,const nmoat* NM_RESTRICT b,int num_nmoats) {return nm_AreEqualEps(a,b,num_nmoats,NM_EPSILON);}


#ifndef NM_NO_STDIO
NM_API_IMPL void nm_Vec2_fprintp(const nmoat* NM_RESTRICT v2,FILE* stream,int width, int precision)  {
    int w = width, p = precision;
    NM_ASSERT(v2);
    fprintf(stream, "( %*.*f %*.*f )\n",w, p, v2[0], w, p, v2[1]);
}
NM_API_IMPL void nm_Vec2_fprint(const nmoat* NM_RESTRICT v2,FILE* stream) {nm_Vec2_fprintp(v2,stream, 6, 2);}
NM_API_IMPL void nm_Vec2_printp(const nmoat* NM_RESTRICT v2,int width, int precision)  {nm_Vec2_fprintp(v2,stdout, width, precision);}
NM_API_IMPL void nm_Vec2_print(const nmoat* NM_RESTRICT v2) {nm_Vec2_fprintp(v2,stdout, 6, 2);}

NM_API_IMPL void nm_Vec3_fprintp(const nmoat* NM_RESTRICT v3,FILE* stream,int width, int precision)  {
    int w = width, p = precision;
    NM_ASSERT(v3);
    fprintf(stream, "( %*.*f %*.*f %*.*f )\n",w, p, v3[0], w, p, v3[1], w, p, v3[2]);
}
NM_API_IMPL void nm_Vec3_fprint(const nmoat* NM_RESTRICT v3,FILE* stream) {nm_Vec3_fprintp(v3,stream, 6, 2);}
NM_API_IMPL void nm_Vec3_printp(const nmoat* NM_RESTRICT v3,int width, int precision)  {nm_Vec3_fprintp(v3,stdout, width, precision);}
NM_API_IMPL void nm_Vec3_print(const nmoat* NM_RESTRICT v3) {nm_Vec3_fprintp(v3,stdout, 6, 2);}

NM_API_IMPL void nm_Vec4_fprintp(const nmoat* NM_RESTRICT v4,FILE* stream,int width, int precision)  {
    int w = width, p = precision;
    NM_ASSERT(v4);
    fprintf(stream, "( %*.*f %*.*f %*.*f %*.*f )\n",w, p, v4[0], w, p, v4[1], w, p, v4[2], w, p, v4[3]);
}
NM_API_IMPL void nm_Vec4_fprint(const nmoat* NM_RESTRICT v4,FILE* stream) {nm_Vec4_fprintp(v4,stream, 6, 2);}
NM_API_IMPL void nm_Vec4_printp(const nmoat* NM_RESTRICT v4,int width, int precision)  {nm_Vec4_fprintp(v4,stdout, width, precision);}
NM_API_IMPL void nm_Vec4_print(const nmoat* NM_RESTRICT v4) {nm_Vec4_fprintp(v4,stdout, 6, 2);}

NM_API_IMPL void nm_Quat_fprintp(const nmoat* NM_RESTRICT q4,FILE* stream,int width, int precision)  {
    int w = width, p = precision;
    NM_ASSERT(q4);
    fprintf(stream, "( %*.*f; %*.*f %*.*f %*.*f )\n",w, p, q4[3], w, p, q4[0], w, p, q4[1], w, p, q4[2]);
}
NM_API_IMPL void nm_Quat_fprint(const nmoat* NM_RESTRICT q4,FILE* stream) {nm_Quat_fprintp(q4,stream, 6, 2);}
NM_API_IMPL void nm_Quat_printp(const nmoat* NM_RESTRICT q4,int width, int precision)  {nm_Quat_fprintp(q4,stdout, width, precision);}
NM_API_IMPL void nm_Quat_print(const nmoat* NM_RESTRICT q4) {nm_Quat_fprintp(q4,stdout, 6, 2);}

NM_API_IMPL void nm_Mat3_fprintp(const nmoat* NM_RESTRICT m9,FILE* stream,int width, int precision)  {
    int w = width, p = precision, r;
    NM_ASSERT(m9);
    for(r = 0; r < 3; r++) {
        fprintf(stream, "| %*.*f %*.*f %*.*f |\n",
            w, p, m9[r], w, p, m9[r+3], w, p, m9[r+6]
        );
    }
}
NM_API_IMPL void nm_Mat3_fprint(const nmoat* NM_RESTRICT m9,FILE* stream) {nm_Mat3_fprintp(m9,stream, 6, 2);}
NM_API_IMPL void nm_Mat3_printp(const nmoat* NM_RESTRICT m9,int width, int precision)  {nm_Mat3_fprintp(m9,stdout, width, precision);}
NM_API_IMPL void nm_Mat3_print(const nmoat* NM_RESTRICT m9) {nm_Mat3_fprintp(m9,stdout, 6, 2);}

NM_API_IMPL void nm_Mat4_fprintp(const nmoat* NM_RESTRICT m16,FILE* stream,int width, int precision)  {
    int w = width, p = precision, r;
    NM_ASSERT(m16);
    for(r = 0; r < 4; r++) {
        fprintf(stream, "| %*.*f %*.*f %*.*f %*.*f |\n",
            w, p, m16[r], w, p, m16[r+4], w, p, m16[r+8], w, p, m16[r+12]
        );
    }
}
NM_API_IMPL void nm_Mat4_fprint(const nmoat* NM_RESTRICT m16,FILE* stream) {nm_Mat4_fprintp(m16,stream, 6, 2);}
NM_API_IMPL void nm_Mat4_printp(const nmoat* NM_RESTRICT m16,int width, int precision)  {nm_Mat4_fprintp(m16,stdout, width, precision);}
NM_API_IMPL void nm_Mat4_print(const nmoat* NM_RESTRICT m16) {nm_Mat4_fprintp(m16,stdout, 6, 2);}
#endif //NM_NO_STDIO

NM_API_INL nmoat* nm_Vec2Copy(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT src2) {NM_COPY_MACRO(dst2,src2,2);return dst2;}
NM_API_INL nmoat* nm_Vec3Copy(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT src3) {NM_COPY_MACRO(dst3,src3,3);return dst3;}
NM_API_INL nmoat* nm_Vec4Copy(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT src4) {NM_COPY_MACRO(dst4,src4,4);return dst4;}
NM_API_INL nmoat* nm_QuatCopy(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT src4) {NM_COPY_MACRO(dst4,src4,4);return dst4;}
NM_API_INL nmoat* nm_Mat3Copy(nmoat* NM_RESTRICT dst9,const nmoat* NM_RESTRICT src9) {NM_COPY_MACRO(dst9,src9,9);return dst9;}
NM_API_INL nmoat* nm_Mat4Copy(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT src16) {NM_COPY_MACRO(dst16,src16,16);return dst16;}

NM_API_IMPL nmoat* nm_Vec2Add(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2) {return nm_priv_VectorAdd(dst2,a2,b2,2);}
NM_API_IMPL nmoat* nm_Vec2Sub(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2) {return nm_priv_VectorSub(dst2,a2,b2,2);}
NM_API_IMPL nmoat* nm_Vec2Mul(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2) {return nm_priv_VectorMul(dst2,a2,b2,2);}
NM_API_IMPL nmoat* nm_Vec2Div(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2) {return nm_priv_VectorDiv(dst2,a2,b2,2);}
NM_API_IMPL nmoat* nm_Vec2MulSca(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,nmoat s) {return nm_priv_VectorMulSca(dst2,a2,s,2);}
NM_API_IMPL nmoat* nm_Vec2DivSca(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2,nmoat s) {return nm_priv_VectorDivSca(dst2,a2,s,2);}
NM_API_IMPL nmoat* nm_Vec2Neg(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2) {return nm_priv_VectorNeg(dst2,a2,2);}
NM_API_IMPL nmoat* nm_Vec2SAdd(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2) {return nm_priv_VectorSAdd(dst2,a2,2);}
NM_API_IMPL nmoat* nm_Vec2SSub(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2) {return nm_priv_VectorSSub(dst2,a2,2);}
NM_API_IMPL nmoat* nm_Vec2SMul(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2) {return nm_priv_VectorSMul(dst2,a2,2);}
NM_API_IMPL nmoat* nm_Vec2SDiv(nmoat* NM_RESTRICT dst2,const nmoat* NM_RESTRICT a2) {return nm_priv_VectorSDiv(dst2,a2,2);}
NM_API_IMPL nmoat* nm_Vec2SMulSca(nmoat* NM_RESTRICT dst2,nmoat s) {return nm_priv_VectorSMulSca(dst2,s,2);}
NM_API_IMPL nmoat* nm_Vec2SDivSca(nmoat* NM_RESTRICT dst2,nmoat s) {return nm_priv_VectorSDivSca(dst2,s,2);}
NM_API_IMPL nmoat* nm_Vec2SNeg(nmoat* NM_RESTRICT dst2) {return nm_priv_VectorSNeg(dst2,2);}
NM_API_IMPL nmoat nm_Vec2Length2(const nmoat* NM_RESTRICT a2) {return a2[0]*a2[0] + a2[1]*a2[1];}
NM_API_IMPL nmoat nm_Vec2Length(const nmoat* NM_RESTRICT a2) {const nmoat res = nm_Vec2Length2(a2);return res!=0 ? nmh_sqrt(res) : 0;}
NM_API_IMPL void nm_Vec2Normalize(nmoat* NM_RESTRICT v2) {
    nmoat len = nm_Vec2Length2(v2);int i;
    if (len>NM_EPSILON) {len = nmh_sqrt(len);for (i=0;i<2;i++) v2[i]/=len;}
    else {v2[0]=(nmoat)0;v2[1]=(nmoat)1;}
}
NM_API_IMPL nmoat* nm_Vec2Normalized(nmoat* NM_RESTRICT vOut2,const nmoat* NM_RESTRICT v2) {
    nmoat len = nm_Vec2Length2(v2);int i;
    if (len>NM_EPSILON) {len = nmh_sqrt(len);for (i=0;i<2;i++) vOut2[i]=v2[i]/len;}
    else {vOut2[0]=(nmoat)0;vOut2[1]=(nmoat)1;}
    return vOut2;
}
NM_API_IMPL nmoat nm_Vec2DistSquared(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2) {
    const nmoat rv[2] = {b2[0]-a2[0],b2[1]-a2[1]};
    return rv[0]*rv[0] + rv[1]*rv[1];
}
NM_API_IMPL nmoat nm_Vec2Dist(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2) {
    const nmoat res = nm_Vec2DistSquared(a2,b2);
    return res!=0 ? nmh_sqrt(res) : 0;
}

NM_API_IMPL nmoat* nm_Vec3Add(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3) {return nm_priv_VectorAdd(dst3,a3,b3,3);}
NM_API_IMPL nmoat* nm_Vec3Sub(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3) {return nm_priv_VectorSub(dst3,a3,b3,3);}
NM_API_IMPL nmoat* nm_Vec3Mul(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3) {return nm_priv_VectorMul(dst3,a3,b3,3);}
NM_API_IMPL nmoat* nm_Vec3Div(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3) {return nm_priv_VectorDiv(dst3,a3,b3,3);}
NM_API_IMPL nmoat* nm_Vec3MulSca(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,nmoat s) {return nm_priv_VectorMulSca(dst3,a3,s,3);}
NM_API_IMPL nmoat* nm_Vec3DivSca(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3,nmoat s) {return nm_priv_VectorDivSca(dst3,a3,s,3);}
NM_API_IMPL nmoat* nm_Vec3Neg(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3) {return nm_priv_VectorNeg(dst3,a3,3);}
NM_API_IMPL nmoat* nm_Vec3SAdd(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3) {return nm_priv_VectorSAdd(dst3,a3,3);}
NM_API_IMPL nmoat* nm_Vec3SSub(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3) {return nm_priv_VectorSSub(dst3,a3,3);}
NM_API_IMPL nmoat* nm_Vec3SMul(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3) {return nm_priv_VectorSMul(dst3,a3,3);}
NM_API_IMPL nmoat* nm_Vec3SDiv(nmoat* NM_RESTRICT dst3,const nmoat* NM_RESTRICT a3) {return nm_priv_VectorSDiv(dst3,a3,3);}
NM_API_IMPL nmoat* nm_Vec3SMulSca(nmoat* NM_RESTRICT dst3,nmoat s) {return nm_priv_VectorSMulSca(dst3,s,3);}
NM_API_IMPL nmoat* nm_Vec3SDivSca(nmoat* NM_RESTRICT dst3,nmoat s) {return nm_priv_VectorSDivSca(dst3,s,3);}
NM_API_IMPL nmoat* nm_Vec3SNeg(nmoat* NM_RESTRICT dst3) {return nm_priv_VectorSNeg(dst3,3);}
NM_API_IMPL nmoat nm_Vec3Dot(const nmoat* a3,const nmoat* b3) {return a3[0]*b3[0]+a3[1]*b3[1]+a3[2]*b3[2];}
NM_API_IMPL nmoat* nm_Vec3Cross(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3) {
    vOut3[0] =	a3[1] * b3[2] - a3[2] * b3[1];
    vOut3[1] =	a3[2] * b3[0] - a3[0] * b3[2];
    vOut3[2] =	a3[0] * b3[1] - a3[1] * b3[0];
    return vOut3;
}
NM_API_IMPL void nm_Vec3Normalize(nmoat* NM_RESTRICT v3) {
    nmoat len = nm_Vec3Dot(v3,v3);int i;
    if (len>NM_EPSILON) {len = nmh_sqrt(len);for (i=0;i<3;i++) v3[i]/=len;}
    else {v3[0]=v3[2]=(nmoat)0;v3[1]=(nmoat)1;}
}
NM_API_IMPL nmoat* nm_Vec3Normalized(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT v3) {
    nmoat len = nm_Vec3Dot(v3,v3);int i;
    if (len>NM_EPSILON) {len = nmh_sqrt(len);for (i=0;i<3;i++) vOut3[i]=v3[i]/len;}
    else {vOut3[0]=vOut3[2]=(nmoat)0;vOut3[1]=(nmoat)1;}
    return vOut3;
}
NM_API_IMPL nmoat nm_Vec3DistSquared(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3) {
    const nmoat rv[3] = {b3[0]-a3[0],b3[1]-a3[1],b3[2]-a3[2]};
    return rv[0]*rv[0] + rv[1]*rv[1] + rv[2]*rv[2];
}
NM_API_IMPL nmoat nm_Vec3Dist(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3) {
    const nmoat res = nm_Vec3DistSquared(a3,b3);
    return res!=0 ? nmh_sqrt(res) : 0;
}
NM_API_IMPL nmoat nm_Vec3Length2(const nmoat* NM_RESTRICT a3) {return a3[0]*a3[0] + a3[1]*a3[1] + a3[2]*a3[2];}
NM_API_IMPL nmoat nm_Vec3Length(const nmoat* NM_RESTRICT a3) {const nmoat res = nm_Vec3Length2(a3);return res!=0 ? nmh_sqrt(res) : 0;}


NM_API_IMPL nmoat* nm_Vec4Add(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4) {return nm_priv_VectorAdd(dst4,a4,b4,4);}
NM_API_IMPL nmoat* nm_Vec4Sub(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4) {return nm_priv_VectorSub(dst4,a4,b4,4);}
NM_API_IMPL nmoat* nm_Vec4Mul(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4) {return nm_priv_VectorMul(dst4,a4,b4,4);}
NM_API_IMPL nmoat* nm_Vec4Div(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4) {return nm_priv_VectorDiv(dst4,a4,b4,4);}
NM_API_IMPL nmoat* nm_Vec4MulSca(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,nmoat s) {return nm_priv_VectorMulSca(dst4,a4,s,4);}
NM_API_IMPL nmoat* nm_Vec4DivSca(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4,nmoat s) {return nm_priv_VectorDivSca(dst4,a4,s,4);}
NM_API_IMPL nmoat* nm_Vec4Neg(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4) {return nm_priv_VectorNeg(dst4,a4,4);}
NM_API_IMPL nmoat* nm_Vec4SAdd(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4) {return nm_priv_VectorSAdd(dst4,a4,4);}
NM_API_IMPL nmoat* nm_Vec4SSub(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4) {return nm_priv_VectorSSub(dst4,a4,4);}
NM_API_IMPL nmoat* nm_Vec4SMul(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4) {return nm_priv_VectorSMul(dst4,a4,4);}
NM_API_IMPL nmoat* nm_Vec4SDiv(nmoat* NM_RESTRICT dst4,const nmoat* NM_RESTRICT a4) {return nm_priv_VectorSDiv(dst4,a4,4);}
NM_API_IMPL nmoat* nm_Vec4SMulSca(nmoat* NM_RESTRICT dst4,nmoat s) {return nm_priv_VectorSMulSca(dst4,s,4);}
NM_API_IMPL nmoat* nm_Vec4SDivSca(nmoat* NM_RESTRICT dst4,nmoat s) {return nm_priv_VectorSDivSca(dst4,s,4);}
NM_API_IMPL nmoat* nm_Vec4SNeg(nmoat* NM_RESTRICT dst4) {return nm_priv_VectorSNeg(dst4,4);}
NM_API_IMPL nmoat nm_Vec4Length2(const nmoat* NM_RESTRICT a4) {return a4[0]*a4[0] + a4[1]*a4[1] + a4[2]*a4[2] + a4[3]*a4[3];}
NM_API_IMPL nmoat nm_Vec4Length(const nmoat* NM_RESTRICT a4) {const nmoat res = nm_Vec4Length2(a4);return res!=0 ? nmh_sqrt(res) : 0;}
NM_API_IMPL void nm_Vec4Normalize(nmoat* NM_RESTRICT v4) {
    nmoat len = nm_Vec4Length2(v4);int i;
    if (len>NM_EPSILON) {len = nmh_sqrt(len);for (i=0;i<4;i++) v4[i]/=len;}
    else {v4[0]=v4[2]=v4[3]=(nmoat)0;v4[1]=(nmoat)1;}
}
NM_API_IMPL nmoat* nm_Vec4Normalized(nmoat* NM_RESTRICT vOut4,const nmoat* NM_RESTRICT v4) {
    nmoat len = nm_Vec4Length2(v4);int i;
    if (len>NM_EPSILON) {len = nmh_sqrt(len);for (i=0;i<4;i++) vOut4[i]=v4[i]/len;}
    else {vOut4[0]=vOut4[2]=vOut4[3]=(nmoat)0;vOut4[1]=(nmoat)1;}
    return vOut4;
}
NM_API_IMPL nmoat nm_Vec4DistSquared(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4) {
    const nmoat rv[4] = {b4[0]-a4[0],b4[1]-a4[1],b4[2]-a4[2],b4[3]-a4[3]};
    return rv[0]*rv[0] + rv[1]*rv[1] + rv[2]*rv[2] + rv[3]*rv[3];
}
NM_API_IMPL nmoat nm_Vec4Dist(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4) {
    const nmoat res = nm_Vec4DistSquared(a4,b4);
    return res!=0 ? nmh_sqrt(res) : 0;
}


NM_API_IMPL nmoat nm_QuatLength2(const nmoat* NM_RESTRICT q4) {return q4[0]*q4[0]+q4[1]*q4[1]+q4[2]*q4[2]+q4[3]*q4[3];}
NM_API_IMPL nmoat nm_QuatLength(const nmoat* NM_RESTRICT q4) {return nmh_sqrt(q4[0]*q4[0]+q4[1]*q4[1]+q4[2]*q4[2]+q4[3]*q4[3]);}
NM_API_IMPL nmoat* nm_QuatNormalized(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT q4) {nmoat *r=result4;const nmoat *q=q4;const nmoat len=nm_QuatLength(q4);if (len>0) {r[0]=q[0]/len;r[1]=q[1]/len;r[2]=q[2]/len;r[3]=q[3]/len;} else {r[3]=(nmoat)1;r[0]=r[1]=r[2]=(nmoat)0;} return r;}
NM_API_IMPL void nm_QuatNormalize(nmoat* NM_RESTRICT q4) {const nmoat len=nm_QuatLength(q4);if (len>0) {q4[0]/=len;q4[1]/=len;q4[2]/=len;q4[3]/=len;} else {q4[0]=q4[1]=q4[2]=q4[3]=(nmoat)0;}}
NM_API_IMPL nmoat nm_QuatDot(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4)  {return a4[0]*b4[0]+a4[1]*b4[1]+a4[2]*b4[2]+a4[3]*b4[3];}
NM_API_IMPL nmoat* nm_QuatNegated(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT src4)  {int i;for (i=0;i<4;i++) qOut4[i]=-src4[i];return qOut4;}
NM_API_IMPL void nm_QuatNegate(nmoat* NM_RESTRICT q4)  {int i;for (i=0;i<4;i++) q4[i]=-q4[i];}
NM_API_IMPL nmoat* nm_QuatMul(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4)  {
#	if (defined(NM_USE_SIMD) && !defined(NM_DOUBLE_PRECISION) && defined(__SSE__))
    __m128 xyzw = NM_MM_LOAD_PS(a4);
    __m128 abcd = NM_MM_LOAD_PS(b4);

    __m128 wzyx = _mm_shuffle_ps(xyzw, xyzw, _MM_SHUFFLE(0,1,2,3));
    __m128 baba = _mm_shuffle_ps(abcd, abcd, _MM_SHUFFLE(0,1,0,1));
    __m128 dcdc = _mm_shuffle_ps(abcd, abcd, _MM_SHUFFLE(2,3,2,3));

    /* variable names below are for parts of componens of result (X,Y,Z,W) */
    /* nX stands for -X and similarly for the other components             */

    /* znxwy  = (xb - ya, zb - wa, wd - zc, yd - xc) */
    __m128 ZnXWY = _mm_hsub_ps(_mm_mul_ps(xyzw, baba), _mm_mul_ps(wzyx, dcdc));

    /* xzynw  = (xd + yc, zd + wc, wb + za, yb + xa) */
    __m128 XZYnW = _mm_hadd_ps(_mm_mul_ps(xyzw, dcdc), _mm_mul_ps(wzyx, baba));

    /* _mm_shuffle_ps(XZYnW, ZnXWY, _MM_SHUFFLE(3,2,1,0)) */
    /*      = (xd + yc, zd + wc, wd - zc, yd - xc)        */
    /* _mm_shuffle_ps(ZnXWY, XZYnW, _MM_SHUFFLE(2,3,0,1)) */
    /*      = (zb - wa, xb - ya, yb + xa, wb + za)        */

    /* _mm_addsub_ps adds elements 1 and 3 and subtracts elements 0 and 2, so we get: */
    /* _mm_addsub_ps(*, *) = (xd+yc-zb+wa, xb-ya+zd+wc, wd-zc+yb+xa, yd-xc+wb+za)     */

    __m128 XZWY = _mm_addsub_ps(_mm_shuffle_ps(XZYnW, ZnXWY, _MM_SHUFFLE(3,2,1,0)),
                                _mm_shuffle_ps(ZnXWY, XZYnW, _MM_SHUFFLE(2,3,0,1)));

    /* now we only need to shuffle the components in place and return the result      */
    NM_MM_STORE_PS(qOut4,_mm_shuffle_ps(XZWY, XZWY, _MM_SHUFFLE(2,1,3,0)));
#   else //NM_USE_SIMD
    /* reference implementation */
    const nmoat x = a4[0],y = a4[1],z = a4[2], w = a4[3];
    const nmoat a = b4[0],b = b4[1],c = b4[2], d = b4[3];
    qOut4[0] =  x*d + y*c - z*b + w*a;
    qOut4[1] = -x*c + y*d + z*a + w*b;
    qOut4[2] =  x*b - y*a + z*d + w*c;
    qOut4[3] = -x*a - y*b - z*c + w*d;
#   endif //NM_USE_SIMD
    return qOut4;
    /*nmoat* q=qOut4;const nmoat *a=a4,*b=b4;
    q[0] = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];
    q[1] = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1];
    q[2] = a[3] * b[1] + a[1] * b[3] + a[2] * b[0] - a[0] * b[2];
    q[3] = a[3] * b[2] + a[2] * b[3] + a[0] * b[1] - a[1] * b[0];
    return qOut4;*/
}
NM_API_IMPL nmoat* nm_QuatMul3Quats(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,const nmoat* NM_RESTRICT c4)  {nmoat tmp4[4];return nm_QuatMul(qOut4,nm_QuatMul(tmp4,a4,b4),c4);}
NM_API_IMPL nmoat* nm_QuatConjugated(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT q4) {qOut4[3]=q4[3];qOut4[0]=-q4[0];qOut4[1]=-q4[1];qOut4[2]=-q4[2];return qOut4;}
NM_API_IMPL void nm_QuatConjugate(nmoat* NM_RESTRICT q4)    {q4[0]=-q4[0];q4[1]=-q4[1];q4[2]=-q4[2];}
NM_API_IMPL nmoat* nm_QuatInverted(nmoat* NM_RESTRICT qOut4,const nmoat* NM_RESTRICT q4)   {nmoat d = -nm_QuatDot(q4,q4);qOut4[3]=-q4[3]/d;qOut4[0]=q4[0]/d;qOut4[1]=q4[1]/d;qOut4[2]=q4[2]/d;return qOut4;}
NM_API_IMPL void nm_QuatInverse(nmoat* NM_RESTRICT q4)  {nmoat d = -nm_QuatDot(q4,q4);q4[3]/=-d;q4[0]/=d;q4[1]/=d;q4[2]/=d;}
NM_API_IMPL nmoat* nm_QuatMulVec3(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT q4,const nmoat* NM_RESTRICT vIn3)   {
//#define NM_QUAT_MULVEC_OPT
#   ifndef NM_QUAT_MULVEC_OPT
    nmoat uv[3],uuv[3];int i;
    nm_Vec3Cross(uuv,q4,nm_Vec3Cross(uv,q4,vIn3));
    for (i=0;i<3;i++) vOut3[i] = vIn3[i] + ((uv[i] * q4[3]) + uuv[i]) * (nmoat)2;
    return vOut3;
#   else //NM_QUAT_MULVEC_OPT
    /*ROTATING A VECTOR BY AN UNIT QUATERNION (FASTER):
    =================================================
    t:=cross(2q(x,y,z),v);
    v2 = (q*v*q`)(x,y,z) = v+q(w)*t + cross(q(x,y,z),t);
    [But still doing a 3x3 matrix * v is faster]*/
    const nmoat qv[3] = {(nmoat)2*q4[0],(nmoat)2*q4[1],(nmoat)2*q4[2]};int i;
    nmoat qt[3]; nm_Vec3Cross(qt,q4,nm_Vec3Cross(vOut3,qv,vIn3));
    for (i=0;i<3;i++)   {vOut3[i]*=q4[3];vOut3[i]+=vIn3[i]+qt[i];}
    return vOut3;
#   endif  //NM_QUAT_MULVEC_OPT
}
NM_API_IMPL nmoat* nm_QuatMulVec3Inv(nmoat* NM_RESTRICT vOut3,const nmoat* NM_RESTRICT q4,const nmoat* NM_RESTRICT vIn3)    {
    nmoat qinv[4];nm_QuatInverted(qinv,q4);
    return nm_QuatMulVec3(vOut3,qinv,vIn3);
}
NM_API_IMPL nmoat* nm_QuatFromAngleAxis(nmoat* NM_RESTRICT qOut4,nmoat rfAngle,nmoat rkAxisX,nmoat rkAxisY,nmoat rkAxisZ)   {
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
    nmoat fSin,fCos;nmh_sincos((nmoat)(0.5)*rfAngle,&fSin,&fCos);
    qOut4[3]=fCos; qOut4[0]=fSin*rkAxisX; qOut4[1]=fSin*rkAxisY; qOut4[2]=fSin*rkAxisZ;
    return qOut4;
}
NM_API_IMPL nmoat* nm_QuatFromAngleAxisVec3(nmoat* NM_RESTRICT qOut4,nmoat rfAngle,const nmoat* NM_RESTRICT rkAxis3)   {return nm_QuatFromAngleAxis(qOut4,rfAngle,rkAxis3[0],rkAxis3[1],rkAxis3[2]);}
NM_API_IMPL void nm_QuatToAngleAxis(const nmoat* NM_RESTRICT q4,nmoat* NM_RESTRICT rfAngleOut1,nmoat* NM_RESTRICT rkAxisOut3)  {
    const nmoat* q=q4;
    // These both seem to work.
    // Implementation 1
            // The quaternion representing the rotation is
            //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

            nmoat fSqrLength = q[0]*q[0]+q[1]*q[1]+q[2]*q[2];
            if (fSqrLength > (nmoat)0)  {
                nmoat fInvLength;*rfAngleOut1 = (nmoat)2*nmh_acos(q[3]);fInvLength = (nmoat)1/nmh_sqrt(fSqrLength);
                rkAxisOut3[0] = q[0]*fInvLength;rkAxisOut3[1] = q[1]*fInvLength;rkAxisOut3[2] = q[2]*fInvLength;
            }
            else    {
                // angle is 0 (mod 2*pi), so any axis will do
                *rfAngleOut1 = rkAxisOut3[0] = rkAxisOut3[2] = (nmoat)0;
                rkAxisOut3[1] = (nmoat)1;
            }
    /*// Implementation 2
        // more based on the glm library code:
        nmoat tmp1 = (nmoat)1 - q[3]*q[3];
        *rfAngleOut1 = nmh_acos(q[3]) * (nmoat)2;
        if (tmp1 <= (nmoat)0)   {rkAxisOut3[0]=rkAxisOut3[1]=(nmoat)0;rkAxisOut3[2]=(nmoat)1;}
        else    {
            nmoat tmp2 = (nmoat)1 / nmh_sqrt(tmp1);
            rkAxisOut[0]=q[0]*tmp2; rkAxisOut[1]=q[1]*tmp2; rkAxisOut[2]=q[2]*tmp2;
        }*/
}
NM_API_IMPL nmoat* nm_QuatFromLookAtYX(nmoat* NM_RESTRICT qOut4,nmoat sourcePosX,nmoat sourcePosY,nmoat sourcePosZ,nmoat targetPosX,nmoat targetPosY,nmoat targetPosZ)   {
    nmoat D[3] = {targetPosX-sourcePosX,targetPosY-sourcePosY,targetPosZ-sourcePosZ};
    nmoat Dxz2 = D[0]*D[0]+D[2]*D[2];
    nmoat Dxz = nmh_sqrt(Dxz2);
    nmoat AY = nmh_atan2(D[0],D[2]);
    nmoat AX = -nmh_atan2(D[1],Dxz);
    nmoat q1[4],q2[4];nm_QuatFromAngleAxis(q1,AY,0,1,0);nm_QuatFromAngleAxis(q2,AX,1,0,0);
    return nm_QuatMul(qOut4,q1,q2);
}
NM_API_IMPL nmoat* nm_QuatFromEuler(nmoat* NM_RESTRICT qOut4,nmoat fAngleX,nmoat fAngleY,nmoat fAngleZ,int eulerRotationMode)  {
    nmoat qx[4],qy[4],qz[4],*q0,*q1,*q2;
    NM_ASSERT(eulerRotationMode>=1 && eulerRotationMode<=6);
    nm_QuatFromAngleAxis(qx,fAngleX,1,0,0);nm_QuatFromAngleAxis(qy,fAngleY,0,1,0);nm_QuatFromAngleAxis(qz,fAngleZ,0,0,1);
    switch (eulerRotationMode) {
    case 1: q0=qz;q1=qy;q2=qx;break; // EULER_XYZ
    case 2: q0=qy;q1=qz;q2=qx;break; // EULER_XZY
    case 3: q0=qz;q1=qx;q2=qy;break; // EULER_YXZ
    case 4: q0=qx;q1=qz;q2=qy;break; // EULER_YZX
    case 5: q0=qy;q1=qx;q2=qz;break; // EULER_ZXY
    case 6: q0=qx;q1=qy;q2=qz;break; // EULER_ZYX
    default: q0=qz;q1=qx;q2=qy;break; // EULER_YXZ
    }
    return nm_QuatMul3Quats(qOut4,q0,q1,q2);
}
NM_API_IMPL nmoat* nm_QuatLerp(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat t,int normalizeQOutAfterLerp/*=1*/)    {
    nmoat ct = (nmoat)1-t;
    nmoat* r = result4;
    r[3] = a4[4]*ct+b4[3]*t;
    r[0] = a4[0]*ct+b4[0]*t;
    r[1] = a4[1]*ct+b4[1]*t;
    r[2] = a4[2]*ct+b4[2]*t;
    if (normalizeQOutAfterLerp) nm_QuatNormalize(r);
    return  r;
}
NM_API_IMPL nmoat* nm_QuatSlerpEps(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/,nmoat eps/*= NM_SLERP_EPSILON*/)  {
    // Adapted from OgraMath (www.ogre3d.org AFAIR)

    //const int normalizeQOutAfterLerp = 1;            // When using Lerp instead of Slerp qOut should be normalized. However some users prefer setting eps small enough so that they can leave the Lerp as it is.
    //const nmoat eps = NM_SLERP_EPSILON;              // In [0 = 100% Slerp,1 = 100% Lerp] Faster but less precise with bigger epsilon (Lerp is used instead of Slerp more often). Users should tune it to achieve a performance boost.
    const nmoat one = (nmoat)1;
    const nmoat *qStart=a4;
    nmoat qEnd[4]={b4[0],b4[1],b4[2],b4[3]};
    nmoat* qOut=result4;

    nmoat fCos = qStart[0] * qEnd[0] + qStart[1] * qEnd[1] + qStart[2] * qEnd[2] + qStart[3] * qEnd[3];

    // Do we need to invert rotation?
    if(fCos < 0)	//Originally it was if(fCos < (nmoat)0 && shortestPath)
        {fCos = -fCos;qEnd[0] = -qEnd[0];qEnd[1] = -qEnd[1];qEnd[2] = -qEnd[2];qEnd[3] = -qEnd[3];}

    if( fCos < one - eps)	// Originally if was "Ogre::Math::Abs(fCos)" instead of "fCos", but we know fCos>0, because we have hard coded shortestPath=true
    {
        // Standard case (slerp)
#       ifndef NM_QUAT_SLERP_USE_ACOS_AND_SIN_INSTEAD_OF_ATAN2_AND_SQRT
        // Ogre::Quaternion uses this branch by default
        nmoat fSin = nmh_sqrt(one - fCos*fCos);
        nmoat fAngle = nmh_atan2(fSin, fCos);
#       else //NM_QUAT_SLERP_USE_ACOS_AND_SIN_INSTEAD_OF_ATAN2_AND_SQRT
        // Possible replacement of the two lines above
        // (it's hard to tell if they're faster, but my instinct tells me I should trust atan2 better than acos (geometry geeks needed here...)):
        // But probably sin(...) is faster than (sqrt + 1 subtraction and mult)
        nmoat fAngle = nmh_acos(fCos);
        nmoat fSin = nmh_sin(fAngle);
#       endif //NM_QUAT_SLERP_USE_ACOS_AND_SIN_INSTEAD_OF_ATAN2_AND_SQRT

        const nmoat fInvSin = one / fSin;
        const nmoat fCoeff0 = sin((one - slerpTime_In_0_1) * fAngle) * fInvSin;
        const nmoat fCoeff1 = sin(slerpTime_In_0_1 * fAngle) * fInvSin;

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

        const nmoat fCoeff0 = one - slerpTime_In_0_1;
        const nmoat fCoeff1 = slerpTime_In_0_1;

        //qOut =  fCoeff0 * qStart + fCoeff1 * qEnd; //Avoided for maximum portability and conversion of the code
        qOut[0] = (fCoeff0 * qStart[0] + fCoeff1 * qEnd[0]);
        qOut[1] = (fCoeff0 * qStart[1] + fCoeff1 * qEnd[1]);
        qOut[2] = (fCoeff0 * qStart[2] + fCoeff1 * qEnd[2]);
        qOut[3] = (fCoeff0 * qStart[3] + fCoeff1 * qEnd[3]);
        if (normalizeResult4AfterLerp)  nm_QuatNormalize(qOut);
    }

    return qOut;
}
NM_API_IMPL nmoat* nm_QuatSlerp(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat slerpTime_In_0_1,int normalizeResult4AfterLerp/*=1*/)  {return nm_QuatSlerpEps(result4,a4,b4,slerpTime_In_0_1,normalizeResult4AfterLerp,NM_SLERP_EPSILON);}


NM_API_IMPL nmoat* nm_Mat3Identity(nmoat* NM_RESTRICT result9) {
    nmoat* m = result9;
    m[0]=m[4]=m[8]=(nmoat)1;
    m[1]=m[2]=m[3]=m[5]=m[6]=m[7]=(nmoat)0;
    return result9;
}
NM_API_IMPL nmoat* nm_Mat3Mul_NoCheck(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT ml9,const nmoat* NM_RESTRICT mr9)   {
    int i,j,j3;
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
              j3 = 3*j; result9[i+j3] = ml9[i]*mr9[j3] + ml9[i+3]*mr9[1+j3] + ml9[i+6]*mr9[2+j3];
        }
    }
    return result9;
}
NM_API_IMPL nmoat* nm_Mat3Mul(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT ml9,const nmoat* NM_RESTRICT mr9) {
    if (result9==ml9) {
        nmoat ML9[9];nm_Mat3Copy(ML9,ml9);
        return nm_Mat3Mul_NoCheck(result9,ML9,mr9);
    }
    else if (result9==mr9) {
        nmoat MR9[9];nm_Mat3Copy(MR9,mr9);
        return nm_Mat3Mul_NoCheck(result9,ml9,MR9);
    }
    return nm_Mat3Mul_NoCheck(result9,ml9,mr9);
}
NM_API_IMPL nmoat* nm_Mat3Mul3(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9,const nmoat* NM_RESTRICT c9)   {nmoat tmp[9];return nm_Mat3Mul(result9,nm_Mat3Mul(tmp,a9,b9),c9);}
NM_API_IMPL nmoat* nm_Mat3Mul3_NoCheck(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9,const nmoat* NM_RESTRICT c9)   {nmoat tmp[9];return nm_Mat3Mul_NoCheck(result9,nm_Mat3Mul_NoCheck(tmp,a9,b9),c9);}
NM_API_IMPL nmoat* nm_QuatFromMat3(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT m9)  {
    // this code is glm based
    nmoat* q=result4;const nmoat* m=m9;
    nmoat *qx=&q[0],*qy=&q[1],*qz=&q[2],*qw=&q[3];
    const nmoat c00=m[0],c01=m[1],c02=m[2], c10=m[3],c11=m[4],c12=m[5], c20=m[6],c21=m[7],c22=m[8];

    nmoat fourXSquaredMinus1 = c00 - c11 - c22, fourYSquaredMinus1 = c11 - c00 - c22;
    nmoat fourZSquaredMinus1 = c22 - c00 - c11, fourWSquaredMinus1 = c00 + c11 + c22;
    nmoat biggestVal,mult,fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    int biggestIndex = 0;

    if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourXSquaredMinus1;biggestIndex = 1;}
    if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourYSquaredMinus1;biggestIndex = 2;}
    if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourZSquaredMinus1;biggestIndex = 3;}

    biggestVal = nmh_sqrt(fourBiggestSquaredMinus1 + (nmoat)1) * (nmoat)0.5;
    mult = (nmoat)0.25 / biggestVal;

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
        //NM_ASSER(1);
        *qx=*qy=*qz=(nmoat)0;*qw=(nmoat)1;
        break;
    }
    return result4;
}
NM_API_IMPL nmoat* nm_Mat3FromQuat(nmoat* NM_RESTRICT result9,const nmoat* NM_RESTRICT q4)    {
    // this code is glm based
    const nmoat one =(nmoat)1,two=(nmoat)2;
    nmoat* m=result9;const nmoat* q=q4;
    const nmoat qx=q[0],qy=q[1],qz=q[2],qw=q[3];
    nmoat *c00=&m[0],*c01=&m[1],*c02=&m[2], *c10=&m[3],*c11=&m[4],*c12=&m[5], *c20=&m[6],*c21=&m[7],*c22=&m[8];

    nmoat qxx = (qx * qx), qyy = (qy * qy), qzz = (qz * qz);
    nmoat qxz = (qx * qz), qxy = (qx * qy), qyz = (qy * qz);
    nmoat qwx = (qw * qx), qwy = (qw * qy), qwz = (qw * qz);

    *c00 = one - two * (qyy +  qzz); *c01 = two * (qxy + qwz); *c02 = two * (qxz - qwy);
    *c10 = two * (qxy - qwz); *c11 = one - two * (qxx +  qzz); *c12 = two * (qyz + qwx);
    *c20 = two * (qxz + qwy); *c21 = two * (qyz - qwx); *c22 = one - two * (qxx +  qyy);

    return result9;
}
NM_API_IMPL void nm_Mat3TransposeInPlace(nmoat* NM_RESTRICT m9)    {
    nmoat* m=m9;
    {nmoat t;t=m[1];m[1]=m[3];m[3]=t;}
    {nmoat t;t=m[2];m[2]=m[6];m[6]=t;}
    {nmoat t;t=m[5];m[5]=m[7];m[7]=t;}
}
NM_API_IMPL nmoat* nm_Mat3Transpose(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT m9)  {
    nmoat* d=mOut9;const nmoat* m=m9;
    d[0]=m[0];d[1]=m[3];d[2]=m[6];d[3]=m[1];d[4]=m[4];d[5]=m[7];d[6]=m[2];d[7]=m[5];d[8]=m[8];
    return mOut9;
}
NM_API_IMPL int nm_Mat3Invert(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT m9)	{
    const nmoat* m = m9;
    nmoat* n = mOut9;

    /*| 00 01 02 |       | 00 10 20 |
      | 10 11 12 | ->T-> | 01 11 21 |
      | 20 21 22 |       | 02 12 22 |  */

    // just copying
    nmoat m00 = m[0],  m10 = m[1],  m20 = m[2];
    nmoat m01 = m[3],  m11 = m[4],  m21 = m[5];
    nmoat m02 = m[6],  m12 = m[7],  m22 = m[8];

    // 3 signed cofactors of the transposed matrix
    nmoat c00 = +(m11*m22-m12*m21);
    nmoat c01 = -(m10*m22-m12*m20);
    nmoat c02 = +(m10*m21-m11*m20);

    // determinant of the original matrix
    nmoat det = m00*c00 +m01*c01 +m02*c02;

    if (det==(nmoat)0) return 0;
    {
        nmoat c10,c20,c11,c21,c12,c22;
        nmoat invDet = 1 / det;

        // the 3 cofactors of the transposed matrix * invDet
        c00*=invDet;c01*=invDet;c02*=invDet;

        // the other 6 cofactors of the transposed matrix * invDet
        c10 = -(m01*m22-m02*m21)*invDet;
        c20 = +(m01*m12-m02*m11)*invDet;
        c11 = +(m00*m22-m02*m20)*invDet;
        c21 = -(m00*m12-m02*m10)*invDet;
        c12 = -(m00*m21-m01*m20)*invDet;
        c22 = +(m00*m11-m01*m10)*invDet;

        n[0] =c00; n[1] =c01; n[2] =c02;
        n[3] =c10; n[4] =c11; n[5] =c12;
        n[6] =c20; n[7] =c21; n[8] =c22;
    }
    return 1;
}
NM_API_IMPL int nm_Mat3InvertInPlace(nmoat* NM_RESTRICT m9)	{return nm_Mat3Invert(m9,m9);}
NM_API_IMPL const nmoat* nm_Mat3SlerpEps(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT mStart9,const nmoat* NM_RESTRICT mEnd9,nmoat slerpTime_In_0_1,nmoat eps)    {
    if (slerpTime_In_0_1<=0)        {if (mOut9!=mStart9) nm_Mat3Copy(mOut9,mStart9);return mOut9;}
    else if (slerpTime_In_0_1>=1)   {if (mOut9!=mEnd9)   nm_Mat3Copy(mOut9,mEnd9); return mOut9;}
    {
        // Slerp rotations
        //const nmoat slerp_eps = NM_SLERP_EPSILON;//(nmoat)0.0001;
        nmoat qStart[4],qEnd[4],qSlerp[4];
        nm_Mat3FromQuat(mOut9,nm_QuatSlerpEps(qSlerp,nm_QuatFromMat3(qStart,mStart9),nm_QuatFromMat3(qEnd,mEnd9),slerpTime_In_0_1,1,eps));
    }
    return mOut9;
}
NM_API_IMPL const nmoat* nm_Mat3Slerp(nmoat* NM_RESTRICT mOut9,const nmoat* NM_RESTRICT mStart9,const nmoat* NM_RESTRICT mEnd9,nmoat slerpTime_In_0_1) {return nm_Mat3SlerpEps(mOut9,mStart9,mEnd9,slerpTime_In_0_1,NM_SLERP_EPSILON);}
/*// The matX_fromEulerXYZ/matX_toEulerXYZ functions are ported/adapted/arranged/inspired from the following sources:
//
// https://github.com/juj/MathGeoLib (Apache 2 license)
// [And going down the chain some of them are adapted from http://www.geometrictools.com/Documentation/EulerAngles.pdf]
//
// OgreMatrix3x3.cpp (based on geometrictools itself)
//
// Reference code used a row-major matrix storage so I had to change many things... I'm not sure it's 100% correct!
// Use it at your own risk!
//
// Also we should make roundtrip tests and see if these functions pass them or not!

// Slow! By factorizing the 3 matrix multiplications of each matX_fromEulerXYZ function we can improve performance
// https://github.com/juj/MathGeoLib (and the Wild Magic Math Library from http://www.geometrictools.com) provides
// factorized implementation, but they use a different convention (row-major order and maybe a different matrix multiplication order),
// so I could not use their code.

// Tip: by only using matX.col[col][row] (instead of matX.m[element_index]), the same code can be written for both mat3 and mat4
// (well, to support plain c we can't use templates, but it still makes copying/pasting easier).

// IMPORTANT: the order of rotation (XYZ,XZY, and so on) is named according to the convention used in the Blender program.
// What I mean is that somebody might say that the correct naming should be inverted (ZYX,YZX, and so on). In short
// if you find strange results try invering the EulerRotationOrder and see if it works.
*/
NM_API_PRIV_INL void nm_priv_helper_Mat3FromEulerCommonHelper(nmoat* X,nmoat* Y,nmoat* Z,nmoat x,nmoat y,nmoat z)	{
    nmoat cx, sx, cy, sy, cz, sz; const nmoat zero=(nmoat)0,one=(nmoat)1;

    nmh_sincos(x, &sx, &cx);
    X[0]=one;X[1]=X[2]=X[3]=X[6]=zero;  X[4]=X[8]=cx;   X[5]=sx;X[7]=-sx;
    //*X=make_mat3(1,0,0,   0,cx,sx,    0,-sx,cx);

    nmh_sincos(y, &sy, &cy);
    Y[4]=one;Y[1]=Y[3]=Y[5]=Y[7]=zero;  Y[0]=Y[8]=cy;   Y[6]=sy;Y[2]=-sy;
    //*Y=make_mat3(cy,0,-sy,  0,1,0,  sy,0,cy);

    nmh_sincos(z, &sz, &cz);
    Z[8]=one;Z[2]=Z[5]=Z[6]=Z[7]=zero;  Z[0]=Z[4]=cz;   Z[1]=sz;Z[3]=-sz;
    //*Z=make_mat3(cz,sz,0,   -sz,cz,0,   0,0,1);
}
NM_API_IMPL nmoat* nm_Mat3FromEulerXYZ(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z)    {nmoat X[9],Y[9],Z[9];nm_priv_helper_Mat3FromEulerCommonHelper(X,Y,Z,x,y,z);return nm_Mat3Mul3_NoCheck(m9,Z,Y,X);}
NM_API_IMPL nmoat* nm_Mat3FromEulerXZY(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z)    {nmoat X[9],Y[9],Z[9];nm_priv_helper_Mat3FromEulerCommonHelper(X,Y,Z,x,y,z);return nm_Mat3Mul3_NoCheck(m9,Y,Z,X);}
NM_API_IMPL nmoat* nm_Mat3FromEulerYXZ(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z)    {nmoat X[9],Y[9],Z[9];nm_priv_helper_Mat3FromEulerCommonHelper(X,Y,Z,x,y,z);return nm_Mat3Mul3_NoCheck(m9,Z,X,Y);}
NM_API_IMPL nmoat* nm_Mat3FromEulerYZX(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z)    {nmoat X[9],Y[9],Z[9];nm_priv_helper_Mat3FromEulerCommonHelper(X,Y,Z,x,y,z);return nm_Mat3Mul3_NoCheck(m9,X,Z,Y);}
NM_API_IMPL nmoat* nm_Mat3FromEulerZXY(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z)    {nmoat X[9],Y[9],Z[9];nm_priv_helper_Mat3FromEulerCommonHelper(X,Y,Z,x,y,z);return nm_Mat3Mul3_NoCheck(m9,Y,X,Z);}
NM_API_IMPL nmoat* nm_Mat3FromEulerZYX(nmoat* NM_RESTRICT m9,nmoat x,nmoat y,nmoat z)    {nmoat X[9],Y[9],Z[9];nm_priv_helper_Mat3FromEulerCommonHelper(X,Y,Z,x,y,z);return nm_Mat3Mul3_NoCheck(m9,X,Y,Z);}
NM_API_IMPL int nm_Mat3ToEulerXYZ(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m9;
    /*
    |  m[0]==col[0][0]==m00  m[3]==col[1][0]==m01  m[6]==col[2][0]==m02  |
    |  m[1]==col[0][1]==m10  m[4]==col[1][1]==m11  m[7]==col[2][1]==m12  |
    |  m[2]==col[0][2]==m20  m[5]==col[1][2]==m21  m[8]==col[2][2]==m22  |
    */
    const nmoat c00=m[0],c01=m[1],c02=m[2], c11=m[4],c12=m[5], c21=m[7],c22=m[8];
    if (c02 < (nmoat)1) {
        if (c02 > -(nmoat)1)    {*y = nmh_asin(-c02); *z = nmh_atan2(c01, c00); *x = nmh_atan2(c12, c22); rv = 1;}
        else    {
            // Not a unique solution.
            *y = M_HALF_PI; *z = -nmh_atan2(-c21, c11); *x = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *y = -M_HALF_PI; *z = nmh_atan2(-c21, c11); *x = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat3ToEulerXZY(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m9;
    const nmoat c00=m[0],c02=m[2], c10=m[3],c11=m[4],c12=m[5], c20=m[6],c22=m[8];
    if (c12 < (nmoat)1) {
        if (c12 > -(nmoat)1)    {*x = nmh_asin(c12); *z = nmh_atan2(-c10, c11); *y = nmh_atan2(-c02, c22); rv = 1;}
        else    {
            // Not a unique solution.
            *x = -M_HALF_PI; *z = -nmh_atan2(-c20, c00); *y = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *x = M_HALF_PI; *z = nmh_atan2(c20, c00); *y = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat3ToEulerYXZ(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m9;
    const nmoat c00=m[0],c01=m[1],c02=m[2], c11=m[4],c12=m[5], c21=m[7],c22=m[8];
    if (c01 < (nmoat)1) {
        if (c01 > -(nmoat)1)    {
            *z = nmh_asin(c01);
            *y = -nmh_atan2(c02, c00);	// Modified: added minus sign
            *x = -nmh_atan2(c21, c11);	// Modified: added minus sign
            rv = 1;
        }
        else    {
            // Not a unique solution.
            *z = -M_HALF_PI; *y = -nmh_atan2(c12, c22); *x = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *z = M_HALF_PI; *y = nmh_atan2(-c12, c22); *x = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat3ToEulerYZX(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m9;
    const nmoat c00=m[0],c01=m[1], c10=m[3],c11=m[4], c20=m[6],c21=m[7],c22=m[8];
    if (c21 < (nmoat)1) {
        if (c21 > -(nmoat)1)    {*x = nmh_asin(-c21); *y = nmh_atan2(c20, c22); *z = nmh_atan2(c01, c11); rv = 1;}
        else    {
            // Not a unique solution.
            *x = M_HALF_PI; *y = -nmh_atan2(-c10, c00); *z = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *x = -M_HALF_PI; *y = nmh_atan2(-c10, c00); *z = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat3ToEulerZXY(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m9;
    const nmoat c00=m[0],c02=m[2], c10=m[3],c11=m[4],c12=m[5], c20=m[6],c22=m[8];
    if (c10 < (nmoat)1) {
        if (c10 > -(nmoat)1)    {*z = nmh_asin(-c10); *x = nmh_atan2(c12, c11); *y = nmh_atan2(c20, c00); rv = 1;}
        else    {
            // Not a unique solution: y - x = atan2(-c02, c22);
            *z = M_HALF_PI; *x = nmh_atan2(-c02, c22); *y = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution: y + x = atan2(-c02, c22);
        *z = -M_HALF_PI; *x = nmh_atan2(-c02, c22); *y = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat3ToEulerZYX(const nmoat* NM_RESTRICT m9,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m9;
    const nmoat c00=m[0],c01=m[1], c10=m[3],c11=m[4], c20=m[6],c21=m[7],c22=m[8];
    if (c20 < (nmoat)1) {
        if (c20 > -(nmoat)1)    {*y = nmh_asin(c20); *x = nmh_atan2(-c21, c22); *z = nmh_atan2(-c10, c00); rv = 1;}
        else {
            // Not a unique solution: z - x = atan2(c01, c11);
            *y = -M_HALF_PI; *x = -nmh_atan2(c01, c11); *z = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution: z + x = atan2(c01, c11);
        *y = M_HALF_PI; *x = nmh_atan2(c01, c11); *z = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL void nm_Mat3FromEuler(nmoat* m9,nmoat x,nmoat y,nmoat z,int eulerRotationMode)   {
    NM_ASSERT(eulerRotationMode>=1 && eulerRotationMode<=6);
    switch (eulerRotationMode) {
    case 1://EULER_XYZ
        nm_Mat3FromEulerXYZ(m9,x,y,z);
        break;
    case 2://EULER_XZY
        nm_Mat3FromEulerXZY(m9,x,y,z);
        break;
    case 3://EULER_YXZ
        nm_Mat3FromEulerYXZ(m9,x,y,z);
        break;
    case 4://EULER_YZX
        nm_Mat3FromEulerYZX(m9,x,y,z);
        break;
    case 5://EULER_ZXY
        nm_Mat3FromEulerZXY(m9,x,y,z);
        break;
    case 6://EULER_ZYX
        nm_Mat3FromEulerZYX(m9,x,y,z);
        break;
    default:
        //NM_ASSERT(1);   // wrong mode
        nm_Mat3FromEulerXYZ(m9,x,y,z);
        break;
    }
}
NM_API_IMPL int nm_Mat3ToEuler(const nmoat* m9,nmoat* x,nmoat* y,nmoat* z,int eulerRotationMode)  {
    NM_ASSERT(eulerRotationMode>=1 && eulerRotationMode<=6);
    switch (eulerRotationMode) {
    case 1://EULER_XYZ
        return nm_Mat3ToEulerXYZ(m9,x,y,z);
        break;
    case 2://EULER_XZY
        return nm_Mat3ToEulerXZY(m9,x,y,z);
        break;
    case 3://EULER_YXZ
        return nm_Mat3ToEulerYXZ(m9,x,y,z);
        break;
    case 4://EULER_YZX
        return nm_Mat3ToEulerYZX(m9,x,y,z);
        break;
    case 5://EULER_ZXY
        return nm_Mat3ToEulerZXY(m9,x,y,z);
        break;
    case 6://EULER_ZYX
        return nm_Mat3ToEulerZYX(m9,x,y,z);
        break;
    default:
        //NM_ASSERT(1);   // wrong mode
        return nm_Mat3ToEulerYXZ(m9,x,y,z);
        break;
    }
    return 0;
}


NM_API_IMPL nmoat* nm_Mat4Identity(nmoat* NM_RESTRICT result16) {
    nmoat* m = result16;
    m[0]=m[5]=m[10]=m[15]=(nmoat)1;
    m[1]=m[2]=m[3]=m[4]=m[6]=m[7]=m[8]=m[9]=m[11]=m[12]=m[13]=m[14]=(nmoat)0;
    return result16;
}
NM_API_IMPL nmoat* nm_Mat4Mul_NoCheck(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT ml16,const nmoat* NM_RESTRICT mr16)   {
#	if (defined(NM_USE_SIMD) && !defined(NM_DOUBLE_PRECISION) && defined(__SSE__))
    /* Code based on http://fhtr.blogspot.com/2010/02/4x4-float-matrix-multiplication-using.html */
    int i,j;__m128 res_col;
    for (i=0; i<16; i+=4) {
        res_col = _mm_mul_ps(NM_MM_LOAD_PS(ml16), _mm_set1_ps(mr16[i]));
        for (j=1; j<4; j++) res_col = _mm_add_ps(_mm_mul_ps(NM_MM_LOAD_PS(&ml16[j*4]), _mm_set1_ps(mr16[i+j])), res_col);
        NM_MM_STORE_PS(&result16[i], res_col);
    }
#	elif (defined(NM_USE_SIMD) && defined(NM_DOUBLE_PRECISION) && defined(__AVX__))
    /* Code based on http://fhtr.blogspot.com/2010/02/4x4-float-matrix-multiplication-using.html */
    int i,j;__m256d res_col;
    for (i=0; i<16; i+=4) {
        res_col = _mm256_mul_pd(NM_MM256_LOAD_PD(ml16), _mm256_set1_pd(mr16[i]));
        for (j=1; j<4; j++) res_col = _mm256_add_pd(_mm256_mul_pd(NM_MM256_LOAD_PD(&ml16[j*4]), _mm256_set1_pd(mr16[i+j])), res_col);
        NM_MM256_STORE_PD(&result16[i], res_col);
    }
#	else //NM_USE_SIMD
    /* reference implementation */
    int i,i4;
    nmoat mri4plus0,mri4plus1,mri4plus2,mri4plus3;
    for(i = 0; i < 4; i++) {
        i4=4*i;mri4plus0=mr16[i4];mri4plus1=mr16[i4+1];mri4plus2=mr16[i4+2];mri4plus3=mr16[i4+3];
        result16[  i4] = ml16[0]*mri4plus0 + ml16[4]*mri4plus1 + ml16[ 8]*mri4plus2 + ml16[12]*mri4plus3;
        result16[1+i4] = ml16[1]*mri4plus0 + ml16[5]*mri4plus1 + ml16[ 9]*mri4plus2 + ml16[13]*mri4plus3;
        result16[2+i4] = ml16[2]*mri4plus0 + ml16[6]*mri4plus1 + ml16[10]*mri4plus2 + ml16[14]*mri4plus3;
        result16[3+i4] = ml16[3]*mri4plus0 + ml16[7]*mri4plus1 + ml16[11]*mri4plus2 + ml16[15]*mri4plus3;
    }
#	endif //NM_USE_SIMD
    return result16;
}
NM_API_IMPL nmoat* nm_Mat4Mul(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT ml16,const nmoat* NM_RESTRICT mr16) {
    if (result16==ml16) {
        nmoat ML16[16];nm_Mat4Copy(ML16,ml16);
        return nm_Mat4Mul_NoCheck(result16,ML16,mr16);
    }
    else if (result16==mr16) {
        nmoat MR16[16];nm_Mat4Copy(MR16,mr16);
        return nm_Mat4Mul_NoCheck(result16,ml16,MR16);
    }
    return nm_Mat4Mul_NoCheck(result16,ml16,mr16);
}
NM_API_IMPL void nm_Mat4LookAt(nmoat* NM_RESTRICT mOut16,nmoat eyeX,nmoat eyeY,nmoat eyeZ,nmoat centerX,nmoat centerY,nmoat centerZ,nmoat upX,nmoat upY,nmoat upZ)    {
    nmoat* m = mOut16;
    const nmoat eps = (nmoat)0.0001;

    nmoat F[3] = {eyeX-centerX,eyeY-centerY,eyeZ-centerZ};
    nmoat length = F[0]*F[0]+F[1]*F[1]+F[2]*F[2];	// length2 now
    nmoat up[3] = {upX,upY,upZ};

    nmoat S[3] = {up[1]*F[2]-up[2]*F[1],up[2]*F[0]-up[0]*F[2],up[0]*F[1]-up[1]*F[0]};
    nmoat U[3] = {F[1]*S[2]-F[2]*S[1],F[2]*S[0]-F[0]*S[2],F[0]*S[1]-F[1]*S[0]};

    if (length==(nmoat)0) length = eps;
    length = nmh_sqrt(length);
    F[0]/=length;F[1]/=length;F[2]/=length;

    length = S[0]*S[0]+S[1]*S[1]+S[2]*S[2];if (length==(nmoat)0) length = eps;
    length = nmh_sqrt(length);
    S[0]/=length;S[1]/=length;S[2]/=length;

    length = U[0]*U[0]+U[1]*U[1]+U[2]*U[2];if (length==(nmoat)0) length = eps;
    length = nmh_sqrt(length);
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
NM_API_IMPL void nm_Mat4Perspective(nmoat* NM_RESTRICT mOut16,nmoat degfovy,nmoat aspect, nmoat zNear, nmoat zFar) {
    nmoat* res = mOut16;
    const nmoat eps = (nmoat)0.0001;
    nmoat f = 1.f/nmh_tan(degfovy*(nmoat)(1.5707963268/180.0)); //cotg
    nmoat Dfn = (zFar-zNear);
    if (Dfn==(nmoat)0) {zFar+=eps;zNear-=eps;Dfn=zFar-zNear;}
    if (aspect==(nmoat)0) aspect = (nmoat)1;

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
NM_API_IMPL void nm_Mat4Ortho(nmoat* NM_RESTRICT mOut16,nmoat left,nmoat right, nmoat bottom, nmoat top,nmoat nearVal,nmoat farVal) {
    nmoat* res = mOut16;
    const nmoat eps = (nmoat)0.0001;
    nmoat Drl = (right-left);
    nmoat Dtb = (top-bottom);
    nmoat Dfn = (farVal-nearVal);
    if (Drl==(nmoat)0) {right+=eps;left-=eps;Drl=right-left;}
    if (Dtb==(nmoat)0) {top+=eps;bottom-=eps;Dtb=top-bottom;}
    if (Dfn==(nmoat)0) {farVal+=eps;nearVal-=eps;Dfn=farVal-nearVal;}

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
NM_API_IMPL void nm_Mat4Ortho3D(nmoat* NM_RESTRICT mOut16,nmoat cameraTargetDistance,nmoat degfovy,nmoat aspect,nmoat znear,nmoat zfar)	{
    // Warning: this function might be WRONG! Use it at your own risk!
    const nmoat FOVTG=nmh_tan(degfovy*(nmoat)(3.14159265358979323846/360.0));
    nmoat y=cameraTargetDistance*FOVTG;//=(zfar-znear)*0.5f;
    //nmoat y=(cameraTargetDistance<zfar?cameraTargetDistance:zfar)*FOVTG;  // or maybe this?
    nmoat x=y*aspect;
    //Helper_Ortho(mOut16, -x, x, -y, y, znear, zfar);  // I thought this was correct
    //Helper_Ortho(mOut16, -x, x, -y, y, -zfar, znear);  // But this works better in my test-case
    nm_Mat4Ortho(mOut16, -x, x, -y, y, -zfar, -znear);  // Or maybe this?
}

NM_API_IMPL int nm_Mat4Project(nmoat objX,nmoat objY,nmoat objZ,const nmoat* NM_RESTRICT mvMatrix16,const nmoat* NM_RESTRICT pMatrix16,const int* viewport4,nmoat* NM_RESTRICT winX,nmoat* NM_RESTRICT winY,nmoat* NM_RESTRICT winZ)    {
    // transforms the specified object coordinates into window coordinates using mvMatrix16, pMatrix16, and viewport4.
    // The result is stored in winX, winY, and winZ. A return value of 1 indicates success, a return value of 0 indicates failure.
    // viewport4 Specifies the current viewport (as from a glGetIntegerv call).

    // To compute the coordinates, let v = objX objY objZ 1.0 represented as a matrix with 4 rows and 1 column.
    // Then gluProject computes v ″ as follows:
    // v ″ = P × M × v
    // where P is the current projection matrix proj and M is the current modelview matrix model
    // (both represented as 4 × 4 matrices in column-major order).
    const nmoat objW = 1;
    // v ″ = P × M × v = P × (M × v) AFAIK
    const nmoat mv[4] = {
        objX*mvMatrix16[0] + objY*mvMatrix16[4] + objZ*mvMatrix16[8]  + objW*mvMatrix16[12],
        objX*mvMatrix16[1] + objY*mvMatrix16[5] + objZ*mvMatrix16[9]  + objW*mvMatrix16[13],
        objX*mvMatrix16[2] + objY*mvMatrix16[6] + objZ*mvMatrix16[10] + objW*mvMatrix16[14],
        objX*mvMatrix16[3] + objY*mvMatrix16[7] + objZ*mvMatrix16[11] + objW*mvMatrix16[15]
    };
    nmoat pmv[4] = {
        mv[0]*pMatrix16[0] + mv[1]*pMatrix16[4] + mv[2]*pMatrix16[8]  + mv[3]*pMatrix16[12],
        mv[0]*pMatrix16[1] + mv[1]*pMatrix16[5] + mv[2]*pMatrix16[9]  + mv[3]*pMatrix16[13],
        mv[0]*pMatrix16[2] + mv[1]*pMatrix16[6] + mv[2]*pMatrix16[10] + mv[3]*pMatrix16[14],
        mv[0]*pMatrix16[3] + mv[1]*pMatrix16[7] + mv[2]*pMatrix16[11] + mv[3]*pMatrix16[15] // (*)
    };

    if(pmv[3]==(nmoat)0) return 0;
    pmv[0]/=pmv[3];pmv[1]/=pmv[3];pmv[2]/=pmv[3];

    // The window coordinates are then computed as follows:
    *winX = (nmoat)viewport4[0] + (nmoat)viewport4[2]*(pmv[0]*(nmoat)0.5+(nmoat)0.5);
    *winY = (nmoat)viewport4[1] + (nmoat)viewport4[3]*(pmv[1]*(nmoat)0.5+(nmoat)0.5);
    *winZ = pmv[2]*(nmoat)0.5+(nmoat)0.5;

    return 1;
}
NM_API_IMPL int nm_Mat4UnProjectMvpInv(nmoat winX,nmoat winY,nmoat winZ,const nmoat* NM_RESTRICT mvpMatrixInv16,const int* viewport4,nmoat* NM_RESTRICT objX,nmoat* NM_RESTRICT objY,nmoat* NM_RESTRICT objZ)    {
    // To compute the coordinates objX objY objZ , gluUnProject multiplies the normalized device coordinates by the inverse of model * proj as follows:
    // [objX objY objZ W] = INV(P*M) * ⁢ [2*(winX-view[0])/view[2]-1    2*(winY-view[1])/view[3]-1  2*winZ-1    1]
    // INV denotes matrix inversion. W is an unused variable, included for consistent matrix notation.
   const nmoat *invpm = mvpMatrixInv16;
   // normalized device coords
   const nmoat v[4] = {
      2*(winX-(nmoat)viewport4[0])/(nmoat)viewport4[2]-1,
      2*(winY-(nmoat)viewport4[1])/(nmoat)viewport4[3]-1,
      2*winZ-1,
      1
   };
   nmoat objW = 0;
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
NM_API_IMPL int nm_Mat4UnProject(nmoat winX,nmoat winY,nmoat winZ,const nmoat* NM_RESTRICT mvMatrix16, const nmoat* NM_RESTRICT pMatrix16, const int* viewport4,nmoat* NM_RESTRICT objX,nmoat* NM_RESTRICT objY,nmoat* NM_RESTRICT objZ)    {
    // maps the specified window coordinates into object coordinates using mvMatrix16, pMatrix16, and viewport4.
    // The result is stored in objX, objY, and objZ. A return value of 1 indicates success; a return value of 0 indicates failure.
    // viewport4 Specifies the current viewport (as from a glGetIntegerv call).
    nmoat pm[16],invpm[16];
    // calculate pm and invert it to get invpm:
    nm_Mat4Invert(invpm,nm_Mat4Mul(pm,pMatrix16,mvMatrix16));
    return nm_Mat4UnProjectMvpInv(winX,winY,winZ,invpm,viewport4,objX,objY,objZ);
}
NM_API_IMPL int nm_Mat4UnProject4MvpInv(nmoat winX,nmoat winY,nmoat winZ,nmoat clipW,const nmoat* NM_RESTRICT mvpMatrixInv16,const int* viewport4,nmoat nearVal,nmoat farVal,nmoat* NM_RESTRICT objX,nmoat* NM_RESTRICT objY,nmoat* NM_RESTRICT objZ,nmoat* NM_RESTRICT objW)    {
   // nm_Mat4UnProject4MvpInv is equivalent to nm_Mat4UnProjectMvpInv when clipW is 1, nearVal is 0, and farVal is 1.

   // [objX objY objZ objW] = INV(P*M) * ⁢ [2*(winX-view[0])/view[2]-1    2*(winY-view[1])/view[3]-1  2*(winZ-nearVal)/(farVal-nearVal)-1    clipW]
   // imuUnProject4 is equivalent to imuUnProject when clipW is 1, nearVal is 0, and farVal is 1.
   const nmoat *invpm = mvpMatrixInv16;
   // normalized device coords
   const nmoat v[4] = {
      2*(winX-(nmoat)viewport4[0])/(nmoat)viewport4[2]-1,
      2*(winY-(nmoat)viewport4[1])/(nmoat)viewport4[3]-1,
      2*(winZ-nearVal)/(farVal-nearVal)-1,
      clipW
   };
   // calculate obj
   *objX =  v[0]*invpm[0] + v[1]*invpm[4] + v[2]*invpm[8]  + v[3]*invpm[12];
   *objY =  v[0]*invpm[1] + v[1]*invpm[5] + v[2]*invpm[9]  + v[3]*invpm[13];
   *objZ =  v[0]*invpm[2] + v[1]*invpm[6] + v[2]*invpm[10] + v[3]*invpm[14];
   *objW =  v[0]*invpm[3] + v[1]*invpm[7] + v[2]*invpm[11] + v[3]*invpm[15];
   return 1;
}
NM_API_IMPL int nm_Mat4UnProject4(nmoat winX, nmoat winY, nmoat winZ, nmoat clipW, const nmoat* NM_RESTRICT mvMatrix16, const nmoat* NM_RESTRICT pMatrix16, const int* viewport4, nmoat nearVal, nmoat farVal, nmoat* NM_RESTRICT objX, nmoat* NM_RESTRICT objY, nmoat* NM_RESTRICT objZ, nmoat* NM_RESTRICT objW)    {
    // int nm_Mat4UnProject4 is equivalent to int nm_Mat4UnProject when clipW is 1, nearVal is 0, and farVal is 1.

    // maps the specified window coordinatesi: winX, winY, and winZ and its clip w coordinate clipW
    // into object coordinates objX objY objZ objW using mvMatrix16, pMatrix16, and viewport4.
    // clipW can be other than 1 as for vertices in glFeedbackBuffer when data type GLU_4D_COLOR_TEXTURE is returned.
    // This also handles the case where the nearVal and farVal planes are different from the default, 0 and 1, respectively.
    // A return value of 1 indicates success; a return value of 0 indicates failure.
    // viewport4 Specifies the current viewport (as from a glGetIntegerv call).
    nmoat pm[16],invpm[16];
    // calculate pm and invert it to get invpm:
    nm_Mat4Invert(invpm,nm_Mat4Mul(pm,pMatrix16,mvMatrix16));
    return nm_Mat4UnProject4MvpInv(winX,winY,winZ,clipW,invpm,viewport4,farVal,nearVal,objX,objY,objZ,objW);
}

NM_API_IMPL nmoat* nm_Mat4Translate(nmoat* NM_RESTRICT mInOut16,nmoat x,nmoat y,nmoat z)  {
    const nmoat m[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        x, y, z, 1};
    return nm_Mat4Mul(mInOut16,mInOut16,m);
}
NM_API_IMPL nmoat* nm_Mat4Rotate(nmoat* NM_RESTRICT mInOut16,nmoat degAngle,nmoat x,nmoat y,nmoat z)  {
    const nmoat angle = degAngle*(nmoat)M_PIOVER180;
    const nmoat c = nmh_cos(angle);
    const nmoat s = nmh_sin(angle);
    nmoat len = x*x+y*y+z*z;
    if (len<(nmoat)0.999 || len>(nmoat)1.001) {len=nmh_sqrt(len);x/=len;y/=len;z/=len;}
    {
        const nmoat m[16] = {
            c + x*x*(1-c),  y*x*(1-c)+z*s,    z*x*(1-c)-y*s,    0,
            x*y*(1-c) - z*s,  c + y*y*(1-c),      z*y*(1-c) + x*s,    0,
            x*z*(1-c) + y*s,  y*z*(1-c) - x*s,    c + z*z*(1-c),      0,
            0,              0,                  0,                  1};
        return nm_Mat4Mul(mInOut16,mInOut16,m);
    }
}
NM_API_IMPL nmoat* nm_Mat4Scale(nmoat* NM_RESTRICT mInOut16,nmoat x,nmoat y,nmoat z)  {
    const nmoat m[16] = {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
    return nm_Mat4Mul(mInOut16,mInOut16,m);
}
NM_API_IMPL void nm_Mat4TransposeInPlace(nmoat* NM_RESTRICT m16)   {
    nmoat* m=m16;
    nmoat s01=m[4],s02=m[8],s03=m[12],   s10=m[1],s12=m[9],s13=m[13],   s20=m[2],s21=m[6],s23=m[14],   s30=m[3],s31=m[7],s32=m[11];
          m[4]=s10;m[8]=s20;m[12]=s30;   m[1]=s01;m[9]=s21;m[13]=s31;   m[2]=s02;m[6]=s12;m[14]=s32;   m[3]=s03;m[7]=s13;m[11]=s23;
}
NM_API_IMPL nmoat* nm_Mat4Transpose(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16)    {
    nmoat* d= mOut16;const nmoat* m=m16;
    d[0]=m[0];d[1]=m[4];d[2]=m[8];d[3]=m[12];d[4]=m[1];d[5]=m[5];d[6]=m[9];d[7]=m[13];d[8]=m[2];d[9]=m[6];d[10]=m[10];d[11]=m[14];d[12]=m[3];d[13]=m[7];d[14]=m[11];d[15]=m[15];
    return mOut16;
}


NM_API_IMPL int nm_Mat4Invert(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16)	{
#	if (defined(NM_USE_SIMD) && !defined(NM_DOUBLE_PRECISION) && defined(__SSE__))
    // Ported from Eric's Blog: https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
    // use block matrix method
    // A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace
    __m128 col0 = NM_MM_LOAD_PS(&m16[0]);
    __m128 col1 = NM_MM_LOAD_PS(&m16[4]);
    __m128 col2 = NM_MM_LOAD_PS(&m16[8]);
    __m128 col3 = NM_MM_LOAD_PS(&m16[12]);

    // sub matrices
    __m128 A = VecShuffle_0101(col0, col1);
    __m128 C = VecShuffle_2323(col0, col1);
    __m128 B = VecShuffle_0101(col2, col3);
    __m128 D = VecShuffle_2323(col2, col3);

    // @Flix: I had to swap detB and detC from https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html#_appendix
    __m128 detA = _mm_set1_ps(m16[0] *m16[5] - m16[4] *m16[1]);
    __m128 detB = _mm_set1_ps(m16[8] *m16[13]- m16[12]*m16[9]);
    __m128 detC = _mm_set1_ps(m16[2] *m16[7] - m16[6] *m16[3]);
    __m128 detD = _mm_set1_ps(m16[10]*m16[15]- m16[14]*m16[11]);

    // let iM = 1/|M| * | X  Y |
    //                  | Z  W |

    // D#C
    __m128 D_C = Mat2AdjMul(D, C);
    // A#B
    __m128 A_B = Mat2AdjMul(A, B);
    // X# = |D|A - B(D#C)
    __m128 X_ = _mm_sub_ps(_mm_mul_ps(detD, A), Mat2Mul(B, D_C));
    // W# = |A|D - C(A#B)
    __m128 W_ = _mm_sub_ps(_mm_mul_ps(detA, D), Mat2Mul(C, A_B));

    // |M| = |A|*|D| + ... (continue later)
    __m128 detM = _mm_mul_ps(detA, detD);

    // Y# = |B|C - D(A#B)#
    __m128 Y_ = _mm_sub_ps(_mm_mul_ps(detB, C), Mat2MulAdj(D, A_B));
    // Z# = |C|B - A(D#C)#
    __m128 Z_ = _mm_sub_ps(_mm_mul_ps(detC, B), Mat2MulAdj(A, D_C));

    const __m128 adjSignMask = _mm_setr_ps(1.f, -1.f, -1.f, 1.f);
    __m128 rDetM;

    // tr((A#B)(D#C))
    __m128 tr;
#	ifdef __SSE3__
    tr = _mm_mul_ps(A_B, VecSwizzle(D_C, 0,2,1,3));
    tr = _mm_hadd_ps(tr, tr);
    tr = _mm_hadd_ps(tr, tr);
#	else
    __m128 vTemp2 = VecSwizzle(D_C, 0,2,1,3);
    __m128 vTemp = _mm_mul_ps(A_B,vTemp2);
    vTemp2 = _mm_shuffle_ps(vTemp2,vTemp,_MM_SHUFFLE(1,0,0,0));
    vTemp2 = _mm_add_ps(vTemp2,vTemp);
    vTemp = _mm_shuffle_ps(vTemp,vTemp2,_MM_SHUFFLE(0,3,0,0));
    vTemp = _mm_add_ps(vTemp,vTemp2);
    tr = _mm_shuffle_ps(vTemp,vTemp,_MM_SHUFFLE(2,2,2,2));
#	endif

    // |M| = |A|*|D| + |B|*|C| - tr((A#B)(D#C))
    detM = _mm_add_ps(detM, _mm_mul_ps(detB, detC));
    detM = _mm_sub_ps(detM, tr);

    // (1/|M|, -1/|M|, -1/|M|, 1/|M|)
    rDetM = _mm_div_ps(adjSignMask, detM);

    X_ = _mm_mul_ps(X_, rDetM);
    Y_ = _mm_mul_ps(Y_, rDetM);
    Z_ = _mm_mul_ps(Z_, rDetM);
    W_ = _mm_mul_ps(W_, rDetM);

    // apply adjugate and store, here we combine adjugate shuffle and store shuffle
    NM_MM_STORE_PS(&mOut16[0], VecShuffle(X_, Z_, 3,1,3,1));
    NM_MM_STORE_PS(&mOut16[4], VecShuffle(X_, Z_, 2,0,2,0));
    NM_MM_STORE_PS(&mOut16[8], VecShuffle(Y_, W_, 3,1,3,1));
    NM_MM_STORE_PS(&mOut16[12],VecShuffle(Y_, W_, 2,0,2,0));

    return 1;
#   else // NM_USE_SIMD
    const nmoat* m = m16;
    nmoat* n = mOut16;

    nmoat m00 = m[0],  m10 = m[1],  m20 = m[2],  m30 = m[3];
    nmoat m01 = m[4],  m11 = m[5],  m21 = m[6],  m31 = m[7];
    nmoat m02 = m[8],  m12 = m[9],  m22 = m[10], m32 = m[11];
    nmoat m03 = m[12], m13 = m[13], m23 = m[14], m33 = m[15];

    nmoat v0 = m20 * m31 - m21 * m30;
    nmoat v1 = m20 * m32 - m22 * m30;
    nmoat v2 = m20 * m33 - m23 * m30;
    nmoat v3 = m21 * m32 - m22 * m31;
    nmoat v4 = m21 * m33 - m23 * m31;
    nmoat v5 = m22 * m33 - m23 * m32;

    nmoat t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    nmoat t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    nmoat t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    nmoat t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    nmoat det = (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
    if (det==(nmoat)0) return 0;
    {
        nmoat invDet = 1 / det;

        nmoat d00 = t00 * invDet;
        nmoat d10 = t10 * invDet;
        nmoat d20 = t20 * invDet;
        nmoat d30 = t30 * invDet;

        nmoat d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        nmoat d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        nmoat d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        nmoat d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;
        {
            nmoat d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            nmoat d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            nmoat d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            nmoat d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            v0 = m21 * m10 - m20 * m11;
            v1 = m22 * m10 - m20 * m12;
            v2 = m23 * m10 - m20 * m13;
            v3 = m22 * m11 - m21 * m12;
            v4 = m23 * m11 - m21 * m13;
            v5 = m23 * m12 - m22 * m13;
            {
                nmoat d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
                nmoat d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
                nmoat d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
                nmoat d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

                n[0] =d00; n[1] =d10; n[2] =d20; n[3] =d30;
                n[4] =d01; n[5] =d11; n[6] =d21; n[7] =d31;
                n[8] =d02; n[9] =d12; n[10]=d22; n[11]=d32;
                n[12]=d03; n[13]=d13; n[14]=d23; n[15]=d33;
            }
        }
    }
    return 1;
#   endif //NM_USE_SIMD
}
NM_API_IMPL int nm_Mat4InvertInPlace(nmoat* NM_RESTRICT m16)	{return nm_Mat4Invert(m16,m16);}
NM_API_IMPL nmoat* nm_Mat4InvertTransformMatrixWithoutScaling(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16)	{
    // It works only for translation + rotation, and only
    // when rotation can be represented by an unit quaternion
    // scaling is discarded
    // Ported from Eric's Blog: https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
#	if (defined(NM_USE_SIMD) && !defined(NM_DOUBLE_PRECISION) && defined(__SSE__))
    // use block matrix method
    // A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace
    __m128 col0 = NM_MM_LOAD_PS(&m16[0]);
    __m128 col1 = NM_MM_LOAD_PS(&m16[4]);
    __m128 col2 = NM_MM_LOAD_PS(&m16[8]);
    __m128 col3 = NM_MM_LOAD_PS(&m16[12]);

    __m128 rc0,rc1,rc2,rc3;

    // transpose 3x3, we know m03 = m13 = m23 = 0
    __m128 t0 = VecShuffle_0101(col0, col1); // 00, 01, 10, 11
    __m128 t1 = VecShuffle_2323(col0, col1); // 02, 03, 12, 13
    rc0 = VecShuffle(t0, col2, 0,2,0,3); // 00, 10, 20, 23(=0)
    rc1 = VecShuffle(t0, col2, 1,3,1,3); // 01, 11, 21, 23(=0)
    rc2 = VecShuffle(t1, col2, 0,2,2,3); // 02, 12, 22, 23(=0)

    //_MM_TRANSPOSE3_PS(col0,col1,col2);rc0 = col0;rc1 = col1;rc2 = col2;
    //col3 = _mm_setr_ps(0.f, 0.f, 0.f, 1.f);_MM_TRANSPOSE4_PS(col0,col1,col2,col3);rc0 = col0;rc1 = col1;rc2 = col2;col3 = NM_MM_LOAD_PS(&m16[12]);	// This line works! (Handy to make the double precision version work)

    // last line
    rc3 =                 _mm_mul_ps(rc0, VecSwizzle1(col3, 0));
    rc3 = _mm_add_ps(rc3, _mm_mul_ps(rc1, VecSwizzle1(col3, 1)));
    rc3 = _mm_add_ps(rc3, _mm_mul_ps(rc2, VecSwizzle1(col3, 2)));
    rc3 = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), rc3);

    NM_MM_STORE_PS(&mOut16[0], rc0);
    NM_MM_STORE_PS(&mOut16[4], rc1);
    NM_MM_STORE_PS(&mOut16[8], rc2);
    NM_MM_STORE_PS(&mOut16[12],rc3);
/*#	elif (defined(NM_USE_SIMD) && defined(NM_DOUBLE_PRECISION) && defined(__AVX__))
    // use block matrix method
    // A is a matrix, then i(A) or iA means inverse of A, A# (or A_ in code) means adjugate of A, |A| (or detA in code) is determinant, tr(A) is trace
    __m256d col0 = NM_MM256_LOAD_PD(&m16[0]);
    __m256d col1 = NM_MM256_LOAD_PD(&m16[4]);
    __m256d col2 = NM_MM256_LOAD_PD(&m16[8]);
    __m256d col3 = NM_MM256_LOAD_PD(&m16[12]);

    __m256d rc0,rc1,rc2,rc3;

    // transpose 3x3, we know m03 = m13 = m23 = 0
    __m256d t0 = VecShuffle_0101(col0, col1); // 00, 01, 10, 11		// WRONG!!
    __m256d t1 = VecShuffle_2323(col0, col1); // 02, 03, 12, 13
    rc0 = VecShuffle(t0, col2, 0,2,0,3); // 00, 10, 20, 23(=0)
    rc1 = VecShuffle(t0, col2, 1,3,1,3); // 01, 11, 21, 23(=0)
    rc2 = VecShuffle(t1, col2, 0,2,2,3); // 02, 12, 22, 23(=0)

    //_MM256_TRANSPOSE3_PD(col0,col1,col2);
    //rc0 = col0;rc1 = col1;rc2 = col2;

    // last line
    rc3 =                 _mm256_mul_pd(rc0, VecSwizzle1(col3, 0));
    rc3 = _mm256_add_pd(rc3, _mm256_mul_pd(rc1, VecSwizzle1(col3, 1)));
    rc3 = _mm256_add_pd(rc3, _mm256_mul_pd(rc2, VecSwizzle1(col3, 2)));
    rc3 = _mm256_add_pd(_mm256_setr_pd(0.0, 0.0, 0.0, 1.0), rc3);

    NM_MM256_STORE_PD(&result16[0], rc0);
    NM_MM256_STORE_PD(&result16[4], rc1);
    NM_MM256_STORE_PD(&result16[8], rc2);
    NM_MM256_STORE_PD(&result16[12],rc3);*/
#	else //NM_USE_SIMD
    /* reference implementation */
    /*		X	Y		Z		T
            |	|		|		|
        | m[0]	m[4]	m[8]	m[12] |				| m[0]	m[1]	m[2]	-T*X |
    M = | m[1]	m[5]	m[9]	m[13] |		Minv =	| m[4]	m[5]	m[6]	-T*Y |
        | m[2]	m[6]	m[10]	m[14] |				| m[8]	m[9]	m[10]	-T*Z |
        | 0		0		0		1     |				| 0		0		0		1    |
    */
    const nmoat* m = m16;
    nmoat* n = mOut16;
    const nmoat T[3] = {-m[12],-m[13],-m[14]};
    //nmoat w;
    // Step 1. Transpose the 3x3 submatrix
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step 2. Adjust translation
    n[12]=T[0]*m[0] + T[1]*m[1] +T[2]*m[2];
    n[13]=T[0]*m[4] + T[1]*m[5] +T[2]*m[6];
    n[14]=T[0]*m[8] + T[1]*m[9] +T[2]*m[10];
    //w    =T[0]*n[3] + T[1]*n[7] +T[2]*n[11];
    //if (w!=(nmoat)0 && w!=(nmoat)1) {n[12]/=w;n[13]/=w;n[14]/=w;} // These last 2 lines are not strictly necessary AFAIK
#	endif //NM_USE_SIMD
    return mOut16;
}
NM_API_IMPL nmoat* nm_Mat4InvertTransformMatrix(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16)	{
    // Ported from Eric's Blog: https://lxjk.github.io/2017/09/03/Fast-4x4-Matrix-Inverse-with-SSE-SIMD-Explained.html
#	if (defined(NM_USE_SIMD) && !defined(NM_DOUBLE_PRECISION) && defined(__SSE__))
    __m128 col0 = NM_MM_LOAD_PS(&m16[0]);
    __m128 col1 = NM_MM_LOAD_PS(&m16[4]);
    __m128 col2 = NM_MM_LOAD_PS(&m16[8]);
    __m128 col3 = NM_MM_LOAD_PS(&m16[12]);

    __m128 rc0,rc1,rc2,rc3,sizeSqr,one,rSizeSqr;

#	ifndef __SSE4_1__
    __m128 b0,b1,bcmp;
#	endif

    // transpose 3x3, we know m03 = m13 = m23 = 0
    __m128 t0 = VecShuffle_0101(col0, col1); // 00, 01, 10, 11
    __m128 t1 = VecShuffle_2323(col0, col1); // 02, 03, 12, 13
    rc0 = VecShuffle(t0, col2, 0,2,0,3); // 00, 10, 20, 23(=0)
    rc1 = VecShuffle(t0, col2, 1,3,1,3); // 01, 11, 21, 23(=0)
    rc2 = VecShuffle(t1, col2, 0,2,2,3); // 02, 12, 22, 23(=0)

    // (SizeSqr(mVec[0]), SizeSqr(mVec[1]), SizeSqr(mVec[2]), 0)
    sizeSqr =                     _mm_mul_ps(rc0, rc0);
    sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(rc1, rc1));
    sizeSqr = _mm_add_ps(sizeSqr, _mm_mul_ps(rc2, rc2));

    // optional test to avoid divide by 0
    one = _mm_set1_ps(1.f);
    // for each component, if(sizeSqr < SMALL_NUMBER) sizeSqr = 1;
#	ifdef __SSE4_1__
    rSizeSqr = _mm_blendv_ps(
        _mm_div_ps(one, sizeSqr),
        one,
        _mm_cmplt_ps(sizeSqr, _mm_set1_ps((1.e-8f)))	// #define SMALL_NUMBER		(1.e-8f)
        );
#	else
    bcmp = _mm_cmplt_ps(sizeSqr, _mm_set1_ps((1.e-8f)));	// #define SMALL_NUMBER		(1.e-8f)
    b0 = _mm_andnot_ps(bcmp, _mm_div_ps(one, sizeSqr));
    b1 = _mm_and_ps(bcmp, one);
    rSizeSqr = _mm_or_ps(b0, b1);       // d = wZ == -1.0 ? 1.0 / (1.0 - wZ) : 1.0 / (1.0 + wZ)
#	endif

    rc0 = _mm_mul_ps(rc0, rSizeSqr);
    rc1 = _mm_mul_ps(rc1, rSizeSqr);
    rc2 = _mm_mul_ps(rc2, rSizeSqr);

    // last line
    rc3 =                 _mm_mul_ps(rc0, VecSwizzle1(col3, 0));
    rc3 = _mm_add_ps(rc3, _mm_mul_ps(rc1, VecSwizzle1(col3, 1)));
    rc3 = _mm_add_ps(rc3, _mm_mul_ps(rc2, VecSwizzle1(col3, 2)));
    rc3 = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), rc3);

    NM_MM_STORE_PS(&mOut16[0], rc0);
    NM_MM_STORE_PS(&mOut16[4], rc1);
    NM_MM_STORE_PS(&mOut16[8], rc2);
    NM_MM_STORE_PS(&mOut16[12],rc3);
#	else //NM_USE_SIMD
    /* reference implementation */
    /*		X	Y		Z		T
            |	|		|		|
        | m[0]	m[4]	m[8]	m[12] |				| m[0]/|X^2|	m[1]/|X^2|	m[2]/|X^2|	-T*X/|X^2| |
    M = | m[1]	m[5]	m[9]	m[13] |		Minv =	| m[4]/|Y^2|	m[5]/|Y^2|	m[6]/|Y^2|	-T*Y/|Y^2| |
        | m[2]	m[6]	m[10]	m[14] |				| m[8]/|Z^2|	m[9]/|Z^2|	m[10]/|Z^2|	-T*Z/|Z^2| |
        | 0		0		0		1     |				| 0				0			0			1          |
    */
    const nmoat* m = m16;
    nmoat* n = mOut16;int i,i4,i4p1,i4p2;
    const nmoat T[3] = {-m[12],-m[13],-m[14]};
    // Step 1. Transpose the 3x3 submatrix
    n[3]=m[3];n[7]=m[7];n[11]=m[11];n[15]=m[15];
    n[0]=m[0];n[1]=m[4];n[2]=m[8];
    n[4]=m[1];n[5]=m[5];n[6]=m[9];
    n[8]=m[2];n[9]=m[6];n[10]=m[10];
    // Step 2. Adjust scaling
    for (i=0;i<3;i++)	{
        nmoat rsi;
        i4=4*i;i4p1=i4+1;i4p2=i4p1+1;
        rsi = m[i4]*m[i4]+m[i4p1]*m[i4p1]+m[i4p2]*m[i4p2];
        if (rsi>1.e-8f) rsi=(nmoat)1.0/rsi;
        else rsi=(nmoat)1.0;
        n[i]*=rsi;n[i+4]*=rsi;n[i+8]*=rsi;
    }
    // Step 3. Adjust translation
    n[12]=T[0]*n[0] + T[1]*n[4] +T[2]*n[8];
    n[13]=T[0]*n[1] + T[1]*n[5] +T[2]*n[9];
    n[14]=T[0]*n[2] + T[1]*n[6] +T[2]*n[10];
#	endif //NM_USE_SIMD
    return mOut16;
}
NM_API_IMPL void nm_Mat4InvertXZAxisInPlace(nmoat* NM_RESTRICT m16)	{
    nmoat* d = m16;
    d[0]=-d[0]; d[8] =-d[8];
    d[1]=-d[1]; d[9] =-d[9];
    d[2]=-d[2]; d[10]=-d[10];
}
NM_API_IMPL nmoat* nm_Mat4InvertXZAxis(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT m16)	{
    nmoat* d = mOut16;const nmoat* s = m16;
    d[0]=-s[0]; d[4]=s[4]; d[8] = -s[8]; d[12]=s[12];
    d[1]=-s[1]; d[5]=s[5]; d[9] = -s[9]; d[13]=s[13];
    d[2]=-s[2]; d[6]=s[6]; d[10]=-s[10]; d[14]=s[14];
    d[3]= s[3]; d[7]=s[7]; d[11]= s[11]; d[15]=s[15];
    return mOut16;
}
NM_API_IMPL nmoat* nm_QuatFromMat4(nmoat* NM_RESTRICT result4,const nmoat* NM_RESTRICT m16)  {
    // this code is glm based
    nmoat* q=result4;const nmoat* m=m16;
    nmoat *qx=&q[0],*qy=&q[1],*qz=&q[2],*qw=&q[3];
    const nmoat c00=m[0],c01=m[1],c02=m[2], c10=m[4],c11=m[5],c12=m[6], c20=m[8],c21=m[9],c22=m[10];

    nmoat fourXSquaredMinus1 = c00 - c11 - c22, fourYSquaredMinus1 = c11 - c00 - c22;
    nmoat fourZSquaredMinus1 = c22 - c00 - c11, fourWSquaredMinus1 = c00 + c11 + c22;
    nmoat biggestVal,mult,fourBiggestSquaredMinus1 = fourWSquaredMinus1;
    int biggestIndex = 0;

    if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourXSquaredMinus1;biggestIndex = 1;}
    if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourYSquaredMinus1;biggestIndex = 2;}
    if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)   {fourBiggestSquaredMinus1 = fourZSquaredMinus1;biggestIndex = 3;}

    biggestVal = nmh_sqrt(fourBiggestSquaredMinus1 + (nmoat)1) * (nmoat)0.5;
    mult = (nmoat)0.25 / biggestVal;

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
        //NM_ASSER(1);
        *qx=*qy=*qz=(nmoat)0;*qw=(nmoat)1;
        break;
    }
    return result4;
}
NM_API_IMPL nmoat* nm_Mat4SetRotationFromQuat(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT q4)    {
    // this code is glm based
    const nmoat one =(nmoat)1,two=(nmoat)2;
    nmoat* m=result16;const nmoat* q=q4;
    const nmoat qx=q[0],qy=q[1],qz=q[2],qw=q[3];
    nmoat *c00=&m[0],*c01=&m[1],*c02=&m[2], *c10=&m[4],*c11=&m[5],*c12=&m[6], *c20=&m[8],*c21=&m[9],*c22=&m[10];

    nmoat qxx = (qx * qx), qyy = (qy * qy), qzz = (qz * qz);
    nmoat qxz = (qx * qz), qxy = (qx * qy), qyz = (qy * qz);
    nmoat qwx = (qw * qx), qwy = (qw * qy), qwz = (qw * qz);

    *c00 = one - two * (qyy +  qzz); *c01 = two * (qxy + qwz); *c02 = two * (qxz - qwy);
    *c10 = two * (qxy - qwz); *c11 = one - two * (qxx +  qzz); *c12 = two * (qyz + qwx);
    *c20 = two * (qxz + qwy); *c21 = two * (qyz - qwx); *c22 = one - two * (qxx +  qyy);

    return result16;
}
NM_API_IMPL nmoat* nm_Mat3GetFromMat4(nmoat* NM_RESTRICT dst9,const nmoat* NM_RESTRICT src16)   {
    nmoat *d=dst9;const nmoat* s=src16;
    d[0]=s[0];   d[1]=s[1];   d[2]=s[2];
    d[3]=s[4];   d[4]=s[5];   d[5]=s[6];
    d[6]=s[8];   d[7]=s[9];   d[8]=s[10];
    return dst9;
}
NM_API_IMPL void nm_Mat4SetRotationFromMat3(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT src9)    {
    nmoat *d=dst16;const nmoat* s=src9;
    d[0]=s[0];   d[1]=s[1];   d[2]=s[2];
    d[4]=s[3];   d[5]=s[4];   d[6]=s[5];
    d[8]=s[6];   d[9]=s[7];   d[10]=s[8];
}
NM_API_IMPL const nmoat* nm_Mat4FromMRotScaTra(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT rot9,nmoat scaX,nmoat scaY,nmoat scaZ,nmoat traX,nmoat traY,nmoat traZ) {
    nmoat* d=dst16;
    nm_Mat4SetRotationFromMat3(d,rot9);
    d[3]=d[7]=d[11]=d[12]=d[13]=d[14]=(nmoat)0;d[15]=(nmoat)1;
    nm_Mat4Scale(d,scaX,scaY,scaZ);
    d[12]=traX;d[13]=traY;d[14]=traZ;
    return d;
}
NM_API_IMPL const nmoat* nm_Mat4FromQRotScaTra(nmoat* NM_RESTRICT dst16,const nmoat* NM_RESTRICT rot4,nmoat scaX,nmoat scaY,nmoat scaZ,nmoat traX,nmoat traY,nmoat traZ) {
    nmoat* d=dst16;
    nm_Mat4SetRotationFromQuat(d,rot4);
    d[3]=d[7]=d[11]=d[12]=d[13]=d[14]=(nmoat)0;d[15]=(nmoat)1;
    nm_Mat4Scale(d,scaX,scaY,scaZ);
    d[12]=traX;d[13]=traY;d[14]=traZ;
    return d;
}
NM_API_IMPL const nmoat* nm_Mat4SlerpEps(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT mStart16,const nmoat* NM_RESTRICT mEnd16,nmoat slerpTime_In_0_1,nmoat eps)  {
    if (slerpTime_In_0_1<=0)        {if (mOut16!=mStart16) nm_Mat4Copy(mOut16,mStart16);return mOut16;}
    else if (slerpTime_In_0_1>=1)   {if (mOut16!=mEnd16)   nm_Mat4Copy(mOut16,mEnd16); return mOut16;}


    {
        if (1)  {
            // Lerp positions
            const nmoat T1[3] = {mStart16[12],mStart16[13],mStart16[14]};
            const nmoat T2[3] = {mEnd16[12],mEnd16[13],mEnd16[14]};
            const nmoat t = slerpTime_In_0_1;
            const nmoat one_minus_t = 1.0-t;

            //mOut16[12] = T1[0]*one_minus_t + T2[0]*t;
            //mOut16[13] = T1[1]*one_minus_t + T2[1]*t;
            //mOut16[14] = T1[2]*one_minus_t + T2[2]*t;
            int i;for (i=0;i<3;i++) mOut16[12+i] = T1[i]*one_minus_t + T2[i]*t;
        }
        if (1)  {
            // Slerp rotations
            //const nmoat slerp_eps = NM_SLERP_EPSILON;//(nmoat)0.0001;
            nmoat qStart[4],qEnd[4],qSlerp[4];
            nm_Mat4SetRotationFromQuat(mOut16,nm_QuatSlerpEps(qSlerp,nm_QuatFromMat4(qStart,mStart16),nm_QuatFromMat4(qEnd,mEnd16),slerpTime_In_0_1,1,eps));
        }

    }

    return mOut16;
}
NM_API_IMPL const nmoat* nm_Mat4Slerp(nmoat* NM_RESTRICT mOut16,const nmoat* NM_RESTRICT mStart16,const nmoat* NM_RESTRICT mEnd16,nmoat slerpTime_In_0_1)  {return nm_Mat4SlerpEps(mOut16,mStart16,mEnd16,slerpTime_In_0_1,NM_SLERP_EPSILON);}
NM_API_IMPL nmoat* nm_Mat4Mul3(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16,const nmoat* NM_RESTRICT c16)   {nmoat tmp[16];return nm_Mat4Mul(result16,nm_Mat4Mul(tmp,a16,b16),c16);}
NM_API_IMPL nmoat* nm_Mat4Mul3_NoCheck(nmoat* NM_RESTRICT result16,const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16,const nmoat* NM_RESTRICT c16) {nmoat tmp[16];return nm_Mat4Mul_NoCheck(result16,nm_Mat4Mul_NoCheck(tmp,a16,b16),c16);}
NM_API_IMPL nmoat* nm_Mat4FromEulerXYZ(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z)   {nmoat tmp[9];return nm_Mat3FromEulerXYZ(tmp,x,y,z);nm_Mat4SetRotationFromMat3(m16,tmp);}
NM_API_IMPL nmoat* nm_Mat4FromEulerXZY(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z)   {nmoat tmp[9];return nm_Mat3FromEulerXZY(tmp,x,y,z);nm_Mat4SetRotationFromMat3(m16,tmp);}
NM_API_IMPL nmoat* nm_Mat4FromEulerYXZ(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z)   {nmoat tmp[9];return nm_Mat3FromEulerYXZ(tmp,x,y,z);nm_Mat4SetRotationFromMat3(m16,tmp);}
NM_API_IMPL nmoat* nm_Mat4FromEulerYZX(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z)   {nmoat tmp[9];return nm_Mat3FromEulerYZX(tmp,x,y,z);nm_Mat4SetRotationFromMat3(m16,tmp);}
NM_API_IMPL nmoat* nm_Mat4FromEulerZXY(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z)   {nmoat tmp[9];return nm_Mat3FromEulerZXY(tmp,x,y,z);nm_Mat4SetRotationFromMat3(m16,tmp);}
NM_API_IMPL nmoat* nm_Mat4FromEulerZYX(nmoat* NM_RESTRICT m16,nmoat x,nmoat y,nmoat z)   {nmoat tmp[9];return nm_Mat3FromEulerZYX(tmp,x,y,z);nm_Mat4SetRotationFromMat3(m16,tmp);}
NM_API_IMPL int nm_Mat4ToEulerXYZ(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m16;
    const nmoat c00=m[0],c01=m[1],c02=m[2], c11=m[5],c12=m[6], c21=m[9],c22=m[10];
    if (c02 < (nmoat)1) {
        if (c02 > -(nmoat)1)    {*y = nmh_asin(-c02); *z = nmh_atan2(c01, c00); *x = nmh_atan2(c12, c22); rv = 1;}
        else    {
            // Not a unique solution.
            *y = M_HALF_PI; *z = -nmh_atan2(-c21, c11); *x = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *y = -M_HALF_PI; *z = nmh_atan2(-c21, c11); *x = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat4ToEulerXZY(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m16;
    const nmoat c00=m[0],c02=m[2], c10=m[4],c11=m[5],c12=m[6], c20=m[8],c22=m[10];
    if (c12 < (nmoat)1) {
        if (c12 > -(nmoat)1)    {*x = nmh_asin(c12); *z = nmh_atan2(-c10, c11); *y = nmh_atan2(-c02, c22); rv = 1;}
        else    {
            // Not a unique solution.
            *x = -M_HALF_PI; *z = -nmh_atan2(-c20, c00); *y = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *x = M_HALF_PI; *z = nmh_atan2(c20, c00); *y = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat4ToEulerYXZ(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m16;
    const nmoat c00=m[0],c01=m[1],c02=m[2], c11=m[5],c12=m[6], c21=m[9],c22=m[10];
    if (c01 < (nmoat)1) {
        if (c01 > -(nmoat)1)    {
            *z = nmh_asin(c01);
            *y = -nmh_atan2(c02, c00);	// Modified: added minus sign
            *x = -nmh_atan2(c21, c11);	// Modified: added minus sign
            rv = 1;
        }
        else    {
            // Not a unique solution.
            *z = -M_HALF_PI; *y = -nmh_atan2(c12, c22); *x = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *z = M_HALF_PI; *y = nmh_atan2(-c12, c22); *x = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat4ToEulerYZX(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m16;
    const nmoat c00=m[0],c01=m[1], c10=m[4],c11=m[5], c20=m[8],c21=m[9],c22=m[10];
    if (c21 < (nmoat)1) {
        if (c21 > -(nmoat)1)    {*x = nmh_asin(-c21); *y = nmh_atan2(c20, c22); *z = nmh_atan2(c01, c11); rv = 1;}
        else    {
            // Not a unique solution.
            *x = M_HALF_PI; *y = -nmh_atan2(-c10, c00); *z = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution.
        *x = -M_HALF_PI; *y = nmh_atan2(-c10, c00); *z = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat4ToEulerZXY(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m16;
    const nmoat c00=m[0],c02=m[2], c10=m[4],c11=m[5],c12=m[6], c20=m[8],c22=m[10];
    if (c10 < (nmoat)1) {
        if (c10 > -(nmoat)1)    {*z = nmh_asin(-c10); *x = nmh_atan2(c12, c11); *y = nmh_atan2(c20, c00); rv = 1;}
        else    {
            // Not a unique solution: y - x = atan2(-c02, c22);
            *z = M_HALF_PI; *x = nmh_atan2(-c02, c22); *y = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution: y + x = atan2(-c02, c22);
        *z = -M_HALF_PI; *x = nmh_atan2(-c02, c22); *y = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL int nm_Mat4ToEulerZYX(const nmoat* NM_RESTRICT m16,nmoat* x,nmoat* y,nmoat* z)    {
    int rv = 0;const nmoat* m=m16;
    const nmoat c00=m[0],c01=m[1], c10=m[4],c11=m[5], c20=m[8],c21=m[9],c22=m[10];
    if (c20 < (nmoat)1) {
        if (c20 > -(nmoat)1)    {*y = nmh_asin(c20);*x = nmh_atan2(-c21, c22);*z = nmh_atan2(-c10, c00);rv = 1;}
        else    {
            // Not a unique solution: z - x = atan2(c01, c11);
            *y = -M_HALF_PI; *x = -nmh_atan2(c01, c11); *z = (nmoat)0;
        }
    }
    else    {
        // Not a unique solution: z + x = atan2(c01, c11);
        *y = M_HALF_PI; *x = nmh_atan2(c01, c11); *z = (nmoat)0;
    }
    return rv;
}
NM_API_IMPL void nm_Mat4FromEuler(nmoat* m16,nmoat x,nmoat y,nmoat z,int eulerRotationMode)   {
    NM_ASSERT(eulerRotationMode>=1 && eulerRotationMode<=6);
    switch (eulerRotationMode) {
    case 1://EULER_XYZ
        nm_Mat4FromEulerXYZ(m16,x,y,z);
        break;
    case 2://EULER_XZY
        nm_Mat4FromEulerXZY(m16,x,y,z);
        break;
    case 3://EULER_YXZ
        nm_Mat4FromEulerYXZ(m16,x,y,z);
        break;
    case 4://EULER_YZX
        nm_Mat4FromEulerYZX(m16,x,y,z);
        break;
    case 5://EULER_ZXY
        nm_Mat4FromEulerZXY(m16,x,y,z);
        break;
    case 6://EULER_ZYX
        nm_Mat4FromEulerZYX(m16,x,y,z);
        break;
    default:
        //NM_ASSERT(1);   // wrong mode
        nm_Mat4FromEulerXYZ(m16,x,y,z);
        break;
    }
}
NM_API_IMPL int nm_Mat4ToEuler(const nmoat* m16,nmoat* x,nmoat* y,nmoat* z,int eulerRotationMode)  {
    NM_ASSERT(eulerRotationMode>=1 && eulerRotationMode<=6);
    switch (eulerRotationMode) {
    case 1://EULER_XYZ
        return nm_Mat4ToEulerXYZ(m16,x,y,z);
        break;
    case 2://EULER_XZY
        return nm_Mat4ToEulerXZY(m16,x,y,z);
        break;
    case 3://EULER_YXZ
        return nm_Mat4ToEulerYXZ(m16,x,y,z);
        break;
    case 4://EULER_YZX
        return nm_Mat4ToEulerYZX(m16,x,y,z);
        break;
    case 5://EULER_ZXY
        return nm_Mat4ToEulerZXY(m16,x,y,z);
        break;
    case 6://EULER_ZYX
        return nm_Mat4ToEulerZYX(m16,x,y,z);
        break;
    default:
        //NM_ASSERT(1);   // wrong mode
        return nm_Mat4ToEulerYXZ(m16,x,y,z);
        break;
    }
    return 0;
}
NM_API_IMPL const nmoat* nm_Mat4LookAtYX(nmoat* NM_RESTRICT mInOut16,nmoat lookAtX,nmoat lookAtY,nmoat lookAtZ,nmoat minDistanceAllowed/*=0*/,nmoat maxDistanceAllowed/*=0*/,nmoat pitchLimit/* = 1.483486111*/)    {
    nmoat* d=mInOut16;
    nmoat D[3]={lookAtX-d[12],lookAtY-d[13],lookAtZ-d[14]};
    nmoat Dxz2=D[0]*D[0]+D[2]*D[2];
    nmoat Dxz=nmh_sqrt(Dxz2);
    nmoat pitch = -nmh_atan2(D[1],Dxz);
    if (pitchLimit<0) pitchLimit = (nmoat) 1.483486111; // default calue close to 90° (AFAIR)
    if (pitch<-pitchLimit) pitch = -pitchLimit;
    else if (pitch>pitchLimit) pitch = pitchLimit;

    nm_Mat4FromEulerYXZ(d,nmh_atan2(D[0],D[2]),pitch,0);

    if (minDistanceAllowed<maxDistanceAllowed)	{
        const nmoat distance=nmh_sqrt(Dxz2+D[1]*D[1]);
        //if (distance<minDistance)       {
            const nmoat diff=minDistanceAllowed-distance;
            d[12]-=d[8]*diff;d[13]-=d[9]*diff;d[14]-=d[10]*diff; // tra-=zAxis*(minDistance-distance)
        /*}
        else if (distance>maxDistance)  {
            const nmoat diff=distance-maxDistance;
            d[12]+=d[8]*diff;d[13]+=d[9]*diff;d[14]+=d[10]*diff; // tra+=zAxis*(distance-maxDistance)
        }*/
    }
    return d;
}
NM_API_IMPL const nmoat* nm_Mat4LookAtYX2D(nmoat* NM_RESTRICT mInOut16,nmoat lookAtX,nmoat lookAtZ,nmoat minDistanceAllowed/*=0*/,nmoat maxDistanceAllowed/*=0*/,nmoat pitchLimit/* = 1.483486111*/)    {return nm_Mat4LookAtYX(mInOut16,lookAtX,mInOut16[13],lookAtZ,minDistanceAllowed,maxDistanceAllowed,pitchLimit);}

NM_API_IMPL void nm_Mat4MulDir(const nmoat* NM_RESTRICT m16,nmoat* NM_RESTRICT dirOut3,const nmoat dirX,nmoat dirY,nmoat dirZ) {
    //nmoat w;
    dirOut3[0] = dirX*m16[0] + dirY*m16[4] + dirZ*m16[8];
    dirOut3[1] = dirX*m16[1] + dirY*m16[5] + dirZ*m16[9];
    dirOut3[2] = dirX*m16[2] + dirY*m16[6] + dirZ*m16[10];
    //w          = dirX*m16[3] + dirY*m16[7] + dirZ*m16[11]; // + m[15] ?
    //if (w!=0 && w!=1) {dirOut3[0]/=w;dirOut3[1]/=w;dirOut3[2]/=w;}
}
NM_API_IMPL void nm_Mat4MulPos(const nmoat* NM_RESTRICT m16,nmoat* NM_RESTRICT posOut3,const nmoat posX,nmoat posY,nmoat posZ) {
    nmoat w;
    posOut3[0] = posX*m16[0] + posY*m16[4] + posZ*m16[8] + m16[12];
    posOut3[1] = posX*m16[1] + posY*m16[5] + posZ*m16[9] + m16[13];
    posOut3[2] = posX*m16[2] + posY*m16[6] + posZ*m16[10]+ m16[14];
    w          = posX*m16[3] + posY*m16[7] + posZ*m16[11]+ m16[15];
    if (w!=(nmoat)0 && w!=(nmoat)1) {posOut3[0]/=w;posOut3[1]/=w;posOut3[2]/=w;}
}


NM_API_IMPL int nm_AreVec3EqualEps3(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3,const nmoat* NM_RESTRICT eps3)  {
    return (nmh_fabs(a3[0]-b3[0])<eps3[0] && nmh_fabs(a3[1]-b3[1])<eps3[1] && nmh_fabs(a3[2]-b3[2])<eps3[2])?1:0;
    //const nmoat d[3] = {a3[0]-b3[0],a3[1]-b3[1],a3[2]-b3[2]};
    //return d[0]*d[0]+d[1]*d[1]+d[2]*d[2]<eps3[0]*eps3[0]+eps3[1]*eps3[1]+eps3[2]*eps3[2];
}
/*NM_API_IMPL int nm_AreVec3EqualEps(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3,nmoat eps) {
    const nmoat eps3[3]={eps,eps,eps};
    return nm_AreVec3EqualEps3(a3,b3,eps3);
}*/
NM_API_IMPL int nm_AreVec3EqualEpsRadius(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3,nmoat epsRadiusSquared) {
    const nmoat d[3] = {a3[0]-b3[0],a3[1]-b3[1],a3[2]-b3[2]};
    return (d[0]*d[0]+d[1]*d[1]+d[2]*d[2]<epsRadiusSquared)?1:0;
}
NM_API_IMPL int nm_AreVec2EqualEps2(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2,const nmoat* NM_RESTRICT eps2)  {
    return (nmh_fabs(a2[0]-b2[0])<eps2[0] && nmh_fabs(a2[1]-b2[1])<eps2[1])?1:0;
}
NM_API_IMPL int nm_AreVec2EqualEpsRadius(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2,nmoat epsRadiusSquared) {
    const nmoat d[2] = {a2[0]-b2[0],a2[1]-b2[1]};
    return (d[0]*d[0]+d[1]*d[1]<epsRadiusSquared)?1:0;
}
/*NM_API_IMPL int nm_AreVec2EqualEps(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2,nmoat eps) {
    const nmoat eps2[2]={eps,eps};
    return nm_AreVec2EqualEps2(a2,b2,eps2);
}*/

NM_API_IMPL int nm_AreMat4EqualEps(const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16,nmoat eps)  {return nm_AreEqualEps(a16,b16,16,eps);}
NM_API_IMPL int nm_AreMat4Equal(const nmoat* NM_RESTRICT a16,const nmoat* NM_RESTRICT b16)               {return nm_AreEqual(a16,b16,16);}

NM_API_IMPL int nm_AreMat3EqualEps(const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9,nmoat eps)  {return nm_AreEqualEps(a9,b9,9,eps);}
NM_API_IMPL int nm_AreMat3Equal(const nmoat* NM_RESTRICT a9,const nmoat* NM_RESTRICT b9)               {return nm_AreEqual(a9,b9,9);}

NM_API_IMPL int nm_AreQuatEqualEps(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat eps)  {
    /* Quat Hints from the web

    WHEN TWO QUATERNIONS ARE SIMILAR:
    =================================
    With quaternions, it’s exactly the same as vectors:

    float matching = quaternion::dot(q1, q2);
    if ( abs(matching-1.0) < 0.001 ){
        // q1 and q2 are similar
    }

    You can also get the angle between q1 and q2 by taking the acos() of this dot product.

    */
    // But here we're just checking if a,b are equal or a,-b are equal...
    int i,r=1;
    for (i=0;i<4;i++) {if (nmh_fabs(a4[i]-b4[i])>eps) {r=0;break;}}
    if (r==0)   {for (i=0;i<4;i++) {if (nmh_fabs(a4[i]+b4[i])>eps) {r=0;break;}}}
    return r;
}
NM_API_IMPL int nm_AreQuatEqual(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4)               {return nm_AreQuatEqualEps(a4,b4,NM_EPSILON);}

NM_API_IMPL int nm_AreVec4EqualEps(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4,nmoat eps)  {return nm_AreEqualEps(a4,b4,4,eps);}
NM_API_IMPL int nm_AreVec4Equal(const nmoat* NM_RESTRICT a4,const nmoat* NM_RESTRICT b4)               {return nm_AreEqual(a4,b4,4);}

NM_API_IMPL int nm_AreVec3EqualEps(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3,nmoat eps)  {return nm_AreEqualEps(a3,b3,3,eps);}
NM_API_IMPL int nm_AreVec3Equal(const nmoat* NM_RESTRICT a3,const nmoat* NM_RESTRICT b3)               {return nm_AreEqual(a3,b3,3);}

NM_API_IMPL int nm_AreVec2EqualEps(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2,nmoat eps)  {return nm_AreEqualEps(a2,b2,2,eps);}
NM_API_IMPL int nm_AreVec2Equal(const nmoat* NM_RESTRICT a2,const nmoat* NM_RESTRICT b2)               {return nm_AreEqual(a2,b2,2);}


NM_API_IMPL void nm_PlaneNormalize(nmoat* NM_RESTRICT p4) {
    int i;nmoat len = p4[0]*p4[0]+p4[1]*p4[1]+p4[2]*p4[2];
    if (len!=0) {
        len = nmh_sqrt(len);
        for (i=0;i<4;i++) p4[i]/=len;
    }
}
NM_API_IMPL nmoat nm_PlanePointDistance(const nmoat* NM_RESTRICT plane4,const nmoat* NM_RESTRICT pt3) {
    nmoat rv = plane4[3];
    int i;for (i=0;i<3;i++) rv+=plane4[i]*pt3[i];
    return rv;
}

// Warning: 'cascadeNearAndFarPlanesArrayOut' (and 'optionalCascadeNearAndFarPlanesInClipSpaceArrayOut' if used) must contain 'numCascades+1' elements
// (returned cascadeNearAndFarPlanesArrayOut[0]==cameraNearClippingPlane and cascadeNearAndFarPlanesArrayOut[numCascades]==cameraFarClippingPlane)
// (returned optionalCascadeNearAndFarPlanesInClipSpaceArrayOut[0]==0 and optionalCascadeNearAndFarPlanesInClipSpaceArrayOut[numCascades]==1)
NM_API_PRIV_INL void nm_GetCascadeNearAndFarClippingPlaneArray(nmoat*  NM_RESTRICT cascadeNearAndFarPlanesArrayOut,size_t numCascades,nmoat lambda,nmoat cameraNearClippingPlane,nmoat cameraFarClippingPlane,nmoat* NM_RESTRICT optionalCascadeNearAndFarPlanesInClipSpaceArrayOut) {
    size_t i;
    NM_ASSERT(numCascades>0);
    NM_ASSERT(cameraNearClippingPlane<cameraFarClippingPlane);
    cascadeNearAndFarPlanesArrayOut[0]=cameraNearClippingPlane;
    if (optionalCascadeNearAndFarPlanesInClipSpaceArrayOut) optionalCascadeNearAndFarPlanesInClipSpaceArrayOut[0] = (nmoat)0;
    for (i=0;i<numCascades;i++) {
        nmoat p = (nmoat)(i+1)/(nmoat)(numCascades);
        nmoat logValue = cameraNearClippingPlane * nmh_pow(cameraFarClippingPlane/cameraNearClippingPlane, p);
        nmoat uniformValue = cameraNearClippingPlane + (cameraFarClippingPlane-cameraNearClippingPlane)*p;
        nmoat d = lambda*(logValue-uniformValue)+uniformValue;
        // cascadeSplit: clip planes in (0,1]. This is converted from Direct3D... maybe in OpenGL they should be in [-1,1]... in any case we don't return these directly ATM.
        nmoat cascadeSplit = (d-cameraNearClippingPlane)/(cameraFarClippingPlane-cameraNearClippingPlane);
        if (optionalCascadeNearAndFarPlanesInClipSpaceArrayOut) optionalCascadeNearAndFarPlanesInClipSpaceArrayOut[i+1] = cascadeSplit;
        // Store cascadeFarPlanesArrayOut in OpenGL units in (cameraNearClippingPlane,cameraFarClippingPlane]
        cascadeNearAndFarPlanesArrayOut[i+1] = (cameraNearClippingPlane + cascadeSplit * (cameraFarClippingPlane - cameraNearClippingPlane));
        //fprintf(stderr,"cascadeSplits[%d] = %1.4f\tcascadeNearAndFarPlanesArrayOut[%d] = %1.4f\n",i+1,cascadeSplit,i+1,cascadeNearAndFarPlanesArrayOut[i+1]);
    }
    cascadeNearAndFarPlanesArrayOut[numCascades]=cameraFarClippingPlane;    // redundant
    if (optionalCascadeNearAndFarPlanesInClipSpaceArrayOut) optionalCascadeNearAndFarPlanesInClipSpaceArrayOut[numCascades] = (nmoat)1; // redundant
}
NM_API_PRIV_INL void nm_NormalizePlane(nmoat* NM_RESTRICT p4) {
    nmoat len = p4[0]*p4[0]+p4[1]*p4[1]+p4[2]*p4[2];
    if (len!=0) {
        int i;len = nmh_sqrt(len);
        for (i=0;i<4;i++) p4[i]/=len;
    }
}
NM_API_IMPL void nm_GetFrustumPlaneEquations(nmoat planeEquationsOut[6][4],const nmoat* NM_RESTRICT vpMatrix16,int normalizePlanes)   {
    // ax+by+cz+d=0 [xl,xr,yb,yt,zn,zf],normalizePlanes=0 -> no normalization
    nmoat m00 = vpMatrix16[0], m01 = vpMatrix16[4], m02 = vpMatrix16[8],  m03 = vpMatrix16[12];
    nmoat m10 = vpMatrix16[1], m11 = vpMatrix16[5], m12 = vpMatrix16[9],  m13 = vpMatrix16[13];
    nmoat m20 = vpMatrix16[2], m21 = vpMatrix16[6], m22 = vpMatrix16[10], m23 = vpMatrix16[14];
    nmoat m30 = vpMatrix16[3], m31 = vpMatrix16[7], m32 = vpMatrix16[11], m33 = vpMatrix16[15];
    nmoat* p = NULL;
    p = &planeEquationsOut[0][0];   // Left
    p[0] = m30+m00; p[1] = m31+m01; p[2] = m32+m02; p[3] = m33+m03;if (normalizePlanes) nm_NormalizePlane(p);
    p = &planeEquationsOut[1][0];   // Right
    p[0] = m30-m00; p[1] = m31-m01; p[2] = m32-m02; p[3] = m33-m03;if (normalizePlanes) nm_NormalizePlane(p);
    p = &planeEquationsOut[2][0];   // Bottom
    p[0] = m30+m10; p[1] = m31+m11; p[2] = m32+m12; p[3] = m33+m13;if (normalizePlanes) nm_NormalizePlane(p);
    p = &planeEquationsOut[3][0];   // Top
    p[0] = m30-m10; p[1] = m31-m11; p[2] = m32-m12; p[3] = m33-m13;if (normalizePlanes) nm_NormalizePlane(p);
    p = &planeEquationsOut[4][0];   // Near
    p[0] = m30+m20; p[1] = m31+m21; p[2] = m32+m22; p[3] = m33+m23;if (normalizePlanes) nm_NormalizePlane(p);
    p = &planeEquationsOut[5][0];   // Far
    p[0] = m30-m20; p[1] = m31-m21; p[2] = m32-m22; p[3] = m33-m23;if (normalizePlanes) nm_NormalizePlane(p);

    // Dbg: test that Near is parallel to Far
    /*if (normalizePlanes) {
        p = &planeEquationsOut[4][0];
        printf("NearFrustumPlane: %1.4f,%1.4f,%1.4f,%1.4f\n",p[0],p[1],p[2],p[3]);
        p = &planeEquationsOut[5][0];
        printf("FarFrustumPlane:  %1.4f,%1.4f,%1.4f,%1.4f\n",p[0],p[1],p[2],p[3]);
        // Examples:
        // Camera vpMatrix [pMatrix: FovyDeg = 45.;NearPlane = 2.5;FarPlane = 75.;]
        // NearFrustumPlane: -0.5230,-0.6730,-0.5230,25.3856
        // FarFrustumPlane:  0.5230,0.6730,0.5230,98.8233
        // Shadow Frustum lvpMatrixForShadowFrustumCulling:
        // NearFrustumPlane: -0.0000,-0.3827,-0.9239,12.5000
        // FarFrustumPlane:  0.0000,0.3827,0.9239,59.9999

        // Hint: maybe we could use 'optionalCascadeNearAndFarPlanesInClipSpaceArrayOut' (size=num_cascades+1; in (0,1])
        // to add plane equations for cascade splits [basic adding other planes parallel to Near/Far].
        // But it's probably better to pass 'optionalCascadeNearAndFarPlanesInClipSpaceArrayOut' to IsAabbVisible() directly
    }*/
}
NM_API_IMPL void nm_GetFrustumPoints(nmoat frustumPoints[8][4],const nmoat* NM_RESTRICT vpMatrixInverse16)    {
    const nmoat v[8][4] = {{-1, -1, -1, 1},{-1,  1, -1, 1},{ 1,  1, -1, 1},{ 1, -1, -1, 1},{-1, -1, 1, 1},{-1,  1, 1, 1},{ 1,  1, 1, 1},{ 1, -1, 1, 1}};
    int i;for (i = 0; i < 8; i++) {
        nm_Mat4MulPos(vpMatrixInverse16,frustumPoints[i],v[i][0],v[i][1],v[i][2]);
        frustumPoints[i][3]=1;
    }
}
NM_API_IMPL void nm_GetFrustumAabbCenterAndHalfExtents(nmoat* NM_RESTRICT frustumCenterOut3,nmoat* NM_RESTRICT frustumHalfExtentsOut3,const nmoat frustumPoints[8][4])    {
    nmoat vmin[3] = {frustumPoints[0][0],frustumPoints[0][1],frustumPoints[0][2]};
    nmoat vmax[3] = {vmin[0],vmin[1],vmin[2]};
    int i,j;
    for (i = 1; i < 8; i++) {
        for (j = 0; j < 3; j++) {
            if      (vmin[j] > frustumPoints[i][j]) vmin[j] = frustumPoints[i][j];
            else if (vmax[j] < frustumPoints[i][j]) vmax[j] = frustumPoints[i][j];
        }
    }
    for (j = 0; j < 3; j++) {
        if (frustumCenterOut3)      frustumCenterOut3[j] = (vmin[j]+vmax[j])*(nmoat)0.5;
        if (frustumHalfExtentsOut3) frustumHalfExtentsOut3[j] = (vmax[j]-vmin[j])*(nmoat)0.5;
    }
}

// returns the frustum radius (by value) and the scalar (positive) distance from the the camera eye to the frustum center (by 'pFrustumCenterDistanceOut').
// 'cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero': the arg name is correct when the result is passed to 'nm_GetLightViewProjectionMatrix' functions.
// Otherwise simply set it when using an ortho3D camera frustum (that needs a camera-target distance), or just set it to zero.
NM_API_IMPL nmoat nm_GetFrustumRadiusAndCenterDistance(nmoat* NM_RESTRICT pFrustumCenterDistanceOut,nmoat cameraNearClippingPlane,nmoat cameraFarClippingPlane,nmoat cameraFovyDeg,nmoat cameraAspectRatio,nmoat cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero)    {
    nmoat radius,frustumCenterDistance;
    nmoat tanFovDiagonalSquared = nmh_tan(cameraFovyDeg*(nmoat)(3.14159265358979323846/360.0)); // At this point this is just TANFOVY
    const nmoat halfNearFarClippingPlane = (nmoat)0.5*(cameraFarClippingPlane+cameraNearClippingPlane);

    if (cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero>cameraFarClippingPlane) cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero = 0;  // Not needed

    // Get frustumCenter and radius
    if (cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero<=0)  {
        // camera perspective mode here
        tanFovDiagonalSquared*=tanFovDiagonalSquared;
        tanFovDiagonalSquared*=((nmoat)1+cameraAspectRatio*cameraAspectRatio);
        frustumCenterDistance = halfNearFarClippingPlane*((nmoat)1+tanFovDiagonalSquared);
        if (frustumCenterDistance > cameraFarClippingPlane) frustumCenterDistance = cameraFarClippingPlane;
        radius = (tanFovDiagonalSquared*cameraFarClippingPlane*cameraFarClippingPlane) + (cameraFarClippingPlane-frustumCenterDistance)*(cameraFarClippingPlane-frustumCenterDistance); // This is actually radiusSquared
    }
    else {
        // camera ortho3d mode here
        const nmoat y=cameraTargetDistanceForUnstableOrtho3DModeOnly_or_zero*tanFovDiagonalSquared;
        const nmoat x=y*cameraAspectRatio;
        const nmoat halfClippingPlaneDistance = (nmoat)(0.5)*(cameraFarClippingPlane-cameraNearClippingPlane);
        frustumCenterDistance = halfNearFarClippingPlane;
        radius = x*x+y*y; // This is actually radiusXYSquared
        radius = radius + halfClippingPlaneDistance*halfClippingPlaneDistance;// This is actually radiusSquared
    }
    radius = nmh_sqrt(radius);
    //fprintf(stderr,"radius=%1.4f frustumCenterDistance=%1.4f nearPlane=%1.4f farPlane = %1.4f\n",radius,frustumCenterDistance,cameraNearClippingPlane,cameraFarClippingPlane);
    if (pFrustumCenterDistanceOut) *pFrustumCenterDistanceOut = frustumCenterDistance;
    return radius;

    // For people trying to save texture space it's:  halfNearFarClippingPlane <= frustumCenterDistance <= cameraFarClippingPlane
    // When frustumCenterDistance == cameraFarClippingPlane, then frustumCenter is on the far clip plane (and half the texture space gets wasted).
    // when frustumCenterDistance == halfNearFarClippingPlane, then we're using an ortho projection matrix, and frustumCenter is in the middle of the near and far plane (no texture space gets wasted).
    // in all the other cases the space wasted can go from zero to half texture
}
// 'frustumCenterOut3' is in world space (mandatory arg).
// 'cameraVMatrixInverse16' must be orthonormal (no scaling).
NM_API_IMPL void nm_GetFrustumCenterFromCenterDistance(nmoat* NM_RESTRICT frustumCenterOut3,const nmoat* NM_RESTRICT cameraVMatrixInverse16,nmoat frustumCenterDistance)    {
    const nmoat cameraPosition3[3] = {cameraVMatrixInverse16[12],cameraVMatrixInverse16[13],cameraVMatrixInverse16[14]};
    const nmoat cameraForwardDirection3[3] = {-cameraVMatrixInverse16[8],-cameraVMatrixInverse16[9],-cameraVMatrixInverse16[10]};
    int i;if (frustumCenterOut3)  {for (i=0;i<3;i++) frustumCenterOut3[i] = cameraPosition3[i] + cameraForwardDirection3[i]*frustumCenterDistance;}
}

// 'optionalPMatrixInverse16' is required only if you need to retrieve (one or more of) the arguments that follow them (otherwise their value is untouched).
// 'normalizedLightDirection3' must be in world space.
NM_API_IMPL void nm_GetLightViewProjectionMatrixExtra(nmoat* NM_RESTRICT lvpMatrixOut16
                                                          ,const nmoat* NM_RESTRICT cameraVMatrixInverse16,const nmoat* NM_RESTRICT cameraFrustumCenterInWorldSpace,nmoat cameraFrustumRadius
                                                          ,const nmoat*  NM_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight
                                                          ,const nmoat* NM_RESTRICT optionalCameraPMatrixInverse16
                                                          ,nmoat* NM_RESTRICT optionalLightViewportClippingOut4,nmoat optionalCameraFrustumPointsInNDCLightSpaceOut[8][4]
                                                          ,nmoat* NM_RESTRICT optionalLVPMatrixForFrustumCullingUsageOut16   // Highly experimental and untested
                                                          )  {
    nmoat lpMatrix[16],lvMatrix[16],lvpMatrixFallback[16];
    const nmoat texelIncrement = shadowMapWidth>=shadowMapHeight ? (nmoat) 1.0/(nmoat)shadowMapHeight : (nmoat) 1.0/(nmoat)shadowMapWidth;
    const nmoat cameraLeftDirection3[3] = {-cameraVMatrixInverse16[0],-cameraVMatrixInverse16[1],-cameraVMatrixInverse16[2]};
    nmoat dotCameraXDirectionLightDirection = 0, lightUpVector3[3]={0,1,0};
    int i;
    if (lvpMatrixOut16==0) lvpMatrixOut16=lvpMatrixFallback;    // AFAIK from the caller point of view it's still lvpMatrixOut16==0, isn't it?

    // Shadow swimming happens when: 1) camera translates; 2) camera rotates; 3) objects move or rotate
    // AFAIK Shadow swimming (3) can't be fixed in any way
    if (texelIncrement>0)   cameraFrustumRadius = nmh_ceil(cameraFrustumRadius/texelIncrement)*texelIncrement;      // This 'should' fix Shadow swimming (1)  [Not sure code is correct!]

    // Get light matrices
    nm_Mat4Ortho(lpMatrix,-cameraFrustumRadius,cameraFrustumRadius,-cameraFrustumRadius,cameraFrustumRadius,0,-(nmoat)2*cameraFrustumRadius);
    if (shadowMapWidth!=shadowMapHeight)    {
        if (lvpMatrixOut16!=lvpMatrixFallback)  {
            // User wants stable shadow mapping... (with popping artifacts)
            // Good (stable with popping artifacts)
            dotCameraXDirectionLightDirection = nmh_fabs(nm_Vec3Dot(cameraLeftDirection3,normalizedLightDirection3));
            if (dotCameraXDirectionLightDirection>(nmoat)0.5) {lightUpVector3[0]=0;lightUpVector3[1]=0;lightUpVector3[2]=1;}
        }
        else {
            // User wants unstable shadow mapping

            /* // Good (unstable without popping)
            dotCameraXDirectionLightDirection = nm_Vector3Dot(cameraLeftDirection3,normalizedLightDirection3);
            lightUpVector3[0]=0;
            lightUpVector3[2]=dotCameraXDirectionLightDirection;
            lightUpVector3[1]=sqrt(1.0-dotCameraXDirectionLightDirection*dotCameraXDirectionLightDirection);
            */
            // Smoother (unstable without popping)
            nm_Vec3Cross(lightUpVector3,cameraLeftDirection3,normalizedLightDirection3);
            /* // Unfinished attempt.
        {
            nmoat tmp[3],tmp2[3];
            // Let's find 'tmp2', the projection of 'normalizedLightDirection3' on the camera plane XY:
            nm_Vector3Cross(tmp,normalizedLightDirection3,cameraForwardDirection3);
            nm_Vector3Cross(tmp2,cameraForwardDirection3,tmp);
        }
        */
        }
    }
    nm_Mat4LookAt(lvMatrix,
            cameraFrustumCenterInWorldSpace[0]-normalizedLightDirection3[0]*cameraFrustumRadius,   // eye[0]
            cameraFrustumCenterInWorldSpace[1]-normalizedLightDirection3[1]*cameraFrustumRadius,   // eye[1]
            cameraFrustumCenterInWorldSpace[2]-normalizedLightDirection3[2]*cameraFrustumRadius,   // eye[2]
            cameraFrustumCenterInWorldSpace[0],cameraFrustumCenterInWorldSpace[1],cameraFrustumCenterInWorldSpace[2],     // target
            lightUpVector3[0],lightUpVector3[1],lightUpVector3[2]                                                   // up (people that cares about wasted texture space can probably change it)
            );
    // Get output
    nm_Mat4Mul(lvpMatrixOut16,lpMatrix,lvMatrix);

    // This 'should' fix Shadow swimming (2) [Not sure code is correct!]
    if (texelIncrement>0)   {
        nmoat shadowOrigin[4]   = {0,0,0,1};
        nmoat roundedOrigin[4]  = {0,0,0,0};
        nmoat roundOffset[4]    = {0,0,0,0};
        nmoat texelCoefficient = texelIncrement*(nmoat)2;
        nm_Mat4MulPos(lvpMatrixOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
        for (i = 0; i < 2; i++) {// Or i<3 ?
            shadowOrigin[i]/= texelCoefficient;
            roundedOrigin[i] = nmh_round(shadowOrigin[i]);
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
        nmoat cameraVPMatrixInv[16],cameraVPMatrixInverseAdjusted[16];nmoat frustumPoints[8][4];
        nmoat minVal[3],maxVal[3],tmp;
        nm_Mat4Mul(cameraVPMatrixInv,cameraVMatrixInverse16,optionalCameraPMatrixInverse16); // vMatrixInverse16 needs an expensive nm_InvertMatrix(...) to be calculated. Here we can exploit the property of the product of 2 inverse matrices.
        // If we call nm_GetFrustumPoints(frustumPoints,cameraVPMatrixInv) we find the frustum corners in world space

        nm_Mat4Mul(cameraVPMatrixInverseAdjusted,lvpMatrixOut16,cameraVPMatrixInv);  // This way we 'should' get all points in the [-1,1] light NDC space (or not?)

        nm_GetFrustumPoints(frustumPoints,cameraVPMatrixInverseAdjusted);

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
            const nmoat zero_point_five = (nmoat) 0.5;
            optionalLightViewportClippingOut4[0] = minVal[0]*zero_point_five+zero_point_five;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[1] = minVal[1]*zero_point_five+zero_point_five;   // In [0,1] from [-1,1]
            optionalLightViewportClippingOut4[2] = (maxVal[0]-minVal[0])*zero_point_five;    // extent x in [0,1]
            optionalLightViewportClippingOut4[3] = (maxVal[1]-minVal[1])*zero_point_five;    // extent y in [0,1]

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
            nmoat minmaxXY[4]={minVal[0]*cameraFrustumRadius,maxVal[0]*cameraFrustumRadius,minVal[1]*cameraFrustumRadius,maxVal[1]*cameraFrustumRadius};
            if (attemptToFixSwimming && texelIncrement>0)   {
                for (i=0;i<4;i++) {
                    // This 'should' fix Shadow swimming (1) in the 'Stable Shadow Mapping Technique'
                    // Not sure it works here too...
                    if (minmaxXY[i]>=0) minmaxXY[i] = nmh_ceil(minmaxXY[i]/texelIncrement)*texelIncrement;
                    else                minmaxXY[i] = -nmh_ceil(-minmaxXY[i]/texelIncrement)*texelIncrement;
                }
            }
            nm_Mat4Ortho(optionalLVPMatrixForFrustumCullingUsageOut16,
                         minmaxXY[0],minmaxXY[1],
                         minmaxXY[2],minmaxXY[3],
                         0,-(nmoat)2*cameraFrustumRadius                      // For z, we just copy nm_Ortho(lpMatrix,...)
                         );
            nm_Mat4Mul(optionalLVPMatrixForFrustumCullingUsageOut16,optionalLVPMatrixForFrustumCullingUsageOut16,lvMatrix);
            // This 'should' fix Shadow swimming (2) in the 'Stable Shadow Mapping Technique'
            // Not here, because the shadow viewport stretches when the camera rotates
            // We try anyway...
            if (attemptToFixSwimming && texelIncrement>0)   {
                nmoat shadowOrigin[4]   = {0,0,0,1};
                nmoat roundedOrigin[4]  = {0,0,0,0};
                nmoat roundOffset[4]    = {0,0,0,0};
                nmoat texelCoefficient = texelIncrement*(nmoat)2;
                nm_Mat4MulPos(optionalLVPMatrixForFrustumCullingUsageOut16,shadowOrigin,shadowOrigin[0],shadowOrigin[1],shadowOrigin[2]);
                for (i = 0; i < 2; i++) {// Or i<3 ?
                    shadowOrigin[i]/= texelCoefficient;
                    roundedOrigin[i] = nmh_round(shadowOrigin[i]);
                    roundOffset[i] = roundedOrigin[i] - shadowOrigin[i];
                    roundOffset[i]*=  texelCoefficient;
                }
                optionalLVPMatrixForFrustumCullingUsageOut16[12]+= roundOffset[0];
                optionalLVPMatrixForFrustumCullingUsageOut16[13]+= roundOffset[1];
            }
        }
    }

}
// 'normalizedLightDirection3' must be in world space.
NM_API_IMPL void nm_GetLightViewProjectionMatrix(nmoat* NM_RESTRICT lvpMatrixOut16
                                                         ,const nmoat* NM_RESTRICT cameraVMatrixInverse16,const nmoat* NM_RESTRICT cameraFrustumCenterInWorldSpace,nmoat cameraFrustumRadius
                                                         ,const nmoat* NM_RESTRICT normalizedLightDirection3,unsigned short shadowMapWidth,unsigned short shadowMapHeight)  {
    nm_GetLightViewProjectionMatrixExtra(lvpMatrixOut16,cameraVMatrixInverse16,cameraFrustumCenterInWorldSpace,cameraFrustumRadius,normalizedLightDirection3,shadowMapWidth,shadowMapHeight,0,0,0,0);
}


/*NM_API_PRIV_INL void nm_Min3(nmoat* NM_RESTRICT res3,const nmoat* a3,const nmoat* b3) {
    int i;for (i=0;i<3;i++) res3[i]=a3[i]<b3[i]?a3[i]:b3[i];
}
NM_API_PRIV_INL void nm_Max3(nmoat* NM_RESTRICT res3,const nmoat* a3,const nmoat* b3) {
    int i;for (i=0;i<3;i++) res3[i]=a3[i]>b3[i]?a3[i]:b3[i];
}*/
NM_API_IMPL int nm_FrustumOBBVisibilityTest(const nmoat frustumPlanes[6][4],const nmoat* NM_RESTRICT mfMatrix16,nmoat aabbMinX,nmoat aabbMinY,nmoat aabbMinZ,nmoat aabbMaxX,nmoat aabbMaxY,nmoat aabbMaxZ) {
    // It "should" perform an AABB test (after having transformed the OBB into an AABB).
    // mfMatrix16 is the matrix M so that: F*M = mvpMatrix (F being the matrix used to extract the frustum planes).
    // We can use: F=pMatrix and M=mvMatrix, or: F=vpMatrix and M=mMatrix too.
    int i;const nmoat zero = (nmoat)0;
    // Start OBB => AABB transformation based on: http://dev.theomader.com/transform-bounding-boxes/
    nmoat aabb[6];const nmoat* m = mfMatrix16;
    for (i=0;i<3;i++)   {
        const nmoat a0i=m[i]*aabbMinX,b0i=m[i]*aabbMaxX,a3i=m[4+i]*aabbMinY,b3i=m[4+i]*aabbMaxY,a6i=m[8+i]*aabbMinZ,b6i=m[8+i]*aabbMaxZ,m12i = m[12+i];
        nmoat vmin,vmax;
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

    // Start AABB visibility test
    // FAST VERSION
    for(i=0; i < 6; i++) {
        const nmoat *pl = &frustumPlanes[i][0];
        const int p[3] = {3*(int)(pl[0]>zero),3*(int)(pl[1]>zero),3*(int)(pl[2]>zero)};   // p[j] = 0 or 3
        const nmoat dp = pl[0]*aabb[p[0]] + pl[1]*aabb[p[1]+1] + pl[2]*aabb[p[2]+2] + pl[3];
        if (dp < 0) return 0;
    }
    // MUCH SLOWER VERSION
    /*{
        const nmoat aabb0=aabb[0],aabb1=aabb[1],aabb2=aabb[2];
        const nmoat aabb3=aabb[3],aabb4=aabb[4],aabb5=aabb[5];
        for(i=0; i < 6; i++)    {
            int sum = 0;
            const nmoat *pl = &frustumPlanes[i][0];
            const nmoat plx=pl[0],ply=pl[1],plz=pl[2],plw=pl[3];
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
    // End AABB visibility test
    return 1;
}
NM_API_IMPL int nm_FrustumBSVisibilityTest(const nmoat normalizedFrustumPlanes[6][4],const nmoat* NM_RESTRICT mfMatrix16,nmoat bsCenterX,nmoat bsCenterY,nmoat bsCenterZ,nmoat bsRadius,const nmoat* NM_RESTRICT optionalFrustumCenterInMfSpace3,nmoat optionalFrustumRadius) {
    // It "should" perform a Bounding Sphere test.
    // mfMatrix16 is the matrix M so that: F*M = mvpMatrix (F being the matrix used to extract the frustum planes).
    // We can use: F=pMatrix and M=mvMatrix, or: F=vpMatrix and M=mMatrix too.

    int i;const nmoat* pl;
    nmoat d,aabb_center[3];   // transformed in mfMatrix16 space (it can be world space for us if mfMatrix16 is mMatrix16)
    nm_Mat4MulPos(mfMatrix16,aabb_center,bsCenterX,bsCenterY,bsCenterZ);

    if (optionalFrustumCenterInMfSpace3)    {
        // If we pass the frustum_center (in mfMatrix16 space) + the frustum_radius, we can further
        // perform a sphere vs sphere collision check here this way:
        nmoat d = 0,dsquared = 0,sumradiisquared = bsRadius+optionalFrustumRadius;
        sumradiisquared*=sumradiisquared;
        for (i=0;i<3;i++)  {
            d = aabb_center[i]-optionalFrustumCenterInMfSpace3[i];
            dsquared+=d*d;
        }
        if (dsquared>sumradiisquared) return 0;  // all outside (no further culling is necessary)
        // I'm still not sure if it's worth adding this step or not...
    }

    // I guess there's a faster way than this...
    for (i=5;i>=0;--i)   {
        // If we start from the far plane (5), ''maybe'' culling is more efficient when
        // used together with the 'optionalFrustumCenterInMfSpace3' step (not measured).
        pl = &normalizedFrustumPlanes[i][0];
        d = pl[0]*aabb_center[0] + pl[1]*aabb_center[1] + pl[2]*aabb_center[2] + pl[3];
        if (d <= -bsRadius) return 0;   // all outside (no further culling is necessary)
        else if (d < bsRadius) return 2;   // intersect (further culling steps can be used to detect visibility, or we can just assume it's visible)
    }
    //printf("Early exit!\n");
    return 1; // all inside (no further culling is necessary)
}
NM_API_IMPL int nm_IsAabbVisible(const nmoat normalizedFrustumPlanes[6][4],const nmoat* NM_RESTRICT mfMatrix16,const nmoat* NM_RESTRICT aabbMin3,const nmoat* NM_RESTRICT aabbMax3,nmoat aabbRadius,nmoat scalingX,nmoat scalingY,nmoat scalingZ,const nmoat* NM_RESTRICT optionalFrustumCenterInMfSpace3,nmoat optionalFrustumRadius)   {
    // mfMatrix16 is the matrix M so that: F*M = mvpMatrix (F being the matrix used to extract the frustum planes).
    // We can use: F=pMatrix and M=mvMatrix, or: F=vpMatrix and M=mMatrix too.

    // Basically here we do a sphere test first, and then an obb test if necessary
    const nmoat max_scaling = scalingX>scalingY?(scalingX>scalingZ?scalingX:scalingZ):(scalingY>scalingZ?scalingY:scalingZ);
    const nmoat aabbCenter[3] = {(aabbMax3[0]+aabbMin3[0])*(nmoat)0.5,(aabbMax3[1]+aabbMin3[1])*(nmoat)0.5,(aabbMax3[2]+aabbMin3[2])*(nmoat)0.5};
    int rv = nm_FrustumBSVisibilityTest(normalizedFrustumPlanes,mfMatrix16,aabbCenter[0],aabbCenter[1],aabbCenter[2],aabbRadius*max_scaling,optionalFrustumCenterInMfSpace3,optionalFrustumRadius);
    if (rv==2) return nm_FrustumOBBVisibilityTest(normalizedFrustumPlanes,mfMatrix16,aabbMin3[0]*scalingX,aabbMin3[1]*scalingY,aabbMin3[2]*scalingZ,aabbMax3[0]*scalingX,aabbMax3[1]*scalingY,aabbMax3[2]*scalingZ);
    else return rv;
}


#ifdef __cplusplus
}   // extern "C"
#endif //__cplusplus


#ifdef NM_NAMESPACE
namespace NM_NAMESPACE  {
#endif //NM_NAMESPACE

#ifndef NM_NO_MATH_STRUCTS

NM_API_IMPL nmoat radians(nmoat deg) {return M_DEG2RAD(deg);}
NM_API_IMPL nmoat degrees(nmoat rad) {return M_RAD2DEG(rad);}

// -------------------
// vec2 implementation
// -------------------
NM_API_IMPL_EXT vec2 make_vec2(nmoat _x, nmoat _y);
#ifndef NM_NO_STDIO
NM_API_IMPL void vec2_fprintp(const vec2* v2,FILE* stream,int width, int precision) {nm_Vec2_fprintp(v2->e,stream,width,precision);}
NM_API_IMPL void vec2_fprint(const vec2* v2,FILE* stream) {nm_Vec2_fprint(v2->e,stream);}
NM_API_IMPL void vec2_printp(const vec2* v2,int width, int precision) {nm_Vec2_printp(v2->e, width, precision);}
NM_API_IMPL void vec2_print(const vec2* v2) {nm_Vec2_print(v2->e);}
#endif //NM_NO_STDIO
//NM_API_IMPL vec2 vec2_copy(const vec2* src) {return make_vec2(src->x,src->y);}
NM_API_IMPL vec2 vec2_add(const vec2* a,const vec2* b)  {return make_vec2(a->x+b->x,a->y+b->y);}
NM_API_IMPL vec2 vec2_sub(const vec2* a,const vec2* b)  {return make_vec2(a->x-b->x,a->y-b->y);}
NM_API_IMPL vec2 vec2_neg(const vec2* src)  {return make_vec2(-src->x,-src->y);}
NM_API_IMPL vec2 vec2_mul(const vec2* a,const vec2* b)  {return make_vec2(a->x*b->x,a->y*b->y);}
NM_API_IMPL vec2 vec2_mulsca(const vec2* src,nmoat f)  {return make_vec2(src->x*f,src->y*f);}
NM_API_IMPL vec2 vec2_div(const vec2* a,const vec2* b)  {return make_vec2(a->x/b->x,a->y/b->y);}
NM_API_IMPL vec2 vec2_divsca(const vec2* src,nmoat f)  {NM_ASSERT(f!=(nmoat)0);return make_vec2(src->x/f,src->y/f);}
NM_API_IMPL const vec2* vec2_sadd(vec2* dst,const vec2* src)  {dst->x+=src->x;dst->y+=src->y;return dst;}
NM_API_IMPL const vec2* vec2_ssub(vec2* dst,const vec2* src)  {dst->x-=src->x;dst->y-=src->y;return dst;}
NM_API_IMPL const vec2* vec2_inv(vec2* dst)  {dst->x=-dst->x;dst->y=-dst->y;return dst;}
NM_API_IMPL const vec2* vec2_smul(vec2* dst,const vec2* src)  {dst->x*=src->x;dst->y*=src->y;return dst;}
NM_API_IMPL const vec2* vec2_smulsca(vec2* dst,nmoat f)  {dst->x*=f;dst->y*=f;return dst;}
NM_API_IMPL const vec2* vec2_sdiv(vec2* dst,const vec2* src)  {dst->x/=src->x;dst->y/=src->y;return dst;}
NM_API_IMPL const vec2* vec2_sdivsca(vec2* dst,nmoat f)  {NM_ASSERT(f!=(nmoat)0);dst->x/=f;dst->y/=f;return dst;}
NM_API_IMPL nmoat vec2_dot(const vec2* a,const vec2* b)  {return a->x*b->x+a->y*b->y;}
NM_API_IMPL nmoat vec2_length2(const vec2* a) {return a->x*a->x+a->y*a->y;}
NM_API_IMPL nmoat vec2_length(const vec2* a) {return nmh_sqrt(a->x*a->x+a->y*a->y);}
NM_API_IMPL const vec2* vec2_normalize(vec2* dst) {const nmoat len=vec2_length(dst);if (len>0) {dst->x/=len;dst->y/=len;} else {dst->x=dst->y=(nmoat)0;}return dst;}
NM_API_IMPL vec2 vec2_normalized(const vec2* src) {const nmoat len=vec2_length(src);return (len>0) ? make_vec2(src->x/len,src->y/len) : make_vec2((nmoat)0,(nmoat)0);}
NM_API_IMPL vec2 vec2_lerp(const vec2* a,const vec2* b,nmoat t) {nmoat ct = (nmoat)1-t;return make_vec2(a->x*ct+b->x*t,a->y*ct+b->y*t);}
NM_API_IMPL vec2 vec2_proj(const vec2* src,const vec2* onto) {
    // NM_API_INL vec2 proj(const vec2& onto) const {return onto*this->dot(onto)/onto.dot(onto);}
    const nmoat s = vec2_dot(src,onto)/vec2_dot(onto,onto);
    return vec2_mulsca(onto,s);
}
NM_API_IMPL nmoat vec2_angle_between(const vec2* a,const vec2* b) {return nmh_acos(vec2_dot(a,b)/(vec2_length(a)*vec2_length(b)));}
NM_API_IMPL int vec2_areequaleps(const vec2* a,const vec2* b,nmoat eps) {return nm_AreVec2EqualEps(a->e,b->e,eps);}
NM_API_IMPL int vec2_areequal(const vec2* a,const vec2* b)              {return nm_AreVec2Equal(a->e,b->e);}

// -------------------
// vec3 implementation
// -------------------
NM_API_IMPL_EXT vec3 make_vec3(nmoat _x, nmoat _y, nmoat _z);
#ifndef NM_NO_STDIO
NM_API_IMPL void vec3_fprintp(const vec3* v3,FILE* stream,int width, int precision) {nm_Vec3_fprintp(v3->e,stream,width,precision);}
NM_API_IMPL void vec3_fprint(const vec3* v3,FILE* stream) {nm_Vec3_fprint(v3->e,stream);}
NM_API_IMPL void vec3_printp(const vec3* v3,int width, int precision) {nm_Vec3_printp(v3->e, width, precision);}
NM_API_IMPL void vec3_print(const vec3* v3) {nm_Vec3_print(v3->e);}
#endif //NM_NO_STDIO
//NM_API_IMPL vec3 vec3_copy(const vec3* src) {return make_vec3(src->x,src->y,src->z);}
NM_API_IMPL vec3 vec3_add(const vec3* a,const vec3* b)  {return make_vec3(a->x+b->x,a->y+b->y,a->z+b->z);}
NM_API_IMPL vec3 vec3_sub(const vec3* a,const vec3* b)  {return make_vec3(a->x-b->x,a->y-b->y,a->z-b->z);}
NM_API_IMPL vec3 vec3_neg(const vec3* src)  {return make_vec3(-src->x,-src->y,-src->z);}
NM_API_IMPL vec3 vec3_mul(const vec3* a,const vec3* b)  {return make_vec3(a->x*b->x,a->y*b->y,a->z*b->z);}
NM_API_IMPL vec3 vec3_mulsca(const vec3* src,nmoat f)  {return make_vec3(src->x*f,src->y*f,src->z*f);}
NM_API_IMPL vec3 vec3_div(const vec3* a,const vec3* b)  {return make_vec3(a->x/b->x,a->y/b->y,a->z/b->z);}
NM_API_IMPL vec3 vec3_divsca(const vec3* src,nmoat f)  {NM_ASSERT(f!=(nmoat)0);return make_vec3(src->x/f,src->y/f,src->z/f);}
NM_API_IMPL const vec3* vec3_sadd(vec3* dst,const vec3* src)  {dst->x+=src->x;dst->y+=src->y;dst->z+=src->z;return dst;}
NM_API_IMPL const vec3* vec3_ssub(vec3* dst,const vec3* src)  {dst->x-=src->x;dst->y-=src->y;dst->z-=src->z;return dst;}
NM_API_IMPL const vec3* vec3_inv(vec3* dst)  {dst->x=-dst->x;dst->y=-dst->y;dst->z=-dst->z;return dst;}
NM_API_IMPL const vec3* vec3_smul(vec3* dst,const vec3* src)  {dst->x*=src->x;dst->y*=src->y;dst->z*=src->z;return dst;}
NM_API_IMPL const vec3* vec3_smulsca(vec3* dst,nmoat f)  {dst->x*=f;dst->y*=f;dst->z*=f;return dst;}
NM_API_IMPL const vec3* vec3_sdiv(vec3* dst,const vec3* src)  {dst->x/=src->x;dst->y/=src->y;dst->z/=src->z;return dst;}
NM_API_IMPL const vec3* vec3_sdivsca(vec3* dst,nmoat f)  {NM_ASSERT(f!=(nmoat)0);dst->x/=f;dst->y/=f;dst->z/=f;return dst;}
NM_API_IMPL nmoat vec3_dot(const vec3* a,const vec3* b)  {return a->x*b->x+a->y*b->y+a->z*b->z;}
NM_API_IMPL vec3 vec3_cross(const vec3* a,const vec3* b)  {return make_vec3(a->y*b->z-a->z*b->y,a->z*b->x-a->x*b->z,a->x*b->y-a->y*b->x);}
NM_API_IMPL nmoat vec3_length3(const vec3* a) {return a->x*a->x+a->y*a->y+a->z*a->z;}
NM_API_IMPL nmoat vec3_length(const vec3* a) {return nmh_sqrt(a->x*a->x+a->y*a->y+a->z*a->z);}
NM_API_IMPL const vec3* vec3_normalize(vec3* dst) {const nmoat len=vec3_length(dst);if (len>0) {dst->x/=len;dst->y/=len;dst->z/=len;} else {dst->x=dst->y=dst->z=(nmoat)0;}return dst;}
NM_API_IMPL vec3 vec3_normalized(const vec3* src) {const nmoat len=vec3_length(src);return (len>0) ? make_vec3(src->x/len,src->y/len,src->z/len) : make_vec3((nmoat)0,(nmoat)0,(nmoat)0);}
NM_API_IMPL vec3 vec3_lerp(const vec3* a,const vec3* b,nmoat t) {nmoat ct = (nmoat)1-t;return make_vec3(a->x*ct+b->x*t,a->y*ct+b->y*t,a->z*ct+b->z*t);}
NM_API_IMPL vec3 vec3_proj(const vec3* src,const vec3* onto) {
    // NM_API_INL vec3 proj(const vec3& onto) const {return onto*this->dot(onto)/onto.dot(onto);}
    const nmoat s = vec3_dot(src,onto)/vec3_dot(onto,onto);
    return vec3_mulsca(onto,s);
}
NM_API_IMPL nmoat vec3_angle_between(const vec3* a,const vec3* b) {return nmh_acos(vec3_dot(a,b)/(vec3_length(a)*vec3_length(b)));}
NM_API_IMPL int vec3_areequaleps(const vec3* a,const vec3* b,nmoat eps) {return nm_AreVec3EqualEps(a->e,b->e,eps);}
NM_API_IMPL int vec3_areequal(const vec3* a,const vec3* b)              {return nm_AreVec3Equal(a->e,b->e);}

// -------------------
// vec4 implementation
// -------------------
NM_API_IMPL_EXT vec4 make_vec4(nmoat _x, nmoat _y, nmoat _z, nmoat _w);
#ifndef NM_NO_STDIO
NM_API_IMPL void vec4_fprintp(const vec4* v4,FILE* stream,int width, int precision) {nm_Vec4_fprintp(v4->e,stream,width,precision);}
NM_API_IMPL void vec4_fprint(const vec4* v4,FILE* stream) {nm_Vec4_fprint(v4->e,stream);}
NM_API_IMPL void vec4_printp(const vec4* v4,int width, int precision) {nm_Vec4_printp(v4->e, width, precision);}
NM_API_IMPL void vec4_print(const vec4* v4) {nm_Vec4_print(v4->e);}
#endif //NM_NO_STDIO
//NM_API_IMPL vec4 vec4_copy(const vec4* src) {return make_vec4(src->x,src->y,src->z,src->w);}
NM_API_IMPL vec4 vec4_add(const vec4* a,const vec4* b)  {return make_vec4(a->x+b->x,a->y+b->y,a->z+b->z,a->w+b->w);}
NM_API_IMPL vec4 vec4_sub(const vec4* a,const vec4* b)  {return make_vec4(a->x-b->x,a->y-b->y,a->z-b->z,a->w-b->w);}
NM_API_IMPL vec4 vec4_neg(const vec4* src)  {return make_vec4(-src->x,-src->y,-src->z,-src->w);}
NM_API_IMPL vec4 vec4_mul(const vec4* a,const vec4* b)  {return make_vec4(a->x*b->x,a->y*b->y,a->z*b->z,a->w*b->w);}
NM_API_IMPL vec4 vec4_mulsca(const vec4* src,nmoat f)  {return make_vec4(src->x*f,src->y*f,src->z*f,src->w*f);}
NM_API_IMPL vec4 vec4_div(const vec4* a,const vec4* b)  {return make_vec4(a->x/b->x,a->y/b->y,a->z/b->z,a->w/b->w);}
NM_API_IMPL vec4 vec4_divsca(const vec4* src,nmoat f)  {NM_ASSERT(f!=(nmoat)0);return make_vec4(src->x/f,src->y/f,src->z/f,src->w/f);}
NM_API_IMPL const vec4* vec4_sadd(vec4* dst,const vec4* src)  {dst->x+=src->x;dst->y+=src->y;dst->z+=src->z;dst->w+=src->w;return dst;}
NM_API_IMPL const vec4* vec4_ssub(vec4* dst,const vec4* src)  {dst->x-=src->x;dst->y-=src->y;dst->z-=src->z;dst->w-=src->w;return dst;}
NM_API_IMPL const vec4* vec4_inv(vec4* dst)  {dst->x=-dst->x;dst->y=-dst->y;dst->z=-dst->z;dst->w=-dst->w;return dst;}
NM_API_IMPL const vec4* vec4_smul(vec4* dst,const vec4* src)  {dst->x*=src->x;dst->y*=src->y;dst->z*=src->z;dst->w*=src->w;return dst;}
NM_API_IMPL const vec4* vec4_smulsca(vec4* dst,nmoat f)  {dst->x*=f;dst->y*=f;dst->z*=f;dst->w*=f;return dst;}
NM_API_IMPL const vec4* vec4_sdiv(vec4* dst,const vec4* src)  {dst->x/=src->x;dst->y/=src->y;dst->z/=src->z;dst->w/=src->w;return dst;}
NM_API_IMPL const vec4* vec4_sdivsca(vec4* dst,nmoat f)  {NM_ASSERT(f!=(nmoat)0);dst->x/=f;dst->y/=f;dst->z/=f;dst->w/=f;return dst;}
NM_API_IMPL nmoat vec4_dot(const vec4* a,const vec4* b)  {return a->x*b->x+a->y*b->y+a->z*b->z+a->w*b->w;}
NM_API_IMPL nmoat vec4_length2(const vec4* a) {return a->x*a->x+a->y*a->y+a->z*a->z+a->w*a->w;}
NM_API_IMPL nmoat vec4_length(const vec4* a) {return nmh_sqrt(a->x*a->x+a->y*a->y+a->z*a->z+a->w*a->w);}
NM_API_IMPL const vec4* vec4_normalize(vec4* dst) {const nmoat len=vec4_length(dst);if (len>0) {dst->x/=len;dst->y/=len;dst->z/=len;dst->w/=len;} else {dst->x=dst->y=dst->z=dst->w=(nmoat)0;}return dst;}
NM_API_IMPL vec4 vec4_normalized(const vec4* src) {const nmoat len=vec4_length(src);return (len>0) ? make_vec4(src->x/len,src->y/len,src->z/len,src->w/len) : make_vec4((nmoat)0,(nmoat)0,(nmoat)0,(nmoat)0);}
NM_API_IMPL vec4 vec4_lerp(const vec4* a,const vec4* b,nmoat t) {nmoat ct = (nmoat)1-t;return make_vec4(a->x*ct+b->x*t,a->y*ct+b->y*t,a->z*ct+b->z*t,a->w*ct+b->w*t);}
NM_API_IMPL vec4 vec4_proj(const vec4* src,const vec4* onto) {
    // NM_API_INL vec4 proj(const vec4& onto) const {return onto*this->dot(onto)/onto.dot(onto);}
    const nmoat s = vec4_dot(src,onto)/vec4_dot(onto,onto);
    return vec4_mulsca(onto,s);
}
NM_API_IMPL int vec4_areequaleps(const vec4* a,const vec4* b,nmoat eps) {return nm_AreVec4EqualEps(a->e,b->e,eps);}
NM_API_IMPL int vec4_areequal(const vec4* a,const vec4* b)              {return nm_AreVec4Equal(a->e,b->e);}

// -------------------
// quat implementation
// -------------------
NM_API_IMPL_EXT quat make_quat(nmoat _w,nmoat _x, nmoat _y, nmoat _z);
NM_API_IMPL_EXT quat make_quat_from_vec3(nmoat _w, vec3* v);
#ifndef NM_NO_STDIO
NM_API_IMPL void quat_fprintp(const quat* q,FILE* stream,int width, int precision) {nm_Quat_fprintp(q->e,stream,width,precision);}
NM_API_IMPL void quat_fprint(const quat* q,FILE* stream) {nm_Quat_fprint(q->e,stream);}
NM_API_IMPL void quat_printp(const quat* q,int width, int precision) {nm_Quat_printp(q->e,width,precision);}
NM_API_IMPL void quat_print(const quat* q) {nm_Quat_print(q->e);}
#endif //NM_NO_STDIO
//NM_API_IMPL quat quat_copy(const quat* src) {return make_quat(src->w,src->x,src->y,src->z);}
NM_API_IMPL quat quat_add(const quat* a,const quat* b)  {return make_quat(a->w+b->w,a->x+b->x,a->y+b->y,a->z+b->z);}
NM_API_IMPL quat quat_sub(const quat* a,const quat* b)  {return make_quat(a->w-b->w,a->x-b->x,a->y-b->y,a->z-b->z);}
NM_API_IMPL quat quat_neg(const quat* src)  {return make_quat(-src->w,-src->x,-src->y,-src->z);}
NM_API_IMPL quat quat_mul(const quat* a,const quat* b)  {
    return make_quat(
                a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z,
                a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y,
                a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z,
                a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x
                );
}
NM_API_IMPL quat quat_mul3quats(const quat* a,const quat* b,const quat* c)  {quat r = quat_mul(a,b);r=quat_mul(&r,c);return r;}
NM_API_IMPL quat quat_mulsca(const quat* src,nmoat f)  {return make_quat(src->w*f,src->x*f,src->y*f,src->z*f);}
NM_API_IMPL quat quat_div(const quat* a,const quat* b) {return make_quat(a->w/b->w,a->x/b->x,a->y/b->y,a->z/b->z);}
NM_API_IMPL quat quat_divsca(const quat* src,nmoat f)  {NM_ASSERT(f!=(nmoat)0);return make_quat(src->w/f,src->x/f,src->y/f,src->z/f);}
NM_API_IMPL const quat* quat_sadd(quat* dst,const quat* src)  {dst->x+=src->x;dst->y+=src->y;dst->z+=src->z;dst->w+=src->w;return dst;}
NM_API_IMPL const quat* quat_ssub(quat* dst,const quat* src)  {dst->x-=src->x;dst->y-=src->y;dst->z-=src->z;dst->w-=src->w;return dst;}
NM_API_IMPL const quat* quat_inv(quat* dst)  {dst->x=-dst->x;dst->y=-dst->y;dst->z=-dst->z;dst->w=-dst->w;return dst;}
NM_API_IMPL const quat* quat_smul(quat* dst,const quat* src)    {*dst = quat_mul(dst,src);return dst;}
NM_API_IMPL const quat* quat_smulsca(quat* dst,nmoat f)  {dst->x*=f;dst->y*=f;dst->z*=f;dst->w*=f;return dst;}
NM_API_IMPL const quat* quat_sdiv(quat* dst,const quat* src) {dst->x/=src->x;dst->y/=src->y;dst->z/=src->z;dst->w/=src->w;return dst;}
NM_API_IMPL const quat* quat_sdivsca(quat* dst,nmoat f)  {NM_ASSERT(f!=(nmoat)0);dst->x/=f;dst->y/=f;dst->z/=f;dst->w/=f;return dst;}
NM_API_IMPL nmoat quat_dot(const quat* a,const quat* b)  {return nm_QuatDot(a->e,b->e);}
/*NM_API_IMPL quat quat_cross(const quat* a,const quat* b)  {
    // same as quat_mul(...) ?
    return make_quat(
                a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z,
                a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y,
                a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z,
                a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x
                );
}*/
NM_API_IMPL vec3 quat_crossvec(const quat* a,const vec3* b)  {return make_vec3(a->y*b->z-a->z*b->y,a->z*b->x-a->x*b->z,a->x*b->y-a->y*b->x);}
NM_API_IMPL nmoat quat_length2(const quat* a) {return a->x*a->x+a->y*a->y+a->z*a->z+a->w*a->w;}
NM_API_IMPL nmoat quat_length(const quat* a) {return nmh_sqrt(a->x*a->x+a->y*a->y+a->z*a->z+a->w*a->w);}
NM_API_IMPL const quat* quat_normalize(quat* dst) {const nmoat len=quat_length(dst);if (len>0) {dst->x/=len;dst->y/=len;dst->z/=len;dst->w/=len;} else {dst->x=dst->y=dst->z=dst->w=(nmoat)0;}return dst;}
NM_API_IMPL quat quat_normalized(const quat* src) {const nmoat len=quat_length(src);return (len>0) ? make_quat(src->w/len,src->x/len,src->y/len,src->z/len) : make_quat((nmoat)1,(nmoat)0,(nmoat)0,(nmoat)0);}
NM_API_IMPL quat quat_lerp(const quat* a,const quat* b,nmoat t,int normalizeQOutAfterLerp/*=1*/)    {quat r;nm_QuatLerp(r.e,a->e,b->e,t,normalizeQOutAfterLerp);return r;}
NM_API_IMPL quat quat_slerp(const quat* a,const quat* b,nmoat slerpTime_In_0_1,int normalizeQOutAfterLerp/*=1*/,nmoat eps/*=(nmoat)0.0001*/)    {quat q;nm_QuatSlerpEps(q.e,a->e,b->e,slerpTime_In_0_1,normalizeQOutAfterLerp,eps);return q;}
NM_API_IMPL quat quat_conjugated(const quat* q) {return make_quat(q->w,-q->x,-q->y,-q->z);}
NM_API_IMPL void quat_conjugate(quat* q)    {q->x=-q->x;q->y=-q->y;q->z=-q->z;}
NM_API_IMPL quat quat_inverted(const quat* q)   {nmoat d = quat_dot(q,q);return make_quat(q->w/d,-q->x/d,-q->y/d,-q->z/d);}
NM_API_IMPL void quat_inverse(quat* q)  {nmoat d = -quat_dot(q,q);q->w/=-d;q->x/=d;q->y/=d;q->z/=d;}
NM_API_IMPL vec3 quat_mulvec(const quat* q,const vec3* v)   {vec3 r;nm_QuatMulVec3(r.e,q->e,v->e);return r;}
NM_API_IMPL vec3 quat_mulvecInv(const quat* q,const vec3* v)    {vec3 r;nm_QuatMulVec3Inv(r.e,q->e,v->e);return r;}
NM_API_IMPL quat quat_fromAngleAxis(nmoat rfAngle,const vec3* rkAxis)   {quat r;nm_QuatFromAngleAxisVec3(r.e,rfAngle,rkAxis->e);return r;}
NM_API_IMPL void quat_toAngleAxis(const quat* q,nmoat* rfAngleOut,vec3* rkAxisOut)  {nm_QuatToAngleAxis(q->e,rfAngleOut,rkAxisOut->e);}
NM_API_IMPL quat quat_fromLookAtYX(const vec3* sourcePosition,const vec3* targetPosition)   {quat r;nm_QuatFromLookAtYX(r.e,sourcePosition->x,sourcePosition->y,sourcePosition->z,targetPosition->x,targetPosition->y,targetPosition->z);return r;}
NM_API_IMPL quat quat_FromEuler(nmoat fAngleX,nmoat fAngleY,nmoat fAngleZ,EulerRotationMode eulerRotationMode)  {quat r;nm_QuatFromEuler(r.e,fAngleX,fAngleY,fAngleZ,(EulerRotationMode)eulerRotationMode);return r;}

NM_API_IMPL int quat_areequaleps(const quat* a,const quat* b,nmoat eps) {return nm_AreQuatEqualEps(a->e,b->e,eps);}
NM_API_IMPL int quat_areequal(const quat* a,const quat* b)              {return nm_AreQuatEqual(a->e,b->e);}

/* Quat Hints from the web

WHEN TWO QUATERNIONS ARE SIMILAR:
=================================
With quaternions, it’s exactly the same as vectors:

float matching = quaternion::dot(q1, q2);
if ( abs(matching-1.0) < 0.001 ){
    // q1 and q2 are similar
}

You can also get the angle between q1 and q2 by taking the acos() of this dot product.

*/


// -------------------
// mat3 implementation
// -------------------
NM_API_IMPL_EXT mat3 make_mat3(nmoat m00,nmoat m10,nmoat m20,nmoat m01,nmoat m11,nmoat m21,nmoat m02,nmoat m12,nmoat m22);
NM_API_IMPL_EXT mat3 make_mat3_from_row_major(nmoat m00,nmoat m01,nmoat m02,nmoat m10,nmoat m11,nmoat m12,nmoat m20,nmoat m21,nmoat m22);
NM_API_IMPL_EXT mat3 mat3_identity();

#ifndef NM_NO_STDIO
NM_API_IMPL void mat3_fprintp(const mat3* m,FILE* stream,int width, int precision)  {nm_Mat3_fprintp(m->m,stream,width,precision);}
NM_API_IMPL void mat3_fprint(const mat3* m,FILE* stream) {nm_Mat3_fprint(m->m,stream);}
NM_API_IMPL void mat3_printp(const mat3* m,int width, int precision) {nm_Mat3_printp(m->m, width, precision);}
NM_API_IMPL void mat3_print(const mat3* m) {nm_Mat3_print(m->m);}
#endif //NM_NO_STDIO
//NM_API_IMPL mat3 mat3_copy(const quat* src);
NM_API_IMPL const mat3* mat3_mul(mat3* dst,const mat3* a,const mat3* b) {nm_Mat3Mul(dst->m,a->m,b->m);return dst;}
NM_API_IMPL const mat3* mat3_mul_nocheck(mat3* dst,const mat3* a,const mat3* b) {nm_Mat3Mul_NoCheck(dst->m,a->m,b->m);return dst;}
NM_API_IMPL const mat3* mat3_mul3(mat3* dst,const mat3* a,const mat3* b,const mat3* c)   {mat3 tmp;return mat3_mul(dst,mat3_mul(&tmp,a,b),c);}
NM_API_IMPL const mat3* mat3_mul3_nocheck(mat3* dst,const mat3* a,const mat3* b,const mat3* c)   {mat3 tmp;return mat3_mul_nocheck(dst,mat3_mul_nocheck(&tmp,a,b),c);}

NM_API_IMPL quat mat3_getQuaternionFromMatrixRotation(const mat3* src)  {quat q;nm_QuatFromMat3(q.e,src->m);return q;}
NM_API_IMPL void mat3_setQuaternionToMatrixRotation(mat3* dst,const quat* q)    {nm_Mat3FromQuat(dst->m,q->e);}

NM_API_IMPL const mat3* mat3_transposed(mat3* dst,const mat3* src) 	{nm_Mat3Transpose(dst->m,src->m);return dst;}
NM_API_IMPL void mat3_transpose(mat3* m) {nm_Mat3TransposeInPlace(m->m);}
NM_API_IMPL const mat3* mat3_inverted(mat3* dst,const mat3* src) {nm_Mat3Invert(dst->m,src->m);return dst;}
NM_API_IMPL int mat3_invert(mat3* m) {return nm_Mat3InvertInPlace(m->m);}
NM_API_IMPL vec3 mat3_muldir(const mat3* m,nmoat dirX,nmoat dirY,nmoat dirZ) {const nmoat* e=m->m;return make_vec3(dirX*e[0] + dirY*e[3] + dirZ*e[6], dirX*e[1] + dirY*e[4] + dirZ*e[7], dirX*e[2] + dirY*e[5] + dirZ*e[8]);}
NM_API_IMPL vec3 mat3_muldirvec(const mat3* m,const vec3* o) {const nmoat* e=m->m;return make_vec3(o->x*e[0] + o->y*e[3] + o->z*e[6], o->x*e[1] + o->y*e[4] + o->z*e[7], o->x*e[2] + o->y*e[5] + o->z*e[8]);}

// A few static methods:
NM_API_IMPL void mat3_slerp(mat3* m,const mat3* mStart,const mat3* mEnd,nmoat slerpTime_In_0_1) {nm_Mat3Slerp(m->m,mStart->m,mEnd->m,slerpTime_In_0_1);}

NM_API_IMPL void mat3_fromEulerXYZ(mat3* m,nmoat x,nmoat y, nmoat z)  {nm_Mat3FromEulerXYZ(m->m,x,y,z);}
NM_API_IMPL void mat3_fromEulerXZY(mat3* m,nmoat x,nmoat y, nmoat z)  {nm_Mat3FromEulerXZY(m->m,x,y,z);}
NM_API_IMPL void mat3_fromEulerYXZ(mat3* m,nmoat x,nmoat y, nmoat z)  {nm_Mat3FromEulerYXZ(m->m,x,y,z);}
NM_API_IMPL void mat3_fromEulerYZX(mat3* m,nmoat x,nmoat y, nmoat z)  {nm_Mat3FromEulerYZX(m->m,x,y,z);}
NM_API_IMPL void mat3_fromEulerZXY(mat3* m,nmoat x,nmoat y, nmoat z)  {nm_Mat3FromEulerZXY(m->m,x,y,z);}
NM_API_IMPL void mat3_fromEulerZYX(mat3* m,nmoat x,nmoat y, nmoat z)  {nm_Mat3FromEulerZYX(m->m,x,y,z);}

NM_API_IMPL int mat3_toEulerXYZ(const mat3* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat3ToEulerXYZ(m->m,x,y,z);}
NM_API_IMPL int mat3_toEulerXZY(const mat3* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat3ToEulerXZY(m->m,x,y,z);}
NM_API_IMPL int mat3_toEulerYXZ(const mat3* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat3ToEulerYXZ(m->m,x,y,z);}
NM_API_IMPL int mat3_toEulerYZX(const mat3* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat3ToEulerYZX(m->m,x,y,z);}
NM_API_IMPL int mat3_toEulerZXY(const mat3* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat3ToEulerZXY(m->m,x,y,z);}
NM_API_IMPL int mat3_toEulerZYX(const mat3* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat3ToEulerZYX(m->m,x,y,z);}

NM_API_IMPL void mat3_fromEuler(mat3* m,nmoat x,nmoat y,nmoat z,EulerRotationMode mode)   {nm_Mat3FromEuler(m->m,x,y,z,(int)mode);}
NM_API_IMPL int mat3_toEuler(const mat3* m,nmoat* x,nmoat* y,nmoat* z,EulerRotationMode mode)   {return nm_Mat3ToEuler(m->m,x,y,z,(int)mode);}

NM_API_IMPL int mat3_areequaleps(const mat3* a,const mat3* b,nmoat eps) {return nm_AreMat3EqualEps(a->m,b->m,eps);}
NM_API_IMPL int mat3_areequal(const mat3* a,const mat3* b)              {return nm_AreMat3Equal(a->m,b->m);}


// -------------------
// mat4 implementation
// -------------------
NM_API_IMPL_EXT mat4 make_mat4(nmoat m00,nmoat m10, nmoat m20, nmoat m30, nmoat m01, nmoat m11, nmoat m21, nmoat m31, nmoat m02, nmoat m12, nmoat m22, nmoat m32, nmoat m03, nmoat m13, nmoat m23, nmoat m33);
NM_API_IMPL_EXT mat4 make_mat4_from_row_major(nmoat m00,nmoat m01,nmoat m02,nmoat m03,   nmoat m10,nmoat m11,nmoat m12,nmoat m13,    nmoat m20,nmoat m21,nmoat m22,nmoat m23,    nmoat m30,nmoat m31,nmoat m32,nmoat m33);
NM_API_IMPL_EXT mat4 mat4_identity();


#ifndef NM_NO_STDIO
NM_API_IMPL void mat4_fprintp(const mat4* m,FILE* stream,int width, int precision)  {nm_Mat4_fprintp(m->m,stream,width,precision);}
NM_API_IMPL void mat4_fprint(const mat4* m,FILE* stream) {nm_Mat4_fprint(m->m,stream);}
NM_API_IMPL void mat4_printp(const mat4* m,int width, int precision)  {nm_Mat4_printp(m->m,width,precision);}
NM_API_IMPL void mat4_print(const mat4* m) {nm_Mat4_print(m->m);}
#endif //NM_NO_STDIO
//NM_API_DEF_EXT_INL mat4 mat4_copy(const quat* src);
NM_API_IMPL const mat4* mat4_mul(mat4* dst,const mat4* a,const mat4* b) {nm_Mat4Mul(dst->m,a->m,b->m);return dst;}
NM_API_IMPL const mat4* mat4_mul_nocheck(mat4* dst,const mat4* a,const mat4* b) {nm_Mat4Mul_NoCheck(dst->m,a->m,b->m);return dst;}
NM_API_IMPL const mat4* mat4_mul3(mat4* dst,const mat4* a,const mat4* b,const mat4* c)   {mat4 tmp;return mat4_mul(dst,mat4_mul(&tmp,a,b),c);}
NM_API_IMPL const mat4* mat4_mul3_nocheck(mat4* dst,const mat4* a,const mat4* b,const mat4* c)   {mat4 tmp;return mat4_mul_nocheck(dst,mat4_mul_nocheck(&tmp,a,b),c);}

NM_API_IMPL quat mat4_getQuaternionFromMatrixRotation(const mat4* src)   {quat q;nm_QuatFromMat4(q.e,src->m);return q;}
NM_API_IMPL void mat4_setQuaternionToMatrixRotation(mat4* dst,const quat* q)    {nm_Mat4SetRotationFromQuat(dst->m,q->e);}

NM_API_IMPL const mat4* mat4_transposed(mat4* dst,const mat4* src)  {nm_Mat4Transpose(dst->m,src->m);return dst;}
NM_API_IMPL void mat4_transpose(mat4* m) {nmoat s01=m->m01,s02=m->m02,s03=m->m03,   s10=m->m10,s12=m->m12,s13=m->m13,   s20=m->m20,s21=m->m20,s23=m->m23,   s30=m->m30,s31=m->m31,s32=m->m32; m->m01=s10;m->m02=s20;m->m03=s30;   m->m10=s01;m->m12=s21;m->m13=s31; m->m20=s02;m->m21=s12;m->m23=s32; m->m30=s03;m->m31=s13;m->m32=s23;}
NM_API_IMPL const mat4* mat4_inverted(mat4* dst,const mat4* src)    {nm_Mat4Invert(dst->m,src->m);return dst;}
NM_API_IMPL int mat4_invert(mat4* m)   {return nm_Mat4Invert(m->m,m->m);}
NM_API_IMPL const mat4* mat4_invertedFast(mat4* dst,const mat4* src)    {nm_Mat4InvertTransformMatrixWithoutScaling(dst->m,src->m);return dst;}
NM_API_IMPL void mat4_invertFast(mat4* m)   {const mat4 tmp=*m;nm_Mat4InvertTransformMatrixWithoutScaling(m->m,tmp.m);}
NM_API_IMPL void mat4_invertXZAxis(mat4* m) {nm_Mat4InvertXZAxisInPlace(m->m);}
NM_API_IMPL const mat4* mat4_invertedXZAxis(mat4* dst,const mat4* src)  {nm_Mat4InvertXZAxis(dst->m,src->m);return dst;}


NM_API_IMPL vec3 mat4_muldir(const mat4* m,nmoat dirX,nmoat dirY,nmoat dirZ) {const nmoat* e=m->m;return make_vec3(dirX*e[0] + dirY*e[3] + dirZ*e[6], dirX*e[1] + dirY*e[4] + dirZ*e[7], dirX*e[2] + dirY*e[5] + dirZ*e[8]);}
NM_API_IMPL vec3 mat4_muldirvec(const mat4* m,const vec3* dir)   {return mat4_muldir(m,dir->x,dir->y,dir->z);}

NM_API_IMPL vec4 mat4_mulpos(const mat4* m,nmoat posX,nmoat posY,nmoat posZ,nmoat posW/*=(nmoat)1*/) {const nmoat* e=m->m;return make_vec4(posX*e[0] + posY*e[4] + posZ*e[8] + posW*e[12], posX*e[1] + posY*e[5] + posZ*e[9] + posW*e[13], posX*e[2] + posY*e[6] + posZ*e[10] + posW*e[14],posX*e[3] + posY*e[7] + posZ*e[11] + posW*e[15]);}
NM_API_IMPL vec4 mat4_mulposvec(const mat4* m,const vec4* pos)    {return mat4_mulpos(m,pos->x,pos->y,pos->z,pos->w);}

NM_API_IMPL const mat4* mat4_lookAt(mat4* m,nmoat eyeX,nmoat eyeY,nmoat eyeZ,nmoat centerX,nmoat centerY,nmoat centerZ,nmoat upX,nmoat upY,nmoat upZ)    {nmoat *e=m->m;nm_Mat4LookAt(e,eyeX,eyeY,eyeZ,centerX,centerY,centerZ,upX,upY,upZ);return m;}
NM_API_IMPL const mat4* mat4_perspective(mat4* m,nmoat degfovy,nmoat aspect, nmoat zNear, nmoat zFar)    {nmoat *e=m->m;nm_Mat4Perspective(e,degfovy,aspect,zNear,zFar);return m;}
NM_API_IMPL const mat4* mat4_ortho(mat4* m,nmoat left,nmoat right, nmoat bottom, nmoat top,nmoat nearVal,nmoat farVal)   {nmoat *e=m->m;nm_Mat4Ortho(e,left,right,bottom,top,nearVal,farVal);return m;}

NM_API_IMPL const mat4* mat4_translate(mat4* m,nmoat x,nmoat y, nmoat z)    {nm_Mat4Translate(m->m,x,y,z);return m;}
NM_API_IMPL const mat4* mat4_rotate(mat4* m,nmoat degAngle,nmoat x,nmoat y, nmoat z)    {nm_Mat4Rotate(m->m,degAngle,x,y,z);return m;}
NM_API_IMPL const mat4* mat4_scale(mat4* m,nmoat x,nmoat y, nmoat z)    {nm_Mat4Scale(m->m,x,y,z);return m;}

NM_API_IMPL const mat4* mat4_fromRotScaTra(mat4* m,const mat3* rot,const vec3* sca,const vec3* tra) {nm_Mat4FromMRotScaTra(m->m,rot->m,sca->x,sca->y,sca->z,tra->x,tra->y,tra->z);return m;}
NM_API_IMPL const mat4* mat4_lookAtYX(mat4* m,const vec3* lookAt,nmoat minDistance/*=0*/,nmoat maxDistance/*=0*/,nmoat pitchLimit/* = 1.483486111*/)    {nm_Mat4LookAtYX(m->m,lookAt->x,lookAt->y,lookAt->z,minDistance,maxDistance,pitchLimit);return m;}

NM_API_IMPL const mat4* mat4_slerp(mat4* m,const mat4* mStart,const mat4* mEnd,nmoat slerpTime_In_0_1)  {nm_Mat4Slerp(m->m,mStart->m,mEnd->m,slerpTime_In_0_1);return m;}


NM_API_IMPL const mat3* mat4_getMat3FromMatrixRotation(mat3* dst,const mat4* src)   {int i,j;for (i=0;i<3;i++) for (j=0;j<3;j++) dst->col[i][j]=src->col[i][j];return dst;}
NM_API_IMPL void mat4_setMat3ToMatrixRotation(mat4* dst,const mat3* src)    {int i,j;for (i=0;i<3;i++) for (j=0;j<3;j++) dst->col[i][j]=src->col[i][j];}

NM_API_IMPL void mat4_fromEulerXYZ(mat4* m,nmoat x,nmoat y, nmoat z)  {nm_Mat4FromEulerXYZ(m->m,x,y,z);}
NM_API_IMPL void mat4_fromEulerXZY(mat4* m,nmoat x,nmoat y, nmoat z)  {nm_Mat4FromEulerXZY(m->m,x,y,z);}
NM_API_IMPL void mat4_fromEulerYXZ(mat4* m,nmoat x,nmoat y, nmoat z)  {nm_Mat4FromEulerYXZ(m->m,x,y,z);}
NM_API_IMPL void mat4_fromEulerYZX(mat4* m,nmoat x,nmoat y, nmoat z)  {nm_Mat4FromEulerYZX(m->m,x,y,z);}
NM_API_IMPL void mat4_fromEulerZXY(mat4* m,nmoat x,nmoat y, nmoat z)  {nm_Mat4FromEulerZXY(m->m,x,y,z);}
NM_API_IMPL void mat4_fromEulerZYX(mat4* m,nmoat x,nmoat y, nmoat z)  {nm_Mat4FromEulerZYX(m->m,x,y,z);}

NM_API_IMPL int mat4_toEulerXYZ(const mat4* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat4ToEulerXYZ(m->m,x,y,z);}
NM_API_IMPL int mat4_toEulerXZY(const mat4* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat4ToEulerXZY(m->m,x,y,z);}
NM_API_IMPL int mat4_toEulerYXZ(const mat4* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat4ToEulerYXZ(m->m,x,y,z);}
NM_API_IMPL int mat4_toEulerYZX(const mat4* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat4ToEulerYZX(m->m,x,y,z);}
NM_API_IMPL int mat4_toEulerZXY(const mat4* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat4ToEulerZXY(m->m,x,y,z);}
NM_API_IMPL int mat4_toEulerZYX(const mat4* m,nmoat* x,nmoat* y,nmoat* z)   {return nm_Mat4ToEulerZYX(m->m,x,y,z);}

NM_API_IMPL void mat4_fromEuler(mat4* m,nmoat x,nmoat y,nmoat z,EulerRotationMode mode)   {nm_Mat4FromEuler(m->m,x,y,z,(int)mode);}
NM_API_IMPL int mat4_toEuler(const mat4* m,nmoat* x,nmoat* y,nmoat* z,EulerRotationMode mode)  {return nm_Mat4ToEuler(m->m,x,y,z,(int)mode);}

NM_API_IMPL int mat4_areequaleps(const mat4* a,const mat4* b,nmoat eps) {return nm_AreMat4EqualEps(a->m,b->m,eps);}
NM_API_IMPL int mat4_areequal(const mat4* a,const mat4* b)              {return nm_AreMat4Equal(a->m,b->m);}




#endif //NM_NO_MATH_STRUCTS

#ifdef NM_NAMESPACE
}
#endif //NM_NAMESPACE

#ifdef NM_WARNING_SUPPRESSION
#   if (defined(__GNUC__) || defined(__MINGW__) || defined(__clang__))
#       pragma GCC diagnostic pop
#   elif defined(_MSC_VER)
#       pragma warning( pop )
#   endif
#endif // NM_WARNING_SUPPRESSION

#endif //MINIMATH_IMPLEMENTATION_GUARD
#endif //MINIMATH_IMPLEMENTATION
