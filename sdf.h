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

/** EMBEDDED FONT LICENSE
 * This file embeds a sdf-AngelFont version of DejaVuSansCondensed-Bold.
 * It was derived from DejaVuSerifCondensed-Bold.ttf.
 * DejaVuSerifCondensed-Bold.ttf is released under the DejaVu Fonts License v1.00.
 * The license can be found at this permalink: http://www.fontsquirrel.com/license/dejavu-sans
 * The embedded font can be disabled by defining SDF_NO_EMBEDDED_FONT before including this file.
*/

// WHAT'S THIS
/*

This file is intended to be used to load AngelFont text-based (= not binary or XML-based) Signed Distance Fonts (SDF) files
and to display them onscreen, NOT INSIDE A 3D WORLD, but as a screen overlay in normal openGL rendering.

This is typically used for title/subtitle/credit screens and things like that...

Dependencies:
1) Needs OpenGL WITH SHADER support.
2) Implementation depends on STL.
*/

// HOW TO CREATE COMPATIBLE FONTS:
/*
The best solution I've found is the java program: runnable-hiero.jar, available here: https://libgdx.badlogicgames.com/tools.html

This is how I usually generate the fonts using Hiero:
I STRICTLY follow all the guidelines under "Generating the font" in https://github.com/libgdx/libgdx/wiki/Distance-field-fonts,
with the following changes:
0) I always choose a bold font (and I prefer bold-condensed whenever available).
1) I always use "Rendering" set to "Java".
2) I can add additional codepoints directly in the "Sample Text" edit box (after that I select "Glyph cache" and "reset cache" to update them).
3) I always output a single .png page (support for multiple pages is yet to come).
4) The "Scale" value must be the last to be set: the docs suggest 32, but if you have a fast PC try something bigger (48 or 64).
5) The output .png size can be easily reduced by using 3rd party programs (e.g. pngnq -n 48 myImage.png).
*/

/* MINIMAL BASIC USAGE (Just to see something on screen: the API is much more complex):
 0) At the top of your main.cpp file:
 #define SDF_IMPLEMENTATION
 #include <sdf.h>   // this file
 // Note that the OpenGL headers must be included BEFORE including <sdf.h>

 1) Inside InitGL():
 Sdf::SdfCharset* gDefaultCharset = Sdf::SdfAddDefaultCharset();    // DejaVuSerifCondensed-Bold
 Sdf::SdfTextChunk* gTextChunks = Sdf::SdfAddTextChunk(gDefaultCharset);
 Sdf::SdfAddText(gTextChunks,"This is a test text.");
 // Please do not delete gDefaultCharset and gTextChunks yourself!

 2) At the end of DrawGL():
 float viewport[4];glGetFloatv(GL_VIEWPORT,viewport);
 Sdf::SdfRender(viewport,(float)(glutGet(GLUT_ELAPSED_TIME)*0.001));    // Last arg is probably used only for text animations and for this example can probably be left to zero
 // The line above should render all the active SdfTextChunks

 3) Inside DestroyGL():
 Sdf::SdfDestroy();   // Frees everything
*/


#ifndef SDF_H_
#define SDF_H_


#ifndef SDF_NO_NAMESPACE
namespace Sdf {
#endif

#ifndef SDF_VEC2_REPLACEMENT
struct SdfVec2 {
    float x,y;
    SdfVec2() : x(0),y(0) {}
    SdfVec2(float _x,float _y) : x(_x),y(_y) {}
    inline const float& operator[](int i) const {return (const float&) (*((const float*)this + i));}
    inline float&       operator[](int i)       {return (float&) (*((float*)this + i));}
    inline SdfVec2 operator*(const SdfVec2& r) const {return SdfVec2(x*r.x,y*r.y);}
    inline SdfVec2 operator*(float f) const {return SdfVec2(x*f,y*f);}
    inline SdfVec2 operator+(const SdfVec2& r) const {return SdfVec2(x+r.x,y+r.y);}
    inline SdfVec2 operator-(const SdfVec2& r) const {return SdfVec2(x-r.x,y-r.y);}

};
#   ifndef SDF_NO_NAMESPACE
typedef SdfVec2 Vec2;
#   endif
#endif

#ifndef SDF_VEC4_REPLACEMENT
struct SdfVec4 {
    float x,y,z,w;
    SdfVec4() : x(0),y(0),z(0),w(0) {}
    SdfVec4(float _x,float _y,float _z,float _w) : x(_x),y(_y),z(_z),w(_w) {}
    inline const float& operator[](int i) const {return (const float&) (*((const float*)this + i));}
    inline float&       operator[](int i)       {return (float&) (*((float*)this + i));}
};
#   ifndef SDF_NO_NAMESPACE
typedef SdfVec4 Vec4;
#   endif
#endif

// Mandatory Stuff---------------------------------
void SdfDestroy();

// Charsets --------------------------------------------------------------------------------------
struct SdfCharsetProperties {
    bool flipYOffset;
    SdfCharsetProperties(bool _flipYOffset=false) : flipYOffset(_flipYOffset) {}
};
// Tip: load the texture into fntTexture (owned by you), before calling these methods
struct SdfCharset* SdfAddCharsetFromFile(const char* fntFilePath,GLuint fntTexture,const SdfCharsetProperties& properties=SdfCharsetProperties(),bool charsetWillDeleteFntTexture=false);
struct SdfCharset* SdfAddCharsetFromMemory(const void* data,unsigned int data_size,GLuint fntTexture,const SdfCharsetProperties& properties=SdfCharsetProperties(),bool charsetWillDeleteFntTexture=false);
// Same as above, except that the texture is created internally from a 32 bit raw RGBA image (with no additional stride). It must be: sizeof(fntRGBAData)=4*fntRGBAWidth*fntRGBAHeight
struct SdfCharset* SdfAddCharsetAndTextureFromMemory(const void* fntData,unsigned int fntDataSize,const void* fntRGBAData,int fntRGBAWidth,int fntRGBAHeight,const SdfCharsetProperties& properties=SdfCharsetProperties());
#ifndef SDF_NO_EMBEDDED_FONT
// In this case a fntTexture is always handled for you (and destroyed in the SdfCharset destructor)
struct SdfCharset* SdfAddDefaultCharset(const SdfCharsetProperties& properties=SdfCharsetProperties());
#endif //SDF_NO_EMBEDDED_FONT
//-------------------------------------------------------------------------------------------------

// TextChunks -------------------------------------------------------------------------------------
enum  SDFTextBufferType {
    SDF_BT_REGULAR=0,
    SDF_BT_OUTLINE=1,
    SDF_BT_SHADOWED=2
};
enum  SDFHAlignment {
    SDF_LEFT=0,
    SDF_CENTER,
    SDF_RIGHT,
    SDF_JUSTIFY
};
enum SDFVAlignment {
    SDF_TOP=0,
    SDF_MIDDLE,
    SDF_BOTTOM
};
struct SdfTextChunkProperties {
    SdfVec2 boundsCenter;        // in normalized units relative to the screen size
    SdfVec2 boundsHalfSize;      // in normalized units relative to the screen size
    float maxNumTextLines;      // This will determine the font size ( = boundsSizeInPixels/maxNumTextLines )
    float lineHeightOverride;  // Used if > 0.f. Usually LineHeight is something like 1.2f [that means 1.2f times the FontSize(==FontHeight)]
    SDFHAlignment halign;
    SDFVAlignment valign;
    SdfTextChunkProperties(float _maxNumTextLines=20,SDFHAlignment _halign=SDF_CENTER,SDFVAlignment _valign=SDF_MIDDLE,const SdfVec2& _boundsCenter=SdfVec2(.5f,.5f),const SdfVec2& _boundsHalfSize=SdfVec2(.5f,.5f),float _lineHeightOverride=0.f) {
        boundsCenter = _boundsCenter;
        boundsHalfSize = _boundsHalfSize;
        maxNumTextLines = _maxNumTextLines;
        lineHeightOverride = _lineHeightOverride;
        halign = _halign;
        valign = _valign;
    }
};
struct SdfTextChunk* SdfAddTextChunk(struct SdfCharset* _charset,int sdfBufferType=SDF_BT_OUTLINE, const SdfTextChunkProperties& properties=SdfTextChunkProperties(),bool preferStreamDrawBufferUsage=false);
SdfTextChunkProperties& SdfTextChunkGetProperties(struct SdfTextChunk* textChunk);
const SdfTextChunkProperties& SdfTextChunkGetProperties(const struct SdfTextChunk* textChunk);
void SdfTextChunkSetStyle(struct SdfTextChunk* textChunk,int sdfTextBufferType=SDF_BT_OUTLINE);
int SdfTextChunkGetStyle(const struct SdfTextChunk* textChunk);
void SdfTextChunkSetMute(struct SdfTextChunk* textChunk,bool flag); // Mute makes it invisible
bool SdfTextChunkGetMute(const struct SdfTextChunk* textChunk);
void SdfRemoveTextChunk(struct SdfTextChunk* chunk);
void SdfRemoveAllTextChunks();
//--------------------------------------------------------------------------------------------------

// Text---------------------------------------------------------------------------------------------
struct SdfTextColor {
    SdfVec4 colorTopLeft;
    SdfVec4 colorTopRight;
    SdfVec4 colorBottomLeft;
    SdfVec4 colorBottomRight;
    SdfTextColor(const SdfVec4& color=SdfVec4(1,1,1,1)) : colorTopLeft(color),colorTopRight(color),colorBottomLeft(color),colorBottomRight(color) {}
    SdfTextColor(const SdfVec4& colorTop,const SdfVec4& colorBottom) : colorTopLeft(colorTop),colorTopRight(colorTop),colorBottomLeft(colorBottom),colorBottomRight(colorBottom) {}
    SdfTextColor(const SdfVec4& _colorTopLeft,const SdfVec4& _colorTopRight,const SdfVec4& _colorBottomLeft,const SdfVec4& _colorBottomRight)
    : colorTopLeft(_colorTopLeft),colorTopRight(_colorTopRight),colorBottomLeft(_colorBottomLeft),colorBottomRight(_colorBottomRight) {}
    SdfTextColor(const unsigned int& in) {
         const float s = 1.0f/255.0f;
         const SdfVec4 color(
        ((in >> 0) & 0xFF) * s,
        ((in >> 8) & 0xFF) * s,
        ((in >> 16) & 0xFF) * s,
        ((in >> 24) & 0xFF) * s);
        colorTopLeft=colorTopRight=colorBottomLeft=colorBottomRight=color;
    }
    static void SetDefault(const SdfTextColor& defaultColor, bool updateAllExistingTextChunks=false);
};
static SdfTextColor SdfTextDefaultColor;
void SdfAddText(struct SdfTextChunk* chunk,const char* startText,bool italic=false,const SdfTextColor* pSdfTextColor=NULL,const SdfVec2* textScaling=NULL,const char* endText=NULL,const SDFHAlignment* phalignOverride=NULL,bool fakeBold=false);
void SdfAddTextWithTags(struct SdfTextChunk* chunk,const char* startText,const char* endText=NULL);
void SdfClearText(struct SdfTextChunk* chunk);
//---------------------------------------------------------------------------------------------------


void SdfRender(const float pViewport[4],float globalTime);   // pViewport is [x,y,width,height] in screen coordinates, not in framebuffer coords.


// Optional/Extra methods:---------------------------------------------------------------
enum SDFAnimationMode {
    SDF_AM_NONE = 0,
    SDF_AM_MANUAL,      // This mode uses a SdfAnimation (= a series of SdfAnimationKeyFrames)

    SDF_AM_FADE_IN,
    SDF_AM_ZOOM_IN,
    SDF_AM_APPEAR_IN,
    SDF_AM_LEFT_IN,
    SDF_AM_RIGHT_IN,
    SDF_AM_TOP_IN,
    SDF_AM_BOTTOM_IN,

    SDF_AM_FADE_OUT,
    SDF_AM_ZOOM_OUT,
    SDF_AM_APPEAR_OUT,
    SDF_AM_LEFT_OUT,
    SDF_AM_RIGHT_OUT,
    SDF_AM_TOP_OUT,
    SDF_AM_BOTTOM_OUT,

    SDF_AM_BLINK,
    SDF_AM_PULSE,
    SDF_AM_TYPING
};
struct SdfAnimationKeyFrame {
    SdfVec2 offset;
    SdfVec2 scale;   // better not use this, as it's not centered correctly (= it affects offset)
    float alpha;
    int startChar,endChar;
    float timeInSeconds;    // of the transition between the previous one and this
    SdfAnimationKeyFrame(float _timeInSeconds=0.f,float _alpha=1.f,int _startChar=0,int _endChar=-1,const SdfVec2& _offset=SdfVec2(0,0),const SdfVec2& _scale=SdfVec2(1,1)) :
    offset(_offset),scale(_scale),alpha(_alpha),startChar(_startChar),endChar(_endChar),timeInSeconds(_timeInSeconds)
    {}
};
struct SdfAnimation* SdfAddAnimation();
void SdfAnimationSetLoopingParams(struct SdfAnimation* animation,bool mustLoop,bool mustHideTextWhenFinishedIfNotLooping=true);
float SdfAnimationAddKeyFrame(struct SdfAnimation* animation,const SdfAnimationKeyFrame& keyFrame); // returns the animation total length in seconds so far
void SdfAnimationClear(struct SdfAnimation* animation);     // clears all SdfKeyFrames
void SdfRemoveAnimation(struct SdfAnimation* animation);    // "animation" no more usable
void SdfRemoveAllAnimations();


struct SdfAnimationParams {
    float speed,timeOffset;
    int startChar,endChar;
    SdfAnimationParams()
    {
        speed = 1.f;timeOffset=0.f;
        startChar=0;endChar=-1;
    }
};
struct SdfGlobalParams {
    SdfVec2 offset,scale;
    float alpha;
    int startChar,endChar;
    SdfGlobalParams()
    {
        offset=SdfVec2(0,0);
        scale=SdfVec2(1,1);
        alpha=1.0f;
        startChar=0;endChar=-1;
    }
};
// Once an animation is active (from its mode), it plays if the text chunk is not mute.
// When it ends the mode can be set to SDF_AM_NONE and the text chunk BAN be set to mute.
// Only a manual animation can have a looping mode.
void SdfTextChunkSetManualAnimation(struct SdfTextChunk* chunk,struct SdfAnimation* animation);
const struct SdfAnimation* SdfTextChunkGetManualAnimation(const struct SdfTextChunk* chunk);
struct SdfAnimation* SdfTextChunkGetManualAnimation(struct SdfTextChunk* chunk);
void SdfTextChunkSetAnimationParams(struct SdfTextChunk* chunk,const SdfAnimationParams& params=SdfAnimationParams());
const SdfAnimationParams& SdfTextChunkGetAnimationParams(const struct SdfTextChunk* chunk);
SdfAnimationParams& SdfTextChunkGetAnimationParams(struct SdfTextChunk* chunk);
void SdfTextChunkSetGlobalParams(struct SdfTextChunk* chunk,const SdfGlobalParams& params=SdfGlobalParams());
const SdfGlobalParams& SdfTextChunkGetGlobalParams(const struct SdfTextChunk* chunk);
SdfGlobalParams& SdfTextChunkGetGlobalParams(struct SdfTextChunk* chunk);
void SdfTextChunkSetAnimationMode(struct SdfTextChunk* chunk,SDFAnimationMode mode=SDF_AM_NONE);
SDFAnimationMode SdfTextChunkGetAnimationMode(const struct SdfTextChunk* chunk);
//------------------------------------------------------------------------------------------




#ifndef SDF_NO_NAMESPACE
} //namespace
#endif

#endif //SDF_H_

#ifdef SDF_IMPLEMENTATION
#ifndef SDF_IMPLEMENTATION_H
#define SDF_IMPLEMENTATION_H

#include <stdio.h>  // FILE* used in SdfCharset::GetFileContent(...)
#include <ctype.h> // tolower
#include <stdint.h>	// uint8_t

#ifndef SDF_ASSERT
#include <assert.h>
#define SDF_ASSERT(X) assert(X)
#endif //SDF_ASSERT

#ifndef SdfVector
#include <vector>
#define SdfVector std::vector
#endif

#ifndef SdfMap
//#include <hashtable.h>
//#define SdfMap std::hashtable
#include <map>
#define SdfMap std::map
//#include <unordered_map>
//#define SdfMap std::unordered_map
#endif

#ifndef SdfUIntPair
#include <utility>  // std::pair
#define SdfUIntPair std::pair<unsigned int,unsigned int>
#endif

#ifndef SDF_MALLOC
#define SDF_MALLOC(X) malloc(X)
#endif

#ifndef SDF_FREE
#define SDF_FREE(X) free(X)
#endif

#ifndef SDF_PLACEMENT_NEW
struct SdfPlacementNewDummy {};
inline void* operator new(size_t, SdfPlacementNewDummy, void* ptr) { return ptr; }
inline void operator delete(void*, SdfPlacementNewDummy, void*) {}
#define SDF_PLACEMENT_NEW(_PTR)  new(SdfPlacementNewDummy(), _PTR)
#endif

#ifndef SDF_NO_NAMESPACE
namespace Sdf {
#endif //SDF_NO_NAMESPACE

class UTF8Helper {
    /* All the code in this class is based on the code found at: http://bjoern.hoehrmann.de/utf-8/decoder/dfa/

   This is the license of the ORIGINAL work at http://bjoern.hoehrmann.de/utf-8/decoder/dfa/:

    Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

    All the things I've added to the original code are in the public domain (but be warned that might be wrong and incorrected...)
*/
public:
protected:
    UTF8Helper() {}

public:

    enum {
        UTF8_ACCEPT=0,
        UTF8_REJECT=1
    };

protected:
    inline static uint8_t utf8d(uint32_t byte) {
        static const uint8_t _utf8d[] = {
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
            7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
            8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
            0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
            0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
            0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
            1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
            1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
            1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
        };
        return _utf8d[byte];
    }
public:
    inline static uint32_t decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
        uint32_t type = utf8d(byte);

        *codep = (*state != UTF8_ACCEPT) ?
                    (byte & 0x3fu) | (*codep << 6) :
                    (0xff >> type) & (byte);

        *state = utf8d(256 + *state*16 + type);
        return *state;
    }

}; // class


static SdfTextColor gSdfTextDefaultColor(SdfVec4(1,1,1,1));
static SdfVec2 gSdfDisplaySize(800,450);

class SdfVertexBuffer {
public:
    void clear() {verts.clear();}
    ~SdfVertexBuffer() {if (vbo) glDeleteBuffers(1,&vbo);}
    void updateBoundVbo() const {
        const GLenum drawMode = preferStreamDrawBufferUsage ? GL_STREAM_DRAW : GL_STATIC_DRAW;
        //glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(VertexDeclaration),verts.size()!=0 ? &verts[0].posAndUV.x : NULL, drawMode);return;

        if (maxVertsSize<(int)verts.size()) maxVertsSize=(int)verts.size();
        glBufferData(GL_ARRAY_BUFFER, maxVertsSize*sizeof(VertexDeclaration), NULL, drawMode);
        if (verts.size()>0) {
            glBufferSubData(GL_ARRAY_BUFFER,0,verts.size()*sizeof(VertexDeclaration),&verts[0].posAndUV.x);
            //fprintf(stderr,"%d indices\n",verts.size());
        }
    }
    inline int getType() const {return type;}
    inline void setType(int _type) {type=_type;}
    inline const SdfVec4* getColorOfVert(int num) const {return (int)verts.size()>num*6 ? &verts[num*6].color : NULL;}
    inline int size() const {return verts.size();}
    inline int numChars() const {return verts.size()/6;}
#ifndef _MSC_VER    // Not sure, but old cl compilers can have problems here
protected:
#endif //_MSC_VER
    SdfVertexBuffer(int _type=SDF_BT_OUTLINE,bool _preferStreamDrawBufferUsage=false) : type(_type),preferStreamDrawBufferUsage(_preferStreamDrawBufferUsage) {maxVertsSize=0;vbo=0;}
    struct VertexDeclaration {
        SdfVec4 posAndUV;
        SdfVec4 color;
    };
    SdfVector<VertexDeclaration> verts;
    GLuint vbo;
    int type;
    bool preferStreamDrawBufferUsage;

    mutable int maxVertsSize;
    friend struct SdfCharset;
    friend struct SdfTextChunk;
    friend void SdfRender(const float pViewport[4],float globalTime);
    friend bool SdfTextChunkEdit(SdfTextChunk* sdfTextChunk,char* buffer,int bufferSize);
};

struct SdfCharDescriptor
{
    unsigned int Id;                // The character id.
    float X, Y;            // The left and top position of the character image in the texture.
    float Width, Height;   // The width and height of the character image in the texture.
    float XOffset, YOffset;         // How much the current position should be offset when copying the image from the texture to the screen   ( top/left offsets ).
    float XOffset2, YOffset2;         // How much the current position should be offset when copying the image from the texture to the screen ( lower/right offsets ).
    float XAdvance;                 // How much the current position should be advanced after drawing the character.
    unsigned char Page;             // The texture page where the character image is found.

    SdfCharDescriptor() : Id(0), X( 0 ), Y( 0 ), Width( 0 ), Height( 0 ), XOffset( 0 ), YOffset( 0 ), XOffset2( 0 ), YOffset2( 0 ),
        XAdvance( 0 ), Page( 0 )
    { }
};

struct SdfAnimation {
    //public:
    SdfAnimation() {totalTime=0.f;}
    SdfAnimation(const SdfAnimation& o) {*this=o;totalTime=0.f;}
    SdfAnimation(const SdfVector<SdfAnimationKeyFrame>& _keyFrames) {cloneKeyFramesFrom(_keyFrames);totalTime=0.f;looping=false;mustMuteAtEnd=true;}
    ~SdfAnimation() {}
    const SdfAnimation& operator=(const SdfAnimation& o)    {
        cloneKeyFramesFrom(o.keyFrames);
        return *this;
    }
    //protected:
    SdfVector<SdfAnimationKeyFrame> keyFrames;
    float totalTime;
    bool looping;
    bool mustMuteAtEnd;
    inline void cloneKeyFramesFrom(const SdfVector<SdfAnimationKeyFrame>& _keyFrames) {
        // deep clone keyframes
        const int nkf = _keyFrames.size();
        keyFrames.clear();keyFrames.reserve(nkf);
        for (int i=0;i<nkf;i++) keyFrames[i] = _keyFrames[i];
    }
    inline float addKeyFrame(const SdfAnimationKeyFrame& kf) {
        keyFrames.push_back(kf);
        totalTime+=kf.timeInSeconds;
        return totalTime;
    }
    inline bool removeKeyFrameAt(int i) {
        if (i<0 || i>=(int)keyFrames.size()) return false;
        for (int j=i,jsz=keyFrames.size()-1;j<jsz;j++) keyFrames[j] = keyFrames[j+1];
        keyFrames.pop_back();
        update();
        return true;
    }
    inline void clear() {keyFrames.clear();totalTime=0.f;}
    inline void update() {
        totalTime = 0.f;
        for (int i=0,isz=keyFrames.size();i<isz;i++)    {
            totalTime+=keyFrames[i].timeInSeconds;
        }
    }
};

struct SdfTextChunk {
    SdfTextChunkProperties props;
    const SdfCharset* charset;
    SdfVertexBuffer* buffer;
    int numValidGlyphs;
    mutable bool dirty;
    SdfVec2 shadowOffsetInPixels;
    bool mute;

    SDFAnimationMode animationMode;
    SdfAnimation* manualAnimationRef;
    float animationStartTime;
    SdfAnimationParams animationParams;
    SdfGlobalParams globalParams;

    struct TextBit {
    SdfVector<SdfCharDescriptor> charDescriptors;
    SdfVector<float> kernings;
        SdfVec2 scaling;
        SdfTextColor sdfTextColor;
        bool italic;
	int hAlignment;
    };
    SdfVector<TextBit> textBits;
    SdfTextChunk() {charset=NULL;buffer=NULL;numValidGlyphs=0;shadowOffsetInPixels.x=shadowOffsetInPixels.y=4.f;mute = false;animationMode=SDF_AM_NONE;manualAnimationRef=NULL;animationStartTime=-1.f;tmpVisible = true;tmpLocalTime=0.f;}
    SdfTextChunk(const SdfCharset* _charset,int bufferType,const SdfTextChunkProperties& properties=SdfTextChunkProperties(),bool preferStreamDrawBufferUsage=false) {
        buffer = (SdfVertexBuffer*) SDF_MALLOC(sizeof(SdfVertexBuffer));
        SDF_PLACEMENT_NEW (buffer) SdfVertexBuffer(bufferType,preferStreamDrawBufferUsage);
        SDF_ASSERT(buffer);
        charset = _charset;
        SDF_ASSERT(charset);
        props = properties;
        dirty = true;
        numValidGlyphs=0;
        shadowOffsetInPixels.x=shadowOffsetInPixels.y=4.f;
        mute = false;
        animationMode=SDF_AM_NONE;manualAnimationRef=NULL;animationStartTime=-1.f;
        tmpVisible = true;tmpLocalTime=0.f;
    }
    ~SdfTextChunk() {
        if (buffer) {
            buffer->~SdfVertexBuffer();
            SDF_FREE(buffer);
	    buffer=NULL;
	}
    }
    inline void setMute(bool flag) {
        if (mute!=flag) {
            animationStartTime = -1.f;
            mute = flag;
        }
    }

    // These tmp variables are valid per frame and set by next methods:
    mutable bool tmpVisible;
    mutable float tmpLocalTime;
    mutable SdfAnimationKeyFrame tmpKeyFrame;

    inline static float Lerp(float t,float v0,float v1) {return v0+t*(v1-v0);}
    inline static int LerpInt(float t,int v0,int v1) {return v0+(int)((t*(float)(v1-v0))+0.5f);}
    inline static void Lerp(float t,SdfAnimationKeyFrame& kfOut,const SdfAnimationKeyFrame& kf0,const SdfAnimationKeyFrame& kf1,int numGlyphs)    {
        kfOut.alpha     = (kf0.alpha==kf1.alpha)		? kf0.alpha	: Lerp(t,kf0.alpha,kf1.alpha);
        kfOut.offset.x	= (kf0.offset.x==kf1.offset.x)	? kf0.offset.x	: Lerp(t,kf0.offset.x,kf1.offset.x);
        kfOut.offset.y	= (kf0.offset.y==kf1.offset.y)	? kf0.offset.y	: Lerp(t,kf0.offset.y,kf1.offset.y);
        kfOut.scale.x	= (kf0.scale.x==kf1.scale.x)	? kf0.scale.x	: Lerp(t,kf0.scale.x,kf1.scale.x);
        kfOut.scale.y	= (kf0.scale.y==kf1.scale.y)	? kf0.scale.y	: Lerp(t,kf0.scale.y,kf1.scale.y);
	kfOut.startChar = (kf0.startChar==kf1.startChar)    ? kf0.startChar : LerpInt(t,kf0.startChar,kf1.startChar);
	kfOut.endChar	= (kf0.endChar==kf1.endChar)	? kf0.endChar	: LerpInt(t,(kf0.endChar<0)?numGlyphs:kf0.endChar,(kf1.endChar<0)?numGlyphs:kf1.endChar);
    }

    inline bool checkVisibleAndEvalutateAnimationIfNecessary(float time) {
        if (mute || (animationMode==SDF_AM_MANUAL && (!manualAnimationRef || manualAnimationRef->keyFrames.size()==0 || manualAnimationRef->totalTime==0.f))) return (tmpVisible=false);
        if (animationStartTime < 0) animationStartTime = time;
        tmpLocalTime=(time-animationStartTime)*animationParams.speed-animationParams.timeOffset;
        if (tmpLocalTime<0) return (tmpVisible=false);
        if (animationMode==SDF_AM_MANUAL && tmpLocalTime>manualAnimationRef->totalTime)   {
            // return (tmpVisible=false);
            if (!manualAnimationRef->looping) {
                animationStartTime=-1.f;animationMode=SDF_AM_NONE;
                if (manualAnimationRef->mustMuteAtEnd) {setMute(true);return (tmpVisible=false);}
                else {/*tmpKeyFrame = SdfAnimationKeyFrame();*/setMute(false);return (tmpVisible=true);}
            }
            else if (manualAnimationRef->totalTime>0.f) {
                while (tmpLocalTime>manualAnimationRef->totalTime) {tmpLocalTime-=manualAnimationRef->totalTime;animationStartTime+=manualAnimationRef->totalTime;}
                //fprintf(stderr,"time: %1.4f/%1.4f (%d frames)\n",tmpLocalTime,manualAnimationRef->totalTime,manualAnimationRef->keyFrames.size());
                }
        }
        // evalutate animation here:
        tmpKeyFrame = SdfAnimationKeyFrame();
        switch (animationMode)  {
        case SDF_AM_FADE_IN:  {if (tmpLocalTime<1.f)  tmpKeyFrame.alpha = tmpLocalTime;else  {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}}
            break;
        case SDF_AM_FADE_OUT:  {if (tmpLocalTime<1.f)  tmpKeyFrame.alpha = 1.f-tmpLocalTime;else {setMute(true);animationStartTime=-1.f;animationMode=SDF_AM_NONE;return (tmpVisible=false);}}
            break;
        case SDF_AM_ZOOM_IN:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = tmpLocalTime;   // in [0,1]
                tmpLocalTime = (2.0f-tmpLocalTime); // in [2,1]
                tmpLocalTime = tmpLocalTime*tmpLocalTime*tmpLocalTime;  // in [8,1]
                tmpKeyFrame.scale.x=tmpKeyFrame.scale.y = tmpLocalTime;
            }
            else  {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}}
            break;
        case SDF_AM_ZOOM_OUT:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = 1.f-tmpLocalTime;
                tmpLocalTime = (1.0f+tmpLocalTime); // in [1,2]
                tmpLocalTime = tmpLocalTime*tmpLocalTime*tmpLocalTime;  // in [1,8]
                tmpKeyFrame.scale.x=tmpKeyFrame.scale.y = tmpLocalTime;
            }
            else {setMute(true);animationStartTime=-1.f;animationMode=SDF_AM_NONE;return (tmpVisible=false);}}
            break;    
        case SDF_AM_APPEAR_IN:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = tmpLocalTime;   // in [0,1]
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.scale.y = tmpLocalTime;
            }
            else  {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}
        }
        break;
        case SDF_AM_APPEAR_OUT:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = 1.f-tmpLocalTime;
                tmpLocalTime = tmpKeyFrame.alpha;
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.scale.y = tmpLocalTime;
            }
            else {setMute(true);animationStartTime=-1.f;animationMode=SDF_AM_NONE;return (tmpVisible=false);}}
        break;
        case SDF_AM_LEFT_IN:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = tmpLocalTime;   // in [0,1]
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.x = -1.0f + tmpLocalTime;
            }
            else  {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}}
        break;
        case SDF_AM_LEFT_OUT:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = 1.f-tmpLocalTime;
                tmpLocalTime = tmpKeyFrame.alpha;
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.x = -1.0f + tmpLocalTime;
            }
            else {setMute(true);animationStartTime=-1.f;animationMode=SDF_AM_NONE;return (tmpVisible=false);}}
        break;
         case SDF_AM_RIGHT_IN:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = tmpLocalTime;   // in [0,1]
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.x = 1.0f - tmpLocalTime;
            }
            else  {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}}
        break;
        case SDF_AM_RIGHT_OUT:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = 1.f-tmpLocalTime;
                tmpLocalTime = tmpKeyFrame.alpha;
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.x = 1.0f - tmpLocalTime;
            }
            else {setMute(true);animationStartTime=-1.f;animationMode=SDF_AM_NONE;return (tmpVisible=false);}}
        break;
        case SDF_AM_TOP_IN:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = tmpLocalTime;   // in [0,1]
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.y = -1.0f + tmpLocalTime;
            }
            else  {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}}
        break;
        case SDF_AM_TOP_OUT:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = 1.f-tmpLocalTime;
                tmpLocalTime = tmpKeyFrame.alpha;
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.y = -1.0f + tmpLocalTime;
            }
            else {setMute(true);animationStartTime=-1.f;animationMode=SDF_AM_NONE;return (tmpVisible=false);}}
        break;
         case SDF_AM_BOTTOM_IN:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = tmpLocalTime;   // in [0,1]
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.y = 1.0f - tmpLocalTime;
            }
            else  {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}}
        break;
        case SDF_AM_BOTTOM_OUT:  {
            if (tmpLocalTime<1.f)  {
                tmpKeyFrame.alpha = 1.f-tmpLocalTime;
                tmpLocalTime = tmpKeyFrame.alpha;
                tmpLocalTime *= tmpLocalTime;
                tmpKeyFrame.offset.y = 1.0f - tmpLocalTime;
            }
            else {setMute(true);animationStartTime=-1.f;animationMode=SDF_AM_NONE;return (tmpVisible=false);}}
        break;
        case SDF_AM_BLINK:   {float tmp = (float) ((((int)(tmpLocalTime*100.f))%101)-50)*0.02f;tmpKeyFrame.alpha = tmp*tmp;}
            break;
        case SDF_AM_PULSE:   {
            tmpKeyFrame.scale.x=tmpKeyFrame.scale.y = 1.0f+((0.005f*20.f)/(float)(this->props.maxNumTextLines))*sinf(tmpLocalTime*10.0f);
            }
            break;
        case SDF_AM_TYPING:   {
            static const float timePerChar = 0.15f;
            const int numChars = buffer->numChars();
            const float timeForAllChars = timePerChar * numChars;
            if (timeForAllChars>0)  {
                if (tmpLocalTime<=timeForAllChars)	{
		    tmpKeyFrame.endChar = (int)(((tmpLocalTime/timeForAllChars)*(float)(numChars))+0.5f);
                }
                else {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}
            }
            else {setMute(false);animationStartTime=-1.f;animationMode=SDF_AM_NONE;}
        }
            break;
        case SDF_AM_MANUAL: {
            const SdfAnimation& an = *manualAnimationRef;
            const int nkf = an.keyFrames.size();
            static SdfAnimationKeyFrame ZeroKF(0.f,0.f);
            float sumTime = 0;int i=0;const SdfAnimationKeyFrame* kf = NULL;
            for (i=0;i<nkf;i++) {
                kf = &an.keyFrames[i];
                if (kf->timeInSeconds==0.f) continue;
                if (tmpLocalTime <= sumTime + kf->timeInSeconds) break;
                sumTime+=kf->timeInSeconds;
            }
	    //if (!kf) kf = (nkf>0 && animationParams.looping) ? &an.keyFrames[nkf-1] : &ZeroKF;
	    //fprintf(stderr,"FRAME: %d\n",i);
        SDF_ASSERT(kf && kf->timeInSeconds);
        const SdfAnimationKeyFrame* kf_prev = (i>=1 ? &an.keyFrames[i-1] : (nkf>0 &&
            (tmpLocalTime>manualAnimationRef->totalTime && manualAnimationRef->looping) // Not sure what I meant here... now I'm just correcting what it was: "tmpLocalTime>manualAnimationRef->looping", but looping is a bool. What did I mean ?
            ) ? &an.keyFrames[nkf-1] : &ZeroKF);
            const float deltaTime = (tmpLocalTime-sumTime)/kf->timeInSeconds;   // in [0,1]
            SDF_ASSERT(deltaTime>=0.f && deltaTime<=1.f);
            Lerp(deltaTime,tmpKeyFrame,*kf_prev,*kf,buffer->numChars());
        }
            break;
        default:
            break;
        }

    const bool applyAnimationParams = true;
    if (applyAnimationParams)   {
        // Here we apply animationParams to the calculated fields
        tmpKeyFrame.alpha*=globalParams.alpha;
        tmpKeyFrame.offset.x+=globalParams.offset.x;
        tmpKeyFrame.offset.y+=globalParams.offset.y;
        tmpKeyFrame.scale.x*=globalParams.scale.x;
        tmpKeyFrame.scale.y*=globalParams.scale.y;
    }

    return (tmpVisible=(tmpLocalTime>=0));
    }

    inline bool setupUniformValuesAndDrawArrays(struct SdfShaderProgram* SP, bool shadowPass, const SdfVec2 &screenSize);


    inline void addText(const char* startText,bool _italic,const SdfTextColor* pSdfTextColor,const SdfVec2* textScaling,const char* endText,const SDFHAlignment *phalignOverride, bool fakeBold);
    inline void clearText() {
        if (buffer) buffer->clear();
	textBits.clear();
        dirty=false;
        numValidGlyphs=0;
    }
    inline void assignText(const char* startText,bool _italic,const SdfTextColor* pSdfTextColor,const SdfVec2* textScaling,const char* endText,const SDFHAlignment *phalignOverride, bool fakeBold) {
        clearText();
        addText(startText,_italic,pSdfTextColor,textScaling,endText,phalignOverride,fakeBold);
    }

    bool endText(SdfVec2 screenSize=SdfVec2(-1,-1));
};

struct SdfShaderProgram {
    GLuint program;
    GLint uniformLocationOrthoMatrix;
    GLint uniformLocationSampler;
    GLint uniformLocationOffsetAndScale;
    GLint uniformLocationAlphaAndShadow;
    SdfShaderProgram() : program(0),uniformLocationOrthoMatrix(-1),uniformLocationSampler(-1),
    uniformLocationOffsetAndScale(-1),uniformLocationAlphaAndShadow(-1) {}
    ~SdfShaderProgram() {destroy();}
    void destroy() {
        if (program) {glDeleteProgram(program);program=0;}
    }
    bool loadShaderProgram(bool forOutlineShaderProgram) {
        if (program) return true;
        program = CompileShaderProgramAndSetCorrectAttributeLocations(forOutlineShaderProgram);
        if (program)    {
            uniformLocationOrthoMatrix = glGetUniformLocation(program,"ortho");
            uniformLocationSampler = glGetUniformLocation(program,"Texture");
            uniformLocationOffsetAndScale = glGetUniformLocation(program,"offsetAndScale");
            uniformLocationAlphaAndShadow = glGetUniformLocation(program,"alphaAndShadow");
            SDF_ASSERT(uniformLocationOrthoMatrix!=-1);
            SDF_ASSERT(uniformLocationSampler!=-1);
            SDF_ASSERT(uniformLocationOffsetAndScale!=-1);
            SDF_ASSERT(uniformLocationAlphaAndShadow!=-1);

            glUseProgram(program);
            resetUniforms();
            glUseProgram(0);
        }
        return program!=0;
    }
    void resetUniforms() {
        resetUniformOrtho(800,450);
        glUniform1i(uniformLocationSampler, 0);
        setUniformOffsetAndScale();
        setUniformAlphaAndShadow();
    }
    inline void setUniformOffsetAndScale(const SdfVec2& offset=SdfVec2(0,0),const SdfVec2& scale=SdfVec2(1,1)) {
        glUniform4f(uniformLocationOffsetAndScale,offset.x,offset.y,scale.x,scale.y);
    }
    // Shadow must be < 1 only in the "shadow pass"
    inline void setUniformAlphaAndShadow(const float alpha=1.f,const float shadow=1.f) {
        glUniform2f(uniformLocationAlphaAndShadow,alpha,shadow);
    }
    void resetUniformOrtho(float displaySizeWidth=-1,float displaySizeHeight=-1) {
        if (displaySizeWidth<=0) displaySizeWidth=gSdfDisplaySize.x;
        if (displaySizeHeight<=0) displaySizeHeight=gSdfDisplaySize.y;
        const float ortho[4][4] = {
            { 2.0f/displaySizeWidth,   0.0f,           0.0f, 0.0f },
            { 0.0f,         2.0f/-displaySizeHeight,   0.0f, 0.0f },
            { 0.0f,         0.0f,          -1.0f, 0.0f },
            {-1.0f,         1.0f,           0.0f, 1.0f },
        };
        glUniformMatrix4fv(uniformLocationOrthoMatrix, 1, GL_FALSE, &ortho[0][0]);
    }
    static const char** GetVertexShaderCodeForBothFonts() {
        static const char* gVertexShaderSource[] = {
#           ifdef SDFIMPL_SHADER_GL3
#           if (defined(SDFIMPL_SHADER_GLES) || defined(__EMSCRIPTEN__))
            "#version 300 es\n"
#           else  //SDFIMPL_SHADER_GLES
            "#version 330\n"
#           endif //SDFIMPL_SHADER_GLES
            "precision highp float;\n"
            "uniform mat4 ortho;\n"
            "uniform vec4 offsetAndScale;\n"
            "layout (location = 0 ) in vec2 Position;\n"
            "layout (location = 1 ) in vec2 UV;\n"
            "layout (location = 2 ) in vec4 Colour;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Colour;\n"
#           else //!SDFIMPL_SHADER_GL3
#           if (defined(SDFIMPL_SHADER_GLES) || defined(__EMSCRIPTEN__))
            "#version 100\n"
            "precision highp float;\n"
#           endif //SDFIMPL_SHADER_GLES
            "uniform mat4 ortho;\n"
            "uniform vec4 offsetAndScale;\n"
            "attribute vec2 Position;\n"
            "attribute vec2 UV;\n"
            "attribute vec4 Colour;\n"
            "varying vec2 Frag_UV;\n"
            "varying vec4 Frag_Colour;\n"
#           endif //!SDFIMPL_SHADER_GL3
            "void main()\n"
            "{\n"
            " Frag_UV = UV;\n"
            " Frag_Colour = Colour;\n"
            "\n"
            " gl_Position = ortho*vec4(offsetAndScale.x+Position.x*offsetAndScale.z,offsetAndScale.y+Position.y*offsetAndScale.w,0,1);\n"
            "}\n"
        };
        return &gVertexShaderSource[0];
    }
    static const char** GetFragmentShaderCodeForRegularFont() {
        static const char* gFragmentShaderSource[] = {
#       ifdef SDFIMPL_SHADER_GL3
#       if (defined(SDFIMPL_SHADER_GLES) || defined(__EMSCRIPTEN__))
            "#version 300 es\n"
#       else //SDFIMPL_SHADER_GLES
            "#version 330\n"
#       endif //SDFIMPL_SHADER_GLES
            "precision mediump float;\n"
            "uniform lowp sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Colour;\n"
            "out vec4 FragColor;\n"
#       else //!SDFIMPL_SHADER_GL3
#       if (defined(SDFIMPL_SHADER_GLES) || defined(__EMSCRIPTEN__))
            "#version 100\n"
            "#extension GL_OES_standard_derivatives : enable\n" // fwidth
            "precision mediump float;\n"
#       endif //SDFIMPL_SHADER_GLES
            "uniform sampler2D Texture;\n"
            "varying vec2 Frag_UV;\n"
            "varying vec4 Frag_Colour;\n"
#       endif //SDFIMPL_SHADER_GL3
            "uniform vec2 alphaAndShadow;\n"
#       ifdef SDFIMP_MSDF_MODE    // never tested
            "float median(float r, float g, float b) {\n"
            "    return max(min(r, g), min(max(r, g), b));\n"
            "}\n"
#       endif //SDFIMP_MSDF_MODE
            "void main(void) {\n"
#       ifndef SDFIMP_MSDF_MODE
            "float dist = texture2D(Texture, Frag_UV.st).a; // retrieve distance from texture\n"
#       else //SDFIMP_MSDF_MODE   // never tested
            "vec3 dist3 = texture2D(Texture, Frag_UV.st).rgb; // retrieve distance from texture\n"
            "float dist = median(dist3.r, dist3.g, dist3.b);// - 0.5;\n"
#       endif //SDFIMP_MSDF_MODE
#       ifdef SDFIMP_SLOWER_SHADER
			"float width = 0.7 * length(float2(dfdx(dist), dfdy(dist)));"
#		else //SDFIMP_SLOWER_SHADER
            "float width = fwidth(dist);"
#		endif //SDFIMP_SLOWER_SHADER
            "\n"
            "float alphaThreshold = 0.5;\n"
            "\n"
            "vec3 fragcolor = Frag_Colour.rgb;\n"
            "float alpha = smoothstep(alphaThreshold - width, alphaThreshold + width, dist);\n"
            "\n"
#       ifdef SDFIMPL_SHADER_GL3
            "FragColor = vec4(fragcolor,alpha*Frag_Colour.a);\n"
#       else //SDFIMPL_SHADER_GL3
            "gl_FragColor = vec4(fragcolor*alphaAndShadow.y,alpha*alphaAndShadow.x*Frag_Colour.a);\n"
#       endif //SDFIMPL_SHADER_GL3
            "}\n"
        };
        return &gFragmentShaderSource[0];
    }
    static const char** GetFragmentShaderCodeForOutlineFont() {
        static const char* gFragmentShaderSource[] = {
#       ifdef SDFIMPL_SHADER_GL3
#       if (defined(SDFIMPL_SHADER_GLES) || defined(__EMSCRIPTEN__))
            "#version 300 es\n"
#       else //SDFIMPL_SHADER_GLES
            "#version 330\n"
#       endif //SDFIMPL_SHADER_GLES
            "precision mediump float;\n"
            "uniform lowp sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Colour;\n"
            "out vec4 FragColor;\n"
#       else //!SDFIMPL_SHADER_GL3
#       if (defined(SDFIMPL_SHADER_GLES) || defined(__EMSCRIPTEN__))
            "#version 100\n"
            "#extension GL_OES_standard_derivatives : enable\n" // fwidth
            "precision mediump float;\n"
#       endif //SDFIMPL_SHADER_GLES
            "uniform sampler2D Texture;\n"
            "varying vec2 Frag_UV;\n"
            "varying vec4 Frag_Colour;\n"
#       endif //SDFIMPL_SHADER_GL3
            "uniform vec2 alphaAndShadow;\n"
#       ifdef SDFIMP_MSDF_MODE    // never tested
            "float median(float r, float g, float b) {\n"
            "    return max(min(r, g), min(max(r, g), b));\n"
            "}\n"
#       endif //SDFIMP_MSDF_MODE
            "void main(void) {\n"
#       ifndef SDFIMP_MSDF_MODE
            "float dist = texture2D(Texture, Frag_UV.st).a; // retrieve distance from texture\n"
#       else //SDFIMP_MSDF_MODE   // never tested
            "vec3 dist3 = texture2D(Texture, Frag_UV.st).rgb; // retrieve distance from texture\n"
            "float dist = median(dist3.r, dist3.g, dist3.b);// - 0.5;\n"
#       endif //SDFIMP_MSDF_MODE
#       ifdef SDFIMP_SLOWER_SHADER
			"float width = 0.7 * length(float2(dfdx(dist), dfdy(dist)));"
#		else //SDFIMP_SLOWER_SHADER
            "float width = fwidth(dist);"
#		endif //SDFIMP_SLOWER_SHADER
            "\n"
            "float alphaThreshold = 0.4;\n"
            "float outlineDarkeningFactor = 0.3;\n"
            "float outlineThreshold = 0.5;" // 0.5f
            "float inside = smoothstep(outlineThreshold - width, outlineThreshold + width, dist) ;\n"
            "//float glow = 1.0-inside;//smoothstep (0.0 , 20.0 , dist ) ;\n"
            "float glow = smoothstep (0.0 , 20.0 , dist ) ; // I don't understand this...\n"
            "vec3 insidecolor = Frag_Colour.rgb;\n"
            "vec3 outlinecolor = Frag_Colour.rgb*outlineDarkeningFactor;\n"
            "vec3 fragcolor = mix ( glow * outlinecolor , insidecolor , inside ) ;\n"
            "float alpha = smoothstep(alphaThreshold - width, alphaThreshold + width, dist);\n"
            "\n"
#       ifdef SDFIMPL_SHADER_GL3
            "FragColor = vec4(fragcolor*alphaAndShadow.y,alpha*alphaAndShadow.x*Frag_Colour.a);\n"
#       else //SDFIMPL_SHADER_GL3
            "gl_FragColor = vec4(fragcolor*alphaAndShadow.y,alpha*alphaAndShadow.x*Frag_Colour.a);\n"
#       endif //SDFIMPL_SHADER_GL3
            "}\n"
        };
        return &gFragmentShaderSource[0];
    }

    static GLuint CreateShader(GLenum type,const GLchar** shaderSource) {
        GLuint shader( glCreateShader( type ) );
        glShaderSource( shader, 1, shaderSource, NULL );
        glCompileShader( shader );

        // check
        GLint bShaderCompiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &bShaderCompiled);

        if (!bShaderCompiled)        {
            int i32InfoLogLength, i32CharsWritten;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

            SdfVector<char> pszInfoLog; pszInfoLog.resize(i32InfoLogLength+2);pszInfoLog[0]='\0';
            glGetShaderInfoLog(shader, i32InfoLogLength, &i32CharsWritten, &pszInfoLog[0]);
            if (type == GL_VERTEX_SHADER) printf("********%s %s\n", "GL_VERTEX_SHADER", &pszInfoLog[0]);
            else if (type == GL_FRAGMENT_SHADER) printf("********%s %s\n", "GL_FRAGMENT_SHADER", &pszInfoLog[0]);
            else printf("******** %s\n", &pszInfoLog[0]);

            fflush(stdout);
            glDeleteShader(shader);shader=0;
        }

        return shader;
    }
    static GLuint CompileShaderProgramAndSetCorrectAttributeLocations(bool isOutlineShaderProgram) {
        GLuint vs = CreateShader(GL_VERTEX_SHADER,GetVertexShaderCodeForBothFonts());
        if (!vs) return 0;
        GLuint fs = CreateShader(GL_FRAGMENT_SHADER,isOutlineShaderProgram ? GetFragmentShaderCodeForOutlineFont() : GetFragmentShaderCodeForRegularFont());
        if (!fs) return 0;

        GLuint program( glCreateProgram() );
        glAttachShader( program, vs );
        glAttachShader( program, fs );

        // Bind attribute locations:
        glBindAttribLocation(program,0,"Position");
        glBindAttribLocation(program,1,"UV");
        glBindAttribLocation(program,2,"Colour");

        // Link
        glLinkProgram( program );

        // check
        GLint bLinked;
        glGetProgramiv(program, GL_LINK_STATUS, &bLinked);
        if (!bLinked)        {
            int i32InfoLogLength, i32CharsWritten;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &i32InfoLogLength);

            SdfVector<char> pszInfoLog; pszInfoLog.resize(i32InfoLogLength+2);pszInfoLog[0]='\0';
            glGetProgramInfoLog(program, i32InfoLogLength, &i32CharsWritten, &pszInfoLog[0]);
            printf("********ShaderLinkerLog:\n%s\n", &pszInfoLog[0]);
            fflush(stdout);

            glDetachShader(program,vs);glDeleteShader(vs);vs=0;
            glDetachShader(program,fs);glDeleteShader(fs);fs=0;

            glDeleteProgram(program);program=0;
        }
        else {
            glDeleteShader(vs);vs=0;
            glDeleteShader(fs);fs=0;
        }

        return program;
    }

};
SdfShaderProgram gSdfShaderPrograms[2]; // 0-Normal and 1-Outline font


struct SdfCharset {
    float LineHeight;
    float Base;
    float Pages;
    float FontSize;
    float StretchH;
    int GlyphsCount;
    int KerningsCount;
    GLuint fntTexture;
    bool fntTextureOwned;

    typedef SdfCharDescriptor CharDescriptor;
    typedef SdfMap<unsigned int,CharDescriptor> SdfHashMapCharDescriptor; // map <char,int>.                     We can change the int to comsume less memory if needed
    SdfHashMapCharDescriptor Chars;

    typedef SdfMap<SdfUIntPair,float> SdfHashMapKerning;
    SdfHashMapKerning Kernings;

    SdfCharset() {clear();}
    ~SdfCharset() {
        if (fntTextureOwned && fntTexture) {
            glDeleteTextures(1,&fntTexture);
            fntTexture=0;
            fntTextureOwned=false;
        }
    }
    void clear() {
        LineHeight=Base=0.f;Pages=GlyphsCount=KerningsCount=0;
        Chars.clear();Kernings.clear();
        FontSize = 0.f;
        StretchH = 100.f;
        fntTexture = 0;
        fntTextureOwned = false;
    }

    bool  loadFromMemory(const void* data, size_t data_size,GLuint _fntTexture,const SdfCharsetProperties& properties,bool charsetWillDeleteFntTexture=false)    {
        const bool isTextFontFile = (data_size>=5 && strncmp((const char*) data,"info ",5)==0);// text .fnt files start with "info " AFAIK
        if (!isTextFontFile) {
            fprintf(stderr,"SdfCharset::loadFromMemory(): Error: SDF Font is not a txt based AngelBitmap .fnt file.\n");
            return false;
        }
        float ScaleW=0.f,ScaleH=0.f;
        fntTexture = _fntTexture;
        fntTextureOwned = charsetWillDeleteFntTexture;

        const char* pdata = (const char*) data;

        // From now on we must use "pdata" and "data_size" only

        // DONE: support some (optional) form of compression of these lines:
        //"char id=34      x=97   y=386  width=10   height=9    xoffset=-1   yoffset=1    xadvance=8    page=0    chnl=0 \n"
        //"kerning first=121 second=46 amount=-1\n"
        // into:
        //"ch id=34 97 386 10 9 -1 1 8 0 0\n"
        //"ke 121 46 -1\n"
        // to reduce size of embedded font text

        // Here we must fill all the fields of "f" based on "pdata" and "data_size":
        char tmp[1024];tmp[0]='\0';
        int tmpi=0;size_t gsize=0,ksize=0;
        CharDescriptor g;
        const char* buf_end = pdata + data_size;
        for (const char* line_start = pdata; line_start < buf_end; ) {
            const char* line_end = line_start;
            while (line_end < buf_end && *line_end != '\n' && *line_end != '\r') line_end++;

            if (strncmp(line_start,"ch",2)==0 && GlyphsCount>0)  {
                //char id=32   x=236   y=116   width=3     height=1     xoffset=-1    yoffset=15    xadvance=4     page=0  chnl=15
                int a[7] = {0,0,0,0,0,0,0};
                float b[3] = {0,0,0};
                int res = 0;
                if (strncmp(&line_start[2],"ar ",2)==0) res = sscanf(&line_start[4], " id=%i  x=%i y=%i width=%i height=%i xoffset=%f yoffset=%f xadvance=%f page=%i chnl=%i",
                        &a[0],&a[1],&a[2],&a[3],&a[4],&b[0],&b[1],&b[2],&a[5],&a[6]);
                else res = sscanf(&line_start[2], " id=%i  %i %i %i %i %f %f %f %i %i",
                        &a[0],&a[1],&a[2],&a[3],&a[4],&b[0],&b[1],&b[2],&a[5],&a[6]);
                if (res)
                {
                    //SDF_ASSERT(res==10);
                    SDF_ASSERT(ScaleW!=0 && ScaleH!=0);
                    g.Id = (unsigned int) a[0];
                    g.X = (float) a[1]/ScaleW;
                    g.Y = (float) a[2]/ScaleH;
                    g.Width = (float) a[3]/ScaleW;
                    g.Height = (float) a[4]/ScaleH;
                    SDF_ASSERT(FontSize!=0);
                    g.XOffset = b[0]/FontSize;//(signed short) ROUNDCAST(b[0]);
                    g.YOffset = b[1]/FontSize;//(signed short) ROUNDCAST(b[1]);
                    g.XOffset2 = (b[0]+(float)a[3])/FontSize;//(signed short) ROUNDCAST(b[0]);
                    g.YOffset2 = (b[1]+(float)a[4])/FontSize;//(signed short) ROUNDCAST(b[1]);
                    if (properties.flipYOffset) g.YOffset = -g.YOffset;
                    g.XAdvance = b[2]/FontSize;//(signed short) ROUNDCAST(b[2]);
                    g.Page = (unsigned char) a[5];

                    //Chars.put((unsigned int) a[0],g);
                    Chars[(unsigned int) a[0]] = g;

                    gsize++;
                }
                else fprintf(stderr,"Warning in SdfCharset::LoadTextFntFileFromMemory(\"glyph \"): skipped line [%.50s] (parsing error).\n",line_start);
            }
            else if (strncmp(line_start,"ke",2)==0 && KerningsCount>0)  {
                int a[3] = {0,0,0};
                int res = 0;
                if (strncmp(&line_start[2],"rning ",5)==0) res = sscanf(&line_start[7]," first=%i  second=%i amount=%i",&a[0],&a[1],&a[2]);
                else res = sscanf(&line_start[2]," %i  %i %i",&a[0],&a[1],&a[2]);
                if (res)
                {
                    //SDF_ASSERT(res==3);
                    SDF_ASSERT(FontSize!=0);
                    //Kernings.put(ImPair<unsigned int,unsigned int>(a[0],a[1]),(float)a[2]/FontSize);
                    Kernings[SdfUIntPair(a[0],a[1])] = (float)a[2]/FontSize;

                    ksize++;
                }
                else fprintf(stderr,"Warning in SdfCharset::LoadTextFntFileFromMemory(\"glyph \"): skipped line [%.50s] (parsing error).\n",line_start);

            }
            else if (strncmp(line_start,"info ",5)==0)   {
                tmp[0]='\0';
                char tmp2[1024];tmp2[0]='\0';
                int a[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
                const char* pline = NULL;

                //We must skip the font name first (we store it inside tmp)
                static const int quote = (int) '"';
                const char* q1=NULL;const char* q2=NULL;
                q1 = strchr((const char*) &line_start[5], quote );  // This method is better than sscanf, because the string can contain spaces
                if (q1) {
                    const char* q11 = ++q1;
                    q2 = strchr(q11, quote);
                    if (q2)  {
                        const size_t sz = q2-q11;
                        strncpy(tmp,q11,sz);
                        tmp[sz]='\0';
                        pline = ++q2;
                    }
                }

                if (pline && (sscanf(pline, " size=%i  bold=%i italic=%i charset=%s unicode=%i stretchH=%i smooth=%i aa=%i padding=%i,%i,%i,%i spacing=%i,%i outline=%i",
                                    &a[0],&a[1],&a[2],tmp2,&a[3],&a[4],&a[5],&a[6],&a[7],&a[8],&a[9],&a[10],&a[11],&a[12],&a[13])))
                {
                    // This is necessary because often tmp2="", with quotes included:
                    size_t tmp2Size;
                    while ((tmp2Size=strlen(tmp2))>0 && tmp2[tmp2Size-1]=='"') tmp2[tmp2Size-1]='\0';
                    while ((tmp2Size=strlen(tmp2))>0 && tmp2[0]=='"') {static char tmp22[1024];strcpy(tmp22,&tmp2[1]);strcpy(tmp2,tmp22);}

                    FontSize = (float) a[0];
                    StretchH = (float) a[4];
                    //fprintf(stderr,"FontSize=%1.0f StretchH=%1.0f charset=\"%s\"\n",FontSize,StretchH,tmp2);

                }
                else fprintf(stderr,"Warning in SdfCharset::LoadTextFntFileFromMemory(\"info \"): skipped line [%.50s] (parsing error).\n",line_start);
            }
            else if (strncmp(line_start,"common ",7)==0)  {
                int a[10] = {0,0,0,0,0,0,0,0,0,0};
                // common lineHeight=16 base=13 scaleW=256 scaleH=256 pages=1 packed=0 alphaChnl=0 redChnl=0 greenChnl=0 blueChnl=0
                if (sscanf(&line_start[7], " lineHeight=%i  base=%i scaleW=%i scaleH=%i pages=%i packed=%i alphaChnl=%i redChnl=%i greenChnl=%i blueChnl=%i",
                           &a[0],&a[1],&a[2],&a[3],&a[4],&a[5],&a[6],&a[7],&a[8],&a[9]))
                {
                    SDF_ASSERT(FontSize!=0);
                    LineHeight = (float) a[0]/FontSize;
                    Base = (float) a[1]/FontSize;
                    ScaleW = (float) a[2];
                    ScaleH = (float) a[3];
                    Pages = (unsigned short) a[4];

                }
                else fprintf(stderr,"Warning in SdfCharset::LoadTextFntFileFromMemory(\"common \"): skipped line [%.50s] (parsing error).\n",line_start);

            }
            else if (strncmp(line_start,"page id=0 ",10)==0)  {
 #ifdef NEVER
                // well, we just support one page, but maybe this is what "filenames" refer to
                // we just fill the first filename we found in the line for now (I must see a file with more than one page to parse it correctly...):
                if (f.filenames.size()==0)  {
                    tmp[0]='\0';

                    static const int quote = (int) '"';
                    const char* q1=NULL;const char* q2=NULL;
                    q1 = strchr((const char*) &line_start[10], quote );
                    if (q1) {
                        const char* q11 = ++q1;
                        q2 = strchr(q11, quote);
                        if (q2)  {
                            const size_t sz = q2-q11;
                            strncpy(tmp,q11,sz);
                            tmp[sz]='\0';
                        }
                    }

                    if (strlen(tmp)>0)    {
                        size_t tmpSize;
                        while ( (tmpSize=strlen(tmp))>0 && tmp[tmpSize-1]=='\"') tmp[tmpSize-1]='\0';
                        gsize = f.filenames.size();
                        f.filenames.resize(gsize+1);
                        char* c = &f.filenames[gsize][0];
                        c[0]='\0';
                        if (strlen(tmp)<=MAX_FILENAME_SIZE) {
                            strcpy(c,tmp);
#                       ifdef DEBUG_PARSED_DATA
                            fprintf(stderr,"Filename = %s\n",c);
#                       endif //DEBUG_PARSED_DATA
                        }
                    }
                    else fprintf(stderr,"Warning in SdfCharset::LoadTextFntFileFromMemory(\"page \"): skipped line [%.50s] (parsing error).\n",line_start);
                }
#endif //NEVER
            }
            else if (strncmp(line_start,"chars count=",12)==0)  {
                if (sscanf(line_start, "chars count=%i", &tmpi))    {
                    GlyphsCount = tmpi;
                    //f.glyphs.reserve(GlyphsCount);
                }
                else fprintf(stderr,"Warning in SdfCharset::LoadTextFntFileFromMemory(): skipped line [%.50s] (parsing error).\n",line_start);
            }
            else if (strncmp(line_start,"kernings count=",15)==0)  {
                if (sscanf(line_start, "kernings count=%i", &tmpi))    {
                    KerningsCount = tmpi;
                }
                else fprintf(stderr,"Warning in SdfCharset::LoadTextFntFileFromMemory(): skipped line [%.50s] (parsing error).\n",line_start);
            }
            line_start = line_end+1;
        }

        //SDF_ASSERT(GlyphsCount==gsize);
        //SDF_ASSERT(KerningsCount==ksize);

        // Add extra codepoints:
        /*
        CharDescriptor sp;
        if (Chars.get((unsigned int)' ',sp)) {
            CharDescriptor tab;
            if (!Chars.get((unsigned int)'\t',tab)) {
                tab = sp;tab.Id='\t';tab.XAdvance=sp.XAdvance*4.f;
                Chars.put((unsigned int)'\t',tab);
            }
            sp.Width=0.f;Chars.put((unsigned int)' ',sp);
        }
        CharDescriptor lf;
        if (!Chars.get((unsigned int)'\n',lf)) {
            lf = sp;lf.Id='\n';lf.XAdvance=lf.Width=lf.XOffset=0.f;
        }
        if (!Chars.get((unsigned int)'\r',lf)) {
            lf = sp;lf.Id='\r';lf.XAdvance=lf.Width=lf.XOffset=0.f;
        }
        */

        SdfHashMapCharDescriptor::const_iterator it;
        CharDescriptor sp;
        if ( (it=Chars.find((unsigned int)' '))!=Chars.end() )  {
            sp = it->second;
            if ( (it=Chars.find((unsigned int)'\t'))==Chars.end() ) {
                CharDescriptor tab = it->second;
                tab = sp;tab.Id='\t';tab.XAdvance=sp.XAdvance*4.f;
                Chars[(unsigned int)'\t'] = tab;
            }
            sp.Width=0.f;Chars[(unsigned int)' '] = sp;
        }
        CharDescriptor lf;
        if ( (it=Chars.find((unsigned int)'\n'))==Chars.end() ) {
            lf = sp;lf.Id='\n';lf.XAdvance=lf.Width=lf.XOffset=0.f;
            Chars[(unsigned int)'\n']=lf;
        }
        if ( (it=Chars.find((unsigned int)'\r'))==Chars.end() ) {
            {lf = sp;lf.Id='\r';lf.XAdvance=lf.Width=lf.XOffset=0.f;}
            Chars[(unsigned int)'\r']=lf;
        }
        else lf = it->second;

        return true;

    }

    static bool GetFileContent(const char *filePath,SdfVector<char> &contentOut,bool clearContentOutBeforeUsage,const char *modes="rb",bool appendTrailingZeroIfModesIsNotBinary=true)   {
        SdfVector<char>& f_data = contentOut;
        if (clearContentOutBeforeUsage) f_data.clear();
        //----------------------------------------------------
        if (!filePath) return false;
        const bool appendTrailingZero = appendTrailingZeroIfModesIsNotBinary && modes && strlen(modes)>0 && modes[strlen(modes)-1]!='b';
        FILE* f;
        if ((f = fopen(filePath, modes)) == NULL) return false;
        if (fseek(f, 0, SEEK_END))  {
            fclose(f);
            return false;
        }
        const long f_size_signed = ftell(f);
        if (f_size_signed == -1)    {
            fclose(f);
            return false;
        }
        size_t f_size = (size_t)f_size_signed;
        if (fseek(f, 0, SEEK_SET))  {
            fclose(f);
            return false;
        }
        f_data.resize(f_size+(appendTrailingZero?1:0));
        const size_t f_size_read = f_size>0 ? fread(&f_data[0], 1, f_size, f) : 0;
        fclose(f);
        if (f_size_read == 0 || f_size_read!=f_size)    return false;
        if (appendTrailingZero) f_data[f_size] = '\0';
        //----------------------------------------------------
        return true;
    }

#   ifndef SDF_NO_EMBEDDED_FONT
    static bool GetEmbeddedFontData(SdfVector<char>& fntFileOut,SdfVector<unsigned char>& rawRGBAOut,int& wOut,int& hOut);
#   endif //SDF_NO_EMBEDDED_FONT
};

void SdfTextChunk::addText(const char* startText, bool _italic, const SdfTextColor* pSdfTextColor, const SdfVec2* textScaling, const char* endText,const SDFHAlignment *phalignOverride, bool fakeBold) {
    SDF_ASSERT(startText);
    SDF_ASSERT(charset);

    const int textBitsSize = textBits.size();
    textBits.push_back(TextBit());
    TextBit& TB = textBits[textBitsSize];

    TB.scaling = textScaling ? *textScaling : SdfVec2(1,1);if (fakeBold) {TB.scaling.x*=1.2f;/*TB.scaling.y*=1.1f;*/}
    TB.italic = _italic;
    TB.sdfTextColor = pSdfTextColor ? *pSdfTextColor : gSdfTextDefaultColor;// change if fakeBold ?
    TB.hAlignment = phalignOverride ? (int)(*phalignOverride) : -1;

    SdfCharDescriptor cd,cdQuestionMark; int lastCdId=0;
    const bool useKernings = charset->KerningsCount>0;float kerning = 0;
    uint32_t codePoint = 0, state = 0;
    if (!endText) endText = startText+strlen(startText);
    TB.charDescriptors.reserve(endText-startText);
    if (useKernings) TB.kernings.reserve(endText-startText);
    const unsigned char* endTextUC = (const unsigned char*) endText;
    SdfCharset::SdfHashMapCharDescriptor::const_iterator it;bool hasQuestionMark;
    if ( (it = charset->Chars.find((unsigned int)'?') )!= charset->Chars.end()) {
        cdQuestionMark = it->second;hasQuestionMark = true;
    }
    SdfCharset::SdfHashMapKerning::const_iterator kit;
    for(const unsigned char* p = (const unsigned char*) startText; p!=endTextUC; ++p)
    {
        if (UTF8Helper::decode(&state, &codePoint, *p)!=UTF8Helper::UTF8_ACCEPT) continue;
        // Here we could count valid chars if we need it...
        ++numValidGlyphs;

        //if (!charset->Chars.get(codePoint,cd) && !charset->Chars.get('?',cd)) continue;
        it = charset->Chars.find(codePoint);
        if (it!=charset->Chars.end()) cd = it->second;
        else {
            //printf("%u codepoint not found\n",codePoint); // 10 is newline \n
            if (hasQuestionMark) {cd = cdQuestionMark;}
            else continue;
        }
        TB.charDescriptors.push_back(cd);

        if (useKernings) {
            if (lastCdId!=0)    {
                if ( (kit = charset->Kernings.find(SdfUIntPair(lastCdId,cd.Id)))!=charset->Kernings.end()) kerning = kit->second;
                else kerning = 0.f;
                TB.kernings.push_back(kerning);
                //fprintf(stderr,"%d) Kerning: %c->%c = %1.4f\n",TB.kernings.size()-1,(char)lastCdId,(char)cd.Id,kerning);
            }
            else TB.kernings.push_back(.0f);            
        }
        lastCdId = cd.Id;
    }

    dirty = true;
}
bool SdfTextChunk::endText(SdfVec2 screenSize) {
    if (!dirty)  return false;
    dirty = false;
    SDF_ASSERT(buffer);
    buffer->clear();
    if (textBits.size()==0) return true;

    if (screenSize.x<=0 || screenSize.y<=0) screenSize = gSdfDisplaySize;
    const SdfVec2 cursorXlimits((props.boundsCenter.x-props.boundsHalfSize.x)*screenSize.x,(props.boundsCenter.x+props.boundsHalfSize.x)*screenSize.x);
    const SdfVec2 cursorYlimits((props.boundsCenter.y-props.boundsHalfSize.y)*screenSize.y,(props.boundsCenter.y+props.boundsHalfSize.y)*screenSize.y);
    //const float cursorXextent = cursorXlimits.y - cursorXlimits.x;
    //const float cursorYextent = cursorYlimits.y - cursorYlimits.x;
    //fprintf(stderr,"cursorXlimits(%1.0f,%1.0f) cursorYlimits(%1.0f,%1.0f) displaySize(%1.0f,%1.0f)\n",cursorXlimits.x,cursorXlimits.y,cursorYlimits.x,cursorYlimits.y,Sdf::GetIO().DisplaySize.x,Sdf::GetIO().DisplaySize.y);
    SDF_ASSERT(props.maxNumTextLines>0);
    const float virtualLineHeight = props.lineHeightOverride>0.f?props.lineHeightOverride:charset->LineHeight;
    const float numPixelsPerTextLine = screenSize.y/*cursorYextent*//(float)props.maxNumTextLines;
    const float globalScale = numPixelsPerTextLine/virtualLineHeight;   // globalScale translates quantities into pixels (it's the FontSize (or the FontHeight) in pixels)
    const float BaseBase = globalScale*charset->Base;
    const float LineHeightBase = globalScale*virtualLineHeight;         // It's the Line Height in pixels
    SdfVec2 cursor = SdfVec2(cursorXlimits.x,cursorYlimits.x);


    SdfVector<SdfVertexBuffer::VertexDeclaration>& vertexBuffer =  buffer->verts;
    int curVertexBufferSize = vertexBuffer.size();
    const int startVertexBufferSize = curVertexBufferSize;
    vertexBuffer.reserve(curVertexBufferSize+numValidGlyphs*6);

    const bool useKernings = charset->KerningsCount>0;bool skipKerning = false;
    SdfVertexBuffer::VertexDeclaration *pVert = NULL;
    float XAdvance=0.f,XOffset0=0.f,YOffset0=0.f,XOffset1=0.f,YOffset1=0.f;
    SdfVec2 totalSizeInPixels(0.f,0.f);
    bool mustEndOneLine = false;

    SdfVec2 localScaleXminmax(0.f,0.f),localScaleYminmax(0.f,0.f);


    const SdfCharDescriptor* lastCd = NULL;
    typedef struct _LineData{
        SdfVec2 sizeInPixels;
        float Base;
        float LineHeight;
        int numGlyphs;
        _LineData(float BaseBase,float LineHeightBase) : sizeInPixels(0.f,0.f),Base(BaseBase),LineHeight(LineHeightBase),numGlyphs(0) {lastTbScalingY=maxTbScalingY=1.f;pVertStartLine=pVertLastLineSplitter=NULL;cursorXAtLastLineSplitter=0.f;lastLineSplitterIsSpace=false;}
        mutable float lastTbScalingY;
        mutable float maxTbScalingY;
        mutable SdfVertexBuffer::VertexDeclaration *pVertStartLine;
        mutable SdfVertexBuffer::VertexDeclaration *pVertLastLineSplitter;
        mutable float cursorXAtLastLineSplitter;
        mutable bool lastLineSplitterIsSpace;
    } LineData;

    LineData lineData(BaseBase,LineHeightBase);
    SDFHAlignment lastTBalignment = props.halign,halign = props.halign;bool mustStartNewAlignment = false;
    for (int i=0,isz=textBits.size();i<isz;i++) {
	const TextBit& TB = textBits[i];
        halign =  (TB.hAlignment==-1) ? props.halign : (SDFHAlignment)TB.hAlignment;
        mustStartNewAlignment = (lastTBalignment!=halign);
        if (mustStartNewAlignment)  {
            //------------------------------------------
            if (totalSizeInPixels.x<lineData.sizeInPixels.x) totalSizeInPixels.x = lineData.sizeInPixels.x;
            //totalSizeInPixels.y+= lineData.LineHeight;
            // h alignment on lineData
            // Horizontal alignment here---------------------------------
            if (lineData.pVertStartLine && pVert) {
                if (lastTBalignment==SDF_JUSTIFY)  {
                    // horizontal alignment here---------------------------------
                    if (lineData.numGlyphs>1 &&  (/*!lastCd || lastCd->Id!='\n' ||*/ lineData.sizeInPixels.x>0.65f*(cursorXlimits.y - cursorXlimits.x)))  {
                        const float deltaX = (cursorXlimits.y - cursorXlimits.x - lineData.sizeInPixels.x)/(float)(lineData.numGlyphs-1);
                        int cnt = 0;float addend = deltaX;
                        for (SdfVertexBuffer::VertexDeclaration* vd = (lineData.pVertStartLine+6);vd!=pVert;++vd)   {
                            vd->posAndUV.x+=addend;
                            if (++cnt==6) {cnt=0;addend+=deltaX;}
                        }
                    }
                }
                else {
                    float offsetX=0.f;
                    if (lastTBalignment!=SDF_LEFT)      {
                        offsetX = cursorXlimits.y - cursorXlimits.x - lineData.sizeInPixels.x;
                        if (lastTBalignment==SDF_CENTER) offsetX*=0.5f;
                    }
                    if (offsetX!=0.f) {
                        for (SdfVertexBuffer::VertexDeclaration* vd = lineData.pVertStartLine;vd!=pVert;++vd)   {
                            vd->posAndUV.x+=offsetX;
                        }
                    }
                }
            }
            //------------------------------------------------------------
            //lineData = LineData(BaseBase,LineHeightBase);
            lineData.pVertStartLine=lineData.pVertLastLineSplitter=NULL;
            lineData.numGlyphs = 0;
            lineData.sizeInPixels=SdfVec2(0,0);
            cursor.x = cursorXlimits.x;
            //------------------------------------------------------------
            lastTBalignment=halign;
        }
        const SdfVec2 localScale = TB.scaling * globalScale //;
                * SdfVec2(TB.italic ? 0.925f : 1.f,1.f);  // This line makes italic a bit slimmer than normal text
        if (i==0) {
            localScaleXminmax.x=localScaleXminmax.y=localScale.x;
            localScaleYminmax.x=localScaleYminmax.y=localScale.y;
        }
        else {
            if      (localScaleXminmax.x>localScale.x) localScaleXminmax.x = localScale.x;
            else if (localScaleXminmax.y<localScale.x) localScaleXminmax.y = localScale.x;
            if      (localScaleYminmax.x>localScale.y) localScaleYminmax.x = localScale.y;
            else if (localScaleYminmax.y<localScale.y) localScaleYminmax.y = localScale.y;
        }
        //if (shadowOffsetInPixels.x<localScale.x) shadowOffsetInPixels.x = localScale.x;
        //if (shadowOffsetInPixels.y<localScale.y) shadowOffsetInPixels.y = localScale.y;
        if (lineData.lastTbScalingY!=TB.scaling.y)   {
            lineData.lastTbScalingY=TB.scaling.y;
            if (lineData.maxTbScalingY<TB.scaling.y)    {
                if (TB.scaling.y > virtualLineHeight) {
                    //const float oldBaseBase = lineData.Base;
                    //const float oldLineHeight = lineData.LineHeight;
                    lineData.Base = BaseBase*TB.scaling.y;
                    lineData.LineHeight = LineHeightBase*TB.scaling.y;
                    // scale all glyphs from lineData.pVertStartLine to pVert
                    if (lineData.pVertStartLine && pVert)    {
                        const float deltaY = (TB.scaling.y - lineData.maxTbScalingY) * globalScale;
                        for (SdfVertexBuffer::VertexDeclaration* vd = lineData.pVertStartLine;vd!=pVert;++vd)   {
                            vd->posAndUV.y+=deltaY;
                        }
                    }

                }
                lineData.maxTbScalingY=TB.scaling.y;
            }
        }
        //const float& Base = lineData.Base;
        const float& LineHeight = lineData.LineHeight;
        const float italicAddend = TB.italic ? localScale.y * 0.125f : 0.f;

        if (useKernings) {SDF_ASSERT(TB.kernings.size()==TB.charDescriptors.size());}
        for (int cdIndex=0,cdIndexSize = TB.charDescriptors.size();cdIndex<cdIndexSize;cdIndex++)   {
            const SdfCharDescriptor& cd =  TB.charDescriptors[cdIndex];
            const float kerning = (useKernings && !skipKerning) ? TB.kernings[cdIndex] : 0.f;
            skipKerning = false;
            //--------------------------------------------------------------------------------------------------------
            XOffset0=cd.XOffset*localScale.x;
            YOffset0=cd.YOffset*localScale.y;
            XOffset1=cd.XOffset2*localScale.x;
            YOffset1=cd.YOffset2*localScale.y;

            if (useKernings) cursor.x +=  kerning*localScale.x;
            XAdvance=cd.XAdvance*localScale.x;
            //--------------------------------------------------------------------------------------------------------
            mustEndOneLine = false;
            bool mustSplitLine = false;
            if (cd.Id=='\n') mustEndOneLine = true;
            else if (cd.Id=='\r') {
                lastCd = &cd;
                continue;
            }
            if (cursor.x+XAdvance>cursorXlimits.y) {
                // Word wrap: but we could just skip using "continue" here
                mustSplitLine = lineData.pVertLastLineSplitter!=NULL;
                mustEndOneLine = true;
            }
            if (mustEndOneLine) {
                const float oldCursorX = cursor.x;
                cursor.x=cursorXlimits.x;
                cursor.y+=LineHeight;
                skipKerning = true;
                SdfVertexBuffer::VertexDeclaration* pLastVertOfLine = pVert;
                LineData nextLineData(BaseBase,LineHeightBase); // reset it
                if (TB.scaling.y>virtualLineHeight) {
                    nextLineData.Base = BaseBase*TB.scaling.y;
                    nextLineData.LineHeight = LineHeightBase*TB.scaling.y;
                    nextLineData.lastTbScalingY = nextLineData.maxTbScalingY = TB.scaling.y;

                    /*// scale all glyphs from lineData.pVertStartLine to pVert
                    if (lineData.pVertStartLine && pVert)    {
                        const float deltaY = (TB.scaling.y - lineData.maxTbScalingY) * globalScale;
                        for (SdfVertexBuffer::VertexDeclaration* vd = lineData.pVertStartLine;vd!=pVert;++vd)   {
                            vd->posAndUV.y+=deltaY;
                        }
                    }*/
                }

                if (mustSplitLine) {
                    nextLineData.numGlyphs = 0;
                    // lineData
                    for (SdfVertexBuffer::VertexDeclaration* vd = lineData.pVertLastLineSplitter;vd!=pVert;vd=vd+6) {
                        --lineData.numGlyphs;++nextLineData.numGlyphs;
                    }
                    pLastVertOfLine = lineData.pVertLastLineSplitter;
                    cursor.x = cursorXlimits.x + oldCursorX - lineData.cursorXAtLastLineSplitter;
                    lineData.sizeInPixels.x = lineData.cursorXAtLastLineSplitter - cursorXlimits.x;
                    // nextLineData
                    float deltaX = lineData.cursorXAtLastLineSplitter-cursorXlimits.x;
                    const float deltaY = LineHeight;
                    float deltaSizeX = 0.f;
                    SdfVec2 yMinMax(lineData.pVertLastLineSplitter->posAndUV.y,lineData.pVertLastLineSplitter->posAndUV.y);
                    // How can I crop leading spaces here ?
                    nextLineData.pVertStartLine = pLastVertOfLine;
                    unsigned int cnt = 0;bool enableCutSpaces = true;
                    for (SdfVertexBuffer::VertexDeclaration* vd = lineData.pVertLastLineSplitter;vd!=pVert;++vd)   {
                        if (enableCutSpaces && cnt==0 /*&& pVert!=vd+6*/) {
                            SdfVertexBuffer::VertexDeclaration* vd2 = vd+1;
                            SdfVertexBuffer::VertexDeclaration* vd6 = vd+6;
                            //fprintf(stderr,"%1.6f -> %1.6f\n",vd2->posAndUV.z-vd->posAndUV.z,vd6->posAndUV.x-vd->posAndUV.x);
                            if (vd2->posAndUV.z==vd->posAndUV.z) {
                                if (pVert!=vd+6) {
                                    nextLineData.pVertStartLine+=6;--nextLineData.numGlyphs;
                                    float cursorXdelta = (vd6->posAndUV.x-vd->posAndUV.x);
                                    deltaSizeX+=cursorXdelta;
                                    deltaX+=cursorXdelta;
                                    cursor.x-=cursorXdelta;
                                }
                                else {
                                    nextLineData.pVertStartLine=NULL;nextLineData.numGlyphs=0;
                                    cursor.x=cursorXlimits.x;

                                    //fprintf(stderr,"Border case to handle\n");
                                    //vertexBuffer.resize(vertexBuffer.size()-6);
                                    //curVertexBufferSize = vertexBuffer.size();
                                    break;
                                }
                                // [Optional] Remove vertex from vertexarray:
                                /*for (SdfVertexBuffer::VertexDeclaration* p = vd;p!=pVert-6;++p) *p = *(p+6);
                                pVert = pVert-6;
                                vertexBuffer.resize(vertexBuffer.size()-6);
                                curVertexBufferSize = vertexBuffer.size();
                                vd=vd-1;cnt=0;continue;*/
                            }
                            else enableCutSpaces = false;
                        }
                        vd->posAndUV.x-=deltaX;
                        vd->posAndUV.y+=deltaY;
                        if (yMinMax.x>vd->posAndUV.y) yMinMax.x=vd->posAndUV.y;
                        else if (yMinMax.y<vd->posAndUV.y) yMinMax.y=vd->posAndUV.y;
                        if (++cnt==6) cnt=0;
                    }
                    /*
                    // This does not work and furthermore gives a Valgrind error
                    const float maxY = (yMinMax.y-yMinMax.x)/globalScale;
                    if (maxY > virtualLineHeight) {
                        nextLineData.maxTbScalingY = maxY;
                        nextLineData.lastTbScalingY = nextLineData.maxTbScalingY;
                        //nextLineData.Base = BaseBase*maxY;
                        //nextLineData.LineHeight = LineHeightBase*maxY;
                    }*/
                    nextLineData.sizeInPixels.x = cursor.x -cursorXlimits.x;

                }

                if (totalSizeInPixels.x<lineData.sizeInPixels.x) totalSizeInPixels.x = lineData.sizeInPixels.x;
                totalSizeInPixels.y+= lineData.LineHeight;
                // Horizontal alignment here---------------------------------
                if (lineData.pVertStartLine && pLastVertOfLine) {
                    if (halign==SDF_JUSTIFY)  {
                        // horizontal alignment here---------------------------------
                        if (lineData.numGlyphs>1 && (cd.Id!='\n' || lineData.sizeInPixels.x>0.65f*(cursorXlimits.y - cursorXlimits.x)))  {
                            const float deltaX = (cursorXlimits.y - cursorXlimits.x - lineData.sizeInPixels.x)/(float)(lineData.numGlyphs-1);
                            int cnt = 0;float addend = deltaX;
                            for (SdfVertexBuffer::VertexDeclaration* vd = (lineData.pVertStartLine+6);vd!=pLastVertOfLine;++vd)   {
                                vd->posAndUV.x+=addend;
                                if (++cnt==6) {cnt=0;addend+=deltaX;}
                            }
                        }
                    }
                    else {
                        float offsetX=0.f;
                        if (halign!=SDF_LEFT)      {
                            offsetX = cursorXlimits.y - cursorXlimits.x - lineData.sizeInPixels.x;
                            if (halign==SDF_CENTER) offsetX*=0.5f;
                        }
                        if (offsetX!=0.f) {
                            for (SdfVertexBuffer::VertexDeclaration* vd = lineData.pVertStartLine;vd!=pLastVertOfLine;++vd)   {
                                vd->posAndUV.x+=offsetX;
                            }
                        }
                    }
                }
                //------------------------------------------------------------
                lineData = nextLineData;
                if (cursor.y>=cursorYlimits.y) {
                    lastCd = &cd;
                    break;
                }
                if (cd.Id=='\n') {
                    lastCd = &cd;
                    continue;  //consume one char
                }
            }
            if (cursor.y+YOffset0<cursorYlimits.x) {
                cursor.x += XAdvance;
                lastCd = &cd;
                continue;
            }

            // Eat starting space if necessary
            if ((!lineData.pVertStartLine || cursor.x==cursorXlimits.x) && cd.Id==' ')   {
                lineData.pVertStartLine = NULL;
                //if (lineData.pVertStartLine) lineData.pVertStartLine = lineData.pVertStartLine+6;
                //lastCd = &cd;
                continue;
            }
            if (pVert && ((cd.Id==' ' && (!lineData.lastLineSplitterIsSpace || !lastCd || lastCd->Id!=' ')) || cd.Id=='\t')) {
                if (lineData.pVertLastLineSplitter!=pVert-1) {
                    lineData.pVertLastLineSplitter = pVert;
                    lineData.cursorXAtLastLineSplitter = cursor.x;
                    lineData.lastLineSplitterIsSpace = cd.Id==' ';
                }
            }

            ++lineData.numGlyphs;

/*
  --------------------------------------------          ^
  ^             ^                                       |
  |     yoffset |                                       |
  |             v                                       |
  |             -----------------                       |
  |             |               ^
  base          |               |                   lineHeight
  |             |               |
  |             |               | height                |
  | xoffset     |               |                       |
  v<----------->|  - - - - - -  |                       |
                |               v                       |
                <--------------->                       |
                      width                             |
  ---------------------------------------------         v

  <----------------------------------> xadvance
*/
                //if (cd.YOffset<0) cd.YOffset = 0;//-cd.YOffset; // This seems to improve alignment slightly in one of my fonts...
                // TODO: Allow scaling from extern and update spacing and LineHeight

                curVertexBufferSize = vertexBuffer.size();
                vertexBuffer.resize(curVertexBufferSize+6); // We use GL_TRIANGLES, as GL_QUADS is not supported on some OpenGL implementations
                pVert = &vertexBuffer[curVertexBufferSize];
                if (!lineData.pVertStartLine) lineData.pVertStartLine = pVert;            


                //upper left
                pVert->posAndUV.z = cd.X;
                pVert->posAndUV.w = cd.Y;
                pVert->posAndUV.x = cursor.x + XOffset0 + italicAddend;
                pVert->posAndUV.y = cursor.y + YOffset0;
                pVert->color = TB.sdfTextColor.colorTopLeft;
                //fprintf(stderr,"%1.2f,%1.2f,%1.2f,%1.2f\n",pVert->posAndUV.x,pVert->posAndUV.y,pVert->posAndUV.z,pVert->posAndUV.w);
                ++pVert;


                //upper right
                pVert->posAndUV.z = cd.X+cd.Width;
                pVert->posAndUV.w = cd.Y;
                pVert->posAndUV.x = cursor.x + XOffset1 + italicAddend;
                pVert->posAndUV.y = cursor.y + YOffset0;
                pVert->color = TB.sdfTextColor.colorTopRight;
                //fprintf(stderr,"%1.2f,%1.2f,%1.2f,%1.2f\n",pVert->posAndUV.x,pVert->posAndUV.y,pVert->posAndUV.z,pVert->posAndUV.w);
                ++pVert;

                //lower right
                pVert->posAndUV.z = cd.X+cd.Width;
                pVert->posAndUV.w = cd.Y+cd.Height;
                pVert->posAndUV.x = cursor.x + XOffset1;
                pVert->posAndUV.y = cursor.y + YOffset1;
                pVert->color = TB.sdfTextColor.colorBottomRight;
                //fprintf(stderr,"%1.2f,%1.2f,%1.2f,%1.2f\n",pVert->posAndUV.x,pVert->posAndUV.y,pVert->posAndUV.z,pVert->posAndUV.w);
                ++pVert;

                *pVert = *(pVert-1);++pVert;

                //lower left
                pVert->posAndUV.z = cd.X ;
                pVert->posAndUV.w = cd.Y+cd.Height;
                pVert->posAndUV.x = cursor.x + XOffset0;
                pVert->posAndUV.y = cursor.y + YOffset1;
                pVert->color = TB.sdfTextColor.colorBottomLeft;
                //fprintf(stderr,"%1.2f,%1.2f,%1.2f,%1.2f\n",pVert->posAndUV.x,pVert->posAndUV.y,pVert->posAndUV.z,pVert->posAndUV.w);
                ++pVert;

                *pVert = *(pVert-5);++pVert;

                if (lineData.sizeInPixels.x < cursor.x + XOffset1 - cursorXlimits.x) lineData.sizeInPixels.x = cursor.x + XOffset1 - cursorXlimits.x;
                if (lineData.sizeInPixels.y < cursor.y + YOffset1 - cursorXlimits.y) lineData.sizeInPixels.y = cursor.y + YOffset1 - cursorXlimits.y;


                cursor.x +=  XAdvance;

                if (cd.Id==',' || cd.Id=='.' || cd.Id==';' || cd.Id==':' ||
                cd.Id=='?' || cd.Id=='!' || cd.Id=='-') {
                    if (lineData.pVertLastLineSplitter!=pVert-1) {
                        lineData.pVertLastLineSplitter = pVert;
                        lineData.cursorXAtLastLineSplitter = cursor.x;
                        lineData.lastLineSplitterIsSpace = false;
                    }
                }
            lastCd = &cd;
            }

            //--------------------------------------------------------------------------------------------------------
        }

    if (!mustEndOneLine) {
        if (totalSizeInPixels.x<lineData.sizeInPixels.x) totalSizeInPixels.x = lineData.sizeInPixels.x;
        totalSizeInPixels.y+= lineData.LineHeight;
        // h alignment on lineData
        // Horizontal alignment here---------------------------------
        if (lineData.pVertStartLine && pVert) {
            if (halign==SDF_JUSTIFY)  {
                // horizontal alignment here---------------------------------
                if (lineData.numGlyphs>1 &&  (/*!lastCd || lastCd->Id!='\n' ||*/ lineData.sizeInPixels.x>0.65f*(cursorXlimits.y - cursorXlimits.x)))  {
                    const float deltaX = (cursorXlimits.y - cursorXlimits.x - lineData.sizeInPixels.x)/(float)(lineData.numGlyphs-1);
                    int cnt = 0;float addend = deltaX;
                    for (SdfVertexBuffer::VertexDeclaration* vd = (lineData.pVertStartLine+6);vd!=pVert;++vd)   {
                        vd->posAndUV.x+=addend;
                        if (++cnt==6) {cnt=0;addend+=deltaX;}
                    }
                }
            }
            else {
                float offsetX=0.f;
                if (halign!=SDF_LEFT)      {
                    offsetX = cursorXlimits.y - cursorXlimits.x - lineData.sizeInPixels.x;
                    if (halign==SDF_CENTER) offsetX*=0.5f;
                }
                if (offsetX!=0.f) {
                    for (SdfVertexBuffer::VertexDeclaration* vd = lineData.pVertStartLine;vd!=pVert;++vd)   {
                        vd->posAndUV.x+=offsetX;
                    }
                }
            }
        }
        //------------------------------------------------------------
    }

    float offsetY=0.f;
    if (props.valign!=SDF_TOP)      {
        offsetY = cursorYlimits.y - cursorYlimits.x - totalSizeInPixels.y;
        if (props.valign==SDF_MIDDLE) offsetY*=0.5f;
    }
    if (offsetY!=0.f) {
        for (int i = startVertexBufferSize,isz=vertexBuffer.size();i<isz;i++)   {
            SdfVertexBuffer::VertexDeclaration& V = vertexBuffer[i];
            V.posAndUV.y+=offsetY;
        }
    }

    /*const float soc = 0.0575f;
    shadowOffsetInPixels.x*=soc;
    shadowOffsetInPixels.y*=soc;*/

    const float soc = 0.0625f*1.5f;
    shadowOffsetInPixels.x=soc*(0.75f*localScaleXminmax.x + 0.25f*localScaleXminmax.y);
    shadowOffsetInPixels.y=soc*(0.75f*localScaleYminmax.x + 0.25f*localScaleYminmax.y);
    //fprintf(stderr,"localScaleXminmax:(%1.3f,%1.3f) localScaleYminmax:(%1.3f,%1.3f)\n",localScaleXminmax.x,localScaleXminmax.y,localScaleYminmax.x,localScaleYminmax.y);

    return true;

    //buffer->updateBoundVbo();
}



struct SdfStaticStructs {
    SdfVector<SdfTextChunk*> gSdfTextChunks;
    SdfVector<SdfCharset*> gSdfCharsets;
    SdfVector<SdfAnimation*> gSdfAnimations;

    SdfStaticStructs() {}
    ~SdfStaticStructs() {DestroyAll();}
    void DestroyAllAnimations();
    void DestroyAllTextChunks();
    void DestroyAllCharsets();
    void DestroyAll() {
        DestroyAllAnimations();
        DestroyAllTextChunks();
        DestroyAllCharsets();
    }
};
static SdfStaticStructs gSdfInit;
void SdfStaticStructs::DestroyAllAnimations()  {
    for (int i=0,isz=gSdfInit.gSdfAnimations.size();i<isz;i++) {
        gSdfInit.gSdfAnimations[i]->~SdfAnimation();
        SDF_FREE(gSdfInit.gSdfAnimations[i]);
        gSdfInit.gSdfAnimations[i] = NULL;
    }
    gSdfInit.gSdfAnimations.clear();

    for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++)  {
        SdfTextChunk* chunk = gSdfInit.gSdfTextChunks[i];
        if (chunk->manualAnimationRef)  {
            if (chunk->animationMode==SDF_AM_MANUAL) {chunk->mute=true;chunk->animationStartTime=-1.f;}
            chunk->manualAnimationRef=NULL;
        }
    }
}
void SdfStaticStructs::DestroyAllTextChunks()  {
    for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++) {
        gSdfInit.gSdfTextChunks[i]->~SdfTextChunk();
        SDF_FREE(gSdfInit.gSdfTextChunks[i]);
        gSdfInit.gSdfTextChunks[i] = NULL;
    }
    gSdfInit.gSdfTextChunks.clear();
}
void SdfStaticStructs::DestroyAllCharsets()  {
    for (int i=0,isz=gSdfInit.gSdfCharsets.size();i<isz;i++) {
        gSdfInit.gSdfCharsets[i]->~SdfCharset();
        SDF_FREE(gSdfInit.gSdfCharsets[i]);
        gSdfInit.gSdfCharsets[i] = NULL;
    }
    gSdfInit.gSdfCharsets.clear();
}
void SdfDestroy() {gSdfInit.DestroyAll();}

SdfCharset* SdfAddCharsetFromFile(const char* fntFilePath,GLuint fntTexture,const SdfCharsetProperties& properties,bool charsetWillDeleteFntTexture)    {
    SdfVector<char> contentOut;
    if (!SdfCharset::GetFileContent(fntFilePath,contentOut,true,"r") || contentOut.size()==0) return NULL;
    return SdfAddCharsetFromMemory(&contentOut[0],contentOut.size(),fntTexture,properties,charsetWillDeleteFntTexture);
}
SdfCharset* SdfAddCharsetFromMemory(const void* data,unsigned int data_size,GLuint fntTexture,const SdfCharsetProperties& properties,bool charsetWillDeleteFntTexture)  {
    SdfCharset* p = (SdfCharset*) SDF_MALLOC(sizeof(SdfCharset));
    SDF_PLACEMENT_NEW (p) SdfCharset();

    if (!p->loadFromMemory(data,data_size,fntTexture,properties,charsetWillDeleteFntTexture)) {
        p->~SdfCharset();
        SDF_FREE(p);
        return NULL;
    }
    gSdfInit.gSdfCharsets.push_back(p);
    return p;
}
SdfCharset* SdfAddCharsetAndTextureFromMemory(const void* fntData,unsigned int fntDataSize,const void* fntRGBAData,int fntRGBAWidth,int fntRGBAHeight,const SdfCharsetProperties& properties)   {
    if (!fntData || fntDataSize==0 || !fntRGBAData || fntRGBAWidth<=0 || fntRGBAHeight<=0) return NULL;
    GLuint fntTexture = 0;

    glGenTextures(1,&fntTexture);
    if (!fntTexture) return NULL;

    glBindTexture(GL_TEXTURE_2D,fntTexture);
    GLenum clampEnum = 0x2900;    // 0x2900 -> GL_CLAMP; 0x812F -> GL_CLAMP_TO_EDGE
#   ifndef GL_CLAMP
#       ifdef GL_CLAMP_TO_EDGE
        clampEnum = GL_CLAMP_TO_EDGE;
#       else //GL_CLAMP_TO_EDGE
        clampEnum = 0x812F;
#       endif // GL_CLAMP_TO_EDGE
#   else //GL_CLAMP
    clampEnum = GL_CLAMP;
#   endif //GL_CLAMP
#   if (defined(__EMSCRIPTEN__) || defined(SDFIMPL_SHADER_GLES))
#       ifdef GL_CLAMP_TO_EDGE
        clampEnum = GL_CLAMP_TO_EDGE;   // Well, WebGL2, OpenGLES2 and upper should have this
#       endif //GL_CLAMP_TO_EDGE
#   endif
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clampEnum);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clampEnum);
    //const GLfloat borderColor[]={0.f,0.f,0.f,1.f};glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fntRGBAWidth, fntRGBAHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, fntRGBAData);
    glBindTexture(GL_TEXTURE_2D,0);

    return SdfAddCharsetFromMemory(fntData,fntDataSize,fntTexture,properties,true);
}

#ifndef SDF_NO_EMBEDDED_FONT
SdfCharset* SdfAddDefaultCharset(const SdfCharsetProperties& properties) {
    SdfVector<char> fntData;SdfVector<unsigned char> rgba;int w,h;
    if (!SdfCharset::GetEmbeddedFontData(fntData,rgba,w,h)) return NULL;
    if (fntData.size()==0 || rgba.size()<4 || w<=0 || h<=0) return NULL;

    return SdfAddCharsetAndTextureFromMemory((void*) &fntData[0],fntData.size(),(void*)&rgba[0],w,h,properties);
}
#endif //SDF_NO_EMBEDDED_FONT

SdfTextChunk* SdfAddTextChunk(SdfCharset* charset, int sdfBufferType, const SdfTextChunkProperties& properties, bool preferStreamDrawBufferUsage) {
    SdfTextChunk* p = (SdfTextChunk*) SDF_MALLOC(sizeof(SdfTextChunk));
    SDF_PLACEMENT_NEW (p) SdfTextChunk(charset,sdfBufferType,properties,preferStreamDrawBufferUsage);
    gSdfInit.gSdfTextChunks.push_back(p);
    return p;
}
SdfTextChunkProperties& SdfTextChunkGetProperties(SdfTextChunk* textChunk)   {
    textChunk->dirty = true;
    return textChunk->props;
}
const SdfTextChunkProperties& SdfTextChunkGetProperties(const SdfTextChunk* textChunk) {
    return textChunk->props;
}
void SdfTextChunkSetStyle(SdfTextChunk* textChunk,int sdfTextBufferType) {
    textChunk->buffer->setType(sdfTextBufferType);
}
int SdfTextChunkGetStyle(const SdfTextChunk* textChunk) {
    return textChunk->buffer->getType();
}
void SdfTextChunkSetMute(SdfTextChunk* textChunk,bool flag) {
    textChunk->setMute(flag);
}
bool SdfTextChunkGetMute(const SdfTextChunk* textChunk)  {
    SDF_ASSERT(textChunk);
    return textChunk->mute;
}
void SdfRemoveTextChunk(SdfTextChunk* chunk)    {
    for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++)  {
        if (chunk == gSdfInit.gSdfTextChunks[i])    {
            gSdfInit.gSdfTextChunks[i] = gSdfInit.gSdfTextChunks[isz-1];
            gSdfInit.gSdfTextChunks.pop_back();
            // destroy chunk
            chunk->~SdfTextChunk();
            SDF_FREE(chunk);
            chunk=NULL;
            break;
        }
    }
}
void SdfRemoveAllTextChunks()   {
    for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++)  {
        SdfTextChunk* chunk = gSdfInit.gSdfTextChunks[i];
        // destroy chunk
        chunk->~SdfTextChunk();
        SDF_FREE(chunk);
    }
    gSdfInit.gSdfTextChunks.clear();
}

void SdfClearText(SdfTextChunk* chunk)    {
    SDF_ASSERT(chunk);
    chunk->clearText();
}
void SdfAddText(SdfTextChunk* chunk, const char* startText, bool italic, const SdfTextColor* pSdfTextColor, const SdfVec2* textScaling, const char* endText,const SDFHAlignment *phalignOverride, bool fakeBold)    {
    SDF_ASSERT(chunk);
    chunk->addText(startText,italic,pSdfTextColor,textScaling,endText,phalignOverride,fakeBold);
}


void SdfAddTextWithTags(SdfTextChunk* chunk,const char* startText,const char* endText)  {
    SDF_ASSERT(chunk);
    if (!startText || startText==endText || startText[0]=='\0') return;
    typedef struct _SdfTagState {
        bool bold;
        bool italic;
        SdfVector<SdfTextColor> color;
        SdfVector<SdfVec2> scaling;
        SdfVector<SDFHAlignment> hAlign;
        _SdfTagState() : bold(false),italic(false) {}
        void SdfAddText(SdfTextChunk* chunk,const unsigned char* startText,const unsigned char* endText) const {
            chunk->addText((const char*)startText,
                           italic,
                           color.size()>0?&color[color.size()-1]:NULL,
                           scaling.size()>0?&scaling[scaling.size()-1]:NULL,
                           (const char*)endText,
                           hAlign.size()>0?&hAlign[hAlign.size()-1]:NULL,
                           bold
                           );
        }
    } SdfTagState;
    SdfTagState TS;

    uint32_t state=UTF8Helper::UTF8_ACCEPT,codePoint=0,lastCodePoint=0,numGlyphs=0;
    const uint32_t startTagCP = '<';    // Hp) startTagCP and endTagCP must be 1 bytes long (in UTF8)
    const uint32_t endTagCP = '>';
    const unsigned char* endTextUC = (const unsigned char*) (endText==NULL?(startText+strlen(startText)):endText);
    const unsigned char* p = (const unsigned char*) startText;
    const unsigned char* startTag=NULL;const unsigned char* endTag=NULL;const unsigned char* startSubchunk = p;
    for(p = (const unsigned char*) startText; p!=endTextUC; ++p)
    {
        if (UTF8Helper::decode(&state, &codePoint, *p)!=UTF8Helper::UTF8_ACCEPT) continue;
        ++numGlyphs;
        if (lastCodePoint == codePoint) continue;
        lastCodePoint = codePoint;
        if (codePoint==startTagCP) startTag = p;
        else if (startTag && codePoint==endTagCP) {
            endTag = p+1;
            bool tagValid = true;
            const char* s = (const char*) (startTag+1);
            const char* e = (const char*) (endTag-1);
            bool negate = false;bool hasEquality = false;
            if (*s=='/') {negate=true;++s;if (s>=e) tagValid=false;}
            const char* equality = e;
            if (tagValid && !negate) {
                equality = strchr(s,'=');
                if (!equality || equality>=e) {
                    hasEquality = false;
                    equality = e;
                }
                else hasEquality = true;
            }
            if (tagValid)   {
                // Parse the two fields
                static char field0[16];field0[0]='\0';
                static char field1[16];field1[0]='\0';
                int cnt=0;bool started = false;
                for (const char* t=s;t!=equality && cnt<15;++t) {
                    if (*t==' ' || *t=='\t') {
                        if (!started) continue;
                        else break;
                    }
                    field0[cnt++]=tolower(*t);   // <ctype.h> tolower()
                    started = true;
                }
                field0[cnt]='\0';
                if (hasEquality) {
                    started = false;bool quoteStarted = false;
                    cnt=0;for (const char* t=equality+1;t!=e && cnt<15;++t) {
                        if (!quoteStarted && (*t==' ' || *t=='\t')) {
                            if (!started) continue;
                            else break;
                        }
                        if (*t=='\'' || *t=='"')    {
                            if (!started) started = true;
                            if (quoteStarted) break;
                            quoteStarted = true;
                            continue;
                        }
                        field1[cnt++]=tolower(*t);   // <ctype.h> tolower()
                        started = true;
                    }
                    field1[cnt]='\0';
                }
                //fprintf(stderr,"Found tag: %.*s (%d): field0:%s field1:%s hasEquality:%s negate:%s [Text before:\"%.*s\"]\n",(int)(e-s),s,(int)(e-s),field0,field1,hasEquality?"true":"false",negate?"true":"false",(int)(startTag-startSubchunk),startSubchunk);

                TS.SdfAddText(chunk,startSubchunk,startTag);
                startSubchunk = endTag;

                // Process Tag and push or pop TS:
                bool error = false;
                if (strcmp(field0,"b")==0)          TS.bold=!negate;
                else if (strcmp(field0,"i")==0)     TS.italic=!negate;
                else if (strncmp(field0,"hal",3)==0)    {
                    if (negate) {if (TS.hAlign.size()>0) TS.hAlign.pop_back();}
                    else if (hasEquality)   {
                        SDFHAlignment hal=SDF_CENTER;
                        if (strncmp(field1,"l",1)==0) hal = SDF_LEFT;
                        else if (strncmp(field1,"c",1)==0) hal = SDF_CENTER;
                        else if (strncmp(field1,"r",1)==0) hal = SDF_RIGHT;
                        else if (strncmp(field1,"j",1)==0) hal = SDF_JUSTIFY;
                        else error = true;
                        if (!error) TS.hAlign.push_back(hal);
                    }
                    else error = true;
                }
                else if (strncmp(field0,"s",1)==0)  {
                    SdfVec2 scaling(1,1);
                    if (negate)  {if (TS.scaling.size()>0) TS.scaling.pop_back();}
                    else if (hasEquality && sscanf(field1, "%f", &scaling.x))  {
                        scaling.y = scaling.x;
                        TS.scaling.push_back(scaling);
                    }
                    else error = true;
                }
                else if (strncmp(field0,"c",1)==0)  {
                    unsigned int  color;
                    if (negate)  {if (TS.color.size()>0) TS.color.pop_back();}
                    else if (hasEquality && sscanf(field1, "%x", &color))  {
                        const bool mustInvertColor = true;
                        if (mustInvertColor) {
                            color = ((color << 8) & 0xFF00FF00 ) | ((color >> 8) & 0xFF00FF );
                            color = (color << 16) | (color >> 16);
                            // TODO: we should still invert R with B if Sdf_USE_BGRA_PACKED_COLOR is defined!
                        }
                        TS.color.push_back(SdfTextColor(color));
                    }
                    else error = true;
                }
                //TODO: other tags here (quad color, vAlign, etc.)
                //if (error) {printf("SdfMarkupError: Can't understand tag: \"%.*s\"\n",(int)(e-s),s);fflush(stdout);}
            }
            startTag=endTag=NULL;
        }

    }
    if (startSubchunk && p) TS.SdfAddText(chunk,startSubchunk,p);
}

void SdfTextColor::SetDefault(const SdfTextColor& defaultColor,bool updateAllExistingTextChunks) {
    gSdfTextDefaultColor=defaultColor;
    if (updateAllExistingTextChunks)    {
        for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++)  {
            gSdfInit.gSdfTextChunks[i]->dirty=true;
        }
    }
}


SdfAnimation* SdfAddAnimation() {
    SdfAnimation* p = (SdfAnimation*) SDF_MALLOC(sizeof(SdfAnimation));
    SDF_PLACEMENT_NEW (p) SdfAnimation();
    gSdfInit.gSdfAnimations.push_back(p);
    return p;
}
void SdfAnimationSetLoopingParams(SdfAnimation* animation,bool mustLoop,bool mustHideTextWhenFinishedIfNotLooping)  {
    SDF_ASSERT(animation);
    animation->looping = mustLoop;
    animation->mustMuteAtEnd = mustHideTextWhenFinishedIfNotLooping;
}

float SdfAnimationAddKeyFrame(SdfAnimation* animation,const SdfAnimationKeyFrame& keyFrame) {
    SDF_ASSERT(animation);
    return animation->addKeyFrame(keyFrame);
}
void SdfAnimationClear(SdfAnimation* animation) {
    SDF_ASSERT(animation);
    animation->clear();
}
void SdfRemoveAnimation(SdfAnimation* animation)    {
    SDF_ASSERT(animation);
    const bool checkItsNotSetToAnyChunk = true;
    if (checkItsNotSetToAnyChunk)   {
        for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++)  {
            SdfTextChunk* chunk = gSdfInit.gSdfTextChunks[i];
            if (chunk->manualAnimationRef==animation)  {
                if (chunk->animationMode==SDF_AM_MANUAL) {chunk->mute=true;chunk->animationStartTime=-1.f;}
                chunk->manualAnimationRef=NULL;
            }
        }
    }

    for (int i=0,isz=gSdfInit.gSdfAnimations.size();i<isz;i++)  {
        SdfAnimation* anim = gSdfInit.gSdfAnimations[i];
        if (anim == animation)  {
            // destroy animation
            animation->~SdfAnimation();
            SDF_FREE(animation);
            // Swap with last element and pop_back
            gSdfInit.gSdfAnimations[i] = gSdfInit.gSdfAnimations[isz-1];
            gSdfInit.gSdfAnimations[isz-1] = NULL;
            gSdfInit.gSdfAnimations.pop_back();
            break;
        }
    }
}
void SdfRemoveAllAnimations() {
    gSdfInit.DestroyAllAnimations();
}
void SdfTextChunkSetManualAnimation(struct SdfTextChunk* chunk,struct SdfAnimation* animation)  {
    SDF_ASSERT(chunk && animation);
    if (chunk->manualAnimationRef != animation) {
        chunk->manualAnimationRef = animation;
        chunk->animationStartTime = -1.f;
    }
}
void SdfTextChunkSetAnimationParams(struct SdfTextChunk* chunk,const SdfAnimationParams& params)   {
    SDF_ASSERT(chunk);
    chunk->animationParams = params;
    chunk->animationStartTime = -1.f;
}
void SdfTextChunkSetAnimationMode(struct SdfTextChunk* chunk,SDFAnimationMode mode) {
    SDF_ASSERT(chunk);
    if (chunk->animationMode!=mode) {
        chunk->animationMode=mode;
        chunk->animationStartTime = -1.f;
    }
}
SDFAnimationMode SdfTextChunkGetAnimationMode(struct SdfTextChunk* chunk) {
    SDF_ASSERT(chunk);
    return chunk->animationMode;
}

const SdfAnimation* SdfTextChunkGetManualAnimation(const SdfTextChunk* chunk)	{
    SDF_ASSERT(chunk);
    return chunk->manualAnimationRef;
}
SdfAnimation* SdfTextChunkGetManualAnimation(SdfTextChunk* chunk)   {
    SDF_ASSERT(chunk);
    return chunk->manualAnimationRef;
}
const SdfAnimationParams& SdfTextChunkGetAnimationParams(const SdfTextChunk* chunk) {
    SDF_ASSERT(chunk);
    return chunk->animationParams;
}
SdfAnimationParams& SdfTextChunkGetAnimationParams(SdfTextChunk* chunk)	{
    SDF_ASSERT(chunk);
    return chunk->animationParams;
}

void SdfTextChunkSetGlobalParams(struct SdfTextChunk* chunk,const SdfGlobalParams& params)    {
    SDF_ASSERT(chunk);
    chunk->globalParams = params;
}
const SdfGlobalParams& SdfTextChunkGetGlobalParams(const struct SdfTextChunk* chunk) {
    SDF_ASSERT(chunk);return chunk->globalParams;
}
SdfGlobalParams& SdfTextChunkGetGlobalParams(struct SdfTextChunk* chunk) {
    SDF_ASSERT(chunk);return chunk->globalParams;
}



bool SdfTextChunk::setupUniformValuesAndDrawArrays(SdfShaderProgram* SP,bool shadowPass,const SdfVec2& screenSize)	{
    if (animationMode==SDF_AM_NONE || (animationParams.startChar==0 && animationParams.endChar==-1))    {
        // is tmpKeyFrame always good even if animationMode==SDF_AM_NONE ?
        if (tmpKeyFrame.alpha==0.f) return false;

	const GLint startCharSize = (tmpKeyFrame.startChar>globalParams.startChar?tmpKeyFrame.startChar:globalParams.startChar)*6;
	const GLint endCharSize1 = (tmpKeyFrame.endChar<0)?buffer->verts.size():(tmpKeyFrame.endChar*6);
	GLint endCharSize = (globalParams.endChar<0)?buffer->verts.size():(globalParams.endChar*6);
	if (endCharSize>=endCharSize1) endCharSize = endCharSize1;

	if (endCharSize<=startCharSize) return false;

    if (shadowPass)	{
        SP->setUniformOffsetAndScale(SdfVec2((tmpKeyFrame.offset.x*screenSize.x)+shadowOffsetInPixels.x*tmpKeyFrame.scale.x
                                            +(screenSize.x*(0.5f-0.5f*tmpKeyFrame.scale.x))  // scaling (not sure this is correct) [can be removed]
                                            ,(tmpKeyFrame.offset.y*screenSize.y)+shadowOffsetInPixels.y*tmpKeyFrame.scale.y
                                            +(screenSize.y*(0.5f-0.5f*tmpKeyFrame.scale.y))  // scaling (not sure this is correct) [can be removed]
                                            ),tmpKeyFrame.scale);
        SP->setUniformAlphaAndShadow(tmpKeyFrame.alpha*0.5f,0.2f);
    }
    else {
        SP->setUniformOffsetAndScale(tmpKeyFrame.offset*screenSize+
                                     (screenSize-screenSize*tmpKeyFrame.scale)*0.5f  // scaling (not sure this is correct) [can be removed]
                                     ,tmpKeyFrame.scale);
        SP->setUniformAlphaAndShadow(tmpKeyFrame.alpha,1.f);
    }

	glDrawArrays(GL_TRIANGLES,startCharSize,endCharSize-startCharSize);
    }
    else {
        if (globalParams.alpha==0.f) return false;
        // We have an animation, that must be limited to a subset of chars.
	const GLint minStartSize = (globalParams.startChar<0)?0:(globalParams.startChar*6);
	const GLint maxEndSize = (globalParams.endChar<0)?buffer->verts.size():(globalParams.endChar*6);
	const int realAnimationParamsStartSize  = (animationParams.startChar*6) > minStartSize ? (animationParams.startChar*6) : minStartSize;
	const int realAnimationParamsEndSize    = (animationParams.endChar<0 || animationParams.endChar*6 > maxEndSize) ? maxEndSize : (animationParams.endChar*6);
	const int realTmpFrameStartSize         = (tmpKeyFrame.startChar*6) > minStartSize ? (tmpKeyFrame.startChar*6) : minStartSize;
	const int realTmpFrameEndSize           = (tmpKeyFrame.endChar<0 || tmpKeyFrame.endChar*6 > maxEndSize) ? maxEndSize : (tmpKeyFrame.endChar*6);

        // 1) draw non-animated edges:
        {
        if (shadowPass)	{
            SP->setUniformOffsetAndScale(SdfVec2(globalParams.offset.x*screenSize.x+shadowOffsetInPixels.x*globalParams.scale.x
                                                +(screenSize.x*(0.5f-0.5f*globalParams.scale.x))  // scaling (not sure this is correct) [can be removed]
                                                ,globalParams.offset.y*screenSize.y+shadowOffsetInPixels.y*globalParams.scale.y
                                                +(screenSize.y*(0.5f-0.5f*globalParams.scale.y))  // scaling (not sure this is correct) [can be removed]
                                                ),globalParams.scale);
            SP->setUniformAlphaAndShadow(globalParams.alpha*0.5f,0.2f);
        }
        else {
            SP->setUniformOffsetAndScale(globalParams.offset*screenSize
                                         +(screenSize-screenSize*globalParams.scale)*0.5f  // scaling (not sure this is correct) [can be removed]
                                         ,globalParams.scale);
            SP->setUniformAlphaAndShadow(globalParams.alpha,1.f);
            }
            // First edge:
	    if (realAnimationParamsStartSize>minStartSize) glDrawArrays(GL_TRIANGLES, minStartSize, realAnimationParamsStartSize-minStartSize);
            // Last Edge:
	    if (maxEndSize-realAnimationParamsEndSize>0) glDrawArrays(GL_TRIANGLES, realAnimationParamsEndSize, maxEndSize-realAnimationParamsEndSize);
        }

        // 2) draw animated portion:
	if (realTmpFrameStartSize<=realAnimationParamsEndSize && realTmpFrameEndSize>=realAnimationParamsStartSize)
	{
        if (shadowPass)	{
            SP->setUniformOffsetAndScale(SdfVec2((tmpKeyFrame.offset.x*screenSize.x)+shadowOffsetInPixels.x*tmpKeyFrame.scale.x
                                                +(screenSize.x*(0.5f-0.5f*tmpKeyFrame.scale.x))  // scaling (not sure this is correct) [can be removed]
                                                ,(tmpKeyFrame.offset.y*screenSize.y)+shadowOffsetInPixels.y*tmpKeyFrame.scale.y
                                                +(screenSize.y*(0.5f-0.5f*tmpKeyFrame.scale.y))  // scaling (not sure this is correct) [can be removed]
                                                ),tmpKeyFrame.scale);
            SP->setUniformAlphaAndShadow(tmpKeyFrame.alpha*0.5f,0.2f);
        }
        else {
            SP->setUniformOffsetAndScale(tmpKeyFrame.offset*screenSize
                                         +(screenSize-screenSize*tmpKeyFrame.scale)*0.5f  // scaling (not sure this is correct) [can be removed]
                                         ,tmpKeyFrame.scale);
            SP->setUniformAlphaAndShadow(tmpKeyFrame.alpha,1.f);
        }
	    const GLint startAnimationCharIndex = realAnimationParamsStartSize>realTmpFrameStartSize?realAnimationParamsStartSize:realTmpFrameStartSize;
	    const GLint endAnimationCharIndex   = realTmpFrameEndSize<realAnimationParamsEndSize?realTmpFrameEndSize:realAnimationParamsEndSize;

	    glDrawArrays(GL_TRIANGLES, startAnimationCharIndex, endAnimationCharIndex-startAnimationCharIndex);
        }    
    }
    return true;
}

void SdfRender(const float pViewport[4],float globalTime) {
    static SdfVec2 displaySizeLast(-1,-1);
    const SdfVec2 displaySize(pViewport[2],pViewport[3]);
    gSdfDisplaySize = displaySize;
    if (displaySize.x==0 || displaySize.y==0) return;
    const bool screenSizeChanged = displaySizeLast.x!=displaySize.x || displaySizeLast.y!=displaySize.y;
    if (screenSizeChanged) {
        displaySizeLast = displaySize;
        for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++) {
            gSdfInit.gSdfTextChunks[i]->dirty = true;
        }
    }

    bool hasRegularFonts=false,hasOutlineFonts=false;
    for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++) {
        SdfTextChunk* c = gSdfInit.gSdfTextChunks[i];
        if (c->textBits.size()==0 || !c->checkVisibleAndEvalutateAnimationIfNecessary(globalTime)) continue;
        hasRegularFonts|=(c->buffer->type==0 || (c->buffer->type&(SDF_BT_SHADOWED)));
        hasOutlineFonts|=(c->buffer->type&(SDF_BT_OUTLINE));
        //if (hasRegularFonts && hasOutlineFonts) break;    // We cannot exit early, because we must evalutate checkVisibleAndEvalutateAnimationIfNecessary(...) for all the text chunks
    }

    if (!hasRegularFonts && !hasOutlineFonts) return;


    const float fb_x = pViewport[0];//pViewportOverride ? pViewportOverride->x*io.DisplayFramebufferScale.x : 0.f;
    const float fb_y = pViewport[1];//pViewportOverride ? pViewportOverride->y*io.DisplayFramebufferScale.y : 0.f;
    const float fb_height = displaySize.y;// * io.DisplayFramebufferScale.y;   // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
    const float fb_width = displaySize.x;// * io.DisplayFramebufferScale.x;
    glViewport((GLint)fb_x, (GLint)fb_y, (GLsizei)fb_width, (GLsizei)fb_height);
    //fprintf(stderr,"%d %d %d %d (%d %d)\n",(GLint)fb_x, (GLint)fb_y, (GLsizei)fb_width, (GLsizei)fb_height,(int)io.DisplaySize.x,(int)io.DisplaySize.y);

    if (hasRegularFonts && !gSdfShaderPrograms[0].program) {
        static bool done = false;
        if (done) return;
        done = true;
        if (!gSdfShaderPrograms[0].loadShaderProgram(false)) return;
        //else {printf("gSdfShaderPrograms[0] OK.\n");fflush(stdout);}
    }
    if (hasOutlineFonts && !gSdfShaderPrograms[1].program) {
        static bool done = false;
        if (done) return;
        done = true;
        if (!gSdfShaderPrograms[1].loadShaderProgram(true)) return;
        //else {printf("gSdfShaderPrograms[1] OK.\n");fflush(stdout);}
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glCullFace(GL_FRONT);       // with this I can leave GL_CULL_FACE as it is
    glDisable(GL_DEPTH_TEST);
    //glEnable(GL_SCISSOR_TEST);    // TO FIX: Does not work well with the y of the glScissor(...) call [when boundsCenter.y changes]


    GLuint lastBoundTexture = 0;
    const SdfVec2 screenSize = displaySize;   // For scissor test
    const SdfVec2 screenOffset = SdfVec2(pViewport[0],pViewport[1]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    if (hasRegularFonts)    {
        SdfShaderProgram& SP = gSdfShaderPrograms[0];
        glUseProgram(SP.program);
        if (screenSizeChanged) SP.resetUniformOrtho(displaySize.x,displaySize.y);
        bool isShadow = false;
        for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++) {
            SdfTextChunk* c = gSdfInit.gSdfTextChunks[i];
            if (c->textBits.size()==0 || !c->tmpVisible) continue;
            hasRegularFonts=(c->buffer->type!=SDF_BT_OUTLINE);
            if (!hasRegularFonts) continue;
            isShadow = (c->buffer->type&(SDF_BT_SHADOWED));

            if (lastBoundTexture!=c->charset->fntTexture) {
                lastBoundTexture=c->charset->fntTexture;
                glBindTexture(GL_TEXTURE_2D,(unsigned long)c->charset->fntTexture);
            }

            if (!c->buffer->vbo) glGenBuffers(1,&c->buffer->vbo);
            glBindBuffer(GL_ARRAY_BUFFER, c->buffer->vbo);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SdfVertexBuffer::VertexDeclaration), 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SdfVertexBuffer::VertexDeclaration),(const void*) (2*sizeof(GLfloat)));
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SdfVertexBuffer::VertexDeclaration), (const void*)(0 + 4*sizeof(GLfloat)));

            if (c->endText(displaySize)) c->buffer->updateBoundVbo();

            glScissor(screenOffset.x+(c->props.boundsCenter.x-c->props.boundsHalfSize.x)*screenSize.x,screenOffset.y+(c->props.boundsCenter.y-c->props.boundsHalfSize.y)*screenSize.y,
            c->props.boundsHalfSize.x*screenSize.x*2.f,c->props.boundsHalfSize.y*screenSize.y*2.f);

            if (isShadow) c->setupUniformValuesAndDrawArrays(&SP,true,screenSize);
            if (!isShadow || (c->buffer->type==SDF_BT_SHADOWED)) c->setupUniformValuesAndDrawArrays(&SP,false,screenSize);

        }
    }
    if (hasOutlineFonts)    {
        SdfShaderProgram& SP = gSdfShaderPrograms[1];
        glUseProgram(SP.program);
        if (screenSizeChanged) SP.resetUniformOrtho(displaySize.x,displaySize.y);
        for (int i=0,isz=gSdfInit.gSdfTextChunks.size();i<isz;i++) {
            SdfTextChunk* c = gSdfInit.gSdfTextChunks[i];
            if (c->textBits.size()==0 || !c->tmpVisible) continue;
            hasOutlineFonts=(c->buffer->type&(SDF_BT_OUTLINE));
            if (!hasOutlineFonts) continue;

            if (lastBoundTexture!=c->charset->fntTexture) {
                lastBoundTexture=c->charset->fntTexture;
                glBindTexture(GL_TEXTURE_2D,(unsigned long)c->charset->fntTexture);
            }

            if (!c->buffer->vbo) glGenBuffers(1,&c->buffer->vbo);
            glBindBuffer(GL_ARRAY_BUFFER, c->buffer->vbo);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(SdfVertexBuffer::VertexDeclaration), 0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(SdfVertexBuffer::VertexDeclaration),(const void*) (2*sizeof(GLfloat)));
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(SdfVertexBuffer::VertexDeclaration), (const void*)(0 + 4*sizeof(GLfloat)));

            if (c->endText(displaySize)) c->buffer->updateBoundVbo();

            glScissor(screenOffset.x+(c->props.boundsCenter.x-c->props.boundsHalfSize.x)*screenSize.x,screenOffset.y+(c->props.boundsCenter.y-c->props.boundsHalfSize.y)*screenSize.y,
            c->props.boundsHalfSize.x*screenSize.x*2.f,c->props.boundsHalfSize.y*screenSize.y*2.f);

            c->setupUniformValuesAndDrawArrays(&SP,false,screenSize);
        }
    }

    glBindTexture(GL_TEXTURE_2D,0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glUseProgram(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
    glCullFace(GL_BACK);
    glDisable(GL_BLEND);

}

#ifndef SDF_NO_EMBEDDED_FONT
bool SdfCharset::GetEmbeddedFontData(SdfVector<char>& fntFileOut,SdfVector<unsigned char>& rawRGBAOut,int& wOut,int& hOut)
{
    // "DejaVuSerifCondensed-Bold.fnt.inl":
    {
        /* We've used this 'text compression' trick here:
        "char id=34      x=97   y=386  width=10   height=9    xoffset=-1   yoffset=1    xadvance=8    page=0    chnl=0 \n"
        "ch id=34 97 386 10 9 -1 1 8 0 0\n"
        and here:
        "kerning first=121 second=46 amount=-1\n"
        "ke 121 46 -1\n"
        [But it's optional, just to save embedded text space]*/
        const char fntData[] = "info face=\"DejaVu Sans Condensed Bold\" size=16 bold=1 italic=0 charset=\"\" unicode=0 stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=-4,-4\n"	\
                               "common lineHeight=19 base=15 scaleW=512 scaleH=512 pages=1 packed=0\n"	\
                               "page id=0 file=\"DejaVuSansCondensed-Bold-512x512.png\"\n"	\
                               "chars count=853\n"	\
                               "ch id=0       426  67   12   19   -2   1    9    0    0 \nch id=13      0    0    0    0    -2   0    0    0    0 \nch id=32      0    0    0    0    -2   0    5    0    0 \nch id=33      254  248  7    16   0    1    7    0    0 \n"	\
                               "ch id=34      97   386  10   9    -1   1    8    0    0 \nch id=35      261  248  16   16   -2   1    12   0    0 \nch id=36      101  26   13   20   -1   0    10   0    0 \nch id=37      268  162  18   17   -2   1    14   0    0 \n"	\
                               "ch id=38      286  162  16   17   -2   1    13   0    0 \nch id=39      504  373  7    9    -1   1    4    0    0 \nch id=40      114  26   9    20   -1   0    7    0    0 \nch id=41      123  26   9    20   -1   0    7    0    0 \n"	\
                               "ch id=42      362  373  12   12   -2   1    8    0    0 \nch id=43      206  329  14   15   -1   2    12   0    0 \nch id=44      429  373  8    11   -2   9    5    0    0 \nch id=45      360  386  10   7    -2   7    6    0    0 \n"	\
                               "ch id=46      169  386  7    8    -1   9    5    0    0 \nch id=47      247  125  10   18   -2   1    5    0    0 \nch id=48      302  162  14   17   -2   1    10   0    0 \nch id=49      277  248  13   16   -1   1    10   0    0 \n"	\
                               "ch id=50      290  248  12   16   -1   1    10   0    0 \nch id=51      316  162  13   17   -2   1    10   0    0 \nch id=52      302  248  14   16   -2   1    10   0    0 \nch id=53      329  162  13   17   -1   1    10   0    0 \n"	\
                               "ch id=54      342  162  14   17   -2   1    10   0    0 \nch id=55      316  248  13   16   -2   1    10   0    0 \nch id=56      356  162  14   17   -2   1    10   0    0 \nch id=57      370  162  14   17   -2   1    10   0    0 \n"	\
                               "ch id=58      502  345  8    13   -1   4    6    0    0 \nch id=59      329  248  9    16   -2   4    6    0    0 \nch id=60      290  329  14   14   -1   3    12   0    0 \nch id=61      467  373  14   10   -1   5    12   0    0 \n"	\
                               "ch id=62      304  329  14   14   -1   3    12   0    0 \nch id=63      338  248  11   16   -1   1    8    0    0 \nch id=64      438  67   18   19   -2   1    14   0    0 \nch id=65      349  248  16   16   -2   1    11   0    0 \n"	\
                               "ch id=66      365  248  13   16   -1   1    11   0    0 \nch id=67      384  162  14   17   -2   1    11   0    0 \nch id=68      378  248  15   16   -1   1    12   0    0 \nch id=69      393  248  12   16   -1   1    10   0    0 \n"	\
                               "ch id=70      405  248  12   16   -1   1    10   0    0 \nch id=71      398  162  15   17   -2   1    12   0    0 \nch id=72      417  248  14   16   -1   1    12   0    0 \nch id=73      431  248  8    16   -1   1    5    0    0 \n"	\
                               "ch id=74      132  26   10   20   -3   1    5    0    0 \nch id=75      439  248  15   16   -1   1    11   0    0 \nch id=76      454  248  12   16   -1   1    9    0    0 \nch id=77      466  248  16   16   -1   1    14   0    0 \n"	\
                               "ch id=78      482  248  14   16   -1   1    12   0    0 \nch id=79      413  162  16   17   -2   1    12   0    0 \nch id=80      496  248  13   16   -1   1    11   0    0 \nch id=81      456  67   16   19   -2   1    12   0    0 \n"	\
                               "ch id=82      0    265  14   16   -1   1    11   0    0 \nch id=83      429  162  13   17   -1   1    10   0    0 \nch id=84      14   265  14   16   -2   1    10   0    0 \nch id=85      442  162  14   17   -1   1    12   0    0 \n"	\
                               "ch id=86      28   265  16   16   -2   1    11   0    0 \nch id=87      44   265  20   16   -2   1    16   0    0 \nch id=88      64   265  15   16   -2   1    11   0    0 \nch id=89      79   265  16   16   -3   1    10   0    0 \n"	\
                               "ch id=90      95   265  14   16   -2   1    10   0    0 \nch id=91      142  26   9    20   -1   0    7    0    0 \nch id=92      257  125  10   18   -2   1    5    0    0 \nch id=93      151  26   10   20   -2   0    7    0    0 \n"	\
                               "ch id=94      107  386  14   9    -1   1    12   0    0 \nch id=95      465  386  12   6    -2   15   7    0    0 \nch id=96      176  386  9    8    -2   0    7    0    0 \nch id=97      318  329  13   14   -2   4    10   0    0 \n"	\
                               "ch id=98      267  125  13   18   -1   0    10   0    0 \nch id=99      331  329  12   14   -2   4    9    0    0 \nch id=100     280  125  14   18   -2   0    10   0    0 \nch id=101     343  329  14   14   -2   4    10   0    0 \n"	\
                               "ch id=102     456  162  11   17   -2   0    6    0    0 \nch id=103     467  162  14   17   -2   4    10   0    0 \nch id=104     481  162  13   17   -1   0    10   0    0 \nch id=105     494  162  7    17   -1   0    5    0    0 \n"	\
                               "ch id=106     120  0    9    21   -3   0    5    0    0 \nch id=107     0    180  13   17   -1   0    10   0    0 \nch id=108     501  162  7    17   -1   0    5    0    0 \nch id=109     150  359  17   13   -1   4    15   0    0 \n"	\
                               "ch id=110     167  359  13   13   -1   4    10   0    0 \nch id=111     357  329  14   14   -2   4    10   0    0 \nch id=112     13   180  13   17   -1   4    10   0    0 \nch id=113     26   180  14   17   -2   4    10   0    0 \n"	\
                               "ch id=114     180  359  11   13   -1   4    7    0    0 \nch id=115     371  329  12   14   -2   4    9    0    0 \nch id=116     109  265  11   16   -2   1    7    0    0 \nch id=117     383  329  13   14   -1   4    10   0    0 \n"	\
                               "ch id=118     191  359  14   13   -2   4    9    0    0 \nch id=119     205  359  17   13   -2   4    13   0    0 \nch id=120     222  359  14   13   -2   4    9    0    0 \nch id=121     40   180  14   17   -2   4    9    0    0 \n"	\
                               "ch id=122     236  359  12   13   -2   4    8    0    0 \nch id=123     161  26   12   20   -1   0    10   0    0 \nch id=124     129  0    7    21   -1   0    5    0    0 \nch id=125     173  26   12   20   -1   0    10   0    0 \n"	\
                               "ch id=126     185  386  14   8    -1   6    12   0    0 \nch id=160     0    0    0    0    -2   0    5    0    0 \nch id=161     120  265  7    16   0    4    7    0    0 \nch id=162     472  67   12   19   -1   1    10   0    0 \n"	\
                               "ch id=163     127  265  13   16   -2   1    10   0    0 \nch id=164     396  329  13   14   -2   3    9    0    0 \nch id=165     140  265  14   16   -2   1    10   0    0 \nch id=166     503  0    7    19   -1   1    5    0    0 \n"	\
                               "ch id=167     294  125  12   18   -2   1    7    0    0 \nch id=168     370  386  9    7    -1   0    7    0    0 \nch id=169     154  265  15   16   0    1    14   0    0 \nch id=170     409  329  11   14   -1   1    8    0    0 \n"	\
                               "ch id=171     374  373  11   12   -1   4    9    0    0 \nch id=172     481  373  14   10   -1   5    12   0    0 \nch id=173     360  386  10   7    -2   7    6    0    0 \nch id=174     169  265  15   16   0    1    14   0    0 \n"	\
                               "ch id=175     379  386  9    7    -1   0    7    0    0 \nch id=176     495  373  9    10   -1   1    7    0    0 \nch id=177     220  329  14   15   -1   2    12   0    0 \nch id=178     437  373  10   11   -2   1    6    0    0 \n"	\
                               "ch id=179     447  373  10   11   -2   1    6    0    0 \nch id=180     199  386  9    8    0    0    7    0    0 \nch id=181     54   180  14   17   -1   4    11   0    0 \nch id=182     306  125  12   18   -2   1    9    0    0 \n"	\
                               "ch id=183     208  386  7    8    -1   5    5    0    0 \nch id=184     215  386  9    8    -1   13   7    0    0 \nch id=185     457  373  10   11   -2   1    6    0    0 \nch id=186     420  329  12   14   -2   1    8    0    0 \n"	\
                               "ch id=187     385  373  12   12   -1   4    9    0    0 \nch id=188     68   180  18   17   -2   1    15   0    0 \nch id=189     86   180  19   17   -2   1    15   0    0 \nch id=190     105  180  18   17   -2   1    15   0    0 \n"	\
                               "ch id=191     123  180  11   17   -1   4    8    0    0 \nch id=192     484  67   16   19   -2   -2   11   0    0 \nch id=193     0    87   16   19   -2   -2   11   0    0 \nch id=194     16   87   16   19   -2   -2   11   0    0 \n"	\
                               "ch id=195     32   87   16   19   -2   -2   11   0    0 \nch id=196     48   87   16   19   -2   -2   11   0    0 \nch id=197     64   87   16   19   -2   -2   11   0    0 \nch id=198     184  265  19   16   -2   1    16   0    0 \n"	\
                               "ch id=199     185  26   14   20   -2   1    11   0    0 \nch id=200     80   87   12   19   -1   -2   10   0    0 \nch id=201     92   87   12   19   -1   -2   10   0    0 \nch id=202     104  87   12   19   -1   -2   10   0    0 \n"	\
                               "ch id=203     116  87   12   19   -1   -2   10   0    0 \nch id=204     500  67   9    19   -2   -2   5    0    0 \nch id=205     128  87   8    19   -1   -2   5    0    0 \nch id=206     136  87   11   19   -3   -2   5    0    0 \n"	\
                               "ch id=207     147  87   9    19   -2   -2   5    0    0 \nch id=208     203  265  16   16   -2   1    12   0    0 \nch id=209     156  87   14   19   -1   -2   12   0    0 \nch id=210     199  26   16   20   -2   -2   12   0    0 \n"	\
                               "ch id=211     215  26   16   20   -2   -2   12   0    0 \nch id=212     231  26   16   20   -2   -2   12   0    0 \nch id=213     247  26   16   20   -2   -2   12   0    0 \nch id=214     263  26   16   20   -2   -2   12   0    0 \n"	\
                               "ch id=215     432  329  14   14   -1   3    12   0    0 \nch id=216     318  125  16   18   -2   0    12   0    0 \nch id=217     279  26   14   20   -1   -2   12   0    0 \nch id=218     293  26   14   20   -1   -2   12   0    0 \n"	\
                               "ch id=219     307  26   14   20   -1   -2   12   0    0 \nch id=220     321  26   14   20   -1   -2   12   0    0 \nch id=221     170  87   16   19   -3   -2   10   0    0 \nch id=222     219  265  13   16   -1   1    11   0    0 \n"	\
                               "ch id=223     334  125  13   18   -1   0    10   0    0 \nch id=224     347  125  13   18   -2   0    10   0    0 \nch id=225     360  125  13   18   -2   0    10   0    0 \nch id=226     373  125  13   18   -2   0    10   0    0 \n"	\
                               "ch id=227     386  125  13   18   -2   0    10   0    0 \nch id=228     399  125  13   18   -2   0    10   0    0 \nch id=229     335  26   13   20   -2   -2   10   0    0 \nch id=230     446  329  19   14   -2   4    15   0    0 \n"	\
                               "ch id=231     134  180  12   17   -2   4    9    0    0 \nch id=232     412  125  14   18   -2   0    10   0    0 \nch id=233     426  125  14   18   -2   0    10   0    0 \nch id=234     440  125  14   18   -2   0    10   0    0 \n"	\
                               "ch id=235     454  125  14   18   -2   0    10   0    0 \nch id=236     146  180  9    17   -3   0    5    0    0 \nch id=237     155  180  9    17   -1   0    5    0    0 \nch id=238     164  180  11   17   -3   0    5    0    0 \n"	\
                               "ch id=239     175  180  9    17   -2   0    5    0    0 \nch id=240     468  125  14   18   -2   0    10   0    0 \nch id=241     184  180  13   17   -1   0    10   0    0 \nch id=242     482  125  14   18   -2   0    10   0    0 \n"	\
                               "ch id=243     496  125  14   18   -2   0    10   0    0 \nch id=244     0    144  14   18   -2   0    10   0    0 \nch id=245     14   144  14   18   -2   0    10   0    0 \nch id=246     28   144  14   18   -2   0    10   0    0 \n"	\
                               "ch id=247     465  329  14   14   -1   3    12   0    0 \nch id=248     234  329  14   15   -2   3    10   0    0 \nch id=249     42   144  13   18   -1   0    10   0    0 \nch id=250     55   144  13   18   -1   0    10   0    0 \n"	\
                               "ch id=251     68   144  13   18   -1   0    10   0    0 \nch id=252     81   144  13   18   -1   0    10   0    0 \nch id=253     136  0    14   21   -2   0    9    0    0 \nch id=254     150  0    13   21   -1   0    10   0    0 \n"	\
                               "ch id=255     163  0    14   21   -2   0    9    0    0 \nch id=256     186  87   16   19   -2   -2   11   0    0 \nch id=257     94   144  13   18   -2   0    10   0    0 \nch id=258     202  87   16   19   -2   -2   11   0    0 \n"	\
                               "ch id=259     107  144  13   18   -2   0    10   0    0 \nch id=260     348  26   16   20   -2   1    11   0    0 \nch id=261     197  180  13   17   -2   4    10   0    0 \nch id=262     364  26   14   20   -2   -2   11   0    0 \n"	\
                               "ch id=263     120  144  12   18   -2   0    9    0    0 \nch id=264     378  26   14   20   -2   -2   11   0    0 \nch id=265     132  144  13   18   -2   0    9    0    0 \nch id=266     392  26   14   20   -2   -2   11   0    0 \n"	\
                               "ch id=267     145  144  12   18   -2   0    9    0    0 \nch id=268     406  26   14   20   -2   -2   11   0    0 \nch id=269     157  144  12   18   -2   0    9    0    0 \nch id=270     218  87   15   19   -1   -2   12   0    0 \n"	\
                               "ch id=271     169  144  19   18   -2   0    10   0    0 \nch id=272     203  265  16   16   -2   1    12   0    0 \nch id=273     188  144  15   18   -2   0    10   0    0 \nch id=274     233  87   12   19   -1   -2   10   0    0 \n"	\
                               "ch id=275     203  144  14   18   -2   0    10   0    0 \nch id=276     245  87   12   19   -1   -2   10   0    0 \nch id=277     217  144  14   18   -2   0    10   0    0 \nch id=278     257  87   12   19   -1   -2   10   0    0 \n"	\
                               "ch id=279     231  144  14   18   -2   0    10   0    0 \nch id=280     420  26   12   20   -1   1    10   0    0 \nch id=281     210  180  14   17   -2   4    10   0    0 \nch id=282     269  87   12   19   -1   -2   10   0    0 \n"	\
                               "ch id=283     245  144  14   18   -2   0    10   0    0 \nch id=284     432  26   15   20   -2   -2   12   0    0 \nch id=285     177  0    14   21   -2   0    10   0    0 \nch id=286     447  26   15   20   -2   -2   12   0    0 \n"	\
                               "ch id=287     191  0    14   21   -2   0    10   0    0 \nch id=288     462  26   15   20   -2   -2   12   0    0 \nch id=289     205  0    14   21   -2   0    10   0    0 \nch id=290     477  26   15   20   -2   1    12   0    0 \n"	\
                               "ch id=291     219  0    14   21   -2   0    10   0    0 \nch id=292     281  87   14   19   -1   -2   12   0    0 \nch id=293     295  87   15   19   -3   -2   10   0    0 \nch id=294     232  265  16   16   -1   1    14   0    0 \n"	\
                               "ch id=295     224  180  14   17   -1   0    11   0    0 \nch id=296     310  87   10   19   -3   -2   5    0    0 \nch id=297     238  180  10   17   -2   0    5    0    0 \nch id=298     320  87   9    19   -2   -2   5    0    0 \n"	\
                               "ch id=299     248  180  9    17   -2   0    5    0    0 \nch id=300     329  87   9    19   -2   -2   5    0    0 \nch id=301     257  180  9    17   -2   0    5    0    0 \nch id=302     492  26   9    20   -1   1    5    0    0 \n"	\
                               "ch id=303     233  0    9    21   -1   0    5    0    0 \nch id=304     338  87   8    19   -1   -2   5    0    0 \nch id=305     248  359  7    13   -1   4    5    0    0 \nch id=306     0    47   13   20   -1   1    11   0    0 \n"	\
                               "ch id=307     242  0    12   21   -1   0    10   0    0 \nch id=308     51   0    11   23   -3   -2   5    0    0 \nch id=309     254  0    11   21   -3   0    5    0    0 \nch id=310     13   47   15   20   -1   1    11   0    0 \n"	\
                               "ch id=311     265  0    13   21   -1   0    10   0    0 \nch id=312     255  359  13   13   -1   4    10   0    0 \nch id=313     346  87   12   19   -1   -2   9    0    0 \nch id=314     358  87   8    19   -1   -2   5    0    0 \n"	\
                               "ch id=315     28   47   12   20   -1   1    9    0    0 \nch id=316     278  0    8    21   -1   0    5    0    0 \nch id=317     248  265  12   16   -1   1    9    0    0 \nch id=318     266  180  10   17   -1   0    7    0    0 \n"	\
                               "ch id=319     260  265  12   16   -1   1    9    0    0 \nch id=320     276  180  10   17   -1   0    8    0    0 \nch id=321     272  265  14   16   -3   1    9    0    0 \nch id=322     286  180  11   17   -3   0    5    0    0 \n"	\
                               "ch id=323     366  87   14   19   -1   -2   12   0    0 \nch id=324     297  180  13   17   -1   0    10   0    0 \nch id=325     40   47   14   20   -1   1    12   0    0 \nch id=326     310  180  13   17   -1   4    10   0    0 \n"	\
                               "ch id=327     380  87   14   19   -1   -2   12   0    0 \nch id=328     323  180  13   17   -1   0    10   0    0 \nch id=329     286  265  18   16   -2   1    14   0    0 \nch id=330     54   47   14   20   -1   1    12   0    0 \n"	\
                               "ch id=331     336  180  13   17   -1   4    10   0    0 \nch id=332     68   47   16   20   -2   -2   12   0    0 \nch id=333     259  144  14   18   -2   0    10   0    0 \nch id=334     84   47   16   20   -2   -2   12   0    0 \n"	\
                               "ch id=335     273  144  14   18   -2   0    10   0    0 \nch id=336     100  47   16   20   -2   -2   12   0    0 \nch id=337     287  144  14   18   -2   0    10   0    0 \nch id=338     349  180  20   17   -2   1    17   0    0 \n"	\
                               "ch id=339     479  329  20   14   -2   4    16   0    0 \nch id=340     394  87   14   19   -1   -2   11   0    0 \nch id=341     369  180  11   17   -1   0    7    0    0 \nch id=342     116  47   14   20   -1   1    11   0    0 \n"	\
                               "ch id=343     380  180  11   17   -1   4    7    0    0 \nch id=344     408  87   14   19   -1   -2   11   0    0 \nch id=345     391  180  11   17   -1   0    7    0    0 \nch id=346     130  47   13   20   -1   -2   10   0    0 \n"	\
                               "ch id=347     301  144  12   18   -2   0    9    0    0 \nch id=348     143  47   13   20   -1   -2   10   0    0 \nch id=349     313  144  12   18   -2   0    9    0    0 \nch id=350     156  47   13   20   -1   1    10   0    0 \n"	\
                               "ch id=351     402  180  12   17   -2   4    9    0    0 \nch id=352     169  47   13   20   -1   -2   10   0    0 \nch id=353     325  144  12   18   -2   0    9    0    0 \nch id=354     182  47   14   20   -2   1    10   0    0 \n"	\
                               "ch id=355     196  47   11   20   -2   1    7    0    0 \nch id=356     422  87   14   19   -2   -2   10   0    0 \nch id=357     414  180  11   17   -2   0    7    0    0 \nch id=358     304  265  14   16   -2   1    10   0    0 \n"	\
                               "ch id=359     318  265  11   16   -2   1    7    0    0 \nch id=360     207  47   14   20   -1   -2   12   0    0 \nch id=361     337  144  13   18   -1   0    10   0    0 \nch id=362     221  47   14   20   -1   -2   12   0    0 \n"	\
                               "ch id=363     350  144  13   18   -1   0    10   0    0 \nch id=364     235  47   14   20   -1   -2   12   0    0 \nch id=365     363  144  13   18   -1   0    10   0    0 \nch id=366     286  0    14   21   -1   -3   12   0    0 \n"	\
                               "ch id=367     249  47   13   20   -1   -2   10   0    0 \nch id=368     262  47   14   20   -1   -2   12   0    0 \nch id=369     376  144  13   18   -1   0    10   0    0 \nch id=370     276  47   14   20   -1   1    12   0    0 \n"	\
                               "ch id=371     425  180  14   17   -1   4    10   0    0 \nch id=372     436  87   20   19   -2   -2   16   0    0 \nch id=373     439  180  17   17   -2   0    13   0    0 \nch id=374     456  87   16   19   -3   -2   10   0    0 \n"	\
                               "ch id=375     300  0    14   21   -2   0    9    0    0 \nch id=376     472  87   16   19   -3   -2   10   0    0 \nch id=377     488  87   14   19   -2   -2   10   0    0 \nch id=378     456  180  12   17   -2   0    8    0    0 \n"	\
                               "ch id=379     0    106  14   19   -2   -2   10   0    0 \nch id=380     468  180  12   17   -2   0    8    0    0 \nch id=381     14   106  14   19   -2   -2   10   0    0 \nch id=382     480  180  12   17   -2   0    8    0    0 \n"	\
                               "ch id=383     492  180  11   17   -2   0    6    0    0 \nch id=884     121  386  7    9    -1   0    4    0    0 \nch id=885     128  386  7    9    -1   12   4    0    0 \nch id=890     224  386  7    8    0    13   7    0    0 \n"	\
                               "ch id=891     499  329  12   14   -2   4    9    0    0 \nch id=892     0    345  12   14   -2   4    8    0    0 \nch id=893     12   345  12   14   -2   4    8    0    0 \nch id=894     329  265  9    16   -2   4    6    0    0 \n"	\
                               "ch id=900     231  386  9    8    0    0    6    0    0 \nch id=901     0    386  10   10   -1   -3   7    0    0 \nch id=902     0    197  16   17   -2   0    11   0    0 \nch id=903     240  386  7    8    -1   5    5    0    0 \n"	\
                               "ch id=904     16   197  16   17   -3   0    12   0    0 \nch id=905     32   197  18   17   -3   0    15   0    0 \nch id=906     50   197  12   17   -3   0    8    0    0 \nch id=908     389  144  18   18   -3   0    13   0    0 \n"	\
                               "ch id=910     62   197  20   17   -3   0    14   0    0 \nch id=911     82   197  18   17   -3   0    13   0    0 \nch id=912     314  0    10   21   -2   -3   6    0    0 \nch id=913     338  265  16   16   -2   1    11   0    0 \n"	\
                               "ch id=914     354  265  13   16   -1   1    11   0    0 \nch id=915     367  265  12   16   -1   1    9    0    0 \nch id=916     379  265  16   16   -2   1    11   0    0 \nch id=917     395  265  12   16   -1   1    10   0    0 \n"	\
                               "ch id=918     407  265  14   16   -2   1    10   0    0 \nch id=919     421  265  14   16   -1   1    12   0    0 \nch id=920     100  197  16   17   -2   1    12   0    0 \nch id=921     435  265  8    16   -1   1    5    0    0 \n"	\
                               "ch id=922     443  265  15   16   -1   1    11   0    0 \nch id=923     458  265  16   16   -2   1    11   0    0 \nch id=924     474  265  16   16   -1   1    14   0    0 \nch id=925     490  265  14   16   -1   1    12   0    0 \n"	\
                               "ch id=926     0    281  11   16   -1   1    9    0    0 \nch id=927     116  197  16   17   -2   1    12   0    0 \nch id=928     11   281  14   16   -1   1    12   0    0 \nch id=929     25   281  13   16   -1   1    11   0    0 \n"	\
                               "ch id=931     38   281  12   16   -1   1    10   0    0 \nch id=932     50   281  14   16   -2   1    10   0    0 \nch id=933     64   281  16   16   -3   1    10   0    0 \nch id=934     80   281  16   16   -2   1    12   0    0 \n"	\
                               "ch id=935     96   281  15   16   -2   1    11   0    0 \nch id=936     111  281  16   16   -2   1    12   0    0 \nch id=937     127  281  16   16   -2   1    12   0    0 \nch id=938     502  87   9    19   -2   -2   5    0    0 \n"	\
                               "ch id=939     472  87   16   19   -3   -2   10   0    0 \nch id=940     407  144  14   18   -2   0    10   0    0 \nch id=941     421  144  12   18   -2   0    8    0    0 \nch id=942     324  0    13   21   -1   0    10   0    0 \n"	\
                               "ch id=943     433  144  9    18   -1   0    6    0    0 \nch id=944     337  0    13   21   -1   -3   10   0    0 \nch id=945     24   345  14   14   -2   4    10   0    0 \nch id=946     350  0    13   21   -1   0    10   0    0 \n"	\
                               "ch id=947     132  197  14   17   -2   4    10   0    0 \nch id=948     442  144  14   18   -2   0    10   0    0 \nch id=949     38   345  12   14   -2   4    8    0    0 \nch id=950     363  0    12   21   -2   0    9    0    0 \n"	\
                               "ch id=951     146  197  13   17   -1   4    10   0    0 \nch id=952     456  144  14   18   -2   0    10   0    0 \nch id=953     50   345  9    14   -1   4    6    0    0 \nch id=954     268  359  13   13   -1   4    10   0    0 \n"	\
                               "ch id=955     159  197  13   17   -2   0    9    0    0 \nch id=956     172  197  14   17   -1   4    11   0    0 \nch id=957     281  359  14   13   -2   4    10   0    0 \nch id=958     375  0    12   21   -2   0    9    0    0 \n"	\
                               "ch id=959     59   345  14   14   -2   4    10   0    0 \nch id=960     73   345  15   14   -2   4    11   0    0 \nch id=961     186  197  13   17   -1   4    10   0    0 \nch id=962     199  197  12   17   -2   4    9    0    0 \n"	\
                               "ch id=963     88   345  15   14   -2   4    11   0    0 \nch id=964     103  345  13   14   -2   4    9    0    0 \nch id=965     116  345  13   14   -1   4    10   0    0 \nch id=966     211  197  15   17   -2   4    11   0    0 \n"	\
                               "ch id=967     226  197  13   17   -2   4    9    0    0 \nch id=968     239  197  15   17   -2   4    11   0    0 \nch id=969     129  345  16   14   -2   4    13   0    0 \nch id=970     470  144  10   18   -2   0    6    0    0 \n"	\
                               "ch id=971     480  144  13   18   -1   0    10   0    0 \nch id=972     493  144  14   18   -2   0    10   0    0 \nch id=973     0    162  13   18   -1   0    10   0    0 \nch id=974     13   162  16   18   -2   0    13   0    0 \n"	\
                               "ch id=976     29   162  14   18   -2   0    9    0    0 \nch id=977     43   162  13   18   -2   0    10   0    0 \nch id=978     143  281  16   16   -2   1    11   0    0 \nch id=979     254  197  20   17   -3   0    14   0    0 \n"	\
                               "ch id=980     28   106  16   19   -2   -2   11   0    0 \nch id=981     387  0    15   21   -2   0    11   0    0 \nch id=982     145  345  17   14   -2   4    13   0    0 \nch id=983     274  197  14   17   -2   4    11   0    0 \n"	\
                               "ch id=984     290  47   16   20   -2   1    12   0    0 \nch id=985     288  197  14   17   -2   4    10   0    0 \nch id=986     306  47   14   20   -2   1    11   0    0 \nch id=987     302  197  12   17   -2   4    9    0    0 \n"	\
                               "ch id=988     159  281  12   16   -1   1    10   0    0 \nch id=989     402  0    12   21   -3   0    7    0    0 \nch id=990     171  281  14   16   -2   1    10   0    0 \nch id=991     314  197  12   17   -1   0    10   0    0 \n"	\
                               "ch id=992     320  47   17   20   -2   1    13   0    0 \nch id=993     185  281  13   16   -2   4    9    0    0 \nch id=994     337  47   20   20   -2   1    16   0    0 \nch id=995     326  197  16   17   -2   4    12   0    0 \n"	\
                               "ch id=996     357  47   15   20   -2   1    12   0    0 \nch id=997     342  197  14   17   -2   4    10   0    0 \nch id=998     372  47   16   20   -1   1    13   0    0 \nch id=999     248  329  14   15   -2   3    11   0    0 \n"	\
                               "ch id=1000    388  47   14   20   -2   1    11   0    0 \nch id=1001    356  197  13   17   -2   4    9    0    0 \nch id=1002    198  281  15   16   -2   1    11   0    0 \nch id=1003    295  359  13   13   -2   4    10   0    0 \n"	\
                               "ch id=1004    56   162  15   18   -2   0    11   0    0 \nch id=1005    71   162  13   18   -1   0    10   0    0 \nch id=1006    402  47   14   20   -2   1    10   0    0 \nch id=1007    416  47   13   20   -2   1    9    0    0 \n"	\
                               "ch id=1008    162  345  14   14   -2   4    11   0    0 \nch id=1009    369  197  13   17   -1   4    10   0    0 \nch id=1010    176  345  12   14   -2   4    9    0    0 \nch id=1011    414  0    9    21   -3   0    5    0    0 \n"	\
                               "ch id=1012    382  197  16   17   -2   1    12   0    0 \nch id=1013    188  345  12   14   -2   4    9    0    0 \nch id=1014    200  345  12   14   -1   4    9    0    0 \nch id=1015    213  281  13   16   -1   1    11   0    0 \n"	\
                               "ch id=1016    423  0    13   21   -1   0    10   0    0 \nch id=1017    398  197  14   17   -2   1    11   0    0 \nch id=1018    226  281  16   16   -1   1    14   0    0 \nch id=1019    412  197  13   17   -1   4    11   0    0 \n"	\
                               "ch id=1020    425  197  14   17   -2   4    10   0    0 \nch id=1021    439  197  14   17   -2   1    10   0    0 \nch id=1022    453  197  14   17   -2   1    11   0    0 \nch id=1023    467  197  14   17   -2   1    10   0    0 \n"	\
                               "ch id=1024    44   106  12   19   -1   -2   10   0    0 \nch id=1025    56   106  12   19   -1   -2   10   0    0 \nch id=1026    429  47   16   20   -2   1    13   0    0 \nch id=1027    68   106  12   19   -1   -2   9    0    0 \n"	\
                               "ch id=1028    481  197  14   17   -2   1    11   0    0 \nch id=1029    495  197  13   17   -1   1    10   0    0 \nch id=1030    242  281  8    16   -1   1    5    0    0 \nch id=1031    80   106  9    19   -2   -2   5    0    0 \n"	\
                               "ch id=1032    501  26   10   20   -3   1    5    0    0 \nch id=1033    250  281  20   16   -2   1    17   0    0 \nch id=1034    270  281  19   16   -1   1    16   0    0 \nch id=1035    289  281  16   16   -2   1    13   0    0 \n"	\
                               "ch id=1036    89   106  15   19   -1   -2   12   0    0 \nch id=1037    104  106  14   19   -1   -2   12   0    0 \nch id=1038    118  106  15   19   -2   -2   11   0    0 \nch id=1039    133  106  14   19   -1   1    12   0    0 \n"	\
                               "ch id=1040    305  281  16   16   -2   1    11   0    0 \nch id=1041    321  281  13   16   -1   1    11   0    0 \nch id=1042    334  281  13   16   -1   1    11   0    0 \nch id=1043    347  281  12   16   -1   1    9    0    0 \n"	\
                               "ch id=1044    147  106  16   19   -2   1    13   0    0 \nch id=1045    359  281  12   16   -1   1    10   0    0 \nch id=1046    371  281  22   16   -2   1    18   0    0 \nch id=1047    0    214  14   17   -2   1    10   0    0 \n"	\
                               "ch id=1048    393  281  14   16   -1   1    12   0    0 \nch id=1049    163  106  14   19   -1   -2   12   0    0 \nch id=1050    407  281  15   16   -1   1    12   0    0 \nch id=1051    422  281  15   16   -2   1    12   0    0 \n"	\
                               "ch id=1052    437  281  16   16   -1   1    14   0    0 \nch id=1053    453  281  14   16   -1   1    12   0    0 \nch id=1054    14   214  16   17   -2   1    12   0    0 \nch id=1055    467  281  14   16   -1   1    12   0    0 \n"	\
                               "ch id=1056    481  281  13   16   -1   1    11   0    0 \nch id=1057    398  197  14   17   -2   1    11   0    0 \nch id=1058    494  281  14   16   -2   1    10   0    0 \nch id=1059    0    297  15   16   -2   1    11   0    0 \n"	\
                               "ch id=1060    15   297  18   16   -2   1    14   0    0 \nch id=1061    33   297  15   16   -2   1    11   0    0 \nch id=1062    177  106  16   19   -1   1    13   0    0 \nch id=1063    48   297  14   16   -1   1    12   0    0 \n"	\
                               "ch id=1064    62   297  20   16   -1   1    18   0    0 \nch id=1065    193  106  22   19   -1   1    19   0    0 \nch id=1066    82   297  17   16   -2   1    14   0    0 \nch id=1067    99   297  18   16   -1   1    15   0    0 \n"	\
                               "ch id=1068    117  297  13   16   -1   1    11   0    0 \nch id=1069    30   214  14   17   -2   1    11   0    0 \nch id=1070    44   214  20   17   -1   1    17   0    0 \nch id=1071    130  297  14   16   -2   1    11   0    0 \n"	\
                               "ch id=1072    212  345  13   14   -2   4    10   0    0 \nch id=1073    84   162  14   18   -2   0    10   0    0 \nch id=1074    308  359  12   13   -1   4    9    0    0 \nch id=1075    320  359  11   13   -1   4    8    0    0 \n"	\
                               "ch id=1076    144  297  15   16   -2   4    12   0    0 \nch id=1077    225  345  14   14   -2   4    10   0    0 \nch id=1078    331  359  19   13   -2   4    14   0    0 \nch id=1079    239  345  12   14   -2   4    8    0    0 \n"	\
                               "ch id=1080    350  359  12   13   -1   4    10   0    0 \nch id=1081    64   214  12   17   -1   0    10   0    0 \nch id=1082    362  359  13   13   -1   4    10   0    0 \nch id=1083    375  359  14   13   -2   4    11   0    0 \n"	\
                               "ch id=1084    389  359  14   13   -1   4    12   0    0 \nch id=1085    403  359  12   13   -1   4    10   0    0 \nch id=1086    251  345  14   14   -2   4    10   0    0 \nch id=1087    415  359  12   13   -1   4    10   0    0 \n"	\
                               "ch id=1088    76   214  13   17   -1   4    10   0    0 \nch id=1089    265  345  12   14   -2   4    9    0    0 \nch id=1090    427  359  13   13   -2   4    8    0    0 \nch id=1091    89   214  14   17   -2   4    9    0    0 \n"	\
                               "ch id=1092    436  0    18   21   -2   0    14   0    0 \nch id=1093    440  359  14   13   -2   4    9    0    0 \nch id=1094    159  297  14   16   -1   4    11   0    0 \nch id=1095    454  359  13   13   -2   4    10   0    0 \n"	\
                               "ch id=1096    467  359  17   13   -1   4    15   0    0 \nch id=1097    173  297  19   16   -1   4    16   0    0 \nch id=1098    484  359  15   13   -2   4    11   0    0 \nch id=1099    0    373  15   13   -1   4    13   0    0 \n"	\
                               "ch id=1100    499  359  12   13   -1   4    9    0    0 \nch id=1101    277  345  12   14   -2   4    9    0    0 \nch id=1102    289  345  17   14   -1   4    14   0    0 \nch id=1103    15   373  13   13   -2   4    9    0    0 \n"	\
                               "ch id=1104    98   162  14   18   -2   0    10   0    0 \nch id=1105    112  162  14   18   -2   0    10   0    0 \nch id=1106    454  0    14   21   -2   0    10   0    0 \nch id=1107    103  214  11   17   -1   0    8    0    0 \n"	\
                               "ch id=1108    306  345  12   14   -2   4    9    0    0 \nch id=1109    318  345  12   14   -2   4    9    0    0 \nch id=1110    503  180  7    17   -1   0    5    0    0 \nch id=1111    114  214  9    17   -2   0    5    0    0 \n"	\
                               "ch id=1112    468  0    9    21   -3   0    5    0    0 \nch id=1113    28   373  18   13   -2   4    14   0    0 \nch id=1114    46   373  17   13   -1   4    14   0    0 \nch id=1115    123  214  14   17   -2   0    11   0    0 \n"	\
                               "ch id=1116    137  214  13   17   -1   0    10   0    0 \nch id=1117    150  214  12   17   -1   0    10   0    0 \nch id=1118    477  0    14   21   -2   0    9    0    0 \nch id=1119    192  297  12   16   -1   4    10   0    0 \n"	\
                               "ch id=1120    162  214  20   17   -2   1    16   0    0 \nch id=1121    330  345  16   14   -2   4    13   0    0 \nch id=1122    204  297  16   16   -2   1    12   0    0 \nch id=1123    220  297  14   16   -2   1    11   0    0 \n"	\
                               "ch id=1124    182  214  17   17   -1   1    15   0    0 \nch id=1125    346  345  14   14   -1   4    12   0    0 \nch id=1126    234  297  19   16   -2   1    14   0    0 \nch id=1127    63   373  16   13   -2   4    12   0    0 \n"	\
                               "ch id=1128    253  297  23   16   -1   1    20   0    0 \nch id=1129    79   373  19   13   -1   4    16   0    0 \nch id=1130    276  297  16   16   -2   1    12   0    0 \nch id=1131    98   373  14   13   -2   4    10   0    0 \n"	\
                               "ch id=1132    292  297  20   16   -1   1    18   0    0 \nch id=1133    112  373  17   13   -1   4    15   0    0 \nch id=1134    38   0    13   24   -2   -3   10   0    0 \nch id=1135    491  0    12   21   -2   0    8    0    0 \n"	\
                               "ch id=1136    312  297  20   16   -2   1    15   0    0 \nch id=1137    0    26   20   21   -2   0    15   0    0 \nch id=1138    199  214  16   17   -2   1    12   0    0 \nch id=1139    360  345  14   14   -2   4    10   0    0 \n"	\
                               "ch id=1140    332  297  16   16   -2   1    12   0    0 \nch id=1141    129  373  14   13   -2   4    10   0    0 \nch id=1142    215  106  16   19   -2   -2   12   0    0 \nch id=1143    215  214  14   17   -2   0    10   0    0 \n"	\
                               "ch id=1144    445  47   21   20   -2   1    17   0    0 \nch id=1145    229  214  19   17   -2   4    15   0    0 \nch id=1146    248  214  19   17   -2   1    15   0    0 \nch id=1147    374  345  16   14   -2   4    12   0    0 \n"	\
                               "ch id=1148    466  47   24   20   -2   -2   20   0    0 \nch id=1149    231  106  21   19   -2   -1   17   0    0 \nch id=1150    490  47   20   20   -2   -2   16   0    0 \nch id=1151    267  214  16   17   -2   1    13   0    0 \n"	\
                               "ch id=1152    0    67   14   20   -2   1    11   0    0 \nch id=1153    283  214  12   17   -2   4    9    0    0 \nch id=1154    143  373  12   13   -2   5    9    0    0 \nch id=1155    135  386  20   9    -11  -1   0    0    0 \n"	\
                               "ch id=1156    388  386  15   7    -8   0    0    0    0 \nch id=1157    247  386  12   8    -8   0    0    0    0 \nch id=1158    259  386  12   8    -8   0    0    0    0 \nch id=1159    271  386  28   8    -14  0    0    0    0 \n"	\
                               "ch id=1160    79   0    41   22   -18  -2   6    0    0 \nch id=1161    0    0    38   26   -17  -4   6    0    0 \nch id=1162    62   0    17   23   -1   -2   14   0    0 \nch id=1163    20   26   15   21   -1   0    12   0    0 \n"	\
                               "ch id=1164    348  297  14   16   -2   1    11   0    0 \nch id=1165    362  297  13   16   -2   1    9    0    0 \nch id=1166    375  297  14   16   -1   1    11   0    0 \nch id=1167    295  214  13   17   -1   4    10   0    0 \n"	\
                               "ch id=1168    252  106  12   19   -1   -2   9    0    0 \nch id=1169    389  297  11   16   -1   1    8    0    0 \nch id=1170    400  297  14   16   -2   1    10   0    0 \nch id=1171    155  373  12   13   -2   4    8    0    0 \n"	\
                               "ch id=1172    14   67   14   20   -1   1    12   0    0 \nch id=1173    308  214  12   17   -1   4    10   0    0 \nch id=1174    264  106  22   19   -2   1    18   0    0 \nch id=1175    414  297  19   16   -2   4    14   0    0 \n"	\
                               "ch id=1176    28   67   14   20   -2   1    10   0    0 \nch id=1177    320  214  12   17   -2   4    8    0    0 \nch id=1178    286  106  15   19   -1   1    11   0    0 \nch id=1179    433  297  13   16   -1   4    10   0    0 \n"	\
                               "ch id=1180    446  297  15   16   -1   1    12   0    0 \nch id=1181    167  373  13   13   -1   4    10   0    0 \nch id=1182    461  297  16   16   -2   1    12   0    0 \nch id=1183    332  214  14   17   -2   0    10   0    0 \n"	\
                               "ch id=1184    477  297  19   16   -2   1    15   0    0 \nch id=1185    180  373  16   13   -2   4    12   0    0 \nch id=1186    301  106  17   19   -1   1    14   0    0 \nch id=1187    496  297  15   16   -1   4    12   0    0 \n"	\
                               "ch id=1188    0    313  19   16   -1   1    16   0    0 \nch id=1189    196  373  16   13   -1   4    13   0    0 \nch id=1190    42   67   21   20   -1   1    18   0    0 \nch id=1191    346  214  17   17   -1   4    15   0    0 \n"	\
                               "ch id=1192    363  214  18   17   -2   1    14   0    0 \nch id=1193    390  345  17   14   -2   4    12   0    0 \nch id=1194    63   67   14   20   -2   1    11   0    0 \nch id=1195    381  214  12   17   -2   4    9    0    0 \n"	\
                               "ch id=1196    318  106  14   19   -2   1    10   0    0 \nch id=1197    19   313  13   16   -2   4    8    0    0 \nch id=1198    32   313  16   16   -3   1    10   0    0 \nch id=1199    393  214  14   17   -2   4    9    0    0 \n"	\
                               "ch id=1200    48   313  16   16   -3   1    10   0    0 \nch id=1201    407  214  14   17   -2   4    9    0    0 \nch id=1202    332  106  15   19   -2   1    11   0    0 \nch id=1203    64   313  14   16   -2   4    9    0    0 \n"	\
                               "ch id=1204    347  106  20   19   -2   1    16   0    0 \nch id=1205    78   313  19   16   -2   4    14   0    0 \nch id=1206    367  106  18   19   -1   1    12   0    0 \nch id=1207    97   313  15   16   -2   4    10   0    0 \n"	\
                               "ch id=1208    112  313  14   16   -1   1    12   0    0 \nch id=1209    212  373  13   13   -2   4    10   0    0 \nch id=1210    126  313  14   16   -1   1    12   0    0 \nch id=1211    421  214  13   17   -1   0    10   0    0 \n"	\
                               "ch id=1212    434  214  19   17   -2   1    15   0    0 \nch id=1213    407  345  15   14   -2   4    12   0    0 \nch id=1214    385  106  19   19   -2   1    15   0    0 \nch id=1215    140  313  15   16   -2   4    12   0    0 \n"	\
                               "ch id=1216    155  313  8    16   -1   1    5    0    0 \nch id=1217    404  106  22   19   -2   -2   18   0    0 \nch id=1218    453  214  19   17   -2   0    14   0    0 \nch id=1219    77   67   15   20   -1   1    11   0    0 \n"	\
                               "ch id=1220    472  214  13   17   -1   4    9    0    0 \nch id=1221    92   67   18   20   -2   1    14   0    0 \nch id=1222    485  214  16   17   -2   4    12   0    0 \nch id=1223    110  67   14   20   -1   1    12   0    0 \n"	\
                               "ch id=1224    0    231  12   17   -1   4    10   0    0 \nch id=1225    124  67   17   20   -1   1    14   0    0 \nch id=1226    12   231  15   17   -1   4    12   0    0 \nch id=1227    426  106  14   19   -1   1    12   0    0 \n"	\
                               "ch id=1228    163  313  13   16   -2   4    10   0    0 \nch id=1229    141  67   19   20   -1   1    16   0    0 \nch id=1230    27   231  17   17   -1   4    13   0    0 \nch id=1231    501  214  7    17   -1   0    5    0    0 \n"	\
                               "ch id=1232    440  106  16   19   -2   -2   11   0    0 \nch id=1233    126  162  13   18   -2   0    10   0    0 \nch id=1234    456  106  16   19   -2   -2   11   0    0 \nch id=1235    139  162  13   18   -2   0    10   0    0 \n"	\
                               "ch id=1236    176  313  19   16   -2   1    16   0    0 \nch id=1237    422  345  19   14   -2   4    15   0    0 \nch id=1238    472  106  12   19   -1   -2   10   0    0 \nch id=1239    152  162  14   18   -2   0    10   0    0 \n"	\
                               "ch id=1240    44   231  16   17   -2   1    12   0    0 \nch id=1241    441  345  14   14   -2   4    10   0    0 \nch id=1242    160  67   16   20   -2   -2   12   0    0 \nch id=1243    166  162  14   18   -2   0    10   0    0 \n"	\
                               "ch id=1244    484  106  22   19   -2   -2   18   0    0 \nch id=1245    60   231  19   17   -2   0    14   0    0 \nch id=1246    176  67   14   20   -2   -2   10   0    0 \nch id=1247    180  162  12   18   -2   0    8    0    0 \n"	\
                               "ch id=1248    79   231  14   17   -1   1    11   0    0 \nch id=1249    93   231  13   17   -2   4    9    0    0 \nch id=1250    0    125  14   19   -1   -2   12   0    0 \nch id=1251    106  231  12   17   -1   0    10   0    0 \n"	\
                               "ch id=1252    14   125  14   19   -1   -2   12   0    0 \nch id=1253    118  231  12   17   -1   0    10   0    0 \nch id=1254    190  67   16   20   -2   -2   12   0    0 \nch id=1255    192  162  14   18   -2   0    10   0    0 \n"	\
                               "ch id=1256    130  231  16   17   -2   1    12   0    0 \nch id=1257    455  345  14   14   -2   4    10   0    0 \nch id=1258    206  67   16   20   -2   -2   12   0    0 \nch id=1259    206  162  14   18   -2   0    10   0    0 \n"	\
                               "ch id=1260    222  67   14   20   -2   -2   11   0    0 \nch id=1261    220  162  12   18   -2   0    9    0    0 \nch id=1262    28   125  15   19   -2   -2   11   0    0 \nch id=1263    35   26   14   21   -2   0    9    0    0 \n"	\
                               "ch id=1264    43   125  15   19   -2   -2   11   0    0 \nch id=1265    49   26   14   21   -2   0    9    0    0 \nch id=1266    58   125  15   19   -2   -2   11   0    0 \nch id=1267    63   26   14   21   -2   0    9    0    0 \n"	\
                               "ch id=1268    73   125  14   19   -1   -2   12   0    0 \nch id=1269    146  231  13   17   -2   0    10   0    0 \nch id=1270    87   125  12   19   -1   1    9    0    0 \nch id=1271    195  313  11   16   -1   4    8    0    0 \n"	\
                               "ch id=1272    99   125  18   19   -1   -2   15   0    0 \nch id=1273    159  231  15   17   -1   0    13   0    0 \nch id=1274    236  67   14   20   -2   1    10   0    0 \nch id=1275    174  231  12   17   -2   4    8    0    0 \n"	\
                               "ch id=1276    250  67   15   20   -2   1    11   0    0 \nch id=1277    186  231  13   17   -2   4    9    0    0 \nch id=1278    206  313  15   16   -2   1    11   0    0 \nch id=1279    225  373  14   13   -2   4    9    0    0 \n"	\
                               "ch id=1280    221  313  13   16   -1   1    11   0    0 \nch id=1281    239  373  12   13   -2   4    9    0    0 \nch id=1282    199  231  19   17   -1   1    17   0    0 \nch id=1283    469  345  17   14   -2   4    13   0    0 \n"	\
                               "ch id=1284    218  231  18   17   -1   1    16   0    0 \nch id=1285    486  345  16   14   -1   4    13   0    0 \nch id=1286    265  67   15   20   -1   1    12   0    0 \nch id=1287    236  231  13   17   -1   4    10   0    0 \n"	\
                               "ch id=1288    249  231  21   17   -2   1    18   0    0 \nch id=1289    0    359  18   14   -2   4    15   0    0 \nch id=1290    270  231  21   17   -1   1    19   0    0 \nch id=1291    18   359  17   14   -1   4    15   0    0 \n"	\
                               "ch id=1292    291  231  15   17   -2   1    12   0    0 \nch id=1293    35   359  12   14   -2   4    9    0    0 \nch id=1294    306  231  17   17   -2   1    14   0    0 \nch id=1295    47   359  15   14   -2   4    12   0    0 \n"	\
                               "ch id=1296    323  231  13   17   -2   1    10   0    0 \nch id=1297    62   359  12   14   -2   4    8    0    0 \nch id=1298    280  67   15   20   -2   1    12   0    0 \nch id=1299    336  231  14   17   -2   4    11   0    0 \n"	\
                               "ch id=1300    234  313  23   16   -2   1    19   0    0 \nch id=1301    251  373  20   13   -2   4    15   0    0 \nch id=1302    257  313  19   16   -1   1    15   0    0 \nch id=1303    350  231  17   17   -1   4    14   0    0 \n"	\
                               "ch id=1304    276  313  19   16   -2   1    16   0    0 \nch id=1305    74   359  18   14   -2   4    15   0    0 \nch id=1306    117  125  16   19   -2   1    12   0    0 \nch id=1307    367  231  14   17   -2   4    10   0    0 \n"	\
                               "ch id=1308    295  313  20   16   -2   1    16   0    0 \nch id=1309    271  373  17   13   -2   4    13   0    0 \nch id=1310    315  313  15   16   -1   1    12   0    0 \nch id=1311    288  373  13   13   -1   4    10   0    0 \n"	\
                               "ch id=1312    295  67   22   20   -2   1    18   0    0 \nch id=1313    381  231  19   17   -2   4    15   0    0 \nch id=1314    317  67   21   20   -1   1    18   0    0 \nch id=1315    400  231  17   17   -1   4    15   0    0 \n"	\
                               "ch id=1316    133  125  17   19   -1   1    14   0    0 \nch id=1317    330  313  15   16   -1   4    12   0    0 \nch id=8192    0    0    0    0    -2   0    7    0    0 \nch id=8193    0    0    0    0    -2   0    14   0    0 \n"	\
                               "ch id=8194    0    0    0    0    -2   0    7    0    0 \nch id=8195    0    0    0    0    -2   0    14   0    0 \nch id=8196    0    0    0    0    -2   0    5    0    0 \nch id=8197    0    0    0    0    -2   0    4    0    0 \n"	\
                               "ch id=8198    0    0    0    0    -2   0    2    0    0 \nch id=8199    0    0    0    0    -2   0    10   0    0 \nch id=8200    0    0    0    0    -2   0    5    0    0 \nch id=8201    0    0    0    0    -2   0    3    0    0 \n"	\
                               "ch id=8202    0    0    0    0    -2   0    1    0    0 \nch id=8203    0    0    0    0    -2   0    0    0    0 \nch id=8204    0    0    0    0    -2   0    0    0    0 \nch id=8210    403  386  14   7    -2   7    10   0    0 \n"	\
                               "ch id=8211    417  386  11   7    -2   7    7    0    0 \nch id=8212    428  386  18   7    -2   7    14   0    0 \nch id=8213    446  386  19   7    -2   7    14   0    0 \nch id=8214    77   26   9    21   -1   0    7    0    0 \n"	\
                               "ch id=8215    299  386  12   8    -2   13   7    0    0 \nch id=8216    10   386  8    10   -1   1    5    0    0 \nch id=8217    18   386  8    10   -2   1    5    0    0 \nch id=8218    26   386  8    10   -1   9    5    0    0 \n"	\
                               "ch id=8219    34   386  8    10   -2   1    5    0    0 \nch id=8220    42   386  12   10   -1   1    9    0    0 \nch id=8221    54   386  11   10   -1   1    9    0    0 \nch id=8222    65   386  11   10   -1   9    9    0    0 \n"	\
                               "ch id=8223    76   386  11   10   -1   1    9    0    0 \nch id=8224    232  162  11   18   -2   1    7    0    0 \nch id=8225    243  162  11   18   -2   1    7    0    0 \nch id=8226    87   386  10   10   0    4    9    0    0 \n"	\
                               "ch id=8230    311  386  17   8    -1   9    14   0    0 \nch id=8239    0    0    0    0    -2   0    3    0    0 \nch id=8240    417  231  25   17   -2   1    21   0    0 \nch id=8242    328  386  8    8    -2   1    4    0    0 \n"	\
                               "ch id=8243    336  386  11   8    -2   1    6    0    0 \nch id=8244    347  386  13   8    -2   1    9    0    0 \nch id=8249    397  373  8    12   -1   4    6    0    0 \nch id=8250    405  373  8    12   -1   4    6    0    0 \n"	\
                               "ch id=8252    345  313  11   16   -1   1    9    0    0 \nch id=8254    465  386  12   6    -2   1    7    0    0 \nch id=8260    442  231  18   17   -5   1    2    0    0 \nch id=8286    504  265  7    15   -1   2    5    0    0 \n"	\
                               "ch id=8352    356  313  17   16   -2   1    13   0    0 \nch id=8353    254  162  14   18   -2   0    10   0    0 \nch id=8354    460  231  14   17   -2   1    10   0    0 \nch id=8355    373  313  13   16   -1   1    10   0    0 \n"	\
                               "ch id=8356    386  313  13   16   -2   1    10   0    0 \nch id=8357    474  231  17   17   -1   2    15   0    0 \nch id=8358    399  313  14   16   -2   1    10   0    0 \nch id=8359    0    248  25   17   -1   1    22   0    0 \n"	\
                               "ch id=8360    491  231  20   17   -1   1    17   0    0 \nch id=8361    413  313  20   16   -2   1    16   0    0 \nch id=8363    86   26   15   21   -2   0    10   0    0 \nch id=8364    25   248  15   17   -3   1    10   0    0 \n"	\
                               "ch id=8365    433  313  15   16   -2   1    10   0    0 \nch id=8366    448  313  14   16   -2   1    10   0    0 \nch id=8367    338  67   21   20   -1   1    20   0    0 \nch id=8368    40   248  14   17   -2   1    10   0    0 \n"	\
                               "ch id=8369    462  313  15   16   -2   1    10   0    0 \nch id=8370    150  125  14   19   -2   0    10   0    0 \nch id=8371    477  313  14   16   -2   1    10   0    0 \nch id=8372    54   248  16   17   -2   1    12   0    0 \n"	\
                               "ch id=8373    359  67   13   20   -1   0    10   0    0 \nch id=8377    491  313  14   16   -2   1    10   0    0 \nch id=8378    0    329  14   16   -2   1    10   0    0 \nch id=8482    155  386  14   9    0    1    14   0    0 \n"	\
                               "ch id=8710    102  329  15   16   -2   1    10   0    0 \nch id=9167    106  359  16   14   -1   3    14   0    0 \nch id=9632    128  248  16   17   -1   2    14   0    0 \nch id=9633    176  248  16   17   -1   2    14   0    0 \n"	\
                               "ch id=9635    192  248  16   17   -1   2    14   0    0 \nch id=9650    160  248  16   17   -2   2    11   0    0 \nch id=9654    96   248  16   17   -2   2    11   0    0 \nch id=9660    144  248  16   17   -2   2    11   0    0 \n"	\
                               "ch id=9664    112  248  16   17   -2   2    11   0    0 \nch id=9733    208  248  16   17   -2   1    13   0    0 \nch id=9734    224  248  16   17   -2   1    13   0    0 \nch id=9744    161  329  15   16   -1   1    13   0    0 \n"	\
                               "ch id=9745    176  329  15   16   -1   1    13   0    0 \nch id=9746    191  329  15   16   -1   1    13   0    0 \nch id=9775    133  329  15   16   -1   1    13   0    0 \nch id=9785    230  125  17   19   -1   0    15   0    0 \n"	\
                               "ch id=9786    213  125  17   19   -1   0    15   0    0 \nch id=9873    148  329  13   16   0    1    13   0    0 \nch id=9986    413  373  16   12   -2   3    12   0    0 \nch id=9992    117  329  16   16   -2   1    12   0    0 \n"	\
                               "ch id=9997    122  359  16   14   -2   2    12   0    0 \nch id=9998    276  329  14   15   -1   1    12   0    0 \nch id=10003   138  359  12   14   0    2    12   0    0 \nch id=10007   240  248  14   17   -1   1    12   0    0 \n"	\
                               "ch id=11360   14   329  13   16   -2   1    9    0    0 \nch id=11361   70   248  10   17   -2   0    5    0    0 \nch id=11362   27   329  14   16   -3   1    9    0    0 \nch id=11363   41   329  14   16   -2   1    11   0    0 \n"	\
                               "ch id=11364   372  67   14   20   -1   1    11   0    0 \nch id=11365   262  329  14   15   -2   3    10   0    0 \nch id=11366   386  67   11   20   -2   -1   7    0    0 \nch id=11367   164  125  17   19   -1   1    14   0    0 \n"	\
                               "ch id=11368   397  67   16   20   -1   0    10   0    0 \nch id=11369   181  125  15   19   -1   1    11   0    0 \nch id=11370   413  67   13   20   -1   0    10   0    0 \nch id=11371   196  125  17   19   -2   1    10   0    0 \n"	\
                               "ch id=11372   55   329  13   16   -2   4    8    0    0 \nch id=11373   80   248  16   17   -2   1    12   0    0 \nch id=11377   301  373  16   13   -2   4    11   0    0 \nch id=11378   68   329  22   16   -2   1    18   0    0 \n"	\
                               "ch id=11379   317  373  20   13   -2   4    15   0    0 \nch id=11380   92   359  14   14   -2   3    9    0    0 \nch id=11381   90   329  12   16   -1   1    10   0    0 \nch id=11382   337  373  10   13   -1   4    8    0    0 \n"	\
                               "ch id=11383   347  373  15   13   -2   4    11   0    0 \n"	\
                               "kernings count=875\n"	\
                               "ke 121 46 -1\nke 8220 65 -2\nke 194 121 -1\nke 87 234 -1\nke 221 79 -1\nke 86 233 -1\nke 84 45 -2\nke 89 195 -2\nke 67 8217 1\nke 356 44 -2\nke 88 79 -1\nke 87 193 -1\n"	\
                               "ke 70 58 -1\nke 70 59 -1\nke 221 65 -2\nke 221 97 -1\nke 75 255 -1\nke 76 79 -1\nke 114 8217 1\nke 256 84 -1\nke 76 85 -1\nke 193 86 -1\nke 119 44 -1\nke 45 88 -1\n"	\
                               "ke 8216 89 1\nke 313 121 -1\nke 376 97 -1\nke 84 99 -2\nke 86 8222 -1\nke 356 235 -2\nke 86 58 -1\nke 320 108 -2\nke 8220 195 -2\nke 376 101 -1\nke 70 114 -1\nke 84 115 -2\n"	\
                               "ke 70 117 -1\nke 196 118 -1\nke 84 119 -2\nke 70 250 -1\nke 196 121 -1\nke 75 214 -1\nke 192 253 -1\nke 88 213 -1\nke 193 356 -1\nke 216 88 -1\nke 82 84 -1\nke 356 229 -2\n"	\
                               "ke 376 227 -1\nke 221 46 -3\nke 260 372 -1\nke 75 121 -1\nke 376 339 -1\nke 376 193 -2\nke 356 194 -1\nke 70 195 -2\nke 89 196 -2\nke 75 199 -1\nke 87 283 -1\nke 195 374 -2\n"	\
                               "ke 84 243 -2\nke 356 46 -2\nke 84 246 -1\nke 313 211 -1\nke 89 58 -1\nke 313 214 -1\nke 356 367 -2\nke 317 216 -1\nke 76 217 -1\nke 313 218 -1\nke 321 219 -1\nke 212 221 -1\n"	\
                               "ke 317 210 -1\nke 344 221 -1\nke 87 224 -1\nke 70 225 -1\nke 356 226 -2\nke 221 227 -1\nke 356 232 -2\nke 221 229 -1\nke 70 230 -1\nke 84 231 -2\nke 89 232 -1\nke 84 234 -2\n"	\
                               "ke 70 235 -1\nke 195 89 -2\nke 87 242 -1\nke 84 242 -1\nke 356 243 -2\nke 356 245 -2\nke 70 246 -1\nke 87 97 -1\nke 70 248 -1\nke 84 249 -2\nke 356 341 -2\nke 356 251 -2\n"	\
                               "ke 70 252 -1\nke 317 253 -1\nke 344 255 -1\nke 86 339 -1\nke 87 196 -1\nke 70 258 -2\nke 70 259 -1\nke 70 260 -2\nke 70 261 -1\nke 86 246 -1\nke 75 268 -1\nke 84 269 -2\n"	\
                               "ke 321 255 -1\nke 8222 87 -2\nke 356 117 -2\nke 70 275 -1\nke 70 277 -1\nke 70 279 -1\nke 192 121 -1\nke 70 281 -1\nke 86 244 -1\nke 84 283 -2\nke 376 249 -1\nke 260 375 -1\n"	\
                               "ke 8218 84 -3\nke 76 220 -1\nke 118 46 -1\nke 88 268 -1\nke 65 89 -2\nke 86 195 -1\nke 84 8222 -2\nke 376 224 -1\nke 194 8222 1\nke 86 252 -1\nke 75 211 -1\nke 211 376 -1\n"	\
                               "ke 376 79 -1\nke 193 8221 -1\nke 256 374 -2\nke 344 89 -1\nke 102 44 -1\nke 313 356 -3\nke 221 224 -1\nke 70 333 -1\nke 70 335 -1\nke 70 337 -1\nke 75 338 -1\nke 84 339 -1\n"	\
                               "ke 70 341 -1\nke 70 343 -1\nke 70 345 -1\nke 83 350 -1\nke 258 121 -1\nke 83 352 -1\nke 84 353 -2\nke 65 354 -1\nke 76 86 -2\nke 89 250 -1\nke 70 361 -1\nke 70 363 -1\n"	\
                               "ke 102 8221 1\nke 70 365 -1\nke 313 366 -1\nke 376 367 -1\nke 258 376 -2\nke 70 369 -1\nke 344 376 -1\nke 70 371 -1\nke 194 372 -1\nke 65 374 -2\nke 65 375 -1\n"	\
                               "ke 214 376 -1\nke 8220 194 -2\nke 70 193 -2\nke 256 221 -2\nke 79 221 -1\nke 195 221 -2\nke 80 196 -1\nke 256 253 -1\nke 66 372 -1\nke 313 8217 -4\nke 321 217 -1\nke 88 199 -1\n"	\
                               "ke 199 8217 1\nke 89 192 -2\nke 341 46 -2\nke 258 354 -1\nke 65 86 -1\nke 221 211 -1\nke 86 192 -1\nke 317 221 -2\nke 89 212 -1\nke 211 89 -1\nke 258 84 -1\nke 84 351 -2\n"	\
                               "ke 221 268 -1\nke 213 88 -1\nke 89 367 -1\nke 356 114 -2\nke 321 356 -3\nke 313 376 -2\nke 196 372 -1\nke 214 89 -1\nke 8216 376 1\nke 270 374 -1\nke 80 44 -3\nke 70 283 -1\n"	\
                               "ke 76 253 -1\nke 376 246 -1\nke 89 45 -2\nke 70 228 -1\nke 321 221 -2\nke 70 121 -1\nke 260 8222 1\nke 376 235 -1\nke 70 97 -1\nke 260 255 -1\nke 321 211 -1\nke 317 213 -1\n"	\
                               "ke 8222 86 -2\nke 344 121 -1\nke 221 214 -1\nke 272 374 -1\nke 376 65 -2\nke 194 255 -1\nke 84 196 -1\nke 8222 89 -3\nke 196 375 -1\nke 86 250 -1\nke 260 8217 -1\nke 356 111 -2\n"	\
                               "ke 68 374 -1\nke 376 251 -1\nke 345 46 -2\nke 376 243 -1\nke 65 253 -1\nke 8220 192 -2\nke 84 97 -2\nke 221 235 -1\nke 313 85 -1\nke 262 8221 1\nke 87 245 -1\nke 317 366 -1\n"	\
                               "ke 350 83 -1\nke 321 216 -1\nke 192 86 -1\nke 70 233 -1\nke 68 221 -1\nke 193 8218 1\nke 356 193 -1\nke 89 226 -1\nke 221 243 -1\nke 221 228 -1\nke 84 226 -1\nke 221 251 -1\n"	\
                               "ke 84 58 -1\nke 376 199 -1\nke 84 229 -1\nke 221 210 -1\nke 75 45 -1\nke 80 195 -1\nke 344 356 -1\nke 192 255 -1\nke 84 244 -1\nke 84 235 -2\nke 272 89 -1\nke 212 376 -1\n"	\
                               "ke 221 225 -1\nke 8216 74 -1\nke 84 255 -2\nke 356 253 -2\nke 195 376 -2\nke 376 210 -1\nke 84 367 -2\nke 86 235 -1\nke 70 192 -2\nke 341 44 -2\nke 70 65 -2\nke 221 199 -1\n"	\
                               "ke 89 97 -1\nke 210 221 -1\nke 84 233 -2\nke 88 211 -1\nke 84 8218 -2\nke 317 79 -1\nke 221 195 -2\nke 82 376 -1\nke 356 224 -2\nke 212 89 -1\nke 8222 84 -3\nke 8218 87 -2\n"	\
                               "ke 76 216 -1\nke 196 374 -2\nke 84 345 -2\nke 313 86 -2\nke 86 224 -1\nke 321 85 -1\nke 89 46 -3\nke 196 221 -2\nke 270 376 -1\nke 221 246 -1\nke 87 228 -1\nke 193 375 -1\n"	\
                               "ke 76 84 -3\nke 376 225 -1\nke 84 121 -2\nke 356 242 -2\nke 89 251 -1\nke 194 84 -1\nke 260 221 -2\nke 89 59 -1\nke 272 221 -1\nke 70 44 -3\nke 87 232 -1\nke 87 244 -1\n"	\
                               "ke 80 46 -3\nke 376 248 -1\nke 260 356 -1\nke 76 121 -1\nke 313 219 -1\nke 70 256 -2\nke 89 101 -1\nke 356 263 -2\nke 8222 356 -3\nke 317 376 -2\nke 221 232 -1\n"	\
                               "ke 70 255 -1\nke 70 244 -1\nke 258 118 -1\nke 258 372 -1\nke 88 338 -1\nke 89 283 -1\nke 208 374 -1\nke 268 8221 1\nke 196 8221 -1\nke 352 350 -1\nke 321 86 -2\nke 196 8222 1\n"	\
                               "ke 376 8222 -2\nke 8216 192 -2\nke 76 366 -1\nke 376 195 -2\nke 8218 356 -3\nke 260 118 -1\nke 376 67 -1\nke 270 221 -1\nke 196 87 -1\nke 210 88 -1\nke 221 262 -1\nke 344 253 -1\n"	\
                               "ke 192 354 -1\nke 70 111 -1\nke 356 353 -2\nke 102 8217 1\nke 194 87 -1\nke 376 46 -3\nke 87 233 -1\nke 88 210 -1\nke 89 245 -1\nke 70 224 -1\nke 86 45 -1\nke 82 253 -1\n"	\
                               "ke 80 8222 -3\nke 221 8222 -2\nke 192 84 -1\nke 79 376 -1\nke 193 8217 -1\nke 86 8218 -2\nke 86 194 -1\nke 260 8221 -1\nke 256 8222 1\nke 68 89 -1\nke 193 374 -2\nke 317 89 -2\n"	\
                               "ke 65 84 -1\nke 356 196 -1\nke 89 224 -1\nke 84 59 -1\nke 65 255 -1\nke 87 192 -1\nke 321 210 -1\nke 195 121 -1\nke 221 194 -2\nke 268 8217 1\nke 356 255 -2\nke 102 46 -1\n"	\
                               "ke 221 252 -1\nke 70 245 -1\nke 89 244 -1\nke 8220 196 -2\nke 212 88 -1\nke 193 221 -2\nke 84 117 -2\nke 376 268 -1\nke 194 118 -1\nke 76 255 -1\nke 193 253 -1\nke 86 230 -1\n"	\
                               "ke 84 111 -2\nke 84 228 -1\nke 356 195 -1\nke 356 248 -1\nke 195 354 -1\nke 313 213 -1\nke 317 338 -1\nke 70 234 -1\nke 89 246 -1\nke 376 252 -1\nke 352 83 -1\nke 75 262 -1\n"	\
                               "ke 65 8218 1\nke 356 269 -2\nke 65 376 -2\nke 45 86 -1\nke 76 218 -1\nke 79 89 -1\nke 256 89 -2\nke 321 8221 -3\nke 86 251 -1\nke 45 356 -2\nke 317 8221 -4\nke 317 212 -1\n"	\
                               "ke 76 376 -2\nke 256 354 -1\nke 221 245 -1\nke 356 115 -2\nke 88 216 -1\nke 89 44 -3\nke 194 86 -1\nke 376 59 -1\nke 70 229 -1\nke 86 97 -1\nke 221 45 -2\nke 89 214 -1\n"	\
                               "ke 70 101 -1\nke 89 8222 -2\nke 255 44 -1\nke 114 44 -2\nke 66 221 -1\nke 8216 198 -2\nke 317 121 -1\nke 195 8218 1\nke 321 87 -1\nke 89 233 -1\nke 192 376 -2\nke 270 89 -1\n"	\
                               "ke 84 192 -1\nke 88 67 -1\nke 89 193 -2\nke 272 376 -1\nke 89 235 -1\nke 317 84 -3\nke 199 8221 1\nke 321 214 -1\nke 87 44 -1\nke 356 65 -1\nke 317 217 -1\nke 256 121 -1\n"	\
                               "ke 266 8217 1\nke 89 243 -1\nke 86 283 -1\nke 221 226 -1\nke 89 65 -2\nke 86 225 -1\nke 260 87 -1\nke 256 8218 1\nke 376 226 -1\nke 321 220 -1\nke 87 111 -1\nke 321 213 -1\n"	\
                               "ke 66 86 -1\nke 70 194 -2\nke 356 121 -2\nke 8218 89 -3\nke 89 210 -1\nke 262 8217 1\nke 89 225 -1\nke 317 356 -3\nke 89 228 -1\nke 356 8218 -2\nke 210 89 -1\nke 258 86 -1\n"	\
                               "ke 356 225 -2\nke 376 242 -1\nke 86 44 -2\nke 87 243 -1\nke 84 101 -2\nke 313 210 -1\nke 84 227 -1\nke 82 89 -1\nke 192 356 -1\nke 196 354 -1\nke 376 212 -1\nke 195 375 -1\n"	\
                               "ke 89 199 -1\nke 194 356 -1\nke 82 121 -1\nke 376 45 -2\nke 260 89 -2\nke 89 252 -1\nke 196 376 -2\nke 66 87 -1\nke 76 338 -1\nke 340 376 -1\nke 317 214 -1\nke 194 221 -2\n"	\
                               "ke 196 86 -1\nke 221 234 -1\nke 253 44 -1\nke 313 221 -2\nke 313 84 -3\nke 356 99 -2\nke 266 8221 1\nke 194 8221 -1\nke 89 234 -1\nke 87 230 -1\nke 65 372 -1\nke 256 375 -1\n"	\
                               "ke 75 79 -1\nke 221 242 -1\nke 86 65 -1\nke 8216 65 -2\nke 75 210 -1\nke 221 59 -1\nke 321 218 -1\nke 258 8217 -1\nke 356 58 -1\nke 65 118 -1\nke 70 243 -1\nke 256 255 -1\n"	\
                               "ke 321 84 -3\nke 87 195 -1\nke 70 227 -1\nke 82 356 -1\nke 376 245 -1\nke 8216 221 1\nke 221 230 -1\nke 194 374 -2\nke 195 118 -1\nke 195 372 -1\nke 213 89 -1\nke 321 89 -2\n"	\
                               "ke 317 211 -1\nke 221 283 -1\nke 313 8221 -4\nke 84 194 -1\nke 356 228 -2\nke 88 45 -1\nke 84 224 -1\nke 255 46 -1\nke 87 101 -1\nke 208 221 -1\nke 345 8217 1\nke 89 117 -1\n"	\
                               "ke 313 87 -1\nke 356 233 -2\nke 350 350 -1\nke 321 338 -1\nke 89 8218 -3\nke 89 262 -1\nke 195 255 -1\nke 208 376 -1\nke 86 248 -1\nke 82 221 -1\nke 256 372 -1\nke 87 339 -1\n"	\
                               "ke 340 253 -1\nke 196 253 -1\nke 80 194 -1\nke 193 121 -1\nke 356 8222 -2\nke 193 376 -2\nke 221 67 -1\nke 86 101 -1\nke 260 376 -2\nke 256 356 -1\nke 84 44 -2\nke 66 374 -1\n"	\
                               "ke 76 356 -3\nke 221 196 -2\nke 76 89 -2\nke 317 219 -1\nke 87 235 -1\nke 65 121 -1\nke 86 232 -1\nke 321 253 -1\nke 356 250 -2\nke 84 230 -2\nke 86 245 -1\nke 313 255 -1\n"	\
                               "ke 192 118 -1\nke 89 194 -2\nke 70 251 -1\nke 86 59 -1\nke 86 234 -1\nke 87 246 -1\nke 192 8222 1\nke 317 8217 -4\nke 89 79 -1\nke 86 117 -1\nke 70 249 -1\nke 8218 221 -3\n"	\
                               "ke 89 213 -1\nke 76 214 -1\nke 213 221 -1\nke 84 245 -1\nke 258 356 -1\nke 45 84 -2\nke 192 375 -1\nke 221 248 -1\nke 356 252 -2\nke 356 192 -1\nke 260 86 -1\nke 86 46 -2\n"	\
                               "ke 356 246 -2\nke 260 374 -2\nke 376 196 -2\nke 193 89 -2\nke 256 8217 -1\nke 376 230 -1\nke 356 244 -2\nke 195 8217 -1\nke 195 356 -1\nke 221 44 -3\nke 84 65 -1\nke 196 356 -1\n"	\
                               "ke 196 89 -2\nke 340 89 -1\nke 86 226 -1\nke 356 231 -2\nke 256 376 -2\nke 194 8217 -1\nke 70 257 -1\nke 356 45 -2\nke 65 8222 1\nke 80 65 -1\nke 89 339 -1\nke 193 8222 1\n"	\
                               "ke 8220 74 -1\nke 356 339 -1\nke 313 220 -1\nke 8216 194 -2\nke 317 255 -1\nke 84 251 -2\nke 258 8218 1\nke 87 46 -1\nke 45 87 -1\nke 258 87 -1\nke 8218 86 -2\nke 352 352 -1\n"	\
                               "ke 86 229 -1\nke 65 8217 -1\nke 89 111 -1\nke 376 338 -1\nke 221 338 -1\nke 119 46 -1\nke 65 356 -1\nke 196 8217 -1\nke 221 193 -2\nke 80 193 -1\nke 88 212 -1\nke 356 227 -2\n"	\
                               "ke 195 8222 1\nke 344 84 -1\nke 195 253 -1\nke 67 8221 1\nke 260 253 -1\nke 76 219 -1\nke 376 211 -1\nke 114 46 -2\nke 83 83 -1\nke 75 213 -1\nke 317 85 -1\nke 82 255 -1\n"	\
                               "ke 86 367 -1\nke 192 372 -1\nke 79 88 -1\nke 70 8218 -3\nke 70 339 -1\nke 253 46 -1\nke 76 87 -1\nke 76 8217 -4\nke 70 8222 -3\nke 192 221 -2\nke 84 114 -2\nke 356 119 -2\n"	\
                               "ke 194 376 -2\nke 86 193 -1\nke 192 8221 -1\nke 208 89 -1\nke 8220 193 -2\nke 87 65 -1\nke 194 253 -1\nke 376 214 -1\nke 76 213 -1\nke 70 196 -2\nke 84 341 -2\nke 313 212 -1\n"	\
                               "ke 258 375 -1\nke 340 121 -1\nke 192 87 -1\nke 356 97 -2\nke 75 212 -1\nke 260 121 -1\nke 376 244 -1\nke 376 229 -1\nke 356 101 -2\nke 356 345 -2\nke 194 89 -2\nke 70 226 -1\n"	\
                               "ke 192 8217 -1\nke 84 193 -1\nke 195 84 -1\nke 258 8221 -1\nke 84 250 -2\nke 317 220 -1\nke 356 234 -2\nke 193 118 -1\nke 121 44 -1\nke 214 221 -1\nke 340 84 -1\nke 196 84 -1\n"	\
                               "ke 8216 193 -2\nke 75 67 -1\nke 8222 221 -3\nke 194 354 -1\nke 70 375 -1\nke 89 242 -1\nke 65 221 -2\nke 84 195 -1\nke 376 117 -1\nke 376 8218 -3\nke 221 58 -1\nke 193 84 -1\n"	\
                               "ke 196 8218 1\nke 193 87 -1\nke 193 255 -1\nke 376 232 -1\nke 376 233 -1\nke 356 59 -1\nke 88 214 -1\nke 340 356 -1\nke 84 252 -2\nke 89 227 -1\nke 89 230 -1\nke 214 88 -1\n"	\
                               "ke 84 248 -1\nke 45 376 -2\nke 258 89 -2\nke 356 283 -2\nke 8216 196 -2\nke 195 86 -1\nke 75 253 -1\nke 87 229 -1\nke 313 216 -1\nke 76 221 -2\nke 193 354 -1\nke 317 86 -2\n"	\
                               "ke 84 232 -2\nke 221 117 -1\nke 221 8218 -3\nke 221 233 -1\nke 84 253 -2\nke 376 58 -1\nke 80 8218 -3\nke 89 249 -1\nke 192 89 -2\nke 221 249 -1\nke 376 228 -1\nke 193 372 -1\n"	\
                               "ke 87 225 -1\nke 340 255 -1\nke 86 227 -1\nke 192 374 -2\nke 45 89 -2\nke 376 283 -1\nke 256 86 -1\nke 313 253 -1\nke 89 67 -1\nke 313 79 -1\nke 256 8221 -1\nke 264 8221 1\n"	\
                               "ke 260 84 -1\nke 86 242 -1\nke 376 111 -1\nke 89 268 -1\nke 221 250 -1\nke 258 253 -1\nke 194 375 -1\nke 87 194 -1\nke 87 227 -1\nke 350 352 -1\nke 321 212 -1\nke 321 121 -1\n"	\
                               "ke 317 87 -1\nke 260 354 -1\nke 86 111 -1\nke 192 8218 1\nke 84 46 -2\nke 213 376 -1\nke 321 376 -2\nke 70 367 -1\nke 376 213 -1\nke 258 374 -2\nke 321 366 -1\nke 211 221 -1\n"	\
                               "ke 221 212 -1\nke 86 243 -1\nke 88 262 -1\nke 89 229 -1\nke 258 221 -2\nke 87 248 -1\nke 195 8221 -1\nke 70 232 -1\nke 376 250 -1\nke 70 253 -1\nke 313 217 -1\nke 86 228 -1\n"	\
                               "ke 89 248 -1\nke 211 88 -1\nke 80 192 -1\nke 221 101 -1\nke 256 87 -1\nke 89 211 -1\nke 87 226 -1\nke 260 8218 1\nke 86 249 -1\nke 76 211 -1\nke 45 221 -2\nke 221 213 -1\n"	\
                               "ke 221 244 -1\nke 376 192 -2\nke 8216 195 -2\nke 313 89 -2\nke 356 230 -2\nke 65 8221 -1\nke 221 367 -1\nke 376 234 -1\nke 76 210 -1\nke 70 242 -1\nke 221 339 -1\nke 8218 376 -3\n"	\
                               "ke 345 44 -2\nke 66 376 -1\nke 376 262 -1\nke 70 46 -2\nke 195 87 -1\nke 221 111 -1\nke 210 376 -1\nke 89 338 -1\nke 341 8217 1\nke 221 192 -2\nke 8220 198 -2\nke 356 351 -2\n"	\
                               "ke 87 45 -1\nke 321 79 -1\nke 258 8222 1\nke 76 8221 -4\nke 118 44 -1\nke 356 249 -2\nke 86 196 -1\nke 68 376 -1\nke 84 263 -2\nke 376 44 -3\nke 65 87 -1\nke 196 255 -1\n"	\
                               "ke 313 338 -1\nke 66 89 -1\nke 317 218 -1\nke 376 194 -2\nke 340 221 -1\nke 84 225 -2\nke 194 8218 1\nke 321 8217 -3\nke 256 118 -1\nke 258 255 -1\nke 264 8217 1\nke 8222 376 -3\n"	\
                               "ke 76 212 -1\n";

        fntFileOut.resize(sizeof(fntData)/sizeof(fntData[0]));
        memcpy(&fntFileOut[0],fntData,fntFileOut.size());
    }
    // "DejaVuSerifCondensed-Bold Image":
    {        
        /* RGBA palette in little-endian encoding */
        unsigned int palette[55] = {
        0U,1258291199U,520093695U,2348810239U,3422552063U,2667577343U,2432696319U,184549375U,1325400063U,2013265919U,2717908991U,2533359615U,1895825407U,2197815295U,1761607679U,1476395007U,1593835519U,1694498815U,2969567231U,788529151U,1610612735U,2080374783U,3070230527U,2483027967U,3288334335U,1409286143U,3170893823U,2902458367U,2264924159U,2785017855U,301989887U,369098751U,234881023U,1375731711U,587202559U,1174405119U,1577058303U,1006632959U,1073741823U,1862270975U,83886079U,704643071U,1157627903U,855638015U,66977277U,167772159U,956301311U,905969663U,419430399U,452984831U,268435455U,687865855U,654311423U,285212671U,3791650815U};

        /* Each entry when uncompressed to 512*512 represents an index into the palette array */
        const char indices[] = {
        "0~fPb~2^0~5ObO0abf0~AXOO^0~aN~2]0~RN~20~:]]00]0~iN~20~[N~20~9XN~20~6XN~20~8XN~20~}0~j]]00]0~TY``20~IPaa^O2b0~4PV?~2`]0~3a8?Qd?""?Z70~?2ZIIZR0~K2Y~22007C~2O0~6CI~2[0~32Y~220~9R__[__Y0~7R8IIC0~7][ZVbUZ_70~6RY~2a0~9^22b0~42Y~220~32Y~222Y~220~3R8IIC0~32Y~220~72Y~220~5XRCe70~8R8IIC0~7]`I~2C0~4]`I~2C0~6]`I~2C0~5Pe``CO0~4^dY~6b00^dY~6b0~5aY~2R0~9bdd2N0~42Y~22002Y~220~<XeY~2f0~7OY~2R0~92Y~220~2][ZVbUZ_70~4P22fae2]0~2^Y~2N0~<aI99V]0~GXV?""?_QDIR0~3[A;;6TY0~3b?;<@E31P0~;]X0O19GGEQb0X]0~FP1>~21P0YAWW_0~52ILG9V0~2c1>~2170~7XS99D99IN0~5cSEGLQa0~6fQ=<UA=T20~5OI>~2ZX0~8[DDIb0~2c1>~21700c1>~211>~21700PZEGLQb00c1>~2170~5c1>~2170~3X`TWAVf0~6cSEGLQa0~6R?6GW_0~4R?6GW_0~6R?6GW_0~4bZA<9>8d0~3YA>~6S^0YA>~6S^0~3]V>~2Ia0~6X[?>>D8R0~2P1>~21Pc1>~2170~;R@>~2`0~7U>~2?20~7P1>~21P00fQ=<UA=T20~3]U@@VIATY0~2dA>>`0~7eVR00"
        "[WM9_0~H^S=38=5>d0~2R?3LA6=Qb0~27Z955;W_770~8^CVSUY>:E95<[_ZZCP0~DfQ6K68P0Y>K9_0~4PV9KE1b0~2NQGK68P0~7XSL:D;GIN0~5CA5=M<UX0~5^S=6E36IO0~5a?;KLS^0~72I3;?b0~2NQGK68P00NQGK68QGK68P00CA5=M<UX0NQGK68P0~5NQGK68P0~3e@65;>[0~6CA5=M<UX0~4O1EM9ZO0~3O1EM9ZO0~5O1EM9ZO0~3OS95BFM=I20~2CWK~53170CWK~53170~3]Z=K5?b0~6dD6MM5<[0~2fQ6K68PNQGK68P0~;eAMK9_0~7UEK:DR0~7fQ6K68P00^S=6E36IO0~3]VEG?E5WC0~2Y>K9_0~6RQESPO8LM>Y00Nd20~DCW5A;<?R0~2YD>?1D>?200`1~2D>T1~3`]0~6`AEL9I<E8SE989L=A_X0~CfQ6M68P0Y>K9_00^ccCA;=Q2cP00NQG468a2O0~4^cS==D==INP0~3bQL3?=GTeP0~5C@=6L>_P0~5a?;MLScP0~5PZ9K=ScP00NQGM68P00NQGM68QGM68P0aQL3?EGTe0NQG468Pc~300NQG468P0~27_96DL=Z770~2^bQL3?=GTeP0~2R_W:91Y0~2Xc_W:91b0~5R_W:91Y0~4[>:F66FK<`0~2CW;~3:F3170CW;;5B:;3170~3]Z=H5?b0~5]VEFF66<[0~2fQ6M68PNQG468a2O0~77beAMJ9_2N0~4XUEJ:DRcc0~5fQ6M68P0^cC@=6L>_"
        "cP0~3YA:"">;9Ie0~2Y>K9_0~6UE5AYbT55T22_8A820~BRU?G9;T`a0~2^e13~2DR00713~2?dV93~28P0~5OI36L59VUYdUU95L3G?20~CP1A~21P0Y>K9_07V8~2>>I8~21b0NQG468?TQ`P007V8~2V_V8~21b0~22?>D?""?>A81a0~3Y1?""?8~31a0~3Y1?A~2881a0~3Y1?<~2881a0c1A~2S700c1A~2S1A~2S702?>D8?>AC0NQG468Y8~3[0NQG468P00`1196@L=1~2e07V8?>DC?>A81b0XS99>99IN0^U88>>?TI_P0~2XS99>99IN0~2XZ=J;T?;JEV0~2RQIIW6M=DU]0RQW;M9D?IU]0~22VQ=H5?SYX0~3^1345@IAC0~2P1A~21PNQG468?TQ`P0~4RS?TAMJ9?T8CX00XU8EJ:D8~2[0~4P1A~21P7V8~2S818~21b00^_Q3E5D`c0~2Y>K9_0~6CWK=82TEET2193:9170~@XZ93G;;L<Sa0~2^134:DR007134;?R?G4468P0~5RD3DS931700XZ=EST3>d0~CfQ6~28P0Y>K9_0OQ366IdVE66Ae0NQG469;53>_X0OQ366IaVE66Ae0~2Y?=;;=E66Ae0~2Y?=;;=E66Ae0~2Y?=;;=E66Ae0~2Y?=;;=E66Ae0NQ6~28P00NQ6~28Q6~28P00RQ6~28e^0NQG468IL669V0NQG468P0713~265;9339`0OQ366IaVE66Ae0XSL:D;GIN0NQ6~29G56>`0~2XSL:D;G"
        "IN0~2c8646QIGJEV0~3OS955<1d^0~2VEJ;ICda^0~22Q9654MG=D[0~37134GI2e70~2fQ6~28PNQG469;53>_X0~2RIE;5LMJE;5LDC00P866HM6~2=V]0~3fQ6~28POQ366IaVE66Ae00O?3;:53>_]00Y>K9_0~62?;M9V`UU`U9MK3AV^0~@XZLFKKJH:<`0~2^134:DR007134;?U9Bg468P0~5OZ1VR11`0~3[11YV1Sb0~CfQ6468P0Y>K9_00U9BM>CI6438N0NQG46M:FHGTR00U9BM>CI6438N00N1=FJMM=JMAe00N1=FJMM=JMAe00N1=FJMM=JMAe00N1=FJMM=JMAe0NQG468P00NQG468QG468P00fQ6468P00NQG46Q=BM9120NQG468P07134:DWA9FF9`00U9BM>CI6438N0XS==D==IN0NQG46MMJH3IO00^S==D==IN0~2NQG46>9K;AC0~3_W:K9Sa0~4`<5K=W>ZX00]V9KHK4FFJ3Ia0~27134GDD1f0~2fQ6468PNQG46M:FHGTR00^Z9KHMKH4F:FJ3Ia0P865HK5~2=V]0~3fQ6468P0U9BM>CI6438N00O?;;GKH6IO00Y>K9_0~671E3TC0~3Y?3EI[P0~AXZL9W>=FH=ZX00^134:DR007134;?""?6FM468P0~5bdda0PP]0~4PP0add2]0~BfQ6468P0Y>K9_00RD5JEZA:B9_00NQG4K9A=FB9_00RD5JEZA:B9_0~2eA:46>W;4MAe00eA:46>W;4MAe0"
        "0eA:46>W;4MAe00eA:46>W;4MAe0NQG468P00NQG468QG468P00fQ6468P00NQG46EKM91200NQG468P07134:DC`9FF9`00RD5JEZA:B9_0~2YV8~2V[]0NQG4BE>3H:DR0PV8~2_8~2C0~2NQG46=F:=A_^00b?6H3I20~5[@EGJKLS^00aI645=H5LJM>C0~27134M:;IO0~2fQ6468PNQG4K9A=FB9_002?;4;><MJ=A3HM>C0^ZTEJ:DDWA1R0~3fQ6468P0RD5JEZA:B9_0~2OI>?IEJ;?a00Y>69_0~4^00RQD[]0~4CTIR0~D`IUCCAMHLSX00^134:DR007134;?<K;;468P0~3][?>>IC0~9YQA>?_^0~AfQ6468P0Y>K9_00P1=JG?9F5DR00NQG4G?dA:HLSX0P1=JG?9F5DR0~2[<BF9_Z=HMAe00[<BF9_Z=HMAe00[<BF9_Z=HMAe00[<BF9_Z=HMAe0NQG468P00NQG468QG468P00fQ6468P00NQG46MKE820~2NQG468P07134:D2`9FF9`00P1=JG?9F5DR0~2a?66LS^00NQG4;?UEJMAe0a?66LS<63?R00NQG46=6MJGDY00C>MF9_0~52?3FF;3LS^00RA:J==H5>KF9U0~271345;;IO0~2fQ6468PNQG4G?dA:HLSX0d>MJ=ZAMJ9_WKF9U00NUEJ:95K:E1O0~2fQ6468P0P1=JG?9F5DR0~3eIEEGK=1c002188Y0~3RSUY22eX0~5]e2~2]0~BXd?TAEBK9_0~2"
        "^134:DR007134;?6FE;468P0~3RT3:M3?20~7aILM:6AY0~AfQ6468P0Y>K9_0~2[WMKW3HL1c00NQG4682T5431^00[WMKW3HL1c0~2`9FB<CUEJMAe00`9FB<CUEJMAe00`9FB<CUEJMAe00`9FB<CUEJMAe0NQG468P00NQG468QG468P00fQ6468P00NQG4F4GA[]0~2NQG468P07134:D2`9FF9`0~2[WMKW3HL1c0~2a?;4LS^00NQG468_9JMAe0a?;4LSI6BEV^0NQG468Q9BFEV]0`9FK>Y0~5[<KF=T81[0~2d>MF9=H5D:JEV0~27134GIIZP0~2fQ6468PNQG4682T5431^0CWKJ9_AMJ9_AMJEV0~2UEJM5;M4M>C0~2fQ6468P00[WMKW3HL1c0~3OI;FJ3WV^00dA>>`0~3`99>D?20~72ID?_a0~BCW5:BG9Q20~2^134:DR007134;<K;D;468P0~3`9GA>;<C0~7Y>5>A3EU0~AfQ6468P0Y>K9_0~2bI6J=5B<_0~2NQG4GIRD5HLS^00bI6J=5B<_0~3[<BF9_Z=JMAe00[<BF9_Z=JMAe00[<BF9_Z=JMAe00[<BF9_Z=JMAe0NQG468P00NQG468QG468P00fQ6468P00NQG465J3TC0~2NQG468P07134:D2`9FF9`0~2bI6J=5B<_0~3a?;4LS^00NQG468_9JMAe0a?;4LSV=JG?a0NQG4GIRD:431^0`9FK>[2a0~3_9FM>Y2a]0~2RA:JE=H5AMJEU0~27134GIO"
        "P0~3fQ6468PNQG4GIRD5HLS^0Y>KJEVAMJ9_>KJEU0~2UEJJ=?<KJEV0~2fQ6468P00bI6J=5B<_0~4OI<93J;T200Y>K9_0~3`9KM:Ae0~7RA:6<QY0~ACW5:BF;W_0~2^134:DR007134;3FEI;468P0~3`>>VS>AC0~7YD>1U>>U0~AfQ6468P0Y>K9_0~2]VEF5B5@R0~2NQG4:WI9BFEU0~2]VEF5B5@R0~3eA:46A>;4MAe00eA:46A>;4MAe00eA:46A>;4MAe00eA:46A>;4MAe0NQG468P00NQG468QG468P00fQ6468P00NQG469MFL?d00NQG468P07134:D2`9FF9`0~2]VEF5B5@R0~3a?;4LS^00NQG468_9JMAe0a?;4LS_9J:DR0NQG4:WI<KHLS^0C>MJ3>DI`700[<BF=AD?`700a?G4;=H5=FKWC0~27134GIN0~4fQ6468PNQG4:WI9BFEU002@546T>MJ9I=JK<[0~2UEJM>YD:HLS^00fQ6468P00]VEF5B5@R0~4RVSSWKMAe00Y>K9_0~3Y?9;MAe0~7RA:KM9_0~Ad?TD9:438N00^134:DR0071345M5AI;468[[a00fddabddP0~7^ed2OddO0~AfQ6468P0Y>K9_0~3dA5gHL1P0~2NQG4;:GM45AY0~3dA5gHL1P0~3N1=FJMM=JMAe00N1=FJMM=JMAe00N1=FJMM=JMAe00N1=FJMM=JMAe0NQG468P00NQG468QG468P00fQ6468P00NQG468EBB=QR0NQG"
        "468P07134:DR`9FF9`0~3dA5gHL1P0~3a?;4LS^00NQG468_9JMAe0a?;4LS_9J:AR0NQG4;:G:4M<`00aILFJM:6AC00RT6JFM:6AC00XV9KHM4BFJ3Ib0~27134GIN0~4fQ6468PNQG4;:GM45AY00P1=FF;5H4MGKHG?20~2UEJ:DRT54LS^00fQ6468P0~2dA5gHL1P0~3b8ELL;H;T200Y>K9_0~42VTATR0~72?AWE9_0~AbdYYI;4;?O00^134:DR0071344J=SI;46<~2_0~4P[18Zd0~5RV11`N0~DfQ6468P0Y>K9_0~3f1LHK<`0~3NQG46=MK5<ZP0~3f1LHK<`0~5Y?=55=EJMAe0~2Y?=55=EJMAe0~2Y?=55=EJMAe0~2Y?=55=EJMAe0NQG56?e00NQG568QG468P00fQ6468P00NQG568IL55E1NNQG568P0^SLHM>YUEJB<[0~3f1LHK<`0~4a?;4LS^00NQG568_9JMAe0a?;4LSVEJ5T20NQG46=MK:E8b0~2C?EG55FL170X`D=G55FL1700289;K4F:LDC0~2PO134GIN0~4fQ6468PNQG46=MK5<ZP0~2YT3MK;MJ3MK;>_]0~2UE55DRA:J=V]00fQ6468P0~2f1LHK<`0~4[<M5~2G<V700Y>K9_0~5XReR2]0~62ReCV8Y0~@CI>~2EM46QN00^134:DR007134g5ACI;4FBBEU0~3]_>L3E?e0~3RIE3LWV^0~B2_IG438P0Y>K9_0~2R_U9F;T20~3NQG468>WD"
        "Za0~3R_U9F;T20~5`WITTI3H5T20~2`WITTI3H5T20~2`WITTI3H5T20~2`WITTI3H5T20PS@WGTCO0PS@~2ZIG431P02_IG438P00PS@~2ZV?@~21fPS@~2ZO00U9FF=DAG4:Ad0~2R_U9F;T20~4a?;431C^0PS@~2Z_9JMAe0O?;431T646QO0NQG468>WA1R0~4eVITDGGQN00^YZITDGGQN0~22ZT=H5A8CX007ZII64GIN0~32_IG438PNQG468>WDZa0~3]CI>WAMJ9>WTUf0~3`@~2I_<KK<[002_IG438P00R_U9F;T20~4UEF9T@IUR0~2Y>K9_0~721TC0~5Y?820XP0~@aILMKKF4M<U]0aR834:D20071355=1OI;55JJEU0~3aI63E;EVYUUCU9;EL;T20~BU9EMJ=ZX0Y>K9_00]Z99GJ=S70~3NQG468YCRP0~3]Z99GJ=S70~5_96E9LKF=1P0~2_96E9LKF=1P0~2_96E9LKF=1P0~2_96E9LKF=1P00OZ=LDW_00O22V9EMJ=ZX0U9EMJ=ZX0~2O22dA==>Y2O00dA==>Y00dD;HF::JFL8O00]Z99GJ=S70~4O?;4G9@200O2~2N_9JMAe0c83H:L6FK9U]0NQG468YCeN0~6XOSE538c0~3^OSE538c0~37Z=H5?b0~3NIL=KHL1P0~3U9EMJ=ZXNQG468YCRP0~6bYCAMJ9_CR^0~472~21<GFLIb00U9EMJ=ZX0]Z99GJ=S70~4[<KG==L91P00Y>69_0~5^Y89L?d0~3"
        "eIL9S70~BY>MB;556<1b0c1A<"":4;?a00XV@~2TCc1@@LJGA[0~3RDLDV9L1A9E>Z=EV?L>d0~BUEJF;AC00Y>K9_00]Z=FF;AC0~4NQGF68P0~6]Z=FF;AC0~6_9KFJFMLTY0~3_9KFJFMLTY0~3_9KFJFMLTY0~3_9KFJFMLTY0~2]VE::9_0~4VEJF;AC00UEJF;AC0~6_9F=1O0~3_9F=1O00^U>6MKK5LTC0~2]Z=FF;AC0~5f8LKF:D20~6_9FMAe00`>G:KM;EI200NQGF68P0~:XSL:9U]0~4XSL:9U]0~3]Z=F5?b0~3NIGBB5W`0~4UEJF;AC0NQGF68P0~;eAMF9_0~:713:=?C0~2UEJF;AC00]Z=FF;AC0~5b89366G68P002188Y0~4]_D=MKEVC``CUEK;?20~BCWK;?T@IS[P0NIGK4BEZ^0~2N2~3X0O1EK6A_70~3NVSU2ZSD;G65ASSe_SVO0~BUEE9AUP00Y>69_00]ZEE9AU70~4NQ<~21P0~6]ZEE9AU70~6CTW9E9>QC]0~3CTW9E9>QC]0~3CTW9E9>QC]0~3CTW9E9>QC]0~3R8AAT[0~4VEE9AU700UEE9AUP0~5P1LGTd0~3P1LGTd0~3N_IAWWA8C]0~2]ZEE9AU70~6C?<<W?20~6_<~2Ae00fU?A>>TSR0~2NQ<~21P0~:XS<>1b0~5XS<>1b0~4]Z<~2?b0~3P1A<<AZO0~4UEE9AUP0NQ<~21P0~;eA<~2_0~:719>1Y0~3UEE9AUP00]ZEE9AU70~62V1"
        "QQIDS70~2P~20~571EMGEIR?99TRQ=KWC0~BeA5K3L33=AY0NIGK5=?R0~9XUW:6A`^0~5]X]0X[<6?I39_X00X]0~CRUU_d70~221QQY0~2eUU_Y70~6d[~2R0~8eUU_Y70~7X2C_U_Ca0~5X2C_U_Ca0~5X2C_U_Ca0~5X2C_U_Ca0~6Oed2X0~4RUU_d70~2RUU_d70~6XUII_X0~3XUII_X0~5adCCRN0~5eUU_Y70~82C[CR0~7a[~2C70~2]aedd2P0~4d[~2R0~<2[YO0~72[YO0~62[~2Y]0~4Od[[dP0~5RUU_d70~2d[~2R0~<7C[~2a0~;YUCO0~4RUU_d70~3eUU_Y70~8]PffORa0~=RIW?Veb?55T2CA5EV0~BPZ<3;5;;M>Y0N8>>@1d0~:NQ<<D`^0~;YIIU_IIC0~RPff0~}0~j^OOX0~5^OOX0~}0~}0~}00RCbX0eA:3IaPSE?Y0~CaU8?~3AAY002YYRP0~<d[[C^0~=OO^^OO0~}0~}0~}0~}0~}0~X[<K<_00eSC0~FfababYCP0~}0~}0~}0~}0~}0~}0~8VEE?20~2X0~}0~}0~}0~}0~}0~}0~XRUU[]0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~O]]00]0~WN~4P0~n7~2^0~c7OaO^0~7ObbO0~>abbc0~9Pb~2^0~:ObPabO0~8NbbObb70~6ObbO0~<abbc0~7Pb~2^0~7NbbObb70~E^7~2^0~=abbc0~7Pb~2^0~87b~2]0~7abO0abO0~37~70"
        "~7Pb~2^0~7ObO0abf0~97b~2]0~9^OaOP0~57~2X0~67~2^0~9RY~2O0~8][ZVbUZ_70~7aY~420~6R__[__Y0~7eQI8IIZc0~27C~4O0~7NY~2e]0~5OYYa0~7ad~2f002d~2^0~5`1~2ZO002Y~4a0~2RY~4P0~5P2dYYN00^dYYRN0~8a`1I?IZCN0~4O1?""?820~<Y?~2Z^0~7PV?~2`]0~8YQ?1ITSc0~6PS?""?Q?""?U0~5O1?""?820~:Y?~2Z^0~5PV?~2`]0~5PS?""?Q?""?U0~7XRCe70~7OZ1~2S20~;Y?~2Z^0~5PV?~2`]0~7_?~2C0~6d?""?8C?""?Qb00`1~7[0~5PV?~2`]0~5a8?Qd?""?Z70~8_?~2C0~7a[1I?I1Ue^00`1~2ZO0~4`1~2ZO0~72?>~2U0~8fQ=<UA=T20~6XZ>~48f0~4XS99D99IN0~5^VEG>G3Ia0~2YAW~3_0~5RC_>~2TC70~3]V>>V]0~5PS>~2_0O8>>AC0~4^13~2DR0P1>~4VX02?>~3A[0~42Z@>~2_00YA>>A1e0~6R8<LG;6=>VX0~3aI=59VP0~:a1=59S70~7[A;;6TY0~7O8L;EWGIO0~6OIGGA5LSX0~4aI=59VP0~8a1=59S70~5[A;;6TY0~5OIGGA5LSX0~5X`TWAVf0~6YA3~2<[0~:a1=59S70~5[A;;6TY0~6]Z=59`0~6YA;ETL3?20713~6LSX0~4[A;;6TY0~5b?;<@E31P0~7]Z=59`0~6R1WLG;G39DC0713~2D20"
        "~3^13~2DR0~72D:KEV0~8^S=6E36IO0~6XSLK~2GIN0~4XSL:D;GIN0~5RT;E95>[0~3Y>K9K9_0~4P1<<EKM<AY0~2X2Z=LZda0~4YA5M>C0P1=K=1N0~3^134:DR0fQ6K~2LSX02D:K~29`0~3^Z95MKEU00CWKK:=8O0~42QEMHJBFJ31^0~4dI=;AC0~9]U<M9120~7e?3LA6=8a0~62D5<6:=S70~6OIGGA5LSX0~47dI=;AC7~20~377U<M912770~3e?3LA6=8a70~3OIGGA5LS770~4e@65;>[0~5]VEF4H=S70~8]U<M9120~5e?3LA6=8a0~5]Z=59`0~6^UW:3;A[0071344H~3LSX0~3e?3LA6=8a0~47Z955;W_0~8]Z=59`0~52QEMHJFFJF9`07134:D20~3^134:DR0~2^c~2]2D:JEV00O22N0~2XC@=6L>_cX0~4^cS<~4QNP0~2^cS==D==INP0~2^cU9GT;9ZcP0~2Y>K9K9_0~4c86BB4JK>Y00cUT<=L<AIC0~2^ZEF3QO00C>M:AY0~3^134:DR0fQ643LLSX02DLL:F9`0~3b?;45LEU00CWLGJM>Y0~3XV9K45E<9331^0~32U1I>>Ve]0~62U1>>8Ve]0~6YA>?""?D>?20~62?>I?>Id]0~621@@?@@U]0~3`1~2I>>1~3e00`1~2>>1~4e00`1A>?`D>?11e00`11@@I@@1~2e0~3_96DL=ZX0~4OI64MH;T20~7a`1>>IZCN0~4YA>?""?D>?20~4a`1@@TZCN"
        "0~4a`1>~2ZCN07134:L~4SX0~3YA>?""?D>?e^0~421D>TI1Ue^0~4a[1@@T1Ue^00XV9K45=<9=;9`07134:D20~3^134:DR00PV8~2`2D:JEVX[QTT8b0^U8~318~2UX0~27V8~2_[[8~21b07V8~2V_V8~21b07V88>>D>?881b00Y>K9K9_0~3Y1T<<EJM<>Y00`>GKFFBM9_0~22T;F9U0~2aI6JEZX0~2^134:DR0fQ64LSZC00fUZA:F9`0~32D:JESZe00bVSWKB<C0~3b?G4M<Z[_I91^0~2YI<3;;6ET`^0~4YI<3;;6ET`^0~4YI<3;;6ET`^0~4YI<3;;6ET`^0~4YI<3;;6ET`^00713~2Dd`9339`0713~2Dd`9339`0713~2D2`9339`0713~2D2`9339`0~3_96@L=ZX0~4YA:FEBK<[0~6R8<LG;6=>VX0~2R8<LG;6=>VX0~2R8<LG;6=>VX0~2R8<LG;6=>VX7134:DS~3C0~3R1WLG;G39DC0~3R1WLG;G39DC0~3R1WLG;G39DC00b?G4M<S[`Z@9`07134:D20~3^134:DR00OI6~2Q2D:JEVYD355AR0NQ6~28A6~28P0~2OQ366IaVE66Ae0OQ366IaVE66Ae0OQ366IeVE66Ae00Y>K9K9_0~2Y?=;;=EJMACP0c83H6=L=69_0~2[<K:Ad0~2XVEJGIa0~2^134:DR0fQ64LSX0~3]RA:F9`0~32D:F9`X0~3]d>MB<C0~3Y>M46Qa00b_e0~2RILK4FFHF6DC0~3"
        "RILK4FFHF6DC0~3RILK4FFHF6DC0~3RILK4FFHF6DC0~3RILK4FFHF6DC007134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0~3eD65;>[0~4]VEFM>5H=S70~42QEMHJBFJ31^002QEMHJBFJ31^002QEMHJBFJ31^002QEMHJBFJ31^7134:W~3>[0~22QEMHJFFJF9`0~22QEMHJFFJF9`0~22QEMHJFFJF9`00Y>M46QRV~4C07134:D20~3^134:DR00]VEF:AdD:JEVS=JK5AR0NQG468=J468P0~3U9BM>CI6438N00U9BM>CI6438N00U9BM>CI6438N00Y>K9K9_00f1=FJMM=JMAe00a?;F9=LZIA`00]V=J;?a0~3[<B:Ae0~2^134:DR0fQ64LSX0~4RA:F9`0~2XRD:F9`0~5d>MB<CX0~2C<BH=Z]0~6^VEBHG993FJL8O00^VEBHG993FJL8O00^VEBHG993FJL8O00^VEBHG993FJL8O00^VEBHG993FJL8O07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0~2CSITWA8_O0~3OI64GI3H;T20~3XV9K45E<9331^0XV9K45E<9331^0XV9K45E<9331^0XV9K45E<9331^7134JB~39`00XV9K45=<9=;9`00XV9K45=<9=;9`00XV9K45=<9=;9`00C<BH=ZUE~48c7134:D20~3^134:DR0~2YA:F9UD:JEV?;J=T?20N"
        "QG46A54468P0~3RD5JEZA:B9_0~2RD5JEZA:B9_0~2RD5JEZA:B9_0~2Y>K9K9_00eA546>W;4MAe00OIG4;=L>?UN00713438P0~3Y>MB<C0~2^134:DR0fQ64LSX0~4RA:F9`00bZS>MF<`0~5eAMFE1Sd00C<BH=V]0~62?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0071=6;5;3WZc0~2YA:J=Z9FK<[0~3b?G4M<Z[_I91^0b?G4M<Z[_I91^0b?G4M<Z[_I91^0b?G4M<Z[_I91^7134B:~39`00b?G4M<S[`Z@9`00b?G4M<S[`Z@9`00b?G4M<S[`Z@9`00C<BH=VUEJ~268c7134:D20~3^134:DR0~2OQ3438D:JEVWKKW[200NQG46=JJ468P0~3P1=JG?9F5DR0~2P1=JG?9F5DR0~2P1=JG?9F5DR0~2Y>K9K9_00[<BF9UZ=HMAe00XV9MHHFMGWU]0c864LS^0~3RAMF9`0~2^134:DR0fQ64LSX0~4RA:F9`00CWL;F;Ad0~5b?6B:3EU00C>K4L1P007RO00Y>M438NXVEJF9_00Y>M438NXVEJF9_00Y>M438NXVEJF9_00Y>M438NXVEJF9_00Y>M438NXVEJF9_07134:D2`9FF9`07134:D2`9FF9`071"
        "34:D2`9FF9`07134:D2`9FF9`00713:;;MH5AY00]VEFK<~2:H=S70~2Y>M46Qa00b_e00Y>M46Qa00b_e00Y>M46Qa00b_e00Y>M46Qa00b_e07134:D~4C00Y>M46QRV~4C00Y>M46QRV~4C00Y>M46QRV~4C00C>K4L1UE=G468c7134:D20~3^134:DR0~3U9F5AD:JES=JG?20~2NQG465MG468P0~4[WMKW3HL1c0~3[WMKW3HL1c0~3[WMKW3HL1c0~2Y>K9K9_00`9FB<CUEJMAe0~221W=6:K4GIa0c864L1^0~3RAMF9`0~2^134:DR0fQ64LSX0~4RA:F9`00CWKF:=?20~5N1E;JKEU002T545A[2eVDVX0C<BH=Z]0[<BJEV00C<BH=Z]0[<BJEV00C<BH=Z]0[<BJEV00C<BH=Z]0[<BJEV00C<BH=Z]0[<BJEV07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0071EA?""?WMFEU00OI64HF~2J4;T20~2C<BH=Z]0~6C<BH=Z]0~6C<BH=Z]0~6C<BH=Z]0~57134:D[~3R00C<BH=ZUE~48c0C<BH=ZUE~48c0C<BH=ZUE~48c02T545A`VIG468c7134:D20~3^134:DR0~3eA5F9D:JE?;JEZ^0~2NQG4KJLG468PP0~3bI6J=5B<_0~4bI6J=5B<_0~4bI6J=5B<_0~3Y>K9K9_00[<BF9`VEJMAe00PV1[S=L9B:DR0713438P0~3Y>MBWC0~2"
        "^134:DR0fQ64LSX0~4RA:F9`00YD>EFK<[0~5RD5H3>>_00P1=FJ3>TA931^0C<BH=V]0[<BJ=V00C<BH=V]0[<BJ=V00C<BH=V]0[<BJ=V00C<BH=V]0[<BJ=V00C<BH=V]0[<BJ=V07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`00]`T9=LL5H=ZX0YA:H;~4FK<[0~2C<BH=V]0~6C<BH=V]0~6C<BH=V]0~6C<BH=V]0~57134:<~48f0C<BH=VUEJ~268c0C<BH=VUEJ~268c0C<BH=VUEJ~268c0P1=FJ6>TDG468c7134:D20~3^134:DR0~3f1LH63:H33KM>C0~3NQG4g:"">G46881R0~2]VEF5B5@R0~4]VEF5B5@R0~4]VEF5B5@R0~3Y>K9K9_00Y>MHL?DG4MAe00O?=>?=L<K5D20]VEJ;?a0~3`<B:Ae0~2^134:DR0fQ64LSX0~4RA:F9`00^eYA:F9`0~5dAMB<`df0~2Y@6FHM5:B31^0C>KHL170_9FJEU00C>KHL170_9FJEU00C>KHL170_9FJEU00C>KHL170_9FJEU00C>KHL170_9FJEU07134:DR`9FF9`07134:DR`9FF9`07134:DR`9FF9`07134:DR`9FF9`00RT6BBM:BHLZX]VEFB<?~2>MH=S700C>K4L1P007RO00C>K4L1P007RO00C>K4L1P007RO00C>K4L1P007RO07134JB~36Qf0C>K4L1UE=G468c0C>K4L1UE=G"
        "468c0C>K4L1UE=G468c00Y@6FHM5:BH68c7134:D20~3^134:DR0~4_<B4~2g4~26Ia0~3NQG4JLQG4;66<C0~3dA5gHL1P0~5dA5gHL1P0~5dA5gHL1P0~3Y>K9K9_00OQ3JB;;6JMAe00O?;M;G;:FL8O00[<K:Ad0~2XZ=JGIO00aR834:D20fQ64LS^0~3XRA:F9`0~32D:F9`0~5d>MB<C0~4X`D=5KBM;ES^0RD54;TYRQ3HKWC00RD54;TYRQ3HKWC00RD54;TYRQ3HKWC00RD54;TYRQ3HKWC00RD54;TYRQ3HKWC0^SLHM>YUEJB<[0^SLHM>YUEJB<[0^SLHM>YUEJB<[0^SLHM>YUEJB<[00[<BBEAA5HLZXOI65;TROaI65;T2002T545A[2eVDVX02T545A[2eVDVX02T545A[2eVDVX02T545A[2eVDVX7135~3M56Qf02T545A`VIG468c02T545A`VIG468c02T545A`VIG468c00X`D=5KBK53<1P7135:De0~2aR834:D20~4R@;55KH5~2EV]0~3NQG55>QG55JB<C0~3f1LHK<`0~6f1LHK<`0~6f1LHK<`0~4Y>K9K9_0~2[A6KK6E55Ae00OIE3;55;=@C0~22T;F9U0~2aI6FEVX0c1A<"":4;?a0fQ64L11C00NV1A:F9`0~32D:F9_dN00^eY>KB<C0~5XY1@>EL?ZR00c1=JF=AD9M46?200c1=JF=AD9M46?200c1=JF=AD9M46?200c1=JF=AD9M46?200c1=JF"
        "=AD9M46?200U9FF=DAG4:Ad00U9FF=DAG4:Ad00U9FF=DAG4:Ad00U9FF=DAG4:Ad00_9FK>1<KHLZXO8@~2SP0`WG@@I200P1=FJ3>TA931^0P1=FJ3>TA931^0P1=FJ3>TA931^0P1=FJ3>TA931^XV@~3WG@@SP0P1=FJ6>TDG468c0P1=FJ6>TDG468c0P1=FJ6>TDG468c0~2XY1@>W>D8_b0XV@WGTCO0c1A<"":4;?a0~4PS@@D:JE@@TY0~4PS@~21S@@LJ3@Y0~2R_U9F;T20~5R_U9F;T20~5R_U9F;T20~4Y>K9K9_0~27`?>>?@~2I20~2dU8?=L?SYX0~2^ZEF3IO00C>M5AY00NIGK4BEZ^0fQ6433LSX02D33MF9`0~32T5HL>>_00YD>EFKWC0~7dAA>5D20~3CAGJF:5BH5W_]0~2CAGJF:5BH5W_]0~2CAGJF:5BH5W_]0~2CAGJF:5BH5W_]0~2CAGJF:5BH5W_]00dD;HF::JFL8O00dD;HF::JFL8O00dD;HF::JFL8O00dD;HF::JFL8O00CWMH;35:HLZX0a2~2O0XZ=LDW_0~3Y@6FHM5:B31^00Y@6FHM5:B31^00Y@6FHM5:B31^00Y@6FHM5:B31^0N2~2Z=LDW_0~2Y@6FHM5:BH68c00Y@6FHM5:BH68c00Y@6FHM5:BH68c0~4PdA==>Y0~3NZ=LDW_0NIGK4BEZ^0~5O22D:JEV22]0~5O2~2OOV9K6D[X00]Z99GJ=S70~4]Z99GJ=S70~4]Z99GJ=S70~4"
        "Y>K9K9_0~2XZ>~482b0~5cZ=LZ70~5YA;M>[0P1=M=1f00NIGK5=?R00fQ6M~2LSX02D:M~29`0~3N8LFJKEU00CWKFJGT20~7d>:M6?b0~3^_AL:KKM6WZO0~3^_AL:KKM6WZO0~3^_AL:KKM6WZO0~3^_AL:KKM6WZO0~3^_AL:KKM6WZO0~2^U>6MKK5LTC0~2^U>6MKK5LTC0~2^U>6MKK5LTC0~2^U>6MKK5LTC0~2aQ=MK5955LZX0~6]VE::9_0~3X`D=5KBM;ES^00X`D=5KBM;ES^00X`D=5KBM;ES^00X`D=5KBM;ES^0~3]VE::9_0~2X`D=5KBK53<1P00X`D=5KBK53<1P00X`D=5KBK53<1P0~5_9F=1O0~3]VE::9_0NIGK5=?R0~82D:FEV0~=CA;GA`X0~2]Z=FF;AC0~5]Z=FF;AC0~5]Z=FF;AC0~5Y>K9K9_0~2XSLK~2GIN0~6]V99ZX0~5PZA~2`0N8AADC0~2N8>>@1d0~2P1A~4V]02?A~4C0~4CT9LLEU00CWLLEA_X0~7RIA>?`]0~47C8AWWAI_O0~57C8AWWAI_O0~57C8AWWAI_O0~57C8AWWAI_O0~57C8AWWAI_O0~4N_IAWWA8C]0~3N_IAWWA8C]0~3N_IAWWA8C]0~3N_IAWWA8C]0~3d8>WA8@~2U0~8R8AAT[0~4XY1@>W>?ZR0~3XY1@>W>?ZR0~3XY1@>W>?ZR0~3XY1@>W>?ZR0~5R8AAT[0~3XY1@>W>D8_b0~3XY1@>W>D8_b0~3XY1@>W>D8_"
        "b0~5P1LGTd0~5R8AAT[0N8>>@1d0~92T<~2U0~<]V<<A_^0~3]ZEE9AU70~5]ZEE9AU70~5]ZEE9AU70~5Y>6969_0~2XS<~4QN0~72``R0~7Oe~2P002eeR^0~32YYRP0~4be~4O0~22e~470~4]eUS~2e00bZSSVC70~9aRd2X0~7NRCCda0~9NRCCda0~9NRCCda0~9NRCCda0~9NRCCda0~8adCCRN0~7adCCRN0~7adCCRN0~7adCCRN0~6OYCea2~2P0~9Oed2X0~6P2YCYb^0~6P2YCYb^0~6P2YCYb^0~6P2YCYb^0~7Oed2X0~5P2YCYRN0~7P2YCYRN0~7P2YCYRN0~7XUII_X0~6Oed2X002YYRP0~:XC[~2b0~=2[[C70~5eUU_Y70~7eUU_Y70~7eUU_Y70~621Q~3Y0~32[~4e0~xX~20~3X~20~}0~}0~}0~z^OOX0~}0~JPf~30~}0~}0~}0~}0~}0~]7~2X07~2X0~37~2X00^7~2^0~37~2X0~77~300]7~20~2]7~2]^OaP0~HObO0abf0~9PbbabbO0~47~4^0~;abbc0~6Pb~2^0~7cOON^0~6abO0abO0~3X7~9]0~?ObPabO0~DObO0abf0~DPbbabbO0~47~2X007~30~7POON]0~:]^7~3]0~4]fOOf]0~:^77^0~37~2X0~8XNaP^7~20~37~2X0~>PabO^0~87~2^0~8X7~20~5X7~80~9caO]0~G]0~@]~70~8`1~2Z`1~2ZO00`1~2ZOaZ1~2ZO00`1~2ZO0~5"
        "`1~3R^U1~2`]X_1~2UZI?1[^0~6aY~420~7a8?Qd?""?Z70~77V?~41O0~2`1~4SUd^0~7Y?~2Z^0~4PV?~2`]0~4bU8?""?QS_20~3d?""?8C?""?Qb00PV1~9_]00bC~2d0~6YQ?1ITSc0~5aY~420~5a8?Qd?""?Z70~6XRCe70~67V?~41O0~2`1~2ZOR1~3e0~42U1I?8Ve]0~6aCV1~4_X00XdV8?""?8VYP0~7bS11ZO00e1~2UP0~5^YZQ?VZ1~2[00`1~2ZO0~;f[1?""?ISC70~5Y1~320~6NV1~2C0~3PV1~8C0~6P[1?IU20~CfCUZUdX0~;^_Z~7[0~6713~2D13~2D20713~2D2Z<3~2DR0713~2D20~4713~2EVOI3~28PNQ3~2?EG;LA`0~5XZ>~48f0~6b?;<@E31P0~7CA;695=Qa00713~4L9D_X0~5a1=59S70~4[A;;6TY0~328<3;;6L9SX0~2YA;ETL3?200b?3~98P0]VW~2?b0~4O8L;EWGIO0~4XZ>~48f0~4b?;<@E31P0~5X`TWAVf0~5CA;695=Qa00713~2D2`9339`0~3YI<3;;6ET`^0~4e8>EL3~38c0P_DE6;;6EAVb0~5^V933AR00`<33A[0~47_A=668A3~21^713~2D20~:NV>L;;G=AUP0~4UE33<C0~62D33=V0~3b?3~7=V0~5^U>L;691b0~@^`1>E=E@C0~;a?=~71P0~57134:D134:D207134:DVW5H;>_^07134:D20~471344;TRI;46"
        "8PNQG4GLF44J6?20~4XSLK~2GIN0~67Z955;W_0~7bILGA;=Ie0~271344H~2FG@e0~4]U<M9120~4e?3LA6=8a00XV9M4FBFHLSX0~2^UW:3;A[0~2b?5H~244H~268P0^Z=B5T2c0~32D5<6:=S770~37SLK~2GIN70~37Z955;W_770~4e@65;>[0~4bILGA;=Ie7007134:D2`9FF9`0~2RILK4FFHF6DC0~3b8=MJ4H~268c0YAGF4BBJF;<Sa0~4eD;J=S700bI6J3QO0~3CAGJH68A:431^7134:D2a2aP0~6`>5HFKJJ;A[00]ccUEJB<Ccc0~2]c2D:H=VcP00b?5H4~2H~2=V0~5YA;HFJM9VccP0PccP0~82TL56LG3QO0~:^V9693E9;<`X0~57134:D134:D207134:D>;4;>UP007134:D20~47134gB9VI;468PNQG4:B55BgKWC0~42S<~4QN0~62U1D>TVe]0~62?>A>>Id]0~27134:==;HF9_0~3bU8>>QS_20~3YA>?""?D>?200OIG4;9<93LSX0~2bU8>~2S_200b?L~3JBL~38P^V8=H5T88[00`1?>I?>I1~2e00`11<~4Q11e00`1~2D>T1~3e0~3_96DL=ZX0~2`1?>A>>I1~2e07134:D2`9FF9`00^VEBHG993FJL8O00]U<K4:3L~38c0Y>K;B9<E54:<U^0~2]VEFKWC0~2]VEJM>C0~2P1=JJ3<1A:431^7134:DS?T?1[N0~3O83HMEWLFJL1c0`8~2EJB<"
        "8~2Y00`88D:H=881b0b?LL34:L~2=V0~4PS=J;<LJ;T881R18~2R0~72TL<==A6T20~9bS1D<_WAZ<I1UP0~47134:D134:D207134:A;H5<VN0~27134:D20~47134MBG?I;468PNQG4JLDTEFJ9U0~3YI<3;;6ET`^0~4YI<3;;6ET`^0~4YI<3;;6ET`^007134:DZ<BJEU0~228<3;;6L9SX0028<3;;6L9SX02T5JEV[U8>ZX0028<3;;6L9SX07US~2EJB<S~2[0OI66H:66=Z]713~2D2`9339`0713~2D[`9339`0713~2Dd`9339`0~3_96@L=ZX00713~2Dd`9339`07134:D2`9FF9`002?;4:"">U`TG4:"">Y00aI64M<8S~3`]0Y>93B<[Q9F4;TR0~2OI64;?2b~2aC>MJ=ZX002T;g5A`RA:431^7134:DE;5;L>Za0~2dA55L1C@;4;?bXSL6~2JF6~29_0XSL66MH6~2Ae07US134:DS~2Ya0~32T;H=ZWKB<66<_966WC0~3OU~2_S1<E318S[U~220~4^V933AR[C`<33A[0~47134:D134:D207134:6H:<ZO0~37134:D20~47134;;B9I;468PNQG4:ACd>MJ=V0~2RILK4FFHF6DC0~3RILK4FFHF6DC0~3RILK4FFHF6DC007134:DV<BF9_00XV9M4FBFHLSX0XV9M4FBFHLSX0b?;4;E>T1Ca00XV9M4FBFHLSX0~2XXUEJB<CXX00OIGB4FBB=Z]7134:D2`9FF"
        "9`07134:D2`9FF9`07134:D2`9FF9`00^ceD65;>[P007134:D2`9FF9`07134:D2`9FF9`00Y>M438NXVEJF9_00dAM46QaX~30~2RI13B<I=K:KB9U0~2eA:431[?~2IRT546IO00Y>K431PRA:431^7134:=BFFHH5<V700R?~3[f8645T2XSLH~2g4HHJ9_0XSLBBJ4BBMAe0~2^134:W~3>QY0~2C>KJEUAMH=;HL1LHG?b0~2]_WEEARC<3EVcO?EE9ZP0~3eD;J=S700bI6J3QO0~37134:D134:D207134HHM91a0~47134:D20~47134;9F6?;468PNQG4;IORA:H=V00^VEBHG993FJL8O00^VEBHG993FJL8O00^VEBHG993FJL8O07134:==;JGDe00OIG4;9<93LSX0OIG4;9<93LSX0P1=F4FK5LA_X0OIG4;9<93LSX0~4UEJB<C0~3NQW=H5W~2V]7134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0PV8~2TWA88SO07134:D2`9FF9`07134:D2`9FF9`00C<BH=Z]0[<BJEV00C<BH=Z]0~7O13B<9K:<G438f00CWBH=ZUE55>YI;45T200C<BH=V]RA:431^7134KML993B4;Ad0~2ab~2XeT;46IOXSLB==JB==J9_0XSLLW:H=WMAe0~2^134JB~3M=Ib00[<FJEUAM43=J;T;JEZX0~22?6J=1NC<9IR0XU<B5AY0~2]VEFKWC0~2]VEJM>C0~37134:D134:D2"
        "07134HHF=Ie0~47134:D20~47134;@;K<;468PNQG4GQN2D:H=V002?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y071344HHJ=A_^002T5JEV[U8>ZX02T5JEV[U8>ZX00Y?=5KJ4J6?202T5JEV[U8>ZX0~4UEJB<C0~4RZ=H5TCCb07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0a?6631UE66DR07134:D2`9FF9`07134:D2`9FF9`00C<BH=V]0[<BJ=V00C<BH=V]0~7^1EE<646QL4;T200[<BH=ZUEJK>YIG4:D200C<BH=V0RA:431^7134J=Q`_ILJFEU0~628=FB9VXXS==WEJB<E=9_0XVAAD:H=TATR0~2^134B:~2BgKWC00[<FJEUAM43>MK<KM>C0~3`<KKWC0aU_R0~2RT;J=Z^00OI64;?2b~2aC>MJEZX0~27134:D134:D207134:6HB=QR0~37134:D20~47134;?EF6;468PNQG4GQN2D:H=V00Y>M438NXVEJF9_00Y>M438NXVEJF9_00Y>M438NXVEJF9_07134:=3BF3Ib00b?;4;E>T1Ca00b?;4;E>T1Ca0~2Rd1D<=;4KWC0b?;4;E>T1Ca0~5UEJB<C0~4]Z=H5T20~27134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0a?;431UEJ:DR07134:D2`9FF9`07134:DR`9FF9`00C>KHL170_9FJEU00C>K4L1"
        "P0~8dUU?;431=H:AR00CWBHLSVEJKWC?;g5T200CWKHLS^Y>M431^7134M>Y00C>MHLS^0~4219MJ3?R00YV~2EJB<V~2R00OeeD:H=VeR]0~2^134:D~2EJJEU00CWBJEUAMHL?GJLJGIa0~2P1LH;?20~7P1LH;?b00eA:431[?~2IRT546IO0~27134:D134:D207134:A;HKE120~27134:D[~3R07134;?D;K;468PNQG4GQN2D:H=V00C<BH=Z]0[<BJEV00C<BH=Z]0[<BJEV00C<BH=Z]0[<BJEV07134:DILHK<`00P1=F4FK5LA_X0P1=F4FK5LA_X0OIDZC[Z<FF<[0P1=F4FK5LA_X0~4UEJB<C0~4]Z=H5T22X07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0a?;4L1UEJ:DR07134:D2`9FF9`0^SLHM>YUEJB<[00RD54;TYRQ3HKWC002T545A[2P0~8O?;431EJMAe00d>M46Q1LJF9UD:4;?a00RA:4G?RV9F431^7134:DR00RD:4LS^0~3219MHGA[0~5UEJB<C0~62D:H=V0~5^134:DRC<BJEV00RA:J=V>MF9ZEJHJEVX0~22T;431P0~8UEJM>Y00CWBH=ZUE55>YI;45T20~27134:D134:D207134:D>;4M9Sa007134:<~48f7134;?SEF4468PNQG4GQN2D:H=V00C<BH=V]0[<BJ=V00C<BH=V]0[<BJ=V00C<BH=V]0[<BJ=V07134:D[WMH38O00"
        "Y?=5KJ4J6?200Y?=5KJ4J6?20a?;E>TD=JKWC00Y?=5KJ4J6?20~4UEJB<C0~4]Z=H:ADTC07134:D2`9FF9`07134:D2`9FF9`07134:D2`9FF9`0a?;4LSUEJ:DR07134:D2`9FF9`00U9FF=DAG4:Ad00c1=JF=AD9M4G?200P1=FJ3>TS20~7O?;431EJM>d00b?;4M<W5GM6AEF438P00OQ3HBE@>;J431^7134:D200Y>MJ=V]0~2e89MH;>_^0~5UEJB<C0~62D:H=V0~5^134:DRCWBJEV00O83H6DEF:AC>MgM>C0~3dAMJEV0~9C<BF9`00[<BH=ZUEJK>YIG4:D20~27134:D134:D207134:DV<54:<ZO07134JB~36Qf7134;?YA54468PNQG4GQN2D:H=V00C>KHL170_9FJEU00C>KHL170_9FJEU00C>KHL170_9FJEU07134:D2I34M>C00Rd1D<=;4KWC00Rd1D<=;4KWC0a?;JM55FHG?200Rd1D<=;4KWC0~4UEJB<C0~5_9FJ::9`07134:DR`9FF9`07134:DR`9FF9`07134:DR`9FF9`0a?;4LSUEJ:DR07134:DR`9FF9`00dD;HF::JFL8O0~2CAGJF:5BH5WU]0~2Y@6FHM591N0~6O?;431=H:AR00^ZEB4KMFE3JMB4FEU0~3_<M4B5M6:431^7134:D2021EFB<`00^C?=KJGAU2Z_X0~3UEJB<C0~62D:H=V0~5^134:DRCWBJEV0~2`W:F5KF=1aI646IO0"
        "~3Y>KJ9_0cd_VV_YO0Y>KJ9_00CWBHLSVEJKWC?;g5T20~271355D8645D2071355D2Z<5~2WU]7135~56Qf7135;?aS=5568PNQG5GQRYAMJEV00RD54;TYRQ3HKWC00RD54;TYRQ3HKWC00RD54;TYRQ3HKWC071355D2_<55=1POIDZC[Z<FF<[0OIDZC[Z<FF<[0a?=;MKBM6>UX0OIDZC[Z<FF<[0~4UE:5<C0~5RTL5~29`0^SLHM>YUEJB<[0^SLHM>YUEJB<[0^SLHM>YUEJB<[0a?;431S=H:DR0^SLHM>YUEJB<[00^U>6MFK5LTC0~3^_AL5J4:3WZO0~3X`@=;5M:Ae0~6O?;531L4;?a0~2R?=:BML?A6KJ4;De0~3a195KM3A:431^71355D2CQ9:4GTR00[A3BF3A_U?9IO0~3UEJB<C0~62D:H=V0~5^1355DR[<BJEU0~2O195KMLTY2?;FEV]0~3Y>KJEUC1A9EE9>8`CWKF9_00d>M46Q1LJF9UA:4;?a0~2XV@~2A<"":4G?a0XV@~2IUV1@~3U0XV@~7SPXV@~21VVT@~2Z^PS@~2STA=FF9_00c1=JF=AD9M46?200c1=JF=AD9M46?200c1=JF=AD9M46?20XV@~2IVVI@~2SPa?;E>TD=JKWC0a?;E>TD=JKWC0]CZ?AWELI_f00a?;E>TD=JKWC0~37`@EL?Y0~57C8EL@TC00U9FF=DAG4:Ad00U9FF=DAG4:Ad00U9FF=DAG4:Ad0OIG4GDA;4:DR00U9FF=DAG4"
        ":Ad0~2N_IWGWA8C]0~47C1<BH=I_O0~5XYS?@9B<C0~6N1@~2IGH=S70~3Y8AW>QCZD=K5WU^0~42SDW>?A:431^XV@~2I8>LKH5WUX00[A;HBL>>96;IO0~3UEJB<C0~62D:H=V0~5XV@~2IDA3HB<`0~32SDW>QC__WK:AY0~4RD546A>=5F~3M3<A=JKWC00b?;4M<W5GM6AEFHL170~3N22IGK4BEZ^00N2~2dA==>Y22P00N2dA==>Y2O00N22dA==>Y22N00O2~2Y>MF45Ad0~2CAGJF:5BH5W_]0~2CAGJF:5BH5W_]0~2CAGJF:5BH5W_]00N22dA==>Y22O0a?;JM55FHG?20a?;JM55FHG?20~2^dAA>5D20~2a?;JM55FHG?20~3dAA>5D20~5dAA>5D2X00dD;HF::JFL8O00dD;HF::JFL8O00dD;HF::JFL8O071=JF556H:DR00dD;HF::JFL8O0~3XZ=LDW_0~8C<BH=V0~9^d>LKWC0~7O22C>MM>[0~5OdCZ?>=;B6>VO0~6PRCYRA:431^0N22IE3:JB6>VO0~2PU>;JJKKFH;IO0~3UEJB<C0~62D:H=V0~6f2~2`9MJHGTR0~4PRCYa_996H3QO0~4f1=FFMKJ5L9<=;FKMBH6?200^ZEB4KMFE3JMB4MW`0~6OIGK5EIR0~6_9F=1O0~6_9F=1O0~6_9F=1O0~8Y>KMG<ZP0~2^_AL:KKM6WZO0~3^_AL:KKM6WZO0~3^_AL:KKM6WZO0~5_9F=1O0~3a?=;MKKM6>UX0"
        "a?=;MKKM6>UX0~3d>:M6?b0~2a?=;MKKM6>UX0~3d>:M6?b0~5d>:M6?b0~2^U>6MKK5LTC0~2^U>6MKK5LTC0~2^U>6MKK5LTC0~2CAGKK3E55DR00^U>6MKK5LTC0~4]VE::9_0~8C<BF=V0~:d>K3?20~928LF=8O0~7c1=MM69?`N0~;RA:F31^0~2^ZEB539?`f0~4fUA3MFB:L>ZP0~3UEFB<C0~62D:F=V0~:`9K:6>UX0~9_9FJ5W`0~6C@3MB5=A1``ZDE;KM6>_]0~2R?=:BML?A6KK591O0~6N8>>@1e0~6P1LGTd0~6P1LGTd0~6P1LGTd0~9dA>A?Ua0~47C8AWWAI_O0~57C8AWWAI_O0~57C8AWWAI_O0~5P1LGTd0~4]CZ?AWWAI_f00]CZ?AWWAI_f0~4RIA>?`]0~2]CZ?AWWAI_f0~4RIA>?`]0~5RIA>?`]0~3N_IAWWA8C]0~3N_IAWWA8C]0~3N_IAWWA8C]0~2P_TW>I@~2I20~2N_IAWWA8C]0~5R8AAT[0~8C><~2V0~:dA<?[0~:Y><<DC0~9YD9>IU2]0~<RD<~2S^0~3YA<DQU20~7P[I><<A8[f0~4U<~2>C0~62T<~2V0~:[>>AI_c0~:_9E9>ZO0~6XCIAWA1Cf00^eZ@W>?_c0~4Y8AW>QC`?>WDS20~8RYY2P0~7XUII_X0~6XUII_X0~6XUII_X0~9^YYdbX0~7NRCCda0~9NRCCda0~9NRCCda0~7XUII_X0~6^aeCCda]0~4^aeCCda]0~6aRd2X0~5"
        "^aeCCda]0~6aRd2X0~7aRd2X0~6adCCRN0~7adCCRN0~7adCCRN0~5X2CYb2~2b0~5adCCRN0~8Oed2X0~8N[~320~:PC[R]0~:P[~2CX0~9^[`Ya]0~>^C[~2R0~4P``RO0~;aY[[eO0~6a[~3N0~6XC[~220~:cYYea0~<2U~2Cf0~9adCeN0~57RCY2]0~6OdCYO0]2YCRP0~G^OOX0~8^OOX0~8^OOX0~x^OOX0~}0~}0~}0~}0~}0~87OaO^0~47~7]0~5]POaOP0~67~90~>7OaO^0~47~2X00^7~2^0~5]7~70~67~2X00X7~20~37~2X00X7~20~67~3]0~27~3X0~7NbbObb70~7NbbObb70~8NbbObb70~8NbbObb70~6NbbObb70~67~7^0~37~2^0~2^7~20~4POaOP0~:]7~70~5]7~70~:7~2X00X7~20~=XNaOP0~I7~4^0~}0~4POON]0~DObbO0~8ObbO0~:a`1I?IZCN00`1~7_X0~3YV1I?I1U20~3`1~9`]0~:a`1I?IZCN00`1~2ZOaZ1~2ZO0~3^U1~7[0~4`1~2ZOfV1~2`]0`1~2ZOfV1~2`]0~3`1~3_X0Y1~3VN0~5PS?""?Q?""?U0~6PS?""?Q?""?U0~7PS?""?Q?""?U0~7PS?""?Q?""?U0~5PS?""?Q?""?U0~521~7Sb00C1~320bS1~2`00RU1I?I1`a0~7^U1~7[0~3^U1~7[0~8`1~2ZOfV1~2`]0~:RVQ?I1_b0~=7dY20~4`1~4SUd^0~82YYP002"
        "Y~220~:2Y~220~7NC[~6Y0~6PRCCY2^0~82U1I?QVeX0~6]dYd]0~5O1?""?820~6O1?""?820~8R8<LG;6=>VX713~7Qf0~2OIE3G;G39Id00713~98P0~9R8<LG;6=>VX713~2D2Z<3~2DR0~3OI3~6LS^0~2713~2D22T3~28P713~2D22T3~28P0~2713~3?b^Z=3~2@20~5OIGGA5LSX0~5OIGGA5LSX0~6OIGGA5LSX0~6OIGGA5LSX0~4OIGGA5LSX0~4C<3~6>Y0XZE339V7U<33=1^XZ93G;GL<SO0~6OI3~6LS^0~2OI3~6LS^0~6713~2D22T3~28P0~9eIE6;GL<Qd0~<CA>QO0~2713~4L9D_X0~6OQ>>[0c1>~2170~8c1>~2170~6C><~6?a0~3XY1DW<>?VR0~6YI<3;;6ET`^0~5RT>TR0~5aI=59VP0~5aI=59VP0~62QEMHJBFJ31^71344H~36Qf0~2OIGFKKB4B=1f071344H~34468P0~82QEMHJBFJ31^7134:DVW5H;>_^0~3OI;44HH44LS^0~27134:D22T5468P7134:D22T5468P0~271344KW[2T;445@20~5OIGGA5LSX0~5OIGGA5LSX0~6OIGGA5LSX0~6OIGGA5LSX0~4OIGGA5LSX0~4C<B4H~3K>Y00eTG4;AYTG4;DY0XZLFKKJH:W`0~6OI;44HH44LS^0~2OI;44HH44LS^0~67134:D22T5468P0~8c1EB;FG:K=I20~9PYV<;I[O0071344H~2F"
        "G@e0~3bC~2A5>C0NQGK68P0~8NQGK68P0~6[<L~5;?a0~27_D=5KKM;9IY0~4RILK4FFHF6AC0~5RAMAR0~6dI=;AC0~6dI=;AC0~5XV9K45E<9331^7134:L~4Qf0~2OI39W>95g5T207134:L~35468P0~7XV9K45E<9331^7134:D>;4;>UP0~4OI;4;LLM4LS^0~27134:D22T5468P7134:D22T5468P0~27134HHL1`<KJg5@20~521@@?@@U20~5Y1@@?@@U20~621@@?@@U]0~621@@?@@U]0~421@@?@@U^0~4C<BJL~4>Y00XU<MJ=1EFK9V^0XZL9>W=FH=ZX0~5OI;4;LLM4LS^0~2OI;4;LLM4LS^0~67134:D22T5468P0~82@3~2K>>;K9VX0~7aZAE3;=>1a07134:==;HF9_0~2]VW~2<;?20NQG468a2O0~6NQG468Pc~30~2[<=S~3?;?a00]`AG:=<W93KLIR0~2^VEBHG993FJL8O0~2^YSAMA8UO0~4OZI>>S20~6`I>>ZO0~4b?G4M<Z[_I91^7134:DS~3`]0~27V8_CY1345D207134:DSST5468C~2ec0~3b?G4M<Z[_I91^7134:A;H5<VN0~5OI;4;?AM4LS^0~27134:DCCT5468P7134:DCCT5468P0~27134;:5D83F=H5@20~4R?<LG;G3<IC]0~2OIE3G;G39Id0~4YI<3;;6ET`^0~4YI<3;;6ET`^0~22?93;;6EDUf0~2aC<BJES~3Za0~2bQLJMW:H"
        "3I20~2`IUCCAMHLSX0~5OI;4;?AM4LS^0~2OI;4;?AM4LSC~2RP0~27134:DCCT5468C~2ec0~4P_8D;5DU<K6Ia0~6fZ<5J~3:DR07134:DZ<BJEU0~2^Z=B5=L1P0NQG468?TI_P0~4NQG468Y8~3[00[<=VXXa?;?a00R?6;>1~2VQ9MEZ7002?;4:"">U`TG4M>Y00X`D=;B;3>Y0~4YA3~2<[0~5713~2D20~4Y>M43Qa00b_e07134:W~3>Qd0~37Y?@DW;J3QO07134:D22T546W~3ASb0~2Y>M46Qa00b_e07134:6HM<>Qd0~5a?;4;IAM4LS^0~27134:W~35468P7134:W~35468P0~27134;3B9A:5=H5@20~4d>KHFBJHKLTY0~2OIGFKKB4B=1f0~2RILK4FFHF6DC0~3RILK4FFHF6DC0~2RAMJFFHF;>UX0]V><BJE>~2VX0~4CA5H;H:"">[0~3Xd?@DEBK9_0~6OI;4;IAM4LS^0~2OI;4;IAM4LW~3DZO007134:W~3546W~3ASb0~5RA:GI2?;:ARaR2^0~2C>54:=;=5DR07134:DV<BF9_00^V8=H5G98[0NQG469G56>`0~4NQG468IL669V00[<=V00a?;?a00U9MW1<3L<=>=;TR00Y>M438NXVEJF9_00d@6JJB:K>Y0~3]VEF4H=S70~47134:D20~4C<BH=Z]0~57134JB~3M=Qa0~3C<5:KK=D[007134:D22T5gB~3K59V^00C<BH=Z]0~57134HHgFBM=Qa0"
        "~42T546QAM4LS^0~27134JB~3F468P7134JB~3F468P0~27134;9B6EFL=H5@20~4d>5L9<E;HF=1N00OI39W>95g5T200^VEBHG993FJL8O00^VEBHG993FJL8O00RA5=<9LBHG@e0XSLKH4K~2=Z]0~4PZEBgF=1N0~4CW5:BG9Q20~6a?;4GIAM4LS^0~2a?;4GIAM4B~3K5<U]07134JB~3FgB~3K59V^0~4CWKLSNQGK>CID?VO071LJB9<;I>DR07134:==;JGDe00OI66H:M6=Z]NQG46MMJH3IO0~3NQG46Q=BM91200[<=V00a?;?a0NQ338W:;63K>>MWC00C<BH=Z]0[<BJEV0]VEFFLMA9>Y0~3OI64MH;T20~47134:D20~4C<BH=V]0~57134B:~2BgMWC0~3C<55MJM=8O07134:D22T54M:~2J4;?200C<BH=V]0~57134B:~2BgMWC0~4RA:431AM4LS^0~27134B:~3B468P7134B:~3B468P0~27134;@;MGK<=H5@20~4d>A1_[VA;4:Ad007V8_CY1345D2002?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y00RAAS``ILJF9U0XSLMJ4M~2=Z]0~47V9BgFESc0~4CW55KF;W`0~62T5468AM4LS^0~22T5468AM4:~3J46Ia07134B:~3B4M:~2J4;?20~4UEF9UOIGMAT3:;<U]a?;4;?<;I_Uc071344HHJ=A_^00OIGB44BB=Z]NQG4BE>3H:DR0~3NQG46EKM"
        "9120~2[<=V00a?;?a02T591L:"">ILK>T59_00C<BH=V00[<BH=V0P1345AMAZ120~3YA:FEBK<[0~47134:D20~4C>K43Qa0~57134:D~2=JJEU0~2^d?TT>64:"">Y07134:D22T546DDA64M>d00C>K4L1P007RO07134:D~2=JJEU0~4CWKJ=VAM4LS^0~27134:D~35468P7134:D~35468P0~27134;?=JJ;?=H5@20~3aUV~5ZEJF9_0~27Y?@DW;J3QO00Y>M438NXVEJF9_00Y>M43>~3EJF9_00P[ST~3>M431P]V><BJE>~2V]0~4CA;4:H5>[0~4d?TD9MH38f0~5RA:4L1AM4LS^0~2RA:4L1AM4LDDA;4:AR07134:D~3546DDA64M>d0~22`1LKWCRD55T=K=;6IO2T5468<;IO0~27134:=3BF3Ib00NQW=HH=WWV]NQG4;?UEJMAe0~3NQG46MKE820~3[<=V00a?;?a0RD:<86;IUEK>@59`00CWKHLS70_9FJ9U0fQ64;AMARc0~3]VEFM>5H=S70~37134:D20~42T;4M<Z[~3a07134:DR[<BJEU00OZVeb2UEJB<C07134:D22T5468RZ=HK>Y002T545A[2eVDVX7134:DR[<BJEU0~2PR1=JB<`AM4LS[[O07134:DRRT5468P7134:DRRT5468[[a07134;?>MJ=Z=H5@[[R00C<E~7FJEV0~3C<5:KK=D[0~2C<BH=Z]0[<BJEV00C<B4K~54JEV0~2NQG5~3B4GI"
        "O0OC<BJEUddO0~4a8=JM<"":JLIb0~4bb2I;4;?[[200O[Z9BJEVAM4LS^0O[Z9BJEVAM4LSR134MAR07134:DRRT5468RZ=HK>Y00RI<9G5DR_<B3T;3Q=5TbbT54GI<;I22007134:DILHK<`0~2RZ=HB<`Cb0NQG468_9JMAe0~3NQG4F4GA[]0~3[<=V00a?;?a02D:<86;?VEK><M>C00RD54;TYRQ3HKWC0P834;AMA`UO0~2OI64GI3H;T20~37134:D20~47SEF45E<~3_07134:D2CWBJEU00RA9A?ID3HKWC07134:D22T5468PV=HK>Y00P1=FJ3>TA931^7134:D2CWBJEU00]UD9:45DRAM4L<<W[07134:D22T5468P7134:D22T546<~2_07134;?I66>Z=H5<~2170CWBJ~64J=V0~3C<55MJM=8O00C<BH=V]0[<BJ=V00C<BHG~5FJ=V0~2NQGF~3J4;IO00C<BJEDD8O0~3]_WMJLQ=FK<UX0~5OI;4;<~2Z]0[WE54KWCAM4LS^0[WE54KWCAM4LS^SL4MAR07134:D22T5468PV=HK>Y0]VEKMF6TU?6B9WKES=;?aOQ64:""><;II?207134:D[WMH38O00]Z=H5DR0~2NQG468_9JMAeP]00NQG465J3TCP^00[<=V00a?;?a0a?;EZEK9>;KWG3Ib00c1=JF=AD9MH6?b0]VEJB9MA>>Y0~2YA:J=Z9FK<[0~37134:D20~5dT3F4JB~2EU07134:D2CWBJEU00RA::"
        ";;5JJ6?207134:D22T5468PV=HK>Y0~2Y@6FHM5:B31^7134:D2CWBJEU00XSLB4B=1RAMgFBB9`07134:D22T5468P7134:D22T5gFBBEU07134;?_88SZ=HJBB3170Y>M4L=~4JJEU0~2^d?TT>64:"">Y00C>KHL170_9FJEU00C>KHLI~39FJEU0~228<~4M46Qf00C<BH::5Tb0~3R?6H5ACD;4;@d0~5OI;4FBB=Z]0`9FHKLIRAM4LS^0`9FHKLIRAM4LS^SL4MAR07134:D22T5468PV=HK>Y0713G9FFGE3F3?9F3<5L1P]V9BJ3<;D=AR07134:D2I34M>C00]Z=H5T22X0NQG468_9JMA88`00NQG469MFL?8V70[<=V00a?;?a071=;TD6BK=BM3A[0~3CAGJF:5BH5W_]00YA;HKMG:"">Y00]VEFK<~2:H=S70~27134:D20~5][?E655JJEU071355DR`<BJEU00RAG:KBK53A_]071355D22T5568RSLHK>Y0~2X`D=5KBM;ES^71355DR`<BJEU00XSLM;EIeRA5~2JF9`071355D2eD:431P71355D22T5~2JJEU07135;?aPP7Z=55K431702T;4;TVVQ3HKWC00OZVeb2UEJB<C00RD54;TYRQ3HKWC00RD54;TYRQ3HKWC002?I[~21EFJ=ZX00C<5~245Tb0~2^ZE55EZ2V9KF=1P0~4OI;55FH=Z]0`9;L<IYY>MHLSX0`9;L<IYRA55LSR134:AR071355D22T5568R"
        "SLHK>Y0]U9:B5E5KM3DSL5;KGAC00R?3FJ5~2:AR0^134MAd_<55=1P0]Z=H:ADTC0NQG468_9JK6631^0NQG468EBB66IO0[<=V00a?;?a00[>:=QT<<W<>I[^0~3^_AL5KKH3>ZO0~27U>3MJK5>Y00OI64HF~2J4;T20~27134:D20~6]RVIT<BJEU0XV@~2IA>3HB<`00OVIA>ELD8[700XV@~2IbaQ@~2ZAW545DR0~3XY1@>EL?ZR0XV@~2IA>3HB<`00XZWA?ZR02I@@3J6DC0XV@~2I?A=FJ=ZXXV@~2IbaQ@@LJGA[0XV@~21N0~2U@@9K5<Z^0P1=JF=AD9M46?200RA9A?ID3HKWC00c1=JF=AD9M46?200c1=JF=AD9M46?200RALW@D9:4:"">C0~2YT@@LH;?b0~2^Z@~2?RIA9K438c0~4c1@@>MH=Z]0CT?1_dTA=JJEV00CT?1_202I@~21A<"":4;?b0XV@~2IbaQ@~2ZAW545DR00b1AWASDW>I[IG6AWTU70~2CTE5KB:3DR0]VEJF=ATI@~2SP00_9FJ::9`0NQG568_955M431^0NQG568IL:4;IO0[<=VR~2?;?a00O1EK=AQ1?<;@d0~57C8D>9K:<VP0~3f`IAMW@1200YA:H;~4FK<[0~27134:D20~8XOC<BJEU00N2~2`9MJHGTR0~2XdAA>5D20~4N2~2b00a2~213KHF=1f0~5dAA>5D20~2N2~2`9MJHGTR0~2bCdb^0~2b1=B3D[^00N2~2eAMF4:"">[00"
        "N2~2b00a1EK6A_700N2~2O0~3PUW:5<Za0~2CAGJF:5BH5W_]00RA::;;5JJ6?20~2CAGJF:5BH5W_]0~2CAGJF:5BH5W_]00RAMK55B4M9SN0~2]CW9;46QO0~3N2~2bbT5B4K<U0~6O2d>MH=Z]0X2aP0CWMF45AY00X2aP0~3b2~286K4BES700N2~2b00a2~213KHF=1f0~2NdCePRCYaRD5=ZCRX0~3]dS@<;A8[^00C>54FM>d2~2O0~2[<5~39`0PS@~2Z[@~2;431^0PS@~2ZY?34;IO0[<=A~4;?a0~2RQE:563;K6>[0~7NRYQ=F;>`0~5RAMARP00]VEFB<?~2>MH=S7007134:D20~:C<BFEU0~5`9K:3>UX0~3d>:M6?b0~>^13K5=TY0~6d>:M6?b0~6`9K:3>UX0~97V<M3@[X0~6eAMM;9SO0~7XUW:6A`^0~;CDG5WZO0~37_AL5KK:6WZO0~2RAG:KBK53A_]0~2^_AL:KKM6WZO0~3^_AL:KKM6WZO0~2RAL5KKMG<120~4C<BJ:<U0~9bT5M;9820~8d>MF=Z]0~5CWKMG<Zc0~<c86K5=?e0~<^13K5=TY0~<CWK9_0~8P[<;IO0~3fZ<GMK>d0~5XZ=3T@@TC00O2~2N^22?;K31^00O2~2N713K;IO0[<G~6?a0~3R1>=36L9?_f0~:d?9~2170~4RAEAR0~2OI65;TROaI65;T20071355D20~:CW<~2_0~5`>>AI`c0~4RIA>?`]0~>^S>>D1Y]0~6RIA>?`]0~6`>"
        ">AI`c0~:OI<<TCX0~7RD>>?Vb0~8NQ<<D`^0~<V<<WVO0~57C8DWWAI_O0~3OVIA>W>D8[70~47C8AWWAI_O0~57C8AWWAI_O0~37[8D>WA?Ub0~5C<EE>Sa0~9a?>>TZ20~9dA<~2Z]0~5YA>A?UO0~=P1>>D1d0~=^S>>D1Y]0~<`>E>C0~9[W9IO0~4aU?>>De0~5NIE9U2~2X0~8OIW~2S^0~6^SW~2QO0eQI~6Sf0~4NCZ8Q1_R]0~<Y`~2R0~5P`U`P0~2O8@~2SP0XV@~2I200XV@~2Ib0~:N[~3a0~5fYYea0~7aRd2X0~@2YYRN0~9aRd2X0~7fYYea0~=Y[[Y]0~8XdYdbX0~:d[[C^0~=2[~2O0~8NRCCda0~6XaeCCYRO0~8NRCCda0~9NRCCda0~7NRCCda]0~6aU~2CN0~;RYY270~:7C[~220~6^YYdbX0~?2YYRc0~?2YYRN0~>NCUVO0~9O``Y0~6XbdYd^0~6CUU20~=RC~220~82C~2R0~2O~7P0~7XPf70~Sa2~2O0~2f2~2b0~3N2~2b0~}0~}0~}0~}0~a]0~}0~}0~;abbc0~9Pb~2^0~:ObPabO0~8NbbObb70~:POc0~8ObbO0~:abbc0~5Pb~2^0~5NbbObb70~5abbc0~3Pb~2^0~3NbbObb70~4ObPabO0~;abbc0~H]]00]0~8abO0abO0~BObO0abf0~57b~2]0~6abO0abO0~6Pb~2^0~6Pb~2^0~9ObPabO0~:ObO0abf007b~2]0~5abbc0~6abbc0~6ab"
        "bc0~7abO0abO0~8abbc0~6abO0abO0~6abO0abO0~;Pb~2^0~<Pb~2^0~9NbbObb70~:abbc0~3NbbObb70~7Y?~2Z^0~7PV?~2`]0~8YQ?1ITSc0~6PS?""?Q?""?U0~9e1?1C0~6O1?""?820~8Y?~2Z^0~3PV?~2`]0~3PS?""?Q?""?U0~4Y?~2Z^00PV?~2`]00PS?""?Q?""?U0~3YQ?1ITSc0~9Y?~2Z^0~6aY~420~7][ZVbUZ_70~5d?""?8C?""?Qb0~5aY~420~3a8?Qd?""?Z70~4_?~2C0~5d?""?8C?""?Qb0~4PV?~2`]0~4PV?~2`]0~7YQ?1ITSc0aY~420a8?Qd?""?Z70_?~2C0~4Y?~2Z^0~4Y?~2Z^0~4Y?~2Z^0~5d?""?8C?""?Qb0~6Y?~2Z^0~4d?""?8C?""?Qb0~4d?""?8C?""?Qb0~9PV?~2`]0~:PV?~2`]0~7PS?""?Q?""?U0~9Y?~2Z^00PS?""?Q?""?U0~6a1=59S70~7[A;;6TY0~7O8L;EWGIO0~6OIGGA5LSX0~72I=;L?e0~5aI=59VP0~6a1=59S70~3[A;;6TY0~3OIGGA5LSX0~2a1=59S700[A;;6TY00OIGGA5LSX00O8L;EWGIO0~8a1=59S70~5XZ>~48f0~6fQ=<UA=T20~5YA;ETL3?20~4XZ>~48f0~2b?;<@E31P0~3]Z=59`0~5YA;ETL3?20~4[A;;6TY0~4[A;;6TY0~6O8L;EWGIOXZ>~48fb?;<@E31P]Z=59`0~3a1=59S7"
        "0~3a1=59S70~3a1=59S70~5YA;ETL3?20~5a1=59S70~4YA;ETL3?20~4YA;ETL3?20~9[A;;6TY0~:[A;;6TY0~7OIGGA5LSX0~7a1=59S700OIGGA5LSX0~4]U<M9120~7e?3LA6=8a0~62D5<6:=S70~6OIGGA5LSX0~7`<;9GEU0~6dI=;AC770~37U<M91270~2e?3LA6=8a0~2OIGGA5LS700]U<M91200e?3LA6=8a0OIGGA5LSX002D5<6:=S770~3^7~2U<M912770~4XSLK~2GIN0~6^S=6E36IO0~57UW:3;A[0~5XSLK~2GIN0~27Z955;W_70~37Z=59`770~37UW:3;A[0~37e?3LA6=8a0~2e?3LA6=8a0~52D5<6:=S7XSLK~2GIN7Z955;W_0]Z=59`0~2]U<M9120~3]U<M9120~277U<M912770~377UW:3;A[70~377U<M9120~57UW:3;A[0~4X7UW:3;A[77]0~27~2Xe?3LA6=8a7~20~3^7e?3LA6=8a70~3^7OIGGA5LS770~3^77U<M912700OIGGA5LSX0~4OZ>>1S20~7YA>?1D>?20~62?>I?>Id0~7c1@@I@@U]0~7U931==Z]0~4`11I>>1~2[00`11>>1~3[00`A>?1D>?1[00`1@@I@@11[0XZ>>1ZO00YA>?1D>?20c1@@I@@U]0`1?>I?>I1~2`]0bS1~2Z>>1~4C0~3XS<~4QN0~7C@=6L>_0~5`1~2>~2Ue70~4`1<~4Q1[00`11D>T1~2[00`11@@"
        "T1~2[00`1~2>~211[00`11A>?`D>?1`]0YA>~4?20~52?>I?>Id0XS<~4QN021D>TZO00U@@TZO00`1>>120~4c1>~21700`1~2>>1U1~2`]0`1~3>~21~2`]0`1~2>>1Ud^0~3`1~2>~2Ud^0~2PV1~2>~21~3_]0C1~2ZYA>?1D>?S1~2d00bS11A>?`D>?11C00bS1~2@@I@@1~2C002S1~2>>1~3[0c1@@I@@U]0~4YA3~2<[0~7^YA3~2<[0~8YA3~2<[0~8YA3~2<[0~8C>ML5<_0~4713~6LSX713~6LSX713~6LSX713~6LSX713~2D200^e13~2DR0~2O13~2D20713~2EVdI3~28P0dA3~2DddSE33EZ]0~3YA3~2<[0~8YA3~2<[0~4713~3L=9DVa0~2713~6LSX713~6LSX713~6LSX713~6LSX713~2DR2T3~28P0^eQGK68R0~7R13~2D200213~2De00713~2D20713~2D20713~2D20~4NQGK68P0713~2EVbI3~28P713~2EVdI3~28P713~4L9D_X00713~4L9D_X00b?3~98P]Z=33AeU933<[C>339U00dA3~2DdfSE33EZ]0dA3~2DdbSE33EZ]0Y>3~7LS^0O13~2D20~4]VEF4H=S70~6]VEF4H=S70~6]VEF4H=S70~6]VEF4H=S70~6]VEF4H=S70~371344H~3LSX71344H~3LSX71344H~3LSX71344H~3LSX7134:D20~27134:D20~27134:D2071344;TRI;"
        "468P0PSEFF=1C>54GTe0~3]VEF4H=S70~6]VEF4H=S70~371344H4HF;<SO0071344H~3LSX71344H~3LSX71344H~3LSX71344H~3LSX7134:D22T5468P00NQG468a2O0~57134:D2007134:D2007134:D207134:D207134:D20~4NQG468P071344;TRI;468P71344;TRI;468P71344H~2FG@e0071344H~2FG@e00b?5H~244H~268P0`<BB<[SLHJEVU9FK>C00PSEFF=1C>54GTe00PSEFF=1C>54GTe00Y>KH~34~2LS^07134:D20~4OI64MH;T20~6OI64MH;T20~6OI64MH;T20~6OI64MH;T20~6OI64MH;T20~37134:L~4SX7134:L~4SX7134:L~4SX7134:L~4SX7134:D20~27134:D20~27134:D207134gB9VI;468P00eTG45>QLJK<UX0~3OI64MH;T20~6OI64MH;T20~37134:LLGB4:W`007134:L~4SX7134:L~4SX7134:L~4SX7134:L~4SX7134:D22T5468P00NQG468?TI_P0~37134:D2007134:D2007134:D207134:D207134:D20~4NQG468P07134gB9VI;468P7134gB9VI;468P7134:==;HF9_007134:==;HF9_00b?L~3JBL~38P0dAMJEVIGBF38SLH5@20~2eTG45>QLJK<UX0~2eTG45>QLJK<UX00Y>L~464K9ZX07134:D20~4YA:F"
        "EBK<[0~6YA:FEBK<[0~6YA:FEBK<[0~6YA:FEBK<[0~6YA:FEBK<[0~37134:DS~3C07134:DS~3C07134:DS~3C07134:DS~3C07134:D20~27134:D20~27134:D207134MBG?I;468P00XU<KJL<MH3I20~4YA:FEBK<[0~6YA:FEBK<[0~37134:D1?EBH3QO07134:DS~3C07134:DS~3C07134:DS~3C07134:DS~3C07134:DCCT5468P00NQG469G56>`0~37134:D2007134:D2007134:D207134:D207134:D20~4NQG468P07134MBG?I;468P7134MBG?I;468P7134:DZ<BJEU007134:DZ<BJEU007US~2EJB<S~2[00b?;438A:;5;TIG46QO0~2XU<KJL<MH3I20~3XU<KJL<MH3I20~2aZS~39KJLI2007134:D20~3]VEFM>5H=S70~4]VEFM>5H=S70~4]VEFM>5H=S70~4]VEFM>5H=S70~4]VEFM>5H=S70~27134:W~3>[07134:W~3>[07134:W~3>[07134:W~3>[07134:D20~27134:D20~27134:D207134;;B9I;468P0~2bQLJM64:"">[0~4]VEFM>5H=S70~4]VEFM>5H=S70~27134:D2R?;4:DR07134:W~3>[07134:W~3>[07134:W~3>[07134:W~3>[07134:W~35468P00NQG46MMJH3IO0~27134:D2007134:D2007134:D207134:D207134:"
        "D20~4NQG468P07134;;B9I;468P7134;;B9I;468P7134:DV<BF9_007134:DV<BF9_0~3XXUEJB<CXX0~2P134;T<BL6M>A:H=ZX0~3bQLJM64:"">[0~5bQLJM64:"">[0~4X~2_W:HGDC0~27134:D20~3OI64GI3H;T20~4OI64GI3H;T20~4OI64GI3H;T20~4OI64GI3H;T20~4OI64GI3H;T20~27134JB~39`07134JB~39`07134JB~39`07134JB~39`07134:D20~27134:D20~27134:D207134;9F6?;468P0~3[>54HF=1N0~4OI64GI3H;T20~4OI64GI3H;T20~27134:D2^1L4K>Y07134JB~39`07134JB~39`07134JB~39`07134JB~39`07134JB~3F468P00NQG4BE>3H:DR0~27134:D2007134:D2007134:D207134:D207134:D20~4NQG468P07134;9F6?;468P7134;9F6?;468P7134:==;JGDe007134:==;JGDe0~5UEJB<C0~5VEJM>EJE=F9<BF9_0~5[>54HF=1N0~6[>54HF=1N0~6CDGH:W_X0~27134:D20~3YA:J=Z9FK<[0~4YA:J=Z9FK<[0~4YA:J=Z9FK<[0~4YA:J=Z9FK<[0~4YA:J=Z9FK<[0~27134B:~39`07134B:~39`07134B:~39`07134B:~39`07134:D20~27134:D20~27134:D207134;@;K<;468P0~3fSEF4;Dd0~5YA"
        ":J=Z9FK<[0~4YA:J=Z9FK<[0~27134:D2^SLHK>Y07134B:~39`07134B:~39`07134B:~39`07134B:~39`07134B:~3B468P00NQG4;?UEJMAe0~27134:D2007134:D2007134:D207134:D207134:D20~4NQG468P07134;@;K<;468P7134;@;K<;468P71344HHJ=A_^0071344HHJ=A_^0~5UEJB<C0~5[<BF93KW9F=EJM>Y0~5fSEF4;Dd0~7fSEF4;Dd0~62ILFK9SO0~37134:D20~2]VEFK<~2:H=S70~2]VEFK<~2:H=S70~2]VEFK<~2:H=S70~2]VEFK<~2:H=S70~2]VEFK<~2:H=S7007134:D~4C07134:D~4C07134:D~4C07134:D~4C07134:D20~27134:D20~27134:D207134;?EF6;468P0~4d>KH=Z^0~4]VEFK<~2:H=S70~2]VEFK<~2:H=S7007134:D2aI64MAe07134:D~4C07134:D~4C07134:D~4C07134:D~4C07134:D~35468P00NQG468_9JMAe0~27134:D2007134:D2007134:D207134:D207134:D20~4NQG468P07134;?EF6;468P7134;?EF6;468P7134:=3BF3Ib007134:=3BF3Ib0~5UEJB<C0~5RA:H=;5D>M634;?20~6d>KH=Z^0~8d>KH=Z^0~5NS9KJLI20~47134:D20~2OI64HF~2J4;T20~2OI64HF~2J4;T20~2OI64"
        "HF~2J4;T20~2OI64HF~2J4;T20~2OI64HF~2J4;T2007134:D[~3R07134:D[~3R07134:D[~3R07134:D[~3R07134:D20~27134:D20~27134:D207134;?D;K;468P0~4d>KH=Z]0~4OI64HF~2J4;T20~2OI64HF~2J4;T2007134:D`S<M4G?a07134:D[~3R07134:D[~3R07134:D[~3R07134:D[~3R07134:DRRT5468P00NQG468_9JMAe0~27134:D2007134:D2007134:D207134:D207134:D[~3R0NQG468P07134;?D;K;468P7134;?D;K;468P7134:DILHK<`007134:DILHK<`0~5UEJB<C0~5OIG46M6I?;5;438c0~6d>KH=Z]0~8d>KH=Z]0~4X_>54GD[~3e007134:D20~2YA:H;~4FK<[0~2YA:H;~4FK<[0~2YA:H;~4FK<[0~2YA:H;~4FK<[0~2YA:H;~4FK<[007134:<~48f7134:<~48f7134:<~48f7134:<~48f7134:D20~27134:D20~27134:D207134;?SEF4468P0~4d>KH=Z]0~4YA:H;~4FK<[0~2YA:H;~4FK<[007134:<<=:4K9VX07134:<~48f7134:<~48f7134:<~48f7134:<~48f7134:D22T5468P00NQG468_9JMAe0~27134:D2007134:D2007134:D207134:D207134:<~48fNQG468P07134;?SEF4468P7134;?SEF446"
        "8P7134:D[WMH38O07134:D[WMH38O0~4UEJB<C0~5^1LHBJ=S83JFJEV]0~6d>KH=Z]0~8d>KH=Z]0~4Y@6H:<~58N07134:D200]VEFB<?~2>MH=S70]VEFB<?~2>MH=S70]VEFB<?~2>MH=S70]VEFB<?~2>MH=S70]VEFB<?~2>MH=S707134JB~36Qf7134JB~36Qf7134JB~36Qf7134JB~36Qf7134:D20~27134:D20~27134:D207134;?YA54468P0~4d>KH=Z]0~3]VEFB<?~2>MH=S70]VEFB<?~2>MH=S707134JBFJHK=I2007134JB~36Qf7134JB~36Qf7134JB~36Qf7134JB~36Qf7134:D22T5468P00NQG468_9JMAe0~27134:D2007134:D2007134:D207134:D207134JB~36QfNQG468P07134;?YA54468P7134;?YA54468P7134:D2I34M>C07134:D2I34M>C0~4UEJB<C0~6U9FgF9_VEJgB<[0~7d>KH=Z]0~8d>KH=Z]0~4`9FgJB~4GQN07134:D200OI65;TROaI65;T20OI65;TROaI65;T20OI65;TROaI65;T20OI65;TROaI65;T20OI65;TROaI65;T207135~56Qf7135~56Qf7135~56Qf7135~56Qf71355D20~271355D20~271355D207135;?aS=5568P0~4d>55=Z]0~3OI65;TROaI65;T20OI65;TROaI65;T207135~3;3<Qd0~2713"
        "5~56Qf7135~56Qf7135~56Qf7135~56Qf71355D22T5568P00NQG568_955Ae0~271355D20071355D20071355D2071355D207135~56QfNQG568P07135;?aS=5568P7135;?aS=5568P71355D2_<55=1P71355D2_<55=1P0~3UE55<C0~6C>5~2>Y[<5~2Ae0~7d>55=Z]0~8d>55=Z]0~4`95~7GQN071355D200O8@~2SP0XV@~2I20O8@~2SP0XV@~2I20O8@~2SP0XV@~2I20O8@~2SP0XV@~2I20O8@~2SP0XV@~2I20XV@~7SPXV@~7SPXV@~7SPXV@~7SPXV@~2Ib0~2XV@~2Ib0~2XV@~2Ib0XV@~21NCT@~2Z^0~4R?@~2U0~4O8@~2SP0XV@~2I20O8@~2SP0XV@~2I20XV@~3T?1_20~3XV@~7SPXV@~7SPXV@~7SPXV@~7SPXV@~2IbaQ@~2Z^00PS@~2Z[@~2?20~2XV@~2Ib00XV@~2Ib00XV@~2Ib0XV@~2Ib0XV@~7SPPS@~2Z^0XV@~21NCT@~2Z^XV@~21NCT@~2Z^XV@~2Ib2I@~2SPXV@~2Ib2I@~2SP0~3`@~2?Y0~62I@~2I2R?@~28a0~7R?@~2U0~9R?@~2U0~5CT@~8SP0XV@~2Ib0~2a2~2O0~2f2~2b0~2a2~2O0~2f2~2b0~2a2~2O0~2f2~2b0~2a2~2O0~2f2~2b0~2a2~2O0~2f2~2b0~2N2~7O00N2~7O00N2~7O00N2~7O00N2~2b0~4N2~2b0~4N"
        "2~2b0~2N2~2O0X2~3N0~62~3P0~5a2~2O0~2f2~2b0~2a2~2O0~2f2~2b0~2N2~4aP0~6N2~7O00N2~7O00N2~7O00N2~7O00N2~2b00a2~2N0~3O2~2N^2~2b0~4N2~2b0~3N2~2b0~3N2~2b0~2N2~2b0~2N2~7O00O2~2N0~2N2~2O0X2~3N00N2~2O0X2~3N00N2~2b00b2~2O00N2~2b00b2~2O0~4^2~3]0~7b2~2b002~3a0~92~3P0~:2~3P0~5X2~9O0~2N2~2b0~77b~2]0~7abO0abO0~6NbbObb70~7ObbO0~6NbbObb70~8abbc0~2NbbObb70~6abbc0~8ObbO0~9ObO0abf0~57~2X00X7~20~5^7~70~5ObO0abf0~57~2X00X7~20~57~2X007~2^00^7~20~6ab~4^0~?]0~G7~30~27~2^00X7~2X0~37~2X00]7~30~37~2X00X7~20~5X7~9]0~27~2^0~2^7~20~2X7~<X0~4]7~2X007~30~=XNaO70~<ObO0abf0~8]7~2X007~30~6]]00]0~9NbbObb70~6ObO0abf0~9NbbObb70~B_?~2C0~6d?""?8C?""?Qb0~4PS?""?Q?""?U0~6O1?""?820~4PS?""?Q?""?U0~7Y?~2Z^0PS?""?Q?""?U0~5Y?~2Z^0~6O1?""?820~7a8?Qd?""?Z70~3`1~2ZOfV1~2`]0~22S1~7Y0~3a8?Qd?""?Z70~3`1~2ZOfV1~2`]0~2`1~2ZOY1~3221~3Y0~4RI?~4`0"
        "~<fCUZUdX0~<XR~2a00R1~3C0C1~2SbPU1~2Vc00`1~2ZO^_1~3R00`1~2ZOfV1~2`]0~2PV1~9_]0C1~320bS1~2`0PV1~<Vc0~2^U1~2U7d1~3R0~:7YZQ?I1`O0~9a8?Qd?""?Z70~6^U1~2U7d1~3R0~3][ZVbUZ_70~6PS?""?Q?""?U0~5a8?Qd?""?Z70~7PS?""?Q?""?U0~A]Z=59`0~6YA;ETL3?20~4OIGGA5LSX0~5aI=59VP0~3OIGGA5LSX0~5a1=59S70OIGGA5LSX0~3a1=59S70~6aI=59VP0~6b?;<@E31P0~2713~2D22T3~28P0~2CW3~6EU0~3b?;<@E31P0~2713~2D22T3~28P00713~2D2UE33WCC<33EU0~4RT35W53?R0~9^`1>E=E@C0~<CDAA1P0[<33LIRV=33>Y`>3~2?20713~2D2[A3~2<[0713~2D22T3~28P0~2b?3~98PXZE339V7U<33=1^b?3~<T20~2OI3~2?O_933<[0~57ffPfUA=6;GLW1R0~8b?;<@E31P0~6OI3~2?O_933<[0~3fQ=<UA=T20~6OIGGA5LSX0~4b?;<@E31P0~7OIGGA5LSX0~>^77Z=59`770~3^7UW:3;A[70~3POIGGA5LS^0~57dI=;AC70~2OIGGA5LS70~377U<M912]0OIGGA5LSX0077U<M9127~20~377dI=;AC770~377Z955;W_77007134:D22T5468P0~2CWK4H~24JEU0~37Z955;W_77007134:D22T5"
        "468P007134:D2UEJBWCC<BJEU0~3X7CT36W5ESNaO^0~52TL56LG3QO0~77~3C<M6Qf0OS9KBE8V=HK>CD6H;A`]07134:DCT3JM9SO07134:D22T5468P0~2b?5H~244H~268P0eTG4;AYTG4;DY0b?5H~244H~26645T20~2OI;4;?O_9JF<[0~4PZQQ1_>;JJBFHME1O0~37~4Z955;W_0X7~2X00OI;4;?O_9JF<[0~3^S=6E36IO0~6OIGGA5LSX0~47Z955;W_70~37~3OIGGA5LSXX7~2X0~82S1~2@@T1~2[002S1~2>~21~2[0]`811@@I@@1SC]0~2`1~2I>>11[00`1@@I@@11[00`1~2>>1~2_Xc1@@I@@U]0`1~2>>1_1~3R00`1~2UI>>1~2`]0C1~3D>TS1~2`07134:D22T5468P0~2CWKJL~2BJEU0~2`1~2D>T1~3`]7134:D22T5468P007134:D2UEJBWCC<BJEU0~2PV1~2@>D>>QQ?IVP0~42TL<I1A6T20~6`1~4<B6Qf002Q=BM9Z=HK>?LJ:<VP007134:D?LFKE1b007134:D22T5468P0~2b?L~3JBL~38P0XU<MJ=1EFK9V^0b?L~3JBL~3645T20~2OI;4;?a_9JF<[0~4a?66>?GHK=<E54MW`0~2R1~3C21D>TSbPU1~2Vc0OI;4;?a_9JF<[0~4C@=6L>_0~7c1@@I@@U]0~4`11D>T1~2[00R1~3C1@@I@@UPU1~2Vc0~7Y>3~7LS^0Y>3~7LS^P133=>S"
        "C?9LLTY00713~6LSX713~6LSX713~7Qf0O13~2D20713~2Dd[A3~2<[0713~2?RV93~28PXZ=339Ud`<33L1^7134:D22T5468P0~2CWKJESWBJEU00713~2?dV93~28P7134:D22T5468P007134:D2UEJBWCC<BJEU0~2b?3~2?dedV<6;G?a0~27cNS1`<==8SOcc^0~2713~5B6Qf0~2d?3J5W=HK>EBK9SO0~27134:D=BB=QR0~27134:DCCT5468P0~27US~2EJB<S~2[0~2bQLJMW:H3I2007US~2EJB<SS8645T20~2OI;4;?C_9JF<[0~4O?;KW<BHLQ[Z9KHL1P00[<33LIRV=33>Y`>3~2?20OI;4;?C_9JF<[0~4YA3~2<[0~8YA3~2<[0~4713~6LSX0[<33LIRV=33>Y`>3~2?20~7Y>KH~34~2LS^0Y>KH~34~2LS^P13HJ:<?3BHFEV]071344H~3LSX71344H~3LSX71344H~36Qf07134:D207134:DCT3JM9SO07134;?R?G4468P0YA:46Ia83HM>[07134:D22T5468P0~2CWKJEVWBJEU007134;?R?G4468P7134:D22T5468P007134:D2UEJBWCC<BJEU0~2^Z=JM>C0RT;44;?a00OS8812C<E31R8~2VP0071344H~36Qf0~3CDGH;=HK9MB=820~37134:EKFL?e0~37134:W~35468P0~4XXUEJB<CXX0~4CA5H;H:"">[0~4XXUEJB<CP8645T20~2NQ64M9"
        "WW9JF<[0~471=J3LHB<>~3;4;?a00OS9KBE8V=HK>CD6H;A`]0NQ64M9WW9JF<[0~3]VEF4H=S70~6]VEF4H=S70~371344H~3LSX0OS9KBE8V=HK>CD6H;A`]0~7Y>L~464K9ZX0Y>L~464K9ZXP1=L:4;9BBGJLSX07134:L~4SX7134:L~4SX7134:L~4Qf07134:D207134:D?LFKE1b007134;?U9Bg468P0N1LHMW[>:46Ia07134:D22T5468P0~2CWBJEUWBJEU007134;?U9Bg468P7134:D22T5468P007134:D2UEJBWCC<BJEU0~3CWKJEZX_9BJG3?a00[>66AdC<3EVC>66AY007134:L~4Qf0~3R?6HJ;HB54BESP0~37134MM4BESf0~37134JB~3F468P0~6UEJB<C0~6PZEBgF=1N0~6UEJB<CP8645T20~2XZEF4B~34F<[0~5CA;JK4JK~4B45D20~22Q=BM9Z=HK>?LJ:<VP00XZEF4B~34F<[0~3OI64MH;T20~6OI64MH;T20~37134:L~4SX002Q=BM9Z=HK>?LJ:<VP0~8aZS~39KJLI200aZS~39KJLI200[Z1W5K6F=<;<`007134:DS~3C07134:DS~3C07134:DS~3`]07134:D207134:D=BB=QR0~27134;?""?6FM468P00[WMHL8=JB9U007134:D22T5468P0~2C<BJ9_WBJEU007134;?""?6FM468P7134:D22T5468P007134:D2UEJBWCC<BJEU0~3"
        "2?;4GIa83H5A8ZP0P1LH38NC<9IRa?GJEZX07134:DS~3`]0~2PZEBJK4ggHM45AC0~37134gH:45>[0~37134B:~3B468P0~6UEJB<C0~67V9BgFESc0~6UEJB<CP8645T20~3YT35:~3HF<[0~5PV>3;4KG~7AR0~3d?3J5W=HK>EBK9SO0~3YT35:~3HF<[0~3YA:FEBK<[0~6YA:FEBK<[0~37134:DS~3C0~3d?3J5W=HK>EBK9SO0~:X~2_W:HGDC0~3X~2_W:HGDC0~3XOS=HF5AZISa007134:W~3>[07134:W~3>[07134:D2X~20~27134:D207134:EKFL?e0~37134;?<K;;468P00aI64:A;4;TR007134:D22T5468P0~2[<FF<`WBJEU007134;?<K;;468P7134:D22T5468P007134:D2UEJBWCC<BJEU0~3^Z=JM>CD5H38f^002T;JEV]aU_R0^SLHGIO07134:D2X~20~4CA54;EKg459KJLQb0~271344;<KH3I20~27134:D~35431P0~6UEJB<C0~6CA;4:H5>[0~6UEJB<CP8645T20~3]CQD~39JF<[0~6N`8LHM>I~61a0~4CDGH;=HK9MB=820~4]CQD~39JF<[0~2]VEFM>5H=S70~4]VEFM>5H=S70~27134:W~3>[0~4CDGH;=HK9MB=820~=CDGH:W_X0~5CDGH:W_X0~5`9F468NOP0~27134JB~39`07134JB~39`07134:D20~57134:D207134MM4BES"
        "f0~37134;?6FE;468P0~2U9BJ=BFEZ^007134:D[[T5468P07CVEJK>[WBJEUY]7134;?6FE;468P7134:D[[T5468[b07134:D[UEJBW[[<BJEUCP0~2CWKJEV9BB9_0~3Y>MF9`0~6UEJ:AR07134:D20~6bQLJK9ILHK<I3HK<U[a07134:WQLJK9V[207134:DRRT5431[[a0~4UEJBW[[Y]0~2a8=JM<"":J3I[e0~4UEJB<[[8645T[[R0~3OR~2_9JF<[[YX0~5U9FJ=8d2eU?QO0~5R?6HJ;HB54BESP0~6ORC[_9JF<[0~2OI64GI3H;T20~4OI64GI3H;T20~27134JB~39`0~4R?6HJ;HB54BESP0~<2ILFK9SO0~52ILFK9SO0~6C>KHLS^0~47134B:~39`07134B:~39`07134:D20~57134:D207134gH:45>[0~37134;<K;D;468P0~2RT;4F4:AY0~27134:<~35468P0eA<54;<~2BJE<?b7134;<K;D;468P7134:<~3546<<U07134:<~2EJB<~3BJE<>C0~22?;46ILH5De0~3[<BBWCNY_U_dP`9FK>Y07134:D20~5XU<MH3IV=HK>[>:4G<<_07134:DCA54;<<V]7134:D22T543<~2_0~4UEJB<~2?b00]_WMJLQ=FK<<QN0~3UEJB<~3645<~2S^0~6_9JF<~2T20~5dD;4MEATA96?b0~4PZEBJK4ggHM45AC0~7RA<<9JF<[0~2YA:J=Z9FK<[0~4YA:J=Z9F"
        "K<[0~27134B:~39`0~3PZEBJK4ggHM45AC0~;NS9KJLI20~5NS9KJLI20~7d>KH=Z]0~47134:D~4C07134:D~4C07134:D20~57134:D2071344;<KH3I20~27134;3FEI;468P0~2^VEFgHL1N0~27134JB~3J468P0d>M4gFB~2H4B5T27134;3FEI;468P7134JB~3JgFBEV07134JB~34HB~3H4BBWC0~2^Z=JM>5438f0~3`9FBWC1A9E9AS`9FKWC07134:D20~5eTG4:"">`V=HK>Y1EFJB9_07134:D2Z9BHB=Z]7134:D22T5gFBB9_0~4UEJHBB5Tb00R?6H5ACD;4BGIN0~3UEJHB~3FgJBB31^0~6_9J4BB:D20~57V<"":HF:5:B5?b0~4CA54;EKg459KJLQb0~6eAMBB4F<[00]VEFK<~2:H=S70~2]VEFK<~2:H=S7007134:D~4C0~3CA54;EKg459KJLQb0~9X_>54GD[~3e00X_>54GD[~3e0~4d>KH=Z]0~47134:D[~3R07134:D[~3R07134:D20~57134:D207134:WQLJK9V70071345M5AI;468P0~2d[A5gMW[0~37135~2KB5~268P0d>MK5~6B5T271345M5AI;468P7135~8JEV07135~=MBWC0~3CWKFEBB9_0~4CWKJEA=:M5M5EA=HM>d07134:D20~4PSE55=1NV=55>YeTGJJ9_071355D22?3FH=Z]71355D22T5~2JJ9_0~4UE55:45Tb0^ZE55EZP"
        "V95FGIN0~3UE5~8B431^0~6_955:4:D20~6aSW3:FFM;=?b0~3bQLJK9ILHK<I3HK<UX0~5eAM4:55<[00OI64HF~2J4;T20~2OI64HF~2J4;T2007134:D[~3R0~2bQLJK9ILHK<I3HK<UX0~8Y@6H:<~58N0Y@6H:<~58N0~3d>KH=Z]0~47134:<~48f7134:<~48f7134:D20~57134:D207134:DCA54;AC0071344J=SI;468P00OI<9:46Ib0~3XV@~2D::A@~2Z^0d>M;@~5AM5T271344J=SI;468PXV@~8=JEV0XV@~>GBWC0~32?;4MH5De0~4R@;4K:KL>T>3B:B4G?b07134:D20~4PS@~2?d0_@~2?RXU<BJ9_0XV@~2Ib0`>KH=Z]XV@~2IbaQ@@<BJ9_0~4`@~2645Tb0^Z@~2?R02QWBGIN0~3`@~8D:431^0~6[T@@34:D20~7O_IA::"">?SC]0~2XU<MH3IV=HK>[>:4G@d0~5eAMHL@@TC00YA:H;~4FK<[0~2YA:H;~4FK<[007134:<~48f0XU<MH3IV=HK>[>:4G@d0~8`9FgJB~4GQN0`9FgJB~4GQN0~3d>KH=Z]0~47134JB~36Qf7134JB~36Qf7134:D20~57134:D207134:D2Z9BJL8a07134g5ACI;468P00a?;F4K9U]0~4N2~2D::AR22N00d>M;?2~3RAM5T27134g5ACI;468P0N2~7Z=JEV00N2~=IGBWC0~3^Z=Jg438N0~47V95K59IC2[?E5K;W"
        "U]07134:D20~5O2~300P2~300[<BJ9_00N2~2b00d>KH=Z]0N2~2b00a2[<BJ9_0~4^2286H5Tb00N2~2b0~2CWBGIN0~3^2~7RD:H31^0~6^2213H:D20~92D::DR70~4eTG4:"">`V=HK>Y1EFFE1N0~4eAMHLS22X0]VEFB<?~2>MH=S70]VEFB<?~2>MH=S707134JB~36Qf0eTG4:"">`V=HK>Y1EFFE1N0~7`95~7GQN0`95~7GQN0~3d>55=Z]0~47135~56Qf7135~56Qf71355D20~571355D2071355D22?355<U071355=1OI;568P00a?;5GEQ20~82D==AR0~4d>==?a0~2RA==T271355=1OI;568P0~8]Z==EV0~?OI==WC0~4CW5~29_0~6bZDWAZ20~2RSA>@VO0071355D20~C[<==9_0~8d>=~2Z]0~9[<==9_0~6c8=~2Tb0~9CW==IN0~<RD=~21^0~871=~2D20~92D55DR0~4PSE55=1NV=55>YeTG55>[0~4eA=~2SX0~2OI65;TROaI65;T20OI65;TROaI65;T207135~56QfPSE55=1NV=55>YeTG55>[0~7CT@~8SP0CT@~8SP0~3R?@~2U0~5XV@~7SPXV@~7SPXV@~2Ib0~5XV@~2Ib0XV@~2Ib0`@~3U0XV@~2TCc1@~2Z^00N1@TIVR0~9P_VV_c0~4OUVV`^0~2fUVV`7XV@~2TCc1@~2Z^0~9dV~2R0~?][VVUa0~42I@~2?e0~7PRCeP0~4cdCR^0~2X"
        "V@~2Ib0~CbV~3R0~8OUV~2d0~:bV~3R0~7CV~2`^0~9aUVV[]0~<P_V~2C0~:CV~2_P0~9a8TTQb0~4PS@~2?d0_@~2?RXU@~3[0~4NUV~2Y0~3O8@~2SP0XV@~2I20O8@~2SP0XV@~2I20XV@~7SPPS@~2?d0_@~2?RXU@~3[0~7X2~9O00X2~9O0~52~3P0~6N2~7O00N2~7O00N2~2b0~7N2~2b0~2N2~2b00^2~3P00N2~3X0O2~2N0~3O22OX0~UN2~3X0O2~2N0~[b2~30~KN2~2b0~}0~kObba0~6O2~300P2~300P2~3^0~?a2~2O0~2f2~2b0~2a2~2O0~2f2~2b0~2N2~7O00O2~300P2~300P2~3^0~GNbbObb70~FNbbObb70~8PbbabbO0~6NbbObb70~47~7]0~5NbbObb70~;POON]0~67~90~:bbO0~67~2X00X7~20~67~2X00^7~2^0~3^7~80~:XO~2^0~<POOP0~:77^0~2X770~S]NaO70~8^7~3X0~PN~2X0~<N~2]0~6N~20~RN~2X0~=N~2]0~7N~20~TN~2X0~=N~2]0~6aY~420~5PS?""?Q?""?U0~7aY~420~6PS?""?Q?""?U0~77V?~41O0~4PS?""?Q?""?U0~3`1~7_X0~3PS?""?Q?""?U0~92U1I?QVeX0~3`1~9`]0~7e?T8a0~4`1~2ZOfV1~2`]0~3`1~2ZOaZ1~2ZO002S1~8[0~7PCZI?IZCc0~82_1II1U20~7R1~220NV11C0~52Y~220~=NY~2"
        "e]0~2bUQ?I1_O0~42_S1~3UP0~5POON]0dYX0~3cRddRc0~5dQII_X0~:CI~2[0~5R8IIC0~67CUC[U`70~4R__[__Y0~5dQII_X0~;CI~2[0~6R8IIC0~7R__[__Y0~5bY~2272e^0~4dQII_X0~;CI~2[0~5XZ>~48f0~4OIGGA5LSX0~5XZ>~48f0~5OIGGA5LSX0~6CA;695=Qa0~4OIGGA5LSX00713~7Qf0~3OIGGA5LSX0~7YI<3;;6ET`^00713~98P0~6RVW5D1`b00713~2D22T3~28P0~2713~2D2Z<3~2DR00Y>3~7LS^0~5bZA=6;6=AZb0~5XCI93;;39?CX0~5_93WC02T3L170~3c1>~2170~<_>~2TR00O1<6;GL>C0~3R8<L3~3?a0~32U1I?QVY?DC0~2Y1D>>D1d0~4[>GGTd0~92ILG9V0~4cSEGLQa0~5[AE>?EDR0~3XS99D99IN0~4[>GGTd0~:2ILG9V0~5cSEGLQa0~5XS99D99IN0~3^S>~28S?AY0~4[>GGTd0~:2ILG9V0~57SLK~2GIN70~3OIGGA5LS770~377SLK~2GIN70~37OIGGA5LS770~37bILGA;=Ie70~2]7OIGGA5LS70071344H~36Qf0077OIGGA5LS7~2X0~3RILK4FFHF6AC0071344H~34468P0~4]C?E6MGL<8O07134:D22T5468P0~27134:DVW5H;>_^00Y>KH~34~2LS^0~4aS95L<><L591a0~4C@3G9WW9G3D[0~4P1L;T20c13"
        ";?20~3NQGK68P0~<UEKMAe00Y>:B66:WC0~2PSEKHBEE;?a0~2YI<3;;6ETL3IO0R?=5MM5=IR0~3fV<"":=1a0~7PV9KE1b0~4CA5=M<UX0~3P1365E5T20~3XSL:D;GIN0~4fV<"":=1a0~8PV9KE1b0~5CA5=M<UX0~4XSL:D;GIN0~3]UW5K9=;9U0~4fV<"":=1a0~8PV9KE1b0~4`11<~4Q11`]0`11@@I@@1~2`]0C1~2<~4Q11`00C1~2@@I@@1~2`00C11?>A>>I1~2`0^U1~2@@I@@11R07134:L~4Qf0`1~3@@I@@U1~2ZO00^VEBHG993FJL8O07134:L~35468P0~4e?3BKK5K;?a07134:D22T5468P0~27134:D>;4;>UP0~2Y>L~464K9ZX0~3^U<5<1`Y[1<5<U^0~2R?3LTVCCUTL3?R0~32T;L1P00U9M>C0~3NQG468a2O0~7c22UEJMAe00[<F5@IA>C0~22T;4gBWE;?a00RILK4FFHF6;>U^]VEBF63KB9U0~4OS9M<U]0~5^CA;=Q20~4bQL3?=GTe0~3OI6><6E1P0~3XS==D==IN0~5OS9M<U]0~6]CA;=Q20~5bQL3?=GTe0~4XS==D==IN0~3[?W=BB=W?[0~5OS9M<U]0~6]CA;=Q20~4713~2?[V93~28P713~2?bV93~28PXZ=339U[`<33L1^XZ=339Ub`<33L1^XZ=339Ud`<33L1^OI3~2?2_933<[07134:DS~3`]713~2D22b2213~2D2002?;4:"">"
        "U`TG4M>Y07134:DSST5468P0~3^ZEF;<"":DWL?a07134:DCCT5468P0~27134:A;H5<VN0~3aZS~39KJLI20~4RT;<8A828AQ<;TR00XV93?DD_`TD?3EV^0~2CWK9_0~2Y>MEV]0~2NQG468?TQ`P0~4Y1TT1EJMAe00Y>5F6<I`f0~2eAMggBWE;?a0^VEBHG9<3FJL8O0c864;T8LH31P0~2CSI?>>8_O0~3CSI>>?8_O0~3C?>DT?>AY0~3CSI?T?8_O0~3CSI?T?8_O0~42UIT>>ZO0~52UI>>I_O0~52?>DT?>AY0~42UITTQV[]0~3UE;=EK;A[]0~42VIT>>Zb0~52VI>>IUb0~47134;?R?G4468P7134;?R?G4468P0YA:46Ia83HM>[00YA:46Ia83HM>[00YA:46Ia83HM>[0OI;4;?O_9JF<[07134:D2X~2007134:D20~27134:D200Y>M438NXVEJF9_07134:D22T5468P0~32T;B9W:D[Q1O07134:W~35468P0~27134:6H:<ZO0~5X~2_W:HGDC0~5U93Q9M9U9MEQ39U00b?GW?GG?I6;@W;?200]VE:Ad0~2a?G6QO0~2NQG469;53>_X0~2Y?=;;=EJMAe00CT=FJK3A_]00RD5ggBWE;?a02?;4:"">US9M4:AY0NQG46IE;:68P0071=6;5;3WZc0071=6;5;3WZc0071=6;5;3WZc0071=6;5;3WZc0071=6;5;3WZc0~2RQ96553WZO0~3RQ96553WZO0~3RQ96"
        "553WZO0~3RQ96553WZO0~3CAT><LJ3?R0~3RQ96556<120~3RQ96556<120~37134;?U9Bg468P7134;?U9Bg468P0N1LHMW[>:46Ia00N1LHMW[>:46Ia00N1LHMW[>:46Ia0OI;4;?a_9JF<[07134:D20~47134:D20~27134:D200C<BH=Z]0[<BJEV07134:D22T5468P0~3Y>M5DW:D[[Cc07134JB~3F468P0~27134HHM91a0~8CDGH:W_X0~471L9U9M9U9MEV9L170Y>G?""?GG?""?6;@IG>Y00OQ6GIa0~2^Z=5DR0~2NQG46M:FHGTR00f1=FJMM=JMAe0P1LFLE5H6?200N8=F4BWE;?a0Y>M438Z<"":EJF9_0NQG469KGA?V^00713:;;MH5AY00713:;;MH5AY00713:;;MH5AY00713:;;MH5AY00713:;;MH5AY00PSEKF55J5W`0~2PSEKF55J5W`0~2PSEKF55J5W`0~2PSEKF55J5W`0~2XU>6MBMJBEZ^00cSEKJMMHM9Z^00cSEKJMMHM9Z^0~27134;?""?6FM468P7134;?""?6FM468P00[WMHL8=JB9U0~3[WMHL8=JB9U0~3[WMHL8=JB9U00OI;4;?a_9JF<[07134:D20~47134:DCCRN7134:D200C<BH=V00[<BH=V07134:D22T5468P0~3C<B;IW:""><<>C07134B:~3B468P0~27134HHF=Ie0~72ILFK9SO0~5NQ6>YQAQR8AIY>6QN0C<31UDDU_"
        "DAU13<[00eA:=ZX0~3[<B<`0~2NQG4K9A=FB9_00eA546>W;4MAe0a?;MAZA5M>d0~2Y?E;BWE;?a0C<BH=ZW5E<BJEV0NQG46=H6@Ua0~271EA?""?WMFEU0071EA?""?WMFEU0071EA?""?WMFEU0071EA?""?WMFEU0071EA?""?WMFEU00eA5JLTDGH38N00eA5JLTDGH38N00eA5JLTDGH38N00eA5JLTDGH38N00RTGHKGGM4;T200eA546A>;4;T200eA546A>;4;T20~27134;?<K;;468P7134;?<K;;468P00aI64:A;4;TR0~3aI64:A;4;TR0~3aI64:A;4;TR00OIG4:D889JF<[07134:D20~47134:WW>D1C134:D200CWKHLS70_9FJ9U07134:D22T5468P0~3C<BGIW:<BBWC07134:D~35431P0~27134:6HB=QR0~5NS9KJLI20~6NQG>YYea0aeCd>GIN0[<317CSIISCP1L<[00_9K<[0~4RD5=S^00NQG4G?dA:HLSX0[<BF9UZ=HMAe0OIGFLAT;:Ae0~3dSAMWE;?a0C<BH=>;=I<BJ=V0NQG469BFG<1a00]`T9=LL5H=ZX0]`T9=LL5H=ZX0]`T9=LL5H=ZX0]`T9=LL5H=ZX0]`T9=LL5H=ZX0[<BB9~2=H5D200[<BB9~2=H5D200[<BB9~2=H5D200[<BB9~2=H5D200[<BJ=IIEJK>C00[<BF9_VEJK>C00[<BF9_VEJK>C0~27134;?6FE;468P7134;?6F"
        "E;468P0~2U9BJ=BFEZ^0~4U9BJ=BFEZ^0~4U9BJ=BFEZ^00P13HJG6~2JF<[07134:D[[R0~27134JBBK5L@134:D200RD54;TYRQ3HKWC07134:D22T5468[[a00CWK;?W:W9BWC07134:DRRT5431[[a07134:A;HKE1[[200X_>54GD[~4CX00P13<S>SO0OZ>1<31P0C>6QUA=GG=AUQ6WC071L5@20~4N83;?b00NQG4682T5431^0`9FB<CUEJMAe0XV9MJ56BL8O0~4eAMWE;?a0CWKHLGL?_9FJEU0NQG46?=M4:<_00RT6BBM:BHLZX0RT6BBM:BHLZX0RT6BBM:BHLZX0RT6BBM:BHLZX0RT6BBM:BHLZX0_9FHF~4MAR00_9FHF~4MAR00_9FHF~4MAR00_9FHF~4MAR00_9FB<C`9FB<C00_9FB<C`9FB<C00_9FB<C`9FB<C0~27134;<K;D;468P7134;<K;D;468P0~2RT;4F4:AY0~5RT;4F4:AY0~5RT;4F4:AY0~3_<"":JH~3gF<[07134:<~21P007134B::KHF=134:D200c1=JF=AD9MH6?b07134:D22T546<~2_00RD5M>W:D9BWC07134:D22T543<~2_07134:D>;4M9<<ZX0Y@6H:<~6DR00]VE=>5W1U1W5>=EV]02@5A>;=<W=;>A5DR02?;38c0~5UEM>C00NQG4GIRD5HLS^0[<BF9`VEJMAe0021<6KH3DC0~5eAMWE;?a0RD54M3TCQ3HKWC0NQG4688<5"
        "H3170[<BBEAA5HLZX0[<BBEAA5HLZX0[<BBEAA5HLZX0[<BBEAA5HLZX0[<BBEAA5HLZX0`9FK9~5AR00`9FK9~5AR00`9FK9~5AR00`9FK9~5AR00`9FB<`U9JKWC00`9FB<`U9JKWC00`9FB<`U9JKWC0~27134;3FEI;468P7134;3FEI;468P0~2^VEFgHL1N0~5^VEFgHL1N0~5^VEFgHL1N0~3aSW=~4JF<[07134JBB68P007134:DDW54;?34:D20~2CAGJF:5BH5W_]07134:D22T5gFBBEU00N8LJ3W:D9BWC07134:D22T5gFBB9_07134:DV<54BBLZX0`9FgJB~5:AR0~2YA5>95LEL59>5AC00P1=~2E8CC8E=~21P0C>M9U0~6Y>MEU00NQG4:WI9BFEU00Y>MHL?DG4MAe0028S?<5J=V]0~4eAMWE;?a0c1=JFLAD9M46?20NQG46A8Q6468P0_9FK>1<KHLZX0_9FK>1<KHLZX0_9FK>1<KHLZX0_9FK>1<KHLZX0_9FK>1<KHLZX0Y>:J=I1I>=1700Y>:J=I1I>=1700Y>:J=I1I>=1700Y>:J=I1I>=1700Y>MHL?T345DR00Y>MHL?T345DR00Y>MHL?T345DR0~271345M5AI;468P71345M5AI;468P0~2d[A5gMW[0~6d[A5gMW[0~6d[A5gMW[0~5O[V~39JF<[071355K468P007134:DR8645T34:D20~2^_AL5KKH3>ZO0071355D22T5~2JJEU0~2[>;F;:L5"
        "BWC071355D22T5~2JJ9_071355D2Z<5BHLZX0`95~6:4:AR0~2f1E6D>ELE>D6E1N0~2CA;9ICbbCI9;AC0]VEM>Y0~6b?;38N0NQG4;:GM45AY00OQ3JB;;6JMAe00Y>=<>6H=ZX0~4eAMWE;?a0eIL5JF:5BH5W_]0NQG46E33KJ=SX0CWMH;35:HLZX0CWMH;35:HLZX0CWMH;35:HLZX0CWMH;35:HLZX0CWMH;35:HLZX0O8LFK636:31700O8LFK636:31700O8LFK636:31700O8LFK636:31700O8LFB;;FB=1c00O8LFBG;FB=1c00O8LFBG;FB=1c0~271344J=SI;468P71344J=SI;468P00OI<9:46Ib0~5OI<9:46Ib0~5OI<9:46Ib0~7]~2_9JF<[0XV@~25468P007134:AAW54;?34:D20~37C8D>9K:<VP00XV@~2IbaQ@@<BJEU0~2PVW6MFK;=AY0XV@~2IbaQ@@<BJ9_0XV@~2Iba1AMHLZX0CT@~634:AR0~3RI=G<D?@<G=IR0~37UW;=>?""?>=;WU70N83G?a0~6^S=5DR0NQG56=MK5<ZP0~2[A6KK6E55Ae00Y>KBMF5>[0~5eAMWE;?a71=;>L5KKM6WZO00NQG56EMBM6AC00aQ=MK5955LZX0aQ=MK5955LZX0aQ=MK5955LZX0aQ=MK5955LZX0aQ=MK5955LZX00CT=5KKM;=170~2CT=5KKM;=170~2CT=5KKM;=170~2CT=5KKM;=170~2CTL:KK5=?d"
        "0~3CTL:KK5=?d0~3CTL:KK5=?d0~37134g5ACI;468P7134g5ACI;468P00a?;F4K9U]0~5a?;F4K9U]0~5a?;F4K9U]0~:_9JF<[00N22T5H68P007134B::KHKE134:D20~5NRYQ=F;>`0~2N2~2b00a2C<BJEU0~3N_IW:"">TSY^00N2~2b00a2[<BJ9_00N2~2b00eAMHLZX0X2~613H:AR0~4dI9G5;5G9IY0~5NVAL5;;5LAVO002DEES^0~7`<E<C0PS@~28>WDZa0~37`?>>?@~2I200dT<E=EAVN0~5eA=WE=?a0`A<V8AWWAI_O0~2PS@~2?AW>?`^0~2d8>WA8@~2U0~2d8>WA8@~2U0~2d8>WA8@~2U0~2d8>WA8@~2U0~2d8>WA8@~2U0~2]Y1DWWA?SY0~3]Y1DWWA?SY0~3]Y1DWWA?SY0~3]Y1DWWA?SY0~3]C8AWWD1d0~4]C8AWWD1d0~4]C8AWWD1d0~471355=1OI;568P71355=1OI;568P00a?;5GEQ20~6a?;5GEQ20~6a?;5GEQ20~;_955<[0~32T=~28P007135~469I1355D20~8d?9~2170~:C<==EU0~5Y><T2P0~<[<==9_0~8eA=~2ZX0~7^1=~2AR0~52V?A>A?V20~7PC1D>>D1CP0~27`UUe0~8bU~2a00O2~2OYCRP0~5]2CC2~3b0~2]R[UVUYP0~6NUV~3`^07_VONRCCda0~5O2~2adCYb0~5OYCea2~2P0~3OYCea2~2P0~3OYCea2~2P0~3OYC"
        "ea2~2P0~3OYCea2~2P0~4NRCCdb70~6NRCCdb70~6NRCCdb70~6NRCCdb70~6NRCCRf0~7NRCCRf0~7NRCCRf0~5XV@~2TCc1@~2Z^XV@~2TCc1@~2Z^00N1@TIVR0~7N1@TIVR0~7N1@TIVR0~<[T@@TC0~37_V~2C0~2XV@~4TIV2V@~2Ib0~9Y`~2R0~;bV~3R0~5PC[YX0~=bV~3R0~8NUV~2d0~9YV~2_c0~6XaeYeaX0~:f2YYRf0~v]0]]0~4]0~}0~}0~TN2~3X0O2~2N00N2~3X0O2~2N0~3O22OX0~9O22OX0~9O22OX0~=^2~3X0~=N2~5OX0N2~2b0~}0~}0~}0~}0~}0~@N~20~TN~2X0~<N~2]0~6N~20~S]]00]0~9N~2]0~6N~20~BcNN0PNN0~g]]00]0~DcNN0PNN0~E]]00]0~:N~4P0~8N~2]0~5N~20~6cNN0PNN0~O]]00]0~8N~4P0~4]N~20~@XN~20~9XN~20~4XN~20~kXN~20~<R8IIC0~77CUC[U`70~5R__[__Y0~5dQII_X0~:CI~2[0~5R8IIC0~6R__[__Y0~5aY~420~5][ZVbUZ_70~6CI~2[0~5R8IIC0~7RY~2a0~3a1I8R1I8R0~7NY~2ecNNP0~9NY~2e]0~4aY~420~6][ZVbUZ_70~6RY~2a0~5a1I8R1I8R0~5aY~420~5][ZVbUZ_70~7eQI8IIZc0~6CI~2[0~4R8IIC0~4a1I8R1I8R0~37CUC[U`70~3aY~420~5][ZVbUZ_70~5eQI8II"
        "Zc0~3`I~2CPOON]0~9]`I~2C0~7]`I~2C0~2]`I~2C0~4OdC[CRc0~6OdCCeN0~4R__[__Y0~3R__[__Y0~8]`I~2C0~:cSEGLQa0~6[AE>?EDR0~4XS99D99IN0~4[>GGTd0~92ILG9V0~4cSEGLQa0~4XS99D99IN0~3XZ>~48f0~4fQ=<UA=T20~52ILG9V0~4cSEGLQa0~5OI>~2ZX0~22TGE1<G>C0~7_>~2T1IIZf0~6RC_>~2TC70~2XZ>~48f0~5fQ=<UA=T20~5OI>~2ZX0~42TGE1<G>C0~4XZ>~48f0~4fQ=<UA=T20~6^VEG>G3Ia0~52ILG9V0~3cSEGLQa0~32TGE1<G>C0~3[AE>?EDR0~2XZ>~48f0~4fQ=<UA=T20~4^VEG>G3Ia0~2R?3G<_1I?QVe]0~7R?6GW_0~7R?6GW_0~2R?6GW_0~2]CQA<<WD1b0~4Y8A<<A1e0~2XS99D99IN00XS99D99IN0~7R?6GW_0~:CA5=M<UX0~4P1365E5T20~4XSL:D;GIN0~4fV<"":=1a0~7PV9KE1b0~4CA5=M<UX0~3XSL:D;GIN0~3XSLK~2GIN0~4^S=6E36IO0~4PV9KE1b0~4CA5=M<UX0~4a?;KLS^0~2XU<M<;ESc0~7UEKMA>G6Ia0~5P1<<EKM<AY0~2XSLK~2GIN0~5^S=6E36IO0~5a?;KLS^0~4XU<M<;ESc0~4XSLK~2GIN0~4^S=6E36IO0~6RT;E95>[0~5PV9KE1b0~3CA5=M<UX0~2XU<M<;ESc0~2P1365"
        "E5T20~2XSLK~2GIN0~4^S=6E36IO0~4RT;E95>[0~2NSEM9I<3;;6ET`^0~5O1EM9ZO0~6O1EM9ZO00O1EM9ZO0~2R?LMBBK5Ae0~3R?=:BB:=I200XSL:D;GIN00XSL:D;GIN0~6O1EM9ZO0~9bQL3?=GTe0~4OI6><6E1P0~4XS==D==IN0~3^cOS9M<UcP0~2^ccCA;=Q2P0~2^bQL3?=GTe0~2^cS==D==IN0~3XS<~4QN0~4^C@=6L>_0~5CA;=Q20~4bQL3?=GTe0~4a?;MLS^0~3bQL:GTe0~5c22UEJMA9F9U]0~5c86BB4JK>Y0~2XS<~4QN0~6C@=6L>_0~6a?;MLS^0~5bQL:GTe0~5XS<~4QN0~5C@=6L>_0~6]U9GT;9Zc0~5CA;=Q20~3bQL3?=GTe0~3bQL:GTe0~2^OI6><6E1P0~2^S<~4QNP0~2^cC@=6L>_P0~2^cU9GT;9Zc0~2`>:EI=K4FFHF6DC0~5_W:91bcP0~5_W:91b0~2_W:91b0~3_9FMLL3;Ae0~2^ZEBF66JK9U]0XS==D==IN00^S==D==IN0~6_W:91b0~:2?>DTT>AY0~47V8ITTQUb0~5YVITTQUb0~2PV8~21>>88SO0PV8~2>>I88SO0PV8?>DC?>ASO0PV8~2V_8~2SO0~2CSI?T?8_O0~3CSI?T?8_O0~42U>>TIZd0~32?>DT?>AY0~32UQA~2Zd0~32UA>>IZd0~3Y1TT1EJMA=5De0~5Y1T<<EJM<>Y0~32UITTQ_O0~52UITTQ_O0~52UIA~2"
        "ZO0~52UA>>Q_O0~52VITTQUb0~52VITTQUb0~52Z>>D>?R0~4aUQ>>?8V20~22?>DT?>AY0~2aUQA>>8V20PV8~21UQ88SO0PV8~2`[8~2SO0PV8~2S8~3SO0PV88>>D>?8SO00U>>Q9BH;9<3FJL8O0~2]YS>>IS88120~2OUQ>>?1[00PV>>8`0~4UEJ;@11?TR0~22?;4GTD;46IO00YV8~2V[]0PV8~2_8~2C0~52V>>TQUb0~9RQ96556<120~3RQ96556<120~3RQ96556<1200a?6631UE66DR0a?6631UE66DR0a?6631UE66DR0a?6631UE66DR0071=6;5;3WZc0071=6;5;3WZc0~228<655G=Z]0028<655G=Z^0~228<655G=Z]0028<655G=Z]00Y?=;;=EJMA>>1P0~4Y?=;;=EJMACP0~2RQ96553WZO0~3RQ96553WZO0~3RQ96553WZO0~3RQ96553WZO0~3RQ96556<120~3RQ96556<120~3RQ96556<120~2PZ<655;39`00PZ<655;39`00PZ<655;39`0a?6631UE66DR0a?6631UE66DR0a?6631UE66DR0a?6631UE66DR00Odd?G4:"">U`T64M>Y0~2CT=;5G<66>Y00^V<655;38P0a?66LS^0~3C>5F;39DVO0~2Y>MJEVZ=H:AR00a?66LS^00a?66LS<63?R0~3RQ96556<120~7cSEKJMMHM9Z^00cSEKJMMHM9Z^00cSEKJMMHM9Z^0a?;431UEJ:DR0a?"
        ";431UEJ:DR0a?;431UEJ:DR0a?;431UEJ:DR00713:;;MH5AY00713:;;MH5AY00PSEM4K:K=Z]0PSEM4K:K=Z]00PSEM4K:K=Z]0PSEM4K:K=Z]0f1=FJMM=JMAddb0~4f1=FJMM=JMAe0~2PSEKF55J5W`0~2PSEKF55J5W`0~2PSEKF55J5W`0~2PSEKF55J5W`0~2cSEKJMMHM9Z^00cSEKJMMHM9Z^00cSEKJMMHM9Z^00eA5J5;5B9`00eA5J5;5B9`00eA5J5;5B9`0a?;431UEJ:DR0a?;431UEJ:DR0a?;431UEJ:DR0a?;431UEJ:DR0~3d>M438N]VEJF9_00OQLFB;B5FGIa002?;4MG;68P0a?;4LS^0~2]`>GJMFF;WUX00[<BB<WW9FKWC00a?;4LS^00a?;4LSI6BEV^00cSEKJMMHM9Z^0~6eA546A>;4;T200eA546A>;4;T200eA546A>;4;T20a?;4L1UEJ:DR0a?;4L1UEJ:DR0a?;4L1UEJ:DR0a?;4L1UEJ:DR0071EA?""?WMFEU0071EA?""?WMFEU00RD54;<DW=Z]0RD54;<DW=Z]00RD54;<DW=Z]0RD54;<DW=Z]0eA546>W;4MAe0~6eA546>W;4MAe0~2eA5JLTDGH38N00eA5JLTDGH38N00eA5JLTDGH38N00eA5JLTDGH38N00eA546A>;4;T200eA546A>;4;T200eA546A>;4;T200CWB5A?D<9`00CWB5A?D<9`00CWB5A?D<9`0a?;4L1UEJ:DR0a"
        "?;4L1UEJ:DR0a?;4L1UEJ:DR0a?;4L1UEJ:DR0~3CWBH=Z]0[<BJ=V00dA:HL?LHJ=ZX00RD:B<I?>1P0a?;4LS^0~22?6H6>9:4G?200`9FHK~34B<C00a?;4LS^00a?;4LSV=JG?a00eA546A>;4;T20~6[<BF9_VEJK>C00[<BF9_VEJK>C00[<BF9_VEJK>C0a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR00]`T9=LL5H=ZX0]`T9=LL5H=ZX0[<BF9VR[Q`00[<BF9VR[Q`0~2[<BF9VR[Q`00[<BF9VR[Q`00[<BF9UZ=HMAe0~6[<BF9UZ=HMAe0~2[<BB9~2=H5D200[<BB9~2=H5D200[<BB9~2=H5D200[<BB9~2=H5D200[<BF9_VEJK>C00[<BF9_VEJK>C00[<BF9_VEJK>C00C>KK3E<DSR00C>KK3E<DSR00C>KK3E<DSR0a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR0~3C<BH=V]0C<BH=V00C<BB<`9FKW[0~2OI3B6EE1R00a?;4LS^0~2CWKF9US=JK>C00`9FFG~3JB<C00a?;4LS^00a?;4LS_9J:DR00[<BF9_VEJK>C0~6_9FB<C`9FB<C00_9FB<C`9FB<C00_9FB<C`9FB<C0a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR00RT6BBM:BHLZX0RT6BBM:BHLZX0_9FB<C00N]00_9FB<C00N"
        "]0~2_9FB<C00N]00_9FB<C00N]00`9FB<CUEJMAe0~6`9FB<CUEJMAe0~2_9FHF~4MAR00_9FHF~4MAR00_9FHF~4MAR00_9FHF~4MAR00_9FB<C`9FB<C00_9FB<C`9FB<C00_9FB<C`9FB<C002?3KJJB59V]02?3KJJB59V]02?3KJJB59V]a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR0a?;4LSUEJ:DR0~3Y>K4L170_9FJEU00[<FKW[<K;T20~2cZ<3JF31P00a?;4LS^0~2_9FBWC`9FB<C00[<BB<QQ9JK>C00a?;4LS^00a?;4LS_9J:AR00_9FB<C`9FB<C0~6`9FB<`U9JKWC00`9FB<`U9JKWC00`9FB<`U9JKWC0a?;431S=H:DR0a?;431S=H:DR0a?;431S=H:DR0a?;431S=H:DR00[<BBEAA5HLZX0[<BBEAA5HLZX0`<FF9_NeVY00`<FF9_NeVY0~2`<FF9_NeVY00`<FF9_NeVY00[<BF9`VEJMAe0~6[<BF9`VEJMAe0~2`9FK9~5AR00`9FK9~5AR00`9FK9~5AR00`9FK9~5AR00`9FB<`U9JKWC00`9FB<`U9JKWC00`9FB<`U9JKWC00N_?<=3546IO0N_?<=3546IO0N_?<=3546IOa?;431S=H:DR0a?;431S=H:DR0a?;431S=H:DR0a?;431S=H:DR0~32@54;@YR8LHKWC00C<BB<Z=J5T20~2eA5BL<<1eX0a?;4LS^0~2`9FB<`U9FKWC00Y>MJEV"
        "S=H:AR00a?;4LS^00a?;4LSVEJ5T200`9FB<`U9JKWC0~6Y>MHL?T345DR00Y>MHL?T345DR00Y>MHL?T345DR0OIG4GDA;4:DR0OIG4GDA;4:DR0OIG4GDA;4:DR0OIG4GDA;4:DR00_9FK>1<KHLZX0_9FK>1<KHLZX0YA:H6DQ@EZ]0YA:H6DQ@EZ]00YA:H6DQ@EZ]0YA:H6DQ@EZ]0Y>MHL?DG4MAe0~6Y>MHL?DG4MAe0~2Y>:J=I1I>=1700Y>:J=I1I>=1700Y>:J=I1I>=1700Y>:J=I1I>=1700Y>MHL?T345DR00Y>MHL?T345DR00Y>MHL?T345DR00d>>I11=H;?a0d>>I11=H;?a0d>>I11=H;?aOIG4GDA;4:DR0OIG4GDA;4:DR0OIG4GDA;4:DR0OIG4GDA;4:DR0~3P1=JFLA@9M4G?200d>MJE?G4M>8R00CWKMAS1T@R0a?;431C^00Y>:HL?T645DR00b?G4;A>546QO00a?;431C^0O?;431T646QO00Y>MHL?T345DR0~6O8LFBG;FB=1c00O8LFBG;FB=1c00O8LFBG;FB=1c071=JF556H:DR071=JF556H:DR071=JF556H:DR071=JF556H:DR00CWMH;35:HLZX0CWMH;35:HLZX0O8=FJ5G5=Z]0O8=FJ5G5=Z]00O8=FJ5G5=Z]0O8=FJ5G5=Z]0OQ3JB;;6JMAe0~6OQ3JB;;6JMAe0~2O8LFK636:31700O8LFK636:31700O8LFK636:31700O8LFK636:"
        "31700O8LFBG;FB=1c00O8LFBG;FB=1c00O8LFBG;FB=1c00d>MG33:H38N0d>MG33:H38N0d>MG33:H38N71=JF556H:DR071=JF556H:DR071=JF556H:DR071=JF556H:DR0~4CAGJF:5B45<U]00aI64M6BKJG<[00dA:J;LL;Ae0O?;4G9@200O8LFBG;FB=1c00XV9KH;5HM<U0~2O?;4G9@20c83H:L6FK9U]00O8LFBG;FB=1c0~7CTL:KK5=?d0~3CTL:KK5=?d0~3CTL:KK5=?d0~2CAGKK3E55DR00CAGKK3E55DR00CAGKK3E55DR00CAGKK3E55DR00aQ=MK5955LZX0aQ=MK5955LZX00Y?=5KK:=Z]00Y?=5KK:=Z]0~2Y?=5KK:=Z]00Y?=5KK:=Z]00[A6KK6E55Ae0~7[A6KK6E55Ae0~3CT=5KKM;=170~2CT=5KKM;=170~2CT=5KKM;=170~2CT=5KKM;=170~2CTL:KK5=?d0~3CTL:KK5=?d0~3CTL:KK5=?d0~2d>G:KBM6>`00d>G:KBM6>`00d>G:KBM6>`00CAGKK3E55DR00CAGKK3E55DR00CAGKK3E55DR00CAGKK3E55DR0~4^_AL5KKM6WZO0~2]UW;KKME65<[00N19;KBK5Ae0f8LKF:D20~2CT=5KK5=?d0~32IE5KM;98b0~2f8LKF:D200`>G:KM;EI20~3CTL:KK5=?d0~8]C8AWWD1d0~4]C8AWWD1d0~4]C8AWWD1d0~3P_TW>I@~2I200P_TW>I"
        "@~2I200P_TW>I@~2I200P_TW>I@~2I20~2d8>WA8@~2U0~2d8>WA8@~2U0~3Y1D>WAI[0~3Y1D>WAI[0~4Y1D>WAI[0~3Y1D>WAI[0~27`?>>?@~2I20~77`?>>?@~2I20~3]Y1DWWA?SY0~3]Y1DWWA?SY0~3]Y1DWWA?SY0~3]Y1DWWA?SY0~3]C8AWWD1d0~4]C8AWWD1d0~4]C8AWWD1d0~3aS?AWW>I_P00aS?AWW>I_P00aS?AWW>I_P00P_TW>I@~2I200P_TW>I@~2I200P_TW>I@~2I200P_TW>I@~2I20~57C8AWWAI_O0~4OUT>>A8I@TY0~22ZT>W>@1b00C?<<W?20~2]Y1AWWD1d0~5RSD>>TZ20~4C?<<W?200fU?A>>TSR0~4]C8AWWD1d0~;NRCCRf0~7NRCCRf0~7NRCCRf0~5X2CYb2~2b0~3X2CYb2~2b0~3X2CYb2~2b0~3X2CYb2~2b0~4OYCea2~2P0~3OYCea2~2P0~4fRCCeO0~5fRCCeO0~6fRCCeO0~5fRCCeO0~4]2CC2~3b0~9]2CC2~3b0~6NRCCdb70~6NRCCdb70~6NRCCdb70~6NRCCdb70~6NRCCRf0~7NRCCRf0~7NRCCRf0~5^aeCCda0~4^aeCCda0~4^aeCCda0~4X2CYb2~2b0~3X2CYb2~2b0~3X2CYb2~2b0~3X2CYb2~2b0~8NRCCda0~7^2YYdaa22]0~372YCY2c0~32C[CR0~5NRCCRf0~7PRYY270~62C[CR0~3]aedd2P0~7NRCCRf0~}"
        "0~}0~}0~}0~}0~}0~pXN~20~;XN~20~s]7]0~5N~2X0~I]]00]0~D]]00]0~z7~20~77~20~DXOaP0~3OaO0~9NaOP0~:POOP0~8POaOP0~57~6^0~8cOOP0~7]fOOf]0~8cOO^0~:7OaO^0~8^OaOP0~9POON]0~8cOON^0~57~2X007~30~@PbbPc~2P0~T]`I~2C0~9]`I~2C0~6NdCCR^0~7feCCeP0~9^^7O2e70~5]^^f2e70~3]2Y[UV1U^0~3dQII_X0~8R__[__Y0~6][ZVbUZ_70~3R__[__Y0~5][ZVbUZ_70~4R__[__Y0~5R__[__Y0~5R__[__Y0~6R__[__Y0~6R__[__Y0~4[1~2`]0~4[1~2`]0~7c_U`U_N0~22VI?1C^0dI?Id0~6aU8?I1U20~6c[1?""?1[c0~4RU1I?I1`a0~2Y1~720~52U8II1U20~4RU8?""?8UR0~5b_8IISY^0~6a`1I?IZCN0~4a[1I?I1Ue^0~42U1I?8Ve]0~4bU8?""?QS_20~2`1~2ZOR1~3e0~4OeY~2c0~3Y1?""?18~21a002Y~220~72Y~22002Y~220~8R?6GW_0~9R?6GW_0~5R1A<W@VO0~5R1AW<AS20~5aCVS11ITAC0~3NCVS18?AC0~2^`?><E=L?20~3[>GGTd0~7XS99D99IN0~5fQ=<UA=T20~2XS99D99IN0~4fQ=<UA=T20~3XS99D99IN0~3XS99D99IN0~3XS99D99IN0~4XS99D99IN0~4XS99D99IN0~2XSL338"
        "P0~3XSL338P0~52U8AEDE>d00O196;LA[PSE;>C0~5a1<3;G3EU0~5cV>L;;L>Vc0~2XZ93G;GL<1a00UE3~5<C0~4dI93;G39Z]0~2RI96;;69IR0~321<3;G=D`X0~4R8<LG;6=>VX0~2R1WLG;G39DC0~3YI<3;;6ET`^0~228<3;;6L9SX0713~2D2`9339`0~2]CIA>~2`0~2Y?=;;=E66Ae0c1>~2170~5c1>~217c1>~2170~6O1EM9ZO0~8O1EM9ZO0~428E:BB;W_]0~3O1E5BB59Sc0~3YQWEL3365<[0~22S>EL36;<[0~2CA6MBJFKW[0~3fV<"":=1a0~6XSL:D;GIN0~5^S=6E36IO0~2XSL:D;GIN0~4^S=6E36IO0~3XSL:D;GIN0~3XSL:D;GIN0~3XSL:D;GIN0~4XSL:D;GIN0~4XSL:D;GIN0~27SLH68P]0~27SLH68P]0~3YI93;53;?200C>:6EM3QCA5=1N0~5`W:HM:BEU0~5[>;HBKH;>[0~2XZLFKKJH:<`00UEJH~3B<C0~32Q=KJKMB=Z]00^SEBJM:JKES^007Z9MJ:MJ6Td0~32QEMHJBFJ31^002QEMHJFFJF9`0~2RILK4FFHF6DC00XV9M4FBFHLSX07134:D2`9FF9`0~2R?3:KK9_00N1=FJMM=JMAe0NQGK68P0~5NQGK68PNQGK68P0~4^c_W:91b0~67cc_W:91bc0~3]U9KK3GH6Ia0~3Y>:J66J5AY0~2Y?LMF:;;G3<[00O19:F5;;3<[00N8"
        "LJ;=E<WD[0~4OS9M<U]0~5XS==D==IN0~5^C@=6L>_0~3XS==D==IN0~5C@=6L>_0~4XS==D==IN0~3XS==D==IN0~3XS==D==IN0~4XS==D==IN0~4XS==D==IN00[11LH681_X0[11LH681_X00RILK4HFJMAe00VEB<Q65D1=5AY0~5]Z=H5>A<EU0~4N8LH5<<5H38N00XZL9>W=FH=ZX0UEJ3=~3<C0~3_<KFL>>9=Z]00OI;4;AA;4;?a00R@;H3A<MFEZX00XV9K45E<9331^0XV9K45=<9=;9`00^VEBHG993FJL8O0OIG4;9<93LSX07134:D2`9FF9`00X`9BF639_00eA:46>W;4MAe0NQG468a2O0~3NQGM68PNQG468P0~3PV88>>8~2C0~4OS881>>8~3C0~2OI6HLQWK:AR0~3`9F:AA5FEV00c1=FB=AT?I1Ua00C>:FLATI1Ua00dA:;DTTIVR^0~42UI>>Q_O0~5YVITTQ_O0~4CSI?T?8_O0~3CSI?T?8_O0~42UITTQ_O0~3]dU8?T?1`N0~3PU8?TTIV20~4YVITTQUb0~5YVITTQUb0~3P_8?T?1V[]0^SL364G33Qf^SL364G33Qf0^V9BJ;;<6;Ae0]V=KW13MA>:EZP0~5]V=J:"">`_8C22700RD5HL11LH5DR0~2`IUCCAMHLSX0UEJ3II1VVb0~2N834G?11_Q[0~2a?;431134;?a00CWKF9_IG4GIO00b?G4M<Z[_I91^0b?G4M<S[`Z@9`002?;4:"">U`TG"
        "4:"">Y02T5JEV[U8>ZX07134:D2`9FF9`0XU8EJ:A81e00[<BF9_Z=HMAe0NQG468?TI_P00c1A~2S7NQG468P0~3a?66LS<63?R0~3[>66AddS=6LQa00RA:B<`WKMAe0~3`<F5DS=H31702@543E;5G98200]V=J;9G56WU^00[<B=355G9Ie0~3RQ96553WZO0~3RQ96553WZO0~271=6;5;3WZc0071=6;5;3WZc0~2RQ96553WZO0~22?93;5;L>ZO0~2bT3;55G91O0~2RQ96556<120~3RQ96556<120~2RD3;5;=A_^0^SLKK4KK6Qf^SLKK4KK6Qf0b?;45>5A:AAe00_9F9T;;TL;@ee2]0~3PU>:H6@CZD~2`00CWKJEUUEJKWC0~2Xd?@AEBK9_00UEJ6GGLWSO0~22T5H==3L<820~2P1=F;WW;F=1P00CWBF9_IG45DR00Y>M46Qa00b_e00Y>M46QRV~4C00Y>M438NXVEJF9_0b?;4;E>T1Ca007134:D2`9FF9`0P866HM66>Y00`9FB<CUEJMAe0NQG469G56>`00NQ6~28PNQG468P0~3a?;4LSI6BEV^00N8LH31f]C>MM>[00Y>KKW<LJG?20~3YA:B=>EJ6QN0d>MHLM5K4K9U]071346K5B4;De00`9FBHM:JB=1N00PSEKF55J5W`0~2PSEKF55J5W`0~2713:;;MH5AY00713:;;MH5AY00PSEKF55J5W`0~22D:K5;MH5>`0~2bT55;:HM>Y00cSEKJMMHM9Z^00c"
        "SEKJ:MHM9Z^00RD:M:FJGAC0XZ>>LH6>>1PXZ>>LH6>>1P0Y>MH=93<GI`P00e@6:;MEWM9QAA?_P0~2`>;JBFLIQ6:EU00[<BJ9__9JF<[0~3CW5:BG98200UEJBKJH5<_0~2eAMH5FBHMESP0~2CT=:KK:=TC0~2dAMHL?>:gMAe00C<BH=Z]0~6C<BH=ZUE~48c0C<BH=Z]0[<BJEV0P1=F4FK5LA_X07134:D2`9FF9`0P86B4JBK>Y00[<BF9_Z=JMAe0NQG46MMJH3IO0NQG468PNQG468P0~3a?;4LSV=JG?a00eA:JEUZ~2?;J=S^0CWKJKBJF=I20~22S1=KJMK4GIN0CWBg:<@9K46Ia0c864K9@EFF9U00`9F4;>A3H:AY00eA5JLTDGH38N00eA5JLTDGH38N0071EA?""?WMFEU0071EA?""?WMFEU00eA5JLTDGH38N002D3WT?>;H38N00bTWT?>:B<C00eA546A>;4;T200eA546A>;4;T200RDEAAEBJ=S^0aYSLH68Y200aYSLH68Y200C<BF93E==S^0~2X`D9EWI36?L:MG>`00OI3HG=BBET;B<[00[<FJ9__9FF<[0~3CW55KF;>`00UE=<W=K46QO00eAMgKE<G45DR007Z95BGGB59Z^00OQLJB;5F4MAe00C<BH=V]0~6C<BH=VUEJ~268c0C<BH=V]0[<BJ=V00Y?=5KJ4J6?207134:D2`9FF9`0P1WEJ:WWAY00eA:46A>;4MAe0NQG4BE>3H:DR0NQG468PNQG"
        "468P0~3a?;4LS_9J:DR00[<BB<D=~2Q64GIO0Y>KB66GBB9U0~2C<LL<L66HGQN0C<B4L12?;4:AR0NQG4G?eA:HLSX0CWBJ=Z`<BF9`00[<BB9~2=H5D200[<BB9~2=H5D200]`T9=LL5H=ZX0]`T9=LL5H=ZX0[<BB9~2=H5D200`>~5=H5D200X[TEELBGDe00[<BF9_VEJK>C00[<BF9<<EJK>C00a1D>~2;4GIO00XSLH68P0~2R[SLH68[e00C<BF9;WG<`0~4Xe_UU<K<9BE<M38N0RA:F9?3JM9B5DR00C<BJEUUEJB<C0~2^d?TD9MH38c0[?Z[CI645T200RA:4;?UEJKWC002T;HL?""?LH;T20~2CDL5:;EJ5D200C>K4L1P007RO00C>K4L1UE=G468c0C>KHL170_9FJEU00Rd1D<=;4KWC07134:DR`9FF9`002UEJ:DCC700N1=FJMM=JMAe0NQG4;?UEJMAe0NQG468PNQG468P0~3a?;4LS_9J:AR00_9FBWD:H=864;?a0dAMKW8?6431700CWBK>SQ=H38P0[<BH=VNQ64MAe0NIG4682T5431^0CWBJ9_C>KJEU00_9FHF~4MAR00_9FHF~4MAR00RT6BBM:BHLZX0RT6BBM:BHLZX0_9FHF~4MAR00_9K~54MAR0~22T5BJG9Q200_9FB<C`9FB<C00_9FHK~34B<C0~2eAMKKBg;?a00XSLH68P00^S<<LH6<<8f0C>KJE;A:AR20~6R?63ILM>IG5T20d>MF9_DGHBF"
        "=1P00dAMH=ZZ=HMAd00OZVeb2I;4;?ON1I[2a8345D200b?;4GI_9FBWC00d>MJEUUEJMAe0~2RU8DA?34GIO002T545A[2eVDVX02T545A`VIG468c0RD54;TYRQ3HKWC0OIDZC[Z<FF<[0^SLHM>YUEJB<[0~2UEJ:DR0~4Y?=55=EJMAe0NQG468_9JMAe0NQG468PNQG468P0~3a?;4LSVEJ5T200`9FB<AMHLQ64;?a02@5F9U83431700dAMB9_IGH=ZX0CWKHLSaIG4:DR0c864GIRD5HLSX0Y>KJEV[<BF9_00`9FK9~5AR00`9FK9~5AR00[<BBEAA5HLZX0[<BBEAA5HLZX0`9FK9~5AR00`9FKL~3H:DR00O`?<<EMK<`00`9FB<`U9JKWC00`9FB<>>EJKWC00cUA9~2G4;?a00XSLH68P00^13FFgFF6Qf0RD545396QI?R0~4^V9MW1LM>IG5T202@5H6AI>:g;DC0~2aI64;DDG46Ia00RD9A?""?<"":46IOa?3W?""?<"":4G?a00P1LH5D8=H:Ae00RA:HLIQLH:AR00]Z9DQI9MFEVX00P1=FJ3>TA931^0P1=FJ6>TDG468c0c1=JF=AD9M46?20a?;E>TD=JKWC00U9FF=DAG4:Ad0~2UEJ:DR0~4`WITTI3H5T20NQG468_9JMAe0NQG468PNQG468P0~3O?;431T646QO00CWKJ=<B:GA5438f0NQ34GAA5J=V]00aIGH3TWMB<`00R@;4;A8WM4GIa0XZ=J:"">89BF"
        "9U002T;46@?LJM>C00Y>:J=I1I>=1700Y>:J=I1I>=1700_9FK>1<KHLZX0_9FK>1<KHLZX0Y>:J=I1I>=1700Y>MF91?646IO00[<A81T;JEU00Y>MHL?T345DR00Y>MHL?T345DR00RDWII9KHL1P00XSLH68P00^1==LH6==Qf0P1=JJ=LE>LAe0~4e@;L8UEFEW:3QO0P1=BJ5G:JKJ3?d00]U<MJ;;JM9U]00RD::;;M4KEVXa?;K;;M4K9VX0~2[>5JG3MFL8O00aI3HK66KH3IO00]Z=56GKJ6Te0~3Y@6FHM5:B31^00Y@6FHM5:BH68c00CAGJF:5BH5W_]0a?;JM55FHG?200dD;HF::JFL8O0~2UEJ:DR0~4_96E9LKF=1P0NQG468_9JMAe0NQG468PNQG468P0~3c83H:L6FK9U]002T;4B:MLF:HFEV]00U<MJ;;JMWC0~2]U9MF;5HG?200PSEKJ;6:4K9VX00CA;H56:4;Ad00PSEBF;GBJLIa00O8LFK636:31700O8LFK636:31700CWMH;35:HLZX0CWMH;35:HLZX0O8LFK636:31700O8LF:36FM9U]00[<"":63GFK<`00O8LFBG;FB=1c00O8LFBG;FB=1c00RD:GGM45>[0~2XSLH68P0~2CZSLH68Z[]00CAGJFK5MMAe0~37ZE:"">[eT3KK;>_0~2d?=5KK5LE55=Qa002895KK5E820~2RDG:KBM;EIR0a?L;MBK;EQR0~3NS<;KK:LTC0~3`A3:KK:3A`0~2]Z"
        "=:KK:LA`]0~3X`D=5KBM;ES^00X`D=5KBK53<1P00^_AL:KKM6WZO00a?=;MKKM6>UX00^U>6MKK5LTC0~3UEJ:DR0~4_9KFJFMLTY00NQG468_9JMAe0NQG468PNQG468P0~4`>G:KM;EI20~2^V<5KMLW;KM3Te0~2b89;MK5E8a0~328E5KM6>_X0~2RQ9;MKK5EI20~2PVW6MKK;<VP0~2eIE5KK:LDC0~3CT=5KKM;=170~2CT=5KKM;=1700aQ=MK5955LZX0aQ=MK5955LZX00CT=5KKM;=170~2CTL:KK;9820~2[<5KBK5=I20~2CTL:KK5=?d0~3CTL:KK5=?d0~2RD;KBMG<SO0~2XSLH68P0~3XSLH68P0~3^_<;5BK:3Ae0~3OQWW1O][?>W@VN0~3d1DWWD1I@~28O0~22SDWWDS20~3NVIA>W>TZR00X[1T>W>TSR0~5aVT>WA8C]0~37`IAWWAI`70~3[IAWWAQC^0~5XY1@>W>?ZR0~3XY1@>W>D8_b0~37C8AWWAI_O0~2]CZ?AWWAI_f0~3N_IAWWA8C]0~3UE55DR0~4CTW9E9>QC]00NQG568_955Ae0NQG568PNQG568P0~4fU?A>>TSR0~4aZDW>IVDW>IC]0~32ZT>>D1R0~52SD>>?_f0~4RZ?>W>D1R0~4OU?>W>TVa0~4e1D>WA8CX0~3]Y1DWWA?SY0~3]Y1DWWA?SY0~3d8>WA8@~2U0~2d8>WA8@~2U0~2]Y1DWWA?SY0~3]C8AW>TZ20~3e8D>W>D1d0~3]C"
        "8AWWD1d0~4]C8AWWD1d0~3OST>W>?Ua0~3XS=~28P0~3XS=~28P0~3]Z=LE3WA8[P0~4RCC20~2bYCR^0~5fRCCRNb2~2a0~4PRCCRP0~5XaRCCY270~4P2YCY270~7^2CCeO0~7OeCCeO0~6ORCCeO0~9P2YCYb^0~6P2YCYRN0~7NRCCda0~6^aeCCda]0~6adCCRN0~5`@~2I20~4X2C_U_Ca0~3PS@~2Z[@~2?20PS@~2Z^PS@~2Z^0~5]aedd2P0~6PRCYa7RCYb0~672YYRc0~7PRYdb]0~6^2YCCRP0~6]bdCY2^0~6PRCCeO0~7NRCCdb70~6NRCCdb70~5OYCea2~2P0~3OYCea2~2P0~4NRCCdb70~6OdCY270~5ORYCYRf0~6NRCCRf0~7NRCCRf0~572YCYbX0~5YV~2C0~5YV~2C0~5C1~3`RO0~}0~}0~V72~2b0~BO2~2N^2~2b0~2O2~2N00O2~2N0~}0~}0~}0~9]~20~7]~20~77~30~}0~}0~}0~PXc~2^a2O0~7PbbPc~2P0~2^c~2X00c~2P0~2Xc~2X0c~2P0~5XccP0~3aaO0~6XccP0~3aaO0~6XNOOP0~3aaO0~6]c~270~7N22O^0~3N~2X0~6N~2]0~4N~20~L^Ob2bf0~7]O22N0~W]]00]0~VN~2]0~3Xc~2^a2O0~5cNN0PNN0~4Xc~2Xa2O0~:X7~90~7N~2]00Xc~2^a2O0~3cNN0PNN0~5O22ac0~8cNNP00^c~2]0c~2P0~9N~20~=N~2]0~?cNN0PNN0"
        "~F2Y~220~6^U8~2Z?TQ`P0~4Y1?""?18~21a07V8~2_]Y8~21b0^U8~2UY8~21b0~2P[Z88Sa00YI?Id0~3P[Z88Sa00YI?Id0~4aZQ?I1C]0YI?Id0~5[8~2ZN0~42UQTTIZd00dQII_X0~4CI~2[0~3R8IIC0~3R__[__Y0~37CUC[U`70~3CSI?T?8_O0~42UITTQ_O0~4bY~220~77CUC[U`700aY~420][ZVbUZ_702Y~22NNP002Y~220~6aY~2R0~7CI~2[0~2^U8~2V?TI_P0~2a1I8R1I8R0~2^U8~2V?TI_P0~5^R_Z1~9C0~5CI~2[0^U8~2V?T8b00a1I8R1I8R0~2aUQTT?8V20~5b1IIZfPV8~2`C8~2SO0~7R8IIC0~;CI~2[0~4RY~2a0~4a1I8R1I8R0~4OeY~2c002Y~220~2c1>~2170~5NQ6~29;53>_X0~2Y?=;;=E66Ae0OQ366IaVE66Ae0fQ6~28_966>d0~2dA=66AR0cS=;>C0~3dA=66AR0cS=;>C0~4Y>6;;=TecS=;>C0~4XZL66T20~328<655G=Z]0[>GGTd0~32ILG9V0~2cSEGLQa00XS99D99IN0~2[AE>?EDR0~271=6;5;3WZc0~2RQ96553WZO0~2Y1>~28``O0~4[AE>?EDR0XZ>~48ffQ=<UA=T2c1>~21IIZfc1>~2170~4]V>~2Ia0~52ILG9V0~2NQ6~29G56>`0~22TGE1<G>C0~2NQ6~29G56>`0~4bVD9=3~8LZX0~32ILG9V0NQ6~29;5"
        "AR002TGE1<G>C00PZ<655;39`0~2bC~2>G6Iaa?6631UE66DR0~6cSEGLQa0~92ILG9V0~3OI>~2ZX0~32TGE1<G>C0~2]CIA>~2`0c1>~21700NQGK68P0~5NQG46M:FHGTR00f1=FJMM=JMAe00U9BM>CI6438N0fQ6468_9JM>d0~2d>3=:AR0CA5=1f0~3d>3=:AR0CA5=1f0~4Y>E<=F9`CA5=1f0~4XZLH5T20~2PSEM4K:K=Z]0fV<"":=1a00PV9KE1b0~2CA5=M<UX0XSL:D;GIN00P1365E5T20~2713:;;MH5AY00PSEKF55J5W`00OI93KG99<`0~3P1365E5T20XSLK~2GIN^S=6E36IONQGK6>G6IaNQGK68P0~4XZLK5Tb0~4PV9KE1b0~2NQG46MMJH3IO00XU<M<;ESc0~2NQG46MMJH3IO0~2a19;BH4H4~2H~3LZX0~2PV9KE1b0NQG46:F:AR00XU<M<;ESc00eA5J5;5B9`00]VW~29F9U]a?;431UEJ:DR0~6CA5=M<UX0~7PV9KE1b0~3a?;KLS^0~3XU<M<;ESc0~2R?3:KK9_0NQGK68P00NQG468Pc~300NQG4K9A=FB9_00eA546>W;4MAe00RD5JEZA:B9_00fQ6468_9JM>d0~22Q8=:ARO1=;AY0~42Q8=:ARO1=;AY0~5bS><=:W[1=;AY0~5XZL:5T20~2RD54;<DW=Z]00OS9M<U]0CA;=Q20~2bQL3?=GTe0XS==D==IN0XOI6><6E1P0~271EA?""?WM"
        "FEU00eA5JLTDGH38N0OI;KgBKK9`]0~2OI6><6E1P0XS<~4QN0C@=6L>_0NQG469F9U]NQG468P0~4XZLH5Tb0~2XcCA;=Q20~3NQG4BE>3H:DR00XbQL:GTe0~3NQG4BE>3H:DR00]U<M4F;3L64:L~4ZX0XcCA;=Q200NQG4F3<9AR00XbQL:GTe0~2CWB5A?D<9`00^Z=B5=5De0a?;4L1UEJ:DR0~3ccbQL3?=GTecc^0~2PcCA;=Q2X0~2Pa?;MLScX0~2PbQL:GTeX00X`9BF639_0NQGM68P00NQG468Y8~3[0NQG4G?dA:HLSX0[<BF9UZ=HMAe00P1=JG?9F5DR00fQ6468_9JM>d0~3OZ=:AR[>:EZccX0~3OZ=:AR[>:EZOOP0~4C<G:LD[>:EZccX0~3]UD~2Qa0~2[<BF9VR[Q`0~2^U8>>ZX^U>>IUX0~22?>D8?>AY00YV8~2V[]^U8~21?TI_P0~2]`T9=LL5H=ZX0[<BB9~2=H5D20N8>34G>~2IUN007V8~2QSR0~22U8~2Ue00^U8~2UP0NQG46=5De0NQG468RR20~2XZLH5T1O0^U88>>?TI_P00NQG4;?UEJMAe0^U88A>>TI_P00NQG4;?UEJMAe00aI64BET11345@S~3C0^U88>>?T8b0NQG45D`UVN0^U88A>>T8b00C>KK3E<DSR0^V8=H5>>8[0a?;4LSUEJ:DR0~2C8~2?>D8?>A8~2U^02188>>I88_]0218QA~288_]02188A>>88_]XU8EJ:A11e0c1A~2S"
        "700NQG468IL669V0NQG4682T5431^0`9FB<CUEJMAe0~2[WMKW3HL1c00fQ6468_9JM>d0~27`Z=:AUQ3;TS88UX007`Z=:AUQ3;T1?""?1C]00YI8IWKEQ3;TS88UX0~2^ZLGGT20~2_9FB<C00N]0~2NQ6~28PNQ6~28P0~3RQ6~28e^00NQ6~28P0NQ6~29G56>`0~2RT6BBM:BHLZX0_9FHF~4MAR002134G<G56>U]00NQ6~28P0~2NQ6~28P00NQ6~28P0NQG46>>1P0NQG46A~2Ib00XZLH5LW`0NQ6~29G56>`00NQG468_9JMAe0NQ6~29G56>`00NQG468_9JMAe00d>M46?eP1345W~3>[0NQ6~29;5AR0NQG468f]]00NQ6~29;5AR002?3KJJB59V]OI66H:66=Z]a?;4LSUEJ:DR0~2UE66DVE66Q8366QN0Y>6~53170Y>6~53170Y>6~5317P866H:DR^00NQ6~28P00NQG46Q=BM9120NQG4GIRD5HLS^0[<BF9`VEJMAe0~2bI6J=5B<_0~2fQ646Q_9JM>d0~22DE=:EE<M9VW668c002DE=:EE<M9>3;;=Te00`99<=FE<M9VW668c00^`A;4GIa0~2`<FF9_NeVY0~2NQG468PNQG468P0~3NQG468P0~2NQG468P0NQG46MMJH3IO00[<BBEAA5HLZX0`9FK9~5AR00P134G:MFHG?b00NQG468P0~2NQG468P00NQG468P0NQG468db00NQG46=M5@200NZLHF59S^NQ"
        "G46MMJH3IO0NQG468_9JMAe0NQG46MMJH3IO0NQG468_9JMAe00C<BH=ZXP134FB~3<[0NQG46:F:AR0NQG468P0~3NQG46:F:AR00N_?<=3546IOOIGB4FBB=Z]a?;431S=H:DR0~2CWKKW1345@T;J=ZX0Y>KB~2H43170Y>KB~2H43170Y>KB~2H4317P86B4:DR0~2NQG468P00NQG46EKM91200NQG4:WI9BFEU00Y>MHL?DG4MAe0~2]VEF5B5@R0~2fQ645A?LHK>?R002D6~3E66?T6568c002D6~3E66?>L<=F9U00`96;;=T66?T6568c00CD6J5<U]0~2YA:H6DQ@EZ]00NQG468PNQG468P0~3NQG468P0~2NQG468P0NQG4BE>3H:DR00_9FK>1<KHLZX0Y>:J=I1I>=1700P134F=>LHM>d00NQG468P0~2NQG468P00NQG468P0NQG468P0~2NQG46=H5@20bSWG45WS20NQG4BE>3H:DR0NQG468_9JMAe0NQG4BE>3H:DR0NQG468_9JMAe00C<BH=V]P134B:~3<[0NQG4F3<9AR0NQG468P0~3NQG4F3<9AR00d>>I11=H;?aNQW=H5W~2V]OIG4GDA;4:DR0~22T5JE?;:KW>MB<[00YAW~2EBKE170YAW~2EBKE170YAW~2EBKE17P1WEJ:DR0~2NQG468P00NQG46MKE820~2NQG4;:GM45AY00OQ3JB;;6JMAe0~3dA5gHL1P0~2fQ64H5;BMJ;>d00OSQ~39K<1EL;6"
        "8c00OSQ~39K<U?8U<B9U002ZQ?I19K<1EL;68c0f8LJ:<SObX00O8=FJ5G5=Z]00NQG468PNQG468P0~3NQG468P0~2NQG468P0NQG4;?UEJMAe00CWMH;35:HLZX0O8LFK636:31700P1345@`9FKWC00NQG468P0~2NQG468P00NQG468P0NQG468P0~2NQG46=55@20C<5545Tb00NQG4;?UEJMAe0NQG468_9JMAe0NQG4;?UEJMAe0NQG468_9JMAe00Y>K438aP1345D~4C0NQG45D`UVN0NQG468P0~3NQG45D`UVN00d>MG33:H38N0RZ=H5TCCb071=JF556H:DR0~2N8343>M=KE9F:AR007CCS9MF=IR007CCS9MF=IR007CCS9MF=IR002UEJ:DR0~2NQG468P00NQG4F4GA[]0~2NQG46=MK5<ZP0~2[A6KK6EJMAe0~3f1LHK<`0~3fQ646:K3LKM>d0~2PffR?G3IW;A;68_]00PffR?G3I22V>;GDd0~2XNaR?G3IW;A;68_]2T5HL1RVT[0~2Y?=5KK:=Z]00NQG468PNQG468P0~3NQG468P0~2NQG468P0NQG468_9JMAe00aQ=MK5955LZX00CT=5KKB;=1700P134GI[<FKWC00NQG468P0~2NQG468P00NQG468P0NQG468P0~2NQG46T~28a0a8=LH5Tb00NQG468_9JMAe0NQG568_955Ae0NQG468_9JMAe0NQG568_9JMAe002T;4M<1_1345@[~3R0NQG468f]]0"
        "0NQG568P0~3NQG468f]]0~2d>G:KBM6>`00]Z=H5T20~3CAGKK3E5:De0~2]VEJ;9B<5LLHGIO0~2PVW5J3?d0~3PVW5J3?d0~3PVW5J3?d0~3UEJ:DR0~2NQG468P00NQG465J3TC0~2NQG468>WDZa0~37`?>>?EJMAe0~2R_U9F;T20~3fQ646>W?""?W>Q20~4^V9MWSL335;38P0~3^V9MW`OZW;6AVe0~4^V9MWSL335;38P2@5HLTD99_0~3Y1DELAI[0~2NQG468PNQG468P0~3NQG468P0~2NQG468P0NQG468_9JMAe0~2d8>WAWG@@U0~2]Y1DWWGTSY0~2P134GI[<FKWC00NQG468P0~2NQG468P00NQG468P0NQG468P0~2NQG468bbO0~2Y1LH5Tb00NQG468_9JMAe0PS@~2ZV@~2?20NQG468_9JMAe0PS@~2ZUEJMAe007ZEB4:L9<345<~48cNQG468P0~3PS@~2ZO0~3NQG468P0~4aS?AWELI_P00]Z=H5T22X00P_TW>I@WGTCO0~2[<BM=MA6;GH=S^00^_>;H6DC2N00^_>;H6DC2N00^_>;H6DC2N0~2UEJ:DR0~2NQG468P00NQG469MFL?d00NQG468YCRP0~5]2CCUEJMAe00]Z99GJ=S70~3fQ6468C22CYO0~5dD;L8S=~2;6=8P0~3dD;L8aCW5;==EV0~4dD;L8S=~2;6=8POI3HB55B9_0~3dAA>5D20~3NQG468PNQG468P0~3NQG468P0~2NQG468P0N"
        "QG468_9JMAe0~3OYCZ=LDW_0~4NRZ=LDW_0~2P134GI[<FKWC00NQG468P0~2NQG468P00NQG468P0NQG468P0~2NQG468P0~4XZLH5Tb00NQG468_9JMAe00O2dA==>Y2b00NQG468_9JMAe00O22I99G4;?20~2R?=KHHFBFgJB~368cNQG468P0~4dA==>Y0~3NQG468P0~5^dAA>5D20~2]Z=H:ADTC0~2X2CYbZ=LDW_0~2RD5FG;?=M:B9`0~2CD6H;AD~2Z^0CD6H;AD~2Z^0CD6H;AD~2Z^00UEJ:DR0~2NQG468P00NQG468EBB=QR0NQGF68P0~<UEFMAe00]Z=FF;AC0~4fQ6F68c0~9PSE:"">CYVV?668C0~3PSE:"">C0CW6~3EV0~3PSE:"">CYVV?668C00`>3:M:G9_0~3d>:M6?b0~3NQG468PNQG468P0~3NQG468P0~2NQG468P0NQG468_9JMAe0~5]VE::9_0~5]VE::9_0~2P134GI[<FKWC00NQG468P0~2NQG468P00NQG468P0NQG468P0~2NQG468P0~4XZLH5Tb00NQG468_9JMAe0~3_9F=1O0~3NQG468_9JMAe0~3OIGFJM9Z^0~3YQ<3;5~868cNQG468P0~4_9F=1O0~3NQG468P0~6d>:M6?b0~3_9FJ::9`0~6]VE::9_0~2OI644319FgMAd0~2`9F4:~33170`9F4:~33170`9F4:~331700UEJ:DR0~2NQG468P00NQG568IL55E1NNQ<~21P0~<_"
        "<~2Ae00]ZEE9AU70~4f89~28c0~9OIWW1N00PVQQUX0~3OIWW1N0R8Q~4C0~3OIWW1N00PVQQUX00P`IA>AIVR0~3RIA>?`]0~3NQG568PNQG568P0~3NQG568P0~2NQG568P0NQG568_955Ae0~6R8AAT[0~6R8AAT[0~2P135GI[<55WC00NQG568P0~2NQG568P00NQG568P0NQG568P0~2NQG568P0~4XZL55Tb00NQG568_955Ae0~2P1LGTd0~4NQG568_955Ae0~3OIE~2W120~5b_1?@~9S7NQG568P0~3P1LGTd0~4NQG568P0~6RIA>?`]0~3RTL5~29`0~7R8AAT[0~2XZ=55EU>55;?a0~2`95~53170`95~53170`95~531700UE55DR0~2NQG568P00PS@~2ZY?@~21f0d[~2R0~=a[~2C70~2eUU_Y70~6d`~2e0~;RCC20~3^ff^0~5RCC20~2cf~40~5RCC20~3^ff^0~4ORdRaX0~5aRd2X0~4PS@~2Z^PS@~2Z^0~3PS@~2Z^0~2PS@~2Z^0PS@~2Z[@~2?20~7Oed2X0~7Oed2X0~2^Z@~2SCT@@?e00PS@~2Z^0~2PS@~2Z^00PS@~2Z^0PS@~2Z^0~2PS@~2Z^0~5U@~28O00PS@~2Z[@~2?20~2XUII_X0~4PS@~2Z[@~2?20~4CU~2Ca0~8Pa2~9O0PS@~2Z^0~3XUII_X0~4PS@~2Z^0~7aRd2X0~4]C8T@@TC0~8Oed2X0~3[@~2TCI@~2Z^0~2CT@~6Z^0CT@~6Z"
        "^0CT@~6Z^00`@~2I20~2PS@~2Z^0~2O2~2N02~3O0~}0~lO2~2N00O2~2N0~5O2~2N0~4O2~2N0~2O2~2N^2~2b0~NN2~2OX2~30~3O2~2N0~4O2~2N0~3O2~2N0~2O2~2N0~4O2~2N0~6P2~2a0~3O2~2N^2~2b0~4^OOX0~6O2~2N^2~2b0~SO2~2N0~5^OOX0~6O2~2N0~DN2~3X0~A^2~3Xb2~2N0~3X2~7N00X2~7N00X2~7N0~272~2b0~4O2~2N0~5XN~20~;]N~20~;]N~20~=]N~20~7]N~20~?]N~20~@POON]0~:POON]0~5Xcc]0~27c~2]00Xc~2^a2O0~AXc~2X0c~2P0~67a22N0~8N22O^0~6PP]NO^0~4]ccP00Xc~270~2Pc~5Pc~20~4]N~20~?Pc^0~2Xf70~5]O22O]0~8^Pc~2X0~?Pcc7007ccP0~6^O2Oc~2P0~5XO22N0~7^a22c0~9POON]0~:7OaO^0~37c~2P]c~30~6^a22c0~6XNaOP0~;7OaO^0~5XNaOP0~;7OaO^0~6cOON^0~9]`I~2C7^0~8`I~2C7~50~4`I~2C7^00X7~2]0~3`I~2C7^0~4`I~2C77]0~27~30~4`I~2CPOON]0~82U1I?8Ve]0~62U1I?8Ve]0~2^U88VR0fZ8~2[0^U8~2V?TI_P0~2XeYYRP0~5^U8~2UY8~21b0~3]YS?TT8C^0~42UQTTIZd0~3O`11UQIZR0~3`881`OU8~2Zf0a18~518~2Y0~3`I~2CX0~4^^0~47`88S"
        "[cRZQ1R0~32VITTQUb0~5b`S8~3U^0~2bY~2N0~5R8~2ZNNZ8~2R0~4eZIT18~21a0~3RVITTQUb0~3]dZ?TT1C^0~52U1I?8Ve]0~6a`1I?IZCN0NZ8~2S_8~3d0~3]dS?TT1C^0~2PYZQ?I1_20~7a`1I?IZCN00PYZQ?I1_20~7a`1I?IZCN0~2bU8?""?QS_20~7R?6GW11S20~6R?3G<1~6[0~2R?3G<11ZOcV1~2_]00R?3G<11ZO0~2R?3G<1~2U^0e1~3C0~2R?3G<_1I?QVe]0~5YI<3;;6ET`^0~4YI<3;;6ET`^00NQ669?ReD66EZXNQ6~29G56>`0~2eD>>DS20~4fQ6~28_966>d0~2][T=;55LA_X0~228<655G=Z]00OZWLW96GE?d00^SL63WZ?6~2?a0RA66E=66A66EU0~2R?3G<1ZCO02_1SC]0~2RA36L>U8E6E1P00RQ96556<120~328<L6~3QN00^S>~2_0~5U966?bb?669U0~3Y?EG;?E66Ae0~22Q9G556<1b0~2[TE;5;LA_X0~3YI<3;;6ET`^0~4R8<LG;6=>VXbT6~2W?6~29`0~3[T=;5;LA_X00_AE6;G3<IY0~5R8<LG;6=>VX0_AE6;G3<IY0~5R8<LG;6=>VX028<3;;6L9SX0~5O1EM93~2<[0~5NSEM93~6LS^0NSEM93~2DR2T3~28P0NSEM93~2DR00NSEM9A3~2DYcSE33EZX0NSEM9I<3;;6ET`^0~3RILK4FFHF6DC0~3RILK4FFHF6DC00N"
        "QGBBEV_9BM>C0NQG46MMJH3IO00d>MK59Sf0~3fQ6468_9JM>d0~2R?3FB:FJGTR00PSEM4K:K=Z]00`W5KW:HBB=8O0^SLMH5A9BB9U]0RAMJE=H5AMJEU00NSEM933=>SC?9LLTY0~2RA:MH;T<M4;?a0PSEKJMMHM9V^00PSEMHFBBGQN00b?;KW[0~4c13HL1771LH38c00f1=BJ;?EJMAe00^Z9KK;5JM<U00R?3FB:HJGTR0~2RILK4FFHF6DC0~32QEMHJBFJ31^bT54H39B4F9`0~2R?3FB:HJGTR00V=JFBJHK=?R0~32QEMHJBFJ31^0V=JFBJHK=?R0~32QEMHJBFJ31^XV9M4FBFHLSX0~5_W:9EF4H=S70~4`>:E1344H~3LS^0`>:E134:DR2T5468P0`>:E134:DR00`>:E1ZEBF=1CA54;@d00`>:EI=K4FFHF6DC0~2^VEBHG993FJL8O00^VEBHG993FJL8O0P1AEFG?13H6Ia0NQG4BE>3H:DR00d>3GJ5AY0~3fQ6468_9JM>d0~2U9FK9A=FF9U00RD54;<DW=Z]0N83HGEJ:9B:"">Y0XVA>5J=G4G?200RAMJE=H5AMJEU00`>:E13HJ:9?LBHF=ZX00RA9>MM>34H38N0eA546A>;4;T200eD54;9W~28f00C>M;T20~42T;J9UCCU9F;T200eA546>8EJMAe002T;5=?D6431P0U9BK9>LFF9U00^VEBHG993FJL8O00XV9K45E<9331^bT545K;MFF9`0~2U9BK9>"
        "LFF9U00V=69<E;HBESP00XV9K45E<9331^0V=69<E;HBESP00XV9K45E<9331^OIG4;9<93LSX0~5V>>I64MH;T20~4U>>Q134:L~4S^0U>>Q134:DR2T5468P0U>>Q134:DR00U>>Q2RTG4:"">8LJK9UX00U>>QEBH;9<3FJLQa002?;4:"">U`TG4:"">Y002?;4:"">U`TG4:"">Y00aYA:KWD5F9U]0NQG4;?UEJMAe00aZ1A5JEVX0~2fQ6468_9JM>d00P134G?dA:HLSX0[<BF9VR[Q`002@5J==H5AMF9`00OR1LH;BB9U]00RAMJE=H5AMJEU00U>>Q1=L:4;9BFGJLS^00b81T5M=:4;>`00[<BF9_VEJK>C00[<BF9VC~2R00]VEJL1P0~4eAMB<TWWT<BMAe00[<BF9UUEJMAe00bQ?~2YVEJ68PP134G?[A:HLSX02?;4:"">U`TG4:"">Y00b?G4M<Z[_I91^bT5H=:F=KF9`00P134G?[A:HLSX0V9IU[V>54;DR00b?G4M<Z[_I91^0V9IU[V>54;DR00b?G4M<Z[_I91^2T5JEV[U8>ZX0~5OdYA:FEBK<[0~4Odd2134:DS~3C00Odd2134:DCCT5468P0Odd2134:DR00Odd20]U<MHL<MH3I20~2Odd?;4:"">U`TG4M>C00Y>M43W~3EJF9_00Y>M438NXVEJF9_0~2OQ3J=<B5Ad00NQG468_9JMAe0~2^N834GIa0~2fQ6468_9JM>d00NQ64682T5431^0_9FB<C00"
        "N]00RA:JE=H5AMJEU0~3`<K44G?20~2RA:JE=H5AMJEU00Odd2[Z1W5K3JLW;<_0~3NV>MJF45WVc00_9FB<C`9FB<C00_9FB<C0~5OI6B<`CCY700Y>KKWD::AWKK>Y00`9FB<CUEJMAe0~2Ob~2[DGFEVXNQ64682T5431^0Y>M43>~3EJF9_00Y>M46Qa00b_e0bT5H==3>KF9`00NQ64682T5431^02_200N1LHK<C00Y>M46QA~2I_e002_UA~2ILHK<C00Y>M46T~4_e0b?;4;E>T1Ca0~7]VEFM>5H=S70~6^134:W~3>`0~4^134:W~35468P0~3^134:DR0~7bQLJM6H:"">[0~5Y>M438N]UEJFEU00C<BH=3BB;<BJEV00C<BH=Z]0[<BJEV0~3U9F;LHL8N00NQG468_9JMAe0~3O834M>C0~2fQ646Q_9JM>d00NQG4GIRD5HLS^0`<FF9U2O0~3RD:H==H5WKF9_0~32T;4B9VX0~22D5H==H5WKF9_0~6XOS=JF5AZISa0~2]_>;HK3J31VC00`<FB<`U9JKWC00`<FF9_b]0~3dA:5W~3AY00dAMB<AMM><BM>d00[<BF9`VEJMAe0~4^[D3J6Td0NQG4GIRD5HLS^0C<B4K~54JEV00C<BH=Z]0~5bT5H=11>KF9`00NQG4GIRD5HLS^0~6UEJF9_00C<BH=Z=M5@20~3]V=M5@EJF9_00C<B45~49`00P1=F4FK5LA_X0~6OI64GI3H;T20~6^134JB~39`0~4^134JB~3F46"
        "8P0~3^134:DR0~8C>54HF=1O0~5C<BH=Z]0[<BH=V00C<BH=3::;<BJ=V00C<BH=V]0[<BJ=V0~3eA5K5B<`0~2NQG468_9JMAe0~3YA:gJEZX00fQ645A?LHK>?R0NQG4:WI9BFEU00YA:4GWD8Y0~2OI64;=H5LJM>C0~32?645DR0~3OI64;=H5LJM>Y0~8`9B46QNOP0~32?6HJEEJ6898P0YA:HL?T345DR00YA:4G>?_P0~2U9FFB~25Ae002@5J=<KK9=J5D200Y>MHL?DG4MAe0~3P`D3J;A`X0NQG4:WI9BFEU00C<BHG~5FJ=V00C<BH=V]0~5bT5H=ZY>KF9`00NQG4:WI9BFEU0~7_9JF9_00C<BH=V=H5@20~3]V=H5@9JF9_00C<BgF~49`0~2Y?=5KJ4J6?20~6YA:J=Z9FK<[0~6^134B:~39`0~4^134B:~3K468P0~3^134:DR0~8fSEF4;DY0~6CWBH=ZX0`<FJEV00C>KHLD~39FJEU00C>KHL170_9FJEU0~3N83H4;TR0~2NQG468_9JMAe0~2XVEJJ4G?b00fQ64H5;BMJ;>d0NQG4;:GM45AY00O8=FHK5L?R00]U9MJ:4KBFLIa0~3_<KgB9_0~3]U<MJ:4KBFL8O0~8Y>K4LS^0~5C>Kg:"">9BM33170O8LFBG;FB=1c00O8=FH:G>`00c13:~346QO00f8LJK:;;MKHL8N00OQ3JB;;6JMAe0~2OVA6J;>UP00NQG4;:GM45AY00C>KHLI~39FJEU00C>K4L1P0"
        "07RO0bT5H=ZY>KF9`00NQG4;:GM45AY00fRc00^Z=HB<[00C>K4L1=55@RO00fRV=55@=HB<[00C>K43<~4`O00Rd1D<=;4KWC0~5]VEFK<~2:H=S70~5^134:D~4C0~4^134:D~35468P0~3^134:DR0~9d>MHLZ^0~6eA:46IbPS=JB<`00RD54;TYRQ3HKWC00RD54;TYRQ3HKWC0~4_9BJ=S70~2NQG568_9JMAe0~22?GJ=FM>C00fQ646:K3LKM>d0NQG46=MK5<ZP0~2Y?EG5:B9_0~2219654K;=TC0~3bI64F438O0~3b1<654K;=?C0~9d>MHLZX0~5eA5:E1?L5H=ZX00CT=5HH5=?d0~3Y?=;5F38c0f1D~33JEV]0~2[A35;99;FMW`0~3[A6KK6EJMAe0021<;J6>UYSC0NQG46=MK5<ZP00RD54;TYRQ3HKWC002T545A[2eVDVXbT5H=ZY>559`00NQG46=MK5<ZP00UAZd2C@;4:Ad002T545AT~28DVX0UAZT~2@;4:Ad002T545A[~2VDVXOIDZC[Z<FF<[0~5OI64HF~2J4;T20~5^134:D[~3e0~4^134:DRRT5468P0~3^134:DR0~9d>MHLZX0~628LFK<V`A5HGTd00c1=JF=AD9M46?200c1=JF=AD9M46?20~4d>KF9`0~3PS@~2Z_9JMAe0~2[<KKW5J=ZX0fQ646>W?""?W>Q20NQG468>WDZa0~4dZ?T>M=V]0~2bUI=H5?SY]0~3_<KFEJ:AV`^0~2bUI=H5?"
        "Sd0~:d>MHLZX0~5cVDA1ADILF9_0~2]Y19FF91d0~5dS?D6;?a00aR~2T5K>C0~37`I@?ZA3B=1O0~37`?>>?EJMAe00`9:43DV1D=S^fQ646Q>WDZ[^00c1=JF=AD9M46?200P1=FJ3>TA931^bT5H=ZR?@@TC00UAG46A>WDZa0~2V=EAT>LFJL8O00P1=FJ3>TA931^0V=EAT>LFJL8O00P1=FJ3>TA931^a?;E>TD=JKWC0~5YA:H;~4FK<[0~5^134:<~48N0~3^134:DR2T5468P0~3^134:DR0~9d>MHLZX0~5XZ<<LJ;WT6JG<<IO00CAGJF:5BH5W_]0~2CAGJF:5BH5W_]0~4d>KF9`0~4O2~2N_9JMAe00P1LHG?34G?b0fQ6468C22CYO00NQG468YCRP0~6^a?EKEV0~4]Z=H5?b0~4bI3H;A5J3=?b0~3]Z=H5?b0~;d>MHLZX0~6PRRY>5GK:AY0~4_9FF9`0~77VE;GIO0~4[<K;?20~47VID9;:EIe0~5]2CCUEJMAe0028EKB3=35LS^^S=J:9~4@20~2CAGJF:5BH5W_]0~2Y@6FHM5:B31^bT5H=Z]2~3X00V=M4M::9`P0~3V=F:5MJJGAC0~3Y@6FHM5:B31^0V=F:5MJJGAC0~3Y@6FJM5:B31^a?;JM55FHG?20~4]VEFB<?~2>MH=S70~4^134JB~3GQN0~3^134:DR2T5468P0~3^134:DR0~9d>MHLZX0~5XSLBB4KWT5gFB;IO00^_AL:KKM6WZO0~3^_AL:KKM"
        "6WZO0~5d>KF9`0~9_9FMAe00eD5JEZ9BM>C0fQ6F68c0~6NQGF68P0~:O?;;AC0~4]Z=F5?b0~4`<KFE1EMF5?b0~3]Z=F5?b0~;d>MHLZX0~9Y>;BM=1O0~4_9FF9`0~8V=M9ZX0~3XZ=JL170~4OIL5:3W1R0~:UEFMAe0~2RQ9;BFMG9ZX0[>;F~4:D20~2^_AL:KKM6WZO0~3X`D=5KBM;ES^bT5F=Z]0~6VEGF6EE9`0~4V9GMKK5LA_70~3X`D=5KBM;ES^0V9GMKK5LA_70~3X`D=5KBM;ES^a?=;MKKM6>UX0~4OI65;TROaI65;T20~4^135~5GQN0~3^1355DR2T5568P0~3^1355DR0~9d>55LZX0~5XSL5~3WT5~3;IO0~27C8AWWAI_O0~57C8AWWAI_O0~6dA<~2`0~9_<~2Ae00U955>CD55=Z^f89~28c0~6NQ<~21P0~:OI<AUP0~4]Z<~2?b0~3XS<~2De8><<?b0~3]Z<~2?b0~;d>55LZX0~9aV@<WQd0~5`<~3`0~8V<>820~4aIG59_0~6CA<A8`O0~;_<~2Ae0~32ZD<<>?U200PUD9E~4D20~37C8AWWAI_O0~5XY1@>W>?ZR0b?<~2Z]0~6RQ<~21UUb0~42V?AW>D1C70~5XY1@>W>?ZR002V?AW>D1C70~5XY1@>W>?ZR0]CZ?AWWAI_f0~5O8@~2SP0XV@~2I20~4XV@~7SP0~3XV@~2I2a8@~2Z70~3XV@~2I20~9R?@~2U0~6]U@~4?8@~41f0~4NRCCda0~9"
        "NRCCda0~8PC[~2O0~9a[~2C700U@~28a1@~2VX0d`~2e0~8d[~2R0~<Y[e70~62[~2Y]0~42[~2C^OY[[Y]0~42[~2Y]0~;R?@~2U0~;^RCCb0~6a[~3O0~82[Ca0~5a8@@?e0~6^C[eO0~=a[~2C70~47R[[YbX0~3^e`U~4`70~5NRCCda0~9P2YCYb^00]Y[~220~8d[~2R0~8XadCCRf0~9P2YCYb^0~3XadCCRf0~9P2YCY2^0~3^aeCCda]0~7a2~2O0~2f2~2b0~6f2~7O0~5f2~2b00a2~2O0~5f2~2b0~;b2~2P0~7P2~5a2~4O0~mP2~2a0O2~2N0~}0~>b2~2P0~ba2~20~}0~}0~V]POaOP0~:POON]0~8POONX0~67~2X0~2XNaOP0~8]]00]0~4Xc~2^a2O0~4^c~2X00c~2P0~7N~2]0~MN~2]0~5N~2X0~8^77^0~37~2X0~57~2X0~27OaO^0~8POON]0~8PN~40~7^a2O^c~2XXc~2X0~8^aN0~;]~70~8N22O^0~2Xc~2^a2O0~4Xc~5P0~5ca22OX0~AXc~7X0~:]Na^0~=N22O^0~2^c~2X00c~2P0~2^c~2X00c~2P0~FXNaO70~<]]00]0~7Xc~2XXc~270~5c~6X0~@YV1I?I1U20~62U1I?8Ve]0~42_1I?QVY^0~3`1~2ZO7CZI?I1`a0~4][ZVbUZ_700^U8~2Z?TQ`P007V8~2_]Y8~21b0~5CI~2[00R__[__Y0~2OY~2R0~;CI~2[0~4dQII_X0~6bS11ZO00"
        "e1~2UP0~3`1~2ZOa`1I?IZCN0~42U1I?8Ve]0~5OSIIQI820~5RZ?TQV8~2_U8~2UX0~3X2C_V?8_[dN0~5^_Z~7[0~52UQTTIZd0^U8~2V?TQ`P00^U8~5120~2PU8?TTIV20~32Y~220~6^U8~7UX0~82UQ?_NReb]0~62UQTTIZd07V8~2_]Y8~21b07V8~2_]Y8~21b002Y~220~<7YZQ?I1`O0~8][ZVbUZ_70~4^U8~2UU8~2SO0~3C8~6_]0~32Y~220~5OIE3G;G39Id0~4YI<3;;6ET`^0~22?93;;6EDUf00713~2D2UA=6;GL<1R0~3fQ=<UA=T200NQ6~29;53>_X0OQ366IaVE66Ae0~42ILG9V0XS99D99IN00U>~2?20~92ILG9V0~4[>GGTd0~5^V933AR00`<33A[0~2713~2DR8WLG;6=>VX0~2YI<3;;6ET`^0~42?66WG9U]0~3RQE;53>366I?6638P0~2aV?W99;=9<A1Y]0~3a?=~71P0~328<655G=Z]NQ6~29;53>_X0NQ6~5>Y0~2bT3;55G91O00d1>~21CY]0~3NQ6~78P0~7dI96LZ1DA?`70~428<655G=Z]OQ366IaVE66Ae0OQ366IaVE66Ae0c1>~2170~67ffPfUA=6;GLW1R0~7fQ=<UA=T20~4NQ6~28>663T20~3UE6~431P0~2c1>~2170~4OIGFKKB4B=1f0~2RILK4FFHF6DC0~2RAMJFFHF;>UX07134:D`>;JJFFHME1a0~2^S=6E36IO00NQ"
        "G46M:FHGTR00U9BM>CI6438N0~3PV9KE1b0XSL:D;GIN00UEK:DR0~8PV9KE1b0~4fV<"":=1a0~4eD;J=S700bI6J3QO007134:DQEMHJBFJ31^00RILK4FFHF6DC0~4]`>:9=6?20~2PZEKB:H;9F:A>MB9_0~2aS<;KFJ4JFB:=TC0~3XV9693E9;<`0~3PSEM4K:K=Z]NQG46M:FHGTR0NQG4:5~2>Y0~2bT55;:HM>Y0aI<GK6<<T20~3NQG4:55:468P0~62I=KHLI=5M6A[0~3PSEM4K:K=Z]0U9BM>CI6HL1f00U9BM>CI6HL1f0NQGK68P0~5PZQQ1_>;JJBFHME1O0~6^S=6E36IO0~4NQG46>;J3@[]0~3UEJK5:431P0~2NQGK68P0~4OI39W>95g5T200^VEBHG993FJL8O00RA5=<9LBHG@e07134:D?6HKL<E54M<_0~2XC@=6L>_c^0NQG4K9A=FB9_00RD5JEZA:B9_0~2XcCA;=Q200XS==D==IN0XUEJ:DRcc0~4XcCA;=Q2cc0~2XcOS9M<Uc^00]VEFKWC0~2]VEJM>C007134:D9K45E<9331^0^VEBHG993FJL8O00^ccO1EGD;<_P00RD5J=A5J35F9EJ5DR00]U<"":4:3L:6LGFFLI20~2OSD<1WAZ<I8C0~2RD54;<DW=Z]NQG4K9A=FB9_0NQG46T~2?R0~2bTWT?>:B<C0a?;FgFB:D2cc00NQG46TTG468YeO0~3]U9KH;9EBFBJ3QO0~2RD54;<DW=Z]0RD5"
        "JEZA:K<`0~2RD5JEZA:K<`00NQG468a2O0~3a?66>?GHK=<E54MW`0~3c~2PC@=6L>_c~2^00NQG46;J3D[X0~4UEJ:DG431P0~2NQG468P0~47V8_CY1345D2002?;4:"">U`TG4:"">Y00RAAS``ILJF9U07134:D<KHLI[V<KH38N0^U8~318~2VPNQG4G?dA:HLSX0P1=JG?9F5DR00^U88>>I81200YV8~2V[]XU8EJ:D8~2[0~2^U88>>I8~3R0^U8~21>>88VP0OI64;?2b~2aC>MJEZX07134:DG4M<S[_Q91^02?;4:"">U`TG4:"">Y0PV8~2`?>@>>?Z70[<BB<864;3436HL1c00aI64M<81A?ST3HK<`0~2[>66A[CS=6LQa00[<BF9VR[Q`0NQG4;?dA:H=SXNQG46>>A8e0~2X[TEELBGDe0a?<G46<<T8~2R0NQG468QG46>>A8e0~2OI64:"">T;H3EF:Ad0~2[<BF9VR[Q`00P1=JG?9F;?20~2P1=JG?9F;?200NQG468?TI_P00O?;KW<BHLQ[Z9KHL1P00C8~21R18~2[Z8~2V70NQG4BH6A8e0~4]V=H:DG431P0~2NQG468P0~57Y?@DW;J3QO00Y>M438NXVEJF9_00P[ST~3>M431P7134:D=HB<`0b?;45T20NQ6~28T6~2?aNQG4682T5431^00[WMKW3HL1c00NQ6~5>Y00NQ6~28P0P866HM6~2=V]00NQ6~28?366>C0NQ6~28T6~2?a0eA:431[?~2IRT546"
        "IO07134:WM46W~3>_e00Y>M43>~3EJF9_0OI6~2QR_WG5GIN0_9FBW134;9B5:B<`0~2dAM46QaOR2^`<BH=SX0N8LH31f]C>MM>[00_9FB<C00N]0NQG468IT5431^NQG4BKK:=8a0~22T5BJG9Q200YQG468?366>C0NQG468QG4BKK:=Qa00RA:438WKF9>KB<[0~2_9FB<C00N]0~2[WMKW3JEZ^0~3[WMKW3JEZ^00NQG469G56>`0071=J3LHB<>~3;4;?a00V966E8A669VW66=QO0NQG4~2K:=8a0~3^SLH;?G431P0~2NQG468P0~6C<5:KK=D[0~2C<BH=Z]0[<BJEV0~2NQG5~3B4GIO7134K554M>d0P134MAR0NQG468EF4;?aNQG4GIRD5HLS^00bI6J=5B<_0~2NQG4:5~2>Y00NQG468P0P865HK5~2=V]00NQG46?LF5WVf0NQG468EF4;?a0CWBH=ZUE55>YI;45T207134JBJgB~49`0~2C<B4K~54JEV0]VEF:AdI64MGIN0`<BB<834;D544;TR0~2C<BH=Z]0~3eAM438P0eA:JEUZ~2?;J=S^0`<FF9_2~2^0NQG4G?L=5HLS^NQG46L6JMWC00O`?<<EMK<`00NQG46?LF5WVf0NQG468QG46L6JMW[00Y>KH=V<BF<AMF9`0~2`<FF9_NeVY0~2bI6J=5:"">C0~4bI6J=5:"">C0~2NQG46MMJH3IO00CA;JK4JK~4B45D200b19MKEAMJ9>;FL?Y00NQG46L6JMW"
        "C0~2XbIG46IG431PP00NQG468P0~6C<55MJM=8O00C<BH=V]0[<BJ=V0~2NQGF~3J4;IO7134JFFgMAe07134MAe0NQG46D;4g;?aNQG4:WI9BFEU0~2]VEF5B5@R0~2NQG46T~2?R00NQG468P0^ZTEJ:DDW>Qd00NQG46=B5<VO00NQG46D;4g;?a0[<BH=ZUEJK>YIG4:D207134B:J4:~49`0~2C<BHG~5FJ=V00YA:F9U>MB9D1P0dA:JET;HL1LHHL1P0~2C<BH=V]0~3RA:468P0[<BB<D=~2Q64GIO0YA:46>T~2[0NQG4MWEKFFEU0NQG468AMJEU00[<A81T;JEU00NQG46=B5<VO00NQG468QG468AMJEU00Y>KH=VWKF9>MKWC0~2YA:H6DQ@EZ]00]VEF5B3QO0~4]VEF5B3QO0~2NQG4BE>3H:DR00PV>3;4KG~7AR0~221EKM9MJ9;J3TC0~2NQG468AMJEU00]UI<MJEIG43881R0NQG468P0~5^d?TT>64:"">Y00C>KHL170_9FJEU0~228<~4M46Qf7134:<LHKWC0OI645DR0NQG46=FFg;?aNQG4;:GM45AY0~3dA5gHL1P0~2NQG46822b0~2NQG468P00NUEJ:95KM=Qa0NQG4;B:<ZO0~2NQG46=FFg;?a0CWBHLSVEJKWC?;g5T207134:"">K43D~4CO00C>KHLI~39FJEU00OQ3438EJ;TRa00O8=F:GF:"">_<BK<`0~3C>K4L1P0c70C>KHL170_9FBWD:H=86"
        "4;?a0O1=BJ:55EU0NQG4;:GK45AY0NQG468D:H=V00[<"":63GFK<`00NQG4;B:<ZO0~2NQG468QG468D:H=V00eA:431T;HL<B5@R0~2O8=FJ5G5=Z]0~2dA5gB9_0~6dA5gB9_0~3NQG4;?UEJMAe0~2N`8LHM>I~61a0~3RQEB:BJ;H6D[]0~2NQG468D:H=V00713M4:"">IG4G66WC0NQG468P0~4OZVeb2UEJB<C00RD54;TYRQ3HKWC002?I[~21EFJ=ZX7134:D9FJES2[A:46IO0NQG465K34;?aNQG46=MK5<ZP0~3f1LHK<`0~3NQG468P0~4NQG468P0~2UEJM5;:4M>C0NQG44FBESN0~2NQG465K34;?a0d>M46Q1LJF9UA:4;?a07134:D;45A`ReVDVX0RD54;TYRQ3HKWC0~2U9F5AGH=1P0~3YTLMK59SU9F;T20~32T;45A__8S[8=JF9U00`9FB<AMHLQ64;?a00d?EG5BJEU0NQG46=MK:6Td0NQG568D:H=V00[<5KBK5=I200NQG44FBESN0~2NQG568QG568D:H=V00a?G45AZ9K53J=SUa0~2Y?=5KK:=Z]0~2f1LH;@R0~5^d1LH;@Ya0~2NQG468_9JMAe0~4U9FJ=8d2eU?QO0~4RI3JJggHH5>[0~3NQG568D:H=V00713MG91IG55JBWC0NQG468P0~4RA9A?ID3HKWC00c1=JF=AD9M46?200RALW@D9:4:"">C07134:DD;45<TA3JB9VX0NQG4KJ334;?a"
        "NQG468>WDZa0~3R_U9F;T20~3NQG468P0~4NQG468P0~2UEJF=?<BF9_0NQG4MEB:"">_]00NQG4KJ334;?a0b?;4M<W5GM6AEFHL1707134:D=FJ6>TA931^0c1=JF=AD9M46?20~2eA5F9MK<[0~4]CI>WDZ_I3J=S70~37ZEBJ69<6E<=MH;@e00CWKJ=<B:GA5438f0~2RZ?AMJEU0NQG468>W<;>[0PS@~2ZA:JEV00e8D>ELD1d0~2NQG4MEB:"">_]00PS@~2ZS@~2ZA:JEV00XV9BJ6>A=JFGA><[0~3Y1DELAI[0~4`<FLS70~5YA><FL>>ZX00NQG468_9JMAe0~4dD;4MEATA96?b0~3fS9BMLFH;GH3?R0~2PS@~2ZA:JEV0071WA?VbS@@LJ3Td0NQG468P0~4RA::;;5JJ6?20~2CAGJF:5BH5W_]00RAMK55B4M9SN07134:DZ9M4K5:JF3?R00NQG4gMW34;?aNQG468YCRP0~3]Z99GJ=S70~3NQG468P0~4NQG468P0~2UEJM>Y>MJEU0NQG46?6H6Td00NQG4gMW34;?a0^ZEB4KMFE3JMB4MW`007134:D@6FHM5:B31^00CAGJF:5BH5W_]0~2f1LH6J;?20~6aYCR?9EKMW[0~5RI=MHFBHFBJF;WU^002T;4B:MLF:HFEV]0~3^eAMJEU0NQG468YC1>Uf00O2219=BB<`0~2dAA>5D20~3NQG46?6H6Td0~2O2~2NO2219=BB<`0~2R?LFHKMB4JFMM<[0~3dAA>5D2"
        "0~5[<FLSX0~5CWKKHKKLSX00NQG468_9JMAe0~47V<"":HF:5:B5?b0~3`>5F=AMJ9<MBESN0~2O2219=BB<`0~22CdbX0OZ9K6D[]0NQG468P0~4RAG:KBK53A_]0~2^_AL:KKM6WZO0~2RAL5KKMG<120071355D219GMKK5=T[0~2NQG4JL834;?aNQGF68P0~6]Z=FF;AC0~4NQG468P0~4NQG468P0~2UEJ:DRAMJEU0NQG468>5F=1a0NQG4JL834;?a00R?=:BML?A6KK591O0071355D[@=5KBM;ES^00^_AL:KKM6WZO0~4_<B4J=Z^0~9b?5JBLQa0~6dQW=36KG6L9DUN0~2^V<5KMLW;KM3Te0~5eAMFEU0NQGF68P02YP0~4P13JB3?20~2d>:M6?b0~3NQG468>5F=1a0~8713JB3?R0~3C?E;KBMGL:KM<[0~3d>:M6?b0~5[<FLSX0~5Y><<FL<<ZX00NQG468_9JMAe0~5aSW3:KKM;=?b0~2R?3H6TAMJ91=F5>`0~4P13JB3?20~8[A56D`X00NQG468P0~4OVIA>W>D8[70~47C8AWWAI_O0~37[8D>WA?Ub0~2XV@~2Ib2V?>W>D1Y]0~2NQG55>135;?aNQ<~21P0~6]ZEE9AU70~4NQG568P0~4NQG568P0~2UE55DRA55EU0NQG568Z955<U0NQG55>135;?a0~2Y8AW>QC`?>WDS20~2XV@~2IbY1@>W>?ZR0~37C8AWWAI_O0~5R@;55>C0~:b?EE<?Y0~8a[Z1QW"
        "A81Ue^0~4aZDW>IVDW>IC]0~5eA<~2_0NQ<~21P0~8P1EE9T[0~3RIA>?`]0~3NQG568Z955<U0~871EE9T[]0~4eS@>W>?8AW>?Y0~3RIA>?`]0~5[<EESX0~5c[[<EES[20~2NQG568_955Ae0~6O_IAWW>?SC]00PSE55W_A559_T656?R0~3P1EE9T[0~8XZ<<A`^0~2NQG568P0~5XaeCCYRO0~8NRCCda0~7NRCCda]0~4N2~2b00X2YCCRc0~4PS@~21V@~21N0d[~2R0~8eUU_Y70~5PS@~2Z^0~4PS@~2Z^0~2`@~2I2I@~2`0PS@~2Z2I@~2U0PS@~21V@~21N0~3OdCYO0]2YCRP0~4N2~2b00P2YCYb^0~6NRCCda0~7PS@~2Qb0~:X[UU`20~;XPbCeP70~8PRCYa7RCYb0~77C[~2a00d[~2R0~:YUU`R]0~4aRd2X0~4PS@~2Z2I@~2U0~9YUU`R]0~6P2YCdbOeCYb0~5aRd2X0~6aU~2d0~8aU~2d0~4PS@~2Z[@~2?20~8OeCCd270~3PS@~212I@~2[_@~2?20~4YUU`R]0~92[[C70~3PS@~2Z^0~uO2~2Of2~2O0~LO2~2N0~6O2~2N0~372~2b0b2~2^00O2~2N0b2~2P00O2~2Of2~2O0~jO2~2a0~}0~PO2~2N0b2~2P0~}O2~2N^2~2b0~EO2~2O0b2~2^72~2b0~OO2~2N0~5Xc~2XXc~2X00Xc~2XXc~2X0~4Xc~2P00c~30~97OaOP0~IX7~80~4Pc~6X0~OPOO"
        "N]0~Sc~60~2]c~270Xc~270~7^7~20~:POaOP0~;ca2bN0~9]7~70~97~2X00X7~20~>^OaOP0~4X7~9]0~8POaO^0~6c~70~2Xc~2^a2O007c~2X0~4PbbPc~2P0~5c~70~7Xc~2XXc~2X0~8XOaP0~3OaO0~BOaO0~;]NaO70~F7~4^0~<^U8~2UU8~2UX^U8~2UU8~2UX0~2^U8~21bd8~3[0~62_1I?I1_20~9R__[__Y0~5cV1~8[0~2218~6_]0~3aY~420~4R__[__Y0~52U1I?8Ve]0~5R__[__Y0~6R__[__Y0~5C8~6C00`8~2SOU8~2Zf0~5OZ1~2[0~7RU1I?I1`a0~7^_8?T?8`f0~6^U1~7[0~7`1~2ZOfV1~2`]0~:a[1I?I1Ue^0PV1~9_]0~52U8I?I1_b0~3C8~7e0^U8~2V?TQ`NZ8~2UX0~2Y1?""?18~21a0~3C8~7e0~5^U8~2UU8~2UX0~62VI?1C^0dI?Id0~@dI?Id0~8XeVQ?I1`a0~:7CCd]0~3`1~4SUd^0~9NQ6~28Q6~28PNQ6~28Q6~28P0~2NQ6~2W[VE66=Z]0~4R?<LG;G3<IC]0~6XS99D99IN0~42T3~7LS^00Y>6~53170~2XZ>~48f0~2XS99D99IN0~3YI<3;;6ET`^0~3XS99D99IN0~4XS99D99IN0~4VE6~4EU0^SE66W_@663?a0~5RA3~21^0~5XZ93G;GL<SO0~6O?3;5;3>_]0~5OI3~6LS^0~5713~2D22T3~28P0~9R1WLG;G39DC0b"
        "?3~98P0~42Q93;;GL9_0~3UE6~59`0NQ6~29;53A_>66=1P00Y?=;;=E66Ae0~3UE6~59`0~5NQ6~28Q6~28P0~5O196;LA[PSE;>C0~?PSE;>C0~7^`TE6;GLW1f00^c~27b2cCAWT20~2713~4L9D_X0~8NQG468QG468PNQG468QG468P0~2NQG4HL1?G4H=Z]0~4d>KHFBJHKLTY0~6XSL:D;GIN0~42T5H~34~2LS^00Y>KB~2H43170~2XSLK~2GIN0~2XSL:D;GIN0~2RILK4FFHF6DC0~3XSL:D;GIN0~4XSL:D;GIN0~4VEJK5~2EU00e?6H6?=F5>`0~6RA:431^Pc~200XZLFKKJH:W`0^c~2X0O?;;GKH6IO0~5OI;44HH44LS^Pc~2007134:D22T5468P]c~2P0~32QEMHJFFJF9`0b?5H~244H~268cP0~2]V9K4FKKJ9_0~3UEJJBBHF9`0NQG46M:HHGT6H;DY00f1=FJMM=JMAe0~3UEJJBBHF9`0~5NQG468QG468P0~5C>:6EM3QCA5=1N0~?CA5=1N0~7[A6FHFFH;IO0^U8~2Z?T1S=3T8C]071344H~2FG@eO22aP0~3NQG468QG468PNQG468QG468P0~2NQG4g5A<KgH=Z]0~4d>5L9<E;HF=1N0~2c~2PS==D==INcc^002TL~464BES^00YAW~2EBKE1700XcS<~4QN00XcS==D==IN00^VEBHG993FJL8O0~2PS==D==IN0~2XccS==D==INcX002VEJ:DT~2`0"
        "0]_W:BE:BESN0~6RA:431218~2[0XZL9>W=FH=ZcZ8~2U^OI>?IEJ;?a0~5OI;4;LLM4LS218~2[07134:D22T5468P`8~21a00XV9K45=<9=;9`0b?L~3JBL~3881200f864KE>WE9_0~3UEJ:WWKF9`0NQG4K9>LFBEBM<U^00eA546>W;4MAe0~3UEJ:WWKF9`Y2^0~2NQG468QG468YeO0~3VEB<Q65D1=5AY0~?O1=5AY0~7aI3JB399=;IO0NI6~29;5=D5;5LTd07134:==;HF9VITT?1UN00NQG46EEG468PNQG46EEG468P0~2NQG4MFE3BBH=Z]0~4d>A1_[VA;4:Ad00C8~21Y18~2VZ8~2V70PUS~21EKJLIR0~27CCS9MB=IR00^U8~2UU8~2VP^U8~2VU8~2VP02?;4:"">U`TG4:"">Y00a18~2_8~3R0^U8~2VV_VV8~2UX71WEJ:W~2_^0~2O1EFKH3?R0~4NRCCA:431d>66=Z]0`IUCCAMHLSa?6~2QN0eIEEGK=1c0~5OI;4;?AM4LSd>66=Z]7134:DCCT5468PSL66AR00b?G4M<S[`Z@9`07US~2EJB<SS=66>Y00fQ64;?YCVIC0~3VEJ:D>KF9`0NQG4G?[A:4FF=8a0~2[<BF9UZ=HMAe0~3VEJ:D>KF9>>TVO00NQG46EEG46>>A8e00]V=KW13MA>:EZP0~?[>:ESP0~7C>MH3?~28WDU]NIG46MKHBEBKHFEZX7134:DZ<BJE9G55;L>d00NQG4FBBF468PNQG"
        "4FBBF468P0~2NQG46:G:G5H=Z]0~3aUV~5ZEJF9_00V966E8A669VW66=QO00]X^V<"":H6DC0~4^VW5J3?d0~2NQ6~28T6~2?aNQ6~28T6~2?a0Y>M43>~3EJF9_00RA66EU<66<C0NQ6~28P0NQ6~28PP13B4JBB9_0~4e?G4M>`0~3XC8D>WW:431d>KH=Z]0Xd?@DEBK9_a?;4GQN0OI;FJ3WV^0~5a?;4;IAM4LSd>KH=Z]7134:W~35468PSL4MAR00Y>M46Qab?9~28c00XXUEJB<CV=HK>Y00XZEBM9D@I20~3]V=H5@>KF9`0NQG4682T5gg;DY0~3`9FB<CUEJMAe0~2]V=H5@>KHK~2;WU]0NQG4FBBFgBKK:=Qa00_9F9T;;TL;@ee2]072dRf0~6aQL;@e0~8UEJB<9;6=:LSXNIG4K9>54BE>;46QN7134:DV<BF9MF5;:M>d00NQG46<<G468PNQG46<<G468PP00NQG463JFE5H=ZP70~2C<E~7FJEV00b19MKEAMJ9>;FL?Y0~4O?;45<A1R0~3NIG469>Qd00NQG468EF4;?aNQG468EF4;?a0C<B4K~54JEV00RAMJEU<BB<C0NQG468P0NQG468PP1EEJ:EE9_0~3NSEBBH3?R0~2CDL5KBBJ431d>KH=Z]00CW5:BG9Q2a?;4GQN0OI<93J;T2P]0~32T546QAM4LSd>KH=Z]7134JB~3F468PSL4MAR00C<BH=Z]bT5F68c0~3UEJB<CV=HK>Y0~2e?E5K:5AR0~2]2SL"
        "H;?>KF9`0NQG4GIRD54JF=8a0~2[<BF9`VEJMAe00]2SLH;?>KFL3546Ia0NQG46<<G46L6JMW[00e@6:;MEWM9QAA?_bZT>D1R0~5`<M9V^0~7]Z=HMA9FGK5LSXNIG4;?Q64;T134;?O7134:==;JGEJ6T?A9>d00NQG468QG468PNQG468QG46881R0NQG46<K5A5H=88Sa00CWBJ~64J=V0~221EKM9MJ9;J3TC0~5O?;BFB:E8a0~2NIGKBFM=I20NQG46D;4g;?aNQG46D;4g;?a0C<BHG~5FJ=V00RAMJEU<BB<C0NQG468U`RQG468P0CVEJ:D@@_0~2]`>:FEMBESN0P1LJHM::B431d>KH=Z]00CW55KF;W`a?;4GQN00e[`>KMA88`0~3RA:431AM4LSd>KH=Z]7134B:~3B468PSL4MAR00C<BH=V]bT5468c0~3UEJB<CV=HK>Y00XV95FK55AR0~2e?<5438>KF9`0NQG4:WI9BF=FM<UX00Y>MHL?DG4MAe00e?<5438>KF91EJ:AR0NQG468QG468AMJEU00X`D9EWI36?L:MG>Z9;M5E1O0~32I3G?R0~8XZ=HMA9FK9TWZXNIG468864GI134;?O71344HHJ=A9F:LEWTZa00NQG468QG468PNQG468QG4;66<C0NQG46?<<T5H6~2AR00Y>M4L=~4JJEU0~3RQEB:BJ;H6D[]0~3X7OIW<9GHMWC0~2f8>><3JK<`0NQG46=FFg;?aNQG46=FFg;?a0C>KHLI~39FJEU00RD"
        ":HLA<BB<C0NQG46EE<TQG468P00VEJB55=V]00R?6H6T=F5>`0b?;45WDA:431Y>KH=Z]00d?TD9MH38b?;4GQN0~3Y>KK66LSX0~2CWKJ=VA:431Y>KH=Z]7134:D~35468P1L4MAR00C>K4L1P2T5468c0~3UEJB<CV=HK>Y00b?;45<TTI2]00CWK4M<U>KF9`0NQG4;:GM4;A6HGDY00OQ3JB;;6JMAe00CWK4M<U>KF9_9JMAe0NQG468QG468D:H=V0~2Xe_UU<K<9BE<M3@;;>3M>C0~2]U<K<U]0~9VEJK>9F;?2C20NIG468864GQ134;?O7134:=3BF3AGBHJKG>[00NQG568QG468PNQG568QG55JB<C0NQG568[[T5~2HMAR002T;4;TVVQ3HKWC0~3RI3JJggHH5>[0~3^U1CeCCU<BJEU002UY~2`WKH=V]NQG465K34;?aNQG465K34;?a0RD54;TYRQ3HKWC00aI6JFMMJB<C0NQG4FBFB6DG468P00VE55KH=V]0PSE55<_D;H6Ia2@5438C>M46Q[<BJ=V0~3bb2I;4;?RD5438P0~3Y>55M4LSX0PR1=JB<`D5468[<BJEV07134:DRR?;4GI28645D200R@5g5A`eA:431P0~3_9FF9_1LHMAd00RD:4312bYSU^0CW56<1Y>KF9`0NQG46=MK;<Z>55=8O00[A6KK6EJMAe00CW56<1Y>559_9JMAe0NQG568QG568D:H=V0~5R?63ILM>IG5>K=Z9F9_0~2R?G3I20~:["
        "<BJ=9F6QT1N0NIG468Q64GQ134;?O7134:DILHK<@9=3MH=SX0PS@~2ZIG438PPS@~2ZS@@LJ3@Y0PS@~2Z^a8@@6B=I200P1=JF=AD9M46?20~2fS9BMLFH;GH3?R0~2NI=>ISV19FJEU00_9AQS89BJEV0NQG4KJ334;?aNQG4KJ334;?a0c1=JF=AD9M46?20~2`A=L~2BB<C0NQG46<9:F9G468P00`@@D5H=V]0PS@~21`1L4;?bb?;45WALB4:<DLJB<`0~5c83HM9@EBJ=ZX0~3e?@@;4LSX]UD9:45DRI64:<DLJK<[07134:D2c834M<@<"":46IO00P1=FJ6>TEFJEV]0~3C>MH3A>54;?2002T54;>I?AE?O0e?TQUC_WKB<[0NQG468>WDZaS@~21N007`?>>?EJMAe00e?TQUaR?@@TUEJMAe0PS@~2ZS@~2ZA:JEV0~4^V9MW1LM>IG5WK=Z9F9_00^V9MW`0~;2T;4M9F6<6IO0NIG468>M4GQ134;?O7134:D[WMH3AI1Q6431^00O22U9EMJ=ZX0O2~2NOV9K6D[X00O2~2N00a1EFL?Y0~3CAGJF:5BH5W_]0~2`>5F=AMJ9<MBESN00NIGMGL=L:4MWC00_9:6L3:4MW[0NQG4gMW34;?aNQG4gMW34;?a00CAGJF:5BH5W_]0~27CZSS<BB<C0NQG468?;JEG468P00719=BB9_0~2O2~2[<E5438NP1LJHK:K9K4M5FH6?20~6_W:4K5B45AC0~522?;4LSXXSLB4B=1cU"
        "9K4M5FH6?207134:D20_<M4K5M4K9U]0~2YD6JHM5K4;AY0~4aI3JJ:MHBEZ700c1=FH:;;:;?O0022OOI9LJM>Y0NQG468YCRP0O2~2O0~3]2CCUEJMAe0~222O0~222eA9G4;?200O2~2NO2219=BB<`0~4e@;L8UEFEW:3D5G>3M>C00d@;L8a0~;XU<"":4B4MK;IO0NIG568=55GQ135;?O7134:D2I34J:633KFEV]0~4UEJF;AC0~6CA;GA`X0~8XUW:3?C0~47_AL5KK:6WZO0~2R?3H6TAMJ91=F5>`00NI3:BJHJF:E8a00_9MFJJB598a0NQG4JL834;?aNQG4JL834;?a00^_AL:KKM6WZO0~5]XC<BB<C0NQG4;6GFK<G468P00^13JF6TR0~6[<BF5<_00CD3MB:=IE;KKM3A_]0~6OS<GMBMG<Zc0~6a?;FLSXXSLM;EIe02QE;KKM3A`]071355D20a19;MBK;EQ20~3X`D=5KKM6WVP0~5[DL:KK5=IR0~3YT=5KBK:6?O0~4O?;JK=8O0NQGF68P0~@UEFMAe0~:eAMJM9Z^0~8713JB3?R0~37ZE:"">[eT3KK;>S95MM=8a0PSE:"">[0~=aS<6MKK5=IO0PS@~2A5>@@SV@~21N71355D2_<5~2MKK:LTY0~5UEE9AUP0~5]V<<A_^0~9P8<<TC0~67C8DWWAI_O0~2PSE55W_A559_T656?R0]`8A9E=E9A1R0~2CI>9E9<DS200NQG55>135;?aNQG55>135;?a0~27C"
        "8AWWAI_O0~8C<55<C0NQG5~3;EQG568P00^1EE9T`]0~6[<E9>ZO00^[I>WA1eS@>WAI`P0~8aU?>W>?UO0~7a?<~2SXXZWA?ZR0~2RS@>WAI`700XV@~2Ib002VT>W>TZR0~5^C1D>W>?UO0~6^CQAWWD1e0~4]Y1@>W>DIU^0~4O?EE<IY00NQ<~21P0~@_<~2Ae0~:eAEEW120~971EE9T[]0~3OQWW1O][?>W@V2SAW>8e00OQWW1O0~>O_IAW>@1Y0~2O22UEEI22Of2~2O0XV@~2Ib2I@~2AWWAQC]0~5RUU_Y70~72[[C70~;e[[Y]0~8NRCCda0~4PS@~212I@~2[_@~2?20~2Od`V~2_dN0~4aY`U_[RP0~2PS@~21V@~21NPS@~21V@~21N0~4NRCCda0~:YT@@?Y0PS@~4?ZS@~2Z^0~2YUU_eX0~7aU~2CN0~4adCdO0PRYCda0~;]bYCY2X0~9Y[~2200bCdb^0~4PRYCda0~4N2~2b0~3^2YCY270~8fRCCda]0~9OeCCRc0~7P2YCYRO]0~5]CUU`20~3d[~2R0~Aa[~2C70~:c`UUCa0~;YUU`R]0~5RCC20~2bYCR^0feCYO0~3RCC20~AaeCY2P0~6RUUC0~9N2~2b00b2~2dCCeO0~qO2~2O0b2~2^72~2b0~MO2~2Of2~2O00O2~2Of2~2O0~F]2~3]00O2~4a^O2~2N0~}0~}0~}0~}0~P7~4^0~GPOOc0~9]Oac0~8]cOaOX0~C^OOP07~2^0~}0~}00P^0~2X7~20~EX"
        "7~20~7^OaO70~9^7~2]0~4^7~7X0~3c~2P0~3]cOOP0~8^7~2^0~77~4^0~67~3^X0~87~70~37~70~37~2X00X7~20~37~2X0~37~2X00^7~2^0~37~2X0~77~3]0~27~3X0~27~300]7~20~37~4^0~8`1~5VYOC~2Y^0~=O`1I?8U20~6OUQ?8[^0~4N[Z8I?IZd]0~4aY~220~5XYSII1[1~2ZO0]YCP0~FPYYX00R[~:CN0]Y[~:YX0~6aCb0~7PC[~:e00PC[~:e0~7NUUP0~;NUUP0~8PeCbX`8SR0NV1~2C0~5OUVVCVV[]0~2Pd`Z1~2[0~4R_SI?I1[f0~6bS1~2U^0~2OZ1~7U700Y8~2120]dU8?""?1`N0~5OZ1~2S20~5`1~4SUY70~2`1~4Z_e70~4`1~7[00`1~7[00`1~2ZOfV1~2`]0`1~2ZO00`1~2ZOaZ1~2ZO00`1~2ZO0~5`1~3_X0Y1~3VN0`1~3R^U1~2`]0`1~5VYP0~4713~4LEAVW~2De0~<21WL;;39I20~4XUW6;3A[0~4C>=3;;6ETC0~3XZ>~2QO0~3][@=G;L>E33AR0b?ASd^0~BXeZAT2071<~:"">C0b?<~:T20~5]V<V^0~6Y><~:8N0Y><~:8N0~6dADR0~;dADR0~8_AWSCD3EZ]2@33=V]0~4Y>E98EEIO0~2[A<=33LS^0~2OQ<LG;GL>Vf0~4XU<3~2IO0~2RD3~7?O00_966>d02TE3;;3WVP0~4YA3~2<[0~4713~4LEAU70713~3L=9DVa0"
        "~2713~6LSX713~6LSX713~2D22T3~28P713~2D20713~2D2Z<3~2DR0713~2D20~4713~3?b^Z=3~2@2713~2EVOI3~28P713~4LEAVf0~371344HH4J;>9BMAefb2bNX0~5NS9:HFKBMAR0~4a?G:LFL1P0~3C<BJFFHF3Ia0~2^13K;?O0~3dT6F4FF;EJMAR02T5E@V2]0~?2UTE5D20713B~9<[0N1LB~838O0~52?GTR0~6Y>3L~8GQN0Y>3L~8GQN0~6_9<`0~;_9<`0~7713;T?3:<U02@5H=V]0~4_9K>T5LS^0~2_9BJJ4LS^0~2OI;JBKJH5>C0~4eTG44;IO0~2RD:H~344;?O00_9JK>d02D:FKFH;AY0~3]VEF4H=S70~37134HFFHF;AY071344H4HF;<SO0071344H~3LSX71344H~3LSX7134:D22T5468P7134:D207134:DVW5H;>_^07134:D20~471344KW[2T;445@271344;TRI;468P71344HH4J;>`0~37134:==3FH39JMA88?T?QZY0~3PC>:4;9>9GAR0~4RD:6AM;IO00PeC<GE<9LFM>Ca00O134;?a0~27SEFJ6996;HMAR02T5F;9?_b0~<a_?9GF:D207134g~7B<[00[>Mg~6K<`0~6_<K9U]0~5Y>LS~7DGQN0Y>LD>~6DGQN0~5P13=S^0~9P13=S^0~7U9K<=BE1a02@5H=V]00OUVSL5T>K9V_f00_96=54LS^0~2OI;=<>LJJ=ZX0~2cSEF;4;IO0"
        "~2RD=~45H38N00_9BK>d02DG9>EBF9_0~3OI64MH;T20~37134:993JFEU07134:LLGB4:W`007134:L~4SX7134:L~4SX7134:D22T5468P7134:D207134:D>;4;>UP007134:D20~47134HHL1`<KJg5@27134gB9VI;468P7134:==3FH38N0~27134:DVT;g56JK6~2;5;6=S^00XVT=JK<T~2DAR0~3NRD5GAMGIO00[A~6>MF<A1PO1?34;?I2002T;45A__A5gMAR02T5g4B6<I[O0~8NC8W6K4g:D207134g~7B<[00aQ3Hg~446Ib0~5aI3H6Ib0~5Y>LSX~52DGQN0Y>L>K~55DGQN00Od~3T5;?d~3N00Od~3TE=?d~3N0~3YA:3M3?R002@5H=V]00CWEEG6EEMEEAR00C?8?54LS^0~2OIW1[CA:43170~2C>5G34;IO0~2cUV~39FK<`0~2_W<<Ad02?@VY>MF9_0~3YA:FEBK<[0~37134:D_>KH=V07134:D1?EBH3QO07134:DS~3C07134:DS~3C07134:DCCT5468P7134:D207134:A;H5<VN0~27134:D20~47134;:5D83F=H5@27134MBG?I;468P7134:DVT;g;?b0~27134:DC?;4FB4JBF4M;5KLS^002?G:4K5~238[70~2d8TI6K9B31P00_9M~6JHM68cb?554M5Ae00Y>M431cP134MAR02T5g~24M3>1CP0~47Y1>LMHg~2:D207134g~7B<[0~2_<Kg~4B"
        "9U0~6[>MgK<`0~5Y>LSX0~42DGQN0Y>L>Kg~45DGQN00V>~3<KM>~4_00V>~3<><>~4_0~3OQ6H;A[0~22@5H=V]00CWKKJKKBBKMAR00]bb?54LS^0~3Y[O0YA:HLS^00bQLK<34;IO0~4]]bI64G?20~2OC~3^0]eedI=JM>C0~2]VEFM>5H=S70~27134:"">>EFB9_07134:D2R?;4:DR07134:W~3>[07134:W~3>[07134:W~35468P7134:D207134:6H:<ZO0~37134:D20~47134;3B9A:5=H5@27134;;B9I;468P7134:DC?;45T20~27134:WWEK469JMW5B<?TW=S^00C<=546=~2<`0~3RI=53<"":BB9U0~2_9~2EKg~2H;998cb?5M4KMAe00[<BH=V]0V=HMAR02T5g~4H:=ASe0~2RZA=:Jg~4:D207134g~7B<[0~22?G4g~24;TR0~5N1LHgH38O0~4Y>LSX0~42DGQN0Y>L>Kg~45DGQN00`A3M~2HJM~2LD[00`A3<~2?T<~2LD[0~32ILH6Ia0~2b?;JEU0~2YA><B<>;3>>TR0~3b?54LS^0~6eI=JMW[00XU<KLI34;IO0~6[WMFEVX0~2[D~2?R0~22Q=BB=1O0~2OI64GI3H;T20~27134FKKFK=?R07134:D2^1L4K>Y07134JB~39`07134JB~39`07134JB~3F468P7134:D207134HHM91a0~47134:D20~47134;9B6EFL=H5@27134;9F6?;468P7134:W"
        "WEK46IO0~27134JBBF4M99JMA;J6=9A8C0~2bS<546<~28b0~27ZEB:F6G4B=1O00[AAEB4ggB3>AA1PaIA34;ATR00[<BH=V00VEJMAR02T5g~6J5E?20aIE;Fg~6:D207134g~7B<[0~2]U9Bg~2FEV^0~5YA:g~2M>C0~4Y>LSX0~42DGQN0Y>L>Kg~45DGQN00^[?=K4gg4MEICX00^[?=<SdY1<EICX0~2fZ9KMMW[0~2OIGF9`00OZ11=:AAME11e]0~3b?54LS^0~5eI=KKE1O00eTG5>134;I[O0~4c1LH5Ad0~3_9::"">d0~2_<KF=Ie0~3YA:J=Z9FK<[0~27134K;;5J:9VX7134:D2^SLHK>Y07134B:~39`07134B:~39`07134B:~3B468P7134:D207134HHF=Ie0~47134:D20~47134;@;MGK<=H5@27134;@;K<;468P7134JBBF4M9U]0~27134K55;3<19JMAEMJJF:=8O00RD;BgFBBEZ2200c195>LFFG::"">Y00_9MM4J~2FM~268cN1?34;?8200CWKHLS^^SLHMAR02T5g~54K3>1a0O1>LMHg~5:D207134g~7B<[0~3RT;4g45Dd0~57S=Jg~2J=1c0~3Y>LSX0~42DGQN0Y>L>Kg~45DGQN0~3d89KggM91R0~5d89<[_991R0~4[>559BL8O00P13M<C002D33G;33B33<C0~4b?54LS^0~4dI=KKEQR0~2`9FE<<34;<<`0~4dA5HL1f0~3_9JK>d00]Z=:"
        "5Dd0~3]VEFK<~2:H=S7007134:D?A;4GIO7134:D2aI64MAe07134:D~4C07134:D~4C07134:D~35468P7134:D207134:6HB=QR0~37134:D20~47134;?=JJ;?=H5@27134;?EF6;468P7134K55;3<1b0~37134:DT?8U_9JMA8>EL;J:Ae00C><3HM<<AYQ?2002Z>STE9D3F<`00_995439~68cb?;;4:;Ae00RD54;?ee?G4MAR02T5g~34B6<I[O0~2N[Q<6K4g~3:D207134g~7B<[0~3^ZEFgJ=SP0~5eD;4g~35AY0~3Y>LSX0~42DGQN0Y>L>Kg~45DGQN0~4UEFJJB9_0~7UEDDTA9_0~42I3BE?6MWC00OSAAT[002D55B55:K55<C0~2^RR?54LSR20~2dI=BKEQCC200`9F~4gJF9`0~3XVEFKW[0~4UEJ5>d0~2UA~2Ia0~3OI64HF~2J4;T2007134:DR8645T27134:D`S<M4G?a07134:D[~3R07134:DR~3^07134:DRRT5468P7134:D207134:A;HKE120~27134:D[~3R07134;?>MJ=Z=H5@27134;?D;K;468P7134:DT?8Ua0~47134:D2bN0_9FKW<T81WBK>Y00PC_<KH6AIA=AR0~2cYbdUZ?6B<[00b_?;4G>TA96W_Y0b?5K4BMAe00N8LHB=DD=B4MAR02T5gg4F;9?_b0~6a_?9GF4gg:D207134g~7B<[0~4YA5g:AC0~5XV9Fg~4FEZ^0~2Y>LSX0~4"
        "2DGQN0Y>L>K4~45DGQN0~3N83BL3FL1P0~5N83>L3AL1P0~2XV9K6?`W;?e002@GG=V]0bI@>K9@6G@@TY0~2CA~254LAA?20OI=BKEW~31P0`9;~44:;9`0~32?G46?b0~3P13FESf00]Z=GGT20~3YA:H;~4FK<[007134:AA<54;?a7134:<<=:4K9VX07134:<~48f7134:D20~47134:D22T5468P7134:D207134:D>;4M9Sa007134:<~48f7134;?I66>Z=H5@27134;?SEF4468P7134:D2bN0~67134:D20~2CWKHMM;33;H5DR0~32ILFJ5;5MAR0~6_93B5Ae0~27S=FJM5:BBWC00aI>34;>@R0~2[>54F55MEJMAR02T5gJ5EDVR]0~92UTE5Jg:D207134g~7B<[0~4c1=JL8N0~52?G4g~44;TR0~2Y>LSb~52DGQN0Y>L>G~6DGQN0~3RD5E?""?=;T20~5RDEE?""?=ET20~2NQ6K<UOSAV7002@5H=V]00bU9K>D5=S22]0~2[<MMB4MM:D20O?;gJB~368c0YI?~334;?IY0~3`<KF9U]0~32?;GTe0~2]Z=H5T20~2]VEFB<?~2>MH=S707134B::K4B=S77134JBFJHK=I2007134JB~36Qf7134:D20~47134:D22T5468P7134:D207134:DV<54:<ZO07134JB~36Qf7134;?_88SZ=H5@27134;?YA54468P7134:D20~871355D20~2bI=;55:KKM;9ZP0~4C?=5"
        "KK:3AR0~6_9M;<ZP0~3Y?=5KBM5LWC0~2R134;?eX0~2fZ<;KK59E55AR02T5M=ASd^0~<XeZA=::D207134g~7B<[0~5C>MW[0~6_<Bg~6B9U]00Y>L?~7DGQN0Y>L?~7DGQN0~3`<98RdQ9WC0~5`<98RdQ9WC0~2N833Ib0ORc0~22@55=V]0071L;TW59_0~5[<5~6D20O?;5~568c00O~3135;IO0~3N8355De0~4RD<<UX0~2]Z=55T20~2OI65;TROaI65;T207135~4GE?d07135~3;3<Qd0~27135~56Qf71355D20~471355D22T5568P71355D2071355D2Z<5~2WU]7135~56Qf7135;?aPP7Z=55@27135;?aS=5568P71355D20~8XV@~2Ib0~3d1T@@D>W>TVb0~6d1D>WA8[70~6_W>?Va0~5Y1A>W>T1`O0~2^135;?O0~4OUT>WDS@~2I202T3>8CP0~@PY1>3D207135~9<[0~5O83Ia0~5aI65~8G?200Y>5~9GQN0Y>5~9GQN0~3_AZb002SA[0~5_AZb002SA[0~3[A@[0~7aQ@~2_0~27Z@@1T@?d0~5CT@~6Ib0N1@~7S70~4^Z@~21f0~3f1@~2SP0~4^C~2O0~4U@~2Qa0~2O8@~2SP0XV@~2I20XV@~4TIZe00XV@~3T?1_20~3XV@~7SPXV@~2Ib0~4XV@~2IbaQ@~2Z^XV@~2Ib0XV@~2Iba1@~3U0XV@~7SPXV@~21N0~2U@~2QaXV@~21NCT@~2Z^XV@~2Ib"
        "0~9N2~2b0~5P2~2RYCY2^0~8PRCCeO0~8OCY2X0~7NRCCY2c0~4XV@~21N0~5X2CCRP2~2b00O8I`O0~DO[QQb0XVT~:?Y0~6`T_0~6O8T~:Qb00R?T~:SP0R?T~:SP0~3PRN0~3OR70~5PRN0~3OR70~37eRX0~8a2~2P0~3O22O2~2]0~5X2~7b0~2O2~7O0~6N2~2O0~5O2~2O0~?P2~2a0~4a2~2O0~2f2~2b0~2N2~5OX0~3N2~4aP0~6N2~7O00N2~2b0~6N2~2b00a2~2N00N2~2b0~2N2~2b00O2~3P00N2~7O00N2~2O0~3P2~2a00N2~2O0X2~3N00N2~2b0~}0~4f2~2O0~COa0~Haa0~2f2~:b]0~6^270~7O2~:a0~3b2~:N0~2b2~:N0~}0~}0~}0~|7~4^0~6X7~9]00X7~2]0~37~2^0~37~2X007~2^00^7~20~37~2^0~2^7~20~3^7~2]0~27~30~3^7~80~=7c~20~7POaNX0~2]7~2X00X7~2]0~6XX0~<XX0~<7~:0~47~3^X0~87~2X0~97~2X00X7~20~47~2X0cNNP0~27~2X0~97~2^0~87~2X0~;X7~9]0~>c~2P0~6^7~2^0~77~4^0~67~7]0~5^7~2^0~77~70~3^7~80~37~2X00X7~20~37~2X0~37~2X00^7~2^0~6^7~2^0~77~3]0~27~3X0~27~300]7~20~27b~2O0~2`1~4SUd^0~2PV1~9_]PV1~2_X00[1~2ZO00C1~2ZOe1~32bS1~2d00C1~3"
        "20bS1~2`00bS1~2U^0d1~3C002S1~8[0~2bC~2d0~3OS8~2[0~4c[1I?QZC0^U1~2VNcV1~2U^0~32_ZSUYP0~72_ZSUYP0~8[1~:d0~2C1~4ZUYc0~4`1~2ZO0~7`1~2VNNV1~2`]00`1~2Zb1IIZf0`1~2ZO0~7C1~2Sa0~6d1~2VN0~9PV1~9_]00bC~2d0~5Y8~2120~4OZ1~2S20~5`1~4SUY70~2`1~7_X0~3OZ1~2S20~5`1~7[002S1~8[00`1~2ZOfV1~2`]0`1~2ZO00`1~2ZOaZ1~2ZO0~4OZ1~2S20~5`1~3_X0Y1~3VN0`1~3R^U1~2`]0_T~28a0713~4L9D_X00b?3~98Pb?3~2?b0N83~2AR0]Z=33AeU933<[C>339U0XZE339V7U<33=1^0dA3~2DdfSE33EZ]0Y>3~7LS^0]VW~2?b0~22@66=V]0~2^U>L;;6=8POI3~2>CCA3~2IO00^`?<=LEAZb0~4^`?<=LEAZb0~6N83~99_00]Z=3~3=9AS20~2713~2D2^0~5213~2DCC@3~2120713~2DY>G6Ia713~2D20~6XZL33Aec0~5_933@20~9b?3~98P0]VW~2?b0~4_966>d0~4YA3~2<[0~4713~4LEAU70713~7Qf0~3YA3~2<[0~4713~6LSX0Y>3~7LS^713~2D22T3~28P713~2D20713~2D2Z<3~2DR0~4YA3~2<[0~4713~3?b^Z=3~2@2713~2EVOI3~28P]V=55@2071344H~2FG@e00b?5H~244H~268"
        "P^Z=JM>C0eA5438N00`<BB<[SLHJEVU9FK>C00eTG4;AYTG4;DY00PSEFF=1C>54GTe00Y>KH~34~2LS^0^Z=B5T2c002@5H=V]0~2eD;HFKB68P0`WMJL88LJMW`00]`A653LG591a0~2]`A653LG591a0~5YA:4FJ44H~2J9_00]Z=H4H4HF591b007134:D1SUY700^SW34:W~3543W177134:D`9F9U]7134:D20~6XZLHMA8SO0~4_9J5@2cc^a2O0~3b?5H~244H~268P0^Z=B5T2c0~3_9JK>d0~3]VEF4H=S70~37134HFFHF;AY071344H~36Qf0~2]VEF4H=S70~371344H~3LSX0Y>KH~34~2LS^7134:D22T5468P7134:D207134:DVW5H;>_^0~3]VEF4H=S70~371344KW[2T;445@271344;TRI;468P]V=B5@207134:==;HF9_00b?L~3JBL~38P0CWKJEZX_9FF9_0~2dAMJEVIGBF38SLH5@200XU<MJ=1EFK9V^0~2eTG45>QLJK<UX00Y>L~464K9ZX^V8=H5T88[02@5:=V]0~2_9BF=><L8P0O8LJM<WMJL8a00eT63AA>A9:<U]00eT63>~2@9:<U]0~3XVEFK<;4;L~39_00]Z=HKLLGK4M<UX07134:33LEAUc0713BgFK~3FgB3177134:DZ=5De07134:D2R~220~2XZLHMA=W_]0~3U9J5@8~2V?TI_P00b?L~3JBL~38P^V8=H5T88[0~2_9BK>d0~3OI64M"
        "H;T20~37134:993JFEU07134:L~4Qf0~2OI64MH;T20~37134:L~4SX0Y>L~464K9ZX7134:D22T5468P7134:D207134:D>;4;>UP0~4OI64MH;T20~37134HHL1`<KJg5@27134gB9VI;468P0VW~2?207134:DZ<BJEU007US~2EJB<S~2[002?;4GIa83H5Ae0~2b?;438A:;5;TIG46QO0~2bQLJMW:H3I20~3XU<KJL<MH3I20~2aZS~39KJLI20OI66H:66=Z]bID~2U0~3VEJMAY[8UX0RUA5H63H5>UR0XVEG@=5M:<EG?20XVEG@5;5;WEG?20~3b?G4G?;4GIS~3R00PZ=HK>SI9K4G?b071344H~2F;>[071<34:<~3543<177134:DZ>>1P07134:DUA~2Ib00XZLHM3F6?a0~2P1LFEQ6~29G56>`007US~2EJB<S~2[0OI66H:66=Z]00_W<<Ad0~3YA:FEBK<[0~37134:D_>KH=V07134:DS~3`]0~2YA:FEBK<[0~37134:DS~3C00aZS~39KJLI207134:DCCT5468P7134:D207134:A;H5<VN0~5YA:FEBK<[0~37134;:5D83F=H5@27134MBG?I;468P]V=:5@207134:DV<BF9_0~3XXUEJB<CXX0~2^Z=JM>CD5438N0~2P134;T<BL6M>A:H=ZX0~3CA5H;H:"">[0~5bQLJM64:"">[0~4X~2_W:HGDC00OIGB4FBB=Z]713;WC00X`V=H:DVVCX00VE~2JBBJ=E"
        "EV]OI69EB3<E<@;WC0OI69T5=>M=@;WC0~3CWMFE?;4GW~3De07Z?=HK>?ZI34M>Y07134:==6FH38f0R134:W~35431R07134:D2ddb007134:DV=M5@200XZLH4B3AUP0~2b?;GTQG46MMJH3IO0~2X2UEJB<CaX00OIGB4FBB=Z]00OC~3^0~2]VEFM>5H=S70~27134:"">>EFB9_07134:D2X~20~3]VEFM>5H=S70~27134:W~3>[0~2X~2_W:HGDC007134:W~35468P7134:D207134:6H:<ZO0~5]VEFM>5H=S70~27134;3B9A:5=H5@27134;;B9I;468P]V=K5@207134:==;JGDe0~5UEJB<C0~5CWKJEV9BF9_0~4VEJM>EJE=F9<BF9_0~4PZEBgF=1N0~6[>54HF=1N0~6CDGH:W_X00NQW=H5W~2V]NQ6F9`00a?==H:=~21^00V=::M44M::=V]2T5A6M>`ZI139_02T5AT5=LK9139_0~27S=J:A?;gB~3M>d0fQ6;4B;GIVEJF<[07134:DV@;g;?b07134FB~3F431P07134:D20~47134:DV=H5@20XYILHKE?C70~3RA<<UQG4BE>3H:DR0~2R?@EJB<@Qa00NQW=H5W~2V]00[D~2?R0~2OI64GI3H;T20~27134FKKFK=?R07134:D20~6OI64GI34;T20~27134JB~39`0~4CDGH:W_X007134JB~3F468P7134:D207134HHM91a0~6OI64GI3H;T20~27134;9B"
        "6EFL=H5@27134;9F6?;468P0U>~2?2071344HHJ=A_^0~5UEJB<C0~52?;46ILH5Ae0~4[<BF93KW9F=EJM>Y0~47V9BgFESc0~6fSEF4;Dd0~62ILFK9SO0~3RZ=H5TCCb0a?;JEU00a?;F4HFF31^00_D~2EFFED~2U02@5A6:ARCU139_02@5AT5L:9Q139_0~2RT;H3?""?;gM:~3>d0fQ6M4JMGIUEJF<`07134:DC?;45T207134B:~3B431P07134:D20~47134:DV=55@20`@LK4MAe0~5^C[[aQG4;?UEJMAe0~2Y>55HJ55@200OI33H533=Z]00_9::"">d0~2YA:J=Z9FK<[0~27134K;;5J:9VX7134:D20~6YA:J=Z9FK<[0~27134B:~39`0~32ILFK9SO0~27134B:~3B468P7134:D207134HHF=Ie0~6YA:J=Z9FK<[0~27134;@;MGK<=H5@27134;@;K<;468P]V=M5@207134:=3BF3Ib0~5UEJB<C0~5^Z=JM>5438N0~4RA:H=;5D>M634;?20~4CA;4:H5>[0~7d>KH=Z^0~5NS9KJLI20~4]Z=H5T20~22@5H=V]0a?9=H:9~21^00V=6~2FJ6~2=V]a?;W=B9?A<IG<[0a?;WT5==5AIG<[0~2_9BH;~2M4GD~3?R0P1>=HK>>11LHKWC07134:WWEK46IO07134:D~35431P07134:D20~47134:D_T~28a^SEB54MAe0~9NQG468_9JMAe0~2Y>::HJ:5@200OI"
        "GFgJFF=Z]00_9JK>d00]VEFK<~2:H=S7007134:D?A;4GIO7134:D20~5]VEFKWCA:H=S7007134:D~4C0~2NS9KJLI20~37134:D~35468P7134:D207134:6HB=QR0~4]VEFKWCA:H=S7007134;?=JJ;?=H5@27134;?EF6;468P]V=M5@207134:DILHK<`0~5UEJB<C0~6CWKFEBF9_0~5OIG46M6I?;5;438c0~3a8=JM<"":JLIb0~6d>KH=Z]0~4X_>54GD[~3e00]Z=H5T22X02@5H=V]0]CV=H:D``eb00V=6~2FF6~2=V]71=LA;K;5<<5DR071=LT5=>:E<5DR00O83HB~3F4GI[~32002Z=HK>[Z>:45@R07134FKKF4M9U]07134:DRR@5431P07134:D[~3R07134:D[~3R00e?LLHMA[~3d0~5NQG468_9JMAe0~2eTDEJB<DIb00OI9=H59~2V]00UEJ5>d00OI64HF~2J4;T2007134:DR8645T27134:D20~5OI64;?[Q64;T2007134:D[~3R00X_>54GD[~3e07134:DRRT5468P7134:D207134:A;HKE120~3OI64;?2Q34;T2007134;?>MJ=Z=H5@27134;?D;K;468P0V>~2?207134:D[WMH38O0~4UEJB<C0~62?;4MH5Ae0~5^1LHBJ=S83JFJEV]0~2]_WMJLQ=FK<UX0~5d>KH=Z]0~4Y@6H:<~58N0]Z=H:ADTC02@5H=V]0RTA=H:A~31f0C8~29FF98~2"
        "C00C>59AE=E>GESP00C>59~2899GESP00YA:J=<~2;4G<~4Z]0]Z=HK<<E5HFESP07134K5~26<12007134:D22@5431P07134:<~48f7134:<~48f0]`8LHM<~4IO0~4NQG468_9JMAe0~2]2UEJB<C20~3YZ=H:ADTC00P13FESf00YA:H;~4FK<[007134:AA<54;?a7134:D20~5YA:H=<~2EJK<[007134:<~48f0Y@6H:<~58N7134:D22T5468P7134:D207134:D>;4M9Sa0~2YA:H=S^UEFK<[007134;?I66>Z=H5@27134;?SEF4468P]V=K5@207134:D2I34M>C0~4UEJB<C0~6^Z=Jg438N0~6U9FgF9_VEJgB<[0~3R?6H5ACD;4;@d0~5d>KH=Z]0~4`9FgJB~4GQN00_9FJ::9`02@5H=V]0d>MM4BM~2GIO00PP`9FF9`PP0~2fZ9:L<WE;L?e0~2fZ9:L<WE;L?e00XVEFM>CC?;gFB~3=Z]0]Z=HHBBJ4BL?e007134:DTTQUb0~27134:D22@5431P07134JB~36Qf7134JB~36Qf00XZLHHB~3;?O0~4NQG468_9JMAe0~4UEJB<C0~5_9FJ::9`002?;GTe00]VEFB<?~2>MH=S707134B::K4B=S77134:D20~4]VEF4B~44H=S707134JB~36Qf0`9FgJB~4GQN7134:D22T5468P7134:D207134:DV<54:<ZO0]VEFK<[0Y>MH=S707134;?_88SZ=H5@27134;?"
        "YA54468P]V=55@2071355D2_<55=1P0~3UE55<C0~7CW5~29_0~7C>5~2>Y[<5~2Ae0~2^ZE55EZPV955=1P0~4d>55=Z]0~4`95~7GQN00RTL5~29`02@;;=V]0d>5~6GIO0~3`9559`0~5bSW355G9?C0~4bSW355G9?C0~2b?G56Iaa?;5~5=Z]0]Z=5~3;39?C0~271355D22O]0~371355D22@5531P07135~56Qf7135~56Qf00XZL5~5;?O0~4NQG568_955Ae0~4UE55<C0~5RTL5~29`00RD<<UX00OI65;TROaI65;T207135~4GE?d071355D20~4OI65~8;T207135~56Qf0`95~7GQN71355D22T5568P71355D2071355D2Z<5~2WU]OI65;T20aI65;T207135;?aPP7Z=55@27135;?aS=5568P0_@~2Qa0XV@~2Ib2I@~2SP0~3`@~2?Y0~72I@~2?e0~72I@~2I2R?@~28a0~2^Z@~2?R02Q@~2SP0~4R?@~2U0~5CT@~8SP00]C8T@@TC0a8?~2_00R?@~71c0~3CT@@TC0~6O`8T@IVR0~6O`8T@IVR0~3aQ@~2U]N1@~7U0~2U@~3T?8U20~3XV@~2Ib0~6^Z@~2QbaQ@~2Z^0XV@~7SPXV@~7SP0~2U@~71N0~4PS@~2Z[@~2?20~4`@~2?Y0~5]C8T@@TC00^C~2O0~2O8@~2SP0XV@~2I20XV@~4TIZe00XV@~2Ib0~4O8@~:I20XV@~7SP0CT@~8SPXV@~2IbaQ@~2"
        "Z^XV@~2Ib0XV@~2Iba1@~3U0O8@~2SP0XV@~2I20XV@~21N0~2U@~2QaXV@~21NCT@~2Z^0P2~2a0~2N2~2b00b2~2O0~4^2~3]0~8b2~30~9b2~2b002~3a0~4N2~2b0~2a2~2O0~62~3P0~5X2~9O0~4N2~3X00Oa~2^0~2b2~7O0~4X2~3X0~8f22OX0~9f22OX0~5a2~2c00O2~7P0~2P2~4ac0~6N2~2b0~8N2~2a00a2~2N0~2N2~7O00N2~7O0~3P2~7O0~6O2~2N^2~2b0~5^2~3]0~7N2~3X0~:a2~2O0~2f2~2b0~2N2~5OX0~3N2~2b0~6a2~:b0~2N2~7O00X2~9O00N2~2b00a2~2N00N2~2b0~2N2~2b00O2~3P00a2~2O0~2f2~2b0~2N2~2O0~3P2~2a00N2~2O0X2~3N0~:7~6X0~27~90~37~4^0~67~70~2X7~9]0~2^7~2]0~27~30~7^7~20~77~2^0~2^7~20~3^7~2^7~2^7~20~7POON]0~6PPX0~4^^0~57~70~3^^X0~9]baP0~9POO^0~3NaN0~37~2X0~87~3]0~27~3X0~27~2X0~5]7~70~87~2X007~2X0~7X7~80~9^7~2^0~77~7^0~37~4^0~67~7]0~27~70~37~30~27~2^00X7~2X0~37~2X007~30~37~2X00]7~30~5]7~70~37~3]0~27~3X0~27~2X00X7~20~37~90~37~4^0~5X7~9]0~5C1~6U70`1~9`]0`1~5VYP0~2`1~7[0PV1~9_]0"
        "bS1~2U^0d1~3C0~521~3Y0~5C1~320bS1~2`002S1~2S1~6Y0~42U1I?8Ve]0~2]`81ZCO02_1SC]0~2`1~7[00aS1S_R0~7C?""?1_b0~621?IZR0N_Q?Q[00`1~2ZO0~6`1~3_X0Y1~3VN0`1~2ZO0~3^U1~7[0~6`1~2ZO`1~2ZO0~5PV1~8C0~7OZ1~2S20~5`1~8200`1~4SUY70~2`1~7_X0`1~7[00R1~3C0C1~2SbPU1~2Vc00`1~2U7R1~3`]0`1~2ZO^_1~3R0~3^U1~7[00`1~3_X0Y1~3VN0`1~2ZOfV1~2`]0`1~9`]0`1~5VYP00PV1~9_]0~3XZL3~5?a713~98P713~4LEAVf0713~6LSXb?3~98P0dA3~2DdfSE33EZ]0~3NCW33EUa0~3XZE339V7U<33=1^0Y>33=W33E<33EU0~3YI<3;;6ET`^00P133=>SC?9LLTY00713~6LSX0eA3L9?e0~6`9;3<8d0~5CW;GE8bU>6;LZX713~2D2^0~4713~3DCb8=3~2@2713~2D20~3OI3~6LS^0~4713~2D213~2D20~5b?3~7=V0~7YA3~2<[0~4713~7<C0713~4LEAU70713~7Qf713~6LSX0[<33LIRV=33>Y`>3~2?20713~2?aV93~28P713~2D2[A3~2<[0~3OI3~6LS^713~3?b^Z=3~2@2713~2D22T3~28P713~98P713~4LEAVf0b?3~98P0~3XZLH~4;?a71344H~34468P71344HH4J;>`071344H~3LSXb?"
        "5H~244H~268P0PSEFF=1C>54GTe0~3d1A9BJE>QC^0~2eTG4;AYTG4;DY00Y>KH=WBJE<BJEU0~2RILK4FFHF6DC00P13HJ:<?3BHFEV]071344H~3LSX0eAMHBEV00^N0~2`9MHM=?d0~4CWB4M<UTGHHLZX7134:D1SUY70071344FLQV9M445@27134:D20~3OI;44HH44LS^0~47134:D2134:D20~5b?5H4~2H~2=V0~6]VEF4H=S70~371344H~3B<C07134HFFHF;AY071344H~36Qf71344H~3LSX0OS9KBE8V=HK>CD6H;A`]07134;?R?G4468P7134:DCT3JM9SO0~3OI;44HH44LS^71344KW[2T;445@27134:D22T5468P71344H~34468P71344HH4J;>`0b?5H~244H~268P0~3XZL~6?a7134:L~35468P7134:==3FH38N713J5L~4SXb?L~3JBL~38P00eTG45>QLJK<UX0~2d?=:F44FM3D[0~2XU<MJ=1EFK9V^00Y>KH=WBJE<BJEU00^ZEBHG993FJL8O0P1=L:4;9BBGJLSX07134:L~4SX0eA95H=ZPdZQe00CTW3BB=8a0~3CW354GTEFB3LZX7134:33LEAUc07134H4K9A;4Jg5@27134:D20~3OI;4;LLM4LS^0~47134:D2134:D20~5b?LL34:L~2=V0~6OI64MH;T20~37134:L~4<C07134:993JFEU07134:L~4Qf7134:L~4SX002Q=BM9Z=HK>?LJ:<"
        "VP007134;?U9Bg468P7134:D?LFKE1b0~4OI;4;LLM4LS^7134HHL1`<KJg5@27134:D22T5468P7134:L~35468P7134:==3FH38Nb?L~3JBL~38P0~4CS~6_^7134:DSST5468P7134:DVT;g;?b0`A6J6AS~2C07US~2EJB<S~2[0~2XU<KJL<MH3I20~2f1=BH:FH5JJ3?20~2bQLJMW:H3I20~2Y>KH=WBJE<BJEU002T;4:"">U`TG4M>C00[Z1W5K6F=<;<`007134:DS~3C007C?;B<`1AEWC00]2[I=FM<_0~3R119BFE:H3I8[071344H~2F;>[07134;5453JB=H5@27134:D20~3OI;4;?AM4LSYRf0~27134:DC134:DCda0~37US134:DS~2Ya0~5YA:FEBK<[0~37134:DS~3Z207134:D_>KH=V07134:DS~3`]7134:DS~3C0~3d?3J5W=HK>EBK9SO0~27134;?""?6FM468P7134:D=BB=QR0~5OI;4;?AM4LS^7134;:5D83F=H5@27134:DCCT5468P7134:DSST5468P7134:DVT;g;?b7US~2EJB<S~2[0~5OQW~4Ad07134:D22T5468P7134:DC?;45T20^[@3FG>VO0~3XXUEJB<CXX0~4bQLJM64:"">[0~3eA545WBJE6HK<[0~3CA5H;H:"">[0~3Y>MH=WBJE<FF9_00C>K431NXVEJF9U0~2XOS=HF5AZISa007134:W~3>[0~2eA::A>=5;T20~32?3MH6I"
        "a0~3P2?3HKH:"">`c]07134:==6FH38f7134;9KHJJ3=H5@27134:D20~3OI;4;IAM4LW>D1Y007134:W~234:WWAI_N0~3^134:W~3>QY0~3]VEFM>5H=S70~27134:WW>D1CX07134:"">>EFB9_07134:D2X~2007134:W~3>[0~4CDGH;=HK9MB=820~37134;?<K;;468P7134:EKFL?e0~6OI;4;IAM4LS^7134;3B9A:5=H5@27134:W~35468P7134:D22T5468P7134:DC?;45T200XXUEJB<CXX0~6OI;B~2K>d07134:D22T5468P7134:WWEK46IO00XC?LF;W`0~5UEJB<C0~7[>54HF=1N0~3C<BH=WBJE9FJEV0~3PZEBgF=1N0~32D546WBJE=JK<[00[<BH=V]0[<BJ=V0~4`9F468NOP0~27134JB~39`0~2[<BG3:JHL1P0~2PZ9B9MM>C0~5`>:gFE1N0~27134:DV@;g;?b7134;?LJ4;A=H5@27134:D20~3a?;4GIAM4BBK5=?d07134JB~3gJBKM6>UX0~2^134JB~3M=Ib0~2OI64GI3H;T20~27134JBBK5L@C07134FKKFK=?R07134:D20~47134JB~39`0~4R?6HJ;HB54BESP0~37134;?6FE;468P7134MM4BESf0~6a?;4GIAM4LS^7134;9B6EFL=H5@27134JB~3F468P7134:D22T5468P7134:WWEK46IO0~3UEJB<C0~8OI;:~3>d07134:D22T546"
        "8P7134JBBF4M9U]00^_A;F3?Y0~5UEJB<C0~7fSEF4;Dd0~4C<BH=WBJE<BJEV0~37V9BgFESc0~3N83HK9BJE54;@R00C<BH=ZX0`9FJEV0~4C>KHLS^0~47134B:~39`00]VEJJHFJB9_0~3[A53=FJEV]0~37V9B4H3I20~27134:DC?;45T27134;?DGG<Z=H5@27134:D20~32T5468AM4::K4FEZ^7134B:~2M4B:MJHG?20~2^134B:~2BgKWC0~2YA:J=Z9FK<[0~27134B::KHF=1P7134K;;5J:9VX7134:D20~47134B:~39`0~3PZEBJK4ggHM45AC0~37134;<K;D;468P7134gH:45>[0~62T5468AM4LS^7134;@;MGK<=H5@27134B:~3B468P7134:D22T5468P7134JBBF4M9U]0~3UEJB<C0~8N1D~4?R07134:D22T5468P7134K55;3<1b00][D6J3@C0~6UEJB<C0~8d>KH=Z^0~4Y>M46WBJE=JF9_0~3CA;4:H5>[0~4`W:4BH4B4BEZ700eA:46IaPS=JB<`0~4d>KH=Z]0~47134:D~4C00f83HKGEM5Ae0~22I35W:LJ31P0~3CA;4;FK9V^007134:WWEK46IO7134;?UII1Z=H5@27134:D20~3RA:4L1AM4LD<"":4GIO7134:D~234:DA34K>C0~2^134:D~2EJJEU00]VEFK<~2:H=S7007134:DDW54;?b7134:D?A;4GIO7134:D20~47134:D~4C0~3C"
        "A54;EKg459KJLQb0~27134;3FEI;468P71344;<KH3I20~5RA:4L1AM4LS^7134;?=JJ;?=H5@27134:D~35468P7134:D22T5468P7134K55;3<1b0~4UEJB<C0~82[~6d07134:D22T5468P7134:DT?8Ua0~2CT3JGA[~2R0~4UEJB<C0~8d>KH=Z]0~4aI3HF6BJLK4;De0~2a8=JM<"":JLIb0~3O19:H4gHK=IR0~228LJM<V`A5HG?e0~4d>KH=Z]0~47134:DR~3^002T53<?9F6Q200^Z95E6;WKGIO00PC8=JM<G4;D[207134FKKF4M9U]7134;?aOOcZ=H5@27134:D200O[S9BJEVAM4LSI;4;?O7134:DR134:DUEJB<C0~2^134:DRC<BJEV00OI64HF~2J4;T2007134:DR8645T27134:DR8645T27134:D20~47134:D[~3R0~2bQLJK9ILHK<I3HK<UX0071345M5AI;468P7134:WQLJK9V70~2O[Z9BJEVAM4LS^7134;?>MJ=Z=H5@27134:DRRT5468P7134:D22T5468P7134:DT?8Ua0~5UEJB<C0~7XZ<~6IO7134:D22T5468P7134:D2bN0~371LF;<~48f0~3UEJB<C0~8d>KH=Z]0~5`A6F44g4J;<V700]_WMJLQ=FK<UX0~321W=BJ=9IY0~2XS<<3J;>T6J6<<IO0~3d>KH=Z]0~47134:D20~5dA>8`1343AIa0XV@@?@@AM;IO00YA9MHL<<KJ=<V]71"
        "34K5~26<1207134;?a00]Z=H5@27134:D200[W=:4KWCAM4LA<"":46QN7134:D2134:A>34M>Y0~2^134:DRCWBJEV00YA:H;~4FK<[007134:AAW54;?a7134:AA<54;?a7134:D20~47134:<~48f0XU<MH3IV=HK>[>:4G@d0071344J=SI;468P7134:DCA54;AC0~2[WE54KWCAM4LS^7134;?I66>Z=H5@27134:D22T5468P7134:D22T5468P7134:D2bN0~7UEJB<C0~7XZLB~4;?a7134:D22T5468P7134:D20~57134FB~36Qf0~3UEJB<C0~8d>KH=Z]0~57_T93BH3EAVa0~2R?6H5ACD;4;@d0~4O[WBJEU20~3XSLBB4K>T5gFB;IO0~3d>KH=Z]0~47134:D20~5P[COP83HF5T200N22b22D5;IO00Y>K44B~2HgJ=V]7134:DTTQUb007134;?a00]Z=H5@27134:D200`9FHKLIRAM4M:K4K9V]7134:D2134B:MJJ3Ib0~2^134:DRCWBJEV0]VEFB<?~2>MH=S707134B::KHKEZ^7134B::K4B=S77134:D20~47134JB~36Qf0eTG4:"">`V=HK>Y1EFFE1N07134g5ACI;468P7134:D2Z9BJL8a00`9FHKLIRAM4LS^7134;?_88SZ=H5@27134:D22T5468P7134:D22T5468P7134:D20~9UEJB<C0~7XZL5~4;?a71355D22T5568P71355D20~57135~56Qf"
        "0~3UE55<C0~8d>55=Z]0~6XeUW55EVYP0~2^ZE55EZPV955=1P0~4CW55EU0~4XSL5~3>T5~3;IO0~3d>55=Z]0~471355D20~:_>3;5T20~6bT;6QN00Y>5~8=V]71355D22O]0~27135;?a00]Z=55@271355D200`9;L<IYRA5~46982071355D2135~3;=D[0~3^1355DRCW55EV0OI65;TROaI65;T207135~469IR07135~4GE?d071355D20~47135~56QfPSE55=1NV=55>YeTG55>[071355=1OI;568P71355D22?355<U00`9;L<IYRA55LS^7135;?aPP7Z=55@271355D22T5568P71355D22T5568P71355D20~9UE55<C0~8U@~61NXV@~2IbaQ@~2Z^XV@~2Ib0~5XV@~7SP0~3`@~2?Y0~8R?@~2U0~9d?@~2`0~4^Z@~2?R02Q@~2SP0~4d?@~2`0~4]U@~4?Q@~41c0~3R?@~2U0~5XV@~2Ib0~:f_8?T8O0~6O1?""?V^00R?@~9_0XV@~2Ib0~5XV@~21N0~2U@~2QaXV@~2Ib00CT?1_202I@~3TIU200XV@~2IbV@~4?SYX0~3XV@~2I2d?@~2`0O8@~2SP0XV@~2I20XV@~4TIV200XV@~4TIZe00XV@~2Ib0~4XV@~7SPPS@~2?d0_@~2?RXU@~3[0XV@~2TCc1@~2Z^XV@~2Ib0`@~3U00CT?1_202I@~2V]XV@~21N0~2U@~2QaXV@~2IbaQ@~2Z^XV@~2IbaQ@~2"
        "Z^XV@~2Ib0~9`@~2?Y0~8P2~6O00N2~2b00a2~2N00N2~2b0~7N2~7O0~4^2~3]0~92~3P0~9]2~370~5N2~2b0~2a2~2O0~62~370~5c2~5a2~4O0~52~3P0~6N2~2b0~=ca2O0~8fOO70~32~:P00N2~2b0~7N2~2O0~3P2~2a00N2~2b0~2X2aP0~3b2~4O]0~3N2~2b0N2~4a70~6f2~2b002~3700a2~2O0~2f2~2b0~2N2~5OX0~3N2~5OX0~3N2~2b0~6N2~7O00O2~300P2~300P2~3^00N2~3X0O2~2N00N2~2b00^2~3P00X2aP0~3b2~2c00N2~2O0~3P2~2a00N2~2b00a2~2N00N2~2b00a2~2N00N2~2b0~:^2~3]0~97~2^0~2^7~20~8^7~20~87~2^0~2^7~20~2]7~2X007~30~37~2X007~2^00^7~20~37~6X0~87~2X0~57~2X0~37~2X0~<^7~4]0~4c~70~3Xc~2XXc~2X0~3Xc~2X]c~2^0c~270~3Xc~2XXc~2X0~4^7~20~;P~270~<7~3]0~87~2X0~4^7~2X0~8X7~9^0~37~2X^7~9^0~8X7~20~9X7~2]0~3]NaO^0~37~2X0~E7~4^0~C7~7^0~3c~2P0Pc~20^c~2^0~2Xc~2X0c~30~37~2X00]7~30~47~2X00]7~30~37~6X00X7~2^0~2Xc~2XXc~2X0~5C1~320bS1~2`0~62S1~2Y0~6C1~320bS1~2`0^U1~2U7d1~3R00`1~2ZOY1~3221~3Y00"
        "R1~6U70~6`1~2ZO0~3`1~2ZO00`1~2ZO0~8^dUS1~4_X0~2e8~7C00^U8~2UU8~2UX00^U8~2U_8~2VC8~2SO00^U8~2UU8~2UX0~221~3Y0~9R1~320~:Y1~3_X0~6`1~2ZO0~22S1~2ZO0~6NV1~:200`1~2ZS1~9ZO00RY~2bNV1~2`0PY`_Cc0PV1~2_X00OUQ?IVP00`1~2ZO0~7RC~2R0~6`1~5VY70~7NCCY^0~221~7Sb00C8~21R18~2YZ8~2V70^U8~2UC8~3R00`1~2ZO^_1~3R00N`1~2Zb^_1~3R00[1~6U7cU1~2Sb0^U8~2UU8~2UX0~3XZ=339U0`<33L1^0~3Od`W33EVCa0~3XZE339V7U<33=1^OI3~2?O_933<[0713~2D2UE33WCC<33EU00[<3~5?O0~5713~2D20~2713~2D20713~2D20~7X_DEL3~48c0~2[<6~5=V00NQ6~28Q6~28P00NQ6~281366IV=66@200NQ6~28Q6~28P0a_U<33EUU20~7[<33WC0~9P1=3~2?20~5713~2D20~2_<3~2>C0~62T3~9>C0713~2DA3~9AR0OI>~2S2D3~21PVA99>C0b?3~2?b0^V<6;G?a0213~2De]0~5P1W~28c0~4713~4LEAUf0~6`>WAd0~2C<3~6>Y00V966E8A669VW66=QO0NQ6~28?366>C0713~2D`[D3~2<[0XVD3~2DI[A3~2<[0^13~6?O_>3~2AY0NQ6~28Q6~28P0~4YA:46Ia83HM>[0~2XC8A9E"
        "KJ=9>I`f0~2eTG4;AYTG4;DY0OI;4;?O_9JF<[07134:D2UEJBWCC<BJEU00[<FHH44;?O0~57134:D20~27134:D207134:D20~7dDGFH~3468c0~2[<FHBBJH=V00NQG468QG468P00NQG468134;IV=H5@200NQG468QG468P0[<EEBJE~2U0~52Y[<FKWCYYN0~6YA54HB<U0~57134:D200aI64HJ=1f0~5^V9B4H~34HL8O07134:D1=J4H~34FESP0c1LK;?2D:431YA;FB<C0^Z=JM>C0RT;44;?a^SW34:W?20~5P86B6Qf0~471344HH4F;>`0~2Xc~2_9K>Y0~2C<B4H~3K>Y00b19MKEAMJ9>;FL?Y00NQG46?LF5WVf07134:A9T3JM9SO0^13:4B:DT3JM9SO0^13H~244;?`AGH5WUP0NQG468QG468P0~4N1LHMW[>:46Ia0~2C@L:FJ4gHFM6>U^00XU<MJ=1EFK9V^0OI;4;?a_9JF<[07134:D2UEJBWCC<BJEU00[<L~2;4;?O0~57134:D20~27134:D207134:D20~7U9FJ6==5468c0~2[<FBWW:H=V00NQG468QG468P00NQG468134;IV=H5@200NQG468QG468P0[<FJ44JJEU0~4P1>><FKW>~2`0~57ZEFMG46Ib0~47134:D200`WMF=J5AY0~6RTGFL~3K:"">[007134:DCA5KL~3B;Dd0~2`<BKWCD:431ZEJ:=<C00CWKJEZX_9BJG3?a^13BgF:D20~4O"
        "S8646QSO0~37134:==3FH38N0^U8~39K>Y0~2C<BJL~4>Y0~221EKM9MJ9;J3TC0~2NQG46=B5<VO007134:A5LFKE1b00^1E34:EDLFKE1b00^1L~3;4;?@6JM<ZO00NQG468QG468P0~5[WMHL8=JB9U0~2OQLJJ56BH3;FH;De0~2bQLJMW:H3I200OI;4;?a_9JF<[07134:D2UEJBWCC<BJEU002ZSSI;4;?CYRc0~27134:DCCRN7134:D207134:DCCRN0~3]Z=HK>VT5468c0~2`9FKWD:H=V00NQG468QG468P00NQG468134;IV=H5@200NQG468QG468P0[<;;FH;;EUbX0~2P86KKHJK~29_0~5eD;H39FK<`0~47134:DC~283H;>:J=S70~5XV9BG?1LJL1O007134:DCS=F3QI3FEZ70~2RT;J=ZD:431?GHL1Z2002?;4GIa83H5A8ZP^1<34:<TCRN0~22@6;4;6D20~37134:DS>;g;?b0NQ6~4K>Y00aC<BJES~3Za0~3RQEB:BJ;H6D[]0~2NQG4;B:<ZO0~27134:AMBB=QR0~3Y134:D=BB=QR0~3CS~2I;4;?LFK91a0~2NQG46EEG468P0~5aI64:A;4;TR0~2dA:4;AWKJETLJF9_0~3CA5H;H:"">[0~2OIG4:D889JF<[07134:D2UEJBWCC<BJEU0~2XXOI;4;WW>D1d007134:WW>D1C134:D207134:WW>D1CX00]V=JK>VT5468c0~2U9JM>D:H=V00NQG46"
        "8QG468P00NQG468134;IV=H5@200NQG468QG468P0dII<BJEWW>?UO00P86GGFBG~29_0~4XVEFK<@;H3QO0~37134:W~4MFE8LH;De0~6R?GB9>::"">C0~27134:W~35MW<K;@e0~37S=HG?D:431>MK<[X0~2^Z=JM>CD5H38f^00R134:WW>D1CX02@5BgB:D20~37134:DW;;45T20NQG4:5~2>Y0]V><BJE>~2VX0~4RI3JJggHH5>[0~3NQG44FBESO0~27134:EKFL?e0~47134:EKFL?e0~5XXOI;4;=BB=820~3NQG4FBBF468P0~6U9BJ=BFEZ^0~2C<BH=ZWKJEV<BH=V]0~2PZEBgF=1N0~2P13HJG6~2JF<[07134:D2UEJBWCC<BJEU0~4OI;4FBBK5=?R07134JBBK5L@134:D207134JBBK5L@C0~2C>5J6==5468c002S=H5DD:H=VO0NQG468QG468b00NQG468134;IV=H5@2X0NQG468QG468P00OC<B4BBKMG<ZP0XUII<FKWIIQY0~4RT;46<<EFM>[0~37134JB~4H5<~2MFEV^0~5XU9B;LJ=1N0~27134JB~3FH36FEV^0~4[WKM>D:431EJ;?20~4CWKJEV9BB9_0~37134JBBK5L@C02?W646WTCa0~27134:WWLH46IO0NQG46T~2?R0XSLKH4K~2=Z]0~3fS9BMLFH;GH3?RP00NQG4MEB:"">_P007134MM4BESf0~47134MM4BESf0~7OI;4:KgK9V^0~3N"
        "QG46<<G468PP0~5RT;4F4:AY0~3C<BH=ZWKJEV<BH=V]0~27V9BgFESc0~3_<"":JH~3gF<[07134:D2UEJBWCC<BJEU0~4OI;4K::K4BEV]7134B::KHF=134:D207134B::KHF=1P00fZ<;H~3468c0e?W546DD:H=D1cNQG46DDG46DI20NQG46DD34;DD=H5DTY0NQG46DDG468P0~2C<BH:~2F45Ae00^N[<FKEE9>120~2]U9BgB~34HL1N0~27134B:~3JgFB~2J4;TR0~4][TE5FKKLAUc007134B:~34gKBM=D`^0~32?;JE>:43>GJ=S^0~42?;46ILH5De0~37134B::KHF=1P]d8646E9WIY007134JBBF4B91a0NQG46822b00XSLMJ4M~2=Z]0~3`>5F=AMJ9<MBE81b0NQG46?6H6T8[07134g4~25>[0~47134gH:45>[0~7OI;4gJM4;DY0~3NQG468QG46881R0~4^VEFgHL1N0~3Y>M4GDWKJE?=JF9_0~3CA;4:H5>[0~3aSW=~4JF<[07134:D2UEJBWCC<BJEU0~4OI;4;DA<M468f7134:DDW54;?34:D207134:DDW54;?b0~2aI3HM=5468c0Y>MH4:~2K4:GIONQG4:~2M4M:DR0NQG4:~34M:~24K:<[0NQG4:~2M468P0~2C<BJEDALJB<`0~3[<FHBBF:EZ7002?645;BJ;;J:"">C0~27134:D~2=JM;:45;KB9U]0~3dT6FJJHFJ;>`007134:DDAGHJHJ~2"
        "6DC0~3^Z=JMMF4KMBMWC0~5^Z=JM>5438f0~37134:DDW54;?b0P864FBJMLQO07134K55;3E;AR0NQG468P0~3]V><BJE>~2V]0~2R?3H6TAMJ91=F56Ae0NQG468>5F6=Z]71344;MKH3I20~371344;<KH3I20~6OI;4J6EBF=1O0~2NQG468QG4;66<C0~4d[A5gMW[0~4aI3JJ;3BJLGB4;Ad0~2a8=JM<"":JLIb0~3O[V~39JF<[07134:D[UEJBW[[<BJEU0~4OI;4;?R?;4GIN7134:DR8645T34:D207134:DR8645T20~2_<MHLT5468c0Y>K:5~5KGIONQG5~5::DR0NQG5~;B<[0NQG55KM5568P0~2C<BJEU`<FF9_0~3[<FK<<3H;?a00_<KB9>MJ9A:J=1P007134:DdA5HL?;43?64G?20~27S=FBL5MEMH3Ia07134:D8LJK=M5=KJL1N0~3C>MHH4gH~2G?b0~6CWKFEBB9_0~47134:DR8645T20P8646<=FMAe07134:DT?88WU70NQG468P0~4OC<BJEUddO0~2PSE55W_A559_T6HMAe0NQG568Z9KH=Z]7134:W5LJK9V70~27134:WQLJK9V70~5OI;4;A?6HMW_]00NQG568QG55JB<C0~3OI<9:46Ib0~5[D3MFH4gHJKGWV700]_WMJLQ=FK<UX0~4]~2_9JF<[07134:<~2EJB<~3BJEU0~4OI;4;AA<M431P7134:AAW54;?34:D207134:AAW54;?a002I6H"
        ":"">T5468c0Y>KL@~55GIOPS@~66:DR0PS@~;=B<[0PS@~25;@~2Z70~2C<BJEAALJB<[0~3[<FKW8EJ5T20aI34;T>MJ91LH5AY007134:DZEFMWI;4319BK<_0~22T;46@5M>=JM>C07134:DA:HL>M5T3H5AR0~3aI=~2:43==EVX0~62?;4MH5De0~47134:AAW54;?a0P8646QWKK>Y07134:D2bN2CO00NQG468P0~5C<BJEU0~5PS@~212I@~2[_9JMAe0PS@~2Z2T5H=Z]7134:A5A54;AC0~27134:DCA54;AC0~5OI;4;?_W:H6?e00PS@~2ZS@@<BB<C0~3a?;F4K9U]0~5^CQ>E=KJLEW?UO0~2R?6H5ACD;4;@d0~7_9JF<[07134JB~34HB~3HJEU0~4OI;4K::K4M<U07134B::KHKE134:D207134B::KHKEZ^0]U<KJL1T5468c0Y>KLS2~3T5GIO0O2~586:DR00O2~:V=K<[00O22?5;?22O0~3C<BH::MFJGTR0~3[<FB66:H6IO0`WMFEZ>MJ9_>MFEZ707134:DT;H3QI;431T;46Ib00Y>KFE@5M>>KF9U07134:D<BB<>M5T9BB<[0~3][ZZD:431ZZe0~7^Z=Jg438N0~47134B::KHKEZ^0P864;6;H;@R07134:D20~6NQG468P0~5C<BJEU0~6O2~2O0b2~2^_9KMAe00O2~2N2T5K=Z]7134:A9?9BJL8a007134:D2Z9BJL8a0~4OI;4;?O1=FFESN00O2~2N"
        "O2C<KK<C0~3a?;5GEQ20~8aCUW55EV[2]0~2^ZE55EZPV955=1P0~6_955<[07135~=EU0~4OI;5~46<1b07135~469I1355D207135~469IR002?655ACT5568c0YAWWZX002?WW8N0~6P1WWT20~=UWW>C0~3a?WWIO0~5C<5~4;LA_X0~3[<5~43A`0O8355Dd>559_Q35;@R071355D955<_I;531VE55<`00C<55<@55>D55EV071355D955>>55T>559`0~5]2D55317]0~9CW5~29_0~57135~469IR00P865~36<V7071355D20~6NQG568P0~5C<55EU0~A`>WWDR0~6b?W~2V]71355D_C?355<U0071355D22?355<U0~4OI;5;?OY@;55>[0~8CAWW>C0~3N1@TIVR0~;e?@~2`0~5^Z@~2?R02Q@~2SP0~6[T@@TC0XV@~?`0~4c1@~4TQUb00XV@~4TIV2V@~2Ib0XV@~4TIV20~22I@~21O8@~2S70^YCCb0~3eCCR0~82CCd]0~=aCCYP0~4RCCR0~6Y?@~4?1C70~4CT@~3T8[P0N1@~2SR?@@T[`@~2?R0XV@~2I@~2I21@~2Ve?@~2`00YT@@?Q@@?I@~2_0XV@~2I@~2I?@@Q?@@TC0~6bQ@~2VX0~:2I@~2?e0~5XV@~4TIV20~27Z@~3TQ_O00XV@~2Ib0~6PS@~2Z^0~5YT@~2[0~ANC~2YX0~7eC~2a0XV@~2Ib0`@~3U00XV@~2Ib0`@~3U0~4c1@~21N^U@~2T[0~8"
        "PYCCYP0~4O22OX0~=2~370~6N2~2b0~2a2~2O0~7^2~3X00N2~?70~5O2~5O]0~3N2~5OX0N2~2b0~2N2~5OX0~4b2~2O0a2~2O0~p]2~5aP0~6X2~5N0~3O2~2O0b2~2^72~30~2N2~2b2~2b0O2~2N02~3^00]2~3a22bb2~2700N2~2b2~2bb22a2~3X0~7a2~2N0~<b2~30~7N2~5OX0~4O2~4O0~4N2~2b0~8O2~2N0~6]2~3^0~VN2~2b00^2~3P0~2N2~2b00^2~3P0~5O2~2O00P2~3^0~B7~2X00X7~70~27c~7P0~3^7~2]0~27~30~3^7~2]0~27~30~2]c~270Xc~270~27c~7Pc~20~6Pc~20PccP0~4]7~2X007~30~2]7~2X0~=XO22f0~57~2X0~3Pc~20PccP0~77~:0~2Xc~5P0~37~2^0~2^7~20~8^7~20~5]7~70~2]7~2^0~37~4^0~3X7~2X0~7^7~90~37~2X007~2^00^7~20~37~2X00]7~30~2Xc~7X0~4X7~2X7~20~6]7^0~:]7~70~7POaNX0~3X7~2^07~2X0~37~2X007~2^00^7~20~3]7~200^7~20~3]7~9]0~2]7~4X0~:7~30~77~80~9`1~2ZOfV1~7Y0NZ8~8R00bS1~2U^0d1~3C00bS1~2U^0d1~3C00`8~2SOU8~2Zf0NZ8~;C0~4a18~2C8~3R0~2^U1~2U7d1~3R0^U1~2U70~;RZITT8`f0~2`1~2ZO00a18~2C8~3R0~5[1~:d0^U8~51"
        "200C1~320bS1~2`0~6OZ1~2[0~3^U1~7[0X_1~2ZO00`1~5VY7PV1~2VP0~3^dUS1~9e00C1~2ZOe1~32bS1~2d00`1~2ZYY_1~3R0^U8~7UX0~2NV1~2V1~2C0~3NCZ11UR]0~6X_1~7C0~4c[1I?QZC007U1~2S`1~2U700C1~2ZOe1~32bS1~2d00XU1~2CbS1~2C00^U1~9U^0]_1~4S`a0~7R1~3R0~5`1~8R0~7713~2D22T3~6EU0bT6~7<C00dA3~2DdfSE33EZ]0dA3~2DdfSE33EZ]^SE66W_@663?a0bT6~7<66=V0~4RA66EU<66<C0~2OI3~2?O_933<[0OI3~2?O0~7dCCdIEG5;3>VP0713~2D200RA66EU<66<C0~4N83~99_0NQ6~5>Y0XZE339V7U<33=1^0~5RA3~21^0~2OI3~7?2eT3~2AR0713~4LEAU[>3~2?a0~2X_DEL3~89`0]Z=33AeU933<[C>339U0713~2A>>A3~2<[0NQ6~78P0~22@33=@33=V]00bS>=3L9TY0~6fQ3~6=V]0~2^U>L;;6=8P0O?3~2W13~2?a0]Z=33AeU933<[C>339U00NI33=VV<33EV]0OI3~9IO0P83~4=<8R0~6U9339U0~4OQ3~7<[0~77134:D22T544H~2JEU0bT5~2BH5~2<C00PSEFF=1C>54GTe00PSEFF=1C>54GTe00e?6H6?=F5>`00bT5~2BH5~2<5H=V0~4RAMJEU<BB<C0~2OI;4;?Q89JF<[0OI;4;?O0~6a?<"
        "<1=BB;5J;>C07134:D200RAMJEU<BB<C0~4YA:4FJ44H~2J9_0NQG4:5~2>Y00eTG4;AYTG4;DY0~6RA:431^0~2OI;44H~24B9VSEFB9V7071344HH4F;>?3HMW_]0~2dDGFH~344H~2F9`00`<BB<[SLHJEVU9FK>C07134:DLM3JM9SO0NQG4B~3468P0~22@5H=@5H=V]07V95B55BWCc~3X00fQ644H~3=V]0~2eD;HJKB68P0O?;4J=134;?a0PZ9BB<?""?LHJE?""?9FK>U]0NIGH=V>5FLIR00OI;H~244H~2;IO0P864H~3MESa0~4f834438N0~3C>;;:HH;;G?20~77134:D22T546L~3EU0O8TTAMJETT?Y0~2eTG45>QLJK<UX0~2eTG45>QLJK<UX00]_W:BE:BESN00O8TTAMJETT@5H=V0~4RAMJEU<BB<C0~2OI;4;?6<9JF<[0OI;4;?O0~6a?;6>:F=?A;JL1P7134:D200RAMJEU<BB<C0~3XVEFK<;4;L~39_0NQG46T~2?R00XU<MJ=1EFK9V^0~6RA:431^0~2OI;4;L~2G4;A>5H3?2007134:==3FH3EBJ=8a0~3U9FJ6==546L~39`00dAMJEVIGBF38SLH5@207134:D?3HKE1b00NQG46WWG468P0~22@5H=@5H=V]0eD;BEDD9W8~4_]0fQ645L~3=V]00a_9FF=><L8P0R?;g4GI34;?R0OIGBJ;~25FJ;~3HK31^0NIGH=@6H6@C0~2OIL~3FFL~3IO0_@6"
        "46==545DQb0~3RD5gg5DR0~3eI>>WM43>>AC0~77134:DCCT5468S~3d00O2RAMJEU2b]0~2XU<KJL<MH3I20~3XU<KJL<MH3I20~3O1EFKH3?R0~3O2RAMJEU2@5H=V0~42@54GEEBB<C0~2OI;4;?6<9JF<[0OI;4;?C~2dO0~2N83:9FM9~2L4;?a7134:D2002@54GEEBB<C0~3b?G4G?;4GIS~3R0NQG46822b0~3bQLJMW:H3I20~4NRCCA:431^0~2OI;4;?SS<MJ=3H:W`0~27134:DVT;g;54;AC0~3]Z=HK>VT5468S~3R00b?;438A:;5;TIG46QO07134:D=BF5<Vf00NQG468QG468P0~22@5H=@5H=V]0`<BG?RR86~431P0fQ645TS~3Y00O8DEJMADD8UXP8<;ggM<34;<8PNQ<;43<<"":;5;<<G46<S^0NIGH=LF5>_X0~2X_S~29FF9<1S_X0V=;4;G~2JBGD200N2`<BKKB9_2N00RT;MMBgKM:AY0~77134:W~35468PX~20~4RAMJEU0~6bQLJM64:"">[0~5bQLJM64:"">[0~5e?G4M>`0~6RAMJEU2@5H=V0~4f1=KB~2HB<C0~2NQ64M96<9JF<[0OI;4;W~3A1R0~2_W5F4HF~45T27134:D200f1=KB~2HB<C0~3CWMFE?;4GW~3De0NQG468P0~4b8?A5H;H:"">?I200XC8D>WW:431^0~2OI;4;IOa8LJMKF=1O0~27134:DC?;4H4B9V70~3]Z=HK>VT546"
        "W~3DR00P134;T<BL6M>A:H=ZX07134:EKFLGG>[00NQG468QG468P0~22@5H=@5H=V]0UEJ31^c86J5~231P0fQ645W~3>C00a?;:4B::31^0c86FgBHBBgF68cOIG;45GGB;5KGG:4;31^0[IGH=KM988d0~4XX`9FF3;AR0~2V=646=~2FB=D20XVDDLHGGHLDDVX0CW9~2546998N0~77134JB~3F468P0~7RAMJEUP^0~5[>54HF=1N0~6[>54HF=1N0~4NSEBBH3?R70~4RAMJEU2@5H=VP70~3YIW<~2BB<CPX0XZEF4B~34F<[0OI;4FB~3:E1f00OVAEJM9~5T27134:D20~2YIW<~2BB<C0~27S=J:A?;gB~3M>d0NQG468PP0~3RA;;54g45;;>d00CDL5KBBJ431^0~2a?;4GIO0CA544;DY0~37134:WWEKggH3?20~5`<MJ6==5gB~3MAR0~2VEJM>EJE=F9<BF9_007134MM4BEQ~2d00NQG468QG468PP00b?;JE?;JEU00UEJ31Pc86B<T~2V^0fQ64FB~3<C00a?==H:=~21^0P8<;43:334;<8POI<EJM<EJE=F9<BF9<1^XZL;4J4;669`0~4^C19FFEDZO0~2UW646W<64:WT20^13:M4::4M:31^0a?==3JM9U_d0~87134B:~3B468P0~7RAMJE88Zf0~4fSEF4;Dd0~7fSEF4;Dd0~4]`>:FEMBE8Sa0~3RAMJE88@5H=88SO0~32C[[<BB<88UX0YT35:M::HF<[0OI"
        ";4K:~2F45Ae0~2PCWKFEQ1I>LZX7134:D20~32U88<BB<C0~2RT;H3?""?;gM:~3>d0NQG46881R0~2RA:MKg~2KMM>d0P1LJHM::B431^0~22T5468f0YD;44GTe0~37134JBBF4MFH3I20~5a8E5H~3gM:~3AR0~2[<BF93KW9F=EJM>Y007134gH:45>[N0~2NQG468QG46881R0OIGF9IGF9`00[<K;D`[86BE~3V00fQ64K:~3<C00aI<=H:<~2S^07ZT;436534;TZ70d[<BF93KW9F=EJM>[R0XZEG4J43EE9`0~4RDL5JF9GTR0~2a864B~2HFL8d]0^1==BK==KB==1^0a?;H4;<120~:7134:D~35468P0~7RAMJ6~2Tb0~5d>KH=Z^0~7YU>KH=Z`c0~3R?6H6T=F56AR0~3RAMJ6~3:H6~2DR0~6C<BB6~2Qf0]CQDD6<9JF<[0OI;4;DDALHB<C0~3bI3JM63GMLZX7134:D20~3NQ6~2FB<C0~2_9BH;~2M4GD~3?R0NQG4;66WC0~22?>>;4:H5>>@e0b?;45WDA:431^0~2RA:4L1^O1=JKBFESN0~27134K55;3<6HK9V^0~5RI3HM=546D~3?20~2RA:H=;5D>M634;?20071344;<KH3I20~2NQG468QG4;66<C0P13M<13M<C002?6J3<<LMHB~2=V00OQ645D~3TY00a?;B4JBB31^0c86M455F54M68c00RA:H=;5D>M634;?20~2eIGH=KB=8Ub0~4aI=9FJG9?20~2P"
        "864:55;E?C0~2^S<=J;<<;H=<1^0a?=L5H5>[0~:7134:DRRT5468P0~7RA5~245Tb0~5d>KH=Z]0~671EEKH=EAe0~2PSE55<_D;HMAR0~3RA5~8H:DR0~6C<55:46Qf0~2ORQE<9JF<[0OI;4;?RUEJF<[0~4[DL:FBM;=ZX7134:D20~3NQG4:55<C00O83HB~3F4GI[~320NQG55JBWC0~3R8=JM<"":JLIRX02@5438RA:431^0O[Z9BJEV]_WMJL3H5>[0~27134:DT?8UW:4;DY0~5_<MHLT5468[~3b0~2OIG46M6I?;5;438c007134:WQLJK9V700NQG568QG55JB<C0OSAATSAAT[00]_>6KBB:;B<~3V0O1T645T?RR2X00aI<=H:<~2Sb0c8=;43=Kgg;=8c00aIG46M6I?;5;438c0~2NIGH=LFM9Zc0~42I<6JF9U20~3P8646TT?Sd0~3^13F4K~3HF31^0^_ZS<KJL1N0~97134:D22T5468P0~72I@@=H5Tb0~5d>KH=Z]0~6713MF4MMAe0~2PS@~21aUEJMAR0~32I@~7EJ:DR0~6YT@@646Qf0~4CU_9JF<[0OI;4;?O_9JF<[0~4XC8A:6A?Zd07134:D20~3NQG46@@?Y00YA:J=<~2;4G<~4Z]PS@@<BBWC0~2]_WMJLQ=FK<UX0a?;45WAA:431^0[WE54KWCR?6H5A>:H3I2007134:D2bNO1EBF=1O0~32I6H:"">T546<~4V0~2^1LHBJ=S83JFJEV]007134:D"
        "CA54;AC00PS@~2ZS@@<BB<C02@GG=@GG=V]00P_?<99A6B<1~2U^b?6;4:6>Y0~4RTA=H:A~31f0[?;43?;4g;?[0~2^1LHBJ=S83JFJEV]0~2NIGH=@6H5>_X0~32?;9FF9`0~4P86468ba^0~4^S>MB<>><BK>S^0~2]2?64:AC0~97134:D22T5468P0~8b2Z=K5Tb0~5d>KH=Z]0~6^ZA>KH=A?R0~3O2~2a0UEKMAR0~4b2~6UEK:DR0~6]2286K6Qf0~6_9JF<[0OI;4;?O_9JF<[0~6RD:68a^007134:D20~3NQGK6822]0XVEFM>CC?;gFB~3=Z]0O2C<KKWC0~2R?6H5ACD;4;@d07SEBHK::B431^0`9FHKLIbZ9BBES1=FK9V^07134:D20~2e?6H:"">`0~2]U<KJL1T5gFB~3=V0~3U9FgF9_VEJgB<[0~27134:D2Z9BJL8a00O2~2NO2C<KK<C02@5H=@5H=V]0~2]2C_`86F3~3IOcVQ645T120~4d>MM4BM~2GIO0O?;431=Hg;?a0~3U9FgF9_VEJgB<[0~3NIGH=V>5H6@C0~3^VT9FF9`0~4P86468c0~6]VEJ5DYY@5JEVX0~3]U9BJ=SP0~871355D22T5568P0~9]VW~2?b0~5d>55=Z]0~7Od>55=Z20~:_W~2TR0~<UW~2T20~8P1W~28c0~6_955<[0OI;5;?O_955<[0~6RD==8c0~271355D20~3f8W~21P0~2b?G56Iaa?;5~5=Z]0~2C>WWAC00^ZE55EZPV"
        "955=1P0RI965~431^0`9;L<IYCA556?Rd@;5;Dd071355D20~2]`>553?2002?655ACT5~6=V0~3C>5~2>Y[<5~2Ae0~271355D22?355<U0~8CAWWAC02@55=@55=V]0~6c865~3;IO0fQ655Tb0~5d>5~6GIO0O?;531<55;?a0~3C>5~2>Y[<5~2Ae0~3NIG5=VZ955LIb0~3N`9559`0~4P86568c0~6OI6531cP1L56Ia0~4RT;5;De0~8XV@~2IbaQ@~2Z^0~:aC~2e0~6R?@~2U0~9R?@~2U0~;OC~2dX0~<OC~2d]0~92C~220~7[T@@TC0f1@~21N[T@@TC0~6fUZZ[0~3XV@~2Ib0~42C~220~3aQ@~2U]N1@~7U0~3PYCCY700^Z@~2?R02Q@~2SP00RVIT@~4VX0CT?1_20CT@~2_]^U@~2?d0XV@~2Ib0~3NS@~2I2002I@~21O8@~7_0~32I@~2I2R?@~28a0~2XV@~2Ib0`@~3U0~8PYCCYP0aQ@~2Q@~2_0~77S@~51c0PS@~28O0~5R?@~71c0N1@~2Z?@~21N0~32I@~2I2R?@~28a0~3PS@~2_b1@~2Qb0~4CT@@TC0~47Z@~2S70~6O8@~2_00`@~28O0~4^Z@~2?R0~9N2~2b00a2~2N0~H2~3P0~:2~3P0~l^2~3X00O2~2O0^2~3X0~8]]0~5N2~2b0~?a2~2c00O2~7P0~<N2~2b0~2a2~2O0~3XO2~5f00X2aP0~2X2~3P00P2~30~2N2~2b0~5O2~2b0~3b2~2O0a"
        "2~770~4b2~2b002~3a0~4N2~2b00^2~3P0~@a2~2a2~2P0~8O2~5O0~2O2~2a0~7b2~7O0~2O2~2Nb2~2O0~5b2~2b002~3a0~5O2~270a2~2a0~5X2~3X0~5O2~2O0~8a2~270072~2a0~6N2~30~<^7~20~97~2X0~97~2^0~87~4^0~6Pc~6X0~37~2X007~2^0~2XOO70~47~2X0~Z]77]0~6^PPX0~8^7~9^0~2^7~9^0~2^7~9^0~}0~B]0~3]0~=^Ob2bf0~8N22O^0~5]O22N0~8]O22O]0~7O22ac0~3^c~2]0c~2P0~:]0~5cOOP0~77OOP0~6^^0~3^70~5^Ob2a70Pb2aX0~E]O22N0]O22N0~6]O22OX0~62S1~2C0~7`1~2ZO0~7`1~2ZO0~6`1~5VY70~2218~6_]00C1~2ZOe1~3202VI?Sd0~2`1~2ZO0~9dV~2d0~9b``a0~:PYV11VY70~3aS11UbX0~5bS1~9ZO02S1~9Za02S1~9Za0~4aRRa0~9aRRa0~8]O22ORVe0~6N2bfaCO0~4^Ob2bfdVR0~42C``c0~>aCZd00eZ[a]0~9CSI?T?8_O0~42UQTTIZd0~32UITTQ_O0~52VITTQUb0~4aUQTT?8V20PV8~2`C8~2SO0~2RVY0~2RVY0~22U8?""?1[P0~3XC1I?1C70~3aZZO00NVSb0~3CSITT?SY1?TIVR0~6^[``[P0~62VITT8`UITT8`O0~32VITTIVR0~5Y>33=VP0~5713~2D20~6^13~2DR0~5713~4"
        "LEAVN00Y>6~53170]Z=33AeU933<[a19G;=IR0713~2D20~8^S=~2170~8U9<Vd^0~7aZAE33EAVO0~2RA33A?Ub]XX00CW3~9Ae0CW3~9>d0CW3~9>d0~3P1DD1N0~7P1DD1N0~62VITTQ8EIR0~3a_QT?1VW8[X00CSI?T?8IE820~228<9>U70~:7RUI>=V]0UEWIURP0~671=6;5;3WZc0~228<655G=Z]00RQ96553WZO0~3RQ96556<120~2PZ<655;39`0a?6631UE66DR0021EIC`C8EIe00U93;;L>_0~3[D=;;LA_]00OZ<<VNPUW9SO0071=6;5;=T=;5G9IR0~5RD99Ae0~5RQ96553><65;3WZO0~2_9655GE?Y0~4Y>KH=ISP0~3^_134:DVC0~5^134:DR0~5N1344HH4F;>`00Y>KB~2H431700`<BB<[SLHJEV`WM44B9U07134:D20~82?;H;T20~8C>;>AY0~6OS<ET88DE<Zf00aS=MMG<IVZVf0CW=>~73Ae0CW=>~73>d0CW=>~73>d0~3NIG;?O0~7NIG;?O0~5RQ9655695>C0~2a1<35;L>;L?b071=6;5;3E;>Y00a19EE6A`X0~7OCS@96M=V]0UEM69D1Ca0~4713:;;MH5AY00PSEM4K:K=Z]0PSEKF55J5W`0~2cSEKJMMHM9Z^00eA5J5;5B9`0a?;431UEJ:DR00U9M=>9W9M=S^0UELEEM6Ia00aI3B==K6?200C>5;>U_>;5WC00713:;;MF6F:;BKE1f"
        "0~4RA:MAe0~4cSEKJMMH;:F;5J5W`0~2_9B:MHB=Qa0~3Y>KH=3Dd0~3b?=34:==1^0~4^134:DR0~4N1?34:==3FH38N0YAW~2EBKE1700dAMJEVIGBF38SLHJMH=Z]7134:D20~8[<KFK<`0~5]P7OI63WVR0~5`W9SU2fe8E>[00]Z=Hg4K6==T20CW=ZY~513Ae0CW=ZY~3SW93>d0CW=<9SYY1E>3>d0~3NIG;?O0~4aC~2IG;?C~2b00cSEKJMMHB9ZN00]U<"":B5MH;F=SP0713:;;MHM<VP00eA=;=>6DC]0~3^RUI>=;BM69V]0U96MF5=>IUR70071EA?""?WMFEU00RD54;<DW=Z]0eA5JLTDGH38N00eA546A>;4;T200CWB5A?D<9`0a?;4L1UEJ:DR00Y?3K:BK:GA[00[T9==;:DR00dAM;TIGKWC00b19K;AAGKE820071EA?""?WM4;>?=J5Ae00^bbRA::Aebb700eA546A>;4JLTDGH38N00_9<D>6HMWC0~2NC>K45EDY0~3b?5Hg4J31^0~4^134:DYa0~3a?;54K;3T;g;?b07CCS9MF=IR0~2b?;438A:;5;TIG4GWM=Z]7134:DC~320~37S=FEFL1P0~4`8SaUEK=EU0~4aI39=9IR0RI38N0]Z=Hg~24H5T20CW=Z]0~3^13Ae0CW=Z]0XUW=83>d0CW=>5<ZS9;>3>d0^b~2IG;?b~2700U<~3G;<~3V]0eA546A>54;T200OI6J=T<54;DY0071EA?""?WMJEU"
        "0~2Y>9EE6>3TC0~3`T93MB;=>IUR002UI>=;BM69D_00]`T9=LL5H=ZX0[<BF9VR[Q`00[<BB9~2=H5D200[<BF9_VEJK>C00C>KK3E<DSR0a?;4LSUEJ:DR0~2C>:3<=B<_^0f8LKL=;:DR00Y>K;I83B<[0~228EB66B=IR0~2]`T9=LL5HL9~2KB<`00`T~3DDT~3_00[<BF9_VEJK9~2=H5@200YQUR`WKJEV00N1>LBH=9?2OO^0b?E34:EE1^0~3X_D34:"">>V]0~2a?;K4FK3?;45T200PVW5J3?d0~3P134;T<BL6M>A:H=A<<V]7134:W~41P0~22T;M>:;@R0~3^SL<UC>M5>``CO00dAEBFKES_S_9DR0]Z=Hg~45T20CW=Z]0~3^13Ae0CW=Z]0dTG@13>d0CW=Z<5W<5W13>d0`T~3G;T~3_00UEB~2FFB~2=V]0[<BF9D6=JK>C00a?;H3E6FBK9_00]`T9=LGHH=ZX00CWL9EA6WL?d0~2UEB:39TSCa]0~4aCST<3:B=V]0RT6BBM:BHLZX0_9FB<C00N]00_9FHF~4MAR00_9FB<C`9FB<C002?3KJJB59V]a?;4LSUEJ:DR0~2_9M>[T;=ZX0a?;;?A5:DR00RD55A@;M>Y0~3RI=BFL?d0~3RT6BBM:B4F~59_00UE5~7=V]0_9FB<C`9FHF~4:AR0~2NX0RA:H=Z]0aI6=KH;G<I~2U]^`134:DVY0~4RTGM4:L=Z]0~2aI>34:WWEK46IO0^_>;H6DC22a0~2VEJM>EJE"
        "=F9<BF9_CC207134JB~368P0~2`<K6I6B<`0~4_9GW<<;H399>Z^0C<G595;?ALT>>Y0]Z=Hg~45T20CW=Z]0~3^13Ae0CW=Z]PSE9V13>d0CW=ZS9;5<Z13>d0UE5~2:M5~2=V]0UE=~2G;=~3V]0_9FB<L39FB<C007SEKJJM3LH31P0RT6BHF:BHLZX00OZ<=E9>3<=1700UEBM69D1[2X0~3Xb[1@93:B=V]0[<BBEAA5HLZX0`<FF9_NeVY00`9FK9~5AR00`9FB<`U9JKWC00N_?<=3546IOa?;431S=H:DR0~2_9M>[T;=ZX0NQ3B6;3:DR00f1EK;;K=Qa0~3eILFFL?Y0~3[<BBEAA5H=9~5_00UE;~7=V]0`9FB<`U9JK9~5DR00RZCOC>MJ=V]0^UA<KH=?A;;31^b?E34:EE1^0~3CW534B5W[0~42134JBBF4M9U]0CD6H;AD~38O00[<BF93KW9F=EJM>Y0~37134B:~368P00P1LFEV9FL1c0~3U<5=LL54;33E1P0[<M;A;5@W;A>WC0]Z=FBJ4gg5T20CW=Z]0~3^13Ae0CW=Z_[A6?213>d0CW=Z_>;;A`13>d0UE;~2::;~2=V]0eZ~2IG;?Z~2Y00`9FB=GD9JKWC0~2RQ<EE>Q9FGIN0[<BgJ6A5HLZX0~2aS9E=9WL;?b00[T<3:B5LW?VR00RV?W=;BM39T_00_9FK>1<KHLZX0YA:H6DQ@EZ]0Y>:J=I1I>=1700Y>MHL?T345DR00d>>I11=H;?aOIG4GDA;4:DR00"
        "]C>:3<LK<_700[DE=<99DR00][Q<==<I`^0~2RQ=B63B=IR0~2_9FK>1<K4GD18D9D200[?~3AAT?~2`00Y>MHL?T34J=I1I>=1700_9AITLJB<`00a?3;BH=VWKH=ZXb?5JgHJ31^0~3YTD34:DZO0~47134K55;3<1b00`9F4:~45Tb00RA:H=;5D>M634;?20~37134:D~4S700R@;:AYA:5DR0~2XZ=ESZA:M<11Ze00Y>MJMJK<S?1EAe0]Z=9<=5FJ5T20CW=Z]0~3^13Ae0CW=?9WE9U^13>d0CW=ZAGE=6D13>d0[?~3G;?~3`00]O~2IG;?O~2X00Y>:4;>T345DR00C1DAT1VILH31P0_9F4;A<KHLZX0~321E9L<=E>Y00X2_8>E;BKGEV]0UEGMB;=>Q_R^00CWMH;35:HLZX0O8=FJ5G5=Z]0O8LFK636:31700O8LFBG;FB=1c00d>MG33:H38N71=JF556H:DR00YT3K:FM:GA`0fQE~5Tb00OIE~5Tb0021EKGAD6BE8200CWMH;35GMJ;335:D200XaaRA:MAeaa^00O8LFBG;FMKK636:31700_95G;FJ6?200^ZE>KH=?3HB<`0b?=34:==1[[R00]2134:D[~3e007134:DT?8Ub0~2`95~545Tb00OIG46M6I?;5;438c0~37134:DR~3O0~2_<B3Q283B9_0~2XZA?R`<B;EU^X0~22?;4gg4;9>E6Ia0]Z=A[SA9E9?20CW=Z]0~3^13Ae0CW=WE56Ad^13>d0CW=D6="
        "QI=3?3>d0Xa~2IG;?a~2^00[?~399?~3`00O8LJB;;FB=1c0P1L5M5LE3BM<_00CWMH;35:HLZX0~4RQ=9;=W9_0~4OCZT<3:=V]0UEM39TSCO0~4aQ=MK5955LZX00Y?=5KK:=Z]00CT=5KKM;=170~2CTL:KK5=?d0~2d>G:KBM6>`00CAGKK3E55DR00U9M=><>9MES^fQ6M~35Tb00OI;M~35Tb00C>;;>_`A;5WC00aQ=:BM3A9;KBM53D20~4RA:MAe0~5CTL:KK;9E5KKM;=1700_95KK:LA`]0~2RU>KH=3FH6?R0^_134:<~48f00^134:<~48N07134:D2bN0~4CT@~4=H5Tb00^1LHBJ=S83JFJEV]0~37134:D20~5c1LF9D~29F38f0~2OR2P1LGAIC0~4XV9Kg~24BMM<U]0]Z=AePd_U_d00CW=ZO~513Ae0CW=Z1=GIaa13>d0CW=E=IeY?L93>d0~3NIG;?O0~4UE;~7=V]0`>;=5KK5=?d0~2`<6L;KBK;91a00_W5MK5955LZX0~5eIE6L9L170~5^2_8>EV]0UE>Q_R70~7d8>WA8@~2U0~3Y1D>WAI[0~2]Y1DWWA?SY0~3]C8AWWD1d0~3aS?AWW>I_P00P_TW>I@~2I20021EIY[C1EQR0P1A~5Ia00N1A~5Ia00NV<WVcPU><ZO0~2e1AW>I[VT>W>@8`P0~4RD<<Ae0~5]C8AW>TZ1DWWA?SY0~2d8AWWA8C^0~4Y>K4BHJ;>_]007134JB~36Qf00^134JB~3GQN0"
        "7134:D20~6X2~4Z=K5Tb0~2U9FgF9_VEJgB<[0~47134:D20~5RD5F:~4F5Ae0~5eD5EWC0~62I=K4gg4KE8b00]Z=Ae0~7CW=?~73Ae0CW=?""?A@?~33>d0CW=?~5T?3>d0~3NIG;?O0~4UE5~7=V]0`A=QAWWD1d0~32I?ST>W>TZ20~2_>E>WA8@~2U0~7dVQA93?a0~8OCVe00RVCO0~;OYCea2~2P0~4fRCCeO0~5NRCCdb70~6NRCCRf0~5^aeCCda0~4X2CYb2~2b0~3RVY0~2RUe0~2aR~520~3bR~520~3OVVO00fUZa0~4OdCYa0^2YCY2f0~6^C[[C70~7NRCC27fRCCdb^0~4NRCCeO0~6Y>5~26EAUf0~27135~56Qf00^135~5GQN071355D20~;]VW~2?b0~2C>5~2>Y[<5~2Ae0~471355D20~5_95~89U0~5U9=1Ua0~7dI96556<8R0~2]Z=Ae0~7CW;~9Ae0CW;~9>d0CW;~9>d0~3PS@@1N0~4`@~9_007`1dRCCRf0~52272YCY270~3cU1YCea2~2P0~8]Oe_1Zf0~}0~tXX0~3X^0~we?@@TIZY^0~3XV@~7SP00XV@~7SP0XV@~2Ib0~<aC~2e0~32I@~2I2R?@~28a0~4XV@~2Ib0~5`@~:_0~5d11e0~:2UITTQUb0~4_TI20~7e?@~9I20e?@~9I20e?@~9I20~4O22O0~572~9P0~2]P0~IXP0~DP70~}0~}0~x2~3OX0~6N2~7O0~3f2~7O0~2N2~2b0~Gb2~2"
        "b002~3a0~6N2~2b0~672~:P0~6^^0~<]O22O0~67ba0~92~:b0~22~:b0~22~:b0~}0~}0~}0~}0~3N22O^0~3]O22OX0~77a2O7ccP0~5O22aP0~3^c~2]0~6]O22O]0~5c~:0~6^Pc~50~2Xc~8^00^c~2]c~20~67ccP00]c~20~3Xc~;P0~4Pc^0~2Xf70~6N22O^0~6Pb2bf]0~2cb2bc0~7^Ob2bf0~7]O22N0~7ca22OX0~6]O22O]0~8N22O^0~4cb2a70~4Xc~2X0]O22O]0~8N22O^0~5O22ac0~57ccP00]c~20~3Xc~2X0Pa2aP0~5]O22O]0~:a270~:Nba0~>XO22f0~6^Ob2a70Pb2aX0~7fb2bc0~8]O22O]0~97c~20~8ca2bN0~:c~2P0~62UQTTIZd002VITTIVR0~4]YS?TIS88120~2OUQTT?1[00PV8~2`0~52VITTQUb0~3Y8~:[0~32_18~6C0]_8~8U^PV8~2`8~2C0~4OS88120`8~2C00]_8~;1b007`88S[c2VQSR0~32UQTTIZd0~3c[1?T?8U7^_8?T?1`O0~4CSI?T?8_O0~42UITTQ_O0~3PU8?TTIV20~42VITTQUb0~52UQTTIZd00P_8?T?1C700^U8~2U2UITTIV20~52UQTTIZd0~2aUQTT?8V20~2OS88120`8~2C00^U8~2UC1?T?1`^0~22VITTQUb0~57e`UITVUCa0~5O_8?Qbdd2X0~8RZITT8`f0~3CSITT?SY1?TIVR0~3]dU8?T?1`N0~5"
        "2VITTQUb0~7NZ8~2Y0~5^_8?T?8`f0~7Y8~2120~428<655G=Z]0_9655GE?Y0~3CT=;5G<66>Y00^V<655;38P0a?66LS^0~3RQ96556<120~2_96~8=V]00RQ<L6~5EU07136~7INa?66LS<63?R0~3[>66Ad^S=6LQa07136~:"">d00RA36L>U896E1P0028<655G=Z]00fV>L;5;3?aOI3;5;3WZO0~271=6;5;3WZc0~2RQ96553WZO0~2bT3;55G91O0~2RQ96556<120~328<655G=Z]0RD3;5;=A_^0NQ6~288965569QR0~328<655G=Z]0PZ<655;39`0~2[>66Ad^S=6LQa0NQ6~28AL;5;LTb00RQ96556<120~3OVD9EL;EE>8d0~3aSW3;DIA>?Uf0~3dCCdIEG5;3>VP0071=6;5;=T=;5G9IR0~22?93;5;L>ZO0~3RQ96556<120~62T669_0~5O?3;5;3>_]0~6_966>d0~3PSEM4K:K=Z]0_9B:MHB=Qa00OQLFB;B5FGIa002?;4MG;68P0a?;4LS^0~2cSEKJMMHM9Z^00_9BJ4B~24JB=V]0cSEKJMMH4FBEU0713B~24FBBGINa?;4LSI6BEV^00N8LH31f]C>MM>[0713F4B~5F4M>d00RA:MH;T<M4;?a0PSEM4K:K=Z]00[>;HM;5;?aOI;5;:H5W_0~2713:;;MH5AY00PSEKF55J5W`0~2bT55;:HM>Y00cSEKJMMHM9Z^00PSEM4K:K=Z]0RD:M:FJGAC0NQG"
        "4689MHMMJKESP00PSEMHK:K=Z]0eA5J5;5B9`00N8LH31f]C>MM>[0NQG46>;JF:M5Tb0cSEKJ:MHM9Z^00XUW;FMMF:BM=I200]U<"":H:D3MMG>_0~2a?<<1=BB;5J;>C00713:;;MF6F:;BKE1f002D:K5;MH5>`0~2cSEKJ:MHM9Z^0~52T5J9_2~3]0O?;;GKH6IO2~30~2_9JK>d0~3RD54;<AW=Z]0_9<A>6HMWC00dA:HL?LHJ=ZX00RD:B<I?>1P0a?;4LS^0~2eA546A>;4;T200`WW:JEW9FKWWV00eA546A>;4;WWU071W~234GW~28Na?;4LSV=JG?a00eA:JEUZ~2?;J=S^^1W:JEW~4;H=Ae00RA9>MM>34H38N0RD54;<DW=Z]0f83H5W?TW?aOI<@?>;H6IO0071EA?""?WMFEU00eA5JLTDGH38N00bTWT?>:B<C00eA546A>;4;T200RD54;<DW=Z]0RDEAAEBJ=S^NQG46@;4G>A645DR00RD54;WDW=Z]0CWB5A?D<9`00eA:JEUZ~2?;J=S^NQG46LHKEAAETb0eA546A>;4;T2002TG46><EAEBK9U00a?645W=J:5H6Ia00a?;6>:F=?A;JL1P071EA?""?WM4;>?=J5Ae002D3WT?>;H38N00eA546A>;4;T20~2]2[UT5J9_?DDTY0OI>?IEJ;?C@DDI200_9BK>d0~3[<BF9=M5@`00YQ=M5WKJEV00C<BB<`9FKW[0~2OI3B6EE1R00a?;4LS^0~2[<BF9_VEJK"
        ">C00OCD:JEV9FK>Ca00[<BF9_VEJK>CO002CC134GICCR0a?;4LS_9J:DR00[<BB<D=~2Q64GIO0[<BB<D=~2Q64GIO00b81T5M=:4;>`00[<BF9VR[Q`002T5H=W~3?aOIW~3=J:DR00]`T9=LL5H=ZX0[<BB9~2=H5D200X[TEELBGDe00[<BF9_VEJK>C00[<BF9VR[Q`00a1D>~2;4GIONQG46<KJEVU9FB<[00[<BF9>~2Q`00C>KK3E<DSR00[<BB<D=~2Q64GIONQG46;469~21SN0[<BF9<<EJK>C00[<BF9UUVYT;431P0dAMH=I;J=<B:AR00N83:9FM9~2L4;?a0]`T9=LL5HL9~2KB<`00`>~5=H5D200[<BF9<<EJK>C00][?<EE5J9_>::WC00eIEEGK=1`9::DR00_W<<Ad0~3_9FB<=H5@200]V=H5A:H=Z]0[<FKW[<K;T20~2cZ<3JF31P00a?;4LS^0~2_9FB<C`9FB<C0~22D:JEV9FK>Y0~2_9FB<C`9FB<C0~4P134GIN0~2a?;4LS_9J:AR00_9FBWD:H=864;?a0_9FBWD:H=864;?a0~2NV>MJF45WVc00_9FB<C00N]00RD:4B~3;?aOI;B~34MAe00RT6BBM:BHLZX0_9FHF~4MAR0~22T5BJG9Q200_9FB<C`9FB<C00_9FB<C00N]0~2eAMKKBg;?aNQG4BBHF9`[<BF9`00_9FHK~231^002?3KJJB59V]0_9FBWD:H=864;?aNQG4JJgJFF=VP00_9FHK~34"
        "B<C00_9FB<C0]OIG4GIN0C>KJE?;JEWK:AR0~2_W5F4HF~45T20RT6BBM:B4F~59_00_9K~54MAR00_9FHK~34B<C00RT6KFBJJ9_>MBWC00OI;FJ3<V`9F:DR00OC~3^0~3`<FF9=55@Y00RZ=55>MJ=V]0C<BB<Z=J5T20~2eA5BL<<1eX0a?;4LS^0~2`9FB<`U9JKWC0~22D:JEV9FK>Y0~2`9FB<`U9JKWC0~4P134GIN0~2a?;4LSVEJ5T200`9FB<AMHLQ64;?a0`9FB<AMHLQ64;?a00]_>;HK3J31VC00`<FF9_NeVY002@5H=E~3?aOIE~4J:AR00[<BBEAA5HLZX0`9FK9~5AR00O`?<<EMK<`00`9FB<`U9JKWC00`<FF9_NeVY00cUA9~2G4;?aNQG46EBF9_`9FB<[00`<FF9~31Y00N_?<=3546IO0`9FB<AMHLQ64;?aNQG46543=~2V_70`9FB<>>EJKWC00[<BB<`YCb?;468c0Y>MJ=83HLEJ6I20~2OVAEJM9~5T20[<BBEAA5H=9~5_00`9FKL~3H:DR00`9FB<>>EJKWC00[<BM9<5JEU>KBWC00OI<93J;D_9F:DR00[D~2?R0~3YA:H6DT@EZ]0_9ATTLJB<`00d>MJE?G4M>8R00CWKMAS1T@R0a?;431C^00Y>MHL?T345DR0~22D:JEV9FK>`200Y>MHL?T345DR0~4P134GI[c00O?;431T646QO00CWKJ=<B:GA5438f0CWKJ=9B:GA5438f002?6HJEEJ6898"
        "P0YA:H6DQ@EZ]0OI64GD18@IaOQD81T64G?a00_9FK>1<KHLZX0Y>:J=I1I>=1700[<A81T;JEU00Y>MHL?T345DR00YA:H6DQ@EZ]0RDWII9KHL1PNQG46A5H3T?3H:Ad00YA:H6DQTEZ]0d>>I11=H;?a0CWKJ=<B:GA5438fNQG4634:<I?<Tb0Y>MHL?T345DR00eA5HL?A<Q<MFEV]02?;4;>>;:5;W?I20~2PCWKFEQ1I>LZX0_9FK>1<K4GD18D9D200Y>MF91?646IO00Y>MHL?T345DR00`9F5D>M43TEFM>Y0~2e[`>KK<T3H5T200_9::"">d0~3O8=FJ5G5=Z]0_95G;FJ6?200aI64M6BKJG<[00dA:J;LL;Ae0O?;4G9@200O8LFBG;FB=1c0~22D:JEV9FFE9V00O8LFBG;FB=1c0~4713459>Y00c83H:L6FK9U]002T;4B:MLF:HFEV]02T;4K:MLF:HFEV]00C>Kg:""><KM368P0O8=FJ5G5=Z]00U<MJ;33;?aOI;33;FK9VX00CWMH;35:HLZX0O8LFK636:31700[<"":63GFK<`00O8LFBG;FB=1c00O8=FJ5G5=Z]0RD:GGM45>[0NQG468=FF;;BF=8O00O8=FJ5G;=Z]0d>MG33:H38N02T;4B:MLF:HFEV]NQG46<"":4MG;5Tb0O8LFBG;FB=1c00PS9MBG;KG:F6Te00^V9KH5G5HH5G;DR0~3bI3JM63GMLZX0CWMH;35GMJ;335:D200O8LF:36FM9U]00O8"
        "LFBG;FB=1c00CWKJG;6FF5KH6Ib0~42T;HM5BF=1P00_9JK>d0~4Y?=5KK:=Z]0_95KK:LA`]00]UW;KKME65<[00N19;KBK5Ae0f8LKF:D20~2CTL:KK5=?d0~32D55EV>5FB=V0~2CTL:KK5=?d0~5XZEKFK>Y0~2`>G:KM;EI20~2^V<5KMLW;KM3Te00^V<5KMLW;KM3Te0~2eA5:E1I=5M68P00Y?=5KK:=Z]00b19;MBK;?aOI;MBK;9820~2aQ=MK5955LZX00CT=5KKM;=1700[<5KBK5=I20~2CTL:KK5=?d0~3Y?=5KK:=Z]0RD;KBMG<SO0NQG568?=5KK5=TY0~3Y?=5KK:=Z]0d>G:KBM6>`00^V<5KMLW;KM3Te0NQG5689;MBM;Tb00CTL:KK5=?d0~328<L6GM63E@`]0~2289;KK:36MB:DR0~4[DL:KKMGEZX0aQ=:BM3A9;KBM53D20~2CTL:KK;9820~3CTL:KK5=?d0~2bILMK;>L:BM6A`0~5XU>6MBM3TY0~2_955>d0~5Y1D>WAI[00d8AWWA8C^0~3OUT>>A8I@TY0~22ZT>W>@1b00C?<<W?20~2]C8AWWD1d0~4bI@~2`SA<<>U0~2]C8AWWD1d0~7eIW<<@d0~2fU?A>>TSR0~4aZDW>IVDW>IC]0~2aZDW>IVDW>IC]0~2cVDA1Rd1DATUX0~2Y1D>WAI[0~32V?>W>@1NPST>W>TZ20~4d8>WA8@~2U0~2]Y1DWWA?SY0~2e8D>W>D1d0~3]C8AWWD1d0~5Y1"
        "D>WAI[00OST>W>?Ua00PS@~2ZY1AWWA1Y]0~4Y1D>WAI[00aS?AWW>I_P0~2aZDW>IVDW>IC]0PS@~2ZV?>W>?Zf00]C8AWWD1d0~5a`S8A>81VdX0~42Z@WWAQI>W>1a0~4XC8AWWA?Zd0~2e1AW>I[VT>W>@8`P0~2]C8AW>TZ20~4]C8AWWD1d0~4YI>WDV8AW>?_P0~6f_?>W>IC]0~2[T@@?R0~6fRCCeO0~3NRCCeO0~6^2YYdaa22]0~372YCY2c0~32C[CR0~5NRCCRf0~6b2~27NY[[YN0~4NRCCRf0~92C[CRX0~3]aedd2P0~6PRCYa7RCYb0~5PRCYa7RCYb0~5PRRO00f2e2^0~4fRCCeO0~5^2YCY2N00f2YCY270~6OYCea2~2P0~4NRCCdb70~4ORYCYRf0~6NRCCRf0~7fRCCeO0~372YCYbX0~3O2~2N0NRCCRN0~7fRCCeO0~3^aeCCda0~5PRCYa7RCYb0~3O2~2NX2YCY2^0~4NRCCRf0~8^cdCaP]0~77RCCeOadCYO0~7OeCCea^0~4OdCYa0^2YCY2f0~6OdCY270~7NRCCRf0~6aYCR7OdCY2]0~8]bYCda0~4^2~30~}0~}0~}0~}0~}0~}0~jc~60~7Xc~2XXc~2X0~:^PPX0~37c~7P0~7O22aP0~6XPc~3O2bc0~ZfO0~A^c~27b2c07b2N0~3Xc~2^a2O0~4Xc~2^a2O0~2Xc~2X00c~2P0~3c~270c~2X]c~2^00]c~270Xc~270~3Pc~6X00Xc~2X00Xc~2"
        "X0c~30~2Xc~2XXc~270~2Xc~2X0c~20~5O2bP00O22f00Xc~3P^0~4Xc~5P0~3c~2P0Pc~20^c~2^0~2Xc~2XXc~2^00Xc~2X0c~30~5c~70~2Xc~2P00c~30~2Xc~2XXc~2X00Xc~7X007c~7P0~2]c~270Xc~270~3Pc~20PccP0~2Xc~2X]c~2^0c~2700Xc~50~7Xc~2X0~9C8~6[0~5^U8~2UU8~2UX0~7O`S88Z[O0NZ8~8R0~4OUQTT?1[0~3^CZ8~4IT?1[P0~4aC``dc~2P0~72V[P0~<]e1QY0~;aR20^U8~2Z?T1YS?T8C]0^U8~2V?TI_P00^U8~2V?T8b0^U8~2UXd8~21200C8~2ZY8~2_`8~2U^0`8~2SOU8~2Zf00218~6_]^U8~2UX^U8~2UY8~3[0^U8~2UU8~2SO0^U8~2UY8~2`P0~2dIT?1R[QTT1O^U8~4S[N00^U8~51200C8~21R18~2YZ8~2V70^U8~2UU8~2VP^U8~2UC8~3R0~3C8~7e0^U8~21bd8~3[0^U8~2UU8~2UX^U8~7UXNZ8~8R00`8~2SOU8~2Zf00a18~2C8~3R0^U8~2U_8~2VC8~2SOXU8~5C0~5^U8~2UX0~8UE6~4=Z]0~4NQ6~28Q6~28P0~621WL63=>V]bT6~7<C0~3^V<655;38P0~2`A=36~3G5;L>U70~2eQW99D8~2120~5d89AVa0~9N[V8==170~9CIAI2NI6~29;5=?E;5LTd0NQ6~29G56>`00NQ6~29;5AR0f8366IaVE66Ad0"
        "0UE66DVE66Q8366QN^SE66W_@663?a00Y>6~5317NQ6~28PNQ6~28IL669V0NQ6~28>663T20f8366I_<66A[0~2[<5;E1T355T2NQ6~4=>V^0NQ6~5>Y00V966E8A669VW66=QO0NQ6~28T6~2?aNQ6~28?366>C0~3UE6~59`0NQ6~2W[VE66=Z]NQ6~28Q6~28PNQ6~78PbT6~7<C0^SE66W_@663?a00RA66EU<66<C0NQ6~281366IV=66@2P86~4EV0~5NQ6~28P0~8UEJK5:H=Zb2~270NQG468QG468O2~2P007Z9:B55K=Z]bT5~2BH5~2<C2]002?;4MG;68P00OI3JK55H4K;5H;AC00^ZEMGM=E66Ad0~37[?=M;<1R0~7bS>=9K9Z^0~8RILG@RNIG46MKHBEKKHFEZXNQG46MMJH3IO0NQG46:F:AR00_9BM>CI643QO00CWKKW1345@T;J=ZX0e?6H6?=F5>`0~2Y>KB~2H4317NQG468PNQG46Q=BM9120NQG46>;J3@[]00_9BM>C1LH38N00[<5FM<=FK5T2NQG4:5:H;T20NQG4:5~2>Y00b19MKEAMJ9>;FL?Y00NQG468EF4;?aNQG46?LF5WVf0~3UEJJBBHF9`0NQG4HL1?G4H=Z]NQG468QG468PNQG4B~3468PbT5~2BH5~2<C00e?6H6?=F5>`0~2RAMJEU<BB<C0NQG468134;IV=H5@2P8655KJEV0~5NQG468P0~8UEJ:DGH=ZQD~2`0NQG468QG4681D~2_00"
        "RD;KEDDW=Z]O8TTAMJETTDDTd00RD:B<I?>1P002T5J=TEJFE?>5J=S^0OIGB<"":=643QO0~2OUA3Fg4:EICX0~4OS9EW5=QYCCO0~5fSE:WUXNIG4K9>54BE>;46QNNQG4BE>3H:DR0NQG4F3<9AR00RD5JEVA:F9U0~22T5JE?;:KW>MB<[00]_W:BE:BESN0~2YAW~2EBKE17NQG468PNQG46EKM91200NQG46;J3D[X0~2RD5JEV_9F5DR00C@T3H;MBE@QaNQG46T<KM>d0NQG46T~2?R0~221EKM9MJ9;J3TC0~2NQG46D;4g;?aNQG46=B5<VO0~4UEJ:WWKF9`0NQG4g5A<KgH=Z]NQG468QG468PNQG46WWG468PO8TTAMJETT?Y00]_W:BE:BESN0~2RAMJEU<BB<C0NQG468134;IV=H5@2^ZTTD:JEVU[20~2NQG468U`RX0~4]V=H:DGH=ZT5:EU0NQG46EEG468QG:=V00[<B6I`@~21c0O2RAMJEUA::WC00OI3B6EE1R0~2b?;J=TEJ:9~234GIO0OIGJ;5>:F9U0~2R1<;J4~3BLD_f0~3`W9A;39EAWWU]0~4[>5E1O0NIG4;?Q64;T134;?ONQG4;?UEJMAe0NQG45D`UVN00c1LH6I9F5Ae0~2N8343>M=KE9F:AR0~2O1EFKH3?R0~37CCS9MF=IR0NQG468PNQG46MKE820~2NQG4BH:"">[X0~3c1LH6I_9FM>d00X2`>54J3?Rb0NQG46ELF;T20NQG46822b0~4RQ"
        "EB:BJ;H6D[]0~2NQG46=FFg;?aNQG4;B:<ZO0~5VEJ:D>KF9`0NQG4MFE3BBH=Z]NQG46EEG468PNQG468QG468P0O2RAMJEU2b]0~2O1EFKH3?R0~3RD:HLA<BB<C0NQG468134;IV=H5@20N22D:JEE9WIC00NQG46EE<T_^0~3^SLH;?GH=ZT5JEU0NQG4FBBF468QGH=V00_9F31UE5;IO0~2RAMJEUAMKWC00cZ<3JF31P0~2^V9MK55HJF~4;?a0_W345<9F5Ae0071E6~9>C0~2P1L?3;<VA9<LIO00Oe2I33?R00NIG468864GI134;?ONQG468_9JMAe0NQG468f]]0~3`<BM>3H38f0~2]VEJ;9B<5LLHGIO0~3e?G4M>`0~5PVW5J3?d00NQG468PNQG4F4GA[]0~2NQG4JFH3I20~4`<BM>8LH5DR0~32ILHK9ZP00NQG4FBF:EI20NQG468P0~6RI3JJggHH5>[0~3NQG465K34;?aNQG44FBESN0~4]V=H5@>KF9`0NQG46:G:G5H=Z]NQG4FBBF468PNQG468QG468P0~2RAMJEU0~6e?G4M>`0~4aI6JFMMJB<C0NQG468134;IV=H5@20~22D:4BBFKLIb0NQG4FBFB6Dd0~2XbIG46IGHLST5JEU0NQG46<<G46QIGH=V00`9F31VEJ;IO0~2RAMJEU>MKWC00eA5BL<<1eX0~228LHGGJ:9~5?a0[W;BM>3H38f00XVT~:?Y0~22T=9:9WIC?=9D2021A895>`0~2NIG4688"
        "64GQ134;?ONQG468_9JMAe0NQG468P0~52T;FE:B<_0~4[<BM=MA6;GH=S^0~2NSEBBH3?R0~3^_>;H6DC2N0NQG468PNQG465J3TC0~2NQG463HK9V^0~32T;FE<MHL8N0~2fS9KBJ5>[00NQG46<9:K<`0NQG468P0~5fS9BMLFH;GH3?R0~2NQG4KJ334;?aNQG4MEB:"">_]0~2]2SLH;?>KF9`0NQG463JFE5H=Z]NQG46<<G468PNQG468QG468P0~2RAMJEU0~5NSEBBH3?R0~4`A=L~2BB<C0NQG468134;IV=H5@20~22D:JE<EBM>Y0NQG46<9:F9_00]UI<MJE834;D<KF9`0NQG468134:"">>:FEU00Y>M5A1LH6QN0~22D546TEFM>d00CWKMAS1T@R0~2`WMK<<BB981I>=V]0aI@;FE:B<_0~27135~9<[0~2CW=3=E<82I=9DR0`<"":"">;ESN0~2NIG468864GQ134;?ONQG468_9JMAe0NQG468P0~57S=HGF5@R0~4RD5FG;?=M:B9`0~2]`>:FEMBESf0~2CD6H;AD~2Z^NQG468PNQG469MFL?d00NQG46>54;DY0~37S=H6;45>[0~22Z>5JL;H3?UPNQG468?;JEU0NQG468P0~5`>5F=AMJ9<MBESN00NQG4gMW34;?aNQG46?6H6Td0~2e?<5438>KF9`0NQG46<K5A5H=Z]NQG468QG468PNQG468QG468P0~2RAMJEU0~4]`>:FEMBESf0~37CZSS<BB<C0NQG"
        "46DD34;DD=H5@20~22D:JE8>MBWC0NQG468?;JEU00713M4:"">U9KJ5M4;De0NQG468U9KH::H5AY00aQ3JGE5K9U]0~2f1LJF5KJ3Ia00dA:J;LL;Ae002I3H3I?6JM33GM=V0~221=HGF5@R0~27134g~7B<[0~2`<GELTAA@>99T20C>M36Te0~3NIG468864GQ134;?ONQG468_9JMAe0NQG468P0~6[WK4HL1P0~4OI644319FgMAd0~2R?6H6T=F5>`0~2`9F4:~3317NQG468PNQG468EBB=QR0NQG468EFF=1O0~3[WKFHM9ZP0~2[<GH5LLFBLT2NQG4;6GFK<`0NQG468P0~4R?3H6TAMJ91=F5>`00NQG4JL834;?aNQG468>5F=1a00CWK4M<U>KF9`0NQG46?<<T5H=Z]NQG468QG468PNQG468QG468P0~2RAMJEU0~4R?6H6T=F5>`0~5]XC<BB<C0NQG4:~34M:~245@20~22D:H66;J5Ae0NQG4;6GFK<`00713MG912Q=:KK;<V^0NQG5682QE5KK;<Zc0~2CDL:KM=I20~4CD3MBM6A[0~2N19;KBK5Ae00U<55W``A3:KK:GEV0~3[<K4HL1P0~27134g~7B<[00O836<E9<<9<ELQO02?;M<_]0~3NIG568865GQ135;?ONQG568_955Ae0NQG568P0~62?;55<`0~5XZ=55EU>55;?a00PSE55<_D;53?200`95~5317NQG568PNQG568IL55E1NNQG568TG55>`0~32"
        "?;55=820~3[<5~7T2NQG5~3;EI20NQG568P0~3PSE55W_A559_T656?R0NQG55>135;?aNQG568Z955<U00CW56<1Y>559`0NQG568[[T55=Z]NQG568QG568PNQG568QG568P0~2RA55EU0~3PSE55<_D;53?20~6C<55<C0NQG5~;@20~22D5~46<Sc0NQG5~3;EQ20071WA?Vb0e1AW>TVO00PS@~2Z^R1AW>@Va0~3XCQAW>8d0~5X[I>W>?`70~32ZT>W>@1b00_@~28O^[8AWWA?ZR0~32?;55<`0~37135~9<[00Y>9?_UU[`_?TTU]0^Z99Qa0~4PS@~2ZZ@~2SV@~21NPS@~2Z[@~2?20PS@~2Z^0~6^Z@~2I20~6[@~2TCI@~2Z^00PS@~21aU@~2I200CT@~6Z^PS@~2Z^PS@~2ZY?@~21fPS@~2ZU@~3[0~3^Z@~2Ie0~4CT@~78aPS@~4?Se00PS@~2Z^0~3PS@~212I@~2[_@~2?20PS@~21V@~21NPS@~2Z2I@~2U00e?TQUaR?@@TC0PS@~2Z^a8@~2U0PS@~2ZS@~2Z7PS@~2ZS@~2Z70~22I@~2[0~3PS@~21aU@~2I20~6YT@@?Y0PS@~<Qa0~2bI@~3TIUa00PS@~4?ZR0~32CdbX0~2NeCY2^0~3O2~2N00NRCCR70~6OdCYO0~8adCYb]0~572YCY2c0~2P2~2a0~2OeCCeaX0~4^Z@~2I20~3^Z@~:TY00bSU2X0~4b22P0~22_UR0~6O2~2NO2~2Of2~2O00O2~2N^2"
        "~2b0~2O2~2N0~8N2~2b0~7^2~3Xb2~2N0~3O2~2a0P2~2b0~2X2~7N00O2~2N00O2~2N02~3O00O2~2NP2~3^0~4N2~2b0~5X2~8a00O2~4a70~3O2~2N0~5O2~2O0b2~2^72~2b0~2O2~2Of2~2O00O2~2N0b2~2P0~222O0~22~3X00O2~2N00a2~2P00O2~2NO2~2O00O2~2NO2~2O0~4b2~2^0~4O2~2a0P2~2b0~7]2~3]00O2~<a0~4b2~4O]0~3O2~4a^0~}0~FN2~2b0~5N2~;]0~2^]0~}0~}0~}0~}0~}00Xc~2X0~2Xc~2X0~4XPc~3P0~4]c~6P0~6Xc~2XXc~2X0~;Pcc70~6Xc~2X0~2^ccP0~7Pc~7P0~2Xc~2Xc~8X00^c~2]00Xa2bP0~6fP0~6c~60~2Xc~2X0c~30~2Xc~5^0c~30~2Xc~2XXc~5P0~3Pc~20PccP0~2]c~270Xc~270~77c~20~5c~6P0]c~270~3c~270c~2X]c~2^00Xc~2XXc~30~2Xc~2X0~2O2O]0~4c~270c~2X00O2O]0~3Xc~2X0~8PP]NO^0~7caa0~wPOOP0~4XNOOP0~5XccP0~4P~90~BO2a]0~377^0~2^U8~2UX0^U8~2UX00^CZ8~4Sa0~3`8~6120~4^U8~2UU8~2UX0~9aS88SO0~4^U8~2UX0PV88120~5218~71a0^U8~2U8~8_]PV8~2`0OUIT?Z70~2aUC1SR]0~3C8~6C0^U8~2U`8~3R0XU8~5VY8~3Y0^U8~2UU8~51200a"
        "18~2C8~3R00`8~2SOU8~2Zf0~5NZ8~2[0~3C8~61b`8~2SO00C8~2ZY8~2_`8~2U^^U8~2SS8~3R0^U8~2UXP`ITI_f0~2C8~2ZY8~2_P_ITI_f00^U8~2UX0~5O`11UQIZR0~4XPS?""?CX0~6a[27CY]0R[ON[e0~8a[200R[O0~5PCUC70~:cR~2200O_1?""?1C]00aZQ?I1C]00P[Z88Sa0~2C8~9C00Ne~9O00X[Q@I_P00[11ZO00NQ6~28P0NQ6~28P00`A=36~3AR0~2^SL6~5>Y0~4NQ6~28Q6~28P0~9`W66W[0~4NQ6~28P0YA669VX0~4YA6~7DR0NQ6~28E6~6=17OI6~2QO_WG5GIN0~2_<A<E?`0~3VE6~4EU0NQ6~29>366>C0P86~5II=66<_0NQ6~28Q6~5>Y00RA66EU<66<C0^SE66W_@663?a0~52T66=V]0~2UE6~5<U?6~2T200UE66DVE66Q8366QNNQ6~2LL366>C0f8366Ia`A656>UX00UE66DVE66Q`>656>UX0NQ6~28P0~4OZWLW96GE?d0~2aZ_I;9`Z[0~4R1<ZUA?b^S<SCW?CX0~5R1<Z]^S<S20~3XU>EA`0~2PR[CN0~2UD~2Ib0Y>3;;=Te00Y>6;;=Te00dA=66AR0~2UE6~7=V]0_A~9U00CD356>`0^SL3DR00NQG468P0NQG468P0OI3JK55H:AR0~2^SL4FBB4K>Y0~4NQG468QG468P0~8bI3HH3Qa0~3NQG468Pf1=JHGTR0~4f1=J:55:"
        "FEZ70NQG468@;J5~2F5AC0]VEF:AdI64MGIN002?656B=S^0~2VEJK5~2EU0NQG46=LF5WVf0P865~24;I=B:<ZO0NQG468QG4:5~2>Y00RAMJE><BB<C00e?6H6?=F5>`0~62T5H=V]0~2UEJJB~2HG@EB:W_X00CWKKW1345@T;J=ZXNQG46D6F5WVf00_9BM>CQ3H~2G?200CWKKW1345@I6H~2G?20NQG468P0~4`W5KW:HBB=8O00`<<I;9TEI20~2YIE=Z>;Tb^1398<L@`P0~3YIE=Z]^1398e0~2OI6W38aOC1D<>V^0]V=:5@20Y>L<=F9U00Y>E<=F9`00d>3=:AR0~2UEM~7=V]0UEM~7=V]^S=6<33QN^SL:DR00NQG468U`RQG468P02T5J=TEJ:AR0~2^SL4;W9FK>U`R^00NQG468QG468_Ca0~6_<K55M<`0~3NQG468CCA5M6B9V^0~4YA5;DA5;@e00NQG468VEF=TEF=1f00YA:F9U>MB9D1P0]`T=:H5>SR002VEJ:DT~2`0NQG46=J5<VO00^ZT~234;EK:9Sa00NQG468QG46T~2?R00RAMJE3<BB<C00P_W:BE:BESa0~3]2[UT5H=V]0~2UEJ:W~25F=5F=1O0~22T5JE?;:KW>MB<[0NQG46LFM=IR0~2RD5JEVA:FL;K>Y002T5JE?;:KW>MFL;K>Y0NQG468UU[X0N83HGEJ:9B:"">Y071LM=;9G;WC0][?=K=<5;Tb^13K=<"":6AUN0][?=K=Z]^13K=?C00O"
        "I393WIQ>=5691c0]V=H5@20e?8U<B9U00bS><=:W[002Q8=:AR0~2_A~9U00UE6~6K=V]c869_W5?b^SL:DR00NQG46EE<TQG468P0b?;J=TEJ:AR0~2^134;?9FKEE<@UP0NQG46EEG46E9WQd0~42I6FEEF3Ib0~2NQG46W~2=JL<K;@e0~4PS=FE=FEV^00NQG46W~2;:"">;5AC0~2OQ3438EJ;TRa002?E?6JB5EV071WEJ:W~2_^0NQG4;B:<ZO0~3N221345KM91b0~2NQG46EEG46822b0~22@54G3EBB<C00dA9EFKH39<_0~2][?<EE5H=V]0~2VEJ:DCZ9BKHGTd0~3N8343>M=KE9F:AR0NQG4;B:<EEU0~2c1LH6I9F5A=5WC00N8343>M=KE9F:A=5WC0NQG46E~2?a02@5J==H5AMF9`00C8W6B:=DVa0a?3MEEM6AU70C?=KL<5;>Y0a?3MEIY00C?=KLIO00[TW<L33:3WIU200^S=H;T2002V>;GDd0~2C<G:LDC0~2OZ=:AR0~2U>~9U00C8~6AM=V]71L3A=6IO^SL:DR00NQG4FBFB6DG468P0^V9MK55H:AR0~2P134;I9FHBFB;AC0NQG4FBBFgFBFM=8O0~2]U<K5EE5K<_0~2NQG4B~4MEE6FEZ70~3NVA55:;DUP00NQG4B~4JLF=IY0~3U9F5AGH=1P00]U9B6K;E5AY0P13B4JBB9_00NQG44FBESN0~5^1344FF=8a0~2NQG4FBBF468P0~4f1=KBFBHB<C00d"
        ">MF4ggFF9_0~2RT6KFBJH=V]00]V=H5@22I34KW_X0~3]VEJ;9B<5LLHGIO0NQG44FBESVR0~3`<BM>3H31TT?e00]VEJ;9B<5LLH6ITT?e0NQG4FBB;?a0RA:JE=H5AMJEU0XUD=55;6<QR0a?;3D=5<S200NUA3GIEK>Y0a?;3D_P00NUA3GIO00e1?D95569TZdP00OIGK9V^0OZW;6AVe00YI8IWKEV007`Z=:AUR00UEK~7=V]00P~5eAM=V]0[>;:5<U]^SL3DR00NQG46<9:F9G468P0028LHGGH:AR00^RI64689FK<95JEV]NQG46<<G46<=F:AR0~22?6JKFFKJ6?200NQG46EELHKBJKF;Dd0~2XUW;F4HFG>`00NQG46E~2JH4HKLI20~2eA5F9MK<[0~2]V>3BJ=T>SP0P1EEJ:EE9_00NQG4M=H:"">_]0~4^134KEKM<U^00NQG46<<G468P0~5YIW<3<BB<C00dA9EFB4399_0~2[<BM9<5H=V]0]2SLH;?bZ9KBHGTd0~4[<BM=MA6;GH=S^0NQG4MEB:"">_]0~32T;FE:B<_b~20~3[<BM=MA6;GH=Zb~200NQG46<~2?a0RD:H==H5WKF9_071=G<;9=:"">C0O89:6>G:98OXV>G59LKLTe0O89:6>U0XV>G591f0f1=3GEAAE;;EAZ^0eA:3I200CW5;==EV00`99<=FEU002DE=:EEV00UE3~7=V]0~7eA6=V]0cUDWAZa00[11VO00NQG468?;JEG468P00`WMK<EJ:"
        "AR00C@9MH=S9FK>IGH=Z]NQG468QG468<BM>d00XU9B;W5;W;K9U]0NQG468>M5W6MW3FESP002?GH;;6;H3Ia0NQG468<MBLBLKB9U0~2f1LH6J;?20~3NV9BMFGT20~2CVEJ:DUU200NQG46=6H6Td0~4^134;ILJ;DC00NQG468QG468P0~62C13<BB<C00P`>:FEMBESb0~2`9F5DT5H=V]0e?<5438[A5F=:F=1O0~3RD5FG;?=M:B9`00NQG46?6H6Td0~37S=HGF;@R0~6RD5FG;?=M:B9`0~4NQG468[[Y00OI64;=H5LJM>C00CATI;91W1O002S<;=D35Tb^13;W<MEIY]002S<;=Z]^13;WS200a?3?6QYdVT<LE1P0C><>`0~2CW6~3EV00`96;;=Td002D6~3EV00Y1~9C0~8b188C0~2^RCeP0~3^^X0~2NQG4;6GFK<G468P02I3H3IEJ:AR00`9BH5>`9FK6GBB9U0NQG468QG4;6;H;T200RTGB9?5;?9BG?R0NQG46Q3J=86MAWM5AY00CWKM>;6<KM>d0NQG468LH6WB9LH38f0~2_<B4J=Z^0~3OILKELEZ70~3VEJ:D20~3NQG46=W5F=1a0~3^134;IA;JLI20NQG468QG468P0~7]_8<BB<C00R?6H6T=F5>`0~2C>MJ;6:H=V]0CWK4M<UI3J;DEF:W_]0~2OI644319FgMAd00NQG468>5F=1a0~3[WK4HL1P0~6OI644319Fg:Ad0~4NQG468P0~3]U9MJ:4KBFLI"
        "a00PCeI=9`CR0~3OV>=Z?=Tb^13AU<91R0~4OV>=Z]^13AUO0~2PS96<VX0^2_SVR00PC~2f0~2R8Q~4C002ZQ?I1C]00OSQ~4C0~27~90~:P~20~BNQG5~3;EQG568P0U<55WUE55AR00`953WZ`95~3;EIR0NQG568QG5~36<V^0XV95G?""?5;?""?G59V]NQG56<55A865AQ35=1P0`95;?;6D55<C0NQG56865=W5995;?a0~2R@;55>C0~5CQ<TITe0~4VE55D20~3NQG569>955<U0~3^135;IV<559ZXNQG568QG568P0~8XC<55<C0PSE55<_D;53?200O19G5~3=V]0CW56<1Z955<UT656TR0~2XZ=55EU>55;?a00NQG568Z955<U0~32?;55<`0~7XZ=55EU>55;?a0~4NQG568P0~421965~2;=TC0~4X`VV20~6P_?_dQ1O]U?`d?Zb0~6P_?_0]U?`70~42V8Va0~4X]0~;cf~40~3XNaO70~4Pf~40~_PS@~4?ZS@~2Z^0_@~28`@~2I200CTT8`OCT@~3?Se00PS@~2ZS@~3TQUO00]V@~2U8@@8U@~2U]PS@~5SS@@I[@~2SP0CT@@1@@Q@@TY0PS@~2S@~2?@TT@@1N0~2PS@~2Qb0~6bU`bR]0~4`@~2Ib0~3PS@~2Z`I@~2U0~3XV@~21a8@~2VXPS@~2ZS@~2Z70~9YT@@?Y0PS@~21aU@~2I20~22VI@~4_00e?TQUaV@~2Qb_@~2?R0~3[@~2TCI@~2Z^00PS@~2Z2"
        "I@~2U0~3^Z@~2I20~8[@~2TCI@~2Z^0~4PS@~2Z^0~5bUI@~2?SY]0~6]]0~8Xa^0OO00Pa]0aP0~8Xa^00Pa]0~6XP]0~}0~@O2~4a^O2~2N00P2~2a72~2b0~2X22N00X2~4a70~3O2~2NO2~4O0~4c2~2Pa22aP2~2c00O2~5OO22b^2~2O00^2~2O22a2~2]00O2~2O2~8O0~4O2~2a0~B72~2b0~5O2~2N0b2~2P0~4f2~2O0a2~2N00O2~2NO2~2O0~:]2~3]00O2~2a0P2~2b0~4XO2~4P0~222O00N2~2a072~30~4^2~3Xb2~2N0~3O2~2N0b2~2P0~4N2~2b0~9^2~3Xb2~2N0~6O2~2N0~7]O2~2b70~}0~}0~3XN~20~4X770~47~2X0~:^7~20~4X77077^0~3X7~5X0~=7~2^7~20~4Xc70~577^77^0~6]77X0~6NNf0~3]~20~FX7~5^^770~:N~2X0~=^0~5N~2]0~:O22]0~;XN~20~:]~20~8]~20~;]X0~E^P~670~E7~20~47~50~47~70~3Pc~3P0~H]0~x^P~670~W]`I~2C0~2PV11C0~2d1~2VN0X2RR20~2aZ1~2R0~2PV11C11Sa00fV1~5Vf0X2RR2R~2O00`1~2S1~220~2eZ8SC^0~2[11Z11Sa0~4PU11Uc0~4CII1200OVZVO0~7OYR]0~8PV1~5ZS11e00cR~2200dQII_X0~3PYUUCb]XYSC0~3CI~2[00NR~220~27SD?C0~2^C[CN0~2]`I~2C00NR~2"
        "200bVZZC0~6bVZZC0~8a[VSVCa0~AOZ1~7200aR~2O2R~2P2RR2X0~2C1~2Y0~2C1~5Y0~2C1~7Y00218~31200R__[__Y00aY~420~2fCUZUdX0~6O`_~7`O00O`_~5200O`_~<R0^[_~>b0OZ1~720~VR?6GW_0~2CA3=Z]00_933@20YTDD?R00eA33<[0~2CA3=SE3Ad002T339933T20YTDD?D~2Z^713~2A33<C00d?E6=D[00^SL3D=3>d0~3X`A33>_^0~3UEGAe00d>=>d0~2aR~31>TC0~8b?3~3E3W<39`00UD~2Ib0[>GGTd0~2bVA9EWIVVT=V]002ILG9V00UA~2Ib07CSEL8O00RD<>C0~2R?6GW_00UA~2Ib0C<==1^0~5C<==1^0~6R1W=LE>IUeaNP0~<2D3~6>Y0N1D~2V?D~2`@DDTY00P1=39U00P1=39=39U00P1=39=3=39U00CW6~3>Y0XS99D99INXZ>~48f^`1AE=E@[0~6CW9~7WC00CW9~5U00CW9~<SX2@9~>_02D3~6>Y0~UO1EM9ZO00O8LB<[0~2_9J5@20C<""::"">d00eAMB<[00O8LB<A5G?a002T5J99J5T20C<""::"">3:31771343AMB<C07S=B4J3Ia0^SL:D=M>d0~3CD6JJGA[]00XZ=;?a00`<"":DR00eQA~3WM31P0~7b?E6E~23L6G9`0]V=:5@20fV<"":=1a00U9;FFK6EEG=V]0PV9KE1b0]V=M5@20dAA>5D200RAM<[00O1EM9Z"
        "O0]V=M5@20CWLE1^0~5C<E31^0~5O1EM63;M69A?QSa0~;2D6~6>Y0O?;:31A::EU<""::<C00eD5LQ200eD5LD5LQ200eD5LD5L5LQ200CWBHHK>Y0XSL:D;GINXSLK~2GINbT=56LG3QO0~5CWBJ~5BWC00CWBJ~3EU00CWBJ~:LSX2D:J~<9_02D6~6>Y0~TR_W:91Y0~2`WM5DR0~2U9J5@20C<BM>d00eA:B<[00`WM5DLJ=S^002T5F99J5T20C<BM>34317713H3A:B<[0a?;4ggMAe0^SL:D=M>d0~2d?LF33B3?Y00P1331P0]V=;?a0XZE:~4K38c0~7X[86EVE363G9`0]V=H5@200OS9M<U]0UE6993MFF;9V]0CA;=Q200]V=H5@20d>:M6?b00RA:98f0_W:91b00]V=H5@20eA5E1^0~5C<3EV]0~5[>MEI1T<35:;6AR0~;2D3~6>Y0O?;431AMJEU<BB<C0XVE;DC00XVE;DE;DC00XVE;DE;E;DC0~2CW=~3>Y0XS==D==INXS<~4QNbTL<I1A6@20~5CWL~7WC00CWL~4EU00CWL~<SX2DL~=9_0OSQ~6120~SXS99>99IN00V=H;?O00P1LFESP0_9B;De00O1=J=ZX0V=H;T5JEU0~2CWKGTLBEZP0_9B;D6F=170`>:5T=J=ZXa?;4ggMAe0^SL:D=M>d002Q=K=?""?=K=IR0OIGEV00c836QN0^S=B9>~3T[0~9P86EVE3<WG9`0]V=55@20~2b1>>ZX0UETUUQ>99A"
        "Vb00`>>Ie0~2]V=55@20RIA>?`]00O8<<QN0V>>820~2]V=55@20PZWE1^0~5C<9?e0~6_>>Q2P2_8DW9EAR0~;2D6~6>Y0O?;531A55EU<55<C071WWU^0071WW1WWU^0071WW1W~3U^0~2aV~4UO00YVV_VV[]02[~4e0NZ1`aOS8SO0~52ZS~7Z2002ZS~5e002ZS~<C0fVS~>R00Pf~6c0~TXSL:D;GIN00V=F;?O00b?;GTe0^SLB9V70~2YA;6IO0V=F;T5FEU00]VEK<?;GTR0^SLB9A:;DY00O1=KWA;6IOP1=F4J6Ia0^SL3D=3>d0]V93EQYe893EZXO8AA[00aI<<1700C@<?Y~3RX0~9P86EVE31IG9`00_@~2Qa0~3bdda00dSY]0OC__dP0~2fddR0~4_T~28a00aRd2X0~3R[[d00Odd20~4_T~28a00O[UY0~6aU_R0~7OYYR0~3NRC_U_c0~;OSQ~6120N1@~2VI@~2`T@@?Y002CCO0~32CC2CCO0~32CC2C~3O0~O7P0~27c70~7X~90~3X~60~3X~=0~2]X~>0~aXS==D==IN00V<~2IO00RA<<UX0b?;6?20~37Z<<T20V<~2?<~2_0071<<IA<<UX0b?;6?<59V^0~2YA<<Z<<T20YT=;3A`0~2[11V11Za00C11S2002Z11[002RR70~2Y[[R0~2Xd[R0~?XUII[IIUZIIY00P2~2a0~;X0~D7b~2O0~J7b~2O0~fPf~6c0~2O2~2fb2~272~3]0~}0~}0~OYVV_VV[]"
        "002[~2d0~2^C[[a00a8TT_0~5b[[CX02[~2Y[~2a0~2R[[dC[[O00a8TT_TTI20~3^C[[2[[Y]0]Y1I8[70~3^^X^~20~377^0~3^770~U^OO]OO^POO0~}0~}0~}0~}0~pObb^0~RObb^bba0~@POc0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}"
        "0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0"
        "~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}"
        "0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0~}0"};

        wOut=hOut=512;
        const unsigned num_indices = sizeof(indices)/sizeof(indices[0])-1;  // or strlen(indices);
    #   ifdef SDF_BIG_ENDIAN /* define this on big endian machines */
        unsigned tmp;const unsigned char* pTmp = (const unsigned char*) &tmp;
        unsigned char* ppal = (unsigned char*) palette;
        for (i=0;i<numPalette;i++) {
            tmp = palette[i];   /* Not sure this is correct: */
            *ppal++ = pTmp[3];  *ppal++ = pTmp[2];  *ppal++ = pTmp[1];  *ppal++ = pTmp[0];
        }
    #   endif
        rawRGBAOut.resize(4*wOut*hOut);
#       define SDF_USE_SAFER_CODE_FOR_FONT_IMAGE_DECOMPRESSION
#       ifndef SDF_USE_SAFER_CODE_FOR_FONT_IMAGE_DECOMPRESSION
        {
            // Not sure if there might be alignment issues with this code on some machines
            // (rawRGBAOut is SdfVector<unsigned char>, and we cast it to unsigned int*):
            unsigned hasReps=0,*pRaw;const char* pc;char lastChar=(char)255,c,j;
            pRaw = (unsigned int*) &rawRGBAOut[0];
            for (pc=indices;*pc!='\0';++pc)  {
                c = *pc;if (hasReps==0 && c=='~') {hasReps=1;continue;}
                c = c>='\\' ? (c-'1') : (c-'0');
                if (hasReps) {
                    hasReps = palette[(unsigned char)lastChar];
                    for (j=0;j<c;j++) *pRaw++ = hasReps;
                    hasReps=0;continue;
                }
                *pRaw++ = palette[(unsigned char)c];lastChar = c;
            }
            // Note that if is safe to do the opposite:
            // declare rawRGBAOut SdfVector<unsigned int> (its size must be 1/4) and then cast it as unsigned char pointer.
        }
#       else // SDF_USE_SAFER_CODE_FOR_FONT_IMAGE_DECOMPRESSION
        {
            unsigned hasReps=0;unsigned char* pRaw;
            const char *pc;unsigned char* ppal;char lastChar=(char)255,c,j,k;
            pRaw = (unsigned char*) &rawRGBAOut[0];
            for (pc=indices;*pc!='\0';++pc)  {
                c = *pc;if (hasReps==0 && c=='~') {hasReps=1;continue;}
                c = c>='\\' ? (c-'1') : (c-'0');
                if (hasReps) {
                    hasReps = palette[(unsigned char)lastChar];
                    for (j=0;j<c;j++) {
                        ppal = (unsigned char*) &hasReps;
                        for (k=0;k<4;k++) *pRaw++ = *ppal++;
                    }
                    hasReps=0;continue;
                }
                ppal = (unsigned char*) &palette[(unsigned char)c];
                for (k=0;k<4;k++) *pRaw++ = *ppal++;
                lastChar = c;
            }
        }
#       endif // SDF_USE_SAFER_CODE_FOR_FONT_IMAGE_DECOMPRESSION
    }

    return true;

}
#endif //SDF_NO_EMBEDDED_FONT

#ifndef SDF_NO_NAMESPACE
} // namespace
#endif

#endif //SDF_IMPLEMENTATION_H
#endif //SDF_IMPLEMENTATION

