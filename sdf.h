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

        // Here we must fill all the fields of "f" based on "pdata" and "data_size":
        char tmp[1024];tmp[0]='\0';
        int tmpi=0;size_t gsize=0,ksize=0;
        CharDescriptor g;
        const char* buf_end = pdata + data_size;
        for (const char* line_start = pdata; line_start < buf_end; ) {
            const char* line_end = line_start;
            while (line_end < buf_end && *line_end != '\n' && *line_end != '\r') line_end++;

            if (strncmp(line_start,"char ",4)==0 && GlyphsCount>0)  {
                //char id=32   x=236   y=116   width=3     height=1     xoffset=-1    yoffset=15    xadvance=4     page=0  chnl=15
                int a[7] = {0,0,0,0,0,0,0};
                float b[3] = {0,0,0};
                if (sscanf(&line_start[4], " id=%i  x=%i y=%i width=%i height=%i xoffset=%f yoffset=%f xadvance=%f page=%i chnl=%i",
                           &a[0],&a[1],&a[2],&a[3],&a[4],&b[0],&b[1],&b[2],&a[5],&a[6]))
                {
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
            else if (strncmp(line_start,"kerning ",7)==0 && KerningsCount>0)  {
                int a[3] = {0,0,0};
                if (sscanf(&line_start[7]," first=%i  second=%i amount=%i",&a[0],&a[1],&a[2]))
                {
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
        const char fntData[] = "info face=\"DejaVu Sans Condensed Bold\" size=16 bold=1 italic=0 charset=\"\" unicode=0 stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=-4,-4\n"	\
                               "common lineHeight=19 base=15 scaleW=512 scaleH=512 pages=1 packed=0\n"	\
                               "page id=0 file=\"DejaVuSansCondensed-Bold-512x512.png\"\n"	\
                               "chars count=853\n"	\
                               "char id=0       x=426  y=67   width=12   height=19   xoffset=-2   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=13      x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                               "char id=32      x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=33      x=254  y=248  width=7    height=16   xoffset=0    yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=34      x=97   y=386  width=10   height=9    xoffset=-1   yoffset=1    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=35      x=261  y=248  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=36      x=101  y=26   width=13   height=20   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=37      x=268  y=162  width=18   height=17   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=38      x=286  y=162  width=16   height=17   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=39      x=504  y=373  width=7    height=9    xoffset=-1   yoffset=1    xadvance=4    page=0    chnl=0 \n"	\
                               "char id=40      x=114  y=26   width=9    height=20   xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=41      x=123  y=26   width=9    height=20   xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=42      x=362  y=373  width=12   height=12   xoffset=-2   yoffset=1    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=43      x=206  y=329  width=14   height=15   xoffset=-1   yoffset=2    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=44      x=429  y=373  width=8    height=11   xoffset=-2   yoffset=9    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=45      x=360  y=386  width=10   height=7    xoffset=-2   yoffset=7    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=46      x=169  y=386  width=7    height=8    xoffset=-1   yoffset=9    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=47      x=247  y=125  width=10   height=18   xoffset=-2   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=48      x=302  y=162  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=49      x=277  y=248  width=13   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=50      x=290  y=248  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=51      x=316  y=162  width=13   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=52      x=302  y=248  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=53      x=329  y=162  width=13   height=17   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=54      x=342  y=162  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=55      x=316  y=248  width=13   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=56      x=356  y=162  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=57      x=370  y=162  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=58      x=502  y=345  width=8    height=13   xoffset=-1   yoffset=4    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=59      x=329  y=248  width=9    height=16   xoffset=-2   yoffset=4    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=60      x=290  y=329  width=14   height=14   xoffset=-1   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=61      x=467  y=373  width=14   height=10   xoffset=-1   yoffset=5    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=62      x=304  y=329  width=14   height=14   xoffset=-1   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=63      x=338  y=248  width=11   height=16   xoffset=-1   yoffset=1    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=64      x=438  y=67   width=18   height=19   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=65      x=349  y=248  width=16   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=66      x=365  y=248  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=67      x=384  y=162  width=14   height=17   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=68      x=378  y=248  width=15   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=69      x=393  y=248  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=70      x=405  y=248  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=71      x=398  y=162  width=15   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=72      x=417  y=248  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=73      x=431  y=248  width=8    height=16   xoffset=-1   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=74      x=132  y=26   width=10   height=20   xoffset=-3   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=75      x=439  y=248  width=15   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=76      x=454  y=248  width=12   height=16   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=77      x=466  y=248  width=16   height=16   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=78      x=482  y=248  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=79      x=413  y=162  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=80      x=496  y=248  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=81      x=456  y=67   width=16   height=19   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=82      x=0    y=265  width=14   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=83      x=429  y=162  width=13   height=17   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=84      x=14   y=265  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=85      x=442  y=162  width=14   height=17   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=86      x=28   y=265  width=16   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=87      x=44   y=265  width=20   height=16   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=88      x=64   y=265  width=15   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=89      x=79   y=265  width=16   height=16   xoffset=-3   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=90      x=95   y=265  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=91      x=142  y=26   width=9    height=20   xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=92      x=257  y=125  width=10   height=18   xoffset=-2   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=93      x=151  y=26   width=10   height=20   xoffset=-2   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=94      x=107  y=386  width=14   height=9    xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=95      x=465  y=386  width=12   height=6    xoffset=-2   yoffset=15   xadvance=7    page=0    chnl=0 \n"	\
                               "char id=96      x=176  y=386  width=9    height=8    xoffset=-2   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=97      x=318  y=329  width=13   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=98      x=267  y=125  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=99      x=331  y=329  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=100     x=280  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=101     x=343  y=329  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=102     x=456  y=162  width=11   height=17   xoffset=-2   yoffset=0    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=103     x=467  y=162  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=104     x=481  y=162  width=13   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=105     x=494  y=162  width=7    height=17   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=106     x=120  y=0    width=9    height=21   xoffset=-3   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=107     x=0    y=180  width=13   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=108     x=501  y=162  width=7    height=17   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=109     x=150  y=359  width=17   height=13   xoffset=-1   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=110     x=167  y=359  width=13   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=111     x=357  y=329  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=112     x=13   y=180  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=113     x=26   y=180  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=114     x=180  y=359  width=11   height=13   xoffset=-1   yoffset=4    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=115     x=371  y=329  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=116     x=109  y=265  width=11   height=16   xoffset=-2   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=117     x=383  y=329  width=13   height=14   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=118     x=191  y=359  width=14   height=13   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=119     x=205  y=359  width=17   height=13   xoffset=-2   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=120     x=222  y=359  width=14   height=13   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=121     x=40   y=180  width=14   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=122     x=236  y=359  width=12   height=13   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=123     x=161  y=26   width=12   height=20   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=124     x=129  y=0    width=7    height=21   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=125     x=173  y=26   width=12   height=20   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=126     x=185  y=386  width=14   height=8    xoffset=-1   yoffset=6    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=160     x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=161     x=120  y=265  width=7    height=16   xoffset=0    yoffset=4    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=162     x=472  y=67   width=12   height=19   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=163     x=127  y=265  width=13   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=164     x=396  y=329  width=13   height=14   xoffset=-2   yoffset=3    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=165     x=140  y=265  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=166     x=503  y=0    width=7    height=19   xoffset=-1   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=167     x=294  y=125  width=12   height=18   xoffset=-2   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=168     x=370  y=386  width=9    height=7    xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=169     x=154  y=265  width=15   height=16   xoffset=0    yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=170     x=409  y=329  width=11   height=14   xoffset=-1   yoffset=1    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=171     x=374  y=373  width=11   height=12   xoffset=-1   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=172     x=481  y=373  width=14   height=10   xoffset=-1   yoffset=5    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=173     x=360  y=386  width=10   height=7    xoffset=-2   yoffset=7    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=174     x=169  y=265  width=15   height=16   xoffset=0    yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=175     x=379  y=386  width=9    height=7    xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=176     x=495  y=373  width=9    height=10   xoffset=-1   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=177     x=220  y=329  width=14   height=15   xoffset=-1   yoffset=2    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=178     x=437  y=373  width=10   height=11   xoffset=-2   yoffset=1    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=179     x=447  y=373  width=10   height=11   xoffset=-2   yoffset=1    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=180     x=199  y=386  width=9    height=8    xoffset=0    yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=181     x=54   y=180  width=14   height=17   xoffset=-1   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=182     x=306  y=125  width=12   height=18   xoffset=-2   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=183     x=208  y=386  width=7    height=8    xoffset=-1   yoffset=5    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=184     x=215  y=386  width=9    height=8    xoffset=-1   yoffset=13   xadvance=7    page=0    chnl=0 \n"	\
                               "char id=185     x=457  y=373  width=10   height=11   xoffset=-2   yoffset=1    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=186     x=420  y=329  width=12   height=14   xoffset=-2   yoffset=1    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=187     x=385  y=373  width=12   height=12   xoffset=-1   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=188     x=68   y=180  width=18   height=17   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=189     x=86   y=180  width=19   height=17   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=190     x=105  y=180  width=18   height=17   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=191     x=123  y=180  width=11   height=17   xoffset=-1   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=192     x=484  y=67   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=193     x=0    y=87   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=194     x=16   y=87   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=195     x=32   y=87   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=196     x=48   y=87   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=197     x=64   y=87   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=198     x=184  y=265  width=19   height=16   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=199     x=185  y=26   width=14   height=20   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=200     x=80   y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=201     x=92   y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=202     x=104  y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=203     x=116  y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=204     x=500  y=67   width=9    height=19   xoffset=-2   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=205     x=128  y=87   width=8    height=19   xoffset=-1   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=206     x=136  y=87   width=11   height=19   xoffset=-3   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=207     x=147  y=87   width=9    height=19   xoffset=-2   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=208     x=203  y=265  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=209     x=156  y=87   width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=210     x=199  y=26   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=211     x=215  y=26   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=212     x=231  y=26   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=213     x=247  y=26   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=214     x=263  y=26   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=215     x=432  y=329  width=14   height=14   xoffset=-1   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=216     x=318  y=125  width=16   height=18   xoffset=-2   yoffset=0    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=217     x=279  y=26   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=218     x=293  y=26   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=219     x=307  y=26   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=220     x=321  y=26   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=221     x=170  y=87   width=16   height=19   xoffset=-3   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=222     x=219  y=265  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=223     x=334  y=125  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=224     x=347  y=125  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=225     x=360  y=125  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=226     x=373  y=125  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=227     x=386  y=125  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=228     x=399  y=125  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=229     x=335  y=26   width=13   height=20   xoffset=-2   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=230     x=446  y=329  width=19   height=14   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=231     x=134  y=180  width=12   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=232     x=412  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=233     x=426  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=234     x=440  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=235     x=454  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=236     x=146  y=180  width=9    height=17   xoffset=-3   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=237     x=155  y=180  width=9    height=17   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=238     x=164  y=180  width=11   height=17   xoffset=-3   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=239     x=175  y=180  width=9    height=17   xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=240     x=468  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=241     x=184  y=180  width=13   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=242     x=482  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=243     x=496  y=125  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=244     x=0    y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=245     x=14   y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=246     x=28   y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=247     x=465  y=329  width=14   height=14   xoffset=-1   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=248     x=234  y=329  width=14   height=15   xoffset=-2   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=249     x=42   y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=250     x=55   y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=251     x=68   y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=252     x=81   y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=253     x=136  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=254     x=150  y=0    width=13   height=21   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=255     x=163  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=256     x=186  y=87   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=257     x=94   y=144  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=258     x=202  y=87   width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=259     x=107  y=144  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=260     x=348  y=26   width=16   height=20   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=261     x=197  y=180  width=13   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=262     x=364  y=26   width=14   height=20   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=263     x=120  y=144  width=12   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=264     x=378  y=26   width=14   height=20   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=265     x=132  y=144  width=13   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=266     x=392  y=26   width=14   height=20   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=267     x=145  y=144  width=12   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=268     x=406  y=26   width=14   height=20   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=269     x=157  y=144  width=12   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=270     x=218  y=87   width=15   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=271     x=169  y=144  width=19   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=272     x=203  y=265  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=273     x=188  y=144  width=15   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=274     x=233  y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=275     x=203  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=276     x=245  y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=277     x=217  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=278     x=257  y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=279     x=231  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=280     x=420  y=26   width=12   height=20   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=281     x=210  y=180  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=282     x=269  y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=283     x=245  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=284     x=432  y=26   width=15   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=285     x=177  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=286     x=447  y=26   width=15   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=287     x=191  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=288     x=462  y=26   width=15   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=289     x=205  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=290     x=477  y=26   width=15   height=20   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=291     x=219  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=292     x=281  y=87   width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=293     x=295  y=87   width=15   height=19   xoffset=-3   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=294     x=232  y=265  width=16   height=16   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=295     x=224  y=180  width=14   height=17   xoffset=-1   yoffset=0    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=296     x=310  y=87   width=10   height=19   xoffset=-3   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=297     x=238  y=180  width=10   height=17   xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=298     x=320  y=87   width=9    height=19   xoffset=-2   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=299     x=248  y=180  width=9    height=17   xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=300     x=329  y=87   width=9    height=19   xoffset=-2   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=301     x=257  y=180  width=9    height=17   xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=302     x=492  y=26   width=9    height=20   xoffset=-1   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=303     x=233  y=0    width=9    height=21   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=304     x=338  y=87   width=8    height=19   xoffset=-1   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=305     x=248  y=359  width=7    height=13   xoffset=-1   yoffset=4    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=306     x=0    y=47   width=13   height=20   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=307     x=242  y=0    width=12   height=21   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=308     x=51   y=0    width=11   height=23   xoffset=-3   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=309     x=254  y=0    width=11   height=21   xoffset=-3   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=310     x=13   y=47   width=15   height=20   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=311     x=265  y=0    width=13   height=21   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=312     x=255  y=359  width=13   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=313     x=346  y=87   width=12   height=19   xoffset=-1   yoffset=-2   xadvance=9    page=0    chnl=0 \n"	\
                               "char id=314     x=358  y=87   width=8    height=19   xoffset=-1   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=315     x=28   y=47   width=12   height=20   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=316     x=278  y=0    width=8    height=21   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=317     x=248  y=265  width=12   height=16   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=318     x=266  y=180  width=10   height=17   xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=319     x=260  y=265  width=12   height=16   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=320     x=276  y=180  width=10   height=17   xoffset=-1   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=321     x=272  y=265  width=14   height=16   xoffset=-3   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=322     x=286  y=180  width=11   height=17   xoffset=-3   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=323     x=366  y=87   width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=324     x=297  y=180  width=13   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=325     x=40   y=47   width=14   height=20   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=326     x=310  y=180  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=327     x=380  y=87   width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=328     x=323  y=180  width=13   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=329     x=286  y=265  width=18   height=16   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=330     x=54   y=47   width=14   height=20   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=331     x=336  y=180  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=332     x=68   y=47   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=333     x=259  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=334     x=84   y=47   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=335     x=273  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=336     x=100  y=47   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=337     x=287  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=338     x=349  y=180  width=20   height=17   xoffset=-2   yoffset=1    xadvance=17   page=0    chnl=0 \n"	\
                               "char id=339     x=479  y=329  width=20   height=14   xoffset=-2   yoffset=4    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=340     x=394  y=87   width=14   height=19   xoffset=-1   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=341     x=369  y=180  width=11   height=17   xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=342     x=116  y=47   width=14   height=20   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=343     x=380  y=180  width=11   height=17   xoffset=-1   yoffset=4    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=344     x=408  y=87   width=14   height=19   xoffset=-1   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=345     x=391  y=180  width=11   height=17   xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=346     x=130  y=47   width=13   height=20   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=347     x=301  y=144  width=12   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=348     x=143  y=47   width=13   height=20   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=349     x=313  y=144  width=12   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=350     x=156  y=47   width=13   height=20   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=351     x=402  y=180  width=12   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=352     x=169  y=47   width=13   height=20   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=353     x=325  y=144  width=12   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=354     x=182  y=47   width=14   height=20   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=355     x=196  y=47   width=11   height=20   xoffset=-2   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=356     x=422  y=87   width=14   height=19   xoffset=-2   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=357     x=414  y=180  width=11   height=17   xoffset=-2   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=358     x=304  y=265  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=359     x=318  y=265  width=11   height=16   xoffset=-2   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=360     x=207  y=47   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=361     x=337  y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=362     x=221  y=47   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=363     x=350  y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=364     x=235  y=47   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=365     x=363  y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=366     x=286  y=0    width=14   height=21   xoffset=-1   yoffset=-3   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=367     x=249  y=47   width=13   height=20   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=368     x=262  y=47   width=14   height=20   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=369     x=376  y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=370     x=276  y=47   width=14   height=20   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=371     x=425  y=180  width=14   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=372     x=436  y=87   width=20   height=19   xoffset=-2   yoffset=-2   xadvance=16   page=0    chnl=0 \n"	\
                               "char id=373     x=439  y=180  width=17   height=17   xoffset=-2   yoffset=0    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=374     x=456  y=87   width=16   height=19   xoffset=-3   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=375     x=300  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=376     x=472  y=87   width=16   height=19   xoffset=-3   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=377     x=488  y=87   width=14   height=19   xoffset=-2   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=378     x=456  y=180  width=12   height=17   xoffset=-2   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=379     x=0    y=106  width=14   height=19   xoffset=-2   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=380     x=468  y=180  width=12   height=17   xoffset=-2   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=381     x=14   y=106  width=14   height=19   xoffset=-2   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=382     x=480  y=180  width=12   height=17   xoffset=-2   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=383     x=492  y=180  width=11   height=17   xoffset=-2   yoffset=0    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=884     x=121  y=386  width=7    height=9    xoffset=-1   yoffset=0    xadvance=4    page=0    chnl=0 \n"	\
                               "char id=885     x=128  y=386  width=7    height=9    xoffset=-1   yoffset=12   xadvance=4    page=0    chnl=0 \n"	\
                               "char id=890     x=224  y=386  width=7    height=8    xoffset=0    yoffset=13   xadvance=7    page=0    chnl=0 \n"	\
                               "char id=891     x=499  y=329  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=892     x=0    y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=893     x=12   y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=894     x=329  y=265  width=9    height=16   xoffset=-2   yoffset=4    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=900     x=231  y=386  width=9    height=8    xoffset=0    yoffset=0    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=901     x=0    y=386  width=10   height=10   xoffset=-1   yoffset=-3   xadvance=7    page=0    chnl=0 \n"	\
                               "char id=902     x=0    y=197  width=16   height=17   xoffset=-2   yoffset=0    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=903     x=240  y=386  width=7    height=8    xoffset=-1   yoffset=5    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=904     x=16   y=197  width=16   height=17   xoffset=-3   yoffset=0    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=905     x=32   y=197  width=18   height=17   xoffset=-3   yoffset=0    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=906     x=50   y=197  width=12   height=17   xoffset=-3   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=908     x=389  y=144  width=18   height=18   xoffset=-3   yoffset=0    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=910     x=62   y=197  width=20   height=17   xoffset=-3   yoffset=0    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=911     x=82   y=197  width=18   height=17   xoffset=-3   yoffset=0    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=912     x=314  y=0    width=10   height=21   xoffset=-2   yoffset=-3   xadvance=6    page=0    chnl=0 \n"	\
                               "char id=913     x=338  y=265  width=16   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=914     x=354  y=265  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=915     x=367  y=265  width=12   height=16   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=916     x=379  y=265  width=16   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=917     x=395  y=265  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=918     x=407  y=265  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=919     x=421  y=265  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=920     x=100  y=197  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=921     x=435  y=265  width=8    height=16   xoffset=-1   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=922     x=443  y=265  width=15   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=923     x=458  y=265  width=16   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=924     x=474  y=265  width=16   height=16   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=925     x=490  y=265  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=926     x=0    y=281  width=11   height=16   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=927     x=116  y=197  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=928     x=11   y=281  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=929     x=25   y=281  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=931     x=38   y=281  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=932     x=50   y=281  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=933     x=64   y=281  width=16   height=16   xoffset=-3   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=934     x=80   y=281  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=935     x=96   y=281  width=15   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=936     x=111  y=281  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=937     x=127  y=281  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=938     x=502  y=87   width=9    height=19   xoffset=-2   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=939     x=472  y=87   width=16   height=19   xoffset=-3   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=940     x=407  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=941     x=421  y=144  width=12   height=18   xoffset=-2   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=942     x=324  y=0    width=13   height=21   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=943     x=433  y=144  width=9    height=18   xoffset=-1   yoffset=0    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=944     x=337  y=0    width=13   height=21   xoffset=-1   yoffset=-3   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=945     x=24   y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=946     x=350  y=0    width=13   height=21   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=947     x=132  y=197  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=948     x=442  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=949     x=38   y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=950     x=363  y=0    width=12   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=951     x=146  y=197  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=952     x=456  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=953     x=50   y=345  width=9    height=14   xoffset=-1   yoffset=4    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=954     x=268  y=359  width=13   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=955     x=159  y=197  width=13   height=17   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=956     x=172  y=197  width=14   height=17   xoffset=-1   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=957     x=281  y=359  width=14   height=13   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=958     x=375  y=0    width=12   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=959     x=59   y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=960     x=73   y=345  width=15   height=14   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=961     x=186  y=197  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=962     x=199  y=197  width=12   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=963     x=88   y=345  width=15   height=14   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=964     x=103  y=345  width=13   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=965     x=116  y=345  width=13   height=14   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=966     x=211  y=197  width=15   height=17   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=967     x=226  y=197  width=13   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=968     x=239  y=197  width=15   height=17   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=969     x=129  y=345  width=16   height=14   xoffset=-2   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=970     x=470  y=144  width=10   height=18   xoffset=-2   yoffset=0    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=971     x=480  y=144  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=972     x=493  y=144  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=973     x=0    y=162  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=974     x=13   y=162  width=16   height=18   xoffset=-2   yoffset=0    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=976     x=29   y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=977     x=43   y=162  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=978     x=143  y=281  width=16   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=979     x=254  y=197  width=20   height=17   xoffset=-3   yoffset=0    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=980     x=28   y=106  width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=981     x=387  y=0    width=15   height=21   xoffset=-2   yoffset=0    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=982     x=145  y=345  width=17   height=14   xoffset=-2   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=983     x=274  y=197  width=14   height=17   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=984     x=290  y=47   width=16   height=20   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=985     x=288  y=197  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=986     x=306  y=47   width=14   height=20   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=987     x=302  y=197  width=12   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=988     x=159  y=281  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=989     x=402  y=0    width=12   height=21   xoffset=-3   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=990     x=171  y=281  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=991     x=314  y=197  width=12   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=992     x=320  y=47   width=17   height=20   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=993     x=185  y=281  width=13   height=16   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=994     x=337  y=47   width=20   height=20   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=995     x=326  y=197  width=16   height=17   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=996     x=357  y=47   width=15   height=20   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=997     x=342  y=197  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=998     x=372  y=47   width=16   height=20   xoffset=-1   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=999     x=248  y=329  width=14   height=15   xoffset=-2   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1000    x=388  y=47   width=14   height=20   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1001    x=356  y=197  width=13   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1002    x=198  y=281  width=15   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1003    x=295  y=359  width=13   height=13   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1004    x=56   y=162  width=15   height=18   xoffset=-2   yoffset=0    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1005    x=71   y=162  width=13   height=18   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1006    x=402  y=47   width=14   height=20   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1007    x=416  y=47   width=13   height=20   xoffset=-2   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1008    x=162  y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1009    x=369  y=197  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1010    x=176  y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1011    x=414  y=0    width=9    height=21   xoffset=-3   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1012    x=382  y=197  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1013    x=188  y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1014    x=200  y=345  width=12   height=14   xoffset=-1   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1015    x=213  y=281  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1016    x=423  y=0    width=13   height=21   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1017    x=398  y=197  width=14   height=17   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1018    x=226  y=281  width=16   height=16   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1019    x=412  y=197  width=13   height=17   xoffset=-1   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1020    x=425  y=197  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1021    x=439  y=197  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1022    x=453  y=197  width=14   height=17   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1023    x=467  y=197  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1024    x=44   y=106  width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1025    x=56   y=106  width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1026    x=429  y=47   width=16   height=20   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1027    x=68   y=106  width=12   height=19   xoffset=-1   yoffset=-2   xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1028    x=481  y=197  width=14   height=17   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1029    x=495  y=197  width=13   height=17   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1030    x=242  y=281  width=8    height=16   xoffset=-1   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1031    x=80   y=106  width=9    height=19   xoffset=-2   yoffset=-2   xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1032    x=501  y=26   width=10   height=20   xoffset=-3   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1033    x=250  y=281  width=20   height=16   xoffset=-2   yoffset=1    xadvance=17   page=0    chnl=0 \n"	\
                               "char id=1034    x=270  y=281  width=19   height=16   xoffset=-1   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1035    x=289  y=281  width=16   height=16   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1036    x=89   y=106  width=15   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1037    x=104  y=106  width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1038    x=118  y=106  width=15   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1039    x=133  y=106  width=14   height=19   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1040    x=305  y=281  width=16   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1041    x=321  y=281  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1042    x=334  y=281  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1043    x=347  y=281  width=12   height=16   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1044    x=147  y=106  width=16   height=19   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1045    x=359  y=281  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1046    x=371  y=281  width=22   height=16   xoffset=-2   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1047    x=0    y=214  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1048    x=393  y=281  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1049    x=163  y=106  width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1050    x=407  y=281  width=15   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1051    x=422  y=281  width=15   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1052    x=437  y=281  width=16   height=16   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1053    x=453  y=281  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1054    x=14   y=214  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1055    x=467  y=281  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1056    x=481  y=281  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1057    x=398  y=197  width=14   height=17   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1058    x=494  y=281  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1059    x=0    y=297  width=15   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1060    x=15   y=297  width=18   height=16   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1061    x=33   y=297  width=15   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1062    x=177  y=106  width=16   height=19   xoffset=-1   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1063    x=48   y=297  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1064    x=62   y=297  width=20   height=16   xoffset=-1   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1065    x=193  y=106  width=22   height=19   xoffset=-1   yoffset=1    xadvance=19   page=0    chnl=0 \n"	\
                               "char id=1066    x=82   y=297  width=17   height=16   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1067    x=99   y=297  width=18   height=16   xoffset=-1   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1068    x=117  y=297  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1069    x=30   y=214  width=14   height=17   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1070    x=44   y=214  width=20   height=17   xoffset=-1   yoffset=1    xadvance=17   page=0    chnl=0 \n"	\
                               "char id=1071    x=130  y=297  width=14   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1072    x=212  y=345  width=13   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1073    x=84   y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1074    x=308  y=359  width=12   height=13   xoffset=-1   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1075    x=320  y=359  width=11   height=13   xoffset=-1   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1076    x=144  y=297  width=15   height=16   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1077    x=225  y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1078    x=331  y=359  width=19   height=13   xoffset=-2   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1079    x=239  y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1080    x=350  y=359  width=12   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1081    x=64   y=214  width=12   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1082    x=362  y=359  width=13   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1083    x=375  y=359  width=14   height=13   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1084    x=389  y=359  width=14   height=13   xoffset=-1   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1085    x=403  y=359  width=12   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1086    x=251  y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1087    x=415  y=359  width=12   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1088    x=76   y=214  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1089    x=265  y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1090    x=427  y=359  width=13   height=13   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1091    x=89   y=214  width=14   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1092    x=436  y=0    width=18   height=21   xoffset=-2   yoffset=0    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1093    x=440  y=359  width=14   height=13   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1094    x=159  y=297  width=14   height=16   xoffset=-1   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1095    x=454  y=359  width=13   height=13   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1096    x=467  y=359  width=17   height=13   xoffset=-1   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1097    x=173  y=297  width=19   height=16   xoffset=-1   yoffset=4    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1098    x=484  y=359  width=15   height=13   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1099    x=0    y=373  width=15   height=13   xoffset=-1   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1100    x=499  y=359  width=12   height=13   xoffset=-1   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1101    x=277  y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1102    x=289  y=345  width=17   height=14   xoffset=-1   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1103    x=15   y=373  width=13   height=13   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1104    x=98   y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1105    x=112  y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1106    x=454  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1107    x=103  y=214  width=11   height=17   xoffset=-1   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1108    x=306  y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1109    x=318  y=345  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1110    x=503  y=180  width=7    height=17   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1111    x=114  y=214  width=9    height=17   xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1112    x=468  y=0    width=9    height=21   xoffset=-3   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1113    x=28   y=373  width=18   height=13   xoffset=-2   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1114    x=46   y=373  width=17   height=13   xoffset=-1   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1115    x=123  y=214  width=14   height=17   xoffset=-2   yoffset=0    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1116    x=137  y=214  width=13   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1117    x=150  y=214  width=12   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1118    x=477  y=0    width=14   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1119    x=192  y=297  width=12   height=16   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1120    x=162  y=214  width=20   height=17   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1121    x=330  y=345  width=16   height=14   xoffset=-2   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1122    x=204  y=297  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1123    x=220  y=297  width=14   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1124    x=182  y=214  width=17   height=17   xoffset=-1   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1125    x=346  y=345  width=14   height=14   xoffset=-1   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1126    x=234  y=297  width=19   height=16   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1127    x=63   y=373  width=16   height=13   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1128    x=253  y=297  width=23   height=16   xoffset=-1   yoffset=1    xadvance=20   page=0    chnl=0 \n"	\
                               "char id=1129    x=79   y=373  width=19   height=13   xoffset=-1   yoffset=4    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1130    x=276  y=297  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1131    x=98   y=373  width=14   height=13   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1132    x=292  y=297  width=20   height=16   xoffset=-1   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1133    x=112  y=373  width=17   height=13   xoffset=-1   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1134    x=38   y=0    width=13   height=24   xoffset=-2   yoffset=-3   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1135    x=491  y=0    width=12   height=21   xoffset=-2   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1136    x=312  y=297  width=20   height=16   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1137    x=0    y=26   width=20   height=21   xoffset=-2   yoffset=0    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1138    x=199  y=214  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1139    x=360  y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1140    x=332  y=297  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1141    x=129  y=373  width=14   height=13   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1142    x=215  y=106  width=16   height=19   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1143    x=215  y=214  width=14   height=17   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1144    x=445  y=47   width=21   height=20   xoffset=-2   yoffset=1    xadvance=17   page=0    chnl=0 \n"	\
                               "char id=1145    x=229  y=214  width=19   height=17   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1146    x=248  y=214  width=19   height=17   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1147    x=374  y=345  width=16   height=14   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1148    x=466  y=47   width=24   height=20   xoffset=-2   yoffset=-2   xadvance=20   page=0    chnl=0 \n"	\
                               "char id=1149    x=231  y=106  width=21   height=19   xoffset=-2   yoffset=-1   xadvance=17   page=0    chnl=0 \n"	\
                               "char id=1150    x=490  y=47   width=20   height=20   xoffset=-2   yoffset=-2   xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1151    x=267  y=214  width=16   height=17   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1152    x=0    y=67   width=14   height=20   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1153    x=283  y=214  width=12   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1154    x=143  y=373  width=12   height=13   xoffset=-2   yoffset=5    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1155    x=135  y=386  width=20   height=9    xoffset=-11  yoffset=-1   xadvance=0    page=0    chnl=0 \n"	\
                               "char id=1156    x=388  y=386  width=15   height=7    xoffset=-8   yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                               "char id=1157    x=247  y=386  width=12   height=8    xoffset=-8   yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                               "char id=1158    x=259  y=386  width=12   height=8    xoffset=-8   yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                               "char id=1159    x=271  y=386  width=28   height=8    xoffset=-14  yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                               "char id=1160    x=79   y=0    width=41   height=22   xoffset=-18  yoffset=-2   xadvance=6    page=0    chnl=0 \n"	\
                               "char id=1161    x=0    y=0    width=38   height=26   xoffset=-17  yoffset=-4   xadvance=6    page=0    chnl=0 \n"	\
                               "char id=1162    x=62   y=0    width=17   height=23   xoffset=-1   yoffset=-2   xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1163    x=20   y=26   width=15   height=21   xoffset=-1   yoffset=0    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1164    x=348  y=297  width=14   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1165    x=362  y=297  width=13   height=16   xoffset=-2   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1166    x=375  y=297  width=14   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1167    x=295  y=214  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1168    x=252  y=106  width=12   height=19   xoffset=-1   yoffset=-2   xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1169    x=389  y=297  width=11   height=16   xoffset=-1   yoffset=1    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1170    x=400  y=297  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1171    x=155  y=373  width=12   height=13   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1172    x=14   y=67   width=14   height=20   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1173    x=308  y=214  width=12   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1174    x=264  y=106  width=22   height=19   xoffset=-2   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1175    x=414  y=297  width=19   height=16   xoffset=-2   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1176    x=28   y=67   width=14   height=20   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1177    x=320  y=214  width=12   height=17   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1178    x=286  y=106  width=15   height=19   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1179    x=433  y=297  width=13   height=16   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1180    x=446  y=297  width=15   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1181    x=167  y=373  width=13   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1182    x=461  y=297  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1183    x=332  y=214  width=14   height=17   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1184    x=477  y=297  width=19   height=16   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1185    x=180  y=373  width=16   height=13   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1186    x=301  y=106  width=17   height=19   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1187    x=496  y=297  width=15   height=16   xoffset=-1   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1188    x=0    y=313  width=19   height=16   xoffset=-1   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1189    x=196  y=373  width=16   height=13   xoffset=-1   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1190    x=42   y=67   width=21   height=20   xoffset=-1   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1191    x=346  y=214  width=17   height=17   xoffset=-1   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1192    x=363  y=214  width=18   height=17   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1193    x=390  y=345  width=17   height=14   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1194    x=63   y=67   width=14   height=20   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1195    x=381  y=214  width=12   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1196    x=318  y=106  width=14   height=19   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1197    x=19   y=313  width=13   height=16   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1198    x=32   y=313  width=16   height=16   xoffset=-3   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1199    x=393  y=214  width=14   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1200    x=48   y=313  width=16   height=16   xoffset=-3   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1201    x=407  y=214  width=14   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1202    x=332  y=106  width=15   height=19   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1203    x=64   y=313  width=14   height=16   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1204    x=347  y=106  width=20   height=19   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1205    x=78   y=313  width=19   height=16   xoffset=-2   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1206    x=367  y=106  width=18   height=19   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1207    x=97   y=313  width=15   height=16   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1208    x=112  y=313  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1209    x=212  y=373  width=13   height=13   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1210    x=126  y=313  width=14   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1211    x=421  y=214  width=13   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1212    x=434  y=214  width=19   height=17   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1213    x=407  y=345  width=15   height=14   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1214    x=385  y=106  width=19   height=19   xoffset=-2   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1215    x=140  y=313  width=15   height=16   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1216    x=155  y=313  width=8    height=16   xoffset=-1   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1217    x=404  y=106  width=22   height=19   xoffset=-2   yoffset=-2   xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1218    x=453  y=214  width=19   height=17   xoffset=-2   yoffset=0    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1219    x=77   y=67   width=15   height=20   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1220    x=472  y=214  width=13   height=17   xoffset=-1   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1221    x=92   y=67   width=18   height=20   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1222    x=485  y=214  width=16   height=17   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1223    x=110  y=67   width=14   height=20   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1224    x=0    y=231  width=12   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1225    x=124  y=67   width=17   height=20   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1226    x=12   y=231  width=15   height=17   xoffset=-1   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1227    x=426  y=106  width=14   height=19   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1228    x=163  y=313  width=13   height=16   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1229    x=141  y=67   width=19   height=20   xoffset=-1   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1230    x=27   y=231  width=17   height=17   xoffset=-1   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1231    x=501  y=214  width=7    height=17   xoffset=-1   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=1232    x=440  y=106  width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1233    x=126  y=162  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1234    x=456  y=106  width=16   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1235    x=139  y=162  width=13   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1236    x=176  y=313  width=19   height=16   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1237    x=422  y=345  width=19   height=14   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1238    x=472  y=106  width=12   height=19   xoffset=-1   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1239    x=152  y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1240    x=44   y=231  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1241    x=441  y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1242    x=160  y=67   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1243    x=166  y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1244    x=484  y=106  width=22   height=19   xoffset=-2   yoffset=-2   xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1245    x=60   y=231  width=19   height=17   xoffset=-2   yoffset=0    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1246    x=176  y=67   width=14   height=20   xoffset=-2   yoffset=-2   xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1247    x=180  y=162  width=12   height=18   xoffset=-2   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1248    x=79   y=231  width=14   height=17   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1249    x=93   y=231  width=13   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1250    x=0    y=125  width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1251    x=106  y=231  width=12   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1252    x=14   y=125  width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1253    x=118  y=231  width=12   height=17   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1254    x=190  y=67   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1255    x=192  y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1256    x=130  y=231  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1257    x=455  y=345  width=14   height=14   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1258    x=206  y=67   width=16   height=20   xoffset=-2   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1259    x=206  y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1260    x=222  y=67   width=14   height=20   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1261    x=220  y=162  width=12   height=18   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1262    x=28   y=125  width=15   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1263    x=35   y=26   width=14   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1264    x=43   y=125  width=15   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1265    x=49   y=26   width=14   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1266    x=58   y=125  width=15   height=19   xoffset=-2   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1267    x=63   y=26   width=14   height=21   xoffset=-2   yoffset=0    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1268    x=73   y=125  width=14   height=19   xoffset=-1   yoffset=-2   xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1269    x=146  y=231  width=13   height=17   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1270    x=87   y=125  width=12   height=19   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1271    x=195  y=313  width=11   height=16   xoffset=-1   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1272    x=99   y=125  width=18   height=19   xoffset=-1   yoffset=-2   xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1273    x=159  y=231  width=15   height=17   xoffset=-1   yoffset=0    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1274    x=236  y=67   width=14   height=20   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1275    x=174  y=231  width=12   height=17   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1276    x=250  y=67   width=15   height=20   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1277    x=186  y=231  width=13   height=17   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1278    x=206  y=313  width=15   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1279    x=225  y=373  width=14   height=13   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1280    x=221  y=313  width=13   height=16   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1281    x=239  y=373  width=12   height=13   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1282    x=199  y=231  width=19   height=17   xoffset=-1   yoffset=1    xadvance=17   page=0    chnl=0 \n"	\
                               "char id=1283    x=469  y=345  width=17   height=14   xoffset=-2   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1284    x=218  y=231  width=18   height=17   xoffset=-1   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1285    x=486  y=345  width=16   height=14   xoffset=-1   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1286    x=265  y=67   width=15   height=20   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1287    x=236  y=231  width=13   height=17   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1288    x=249  y=231  width=21   height=17   xoffset=-2   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1289    x=0    y=359  width=18   height=14   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1290    x=270  y=231  width=21   height=17   xoffset=-1   yoffset=1    xadvance=19   page=0    chnl=0 \n"	\
                               "char id=1291    x=18   y=359  width=17   height=14   xoffset=-1   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1292    x=291  y=231  width=15   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1293    x=35   y=359  width=12   height=14   xoffset=-2   yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=1294    x=306  y=231  width=17   height=17   xoffset=-2   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1295    x=47   y=359  width=15   height=14   xoffset=-2   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1296    x=323  y=231  width=13   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1297    x=62   y=359  width=12   height=14   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=1298    x=280  y=67   width=15   height=20   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1299    x=336  y=231  width=14   height=17   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=1300    x=234  y=313  width=23   height=16   xoffset=-2   yoffset=1    xadvance=19   page=0    chnl=0 \n"	\
                               "char id=1301    x=251  y=373  width=20   height=13   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1302    x=257  y=313  width=19   height=16   xoffset=-1   yoffset=1    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1303    x=350  y=231  width=17   height=17   xoffset=-1   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1304    x=276  y=313  width=19   height=16   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1305    x=74   y=359  width=18   height=14   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1306    x=117  y=125  width=16   height=19   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1307    x=367  y=231  width=14   height=17   xoffset=-2   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1308    x=295  y=313  width=20   height=16   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=1309    x=271  y=373  width=17   height=13   xoffset=-2   yoffset=4    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=1310    x=315  y=313  width=15   height=16   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=1311    x=288  y=373  width=13   height=13   xoffset=-1   yoffset=4    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=1312    x=295  y=67   width=22   height=20   xoffset=-2   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1313    x=381  y=231  width=19   height=17   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1314    x=317  y=67   width=21   height=20   xoffset=-1   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=1315    x=400  y=231  width=17   height=17   xoffset=-1   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=1316    x=133  y=125  width=17   height=19   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=1317    x=330  y=313  width=15   height=16   xoffset=-1   yoffset=4    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=8192    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8193    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=8194    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8195    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=8196    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=8197    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=4    page=0    chnl=0 \n"	\
                               "char id=8198    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=2    page=0    chnl=0 \n"	\
                               "char id=8199    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8200    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=8201    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=3    page=0    chnl=0 \n"	\
                               "char id=8202    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=1    page=0    chnl=0 \n"	\
                               "char id=8203    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                               "char id=8204    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                               "char id=8210    x=403  y=386  width=14   height=7    xoffset=-2   yoffset=7    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8211    x=417  y=386  width=11   height=7    xoffset=-2   yoffset=7    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8212    x=428  y=386  width=18   height=7    xoffset=-2   yoffset=7    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=8213    x=446  y=386  width=19   height=7    xoffset=-2   yoffset=7    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=8214    x=77   y=26   width=9    height=21   xoffset=-1   yoffset=0    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8215    x=299  y=386  width=12   height=8    xoffset=-2   yoffset=13   xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8216    x=10   y=386  width=8    height=10   xoffset=-1   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=8217    x=18   y=386  width=8    height=10   xoffset=-2   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=8218    x=26   y=386  width=8    height=10   xoffset=-1   yoffset=9    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=8219    x=34   y=386  width=8    height=10   xoffset=-2   yoffset=1    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=8220    x=42   y=386  width=12   height=10   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=8221    x=54   y=386  width=11   height=10   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=8222    x=65   y=386  width=11   height=10   xoffset=-1   yoffset=9    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=8223    x=76   y=386  width=11   height=10   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=8224    x=232  y=162  width=11   height=18   xoffset=-2   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8225    x=243  y=162  width=11   height=18   xoffset=-2   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8226    x=87   y=386  width=10   height=10   xoffset=0    yoffset=4    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=8230    x=311  y=386  width=17   height=8    xoffset=-1   yoffset=9    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=8239    x=0    y=0    width=0    height=0    xoffset=-2   yoffset=0    xadvance=3    page=0    chnl=0 \n"	\
                               "char id=8240    x=417  y=231  width=25   height=17   xoffset=-2   yoffset=1    xadvance=21   page=0    chnl=0 \n"	\
                               "char id=8242    x=328  y=386  width=8    height=8    xoffset=-2   yoffset=1    xadvance=4    page=0    chnl=0 \n"	\
                               "char id=8243    x=336  y=386  width=11   height=8    xoffset=-2   yoffset=1    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=8244    x=347  y=386  width=13   height=8    xoffset=-2   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=8249    x=397  y=373  width=8    height=12   xoffset=-1   yoffset=4    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=8250    x=405  y=373  width=8    height=12   xoffset=-1   yoffset=4    xadvance=6    page=0    chnl=0 \n"	\
                               "char id=8252    x=345  y=313  width=11   height=16   xoffset=-1   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=8254    x=465  y=386  width=12   height=6    xoffset=-2   yoffset=1    xadvance=7    page=0    chnl=0 \n"	\
                               "char id=8260    x=442  y=231  width=18   height=17   xoffset=-5   yoffset=1    xadvance=2    page=0    chnl=0 \n"	\
                               "char id=8286    x=504  y=265  width=7    height=15   xoffset=-1   yoffset=2    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=8352    x=356  y=313  width=17   height=16   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=8353    x=254  y=162  width=14   height=18   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8354    x=460  y=231  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8355    x=373  y=313  width=13   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8356    x=386  y=313  width=13   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8357    x=474  y=231  width=17   height=17   xoffset=-1   yoffset=2    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=8358    x=399  y=313  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8359    x=0    y=248  width=25   height=17   xoffset=-1   yoffset=1    xadvance=22   page=0    chnl=0 \n"	\
                               "char id=8360    x=491  y=231  width=20   height=17   xoffset=-1   yoffset=1    xadvance=17   page=0    chnl=0 \n"	\
                               "char id=8361    x=413  y=313  width=20   height=16   xoffset=-2   yoffset=1    xadvance=16   page=0    chnl=0 \n"	\
                               "char id=8363    x=86   y=26   width=15   height=21   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8364    x=25   y=248  width=15   height=17   xoffset=-3   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8365    x=433  y=313  width=15   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8366    x=448  y=313  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8367    x=338  y=67   width=21   height=20   xoffset=-1   yoffset=1    xadvance=20   page=0    chnl=0 \n"	\
                               "char id=8368    x=40   y=248  width=14   height=17   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8369    x=462  y=313  width=15   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8370    x=150  y=125  width=14   height=19   xoffset=-2   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8371    x=477  y=313  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8372    x=54   y=248  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=8373    x=359  y=67   width=13   height=20   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8377    x=491  y=313  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8378    x=0    y=329  width=14   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=8482    x=155  y=386  width=14   height=9    xoffset=0    yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=8710    x=102  y=329  width=15   height=16   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=9167    x=106  y=359  width=16   height=14   xoffset=-1   yoffset=3    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=9632    x=128  y=248  width=16   height=17   xoffset=-1   yoffset=2    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=9633    x=176  y=248  width=16   height=17   xoffset=-1   yoffset=2    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=9635    x=192  y=248  width=16   height=17   xoffset=-1   yoffset=2    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=9650    x=160  y=248  width=16   height=17   xoffset=-2   yoffset=2    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=9654    x=96   y=248  width=16   height=17   xoffset=-2   yoffset=2    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=9660    x=144  y=248  width=16   height=17   xoffset=-2   yoffset=2    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=9664    x=112  y=248  width=16   height=17   xoffset=-2   yoffset=2    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=9733    x=208  y=248  width=16   height=17   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=9734    x=224  y=248  width=16   height=17   xoffset=-2   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=9744    x=161  y=329  width=15   height=16   xoffset=-1   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=9745    x=176  y=329  width=15   height=16   xoffset=-1   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=9746    x=191  y=329  width=15   height=16   xoffset=-1   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=9775    x=133  y=329  width=15   height=16   xoffset=-1   yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=9785    x=230  y=125  width=17   height=19   xoffset=-1   yoffset=0    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=9786    x=213  y=125  width=17   height=19   xoffset=-1   yoffset=0    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=9873    x=148  y=329  width=13   height=16   xoffset=0    yoffset=1    xadvance=13   page=0    chnl=0 \n"	\
                               "char id=9986    x=413  y=373  width=16   height=12   xoffset=-2   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=9992    x=117  y=329  width=16   height=16   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=9997    x=122  y=359  width=16   height=14   xoffset=-2   yoffset=2    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=9998    x=276  y=329  width=14   height=15   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=10003   x=138  y=359  width=12   height=14   xoffset=0    yoffset=2    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=10007   x=240  y=248  width=14   height=17   xoffset=-1   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=11360   x=14   y=329  width=13   height=16   xoffset=-2   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=11361   x=70   y=248  width=10   height=17   xoffset=-2   yoffset=0    xadvance=5    page=0    chnl=0 \n"	\
                               "char id=11362   x=27   y=329  width=14   height=16   xoffset=-3   yoffset=1    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=11363   x=41   y=329  width=14   height=16   xoffset=-2   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=11364   x=372  y=67   width=14   height=20   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=11365   x=262  y=329  width=14   height=15   xoffset=-2   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=11366   x=386  y=67   width=11   height=20   xoffset=-2   yoffset=-1   xadvance=7    page=0    chnl=0 \n"	\
                               "char id=11367   x=164  y=125  width=17   height=19   xoffset=-1   yoffset=1    xadvance=14   page=0    chnl=0 \n"	\
                               "char id=11368   x=397  y=67   width=16   height=20   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=11369   x=181  y=125  width=15   height=19   xoffset=-1   yoffset=1    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=11370   x=413  y=67   width=13   height=20   xoffset=-1   yoffset=0    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=11371   x=196  y=125  width=17   height=19   xoffset=-2   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=11372   x=55   y=329  width=13   height=16   xoffset=-2   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=11373   x=80   y=248  width=16   height=17   xoffset=-2   yoffset=1    xadvance=12   page=0    chnl=0 \n"	\
                               "char id=11377   x=301  y=373  width=16   height=13   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "char id=11378   x=68   y=329  width=22   height=16   xoffset=-2   yoffset=1    xadvance=18   page=0    chnl=0 \n"	\
                               "char id=11379   x=317  y=373  width=20   height=13   xoffset=-2   yoffset=4    xadvance=15   page=0    chnl=0 \n"	\
                               "char id=11380   x=92   y=359  width=14   height=14   xoffset=-2   yoffset=3    xadvance=9    page=0    chnl=0 \n"	\
                               "char id=11381   x=90   y=329  width=12   height=16   xoffset=-1   yoffset=1    xadvance=10   page=0    chnl=0 \n"	\
                               "char id=11382   x=337  y=373  width=10   height=13   xoffset=-1   yoffset=4    xadvance=8    page=0    chnl=0 \n"	\
                               "char id=11383   x=347  y=373  width=15   height=13   xoffset=-2   yoffset=4    xadvance=11   page=0    chnl=0 \n"	\
                               "kernings count=875\n"	\
                               "kerning first=121 second=46 amount=-1\n"	\
                               "kerning first=8220 second=65 amount=-2\n"	\
                               "kerning first=194 second=121 amount=-1\n"	\
                               "kerning first=87 second=234 amount=-1\n"	\
                               "kerning first=221 second=79 amount=-1\n"	\
                               "kerning first=86 second=233 amount=-1\n"	\
                               "kerning first=84 second=45 amount=-2\n"	\
                               "kerning first=89 second=195 amount=-2\n"	\
                               "kerning first=67 second=8217 amount=1\n"	\
                               "kerning first=356 second=44 amount=-2\n"	\
                               "kerning first=88 second=79 amount=-1\n"	\
                               "kerning first=87 second=193 amount=-1\n"	\
                               "kerning first=70 second=58 amount=-1\n"	\
                               "kerning first=70 second=59 amount=-1\n"	\
                               "kerning first=221 second=65 amount=-2\n"	\
                               "kerning first=221 second=97 amount=-1\n"	\
                               "kerning first=75 second=255 amount=-1\n"	\
                               "kerning first=76 second=79 amount=-1\n"	\
                               "kerning first=114 second=8217 amount=1\n"	\
                               "kerning first=256 second=84 amount=-1\n"	\
                               "kerning first=76 second=85 amount=-1\n"	\
                               "kerning first=193 second=86 amount=-1\n"	\
                               "kerning first=119 second=44 amount=-1\n"	\
                               "kerning first=45 second=88 amount=-1\n"	\
                               "kerning first=8216 second=89 amount=1\n"	\
                               "kerning first=313 second=121 amount=-1\n"	\
                               "kerning first=376 second=97 amount=-1\n"	\
                               "kerning first=84 second=99 amount=-2\n"	\
                               "kerning first=86 second=8222 amount=-1\n"	\
                               "kerning first=356 second=235 amount=-2\n"	\
                               "kerning first=86 second=58 amount=-1\n"	\
                               "kerning first=320 second=108 amount=-2\n"	\
                               "kerning first=8220 second=195 amount=-2\n"	\
                               "kerning first=376 second=101 amount=-1\n"	\
                               "kerning first=70 second=114 amount=-1\n"	\
                               "kerning first=84 second=115 amount=-2\n"	\
                               "kerning first=70 second=117 amount=-1\n"	\
                               "kerning first=196 second=118 amount=-1\n"	\
                               "kerning first=84 second=119 amount=-2\n"	\
                               "kerning first=70 second=250 amount=-1\n"	\
                               "kerning first=196 second=121 amount=-1\n"	\
                               "kerning first=75 second=214 amount=-1\n"	\
                               "kerning first=192 second=253 amount=-1\n"	\
                               "kerning first=88 second=213 amount=-1\n"	\
                               "kerning first=193 second=356 amount=-1\n"	\
                               "kerning first=216 second=88 amount=-1\n"	\
                               "kerning first=82 second=84 amount=-1\n"	\
                               "kerning first=356 second=229 amount=-2\n"	\
                               "kerning first=376 second=227 amount=-1\n"	\
                               "kerning first=221 second=46 amount=-3\n"	\
                               "kerning first=260 second=372 amount=-1\n"	\
                               "kerning first=75 second=121 amount=-1\n"	\
                               "kerning first=376 second=339 amount=-1\n"	\
                               "kerning first=376 second=193 amount=-2\n"	\
                               "kerning first=356 second=194 amount=-1\n"	\
                               "kerning first=70 second=195 amount=-2\n"	\
                               "kerning first=89 second=196 amount=-2\n"	\
                               "kerning first=75 second=199 amount=-1\n"	\
                               "kerning first=87 second=283 amount=-1\n"	\
                               "kerning first=195 second=374 amount=-2\n"	\
                               "kerning first=84 second=243 amount=-2\n"	\
                               "kerning first=356 second=46 amount=-2\n"	\
                               "kerning first=84 second=246 amount=-1\n"	\
                               "kerning first=313 second=211 amount=-1\n"	\
                               "kerning first=89 second=58 amount=-1\n"	\
                               "kerning first=313 second=214 amount=-1\n"	\
                               "kerning first=356 second=367 amount=-2\n"	\
                               "kerning first=317 second=216 amount=-1\n"	\
                               "kerning first=76 second=217 amount=-1\n"	\
                               "kerning first=313 second=218 amount=-1\n"	\
                               "kerning first=321 second=219 amount=-1\n"	\
                               "kerning first=212 second=221 amount=-1\n"	\
                               "kerning first=317 second=210 amount=-1\n"	\
                               "kerning first=344 second=221 amount=-1\n"	\
                               "kerning first=87 second=224 amount=-1\n"	\
                               "kerning first=70 second=225 amount=-1\n"	\
                               "kerning first=356 second=226 amount=-2\n"	\
                               "kerning first=221 second=227 amount=-1\n"	\
                               "kerning first=356 second=232 amount=-2\n"	\
                               "kerning first=221 second=229 amount=-1\n"	\
                               "kerning first=70 second=230 amount=-1\n"	\
                               "kerning first=84 second=231 amount=-2\n"	\
                               "kerning first=89 second=232 amount=-1\n"	\
                               "kerning first=84 second=234 amount=-2\n"	\
                               "kerning first=70 second=235 amount=-1\n"	\
                               "kerning first=195 second=89 amount=-2\n"	\
                               "kerning first=87 second=242 amount=-1\n"	\
                               "kerning first=84 second=242 amount=-1\n"	\
                               "kerning first=356 second=243 amount=-2\n"	\
                               "kerning first=356 second=245 amount=-2\n"	\
                               "kerning first=70 second=246 amount=-1\n"	\
                               "kerning first=87 second=97 amount=-1\n"	\
                               "kerning first=70 second=248 amount=-1\n"	\
                               "kerning first=84 second=249 amount=-2\n"	\
                               "kerning first=356 second=341 amount=-2\n"	\
                               "kerning first=356 second=251 amount=-2\n"	\
                               "kerning first=70 second=252 amount=-1\n"	\
                               "kerning first=317 second=253 amount=-1\n"	\
                               "kerning first=344 second=255 amount=-1\n"	\
                               "kerning first=86 second=339 amount=-1\n"	\
                               "kerning first=87 second=196 amount=-1\n"	\
                               "kerning first=70 second=258 amount=-2\n"	\
                               "kerning first=70 second=259 amount=-1\n"	\
                               "kerning first=70 second=260 amount=-2\n"	\
                               "kerning first=70 second=261 amount=-1\n"	\
                               "kerning first=86 second=246 amount=-1\n"	\
                               "kerning first=75 second=268 amount=-1\n"	\
                               "kerning first=84 second=269 amount=-2\n"	\
                               "kerning first=321 second=255 amount=-1\n"	\
                               "kerning first=8222 second=87 amount=-2\n"	\
                               "kerning first=356 second=117 amount=-2\n"	\
                               "kerning first=70 second=275 amount=-1\n"	\
                               "kerning first=70 second=277 amount=-1\n"	\
                               "kerning first=70 second=279 amount=-1\n"	\
                               "kerning first=192 second=121 amount=-1\n"	\
                               "kerning first=70 second=281 amount=-1\n"	\
                               "kerning first=86 second=244 amount=-1\n"	\
                               "kerning first=84 second=283 amount=-2\n"	\
                               "kerning first=376 second=249 amount=-1\n"	\
                               "kerning first=260 second=375 amount=-1\n"	\
                               "kerning first=8218 second=84 amount=-3\n"	\
                               "kerning first=76 second=220 amount=-1\n"	\
                               "kerning first=118 second=46 amount=-1\n"	\
                               "kerning first=88 second=268 amount=-1\n"	\
                               "kerning first=65 second=89 amount=-2\n"	\
                               "kerning first=86 second=195 amount=-1\n"	\
                               "kerning first=84 second=8222 amount=-2\n"	\
                               "kerning first=376 second=224 amount=-1\n"	\
                               "kerning first=194 second=8222 amount=1\n"	\
                               "kerning first=86 second=252 amount=-1\n"	\
                               "kerning first=75 second=211 amount=-1\n"	\
                               "kerning first=211 second=376 amount=-1\n"	\
                               "kerning first=376 second=79 amount=-1\n"	\
                               "kerning first=193 second=8221 amount=-1\n"	\
                               "kerning first=256 second=374 amount=-2\n"	\
                               "kerning first=344 second=89 amount=-1\n"	\
                               "kerning first=102 second=44 amount=-1\n"	\
                               "kerning first=313 second=356 amount=-3\n"	\
                               "kerning first=221 second=224 amount=-1\n"	\
                               "kerning first=70 second=333 amount=-1\n"	\
                               "kerning first=70 second=335 amount=-1\n"	\
                               "kerning first=70 second=337 amount=-1\n"	\
                               "kerning first=75 second=338 amount=-1\n"	\
                               "kerning first=84 second=339 amount=-1\n"	\
                               "kerning first=70 second=341 amount=-1\n"	\
                               "kerning first=70 second=343 amount=-1\n"	\
                               "kerning first=70 second=345 amount=-1\n"	\
                               "kerning first=83 second=350 amount=-1\n"	\
                               "kerning first=258 second=121 amount=-1\n"	\
                               "kerning first=83 second=352 amount=-1\n"	\
                               "kerning first=84 second=353 amount=-2\n"	\
                               "kerning first=65 second=354 amount=-1\n"	\
                               "kerning first=76 second=86 amount=-2\n"	\
                               "kerning first=89 second=250 amount=-1\n"	\
                               "kerning first=70 second=361 amount=-1\n"	\
                               "kerning first=70 second=363 amount=-1\n"	\
                               "kerning first=102 second=8221 amount=1\n"	\
                               "kerning first=70 second=365 amount=-1\n"	\
                               "kerning first=313 second=366 amount=-1\n"	\
                               "kerning first=376 second=367 amount=-1\n"	\
                               "kerning first=258 second=376 amount=-2\n"	\
                               "kerning first=70 second=369 amount=-1\n"	\
                               "kerning first=344 second=376 amount=-1\n"	\
                               "kerning first=70 second=371 amount=-1\n"	\
                               "kerning first=194 second=372 amount=-1\n"	\
                               "kerning first=65 second=374 amount=-2\n"	\
                               "kerning first=65 second=375 amount=-1\n"	\
                               "kerning first=214 second=376 amount=-1\n"	\
                               "kerning first=8220 second=194 amount=-2\n"	\
                               "kerning first=70 second=193 amount=-2\n"	\
                               "kerning first=256 second=221 amount=-2\n"	\
                               "kerning first=79 second=221 amount=-1\n"	\
                               "kerning first=195 second=221 amount=-2\n"	\
                               "kerning first=80 second=196 amount=-1\n"	\
                               "kerning first=256 second=253 amount=-1\n"	\
                               "kerning first=66 second=372 amount=-1\n"	\
                               "kerning first=313 second=8217 amount=-4\n"	\
                               "kerning first=321 second=217 amount=-1\n"	\
                               "kerning first=88 second=199 amount=-1\n"	\
                               "kerning first=199 second=8217 amount=1\n"	\
                               "kerning first=89 second=192 amount=-2\n"	\
                               "kerning first=341 second=46 amount=-2\n"	\
                               "kerning first=258 second=354 amount=-1\n"	\
                               "kerning first=65 second=86 amount=-1\n"	\
                               "kerning first=221 second=211 amount=-1\n"	\
                               "kerning first=86 second=192 amount=-1\n"	\
                               "kerning first=317 second=221 amount=-2\n"	\
                               "kerning first=89 second=212 amount=-1\n"	\
                               "kerning first=211 second=89 amount=-1\n"	\
                               "kerning first=258 second=84 amount=-1\n"	\
                               "kerning first=84 second=351 amount=-2\n"	\
                               "kerning first=221 second=268 amount=-1\n"	\
                               "kerning first=213 second=88 amount=-1\n"	\
                               "kerning first=89 second=367 amount=-1\n"	\
                               "kerning first=356 second=114 amount=-2\n"	\
                               "kerning first=321 second=356 amount=-3\n"	\
                               "kerning first=313 second=376 amount=-2\n"	\
                               "kerning first=196 second=372 amount=-1\n"	\
                               "kerning first=214 second=89 amount=-1\n"	\
                               "kerning first=8216 second=376 amount=1\n"	\
                               "kerning first=270 second=374 amount=-1\n"	\
                               "kerning first=80 second=44 amount=-3\n"	\
                               "kerning first=70 second=283 amount=-1\n"	\
                               "kerning first=76 second=253 amount=-1\n"	\
                               "kerning first=376 second=246 amount=-1\n"	\
                               "kerning first=89 second=45 amount=-2\n"	\
                               "kerning first=70 second=228 amount=-1\n"	\
                               "kerning first=321 second=221 amount=-2\n"	\
                               "kerning first=70 second=121 amount=-1\n"	\
                               "kerning first=260 second=8222 amount=1\n"	\
                               "kerning first=376 second=235 amount=-1\n"	\
                               "kerning first=70 second=97 amount=-1\n"	\
                               "kerning first=260 second=255 amount=-1\n"	\
                               "kerning first=321 second=211 amount=-1\n"	\
                               "kerning first=317 second=213 amount=-1\n"	\
                               "kerning first=8222 second=86 amount=-2\n"	\
                               "kerning first=344 second=121 amount=-1\n"	\
                               "kerning first=221 second=214 amount=-1\n"	\
                               "kerning first=272 second=374 amount=-1\n"	\
                               "kerning first=376 second=65 amount=-2\n"	\
                               "kerning first=194 second=255 amount=-1\n"	\
                               "kerning first=84 second=196 amount=-1\n"	\
                               "kerning first=8222 second=89 amount=-3\n"	\
                               "kerning first=196 second=375 amount=-1\n"	\
                               "kerning first=86 second=250 amount=-1\n"	\
                               "kerning first=260 second=8217 amount=-1\n"	\
                               "kerning first=356 second=111 amount=-2\n"	\
                               "kerning first=68 second=374 amount=-1\n"	\
                               "kerning first=376 second=251 amount=-1\n"	\
                               "kerning first=345 second=46 amount=-2\n"	\
                               "kerning first=376 second=243 amount=-1\n"	\
                               "kerning first=65 second=253 amount=-1\n"	\
                               "kerning first=8220 second=192 amount=-2\n"	\
                               "kerning first=84 second=97 amount=-2\n"	\
                               "kerning first=221 second=235 amount=-1\n"	\
                               "kerning first=313 second=85 amount=-1\n"	\
                               "kerning first=262 second=8221 amount=1\n"	\
                               "kerning first=87 second=245 amount=-1\n"	\
                               "kerning first=317 second=366 amount=-1\n"	\
                               "kerning first=350 second=83 amount=-1\n"	\
                               "kerning first=321 second=216 amount=-1\n"	\
                               "kerning first=192 second=86 amount=-1\n"	\
                               "kerning first=70 second=233 amount=-1\n"	\
                               "kerning first=68 second=221 amount=-1\n"	\
                               "kerning first=193 second=8218 amount=1\n"	\
                               "kerning first=356 second=193 amount=-1\n"	\
                               "kerning first=89 second=226 amount=-1\n"	\
                               "kerning first=221 second=243 amount=-1\n"	\
                               "kerning first=221 second=228 amount=-1\n"	\
                               "kerning first=84 second=226 amount=-1\n"	\
                               "kerning first=221 second=251 amount=-1\n"	\
                               "kerning first=84 second=58 amount=-1\n"	\
                               "kerning first=376 second=199 amount=-1\n"	\
                               "kerning first=84 second=229 amount=-1\n"	\
                               "kerning first=221 second=210 amount=-1\n"	\
                               "kerning first=75 second=45 amount=-1\n"	\
                               "kerning first=80 second=195 amount=-1\n"	\
                               "kerning first=344 second=356 amount=-1\n"	\
                               "kerning first=192 second=255 amount=-1\n"	\
                               "kerning first=84 second=244 amount=-1\n"	\
                               "kerning first=84 second=235 amount=-2\n"	\
                               "kerning first=272 second=89 amount=-1\n"	\
                               "kerning first=212 second=376 amount=-1\n"	\
                               "kerning first=221 second=225 amount=-1\n"	\
                               "kerning first=8216 second=74 amount=-1\n"	\
                               "kerning first=84 second=255 amount=-2\n"	\
                               "kerning first=356 second=253 amount=-2\n"	\
                               "kerning first=195 second=376 amount=-2\n"	\
                               "kerning first=376 second=210 amount=-1\n"	\
                               "kerning first=84 second=367 amount=-2\n"	\
                               "kerning first=86 second=235 amount=-1\n"	\
                               "kerning first=70 second=192 amount=-2\n"	\
                               "kerning first=341 second=44 amount=-2\n"	\
                               "kerning first=70 second=65 amount=-2\n"	\
                               "kerning first=221 second=199 amount=-1\n"	\
                               "kerning first=89 second=97 amount=-1\n"	\
                               "kerning first=210 second=221 amount=-1\n"	\
                               "kerning first=84 second=233 amount=-2\n"	\
                               "kerning first=88 second=211 amount=-1\n"	\
                               "kerning first=84 second=8218 amount=-2\n"	\
                               "kerning first=317 second=79 amount=-1\n"	\
                               "kerning first=221 second=195 amount=-2\n"	\
                               "kerning first=82 second=376 amount=-1\n"	\
                               "kerning first=356 second=224 amount=-2\n"	\
                               "kerning first=212 second=89 amount=-1\n"	\
                               "kerning first=8222 second=84 amount=-3\n"	\
                               "kerning first=8218 second=87 amount=-2\n"	\
                               "kerning first=76 second=216 amount=-1\n"	\
                               "kerning first=196 second=374 amount=-2\n"	\
                               "kerning first=84 second=345 amount=-2\n"	\
                               "kerning first=313 second=86 amount=-2\n"	\
                               "kerning first=86 second=224 amount=-1\n"	\
                               "kerning first=321 second=85 amount=-1\n"	\
                               "kerning first=89 second=46 amount=-3\n"	\
                               "kerning first=196 second=221 amount=-2\n"	\
                               "kerning first=270 second=376 amount=-1\n"	\
                               "kerning first=221 second=246 amount=-1\n"	\
                               "kerning first=87 second=228 amount=-1\n"	\
                               "kerning first=193 second=375 amount=-1\n"	\
                               "kerning first=76 second=84 amount=-3\n"	\
                               "kerning first=376 second=225 amount=-1\n"	\
                               "kerning first=84 second=121 amount=-2\n"	\
                               "kerning first=356 second=242 amount=-2\n"	\
                               "kerning first=89 second=251 amount=-1\n"	\
                               "kerning first=194 second=84 amount=-1\n"	\
                               "kerning first=260 second=221 amount=-2\n"	\
                               "kerning first=89 second=59 amount=-1\n"	\
                               "kerning first=272 second=221 amount=-1\n"	\
                               "kerning first=70 second=44 amount=-3\n"	\
                               "kerning first=87 second=232 amount=-1\n"	\
                               "kerning first=87 second=244 amount=-1\n"	\
                               "kerning first=80 second=46 amount=-3\n"	\
                               "kerning first=376 second=248 amount=-1\n"	\
                               "kerning first=260 second=356 amount=-1\n"	\
                               "kerning first=76 second=121 amount=-1\n"	\
                               "kerning first=313 second=219 amount=-1\n"	\
                               "kerning first=70 second=256 amount=-2\n"	\
                               "kerning first=89 second=101 amount=-1\n"	\
                               "kerning first=356 second=263 amount=-2\n"	\
                               "kerning first=8222 second=356 amount=-3\n"	\
                               "kerning first=317 second=376 amount=-2\n"	\
                               "kerning first=221 second=232 amount=-1\n"	\
                               "kerning first=70 second=255 amount=-1\n"	\
                               "kerning first=70 second=244 amount=-1\n"	\
                               "kerning first=258 second=118 amount=-1\n"	\
                               "kerning first=258 second=372 amount=-1\n"	\
                               "kerning first=88 second=338 amount=-1\n"	\
                               "kerning first=89 second=283 amount=-1\n"	\
                               "kerning first=208 second=374 amount=-1\n"	\
                               "kerning first=268 second=8221 amount=1\n"	\
                               "kerning first=196 second=8221 amount=-1\n"	\
                               "kerning first=352 second=350 amount=-1\n"	\
                               "kerning first=321 second=86 amount=-2\n"	\
                               "kerning first=196 second=8222 amount=1\n"	\
                               "kerning first=376 second=8222 amount=-2\n"	\
                               "kerning first=8216 second=192 amount=-2\n"	\
                               "kerning first=76 second=366 amount=-1\n"	\
                               "kerning first=376 second=195 amount=-2\n"	\
                               "kerning first=8218 second=356 amount=-3\n"	\
                               "kerning first=260 second=118 amount=-1\n"	\
                               "kerning first=376 second=67 amount=-1\n"	\
                               "kerning first=270 second=221 amount=-1\n"	\
                               "kerning first=196 second=87 amount=-1\n"	\
                               "kerning first=210 second=88 amount=-1\n"	\
                               "kerning first=221 second=262 amount=-1\n"	\
                               "kerning first=344 second=253 amount=-1\n"	\
                               "kerning first=192 second=354 amount=-1\n"	\
                               "kerning first=70 second=111 amount=-1\n"	\
                               "kerning first=356 second=353 amount=-2\n"	\
                               "kerning first=102 second=8217 amount=1\n"	\
                               "kerning first=194 second=87 amount=-1\n"	\
                               "kerning first=376 second=46 amount=-3\n"	\
                               "kerning first=87 second=233 amount=-1\n"	\
                               "kerning first=88 second=210 amount=-1\n"	\
                               "kerning first=89 second=245 amount=-1\n"	\
                               "kerning first=70 second=224 amount=-1\n"	\
                               "kerning first=86 second=45 amount=-1\n"	\
                               "kerning first=82 second=253 amount=-1\n"	\
                               "kerning first=80 second=8222 amount=-3\n"	\
                               "kerning first=221 second=8222 amount=-2\n"	\
                               "kerning first=192 second=84 amount=-1\n"	\
                               "kerning first=79 second=376 amount=-1\n"	\
                               "kerning first=193 second=8217 amount=-1\n"	\
                               "kerning first=86 second=8218 amount=-2\n"	\
                               "kerning first=86 second=194 amount=-1\n"	\
                               "kerning first=260 second=8221 amount=-1\n"	\
                               "kerning first=256 second=8222 amount=1\n"	\
                               "kerning first=68 second=89 amount=-1\n"	\
                               "kerning first=193 second=374 amount=-2\n"	\
                               "kerning first=317 second=89 amount=-2\n"	\
                               "kerning first=65 second=84 amount=-1\n"	\
                               "kerning first=356 second=196 amount=-1\n"	\
                               "kerning first=89 second=224 amount=-1\n"	\
                               "kerning first=84 second=59 amount=-1\n"	\
                               "kerning first=65 second=255 amount=-1\n"	\
                               "kerning first=87 second=192 amount=-1\n"	\
                               "kerning first=321 second=210 amount=-1\n"	\
                               "kerning first=195 second=121 amount=-1\n"	\
                               "kerning first=221 second=194 amount=-2\n"	\
                               "kerning first=268 second=8217 amount=1\n"	\
                               "kerning first=356 second=255 amount=-2\n"	\
                               "kerning first=102 second=46 amount=-1\n"	\
                               "kerning first=221 second=252 amount=-1\n"	\
                               "kerning first=70 second=245 amount=-1\n"	\
                               "kerning first=89 second=244 amount=-1\n"	\
                               "kerning first=8220 second=196 amount=-2\n"	\
                               "kerning first=212 second=88 amount=-1\n"	\
                               "kerning first=193 second=221 amount=-2\n"	\
                               "kerning first=84 second=117 amount=-2\n"	\
                               "kerning first=376 second=268 amount=-1\n"	\
                               "kerning first=194 second=118 amount=-1\n"	\
                               "kerning first=76 second=255 amount=-1\n"	\
                               "kerning first=193 second=253 amount=-1\n"	\
                               "kerning first=86 second=230 amount=-1\n"	\
                               "kerning first=84 second=111 amount=-2\n"	\
                               "kerning first=84 second=228 amount=-1\n"	\
                               "kerning first=356 second=195 amount=-1\n"	\
                               "kerning first=356 second=248 amount=-1\n"	\
                               "kerning first=195 second=354 amount=-1\n"	\
                               "kerning first=313 second=213 amount=-1\n"	\
                               "kerning first=317 second=338 amount=-1\n"	\
                               "kerning first=70 second=234 amount=-1\n"	\
                               "kerning first=89 second=246 amount=-1\n"	\
                               "kerning first=376 second=252 amount=-1\n"	\
                               "kerning first=352 second=83 amount=-1\n"	\
                               "kerning first=75 second=262 amount=-1\n"	\
                               "kerning first=65 second=8218 amount=1\n"	\
                               "kerning first=356 second=269 amount=-2\n"	\
                               "kerning first=65 second=376 amount=-2\n"	\
                               "kerning first=45 second=86 amount=-1\n"	\
                               "kerning first=76 second=218 amount=-1\n"	\
                               "kerning first=79 second=89 amount=-1\n"	\
                               "kerning first=256 second=89 amount=-2\n"	\
                               "kerning first=321 second=8221 amount=-3\n"	\
                               "kerning first=86 second=251 amount=-1\n"	\
                               "kerning first=45 second=356 amount=-2\n"	\
                               "kerning first=317 second=8221 amount=-4\n"	\
                               "kerning first=317 second=212 amount=-1\n"	\
                               "kerning first=76 second=376 amount=-2\n"	\
                               "kerning first=256 second=354 amount=-1\n"	\
                               "kerning first=221 second=245 amount=-1\n"	\
                               "kerning first=356 second=115 amount=-2\n"	\
                               "kerning first=88 second=216 amount=-1\n"	\
                               "kerning first=89 second=44 amount=-3\n"	\
                               "kerning first=194 second=86 amount=-1\n"	\
                               "kerning first=376 second=59 amount=-1\n"	\
                               "kerning first=70 second=229 amount=-1\n"	\
                               "kerning first=86 second=97 amount=-1\n"	\
                               "kerning first=221 second=45 amount=-2\n"	\
                               "kerning first=89 second=214 amount=-1\n"	\
                               "kerning first=70 second=101 amount=-1\n"	\
                               "kerning first=89 second=8222 amount=-2\n"	\
                               "kerning first=255 second=44 amount=-1\n"	\
                               "kerning first=114 second=44 amount=-2\n"	\
                               "kerning first=66 second=221 amount=-1\n"	\
                               "kerning first=8216 second=198 amount=-2\n"	\
                               "kerning first=317 second=121 amount=-1\n"	\
                               "kerning first=195 second=8218 amount=1\n"	\
                               "kerning first=321 second=87 amount=-1\n"	\
                               "kerning first=89 second=233 amount=-1\n"	\
                               "kerning first=192 second=376 amount=-2\n"	\
                               "kerning first=270 second=89 amount=-1\n"	\
                               "kerning first=84 second=192 amount=-1\n"	\
                               "kerning first=88 second=67 amount=-1\n"	\
                               "kerning first=89 second=193 amount=-2\n"	\
                               "kerning first=272 second=376 amount=-1\n"	\
                               "kerning first=89 second=235 amount=-1\n"	\
                               "kerning first=317 second=84 amount=-3\n"	\
                               "kerning first=199 second=8221 amount=1\n"	\
                               "kerning first=321 second=214 amount=-1\n"	\
                               "kerning first=87 second=44 amount=-1\n"	\
                               "kerning first=356 second=65 amount=-1\n"	\
                               "kerning first=317 second=217 amount=-1\n"	\
                               "kerning first=256 second=121 amount=-1\n"	\
                               "kerning first=266 second=8217 amount=1\n"	\
                               "kerning first=89 second=243 amount=-1\n"	\
                               "kerning first=86 second=283 amount=-1\n"	\
                               "kerning first=221 second=226 amount=-1\n"	\
                               "kerning first=89 second=65 amount=-2\n"	\
                               "kerning first=86 second=225 amount=-1\n"	\
                               "kerning first=260 second=87 amount=-1\n"	\
                               "kerning first=256 second=8218 amount=1\n"	\
                               "kerning first=376 second=226 amount=-1\n"	\
                               "kerning first=321 second=220 amount=-1\n"	\
                               "kerning first=87 second=111 amount=-1\n"	\
                               "kerning first=321 second=213 amount=-1\n"	\
                               "kerning first=66 second=86 amount=-1\n"	\
                               "kerning first=70 second=194 amount=-2\n"	\
                               "kerning first=356 second=121 amount=-2\n"	\
                               "kerning first=8218 second=89 amount=-3\n"	\
                               "kerning first=89 second=210 amount=-1\n"	\
                               "kerning first=262 second=8217 amount=1\n"	\
                               "kerning first=89 second=225 amount=-1\n"	\
                               "kerning first=317 second=356 amount=-3\n"	\
                               "kerning first=89 second=228 amount=-1\n"	\
                               "kerning first=356 second=8218 amount=-2\n"	\
                               "kerning first=210 second=89 amount=-1\n"	\
                               "kerning first=258 second=86 amount=-1\n"	\
                               "kerning first=356 second=225 amount=-2\n"	\
                               "kerning first=376 second=242 amount=-1\n"	\
                               "kerning first=86 second=44 amount=-2\n"	\
                               "kerning first=87 second=243 amount=-1\n"	\
                               "kerning first=84 second=101 amount=-2\n"	\
                               "kerning first=313 second=210 amount=-1\n"	\
                               "kerning first=84 second=227 amount=-1\n"	\
                               "kerning first=82 second=89 amount=-1\n"	\
                               "kerning first=192 second=356 amount=-1\n"	\
                               "kerning first=196 second=354 amount=-1\n"	\
                               "kerning first=376 second=212 amount=-1\n"	\
                               "kerning first=195 second=375 amount=-1\n"	\
                               "kerning first=89 second=199 amount=-1\n"	\
                               "kerning first=194 second=356 amount=-1\n"	\
                               "kerning first=82 second=121 amount=-1\n"	\
                               "kerning first=376 second=45 amount=-2\n"	\
                               "kerning first=260 second=89 amount=-2\n"	\
                               "kerning first=89 second=252 amount=-1\n"	\
                               "kerning first=196 second=376 amount=-2\n"	\
                               "kerning first=66 second=87 amount=-1\n"	\
                               "kerning first=76 second=338 amount=-1\n"	\
                               "kerning first=340 second=376 amount=-1\n"	\
                               "kerning first=317 second=214 amount=-1\n"	\
                               "kerning first=194 second=221 amount=-2\n"	\
                               "kerning first=196 second=86 amount=-1\n"	\
                               "kerning first=221 second=234 amount=-1\n"	\
                               "kerning first=253 second=44 amount=-1\n"	\
                               "kerning first=313 second=221 amount=-2\n"	\
                               "kerning first=313 second=84 amount=-3\n"	\
                               "kerning first=356 second=99 amount=-2\n"	\
                               "kerning first=266 second=8221 amount=1\n"	\
                               "kerning first=194 second=8221 amount=-1\n"	\
                               "kerning first=89 second=234 amount=-1\n"	\
                               "kerning first=87 second=230 amount=-1\n"	\
                               "kerning first=65 second=372 amount=-1\n"	\
                               "kerning first=256 second=375 amount=-1\n"	\
                               "kerning first=75 second=79 amount=-1\n"	\
                               "kerning first=221 second=242 amount=-1\n"	\
                               "kerning first=86 second=65 amount=-1\n"	\
                               "kerning first=8216 second=65 amount=-2\n"	\
                               "kerning first=75 second=210 amount=-1\n"	\
                               "kerning first=221 second=59 amount=-1\n"	\
                               "kerning first=321 second=218 amount=-1\n"	\
                               "kerning first=258 second=8217 amount=-1\n"	\
                               "kerning first=356 second=58 amount=-1\n"	\
                               "kerning first=65 second=118 amount=-1\n"	\
                               "kerning first=70 second=243 amount=-1\n"	\
                               "kerning first=256 second=255 amount=-1\n"	\
                               "kerning first=321 second=84 amount=-3\n"	\
                               "kerning first=87 second=195 amount=-1\n"	\
                               "kerning first=70 second=227 amount=-1\n"	\
                               "kerning first=82 second=356 amount=-1\n"	\
                               "kerning first=376 second=245 amount=-1\n"	\
                               "kerning first=8216 second=221 amount=1\n"	\
                               "kerning first=221 second=230 amount=-1\n"	\
                               "kerning first=194 second=374 amount=-2\n"	\
                               "kerning first=195 second=118 amount=-1\n"	\
                               "kerning first=195 second=372 amount=-1\n"	\
                               "kerning first=213 second=89 amount=-1\n"	\
                               "kerning first=321 second=89 amount=-2\n"	\
                               "kerning first=317 second=211 amount=-1\n"	\
                               "kerning first=221 second=283 amount=-1\n"	\
                               "kerning first=313 second=8221 amount=-4\n"	\
                               "kerning first=84 second=194 amount=-1\n"	\
                               "kerning first=356 second=228 amount=-2\n"	\
                               "kerning first=88 second=45 amount=-1\n"	\
                               "kerning first=84 second=224 amount=-1\n"	\
                               "kerning first=255 second=46 amount=-1\n"	\
                               "kerning first=87 second=101 amount=-1\n"	\
                               "kerning first=208 second=221 amount=-1\n"	\
                               "kerning first=345 second=8217 amount=1\n"	\
                               "kerning first=89 second=117 amount=-1\n"	\
                               "kerning first=313 second=87 amount=-1\n"	\
                               "kerning first=356 second=233 amount=-2\n"	\
                               "kerning first=350 second=350 amount=-1\n"	\
                               "kerning first=321 second=338 amount=-1\n"	\
                               "kerning first=89 second=8218 amount=-3\n"	\
                               "kerning first=89 second=262 amount=-1\n"	\
                               "kerning first=195 second=255 amount=-1\n"	\
                               "kerning first=208 second=376 amount=-1\n"	\
                               "kerning first=86 second=248 amount=-1\n"	\
                               "kerning first=82 second=221 amount=-1\n"	\
                               "kerning first=256 second=372 amount=-1\n"	\
                               "kerning first=87 second=339 amount=-1\n"	\
                               "kerning first=340 second=253 amount=-1\n"	\
                               "kerning first=196 second=253 amount=-1\n"	\
                               "kerning first=80 second=194 amount=-1\n"	\
                               "kerning first=193 second=121 amount=-1\n"	\
                               "kerning first=356 second=8222 amount=-2\n"	\
                               "kerning first=193 second=376 amount=-2\n"	\
                               "kerning first=221 second=67 amount=-1\n"	\
                               "kerning first=86 second=101 amount=-1\n"	\
                               "kerning first=260 second=376 amount=-2\n"	\
                               "kerning first=256 second=356 amount=-1\n"	\
                               "kerning first=84 second=44 amount=-2\n"	\
                               "kerning first=66 second=374 amount=-1\n"	\
                               "kerning first=76 second=356 amount=-3\n"	\
                               "kerning first=221 second=196 amount=-2\n"	\
                               "kerning first=76 second=89 amount=-2\n"	\
                               "kerning first=317 second=219 amount=-1\n"	\
                               "kerning first=87 second=235 amount=-1\n"	\
                               "kerning first=65 second=121 amount=-1\n"	\
                               "kerning first=86 second=232 amount=-1\n"	\
                               "kerning first=321 second=253 amount=-1\n"	\
                               "kerning first=356 second=250 amount=-2\n"	\
                               "kerning first=84 second=230 amount=-2\n"	\
                               "kerning first=86 second=245 amount=-1\n"	\
                               "kerning first=313 second=255 amount=-1\n"	\
                               "kerning first=192 second=118 amount=-1\n"	\
                               "kerning first=89 second=194 amount=-2\n"	\
                               "kerning first=70 second=251 amount=-1\n"	\
                               "kerning first=86 second=59 amount=-1\n"	\
                               "kerning first=86 second=234 amount=-1\n"	\
                               "kerning first=87 second=246 amount=-1\n"	\
                               "kerning first=192 second=8222 amount=1\n"	\
                               "kerning first=317 second=8217 amount=-4\n"	\
                               "kerning first=89 second=79 amount=-1\n"	\
                               "kerning first=86 second=117 amount=-1\n"	\
                               "kerning first=70 second=249 amount=-1\n"	\
                               "kerning first=8218 second=221 amount=-3\n"	\
                               "kerning first=89 second=213 amount=-1\n"	\
                               "kerning first=76 second=214 amount=-1\n"	\
                               "kerning first=213 second=221 amount=-1\n"	\
                               "kerning first=84 second=245 amount=-1\n"	\
                               "kerning first=258 second=356 amount=-1\n"	\
                               "kerning first=45 second=84 amount=-2\n"	\
                               "kerning first=192 second=375 amount=-1\n"	\
                               "kerning first=221 second=248 amount=-1\n"	\
                               "kerning first=356 second=252 amount=-2\n"	\
                               "kerning first=356 second=192 amount=-1\n"	\
                               "kerning first=260 second=86 amount=-1\n"	\
                               "kerning first=86 second=46 amount=-2\n"	\
                               "kerning first=356 second=246 amount=-2\n"	\
                               "kerning first=260 second=374 amount=-2\n"	\
                               "kerning first=376 second=196 amount=-2\n"	\
                               "kerning first=193 second=89 amount=-2\n"	\
                               "kerning first=256 second=8217 amount=-1\n"	\
                               "kerning first=376 second=230 amount=-1\n"	\
                               "kerning first=356 second=244 amount=-2\n"	\
                               "kerning first=195 second=8217 amount=-1\n"	\
                               "kerning first=195 second=356 amount=-1\n"	\
                               "kerning first=221 second=44 amount=-3\n"	\
                               "kerning first=84 second=65 amount=-1\n"	\
                               "kerning first=196 second=356 amount=-1\n"	\
                               "kerning first=196 second=89 amount=-2\n"	\
                               "kerning first=340 second=89 amount=-1\n"	\
                               "kerning first=86 second=226 amount=-1\n"	\
                               "kerning first=356 second=231 amount=-2\n"	\
                               "kerning first=256 second=376 amount=-2\n"	\
                               "kerning first=194 second=8217 amount=-1\n"	\
                               "kerning first=70 second=257 amount=-1\n"	\
                               "kerning first=356 second=45 amount=-2\n"	\
                               "kerning first=65 second=8222 amount=1\n"	\
                               "kerning first=80 second=65 amount=-1\n"	\
                               "kerning first=89 second=339 amount=-1\n"	\
                               "kerning first=193 second=8222 amount=1\n"	\
                               "kerning first=8220 second=74 amount=-1\n"	\
                               "kerning first=356 second=339 amount=-1\n"	\
                               "kerning first=313 second=220 amount=-1\n"	\
                               "kerning first=8216 second=194 amount=-2\n"	\
                               "kerning first=317 second=255 amount=-1\n"	\
                               "kerning first=84 second=251 amount=-2\n"	\
                               "kerning first=258 second=8218 amount=1\n"	\
                               "kerning first=87 second=46 amount=-1\n"	\
                               "kerning first=45 second=87 amount=-1\n"	\
                               "kerning first=258 second=87 amount=-1\n"	\
                               "kerning first=8218 second=86 amount=-2\n"	\
                               "kerning first=352 second=352 amount=-1\n"	\
                               "kerning first=86 second=229 amount=-1\n"	\
                               "kerning first=65 second=8217 amount=-1\n"	\
                               "kerning first=89 second=111 amount=-1\n"	\
                               "kerning first=376 second=338 amount=-1\n"	\
                               "kerning first=221 second=338 amount=-1\n"	\
                               "kerning first=119 second=46 amount=-1\n"	\
                               "kerning first=65 second=356 amount=-1\n"	\
                               "kerning first=196 second=8217 amount=-1\n"	\
                               "kerning first=221 second=193 amount=-2\n"	\
                               "kerning first=80 second=193 amount=-1\n"	\
                               "kerning first=88 second=212 amount=-1\n"	\
                               "kerning first=356 second=227 amount=-2\n"	\
                               "kerning first=195 second=8222 amount=1\n"	\
                               "kerning first=344 second=84 amount=-1\n"	\
                               "kerning first=195 second=253 amount=-1\n"	\
                               "kerning first=67 second=8221 amount=1\n"	\
                               "kerning first=260 second=253 amount=-1\n"	\
                               "kerning first=76 second=219 amount=-1\n"	\
                               "kerning first=376 second=211 amount=-1\n"	\
                               "kerning first=114 second=46 amount=-2\n"	\
                               "kerning first=83 second=83 amount=-1\n"	\
                               "kerning first=75 second=213 amount=-1\n"	\
                               "kerning first=317 second=85 amount=-1\n"	\
                               "kerning first=82 second=255 amount=-1\n"	\
                               "kerning first=86 second=367 amount=-1\n"	\
                               "kerning first=192 second=372 amount=-1\n"	\
                               "kerning first=79 second=88 amount=-1\n"	\
                               "kerning first=70 second=8218 amount=-3\n"	\
                               "kerning first=70 second=339 amount=-1\n"	\
                               "kerning first=253 second=46 amount=-1\n"	\
                               "kerning first=76 second=87 amount=-1\n"	\
                               "kerning first=76 second=8217 amount=-4\n"	\
                               "kerning first=70 second=8222 amount=-3\n"	\
                               "kerning first=192 second=221 amount=-2\n"	\
                               "kerning first=84 second=114 amount=-2\n"	\
                               "kerning first=356 second=119 amount=-2\n"	\
                               "kerning first=194 second=376 amount=-2\n"	\
                               "kerning first=86 second=193 amount=-1\n"	\
                               "kerning first=192 second=8221 amount=-1\n"	\
                               "kerning first=208 second=89 amount=-1\n"	\
                               "kerning first=8220 second=193 amount=-2\n"	\
                               "kerning first=87 second=65 amount=-1\n"	\
                               "kerning first=194 second=253 amount=-1\n"	\
                               "kerning first=376 second=214 amount=-1\n"	\
                               "kerning first=76 second=213 amount=-1\n"	\
                               "kerning first=70 second=196 amount=-2\n"	\
                               "kerning first=84 second=341 amount=-2\n"	\
                               "kerning first=313 second=212 amount=-1\n"	\
                               "kerning first=258 second=375 amount=-1\n"	\
                               "kerning first=340 second=121 amount=-1\n"	\
                               "kerning first=192 second=87 amount=-1\n"	\
                               "kerning first=356 second=97 amount=-2\n"	\
                               "kerning first=75 second=212 amount=-1\n"	\
                               "kerning first=260 second=121 amount=-1\n"	\
                               "kerning first=376 second=244 amount=-1\n"	\
                               "kerning first=376 second=229 amount=-1\n"	\
                               "kerning first=356 second=101 amount=-2\n"	\
                               "kerning first=356 second=345 amount=-2\n"	\
                               "kerning first=194 second=89 amount=-2\n"	\
                               "kerning first=70 second=226 amount=-1\n"	\
                               "kerning first=192 second=8217 amount=-1\n"	\
                               "kerning first=84 second=193 amount=-1\n"	\
                               "kerning first=195 second=84 amount=-1\n"	\
                               "kerning first=258 second=8221 amount=-1\n"	\
                               "kerning first=84 second=250 amount=-2\n"	\
                               "kerning first=317 second=220 amount=-1\n"	\
                               "kerning first=356 second=234 amount=-2\n"	\
                               "kerning first=193 second=118 amount=-1\n"	\
                               "kerning first=121 second=44 amount=-1\n"	\
                               "kerning first=214 second=221 amount=-1\n"	\
                               "kerning first=340 second=84 amount=-1\n"	\
                               "kerning first=196 second=84 amount=-1\n"	\
                               "kerning first=8216 second=193 amount=-2\n"	\
                               "kerning first=75 second=67 amount=-1\n"	\
                               "kerning first=8222 second=221 amount=-3\n"	\
                               "kerning first=194 second=354 amount=-1\n"	\
                               "kerning first=70 second=375 amount=-1\n"	\
                               "kerning first=89 second=242 amount=-1\n"	\
                               "kerning first=65 second=221 amount=-2\n"	\
                               "kerning first=84 second=195 amount=-1\n"	\
                               "kerning first=376 second=117 amount=-1\n"	\
                               "kerning first=376 second=8218 amount=-3\n"	\
                               "kerning first=221 second=58 amount=-1\n"	\
                               "kerning first=193 second=84 amount=-1\n"	\
                               "kerning first=196 second=8218 amount=1\n"	\
                               "kerning first=193 second=87 amount=-1\n"	\
                               "kerning first=193 second=255 amount=-1\n"	\
                               "kerning first=376 second=232 amount=-1\n"	\
                               "kerning first=376 second=233 amount=-1\n"	\
                               "kerning first=356 second=59 amount=-1\n"	\
                               "kerning first=88 second=214 amount=-1\n"	\
                               "kerning first=340 second=356 amount=-1\n"	\
                               "kerning first=84 second=252 amount=-2\n"	\
                               "kerning first=89 second=227 amount=-1\n"	\
                               "kerning first=89 second=230 amount=-1\n"	\
                               "kerning first=214 second=88 amount=-1\n"	\
                               "kerning first=84 second=248 amount=-1\n"	\
                               "kerning first=45 second=376 amount=-2\n"	\
                               "kerning first=258 second=89 amount=-2\n"	\
                               "kerning first=356 second=283 amount=-2\n"	\
                               "kerning first=8216 second=196 amount=-2\n"	\
                               "kerning first=195 second=86 amount=-1\n"	\
                               "kerning first=75 second=253 amount=-1\n"	\
                               "kerning first=87 second=229 amount=-1\n"	\
                               "kerning first=313 second=216 amount=-1\n"	\
                               "kerning first=76 second=221 amount=-2\n"	\
                               "kerning first=193 second=354 amount=-1\n"	\
                               "kerning first=317 second=86 amount=-2\n"	\
                               "kerning first=84 second=232 amount=-2\n"	\
                               "kerning first=221 second=117 amount=-1\n"	\
                               "kerning first=221 second=8218 amount=-3\n"	\
                               "kerning first=221 second=233 amount=-1\n"	\
                               "kerning first=84 second=253 amount=-2\n"	\
                               "kerning first=376 second=58 amount=-1\n"	\
                               "kerning first=80 second=8218 amount=-3\n"	\
                               "kerning first=89 second=249 amount=-1\n"	\
                               "kerning first=192 second=89 amount=-2\n"	\
                               "kerning first=221 second=249 amount=-1\n"	\
                               "kerning first=376 second=228 amount=-1\n"	\
                               "kerning first=193 second=372 amount=-1\n"	\
                               "kerning first=87 second=225 amount=-1\n"	\
                               "kerning first=340 second=255 amount=-1\n"	\
                               "kerning first=86 second=227 amount=-1\n"	\
                               "kerning first=192 second=374 amount=-2\n"	\
                               "kerning first=45 second=89 amount=-2\n"	\
                               "kerning first=376 second=283 amount=-1\n"	\
                               "kerning first=256 second=86 amount=-1\n"	\
                               "kerning first=313 second=253 amount=-1\n"	\
                               "kerning first=89 second=67 amount=-1\n"	\
                               "kerning first=313 second=79 amount=-1\n"	\
                               "kerning first=256 second=8221 amount=-1\n"	\
                               "kerning first=264 second=8221 amount=1\n"	\
                               "kerning first=260 second=84 amount=-1\n"	\
                               "kerning first=86 second=242 amount=-1\n"	\
                               "kerning first=376 second=111 amount=-1\n"	\
                               "kerning first=89 second=268 amount=-1\n"	\
                               "kerning first=221 second=250 amount=-1\n"	\
                               "kerning first=258 second=253 amount=-1\n"	\
                               "kerning first=194 second=375 amount=-1\n"	\
                               "kerning first=87 second=194 amount=-1\n"	\
                               "kerning first=87 second=227 amount=-1\n"	\
                               "kerning first=350 second=352 amount=-1\n"	\
                               "kerning first=321 second=212 amount=-1\n"	\
                               "kerning first=321 second=121 amount=-1\n"	\
                               "kerning first=317 second=87 amount=-1\n"	\
                               "kerning first=260 second=354 amount=-1\n"	\
                               "kerning first=86 second=111 amount=-1\n"	\
                               "kerning first=192 second=8218 amount=1\n"	\
                               "kerning first=84 second=46 amount=-2\n"	\
                               "kerning first=213 second=376 amount=-1\n"	\
                               "kerning first=321 second=376 amount=-2\n"	\
                               "kerning first=70 second=367 amount=-1\n"	\
                               "kerning first=376 second=213 amount=-1\n"	\
                               "kerning first=258 second=374 amount=-2\n"	\
                               "kerning first=321 second=366 amount=-1\n"	\
                               "kerning first=211 second=221 amount=-1\n"	\
                               "kerning first=221 second=212 amount=-1\n"	\
                               "kerning first=86 second=243 amount=-1\n"	\
                               "kerning first=88 second=262 amount=-1\n"	\
                               "kerning first=89 second=229 amount=-1\n"	\
                               "kerning first=258 second=221 amount=-2\n"	\
                               "kerning first=87 second=248 amount=-1\n"	\
                               "kerning first=195 second=8221 amount=-1\n"	\
                               "kerning first=70 second=232 amount=-1\n"	\
                               "kerning first=376 second=250 amount=-1\n"	\
                               "kerning first=70 second=253 amount=-1\n"	\
                               "kerning first=313 second=217 amount=-1\n"	\
                               "kerning first=86 second=228 amount=-1\n"	\
                               "kerning first=89 second=248 amount=-1\n"	\
                               "kerning first=211 second=88 amount=-1\n"	\
                               "kerning first=80 second=192 amount=-1\n"	\
                               "kerning first=221 second=101 amount=-1\n"	\
                               "kerning first=256 second=87 amount=-1\n"	\
                               "kerning first=89 second=211 amount=-1\n"	\
                               "kerning first=87 second=226 amount=-1\n"	\
                               "kerning first=260 second=8218 amount=1\n"	\
                               "kerning first=86 second=249 amount=-1\n"	\
                               "kerning first=76 second=211 amount=-1\n"	\
                               "kerning first=45 second=221 amount=-2\n"	\
                               "kerning first=221 second=213 amount=-1\n"	\
                               "kerning first=221 second=244 amount=-1\n"	\
                               "kerning first=376 second=192 amount=-2\n"	\
                               "kerning first=8216 second=195 amount=-2\n"	\
                               "kerning first=313 second=89 amount=-2\n"	\
                               "kerning first=356 second=230 amount=-2\n"	\
                               "kerning first=65 second=8221 amount=-1\n"	\
                               "kerning first=221 second=367 amount=-1\n"	\
                               "kerning first=376 second=234 amount=-1\n"	\
                               "kerning first=76 second=210 amount=-1\n"	\
                               "kerning first=70 second=242 amount=-1\n"	\
                               "kerning first=221 second=339 amount=-1\n"	\
                               "kerning first=8218 second=376 amount=-3\n"	\
                               "kerning first=345 second=44 amount=-2\n"	\
                               "kerning first=66 second=376 amount=-1\n"	\
                               "kerning first=376 second=262 amount=-1\n"	\
                               "kerning first=70 second=46 amount=-2\n"	\
                               "kerning first=195 second=87 amount=-1\n"	\
                               "kerning first=221 second=111 amount=-1\n"	\
                               "kerning first=210 second=376 amount=-1\n"	\
                               "kerning first=89 second=338 amount=-1\n"	\
                               "kerning first=341 second=8217 amount=1\n"	\
                               "kerning first=221 second=192 amount=-2\n"	\
                               "kerning first=8220 second=198 amount=-2\n"	\
                               "kerning first=356 second=351 amount=-2\n"	\
                               "kerning first=87 second=45 amount=-1\n"	\
                               "kerning first=321 second=79 amount=-1\n"	\
                               "kerning first=258 second=8222 amount=1\n"	\
                               "kerning first=76 second=8221 amount=-4\n"	\
                               "kerning first=118 second=44 amount=-1\n"	\
                               "kerning first=356 second=249 amount=-2\n"	\
                               "kerning first=86 second=196 amount=-1\n"	\
                               "kerning first=68 second=376 amount=-1\n"	\
                               "kerning first=84 second=263 amount=-2\n"	\
                               "kerning first=376 second=44 amount=-3\n"	\
                               "kerning first=65 second=87 amount=-1\n"	\
                               "kerning first=196 second=255 amount=-1\n"	\
                               "kerning first=313 second=338 amount=-1\n"	\
                               "kerning first=66 second=89 amount=-1\n"	\
                               "kerning first=317 second=218 amount=-1\n"	\
                               "kerning first=376 second=194 amount=-2\n"	\
                               "kerning first=340 second=221 amount=-1\n"	\
                               "kerning first=84 second=225 amount=-2\n"	\
                               "kerning first=194 second=8218 amount=1\n"	\
                               "kerning first=321 second=8217 amount=-3\n"	\
                               "kerning first=256 second=118 amount=-1\n"	\
                               "kerning first=258 second=255 amount=-1\n"	\
                               "kerning first=264 second=8217 amount=1\n"	\
                               "kerning first=8222 second=376 amount=-3\n"	\
                               "kerning first=76 second=212 amount=-1\n";




        fntFileOut.resize(sizeof(fntData)/sizeof(fntData[0]));
        memcpy(&fntFileOut[0],fntData,fntFileOut.size());
    }
    // "DejaVuSerifCondensed-Bold Image":
    {
        const unsigned int palette[55] = {	// each is a 4 bytes (RGBA) unsigned integer in little-endian encoding
                                            0U,234881023U,452984831U,167772159U,369098751U,419430399U,285212671U,83886079U,301989887U,66977277U,704643071U,905969663U,520093695U,1073741823U,1476395007U,1325400063U,1375731711U,687865855U,1157627903U,184549375U,1409286143U,587202559U,788529151U,855638015U,956301311U,1006632959U,654311423U,2013265919U,1610612735U,1694498815U,
                                            2533359615U,2432696319U,1577058303U,1895825407U,1593835519U,2080374783U,2348810239U,1258291199U,2483027967U,1761607679U,1862270975U,2264924159U,268435455U,1174405119U,2197815295U,2785017855U,2667577343U,2717908991U,2902458367U,2969567231U,3070230527U,3422552063U,3288334335U,3170893823U,3791650815U};

        unsigned char indices[512*512+1] = 	// [width x height (+ end string char)]: each (char-'0'), [skipping (char)92], points to a palette entry
                "0000000000000000000000000000000000000000000000000000001222300000042405260000000000000000007443000000000000000000000000000000000000000000000000088890000000000000000000000000000000000088800000000000990090000000000000000000000000000000000000000000000000000000008880000000000000000000000000000000000000000000088800000000007888000000078880000000007888000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000990090000000000000000000000000000000000000:;;<0000000000000000000000000015534<2000001=>>>;900005?>@A>>BC0000000000000000<BDDBE0000000000000000000000000000<"":::<00CFFF40000000FDDDG0000<"":::<0000000000EHHGHH:00000000E?DDF000000009GB=2IBHC0000000E:::500000000003<<200000<"":::<0000<"":::<<"":::<0000E?DDF0000<"":::<00000000<"":::<0000"	\
                "007EFJC000000000E?DDF000000009;DDDF000009;DDDF00000009;DDDF0000001J;;F4000003A:::::::2003A:::::::20000005:::E00000000002AA<800000<"":::<00<"":::<00000000000007J:::6000000004:::E0000000000<"":::<0009GB=2IBHC000001<<65J<90003:::800000000000005DKK=90000000000000000000000007=>>H@LDE0000GMNNOP:00002>NQRSTU10000000000009704UKVVS@2079000000000000000000000001UWWWU10:MXXH000000<DYVK=000ZUWWWUC000000007[KKLKKD8000000Z[SVY@500000006@]QIM]P<0000004DWWWB7000000000GLLD2000ZUWWWUC00ZUWWWUUWWWUC001BSVY@200ZUWWWUC000000ZUWWWUC00007;PXM=60000000Z[SVY@50000000E>OVXH00000E>OVXH0000000E>OVXH000002BMQKW?A0000:MWWWWWWW[30:MWWWWWWW[300009=WWWD500000007G>WWL?E0001UWWWU1ZUWWWUC000000000000ERWWW;00000000IWWW><000000001UWWWU1006@]QIM]P<00009IRR=DMP:000AMWW;00000000J=E00GX^KH00000000000000000000000003[]T?]_WA000E>TYMO]@2000"	\
                "CBK__NXHCC0000000003F=[I:W`SK_QGHBBF10000000000000000000006@OaO?10:WaKH000001=KaSU20008@VaO?1000000007[Y`LNVD8000000FM_]^QI70000003[]OSTOD40000005>NaY[300000000<DTN>20008@VaO?1008@VaO?@VaO?100FM_]^QI708@VaO?10000008@VaO?10000JRO_NWG0000000FM_]^QI7000004US^KB400004US^KB40000004US^KB400004[K_bc^]D<000FXaaaaaaTUC0FXaaaaaaTUC00009B]a_>20000000ALO^^_QG0006@OaO?18@VaO?1000000000000JM^aKH00000000ISa`LE000000006@OaO?1003[]OSTOD400009=SV>S_XF000:WaKH0000000E@S[14?Y^W:008A<000000000000000000000FX_MNQ>E000:LW>ULW><00;UUULWPUUUU;90000000;MSYKDQS?[SK?KY]MH7000000000000000000006@O^O?10:WaKH003ZZFMN]@<Z1008@VdO?5<4000003Z[]]L]]D8100002@YT>]VPJ1000000FR]OYWH10000005>N^Y[Z10000001BKa][Z1008@V^O?1008@V^O?@V^O?105@YT>SVPJ08@VdO?1ZZZZ008@VdO?1000CHKOLY]BCC00032@YT>]VPJ1000EHX`KU:0007ZHX`KU2000000EHX`KU:00000G"	\
                "W`cOOcaQ;000FXNNNN`cTUC0FXNN_b`NTUC00009B]e_>20000009=SccOOQG0006@O^O?18@VdO?5<400000000C2JM^fKH<8000007ISf`LEZZ0000006@O^O?103ZFR]OYWHZ10000:M`WNKDJ000:WaKH0000000IS_M:2P__P<<H?M?<0000000000000000000EI>VKNP;50003JUTTTLE00CUTTT>A=KTTT?10000004DTOY_K=I:AIIK_YTV><000000000000000000001UMMMU10:WaKH0C=?""?""?WWD?""?""?U208@VdO?>P@;100C=?""?""?=H=?""?""?U2000<>WL>>WM?U50000:U>>?""?""?""?U50000:U>MMM?""?U50000:U>QQQ?""?U50ZUMMM[C00ZUMMM[UMMM[C0<>WL?>WMF08@VdO?:?""?""?""?G08@VdO?100;UUKORY]UUUJ0C=?>WLF>WM?U207[KKWKKD803I?""?WW>PDH10007[KKWKKD80007B]fNP>NfS=000E@DDXO^]LI90E@XN^KL>DI9000<=@]e_>[:700003UTd_RDMF0001UMMMU18@VdO?>P@;100000E[>PM^fK>P?F7007I?Sf`L?""?""?G000001UMMMU1C=?""?""?[?U?""?""?U2003H@TS_L;Z000:WaKH0000000FXa]?<PSSP<UKT`KUC000000000000000007BKTVNNYQ[50003UTd`LE00CUTdN>E>VddO?1000000ELTL[KTUC007B]S[PTWA000000000000000000006@OOO?"	\
                "10:WaKH04@TOODA=SOOMJ08@VdOKN_TWH704@TOOD5=SOOMJ000:"">]NN]SOOMJ000:"">]NN]SOOMJ000:"">]NN]SOOMJ000:"">]NN]SOOMJ08@OOO?1008@OOO?@OOO?100E@OOO?J308@VdO?DYOOK=08@VdO?10CUTTTO_NKTTK;04@TOOD5=SOOMJ07[Y`LNVD808@OOOKV_OW;0007[Y`LNVD8000Z?OdO@DVfS=00004[K__QUA3000=SfNDFA53000<@KO_d^V]LG0000CUTdVD<JC0006@OOO?18@VdOKN_TWH7000EDSN_Y^fSN_YLF001?OOe^OOO]=900006@OOO?14@TOOD5=SOOMJ004>TN`_TWH900:WaKH0000000<>N^K=;II;IK^aTM=3000000000000000007BYcaafe`Q;0003UTd`LE00CUTdN>IKbgdO?10000004BU=EUU;0000GUU:=U[2000000000000000000006@OdO?10:WaKH00IKb^WFDOdT?808@VdO^`ceVPE00IKb^WFDOdT?8008U]cf^^]f^MJ008U]cf^^]f^MJ008U]cf^^]f^MJ008U]cf^^]f^MJ08@VdO?1008@VdO?@VdO?1006@OdO?1008@VdO@]b^KU<08@VdO?10CUTd`LXMKccK;00IKb^WFDOdT?807[]]L]]D808@VdO^^feTD4003[]]L]]D80008@VdOWKaNMF0000HX`aK[500000;Q_a]XWB7009=KaeadccfTD5000CUTdVLLU6000"	\
                "6@OdO?18@VdO^`ceVPE003BKae^aedc`cfTD501?O_ea___]=900006@OdO?10IKb^WFDOdT?8004>NNVaeOD400:WaKH0000000CUSTPF0000:"">TSDG10000000000000000007BYKXW]ce]B7003UTd`LE00CUTdN>>Oc^dO?10000002AA50119000001105AA<900000000000000000006@OdO?10:WaKH00EL_fSBM`bKH008@VdaKM]cbKH00EL_fSBM`bKH000JM`dOWXNd^MJ00JM`dOWXNd^MJ00JM`dOWXNd^MJ00JM`dOWXNd^MJ08@VdO?1008@VdO?@VdO?1006@OdO?1008@VdOSa^KU<008@VdO?10CUTd`LF;KccK;00EL_fSBM`bKH000:=?""?""?=G908@VdbSWTe`LE01=?""?""?H?""?""?F0008@VdO]c`]MH3002>OeTD<000000GRSVfaY[3005DOd_]e_Yf^WF000CUTd^`ND40006@OdO?18@VdaKM]cbKH00<>NdNWQ^f]MTe^WF03BPSf`LLXMUE00006@OdO?10EL_fSBM`bKH0004DW>DSfN>500:WOKH00000300E@LG900000FPDE000000000000000000000;DIFFM^eY[7003UTd`LE00CUTdN>QaNNdO?100009G>WWDF0000000000:@MW>H30000000000000000006@OdO?10:WaKH001U]fV>Kc_LE008@VdV>AM`eY[701U]fV>Kc_LE000GQbcKHB]e^MJ00"	\
                "GQbcKHB]e^MJ00GQbcKHB]e^MJ00GQbcKHB]e^MJ08@VdO?1008@VdO?@VdO?1006@OdO?1008@VdO^aS?<0008@VdO?10CUTd`L<;KccK;001U]fV>Kc_LE0005>OOY[3008@VdN>ISf^MJ05>OOY[QOT>E008@VdO]O^fVL:00FW^cKH000000<>TccNTY[300EM`f]]e_WacKI000CUTd_NND40006@OdO?18@VdV>AM`eY[70AW^f]BM^fKHXacKI008ISf`K_a`SU40006@OdO?101U]fV>Kc_LE0000JDSSVa]UZ00<U?""?:0000E[I:<<J70000009J<<<900000000000000000007A>PMSbaKH0003UTd`LE00CUTdN>OcSNdO?10000EPT`^T><000000005DY^`OM:0000000000000000006@OdO?10:WaKH000GX^aXTeYUZ008@VdO?<P_dTU300GX^aXTeYUZ000;KcbQFISf^MJ00;KcbQFISf^MJ00;KcbQFISf^MJ00;KcbQFISf^MJ08@VdO?1008@VdO?@VdO?1006@OdO?1008@VdcdVMG90008@VdO?10CUTd`L<;KccK;000GX^aXTeYUZ0005>NdY[3008@VdO?HKf^MJ05>NdY[DObS=308@VdO?@KbcS=90;KcaW:000000GQac]P?UG000AW^cK]e_L`fS=000CUTdVDDB10006@OdO?18@VdO?<P_dTU30FXafKHM^fKHM^fS=000ISf^_N^d^WF0006@OdO?100G"	\
                "X^aXTeYUZ00004DNcfTX=300AMWW;0000;KKWL><00000000<DL>H50000000000000000000FX_`bVK@<0003UTd`LE00CUTdNQaNLNdO?10000;KVMWNQF00000000:W_WMTSI0000000000000000006@OdO?10:WaKH0002DOf]_bQH0008@VdVDEL_eY[3002DOf]_bQH0000GQbcKHB]f^MJ00GQbcKHB]f^MJ00GQbcKHB]f^MJ00GQbcKHB]f^MJ08@VdO?1008@VdO?@VdO?1006@OdO?1008@VdO_fTPF0008@VdO?10CUTd`L<;KccK;0002DOf]_bQH00005>NdY[3008@VdO?HKf^MJ05>NdY[=]fV>508@VdVDEL`dTU30;KcaWG<50000HKc^W:<59000EM`fS]e_M^fSI000CUTdVD4100006@OdO?18@VdVDEL_eY[30:WafS=M^fKHWafSI000ISff]>QafS=0006@OdO?1002DOf]_bQH000004DQKTfNP<00:WaKH0000;Ka^`MJ00000000EM`OQ@:000000000000000000FX_`bcNXH0003UTd`LE00CUTdNTcSDNdO?10000;WW=[WMF00000000:LWUIWWI0000000000000000006@OdO?10:WaKH0009=Sc_b_RE0008@Vd`XDKbcSI0009=Sc_b_RE0000JM`dOMWNd^MJ00JM`dOMWNd^MJ00JM`dOMWNd^MJ00JM`dOMWNd^MJ08@VdO?1008@VdO?@VdO?100"	\
                "6@OdO?1008@VdOK^cY>A008@VdO?10CUTd`L<;KccK;0009=Sc_b_RE00005>NdY[3008@VdO?HKf^MJ05>NdY[HKf`LE08@Vd`XDQaeY[30FW^fTWLD;C00GQbc]ML>;C005>VdN]e_]caXF000CUTdVD8000006@OdO?18@Vd`XDKbcSI00<R_dOPW^fKD]faQG000ISf^W:L`eY[3006@OdO?1009=Sc_b_RE00000E=[[Xa^MJ00:WaKH0000:"">KN^MJ00000000EM`a^KH000000000000000000A>PLK`dT?8003UTd`LE00CUTd_^_MDNdO?GG5006AA52AA1000000003JA<4AA40000000000000000006@OdO?10:WaKH0000AM_geYU10008@VdN`V^d_M:0000AM_geYU100008U]cf^^]f^MJ008U]cf^^]f^MJ008U]cf^^]f^MJ008U]cf^^]f^MJ08@VdO?1008@VdO?@VdO?1006@OdO?1008@VdO?Sbb]@E08@VdO?10CUTd`LE;KccK;0000AM_geYU100005>NdY[3008@VdO?HKf^MJ05>NdY[HKf`ME08@VdN`V`d^Q;005DYcf^`OMF00EPOfc^`OMF007=Kae^dbcfTD2000CUTdVD8000006@OdO?18@VdN`V^d_M:001U]ccN_ed^VaeV><000ISf`LEP_dY[3006@OdO?1000AM_geYU100002?SYYNeNP<00:WaKH00000<=PMPE00000000<>MXSKH000000000"	\
                "0000000002A::DNdN>4003UTd`LE00CUTddf][DNdOQQQH000001GU?BA000000E=UU;80000000000000000000006@OdO?10:WaKH00006UYeaQ;00008@VdO]^a_QB100006UYeaQ;000000:"">]__]Sf^MJ000:"">]__]Sf^MJ000:"">]__]Sf^MJ000:"">]__]Sf^MJ08@V_O>J008@V_O?@VdO?1006@OdO?1008@V_O?DY__SU88@V_O?103[Ye^W:ISfbQG00006UYeaQ;000005>NdY[3008@V_O?HKf^MJ05>NdY[=Sf_P<08@VdO]^a`S?2000F>SV__cYUC07;L]V__cYUC00<?KNadc`YLF00014UTdVD8000006@OdO?18@VdO]^a_QB1000:PT^aN^fT^aNWH9000IS__LEM`f]=9006@OdO?10006UYeaQ;00000GQ^___VQ=C00:WaKH0000007EJE<90000000<EJF=?:00000000000000000FDWWWS^dO@8003UTd`LE00CUTdg_MFDNdcbbSI00009HWYTS>J0000EDSTYX=30000000000000000000<HDVdT?10:WaKH000EHIKcNP<00008@VdO?WXLB50000EHIKcNP<000000;XDPPDTe_P<000;XDPPDTe_P<000;XDPPDTe_P<000;XDPPDTe_P<01[RXVPF401[RRRBDVdTU10<HDVdT?1001[RRRB=>RRRU61[RRRB400IKcc]LMVd`MA000EHIKcNP<000005>NdT"	\
                "UF301[RRRBHKf^MJ04>NdTUPOdO@408@VdO?WXMUE00000J=DPLVV@8003:BDPLVV@8000<BP]e_M?F700CBDDOdVD80000<HDVdT?18@VdO?WXLB500009FDWXM^fKWXPI60000;RRRDHQaaQG00<HDVdT?100EHIKcNP<00000IScKPRDIE000:WaKH00000000<UPF000000:"">?<071000000000000000005DY^aacd^QI905E?Td`L<00CUT__]U4DN__ffSI00005DOTSNS=:IIFIKNSYNP<0000000000000000000IKS^f]B70:WaKH009BKKVf][C00008@VdO?:FE100009BKKVf][C000000HKOSKYac]U1000HKOSKYac]U1000HKOSKYac]U1000HKOSKYac]U1004B]YLXH004<<=KS^f]B70IKS^f]B70004<<AM]]W:<400AM]]W:00ALNec``fcY?4009BKKVf][C000004>NdVKR<004<<<8HKf^MJ0Z?Te`YOcaKI908@VdO?:FJ8000000074[S_T?Z000034[S_T?Z0000CB]e_>200008DY]aeYU10000IKS^f]B78@VdO?:FE100000002:FM^fKHFE300000C<<<UQVcYD200IKS^f]B709BKKVf][C00000GQaV]]YKU100:WOKH0000003:?KY>A0000JDYK[C0000000000000000000:W^bN__OQU20ZUMQ`dN>5007=RRRPFZURRYfVMG0000ELYL=KYUMKSWB]"	\
                "S=>YWA0000000000000000000ISfcNMF00:WaKH009B]ccNMF000008@VcO?100000009B]ccNMF0000000HKacfc^YP:0000HKacfc^YP:0000HKacfc^YP:0000HKacfc^YP:0009=S``KH00000=SfcNMF00ISfcNMF0000000HKc]U40000HKc]U4003IWO^aa_YPF0009B]ccNMF0000006?Yac`L<0000000HKc^MJ00;WV`a^NSD<008@VcO?1000000000007[Y`KI9000007[Y`KI900009B]c_>200008DVbb_X;00000ISfcNMF08@VcO?1000000000000JM^cKH00000000000CUT`]>F000ISfcNMF009B]ccNMF0000002?KTOOVO?100<U?""?:000009HL]^aS=F;;FISaN><0000000000000000000FXaN>PRD[G108DVadbSB30008<<<<704USaOMHC00008=[I<B[LNVO_M[[JH[=40000000000000000000ISSKMI100:WOKH009BSSKMIC000008@QQQU100000009BSSKMIC0000000FPXKSKW@F90000FPXKSKW@F90000FPXKSKW@F90000FPXKSKW@F90000E?MMPG00000=SSKMIC00ISSKMI10000001UYVPA00001UYVPA00008HDMXXM?F90009BSSKMIC0000000F>QQX><0000000HQQQMJ006I>MWWP[E0008@QQQU1000000000007[QWU20000007[QW"	\
                "U2000009BQQQ>200001UMQQMB400000ISSKMI108@QQQU1000000000000JMQQQH00000000000CUKWU:0000ISSKMI1009BSSKMIC0000000<=U@@DL[C000111000000CUS^VSDE>KKPE@]aXF0000000000000000000JM_aTYTT]M:08DVa_]>E00000000007IX`OM;300000097907GQO>DTKH7007900000000000000000000EIIHAC000<U@@:000JIIH:C0000000AGGGE000000000JIIH:C000000007<FHIHF50000007<FHIHF50000007<FHIHF50000007<FHIHF500000004JA<700000EIIHAC000EIIHAC00000007IDDH700007IDDH70000005AFFE8000000JIIH:C000000000<FGFE000000005GGGFC00095JAA<100000AGGGE0000000000000<G:400000000<G:40000000<GGG:9000004AGGA1000000EIIHAC000AGGGE0000000000000CFGGG5000000000000:IF400000EIIHAC0000JIIH:C00000000091664E500000000000000EDX>=J2>__P<FM_S=00000000000000000001BQTN_NN^W:08?WWRUA000000000008@QQL;3000000000000:DDIHDDF0000000000000000000000000000000000016600000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003447000000344700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000EF270JM`TD51[S>:000000000000000000005I?>>>>MM:00<""::E10000000000000AGGF30000000000000044334400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "0000000000000000000000000000000000000000000000000000000000000000000000000GQaQH00J[F0000000000000000000000065252:F1000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000=SS><000700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000EIIG9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000099009000000000000000000000000000000000000000088888100000000000000000000000000000000000000000000000000000000000000CCC3000000000000000000000000000000000000000000000000000C4543000000004224000000000000000522Z00000000001222300000000000421524000000000822422C0000000422400"	\
                "00000000000522Z000000001222300000000822422C00000000000000000000003CCC300000000000000522Z0000000012223000000000C22290000000052405240000CCCCCCCC00000000122230000000042405260000000000C2229000000000034541000000CCC70000000CCC30000000000E:::40000000009GB=2IBHC000000005:::::<0000000EHHGHH:00000000J@D?DDBZ000CFFFFF4000000008:::J90000004::5000000005AAA600<AAA3000000;UUUB400<"":::::5000E:::::10000001<A::8003A::E80000000005;UD>DBF8000004U>>?<0000000000000:"">>>B3000000001=>>>;9000000000:@>UDP[Z00000001[>>@>>I0000004U>>?<00000000000:"">>>B30000001=>>>;90000001[>>@>>I000000007EFJC000000004BUUU[<000000000000:"">>>B30000001=>>>;900000000H>>>F0000000A>>?F>>@200;UUUUUUUUG0000001=>>>;90000005?>@A>>BC000000000H>>>F000000005GUD>DUIJ300;UUUB400000;UUUB400000000<>WWWI0000000006@]QIM]P<00000007BWWWWW?6000007[KKLKKD80000"	\
                "003=SVWVTD5000:MXXXXH000000EFHWWWPFC00009=WW=90000001[WWWH04?WWMF000003UTTTLE01UWWWWW=70<>WWWWMG00000<BRWWWH00:MWWMUJ0000000E?QYVNO]W=700005D]_K=1000000000005U]_K[C00000000GMNNOP:000000004?YNSXVD400000004DVVM_Y[7000005D]_K=10000000005U]_K[C000000GMNNOP:0000004DVVM_Y[70000007;PXM=60000000:MTTTQG000000000005U]_K[C000000GMNNOP:00000009B]_K;0000000:MNSPYT><0CUTTTTTTTY[700000GMNNOP:0000002>NQRSTU1000000009B]_K;0000000EUXYVNVTKLF0CUTTTL<00003UTTTLE00000000<L`aS=0000000003[]OSTOD400000007[YaaaVD8000007[Y`LNVD8000000EPNSK_WG0000:WaKaKH000001UQQSa^QM:0007<B]YBA500000:M_^WF01U]a]U800003UTd`LE06@OaaaY[70<L`aaaK;00003BK_^aSI00FXaa`]?400000<@S^efbcfTU300000AD]NMF00000000009IQ^KU<00000000J>TYMO]?50000000<L_QO`][C00000004DVVM_Y[700000CAD]NMFCCC0000CCIQ^KU<CC0000J>TYMO]?5C00004DVVM_Y[CC00000JRO_NWG0000009"	\
                "=Scde][C0000000009IQ^KU<000000J>TYMO]?50000009B]_K;00000003IX`TNMG00CUTddeeeeY[70000J>TYMO]?500000CBK__NXH0000000009B]_K;000000<@S^efccfcK;0CUTd`L<00003UTd`LE0003ZZZ9<L`fS=004<<80007FR]OYWHZ7000003Z[QQQQQ@810003Z[]]L]]D810003ZIKVPNKBZ1000:WaKaKH00000Z?ObbdfaW:00ZIPQ]YQMDF0003BScT@400FW^`M:00003UTd`LE06@OdTYY[70<LYY`cK;00002>Nd_YSI00FXYVf^W:00007=Kad_SQKTTU30000<IUDWW=J90000000<IUWW?=J90000000:MW>>LW><0000000<>WD>WDA90000000<URR>RRI90000;UUUDWWUUUUJ00;UUUWWUUUUUJ00;UMW>;LW>UUJ00;UURRDRRUUUJ0000HKOLY]B7000004DOd^eNP<000000005;UWWDBF800000:MW>>LW><000005;URRPBF8000005;UWWWBF80CUTd`YYYYY[70000:MW>>LW>J300000<ULWPDUIJ3000005GURRPUIJ3007=Kad_]QK]NK;0CUTd`L<00003UTd`LE001=?""?""?;<L`fS=7G@PP?203I?""?""?""?U?""?""?I7000C=?""?""?HGG?""?""?U20C=?""?""?=H=?""?""?U20C=?""?WWLW>?""?U200:WaKaKH0000:UPQQSf^QW:00;WVaccb^KH000<PNcKI0005DOf"	\
                "SB70003UTd`LE06@OdY[BF006IBM`cK;0000<L`fS[BJ002=[XabQF00002>Vd^QBGHDKU3000:DQTNNOSP;300000:DQTNNOSP;300000:DQTNNOSP;300000:DQTNNOSP;300000:DQTNNOSP;300CUTTTLA;KTTK;0CUTTTLA;KTTK;0CUTTTL<;KTTK;0CUTTTL<;KTTK;0000HKORY]B700000:M`cSbaQG0000000E?QYVNO]W=7000E?QYVNO]W=7000E?QYVNO]W=7000E?QYVNO]W=7CUTd`L[[[[F0000EUXYVNVTKLF0000EUXYVNVTKLF0000EUXYVNVTKLF002>Vd^Q[G;BRK;0CUTd`L<00003UTd`LE004DOOO@<L`fS=:LT__ME08@OOO?MOOO?10004@TOOD5=SOOMJ04@TOOD5=SOOMJ04@TOODJ=SOOMJ00:WaKaKH000:"">]NN]Sf^MF10Z?TeO]Y]OKH000GQa`MA0007=SfVD50003UTd`LE06@OdY[700009EM`cK;0000<L`cK;700009AW^bQF0000:W^dO@5002HJ000EDYadccecOLF0000EDYadccecOLF0000EDYadccecOLF0000EDYadccecOLF0000EDYadccecOLF00CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;0000JLO_NWG000009=Sc^W_e][C00000<@S^efbcfTU300<@S^efbcfTU300<@S^efbcfTU300<@S^efbc"	\
                "fTU3CUTd`XXXXWG000<@S^efccfcK;000<@S^efccfcK;000<@S^efccfcK;00:W^dO@E=====F0CUTd`L<00003UTd`LE009=Sc`MAL`fS=[]fa_ME08@VdO?]fdO?10000IKb^WFDOdT?800IKb^WFDOdT?800IKb^WFDOdT?800:WaKaKH006U]cf^^]f^MJ005>NcK]YBDM;009=]fN>50000GQb`MJ0003UTd`LE06@OdY[700000EM`cK;0007EL`cK;000000AW^bQF7000FQbe]B900000003=SbeVKKTcfY?4003=SbeVKKTcfY?4003=SbeVKKTcfY?4003=SbeVKKTcfY?4003=SbeVKKTcfY?40CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;000F[DPXM?H400004DOdVDTeNP<00007=Kad_SQKTTU307=Kad_SQKTTU307=Kad_SQKTTU307=Kad_SQKTTU3CUTdfbbbbK;007=Kad_]QK]NK;007=Kad_]QK]NK;007=Kad_]QK]NK;00FQbe]BISSSSS?ZCUTd`L<00003UTd`LE000:M`cKIL`fS=>Nf]P><08@VdOM_ddO?10000EL_fSBM`bKH000EL_fSBM`bKH000EL_fSBM`bKH000:WaKaKH00JM_dOWXNd^MJ004DVdN]YW>I800CUTdT?10000:W^bQF0003UTd`LE06@OdY[700000EM`cK;002B[W^cQ;000000JM^cSU[A00FQbe]="	\
                "90000000<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;00CU]ON_NTXBZ000:M`f]BKcaQG00002>Vd^QBGHDKU302>Vd^QBGHDKU302>Vd^QBGHDKU302>Vd^QBGHDKU3CUTdb````K;002>Vd^Q[G;BRK;002>Vd^Q[G;BRK;002>Vd^Q[G;BRK;00FQbe]=ISfffO?ZCUTd`L<00003UTd`LE0004@TdT?L`fS=XaaXG<008@VdO]ffdO?100001U]fV>Kc_LE0001U]fV>Kc_LE0001U]fV>Kc_LE000:WaKaKH00GQbcKIB]e^MJ007=K^eec^VXI90Z?OdY[30000EM^cK;0003UTd`LE06@OdY[700000EM`cK;00FXYNcNMA0000002>Ob`TSI00FWadYU100CE400:W^dT?87=SfcKH00:W^dT?87=SfcKH00:W^dT?87=SfcKH00:W^dT?87=SfcKH00:W^dT?87=SfcKH0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;00CUT`NN^e_M:009=ScaQQQ`e][C000:W^dO@5002HJ00:W^dO@5002HJ00:W^dO@5002HJ00:W^dO@5002HJ0CUTd`LLLLLF00:W^dO@E=====F00:W^dO@E=====F00:W^dO@E=====F00FW"	\
                "adYUIS]VdO?ZCUTd`L<00003UTd`LE0000IKc_ML`fS[]fV><0008@VdO_^VdO?100000GX^aXTeYUZ0000GX^aXTeYUZ0000GX^aXTeYUZ000:WaKaKH00;KcbQFISf^MJ000<UX]O`adVD50Z?OdYU30000EM^cK;0003UTd`LE06@OdY[700000EM`cK;00FXac`]><0000008USNfaSI00<P_d_MG<J=L=70FQbe]B90GQbfS=00FQbe]B90GQbfS=00FQbe]B90GQbfS=00FQbe]B90GQbfS=00FQbe]B90GQbfS=0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;00CUSM>>X^cSI004DOdecccfdNP<000FQbe]B90000000FQbe]B90000000FQbe]B90000000FQbe]B9000000CUTd`LGGGGE00FQbe]BISSSSS?Z0FQbe]BISSSSS?Z0FQbe]BISSSSS?Z0<P_d_M;=DVdO?ZCUTd`L<00003UTd`LE0000JM_cKL`fS>NfSB30008@VdafYVdO?1100002DOf]_bQH000002DOf]_bQH000002DOf]_bQH0000:WaKaKH00GQbcK;=Sf^MJ001=UG[]YKb`LE0CUTdT?10000:W^bXF0003UTd`LE06@OdY[700000EM`cK;00:LWScaQG000000EL_eTWWH001U]cfTWPMKTU30FQbe]=90GQbf]=00FQbe]=90GQbf]=00FQbe]=90GQbf]=00FQbe]=90"	\
                "GQbf]=00FQbe]=90GQbf]=0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;009;PK]YY_e]B70:M`eNNNNNcaQG000FQbe]=90000000FQbe]=90000000FQbe]=90000000FQbe]=9000000CUTd`QQQQQ?60FQbe]=ISfffO?Z0FQbe]=ISfffO?Z0FQbe]=ISfffO?Z01U]cfOWPLVdO?ZCUTd`L<00003UTd`LE00006UYeOT`eTTa^WF00008@Vdg`WVdO?""?UE0009=Sc_b_RE000009=Sc_b_RE000009=Sc_b_RE0000:WaKaKH00:W^eY>LVd^MJ004>]W>]YQa_L<09=SfN>50000;Qb`MJ0003UTd`LE06@OdY[700000EM`cK;003J:M`cK;000000AM^bQ;A6000:ROce^_`bTU30FWaeYUC0HKcfSI00FWaeYUC0HKcfSI00FWaeYUC0HKcfSI00FWaeYUC0HKcfSI00FWaeYUC0HKcfSI0CUTd`LE;KccK;0CUTd`LE;KccK;0CUTd`LE;KccK;0CUTd`LE;KccK;00EPObb^`beYB79=ScbQ>>>W^e][C00FWadYU100CE400FWadYU100CE400FWadYU100CE400FWadYU100CE40CUTdfbbbbO@60FWadYUIS]VdO?Z0FWadYUIS]VdO?Z0FWadYUIS]VdO?Z00:ROce^_`beO?ZCUTd`L<00003UTd`LE00000HQbdddgdddOD500008@VdfY@VdNOO"	\
                "QF0000AM_geYU1000000AM_geYU1000000AM_geYU10000:WaKaKH004@TfbNNOf^MJ004>N^NVN`cY?400GQa`MA0007B]fVD4005E?Td`L<06@OdY[300007EM`cK;0000<L`cK;000000AW^bQF000007;L]_ab^NS[30EL_dNP:E@TeaXF00EL_dNP:E@TeaXF00EL_dNP:E@TeaXF00EL_dNP:E@TeaXF00EL_dNP:E@TeaXF03[Ye^W:ISfbQG03[Ye^W:ISfbQG03[Ye^W:ISfbQG03[Ye^W:ISfbQG00GQbbSMM_eYB74DO_NPE45DO_NP<00<P_d_MG<J=L=70<P_d_MG<J=L=70<P_d_MG<J=L=70<P_d_MG<J=L=7CUT____^_O@60<P_d_M;=DVdO?Z0<P_d_M;=DVdO?Z0<P_d_M;=DVdO?Z007;L]_aba_TQU1CUT_`LJ0005E?Td`L<00000ERN__ae___S=900008@V__W@V__fbQF00006UYeaQ;00000006UYeaQ;00000006UYeaQ;00000:WaKaKH000GMOaaOS__MJ004DSTN__N]RF000<PNcKI0005DOcS=70ZUMQ`dN>506@OdYUUF008=UM`cK;0000<L`cKHA8003J:WabQF0000007:URWSY>BE00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><00IKcc]LMVd`MA00IKcc]LMVd`MA00IKcc]LMVd`MA"	\
                "00IKcc]LMVd`MA00HKcaWUQaeYB74?RRR[10;XVRRD<001U]cfTWPMKTU301U]cfTWPMKTU301U]cfTWPMKTU301U]cfTWPMKTU37=RRRRXVRR[101U]cfOWPLVdO?Z01U]cfOWPLVdO?Z01U]cfOWPLVdO?Z0007:URWXWL?H207=RXVPF40ZUMQ`dN>5000001[RRL`fSRRP:000001[RRRU[RRYfTR:000EHIKcNP<000000EHIKcNP<000000EHIKcNP<00000:WaKaKH000C;>WW>RRRD<000AI?>]Y>[:70003BScTD400FW^_M:008DVadbSB306@OdTTY[70<LTT^cK;0000<P_eYWWH00:LWScaXF00000000AMMW_L<0000FMVfc`_be_XH9000FMVfc`_be_XH9000FMVfc`_be_XH9000FMVfc`_be_XH9000FMVfc`_be_XH900ALNec``fcY?400ALNec``fcY?400ALNec``fcY?400ALNec``fcY?400FX^eNT_`eYB705<<<407B]YLXH0000:ROce^_`bTU300:ROce^_`bTU300:ROce^_`bTU300:ROce^_`bTU308<<<B]YLXH000:ROce^_`beO?Z00:ROce^_`beO?Z00:ROce^_`beO?Z000001AM]]W:00008B]YLXH08DVadbSB30000004<<L`fS=<<90000004<<<44=KaOLG7009BKKVf][C000009BKKVf][C000009BKKVf][C00000:WaKaKH0007BWWWWW?"	\
                "<2000000ZB]YBC000000:MN^WG01U]^]U6008DVa_]>E006@O^^^Y[70<L`^^^K;00008?YcfaSI00FXacfVP<00000000AW`^O>200003HMY`aa^OXB400003HMY`aa^OXB400003HMY`aa^OXB400003HMY`aa^OXB400003HMY`aa^OXB40003IWO^aa_YPF0003IWO^aa_YPF0003IWO^aa_YPF0003IWO^aa_YPF0005@]^a_K__YB700000009=S``KH00007;L]_ab^NS[3007;L]_ab^NS[3007;L]_ab^NS[3007;L]_ab^NS[300009=S``KH0007;L]_aba_TQU1007;L]_aba_TQU1007;L]_aba_TQU1000000HKc]U400009=S``KH08DVa_]>E000000000<L`cS=00000000000000FMNVM;70009B]ccNMF0000009B]ccNMF0000009B]ccNMF000000:WaKaKH0007[YaaaVD800000009=KKB70000001BMMM;08?MMLF0008?WWRUA0001UMMMMM=90<>MMMMMF00000FPKYYSI00FXYYSMH700000000EDMW>;900000CF?MXXMDH4000000CF?MXXMDH4000000CF?MXXMDH4000000CF?MXXMDH4000000CF?MXXMDH4000008HDMXXM?F900008HDMXXM?F900008HDMXXM?F900008HDMXXM?F90000A?WXM?RRRI000000000E?MMPG000007:URWXW>BE00007:U"	\
                "RWXW>BE00007:URWXW>BE00007:URWXW>BE000000E?MMPG00007:URWXWL?H200007:URWXWL?H200007:URWXWL?H20000001UYVPA000000E?MMPG08?WWRUA0000000000<PQQQI00000000000009=QQMH300009BSSKMIC0000009BSSKMIC0000009BSSKMIC000000:WOKOKH0007[QQQQQ@800000000<;;E000000004JJJ100<JJE30000<""::E1000002JJJJJ4000<JJJJJC000009JI[[[J002B[[=FC00000000005EA<7000000008EFFA500000000008EFFA500000000008EFFA500000000008EFFA500000000008EFFA50000000005AFFE8000000005AFFE8000000005AFFE8000000005AFFE800000004:FJ5<<<100000000004JA<700000001<"":F:2300000001<"":F:2300000001<"":F:2300000001<"":F:23000000004JA<70000001<"":F:E8000000001<"":F:E8000000001<"":F:E8000000007IDDH700000004JA<700<""::E1000000000007FGGG200000000000000<GGFC000000JIIH:C00000000JIIH:C00000000JIIH:C0000000<U@@@@:0000<GGGGGJ0000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000777000077700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000344700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000166660000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "000000000000000000000000000000000000000000000000000000000000000000CCC70CCC70000CCC7003CCC30000CCC700000000CCCC009CCC0009CCC93451000000000000000000000000042405260000000000122522400000CCCCC3000000000000522Z00000001222300000000Z44830000000524052400007CCCCCCCCCC900000000000000004215240000000000000000000004240526000000000000000000000122522400000CCC700CCCC00000000144890000000000093CCCC900000964469000000000003CC30000CCC700000000078513CCC0000CCC700000000000000015243000000000CCC30000000007CCC0000007CCCCCCCCC0000000000Z54900000000000000000000000090000000000000000099999999000000000;UUUB;UUUB400;UUUB45BUUUB400;UUUB4000000;UUUUE3IUUU;97HUUUIBD>UG300000005:::::<000000005?>@A>>BC00000000C=>>>>>U4000;UUUUU[IA300000000:"">>>B3000001=>>>;9000002I?>>@[H<0000A>>?F>>@2001=UUUUUUUUUUH9002FFFA0000000:@>UDP[Z000000"	\
                "5:::::<0000005?>@A>>BC00000007EFJC0000000C=>>>>>U4000;UUUB4EUUUUJ00000<IUD>?=J900000005F=UUUUUH7007A=?>>?=:1000000002[UUB400JUUUI10000003:B@>=BUUUG00;UUUB40000000000006GU>>D[FC000000:UUUU<00000008=UUUF00001=UUUUUUUUUF00000001GU>DI<000000000000000000006FIBIA70000000000003HBBBBBBBBG0000000CUTTTLUTTTL<0CUTTTL<BQTTTLE0CUTTTL<00000CUTTTS=4DTTT?18@TTT>SVNYM;0000007BWWWWW?600000002>NQRSTU100000000FMNOK_]@500CUTTTTTYKLH70000005U]_K[C00000GMNNOP:0000<?QTNNOYK[7000:MNSPYT><002>TTTTTTTTTT?109=XXX>2000004?YNSXVD4000007BWWWWW?6000002>NQRSTU10000007;PXM=6000000FMNOK_]@500CUTTTL<;KTTK;0000:DQTNNOSP;300000J?WSYTTTT?Z01HLSONNOSM=20000003=KTTME00;QTTMG00000CHM]OO?MTTTU3CUTTTL<000000000008=WYNNV]MI100000ISTTQF0000000<LTT]=00002>TTTTTTTT]=0000003IWYNOKU2000000000000000003;UWS]SRF0000000000005>]]]]]]]]U1000000"	\
                "CUTd`LUTd`L<0CUTd`L=X_eNWH30CUTd`L<00000CUTddNPEDNdO?18@VdVYcddfO><000007[YaaaVD80000000CBK__NXH000000002DYVMN]DJ000CUTddeeecVRJ000009IQ^KU<00000J>TYMO]?5007=K^dcbceY[70003IX`TNMG0002>_eeeddeeeO?103B]b_P<Z0000<L_QO`][CC0000C[YaaaVD8C0000CBK__NXHCC00000JRO_NWG000002DYVMN]DJC00CUTd`L<;KccK;000EDYadccecOLF00002?]^fdeeeO?Z0:MVcdbbfcNQ[500000JLNf][C002DOfT@40000FMVfeO?M`dTU3CUTd`L<5<510000000;W_ecaffNMG009ZZISfbQFZZ0009Z<L`e]=Z1002>_edddeee]=000000:MNecf^K=ZZ101ZZ1000000000<PY_OYVT@4000000000003=KOKTSKNQ;7000000CUTd`LUTd`L<0CUTd`LWNdNWI100CUTd`L<00000CUTdgbK=DNdO?18@Vd`b__bgaXF00000<[QQQQQ@80000000<IULWP=J90000000<>WMWWDA9000CUTd`]]NecKH00002I?WW@[H<0000:MW>>LW><004DVdNKQKTY[70002I?WWW[H<002>YYYYfbYYYY?13=?]e_P?""?G00;U>WD>WDUUUJ00;UUQQQQQ@UUJ00;UUULWPUUUUJ0000HKOLY]B7000;U>WMWWDUUUJ0CUTd`L<;KccK"	\
                ";003=SbeVKKTcfY?4009IQad`TYYYY?Z0:WaNbKQS_d`QI30009=ScaXF0009=Sf^WF0001U]ffTQUM`dTU3CUTd`L[>P>UG800004?Te^SXYcfYUZ0;?""?""?SfbQ?""?""?:00;?""?L`e]?""?U202>YYTd`YYY]=000001[]fNQYfNP?""?UEU?""?""?E00000000<PYQ]]MOP<00000000002[ULQHXMBQDUI100000CUTd`LUTd`L<0CUTd`MNe_Q=8000CUTd`L<00000CUTd^bV>DNdO?18@VdfYLPScfKI0000:DQTNNOSP;300000:DQTNNOSP;300000:DQTNNOSP;300CUTd`LBQbfSI000<?QTNNOYK[700<?QTNNOYK[70<P_fS=GI?WB700<?QTNNOYK[70CI[[[SfbQ[[[G04DOOe`OO]B9CUTTTL<;KTTK;0CUTTTLG;KTTK;0CUTTTLA;KTTK;0000HKORY]B700CUTTTLA;KTTK;0CUTd`L<;KccK;00<>Nd`WI;PVd`W:005DOd^Q?[[[[;90:WKTbQG@KcdNPE0004DOdN><2225FW^f]B700<PNg_M;EM`dTU3CUTd`LSN_NYWB5000AM__YUFRNdN>27[YOOOfcOOOKH07[YOO^eOOOMJ0CI[UTd`L[[[:50000<PNe]BXabQOOQHKOOXF00004IIIH[UQSTU?[GIII<000003=KTTMEGF;QTTMG00000CUTd`LUTd`L<0CUTd`Oe`QB40000CUTd`L<00000CUTdNNbKDNdO?18@Vd`MFAW^"	\
                "f]=000EDYadccecOLF0000EDYadccecOLF0000EDYadccecOLF00CUTd`L=QbcKH007=K^dcbceY[707=K^dcbceY[702>NdNSWPUF5007=K^dcbceY[700077ISfbQF77004DVbdcbb]B9CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;003ZJLO_NWG100CUTd`L<;KccK;0CUTd`L<;KccK;00:W^dT?87=SfcKH00AM^dO@57777000EDUTbQD]a`abKI000JM`dTUG>>>DEP_dOD400:WadTU1EM`dTU3CUTd`]bccee_Q=C00E>>>>G6?Od_P<7[YeeegdeefKH07[Ybbfdbb^MJ0003UTd`XXXXW@:000FWafSIM^e]NeYUYeV>20009HXSSMEFQTS=Z4>SSKB10000JLNf][C002DOfT@40000CUTd`LUTd`L<0CUTdee^KU500000CUTd`L<00000CUTdNKcO>NdO?18@VdND4EM`e]=003=SbeVKKTcfY?4003=SbeVKKTcfY?4003=SbeVKKTcfY?40CUTd`]]NfVLJ004DVdNKQKTY[704DVdNKQKTY[701U]cdca_YMH704DVdNKQKTY[700000ISfbQF00008@X]e_XXX=9CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;01=?""?""?PXM?""?[40CUTd`L<;KccK;0CUTd`L<;KccK;00FQbe]B90GQbfS=00FQbe]B9000000004UTbQKa`QVdT?600FXbe]BIS__W:DN"	\
                "d_P<00FQbe]=9EM`dTU3CUTda^YKKTbdNMA00052227JPNdOD47[Yb]]fb]]fKH07[YYX`e]X^MJ0003UTdfbbbb^]D200GQcfSIM^dT]fNPNfSB7000<>Of]U8FQKDE07IQb_M:0009=ScaXF0009=Sf^WF0000CUTd`LUTd`L<0CUTdeec]DJ00000CUTd`L<00000CUTdNRNaQNdO?18@VdV@8<L`e]=00<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:0CUTddeef]MH300<P_fS=GI?WB70<P_fS=GI?WB700:"">]_afdfO><0<P_fS=GI?WB700000ISfbQF00000EB]e_PFF20CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;05>OOTUISOOLE0CUTd`L<;KccK;0CUTd`L<;KccK;00FQbe]=90GQbf]=00FQbe]=9000000003USSQOdO@YdNP<00GQbe]BISfaW:DVd`L<00FQbe]=0EM`dTU3CUTdf]@;HDYfcSI0000000<?]cbK=77[]]XSfbQS]KH07=MML`e]PMPE0003UTdb```bgaXF00GQcfSIM^dTW^aQa^WF0000;QaaXF05IHE000EPNf]B3004DOdN><2225FW^fSB7000CUTd`LUTd`L<0CUTd`Oeb]@E0000CUTd`L<00000CUTdN>ScONdO?18@VdV@8<L`e]=00:W^dT?87=SfcKH00:W^dT?87=SfcKH00:W^dT?87=SfcKH0CUTd`]TbcTD2"	\
                "002>NdNSWPUF5002>NdNSWPUF5000EAULQ]NdaXF02>NdNSWPUF5000000ISfbQF000009B]e_P<000CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;05>NdTUISf`LE0CUTd`L<;KccK;0CUTd`LE;KccK;00FWaeYUC0HKcfSI00FWadYU1000000000AII>NdTU]e`ME00FXbeY[=SfaXF>Ng_P<00FXaeY[3:W^dTU3CUTd^W:00FW^eY[300000<UK^fT>E00:===SfbQ===E004JJL`e]=JE90003UTd`LLLSffSI00FXbfSIM^eY>VfYfVD50001UYeN><000000001UYeN>200JM`dTUG>>>DEP_dOD4000CUTd`LUTd`L<0CUTd`MNeaSU<000CUTd`LGGGGE0CUTdN>LNaNdO?18@VdV@8<L`e]=00FQbe]B90GQbfS=00FQbe]B90GQbfS=00FQbe]B90GQbfS=0CUTd`LDYeaQ;001U]cdca_YMH701U]cdca_YMH704DLBFGBQccQG01U]cdca_YMH700000ISfbQF000009B]e_P<<70CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;05>NdYUISf`LE0CUTd`L<;KccK;03[Ye^W:ISfbQG00EL_dNP:E@TeaXF00<P_d_MG<10000000004>NdTUSf^MJ00AW^dO@UYfcKIL`dN>500EM`dV>E=KcdTU3CUTd`LE00EL`dY[30000<UK^eVMG000000ISfbQF0000"	\
                "000<L`e]=0000003UTd`LEFQbfS=00EM`f]=W^cKBSfefS=7000<PNdTU1000000000ISf^W:00FXbe]BIS__W:DNd_P<000CUTd`LUTd`L<0CUTd`LWNd^K[500CUTd`QQQQQ?6CUTdN>[ScddO?18@VdV@8<L`e]=00FQbe]=90GQbf]=00FQbe]=90GQbf]=00FQbe]=90GQbf]=0CUTd`LGX^eT?400:"">]_afdfO><00:"">]_afdfO><05>NSWPL]faXF00:"">]_afdfO><00000ISfbQF000009B]e`MLPF0CUTd`L<;KccK;0CUTd`L<;KccK;0CUTd`L<;KccK;05>NdY[ISf`LE0CUTd`L<;KccK;00IKcc]LMVd`MA00ZU]fc]MLK^dV><001U]cfTWP[<000000004>NdTUSf^WA002>Nd^QX_V^OMScdT?1004@TebSRWNfdTU3CUTd`L<00:W^f]=9000J?K^eNWH3000000ISfbQF0000000<L`e]=0000003UTd`LEFXbfS=004?TeOLSc`MFW^g^WF0000AM^fS=0000000000FQbcK;00GQbe]BISfaW:DVd`L<000CUTd`LUTd`L<0CUTd`L=Q_d`QB40CUTdfbbbbO@6CUTdN>:M_ddO?18@VdV@8<L`e]=00FWaeYUC0HKcfSI00FWaeYUC0HKcfSI00FWaeYUC0HKcfSI0CUTd`L<DTd^WF00EAULQ]NdaXF00EAULQ]NdaXF05>Nf^__ceV><00EAULQ]NdaXF00000ISfbQF"	\
                "000000HKcf``K;0CUTd`LE;KccK;0CUTd`LE;KccK;0CUTd`LE;KccK;05>NdY[ISf`LE0CUTd`LE;KccK;00ALNec``fcY?4000FMVfc`_be_XI9000:ROce^_KU800000004>NdTU]e`ME003BSbda^cSTf^bdcSI0000HQ^db_^O`dTU3CUTd`L<0<UScbQ;003F>]afVMI<BH70000ISfbQF0000000<L`e]=0000003UTd`LEFXbfS=000;X`c_ac]U5DOdOD40000:WafKH0ZAH==H:40:WafKH00FXbeY[=SfaXF>Ng_P<000CUT__L?Od_L<0CUT__L<BQ___XI9CUT______O@6CUT_N>5[]__O?18@V_V@E:M^fS=00EL_dNP:E@TeaXF00EL_dNP:E@TeaXF00EL_dNP:E@TeaXF0CUT__L<HQ__]U14DLBFGBQccQG04DLBFGBQccQG05>]N^ab^OWI704DLBFGBQccQG00000IS`_QF000000EPY___K;03[Ye^W:ISfbQG03[Ye^W:ISfbQG03[Ye^W:ISfbQG05>NdTU[]e`LE03[Ye^W:ISfbQG003IWO^ca_YPF00003HMY_fd`TXB400007;R]N_^`MJ00000004>N_TUYdN>5000E>]`b^Y>MOafdNLJ00005UK_a^TM`dTU3CUT__L<F@K`dVPE00GMTbcTMHI>KD40000ISfbQF0000000<L`e]=0000003UT__LEGQbfSI0004UK_a^YP:<>NcS=90000:WafSIFUMKSSK"	\
                "W?;FXacKH00AW^dO@UYfcKIM`dN>50007=RRRMQ`dV>507=RRRDI=URRRRI07=RRRRRRRR[17=RRRU==PRRRB31[RRR[PM]ccKH00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><07=RRRD==DRRR[15>NSWPL]faXF05>NSWPL]faXF09FB>MXSYDH6005>NSWPL]faXF0000C;RSY>:000000CF?SYRPF00IKcc]LMVd`MA00IKcc]LMVd`MA00IKcc]LMVd`MA04DVdVLMNd`LE00IKcc]LMVd`MA0008HDXVXM?F900000CFUQbe]DH40000007:[>RKbQF00000008URRRDVe][C0000:?MXW@FBL]a_XI300000<[LXW>M`dTU37=RRRD?WYae_XI700GMNebYWWKOND40000ISfbQF0000000<L`e]=0000007=RRRDLMTebQ;0000<[LXW@FHHXa`M:00000EL_dOMW]_cccc^TQM]faXF002>Nd^QX_V^OMSceYUC00008<<DVadbSB3008<<<AM]]W:<<1008<AM]]W:<4008<<AM]]W:<<8004<<<"":W^cd_MA000FMVfc`_be_XH9000FMVfc`_be_XH9000FMVfc`_be_XH9008<<AM]]W:<<405>Nf^__ceV><05>Nf^__ceV><0003AMMW_L<0005>Nf^__ceV><0000AMMW_L<000000AMMW_L<700ALNec``fcY?400ALNec``fcY?400ALNec``fcY?40CU]fc__"	\
                "Oe`LE00ALNec``fcY?400007B]YLXH000000000FQbe]=00000000003AWYaXF000000004<<FW^^WG0000004AFB>W]NbOW=400000001EF:EM`dTU308<<DST`fbOW=40001IWNffaaceND40000ISfbQF0000000<L`e]=00000006<<<;K^feVPE000001EF:5HKKOeT@4000006U]cc^af_YKQ]Nca^beO><003BSbda^cSTf^bd^X;00000004DVa_SDE0000000HKc]U40000000HKc]U40000000HKc]U4000000000:Wa^VQB10003HMY`aa^OXB400003HMY`aa^OXB400003HMY`aa^OXB4000000HKc]U400005>]N^aa^OWI705>]N^aa^OWI70000AW`^O>20005>]N^aa^OWI70000AW`^O>2000000AW`^O>20003IWO^aa_YPF0003IWO^aa_YPF0003IWO^aa_YPF000FMVaaTS__LE003IWO^aa_YPF000009=S``KH000000000FQbc]=00000000000AWaT><0000000000<?Yc]?400000000ZU]^^OK>;8000000000000EM`cTU30003BSb_TK>;6000006IMT^cb`YWB10000IScbQF0000000<L`c]=00000000000;Ka`OWI70000000000HKcf_X;0000000FRT^b_]MU;;BLSNa^OWH9000E>]`b^Y>MOaa_KU400000008?WWRUJ00000001UYVPA00000001U"	\
                "YVPA00000001UYVPA0000000000AMWM>I500000CF?MXXMDH4000000CF?MXXMDH4000000CF?MXXMDH40000001UYVPA000009FB>MXXMDH6009FB>MXXMDH600000EDMW>;90009FB>MXXMDH600000EDMW>;9000000EDMW>;900008HDMXXM?F900008HDMXXM?F900008HDMXXM?F90001HPXWDRRRD<0008HDMXXM?F9000000E?MMPG000000000FWQQQ=00000000000AMQ>G00000000000:WQQLF0000000000:LKWDI<90000000000000ELQQQ[30000:MQL@I<000000001GDWQQM?G600000IQQQWF0000000<PQQQ=00000000000GWWMDHZ00000000000HKSKWB400000007FDMXMUF6003JBRXW>HZ00000:?MXW@F;>WXL[<000000000E::<1000000007IDDH700000007IDDH700000007IDDH700000000003::A27000000008EFFA500000000008EFFA500000000008EFFA5000000007IDDH7000000035JFFA590000035JFFA5900000005EA<700000035JFFA5900000005EA<7000000005EA<700000005AFFE8000000005AFFE8000000005AFFE80000007<F:2<<<20000005AFFE80000000004JA<70000000008GGGG<000000000001FGE9000"	\
                "000000001GGGF700000000003G;:590000000000000003FGGGE000001;;E40000000000005:GGJ400000005GGGG800000007FGGG<00000000000Z::J50000000000000<IIIF600000000005AFJ8000000CEF:<900000004AF:409<"":FE100000000000000000000000034470000000003447000000000344700000000000000000000000000000000000000000000000000000000000000000000000034470000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000C454300000CCCCCCCC90000009145410000000CCCCCCCCCC000000000000000C454300000CCC7003CCC3000000"	\
                "9CCCCCCCC0000000CCC7007CCC0000CCC7007CCC0000000CCCC9000CCCC700000000822422C00000000822422C000000000822422C000000000822422C0000000822422C0000000CCCCCCCC30000CCC30003CCC0000014541000000000009CCCCCCCC0000009CCCCCCCC00000000000CCC7007CCC000000000000007854100000000000000000000000000CCCCC300000000000000000000000000000000000000000000000000000000000000000000000000000000001448900000000000000000000042240000000004224000000000005;UD>DBF800;UUUUUUUUH70000:=UD>DUI<0000;UUUUUUUUUU;9000000000005;UD>DBF800;UUUB45BUUUB400003IUUUUUUUUG00000;UUUB46=UUU;90;UUUB46=UUU;90000;UUUUH70:UUUU=80000001[>>@>>I00000001[>>@>>I000000001[>>@>>I000000001[>>@>>I0000001[>>@>>I000000<UUUUUUUU[200FUUUU<02[UUU;00EIUD>DU;5000000003IUUUUUUUUG00003IUUUUUUUUG000000000;UUUB46=UUU;900000000000E=@>DUH200000000000000CA:<00000;UUUUU[IA30"	\
                "00000000<""::100<"":::<00000000000<"":::<000000008FGGGGGGG:00000001EFF:<3000000000<IUD>@=J700000009A:A90000004U>>?<00000004U>>?<000000000E?QYVNO]W=7CUTTTTTTTT@60004DSTVNVTKDA00CUTTTTTTTTTT?10000000000E?QYVNO]W=7CUTTTL<BQTTTLE00004DTTTTTTTY[3000CUTTTL<<PTTT?1CUTTTL<<PTTT?1000CUTTTT>23B]TTTR<0000004DVVM_Y[70000004DVVM_Y[700000004DVVM_Y[700000004DVVM_Y[7000004DVVM_Y[700000FQTTTTTTTW:07BSTTK=CIQTT]U37BKTVNVYQ[400000004DTTTTTTTY[30004DTTTTTTTY[30000000CUTTTL<<PTTT?10000000000JDSONVYQ@A0000000000000FMW@4000CUTTTTTYKLH700000004@WWG0ZUWWWUC000000000ZUWWWUC0000000FWQQQQQQQ>500007:ULXQW>=E0000000:DQTNNOSP;3000000EPWPE0000005D]_K=10000005D]_K=10000000<@S^efbcfTU3CUTddeeeeO@60004DVcaabdb]U60CUTddeeeeddO?1000000000<@S^efbcfTU3CUTd`L=X_eNWH300004DNddeeddY[3000CUTd`L<<P_dO?1CUTd`L<<P_dO?1000CUTddaXG<PNdd_R<000"	\
                "0004DVVM_Y[70000004DVVM_Y[700000004DVVM_Y[700000004DVVM_Y[7000004DVVM_Y[700000FQbdeeeeaW:00JPVdNM:PVdNL:07BYcaafe`X;00000004DNddeeddY[30004DNddeeddY[30000000CUTd`L<<P_dO?1000000000ZUSbNcV`a]D<00000000001:=QNDG400CUTddeeecVRJ00002FFFM_WF08@VaO?10000000008@VaO?10000000GQYYYYYYN>5000CHL]_aa^NKD:00000EDYadccecOMF000000EM^ME0000000AD]NMF0000000AD]NMF0000007=Kad_SQKTTU3CUTd`YYYYY@60004DTKXWK_g_P<0CUTd`YYYY_dO?1000000007=Kad_SQKTTU3CUTd`LWNdNWI1000004DNdNYY^dY[3000CUTd`L<<P_dO?1CUTd`L<<P_dO?1000CUTdeeYU;Qafg_R<000000<URR>RRI<000000:URR>RRI<0000000<URR>RRI90000000<URR>RRI900000<URR>RRI300000FQbfYYYYYW:007IQ^f]UScaK=307BYKWX]ce]B70000004DNdNYY^dY[30004DNdNYY^dY[30000000CUTd`L<<P_dO?1000000000<RTTTaWWNaK=7000000005BMSTN]WU50CUTd`]]NecKH0009=XXXQN><08@VdO?5<400000008@VdO?1ZZZZ000GQ][[[[>N>5009;MV`]QX"	\
                "KTaYDE0003=SbeVKKTcfY?40003:[M^M?I4000004BDWW[<0000000;DWWB4000002>Vd^QBGHDKU3CUTd`L[[[[;9000C=?HF:UTd_L<0CUTd`L[[P_dO?FFFJZ00002>Vd^QBGHDKU3CUTd`MNe_Q=80000004DNdN>M^dY[3000CUTd`LFFP_dO?1CUTd`LFFP_dO?1000CUTdN`_L?Tc]e_R<00000E>QYVNVTQDF90004DSTVNVTKDA00000:DQTNNOSP;300000:DQTNNOSP;3000<>KTNNOSLI60005FQbfS[[[[B50002@Yf^X`eTD<000;DIFFM^eY[70000004DNdN>M^dY[30004DNdN>M^dY[FFFE1000CUTd`LFFP_dO?FFFJZ000001H?LN_LIQaOD500000006BQ_ffff`LE0CUTd`LBQbfSI0003B]b_]YU108@VdO?>PDH1000008@VdO?:?""?""?""?G00GQ]=775>N>500E>ONWUUU=@K^SBC00<>Nd`WI;PVd^W:007;L]NbNTW:00000:MTTTQG000000CUTTTL<00000:W^dT@5002HJ0CUTd`XXXXW@A0000C:"">RLXNfT@40CUTd`L<<P_dOXXXXM[2000:W^dO@5002HJ0CUTd`Oe^QW@A0000005>NdNDM^dY[3000CUTd`XXXX_dO?1CUTd`XXXX_dO?1000CUTdNTbKM`_]e_R<00000AWaecbfeaYP:0004DVcaabdb]U6000EDYadccecOLF0000EDYadccecOLF000E"	\
                "M^fccecNWI709=WQbfSWWW=700000FM_eNe`WG00007A>RLSbaKH00000004DNdNDM^dY[30004DNdNDM^dYXXXXLB400CUTd`XXXX_dOXXXXM[2000000EM`VD<>N`ME5E<3000FW_d`]N]_LE0CUTd`L=QbcKH003=?]e_VK?G08@VdOKV_OW;000008@VdO?DYOOK=00GQ]=005>N>500IK^XUQTYQ]W]NPE00:W^dT?87=SfcKH00AROffb`aW:00009=Scde][C00000CUTd`L<00000FQbe]B9000000CUTdfbbbb^]@50000FQ_`aa]LG00CUTd`L<<P_gbbbba_K=300FQbe]B9000000CUTdeegcb^]@500000<P_dO@M^dY[3000CUTdfbbbbcdO?1CUTdfbbbbcdO?1000CUTdNKbOScY]e_R<00000AW_YKQSNec]U8004DTKXWK_g_P<003=SbeVKKTcfY?4003=SbeVKKTcfY?400EM_]QKYbeVRJ07[Yaedaaa]B9000001BSbgc]U800000FX_`bVK@<00000005>NdVDM^dY[30005>NdVDM^dbbbba_QI90CUTdfbbbbcgbbbba_K=300000FXaY[8@VaWFDL>=40CUYfbKQNDWLE0CUTd`]]NfVLJ004DOOe`^O]B98@VdO^^feTD400008@VdO@]b^KU<00GQ]=005>N>508@TT?X`NOTaWW^XF00FQbe]B90GQbfS=09=SccY^MKW:00004DOd^eNP<00000CUTd`L<0000"	\
                "0FQbe]=9000000CUTdb```bg^XF0000FQ__^f^]?40CUTd`L<<P_d^```fdN><00FQbe]=9000000CUTdb```bg^XF00000EM`dTUM^dY[3000CUTdb````bdO?1CUTdb````bdO?1000CUTdNRN^VaQ]e_R<00000AWMUHG=MNd`MA00C=?HF:UTd_L<00<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:00EMM[;;DYfcKI07[Y^fd^^^]B900000C=KbgcS[Z00000FX__acNX;0000000<P_dO?M^dY[3000<P_dO?M^d````fdOD50CUTdb````bd^```fdN><00000IScKI4DV^MPT`NQI95>NdN>QNDHIZ0CUTddeef]MH3004DVbddbb]B98@VdbSWTe`LE00008@VdOSa^KU<000GQ]=005>N>50<P_KUY`WDYaWP_KH00FQbe]=00GQbe]=01UTd_M^MBU<0000:M`cSbaQG00000CUTd`L<00000FWadT@5000000CUTd`LLL]ffSI0003A>PPWOd`W:0CUTd`L<<P_dOLLMOd^WA00FWadYU100CE40CUTd`LLL]ffSI00000FXaf]=M^dY[3000CUTd`LLLL_dO?1CUTd`LLLL_dO?1000CUTdN>]ffN>]e_R<00005I======BSfcKH000C:"">RLXNfT@400:W^dT?87=SfcKH00:W^dTWWWWSfcKH001G[PPPPW^dTU19=WQbfSWWW=900000FMNd`e_WG00000A>PLK^eT?6000000EM`dY"	\
                "UM^dY[3000EM`dYUM^dYLLMNd`ME0CUTd`LLLL_dOLLMOd^WA000<;UYaXFEL__P]a]NOD4<P_dO?QND4000CUTd`]TbcTD2008@X]ee]XX=98@VdN>ISf^MJ00008@VdO^aS?<0000GQ]=005>N>50EL`Q?ONDISaWR_K;00FXaeY[C0HKcfKI06@OdNM^MEZ00009=Sc^W_e][C0000CUTd`L<00000<PNd^QBGGGG50CUTd`LEGQbfSI004B=J2<ISfbQF0CUTd`L<<P_dO?EB]eaW:00<P_d_MG<J=L=7CUTd`LEGQbfSI0001EU]fbQ;M^dY[GG40CUTd`LEEP_dO?1CUTd`LEEP_dO?GG50CUTdN>W^f]B]e_RGGE00FQSSSSSSSScfS=0000FQ_`aa]LG000FQbe]B90GQbfS=00FQbdaaaaaadfS=0008@V____bdVD404FQbfSIAA4000005?]f^Q`fYD20000022<DNdN>GG<004GBKbfS=M^dY[304GBKbfS=M^dY[EUTd^ME0CUTd`LEEP_dO?EB]eaW:00EDQKV_LEHQbTPNT@]_P22P_dVDQND<<00CUTd`LDYeaQ;000EB]ebQ;F208@VdO?HKf^MJ00008@VdcdVMG90000GQ]=005>N>50<L`Q?ON>=SaWQ^WF00EL_dNP:E@TeaXF01?TdNM^M;I40004DOdVDTeNP<0000CUTd`L<00000C[Scd_SQQQQH0CUTd`L<FXbfSI00EMKM>DLTeaXF0CUTd`L<<P_dO?1=]eaW:00"	\
                "1U]cfTWPMKTU3CUTd`L<FXbfSI009ILK`d_LEM^dYQQXG0CUTd`L<<P_dO?1CUTd`L<<P_dOQQQH0CUTdN>DOOWB]e_QQQUC0FXbfffffffdf]=0000FQ__^f^]?400FQbe]=90GQbf]=00FQbeVVVVVVcf]=0008@VccccfdND400FQbfSLL?400009HX^fY@]caQI70000004DNdNQQQB90GXS_daXFM^dY[30GXS_daXFM^dY[3[Yd^ME0CUTd`L<<P_dO?1=]eaW:09=Sa^cOPI>ObKXaS[]N>54@Od`WQNDD><0CUTd`LGX^eT?4009B]e_LE0008@VdO?HKf^MJ19008@VdO_fTPF1300GQ]=005>N>505>NSBSaKWNaXVTD200ZU]fc]MLK^eO>209=SfbK^MWW:000:M`f]BKcaQG0000CUTd`L<000000APTcdfbbbSI0CUTd`L<FXbfSI00EM``NN_ffO><0CUTd`L<<P_dO?1=]eaW:000:ROce^_`bTU3CUTd`L<FXbfSI007[Ybdb]UEM^gcbbK;0CUTd`L<<P_dO?1CUTd`L<<P_gcbbSI0CUTdN>H?""?[B]efbbTUC0:W^dY]]]]]ffSI0003A>PPWOd`W:00FWaeYUC0HKcfSI00FWaeYDDDDKcfSI000<?QQQQQ^dO@600FQbe``_P20000E>Oe_MFLNdNRA0000004DNdcbb]B90;KceaYDEM^dY[30;KceaYDEM^dY[3[Yd^ME0CUTd`L<<P_dO?1=]eaW:0CUTVKccVSTcT>K"	\
                "cTQ_YU19=KbfTQNL]ME0CUTd`L<DTd^WF009B]e_P<<708@VdO?HKf^M?""?;008@VdOK^cY>?=C0GQ]=005>N>50CU]NPLOba]b^TMG0000FMVfc`_be_XH900:MNea^V`W:009=ScaQQQ`e][C000CUTd`L<0000009G>SO__ffSI0CUT__LE;QbfSI00EMV`aba_TMH90CUT__L<<P__O?E[YeaW:0007;L]_ab^NS[3CUT__LE;QbfSI007[Y^NSDJEM___fcK;0CUT__L<JL`dTU1CUT__L<<P___ffSI0CUT_N>511CB]__adTUC0<PNdNP==@TeaXF004B=J2<ISfbQF00EL_dNP:E@TeaXF00EL_dNP:E@TeaXF00<>DGGGUScf]B700FQ___d_P20003BS__SB<=Kac]U1000004DN__ce]B90;KNYQD::W^eY[70;KNYQD:EM__Y[EUTd`ME0CUT__L<<P__O?E[YeaW:09IK`b_S_a^TL[Y_NaVMF00E>Tcf___`ME03UTd^MAHQ__]U109B]e`MLPF08@VdO?HKfaOOTU308@VdO?SbbOOD40GQ]=005>N>500GW`]@PQQXQWDG300003HMY_aaeTWB4000CIWT^fa_W:004DOdecccfdNP<000CUTd`L<00000009E=DPQbfSI07=RRRDMWTebQ;004=DMWSYL?GC007=RRRD25@RRRBMX_d_LE00007:URWSY>BE07=RRRDMWTebQ;007BXM>BE0<DRRTfOLF07=RRRD>M]cf]B77=RR"	\
                "RD25@RRYfVMG07=RRRU8000IRRKa_QB301U]fc]MLK^dO><00EMKM>DLTeaXF00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><00EMYXRLK`d`WF000:PRRYeN>20003BRRR>EDMKadT?Z00000ZURRW^e]B90FP>UHAPM]ffS=00FP>UH<0<DRRRUMQ`dN>207=RRRD25@RRRBMX_d_LE002UMXM[LXWDGDVOMXPIC000FPS_ab`TLE09=Sfc]MPDRRR[100HKcf``K;08@V_O?HK__^dTU308@V_O?DY`dND40GQ]=EEE>N>5004USa]M@U>QNRA000000CF?LWKa`Q=100006;DM^XRU<00:M`eNNNNNcaQG000CUTd`L<00000000074FQbfSI008<<<;K^feVPE0007AMMW_L<000008<<<2005<<<UTaec]U6000000AMMW_L<0008<<<;K^feVPE0002FA230002U]bTLG3008<<<JM^cd`WG008<<<2005USaOMHC008<<<400001IX`_QB5000FMVfc`_be_XH900EM``NN_ffO><000FMVfc`_be_XH9000FMVfc`_be_XH900EM^a__bd^K[80009FXKNdO@400008<<<22P_bdaQI00000004<AW^e]B907<510FX^cd_M:007<5100002<<<?OadbS[C008<<<2005<<<UTaec]U60008AFJ1EF:5EL_]BFE700009A[RQNM?G300FW_dc^WA<<<4000GQ____K;01[RRRBGRRRNdTU301[R"	\
                "RRB:"">TdND40GQ]MMMMMN>5000E@S`_OTNaOWG000000008E:@]cNW;000000EM^ME1009=ScbQ>>>W^e][C00CUTd`L<00000000000FQbcSI000000;Ka`TWI70000AW`^O>20000000000000003UTa_]P:0000000AW`^O>20000000;Ka`TWI70000000000C=Q^TRG70000000JM^^NK[4000000007IX`OM;3000000000000FLV_XB40000CHMY_aa`OXB4000EMV`aba_TMH90003HMY`aa^OXB400003HMY`aa^OXB4000EMY_aa^VQU<00000FQbf`QI00000000002P_^NK?<000000000AW^c]B9000000FXa^VQBZ0000000000000Z?Oa_]>J00000000000003UTa_]P:0000000000000FXaKH0000000001GQND400006BQV^aWA0000007B]TPRRPF004<<<83<<>NaTU3004<<<8CUTaND40GQVVVVVVV>50000EUW]TOYK>H600000000000A>KKKUC00000EMSME0004DO_NPE45DO_NP<00CUT__L<00000000000FXQQQH000000;WWMD;Z00000EDMW>;90000000000000003[WWLU:90000000EDMW>;90000000;WWMD;Z000000000004DQQPF700000000ELWW>=20000000008@QQL;30000000000000=QQX=4000000CF?LXXMDH400004=DMWXWL?GC0000"	\
                "0CF?MXXMDH4000000CF?MXXMDH40000CG?LWXM>I2000000FQSSW[500000000005>WWPB<0000000000AMQQQB9000000:MWM>I4000000000000001UWWLUA000000000000003[WWLU:90000000000000;WSWF0000000000GXKD4000005I>WWLJ0000008DSKI<<<70000000004DXXX[300000003[XXX@40J@DDDDDDD[6000008FB?@UHE90000000000000:;;;E0000001;I;10004?RRR[107=RRRD<007=RRRD2000000000008GGGG50000006::J5000000005EA<700000000000000000<""::E800000000005EA<7000000006::J500000000000000:GG:90000000007A:A2700000000000AGGF300000000000000<GGG40000000008EFFA5000000075JFF:E40000000008EFFA500000000008EFFA5000000008EFFA5900000005IIIF8000000000000E::<C00000000000CFGGG<00000003::A270000000000000000<""::EZ0000000000000000<""::E80000000000000008FI=400000000004;;:000000072A:A30000000FII<00000000000000EFFF<000000000<FFFE00044444444100000000716C0000000000000000000000000000000"	\
                "000005<<<40006<<<200008<<<200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000090000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000522Z00000000001222300000000000421524000000000822422C0000000000014Z000000000422400000000000522Z00000012223000000822422C000000522Z0000122230000822422C00000421524000000000000522Z00000000000000000000000009900900000000052405240000000000000000000424052600"	\
                "0000C222900000005240524000000012223000000012223000000000042152400000000000424052600C2229000000522Z0000000522Z0000000522Z000000005240524000000000522Z0000000524052400000005240524000000000000122230000000000000122230000000000822422C00000000000522Z0000822422C00000000:"">>>B3000000001=>>>;9000000000:@>UDP[Z00000001[>>@>>I0000000000JU>UF00000004U>>?<000000000:"">>>B300001=>>>;900001[>>@>>I00000:"">>>B3001=>>>;9001[>>@>>I0000:@>UDP[Z0000000000:"">>>B300000005:::::<000000009GB=2IBHC000000A>>?F>>@20000005:::::<00005?>@A>>BC00000H>>>F000000A>>?F>>@2000001=>>>;9000001=>>>;900000000:@>UDP[Z05:::::<05?>@A>>BC0H>>>F00000:"">>>B300000:"">>>B300000:"">>>B3000000A>>?F>>@20000000:"">>>B300000A>>?F>>@200000A>>?F>>@200000000001=>>>;9000000000001=>>>;9000000001[>>@>>I0000000000:"">>>B3001[>>@>>I00000005U]_K[C00000000GMNNOP:00000"	\
                "0004?YNSXVD400000004DVVM_Y[700000000<D]NY>J0000005D]_K=100000005U]_K[C0000GMNNOP:00004DVVM_Y[70005U]_K[C00GMNNOP:004DVVM_Y[7004?YNSXVD40000000005U]_K[C0000007BWWWWW?600000006@]QIM]P<000000:MNSPYT><000007BWWWWW?60002>NQRSTU100009B]_K;000000:MNSPYT><00000GMNNOP:00000GMNNOP:00000004?YNSXVD47BWWWWW?62>NQRSTU19B]_K;00005U]_K[C00005U]_K[C00005U]_K[C000000:MNSPYT><0000005U]_K[C00000:MNSPYT><00000:MNSPYT><0000000000GMNNOP:00000000000GMNNOP:000000004DVVM_Y[7000000005U]_K[C004DVVM_Y[7000009IQ^KU<00000000J>TYMO]?50000000<L_QO`][C00000004DVVM_Y[700000000;QNKVSI0000000AD]NMFCC0000CIQ^KU<C000J>TYMO]?50004DVVM_Y[C009IQ^KU<00J>TYMO]?504DVVM_Y[700<L_QO`][CC00003CCCIQ^KU<CC000007[YaaaVD800000003[]OSTOD4000000CIX`TNMG0000007[YaaaVD8000CBK__NXHC0000CB]_K;CC0000CIX`TNMG0000CJ>TYMO]?5000J>TYMO]?5000000<L_QO`][C"	\
                "7[YaaaVD8CBK__NXH09B]_K;0009IQ^KU<00009IQ^KU<000CCIQ^KU<CC0000CCIX`TNMGC0000CCIQ^KU<000000CIX`TNMG000007CIX`TNMGCC9000CCC7J>TYMO]?5CCC00003CJ>TYMO]?5C00003C4DVVM_Y[CC00003CCIQ^KU<C004DVVM_Y[7000004BWWU[<00000000:MW>ULW><0000000<>WD>WDA00000000ZURRDRRI900000000IKTU]]B900000;UUDWWUUUG00;UUWWUUUUG00;MW>ULW>UG00;URRDRRUUG07BWWUB400:MW>ULW><0ZURRDRRI90;U>WD>WDUUU;902[UUUBWWUUUUUF00007[QQQQQ@800000000FR]OYWH000000;UUUWWWIJC00000;UQQQQQ@UG00;UULWPUUUG00;UURRPUUUG00;UUUWWWUUG00;UUMW>;LW>U;90:MWWWWW><000000<>WD>WDA07[QQQQQ@80<ULWPB400IRRPB400;UWWU<00000ZUWWWUC00;UUUWWUIUUU;90;UUUUWWWUUU;90;UUUWWUIA30000;UUUWWWIA30001=UUUWWWUUUUH90FUUUB:MW>ULW>[UUUA002[UUMW>;LW>UUF002[UUURRDRRUUUF00<[UUUWWUUUUG0ZURRDRRI900000:MTTTQG000000003:MTTTQG000000000:MTTTQG000000000:MTTTQG000000000FW^Y_QH00000CUTTTTTTTY[7CUTT"	\
                "TTTTTY[7CUTTTTTTTY[7CUTTTTTTTY[7CUTTTL<003JUTTTLE0004UTTTL<0CUTTTS=ADTTT?10AMTTTLAA[STTSB90000:MTTTQG000000000:MTTTQG00000CUTTTTY]KL=5000CUTTTTTTTY[7CUTTTTTTTY[7CUTTTTTTTY[7CUTTTTTTTY[7CUTTTLE<PTTT?103J@VaO?E00000000EUTTTL<00<UTTTLJ00CUTTTL<0CUTTTL<0CUTTTL<000008@VaO?10CUTTTS=2DTTT?1CUTTTS=ADTTT?1CUTTTTTYKLH700CUTTTTTYKLH7002>TTTTTTTTTT?19B]TTMJIKTTQGFWTTKI00AMTTTLA6[STTSB90AMTTTLA2[STTSB90:WTTTTTTTTY[304UTTTL<000009=Scde][C00000009=Scde][C00000009=Scde][C00000009=Scde][C00000009=Scde][C0000CUTddeeeeY[7CUTddeeeeY[7CUTddeeeeY[7CUTddeeeeY[7CUTd`L<000CUTd`L<000CUTd`L<0CUTddNPEDNdO?101[Scc]UFW_dVPJ00009=Scde][C00000009=Scde][C0000CUTddedecNQ[400CUTddeeeeY[7CUTddeeeeY[7CUTddeeeeY[7CUTddeeeeY[7CUTd`L<<P_dO?1008@VdO?5<4000000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`L<000008@VdO?10CUTddNPEDNdO?1CUTd"	\
                "dNPEDNdO?1CUTddeeecVRJ00CUTddeeecVRJ002>_eeeddeeeO?10;QbbQG[YefS=IKcaWF001[Scc]UFW_dVPJ001[Scc]UFW_dVPJ00:WaeeeedddY[30CUTd`L<000004DOd^eNP<00000004DOd^eNP<00000004DOd^eNP<00000004DOd^eNP<00000004DOd^eNP<0000CUTd`YYYYY[7CUTd`YYYYY[7CUTd`YYYYY[7CUTd`YYYYY[7CUTd`L<000CUTd`L<000CUTd`L<0CUTdgbK=DNdO?100JPVd_W@YfaQI700004DOd^eNP<00000004DOd^eNP<0000CUTd`YYVbd`X;00CUTd`YYYYY[7CUTd`YYYYY[7CUTd`YYYYY[7CUTd`YYYYY[7CUTd`L<<P_dO?1008@VdO?>PDH10000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`L<000008@VdO?10CUTdgbK=DNdO?1CUTdgbK=DNdO?1CUTd`]]NecKH00CUTd`]]NecKH002>YYYYfbYYYY?10AM^fS=DVbcT?[Ye_R<000JPVd_W@YfaQI7000JPVd_W@YfaQI700:WYYYYYOdaKB70CUTd`L<00000:M`cSbaQG0000000:M`cSbaQG0000000:M`cSbaQG0000000:M`cSbaQG0000000:M`cSbaQG0000CUTd`L[[[[F0CUTd`L[[[[F0CUTd`L[[[[F0CUTd`L[[[[F0CUTd`L<000CUTd`L<000CUTd`L<0CUTd"	\
                "^bV>DNdO?1007IQafYQ^eTD<00000:M`cSbaQG0000000:M`cSbaQG0000CUTd`LU>SbeT@40CUTd`L[[[[F0CUTd`L[[[[F0CUTd`L[[[[F0CUTd`L[[[[F0CUTd`LFFP_dO?1008@VdOKV_OW;0000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`L<000008@VdO?10CUTd^bV>DNdO?1CUTd^bV>DNdO?1CUTd`LBQbfSI00CUTd`LBQbfSI00CI[[[SfbQ[[[G002>NdT?M`N_NPDVdO@40007IQafYQ^eTD<00007IQafYQ^eTD<0005B[[[[KafYD<00CUTd`L<00009=Sc^W_e][C000009=Sc^W_e][C000009=Sc^W_e][C000009=Sc^W_e][C000009=Sc^W_e][C000CUTd`XXXXWG0CUTd`XXXXWG0CUTd`XXXXWG0CUTd`XXXXWG0CUTd`L<000CUTd`L<000CUTd`L<0CUTdNNbKDNdO?10002@Yf^Od`WG000009=Sc^W_e][C000009=Sc^W_e][C000CUTd`L<E>Nd`LE0CUTd`XXXXWG0CUTd`XXXXWG0CUTd`XXXXWG0CUTd`XXXXWG0CUTd`XXXX_dO?1008@VdO^^feTD4000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`L<000008@VdO?10CUTdNNbKDNdO?1CUTdNNbKDNdO?1CUTd`L=QbcKH00CUTd`L=QbcKH000077ISfbQF770001UTdNPQbYO^"	\
                "WM`e]B700002@Yf^Od`WG0000002@Yf^Od`WG00000777HX`eVLF000CUTd`L<00004DOdVDTeNP<000004DOdVDTeNP<000004DOdVDTeNP<000004DOdVDTeNP<000004DOdVDTeNP<000CUTdfbbbbK;0CUTdfbbbbK;0CUTdfbbbbK;0CUTdfbbbbK;0CUTd`L<000CUTd`L<000CUTd`L<0CUTdNKcO>NdO?10000GW_dec]U8000004DOdVDTeNP<000004DOdVDTeNP<000CUTd`L<3UYdaW:0CUTdfbbbbK;0CUTdfbbbbK;0CUTdfbbbbK;0CUTdfbbbbK;0CUTdfbbbbcdO?1008@VdbSWTe`LE000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`L<000008@VdO?10CUTdNKcO>NdO?1CUTdNKcO>NdO?1CUTd`]]NfVLJ00CUTd`]]NfVLJ000000ISfbQF000000=Sf^WSfS]cKQbcKH000000GW_dec]U80000000GW_dec]U80000000FLVe`XH7000CUTd`L<0000:M`f]BKcaQG00000:M`f]BKcaQG00000:M`f]BKcaQG00000:M`f]BKcaQG00000:M`f]BKcaQG000CUTdb````K;0CUTdb````K;0CUTdb````K;0CUTdb````K;0CUTd`L<000CUTd`L<000CUTd`L<0CUTdNRNaQNdO?100006[ScdNLA000000:M`f]BKcaQG00000:M`f]BKcaQG000CUTd`L"	\
                "<3[YeaW:0CUTdb````K;0CUTdb````K;0CUTdb````K;0CUTdb````K;0CUTdb````bdO?1008@VdN>ISf^MJ000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`L<000008@VdO?10CUTdNRNaQNdO?1CUTdNRNaQNdO?1CUTddeef]MH300CUTddeef]MH3000000ISfbQF000000GQbcKTaXKc]Sf^W:0000006[ScdNLA000000006[ScdNLA0000000<DYcaK[40000CUTd`L<0009=ScaQQQ`e][C0009=ScaQQQ`e][C0009=ScaQQQ`e][C0009=ScaQQQ`e][C0009=ScaQQQ`e][C00CUTd`LLLLLF0CUTd`LLLLLF0CUTd`LLLLLF0CUTd`LLLLLF0CUTd`L<000CUTd`L<000CUTd`L<0CUTdN>ScONdO?100000AWae]B3000009=ScaQQQ`e][C0009=ScaQQQ`e][C00CUTd`L<5DOd^MJ0CUTd`LLLLLF0CUTd`LLLLLF0CUTd`LLLLLF0CUTd`LLLLLF0CUTd`LLLL_dO?1008@VdO?HKf^MJ000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`L<000008@VdO?10CUTdN>ScONdO?1CUTdN>ScONdO?1CUTd`]TbcTD200CUTd`]TbcTD2000000ISfbQF000000EM`e]N_LW^OTdN><0000000AWae]B3000000000AWae]B30000008[KafYD<00000CUTd`L<00"	\
                "04DOdecccfdNP<0004DOdecccfdNP<0004DOdecccfdNP<0004DOdecccfdNP<0004DOdecccfdNP<00CUTd`LGGGGE0CUTd`LGGGGE0CUTd`LGGGGE0CUTd`LGGGGE0CUTd`L<000CUTd`L<000CUTd`L<0CUTdN>LNaNdO?100000AWae]B9000004DOdecccfdNP<0004DOdecccfdNP<00CUTd`L;[Q^dV>50CUTd`LGGGGE0CUTd`LGGGGE0CUTd`LGGGGE0CUTd`LGGGGE0CUTd`LEEP_dO?1008@VdO?HKf^MJ000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`LGGGGE08@VdO?10CUTdN>LNaNdO?1CUTdN>LNaNdO?1CUTd`LDYeaQ;00CUTd`LDYeaQ;000000ISfbQF0000004DVdO^OD>N_NdT?Z0000000AWae]B9000000000AWae]B9000007HW_dVLGGGGJ00CUTd`L<000:M`eNNNNNcaQG000:M`eNNNNNcaQG000:M`eNNNNNcaQG000:M`eNNNNNcaQG000:M`eNNNNNcaQG00CUTd`QQQQQ?6CUTd`QQQQQ?6CUTd`QQQQQ?6CUTd`QQQQQ?6CUTd`L<000CUTd`L<000CUTd`L<0CUTdN>[ScddO?100000AWae]B900000:M`eNNNNNcaQG000:M`eNNNNNcaQG00CUTd`QQ]`daK=70CUTd`QQQQQ?6CUTd`QQQQQ?6CUTd`QQQQQ?6CUTd`QQQQQ?6CUTd`L<"	\
                "<P_dO?1008@VdO?HKf^MJ000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTd`QQQQQ?68@VdO?10CUTdN>[ScddO?1CUTdN>[ScddO?1CUTd`LGX^eT?40CUTd`LGX^eT?400000ISfbQF0000003UYebf][?TfcfS=90000000AWae]B9000000000AWae]B900000:ROe`QQQQQQ?80CUTd`L<009=ScbQ>>>W^e][C09=ScbQ>>>W^e][C09=ScbQ>>>W^e][C09=ScbQ>>>W^e][C09=ScbQ>>>W^e][C0CUTdfbbbbO@6CUTdfbbbbO@6CUTdfbbbbO@6CUTdfbbbbO@6CUTd`L<000CUTd`L<000CUTd`L<0CUTdN>:M_ddO?100000AWae]B900009=ScbQ>>>W^e][C09=ScbQ>>>W^e][C0CUTdfbcfea]D<00CUTdfbbbbO@6CUTdfbbbbO@6CUTdfbbbbO@6CUTdfbbbbO@6CUTd`L<<P_dO?1008@VdO?HKf^MJ000CUTd`L<00CUTd`L<00CUTd`L<0CUTd`L<0CUTdfbbbbO@68@VdO?10CUTdN>:M_ddO?1CUTdN>:M_ddO?1CUTd`L<DTd^WF0CUTd`L<DTd^WF00000ISfbQF0000000IKcgcKH=SfgbQG00000000AWae]B9000000000AWae]B900000;KcgfbbbbbV@80CUTd`L<004DO_NPE45DO_NP<04DO_NPE45DO_NP<04DO_NPE45DO_NP<04DO_NPE45DO_NP<0"	\
                "4DO_NPE45DO_NP<0CUT______O@6CUT______O@6CUT______O@6CUT______O@6CUT__L<000CUT__L<000CUT__L<0CUT_N>5[]__O?100000AW__]B900004DO_NPE45DO_NP<04DO_NPE45DO_NP<0CUT____NTQ@A000CUT______O@6CUT______O@6CUT______O@6CUT______O@6CUT__L<<P__O?1008@V_O?HK__MJ000CUT__L<00CUT__L<00CUT__L<0CUT__L<0CUT______O@68@V_O?10CUT_N>5[]__O?1CUT_N>5[]__O?1CUT__L<HQ__]U1CUT__L<HQ__]U10000IS__QF0000000FW___W:GQ___MJ00000000AW__]B9000000000AW__]B900000;K________V@80CUT__L<004?RRR[107=RRRD<04?RRR[107=RRRD<04?RRR[107=RRRD<04?RRR[107=RRRD<04?RRR[107=RRRD<07=RRRRRRRR[17=RRRRRRRR[17=RRRRRRRR[17=RRRRRRRR[17=RRRD20007=RRRD20007=RRRD207=RRRU8FPRRRB300000E>RRRI000004?RRR[107=RRRD<04?RRR[107=RRRD<07=RRRRP>UH<00007=RRRRRRRR[17=RRRRRRRR[17=RRRRRRRR[17=RRRRRRRR[17=RRRD25@RRRB3001[RRRBGRRR><0007=RRRD2007=RRRD2007=RRRD207=RRRD207=RRRR"	\
                "RRRR[11[RRRB307=RRRU8FPRRRB37=RRRU8FPRRRB37=RRRD2<DRRR[17=RRRD2<DRRR[10000;RRR>:0000000<DRRRD<E>RRR?500000000E>RRRI0000000000E>RRRI000000FPRRRRRRRRR[107=RRRD20005<<<40006<<<20005<<<40006<<<20005<<<40006<<<20005<<<40006<<<20005<<<40006<<<20008<<<<<<<<4008<<<<<<<<4008<<<<<<<<4008<<<<<<<<4008<<<2000008<<<2000008<<<20008<<<407<<<<80000000<<<<10000005<<<40006<<<20005<<<40006<<<20008<<<<<5100000008<<<<<<<<4008<<<<<<<<4008<<<<<<<<4008<<<<<<<<4008<<<2005<<<800004<<<83<<<2000008<<<200008<<<200008<<<20008<<<20008<<<<<<<<4004<<<80008<<<407<<<<8008<<<407<<<<8008<<<2002<<<4008<<<2002<<<4000003<<<<9000000002<<<200<<<<50000000000<<<<100000000000<<<<10000007<<<<<<<<<<40008<<<200000000C22290000000052405240000000822422C0000000042240000000822422C000000000522Z000822422C0000000522Z00000000042240000000000424052"	\
                "6000000CCC7007CCC0000003CCCCCCCC0000004240526000000CCC7007CCC000000CCC700CCC3003CCC0000000522222300000000000000009000000000000000000000000CCCC000CCC3007CCC70000CCC7009CCCC0000CCC7007CCC0000007CCCCCCCCCC9000CCC30003CCC0007CCCCCCCCCCCCC7000009CCC700CCCC000000000000007854C000000000000042405260000000009CCC700CCCC0000000990090000000000822422C000000042405260000000000822422C0000000000000000000H>>>F0000000A>>?F>>@2000001[>>@>>I00000004U>>?<000001[>>@>>I00000000:"">>>B301[>>@>>I000000:"">>>B300000004U>>?<000000005?>@A>>BC0000;UUUB46=UUU;9000<[UUUUUUUU:00005?>@A>>BC0000;UUUB46=UUU;9000;UUUB4:UUUU<<UUUU:00000ED>>>>>;00000000000006FIBIA700000000000007EEE500EUUUUF0FUUU[21IUUU=Z00;UUUB43HUUUUE00;UUUB46=UUU;90001=UUUUUUUUUUH90FUUUU<02[UUU;01=UUUUUUUUUUUUU=Z0003IUUUICAUUUUE00000000000C:B@>DU;400000000005?>@A>"	\
                ">BC00000003IUUUICAUUUUE00009GB=2IBHC00000001[>>@>>I0000005?>@A>>BC000000001[>>@>>I0000000000000000009B]_K;0000000:MNSPYT><000004DVVM_Y[70000005D]_K=100004DVVM_Y[70000005U]_K[C04DVVM_Y[700005U]_K[C00000005D]_K=100000002>NQRSTU1000CUTTTL<<PTTT?1000FXTTTTTTTSI00002>NQRSTU1000CUTTTL<<PTTT?100CUTTTL<ISTTXFFQTTSI00000EPT_X_T>E00000000003;UWS]SRF0000000000000FLMMU10GQTTYDE=]TTW:;WTTT><0CUTTTL<GMTTTQG0CUTTTL<<PTTT?10002>TTTTTTTTTT?17BSTTK=CIQTT]U32>TTTTTTTTTTTTTP<0004DTTT>4HKTTQG000000C6616IM]ONVYXUE0000000002>NQRSTU100000004DTTT>4HKTTQG00006@]QIM]P<00000004DVVM_Y[7000002>NQRSTU1000000004DVVM_Y[70000000000000003CCB]_K;CC00003CIX`TNMGC000014DVVM_Y[3000000CAD]NMFC0004DVVM_Y[C0000CCIQ^KU<904DVVM_Y[700CCIQ^KU<CCC0000CCAD]NMFCC0000CCBK__NXHCC00CUTd`L<<P_dO?1000FXadeeedfSI0000CBK__NXHCC00CUTd`L<<P_dO?10"	\
                "0CUTd`L<ISfbXFFQbfSI00007CFPTOX_S[8543000000<PY_OYVT@400000000CCCCFQ^O@604[KabS?=]eaWFLOeNM;90CUTd`LFPTf^K[40CUTd`L<<P_dO?10002>_eeeddeeeO?10JPVdNM:PVdNL:02>_eeeddeeeOOd_P<0004DNdN>4HKfcQG000001B@@UHWNffbce^SU40000CCCCCBK__NXH07CCC7004DNdN>4HKfcQG00003[]OSTOD400000004DVVM_Y[700000CBK__NXHC0000CCCC4DVVM_Y[77CCC7000000000<[UUURRPUUUG00<[UUUWWWUUUG09;?UURRDRRU[F9000;UUUDWWUUG00;URRDRRUUG00;UUUWWUUUH7ZURRDRRI90;UUUWWUHUUUUE00;UUUIDWWUUU;90FUUUULWP[UUU;0CUTd`L<<P_dO?1000FXafYYYbfSI000;UUULWPUUUU;9CUTd`L<<P_dO?100CUTd`L<ISfbXFFQbfSI0001=UUURWLWW@@>D=100000<PYQDUMOP<0000000;UUUUUQbO@600<@]b^KB]eaW>Yf`Q=100CUTd`L>YcaSU200CUTd`L<<P_dO?10002>YYYYfbYYYY?107IQ^f]UScaK=302>YYYYfbYYYYOd_P<0004DNdN>5HKfcQG000005>OOW>Vea]QS_d^X;000EUUUUF<ULWP[21IUUU=Z04DNdN>5HKfcQG00000FR]OYWH00000000ZURRDRRI900000;UULWPU"	\
                "UUG00EUUUUFURRDRRI1IUUU=Z00000000:WTTTTTTTTY[30:WTTTTTTTTY[31UTT]W[F>KYYP:00CUTTTTTTTY[7CUTTTTTTTY[7CUTTTTTTTT@604UTTTL<0CUTTTLAGMTTTQG0CUTTT>E=KTTT?17B]TTKIA;QTTYU3CUTd`L<<P_dO?1000FXafS[XbfSI00CUTTT>A=KTTT?1CUTd`L<<P_dO?100CUTd`L<ISfbXFFQbfSI0002>TTT>AJA=QONV>5000CZ8[U;Q]]?[4ZZ3000CUTTTTTTbO@6000A>Tf_X]eaWSbaK[4000CUTd`L]bb]@E000CUTd`LFFP_dO?1000CI[[[SfbQ[[[G0002@Yf^X`eTD<00CI[[[SfbQ[[?Od_P<0004DNdN>FHKfcQG000004>NaXQbeY@GBKaeYU100GQTTYDE=]TTW:;WTTT><04DNdN>FHKfcQG00000:MTTTQG000000000:MTTTQG00000CUTTTTTTTY[70GQTTYDE=]TTW:;WTTT><00000000:WaeeeedddY[30:WaeeeedddY[31UTef`Q>TbecS=90CUTddeeeeY[7CUTddeeeeY[7CUTddeeeeO@60CUTd`L<0CUTd`LFPTf^K[40CUTdN>E>VddO?10:M`dOD5?Te^WG0CUTd`L<<P_dO?1000FXafS=XbfSI00CUTdN>E>VddO?1CUTd`L<<P_dO?100CUTd`L<ISfbXFFQbfSI0003B]f^WF0EPNddN>5004[?""?U<FQSTUE?""?""?=100CUTd"	\
                "deeeeO@60000FLVeN]eaK^b]?<0000CUTd`SacY>J0000CUTd`XXXX_dO?10000077ISfbQF7700000FM_eNe`WG0000077ISfbQF1?Od_P<0008@Od^KXXKfcQG00000CU]fTYebQWWWWNdN>5004[KabS?=]eaWFLOeNM;908@Od^KXXKfcQG00009=Scde][C00000009=Scde][C0000CUTddeeeeY[704[KabS?=]eaWFLOeNM;900000000:WYYYYYOdaKB70:WYYYYYOdaKB71U]Y`dNKbbVfY[70CUTd`YYYYY[7CUTd`YYYYY[7CUTd`YYYYY@60CUTd`L<0CUTd`L>YcaSU200CUTdN>IKbgdO?108UYe^XGW`dOD50CUTd`L<<P_dO?1000FXbfSIXbfSI00CUTdN>IKbgdO?1CUTd`L<<P_dO?100CUTd`L<ISfbXFFQbfSI0000FXafSB7HKbfVT>500GWOOMAFQTS=FWOOM:00CUTd`YYYYY@60000E>OefNeb_dbS[10000CUTd^^dbS[60000CUTdfbbbbcdO?10000000ISfbQF00000001BSbgc]U80000000ISfbQF1?Od_P<0007BScdbbbbdcQG000000FMNfadfaaaaabd_L<000<@]b^KB]eaW>Yf`Q=1007BScdbbbbdcQG00004DOd^eNP<00000004DOd^eNP<0000CUTd`YYYYY[700<@]b^KB]eaW>Yf`Q=10000000005B[[[[KafYD<005B[[[[KafYD<00GBU"	\
                "X_aOc]QNQ;00CUTd`L[[[[F0CUTd`L[[[[F0CUTd`L[[[[;90CUTd`L<0CUTd`L]bb]@E000CUTdN>>Oc^dO?100GX^eY?]fbKI00CUTd`L<<P_dO?1000FQbfKHXbfSI00CUTdN>>Oc^dO?1CUTd`L<<P_dO?100CUTd`L<ISfbXFFQbfSI0000<>NdVD5?Te_M?B101UYeT?8FQKDE5>VfSB70CUTd`L[[[[;90001BSbfadgge^d_MF0000CUTdge`d_WG0000CUTdb````bdO?10000000ISfbQF0000000C=KbgcS[Z0000000ISfbQF1?Od_P<0000:PT_````ecQG0000001=WTNdaVVVVVVVVME0000A>Tf_X]eaWSbaK[40000:PT_````ecQG0000:M`cSbaQG0000000:M`cSbaQG0000CUTd`L[[[[F0000A>Tf_X]eaWSbaK[400000000000777HX`eVLF0000777HX`eVLF000074[]ec_MBD[500CUTd`XXXXWG0CUTd`XXXXWG0CUTd`L<777000CUTd`L<0CUTd`SacY>J0000CUTdN>QaNNdO?1005DOd`MNdNPE00CUTd`L<<P_dO?1000GQccQ;XbfSI00CUTdN>QaNNdO?1CUTd`L<<P_dO?100CUTd`L<ISfbXFFQbfSI00003B]f^WFL_eT?6300<PNfS=95IHE03[YeVD40CUTd`L<77700000FM_dNSagd_KafY@2000CUTddNQaeTD<000CUTd`LLLL_dTU100000"	\
                "00ISfbQF0000000FMNd`e_WG0000000ISfbQF1?Od_P<00009F@LLLLKfcQG00000008;?Ye^WDDDDDDDU500000FLVeN]eaK^b]?<000009F@LLLLKfcQG0009=Sc^W_e][C000009=Sc^W_e][C000CUTd`XXXXWG00000FLVeN]eaK^b]?<00000000000000FLVe`XH7000000FLVe`XH7000000;KcdO?841000CUTdfbbbbK;0CUTdfbbbbK;0CUTd`L<000000CUTd`L<0CUTd^^dbS[60000CUTdN>OcSNdO?1000IKbf]bcSB300CUTd`LGGP_dO?10CF=SfaWGXbfSI:9CUTdN>OcSNdO?1CUTd`LGGP_dO?G20CUTd`LGISfbXGGQbfSIF1000FXafS=KbbKH0000:W^cK;0000000ISf`ME0CUTd`L<00000002@YfaKDYeaQDTeaQIG50CUTd`X@YfaK=G<0CUTd`LEEP_dTUGG500000ISfbXGG:90005?]f^Q`fTDGJ00000ISfbQGG?Od_PGGE00004EEEHKfcQGG:7000000IKcf]?A<JI>@4000000E>OefNeb_dbS[100000004EFGHKfcQG0004DOdVDTeNP<000004DOdVDTeNP<000CUTdfbbbbK;00000E>OefNeb_dbS[10000000000000<DYcaK[4000000<DYcaK[40000000FWaeY[300000CUTdb````K;0CUTdb````K;0CUTd`L<000000CUTd`L<0CUTdge`"	\
                "d_WG0000CUTdNQaNLNdO?1000EPNdcd`M:000CUTd`QQQQ_dO?10JMQ_dNQQQbfSQ>2CUTdNQaNLNdO?1CUTd`QQQQ_dOQQI0CUTd`QQQSfbQQQQbfSQWF000<>NdODYe_LJ0000GQbbXF8:HIHA1;KcaW:0CUTd`L<0000007IQ^eTD=]eaWGW`dVQQH0CUTd`LFM_dNQQ=9CUTd`L<<P_dTQQQH00000ISfbQQQ>2009HX^fY@]caQQ@80000ISfbQQQQOd_QQQ[30000000HKfcQQQP<000000ALNd^SMPMKO>2000001BSbfadgge^d_MF00000000EMQQKfcQG000:M`f]BKcaQG00000:M`f]BKcaQG000CUTdb````K;00001BSbfadgge^d_MF0000000000008[KafYD<0000008[KafYD<00000000AWae]B900000CUTd`LLLLLF0CUTd`LLLLLF0CUTd`L<000000CUTd`L<0CUTddNQaeTD<000CUTdNTcSDNdO?10003=ScgeYU8000CUTdfbbbbfdO?10AW^dgcbbbedb_P<CUTdNTcSDNdO?1CUTdfbbbbfgcbS=0CUTdfbbbbdebbbbedbbXF0003B]f^W_dT?60000;KcbXFUMKSKM[;KcaXF0CUTd`L<000000JPVd`W;=]eaW:UScfbKH0CUTd`L<BKbeb]B9CUTd`L<<P_gcbbKH00000ISfebb_P200E>Oe_MFLNdbVD80000ISfebbbbcgfbbTU30000000HKfdbb`L<0"	\
                "00000C=Q`ec`_`b_>200000FM_dNSagd_KafY@20000000JM^bbdcQG009=ScaQQQ`e][C0009=ScaQQQ`e][C00CUTd`LLLLLF0000FM_dNSagd_KafY@200000000007HW_dVLGGGGJ007HW_dVLGGGGJ00000AWae]B900000CUTd`LGGGGE0CUTd`LGGGGE0CUTd`L<000000CUTd`L<0CUTd`X@YfaK=C00CUTd_^_MDNdO?1000AGM_g^XG0000CUT___ab___O?10AW^a_______b_P<CUTd_^_MDNdO?1CUT_________fS=0CUT______________^bXF0000FXacSbbKH00000FXafSM]`^_^_SM]e^WA0CUTd`L<000001[S__]U8=]__W:JPVffKH0CUT__L<<>Tce]B9CUT__L<<P___ffKH00000IS__`d_P203BS__SB1=K_cVD80000IS_________bdTU30000000HK__`d`L<00000005[XT`cc^N]>200002@YfaKDYeaQDTeaQI7000000JM^d`__QG004DOdecccfdNP<0004DOdecccfdNP<00CUTd`LGGGGE0002@YfaKDYeaQDTeaQI7000000000:ROe`QQQQQQ?80:ROe`QQQQQQ?80000AWae]B900000CUTd`QQQQQ?6CUTd`QQQQQ?6CUTd`L<000000CUTd`L<0CUTd`LFM_dNMF00CUTddf][DNdO?1004DQK`dOD200007=RRRL``MRRRB30AW^NRRRRRRM^"	\
                "_P<CUTddf][DNdO?17=RRRRRRRRR]fS=07=RRRRRRRRRRRRRRRVbXF0000<>Nd^e_LJ00000ERNda`aYWPWTb`bdV>20CUTd`L<000001[RRR>A0HRRR>E7IQbfKH07=RRRD20;Wae]B97=RRRD25@RRQbfKH00000;RRROd_P203BRRR>E0<@XbVD80000;RRRRRRRRRL`dTU30000000GPRRTd`L<000000004HDM``W>[F90007IQ^eTD=]eaWGW`dVRA000000JM^eYRRPF00:M`eNNNNNcaQG000:M`eNNNNNcaQG00CUTd`QQQQQ?607IQ^eTD=]eaWGW`dVRA000000000;KcgfbbbbbV@80;KcgfbbbbbV@80000AWae]B900000CUTdfbbbbO@6CUTdfbbbbO@6CUTd`L<000000CUTd`L<0CUTd`L<BKbfY?50CUTdg_MFDNdO?1005>NcdaKI9000008<<<L``ME<<800AW^N><<<<EM^_P<CUTdg_MFDNdO?108<<<<<<<<B]fS=008<<<<<<<<<<<<<<DVbXF00003B]fgdT?800000C=K_a_KDF<G>S_aNXI90CUTd`L<0000004<<<<001<<<<00GQbfKH008<<<200AWae]B908<<<2005<GQbfKH000003<<?Oe_P2008<<<2000FXbVD800003<<<<<<<<EL`eTU300000003<<UTe`L<0000000000<L``LEC00000JPVd`W;=]eaW:USccSU800000JM^eY[<<709=ScbQ>>"	\
                ">W^e][C09=ScbQ>>>W^e][C0CUTdfbbbbO@60JPVd`W;=]eaW:USccSU800000000;K________V@80;K________V@80000AW__]B900000CUT______O@6CUT______O@6CUT__L<000000CUT__L<0CUT__L<<>T__QI0CUT__]U4DN_O?1005>N_VS@<000000000<L]]ME00000AW]]>5000EM]]P<CUT__]U4DN_O?10000000009B]]S=00000000000000004D]]XF00000FX___KH00000002BLXMB<000E[MWR=400CUT__L<00000000000000000000GQ]]KH000000000AW]]]B90000000000GQ]]KH0000000Z?]]]P20000000000FX]]D80000000000000EL]]]U3000000000CU]]]L<0000000000<L__LE000001[S__]U8=]__W:JPV__WG00000JM]]][70004DO_NPE45DO_NP<04DO_NPE45DO_NP<0CUT______O@61[S__]U8=]__W:JPV__WG00000000FPRRRRRRRRR[10FPRRRRRRRRR[10000E>RRRI0000007=RRRRRRRR[17=RRRRRRRR[17=RRRD20000007=RRRD207=RRRD20;RRRRI07=RRRPFZURRRB3008URPD=E00000000001H==HZ000004I==;30006I==;C7=RRRPFZURRRB30000000000A===E00000000000000009G==I500000<DRRR"	\
                ">J000000001EFJ100000ZAFE30007=RRRD2000000000000000000002====E0000000004I===A000000000002====E00000000F===;300000000005I==G900000000000001H===F00000000000F===H100000000005?PP@2000001[RRR>A0HRRR>E7IRRRRG000008I===:00004?RRR[107=RRRD<04?RRR[107=RRRD<07=RRRRRRRR[11[RRR>A0HRRR>E7IRRRRG000000007<<<<<<<<<<4007<<<<<<<<<<4000000<<<<100000008<<<<<<<<4008<<<<<<<<4008<<<2000000008<<<20008<<<2003<<<<1008<<<<704<<<800004<<47000000000000000000000000000000000000008<<<<704<<<8000000000000000000000000000000000000000000002<<<<00000000000000000000000000008<<<20000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000422500000004<<<<001<<<<001<<<<300000000000000005<<<40006<<<20005<<<40006<<<20008<<<<<<<<4004<<<<001<<<<001<<<<30000000"	\
                "00000000000000000822422C00000000000000000000000822422C00000000012252240000000822422C00000CCCCCCCC9000000822422C000000000000144890000000CCCCCCCCCC000000000002240000000CCC7007CCC0000000CCC7003CCC300003CCCCCCCCC00000000000744430000000000000144100000000000CC30007CC0000000000000000000000000000000000009854C0000000003CCCC70000000000000000000000000000000008887000000000000088890000000888000000000000000000000000000000000008887000000000000008889000000008880000000000000000000000000000000000000888700000000000000888900000005:::::<0000001[>>@>>I000000005:::::<00000001[>>@>>I00000000C=>>>>>U4000001[>>@>>I0000;UUUUUUUUH700001[>>@>>I0000000000<IUD>@=J70000;UUUUUUUUUU;900000000J>P?500000;UUUB46=UUU;90000;UUUB45BUUUB400<[UUUUUUUUUG000000001FBD>DBFZ000000000<HUDDUI<00000000EUUU<08=UUF000000<"":::<000000000000008"	\
                ":::J90002I@>DUH400000<H[UUUUI1000000144890A:70000ZEAAEZ000000A@DDH700000000000FDDDG000000E?DDF0000000CFIFGI;C00000EHHGHH:000000A@DDH7000000000000FDDDG0000000E?DDF00000000EHHGHH:0000002:::<C<J300000A@DDH7000000000000FDDDG0000007BWWWWW?6000004DVVM_Y[70000007BWWWWW?60000004DVVM_Y[70000000FMNOK_]@5000004DVVM_Y[700CUTTTTTTTT@600004DVVM_Y[700000000:DQTNNOSP;300CUTTTTTTTTTT?10000000E=X_LU;200CUTTTL<<PTTT?1000CUTTTL<BQTTTLE00:WTTTTTTTTY[30000002BM]ONO]MB20000007FDKTNNTK>F7000000HKTXF0<PTYUC0000ZUWWWUC0000000000000HWWWPE004UQONVYWF0000E?QYTTTT>50000<IUD>@=:"">LF000:ULWWLUA00000GWVVPA0000000000<DYVK=00000Z[SVY@5000000GMSW>SLE00007[KKLKKD800000GWVVPA00000000000<DYVK=000000Z[SVY@50000007[KKLKKD800003[WWW?[>M:00000GWVVPA00000000000<DYVK=000000C[YaaaVD8C00004DVVM_Y[CC0000CC[YaaaVD8C0000C4DVVM_Y[CC0000C2DY"	\
                "VMN]DJC0009C4DVVM_Y[C00CUTddeeeeO@600CC4DVVM_Y[CCC70000EDYadccecOMF00CUTddeeeeddO?1000009F>SO^VYQ?40CUTd`L<<P_dO?1000CUTd`L=X_eNWH300:WaeeeedddY[3000005[K_YQWQY_KU500000FRTVKXXKVTLG000001UYNP<0ZUTN><00008@VaO?10000000000000ISa^MJ00:W`bOO`XF0001[SaebSSN>5000:DQTNNOSPYTD40E>]_^^_]DE00006=Q`]U5000000001=KaSU200000FM_]^QI700001UTO_S_P<00007[Y`LNVD8000006=Q`]U50000000001=KaSU2000000FM_]^QI7000007[Y`LNVD800009IX_aK]NKI000006=Q`]U50000000001=KaSU200000;UUQQQQQ@UU;90;UURRDRRUUU;90FUUUQQQQQ@UU;00FUUURRDRRUUU;00FUU>WMWWDUUU;03IUUURRDRRUUE0CUTd`YYYYY@60;UUUURRDRRIUUUB4003=SbeVKKTcfY?40CUTd`YYYY_dO?100000J>Tbaa_aN>50CUTd`L<<P_dO?1000CUTd`LWNdNWI1000:WYYYYYOdaKB700003IQ_QU;:GUQ_QI3000E>TYP=FFIPYT>E0000<PNYU100IK^WF00008@VdO?5<400000000Z<<ISf^MJ00GQc_RDMWF000<PNdgbXSN>500EDYadccecONWI39=SbcOTabKI000004["	\
                "K^QI90000003FMN]@<000002@YT>]VPJ00004DOWQOSU100007[]]L]]D80000004[K^QI900000009FMN]@<0000002@YT>]VPJ000007[]]L]]D80000G>X]bb]X>G0000004[K^QI900000009FMN]@<00000CUTTT>G=KTTT?1CUTTT>2=KTTT?17B]TTKIG;QTTYU37B]TTKI2;QTTYU37B]TTKIA;QTTYU34DTTT><HKTTQG0CUTd`L[[[[;9CUTTTL<<2<<UTTTL<00<>Nd`WI;PVd^W:0CUTd`L[[P_dO?100003BScNQ`LXY>50CUTd`LFFP_dO?1000CUTd`MNe_Q=800005B[[[[KafYD<00000EPNQ?M?<?M@QNPE007=KT>LLH;PL>TS=3000FXaKH000:W^S=90008@VdO?>P@;100000:UPPUSf^MJ00:W_cOQD;6000JM^ggbXSN>503=SbeVKQTcfY?40Z?OdNP?YeTU1000F[D>WW?H40000F[DWW>?H40000F>WLP>WM:0000F[D>P>?H40000F[D>P>?H400000<IDPWWB4000000<IDWWDH4000000<>WLP>WM:00000<IDPP@=G90000ISN]SaNMG900000<=DPWWB2000000<=DWWDI200000CUTdN>E>VddO?1CUTdN>E>VddO?10:M`dOD5?Te^WG00:M`dOD5?Te^WG00:M`dOD5?Te^WG04DNdN>4HKfcQG0CUTd`L<77700CUTd`L<000CUTd`L<00:W^dT?87=S"	\
                "fcKH0CUTd`L<<P_dO?10000<PNbKX`LG@U40CUTd`XXXX_dO?1000CUTd`Oe`QB4000000777HX`eVLF000000IKT@K^KIK^S@TKI002>VX>VV>DONRXN><009=S`MA0005>VO@40008@VdOKN_TWH7000:"">]NN]Sf^MJ00FP]cfaTMH900EL_ggbXSN>50<>Nd`WI[K^d`M:08@VdODSN`O?100CU]ON_NTXBZ00CU]ON_NTXBZ00CU]ON_NTXBZ00CU]ON_NTXBZ00CU]ON_NTXBZ000E@KO__TXB40000E@KO__TXB40000E@KO__TXB40000E@KO__TXB40000FMPWQYfT>E0000E@KO__OQU<0000E@KO__OQU<0000CUTdN>IKbgdO?1CUTdN>IKbgdO?108UYe^XGW`dOD5008UYe^XGW`dOD5008UYe^XGW`dOD504DNdN>5HKfcQG0CUTd`L<00000CUTd`L<000CUTd`L<00FQbe]B90GQbfS=0CUTd`L<<P_dO?10000:W^_LX`LGGFZ0CUTdfbbbbcdO?1000CUTdee^KU5000000000FLVe`XH700000CUYKIK^KIK^S=KYUC0:WV>>VV>>ONRDVW:004@OVD50003B]_LE0008@VdO^`ceVPE006U]cf^^]f^MJ01UYcYS_eO><008?]cdbXSN>50:W^dT?BQ`SfcKH08@VdOKaVM>=300CUT`NN^e_M:00CUT`NN^e_M:00CUT`NN^e_M:00CUT`NN^e_M:00CUT`NN^e_M:001[S"	\
                "ac__f_X;0001[Sac__f_X;0001[Sac__f_X;0001[Sac__f_X;0007IWO^b^fbSB300Z[Saf^^e^KB300Z[Saf^^e^KB3000CUTdN>>Oc^dO?1CUTdN>>Oc^dO?100GX^eY?]fbKI0000GX^eY?]fbKI0000GX^eY?]fbKI004DNdN>5HKfcQG0CUTd`L<00000CUTd`LFFE8CUTd`L<00FQbe]=00GQbe]=0CUTd`L<<P_dO?10000FQbNDX`WQQWF0CUTdb````bdO?1000CUTdeec]DJ00000000<DYcaK[40000008@OW:@M@E?MD:WO@80FQTUILLIHLMIUTQG00JM`]B70000GQbQ;0008@VdaKM]cbKH00JM_dOWXNd^MJ05>N^MBM_^WA000:"">SNbXSN>50FQbe]BX_SQbfS=08@VdO]eORI5000CUSM>>X^cSI00CUSM>>X^cSI00CUSM>>X^cSI00CUSM>>X^cSI00CUSM>>X^cSI00JM_fYPLVeT?800JM_fYPLVeT?800JM_fYPLVeT?800JM_fYPLVeT?800EPVeaVV^dNP<00JM_dOMWNdNP<00JM_dOMWNdNP<000CUTdN>QaNNdO?1CUTdN>QaNNdO?1005DOd`MNdNPE00005DOd`MNdNPE00005DOd`MNdNPE004DVd`L?""?KfcQG0CUTd`L<00000CUTd`XXWLUFUTd`L<00FXaeY[C0HKcfKI0CUTd`L<<P_dO?10000FQbVDX`QbbXF0CUTd`LLLL_dTU1000CUTd`Oeb]@E"	\
                "0000008[KafYD<00000008@VW::J505JFAWVD80GQTUCF[DD[F1UYQG00HKaQG00000EL_][3008@VdV>AM`eY[70GQbcKIB]e^MJ04DVcYMPN`MJ0000A[M^XSN>50FQbe]WN]DQbf]=08@VdOKbcVQU5009;PK]YY_e]B709;PK]YY_e]B709;PK]YY_e]B709;PK]YY_e]B709;PK]YY_e]B70GQbbKKK]e_L<00GQbbKKK]e_L<00GQbbKKK]e_L<00GQbbKKK]e_L<00GQbf]DDSfaWF00GQbcKH=SfaWF00GQbcKH=SfaWF000CUTdN>OcSNdO?1CUTdN>OcSNdO?1000IKbf]bcSB300000IKbf]bcSB300000IKbf]bcSB3001UTefVOOOfcQG0CUTd`LGGE000CUTdfbba_YRUTd`L<00EL_dNP:E@TeaXF0CUTd`L<<P_dO?GG500FXaN>X`XKbXF0CUTd`LEEP_dTUGG50CUTd`MNeaSUGG<007HW_dVLGGGGGF7001UTQ[W[404BWUQTU10FWO@IM]VV]MI@OXF0CUY_R<000008?TN>2008@VdO?<P_dTU30;KcbQFISf^MJ07=K^f_ObY?400000JM^XSN>50FXaeYVY>HKcfSI08@VdO>]^d`QH00EPObb^`beYB70EPObb^`beYB70EPObb^`beYB70EPObb^`beYB70EPObb^`beYB70HKceccccc^ME00HKceccccc^ME00HKceccccc^ME00HKceccccc^ME00HKcbQF;KcbQ"	\
                "F00HKcbQF;KcbQF00HKcbQF;KcbQF000CUTdNQaNLNdO?1CUTdNQaNLNdO?1000EPNdcd`M:000000EPNdcd`M:000000EPNdcd`M:0000HQ`feeeegcQG0CUTd`QQQU100CUTdb``aec]UTd`L<00ZU]fc]MLK^eO>20CUTd`L<<P_dOQQQH00EL_^WX`LKbXF0CUTd`L<<P_dTQQQH0CUTd`LWNd^KQQB70:ROe`QQQQQQQLE009=S]W_XUIUX_W]S=90<R_MWN]QX]NWM_LE0<>NT?Z000000IS^WF008@VdVDEL_eY[30GQbcK;=Sf^MJ00<UQOaeTLF000000JM^XSN>50EL_d^TPF@TeaXF08@VdO?""?Q_eTUC0GQbbSMM_eYB70GQbbSMM_eYB70GQbbSMM_eYB70GQbbSMM_eYB70GQbbSMM_eYB70;KcaKKKKKKME00;KcaKKKKKKME00;KcaKKKKKKME00;KcaKKKKKKME00;KcbQ;IKfaXF00;KcbQ;IKfaXF00;KcbQ;IKfaXF000CUTdNTcSDNdO?1CUTdNTcSDNdO?10003=ScgeYU80000003=ScgeYU80000003=ScgeYU800005[X]]]]]fcQG0CUTdfbbO?100CUTd`LLX_dN>Td`L<000FMVfc`_be_XH90CUTd`L<<P_gcbbSI008?YfTX`LKbXF0CUTd`L<<P_gcbbKH0CUTd`L=Q_dbbYB70;Kcgfbbbbbb`ME000:M_WK_YSY_KW_MF001U]]]S?FF?S]]]U10FW^KI000"	\
                "0000:W^SI008@Vd`XDKbcSI00:W^eY>LVd^MJ00<?[>Q_f]=900000JM^XSN>50ZU]fcYMLK^dO><08@VdOM?@OdO?10HKcaWUQaeYB70HKcaWUQaeYB70HKcaWUQaeYB70HKcaWUQaeYB70HKcaWUQaeYB70:W`f]DUDW]UC00:W`f]DUDW]UC00:W`f]DUDW]UC00:W`f]DUDW]UC00:W^eY>PTd_LE00:W^eY>PTd_LE00:W^eY>PTd_LE000CUTd_^_MDNdO?1CUTd_^_MDNdO?1000AGM_g^XG0000000AGM_g^XG0000000AGM_g^XG0000004G====KfcQG0CUT__adO?100CUTd`LE?Od_PTd`L<0003HMY_aaeTWB400CUT__L<<P___ffSI000GWNcN`Y_bXF0CUT__L<<P___ffKH0CUT__L<BQ_beYB70;K_______`d`ME0006USOLWSYSWLOSU8000FMNKDF22FDKNMF09=S^W:00000002>NT?808@VdN`V^d_M:004@TfbNNOf^MJ00:W]QWOe]B700000JM^XSN>50JDY_fc`_be_XH908@VdOSTTaf][70FX^eNT_`eYB70FX^eNT_`eYB70FX^eNT_`eYB70FX^eNT_`eYB70FX^eNT_`eYB704?YcaOTO`TUC004?YcaOTO`TUC004?YcaOTO`TUC004?YcaOTO`TUC004?YcbNNcb]UZ004?YcbVNcb]UZ004?YcbVNcb]UZ000CUTddf][DNdO?1CUTddf][DNdO?1004D"	\
                "QK`dOD20000004DQK`dOD20000004DQK`dOD200000000999HKfcQG07=RRR_dO?100CUTd`MMX_dN>Td`L<0000CF?LWKa`Q=1007=RRRD25@RRQbfSI0001=XO^caN]M:07=RRRD25@RRQbfKH07=RRRD25UM^eYB70FPRRRRRRRTd`ME0000ED]VQL>RQV]DE0000CIXN]W>>W]NXIC08?TV>500000003[]_LE08@V_O]^a_QB1000GMOaaOS__MJ00:Wab^c_WG000000JM^XSN>5CU]NWY_aa^OXB4008@V_OS^b^OMF005@]^a_K__YB705@]^a_K__YB705@]^a_K__YB705@]^a_K__YB705@]^a_K__YB700FP]_aa^N]UC000FP]_aa^N]UC000FP]_aa^N]UC000FP]_aa^N]UC000FPY`aa_]>A0000FPY`aa_]>A0000FPY`aa_]>A0000CUTdg_MFDNdO?1CUTdg_MFDNdO?1005>NcdaKI90000005>NcdaKI90000005>NcdaKI900000000000HKfcQG008<<P_eO?100CUTdb``aeaSUTd`L<0000008E:@]cNW;0008<<<2005<FQbfSI00008HDX`WP[:3008<<<2005<GQbfKH008<<<200JM^eYB707<<<<<<<UTe`ME00000ADKV_N_VKD:0000008=MY_NN_YM=400<LSS[300000000;QSQF01[RRR?WXLB50000C;>WW>RRRD<00APQS]SM=8000000JM]XS]>50;"	\
                "MQ=?MXXMDH40001[RRR>MXW>;3000A?WXM?RRRI000A?WXM?RRRI000A?WXM?RRRI000A?WXM?RRRI000A?WXM?RRRI0009:ULXXM>[:00009:ULXXM>[:00009:ULXXM>[:00009:ULXXM>[:00009F?MXXLUA000009F?MXXLUA000009F?MXXLUA00000CUT__]U4DN_O?1CUT__]U4DN_O?1005>N_VS@<00000005>N_VS@<00000005>N_VS@<000000000000HK__QG0000<P]]]?100CUT_____OKDUT__L<000000000A>KKKUC00000000000FQ]]SI000000:WQP<10000000000000GQ]]KH000000000JM]]]B7000000003U]]]ME000000<=>MWM>=<000000001FULWWLUF1000C;IIJ0000000002III5004<<<4:FE10000009<FF<<<<20009EGI=I:100000008I====;30CH=48EFFA50000004<<<5AF:20000004:FJ5<<<100004:FJ5<<<100004:FJ5<<<100004:FJ5<<<100004:FJ5<<<1000008EFFA2C00000008EFFA2C00000008EFFA2C00000008EFFA2C00000008EFFE6000000008EFFE6000000008EFFE60000007=RRRPFZURRRB37=RRRPFZURRRB3008URPD=E000000008URPD=E000000008URPD=E0000000000000GPRRPF0000CH===F"	\
                "0007=RRRRRPD=<=RRRD20000000000:;;;E0000000000002====E0000001FG:7000000000000002====E0000000008I===A0000000000:===HZ000000075J:J57000000000006<""::E600000000000000000000000000000000000000000000000000000000000000000000009099000009000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008<<<<704<<<8008<<<<704<<<800004<<4700000000004<<4700000000004<<47000000000000003<<<<7000000000000008<<<<<<4708<<<200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000088800000000000000000000000000000000000008887000000000000088890000000888000000000000000000000000000000000000990090000000000888900000008880000000000000000000Z880188000000000000000000000000000000000000000000000000000000099009000000000000000000000Z8801880000000000000000000000990090000000000088888100000000088890000008880000000Z8801880000000000000000000000000000000099009000000000888881000009888000000000000000007888000000000078880000078880000000000000000000000000000000000000000000000000000000000078880000000000000E?DDF00000000CFIFGI;C000000EHHGHH:000000A@DDH700000000000FDDDG000000E?DDF0000000EHHGHH:0000005:::::<0000009GB=2IBHC0000000FDDDG000000E?DDF00000000E:::500005"	\
                "UD?EUD?E000000008:::JZ88100000000008:::J9000005:::::<00000009GB=2IBHC0000000E:::50000005UD?EUD?E0000005:::::<0000009GB=2IBHC00000000J@D?DDBZ0000000FDDDG00000E?DDF000005UD?EUD?E0000CFIFGI;C00005:::::<0000009GB=2IBHC000000J@D?DDBZ0000;DDDF1448900000000009;DDDF000000009;DDDF0009;DDDF000004AFGFEZ00000004AFFJ800000EHHGHH:0000EHHGHH:0000000009;DDDF00000000000Z[SVY@50000000GMSW>SLE000007[KKLKKD800000GWVVPA0000000000<DYVK=00000Z[SVY@5000007[KKLKKD800007BWWWWW?6000006@]QIM]P<000000<DYVK=00000Z[SVY@50000004DWWWB7000<PVSUQVWF00000000HWWWPUDDB60000000EFHWWWPFC0007BWWWWW?60000006@]QIM]P<0000004DWWWB700000<PVSUQVWF000007BWWWWW?6000006@]QIM]P<00000003=SVWVTD5000000<DYVK=0000Z[SVY@50000<PVSUQVWF0000GMSW>SLE0007BWWWWW?6000006@]QIM]P<000003=SVWVTD5000E>TVQHUD>@=J900000000E>OVXH00000000E>OVXH000E>OVXH0009F@M"	\
                "QQXLU200000:?MQQMUJ0007[KKLKKD8007[KKLKKD800000000E>OVXH00000000000FM_]^QI7000001UTO_S_P<000007[Y`LNVD8000006=Q`]U5000000001=KaSU200000FM_]^QI700007[Y`LNVD800007[YaaaVD8000003[]OSTOD4000001=KaSU200000FM_]^QI7000005>NaY[30007IQ^QNS[Z00000000ISa^MWVOD50000001UQQSa^QM:0007[YaaaVD80000003[]OSTOD40000005>NaY[3000007IQ^QNS[Z000007[YaaaVD8000003[]OSTOD40000000EPNSK_WG0000001=KaSU20000FM_]^QI70007IQ^QNS[Z0001UTO_S_P<0007[YaaaVD8000003[]OSTOD400000EPNSK_WG0008[S^KDQTNNOSP;30000004US^KB400000004US^KB4004US^KB4000E>Y^bba_MJ0000E>]`bb`]D<007[Y`LNVD8007[Y`LNVD800000004US^KB400000000002@YT>]VPJ000004DOWQOSU1000007[]]L]]D800003Z4[K^QIZ10003ZZFMN]@<100032@YT>]VPJ0003Z[]]L]]D800007[QQQQQ@8000003FR]OYWH000000FMN]@<000002@YT>]VPJ000005>N^Y[300002@Y`VPJ000000Z<<ISf^MKcKI9000000Z?ObbdfaW:0007[QQQQQ@80000000FR]"	\
                "OYWH00000005>N^Y[30000002@Y`VPJ0000007[QQQQQ@8000000FR]OYWH00000009IKVPNKBZ000000FMN]@<00002@YT>]VPJ00002@Y`VPJ00034DOWQOSU10003[QQQQQ@810003ZFR]OYWH10003ZIKVPNKBZ000;W`SD]adccecOLF000000HX`KU2Z1000000HX`KU2000HX`KU20000HKc^YYTNMJ0003BSbcOOfaKI907[]]L]]D8003[]]L]]D80000000HX`KU200000000000<>WLPPWM:00000C=?DPP@I2000000:=DPP@I20001=?""?""?UWW?""?[401=?""?""?WWD?""?[401=?>WLF>WM[401=?""?""?=H?""?""?[4000F[D>P>?H40000F[D>P>?H400000<IWWPDBA0000<>WLP>WM:0000<I@MMMBA0000<IMWWDBA0000:UPPUSf^M]_LJ000000:UPQQSf^QW:0000<IDPP@H4000000<IDPP@H4000000<IDMMMB4000000<IMWW@H4000000<=DPP@I2000000<=DPP@I2000000<BWWLW>E000005I@WW>?=<000<>WLP>WM:0005I@MWW?=<01=?""?""?UI@?""?[401=?""?""?;G?""?""?[401=?""?""?[?""?""?""?[401=?""?WWLW>?[400IWW@KbeNKQTcfY?40009:[WWD[?""?U<0004I@WW>UG001=WW?;00000ISfNRUU>PE000<>NdVPLNdOD400:=?""?""?=G901=?""?""?H?""?""?F000000<=WWP@I200000000"	\
                "00E@KO__OQU<0000E@KO__OQU<0000E@KO__OQU<005>OOTUISOOLE05>OOTUISOOLE05>OOTUISOOLE05>OOTUISOOLE00CU]ON_NTXBZ00CU]ON_NTXBZ000<?QO__V]B900<?QO__V]B3000<?QO__V]B900<?QO__V]B900:"">]NN]Sf^MWWU100000:"">]NN]Sf^MF1000E@KO__TXB40000E@KO__TXB40000E@KO__TXB40000E@KO__TXB40000E@KO__OQU<0000E@KO__OQU<0000E@KO__OQU<0001BQO__NTK;001BQO__NTK;001BQO__NTK;05>OOTUISOOLE05>OOTUISOOLE05>OOTUISOOLE05>OOTUISOOLE004AA>Vd`WI;POd^W:000FP]N_VQOOW:003=QO__NT?105>OOY[30000FW_cNTKL=4000:W^fS=B]e`ME005>OOY[3005>OOY[QOT>E0000E@KO__OQU<00000000Z[Saf^^e^KB300Z[Saf^^e^KB300Z[Saf^^e^KB305>NdTUISf`LE05>NdTUISf`LE05>NdTUISf`LE05>NdTUISf`LE00CUT`NN^e_M:00CUT`NN^e_M:001[S^da`a]B901[S^da`a]B9001[S^da`a]B901[S^da`a]B906U]cf^^]f^MAA2000006U]cf^^]f^MJ0001[Sac__f_X;0001[Sac__f_X;0001[Sac__f_X;0001[Sac__f_X;000Z[Saf^^e^KB300Z[Saf^^e^KB300"	\
                "Z[Saf^^e^KB300JM_f_N_bK;00JM_f_N_bK;00JM_f_N_bK;05>NdTUISf`LE05>NdTUISf`LE05>NdTUISf`LE05>NdTUISf`LE0000AW^dT?89=SfcKH004@YcbNb_cVD500<>Nd^VNO?105>NdY[30009;WVf^ccNXI700GQbbQXXKcaXF005>NdY[3005>NdY[DObS=300Z[Saf^^e^KB30000000JM_dOMWNdNP<00JM_dOMWNdNP<00JM_dOMWNdNP<05>NdYUISf`LE05>NdYUISf`LE05>NdYUISf`LE05>NdYUISf`LE00CUSM>>X^cSI00CUSM>>X^cSI00EL_dNQLX]B90EL_dNQLX]B900EL_dNQLX]B90EL_dNQLX]B90JM_dOWXNd^MJ0000000JM_dOWXNd^MJ000JM_fYPLVeT?800JM_fYPLVeT?800JM_fYPLVeT?800JM_fYPLVeT?800JM_dOMWNdNP<00JM_dOMWNdNP<00JM_dOMWNdNP<00FXb_M>LQK;00FXb_M>LQK;00FXb_M>LQK;05>NdYUISf`LE05>NdYUISf`LE05>NdYUISf`LE05>NdYUISf`LE0000FXbe]B90GQbf]=00AM`eY>Yef]B700EL`bQD>WU105>NdY[3000<>OeOWK`dV><00;KceaaaadbQF005>NdY[3005>NdY[=]fV>500JM_dOMWNdNP<0000000GQbcKH=SfaWF00GQbcKH=SfaWF00GQbcKH=SfaWF05>NdY[ISf`LE05>NdY[ISf"	\
                "`LE05>NdY[ISf`LE05>NdY[ISf`LE009;PK]YY_e]B709;PK]YY_e]B70GQbcK=EG@;00GQbcK=EG@;000GQbcK=EG@;00GQbcK=EG@;00GQbcKIB]e^MJ0000000GQbcKIB]e^MJ000GQbbKKK]e_L<00GQbbKKK]e_L<00GQbbKKK]e_L<00GQbbKKK]e_L<00GQbcKH=SfaWF00GQbcKH=SfaWF00GQbcKH=SfaWF00FWaaTSQL[E00FWaaTSQL[E00FWaaTSQL[E05>NdY[ISf`LE05>NdY[ISf`LE05>NdY[ISf`LE05>NdY[ISf`LE0000FQbe]=90FQbe]=00FQbbQ;KcaXG0004DTbOSSUE005>NdY[3000FXacKI[]faWF00;KccVVVVfbQF005>NdY[3005>NdY[HKf`LE00GQbcKH=SfaWF0000000HKcbQF;KcbQF00HKcbQF;KcbQF00HKcbQF;KcbQF05>NdY[ISf`LE05>NdY[ISf`LE05>NdY[ISf`LE05>NdY[ISf`LE00EPObb^`beYB70EPObb^`beYB70HKcbQF008900HKcbQF0089000HKcbQF008900HKcbQF008900;KcbQFISf^MJ0000000;KcbQFISf^MJ000HKceccccc^ME00HKceccccc^ME00HKceccccc^ME00HKceccccc^ME00HKcbQF;KcbQF00HKcbQF;KcbQF00HKcbQF;KcbQF00<>Taffb_K=90<>Taffb_K=90<>Taffb_K=95>NdY[ISf`LE05>"	\
                "NdY[ISf`LE05>NdY[ISf`LE05>NdY[ISf`LE0000:WadYUC0HKcfSI00GQcaXGQaNP<000ZBQTfcTU1005>NdY[3000HKcbXF;KcbQF00GQbbQ@@KfaWF005>NdY[3005>NdY[HKf`ME00HKcbQF;KcbQF0000000;KcbQ;IKfaXF00;KcbQ;IKfaXF00;KcbQ;IKfaXF05>NdTU[]e`LE05>NdTU[]e`LE05>NdTU[]e`LE05>NdTU[]e`LE00GQbbSMM_eYB70GQbbSMM_eYB70;QccKH8J=:00;QccKH8J=:000;QccKH8J=:00;QccKH8J=:00GQbcK;=Sf^MJ0000000GQbcK;=Sf^MJ000;KcaKKKKKKME00;KcaKKKKKKME00;KcaKKKKKKME00;KcaKKKKKKME00;KcbQ;IKfaXF00;KcbQ;IKfaXF00;KcbQ;IKfaXF008H>Q]T_dOD408H>Q]T_dOD408H>Q]T_dOD45>NdTU[]e`LE05>NdTU[]e`LE05>NdTU[]e`LE05>NdTU[]e`LE0000<R_dNR:E?YeaXF00FQbbQB]f_P<000JM_bYQQUJ705>NdY[3000;KcbQ;IKcaXF00:W^fS=[]e`ME005>NdY[3005>NdY[=Sf_P<00;KcbQ;IKfaXF0000000:W^eY>PTd_LE00:W^eY>PTd_LE00:W^eY>PTd_LE04DVdVLMNd`LE04DVdVLMNd`LE04DVdVLMNd`LE04DVdVLMNd`LE00HKcaWUQaeYB70HKcaWUQaeYB70:M`eOL@"	\
                "RSB90:M`eOL@RSB900:M`eOL@RSB90:M`eOL@RSB90:W^eY>LVd^MJ0000000:W^eY>LVd^MJ000:W`f]DUDW]UC00:W`f]DUDW]UC00:W`f]DUDW]UC00:W`f]DUDW]UC00:W^eY>PTd_LE00:W^eY>PTd_LE00:W^eY>PTd_LE00AWWDUU]eN>50AWWDUU]eN>50AWWDUU]eN>54DVdVLMNd`LE04DVdVLMNd`LE04DVdVLMNd`LE04DVdVLMNd`LE00001U]fcYMRK^dV><00AW^fS>Vd^W?E00FXa^M[UPRE05>NdTUF300:W`eY>POd_LE002>VdNMW_dO@4005>NdTUF304>NdTUPOdO@400:W^eY>PTd_LE00000004?YcbVNcb]UZ004?YcbVNcb]UZ004?YcbVNcb]UZ0CU]fc__Oe`LE0CU]fc__Oe`LE0CU]fc__Oe`LE0CU]fc__Oe`LE00FX^eNT_`eYB70FX^eNT_`eYB704?]cf_V_]B904?]cf_V_]B9004?]cf_V_]B904?]cf_V_]B904@TfbNNOf^MJ00000004@TfbNNOf^MJ0004?YcaOTO`TUC004?YcaOTO`TUC004?YcaOTO`TUC004?YcaOTO`TUC004?YcbVNcb]UZ004?YcbVNcb]UZ004?YcbVNcb]UZ00AW^VTT`eT?80AW^VTT`eT?80AW^VTT`eT?8CU]fc__Oe`LE0CU]fc__Oe`LE0CU]fc__Oe`LE0CU]fc__Oe`LE00000FMVfc`_bd_QI9005DOd^Oba"	\
                "fVQG00AM`fNYYNMJ04>NdVKR<004?YcbVNcb]UZ007=KaeN_e^QI0004>NdVKR<0Z?Te`YOcaKI9004?YcbVNcb]UZ00000000FPY`aa_]>A0000FPY`aa_]>A0000FPY`aa_]>A000FMVaaTS__LE00FMVaaTS__LE00FMVaaTS__LE00FMVaaTS__LE005@]^a_K__YB705@]^a_K__YB700:"">]_aa`]B900:"">]_aa`]B9000:"">]_aa`]B900:"">]_aa`]B900GMOaaOS__MJ00000000GMOaaOS__MJ0000FP]_aa^N]UC000FP]_aa^N]UC000FP]_aa^N]UC000FP]_aa^N]UC000FPY`aa_]>A0000FPY`aa_]>A0000FPY`aa_]>A000AWV`ab^OW;00AWV`ab^OW;00AWV`ab^OW;00FMVaaTS__LE00FMVaaTS__LE00FMVaaTS__LE00FMVaaTS__LE000003HMY_aa^OXB40009IXNaa^SO_QG008UKNaba_MJ06?Yac`L<000FP]_aa_]>A0000<DS_a^NK?20006?Yac`L<00;WV`a^NSD<0000FPY`aa_]>A0000000009F?MXXLUA000009F?MXXLUA000009F?MXXLUA00001HPXWDRRRD<001HPXWDRRRD<001HPXWDRRRD<001HPXWDRRRD<000A?WXM?RRRI000A?WXM?RRRI0000:ULWXMDG0000:ULWXMDG00000:ULWXMDG0000:ULWXMDG000C;>WW>RRRD<00000000C;"	\
                ">WW>RRRD<00009:ULXXM>[:00009:ULXXM>[:00009:ULXXM>[:00009:ULXXM>[:00009F?MXXLUA000009F?MXXLUA000009F?MXXLUA00005[>MXXWDH1005[>MXXWDH1005[>MXXWDH1001HPXWDRRRD<001HPXWDRRRD<001HPXWDRRRD<001HPXWDRRRD<000000CF?MXXMDH4000004IPWWM?DRP:000<BPWXWRU200F>QQX><0009:UMXXLUA000000E[LWWPB<00000F>QQX><006I>MWWP[E000009F?MXXLUA0000000000008EFFE6000000008EFFE6000000008EFFE60000007<F:2<<<200007<F:2<<<200007<F:2<<<200007<F:2<<<2000004:FJ5<<<100004:FJ5<<<1000006EFFJ40000006EFFJ400000006EFFJ40000006EFFJ4000009<FF<<<<200000000009<FF<<<<200000008EFFA2C00000008EFFA2C00000008EFFA2C00000008EFFA2C00000008EFFE6000000008EFFE6000000008EFFE600000035JFFA50000035JFFA50000035JFFA5000007<F:2<<<200007<F:2<<<200007<F:2<<<200007<F:2<<<20000000008EFFA5000000003<""::A55<<90000C<"":F:<Z0000<FGFE0000008EFFE6000000001E::<C0000000<FGFE00"	\
                "0095JAA<1000000008EFFE600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007888000000000000788800000000000000000000000000000000000000000000000000000000000000000009C9000000888700000000000000000000000000990090000000000000000000009900900000000000000000000000000000000000000000000000000000000000000000000000000CCC00000000CCC000000"	\
                "000000000000000745100004540000000000854100000000000144100000000014541000000CCCCCCC3000000000Z44100000000964469000000000Z44300000000000C454300000000034541000000000014489000000000Z4483000000CCC700CCCC000000000000000001221ZZZ100000000000000000000000000000000000009;DDDF00000000009;DDDF00000008AFFE3000000006JFFJ1000000000033C4<JC0000009336<JC00009<"":GI=UI30000A@DDH7000000000EHHGHH:00000009GB=2IBHC0000EHHGHH:0000009GB=2IBHC00000EHHGHH:000000EHHGHH:000000EHHGHH:0000000EHHGHH:0000000EHHGHH:00000GUUU;900000GUUU;900000000ZHI;IH8000<=D>UF30AD>DA00000005I?>DUI<0000000ZGU>>UGZ00000EIUD>DU;5000:UUUUUUUU<000000<I?DDUI<00000EI?>>?IE0000002H?DD[:300000005;UD>DBF8000005GUD>DUIJ300000<IUD>?=J9000002I?>>@[H<000;UUUB4EUUUUJ000004J:::Z0000:U>>U?""?""?U500<"":::<00000000<"":::<00<"":::<000000000E>OVXH0000000000E>OVXH000000"	\
                "EUMQXR=4000000EUMXQM[<0000005F=[UUDPMF00008F=[U?>MF0003;>WQS]Y><0000GWVVPA000000007[KKLKKD80000006@]QIM]P<0007[KKLKKD8000006@]QIM]P<00007[KKLKKD800007[KKLKKD800007[KKLKKD8000007[KKLKKD8000007[KKLKKD80007[YTT?100007[YTT?1000000<I?MSLSWA004UKONYMG1[SNWF0000005UQTNVTSI000000Z=WYNNYW=Z0007BKTVNVYQU500ISTTTTTTQF00000ADKTNVTKB9000EDKONNOKDE0000<UQTNV]L;700000E?QYVNO]W=7000EUXYVNVTKLF0000:DQTNNOSP;3000<?QTNNOYK[70CUTTTL<;KTTK;0009FDMWWW;000:"">]NN]SOOMJ0ZUWWWUC000000ZUWWWUCZUWWWUC00000004US^KB40000000004US^KB400000<?S`bbNXH900004US_bb_K[Z0000:@XSYTTO_QG000<[WSYTONQG000FMO^bfcaXG00006=Q`]U500000007[Y`LNVD80000003[]OSTOD40007[Y`LNVD8000003[]OSTOD400007[Y`LNVD800007[Y`LNVD800007[Y`LNVD8000007[Y`LNVD8000007[Y`LNVD8000C[YeO?19000C[YeO?190000:DKTN_TN><00FW`OS^T@FM_]U8000000;X`e^`bSI000000GWNebaeNWG0007BY"	\
                "caafe`Q;00ISfeeeebQF0000<@]afa^b]B9003[Sbf^`faS[300CBK^f`^fOPA0000<@S^efbcfTU300<@S^efccfcK;000EDYadccecOLF007=K^dcbceY[70CUTd`L<;KccK;000E>T`aaKH008U]cf^^]f^MJ08@VaO?10000008@VaO?18@VaO?1000003ZHX`KU20000000CZZHX`KU2Z00009IKaaTVeOD50000:W`fOOf_M:000:"">Y^c`NNVTQG004UK`c_NNTQG008?YfN]SQXLG000004[K^QI90000007[]]L]]D80000003FR]OYWH00007[]]L]]D8000000FR]OYWH000007[]]L]]D800007[]]L]]D800007[]]L]]D8000007[]]L]]D8000007[]]L]]D800GUUYeO?UH70GUUYeO?UH700EDYadecf^MJ00=SbQ@O_LU]_M:0000009B]e_WMQSI000008?Ye_QQ_eT?8007BYKWX]ce]B70ISfT]]]]QF0000HQacYWWK]B9004DNdNMMNdN>500ERNeTMQ^cSB7007=Kad_SQKTTU307=Kad_]QK]NK;003=SbeVKKTcfY?404DVdNKQKTY[70CUTd`L<;KccK;007;KbcOTKH00JM`dOWXNd^MJ08@VdO?5<400008@V^O?18@VdO?100001=?""?WW?""?""?F000004[?""?UWW?""?""?""?F0004DOeY@Xa`ME0000;Kc`MM_cS=00ZU]cb]MP>DUI500FW`cYMPDUI500AM`NLPPD=E3"	\
                "00000<IDWW@H4000000:=DPP@H400000F[D>P>?H40000F[D>P>?H400000<IDPP@H400009AI?>P>U;800001I?>PPD=<00000:=DPP@I2000000:=DPP@I200001H?>P>U=G903[YTOdVTT@63[YTOdVTT@603=KbfNNQONMJ09=]aXUT^MW`SB10000009=]f`W;H?F<<C00EL_eYUUYe_LE000;DIFFM^eY[70ISfTDDU==20008?TdV>UUH@G0005>NdTUUTdN>500FXacKHDVdVD4002>Vd^QBGHDKU302>Vd^Q[G;BRK;00<>Nd`WI;PVd`W:0<P_fS=GI?WB70CUTd`L<;KccK;07I?Sf`M?UJ00GQbcKHB]e^MJ08@VdO?>PDH100ZUMMM[C8@VdO?100005>OOY[QOT>E0000GWOOMAA[]OY@500EM`bQ;Xa^MJ0000;Qc_L[]eTUC0<R_dTSN_VK?<009=]fNKV_OXI300GQb]T__VKDJ0000E@KO__TXB40000E@KO__TXB4000CU]ON_NTXBZ00CU]ON_NTXBZ000E@KO__TXB4000<>KTN_NYWB40002PTN__VKU4000E@KO__OQU<0000E@KO__OQU<000ELTN_N]MH303[YaadaaO@63[YaadaaO@602>Nd_W_M`MMJ00HKcKPNNPYNRJJ<900001IW`eORFBLLL;00FXafSIISfaXF0007A>RMSbaKH00ISfOVVYX[4000<P_e]]TYQ?<0001U]cNXXNc]U100FXbcKHDVd_LE0"	\
                "0:W^dO@5002HJ00:W^dO@E=====F00:W^dT?87=SfcKH02>NdNSWPUF500CUTd`L<;KccK;01?OOe^OOW:00;KcbQFISf^MJ08@VdOKV_OW;008@OOO?18@VdO?100005>NdY[DObS=3008?YeTU69FW^^WG00:WaaXQYfV><0000:M`b]WSfO@80AW^eY^_adaKI90CUTdOa_bdNLJ00;Kcbe^`fb]U8001[Sac__f_X;0001[Sac__f_X;000CUT`NN^e_M:00CUT`NN^e_M:001[Sac__f_X;000<L`a_N^e_W;0002P__N`e^W:00Z[Saf^^e^KB300Z[Saf`^e^KB300EL`^`cfVMF07BWWYeOWWU17BWWYeOWWU10:W^e]KTQVD;100JRO`N^SX^K@MM>H1000;WNfbcYD@O`SI00GQbfKHHKfcQG0000FX_`bVK?<00ISfbafe_QH000JM^e_cbe^S[1000FP]`aa`]PF000AM^eY>W`g^MJ00FQbe]B90000000FQbe]BISSSSS?Z0FQbe]B90GQbfS=01U]cdca_YMH70CUTd`L<;KccK;01?ObdfbaW:00GQbcKHB]f^MJ08@VdO^^feTD408@VdO?18@VdO?100005>NdY[=]fV>500JM`fSIBBB>Nf][30FXafabfc]D<000<[U]af^adVD80FXbg`QRKadOD50Z?OdaKRSccKI00;KcdNWMTe`M:00JM_fYPLVeT?800JM_fYPLVeT?800CUSM>>X^cSI00CUSM>>X^cSI00JM_fYPL"	\
                "VeT?800<LTXP>WNeT?8002PXP>W`bQF00JM_dOMWNdNP<00JM_dOMWNdNP<00ELSMMSbf][305:[YeO?:<005:[YeO?:<00FQbcKTS]][30007;LKSXDTO>Y`^VW;004DTeV]bbSPNbQG00GQcfKHHKccQG0000FX__acNW;00IS]QX]adO@400JM^gaSQVd_LE00CBK_bVVb_KB3004@YfbN_cd^MJ00FQbe]=90000000FQbe]=ISfffO?Z0FQbe]=90GQbf]=00:"">]_afdfO><0CUTd`L<;KccK;01UXSf`XXM:00JM`dOMWNd^MJ08@VdbSWTe`LE08@VdO?18@VdO?100005>NdY[HKf`LE00GQbbQL]]]@OdVD40:WabOOVbbKI000FQYYQYOOeV@80FQbdYU<>Nd`ME08@VdV>JM`eY[70FXbf]B;QbcK;00GQbbKKK]e_L<00GQbbKKK]e_L<009;PK]YY_e]B709;PK]YY_e]B70GQbbKKK]e_L<00;WWWWWW]e_L<007GPSSYbVLJ00GQbcKH=SfaWF00GQbcKQQSfaWF005ULWWWNdVD4007[YeO?1000EG[YeO?GJ00FQbcKNXVQ;000007JHIIQaQKbSQ^T?80EM`cK>Tf^Kb_LE00FQbfSIISfbQF0003A>PLK^eT?Z0G>BGFDOd_P<00EM`dN>ISfaXF00<PNeY>>YeNP<000FLY_`NSf_L<00FWadYU100CE400FWadYUIS]VdO?Z0FWaeYUC0HKcfSI00EAULQ]NdaXF0CUTd`L"	\
                "E;KccK;00<ISf`LFFC008U]cf^^]f^MJ08@VdN>ISf^MJ08@VdO?18@VdO?100005>NdY[HKf`ME00HKcbXL`e]?OdN>50AM^aX?>OdTUC00FXbaW[@]eT?10GQbe]=8@Od^MJ08DVdO?<P_dTU30FXbfKHFWafSI00HKceccccc^ME00HKceccccc^ME00EPObb^`beYB70EPObb^`beYB70HKceccccc^ME00HKaaaaaad^ME000<P_bfVK@<00HKcbQF;KcbQF00HKceaaaadbQF000JM^aabgN>5007[YeO?1003[QQYeOQQ?60FWafSNM`ME<0000000E>OTDY^WDV_P<0AW^cKHLVebc]U100AM^e]BB]e^MA004B=J2<DNdN>48UDG<5?Td_L<002>NdVDHKcbXF00AW^fSIISf^MJ000EI?LM>TdVD400<P_d_MG<J=L=70<P_d_M;=DVdO?Z0EL_dNP:E@TeaXF04DLBFGBQccQG03[Ye^W:ISfbQG000ISf`LE00000:"">]__]Sf^MJ08@VdO?HKf^MJ08@VdO?18@VdO?100005>NdY[=Sf_P<00;KcbQM^eY@OdN>50<R_cKI?TdTUC00AM^bKHDVe]B70FXaeY[5DVd`LE0Z?OdVDEL_eY[70:WafS=GQbcKH00;KcaKKKKKKME00;KcaKKKKKKME00GQbbSMM_eYB70GQbbSMM_eYB70;KcaKKKKKKME00;KcaYYYYe`LE004;>QQS^aQ;00;KcbQ;IKfaXF00;KcbQWWSfaXF00ZIM"	\
                "KKKVdN>5007[YeO?1003UTccgccO@60EL_d_TKO@D>E000003=K^XUY^WDV_P<0<R_eOMDW`gNLF0005DOdNLLVdOD500ELKM>>Q`dOD45>TX>>Q`dV>5001UYe_L?]e`MJ00EM`eYD@Ye`ME009BKL@DK^cS=7001U]cfTWPMKTU301U]cfOWPLVdO?Z0ZU]fc]MLK^dO><05>NSWPL]faXF00IKcc]LMVd`MA000ISf`LE00000;XDPPDTe_P<08@VdO?HKf^MJ08@VdO?18@VdO?100004>NdTUPOdO@400FXaf]Qb`VM_dT?608@TdVMM_f]=9005DVeTPX^bQ;00ERNdNM?X^dVD507B]f`W?KbcKI00<PNdOR>Yf^WF00:W`f]DUDW]UC00:W`f]DUDW]UC00HKcaWUQaeYB70HKcaWUQaeYB70:W`f]DUDW]UC00:W^cKU>OdOD400GQM?UPNfSI00:W^eY>PTd_LE00:W^eY>PTd_LE00ELXDDKaeYU1007[YeO?1003U]]YeO]]@601U]ff]YSWYMJ00000JRNY?IScSX`T@401U]bf_V`fafT>A009IQ^fNNf^KI900EL``NN^daS=75>NaNN^daK=7000GW_fVT^cY?4005DTeaOOaeTD4009B]_OVafOPJ0000:ROce^_`bTU300:ROce^_`beO?Z00FMVfc`_be_XH905>Nf^__ceV><00ALNec``fcY?4000ISf`LE00000HKOSKYac]U108@VdO?HKf^MJ08@VdO?18@VdO?10000"	\
                "Z?Te`YOcaKI900<PNdb`^Yc`ecS=900IQ^fNNf^XF0009IK^cN_eV><001[SafNO`daK=700FMNe_O`dNMA001[SbcNVbfYD5004?YcaOTO`TUC004?YcaOTO`TUC00FX^eNT_`eYB70FX^eNT_`eYB704?YcaOTO`TUC004?Yc`TOc^KI900GQ`OTVcaQ;004?YcbVNcb]UZ004?YcbVNcb]UZ00EL`VV^d_WG0007[YeO?1000FB[YeO?BG900FMVfca_^^MJ0000CBS`WGJPTaaNWH000A>]_aa_YS__]@500<?K_aa_S?<000ELV`ab^NSDE05>YN^baNS@E00008[QNaa`YPF0000;MT`aa`TM;0009B]`aa`YM;900007;L]_ab^NS[3007;L]_aba_TQU1003HMY`aa^OXB4005>]N^aa^OWI7003IWO^aa_YPF0000ISf`LE00000HKacfc^YP:008@VdO?HKf^MJ08@VdO?18@VdO?100000;WV`a^NSD<0003=Q_a^YXNa^TPJ0002?KN^a_S?50000<?S_a^OWH7000E@KN^aa_SD<0001=XO^aaNQ=1000JDS_aa`YLF0000FP]_aa^N]UC000FP]_aa^N]UC005@]^a_K__YB705@]^a_K__YB700FP]_aa^N]UC000FPY`aaNK?<000GQ_aba_]D<000FPY`aa_]>A0000FPY`aa_]>A000ELNab^VQ[40007[YeO?100007[YeO?100003HQN_ba`TMJ00004@XXU49G>WXR=8000"	\
                "0AULXXLUDRRR?4000<[LXXL[<00008=DMWXWPBE007GUPWXWP[E0000005=PWXM?F90000C;DMXXMD;C0000GDMXXM@F30000007:URWXW>BE00007:URWXWL?H20000CF?MXXMDH40009FB>MXXMDH600008HDMXXM?F90000IS__LE00000FPXKSKW@F9008@V_O?HK__MJ08@V_O?18@V_O?1000006I>MWWP[E000005BLXWD=LXWDF90000<BPWWLUE000000<[LWW>H600000EB>WXWLUE000004I>WXWP=500000JULWXM?F700009:ULXXM>[:00009:ULXXM>[:0000A?WXM?RRRI000A?WXM?RRRI0009:ULXXM>[:00009F?MXWPB<0000J?LWXWLUA00009F?MXXLUA000009F?MXXLUA00004[PWXW>I500007[]]]?100007[]]]?100009B]YSTXM?G100000EFF<0002:FE30000006EFFE82<<<5000001EFFE100000075EFF:<C000001<"":F:<C000000003<FFJ4000000004JFFJ400000004EFFJ400000000001<"":F:2300000001<"":F:E8000000008EFFA5000000035JFFA5900000005AFFE8000000;RRRD<000007<FHIHF500001[RRRBGRRR><01[RRRB31[RRRB300000095JAA<100000001EF:5CEF:20000000C<""::EZ000000001E:A2900000003<"":F"	\
                "FE1000000092AF:<300000001EFFJ4000000008EFFA2C00000008EFFA2C0000004:FJ5<<<100004:FJ5<<<1000008EFFA2C00000004AF:<C0000004E:F:E600000008EFFE6000000008EFFE6000000C<"":F:27000000:===F000000:===F000000FUUUU;E40000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000C<<<200000000000000000004<<<83<<<20004<<<8004<<<800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009990000000099900000000CCCC000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007ZZZ35<4000000001221ZZZ10003ZZZ700ZZZ10007ZZZ70ZZZ10000007ZZ1000055400000007ZZ10000554000000078441000055400000009ZZZC000000008<<4300008887000000088890000088800000000000000000000000000000342<260000000094<<8000000000000000000000000000000000000000099009000000000000000000000000000000000000000888900007ZZZ35<4000000Z880188000007ZZZ75<4000000000007CCCCCCCCCC000000008889007ZZZ35<40000Z8801880000004<<5Z000000000Z881003ZZZ90ZZZ100000000008880000000000000088890000000000000000Z88018800000000000000000000000<"":::<00000003I?""?""?B>P@;100000:U>>U?""?""?U50C=?""?""?H9:?""?""?U203I?""?""?I:?""?""?U20001GB?""?[500:D>DA00001GB?""?[500:D>DA000005B@>DUF90:D>DA000000G?"	\
                "?""?B800000<I@PPDBA00A@DDH700000FDDDG0000E?DDF0000EHHGHH:0000CFIFGI;C0000F[D>P>?H400000<IDPP@H4000002:::<00000000CFIFGI;C005:::::<09GB=2IBHC0<"":::<88100<"":::<00000005:::E00000000FDDDG0003I?""?""?=>PDH10005UD?EUD?E0003I?""?""?=>PDH10000003EHBUUUUUUUUUUF000000FDDDG03I?""?""?=>P?2005UD?EUD?E0005I@PP>?=<0000002UDDB61=?""?""?;F?""?""?[400000000E?DDF000000000000FDDDG00000E:::5000005UD?EUD?E000004J:::Z00<"":::<000ZUWWWUC0000008@OOOKN_TWH7000:"">]NN]SOOMJ04@TOOD5=SOOMJ06@OOO?HKOOWA000AM]OOME0Z[]NWF0000AM]OOME0Z[]NWF00000:WONN]PJZ[]NWF000007BYOOP<0000<?QO__V]B90GWVVPA0000<DYVK=000Z[SVY@5007[KKLKKD8000GMSW>SLE000CU]ON_NTXBZ000E@KO__TXB4000:UWWW?;;400000GMSW>SLE07BWWWWW?66@]QIM]P<ZUWWWUDDB6ZUWWWUC000009=WWWD5000000<DYVK=0008@OOOKV_OW;000<PVSUQVWF0008@OOOKV_OW;000002=LK]TTTTTTTTTYB70000<DYVK=08@OOOKN_ME00<PVSUQVWF001BQO__NTK;000"	\
                "2FFFWVOD55>OOTUISOOLE0000000Z[SVY@50000000000<DYVK=00004DWWWB70000<PVSUQVWF0009FDMWWW;0ZUWWWUC008@VaO?10000008@VdO^`ceVPE006U]cf^^]f^MJ00IKb^WFDOdT?806@OdO?HKf^WA000AWT]`ME0FM_]U60000AWT]`ME0FM_]U600000:WSQ]cK;FM_]U6000007BYe_P<0001[S^da`a]B906=Q`]U5001=KaSU2000FM_]^QI707[Y`LNVD8001UTO_S_P<000CUT`NN^e_M:001[Sac__f_X;004DKTaVKKQ;00001UTO_S_P<07[YaaaVD83[]OSTOD48@VaOWVOD58@VaO?1000007BYa_P2000001=KaSU20008@VdO^^feTD4007IQ^QNS[Z0008@VdO^^feTD40005UKNbededddeeeeYB70001=KaSU208@VdO`c`ME007IQ^QNS[Z00JM_f_N_bK;009=XXXKcKI95>NdTUISf`LE0000000FM_]^QI7000000001=KaSU200005>NaY[300007IQ^QNS[Z000E>T`aaKH08@VaO?1008@VdO?1ZZZZ008@VdaKM]cbKH00JM_dOWXNd^MJ00EL_fSBM`bKH006@OdO?HKf^WA000<@?]`ME4U]NM:00000<@?]`ME4U]NM:0000002[WQ]`XGU]NM:0000007BY`_P<000EL_dNQLX]B9004[K^QI90FMN]@<0002@YT>]VPJ07[]]L]]D8074DOWQO"	\
                "SU1000CUSM>>X^cSI00JM_fYPLVeT?804DNagbaaK;90004DOWQOSU107[QQQQQ@80FR]OYWH08@VdOKcKI98@VdO?1000007BYe_P20007ZFMN]@<00008@VdbSWTe`LE0072@Y`VPJ00008@VdbSWTe`LE009IQ^dcNTYOd`YYYYYB707ZFMN]@<008@VdcTQKME0072@Y`VPJ000FXb_M>LQK;003B]b_]_LJ05>NdYUISf`LE0000ZZ2@YT>]VPJZZ30001ZFMN]@<700015>N^Y[Z700012@Y`VPJ7007;KbcOTKH08@V^O?1008@VdO?:?""?""?""?G08@VdV>AM`eY[70GQbcKIB]e^MJ001U]fV>Kc_LE006@OdO?HKf^WA00004B]`MEGW`SBZZ700004B]`MEGW`SB44100000FQV`YLGW`SBZZ700009ILLL@5000GQbcK=EG@;0003I?WWB73IWWDI7000<>WL?>WM:00:=?""?""?=G93I?""?""?U>PDH10009;PK]YY_e]B70GQbbKKK]e_L<08?WTdVWWWDI800C=?""?""?@[E000<I?""?""?IJ003I?""?""?I108@VdO]_LJ08@VdO?EE<0007BYe_PU403I?""?WW>PDH1008@VdN>ISf^MJ03I?""?MWWPDH1008@VdN>ISf^MJ005DOdbSPUUTd_R[[[[F03I?""?WW>P?208@Vd_L;I=803I?""?MWWP?200FWaaTSQL[E03=?]e_WW?G05>NdY[ISf`LE000F?""?""?>WL?>WM?""?""?I30<U?""?WWD?""?H90<U?@MMM?""?H9"	\
                "0<U?""?MWW?""?H97I?Sf`MUUJ0ZUMMM[C008@VdO?DYOOK=08@VdO?<P_dTU30;KcbQFISf^MJ000GX^aXTeYUZ006@OdO?HKf^WA000C;B]`MI@TNP[?""?I700C;B]`MI@TNPU>>UF900:D?DXaS@TNP[?""?I70003BYVVP<000HKcbQF00890008@OOO?18@OOO?10000E@OOO?J3008@OOO?108@OOOKV_OW;000EPObb^`beYB70HKceccccc^ME00<UTdVQV_OWI9008@OOO?10008@OOO?1008@OOO?108@VdOWWU108@VdOMMMD2007BYe_YX;08@OOOKV_OW;008@VdO?HKf^MJ08@OOOKV_OW;008@VdO?HKf^MJ00AW^dO>J1UTd_XXXXWG08@OOOKN_ME08@VdO?699008@OOOKN_ME00<>Taffb_K=94DOOe`OO]B95>NdY[ISf`LE000ISOOL=SOO@?TOO@80:WOOOOOOTUC0:WOOOOOOTUC0:WOOOOOOTUC1?OOe`LE3008@OOO?1008@VdO@]b^KU<08@VdVDEL_eY[30GQbcK;=Sf^MJ0002DOf]_bQH0006@OdO@HKf^WA000<LS]`SSQ^K=XOO?Z00<LS]`SSQ^KWTNN]PJ00;KKQ]cSQ^K=XOO?Z003;MNdVD5000;QccKH8J=:0008@VdO?18@VdO?100008@VdO?10008@VdO?108@VdO^^feTD400GQbbSMM_eYB70;KcaKKKKKKME001UTdV`^ceV>2008@VdO?10008@VdO?1"	\
                "008@VdO?108@VdO?A2008@VdO]^_R<008BYec_K[38@VdO^^feTD408@VdO?HKf^MJ08@VdO^^feTD408@VdO?HKf^MJ00FQbe]B71UTdcbbbbQG08@VdO`c`ME08@VdO?100008@VdO`c`ME008H>Q]T_dOD44DVbdcbb]B95>NdTU[]e`LE000FXaaXUTd_RPNf]B70:WabbbedTUC0:WabbbedTUC0:WabbbedTUC1?Obd`LE0008@VdO?1008@VdOSa^KU<008@Vd`XDKbcSI00:W^eY>LVd^MJ0009=Sc_b_RE0006@Od_M>YeaW>E00<LOOOOSOO>PO_O?Z00<LOOOOSOO>WYQ]cKI00;KONN]POO>PO_O?Z00FLOf_QI9000:M`eOL@RSB9008@VdO?18@VdO?100008@VdO?10008@VdO?108@VdbSWTe`LE00HKcaWUQaeYB70:W`f]DUDW]UC001UTdc]WYe^WA008@VdO?10008@VdO?1008@VdO?108@VdO?10008@VdO]e_R<02[XVd_X[<08@VdbSWTe`LE08@VdO?HKf^MJ08@VdbSWTe`LE08@VdO?HKf^MJ00FQbe]=91UTdb````QG08@VdcTQKME08@VdO?100008@VdcTQKME00AWWDUU]eN>58@X]e_XXX=94DVdVLMNd`LE000<P_fS>N`aXW^bQG00:MXXXSbaSUC0:MXXXSbaSUC0:MXXXSbaSUC1UXSf`LE0008@VdO?1008@VdO^aS?<0008@VdN`V^d_M:004@Tfb"	\
                "NNOf^MJ0000AM_geYU10006@Ode_Nb^fNWA004[@@@@KaQUSYNO?Z004[@@@@KaQI>?IQbKI00<B@>DUKaQUSYNO?Z06?Yf`Q[427004?]cf_V_]B9008@VdO?18@VdO?100008@VdO?10008@VdO?108@VdN>ISf^MJ00FX^eNT_`eYB704?YcaOTO`TUC001UTd_R;KcaXF008@VdO?10008@VdO?1008@VdO?108@VdO?10008@VdO]__R<0FQ__d_P2008@VdN>ISf^MJ08@VdO?HKf^MJ08@VdN>ISf^MJ08@VdO?HKf^MJ00:WadT?51UTd_LLLLLF08@Vd_L;I=808@VdO?100008@Vd_L;I=800AW^VTT`eT?80EB]e_PFF20CU]fc__Oe`LE0008?TdTW^]aSKc`ME00CFF[K^c]DE00CFF[K^c]DE00CFF[K^c]DE00<ISf`LE0008@VdO?1008@VdcdVMG90008@VdO]^a_QB1000GMOaaOSf^MJ00006UYeaQ;00006@OdO`aTYa^WA000166E>VTDXNMNO?H900166E>VTD<<=WNVLA000785E>VTDXNMNO?H9<P_eYUE=PG000:"">]_aa`]B9008@VdO?18@VdO?100008@VdO?10008@VdO?108@VdO?HKf^MJ005@]^a_K__YB700FP]_aabN]UC001UTdVDGQcaXF008@VdO?10008@VdO?1008@VdO?108@VdO?10008@VdOPPP?505?]Ye_P2008@VdO?HKf^MJ08@V_O?HK__"	\
                "MJ08@VdO?HKf^MJ08@V_O?HKf^MJ00<PNd^QUHUTd_RGGGGE08@VdO?699008@V_O?100008@VdO?699000AWV`ab^OW;009B]e_P<0000FMVaaTS_`LJ0009=SfNKbQ_YYeVD40001=X_fT>A00001=X_fT>A00001=X_fT>A0000ISf`LE0008@VdO?1008@VdO_fTPF0008@VdO?WXLB50000C;>WW>Sf^MJ000EHIKcNP<00006@OdOWX>>XW@<000003=K^X[YTT_NT?100003=K^X;4BXNOM=J000003=K^X[YTT_NT?1<R_eYPLKKH0000:ULSYMDG0008@VdO?18@VdO?100008@VdO?10008@VdO?108@VdO?HKf^MJ000A?WXMXVRRI0009:ULXXVP[:0001UTdVDGQcaXF008@VdO?10008@VdO?1008@VdO?108@VdO?10008@VdO?224000:UYe_P2008@VdO?HKf^MJ01[RRRB=RRR><08@VdO?HKf^MJ01[RRRBISf^MJ00CBSbd`YKQTd_QQQQQ?Z8@VdO?100001[RRRB400008@VdO?1000005[>MXSYDH1009B]e_P<<7001HPXWDRXVPF4000GQb^]^MONVe][3003HWNeOLF<8003HWNeOLF<8003HWNeOLF<8000ISf`LE0008@VdO?1008@VdOK^cY>A008@VdO?:FE10000009<FFISf^MJ009BKKVf][C00006@OdO?F<<F:4000000ALNY?[]]]NO]?10000ALNY?5"	\
                "FX_N]]S=00000ALNY?[]]]NO]?14DTeb__bKH0000AMMW_L<00008@VdO?18@VdO?100008@VdO?10008@VdO?108@VdO?HKf^MJ00004:FB]YLXH000008EB]YLXH0001UTdVDGQcaXF008@VdO?10008@VdO?1008@VdO?108@VdO?10008@VdO?1000007BYe_P2008@VdO?HKf^MJ004<AM]]W:<2008@VdO?HKf^MJ004<<DKKVdN><000E>]aeecbcgfbbbbO?Z8@VdO?100000AM]]W:00008@VdO?10000003AMMW_L<0009B]e`MLPF0007<F:2B]YLXH000EL_cVN>]^`bK;000FLOeNMLLLB30FLOeNMLLLB30FLOeNMLLLB300ISf`LE0008@VdO?1008@VdO?Sbb]@E08@VcO?10000000000000ISc^MJ009B]ccNMF000006@OcO?Z00000000001[S`WF:==>OO?F00001[S`WF0FXOOOOS=00001[S`WF:==>OO?F00;WT`^`VKH0000AW`^O>200008@VdO?18@VdO?100008@VdO?10008@VdO?108@VdO?HKf^MJ0000009=S``KH0000009=S``KH0001UTdVDGQcaXF008@VdO?10008@VdO?1008@VdO?108@VdO?10008@VdO?1000007BYe_P2008@VdO?HKf^MJ0000HKc]U400008@VdO?HKf^MJ00004DVcf^KB30000:@QTN_________O?Z8@VdO?100000HKc"	\
                "]U400008@VdO?10000000AW`^O>20000HKcf``K;00000009=S``KH0004DOddTUKcg^MA000;Kcd````TUC0;Kcd````TUC0;Kcd````TUC00ISf`LE0008@VdO?1008@V_O?DY__SU88@QQQU10000000000000HQQQMJ009BSSKMIC000006?KKK?Z00000000004DXXU8001=@@I700004DXXU80E?@@@@@F00004DXXU8001=@@I7001;DMWMD=E0000EDMW>;900008@V_O?18@V_O?100008@V_O?10008@V_O?108@V_O?HK__MJ0000000E?MMPG0000000E?MMPG0001UT_VDGQ__XF008@V_O?10008@V_O?1008@V_O?108@V_O?10008@V_O?1000007BY__P2008@V_O?HK__MJ0001UYVPA000008@V_O?HK__MJ00004DSSSXU<0000002HU>RRRRRRRRRR[C8@V_O?100001UYVPA000008@V_O?10000000EDMW>;90000EPY___K;00000000E?MMPG0007B]__SIW__N>5000;K______TUC0;K______TUC0;K______TUC00IS__LE0008@V_O?1001[RRRB:"">RRRU60AGGGE000000000000005GGGFC000JIIH:C0000000A;;;J000000000000EFF<00003663000000EFF<000Z66666000000EFF<00003663000004EAE570000005EA<7000001[RRRB31[RRR"	\
                "B300001[RRRB30001[RRRB301[RRRBGRRR><000000004JA<7000000004JA<70003BRRR[FPRR>J001[RRRB30001[RRRB3001[RRRB301[RRRB30001[RRRB3000000IRRR?4001[RRRBGRRR><0007IDDH7000001[RRRBGRRR><00000FIIIF500000000015<<<<<<<<<<401[RRRB300007IDDH7000001[RRRB3000000005EA<7000009F?PRRPF0000000004JA<70000GRRRPFDRRRB3000FPRRRRRRRB30FPRRRRRRRB30FPRRRRRRRB300;RRRD<0001[RRRB30004<<<80<<<<4000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004<<<8004<<<80000004<<<8000004<<<80004<<<83<<<200000000000000000000000000000008<<<47<<<<00004<<<8000004<<<800004<<<80004<<<8000004<<<800000001<<<500004<<<83<<<200000344700000004<<<83<<<20000000000000000000000000000000000004<<<8000000344700000004<<<80000000000000000000008<<<<70000000000000000003<<<<7"	\
                "2<<<800007<<<<<<<<8007<<<<<<<<8007<<<<<<<<8000C<<<2000004<<<800000078880000000000009888000000000000988800000000000000988800000000988800000000000000009888000000000000000001448900000000000144890000007ZZ9000CZZZ9007ZZZ35<40000000000000000007ZZZ70ZZZ10000000C5<<80000000008<<430000000119843000009ZZ1007ZZZC0001ZZZZZZ1ZZZ00000988800000000000000001Z300076C00000094<<4900000000031ZZZ700000000000000001ZZC00CZZ1000000034<4ZZZ100000074<<80000000035<<Z00000000001448900000000000C45430000CZZZ19ZZZZ000000035<<Z000000078541000000000000C454300000078541000000000000C45430000000Z448300000000009;DDDFC3000000000;DDDFCCCCCC00000;DDDFC3007CCC90000;DDDFC300000;DDDFCC9000CCCC00000;DDDF14489000000000<IUD>?=J90000000<IUD>?=J90003I?""?=E06B?""?""?G03I?""?""?=>PDH10007J::E10000003I?""?""?I:?""?""?U200009:[>PP?F300000<I@PPDBA00004;UUI@DBE0"	\
                "000;?""?U;4I?""?""?B605U?""?""?""?""?""?U?""?""?:0000;DDDF7000003300000C;?""?[GZEB@UE0000<=DPP@I20000002;[?""?""?""?I30002:::8000000E?""?""?B88B?""?""?E00000JBDPU?""?""?U50000E=DPP@I200009AB>PPUF3000000<IUD>?=J900000005;UD>DBF808B?""?""?[H?""?""?""?A00009A[>PPUF30001:B@>DUH<000000005;UD>DBF8001:B@>DUH<000000005;UD>DBF80002I?>>@[H<00000000E>OVXUU[<0000000E>TVQUUUUUUUG000E>TVQUUB4Z=UUUH900E>TVQUUB4000E>TVQUUUI30JUUUUF000E>TVQHUD>@=J9000000:DQTNNOSP;300000:DQTNNOSP;3008@OOK>EJLOOSB78@OOOKV_OW;000JLWWL[<000006@OOO?HKOOWA0009GP]N__YMH7000<?QO__V]B9004BXYXKOVS>A003[YOTXB>OOO>50EMOOS]OOMOOSI000E>TVQUBF40<HU[F9000EMTOYWI?SOSU100E@KO__OQU<0000<?QYOOOO@8003[WWWH000000IKOO>22>OOKI0000:"">SVN>SOOMJ000<@KV__OQU2000GPSN_NYMH70000:DQTNNOSP;300000E?QYVNO]W=72POOOX>OOOK;0000GP]N_NYMH700HMSONVTQD:000000E?QYVNO]W=70HMSONVTQD:000000E?QYVNO]W=70<?QTNNOYK[700000"	\
                "04US^KTTTQG0000008[S^KTTTTTTTY[308[S^KTTTLE<PTTT?108[S^KTTTLE008[S^KMTTTL:Z[STTSB708[S^KDQTNNOSP;30000EDYadccecOLF0000EDYadccecOLF008@VbbS=HKb^WF08@VdO^^feTD400AW^a_K[600006@OdO?HKf^WA000E>Tcb`cfVPE001[S^da`a]B900;X_aX`ebb]?403[Y^e_MKbbKI90EM^fS]e_M^fSI008[S^KTT]W[F>KYYP:000EM`^eNPQ^dN>501[Saf^^e^K=3001[S^ecbbV@8002>NaXG00000ZUTeYUCCUYeT?Z006U]bfN>Sf^MJ003BKaaN_f^QI00E>Tcb`efVPE000EDYadccecOLF0000<@S^efbcfTU32P_deTKbdcK;000E>Tcb`efVPE00=]fcbfea]>E0000<@S^efbcfTU30=]fcbfea]>E0000<@S^efbcfTU37=K^dcbceY[7000000HX`KScde][C00000;W`SUTddeeeeY[30;W`SUTd`LE<P_dO?10;W`SUTd`LE00;W`SUBSbc]UFM_dNRA00;W`SD]adccecOLF0003=SbeVKKTcfY?4003=SbeVKKTcfY?401UMScV>UTeOD508@VdbSWTe`LE00AWTVf_M:00006@OdO?HKf^WA000IKcaKM]ccKI00EL_dNQLX]B908?TeVSf`Kb`W:07=MW_f]VdV><00EM^fS]e_M^fSI00;W`SUTef`K>Ybec]B700EMKW^^WTdeT?8"	\
                "0JM_dOMWNdNP<00JL_dNKXXX?600FW^NP<00000<PNfKIFFIKcNP<00JM_dOW?Sf^MJ00<PN_]>LOdTU10IKbaKWYccKI003=SbeVKKTcfY?4007=Kad_SQKTTU32P_d_aN^ccK;000IKbaKWYccKI00=]OKQSNebS[1007=Kad_SQKTTU30=]OKQSNebS[1007=Kad_SQKTTU34DVdNKQKTY[7000000=WWDOd^eNP<00000IWW@UTd`YYYYY[30IWW@UTd`LE<P_dO?10IWW@UTd`LE00IWW@<EPVd`W?YfaKI700IWW@SbeNKQTcfY@500<>Nd`WI;PVd`W:00<>Nd`WI;PVd`W:005:M`aXL_cKI908@VdN>ISf^MJ005BUM_fS=70006@OdO?HKf^WA001UTdV>AM`eY[70GQbcK=EG@;00<R_f]]e_M^cK;004EUYeNbbKI900EM^fS]e_M^fSI00IWW@U]Y`dNKbcVfY[3002?UP_^]`dNW;00GQbcKH=SfaWF00GQbcK=FFFE009=SfYU100000JM^bQPXXPQb^MJ00GQbcKIISf^MJ002@>>>:=SfO?11UTdV>GM`eY[70<>Nd`WI;PVd`W:002>Vd^QBGHDKU32P_e]`c]acK;001UTdV>GM`eY[70=KDIG=W_dNLE002>Vd^QBGHDKU30=KDIG=W_dNLE002>Vd^QBGHDKU3<P_fS=GI?WB70000004A:M`cSbaQG000004AA<UTd`L[[[[F004AA<UTd`LFFP_dO?104AA<UTd`LE004"	\
                "AA<09IQ^eYQ^eTD<0004AA>Nd`WI;PVd^WF00:W^dTXXXXSfcKH00:W^dT?87=SfcKH0004@Tf]Qb_MA008@VdO?HKf^MJ00038?TdVD50006@OdO?HKf^WA008@OdO?<P_dTU30HKcbQF008900EM`fS]e_M^fSI0000;QaddV><000EM`fS]e_M^fSI004AA<GBUX_aTfYXNQH00008=W^fcd_X=Z00HKcbQF;KcbQF00HKcbQF0000004DObQ;FF:C00:WaaXL``MXaaW:00;KcbQFISf^MJ0004222GLVcS=78@OdO?<P_dTU30:W^dTWWWWSfcKH00:W^dO@5002HJ02P_e]]TWacK;008@OdO?<P_dTU30<H<008UYeaQF00:W^dO@MMMDHJ00<HIMMMDYeaQF00:W^dOPPPPPHJ02>NdNSWPUF5000000009=Sc^W_e][C00000003UTd`XXXXW;000003UTd`XXXX_dO?100003UTd`LE000000002@Yf^Oe`WG000000:W^dT?89ISfcSI00FQbe]TbbNQbfS=00FQbe]B90GQbfS=0000IKcNYeY?8008@VdO?HKf^MJ00004?Td^WF0006@OdO@HKf^WA008@VdVDEL_eY[30;QccKI<40000EL`e]]e_XacKH0000<PNdbK=7000<L_e]]e_XacKH000000074[]fc_MBD[50009HWNeaTfTU=F00;QcbQ;IKfaXF00;QccKH290000AM`_XXXXM:00AM^bQM^^WQb^WA00GQbcK;=Sf"	\
                "^MJ000003GLTfOPA08@VdVDEL_eY[30FQbdaaaaaadfS=00FQbe]B90000002P_e]UUWacK;008@VdVDEL_eY[30000000ISfcKH00FQbe]B]^_R<00009=]^_RSfcKH00FQbd_____K;001U]cdca_YMH700000004DOdVDTeNP<00000003UTdfbbbbK;000003UTdfbbbbcdO?100003UTd`LE000000000FW_dec]U4000000FQbe]B90GQbe]=00FQbe]T``NQbf]=00FQbe]=90GQbf]=0000JM_a_bQ;0008@VdO?HKf^MJ0000:M`gfSB7006@Od_M>YeaW>E08@Vd`XDKbcSI00:M`dVXL?:0004DOdN]e_Yf^WF0000<>Od_LE00004DOdN]e_Yf^W:000000000;KbdO@8410000<>OefSSfO?K?10:M`eY>PTd_LE00:M`dVW>H1000IKccbbb_MJ00<R_f]QaaK]f_L<00:W^eY>LVd^MJ00001;LTfNM;708@Vd`XDKbcSI00FQbeVVVVVVcf]=00FQbe]=90000002P_e]B:WacK;008@Vd`XDKbcSI00000000HKfcKH00FQbe]=]e_R<00009=]e_RKfcKH00FQbgcccccK;000:"">]_afdfO><0000000:M`f]BKcaQG00000003UTdb````K;000003UTdb````adO?100003UTd`LE0000000006[ScdNL:0000000FXbe]B70;QcfS=00FWaeYLLLLKcfSI00FWaeYUC0HKc"	\
                "fSI00008?TedNPE0008@VdO?HKf^MJ0007=SffdV>2006@Ode_Nb^fNWA08@VdN`V^d_M:004?]cea_Y>E009IK^f`dabcYD50000HQagbKH00009IQ^f`dabcY?4000000000:WadY[3000000FWag`WKb^TTUC04?YcbVNcb]UZ004?]ce`VW;00ZUT````dO@4006?Yfa`NN^aeY?8004@TfbNNOf^MJ0004=MOfNWI1008@VdN`V^d_M:00FWaeYDDDDKcfSI00FWadYU100CE402P_e]B:WacK;008@VdN`V^d_M:006EZ003B]ebQG00FWadYU]__RE4006E=]__R]ebQG00FWadTQQQQQ;400EAULQ]NdaXF0000009=ScaQQQ`e][C0000003UTd`LLLLLF000003UTd`LLLL_dO?100003UTd`LE0000000000AW^eYB30000000JM`dOD21[]fbQ;00EL_dNP:E@TeaXF00EL_dNP:E@TeaXF00000HKbf][C0008@V_O?HKf^MJ000<>Vf]c^WF006@OdO`aTYa^WA08@VdO]^a_QB1000:"">SV_`bKH000<UKO_daN]PF00002DOdcdT?400002UQO_daN]>F0000000000AW^eYB7000000JM_`SU>Y_e]B700FP]_ee_]>A0000:"">]N_cT?Z06ULLLLTfS=9000GMT_NKKNc^X;0000GMOaaOSf^MJ00<UQNfOWI:[F08@VdO]^a_QB100EL_dNP:E@TeaXF00<P_d_MG<J=L=72P_e"	\
                "]B:W__K;008@VdO]^a_QB100IMBA<FRNd`MA00<P_d_MPPP?L=70IMBPPPRNd`MA00<P_d_MGGG=L=74DLBFGBQccQG0000004DOdecccfdNP<0000003UTd`LGGGGJ000003UTd`LEEP_dO?100003UTd`LE0000000000AW^eYB70000000<?YcaQ=;M_eVPA00ZU]fc]MLK^dO><00ZU]fc]MLK^dO><00000AWacK;00001[RRRBHKf^MJ000GQaaX_f]B706@OdOWX>>XW@<08@VdO?WXLB500000AB>PW^]=90002ID]e_>[:90000HQacSf`M=;30002ID]e_>[A00000000000AW^eYB7000000Z=LMUMLDYcKH0009:UKccKUA000000A[>LON>5005EEEP_aWF0000C;DR>BMTb]U40000C;>WW>Sf^MJ00;K`dTL=UL][36@OdO@WXLBG300ZU]fc]MLK^dO><001U]cfTWPMKTU32P_e]BE>RRPF00IMVdOMWXLB5000=]SMPWYcfY?4001U]cfTWPMKTU30=]SMPWYcfY?4001U]cfTWPMKTU35>NSWPL]faXF000000:M`eNNNNNcaQG0000003UTd`QQQQQ?800003UTd`LE<P_dO?100003UTd`LE0000000000AW^eYB70000007BQQYfNXPOfVQQD400FMVfc`_be_XH9000FMVfc`_be_XH900000AWacK;000004<<<8HKf^MJ001UYeV>TdV>206@OdO?F<<F:4008@VdO?"	\
                ":FE1000000035>SaS=000009B]e_>2000002DTeNM_fT]>200009B]e_>2000000000000AW^eYB700000001EE:W_Va`M:00000HKccK;00000000C=SNVD400000GQaN><00000C=DLKN`SDJ0000009<FFISf^MJ00<?SabT]T_Y[33[]f`KKKKKR<000FMVfc`_be_XH9000:ROce^_`bTU32P_e]B9<<<<700=]^d^``K;10000=]c`_^ffVMF0000:ROce^_`bTU30=]c`_^ffVMF0000:ROcf^_`bTU35>Nf^__ceV><000009=ScbQ>>>W^e][C000003UTdfbbbbV@800003UTd`LE<P_dO?100003UTd`LE0000000000AW^eYB70000007[YbbdaXP_gcbND4003HMY`aa^OXB400003HMY`aa^OXB4000000AWacK;0000000000HKc^MJ00JL_fSBKb^WF06@OcO?Z00000008@VcO?1000000000004>NNMF000009B]c_>200000;QacSUS^c_>200009B]c_>2000000000000AW^eYB70000000000:WNb^]U400000HKccK;000000000=]^KB700007B]fYUC000004DY_`TXUE00000000000ISc^MJ000E@KNbc^VKB70GWNccccc`L<0003HMY`aa^OXB400007;L]_ab^NS[32P_c]B90000000=SVcOSSK;00000=KV^aa_YMHC00007;L]_ab^NS[30=KV^aa_YMHC0"	\
                "0007;L]_ab^NS[35>]N^aa^OWI7000004DO_NPE45DO_NP<000003UT______V@800003UT__LE<P__O?100003UT__LE0000000000AW__YB70000007[Y____XP____ND4000CF?MXXMDH4000000CF?MXXMDH40000000AMQQQ;0000000000HQQQMJ00IK__WFL__]B36?KKK?Z00000008@QQQU1000000000004DQMI1000009BQQQ>200007[QQQLJ?WQQ>200009BQQQ>2000000000000AW__YB700000000005=RQX@A000000;QQQQ;000000000=QW?<000005DV_KH0000000FMQM?;4000000000000HQQQMJ0000<BLQQW>I<001ILKSSSSSL<0000CF?MXXMDH40000007:URWXW>BE02>QQQB90000000E@QQQUII200000<=>MXWLUFC0000007:URWXW>BE00<=>MXWLUFC0000007:URWXW>BE09FB>MXXMDH60000004?RRR[107=RRRD<000007=RRRRRRRR[100007=RRRD<5?RRRBC00007=RRRD<0000000000E>RRRI00000009IRRRRR>?RRRRRU6000008EFFA500000000008EFFA50000000001FGGG400000000005GGGFC00IRRR?5URRR=70A;;;J000000000AGGGE0000000000000:GJC0000000<GGG:900000<GGGF34:GG:900000<GGG:9000000"	\
                "000000E>RRRI0000000000003EFF200000005GGGG4000000000<GF50000005?RR>J00000003FGJ4000000000000005GGGFC00000CEGG:2700003J;IIIII;C0000008EFFA500000000001<"":F:23009:GGG<000000000AGGGE00000000075AFFE600000000001<"":F:23000075AFFE600000000001<"":F:<3000035JFFA59000000005<<<40006<<<200000006<<<<<<<<40000006<<<2005<<<40000006<<<20000000000002<<<1000000001<<<<<<5<<<<<400000000000000000000000000000000000000000000000000000000000001<<<504<<<8000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002<<<1000000000000000000000000000000000000000000000000005<<<000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000091454100000000000144890000000"	\
                "00144870000000CCC70007854100000000099009000007ZZZ35<4000003ZZZ700ZZZ1000000008889000000000000000000000000000000888900000088870000000003CC30000CCC7000000CCC7000C4543000000000144890000000001888880000000035<43ZZZ77ZZZ7000000000358000000000000999999990000000008<<430007ZZZ35<4000007ZZZZZZ1000000Z5<<470000000000000000007ZZZZZZZZ7000000000009853000000000000008<<430003ZZZ700ZZZ10003ZZZ700ZZZ1000000000000000000000007854C000000000000099009000000007ZZZ77ZZZC000000ZZZZZZZ700000000000000000:=UD>DUI<0000000<IUD>?=J900000<HUD>@=:30000;UUUB4CFBD>DU;5000009GB=2IBHC003I?""?""?B>P@;100C=?""?""?H9:?""?""?U2000000FDDDG00EHHGHH:0004:::E000000000000FDDDG00000A@DDH700000002[UUB400JUUUI10000;UUUB45;UD>DBF800000<IUD>?=J90000004[DD@D?<000000EB>P@=?""?""?HI?""?""?I700007<FH=>?HGA80000003HBBBBBBBBG000000<I@PPDBA03I?""?""?=>P@;1003I?""?""?""?""?""?U<00"	\
                "01I?>PPD=<0000<"":::<00000003I?""?""?""?""?""?""?""?I7000000000<I@>H8EJ290000000<I@PPDBA0C=?""?""?H9:?""?""?U20C=?""?""?H9:?""?""?U200<"":::<0000000000000C:B@>DU;40000000009GB=2IBHC000003I?""?""?II?""?""?[40000F?""?""?""?""?""?""?H90000<"":::<0000004DSTVNVTKDA00000:DQTNNOSP;3000<>KTNNOSLI600CUTTTL<IM]ONVYQUE00006@]QIM]P<008@OOOKN_TWH704@TOOD5=SOOMJ00000<DYVK=07[KKLKKD800IWWW><0000000000<DYVK=00000GWVVPA0000003=KTTME00;QTTMG000CUTTTLE?XYVNO]W=7000:DQTNNOSP;300000<>OOXVKI90000E@SN_TWTOOD>OOT?10005=>XKKN]KQMU:900005>]]]]]]]]U10000<?QO__V]B98@OOOKN_TWH708@OOOOOOW:0002PTN__VKU400AUWWWUF:900008@OOOOOOOO?100000000ADKOYBULM>;C00000<?QO__V]B94@TOOD5=SOOMJ04@TOOD5=SOOMJ0ZUWWWUC0000000C6616IM]ONVYXUE000000006@]QIM]P<000008@OOO?WOOTP<0000ISOOOOOTU1000ZUWWWUC000004DVcaabdb]U6000EDYadccecOLF000EM^fccecNWI70CUTd`L;WNffcce^SU50003[]OSTOD4008@VdO^`ceVPE00IKb^WF"	\
                "DOdT?800001=KaSU207[Y`LNVD800ISa`LE0000000001=KaSU2000006=Q`]U500000JLNf][C002DOfT@400CUTd`L@S^efbcfTU300EDYadccecOLF000009;W`K]O><0001BSab`eNKc`MW^bKH0005[QNacfdfcb`]PF00007=KOKTSKNQ;00001[S^da`a]B98@VdO^`ceVPE08@Vd`___W:0002P__N`e^W:05DQVaOQQP<00008@Vd`__`dO?10000000<D]aeYD]_^OMG00001[S^da`a]B90IKb^WFDOeYU600IKb^WFDOeYU608@VaO?10000001B@@UHWNffbce^SU400000003[]OSTOD4000008@VdOWNfTRG90000ISfa_`dTU10008@VaO?1000004DTKXWK_g_P<003=SbeVKKTcfY?400EM_]QKYbeVRJ0CUTd`L>OeaYQS_d^QH0007FR]OYWHZ308@VdaKM]cbKH00EL_fSBM`bKH0007ZFMN]@<007[]]L]]D807ISf`LEZZ000007ZFMN]@<ZZ0007Z4[K^QIZ3009=ScaXF0009=Sf^WF00CUTd`LKad_SQKTTU303=SbeVKKTcfY?4003ZZ4USVLNQH100EL_f]M_fT_cKSf_LE009IQ`d`TY`OYVccYD<0004[LQUXMBQD?F000EL_dNQLX]B98@VdaKM]cbKH08@VdOPPP>E0002PXP>W`bQF05>Ncgcb`L<ZZ008@VdOPPVdO?:J400009IKaeNKSbcbfT@4000EL"	\
                "_dNQLX]B90EL_fSBM`aQ;000EL_fSBM`aQ;008@VdO?5<400005>OOW>Vea]QS_d^X;0000ZZZ1FR]OYWHZZZ3008@VdONfTLG700000ISf`LVdTU10008@VdO?100000C=?HF:UTd_L<00<>Nd`WI;PVd`W:00EMM[;;DYfcKI0CUTd`LQaeYDG=QaeT?803I?""?""?""?U?""?""?=18@VdV>AM`eY[701U]fV>Kc_LE003I?""?WWD?U<00:=?""?""?=G97I?Sf`L?""?""?G0003I?""?WWD?""?""?""?E03I?""?""?UWW?""?=104DOdN><2225FW^fSB70CUTd`LVd^Q[GH@KU30<>Nd`WI;PVd`W:01=?""?""?;>WRWW>BC0GQbbQ?OdNTdTOeYUZ005DOd^Q?UM>[PTeaQ;000GWOOMGF[]OY@500GQbcK=EG@;08@VdN>AM`e][78@VdOWWM?J0007GPSSYbVLJ05>QVdOQQP?""?""?E08@VdO?@VdOWWM?J0004DOd`WPNeTSc`MA000GQbcK=EG@;001U]fV>KcN><0001U]fV>KcN><008@VdO?>PDH1004>NaXQbeY@GBKaeYU100F?""?""?UEU?""?""?GB?""?""?=C08@VdbeOM?J000009=]e`LVdTU10008@VdO?1000000C:"">RLXNfT@400:W^dT?87=SfcKH001G[PPPPW^dTU1CUTd`L]ebQ;02>Nd_P<08@OOO?POOO>58@VdO?<P_dTU300GX^aXTeYUZ008@OOOOOOW:008@OOO?101?OOe^OOO]=9008@OOO?>TOOWF08@OOO?POOO"	\
                ">50JM`dTUG>>>DEP_dOD40CUTd`X^dOXXXXWHJ00:W^dTWWWWSfcKH04DOOO@EHXV_VD80HKcbXUTdNKb_`bQ;000AM^dO@54E<3;Qbe][708?YeTU69FW^^WG00HKcbQF008908@VdO?DP_dTU38@Vdbaa`]?5000<P_bfVK@<00:@VdO?>TOOWF08@VdO?@Vdbaa`]@500EM`dT?XacKWabQG000HKcbQF0089000GX^aXTfSB30000GX^aXTfSB3008@VdOKV_OW;00CU]fTYebQWWWWNdN>500=KOOS?MOOK=XOO]@408@Vddda`]?500003[YeN>VdTU10008@VdO?10000000FQ_`aa]LG000FQbe]B90GQbfS=0008@V____bdVD4CUTda__d^WA01UTd^ME08@VdO?ScdN>58@VdVDEL_eY[3002DOf]_bQH0008@Vd`___W:008@VdO?101?O_ea___]=9008@VdO>Yc_X=608@VdO?ScdN>50FXbe]BIS__W:DNd_P<0CUTdfbfgbbbbbK;000FQbdaaaaaadfS=09=Sc`MADOd^VD80;QbbQ?TdNL_ddNPE000FQbe]B90000JM^dT?10JM`fSIBBB>Nf][30;QccKH<<<308@VdV>Y]_eY[38@VdOYOf^XF004;>QQS^aQ;008@VdO>Yc_X=608@VdO?@VdOYOf^XG00:Wae]=QbcQM^cK;000;QccKH8J=:0002DOf]_`WF000002DOf]_`WF0008@VdO^^feTD400FMNfadfaaaaab"	\
                "d_L<002UK^aSM^fKWNcY>:008@VdOYOf^XF00072DVdODVdTU11008@VdO?10000000FQ__^f^]?400FQbe]=90GQbf]=0008@VccccfdND4CUTdfccg^MJ0CUTd^MJ08@VdOLNdgN>58@Vd`XDKbcSI0009=Sc_b_RE0008@VdOPPP>E008@VdO?103BPSf`LLXW@A008@VdO]b_Q=4008@VdOLNdgN>50GQbe]BISfaW:DVd`L<0CUTdb`fd`````K;000FQbeVVVVVVcf]=00:M`cKIW^bKLU10AM`fSPNeYUYeeYU1000FQbe]=90000EM`dO?10GQbbQL]]]@OdVD40:M`dOWPPPG08@Vd^XSaccSI08@VdO?M^fSI00GQM?UPNfSI008@VdO]b_Q=4008@VdO?@VdO?M^fSI00:Wae]=XacKW^aXF000:M`eOL@RSB9009=Sc_bT@4000009=Sc_bT@40008@VdbSWTe`LE001=WTNdaVVVVVVVVME000<USa^K^fKNfTPF0008@VdO?M^fSI009IDQ^fSDVdT?""?UE08@VdO?10000003A>PPWOd`W:00FWaeYUC0HKcfSI000<?QQQQQ^dO@6CUTd`QYeaXF04DOd_LE08@VdO]ccgN>58@VdN`V^d_M:0000AM_geYU10008@VdO?<<20008@VdO?1008ISf`K_a^]@508@VdNb`QB40008@VdO]ccgN>50FXbeY[=SfaXF>Ng_P<0CUTd`WadTLLLLLF400FWaeYDDDDKcfSI004@TdT?Sf"	\
                "NPE5004?]c`Vc`WHQbaQ;0000FWadYU10ZC0FWaeYUC0HKcbXL`e]?OdN>504U]bf`__SI08@VdN`Vad_M:08@VdO?L`e]=00GQ`OTVcaQ;008@VdNb`QB40008@VdO?@VdO?L`e]=00JM`dTUPNeYQb_RE0004?]cf_V_]B9000AM_gbKH0000000AM_gbKH00008@VdN>ISf^MJ0008;?Ye^WDDDDDDDU50000E@Sb`bfNeOLG90008@VdO?L`e]=00CUT^d`WDVdVOOXF08@VdO?1000004B=J2<ISfbQF00EL_dNP:E@TeaXF00<>DGGGUScf]B7CUTd`LKcfS[<GM`dOD408@VdO_aTdN>58@VdO]^a_QB100006UYeaQ;00008@VdO?1000008@VdO?1000ISf^_N`d^WF08@VddcbS[80008@VdO_aTdN>50AW^dO@UYfcKIM`dN>50CUTd`LNd_M;EJ=L=70EL_dNP:E@TeaXF000IKc_MVe]U10000:PY^a_K[IKcNP<0000<PNd_MHH?[G?]fcKI00;KcbQM^eY@OdN>500A>SV_bfSI08@VdO]^a`OPA08@V_O?L`e]=00GQ_aba_]D<008@VddcbS[80008@V_O?@V_O?L`e]=005>Vd_MBKa_Tf][I5000:"">]_aa`]B90006UYeNRE0000003AUYeNR:50008@VdO?HKf^MJ00000IKcf]?A<JI>@400000EDTffggee_WG00008@V_O?L`e]=00CUT^VKUDV__fbXF08@VdO?10000"	\
                "0EMKM>DLTeaXF00ZU]fc]MLK^dO><00EMYXRLK`d`WF0CUTd`LLNd_QPMTfbK=708@VdafTTdN>58@VdO?WXLB50000EHIKcNP<00008@VdO?1000008@VdO?1000ISfc]>QbcKH08@Vd^Sb`WH9008@VdafTTdN>502>Nd^QX_V^OMSceYUC0CUTd`L]cfOWPMKTU30ZU]fc]MLK^dO><000JM_cK^aQG000009FDWXLBHDTf][C0000CBSbfOKQOSQ]^eNRJ00FXaf]Qb`VM_dT?6000EB>M^fSI08@VdO?WXQNWG01[RRRBM`fS=00J?LWSYLUA0008@Vd^Sb`WH9001[RRRB[RRRBM`fS=007=KbfOWM]fcVMWQG0000:ULSYMDG00000;QcY[C000000:MWQcYWWB7008@VdO?HKf^MJ00000ALNd^SMPMKO>200006[Kb^YceNVeT>E0001[RRRBM`fS=00CUXM>=2[RRYfTPA08@VdO?100000EM``NN_ffO><000FMVfc`_be_XH900EM^a__bd^K[80CUTd`LBK^da_`fcT>E008@Vdg^XTdN>58@VdO?:FE100009BKKVf][C00008@VdO?1000008@VdO?1000ISf^W:W^fSI08@VdO>OeOPA008@Vdg^XTdN>503BSbda^cSTf^bd^X;00CUTd`LROce^_`bTU300FMVfc`_be_XH90006UYeOfN><00000005:FE>KSa^XG000000ED]^ecbecbfcNXI300<PNdb`^Yc`ecS=900003"	\
                "JM^fSI08@VdO?:FUWI6004<<UK]bbQ;000AMMW_L<00008@VdO>OeOPA0004<<<84<<UK]bbQ;000E>Ycea^bdfc^^QG0000AMMW_L<000000GQcY[7000000FXaaeaaY[7008@VdO?HKf^MJ00000C=Q`ec`_`b_>20000;W_c]M^fKQ^bS[80004<<UK]bbQ;000<FA2704BKaOLG908@VdO?100000EMV`aba_TMH90003HMY`aa^OXB4000EMY_aa^VQU<00CUT__L<UKV^aa_]PG0008@VdfY?TdN>58@VcO?100000009B]ccNMF000008@VdO?1000008@VdO?1000ISf`LEM^fSI08@VdO?W_c]U508@VdfY?TdN>500E>]`b^Y>MOaa_KU400CUT__LGR]_ab^NS[3003HMY`aa^OXB400000HQbdf]B300000000002>_fbY@50000000A@X]TOaVOYKLI80003=Q_a^YXNa^TPJ000000JM^cSI08@VcO?10<"":1000001UTfbT><000AW`^O>200008@VdO?W_c]U5000000000CUTfbT>E0000F>SNab^VY`a^QG0000AW`^O>2000000GQcY[7000000:WQQcYQQB7008@VdO?HKf^MJ0000005[XT`aa^N]>2000E>TeOPM^fKU]c_W;000001UTfbT><000000000GM_OL;7008@VdO?1000004=DMWXWL?GC00000CF?MXXMDH40000CG?LWXM>I20007=RRRD2<=>WXWLU:9000"	\
                "8@V__WUT_N>58@QQQU100000009BSSKMIC000008@V_O?1000008@V_O?1000IS__LEM__SI08@V_O?BK__QI08@V__WUT_N>5000:?MXW@F;>WXL[<0007=RRRD2:URWXW>BE0000CF?MXXMDH4000000ERN__WF000000000002>SSQ>:0000000005GBU@XM?UIJ3000005BLXWD=LXWDF9000000JMQQQH08@QQQU10000000001USSKPG0000EDMW>;900008@V_O?BK__QI000000000CUSSKPG900000J[RWXW>?MXW>:0000EDMW>;9000000GQSS[7000000ZGGQSS[G<0008@V_O?HK__MJ00000004HDMXXW>[F9001[S__XHM__KHPO_O>E00001USSKPG0000000007BQQM;30008@V_O?100000075JFF:E40000000008EFFA5000000008EFFA59000008<<<2007<"":FFEZ000001[RRRU=RRRU80AGGGE000000000JIIH:C0000001[RRRB3000001[RRRB3000;RRRD<DRRR;01[RRRB<DRRRI01[RRRU=RRRU800004AF:409<"":FE1000008<<<2001<"":F:2300000008EFFA5000000001[RRR@2000000000007GII;<000000000000712FJ1C0000000001EF:5CEF:200000000CFGGG500AGGGE00000000000:II;E9000005EA<7000001[RRRB<DRRRI0000000"	\
                "000:II;E900000001<"":FA24JF:20000005EA<700000005IIIA0000000005IIIA000001[RRRBGRRR><0000000004JFFA<C00001[RRRU<DRRRGHRRR><00000:II;E90000000000<GGFC00001[RRRB30000000000000000000000000000000000000000000000000000000000000000000004<<<46<<<4000000000000000000000000000004<<<800000004<<<80000C<<<202<<<3004<<<802<<<1004<<<46<<<400000000000000000000000000000000000000000000000000000000004<<<5000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004<<<802<<<1000000000000000000000000000000000000000000000000000000000000000000000000000004<<<83<<<200000000000000000000004<<<402<<<3C<<<2000000000000000000000000000000004<<<80000007ZZZ77ZZZ7007ZZZ77ZZZ7000007ZZZ100ZZZZ0000000000C4541000000000000000000000000007CCCCCCCCC000001ZZZZZZZ7000000000000000000000000"	\
                "0000000014489000000000000000000000000000000000000ZZZZZZZ0009ZZZC07ZZZC000000003CCC0000000000014541000000000000Z5<2800000000009CCCCCCCC0000000000CCC7007CCC00000000000000034541000007CCCCCCCCCC9000000000145430000000ZZZZZZZZ0007ZZZ35<400CZZZ7000001221ZZZ1000000ZZZZZZZZ000000007ZZZ77ZZZ70000000007451000045400000000000000000004540000000000009854C00000000000000000000000CCCCC300000000000003I?""?""?II?""?""?I73I?""?""?II?""?""?I70003I?""?""?U2A?""?""?""?G0000000<HUD>DUH<0000000000EHHGHH:000000Z=UUUUUUUUUG000<U?""?""?""?""?""?""?H900005:::::<00000EHHGHH:000000<IUD>?=J9000000EHHGHH:0000000EHHGHH:000000F?""?""?""?""?""?""?F00;?""?""?[4I?""?""?B60000004BUUUG00000000EIUD>DU;5000000003H?>P>?;600000003IUUUUUUUUG00000000;UUUB46=UUU;9000000000005GUD>DUIJ301=UUUUUUUUUUH9000000<I?D>DUH20000F?""?""?""?""?""?""?""?J03I?""?""?=>P@;8B?""?""?I7000:U>>U?""?""?U50000F?""?""?""?""?""?""?""?J0000003I?""?""?II?""?""?I70000"	\
                "000<=D>UF30AD>DA00000000000000000AD>DA0000000007J=@>DU;500000000000CFFA90000;UUUUU[IA300000000008@OOO?@OOO?18@OOO?@OOO?10008@OOOXG=SOO]B900000E>QYVNVTQDF900000007[KKLKKD800000<PTTTTTTTTY[300:WOOOOOOTUC0007BWWWWW?60007[KKLKKD80000:DQTNNOSP;300007[KKLKKD8000007[KKLKKD800000=SOOOOOSI03[SOOXHROOT>5000000EMTTTU30000007BKTVNVYQ[400000004>TN_NTWH90000004DTTTTTTTY[3000000CUTTTL<<PTTT?10000000000EUXYVNVTKLF02>TTTTTTTTTT?100000<@KTNNVYKH0000ISOOOOOOK;08@OOOKN_TMHWOO]U100:"">]NN]SOOMJ0000ISOOOOOOK;0000008@OOO?@OOO?10000004UKONYMG1[SNWF00000000000000001[SNWF000000003;PSONVYXU6003ZZZC2<ZFMXP<000CUTTTTTYKLH70000000008@VdO?@VdO?18@VdO?@VdO?10008@VdeYU>Vde]B900000AWaecbfeaYP:00000007[Y`LNVD800000<P_eeeedddY[300:WabbbedTUC0007[YaaaVD80007[Y`LNVD8000EDYadccecOLF00007[Y`LNVD8000007[Y`LNVD800000=Sfa___SI00J>OeO"	\
                ">]c_W;0000000EM`dTU31ZZZ007BYcaafe`X;03ZZZ704>NNVaeOD40000004DNddeeddY[31ZZZ00CUTd`L<<P_dO?19ZZZ10000<@S^efccfcK;02>_eeeddeeeO?Z10009=KadcaafKH0000ISffbbecK;08@VdO^`eeVPOeNL:006U]cf^^]f^MJ0000ISffbbecK;0000008@VdO?@VdO?1000000FW`OS^T@FM_]U80000000000000000FM_]U800000000GMOcecceND403I?""?""?B>PU[]TP?F90CUTddeeecVRJ4<<5100008@VdO?@VdO?18@VdO?@VdO?10008@Vdg_MQage]B900000AW_YKQSNec]U8000ZZZ1[]]L]]D8ZZ300<PYYYYYOdbS[300:MXXXSbaSUC007Z[QQQQQ@8007Z[]]L]]D8003=SbeVKKTcfY?40001[]]L]]D80007ZZ[]]L]]D8Z700<=Sf`LPPP;009HX`bS`bS[80000000EM`dTU<U?""?""?G07BYKWX]ce]BZB?""?""?I34DW>DSfN>50000004DNdNYY^dY[<U?""?""?G0CUTd`L<<P_dO?1;?""?""?U5007=Kad_]QK]NK;02>YYYYfbYYYY?""?U<006?OdaSWXSKH0000ISf`XXacK;08@VdaKWYcbSb^QI300JM_dOWXNd^MJ0000ISf`XXacK;:<30008@VdO?@VdO?:J40000=SbQ@O_LU]_M:00000000000000004U]_M:000000005DTfbTKK]ND408DOOOK"	\
                "N_]L_N_YPA0CUTd`]]NecK=DPP>UI8008@VdOSSVdO?18@VdOSSVdO?10008@Vd^cSTbbe]B900000AWMUHG=MNd`MA00F?""?""?U:U?""?""?=B?""?""?=C01I[[[USafYDE000CFF[K^b]DE003I?""?""?II?""?""?=13I?""?""?=I?""?""?=10<>Nd`WI;PVd`W:005U?""?""?H?""?""?""?E03I?""?""?==H==?""?""?I7CUXSf`XXXH30004UScaeT>E000008EFFM`dTUAWOO]B90;DIFFM^eY[5>OOO@80JDSSVa]UZ0000004DNdN>M^dY[AWOO]B9CUTd`LFFP_dO?1[YOOME002>Vd^Q[G;BRK;0CI[[[SfbQ[[]OOW:006@OdN>:F=DF0000=Sf`LWacK;08@VdV>GM`dcc]?5000GQbcKIB]e^MJ0000=Sf`LWacKWWP=4008@VdOSSVdOWWM?J009=]aXUT^MW`SB10000000000000000GW`S[100000000FW^eT>>>?XLI98DVdO^aebSbaecSB7CUTd`LBQbfSKV__NYWA008@VdcbbcdO?18@VdcbbcdO?10008@VdO`V`V_e]B900005I======BSfcKH00=KOOS?MOOK=XOO]@400973=Q`eOLF000003=X_fT>A0008@OOO?POOO>58@OOO?POOO>50:W^dTWWWWSfcKH00EMOOSIQOOQF08@OOO?108@OOO?11UTbdfbbKH00000J>Vd^W;00007F?LWXX`dTUAWae]B907A>RLSbaKH5>NdV@804DNcfTX=30000005>Nd"	\
                "NDM^dY[AWae]B9CUTd`XXXX_dO?1[Yd^ME00:W^dO@52>KKK?Z0077ISfbQF=]eaW:007BSb^KLRD<00009=]e_RWacK;08@VdO?<P_ggNL:0000;KcbQFISf^MJ0009=]e_RWaeaaaNXI908@Vdcbbcgbaa`]@500HKcKPNNPYNRJJ<90C<AE600000005@YNRJ000000000ISfbQKNO]`Y[78DVdaKW_dbSWNdO@8CUTd`L=QbcK^c_N`^WA008@VdOQQVdO?18@VdOQQVdO?11008@VdOTfcS_e]B1C000FQSSSSSSSScfS=002UK^aSM^fKWNcY>:000004>Nd_QMUE00008DVdOKW@A008@VdO?ScdN>58@VdO?ScdN>50FQbdaaaaaadfS=00EM^fSIQbbQF08@VdO?108@VdO?11USSf`SSKH00008[SbbeT>E000FLY_abbfdTUAWae]B900FX_`bVK@<5>NdV@804DQKTfNP<190000<P_dO@M^dY[AWae]B9CUTdfbbbbcdO?1[Yd^ME00FQbe]B92P_cO?Z0000ISfbQF=]eaW:000J>S_a`_ME0009<[YeN>WacK;08@VdVDEL_dfc]?5000GQbcK;=Sf^MJ009<[YeN>WacYT_dOD508@VdOQQVdOYOf^XG00JRO`N^SX^K@MM>H2BPWLUE000000;Q^K=3000000009B]e^MKcVa_Y[78DVdN>@OdNPUTdN>4CUTd`]]NfVSfOP>MKWA008@VdO?@VdO?18@VdO?@VdO?""?UE08@VdO"	\
                "Qa_M_e]?""?[500FXbfffffffdf]=000<USa^K^fKNfTPF0000004>Nbcb`S?50008DVabc^]D<08@VdOLNdgN>58@VdOLNdgN>50FQbeVVVVVVcf]=00EM^fSIQbbQF08@VdO?I;E@VdO?10F=Sf`LRRH0009;W`cS^bS[801UYfe^``bdTUAWae]B900FX__acNX;5>NdV@800JG;Wa^M?""?;0000EM`dTUM^dY[AWae]B9CUTdb````bdO?1[Yd^ME00FQbe]=92P_dO?Z0000ISfbQF=]eaW:007=K_ca__ME000J>Q_dT?WacK;08@Vd`XDKbc]c^QI700:W^eY>LVd^MJ00J>Q_dT?WacKUSf`ME08@VdO?@VdO?M^fSI007;LKSXDTO>Y`^VWBKN^_SU40000<DTV>E0000000007B]e^MKcaKPXB78DVdO?""?OdVDUTdN>4CUTddeef]MKc`YSXPB5008@VdO?@VdO?18@VdO?@VdNOOQF08@VdO>QQP_eOOOME00:W^dY]]]]]ffSI0000E@Sb`bfNeOLG900007C4DXQKVe^XF0006?WWQTfaQ;08@VdO]ccgN>58@VdO]ccgN>50FWaeYDDDDKcfSI00EL`eYMQbbQF08@VdOSSQP@VdO?100=Sfb__]=900E>OeOP]c_W;02>Nd_XLM`dTU:Wae]B900A>PLK^eT?2>NdV@80000:WaaOOY[7000FXaf]=M`dTU:Wae]B9CUTd`LLLL_dO?1UYd^ME00FWadYU1<P_dO?Z0000ISfbQF=]ea"	\
                "W:002>Nd_QPPD<900FXad^QIWacK;08@VdN`V^dNMOeVL:004@TfbNNOf^MJ00FXad^QIWacKHKf^MJ08@VdO?@VdO?L`e]=0007JHIIQaQKbSQ^TRNNWT^WF0009IQaQI90000000000=SfaWKcN><F<08DVdO?""?OdV@UTdN>4CUTd`]TbcTMVbefaVWG008@V_O?@VdO?18@V_O?@V__fbQF08@V_O?GGP___e^ME00<PNdNP==@TeaXF0000EDTffggee_WG00003IUFJFFIQbfSI00<I:::;Xae]=98@VdO_aTdN>58@VdO_aTdN>50EL_dNP:E@TeaXF005DOfc^^fbQF08@VdcbcbOLVdO?100=S__ae]=901[S__QHLNeOD5<R_dT?FW^dO@GQbf]=000022<DNdN>EL_dT?10000:W__^dY[701EU]fbQ;L_dO?GQbfS=0CUTd`LEE>NdVD<?Od_L<00ER_g_M;JM`dTU10000HKccKHUYe^MA00EL`dTU<2:[I30FX_OQU:WacK;08@VdO]^aNQBW__]?400GMOaaOSf^MJ00FX_OQU:W__KHKf^MJ08@V_O?@V_O?L`e]=000000E>OTDY^WDV_Wa]BKcKH000E>VTD<00000000000GQbf]KcO@PU808DVdO?@OdV@UTdN>4CUTd`LDYeaQRK]T^e][701[RRRBDVdT?11[RRRB[RRYfTR:01[RRRB35?RROb]D<001U]fc]MLK^dO><0006[Kb^YceNVeT>E0008D]WD[=UKcfSI00HK"	\
                "M@[?KbfS=08@VdafTTdN>58@VdafTTdN>50ZU]fc]MLK^dO><000;M]YYYbbQF08@VdOQK`cKVdO?100;RRL_e]=901[RRRU;UYdN>22>Nd_XMYbd`QLYfbQ;000000Z?Te^KRSbf]B70000J>RRNdY[79ILK`d_LEDOd`QLYfaQG0CUTd`L<Z?Td^QRQ`dOD4001U]cfOWPScfS=90000FW^eTMW_dN><00<P_dNWD>MS>40J>P@IFHXabQG08@VdO?WXLB5[RRRU800C;>WW>Sf^MJ00J>P@I5E>RRPISf^MJ01[RRRB[RRRBM`fS=000003=K^XUY^WDV_Xa]BKcKH003=K^X;000000000000<PNd^KcOQOD408DVdO?W^dV@UTdN>4CUTd`LGX^eTMDU@OdTU3004<<IKS^f]B704<<<84=KaOLG7004<<<8005UScY>:0000FMVfc`_be_XH9000;W_c]M^fKQ^bS[8008DV^VY]Y`d^XF00HK`OYT`d^XG08@Vdg^XTdN>58@Vdg^XTdN>500FMVfc`_be_XH9000CFB[[QbbQF08@VdO?>NfSVdO?100CUK]bbKH0004<<<GQS_dT?81UYfea`aKad^_ceO><0000000HX`da_bd_MF000000<<>NdY[77[Ybdb]UZIKad^_ceO><0CUTd`L<0HQ^da_^daKI9000:LOfe^_adNM:000005DTff`^ebSBC00ZU]ce`NN`N>400<<44DKYf^W:08@VdO?:FE104<<<400009<FFISf^MJ000<"	\
                "<4000<<JMKVdN><004<<<84<<UK]bbQ;00000JRNY?IScSX`TL_VWT^WF00ARNY?50000000000007IQ`dbd^aND408DV_O?]__V@UT_N>4CUTd`L<DTdf`OTTacS=900000ISfcNMF0000000FMNVM;70000000007IX`T>F00000CHMY_aa`OXB4000E>TeOPM^fKU]c_W;008DT`bfefc`S?500HK^cffb_K?508@VdfY?TdN>58@VdfY?TdN>5003HMY`aa^OXB400000097FQbbQF08@VdNOVcaQVdO?1003UTfcOPE0000000GQbc_QH00FLT^b`]DSNaa^TMH900000004[QV^b^VQBZ00000005>NcY[77[Y^NSDJ0<@SNaa^TM;90CUT__L<05UKN^baNS@<00007;L]_aa^OX=1000000GLY`aa_]DE0000:P]_aba`O>4000004>Nfa]?408@VcO?100000000000000000ISc^MJ00000000000JM^f^KB3000000000CUTfbT>E0000CBS`WGJPTaaNW[K_^^]?501[S`WG000000000000005[QO^aa_]D401[RRRM_WRR[=RRRU8CUT__L<HQ___^aa`YP:000000ISSKMI10000009=QQMH300000000001?QQPF0000000CF?LXXMDH40001[S__XHM__KHPO_O>E09;?MKS]SKMUE000FDWKSKQL[<008@V__WUT_N>58@V__WUT_N>5000CF?MXXMDH4000000000FQ__QF08"	\
                "@V____NS@V_O?1003USSKP;90000000GQSKWB4003GDWXMUJ[RWXMD;10000000005I>WXW>I4000000005>QQQ[77BXM>BE000E[RWXMD;C007=RRRD200<=PWXWPBE0000003FULWXW>I400000003F@MXXLUJ000009:URWXWLDI3000004>SSQD:008@QQQU100000000000000000HQQQMJ00000000000JMSSXU<0000000000CUSSKPG900004@XXU49G>WXR=<[MXW?J004@XXU40000000000000004HDMXWRU:0004<<ISSD<<46<<<407=RRRD2<DRRRMXXM@F9000000EIIH:C00000000<GGFC000000000000JGG:90000000008EFFA5000001[RRRU<DRRRGHRRR><0004A;===HA8000005:;IHGE10001[RRRU=RRRU81[RRRU=RRRU8000008EFFA500000000000:PRR>:01[RRRRR>B[RRRB3000:IIHJ7000000005IIIF8000005AFA401E:FA500000000000092:F:<70000000000:GGG<002FA23000001E:FA5000008<<<200003<"":F:<C0000000006EFFA5900000000004JFFEZ000000001<"":F:E490000009FII;<0000AGGGE0000000000000000005GGGFC00000000000Z;IIF5000000000000:II;E9000000EFF<0002:FE306JF:40000EFF<0"	\
                "000000000000000005JF:<10000000EIIF00000000008<<<2002<<<AFFJ4000000000000000000000000000000000000000000000000000000000000000004<<<402<<<3C<<<20000000000000000000000000000004<<<46<<<4004<<<46<<<4000000000000000000000009<<<<9004<<<<<534<<<800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000CCCCC3000000000000000000000000144Z0000000000945Z0000000009Z45470000000000000000000034410CCC30000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000130007CCC00000000000000000000007CCC000000003454C00000000003CCC9000003CCCCCCCC70000ZZZ100009Z4410000000003CCC300000000CCCCC30000000CCCC37000000000CCCCCCCC0000CCCCCCCC0000CCC7007CCC0000CCC70000CCC7003CCC30000CCC700000000CCCC9000CCCC7000CCCC009CCC0000CCCCC3000000000;UUUUUU=:4FFF:3000000000000004;UD>?I<00000004I@>?G3000008GB?D>DBA9000005:::<0000007:[DDUGUUUB409:F1000000000000000000000001::700EGGGGGGGGGGGF809:GGGGGGGGGGG:700000005F2000000001FGGGGGGGGGGGJ001FGGGGGGGGGGGJ000000008II10000000000008II10000000001JF27;?[E08=UUUF0000004I==F==G90001A;BUUUG00000EH[D>DUG600000002[UUUI30004BUUUUUUUUIC00:?""?""?U<09AI?>>U;80000004BUUU[<000000;UUUUU[I:C000;UUUUUBHJC00000;UUUUUUUUG00;UUUUUUUUG00;UUUB46=UUU;90;UUUB400;UUUB45BUUUB400;UUUB4000000;UUUUH70:UUUU=80;UUUUE3IUUU;90;UUUUUU=:100000"	\
                "CUTTTTTYSM=XXXLJ0000000000000<UXYNNTKD<000007IXONTMG00000FW]TNNOSPF00007BWWW@400009GR]VNYWSTTME02>M[A300000000000000000007JBMP<0CUQQQQQQQQQQQWF02>QQQQQQQQQQQP<0000009=Q=30000000:WQQQQQQQQQQQ?80:WQQQQQQQQQQQ?80000000AMLE000000000000AMLE000000000HMX[FLTSB9<RTT]=900000:WSK?SSD4000GMQ]TTY[30004@QYVNVYW=6000007IQTTTD4000ELTTTTTTTT>400HKOOWA0<PSTNNTX=100000:MTTTQG00000CUTTTTTYSMIC0CUTTTTY]KL=5000CUTTTTTTTY[7CUTTTTTTTY[7CUTTTL<<PTTT?1CUTTTL<0CUTTTL<BQTTTLE0CUTTTL<00000CUTTTT>23B]TTTR<CUTTTS=4DTTT?1CUTTTTTYSM=60000CUTddeedfNWKb^MJ62<2870000008[K`ecab^ME000005>V`YcYU10000FQbfccecTD50003UTaN>40000APOcdccNSf^ME0<P_SR=<90000000000000000<IPS_L<0CUTbbbbbbbbbbQG08UYbbbbbbbbbT?4000000<>VPE0000000:WTYYYYYYYYYV@80:WTYYYYYYYYYV@80000000HKQ;000000000000HKQ;00000000CUTNP>T`QI0<R_e]=900000HKaWP_Y[3000HKbffdY[30"	\
                "004DNfbafe_WF00000JPVddND4000EL`eeeeddN>400HKfaWA0<L`caceNM:00009=Scde][C0000CUTdeccecNM:0CUTddedecNQ[400CUTddeeeeY[7CUTddeeeeY[7CUTd`L<<P_dO?1CUTd`L<0CUTd`L=X_eNWH30CUTd`L<00000CUTddaXG<PNdd_R<CUTddNPEDNdO?1CUTddeedfNW;0000CUTd`]]TceTKf^M?""?>P>@B:00001FW`dNKWKVME00000EL`OM^ND4001JFQVSQKYc^WF5004UTdN>5000C[ScfOKKONe^ME0<P_cNK>H200000000000005H>KVc`L<0CUTdggggggggbQG00GW^gggggggaQ;0000000HQaKI9000000:WY[[[[[[[[LV@80:WYLWWWWWWWLV@80000001UT][300000000001UT][300000000IKaQ]bSU50<R_e]=9004I=[Y_PWaK=H600HKO]_dY[30004DN]QWYff]B7000Z[ScNdND4000EL]]]]]_eT?800HKbaWA0<LVKWSbcKH00004DOd^eNP<0000CUTd`KKTfcSI0CUTd`YYVbd`X;00CUTd`YYYYY[7CUTd`YYYYY[7CUTd`L<<P_dO?1CUTd`L<0CUTd`LWNdNWI100CUTd`L<00000CUTdeeYU;Qafg_R<CUTdgbK=DNdO?1CUTd`]]TceT?8000CUTd`L=PNg_OfaOOON_NO][3007=P]faQPPPLME00008EL_VM^VD400GMMMMMMMW"	\
                "^cQMU14U>TdN>D<00<PNd_MHHM_g^ME0<P_gdbOQDG40000000008F?XOadg`L<0CUTdggggggggbQG005@TegggggdOD20000005DTeOD2000000:WY[777777<LV@80:WYWaaaaaa_LV@8004AAAAP_N>AAAA8004AAAAPS]>AAAA80000:M`T^T>E00<R_e]=900FXSSVOSS^SSME00F>?>_dY[30004DXUGFM`dTUC000FW_VTdND4000ZI====KcaQ;000HXQQMA0<>R=:W^cKH0000:M`cSbaQG0000CUTd`LHWae]=0CUTd`LU>SbeT@40CUTd`L[[[[F0CUTd`L[[[[F0CUTd`LFFP_dO?1CUTd`L<0CUTd`MNe_Q=8000CUTd`L<00000CUTdN`_L?Tc]e_R<CUTd^bV>DNdO?1CUTd`L=PNgN>2000CUTd`LF>Ndcbdfbcd^N_aY[300<>V`da___T?GC000A?PDOaKbTU100HK^^^^^^^fe^O?Z2>__d^_MJ00:W^dTUZ1UTd^ME0<P_gggd^TWUF100000C:UWY^eggg`L<0CUTdggggggggbQG000HQagggggbKI0000000GW^gaQ;000000:WY[700000<LV@80:WYWaggggg_LV@800=WWWWQa^WWWWWH00=WWWWQWQWWWWWH00004@OeNMG000<R_e]=900FXaafaabba^ME00922>_dY[30000:G40:M`eY[3002@YaQTdND400000992DOdV><0004FFFF309JJAD]f^WF0009"	\
                "=Sc^W_e][C000CUTd`WWScbKH0CUTd`L<E>Nd`LE0CUTd`XXXXWG0CUTd`XXXXWG0CUTd`XXXX_dO?1CUTd`L<0CUTd`Oe`QB40000CUTd`L<00000CUTdNTbKM`_]e_R<CUTdNNbKDNdO?1CUTd`LF>Nd_P<000CUTd`XXSadOKf^X_bQ>PX][300FQ]_dO]]]Q;0000ED]_TQ`bbKI000HKKKSagggeNKK?Z2>_^da^MJ00GQbe]=90=]e^ME0<P_ggggge`]M[J000EBM]`fggggg`L<0CUTdggggggggbQG000<>VdgggdNPE0000008UYegeT?400000:WY[700000<LV@80:WYWaggggg_LV@800;MT^^^ef^^^YLG00;MTQQQ>PQQQYLG0000<DYeOD50002>NfSI000:MWQbQWNTWWPE00002>_dY[30000000JD]f^XG007IQaYDTdND40000000GX^cS=7000GLLL>E000<@]bb]U40004DOdVDTeNP<000CUTdcaaca]>E0CUTd`L<3UYdaW:0CUTdfbbbbK;0CUTdfbbbbK;0CUTdfbbbbcdO?1CUTd`L<0CUTdee^KU500000CUTd`L<00000CUTdNKbOScY]e_R<CUTdNKcO>NdO?1CUTd`XXSadOD4000CUTdfbbcd^KKf^MNfO]KM?F0002[Q_dOQQQ?2000CBSb`cOVdb]U400GMMSbdggbTWMMU15DMTdNMPE00GQbe]=00=Sf^ME0<P_gggggggf_S><05DSNcggggggg`L<0"	\
                "CUTdggggggggbQG0009IKbgggcS=3000000:M`ggg^WF00000:WY[700000<LV@80:WYWaggggg_LV@8003G>]adggd^SDF7003G>]Q[A:UQSDF70006BKa^^XG0004DVcK;004BUU]`MM^SUUJ900002>_dY[3000000JD]aaSU400JPV_WUTdNDG400000ZUYe_MA0000HK``WA000HQac]DJ0000:M`f]BKcaQG000CUTdaNN_f`K=7CUTd`L<3[YeaW:0CUTdb````K;0CUTdb````K;0CUTdb````bdO?1CUTd`L<0CUTdeec]DJ00000CUTd`L<00000CUTdNRN^VaQ]e_R<CUTdNRNaQNdO?1CUTdfbbcd^KI9000CUTda__NTQUKf^MS^ffc`]?400ELNbgcbbSB<<00ZUK_WYccV``W:00HK^^dfffc^^^O?Z8U>TdN>?<00FXaeY[33[Ye^ME0<P_ggggggdaTWU504UWY^egggggg`L<0CUTdggggggggbQG0000EPNdgd_LA000000C[]fgggf]UZ0000:WY[700000<LV@80:WYWaggggg_LV@80000A?Kagg^KUE000000A?KQGHKKUE00000GW__KbY?4001UT^QF00<LTTVNTTbTTQF000002>_dY[300000AD]aaS@E000;KcSQQTdNQQ;00000AM_eYU60000HKfaWA009B]`_LA00009=ScaQQQ`e][C00CUTd`L>MNdVD4CUTd`L<5DOd^MJ0CUTd`LLLLLF0CUTd`LLLLLF"	\
                "0CUTd`LLLL_dO?1CUTd`L<0CUTd`Oeb]@E0000CUTd`L<00000CUTdN>]ffN>]e_R<CUTdN>ScONdO?1CUTda__NTQU20000CUTd`LP>?IHKf^M?WSYNf`MJ00FWQTe^QQM:@><00<BW[PSKLTcQ;00HKK_dTKKKKKKK?Z2>NNd`NMJ00EL_dN>JJ>Vd^ME0<P_ggggdbOQDG40008G@QOadgggg`L<0CUTdggggggggbQG00003BScgf][1000000JLNdgggg_M:0000:WY[700000<LV@80:WYWaggggg_LV@800000IScffbKH00000000ISLLPMKH00000<DTbS>O^XF004[MMPG00<L__b__`a__QF0003EE>_dY[E<000AD]baS@FF<00;KcccccgfcK;00007=ScaXG00000ISf_WA000IMMMD500004DOdecccfdNP<00CUTd`LE?Od_P<CUTd`L;[Q^dV>50CUTd`LGGGGE0CUTd`LEEEE30CUTd`LEEP_dO?1CUTd`L<0CUTd`MNeaSU<000CUTd`LGGGGE0CUTdN>W^f]B]e_R<CUTdN>LNaNdO?1CUTd`LP>?I500000CUTd`L<280HKcaXQP?UXbaW:001FHQaeOMDM]ME000Z:2AIB>ObQG002H>NdVWPMKOXH:02>_adb^MJ008?Yeb]LL]bd^ME0<P_ggdcNK>H200000005H>KVcdgg`L<0CUTdggggggggbQG00000:M_g`MF0000007=KcgggggcSB3000:WY[700000<LV@8"	\
                "0:WYWaddddd_LV@800008?TbYTcYU10000008?TWYTMYU10007=KaO>;XN>J00<RVV]=902DRWaKROVRRP:000FMMM_dYMM><04D]baSXXXXU10;KNNNNNd`NK;0000<>VdO>200001UTcS[6009B]VVP<0000:M`eNNNNNcaQG00CUTd`MMQ_dN>5CUTd`QQ]`daK=70CUTd`QQQQQ?6CUTd`L<00000CUTd`L<<P_dO?1CUTd`L<0CUTd`LWNd^K[500CUTd`QQQQQ?6CUTdN>DOOWB]e_R<CUTdN>[ScddO?1CUTd`L<280000000CUTd`L<000FXae^^NTTNe_LE0000<DYcf_N_^ME0000000HKTb_MJ000C[]cf^_`bbXF005DWTdNWRE000GW_dc__^Sf^ME0<P_gf_SL=E90000000000<IPS_fg`L<0CUTdggggggggbQG00000ZU]fY?8000000<>VdgggggdNPE000:WY[222222<LV@80:WYWVVVVVVVLV@80000EL_S>>]NP<000000ELSS>>]SP<0008@OaQI4[M=C00<R_e]=9002IKaWL_][<<9000GQ^^bd^^`L<04>NgfbbbbO?Z0:D>>>>TdN>D:0000;QacKI90000<>NVPJ0009B]e_P<0009=ScbQ>>>W^e][C0CUTdb``adb][CCUTdfbcfea]D<00CUTdfbbbbO@6CUTd`L<00000CUTd`L<<P_dO?1CUTd`L<0CUTd`L=Q_d`QB40CUTdfbbbbO@6CUTdN>H?""?[B]e_"	\
                "R<CUTdN>:M_ddO?1CUTd`L<000000000CUT__L<0002D]N__`aa^NKB100000F>]_aa`TME0000000HK^NQB10000:"">]_ab^_YXF000EUTdN>J70006BQNaa_KS__ME0<P_^]M[A300000000000007JBM]``L<0CUTdggggggggbQG000000FW^XG0000000HQbgggggggbKI900:WY>>>>>>>>LV@80:WY>>>>>>>>LV@80000;QK?EA@KXF000000;QK?EA@KXF0008?TTD204EZ000<R__]=900CUYNPX_KH000000GQ_______L<04>N______O?Z004444UT_ND400008?T__LJ00000ELQQI70009B]__P<0004DO_NPE45DO_NP<0CUT_____VS>A0CUT____NTQ@A000CUT______O@6CUT__L<00000CUT__L<<P__O?1CUT__L<0CUT__L<BQ___XI9CUT______O@6CUT_N>511CB]__R<CUT_N>5[]__O?1CUT__L<0000000007=RRRD20000AUPRRLWXWP=20000000AULWXM?GC0000000HXW>=5000000:UMWXWPU;40003UT_N>4000004IPWXL[RRRD<0<PTW?F1000000000000000001:UWTL<0CUT__________QG0000004?TD50000005DO_________V><00:W__________V@80:W__________V@80000HMB200<[MG000000HMB200<[MG0000GMRG000000005@"	\
                "RRRH000CBRRUPR>A000000FPRRRRRRRD208URRRRRRRR[C000003BRRRU600006URRR[1000003FFF400000IRRR@50004?RRR[107=RRRD<07=RRRRRPDBJ007=RRRRP>UH<00007=RRRRRRRR[17=RRRD2000007=RRRD25@RRRB37=RRRD207=RRRD25URRRRI07=RRRRRRRR[17=RRRU8000IRRR@57=RRRU8FPRRRB37=RRRD200000000008<<<20000001<<<E:F:<30000000001EFFJ40000000004F:<7000000008EFF:<Z000007=RRRU80000007<FFE1<<<2004?D;40000000000000000000004G@@207=PPPPPPPPPPP>:0000000;PH00000004?PPPPPPPPPPP@200E>PPPPPPPPPPP[10E>PPPPPPPPPPP[100001E800004EC0000001E800004EC0000CJE70000000005<<<100004<<4<<<90000007<<<<<<<<20004<<<<<<<<400000008<<<40000004<<<400000000000000001<<<5000005<<<40006<<<20008<<<<<<4700008<<<<<5100000008<<<<<<<<4008<<<200000008<<<2005<<<8008<<<20008<<<2004<<<<1008<<<<<<<<4008<<<400001<<<5008<<<407<<<<8008<<<2000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000006<<<400000000000000000000450000000000000000000000000550006<<<<<<<<<<<2900000003<C000000004<<<<<<<<<<<500002<<<<<<<<<<<80002<<<<<<<<<<<80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000CCCCC300000007CCCCCCCCCC9007CCC90000CCC30000CCC700CCC3003CCC0000CCC30003CCC00003CCC9000CCCC00003CCCCCCCCC00000000000000CZZZ00000000145870009CCC7007CCC90000000770000000000000770000000000000CCCCCCCCCCC00000CCCC37000000000CCC70000000000CCC7007CCC00000CCC70Z881000CCC70000000000CCC3000000000CCC70000000000007CCCCCCCCCC9000"	\
                "000000000000ZZZ100000003CCC300000000CCCCC30000000CCCCCCCC90000003CCC300000000CCCCCCCC00003CCCCCCCCC0000CCC7007CCC0000CCC70000CCC7003CCC300000003CCC300000000CCCC9000CCCC7000CCCC009CCC000C2224000;UUUUU[IA30001=UUUUUUUUUUH91=UUUH700GUUUB400FUUUB4JUUUU<2[UUUA00FUUUU<02[UUU;002[UUUI30AUUUUF00<[UUUUUUUUUG0002FFFA00004[?""?""?G00000ZGUD>@BF03IUUU=8Z=UUUI30000<HB[I:100000000<HB[I:1000000000GUUUUUUUUUUUA000FUUUUUBI:Z00000;UUUB400000000;UUU=88=UUU;900;UUUB2UDDB60;UUUB400000000FUUU[50000000AUUU=800000000001=UUUUUUUUUUH9002FFFA000000:?""?""?U<000004BUUU[<000000;UUUUU[I:C000;UUUUUUUUH700004BUUU[<000000;UUUUUUUUG00<[UUUUUUUUUG00;UUUB46=UUU;90;UUUB400;UUUB45BUUUB4000004BUUU[<000000;UUUUH70:UUUU=80;UUUUE3IUUU;90HPPP?50CUTTTTTYKLH7002>TTTTTTTTTT?12>TTT>208?TTTME09B]TTMJIKTTQGFWTTKI07BSTTK=CIQTT]U30AMTTTLA6[STTSB90"	\
                ":WTTTTTTTTY[309=XXX>2000<ROO]=90003IWYNNO]?14DTTTWFFMTTTD4003;>Q]YSMB2000003;>Q]YSMB200000008?TTTTTTTTTTKH009B]TTTT]KM[<000CUTTTL<3000000<UTTTLFFRTTTU<0CUTTTL:WVOD5CUTTTL<00000007BYTTMJZ000000HKTTR<00000000002>TTTTTTTTTT?109=XXX>200000HKOOWA00000:MTTTQG00000CUTTTTTYSMIC0CUTTTTTTTT@60000:MTTTQG00000CUTTTTTTTY[70:WTTTTTTTTY[3CUTTTL<<PTTT?1CUTTTL<0CUTTTL<BQTTTLE00000:MTTTQG00000CUTTTT>23B]TTTR<CUTTTS=4DTTT?19=]__R<0CUTddeeecVRJ002>_eeeddeeeO?13B]f^WF0JM_dT?800;QbbQG[YefS=IKcaWF00JPVdNM:PVdNL:001[Scc]UFW_dVPJ00:WaeeeedddY[303B]b_P<Z00<R_e]=9000JLNecabO?10;X^fY?""?Yf^X;009;MO_TYV_KU50009;MO_TYV_KU5000000:M`dcfddeeefKH009B]ededec_KU200CUTd`LU[I:C003[XTd`XXXX_dTXUCCUTd`L;KcKI9CUTd`L<00000007BYe^M?[400000HKf_R<ZZ35<400002>_eeeddeeeO?103B]b_P<Z0000HKfaWA00009=Scde][C0000CUTdeccecNM:0CUTddeeeeO@60009="	\
                "Scde][C0000CUTddeeeeY[70:WaeeeedddY[3CUTd`L<<P_dO?1CUTd`L<0CUTd`L=X_eNWH300009=Scde][C0000CUTddaXG<PNdd_R<CUTddNPEDNdO?19=]b_R<0CUTd`]]NecKH002>YYYYfbYYYY?10FXafSB7HKccKH000AM^fS=DVbcT?[Ye_R<007IQ^f]UScaK=3000JPVd_W@YfaQI700:WYYYYYOdaKB73=?]e_P?""?G0<R_`]=9000HKbc]WQY?104?Yf^QX^fY?500JPOTMMWMK`QI900JPOTWWWRK`QI900007=ScaQNdNYYYYKH009B]eaYYVad^QI70CUTd`TTYSMIZ0CUTbgcaaaacgbTUCCUTd`LB]_LJ0CUTd`L<EEE<0007BYe^M]XH90000IKf_R?""?""?=>PDH1002>YYYYfbYYYY?13=?]e_P?""?G000HKbaWA00004DOd^eNP<0000CUTd`KKTfcSI0CUTd`YYYYY@60004DOd^eNP<0000CUTd`YYYYY[70:WYYYYYOdaKB7CUTd`L<<P_dO?1CUTd`L<0CUTd`LWNdNWI1000004DOd^eNP<0000CUTdeeYU;Qafg_R<CUTdgbK=DNdO?10=XXX><0CUTd`LBQbfSI00CI[[[SfbQ[[[G00<>NdVD5?Te_MJ0002>NdT?M`N_NPDVdO@40002@Yf^X`eTD<00007IQafYQ^eTD<0005B[[[[KafYD<04DOOe`OO]B92DLLLI0000=Sf^M:G?I70EIM_eOTe_WIE07=SVR]"	\
                "_^`QSV><07=SVR_N_NXSV><00002>VdV>NdVD[[[[E001B]eaW[DKadV>20CUTddeeecNWG0CUQTd`QQQQ_dTQUCCUTd`LBWWU10CUTd`LIMMMD2007BYe^TcO>50001UYcS@OOOKV_OW;00CI[[[SfbQ[[[G04DOOe`OO]B900HXQQMA0000:M`cSbaQG0000CUTd`LHWae]=0CUTd`L[[[[;9000:M`cSbaQG0000CUTd`L[[[[F005B[[[[KafYD<0CUTd`LFFP_dO?1CUTd`L<0CUTd`MNe_Q=8000000:M`cSbaQG0000CUTdN`_L?Tc]e_R<CUTd^bV>DNdO?19=]`_R<0CUTd`L=QbcKH000077ISfbQF770003B]f^WFL_dT?80001UTdNPQbYO^WM`e]B70000FM_eNe`WG0000002@Yf^Od`WG00000777HX`eVLF004DVbdcbb]B9CUTNXF007;=]e`L==F700=SSSfbbf]SS=94DOKSbTQSQRNXF04DOKP_]W^]RNXF0000FX^cS>NdVXXXXLJ0CB>]eaW>BDTd^W:0CUTd`]]OceT?60EUTd`XXXX_dTUE0CUTd`L<AA200CUTd`L=]^_R<007BYedbTMI10002>NVP@VdO^^feTD40007<ISfbQF57004DVbdcbb]B9004FFFF30009=Sc^W_e][C000CUTd`WWScbKH0CUTd`L<77700009=Sc^W_e][C000CUTd`XXXXWG000777HX`eVLF00CUTd`XXXX_dO?1CUTd`L<0CUTd`"	\
                "Oe`QB40000009=Sc^W_e][C000CUTdNTbKM`_]e_R<CUTdNNbKDNdO?19=]a_R<0CUTd`]]NfVLJ000000ISfbQF000000FXafS=KbcKH00000=Sf^WSfS]cKQbcKH000001BSbgc]U80000000GW_dec]U80000000FLVe`XH7008@X]e_XXX=98@OcK;005>]]e`]]]U300=]``^dd^``]=9<P_MO^W;BDUTKH0<P_MP_]YaKUTKH000C[]f`M>Ngbbbb^WA06@ONdbNVD=SfcQG0CUTd`L=RNgN>20CUTdcbbbbcdTU10CUTd`L<00000CUTd`L=]e_R<07:DYeaS>FC0000EMQQI@VdbSWTe`LE000E>RSfbQR@5008@X]e_XXX=900GLLL>E0004DOdVDTeNP<000CUTdcaaca]>E0CUTd`L<00000004DOdVDTdNP<000CUTdfbbbbK;00000FLVe`XH700CUTdfbbbbcdO?1CUTd`L<0CUTdee^KU500000004DOdVDTeNP<000CUTdNKbOScY]e_R<CUTdNKcO>NdO?10IWWW><0CUTddeef]MH3000000ISfbQF000000<>NdODYe_MJ00000GQbcKTaXKc]Sf^W:00000C=KbgcS[Z00000006[ScdNLA0000000<DYcaK[40000EB]e_PFF205>NfSI005>NcdeccTU300HLLLSccSLLLI0<R_MO`MEFIUTKH0<R_MP_Y`K@UTKH000EPNeT>>Ng^````WA06@O^df^VDISfcQ;0CUTd`"	\
                "LF>Nd_P<0CUTdb````bdTU10CUTd`L<00000CUTd`L=]__R<0;RYad^MJ0000003FGG5@VdN>ISf^MJ000:W__ef__R<004DTTe_TT]B900HK``WA000:M`f]BKcaQG000CUTdaNN_f`K=7CUTd`L<0000000:M`f]BKcaQG000CUTdb````K;0000<DYcaK[4000CUTdb````bdO?1CUTd`L<0CUTdeec]DJ0000000:M`f]BKcaQG000CUTdNRN^VaQ]e_R<CUTdNRNaQNdO?19=]^_R<0CUTd`]TbcTD2000000ISfbQF0000003B]f^W_dT?800000EM`e]N_LW^OTdN><00000FMNd`e_WG00000000AWae]B30000008[KafYD<000009B]e_P<000<R_e]=905>K]e`KKKU300=]OOOcfOOO]=95>NX]bK>MQDVQG05>NXP_]]_MDVQG000HKbeNNN^dVLLLL>E01UW]eaWWUUYeaXF0CUTd`XXSadOD40CUTd`LLLL_dTU10CUTd`L<00000CUTd`LHPPP?53[Sb_d^MJ00000000008@VdO?HKf^MJ000:W``ef`_R<004DVcgfcc]B900HKfaWA009=ScaQQQ`e][C00CUTd`L>MNdVD4CUTd`L<0000009=ScaXFM`e][C00CUTd`LLLLLF0008[KafYD<0000CUTd`LLLL_dO?1CUTd`L<0CUTd`Oeb]@E000009=ScaXFM`e][C00CUTdN>]ffN>]e_R<CUTdN>ScONdO?19=]^_R<0"	\
                "CUTd`LDYeaQ;000000ISfbQF0000000FXacSbcKH0000004DVdO^OD>N_NdT?Z00005?]f^Q`fYD20000000AWae]B9000007HW_dVLGGGGJ009B]e_P<<70<R_e]=909F=]e`L;;J200=]OOOccOOO]=9CU]YMNaN_QQ_LE0CU]YP_]W`SQ_LE004?TebbbbcdVDGGGG<00<B]eaWGBW`d_RE0CUTdcaacd^KI90CUTd`LEER_dTU10CUTd`LGGGGE0CUTd`LGGGGE00J>YYe^MGGGGA0000008@VdO?HKf^MJ000JPLSfbQLD2004DK]e_KKK=900ISf_WA004DOdecccfdNP<00CUTd`LE?Od_P<CUTd`L<0000004DOdN>G@OdNP<00CUTd`LGGGGE007HW_dVLGGGGJ0CUTd`LEEP_dO?1CUTd`L<0CUTd`MNeaSU<00004DOdN><@TdNP<00CUTdN>W^f]B]e_R<CUTdN>LNaNdO?10=WWW><0CUTd`LGX^eT?400000ISfbQF0000000<>Nd^e_MJ0000003UYebf][?TfcfS=90009HX^fY@]caQI7000000AWae]B900000:ROe`QQQQQQ?809B]e`MLPF0<R_e]=90EPM]e`MMMMU60F?""?""?KccK?""?""?F00FW_KMS]SWVS[100FW_KKK?KKVS[100:M`f]QQQNdVQQQQQB909B]eaQQS_ecS[10CUTda___OQU<00CUTd`L<<R_dTU10CUTd`QQQQQ?6CUTd`QQQQQ?609;?Ye^QQQQQD400"	\
                "0008@VdO?HKf^MJ0009<ISfbQF<0000:B]e`MLPF001UTcS[600:M`eNNNNNcaQG00CUTd`MMQ_dN>5CUTd`L<000000:M`e]QQQSfaQG00CUTd`QQQQQ?60:ROe`QQQQQQ?8CUTd`L<<P_dO?1CUTd`L<0CUTd`LWNd^K[5000:M`e][3IScaQG00CUTdN>DOOWB]e_R<CUTdN>[ScddO?19=]a_R<0CUTd`L<DTd^WF00000ISfbQF00000003B]fgdT?80000000IKcgcKH=SfgbQG0000E>Oe_MFLNdNRA000000AWae]B900000;KcgfbbbbbV@800HKcf``K;0<R_e]=90AW^^db^^^VD40011;KccK;110006BK`YQXSNY>J0006BK`YQXSNY>J007=Sc^WFF>Ngcbbbb]B909B]eebbfdbY>J00CUTd`LPP@I2000CUTd`L<<R_dTU10CUTdfbbbbO@6CUTdfbbbbO@6007BYeebbbbN>4000008@VdO?HKf^MJ00000ISfbQF000000HKcf``K;00<>NVPJ009=ScbQ>>>W^e][C0CUTdb``adb][CCUTd`L<000009=Scdbbbbbde][C0CUTdfbbbbO@60;KcgfbbbbbV@8CUTd`L<<P_dO?1CUTd`L<0CUTd`L=Q_d`QB409=ScaQG0:W^e][C0CUTdN>H?""?[B]e_R<CUTdN>:M_ddO?19=]__R<0CUT__L<HQ__]U10000IS__QF00000000FX___KH00000000FW___W:GQ___MJ000"	\
                "3BS__SB1=K__]U100000AW__]B900000;K________V@800EPY___K;0<RNN]=90AW_______VD40000;K__K;0000002[XT__VK>F000002[XT__VK>F0002>V_OD55>N______]B909B]____NTK>F000CUT__L<<490000CUT__L<<R__TU10CUT______O@6CUT______O@6007BY______N>4000008@V_O?HK__MJ00000IS__QF000000EPY___K;00ELQQI7004DO_NPE45DO_NP<0CUT_____VS>A0CUT__L<000004DO_________NP<0CUT______O@60;K________V@8CUT__L<<P__O?1CUT__L<0CUT__L<BQ___XI94DO_NP<05DO_NP<0CUT_N>511CB]__R<CUT_N>5[]__O?10HRRR@507=RRRD2<DRRR[10000;RRR>:00000000<DRRR>J00000000<DRRRD<E>RRR?50003BRRR>E0<@RRR[100000E>RRRI000000FPRRRRRRRRR[1009F?PRRPF05?>>>H00E>RRRRRRRRUZ0000FPRRPF00000004;?PRD=E00000004;?PRD=E00005@RRRI98URRRRRRRRI000IRRRRP>?I<00007=RRRD200000003BRRR@25@RRRB307=RRRRRRRR[17=RRRRRRRR[1000IRRRRRRRRU8000001[RRRBGRRR><00000;RRR>:0000009F?PRRPF003FFF40004?RRR[107=RRRD"	\
                "<07=RRRRRPDBJ007=RRRD2000004?RRRRRRRRRRRD<07=RRRRRRRR[10FPRRRRRRRRR[17=RRRD25@RRRB37=RRRD207=RRRD25URRRRI04?RRR[107=RRRD<07=RRRU8000IRRR@57=RRRU8FPRRRB301<<<50008<<<2002<<<4000003<<<<90000000002<<<<00000000002<<<200<<<<5000008<<<20005<<<40000000<<<<10000007<<<<<<<<<<4000008<<<<700455530002<<<<<<<<4000007<<<<70000000006<<4700000000006<<470000005<<<Z004<<<<<<<<10001<<<<<5Z00000008<<<20000000008<<<5005<<<80008<<<<<<<<4008<<<<<<<<400001<<<<<<<<400000004<<<83<<<20000003<<<<9000000008<<<<7000000000005<<<40006<<<20008<<<<<<4700008<<<200000005<<<<<<<<<<<20008<<<<<<<<4007<<<<<<<<<<4008<<<2005<<<8008<<<20008<<<2004<<<<1005<<<40006<<<20008<<<400001<<<5008<<<407<<<<800000000000CCCCCCC7000CCCCCCCCCC0000CCCCC30000000CCCCCCCC0007CCCCCCCCCC90003CCC9000CCCC000000003CCC00000000CCC30003CCC00003CCC3CCC3CCC000"	\
                "000001448900000001170000033000000CCCCCCCC0000337000000000092510000000000144300008580000CCC7000000000CCCC9000CCCC7000CCC70000009CCCCCCCC000000000CCC700CCC7000000007CCCCCCCCC00000000003CCC300000000CCCCCCCC30000CCCCC30000000CCCCCCCC9000CCCCCCCC0000CCCC000CCC3007CCC70000CCC700CCCC0000CCC7009CCCC0000009CCCCCCCC0000CCCC9000CCCC7000CCC7007CCC0000CCCCCCCCCC0000CCCCC30000007CCCCCCCCCC9000000FUUUUUUUIC0;UUUUUUUUUU;90;UUUUUU=:1000;UUUUUUUUG01=UUUUUUUUUUH902[UUUI30AUUUUF000000<UUUU:000000FUUUU<02[UUU;00<[UUU[UUUUUUU:00000<IUD>?=J90009;?UBF40<HU[F9000;UUUUUUUUG005[U[HE00000000F>>UH20000000<U>DBE08H@>@G00;UUUB40000000;UUUUH70:UUUU=80;UUUB400003IUUUUUUUUG0000000;UUUB4;UUUB40000001=UUUUUUUUUF000000004BUUU[<000000;UUUUUUUUU<00;UUUUU[I:C000;UUUUUUUUH70;UUUUUUUUG00EUUUUF0FUUU[21IUUU=Z00;UUUICEUUUU;90;UUUB43H"	\
                "UUUUE00003IUUUUUUUUG00;UUUUH70:UUUU=80;UUUB46=UUU;90;UUUUUUUUUU;90;UUUUUU=:1001=UUUUUUUUUUH900007BYTTTTTT>5CUTTTTTTTTTT?1CUTTTTTYSM=60CUTTTTTTTY[72>TTTTTTTTTT?10AMTTTLA6[STTSB900008FXTTSI500007BSTTK=CIQTT]U30:WTT]XTTSQTTSI0000:DQTNNOSP;3001UTT]W[F>KYYP:00CUTTTTTTTY[70JMTYK>J0000000;KNTQ?A000000FXNVS?2IWONYB7CUTTTL<300000CUTTTTLF2?]TTTR<CUTTTL<00004DTTTTTTTY[300000CUTTTL<UTTTL<0000002>TTTTTTTT]=00000000:MTTTQG00000CUTTTTTTTTQF0CUTTTTTYSMIC0CUTTTTTTTT@6CUTTTTTTTY[70GQTTYDE=]TTW:;WTTT><0CUTTT>5=KTTT?1CUTTTL<GMTTTQG00004DTTTTTTTY[3CUTTTT>23B]TTTR<CUTTTL<<PTTT?1CUTTTTTTTTTT?1CUTTTTTYSM=602>TTTTTTTTTT?100007BYeeeeeN>5CUTddeeeeddO?1CUTddeedfNW;0CUTddeeeeY[72>_eeeddeeeO?101[Scc]UFW_dVPJ0000AUMKbfSW@F3000JPVdNM:PVdNL:00:Wae]XbfSQbfSI000EDYadccecOLF001UTef`Q>TbecS=90CUTddeeeeY[70JM^ebS=0038000;K^e^]"	\
                ">A00000FXbd^QIPVeeYB7CUTd`LU[I:C00CUTddcY@=K^dd_R<CUTd`L<00004DNddeeddY[300000CUTd`L<UTd`L<0000002>_edddeee]=00000009=Scde][C0000CUTddeeeebQF0CUTdeccecNM:0CUTddeeeeO@6CUTddeeeeY[704[KabS?=]eaWFLOeNM;90CUTdN>E>VddO?1CUTd`LFPTf^K[400004DNddeeddY[3CUTddaXG<PNdd_R<CUTd`L<<P_dO?1CUTddeeeeddO?1CUTddeedfNW;02>_eeeddeeeO?100007BYYYYYYY>5CUTd`YYYY_dO?1CUTd`]]TceT?8CUTf_YYYYY[72>YYYYfbYYYY?100JPVd_W@YfaQI7000A>]`cddc^TLG0007IQ^f]UScaK=300:Wae]XbfSQbfSI003BSbeVKKTcfY?401U]Y`dNKbbVfY[70CUTd`YYYYY[70JMK_e]B1AB@J00FPXTbb]?50000FXT_dVPScbTYB7CUTd`TTYSMIZ0CUTdedaKMNdfg_R<CUTd`L<00004DNdNYY^dY[300000CUTd`L<UTd`L<0000002>YYTd`YYY]=00000004DOd^eNP<0000CUTd`YYYYYQF0CUTd`KKTfcSI0CUTd`YYYYY@6CUTd`YYYYY[700<@]b^KB]eaW>Yf`Q=100CUTdN>IKbgdO?1CUTd`L>YcaSU2000004DNdNYY^dY[3CUTdeeYU;Qafg_R<CUTd`L<<P_dO?1CUTd`YYYY_dO?"	\
                "1CUTd`]]TceT?82>YYYYfbYYYY?100000F[[[[[[[H3CUTd`L[[P_dO?1CUTd`L=PNgN>20;MOfOM[[[F0CI[[[SfbQ[[[G0007IQafYQ^eTD<0006U]be`ce_ffT><0002@Yf^X`eTD<000:Wae]XbfSQbfSI00<PNd`WI;PVd^WF00GBUX_aOc]QNQ;00CUTd`L[[[[F00CF>NbQ;UMSXF009<GD]c^QH0000EUUKbcS`eTD?G0CUTddeeecNWG0CUTdN_d_Tfb]e_R<CUTd`L<00004DNdN>M^dY[:E6000CUTd`LFUTd`LFA50000CI[UTd`L[[[:5000000:M`cSbaQG0000CUTd`L[[[[B<0CUTd`LHWae]=0CUTd`L[[[[;9CUTd`L[[[[F0000A>Tf_X]eaWSbaK[4000CUTdN>>Oc^dO?1CUTd`L]bb]@E0000004DNdN>M^dY[3CUTdN`_L?Tc]e_R<CUTd`LFFP_dO?1CUTd`L[[P_dO?1CUTd`L=PNgN>2CI[[[SfbQ[[[G0000004@XXXXXMA0CUTd`L<<P_dO?1CUTd`LF>Nd_P<03GRTcVW=4000077ISfbQF77000002@Yf^Od`WG0000JM_d_XbfSOeaQG0000FM_eNe`WG0000:W^e]XbfSQccKH00FWadTU87=SfcKI00074[]ec_MBD[500CUTd`XXXXWG000JM``MW]_NP<0000<>T^eOD500001<>Teae`W;Z90CUTd`]]OceT?6CUTdNKaeffT]e_R<CUTd`L<00004DN"	\
                "dNDM^dYXWLU:00CUTd`XXXTd`XXMDH800003UTd`XXXXW@:00009=Sc^W_e][C000CUTd`XXWLUF70CUTd`WWScbKH0CUTd`L<77700CUTd`XXXXWG00000FLVeN]eaK^b]?<0000CUTdN>QaNNdO?1CUTd`SacY>J00000004DNdNDM^dY[3CUTdNTbKM`_]e_R<CUTd`XXXX_dO?1CUTd`L<<P_dO?1CUTd`LF>Nd_P<0077ISfbQF7700000004DNbbbaWA0CUTd`L<<P_dO?1CUTd`XXSadOD4007F>YcNX;000000ISfbQF00000000GW_dec]U80000FQbe]XbfSKcfS=00001BSbgc]U80000<L_dOXbfS]faQG00GQbe]=90GQbf]=00000;KcdO?841000CUTdfbbbbK;000GQbVT`feYU10001BKbK^^WF000000;W`gcSU8000CUTd`L=RNgN>2CUTdN>YfdNM]e_R<CUTd`L<00005>NdVDM^dbba_]>A0CUTdfbbbbgfba^OWI70003UTdfbbbb^]D20004DOdVDTeNP<000CUTdfbba_YRF0CUTdcaaca]>E0CUTd`L<00000CUTdfbbbbK;00000E>OefNeb_dbS[10000CUTdN>OcSNdO?1CUTd^^dbS[600000005>NdVDM^dY[3CUTdNKbOScY]e_R<CUTdfbbbbcdO?1CUTd`L<<P_dO?1CUTd`XXSadOD40000ISfbQF0000000004DN````WA0CUTd`L<<P_dO?1CUTdfbb"	\
                "cd^KI9003HMNcT>:000000ISfbQF000000006[ScdNLA00000FQbe]XbfSQbfS=0000C=KbgcS[Z00008?TeaKbfS_dNRE00FQbe]B70;KcfS=00000FWaeY[300000CUTdb````K;009=SffecfbKH0000GM_T]cfS=90000C=KbdeTD<000CUTd`LF>Nd_P<CUTdN>LVVQB]e_R<CUTd`L<0000<P_dO?M^d``adcSB3CUTdb```^db`^feV><0003UTdb```bgaXF000:M`f]BKcaQG000CUTdb``aec]U1CUTdaNN_f`K=7CUTd`L<00000CUTdb````K;00001BSbfadgge^d_MF0000CUTdNQaNLNdO?1CUTdge`d_WG0000000<P_dO?M^dY[3CUTdNRN^VaQ]e_R<CUTdb````bdO?1CUTd`L<<P_dO?1CUTdfbbcd^KI90000ISfbQF0000000008ULLLLL>E0CUTd`L<<P_dO?1CUTda__NTQU2009GLOfTRF0000000ISfbQF000000000AWae]B300000:W^dOXbfS]fcKH0000FMNd`e_WG00000;X`dbedbdbSBC00JM`dOD51[]fbQ;00000AWae]B900000CUTd`LLLLLF006?TeaVS^_MJ000<DT_X`YfTU10000FMNdNcaK=300CUTd`XXSadOD4CUTdN>IDDUB]e_R<CUTd`L<0000EM`dYUM^dYLQ`dVD4CUTd`LLLTd`LMTdaWF0003UTd`LLLSffSI009=ScaQQQ`e][C0"	\
                "0CUTd`LLX_dN>2CUTd`L>MNdVD4CUTd`L<00000CUTd`LLLLLF0000FM_dNSagd_KafY@2000CUTdNTcSDNdO?1CUTddNQaeTD<000000EM`dYUM^dY[3CUTdN>]ffN>]e_R<CUTd`LLLL_dO?1CUTd`L<<P_dO?1CUTda__NTQU200000ISfbQF000000000<GGGGGGGA0CUTd`L<<P_dO?1CUTd`LP>?I5000FPTfVMGGGE00000ISfbQF000000000AWae]B9000005DTecObfYadNLJ0005?]f^Q`fYD200004UK`edgea]DE000<?Yf^Q=;M_eV>J00000AWae]B900000CUTd`LEEEE300<P_TQ>KcO@<003BK_SONXaVD4001F?]f^QVdNLG<0CUTdcaacd^KI9CUTdN>544ZB]e_R<CUTd`L<004G[KbfS=M^dY[DNdN>4CUTd`LEUTd`LISfbQF0003UTd`LEFQbfS=004DOdecccfdNP<00CUTd`LE?Od_P<CUTd`LE?Od_P<CUTd`L<00000CUTd`LGGGGE0002@YfaKDYeaQDTeaQI700CUTd_^_MDNdO?1CUTd`X@YfaK=C0004GBKbfS=M^dY[3CUTdN>W^f]B]e_R<CUTd`LEEP_dO?1CUTd`L<<P_dO?1CUTd`LP>?I5000000ISfbQF000000007BQQQQQQQD4CUTd`L<<P_dO?1CUTd`L<280000CUYcNQQQQQ?60000ISfbQF000000000AWae]B9000000;MOcddgdfNQ=C0"	\
                "09HX^fY@]caQI70000<UX]bf]KD:0007[QQTfNWPOfOQQD40000AWae]B900000CUTd`L<000000AMW?;UTdTMD507=RR>RRM^ND400:MK^eYQQaf]Q=9CUTda___OQU<0CUTdN>5009B]e_R<CUTd`L<00GX]`daXFM^dYMQ`dO@8CUTd`L<UTd`MWTd^W:0003UTd`LEFXbfS=00:M`eNNNNNcaQG00CUTd`MMX_dN>5CUTd`MMQ_dN>5CUTd`L<00000CUTd`QQQQQ?607IQ^eTD=]eaWGW`dVRA00CUTddf][DNdO?1CUTd`LFM_dNMF000GXS_daXFM^dY[3CUTdN>DOOWB]e_R<CUTd`L<<P_dO?1CUTd`L<<P_dO?1CUTd`L<2800000000ISfbQF000000007BYbbbbbN>5CUTd`L<<P_dO?1CUTd`L<000000CUTdcbbbbO@60000ISfbQF000000000AWae]B9000000CHPKTbeTSM=5000E>Oe_MFLNdNRA000004GXbfSI<00007[YbbdaWP_gcbND40000AWae]B900000CUTd`L<0000001GF41?Tec_P<008<<2<<L_ND400:Waddbbbegf]=9CUTd`LPP@I200CUTdN>5009B]e_R<CUTd`L<00;KceaYDEM^d^`adaK=9CUTd`L<UTdb`^ffTD20003UTd`LEFXbfS=09=ScbQ>>>W^e][C0CUTdb``aeaSB3CUTdb``adb][CCUTd`L<00000CUTdfbbbbO@60JPVd`W;=]eaW"	\
                ":USccSU80CUTdg_MFDNdO?1CUTd`L<BKbfY?500;KceaYDEM^dY[3CUTdN>H?""?[B]e_R<CUTd`L<<P_dO?1CUTd`L<<P_dO?1CUTd`L<0000000000ISfbQF000000007BY_____N>5CUT__L<<P__O?1CUT__L<000000CUT______O@60000IS__QF000000000AW__]B900000007JIX__S=:10003BS__SB1=K__]U100000FX__SI000007[Y____WP____ND40000AW__]B900000CUT__L<00000000000HWTN_P<00000002PNO@800:W_________]=9CUT__L<<49000CUT_N>5009B]__R<CUT__L<00;KNYQD:EM_____OK?<0CUT__L<UT____N]LG00003UT__LEFX__S=04DO_NPE45DO_NP<0CUT_____OKDE0CUT_____VS>A0CUT__L<00000CUT______O@61[S__]U8=]__W:JPV__WG0CUT__]U4DN_O?1CUT__L<<>T__QI00;KNYQD:EM__Y[3CUT_N>511CB]__R<CUT__L<<P__O?1CUT__L<<P__O?1CUT__L<0000000000IS__QF000000000IRRRRRRRU87=RRRD25@RRRB37=RRRD20000007=RRRRRRRR[10000;RRR>:000000000E>RRRI0000000000A>RRR;000003BRRR>E0<@RRR[100000A>RRR;000009IRRRRR>@RRRRRUZ0000E>RRRI0000007"	\
                "=RRRD2000000000006H?>P?400000004U>>=300E>RRRRRRRRRRH07=RRRD20000007=RRRU8000IRRR@57=RRRD200FP>UH<0<DRRRRPDI<007=RRRD2=RRRRR>[:700007=RRRD<A>RRR;04?RRR[107=RRRD<07=RRRRRPD=<007=RRRRRPDBJ007=RRRD2000007=RRRRRRRR[11[RRR>A0HRRR>E7IRRRRG07=RRRPFZURRRB37=RRRD20;RRRRI00FP>UH<0<DRRR=97=RRRU8000IRRR@57=RRRD25@RRRB37=RRRD25@RRRB37=RRRD20000000000;RRR>:0000000001<<<<<<<4008<<<2005<<<8008<<<2000000008<<<<<<<<4000003<<<<90000000000<<<<100000000009<<<<C0000008<<<20005<<<40000000<<<<C000000Z<<<<<<5<<<<<4000000<<<<100000008<<<200000000000000Z5<4000000000644C0000<<<<<<<<<<<1008<<<2000000008<<<400001<<<5008<<<20007<5100002<<<<<4900008<<<208<<<<<5C00000006<<<200<<<<C005<<<40006<<<20008<<<<<<4700008<<<<<<4700008<<<200000008<<<<<<<<4004<<<<001<<<<001<<<<3008<<<<704<<<8008<<<2003<<<<1007<5100002<<<Z008<<<400001"	\
                "<<<5008<<<2005<<<8008<<<2005<<<8008<<<2000000000003<<<<90000000000CCC30003CCC0000000003CCC000000000CCC30003CCC0009CCC700CCCC0000CCC700CCC3003CCC0000CCCCCCC7000000000CCC7000000CCC70000CCC700000000000003CCCCC900000ZZZZZZZZ00007ZZZ77ZZZ700007ZZZ79ZZZ30ZZZC00007ZZZ77ZZZ7000003CCC000000000000111C0000000000000CCCC9000000000CCC7000003CCC70000000007CCCCCCCCCC30000CCC73CCCCCCCCCC30000000007CCC00000000007CCC90000985430000CCC70000000000000000000000CCCCC300000000000000000000CCCCCCCC30000ZZZ101ZZZ03ZZZ30007ZZZ70ZZZZ0000CCC7009CCCC00000CCC7009CCCC0000CCCCCCC7007CCC30007ZZZ77ZZZ7000000FUUUU<02[UUU;0000000<[UUU:0000000FUUUU<02[UUU;03IUUUICAUUUUE00;UUUB4:UUUU<<UUUU:00EUUUUUUUIC0000000;UUUB40000;UUUB400;UUUB40000000003AI[UUUUUH7000J?""?""?""?""?""?""?""?F003I?""?""?II?""?""?I7003I?""?""?IH?""?""?=F?""?""?[4003I?""?""?II?""?""?I7000<UUUU:0000000000E"	\
                "UUUU<00000000000:UUUUH70000000;UUUB4000<[UUUB400000008=UUUUUUUUUUU<00;UUUB[UUUUUUUUUUB400E:::28=UUU;01:;HFZ01=UUUH7004I@>D=100;UUUB400000000EFFFE0000000;UUUUUU=:C000000008FF:3000<UUUUUUUU[200F?""?""?UEU?""?""?:B?""?""?=C03I?""?""?IF?""?""?""?E00;UUUB43HUUUUE008;UUUB23HUUUUE00GUUUUUUUICZIUUU[203I?""?""?II?""?""?I700007B]TTKI0;QTTYU300004A;XTTS=F500007BSTTK=CIQTT]U34DTTT>4HKTTQG0CUTTTL<ISTTXFFQTTSI00GQTTTTTT>4000000CUTTTL<000CUTTTL<0CUTTTL<000000007HLSYTTTTT?Z000GQOOOOOO]=008@OOO?@OOO?1008@OOO?UTOOD=]OOR<008@OOO?@OOO?105HIQTTSII<00000000GQTTXF00000000001U]TTT><000000CUTTTL<000HQTTTWF0000000<PTTTTTTTTTTWF0CUTTTLMTTTTTTTTTTME04DWWW[<LTTTU1=MKKWF02>TTT>203=QONV>50<UTTTLJ90000001UXXX?Z00000CUTTTTTYSMI60000000;WXMA000FQTTTTTTTW:00=KOOS?MOOK=XOO]@408@OOO?>TOOWF0CUTTTL;GLTTTQG07=LTTTLDGMTTTQG03UTTTTTTT>4HWTTTM:08@OOO?@OOO?10000"	\
                "0:M`dOD5?Te^WG0007F?MKSaf]KWD;6000JPVdNM:PVdNL:04DNdN>4HKfcQG0CUTd`L<ISfbXFFQbfSI00GQceeddN>4000000CUTd`L<000CUTd`L<0CUTd`L<00000000ALVceeeedO?Z000GQcebbfe]=008@VdO?@VdO?1008@VdO?UTdND=]e_R<008@VdO?@VdO?10GQSSbfSSSI000000<"":GQcaXF::80000000:M_debQI000000CUTd`L<005DOdef]U60000003=KbdeeeedeY?40CUTd`LU]fdeeeedcS[10ZUYaN><L`dTU:MNcbQF03B]f^WF0EPNddN>53[XTd`X><0000001?ObO@600000CUTddeedcNW;0007ZZZHKaW:000FQbdeeeeaW:002UK^aSM^fKWNcY>:008@VdO>Yc_X=60CUTd`MKPTf^K[403UT`db`LPTf^K[403UTeeeddN>;MVe_XI108@VdO?@VdO?1000008UYe^XGW`dOD5000FRY`cfdgec^OWI3007IQ^f]UScaK=304DNdN>5HKfcQG0CUTd`L<ISfbXFFQbfSI00GQYYYNdN>4000000CUTd`L<000CUTd`L<0CUTd`L<00000000IKcfO]]_dO?Z000GQcbXX`e]=008@VdO?@VdO?1008@VdO?UTdND=]e_R<008@VdO?@VdO?10GQcfddffSI000001UWWQcaXWWW;000000CBSc^VdOD200000CUTd`L<00;X^c]f_M:0000000EPVcYYYYa`"	\
                "WG00CUTd`LFM_aYYYYbNLA000;QbaXFL`dTUBSf`]QF00FXafSB7HKbfVT>53UTbgc`L<000004[?OdO@[40000CUTd`]]TceT?803I?""?""?""?KaW:000FQbfYYYYYW:000<USa^K^fKNfTPF0008@VdO]b_Q=400CUTd`M_YcaSU2003USTd`SLYcaSU2003UYYYYNdN>ROf^QB4008@VdO?@VdO?1000000GX^eY?]fbKI0004@Yff_ObeTNceNLJ0002@Yf^X`eTD<004DNdN>5HKfcQG0CUTd`L<ISfbXFFQbfSI00<B[[DNdN>F:EZ000CUTd`LFFE8CUTd`L<0CUTd`LFFE800009B]eaW=P_dO?Z000;KcaXL`e]=008@VdO?@VdO?1008@VdO?UTdND=]e_R<008@VdO?@VdO?10GQNNceNNSI270001?OaaefaaaKH000000JLNeTKcaQ;00000CUTd`LFFF?TeNW`f][C0000007=KbV>UYfYU400CUTd`LF[]cT@DTcSBC000EPNf]BL`dTU>VeYUB<00<>NdVD5?Te_M?B13UQTd`QPFE8000<RONdNOL<0000CUTd`L[WNgN>208@OOOOOaW:005FQbfS[[[[B50000E@Sb`bfNeOLG90008@VdNb`QB4000CUTd`M^bb]@E0000:UTd`L]bb]@E0000F[[[DNdN>YcaKU50008@VdOSSVdO?10000005DOd`MNdNPE000AM`dNMXafSPYfcKH0000FM_eNe`WG0004DVd`L?""?KfcQG0CU"	\
                "Td`L<ISfbXFFQbfSI000774DNdNXXWLUA00CUTd`XXWLUFUTd`L<0CUTd`XXWLUF7009=]faW=P_dO?Z000IKf^WL`e]=008@VdO?@VdO?1008@VdO?UTdND=]e_R<008@VdO?@VdO?10ADDQbfSXXW>I4001?OVVcbVVVKH000007=ScaQRNeT@40000CUTd`XXXXX^cS?YeNLJ0000000E>VbKW``WF000CUTd`XXXX_^XQaNRJ0000C[]eV>L`dTUW^aQG70003B]f^WFL_eT?6300EUTd`XXWLUF70<R_bgb`L<0000CUTd`LXNNd_P<08@Vd`___W:09=WQbfSWWW=700000EDTffggee_WG00008@VddcbS[4000CUTd`SacY>J00000CUTd`SacY>J000000774DNdN]bb]?<00008@VdcbbcdO?10000000IKbf]bcSB3000FQbe]BXafS=Qbe]=90001BSbgc]U80001UTefVOOOfcQG0CUTd`L<ISfbXFFQbfSI000004DNdcbba_]>E0CUTdfbba_YRUTd`L<0CUTdfbba_YRF000FW_fO]]_dO?Z00<[]e_LL`e]=408@VdO?@VdO?2008@VdO?UTdND=]e_R<708@VdO?@VdO?1004FQbdbba^VQB107IDDQcaXDD@:00000EPNdOQQSc^WG0000CUTdfbbbbbe_QQQ^cS=30000007IKbNYf]U8000CUTdfbbbbceTOcS=300000GXa^WL`dTUSfN><00000FXafS=KbbKH0000CUT"	\
                "dfbba_YRF0<>XOdOXPF5000CUTd`XXYedOD408@VdOPPP>E07[Yaedaaa]B900006[Kb^YceNVeT>E1008@Vd^Sb`WH100CUTd^^dbS[600000CUTd^^dbS[6000000004DNd`agaK=300008@VdOQQVdO?11000000EPNdcd`M:0000FQbe]BXafS=Qbe]=9000C=KbgcS[Z0000HQ`feeeegcQG0CUTd`L<ISfbXFFQbfSI000004DNda``adbS=9CUTdb``aec]UTd`L<0CUTdb``aec]U1006BQNeeeedO?Z0J>X_dOLL`e]LUZ8@VdOLLVdOLD<08@VdOLLTdNLL]e_LP:08@VdOLLVdO?1000FQbe```cd_MJ0038GQcaSSKWU<0009IKbgbbbbdeYU8000CUTdb````fgcbbbfdNPE000009GPS_caaYMIZ00CUTdb````dgab^]L;30000<>NfSW`dTWVf][300000<>NdODYe_LJ0000CUTdb``aec]U19A?OdOSKXD:00CUTdfbbcdbKU508@VdO?<<2007[Y^fd^^^]B90000;W_c]M^fKQ^bS?U208@VdO>OeOP?G0CUTdgddd_WG00000CUTdge`d_WG000000004DNdgf^dNL:00008@VdO?@VdO?""?UE000003=ScgeYU80000:W^dVLXafS>]fcKH0000FMNd`e_WG00005[X]]]]]fcQG0CUTd`L<ISfbXFFQbfSI000004DNdNLMQ^dO?6CUTd`LLX_dN>Td`L<0CUTd`LLX_dN"	\
                ">20005DTe^]_dO?Z0:W^ed```ad`VD48@Vd```^d^`LE08@Vd````d^```da`QG08@Vd```^dO?1000FQbfSLMYfbQ;0000GQcebbc`SBC00<>Od_NbfNNf`WF000CUTd`LLL]f^N`d_NabKI90000APOcffecfNW;00CUTd`LLMVefefffOLF00003B]f^^cda^b^XF0000003B]f^W_dT?60000CUTd`LLX_dN>201?Odcbf^Y@40CUTda__NTSNME08@VdO?100009=WQbfSWWW=9000E>TeOPM^fKU]c_OMJ08@VdO?W_cO]B9CUTddN^aeTD<0000CUTddNQaeTD<00000004DNdfOSbc]U40008@VdO?@VdNOOQF00000AGM_g^XG000005DTffNTbfYVbdNMA0005?]f^Q`fYD200004G====KfcQG0CUTd`LGISfbXGGQbfSI000004DNdN>E>NdVD8CUTd`LE?Od_PTd`L<0CUTd`LE?Od_P<000HQ^eYP_dO?Z0:Wa`______aVD48@V______``LE08@V____________bQG08@V__a^__O?1000FQbfSI;QccKH0000GQcaQQTeN>500HQabKW^fKM`f]U100CUTd`LAM_eY>NdT>OdV><000C[]cbY_^S^eTD50CUTd`L?Yfa]^_]afYU80000FW^eedgeeeV>20000000FXacSbbKH00000CUTd`LE?Od_P<01?OdOQ]c^MJ0CUTd`LP>?""?XIC08@VdO?1000004FQbfSIAA40001["	\
                "S__XHM__KHPOe^MJ08@V_O?BKae]B9CUTd`X_YfaK=C000CUTd`X@YfaK=C0000004DNdNM>Oe^XH9008@V_O?@V__fbQF00004DQK`dOD2000000GLT^cedgefaVX=C009HX^fY@]caQI700000999HKfcQG0CUTd`QQQSfbQQQQbfSI000004DNdNMMQ^dTU1CUTd`MMX_dN>Td`L<0CUTd`MMX_dN>500<DOe`WP_dO?Z0:WaYRRRRRR_VD41[RRRRRRRO`LE01[RRRRRRRRRRRR]bQG01[RRR_NRRRBC000FQbfSMMYfbQG0000GQcaX?Sf_P<05DTdNPW^fKUYe_M:00CUTd`LBSc^XDNdTUKbaQH000<PNdOR_^W]f^WF0CUTd`LM`eYW^_PTe_ME00005D]]]`dT]]S=70000000<>Nd^e_LJ00000CUTd`MMX_dN>501?OdO@XaaW:0CUTd`L<28<F4008@VdO?1000000FQbfSI0000001[RRRU<DRRRGHKf^MJ01[RRRB<P_e]B9CUTd`M_M_dNMF000CUTd`LFM_dNMF0000004DNdN>HX`eO>J001[RRRB[RRQbbQF00005>NcdaKI90000003F@WS]afYSX>I4000E>Oe_MFLNdNRA00000000HKfcQG0CUTdfbbbbdebbbbefSI000004DNda``ad^QI0CUTdb``aeaSUTd`L<0CUTdb``aeaSB309IQafYUP_dO?Z0:WaY[<<<<P_VD404<<<<<<?O`LE004<<<<<<<<<<<=]aQG0"	\
                "04<<>_N><<40000FQbe``^cfVPE0000GQcbOO`eOD40;X^cSBW^fKHW^cSBC0CUTd`LPNeT@DNdTUPNdOD200:WacSR_^WWacKI0CUTd`LQbbQW^_PKbbQG00009GBBL`dTUBBJ000000003B]fgdT?800000CUTdb``aeaSB301?OdNONeNRE0CUTd`L<00000008@VdO?1000000FQbfSI00000004<<<402<<<3HKa^MJ004<<<8<P_a]B9CUTd`MK>KbfY?500CUTd`L<BKbfY?5000004DNdN>4U]ccS[8004<<<84<FQaaQF00005>N_VS@<0000000005FIX__S=G<90003BS__SB1=K__]U10000000HK__QG0CUT______________SI000004DN_____OQU20CUT_____OKDUT__L<0CUT_____OKDE00<>O__MFP__O?Z0:MXXB700<>XX?800000001UXXP<00000000000000IXXWF00005>XXD4000000FQ_____NYMH70000GQ_____TM;04?T__LAW__KH@T_NRE0CUT__LK__QHDN_TU=S__Q;00FQ__QR__WL__S=0CUT__LK__WW__PW__K;0000009<L__TUC90000000000FX___KH000000CUT_____OKDE001?O____OQ=C0CUT__L<00000008@V_O?1000000FQ__SI000000000000000000;WXXLE00000002>XXX=9CUT__LHF>T__QI00CUT__L<<>T__QI0000"	\
                "04DN_N>4:RN__WG000000000FMXXWF00008URPD=E000000000000J>RRR;0000003BRRR>E0<@RRR[10000000GPRRPF07=RRRRRRRRRRRRRRRR;00000ZURRRRRP@I2007=RRRRRPD=<=RRRD207=RRRRRPD=<000<DRRRU4?RRR[C03:FF20000JFFE000000000<FFA9000000000000005FF:100000EFFE0000000:"">RRRRR>UFC00000FPRRRRP?G108URRR[E>RRPG;RRR>E07=RRRDRRRD<URRR=J>RRR;00:PRR>@RR>DRRRH07=RRRDRRRD>RR@>RRPF00000002@RRR=700000000000<DRRR>J0000007=RRRRRPD=<000CBRRRRP@H4007=RRRD200000001[RRRB3000000:PRRRG0000000000000000008FFF:700000000JFFF507=RRRD20;RRRRI007=RRRD20;RRRRI00000ZURRRU83IRRRPG0000000001:FF:1000004<<4700000000000000<<<<C00000008<<<20005<<<4000000003<<<<7008<<<<<<<<<<<<<<<<C0000004<<<<<<4900008<<<<<<4708<<<20008<<<<<<47000002<<<405<<<400000000000000000000000000000000000000000000000000000000000000009<<<<<<5100000007<<<<<<800004<<<402<<<3C<<<<0008<"	\
                "<<2<<<204<<<80<<<<3009<<<<5<<22<<<C008<<<2<<<22<<5<<<<7000000005<<<800000000000002<<<<000000008<<<<<<47000004<<<<<4000008<<<20000000004<<<800000009<<<<30000000000000000000000000000000000000008<<<2003<<<<10008<<<2003<<<<10000004<<<4001<<<<30000000000000000000CCC7007CCCCCCCC000CZZZZZZZZ100003CCC9000CCCC00003CCC9000CCCC0009ZZZC07ZZZC000CZZZZZZZZ1ZZZ00000001ZZZ01ZZ1000009CCC700CCCC0009CCC70000000000000074<<6000000CCC700001ZZZ01ZZ100000000CCCCCCCCCCC0007ZZZZZZ10000CCC30003CCC0000000003CCC0000009CCCCCCCC0009CCC30000CCCCC300007CCC7000000003CCCCCCCCCC0000CCC700CCC3003CCC0000CCC7009CCCC0007ZZZZZZZZ7000007CCC7CCC00000009C3000000000009CCCCCCCC000000001458700007CCC30CCC70000CCC700CCC3003CCC00009CCC003CCC00009CCCCCCCCCC90009CCCCC700000000000CCCC00000000CCCCCCCCC0000000000;UUUB46=UUUUUUUU:08B?""?""?""?""?""?""?""?""?E0"	\
                "02[UUUI30AUUUUF002[UUUI30AUUUUF00;?""?""?[4I?""?""?B608B?""?""?""?""?""?""?""?""?""?""?""?F000005U?""?""?F?""?""?""?E0003IUUUICAUUUUE03IUUUIC000000000000EBDPP?;6000;UUUB4005U?""?""?F?""?""?""?E000000GUUUUUUUUUUUA03I?""?""?""?""?""?U<00FUUUU<02[UUU;00000004BUUUG00003IUUUUUUUUG07HUUUB400;UUUUUU=:C1=UUU=100003AI[UUUUUUUUUUJ00FUUUB4JUUUU<2[UUUA00;UUUB::HUUUUE03I?""?""?""?""?""?""?""?I70008=UUU=UUUF00008FBUUIE900000007HUUUUUUUUF00000ZGUD>@BF00CIUUU[;UUUIC00FUUUB4JUUUU<2[UUUA007IUUUF2[UUUF003IUUUUUUUUUUI309HUUUUU[;500000000EUUUUE000000;UUUUUUUUUE00000000CUTTTL<<PTTTTTTTSI02POOOOOOOOQF00AMTTTLA6[STTSB90AMTTTLA6[STTSB93[SOOXHROOT>502POOOOOOOOQOO]=00000EMOOSIQOOQF0004DTTT>4HKTTQG04DTTT>400000000AFFADSV_NTW=10CUTTTL<00EMOOSIQOOQF000008?TTTTTTTTTTKH08@OOOOOOW:07BSTTK=CIQTT]U3000000EMTTTU30004DTTTTTTTT><JPTTTME0CUTTTTTYSMIGWTTT>50007HLSYTTTTTTTTTK;09B]TTMJIKTTQGFWTTKI0CUTTT"	\
                "MWWMTTTQG08@OOOOOOOO?1000<RTT]RTT]=9002[W]TYKP:00000006@TTTTTTT]=90003IWYNNO]?104>TTTXUTTT>509B]TTMJIKTTQGFWTTKI008DTT]==QTTS=904DTTTTTTTTTTD401?TTTTT]Q?E0000000IKTTKI000004@TTTTTTTTQG00000000CUTd`L<<P_ddeeefSI02P___be___QF001[Scc]UFW_dVPJ001[Scc]UFW_dVPJ00J>OeO>]c_W;002P___be___Q_e]=00000EM^fSIQbbQF0004DNdN>@?KfcQG04DNdN>400000005>QQU]bbN_fNWF0CUTd`L<00EM^fSIQbbQF00000:M`dcfddeeefKH08@Vd`___W:00JPVdNM:PVdNL:0000000EM`dTU30004DNddeeedbK=[ScbK=C0CUTddeedcNW>Te^XH9000ALVceeeeddeeecK;00;QbbQG[YefS=IKcaWF0CUTd`LY^Tf^K[408@VdbbbbdO?1000<R_e]R_e]=90C=K_b__bXFZZZZ7006@Oddeeee]=9000JLNefabO?104>Ndf]UTdN>501BKbbQ>>YefS>>KcaWI908DVe]=W_cYDE004DNeeeddeeeND401?Odeeee^S[5000006?TddT?80000FWNN`eeNNV><00000000CUTd`L<<P_dOYYYYSI04?PPM^fSPP>:000JPVd_W@YfaQI7000JPVd_W@YfaQI7009HX`bS`bS[8004?PPM^fSPPR_e]=000"	\
                "00EM^fSIQbbQF0004DNdN>OQKfcQG04DNdN>400000005>NOW`c]>MNfYU1CUTd`L<00EM^fSIQbbQF00007=ScaQNdNYYYYKH08@VdOPPP>E007IQ^f]UScaK=30000000EM`dTU30004DNdNYYYVdNMW_eT><00CUTd`]]TceTSbf]?50000IKcfO]]_dOYYYYK;00AM^fS=DVbcT?[Ye_R<0CUTd`L>TeaSU2008@VdOXXVdO?1000<R_e]R_e]=90JLNbSLLKX?""?""?""?""?H906@Od_YYYY]=9005HKcc]WQY?10E>NgdVDTdN>E04DVbfNNN_cfNNNNeaTU308DVe]ROeORF0004DYYYYccYYYYD40HROdO]]_d_L@20000EL_gg_LE0000JDWWX^dTWWMF00000000CUTd`LFFP_dO?[[[[A004<EM^fSI<290007IQafYQ^eTD<00007IQafYQ^eTD<00004UScaeT>E00004<EM^fSI<R_e]=00000<R_dVSSbbQF0004DNdN>OQKfcQG04DNdN>FFFA40008?T`Kc^KKKYdN>5CUTd`L<00<R_dVSSbbQF00002>VdV>NdVD[[[[E08@VdO?<<200002@Yf^X`eTD<000008EFFM`dTU30004DNdN>[[Q^f]Te`X;000CUTd`L=PNgN_dNMF00009B]eaW=P_dO?[[[[E002>NdT?M`N_NPDVdO@40CUTd`L]bc_Q=6008@VdO?@VdO?1000<R_e]R_e]=90;QbV>EE?OOOOOTU106@Od_P[[[["	\
                ":004?LSf^MLL?I71?QNgg^QTdNQ?18@QNdTQQ`N_NQQVdOQ[308DVe]Yc_WH70007H[[[KccKQU[H70=]NdNVVVfbVL<008<;QbaabKH<800EPN^^bga^`M:00000000CUTd`XXXX_dO?177700000EM^fSI00000002@Yf^Od`WG0000002@Yf^Od`WG000000J>Vd^W;0000000EM^fSI<R_e]=000006U]abbbebQF0008@Od^KOQKfcQG04DNdNXXXXMUE000HX_cdeccccc_P<CUTd`L<006U]abbbebQF0000FX^cS>NdVXXXXLJ08@VdO?1000002?>M_eNe`W>D<007F?LWXX`dTU30004DNdND45?Yf^ac]U4000CUTd`LF>NdedbK=C00009B]eaW=P_dOXXXXLE001UTdNPQbYO^WM`e]B70CUTd`SacYVVWG008@VdO?@VdO?1000<R_e]R_e]=90ISfTU3Z?Of___TU106@Od_XXXXWF005>N`db``TU30Z?OcgbebbgcO?Z4DVNd_VVbN_aVV`dNTU30GDVe]a^K?""?A0000077;KccTNME000=]OdO]]]cb]L<07=LLYeVVeYLL=70FXKKK_dOKK?800000000CUTdfbbbbcdO?100000000EM^fSI13000000GW_dec]U80000000GW_dec]U8000008[SbbeT>EC00000EM^fSI<R_e]=1C0000:DXQQQbbQF1707BScdbbbbdcQG04DNdcbbbb`SU6004=MSf^KKKKKKP<CUTd`"	\
                "L<000:DXQQQbbQF000C[]f`M>Ngbbbb^WA08@VdO?110000EMNN_dgd_NNWA00FLY_abbfdTU30005>NdVD40FM_ddNL:0000CUTd`XXSaggeT><000000;Q^fO]]_gbbbb^ME000=Sf^WSfS]cKQbcKH00CUTd^^dbS@@@A008@VdO?@VdO?11002>NfS>NfSI00ISfTU1Z?ObQPPP=306@OdcbbbbQF005>]]e`]]]U301?QNdT`TTdNQ?14DQSf^QSfS]cKQbcKQU37BYNdfdNOOK;000003FUKccSLB4000IXOdOXQOd`XP<03UT`^d``d^`TU305>]]Tf^KIHA000000000CUTdb````bdO?100000000EM^fS?""?B6000006[ScdNLA000000006[ScdNLA000009;W`cS^bS?[50000EM^fS?""?R_e]?""?[40000<FGGQbbQ?""?I70:PT_`^``ecQG04DNda```cd_MJ0001FXacS@UDWYB7CUTd`L<0000<I?""?QbbQF000EPNeT>>Ng^````WA08@VdO?""?UE000EM`^aggga^^WA01UYfe^``bdTU3000<P_dO?60:LNddVPJ0000CUTdfbbcd^ceTD<0000005?S_eeeeg^````ME000GQbcKTaXKc]Sf^W:00CUTdge`d_WG80008@VdO?@VdO?""?UE04DVcKDVcK;00GQaNL;G?ObSSSS=006@Oda````QF005DQ]e`QQQ[30CBPNdTO_TdNPBC0AGQbcKTaXKc]Sf^WGE07BSVdfdTSSK;000"	\
                "00ELY_fcKVPE0005?OdbbbecY?A903U]]ba]]ab]]U305>NedNQU<00000000000CUTd`LLLL_dO?100000000EM^fOOOP2000000AWae]B300000000:IWae]B;Z0000E>OeOP]c_OME0000EM^fOOOO`eOOOLE0000000FQbbOOO@609F@LLOQKfcQG04DNdNLLMYebQF00002DTf^OTV^YB7CUTd`L<00008@OOOcbQF000HKbeNNN^dVLLLL>E08@VdNOOXF000<>WWNd`e_WWRJ02>Nd_XLM`dTU3000EM`dYU34U]fabcS[8000CUTda__NTQOeaK=3000000EDTe^]_dOLLLL><000EM`e]N_LW^OTdN><00CUTddNQaeTD<0008@VdO?@VdNOOQF01UT^QUT^QF00<>OfTQQY^ebbb]=004@Od_LLLLP:005>NbdfbbTU30Z?O^d__c_d^O?Z00EM`e]N_LW^OTdN><000JDVe]ab]?I2000005D]KcfVK><0001?Od`__NS>F0003[Q]fNQQNe]QU305>]Y_e_WG00000000000CUTd`LEEP_dO?100000000EM___d_P2000000AWae]B90000000CUSSae]SMJ0001[S__QHLNe^ME0000EM_________e`LE0000000FQ__`dO@60004E@SQKfcQG04DNdN>EISfcQG00000GLY`cb^N]B7CUTd`L<00008@Vd`__QF004?TebbbbcdVDGGGG<08@V__fbXF0000E?]f^Q`fYDE70<R_"	\
                "dT?EM`dTU304GBKbfS=9HX^fYTe_WG000CUTd`LP>?IX`dNL:000000HQ^eYP_dO?GGGG20004DVdO^OD>N_NdT?Z00CUTd`X@YfaK=C008@V_O?@V__fbQF04[MMP[MMPG009HWOabb`NbQQQQ=04UPOd_P>EE<7005DQ]e`QQQ[20Z?]NdT]aggN]?Z005DVdO^OD>N_NdT?Z0008DVe]Yc^KBZ00000<DQOfcKI<00001?OdOPP>[A00003UTcdaaaaecTU303HB[QafYU80000000000CUTd`L<<P_dO?100000000<DRR]e_P2000000AWae]B90000000CUT^cd^^MJ0001[RRRU5ISf^ME0000<DRRRRRRRRSf`LE0000000:PRROdO@600000FIHKfcQG04DNdN>4HKfcQG000007F?M`OM>BA0CUTd`L<00008@VdORR>:00:M`f]QQQNdVQQQQQB91[RRQbbXF0009HX^fY@]caQI705>Nd_XMM`dTU30GXS_daXFE>Oe_MW`eTD<00CUTd`L<284USbc]U40000<DOe`WP_dOQQQQQ=0003UYebf][?TfcfS=900CUTd`LFM_dNMF001[RRRB[RRQbbQF0<RVV]RVV]=9001H>QKKMObQUUUI32>ONd`OW:00000EPM]e`MMMMU60G>NdT>NdgN>G0003UYebf][?TfcfS=90008DVe]ROe_WH70000<>NKccK;000001?OdO?253000003[W^bQWWQbaW[30009<>Od`MF0000000000"	\
                "CUTd`L<<P_dO?10000000002<B]a_P2000000AWae]B900000003BMWae]M>E00004<<<50ISa^ME000002<<<<<<<ISa`LE00000009<<?OaO@60000000HKfcQG04DNdN>4HKfcQG0000000EL`O?5300CUTd`L<00008@VaO?<<907=Sc^WFF>Ngcbbbb]B904<FQaaXF000E>Oe_MFLNdNRA0C[Sbea``bdTU30;KceaYD2BKbbS[U]caK=30CUTd`L<000J>Oe`W;0009IQafYUP_gcbbbb]=0000IKcgcKH=SfgbQG000CUTd`L<BKbfY?5004<<<84<FQaaQF0<R_e]R_e]=90009<FH;?OcTTTTD4Z=@Od_PU<00000AW^^db^^^VD404>NdTU]egN>50000IKcgcKH=SfgbQG00008DVe]=W_eORF00003=PKccK;000001?OdO?Z00000009=Sf_L::R_fS=700009IKbf][1000000000CUT__L<<P__O?100000000009=XXX>2000000AW__]B9000000004AW__]B<00000000000HXXXPE0000000000000IXXXP<0000000001UXXX?Z0000000HK__QG04DN_N>4HK__QG0000000EL]]?Z000CUT__L<00006?XXXU10002>V_OD55>N______]B9000FWXXMF003BS__SB1=K__]U10EDKO_____TU30;KNYQD:FM__O>EARN_NLA0CUT__L<0009;W__T><00<>O__MFP___"	\
                "____]=0000FW___W:GQ___MJ000CUT__L<<>T__QI000000000FMXXMF0<R__]R__]=90000000Z?O____ND406@O__P2000000AW_______VD404>N_TUQ__N>50000FW___W:GQ___MJ00008DV_]=BK__YD200008;K__K;000001?O_O?Z00000004DO_TUZ1UY_OD500000EPN_NLJ0000000007=RRRD25@RRRB3000000000005FFFJ0000000E>RRRI0000000000E>RRRI0000000000004FFFA700000000000004FFFA90000000000<FFF<00000000GPRRPF06URRRU8GPRRPF00000006IBBG00007=RRRD200000<FFF<00005@RRRI98URRRRRRRRI00001:FF:C003BRRR>E0<@RRR[100E=DPRRRRR=70FP>UH<0FPRRRH93IRRR>A07=RRRD200008[RRRD<00<DRRRU4?RRRRRRRRH0000<DRRRD<E>RRR?50007=RRRD20;RRRRI0000000001:FF:105@RRR@RRRH00000000C[RRRRRRUZ01[RRR?4000000E>RRRRRRRRUZ08URRRB>RRRU80000<DRRRD<E>RRR?500001[RRRH2URRR@200000FPRRPF00000CBRRR[C00000004?RRRH00;RRR?4000003BRRR>E00000000008<<<2005<<<80000000000000000000000000<<<<100000000000<<<<100000"	\
                "00000000000000000000000000000000000000000000000000000003<<<<7004<<<403<<<<7000000000990000008<<<200000000000000005<<<Z004<<<<<<<<100000000000008<<<20005<<<4000074<<<<<<6007<510007<<<<1001<<<<0008<<<20000004<<<200002<<<405<<<<<<<<C000002<<<200<<<<5000008<<<2003<<<<1000000000000000005<<<5<<<10000000004<<<<<<40004<<<5000000002<<<<<<<<40004<<<82<<<40000002<<<200<<<<50000004<<<C05<<<50000007<<<<70000004<<<40000000005<<<C00C<<<500000008<<<<00000000000003CCC0000000000CCC70000000000CCC3000000000CCCCC300000001ZZZZZZZ70000CCC700CCC3000744C00000CCC700000000000000000000000000000000000000000009CC9000000031170000000003CCCCCCCCCC30003CCCCCCCCCC30003CCCCCCCCCC300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000090000900000000000000342<260000000008<<4300000094<<8"	\
                "00000000094<<49000000004<<5Z00003ZZZ90ZZZ1000000000009000000Z44100000000C44100000003300003C000000342<5C012<57000000000000000000000094<<8094<<8000000094<<470000000<[UUUF00000000;UUUB400000000;UUUB40000000;UUUUUU=:C000<U?""?""?""?""?""?""?H900FUUUB4JUUUU<0<=D>[A000;UUUB40000000000A===A00000000002;;5000000000001:=UU=:C00005[UUI270000002[UUUUUUUUUUB40<[UUUUUUUUUUB50<[UUUUUUUUUUB5000005EE500000000005EE500000000094<<4E=J00000008<265F400000342<26A=E00000<F;;Z0000000000000005FBA00JBG590000000000F[D>P>?H400000<I@PPDBA0000<IDPP@H4000000<=DPP@I2000005I@PP>?=<01=?""?""?;F?""?""?[4000E=:000E=:000<I?>>UG100007FUD>UFC00005BB4008=[20000F[DPP>[:U>PD=E00000003G;;G10000000<=DPP?;IDPP?;40000<=DPPD=E000000:WTT]=1000000CUTTTL<00000003UTTTLE000000CUTTTTTYSM=800:WOOOOOOTUC09B]TTMJIKTTQG5UKVN]DE0CUTTTL<0000000003[]]]UC000000000IKQ=A3"	\
                "000000005BMSTTSM=4000EMTTM>I297700FXTTTTTTTTTTMJ0FXTTTTTTTTTTWA0FXTTTTTTTTTTWA00001ULLU8000000001ULLU80000000<=DPP@?SDE00005H@P>U=X?G700F[D>P>?DS?<000<?QKWIC00000000000CEIDW]=90ISXDIE10000000CU]ON_NTXBZ000<?QO__V]B900E@KO__TXB40000E@KO__OQU<0001BQO__NTK;05>OOTUISOOLE00<USDF;F?SDJ00IKTNNYWH0000GL]NNYMH9004BQQ=81IXK[400CU]ON_N]P]N_VKDE000000ELKKMJ000000E@KO__TWQO_NTXB4000HKO__VS>:00000:Wae]D[100003HUTd`L=F0000003UTd`LE0000008UTddeedcNW;00:WabbbedTUC00;QbbQG[YefS=;X^ddbKI0CUTd`L<000000000<>NeNP<000000000FWNWM:00000004[QSP?""?LSQB6005[]^^VQD=B=60FX]WWWWWWWWTMJ0FX]WWWWWWWWTWA0FX]WWWWWWWWTWA00008DVN>4000000008DVN>4000000E@KO__OK_WF0005UQT_NYWNY>20CU]ON_NTSNW:005UKSSOM;7000000004F[RKO^]=90IS^OKLUF500000CUT`NN^e_M:001[S^da`a]B901[Sac__f_X;000Z[Saf^^e^KB300JM_f_N_bK;05>NdTUISf`LE00IK^]WKXK^][30ISYSS^"	\
                "OD5005DTb]]aO><00FW_NWIHWN_XF00CUT`NN^cOc`NbaSU600000EM`^MJ00000Z[Saf^^eN`cN_f_X;000HKb`^eb]@50000:Wae]TLA00002>]Td`]]U3000003UTd`LE000008U>Td`]]TceT?80:MXXXSbaSUC00AM^fS=DVbcT?[Yef^e]B9CUTd`L<000000000GQacaQ;00000091C4DOTX=E000000;XK[I<6J?SWG009B]egdaO]]P<0FX]B::::::UTMJ0FX]B::::[XKTWA0FX]QK[::USWTWA00008DVN>4000005FFFDVN>FFF200Z[Saf^^ebKB8009IQ`b_^eNc][10CUT`NN^e^Q=100JM]N]WOLF900003EIDW]Nb^OK=90IKO^c_]WDIEC00CUSM>>X^cSI00EL_dNQLX]B90JM_fYPLVeT?800JM_dOMWNdNP<00FXb_M>LQK;05>NdYUISf`LE00:"">Ta`ba`VMG00GPK]]N`LE00AM^NPDVaXF002UKaNMMVaS?<00CUSM>>X^dNW>]f_MJ00322EM``MJ22C00JM_dOMWNdfYPLVeT?800HKQLWOe^XF0008FWad_SL:00002>_egdfTU3000003UTd`L:500005>N_daNTPNgN>20CFF[K^c]DE0002>NdT?M`N_NPDVdVX^]B9CUTd`LFFFF<0000C[]cScYU100000;?[5ISa]SI000005DTK]KDE0EDT?809B]egggde_P<0FX]B900003UTMJ0FX]B907IX]?TWA0"	\
                "FX]W_QB[KNWTWA03222DVN>222C00IQQQQVNQQQQ=90JM_dOMW_dNP<004DOf]PQ_dNL:00CUSM>>X^fSI000:WKSSOWTPF0000;PKT^bN]WDIE00<IDW]Nb^OKLH009;PK]YY_e]B70GQbcK=EG@;00GQbbKKK]e_L<00GQbcKH=SfaWF00FWaaTSQL[E05>NdY[ISf`LE000FW`TQ]bQH306?YaY]N`LE00:WaND?TbQG000<?SbOOb]DE0009;PK]YY_eYKKKabQ;00;PPPPLLPPPPH00GQbcKH=SfaKKK]e_R<00:@IE;XafS=008UWYbe]K><44302>STd`SSU300007HLTd`WW=90005>NadcaT>Nd_P<001=X_fT>A00001UTdNPQbYO^WM`e]MQQ=9CUTd`XXXXXU1000<PN^W`NRE00003[YQIFW^_W;;F400AMSbcaS[H[HKLE09B]eggggg_P<0FX]B900003UTMJ0FX]B90APVRUTWA0FX]BQ_XQ_XUTWA0;PPPPVNPPPPH00ISbbbccbbb]=90GQbcKLO]faWF005>NeTSOcbaKH009;PK]YVee]B700FXYKSMOXY>A000ISb`TKP[F59000005F[PQT`b]=90EPObb^`beYB70HKcbQF008900HKceccccc^ME00HKcbQF;KcbQF00<>Taffb_K=95>NdY[ISf`LE000HK^WGPN]B705>NN>M_`LE00EL__MRN^W:0000ED]bcY>A0000EPObb^`bdccccccKH00IS________]=90"	\
                "HKcbQF;Kceccccc`ME000870EM`e]B905DO]aeNVQDDDI93;UTd`L=:00000EPV^d`Y]B90005DWTd`XXSadOD403HWNeOLF<<5000=Sf^WSfS]cKQbcKHFF<0CUTdfbbbbO?1000;QaODObQ;00000HKVXQQNeTKKWB30FQV_K_N>MYPWW:09B]eggggg_P<0FX]B900003UTMJ0FX]B91[SK=UTWA0FX]B[KN_QBUTWA0IS___`^___]=90IS]]]VN]]]]=90HKcbQYTKcbQF00C[Saff^TYeTU10EPObec`beYB7004BQ]SKWTQ]UC00ISb^OKLUG<7000072GURKT`b]=90GQbbSMM_eYB70;QccKH8J=:00;KcaKKKKKKME00;KcbQ;IKfaXF008H>Q]T_dOD45>NdTU[]e`LE000HK^WGPN]B708@TbONT`LE006USaNNa]@50000JDYccY>:0000GQbbSMM_e]KKKKKKH00ISNNNNNNNN]=90;KcbQ;IKfaKKKKKKLE00EBF4FW^f]=903IMQae]>MNNTU32>STd`SSU30000FX_Tdb_XG00000<UTdfbbcd^KI90FLOeNMLLLL?400GQbcKTaXKc]Sf^W:0000CUTdb````O?1001UYcS=KcYUZ0000IQ_]YY_dNTTSU10GQ^NMN_RXNMWXF09B]cbfdgg_P<0FX]B900003UTMJ0FX]BHGMO><UTWA0FX]BHWNNM;UTWA0ISNNN``NNN]=90JBBBDVN>BBB:00;Kcb]VLKfaXF000E@QSSW"	\
                "@KcVD80GQbgfOM_eYB70005[KS]KXYN>200GPQT`b_YX>=E00E=>X]Nb^TKPH00HKcaWUQaeYB70:M`eOL@RSB90:W`f]DUDW]UC00:W^eY>PTd_LE00AWWDUU]eN>54DVdVLMNd`LE009FW`TQYaQHC00GLS]QKKLE009G@Q]]QD;3000E@]bOTb]DE000HKcaWUQadVLU?LKL<00G>>>>MMP>>>;00:W^eY>PTdf]DUDW]UC00HKMDPYfbQ;005>TNbe]=Xae]B72>_fgefTU30000:PLTd`LB400000CUTda__NTQU200;Kcd`````_P200EM`e]N_LW^OTdN><0000CUTd`LLLLL[C00ERN`M:M`_LE0007B]S[BM`^QUUBJ00:W^f^faQ[>USMJ09B]KQ]_cf_P<0FX]B900003UTMJ0FX]>KXSKI3UTWA0FX]BMVS]OLUTWA0G>>>>VN>>>>;009444DVN>444700:W`dNWPTd_LE00FULMPU=DYeTU10HKcdNMQaeYB70000<USKYQ]SW:007<H?WSNbaVS=90ISV^bN]W@HE300FX^eNT_`eYB704?]cf_V_]B904?YcaOTO`TUC004?YcbVNcb]UZ00AW^VTT`eT?8CU]fc__Oe`LE00:PTa`c^`VM;06@SSSSSSP2004DSSSSSSP200<USaVMLObS?<00FX^eNT_V^fNTT_`L<00755EM`^MJ553004?YcbVNc^aaOTO`TUC00HK_VNcfO><003BSWae]>TebQ;02>]Td`]]UGGE009<UT"	\
                "d`LGGGGJ00CUTd`LP>?I2000;K______d_P2004DVdO^OD>N_NdT?Z0000CUTd`LEEEE4000HQbT@<?TbKH0007BM>E;QbNSI37000<>NdggdNKWSOD509B]MG[MKSK><0FX]B900003UTMJ0FX]XS_OMA3UTWA0FX]LO]@D]T>TWA07555DVN>555300G>>>>KK>>>>;004?YfbNNcb]UZ01UY_^_YSTb^QH00FX^eNT_`eYB700000E@]KN]XKH000004FBPQT`]=90IS^TKP[F4000005@]^a_K__YB700:"">]_aa`]B900FP]_aa^N]UC000FPY`aa_]>A000AWV`ab^OW;00FMVaaTS__LE00IK^]WQWK^S[36@O^^^^_P2004DN^^^^_P200FWNNWH;MN_XF005@]`b^TMKNab^_TL<00000EM`^MJ000000FPY`aaNKS_aa^N]UC00HK_aa`YM;9000EIWae]TceO>E03HUTd`QQQQQ?6003UTd`QQQQQ?80CUTd`L<2800000FPRRRRR]e_P2003UYebf][?TfcfS=90000CUTd`L<000000ZUYcKLLLKcT?60004E<1UYVMDF000007=Kagggdb^^QI909B]MJ1AHIHA00FX]B444444UTMJ0FX]BU]VD55UTWA0FX]S]DJ:"">YKTWA00008DVN>400000ISNNNNNNNN]=90;WN]_aa_]>A000;QOYNabaNKU500HX_^a_K__YB7000000JDSOYKYUC0000003<H?WS=90ISW@HEC00000000"	\
                "A?WXM?RRRI0000:ULWXMDG0009:ULXXM>[:00009F?MXXLUA00005[>MXXWDH1001HPXWDRRRD<00<USD:GFUS@E01UMMMMMMD5008UMMMMMMD5008=QX=Z1IWQB4000JUMXWDG=PWXWR?;100000ELQQMJ0000009F?MXWPBULXXM>[:000A?MXXM?F300000:WadbefNWH900CUTdfbbbbO@6003UTdfbbbbV@80CUTd`L<00000007<<<<<B]a_P2000IKcgcKH=SfgbQG00000CUTd`L<000000EL_c`````c_MJ000000JL_SXF0000000<D]adggdaS?2009B]MJ00000000FX]>>>>>>>>TMJ0FX]>>MR>>>>TWA0FX]>>>>>>P>TWA00008DVN>400000IS________]=90;M]@MXXLUA0000<D>[PWXWPB<000HWSWXM?RRRI00000000A=@MKT>50000000004F=J00E=F40000000000004:FJ5<<<1000006EFFJ40000008EFFA2C00000008EFFE600000035JFFA5000007<F:2<<<20000E=:000EIJ0005EEEEEE<00002EEEEEE<00004==4006IB5000004AF:503<"":F:<600000003FGGFC000000008EFF<C6EFFA23000008EFFJ40000000:W___OSMI6000CUT______O@6003UT______V@80CUT__L<0000000000009=XXX>2000FW___W:GQ___MJ00000CUT__L"	\
                "<000000HK_________KI000000IK]UI500000000ADKO__OQ?E0009B]MJ00000000FXNNNNNNNNNNMJ0FXNNNNNNNNNNWA0FXNNNNNNNNNNWA00001[RRU800000;RRRRRRRRRRH00C;UAEFFE6000000<<C<"":F:<C0000ZIU:FJ5<<<100000000094JHUB600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007700007300000000000000000000000000000000000000000000000000000000000000000000000J>RRPDB:300007=RRRRRRRR[1007=RRRRRRRR[107=RRRD200000000000005FFFJ0000<DRRRD<E>RRR?5000007=RRRD2000000;RRRRRRRRRRRH000000AUUJ00000000000<IDPP@I200000HPD<00000000J>RRRRRRRRRRD<0J>RRRRRRRRRRD<0J>RRRRRRRRRRD<000004<<4000000C<<<<<<<<<<10009100000000000000000000000000710000000000000000000001C00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000<<<<4700000008<<<<<<<<400006<<<<<<<<40008<<<20000000000000000000000002<<<200<<<<500000008<<<20000000C<<<<<<<<<<<1000000033000000000000094<<40000000C250000000000<<<<<<<<<<<2000<<<<<<<<<<<2000<<<<<<<<<<<20000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008<<43000094<<4700000000C5<4CZZ10000004<<5100003ZZZ9000000094<<49000000ZZZZZZZZZZZ000000031ZZZZZZ0007ZZZZZZZZZ3003ZZZ9ZZZ0000000CZZ1009ZZZ00007ZZZZZZZZZZZZ1"	\
                "000001Z300076C00000008<<43000000012<269000Z2<2Z00000000342<260000000094<<800000000Z5<<47000000094<<490000000008<<4300000Z2<5C000007ZZZ7094<<490000000008<<430000004<<5Z000000CZZ1009ZZZ00007ZZZ7015<5100000094<<49000000000005<C0000000000082500000000000000074<<60000000342<5C012<570000000062<2Z00000000094<<490000000000CZZZ000000000Z5<2800000000000ZZZ10000000<I@PPDBA00<=DPPD=E000009:[>PD[?""?U<0004I@PP>UG001=?""?""?;000000<=DPP@I20000:?""?""?""?""?""?""?""?""?""?""?G0000<HU?""?""?""?""?""?""?F09H?""?""?""?""?""?""?""?""?I31=?""?""?;?""?""?F000004[?""?U<0;?""?""?F009H?""?""?""?""?""?""?""?""?""?""?""?U200C;?""?[GZ<=@[E0000<I@PPDBA0000ZGU>P>?IC3H?>P>U;400000F[D>P>?H400000<IDPP@H400001I?>PPD=<00000<=DPP@I2000000<I@PPDBA001H?>P>UFC003I?""?""?I<IDPPD=<000000<I@PPDBA0005I@PP>?=<0004[?""?U<0;?""?""?F003I?""?""?IFU>P>U;3000<=DPP@I2000000CJ;IDP=IF50000004H?>@2AA<7000000000EBDPP?;60000F[DPP>[:U>PD=E00009AI?>P"	\
                ">U;8000000<=DPP@I2000000008B?""?""?:0000003H?>P>?;600000000:?""?""?U<00000<?QO__V]B90HKO__VS>:0000FP]N_VQOOW:003=QO__NT?105>OOY[30000E@KO__OQU<000HKOOOOOOOOO]=900E@QYOOOOOOSI0CUTOOOOOOOOD85>OOY[QOT>E0000GWOOMA3[]OY@50CUTOOOOOOOOOOOWA00EMTOYWI?KOSU100<?QO__V]B9006=WYN_NT>54DTN_NTXB4000CU]ON_NTXBZ000E@KO__TXB40002PTN__VKU4000E@KO__OQU<0000<?QO__V]B90ELTN_N]MH308@OOO?""?KO__OK@E0000<?QO__V]B901BQO__NTK;000GWOOMA3[]OY@508@OOO?MYN_NYP200E@KO__OQU<00004=LKSYNSSW?A00005[XTNLDMW>I60000AFFADSV_NTW=100CU]ON_N]P]N_VKDE000<>KTN_NYWB40000E@KO__OQU<0000000<POOKH0000004>TN_NTWH90000000HKOOWA00001[S^da`a]B90HKb`^eb]@5004@YcbNb_cVD500<>Nd^VNO?105>NdY[3000Z[Saf^^e^KB300HKbfdbbbdfb]=90Z[Saf^^edcbSI0CUTbbbdcbbVD85>NdY[DObS=3008?YeTU69FW^^WG0CUTcdbbbbbbcd^WA00EM`^eNPQ^dN>501[S^da`a]B900GWNe^N_N>54DN_N`e_XH000CUT`NN^e_M:"	\
                "001[Sac__f_X;0002P__N`e^W:00Z[Saf^^e^KB3001[S^da`a]B90EL`^`cfVMF08@VdO?K^e^^faS[1001[S^ea`a]B90JM_f_N_bK;008?YeTU69FW^^WG08@VdOWNfc`^_P20Z[Saf`^e^KB3007IXNc^^c`b^]D<009IQ`e`LT^^VWH0005>QQU]bbN_fNWF00CUT`NN^cOc`NbaSU600<L`a_N^e_W;000Z[Saf`^e^KB3000000<P_fKH<<<<904>NNVaeOD4<<<<000HKfaWA0000EL_dNQMX]B90HKQMWOe^XF00AM`eY>Yef]B700EL`bQD>WU105>NdY[3000JM_dOMWNdNP<00;XX`fSXKcaXX=00JM_dOMWNdNXXI0CUXXXTdVXXX?85>NdY[=]fV>500JM`fSIBBB>Nf][33UX`fSXXXXXNe]MJ00EMKW^^WTdeT?80EL_dNQLX]B906?Te_X>PX>54DQR>WNeOD400CUSM>>X^cSI00JM_fYPLVeT?8002PXP>W`bQF00JM_dOMWNdNP<00EL_dNQLX]B90ELSMMSbf][38@VdORNdVWMOd_LE00EL_dNXLX]B90FXb_M>LQK;00JM`fSIBBB>Nf][38@VdOYeaSMMSP20JM_dOMWNdNP<00<PVdOWQSMSbaKI005>Od_X]f`_eOD5005>NOW`c]>MNfYU10CUSM>>X^dNW>]f_MJ00<LTXP>WNeT?800JM_dOMWNdNP<0009<GIP_fKH>LLP:04DW>DSfN>FRLLD<00HKbaWA000"	\
                "0GQbcK]^_R;00:@]^_XafS=00FQbbQ;KcaXG0004DTbOSSUE005>NdY[3000GQbcKH=SfaWF004FL`fS=KcaWF500GQbcKH=SfaWF400<FFUTdVDFFE05>NdY[HKf`LE00GQbbQL]]]@OdVD40GQbbQL]]]@OdVD4002?UP_^]`dNW;00GQbcK=EG@;00<P_e]XXXX>54DXXXX]f`LE009;PK]YY_e]B70GQbbKKK]e_L<007GPSSYbVLJ00GQbcKH=SfaWF00GQbcK=EG@;005ULWWWNdVD48@VdOQafS=IKcbQG00GQbcKWWW@;00FWaaTSQL[E00GQbbQL]]]@OdVD48@VdONdOKKKU[80GQbcKQQSfaWF00GQbcKII=:PNdTU10AM^e]DNf]Qb`ME008?T`Kc^KKKYdN>509;PK]YY_eYKKKabQ;00;WWWWWW]e_L<00GQbcKQQSfaWF009G>QSS_fKHW``XF00JDSSVa]U;K``LE00HXQQMA0000HKcbQ]e_R<009=]e_M`e]B90GQcaXGQaNP<000ZBQTfcTU1005>NdY[3000HKcbQF;KcbQF000<L`fS=KcaW:000HKcbQF;KcbQF000001UTdVD80005>NdY[HKf`ME00HKcbXL`e]?OdN>50HKcbXL`e]?OdN>50008=W^fcd_X=Z00HKcbQF008900EL`dbbbbN>54DNbbbbd^MJ00EPObb^`beYB70HKceccccc^ME000<P_bfVK@<00HKcbQF;KcbQF00HKcbQF0089000JM^aabgN>"	\
                "58@VdbbecK;GQbcK;00HKceaaaTU300<>Taffb_K=90HKcbXL`e]?OdN>58@Vdffgfcc]=100HKceaaaadbQF00HKcbQF094DVdVD80FWafS>NfSXa`ME000HX_cdeccccc_P<0EPObb^`bdccccccKH00HKaaaaaad^ME00HKceaaaadbQF00EPOacbffKHW^bXF004DNcfTQ=;Kc`LE004FFFF30000;QccK]__R:00EB]__W^f]=90FQbbQB]f_P<000JM_bYQQUJ705>NdY[3000;KcbQ;IKfaXF000<L`fS=KcaW:000;KcbQ;IKfaXF000001UTdVD80005>NdY[=Sf_P<00;KcbQM^eY@OdN>50;KcbQM^eY@OdN>5009HWNeaTfTU=F00;QccKH8J=:00<R_e]SSSS>54DSSSSSf`ME00GQbbSMM_eYB70;KcaKKKKKKME004;>QQS^aQ;00;KcbQ;IKfaXF00;QccKH8J=:00ZIMKKKVdN>58@VdOSbcKH;KcbQG00;QccKKKKU:008H>Q]T_dOD40;KcbQM^eY@OdN>58@VdO_dT]]]=HC0;KcbQWWSfaXF00GQbbQ;:F2>NdO?Z0:W^f]?TeYSfOD<0004=MSf^KKKKKKP<0GQbbSMM_e]KKKKKKH00;KcaYYYYe`LE00;KcbQWWSfaXF00GQb^KQ_fSIWabXF004DQKTfNLHKc`LE00GLLL>E0000:M`eOLPRSB90HKMPPYfbQ;00AW^fS>Vd^W?E00FXa^M[UPRE05>NdTUF300:W^e"	\
                "Y>PTd_LE000<L`fS=KcaW;<00:W^eY>PTd_LE000001UTdVDGZ004>NdTUPOdO@400FXaf]Qb`VM_dT?60FXaf]Kb`VM_dT?600<>OefSSfO?K?10:M`eOL@RSB904DOdVLU?RD54@L?UPOdV>500HKcaWUQaeYB70:W`f]DUDW]UC00GQM?UPNfSI00:W^eY>PTd_LE00:M`eOL@RSB90ELXDDKaeYU18@VdOM_eTP>Te`MA00:M`eOL@PSB90AWWDUU]eN>50FXaf]Qb`VM_dT?68@VdOTd`QD>QP20:W^eY>PTd_LE00JM_eY>MQ@Q^cS=90<>NdNWWN`_NX>D<0001FXacS@UDWYB70HKcaWUQadVLU?LKL<00:W^cKU>OdOD400:W^eY>PTd_LE00;Kc_LW^dTPSc^W:000JG;WaaQPTe_P<00HK``WA00004?]cf_V_]B90HK_VNcfO><005DOd^ObafVQG00AM`fNYYNMJ04>NdVKR<004?YcbVNcb]UZ000<L`fS=KccSK=004?YcbVNcb]UZ00000CUTd_KW:00Z?Te`YOcaKI900<PNdb`^Yc`ecS=90<PNda`^Yc`ecS=900FWag`WQa^TO?104?]cf_V_]B900IQ^fNTTN>54DNTTNcaK=700FX^eNT_`eYB704?YcaOTO`TUC00GQ`OTVcaQ;004?YcbVNcb]UZ004?]cf_V_]B90EL`VV^d_WG08@VdO?]ccNNbc]?4004?]cf_VN]B90AW^VTT`eT?80<PNdb`^Yc`ecS=98@VdOQ"	\
                "`d^VN_P204?YcbVNcb]UZ001[K^bVNaV`cOPJ003=Kae_V_ee_VNLE00002DTf^OTV^YB70FX^eNT_V^fNTT_`L<004?Yc`TOc^KI9004?YcbVNcb]UZ00FXafVNOcc_aeOD200000<PNe^_bc]U100HKfaWA00000:"">]_aa`]B90HK_aa`YM;9009IXNaa^SO_QG008UKNaba_MJ06?Yac`L<000FPY`aa_]>A0000<L__S=W_cb]=000FPY`aa_]>A0000007BSacaW:000;WV`a^NSD<0003=Q_a^YXNa^TPJ003=Q_a^YXNa^TPJ000JM_`SUD]_^O?100:"">]_aa`]B9002UKN^baN>54DN^baNK?<0005@]^a_K__YB700FP]_aa^N]UC00GQ_aba_]D<000FPY`aa_]>A0000:"">]_aa`]B90ELNab^VQ[408@V_O?>]_aa_]P:0000:"">]_aa`]B90AWV`ab^OW;003=Q_a^YXNa^TPJ08@V_O?KN^b^NP200FPY`aa_]>A0000<?QYOV^OTSR;9000<?KNaa`TO^b`LE00000GLY`aa^VSB705@]`b^TMKNab^_TL<000FPY`aaNK?<0000FPY`aa_]>A0002DY^aNWY`b^OM;0000007IWO^b^TP:000HK__WA000000:ULWXMDG00A?MXXM?F300004IPWWM?DRP:000<BPWXWRU200F>QQX><0009F?MXXLUA000002DRRR;[MQQWI0009F?MXXLUA00000000JDXQQRA0006I>MWWP[E00"	\
                "0005BLXWD=LXWDF90005BLXWD=LXWDF9000Z=LMUEAULMPI7000:ULWXMDG0000<=>WXWRU81[PWXWPB<00000A?WXM?RRRI0009:ULXXM>[:000J?LWXWLUA00009F?MXXLUA000000:ULWXMDG004[PWXW>I5001[RRRB:UMXXMU:900000:ULWXMDG005[>MXXWDH10005BLXWD=LXWDF901[RRRB=>WXW>B6009F?MXXLUA0000005;[?MW?U=A700000<BRXXM@DWXWU5000007F?MXXM>BA000JUMXWDG=PWXWR?;10009F?MXWPB<000009F?MXXLUA00000:DWXL=?MXW>H100000006H>WXWDF9000GPRR>E00000006EFFJ400008EFFJ400000003<""::A55<<90000C<"":F:<Z0000<FGFE0000008EFFE600000002<<<C8:GG:8000008EFFE60000000000<FGFE7000095JAA<100000001EF:5CEF:20000001EF:5CEF:20000001EE4006<J<3000006EFFJ40000003<"":F:<8006<"":F:<C00000004:FJ5<<<1000008EFFA2C000004E:F:E600000008EFFE6000000006EFFJ40000C<"":F:2700004<<<808EFFE8000000006EFFJ4000035JFFA50000001EF:5CEF:200004<<<87<"":F:<3000008EFFE60000000003ZAF51900000000CEFFJ45AF:4000000004JF"	\
                "FJ53000004AF:503<"":F:<600000004AF:<C000000008EFFE600000005:FEC4AF:<900000000092:FA5000003<<<<0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000ZZZZZZZ000000007ZZZ77ZZZ70000000000031170000CZZZZZZZZ1000000004<<51000000071ZZZZ4<2Z0000000000000000000000000000000000000000000640000000000000000003ZZZC2<Z0C2<800007ZZZ35<4000007ZZZ35<4000"	\
                "7ZZZ700ZZZ10000ZZZC0ZZZ79ZZZ3009ZZZC07ZZZC00001ZZZZZZZ7007ZZZ7007ZZZ70ZZZZ0007ZZZ77ZZZC0007ZZZ70ZZZ0000004<21004<<6007ZZZZ13000007ZZZZZZ10000ZZZ101ZZZ03ZZZ30007ZZZ77ZZZ3007ZZZ70ZZZZ000000ZZZZZZZZ0007ZZZ100ZZZZ0007ZZZ77ZZZ7007ZZZZZZZZ700CZZZZZZZZ10009ZZZC07ZZZC00001ZZZ01ZZ10007ZZZ79ZZZ30ZZZC007ZZZZZZ000000007ZZZ70000000000F?""?""?""?""?""?""?G0000003I?""?""?II?""?""?I7000000004;[?""?BG408B?""?""?""?""?""?""?""?""?E000004I@PP>UG00003FB?""?""?""?""?DP>UG1000005F;;AZZZ100000000<=G100000000000009JU@:0000000000005E<03I?""?""?B>PU:[>P?F903I?""?""?=>PDH1003I?""?""?=>P?203I?""?""?I7A?""?""?U<00F?""?""?B:?""?""?H;?""?""?I30;?""?""?[4I?""?""?B600<U?""?""?""?""?""?""?H93I?""?""?I73I?""?""?I:?""?""?""?G03I?""?""?II?""?""?[403I?""?""?I:?""?""?;1000ADP>UEG@PPU43I?""?""?""?""?[G8003I?""?""?""?""?""?U<00F?""?""?UEU?""?""?:B?""?""?=C03I?""?""?II?""?""?=13I?""?""?IF?""?""?""?E0000F?""?""?""?""?""?""?""?J03I?""?""?U2A?""?""?""?G03I?""?""?II?""?""?I73I?""?""?""?""?""?""?""?I78B?""?""?""?""?""?""?""?""?E00;?""?""?[4I?""?""?B6005U?""?""?F?""?""?""?E03I?""?""?IH?""?""?=F?"	\
                "?""?[47I?""?""?""?""?""?F0000003I?""?""?I7000000000ISOOOOO]B9000008@OOO?@OOO?10000000<UXYOT]W=92POOOOOOOOQF00003=QO__NT?1000;M]TOOOOV_NYWIC000J@XKKL?""?""?U<000000A?KM=500000000008G=?]]UC0000000000FDMD<8DOOOKN_]>SN_YPA08@OOOKV_OW;008@OOOKN_ME06?TOOD5=SOOMA00ISOOL=SOO@?TOO@83[SOOXHROOT>500:WOOOOOOTUC8@OOO?18@OOO?DYOOK=08@OOO?WOOTP<06?TOODHQOOMG000GQ_NSUPT__P<8@OOOOO]W=308@OOOOOOW:00=KOOS?MOOK=XOO]@408@OOO?POOO>58@OOO?>TOOWF0000ISOOOOOOK;08@OOOXG=SOO]B98@OOO?@OOO?18@OOOOOOOO?12POOOOOOOOQF03[SOOXHROOT>500EMOOSIQOOQF08@OOO?UTOOD=]OOR<1?OOOOOS=0000008@OOO?1000000000ISfa_`e]B2<<<C08@VdO?@VdO?4<<<100CBK`b__a]B92P___be___QF<900<>Nd^VNO?1004DTfa__edaN_eNMF003BS^V^]SOOMA0000CG>]^NQUE000000002[W]KaKB3000000000EDYVRE8DVdO^aebSaaecSB78@VdO^^feTD408@VdO`c`ME00HKb^WFDOdT@400FXaaXUTd_RPNf]B70J>OeO>]c_W;000:WabbbedTUC8@VdO?18"	\
                "@VdO@]b^KU<08@VdOWNfTRG900HKb^WFUYeT?800GQ_c^Q]ca_P<8@Vd`_`eNP<08@Vd`___W:002UK^aSM^fKWNcY>:008@VdO?ScdN>58@VdO>Yc_X=60000ISffbbecK;08@VdeYU>Vde]B98@VdO?@VdO?18@VdbbbbdO?12P___be___QF00J>OeO>]c_W;000EM^fSIQbbQF08@VdO?UTdND=]e_R<1?O__afS=0000008@VdO?1000000000ISf`LVe]B@LLL;08@VdO?@VdO?ULLLH00ELNaSLLX]B94?PPM^fSPPLLPA00EL`bQD>WU100<P_f]PSfcS>W_f][304DVbQ`]OdT@40004IMTcgd`SDF7000004[KSX_]@:FF40000006[S`XI78DVdaKW_dbSWNdO@88@VdbSWTe`LE08@VdcTQKME00EL_fS=M`cKI000<P_fS>N`aXW^bQG009HX`bS`bS[8000:MXXXSbaSUC8@VdO?18@VdOSa^KU<008@VdONfTLG7000EL_fS=HKc_LE00FRPTeN^bSR@58@VdOPQa^WA08@VdOPPP>E000<USa^K^fKNfTPF0008@VdOLNdgN>58@VdO]b_Q=400000ISf`XXacK;08@Vdg_MQage]B98@VdO?@VdO?18@VdOXXVdO?14?PPM^fSPP>:009HX`bS`bS[8000EM^fSIQbbQF08@VdO?UTdND=]e_R<3BPPL`fS=IG<0008@VdO?I;E7000009=]e`LVe]BP_`SI08@VdOSSVdO?@V`"	\
                "]=00GQbOD;RRRUZ04<EM^fSIM``XF004DTbOSSUE0002>Nf]PSf`KKKTdVD404DVfN_W`cKI000EUQNfddddbYLH60000;XKMNTKSMXXI900000GW_SU408DVdN>@OdNPUTdN>48@VdN>ISf^MJ08@Vd_L;I=800ZUYeODKc_MJ0008?TdTW^]aSKc`ME0004UScaeT>E0000CFF[K^c]DE08@VdO?18@VdO^aS?<0008@Vdbe`WG70000ZUYeODHKc^WA007<;W_dfT>E208@VdOSYcNP<08@VdO?<<200000E@Sb`bfNeOLG90008@VdO]ccgN>58@VdNb`QB4000000=Sf`LWacK;08@Vd^cSTbbe]B98@VdOSSVdO?18@VdO?@VdO?104<EM^fSI<290004UScaeT>E0000EL`eYMQbbQF08@VdO?UTdND=]e_R<08<<L`fSSKXDF008@VdOSSQPH300003[YeN>Ve]BP_fSI08@VdcbbcdO?@Ve]=00HKcTUIS_ND4000EM^fSIM^aXF00ZBQTfcTU10003=K^a__efcccccN>50HXTd_QKc_MJ00CUSOOOOOOOOOOWF0001UY>TNQ=MKQYD4004J<DTT>E008DVdO?""?OdVDUTdN>48@VdO?HKf^MJ08@VdO?6990000;Qb^WTeT?60009=SfNKbQ_YYeVD40000J>Vd^W;0000001=X_fT>A008@VdO?18@VdcdVMG90008@VdfceTD<00000;Qb^W?Ye_LE0000<DYeaKB1008@Vdcbc`SD<0"	\
                "8@VdO?10000000EDTffggee_WG00008@VdO_aTdN>58@VddcbS[8000009=]e_RWacK;08@VdO`V`V_e]B98@VdcbbcdO?18@VdO?@VdO?1000EM^fSI0000000J>Vd^W;000005DOfc^^fbQF08@VdO?UTdND=]e_R<000<L`dbbcaYD208@VdcbcbOLA00072DVdODVeY[P_fSI08@VdOQQVdO@DVe]=00;KcTU=SfND4000EM^fSIW^aXF00JM_bYQQUJ7000<?YeVVf`KKKKKK>50GXNb^WTeT?6007=PPPPPPPPPPP>:000<P]K`KXDF>]KL<0<UM?K_W;0008DVdO?""?OdV@UTdN>48@VdO?HKf^MJ08@VdO?1000000<PNcS`bQH00000GQb^]^MONVe][30008[SbbeT>E00003HWNeOLF<808@VdO?18@VdO_fTPF0008@VdOTeaK=30000<PNcSQ^eY?80006[Kabf_WG008@VdOQK`aQ;08@VdO?10000006[Kb^YceNVeT>E0008@VdafTTdN>58@Vd^Sb`WH90009<[YeN>WacK;08@VdOTfcS_e]B98@VdOQQVdO?18@VdO?@VdO?1000EM^fSI0000008[SbbeT>E00000;M]YYYbbQF08@VdO?UTdND=]e_R<000<L`fSQSb^W:08@VdOQK`cKH009IDQ^fS?TdNLQacK;08@VdO?UTd`WW`cSI00:W^_MUYeO@8000<L_dOPSc^WA00FXa^M[UPRE000;X^aQQbbK?UDW]=905DR"	\
                "NcS`bQH000CUT__________QG000FX]T]SQ?<D]KLE0;Q`WNS[80008DVdO?""?OdV@UTdN>48@VdO?HKf^MJ08@VdO?1000000C[]eVc_RE00000EL_cVN>]^`bK;0009;W`cS^bS[6000FLOeNMLLLB38@VdO?18@VdOK^cY>A008@VdOW_dNL:0000C[]eONd_WG000<BW_fYNeT>I18@VdO?>NfSI08@VdO?1000000;W_c]M^fKQ^bS[8008@Vdg^XTdN>58@VdO>OeOPA000J>Q_dT?WacK;08@VdOQa_M_e]B98@VdO?@VdO?18@VdO?@VdO?1000EM^fSI000009;W`cS^bS[60000CFB[[QbbQF08@VdOLLTdNLL]e_R<000<L`fS?W^bXF08@VdO?>NfSI00CUT^d`WIKaf_^dNLJ08@VdO?IKae``e_M:005@TfVS_aKI90006UYfc_afTD500AM`fNYYNMJ00<DTeTD>Of^TTV^]=000<U]eVc_RE000CUTdggggggggbQG000;QVSYPMMRWKKP<0FW^TOPJ00008DVdO?""?OdV@UTdN>48@VdO?HKf^MJ08@VdO?10000000GXadeYU1000004DOddTUKcg^MA000E>OeOP]c_W;000;Kcd````TUC8@VdO?18@VdO?Sbb]@E08@VdO?Scc]U40000GXace^KB1000GQVe_YYcbYP<8@VdNOVcaQ;08@VdO?100000E>TeOPM^fKU]c_W;008@VdfY?TdN>58@VdO?W_c]U500FXad^QIW"	\
                "acK;08@VdO>QQP_e]B98@VdO?@VdO?18@VdO?@VdO?1000EM^fSI00000E>OeOP]c_W;00000097FQbbQF08@Vd````d^```d_R<000<L`eOONf_MJ08@VdNOVcaQ;00CUT^VKU<@]`aaNQ=308@V_O?<@S_aaNQBZ000FLY`a^]D<00000FLT^b^OMG0008UKNaba_MJ00IQ__X;;MT`aa`VS=0000GQadeYU1000CUTdggggggggbQG004?TOQSKQQKQSY@40<>N^QH900008DV_O?""?O_V@UT_N>48@V_O?HK__MJ08@V_O?10000000<>N__Q;0000007B]__SIW__N>5001[S__QHLN_T><00;K______TUC8@V_O?18@V_O?DY__SU88@V_O?PV__W;0000<>N__]?<0000GQ________P<8@V____NSD<08@V_O?100001[S__XHM__KHPO_O>E08@V__WUT_N>58@V_O?BK__QI00FX_OQU:W__K;08@V_O?GGP__]B98@V_O?@V_O?18@V_O?@V_O?1000EM__SI00001[S__QHLN_T><0000000FQ__QF08@V____________R<000<L_____OQ[Z08@V____NS@<00CUXM>=20JUMXWP=4001[RRRB3EUMXWR=500007F@MXW?A0000007GDWXW>;C0000<BPWXWRU200HRRR?43G?MXXM>BE0000<>N__Q;0000CUT__________QG00:WK>HIIG;H>PPI903BKK@5000001[RRRBBRRR"	\
                "[=RRRU81[RRRBGRRR><01[RRRB300000003BRRRD<0000000GRRRPFDRRRB3001[RRRU5IRRRD<00FPRRRRRRRB31[RRRB31[RRRB:"">RRRU61[RRRBIRRRRG00003BRRRDJ00000FPRRRRRRRR?51[RRRRR>[J001[RRRB300001[RRRU<DRRRGHRRR><01[RRRU=RRRU81[RRRB<DRRRI00J>P@I5E>RRPF01[RRRB35?RRRI01[RRRB[RRRBC1[RRRB[RRRBC000<DRRRG00001[RRRU5IRRRD<0000000:PRR>:01[RRRRRRRRRRRRR@50002DRRRRPDI5001[RRRRR>BE0000<FA270008JF:<300004<<<8008EFFEC00000004AF:40000000005AF:29000000C<"":F:<Z0001<<<50004JFFJ57000003BRRRD<00003BRRRRRRRRRRRP:002[I<7000002<<1000<HIE00000004<<<84<<<46<<<4004<<<83<<<20004<<<80000000008<<<2000000003<<<<72<<<800004<<<501<<<20007<<<<<<<<8004<<<8004<<<80<<<<4004<<<81<<<<3000008<<<20000007<<<<<<<<<5004<<<<<5C00004<<<80000004<<<402<<<3C<<<20004<<<46<<<4004<<<802<<<1000<<4000<<<<7004<<<8005<<<1004<<<84<<<4004<<<84<<<4000002<<<3000004<<<501"	\
                "<<<2000000009<<<<9004<<<<<<<<<<<<<5000002<<<<<4900004<<<<<5300000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008<<<20000008<<<<<<<<<<<<9000390000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007ZZZ70007ZZZ70000071ZZZZ1000009ZZZZZZZ100000007ZZZ77ZZZ70000000000001ZZC00000007ZZZ70003ZZ1000000001ZZZZZZZZ10007ZZZ7ZZZZZZZZZ7003ZZZ90075<210000000610000000ZZZZZZZ0007ZZZ70ZZZZ0007ZZZZZZ30ZZZZ0007ZZZ77ZZZZZZ100001ZZZ01ZZ10"	\
                "009ZZZC07ZZZC00000000CZZZ000000ZZZZZZZ109ZZZC0000ZZZC0ZZZ79ZZZ3007ZZZ77ZZZZ0007ZZZ70004<4900000ZZZC0ZZZ7004<4900007ZZZ700000000011984300000000Z5500000000000000000000000000000000000000000000000000000000000000000000000144100000784410000007ZZ100000111111111100000000000000000004<590000CC30003I?""?""?I703I?""?""?I7003FB?""?""?""?""?[50000;?""?""?""?""?""?""?U<000003I?""?""?II?""?""?I700000000005[?""?[4000003I?""?""?I701=?""?U<000000<U?""?""?""?""?""?""?""?U503I?""?""?I?""?""?""?""?""?""?""?""?H91=?""?""?;04IDP>BC0005IFU[E90000F?""?""?""?""?""?""?F03I?""?""?I;?""?""?""?E07I?""?""?""?""?""?=:?""?""?""?:03I?""?""?II?""?""?""?""?""?U<005U?""?""?F?""?""?""?E00;?""?""?[4I?""?""?B60000008B?""?""?G0000F?""?""?""?""?""?""?U2;?""?""?[400F?""?""?B:?""?""?H;?""?""?I33I?""?""?[[?""?""?""?E03I?""?""?I71;DPDH6000F?""?""?B:?""?""?H1HDPDH6003I?""?""?I70000004;UUI@DBE0000071[>>F700000005G<CF:90EG48GJ0000000005G<00EG40000001FIFC00000000000ZEEE<004HU>>UF9005B@>DUF9001GB?""?[5000F?""?""?""?""?""?""?""?""?""?F008JJJJJJJJJJ4007G@RDH100GUUB400"	\
                "8@OOO?108@OOO?100;M]TOOOOME0003[YOOOOOOW:000008@OOO?@OOO?10000000000;XOOXG000008@OOO?10:MOOK=700000:MOOOOOOOOLE08@OOO?SOOOOOOO]UC4DOOO@4HXV_VD8000HQMQS>;0000=SOOOOOSI08@OOOKWTOOWF01?OOOOOODD]OOQH08@OOO?@OOOOOOW:00EMOOSIQOOQF03[SOOXHROOT>5000000<POO]=9000ISOOOOOOQI>OOOP<00ISOOL=SOO@?TOO@88@OOOYYTOOWF06?TOOD5;MO_OWI700ISOOL=SOO@;WO_OWI708@OOO?1000004BXYXKOVS>A0005BHDNK;BG00000EUQBIM>23[Q[FX>F7000000EUQB93[Q[<00007IWSM;0001EGF8000ILLLD20:WTNN]PJ00:WONN]PJ00AM]OOME000ISOOOOOOOO]=90HMMMMMMMMMMI00FLT_OW;03[YTLE008@VdO?108@VdO?104DTfa__e`ME0003[YdcbbdaW:000008@VdO?@VdO?10000000002DTeeT@500008@VdO?16U]feVPE000006U]f`__`cSBC08@VdO?RNf___c_MF09=Sc`MADOd^VD800<>O_Ob][3000=Sfa___SI08@VdO]Yc_X=601?O___dND]b`QB408@VdO?@Vd`___W:00EM^fSWQbbQF00J>OeO>]c_W;0000000<P_e]=9000ISffbbbeVRSb`XH700FXaaXUTd_RPNf]B7"	\
                "8@VdOLOc_X=600HKb^WF@TeeeV><00FXaaXUTd_RDOeeeV><08@VdO?100000;X_aX`ebb]?400;QQDNKPSD<000:DS]BWNP23UTK?QYR;10000:DS]B93UTK?J0004DOXT?54FULQW=309=]`_R<0:WYQ]cKI00:WSQ]cK;00AWT]`ME000IS^^^^^^^^]=90IS^^^^^^^^]=93[]OQTT@83[Y`LE008@VdO?I;E@VdO?10<P_f]PSf`ME0003[YdNXKcaWI;E3008@VdO?@VdO?HF50000000HQa__^Q;00008@VdO?FFM_^ObK=300000:M_NLM_NRJ008@VdO?=Sc]PSc]U600:M`cKIW^bKLU109;P]`e_W[E00<=Sf`LPPP;08@VdO]f_Q=4003BPPPTdNSa`K[5008@VdO?@VdOPPP>E00EM^fSTQbbQF001HX`bS`bS[500009<GIP_e]=9000ISf`XXX_c]_c]U4000<P_fS>N`aXW^bQG08@VdOYc^]DE000EL_fS=M`cYNaW:00<P_fS>N`aXW^cYNaW:08@VdO?IIG708?TeVSf`Kb`W:0CUY^]NKVNXF09G>]a]Q_NP23UTa]Q`OMI809G>]a]B93UTa]>F004DTKTXD@W]_OKUZ09=]e_R<0J>?IQbKI002[WQ]`XG00<@?]`ME000HMMMMMMMMMMI00ISOOOOOOOa]=9Z?OKHX_>23[Y`LE008@VdOSSQP@VdO?102>Nf]PSf`ME0003UTdN>KcaSSQRI108@VdOSSVdOSKX@A000"	\
                "00<DOcSScTD20008@VdOXXX]fYQaNRJ000001[]cS]cS=3008@VdOXXXN`WN_MF0004@TdT?SfNPE500<>S>Ofb_S=0CUXSf`XXXH308@VdNb`QB400008<<UTd_a^KU20008@VdOSSVdO?<<2000<R_dVTSbbQF00AMKScaeTKQH0009G>QSS_e]=9000=Sf`LFBKbaeVPA00008?TdTW^]aSKc`ME08@VdNb`QSSI000ZUYeODKc_M]_XF008?TdTW^]aSKc`M]_XF08@VdOSSS>50<R_f]]e_M^cK;00F?XOb`]L=505>T^SS^OMIC0F>]aYQ_NW:05>T^SD:00F>]aYD400GPXQYTT`TXDI<003[]eNP<00<=WNVLA000FQV`YLF0004B]`ME000IWWWWWWWWWWI00F?""?""?""?""?""?""?M^]=9CUYTM]OD43[Y`LE008@VdcbcbOLVdO?103=K^a__e`ME0001UTdNDKcebcbNMF08@Vdcbbcgcbc^]?40009IQa_SS_aQH0008@Vdbbbbb^SSOcSBC00008=M__`NLI1008@VdbbbbbfYc]D:0000IKc_MVe]U1009IKbOaNS_M:01UTbdfbbKH008@VddcbS[80000003UTddcc]?50008@VdcbbcdO?1000006U]abcbebQF00AW^cdggccKH000EPOacbfe]=9009=]e_R<<DTdaXH700009=SfNKbQ_YYeVD408@VddcbS[=E0000;Qb^WTeTUPP>J009=SfNKbQ_YYeODPP>J08@VdcbbN>50EM`f"	\
                "S]e_M^fSI07IL]__NOQ@E05>NTL]_Q[<008IMTVDSaW:05>NTLH1008IMTVD400JU>LK__OKPBA1004DVaK=304BXNOM=J00:D?DXaS=00C;B]`MIE00ISaaaaaaaa]=900111111JM^]=90GWN`_QI93[YTLE008@VdOQK`cKVdO?100<?YeVVe`ME003EDOdO?KcaQK_fS=98@VdOQQVdOQ]c`ME000<>OfaccafO><008@VdOSSYeabfacNLA0007IXNcdecVW;008@VdOSSSfedeaYD<000JM_cK^aQG0009=WTbf]PW[101USSf`SSKH008@Vd^]e`WH9000003UTdaSa^QI3008@VdOQQVdO?1000000:DXQTQbbQF00AMKScbdTKKH000GQb^KQ_e]=909<[YeN>2BKabeVPA00000GQb^]^MONVe][308@Vd^Sb`WH90000<PNcS`bQH2220000GQb^]^MONVe]B222008@VdOQQQ>50EL`e]]e_XacKH0CU]VQNK]`WF04?K`OWV`K?47=WV_KYaYPJ04?K`OWI07=WV_KU606U]TVSMMSNNSMB30JM`TD<00FX_N]]S=00;KKQ]cSI00<LS]`SS=00ISTTTTTTTT]=900000000JMO]=90ZILXMB500GUU=4008@VdO?>NfSVdO?100;X^aQSf`ME00FRK^e][KcaWDVe]B98@VdO?@VdO?Qb^WA007IKbNX_NXNaKI908@VdO?W^_XO^XTcS[100<>VeNNONeTD508@VdO?Q^bYbYabKI"	\
                "0006UYeOfN><00008=Kb^cVP<000F=Sf`LII<008@VdO]OeOPA000003UTdNDYfNLF008@VdO?@VdO?10000000<FUTQbbQF001;W`cS^bS[2000;Kc_LP_e]=90J>Q_dT?GM_c]`c]U40000EL_cVN>]^`bK;008@VdO>OeOPA0000C[]eVcNRE0000000EL_cVN>]^`bK;000008@VdO?GG:004DOdN]e_Yf^WF00FMPDNKUXU400<[QN]LT_P23UTNXQ^SD:900<[QN]B93UTNX[<005>T>O@:A=PQYSU10FWQW;000FXOOOOS=00;KONN]PA00<LOOOOS=00:UUUUUUUUUUF0000000002U?""?F0003EFJ100003370008@VdNOVcaQVdO?10<DTeTDSf`ME00;Kbe_W;KcaOVbbKI08@VdO?@VdNONeNP<00EPVbK>_N>KbV>E08@VdO@Tf]?O^MX^_M:00FXa^WNOQa^WA08@VdO?YeOXbKYeT?6000HQbdf]B300004DYaSYSBC0000=Sf`L<00008@VdO]X_c]U500003UTdNDMNfYD<08@VdO?@VdO?1000000009H?QbbQF00E>OeOP]c_W;000FW^fNO`e]=90FXad^QIDTfNLSc`XH90004DOddTUKcg^MA008@VdO?W_c]U50000GXadeYU100000004DOddTUKcg`MA000008@VdO?100009IK^f`dabcYD5001FJD]K;FE00004=W]B>]P23UTMIQKUE000004=W]B93UTMI40001["	\
                "KOQ=703<H[=E001FFF6000E?@@@@@F00<B@>DUF9004[@@@@@F000CCCCCCCCCC0000000000011100000000000000000008@V____NS@V_O?10IQ__XIS__ME00;K_TXB;K____NSDE08@V_O?@V____OQ=307=K_V>>_N>>V_K=98@V_OQ__M?O_M@T_]U10;K_N>NOL__QF08@V_O?O_]X_KK_N>5000ERN__WF000000F@QPDPJ00000=S__L<00008@V_OKWK__QI00003UT_ND=Q__KB78@V_O?@V_O?10000000007FQ__QF01[S__QHLN_T><004UKV____]=90FX_OQUBK__QIPO_OPE0007B]__SIW__N>5008@V_O?BK__QI0000<>N__Q;000000007B]__SIW__N>5000008@V_O?100000<UKO___N]PF000007;==<00000001H>HA@U49I>;A>B200000001H>H09I>;C00000<=?=50000079000000000000Z6666600007854C00000166666000000000000000000000000000000000000000000000001[RRRRR>B[RRRB30HRRR?;RRRD<00FPP?;4FPRRRR>[J001[RRRB[RRRRP@I4009=RRRI?RR?IRRRI91[RRRRRR[[RRDGRRR[10FPRRURR@RRP:01[RRR[RRR>RPPRRU80001[RRR@200000002I;2E900000;RRRD200001[RRRB;DRRRI00007=RRRU5?R"	\
                "RR=71[RRRB[RRRBC0000000000:PRR>:01[RRRU5IRRRD<000<=DRRRRRH00J>P@I5=RRR@2HRRR>E0000GRRRPFDRRRB3001[RRRB<DRRRI00003BRRRD<000000000GRRRPFDRRRB3000001[RRRB30000002IDRRR>[:90000000990000000007530440015905100000000075300159000000071900000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004<<<<<534<<<8001<<<5C<<<20007<<8007<<<<<5C00004<<<84<<<<<400000Z<<<15<<51<<<Z004<<<<<<44<<23<<<4003<<<4<<5<<<9004<<<4<<<<<<<<<4000004<<<50000000000000000000C<<<20000004<<<802<<<1000006<<<405<<<8004<<<84<<<4000000000009<<<<9004<<<501<<<20000074<<<<<1000<<4008<<<50C<<<<000003<<<<72<<<800004<<<802<<<1000008<<<200000000003<<<<72<<<800000004<<<80000000094<<<2C00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "0000000000000000000000000000000000007888000007CC00000CCC7000000000003CCC000007CC0CC300007CCCCCC700000000000000CCC3CCC000007ZC000000CC3CC300000009CC700000008860000999000000000000000000000007CCCCCC33CC0000000000088870000000000000030000008889000000000004<<900000000000078880000000000099900000000099900000000000097000000000000000000000031111111C0000000000000000000000CCC00000CCCCCC00000CCCCCCCC00001ZZZZ10000000000000000000000000900000000000000000000000000000000000000000000000000000000000000000000000031111111C00000000000000000000000000000000000000009;DDDF0001=UUF000AUUU=807<EE<0005BUUUE0001=UUFUU[5006=UUUUUU=607<EE<EEE400;UUU[UUU<000JB?[F3000GUUBUU[5000001IUUIZ00000FDDU<004=B=4000000004:E90000000001=UUUUUUB[UUJ00ZEEE<00A@DDH700001:IIF297:[F0000FDDDG008EEE<000C[L>F0003FGF80009;DDDF008EEE<002=BBF000"	\
                "00002=BBF0000000005G=[=F50000000000000000004BUUUUUUUU<005EEE4<EEE1<EE<7000FUUU:000FUUUUUU:000FUUUUUUUU:00<U?""?""?""?U<00EHHGHH:005:::::<0006FIBIA700000004;HHHHHHHH;4004;HHHHHH<004;HHHHHHHHHHHHHE03GHHHHHHHHHHHHHHH204BUUUUUUUU<000000000000000000000000000000000000000E>OVXH000FMT]B900HKTTR<0:PLL>E00JMTTQG000FMT][STMA00<PTTKKTTP<0:PLL>LLLB3CUTTTMTTQF00A>SO]LG003[YTL]TWA00007;MTTWH30000ISVMJ00AW]WA0005EEEEUWPF0000000002>TTTTSTXQTK;00ILLLD20GWVVPA0002=MKSXD==P]=900<DYVK=00IMMMD20CF[SY?400ELQWF000E>OVXH00IMMMD20FQ]]U3000000FQ]]U30000000EUX]YSWDIJ5810000000000000<LTTTTTTTW:08ULLL=>LLL;RLLP:001U]TKI001U]TK]TKI001U]TK]T]TKI00FXOOOOW:07[KKLKKD87BWWWWW?63;UMS]SRG0000000FXKKKKKKKKXF00FXKKKKKKI00FXKKKKKKKKKKKKK[7<RKKKKKKKKKKKKKKKH0<LTTTTTTTW:000000000000000000000000000000000000004US^KB4004?YbQG000HKf_R<0FQ``W"	\
                "A00JM^bQG004?YbQM_V>500<P_fKKf_P<0FQ``WT`TUCCUTdTM^bQF0C[]bdfTD503[Y`L]^WA0000FLOffVMG9007B]N>500;Q`LE00J@MMMMX^TU1000000002>SOSSSTYOVK;09=]`_R<06=Q`]U500IKNccaOSSV]=901=KaSU209=]^_R<0AMMW_L<00EM^QG004US^KB409=]^_R<0FXYSU3000000FQSTU30000004US^OTN^OKM>@[5000000000000<LOOOOOOOW:04>N`TUM``SIQ``QF00JL_Y@<00JL_YL_Y@<00JL_YL_Y_Y@<00FXbeeaW:07[Y`LNVD87[YaaaVD82P]_OYVT@4000000FXbffffffbXF00FXbffffSI00FXbfffffffffffY[7<L`fffffffffffffKH0<LOOOOOOOW:0000000000000000000000000000000000000EHX`KU:000;X^_LE000IKf_R<0FQb^WA00JM`bQG00;X^_LYf][300<P_cKKf_P<0FQb^WTdTUCCUTeTM`bQG05>Ndgg^MJ03[Y`L]^WA000A>YcTTbT>:001UTTU109=]N>507BS`````aT?Z000000007G?OS=STOTVK;09=]e_R<004[K^QI90ISOKKT^ccNK=90FMN]@<009=]e_R<0AW`^O>200EM`K?60HX`KU2009=]e_R<0JM_SU3000000FQTS=9000000GW^SDUPQT_`NOME000000000000<LTTTTTTTW:04>NdTUM^f"	\
                "SIQbbQF07=SNLF007=SNLSNLF007=SNLSNSNLF000FX]]]]W:07[]]L]]D87[QQQQQ@82PYQDUMOR<000000FXYYYYYYYYXF00FXYYYYYSI00FXYYYYYYYYYYYYY[7<LYYYYYYYYYYYYYYKH04[@@@@@@@U<0000000000000000000000000000000000007[KKWKKD800=]eN>4001UYcS[10HKbNLJ004U]f]B70=]eNP_fSI000FXaVPYbSB10HKbNLOc]UC0;W`_P]f]B75>Ndgg^MJ03[Y`L]^WA00<@]a]>>]a]DE04DVS=00Z?TO@803[]bKWWWWPG00000000001?OS=STQXVK;09=]__R<0002UWWB70ISPII@WKKM=200;WWDJ0009=]__R<0EDMW>;9004?QQ@80=WW?<0009=]__R<01BXSU3000000FQK>J0000000HWW@<1<H?LXKSME000000000000<LOOOOOOOW:04>N_TUM__SIQ__QF0CUXXI300CUXXUXXI300CUXXUXXXXI30005=====I400:==H==G90<GGGGGJ08BU;54[?[4000000<B[[[[[[[[B<00<B[[[[[[J00<B[[[[[[[[[[[[[F06=[[[[[[[[[[[[[[[E0016666666Z00000000000000000000000000000000000007[Y`LNVD800=]cN>4002>NVPJ03[YbK=C000:MNOD40=]cNP_cSI009=SaQ>NVPE03[YbKM`NL:004U]aXMNOD41U]cdfOD5"	\
                "03[YTL]TWA09=KTS@:J?KTSB74?MMG005DQQUC00FRQ>::::E700000000001?OS=STUDVK;00HRRR@500002AA500A[:904FHHA10006AAE00000HPPP?5005EA<70000EGGA004AA<00000HPPP?5004GI:00000005IHE000000004::E00008EFHIHZ0000000000004[@@@@@@@U<08URRR=DRRR;PRR>:00<FF40000<FF<FF40000<FF<FFFF400000000000000000000000000000000C1000CZC00000000777777777700007777777000077777777777777000977777777777777700000000000000000000000000000000000000000000000007[]]L]]D800=QQQD400EMQQI702>NO><0000CBQQP<0=QQQ>QQQH00CUQQDMQQI702>NO>Q_K=3000:MQQBQQP<0:P]NTM;000GUU=UUB500FUU[<00<BUUG00<EEC000:GGE0007AGE00000000000000007IDDGDDIBDD:001<<<50000000000007000000000000000000000C2224000000000000000000000000000C222400000000000000000000000000000000000000000000000000000016666666Z0004<<<62<<<C<<<<9000000000000000000000000000000000000000000000000000000000"	\
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000:==H==G900<GGGA0003FGG5005?PPH0000002GGF70<GGG:GGG5000EGGAFGG4005?PPHPPD<00003FGG<GG:909:UD?GC00003373330000CC300003CC00000000000000000000000000000000000000344944314400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000042230000000000000000000000000000000000042232250000000000000000014Z0000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"	\
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

        wOut=hOut=512;
        const unsigned num_indices = sizeof(indices)/sizeof(indices[0])-1;  // or strlen(indices);
        SDF_ASSERT(wOut*hOut==num_indices);
        for (unsigned i=0;i<num_indices;i++)	{
            unsigned char& c=indices[i];
            if (c>=']') c-='1';
            else c-='0';
        }

        rawRGBAOut.resize(4*sizeof(indices)/sizeof(indices[0]));
        int idx;unsigned int pal;const unsigned char *pPal;unsigned char *pRGBA=(unsigned char *) &rawRGBAOut[0];
        const int maxPaletteIdx = sizeof(palette)/sizeof(palette[0]);
        /*unsigned i = 0;
        for (int y=0;y<hOut;y++)   {
            //pRGBA = &rawRGBAOut[wOut*(hOut-1-y)*4];   // Flipping
            pRGBA = &rawRGBAOut[wOut*y*4];
            for (int x=0;x<wOut;x++)   {
            idx = indices[i++];
        */
        for (unsigned i=0,isz=wOut*hOut;i<isz;i++)    {
            idx = indices[i];
            SDF_ASSERT(idx<maxPaletteIdx);
            pal = palette[idx];

            pPal=(unsigned char*) &pal;

            // debug line:
            //printf("[");for (int c=0;c<4;c++) printf("%X",pPal[c]);printf("]");if (i%wOut==(wOut-1)) {printf("\n");fflush(stdout);}
#               if (!defined(SDF_BIG_ENDIAN))
            for (int c=0;c<4;c++) *pRGBA++=*pPal++;     // This works only on little-endian machines
#               else
            //for (int c=3;c>=0;--c) *pRGBA++=pPal[c];  // This works only on big-endian machines (maybe)
#               endif
            //*pRGBA++=255;*pRGBA++=255;*pRGBA++=0;*pRGBA++=255;    // Well, something is visible with this on!
            //}
        }
    }

    return true;

}
#endif //SDF_NO_EMBEDDED_FONT

#ifndef SDF_NO_NAMESPACE
} // namespace
#endif

#endif //SDF_IMPLEMENTATION_H
#endif //SDF_IMPLEMENTATION

