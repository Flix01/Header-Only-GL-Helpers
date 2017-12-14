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

const int indices[512*512] = {	// [width x height]: each points to a palette entry
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,4,2,4,0,5,2,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,7,8,
8,8,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,11,11,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,3,4,12,2,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,18,20,20,18,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,10,10,10,12,
0,0,19,22,22,22,4,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,0,0,21,15,20,20,22,0,0,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,0,21,10,10,10,5,0,0,0,0,0,0,0,0,0,0,3,12,12,2,0,0,0,0,0,12,10,10,10,12,0,0,0,0,12,10,10,10,12,12,10,10,10,12,0,0,0,0,21,15,20,20,22,0,0,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,12,10,10,10,12,0,0,0,0,
0,0,7,21,22,26,19,0,0,0,0,0,0,0,0,0,21,15,20,20,22,0,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,0,0,9,11,20,20,20,22,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,0,0,0,1,26,11,11,22,4,0,0,0,0,0,3,17,10,10,10,10,10,10,10,2,0,0,3,17,10,10,10,10,10,10,10,2,0,0,0,0,0,0,5,10,10,10,21,0,0,0,0,0,0,0,0,0,0,2,17,17,12,8,0,0,0,0,0,12,10,10,10,12,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,0,0,0,0,0,7,26,10,10,10,6,
0,0,0,0,0,0,0,0,4,10,10,10,21,0,0,0,0,0,0,0,0,0,0,12,10,10,10,12,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,1,12,12,6,5,26,12,9,0,0,0,3,10,10,10,8,0,0,0,0,0,0,0,0,0,0,0,0,0,5,20,27,27,13,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,13,14,14,24,16,28,20,21,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,0,0,0,0,0,0,0,0,0,9,7,0,4,37,27,38,38,35,16,2,
0,7,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,37,39,39,39,37,1,0,10,29,40,40,24,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,42,37,39,39,39,37,19,0,0,0,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,0,4,20,39,39,39,18,7,0,0,0,0,0,0,0,0,0,23,28,28,20,2,0,0,0,42,37,39,39,39,37,19,0,0,42,37,39,39,39,37,37,39,39,39,37,19,0,0,
1,18,35,38,41,16,2,0,0,42,37,39,39,39,37,19,0,0,0,0,0,0,42,37,39,39,39,37,19,0,0,0,0,7,11,32,40,29,13,6,0,0,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,0,0,21,14,31,38,40,24,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,0,0,2,18,29,33,27,39,15,17,0,0,0,0,10,29,39,39,39,39,39,39,39,43,3,0,10,29,39,39,39,39,39,39,39,43,3,0,0,0,0,9,13,39,39,39,20,5,0,0,0,0,0,0,0,7,23,14,39,39,28,15,21,0,0,0,
1,37,39,39,39,37,1,42,37,39,39,39,37,19,0,0,0,0,0,0,0,0,0,0,0,0,21,34,39,39,39,11,0,0,0,0,0,0,0,0,25,39,39,39,14,12,0,0,0,0,0,0,0,0,1,37,39,39,39,37,1,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,9,25,34,34,13,20,29,32,10,0,0,0,17,29,39,39,11,0,0,0,0,0,0,0,0,26,13,21,0,0,23,40,45,27,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,43,44,36,15,44,46,39,17,0,0,0,21,14,36,41,29,31,44,16,2,0,0,0,
19,18,27,46,46,30,40,24,19,19,0,0,0,0,0,0,0,0,0,3,22,13,43,25,10,39,47,35,27,46,33,23,24,18,18,22,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,48,31,15,1,0,10,39,48,27,24,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,8,16,38,48,31,15,1,0,0,0,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,0,5,14,30,48,41,43,3,0,0,0,0,0,0,0,0,
12,20,36,30,14,2,0,0,0,8,16,38,48,31,15,1,0,0,8,16,38,48,31,15,16,38,48,31,15,1,0,0,22,29,46,44,45,33,25,7,0,8,16,38,48,31,15,1,0,0,0,0,0,0,8,16,38,48,31,15,1,0,0,0,0,26,34,31,46,30,39,23,0,0,0,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,0,0,4,37,35,45,27,18,4,0,0,0,0,4,37,35,45,27,18,4,0,0,0,0,0,0,4,37,35,45,27,18,4,0,0,0,0,4,43,27,46,49,50,45,44,20,12,0,0,0,22,40,48,48,48,48,48,48,36,37,19,0,22,40,48,48,48,48,48,48,
36,37,19,0,0,0,0,9,18,44,48,46,14,2,0,0,0,0,0,0,0,17,28,31,45,45,46,33,23,0,0,0,6,16,31,48,31,15,1,8,16,38,48,31,15,1,0,0,0,0,0,0,0,0,0,0,0,0,26,29,45,48,27,24,0,0,0,0,0,0,0,0,25,35,48,47,28,21,0,0,0,0,0,0,0,0,6,16,31,48,31,15,1,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,9,13,35,38,14,35,46,40,22,0,0,0,10,39,48,27,24,0,0,0,0,0,0,0,21,16,35,43,1,4,15,41,45,39,10,0,0,8,17,12,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,22,40,46,29,30,33,14,21,0,0,0,10,28,39,14,37,28,39,14,12,0,0,11,37,37,37,28,39,32,37,37,37,37,11,9,0,0,0,0,0,0,0,11,29,35,41,27,20,33,35,15,43,35,27,15,27,41,44,29,24,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,45,31,15,1,0,10,39,48,27,24,0,0,3,42,42,22,29,30,44,16,12,42,1,0,0,8,16,38,51,31,15,5,12,4,0,0,0,0,0,3,42,43,44,44,28,44,44,20,8,1,0,0,0,0,2,16,41,36,14,44,38,32,26,1,0,0,0,
0,0,0,22,34,44,31,41,39,24,1,0,0,0,0,0,0,5,14,30,45,41,43,42,1,0,0,0,0,0,0,1,18,27,48,44,43,42,1,0,0,8,16,38,45,31,15,1,0,0,8,16,38,45,31,15,16,38,45,31,15,1,0,5,16,41,36,14,35,38,32,26,0,8,16,38,51,31,15,1,42,42,42,42,0,0,8,16,38,51,31,15,1,0,0,0,19,24,27,31,28,41,44,18,19,19,0,0,0,3,2,16,41,36,14,44,38,32,26,1,0,0,0,21,24,40,47,27,37,10,0,0,0,7,42,24,40,47,27,37,2,0,0,0,0,0,0,21,24,40,47,27,37,10,0,0,0,0,0,23,
39,47,50,31,31,50,48,33,11,0,0,0,22,40,30,30,30,30,47,50,36,37,19,0,22,40,30,30,46,49,47,30,36,37,19,0,0,0,0,9,18,44,52,46,14,2,0,0,0,0,0,0,9,13,35,50,50,31,31,33,23,0,0,0,6,16,31,45,31,15,1,8,16,38,51,31,15,5,12,4,0,0,0,0,0,0,0,0,19,2,26,29,45,53,27,24,12,8,0,0,0,0,0,7,25,35,53,47,28,21,42,42,0,0,0,0,0,0,6,16,31,45,31,15,1,0,3,42,22,34,44,31,41,39,24,42,1,0,0,0,0,10,29,47,39,30,27,20,26,0,0,0,10,39,48,27,24,0,0,0,
0,0,0,0,25,35,46,29,10,2,32,46,46,32,12,12,24,15,29,15,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,25,14,38,27,30,32,11,5,0,0,0,3,26,37,36,36,36,28,21,0,0,19,37,36,36,36,14,17,13,27,36,36,36,15,1,0,0,0,0,0,0,4,20,36,31,41,46,27,13,25,10,17,25,25,27,46,41,36,38,14,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,37,29,29,29,37,1,0,10,39,48,27,24,0,19,13,15,15,15,39,39,20,15,15,15,37,2,0,8,16,38,51,31,15,14,32,16,11,
1,0,0,19,13,15,15,15,13,24,13,15,15,15,37,2,0,0,0,12,14,39,28,14,14,39,29,15,37,5,0,0,0,0,10,37,14,14,15,15,15,15,37,5,0,0,0,0,10,37,14,29,29,29,15,15,37,5,0,0,0,0,10,37,14,33,33,33,15,15,37,5,0,42,37,29,29,29,43,19,0,0,42,37,29,29,29,43,37,29,29,29,43,19,0,12,14,39,28,15,14,39,29,22,0,8,16,38,51,31,15,10,15,15,15,15,23,0,8,16,38,51,31,15,1,0,0,11,37,37,27,31,34,41,44,37,37,37,26,0,19,13,15,14,39,28,22,14,39,29,15,37,2,0,7,43,27,27,39,27,
27,20,8,0,3,25,15,15,39,39,14,32,20,24,1,0,0,0,7,43,27,27,39,27,27,20,8,0,0,0,7,18,44,53,30,32,14,30,53,35,13,0,0,0,21,16,20,20,40,31,45,44,28,25,9,0,21,16,40,30,45,27,28,14,20,25,9,0,0,0,12,13,16,44,52,46,14,43,10,7,0,0,0,0,3,37,36,51,46,34,20,29,22,0,0,0,1,37,29,29,29,37,1,8,16,38,51,31,15,14,32,16,11,1,0,0,0,0,0,21,43,14,32,29,45,53,27,14,32,15,22,7,0,0,7,25,15,35,53,47,28,15,15,15,23,0,0,0,0,0,1,37,29,29,29,37,1,19,13,15,
15,15,43,15,37,15,15,15,37,2,0,0,3,24,16,36,35,46,28,11,42,0,0,0,10,39,48,27,24,0,0,0,0,0,0,0,22,40,48,44,15,12,32,35,35,32,12,37,27,36,47,27,37,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,18,27,36,38,30,30,41,33,43,5,0,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,30,14,21,14,38,51,51,31,15,1,0,0,0,0,0,0,21,28,36,28,43,27,36,37,19,0,0,7,18,44,35,43,32,36,39,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,31,31,15,
1,0,10,39,48,27,24,0,4,16,36,31,31,20,17,13,35,31,31,29,26,0,8,16,38,51,31,27,30,46,36,39,24,7,0,4,16,36,31,31,20,5,13,35,31,31,29,26,0,0,0,10,14,44,30,30,44,35,31,31,29,26,0,0,0,10,14,44,30,30,44,35,31,31,29,26,0,0,0,10,14,44,30,30,44,35,31,31,29,26,0,0,0,10,14,44,30,30,44,35,31,31,29,26,0,8,16,31,31,31,15,1,0,0,8,16,31,31,31,15,16,31,31,31,15,1,0,0,21,16,31,31,31,15,26,3,0,8,16,38,51,31,15,20,41,31,31,27,13,0,8,16,38,51,31,15,1,0,19,37,
36,36,36,31,46,30,27,36,36,27,11,0,4,16,36,31,31,20,5,13,35,31,31,29,26,0,7,43,41,47,28,30,38,20,8,0,8,16,31,31,31,27,38,46,31,39,11,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,42,15,31,51,31,16,20,38,53,35,13,0,0,0,0,4,43,27,46,46,33,37,17,3,0,0,0,13,35,53,30,20,22,17,5,3,0,0,0,12,16,27,31,46,51,45,38,44,28,23,0,0,0,0,19,37,36,51,38,20,12,26,19,0,0,0,6,16,31,31,31,15,1,8,16,38,51,31,27,30,46,36,39,24,7,0,0,0,21,20,35,30,46,41,45,53,35,30,
46,41,28,22,0,0,1,15,31,31,52,45,31,31,31,44,13,9,0,0,0,0,6,16,31,31,31,15,1,4,16,36,31,31,20,5,13,35,31,31,29,26,0,0,4,14,36,30,47,46,36,39,24,9,0,0,10,39,48,27,24,0,0,0,0,0,0,0,12,14,30,45,27,13,11,25,25,11,25,27,45,48,36,29,13,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,18,41,50,48,48,53,52,47,33,11,0,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,30,14,25,27,49,54,51,31,15,1,0,0,0,0,0,0,4,18,37,13,21,37,37,11,0,0,0,0,23,37,
37,10,13,37,43,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,1,0,10,39,48,27,24,0,0,25,27,49,45,39,22,20,31,51,36,15,8,0,8,16,38,51,31,45,47,50,52,38,32,21,0,0,25,27,49,45,39,22,20,31,51,36,15,8,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,
6,16,31,51,31,15,1,0,0,8,16,38,51,31,16,44,49,45,27,37,12,0,8,16,38,51,31,15,1,0,19,37,36,51,47,28,40,29,27,50,50,27,11,0,0,25,27,49,45,39,22,20,31,51,36,15,8,0,7,43,44,44,28,44,44,20,8,0,8,16,38,51,31,45,45,53,52,36,20,4,0,0,3,43,44,44,28,44,44,20,8,0,0,0,8,16,38,51,31,39,27,48,30,29,22,0,0,0,0,24,40,47,48,27,43,5,0,0,0,0,0,11,33,46,48,44,40,39,18,7,0,0,9,13,27,48,52,48,51,50,50,53,36,20,5,0,0,0,19,37,36,51,38,28,28,37,6,0,0,0,
6,16,31,51,31,15,1,8,16,38,51,31,45,47,50,52,38,32,21,0,0,3,18,27,48,52,45,48,52,51,50,47,50,53,36,20,5,0,1,15,31,46,52,48,46,46,46,44,13,9,0,0,0,0,6,16,31,51,31,15,1,0,25,27,49,45,39,22,20,31,51,36,15,8,0,0,4,14,30,30,38,48,52,31,20,4,0,0,10,39,48,27,24,0,0,0,0,0,0,0,19,37,35,36,32,22,0,0,0,0,10,14,36,35,20,23,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,18,41,27,40,39,44,50,52,44,18,7,0,0,3,37,36,51,47,28,21,0,0,19,37,
36,51,30,14,14,31,50,45,51,31,15,1,0,0,0,0,0,0,2,17,17,5,0,1,1,9,0,0,0,0,0,1,1,0,5,17,17,12,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,1,0,10,39,48,27,24,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,8,16,38,51,48,27,29,44,50,49,27,24,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,0,26,29,47,51,31,39,40,30,51,45,29,26,0,0,26,29,47,51,31,39,40,30,51,45,29,26,0,0,26,29,47,51,31,39,40,30,51,45,29,26,0,0,26,29,47,51,
31,39,40,30,51,45,29,26,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,6,16,31,51,31,15,1,0,0,8,16,38,51,31,35,48,45,27,37,12,0,0,8,16,38,51,31,15,1,0,19,37,36,51,47,28,22,11,27,50,50,27,11,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,0,10,13,15,15,15,13,23,9,0,8,16,38,51,49,35,39,36,52,47,28,21,0,1,13,15,15,15,24,15,15,15,22,0,0,0,8,16,38,51,31,44,50,47,44,29,24,3,0,0,2,14,31,52,36,20,12,0,0,0,0,0,0,23,34,35,38,53,48,41,
43,3,0,0,5,20,31,51,46,44,52,46,41,53,45,39,22,0,0,0,19,37,36,51,45,47,30,20,4,0,0,0,6,16,31,51,31,15,1,8,16,38,51,48,27,29,44,50,49,27,24,0,0,12,14,30,51,30,39,33,45,53,44,29,36,52,45,39,22,0,3,18,32,35,53,47,28,28,40,29,37,21,0,0,0,0,6,16,31,51,31,15,1,0,21,28,46,53,35,18,29,47,49,27,24,0,0,0,4,20,39,14,20,35,53,30,14,5,0,0,10,39,31,27,24,0,0,0,0,0,3,0,0,21,16,28,23,9,0,0,0,0,0,22,32,20,21,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,11,20,25,22,22,29,45,52,41,43,7,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,30,14,33,48,30,30,51,31,15,1,0,0,0,0,9,23,14,39,39,20,22,0,0,0,0,0,0,0,0,0,0,10,16,29,39,14,24,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,1,0,10,39,48,27,24,0,0,1,37,44,53,38,14,27,50,46,28,21,0,0,8,16,38,51,38,14,17,29,47,52,41,43,7,0,1,37,44,53,38,14,27,50,46,28,21,0,0,0,23,33,49,50,27,24,18,44,52,45,29,26,0,0,
23,33,49,50,27,24,18,44,52,45,29,26,0,0,23,33,49,50,27,24,18,44,52,45,29,26,0,0,23,33,49,50,27,24,18,44,52,45,29,26,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,6,16,31,51,31,15,1,0,0,8,16,38,51,31,45,48,35,15,12,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,1,37,44,53,38,14,27,50,46,28,21,0,0,0,5,14,31,31,41,43,3,0,0,8,16,38,51,30,14,25,35,53,45,29,26,0,5,14,31,31,41,43,33,31,36,14,21,0,0,8,16,
38,51,31,44,31,45,53,38,28,10,0,0,22,39,45,50,27,24,0,0,0,0,0,0,12,14,36,50,50,30,36,41,43,3,0,0,21,29,47,53,44,44,52,46,39,48,50,27,25,0,0,0,19,37,36,51,46,30,30,20,4,0,0,0,6,16,31,51,31,15,1,8,16,38,51,38,14,17,29,47,52,41,43,7,0,17,39,45,53,44,18,29,45,53,27,24,40,48,50,27,25,0,0,8,25,35,53,47,27,46,48,47,35,37,4,0,0,0,6,16,31,51,31,15,1,0,1,37,44,53,38,14,27,50,46,28,21,0,0,0,0,26,20,35,35,38,48,44,37,42,0,0,12,37,15,15,10,0,0,0,
0,21,43,25,10,12,12,26,7,0,0,0,0,0,0,9,26,12,12,12,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,17,14,32,29,35,49,48,27,24,0,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,30,14,31,50,35,30,51,31,15,1,0,0,0,0,21,32,36,47,45,36,14,12,0,0,0,0,0,0,0,0,5,20,41,45,47,31,29,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,1,0,10,39,48,27,24,0,0,0,23,40,45,48,40,36,52,41,37,42,0,0,8,16,38,51,31,15,12,32,46,51,
36,37,3,0,0,23,40,45,48,40,36,52,41,37,42,0,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,6,16,31,51,31,15,1,0,0,8,16,38,51,50,51,38,29,23,9,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,0,23,40,45,48,40,36,52,41,37,42,0,0,0,5,14,30,51,41,
43,3,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,5,14,30,51,41,43,20,31,49,35,13,3,0,8,16,38,51,31,15,16,27,49,50,35,13,9,0,11,27,50,48,39,10,0,0,0,0,0,0,23,33,48,50,44,32,15,37,23,0,0,0,17,39,45,50,27,44,52,46,28,47,53,35,13,0,0,0,19,37,36,51,38,20,20,18,1,0,0,0,6,16,31,51,31,15,1,8,16,38,51,31,15,12,32,46,51,36,37,3,0,22,40,48,53,27,24,29,45,53,27,24,29,45,53,35,13,0,0,0,25,35,53,45,46,30,45,51,45,39,22,0,0,0,6,16,31,51,31,15,1,0,0,23,
40,45,48,40,36,52,41,37,42,0,0,0,0,4,20,30,50,53,36,40,13,3,0,0,17,29,39,39,11,0,0,0,0,11,27,27,39,28,14,12,0,0,0,0,0,0,0,0,12,20,28,14,24,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,46,47,49,38,27,16,12,0,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,30,33,48,30,28,30,51,31,15,1,0,0,0,0,11,27,38,29,39,30,33,22,0,0,0,0,0,0,0,0,10,39,46,39,29,36,35,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,
1,0,10,39,48,27,24,0,0,0,2,20,31,53,44,46,49,33,24,0,0,0,8,16,38,51,38,20,21,28,46,52,41,43,3,0,0,2,20,31,53,44,46,49,33,24,0,0,0,0,23,33,49,50,27,24,18,44,53,45,29,26,0,0,23,33,49,50,27,24,18,44,53,45,29,26,0,0,23,33,49,50,27,24,18,44,53,45,29,26,0,0,23,33,49,50,27,24,18,44,53,45,29,26,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,6,16,31,51,31,15,1,0,0,8,16,38,51,31,46,53,36,32,22,0,0,0,8,16,38,51,31,15,1,0,19,37,
36,51,47,28,12,11,27,50,50,27,11,0,0,0,2,20,31,53,44,46,49,33,24,0,0,0,0,5,14,30,51,41,43,3,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,5,14,30,51,41,43,13,44,53,38,14,5,0,8,16,38,51,38,20,21,28,47,51,36,37,3,0,11,27,50,48,39,23,12,5,0,0,0,0,24,27,50,45,39,10,12,5,9,0,0,0,21,29,47,53,35,44,52,46,29,45,53,35,25,0,0,0,19,37,36,51,38,20,4,1,0,0,0,0,6,16,31,51,31,15,1,8,16,38,51,38,20,21,28,46,52,41,43,3,0,10,39,48,53,35,13,29,45,53,27,24,
39,48,53,35,25,0,0,0,25,35,53,53,44,14,33,48,53,35,13,0,0,0,6,16,31,51,31,15,1,0,0,2,20,31,53,44,46,49,33,24,0,0,0,0,0,4,20,33,27,36,53,30,32,12,0,0,10,39,48,27,24,0,0,0,0,11,27,48,45,47,29,26,0,0,0,0,0,0,0,0,21,29,47,31,33,16,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,46,47,49,50,30,40,24,0,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,30,36,50,35,20,30,51,31,15,1,0,0,0,0,11,39,39,13,43,39,29,22,0,0,0,0,0,0,0,0,
10,28,39,37,25,39,39,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,1,0,10,39,48,27,24,0,0,0,9,13,35,50,46,49,46,34,21,0,0,0,8,16,38,51,47,40,20,27,49,50,35,25,0,0,0,9,13,35,50,46,49,46,34,21,0,0,0,0,26,29,47,51,31,29,39,30,51,45,29,26,0,0,26,29,47,51,31,29,39,30,51,45,29,26,0,0,26,29,47,51,31,29,39,30,51,45,29,26,0,0,26,29,47,51,31,29,39,30,51,45,29,26,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,
6,16,31,51,31,15,1,0,0,8,16,38,51,31,27,45,50,41,14,17,0,0,8,16,38,51,31,15,1,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,0,9,13,35,50,46,49,46,34,21,0,0,0,0,5,14,30,51,41,43,3,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,5,14,30,51,41,43,24,27,53,47,28,21,0,8,16,38,51,47,40,20,33,48,52,41,43,3,0,22,39,45,53,36,39,28,20,11,19,0,0,23,33,49,50,44,29,28,14,11,19,0,0,5,14,38,51,30,44,52,46,44,50,48,40,22,0,0,0,19,37,36,51,38,20,8,0,0,0,0,0,
6,16,31,51,31,15,1,8,16,38,51,47,40,20,27,49,50,35,25,0,0,12,34,46,51,31,32,39,45,53,27,20,44,53,48,33,23,0,0,0,25,35,53,45,39,10,28,47,52,41,43,3,0,0,6,16,31,51,31,15,1,0,0,9,13,35,50,46,49,46,34,21,0,0,0,0,0,21,13,43,43,40,48,45,29,26,0,0,10,39,48,27,24,0,0,0,0,10,14,27,30,45,29,26,0,0,0,0,0,0,0,0,21,29,47,48,45,27,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,14,32,28,27,47,51,36,15,8,0,0,3,37,36,51,47,28,21,0,0,19,37,
36,51,46,45,46,29,20,30,51,31,15,23,23,5,0,0,6,17,17,5,2,17,17,1,0,0,0,0,0,0,0,0,3,26,17,12,4,17,17,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,1,0,10,39,48,27,24,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,8,16,38,51,30,47,38,45,51,46,29,10,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,0,8,37,44,50,
53,45,45,44,53,45,29,26,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,6,16,31,51,31,15,1,0,0,8,16,38,51,31,15,35,49,49,44,16,21,0,8,16,38,51,31,15,1,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,0,5,14,30,51,41,43,3,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,5,14,30,51,41,43,24,27,53,47,29,21,0,8,16,38,51,30,47,38,47,51,45,33,11,0,0,5,20,41,50,53,45,47,31,29,22,0,0,21,32,31,53,50,45,47,31,
29,22,0,0,7,13,27,48,52,45,51,49,50,53,36,20,2,0,0,0,19,37,36,51,38,20,8,0,0,0,0,0,6,16,31,51,31,15,1,8,16,38,51,30,47,38,45,51,46,29,10,0,0,1,37,44,50,50,30,46,52,51,45,38,48,52,38,14,12,0,0,0,25,35,53,47,28,21,32,46,51,41,43,3,0,0,6,16,31,51,31,15,1,0,0,0,17,29,46,54,52,41,37,1,0,0,0,0,2,15,35,41,41,30,52,30,32,12,0,0,10,39,48,27,24,0,0,0,0,0,12,13,32,29,32,21,0,0,0,0,0,0,0,0,12,14,29,40,35,27,24,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,2,17,10,10,20,30,51,30,14,4,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,51,53,44,43,20,30,51,31,33,33,33,24,0,0,0,0,0,1,23,37,15,18,17,0,0,0,0,0,0,21,13,37,37,11,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,51,31,15,1,0,10,39,48,27,24,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,8,16,38,51,31,44,45,48,46,33,18,1,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,0,0,10,14,44,46,46,44,35,53,45,29,26,0,0,
0,10,14,44,46,46,44,35,53,45,29,26,0,0,0,10,14,44,46,46,44,35,53,45,29,26,0,0,0,10,14,44,46,46,44,35,53,45,29,26,0,8,16,38,46,31,14,26,0,0,8,16,38,46,31,15,16,38,51,31,15,1,0,0,6,16,31,51,31,15,1,0,0,8,16,38,46,31,15,20,41,46,46,35,37,8,8,16,38,46,31,15,1,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,0,5,14,30,51,41,43,3,0,0,8,16,38,46,31,15,24,27,53,45,29,26,0,5,14,30,51,41,43,13,35,53,46,32,12,0,8,16,
38,51,31,44,45,48,47,35,15,2,0,0,0,22,14,35,38,46,46,50,41,37,19,0,7,11,28,44,38,46,46,50,41,37,19,0,0,12,15,27,30,48,51,50,47,41,28,22,0,0,0,1,4,37,36,51,38,20,8,0,0,0,0,0,6,16,31,51,31,15,1,8,16,38,51,31,44,45,48,46,33,18,1,0,0,0,10,32,36,45,48,30,45,53,36,45,48,30,39,24,9,0,0,0,25,35,46,46,28,21,29,47,53,44,13,9,0,0,6,16,31,51,31,15,1,0,0,0,6,37,41,52,48,33,11,0,0,0,0,0,23,33,45,46,46,46,38,33,13,19,0,0,10,39,48,27,24,0,0,0,
0,0,0,7,21,26,21,12,9,0,0,0,0,0,0,0,12,21,26,22,13,15,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,20,39,39,39,35,45,51,31,16,8,0,0,3,37,36,51,47,28,21,0,0,19,37,36,51,54,46,29,22,20,30,51,50,49,49,35,25,0,0,0,0,9,24,39,41,36,35,14,26,0,0,0,0,21,20,35,36,41,40,13,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,24,20,38,51,36,15,1,0,10,39,48,27,24,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,8,16,38,51,31,15,39,40,28,18,
5,0,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,0,0,11,40,20,32,32,20,36,52,46,32,12,0,0,0,11,40,20,32,32,20,36,52,46,32,12,0,0,0,11,40,20,32,32,20,36,52,46,32,12,0,0,0,11,40,20,32,32,20,36,52,46,32,12,0,1,43,34,40,38,32,22,4,0,1,43,34,34,34,18,20,38,51,36,37,1,0,12,24,20,38,51,36,15,1,0,0,1,43,34,34,34,18,13,14,34,34,34,37,6,1,43,34,34,34,18,4,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,0,5,14,30,51,36,
37,22,3,0,1,43,34,34,34,18,24,27,53,45,29,26,0,4,14,30,51,36,37,32,31,51,31,16,4,0,8,16,38,51,31,15,39,40,29,37,21,0,0,0,0,0,26,13,20,32,28,38,38,16,8,0,0,3,10,18,20,32,28,38,38,16,8,0,0,0,12,18,32,44,52,46,29,15,22,7,0,0,19,18,20,20,31,51,38,20,8,0,0,0,0,12,24,20,38,51,36,15,1,8,16,38,51,31,15,39,40,28,18,5,0,0,0,0,9,22,20,39,40,29,45,53,27,39,40,32,25,6,0,0,0,0,11,34,34,34,20,24,33,48,48,33,23,0,0,12,24,20,38,51,36,15,1,0,0,21,
24,25,27,50,30,32,12,0,0,0,0,0,25,35,50,27,32,34,20,25,21,0,0,0,10,39,48,27,24,0,0,0,0,0,0,0,0,12,37,32,22,0,0,0,0,0,0,10,14,15,12,0,7,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,20,41,45,48,48,50,51,45,33,25,9,0,5,21,15,36,51,47,28,12,0,0,19,37,36,46,46,44,37,4,20,30,46,46,53,53,35,25,0,0,0,0,5,20,31,36,35,30,35,13,10,25,25,22,25,27,30,35,41,30,32,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,27,35,45,53,44,18,
7,0,10,39,48,27,24,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,8,16,38,51,31,15,10,22,21,1,0,0,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,0,0,24,27,31,35,27,41,48,50,44,37,1,0,0,0,24,27,31,35,27,41,48,50,44,37,1,0,0,0,24,27,31,35,27,41,48,50,44,37,1,0,0,0,24,27,31,35,27,41,48,50,44,37,1,0,0,4,18,44,41,28,40,24,0,0,4,12,12,13,27,35,45,53,44,18,7,0,25,27,35,45,53,44,18,7,0,0,0,4,12,12,17,29,44,44,39,10,12,4,0,0,17,29,44,44,39,10,0,0,17,
28,30,52,50,47,47,53,50,41,15,4,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,0,4,14,30,51,38,27,34,12,0,0,4,12,12,12,8,24,27,53,45,29,26,0,42,15,36,52,47,41,31,50,48,27,25,9,0,8,16,38,51,31,15,10,22,26,8,0,0,0,0,0,0,0,7,4,43,35,46,36,15,42,0,0,0,0,3,4,43,35,46,36,15,42,0,0,0,0,19,18,44,52,46,14,2,0,0,0,0,8,20,41,44,48,52,41,37,1,0,0,0,0,25,27,35,45,53,44,18,7,8,16,38,51,31,15,10,22,21,1,0,0,0,0,0,0,0,2,10,22,29,45,53,27,24,
22,21,3,0,0,0,0,0,19,12,12,12,37,33,38,50,41,20,2,0,0,25,27,35,45,53,44,18,7,0,9,18,27,27,38,53,44,43,19,0,0,0,0,0,23,33,48,38,44,44,41,27,37,1,0,0,10,39,31,27,24,0,0,0,0,0,0,3,10,15,27,41,14,17,0,0,0,0,26,20,41,27,43,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,39,45,49,30,46,46,31,33,37,2,0,42,37,29,33,47,51,30,14,5,0,0,7,13,34,34,34,32,22,42,37,34,34,41,53,38,29,23,0,0,0,0,21,28,41,28,13,27,41,37,29,27,35,39,18,44,
35,13,14,41,39,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,35,53,50,30,29,22,0,0,10,39,48,27,24,0,0,9,18,44,50,50,30,29,22,0,0,0,0,0,8,16,38,50,31,15,1,0,0,0,0,0,0,0,9,18,44,50,50,30,29,22,0,0,0,0,0,0,0,24,27,48,50,53,50,45,41,32,10,0,0,0,0,24,27,48,50,53,50,45,41,32,10,0,0,0,0,24,27,48,50,53,50,45,41,32,10,0,0,0,0,24,27,48,50,53,50,45,41,32,10,0,0,0,9,13,35,47,47,27,24,0,0,0,0,0,13,35,53,50,30,29,22,0,0,25,
35,53,50,30,29,22,0,0,0,0,0,0,0,24,27,50,44,37,4,0,0,0,0,24,27,50,44,37,4,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,9,18,44,50,50,30,29,22,0,0,0,0,0,0,6,15,41,48,50,47,28,12,0,0,0,0,0,0,0,24,27,50,45,29,26,0,0,11,39,38,47,48,45,30,35,20,12,0,0,8,16,38,50,31,15,1,0,0,0,0,0,0,0,0,0,0,0,7,43,41,47,27,25,9,0,0,0,0,0,7,43,41,47,27,25,9,0,0,0,0,9,18,44,50,46,14,2,0,0,0,0,8,20,38,49,49,46,40,11,0,0,0,0,0,25,
35,53,50,30,29,22,0,8,16,38,50,31,15,1,0,0,0,0,0,0,0,0,0,0,0,0,26,29,45,50,27,24,0,0,0,0,0,0,0,0,0,0,0,19,37,36,47,44,14,22,0,0,0,25,35,53,50,30,29,22,0,0,9,18,44,50,50,30,29,22,0,0,0,0,0,0,2,15,27,36,31,31,38,31,15,1,0,0,12,37,15,15,10,0,0,0,0,0,9,24,28,44,45,48,35,13,22,11,11,22,25,35,48,30,14,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,48,30,14,32,34,20,43,23,1,0,8,20,38,48,51,49,35,18,3,0,0,0,8,
12,12,12,12,7,0,4,37,35,48,31,29,24,19,0,0,0,0,8,13,43,25,12,18,43,28,30,38,31,46,29,43,43,26,24,43,13,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,35,35,27,29,25,1,0,0,10,39,31,27,24,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,8,16,33,33,33,37,1,0,0,0,0,0,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,0,0,22,32,40,27,35,27,39,16,22,9,0,0,0,0,22,32,40,27,35,27,39,16,22,9,0,0,0,0,22,32,40,27,35,27,39,16,22,9,0,0,0,0,22,32,40,
27,35,27,39,16,22,9,0,0,0,0,21,15,29,29,32,23,0,0,0,0,0,13,35,35,27,29,25,19,0,0,25,35,35,27,29,25,1,0,0,0,0,0,0,1,37,41,38,32,17,0,0,0,0,1,37,41,38,32,17,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,0,0,22,14,33,33,40,14,12,0,0,0,0,0,0,0,24,33,33,33,29,26,0,0,6,25,14,29,39,39,32,43,21,0,0,0,8,16,33,33,33,37,1,0,0,0,0,0,0,0,0,0,0,0,7,43,33,39,37,2,0,0,0,0,0,0,7,43,33,39,
37,2,0,0,0,0,0,9,18,33,33,33,14,2,0,0,0,0,1,37,29,33,33,29,18,4,0,0,0,0,0,25,35,35,27,29,25,1,0,8,16,33,33,33,37,1,0,0,0,0,0,0,0,0,0,0,0,0,26,29,33,33,33,24,0,0,0,0,0,0,0,0,0,0,0,19,37,27,39,37,10,0,0,0,0,25,35,35,27,29,25,1,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,0,0,12,13,37,16,16,20,28,43,19,0,0,0,1,1,1,0,0,0,0,0,0,19,37,35,45,38,35,20,21,14,27,27,32,21,16,44,48,40,22,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,26,29,46,48,36,41,36,36,44,29,10,0,8,20,38,48,46,44,14,21,0,0,0,0,0,0,0,0,0,0,7,25,40,47,31,29,11,3,0,0,0,0,0,0,9,7,9,0,7,23,33,31,14,20,36,27,24,7,0,0,7,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,25,25,24,17,19,0,0,0,12,37,16,16,10,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,0,7,12,22,24,25,24,22,5,0,0,0,0,0,
0,7,12,22,24,25,24,22,5,0,0,0,0,0,0,7,12,22,24,25,24,22,5,0,0,0,0,0,0,7,12,22,24,25,24,22,5,0,0,0,0,0,0,0,4,26,17,12,7,0,0,0,0,0,21,25,25,24,17,19,0,0,0,21,25,25,24,17,19,0,0,0,0,0,0,0,7,25,20,20,24,7,0,0,0,0,7,25,20,20,24,7,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,0,0,12,22,23,22,21,0,0,0,0,0,0,0,0,5,23,23,23,22,19,0,0,0,9,5,26,17,17,12,1,0,0,0,0,0,17,
23,23,23,21,0,0,0,0,0,0,0,0,0,0,0,0,0,12,23,10,4,0,0,0,0,0,0,0,0,12,23,10,4,0,0,0,0,0,0,0,12,23,23,23,10,9,0,0,0,0,0,4,17,23,23,17,1,0,0,0,0,0,0,21,25,25,24,17,19,0,0,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,0,0,0,0,19,22,23,23,23,5,0,0,0,0,0,0,0,0,0,0,0,0,10,25,22,4,0,0,0,0,0,21,25,25,24,17,19,0,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,0,0,9,1,6,6,4,21,5,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,21,20,40,14,13,26,2,14,46,46,32,12,22,29,46,35,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,18,33,36,30,46,30,30,45,39,10,0,8,15,39,39,34,37,17,0,0,0,0,0,0,0,0,0,0,0,8,16,33,33,28,11,3,0,0,0,0,0,0,0,0,0,0,0,0,10,20,20,25,24,20,20,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,4,7,0,0,0,0,0,0,3,4,4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,22,2,7,0,26,29,47,36,20,5,1,43,35,14,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,25,15,14,14,14,14,29,29,10,0,0,12,10,10,21,1,0,0,0,0,0,0,0,0,0,0,0,0,0,17,23,23,22,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,3,3,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,33,48,33,24,0,0,26,43,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,5,2,5,2,10,22,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,35,35,14,12,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,25,25,23,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,4,5,4,3,0,0,0,0,0,0,0,0,4,2,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,0,0,0,4,2,1,5,2,4,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,4,2,2,4,0,0,
0,0,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,0,19,2,2,2,9,0,0,0,0,0,0,0,0,5,2,4,0,5,2,4,0,0,0,0,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,
0,0,0,4,2,4,0,5,2,6,0,0,0,0,0,0,0,0,0,0,19,2,2,2,9,0,0,0,0,0,0,0,0,0,0,3,4,5,4,1,0,0,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,19,19,19,3,0,0,0,0,0,0,0,0,0,0,21,10,10,10,4,0,0,0,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,0,0,26,16,20,15,20,20,18,42,0,0,0,19,22,22,22,22,22,4,0,0,0,0,0,0,0,0,8,10,10,
10,26,9,0,0,0,0,0,0,4,10,10,5,0,0,0,0,0,0,0,0,5,17,17,17,6,0,0,12,17,17,17,3,0,0,0,0,0,0,11,37,37,37,18,4,0,0,12,10,10,10,10,10,5,0,0,0,21,10,10,10,10,10,1,0,0,0,0,0,0,1,12,17,10,10,8,0,0,3,17,10,10,21,8,0,0,0,0,0,0,0,0,0,5,11,37,20,14,20,18,22,8,0,0,0,0,0,4,37,14,14,15,12,0,0,0,0,0,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,0,0,0,10,16,14,37,
20,32,43,42,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,4,37,14,14,15,12,0,0,0,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,7,21,22,26,19,0,0,0,0,0,0,0,0,4,18,37,37,37,43,12,0,0,0,0,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,0,0,24,14,14,14,22,0,0,0,0,0,0,0,17,14,14,15,22,14,
14,16,2,0,0,11,37,37,37,37,37,37,37,37,23,0,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,0,0,0,0,0,24,14,14,14,22,0,0,0,0,0,0,0,0,5,23,37,20,14,20,37,25,26,3,0,0,11,37,37,37,18,4,0,0,0,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,0,12,14,39,39,39,25,0,0,0,0,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,
0,0,3,13,35,38,39,38,36,20,5,0,0,0,10,29,40,40,40,40,24,0,0,0,0,0,0,21,22,24,39,39,39,32,22,19,0,0,0,0,9,13,39,39,13,9,0,0,0,0,0,0,1,43,39,39,39,24,0,4,15,39,39,29,22,0,0,0,0,0,3,37,36,36,36,28,21,0,1,37,39,39,39,39,39,13,7,0,12,14,39,39,39,39,29,23,0,0,0,0,0,12,18,34,39,39,39,24,0,0,10,29,39,39,29,37,26,0,0,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,0,0,0,0,5,20,44,46,27,13,1,0,0,0,0,0,0,0,0,0,0,0,5,37,44,
46,27,43,19,0,0,0,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,0,0,0,4,15,41,30,35,40,38,20,4,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,5,20,44,46,27,13,1,0,0,0,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,7,11,32,40,29,13,6,0,0,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,0,23,
29,30,30,31,32,10,0,0,0,0,0,0,0,9,18,44,46,27,11,0,0,0,0,0,0,0,10,29,30,35,32,41,36,14,12,0,19,37,36,36,36,36,36,36,36,41,43,7,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,0,0,0,0,0,9,18,44,46,27,11,0,0,0,0,0,0,0,21,37,40,41,38,30,38,36,27,28,22,0,19,37,36,36,36,28,12,0,0,0,0,3,37,36,36,36,28,21,0,0,0,0,0,0,0,0,12,28,47,48,35,13,0,0,0,0,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,
0,0,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,0,21,32,30,35,27,46,39,23,0,0,0,0,10,39,48,27,48,27,24,0,0,0,0,0,1,37,33,33,35,48,45,33,29,10,0,0,0,7,12,18,44,41,18,17,5,0,0,0,0,0,10,29,46,45,39,22,0,1,37,44,48,44,37,8,0,0,0,0,3,37,36,51,47,28,21,0,6,16,31,48,48,48,41,43,7,0,12,28,47,48,48,48,27,11,0,0,0,0,3,18,27,46,45,48,35,25,0,0,22,40,48,48,47,44,15,4,0,0,0,0,0,12,16,35,45,52,
53,49,50,53,36,37,3,0,0,0,0,0,17,20,44,30,29,22,0,0,0,0,0,0,0,0,0,0,9,25,33,45,27,37,12,0,0,0,0,0,0,0,0,26,14,36,41,29,31,44,15,5,0,0,0,0,0,0,0,12,28,46,33,31,47,44,43,19,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,19,17,20,44,30,29,22,19,19,19,0,0,0,0,19,19,25,33,45,27,37,12,19,19,0,0,0,0,26,14,36,41,29,31,44,15,5,19,0,0,0,0,4,20,38,38,29,46,41,43,19,19,0,0,0,0,0,26,34,31,46,30,39,23,0,0,0,0,0,0,9,
13,35,50,51,52,44,43,19,0,0,0,0,0,0,0,0,0,9,25,33,45,27,37,12,0,0,0,0,0,0,26,14,36,41,29,31,44,15,5,0,0,0,0,0,0,9,18,44,46,27,11,0,0,0,0,0,0,0,3,25,40,47,36,30,29,23,0,0,19,37,36,51,51,52,52,52,52,41,43,7,0,0,0,0,26,14,36,41,29,31,44,15,5,0,0,0,0,0,19,18,27,46,46,30,40,24,0,0,0,0,0,0,0,0,0,9,18,44,46,27,11,0,0,0,0,0,0,12,16,35,45,52,53,50,50,53,50,27,11,0,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,
0,3,42,42,42,9,12,28,47,53,35,13,0,0,4,12,12,8,0,0,0,7,22,34,44,31,41,39,24,42,7,0,0,0,0,0,3,42,43,33,33,33,33,33,16,8,1,0,0,0,3,42,43,44,44,28,44,44,20,8,1,0,0,0,3,42,25,27,38,32,30,27,18,42,1,0,0,0,10,39,48,27,48,27,24,0,0,0,0,0,42,15,31,49,49,51,53,48,39,10,0,0,42,25,32,33,44,41,33,29,20,22,0,0,0,3,18,35,50,36,16,4,0,0,22,39,45,47,29,10,0,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,36,41,41,43,7,0,12,28,41,41,47,50,27,11,
0,0,0,0,2,14,30,51,46,41,35,25,0,0,22,40,41,38,53,45,39,10,0,0,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,0,0,0,0,12,25,37,20,39,39,13,26,9,0,0,0,0,0,0,0,12,25,37,39,39,15,13,26,9,0,0,0,0,0,0,0,10,29,39,14,14,28,39,14,12,0,0,0,0,0,0,0,12,14,39,20,14,39,20,17,9,0,0,0,0,0,0,0,12,37,34,34,14,34,34,25,9,0,0,0,0,11,37,37,37,20,39,39,37,37,37,37,26,0,0,11,37,37,37,39,39,37,37,37,37,37,26,0,0,11,37,29,39,14,11,28,39,14,37,37,26,
0,0,11,37,37,34,34,20,34,34,37,37,37,26,0,0,0,0,24,27,31,28,41,44,18,7,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,0,0,0,0,5,11,37,39,39,20,18,22,8,0,0,0,0,0,10,29,39,14,14,28,39,14,12,0,0,0,0,0,5,11,37,34,34,32,18,22,8,0,0,0,0,0,5,11,37,39,39,39,18,22,8,0,19,37,36,51,47,41,41,41,41,41,43,7,0,0,0,0,10,29,39,14,14,28,39,14,26,3,0,0,0,0,0,12,37,28,39,32,20,37,25,26,3,0,0,0,0,0,5,23,37,34,34,32,37,25,26,3,0,0,7,13,
27,48,51,46,44,33,27,44,30,27,11,0,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,1,13,15,15,15,11,12,28,47,53,35,13,7,23,16,32,32,15,2,0,3,25,15,15,15,15,37,15,15,15,25,7,0,0,0,19,13,15,15,15,24,23,23,15,15,15,37,2,0,19,13,15,15,15,13,24,13,15,15,15,37,2,0,19,13,15,15,39,39,28,39,14,15,15,37,2,0,0,10,39,48,27,48,27,24,0,0,0,0,10,37,32,33,33,35,53,45,33,39,10,0,0,11,39,38,48,50,50,49,45,27,24,0,0,0,12,32,30,50,27,25,0,0,0,5,20,31,53,
35,18,7,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,18,22,0,0,6,25,18,29,47,50,27,11,0,0,0,0,12,28,47,53,35,43,18,26,0,0,2,13,43,40,48,49,33,22,0,0,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,19,37,36,36,36,28,17,11,27,
36,36,27,11,0,19,37,36,36,36,28,17,11,27,36,36,27,11,0,19,37,36,36,36,28,12,11,27,36,36,27,11,0,19,37,36,36,36,28,12,11,27,36,36,27,11,0,0,0,0,24,27,31,34,41,44,18,7,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,0,0,0,21,15,33,41,38,30,31,44,39,13,7,0,0,0,21,15,33,41,38,30,31,44,39,13,7,0,0,0,21,15,33,41,38,30,31,44,39,13,7,19,37,36,51,47,28,43,43,43,43,22,0,0,0,0,21,37,40,41,38,30,38,36,27,28,22,0,0,
0,0,21,37,40,41,38,30,38,36,27,28,22,0,0,0,0,21,37,40,41,38,30,38,36,27,28,22,0,0,2,14,38,51,45,33,43,23,11,18,34,27,11,0,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,4,20,31,31,31,16,12,28,47,53,35,13,10,28,36,46,46,29,21,0,8,16,31,31,31,15,29,31,31,31,15,1,0,0,0,4,16,36,31,31,20,5,13,35,31,31,29,26,0,4,16,36,31,31,20,5,13,35,31,31,29,26,0,4,16,36,31,31,20,26,13,35,31,31,29,26,0,0,10,39,48,27,48,27,24,0,0,0,10,14,44,30,30,44,35,53,
45,29,22,1,0,42,15,36,52,31,44,41,44,31,27,24,0,0,0,23,33,48,47,29,17,0,0,0,7,13,35,53,38,20,5,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,7,0,0,0,0,9,21,29,47,50,27,11,0,0,0,0,12,28,47,50,27,11,7,0,0,0,0,9,17,39,45,49,33,22,0,0,0,0,10,39,45,51,31,16,5,0,0,2,24,26,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,
52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,0,0,26,28,31,46,30,39,23,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,12,16,35,45,52,53,49,50,53,36,37,3,0,0,12,16,35,45,52,53,49,50,53,36,37,3,0,0,12,16,35,45,52,53,49,50,53,36,37,3,0,0,12,16,35,45,52,53,49,50,
53,36,37,3,19,37,36,51,47,40,40,40,40,39,23,0,0,0,12,16,35,45,52,53,50,50,53,50,27,11,0,0,0,12,16,35,45,52,53,50,50,53,50,27,11,0,0,0,12,16,35,45,52,53,50,50,53,50,27,11,0,0,10,39,45,51,31,16,21,13,13,13,13,13,22,0,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,9,13,35,50,47,29,17,28,47,53,35,13,43,44,53,48,46,29,21,0,8,16,38,51,31,15,44,53,51,31,15,1,0,0,0,0,25,27,49,45,39,22,20,31,51,36,15,8,0,0,25,27,49,45,39,22,20,31,51,36,15,8,0,0,
25,27,49,45,39,22,20,31,51,36,15,8,0,0,10,39,48,27,48,27,24,0,0,6,37,44,50,53,45,45,44,53,45,29,26,0,0,5,14,30,50,27,44,41,18,20,29,11,0,0,9,13,44,53,30,14,5,0,0,0,0,23,33,49,47,29,26,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,7,0,0,0,0,0,21,29,47,50,27,11,0,0,0,7,21,28,47,50,27,11,0,0,0,0,0,0,17,39,45,49,33,22,7,0,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,
36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,0,22,43,20,32,40,29,15,24,4,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,0,7,13,27,48,51,
46,35,33,27,36,36,37,3,0,7,13,27,48,51,46,35,33,27,36,36,37,3,0,7,13,27,48,51,46,35,33,27,36,36,37,3,19,37,36,51,53,49,49,49,49,27,11,0,0,7,13,27,48,51,46,44,33,27,44,30,27,11,0,0,7,13,27,48,51,46,44,33,27,44,30,27,11,0,0,7,13,27,48,51,46,44,33,27,44,30,27,11,0,0,22,33,49,52,44,18,25,35,35,35,35,35,15,42,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,0,10,29,47,50,27,25,28,47,53,35,13,14,30,53,44,32,14,12,0,8,16,38,51,31,29,46,51,51,31,15,1,
0,0,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,0,10,39,48,27,48,27,24,0,0,26,29,46,51,31,39,40,30,51,45,29,26,0,0,4,20,38,51,30,44,41,39,14,25,8,0,0,19,37,36,51,36,15,1,0,0,0,0,10,39,45,49,33,22,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,7,0,0,0,0,0,21,29,47,50,27,11,0,0,2,18,43,39,45,50,33,11,0,0,0,0,0,0,26,29,45,50,35,37,43,17,0,0,22,33,49,52,44,13,
9,0,0,0,0,0,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,0,10,29,
47,53,44,18,27,50,48,33,23,0,0,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,0,2,14,38,51,45,33,18,23,24,20,27,37,3,0,2,14,38,51,45,33,18,23,24,20,27,37,3,0,2,14,38,51,45,33,18,23,24,20,27,37,3,19,37,36,51,49,47,47,47,47,27,11,0,0,2,14,38,51,45,33,43,23,11,18,34,27,11,0,0,2,14,38,51,45,33,43,23,11,18,34,27,11,0,0,2,14,38,51,45,33,43,23,11,18,34,27,11,0,0,22,33,49,52,44,13,25,35,53,53,53,31,15,42,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,
0,4,16,36,51,36,15,28,47,53,35,13,40,48,48,40,23,12,0,0,8,16,38,51,31,44,53,53,51,31,15,1,0,0,0,0,1,37,44,53,38,14,27,50,46,28,21,0,0,0,1,37,44,53,38,14,27,50,46,28,21,0,0,0,1,37,44,53,38,14,27,50,46,28,21,0,0,0,10,39,48,27,48,27,24,0,0,23,33,49,50,27,25,18,44,52,45,29,26,0,0,7,13,27,45,52,52,50,45,38,40,25,9,0,42,15,31,51,41,43,3,0,0,0,0,21,29,45,50,27,11,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,7,0,0,0,0,0,21,29,47,50,27,11,
0,0,22,40,41,30,50,30,29,17,0,0,0,0,0,0,2,14,31,49,47,36,35,25,0,0,22,39,48,51,41,37,1,0,0,19,21,4,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,
0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,0,10,39,45,51,31,16,5,0,0,2,24,26,0,0,10,39,45,51,31,16,5,0,0,2,24,26,0,0,10,39,45,51,31,16,5,0,0,2,24,26,0,0,10,39,45,51,31,16,5,0,0,2,24,26,0,19,37,36,51,47,28,28,28,28,28,22,0,0,10,39,45,51,31,16,21,13,13,13,13,13,22,0,0,10,39,45,51,31,16,21,13,13,13,13,13,22,0,0,10,39,45,51,31,16,21,13,13,13,13,13,22,0,0,22,39,
48,51,41,37,25,35,44,38,51,31,15,42,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,25,27,50,46,29,28,47,53,35,43,44,53,38,14,12,0,0,0,8,16,38,51,31,46,45,38,51,31,15,1,0,0,0,0,0,23,40,45,48,40,36,52,41,37,42,0,0,0,0,23,40,45,48,40,36,52,41,37,42,0,0,0,0,23,40,45,48,40,36,52,41,37,42,0,0,0,10,39,48,27,48,27,24,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,0,12,37,40,44,31,47,48,51,38,20,5,0,42,15,31,51,41,37,3,0,0,0,0,21,29,45,
50,27,11,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,7,0,0,0,0,0,21,29,47,50,27,11,0,0,22,40,48,50,47,44,14,12,0,0,0,0,0,0,8,37,35,30,53,48,35,25,0,0,12,32,46,51,46,29,23,12,26,13,28,13,7,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,19,37,36,51,47,28,12,11,27,
50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,19,37,36,51,47,28,23,23,23,23,21,0,0,22,33,49,52,44,18,25,35,35,35,35,35,15,42,0,
22,33,49,52,44,18,25,35,35,35,35,35,15,42,0,22,33,49,52,44,18,25,35,35,35,35,35,15,42,0,12,32,46,51,46,29,11,13,20,38,51,31,15,42,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,26,29,46,50,27,28,47,53,35,14,30,53,35,18,3,0,0,0,8,16,38,51,48,53,41,38,51,31,15,1,1,0,0,0,0,2,20,31,53,44,46,49,33,24,0,0,0,0,0,2,20,31,53,44,46,49,33,24,0,0,0,0,0,2,20,31,53,44,46,49,33,24,0,0,0,0,10,39,48,27,48,27,24,0,0,23,33,49,50,27,11,13,35,53,
45,29,26,0,0,1,13,37,23,43,44,41,27,49,47,28,21,0,19,37,36,51,36,15,1,0,0,0,0,10,39,45,49,40,22,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,7,0,0,0,0,0,21,29,47,50,27,11,0,0,10,28,39,35,50,48,33,23,0,0,0,0,0,0,21,28,46,52,36,39,39,24,0,0,1,37,44,50,53,36,39,32,29,27,36,37,3,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,
23,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,9,11,32,27,44,41,41,46,52,44,18,7,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,22,33,49,52,44,13,9,0,0,0,0,0,0,0,22,33,49,52,44,13,9,0,0,0,0,0,0,0,22,33,49,52,44,13,9,0,0,0,0,0,0,0,22,33,49,52,44,13,9,0,0,
0,0,0,0,19,37,36,51,47,33,33,33,33,33,15,6,0,22,33,49,52,44,13,25,35,53,53,53,31,15,42,0,22,33,49,52,44,13,25,35,53,53,53,31,15,42,0,22,33,49,52,44,13,25,35,53,53,53,31,15,42,0,1,37,44,50,53,31,39,32,28,38,51,31,15,42,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,6,37,41,52,31,36,47,52,36,36,48,45,39,22,0,0,0,0,8,16,38,51,54,47,39,38,51,31,15,15,37,21,0,0,0,9,13,35,50,46,49,46,34,21,0,0,0,0,0,9,13,35,50,46,49,46,34,21,0,0,0,0,
0,9,13,35,50,46,49,46,34,21,0,0,0,0,10,39,48,27,48,27,24,0,0,10,39,45,52,41,14,28,38,51,45,29,26,0,0,4,14,44,39,14,44,41,33,48,46,28,12,0,9,13,35,53,30,14,5,0,0,0,0,11,33,49,47,29,26,0,0,0,3,37,36,51,47,28,21,0,6,16,31,51,41,43,7,0,0,0,0,0,21,29,47,50,27,11,0,0,3,26,10,29,47,50,27,11,0,0,0,0,0,0,17,29,45,49,33,11,17,6,0,0,0,10,34,31,50,52,45,46,47,49,36,37,3,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,22,39,48,52,41,37,19,0,
24,27,50,53,35,25,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,0,21,32,31,49,49,45,47,49,52,41,18,7,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,0,22,39,48,51,41,37,1,0,0,19,21,4,0,0,22,39,48,51,41,
37,1,0,0,19,21,4,0,0,22,39,48,51,41,37,1,0,0,19,21,4,0,0,22,39,48,51,41,37,1,0,0,19,21,4,0,19,37,36,51,53,49,49,49,49,31,16,6,0,22,39,48,51,41,37,25,35,44,38,51,31,15,42,0,22,39,48,51,41,37,25,35,44,38,51,31,15,42,0,22,39,48,51,41,37,25,35,44,38,51,31,15,42,0,0,10,34,31,50,52,45,46,47,49,52,31,15,42,19,37,36,51,47,28,12,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,24,33,49,51,51,51,54,51,51,51,31,20,5,0,0,0,0,8,16,38,51,53,41,16,38,51,30,31,31,
33,22,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,0,10,39,48,27,48,27,24,0,0,4,16,36,53,49,30,30,31,53,45,29,26,0,0,4,14,30,45,30,38,30,47,50,41,15,4,0,0,23,33,48,47,29,17,0,0,0,7,18,44,53,38,20,4,0,0,5,21,15,36,51,47,28,12,0,6,16,31,51,41,43,3,0,0,0,0,7,21,29,47,50,27,11,0,0,0,0,12,28,47,50,27,11,0,0,0,0,0,0,17,39,45,49,33,22,0,0,0,0,0,7,11,28,44,46,
48,49,45,30,35,43,3,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,0,23,33,49,49,35,29,29,46,52,41,18,7,4,20,31,46,
30,32,21,4,5,20,31,46,30,32,12,0,0,12,32,46,51,46,29,23,12,26,13,28,13,7,0,12,32,46,51,46,29,23,12,26,13,28,13,7,0,12,32,46,51,46,29,23,12,26,13,28,13,7,0,12,32,46,51,46,29,23,12,26,13,28,13,7,19,37,36,46,46,46,46,45,46,31,16,6,0,12,32,46,51,46,29,11,13,20,38,51,31,15,42,0,12,32,46,51,46,29,11,13,20,38,51,31,15,42,0,12,32,46,51,46,29,11,13,20,38,51,31,15,42,0,0,7,11,28,44,46,48,49,48,46,36,33,37,1,19,37,36,46,47,28,26,0,0,0,5,21,15,36,51,47,28,12,0,0,
0,0,0,21,34,30,46,46,48,52,46,46,46,35,13,9,0,0,0,0,8,16,38,46,46,39,16,38,46,46,53,49,33,22,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,0,10,39,48,27,48,27,24,0,0,0,23,29,31,48,48,31,35,46,46,29,26,0,0,4,20,35,36,30,46,46,30,44,34,22,0,0,0,12,32,30,50,27,25,0,0,0,5,20,31,50,35,13,7,0,42,37,29,33,47,51,30,14,5,0,6,16,31,51,41,37,37,22,0,0,8,13,37,29,47,50,27,11,
0,0,0,0,12,28,47,50,27,24,17,8,0,0,3,26,10,39,48,49,33,22,0,0,0,0,0,0,7,10,37,34,39,35,41,14,18,21,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,25,27,50,50,44,28,29,38,51,47,29,17,
0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,24,27,50,48,39,37,33,48,52,41,18,7,4,15,34,34,34,43,1,0,11,40,38,34,34,20,12,0,0,1,37,44,50,53,36,39,32,29,27,36,37,3,0,1,37,44,50,53,36,39,32,29,27,36,37,3,0,1,37,44,50,53,36,39,32,29,27,36,37,3,0,1,37,44,50,53,36,39,32,29,27,36,37,3,7,13,34,34,34,34,40,38,34,34,43,1,0,1,37,44,50,53,31,39,32,28,38,51,31,15,42,0,1,37,44,50,53,31,39,32,28,38,51,31,15,42,0,1,37,44,50,53,31,39,32,28,38,51,31,15,42,0,0,0,
7,10,37,34,39,40,39,28,15,24,2,0,7,13,34,40,38,32,22,4,0,42,37,29,33,47,51,30,14,5,0,0,0,0,0,1,43,34,34,28,47,53,35,34,34,32,10,0,0,0,0,0,1,43,34,34,34,37,43,34,34,41,53,36,34,10,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,0,10,39,48,27,48,27,24,0,0,0,19,11,14,39,39,14,34,34,34,20,12,0,0,0,17,25,15,14,44,41,14,43,10,7,0,0,0,3,18,35,50,36,20,4,0,0,22,39,45,46,
29,10,0,0,8,20,38,48,51,49,35,18,3,0,6,16,31,51,36,36,41,43,7,0,12,28,36,36,45,50,27,11,0,0,0,0,12,32,46,52,41,39,39,24,0,0,10,28,39,35,50,48,40,22,0,0,0,0,0,0,0,0,17,29,29,39,46,28,12,0,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,17,28,30,52,50,47,47,53,
50,41,15,4,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,22,40,45,52,30,36,46,47,52,41,18,7,0,5,12,12,12,4,0,7,18,44,41,28,40,24,0,0,0,0,10,34,31,50,52,45,46,47,49,36,37,3,0,0,10,34,31,50,52,45,46,47,49,36,37,3,0,0,10,34,31,50,52,45,46,47,49,36,37,3,0,0,10,34,31,50,52,45,46,47,49,36,37,3,0,8,12,12,12,18,44,41,28,40,24,0,0,0,10,34,31,50,52,45,46,47,49,52,31,15,42,0,
0,10,34,31,50,52,45,46,47,49,52,31,15,42,0,0,10,34,31,50,52,45,46,47,49,52,31,15,42,0,0,0,0,0,1,17,29,44,44,39,10,0,0,0,0,8,18,44,41,28,40,24,0,8,20,38,48,51,49,35,18,3,0,0,0,0,0,0,4,12,12,28,47,53,35,13,12,12,9,0,0,0,0,0,0,4,12,12,12,4,4,13,27,48,31,28,23,7,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,0,10,39,48,27,48,27,24,0,0,0,7,18,39,39,39,39,39,15,
12,2,0,0,0,0,0,0,42,18,44,41,18,19,0,0,0,0,0,0,10,29,30,45,39,23,0,1,37,44,45,44,37,6,0,0,8,20,38,48,46,44,14,21,0,0,6,16,31,45,45,45,41,43,7,0,12,28,47,45,45,45,27,11,0,0,0,0,8,15,41,50,53,48,35,25,0,0,22,40,48,50,53,38,32,12,0,0,0,0,0,0,0,0,17,39,47,45,31,14,2,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,
45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,5,16,44,45,48,46,27,46,46,41,18,7,0,0,0,0,0,0,0,9,13,35,47,47,27,24,0,0,0,0,7,11,28,44,46,48,49,45,30,35,43,3,0,0,7,11,28,44,46,48,49,45,30,35,43,3,0,0,7,11,28,44,46,48,49,45,30,35,43,3,0,0,7,11,28,44,46,48,49,45,
30,35,43,3,0,0,0,0,9,13,35,47,47,27,24,0,0,0,7,11,28,44,46,48,49,48,46,36,33,37,1,0,0,7,11,28,44,46,48,49,48,46,36,33,37,1,0,0,7,11,28,44,46,48,49,48,46,36,33,37,1,0,0,0,0,0,0,24,27,50,44,37,4,0,0,0,0,9,13,35,47,47,27,24,0,8,20,38,48,46,44,14,21,0,0,0,0,0,0,0,0,0,12,28,47,50,35,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,29,30,38,29,11,7,0,0,0,9,18,44,50,50,30,29,22,0,0,0,0,0,0,9,18,44,50,50,30,29,22,0,0,0,0,0,0,
9,18,44,50,50,30,29,22,0,0,0,0,0,0,10,39,48,27,48,27,24,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,0,0,0,0,9,13,27,27,18,7,0,0,0,0,0,0,1,18,29,29,29,11,0,8,15,29,29,28,22,0,0,0,8,15,39,39,34,37,17,0,0,0,1,37,29,29,29,29,29,13,9,0,12,14,29,29,29,29,29,22,0,0,0,0,0,22,32,27,41,41,35,25,0,0,22,40,41,41,35,29,24,7,0,0,0,0,0,0,0,0,21,20,29,39,14,11,9,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,
29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,0,0,0,0,0,0,21,15,29,29,32,23,0,0,0,0,0,7,10,37,34,39,40,39,14,18,21,0,0,0,0,7,10,37,
34,39,40,39,14,18,21,0,0,0,0,7,10,37,34,39,40,39,14,18,21,0,0,0,0,7,10,37,34,39,40,39,14,18,21,0,0,0,0,0,0,21,15,29,29,32,23,0,0,0,0,7,10,37,34,39,40,39,28,15,24,2,0,0,0,0,7,10,37,34,39,40,39,28,15,24,2,0,0,0,0,7,10,37,34,39,40,39,28,15,24,2,0,0,0,0,0,0,1,37,41,38,32,17,0,0,0,0,0,0,21,15,29,29,32,23,0,8,15,39,39,34,37,17,0,0,0,0,0,0,0,0,0,0,12,32,33,33,33,25,0,0,0,0,0,0,0,0,0,0,0,0,0,9,13,33,33,29,24,3,
0,0,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,0,10,39,31,27,31,27,24,0,0,0,7,43,33,33,33,33,33,16,8,0,0,0,0,0,0,0,0,12,11,11,21,0,0,0,0,0,0,0,0,4,26,26,26,1,0,0,12,26,26,21,3,0,0,0,0,12,10,10,21,1,0,0,0,0,0,2,26,26,26,26,26,4,0,0,0,12,26,26,26,26,26,19,0,0,0,0,0,9,26,25,43,43,43,26,0,0,2,18,43,43,13,22,19,0,0,0,0,0,0,0,0,0,0,5,
21,17,12,7,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,0,
0,0,0,0,0,4,26,17,12,7,0,0,0,0,0,0,0,1,12,10,22,10,2,3,0,0,0,0,0,0,0,1,12,10,22,10,2,3,0,0,0,0,0,0,0,1,12,10,22,10,2,3,0,0,0,0,0,0,0,1,12,10,22,10,2,3,0,0,0,0,0,0,0,0,4,26,17,12,7,0,0,0,0,0,0,1,12,10,22,10,21,8,0,0,0,0,0,0,0,0,1,12,10,22,10,21,8,0,0,0,0,0,0,0,0,1,12,10,22,10,21,8,0,0,0,0,0,0,0,0,7,25,20,20,24,7,0,0,0,0,0,0,0,4,26,17,12,7,0,0,12,10,10,21,1,0,0,0,0,0,
0,0,0,0,0,0,7,22,23,23,23,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,23,23,22,19,0,0,0,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,12,37,16,16,16,16,10,0,0,0,0,12,23,23,23,23,23,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,7,7,7,0,0,0,0,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,3,4,4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,7,0,19,19,19,7,0,0,0,0,19,19,19,7,0,0,3,19,19,19,3,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,19,19,19,19,0,0,9,19,19,19,0,0,0,9,19,19,19,9,3,4,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,4,0,5,2,6,
0,0,0,0,0,0,0,0,0,0,1,2,2,5,2,2,4,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,42,4,4,8,3,0,0,0,0,0,0,0,5,2,4,0,5,2,4,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,1,5,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,4,0,5,2,6,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,1,2,2,5,2,2,4,0,0,0,0,0,19,19,19,7,0,0,19,19,19,19,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,0,0,0,0,9,3,19,19,19,19,9,0,0,0,0,0,9,6,4,4,6,9,0,0,0,0,0,0,0,0,0,0,0,3,19,19,3,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,7,8,5,1,3,19,19,19,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,2,4,3,0,0,0,0,0,0,0,0,0,19,19,19,3,0,0,0,0,0,
0,0,0,0,7,19,19,19,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,42,5,4,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0,0,11,37,37,37,18,11,37,37,37,18,4,0,0,11,37,37,37,18,4,5,18,37,37,37,18,4,0,0,11,37,37,37,18,4,0,0,0,0,0,0,11,37,37,37,37,21,3,25,37,37,37,11,9,7,24,37,37,37,25,18,20,14,37,
23,3,0,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,0,0,0,0,19,13,14,14,14,14,14,37,4,0,0,0,11,37,37,37,37,37,43,25,17,3,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,2,25,15,14,14,16,43,24,12,0,0,0,0,17,14,14,15,22,14,14,16,2,0,0,1,13,37,37,37,37,37,37,37,37,37,37,24,9,0,0,2,22,22,22,17,0,0,0,0,0,0,0,10,16,14,37,20,32,43,42,0,0,0,0,0,0,
5,10,10,10,10,10,12,0,0,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,0,0,0,7,21,22,26,19,0,0,0,0,0,0,0,19,13,14,14,14,14,14,37,4,0,0,0,11,37,37,37,18,4,21,37,37,37,37,26,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,0,0,0,0,5,22,13,37,37,37,37,37,24,7,0,0,7,17,13,15,14,14,15,13,10,1,0,0,0,0,0,0,0,0,2,43,37,37,18,4,0,0,26,37,37,37,25,1,0,0,0,0,0,0,3,10,18,16,14,13,18,37,37,37,23,0,0,11,37,37,37,18,4,0,0,0,0,0,
0,0,0,0,0,0,0,6,23,37,14,14,20,43,22,19,0,0,0,0,0,0,10,37,37,37,37,12,0,0,0,0,0,0,0,8,13,37,37,37,22,0,0,0,0,1,13,37,37,37,37,37,37,37,37,37,22,0,0,0,0,0,0,0,1,23,37,14,20,25,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,22,25,18,25,17,7,0,0,0,0,0,0,0,0,0,0,0,0,3,24,18,18,18,18,18,18,18,18,23,0,0,0,0,0,0,0,19,37,36,36,36,28,37,36,36,36,28,12,0,19,37,36,36,36,28,12,18,33,36,36,36,28,21,0,19,37,36,36,
36,28,12,0,0,0,0,0,19,37,36,36,36,35,13,4,20,36,36,36,15,1,8,16,36,36,36,14,35,38,30,41,29,11,0,0,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,0,0,0,0,0,22,29,30,31,27,46,44,16,5,0,0,19,37,36,36,36,36,36,41,27,28,24,7,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,12,15,33,36,30,30,31,41,27,43,7,0,0,0,10,29,30,35,32,41,36,14,12,0,0,2,14,36,36,36,36,36,36,36,36,
36,36,15,1,0,9,13,40,40,40,14,2,0,0,0,0,0,4,15,41,30,35,40,38,20,4,0,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,0,0,0,7,11,32,40,29,13,6,0,0,0,0,0,0,22,29,30,31,27,46,44,16,5,0,0,19,37,36,36,36,28,12,11,27,36,36,27,11,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,26,15,39,35,41,36,36,36,36,15,42,0,1,24,28,35,31,30,30,31,35,29,13,2,0,0,0,0,0,0,3,13,27,36,36,29,21,0,0,11,33,36,36,
29,23,0,0,0,0,0,19,24,29,44,31,31,15,29,36,36,36,37,3,19,37,36,36,36,28,12,0,0,0,0,0,0,0,0,0,0,0,8,13,39,41,30,30,38,44,29,25,1,0,0,0,0,0,25,35,36,36,33,22,0,0,0,0,0,0,0,12,28,36,36,44,13,0,0,0,0,2,14,36,36,36,36,36,36,36,36,44,13,0,0,0,0,0,0,3,25,39,41,30,31,27,37,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,11,37,39,35,44,35,34,22,0,0,0,0,0,0,0,0,0,0,0,0,5,14,44,44,44,44,44,44,44,44,37,1,0,0,0,0,0,0,
19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,28,13,40,46,52,30,39,24,3,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,51,30,32,21,20,30,51,31,15,1,8,16,38,51,38,41,50,51,51,53,31,14,12,0,0,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,0,0,0,0,19,18,27,46,46,30,40,24,0,0,0,0,0,0,0,0,2,20,41,38,29,30,44,20,26,0,0,0,19,37,36,51,51,52,52,52,50,38,34,26,0,0,0,0,0,9,25,33,45,27,37,12,0,0,0,0,0,26,14,36,41,29,31,44,15,5,0,0,7,13,27,45,
51,50,49,50,52,41,43,7,0,0,0,3,25,40,47,36,30,29,23,0,0,0,2,14,46,52,52,52,51,51,52,52,52,31,15,1,0,3,18,44,49,46,32,12,42,0,0,0,0,12,28,46,33,31,47,44,43,19,19,0,0,0,0,19,43,41,48,48,48,38,20,8,19,0,0,0,0,19,18,27,46,46,30,40,24,19,19,0,0,0,0,0,26,34,31,46,30,39,23,0,0,0,0,0,2,20,41,38,29,30,44,20,26,19,0,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,2,15,44,45,53,51,52,52,52,31,15,42,
0,10,29,38,50,51,49,49,53,50,30,33,43,5,0,0,0,0,0,26,28,30,53,44,43,19,0,0,2,20,31,53,36,16,4,0,0,0,0,22,29,38,53,52,31,15,29,47,51,36,37,3,19,37,36,51,47,28,12,5,12,5,1,0,0,0,0,0,0,0,11,39,46,52,50,48,53,53,30,29,23,0,0,9,42,42,25,35,53,49,33,22,42,42,0,0,0,9,42,12,28,47,52,44,13,42,1,0,0,2,14,46,52,51,51,51,52,52,52,44,13,0,0,0,0,0,0,10,29,30,52,50,53,45,27,13,42,42,1,0,1,42,42,1,0,0,0,0,0,0,0,0,0,12,32,41,46,31,41,38,
36,16,4,0,0,0,0,0,0,0,0,0,0,0,3,13,27,31,27,36,35,27,30,33,11,7,0,0,0,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,28,39,30,51,30,39,25,1,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,54,49,27,13,20,30,51,31,15,1,8,16,38,51,47,49,46,46,49,54,48,40,22,0,0,0,0,0,12,43,33,33,33,33,33,16,8,0,0,0,0,0,0,0,12,25,37,28,39,32,13,26,9,0,0,0,0,0,0,0,12,14,39,29,39,39,20,17,9,0,0,0,19,37,36,51,47,44,44,30,52,50,27,24,
0,0,0,0,2,25,15,39,39,16,43,24,12,0,0,0,0,10,29,39,14,14,28,39,14,12,0,0,4,20,38,51,30,27,33,27,36,41,43,7,0,0,0,2,25,15,39,39,39,43,24,12,0,0,2,14,41,41,41,41,53,49,41,41,41,41,15,1,3,13,15,44,52,46,32,15,15,23,0,0,11,37,14,39,20,14,39,20,37,37,37,26,0,0,11,37,37,33,33,33,33,33,16,37,37,26,0,0,11,37,37,37,28,39,32,37,37,37,37,26,0,0,0,0,24,27,31,28,41,44,18,7,0,0,0,11,37,14,39,29,39,39,20,37,37,37,26,0,19,37,36,51,47,28,12,11,27,50,50,27,
11,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,9,25,33,48,51,47,36,41,41,41,41,15,42,0,10,39,48,30,49,27,33,35,46,51,47,33,25,3,0,0,0,9,13,35,50,48,40,22,0,0,0,9,13,35,53,45,39,22,0,0,0,1,37,44,53,53,36,33,37,29,47,51,36,37,3,19,37,36,51,47,28,43,14,32,14,37,23,8,0,0,0,0,4,15,36,52,45,35,40,41,50,53,41,37,42,0,11,15,15,15,35,53,49,33,15,15,15,10,0,0,11,15,15,28,47,52,44,15,15,37,2,0,2,14,41,41,36,51,47,41,41,41,44,13,0,0,0,0,0,1,43,
44,53,30,33,41,53,30,32,15,15,37,21,37,15,15,15,21,0,0,0,0,0,0,0,0,12,32,41,33,44,44,29,31,32,12,0,0,0,0,0,0,0,0,0,0,2,43,37,28,33,24,40,29,18,33,20,37,25,1,0,0,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,29,30,52,46,33,13,8,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,45,49,38,14,20,30,51,31,15,1,8,16,38,51,53,41,28,32,35,50,53,27,25,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,
11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,19,37,36,51,47,28,18,33,49,53,35,25,0,0,0,12,15,33,36,30,30,31,41,27,43,7,0,0,12,15,33,36,30,30,31,41,27,43,7,0,12,32,46,53,35,13,23,25,15,39,18,7,0,0,12,15,33,36,30,30,31,41,27,43,7,0,19,25,43,43,43,35,53,49,33,43,43,43,23,0,4,20,31,31,52,47,31,31,44,18,9,19,37,36,36,36,28,12,11,27,36,36,27,11,0,19,37,36,36,36,28,23,11,27,36,36,27,11,0,19,37,36,36,36,28,17,11,27,36,36,27,11,0,0,0,0,24,27,31,34,
41,44,18,7,0,0,19,37,36,36,36,28,17,11,27,36,36,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,5,20,31,51,45,33,15,43,43,43,43,11,9,0,10,39,27,36,49,33,23,16,27,50,51,30,32,21,0,0,0,4,20,31,51,30,14,12,2,2,2,5,22,39,45,53,44,18,7,0,0,12,32,30,54,46,29,11,21,29,47,51,36,37,3,19,37,36,51,47,28,35,30,46,30,41,39,18,5,0,0,0,17,29,46,46,41,37,22,34,30,51,30,14,2,7,43,41,31,31,31,53,50,31,31,31,27,24,0,
7,43,41,31,31,45,52,31,31,31,29,26,0,19,25,43,37,36,51,47,28,43,43,43,10,5,0,0,0,0,12,32,30,52,44,18,40,48,49,33,31,31,33,24,27,31,31,40,22,0,0,0,0,4,25,25,25,24,43,37,33,35,36,37,15,43,23,25,25,25,12,0,0,0,0,0,3,13,27,36,36,29,21,23,22,11,33,36,36,29,23,0,0,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,31,52,47,33,18,4,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,30,49,27,20,30,51,31,15,1,8,16,38,51,47,29,22,17,39,45,
53,44,13,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,19,37,36,51,47,28,13,33,49,50,27,24,0,0,7,13,27,45,51,50,49,50,52,41,43,7,0,7,13,27,45,51,50,49,50,52,41,43,7,0,2,14,30,51,30,35,39,32,37,22,5,0,0,7,13,27,45,51,50,49,50,52,41,43,7,0,0,0,7,7,25,35,53,49,33,22,7,7,0,0,4,20,38,49,51,50,49,49,44,18,9,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,
51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,3,42,26,28,31,46,30,39,23,1,0,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,17,29,45,51,31,16,5,7,7,7,7,0,0,0,21,20,37,36,49,33,20,44,48,47,48,49,27,25,0,0,0,26,29,47,51,36,37,23,14,14,14,20,21,32,46,51,31,20,4,0,0,10,39,48,51,36,37,1,21,29,47,51,36,37,3,19,37,36,51,47,44,49,50,50,52,52,46,
33,13,19,0,0,21,14,14,14,14,23,6,15,31,51,46,32,12,7,43,41,52,52,52,54,51,52,52,53,27,24,0,7,43,41,49,49,53,51,49,49,45,29,26,0,0,0,3,37,36,51,47,40,40,40,40,39,16,10,0,0,0,22,39,48,53,35,25,29,45,52,44,30,52,41,37,41,52,38,14,2,0,0,0,9,24,40,35,35,29,21,22,33,36,35,13,42,4,14,35,35,27,18,1,0,0,0,0,26,28,30,53,44,43,19,0,0,2,20,31,53,36,16,4,0,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,52,52,45,27,37,5,0,0,0,0,0,19,37,36,51,
47,28,12,0,0,0,0,0,19,37,36,51,30,27,50,31,14,30,51,31,15,1,8,16,38,51,30,20,4,21,29,47,52,44,13,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,19,37,36,51,47,44,44,30,53,38,28,26,0,0,4,20,38,51,30,27,33,27,36,41,43,7,0,4,20,38,51,30,27,33,27,36,41,43,7,0,1,37,44,50,51,50,48,46,41,29,24,7,0,4,20,38,51,30,27,33,27,36,41,43,7,0,0,0,0,0,25,35,53,49,33,22,
0,0,0,0,8,16,40,44,52,46,40,40,40,13,9,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,1,13,15,15,15,32,40,29,15,15,43,4,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,0,0,4,37,36,49,33,27,48,47,33,38,51,36,15,6,0,0,22,40,49,52,44,18,25,35,46,46,39,10,20,30,
51,46,32,12,0,0,22,33,49,52,44,13,9,21,29,47,51,36,37,3,19,37,36,51,48,45,41,27,27,36,49,51,30,29,17,0,0,0,5,2,2,2,7,26,32,30,51,31,20,4,7,43,41,49,44,44,53,49,44,44,53,27,24,0,7,43,41,41,40,47,52,44,40,45,29,26,0,0,0,3,37,36,51,53,49,49,49,49,45,44,20,2,0,0,23,33,50,53,35,25,29,45,51,36,44,53,30,32,30,53,35,18,7,0,0,0,12,14,31,53,44,37,8,22,33,27,20,21,0,7,25,33,49,46,29,10,0,0,0,9,13,35,50,48,40,22,0,0,0,9,13,35,53,45,39,22,0,0,0,0,
19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,52,52,50,44,20,26,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,34,30,48,33,30,51,31,15,1,8,16,38,51,38,16,8,12,28,47,52,44,13,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,19,37,36,51,51,52,52,53,44,29,24,3,0,0,12,32,46,53,35,13,23,25,15,39,18,7,0,12,32,46,53,35,13,23,25,15,39,18,7,0,0,10,14,44,
46,48,53,51,53,31,14,12,0,12,32,46,53,35,13,23,25,15,39,18,7,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,21,18,44,52,46,32,22,22,2,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,5,14,31,31,36,37,25,35,31,31,28,21,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,0,0,0,0,0,
0,0,3,37,35,35,33,31,51,31,16,41,51,30,32,12,0,0,23,33,49,52,44,18,25,35,53,48,39,10,20,38,51,47,28,12,0,0,22,33,49,52,44,13,0,21,29,47,51,36,37,3,19,37,36,51,53,44,16,11,24,20,41,53,50,35,25,0,0,0,0,0,0,0,12,15,44,50,49,27,13,7,7,43,44,44,40,35,53,49,33,35,44,27,24,0,7,13,29,29,28,47,52,44,32,29,32,21,0,0,0,3,37,36,51,49,47,47,47,49,54,48,40,22,0,0,23,33,50,53,35,25,29,45,51,36,39,45,48,33,48,45,39,22,0,0,0,0,11,33,48,48,40,22,0,5,25,24,21,0,
0,0,21,32,30,53,44,18,3,0,0,4,20,31,51,30,14,12,2,2,2,5,22,39,45,53,35,18,7,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,31,52,49,44,16,21,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,14,35,50,31,30,51,31,15,1,8,16,38,51,38,16,8,12,28,47,52,44,13,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,19,37,36,51,47,44,36,49,50,36,20,2,
0,0,2,14,30,51,30,35,39,32,37,22,5,0,0,2,14,30,51,30,35,39,32,37,22,5,0,0,0,21,17,37,28,33,44,30,51,48,40,22,0,2,14,30,51,30,35,39,32,37,22,5,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,9,18,44,52,46,32,12,0,0,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,5,14,30,51,36,37,25,35,53,47,28,21,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,21,11,27,50,50,27,
11,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,22,39,48,51,41,37,1,0,0,0,0,0,0,0,0,0,17,25,25,14,30,51,36,37,44,52,47,29,21,0,0,22,40,49,52,41,43,13,35,53,48,40,22,14,30,54,46,32,12,0,0,22,40,48,52,41,43,3,10,39,45,51,36,37,3,19,37,36,51,45,39,10,0,0,22,39,45,52,41,43,3,0,0,0,0,0,12,37,27,45,53,36,14,21,0,0,10,13,13,13,35,53,49,33,13,13,13,21,0,0,4,26,26,28,47,52,44,13,26,21,9,0,0,0,3,37,36,51,47,28,28,28,35,53,53,35,25,0,0,22,40,
49,53,35,25,29,45,52,41,14,38,53,41,53,38,20,5,0,0,0,1,37,41,52,30,14,12,0,0,0,0,0,0,0,0,1,37,41,52,30,14,2,0,0,26,29,47,51,36,37,23,14,14,14,20,21,32,46,51,31,20,4,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,29,30,52,48,35,37,12,0,0,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,30,14,28,30,48,30,51,31,15,1,8,16,38,51,38,16,8,12,28,47,52,44,13,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,22,33,49,52,44,18,9,0,23,33,49,
53,35,13,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,19,37,36,51,47,28,20,41,52,48,33,11,0,0,1,37,44,50,51,50,48,46,41,29,24,7,0,1,37,44,50,51,50,48,46,41,29,24,7,0,4,20,28,18,22,23,18,33,50,50,33,23,0,1,37,44,50,51,50,48,46,41,29,24,7,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,9,18,44,52,46,32,12,12,7,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,5,14,30,51,41,37,25,
35,53,47,28,21,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,12,32,46,51,46,29,23,12,1,0,0,0,0,0,0,0,0,0,4,14,30,51,36,37,35,53,45,29,26,0,0,17,39,45,51,31,16,37,41,53,50,27,25,28,47,51,30,14,5,0,0,21,29,47,51,38,14,21,13,27,50,51,36,37,3,19,37,36,51,47,28,21,0,0,21,28,47,51,41,43,3,0,0,0,0,12,37,27,45,52,38,29,23,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,
0,0,0,12,28,47,52,44,13,0,0,0,0,0,0,3,37,36,51,47,28,21,22,33,49,53,35,13,0,0,21,29,47,53,44,13,39,45,50,27,18,35,53,52,53,35,13,7,0,0,0,12,32,30,51,36,37,1,0,0,0,0,0,0,0,0,0,25,35,53,45,39,10,0,0,22,40,49,52,44,18,25,35,46,46,39,10,20,30,51,46,32,12,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,28,39,30,51,45,27,43,5,0,0,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,30,14,43,35,50,51,51,31,15,1,8,16,38,51,38,16,8,12,28,47,
52,44,13,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,19,37,36,51,47,28,23,40,45,52,36,15,4,0,0,10,14,44,46,48,53,51,53,31,14,12,0,0,10,14,44,46,48,53,51,53,31,14,12,0,5,14,30,35,39,32,28,44,53,48,40,22,0,0,10,14,44,46,48,53,51,53,31,14,12,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,9,18,44,52,47,29,28,32,22,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,19,37,36,
51,47,28,12,11,27,50,50,27,11,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,5,14,30,51,41,43,25,35,53,47,28,21,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,42,37,44,53,50,44,29,28,27,45,51,38,14,12,0,0,1,37,44,50,53,36,39,32,43,12,0,0,0,0,0,0,0,0,4,14,30,51,36,37,35,53,45,39,17,0,0,2,14,30,51,45,33,40,46,38,45,31,29,35,50,51,36,15,1,0,0,4,16,36,52,49,35,34,39,30,53,51,36,37,3,19,37,36,51,47,28,12,0,0,10,39,45,
53,44,13,9,0,0,0,26,15,27,45,52,30,39,24,3,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,12,28,47,52,44,13,0,0,0,0,0,0,3,37,36,51,47,28,21,22,40,49,53,35,13,0,0,4,15,36,52,31,28,35,50,47,29,22,39,45,54,45,39,22,0,0,0,0,17,29,45,53,35,13,0,0,0,0,0,0,0,0,0,0,22,33,49,50,27,11,0,0,23,33,49,52,44,18,25,35,53,48,39,10,20,38,51,47,28,12,0,0,0,19,37,36,51,47,28,37,36,51,47,28,12,0,19,37,36,51,47,28,13,33,46,51,47,33,18,4,0,19,37,36,51,
53,49,49,49,49,31,16,6,19,37,36,51,30,14,10,29,46,51,51,31,15,1,8,16,38,51,38,16,8,12,28,47,52,44,13,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,19,37,36,51,47,28,12,20,36,51,45,39,22,0,0,21,17,37,28,33,44,30,51,48,40,22,0,0,21,17,37,28,33,44,30,51,48,40,22,0,5,14,30,53,45,46,46,50,52,38,14,12,0,0,21,17,37,28,33,44,30,51,48,40,22,0,0,0,0,0,25,35,53,49,33,22,
0,0,0,0,0,0,24,27,50,53,47,47,27,11,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,5,14,30,51,41,43,25,35,53,47,28,21,0,19,37,36,51,47,28,21,11,27,50,50,27,11,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,0,22,29,38,53,50,47,46,49,52,46,40,25,9,0,0,0,10,34,31,50,52,45,46,27,37,8,0,0,0,0,0,0,0,4,14,30,51,36,37,44,52,47,29,21,0,0,3,18,35,49,51,48,45,50,35,36,53,45,49,51,
50,35,25,0,0,0,0,24,33,45,51,49,46,45,31,47,51,36,37,3,19,37,36,51,47,28,12,0,12,37,35,50,49,33,11,0,0,3,22,14,44,48,53,38,29,25,12,18,24,7,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,12,28,47,52,44,13,0,0,0,0,0,0,3,37,36,51,47,28,21,22,40,49,53,35,13,0,0,0,11,40,47,50,46,48,50,44,37,5,20,31,51,31,20,4,0,0,0,0,10,39,48,53,27,24,0,42,17,24,13,13,24,10,4,0,10,39,48,53,27,24,0,0,22,40,49,52,41,43,13,35,53,48,40,22,14,30,54,46,32,12,0,0,0,
19,37,36,46,46,28,15,31,51,46,28,12,0,19,37,36,46,46,28,12,18,33,46,46,46,40,25,9,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,30,14,5,43,44,46,46,31,15,1,8,16,38,46,38,16,21,10,29,45,53,35,13,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,19,37,36,46,46,28,12,24,33,46,46,44,37,1,4,20,28,18,22,23,18,33,50,50,33,23,0,4,20,28,18,22,23,18,33,50,50,33,23,0,5,14,44,30,
45,48,49,45,31,39,25,7,0,4,20,28,18,22,23,18,33,50,50,33,23,0,0,0,0,0,25,35,47,46,33,22,0,0,0,0,0,0,21,32,41,46,46,46,27,11,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,5,14,30,51,36,37,43,44,52,47,28,21,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,0,3,25,39,31,45,50,48,46,41,32,22,0,0,0,0,3,24,29,41,46,53,51,47,36,40,18,4,0,0,0,0,7,11,34,44,30,46,45,47,29,26,0,0,
0,0,0,0,0,4,14,30,46,36,37,41,51,30,14,5,0,0,0,21,14,44,47,49,45,41,14,29,31,48,53,51,30,28,26,0,0,0,0,5,37,27,46,48,45,36,29,47,51,36,37,3,19,37,36,46,46,28,12,22,16,27,47,51,38,32,21,0,0,23,29,36,49,50,36,29,24,25,14,27,20,4,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,12,28,47,52,44,13,0,0,0,0,0,0,3,37,36,46,46,28,21,23,33,49,53,35,25,0,0,0,4,37,27,46,48,45,41,32,10,12,14,30,50,35,13,9,0,0,0,0,10,39,48,53,35,25,22,37,29,27,35,35,27,
39,15,11,22,40,48,50,27,24,0,0,17,39,45,51,31,16,37,41,53,50,27,25,29,47,51,30,14,5,0,0,0,7,13,34,34,34,29,33,47,51,38,14,5,0,7,13,34,34,34,20,25,13,37,34,34,34,34,25,0,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,37,13,13,32,34,34,34,18,3,1,43,34,34,34,43,32,29,44,50,50,27,24,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,7,13,34,34,34,20,13,13,20,34,34,34,
43,1,5,14,30,35,39,32,28,44,53,48,40,22,0,5,14,30,35,39,32,28,44,53,48,40,22,0,9,22,18,14,29,40,35,41,20,24,6,0,0,5,14,30,35,39,32,28,44,53,48,40,22,0,0,0,0,19,11,34,35,41,14,10,0,0,0,0,0,0,19,22,15,35,41,34,32,22,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,4,20,38,51,38,28,29,30,51,47,28,21,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,0,8,24,20,40,38,40,29,15,22,9,
0,0,0,0,0,19,22,37,33,49,52,44,20,24,4,0,0,0,0,0,0,7,10,43,14,34,27,49,33,22,0,0,0,0,0,0,0,8,37,34,34,34,20,38,52,44,43,19,0,0,0,0,10,15,29,40,39,16,22,18,28,44,48,46,40,25,3,0,0,0,0,0,12,43,28,40,39,14,29,47,51,36,37,3,7,13,34,34,34,20,15,39,41,48,52,46,40,25,7,0,0,23,29,30,52,49,41,39,39,27,31,30,20,4,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,12,28,47,52,44,13,0,0,0,0,0,0,7,13,34,34,34,20,28,29,36,52,49,33,11,0,0,0,0,
12,43,28,40,39,16,22,24,24,40,48,47,29,10,0,0,0,0,0,21,28,46,51,31,29,39,44,46,50,50,50,50,45,36,33,29,44,53,48,40,22,0,0,2,14,30,51,45,33,40,46,38,45,31,29,35,50,52,41,37,19,0,0,0,0,8,12,12,20,38,48,51,49,35,18,3,0,0,8,12,12,12,17,29,44,44,39,10,12,12,1,0,0,8,12,17,29,44,44,39,10,12,4,0,0,8,12,12,17,29,44,44,39,10,12,12,8,0,0,4,12,12,12,10,39,45,50,51,46,29,17,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,
40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,8,12,12,17,29,44,44,39,10,12,12,4,0,5,14,30,53,45,46,46,50,52,38,14,12,0,5,14,30,53,45,46,46,50,52,38,14,12,0,0,0,3,17,29,29,39,46,28,12,0,0,0,5,14,30,53,45,46,46,50,52,38,14,12,0,0,0,0,17,29,29,39,46,28,12,0,0,0,0,0,0,17,29,29,39,46,28,12,7,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,19,37,44,53,50,46,46,
31,52,47,28,21,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,0,0,7,18,44,41,28,40,24,0,0,0,0,0,0,0,0,0,22,33,49,52,44,13,0,0,0,0,0,0,0,0,0,0,3,17,39,41,48,40,22,0,0,0,0,0,0,0,0,4,12,12,22,39,45,45,39,23,0,0,0,0,0,0,4,17,22,18,14,39,44,30,49,31,39,13,4,0,0,0,0,0,0,0,1,21,22,10,21,29,47,51,36,37,3,0,8,12,12,20,35,36,47,53,49,31,39,13,4,0,0,0,1,25,39,30,53,53,48,48,50,52,30,20,4,0,0,0,0,25,35,53,49,33,22,0,0,0,0,
0,0,0,12,28,47,52,44,13,0,0,0,0,0,0,0,6,12,12,12,11,27,45,53,52,38,32,21,0,0,0,0,0,1,21,22,10,5,24,27,27,31,52,36,16,4,0,0,0,0,0,6,37,44,50,50,45,48,53,46,41,27,33,44,30,50,48,45,49,52,31,14,12,0,0,3,18,35,49,51,48,45,50,35,36,53,45,49,51,45,40,11,0,0,0,0,0,0,0,4,20,38,48,46,35,20,21,0,0,0,0,0,0,0,24,27,50,44,37,4,0,0,0,0,0,0,0,24,27,50,44,37,4,0,0,0,0,0,0,0,24,27,50,44,37,4,0,0,0,0,0,0,0,0,0,10,39,48,45,38,
33,18,1,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,0,0,24,27,50,44,37,4,0,0,0,0,5,14,44,30,45,48,48,45,31,39,25,7,0,5,14,44,30,45,48,48,45,31,39,25,7,0,0,0,0,17,39,47,45,31,14,2,0,0,0,5,14,44,30,45,48,48,45,31,39,25,7,0,0,0,0,17,39,47,45,31,14,2,0,0,0,0,0,0,17,39,47,45,31,14,2,0,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,3,25,
39,31,45,48,48,46,41,32,22,0,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,0,0,9,13,35,47,47,27,24,0,0,0,0,0,0,0,0,0,22,33,49,50,44,13,0,0,0,0,0,0,0,0,0,0,0,17,39,48,36,14,12,0,0,0,0,0,0,0,0,0,0,12,15,41,50,44,15,4,0,0,0,0,0,0,0,0,42,37,44,45,45,31,27,14,11,8,0,0,0,0,0,0,0,0,0,0,0,0,21,29,47,50,36,37,3,0,0,0,3,18,35,49,46,36,27,14,11,
6,0,0,0,0,0,6,25,29,36,45,50,49,47,41,39,18,1,0,0,0,0,25,35,50,49,33,22,0,0,0,0,0,0,0,12,28,47,50,44,13,0,0,0,0,0,0,0,0,0,0,0,11,27,48,47,31,39,25,7,0,0,0,0,0,0,0,0,0,0,24,27,50,53,46,40,11,0,0,0,0,0,0,0,22,34,36,45,49,46,44,29,37,11,11,18,28,35,30,48,45,31,39,24,9,0,0,0,21,14,44,47,49,45,41,14,29,31,48,48,46,27,37,4,0,0,0,0,0,0,0,8,15,39,39,34,37,26,0,0,0,0,0,0,0,1,37,41,38,32,17,0,0,0,0,0,0,0,1,37,
41,38,32,17,0,0,0,0,0,0,0,1,37,41,38,32,17,0,0,0,0,0,0,0,0,0,0,17,29,39,29,14,25,5,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,1,37,41,38,32,17,0,0,0,0,0,9,22,18,14,29,40,40,29,20,24,6,0,0,9,22,18,14,29,40,40,29,20,24,6,0,0,0,0,0,21,20,29,39,14,11,9,0,0,0,9,22,18,14,29,40,40,29,20,24,6,0,0,0,0,0,21,20,29,39,14,11,9,
0,0,0,0,0,0,21,20,29,39,14,11,9,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,0,8,24,20,29,40,40,29,15,22,9,0,0,0,0,0,0,21,15,29,29,32,23,0,0,0,0,0,0,0,0,0,22,39,33,33,33,13,0,0,0,0,0,0,0,0,0,0,0,17,29,33,14,23,0,0,0,0,0,0,0,0,0,0,0,10,39,33,33,28,22,0,0,0,0,0,0,0,0,0,0,10,28,27,39,20,25,12,9,
0,0,0,0,0,0,0,0,0,0,0,0,0,21,28,33,33,33,43,3,0,0,0,0,10,29,33,28,16,25,12,0,0,0,0,0,0,0,0,1,23,20,39,33,33,29,15,23,6,0,0,0,0,0,25,33,33,33,39,22,0,0,0,0,0,0,0,12,32,33,33,33,13,0,0,0,0,0,0,0,0,0,0,0,23,39,39,29,20,24,42,0,0,0,0,0,0,0,0,0,0,0,24,27,35,27,39,18,4,0,0,0,0,0,0,0,7,22,20,29,40,29,37,22,6,0,0,3,26,18,34,40,39,14,24,42,0,0,0,0,0,10,15,29,40,39,16,22,11,14,39,40,28,43,12,0,0,0,0,0,
0,0,0,0,21,10,10,12,1,0,0,0,0,0,0,0,0,7,25,20,20,24,7,0,0,0,0,0,0,0,7,25,20,20,24,7,0,0,0,0,0,0,0,7,25,20,20,24,7,0,0,0,0,0,0,0,0,0,0,3,10,10,17,2,7,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,7,25,20,20,24,7,0,0,0,0,0,0,0,3,5,26,22,22,17,5,9,0,0,0,0,0,3,5,26,22,22,17,5,9,0,0,0,0,0,0,0,
5,21,17,12,7,0,0,0,0,0,0,3,5,26,22,22,17,5,9,0,0,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,0,0,0,4,26,17,12,7,0,0,0,0,0,0,0,0,0,8,23,23,23,23,12,0,0,0,0,0,0,0,0,0,0,0,1,22,23,21,9,0,0,0,
0,0,0,0,0,0,0,0,1,23,23,23,22,7,0,0,0,0,0,0,0,0,0,0,3,23,11,10,5,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,22,23,23,23,21,0,0,0,0,0,1,11,11,21,4,0,0,0,0,0,0,0,0,0,0,0,0,5,10,23,23,26,4,0,0,0,0,0,0,0,5,23,23,23,23,8,0,0,0,0,0,0,0,7,22,23,23,23,12,0,0,0,0,0,0,0,0,0,0,0,42,10,10,26,5,0,0,0,0,0,0,0,0,0,0,0,0,0,12,25,25,25,22,6,0,0,0,0,0,0,0,0,0,0,5,17,22,26,8,0,0,0,0,0,
0,19,21,22,10,12,9,0,0,0,0,0,0,0,4,17,22,10,4,0,9,12,10,22,21,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,4,7,0,0,0,0,0,0,0,0,0,3,4,4,7,0,0,0,0,0,0,0,0,0,3,4,4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,4,7,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,4,5,4,3,0,0,0,0,0,19,19,19,19,19,19,19,19,9,0,0,0,0,0,0,9,1,4,5,4,1,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,4,5,4,3,0,0,0,0,0,19,19,19,7,0,0,3,19,19,19,3,0,0,0,0,0,0,
9,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,0,0,19,19,19,19,9,0,0,0,19,19,19,19,7,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,3,0,0,0,0,19,19,19,3,
0,0,0,3,19,19,19,0,0,0,0,0,1,4,5,4,1,0,0,0,0,0,0,0,0,0,0,0,9,19,19,19,19,19,19,19,19,0,0,0,0,0,0,9,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,5,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,2,4,0,0,0,0,0,0,0,0,0,4,2,2,4,0,0,0,0,0,0,0,0,0,0,0,5,11,37,20,14,20,18,22,8,0,0,11,37,37,37,37,37,37,37,37,24,7,0,0,0,0,10,13,37,20,14,20,37,25,12,0,0,0,0,11,37,37,37,37,37,37,37,37,37,37,11,9,0,0,0,0,0,0,0,0,
0,0,0,5,11,37,20,14,20,18,22,8,0,0,11,37,37,37,18,4,5,18,37,37,37,18,4,0,0,0,0,3,25,37,37,37,37,37,37,37,37,23,0,0,0,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,0,0,0,11,37,37,37,37,24,7,0,10,37,37,37,37,13,8,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,
1,43,14,14,16,14,14,25,0,0,0,0,0,0,12,37,37,37,37,37,37,37,37,43,2,0,0,22,37,37,37,37,12,0,2,43,37,37,37,11,0,0,21,25,37,20,14,20,37,11,5,0,0,0,0,0,0,0,0,3,25,37,37,37,37,37,37,37,37,23,0,0,0,0,3,25,37,37,37,37,37,37,37,37,23,0,0,0,0,0,0,0,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,0,0,0,0,0,0,0,0,0,0,21,13,16,14,20,37,24,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,17,10,12,0,0,0,0,0,11,37,37,37,37,37,43,25,17,3,0,
0,0,0,0,0,0,0,0,12,10,10,1,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,0,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,8,22,23,23,23,23,23,23,23,10,0,0,0,0,0,0,0,1,21,22,22,10,12,3,0,0,0,0,0,0,0,0,0,12,25,37,20,14,16,13,26,7,0,0,0,0,0,0,0,9,17,10,17,9,0,0,0,0,0,0,4,37,14,14,15,12,0,0,0,0,0,0,0,4,37,14,14,15,12,0,0,0,0,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,19,37,36,36,36,36,36,36,36,36,16,6,0,0,0,4,20,35,
36,38,30,38,36,27,20,17,0,0,19,37,36,36,36,36,36,36,36,36,36,36,15,1,0,0,0,0,0,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,19,37,36,36,36,28,12,18,33,36,36,36,28,21,0,0,0,0,4,20,36,36,36,36,36,36,36,41,43,3,0,0,0,19,37,36,36,36,28,12,12,32,36,36,36,15,1,19,37,36,36,36,28,12,12,32,36,36,36,15,1,0,0,0,19,37,36,36,36,36,14,2,3,18,44,36,36,36,34,12,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,
0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,22,33,36,36,36,36,36,36,36,39,10,0,7,18,35,36,36,27,13,19,25,33,36,36,44,37,3,7,18,27,36,38,30,38,41,33,43,4,0,0,0,0,0,0,0,4,20,36,36,36,36,36,36,36,41,43,3,0,0,0,4,20,36,36,36,36,36,36,36,41,43,3,0,0,0,0,0,0,0,19,37,36,36,36,28,12,12,32,36,36,36,15,1,0,0,0,0,0,0,0,0,0,0,26,20,35,31,30,38,41,33,16,17,0,
0,0,0,0,0,0,0,0,0,0,0,0,22,29,39,16,4,0,0,0,19,37,36,36,36,36,36,41,27,28,24,7,0,0,0,0,0,0,0,4,16,39,39,23,0,42,37,39,39,39,37,19,0,0,0,0,0,0,0,0,0,42,37,39,39,39,37,19,0,0,0,0,0,0,0,22,39,33,33,33,33,33,33,33,14,5,0,0,0,0,7,10,37,28,40,33,39,14,13,21,0,0,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,0,21,32,39,32,21,0,0,0,0,0,0,5,20,44,46,27,13,1,0,0,0,0,0,0,5,20,44,46,27,13,1,0,0,0,0,0,
0,0,12,16,35,45,52,53,49,50,53,36,37,3,19,37,36,51,51,52,52,52,52,31,16,6,0,0,0,4,20,38,50,48,48,49,51,49,44,37,6,0,19,37,36,51,51,52,52,52,52,51,51,31,15,1,0,0,0,0,0,0,0,0,0,12,16,35,45,52,53,49,50,53,36,37,3,19,37,36,51,47,28,13,40,46,52,30,39,24,3,0,0,0,0,4,20,30,51,51,52,52,51,51,41,43,3,0,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,19,37,36,51,51,48,40,23,12,32,30,51,51,46,34,12,0,0,0,
0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,22,33,49,51,52,52,52,52,48,39,10,0,0,26,32,38,51,30,29,10,32,38,51,30,28,10,0,7,18,41,50,48,48,53,52,47,40,11,0,0,0,0,0,0,0,4,20,30,51,51,52,52,51,51,41,43,3,0,0,0,4,20,30,51,51,52,52,51,51,41,43,3,0,0,0,0,0,0,0,19,37,36,
51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,0,0,0,0,0,42,37,35,49,30,50,38,47,48,44,20,12,0,0,0,0,0,0,0,0,0,0,1,10,13,33,30,20,23,4,0,0,19,37,36,51,51,52,52,52,50,38,34,26,0,0,0,0,2,22,22,22,29,46,39,22,0,8,16,38,48,31,15,1,0,0,0,0,0,0,0,0,0,8,16,38,48,31,15,1,0,0,0,0,0,0,0,23,33,41,41,41,41,41,41,30,14,5,0,0,0,19,24,28,44,46,48,48,45,30,27,20,10,0,0,0,0,0,21,20,41,48,51,50,50,52,50,31,29,22,0,0,0,0,0,0,21,29,45,29,
21,0,0,0,0,0,0,0,17,20,44,30,29,22,0,0,0,0,0,0,0,17,20,44,30,29,22,0,0,0,0,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,19,37,36,51,47,41,41,41,41,41,16,6,0,0,0,4,20,36,27,40,39,27,46,54,46,32,12,0,19,37,36,51,47,41,41,41,41,46,51,31,15,1,0,0,0,0,0,0,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,19,37,36,51,47,28,39,30,51,30,39,25,1,0,0,0,0,0,4,20,30,51,30,41,41,45,51,41,43,3,0,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,
47,28,12,12,32,46,51,31,15,1,0,0,0,19,37,36,51,52,52,41,37,11,33,48,53,54,46,34,12,0,0,0,0,0,0,12,37,34,34,14,34,34,25,12,0,0,0,0,0,0,10,37,34,34,14,34,34,25,12,0,0,0,0,0,0,0,12,37,34,34,14,34,34,25,9,0,0,0,0,0,0,0,12,37,34,34,14,34,34,25,9,0,0,0,0,0,12,37,34,34,14,34,34,25,3,0,0,0,0,0,22,33,49,53,41,41,41,41,41,39,10,0,0,7,25,33,45,53,44,37,35,50,48,27,13,3,0,7,18,41,27,39,40,44,50,52,44,18,7,0,0,0,0,0,0,4,20,30,51,30,
41,41,45,51,41,43,3,0,0,0,4,20,30,51,30,41,41,45,51,41,43,3,0,0,0,0,0,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,0,0,0,0,0,12,34,36,36,36,48,39,39,30,48,27,13,7,0,0,0,0,0,0,0,0,5,18,29,35,36,30,44,39,37,5,0,19,37,36,51,47,44,44,30,52,50,27,24,0,0,0,9,13,40,40,40,33,30,14,12,0,8,16,38,51,31,15,5,12,4,0,0,0,0,0,0,0,8,16,38,51,31,15,1,42,42,42,42,0,0,0,23,33,44,43,43,43,43,14,30,14,5,0,0,9,11,29,38,47,44,33,40,
27,36,48,41,20,21,0,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,0,3,10,43,29,45,29,15,25,4,0,0,0,0,0,4,18,20,39,39,43,12,0,0,0,0,0,0,0,11,20,39,39,18,4,0,0,0,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,19,37,36,51,47,28,43,43,43,43,11,9,0,0,0,19,13,15,24,22,10,37,36,51,46,28,12,0,19,37,36,51,47,28,43,43,32,46,51,31,15,22,22,22,26,42,0,0,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,19,37,36,51,47,29,30,52,46,33,13,8,0,0,0,0,0,0,4,
20,30,51,30,14,29,45,51,41,43,3,0,0,0,19,37,36,51,47,28,22,22,32,46,51,31,15,1,19,37,36,51,47,28,22,22,32,46,51,31,15,1,0,0,0,19,37,36,51,30,47,46,28,15,36,50,44,52,46,34,12,0,0,0,0,0,21,14,33,41,38,30,38,36,33,20,22,9,0,0,0,4,20,35,36,38,30,38,36,27,20,17,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,12,14,27,36,30,30,31,35,28,25,6,0,0,0,5,22,33,49,53,35,43,43,43,43,18,5,0,0,0,2,16,41,53,
45,40,47,52,36,20,12,0,0,0,11,20,25,22,22,29,45,52,41,43,7,0,0,0,0,0,0,4,20,30,51,30,14,29,45,51,41,43,3,0,0,0,4,20,30,51,30,14,29,45,51,41,43,22,22,22,21,1,0,0,0,19,37,36,51,47,28,22,22,32,46,51,31,15,22,22,22,26,42,0,0,0,0,0,1,24,15,28,30,46,28,25,33,48,31,20,5,0,0,0,0,0,0,0,6,18,33,46,53,53,53,53,47,28,21,0,19,37,36,51,47,28,18,33,49,53,35,25,0,0,0,3,18,44,49,46,44,41,37,1,0,8,16,38,51,31,15,14,32,20,24,1,0,0,0,0,0,8,16,38,
51,31,15,10,15,15,15,15,23,0,0,23,33,44,13,7,7,5,14,30,14,5,0,0,21,14,31,30,39,37,37,37,13,16,27,45,35,18,19,0,0,12,14,30,51,47,39,25,11,32,38,51,45,39,10,0,0,7,11,28,44,30,49,30,36,39,10,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,0,19,37,36,36,36,28,12,0,0,0,0,0,10,39,45,51,36,16,5,0,0,2,24,26,0,19,37,36,51,47,40,40,40,40,39,16,17,0,0,0,0,19,10,14,34,28,40,30,53,36,16,4,0,19,37,36,51,47,28,12,12,32,46,51,31,40,40,40,40,29,43,2,0,0,0,
10,39,45,51,31,16,5,0,0,2,24,26,0,19,37,36,51,47,31,52,45,33,39,16,17,0,0,0,0,0,0,5,14,30,51,30,20,29,45,51,41,43,3,0,0,0,19,37,36,51,47,40,40,40,40,46,51,31,15,1,19,37,36,51,47,40,40,40,40,46,51,31,15,1,0,0,0,19,37,36,51,30,36,49,27,29,47,46,44,52,46,34,12,0,0,0,0,0,17,39,48,52,50,49,53,52,48,41,32,10,0,0,0,4,20,38,50,48,48,49,51,49,44,37,6,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,21,
29,45,53,50,50,52,50,30,39,25,7,0,9,13,39,33,49,53,35,39,39,39,13,7,0,0,0,0,0,22,29,46,52,30,52,47,39,23,0,0,0,0,7,17,14,34,28,35,49,48,27,24,0,0,0,0,0,0,0,4,20,30,51,30,20,29,45,51,41,43,3,0,0,0,4,20,30,51,30,20,29,45,51,41,40,40,40,40,28,18,4,0,0,19,37,36,51,47,40,40,40,40,46,51,31,40,40,40,40,29,43,2,0,0,0,0,0,0,21,29,47,38,20,12,14,30,47,29,21,5,21,12,3,0,0,0,22,39,46,51,47,44,30,44,46,28,21,0,19,37,36,51,47,28,13,33,49,50,27,24,
0,0,3,13,15,44,52,46,38,27,15,23,0,8,16,38,51,31,27,38,46,31,39,11,0,0,0,0,0,8,16,38,51,31,15,20,41,31,31,27,13,0,0,23,33,44,13,0,0,5,14,30,14,5,0,0,25,27,45,40,37,33,36,41,33,44,39,44,30,32,21,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,17,34,31,53,53,49,47,48,39,10,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,19,37,36,51,53,49,49,49,49,45,44,16,5,0,0,0,0,22,
33,46,47,48,48,44,28,23,0,0,19,37,36,51,47,28,12,12,32,46,54,49,49,49,49,48,46,27,13,3,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,19,37,36,51,52,52,54,50,49,45,44,16,5,0,0,0,0,0,12,32,46,51,31,16,29,45,51,41,43,3,0,0,0,19,37,36,51,53,49,49,49,49,50,51,31,15,1,19,37,36,51,53,49,49,49,49,50,51,31,15,1,0,0,0,19,37,36,51,30,27,49,31,35,50,41,44,52,46,34,12,0,0,0,0,0,17,39,46,41,27,33,35,30,52,50,44,37,8,0,0,4,20,36,27,40,39,27,46,54,46,32,12,0,0,3,
13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,21,29,46,44,33,27,41,49,52,38,34,26,0,7,43,41,48,52,51,48,48,48,44,18,9,0,0,0,0,0,1,18,35,49,54,50,44,37,8,0,0,0,0,0,22,40,46,47,49,38,27,16,12,0,0,0,0,0,0,0,5,14,30,51,38,20,29,45,51,41,43,3,0,0,0,5,14,30,51,38,20,29,45,51,49,49,49,49,48,46,33,25,9,0,19,37,36,51,53,49,49,49,49,50,54,49,49,49,49,48,46,27,13,3,0,0,0,0,0,22,40,48,41,43,8,16,38,48,39,
22,20,28,14,13,4,0,19,37,41,53,49,27,33,30,20,39,28,21,0,19,37,36,51,47,44,44,30,53,38,28,26,0,0,4,20,31,31,52,47,45,31,44,18,9,8,16,38,51,31,45,45,53,52,36,20,4,0,0,0,0,8,16,38,51,31,16,44,49,45,27,37,12,0,0,23,33,44,13,0,0,5,14,30,14,5,0,8,16,36,36,15,40,47,30,31,36,48,39,39,45,40,22,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,9,13,35,50,50,41,45,29,27,39,10,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,
0,22,33,49,52,44,13,9,0,0,0,0,0,0,19,37,36,51,49,47,47,47,49,54,45,40,22,0,0,0,0,22,33,46,46,45,53,45,44,15,4,0,19,37,36,51,47,28,12,12,32,46,51,45,47,47,47,53,51,30,14,12,0,0,22,33,49,52,44,13,9,0,0,0,0,0,0,19,37,36,51,49,47,47,47,49,54,45,40,22,0,0,0,0,0,21,29,47,51,36,37,29,45,51,41,43,3,0,0,0,19,37,36,51,49,47,47,47,47,49,51,31,15,1,19,37,36,51,49,47,47,47,47,49,51,31,15,1,0,0,0,19,37,36,51,30,34,30,45,38,48,33,44,52,46,34,12,0,0,0,
0,0,17,39,29,37,24,23,13,29,30,51,47,29,17,0,0,19,13,15,24,22,10,37,36,51,46,28,12,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,21,29,29,43,11,11,20,41,53,50,27,25,0,7,43,41,45,53,51,45,45,45,44,18,9,0,0,0,0,0,19,13,27,49,54,50,35,43,42,0,0,0,0,0,22,40,46,46,48,50,30,40,11,0,0,0,0,0,0,0,12,32,46,51,31,15,29,45,51,41,43,3,0,0,0,12,32,46,51,31,15,29,45,51,47,47,47,47,53,51,31,20,5,0,19,37,36,
51,49,47,47,47,47,49,51,45,47,47,47,53,51,30,14,12,0,0,0,0,0,25,35,50,27,25,4,20,38,45,29,32,36,47,30,33,25,9,5,14,30,51,30,14,33,30,20,24,25,42,0,19,37,36,51,51,52,52,53,44,29,24,3,0,0,4,20,38,49,51,51,49,49,44,18,9,8,16,38,51,49,35,39,36,52,47,28,21,0,0,0,0,8,16,38,51,31,35,48,45,27,37,12,0,0,0,23,33,44,13,0,0,5,14,30,14,5,0,12,32,46,27,37,41,47,39,20,41,48,39,32,46,27,24,0,0,22,33,49,52,44,13,0,0,23,33,49,52,44,13,0,1,37,36,51,46,29,45,29,
18,37,12,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,22,39,48,51,36,16,5,0,0,0,0,0,0,19,37,36,51,47,28,28,28,44,53,53,35,25,0,0,0,3,17,14,32,32,39,31,51,47,39,10,0,19,37,36,51,47,28,12,12,32,46,51,31,28,28,29,31,51,45,39,17,0,0,22,39,48,51,41,37,1,0,0,19,21,4,0,19,37,36,51,47,28,28,28,44,53,53,35,25,0,0,0,0,0,22,40,48,53,44,13,29,45,51,41,43,3,0,0,0,19,37,36,51,47,28,28,28,28,46,51,31,15,1,19,37,36,51,
47,28,28,28,28,46,51,31,15,1,0,0,0,19,37,36,51,30,14,44,53,53,30,14,44,52,46,34,12,0,0,0,0,5,25,13,13,13,13,13,13,18,35,53,50,27,24,0,0,0,19,10,14,34,28,40,30,53,36,16,4,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,10,39,45,51,36,39,39,39,39,35,53,50,27,24,0,0,1,23,43,32,32,32,32,39,45,51,36,37,1,9,13,39,33,49,53,35,39,39,39,13,9,0,0,0,0,0,22,29,30,51,47,52,46,39,23,0,0,0,0,0,17,14,32,28,27,45,52,36,15,6,0,0,0,0,0,0,21,29,47,51,41,
37,29,45,51,41,43,3,0,0,0,21,29,47,51,41,37,29,45,51,41,28,28,29,30,51,47,29,21,0,19,37,36,51,47,28,28,28,28,46,51,31,28,28,29,31,51,45,39,17,0,0,0,12,11,37,41,48,40,22,21,28,46,46,32,44,48,44,30,31,20,4,12,32,46,51,31,15,33,30,20,4,0,0,0,19,37,36,51,47,44,36,49,50,36,20,2,0,0,8,16,40,44,52,52,44,40,40,13,9,8,16,38,51,30,14,25,35,53,45,29,26,0,0,0,0,8,16,38,51,31,45,48,35,15,12,0,0,0,0,23,33,44,13,0,0,5,14,30,14,5,0,21,28,47,33,15,31,30,20,25,
35,48,39,34,46,27,11,0,0,22,40,48,52,41,43,19,0,24,27,50,53,27,25,0,6,16,31,51,30,29,45,29,21,42,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,12,32,30,51,45,33,18,23,23,23,23,5,0,19,37,36,51,47,28,21,23,33,49,53,35,25,0,0,4,18,13,26,2,12,25,35,53,49,33,22,0,19,37,36,51,47,28,12,12,32,46,51,31,15,21,18,44,52,48,39,10,0,0,12,32,46,51,46,29,23,12,26,13,28,13,7,19,37,36,51,47,28,21,23,33,49,53,35,25,0,0,0,1,21,37,
44,53,49,33,11,29,45,51,41,43,23,23,4,0,19,37,36,51,47,28,21,21,32,46,51,31,15,1,19,37,36,51,47,28,21,21,32,46,51,31,15,23,23,5,0,19,37,36,51,30,14,39,45,53,44,18,44,52,46,34,23,23,21,0,0,22,33,35,35,35,35,35,35,35,35,50,53,35,13,0,0,0,0,22,33,46,47,48,48,44,28,23,0,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,22,33,49,51,48,48,48,48,48,48,51,53,35,13,0,0,0,8,16,38,46,46,46,46,49,51,38,20,4,0,4,22,33,49,53,35,25,17,17,4,0,0,0,0,0,5,15,44,53,
45,33,47,53,41,20,2,0,0,0,0,0,2,2,12,20,30,51,30,14,23,23,12,0,0,4,23,18,27,49,53,35,13,29,45,51,41,43,3,0,4,23,18,27,49,53,35,13,29,45,51,41,43,21,37,36,51,45,29,21,0,19,37,36,51,47,28,21,21,32,46,51,31,15,21,18,44,52,48,39,10,0,0,21,20,33,27,38,46,28,21,24,33,49,36,32,30,36,16,44,46,32,2,2,32,46,51,38,20,33,30,20,12,12,0,0,19,37,36,51,47,28,20,41,52,48,33,11,0,0,0,21,18,44,52,49,33,11,22,2,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,8,16,38,
51,50,51,38,29,23,9,0,0,0,0,23,33,44,13,0,0,5,14,30,14,5,0,12,28,47,33,15,31,30,14,13,35,48,39,33,45,39,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,1,15,36,51,30,29,45,29,11,25,4,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,43,35,50,51,46,35,33,33,33,33,24,0,19,37,36,51,47,28,12,22,40,49,53,35,25,0,0,21,29,27,29,14,20,28,36,52,48,40,22,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,13,44,52,48,39,10,0,0,
1,37,44,50,53,36,39,32,29,27,36,37,3,19,37,36,51,47,28,12,22,40,49,53,35,25,0,0,9,25,28,27,47,51,46,28,21,29,45,51,41,33,33,40,23,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,33,33,33,24,0,19,37,36,51,30,14,20,31,31,39,18,44,52,46,33,33,33,37,19,0,22,40,49,53,53,53,53,53,53,53,51,53,44,13,0,0,0,0,22,33,46,46,45,53,45,44,15,4,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,22,33,49,52,38,38,38,38,38,38,50,53,44,13,0,0,0,
8,16,38,50,50,50,50,53,51,30,20,4,0,0,22,33,49,53,35,28,28,15,4,0,0,0,0,9,24,40,45,53,41,16,44,50,48,33,25,7,0,0,0,0,0,0,4,20,30,51,30,33,33,33,18,9,0,23,40,35,46,51,48,40,22,29,45,51,41,43,3,0,23,40,35,46,51,48,40,22,29,45,51,41,43,3,43,41,51,45,29,21,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,13,44,52,48,39,10,0,9,13,35,48,45,50,31,32,25,14,31,49,27,40,48,35,43,44,30,14,5,4,16,31,51,47,39,33,30,20,20,14,12,0,19,37,36,51,47,28,23,40,45,52,36,15,
4,0,0,9,18,44,52,46,28,21,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,1,9,0,0,8,16,38,51,31,46,53,36,32,22,1,3,0,0,23,33,44,13,0,0,5,14,30,14,5,0,5,14,30,35,18,35,48,27,39,30,48,40,38,36,20,2,0,0,42,37,44,53,50,44,29,28,27,45,52,31,14,2,0,9,13,35,53,49,27,45,29,39,39,10,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,17,32,36,50,51,53,49,49,49,35,25,0,19,37,36,51,47,28,12,22,40,49,53,35,25,0,0,21,29,47,
47,30,30,46,53,53,31,14,12,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,13,44,52,48,39,10,0,0,0,10,34,31,50,52,45,46,47,49,36,37,3,19,37,36,51,47,28,12,22,40,49,53,35,25,0,0,7,43,41,49,51,49,44,37,21,29,45,54,50,49,49,27,11,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,54,50,49,49,35,25,0,19,37,36,51,30,14,24,15,15,43,18,44,52,53,49,49,36,37,19,0,10,39,45,51,41,44,44,44,44,44,53,53,35,25,0,0,0,3,17,14,32,32,39,31,51,47,39,10,0,0,22,
39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,22,39,48,52,41,20,20,20,20,27,50,53,35,25,0,0,0,12,15,33,33,33,33,33,45,51,31,16,6,0,0,22,33,49,52,47,47,46,32,2,0,0,0,0,21,14,31,52,46,29,22,28,30,51,30,34,17,0,0,0,0,0,0,4,20,30,51,50,49,49,44,18,9,0,11,27,50,52,48,41,20,21,29,45,51,41,43,3,0,11,27,50,52,48,41,20,21,29,45,51,41,43,3,43,41,51,45,29,21,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,13,44,52,48,39,10,0,19,37,36,38,27,50,50,38,35,36,50,36,14,27,
50,36,33,46,41,37,1,9,13,27,49,53,36,33,30,28,44,29,21,0,19,37,36,51,47,28,12,20,36,51,45,39,22,0,0,9,18,44,52,46,32,12,12,7,0,8,16,38,51,31,15,24,27,53,45,29,15,15,11,0,0,8,16,38,51,31,27,45,50,41,14,15,13,19,0,23,33,44,13,0,0,5,14,30,14,5,0,19,37,44,30,32,28,31,49,48,44,49,45,36,29,23,0,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,10,29,30,52,48,45,38,47,39,10,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,0,19,37,36,51,47,28,12,0,0,0,0,
0,0,9,23,14,35,31,46,46,53,53,35,25,0,19,37,36,46,46,28,21,11,33,49,53,35,25,0,0,21,29,38,47,48,49,48,46,36,29,24,9,0,19,37,36,46,46,28,12,12,32,46,46,31,15,21,43,41,52,48,39,10,0,0,0,7,11,28,44,46,48,49,45,30,35,43,3,19,37,36,46,46,28,21,11,33,49,53,35,25,0,0,7,43,41,45,30,35,20,26,21,29,46,46,46,53,50,27,11,0,19,37,36,46,46,28,12,26,28,47,51,36,37,1,19,37,36,46,46,28,12,12,32,46,46,46,53,53,35,25,0,19,37,36,46,30,14,5,1,1,19,18,44,46,46,48,51,36,37,19,
0,12,32,30,51,30,32,13,13,16,36,52,48,40,22,0,0,4,18,13,26,2,12,25,35,53,49,33,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,12,14,20,23,23,23,37,35,50,53,44,18,7,0,0,22,33,46,46,46,51,46,32,2,0,0,0,3,18,35,46,46,35,18,12,13,27,48,50,44,37,1,0,0,0,0,0,4,20,30,46,46,50,52,44,18,9,0,11,27,30,41,33,20,10,10,39,45,52,41,43,7,0,11,27,30,41,33,20,10,21,29,46,46,41,43,21,37,36,51,47,29,21,0,19,37,36,
46,46,28,12,12,32,46,46,31,15,21,43,41,52,48,39,10,0,9,25,27,47,49,46,35,46,48,45,36,28,43,41,46,30,48,38,29,22,0,0,21,14,36,50,53,46,46,46,47,29,21,0,3,37,36,51,45,29,17,24,33,46,46,44,37,1,0,9,18,44,52,47,29,28,32,22,0,8,16,38,51,31,15,24,27,53,48,31,31,36,37,3,0,8,16,38,51,31,15,35,49,49,31,31,20,4,0,23,33,44,13,0,0,5,14,30,14,5,0,0,23,39,47,44,16,32,33,33,40,33,39,20,23,3,0,0,0,0,3,24,29,41,46,48,48,52,36,39,18,4,0,0,0,19,25,39,36,45,53,48,
46,39,10,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,9,21,13,20,32,33,49,53,35,25,0,7,13,34,34,34,20,29,39,36,52,49,33,11,0,0,4,13,20,29,39,35,41,28,15,23,19,0,0,7,13,34,34,34,20,2,5,16,34,34,34,18,29,40,46,51,46,28,21,0,0,0,0,7,10,37,34,39,35,41,14,18,21,0,7,13,34,34,34,20,29,39,36,52,49,33,11,0,0,7,18,40,29,14,18,21,0,12,20,34,34,36,53,31,28,22,0,7,13,34,34,34,20,14,29,44,50,53,44,18,7,7,13,34,34,
34,20,2,5,16,34,34,41,53,38,29,23,0,7,13,34,34,34,37,8,0,0,0,25,34,34,27,48,46,33,18,3,0,1,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,21,29,27,29,14,20,28,36,52,48,40,22,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,21,29,41,40,34,28,27,47,51,47,39,22,0,0,0,10,32,34,34,41,52,30,14,2,0,0,0,3,18,34,34,34,14,21,20,29,27,48,51,36,15,42,0,0,0,0,0,42,37,34,34,39,45,52,44,18,9,0,22,32,14,37,24,17,32,
29,44,53,53,35,13,0,0,22,32,14,37,24,12,0,12,20,34,34,34,37,29,33,47,51,30,14,2,0,7,13,34,34,34,20,2,5,16,34,34,34,18,29,40,46,51,46,28,21,0,0,2,37,29,40,29,43,28,40,39,20,23,20,38,31,29,40,32,25,19,0,0,0,22,32,35,46,48,49,47,36,28,21,0,9,13,35,53,50,44,29,32,20,34,34,34,43,1,0,0,24,27,50,53,47,47,27,11,0,8,16,38,46,31,15,24,27,46,46,45,51,36,37,3,0,8,16,38,46,31,15,20,41,47,51,30,20,4,0,23,33,44,13,21,21,21,14,30,14,5,0,0,4,37,35,48,44,29,16,37,
14,33,30,34,17,0,0,0,0,0,0,19,22,15,28,39,27,48,47,33,13,1,0,0,0,0,6,11,20,29,45,40,34,37,12,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,0,0,7,4,22,33,49,53,35,25,0,0,8,12,12,12,11,27,45,53,52,38,32,21,0,0,0,7,17,29,29,39,46,28,12,0,0,0,0,0,8,12,12,12,2,0,0,5,12,12,12,37,36,48,52,50,44,37,6,0,0,0,0,0,0,17,29,29,39,46,28,12,0,0,0,8,12,12,12,11,27,45,53,52,38,32,21,0,0,0,2,22,17,
2,3,0,0,0,2,37,44,49,36,28,23,3,0,0,8,12,12,12,26,29,45,50,51,47,39,23,0,0,8,12,12,12,2,0,0,5,37,35,48,31,29,24,19,0,0,8,12,12,12,4,0,0,0,0,1,25,40,47,46,33,18,5,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,21,29,47,47,30,30,46,53,53,31,14,12,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,21,29,45,48,46,46,49,51,45,27,43,8,0,0,0,9,22,40,27,30,51,31,16,4,0,0,0,0,8,12,12,12,2,
2,32,46,49,51,48,33,25,0,0,0,0,0,0,0,4,12,17,39,45,52,44,18,9,0,7,12,5,1,0,22,40,45,50,51,46,29,10,0,0,7,12,5,1,0,0,0,0,2,12,12,12,15,31,48,51,49,35,43,19,0,0,8,12,12,12,2,0,0,5,12,12,12,37,36,48,52,50,44,37,6,0,0,0,8,17,22,26,1,21,22,10,5,21,28,46,44,18,22,21,7,0,0,0,0,9,17,43,34,33,30,29,15,23,3,0,0,22,39,46,51,50,45,39,17,12,12,12,4,0,0,0,23,33,46,46,46,46,27,11,0,1,43,34,34,34,18,23,34,34,34,30,51,36,37,3,0,1,43,34,
34,34,18,10,14,36,51,30,20,4,0,23,33,44,29,29,29,29,29,30,14,5,0,0,0,21,16,35,47,46,31,36,30,48,31,39,23,0,0,0,0,0,0,0,0,8,21,10,16,44,50,30,39,11,0,0,0,0,0,0,21,29,45,29,21,1,0,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,0,0,0,0,22,33,49,50,35,25,0,0,0,0,0,0,11,27,48,47,36,39,25,7,0,0,0,0,17,39,47,45,31,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,37,36,48,46,44,32,10,0,0,0,
0,0,0,0,17,39,47,45,31,14,2,0,0,0,0,0,0,0,11,27,48,47,36,39,25,7,0,0,0,0,0,0,0,0,0,0,19,13,33,45,36,34,23,7,0,0,0,0,0,0,0,26,29,45,45,30,27,43,4,0,0,0,0,0,0,0,0,7,25,40,47,31,29,11,3,0,0,0,0,0,0,0,0,0,0,0,0,22,28,38,46,40,18,4,0,0,0,0,19,24,29,41,46,48,48,47,31,40,18,4,0,0,0,21,29,38,47,48,49,48,46,36,29,24,9,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,21,
29,41,46,48,48,45,38,33,37,12,0,0,0,0,0,22,33,49,53,47,33,25,0,0,0,0,0,0,0,0,0,0,2,32,46,45,30,27,15,12,0,0,0,0,0,0,0,0,0,17,39,45,50,44,18,9,0,0,0,0,0,0,22,40,48,45,38,33,18,42,0,0,0,0,0,0,0,0,0,0,0,0,0,42,15,31,48,46,44,14,26,0,0,0,0,0,0,0,0,0,0,0,0,0,3,37,36,48,46,44,32,10,0,0,0,0,0,0,0,0,0,0,0,0,0,22,40,48,27,24,0,0,0,0,0,0,0,0,0,1,23,33,30,20,4,0,0,0,0,6,18,33,38,45,48,39,17,0,0,0,
0,0,0,7,18,44,36,32,34,34,32,22,0,0,4,12,12,12,8,3,12,12,14,30,48,36,37,3,0,0,4,12,12,12,8,19,37,36,48,30,20,4,0,23,33,38,38,38,38,38,38,38,14,5,0,0,0,0,21,37,39,44,36,31,41,27,14,24,6,0,0,0,0,0,0,0,0,0,0,0,17,14,27,27,27,37,19,0,0,0,0,0,21,29,35,29,21,0,0,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,0,19,37,36,46,46,28,12,0,0,0,0,0,0,0,0,0,0,0,22,40,33,33,33,24,0,0,0,0,0,0,11,39,39,29,20,11,42,0,0,0,0,0,21,
20,29,39,14,11,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,43,39,39,28,37,10,9,0,0,0,0,0,0,0,21,20,29,39,14,11,9,0,0,0,0,0,0,0,11,39,39,29,20,11,42,0,0,0,0,0,0,0,0,0,0,0,4,20,33,33,32,22,7,0,0,0,0,0,0,0,0,21,28,39,39,14,13,2,0,0,0,0,0,0,0,0,0,8,16,33,33,28,11,3,0,0,0,0,0,0,0,0,0,0,0,0,0,13,33,33,40,13,4,0,0,0,0,0,0,19,22,15,28,40,40,29,20,24,4,0,0,0,0,4,13,20,29,39,40,39,28,15,23,19,0,0,0,0,
0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,19,23,15,28,39,40,29,14,25,2,0,0,0,0,0,0,22,33,35,35,39,43,5,0,0,0,0,0,0,0,0,0,0,5,14,39,39,32,18,12,0,0,0,0,0,0,0,0,0,0,17,29,33,33,33,18,9,0,0,0,0,0,0,10,29,39,29,14,25,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,37,39,39,28,37,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,43,39,39,28,37,10,9,0,0,0,0,0,0,0,0,0,0,0,0,0,11,39,35,
39,22,0,0,0,0,0,0,0,0,0,0,23,40,27,20,4,0,0,0,0,0,5,25,14,39,39,28,26,0,0,0,0,0,0,8,20,35,27,25,12,12,12,7,0,0,0,0,0,0,0,0,0,4,20,40,40,40,43,3,0,0,0,0,0,0,0,3,43,40,40,40,16,4,0,26,16,20,20,20,20,20,20,20,43,6,0,0,0,0,0,8,22,18,15,16,37,24,21,9,0,0,0,0,0,0,0,0,0,0,0,0,0,10,11,11,11,21,0,0,0,0,0,0,1,11,25,11,1,0,0,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,0,7,13,34,34,34,20,2,0,0,0,0,
0,0,0,0,0,0,0,8,23,23,23,23,5,0,0,0,0,0,0,6,10,10,26,5,0,0,0,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,10,10,21,8,0,0,0,0,0,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,0,0,0,6,10,10,26,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,23,23,10,9,0,0,0,0,0,0,0,0,0,7,17,10,17,2,7,0,0,0,0,0,0,0,0,0,0,0,17,23,23,22,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,23,23,23,4,0,0,0,0,
0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,7,5,26,22,22,10,21,4,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,8,21,22,22,17,5,9,0,0,0,0,0,0,0,5,25,25,25,22,8,0,0,0,0,0,0,0,0,0,0,0,0,21,10,10,12,19,0,0,0,0,0,0,0,0,0,0,0,19,22,23,23,23,12,0,0,0,0,0,0,0,3,10,10,17,2,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,10,10,21,42,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,12,10,10,21,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,22,25,13,4,0,0,0,0,0,0,0,0,0,0,4,11,11,10,0,0,0,0,0,0,0,7,2,17,10,17,3,0,0,0,0,0,0,0,22,25,25,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,22,22,22,12,0,0,0,0,0,0,0,0,0,12,22,22,22,21,0,0,0,4,4,4,4,4,4,4,4,1,0,0,0,0,0,0,0,0,7,1,6,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,0,0,0,4,2,1,5,2,4,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,0,0,1,4,42,0,0,0,0,0,0,0,0,0,4,2,2,4,0,0,0,0,0,0,0,0,0,0,
0,5,2,2,42,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,5,2,2,42,0,0,0,0,1,2,2,2,3,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,4,2,1,5,2,4,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,5,2,4,0,5,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,4,0,5,2,6,0,0,
0,0,0,0,19,2,2,2,9,0,0,0,0,0,0,0,5,2,4,0,5,2,4,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,0,0,4,2,1,5,2,4,0,0,0,0,0,0,0,0,0,0,0,4,2,4,0,5,2,6,0,0,19,2,2,2,9,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,0,5,2,4,0,5,2,4,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,5,2,4,0,5,
2,4,0,0,0,0,0,0,0,5,2,4,0,5,2,4,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,3,0,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,0,0,0,10,16,14,37,20,32,43,42,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,
0,0,0,0,0,26,37,14,37,22,0,0,0,0,0,0,0,4,37,14,14,15,12,0,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,10,14,14,14,18,3,0,0,1,13,14,14,14,11,9,0,0,1,43,14,14,16,14,14,25,0,0,0,0,10,16,14,37,20,32,43,42,0,0,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,17,14,14,15,
22,14,14,16,2,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,0,24,14,14,14,22,0,0,0,0,0,0,17,14,14,15,22,14,14,16,2,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,0,0,10,16,14,37,20,32,43,42,0,5,10,10,10,10,10,12,0,5,15,14,16,17,14,14,18,19,0,24,14,14,14,22,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,0,17,
14,14,15,22,14,14,16,2,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,0,0,0,17,14,14,15,22,14,14,16,2,0,0,0,0,0,17,14,14,15,22,14,14,16,2,0,0,0,0,0,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,0,0,0,0,0,1,13,14,14,14,11,9,0,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,
0,0,0,4,15,41,30,35,40,38,20,4,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,0,12,20,44,30,41,14,26,0,0,0,0,0,0,5,20,44,46,27,13,1,0,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,5,37,44,46,27,43,19,0,0,23,29,30,30,31,32,10,0,0,4,20,38,38,29,46,41,43,7,0,0,4,15,41,30,35,40,38,20,4,0,0,0,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,0,7,18,39,
39,39,39,39,15,6,0,0,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,0,10,29,30,35,32,41,36,14,12,0,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,0,9,18,44,46,27,11,0,0,0,0,0,0,10,29,30,35,32,41,36,14,12,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,0,0,4,15,41,30,35,40,38,20,4,7,18,39,39,39,39,39,15,6,2,14,30,33,34,35,36,37,1,9,18,44,46,27,11,0,0,0,0,5,37,44,46,
27,43,19,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,0,10,29,30,35,32,41,36,14,12,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,10,29,30,35,32,41,36,14,12,0,0,0,0,0,10,29,30,35,32,41,36,14,12,0,0,0,0,0,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,0,0,0,0,0,0,23,29,30,30,31,32,10,0,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,0,5,37,44,46,27,43,19,0,0,4,20,38,38,29,46,41,43,7,0,
0,0,0,0,9,25,33,45,27,37,12,0,0,0,0,0,0,0,0,26,14,36,41,29,31,44,15,5,0,0,0,0,0,0,0,12,28,46,33,31,47,44,43,19,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,0,11,33,30,27,38,35,25,0,0,0,0,0,0,0,17,20,44,30,29,22,19,19,0,0,0,0,19,25,33,45,27,37,12,19,0,0,0,26,14,36,41,29,31,44,15,5,0,0,0,4,20,38,38,29,46,41,43,19,0,0,9,25,33,45,27,37,12,0,0,26,14,36,41,29,31,44,15,5,0,4,20,38,38,29,46,41,43,7,0,0,12,28,
46,33,31,47,44,43,19,19,0,0,0,0,3,19,19,19,25,33,45,27,37,12,19,19,0,0,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,0,19,25,40,47,36,30,29,23,0,0,0,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,19,18,27,46,46,30,40,24,19,0,0,0,0,19,18,44,46,27,11,19,19,0,0,0,0,19,25,40,47,36,30,29,23,0,0,0,0,19,26,14,36,41,29,31,44,15,5,0,0,0,26,14,36,41,29,31,44,15,5,0,0,0,0,0,0,12,28,46,33,31,47,44,43,19,
7,43,41,48,48,48,38,20,8,19,18,27,46,46,30,40,24,0,9,18,44,46,27,11,0,0,0,9,25,33,45,27,37,12,0,0,0,0,9,25,33,45,27,37,12,0,0,0,19,19,25,33,45,27,37,12,19,19,0,0,0,0,19,19,25,40,47,36,30,29,23,19,0,0,0,0,19,19,25,33,45,27,37,12,0,0,0,0,0,0,19,25,40,47,36,30,29,23,0,0,0,0,0,7,19,25,40,47,36,30,29,23,19,19,9,0,0,0,19,19,19,7,26,14,36,41,29,31,44,15,5,19,19,19,0,0,0,0,3,19,26,14,36,41,29,31,44,15,5,19,0,0,0,0,3,19,4,20,38,38,
29,46,41,43,19,19,0,0,0,0,3,19,19,25,33,45,27,37,12,19,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,4,18,39,39,37,43,12,0,0,0,0,0,0,0,0,10,29,39,14,37,28,39,14,12,0,0,0,0,0,0,0,12,14,39,20,14,39,20,17,0,0,0,0,0,0,0,0,42,37,34,34,20,34,34,25,9,0,0,0,0,0,0,0,0,25,27,36,37,44,44,18,9,0,0,0,0,0,11,37,37,20,39,39,37,37,37,23,0,0,11,37,37,39,39,37,37,37,37,23,0,0,11,29,39,14,37,28,39,14,37,23,0,0,11,37,34,34,20,34,34,37,37,23,0,
7,18,39,39,37,18,4,0,0,10,29,39,14,37,28,39,14,12,0,42,37,34,34,20,34,34,25,9,0,11,37,14,39,20,14,39,20,37,37,37,11,9,0,2,43,37,37,37,18,39,39,37,37,37,37,37,22,0,0,0,0,7,43,33,33,33,33,33,16,8,0,0,0,0,0,0,0,0,22,34,44,31,41,39,24,0,0,0,0,0,0,11,37,37,37,39,39,39,25,26,19,0,0,0,0,0,11,37,33,33,33,33,33,16,37,23,0,0,11,37,37,28,39,32,37,37,37,23,0,0,11,37,37,34,34,32,37,37,37,23,0,0,11,37,37,37,39,39,39,37,37,23,0,0,11,37,37,29,39,14,
11,28,39,14,37,11,9,0,10,29,39,39,39,39,39,14,12,0,0,0,0,0,0,12,14,39,20,14,39,20,17,0,7,43,33,33,33,33,33,16,8,0,12,37,28,39,32,18,4,0,0,25,34,34,32,18,4,0,0,11,37,39,39,37,12,0,0,0,0,0,42,37,39,39,39,37,19,0,0,11,37,37,37,39,39,37,25,37,37,37,11,9,0,11,37,37,37,37,39,39,39,37,37,37,11,9,0,11,37,37,37,39,39,37,25,17,3,0,0,0,0,11,37,37,37,39,39,39,25,17,3,0,0,0,1,13,37,37,37,39,39,39,37,37,37,37,24,9,0,22,37,37,37,18,10,29,39,14,37,28,
39,14,43,37,37,37,17,0,0,2,43,37,37,29,39,14,11,28,39,14,37,37,22,0,0,2,43,37,37,37,34,34,20,34,34,37,37,37,22,0,0,12,43,37,37,37,39,39,37,37,37,37,23,0,42,37,34,34,20,34,34,25,9,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,0,0,0,3,10,29,36,36,36,33,23,0,0,0,0,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,0,0,0,0,22,39,45,41,46,33,24,0,0,0,0,0,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,
36,36,36,36,36,41,43,7,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,28,12,0,0,3,26,37,36,36,36,28,21,0,0,0,4,37,36,36,36,28,12,0,19,37,36,36,36,35,13,17,20,36,36,36,15,1,0,17,29,36,36,36,28,17,17,43,35,36,36,35,18,9,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,19,37,36,36,36,36,41,44,27,28,13,5,0,0,0,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,36,36,36,36,41,43,
7,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,28,21,12,32,36,36,36,15,1,0,3,26,16,38,48,31,15,21,0,0,0,0,0,0,0,0,21,37,36,36,36,28,12,0,0,12,37,36,36,36,28,26,0,0,19,37,36,36,36,28,12,0,19,37,36,36,36,28,12,0,19,37,36,36,36,28,12,0,0,0,0,0,8,16,38,48,31,15,1,0,19,37,36,36,36,35,13,2,20,36,36,36,15,1,19,37,36,36,36,35,13,17,20,36,36,36,15,1,19,37,36,36,36,36,36,41,27,28,24,7,0,0,19,37,36,36,36,36,36,41,
27,28,24,7,0,0,2,14,36,36,36,36,36,36,36,36,36,36,15,1,9,18,44,36,36,29,26,25,27,36,36,33,23,22,39,36,36,27,25,0,0,17,29,36,36,36,28,17,6,43,35,36,36,35,18,9,0,17,29,36,36,36,28,17,2,43,35,36,36,35,18,9,0,10,39,36,36,36,36,36,36,36,36,41,43,3,0,4,37,36,36,36,28,12,0,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,
0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,51,30,32,21,20,30,51,31,15,1,0,1,43,35,50,50,44,37,22,39,46,51,38,32,26,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,51,52,
51,52,50,30,33,43,4,0,0,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,8,16,38,51,31,15,5,12,4,0,0,0,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,0,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,51,30,32,21,20,30,51,31,15,1,19,37,36,51,
51,30,32,21,20,30,51,31,15,1,19,37,36,51,51,52,52,52,50,38,34,26,0,0,19,37,36,51,51,52,52,52,50,38,34,26,0,0,2,14,46,52,52,52,51,51,52,52,52,31,15,1,0,11,33,49,49,33,23,43,41,52,53,35,13,25,27,50,48,39,22,0,0,1,43,35,50,50,44,37,22,39,46,51,38,32,26,0,0,1,43,35,50,50,44,37,22,39,46,51,38,32,26,0,0,10,39,48,52,52,52,52,51,51,51,41,43,3,0,19,37,36,51,47,28,12,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,
0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,54,49,27,13,20,30,51,31,15,1,0,0,26,32,38,51,46,39,16,41,53,48,33,25,7,0,0,0,0,4,20,31,
51,45,52,30,32,12,0,0,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,19,37,36,51,47,41,41,38,49,51,47,40,11,0,0,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,8,16,38,51,31,15,14,32,20,24,1,0,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,
12,0,0,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,54,49,27,13,20,30,51,31,15,1,19,37,36,51,54,49,27,13,20,30,51,31,15,1,19,37,36,51,47,44,44,30,52,50,27,24,0,0,19,37,36,51,47,44,44,30,52,50,27,24,0,0,2,14,41,41,41,41,53,49,41,41,41,41,15,1,0,17,29,45,53,35,13,20,38,49,50,36,15,43,41,52,46,34,12,0,0,0,26,32,38,51,46,39,16,41,53,48,33,25,7,0,0,0,26,32,38,51,46,39,16,41,53,48,33,25,7,0,0,10,39,41,41,41,41,41,31,51,48,27,18,7,0,19,37,36,51,47,28,12,0,0,
0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,
45,49,38,14,20,30,51,31,15,1,0,0,7,25,33,48,53,41,33,45,52,36,20,12,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,47,28,37,14,35,49,52,36,16,4,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,22,22,32,46,51,31,15,1,0,0,8,16,38,51,31,27,38,46,31,39,11,0,0,0,0,19,37,36,51,47,28,12,0,
0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,0,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,45,49,38,14,20,30,51,31,15,1,19,37,36,51,45,49,38,14,20,30,51,31,15,1,19,37,36,51,47,28,18,33,49,53,35,25,0,0,19,37,36,51,47,28,18,33,49,53,35,25,0,0,19,25,43,43,43,35,53,49,33,43,43,43,23,0,0,2,14,30,51,36,15,29,47,30,46,30,32,20,38,51,31,16,4,0,0,0,7,25,33,48,53,41,33,45,52,36,20,12,0,0,0,0,7,25,33,48,53,41,
33,45,52,36,20,12,0,0,0,5,18,43,43,43,43,27,48,53,41,20,12,0,0,19,37,36,51,47,28,12,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,
19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,30,30,49,27,20,30,51,31,15,1,0,0,0,2,16,41,53,45,31,51,47,39,23,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,47,28,12,21,14,30,51,47,28,21,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,
40,40,46,51,31,15,1,0,0,8,16,38,51,31,45,45,53,52,36,20,4,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,0,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,30,30,49,27,20,30,51,31,15,1,19,37,36,51,30,30,49,27,20,30,51,31,15,1,19,37,36,51,47,28,13,33,49,50,27,24,0,0,19,37,36,51,47,28,13,33,49,50,27,24,0,0,0,0,7,7,25,35,53,49,33,22,7,7,0,0,0,1,37,36,51,30,32,33,49,41,31,45,
39,29,47,52,44,18,7,0,0,0,0,2,16,41,53,45,31,51,47,39,23,0,0,0,0,0,0,2,16,41,53,45,31,51,47,39,23,0,0,0,0,0,7,7,7,24,40,47,52,38,28,22,0,0,0,19,37,36,51,47,28,12,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,
53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,30,27,50,31,14,30,51,31,15,1,0,0,0,0,23,39,46,51,52,50,44,37,8,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,19,37,36,51,47,28,12,3,37,41,51,48,39,10,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,27,11,
0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,50,51,31,15,1,0,0,8,16,38,51,49,35,39,36,52,47,28,21,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,0,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,30,27,50,31,14,30,51,31,15,1,19,37,36,51,30,27,50,31,14,30,51,31,15,1,19,37,36,51,47,44,44,30,53,38,28,26,0,0,19,37,36,51,47,44,44,30,
53,38,28,26,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,13,35,53,45,39,35,53,35,44,50,27,33,49,50,27,24,0,0,0,0,0,0,23,39,46,51,52,50,44,37,8,0,0,0,0,0,0,0,23,39,46,51,52,50,44,37,8,0,0,0,0,0,0,0,22,28,38,52,47,40,24,7,0,0,0,19,37,36,51,47,28,12,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,
0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,30,34,30,48,33,30,51,31,15,1,0,0,0,0,6,43,35,50,51,30,28,17,0,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,47,28,
12,3,43,41,52,48,39,10,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,49,51,31,15,1,0,0,8,16,38,51,30,14,25,35,53,45,29,26,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,0,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,30,34,30,48,33,30,51,31,15,1,19,37,36,51,
30,34,30,48,33,30,51,31,15,1,19,37,36,51,51,52,52,53,44,29,24,3,0,0,19,37,36,51,51,52,52,53,44,29,24,3,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,23,33,49,50,27,36,48,40,27,50,44,35,53,45,39,10,0,0,0,0,0,0,6,43,35,50,51,30,28,17,0,0,0,0,0,0,0,0,6,43,35,50,51,30,28,17,0,0,0,0,0,0,0,12,20,41,50,48,27,43,4,0,0,0,0,19,37,36,51,47,28,12,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,
0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,30,14,35,50,31,30,51,31,15,1,0,0,0,0,0,17,39,48,52,44,18,3,0,0,0,0,0,9,13,35,50,48,
33,33,33,47,52,44,43,19,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,19,37,36,51,47,28,12,5,20,31,51,45,29,26,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,46,51,31,15,1,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,
12,0,0,0,0,0,8,16,38,51,31,15,1,0,19,37,36,51,30,14,35,50,31,30,51,31,15,1,19,37,36,51,30,14,35,50,31,30,51,31,15,1,19,37,36,51,47,44,36,49,50,36,20,2,0,0,19,37,36,51,47,44,36,49,50,36,20,2,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,21,29,47,52,44,30,46,28,39,45,31,36,51,30,14,12,0,0,0,0,0,0,0,17,39,48,52,44,18,3,0,0,0,0,0,0,0,0,0,17,39,48,52,44,18,3,0,0,0,0,0,0,8,43,27,48,53,41,20,12,0,0,0,0,0,19,37,36,51,47,28,12,0,0,
0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,
30,14,28,30,48,30,51,31,15,1,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,19,37,36,51,47,28,11,43,33,45,51,38,14,5,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,21,21,32,46,51,31,15,1,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,19,37,36,51,47,28,12,0,
0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,23,23,23,23,21,0,8,16,38,51,31,15,1,0,19,37,36,51,30,14,28,30,48,30,51,31,15,1,19,37,36,51,30,14,28,30,48,30,51,31,15,1,19,37,36,51,47,28,20,41,52,48,33,11,0,0,19,37,36,51,47,28,20,41,52,48,33,11,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,4,20,38,51,31,45,31,20,14,30,46,30,51,36,15,42,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,0,0,0,17,39,48,
52,44,18,9,0,0,0,0,0,7,24,39,46,51,38,28,23,23,23,23,26,0,0,19,37,36,51,47,28,12,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,
19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,30,14,43,35,50,51,51,31,15,1,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,19,37,36,51,47,33,33,44,47,51,48,27,13,7,0,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,28,12,
12,32,46,51,31,15,1,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,47,33,33,33,33,33,15,6,8,16,38,51,31,15,1,0,19,37,36,51,30,14,43,35,50,51,51,31,15,1,19,37,36,51,30,14,43,35,50,51,51,31,15,1,19,37,36,51,47,28,23,40,45,52,36,15,4,0,19,37,36,51,47,28,23,40,45,52,36,15,4,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,3,37,41,52,49,53,44,43,15,36,
53,50,53,35,13,9,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,10,34,31,52,47,33,33,33,33,33,33,15,8,0,19,37,36,51,47,28,12,0,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,
53,49,49,49,49,31,16,6,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,30,14,10,29,46,51,51,31,15,1,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,19,37,36,51,53,49,50,53,52,48,44,20,12,0,0,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,53,49,49,49,49,31,16,
6,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,19,37,36,51,53,49,49,49,49,31,16,6,8,16,38,51,31,15,1,0,19,37,36,51,30,14,10,29,46,51,51,31,15,1,19,37,36,51,30,14,10,29,46,51,51,31,15,1,19,37,36,51,47,28,12,20,36,51,45,39,22,0,19,37,36,51,47,28,12,20,
36,51,45,39,22,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,25,27,50,54,50,27,24,13,35,53,54,49,33,23,0,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,11,27,50,54,53,49,49,49,49,49,38,16,8,0,19,37,36,51,47,28,12,0,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,
4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,28,12,0,0,0,19,37,36,46,46,28,12,0,0,0,19,37,36,46,46,28,12,0,19,37,36,46,30,14,5,43,44,46,46,31,15,1,0,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,19,37,36,46,46,46,
46,30,36,33,16,17,0,0,0,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,28,12,12,32,46,46,31,15,1,0,0,8,16,38,46,31,15,24,27,46,46,29,26,0,0,0,19,37,36,46,46,28,12,0,0,19,37,36,46,46,28,12,0,0,19,37,36,46,46,28,12,0,19,37,36,46,46,28,12,0,19,37,36,46,46,46,46,46,46,31,16,6,8,16,38,46,31,15,1,0,19,37,36,46,30,14,5,43,44,46,46,31,15,1,19,37,36,46,
30,14,5,43,44,46,46,31,15,1,19,37,36,46,46,28,12,24,33,46,46,44,37,1,19,37,36,46,46,28,12,24,33,46,46,44,37,1,0,0,0,0,25,35,46,46,33,22,0,0,0,0,0,0,0,22,39,46,46,46,39,10,23,33,46,46,46,29,26,0,0,0,0,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,0,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,0,11,27,46,46,46,46,46,46,46,46,38,16,8,0,19,37,36,46,46,28,12,0,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,
4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,20,2,0,0,0,7,13,34,34,34,20,2,0,0,0,7,13,34,34,34,20,2,0,7,13,34,34,34,37,8,22,32,34,34,34,18,3,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,4,15,34,34,34,43,
1,0,7,13,34,34,34,20,12,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,7,13,34,34,34,34,32,14,37,24,12,0,0,0,0,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,20,2,5,16,34,34,34,18,3,0,0,1,43,34,34,34,18,23,34,34,34,14,12,0,0,0,7,13,34,34,34,20,2,0,0,7,13,34,34,34,20,2,0,0,7,13,34,34,34,20,2,0,7,13,34,34,34,20,2,0,7,13,34,34,34,34,
34,34,34,34,43,1,1,43,34,34,34,18,3,0,7,13,34,34,34,37,8,22,32,34,34,34,18,3,7,13,34,34,34,37,8,22,32,34,34,34,18,3,7,13,34,34,34,20,2,12,20,34,34,34,43,1,7,13,34,34,34,20,2,12,20,34,34,34,43,1,0,0,0,0,11,34,34,34,14,10,0,0,0,0,0,0,0,12,20,34,34,34,20,12,21,14,34,34,34,15,5,0,0,0,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,22,32,34,34,34,34,34,34,34,34,34,43,1,0,7,13,34,34,34,20,2,0,0,
0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,2,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,8,12,12,12,2,0,0,0,8,12,12,
12,4,0,7,12,12,12,12,8,0,0,0,0,0,0,0,12,12,12,12,1,0,0,0,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,8,12,12,12,12,12,5,1,0,0,0,0,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,2,0,0,5,12,12,12,8,0,0,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,0,0,8,12,12,12,2,0,0,
0,0,8,12,12,12,2,0,0,0,0,8,12,12,12,2,0,0,0,8,12,12,12,2,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,4,12,12,12,8,0,0,0,8,12,12,12,4,0,7,12,12,12,12,8,0,0,8,12,12,12,4,0,7,12,12,12,12,8,0,0,8,12,12,12,2,0,0,2,12,12,12,4,0,0,8,12,12,12,2,0,0,2,12,12,12,4,0,0,0,0,0,3,12,12,12,12,9,0,0,0,0,0,0,0,0,2,12,12,12,2,0,0,12,12,12,12,5,0,0,0,0,0,0,0,0,0,0,12,12,12,12,1,0,0,0,0,0,0,0,0,0,0,0,12,12,
12,12,1,0,0,0,0,0,0,7,12,12,12,12,12,12,12,12,12,12,4,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,19,2,2,2,9,0,0,0,0,0,0,0,0,5,2,4,0,5,2,4,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,4,2,2,4,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,5,2,2,42,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,5,2,2,42,0,0,0,0,0,0,0,0,0,4,2,2,4,0,0,0,0,0,0,0,0,0,0,4,2,4,0,5,2,
6,0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,0,3,19,19,19,19,19,19,19,19,0,0,0,0,0,0,4,2,4,0,5,2,6,0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,0,19,19,19,7,0,0,19,19,19,3,0,0,3,19,19,19,0,0,0,0,0,0,0,5,2,2,2,2,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,0,0,0,19,19,19,3,0,0,7,19,19,19,7,0,0,0,0,
19,19,19,7,0,0,9,19,19,19,19,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,9,0,0,0,19,19,19,3,0,0,0,3,19,19,19,0,0,0,7,19,19,19,19,19,19,19,19,19,19,19,19,19,7,0,0,0,0,0,9,19,19,19,7,0,0,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,5,4,19,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,4,0,5,2,6,0,0,0,0,0,0,0,0,0,9,19,19,19,7,0,0,19,19,19,19,0,0,0,0,0,0,0,9,9,0,
0,9,0,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,4,2,4,0,5,2,6,0,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,14,14,14,22,0,0,0,0,0,0,0,17,14,14,15,22,14,14,16,2,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,4,37,14,14,15,12,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,10,14,14,14,18,3,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,10,14,
14,14,18,3,0,0,0,0,0,0,0,4,37,14,14,15,12,0,0,0,0,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,0,0,12,43,37,37,37,37,37,37,37,37,10,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,0,0,11,37,37,37,18,4,10,37,37,37,37,12,12,37,37,37,37,10,0,0,0,0,0,21,20,14,14,14,14,14,11,0,0,0,0,0,0,0,0,0,0,0,0,0,6,22,25,18,25,17,7,0,0,0,0,0,0,0,0,0,0,0,
0,0,7,21,21,21,5,0,0,21,37,37,37,37,22,0,22,37,37,37,43,2,1,25,37,37,37,13,42,0,0,11,37,37,37,18,4,3,24,37,37,37,37,21,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,0,0,1,13,37,37,37,37,37,37,37,37,37,37,24,9,0,22,37,37,37,37,12,0,2,43,37,37,37,11,0,1,13,37,37,37,37,37,37,37,37,37,37,37,37,37,13,42,0,0,0,3,25,37,37,37,25,19,17,37,37,37,37,21,0,0,0,0,0,0,0,0,0,0,0,19,10,18,16,14,20,37,11,4,0,0,0,0,0,0,0,0,0,0,5,15,14,16,17,14,
14,18,19,0,0,0,0,0,0,0,3,25,37,37,37,25,19,17,37,37,37,37,21,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,5,15,14,16,17,14,14,18,19,0,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,18,44,46,27,11,0,0,0,0,0,0,0,10,29,30,35,32,41,36,14,12,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,5,20,44,46,27,13,1,0,0,0,0,4,20,38,38,29,46,41,
43,7,0,0,0,0,0,0,5,37,44,46,27,43,19,0,4,20,38,38,29,46,41,43,7,0,0,0,0,5,37,44,46,27,43,19,0,0,0,0,0,0,0,5,20,44,46,27,13,1,0,0,0,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,19,37,36,36,36,28,12,12,32,36,36,36,15,1,0,0,0,22,40,36,36,36,36,36,36,36,35,25,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,19,37,36,36,36,28,12,12,32,36,36,36,15,1,0,0,19,37,36,36,36,28,12,25,35,36,36,40,22,22,33,36,36,35,25,0,0,0,0,0,21,32,36,46,40,46,36,
14,21,0,0,0,0,0,0,0,0,0,0,3,11,37,39,35,44,35,34,22,0,0,0,0,0,0,0,0,0,0,0,0,0,22,28,29,29,37,1,0,23,33,36,36,41,20,21,13,44,36,36,39,10,11,39,36,36,36,14,12,0,19,37,36,36,36,28,12,23,29,36,36,36,33,23,0,19,37,36,36,36,28,12,12,32,36,36,36,15,1,0,0,0,2,14,36,36,36,36,36,36,36,36,36,36,15,1,7,18,35,36,36,27,13,19,25,33,36,36,44,37,3,2,14,36,36,36,36,36,36,36,36,36,36,36,36,36,32,12,0,0,0,4,20,36,36,36,14,4,24,27,36,36,33,23,0,0,0,0,
0,0,19,6,6,1,6,25,29,44,31,30,38,41,40,37,21,0,0,0,0,0,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,0,0,0,0,4,20,36,36,36,14,4,24,27,36,36,33,23,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,2,14,30,33,34,35,36,37,1,0,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,19,19,18,44,46,27,11,19,19,0,0,0,0,3,19,25,40,47,36,30,29,23,19,0,0,0,0,1,4,
20,38,38,29,46,41,43,3,0,0,0,0,0,0,19,17,20,44,30,29,22,19,0,0,0,4,20,38,38,29,46,41,43,19,0,0,0,0,19,19,25,33,45,27,37,12,9,0,4,20,38,38,29,46,41,43,7,0,0,19,19,25,33,45,27,37,12,19,19,19,0,0,0,0,19,19,17,20,44,30,29,22,19,19,0,0,0,0,19,19,18,27,46,46,30,40,24,19,19,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,22,40,48,51,52,52,52,51,53,35,25,0,0,0,0,19,18,27,46,46,30,40,24,19,19,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,
0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,0,7,19,22,32,36,31,40,46,35,43,8,5,4,3,0,0,0,0,0,0,12,32,41,46,31,41,38,36,16,4,0,0,0,0,0,0,0,0,19,19,19,19,22,33,45,31,16,6,0,4,43,27,48,49,35,15,13,44,52,48,39,22,28,31,52,30,29,11,9,0,19,37,36,51,47,28,22,32,36,53,45,27,43,4,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,2,14,46,52,52,52,51,51,52,52,52,31,15,1,0,26,32,38,51,30,29,10,32,38,51,30,28,10,0,2,14,46,52,52,
52,51,51,52,52,52,31,31,51,46,32,12,0,0,0,4,20,30,51,30,14,4,24,27,53,50,33,23,0,0,0,0,0,1,18,16,16,37,24,39,30,53,53,49,50,52,45,35,37,4,0,0,0,0,19,19,19,19,19,18,27,46,46,30,40,24,0,7,19,19,19,7,0,0,4,20,30,51,30,14,4,24,27,53,50,33,23,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,19,18,27,46,46,30,40,24,19,0,0,0,0,19,19,19,19,4,20,38,38,29,46,41,43,7,7,19,19,19,7,0,0,0,0,0,0,0,0,
0,12,43,37,37,37,34,34,32,37,37,37,23,0,0,12,43,37,37,37,39,39,39,37,37,37,23,0,9,11,15,37,37,34,34,20,34,34,37,43,22,9,0,0,0,11,37,37,37,20,39,39,37,37,23,0,0,11,37,34,34,20,34,34,37,37,23,0,0,11,37,37,37,39,39,37,37,37,24,7,42,37,34,34,20,34,34,25,9,0,11,37,37,37,39,39,37,24,37,37,37,37,21,0,0,11,37,37,37,25,20,39,39,37,37,37,11,9,0,22,37,37,37,37,28,39,32,43,37,37,37,11,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,22,40,48,53,41,41,41,49,53,35,
25,0,0,0,11,37,37,37,28,39,32,37,37,37,37,11,9,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,1,13,37,37,37,34,39,28,39,39,16,16,14,20,13,1,0,0,0,0,0,12,32,41,33,20,37,29,31,32,12,0,0,0,0,0,0,0,11,37,37,37,37,37,33,49,31,16,6,0,0,12,16,44,49,45,27,18,44,52,48,39,14,41,53,47,33,13,1,0,0,19,37,36,51,47,28,14,41,50,48,35,37,2,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,2,14,
41,41,41,41,53,49,41,41,41,41,15,1,0,7,25,33,45,53,44,37,35,50,48,27,13,3,0,2,14,41,41,41,41,53,49,41,41,41,41,31,51,46,32,12,0,0,0,4,20,30,51,30,14,5,24,27,53,50,33,23,0,0,0,0,0,5,14,31,31,39,14,38,52,48,44,33,35,46,51,45,40,11,0,0,0,21,37,37,37,37,22,12,37,28,39,32,43,2,1,25,37,37,37,13,42,0,4,20,30,51,30,14,5,24,27,53,50,33,23,0,0,0,0,0,22,34,44,31,41,39,24,0,0,0,0,0,0,0,0,42,37,34,34,20,34,34,25,9,0,0,0,0,0,11,37,37,28,39,32,37,
37,37,23,0,0,21,37,37,37,37,22,37,34,34,20,34,34,25,1,25,37,37,37,13,42,0,0,0,0,0,0,0,0,10,39,36,36,36,36,36,36,36,36,41,43,3,0,10,39,36,36,36,36,36,36,36,36,41,43,3,1,37,36,36,44,39,43,22,14,27,41,41,32,10,0,0,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,36,36,36,36,36,16,6,0,4,37,36,36,36,28,12,0,19,37,36,36,36,28,17,23,29,36,36,36,33,23,0,19,37,36,36,36,14,21,13,27,36,36,36,15,1,7,18,44,36,36,27,25,17,11,33,
36,36,41,37,3,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,22,40,48,53,35,43,40,49,53,35,25,0,0,19,37,36,36,36,14,17,13,27,36,36,36,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,2,14,36,36,36,14,17,26,17,13,33,31,30,38,14,5,0,0,0,19,42,8,43,37,11,33,44,44,15,43,4,42,42,3,0,0,0,19,37,36,36,36,36,36,36,49,31,16,6,0,0,0,17,14,36,53,46,40,44,52,48,39,35,49,48,27,43,4,0,0,0,19,37,
36,51,47,28,44,49,49,44,16,21,0,0,0,19,37,36,51,47,28,22,22,32,46,51,31,15,1,0,0,0,19,25,43,43,43,35,53,49,33,43,43,43,23,0,0,0,2,16,41,53,45,40,47,52,36,20,12,0,0,19,25,43,43,43,35,53,49,33,43,43,15,31,51,46,32,12,0,0,0,4,20,30,51,30,14,22,24,27,53,50,33,23,0,0,0,0,0,4,14,30,48,40,33,49,52,41,16,23,18,27,48,52,41,37,1,0,0,23,33,36,36,41,20,21,13,44,36,36,39,10,11,39,36,36,36,14,12,0,4,20,30,51,30,14,22,24,27,53,50,33,23,0,0,0,0,0,10,29,36,36,
36,33,23,0,0,0,0,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,19,37,36,36,36,36,36,36,36,41,43,7,0,23,33,36,36,41,20,21,13,44,36,36,39,10,11,39,36,36,36,14,12,0,0,0,0,0,0,0,0,10,39,48,52,52,52,52,51,51,51,41,43,3,0,10,39,48,52,52,52,52,51,51,51,41,43,3,1,37,36,52,53,47,33,14,36,49,52,50,35,13,9,0,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,31,16,6,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,22,
32,36,53,45,27,43,4,0,19,37,36,51,30,14,21,14,38,51,51,31,15,1,0,10,29,47,51,31,20,5,15,36,52,45,39,23,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,22,40,48,53,35,13,40,49,53,35,25,0,0,19,37,36,51,30,14,21,14,38,51,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,3,18,44,53,45,39,22,0,21,32,30,51,51,30,14,5,0,0,4,43,15,15,37,12,22,33,35,36,37,21,15,15,15,13,1,0,0,19,37,36,51,
51,52,52,52,52,31,16,6,0,0,0,0,22,28,38,52,30,44,52,48,27,45,49,44,15,12,0,0,0,0,19,37,36,51,47,35,48,50,41,14,26,0,0,0,0,19,37,36,51,47,40,40,40,40,46,51,31,15,1,0,0,0,0,0,7,7,25,35,53,49,33,22,7,7,0,0,0,0,0,22,29,46,52,30,52,47,39,23,0,0,0,0,0,7,7,25,35,53,49,33,22,1,15,31,51,46,32,12,0,0,0,8,16,31,51,45,27,40,40,27,53,50,33,23,0,0,0,0,0,19,37,44,53,36,41,52,49,33,39,39,39,39,30,51,30,14,5,0,0,4,43,27,48,49,35,15,13,44,52,48,
39,22,28,31,52,30,29,11,9,0,8,16,31,51,45,27,40,40,27,53,50,33,23,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,51,52,52,52,52,41,43,7,0,4,43,27,48,49,35,15,13,44,52,48,39,22,28,31,52,30,29,11,9,0,0,0,0,0,0,0,0,10,39,41,41,41,41,41,31,51,48,27,18,7,0,10,39,41,41,41,41,41,31,51,48,27,18,7,1,37,44,41,47,51,30,27,49,49,38,53,41,43,7,0,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,
41,41,43,7,19,37,36,51,47,41,41,41,41,41,16,6,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,14,41,50,48,35,37,2,0,0,19,37,36,51,30,14,25,27,49,54,51,31,15,1,0,8,37,41,52,45,40,23,39,47,51,31,20,5,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,22,40,49,53,35,25,40,49,53,35,25,0,0,19,37,36,51,30,14,25,27,49,54,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,0,22,40,48,53,35,18,7,24,
27,49,53,38,36,14,5,0,0,23,39,31,31,29,17,22,33,36,35,13,22,39,31,31,29,10,0,0,19,37,36,51,47,41,41,41,41,41,16,6,0,0,0,0,21,14,31,52,53,30,52,49,46,51,49,35,43,1,0,0,0,0,19,37,36,51,45,45,51,49,35,43,6,0,0,0,0,19,37,36,51,53,49,49,49,49,50,51,31,15,1,0,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,1,18,35,49,54,50,44,37,8,0,0,0,0,0,0,0,25,35,53,49,33,22,1,15,31,51,46,32,12,0,0,0,7,18,35,50,51,49,49,49,49,51,50,33,23,0,0,0,0,
0,0,22,29,30,53,48,51,53,48,48,48,48,48,49,51,46,28,12,0,0,0,12,16,44,49,45,27,18,44,52,48,39,14,41,53,47,33,13,1,0,0,7,18,35,50,51,49,49,49,49,51,50,33,23,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,19,37,36,51,47,41,41,41,41,41,43,7,0,0,12,16,44,49,45,27,18,44,52,48,39,14,41,53,47,33,13,1,0,0,0,0,0,0,0,0,0,5,18,43,43,43,43,27,48,53,41,20,12,0,0,5,18,43,43,43,43,27,48,53,41,20,12,0,0,23,18,37,
40,46,48,31,50,44,33,30,33,11,0,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,11,9,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,44,49,49,44,16,21,0,0,0,19,37,36,51,30,14,14,31,50,45,51,31,15,1,0,0,23,40,45,52,41,15,44,53,49,27,25,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,22,33,49,53,27,24,40,49,53,35,25,0,0,19,37,36,51,30,14,14,31,50,45,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,
0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,0,12,14,30,51,38,20,5,15,36,52,46,29,15,18,1,0,1,37,41,52,36,15,8,22,33,27,20,21,5,14,38,53,35,18,7,0,19,37,36,51,47,28,43,43,43,43,11,9,0,0,0,1,18,35,49,53,48,51,54,54,52,45,51,46,29,22,0,0,0,0,19,37,36,51,54,52,47,51,46,39,23,0,0,0,0,19,37,36,51,49,47,47,47,47,49,51,31,15,1,0,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,19,13,27,49,54,50,35,43,42,0,0,0,0,0,0,0,25,
35,53,49,33,22,1,15,31,51,46,32,12,0,0,0,0,10,32,36,46,47,47,47,47,52,50,33,23,0,0,0,0,0,0,1,13,39,36,30,51,48,38,38,38,38,38,38,38,38,29,21,0,0,0,0,17,14,36,53,46,40,44,52,48,39,35,49,48,27,43,4,0,0,0,0,10,32,36,46,47,47,47,47,52,50,33,23,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,47,28,43,43,43,43,22,0,0,0,0,17,14,36,53,46,40,44,52,48,39,35,49,48,27,43,4,0,0,0,0,0,0,0,0,0,
0,0,7,7,7,24,40,47,52,38,28,22,0,0,0,0,7,7,7,24,40,47,52,38,28,22,0,0,0,0,7,4,43,44,52,50,46,29,18,20,43,5,0,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,28,12,7,7,7,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,35,48,50,41,14,26,0,0,0,0,19,37,36,51,30,14,33,48,30,30,51,31,15,1,0,0,5,20,31,51,47,29,30,51,30,32,21,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,23,33,50,50,33,11,40,49,53,35,
25,0,0,19,37,36,51,30,14,33,48,30,30,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,0,3,18,44,53,45,39,22,28,46,52,36,15,6,3,0,0,12,32,30,53,35,13,9,5,25,24,21,0,3,43,41,52,38,20,4,0,19,37,36,51,47,28,12,7,7,7,0,0,0,0,0,22,29,46,51,30,35,48,54,51,46,27,48,53,41,16,2,0,0,0,19,37,36,51,51,30,33,48,52,36,20,12,0,0,0,19,37,36,51,47,28,28,28,28,46,51,36,37,1,0,0,0,0,0,
0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,22,29,30,51,47,52,46,39,23,0,0,0,0,0,0,0,25,35,53,49,33,22,1,15,31,51,46,32,12,0,0,0,0,9,22,16,28,28,28,28,27,53,50,33,23,0,0,0,0,0,0,0,8,11,15,41,52,45,39,20,20,20,20,20,20,20,37,5,0,0,0,0,0,22,28,38,52,30,44,52,48,27,45,49,44,15,12,0,0,0,0,0,9,22,16,28,28,28,28,27,53,50,33,23,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,47,40,40,40,
40,39,23,0,0,0,0,0,22,28,38,52,30,44,52,48,27,45,49,44,15,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,28,38,52,47,40,24,7,0,0,0,0,0,0,22,28,38,52,47,40,24,7,0,0,0,0,0,0,11,27,50,51,31,15,8,4,1,0,0,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,47,28,12,0,0,0,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,45,45,51,49,35,43,6,0,0,0,0,19,37,36,51,30,14,31,50,35,30,51,31,15,1,0,0,0,25,27,49,53,44,49,50,
35,18,3,0,0,19,37,36,51,47,28,23,23,32,46,51,31,15,1,0,19,22,13,35,53,48,39,23,40,49,53,35,25,10,9,19,37,36,51,30,14,31,50,35,30,51,31,15,1,19,37,36,51,47,28,23,23,32,46,51,31,15,23,2,0,19,37,36,51,47,28,23,25,35,53,49,40,23,23,33,49,53,35,25,22,1,0,0,0,22,40,48,53,35,13,27,49,49,27,24,0,0,0,0,10,39,45,50,27,11,0,0,0,0,0,0,0,25,35,53,47,29,21,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,2,16,41,53,48,27,20,41,52,48,33,20,36,52,48,33,25,23,5,0,19,37,
36,51,47,40,16,41,53,48,27,13,23,12,0,19,37,36,51,47,28,21,21,32,46,51,36,37,23,23,5,0,0,0,0,0,25,35,53,49,40,23,23,10,9,0,0,0,5,15,44,53,45,33,47,53,36,20,23,26,0,0,0,0,0,25,35,53,49,33,23,23,15,31,51,46,32,23,23,21,0,0,0,0,4,21,21,21,24,27,53,50,33,23,23,10,7,0,0,0,0,0,0,25,27,50,53,44,15,17,12,26,25,14,16,4,0,0,0,0,0,0,21,14,31,52,53,30,52,49,46,51,49,35,43,1,0,0,0,0,0,0,0,4,21,22,23,24,27,53,50,33,23,0,0,0,4,20,31,51,38,20,
36,52,30,32,12,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,19,37,36,51,53,49,49,49,49,27,11,0,0,0,0,0,21,14,31,52,53,30,52,49,46,51,49,35,43,1,0,0,0,0,0,0,0,0,0,0,0,0,0,12,20,41,50,48,27,43,4,0,0,0,0,0,0,12,20,41,50,48,27,43,4,0,0,0,0,0,0,0,22,39,48,52,41,43,3,0,0,0,0,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,47,28,12,0,0,0,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,54,52,47,
51,46,39,23,0,0,0,0,19,37,36,51,30,33,48,30,28,30,51,31,15,1,0,0,0,21,32,30,51,50,51,47,29,10,0,0,0,19,37,36,51,47,33,33,33,33,46,51,31,15,1,0,26,29,33,46,51,30,33,33,33,49,53,35,33,14,2,19,37,36,51,30,33,48,30,28,30,51,31,15,1,19,37,36,51,47,33,33,33,33,46,51,31,33,33,25,0,19,37,36,51,47,33,33,33,35,53,49,33,33,33,33,49,53,35,33,39,22,0,0,0,12,14,30,51,31,20,41,52,46,28,26,0,0,0,0,23,33,49,49,40,22,8,10,24,25,24,17,1,11,27,50,48,39,10,0,19,37,36,51,
47,28,12,0,0,0,0,0,0,7,25,33,45,52,36,20,13,44,52,48,39,23,39,47,51,38,33,33,24,0,19,37,36,51,47,28,22,29,46,51,30,33,33,13,9,19,37,36,51,47,28,12,12,32,46,51,36,33,33,33,24,0,0,0,0,0,25,35,53,49,33,33,33,14,2,0,0,9,24,40,45,53,41,16,44,50,48,33,33,16,8,0,0,0,0,25,35,53,49,33,33,33,33,31,51,46,33,33,33,43,3,0,0,0,0,0,0,0,24,27,53,50,33,33,33,32,12,0,0,0,0,0,0,17,28,30,51,45,35,29,32,29,27,31,14,2,0,0,0,0,0,1,18,35,49,53,48,51,54,54,
52,45,51,46,29,22,0,0,0,0,0,0,0,0,21,29,33,33,27,53,50,33,23,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,49,47,47,47,47,27,11,0,0,0,0,1,18,35,49,53,48,51,54,54,52,45,51,46,29,22,0,0,0,0,0,0,0,0,0,0,0,0,8,43,27,48,53,41,20,12,0,0,0,0,0,0,8,43,27,48,53,41,20,12,0,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,
28,28,22,0,19,37,36,51,47,28,12,0,0,0,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,51,30,33,48,52,36,20,12,0,0,0,19,37,36,51,30,36,50,35,20,30,51,31,15,1,0,0,0,3,13,35,50,54,52,41,37,8,0,0,0,19,37,36,51,53,49,49,49,49,53,51,31,15,1,0,17,39,45,51,54,50,49,49,49,52,51,49,46,32,12,19,37,36,51,30,36,50,35,20,30,51,31,15,1,19,37,36,51,53,49,49,49,49,53,54,50,49,35,13,0,19,37,36,51,53,49,49,49,49,51,52,49,49,49,49,52,51,49,49,40,22,0,0,0,3,18,44,53,45,39,46,
51,36,15,6,0,0,0,0,11,27,50,49,40,22,37,29,27,35,27,29,43,11,27,50,48,40,22,0,19,37,36,51,47,28,12,0,0,0,0,0,0,26,32,38,51,47,39,11,13,44,52,48,39,10,37,35,50,53,49,27,24,0,19,37,36,51,47,28,12,18,27,49,52,49,44,18,9,19,37,36,51,47,28,12,12,32,46,54,50,49,49,27,24,0,0,0,0,0,25,35,53,52,49,49,46,32,2,0,0,21,14,31,52,46,29,22,28,30,51,49,38,20,8,0,0,0,0,25,35,53,52,49,49,49,49,50,54,53,49,49,36,37,3,0,0,0,0,0,0,0,24,27,53,51,49,49,47,28,12,0,
0,0,0,0,0,19,13,33,47,52,50,47,46,47,49,46,14,2,0,0,0,0,0,22,29,46,51,30,35,48,54,51,46,27,48,53,41,16,2,0,0,0,0,0,0,0,26,29,45,49,49,51,50,33,23,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,19,37,36,51,47,28,28,28,28,28,22,0,0,0,0,22,29,46,51,30,35,48,54,51,46,27,48,53,41,16,2,0,0,0,0,0,0,0,0,0,0,7,24,39,46,51,38,28,23,23,23,23,26,0,0,7,24,39,46,51,38,28,23,23,23,23,26,0,0,0,0,0,
17,39,48,52,44,18,9,0,0,0,0,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,12,0,0,0,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,40,16,41,53,48,27,13,19,0,0,19,37,36,51,46,45,46,29,20,30,51,31,15,1,0,0,0,17,23,29,46,54,45,40,23,0,0,0,0,19,37,36,46,46,46,48,49,46,46,46,31,15,1,0,17,39,45,48,46,46,46,46,46,46,46,49,46,32,12,19,37,36,51,46,45,46,29,20,30,51,31,15,1,19,37,36,46,46,46,46,46,46,46,46,46,53,35,13,
0,19,37,36,46,46,46,46,46,46,46,46,46,46,46,46,46,46,45,49,40,22,0,0,0,0,22,40,48,50,35,49,49,27,24,0,0,0,0,0,22,40,48,53,35,29,44,47,45,46,45,46,35,29,44,52,45,39,17,0,19,37,36,51,47,28,12,0,0,0,0,0,1,43,35,46,46,44,37,8,13,44,46,46,39,10,26,32,38,53,53,27,24,0,19,37,36,46,46,28,12,12,14,36,50,52,44,18,9,19,37,36,46,46,28,12,12,32,46,46,46,53,53,27,24,0,0,0,0,0,25,35,46,46,47,51,46,32,2,0,3,18,35,46,46,35,18,1,13,27,46,50,38,20,8,0,0,0,0,25,
35,46,46,46,46,46,46,46,46,46,49,51,36,37,3,0,0,0,0,0,0,0,24,27,46,46,47,51,47,28,12,0,0,0,0,0,0,0,5,43,40,36,47,50,50,45,30,44,14,2,0,0,0,0,2,16,41,53,48,27,20,41,52,48,33,20,36,52,48,33,25,7,0,0,0,0,0,0,26,29,45,51,47,46,46,33,23,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,19,37,36,51,47,28,23,23,23,23,21,0,0,0,2,16,41,53,48,27,20,41,52,48,33,20,36,52,48,33,25,7,0,0,0,0,0,0,0,0,
0,10,34,31,52,47,33,33,33,33,33,33,15,8,0,10,34,31,52,47,33,33,33,33,33,33,15,8,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,28,12,0,0,0,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,22,29,46,51,30,29,22,0,0,19,37,36,51,51,53,44,43,20,30,51,31,15,1,0,0,4,20,33,27,47,51,31,20,2,0,0,0,0,7,13,34,34,34,28,47,47,29,34,34,34,18,3,0,17,39,45,30,34,34,34,34,34,34,29,45,
46,32,12,19,37,36,51,51,53,44,43,20,30,51,31,15,1,7,13,34,34,34,34,34,34,34,34,34,44,53,35,13,0,7,13,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,38,49,40,22,0,0,0,0,12,14,30,51,45,52,46,28,26,0,0,0,0,0,21,34,30,51,48,47,48,41,39,32,39,36,49,47,49,51,38,14,2,0,19,37,36,51,47,28,12,0,0,0,0,0,1,43,34,34,34,14,17,0,24,34,34,34,14,21,7,25,33,49,53,27,24,0,7,13,34,34,34,20,2,0,11,39,48,52,44,18,9,7,13,34,34,34,20,2,5,16,34,34,33,49,53,27,24,0,0,0,
0,0,11,34,34,34,31,51,46,32,2,0,3,18,34,34,34,14,21,0,12,16,40,49,38,20,8,0,0,0,0,11,34,34,34,34,34,34,34,34,34,28,47,51,36,37,3,0,0,0,0,0,0,0,23,32,34,34,36,51,47,28,12,0,0,0,0,0,0,0,0,4,24,20,29,47,47,39,14,43,22,9,0,0,0,7,25,33,45,52,36,20,13,44,52,48,39,23,39,47,51,38,34,17,0,0,0,0,0,0,26,29,45,52,41,34,34,32,22,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,19,37,36,51,47,33,33,33,
33,33,15,6,0,7,25,33,45,52,36,20,13,44,52,48,39,23,39,47,51,38,34,17,0,0,0,0,0,0,0,0,0,11,27,50,54,53,49,49,49,49,49,38,16,8,0,11,27,50,54,53,49,49,49,49,49,38,16,8,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,47,28,12,0,0,0,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,18,27,49,53,41,15,5,0,19,37,36,51,54,46,29,22,20,30,51,31,15,1,0,0,5,14,30,50,51,48,27,25,
9,0,0,0,0,0,8,12,12,12,28,47,47,29,21,12,12,8,0,0,17,39,45,30,14,12,12,12,12,21,29,45,46,32,12,19,37,36,51,54,46,29,22,20,30,51,31,15,1,0,8,12,12,12,12,12,12,12,12,18,44,53,35,13,0,0,8,12,12,12,12,12,12,12,12,12,12,12,12,12,12,20,38,49,40,22,0,0,0,0,3,18,44,53,54,51,36,15,8,0,0,0,0,0,19,13,27,46,48,46,27,20,22,12,23,14,35,46,48,30,40,25,9,0,19,37,36,51,47,28,12,0,0,0,0,0,0,4,12,12,12,12,0,0,1,12,12,12,12,0,0,23,33,49,53,27,24,0,0,8,
12,12,12,2,0,0,17,39,48,52,44,18,9,0,8,12,12,12,2,0,0,5,12,23,33,49,53,27,24,0,0,0,0,0,3,12,12,15,31,52,46,32,2,0,0,8,12,12,12,2,0,0,0,22,40,49,38,20,8,0,0,0,0,3,12,12,12,12,12,12,12,12,21,28,47,52,36,37,3,0,0,0,0,0,0,0,3,12,12,37,36,52,47,28,12,0,0,0,0,0,0,0,0,0,0,12,28,47,47,28,21,19,0,0,0,0,0,26,32,38,51,47,39,11,13,44,52,48,39,10,37,35,50,50,35,37,8,0,0,0,0,0,26,29,45,52,41,43,12,12,7,0,9,13,35,50,49,33,14,14,
14,39,45,52,44,43,19,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,19,37,36,51,53,49,49,49,49,31,16,6,0,26,32,38,51,47,39,11,13,44,52,48,39,10,37,35,50,50,35,37,8,0,0,0,0,0,0,0,0,11,27,46,46,46,46,46,46,46,46,38,16,8,0,11,27,46,46,46,46,46,46,46,46,38,16,8,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,0,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,28,12,0,0,0,0,0,0,19,37,36,46,46,28,12,0,19,37,36,46,46,28,12,
12,14,36,46,46,33,25,0,19,37,36,46,46,44,37,4,20,30,46,31,15,1,0,0,5,14,30,46,38,35,16,12,0,0,0,0,0,0,0,0,0,12,28,44,44,29,21,0,0,0,0,0,17,39,44,44,14,5,0,0,0,21,29,44,44,32,12,19,37,36,46,46,44,37,4,20,30,46,31,15,1,0,0,0,0,0,0,0,0,0,9,18,44,44,35,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,20,44,44,40,22,0,0,0,0,0,22,40,46,46,46,27,24,0,0,0,0,0,0,0,2,18,28,40,29,18,12,0,0,0,21,43,29,39,34,13,4,0,0,19,37,36,46,
46,28,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,33,44,44,27,24,0,0,0,0,0,0,0,0,0,17,39,44,44,44,18,9,0,0,0,0,0,0,0,0,0,0,23,33,44,44,27,24,0,0,0,0,0,0,0,42,15,44,44,44,32,2,0,0,0,0,0,0,0,0,0,0,22,40,44,44,20,8,0,0,0,0,0,0,0,0,0,0,0,0,0,21,28,44,44,44,37,3,0,0,0,0,0,0,0,0,0,19,37,44,44,44,28,12,0,0,0,0,0,0,0,0,0,0,12,28,46,46,28,21,0,0,0,0,0,1,43,35,46,46,44,37,8,13,44,46,46,
39,10,26,32,38,46,46,39,23,0,0,0,0,0,26,29,44,44,44,43,7,0,0,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,19,37,36,46,46,46,46,46,46,31,16,6,1,43,35,46,46,44,37,8,13,44,46,46,39,10,26,32,38,46,46,39,23,0,0,0,0,0,0,0,0,22,32,34,34,34,34,34,34,34,34,34,43,1,0,22,32,34,34,34,34,34,34,34,34,34,43,1,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,
34,34,43,1,7,13,34,34,34,20,2,0,0,0,0,0,0,7,13,34,34,34,20,2,0,7,13,34,34,34,20,2,0,11,34,34,34,34,25,0,7,13,34,34,34,32,22,42,37,34,34,34,18,3,0,0,8,37,34,32,20,13,21,0,0,0,0,0,0,0,0,0,0,1,24,13,13,24,42,0,0,0,0,0,4,25,13,13,11,3,0,0,0,6,25,13,13,11,19,7,13,34,34,34,32,22,42,37,34,34,34,18,3,0,0,0,0,0,0,0,0,0,0,17,13,13,13,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,23,13,13,25,5,0,0,0,0,0,12,20,34,34,34,
14,26,0,0,0,0,0,0,0,0,1,21,22,26,1,0,0,0,0,0,42,17,22,21,3,0,0,0,7,13,34,34,34,20,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,13,13,13,13,21,0,0,0,0,0,0,0,0,0,4,25,13,13,13,17,0,0,0,0,0,0,0,0,0,0,0,2,13,13,13,13,21,0,0,0,0,0,0,0,0,22,13,13,13,11,3,0,0,0,0,0,0,0,0,0,0,5,25,13,13,23,9,0,0,0,0,0,0,0,0,0,0,0,0,0,1,24,13,13,13,22,0,0,0,0,0,0,0,0,0,0,0,22,13,13,13,24,1,0,
0,0,0,0,0,0,0,0,0,5,15,32,32,16,2,0,0,0,0,0,1,43,34,34,34,14,17,0,24,34,34,34,14,21,7,25,34,34,34,34,23,0,0,0,0,0,8,25,13,13,13,10,0,0,0,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,7,13,34,34,34,34,34,34,34,34,43,1,1,43,34,34,34,14,17,0,24,34,34,34,14,21,7,25,34,34,34,34,23,0,0,0,0,0,0,0,0,7,12,12,12,12,12,12,12,12,12,12,4,0,0,7,12,12,12,12,12,12,12,12,12,12,4,0,0,0,0,0,
0,12,12,12,12,1,0,0,0,0,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,8,12,12,12,2,0,0,3,12,12,12,12,1,0,0,8,12,12,12,12,7,0,4,12,12,12,8,0,0,0,0,4,12,12,4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,12,7,0,4,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,12,12,12,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,2,5,0,0,0,0,0,0,0,4,12,12,12,12,0,0,1,12,12,12,12,0,0,1,12,12,12,12,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,4,12,12,12,12,0,0,1,12,12,12,12,0,0,1,12,12,12,12,3,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,1,2,2,5,2,2,4,0,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,19,19,19,19,19,19,19,19,9,0,0,0,0,0,0,8,2,2,4,2,2,19,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,2,2,4,0,
0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,0,0,19,19,19,7,0,0,3,19,19,19,3,0,0,0,0,3,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,7,4,4,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,1,0,0,0,0,0,0,0,0,0,0,0,19,19,3,0,0,0,7,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,8,5,4,19,0,0,0,0,0,0,0,0,0,3,19,19,19,19,7,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,19,13,14,14,14,14,14,37,4,0,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,11,37,37,37,37,37,37,37,37,24,7,0,0,0,0,1,43,14,14,16,14,14,25,0,0,0,0,0,0,0,0,0,0,12,25,37,20,14,16,13,
26,7,0,0,0,0,11,37,37,37,37,37,37,37,37,37,37,11,9,0,0,0,0,0,0,0,0,26,14,32,15,5,0,0,0,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,0,0,0,11,37,37,37,18,4,5,18,37,37,37,18,4,0,0,12,43,37,37,37,37,37,37,37,37,37,23,0,0,0,0,0,0,0,0,1,22,18,20,14,20,18,22,42,0,0,0,0,0,0,0,0,0,12,24,37,20,20,37,25,12,0,0,0,0,0,0,0,0,21,37,37,37,12,0,8,13,37,37,22,0,0,0,0,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,
10,10,10,26,9,0,0,0,2,25,16,14,20,37,24,4,0,0,0,0,0,12,24,43,37,37,37,37,25,1,0,0,0,0,0,0,1,4,4,8,9,0,17,10,7,0,0,0,0,42,21,17,17,21,42,0,0,0,0,0,0,17,16,20,20,24,7,0,0,0,0,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,0,21,15,20,20,22,0,0,0,0,0,0,0,19,22,25,22,23,25,11,19,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,17,16,20,20,24,7,0,0,0,0,0,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,0,0,21,15,20,
20,22,0,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,2,10,10,10,12,19,12,26,3,0,0,0,0,0,17,16,20,20,24,7,0,0,0,0,0,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,22,29,30,31,27,46,44,16,5,0,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,19,37,36,36,36,36,36,36,36,
36,16,6,0,0,0,0,4,20,38,38,29,46,41,43,7,0,0,0,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,19,37,36,36,36,36,36,36,36,36,36,36,15,1,0,0,0,0,0,0,0,21,13,40,46,28,37,11,2,0,0,19,37,36,36,36,28,12,12,32,36,36,36,15,1,0,0,0,19,37,36,36,36,28,12,18,33,36,36,36,28,21,0,0,10,39,36,36,36,36,36,36,36,36,41,43,3,0,0,0,0,0,0,2,18,29,44,31,30,31,44,29,18,2,0,0,0,0,0,0,7,22,20,27,36,30,30,36,27,14,22,7,0,0,0,0,0,0,24,27,36,40,22,
0,12,32,36,41,37,19,0,0,0,0,42,37,39,39,39,37,19,0,0,0,0,0,0,0,0,0,0,0,0,0,24,39,39,39,32,21,0,0,4,37,33,31,30,38,41,39,22,0,0,0,0,21,15,33,41,36,36,36,36,14,5,0,0,0,0,12,25,37,20,14,16,13,10,14,28,22,0,0,0,10,37,28,39,39,28,37,17,0,0,0,0,0,23,39,38,38,32,17,0,0,0,0,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,0,23,29,35,39,14,35,28,21,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,23,
39,38,38,32,17,0,0,0,0,0,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,3,43,39,39,39,15,43,14,29,10,0,0,0,0,0,23,39,38,38,32,17,0,0,0,0,0,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,0,0,0,19,43,41,48,48,48,38,20,8,19,0,0,0,0,4,20,38,38,29,46,41,43,19,19,0,0,0,0,19,19,43,41,48,48,48,38,20,8,19,0,0,0,0,19,4,20,38,38,29,46,41,43,19,19,0,0,0,0,19,2,20,41,
38,29,30,44,20,26,19,0,0,0,9,19,4,20,38,38,29,46,41,43,19,0,0,19,37,36,51,51,52,52,52,52,31,16,6,0,0,19,19,4,20,38,38,29,46,41,43,19,19,19,7,0,0,0,0,21,20,41,48,51,50,50,52,50,31,29,22,0,0,19,37,36,51,51,52,52,52,52,51,51,31,15,1,0,0,0,0,0,9,22,14,35,31,45,38,41,33,15,4,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,19,37,36,51,47,28,13,40,46,52,30,39,24,3,0,0,10,39,48,52,52,52,52,51,51,51,41,43,3,0,0,0,0,0,5,43,27,46,41,33,39,33,41,
46,27,37,5,0,0,0,0,0,22,34,36,38,27,40,40,27,38,36,28,23,0,0,0,0,0,1,37,41,30,32,12,0,42,37,36,30,14,12,0,0,0,0,8,16,38,48,31,15,1,0,0,0,0,0,0,0,0,0,0,0,0,0,25,35,48,45,29,26,0,0,10,39,47,49,31,31,47,40,22,0,0,0,1,43,35,48,52,49,35,35,30,14,5,0,0,0,10,20,33,36,30,30,31,35,32,41,36,20,4,0,21,14,44,46,45,45,46,44,20,21,0,0,0,0,6,13,33,47,44,37,5,0,0,0,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,0,22,29,46,44,45,33,25,7,
0,0,0,0,1,37,36,31,46,35,46,32,12,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,6,13,33,47,44,37,5,0,0,0,0,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,9,25,40,46,48,27,44,30,27,25,0,0,0,0,0,6,13,33,47,44,37,5,0,0,0,0,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,0,11,37,37,33,33,33,33,33,16,37,37,11,9,0,11,37,37,34,34,20,34,34,37,37,37,11,9,0,22,37,37,
37,33,33,33,33,33,16,37,37,11,0,0,22,37,37,37,34,34,20,34,34,37,37,37,11,0,0,22,37,37,14,39,29,39,39,20,37,37,37,11,0,3,25,37,37,37,34,34,20,34,34,37,37,21,0,19,37,36,51,47,41,41,41,41,41,16,6,0,11,37,37,37,37,34,34,20,34,34,25,37,37,37,18,4,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,19,37,36,51,47,41,41,41,41,46,51,31,15,1,0,0,0,0,0,26,14,36,49,48,48,46,48,30,14,5,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,19,37,36,51,47,28,39,30,51,30,39,
25,1,0,0,0,10,39,41,41,41,41,41,31,51,48,27,18,7,0,0,0,0,3,25,33,46,33,37,11,10,23,37,33,46,33,25,3,0,0,0,21,14,36,41,32,13,22,22,25,32,41,36,14,21,0,0,0,0,12,32,30,41,37,1,0,0,25,27,45,39,22,0,0,0,0,8,16,38,51,31,15,5,12,4,0,0,0,0,0,0,0,0,42,12,12,25,35,53,45,29,26,0,0,23,33,50,46,34,20,29,39,22,0,0,0,12,32,30,51,54,49,40,35,30,14,5,0,0,21,20,41,48,51,50,50,52,50,31,30,39,25,3,9,13,35,49,50,31,36,48,49,27,25,0,0,0,0,0,4,43,
27,45,33,25,9,0,0,0,0,0,0,3,22,29,30,44,16,12,0,0,0,0,0,2,16,41,36,14,44,38,32,26,0,0,0,0,4,20,31,39,33,31,35,37,1,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,0,0,4,43,27,45,33,25,9,0,0,0,0,0,0,0,9,22,29,30,44,16,12,0,0,0,0,0,0,2,16,41,36,14,44,38,32,26,0,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,23,14,40,44,49,49,44,40,14,23,0,0,0,0,0,0,4,43,27,45,33,25,9,0,0,0,0,0,0,0,9,22,29,30,44,16,12,0,0,0,0,0,
19,37,36,36,36,14,23,13,27,36,36,36,15,1,19,37,36,36,36,14,2,13,27,36,36,36,15,1,7,18,44,36,36,27,25,23,11,33,36,36,41,37,3,7,18,44,36,36,27,25,2,11,33,36,36,41,37,3,7,18,44,36,36,27,25,17,11,33,36,36,41,37,3,4,20,36,36,36,14,12,24,27,36,36,33,23,0,19,37,36,51,47,28,43,43,43,43,11,9,19,37,36,36,36,28,12,12,2,12,12,37,36,36,36,28,12,0,0,12,14,30,51,47,39,25,11,32,38,51,45,39,10,0,19,37,36,51,47,28,43,43,32,46,51,31,15,1,0,0,0,0,3,18,35,50,30,33,47,28,40,
41,14,5,0,19,37,36,51,47,28,22,22,32,46,51,31,15,1,0,0,0,19,37,36,51,47,29,30,52,46,33,13,8,0,0,0,0,5,18,43,43,43,43,27,48,53,41,20,12,0,0,0,0,0,21,32,30,33,15,29,15,12,15,29,16,33,30,32,21,0,0,7,13,27,36,14,28,28,24,11,32,28,14,36,35,13,3,0,0,0,22,40,48,27,24,0,0,0,10,39,45,35,13,9,0,0,0,8,16,38,51,31,15,14,32,16,11,1,0,0,0,0,0,10,37,32,32,37,35,53,45,29,26,0,0,10,39,46,50,31,33,20,11,6,0,0,0,26,29,45,54,54,49,40,35,30,14,5,0,3,
13,35,49,52,38,27,33,36,50,53,41,15,4,0,42,15,31,51,30,32,15,41,52,36,37,1,0,0,0,22,43,20,14,39,39,15,24,4,0,0,0,0,22,43,20,39,39,14,15,24,4,0,0,0,0,22,14,39,28,32,14,39,29,10,0,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,0,12,25,20,32,39,39,18,4,0,0,0,0,0,0,12,25,20,39,39,20,24,4,0,0,0,0,0,0,12,14,39,28,32,14,39,29,10,0,0,0,0,0,12,25,20,32,32,16,13,23,9,0,0,0,0,25,35,30,44,35,48,30,29,23,9,
0,0,0,0,0,12,13,20,32,39,39,18,2,0,0,0,0,0,0,12,13,20,39,39,20,25,2,0,0,0,0,0,19,37,36,51,30,14,21,14,38,51,51,31,15,1,19,37,36,51,30,14,21,14,38,51,51,31,15,1,0,10,29,47,51,31,20,5,15,36,52,45,39,23,0,0,10,29,47,51,31,20,5,15,36,52,45,39,23,0,0,10,29,47,51,31,20,5,15,36,52,45,39,23,0,4,20,30,51,30,14,4,24,27,53,50,33,23,0,19,37,36,51,47,28,12,7,7,7,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,10,39,45,51,36,15,8,7,13,35,
53,50,27,24,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,12,32,30,49,27,40,47,28,23,16,37,4,0,19,37,36,51,47,40,40,40,40,46,51,31,15,1,0,0,0,19,37,36,51,47,31,52,47,33,18,4,0,0,0,0,0,0,7,7,7,24,40,47,52,38,28,22,0,0,0,0,0,0,25,27,36,16,27,45,27,25,27,45,35,16,36,27,25,0,0,2,14,38,40,14,38,38,14,20,31,30,34,40,30,14,12,0,0,9,13,35,47,29,17,0,0,0,5,14,38,31,16,4,0,0,0,8,16,38,51,31,27,30,46,36,39,24,7,0,0,0,10,14,44,30,30,44,
35,53,45,29,26,0,0,22,32,44,50,53,48,36,29,24,9,0,0,21,28,46,54,54,49,40,35,30,14,5,0,12,14,30,51,47,39,25,43,27,45,51,47,29,10,0,8,16,38,51,31,20,35,30,47,31,15,1,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,
36,40,18,4,0,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,22,29,32,39,33,41,53,36,14,21,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,19,37,36,51,30,14,25,27,49,54,51,31,15,1,19,37,36,51,30,14,25,27,49,54,51,31,15,1,0,8,37,41,52,45,40,23,39,47,51,31,20,5,0,0,8,37,41,52,45,40,23,39,47,51,31,20,5,0,0,8,37,41,52,45,40,23,39,47,51,31,20,5,0,4,20,30,51,30,14,5,24,27,53,50,33,23,0,19,37,36,51,47,28,12,0,0,
0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,10,39,45,46,28,40,47,28,23,23,22,42,0,19,37,36,51,53,49,49,49,49,50,51,31,15,1,0,0,0,19,37,36,51,52,52,45,27,37,5,0,0,0,0,0,0,0,0,0,22,28,38,52,47,40,24,7,0,0,0,0,0,19,37,41,27,25,27,45,27,25,27,45,35,13,27,41,37,19,0,10,39,38,14,14,38,38,14,14,31,30,34,20,38,39,10,0,0,4,16,31,38,20,5,0,
0,0,3,18,44,46,28,21,0,0,0,8,16,38,51,31,45,47,50,52,38,32,21,0,0,6,37,44,50,53,45,45,44,53,45,29,26,0,1,37,41,50,41,35,46,52,31,14,12,0,0,8,15,44,50,51,49,40,35,30,14,5,0,10,39,45,51,36,15,18,33,47,35,53,50,27,24,0,8,16,38,51,31,27,48,38,29,14,13,3,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,1,43,35,
48,50,46,46,53,46,40,11,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,7,25,39,31,45,49,45,53,49,35,18,3,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,0,19,37,36,51,30,14,14,31,50,45,51,31,15,1,19,37,36,51,30,14,14,31,50,45,51,31,15,1,0,0,23,40,45,52,41,15,44,53,49,27,25,0,0,0,0,23,40,45,52,41,15,44,53,49,27,25,0,0,0,0,23,40,45,52,
41,15,44,53,49,27,25,0,0,4,20,30,51,30,14,5,24,27,53,50,33,23,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,28,22,22,21,8,19,37,36,51,47,28,12,0,0,22,33,49,52,44,13,0,0,23,33,49,52,44,13,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,22,33,49,30,20,40,47,39,33,33,39,22,0,19,37,36,51,49,47,47,47,47,49,51,31,15,1,0,0,0,19,37,36,51,52,52,50,44,20,26,0,0,0,0,0,0,0,0,12,20,41,50,48,27,43,4,0,0,0,0,0,0,8,16,31,39,10,16,29,16,21,15,29,
20,10,39,31,16,8,0,22,33,36,37,25,28,28,25,24,28,29,25,37,36,33,23,0,0,26,29,47,44,18,7,0,0,0,0,23,33,49,33,11,0,0,0,8,16,38,51,48,27,29,44,50,49,27,24,0,0,26,29,46,51,31,39,40,30,51,45,29,26,0,5,14,30,45,29,18,29,46,45,39,17,0,0,0,10,14,35,30,49,40,35,30,14,5,0,22,33,49,52,44,18,40,46,35,33,49,53,35,13,0,8,16,38,51,31,44,52,31,34,25,5,0,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,19,37,35,29,14,14,40,45,50,35,
25,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,21,32,38,52,48,38,38,45,51,30,32,12,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,0,19,37,36,51,30,14,33,48,30,30,51,31,15,1,19,37,36,51,30,14,33,48,30,30,51,31,15,1,0,0,5,20,
31,51,47,29,30,51,30,32,21,0,0,0,0,5,20,31,51,47,29,30,51,30,32,21,0,0,0,0,5,20,31,51,47,29,30,51,30,32,21,0,0,4,20,38,51,47,28,15,15,27,53,50,33,23,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,40,40,39,28,37,22,37,36,51,47,28,12,0,0,22,40,48,52,41,43,19,0,24,27,50,53,27,25,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,22,33,49,38,20,40,47,33,49,49,40,22,0,19,37,36,51,47,28,28,28,28,46,51,36,37,1,0,0,0,19,37,36,51,47,31,52,49,44,16,21,
0,0,0,0,0,0,8,43,27,48,53,41,20,12,0,0,0,0,0,0,0,8,16,38,39,10,10,26,5,0,5,26,22,17,39,38,20,8,0,23,33,36,37,19,22,43,20,20,43,22,1,37,41,33,23,0,0,24,27,48,33,23,0,0,0,0,0,21,28,46,44,43,3,0,0,8,16,38,51,38,14,17,29,47,52,41,43,7,0,23,33,49,50,27,25,18,44,52,45,29,26,0,4,20,38,50,41,29,32,30,47,29,26,0,0,0,0,17,43,29,45,40,35,30,14,5,0,22,33,49,52,44,39,30,44,20,33,49,53,44,13,0,8,16,38,51,31,27,49,50,38,33,37,5,0,0,9,11,32,27,
44,41,41,46,52,44,18,7,0,9,11,32,27,44,41,41,46,52,44,18,7,0,9,11,32,27,44,41,41,46,52,44,18,7,0,9,11,32,27,44,41,41,46,52,44,18,7,0,9,11,32,27,44,41,41,46,52,44,18,7,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,53,44,20,20,35,53,48,39,22,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,0,
19,37,36,51,30,14,31,50,35,30,51,31,15,1,19,37,36,51,30,14,31,50,35,30,51,31,15,1,0,0,0,25,27,49,53,44,49,50,35,18,3,0,0,0,0,0,25,27,49,53,44,49,50,35,18,3,0,0,0,0,0,25,27,49,53,44,49,50,35,18,3,0,0,1,37,36,52,53,38,31,31,31,53,50,33,23,0,19,37,36,51,47,28,23,23,21,0,0,0,19,37,36,51,53,49,49,48,46,41,34,37,36,51,47,28,12,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,19,37,36,51,47,28,12,12,32,46,51,31,15,23,23,5,0,0,22,40,48,30,14,40,47,40,27,
49,40,22,0,19,37,36,51,47,28,21,21,32,46,51,36,37,23,23,5,0,19,37,36,51,47,29,30,52,48,35,37,23,23,12,0,0,7,24,39,46,51,38,28,23,23,23,23,23,22,7,0,0,1,37,36,33,43,39,43,4,0,4,18,39,37,33,36,37,1,0,22,39,31,16,25,29,44,38,38,44,29,25,16,31,40,22,0,19,37,41,46,34,12,0,0,0,0,0,8,15,36,30,14,2,0,0,8,16,38,51,31,15,12,32,46,51,36,37,3,0,11,27,50,49,33,22,25,35,53,45,29,26,0,7,13,27,45,53,46,31,49,41,15,4,0,0,0,0,0,26,29,45,40,35,30,14,5,0,22,
40,48,52,41,38,41,14,24,27,50,53,35,25,0,8,16,38,51,31,14,44,45,51,47,33,24,0,0,21,32,31,49,49,45,47,49,52,41,18,7,0,21,32,31,49,49,45,47,49,52,41,18,7,0,21,32,31,49,49,45,47,49,52,41,18,7,0,21,32,31,49,49,45,47,49,52,41,18,7,0,21,32,31,49,49,45,47,49,52,41,18,7,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,49,33,22,11,27,50,49,33,
22,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,0,19,37,36,51,30,33,48,30,28,30,51,31,15,1,19,37,36,51,30,33,48,30,28,30,51,31,15,1,0,0,0,21,32,30,51,50,51,47,29,10,0,0,0,0,0,0,21,32,30,51,50,51,47,29,10,0,0,0,0,0,0,21,32,30,51,50,51,47,29,10,0,0,0,0,24,33,47,53,52,52,52,52,54,50,33,23,0,19,37,36,51,47,33,33,33,37,1,0,0,19,37,36,51,49,47,47,48,52,50,44,37,36,51,47,28,12,0,0,42,37,44,53,50,44,29,28,27,45,
52,31,14,2,0,19,37,36,51,47,28,12,12,32,46,51,31,33,33,33,24,0,0,21,28,46,45,39,40,47,28,27,49,40,22,0,19,37,36,51,47,28,12,12,32,46,51,36,33,33,33,24,0,19,37,36,51,47,28,39,30,51,45,27,33,33,18,7,0,10,34,31,52,47,33,33,33,33,33,33,33,28,21,0,0,9,13,35,44,39,46,40,37,25,37,40,46,39,44,35,13,9,0,12,34,46,29,39,30,44,33,40,44,30,39,29,46,28,21,0,12,14,30,36,15,42,0,0,0,0,0,0,25,35,45,39,22,0,0,8,16,38,51,38,20,21,28,46,52,41,43,3,0,23,33,49,50,27,11,13,
35,53,45,29,26,0,0,12,37,33,31,48,52,36,28,22,0,0,0,0,0,0,26,29,45,40,35,30,14,5,0,21,28,46,51,45,36,32,22,16,36,52,48,40,22,0,8,16,38,51,31,15,15,33,46,52,36,37,19,0,23,33,49,49,35,29,29,46,52,41,18,7,0,23,33,49,49,35,29,29,46,52,41,18,7,0,23,33,49,49,35,29,29,46,52,41,18,7,0,23,33,49,49,35,29,29,46,52,41,18,7,0,23,33,49,49,35,29,29,46,52,41,18,7,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,48,27,27,27,
27,27,27,29,21,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,0,19,37,36,51,30,36,50,35,20,30,51,31,15,1,19,37,36,51,30,36,50,35,20,30,51,31,15,1,0,0,0,3,13,35,50,54,52,41,37,8,0,0,0,0,0,0,3,13,35,50,54,52,41,37,8,0,0,0,0,0,0,3,13,35,50,54,52,41,37,8,0,0,0,0,5,43,40,44,44,44,44,44,53,50,33,23,0,19,37,36,51,53,49,49,31,15,
1,0,0,19,37,36,51,47,28,28,40,46,51,30,14,36,51,47,28,12,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,19,37,36,51,47,28,12,12,32,46,54,50,49,49,35,25,0,0,8,15,41,53,36,40,47,28,27,49,40,22,0,19,37,36,51,47,28,12,12,32,46,54,50,49,49,27,24,0,19,37,36,51,47,28,13,33,46,51,49,49,41,18,7,0,11,27,50,54,53,49,49,49,49,49,49,47,29,21,0,0,0,10,29,46,39,27,46,41,35,41,46,27,39,46,29,22,0,0,1,37,44,44,44,35,15,22,22,15,35,44,44,44,37,1,0,22,39,45,27,25,0,0,0,
0,0,0,0,10,39,45,35,25,0,0,8,16,38,51,47,40,20,27,49,50,35,25,0,0,10,39,45,52,41,14,28,38,51,45,29,26,0,0,12,15,43,14,33,46,53,44,13,9,0,0,0,0,0,26,29,45,40,35,30,14,5,0,42,37,44,53,50,41,29,28,27,45,51,31,14,12,0,8,16,38,51,31,29,15,16,31,51,31,15,1,0,24,27,50,48,39,37,33,48,52,41,18,7,0,24,27,50,48,39,37,33,48,52,41,18,7,0,24,27,50,48,39,37,33,48,52,41,18,7,0,24,27,50,48,39,37,33,48,52,41,18,7,0,24,27,50,48,39,37,33,48,52,41,18,7,0,10,39,47,
53,44,20,37,20,39,44,37,19,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,0,19,37,36,51,46,45,46,29,20,30,51,31,15,1,19,37,36,51,46,45,46,29,20,30,51,31,15,1,0,0,0,17,23,29,46,54,45,40,23,0,0,0,0,0,0,0,17,23,29,46,54,45,40,23,0,0,0,0,0,0,0,17,23,29,
46,54,45,40,23,0,0,0,0,0,0,4,23,13,13,13,13,27,53,50,33,23,0,19,37,36,46,46,48,51,31,15,1,0,0,19,37,36,51,47,28,21,15,31,51,46,32,36,51,47,28,12,0,0,0,3,24,29,41,46,48,48,52,36,39,18,4,0,0,19,37,36,46,46,28,12,12,32,46,46,46,53,53,35,25,0,0,0,23,39,30,50,30,47,41,46,49,40,22,0,19,37,36,46,46,28,12,12,32,46,46,46,53,53,27,24,0,19,37,36,46,46,28,12,18,33,46,49,52,41,18,7,0,11,27,46,46,46,46,46,46,46,47,51,47,29,21,0,0,0,6,37,35,31,28,39,35,41,35,39,
28,31,35,37,8,0,0,0,22,29,30,27,20,22,2,2,22,20,27,30,29,22,0,9,13,35,45,39,10,0,0,0,0,0,0,0,2,14,30,36,15,8,0,8,16,38,51,30,47,38,45,51,46,29,10,0,0,4,16,36,53,49,30,30,31,53,45,29,26,0,0,10,39,44,33,39,31,52,44,18,7,0,0,0,0,0,26,29,45,40,35,30,14,5,0,26,20,41,46,53,50,47,46,49,52,46,40,24,9,0,8,16,38,51,31,35,36,36,48,53,44,43,7,0,22,40,45,52,30,36,46,47,52,41,18,7,0,22,40,45,52,30,36,46,47,52,41,18,7,0,22,40,45,52,30,36,46,47,52,41,
18,7,0,22,40,45,52,30,36,46,47,52,41,18,7,0,22,40,45,52,30,36,46,47,52,41,18,7,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,49,30,30,50,49,44,37,42,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,0,19,37,36,51,51,53,44,43,20,30,51,31,15,1,19,37,36,51,51,53,44,43,20,30,51,31,15,1,0,0,4,20,
33,27,47,51,31,20,2,0,0,0,0,0,0,4,20,33,27,47,51,31,20,2,0,0,0,0,0,0,4,20,33,27,47,51,31,20,2,0,0,0,0,0,0,0,0,9,9,9,24,27,53,50,33,23,0,7,13,34,34,34,46,51,31,15,1,0,0,19,37,36,51,47,29,29,40,46,51,30,14,36,51,47,28,12,0,0,0,0,19,22,15,28,39,27,48,47,33,13,1,0,0,7,13,34,34,34,20,2,5,16,34,34,33,49,53,35,25,0,0,0,1,13,40,31,45,50,48,30,44,29,10,0,7,13,34,34,34,20,2,5,16,34,34,33,49,53,27,24,0,7,13,34,34,34,20,2,5,37,29,45,
52,41,18,7,0,22,32,34,34,34,34,34,34,34,36,51,47,29,21,0,0,0,0,21,20,44,38,33,28,14,34,33,38,44,20,21,0,0,0,0,19,25,40,30,44,39,14,14,39,44,30,40,25,19,0,8,15,36,38,14,5,0,0,0,0,0,0,0,3,43,44,46,28,21,0,8,16,38,46,31,44,45,48,46,33,18,1,0,0,0,23,29,31,48,48,31,35,46,46,29,26,0,0,10,39,48,49,45,50,46,39,23,0,0,0,0,0,0,26,29,45,40,35,30,14,5,19,37,44,30,39,41,46,48,48,45,31,40,18,4,0,0,8,16,38,46,31,35,45,49,45,31,29,22,0,0,5,16,44,45,
48,46,27,46,46,41,18,7,0,5,16,44,45,48,46,27,46,46,41,18,7,0,5,16,44,45,48,46,27,46,46,41,18,7,0,5,16,44,45,48,46,27,46,46,41,18,7,0,5,16,44,45,48,46,27,46,46,41,18,7,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,
19,37,36,51,54,46,29,22,20,30,51,31,15,1,19,37,36,51,54,46,29,22,20,30,51,31,15,1,0,0,5,14,30,50,51,48,27,25,9,0,0,0,0,0,0,5,14,30,50,51,48,27,25,9,0,0,0,0,0,0,5,14,30,50,51,48,27,25,9,0,0,0,0,0,0,0,0,0,0,0,24,27,53,50,33,23,0,0,8,12,12,32,46,52,31,15,1,0,0,19,37,36,51,49,47,47,48,52,48,35,37,36,51,47,28,12,0,0,0,0,0,0,8,21,10,16,44,50,30,39,11,0,0,0,8,12,12,12,2,0,0,5,12,22,33,49,53,35,25,0,0,0,0,8,24,20,40,47,39,32,
43,10,3,0,0,8,12,12,12,2,0,0,5,12,23,33,49,53,27,24,0,0,8,12,12,12,2,0,0,26,29,45,52,41,18,7,0,7,12,12,12,12,12,12,12,37,36,52,47,29,21,0,0,0,0,0,17,20,27,38,46,30,46,38,27,20,10,0,0,0,0,0,0,8,13,29,41,46,30,30,46,41,29,13,4,0,0,12,28,35,35,43,3,0,0,0,0,0,0,0,0,11,33,35,33,22,0,1,43,34,34,34,15,39,40,28,18,5,0,0,0,0,19,11,14,39,39,14,34,34,34,20,12,0,0,17,32,33,35,44,35,29,13,8,0,0,0,0,0,0,26,29,44,40,35,44,14,5,0,11,
29,33,13,15,29,40,40,29,20,24,4,0,0,0,1,43,34,34,34,14,29,40,39,14,11,3,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,22,15,29,40,40,28,37,17,0,
0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,19,37,36,46,46,44,37,4,20,30,46,31,15,1,19,37,36,46,46,44,37,4,20,30,46,31,15,1,0,0,5,14,30,46,38,35,16,12,0,0,0,0,0,0,0,5,14,30,46,38,35,16,12,0,0,0,0,0,0,0,5,14,30,46,38,35,16,12,0,0,0,0,0,0,0,0,0,0,0,0,24,27,46,46,33,23,0,0,0,0,12,32,44,44,44,15,1,0,0,19,37,36,46,46,46,46,46,31,27,20,37,36,46,46,28,12,0,0,0,0,0,0,0,0,0,17,14,27,
27,27,37,19,0,0,0,0,0,0,0,0,0,0,0,22,33,44,44,35,25,0,0,0,0,0,0,10,39,33,32,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,23,33,44,44,27,24,0,0,0,0,0,0,0,0,0,26,29,44,44,44,18,7,0,0,0,0,0,0,0,0,3,37,44,44,44,29,21,0,0,0,0,0,0,12,13,14,29,39,29,14,13,12,0,0,0,0,0,0,0,0,1,22,37,28,39,39,28,37,22,1,0,0,0,19,11,25,25,26,0,0,0,0,0,0,0,0,0,2,25,25,25,5,0,0,4,12,12,12,4,10,22,21,1,0,0,0,0,0,0,9,12,22,22,12,
12,12,12,2,0,0,0,9,21,23,25,13,25,10,1,0,0,0,0,0,0,0,8,25,13,13,13,13,11,3,0,19,24,13,4,8,21,22,22,17,5,0,0,0,0,0,0,4,12,12,12,5,17,22,10,2,0,0,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,
17,2,19,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,7,13,34,34,34,32,22,42,37,34,34,34,18,3,7,13,34,34,34,32,22,42,37,34,34,34,18,3,0,0,8,37,34,32,20,13,21,0,0,0,0,0,0,0,0,8,37,34,32,20,13,21,0,0,0,0,0,0,0,0,8,37,34,32,20,13,21,0,0,0,0,0,0,0,0,0,0,0,0,0,23,32,34,34,32,22,0,0,0,0,19,24,13,13,13,22,
0,0,0,7,13,34,34,34,34,34,32,20,13,12,13,34,34,34,20,2,0,0,0,0,0,0,0,0,0,0,10,11,11,11,21,0,0,0,0,0,0,0,0,0,0,0,0,2,13,13,13,13,21,0,0,0,0,0,0,1,22,23,10,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,13,13,13,13,21,0,0,0,0,0,0,0,0,0,8,25,13,13,13,17,0,0,0,0,0,0,0,0,0,0,10,13,13,13,24,42,0,0,0,0,0,0,0,7,5,26,10,26,5,7,0,0,0,0,0,0,0,0,0,0,0,6,12,10,10,21,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,9,9,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,12,7,0,4,12,12,12,8,0,0,8,12,12,12,12,7,0,4,12,12,12,8,0,0,0,0,4,12,12,4,7,0,0,0,0,0,0,0,0,0,0,4,12,12,4,7,0,0,0,0,0,0,0,0,0,0,4,12,12,
4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,12,12,12,12,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,12,12,12,4,7,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,8,8,0,1,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,8,8,0,1,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,1,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,42,8,8,0,1,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,8,8,8,8,8,1,0,0,0,0,0,9,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,
0,0,0,0,21,15,20,20,22,0,0,0,0,0,0,0,0,19,22,25,22,23,25,11,19,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,17,16,20,20,24,7,0,0,0,0,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,0,21,15,20,20,22,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,0,21,15,20,20,22,0,0,0,0,0,0,0,0,21,10,10,10,5,0,0,0,0,5,
37,20,15,21,37,20,15,21,0,0,0,0,0,0,0,0,8,10,10,10,26,42,8,8,1,0,0,0,0,0,0,0,0,0,0,8,10,10,10,26,9,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,0,21,10,10,10,5,0,0,0,0,0,0,5,37,20,15,21,37,20,15,21,0,0,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,0,0,26,16,20,15,20,20,18,42,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,21,15,20,
20,22,0,0,0,0,0,5,37,20,15,21,37,20,15,21,0,0,0,0,19,22,25,22,23,25,11,19,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,0,26,16,20,15,20,20,18,42,0,0,0,0,11,20,20,20,22,1,4,4,8,9,0,0,0,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,9,11,20,20,20,22,0,0,0,0,0,4,17,22,23,22,21,42,0,0,0,0,0,0,0,4,17,22,22,26,8,0,0,0,0,0,21,24,24,23,24,24,10,0,0,
0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,0,0,0,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,0,0,23,29,35,39,14,35,28,21,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,23,39,38,38,32,17,0,0,0,0,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,0,12,20,41,
38,27,13,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,0,4,20,39,39,39,18,7,0,0,0,12,32,38,35,37,33,38,39,22,0,0,0,0,0,0,0,0,24,39,39,39,32,37,20,20,18,6,0,0,0,0,0,0,0,21,22,24,39,39,39,32,22,19,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,0,4,20,39,39,39,18,7,0,0,0,0,0,12,32,38,35,37,33,38,39,22,0,0,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,
0,0,0,3,13,35,38,39,38,36,20,5,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,12,32,38,35,37,33,38,39,22,0,0,0,0,23,29,35,39,14,35,28,21,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,3,13,35,38,39,38,36,20,5,0,0,0,21,14,36,38,33,24,37,20,14,16,13,26,9,0,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,21,14,31,38,40,24,0,0,0,9,22,16,29,
33,33,40,28,37,2,0,0,0,0,0,10,15,29,33,33,29,37,26,0,0,0,7,43,27,27,28,27,27,20,8,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,0,0,0,0,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,0,0,1,37,36,31,46,35,46,32,12,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,6,13,33,47,44,37,5,0,0,0,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,
7,43,41,48,48,48,38,20,8,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,0,0,5,14,30,48,41,43,3,0,0,0,7,25,33,45,33,30,35,43,42,0,0,0,0,0,0,0,0,25,35,48,45,29,39,38,31,20,5,0,0,0,0,0,0,1,37,33,33,35,48,45,33,29,10,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,0,5,14,30,48,41,43,3,0,0,0,0,0,7,25,33,45,33,30,35,43,42,
0,0,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,0,0,21,32,30,35,27,46,39,23,0,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,7,25,33,45,33,30,35,43,42,0,0,0,1,37,36,31,46,35,46,32,12,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,21,32,30,35,27,46,39,23,0,0,0,8,43,35,45,27,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,0,4,37,35,45,27,
18,4,0,0,0,0,0,0,0,4,37,35,45,27,18,4,0,0,4,37,35,45,27,18,4,0,0,0,21,14,41,45,49,49,48,46,29,26,0,0,0,0,21,14,44,47,49,49,47,44,20,12,0,0,7,43,41,47,28,30,38,20,8,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,0,0,4,37,35,45,27,18,4,0,0,0,0,0,0,0,0,0,0,2,16,41,36,14,44,38,32,26,0,0,0,0,0,4,20,31,39,33,31,35,37,1,0,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,3,42,4,43,27,45,33,25,42,1,0,0,0,3,42,42,22,29,30,44,16,
12,1,0,0,0,3,2,16,41,36,14,44,38,32,26,0,0,0,3,42,43,44,44,28,44,44,20,8,0,0,0,0,7,43,33,33,33,33,33,16,8,0,0,0,0,0,3,22,34,44,31,41,39,24,0,0,0,0,0,0,22,29,30,44,16,12,0,0,0,0,0,2,16,41,36,14,44,38,32,26,0,0,0,0,0,5,14,30,45,41,43,3,0,0,0,0,2,16,41,47,38,32,26,0,0,0,0,0,0,42,12,12,25,35,53,45,29,27,50,27,25,9,0,0,0,0,0,0,42,15,31,49,49,51,53,48,39,10,0,0,0,7,43,33,33,33,33,33,16,8,0,0,0,0,0,0,0,22,34,44,
31,41,39,24,0,0,0,0,0,0,0,5,14,30,45,41,43,3,0,0,0,0,0,0,2,16,41,47,38,32,26,0,0,0,0,0,0,7,43,33,33,33,33,33,16,8,0,0,0,0,0,0,22,34,44,31,41,39,24,0,0,0,0,0,0,0,9,25,27,38,32,30,27,18,42,0,0,0,0,0,0,22,29,30,44,16,12,0,0,0,0,2,16,41,36,14,44,38,32,26,0,0,0,0,2,16,41,47,38,32,26,0,0,0,3,4,20,31,39,33,31,35,37,1,0,0,0,3,43,33,33,33,33,33,16,8,1,0,0,0,3,42,22,34,44,31,41,39,24,1,0,0,0,3,42,25,27,38,32,30,
27,18,42,0,0,0,11,39,47,35,20,44,48,51,50,50,52,50,31,28,22,0,0,0,0,0,0,24,40,47,27,37,2,42,1,0,0,0,0,0,0,24,40,47,27,37,2,0,0,0,24,40,47,27,37,2,0,0,0,0,24,27,50,45,41,41,36,30,29,26,0,0,0,3,18,35,49,50,31,31,53,48,27,25,9,0,7,43,44,44,28,44,44,20,8,0,0,3,43,44,44,28,44,44,20,8,0,0,0,0,0,0,0,24,40,47,27,37,2,0,0,0,0,0,0,0,0,0,0,0,12,14,39,28,32,32,39,29,10,0,0,0,0,0,19,13,15,20,32,32,16,25,2,0,0,0,0,0,0,10,
13,20,32,32,16,25,2,0,0,0,1,13,15,15,15,37,39,39,15,15,43,4,0,1,13,15,15,15,39,39,20,15,15,43,4,0,1,13,15,14,39,28,22,14,39,29,43,4,0,1,13,15,15,15,13,24,15,15,15,43,4,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,0,12,25,39,39,32,20,18,17,0,0,0,0,12,14,39,28,32,14,39,29,10,0,0,0,0,12,25,16,29,29,29,18,17,0,0,0,0,12,25,29,39,39,20,18,17,0,0,0,0,10,37,32,32,37,35,53,45,29,44,46,28,26,0,0,0,0,0,0,10,
37,32,33,33,35,53,45,33,39,10,0,0,0,0,12,25,20,32,32,16,24,4,0,0,0,0,0,0,12,25,20,32,32,16,24,4,0,0,0,0,0,0,12,25,20,29,29,29,18,4,0,0,0,0,0,0,12,25,29,39,39,16,24,4,0,0,0,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,0,0,12,18,39,39,28,39,14,21,0,0,0,0,0,5,25,16,39,39,14,15,13,12,0,0,0,12,14,39,28,32,14,39,29,10,0,0,0,5,25,16,29,39,39,15,13,12,0,1,13,15,15,15,37,25,16,15,15,43,4,0,1,13,
15,15,15,11,23,15,15,15,43,4,0,1,13,15,15,15,43,15,15,15,15,43,4,0,1,13,15,15,39,39,28,39,14,15,43,4,0,0,25,39,39,16,27,49,52,30,27,33,36,50,53,41,15,4,0,0,0,9,10,43,39,39,20,43,15,15,37,12,0,0,0,4,25,16,39,39,14,37,23,0,0,1,13,39,39,15,11,0,0,0,0,0,25,35,53,30,34,37,37,14,32,21,0,0,0,12,14,30,51,38,32,28,30,51,31,20,4,0,0,10,13,15,15,15,13,23,9,0,1,13,15,15,15,24,15,15,15,22,0,0,0,0,0,0,12,13,39,39,32,16,25,2,0,0,0,0,0,0,0,0,
0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,5,14,31,31,36,37,25,35,31,31,28,21,0,5,14,31,31,36,37,25,35,31,31,28,21,0,5,14,31,31,36,37,25,35,31,31,28,21,0,5,14,31,31,36,37,25,35,31,31,28,21,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,0,12,15,33,31,46,46,38,44,18,9,0,0,12,15,33,31,46,46,38,44,18,3,0,0,0,12,15,33,31,46,46,38,44,18,9,0,0,12,
15,33,31,46,46,38,44,18,9,0,0,10,14,44,30,30,44,35,53,45,29,39,39,37,1,0,0,0,0,0,10,14,44,30,30,44,35,53,45,29,22,1,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,1,18,33,31,46,46,30,36,27,11,0,0,1,18,33,31,46,46,
30,36,27,11,0,0,1,18,33,31,46,46,30,36,27,11,0,5,14,31,31,36,37,25,35,31,31,28,21,0,5,14,31,31,36,37,25,35,31,31,28,21,0,5,14,31,31,36,37,25,35,31,31,28,21,0,5,14,31,31,36,37,25,35,31,31,28,21,0,0,4,17,17,14,38,51,47,39,25,11,32,31,51,45,39,10,0,0,0,22,32,44,30,46,38,33,31,31,39,10,0,0,3,13,33,31,46,46,30,36,15,1,0,5,14,31,31,41,43,3,0,0,0,0,22,39,46,50,30,36,27,28,13,4,0,0,0,10,39,45,53,35,13,18,44,52,47,29,21,0,0,5,14,31,31,41,43,3,0,0,
5,14,31,31,41,43,33,31,36,14,21,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,0,0,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,5,14,30,51,36,37,25,35,53,47,28,21,0,5,14,30,51,36,37,25,35,53,47,28,21,0,5,14,30,51,36,37,25,35,53,47,28,21,0,5,14,30,51,36,37,25,35,53,47,28,21,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,1,43,35,45,51,48,47,
48,44,18,9,0,1,43,35,45,51,48,47,48,44,18,9,0,0,1,43,35,45,51,48,47,48,44,18,9,0,1,43,35,45,51,48,47,48,44,18,9,0,6,37,44,50,53,45,45,44,53,45,29,17,17,2,0,0,0,0,0,6,37,44,50,53,45,45,44,53,45,29,26,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,
42,43,35,48,53,45,45,52,45,27,18,3,0,0,26,29,46,53,46,30,46,49,27,11,0,0,26,29,46,53,46,30,46,49,27,11,0,0,26,29,46,53,46,30,46,49,27,11,0,5,14,30,51,36,37,25,35,53,47,28,21,0,5,14,30,51,36,37,25,35,53,47,28,21,0,5,14,30,51,36,37,25,35,53,47,28,21,0,5,14,30,51,36,37,25,35,53,47,28,21,0,0,0,0,17,39,45,51,36,15,8,9,13,35,53,50,27,24,0,0,4,16,41,50,49,30,49,46,50,38,20,5,0,0,12,14,30,51,45,38,30,31,15,1,0,5,14,30,51,41,43,3,0,0,0,9,11,39,38,53,
45,50,50,30,40,25,7,0,0,23,33,49,49,33,40,40,27,50,48,40,22,0,0,5,14,30,51,41,43,3,0,0,5,14,30,51,41,43,20,31,49,35,13,3,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,0,0,0,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,5,14,30,51,41,37,25,35,53,47,28,21,0,5,14,30,51,41,37,25,35,53,47,28,21,0,5,14,30,51,41,37,25,35,53,47,28,21,0,5,14,30,51,41,37,25,35,53,47,28,21,0,0,19,
37,35,29,14,14,40,45,50,35,25,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,21,28,46,51,30,33,28,40,44,18,9,0,21,28,46,51,30,33,28,40,44,18,9,0,0,21,28,46,51,30,33,28,40,44,18,9,0,21,28,46,51,30,33,28,40,44,18,9,0,26,29,46,51,31,39,40,30,51,45,29,26,0,0,0,0,0,0,0,26,29,46,51,31,39,40,30,51,45,29,26,0,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,53,41,32,28,38,52,36,
15,8,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,22,40,49,46,29,14,28,33,27,11,0,0,22,40,49,46,29,14,28,33,27,11,0,0,22,40,49,46,29,14,28,33,27,11,0,5,14,30,51,41,37,25,35,53,47,28,21,0,5,14,30,51,41,37,25,35,53,47,28,21,0,5,14,30,51,41,37,25,35,53,47,28,21,0,5,14,30,51,41,37,25,35,53,47,28,21,0,0,0,0,22,40,49,52,44,18,9,0,23,33,49,53,44,13,0,0,17,29,47,52,41,14,41,52,
53,44,18,7,0,0,21,28,47,49,33,20,14,39,37,1,0,5,14,30,51,41,43,3,0,0,0,12,14,31,52,31,39,27,47,51,38,14,12,0,0,11,27,50,52,48,48,48,48,51,49,33,22,0,0,5,14,30,51,41,43,3,0,0,5,14,30,51,41,43,13,44,53,38,14,5,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,0,0,0,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,
47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,0,9,11,32,27,44,41,41,46,52,44,18,7,0,9,11,32,27,44,41,41,46,52,44,18,7,0,23,33,49,50,27,13,21,23,16,11,0,0,23,33,49,50,27,13,21,23,16,11,0,0,0,23,33,49,50,27,13,21,23,16,11,0,0,23,33,49,50,27,13,21,23,16,11,0,0,23,33,49,50,27,25,18,44,52,45,29,26,0,0,0,0,0,0,0,23,33,49,50,27,25,18,44,52,45,29,26,0,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,49,27,27,
27,44,52,46,28,12,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,22,39,48,48,36,35,33,28,43,21,0,0,22,39,48,48,36,35,33,28,43,21,0,0,22,39,48,48,36,35,33,28,43,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,
53,47,28,21,0,0,0,0,22,33,49,52,44,13,9,0,22,33,49,52,44,13,0,0,22,33,49,49,33,11,27,50,48,40,23,0,0,0,4,20,36,49,31,35,35,37,21,0,0,5,14,30,51,41,43,3,0,0,0,22,40,48,50,27,25,43,44,53,48,39,22,0,0,11,27,50,50,38,38,38,38,53,49,33,22,0,0,5,14,30,51,41,43,3,0,0,5,14,30,51,41,43,24,27,53,47,28,21,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,0,0,0,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,
49,33,22,11,27,50,49,33,22,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,0,21,32,31,49,49,45,47,49,52,41,18,7,0,21,32,31,49,49,45,47,49,52,41,18,7,0,24,27,50,49,33,22,0,0,8,9,0,0,24,27,50,49,33,22,0,0,8,9,0,0,0,24,27,50,49,33,22,0,0,8,9,0,0,24,27,50,49,33,22,0,0,8,9,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,0,0,0,0,0,11,27,50,
49,33,22,25,35,53,45,29,26,0,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,12,14,36,48,53,53,49,46,27,13,9,0,12,14,36,48,53,53,49,46,27,13,9,0,12,14,36,48,53,53,49,46,27,13,9,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,
30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,0,0,0,10,39,48,51,41,37,19,0,24,27,50,53,35,25,0,0,23,33,50,48,40,23,33,48,30,32,12,0,0,0,42,18,33,36,53,50,36,37,1,0,0,5,14,30,51,41,43,3,0,0,0,24,27,50,49,40,22,11,27,50,49,33,22,0,0,23,33,49,49,33,16,16,27,53,48,39,22,0,0,5,14,30,51,41,43,3,0,0,5,14,30,51,41,43,24,27,53,47,29,21,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,0,0,0,0,
0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,5,14,30,51,36,37,43,44,52,47,28,21,0,5,14,30,51,36,37,43,44,52,47,28,21,0,5,14,30,51,36,37,43,44,52,47,28,21,0,5,14,30,51,36,37,43,44,52,47,28,21,0,0,23,33,49,49,35,29,29,46,52,41,18,7,0,23,33,49,49,35,29,29,46,52,41,18,7,0,11,33,50,50,27,24,8,26,13,10,0,0,11,33,50,50,27,24,8,26,13,10,0,0,0,11,33,50,50,27,24,8,26,13,10,0,0,11,33,
50,50,27,24,8,26,13,10,0,0,23,33,49,50,27,11,13,35,53,45,29,26,0,0,0,0,0,0,0,23,33,49,50,27,11,13,35,53,45,29,26,0,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,8,24,14,33,44,36,46,51,31,20,4,0,8,24,14,33,44,36,
46,51,31,20,4,0,8,24,14,33,44,36,46,51,31,20,4,5,14,30,51,36,37,43,44,52,47,28,21,0,5,14,30,51,36,37,43,44,52,47,28,21,0,5,14,30,51,36,37,43,44,52,47,28,21,0,5,14,30,51,36,37,43,44,52,47,28,21,0,0,0,0,12,34,46,51,30,34,10,21,15,41,52,48,40,22,0,0,22,33,49,49,33,18,44,53,46,32,12,0,0,0,26,29,46,49,41,33,33,37,26,7,0,5,14,30,51,41,43,3,0,0,0,11,27,50,49,33,11,25,27,50,48,40,22,0,0,10,39,45,53,35,13,43,44,52,47,29,21,0,0,5,14,30,51,41,43,3,0,0,
5,14,30,51,41,43,13,35,53,46,32,12,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,0,0,0,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,4,20,38,51,38,28,29,30,51,47,28,21,0,4,20,38,51,38,28,29,30,51,47,28,21,0,4,20,38,51,38,28,29,30,51,47,28,21,0,4,20,38,51,38,28,29,30,51,47,28,21,0,0,24,27,50,48,39,37,33,48,52,41,18,7,0,24,27,50,48,39,37,33,48,52,41,18,7,0,10,29,47,52,31,28,16,
34,35,18,9,0,10,29,47,52,31,28,16,34,35,18,9,0,0,10,29,47,52,31,28,16,34,35,18,9,0,10,29,47,52,31,28,16,34,35,18,9,0,10,39,45,52,41,14,28,38,51,45,29,26,0,0,0,0,0,0,0,10,39,45,52,41,14,28,38,51,45,29,26,0,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,
10,39,45,52,41,14,32,36,51,46,28,21,0,0,17,39,39,20,37,37,44,52,30,14,5,0,17,39,39,20,37,37,44,52,30,14,5,0,17,39,39,20,37,37,44,52,30,14,5,4,20,38,51,38,28,29,30,51,47,28,21,0,4,20,38,51,38,28,29,30,51,47,28,21,0,4,20,38,51,38,28,29,30,51,47,28,21,0,4,20,38,51,38,28,29,30,51,47,28,21,0,0,0,0,1,37,44,53,50,41,29,34,27,45,51,38,14,12,0,0,17,39,45,53,35,14,38,51,45,39,15,21,0,0,22,40,48,45,29,43,37,32,34,21,0,5,14,30,51,36,37,22,3,0,0,10,39,47,52,41,
14,32,31,51,46,28,21,0,0,2,14,38,51,30,29,39,46,51,31,16,4,0,0,5,14,30,51,36,37,22,3,0,4,14,30,51,36,37,32,31,51,31,16,4,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,0,0,0,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,19,37,44,53,50,46,46,31,52,47,28,21,0,19,37,44,53,50,46,46,31,52,47,28,21,0,19,37,44,53,50,46,46,31,52,47,28,21,0,19,37,44,53,50,46,46,31,52,47,28,21,0,0,22,
40,45,52,30,36,46,47,52,41,18,7,0,22,40,45,52,30,36,46,47,52,41,18,7,0,4,15,44,50,53,46,38,46,44,18,9,0,4,15,44,50,53,46,38,46,44,18,9,0,0,4,15,44,50,53,46,38,46,44,18,9,0,4,15,44,50,53,46,38,46,44,18,9,0,4,16,36,53,49,30,30,31,53,45,29,26,0,0,0,0,0,0,0,4,16,36,53,49,30,30,31,53,45,29,26,0,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,48,31,36,31,47,36,
37,19,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,17,39,45,38,36,36,47,52,36,15,8,0,17,39,45,38,36,36,47,52,36,15,8,0,17,39,45,38,36,36,47,52,36,15,8,19,37,44,53,50,46,46,31,52,47,28,21,0,19,37,44,53,50,46,46,31,52,47,28,21,0,19,37,44,53,50,46,46,31,52,47,28,21,0,19,37,44,53,50,46,46,31,52,47,28,21,0,0,0,0,0,22,29,38,53,50,47,46,49,51,46,33,25,9,0,0,5,20,31,51,45,31,49,48,
53,38,33,23,0,0,17,29,47,53,30,41,41,30,29,26,0,4,14,30,51,38,27,34,12,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,7,13,27,48,52,30,46,52,45,33,25,0,0,0,4,14,30,51,38,27,34,12,0,42,15,36,52,47,41,31,50,48,27,25,9,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,0,0,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,22,29,38,48,48,36,35,46,
46,28,21,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,5,16,44,45,48,46,27,46,46,41,18,7,0,5,16,44,45,48,46,27,46,46,41,18,7,0,0,10,14,44,46,48,48,47,44,18,9,0,0,10,14,44,46,48,48,47,44,18,9,0,0,0,10,14,44,46,48,48,47,44,18,9,0,0,10,14,44,46,48,48,47,44,18,9,0,0,23,29,31,48,48,31,35,46,46,29,26,0,0,0,0,0,0,0,0,23,29,31,48,48,31,35,46,46,29,26,0,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,44,46,48,
48,45,30,44,37,19,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,17,39,38,47,48,49,45,31,39,11,0,0,17,39,38,47,48,49,45,31,39,11,0,0,17,39,38,47,48,49,45,31,39,11,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,22,29,38,48,48,36,35,
46,46,28,21,0,0,0,0,0,3,24,29,41,46,48,48,45,31,40,18,4,0,0,0,9,25,40,30,48,48,45,35,31,46,33,23,0,0,8,37,27,30,48,49,48,46,29,26,0,6,15,41,48,50,47,28,12,0,0,0,22,32,44,46,48,48,46,44,14,17,0,0,0,0,12,20,35,46,48,45,30,27,15,2,0,0,0,6,15,41,48,50,47,28,12,0,0,11,39,38,47,48,45,30,35,20,12,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,9,22,
15,29,40,40,28,37,17,0,0,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,0,10,37,28,39,40,29,20,23,0,0,0,0,10,37,28,39,40,29,20,23,0,0,0,0,0,10,37,28,39,40,29,20,23,0,0,0,0,10,37,28,39,40,29,20,23,0,0,0,19,11,14,39,39,14,34,34,34,20,12,0,0,0,0,0,0,0,0,19,11,
14,39,39,14,34,34,34,20,12,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,5,43,14,29,40,40,39,20,24,1,0,0,5,43,14,29,40,40,39,20,24,1,0,0,5,43,14,29,40,40,39,20,24,1,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,1,
24,32,40,39,20,34,34,34,20,12,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,4,25,32,39,39,29,15,20,34,32,10,0,0,0,12,18,32,39,40,39,34,37,2,0,0,22,14,33,33,40,14,12,0,0,0,9,10,37,29,40,40,28,37,17,0,0,0,0,0,0,21,43,28,39,39,32,18,12,0,0,0,0,0,22,14,33,33,40,14,12,0,0,6,25,14,29,39,39,32,43,21,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,0,0,0,
0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,0,
0,6,21,22,22,26,4,0,0,0,0,0,9,12,22,22,12,12,12,12,2,0,0,0,0,0,0,0,0,0,0,9,12,22,22,12,12,12,12,2,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,3,5,26,22,22,17,5,0,0,0,0,0,3,5,26,22,22,
17,5,0,0,0,0,0,3,5,26,22,22,17,5,0,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,3,12,10,10,17,5,5,12,12,9,0,0,0,0,19,12,10,22,10,12,42,0,0,0,0,12,22,23,22,21,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,1,21,10,10,12,19,0,0,0,0,0,0,0,12,22,23,22,21,0,0,
0,0,9,5,26,17,17,12,1,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,19,9,0,
0,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,0,0,0,0,0,0,0,0,19,19,19,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,5,1,0,0,0,0,4,5,4,0,0,0,0,0,0,0,0,0,0,8,5,4,1,0,0,0,0,0,0,0,0,0,0,0,1,4,4,1,0,0,0,0,0,0,0,0,0,1,4,5,4,1,0,0,0,0,0,0,19,19,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,42,4,4,1,0,0,0,0,0,0,0,0,9,6,4,4,6,9,0,0,0,0,0,0,0,0,0,42,4,4,3,0,0,0,0,0,0,0,0,0,0,0,19,4,5,4,3,0,0,0,0,0,0,0,0,0,3,4,5,4,1,0,0,0,0,0,0,0,
0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,0,0,42,4,4,8,3,0,0,0,0,0,0,19,19,19,7,0,0,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,1,42,42,42,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,0,0,0,0,8,17,22,22,21,3,0,0,0,0,0,0,0,0,6,26,22,22,26,1,0,0,0,0,0,0,0,0,0,0,3,
3,19,4,12,26,19,0,0,0,0,0,0,9,3,3,6,12,26,19,0,0,0,0,9,12,10,23,25,13,37,25,3,0,0,0,0,17,16,20,20,24,7,0,0,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,0,21,
24,24,23,24,24,10,0,0,0,0,0,23,37,37,37,11,9,0,0,0,0,0,23,37,37,37,11,9,0,0,0,0,0,0,0,0,42,24,25,11,25,24,8,0,0,0,12,13,20,14,37,22,3,0,17,20,14,20,17,0,0,0,0,0,0,0,5,25,15,14,20,37,25,12,0,0,0,0,0,0,0,42,23,37,14,14,37,23,42,0,0,0,0,0,21,25,37,20,14,20,37,11,5,0,0,0,10,37,37,37,37,37,37,37,37,12,0,0,0,0,0,0,12,25,15,20,20,37,25,12,0,0,0,0,0,21,25,15,14,14,15,25,21,0,0,0,0,0,0,2,24,15,20,20,43,10,3,0,0,0,
0,0,0,0,5,11,37,20,14,20,18,22,8,0,0,0,0,0,5,23,37,20,14,20,37,25,26,3,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,0,0,2,25,15,14,14,16,43,24,12,0,0,0,11,37,37,37,18,4,21,37,37,37,37,26,0,0,0,0,0,4,26,10,10,10,42,0,0,0,0,10,37,14,14,37,15,15,15,37,5,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,12,10,10,10,12,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,0,0,0,
21,37,29,33,40,34,13,4,0,0,0,0,0,0,21,37,29,40,33,29,43,12,0,0,0,0,0,0,5,22,13,43,37,37,20,32,29,22,0,0,0,0,8,22,13,43,37,15,14,29,22,0,0,0,3,11,14,39,33,35,44,41,14,12,0,0,0,0,23,39,38,38,32,17,0,0,0,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,
0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,7,43,41,36,36,15,1,0,0,0,0,7,43,41,36,36,15,1,0,0,0,0,0,0,12,25,15,29,35,28,35,39,17,0,0,4,37,27,31,30,41,29,23,1,43,35,30,39,22,0,0,0,0,0,0,5,37,33,36,30,38,36,35,25,0,0,0,0,0,0,42,13,39,41,30,30,41,39,13,42,0,0,0,7,18,27,36,38,30,38,41,33,37,5,0,0,25,35,36,36,36,36,36,36,33,22,0,0,0,0,0,17,20,27,36,30,38,36,
27,18,9,0,0,0,21,20,27,31,30,30,31,27,20,21,0,0,0,0,12,37,33,36,30,38,44,28,11,7,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,0,0,0,21,37,40,41,38,30,38,36,27,28,22,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,12,15,33,36,30,30,31,41,27,43,7,0,19,37,36,36,36,28,12,11,27,36,36,27,11,0,0,0,9,22,20,29,39,39,39,11,0,0,0,10,14,44,30,30,44,35,31,31,29,26,0,42,37,39,39,39,37,19,0,0,0,0,0,0,42,37,39,39,39,37,19,42,37,39,39,39,37,19,0,0,0,0,
0,0,0,4,37,35,45,27,18,4,0,0,0,0,0,0,0,0,0,4,37,35,45,27,18,4,0,0,0,0,0,12,15,35,47,49,49,30,40,24,9,0,0,0,0,4,37,35,46,49,49,46,27,43,42,0,0,0,0,10,16,40,35,41,36,36,31,46,33,23,0,0,0,12,43,39,35,41,36,31,30,33,23,0,0,0,22,29,31,45,49,53,50,48,40,23,0,0,0,0,6,13,33,47,44,37,5,0,0,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,3,43,44,31,35,
36,31,20,4,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,19,43,41,52,31,15,1,9,0,0,0,19,43,41,52,31,15,1,9,0,0,0,0,10,20,27,36,30,46,36,30,14,12,0,0,22,39,47,31,35,45,36,16,22,29,46,44,37,8,0,0,0,0,0,0,11,40,47,52,45,47,49,35,25,0,0,0,0,0,0,23,39,30,52,49,48,52,30,39,23,0,0,0,7,18,41,
50,48,48,53,52,47,33,11,0,0,25,35,53,52,52,52,52,49,33,22,0,0,0,0,12,16,44,48,53,48,45,49,44,18,9,0,0,3,43,35,49,53,45,47,53,48,35,43,3,0,0,19,18,27,45,53,47,45,53,31,32,17,0,0,0,0,12,16,35,45,52,53,49,50,53,36,37,3,0,0,12,16,35,45,52,53,50,50,53,50,27,11,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,7,13,27,45,51,50,49,50,52,41,43,7,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,0,0,21,14,36,47,48,48,27,24,0,0,8,37,44,50,53,45,45,44,53,45,29,26,
0,8,16,38,48,31,15,1,0,0,0,0,0,0,8,16,38,48,31,15,1,8,16,38,48,31,15,1,0,0,0,0,0,3,42,24,40,47,27,37,2,0,0,0,0,0,0,0,19,42,42,24,40,47,27,37,2,42,0,0,0,0,9,25,27,48,48,36,38,52,31,20,5,0,0,0,0,10,39,47,53,31,31,53,46,29,10,0,0,0,10,14,41,45,50,47,30,30,38,36,33,23,0,0,4,37,27,47,50,46,30,30,36,33,23,0,0,8,15,41,53,30,44,35,33,40,28,23,0,0,0,0,0,4,43,27,45,33,25,9,0,0,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,0,
0,3,22,34,44,31,41,39,24,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,0,0,22,34,44,31,41,39,24,0,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,23,37,37,41,52,31,15,37,24,7,0,23,37,37,41,52,31,15,37,24,7,0,0,21,20,41,48,51,52,50,53,45,29,26,0,0,13,35,49,33,16,31,46,28,37,44,46,29,10,0,0,0,0,0,0,
9,18,44,52,46,39,29,33,35,25,0,0,0,0,0,8,15,41,52,46,33,33,46,52,36,15,8,0,0,7,18,41,27,39,40,44,50,52,44,18,7,0,25,35,53,36,44,44,44,44,33,22,0,0,0,0,24,33,48,50,41,39,39,27,44,18,9,0,0,4,20,30,51,30,29,29,30,51,30,14,5,0,0,21,34,30,52,36,29,33,45,50,35,18,7,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,0,7,13,27,48,51,46,44,33,27,44,30,27,11,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,4,20,38,51,30,27,33,27,36,41,43,7,0,19,37,36,51,47,28,
12,11,27,50,50,27,11,0,0,7,11,27,49,50,31,36,27,24,0,0,26,29,47,51,31,39,40,30,51,45,29,26,0,8,16,38,51,31,15,5,12,4,0,0,0,0,8,16,38,45,31,15,1,8,16,38,51,31,15,1,0,0,0,0,1,13,15,15,39,39,15,15,15,22,0,0,0,0,0,4,43,15,15,37,39,39,15,15,15,15,22,0,0,0,4,20,31,52,41,16,40,48,47,29,21,0,0,0,0,11,27,50,47,29,29,46,50,35,13,0,0,42,37,44,50,49,44,29,32,14,20,37,25,5,0,0,22,39,47,50,41,29,32,20,37,25,5,0,0,17,29,47,30,28,32,32,20,13,21,3,
0,0,0,0,0,12,25,20,39,39,16,24,4,0,0,0,0,0,0,10,13,20,32,32,16,24,4,0,0,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,0,12,25,20,32,32,16,24,4,0,0,0,0,9,17,25,15,14,32,14,37,11,8,0,0,0,0,1,25,15,14,32,32,20,13,12,0,0,0,0,0,10,13,20,32,32,16,25,2,0,0,0,0,0,0,10,13,20,32,32,16,25,2,0,0,0,0,1,24,15,14,32,14,37,13,23,9,0,3,43,41,36,31,51,38,36,36,16,6,3,43,41,36,31,51,38,36,36,16,6,0,3,
13,27,49,53,30,30,33,31,30,29,26,0,9,13,44,48,40,37,36,45,29,39,47,35,18,1,0,0,0,0,0,0,9,13,44,53,47,39,11,24,15,22,12,12,19,0,0,21,28,46,52,41,37,37,41,52,46,28,21,0,0,0,11,20,25,22,22,29,45,52,41,43,7,0,25,35,53,36,20,20,37,13,13,2,0,0,0,8,15,36,51,38,14,37,37,24,16,23,0,0,0,5,14,30,51,36,37,37,36,51,30,14,5,0,0,22,40,48,50,27,24,20,38,51,38,20,4,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,0,2,14,38,51,45,33,43,23,11,18,34,27,11,0,0,12,14,
30,51,47,39,25,11,32,38,51,47,39,10,0,12,32,46,53,35,13,23,25,15,39,18,7,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,7,25,15,35,53,47,29,15,37,26,0,0,23,33,49,50,27,24,18,44,52,45,29,26,0,8,16,38,51,31,15,14,32,20,24,1,0,0,42,37,29,29,29,43,19,8,16,38,51,31,15,1,0,0,0,0,5,14,31,31,41,43,33,31,36,14,21,0,0,0,0,23,39,31,31,29,17,17,43,44,31,41,16,5,0,0,21,29,47,49,33,11,40,48,45,29,26,0,0,0,0,11,33,50,46,28,43,44,52,36,37,19,0,12,34,46,51,36,35,30,
46,38,27,15,12,0,0,9,13,44,53,30,27,38,46,31,40,25,3,0,0,23,33,49,44,36,46,46,38,27,20,26,0,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,12,14,27,36,30,46,30,41,39,18,4,0,0,0,2,32,36,30,46,46,38,27,37,4,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,21,28,36,
30,46,30,44,29,24,3,0,3,43,41,48,48,51,48,48,31,16,6,3,43,41,48,48,51,48,48,31,16,6,0,2,14,30,51,46,39,46,29,47,29,29,26,0,0,24,27,50,27,32,30,30,32,41,30,34,26,26,12,9,0,0,0,0,1,25,39,47,52,31,34,22,18,28,28,28,11,0,0,22,40,48,53,35,25,25,35,53,48,40,22,0,0,0,7,17,14,34,29,35,49,48,27,24,0,0,25,35,53,31,38,38,41,40,43,4,0,0,0,12,32,46,52,44,44,36,41,33,15,12,0,0,0,1,37,44,50,30,40,40,30,50,44,37,1,0,0,22,40,49,50,27,24,20,38,51,46,28,21,0,
0,10,39,45,51,31,16,5,0,0,2,24,26,0,0,10,39,45,51,31,16,21,13,13,13,13,13,22,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,2,14,30,51,30,35,39,32,37,22,5,0,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,1,15,31,31,52,45,31,31,39,10,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,8,16,38,51,31,27,38,46,31,39,11,0,0,8,16,31,31,31,15,1,8,16,38,51,31,15,1,0,0,0,0,5,14,30,51,41,43,20,31,49,35,13,3,0,0,8,15,41,52,36,37,6,9,22,39,45,45,39,23,0,0,10,39,
48,48,40,33,41,53,38,14,12,0,0,0,0,10,29,47,49,44,39,35,53,31,16,8,0,17,39,45,52,41,45,46,48,51,48,27,25,9,0,19,37,36,51,31,48,46,49,51,30,28,26,0,0,11,27,50,49,52,45,47,53,49,44,37,8,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,12,28,47,48,46,30,45,52,46,39,11,0,0,0,2,32,46,46,30,47,52,45,39,10,0,
0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,42,43,35,48,53,47,45,52,45,27,18,3,0,0,21,28,47,45,47,50,53,38,29,22,0,7,18,39,39,41,52,31,39,39,37,1,7,18,39,39,41,52,31,39,39,37,1,0,10,39,45,52,44,27,36,33,38,20,11,1,0,0,26,34,31,47,30,45,35,40,45,27,16,29,29,14,24,1,0,0,0,11,39,30,53,49,50,41,20,16,31,47,35,25,0,0,23,33,49,53,27,24,24,27,53,50,33,23,0,0,0,0,22,40,46,47,49,38,27,15,12,0,0,25,35,53,49,48,53,52,46,33,24,0,0,0,26,29,45,52,46,50,49,52,45,
35,43,1,0,0,0,22,32,44,47,48,48,47,44,32,22,0,0,0,17,29,45,52,41,14,39,47,54,45,29,26,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,0,22,33,49,52,44,18,25,35,35,35,35,35,15,42,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,1,37,44,50,51,50,48,46,41,29,24,7,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,1,15,31,49,51,53,49,48,39,10,0,0,23,33,49,50,27,24,18,44,53,45,29,26,0,8,16,38,51,31,45,45,53,52,36,20,4,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,
5,14,30,51,41,43,13,44,53,38,14,5,0,0,26,29,47,53,35,25,18,18,18,14,30,53,44,43,3,0,22,40,48,53,48,49,53,50,44,20,12,0,0,0,12,43,37,44,48,53,45,48,51,38,20,8,0,22,40,49,54,47,33,34,27,48,51,31,20,5,0,42,15,31,51,48,27,34,35,50,50,27,25,0,0,11,27,50,51,30,39,29,36,52,47,29,10,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,26,29,46,53,41,32,28,
38,52,36,15,8,0,0,12,28,36,40,32,14,39,30,52,36,15,8,0,0,2,32,40,32,14,39,47,49,33,22,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,21,28,35,29,29,35,49,53,44,43,3,0,5,10,43,41,52,31,15,10,12,0,0,5,10,43,41,52,31,15,10,12,0,0,22,33,49,50,27,36,35,44,44,43,3,0,0,0,7,11,28,27,35,40,20,36,31,14,41,47,45,38,39,11,0,0,4,20,36,52,38,44,49,49,35,32,30,49,33,23,0,0,23,33,50,53,27,24,24,27,50,50,33,23,0,0,0,0,22,
40,46,46,48,50,30,39,11,0,0,25,35,44,33,40,44,48,51,31,16,4,0,0,26,29,45,54,48,35,33,38,51,46,28,21,0,0,19,18,27,46,49,38,38,49,46,27,18,3,0,0,4,16,41,53,49,30,46,50,51,45,29,26,0,0,22,33,49,52,44,13,9,0,0,0,0,0,0,0,22,33,49,52,44,13,25,35,53,53,53,31,15,42,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,10,14,44,46,48,53,51,53,31,14,12,0,19,37,36,51,47,28,12,11,27,50,50,27,11,0,1,37,40,35,53,47,40,40,29,10,0,0,26,29,47,51,31,29,39,30,51,45,29,26,
0,8,16,38,51,49,35,39,36,52,47,28,21,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,5,14,30,51,41,43,24,27,53,47,28,21,0,0,23,33,49,49,33,28,44,44,44,16,31,51,38,20,4,0,10,39,48,49,31,31,38,49,49,27,25,0,0,0,22,33,41,41,33,41,31,31,52,38,16,8,0,22,33,49,51,41,37,12,14,30,51,47,29,21,0,8,16,38,51,38,14,26,29,47,52,41,43,7,0,22,40,49,53,44,18,11,33,49,50,27,11,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,9,
11,32,27,44,41,41,46,52,44,18,7,0,9,11,32,27,44,41,41,46,52,44,18,7,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,11,39,39,39,39,39,39,44,52,46,28,12,0,0,7,23,32,35,35,41,49,38,28,26,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,33,33,35,53,48,39,22,0,0,5,37,28,39,39,39,30,51,38,20,4,0,0,7,43,41,52,31,15,1,0,0,0,21,23,43,41,52,31,15,23,26,0,0,22,33,49,50,27,30,40,38,33,11,0,0,0,0,0,7,26,24,25,25,33,48,33,27,49,35,33,45,36,15,8,0,21,
29,47,50,27,14,36,53,45,27,49,46,28,21,0,0,22,33,49,53,35,25,25,35,53,49,33,22,0,0,0,3,17,14,32,28,27,45,52,36,15,42,0,23,14,18,23,22,20,31,51,46,32,12,0,0,21,29,47,51,30,14,25,35,53,48,40,22,0,0,12,32,30,52,41,14,14,41,52,30,32,12,0,0,0,22,28,41,46,47,30,35,53,46,28,12,0,0,22,39,48,51,41,37,1,0,0,19,21,4,0,0,22,39,48,51,41,37,25,35,44,38,51,31,15,42,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,21,17,37,28,33,44,30,51,48,40,22,0,19,37,36,51,47,28,
21,11,27,50,50,27,11,0,0,12,25,35,53,47,28,22,22,19,0,0,8,37,44,50,53,45,45,44,53,45,29,26,0,8,16,38,51,30,14,25,35,53,45,29,26,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,5,14,30,51,41,43,24,27,53,47,29,21,0,0,24,27,50,49,40,28,47,52,44,15,31,51,30,14,5,0,17,29,45,48,40,15,14,31,51,36,37,19,0,0,22,40,49,48,39,43,16,44,52,36,15,1,0,23,33,49,52,44,13,8,16,31,51,45,29,26,0,8,20,38,51,31,15,12,32,46,51,36,37,3,0,22,40,49,53,27,24,22,39,48,53,35,
25,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,21,32,31,49,49,45,47,49,52,41,18,7,0,21,32,31,49,49,45,47,49,52,41,18,7,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,48,48,48,48,48,48,51,45,29,21,0,0,0,12,32,46,49,53,38,27,16,12,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,52,48,48,48,48,51,49,33,22,0,0,0,26,29,45,48,48,49,54,30,14,5,0,0,7,43,41,52,31,15,1,0,0,3,43,33,33,41,52,31,33,33,15,6,0,22,
39,48,53,35,30,29,47,29,21,12,0,0,0,0,0,0,0,21,14,31,36,20,41,45,39,20,38,46,32,12,0,17,39,45,50,27,24,28,38,52,49,50,44,37,1,0,0,17,29,45,52,44,18,18,44,52,45,29,17,0,0,4,18,13,26,2,12,20,30,51,30,14,4,8,37,20,23,12,5,15,36,51,46,28,12,0,0,2,14,30,51,38,20,24,27,50,49,40,22,0,0,17,39,45,53,35,25,25,35,53,45,29,26,0,0,0,21,25,15,28,29,14,36,51,38,20,4,0,0,12,32,46,51,46,29,23,12,26,13,28,13,7,0,12,32,46,51,46,29,11,13,20,38,51,31,15,42,0,21,28,
46,51,30,32,10,21,16,36,52,48,40,22,0,4,20,28,18,22,23,18,33,50,50,33,23,0,3,43,41,52,45,39,10,25,35,53,49,33,23,0,0,0,25,35,53,47,28,21,0,0,0,0,0,10,14,44,46,46,44,35,53,45,29,26,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,5,14,30,51,41,43,13,35,53,46,32,12,0,0,11,27,50,49,33,29,45,52,41,16,31,51,30,14,5,0,12,34,46,50,27,25,15,36,51,36,37,19,0,0,17,29,45,49,27,24,20,38,52,44,18,7,0,22,40,48,52,41,43,5,
20,38,51,47,28,21,0,42,15,31,51,38,20,21,28,46,52,41,43,7,0,10,39,48,53,35,13,23,33,49,50,27,24,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,23,33,49,49,35,29,29,46,52,41,18,7,0,23,33,49,49,35,29,29,46,52,41,18,7,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,48,41,41,41,41,52,47,28,21,0,0,4,11,14,33,33,35,45,48,33,11,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,11,27,50,49,33,39,39,35,53,48,40,22,0,0,42,25,29,
27,27,27,38,51,30,14,5,0,0,7,43,41,52,31,15,1,0,0,3,37,36,50,50,54,50,50,31,16,6,0,21,28,46,51,46,36,27,31,16,20,14,21,0,0,0,0,0,3,13,27,45,40,37,41,45,39,20,38,46,32,12,0,12,34,46,52,31,29,20,39,47,54,30,28,22,0,0,0,5,20,31,51,30,28,28,38,51,31,20,5,0,0,21,28,27,29,14,14,33,47,51,31,20,4,5,14,36,40,14,14,33,47,51,38,14,5,0,0,1,37,41,52,46,28,15,44,52,47,29,26,0,0,21,29,47,52,41,20,16,41,52,47,29,21,0,0,9,18,27,28,16,20,27,45,50,35,13,7,0,
0,1,37,44,50,53,36,39,32,29,27,36,37,3,0,1,37,44,50,53,31,39,32,28,38,51,31,15,42,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,5,14,30,35,39,32,28,44,53,48,40,22,0,0,25,27,50,50,44,28,29,38,51,47,29,17,0,0,0,25,35,53,47,28,21,0,0,0,0,0,11,40,20,32,32,20,36,52,46,32,12,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,4,14,30,51,36,37,32,31,51,31,16,4,0,0,22,40,48,53,44,33,49,47,38,29,46,51,36,15,6,0,8,16,
36,51,38,29,29,46,53,44,13,9,0,0,5,20,38,52,36,32,40,45,49,33,11,0,0,21,34,30,51,30,29,15,40,45,51,38,20,5,0,7,18,44,53,47,39,15,27,49,50,27,25,0,0,12,32,30,51,31,34,14,41,53,45,39,22,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,24,27,50,48,39,37,33,48,52,41,18,7,0,24,27,50,48,39,37,33,48,52,41,18,7,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,45,50,27,37,14,31,51,31,20,4,0,0,23,33,29,15,37,32,30,53,35,25,0,
0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,21,28,40,20,20,27,48,52,41,37,1,0,0,7,43,41,52,31,15,1,0,0,3,37,44,44,41,52,31,44,44,16,6,0,1,37,44,53,53,44,41,35,39,41,29,26,0,0,0,0,0,26,34,30,41,15,25,35,50,35,40,47,36,16,4,0,1,37,44,49,53,46,38,47,53,48,53,36,14,17,0,0,9,25,33,45,53,30,30,53,45,27,25,9,0,0,21,28,47,47,30,30,45,51,48,35,13,7,5,14,30,48,30,30,45,51,48,27,13,7,0,0,0,23,39,46,53,38,36,45,50,
41,15,4,0,0,5,20,36,52,48,31,31,48,52,36,20,4,0,0,9,18,44,46,31,38,48,53,31,32,26,0,0,0,0,10,34,31,50,52,45,46,47,49,36,37,3,0,0,10,34,31,50,52,45,46,47,49,52,31,15,42,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,5,14,30,53,45,46,46,50,52,38,14,12,0,0,17,28,30,52,50,47,47,53,50,41,15,4,0,0,0,25,35,53,47,28,21,0,0,0,0,0,24,27,31,35,27,41,48,50,44,37,1,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,
42,15,36,52,47,41,31,50,48,27,25,9,0,0,12,32,30,51,49,47,45,41,50,47,52,50,35,13,9,0,0,25,33,45,53,30,30,53,45,40,22,0,0,0,9,25,27,45,50,30,46,52,38,14,12,0,0,1,43,35,48,53,30,31,47,51,48,27,13,7,0,0,22,29,30,52,46,31,47,51,30,29,17,0,0,1,43,35,49,50,30,38,49,53,41,20,5,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,22,40,45,52,30,36,46,47,52,41,18,7,0,22,40,45,52,30,36,46,47,52,41,18,7,0,4,15,41,50,48,31,36,
31,47,36,37,19,0,0,4,15,41,50,47,36,31,50,45,27,25,9,0,0,23,33,47,31,36,38,50,48,33,11,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,21,28,47,38,38,45,51,46,39,23,0,0,0,7,43,41,52,31,15,1,0,0,0,22,18,43,41,52,31,15,18,23,9,0,0,22,29,38,53,50,48,46,45,45,29,26,0,0,0,0,19,18,35,47,39,23,26,32,36,48,48,30,39,24,0,0,0,17,14,44,46,48,48,46,41,35,46,46,44,16,5,0,0,12,15,27,46,48,48,46,35,15,12,0,0,0,21,28,38,
47,48,49,45,30,35,20,21,0,5,14,41,30,45,49,48,30,35,16,21,0,0,0,0,8,43,33,30,48,48,47,41,32,22,0,0,0,0,11,29,36,47,48,48,47,36,29,11,0,0,0,9,18,44,47,48,48,47,41,29,11,9,0,0,0,0,7,11,28,44,46,48,49,45,30,35,43,3,0,0,7,11,28,44,46,48,49,48,46,36,33,37,1,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,5,14,44,30,45,48,48,45,31,39,25,7,0,0,3,25,39,31,45,48,48,46,41,32,22,0,0,0,0,25,35,53,47,28,21,0,0,0,0,0,24,27,48,50,53,50,45,41,32,10,0,
0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,0,11,39,38,47,48,45,30,35,20,12,0,0,0,3,13,33,46,48,45,41,40,30,48,45,36,32,26,0,0,0,2,15,27,30,45,48,46,35,15,5,0,0,0,0,12,15,35,46,48,45,31,39,24,7,0,0,0,21,16,27,30,45,48,48,46,35,20,12,0,0,0,1,13,40,31,45,48,48,30,33,13,1,0,0,0,26,20,35,46,48,48,47,41,28,22,0,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,5,
16,44,45,48,46,27,46,46,41,18,7,0,5,16,44,45,48,46,27,46,46,41,18,7,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,0,22,32,41,47,48,48,30,27,15,12,0,0,0,23,33,46,48,49,48,46,44,20,12,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,21,28,30,48,49,45,38,33,43,4,0,0,0,7,43,41,52,31,15,1,0,0,0,0,7,43,41,52,31,15,1,0,0,0,0,3,24,33,30,46,49,48,47,36,29,26,0,0,0,0,4,16,40,40,37,4,9,23,14,39,40,34,13,8,0,0,0,
0,17,37,28,40,40,28,37,20,34,34,34,15,4,0,0,0,12,43,28,40,40,28,43,12,0,0,0,0,8,13,20,29,39,40,39,32,18,21,0,0,7,23,37,32,39,40,39,32,43,21,0,0,0,0,0,0,5,13,32,39,40,29,15,22,9,0,0,0,0,19,11,20,29,40,40,29,20,11,19,0,0,0,0,23,20,29,40,40,29,16,22,3,0,0,0,0,0,0,7,10,37,34,39,40,39,14,18,21,0,0,0,0,7,10,37,34,39,40,39,28,15,24,2,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,9,22,18,14,29,40,40,29,20,24,6,0,0,0,0,8,24,20,29,
40,40,29,15,22,9,0,0,0,0,25,35,46,46,28,21,0,0,0,0,0,22,32,40,27,35,27,39,16,22,9,0,0,8,16,38,46,31,15,24,27,46,46,29,26,0,8,16,38,46,31,15,1,8,16,38,46,31,15,1,0,0,0,0,0,6,25,14,29,39,39,32,43,21,0,0,0,0,0,5,18,28,40,39,20,13,28,40,39,20,22,9,0,0,0,0,12,18,32,39,39,28,37,21,0,0,0,0,0,0,12,43,28,39,39,14,24,6,0,0,0,0,0,21,18,14,39,40,39,28,37,21,0,0,0,0,0,4,25,14,39,40,39,32,13,5,0,0,0,0,0,26,37,28,39,40,29,15,22,7,
0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,22,15,29,40,39,32,18,12,0,0,0,0,26,15,28,39,40,39,28,37,17,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,4,43,32,39,40,39,14,25,5,0,0,0,0,7,43,44,44,44,15,1,0,0,0,0,7,43,44,44,44,15,1,0,0,0,0,
9,18,44,41,35,36,40,29,15,23,1,0,0,0,0,0,21,22,22,12,0,0,0,2,10,22,21,3,0,0,0,0,0,0,6,21,22,22,21,8,2,12,12,12,5,0,0,0,0,0,1,21,22,22,21,1,0,0,0,0,0,0,7,5,21,22,22,10,12,19,0,0,0,0,0,1,12,10,22,10,12,19,0,0,0,0,0,0,0,0,3,12,22,22,26,4,0,0,0,0,0,0,0,0,4,26,22,22,26,4,0,0,0,0,0,0,0,4,21,22,22,26,4,0,0,0,0,0,0,0,0,0,0,1,12,10,22,10,2,3,0,0,0,0,0,0,0,1,12,10,22,10,21,8,0,0,0,0,0,0,
0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,3,5,26,22,22,17,5,9,0,0,0,0,0,0,0,5,17,22,22,21,8,0,0,0,0,0,0,11,34,34,34,20,12,0,0,0,0,0,7,12,22,24,25,24,22,5,0,0,0,0,1,43,34,34,34,18,23,34,34,34,14,12,0,1,43,34,34,34,18,3,1,43,34,34,34,18,3,0,0,0,0,0,0,9,5,26,17,17,12,1,0,0,0,0,0,0,0,1,21,22,10,5,19,21,22,10,2,0,0,0,0,0,0,0,19,12,10,10,21,42,0,0,0,0,0,0,0,0,1,21,10,17,2,9,0,0,0,0,0,0,0,3,12,10,22,
22,21,1,0,0,0,0,0,0,0,9,2,17,22,10,12,3,0,0,0,0,0,0,0,1,21,22,22,26,4,0,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,4,17,22,10,12,19,0,0,0,0,0,0,4,21,10,22,10,21,6,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,19,12,
10,22,10,2,7,0,0,0,0,0,0,10,13,13,13,22,0,0,0,0,0,0,10,13,13,13,22,0,0,0,0,0,0,22,37,37,37,37,11,21,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,4,12,12,12,8,0,0,4,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,42,42,42,3,5,12,4,0,0,0,0,0,0,0,0,1,2,2,1,42,42,42,1,0,0,0,3,42,42,42,7,0,0,42,42,42,1,0,0,0,7,42,42,42,7,0,42,42,42,1,0,0,0,0,0,0,7,42,42,1,0,0,0,0,5,5,4,0,0,0,0,0,0,0,7,42,42,1,0,0,0,0,5,5,4,0,0,0,0,0,0,0,7,8,4,4,1,0,0,0,0,5,5,4,0,0,0,0,0,0,0,9,42,42,42,19,0,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,8,
8,8,9,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,2,12,2,6,0,0,0,0,0,0,0,0,9,4,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,7,42,42,42,3,5,12,4,0,
0,0,0,0,0,42,8,8,0,1,8,8,0,0,0,0,0,7,42,42,42,7,5,12,4,0,0,0,0,0,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,8,8,8,9,0,0,7,42,42,42,3,5,12,4,0,0,0,0,42,8,8,0,1,8,8,0,0,0,0,0,0,4,12,12,5,42,0,0,0,0,0,0,0,0,0,42,8,8,1,0,0,3,42,42,42,9,0,42,42,42,1,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,42,8,8,0,1,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,10,10,10,12,0,0,0,0,0,0,0,3,25,15,15,15,18,14,32,16,11,1,0,0,0,0,0,10,37,14,14,37,15,15,15,37,5,0,19,13,15,15,15,24,9,10,15,15,15,37,2,0,3,25,15,15,15,25,10,15,15,15,37,2,0,0,0,1,23,18,15,15,43,5,0,0,10,20,14,20,17,0,0,0,0,1,23,18,15,15,43,5,0,0,10,20,14,20,17,0,0,0,0,0,5,18,16,14,20,37,22,9,0,10,20,14,20,17,0,0,0,0,0,0,23,15,
15,15,18,8,0,0,0,0,0,12,25,16,32,32,20,18,17,0,0,17,16,20,20,24,7,0,0,0,0,0,22,20,20,20,23,0,0,0,0,21,15,20,20,22,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,19,22,25,22,23,25,11,19,0,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,0,12,25,20,32,32,16,24,4,0,0,0,0,0,2,10,10,10,12,0,0,0,0,0,0,0,0,19,22,25,22,23,25,11,19,0,0,5,10,10,10,10,10,12,0,9,23,18,13,2,25,18,24,19,0,12,10,10,10,12,8,8,1,0,0,12,10,10,10,12,0,0,0,0,0,0,
0,5,10,10,10,21,0,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,3,25,15,15,15,13,14,32,20,24,1,0,0,0,5,37,20,15,21,37,20,15,21,0,0,0,3,25,15,15,15,13,14,32,20,24,1,0,0,0,0,0,0,3,21,24,18,37,37,37,37,37,37,37,37,37,37,22,0,0,0,0,0,0,22,20,20,20,23,0,3,25,15,15,15,13,14,32,15,2,0,0,5,37,20,15,21,37,20,15,21,0,0,0,5,25,16,32,32,14,15,13,12,0,0,0,0,0,0,2,37,20,20,18,6,1,13,15,15,15,11,22,15,15,15,43,4,0,0,0,0,0,0,0,0,21,15,20,
20,22,0,0,0,0,0,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,21,10,10,10,5,0,0,0,0,0,5,37,20,15,21,37,20,15,21,0,0,0,0,0,4,26,10,10,10,42,0,0,12,10,10,10,12,0,0,0,42,37,39,39,39,37,19,0,0,0,0,0,0,8,16,31,31,31,27,30,46,36,39,24,7,0,0,0,10,14,44,30,30,44,35,31,31,29,26,0,4,16,36,31,31,20,5,13,35,31,31,29,26,0,6,16,31,31,31,15,24,27,31,31,39,17,0,0,0,17,29,44,31,31,29,21,0,42,43,44,30,39,22,0,0,0,0,17,29,44,31,31,29,21,0,42,
43,44,30,39,22,0,0,0,0,0,10,39,31,30,30,44,32,26,42,43,44,30,39,22,0,0,0,0,0,7,18,41,31,31,32,12,0,0,0,0,12,15,33,31,46,46,38,44,18,9,0,23,39,38,38,32,17,0,0,0,0,12,20,41,38,27,13,0,0,0,42,43,35,38,41,16,5,0,0,7,43,27,27,28,27,27,20,8,0,0,0,23,29,35,39,14,35,28,21,0,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,10,37,39,39,39,15,11,11,4,0,0,0,0,0,23,29,35,39,14,35,28,21,0,7,18,39,39,39,39,39,15,
6,6,16,44,33,25,29,44,32,12,42,37,39,39,39,37,20,20,18,6,42,37,39,39,39,37,19,0,0,0,0,0,9,13,39,39,39,20,5,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,8,16,31,31,31,27,38,46,31,39,11,0,0,0,12,32,38,35,37,33,38,39,22,0,0,0,8,16,31,31,31,27,38,46,31,39,11,0,0,0,0,0,2,13,28,27,44,36,36,36,36,36,36,36,36,36,41,18,7,0,0,0,0,12,20,41,38,27,13,0,8,16,31,31,31,27,30,46,29,21,0,0,12,32,38,35,37,33,38,39,22,0,0,1,18,33,31,46,46,30,36,27,11,0,0,0,
2,22,22,22,39,38,31,20,5,5,14,31,31,36,37,25,35,31,31,28,21,0,0,0,0,0,0,0,42,43,35,38,41,16,5,0,0,0,0,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,0,4,20,39,39,39,18,7,0,0,0,0,12,32,38,35,37,33,38,39,22,0,0,0,9,22,20,29,39,39,39,11,0,42,37,39,39,39,37,19,0,0,8,16,38,48,31,15,1,0,0,0,0,0,0,8,16,38,51,31,45,47,50,52,38,32,21,0,0,6,37,44,50,53,45,45,44,53,45,29,26,0,0,25,27,49,45,39,22,20,31,51,36,15,8,0,6,16,31,51,31,15,24,27,53,45,
39,17,0,0,0,17,39,36,44,47,29,21,0,22,29,46,44,37,6,0,0,0,0,17,39,36,44,47,29,21,0,22,29,46,44,37,6,0,0,0,0,0,10,39,35,33,44,50,27,11,22,29,46,44,37,6,0,0,0,0,0,7,18,41,52,46,32,12,0,0,0,1,43,35,45,51,48,47,48,44,18,9,0,6,13,33,47,44,37,5,0,0,1,13,27,48,35,37,2,0,0,0,22,29,46,44,45,33,25,7,0,7,43,41,47,28,30,38,20,8,0,0,1,37,36,31,46,35,46,32,12,0,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,
4,20,27,36,48,38,27,27,33,11,0,0,0,0,1,37,36,31,46,35,46,32,12,0,7,43,41,48,48,48,38,20,8,3,43,44,31,35,36,31,20,4,8,16,38,48,31,39,38,31,20,5,8,16,38,48,31,15,1,0,0,0,0,0,7,18,41,48,46,32,2,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,8,16,38,51,31,45,45,53,52,36,20,4,0,0,7,25,33,45,33,30,35,43,42,0,0,0,8,16,38,51,31,45,45,53,52,36,20,4,0,0,0,5,37,27,30,49,52,51,52,51,51,51,52,52,52,52,41,18,7,0,0,0,1,13,27,48,35,37,2,0,8,16,38,51,
31,47,50,47,29,21,0,0,7,25,33,45,33,30,35,43,42,0,0,26,29,46,53,46,30,46,49,27,11,0,0,9,13,40,40,40,27,50,27,25,9,5,14,30,51,36,37,25,35,53,47,28,21,0,0,0,0,0,0,0,22,29,46,44,45,33,25,7,0,0,0,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,5,14,30,48,41,43,3,0,0,0,0,7,25,33,45,33,30,35,43,42,0,0,0,21,14,36,47,48,48,27,24,0,8,16,38,48,31,15,1,0,0,8,16,38,51,31,15,1,42,42,42,42,0,0,8,16,38,51,48,27,29,44,50,49,27,24,0,0,26,29,46,51,31,
39,40,30,51,45,29,26,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,6,16,31,51,31,15,24,27,53,45,39,17,0,0,0,12,16,15,44,47,29,21,4,37,44,30,29,10,0,0,0,0,0,12,16,15,44,47,29,21,4,37,44,30,29,10,0,0,0,0,0,0,2,43,39,33,44,47,40,23,37,44,30,29,10,0,0,0,0,0,0,7,18,41,47,46,32,12,0,0,0,21,28,46,51,30,33,28,40,44,18,9,0,0,4,43,27,45,33,25,9,0,22,29,30,44,16,12,0,0,0,2,16,41,36,14,44,38,32,26,0,7,43,44,44,28,44,44,20,8,0,7,4,20,31,39,33,31,
35,37,1,0,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,4,20,30,48,54,49,48,48,27,11,9,0,0,0,4,20,31,39,33,31,35,37,1,0,7,43,33,33,33,33,33,16,8,0,22,34,44,31,41,39,24,0,8,16,38,51,31,27,50,27,25,9,8,16,38,51,31,15,1,0,0,0,0,0,7,18,41,52,46,32,2,0,0,0,7,42,22,29,30,44,16,12,0,0,0,0,8,16,38,51,49,35,39,36,52,47,28,21,0,0,7,2,16,41,47,38,32,26,0,0,0,0,8,16,38,51,49,35,39,36,52,47,28,21,0,0,9,25,
33,45,51,50,30,36,41,31,51,47,41,41,41,41,41,18,7,0,7,42,22,29,30,44,16,12,0,0,8,16,38,51,50,36,33,27,29,21,0,0,7,2,16,41,47,38,32,26,0,0,0,22,40,49,46,29,14,28,33,27,11,0,0,3,18,44,49,46,44,46,28,26,0,5,14,30,51,41,37,25,35,53,47,28,21,0,0,0,0,42,42,2,16,41,36,14,44,38,32,26,42,42,3,0,0,0,1,42,22,29,30,44,16,12,7,0,0,0,1,5,14,30,45,41,43,42,7,0,0,0,1,2,16,41,47,38,32,26,7,0,0,7,11,27,49,50,31,36,27,24,0,8,16,38,45,31,15,1,0,0,
8,16,38,51,31,15,10,15,15,15,15,23,0,8,16,38,51,38,14,17,29,47,52,41,43,7,0,23,33,49,50,27,25,18,44,52,45,29,26,0,0,1,37,44,53,38,14,27,50,46,28,21,0,0,6,16,31,51,31,15,24,27,53,45,39,17,0,0,0,0,4,18,44,47,29,21,23,39,47,35,18,42,42,7,0,0,0,0,4,18,44,47,29,21,23,39,47,35,18,4,4,1,0,0,0,0,0,22,33,38,47,41,28,23,39,47,35,18,42,42,7,0,0,0,0,9,25,28,28,28,16,5,0,0,0,23,33,49,50,27,13,21,23,16,11,0,0,0,3,25,15,39,39,18,7,3,25,39,39,20,
25,7,0,0,0,12,14,39,28,15,14,39,29,10,0,0,10,13,15,15,15,13,23,9,3,25,15,15,15,37,14,32,20,24,1,0,0,0,9,11,32,27,44,41,41,46,52,44,18,7,0,23,33,49,49,27,27,27,44,52,46,28,12,0,8,15,39,36,51,38,39,39,39,20,25,8,0,0,19,13,15,15,15,16,43,21,0,0,0,12,25,15,15,15,25,26,0,0,3,25,15,15,15,25,1,0,8,16,38,51,31,44,46,28,26,0,8,16,38,51,31,15,21,21,12,0,0,0,7,18,41,52,46,32,37,4,0,3,25,15,15,39,39,14,32,20,24,1,0,0,8,16,38,51,30,14,25,35,53,45,
29,26,0,3,25,15,15,29,39,39,32,20,24,1,0,0,8,16,38,51,30,14,25,35,53,45,29,26,0,0,5,20,31,51,49,35,32,37,37,36,51,46,34,43,43,43,43,22,0,3,25,15,15,39,39,14,32,15,2,0,8,16,38,51,46,28,11,25,13,8,0,3,25,15,15,29,39,39,32,15,2,0,0,22,39,48,48,36,35,33,28,43,21,0,3,13,15,44,52,46,39,39,15,23,0,5,14,30,51,41,43,25,35,53,47,28,21,0,0,0,22,15,15,15,14,39,28,15,14,39,29,15,15,15,25,3,0,12,37,15,15,39,39,20,15,15,24,9,0,12,37,15,16,29,29,29,15,15,24,9,
0,12,37,15,15,29,39,39,15,15,24,9,7,25,15,35,53,47,29,37,37,26,0,42,37,29,29,29,43,19,0,0,8,16,38,51,31,15,20,41,31,31,27,13,0,8,16,38,51,31,15,12,32,46,51,36,37,3,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,0,23,40,45,48,40,36,52,41,37,42,0,0,6,16,31,51,31,15,24,27,53,45,39,17,0,0,0,19,11,18,44,47,29,25,16,36,30,32,43,15,15,25,7,0,0,19,11,18,44,47,29,25,16,36,30,32,37,14,14,37,22,9,0,0,10,20,15,20,40,48,35,16,36,30,32,43,15,15,25,7,0,0,0,3,18,41,
38,38,32,12,0,0,0,24,27,50,49,33,22,0,0,8,9,0,0,0,8,16,31,31,31,15,1,8,16,31,31,31,15,1,0,0,0,0,21,16,31,31,31,15,26,3,0,0,8,16,31,31,31,15,1,0,8,16,31,31,31,27,38,46,31,39,11,0,0,0,21,32,31,49,49,45,47,49,52,41,18,7,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,12,37,36,51,38,33,38,46,31,39,25,9,0,0,8,16,31,31,31,15,1,0,0,0,8,16,31,31,31,15,1,0,0,8,16,31,31,31,15,1,0,8,16,38,51,31,39,39,37,1,0,8,16,38,51,31,29,29,29,20,2,0,0,
7,18,41,52,46,41,40,11,0,8,16,31,31,31,27,38,46,31,39,11,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,31,31,31,27,38,46,31,39,11,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,17,39,45,51,31,14,26,1,37,36,51,46,40,40,40,40,39,23,0,8,16,31,31,31,27,30,46,29,21,0,8,16,38,51,31,15,6,9,9,0,0,8,16,31,31,31,27,30,46,29,21,0,0,12,14,36,48,53,53,49,46,27,13,9,4,20,31,31,52,47,31,31,44,18,9,5,14,30,51,41,43,25,35,53,47,28,21,0,0,0,25,35,31,31,28,13,35,31,
31,16,15,36,31,31,16,8,0,10,39,31,31,31,31,31,31,36,37,19,0,10,39,31,31,31,31,31,31,36,37,19,0,10,39,31,31,31,31,31,31,36,37,19,1,15,31,31,52,47,28,21,3,0,0,8,16,31,31,31,15,1,0,0,8,16,38,51,31,16,44,49,45,27,37,12,0,8,16,38,51,38,20,21,28,46,52,41,43,3,0,23,33,49,50,27,11,13,35,53,45,29,26,0,0,0,2,20,31,53,44,46,49,33,24,0,0,0,6,16,31,51,31,16,24,27,53,45,39,17,0,0,0,12,28,35,44,47,35,35,33,45,27,13,40,31,31,15,42,0,0,12,28,35,44,47,35,35,33,45,
27,39,36,30,30,44,32,26,0,0,11,27,27,33,44,50,35,33,45,27,13,40,31,31,15,42,0,0,3,11,29,30,51,38,20,5,0,0,0,11,33,50,50,27,24,8,26,13,10,0,0,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,45,45,53,52,36,20,4,0,0,23,33,49,49,35,29,29,46,52,41,18,7,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,1,37,36,51,38,47,45,50,52,38,14,2,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,
0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,17,2,0,0,8,16,38,51,31,44,45,46,34,12,0,0,8,18,41,52,50,46,27,43,3,8,16,38,51,31,45,45,53,52,36,20,4,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,45,45,53,52,36,20,4,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,22,33,49,52,44,18,7,1,37,36,51,50,49,49,49,49,33,23,0,8,16,38,51,31,47,50,47,29,21,0,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,31,47,50,47,29,21,0,0,8,24,14,33,44,36,46,51,31,20,4,4,20,
38,49,51,50,49,49,44,18,9,5,14,30,51,36,37,43,44,52,47,28,21,0,0,0,22,40,48,48,40,37,36,51,46,34,32,30,53,44,18,7,0,10,39,48,49,49,49,52,51,36,37,19,0,10,39,48,49,49,49,52,51,36,37,19,0,10,39,48,49,49,49,52,51,36,37,19,1,15,31,49,51,47,28,21,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,35,48,45,27,37,12,0,0,8,16,38,51,47,40,20,27,49,50,35,25,0,0,10,39,45,52,41,14,28,38,51,45,29,26,0,0,0,9,13,35,50,46,49,46,34,21,0,0,0,6,16,31,51,46,29,14,41,52,48,
39,14,21,0,0,12,28,31,31,31,31,35,31,31,14,32,31,46,31,15,42,0,0,12,28,31,31,31,31,35,31,31,14,39,41,33,44,50,27,25,0,0,11,27,31,30,30,44,32,31,31,14,32,31,46,31,15,42,0,0,22,28,31,53,46,33,25,9,0,0,0,10,29,47,52,31,28,16,34,35,18,9,0,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,8,16,38,51,49,35,39,36,52,47,28,21,0,0,24,27,50,48,39,37,33,48,52,41,18,7,0,10,39,47,53,44,20,37,20,39,44,37,19,0,
0,1,37,36,51,50,44,39,41,52,45,39,17,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,44,52,46,34,12,0,2,43,40,38,51,46,40,43,12,0,8,16,38,51,49,35,39,36,52,47,28,21,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,49,35,39,36,52,47,28,21,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,22,33,49,52,44,13,9,1,37,36,51,49,47,47,47,47,33,23,0,8,16,38,51,50,36,33,27,29,21,0,8,16,38,51,
31,15,1,0,0,0,0,8,16,38,51,50,36,33,27,29,21,0,0,17,39,39,20,37,37,44,52,30,14,5,8,16,40,44,52,46,40,40,40,13,9,4,20,38,51,38,28,29,30,51,47,28,21,0,0,0,12,32,46,53,35,14,30,47,48,40,39,45,49,33,23,0,0,10,29,40,40,40,35,49,48,35,37,19,0,10,29,40,40,40,35,49,48,35,37,19,0,10,29,40,40,40,35,49,48,35,37,19,1,37,40,35,53,47,28,21,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,45,48,35,15,12,0,0,0,8,16,38,51,30,47,38,45,51,46,29,10,0,0,4,16,36,53,49,
30,30,31,53,45,29,26,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,6,16,31,51,52,46,30,49,45,53,30,39,17,0,0,4,43,16,16,16,16,27,48,33,37,35,41,30,31,15,42,0,0,4,43,16,16,16,16,27,48,33,25,14,15,25,33,49,27,25,0,0,12,18,16,14,20,37,27,48,33,37,35,41,30,31,15,42,0,6,15,41,53,47,33,43,4,2,7,0,0,4,15,44,50,53,46,38,46,44,18,9,0,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,8,16,38,51,30,14,25,35,
53,45,29,26,0,0,22,40,45,52,30,36,46,47,52,41,18,7,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,1,37,36,51,46,34,11,27,50,48,40,22,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,44,46,46,34,12,0,22,33,46,46,51,46,32,2,0,0,8,16,38,51,30,14,25,35,53,45,29,26,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,30,14,25,35,53,45,29,26,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,10,39,
48,51,36,15,5,1,37,36,51,46,28,28,28,28,28,22,0,8,16,38,51,46,28,11,25,13,8,0,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,46,28,11,25,13,8,0,0,17,39,45,38,36,36,47,52,36,15,8,0,21,18,44,52,46,32,22,22,2,0,19,37,44,53,50,46,46,31,52,47,28,21,0,0,0,8,15,36,51,36,39,45,44,48,35,27,50,47,29,21,0,0,19,22,22,43,27,45,50,44,20,21,0,0,19,22,22,43,27,45,50,44,20,21,0,0,19,22,22,43,27,45,50,44,20,21,0,0,12,25,35,53,47,28,21,0,0,0,8,16,38,51,31,15,1,0,0,
8,16,38,51,50,51,38,29,23,9,0,0,0,8,16,38,51,31,44,45,48,46,33,18,1,0,0,0,23,29,31,48,48,31,35,53,45,29,26,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,6,16,31,51,31,47,48,36,41,48,45,39,17,0,0,0,1,6,6,21,14,38,36,20,40,30,29,30,31,15,24,9,0,0,1,6,6,21,14,38,36,20,12,12,13,39,30,38,28,17,0,0,0,7,8,5,21,14,38,36,20,40,30,29,30,31,15,24,9,12,32,46,52,41,37,21,13,32,23,0,0,0,10,14,44,46,48,48,47,44,18,9,0,0,8,16,38,51,31,15,1,8,16,38,51,31,
15,1,0,0,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,5,16,44,45,48,46,27,46,46,41,18,7,0,0,22,32,44,46,48,48,49,30,44,37,19,0,0,1,37,36,51,38,20,23,33,50,48,40,22,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,32,32,32,15,5,0,5,15,44,41,52,46,32,2,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,46,31,15,24,27,46,46,
29,26,0,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,46,31,15,24,27,53,45,29,26,0,0,12,32,30,51,45,33,37,24,37,36,51,46,34,23,23,23,23,21,0,8,16,38,51,31,15,6,9,9,0,0,8,16,38,46,31,15,1,0,0,0,0,8,16,38,51,31,15,6,9,9,0,0,0,17,39,38,47,48,49,45,31,39,11,0,0,9,18,44,52,46,32,12,0,0,0,0,22,29,38,48,48,36,35,46,47,28,26,0,0,0,9,13,35,53,30,27,49,33,46,41,41,52,38,20,4,0,0,0,1,13,40,46,53,36,14,17,0,0,0,0,1,13,40,46,53,36,14,17,0,0,
0,0,1,13,40,46,53,36,14,17,0,0,0,0,25,35,53,47,28,21,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,46,53,36,32,22,0,0,0,8,16,38,51,31,15,39,40,28,18,5,0,0,0,0,19,11,14,39,39,14,35,53,45,29,26,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,6,16,31,51,31,39,40,14,14,40,39,16,12,0,0,0,0,0,3,13,27,45,40,43,41,36,36,46,30,36,15,1,0,0,0,0,3,13,27,45,40,11,4,18,40,30,31,29,13,26,0,0,0,0,0,3,13,27,45,40,43,41,36,36,46,30,36,15,1,12,34,46,52,41,
32,28,27,27,24,0,0,0,0,10,37,28,35,41,29,20,23,0,0,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,17,15,39,40,29,40,38,34,34,25,0,0,0,9,10,37,28,40,40,38,32,43,10,0,0,0,1,37,36,51,38,20,23,33,50,48,40,22,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,2,2,4,0,0,0,
10,37,41,52,46,32,2,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,1,43,34,34,34,18,13,34,34,34,14,12,0,8,16,38,51,31,15,24,27,53,45,29,26,0,1,43,34,34,34,18,25,35,53,45,29,26,0,0,19,18,35,49,51,47,41,27,33,36,51,46,33,33,33,33,33,15,42,8,16,38,51,31,15,1,0,0,0,0,1,43,34,34,34,18,4,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,5,43,14,29,40,35,41,20,24,1,0,0,9,18,44,52,46,32,12,12,7,0,0,1,24,32,40,39,20,34,40,38,32,22,4,0,0,0,23,33,49,45,44,45,29,
31,30,38,52,44,43,3,0,0,3,24,39,30,52,31,28,22,12,8,0,0,3,24,39,30,52,31,28,22,12,8,0,0,3,24,39,30,52,31,28,22,12,8,0,0,0,25,35,53,47,28,21,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,27,45,50,41,14,17,0,0,8,16,38,51,31,15,10,22,21,1,0,0,0,0,0,0,9,12,22,22,25,35,53,45,29,26,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,6,16,31,51,31,15,22,12,12,22,10,4,0,0,0,0,0,0,17,28,30,41,15,43,44,44,44,30,31,44,15,1,0,0,0,0,17,28,30,41,15,5,
22,40,46,30,44,44,35,13,0,0,0,0,0,17,28,30,41,15,43,44,44,44,30,31,44,15,1,4,20,36,52,49,46,46,49,27,24,0,0,0,0,17,29,29,39,46,28,12,0,0,0,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,4,10,22,18,44,41,28,40,24,0,0,0,0,0,8,21,18,44,41,28,40,24,0,0,0,1,37,36,51,38,20,23,33,50,48,40,22,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,
0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,7,18,41,52,46,32,2,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,4,12,17,29,44,44,39,10,12,2,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,4,12,12,20,27,27,38,51,30,14,12,0,0,0,21,14,44,48,52,52,50,49,50,54,53,49,49,49,49,31,15,42,8,16,38,51,31,15,1,0,0,0,0,0,17,29,44,44,39,10,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,0,3,17,29,29,39,46,28,12,0,0,0,9,
18,44,52,47,29,28,32,22,0,0,0,7,12,22,10,2,18,44,41,28,40,24,0,0,0,21,28,46,50,38,30,14,44,45,47,49,27,11,0,0,0,22,28,31,52,30,29,28,28,28,18,3,0,22,28,31,52,30,29,28,28,28,18,3,0,22,28,31,52,30,29,28,28,28,18,3,0,0,25,35,53,47,28,21,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,35,49,49,44,16,21,0,8,16,38,50,31,15,1,0,0,0,0,0,0,0,0,0,0,0,0,0,25,35,50,45,29,26,0,0,9,18,44,50,50,30,29,22,0,0,0,0,0,6,16,31,50,31,15,42,0,0,0,
0,0,0,0,0,0,0,1,43,35,47,39,22,10,13,13,14,31,31,15,22,0,0,0,0,1,43,35,47,39,22,0,22,40,31,31,31,31,35,13,0,0,0,0,1,43,35,47,39,22,10,13,13,14,31,31,15,22,0,0,11,39,36,47,45,47,38,27,24,0,0,0,0,17,39,47,45,31,14,2,0,0,0,0,8,16,38,51,31,15,1,8,16,38,51,31,15,1,0,0,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,0,0,9,13,35,47,47,27,24,0,0,0,0,0,0,9,13,35,47,47,27,24,0,0,
0,1,37,36,51,38,20,23,33,50,48,40,22,0,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,7,18,41,52,46,32,2,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,24,27,50,44,37,4,0,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,4,20,38,50,53,45,27,18,3,0,0,0,0,10,16,33,36,30,46,46,46,46,46,46,46,46,46,31,15,42,8,16,38,51,31,15,1,0,0,0,0,0,24,27,50,
44,37,4,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,0,0,17,39,47,45,31,14,2,0,0,0,0,24,27,50,53,47,47,27,11,0,0,0,0,0,0,0,9,13,35,47,47,27,24,0,0,0,4,20,31,51,51,36,37,27,50,54,45,29,17,0,0,0,11,27,50,51,47,47,47,47,36,37,19,0,11,27,50,51,47,47,47,47,36,37,19,0,11,27,50,51,47,47,47,47,36,37,19,0,0,25,35,53,47,28,21,0,0,0,8,16,38,51,31,15,1,0,0,8,16,38,46,31,15,20,41,46,46,35,37,8,8,16,33,33,33,37,1,0,0,0,0,0,0,0,0,0,0,0,0,
0,24,33,33,33,29,26,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,6,15,27,27,27,15,42,0,0,0,0,0,0,0,0,0,0,4,20,40,40,37,8,0,0,1,13,16,16,25,7,0,0,0,0,4,20,40,40,37,8,0,21,15,16,16,16,16,16,22,0,0,0,0,4,20,40,40,37,8,0,0,1,13,16,16,25,7,0,0,1,11,20,29,39,29,20,13,21,0,0,0,0,21,20,29,39,14,11,9,0,0,0,0,8,16,38,46,31,15,1,8,16,38,46,31,15,1,0,0,0,0,8,16,38,46,31,15,1,0,0,0,8,16,38,46,31,15,1,0,8,16,38,46,31,15,24,27,
46,46,29,26,0,0,0,0,0,0,0,21,15,29,29,32,23,0,0,0,0,0,0,0,21,15,29,29,32,23,0,0,0,1,37,36,46,38,20,23,33,46,46,40,22,0,0,8,16,38,46,31,15,1,0,0,0,8,16,38,46,31,15,1,0,0,8,16,38,46,31,15,1,0,8,16,38,46,31,15,1,0,0,0,8,16,38,46,31,15,1,0,0,0,0,0,7,18,41,46,46,32,2,0,0,8,16,38,46,31,15,24,27,46,46,29,26,0,0,0,1,37,41,38,32,17,0,0,0,0,0,8,16,38,46,31,15,24,27,46,46,29,26,0,0,0,0,4,20,35,35,35,40,37,12,0,0,0,0,0,
0,2,24,37,14,34,34,34,34,34,34,34,34,34,34,43,19,8,16,38,46,31,15,1,0,0,0,0,1,37,41,38,32,17,0,0,0,0,0,8,16,38,46,31,15,1,0,0,0,0,0,0,0,21,20,29,39,14,11,9,0,0,0,0,21,32,41,46,46,46,27,11,0,0,0,0,0,0,0,0,21,15,29,29,32,23,0,0,0,7,18,44,46,46,35,25,39,46,46,30,14,5,0,0,0,11,27,46,46,46,46,46,46,36,37,19,0,11,27,46,46,46,46,46,46,36,37,19,0,11,27,46,46,46,46,46,46,36,37,19,0,0,25,35,46,46,28,21,0,0,0,8,16,38,46,31,15,1,0,0,
1,43,34,34,34,18,10,14,34,34,34,37,6,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,23,23,23,22,19,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,0,17,11,11,11,26,0,0,0,0,0,0,0,0,0,0,0,0,21,22,22,12,0,0,0,0,3,6,6,3,0,0,0,0,0,0,21,22,22,12,0,0,0,42,6,6,6,6,6,0,0,0,0,0,0,21,22,22,12,0,0,0,0,3,6,6,3,0,0,0,0,0,4,21,17,21,5,7,0,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,1,43,34,34,34,18,3,1,43,34,34,34,
18,3,0,0,0,0,1,43,34,34,34,18,3,0,0,0,1,43,34,34,34,18,3,0,1,43,34,34,34,18,23,34,34,34,14,12,0,0,0,0,0,0,0,0,4,26,17,12,7,0,0,0,0,0,0,0,0,4,26,17,12,7,0,0,0,3,18,34,34,34,43,22,32,34,34,14,26,0,0,1,43,34,34,34,18,3,0,0,0,1,43,34,34,34,18,3,0,0,1,43,34,34,34,18,3,0,1,43,34,34,34,18,3,0,0,0,1,43,34,34,34,18,3,0,0,0,0,0,0,25,34,34,34,15,4,0,0,1,43,34,34,34,18,23,34,34,34,14,12,0,0,0,7,25,20,20,24,7,0,0,
0,0,0,1,43,34,34,34,18,23,34,34,34,14,12,0,0,0,0,0,22,25,25,25,22,5,0,0,0,0,0,0,0,0,0,1,5,12,12,12,12,12,12,12,12,12,12,4,0,1,43,34,34,34,18,3,0,0,0,0,7,25,20,20,24,7,0,0,0,0,0,1,43,34,34,34,18,3,0,0,0,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,9,22,15,32,34,34,32,22,0,0,0,0,0,0,0,0,0,4,26,17,12,7,0,0,0,0,23,34,34,34,32,22,20,34,34,34,18,3,0,0,0,22,32,34,34,34,34,34,34,34,18,3,0,22,32,34,34,34,34,34,34,34,18,3,
0,22,32,34,34,34,34,34,34,34,18,3,0,0,11,34,34,34,20,12,0,0,0,1,43,34,34,34,18,3,0,0,0,4,12,12,12,8,0,12,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,8,0,0,4,12,12,12,8,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,4,12,12,12,8,0,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,4,7,12,12,12,12,0,0,0,0,4,12,12,12,8,0,0,0,0,0,4,12,12,12,8,0,0,0,0,4,12,12,12,8,0,0,0,4,12,12,12,8,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,
0,1,12,12,12,5,0,0,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,0,0,3,4,4,7,0,0,0,0,0,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,3,4,4,7,0,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,12,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,12,12,12,12,7,
2,12,12,12,8,0,0,0,0,7,12,12,12,12,12,12,12,12,8,0,0,7,12,12,12,12,12,12,12,12,8,0,0,7,12,12,12,12,12,12,12,12,8,0,0,0,19,12,12,12,2,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,9,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,9,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,8,8,8,0,0,0,0,0,0,0,0,9,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,8,8,8,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,7,42,42,9,0,0,0,19,42,42,42,9,0,0,7,42,42,42,3,5,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,42,42,42,7,0,42,42,42,1,0,0,0,0,0,0,0,19,5,12,12,8,0,0,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,0,0,0,0,1,1,9,8,4,3,0,0,0,0,0,9,42,42,1,0,0,7,42,42,42,19,0,0,0,1,42,42,42,42,42,42,1,42,42,42,0,0,0,0,
0,9,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,42,3,0,0,0,7,6,19,0,0,0,0,0,0,9,4,12,12,4,9,0,0,0,0,0,0,0,0,0,3,1,42,42,42,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,42,42,19,0,0,19,42,42,1,0,0,0,0,0,0,0,3,4,12,4,42,42,42,1,0,0,0,0,0,0,7,4,12,12,8,0,0,0,0,0,0,0,0,3,5,12,12,42,0,0,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,0,0,0,0,19,4,5,4,3,0,0,0,0,19,42,42,
42,1,9,42,42,42,42,0,0,0,0,0,0,0,3,5,12,12,42,0,0,0,0,0,0,0,7,8,5,4,1,0,0,0,0,0,0,0,0,0,0,0,0,19,4,5,4,3,0,0,0,0,0,0,7,8,5,4,1,0,0,0,0,0,0,0,0,0,0,0,0,19,4,5,4,3,0,0,0,0,0,0,0,42,4,4,8,3,0,0,0,0,0,0,0,0,0,0,9,11,20,20,20,22,19,3,0,0,0,0,0,0,0,0,0,11,20,20,20,22,19,19,19,19,19,19,0,0,0,0,0,11,20,20,20,22,19,3,0,0,7,19,19,19,9,0,0,0,0,11,20,20,20,22,19,3,0,0,0,0,
0,11,20,20,20,22,19,19,9,0,0,0,19,19,19,19,0,0,0,0,0,11,20,20,20,22,1,4,4,8,9,0,0,0,0,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,3,25,15,15,13,21,0,6,18,15,15,15,23,0,3,25,15,15,15,13,14,32,20,24,1,0,0,0,7,26,10,10,21,1,0,0,0,0,0,0,3,25,15,15,15,25,10,15,15,15,37,2,0,0,0,0,9,10,43,14,32,32,15,22,3,0,0,0,0,0,12,25,16,32,32,20,18,17,0,0,0,0,4,11,37,37,25,16,20,18,21,0,
0,0,0,11,15,15,37,11,4,25,15,15,15,18,6,0,5,37,15,15,15,15,15,15,37,15,15,15,10,0,0,0,0,11,20,20,20,22,7,0,0,0,0,0,3,3,0,0,0,0,0,19,11,15,15,43,23,42,21,18,16,37,21,0,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,0,0,2,11,43,15,15,15,15,25,3,0,0,0,2,10,10,10,8,0,0,0,0,0,0,21,15,15,15,18,8,8,18,15,15,15,21,0,0,0,0,0,26,18,20,32,37,15,15,15,37,5,0,0,0,0,21,13,20,32,32,16,25,2,0,0,0,0,9,17,18,14,32,32,37,22,3,0,0,0,0,
0,0,12,25,37,20,14,15,13,26,9,0,0,0,0,0,0,0,5,11,37,20,14,20,18,22,8,0,8,18,15,15,15,43,24,15,15,15,15,17,0,0,0,0,9,17,43,14,32,32,37,22,3,0,0,0,1,10,18,16,14,20,37,24,12,0,0,0,0,0,0,0,0,5,11,37,20,14,20,18,22,8,0,0,1,10,18,16,14,20,37,24,12,0,0,0,0,0,0,0,0,5,11,37,20,14,20,18,22,8,0,0,0,2,25,15,14,14,16,43,24,12,0,0,0,0,0,0,0,0,21,14,31,38,40,37,37,43,12,0,0,0,0,0,0,0,21,14,36,38,33,37,37,37,37,37,37,37,23,0,
0,0,21,14,36,38,33,37,37,18,4,42,13,37,37,37,24,9,0,0,21,14,36,38,33,37,37,18,4,0,0,0,21,14,36,38,33,37,37,37,25,3,0,26,37,37,37,37,22,0,0,0,21,14,36,38,33,24,37,20,14,16,13,26,9,0,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,8,16,31,31,27,14,21,26,28,31,31,35,18,7,8,16,31,31,31,27,38,46,31,39,11,0,0,0,26,28,39,39,28,43,12,0,0,0,0,0,6,16,31,31,31,15,24,27,31,31,39,17,0,0,0,9,23,32,44,30,
46,46,41,29,24,7,0,0,0,12,15,33,31,46,46,38,44,18,9,0,0,4,18,40,41,40,27,31,38,35,14,17,0,0,3,43,41,31,36,40,18,14,31,31,31,14,5,0,21,29,31,31,35,44,31,31,29,31,31,35,25,0,0,0,21,14,36,38,33,37,18,22,4,0,12,24,37,43,22,9,0,0,0,21,29,36,31,41,39,25,15,35,31,35,37,1,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,12,15,33,41,31,31,31,31,16,8,0,0,3,43,39,39,39,24,0,0,0,0,0,0,25,27,31,31,14,2,2,14,31,31,27,25,0,0,0,0,10,14,35,38,30,14,35,31,
31,29,26,0,0,0,12,16,27,38,46,46,31,33,37,2,0,0,0,23,32,35,30,46,30,41,29,24,7,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,2,32,31,31,31,40,14,31,31,31,27,11,0,0,0,0,23,32,44,30,46,30,41,29,24,7,0,0,24,29,35,31,30,38,36,33,20,10,0,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,0,24,29,35,31,30,38,36,33,20,10,0,0,0,0,0,0,21,15,33,41,38,30,31,44,39,13,7,0,12,15,33,36,30,30,31,41,27,43,7,0,0,0,0,0,
0,4,37,35,45,27,36,36,36,33,23,0,0,0,0,0,0,8,43,35,45,27,36,36,36,36,36,36,36,41,43,3,0,8,43,35,45,27,36,36,36,28,21,12,32,36,36,36,15,1,0,8,43,35,45,27,36,36,36,28,21,0,0,8,43,35,45,27,29,36,36,36,28,10,42,43,35,36,36,35,18,7,0,8,43,35,45,27,20,33,36,30,30,31,35,32,11,3,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,8,16,38,49,49,35,13,24,27,49,45,39,22,0,8,16,38,51,31,45,45,53,52,36,20,4,0,0,
17,39,45,48,46,27,43,6,0,0,0,0,6,16,31,51,31,15,24,27,53,45,39,17,0,0,0,21,14,36,50,49,47,50,53,38,32,21,0,0,1,43,35,45,51,48,47,48,44,18,9,0,0,11,40,46,48,40,47,52,49,49,44,15,4,0,3,43,41,45,52,46,29,27,49,49,27,25,9,0,21,29,45,53,35,44,52,46,29,45,53,35,25,0,0,8,43,35,45,27,36,36,44,39,43,22,14,27,41,41,32,10,0,0,0,21,29,47,45,52,30,32,33,45,51,30,14,5,0,1,43,35,48,53,45,45,52,45,27,13,3,0,0,1,43,35,45,52,50,49,49,38,16,8,0,0,2,14,30,48,
40,23,0,0,0,0,0,42,37,36,52,41,37,19,19,37,41,52,36,15,42,0,0,6,37,44,49,53,30,14,35,53,45,29,26,0,0,3,18,27,48,48,30,46,53,45,33,25,0,0,21,14,36,50,49,47,52,53,38,32,21,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,12,16,35,45,52,53,49,50,53,36,37,3,2,32,46,51,52,36,27,49,51,50,27,11,0,0,0,21,14,36,50,49,47,52,53,38,32,21,0,0,13,44,53,50,49,53,52,48,44,14,21,0,0,0,0,12,16,35,45,52,53,49,50,53,36,37,3,0,13,44,53,50,49,53,52,48,44,14,21,0,
0,0,0,12,16,35,45,52,53,49,50,53,36,37,3,7,13,27,45,51,50,49,50,52,41,43,7,0,0,0,0,0,0,24,40,47,27,35,50,51,52,44,43,19,0,0,0,0,0,11,39,47,35,37,36,51,51,52,52,52,52,41,43,3,0,11,39,47,35,37,36,51,47,28,21,12,32,46,51,31,15,1,0,11,39,47,35,37,36,51,47,28,21,0,0,11,39,47,35,37,18,35,49,50,44,37,22,29,46,51,30,34,17,0,0,11,39,47,35,20,44,48,51,50,50,52,50,31,28,22,0,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,13,35,49,52,38,27,27,36,50,53,
41,15,4,0,1,37,29,35,50,38,14,37,36,52,31,20,5,0,8,16,38,51,49,35,39,36,52,47,28,21,0,0,17,39,36,38,53,46,29,10,0,0,0,0,6,16,31,51,31,15,24,27,53,45,39,17,0,0,0,25,27,50,48,27,29,44,50,50,27,25,0,0,21,28,46,51,30,33,28,40,44,18,9,0,8,15,36,52,38,35,53,47,27,49,47,39,10,0,7,13,29,39,46,53,44,38,51,38,14,12,0,0,21,29,45,53,35,44,52,46,29,45,53,35,25,0,0,11,39,47,35,37,36,52,53,47,27,14,41,49,52,50,44,18,7,0,0,21,29,27,39,45,45,39,36,51,52,36,15,8,
0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,26,28,46,51,30,27,40,40,40,15,6,0,0,22,39,45,30,32,12,0,0,0,0,0,12,32,30,53,27,25,22,22,25,27,50,30,32,12,0,0,26,29,46,51,31,39,15,35,53,45,29,26,0,0,12,32,30,46,44,14,28,31,51,36,37,1,0,25,27,49,48,27,39,41,50,50,27,25,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,2,32,46,51,46,48,30,45,50,50,27,11,0,0,0,25,27,49,48,27,39,41,50,50,27,25,0,0,13,44,31,27,33,35,30,52,
49,35,43,1,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,0,13,44,31,27,33,35,30,52,49,35,43,1,0,0,7,13,27,48,51,46,35,33,27,36,36,37,3,4,20,38,51,30,27,33,27,36,41,43,7,0,0,0,0,0,0,13,39,39,20,31,51,45,52,30,32,12,0,0,0,0,0,25,39,39,16,37,36,51,47,41,41,41,41,41,43,3,0,25,39,39,16,37,36,51,47,28,21,12,32,46,51,31,15,1,0,25,39,39,16,37,36,51,47,28,21,0,0,25,39,39,16,12,21,32,38,51,47,39,15,41,53,48,27,25,7,0,0,25,39,39,16,35,49,52,30,27,33,36,50,53,
41,16,5,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,5,10,29,47,48,40,28,46,50,27,25,9,0,8,16,38,51,30,14,25,35,53,45,29,26,0,0,5,18,37,29,46,53,35,13,7,0,0,0,6,16,31,51,31,15,24,27,53,45,39,17,0,0,1,37,36,51,38,14,17,29,47,52,41,43,7,0,23,33,49,50,27,13,21,23,16,11,0,0,12,34,46,53,44,44,52,46,29,45,50,27,11,0,0,4,21,37,41,52,30,49,49,27,25,9,0,0,21,29,45,53,35,44,52,46,29,45,53,35,25,0,0,25,
39,39,16,37,44,41,47,51,30,27,49,50,38,53,41,43,3,0,0,2,15,37,32,46,45,44,47,51,30,39,11,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,13,22,22,22,21,0,0,9,13,35,53,41,37,1,0,0,0,0,0,26,29,45,49,33,32,40,40,32,33,49,45,29,26,0,0,23,33,49,50,27,25,25,35,53,45,29,26,0,0,2,16,14,14,14,10,13,35,53,31,15,1,1,37,36,51,38,14,23,29,47,52,41,43,7,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,2,32,46,52,
44,47,50,44,48,50,27,11,0,0,1,37,36,51,38,14,23,29,47,52,41,43,7,0,13,27,20,25,23,13,39,46,51,30,28,21,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,0,13,27,20,25,23,13,39,46,51,30,28,21,0,0,2,14,38,51,45,33,18,23,24,20,27,37,3,12,32,46,53,35,13,23,25,15,39,18,7,0,0,0,0,0,0,4,17,10,29,47,50,35,49,48,33,23,0,0,0,0,0,4,17,17,12,37,36,51,47,28,43,43,43,43,22,0,0,4,17,17,12,37,36,51,47,28,22,22,32,46,51,31,15,1,0,4,17,17,12,37,36,51,47,28,21,0,0,4,
17,17,12,0,9,25,33,45,52,41,33,45,52,36,20,12,0,0,0,4,17,17,14,30,51,47,39,25,11,32,38,51,45,39,22,0,0,10,39,45,51,36,40,40,40,40,35,53,50,27,24,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,0,4,16,36,53,44,33,49,46,29,17,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,3,8,15,36,51,38,20,5,0,0,0,6,16,31,51,31,15,24,27,53,45,39,17,0,0,8,16,31,51,31,15,12,32,46,51,36,37,3,0,24,27,50,49,33,22,0,0,8,9,0,0,21,29,47,53,35,44,52,46,29,45,53,35,
25,0,0,0,0,11,33,48,51,51,38,14,12,0,0,0,21,29,47,53,35,44,52,46,29,45,53,35,25,0,0,4,17,17,12,23,18,37,40,46,48,36,53,41,40,30,33,24,0,0,0,0,8,13,39,45,53,50,51,46,40,13,42,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,49,33,22,0,0,0,0,0,0,4,20,31,49,33,11,22,22,10,19,0,0,10,39,48,48,40,28,47,47,29,40,48,48,39,10,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,0,4,2,2,2,23,28,38,50,35,13,7,8,16,31,51,31,15,12,32,46,51,36,37,3,0,10,
39,45,51,36,39,39,39,39,35,53,50,27,24,0,0,10,39,45,51,31,16,5,0,0,2,24,26,0,2,32,46,52,44,44,36,39,48,50,27,11,0,0,8,16,31,51,31,15,12,32,46,51,36,37,3,0,12,24,12,0,0,8,37,41,52,48,33,22,0,0,10,39,45,51,31,16,29,29,29,20,24,26,0,0,12,24,25,29,29,29,20,41,52,48,33,22,0,0,10,39,45,51,31,32,32,32,32,32,24,26,0,2,14,30,51,30,35,39,32,37,22,5,0,0,0,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,0,0,0,0,3,37,36,51,47,40,40,40,40,39,11,0,
0,0,0,0,3,37,36,51,47,40,40,40,40,46,51,31,15,1,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,0,0,0,2,16,41,53,45,31,52,47,39,23,0,0,0,0,0,0,10,39,45,51,36,15,8,9,25,35,53,50,35,25,0,0,22,33,49,52,44,36,49,49,30,33,49,53,35,13,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,0,0,25,27,50,30,41,52,41,15,8,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,4,15,36,51,45,39,22,0,0,0,6,16,31,51,31,16,24,27,53,45,39,17,0,0,8,16,38,51,38,20,
21,28,46,52,41,43,3,0,11,33,50,50,27,25,12,4,0,0,0,0,21,28,47,52,44,44,52,46,40,48,50,27,24,0,0,0,0,12,32,30,51,49,27,13,7,0,0,0,12,28,46,52,44,44,52,46,40,48,50,27,24,0,0,0,0,0,0,0,7,4,43,44,53,50,46,29,18,20,43,5,0,0,0,9,24,39,30,52,48,36,53,36,37,13,22,0,0,11,33,50,49,33,11,25,27,53,48,40,22,0,0,11,33,50,50,27,24,2,9,0,0,0,0,17,29,47,46,40,40,40,40,29,10,0,0,17,29,45,49,33,29,45,45,39,33,49,45,39,17,0,0,23,33,49,50,27,11,13,35,53,
45,29,26,0,0,0,0,0,3,23,28,36,53,31,32,17,0,8,16,38,51,38,20,21,28,46,52,41,43,3,0,22,33,49,51,48,48,48,48,48,48,51,53,35,13,0,0,22,33,49,52,44,18,9,0,0,0,0,0,0,2,32,46,52,44,37,37,39,48,50,27,11,0,0,8,16,38,51,38,20,21,28,46,52,41,43,3,0,0,0,0,0,0,0,25,35,53,50,27,24,0,0,22,33,49,52,44,18,44,45,46,34,12,0,0,0,0,9,13,44,45,46,34,35,53,50,27,24,0,0,22,33,49,51,46,46,46,46,46,27,11,0,0,1,37,44,50,51,50,48,46,41,29,24,7,0,0,0,0,0,
0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,0,0,0,0,3,37,36,51,53,49,49,49,49,27,11,0,0,0,0,0,3,37,36,51,53,49,49,49,49,50,51,31,15,1,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,0,0,0,0,22,39,46,51,52,50,44,37,4,0,0,0,0,0,0,22,33,49,52,44,18,9,0,23,33,49,52,44,13,0,0,22,33,49,52,44,36,47,47,30,33,49,53,44,13,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,0,0,26,29,46,48,46,49,33,11,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,
0,0,10,29,47,54,53,35,18,7,0,0,6,16,31,51,46,29,14,41,52,48,39,14,21,0,8,16,38,51,47,40,20,27,49,50,35,25,0,0,10,29,47,51,38,40,28,15,10,0,0,0,4,20,31,51,30,44,52,46,41,53,45,39,22,0,0,0,0,12,14,31,51,46,28,21,0,0,0,0,4,20,31,51,30,44,52,46,41,53,45,39,10,0,0,0,0,0,0,0,0,0,11,27,49,51,31,16,8,4,1,0,0,0,0,12,14,31,52,53,35,35,53,31,15,27,15,1,0,10,29,47,52,41,14,32,36,51,46,28,21,0,0,10,29,47,51,38,39,14,24,1,0,0,0,25,27,50,50,49,
49,49,46,29,26,0,0,12,34,46,53,44,33,48,48,27,44,53,46,28,12,0,0,10,39,45,52,41,14,28,38,51,45,29,26,0,0,0,0,1,11,28,36,53,30,29,11,7,0,8,16,38,51,47,40,20,27,49,50,35,25,0,0,22,33,49,52,38,38,38,38,38,38,50,53,44,13,0,0,22,33,49,52,44,13,9,0,0,0,0,0,0,2,32,46,52,44,18,10,39,48,50,27,11,0,0,8,16,38,51,47,40,20,27,49,50,35,25,0,0,0,0,0,0,0,0,24,27,53,50,27,24,0,0,22,33,49,52,44,13,44,52,46,34,12,0,0,0,0,9,13,44,52,46,34,27,53,50,27,24,
0,0,22,33,49,54,50,50,50,50,50,27,11,0,0,0,10,14,44,46,48,53,51,53,31,14,12,0,0,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,0,0,0,0,3,37,36,51,49,47,47,47,47,27,11,0,0,0,0,0,3,37,36,51,49,47,47,47,47,48,51,31,15,1,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,0,0,0,0,6,43,35,50,51,30,28,10,0,0,0,0,0,0,0,22,40,49,52,44,18,7,0,11,33,50,53,35,13,0,0,22,39,48,52,41,28,28,28,28,27,50,53,35,25,0,0,22,39,48,52,41,37,19,0,24,27,50,
53,35,25,0,0,0,0,8,15,36,52,51,30,32,21,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,7,13,35,53,53,51,38,14,2,0,0,6,16,31,51,52,46,30,49,45,53,30,39,17,0,8,16,38,51,30,47,38,45,51,46,29,10,0,0,4,15,44,50,52,48,46,41,14,21,0,0,9,25,27,45,53,47,51,48,49,50,41,20,5,0,0,0,0,24,33,48,54,49,27,24,0,0,0,0,9,25,33,45,53,47,51,48,49,50,41,15,4,0,0,0,0,0,0,0,0,0,10,39,48,51,41,43,3,0,0,0,0,0,0,22,39,48,54,47,39,27,49,45,36,36,37,19,
0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,44,50,52,47,38,39,11,0,0,42,37,36,47,47,47,47,51,31,16,4,0,0,6,15,41,53,48,47,30,30,45,48,52,41,15,8,0,0,4,16,36,53,49,30,30,31,53,45,29,26,0,0,0,4,13,29,31,53,30,39,25,1,0,0,8,16,38,51,30,47,38,45,51,46,29,10,0,0,22,39,48,52,41,20,20,20,20,27,50,53,35,25,0,0,22,39,48,51,41,37,1,0,0,19,21,4,0,2,32,46,52,44,18,10,39,48,50,27,11,0,0,8,16,38,51,30,47,38,45,51,46,29,10,0,0,6,21,42,0,0,3,18,44,
52,49,33,23,0,0,22,39,48,51,41,37,44,46,46,34,21,4,0,0,6,21,13,44,46,46,34,44,52,49,33,23,0,0,22,39,48,51,36,33,33,33,33,33,11,4,0,0,21,17,37,28,33,44,30,51,48,40,22,0,0,0,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,0,0,0,0,3,37,36,51,47,28,28,28,28,28,22,0,0,0,0,0,3,37,36,51,47,28,28,28,28,46,51,31,15,1,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,0,0,0,0,0,17,39,45,52,41,18,3,0,0,0,0,0,0,0,26,29,47,51,31,20,2,1,43,44,53,
49,33,11,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,0,0,0,24,27,49,53,44,43,19,0,0,0,8,16,38,46,31,15,24,27,53,45,29,26,0,0,0,12,14,38,53,44,50,45,39,22,0,0,6,16,31,51,31,47,48,36,41,48,45,39,17,0,8,16,38,51,31,44,45,48,46,33,18,1,0,0,0,10,14,35,38,46,47,49,27,24,0,0,0,12,37,27,31,46,51,48,30,44,32,22,0,0,0,0,2,20,31,51,50,51,36,15,4,0,0,0,0,2,37,33,31,46,51,48,30,44,14,22,0,0,0,0,
0,0,0,0,0,0,17,39,45,52,41,18,7,0,0,0,0,0,0,26,29,46,47,35,37,14,41,46,52,44,18,7,0,0,22,32,44,46,52,52,46,44,14,17,0,0,0,0,10,14,44,30,46,50,36,15,42,0,6,37,28,28,28,28,36,53,35,13,9,0,0,0,23,29,36,46,30,27,27,30,50,45,40,11,0,0,0,0,23,29,31,48,48,31,35,53,45,29,26,0,0,12,37,33,30,53,31,39,25,10,43,22,0,8,16,38,51,31,44,45,48,46,33,18,1,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,12,32,46,51,46,29,23,12,26,13,28,13,7,2,32,46,52,
44,18,10,39,46,46,27,11,0,0,8,16,38,51,31,44,45,48,46,33,18,1,0,0,25,29,18,17,12,22,34,30,51,47,29,17,0,0,12,32,46,51,46,29,32,32,32,15,28,13,7,0,25,29,18,32,32,32,34,30,51,47,29,17,0,0,12,32,46,51,46,29,23,23,23,13,28,13,7,4,20,28,18,22,23,18,33,50,50,33,23,0,0,0,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,0,0,0,0,3,37,36,51,47,28,23,23,23,23,26,0,0,0,0,0,3,37,36,51,47,28,21,21,32,46,51,31,15,1,0,0,0,0,3,37,36,51,47,28,21,0,0,0,
0,0,0,0,0,0,0,17,39,45,52,41,18,7,0,0,0,0,0,0,0,12,15,41,50,48,33,13,11,29,46,52,38,32,17,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,0,0,0,17,39,48,50,27,11,0,0,0,0,1,43,34,34,34,18,24,27,53,45,29,26,0,0,0,23,33,48,48,40,46,53,44,18,7,0,6,16,31,51,31,39,40,14,14,40,39,16,12,0,8,16,38,51,31,15,39,40,28,18,5,0,0,0,0,0,17,18,14,32,39,45,44,13,9,0,0,0,2,25,20,44,52,46,14,43,10,9,
0,0,0,0,24,33,48,50,35,53,47,29,13,11,3,0,0,0,2,25,20,44,52,46,14,43,17,0,0,0,0,0,0,0,0,0,0,0,17,39,45,52,41,18,7,0,0,0,0,0,0,42,13,28,29,37,29,28,20,41,50,27,24,0,0,0,9,10,37,27,50,50,27,37,17,0,0,0,0,0,0,17,43,14,28,31,30,14,5,0,0,5,21,21,21,32,46,48,39,22,0,0,0,0,19,11,20,34,14,18,29,36,49,44,37,4,0,0,0,0,19,11,14,39,39,14,35,53,45,29,26,0,0,11,27,47,51,36,28,13,37,28,44,43,3,6,16,31,51,31,16,39,40,28,18,23,3,0,0,42,
37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,1,37,44,50,53,36,39,32,29,27,36,37,3,2,32,46,52,44,18,21,14,34,34,32,22,0,0,25,29,38,51,31,29,39,40,28,18,5,0,0,0,13,44,35,29,32,39,41,50,53,41,15,4,0,0,1,37,44,50,53,36,39,32,29,27,36,37,3,0,13,44,35,29,32,39,41,50,53,41,15,4,0,0,1,37,44,50,53,36,39,32,29,27,36,37,3,5,14,30,35,39,32,28,44,53,48,40,22,0,0,0,0,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,0,0,0,0,3,37,36,51,47,33,33,33,33,33,15,8,
0,0,0,0,3,37,36,51,47,28,21,12,32,46,51,31,15,1,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,0,0,0,0,0,17,39,45,52,41,18,7,0,0,0,0,0,0,7,18,33,33,41,53,30,40,32,31,53,38,33,33,20,4,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,0,0,17,39,48,50,27,11,0,0,0,0,0,4,12,12,12,8,24,27,53,45,29,26,0,0,1,37,41,52,38,14,36,51,38,14,2,0,6,16,31,51,31,15,22,12,12,22,10,4,0,0,8,16,38,51,31,15,
10,22,21,1,0,0,0,0,0,0,0,3,5,14,35,48,35,13,0,0,0,0,0,9,18,44,52,46,14,2,0,0,0,0,0,2,20,36,52,30,29,46,53,36,44,14,2,0,0,0,0,9,18,44,52,46,14,2,0,0,0,0,0,0,0,0,0,0,0,0,17,39,45,52,41,18,7,0,0,0,0,0,0,0,1,21,21,10,39,46,38,48,47,29,10,0,0,0,0,0,24,27,50,50,27,11,0,0,0,0,0,0,0,0,19,13,35,30,38,20,4,0,0,0,0,0,23,33,48,30,14,12,0,0,0,0,0,19,13,20,28,27,30,47,35,20,26,0,0,0,0,0,0,9,12,22,22,25,35,53,
45,29,26,0,0,12,15,35,48,49,36,44,36,46,41,43,3,3,43,44,53,47,27,27,27,27,27,34,12,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,10,34,31,50,52,45,46,47,49,36,37,3,2,32,46,52,44,18,9,12,12,12,12,7,0,0,13,44,45,51,45,47,47,27,11,1,0,0,0,0,13,44,50,47,46,45,53,53,38,29,22,0,0,0,0,10,34,31,50,52,45,46,47,49,36,37,3,0,13,44,50,47,46,45,53,53,38,29,22,0,0,0,0,10,34,31,50,53,45,46,47,49,36,37,3,5,14,30,53,45,46,46,50,52,38,14,12,0,0,0,0,0,
9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,0,0,0,0,3,37,36,51,53,49,49,49,49,38,16,8,0,0,0,0,3,37,36,51,47,28,21,12,32,46,51,31,15,1,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,0,0,0,0,0,17,39,45,52,41,18,7,0,0,0,0,0,0,7,43,41,49,49,51,48,40,32,46,54,50,49,30,20,4,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,0,0,17,39,48,50,27,11,0,0,0,0,0,0,0,0,0,0,24,27,50,45,29,26,0,0,
26,28,46,53,35,18,27,49,45,39,22,0,6,16,31,50,31,15,42,0,0,0,0,0,0,0,8,16,38,50,31,15,1,0,0,0,0,0,0,0,0,0,0,0,4,14,30,30,29,22,0,0,0,0,0,9,18,44,50,46,14,2,0,0,0,0,0,11,33,48,50,35,37,35,45,50,46,14,2,0,0,0,0,9,18,44,50,46,14,2,0,0,0,0,0,0,0,0,0,0,0,0,17,39,45,52,41,18,7,0,0,0,0,0,0,0,0,0,0,10,39,30,49,45,44,37,4,0,0,0,0,0,24,27,50,50,27,11,0,0,0,0,0,0,0,0,0,13,44,45,27,18,7,0,0,0,0,7,18,44,
53,41,37,19,0,0,0,0,0,4,20,41,46,47,36,40,37,21,0,0,0,0,0,0,0,0,0,0,0,25,35,50,45,29,26,0,0,0,21,16,27,30,49,50,45,38,27,18,7,0,23,39,30,50,50,50,50,50,47,28,12,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,7,11,28,44,46,48,49,45,30,35,43,3,2,32,46,50,44,18,9,0,0,0,0,0,0,0,13,35,38,50,31,35,35,27,11,0,0,0,0,0,13,27,38,45,48,48,46,41,29,24,19,0,0,0,0,7,11,28,44,46,48,49,45,30,35,43,3,0,13,27,38,45,48,48,46,41,29,24,19,0,
0,0,0,7,11,28,44,46,48,49,45,30,35,43,3,5,14,44,30,45,48,48,45,31,39,25,7,0,0,0,0,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,0,0,0,0,3,37,36,46,46,46,46,46,46,38,16,8,0,0,0,0,3,37,36,46,46,28,21,12,32,46,46,31,15,1,0,0,0,0,3,37,36,46,46,28,21,0,0,0,0,0,0,0,0,0,0,17,39,46,46,41,18,7,0,0,0,0,0,0,7,43,41,46,46,46,46,40,32,46,46,46,46,30,20,4,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,19,22,15,29,40,40,29,20,24,
4,0,0,0,0,0,0,0,17,29,33,33,33,11,0,0,0,0,0,0,0,0,0,0,24,33,33,33,29,26,0,0,25,27,46,46,39,22,28,46,46,44,18,3,6,15,27,27,27,15,42,0,0,0,0,0,0,0,8,16,33,33,33,37,1,0,0,0,0,0,0,0,0,0,0,0,4,20,33,29,25,1,0,0,0,0,0,9,18,33,33,33,14,2,0,0,0,0,7,43,33,33,33,28,26,15,39,33,33,14,2,0,0,0,0,9,18,33,33,33,14,2,0,0,0,0,0,0,0,0,0,0,0,0,17,39,46,46,41,18,7,0,0,0,0,0,0,0,0,0,0,5,13,34,33,40,16,17,0,0,
0,0,0,0,11,33,33,33,33,11,0,0,0,0,0,0,0,0,0,13,33,39,15,12,0,0,0,0,0,5,20,38,46,27,24,0,0,0,0,0,0,0,22,29,33,29,15,11,4,0,0,0,0,0,0,0,0,0,0,0,0,24,33,33,33,29,26,0,0,0,0,12,18,28,33,33,39,14,25,12,0,0,1,25,28,27,35,35,35,35,35,28,12,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,7,10,37,34,39,40,39,14,18,21,0,2,14,33,33,33,18,9,0,0,0,0,0,0,0,21,16,33,33,33,37,25,25,2,0,0,0,0,0,12,13,14,29,40,39,28,37,
22,19,0,0,0,0,0,0,7,10,37,34,39,40,39,14,18,21,0,0,12,13,14,29,40,39,28,37,22,19,0,0,0,0,0,0,7,10,37,34,39,40,39,14,18,21,0,9,22,18,14,29,40,40,29,20,24,6,0,0,0,0,0,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,0,0,0,0,7,13,34,34,34,34,34,34,34,34,43,1,0,0,0,0,7,13,34,34,34,20,12,5,15,34,34,34,18,19,0,0,0,0,7,13,34,34,34,20,12,0,0,0,0,0,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,0,9,25,34,34,34,34,34,14,15,34,34,34,
34,34,37,6,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,1,22,23,23,23,4,0,0,0,0,0,0,0,0,0,0,5,23,23,23,22,19,0,0,25,34,34,34,15,5,37,34,34,34,13,7,0,17,11,11,11,26,0,0,0,0,0,0,0,0,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,0,0,0,0,10,23,26,19,0,0,0,0,0,0,0,12,23,23,23,10,9,0,0,0,0,0,12,23,23,23,22,3,4,10,23,23,10,9,0,0,0,0,0,12,23,23,23,10,9,0,0,0,0,0,0,
0,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,0,0,0,0,0,0,3,21,22,22,2,0,0,0,0,0,0,0,5,23,23,23,23,4,0,0,0,0,0,0,0,0,0,12,23,22,5,0,0,0,0,0,0,5,15,34,34,14,26,0,0,0,0,0,0,0,3,22,23,26,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,23,23,23,22,19,0,0,0,0,0,19,21,23,23,10,2,7,0,0,0,0,3,26,11,25,25,25,25,25,11,19,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,1,12,10,22,10,2,3,0,0,9,10,23,23,
23,12,0,0,0,0,0,0,0,0,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,7,5,17,22,22,21,6,0,0,0,0,0,0,0,0,0,0,1,12,10,22,10,2,3,0,0,0,0,7,5,17,22,22,21,6,0,0,0,0,0,0,0,0,0,0,1,12,10,22,10,12,3,0,0,0,0,3,5,26,22,22,17,5,9,0,0,0,0,0,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,0,0,0,0,6,12,12,12,12,12,12,12,12,4,0,0,0,0,0,0,6,12,12,12,2,0,0,5,12,12,12,4,0,0,0,0,0,0,6,12,12,12,2,0,0,0,0,
0,0,0,0,0,0,0,0,2,12,12,12,1,0,0,0,0,0,0,0,0,1,12,12,12,12,12,12,5,12,12,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,12,12,12,5,0,4,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,12,12,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,12,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,1,4,5,4,1,0,0,0,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,
0,0,1,4,4,8,7,0,0,0,0,0,0,0,19,19,19,7,0,0,0,7,8,5,4,1,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,7,42,42,42,3,5,12,4,0,0,0,0,0,3,42,42,42,7,0,0,42,42,42,1,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,3,19,19,3,0,0,0,0,19,19,19,7,0,0,0,0,0,0,19,19,19,7,0,0,0,19,
4,5,4,3,0,0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,0,0,1,8,8,8,8,8,0,0,0,0,0,0,0,0,3,5,12,4,3,42,42,42,7,7,42,42,42,7,0,0,0,0,0,0,0,0,0,3,5,8,0,0,0,0,0,0,0,0,0,0,0,0,9,9,9,9,9,9,9,9,0,0,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,7,42,42,42,3,5,12,4,0,0,0,0,0,7,42,42,42,42,42,42,1,0,0,0,0,0,0,42,5,12,12,4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,42,42,42,42,
42,42,42,42,7,0,0,0,0,0,0,0,0,0,0,0,9,8,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,3,42,42,42,7,0,0,42,42,42,1,0,0,0,3,42,42,42,7,0,0,42,42,42,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,5,4,19,0,0,0,0,0,0,0,0,0,0,0,0,0,9,9,0,0,9,0,0,0,0,0,0,0,0,7,42,42,42,7,7,42,42,42,19,0,0,0,0,0,0,42,42,42,42,42,42,42,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,10,13,37,20,14,20,37,25,12,0,0,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,0,0,12,24,37,20,14,16,13,10,3,0,0,0,0,11,37,37,37,18,4,19,22,18,20,14,20,37,11,5,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,3,25,15,15,15,18,14,32,16,11,1,0,0,19,13,15,15,15,24,9,10,15,15,15,37,2,0,0,0,0,0,0,22,20,20,20,23,0,0,21,24,24,23,24,24,10,0,0,0,4,10,10,10,21,0,0,0,0,0,0,0,0,0,0,0,0,22,20,20,20,23,0,0,0,0,0,17,16,20,20,24,7,0,0,
0,0,0,0,0,2,43,37,37,18,4,0,0,26,37,37,37,25,1,0,0,0,0,11,37,37,37,18,4,5,11,37,20,14,20,18,22,8,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,0,0,0,4,43,20,20,16,20,15,12,0,0,0,0,0,0,21,18,14,32,16,13,15,15,15,24,25,15,15,15,25,7,0,0,0,0,7,12,22,24,13,14,15,24,23,17,8,0,0,0,0,0,0,3,24,18,18,18,18,18,18,18,18,23,0,0,0,0,0,0,12,25,16,32,32,20,18,17,0,3,25,15,15,15,13,14,32,16,11,1,0,0,3,25,15,15,15,15,15,15,37,12,0,0,
0,1,25,15,14,32,32,20,13,12,0,0,0,0,12,10,10,10,12,0,0,0,0,0,0,0,3,25,15,15,15,15,15,15,15,15,25,7,0,0,0,0,0,0,0,0,0,12,25,16,14,24,8,21,26,2,9,0,0,0,0,0,0,0,12,25,16,32,32,20,18,17,0,19,13,15,15,15,24,9,10,15,15,15,37,2,0,19,13,15,15,15,24,9,10,15,15,15,37,2,0,0,12,10,10,10,12,0,0,0,0,0,0,0,0,0,0,0,0,0,19,10,18,16,14,20,37,11,4,0,0,0,0,0,0,0,0,0,9,23,18,13,2,25,18,24,19,0,0,0,0,0,3,25,15,15,15,25,25,15,
15,15,43,4,0,0,0,0,22,15,15,15,15,15,15,15,24,9,0,0,0,0,12,10,10,10,12,0,0,0,0,0,0,4,20,35,36,38,30,38,36,27,20,17,0,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,12,14,27,36,30,30,31,35,28,25,6,0,0,19,37,36,36,36,28,12,25,29,44,31,30,38,41,33,37,21,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,8,16,31,31,31,27,30,46,36,39,24,7,0,4,16,36,31,31,20,5,13,35,31,31,29,26,0,0,0,0,0,12,20,41,38,27,13,0,7,43,27,27,28,27,27,20,8,0,0,25,39,39,
39,14,12,0,0,0,0,0,0,0,0,0,0,12,20,41,38,27,13,0,0,0,0,0,23,39,38,38,32,17,0,0,0,0,0,0,3,13,27,36,36,29,21,0,0,11,33,36,36,29,23,0,0,0,19,37,36,36,36,28,21,15,40,41,38,30,31,44,39,13,7,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,0,12,14,31,31,40,38,27,25,9,0,0,0,0,21,16,35,30,46,36,39,36,31,31,20,14,31,31,36,15,1,0,0,0,5,13,14,40,27,27,30,44,27,33,29,37,10,9,0,0,0,0,5,14,44,44,44,44,44,44,44,44,37,1,0,0,0,0,12,15,33,
31,46,46,38,44,18,9,8,16,31,31,31,27,30,46,36,39,24,7,0,8,16,31,31,31,31,31,31,39,10,0,0,0,2,32,36,30,46,46,38,27,37,4,0,0,17,37,39,39,39,37,22,10,9,0,0,0,0,8,16,31,31,31,31,31,31,31,31,15,1,0,0,0,0,0,0,0,0,17,20,27,31,41,18,37,28,29,14,11,19,0,0,0,0,0,12,15,33,31,46,46,38,44,18,9,4,16,36,31,31,20,5,13,35,31,31,29,26,0,4,16,36,31,31,20,5,13,35,31,31,29,26,0,42,37,39,39,39,37,19,0,0,0,0,0,0,0,19,6,6,1,6,25,29,44,31,30,38,41,40,
37,21,0,0,0,0,0,0,0,0,6,16,44,33,25,29,44,32,12,0,0,0,0,0,8,16,31,31,31,15,39,31,31,36,32,12,0,0,0,0,25,35,31,31,31,31,31,36,37,1,0,0,0,42,37,39,39,39,37,19,0,0,0,0,0,4,20,38,50,48,48,49,51,49,44,37,6,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,21,29,45,53,50,50,52,50,30,39,25,7,0,19,37,36,51,47,28,11,39,30,53,53,50,50,52,45,35,37,5,0,0,0,3,43,44,31,35,36,31,20,4,0,0,8,16,38,51,31,45,47,50,52,38,32,21,0,0,25,27,49,45,39,22,
20,31,51,36,15,8,0,0,0,0,1,13,27,48,35,37,2,0,7,43,41,47,28,30,38,20,8,0,0,25,35,48,47,28,21,0,0,0,0,0,0,0,0,0,1,13,27,48,35,37,2,0,0,0,0,0,6,13,33,47,44,37,5,0,0,0,0,0,26,28,30,53,44,43,19,0,0,2,20,31,53,36,16,4,0,0,19,37,36,51,47,28,16,35,45,52,53,49,50,53,36,37,3,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,0,9,11,39,47,27,44,31,14,12,0,0,0,1,18,35,48,49,47,52,30,27,50,47,29,39,45,49,27,24,0,0,0,5,43,33,30,48,50,
53,51,53,50,49,47,44,32,22,0,0,0,0,7,13,27,31,27,36,35,27,30,33,11,0,0,0,0,1,43,35,45,51,48,47,48,44,18,9,8,16,38,51,31,45,47,50,52,38,32,21,0,8,16,38,51,47,46,46,46,39,10,0,0,0,2,32,46,46,30,47,52,45,39,10,0,5,20,33,38,48,31,33,33,32,12,0,0,0,0,8,16,38,51,47,46,46,47,51,31,15,1,0,0,0,0,0,0,0,12,20,44,48,52,41,20,44,46,45,31,29,23,0,0,0,0,1,43,35,45,51,48,47,48,44,18,9,0,25,27,49,45,39,22,20,31,52,41,37,6,0,0,25,27,49,45,39,22,20,31,
52,41,37,6,0,8,16,38,48,31,15,1,0,0,0,0,0,0,1,18,16,16,37,24,39,30,53,53,49,50,52,45,35,37,4,0,0,0,0,0,0,0,3,43,44,31,35,36,31,20,4,0,0,0,0,0,8,16,38,51,31,39,30,53,36,34,23,9,0,0,0,0,25,35,53,48,46,47,51,36,37,1,0,0,0,8,16,38,48,31,15,1,0,0,0,0,0,4,20,36,27,40,39,27,46,54,46,32,12,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,21,29,46,44,33,27,41,49,52,38,34,26,0,19,37,36,51,47,28,14,31,52,48,41,33,35,46,51,45,33,24,0,0,
0,7,22,34,44,31,41,39,24,42,3,0,8,16,38,51,48,27,29,44,50,49,27,24,0,0,21,28,46,53,35,18,29,47,49,27,24,0,0,0,7,42,22,29,30,44,16,12,0,0,7,43,44,44,28,44,44,20,8,0,7,25,35,53,47,28,21,42,42,0,0,0,0,0,7,42,22,29,30,44,16,12,42,42,0,0,0,7,42,4,43,27,45,33,25,42,3,0,0,9,13,35,50,48,40,22,0,0,0,9,13,35,53,45,39,22,0,0,19,37,36,51,47,28,27,48,51,46,35,33,27,36,36,37,3,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,3,42,42,4,37,35,38,28,
30,33,24,1,0,0,21,28,46,53,44,29,46,53,36,46,50,27,35,53,46,28,21,0,0,9,25,33,47,51,47,36,41,47,31,41,38,50,50,41,20,12,0,0,0,4,43,28,33,37,40,29,18,33,20,15,22,0,0,0,21,28,46,51,30,33,28,40,44,18,9,8,16,38,51,48,27,29,44,50,49,27,24,0,8,16,38,51,31,32,32,32,14,21,0,0,0,2,32,40,32,14,39,47,49,33,22,0,5,14,30,50,54,50,49,47,28,12,42,42,0,0,8,16,38,51,31,32,32,38,51,31,15,10,26,4,0,0,0,0,9,25,27,48,52,30,27,35,49,50,49,53,36,16,4,0,0,0,21,28,
46,51,30,33,28,40,44,18,9,0,21,28,46,53,35,18,29,47,48,33,11,0,0,0,21,28,46,53,35,18,29,47,48,33,11,0,0,8,16,38,51,31,15,5,12,4,0,0,0,0,5,14,31,31,39,14,38,52,48,44,33,35,46,51,45,40,11,0,0,0,0,42,42,42,1,22,34,44,31,41,39,24,42,42,42,3,0,0,8,16,38,51,31,30,53,36,28,23,7,0,0,0,0,0,25,35,53,47,28,38,51,36,37,1,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,19,13,15,24,22,10,37,36,51,46,28,12,0,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,21,
29,29,43,11,11,20,41,53,50,27,25,0,19,37,36,51,47,28,33,48,52,41,20,23,13,33,48,52,36,15,8,0,3,25,15,15,15,15,37,15,15,15,13,1,8,16,38,51,38,14,17,29,47,52,41,43,7,0,1,37,44,53,38,14,27,50,46,28,21,0,0,3,25,15,15,39,39,20,15,37,12,0,0,10,13,15,15,15,13,23,9,7,25,15,35,53,47,28,15,15,15,23,0,0,0,3,25,15,15,39,39,20,15,15,15,15,21,0,3,25,15,15,15,37,39,39,15,15,13,1,0,4,20,31,51,30,14,12,2,2,2,5,22,39,45,53,35,18,7,0,19,37,36,51,47,28,38,51,45,33,
43,23,24,16,27,37,3,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,1,13,15,15,15,11,14,39,34,39,39,14,18,19,0,23,33,49,49,33,15,31,51,30,36,51,36,31,52,41,37,42,0,0,5,20,31,51,45,33,15,37,29,14,43,32,36,52,48,33,11,0,0,0,23,39,31,31,29,23,22,43,44,31,41,16,5,0,0,23,33,49,50,27,13,21,23,16,11,0,8,16,38,51,30,14,17,29,47,52,44,43,7,8,16,38,51,31,39,39,29,15,26,0,0,0,7,23,32,35,35,41,49,38,28,26,0,5,14,33,38,51,31,33,33,32,15,15,15,21,0,8,16,38,51,31,15,
16,38,51,31,39,39,29,15,26,0,0,0,4,20,31,51,47,39,32,30,52,36,35,50,47,29,17,0,0,0,23,33,49,50,27,13,21,23,16,11,0,0,1,37,44,53,38,14,27,50,30,14,12,0,0,0,1,37,44,53,38,14,27,50,30,14,12,0,0,8,16,38,51,31,15,14,32,20,24,1,0,0,4,14,30,48,40,33,49,52,41,16,23,18,27,48,52,41,37,1,0,0,22,15,15,15,37,21,37,15,15,15,23,18,15,15,15,13,19,0,8,16,38,51,49,52,31,29,15,26,0,0,0,0,0,9,13,44,52,47,28,38,51,36,37,1,0,0,0,8,16,38,51,31,15,1,0,0,0,0,
0,0,19,10,14,34,28,40,30,53,36,16,4,0,0,10,39,45,51,36,15,8,7,13,35,53,50,27,24,0,0,1,23,43,32,32,32,32,39,45,51,36,37,1,19,37,36,51,47,28,44,52,49,33,11,0,2,14,30,51,46,32,12,0,8,16,31,31,31,15,32,31,31,31,14,5,8,16,38,51,31,15,12,32,46,51,36,37,3,0,0,23,40,45,48,40,36,52,41,37,42,0,0,8,16,31,31,31,31,31,31,39,10,0,0,8,16,31,31,31,15,1,0,1,15,31,31,52,45,31,31,31,44,13,9,0,0,8,16,31,31,31,15,14,36,31,31,39,22,0,8,16,31,31,31,15,32,31,31,31,
14,5,0,26,29,47,51,36,37,23,14,14,14,20,21,32,46,51,31,20,4,0,19,37,36,51,47,40,45,51,31,40,40,40,40,39,24,26,0,0,10,39,45,51,36,39,39,39,39,35,53,50,27,24,0,4,20,31,31,31,16,21,24,40,38,46,38,20,8,0,24,27,50,49,40,37,36,51,30,27,49,46,47,49,33,11,0,0,0,17,29,45,51,31,16,5,4,21,12,3,11,33,49,52,44,43,7,0,8,15,41,52,36,37,6,9,22,39,45,45,39,23,0,0,24,27,50,49,33,22,0,0,8,9,0,8,16,38,51,31,15,20,32,46,51,36,37,3,8,16,38,51,49,48,48,47,44,15,5,0,
0,0,12,32,46,49,53,38,27,16,12,0,0,10,16,38,51,31,15,14,36,31,31,39,22,0,8,16,38,51,31,15,16,38,51,49,48,48,47,44,16,5,0,0,21,29,47,51,36,15,40,48,50,27,39,48,49,33,23,0,0,0,24,27,50,49,33,22,0,0,8,9,0,0,0,23,40,45,48,40,36,53,35,18,3,0,0,0,0,23,40,45,48,40,36,53,35,18,3,0,0,8,16,38,51,31,27,38,46,31,39,11,0,0,19,37,44,53,36,41,52,49,33,39,39,39,39,30,51,30,14,5,0,0,13,27,31,31,35,15,29,31,31,27,13,40,31,31,44,16,4,0,8,16,38,51,51,51,48,47,
44,15,5,0,0,0,0,3,43,41,52,30,14,38,51,36,37,1,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,0,0,22,33,46,47,48,48,44,28,23,0,0,0,22,33,49,52,44,18,9,0,23,33,49,53,35,13,0,0,0,8,16,38,46,46,46,46,49,51,38,20,4,19,37,36,51,48,46,46,51,45,39,17,0,1,37,36,51,45,29,21,0,8,16,38,51,31,15,35,50,51,30,14,5,8,16,38,51,38,20,21,28,46,52,41,43,3,0,0,2,20,31,53,44,46,49,33,24,0,0,0,8,16,38,51,47,46,46,46,39,10,0,0,8,16,38,51,31,15,1,0,1,15,31,46,52,
48,46,46,46,44,13,9,0,0,8,16,38,51,31,14,41,50,46,40,13,6,0,8,16,38,51,31,15,35,50,51,30,14,5,0,22,40,49,52,44,18,25,35,46,46,39,10,20,30,51,46,32,12,0,19,37,36,51,53,49,53,54,49,49,49,49,49,27,11,0,0,0,22,33,49,51,48,48,48,48,48,48,51,53,35,13,0,9,13,35,50,47,29,17,20,31,51,45,38,20,8,0,11,33,49,49,33,15,36,51,30,28,46,51,51,30,32,21,0,0,0,22,33,49,52,44,18,9,0,0,0,0,26,29,45,51,36,15,1,0,26,29,47,53,35,25,18,18,18,14,30,53,44,43,3,0,11,33,50,50,
27,24,12,12,12,3,0,8,16,38,51,38,14,41,44,46,52,41,43,3,8,16,38,51,31,41,31,53,45,40,22,0,0,4,11,14,33,33,35,45,48,33,11,0,0,8,16,38,51,31,14,41,50,46,40,13,6,0,8,16,38,51,31,15,16,38,51,31,41,31,53,45,40,23,0,0,10,39,48,52,44,13,33,49,50,33,29,45,50,27,11,0,0,0,11,33,50,50,27,24,8,26,13,10,0,0,0,2,20,31,53,44,46,47,39,22,0,0,0,0,0,2,20,31,53,44,46,47,39,22,0,0,0,8,16,38,51,31,45,45,53,52,36,20,4,0,0,22,29,30,53,48,51,53,48,48,48,48,48,49,
51,46,28,12,0,0,2,37,27,45,48,35,29,45,53,27,39,30,50,41,14,10,0,0,8,16,38,51,31,41,31,53,45,40,22,0,0,0,7,2,20,38,51,31,20,38,51,36,37,1,1,0,0,8,16,38,51,31,15,1,0,0,0,0,0,0,0,22,33,46,46,45,53,45,44,15,4,0,0,22,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,0,8,16,38,50,50,50,50,53,51,30,20,4,19,37,36,51,53,50,50,54,45,29,26,0,19,37,36,51,45,29,26,0,8,16,38,51,31,28,30,51,54,30,14,5,8,16,38,51,47,40,20,27,49,50,35,25,0,0,0,9,13,35,50,46,
49,46,34,21,0,0,0,8,16,38,51,31,32,32,32,14,21,0,0,8,16,38,51,31,15,1,0,3,18,32,35,53,47,28,28,40,39,16,17,0,0,8,16,38,51,31,44,49,46,33,13,4,0,0,8,16,38,51,31,28,30,51,54,30,14,5,0,23,33,49,52,44,18,25,35,53,48,39,10,20,38,51,47,28,12,0,19,37,36,51,49,47,53,51,47,47,47,47,47,27,11,0,0,0,22,33,49,52,38,38,38,38,38,38,50,53,44,13,0,0,10,29,47,50,27,25,39,45,49,27,28,37,1,0,17,29,47,53,35,32,30,52,41,37,41,52,52,41,37,1,0,0,0,22,33,49,52,44,13,9,
0,0,0,0,21,29,47,51,31,15,1,0,23,33,49,49,33,28,44,44,44,16,31,51,38,20,4,0,10,29,47,51,31,39,32,32,32,23,0,8,16,38,51,45,40,35,48,50,50,35,25,0,8,16,38,51,31,15,29,45,53,35,25,0,0,23,33,29,15,37,32,30,53,35,25,0,0,8,16,38,51,31,44,49,46,33,13,4,0,0,8,16,38,51,31,15,16,38,51,31,15,29,45,53,35,25,0,0,10,39,48,52,44,13,40,48,50,27,39,45,48,40,22,0,0,0,10,29,47,52,31,28,16,34,35,18,9,0,0,9,13,35,50,46,49,36,16,4,0,0,0,0,0,9,13,35,50,46,49,36,
16,4,0,0,0,8,16,38,51,49,35,39,36,52,47,28,21,0,0,1,13,39,36,30,51,48,38,38,38,38,38,38,38,38,29,21,0,0,0,12,37,35,48,45,27,45,53,27,30,53,36,32,22,0,0,0,8,16,38,51,31,15,29,45,53,35,25,0,0,9,25,20,33,45,53,35,20,38,51,36,15,15,37,21,0,8,16,38,51,31,15,1,0,0,0,0,0,0,3,17,14,32,32,39,31,51,47,39,10,0,0,22,39,48,52,41,37,19,0,24,27,50,53,35,25,0,0,0,12,15,33,33,33,33,33,45,51,31,16,6,19,37,36,51,47,33,41,52,48,40,22,0,4,20,31,51,46,28,21,0,
8,16,38,51,31,44,50,50,54,30,14,5,8,16,38,51,30,47,38,45,51,46,29,10,0,0,0,0,17,29,46,54,52,41,37,1,0,0,0,8,16,38,51,31,15,12,12,2,0,0,0,8,16,38,51,31,15,1,0,0,8,25,35,53,47,27,46,48,45,44,16,5,0,8,16,38,51,30,49,47,33,18,4,0,0,0,8,16,38,51,31,44,50,50,54,30,14,5,0,22,40,49,52,41,43,13,35,53,48,40,22,14,30,54,46,32,12,0,19,37,36,51,47,39,48,51,36,28,28,28,28,28,22,4,0,0,22,39,48,52,41,20,20,20,20,27,50,53,35,25,0,0,4,16,36,51,36,15,35,53,
30,32,21,5,0,0,4,15,44,50,47,38,50,47,39,24,33,49,48,33,11,0,0,0,0,22,39,48,51,41,37,1,0,42,19,0,22,39,48,52,41,37,19,0,24,27,50,49,40,28,47,52,44,15,31,51,30,14,5,0,4,37,44,49,53,47,46,46,35,25,0,8,16,38,51,30,47,38,48,51,46,29,10,0,8,16,38,51,31,15,28,47,52,44,13,0,0,23,33,47,31,36,38,50,48,33,11,0,0,8,16,38,51,30,49,47,33,18,4,0,0,0,8,16,38,51,31,15,16,38,51,31,15,28,47,52,44,13,0,0,26,29,47,51,36,37,32,30,52,41,33,49,46,34,21,0,0,0,4,15,
44,50,53,46,38,46,44,18,9,0,0,0,17,29,46,54,49,27,24,0,0,0,0,0,0,0,17,29,46,54,49,27,24,0,0,0,0,8,16,38,51,30,14,25,35,53,45,29,26,0,0,0,8,11,15,41,52,45,39,20,20,20,20,20,20,20,37,5,0,0,0,0,21,16,35,49,47,49,53,30,52,31,28,23,9,0,0,0,8,16,38,51,31,15,28,47,52,44,13,0,0,19,37,36,45,51,47,39,20,38,51,38,31,31,40,22,0,8,16,38,51,31,15,1,0,0,0,0,0,4,18,13,26,2,12,25,35,53,49,33,22,0,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,12,
14,20,23,23,23,37,35,50,53,44,18,7,19,37,36,51,47,28,27,50,53,35,43,12,23,29,47,51,31,20,4,0,8,16,38,51,31,46,48,36,51,30,14,5,8,16,38,51,31,44,45,48,46,33,18,1,0,0,0,0,6,37,41,52,48,33,11,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,25,35,53,45,46,30,47,51,45,39,22,0,8,16,38,51,51,50,49,35,43,8,0,0,0,8,16,38,51,31,46,48,36,51,30,14,5,0,17,39,45,51,31,16,37,41,53,50,27,25,29,47,51,30,14,5,0,19,37,36,51,47,28,30,51,46,29,
11,21,26,13,28,13,7,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,0,25,27,50,46,29,38,52,44,37,1,0,0,0,0,10,32,41,45,48,46,27,43,25,27,50,30,32,12,0,0,0,0,12,32,30,51,46,29,24,24,15,43,23,15,44,53,50,27,25,0,0,11,27,50,49,33,29,45,52,41,16,31,51,30,14,5,0,0,17,14,35,38,46,49,53,35,25,0,8,16,38,51,31,44,45,48,47,31,32,17,0,8,16,38,46,31,15,28,47,52,44,13,0,0,23,33,46,48,49,48,46,44,20,12,0,0,8,16,38,51,51,50,49,35,43,8,0,0,0,8,16,38,46,31,15,
16,38,46,31,15,28,47,52,44,13,0,0,5,14,38,51,46,29,18,27,48,46,36,53,44,43,25,5,0,0,0,10,14,44,46,48,48,47,44,18,9,0,0,0,6,37,41,52,30,34,21,0,0,0,0,0,0,3,17,37,41,52,30,34,10,5,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,0,25,27,50,53,44,15,17,12,26,25,14,16,4,0,0,0,0,0,21,20,36,53,53,54,54,52,52,46,39,23,0,0,0,0,8,16,38,46,31,15,28,47,52,44,13,0,0,19,37,36,45,38,27,37,20,38,46,46,53,49,40,22,0,8,16,38,51,31,15,1,0,0,0,0,
0,21,29,27,29,14,20,28,36,52,48,40,22,0,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,21,29,41,40,34,28,27,47,51,47,39,22,0,19,37,36,51,47,28,28,30,51,46,33,32,29,36,53,49,27,13,7,0,8,16,38,51,48,53,36,36,51,30,14,5,8,16,38,51,31,15,39,40,28,18,5,0,0,0,0,21,24,25,27,50,30,32,12,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,25,35,53,50,44,14,33,49,50,27,24,0,8,16,38,51,45,35,49,47,39,24,9,0,0,8,16,38,51,48,53,36,36,51,30,
14,5,0,2,14,30,51,45,33,40,46,38,45,31,29,35,50,52,41,37,19,0,19,37,36,51,47,28,44,50,53,31,39,32,29,27,36,37,3,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,0,26,29,46,50,27,45,48,33,23,0,0,0,0,0,9,22,20,39,40,28,18,24,20,36,53,44,43,19,0,0,0,0,19,18,35,49,53,31,27,33,31,35,33,44,45,52,30,34,26,0,0,22,40,48,53,44,33,49,47,38,29,46,51,36,15,6,0,0,0,21,18,14,29,45,53,35,25,0,8,16,38,51,31,15,39,40,33,30,39,23,0,1,43,34,34,34,18,29,47,53,35,13,0,
0,26,15,28,39,35,41,28,37,17,0,0,0,8,16,38,51,45,35,49,47,39,24,9,0,0,1,43,34,34,34,18,43,34,34,34,18,29,47,53,35,13,0,0,7,13,27,49,53,31,39,29,44,53,50,38,29,39,33,23,0,0,0,0,10,37,28,35,41,29,20,23,0,0,0,0,0,11,33,50,41,43,19,0,0,0,0,0,0,10,29,39,33,50,41,39,39,18,7,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,0,17,28,30,51,45,35,29,32,29,27,31,14,2,0,0,0,0,6,43,27,49,45,41,50,52,30,38,52,36,14,21,0,0,0,1,43,34,34,34,18,29,47,
53,35,13,0,0,19,37,40,29,14,13,2,43,34,34,41,53,36,32,17,0,8,16,38,51,31,15,1,0,0,0,0,0,21,29,47,47,30,30,46,53,53,31,14,12,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,21,29,45,48,46,46,49,51,45,27,43,8,0,19,37,36,51,47,28,18,27,45,51,48,46,47,53,50,36,14,21,0,0,8,16,38,51,54,45,40,36,51,30,14,5,8,16,38,51,31,15,10,22,21,1,0,0,0,0,9,18,27,27,38,53,44,43,19,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,25,35,53,
45,39,10,39,45,53,35,25,0,8,16,38,51,31,14,31,52,31,32,17,0,0,8,16,38,51,54,45,40,36,51,30,14,5,0,3,18,35,49,51,48,45,50,35,36,53,45,49,51,45,40,11,0,0,19,37,36,51,47,28,34,31,50,52,45,46,47,49,36,37,3,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,6,37,41,52,31,53,30,14,12,0,0,0,0,0,0,0,5,10,22,21,14,27,35,48,45,40,23,0,0,0,0,0,0,21,20,44,45,52,50,49,52,50,49,53,50,30,40,25,3,0,0,12,32,30,51,49,47,45,41,50,47,52,50,35,13,9,0,0,0,0,3,
26,29,45,53,35,25,0,8,16,38,51,31,15,10,22,37,39,25,6,0,0,4,12,12,37,27,44,49,49,33,11,0,0,0,17,29,29,39,46,28,12,0,0,0,0,8,16,38,51,31,14,31,52,31,32,17,0,0,0,4,12,12,12,8,4,12,12,37,27,44,49,49,33,11,0,0,0,21,14,41,50,52,48,45,49,51,53,50,45,45,33,23,0,0,0,0,17,29,29,39,46,28,12,0,0,0,0,0,0,23,33,50,41,43,7,0,0,0,0,0,0,22,40,48,48,52,48,48,41,43,7,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,0,19,13,33,47,52,50,47,46,47,49,
46,14,2,0,0,0,0,11,39,46,50,44,29,45,53,27,33,45,49,35,43,8,0,0,0,4,12,12,37,27,44,49,49,33,11,0,0,0,12,22,17,2,7,0,4,18,27,48,31,28,23,9,0,8,16,38,51,31,15,1,0,0,0,0,0,21,29,38,47,48,49,48,46,36,29,24,9,0,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,21,29,41,46,48,48,45,38,33,37,12,0,0,19,37,36,46,46,28,12,37,27,38,45,48,48,46,44,32,23,0,0,0,8,16,38,51,53,41,15,36,51,30,14,5,8,16,38,50,31,15,1,0,0,0,0,0,0,0,9,18,44,50,50,30,
29,22,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,25,35,53,47,28,21,29,45,53,35,25,0,8,16,38,51,31,15,39,46,50,44,37,5,0,8,16,38,51,53,41,15,36,51,30,14,5,0,0,21,14,44,47,49,45,41,14,29,31,48,48,46,27,37,4,0,0,19,37,36,46,46,28,23,34,44,46,48,49,45,30,35,43,3,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,0,24,33,49,51,53,44,18,3,0,0,0,0,0,0,0,0,0,0,2,14,46,53,49,41,16,5,0,0,0,0,0,0,0,17,16,40,44,36,
31,48,38,31,41,27,28,25,8,0,0,0,3,13,33,46,48,45,41,40,30,48,45,36,32,26,0,0,0,0,0,0,26,29,45,50,35,25,0,8,16,38,50,31,15,1,0,12,10,1,0,0,0,0,0,1,37,36,53,49,36,14,12,0,0,0,17,39,47,45,31,14,2,0,0,0,0,8,16,38,51,31,15,39,46,50,44,37,5,0,0,0,0,0,0,0,0,0,19,37,36,53,49,36,14,21,0,0,0,0,22,14,35,30,48,49,45,38,41,47,48,45,33,23,0,0,0,0,17,39,47,45,31,14,2,0,0,0,0,0,0,23,33,50,41,43,7,0,0,0,0,0,0,10,39,33,33,50,41,33,
33,18,7,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,0,0,5,43,40,36,47,48,48,45,30,44,14,2,0,0,0,21,14,36,52,31,32,29,45,53,27,37,44,50,46,39,11,0,0,0,0,0,1,37,36,53,49,36,14,12,0,0,0,0,0,0,0,0,0,23,29,46,31,28,11,7,0,0,8,16,38,51,31,15,1,0,0,0,0,0,4,13,20,29,39,40,39,28,15,23,19,0,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,19,23,15,28,39,40,29,14,25,2,0,0,0,7,13,34,34,34,20,2,12,13,14,39,40,39,28,37,10,9,0,0,0,
8,16,38,46,46,39,37,36,46,30,14,5,8,16,33,33,33,37,1,0,0,0,0,0,0,0,9,18,35,35,27,29,25,19,0,0,0,0,0,8,16,38,46,31,15,1,0,0,0,0,0,8,16,38,46,31,15,1,0,0,0,25,35,46,46,28,21,29,46,46,35,25,0,8,16,38,46,31,15,18,27,46,46,33,25,0,8,16,38,46,46,39,37,36,46,30,14,5,0,0,0,10,15,29,40,39,16,22,11,14,39,40,28,43,12,0,0,0,7,13,34,34,34,20,2,10,37,34,39,40,39,14,18,21,0,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,21,34,30,46,46,39,
22,0,0,0,0,0,0,0,0,0,0,0,2,14,35,35,33,14,10,0,0,0,0,0,0,0,0,0,5,23,18,37,16,40,29,15,37,25,26,3,0,0,0,0,0,5,18,28,40,39,20,13,28,40,39,20,22,9,0,0,0,0,0,0,26,29,33,33,33,24,0,8,16,33,33,33,37,1,0,0,0,0,0,0,0,0,0,1,37,35,35,27,32,23,0,0,0,0,21,20,29,39,14,11,9,0,0,0,0,8,16,38,46,31,15,18,27,46,46,33,25,0,0,0,0,0,0,0,0,0,19,37,35,35,27,32,23,9,0,0,0,0,0,26,43,34,39,40,39,14,15,29,40,39,14,10,0,0,0,0,
21,20,29,39,14,11,9,0,0,0,0,0,0,23,33,35,35,43,7,0,0,0,0,0,0,42,23,23,33,35,35,43,23,12,0,0,0,8,16,38,46,31,15,24,27,46,46,29,26,0,0,0,0,0,0,0,4,24,20,29,40,40,39,14,43,22,9,0,0,1,43,35,46,46,40,24,29,46,46,27,24,32,31,46,31,14,21,0,0,0,0,1,37,35,35,27,32,23,0,0,0,0,0,0,0,0,0,7,18,33,33,29,11,3,0,0,0,8,16,38,46,31,15,1,0,0,0,0,0,0,7,5,26,22,22,10,21,4,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,
0,8,21,22,22,17,5,9,0,0,0,0,0,8,12,12,12,2,0,0,7,12,10,22,22,21,42,0,0,0,0,0,1,43,34,34,34,37,13,34,34,34,37,8,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,26,25,25,24,10,19,0,0,0,0,0,0,1,43,34,34,34,18,3,0,0,0,0,0,1,43,34,34,34,18,3,0,0,0,11,34,34,34,20,12,20,34,34,34,11,0,1,43,34,34,34,18,12,20,34,34,34,25,0,1,43,34,34,34,37,13,34,34,34,37,8,0,0,0,0,4,17,22,10,4,0,9,12,10,22,21,1,0,0,0,0,0,8,12,12,12,2,0,0,1,12,
10,22,10,2,3,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,1,43,34,34,34,16,2,0,0,0,0,0,0,0,0,0,0,0,7,23,25,25,11,12,0,0,0,0,0,0,0,0,0,0,0,0,7,1,2,22,26,1,19,0,0,0,0,0,0,0,0,0,1,21,22,10,5,19,21,22,10,2,0,0,0,0,0,0,0,0,19,22,23,23,23,5,0,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,0,0,10,25,25,11,21,9,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,1,43,34,34,34,18,12,20,34,34,34,25,0,0,0,0,0,0,0,
0,0,0,10,25,25,11,21,9,0,0,0,0,0,0,0,1,12,10,22,17,2,4,26,22,10,2,0,0,0,0,0,0,5,21,17,12,7,0,0,0,0,0,0,0,5,25,25,25,17,0,0,0,0,0,0,0,0,0,5,25,25,25,17,0,0,0,0,0,1,43,34,34,34,18,23,34,34,34,14,12,0,0,0,0,0,0,0,0,0,4,26,22,22,17,12,19,0,0,0,0,1,43,34,34,34,37,12,20,34,34,34,23,24,34,34,34,14,12,0,0,0,0,0,10,25,25,11,21,9,0,0,0,0,0,0,0,0,0,0,12,23,23,22,19,0,0,0,0,1,43,34,34,34,18,3,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,4,6,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,19,12,12,12,2,0,2,12,12,12,3,0,0,4,12,12,12,8,0,2,12,12,12,1,0,0,4,12,12,12,4,6,12,12,12,
4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,8,0,2,12,12,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,4,0,2,12,12,12,3,19,12,12,12,2,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,7,42,42,42,7,7,42,42,42,7,0,0,0,0,0,7,42,42,42,1,0,0,42,42,42,42,0,0,0,0,0,0,0,0,0,0,19,4,5,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,0,0,0,0,0,1,42,42,42,42,42,42,42,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,42,42,42,42,42,42,0,0,0,9,42,42,42,19,0,7,42,42,42,19,0,0,0,0,0,0,0,0,3,19,19,19,0,0,0,0,0,0,0,0,0,0,0,1,4,5,4,1,0,0,0,0,0,0,0,0,0,0,0,0,42,5,12,2,8,0,0,0,0,0,0,0,0,0,0,9,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,3,4,5,4,1,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,9,0,0,0,0,0,0,0,0,0,1,4,5,4,3,0,0,0,0,0,0,0,42,42,42,42,42,42,42,42,0,0,0,7,42,42,42,3,5,12,4,0,0,19,42,42,42,7,0,0,0,0,0,1,2,2,1,42,42,42,1,0,0,0,0,0,0,42,42,42,42,42,42,42,42,0,0,0,0,0,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,0,0,0,0,0,0,0,7,4,5,1,0,0,0,0,4,5,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,4,5,4,0,0,0,0,0,0,0,0,0,0,0,0,9,8,5,4,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,25,15,15,15,25,25,15,15,15,25,7,3,25,15,15,15,25,25,15,15,15,25,7,0,0,0,3,25,15,15,15,37,2,17,15,15,15,15,23,0,0,0,0,0,0,0,12,24,37,20,14,20,37,24,12,0,0,0,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,42,13,37,37,37,37,37,37,37,37,37,23,0,0,0,12,37,
15,15,15,15,15,15,15,24,9,0,0,0,0,5,10,10,10,10,10,12,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,0,21,24,24,23,24,24,10,0,0,0,0,0,0,22,15,15,15,15,15,15,15,22,0,0,11,15,15,15,43,4,25,15,15,15,18,6,0,0,0,0,0,0,4,18,37,37,37,23,0,0,0,0,0,0,0,0,21,25,37,20,14,20,37,11,5,0,0,0,0,0,0,0,0,3,24,15,14,32,14,15,11,6,0,0,0,0,0,0,0,3,25,37,37,
37,37,37,37,37,37,23,0,0,0,0,0,0,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,0,0,0,0,0,0,0,0,0,0,5,23,37,20,14,20,37,25,26,3,0,1,13,37,37,37,37,37,37,37,37,37,37,24,9,0,0,0,0,0,0,12,25,15,20,14,20,37,24,2,0,0,0,0,22,15,15,15,15,15,15,15,15,26,0,3,25,15,15,15,13,14,32,16,11,8,18,15,15,15,25,7,0,0,0,10,37,14,14,37,15,15,15,37,5,0,0,0,0,22,15,15,15,15,15,15,15,15,26,0,0,0,0,0,0,3,25,15,15,15,25,25,15,15,15,25,7,0,0,0,0,
0,0,0,12,13,20,14,37,22,3,0,17,20,14,20,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,17,20,14,20,17,0,0,0,0,0,0,0,0,0,7,26,13,16,14,20,37,11,5,0,0,0,0,0,0,0,0,0,0,0,19,22,22,17,9,0,0,0,0,11,37,37,37,37,37,43,25,17,3,0,0,0,0,0,0,0,0,0,0,8,16,31,31,31,15,16,31,31,31,15,1,8,16,31,31,31,15,16,31,31,31,15,1,0,0,0,8,16,31,31,31,40,23,13,35,31,31,44,18,9,0,0,0,0,0,21,14,33,41,38,30,38,36,33,20,22,9,0,0,0,0,0,0,
0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,12,32,36,36,36,36,36,36,36,36,41,43,3,0,0,10,39,31,31,31,31,31,31,36,37,19,0,0,0,7,18,39,39,39,39,39,15,6,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,7,43,27,27,28,27,27,20,8,0,0,0,0,0,13,35,31,31,31,31,31,35,25,0,3,43,35,31,31,40,24,34,31,31,36,14,5,0,0,0,0,0,0,21,29,36,36,36,37,3,0,0,0,0,0,0,7,18,27,36,38,30,
38,41,33,43,4,0,0,0,0,0,0,0,4,14,36,30,46,30,36,39,24,9,0,0,0,0,0,0,4,20,36,36,36,36,36,36,36,41,43,3,0,0,0,0,0,0,19,37,36,36,36,28,12,12,32,36,36,36,15,1,0,0,0,0,0,0,0,0,0,0,21,37,40,41,38,30,38,36,27,28,22,0,2,14,36,36,36,36,36,36,36,36,36,36,15,1,0,0,0,0,0,12,16,27,36,30,30,38,41,27,24,0,0,0,0,25,35,31,31,31,31,31,31,27,11,0,8,16,31,31,31,27,30,46,36,29,24,39,31,31,44,37,1,0,0,10,14,44,30,30,44,35,31,31,29,26,0,0,0,0,
25,35,31,31,31,31,31,31,27,11,0,0,0,0,0,0,8,16,31,31,31,15,16,31,31,31,15,1,0,0,0,0,0,0,4,37,27,31,30,41,29,23,1,43,35,30,39,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,43,35,30,39,22,0,0,0,0,0,0,0,0,3,11,32,35,31,30,38,41,40,37,6,0,0,3,42,42,42,19,2,12,42,22,29,40,32,12,0,0,0,19,37,36,36,36,36,36,41,27,28,24,7,0,0,0,0,0,0,0,0,0,8,16,38,51,31,15,16,38,51,31,15,1,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,8,16,38,51,52,
41,37,14,38,51,52,44,18,9,0,0,0,0,0,17,39,48,52,50,49,53,52,48,41,32,10,0,0,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,12,32,46,52,52,52,52,51,51,51,41,43,3,0,0,10,39,48,49,49,49,52,51,36,37,19,0,0,0,7,43,41,48,48,48,38,20,8,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,0,0,0,13,35,53,48,46,46,46,35,25,0,0,26,14,31,52,31,
14,44,50,46,39,11,0,0,0,0,0,0,0,21,29,47,51,36,37,3,1,42,42,42,0,0,7,18,41,50,48,48,53,52,47,40,11,0,3,42,42,42,7,0,4,14,30,30,38,48,52,31,20,4,0,0,0,0,0,0,4,20,30,51,51,52,52,51,51,41,43,3,1,42,42,42,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,9,42,42,42,1,0,0,0,0,12,16,35,45,52,53,50,50,53,50,27,11,0,2,14,46,52,52,52,51,51,52,52,52,31,15,42,1,0,0,0,9,13,27,48,51,50,48,48,53,27,24,0,0,0,0,25,35,53,53,49,49,52,50,27,11,0,8,16,
38,51,31,45,47,52,52,38,32,31,52,30,28,10,0,0,6,37,44,50,53,45,45,44,53,45,29,26,0,0,0,0,25,35,53,53,49,49,52,50,27,11,0,0,0,0,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,0,0,0,22,39,47,31,35,45,36,16,22,29,46,44,37,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,29,46,44,37,8,0,0,0,0,0,0,0,0,23,29,31,50,52,50,50,52,30,20,4,0,3,25,15,15,15,18,14,32,37,43,44,36,32,15,22,9,0,19,37,36,51,51,52,52,52,50,38,34,26,4,12,12,5,1,0,0,0,0,
8,16,38,51,31,15,16,38,51,31,15,1,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,8,16,38,51,54,46,29,33,48,54,52,44,18,9,0,0,0,0,0,17,39,46,41,27,33,35,30,52,50,44,37,8,0,0,0,42,42,42,1,43,44,44,28,44,44,20,8,42,42,3,0,0,12,32,41,41,41,41,41,31,51,49,35,43,3,0,0,10,29,40,40,40,35,49,48,35,37,19,0,0,7,42,43,33,33,33,33,33,16,8,0,0,7,42,43,44,44,28,44,44,20,8,0,0,3,13,35,49,52,38,27,27,36,50,53,41,15,4,0,0,0,1,43,44,44,28,44,44,20,8,0,0,0,
7,42,42,43,44,44,28,44,44,20,8,42,7,0,0,12,13,35,53,47,28,32,32,32,11,0,0,9,24,40,47,49,35,47,49,35,43,8,0,0,0,0,0,0,0,21,29,47,51,36,37,12,37,15,15,15,23,0,7,18,41,27,39,40,44,50,52,44,18,42,18,15,15,15,25,3,4,20,39,14,20,35,53,30,14,5,0,0,0,0,0,0,4,20,30,51,30,41,41,45,51,41,43,12,37,15,15,15,23,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,11,15,15,15,37,5,0,0,7,13,27,48,51,46,44,33,27,44,30,27,11,0,2,14,41,41,41,41,53,49,41,41,41,41,15,15,
37,12,0,0,6,15,31,51,48,35,39,40,35,27,24,0,0,0,0,25,35,53,47,40,40,48,50,27,11,0,8,16,38,51,48,27,39,41,50,49,35,49,45,33,25,3,0,0,26,29,46,51,31,39,40,30,51,45,29,26,0,0,0,0,25,35,53,47,40,40,48,50,27,11,10,12,3,0,0,0,8,16,38,51,31,15,16,38,51,31,15,10,26,4,0,0,0,0,13,35,49,33,16,31,46,28,37,44,46,29,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,37,44,46,29,10,0,0,0,0,0,0,0,0,5,20,36,53,49,36,27,27,44,30,20,4,0,8,20,31,31,31,27,
30,46,44,28,46,30,46,41,32,17,0,19,37,36,51,47,44,44,30,52,50,27,13,20,32,32,14,37,25,8,0,0,8,16,38,51,31,35,35,38,51,31,15,1,8,16,38,51,31,35,35,38,51,31,15,1,0,0,0,8,16,38,51,45,50,35,36,49,49,52,44,18,9,0,0,0,0,0,17,39,29,37,24,23,13,29,30,51,47,29,17,0,0,22,15,15,15,37,10,37,15,15,15,13,18,15,15,15,13,19,0,1,25,43,43,43,37,35,48,53,41,20,21,0,0,0,19,22,22,43,27,45,49,44,20,21,0,0,3,25,15,15,15,25,25,15,15,15,13,1,3,25,15,15,15,13,25,15,15,15,
13,1,0,12,14,30,51,47,39,25,11,32,38,51,47,39,10,0,0,5,37,15,15,15,24,15,15,15,15,21,0,3,25,15,15,15,13,13,24,13,13,15,15,15,25,7,19,37,40,35,53,47,40,40,40,24,3,0,0,0,4,37,35,50,48,52,36,14,21,0,0,0,0,0,8,21,22,22,29,47,51,36,37,17,39,31,31,44,18,9,0,11,20,25,22,22,29,45,52,41,43,5,14,31,31,31,16,8,0,26,20,35,35,38,48,44,37,42,0,0,0,0,0,0,4,20,30,51,30,14,29,45,51,41,43,17,39,31,31,44,18,9,19,37,36,51,47,28,22,22,32,46,51,31,15,1,43,41,31,31,
29,21,0,0,2,14,38,51,45,33,43,23,11,18,34,27,11,0,19,25,43,43,43,35,53,49,33,43,43,44,31,31,39,10,0,0,6,16,31,51,30,14,10,22,13,20,22,0,0,0,0,13,35,53,47,28,39,48,50,27,11,0,8,16,38,51,38,14,23,29,47,51,50,50,44,15,5,0,0,0,23,33,49,50,27,25,18,44,52,45,29,26,0,0,0,0,13,35,53,47,28,39,48,50,27,39,39,32,13,4,0,0,8,16,38,51,31,35,35,38,51,31,39,39,29,15,26,0,0,9,13,44,48,40,37,36,45,29,39,47,35,18,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,
39,47,35,43,1,0,0,0,0,0,0,0,0,22,39,45,52,36,14,14,14,15,40,28,25,9,8,20,38,51,31,45,48,52,49,35,49,48,52,50,35,18,7,19,37,36,51,47,28,18,33,49,53,35,27,38,46,46,30,41,39,17,0,0,8,16,38,51,50,49,49,50,51,31,15,1,8,16,38,51,50,49,49,50,51,31,15,1,0,0,0,8,16,38,51,31,47,38,47,38,46,52,44,18,9,0,0,0,0,5,25,13,13,13,13,13,13,18,35,53,50,27,24,0,0,13,27,31,31,35,15,29,31,31,27,13,40,31,31,44,16,4,0,0,9,7,3,13,33,47,52,31,28,22,0,0,0,0,0,3,
13,40,46,53,36,14,17,0,0,0,8,16,31,31,31,15,32,31,31,31,14,5,8,16,31,31,31,15,32,31,31,31,14,5,0,10,39,45,51,36,39,39,39,39,35,53,50,27,24,0,0,21,29,31,31,35,25,33,31,31,33,22,0,8,16,31,31,31,15,1,0,8,16,31,31,31,15,1,1,37,36,49,51,53,49,49,27,24,0,0,0,0,0,26,14,38,51,45,39,11,0,0,0,0,7,22,15,28,39,40,40,47,51,36,37,17,39,48,52,44,18,9,0,7,17,14,34,28,35,49,48,27,24,5,14,30,51,38,16,8,0,4,20,30,50,53,36,40,13,3,0,0,0,0,0,0,5,14,30,51,
30,20,29,45,51,41,43,17,39,48,52,44,18,9,19,37,36,51,47,40,40,40,40,46,51,31,15,1,43,41,51,45,29,21,0,0,10,39,45,51,31,16,5,2,14,27,27,27,15,42,0,0,7,7,25,35,53,49,33,22,13,44,52,48,39,10,0,0,7,18,35,49,45,27,28,34,20,12,0,0,0,0,9,13,44,52,46,34,39,48,50,27,11,0,8,16,38,51,31,15,12,32,46,54,54,30,28,10,0,0,0,0,11,27,50,49,33,22,25,35,53,45,29,26,0,0,0,9,13,44,52,46,34,39,48,52,48,48,48,30,40,25,9,0,8,16,38,51,50,49,49,50,54,49,48,48,47,44,16,5,
0,0,24,27,50,27,32,30,30,32,41,30,34,26,26,12,9,0,19,12,17,21,6,0,0,0,0,0,0,0,5,16,41,30,34,26,0,0,0,0,0,0,0,0,0,25,35,53,49,33,27,30,31,44,47,41,43,7,8,20,38,51,48,27,39,46,51,49,35,39,30,51,31,16,8,19,37,36,51,47,28,13,33,49,50,27,45,50,46,30,47,45,39,17,0,0,8,16,38,51,31,33,33,38,51,31,15,1,8,16,38,51,31,33,33,38,51,31,15,1,1,0,0,8,16,38,51,31,36,53,50,35,46,52,44,18,1,19,0,0,0,22,33,35,35,35,35,35,35,35,35,50,53,35,13,0,0,2,37,27,
45,48,35,29,45,53,27,39,30,50,41,14,10,0,0,0,0,0,4,14,30,51,46,33,29,37,21,0,0,0,0,8,20,38,51,31,27,39,16,17,0,0,8,16,38,51,31,15,35,50,51,30,14,5,8,16,38,51,31,15,35,50,51,30,14,5,0,22,33,49,51,48,48,48,48,48,48,51,53,35,13,0,0,21,29,45,53,35,25,33,49,49,33,22,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,1,37,35,35,53,47,35,35,27,24,0,0,0,0,8,43,35,49,49,52,36,14,21,0,0,0,22,28,41,46,48,49,49,53,51,36,37,17,39,48,52,44,18,9,0,0,22,40,46,47,
49,38,27,16,12,5,14,30,51,38,16,8,0,4,20,33,27,36,53,30,32,12,1,9,0,0,0,0,12,32,46,51,31,16,29,45,51,41,43,17,39,48,52,44,18,9,19,37,36,51,53,49,49,49,49,50,51,31,15,1,43,41,51,45,29,21,0,0,22,33,49,52,44,18,9,2,32,46,50,31,15,42,0,0,0,0,25,35,53,49,33,22,13,44,52,48,39,10,0,0,0,26,14,35,46,48,47,46,29,21,0,0,0,9,12,43,41,52,30,14,39,48,50,27,11,0,8,16,38,51,38,20,21,28,46,51,53,50,44,15,5,0,0,0,23,33,49,50,27,11,13,35,53,45,29,26,0,0,9,12,
43,41,52,30,14,39,48,50,41,36,46,51,31,20,5,0,8,16,38,51,31,33,33,38,51,31,41,31,53,45,40,23,0,0,26,34,31,47,30,45,35,40,45,27,16,29,29,14,24,2,18,32,39,28,37,21,0,0,0,0,0,0,11,33,45,27,13,3,0,0,0,0,0,0,0,0,9,18,44,52,45,29,27,50,38,48,46,41,43,7,8,20,38,51,30,14,16,31,51,30,32,37,36,51,30,14,4,19,37,36,51,47,44,44,30,53,38,35,53,31,32,14,29,27,39,17,0,0,8,16,38,51,31,15,16,38,51,31,15,1,8,16,38,51,31,15,16,38,51,31,15,15,37,21,0,8,16,38,51,31,
33,48,46,29,46,52,44,15,15,43,5,0,0,22,40,49,53,53,53,53,53,53,53,51,53,44,13,0,0,0,12,37,35,48,45,27,45,53,27,30,53,36,32,22,0,0,0,0,0,0,4,14,30,49,50,49,47,35,15,5,0,0,0,8,20,38,48,49,50,45,44,20,12,0,8,16,38,51,31,28,30,51,54,30,14,5,8,16,38,51,31,28,30,51,54,30,14,5,0,22,33,49,52,38,38,38,38,38,38,50,53,44,13,0,0,21,29,45,53,35,25,33,49,49,33,22,0,8,16,38,51,31,15,25,11,21,16,38,51,31,15,1,0,22,13,35,53,47,28,34,34,24,0,0,0,9,11,39,47,50,
35,45,49,35,43,8,0,1,37,41,53,52,45,47,47,49,51,36,37,17,39,48,52,44,18,9,0,0,22,40,46,46,48,50,30,40,11,5,14,30,51,38,16,8,0,0,26,23,11,39,48,45,29,15,15,11,0,0,0,0,21,29,47,51,36,37,29,45,51,41,43,17,39,48,52,44,18,9,19,37,36,51,49,47,47,47,47,49,51,31,15,1,43,41,51,45,29,21,0,0,22,33,49,52,44,13,9,2,32,46,51,31,15,42,0,0,0,0,25,35,53,49,33,22,13,44,52,48,39,10,0,0,7,13,27,46,50,48,46,46,29,21,0,0,0,26,14,33,46,51,36,15,39,48,50,27,11,0,8,16,
38,51,47,40,20,27,49,50,44,50,45,33,25,7,0,0,10,39,45,52,41,14,28,38,51,45,29,26,0,0,26,14,33,46,51,36,15,39,48,50,27,37,35,53,47,29,21,0,8,16,38,51,31,15,16,38,51,31,15,29,45,53,35,25,0,0,7,11,28,27,35,40,20,36,31,14,41,47,45,38,39,18,27,30,45,46,35,37,4,0,0,0,0,12,20,36,38,14,21,0,0,0,0,0,0,0,0,0,7,18,44,52,45,29,27,50,48,27,32,40,18,7,8,20,38,51,31,15,15,31,51,38,20,37,36,51,30,14,4,19,37,36,51,51,52,52,53,44,29,27,50,47,41,35,40,32,18,5,0,0,
8,16,38,51,31,15,16,38,51,31,15,1,8,16,38,51,31,15,16,38,51,30,31,31,33,22,0,8,16,38,51,31,14,33,33,32,46,52,31,31,31,29,21,0,0,10,39,45,51,41,44,44,44,44,44,53,53,35,25,0,0,0,0,21,16,35,49,47,49,53,30,52,31,28,23,9,0,0,0,0,7,19,4,20,40,33,27,38,52,45,40,22,0,0,0,6,15,39,39,33,36,53,48,33,11,0,8,16,38,51,31,44,50,50,54,30,14,5,8,16,38,51,31,44,50,50,54,30,14,5,0,22,39,48,52,41,20,20,20,20,27,50,53,35,25,0,0,21,28,47,52,41,29,33,49,49,33,22,0,8,
16,38,51,31,35,35,33,32,16,38,51,31,15,1,0,0,13,35,53,49,46,46,44,13,9,0,0,21,14,31,52,31,32,44,50,46,39,11,0,2,14,30,51,46,40,28,29,47,51,36,37,10,39,48,52,44,18,9,0,0,17,14,32,28,27,45,52,36,15,2,14,30,51,38,16,8,0,0,0,0,10,39,48,48,31,31,41,43,7,0,0,0,22,40,48,53,44,13,29,47,51,36,37,10,39,48,52,44,18,9,19,37,36,51,47,28,28,28,28,46,51,31,15,1,37,41,51,45,29,21,0,0,22,39,48,51,41,37,1,12,32,46,51,31,15,42,0,0,0,0,25,35,53,49,33,22,13,44,52,48,
39,10,0,0,2,14,30,51,46,33,32,32,20,12,9,0,0,22,40,48,51,45,33,25,39,48,50,27,11,0,8,16,38,51,30,47,38,45,51,30,29,31,52,38,28,10,0,0,4,16,36,53,49,30,30,31,53,45,29,26,0,0,22,40,48,51,45,33,25,39,48,50,27,24,27,53,45,29,26,0,8,16,38,51,31,15,16,38,51,31,15,28,47,52,44,13,0,0,0,7,26,24,25,25,33,48,33,27,49,35,33,45,36,34,30,30,39,36,45,39,22,0,0,0,9,25,33,48,33,25,9,0,0,0,0,0,0,0,0,0,0,13,35,53,48,39,27,50,30,14,12,22,12,0,8,20,38,51,31,15,
15,31,51,38,16,37,36,51,30,14,4,19,37,36,51,47,44,36,49,50,36,29,38,49,52,53,48,38,39,23,0,0,8,16,38,46,31,15,16,38,51,31,15,1,8,16,38,46,31,15,16,38,46,46,53,49,33,22,0,8,16,38,46,31,15,23,23,32,46,46,46,52,45,29,21,0,0,12,32,30,51,30,32,13,13,16,36,52,48,40,22,0,0,0,0,21,20,36,53,53,54,54,52,52,46,39,23,0,0,0,0,3,25,37,22,26,22,22,25,33,49,53,35,25,0,0,12,25,10,10,10,11,40,48,52,44,13,9,8,16,38,51,31,46,48,36,51,30,14,5,8,16,38,51,31,46,48,36,51,30,
14,5,0,21,28,46,51,30,32,10,21,16,36,52,48,40,22,0,0,5,20,31,53,50,45,45,53,49,33,22,0,8,16,38,51,50,49,50,49,31,28,38,51,31,15,1,0,0,13,35,46,46,48,52,44,13,9,0,1,43,35,46,46,33,24,28,30,52,31,20,5,12,34,46,51,36,15,22,39,45,51,31,16,23,33,49,53,44,13,0,0,0,0,2,2,12,20,30,51,30,14,21,28,46,51,36,15,1,0,0,0,0,10,39,46,46,45,51,41,43,7,0,1,21,37,44,53,49,33,11,28,46,51,31,15,23,33,49,53,35,13,0,19,37,36,51,47,28,21,21,14,30,51,38,20,12,15,31,51,46,
28,12,0,0,21,34,46,54,46,29,11,26,29,47,51,36,37,1,0,0,0,0,24,27,50,50,27,24,37,41,52,45,29,17,0,0,21,28,47,51,36,37,12,2,10,43,25,3,0,22,40,46,31,33,37,10,39,48,50,27,11,0,8,16,38,51,31,44,45,48,30,33,18,39,46,46,44,15,4,0,0,23,29,31,48,48,31,35,53,45,29,26,0,0,22,40,46,31,33,37,10,39,46,46,27,24,27,53,45,29,26,0,8,16,38,46,31,15,16,38,46,31,15,28,47,52,44,13,0,0,0,0,0,0,21,14,31,36,20,41,45,39,20,38,46,39,48,44,18,27,50,27,24,0,0,0,21,14,38,36,
20,12,0,0,0,0,0,0,0,0,0,0,0,23,33,49,53,44,27,50,31,16,32,37,8,0,8,20,38,51,31,15,16,31,51,38,16,37,36,51,30,14,4,19,37,36,51,47,28,20,41,52,48,33,34,27,44,36,45,52,44,43,7,0,1,43,34,34,34,18,20,38,51,36,15,1,1,43,34,34,34,18,43,34,34,41,53,36,34,10,0,1,43,34,34,34,18,3,5,15,34,34,31,49,44,20,12,0,0,1,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,0,6,43,27,49,45,41,50,52,30,38,52,36,14,21,0,0,0,8,20,44,39,20,43,13,37,27,50,53,35,25,0,0,24,27,
29,16,43,15,27,49,53,35,13,0,8,16,38,51,48,53,36,36,51,30,14,5,8,16,38,51,48,53,36,36,51,30,14,5,0,42,37,44,53,50,44,29,28,27,45,51,31,14,12,0,0,0,11,29,44,41,41,41,49,49,33,22,0,8,16,38,51,31,33,27,47,50,27,38,51,31,15,1,0,0,11,34,34,28,46,52,44,13,9,0,1,43,34,34,34,37,11,37,41,51,30,14,2,2,14,30,51,46,40,29,41,49,51,47,33,28,41,53,49,33,11,0,0,0,0,0,0,42,15,36,52,45,27,34,35,49,53,44,18,7,0,0,0,0,26,14,34,34,30,51,41,43,7,9,25,28,27,47,51,46,
28,21,20,31,51,47,33,28,41,53,48,33,23,0,19,37,36,51,47,28,12,42,15,36,51,45,33,34,33,47,51,31,20,4,0,0,1,37,44,50,53,31,39,32,35,50,53,35,13,9,0,0,0,0,22,39,45,52,36,29,39,46,51,30,14,12,0,0,12,32,46,51,30,39,20,14,29,35,14,4,0,26,14,32,16,25,22,24,40,48,49,33,23,0,8,16,38,51,31,15,39,40,28,18,5,43,34,34,34,37,8,0,0,19,11,14,39,39,14,35,53,45,29,26,0,0,26,14,32,16,25,5,21,14,34,34,32,25,35,53,45,29,26,0,1,43,34,34,34,18,43,34,34,34,18,29,47,53,35,13,
0,0,0,0,0,3,13,27,45,40,37,41,45,39,20,38,46,40,48,44,18,27,50,27,24,0,0,3,13,27,45,40,11,0,0,0,0,0,0,0,0,0,0,0,0,12,32,30,51,45,27,50,31,33,31,20,4,0,8,20,38,51,31,15,39,45,51,38,16,37,36,51,30,14,4,19,37,36,51,47,28,23,40,45,52,36,29,20,37,16,31,51,36,37,3,0,0,4,12,12,25,27,35,45,53,44,18,7,0,4,12,12,12,8,4,13,27,48,31,28,23,7,0,0,4,12,12,12,8,0,0,5,37,35,50,41,14,10,0,0,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,11,39,
46,50,44,29,45,53,27,33,45,49,35,43,8,0,0,8,20,38,45,38,41,44,41,47,51,45,40,22,0,0,24,27,47,31,41,36,47,51,45,40,23,0,8,16,38,51,54,45,40,36,51,30,14,5,8,16,38,51,54,45,40,36,51,30,14,5,0,0,22,29,38,53,50,47,46,49,52,46,40,24,9,0,0,0,19,22,18,43,43,33,49,49,33,22,0,8,16,38,51,31,15,14,30,53,35,38,51,31,15,1,0,0,19,37,27,44,49,49,27,24,0,0,0,4,12,12,12,23,33,35,46,51,36,15,8,1,37,41,53,52,48,47,48,27,48,51,45,46,50,52,31,14,12,0,0,0,0,0,0,0,
24,40,47,51,48,46,49,51,46,29,22,0,0,0,0,0,0,12,12,14,30,51,41,43,7,7,43,41,49,51,49,44,37,42,25,27,48,51,45,46,50,52,31,14,12,0,19,37,36,51,47,28,12,0,24,33,45,51,48,46,45,51,48,27,25,9,0,0,0,10,28,31,53,52,45,46,48,51,30,29,10,0,0,0,0,0,5,20,36,53,53,47,45,52,49,35,18,19,0,0,42,37,44,50,52,47,30,30,47,30,14,4,0,0,12,12,4,4,20,27,41,53,45,39,10,0,8,16,38,51,31,15,10,22,21,1,0,4,12,12,12,4,0,0,0,0,9,12,22,22,25,35,53,45,29,26,0,0,0,12,
12,4,0,0,0,12,12,26,29,27,38,51,30,14,12,0,0,4,12,12,12,8,4,12,12,37,27,44,49,49,33,11,0,0,0,0,0,26,34,30,41,15,25,35,50,35,40,47,36,28,46,38,39,36,45,39,22,0,0,17,34,30,41,15,5,0,0,0,0,0,0,0,0,0,0,0,0,7,25,33,47,51,49,51,45,48,30,20,4,0,8,20,38,46,31,15,44,46,46,38,16,37,36,46,30,14,4,19,37,36,51,47,28,12,20,36,51,53,47,31,36,36,48,50,35,13,9,0,0,0,0,0,25,35,53,50,30,29,22,0,0,0,0,0,0,0,22,29,30,38,29,11,7,0,0,0,0,0,0,0,
0,0,7,25,40,47,36,14,22,0,0,0,0,0,19,24,29,41,46,48,48,47,31,40,18,4,0,0,0,21,14,36,52,31,32,29,45,53,27,37,44,50,46,39,11,0,0,8,20,36,47,49,53,52,53,50,47,35,15,5,0,0,24,27,45,50,53,53,49,46,27,15,5,0,8,16,38,51,53,41,15,36,51,30,14,5,8,16,38,51,53,41,15,36,51,30,14,5,0,0,3,24,29,41,47,48,48,45,31,40,18,4,0,0,0,0,0,0,9,7,22,33,49,49,33,22,0,8,16,38,51,30,31,38,50,48,33,38,51,31,15,1,0,0,3,37,36,53,50,31,32,21,0,0,0,0,0,0,0,23,
33,49,50,46,33,24,0,0,22,28,36,45,49,47,44,20,35,30,48,48,45,36,29,24,9,0,0,0,0,0,0,0,4,43,33,38,45,49,45,38,33,18,42,0,0,0,0,0,0,0,5,14,30,50,41,43,7,7,43,41,45,30,35,20,26,0,12,16,35,30,48,48,45,36,29,11,9,0,19,37,36,46,46,28,12,0,5,37,27,30,45,49,48,30,35,16,12,0,0,0,0,7,11,28,44,46,48,48,45,31,40,13,1,0,0,0,0,0,0,23,28,41,47,48,48,46,44,20,21,0,0,0,0,10,32,44,46,48,49,48,47,31,14,4,0,0,0,0,0,4,14,30,53,48,44,15,4,0,8,16,
38,50,31,15,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,35,50,45,29,26,0,0,0,0,0,0,0,0,0,0,0,26,29,45,53,45,27,18,3,0,0,0,0,0,0,0,0,0,19,37,36,53,49,36,14,21,0,0,0,0,19,18,35,47,39,23,26,32,36,48,48,30,39,43,27,46,45,45,44,15,5,0,1,43,35,47,39,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,43,33,31,45,48,48,46,44,20,4,0,1,43,34,34,34,29,46,39,34,34,43,13,34,34,34,37,8,19,37,36,46,46,28,12,24,33,46,46,46,45,48,48,47,41,32,10,0,0,
0,0,0,0,25,35,35,27,29,25,1,0,0,0,0,0,0,9,13,33,33,29,24,3,0,0,0,0,0,0,0,0,0,0,1,15,33,33,32,22,0,0,0,0,0,0,0,19,22,15,28,40,40,29,20,24,4,0,0,0,1,43,35,46,46,40,24,29,46,46,27,24,32,31,46,31,14,21,0,9,11,15,29,27,35,44,35,27,29,37,21,0,0,0,22,20,39,27,35,27,33,28,43,12,0,0,8,16,38,46,46,39,37,36,46,30,14,5,8,16,38,46,46,39,37,36,46,30,14,5,0,0,0,19,22,15,29,40,40,29,20,24,4,0,0,0,0,0,0,0,0,0,22,33,46,46,33,22,0,8,
16,38,46,46,46,46,30,35,16,38,46,31,15,1,0,0,3,37,35,35,27,32,11,9,0,0,0,0,0,0,0,23,33,35,27,39,18,4,0,0,3,23,20,39,40,29,37,26,43,34,39,40,29,20,11,1,0,0,0,0,0,0,0,0,0,5,25,14,39,40,39,14,25,4,0,0,0,0,0,0,0,0,5,14,33,33,33,43,7,7,18,40,29,14,18,21,0,0,0,21,43,34,39,40,29,20,11,19,0,0,7,13,34,34,34,20,2,0,0,12,13,32,39,40,39,32,18,21,0,0,0,0,0,0,3,22,37,28,39,40,39,14,25,4,0,0,0,0,0,0,0,3,22,16,29,40,40,28,37,26,
0,0,0,0,0,9,10,37,34,39,40,39,28,20,25,3,0,0,0,0,0,4,14,35,35,33,20,10,0,0,8,16,33,33,33,37,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,33,33,33,29,26,0,0,0,0,0,0,0,0,0,0,0,26,29,35,35,40,37,12,0,0,0,0,0,0,0,0,0,0,19,37,35,35,27,32,23,9,0,0,0,0,4,16,40,40,37,4,9,23,14,39,40,34,13,12,43,29,40,39,15,26,0,0,4,16,40,40,37,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,24,20,29,40,39,34,37,10,0,0,0,4,12,12,25,35,
35,20,12,12,4,6,12,12,12,4,0,7,13,34,34,34,20,2,12,20,34,34,34,29,40,40,29,16,22,9,0,0,0,0,0,0,21,25,25,24,10,19,0,0,0,0,0,0,0,0,12,23,23,22,19,0,0,0,0,0,0,0,0,0,0,0,0,26,23,23,10,9,0,0,0,0,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,1,43,34,34,34,37,12,20,34,34,34,23,24,34,34,34,14,12,0,0,0,4,17,11,13,13,13,24,17,8,0,0,0,0,0,5,10,11,25,24,23,21,1,0,0,0,1,43,34,34,34,37,13,34,34,34,37,8,1,43,34,34,34,37,13,34,34,34,
37,8,0,0,0,0,0,8,21,22,22,17,5,0,0,0,0,0,0,0,0,0,0,0,10,32,34,34,14,10,0,1,43,34,34,34,34,34,14,18,43,34,34,34,18,3,0,0,0,10,25,25,24,26,7,0,0,0,0,0,0,0,0,5,25,25,25,22,8,0,0,0,0,0,5,17,22,17,4,0,1,21,10,22,17,5,0,0,0,0,0,0,0,0,0,0,0,0,9,2,10,22,10,12,7,0,0,0,0,0,0,0,0,0,0,10,23,23,23,12,0,0,2,22,17,2,3,0,0,0,0,0,1,21,10,22,17,5,0,0,0,0,0,8,12,12,12,2,0,0,0,0,3,12,10,22,10,12,19,0,
0,0,0,0,0,0,0,0,6,21,22,22,17,5,9,0,0,0,0,0,0,0,0,0,0,4,26,22,22,21,42,0,0,0,0,0,0,0,0,1,12,10,22,10,21,4,9,0,0,0,0,0,0,9,22,25,25,11,12,0,0,0,0,17,23,23,23,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,23,23,23,22,19,0,0,0,0,0,0,0,0,0,0,0,42,11,25,25,22,5,0,0,0,0,0,0,0,0,0,0,0,0,10,25,25,11,21,9,0,0,0,0,0,0,21,22,22,12,0,0,0,2,10,22,21,3,0,6,26,22,10,4,0,0,0,0,21,22,22,12,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,26,22,10,12,1,0,0,0,0,0,0,0,21,25,25,22,0,0,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,2,12,12,12,17,22,22,26,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,4,0,2,12,12,12,3,19,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,4,12,12,12,4,6,12,12,12,4,0,0,4,12,12,12,4,6,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,12,12,12,12,9,0,0,4,12,12,12,12,12,5,3,4,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,42,0,0,0,0,0,0,0,0,0,0,9,4,5,42,0,0,0,0,0,0,0,0,0,9,42,4,5,4,
7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,4,1,0,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,0,0,0,7,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,19,19,19,0,0,0,0,0,0,0,0,3,4,5,4,19,0,0,0,0,0,0,0,0,0,0,3,19,19,19,9,0,0,0,0,0,3,19,19,19,19,19,19,19,19,7,0,0,0,0,42,42,42,1,0,0,0,0,9,42,4,4,1,0,0,0,0,0,0,0,0,0,3,19,19,19,3,0,0,0,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,19,19,19,19,
3,7,0,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,19,19,19,7,0,0,0,0,19,19,19,7,0,0,3,19,19,19,3,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,19,19,19,19,9,0,0,0,19,19,19,19,7,0,0,0,19,19,19,19,0,0,9,19,19,19,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,11,37,37,37,37,37,37,13,10,4,22,22,22,10,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,11,
37,20,14,15,25,12,0,0,0,0,0,0,0,4,25,16,14,15,23,3,0,0,0,0,0,8,23,18,15,20,14,20,18,17,9,0,0,0,0,0,5,10,10,10,12,0,0,0,0,0,0,7,10,43,20,20,37,23,37,37,37,18,4,0,9,10,22,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,10,10,7,0,0,21,23,23,23,23,23,23,23,23,23,23,23,22,8,0,9,10,23,23,23,23,23,23,23,23,23,23,23,10,7,0,0,0,0,0,0,0,5,22,2,0,0,0,0,0,0,0,0,1,22,23,23,23,23,23,23,23,23,23,23,23,26,0,
0,1,22,23,23,23,23,23,23,23,23,23,23,23,26,0,0,0,0,0,0,0,0,8,25,25,1,0,0,0,0,0,0,0,0,0,0,0,0,8,25,25,1,0,0,0,0,0,0,0,0,0,1,26,22,2,7,11,15,43,21,0,8,13,37,37,37,22,0,0,0,0,0,0,4,25,13,13,22,13,13,23,9,0,0,0,1,17,11,18,37,37,37,23,0,0,0,0,0,21,24,43,20,14,20,37,23,6,0,0,0,0,0,0,0,2,43,37,37,37,25,3,0,0,0,4,18,37,37,37,37,37,37,37,37,25,19,0,0,10,15,15,15,37,12,0,9,17,25,15,14,14,37,11,8,0,0,0,0,0,
0,4,18,37,37,37,43,12,0,0,0,0,0,0,11,37,37,37,37,37,43,25,10,19,0,0,0,11,37,37,37,37,37,18,24,26,19,0,0,0,0,0,11,37,37,37,37,37,37,37,37,23,0,0,11,37,37,37,37,37,37,37,37,23,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,11,37,37,37,18,4,0,0,11,37,37,37,18,4,5,18,37,37,37,18,4,0,0,11,37,37,37,18,4,0,0,0,0,0,0,11,37,37,37,37,24,7,0,10,37,37,37,37,13,8,0,11,37,37,37,37,21,3,25,37,37,37,11,9,0,11,37,37,37,37,37,37,13,10,1,0,0,0,0,0,
19,37,36,36,36,36,36,41,35,29,13,40,40,40,28,26,0,0,0,0,0,0,0,0,0,0,0,0,0,12,37,40,41,30,30,36,27,20,12,0,0,0,0,0,7,25,40,31,30,36,29,23,0,0,0,0,0,22,39,44,36,30,30,31,35,32,22,0,0,0,0,7,18,39,39,39,16,4,0,0,0,0,9,23,34,44,38,30,41,39,35,36,36,29,21,0,2,14,29,43,17,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,26,18,29,32,12,0,19,37,33,33,33,33,33,33,33,33,33,33,33,39,22,0,2,14,33,33,33,33,33,33,33,33,33,33,33,32,12,0,
0,0,0,0,0,9,13,33,13,3,0,0,0,0,0,0,0,10,39,33,33,33,33,33,33,33,33,33,33,33,15,8,0,10,39,33,33,33,33,33,33,33,33,33,33,33,15,8,0,0,0,0,0,0,0,17,29,28,21,0,0,0,0,0,0,0,0,0,0,0,0,17,29,28,21,0,0,0,0,0,0,0,0,0,24,29,40,43,22,28,36,35,18,9,12,34,36,36,44,13,9,0,0,0,0,0,10,39,35,27,15,35,35,20,4,0,0,0,23,29,33,44,36,36,41,43,3,0,0,0,4,16,33,41,38,30,38,41,39,13,6,0,0,0,0,0,7,25,33,36,36,36,20,4,0,0,0,21,28,36,
36,36,36,36,36,36,36,14,4,0,0,24,27,31,31,39,17,0,12,32,35,36,30,30,36,40,13,1,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,19,37,36,36,36,36,36,41,35,29,25,19,0,19,37,36,36,36,36,41,44,27,28,13,5,0,0,0,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,36,36,36,36,41,43,7,19,37,36,36,36,28,12,12,32,36,36,36,15,1,19,37,36,36,36,28,12,0,19,37,36,36,36,28,12,18,33,36,36,36,28,21,0,19,37,36,36,36,28,12,0,0,0,0,0,19,37,36,36,36,36,14,2,3,18,44,36,36,36,
34,12,19,37,36,36,36,35,13,4,20,36,36,36,15,1,19,37,36,36,36,36,36,41,35,29,13,6,0,0,0,0,19,37,36,51,51,52,52,51,53,30,39,27,49,45,29,26,6,2,12,2,8,7,0,0,0,0,0,0,8,43,27,47,52,50,48,49,45,29,21,0,0,0,0,0,5,14,38,47,41,50,41,37,1,0,0,0,0,22,33,49,53,50,50,52,50,36,20,5,0,0,0,3,37,36,48,30,14,4,0,0,0,0,17,32,31,50,51,50,50,30,35,53,45,29,21,0,12,32,46,35,34,13,12,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,25,32,35,46,28,12,0,
19,37,36,49,49,49,49,49,49,49,49,49,49,33,23,0,8,37,41,49,49,49,49,49,49,49,49,49,36,15,4,0,0,0,0,0,0,12,14,38,32,21,0,0,0,0,0,0,0,10,39,36,41,41,41,41,41,41,41,41,41,38,16,8,0,10,39,36,41,41,41,41,41,41,41,41,41,38,16,8,0,0,0,0,0,0,0,24,27,33,11,0,0,0,0,0,0,0,0,0,0,0,0,24,27,33,11,0,0,0,0,0,0,0,0,19,37,36,30,32,14,36,47,33,25,0,12,34,46,52,44,13,9,0,0,0,0,0,24,27,48,39,32,46,41,43,3,0,0,0,24,27,49,53,53,51,41,43,3,0,
0,0,4,20,30,53,49,48,53,52,46,39,22,0,0,0,0,0,26,32,38,51,51,30,20,4,0,0,0,21,28,47,52,52,52,52,51,51,30,14,4,0,0,24,27,53,48,39,17,0,12,28,47,50,48,50,52,30,29,10,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,52,50,50,52,50,30,29,10,0,19,37,36,51,51,52,51,52,50,30,33,43,4,0,0,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,51,52,52,52,52,41,43,7,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,28,13,40,46,
52,30,39,24,3,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,51,48,40,23,12,32,30,51,51,46,34,12,19,37,36,51,51,30,32,21,20,30,51,31,15,1,19,37,36,51,51,52,52,51,53,30,39,11,0,0,0,0,19,37,36,51,47,44,44,36,50,52,36,27,53,45,29,15,15,14,32,14,16,18,10,0,0,0,0,1,22,39,47,51,30,27,39,27,38,29,21,0,0,0,0,0,21,28,47,31,29,45,30,20,4,0,0,1,26,22,33,38,35,33,27,41,50,45,39,22,5,0,0,4,37,36,51,30,14,5,0,0,0,19,43,35,50,53,31,27,27,31,30,52,45,29,21,0,
12,32,46,50,30,27,14,24,2,0,0,0,0,0,0,0,0,0,0,0,0,0,5,24,14,27,38,50,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,23,39,45,54,54,54,54,54,54,54,48,33,11,0,0,0,0,0,0,0,24,33,48,27,25,9,0,0,0,0,0,0,10,39,41,43,43,43,43,43,43,43,43,28,38,16,8,0,10,39,41,28,39,39,39,39,39,39,39,28,38,16,8,0,0,0,0,0,0,1,37,36,44,43,3,0,0,0,0,0,0,0,0,0,0,1,37,36,44,43,3,0,0,0,0,0,0,0,0,25,27,48,33,44,49,35,37,5,0,12,34,
46,52,44,13,9,0,0,4,25,13,43,41,46,32,39,48,27,13,24,6,0,0,24,27,31,44,46,51,41,43,3,0,0,0,4,20,30,44,33,39,41,53,53,44,18,7,0,0,0,42,43,35,50,30,51,30,20,4,0,0,0,21,28,44,44,44,44,44,46,52,36,15,8,0,0,24,27,49,48,39,17,0,12,28,38,27,39,35,49,50,27,24,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,19,37,36,51,47,27,27,36,53,50,35,25,0,19,37,36,51,47,41,41,38,49,51,47,40,11,0,0,19,37,36,51,47,41,41,41,41,41,43,7,19,37,36,51,47,41,41,41,41,41,43,
7,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,28,39,30,51,30,39,25,1,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,52,52,41,37,11,33,48,53,54,46,34,12,19,37,36,51,54,49,27,13,20,30,51,31,15,1,19,37,36,51,47,44,44,36,50,52,36,15,8,0,0,0,19,37,36,51,47,28,13,32,30,54,46,31,53,48,31,31,31,30,46,30,31,44,43,3,0,0,7,13,32,44,53,48,33,32,32,32,28,29,21,0,0,0,0,8,21,28,46,38,29,45,38,20,4,0,0,23,29,29,29,29,29,29,29,39,
45,50,33,29,37,1,4,37,14,36,51,30,14,20,12,0,0,12,32,30,51,46,29,24,24,29,46,54,45,29,21,0,12,32,46,54,51,49,31,33,20,23,4,0,0,0,0,0,0,0,0,0,8,22,15,40,31,48,51,54,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,5,16,36,52,54,54,54,54,54,51,31,20,2,0,0,0,0,0,0,5,20,36,52,31,20,2,0,0,0,0,0,0,10,39,41,43,7,7,7,7,7,7,12,28,38,16,8,0,10,39,41,39,48,48,48,48,48,48,46,28,38,16,8,0,0,4,17,17,17,17,32,46,30,14,17,17,17,17,8,
0,0,4,17,17,17,17,32,35,44,14,17,17,17,17,8,0,0,0,0,10,29,47,36,45,36,14,21,0,0,12,34,46,52,44,13,9,0,0,22,40,35,35,38,31,35,35,45,35,35,29,21,0,0,22,14,15,14,46,51,41,43,3,0,0,0,4,20,40,37,23,22,29,47,51,36,37,19,0,0,0,22,39,46,38,36,51,30,20,4,0,0,0,42,25,13,13,13,13,27,50,48,33,11,0,0,0,24,40,33,33,29,17,0,12,14,34,13,10,39,45,50,27,24,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,47,28,24,39,48,52,44,13,0,19,37,36,51,47,28,
37,14,35,49,52,36,16,4,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,43,43,43,43,22,0,19,37,36,51,47,28,22,22,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,29,30,52,46,33,13,8,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,47,46,28,15,36,50,44,52,46,34,12,19,37,36,51,45,49,38,14,20,30,51,31,15,1,19,37,36,51,47,28,13,32,30,54,30,14,2,0,0,0,19,37,36,51,47,28,22,14,30,51,50,49,51,53,49,50,51,45,30,46,48,41,43,3,0,0,12,14,38,47,51,48,
46,46,46,36,15,23,19,0,0,0,17,15,32,20,31,48,27,49,36,37,1,0,0,24,27,45,45,45,45,45,45,45,53,52,45,31,15,42,2,14,46,46,51,45,46,29,26,0,0,10,39,45,51,36,37,42,1,37,36,51,45,29,21,0,12,32,46,54,54,54,51,45,36,39,37,22,1,0,0,0,0,0,19,10,37,39,41,45,52,54,54,54,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,24,33,48,54,54,54,54,54,49,27,25,0,0,0,0,0,0,0,23,39,45,54,48,33,11,0,0,0,0,0,0,10,39,41,43,7,0,0,0,0,0,12,28,38,16,8,
0,10,39,41,39,48,54,54,54,54,54,46,28,38,16,8,0,0,13,39,39,39,39,33,48,45,39,39,39,39,39,24,0,0,13,39,39,39,39,33,39,33,39,39,39,39,39,24,0,0,0,0,4,16,31,52,30,29,23,0,0,0,12,34,46,52,44,13,9,0,0,22,40,48,48,53,48,48,49,49,48,45,29,21,0,0,9,2,2,14,46,51,41,43,3,0,0,0,0,10,23,4,0,10,29,47,52,41,43,3,0,0,2,16,41,48,33,36,51,30,20,4,0,0,0,0,0,9,9,2,20,31,51,38,14,12,0,0,0,4,22,22,22,22,3,0,9,26,26,17,20,44,53,45,39,22,0,0,0,9,
13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,47,39,39,35,50,49,27,24,0,19,37,36,51,47,28,12,21,14,30,51,47,28,21,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,39,23,0,19,37,36,51,47,40,40,40,40,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,31,52,47,33,18,4,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,36,49,27,29,47,46,44,52,46,34,12,19,37,36,51,30,30,49,27,20,30,51,31,15,1,19,37,36,51,47,28,22,14,30,51,46,32,12,0,0,0,
19,37,36,51,47,40,40,35,48,51,31,27,53,45,40,46,49,33,14,32,40,44,43,3,0,0,22,33,44,46,51,31,44,44,44,33,11,0,0,0,0,21,20,44,46,36,33,47,49,49,27,25,0,0,0,24,27,27,27,35,48,54,54,54,52,30,27,27,15,42,2,14,46,45,51,48,45,29,26,0,0,23,33,49,52,44,13,9,0,13,44,52,45,29,21,0,12,32,46,54,54,54,54,54,52,47,44,29,43,26,0,0,0,21,18,29,44,47,53,54,54,54,54,54,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,12,14,38,51,54,54,54,51,30,32,21,0,0,0,
0,0,0,8,37,41,52,54,52,36,15,4,0,0,0,0,0,10,39,41,43,7,0,0,0,0,0,12,28,38,16,8,0,10,39,41,39,48,54,54,54,54,54,46,28,38,16,8,0,0,11,29,36,45,45,45,52,53,45,45,45,41,28,23,0,0,11,29,36,33,33,33,14,32,33,33,33,41,28,23,0,0,0,0,12,20,41,52,31,20,5,0,0,0,2,14,30,53,35,25,0,0,0,10,29,39,33,49,33,39,30,36,39,39,32,21,0,0,0,0,2,14,46,51,41,43,3,0,0,0,0,0,0,0,26,20,44,53,45,40,23,0,0,7,25,33,48,41,20,36,51,30,20,4,0,0,0,0,0,0,
0,23,40,45,50,35,13,7,0,0,0,23,28,28,28,14,21,0,0,0,12,16,44,49,49,44,37,4,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,19,37,36,51,50,48,48,50,48,44,14,21,0,19,37,36,51,47,28,12,3,37,41,51,48,39,10,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,27,11,0,19,37,36,51,53,49,49,49,49,50,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,52,52,45,27,37,5,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,27,49,31,35,50,41,44,52,46,
34,12,19,37,36,51,30,27,50,31,14,30,51,31,15,1,19,37,36,51,47,40,40,35,48,51,31,20,4,0,0,0,19,37,36,51,53,49,49,50,51,45,27,27,53,45,29,30,53,31,44,27,29,15,22,0,0,0,2,43,33,46,51,31,33,33,33,15,2,0,0,0,19,18,35,49,47,50,31,38,51,49,44,37,4,0,0,23,29,29,35,49,51,54,54,49,36,39,29,29,37,1,5,20,29,36,51,30,29,32,21,0,0,23,33,49,52,44,13,0,0,13,35,53,45,29,21,0,12,32,46,54,54,54,54,54,54,54,53,46,35,14,12,0,5,20,35,30,50,54,54,54,54,54,54,54,47,28,12,0,
19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,9,25,27,49,54,54,54,50,35,13,3,0,0,0,0,0,0,10,29,47,54,54,54,45,39,22,0,0,0,0,0,10,39,41,43,7,0,0,0,0,0,12,28,38,16,8,0,10,39,41,39,48,54,54,54,54,54,46,28,38,16,8,0,0,3,23,14,44,48,51,54,54,51,45,35,20,22,7,0,0,3,23,14,44,33,43,17,10,37,33,35,20,22,7,0,0,0,6,18,27,48,45,45,40,23,0,0,0,4,20,38,50,27,11,0,0,4,18,37,37,44,47,29,29,45,35,37,37,26,9,0,0,0,0,2,14,46,51,41,43,3,0,
0,0,0,0,0,26,20,44,48,48,35,37,4,0,0,26,32,38,46,39,37,36,51,30,20,23,4,0,0,0,0,0,42,37,41,52,46,29,17,0,0,0,0,24,27,47,47,39,17,0,0,0,24,33,48,50,44,20,26,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,48,30,30,46,53,47,27,13,7,19,37,36,51,47,28,12,3,43,41,52,48,39,10,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,27,11,0,19,37,36,51,49,47,47,47,47,49,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,52,52,50,44,20,
26,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,34,30,45,38,48,33,44,52,46,34,12,19,37,36,51,30,34,30,48,33,30,51,31,15,1,19,37,36,51,53,49,49,50,51,45,27,25,9,0,0,0,19,37,36,51,48,46,46,30,36,33,37,27,53,45,29,35,45,53,53,50,47,44,15,4,0,0,21,28,30,49,54,50,49,49,35,18,12,12,0,0,42,37,27,46,39,41,50,50,38,47,47,39,10,0,0,24,27,45,45,51,53,53,53,50,45,45,45,31,15,42,8,37,14,36,51,30,14,15,12,0,0,22,40,48,52,41,43,3,3,43,41,52,45,29,21,0,
12,32,46,54,54,54,54,54,54,51,48,36,39,37,5,0,4,37,39,41,45,52,54,54,54,54,54,54,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,0,21,32,30,51,54,51,46,28,17,0,0,0,0,0,0,19,43,44,53,54,54,54,53,44,37,42,0,0,0,0,10,39,41,43,7,0,0,0,0,0,12,28,38,16,8,0,10,39,41,39,48,54,54,54,54,54,46,28,38,16,8,0,0,0,0,17,15,27,48,54,54,45,27,37,21,0,0,0,0,0,0,17,15,27,33,23,24,27,27,37,21,0,0,0,0,0,23,39,46,46,27,49,41,15,4,0,0,1,37,
36,45,33,22,0,0,12,28,36,36,38,30,36,36,49,36,36,33,22,0,0,0,0,0,2,14,46,51,41,43,3,0,0,0,0,0,17,20,44,48,48,35,16,21,0,0,0,11,27,50,35,33,33,36,51,30,33,33,11,0,0,0,0,0,17,29,46,52,41,37,6,0,0,0,0,24,27,53,48,39,17,0,0,9,18,44,47,46,28,17,0,0,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,0,19,37,36,51,47,28,14,29,30,51,38,20,4,19,37,36,51,47,28,12,5,20,31,51,45,29,26,0,19,37,36,51,47,28,28,28,28,28,22,0,19,37,36,51,47,28,28,28,28,28,22,
0,19,37,36,51,47,28,28,28,28,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,31,52,49,44,16,21,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,30,14,44,53,53,30,14,44,52,46,34,12,19,37,36,51,30,14,35,50,31,30,51,31,15,1,19,37,36,51,48,46,46,30,36,33,37,2,0,0,0,0,19,37,36,51,47,28,32,14,15,25,24,27,53,45,29,15,39,35,41,30,53,47,29,26,0,0,22,39,33,36,52,45,33,33,29,10,16,14,12,0,0,12,18,39,43,32,35,27,28,36,50,33,11,0,0,24,27,27,46,51,36,27,27,27,
27,27,27,27,15,42,2,14,30,30,51,47,30,29,26,0,0,21,28,46,51,30,14,26,26,14,38,51,45,29,21,0,12,32,46,54,54,54,54,51,49,31,33,20,23,4,0,0,0,8,23,16,33,31,48,51,54,54,54,54,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,0,3,18,35,50,54,53,44,43,1,0,0,0,0,0,0,26,28,30,51,54,54,54,54,46,29,10,0,0,0,0,10,39,41,43,7,0,0,0,0,0,12,28,38,16,8,0,10,39,41,39,48,54,54,54,54,54,46,28,38,16,8,0,0,0,0,0,25,35,50,53,53,49,27,24,0,0,0,
0,0,0,0,0,25,35,28,28,32,29,27,24,0,0,0,0,0,12,20,36,49,35,14,31,45,40,22,0,0,4,43,29,29,32,23,0,0,12,28,46,46,49,46,46,47,48,46,46,33,22,0,0,0,3,21,21,14,46,51,41,43,21,12,0,0,0,17,20,44,49,48,35,16,22,22,12,0,0,11,27,50,50,50,50,50,54,53,50,27,11,0,0,0,0,7,13,35,50,48,40,23,0,0,0,0,0,25,35,53,46,39,17,0,0,0,25,29,29,29,20,5,0,0,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,19,37,36,51,47,28,21,15,31,51,46,32,12,19,37,36,51,47,28,
11,43,33,45,51,38,14,5,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,21,21,21,21,3,0,19,37,36,51,47,28,21,21,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,29,30,52,48,35,37,12,0,0,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,30,14,39,45,53,44,18,44,52,46,34,12,19,37,36,51,30,14,28,30,48,30,51,31,15,1,19,37,36,51,47,28,32,14,15,25,5,0,0,0,0,0,19,37,36,51,47,28,12,2,8,0,24,27,50,48,40,33,32,15,37,40,49,48,39,10,0,0,1,22,24,33,48,52,
31,29,20,29,44,29,21,0,0,0,42,10,2,17,25,18,14,31,49,33,23,0,0,2,24,14,30,51,38,39,32,29,27,31,40,24,10,0,2,14,46,48,51,49,45,29,26,0,0,8,15,41,52,49,44,28,28,44,49,51,45,29,21,0,12,32,46,54,54,51,50,30,27,14,24,2,0,0,0,0,0,0,0,5,24,14,27,38,50,51,54,54,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,0,0,10,29,46,54,47,29,22,0,0,0,0,0,0,7,13,27,50,54,54,54,54,54,50,35,18,3,0,0,0,10,39,41,43,7,0,0,0,0,0,12,28,38,16,8,
0,10,39,41,39,48,51,51,51,51,51,46,28,38,16,8,0,0,0,0,8,15,36,49,41,36,50,41,37,1,0,0,0,0,0,0,8,15,36,39,41,36,29,41,37,1,0,0,0,7,13,27,48,31,14,11,40,30,14,26,0,0,12,34,38,38,44,13,9,0,2,20,34,39,48,27,34,31,38,34,34,32,10,0,0,0,22,29,29,29,46,51,41,29,29,14,12,0,4,20,44,49,48,35,40,40,40,40,37,1,0,11,27,30,30,30,30,30,51,47,30,27,11,0,0,0,0,12,14,38,51,31,14,2,0,0,0,0,1,37,36,50,35,43,6,0,0,9,18,44,38,38,32,12,0,0,0,0,10,29,
47,52,30,30,30,30,30,50,48,33,23,0,0,19,37,36,51,47,29,29,33,46,51,30,14,5,19,37,36,51,47,33,33,44,47,51,48,27,13,7,0,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,28,39,30,51,45,27,43,5,0,0,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,30,14,20,31,31,39,18,44,52,46,34,12,19,37,36,51,30,14,43,35,50,51,51,31,15,1,19,37,36,51,47,28,12,2,8,0,0,0,0,0,0,0,
19,37,36,51,47,28,12,0,0,0,22,40,48,52,45,45,30,36,36,30,52,46,28,21,0,0,0,0,12,20,41,50,53,46,30,46,45,29,21,0,0,0,0,0,0,0,24,27,36,49,46,29,26,0,0,0,19,43,44,50,53,45,46,47,49,49,40,22,0,0,5,20,39,36,51,30,39,34,21,0,0,0,23,39,46,51,50,46,46,45,35,53,45,29,21,0,12,32,46,54,53,46,35,28,13,21,9,0,0,0,0,0,0,0,0,0,0,12,25,32,35,46,53,54,47,28,12,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,0,0,42,37,44,53,41,15,8,0,0,0,0,0,
0,12,14,38,51,54,54,54,54,54,51,30,32,21,0,0,0,10,39,41,43,2,2,2,2,2,2,12,28,38,16,8,0,10,39,41,39,38,38,38,38,38,38,38,28,38,16,8,0,0,0,0,21,28,46,35,14,14,44,30,32,12,0,0,0,0,0,0,21,28,35,35,14,14,44,35,32,12,0,0,0,8,16,31,48,33,25,4,43,29,13,19,0,0,12,34,46,52,44,13,9,0,0,2,25,27,48,39,28,46,44,43,12,12,9,0,0,0,23,33,45,45,49,51,45,45,47,28,12,0,4,14,30,54,53,49,49,49,49,31,15,42,0,10,20,14,14,14,14,36,51,30,14,20,10,0,0,0,0,11,
33,48,50,27,25,9,0,0,0,0,12,14,30,38,32,26,0,0,0,9,18,44,52,46,32,12,0,0,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,19,37,36,51,49,47,47,48,51,49,44,43,19,19,37,36,51,53,49,50,53,52,48,44,20,12,0,0,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,28,13,33,46,51,47,33,18,4,0,19,37,36,51,53,49,49,49,49,31,16,6,19,37,36,51,30,14,24,15,15,43,18,44,52,46,
34,12,19,37,36,51,30,14,10,29,46,51,51,31,15,1,19,37,36,51,47,28,12,0,0,0,0,0,0,0,0,0,19,37,36,46,46,28,12,0,0,0,2,20,44,30,46,46,47,48,48,45,30,27,18,1,0,0,0,0,0,22,14,44,46,48,48,47,36,29,21,0,0,0,0,0,0,0,24,27,45,30,33,18,1,0,0,0,0,10,14,44,46,48,49,45,46,41,40,22,0,0,0,21,37,36,51,30,14,26,7,0,0,0,6,18,33,30,48,48,46,27,35,46,46,29,21,0,12,32,46,45,44,29,43,17,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,26,18,29,44,47,47,28,12,0,
19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,0,0,0,22,39,45,40,23,0,0,0,0,0,0,0,24,33,49,54,54,54,54,54,54,54,49,27,25,9,0,0,10,39,41,14,14,14,14,14,14,14,14,28,38,16,8,0,10,39,41,14,14,14,14,14,14,14,14,28,38,16,8,0,0,0,0,11,33,27,15,21,17,16,27,40,22,0,0,0,0,0,0,11,33,27,15,21,17,16,27,40,22,0,0,0,8,15,36,36,20,2,0,4,21,42,0,0,0,12,34,46,46,44,13,9,0,0,19,37,41,30,32,40,46,27,24,0,0,0,0,0,0,23,33,46,46,46,46,46,46,46,28,
12,0,4,14,30,46,46,46,46,46,46,31,15,42,0,0,4,4,4,4,37,36,46,30,20,4,0,0,0,0,8,15,36,46,46,28,26,0,0,0,0,0,21,28,33,33,25,7,0,0,0,9,18,44,46,46,32,12,0,0,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,19,37,36,46,46,46,46,46,38,35,14,17,0,19,37,36,46,46,46,46,30,36,33,16,17,0,0,0,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,28,12,0,0,0,0,0,19,37,36,46,46,28,12,12,32,46,46,31,15,1,19,37,36,46,46,28,12,0,19,37,36,46,46,28,12,18,33,
46,46,46,40,25,9,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,30,14,5,1,1,19,18,44,46,46,34,12,19,37,36,46,30,14,5,43,44,46,46,31,15,1,19,37,36,46,46,28,12,0,0,0,0,0,0,0,0,0,7,13,34,34,34,20,2,0,0,0,0,17,37,32,34,34,28,39,40,39,32,13,2,0,0,0,0,0,0,0,17,37,28,39,40,29,15,23,19,0,0,0,0,0,0,0,24,40,39,14,13,5,0,0,0,0,0,0,10,37,29,39,40,39,32,37,11,4,0,0,0,3,37,36,46,30,14,4,0,0,0,0,0,4,25,32,39,40,28,43,34,34,34,20,12,0,
12,32,36,39,15,22,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,10,37,39,36,28,12,0,19,37,36,46,46,46,46,46,46,46,46,46,46,33,23,0,0,0,0,0,0,4,15,36,20,5,0,0,0,0,0,0,5,20,31,46,46,46,46,46,46,46,46,46,38,14,12,0,0,10,39,46,46,46,46,46,46,46,46,46,46,38,16,8,0,10,39,46,46,46,46,46,46,46,46,46,46,38,16,8,0,0,0,0,24,29,18,2,0,0,12,43,29,23,0,0,0,0,0,0,24,29,18,2,0,0,12,43,29,23,0,0,0,0,23,29,34,23,0,0,0,0,0,0,0,0,5,16,
34,34,34,24,0,0,0,19,18,34,34,37,32,34,14,17,0,0,0,0,0,0,22,32,34,34,34,34,34,34,34,20,2,0,8,37,34,34,34,34,34,34,34,34,43,19,0,0,0,0,0,3,18,34,34,34,37,6,0,0,0,0,6,37,34,34,34,43,1,0,0,0,0,0,3,22,22,22,4,0,0,0,0,0,25,34,34,34,16,5,0,0,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,7,13,34,34,34,34,34,32,20,18,26,0,0,7,13,34,34,34,34,32,14,37,24,12,0,0,0,0,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,20,2,0,0,0,0,
0,7,13,34,34,34,20,2,5,16,34,34,34,18,3,7,13,34,34,34,20,2,0,7,13,34,34,34,20,2,5,37,34,34,34,34,25,0,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,37,8,0,0,0,25,34,34,34,16,5,7,13,34,34,34,37,8,22,32,34,34,34,18,3,7,13,34,34,34,20,2,0,0,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,1,12,12,12,21,10,22,10,12,3,0,0,0,0,0,0,0,0,0,1,21,22,22,26,4,0,0,0,0,0,0,0,0,0,4,22,10,12,7,0,0,0,0,0,0,0,0,8,21,22,22,10,
12,42,0,0,0,0,0,7,13,34,34,34,37,8,0,0,0,0,0,0,7,12,22,22,21,1,12,12,12,2,0,0,4,15,20,11,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,23,16,16,2,0,7,13,32,32,32,32,32,32,32,32,32,32,32,14,10,0,0,0,0,0,0,0,11,32,24,0,0,0,0,0,0,0,4,15,32,32,32,32,32,32,32,32,32,32,32,16,2,0,0,21,14,32,32,32,32,32,32,32,32,32,32,32,43,1,0,21,14,32,32,32,32,32,32,32,32,32,32,32,43,1,0,0,0,0,1,21,8,0,0,0,0,4,21,19,0,0,
0,0,0,0,1,21,8,0,0,0,0,4,21,19,0,0,0,0,19,26,21,7,0,0,0,0,0,0,0,0,0,5,12,12,12,1,0,0,0,0,4,12,12,4,12,12,12,9,0,0,0,0,0,0,7,12,12,12,12,12,12,12,12,2,0,0,0,4,12,12,12,12,12,12,12,12,4,0,0,0,0,0,0,0,8,12,12,12,4,0,0,0,0,0,0,4,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,12,12,12,5,0,0,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,8,12,12,12,12,12,12,4,7,0,0,0,0,8,12,12,12,12,
12,5,1,0,0,0,0,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,2,0,0,0,0,0,0,0,8,12,12,12,2,0,0,5,12,12,12,8,0,0,8,12,12,12,2,0,0,0,8,12,12,12,2,0,0,4,12,12,12,12,1,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,4,0,0,0,0,1,12,12,12,5,0,0,8,12,12,12,4,0,7,12,12,12,12,8,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0,0,0,6,12,12,12,12,12,12,12,12,12,12,12,2,9,0,0,0,0,0,0,0,3,12,19,0,0,0,0,0,0,0,0,4,12,12,12,12,12,12,12,12,12,12,12,5,0,0,0,0,2,12,12,12,12,12,12,12,12,12,12,12,8,0,
0,0,2,12,12,12,12,12,12,12,12,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,9,0,0,7,19,19,19,9,0,0,0,0,19,19,19,3,0,0,0,0,19,19,19,7,0,0,19,19,19,3,0,0,3,19,19,19,0,0,0,0,19,19,19,3,0,0,0,3,19,19,19,0,0,0,0,3,19,19,19,9,0,0,0,19,19,19,19,0,0,0,0,3,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,42,42,42,0,0,0,0,0,0,0,0,1,4,5,8,7,0,0,0,9,19,19,19,7,0,0,7,19,19,19,9,0,0,0,0,0,0,0,
7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,19,19,19,0,0,0,0,0,19,19,19,19,3,7,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,0,19,19,19,7,0,42,8,8,1,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,19,19,19,3,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,9,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,42,42,42,1,0,0,0,0,0,0,0,3,19,19,19,3,0,0,0,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,9,0,0,0,0,0,0,3,19,19,19,3,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,0,0,0,0,3,19,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,19,19,19,7,0,0,0,0,19,19,19,7,0,0,3,19,19,19,3,0,0,0,0,0,0,0,3,19,19,19,3,0,0,0,0,0,0,0,0,19,19,19,19,
9,0,0,0,19,19,19,19,7,0,0,0,19,19,19,19,0,0,9,19,19,19,0,0,0,19,2,2,2,4,0,0,0,11,37,37,37,37,37,43,25,17,3,0,0,0,1,13,37,37,37,37,37,37,37,37,37,37,24,9,1,13,37,37,37,24,7,0,0,23,37,37,37,18,4,0,0,22,37,37,37,18,4,26,37,37,37,37,12,2,43,37,37,37,17,0,0,22,37,37,37,37,12,0,2,43,37,37,37,11,0,0,2,43,37,37,37,25,3,0,17,37,37,37,37,22,0,0,12,43,37,37,37,37,37,37,37,37,37,23,0,0,0,2,22,22,22,17,0,0,0,0,4,43,15,15,15,23,0,0,
0,0,0,42,23,37,20,14,16,18,22,0,3,25,37,37,37,13,8,42,13,37,37,37,25,3,0,0,0,0,12,24,18,43,25,10,1,0,0,0,0,0,0,0,0,12,24,18,43,25,10,1,0,0,0,0,0,0,0,0,0,23,37,37,37,37,37,37,37,37,37,37,37,17,0,0,0,22,37,37,37,37,37,18,25,10,42,0,0,0,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,0,11,37,37,37,13,8,8,13,37,37,37,11,9,0,0,11,37,37,37,18,2,37,20,20,18,6,0,11,37,37,37,18,4,0,0,0,0,0,0,0,0,22,37,37,37,43,5,0,0,0,0,0,0,0,
17,37,37,37,13,8,0,0,0,0,0,0,0,0,0,0,1,13,37,37,37,37,37,37,37,37,37,37,24,9,0,0,2,22,22,22,17,0,0,0,0,0,0,10,15,15,15,37,12,0,0,0,0,0,4,18,37,37,37,43,12,0,0,0,0,0,0,11,37,37,37,37,37,43,25,10,19,0,0,0,11,37,37,37,37,37,37,37,37,24,7,0,0,0,0,4,18,37,37,37,43,12,0,0,0,0,0,0,11,37,37,37,37,37,37,37,37,23,0,0,12,43,37,37,37,37,37,37,37,37,37,23,0,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,11,37,37,37,18,4,0,0,11,37,37,37,
18,4,5,18,37,37,37,18,4,0,0,0,0,0,4,18,37,37,37,43,12,0,0,0,0,0,0,11,37,37,37,37,24,7,0,10,37,37,37,37,13,8,0,11,37,37,37,37,21,3,25,37,37,37,11,9,0,24,32,32,32,15,5,0,19,37,36,36,36,36,36,41,27,28,24,7,0,0,2,14,36,36,36,36,36,36,36,36,36,36,15,1,2,14,36,36,36,14,2,0,8,15,36,36,36,29,21,0,9,18,44,36,36,29,26,25,27,36,36,33,23,22,39,36,36,27,25,0,7,18,35,36,36,27,13,19,25,33,36,36,44,37,3,0,17,29,36,36,36,28,17,6,43,35,36,36,35,18,9,0,
10,39,36,36,36,36,36,36,36,36,41,43,3,0,9,13,40,40,40,14,2,0,0,0,12,34,31,31,44,13,9,0,0,0,3,25,39,41,30,30,31,44,15,1,4,20,36,36,36,39,22,22,29,36,36,36,20,4,0,0,3,11,14,33,44,41,35,29,18,2,0,0,0,0,0,3,11,14,33,44,41,35,29,18,2,0,0,0,0,0,0,0,8,15,36,36,36,36,36,36,36,36,36,36,27,24,0,0,9,18,44,36,36,36,36,44,27,29,43,12,0,0,0,19,37,36,36,36,28,12,3,0,0,0,0,0,0,12,37,36,36,36,28,22,22,34,36,36,36,37,12,0,19,37,36,36,36,28,10,39,
38,31,20,5,19,37,36,36,36,28,12,0,0,0,0,0,0,0,7,18,41,36,36,29,26,42,0,0,0,0,0,0,24,27,36,36,34,12,0,0,0,0,0,0,0,0,0,0,2,14,36,36,36,36,36,36,36,36,36,36,15,1,0,9,13,40,40,40,14,2,0,0,0,0,0,24,27,31,31,39,17,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,19,37,36,36,36,36,36,41,35,29,25,19,0,19,37,36,36,36,36,36,36,36,36,16,6,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,19,37,36,36,36,36,36,36,36,41,43,7,0,10,39,36,36,36,36,36,36,
36,36,41,43,3,19,37,36,36,36,28,12,12,32,36,36,36,15,1,19,37,36,36,36,28,12,0,19,37,36,36,36,28,12,18,33,36,36,36,28,21,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,19,37,36,36,36,36,14,2,3,18,44,36,36,36,34,12,19,37,36,36,36,35,13,4,20,36,36,36,15,1,9,13,44,46,46,34,12,0,19,37,36,51,51,52,52,52,50,38,34,26,0,0,2,14,46,52,52,52,51,51,52,52,52,31,15,1,3,18,44,53,45,39,22,0,26,29,46,51,36,15,8,0,0,11,33,49,49,33,23,43,41,52,53,35,13,25,27,50,48,39,22,0,
0,26,32,38,51,30,29,10,32,38,51,30,28,10,0,0,1,43,35,50,50,44,37,22,39,46,51,38,32,26,0,0,10,39,48,52,52,52,52,51,51,51,41,43,3,0,3,18,44,49,46,32,12,42,0,0,12,34,46,52,44,13,9,0,0,0,26,28,30,52,50,48,49,31,15,1,0,11,40,45,53,41,15,15,41,53,45,40,11,0,0,9,11,29,31,46,36,41,38,46,27,37,5,0,0,0,9,11,29,31,46,36,41,38,46,27,37,5,0,0,0,0,0,0,10,29,47,51,50,53,51,51,52,52,52,53,27,24,0,0,9,18,44,52,51,52,51,52,50,46,27,37,2,0,0,19,37,36,51,47,
28,37,43,25,10,19,0,0,3,43,40,36,51,47,40,40,40,40,46,51,36,40,37,19,19,37,36,51,47,28,11,27,50,27,25,9,19,37,36,51,47,28,12,0,0,0,0,0,0,0,7,18,41,52,45,29,15,43,4,0,0,0,0,0,24,27,53,46,34,12,42,42,3,5,12,4,0,0,0,0,2,14,46,52,52,52,51,51,52,52,52,31,15,1,0,3,18,44,49,46,32,12,42,0,0,0,0,24,27,53,48,39,17,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,52,50,50,52,50,30,29,10,0,19,37,36,51,51,52,52,52,52,31,16,6,0,0,0,9,13,
35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,51,52,52,52,52,41,43,7,0,10,39,48,52,52,52,52,51,51,51,41,43,3,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,28,13,40,46,52,30,39,24,3,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,51,48,40,23,12,32,30,51,51,46,34,12,19,37,36,51,51,30,32,21,20,30,51,31,15,1,9,13,44,49,46,34,12,0,19,37,36,51,47,44,44,30,52,50,27,24,0,0,2,14,41,41,41,41,53,49,41,41,41,41,15,1,0,22,40,48,
53,35,18,7,24,27,50,50,27,24,0,0,0,17,29,45,53,35,13,20,38,49,50,36,15,43,41,52,46,34,12,0,0,7,25,33,45,53,44,37,35,50,48,27,13,3,0,0,0,26,32,38,51,46,39,16,41,53,48,33,25,7,0,0,10,39,41,41,41,41,41,31,51,48,27,18,7,3,13,15,44,52,46,32,15,15,23,0,12,34,46,47,44,13,9,0,0,0,24,27,49,50,44,39,33,41,15,1,0,4,15,41,53,45,33,40,45,53,41,15,5,0,0,26,32,31,36,29,29,39,29,27,47,33,25,9,0,0,26,32,31,36,39,39,39,34,27,47,33,25,9,0,0,0,0,7,13,35,50,48,
33,30,51,30,41,41,41,41,27,24,0,0,9,18,44,52,48,41,41,38,48,51,45,33,25,7,0,19,37,36,51,47,36,36,41,35,29,25,42,0,19,37,36,49,54,50,48,48,48,48,50,54,49,36,37,19,19,37,36,51,47,28,18,44,46,28,26,0,19,37,36,51,47,28,12,21,21,21,12,0,0,0,7,18,41,52,45,29,44,40,24,9,0,0,0,0,25,27,53,46,34,15,15,15,13,14,32,20,24,1,0,0,2,14,41,41,41,41,53,49,41,41,41,41,15,1,3,13,15,44,52,46,32,15,15,23,0,0,0,24,27,49,48,39,17,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,
0,0,19,37,36,51,47,27,27,36,53,50,35,25,0,19,37,36,51,47,41,41,41,41,41,16,6,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,19,37,36,51,47,41,41,41,41,41,43,7,0,10,39,41,41,41,41,41,31,51,48,27,18,7,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,28,39,30,51,30,39,25,1,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,0,19,37,36,51,52,52,41,37,11,33,48,53,54,46,34,12,19,37,36,51,54,49,27,13,20,30,51,31,15,1,0,13,40,40,40,14,12,0,
19,37,36,51,47,28,18,33,49,53,35,25,0,0,19,25,43,43,43,35,53,49,33,43,43,43,23,0,0,12,14,30,51,38,20,5,15,36,52,46,29,26,0,0,0,2,14,30,51,36,15,29,47,30,46,30,32,20,38,51,31,16,4,0,0,0,2,16,41,53,45,40,47,52,36,20,12,0,0,0,0,7,25,33,48,53,41,33,45,52,36,20,12,0,0,0,5,18,43,43,43,43,27,48,53,41,20,12,0,4,20,31,31,52,47,31,31,44,18,9,2,20,28,28,28,25,0,0,0,0,13,35,53,45,29,10,23,15,25,7,0,21,25,29,46,52,31,36,52,46,39,25,21,0,7,13,35,38,34,44,
46,45,47,33,35,38,14,12,0,7,13,35,38,34,46,30,46,30,40,35,38,14,12,0,0,0,0,2,14,38,51,38,14,30,51,38,20,43,43,43,43,21,0,0,1,18,44,52,48,39,43,20,27,48,51,38,14,2,0,19,37,36,51,51,52,52,52,50,30,39,23,0,19,37,33,36,51,47,33,33,33,33,46,51,36,33,37,19,19,37,36,51,47,28,18,39,39,37,1,0,19,37,36,51,47,28,25,29,29,29,20,2,0,0,7,18,41,52,45,36,50,31,14,5,0,0,0,1,37,41,50,35,16,31,31,31,27,38,46,31,39,11,0,0,19,25,43,43,43,35,53,49,33,43,43,43,23,0,4,20,
31,31,52,47,31,31,44,18,9,0,0,24,40,33,33,29,17,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,47,28,24,39,48,52,44,13,0,19,37,36,51,47,28,43,43,43,43,11,9,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,47,28,43,43,43,43,22,0,0,5,18,43,43,43,43,27,48,53,41,20,12,0,19,37,36,51,47,28,22,22,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,29,30,52,46,33,13,8,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,30,47,
46,28,15,36,50,44,52,46,34,12,19,37,36,51,45,49,38,14,20,30,51,31,15,1,9,13,44,47,46,34,12,0,19,37,36,51,47,28,13,33,49,50,27,24,0,0,0,0,7,7,25,35,53,49,33,22,7,7,0,0,0,3,18,44,53,45,39,22,28,46,51,36,15,8,0,0,0,1,37,36,51,30,32,33,49,41,31,45,39,29,47,52,44,18,7,0,0,0,0,22,29,46,52,30,52,47,39,23,0,0,0,0,0,0,2,16,41,53,45,31,51,47,39,23,0,0,0,0,0,7,7,7,24,40,47,52,38,28,22,0,0,4,20,38,49,51,50,49,49,44,18,9,19,37,36,30,40,22,0,0,
7,11,13,44,52,47,28,13,13,22,7,0,0,13,35,35,35,53,49,49,53,44,35,35,13,9,4,20,31,27,35,49,36,33,35,33,34,30,40,22,0,4,20,31,27,32,46,44,39,45,44,34,30,40,22,0,0,0,0,22,40,45,50,35,14,30,51,38,40,40,40,40,28,26,0,19,18,14,44,52,48,39,14,18,20,36,51,45,39,10,0,19,37,36,51,47,44,44,31,50,52,36,15,6,0,21,37,36,51,47,40,40,40,40,46,51,36,37,21,0,19,37,36,51,47,28,12,17,17,2,0,0,19,37,36,51,47,28,13,44,45,46,34,12,0,0,7,18,41,52,51,49,36,29,25,1,0,0,0,2,
14,30,38,32,16,38,51,31,45,45,53,52,36,20,4,0,0,0,7,12,25,35,53,49,33,22,5,7,0,0,4,20,38,49,51,50,49,49,44,18,9,0,0,4,22,22,22,22,3,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,47,39,39,35,50,49,27,24,0,19,37,36,51,47,28,12,7,7,7,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,47,40,40,40,40,39,23,0,0,0,7,7,7,24,40,47,52,38,28,22,0,0,19,37,36,51,47,40,40,40,40,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,
31,52,47,33,18,4,0,0,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,30,36,49,27,29,47,46,44,52,46,34,12,19,37,36,51,30,30,49,27,20,30,51,31,15,1,9,13,44,48,46,34,12,0,19,37,36,51,47,44,44,30,53,38,28,26,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,22,40,48,53,35,13,27,49,50,27,24,0,0,0,0,0,13,35,53,45,39,35,53,35,44,50,27,33,49,50,27,24,0,0,0,0,0,1,18,35,49,54,50,44,37,8,0,0,0,0,0,0,0,23,39,46,51,52,50,44,37,8,0,0,0,0,
0,0,0,22,28,38,52,47,40,24,7,0,0,8,16,40,44,52,46,40,40,40,13,9,8,16,31,50,27,11,0,0,5,14,44,44,52,47,44,44,44,37,3,0,0,13,44,47,47,45,51,51,45,47,47,44,13,9,12,32,46,29,31,45,39,11,18,20,37,36,27,24,0,12,32,46,29,32,46,44,41,48,27,37,36,27,24,0,0,0,19,43,44,53,47,29,14,30,54,49,49,49,49,45,39,17,0,6,16,31,30,51,49,30,38,20,13,35,53,50,33,23,0,19,37,36,51,47,28,13,34,30,54,30,14,2,0,19,37,36,51,50,49,49,49,49,50,51,36,37,1,0,19,37,36,51,47,28,12,0,
0,0,0,0,19,37,36,51,47,28,13,44,52,46,34,12,0,7,10,20,41,52,48,35,14,22,19,0,0,0,0,21,29,33,33,25,16,38,51,49,35,39,36,52,47,28,21,0,0,0,21,14,34,35,53,49,33,34,16,5,0,0,8,16,40,44,52,46,40,40,40,13,9,0,0,23,28,28,28,14,21,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,19,37,36,51,50,48,48,50,48,44,14,21,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,4,20,31,51,38,20,36,51,30,32,12,0,0,0,19,37,36,51,53,49,49,49,49,27,11,0,0,0,0,0,22,28,38,52,47,
40,24,7,0,0,19,37,36,51,53,49,49,49,49,50,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,52,52,45,27,37,5,0,0,0,0,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,19,37,36,51,30,27,49,31,35,50,41,44,52,46,34,12,19,37,36,51,30,27,50,31,14,30,51,31,15,1,0,25,39,39,39,14,12,0,19,37,36,51,51,52,52,53,44,29,24,3,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,12,14,30,51,31,20,41,52,46,29,26,0,0,0,0,0,23,33,49,50,27,36,48,40,27,50,44,35,53,45,39,10,0,0,
0,0,0,19,13,27,49,54,50,35,43,42,0,0,0,0,0,0,0,6,43,35,50,51,30,28,17,0,0,0,0,0,0,0,12,20,41,50,48,27,43,4,0,0,0,0,21,18,44,52,46,32,22,22,2,0,5,14,30,53,35,25,0,0,5,14,30,50,51,52,50,50,36,37,3,0,0,24,28,28,28,35,50,50,35,28,28,28,25,0,12,34,46,29,31,47,29,21,22,25,37,36,27,24,0,12,34,46,29,32,46,41,47,27,16,37,36,27,24,0,0,0,21,32,30,52,36,14,14,30,54,45,47,47,47,47,39,17,0,6,16,31,45,51,53,45,38,20,25,35,53,50,33,11,0,19,37,36,51,47,
28,22,14,30,51,46,32,12,0,19,37,36,51,49,47,47,47,47,49,51,36,37,1,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,28,13,44,46,46,34,12,0,11,34,41,48,51,45,29,26,0,0,0,0,0,0,3,22,23,23,5,16,38,51,30,14,25,35,53,45,29,26,0,0,0,10,39,46,46,52,53,46,46,34,12,0,0,4,20,36,36,52,46,36,36,44,18,9,0,0,24,27,47,47,39,17,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,48,30,30,46,53,47,27,13,7,19,37,36,51,47,28,12,0,0,0,0,0,0,0,10,29,47,
53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,49,47,47,47,47,27,11,0,0,0,0,12,20,41,50,48,27,43,4,0,0,0,19,37,36,51,49,47,47,47,47,49,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,52,52,50,44,20,26,0,0,0,0,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,30,34,30,45,38,48,33,44,52,46,34,12,19,37,36,51,30,34,30,48,33,30,51,31,15,1,9,13,44,45,46,34,12,0,19,37,36,51,47,44,36,49,50,36,20,2,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,3,18,
44,53,45,39,46,51,36,15,8,0,0,0,0,0,21,29,47,52,44,30,46,28,39,45,31,36,51,30,14,12,0,0,0,0,0,22,29,30,51,47,52,46,39,23,0,0,0,0,0,0,0,0,17,39,48,52,44,18,3,0,0,0,0,0,0,8,43,27,48,53,41,20,12,0,0,0,0,0,9,18,44,52,46,32,12,0,0,0,12,34,46,52,44,13,9,0,5,14,27,44,52,47,27,27,27,37,3,0,0,13,44,31,31,31,50,53,31,31,31,44,13,9,5,14,30,40,44,49,27,14,29,33,20,38,33,23,0,5,14,30,40,32,46,44,44,46,29,20,38,33,23,0,0,0,24,27,49,52,30,30,
30,45,51,38,28,28,28,28,14,21,0,1,37,39,44,52,48,39,39,37,37,41,52,48,40,22,0,19,37,36,51,47,40,40,35,48,51,31,20,4,0,19,37,36,51,47,28,28,28,28,46,51,36,37,1,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,28,24,32,32,32,15,5,3,43,35,49,46,51,45,29,26,0,0,0,0,0,0,0,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,10,39,47,47,52,53,47,46,34,12,0,0,4,20,38,50,54,53,50,50,44,18,9,0,0,24,27,53,48,39,17,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,
0,0,19,37,36,51,47,28,14,29,30,51,38,20,4,19,37,36,51,47,28,12,0,0,0,0,0,0,9,13,35,50,48,40,22,29,47,52,44,43,19,0,0,19,37,36,51,47,28,28,28,28,28,22,0,0,0,8,43,27,48,53,41,20,12,0,0,0,0,19,37,36,51,47,28,28,28,28,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,31,52,49,44,16,21,0,0,0,0,0,9,13,35,50,48,40,22,29,47,52,44,43,19,0,0,19,37,36,51,30,14,44,53,53,30,14,44,52,46,34,12,19,37,36,51,30,14,35,50,31,30,51,31,15,1,9,13,44,45,46,34,12,0,
19,37,36,51,47,28,20,41,52,48,33,11,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,22,40,48,50,35,49,50,27,24,0,0,0,0,0,0,4,20,38,51,31,45,31,20,14,30,46,30,51,36,15,42,0,0,0,0,5,15,44,53,45,33,47,53,41,20,2,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,7,24,39,46,51,38,28,23,23,23,23,26,0,0,9,18,44,52,46,32,12,12,7,0,12,34,46,52,44,13,9,0,9,22,13,44,52,47,28,11,11,26,2,0,0,13,44,31,31,31,50,50,31,31,31,44,13,9,19,37,44,41,29,30,
48,30,46,33,33,46,28,21,0,19,37,44,41,32,46,44,39,47,35,33,46,28,21,0,0,4,15,36,52,49,49,49,49,50,51,38,20,23,23,23,23,12,0,0,12,18,44,52,48,39,23,18,39,47,51,46,34,21,0,19,37,36,51,50,48,48,50,51,45,27,25,9,0,19,37,36,51,47,28,21,21,34,46,51,36,37,1,0,19,37,36,51,47,28,23,23,23,23,21,0,19,37,36,51,47,28,23,23,23,23,21,0,0,26,14,41,41,52,45,29,23,23,23,23,17,0,0,0,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,26,32,28,35,53,49,33,28,20,2,0,0,4,20,
27,44,52,46,27,27,27,13,9,0,0,25,35,53,46,39,17,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,19,37,36,51,47,28,21,15,31,51,46,32,12,19,37,36,51,47,28,12,0,0,0,0,0,0,4,20,31,51,30,14,23,16,31,51,30,32,12,0,0,19,37,36,51,47,28,23,23,23,23,21,0,0,7,24,39,46,51,38,28,23,23,23,23,26,0,19,37,36,51,47,28,21,21,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,29,30,52,48,35,37,12,0,0,0,0,4,20,31,51,30,14,12,16,36,51,30,32,12,0,0,19,37,36,51,30,14,
39,45,53,44,18,44,52,46,34,12,19,37,36,51,30,14,28,30,48,30,51,31,15,1,0,13,39,39,39,14,12,0,19,37,36,51,47,28,23,40,45,52,36,15,4,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,12,14,30,51,45,52,46,29,26,0,0,0,0,0,0,3,37,41,52,49,53,44,43,15,36,53,50,53,35,13,9,0,0,0,9,24,40,45,53,41,16,44,50,48,33,25,7,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,10,34,31,52,47,33,33,33,33,33,33,15,8,0,9,18,44,52,47,29,28,32,22,0,12,34,46,52,44,13,9,0,
21,32,29,44,52,47,29,29,29,29,37,6,0,22,15,15,15,27,50,50,27,15,15,15,22,0,0,22,39,46,27,29,35,44,35,39,38,35,43,1,0,0,22,39,46,27,27,27,15,27,27,38,35,43,1,0,0,10,29,47,53,44,33,33,33,30,51,38,33,33,33,33,33,18,9,0,9,18,44,52,48,33,33,35,46,52,50,35,43,1,0,19,37,36,51,48,46,46,46,31,33,37,12,0,0,19,37,36,51,47,28,12,12,34,46,51,36,37,1,0,19,37,36,51,47,33,33,33,33,33,15,6,19,37,36,51,47,33,33,33,33,33,15,6,0,9,11,15,41,52,45,33,33,33,33,33,20,4,0,0,
0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,9,12,25,35,53,49,33,22,12,0,0,0,0,10,18,44,52,47,29,28,32,22,0,0,1,37,36,50,35,43,6,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,19,37,36,51,47,29,29,33,46,51,30,14,5,19,37,36,51,47,28,12,0,0,0,0,0,0,10,29,47,52,44,33,33,33,35,53,48,33,23,0,0,19,37,36,51,47,33,33,33,33,33,15,6,0,10,34,31,52,47,33,33,33,33,33,33,15,8,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,
28,39,30,51,45,27,43,5,0,0,0,10,29,47,52,44,43,3,25,35,50,48,33,23,0,0,19,37,36,51,30,14,20,31,31,39,18,44,52,46,34,12,19,37,36,51,30,14,43,35,50,51,51,31,15,1,9,13,44,48,46,34,12,0,19,37,36,51,47,28,12,20,36,51,45,39,22,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,3,18,44,53,54,51,36,15,8,0,0,0,0,0,0,0,25,27,50,54,50,27,24,13,35,53,54,49,33,23,0,0,0,0,21,14,31,52,46,29,22,28,30,51,30,34,17,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,
11,27,50,54,53,49,49,49,49,49,38,16,8,0,0,24,27,50,53,47,47,27,11,0,12,34,46,52,44,13,9,0,17,39,45,45,51,49,45,45,45,38,20,4,0,0,1,1,11,27,50,50,27,11,1,1,0,0,0,6,18,27,47,41,33,40,35,30,41,14,26,0,0,0,6,18,27,47,41,33,40,35,30,41,14,26,0,0,7,13,35,50,45,39,22,22,14,30,54,50,49,49,49,49,44,18,9,0,9,18,44,52,52,49,49,53,51,49,41,14,26,0,0,19,37,36,51,47,28,32,32,16,25,2,0,0,0,19,37,36,51,47,28,12,12,34,46,51,36,37,1,0,19,37,36,51,53,49,49,49,
49,31,16,6,19,37,36,51,53,49,49,49,49,31,16,6,0,0,7,18,41,52,52,49,49,49,49,30,14,4,0,0,0,0,0,8,16,38,51,31,15,24,27,53,45,29,26,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,24,27,50,53,47,47,27,11,0,0,12,14,30,38,32,26,0,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,19,37,36,51,49,47,47,48,51,49,44,43,19,19,37,36,51,47,28,12,0,0,0,0,0,9,13,35,50,51,49,49,49,49,49,51,52,44,43,19,0,19,37,36,51,53,49,49,49,49,31,16,6,0,11,27,50,54,53,49,49,49,
49,49,38,16,8,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,19,37,36,51,47,28,13,33,46,51,47,33,18,4,0,9,13,35,50,48,33,23,0,10,39,45,52,44,43,19,0,19,37,36,51,30,14,24,15,15,43,18,44,52,46,34,12,19,37,36,51,30,14,10,29,46,51,51,31,15,1,9,13,44,46,46,34,12,0,19,37,36,46,46,28,12,24,33,46,46,44,37,1,0,0,0,0,25,35,46,46,33,22,0,0,0,0,0,0,0,0,22,40,46,46,46,27,24,0,0,0,0,0,0,0,0,22,39,46,46,46,39,10,23,33,46,46,46,29,26,0,0,0,
3,18,35,46,46,35,18,1,13,27,46,46,44,37,1,0,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,0,11,27,46,46,46,46,46,46,46,46,38,16,8,0,0,21,32,41,46,46,46,27,11,0,12,34,30,30,44,13,9,0,17,39,46,46,46,46,46,46,46,38,20,4,0,0,0,0,11,27,46,46,27,11,0,0,0,0,0,0,2,43,40,36,46,46,38,27,14,22,0,0,0,0,0,2,43,40,36,46,46,38,27,14,22,0,0,0,2,14,38,46,31,20,5,5,14,30,46,46,46,46,46,46,44,18,9,0,9,18,44,46,46,46,46,30,36,27,14,22,0,0,0,19,37,36,46,46,
28,12,12,4,9,0,0,0,0,19,37,36,46,46,28,12,12,34,46,46,36,37,1,0,19,37,36,46,46,46,46,46,46,31,16,6,19,37,36,46,46,46,46,46,46,31,16,6,0,0,7,18,41,46,46,46,46,46,46,30,14,4,0,0,0,0,0,8,16,38,46,31,15,24,27,46,46,29,26,0,0,0,0,0,25,35,46,46,33,22,0,0,0,0,0,0,21,32,41,46,46,46,27,11,0,0,21,28,33,33,25,7,0,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,19,37,36,46,46,46,46,46,38,35,14,17,0,19,37,36,46,46,28,12,0,0,0,0,0,4,20,31,46,46,
46,46,46,46,46,46,46,30,32,12,0,19,37,36,46,46,46,46,46,46,31,16,6,0,11,27,46,46,46,46,46,46,46,46,38,16,8,19,37,36,46,46,28,12,12,32,46,46,31,15,1,19,37,36,46,46,28,12,0,19,37,36,46,46,28,12,18,33,46,46,46,40,25,9,4,20,31,46,30,32,12,0,5,20,31,46,30,32,12,0,19,37,36,46,30,14,5,1,1,19,18,44,46,46,34,12,19,37,36,46,30,14,5,43,44,46,46,31,15,1,0,24,34,34,34,16,5,0,7,13,34,34,34,20,2,12,20,34,34,34,43,1,0,0,0,0,11,34,34,34,14,10,0,0,0,0,0,0,0,0,
12,20,34,34,34,14,26,0,0,0,0,0,0,0,0,12,20,34,34,34,20,12,21,14,34,34,34,15,5,0,0,0,3,18,34,34,34,14,21,0,12,16,34,34,34,43,1,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,22,32,34,34,34,34,34,34,34,34,34,43,1,0,0,9,22,15,32,34,34,32,22,0,5,15,14,14,14,24,0,0,21,14,34,34,34,34,34,34,34,34,37,42,0,0,0,0,22,32,34,34,32,22,0,0,0,0,0,0,0,4,11,15,32,34,20,13,21,0,0,0,0,0,0,0,4,11,15,32,34,20,13,21,0,0,0,0,5,16,34,34,34,25,9,8,
37,34,34,34,34,34,34,34,34,25,0,0,0,25,34,34,34,34,32,14,15,25,12,0,0,0,0,7,13,34,34,34,20,2,0,0,0,0,0,0,0,3,18,34,34,34,16,2,5,16,34,34,34,18,3,0,7,13,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,34,34,34,34,34,43,1,0,0,0,25,34,34,34,34,34,34,34,34,37,8,0,0,0,0,0,1,43,34,34,34,18,23,34,34,34,14,12,0,0,0,0,0,11,34,34,34,14,10,0,0,0,0,0,0,9,22,15,32,34,34,32,22,0,0,3,22,22,22,4,0,0,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,
12,0,7,13,34,34,34,34,34,32,20,18,26,0,0,7,13,34,34,34,20,2,0,0,0,0,0,4,15,34,34,34,34,34,34,34,34,34,34,34,20,12,0,7,13,34,34,34,34,34,34,34,34,43,1,0,22,32,34,34,34,34,34,34,34,34,34,43,1,7,13,34,34,34,20,2,5,16,34,34,34,18,3,7,13,34,34,34,20,2,0,7,13,34,34,34,20,2,5,37,34,34,34,34,25,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,0,7,13,34,34,34,37,8,0,0,0,25,34,34,34,16,5,7,13,34,34,34,37,8,22,32,34,34,34,18,3,0,1,12,12,12,5,0,0,
0,8,12,12,12,2,0,0,2,12,12,12,4,0,0,0,0,0,3,12,12,12,12,9,0,0,0,0,0,0,0,0,0,2,12,12,12,12,0,0,0,0,0,0,0,0,0,0,2,12,12,12,2,0,0,12,12,12,12,5,0,0,0,0,0,8,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,0,0,0,0,12,12,12,12,1,0,0,0,0,0,0,7,12,12,12,12,12,12,12,12,12,12,4,0,0,0,0,0,8,12,12,12,12,7,0,0,4,5,5,5,3,0,0,0,2,12,12,12,12,12,12,12,12,4,0,0,0,0,0,7,12,12,12,12,7,0,0,0,0,0,0,0,0,0,6,
12,12,4,7,0,0,0,0,0,0,0,0,0,0,6,12,12,4,7,0,0,0,0,0,0,5,12,12,12,42,0,0,4,12,12,12,12,12,12,12,12,1,0,0,0,1,12,12,12,12,12,5,42,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,8,12,12,12,5,0,0,5,12,12,12,8,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,12,12,12,12,12,4,0,0,0,0,1,12,12,12,12,12,12,12,12,4,0,0,0,0,0,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,0,0,0,3,12,12,12,12,9,0,0,0,0,0,0,
0,0,8,12,12,12,12,7,0,0,0,0,0,0,0,0,0,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,8,12,12,12,12,12,12,4,7,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,5,12,12,12,12,12,12,12,12,12,12,12,2,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,7,12,12,12,12,12,12,12,12,12,12,4,0,0,8,12,12,12,2,0,0,5,12,12,12,8,0,0,8,12,12,12,2,0,0,0,8,12,12,12,2,0,0,4,12,12,12,12,1,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,8,12,12,12,4,
0,0,0,0,1,12,12,12,5,0,0,8,12,12,12,4,0,7,12,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,7,0,0,0,19,19,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,0,0,0,7,19,19,19,19,19,19,19,19,19,19,9,0,0,0,3,19,19,19,9,0,0,0,19,19,19,19,0,0,0,0,0,0,0,0,3,19,19,19,0,0,0,0,0,0,0,0,19,19,19,3,0,0,0,3,19,19,19,0,0,0,0,3,19,19,19,3,19,19,19,3,19,19,19,0,0,0,
0,0,0,0,0,1,4,4,8,9,0,0,0,0,0,0,0,1,1,7,0,0,0,0,0,3,3,0,0,0,0,0,0,19,19,19,19,19,19,19,19,0,0,0,0,3,3,7,0,0,0,0,0,0,0,0,0,0,9,2,5,1,0,0,0,0,0,0,0,0,0,0,1,4,4,3,0,0,0,0,8,5,8,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,19,19,19,19,9,0,0,0,19,19,19,19,7,0,0,0,19,19,19,7,0,0,0,0,0,0,9,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,19,19,19,7,0,0,0,0,0,0,
0,0,7,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,3,19,19,19,3,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,3,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,9,0,0,0,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,19,0,0,0,19,19,19,3,0,0,7,19,19,19,7,0,0,0,0,19,19,19,7,0,0,19,19,19,19,0,0,0,0,19,19,19,7,0,0,9,19,19,19,19,0,0,0,0,0,0,9,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,19,9,0,0,0,19,
19,19,19,7,0,0,0,19,19,19,7,0,0,7,19,19,19,0,0,0,0,19,19,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,9,0,0,0,0,0,0,22,37,37,37,37,37,37,37,25,19,0,11,37,37,37,37,37,37,37,37,37,37,11,9,0,11,37,37,37,37,37,37,13,10,1,0,0,0,11,37,37,37,37,37,37,37,37,23,0,1,13,37,37,37,37,37,37,37,37,37,37,24,9,0,2,43,37,37,37,25,3,0,17,37,37,37,37,22,0,0,0,0,0,0,12,37,37,37,37,10,0,0,0,0,0,
0,22,37,37,37,37,12,0,2,43,37,37,37,11,0,0,12,43,37,37,37,43,37,37,37,37,37,37,37,10,0,0,0,0,0,12,25,37,20,14,15,13,26,9,0,0,0,9,11,15,37,18,22,4,0,12,24,37,43,22,9,0,0,0,11,37,37,37,37,37,37,37,37,23,0,0,5,43,37,43,24,21,0,0,0,0,0,0,0,0,22,14,14,37,24,2,0,0,0,0,0,0,0,12,37,14,20,18,21,0,8,24,16,14,16,23,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,11,37,37,37,37,24,7,0,10,37,37,37,37,13,8,0,11,37,37,37,18,4,0,0,0,0,3,25,37,
37,37,37,37,37,37,37,23,0,0,0,0,0,0,0,11,37,37,37,18,4,11,37,37,37,18,4,0,0,0,0,0,0,1,13,37,37,37,37,37,37,37,37,37,22,0,0,0,0,0,0,0,0,4,18,37,37,37,43,12,0,0,0,0,0,0,11,37,37,37,37,37,37,37,37,37,12,0,0,11,37,37,37,37,37,43,25,10,19,0,0,0,11,37,37,37,37,37,37,37,37,24,7,0,11,37,37,37,37,37,37,37,37,23,0,0,21,37,37,37,37,22,0,22,37,37,37,43,2,1,25,37,37,37,13,42,0,0,11,37,37,37,25,19,21,37,37,37,37,11,9,0,11,37,37,37,18,4,3,24,
37,37,37,37,21,0,0,0,0,3,25,37,37,37,37,37,37,37,37,23,0,0,11,37,37,37,37,24,7,0,10,37,37,37,37,13,8,0,11,37,37,37,18,4,6,13,37,37,37,11,9,0,11,37,37,37,37,37,37,37,37,37,37,11,9,0,11,37,37,37,37,37,37,13,10,1,0,0,1,13,37,37,37,37,37,37,37,37,37,37,24,9,0,0,0,0,7,18,41,36,36,36,36,36,36,14,5,19,37,36,36,36,36,36,36,36,36,36,36,15,1,19,37,36,36,36,36,36,41,35,29,13,6,0,19,37,36,36,36,36,36,36,36,41,43,7,2,14,36,36,36,36,36,36,36,36,36,36,15,1,
0,17,29,36,36,36,28,17,6,43,35,36,36,35,18,9,0,0,0,0,8,22,40,36,36,35,25,5,0,0,0,0,7,18,35,36,36,27,13,19,25,33,36,36,44,37,3,0,10,39,36,36,44,40,36,36,35,33,36,36,35,25,0,0,0,0,10,20,33,36,30,30,31,35,32,11,3,0,0,1,37,36,36,44,39,43,22,14,27,41,41,32,10,0,0,19,37,36,36,36,36,36,36,36,41,43,7,0,26,29,36,41,27,14,26,0,0,0,0,0,0,0,11,27,30,36,33,15,17,0,0,0,0,0,0,22,40,30,38,35,15,2,25,39,31,30,41,18,7,19,37,36,36,36,28,12,3,0,0,0,
0,0,19,37,36,36,36,36,28,22,2,15,44,36,36,36,34,12,19,37,36,36,36,28,12,0,0,0,0,4,20,36,36,36,36,36,36,36,41,43,3,0,0,0,0,0,19,37,36,36,36,28,12,37,36,36,36,28,12,0,0,0,0,0,0,2,14,36,36,36,36,36,36,36,36,44,13,0,0,0,0,0,0,0,0,10,29,36,36,36,33,23,0,0,0,0,0,19,37,36,36,36,36,36,36,36,36,33,22,0,19,37,36,36,36,36,36,41,35,29,25,19,0,19,37,36,36,36,36,36,36,36,36,16,6,19,37,36,36,36,36,36,36,36,41,43,7,0,23,33,36,36,41,20,21,13,44,36,36,39,
10,11,39,36,36,36,14,12,0,19,37,36,36,36,14,5,13,27,36,36,36,15,1,19,37,36,36,36,28,12,23,29,36,36,36,33,23,0,0,0,0,4,20,36,36,36,36,36,36,36,41,43,3,19,37,36,36,36,36,14,2,3,18,44,36,36,36,34,12,19,37,36,36,36,28,12,12,32,36,36,36,15,1,19,37,36,36,36,36,36,36,36,36,36,36,15,1,19,37,36,36,36,36,36,41,35,29,13,6,0,2,14,36,36,36,36,36,36,36,36,36,36,15,1,0,0,0,0,7,18,41,52,52,52,52,52,30,14,5,19,37,36,51,51,52,52,52,52,51,51,31,15,1,19,37,36,51,51,52,52,
51,53,30,39,11,0,19,37,36,51,51,52,52,52,52,41,43,7,2,14,46,52,52,52,51,51,52,52,52,31,15,1,0,1,43,35,50,50,44,37,22,39,46,51,38,32,26,0,0,0,0,17,37,29,27,49,53,35,39,16,22,3,0,0,0,26,32,38,51,30,29,10,32,38,51,30,28,10,0,0,10,39,48,52,44,40,49,53,35,33,49,53,35,25,0,0,0,21,20,41,48,51,50,50,52,50,31,28,22,0,0,1,37,36,52,53,47,33,14,36,49,52,50,35,13,9,0,19,37,36,51,51,52,52,52,52,41,43,7,0,26,29,45,52,49,35,13,0,0,3,8,0,0,0,11,27,45,52,45,44,
14,17,0,0,0,0,0,22,40,49,51,45,33,25,32,38,52,52,41,18,7,19,37,36,51,47,28,37,43,25,10,19,0,0,19,37,36,51,51,50,41,16,13,27,45,51,51,46,34,12,19,37,36,51,47,28,12,0,0,0,0,4,20,30,51,51,52,52,51,51,41,43,3,0,0,0,0,0,19,37,36,51,47,28,12,37,36,51,47,28,12,0,0,0,0,0,0,2,14,46,52,51,51,51,52,52,52,44,13,0,0,0,0,0,0,0,9,13,35,50,51,52,44,43,19,0,0,0,0,19,37,36,51,51,52,52,52,52,49,33,22,0,19,37,36,51,52,50,50,52,50,30,29,10,0,19,37,36,51,51,
52,52,52,52,31,16,6,19,37,36,51,51,52,52,52,52,41,43,7,0,4,43,27,48,49,35,15,13,44,52,48,39,22,28,31,52,30,29,11,9,0,19,37,36,51,30,14,21,14,38,51,51,31,15,1,19,37,36,51,47,28,22,32,36,53,45,27,43,4,0,0,0,0,4,20,30,51,51,52,52,51,51,41,43,3,19,37,36,51,51,48,40,23,12,32,30,51,51,46,34,12,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,51,52,52,52,52,51,51,31,15,1,19,37,36,51,51,52,52,51,53,30,39,11,0,2,14,46,52,52,52,51,51,52,52,52,31,15,1,0,0,0,0,
7,18,41,41,41,41,41,41,41,14,5,19,37,36,51,47,41,41,41,41,46,51,31,15,1,19,37,36,51,47,44,44,36,50,52,36,15,8,19,37,36,53,46,41,41,41,41,41,43,7,2,14,41,41,41,41,53,49,41,41,41,41,15,1,0,0,26,32,38,51,46,39,16,41,53,48,33,25,7,0,0,0,17,14,44,47,50,51,51,50,45,36,28,23,0,0,0,7,25,33,45,53,44,37,35,50,48,27,13,3,0,0,10,39,48,52,44,40,49,53,35,33,49,53,35,25,0,0,3,18,35,49,52,38,27,27,36,50,53,41,15,4,0,1,37,44,41,47,51,30,27,49,49,38,53,41,43,7,0,19,
37,36,51,47,41,41,41,41,41,43,7,0,26,29,27,46,52,44,18,1,17,18,16,26,0,0,22,32,40,36,49,49,44,15,5,0,0,0,0,22,40,36,46,51,38,32,35,50,49,36,41,18,7,19,37,36,51,47,36,36,41,35,29,25,42,0,19,37,36,51,52,51,48,27,29,30,51,53,54,46,34,12,19,37,36,51,47,28,12,0,0,0,0,4,20,30,51,30,41,41,45,51,41,43,3,0,0,0,0,0,19,37,36,51,47,28,12,37,36,51,47,28,12,0,0,0,0,0,0,2,14,41,41,36,51,47,41,41,41,44,13,0,0,0,0,0,0,0,4,20,31,51,45,52,30,32,12,0,0,0,
0,19,37,36,51,47,41,41,41,41,41,33,22,0,19,37,36,51,47,27,27,36,53,50,35,25,0,19,37,36,51,47,41,41,41,41,41,16,6,19,37,36,51,47,41,41,41,41,41,43,7,0,0,12,16,44,49,45,27,18,44,52,48,39,14,41,53,47,33,13,1,0,0,19,37,36,51,30,14,25,27,49,54,51,31,15,1,19,37,36,51,47,28,14,41,50,48,35,37,2,0,0,0,0,0,4,20,30,51,30,41,41,45,51,41,43,3,19,37,36,51,52,52,41,37,11,33,48,53,54,46,34,12,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,41,41,41,41,46,51,31,15,
1,19,37,36,51,47,44,44,36,50,52,36,15,8,2,14,41,41,41,41,53,49,41,41,41,41,15,1,0,0,0,0,0,22,43,43,43,43,43,43,43,24,3,19,37,36,51,47,28,43,43,32,46,51,31,15,1,19,37,36,51,47,28,13,32,30,54,30,14,2,0,11,29,31,53,31,29,43,43,43,22,0,19,25,43,43,43,35,53,49,33,43,43,43,23,0,0,0,7,25,33,48,53,41,33,45,52,36,20,12,0,0,0,6,37,44,49,52,47,50,52,46,53,53,36,14,12,0,0,0,2,16,41,53,45,40,47,52,36,20,12,0,0,0,10,39,48,52,44,40,49,53,35,33,49,53,35,25,0,0,
12,32,30,51,47,39,25,11,32,38,51,45,39,22,0,0,23,18,37,40,46,48,31,50,44,33,30,33,11,0,0,19,37,36,51,47,28,43,43,43,43,22,0,0,19,22,14,30,49,33,11,37,29,35,40,22,0,0,9,12,23,20,44,50,45,33,24,0,0,0,0,21,37,37,27,49,50,35,47,52,36,20,15,23,0,19,37,36,51,51,52,52,52,50,30,39,23,0,19,37,36,51,30,46,51,46,36,53,49,44,52,46,34,12,19,37,36,51,47,28,12,0,0,0,0,4,20,30,51,30,14,29,45,51,41,43,10,21,6,0,0,0,19,37,36,51,47,28,22,37,36,51,47,28,22,17,5,0,0,0,
0,19,25,43,37,36,51,47,28,43,43,43,10,5,0,0,0,0,0,0,10,29,47,50,35,49,48,33,23,0,0,0,0,19,37,36,51,47,28,43,43,43,43,18,12,0,19,37,36,51,47,28,24,39,48,52,44,13,0,19,37,36,51,47,28,43,43,43,43,11,9,19,37,36,51,47,28,43,43,43,43,22,0,0,0,0,17,14,36,53,46,40,44,52,48,39,35,49,48,27,43,4,0,0,0,19,37,36,51,30,14,14,31,50,45,51,31,15,1,19,37,36,51,47,28,44,49,49,44,16,21,0,0,0,0,0,0,4,20,30,51,30,14,29,45,51,41,43,3,19,37,36,51,30,47,46,28,15,36,50,
44,52,46,34,12,19,37,36,51,47,28,22,22,32,46,51,31,15,1,19,37,36,51,47,28,43,43,32,46,51,31,15,1,19,37,36,51,47,28,13,32,30,54,30,14,2,19,25,43,43,43,35,53,49,33,43,43,43,23,0,0,0,0,0,0,4,16,40,40,40,40,40,29,17,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,22,14,30,51,46,32,12,0,3,23,34,36,50,38,39,13,4,0,0,0,0,7,7,25,35,53,49,33,22,7,7,0,0,0,0,0,2,16,41,53,45,31,51,47,39,23,0,0,0,0,26,29,46,51,46,40,49,53,35,31,52,48,33,23,0,
0,0,0,22,29,46,52,30,52,47,39,23,0,0,0,0,10,39,45,52,44,40,49,53,35,33,50,50,27,24,0,0,22,39,48,51,36,37,8,7,13,35,53,50,27,25,0,0,0,7,4,43,44,52,50,46,29,18,20,43,5,0,0,19,37,36,51,47,40,40,40,40,39,23,0,0,0,26,29,47,47,29,39,44,46,30,32,12,0,0,0,0,12,14,36,45,52,31,20,5,0,0,0,0,1,12,14,36,52,48,52,47,39,11,42,9,0,19,37,36,51,47,44,44,31,50,52,36,15,6,19,37,36,51,30,27,48,52,53,53,36,44,52,46,34,12,19,37,36,51,47,28,12,0,0,0,0,4,20,30,
51,30,20,29,45,51,41,40,39,28,37,10,0,0,19,37,36,51,47,40,40,40,36,51,47,40,40,29,20,24,8,0,0,0,0,3,37,36,51,47,40,40,40,40,39,16,10,0,0,0,0,9,13,35,50,45,39,46,52,44,43,19,0,0,0,19,37,36,51,47,40,40,39,28,37,22,7,0,19,37,36,51,47,39,39,35,50,49,27,24,0,19,37,36,51,47,28,12,7,7,7,0,0,19,37,36,51,47,40,40,40,40,39,23,0,0,0,0,0,22,28,38,52,30,44,52,48,27,45,49,44,15,12,0,0,0,0,19,37,36,51,30,14,33,48,30,30,51,31,15,1,19,37,36,51,47,35,48,50,41,
14,26,0,0,0,0,0,0,0,4,20,30,51,30,20,29,45,51,41,43,3,19,37,36,51,30,36,49,27,29,47,46,44,52,46,34,12,19,37,36,51,47,40,40,40,40,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,22,14,30,51,46,32,12,0,0,7,7,25,35,53,49,33,22,7,7,0,0,0,0,0,0,0,4,20,30,49,49,49,48,39,17,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,40,40,35,48,51,31,20,4,0,0,7,22,14,41,50,30,40,11,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,
0,0,0,0,23,39,46,51,52,50,44,37,8,0,0,0,0,22,33,49,52,44,40,49,53,35,27,50,53,35,13,0,0,0,0,1,18,35,49,54,50,44,37,8,0,0,0,0,12,28,46,51,31,40,49,53,35,44,53,48,33,23,0,0,23,33,49,52,44,13,9,0,23,33,49,53,44,13,0,0,0,0,0,11,27,50,51,31,15,8,4,1,0,0,0,19,37,36,51,53,49,49,49,49,27,11,0,0,0,23,33,49,38,36,47,53,52,41,37,1,0,0,0,1,18,27,49,27,45,45,39,22,0,0,0,0,0,0,11,39,47,54,50,35,37,8,0,0,0,19,37,36,51,47,28,13,34,30,54,30,
14,2,19,37,36,51,30,14,41,53,51,30,29,44,52,46,34,12,19,37,36,51,47,28,12,0,0,0,0,5,14,30,51,38,20,29,45,51,49,49,48,46,44,14,17,0,19,37,36,51,53,49,49,49,49,54,53,49,48,45,31,39,25,7,0,0,0,3,37,36,51,53,49,49,49,49,45,44,20,2,0,0,0,4,20,31,51,38,20,36,52,30,32,12,0,0,0,19,37,36,51,53,49,49,48,46,41,34,22,0,19,37,36,51,50,48,48,50,48,44,14,21,0,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,53,49,49,49,49,27,11,0,0,0,0,0,21,14,31,52,53,30,52,49,46,
51,49,35,43,1,0,0,0,0,19,37,36,51,30,14,31,50,35,30,51,31,15,1,19,37,36,51,45,45,51,49,35,43,6,0,0,0,0,0,0,0,5,14,30,51,38,20,29,45,51,41,43,3,19,37,36,51,30,27,49,31,35,50,41,44,52,46,34,12,19,37,36,51,53,49,49,49,49,50,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,40,40,35,48,51,31,20,4,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,0,4,20,30,47,47,47,47,39,17,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,53,49,49,
50,51,45,27,25,9,0,0,3,24,29,30,50,36,14,10,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,6,43,35,50,51,30,28,17,0,0,0,0,0,22,33,49,52,44,40,49,53,35,33,49,53,35,13,0,0,0,0,19,13,27,49,54,50,35,43,42,0,0,0,0,8,15,36,52,48,27,49,53,35,46,51,30,34,21,0,0,22,33,49,52,44,18,7,0,11,27,50,53,35,13,0,0,0,0,0,22,39,48,52,41,43,3,0,0,0,0,0,19,37,36,51,49,47,47,47,47,27,11,0,0,9,13,35,53,53,52,50,53,49,27,24,0,0,0,0,23,29,46,36,44,
50,53,35,13,9,0,0,0,0,19,13,27,49,51,52,36,20,12,0,0,0,19,37,36,51,47,28,22,14,30,51,46,32,12,19,37,36,51,30,14,28,38,38,33,18,44,52,46,34,12,19,37,36,51,47,28,12,0,0,0,0,12,32,46,51,31,15,29,45,51,47,47,48,51,50,35,18,3,19,37,36,51,49,47,47,47,45,51,49,47,45,53,52,38,14,12,0,0,0,3,37,36,51,49,47,47,47,49,54,48,40,22,0,0,0,10,29,47,53,44,18,27,50,48,33,23,0,0,0,19,37,36,51,49,47,47,48,52,50,44,37,1,19,37,36,51,48,30,30,46,53,47,27,13,7,19,37,36,51,47,
28,12,0,0,0,0,0,19,37,36,51,49,47,47,47,47,27,11,0,0,0,0,1,18,35,49,53,48,51,54,54,52,45,51,46,29,22,0,0,0,0,19,37,36,51,30,33,48,30,28,30,51,31,15,1,19,37,36,51,54,52,47,51,46,39,23,0,0,0,0,0,0,0,12,32,46,51,31,15,29,45,51,41,43,3,19,37,36,51,30,34,30,45,38,48,33,44,52,46,34,12,19,37,36,51,49,47,47,47,47,49,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,53,49,49,50,51,45,27,25,9,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,
0,8,37,28,28,28,28,28,14,21,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,48,46,46,30,36,33,37,2,0,0,9,23,28,31,53,36,34,22,0,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,0,17,39,48,52,44,18,3,0,0,0,0,0,10,39,45,51,31,40,49,53,35,44,53,50,27,24,0,0,0,0,22,29,30,51,47,52,46,39,23,0,0,0,0,0,11,40,47,51,49,52,51,49,51,49,35,18,19,0,0,26,29,47,51,31,20,5,1,43,44,53,49,33,11,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,19,
37,36,51,47,28,28,28,28,28,22,0,0,6,15,36,52,48,38,35,45,46,29,26,0,0,0,12,20,36,46,40,47,41,53,36,37,1,0,0,0,0,22,29,30,51,30,50,48,27,13,3,0,0,19,37,36,51,47,40,40,35,48,51,31,20,4,19,37,36,51,30,14,25,20,20,37,18,44,52,46,34,12,19,37,36,51,47,28,12,0,0,0,0,21,29,47,51,41,37,29,45,51,41,28,33,47,51,38,20,4,19,37,36,51,47,28,28,28,36,51,47,28,29,36,51,48,39,22,0,0,0,3,37,36,51,47,28,28,28,35,53,53,35,25,0,0,9,13,35,50,48,33,33,33,47,52,44,43,19,0,
0,19,37,36,51,47,28,28,40,46,51,30,14,2,19,37,36,51,47,28,14,29,30,51,38,20,4,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,28,28,28,28,28,22,0,0,0,0,22,29,46,51,30,35,48,54,51,46,27,48,53,41,16,2,0,0,0,19,37,36,51,30,36,50,35,20,30,51,31,15,1,19,37,36,51,51,30,33,48,52,36,20,12,0,0,0,0,0,0,21,29,47,51,41,37,29,45,51,41,43,3,19,37,36,51,30,14,44,53,53,30,14,44,52,46,34,12,19,37,36,51,47,28,28,28,28,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,
1,19,37,36,51,48,46,46,30,36,33,37,2,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,0,12,23,23,23,23,23,23,23,17,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,32,14,15,25,5,0,0,0,22,32,36,53,38,29,23,23,23,21,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,5,20,36,52,50,31,49,53,41,48,51,30,28,26,0,0,0,5,15,44,53,45,33,47,53,41,20,2,0,0,0,0,4,37,27,47,52,51,54,52,48,44,20,21,0,0,0,
12,15,41,53,45,33,13,11,29,46,52,38,14,26,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,19,37,36,51,47,28,21,21,21,21,3,0,0,12,32,46,36,33,14,27,50,31,16,12,0,0,3,18,27,46,35,31,30,40,48,38,20,4,0,0,1,22,15,44,53,45,33,38,51,30,28,23,12,0,19,37,36,51,50,48,48,50,51,45,27,25,9,19,37,36,51,30,14,5,4,4,42,18,44,52,46,34,12,19,37,36,51,47,28,12,0,0,4,23,43,27,49,53,35,13,29,45,51,41,43,20,30,51,30,14,4,19,37,36,51,47,28,21,37,36,51,47,28,25,35,53,49,33,22,
0,0,0,3,37,36,51,47,28,21,22,33,49,53,35,13,0,0,4,20,31,51,52,50,50,50,53,51,30,32,12,0,0,19,37,36,51,47,28,21,15,31,51,46,32,12,19,37,36,51,47,28,21,15,31,51,46,32,12,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,28,23,23,23,23,21,0,0,0,2,16,41,53,48,27,20,41,52,48,33,20,36,52,48,33,25,7,0,0,19,37,36,51,46,45,46,29,20,30,51,31,15,1,19,37,36,51,47,40,16,41,53,48,27,13,19,0,0,0,4,23,18,27,49,53,35,13,29,45,51,41,43,3,19,37,36,51,30,14,39,45,53,44,18,
44,52,46,34,12,19,37,36,51,47,28,21,21,32,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,32,14,15,25,5,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,7,18,33,33,33,33,33,33,33,20,4,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,2,8,0,0,0,0,19,37,41,50,30,33,33,33,33,33,15,6,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,11,29,31,50,51,51,54,51,53,30,33,13,19,0,
0,9,24,40,45,53,41,16,44,50,48,33,25,7,0,0,0,0,12,37,40,44,49,53,44,27,20,10,0,0,0,7,43,33,33,36,53,30,39,32,31,53,31,33,33,20,4,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,17,29,39,15,11,37,36,51,36,29,20,5,0,7,13,34,34,14,34,34,29,45,30,20,4,0,0,10,29,27,45,52,41,33,33,48,53,44,33,13,9,19,37,36,51,48,46,46,46,31,33,37,12,0,19,37,36,51,30,14,5,0,0,9,18,44,52,46,34,12,19,37,36,51,47,28,12,0,0,23,40,44,47,51,
48,40,22,29,45,51,41,29,33,47,51,31,16,8,19,37,36,51,47,28,12,37,36,51,47,29,39,36,51,45,39,10,0,0,0,3,37,36,51,47,28,21,22,40,49,53,35,13,0,0,10,29,47,52,30,30,30,30,30,50,48,33,23,0,0,19,37,36,51,47,29,29,40,46,51,30,14,5,19,37,36,51,47,29,29,33,46,51,30,14,5,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,47,33,33,33,33,33,15,6,0,7,25,33,45,52,36,20,13,44,52,48,39,23,39,47,51,38,34,17,0,0,19,37,36,51,51,53,44,43,20,30,51,31,15,1,19,37,36,51,47,28,22,29,46,
51,30,29,22,0,0,0,23,40,35,46,51,48,40,22,29,45,51,41,43,3,19,37,36,51,30,14,20,31,31,39,18,44,52,46,34,12,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,2,8,0,0,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,7,18,41,49,49,49,49,49,30,14,5,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,0,0,0,0,0,19,37,36,51,50,49,49,49,49,31,16,6,0,0,0,0,25,35,53,49,33,22,0,0,0,0,
0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,19,24,32,27,36,49,52,36,35,29,13,5,0,0,0,21,14,31,52,46,29,22,28,30,51,30,34,17,0,0,0,0,0,4,23,40,49,53,35,25,12,0,0,0,0,7,43,41,49,49,51,48,39,32,46,54,50,49,30,20,4,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,1,23,22,4,1,15,36,52,50,46,32,12,0,0,8,12,12,2,12,12,28,46,30,20,4,0,0,10,39,48,51,51,49,49,49,52,54,53,44,13,9,19,37,36,51,47,28,32,32,16,25,2,
0,0,19,37,36,51,30,14,5,0,0,9,18,44,52,46,34,12,19,37,36,51,47,28,12,0,0,11,27,50,52,48,41,20,21,29,45,51,45,47,48,51,48,27,13,9,19,37,36,51,47,28,12,37,36,51,49,47,45,53,53,36,20,2,0,0,0,3,37,36,51,47,28,21,22,40,49,53,35,13,0,9,13,35,50,49,33,14,14,14,39,45,52,44,43,19,0,19,37,36,51,49,47,47,48,52,48,35,18,3,19,37,36,51,49,47,47,48,51,49,44,43,19,19,37,36,51,47,28,12,0,0,0,0,0,19,37,36,51,53,49,49,49,49,31,16,6,0,26,32,38,51,47,39,11,13,44,52,48,39,
10,37,35,50,50,35,37,8,0,19,37,36,51,54,46,29,22,20,30,51,31,15,1,19,37,36,51,47,28,12,18,27,49,53,41,15,5,0,0,11,27,50,52,48,41,20,21,29,45,51,41,43,3,19,37,36,51,30,14,24,15,15,43,18,44,52,46,34,12,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,12,32,46,51,31,15,1,19,37,36,51,47,28,12,0,0,0,0,0,0,0,0,0,0,25,35,53,49,33,22,0,0,0,0,0,0,0,0,7,18,41,46,46,46,46,46,30,14,5,19,37,36,46,46,28,12,12,32,46,46,31,15,1,19,37,36,46,46,28,12,
0,0,0,0,0,0,19,37,36,46,46,46,46,46,46,31,16,6,0,0,0,0,25,35,46,46,33,22,0,0,0,0,0,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,0,0,0,7,26,25,40,46,46,35,13,10,1,0,0,0,3,18,35,46,46,35,18,1,13,27,46,46,44,37,1,0,0,0,0,0,22,40,46,46,35,25,0,0,0,0,0,7,43,41,46,46,46,46,39,32,46,46,46,46,30,20,4,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,0,19,37,36,46,46,28,12,0,0,0,0,0,0,0,0,0,0,0,24,39,36,30,46,32,12,0,0,0,0,0,0,0,2,
32,30,31,16,8,0,0,10,39,46,46,46,46,46,46,46,46,46,44,13,9,19,37,36,46,46,28,12,12,4,9,0,0,0,19,37,36,46,30,14,5,0,0,9,18,44,46,46,34,12,19,37,36,46,46,28,12,0,0,11,27,30,41,33,20,10,21,29,46,46,46,46,46,31,27,15,12,0,19,37,36,46,46,28,12,37,36,46,46,46,46,30,44,28,23,0,0,0,0,3,37,36,46,46,28,21,22,40,46,46,35,13,0,4,20,31,46,30,32,21,4,5,20,31,46,30,32,12,0,19,37,36,46,46,46,46,46,31,27,20,21,0,19,37,36,46,46,46,46,46,38,35,14,17,0,19,37,36,46,46,
28,12,0,0,0,0,0,19,37,36,46,46,46,46,46,46,31,16,6,1,43,35,46,46,44,37,8,13,44,46,46,39,10,26,32,38,46,46,39,23,0,19,37,36,46,46,44,37,4,20,30,46,31,15,1,19,37,36,46,46,28,12,12,14,36,46,46,33,25,0,0,11,27,30,41,33,20,10,21,29,46,46,41,43,3,19,37,36,46,30,14,5,1,1,19,18,44,46,46,34,12,19,37,36,46,46,28,12,12,32,46,46,31,15,1,19,37,36,46,46,28,12,12,32,46,46,31,15,1,19,37,36,46,46,28,12,0,0,0,0,0,0,0,0,0,0,25,35,46,46,33,22,0,0,0,0,0,0,0,0,
0,25,34,34,34,34,34,34,34,37,8,7,13,34,34,34,20,2,5,16,34,34,34,18,3,7,13,34,34,34,20,2,0,0,0,0,0,0,7,13,34,34,34,34,34,34,34,34,43,1,0,0,0,0,11,34,34,34,14,10,0,0,0,0,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,0,0,0,0,17,14,34,34,34,11,0,0,0,0,0,3,18,34,34,34,14,21,0,12,16,34,34,34,43,1,0,0,0,0,0,17,14,34,34,34,11,0,0,0,0,0,9,25,34,34,34,34,34,14,16,34,34,34,34,34,37,42,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,7,
13,34,34,34,20,2,0,0,0,0,0,0,0,0,0,0,0,6,24,15,14,32,15,4,0,0,0,0,0,0,0,4,37,14,14,13,3,0,0,21,14,34,34,34,34,34,34,34,34,34,34,24,0,7,13,34,34,34,20,2,0,0,0,0,0,0,7,13,34,34,34,37,8,0,0,0,25,34,34,34,16,5,7,13,34,34,34,20,2,0,0,22,32,14,37,24,12,0,12,20,34,34,34,34,32,20,25,12,0,0,7,13,34,34,34,20,2,13,34,34,34,34,34,14,43,10,7,0,0,0,0,7,13,34,34,34,20,12,17,14,34,34,34,11,0,4,15,34,34,34,43,1,0,7,13,34,34,34,20,12,
0,7,13,34,34,34,34,34,32,20,13,12,0,0,7,13,34,34,34,34,34,32,20,18,26,0,0,7,13,34,34,34,20,2,0,0,0,0,0,7,13,34,34,34,34,34,34,34,34,43,1,1,43,34,34,34,14,17,0,24,34,34,34,14,21,7,25,34,34,34,34,23,0,7,13,34,34,34,32,22,42,37,34,34,34,18,3,7,13,34,34,34,20,2,0,11,34,34,34,34,25,0,0,22,32,14,37,24,12,0,12,20,34,34,34,13,9,7,13,34,34,34,37,8,0,0,0,25,34,34,34,16,5,7,13,34,34,34,20,2,5,16,34,34,34,18,3,7,13,34,34,34,20,2,5,16,34,34,34,18,
3,7,13,34,34,34,20,2,0,0,0,0,0,0,0,0,0,0,11,34,34,34,14,10,0,0,0,0,0,0,0,0,0,1,12,12,12,12,12,12,12,4,0,0,8,12,12,12,2,0,0,5,12,12,12,8,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,0,0,0,3,12,12,12,12,9,0,0,0,0,0,0,0,0,0,0,12,12,12,12,1,0,0,0,0,0,0,0,0,0,0,9,12,12,12,12,19,0,0,0,0,0,0,8,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,0,0,0,0,12,12,12,12,19,0,0,0,0,0,0,
42,12,12,12,12,12,12,5,12,12,12,12,12,4,0,0,0,0,0,0,12,12,12,12,1,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,5,12,4,0,0,0,0,0,0,0,0,0,6,4,4,19,0,0,0,0,12,12,12,12,12,12,12,12,12,12,12,1,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,8,12,12,12,4,0,0,0,0,1,12,12,12,5,0,0,8,12,12,12,2,0,0,0,7,12,5,1,0,0,0,0,2,12,12,12,12,12,4,9,0,0,0,0,8,12,12,12,2,0,8,12,12,12,12,12,5,19,0,0,0,
0,0,0,0,6,12,12,12,2,0,0,12,12,12,12,19,0,0,5,12,12,12,4,0,0,0,6,12,12,12,2,0,0,0,8,12,12,12,12,12,12,4,7,0,0,0,0,8,12,12,12,12,12,12,4,7,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,4,12,12,12,12,0,0,1,12,12,12,12,0,0,1,12,12,12,12,3,0,0,8,12,12,12,12,7,0,4,12,12,12,8,0,0,8,12,12,12,2,0,0,3,12,12,12,12,1,0,0,7,12,5,1,0,0,0,0,2,12,12,12,42,0,0,8,12,12,12,4,0,0,0,0,1,
12,12,12,5,0,0,8,12,12,12,2,0,0,5,12,12,12,8,0,0,8,12,12,12,2,0,0,5,12,12,12,8,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,3,12,12,12,12,9,0,0,0,0,0,0,0,0,0,0,19,19,19,3,0,0,0,3,19,19,19,0,0,0,0,0,0,0,0,0,3,19,19,19,0,0,0,0,0,0,0,0,0,19,19,19,3,0,0,0,3,19,19,19,0,0,0,9,19,19,19,7,0,0,19,19,19,19,0,0,0,0,19,19,19,7,0,0,19,19,19,3,0,0,3,19,19,19,0,0,0,0,19,19,19,19,19,19,19,7,0,0,0,0,
0,0,0,0,0,19,19,19,7,0,0,0,0,0,0,19,19,19,7,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,0,0,3,19,19,19,19,19,9,0,0,0,0,0,42,42,42,42,42,42,42,42,0,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,0,0,7,42,42,42,7,9,42,42,42,3,0,42,42,42,19,0,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,0,0,0,3,19,19,19,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,19,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,9,0,0,0,0,0,0,0,0,0,19,
19,19,7,0,0,0,0,0,3,19,19,19,7,0,0,0,0,0,0,0,0,0,7,19,19,19,19,19,19,19,19,19,19,3,0,0,0,0,19,19,19,7,3,19,19,19,19,19,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,7,19,19,19,0,0,0,0,0,0,0,0,0,0,7,19,19,19,9,0,0,0,0,9,8,5,4,3,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,3,0,0,0,0,
42,42,42,1,0,1,42,42,42,0,3,42,42,42,3,0,0,0,7,42,42,42,7,0,42,42,42,42,0,0,0,0,19,19,19,7,0,0,9,19,19,19,19,0,0,0,0,0,19,19,19,7,0,0,9,19,19,19,19,0,0,0,0,19,19,19,19,19,19,19,7,0,0,7,19,19,19,3,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,0,0,0,0,22,37,37,37,37,12,0,2,43,37,37,37,11,0,0,0,0,0,0,0,12,43,37,37,37,10,0,0,0,0,0,0,0,22,37,37,37,37,12,0,2,43,37,37,37,11,0,3,25,37,37,37,25,19,17,37,37,37,37,21,0,0,11,
37,37,37,18,4,10,37,37,37,37,12,12,37,37,37,37,10,0,0,21,37,37,37,37,37,37,37,25,19,0,0,0,0,0,0,0,11,37,37,37,18,4,0,0,0,0,11,37,37,37,18,4,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,0,0,3,17,25,43,37,37,37,37,37,24,7,0,0,0,26,15,15,15,15,15,15,15,15,22,0,0,3,25,15,15,15,25,25,15,15,15,25,7,0,0,3,25,15,15,15,25,24,15,15,15,13,22,15,15,15,43,4,0,0,3,25,15,15,15,25,25,15,15,15,25,7,0,0,0,12,37,37,37,37,10,0,0,0,0,0,0,0,0,0,0,21,
37,37,37,37,12,0,0,0,0,0,0,0,0,0,0,0,10,37,37,37,37,24,7,0,0,0,0,0,0,0,11,37,37,37,18,4,0,0,0,12,43,37,37,37,18,4,0,0,0,0,0,0,0,8,13,37,37,37,37,37,37,37,37,37,37,37,12,0,0,11,37,37,37,18,43,37,37,37,37,37,37,37,37,37,37,18,4,0,0,21,10,10,10,2,8,13,37,37,37,11,0,1,10,11,24,22,42,0,1,13,37,37,37,24,7,0,0,4,25,16,14,20,13,1,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,0,21,22,22,22,21,0,0,0,0,0,0,0,11,37,37,37,37,37,37,13,
10,19,0,0,0,0,0,0,0,0,8,22,22,10,3,0,0,0,12,37,37,37,37,37,37,37,37,43,2,0,0,22,15,15,15,37,21,37,15,15,15,10,18,15,15,15,13,19,0,3,25,15,15,15,25,22,15,15,15,15,21,0,0,11,37,37,37,18,4,3,24,37,37,37,37,21,0,0,8,11,37,37,37,18,2,3,24,37,37,37,37,21,0,0,23,37,37,37,37,37,37,37,25,19,42,25,37,37,37,43,2,0,3,25,15,15,15,25,25,15,15,15,25,7,0,0,0,0,7,18,44,36,36,27,25,0,11,33,36,36,41,37,3,0,0,0,0,4,17,11,40,36,36,35,13,22,5,0,0,0,
0,7,18,35,36,36,27,13,19,25,33,36,36,44,37,3,4,20,36,36,36,14,4,24,27,36,36,33,23,0,19,37,36,36,36,28,12,25,35,36,36,40,22,22,33,36,36,35,25,0,0,23,33,36,36,36,36,36,36,14,4,0,0,0,0,0,0,19,37,36,36,36,28,12,0,0,0,19,37,36,36,36,28,12,0,19,37,36,36,36,28,12,0,0,0,0,0,0,0,0,7,24,28,35,41,36,36,36,36,36,15,42,0,0,0,23,33,31,31,31,31,31,31,44,13,0,0,8,16,31,31,31,15,16,31,31,31,15,1,0,0,8,16,31,31,31,15,37,36,31,31,20,13,44,31,31,34,12,0,0,
8,16,31,31,31,15,16,31,31,31,15,1,0,5,24,25,33,36,36,35,25,25,12,0,0,0,0,0,0,0,0,23,33,36,36,40,22,0,0,0,0,0,0,0,0,0,0,1,37,44,36,36,36,14,12,0,0,0,0,0,0,19,37,36,36,36,28,12,0,0,0,24,33,36,36,36,39,22,0,0,0,0,0,0,0,12,32,36,36,36,36,36,36,36,36,36,36,39,22,0,19,37,36,36,36,28,29,36,36,36,36,36,36,36,36,36,36,29,21,0,4,20,39,39,39,43,12,28,36,36,36,37,1,13,29,27,27,39,22,0,2,14,36,36,36,14,2,0,3,13,33,31,30,38,14,5,0,12,37,36,
36,36,28,26,9,0,0,0,0,0,0,1,37,40,40,40,15,42,0,0,0,0,0,19,37,36,36,36,36,36,41,35,29,25,6,0,0,0,0,0,0,0,11,39,40,29,17,0,0,0,22,33,36,36,36,36,36,36,36,39,10,0,0,13,27,31,31,35,15,29,31,31,27,13,40,31,31,44,16,4,0,8,16,31,31,31,15,14,36,31,31,39,22,0,19,37,36,36,36,28,11,23,28,36,36,36,33,23,0,7,13,28,36,36,36,28,20,23,29,36,36,36,33,23,0,3,37,36,36,36,36,36,36,36,14,4,24,39,36,36,36,29,10,0,8,16,31,31,31,15,16,31,31,31,15,1,0,0,0,0,
0,10,29,47,51,31,20,5,15,36,52,45,39,23,0,0,0,7,22,15,29,27,35,48,53,44,27,39,20,11,6,0,0,0,26,32,38,51,30,29,10,32,38,51,30,28,10,0,4,20,30,51,30,14,4,24,27,53,50,33,23,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,23,33,50,52,52,51,51,30,14,4,0,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,0,17,28,38,50,52,52,52,52,51,31,15,42,0,0,0,23,33,50,52,49,49,53,52,44,13,0,0,8,
16,38,51,31,15,16,38,51,31,15,1,0,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,23,33,35,35,49,53,35,35,35,25,0,0,0,0,0,0,12,10,23,33,50,48,40,22,10,10,8,0,0,0,0,0,0,0,10,29,46,51,52,49,33,25,0,0,0,0,0,0,19,37,36,51,47,28,12,0,0,5,20,31,51,52,53,44,37,6,0,0,0,0,0,0,3,13,27,49,51,52,52,52,52,51,52,41,15,4,0,19,37,36,51,47,28,37,44,53,51,52,52,52,52,51,50,35,43,1,0,42,37,41,48,30,14,12,28,
47,51,36,37,10,29,30,50,49,33,22,0,3,18,44,53,45,39,22,0,21,32,30,51,51,30,14,5,3,43,40,36,51,47,40,14,12,0,0,0,0,0,0,1,15,31,49,31,16,6,0,0,0,0,0,19,37,36,51,51,52,52,51,50,30,39,11,0,0,0,7,42,42,42,24,27,48,39,10,0,0,0,22,33,49,51,52,52,52,52,48,39,10,0,0,2,37,27,45,48,35,29,45,53,27,39,30,50,41,14,10,0,0,8,16,38,51,31,14,41,50,46,40,13,6,0,19,37,36,51,47,29,27,32,36,53,45,27,43,4,0,3,37,36,47,51,49,47,28,32,36,53,45,27,43,4,0,3,37,36,
52,52,52,51,51,30,14,11,29,38,52,46,40,25,1,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,0,0,8,37,41,52,45,40,23,39,47,51,31,20,5,0,0,0,22,34,41,47,50,53,51,54,52,50,45,31,39,25,3,0,0,7,25,33,45,53,44,37,35,50,48,27,13,3,0,4,20,30,51,30,14,5,24,27,53,50,33,23,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,23,33,41,41,41,30,51,30,14,4,0,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,19,37,36,51,47,28,12,0,19,37,36,51,47,28,12,0,0,0,0,
0,0,0,0,25,27,50,53,31,44,44,46,51,31,15,42,0,0,0,23,33,50,49,40,40,47,52,44,13,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,23,33,50,53,51,51,53,53,35,25,0,0,0,0,0,1,37,39,39,33,50,48,40,39,39,39,11,0,0,0,0,0,0,19,18,35,50,45,38,51,31,20,2,0,0,0,0,0,19,37,36,51,47,28,12,0,0,11,40,45,50,44,53,46,29,10,0,0,0,0,0,0,0,21,32,38,50,41,41,41,41,48,47,
39,23,0,0,19,37,36,51,47,28,22,29,46,48,41,41,41,41,49,30,28,17,0,0,0,11,33,49,48,40,22,28,47,51,36,37,18,35,53,47,44,33,22,0,0,22,40,48,53,35,18,7,24,27,49,53,38,36,14,5,3,37,36,49,54,50,47,28,12,0,0,0,0,0,4,43,15,31,51,31,16,43,4,0,0,0,0,19,37,36,51,47,44,44,36,50,52,36,15,8,0,3,25,15,15,15,15,27,48,39,10,0,0,0,22,33,49,53,41,41,41,41,41,39,10,0,0,0,12,37,35,48,45,27,45,53,27,30,53,36,32,22,0,0,0,8,16,38,51,31,44,49,46,33,13,4,0,0,19,37,
36,51,47,29,46,41,50,48,35,37,2,0,0,3,37,35,36,51,47,35,28,41,50,48,35,37,2,0,0,3,37,41,41,41,41,30,51,30,14,34,31,53,45,33,18,4,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,0,0,0,23,40,45,52,41,15,44,53,49,27,25,0,0,0,4,16,41,53,53,46,31,49,52,36,30,50,52,30,28,26,0,0,0,2,16,41,53,45,40,47,52,36,20,12,0,0,4,20,30,51,30,14,5,24,27,53,50,33,23,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,12,18,43,43,20,30,51,30,14,22,10,21,42,
0,0,0,19,37,36,51,47,28,22,22,21,8,19,37,36,51,47,28,12,0,19,37,36,51,47,28,22,22,21,8,0,0,0,0,9,18,44,52,48,39,13,32,46,51,31,15,42,0,0,0,11,27,50,48,40,28,47,52,44,13,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,23,33,30,30,50,52,30,30,35,25,2,7,0,0,0,1,15,31,48,48,52,53,48,48,48,27,24,0,0,0,0,0,0,26,28,30,52,36,27,50,48,33,11,0,0,0,0,0,19,37,36,
51,47,28,22,22,22,15,36,52,30,39,47,53,44,43,19,0,0,0,0,0,0,7,13,27,49,38,14,37,41,53,41,37,4,0,0,19,37,36,51,47,28,22,43,44,50,36,16,20,36,50,35,18,19,0,0,0,21,32,30,53,44,18,28,47,51,36,37,14,38,52,41,37,18,12,0,0,12,14,30,51,38,20,5,15,36,52,46,29,15,18,1,3,37,33,36,51,47,33,32,22,21,8,0,0,0,12,34,31,30,51,30,31,28,12,0,0,0,0,19,37,36,51,47,28,43,39,30,54,30,14,2,0,8,16,31,31,31,31,31,48,39,10,0,0,5,22,33,49,53,35,43,43,43,43,18,5,0,0,0,
0,21,16,35,49,47,49,53,30,52,31,28,23,9,0,0,0,8,16,38,51,30,49,47,33,18,4,0,0,0,19,37,36,51,47,29,45,49,49,44,16,21,0,0,0,0,10,37,36,51,47,28,44,49,49,44,16,21,0,0,0,0,22,43,43,43,20,30,51,30,14,41,50,48,27,37,5,0,0,0,8,16,38,51,31,35,35,38,51,31,15,1,0,0,0,0,0,0,5,20,31,51,47,29,30,51,30,32,21,0,0,0,17,29,47,51,30,29,40,48,53,35,32,41,53,50,27,24,0,0,0,0,22,29,46,52,30,52,47,39,23,0,0,0,4,20,38,51,47,28,15,15,27,53,50,33,23,0,19,37,
36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,7,7,4,20,30,51,30,40,40,39,28,37,17,0,0,19,37,36,51,47,40,40,39,28,37,22,37,36,51,47,28,12,0,19,37,36,51,47,40,40,39,28,37,22,7,0,0,9,13,44,53,48,39,13,32,46,51,31,15,42,0,0,0,25,27,53,45,39,28,47,52,44,13,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,17,20,20,33,49,53,35,40,40,39,14,25,4,0,0,1,15,31,38,
38,50,49,38,38,38,27,24,0,0,0,0,0,7,13,35,50,48,33,34,30,52,36,16,4,0,0,0,0,19,37,36,51,47,40,40,40,40,40,45,50,35,15,41,52,30,28,26,0,0,0,0,0,0,0,21,14,38,49,27,39,47,47,39,22,0,0,0,19,37,36,51,47,40,40,40,40,46,45,40,33,48,30,34,26,0,0,0,0,19,43,44,52,38,14,28,47,51,36,37,39,45,48,33,23,7,0,0,0,3,18,44,53,45,39,22,28,46,52,36,15,6,3,0,0,21,37,36,51,47,40,40,39,28,37,22,7,0,12,34,46,49,54,49,47,28,12,0,0,0,0,19,37,36,51,47,28,40,30,30,
51,46,32,12,0,8,16,38,51,47,46,46,46,39,10,0,9,13,39,33,49,53,35,39,39,39,13,7,0,0,0,0,0,21,20,36,53,53,54,54,52,52,46,39,23,0,0,0,0,8,16,38,51,51,50,49,35,43,4,0,0,0,19,37,36,51,47,35,48,50,41,14,26,0,0,0,0,0,19,37,36,51,47,35,48,50,41,14,26,0,0,0,0,0,0,7,7,4,20,30,51,30,44,49,49,44,15,12,0,0,0,0,8,16,38,51,50,49,49,50,51,31,15,1,0,0,0,0,0,0,0,25,27,49,53,44,49,50,35,18,3,0,0,0,22,33,49,52,44,18,40,48,53,35,13,33,49,52,44,13,
9,0,0,0,1,18,35,49,54,50,44,37,8,0,0,0,1,37,36,52,53,38,31,31,31,53,50,33,23,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,0,0,4,20,30,51,50,49,49,48,46,44,14,21,0,19,37,36,51,53,49,49,48,46,41,34,37,36,51,47,28,12,0,19,37,36,51,53,49,49,48,46,41,34,22,0,0,0,22,39,46,53,31,44,44,46,51,31,15,42,0,0,12,43,44,52,46,28,28,47,52,44,13,4,0,8,16,38,51,31,15,16,38,51,31,15,2,0,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,7,0,
8,16,38,51,31,15,16,38,51,31,15,1,0,0,4,22,33,49,51,49,49,48,45,38,33,18,1,0,7,25,20,20,33,50,48,40,20,20,16,10,0,0,0,0,0,21,32,30,51,31,33,33,35,50,45,39,23,0,0,0,0,19,37,36,51,53,49,49,49,49,49,52,46,33,33,33,45,50,35,13,3,0,0,0,0,0,0,7,25,27,49,30,41,53,44,37,8,0,0,0,19,37,36,51,53,49,49,49,49,50,52,36,31,50,35,13,3,0,0,0,0,0,23,40,48,45,39,28,47,51,36,37,35,53,30,14,12,0,0,0,0,0,22,40,48,53,35,13,27,49,49,27,24,0,0,0,0,19,37,36,
51,53,49,49,48,46,41,34,22,0,12,14,40,31,51,31,40,32,22,5,0,0,0,19,37,36,51,47,40,40,41,52,51,31,20,4,0,8,16,38,51,31,32,32,32,14,21,0,7,43,41,48,52,51,48,48,48,44,18,9,0,0,0,0,6,43,27,49,45,41,50,52,30,38,52,36,14,21,1,0,0,8,16,38,51,45,35,49,47,39,24,1,0,0,19,37,36,51,45,45,51,49,35,43,6,0,0,0,0,0,19,37,36,51,45,45,51,49,35,43,6,0,0,0,0,0,0,0,0,4,20,30,51,47,48,54,48,27,13,3,0,0,0,0,8,16,38,51,31,33,33,38,51,31,15,1,1,0,0,0,
0,0,0,21,32,30,51,50,51,47,29,10,0,0,0,0,22,33,49,52,44,18,40,48,53,35,13,33,49,52,44,13,9,0,0,0,19,13,27,49,54,50,35,43,42,0,0,0,0,24,33,47,53,52,52,52,52,54,50,33,23,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,0,0,4,20,30,51,48,47,47,48,51,49,35,13,9,19,37,36,51,49,47,47,48,52,50,44,37,36,51,47,28,12,0,19,37,36,51,49,47,47,48,52,50,44,37,1,0,0,6,18,33,30,52,52,52,52,51,31,15,42,0,26,14,40,46,51,31,28,28,47,52,44,28,37,42,8,
16,38,51,31,28,28,38,51,31,28,20,12,0,8,16,38,51,31,28,28,36,51,30,28,28,44,52,46,28,32,10,0,8,16,38,51,31,28,28,38,51,31,15,1,0,0,0,22,33,49,52,47,47,47,50,51,46,29,26,0,0,3,8,23,33,50,48,35,35,27,39,37,12,0,0,0,9,25,27,49,54,49,49,49,49,51,52,41,37,8,0,0,0,19,37,36,51,49,47,47,47,47,53,54,50,49,49,49,53,51,30,32,21,0,0,0,0,0,9,23,32,35,46,50,48,48,41,29,25,42,0,0,19,37,36,51,49,47,47,47,47,51,54,48,49,45,44,28,11,3,0,0,0,0,12,14,30,53,35,39,
47,51,36,39,38,53,44,43,3,0,0,0,0,0,12,14,30,51,31,20,41,52,46,28,26,0,0,0,0,19,37,36,51,49,47,47,48,52,50,44,37,1,9,17,15,31,51,31,35,27,40,20,10,0,0,19,37,36,51,53,49,49,50,51,49,27,37,5,0,8,16,38,51,31,15,12,12,2,0,0,7,43,41,45,53,51,45,45,45,44,18,9,0,0,0,0,11,39,46,50,44,29,45,53,27,33,45,49,35,15,37,2,0,8,16,38,51,31,14,31,52,31,32,15,23,0,19,37,36,51,54,51,51,51,46,39,23,0,0,0,0,0,19,37,36,51,54,52,47,51,46,39,23,0,0,0,0,0,0,0,
0,4,20,30,51,54,53,45,51,30,28,10,0,0,0,0,8,16,38,51,31,15,16,38,51,31,15,15,37,21,0,0,0,0,0,3,13,35,50,54,52,41,37,8,0,0,0,0,10,39,45,51,38,28,40,48,53,35,14,44,53,50,27,24,0,0,0,0,22,29,30,51,47,52,46,39,23,0,0,0,0,5,43,40,44,44,44,44,44,53,50,33,23,0,19,37,36,51,47,28,12,25,35,53,49,40,22,22,33,49,53,35,25,0,0,0,0,0,4,20,30,51,30,28,29,33,45,51,31,15,6,19,37,36,51,47,28,28,40,46,51,30,14,36,51,47,28,12,0,19,37,36,51,47,28,28,40,46,51,30,
14,2,0,0,0,5,20,36,52,45,44,46,51,31,15,42,0,10,39,45,52,51,47,47,47,48,51,47,38,20,4,8,16,38,51,47,47,47,45,51,45,47,28,21,0,8,16,38,51,47,47,47,47,51,45,47,47,47,51,48,47,33,23,0,8,16,38,51,47,47,47,45,51,31,15,1,0,0,0,22,33,49,53,35,28,29,41,53,49,33,11,0,0,0,0,23,33,50,52,49,49,50,47,35,18,19,0,0,12,14,31,51,46,30,49,53,30,30,53,47,39,22,0,0,0,19,37,36,51,47,28,28,28,44,53,45,30,47,51,46,30,48,49,27,25,9,0,0,0,0,17,32,31,50,53,53,52,50,53,30,
39,11,0,0,19,37,36,51,47,28,28,29,38,52,53,52,53,53,53,31,28,22,0,0,0,0,3,18,44,53,45,45,50,51,48,45,49,45,40,22,0,0,0,0,0,0,3,18,44,53,45,39,46,51,36,15,6,0,0,0,0,19,37,36,51,47,28,28,40,46,51,30,14,2,0,1,15,31,51,50,49,53,45,41,16,4,0,19,37,36,51,48,46,46,30,36,35,30,29,21,0,8,16,38,51,31,15,1,0,0,0,0,9,13,39,33,49,53,35,39,39,39,13,9,0,0,0,21,14,36,52,31,32,29,45,53,27,37,44,50,46,31,29,26,0,8,16,38,51,31,15,39,46,50,31,44,18,9,19,37,
36,51,51,30,45,48,52,36,20,12,0,0,0,0,19,37,36,51,51,30,33,48,52,36,20,12,0,0,0,0,0,0,0,4,20,30,51,53,31,35,49,50,44,37,4,0,0,0,8,16,38,51,31,15,16,38,51,30,31,31,33,22,0,0,0,0,0,17,23,29,46,54,45,40,23,0,0,0,0,0,5,20,36,53,53,30,36,49,53,41,38,49,51,30,29,17,0,0,0,5,15,44,53,45,33,47,53,41,20,2,0,0,0,0,4,23,13,13,13,13,27,53,50,33,23,0,19,37,36,51,47,28,23,25,35,53,49,40,23,23,33,49,53,35,25,0,0,0,0,0,4,20,30,51,30,14,21,14,30,51,
38,20,8,19,37,36,51,47,28,21,15,31,51,46,32,36,51,47,28,12,0,19,37,36,51,47,28,21,15,31,51,46,32,12,0,0,0,24,33,45,52,41,32,46,51,31,15,42,0,10,39,48,47,46,46,46,46,46,46,48,38,20,4,8,16,38,46,46,46,46,46,46,47,47,28,21,0,8,16,38,46,46,46,46,46,46,46,46,46,46,46,46,49,33,23,0,8,16,38,46,46,48,45,46,46,31,15,1,0,0,0,22,33,49,53,35,25,11,33,50,50,27,24,0,0,0,0,23,33,50,48,33,33,36,52,30,14,5,0,0,24,33,48,49,27,39,45,53,27,29,47,53,44,37,1,0,0,19,37,36,
51,47,28,17,29,46,52,41,14,30,51,36,14,31,51,38,14,12,0,0,0,19,43,44,50,49,41,46,45,35,45,52,36,20,5,0,19,37,36,51,47,28,15,41,53,48,44,45,46,44,48,53,41,37,8,0,0,0,0,22,39,45,52,52,51,54,52,52,52,38,14,2,0,0,0,0,0,0,0,22,40,48,50,35,49,49,27,24,0,0,0,0,0,19,37,36,51,47,28,21,15,31,51,46,32,12,0,1,15,31,51,31,33,44,50,45,29,26,0,19,37,36,51,47,28,32,14,15,15,40,25,19,0,8,16,38,51,31,15,1,0,0,0,0,0,4,22,33,49,53,35,25,17,17,4,0,0,0,1,43,
35,46,46,40,24,29,46,46,27,24,32,31,52,45,29,26,0,8,16,38,46,31,15,18,27,48,52,44,18,9,19,37,36,51,47,40,46,41,53,48,27,13,19,0,0,0,19,37,36,51,47,40,16,41,53,48,27,13,19,0,0,0,0,0,0,4,20,30,51,30,29,14,31,52,45,40,24,9,0,0,8,16,38,46,31,15,16,38,46,46,53,49,33,22,0,0,0,0,4,20,33,27,47,51,31,20,2,0,0,0,0,0,0,23,28,36,45,50,52,51,54,52,53,48,38,40,13,19,0,0,9,24,40,45,53,41,16,44,50,48,33,25,7,0,0,0,0,0,9,9,9,24,27,53,50,33,23,0,19,37,
36,51,47,33,33,33,35,53,49,33,33,33,33,49,53,35,25,0,0,0,0,0,4,20,30,51,30,29,29,33,45,51,36,37,1,19,37,36,51,47,29,29,40,46,51,30,14,36,51,47,28,12,0,19,37,36,51,47,29,29,40,46,51,30,14,5,0,0,12,20,31,52,47,39,32,46,51,31,15,42,0,10,39,48,41,34,34,34,34,34,34,46,38,20,4,1,43,34,34,34,34,34,34,34,31,47,28,21,0,1,43,34,34,34,34,34,34,34,34,34,34,34,34,44,49,33,23,0,1,43,34,34,34,46,30,34,34,34,18,19,0,0,0,22,33,49,53,35,29,29,41,53,49,33,23,0,0,0,0,23,
33,50,48,40,15,35,53,46,32,12,0,5,20,36,51,30,32,39,45,53,27,37,41,52,46,29,10,0,0,19,37,36,51,47,28,18,35,50,45,40,20,30,51,36,37,27,49,48,33,24,0,0,0,12,32,30,51,31,34,46,45,39,44,53,45,39,22,0,19,37,36,51,47,28,29,47,52,41,39,45,46,32,36,52,46,29,21,0,0,0,0,5,20,44,44,44,47,51,36,44,44,35,13,7,0,0,0,0,0,0,0,12,14,30,51,45,52,46,28,26,0,0,0,0,0,19,37,36,51,47,29,29,40,46,51,30,14,5,0,1,15,31,51,31,16,40,48,48,39,10,0,19,37,36,51,47,28,12,2,8,
12,22,4,0,0,8,16,38,51,31,15,1,0,0,0,0,0,0,22,33,49,53,35,25,0,0,0,0,0,0,1,43,34,34,34,37,12,20,34,34,34,23,24,27,53,45,29,26,0,1,43,34,34,34,18,12,32,46,52,44,18,9,19,37,36,51,47,29,46,29,46,51,30,29,22,0,0,0,19,37,36,51,47,28,22,29,46,51,30,29,22,0,0,0,0,0,0,4,20,30,51,30,14,24,40,47,52,31,14,26,0,0,1,43,34,34,34,18,43,34,34,33,49,49,33,22,0,0,0,0,5,14,30,50,51,48,27,25,9,0,0,0,0,0,0,3,22,16,39,35,44,48,53,41,35,40,14,25,4,0,
0,0,21,14,31,52,46,29,22,28,30,51,30,34,17,0,0,0,0,0,0,0,0,24,27,53,50,33,23,0,19,37,36,51,53,49,49,49,49,51,52,49,49,49,49,52,53,35,25,0,0,0,0,0,4,20,30,51,48,47,47,48,51,45,33,25,0,19,37,36,51,49,47,47,48,52,48,35,37,36,51,47,28,12,0,19,37,36,51,49,47,47,48,52,48,35,18,3,0,9,25,33,48,53,41,37,32,46,51,31,15,42,0,10,39,48,41,43,12,12,12,12,32,46,38,20,4,0,4,12,12,12,12,12,12,15,31,47,28,21,0,0,4,12,12,12,12,12,12,12,12,12,12,12,13,44,48,33,23,0,
0,4,12,12,14,46,30,14,12,12,4,0,0,0,0,22,33,49,52,47,47,45,50,53,38,32,21,0,0,0,0,23,33,50,49,31,31,47,52,31,20,4,0,11,40,45,50,35,18,39,45,53,27,24,39,45,50,35,18,19,0,19,37,36,51,47,28,32,30,52,36,16,20,30,51,36,37,32,30,51,31,20,2,0,0,10,39,48,50,35,34,46,45,39,39,48,50,27,25,0,19,37,36,51,47,28,33,49,49,33,39,45,46,32,27,49,49,33,23,0,0,0,0,9,23,18,18,28,47,51,36,37,18,18,26,0,0,0,0,0,0,0,0,3,18,44,53,54,51,36,15,8,0,0,0,0,0,19,37,36,
51,49,47,47,48,52,48,35,18,3,0,1,15,31,51,30,31,30,52,30,34,21,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,0,22,33,49,53,35,25,0,0,0,0,0,0,0,4,12,12,12,4,0,2,12,12,12,3,24,27,48,45,29,26,0,0,4,12,12,12,8,12,32,46,48,44,18,9,19,37,36,51,47,29,27,14,27,49,53,41,15,5,0,0,19,37,36,51,47,28,12,18,27,49,53,41,15,5,0,0,0,0,0,4,20,30,51,30,14,4,37,44,50,50,35,43,8,0,0,4,12,12,12,8,4,12,22,33,48,48,33,22,0,0,
0,0,5,14,30,46,38,35,16,12,0,0,0,0,0,0,0,0,0,5,22,25,40,46,46,35,13,23,12,9,0,0,0,3,18,35,46,46,35,18,1,13,27,46,46,44,37,1,0,0,0,0,0,0,0,24,27,46,46,33,23,0,19,37,36,46,46,46,46,46,46,46,46,46,46,46,46,46,46,35,25,0,0,0,0,0,4,20,30,46,46,46,46,46,31,33,37,2,0,19,37,36,46,46,46,46,46,31,27,20,37,36,46,46,28,12,0,19,37,36,46,46,46,46,46,31,27,20,21,0,0,12,14,31,46,46,29,22,32,46,46,31,15,42,0,10,29,40,40,18,7,0,0,12,14,40,40,15,8,0,
0,0,0,0,0,0,1,37,40,40,32,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,40,40,39,22,0,0,0,0,5,14,40,40,20,4,0,0,0,0,0,0,22,33,46,46,46,46,46,30,41,29,24,7,0,0,0,0,23,33,46,46,46,46,46,36,29,11,0,4,15,36,46,46,28,17,39,46,46,27,24,16,36,46,30,34,21,0,19,37,36,46,46,28,27,46,46,33,24,20,30,46,36,37,13,35,46,46,33,11,0,0,22,33,46,46,33,34,46,46,39,28,46,46,35,13,0,19,37,36,46,46,28,27,46,46,39,39,46,46,32,39,46,46,27,11,0,0,0,0,0,0,9,12,28,
46,46,36,37,19,9,0,0,0,0,0,0,0,0,0,0,22,40,46,46,46,27,24,0,0,0,0,0,0,19,37,36,46,46,46,46,46,31,27,20,21,0,0,1,15,31,46,46,46,46,31,33,13,19,0,19,37,36,46,46,28,12,0,0,0,0,0,0,0,8,16,38,46,31,15,1,0,0,0,0,0,0,22,33,46,46,35,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,39,40,40,28,21,0,0,0,0,0,0,0,2,14,40,40,40,13,9,19,37,36,46,46,28,24,22,14,36,46,46,33,25,0,0,19,37,36,46,46,28,12,12,14,36,46,46,33,25,0,0,0,0,
0,4,20,30,46,30,14,4,10,34,30,46,46,39,23,0,0,0,0,0,0,0,0,0,22,29,40,40,39,22,0,0,0,0,8,37,34,32,20,13,21,0,0,0,0,0,0,0,0,0,0,0,0,26,14,34,34,34,11,0,0,0,0,0,0,3,18,34,34,34,14,21,0,12,16,34,34,34,43,1,0,0,0,0,0,0,0,23,32,34,34,32,22,0,7,13,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,34,11,0,0,0,0,0,42,37,34,34,34,34,34,32,16,25,2,0,0,7,13,34,34,34,34,34,32,20,13,12,13,34,34,34,20,2,0,7,13,34,34,34,34,34,32,20,13,12,
0,0,0,12,20,34,34,34,37,4,15,34,34,34,43,19,0,3,10,22,22,2,0,0,0,0,26,22,22,21,0,0,0,0,0,0,0,0,0,12,22,22,17,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,22,22,10,1,0,0,0,0,0,21,22,22,21,0,0,0,0,0,0,0,10,14,34,34,34,34,34,14,37,22,19,0,0,0,0,0,22,32,34,34,34,34,32,15,23,1,0,8,37,34,34,34,43,21,14,34,34,32,23,11,34,34,34,14,21,0,7,13,34,34,34,20,34,34,34,20,12,37,34,34,34,13,26,14,34,34,34,11,0,0,10,32,34,34,14,16,34,34,14,20,34,
34,34,24,0,7,13,34,34,34,20,34,34,34,20,14,34,34,16,14,34,34,32,22,0,0,0,0,0,0,0,2,16,34,34,34,13,7,0,0,0,0,0,0,0,0,0,0,0,12,20,34,34,34,14,26,0,0,0,0,0,0,7,13,34,34,34,34,34,32,20,13,12,0,0,0,19,18,34,34,34,34,32,16,24,4,0,0,7,13,34,34,34,20,2,0,0,0,0,0,0,0,1,43,34,34,34,18,3,0,0,0,0,0,0,10,32,34,34,34,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,22,22,22,10,7,0,0,0,0,0,0,0,0,26,22,22,22,5,0,7,13,
34,34,34,20,2,0,11,34,34,34,34,25,0,0,7,13,34,34,34,20,2,0,11,34,34,34,34,25,0,0,0,0,0,42,37,34,34,34,37,8,3,25,34,34,34,32,23,0,0,0,0,0,0,0,0,0,1,10,22,22,10,1,0,0,0,0,0,4,12,12,4,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,12,12,12,19,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,0,0,0,0,0,3,12,12,12,12,7,0,0,8,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,19,0,0,0,0,0,0,4,12,12,12,12,12,12,4,9,
0,0,0,0,8,12,12,12,12,12,12,4,7,0,8,12,12,12,2,0,0,0,8,12,12,12,12,12,12,4,7,0,0,0,0,0,2,12,12,12,4,0,5,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,12,12,12,12,12,12,5,1,0,0,0,0,0,0,0,7,12,12,12,12,12,12,8,0,0,0,0,4,12,12,12,4,0,2,12,12,12,3,19,12,12,12,12,0,0,0,8,12,
12,12,2,12,12,12,2,0,4,12,12,12,8,0,12,12,12,12,3,0,0,9,12,12,12,12,5,12,12,2,2,12,12,12,19,0,0,8,12,12,12,2,12,12,12,2,2,12,12,5,12,12,12,12,7,0,0,0,0,0,0,0,0,5,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,2,12,12,12,12,0,0,0,0,0,0,0,0,8,12,12,12,12,12,12,4,7,0,0,0,0,0,4,12,12,12,12,12,4,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,0,9,12,12,12,12,3,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,3,12,12,12,12,1,0,0,0,8,12,12,12,2,0,0,3,12,12,12,12,1,0,0,0,0,0,0,4,12,12,12,4,0,0,1,12,12,12,12,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,7,19,19,19,19,19,19,19,19,0,0,0,19,42,42,42,42,42,42,42,42,1,0,0,0,0,3,19,19,19,9,0,0,0,19,19,19,19,0,0,0,0,3,19,19,19,9,0,0,0,19,19,19,19,0,0,
0,9,42,42,42,19,0,7,42,42,42,19,0,0,0,19,42,42,42,42,42,42,42,42,1,42,42,42,0,0,0,0,0,0,0,1,42,42,42,0,1,42,42,1,0,0,0,0,0,9,19,19,19,7,0,0,19,19,19,19,0,0,0,9,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,12,12,6,0,0,0,0,0,0,19,19,19,7,0,0,0,0,1,42,42,42,0,1,42,42,1,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,19,19,19,0,0,0,7,42,42,42,42,42,42,1,0,0,0,0,19,19,19,3,0,0,0,3,19,19,19,0,0,0,0,0,
0,0,0,0,3,19,19,19,0,0,0,0,0,0,9,19,19,19,19,19,19,19,19,0,0,0,9,19,19,19,3,0,0,0,0,19,19,19,19,19,3,0,0,0,0,7,19,19,19,7,0,0,0,0,0,0,0,0,3,19,19,19,19,19,19,19,19,19,19,0,0,0,0,19,19,19,7,0,0,19,19,19,3,0,0,3,19,19,19,0,0,0,0,19,19,19,7,0,0,9,19,19,19,19,0,0,0,7,42,42,42,42,42,42,42,42,7,0,0,0,0,0,7,19,19,19,7,19,19,19,0,0,0,0,0,0,0,9,19,3,0,0,0,0,0,0,0,0,0,0,0,9,19,19,19,19,19,19,19,19,
0,0,0,0,0,0,0,0,1,4,5,8,7,0,0,0,0,7,19,19,19,3,0,19,19,19,7,0,0,0,0,19,19,19,7,0,0,19,19,19,3,0,0,3,19,19,19,0,0,0,0,9,19,19,19,0,0,3,19,19,19,0,0,0,0,9,19,19,19,19,19,19,19,19,19,19,9,0,0,0,9,19,19,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,19,19,19,19,0,0,0,0,0,0,0,0,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,11,37,37,37,18,4,6,13,37,37,37,37,37,37,37,37,10,0,8,18,15,15,15,15,15,15,15,15,15,21,0,
0,2,43,37,37,37,25,3,0,17,37,37,37,37,22,0,0,2,43,37,37,37,25,3,0,17,37,37,37,37,22,0,0,11,15,15,15,43,4,25,15,15,15,18,6,0,8,18,15,15,15,15,15,15,15,15,15,15,15,15,22,0,0,0,0,0,5,37,15,15,15,22,15,15,15,15,21,0,0,0,3,25,37,37,37,25,19,17,37,37,37,37,21,0,3,25,37,37,37,25,19,0,0,0,0,0,0,0,0,0,0,0,0,21,18,20,32,32,15,11,6,0,0,0,11,37,37,37,18,4,0,0,5,37,15,15,15,22,15,15,15,15,21,0,0,0,0,0,0,23,37,37,37,37,37,37,37,37,37,37,
37,17,0,3,25,15,15,15,15,15,15,37,12,0,0,22,37,37,37,37,12,0,2,43,37,37,37,11,0,0,0,0,0,0,0,4,18,37,37,37,23,0,0,0,0,3,25,37,37,37,37,37,37,37,37,23,0,7,24,37,37,37,18,4,0,0,11,37,37,37,37,37,37,13,10,19,1,13,37,37,37,13,1,0,0,0,0,3,17,25,43,37,37,37,37,37,37,37,37,37,37,26,0,0,22,37,37,37,18,4,26,37,37,37,37,12,2,43,37,37,37,17,0,0,11,37,37,37,18,10,10,24,37,37,37,37,21,0,3,25,15,15,15,15,15,15,15,15,25,7,0,0,0,8,13,37,37,37,13,37,
37,37,22,0,0,0,0,8,22,18,37,37,25,21,9,0,0,0,0,0,0,0,7,24,37,37,37,37,37,37,37,37,22,0,0,0,0,0,42,23,37,20,14,16,18,22,0,0,19,25,37,37,37,43,11,37,37,37,25,19,0,0,22,37,37,37,18,4,26,37,37,37,37,12,2,43,37,37,37,17,0,0,7,25,37,37,37,22,2,43,37,37,37,22,0,0,3,25,37,37,37,37,37,37,37,37,37,37,25,3,0,9,24,37,37,37,37,37,43,11,5,0,0,0,0,0,0,0,0,21,37,37,37,37,21,0,0,0,0,0,0,11,37,37,37,37,37,37,37,37,37,21,0,0,0,0,0,0,0,0,
19,37,36,36,36,28,12,12,32,36,36,36,36,36,36,36,35,25,0,2,32,31,31,31,31,31,31,31,31,33,22,0,0,17,29,36,36,36,28,17,6,43,35,36,36,35,18,9,0,17,29,36,36,36,28,17,6,43,35,36,36,35,18,9,3,43,35,31,31,40,24,34,31,31,36,14,5,0,2,32,31,31,31,31,31,31,31,31,33,31,31,44,13,0,0,0,0,0,21,29,31,31,35,25,33,31,31,33,22,0,0,0,4,20,36,36,36,14,4,24,27,36,36,33,23,0,4,20,36,36,36,14,4,0,0,0,0,0,0,0,0,17,22,22,17,20,35,38,46,30,36,39,13,1,0,19,37,36,36,36,
28,12,0,0,21,29,31,31,35,25,33,31,31,33,22,0,0,0,0,0,8,15,36,36,36,36,36,36,36,36,36,36,27,24,0,8,16,31,31,31,31,31,31,39,10,0,7,18,35,36,36,27,13,19,25,33,36,36,44,37,3,0,0,0,0,0,0,21,29,36,36,36,37,3,0,0,0,4,20,36,36,36,36,36,36,36,36,14,12,26,32,36,36,36,29,21,0,19,37,36,36,36,36,36,41,35,29,25,23,39,36,36,36,14,5,0,0,0,7,24,28,35,41,36,36,36,36,36,36,36,36,36,27,11,0,9,18,44,36,36,29,26,25,27,36,36,33,23,22,39,36,36,27,25,0,19,37,36,36,36,
29,39,39,29,36,36,36,33,23,0,8,16,31,31,31,31,31,31,31,31,15,1,0,0,0,12,34,36,36,44,34,36,36,44,13,9,0,0,2,43,39,44,36,41,27,32,10,0,0,0,0,0,0,0,6,16,36,36,36,36,36,36,36,44,13,9,0,0,0,3,25,39,41,30,30,31,44,15,1,0,4,14,36,36,36,40,37,36,36,36,14,5,0,9,18,44,36,36,29,26,25,27,36,36,33,23,22,39,36,36,27,25,0,0,8,20,36,36,44,13,13,33,36,36,35,13,9,0,4,20,36,36,36,36,36,36,36,36,36,36,20,4,0,1,15,36,36,36,36,36,44,33,15,21,0,0,0,0,0,0,
0,25,27,36,36,27,25,0,0,0,0,0,4,16,36,36,36,36,36,36,36,36,33,23,0,0,0,0,0,0,0,0,19,37,36,51,47,28,12,12,32,46,51,51,52,52,52,53,35,25,0,2,32,46,46,46,49,52,46,46,46,33,22,0,0,1,43,35,50,50,44,37,22,39,46,51,38,32,26,0,0,1,43,35,50,50,44,37,22,39,46,51,38,32,26,0,0,26,14,31,52,31,14,44,50,46,39,11,0,0,2,32,46,46,46,49,52,46,46,46,33,46,52,44,13,0,0,0,0,0,21,29,45,53,35,25,33,49,49,33,22,0,0,0,4,20,30,51,30,14,16,15,27,53,50,33,23,0,4,20,
30,51,30,14,4,0,0,0,0,0,0,0,5,14,33,33,37,44,49,49,30,46,53,30,39,22,0,19,37,36,51,47,28,12,0,0,21,29,45,53,35,25,33,49,49,33,22,0,0,0,0,0,10,29,47,51,50,53,51,51,52,52,52,53,27,24,0,8,16,38,51,47,46,46,46,39,10,0,0,26,32,38,51,30,29,10,32,38,51,30,28,10,0,0,0,0,0,0,0,21,29,47,51,36,37,3,0,0,0,4,20,30,51,51,52,52,52,51,49,27,13,43,35,50,49,27,13,19,0,19,37,36,51,51,52,52,51,50,30,39,14,36,52,45,40,24,9,0,0,0,17,28,38,50,52,52,52,52,51,51,
52,52,52,50,27,11,0,0,11,33,49,49,33,23,43,41,52,53,35,13,25,27,50,48,39,22,0,19,37,36,51,47,28,41,45,36,53,45,27,43,4,0,8,16,38,51,49,49,49,49,51,31,15,1,0,0,0,12,34,46,52,44,34,46,52,44,13,9,0,19,13,27,46,49,46,46,49,40,22,42,42,42,42,7,0,0,6,16,31,51,51,52,52,52,52,44,13,9,0,0,0,26,28,30,52,53,48,49,31,15,1,0,4,14,30,51,53,44,37,36,51,30,14,5,0,1,18,27,49,49,33,14,14,41,52,53,35,14,14,27,50,48,39,25,9,0,8,20,38,52,44,13,39,46,50,41,20,21,0,0,
4,20,30,52,52,52,51,51,52,52,52,30,20,4,0,1,15,31,51,52,52,52,52,45,35,43,5,0,0,0,0,0,6,15,36,51,51,36,15,8,0,0,0,0,22,39,30,30,47,52,52,30,30,38,14,12,0,0,0,0,0,0,0,0,19,37,36,51,47,28,12,12,32,46,51,31,41,41,41,41,35,25,0,4,15,32,32,29,45,53,35,32,32,14,10,0,0,0,26,32,38,51,46,39,16,41,53,48,33,25,7,0,0,0,26,32,38,51,46,39,16,41,53,48,33,25,7,0,0,9,24,40,47,49,35,47,49,35,43,8,0,0,4,15,32,32,29,45,53,35,32,32,34,46,52,44,13,0,0,0,
0,0,21,29,45,53,35,25,33,49,49,33,22,0,0,0,4,20,30,51,30,14,31,33,27,53,50,33,23,0,4,20,30,51,30,14,4,0,0,0,0,0,0,0,5,14,30,31,39,47,50,44,14,29,30,53,41,37,1,19,37,36,51,47,28,12,0,0,21,29,45,53,35,25,33,49,49,33,22,0,0,0,0,7,13,35,50,48,33,30,51,30,41,41,41,41,27,24,0,8,16,38,51,31,32,32,32,14,21,0,0,7,25,33,45,53,44,37,35,50,48,27,13,3,0,0,0,0,0,0,0,21,29,47,51,36,37,3,0,0,0,4,20,30,51,30,41,41,41,38,51,30,29,39,46,52,36,14,12,0,
0,19,37,36,51,47,44,44,36,50,52,36,35,49,53,44,15,5,0,0,0,0,25,27,50,53,31,44,44,46,51,31,41,41,41,41,27,11,0,0,17,29,45,53,35,13,20,38,49,50,36,15,43,41,52,46,34,12,0,19,37,36,51,47,28,14,36,52,48,35,37,2,0,0,8,16,38,51,31,40,40,38,51,31,15,1,0,0,0,12,34,46,52,44,34,46,52,44,13,9,0,26,28,30,49,35,28,28,27,40,15,15,15,15,15,24,9,0,6,16,31,51,46,41,41,41,41,44,13,9,0,0,5,24,27,50,50,44,39,33,41,15,1,0,21,14,30,54,51,38,20,36,51,30,14,21,0,4,20,38,
49,53,30,30,30,46,50,53,30,30,30,30,52,48,36,37,3,0,8,20,38,52,44,34,31,52,31,34,22,0,0,0,4,20,41,41,41,41,50,50,41,41,41,41,20,4,0,24,34,31,51,31,44,44,46,51,46,28,16,2,0,0,0,0,21,28,46,54,54,46,28,21,0,0,0,0,26,20,39,39,40,45,51,36,39,39,29,22,0,0,0,0,0,0,0,0,19,37,36,51,47,28,22,22,32,46,51,31,15,43,43,43,43,17,0,0,4,12,21,29,45,53,35,25,12,2,9,0,0,0,7,25,33,48,53,41,33,45,52,36,20,12,0,0,0,0,7,25,33,48,53,41,33,45,52,36,20,12,0,0,
0,0,4,37,35,50,48,52,36,14,21,0,0,0,0,4,12,21,29,45,53,35,25,12,34,46,52,44,13,0,0,0,0,0,12,34,46,51,38,35,35,49,49,33,22,0,0,0,4,20,30,51,30,14,31,33,27,53,50,33,23,0,4,20,30,51,30,14,22,22,22,17,4,0,0,0,8,15,36,47,27,50,45,27,27,27,41,51,30,14,5,19,37,36,51,47,28,12,0,0,12,34,46,51,38,35,35,49,49,33,22,0,0,0,0,2,14,38,51,38,14,30,51,38,20,43,43,43,43,21,0,8,16,38,51,31,15,12,12,2,0,0,0,0,2,16,41,53,45,40,47,52,36,20,12,0,0,0,0,0,
8,21,22,22,29,47,51,36,37,3,0,0,0,4,20,30,51,30,14,43,43,33,45,53,44,36,52,47,40,11,0,0,0,19,37,36,51,47,28,13,32,30,54,30,46,51,30,29,22,0,0,0,0,9,18,44,52,48,39,13,32,46,51,31,15,43,43,43,43,21,0,0,2,14,30,51,36,15,29,47,30,46,30,32,20,38,51,31,16,4,0,19,37,36,51,47,28,44,49,50,46,33,13,6,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,12,34,46,52,44,34,46,52,44,13,9,0,11,33,49,38,14,21,21,15,31,31,31,31,31,36,37,1,0,6,16,31,51,46,32,43,43,43,43,
10,0,0,4,15,28,35,53,45,29,28,28,15,25,7,1,15,33,30,54,54,45,33,36,51,30,33,15,1,8,16,33,30,51,36,33,33,47,30,46,30,33,33,38,51,31,33,43,3,0,8,20,38,52,44,41,50,46,39,24,7,0,0,0,7,24,43,43,43,27,50,50,27,33,37,43,24,7,0,13,44,30,51,30,38,38,38,53,49,38,28,12,0,0,8,12,11,33,49,48,48,49,27,24,12,8,0,0,21,32,30,45,45,49,54,48,45,47,29,10,0,0,0,0,0,0,0,0,19,37,36,51,47,40,40,40,40,46,51,31,15,1,7,7,7,0,0,0,0,0,21,29,45,53,35,25,0,0,0,0,
0,0,0,2,16,41,53,45,31,51,47,39,23,0,0,0,0,0,0,2,16,41,53,45,31,51,47,39,23,0,0,0,0,0,0,26,14,38,51,45,39,11,0,0,0,0,0,0,0,21,29,45,53,35,25,12,34,46,52,44,13,0,0,0,0,0,6,37,44,48,49,49,49,52,49,33,22,0,0,0,8,16,31,51,45,27,31,33,27,53,50,33,23,0,4,20,30,51,30,40,40,40,40,29,37,21,0,0,0,24,40,46,50,51,52,50,50,50,50,50,46,32,12,19,37,36,51,47,28,12,0,0,6,37,44,48,49,49,49,52,49,33,22,0,0,0,0,22,40,45,50,35,14,30,51,38,40,40,40,40,
28,26,0,8,16,38,51,31,15,1,0,0,0,0,0,2,15,14,29,46,52,30,52,47,39,14,20,12,0,0,7,22,15,28,39,40,40,47,51,36,37,3,0,0,0,4,20,30,51,30,20,4,5,15,41,53,45,48,50,44,37,4,0,0,0,19,37,36,51,47,28,22,14,30,51,52,51,49,27,13,19,0,0,0,0,9,18,44,52,48,39,13,32,46,51,31,40,40,40,40,28,21,0,0,1,37,36,51,30,32,33,49,41,31,45,39,29,47,52,44,18,7,0,19,37,36,51,47,35,48,50,41,38,38,39,23,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,12,34,46,52,44,34,46,
52,44,13,9,0,25,35,53,36,37,3,42,15,31,53,46,46,46,36,37,1,0,6,16,31,51,46,40,40,40,40,39,22,0,0,5,14,30,47,51,49,47,47,36,37,3,0,42,15,31,50,54,49,52,49,49,54,50,31,15,42,4,20,38,30,51,46,38,38,49,30,46,48,38,38,47,51,30,36,37,3,0,23,20,38,52,44,48,45,27,15,15,17,0,0,0,0,0,7,7,11,27,50,50,36,30,29,21,0,0,0,13,44,31,51,31,44,44,44,50,49,44,28,12,0,7,13,28,28,41,52,38,38,52,41,28,28,13,7,0,22,40,27,27,27,46,51,31,27,27,15,8,0,0,0,0,0,0,0,0,
19,37,36,51,53,49,49,49,49,50,51,31,15,1,0,0,0,0,0,0,0,0,21,29,45,53,35,25,1,3,0,0,0,0,0,0,23,39,46,51,52,50,44,37,8,0,0,0,0,0,0,0,23,39,46,51,52,50,44,37,8,0,0,0,0,0,8,43,35,49,49,52,36,14,21,19,0,0,0,0,0,21,29,45,53,35,25,12,34,46,52,44,13,1,19,0,0,0,0,10,20,40,33,33,33,49,49,33,22,1,7,0,7,18,35,50,51,49,49,49,49,51,50,33,23,0,4,20,30,51,50,49,49,49,49,47,35,37,6,0,0,4,13,29,35,53,45,27,27,27,27,27,27,32,12,19,37,36,51,47,
28,12,0,0,0,10,20,40,33,33,33,49,49,33,22,0,0,0,19,43,44,53,47,29,14,30,54,49,49,49,49,45,39,17,0,8,16,38,51,31,15,1,1,0,0,0,0,21,29,30,30,46,51,54,51,46,30,30,39,17,0,0,22,28,41,46,48,49,49,53,51,36,37,3,0,0,0,5,14,30,51,38,20,4,0,22,29,46,51,51,30,28,10,0,0,0,0,19,37,36,51,47,40,40,35,48,54,54,52,36,14,12,0,0,0,0,0,0,11,33,45,53,31,44,44,46,54,49,49,49,49,45,29,21,0,0,0,13,35,53,45,39,35,53,35,44,50,27,33,49,50,27,24,0,0,19,37,36,51,45,
45,51,49,35,16,16,16,17,0,0,8,16,38,51,31,15,16,38,51,31,15,1,1,0,0,2,14,30,53,35,14,30,53,35,25,0,0,25,35,53,36,37,1,42,15,31,49,33,32,32,32,13,3,0,6,16,31,51,50,49,49,49,49,33,22,0,0,5,14,44,44,52,47,44,44,44,37,3,0,1,15,33,30,51,36,47,36,36,51,30,33,15,1,4,20,33,35,53,45,33,35,53,35,44,50,27,33,49,50,27,33,37,3,7,18,41,30,51,53,51,30,31,31,27,11,0,0,0,0,0,3,22,37,27,50,50,35,28,18,4,0,0,0,25,40,31,51,31,40,33,31,51,47,40,32,12,0,3,37,36,
47,45,51,47,47,51,45,47,36,37,3,0,5,14,44,44,36,53,45,27,25,24,17,0,0,0,0,0,0,0,0,0,19,37,36,51,49,47,47,47,47,49,51,31,15,1,0,0,0,0,0,0,0,0,21,29,45,53,35,15,15,18,6,0,0,0,0,0,6,43,35,50,51,30,28,17,0,0,0,0,0,0,0,0,6,43,35,50,51,30,28,17,0,0,0,0,0,9,11,39,47,50,35,45,49,35,15,43,5,0,0,0,0,21,29,45,53,35,15,15,34,46,52,44,15,15,43,4,0,0,0,0,12,22,23,23,33,49,49,33,15,15,25,7,0,10,32,36,46,47,45,47,47,52,50,33,23,0,4,20,
30,51,48,47,47,47,50,51,46,29,26,0,0,0,1,22,40,48,50,35,16,37,20,39,41,18,7,19,37,36,51,47,28,12,0,0,0,0,12,25,15,15,33,49,49,33,22,0,0,0,21,32,30,52,36,14,14,30,54,45,47,47,47,47,39,17,0,8,16,38,51,31,15,15,37,21,0,0,0,21,29,47,45,48,54,54,54,48,45,45,39,17,0,1,37,41,53,52,45,47,47,49,51,36,37,3,0,0,0,12,32,46,51,31,15,6,0,10,28,30,51,51,38,32,26,0,0,0,0,19,37,36,51,53,49,49,50,51,45,50,52,36,20,12,0,0,0,0,0,0,5,15,35,46,52,52,52,52,54,45,
47,47,47,47,29,21,0,0,0,23,33,49,50,27,36,48,40,27,50,44,35,53,45,39,10,0,0,19,37,36,51,54,52,47,51,46,39,23,8,0,0,0,8,16,38,51,31,15,16,38,51,31,15,15,37,21,0,4,20,38,50,27,20,38,50,27,11,0,0,23,33,48,30,28,11,23,15,31,49,35,35,35,35,13,0,0,6,16,31,51,48,47,47,47,47,33,22,0,0,5,20,33,44,52,47,33,33,33,43,3,0,19,18,32,30,51,36,31,46,36,51,30,32,18,19,0,17,23,33,49,50,27,36,48,40,27,50,44,35,53,45,39,23,21,0,7,18,35,38,51,53,51,36,35,35,27,11,0,0,0,
0,0,21,28,41,46,53,50,27,38,32,21,0,0,0,5,15,31,51,49,49,49,52,50,41,15,17,9,0,3,37,44,44,49,48,44,44,48,49,44,44,37,3,0,5,14,30,52,51,30,33,37,12,0,0,0,0,0,0,0,0,0,0,0,19,37,36,51,47,28,28,28,28,46,51,31,15,1,0,0,0,0,0,0,0,0,21,29,45,53,31,31,31,32,2,0,0,0,0,0,0,17,39,48,52,44,18,3,0,0,0,0,0,0,0,0,10,25,39,48,52,44,18,11,42,0,0,0,0,21,14,31,52,31,32,44,50,46,31,29,21,0,0,0,0,21,29,45,53,31,31,31,31,47,52,31,31,31,28,21,
0,0,0,0,0,0,0,22,33,49,49,31,31,31,16,6,0,9,22,16,28,28,31,33,27,53,50,33,23,0,4,20,30,51,30,28,28,29,41,52,49,33,22,0,0,0,0,2,20,36,53,45,31,36,38,45,41,18,7,19,37,36,51,47,28,12,0,0,0,0,8,16,31,31,31,50,49,33,22,0,0,0,24,27,49,52,30,30,30,45,51,38,28,28,28,28,14,21,0,8,16,38,51,30,31,31,40,22,0,0,0,12,14,39,39,30,51,47,52,46,39,39,34,26,0,2,14,30,51,46,40,28,29,47,51,36,37,3,0,0,0,21,29,47,51,41,37,3,4,37,44,53,48,49,50,35,43,8,0,0,
0,19,37,36,51,48,46,46,30,36,33,31,52,48,27,13,3,0,0,0,0,0,0,21,20,36,52,45,44,46,51,31,28,28,28,28,14,12,0,0,0,21,29,47,52,44,30,46,28,39,45,31,36,51,30,14,12,0,0,19,37,36,51,51,30,33,48,52,36,20,12,0,0,0,8,16,38,51,31,15,16,38,51,30,31,31,33,22,0,1,37,36,45,33,37,36,45,33,22,0,0,12,14,31,53,36,33,33,41,45,52,49,49,49,44,13,0,0,4,16,31,51,46,28,28,28,28,32,10,0,0,5,14,30,49,51,53,49,49,36,37,3,0,42,15,31,45,51,46,46,50,46,51,45,31,15,42,0,0,21,
29,47,52,44,30,46,28,39,45,31,36,51,30,14,12,0,0,0,26,20,38,52,44,48,49,44,15,25,2,0,0,0,0,0,5,20,44,27,50,53,38,27,14,12,0,0,0,1,15,31,51,47,46,46,30,35,14,22,0,0,0,3,43,33,44,53,30,33,33,30,52,44,33,37,3,0,5,14,44,41,46,52,46,39,23,0,0,0,0,0,0,0,0,0,0,0,19,37,36,51,47,28,21,21,32,46,51,31,15,1,0,0,0,0,0,0,0,0,21,29,46,46,46,51,46,32,2,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,0,19,37,35,35,48,52,44,35,29,26,0,0,0,
1,43,35,46,46,33,24,28,30,52,45,29,21,0,0,0,0,21,29,46,46,46,46,46,46,46,46,46,52,47,28,21,0,0,0,0,0,0,0,22,33,46,46,47,51,31,16,6,0,0,0,4,21,16,35,33,27,53,50,33,23,0,4,20,30,51,30,14,21,25,35,53,50,33,23,0,0,0,0,0,23,28,41,47,50,49,45,30,44,18,7,19,37,36,51,47,28,12,0,0,0,0,8,16,38,51,47,46,46,33,22,0,0,4,15,36,52,49,49,49,49,50,51,38,20,23,23,23,23,12,0,8,16,38,46,46,53,49,40,22,0,0,0,0,21,15,44,53,45,33,47,53,41,20,21,7,0,12,34,46,
51,36,15,21,29,47,51,36,37,3,0,4,23,18,27,49,53,35,13,9,24,40,45,53,41,36,52,46,39,23,0,0,0,19,37,36,51,47,28,32,14,15,25,40,47,51,30,28,10,0,0,0,0,0,0,24,33,45,52,41,32,46,51,31,15,23,23,23,23,2,0,0,0,4,20,38,51,31,45,31,20,14,30,46,30,51,36,15,42,0,0,19,37,36,51,47,40,16,41,53,48,27,13,19,0,0,8,16,38,46,31,15,16,38,46,46,53,49,33,22,0,4,43,29,29,32,43,29,29,32,23,0,0,9,24,39,31,48,49,49,47,30,49,33,33,33,33,13,0,4,37,32,31,51,46,32,14,21,21,12,
7,0,0,5,20,33,44,52,47,33,33,33,43,2,0,42,15,44,30,51,36,44,48,54,54,30,44,15,42,0,0,5,20,38,51,31,45,31,20,14,30,46,30,51,36,15,42,0,0,0,8,20,38,52,44,41,50,45,27,18,42,0,0,0,0,0,12,20,33,31,53,50,27,25,12,0,0,0,0,1,15,31,51,31,32,32,14,43,17,0,0,0,0,3,37,36,50,51,48,48,48,48,52,50,36,37,3,0,3,24,18,43,33,48,53,41,37,8,0,0,0,0,0,0,0,0,0,0,19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,0,0,0,0,12,20,34,34,44,52,46,32,2,0,
0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,0,19,37,36,45,50,51,45,45,29,26,0,0,0,1,43,34,34,34,37,5,25,35,53,45,29,21,0,0,0,0,12,20,34,34,34,34,34,34,34,34,35,53,47,28,21,0,0,0,0,0,0,0,10,32,34,34,31,51,31,16,6,0,0,0,0,0,22,25,24,27,53,50,33,23,0,4,20,30,51,30,14,4,24,27,53,50,33,23,0,0,0,0,0,7,22,15,29,47,31,29,14,18,17,0,19,37,36,51,47,28,12,0,0,0,0,8,16,38,51,31,34,34,14,10,0,0,10,29,47,53,44,33,33,33,30,51,38,33,33,33,33,
33,18,9,1,43,34,34,33,49,49,40,22,0,0,0,9,24,40,45,53,41,16,44,50,48,33,25,7,0,5,14,30,51,46,40,29,29,47,51,36,37,3,0,23,40,35,46,51,48,40,22,21,14,31,52,46,29,39,47,52,36,20,12,0,0,19,37,36,51,47,28,12,2,8,4,37,35,49,50,44,37,4,0,0,0,0,12,20,31,52,47,39,32,46,51,31,33,33,33,33,33,13,0,0,0,3,37,41,52,49,53,44,43,15,36,53,50,53,35,13,9,0,0,19,37,36,51,47,28,22,29,46,51,30,29,22,0,0,1,43,34,34,34,18,43,34,34,33,49,49,33,22,0,12,34,38,38,44,34,38,
38,44,13,9,0,0,1,24,14,33,27,27,29,31,49,33,37,37,37,25,3,2,14,31,30,51,47,31,39,10,0,0,0,0,0,21,32,29,44,52,47,29,29,29,29,37,6,0,23,14,30,51,36,14,30,51,54,30,14,23,0,0,0,3,37,41,52,49,53,44,43,15,36,53,50,53,35,13,9,0,0,0,8,20,38,52,44,34,31,52,46,39,24,7,0,0,0,0,12,14,30,27,50,50,27,11,0,0,0,0,0,1,15,31,51,31,15,2,5,3,0,0,0,0,0,3,43,39,45,49,33,39,39,33,49,48,39,43,3,0,0,0,9,12,14,31,51,47,29,22,0,0,0,0,0,0,0,0,0,0,
19,37,36,51,47,28,12,12,32,46,51,31,15,1,0,0,0,0,0,0,0,0,0,2,12,18,44,48,46,32,2,0,0,0,0,0,0,17,39,48,52,44,18,9,0,0,0,0,0,0,0,3,18,29,39,48,52,44,29,14,21,0,0,0,0,4,12,12,12,5,0,25,35,48,45,29,21,0,0,0,0,0,2,12,12,12,12,12,12,12,25,35,48,47,28,21,0,0,0,0,0,0,0,9,12,12,15,31,48,31,16,6,0,0,0,0,0,0,0,24,27,53,50,33,23,0,4,20,30,51,30,14,4,24,27,53,50,33,23,0,0,0,0,0,0,0,21,28,47,31,15,5,3,0,0,19,37,36,51,47,
28,12,0,0,0,0,8,16,38,48,31,15,12,12,9,0,7,13,35,50,45,39,22,22,14,30,54,50,49,49,49,49,44,18,9,0,4,12,22,33,48,48,40,22,0,0,0,21,14,31,52,46,29,22,28,30,51,30,34,17,0,19,43,35,49,52,48,47,47,49,51,36,37,3,0,11,27,50,52,48,41,20,2,18,27,49,49,35,43,37,44,50,48,27,13,3,0,19,37,36,51,47,28,12,0,0,0,26,14,31,52,47,39,11,0,0,0,9,25,33,48,53,41,37,32,46,54,50,49,49,49,49,44,13,0,0,0,0,25,27,50,54,50,27,24,13,35,53,54,49,33,23,0,0,0,19,37,36,51,47,
28,12,18,27,49,53,41,15,5,0,0,4,12,12,12,8,4,12,22,33,48,48,33,22,0,12,34,46,52,44,34,46,52,44,13,9,0,0,0,9,12,22,24,11,15,31,50,36,36,36,36,20,4,42,13,16,31,51,46,32,37,12,0,0,0,0,0,17,39,45,45,51,49,45,45,45,38,20,4,0,4,14,30,51,36,37,44,52,54,30,14,5,0,0,0,0,25,27,50,54,50,27,24,13,35,53,54,49,33,23,0,0,0,0,8,20,38,52,44,13,39,46,52,31,34,22,0,0,0,0,3,13,32,27,50,50,27,11,0,0,0,0,0,1,15,31,51,31,15,42,0,0,0,0,0,0,0,9,13,35,
53,46,28,10,10,34,46,53,35,13,7,0,0,0,0,9,25,27,49,53,44,43,1,0,0,0,0,0,0,0,0,0,19,37,36,46,46,28,12,12,32,46,46,31,15,1,0,0,0,0,0,0,0,0,0,0,9,13,40,40,40,14,2,0,0,0,0,0,0,17,39,46,46,44,18,9,0,0,0,0,0,0,0,0,4,17,39,46,46,44,18,12,0,0,0,0,0,0,0,0,0,0,0,24,40,40,40,32,21,0,0,0,0,0,0,0,0,0,0,0,0,0,25,40,40,40,32,12,0,0,0,0,0,0,0,0,0,1,37,40,40,40,15,42,0,0,0,0,0,0,0,24,27,46,46,33,23,0,4,20,
30,46,30,14,4,24,27,46,46,33,23,0,0,0,0,0,0,0,21,28,44,44,15,42,0,0,0,19,37,36,46,46,28,12,0,0,0,0,6,15,40,40,40,37,1,0,0,0,2,14,38,46,31,20,5,5,14,30,46,46,46,46,46,46,44,18,9,0,0,0,22,39,40,40,29,22,0,0,3,18,35,46,46,35,18,1,13,27,46,46,44,37,1,0,21,20,27,31,46,46,46,46,46,36,37,3,0,11,27,30,41,33,20,10,22,29,46,46,31,14,21,17,34,30,46,30,28,17,0,19,37,36,46,46,28,12,0,0,0,9,11,39,46,46,36,14,12,0,0,12,14,31,46,46,29,22,32,46,46,46,
46,46,46,46,44,13,0,0,0,0,22,39,46,46,46,39,10,23,33,46,46,46,29,26,0,0,0,19,37,36,46,46,28,12,12,14,36,46,46,33,25,0,0,0,0,0,0,0,0,0,22,29,40,40,29,22,0,12,34,46,46,44,34,46,46,44,13,9,0,0,0,0,0,0,0,42,15,31,46,46,46,46,30,20,4,0,6,16,31,46,46,32,2,0,0,0,0,0,0,17,39,46,46,46,46,46,46,46,38,20,4,0,4,14,30,46,36,37,33,46,46,30,14,5,0,0,0,0,22,39,46,46,46,39,10,23,33,46,46,46,29,26,0,0,0,0,8,20,38,46,44,13,18,27,46,46,41,20,2,0,
0,0,0,8,11,27,46,46,27,11,0,0,0,0,0,1,15,31,46,31,15,42,0,0,0,0,0,0,0,4,20,31,46,36,37,42,1,37,41,46,31,20,5,0,0,0,0,0,21,32,30,46,30,28,26,0,0,0,0,0,0,0,0,0,7,13,34,34,34,20,2,5,16,34,34,34,18,3,0,0,0,0,0,0,0,0,0,0,0,5,22,22,22,26,0,0,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,0,0,0,0,21,14,34,34,34,25,0,0,0,0,0,0,0,0,0,0,0,0,4,22,22,22,17,7,0,0,0,0,0,0,0,0,0,0,0,0,0,4,22,22,22,17,9,
0,0,0,0,0,0,0,0,0,0,12,22,22,22,12,0,0,0,0,0,0,0,0,23,32,34,34,32,22,0,6,37,34,34,34,37,8,23,32,34,34,32,22,0,0,0,0,0,0,0,6,25,18,18,23,0,0,0,0,7,13,34,34,34,20,2,0,0,0,0,0,12,22,22,22,12,0,0,0,0,5,16,34,34,34,25,9,8,37,34,34,34,34,34,34,34,34,25,0,0,0,0,1,10,22,22,10,19,0,0,3,18,34,34,34,14,21,0,12,16,34,34,34,43,1,0,0,21,13,20,32,34,34,34,34,34,13,7,0,22,32,14,37,24,12,0,22,32,34,34,34,24,9,3,25,34,34,34,14,17,
0,7,13,34,34,34,20,2,0,0,0,0,8,43,34,34,34,20,12,0,0,12,20,34,34,34,37,4,15,34,34,34,34,34,34,34,34,24,0,0,0,0,12,20,34,34,34,20,12,21,14,34,34,34,15,5,0,0,0,7,13,34,34,34,20,2,0,11,34,34,34,34,25,0,0,0,0,0,0,0,0,0,1,10,22,22,10,1,0,5,16,34,34,34,16,34,34,34,24,0,0,0,0,0,0,0,0,19,43,34,34,34,34,34,34,37,42,0,1,43,34,34,34,15,4,0,0,0,0,0,0,21,14,34,34,34,34,34,34,34,34,37,42,0,8,37,34,34,34,18,14,34,34,34,37,8,0,0,0,0,
12,20,34,34,34,20,12,21,14,34,34,34,15,5,0,0,0,0,1,43,34,34,34,24,2,37,34,34,34,16,2,0,0,0,0,0,22,32,34,34,32,22,0,0,0,0,0,19,18,34,34,34,43,19,0,0,0,0,0,0,0,4,15,34,34,34,24,0,0,11,34,34,34,15,4,0,0,0,0,0,3,18,34,34,34,14,21,0,0,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,5,12,12,12,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,12,12,12,1,0,0,0,0,0,0,0,0,0,0,0,12,12,12,12,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,12,12,12,12,7,0,0,4,12,12,12,4,0,3,12,12,12,12,7,0,0,0,0,0,0,0,0,0,9,9,0,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,12,12,42,0,0,4,12,12,12,12,12,12,12,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,5,12,12,12,4,0,0,0,0,
7,4,12,12,12,12,12,12,6,0,0,7,12,5,1,0,0,0,7,12,12,12,12,1,0,0,1,12,12,12,12,0,0,0,8,12,12,12,2,0,0,0,0,0,0,4,12,12,12,2,0,0,0,0,2,12,12,12,4,0,5,12,12,12,12,12,12,12,12,19,0,0,0,0,0,2,12,12,12,2,0,0,12,12,12,12,5,0,0,0,0,0,8,12,12,12,2,0,0,3,12,12,12,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,12,12,5,12,12,12,1,0,0,0,0,0,0,0,0,0,4,12,12,12,12,12,12,4,0,0,0,4,12,12,12,5,0,0,0,0,
0,0,0,0,2,12,12,12,12,12,12,12,12,4,0,0,0,4,12,12,12,8,2,12,12,12,4,0,0,0,0,0,0,2,12,12,12,2,0,0,12,12,12,12,5,0,0,0,0,0,0,4,12,12,12,19,0,5,12,12,12,5,0,0,0,0,0,0,7,12,12,12,12,7,0,0,0,0,0,0,4,12,12,12,4,0,0,0,0,0,0,0,0,0,5,12,12,12,19,0,0,19,12,12,12,5,0,0,0,0,0,0,0,8,12,12,12,12,0,0,0,0,0,0,0,0,0,0,0,0,0,3,19,19,19,0,0,0,0,0,0,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,19,
19,19,3,0,0,0,0,0,0,0,0,0,19,19,19,19,19,3,0,0,0,0,0,0,0,1,42,42,42,42,42,42,42,7,0,0,0,0,19,19,19,7,0,0,19,19,19,3,0,0,0,7,4,4,19,0,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,19,19,9,0,0,0,0,0,0,0,3,1,1,7,0,0,0,0,0,0,0,0,0,3,19,19,19,19,19,19,19,19,19,19,3,0,0,0,3,19,19,19,19,19,19,19,19,19,19,3,0,0,
0,3,19,19,19,19,19,19,19,19,19,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,2,12,2,6,0,0,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,0,0,0,9,4,12,12,8,
0,0,0,0,0,0,0,0,0,9,4,12,12,4,9,0,0,0,0,0,0,0,0,4,12,12,5,42,0,0,0,0,3,42,42,42,9,0,42,42,42,1,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,42,4,4,1,0,0,0,0,0,0,0,0,19,4,4,1,0,0,0,0,0,0,0,3,3,0,0,0,0,3,19,0,0,0,0,0,0,3,4,2,12,5,19,0,1,2,12,5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,4,12,12,8,0,9,4,12,12,8,0,0,0,0,0,0,0,9,4,12,12,4,7,0,0,0,0,0,
0,0,12,43,37,37,37,22,0,0,0,0,0,0,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,11,37,37,37,37,37,37,13,10,19,0,0,0,12,37,15,15,15,15,15,15,15,24,9,0,0,22,37,37,37,18,4,26,37,37,37,37,12,0,12,13,20,14,43,17,0,0,0,11,37,37,37,18,4,0,0,0,0,0,0,0,0,0,0,17,13,13,13,17,0,0,0,0,0,0,0,0,0,0,2,11,11,5,0,0,0,0,0,0,0,0,0,0,0,1,10,13,37,37,13,10,19,0,0,0,0,5,43,37,37,25,2,7,0,0,0,0,
0,0,2,43,37,37,37,37,37,37,37,37,37,37,18,4,0,12,43,37,37,37,37,37,37,37,37,37,37,18,5,0,12,43,37,37,37,37,37,37,37,37,37,37,18,5,0,0,0,0,0,5,21,21,5,0,0,0,0,0,0,0,0,0,0,5,21,21,5,0,0,0,0,0,0,0,0,0,9,4,12,12,4,21,13,26,0,0,0,0,0,0,0,8,12,2,6,5,22,4,0,0,0,0,0,3,4,2,12,2,6,17,13,21,0,0,0,0,0,12,22,11,11,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,22,18,17,0,0,26,18,23,5,9,0,0,0,0,0,0,0,0,0,0,
22,43,20,14,32,14,15,24,4,0,0,0,0,0,12,25,16,32,32,20,18,17,0,0,0,0,12,25,20,32,32,16,24,4,0,0,0,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,0,5,25,16,32,32,14,15,13,12,0,1,13,15,15,15,11,22,15,15,15,43,4,0,0,0,21,13,10,0,0,0,21,13,10,0,0,0,12,25,15,14,14,37,23,1,0,0,0,0,7,22,37,20,14,37,22,19,0,0,0,0,5,18,18,4,0,0,8,13,43,2,0,0,0,0,22,43,20,32,32,14,43,10,37,14,32,20,13,21,0,0,0,0,0,0,0,3,23,11,11,23,1,0,0,0,0,0,
0,0,12,13,20,32,32,15,11,25,20,32,32,15,11,4,0,0,0,0,12,13,20,32,32,20,13,21,0,0,0,0,0,0,10,39,36,36,44,13,1,0,0,0,0,0,0,19,37,36,36,36,28,12,0,0,0,0,0,0,0,3,37,36,36,36,28,21,0,0,0,0,0,0,19,37,36,36,36,36,36,41,35,29,13,8,0,0,10,39,31,31,31,31,31,31,36,37,19,0,9,18,44,36,36,29,26,25,27,36,36,33,23,5,37,27,38,30,44,20,21,0,19,37,36,36,36,28,12,0,0,0,0,0,0,0,0,0,3,43,44,44,44,37,19,0,0,0,0,0,0,0,0,0,25,27,33,13,17,3,
0,0,0,0,0,0,0,0,5,18,29,35,36,36,35,29,13,4,0,0,0,21,29,36,36,29,14,25,2,9,7,7,0,0,22,40,36,36,36,36,36,36,36,36,36,36,29,26,0,22,40,36,36,36,36,36,36,36,36,36,36,39,17,0,22,40,36,36,36,36,36,36,36,36,36,36,39,17,0,0,0,0,1,37,28,28,37,8,0,0,0,0,0,0,0,0,1,37,28,28,37,8,0,0,0,0,0,0,0,12,13,20,32,32,16,15,35,20,21,0,0,0,0,5,24,16,32,14,37,13,40,15,23,7,0,0,22,43,20,14,32,14,15,20,35,15,12,0,0,0,12,15,33,27,39,25,19,0,0,0,
0,0,0,0,0,0,0,0,19,21,25,20,39,44,13,9,0,25,35,40,20,25,21,1,0,0,0,0,0,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,0,12,15,33,31,46,46,38,44,18,9,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,1,18,33,31,46,46,30,36,27,11,0,5,14,31,31,36,37,25,35,31,31,28,21,0,0,12,37,35,20,22,11,22,15,35,20,26,0,0,25,27,36,30,30,41,39,24,0,0,0,0,23,28,44,30,30,41,29,24,9,0,0,4,18,33,33,13,8,1,25,40,27,43,4,0,0,19,
37,44,31,30,46,30,44,32,44,30,46,38,27,20,21,0,0,0,0,0,0,21,28,27,27,29,26,0,0,0,0,0,0,21,16,27,31,46,46,36,39,33,31,46,30,36,40,18,4,0,0,0,24,27,31,46,46,38,35,14,10,0,0,0,0,0,10,39,48,52,44,20,43,1,0,0,0,0,3,24,37,36,51,47,28,13,22,0,0,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,0,8,37,36,51,51,52,52,51,50,30,39,11,0,0,10,39,48,49,49,49,52,51,36,37,19,0,0,11,33,49,49,33,23,43,41,52,53,35,13,11,40,45,51,51,49,27,25,0,19,37,36,51,47,28,
12,0,0,0,0,0,0,0,0,0,12,14,30,52,30,32,12,0,0,0,0,0,0,0,0,0,22,39,30,39,29,10,0,0,0,0,0,0,0,4,43,33,35,32,15,15,28,35,33,18,6,0,0,5,43,44,45,45,38,33,20,13,18,13,6,0,22,40,44,39,39,39,39,39,39,39,39,36,29,26,0,22,40,44,39,39,39,39,39,39,39,39,36,39,17,0,22,40,44,39,39,39,39,39,39,39,39,36,39,17,0,0,0,0,8,20,38,30,14,4,0,0,0,0,0,0,0,0,8,20,38,30,14,4,0,0,0,0,0,0,21,16,27,31,46,46,31,27,46,39,22,0,0,0,5,37,33,36,46,30,
41,39,30,41,14,2,0,19,37,44,31,30,46,30,36,35,30,39,10,0,0,5,37,27,35,35,31,29,11,7,0,0,0,0,0,0,0,0,4,22,43,34,27,31,45,44,13,9,0,25,35,45,31,27,28,37,22,5,0,0,0,0,0,19,37,36,47,30,30,45,52,46,29,10,0,0,1,43,35,45,51,48,47,48,44,18,9,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,26,29,46,53,46,30,46,49,27,11,0,5,14,30,51,36,37,25,35,53,47,28,21,0,0,25,27,45,44,39,27,40,27,45,44,43,3,0,25,35,41,35,35,45,
31,20,5,0,0,5,20,36,49,44,44,48,31,14,12,0,0,22,39,46,30,39,25,24,39,30,46,40,22,0,0,19,37,36,47,30,30,45,50,31,50,47,30,49,48,35,37,6,0,0,0,0,0,21,29,47,45,29,26,0,0,0,0,0,42,43,35,48,53,45,45,52,30,47,50,30,46,53,46,40,11,0,0,0,24,27,49,47,45,52,49,44,16,5,0,0,0,0,10,39,48,52,44,36,28,17,0,0,0,0,2,14,44,36,51,47,44,44,37,3,0,0,0,0,0,3,37,36,51,47,28,21,0,0,0,0,0,8,37,14,36,51,47,44,44,36,50,52,36,15,8,0,10,29,40,40,40,35,49,48,
35,37,19,0,0,17,29,45,53,35,13,20,38,49,50,36,15,43,41,52,53,45,52,44,18,9,19,37,36,51,47,28,12,0,0,0,0,0,0,0,0,0,23,33,48,50,48,33,11,0,0,0,0,0,0,9,1,19,4,20,31,36,40,13,21,0,0,0,0,0,0,11,40,27,43,25,12,6,26,15,35,39,23,0,0,9,18,44,52,54,51,48,31,44,44,32,12,0,22,40,44,18,10,10,10,10,10,10,37,36,29,26,0,22,40,44,18,10,10,10,10,43,40,27,36,39,17,0,22,40,44,33,27,43,10,10,37,35,39,36,39,17,0,0,0,0,8,20,38,30,14,4,0,0,0,0,0,5,22,22,
22,20,38,30,14,22,22,22,2,0,0,42,43,35,48,53,45,45,52,49,27,18,8,0,0,9,25,33,47,49,46,45,52,30,50,44,43,1,0,19,37,36,47,30,30,45,52,45,33,13,1,0,0,26,29,44,30,44,39,31,28,22,9,0,0,0,0,3,21,25,20,39,44,30,49,45,31,27,13,9,0,25,27,31,45,50,46,44,39,20,25,21,19,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,21,28,46,51,30,33,28,40,44,18,9,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,22,40,49,46,29,14,28,33,27,11,0,5,
14,30,51,41,37,25,35,53,47,28,21,0,0,10,14,36,48,47,49,48,47,38,29,23,0,0,23,32,27,44,44,30,47,28,21,0,0,17,29,45,30,32,20,38,48,40,22,0,0,2,37,27,48,30,29,29,38,48,35,15,12,0,0,19,37,35,29,14,14,40,45,51,30,39,14,44,53,46,29,26,0,0,3,2,2,21,29,47,47,29,26,2,2,19,0,0,26,29,46,51,31,29,39,30,51,53,41,32,28,38,52,36,15,8,0,0,24,27,33,28,39,31,52,45,40,22,0,0,0,8,22,39,48,51,46,35,28,10,0,0,0,0,2,14,46,52,54,51,53,36,37,3,0,0,0,0,0,3,37,36,
51,47,28,10,5,0,0,0,0,5,14,30,46,51,48,30,36,32,30,54,30,14,2,0,19,22,22,43,27,45,50,44,20,21,0,0,0,2,14,30,51,36,15,29,47,30,46,30,32,20,38,51,38,40,45,44,18,9,19,37,36,51,47,28,22,22,22,22,12,0,0,0,0,19,43,44,50,35,50,41,37,1,0,0,0,0,0,11,15,43,5,25,35,48,44,35,25,0,0,0,0,0,5,20,36,27,44,27,20,21,0,21,20,36,15,8,0,9,18,44,52,54,54,54,51,52,46,32,12,0,22,40,44,18,9,0,0,0,0,3,37,36,29,26,0,22,40,44,18,9,0,7,25,40,44,15,36,39,17,0,
22,40,44,39,46,33,18,43,27,30,39,36,39,17,0,3,2,2,2,20,38,30,14,2,2,2,19,0,0,25,33,33,33,33,38,30,33,33,33,33,13,9,0,26,29,46,51,31,29,39,46,51,30,32,12,0,0,4,20,31,53,44,32,33,46,51,30,28,10,0,0,19,37,35,29,14,14,40,45,53,35,25,0,0,0,10,39,27,35,35,31,39,36,32,22,0,0,0,0,11,32,27,36,45,49,30,44,39,20,25,21,0,0,12,25,20,39,44,30,49,45,31,27,28,24,0,0,9,11,32,27,44,41,41,46,52,44,18,7,0,23,33,49,50,27,13,21,23,16,11,0,0,23,33,49,49,27,27,27,44,
52,46,28,12,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,22,39,48,48,36,35,33,28,43,21,0,5,14,30,51,41,43,25,35,53,47,28,21,0,0,0,22,39,47,36,33,44,49,33,24,3,0,6,15,41,48,41,44,30,47,28,21,0,0,10,39,48,30,20,15,36,49,33,23,0,0,0,12,15,35,49,31,31,49,44,20,21,0,0,0,9,11,32,27,44,41,41,46,52,41,27,27,27,48,49,33,11,0,0,11,32,32,32,32,28,28,32,32,32,32,24,0,0,23,33,49,50,27,24,13,35,53,48,27,27,27,44,52,46,34,12,0,0,10,16,25,21,11,40,48,53,35,13,0,0,
8,37,39,41,49,52,44,27,14,12,4,4,3,0,2,14,35,36,51,47,35,35,37,3,0,0,0,0,7,24,28,36,51,47,39,39,13,9,0,0,0,5,14,30,48,51,50,48,36,14,30,51,46,32,12,0,0,1,13,40,46,53,36,14,17,0,0,0,0,1,37,36,51,30,32,33,49,41,31,45,39,29,47,52,44,29,33,33,13,9,19,37,36,51,47,40,40,40,40,40,37,1,0,0,0,12,32,30,45,39,47,30,34,21,0,0,0,0,3,43,41,33,25,22,39,45,46,39,11,11,22,4,0,0,17,29,35,49,50,48,35,43,24,43,24,27,28,21,0,9,18,44,52,54,54,54,54,54,46,32,
12,0,22,40,44,18,9,0,0,0,0,3,37,36,29,26,0,22,40,44,18,9,0,17,32,38,34,37,36,39,17,0,22,40,44,18,33,46,40,33,46,40,37,36,39,17,0,11,32,32,32,32,38,30,32,32,32,32,24,0,0,25,35,49,49,49,50,50,49,49,49,44,13,9,0,23,33,49,50,27,28,31,44,53,48,39,22,0,0,5,14,30,52,36,35,31,50,49,48,27,24,0,0,9,11,32,27,44,41,38,52,52,44,18,7,0,0,22,40,41,27,35,29,31,40,41,14,17,0,0,0,25,35,49,47,36,27,32,43,22,5,9,0,0,0,0,0,5,22,43,32,33,36,47,49,44,13,9,0,21,
32,31,49,49,45,47,49,52,41,18,7,0,24,27,50,49,33,22,0,0,8,9,0,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,12,14,36,48,53,53,49,46,27,13,9,5,14,30,51,41,43,25,35,53,47,28,21,0,0,0,24,27,45,39,23,32,30,44,18,7,0,5,14,30,30,14,29,46,47,28,21,0,0,21,28,46,46,29,34,30,45,39,10,0,0,0,0,21,20,44,49,50,41,14,17,0,0,0,0,21,32,31,49,49,45,47,49,51,50,50,50,50,50,50,27,24,0,0,25,35,46,46,46,46,46,46,46,46,44,13,9,0,
24,27,50,49,33,22,11,27,50,52,50,50,50,50,50,47,29,21,0,0,0,8,7,0,21,29,47,52,44,18,9,0,5,20,31,44,48,52,30,38,33,20,20,20,25,9,3,11,37,36,51,47,28,13,10,0,0,0,0,0,21,32,38,45,51,47,41,44,18,9,0,0,0,5,20,39,36,51,47,40,40,35,48,51,31,20,4,0,3,24,39,30,52,31,28,22,12,12,5,0,0,0,13,35,53,45,39,35,53,35,44,50,27,33,49,50,27,24,22,22,12,0,19,37,36,51,53,49,49,49,49,31,15,1,0,0,0,11,33,48,31,20,31,49,33,11,0,0,0,0,0,24,27,38,40,33,33,30,52,36,
27,27,39,18,3,0,22,33,38,46,27,46,30,14,29,41,32,39,39,10,0,9,18,44,52,54,54,54,54,54,46,32,12,0,22,40,44,18,9,0,0,0,0,3,37,36,29,26,0,22,40,44,18,9,1,43,35,27,13,37,36,39,17,0,22,40,44,18,43,27,30,46,33,18,37,36,39,17,0,25,35,46,46,46,47,45,46,46,46,44,13,9,0,25,35,44,44,44,38,30,44,44,44,44,13,9,0,24,27,50,49,33,41,36,27,50,49,33,22,0,0,19,43,35,48,53,53,45,36,41,52,36,37,1,0,21,32,31,49,52,50,47,49,52,41,18,7,0,0,4,18,33,44,35,27,39,36,33,44,37,
19,0,0,25,35,49,45,31,27,28,37,23,12,7,0,0,0,0,7,2,23,37,34,27,36,47,49,44,13,9,0,23,33,49,49,35,29,29,46,52,41,18,7,0,11,33,50,50,27,24,8,26,13,10,0,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,8,24,14,33,44,36,46,51,31,20,4,5,14,30,51,36,37,43,44,52,47,28,21,0,0,0,24,27,45,39,23,32,30,44,18,7,0,8,16,36,49,31,30,36,47,28,21,0,0,6,37,35,48,30,30,48,44,16,5,0,0,0,0,26,20,41,50,50,41,14,10,0,0,0,0,23,
33,49,49,35,29,29,46,52,44,27,27,27,27,27,27,24,0,0,25,35,30,30,30,30,30,30,30,30,44,13,9,0,11,27,50,49,33,11,25,27,53,48,27,27,27,27,27,27,28,21,0,0,21,18,22,4,22,39,45,53,44,13,9,0,3,25,29,33,48,52,44,14,29,30,30,36,37,3,2,14,35,36,51,47,35,35,37,3,0,0,0,0,22,40,46,36,51,49,46,40,23,0,0,0,0,0,12,37,36,51,53,49,49,50,51,45,27,25,9,0,22,28,31,52,30,29,28,28,28,28,15,4,0,0,23,33,49,50,27,36,48,40,27,50,44,35,53,45,39,10,0,0,0,0,19,37,36,51,49,47,
47,47,47,31,15,1,0,0,1,37,41,50,35,13,27,50,41,37,42,0,0,0,0,25,33,46,44,41,41,46,51,30,36,36,35,37,1,0,23,33,45,30,29,30,46,34,40,30,29,39,40,22,0,9,18,44,50,49,53,51,54,54,46,32,12,0,22,40,44,18,9,0,0,0,0,3,37,36,29,26,0,22,40,44,18,24,23,29,31,14,12,37,36,39,17,0,22,40,44,18,24,39,30,30,29,11,37,36,39,17,0,25,35,30,30,30,47,47,30,30,30,44,13,9,0,26,18,18,18,20,38,30,14,18,18,18,10,0,0,11,27,50,49,44,38,28,27,53,48,40,22,0,0,0,21,16,33,35,35,39,
16,27,50,38,20,8,0,23,33,49,54,53,31,29,46,52,41,18,7,0,0,0,5,43,27,35,44,27,40,41,30,14,2,0,0,23,32,33,36,47,49,46,41,40,14,13,21,0,0,21,13,14,40,44,30,49,45,36,27,32,24,0,0,24,27,50,48,39,37,33,48,52,41,18,7,0,10,29,47,52,31,28,16,34,35,18,9,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,17,39,39,20,37,37,44,52,30,14,5,4,20,38,51,38,28,29,30,51,47,28,21,0,0,9,22,39,47,36,33,41,48,33,24,19,0,0,23,28,35,44,33,27,
27,28,21,0,0,9,23,16,33,44,44,33,20,11,3,0,0,0,21,16,44,49,31,36,49,44,20,21,0,0,0,24,27,50,48,39,37,33,48,51,38,28,37,15,28,27,28,12,0,0,23,14,14,14,14,29,29,32,14,14,14,11,0,0,10,39,45,52,41,14,32,36,51,53,44,20,37,20,39,44,37,19,0,0,24,27,29,20,32,41,53,49,33,11,0,0,5,14,36,30,49,52,44,13,40,48,52,44,18,7,2,14,46,53,54,52,53,36,37,3,0,0,0,0,10,32,28,36,51,47,28,18,4,0,0,0,0,0,19,37,36,51,48,46,46,30,36,33,37,2,0,0,11,27,50,51,47,47,47,47,
47,46,32,2,0,0,21,29,47,52,44,30,46,28,39,45,31,36,51,30,14,12,0,0,0,0,19,37,36,51,47,28,28,28,28,28,43,19,0,0,21,34,30,47,29,10,29,47,46,28,21,0,0,0,7,18,44,35,43,18,29,47,45,33,37,37,18,26,0,0,10,39,45,53,45,53,48,33,43,14,37,35,29,26,0,9,18,44,27,33,44,46,50,53,46,32,12,0,22,40,44,18,9,0,0,0,0,3,37,36,29,26,0,22,40,44,14,27,40,35,27,25,3,37,36,39,17,0,22,40,44,18,29,38,35,44,31,28,37,36,39,17,0,23,14,14,14,14,38,30,14,14,14,14,11,0,0,9,4,4,
4,20,38,30,14,4,4,4,7,0,0,10,39,47,51,30,39,32,36,51,46,28,21,0,0,22,37,28,29,32,37,13,20,41,52,36,37,1,0,24,27,50,51,30,29,33,48,52,41,18,7,0,0,0,0,12,37,35,27,41,33,44,35,39,10,0,0,7,12,24,15,39,35,30,49,48,38,35,13,9,0,25,35,38,45,49,30,44,39,16,24,21,3,0,0,22,40,45,52,30,36,46,47,52,41,18,7,0,4,15,44,50,53,46,38,46,44,18,9,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,17,39,45,38,36,36,47,52,36,15,8,19,
37,44,53,50,46,46,31,52,47,28,21,0,0,10,32,36,48,47,50,45,47,38,29,11,0,6,16,35,35,35,35,35,35,32,2,0,0,4,20,35,35,35,35,35,35,32,2,0,0,12,37,35,48,38,29,28,31,49,35,15,12,0,0,22,40,45,52,30,36,46,38,45,53,30,36,36,46,47,28,12,0,0,7,5,5,21,29,47,45,29,26,5,5,3,0,0,4,15,41,50,49,38,30,50,45,48,48,31,36,31,47,36,37,19,0,0,24,27,46,38,30,50,53,31,14,12,0,0,3,18,35,39,48,52,44,14,36,52,49,33,11,0,2,14,44,36,51,47,44,44,37,23,23,21,0,0,9,12,37,36,
51,47,28,23,23,23,23,26,0,0,19,37,36,51,47,28,32,14,15,25,2,0,0,0,11,27,46,46,46,46,46,46,51,46,32,2,0,0,4,20,38,51,31,45,31,20,14,30,46,30,51,36,15,42,0,0,0,0,19,37,36,51,47,28,21,21,21,21,4,0,0,0,24,33,49,36,16,12,15,36,49,27,24,0,0,0,7,18,29,14,21,11,33,49,30,35,25,3,7,0,0,0,12,14,30,51,54,54,51,30,27,39,35,31,20,5,0,9,18,44,29,23,43,29,27,35,27,14,12,0,22,40,44,18,9,0,0,0,0,3,37,36,29,26,0,22,40,44,40,35,46,31,29,17,3,37,36,39,17,0,
22,40,44,28,31,44,16,20,44,36,14,36,39,17,0,7,5,5,5,20,38,30,14,5,5,5,3,0,0,23,14,14,14,14,27,27,14,14,14,14,11,0,0,4,15,41,53,49,30,30,50,49,44,37,42,0,1,37,41,46,45,46,41,35,36,49,45,33,24,0,0,22,40,45,52,30,36,46,47,52,41,18,7,0,0,0,0,0,21,16,44,27,30,44,40,27,24,0,0,0,0,0,4,22,18,32,33,36,47,44,13,9,0,25,35,45,36,27,32,43,22,4,0,0,0,0,0,5,16,44,45,48,46,27,46,46,41,18,7,0,0,10,14,44,46,48,48,47,44,18,9,0,0,22,32,44,46,48,48,45,
30,44,37,19,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,17,39,38,47,48,49,45,31,39,11,0,0,22,29,38,48,48,36,35,46,46,28,21,0,0,25,27,45,44,39,33,39,27,45,35,43,3,6,16,31,45,45,45,45,46,32,2,0,0,4,20,30,45,45,45,45,46,32,2,0,0,22,39,30,30,39,24,11,29,30,46,40,22,0,0,5,16,44,47,49,45,36,29,27,30,48,49,45,46,36,28,12,0,0,0,0,0,21,29,47,45,29,26,0,0,0,0,0,0,22,32,41,47,48,48,30,27,35,46,48,48,45,30,44,37,19,0,0,24,27,46,48,48,47,41,29,11,9,0,0,
0,21,25,39,48,52,44,36,50,52,31,14,21,0,3,24,37,36,51,47,33,33,33,33,33,15,6,0,0,3,37,36,51,47,33,33,33,33,33,15,8,0,19,37,36,51,47,28,12,2,8,0,0,0,0,0,22,32,34,34,34,34,34,44,52,46,32,2,0,0,3,37,41,52,49,53,44,43,15,36,53,50,53,35,13,9,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,42,37,41,50,27,28,28,28,27,50,36,15,6,0,0,0,4,21,12,1,37,41,38,29,20,22,0,0,0,0,0,7,13,27,48,54,54,54,51,49,45,45,33,25,9,0,9,18,44,29,26,1,17,24,25,24,17,
0,0,22,40,44,18,4,4,4,4,4,4,37,36,29,26,0,22,40,44,18,37,44,38,20,5,5,37,36,39,17,0,22,40,44,35,44,20,26,10,14,41,27,36,39,17,0,0,0,0,8,20,38,30,14,4,0,0,0,0,0,25,35,30,30,30,30,30,30,30,30,44,13,9,0,11,39,30,44,46,48,48,46,44,14,17,0,0,0,11,33,31,41,30,48,49,48,30,27,37,5,0,0,24,40,46,45,48,46,27,46,46,41,18,7,0,0,0,0,0,0,26,20,35,31,41,27,41,37,19,0,0,0,0,0,0,3,12,24,15,39,35,13,9,0,25,35,39,16,24,21,19,0,0,0,0,0,0,0,0,
17,15,39,40,29,15,34,34,34,25,0,0,0,0,10,37,28,39,40,29,20,23,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,5,43,14,29,40,40,39,20,24,1,0,0,1,24,32,40,39,20,34,34,34,20,12,0,0,12,37,35,20,10,23,22,37,35,16,21,0,1,37,29,29,29,29,29,29,20,5,0,0,8,37,29,29,29,29,29,29,20,5,0,0,8,13,33,40,13,42,1,25,39,33,18,4,0,0,0,26,37,29,40,39,20,23,13,32,39,40,39,34,15,11,1,0,0,0,0,0,21,28,33,33,29,26,0,0,0,0,0,
0,9,22,15,29,40,39,32,18,37,28,40,40,29,14,43,10,0,0,0,17,15,29,40,40,29,15,22,3,0,0,0,0,0,10,39,48,51,49,52,53,30,39,24,9,0,0,19,37,36,51,53,49,49,49,49,31,16,6,0,0,3,37,36,51,53,49,49,49,49,38,16,8,0,19,37,36,51,47,28,12,0,0,0,0,0,0,0,7,12,12,12,12,12,18,44,48,46,32,2,0,0,0,25,27,50,54,50,27,24,13,35,53,54,49,33,23,0,0,0,0,0,19,37,36,51,47,28,12,0,0,0,0,0,0,21,28,46,50,47,47,47,47,47,50,46,29,26,0,0,0,0,0,0,26,28,46,35,40,22,
0,0,0,0,0,0,0,12,20,44,48,51,54,54,51,48,35,15,2,0,0,9,18,44,29,26,0,0,0,0,0,0,0,0,22,40,44,14,14,14,14,14,14,14,14,36,29,26,0,22,40,44,14,14,29,34,14,14,14,14,36,39,17,0,22,40,44,14,14,14,14,14,14,32,14,36,39,17,0,0,0,0,8,20,38,30,14,4,0,0,0,0,0,25,35,46,46,46,46,46,46,46,46,44,13,9,0,11,29,44,16,29,40,40,28,37,17,0,0,0,0,12,20,14,43,32,39,40,39,32,18,12,0,0,0,24,39,35,39,40,29,15,34,34,34,25,0,0,0,0,0,0,0,0,17,13,16,29,27,36,
14,5,0,0,0,0,0,0,0,0,0,4,22,13,26,0,0,21,13,22,4,0,0,0,0,0,0,0,0,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,3,5,26,22,22,17,5,0,0,0,0,0,7,12,22,10,2,12,12,12,2,0,0,0,0,21,13,10,0,0,0,21,25,26,0,0,0,5,21,21,21,21,21,21,12,0,0,0,0,2,21,21,21,21,21,21,12,0,0,0,0,4,13,13,4,0,0,6,25,18,5,0,0,0,0,
0,4,17,22,10,5,0,3,12,10,22,10,12,6,0,0,0,0,0,0,0,3,22,23,23,22,19,0,0,0,0,0,0,0,0,8,21,22,22,12,19,6,21,22,22,17,2,3,0,0,0,0,0,8,21,22,22,26,4,0,0,0,0,0,0,0,10,39,46,46,46,31,35,29,25,6,0,0,0,19,37,36,46,46,46,46,46,46,31,16,6,0,0,3,37,36,46,46,46,46,46,46,38,16,8,0,19,37,36,46,46,28,12,0,0,0,0,0,0,0,0,0,0,0,0,9,13,40,40,40,14,2,0,0,0,22,39,46,46,46,39,10,23,33,46,46,46,29,26,0,0,0,0,0,19,37,36,46,46,28,
12,0,0,0,0,0,0,24,27,46,46,46,46,46,46,46,46,46,27,25,0,0,0,0,0,0,25,27,44,37,25,5,0,0,0,0,0,0,0,0,17,20,27,31,46,46,31,33,15,21,0,0,0,9,18,44,29,26,0,0,0,0,0,0,0,0,22,40,30,30,30,30,30,30,30,30,30,30,29,26,0,22,40,30,30,30,30,30,30,30,30,30,30,39,17,0,22,40,30,30,30,30,30,30,30,30,30,30,39,17,0,0,0,0,1,43,34,34,37,8,0,0,0,0,0,11,34,34,34,34,34,34,34,34,34,34,24,0,0,19,11,37,17,21,22,22,21,6,0,0,0,0,0,0,12,12,19,12,10,22,
10,12,19,0,0,0,0,42,25,37,10,22,26,5,12,12,12,1,0,0,0,0,0,0,0,0,0,9,4,26,24,37,18,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,0,0,0,0,7,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,26,14,34,34,32,20,18,10,3,0,0,0,0,7,13,34,34,34,34,34,34,34,34,43,1,0,0,7,13,34,34,34,34,34,34,34,34,43,1,0,7,13,34,34,34,20,2,0,0,0,0,0,0,0,0,0,0,0,0,0,5,22,
22,22,26,0,0,0,0,12,20,34,34,34,20,12,21,14,34,34,34,15,5,0,0,0,0,0,7,13,34,34,34,20,2,0,0,0,0,0,0,11,34,34,34,34,34,34,34,34,34,34,34,24,0,0,0,0,0,0,17,37,37,26,0,0,0,0,0,0,0,0,0,0,0,12,25,20,32,32,16,25,2,0,0,0,0,0,24,32,20,12,0,0,0,0,0,0,0,0,26,14,34,34,34,34,34,34,34,34,34,34,20,12,0,26,14,34,34,34,34,34,34,34,34,34,34,20,12,0,26,14,34,34,34,34,34,34,34,34,34,34,20,12,0,0,0,0,0,4,12,12,4,0,0,0,0,0,0,19,12,12,
12,12,12,12,12,12,12,12,1,0,0,0,9,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,12,12,12,4,7,0,0,0,0,0,0,0,8,12,12,12,12,12,12,12,12,4,0,0,0,0,6,12,
12,12,12,12,12,12,12,4,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,12,12,12,2,0,0,12,12,12,12,5,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,19,12,12,12,12,12,12,12,12,12,12,12,1,0,0,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,9,4,12,12,4,0,0,0,0,0,0,0,19,2,5,0,0,0,0,0,0,0,0,0,0,12,12,12,12,12,12,12,12,12,12,12,2,0,0,0,12,12,12,12,12,12,12,12,12,12,12,2,0,0,
0,12,12,12,12,12,12,12,12,12,12,12,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,8,12,12,4,3,0,0,0,0,9,4,12,12,4,7,0,0,0,0,0,0,0,0,19,5,12,4,19,42,42,1,0,0,0,0,0,0,4,12,12,5,1,0,0,0,0,3,42,42,42,9,0,0,0,0,0,0,0,9,4,12,12,4,9,0,0,0,0,0,0,42,42,42,42,42,42,42,42,42,42,42,0,0,0,0,0,0,0,3,1,42,42,42,42,42,42,0,0,0,7,42,42,42,42,42,42,42,42,42,3,0,0,3,42,42,42,9,42,42,42,0,0,0,0,0,0,0,19,42,42,1,0,0,9,42,42,42,0,0,0,0,7,42,42,42,42,42,42,42,42,42,42,42,42,1,
0,0,0,0,0,1,42,3,0,0,0,7,6,19,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,0,0,0,0,1,2,12,2,6,9,0,0,0,42,2,12,2,42,0,0,0,0,0,0,0,0,3,4,2,12,2,6,0,0,0,0,0,0,0,0,9,4,12,12,8,0,0,0,0,0,0,0,0,42,5,12,12,4,7,0,0,0,0,0,0,0,9,4,12,12,4,9,0,0,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,0,0,42,2,12,5,19,0,0,0,0,0,7,42,42,42,7,0,9,4,12,12,4,9,0,0,0,0,0,0,0,0,0,8,12,12,4,3,0,0,0,0,
0,0,4,12,12,5,42,0,0,0,0,0,0,19,42,42,1,0,0,9,42,42,42,0,0,0,0,7,42,42,42,7,0,1,5,12,5,1,0,0,0,0,0,0,9,4,12,12,4,9,0,0,0,0,0,0,0,0,0,0,0,5,12,19,0,0,0,0,0,0,0,0,0,0,0,8,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,12,12,6,0,0,0,0,0,0,0,3,4,2,12,5,19,0,1,2,12,5,7,0,0,0,0,0,0,0,0,6,2,12,2,42,0,0,0,0,0,0,0,0,0,9,4,12,12,4,9,0,0,0,0,0,0,0,0,0,0,19,42,42,42,0,
0,0,0,0,0,0,0,0,42,5,12,2,8,0,0,0,0,0,0,0,0,0,0,0,42,42,42,1,0,0,0,0,0,0,0,12,25,16,32,32,20,18,17,0,0,12,13,20,32,32,20,13,21,0,0,0,0,0,9,10,43,14,32,20,43,15,15,37,12,0,0,0,4,25,16,32,32,14,37,23,0,0,1,13,15,15,15,11,0,0,0,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,10,15,15,15,15,15,15,15,15,15,15,15,23,0,0,0,0,12,24,37,15,15,15,15,15,15,15,22,0,9,24,15,15,15,15,15,15,15,15,15,25,3,1,13,15,15,15,11,15,15,15,22,0,0,
0,0,0,4,43,15,15,37,12,0,11,15,15,15,22,0,0,9,24,15,15,15,15,15,15,15,15,15,15,15,15,37,2,0,0,19,11,15,15,43,23,42,12,13,16,43,21,0,0,0,0,12,25,16,32,32,20,18,17,0,0,0,0,42,23,37,14,32,14,15,25,19,3,24,15,14,32,14,37,11,4,0,0,0,0,0,22,43,20,14,32,14,15,24,4,0,0,0,0,0,12,25,20,32,32,16,24,4,0,0,0,0,1,25,15,14,32,32,20,13,12,0,0,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,0,0,12,25,16,32,32,20,18,17,0,0,1,24,15,14,32,14,37,22,19,0,
0,3,25,15,15,15,25,12,25,20,32,32,20,13,12,0,0,0,0,0,0,12,25,16,32,32,20,18,17,0,0,0,5,25,16,32,32,14,15,13,12,0,0,0,4,43,15,15,37,12,0,11,15,15,15,22,0,0,3,25,15,15,15,25,22,37,14,32,14,37,11,3,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,0,0,19,26,11,25,20,32,13,25,22,5,0,0,0,0,0,0,4,24,15,14,16,2,17,17,12,7,0,0,0,0,0,0,0,0,0,21,18,20,32,32,15,11,6,0,0,0,0,22,43,20,32,32,14,43,10,37,14,32,20,13,21,0,0,0,0,9,17,25,15,14,32,
14,37,11,8,0,0,0,0,0,0,12,13,20,32,32,16,25,2,0,0,0,0,0,0,0,0,8,18,15,15,15,10,0,0,0,0,0,0,3,24,15,14,32,14,15,11,6,0,0,0,0,0,0,0,0,10,15,15,15,37,12,0,0,0,0,0,12,15,33,31,46,46,38,44,18,9,0,24,27,31,46,46,38,35,14,10,0,0,0,0,22,32,44,30,46,38,33,31,31,39,10,0,0,3,13,33,31,46,46,30,36,15,1,0,5,14,31,31,41,43,3,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,24,27,31,31,31,31,31,31,31,31,31,44,13,9,0,0,21,16,33,41,31,31,
31,31,31,31,35,25,0,19,37,36,31,31,31,31,31,31,31,31,20,8,5,14,31,31,41,43,33,31,36,14,21,0,0,0,0,23,39,31,31,29,17,3,43,44,31,41,16,5,0,19,37,36,31,31,31,31,31,31,31,31,31,31,31,39,17,0,0,21,29,36,31,41,39,25,15,27,31,35,37,1,0,0,12,15,33,31,46,46,38,44,18,9,0,0,6,13,39,41,30,46,30,36,14,5,4,20,36,30,46,30,36,40,18,4,0,0,0,19,37,44,31,30,46,30,36,40,18,42,0,0,0,21,16,27,31,46,46,36,40,18,4,0,0,0,2,32,36,30,46,46,38,27,37,4,0,0,0,21,16,27,
31,46,46,31,33,37,12,0,0,0,0,12,15,33,31,46,46,38,44,18,9,0,21,28,36,30,46,30,44,29,24,3,0,8,16,31,31,31,15,15,27,31,46,46,31,27,16,21,0,0,0,0,12,15,33,31,46,46,38,44,18,9,0,1,18,33,31,46,46,30,36,27,11,0,0,0,23,39,31,31,29,17,3,43,44,31,41,16,5,0,8,16,31,31,31,15,29,41,30,46,30,41,32,2,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,4,13,28,27,35,41,30,35,35,39,15,17,0,0,0,0,5,43,40,36,30,28,20,29,39,14,25,6,0,0,0,0,17,22,22,17,20,35,38,46,
30,36,39,13,1,0,0,19,37,44,31,30,46,30,44,32,44,30,46,38,27,20,21,0,0,0,12,14,27,36,30,46,30,41,39,18,4,0,0,0,0,21,16,27,31,46,46,31,33,37,12,0,0,0,0,0,0,0,12,32,31,31,27,24,0,0,0,0,0,0,4,14,36,30,46,30,36,39,24,9,0,0,0,0,0,0,0,24,27,31,31,39,17,0,0,0,0,1,43,35,45,51,48,47,48,44,18,9,0,24,27,49,47,45,52,49,44,16,5,0,0,4,16,41,50,49,30,49,46,50,38,20,5,0,0,12,14,30,51,45,38,30,31,15,1,0,5,14,30,51,41,43,3,0,0,0,42,43,35,48,
53,45,45,52,45,27,18,3,0,0,24,27,49,53,51,49,49,49,51,53,49,44,13,9,0,42,43,35,48,53,45,45,52,51,50,49,35,25,0,19,37,36,49,49,49,51,50,49,49,38,20,8,5,14,30,51,41,43,20,31,49,35,13,3,0,0,8,15,41,52,36,37,6,9,22,39,45,45,39,23,0,19,37,36,50,51,49,49,49,49,49,49,50,51,45,39,17,0,0,21,29,47,45,52,30,32,33,45,51,30,14,5,0,1,43,35,45,51,48,47,48,44,18,9,0,0,23,39,30,52,45,30,46,30,14,5,4,20,30,46,30,47,52,46,40,24,0,0,0,19,37,36,47,30,30,45,52,46,29,10,
0,0,1,43,35,48,50,46,46,53,46,40,11,0,0,0,2,32,46,46,30,47,52,45,39,10,0,0,42,43,35,48,53,45,45,52,45,27,18,3,0,0,1,43,35,45,51,48,47,48,44,18,9,0,21,28,47,45,47,50,53,38,29,22,0,8,16,38,51,31,15,27,45,52,45,45,53,48,35,43,1,0,0,1,43,35,45,52,48,47,48,44,18,9,0,26,29,46,53,46,30,46,49,27,11,0,0,8,15,41,52,36,37,6,9,22,39,45,45,39,23,0,8,16,38,51,31,39,30,53,50,47,45,46,32,2,0,42,43,35,48,53,47,45,52,45,27,18,3,0,0,7,25,40,30,50,45,45,50,47,
49,45,44,20,12,0,0,9,25,33,47,52,47,28,36,45,45,38,39,24,0,0,0,5,14,33,33,37,44,49,49,30,46,53,30,39,22,0,0,19,37,36,47,30,30,45,50,31,50,47,30,49,48,35,37,6,0,0,12,28,47,48,46,30,45,52,46,39,11,0,0,0,42,43,35,48,53,47,45,52,45,27,18,3,0,0,0,0,0,0,12,32,46,53,27,24,12,12,12,12,9,0,4,14,30,30,38,48,52,31,20,4,12,12,12,12,0,0,0,24,27,53,48,39,17,0,0,0,0,21,28,46,51,30,33,29,40,44,18,9,0,24,27,33,29,39,31,52,45,40,22,0,0,17,29,47,52,41,14,41,
52,53,44,18,7,0,0,21,28,47,49,33,20,14,39,37,1,0,5,14,30,51,41,43,3,0,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,11,40,40,47,53,35,40,27,50,48,40,40,13,0,0,26,29,46,51,31,29,39,30,51,30,40,40,25,0,19,37,40,40,40,36,51,38,40,40,40,15,8,5,14,30,51,41,43,13,44,53,38,14,5,0,0,26,29,47,53,35,25,18,18,18,14,30,53,44,43,3,3,37,40,47,53,35,40,40,40,40,40,30,52,44,29,26,0,0,21,29,27,39,45,45,39,36,51,52,36,15,8,0,21,28,46,51,30,33,28,40,44,18,9,0,6,15,36,
52,46,40,14,32,40,14,5,4,20,33,34,14,39,30,52,31,20,4,0,0,19,37,35,29,14,14,40,45,50,35,25,0,0,26,29,46,53,41,32,28,38,52,36,15,8,0,0,2,32,40,32,14,39,47,49,33,22,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,21,28,46,51,30,33,28,40,44,18,9,0,21,28,35,29,29,35,49,53,44,43,3,8,16,38,51,31,34,30,51,38,39,29,31,51,46,28,21,0,0,21,28,46,51,30,40,28,40,44,18,9,0,22,40,49,46,29,14,28,33,27,11,0,0,26,29,47,53,35,25,18,18,18,14,30,53,44,43,3,8,16,38,51,31,41,
52,48,35,29,29,35,32,2,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,12,32,38,51,31,39,33,35,29,35,49,48,27,25,0,0,5,14,31,51,46,40,44,53,47,46,52,31,20,5,0,0,5,14,30,31,39,47,50,44,14,29,30,53,41,37,1,0,19,37,35,29,14,14,40,45,51,30,39,14,44,53,46,29,26,0,0,12,28,36,40,32,14,39,30,52,36,15,8,0,0,26,29,46,51,31,29,39,30,51,30,32,12,0,0,0,9,12,23,25,32,46,53,27,24,14,28,28,32,10,0,4,20,39,14,20,35,53,30,14,22,34,28,28,20,12,0,0,24,27,49,48,39,17,0,0,0,
0,23,33,49,50,27,44,45,46,34,11,0,0,10,16,44,45,46,40,48,53,35,13,0,0,22,33,49,49,33,11,27,50,48,40,23,0,0,0,4,20,36,49,31,35,35,37,21,0,0,5,14,30,51,41,43,3,0,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,4,22,28,47,53,35,13,27,50,48,39,22,5,0,0,23,33,49,50,27,24,13,35,53,48,39,22,4,0,0,12,22,22,37,36,51,38,20,22,22,21,0,5,14,30,51,41,43,24,27,53,47,28,21,0,0,23,33,49,49,33,28,44,44,44,16,31,51,38,20,4,0,23,33,49,49,33,28,44,44,44,16,31,51,38,20,
4,0,0,2,15,37,32,46,45,44,47,51,30,39,11,0,0,23,33,49,50,27,13,21,23,16,11,0,0,12,32,46,52,44,40,40,40,40,14,5,4,20,40,40,40,40,44,53,47,28,21,0,0,9,11,32,27,44,41,41,46,52,44,18,7,0,23,33,49,49,27,27,27,44,52,46,28,12,0,0,7,23,32,35,35,41,49,38,28,26,0,0,23,33,49,50,27,24,13,35,53,48,39,22,0,0,23,33,49,50,27,13,21,23,16,11,0,0,5,37,28,39,39,39,30,51,38,20,4,8,16,38,51,31,33,48,53,35,13,25,27,50,49,33,23,0,0,23,33,49,50,27,39,39,39,16,11,0,0,22,
39,48,48,36,35,33,28,43,21,0,0,23,33,49,49,33,28,44,44,44,16,31,51,38,20,4,8,16,38,51,31,30,51,31,27,27,27,37,43,8,0,23,33,49,50,27,33,33,35,53,48,39,22,0,0,23,33,49,50,27,25,25,13,10,32,30,51,36,37,1,0,17,29,45,52,44,20,30,53,44,33,49,47,29,21,0,0,8,15,36,47,27,50,45,27,27,27,41,51,30,14,5,0,9,11,32,27,44,41,41,46,52,41,27,27,27,48,49,33,11,0,0,11,39,39,39,39,39,39,44,52,46,28,12,0,0,23,33,49,50,27,33,33,35,53,48,39,22,0,0,9,23,14,33,35,35,46,53,27,24,
39,47,47,40,22,0,0,26,20,35,35,38,48,44,37,11,27,47,47,28,21,0,0,24,40,33,33,29,17,0,0,0,0,24,27,50,49,33,44,52,46,34,12,0,0,9,13,44,52,46,29,47,52,44,18,9,0,23,33,50,48,40,23,33,48,30,32,12,0,0,0,42,18,33,36,53,50,36,37,1,0,0,5,14,30,51,41,43,3,0,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,0,12,28,47,53,35,13,27,50,48,39,10,0,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,0,0,0,1,37,36,51,38,20,8,0,0,0,5,14,30,51,41,43,24,27,53,47,29,21,
0,0,24,27,50,49,40,28,47,52,44,15,31,51,30,14,5,0,24,27,50,49,40,28,47,52,44,15,31,51,30,14,5,0,0,0,8,13,39,45,53,50,51,46,40,13,42,0,0,24,27,50,49,33,22,0,0,8,9,0,0,21,28,47,51,49,49,49,49,30,14,5,4,20,30,49,49,49,49,51,45,29,26,0,0,21,32,31,49,49,45,47,49,52,41,18,7,0,24,27,50,52,50,50,50,50,50,45,29,21,0,0,0,12,32,46,49,53,38,27,16,12,0,0,24,27,50,49,33,22,11,27,50,49,33,22,0,0,24,27,50,49,33,22,0,0,8,9,0,0,0,26,29,45,48,48,49,54,30,14,
5,8,16,38,51,49,49,52,50,27,11,23,33,49,50,27,11,0,0,24,27,50,52,48,48,48,36,37,3,0,0,12,14,36,48,53,53,49,46,27,13,9,0,24,27,50,49,40,28,47,52,44,15,31,51,30,14,5,8,16,38,51,53,53,54,53,50,50,44,13,1,0,0,24,27,50,52,48,48,48,48,51,49,33,22,0,0,24,27,50,49,33,22,0,9,4,20,38,51,38,20,8,0,22,39,48,53,35,14,30,53,35,40,48,47,29,21,0,0,0,24,40,46,50,51,52,50,50,50,50,50,46,32,12,0,21,32,31,49,49,45,47,49,51,50,50,50,50,50,50,27,24,0,0,24,27,48,48,48,48,
48,48,51,45,29,21,0,0,24,27,50,52,48,48,48,48,51,49,33,22,0,0,21,32,31,48,50,49,53,53,27,24,39,45,49,40,22,0,0,4,20,30,50,53,36,33,13,11,27,50,47,28,21,0,0,4,22,22,22,22,3,0,0,0,0,11,33,50,50,27,44,46,46,34,10,0,0,21,18,44,46,46,39,45,53,44,13,9,0,22,33,49,49,33,18,44,53,46,32,12,0,0,0,26,29,46,49,41,33,33,37,26,7,0,5,14,30,51,41,43,3,0,0,0,11,27,50,49,33,11,25,27,53,48,40,22,0,0,0,12,28,47,53,35,13,27,50,48,39,10,0,0,0,11,27,50,49,33,11,25,
27,53,48,40,22,0,0,0,0,0,1,37,36,51,38,20,8,0,0,0,5,14,30,51,41,43,13,35,53,46,32,12,0,0,11,27,50,49,33,29,45,52,41,16,31,51,30,14,5,0,11,27,50,49,33,29,45,52,41,16,31,51,30,14,5,0,0,9,24,39,30,52,48,36,53,36,37,13,22,0,0,11,33,50,50,27,24,8,26,13,10,0,0,12,34,46,52,44,35,35,35,35,14,5,4,20,35,35,35,35,35,53,47,29,21,0,0,23,33,49,49,35,29,29,46,52,41,18,7,0,11,27,50,48,27,27,27,27,27,27,29,21,0,0,4,11,14,33,33,35,45,48,33,11,0,0,11,27,50,49,
33,11,25,27,53,48,40,22,0,0,11,33,50,50,27,24,8,26,13,10,0,0,42,25,29,27,27,27,38,51,30,14,5,8,16,38,51,31,35,49,50,27,24,11,27,50,49,33,23,0,0,11,33,50,50,27,27,27,27,37,10,0,0,8,24,14,33,44,36,46,51,31,20,4,0,11,27,50,49,33,29,45,52,41,16,31,51,30,14,5,8,16,38,51,31,46,51,36,44,44,44,13,24,19,0,11,27,50,49,33,39,39,35,53,48,40,22,0,0,23,33,49,49,33,11,10,22,2,14,30,51,31,15,42,0,10,39,45,53,44,15,36,52,41,35,53,31,20,12,0,0,0,4,13,29,35,53,45,27,27,
27,27,27,27,32,12,0,23,33,49,49,35,29,29,46,52,44,27,27,27,27,27,27,24,0,0,11,27,50,48,41,41,41,41,52,47,28,21,0,0,11,27,50,49,33,39,39,35,53,48,40,22,0,0,23,33,49,45,27,33,46,53,35,25,39,48,49,40,22,0,0,4,20,33,27,36,53,30,28,24,27,50,47,28,21,0,0,23,28,28,28,14,21,0,0,0,0,10,29,47,52,31,28,32,34,35,18,9,0,24,27,29,32,32,41,53,49,33,11,0,0,17,39,45,53,35,14,38,51,45,39,15,21,0,0,22,40,48,45,29,43,37,32,34,21,0,5,14,30,51,36,37,22,3,0,0,10,39,45,52,
41,14,32,36,51,46,28,21,0,0,0,12,28,47,53,35,13,27,50,48,39,11,12,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,0,0,0,1,37,36,51,38,20,23,42,0,0,4,14,30,51,36,37,32,31,51,31,16,4,0,0,22,40,48,53,44,33,49,47,38,29,46,51,36,15,6,0,22,40,48,53,44,27,49,47,38,29,46,51,36,15,6,0,0,12,14,31,52,53,35,35,53,31,15,27,15,1,0,10,29,47,52,31,28,16,34,35,18,9,0,4,20,31,51,38,28,37,15,34,20,5,4,16,28,15,37,32,31,51,38,14,5,0,0,24,27,50,48,39,37,33,48,52,41,18,
7,0,10,39,47,53,44,20,37,20,39,44,37,19,0,0,23,33,29,15,37,32,30,53,35,25,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,10,29,47,52,31,28,16,34,35,18,9,0,21,28,40,20,20,27,48,52,41,37,1,8,16,38,51,31,29,46,52,36,32,14,36,52,47,29,17,0,0,10,29,47,52,31,28,16,32,35,18,9,0,17,39,39,20,37,37,44,52,30,14,5,0,22,40,48,53,44,33,49,47,38,29,46,51,36,15,6,8,16,38,51,31,36,51,47,33,20,14,33,32,2,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,26,29,46,52,41,14,29,33,16,
33,45,50,35,13,9,0,12,14,30,51,30,39,39,30,47,46,30,40,14,20,12,0,0,0,1,22,40,48,50,35,16,37,20,39,41,18,7,0,24,27,50,48,39,37,33,48,51,38,28,37,15,28,27,28,12,0,0,10,39,45,50,27,37,14,31,51,31,20,4,0,0,10,39,45,52,41,14,32,36,51,46,28,21,0,0,11,27,50,46,28,39,45,51,36,32,35,50,45,39,10,0,0,0,26,23,11,39,48,48,33,32,36,52,46,32,12,0,0,24,27,47,47,39,17,0,0,0,0,4,15,44,50,53,46,38,46,44,18,9,0,24,27,46,38,30,50,53,31,14,12,0,0,5,20,31,51,45,31,49,
48,53,38,33,23,0,0,17,29,47,53,30,41,41,30,29,26,0,4,14,30,51,38,27,34,12,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,0,12,28,47,53,35,13,27,50,50,35,27,13,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,0,0,0,19,37,36,51,46,27,39,10,0,0,42,15,36,52,47,41,31,50,48,27,25,9,0,0,12,32,30,51,49,47,45,41,50,47,52,50,35,13,9,0,12,32,30,51,48,47,45,41,50,47,52,50,35,13,9,0,0,22,39,48,54,47,39,33,48,45,36,31,15,1,0,4,15,44,50,53,46,38,46,44,18,9,0,0,25,33,
45,53,30,36,36,30,14,5,4,20,30,36,36,30,50,48,27,13,7,0,0,22,40,45,52,30,36,46,47,52,41,18,7,0,4,15,41,50,48,31,36,31,47,36,37,19,0,0,23,33,47,31,36,38,50,48,33,11,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,4,15,44,50,53,46,38,46,44,18,9,0,21,28,47,38,38,45,51,46,39,23,0,8,16,38,51,31,15,44,50,50,30,30,49,50,44,15,4,0,0,4,15,44,50,53,46,38,30,44,18,9,0,17,39,45,38,36,36,47,52,36,15,8,0,12,32,30,51,49,47,45,41,50,47,52,50,35,13,9,8,16,38,51,31,33,
47,51,45,38,30,46,32,2,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,1,43,27,45,49,38,30,48,38,47,50,31,32,26,0,0,3,13,27,48,52,46,38,46,52,52,46,38,30,28,21,0,0,0,0,2,20,36,53,45,31,36,38,45,41,18,7,0,22,40,45,52,30,36,46,38,45,53,30,36,36,46,47,28,12,0,0,4,15,41,50,47,36,31,50,45,27,25,9,0,0,4,15,41,50,49,38,30,50,49,44,37,42,0,0,22,40,48,53,38,30,31,50,50,46,48,52,31,20,2,0,0,0,0,0,12,32,30,52,45,46,49,50,44,37,1,0,0,24,27,53,48,39,17,0,0,0,
0,0,10,14,44,46,48,48,47,44,18,9,0,24,27,46,48,48,47,41,29,11,9,0,0,9,25,40,30,48,48,45,35,31,46,33,23,0,0,8,37,27,30,48,49,48,46,29,26,0,6,15,41,48,50,47,28,12,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,12,28,46,46,35,13,39,46,50,49,44,13,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,0,0,7,18,35,48,50,48,39,10,0,0,0,11,39,38,47,48,45,30,35,20,12,0,0,0,3,13,33,46,48,45,41,40,30,48,45,36,32,26,0,0,3,13,33,46,48,45,41,40,30,48,45,36,32,26,
0,0,0,26,29,46,47,35,37,20,44,46,45,31,15,1,0,0,10,14,44,46,48,48,47,44,18,9,0,0,2,37,27,30,45,49,48,30,14,5,4,20,30,45,49,48,30,27,15,12,0,0,0,5,16,44,45,48,46,27,46,46,41,18,7,0,0,22,32,44,46,48,48,45,30,44,37,19,0,0,23,33,46,48,49,48,46,44,20,12,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,10,14,44,46,48,48,47,44,18,9,0,21,28,30,48,49,45,38,33,43,4,0,8,16,38,46,31,15,14,44,46,48,48,46,44,32,10,0,0,0,0,10,14,44,46,48,48,47,44,18,9,0,17,
39,38,47,48,49,45,31,39,11,0,0,3,13,33,46,48,45,41,40,30,48,45,36,32,26,0,8,16,38,46,31,15,27,30,45,49,45,30,32,2,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,0,12,15,33,41,31,38,45,31,36,35,34,11,9,0,0,0,12,15,27,30,48,48,47,36,31,45,49,47,28,21,0,0,0,0,0,23,28,41,47,48,48,45,38,35,18,7,0,5,16,44,47,49,45,36,29,27,30,48,49,45,46,36,28,12,0,0,0,22,32,41,47,48,48,30,27,15,12,0,0,0,0,22,32,41,47,48,48,46,44,14,17,0,0,0,2,20,41,45,48,30,39,41,47,49,
45,31,29,11,0,0,0,0,0,0,7,25,39,31,45,49,45,36,32,10,0,0,0,24,27,46,46,39,17,0,0,0,0,0,0,10,37,28,39,40,29,20,23,0,0,17,15,29,40,40,29,15,22,3,0,0,0,0,4,25,32,39,39,29,15,20,34,32,10,0,0,0,12,18,32,39,40,39,34,37,2,0,0,22,14,33,33,40,14,12,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,2,20,34,34,34,11,43,29,33,33,39,25,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,0,0,0,26,20,40,33,33,34,17,0,0,0,6,25,14,29,39,39,32,43,21,0,0,
0,0,0,5,18,28,40,39,20,13,28,40,39,20,22,9,0,0,0,5,18,28,40,39,20,13,28,40,39,20,22,9,0,0,0,42,13,28,29,37,21,17,37,28,29,32,25,7,0,0,0,10,37,28,39,40,29,20,23,0,0,0,0,12,13,14,39,40,39,34,37,8,1,43,32,39,40,39,32,18,12,0,0,0,0,0,17,15,39,40,29,15,34,34,34,25,0,0,0,9,10,37,28,40,40,29,14,43,10,0,0,0,26,15,28,39,40,39,28,37,17,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,0,10,37,28,39,40,29,20,23,0,0,4,43,32,39,40,39,14,25,5,0,
0,1,43,34,34,34,18,10,37,29,40,40,29,37,10,9,0,0,0,0,0,10,37,28,39,40,29,20,23,0,0,5,43,14,29,40,40,39,20,24,1,0,0,0,5,18,28,40,39,20,13,28,40,39,20,22,9,0,1,43,34,34,34,18,13,14,39,40,39,14,18,6,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,0,5,11,43,15,29,39,15,37,13,17,7,0,0,0,0,0,12,18,34,40,40,29,16,20,39,40,39,37,5,0,0,0,0,0,7,22,15,29,40,40,29,14,18,17,0,0,0,26,37,29,40,39,20,23,13,32,39,40,39,34,15,11,1,0,0,0,9,22,15,29,40,
39,32,18,12,0,0,0,0,0,9,22,15,29,40,40,28,37,17,0,0,0,0,0,10,20,39,40,28,13,15,29,40,39,14,24,1,0,0,0,0,0,0,0,6,24,14,39,40,39,20,22,9,0,0,0,23,32,34,34,14,21,0,0,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,8,21,22,22,26,4,0,0,0,0,0,0,0,3,12,10,10,17,5,5,12,12,9,0,0,0,0,19,12,10,22,10,12,42,0,0,0,0,12,22,23,22,21,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,2,12,12,12,19,8,10,23,23,10,8,0,0,0,0,0,8,21,22,22,
21,6,0,0,0,0,0,0,0,0,0,0,12,22,23,22,21,7,0,0,0,0,9,5,26,17,17,12,1,0,0,0,0,0,0,0,1,21,22,10,5,19,21,22,10,2,0,0,0,0,0,0,1,21,22,10,5,19,21,22,10,2,0,0,0,0,0,0,1,21,21,4,0,0,6,12,26,12,3,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,0,0,3,12,10,22,10,12,8,0,0,6,12,10,22,10,12,19,0,0,0,0,0,0,0,4,10,22,26,5,12,12,12,1,0,0,0,0,0,8,21,22,22,17,2,19,0,0,0,0,0,4,21,10,22,10,21,6,0,0,0,0,0,0,0,8,
21,22,22,21,6,0,0,0,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,19,12,10,22,10,2,7,0,0,0,0,4,12,12,12,8,0,8,21,22,22,21,8,0,0,0,0,0,0,0,0,6,21,22,22,26,4,0,0,0,0,3,5,26,22,22,17,5,0,0,0,0,0,0,1,21,22,10,5,19,21,22,10,2,0,0,0,0,4,12,12,12,8,7,12,10,22,10,12,3,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,0,0,3,42,17,22,5,1,9,0,0,0,0,0,0,0,0,19,21,22,22,26,4,5,17,22,10,4,0,0,0,0,0,0,0,0,4,26,22,
22,26,5,3,0,0,0,0,0,4,17,22,10,5,0,3,12,10,22,10,12,6,0,0,0,0,0,0,0,4,17,22,10,12,19,0,0,0,0,0,0,0,0,8,21,22,22,21,6,0,0,0,0,0,0,0,5,10,22,21,19,4,17,22,10,12,9,0,0,0,0,0,0,0,0,0,9,2,10,22,17,5,0,0,0,0,0,3,12,12,12,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,42,42,42,42,42,42,0,0,0,0,0,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,0,
0,0,0,0,0,0,0,0,3,1,1,7,0,0,0,0,19,42,42,42,42,42,42,42,42,1,0,0,0,0,0,0,0,0,4,12,12,5,1,0,0,0,0,0,0,0,7,1,42,42,42,42,4,12,2,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,42,42,42,19,2,12,42,0,19,2,12,8,0,0,0,0,7,42,42,42,3,5,12,4,0,0,0,0,0,7,42,42,42,3,5,12,4,0,0,0,
7,42,42,42,7,0,0,42,42,42,1,0,0,0,0,42,42,42,19,0,42,42,42,7,9,42,42,42,3,0,0,9,42,42,42,19,0,7,42,42,42,19,0,0,0,0,1,42,42,42,42,42,42,42,7,0,0,7,42,42,42,7,0,0,7,42,42,42,7,0,42,42,42,42,0,0,0,7,42,42,42,7,7,42,42,42,19,0,0,0,7,42,42,42,7,0,42,42,42,0,0,0,0,0,0,4,12,2,1,0,0,4,12,12,6,0,0,7,42,42,42,42,1,3,0,0,0,0,0,7,42,42,42,42,42,42,1,0,0,0,0,42,42,42,1,0,1,42,42,42,0,3,42,42,42,3,0,0,0,7,
42,42,42,7,7,42,42,42,3,0,0,7,42,42,42,7,0,42,42,42,42,0,0,0,0,0,0,42,42,42,42,42,42,42,42,0,0,0,7,42,42,42,1,0,0,42,42,42,42,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,7,42,42,42,42,42,42,42,42,7,0,0,19,42,42,42,42,42,42,42,42,1,0,0,0,9,42,42,42,19,0,7,42,42,42,19,0,0,0,0,1,42,42,42,0,1,42,42,1,0,0,0,7,42,42,42,7,9,42,42,42,3,0,42,42,42,19,0,0,7,42,42,42,42,42,42,0,0,0,0,0,0,0,0,7,42,42,42,7,0,0,0,0,0,0,0,
0,0,0,22,15,15,15,15,15,15,15,23,0,0,0,0,0,0,3,25,15,15,15,25,25,15,15,15,25,7,0,0,0,0,0,0,0,0,4,11,43,15,15,18,23,4,0,8,18,15,15,15,15,15,15,15,15,15,21,0,0,0,0,0,4,25,16,32,32,14,37,23,0,0,0,0,3,22,18,15,15,15,15,15,20,32,14,37,23,1,0,0,0,0,0,5,22,11,11,17,42,42,42,1,0,0,0,0,0,0,0,0,12,13,23,1,0,0,0,0,0,0,0,0,0,0,0,0,0,9,26,37,16,10,0,0,0,0,0,0,0,0,0,0,0,0,5,21,12,0,3,25,15,15,15,18,14,32,37,10,
43,14,32,15,22,9,0,3,25,15,15,15,13,14,32,20,24,1,0,0,3,25,15,15,15,13,14,32,15,2,0,3,25,15,15,15,25,7,17,15,15,15,37,12,0,0,22,15,15,15,18,10,15,15,15,24,11,15,15,15,25,3,0,11,15,15,15,43,4,25,15,15,15,18,6,0,0,12,37,15,15,15,15,15,15,15,24,9,3,25,15,15,15,25,7,3,25,15,15,15,25,10,15,15,15,15,23,0,3,25,15,15,15,25,25,15,15,15,43,4,0,3,25,15,15,15,25,10,15,15,15,11,1,0,0,0,17,20,32,14,37,21,23,16,32,32,37,4,3,25,15,15,15,15,15,43,23,8,0,0,
3,25,15,15,15,15,15,15,37,12,0,0,22,15,15,15,37,21,37,15,15,15,10,18,15,15,15,13,19,0,3,25,15,15,15,25,25,15,15,15,13,1,3,25,15,15,15,25,22,15,15,15,15,21,0,0,0,0,22,15,15,15,15,15,15,15,15,26,0,3,25,15,15,15,37,2,17,15,15,15,15,23,0,3,25,15,15,15,25,25,15,15,15,25,7,3,25,15,15,15,15,15,15,15,15,25,7,8,18,15,15,15,15,15,15,15,15,15,21,0,0,11,15,15,15,43,4,25,15,15,15,18,6,0,0,5,37,15,15,15,22,15,15,15,15,21,0,3,25,15,15,15,25,24,15,15,15,13,22,15,
15,15,43,4,7,25,15,15,15,15,15,15,22,0,0,0,0,0,0,3,25,15,15,15,25,7,0,0,0,0,0,0,0,0,0,25,35,31,31,31,31,31,44,18,9,0,0,0,0,0,8,16,31,31,31,15,16,31,31,31,15,1,0,0,0,0,0,0,0,12,37,40,41,31,36,44,39,13,9,2,32,31,31,31,31,31,31,31,31,33,22,0,0,0,0,3,13,33,31,46,46,30,36,15,1,0,0,0,11,29,44,36,31,31,31,31,38,46,30,41,39,25,19,0,0,0,26,16,40,27,27,28,15,15,15,37,12,0,0,0,0,0,0,17,15,27,29,13,5,0,0,0,0,0,0,0,0,0,0,8,
23,13,15,44,44,37,19,0,0,0,0,0,0,0,0,0,0,22,20,29,20,12,8,20,31,31,31,27,30,46,44,14,35,30,46,41,32,17,0,8,16,31,31,31,27,38,46,31,39,11,0,0,8,16,31,31,31,27,30,46,29,21,0,6,15,36,31,31,20,5,13,35,31,31,29,17,0,0,25,35,31,31,28,13,35,31,31,16,15,36,31,31,16,8,3,43,35,31,31,40,24,34,31,31,36,14,5,0,0,10,39,31,31,31,31,31,31,36,37,19,8,16,31,31,31,15,1,8,16,31,31,31,15,20,41,31,31,27,13,0,8,16,31,31,31,15,39,31,31,36,32,12,0,6,15,36,31,31,20,24,
33,31,31,29,23,0,0,0,23,33,46,30,35,37,32,36,46,46,32,12,8,16,31,31,31,31,31,44,39,13,3,0,8,16,31,31,31,31,31,31,39,10,0,0,13,27,31,31,35,15,29,31,31,27,13,40,31,31,44,16,4,0,8,16,31,31,31,15,32,31,31,31,14,5,8,16,31,31,31,15,14,36,31,31,39,22,0,0,0,0,25,35,31,31,31,31,31,31,27,11,0,8,16,31,31,31,40,23,13,35,31,31,44,18,9,8,16,31,31,31,15,16,31,31,31,15,1,8,16,31,31,31,31,31,31,31,31,15,1,2,32,31,31,31,31,31,31,31,31,33,22,0,3,43,35,31,31,40,24,34,
31,31,36,14,5,0,0,21,29,31,31,35,25,33,31,31,33,22,0,8,16,31,31,31,15,37,36,31,31,20,13,44,31,31,34,12,1,15,31,31,31,31,31,35,13,0,0,0,0,0,0,8,16,31,31,31,15,1,0,0,0,0,0,0,0,0,0,25,35,53,48,46,47,52,44,18,2,12,12,12,19,0,8,16,38,51,31,15,16,38,51,31,15,4,12,12,12,1,0,0,19,18,27,47,49,46,46,48,44,18,9,2,32,46,46,46,49,52,46,46,46,33,22,12,9,0,0,12,14,30,51,45,38,30,31,15,1,0,0,4,20,36,53,48,46,46,52,51,48,30,46,52,30,29,22,0,0,3,18,35,
45,38,45,44,35,31,31,29,17,0,0,0,0,19,23,14,44,45,30,33,37,21,0,0,0,0,0,0,0,0,2,43,39,44,27,48,27,18,3,0,0,0,0,0,0,0,0,0,21,20,41,38,34,21,8,20,38,51,31,45,48,52,49,35,48,48,52,50,35,18,7,8,16,38,51,31,45,45,53,52,36,20,4,0,8,16,38,51,31,47,50,47,29,21,0,0,24,27,49,45,39,22,20,31,51,36,16,4,0,0,22,40,48,48,40,37,36,51,46,34,32,30,53,44,18,7,0,26,14,31,52,31,14,44,50,46,39,11,0,0,0,10,39,48,49,49,49,52,51,36,37,19,8,16,38,51,31,15,1,8,
16,38,51,31,16,44,49,45,27,37,12,0,8,16,38,51,31,39,30,53,36,34,23,9,0,0,24,27,49,45,39,22,37,41,52,36,15,8,0,0,23,33,46,50,45,33,44,50,48,46,32,12,8,16,38,51,47,46,47,52,30,32,12,0,8,16,38,51,47,46,46,46,39,10,0,0,2,37,27,45,48,35,29,45,53,27,39,30,50,41,14,10,0,0,8,16,38,51,31,15,35,50,51,30,14,5,8,16,38,51,31,14,41,50,46,40,13,6,0,0,0,0,25,35,53,53,49,49,52,50,27,11,0,8,16,38,51,52,41,37,14,38,51,52,44,18,9,8,16,38,51,31,15,16,38,51,31,15,1,8,
16,38,51,49,49,49,49,51,31,15,1,2,32,46,46,46,49,52,46,46,46,33,22,0,0,26,14,31,52,31,14,44,50,46,39,11,0,0,0,21,29,45,53,35,25,33,49,49,33,22,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,1,15,31,46,46,48,53,35,13,0,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,0,0,0,0,0,25,35,53,47,28,38,52,44,18,16,28,28,28,11,0,8,16,38,51,31,15,16,38,51,31,15,37,28,28,28,24,0,0,21,28,30,48,35,28,28,40,44,18,9,4,15,32,32,29,45,53,35,32,32,28,28,32,17,0,0,21,
28,47,49,33,20,14,39,37,1,0,0,12,32,46,53,44,32,35,53,50,35,14,39,46,53,44,43,3,0,4,20,38,49,33,47,44,31,51,36,16,4,0,0,0,4,25,29,36,50,54,51,47,35,20,22,7,0,0,0,0,0,4,43,27,35,40,46,44,16,10,22,22,4,0,0,0,0,0,0,6,43,35,47,40,25,7,8,20,38,51,48,27,39,46,51,49,35,39,30,51,31,16,8,8,16,38,51,49,35,39,36,52,47,28,21,0,8,16,38,51,50,36,33,27,29,21,0,0,21,28,46,53,35,13,29,47,50,27,25,0,0,0,12,32,46,53,35,14,30,47,48,40,39,45,49,33,23,0,0,9,
24,40,47,49,35,47,49,35,43,8,0,0,0,10,29,40,40,40,35,49,48,35,37,19,8,16,38,51,31,15,1,8,16,38,51,31,35,48,45,27,37,12,0,0,8,16,38,51,31,30,53,36,28,23,7,0,0,0,21,28,46,53,35,13,24,27,50,46,28,21,0,0,22,34,32,36,52,30,45,49,35,34,16,5,8,16,38,51,31,32,33,48,45,39,17,0,8,16,38,51,31,32,32,32,14,21,0,0,0,12,37,35,48,45,27,45,53,27,30,53,36,32,22,0,0,0,8,16,38,51,31,28,30,51,54,30,14,5,8,16,38,51,31,44,49,46,33,13,4,0,0,0,0,0,25,35,53,47,40,40,
48,50,27,11,0,8,16,38,51,54,46,29,33,48,54,52,44,18,9,8,16,38,51,31,15,16,38,51,31,15,1,8,16,38,51,31,40,40,38,51,31,15,1,4,15,32,32,29,45,53,35,32,32,14,10,0,0,9,24,40,47,49,35,47,49,35,43,8,0,0,0,21,29,45,53,35,25,33,49,49,33,22,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,3,18,32,32,28,47,53,35,13,25,23,12,0,0,0,8,16,38,51,31,15,25,11,21,7,0,0,0,0,0,9,13,44,52,47,28,38,52,44,18,32,46,47,35,25,0,8,16,38,51,31,35,35,38,51,31,15,16,38,47,
44,13,0,0,23,33,49,31,20,11,34,34,34,37,42,0,4,12,21,29,45,53,35,25,29,47,47,40,22,0,0,4,20,36,49,31,35,35,37,21,0,0,0,2,14,30,53,44,32,35,53,47,27,27,27,36,51,38,20,4,0,4,20,38,53,30,46,39,47,50,27,25,0,0,0,21,37,33,30,53,51,51,51,51,49,41,28,24,6,0,0,0,0,11,40,27,29,30,36,27,35,29,40,40,25,9,0,0,0,0,0,23,39,46,35,37,4,0,8,20,38,51,30,14,16,31,51,30,32,37,36,51,30,14,4,8,16,38,51,30,14,25,35,53,45,29,26,0,8,16,38,51,46,28,11,25,13,8,0,0,
42,37,41,52,31,20,27,50,46,29,26,0,0,0,8,15,36,51,36,39,45,44,48,35,27,50,47,29,21,0,0,0,4,37,35,50,48,52,36,14,21,0,0,0,0,19,22,22,43,27,45,50,44,20,21,0,8,16,38,51,31,15,1,8,16,38,51,31,45,48,35,15,12,0,0,0,8,16,38,51,49,52,47,39,23,7,0,0,0,0,42,37,41,52,31,20,24,27,50,45,39,17,0,0,7,12,11,39,46,51,53,36,14,21,2,0,8,16,38,51,31,35,41,50,30,32,12,0,8,16,38,51,31,15,12,12,2,0,0,0,0,0,21,16,35,49,47,49,53,30,52,31,28,23,9,0,0,0,8,16,
38,51,31,44,50,50,54,30,14,5,8,16,38,51,30,49,47,33,18,4,0,0,0,0,0,0,13,35,53,47,28,39,48,50,27,11,0,8,16,38,51,45,50,35,36,49,49,52,44,18,9,8,16,38,51,31,35,35,38,51,31,15,1,8,16,38,51,31,15,16,38,51,31,15,1,0,4,12,21,29,45,53,35,25,12,2,9,0,0,0,4,37,35,50,48,52,36,14,21,0,0,0,0,21,28,47,52,41,29,33,49,49,33,22,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,0,8,12,12,28,47,53,35,35,27,40,20,22,0,0,8,16,38,51,31,35,35,33,32,24,3,0,0,
0,0,3,43,41,52,30,14,38,52,44,18,32,46,53,35,25,0,8,16,38,51,50,49,49,50,51,31,15,16,38,52,44,13,0,0,24,27,50,36,37,25,35,46,30,20,4,0,0,0,21,29,45,53,35,25,29,45,48,40,22,0,0,42,18,33,36,53,50,36,37,1,0,0,0,3,13,27,45,48,46,46,52,53,50,50,50,50,50,30,14,5,0,24,40,36,51,46,33,27,50,46,29,26,0,0,19,37,35,31,31,31,31,31,31,31,31,31,31,39,22,0,0,0,1,37,41,14,36,30,33,13,29,27,33,41,20,4,0,0,4,26,12,20,36,36,14,21,0,0,8,20,38,51,31,15,15,31,51,38,
20,37,36,51,30,14,4,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,6,9,9,0,0,0,0,11,33,49,45,39,36,52,36,15,6,0,0,0,9,13,35,53,30,27,49,33,46,41,41,52,38,20,4,0,0,0,0,26,14,38,51,45,39,11,0,0,0,0,0,0,1,13,40,46,53,36,14,17,0,0,8,16,38,51,31,15,1,8,16,38,51,50,51,38,29,23,9,0,0,0,8,16,38,51,53,50,52,36,20,12,0,0,0,0,0,11,33,49,45,39,15,41,52,46,28,21,0,0,0,0,12,20,41,52,48,27,18,1,0,0,8,16,38,51,50,49,50,47,35,20,12,0,
8,16,38,51,31,15,1,0,0,0,0,0,0,0,21,20,36,53,53,54,54,52,52,46,39,23,0,0,0,0,8,16,38,51,31,46,48,36,51,30,14,5,8,16,38,51,51,50,49,35,43,8,0,0,0,0,0,9,13,44,52,46,34,39,48,50,27,11,0,8,16,38,51,31,47,38,47,38,46,52,44,18,9,8,16,38,51,50,49,49,50,51,31,15,1,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,21,29,45,53,35,25,0,0,0,0,0,0,0,26,14,38,51,45,39,11,0,0,0,0,0,5,20,31,53,50,45,45,53,49,33,22,0,8,16,38,51,31,15,37,36,51,30,20,13,44,
52,46,34,12,0,0,0,12,28,47,51,49,49,50,48,41,20,2,0,8,16,38,51,50,49,50,49,31,28,17,0,0,0,7,2,20,38,51,31,20,38,52,41,43,32,46,53,35,25,0,8,16,38,51,31,33,33,38,51,31,16,20,38,52,44,13,0,0,11,27,50,36,37,13,35,53,30,20,4,0,0,0,21,29,45,53,35,25,39,45,48,40,22,0,0,26,29,46,49,41,33,33,37,26,7,0,0,0,12,15,41,52,38,38,53,47,27,27,27,27,27,27,14,5,0,23,40,30,49,45,39,36,52,36,15,6,0,0,7,13,32,32,32,32,32,32,32,32,32,32,32,14,10,0,0,0,12,32,44,27,
47,27,40,20,22,14,44,27,28,12,0,12,37,29,15,27,46,39,11,0,0,0,8,20,38,51,31,15,15,31,51,38,16,37,36,51,30,14,4,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,1,0,0,0,0,0,0,12,32,30,50,35,47,49,33,24,0,0,0,0,0,23,33,49,45,44,45,29,31,30,38,52,44,43,3,0,0,0,8,43,35,49,49,52,36,14,21,0,0,0,0,3,24,39,30,52,31,28,22,12,8,0,8,16,38,51,31,15,1,8,16,38,51,31,46,53,36,32,22,0,0,0,8,16,38,51,31,36,52,48,27,13,3,0,0,0,0,12,32,30,50,35,
33,45,52,41,15,8,0,0,0,6,43,27,48,49,53,46,39,23,0,0,8,16,38,51,31,33,27,47,48,33,11,0,8,16,38,51,31,15,1,0,0,0,0,0,0,6,43,27,49,45,41,50,52,30,38,52,36,14,21,0,0,0,8,16,38,51,48,53,36,36,51,30,14,5,8,16,38,51,45,35,49,47,39,24,9,0,0,0,9,12,43,41,52,30,14,39,48,50,27,11,0,8,16,38,51,31,36,53,50,35,46,52,44,18,9,8,16,38,51,31,33,33,38,51,31,15,1,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,21,29,45,53,35,25,0,0,0,0,0,0,8,43,35,49,49,52,
36,14,21,0,0,0,0,0,11,29,44,41,41,41,49,49,33,22,0,8,16,38,51,31,15,37,36,51,30,20,13,44,52,46,34,12,0,0,0,12,28,47,53,35,33,35,49,45,39,10,0,8,16,38,51,31,33,27,47,50,27,24,0,0,9,25,20,33,45,53,35,15,36,51,30,28,33,48,50,27,11,0,8,16,38,51,31,15,37,36,51,47,39,39,47,50,35,25,0,0,10,39,45,46,29,37,41,52,31,16,8,0,0,0,12,28,46,51,31,32,35,50,45,39,17,0,0,22,40,48,45,29,43,37,32,34,21,0,0,0,11,40,45,48,33,33,49,49,27,15,37,20,39,44,13,9,0,5,20,34,
30,50,35,47,49,33,24,0,0,0,19,37,36,46,46,46,46,46,46,46,46,46,46,33,23,0,0,0,22,40,44,36,44,35,33,15,12,20,44,27,28,21,0,11,33,47,39,30,35,43,8,0,0,0,8,20,38,51,31,15,15,31,51,38,16,37,36,51,30,14,4,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,1,0,0,0,0,0,0,19,43,44,52,38,50,46,34,21,0,0,0,0,0,21,28,46,50,38,30,14,44,45,47,49,27,11,0,0,0,9,11,39,47,50,35,45,49,35,43,6,0,0,0,22,28,31,52,30,29,28,28,28,18,3,8,16,38,51,31,15,1,8,
16,38,51,31,27,45,50,41,14,17,0,0,8,16,38,51,31,39,46,51,30,28,10,0,0,0,0,19,43,44,52,31,30,51,46,39,23,0,0,0,12,18,39,46,53,41,30,52,36,14,25,1,8,16,38,51,31,15,14,30,53,35,25,0,8,16,38,51,31,15,1,0,0,0,0,0,0,11,39,46,50,44,29,45,53,27,33,45,49,35,43,8,0,0,8,16,38,51,54,45,40,36,51,30,14,5,8,16,38,51,31,14,31,52,31,32,17,0,0,0,26,14,33,46,51,36,15,39,48,50,27,11,0,8,16,38,51,31,33,48,46,29,46,52,44,18,9,8,16,38,51,31,15,16,38,51,31,15,1,8,
16,38,51,31,15,16,38,51,31,15,1,0,0,0,21,29,45,53,35,25,0,0,0,0,0,9,11,39,47,50,35,45,49,35,43,6,0,0,0,0,19,22,18,43,43,33,49,49,33,22,0,8,16,38,51,31,28,28,36,51,30,28,28,44,52,46,34,12,0,0,0,12,28,47,53,35,15,39,45,49,40,22,0,8,16,38,51,31,15,14,30,53,35,25,0,0,19,37,36,45,51,47,39,25,27,48,53,46,45,51,30,28,26,0,8,16,38,51,31,15,25,27,48,52,47,47,52,46,29,10,0,0,5,16,36,53,38,35,46,48,27,25,9,0,0,0,6,37,41,53,50,46,48,53,36,20,5,0,0,17,
29,47,53,30,41,41,30,29,26,0,0,12,20,36,52,36,20,14,31,53,45,36,36,38,45,44,13,0,0,0,12,37,44,52,38,50,46,34,21,0,0,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,0,11,33,38,35,41,32,29,29,34,39,27,27,32,12,0,22,39,45,36,31,32,26,0,0,0,0,8,20,38,51,31,15,15,31,51,38,16,37,36,51,30,14,4,8,16,38,51,31,15,24,27,53,45,29,26,0,8,16,38,51,31,15,1,0,0,0,0,0,0,0,23,40,48,51,52,41,37,1,0,0,0,0,0,4,20,31,51,51,36,37,27,50,54,45,29,17,0,0,0,21,
14,31,52,31,32,44,50,46,39,11,0,0,0,11,27,50,51,47,47,47,47,36,37,19,8,16,38,51,31,15,1,8,16,38,51,31,15,35,49,49,44,16,21,0,8,16,38,51,31,15,35,50,50,44,37,4,0,0,0,0,23,40,48,50,52,45,27,18,1,0,0,0,23,33,38,52,46,41,41,50,49,41,32,12,8,16,38,51,30,31,38,50,48,33,11,0,8,16,38,51,31,15,1,0,0,0,0,0,21,14,36,52,31,32,29,45,53,27,37,44,50,46,39,11,0,0,8,16,38,51,53,41,15,36,51,30,14,5,8,16,38,51,31,15,39,46,50,44,37,5,0,0,22,40,48,51,45,33,25,39,
48,50,27,11,0,8,16,38,51,31,14,33,33,32,46,52,44,18,9,8,16,38,51,31,15,16,38,51,31,15,1,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,21,29,45,53,35,25,0,0,0,0,0,21,14,31,52,31,32,44,50,46,39,11,0,0,0,0,0,0,9,7,22,33,49,49,33,22,0,8,16,38,51,47,47,47,47,51,45,47,47,47,51,46,34,12,0,0,0,12,28,47,52,31,31,30,53,46,29,26,0,8,16,38,51,30,31,38,50,48,33,11,0,0,19,37,36,45,38,27,37,12,16,44,47,48,48,30,33,13,3,0,8,16,38,46,31,15,12,16,35,46,48,48,30,33,
18,42,0,0,0,22,28,41,47,48,45,44,20,12,0,0,0,0,0,22,28,36,45,49,45,31,29,23,0,0,0,8,37,27,30,48,49,48,46,29,26,0,0,25,33,46,46,40,11,11,29,36,47,48,48,47,38,35,13,0,0,0,0,23,33,48,51,52,41,37,1,0,0,0,19,37,36,51,54,54,54,54,54,54,54,54,49,33,23,0,0,4,15,36,31,33,35,27,33,33,27,33,35,41,16,4,0,12,14,30,45,33,24,9,0,0,0,0,8,20,38,46,31,15,15,31,46,38,16,37,36,46,30,14,4,8,16,38,46,31,15,24,27,46,46,29,26,0,8,16,38,46,31,15,1,0,0,0,0,0,
0,0,12,14,30,46,46,33,11,0,0,0,0,0,0,7,18,44,46,46,35,25,39,46,46,30,14,5,0,0,1,43,35,46,46,33,24,28,30,46,36,14,12,0,0,11,27,46,46,46,46,46,46,36,37,19,8,16,38,46,31,15,1,8,16,38,46,31,15,20,41,46,46,35,37,8,8,16,38,46,31,15,32,38,46,46,39,11,0,0,0,0,12,14,30,46,46,44,15,12,0,0,0,0,23,33,46,46,46,46,46,46,46,46,32,12,8,16,38,46,46,46,46,30,35,20,12,0,8,16,38,46,31,15,1,0,0,0,0,1,43,35,46,46,40,24,29,46,46,27,24,32,31,46,31,14,21,0,8,16,
38,46,46,39,37,36,46,30,14,5,8,16,38,46,31,15,18,27,46,46,33,25,0,0,22,40,46,31,33,37,10,39,46,46,27,11,0,8,16,38,46,31,15,23,23,32,46,46,44,18,9,8,16,38,46,31,15,16,38,46,31,15,1,8,16,38,46,31,15,16,38,46,31,15,1,0,0,0,21,29,46,46,35,25,0,0,0,0,1,43,35,46,46,33,24,28,30,46,36,14,12,0,0,0,0,0,0,0,22,33,46,46,33,22,0,8,16,38,46,46,46,46,46,46,46,46,46,46,46,46,34,12,0,0,0,12,28,46,46,46,46,46,31,33,43,42,0,8,16,38,46,46,46,46,30,35,16,12,0,0,
19,37,40,29,14,13,2,0,26,37,29,40,39,32,13,4,0,0,1,43,34,34,34,18,3,21,37,29,40,39,34,13,5,0,0,0,0,7,22,16,29,40,39,15,17,0,0,0,0,0,0,7,23,20,39,40,39,14,11,19,0,0,0,0,12,18,32,39,40,39,34,37,2,0,0,24,34,34,34,15,4,3,23,15,29,40,40,29,14,18,21,0,0,0,0,12,14,30,46,46,33,11,0,0,0,0,19,37,36,46,46,46,46,46,46,46,46,46,46,33,23,0,0,10,39,27,14,24,25,25,23,11,24,14,32,32,25,9,0,3,18,27,27,16,5,0,0,0,0,0,1,43,34,34,34,18,18,34,34,34,
43,13,34,34,34,37,8,1,43,34,34,34,18,23,34,34,34,14,12,0,1,43,34,34,34,18,3,0,0,0,0,0,0,0,3,18,34,34,34,20,12,0,0,0,0,0,0,0,23,34,34,34,32,22,20,34,34,34,18,3,0,0,1,43,34,34,34,37,5,25,34,34,34,20,12,0,0,22,32,34,34,34,34,34,34,34,18,3,1,43,34,34,34,18,3,1,43,34,34,34,18,10,14,34,34,34,37,6,1,43,34,34,34,18,25,34,34,34,34,23,0,0,0,0,3,18,34,34,34,20,26,0,0,0,0,0,22,32,34,34,34,34,34,34,34,34,15,5,1,43,34,34,34,34,34,14,43,26,0,0,
1,43,34,34,34,18,3,0,0,0,0,1,43,34,34,34,37,12,20,34,34,34,23,24,34,34,34,14,12,0,1,43,34,34,34,37,13,34,34,34,37,8,1,43,34,34,34,18,12,20,34,34,34,25,0,0,26,14,32,16,25,5,21,14,34,34,32,22,0,1,43,34,34,34,18,3,5,15,34,34,34,25,0,1,43,34,34,34,18,43,34,34,34,18,19,1,43,34,34,34,18,43,34,34,34,18,19,0,0,0,12,20,34,34,34,23,0,0,0,0,1,43,34,34,34,37,5,25,34,34,34,20,12,0,0,0,0,0,0,0,10,32,34,34,14,10,0,1,43,34,34,34,34,34,34,34,34,34,34,34,
34,34,16,5,0,0,0,2,20,34,34,34,34,32,20,25,5,0,0,1,43,34,34,34,34,34,14,18,21,0,0,0,0,12,22,17,2,7,0,0,0,8,26,22,10,12,3,0,0,0,0,4,12,12,12,8,0,0,8,21,22,22,21,19,0,0,0,0,0,0,0,4,17,22,10,4,0,0,0,0,0,0,0,0,0,5,17,22,10,2,9,0,0,0,0,0,0,19,12,10,22,10,12,42,0,0,0,1,12,12,12,5,0,0,0,4,26,22,22,26,5,7,0,0,0,0,0,3,18,34,34,34,20,12,0,0,0,0,3,18,34,34,34,34,34,34,34,34,34,34,34,32,10,0,0,2,43,25,12,7,
0,0,0,0,0,2,12,12,1,0,0,0,12,24,25,21,0,0,0,0,0,0,0,4,12,12,12,8,4,12,12,12,4,6,12,12,12,4,0,0,4,12,12,12,8,3,12,12,12,2,0,0,0,4,12,12,12,8,0,0,0,0,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,3,12,12,12,12,7,2,12,12,12,8,0,0,0,0,4,12,12,12,5,0,1,12,12,12,2,0,0,0,7,12,12,12,12,12,12,12,12,8,0,0,4,12,12,12,8,0,0,4,12,12,12,8,0,12,12,12,12,4,0,0,4,12,12,12,8,1,12,12,12,12,3,0,0,0,0,0,8,12,12,
12,2,0,0,0,0,0,0,7,12,12,12,12,12,12,12,12,12,5,0,0,4,12,12,12,12,12,5,19,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,4,12,12,12,4,0,2,12,12,12,3,19,12,12,12,2,0,0,0,4,12,12,12,4,6,12,12,12,4,0,0,4,12,12,12,8,0,2,12,12,12,1,0,0,0,12,12,4,0,0,0,12,12,12,12,7,0,0,4,12,12,12,8,0,0,5,12,12,12,1,0,0,4,12,12,12,8,4,12,12,12,4,0,0,4,12,12,12,8,4,12,12,12,4,0,0,0,0,0,2,12,12,12,3,0,0,0,0,0,4,12,12,12,5,0,1,
12,12,12,2,0,0,0,0,0,0,0,0,9,12,12,12,12,9,0,0,4,12,12,12,12,12,12,12,12,12,12,12,12,12,5,0,0,0,0,0,2,12,12,12,12,12,4,9,0,0,0,0,4,12,12,12,12,12,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
8,12,12,12,2,0,0,0,0,0,0,8,12,12,12,12,12,12,12,12,12,12,12,12,9,0,0,0,3,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,42,42,42,7,0,0,0,7,42,42,42,7,0,0,0,0,0,7,1,42,42,42,42,1,0,0,0,0,0,9,42,42,42,42,42,42,42,1,0,0,0,0,0,0,0,7,42,42,42,7,7,42,42,42,7,0,0,0,0,0,0,0,
0,0,0,0,0,1,42,42,19,0,0,0,0,0,0,0,7,42,42,42,7,0,0,0,3,42,42,1,0,0,0,0,0,0,0,0,1,42,42,42,42,42,42,42,42,1,0,0,0,7,42,42,42,7,42,42,42,42,42,42,42,42,42,7,0,0,3,42,42,42,9,0,0,7,5,12,2,1,0,0,0,0,0,0,0,6,1,0,0,0,0,0,0,0,42,42,42,42,42,42,42,0,0,0,7,42,42,42,7,0,42,42,42,42,0,0,0,7,42,42,42,42,42,42,3,0,42,42,42,42,0,0,0,7,42,42,42,7,7,42,42,42,42,42,42,1,0,0,0,0,1,42,42,42,0,1,42,42,1,0,
0,0,9,42,42,42,19,0,7,42,42,42,19,0,0,0,0,0,0,0,0,19,42,42,42,0,0,0,0,0,0,42,42,42,42,42,42,42,1,0,9,42,42,42,19,0,0,0,0,42,42,42,19,0,42,42,42,7,9,42,42,42,3,0,0,7,42,42,42,7,7,42,42,42,42,0,0,0,7,42,42,42,7,0,0,0,4,12,4,9,0,0,0,0,0,42,42,42,19,0,42,42,42,7,0,0,4,12,4,9,0,0,0,0,7,42,42,42,7,0,0,0,0,0,0,0,0,0,1,1,9,8,4,3,0,0,0,0,0,0,0,0,42,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,4,1,0,0,0,0,0,7,8,4,4,1,0,0,0,0,0,0,7,42,42,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,12,5,9,0,0,0,0,19,19,3,0,0,0,3,25,15,15,15,25,7,0,3,25,15,15,15,25,7,0,0,3,22,18,15,15,15,15,15,43,5,0,0,0,0,11,
15,15,15,15,15,15,15,37,12,0,0,0,0,0,3,25,15,15,15,25,25,15,15,15,25,7,0,0,0,0,0,0,0,0,0,0,5,43,15,15,43,4,0,0,0,0,0,3,25,15,15,15,25,7,0,1,13,15,15,37,12,0,0,0,0,0,0,12,37,15,15,15,15,15,15,15,15,37,5,0,3,25,15,15,15,25,15,15,15,15,15,15,15,15,15,24,9,1,13,15,15,15,11,0,4,25,20,32,14,18,19,0,0,0,5,25,22,37,43,21,9,0,0,0,0,22,15,15,15,15,15,15,15,22,0,3,25,15,15,15,25,11,15,15,15,15,21,0,7,25,15,15,15,15,15,15,13,10,15,15,
15,15,10,0,3,25,15,15,15,25,25,15,15,15,15,15,15,37,12,0,0,5,37,15,15,15,22,15,15,15,15,21,0,0,11,15,15,15,43,4,25,15,15,15,18,6,0,0,0,0,0,0,8,18,15,15,15,23,0,0,0,0,22,15,15,15,15,15,15,15,37,2,11,15,15,15,43,4,0,0,22,15,15,15,18,10,15,15,15,24,11,15,15,15,25,3,3,25,15,15,15,43,43,15,15,15,15,21,0,3,25,15,15,15,25,7,1,11,20,32,20,24,6,0,0,0,22,15,15,15,18,10,15,15,15,24,1,24,20,32,20,24,6,0,0,3,25,15,15,15,25,7,0,0,0,0,0,0,4,11,
37,37,25,16,20,18,21,0,0,0,0,0,7,1,43,14,14,22,7,0,0,0,0,0,0,0,5,23,12,19,22,10,9,0,21,23,4,8,23,26,0,0,0,0,0,0,0,0,0,5,23,12,0,0,21,23,4,0,0,0,0,0,0,1,22,25,22,19,0,0,0,0,0,0,0,0,0,0,0,42,21,21,21,12,0,0,4,24,37,14,14,37,22,9,0,0,5,18,16,14,20,37,22,9,0,0,1,23,18,15,15,43,5,0,0,0,22,15,15,15,15,15,15,15,15,15,15,22,0,0,8,26,26,26,26,26,26,26,26,26,26,4,0,0,7,23,16,34,20,24,1,0,0,23,37,37,18,4,0,0,
8,16,31,31,31,15,1,0,8,16,31,31,31,15,1,0,0,11,29,44,36,31,31,31,31,29,21,0,0,0,3,43,41,31,31,31,31,31,31,39,10,0,0,0,0,0,8,16,31,31,31,15,16,31,31,31,15,1,0,0,0,0,0,0,0,0,0,0,11,40,31,31,40,23,0,0,0,0,0,8,16,31,31,31,15,1,0,10,29,31,31,27,13,7,0,0,0,0,0,10,29,31,31,31,31,31,31,31,31,28,21,0,8,16,31,31,31,15,35,31,31,31,31,31,31,31,44,37,19,4,20,31,31,31,16,4,24,40,38,46,38,20,8,0,0,0,24,33,29,33,35,14,11,0,0,0,0,13,35,31,
31,31,31,31,35,25,0,8,16,31,31,31,27,39,36,31,31,39,22,0,1,15,31,31,31,31,31,31,20,20,44,31,31,33,24,0,8,16,31,31,31,15,16,31,31,31,31,31,31,39,10,0,0,21,29,31,31,35,25,33,31,31,33,22,0,3,43,35,31,31,40,24,34,31,31,36,14,5,0,0,0,0,0,0,12,32,31,31,44,13,9,0,0,0,25,35,31,31,31,31,31,31,33,25,14,31,31,31,32,12,0,0,25,35,31,31,28,13,35,31,31,16,15,36,31,31,16,8,8,16,31,31,31,41,41,36,31,31,39,22,0,6,15,36,31,31,20,5,11,29,31,46,31,39,25,7,0,0,25,35,
31,31,28,13,35,31,31,16,11,39,31,46,31,39,25,7,0,8,16,31,31,31,15,1,0,0,0,0,0,4,18,40,41,40,27,31,38,35,14,17,0,0,0,5,18,24,20,30,27,11,18,23,0,0,0,0,0,21,37,33,18,25,29,14,2,3,43,33,43,22,40,14,22,7,0,0,0,0,0,0,21,37,33,18,9,3,43,33,43,12,0,0,0,0,7,25,39,35,29,11,0,0,0,1,21,23,22,8,0,0,0,25,28,28,28,20,2,0,10,39,36,30,30,44,32,26,0,0,10,39,31,30,30,44,32,26,0,0,17,29,44,31,31,29,21,0,0,0,25,35,31,31,31,31,31,31,31,31,44,13,
9,0,24,29,29,29,29,29,29,29,29,29,29,25,0,0,22,28,36,46,31,39,11,0,3,43,41,36,28,21,0,0,8,16,38,51,31,15,1,0,8,16,38,51,31,15,1,0,4,20,36,53,48,46,46,52,47,29,21,0,0,0,3,43,41,51,50,49,49,51,48,39,10,0,0,0,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,0,0,0,0,0,0,2,20,36,52,52,36,16,5,0,0,0,0,8,16,38,51,31,15,1,6,37,44,53,52,38,32,21,0,0,0,0,0,6,37,44,53,47,46,46,47,50,35,18,19,0,8,16,38,51,31,15,34,30,53,46,46,46,50,46,29,22,
0,9,13,35,50,47,29,17,20,31,51,45,38,20,8,0,0,12,14,31,46,31,49,44,43,3,0,0,0,13,35,53,48,46,46,46,35,25,0,8,16,38,51,31,44,41,50,46,40,13,6,0,1,15,31,46,46,46,51,30,20,44,49,47,33,18,4,0,8,16,38,51,31,15,16,38,51,47,46,46,46,39,10,0,0,21,29,45,53,35,39,33,49,49,33,22,0,0,26,14,31,52,31,14,44,50,46,39,11,0,0,0,0,0,0,0,12,32,46,52,44,13,9,0,0,0,25,35,53,53,49,49,49,52,38,34,35,49,47,40,24,7,0,0,22,40,48,48,40,37,36,51,46,34,32,30,53,44,18,7,
8,16,38,51,31,28,31,50,46,40,13,6,0,0,24,27,49,45,39,22,16,36,52,52,52,38,14,12,0,0,22,40,48,48,40,37,36,51,46,34,20,31,52,52,52,38,14,12,0,8,16,38,51,31,15,1,0,0,0,0,0,11,40,46,48,40,47,52,49,49,44,15,4,0,0,11,33,33,20,30,27,32,35,20,12,0,0,0,10,20,35,44,18,39,30,32,2,3,37,36,27,15,33,41,34,11,1,0,0,0,0,10,20,35,44,18,9,3,37,36,27,15,26,0,0,0,4,20,31,40,36,15,5,4,22,37,28,33,39,13,3,0,9,13,44,47,46,34,12,0,10,39,41,33,44,50,27,25,0,0,
10,39,35,33,44,50,27,11,0,0,17,39,36,44,47,29,21,0,0,0,25,35,45,45,45,45,45,45,45,45,44,13,9,0,25,35,45,45,45,45,45,45,45,45,44,13,9,3,43,44,31,33,36,36,16,8,3,43,41,47,28,21,0,0,8,16,38,51,31,15,25,11,21,16,38,51,31,15,1,0,12,32,46,53,44,32,35,53,47,29,21,0,0,0,3,43,41,51,30,40,27,50,48,39,25,11,21,3,0,0,8,16,38,51,31,15,16,38,51,31,15,24,22,5,0,0,0,0,0,0,0,24,33,48,46,46,45,33,11,0,0,0,0,8,16,38,51,31,15,22,22,29,46,45,31,49,27,13,3,0,
0,0,0,0,10,29,46,30,28,29,46,30,34,26,0,0,8,16,38,51,31,15,13,35,50,44,32,35,50,44,37,6,0,0,10,29,47,50,27,25,39,45,49,27,28,37,1,0,9,11,32,44,47,52,46,39,43,21,0,0,12,13,35,53,47,28,32,32,32,11,0,8,16,38,51,31,44,53,46,33,13,4,0,0,3,18,32,32,32,36,51,30,35,48,47,27,43,5,0,0,8,16,38,51,31,15,16,38,51,31,32,32,32,14,21,0,0,21,29,45,53,35,36,33,49,49,33,22,0,0,1,24,40,47,49,35,47,49,35,43,5,0,0,0,0,9,12,23,25,32,46,52,44,13,9,0,0,0,25,35,
53,47,40,40,40,46,50,44,46,50,44,37,4,0,0,0,12,32,46,53,35,14,30,47,48,40,39,45,49,33,23,0,8,16,38,51,31,41,50,45,44,20,21,0,0,0,21,28,46,53,35,13,29,47,50,41,30,48,39,10,0,0,12,32,46,53,35,14,30,47,48,40,39,45,50,41,30,48,39,10,0,8,16,38,51,31,15,25,25,23,7,0,8,15,36,52,38,35,53,47,27,49,47,39,10,0,19,37,41,45,44,30,27,38,30,40,22,0,9,23,14,44,48,44,33,46,30,32,2,3,37,36,48,44,33,47,31,29,25,8,0,9,23,14,44,48,44,18,9,3,37,36,48,44,14,22,0,0,4,20,
36,27,36,40,20,16,39,44,46,31,27,37,42,0,9,13,44,52,46,34,12,0,26,14,15,25,33,49,27,25,0,0,2,43,39,33,44,47,40,23,0,0,12,16,15,44,47,29,21,0,0,0,24,29,29,29,29,29,29,29,29,29,29,25,0,0,25,35,31,31,31,31,31,31,31,48,44,13,9,42,15,31,27,24,40,46,14,2,3,43,41,47,28,21,0,0,8,16,38,51,31,35,35,33,32,16,38,51,31,15,1,0,2,14,30,53,44,32,35,53,47,29,21,0,0,0,3,37,36,51,30,14,27,50,48,35,35,33,34,25,1,0,8,16,38,51,31,35,35,38,51,31,35,27,40,16,17,0,0,0,
0,0,12,20,31,50,35,35,50,36,20,2,0,0,0,8,16,38,51,31,40,40,40,44,53,41,33,48,30,34,26,0,0,0,0,0,1,43,44,50,35,44,50,35,13,3,0,0,8,16,38,51,31,40,40,40,30,47,39,30,46,29,22,0,0,0,4,16,36,51,36,15,35,53,30,32,21,5,0,0,12,14,35,14,31,53,49,46,35,13,0,19,37,40,35,53,47,40,40,40,24,3,0,8,16,38,51,30,49,47,33,18,4,0,0,0,0,8,12,12,37,36,51,46,48,45,27,37,2,0,0,0,8,16,38,51,31,35,35,38,51,31,15,12,12,2,0,0,0,12,34,46,51,38,36,35,49,49,33,22,
0,0,17,29,27,35,50,48,52,36,27,33,24,0,0,0,9,23,14,33,35,35,46,52,44,13,9,0,0,0,13,35,53,47,28,22,18,27,49,48,52,38,32,17,0,0,0,0,8,15,36,51,36,39,45,44,48,35,27,50,47,29,21,0,8,16,38,51,30,49,47,33,35,35,25,0,0,0,42,37,41,52,31,20,27,50,46,29,44,46,40,22,0,0,8,15,36,51,36,39,45,44,48,35,27,50,47,29,44,46,40,22,0,8,16,38,51,31,35,35,35,14,5,0,12,34,46,53,44,44,52,46,29,45,50,27,11,0,0,22,15,40,31,49,47,44,28,13,5,0,5,14,36,45,35,35,45,31,29,25,
19,0,22,14,44,48,41,33,46,30,39,10,0,5,14,36,45,35,20,10,0,0,22,14,44,48,41,20,4,0,0,23,32,40,33,41,36,36,47,36,40,20,25,12,0,0,3,43,44,52,30,32,12,0,0,12,13,39,30,38,28,17,0,0,0,22,33,38,47,41,28,22,0,0,0,4,18,44,47,29,21,0,0,0,25,39,39,39,39,39,39,39,39,39,39,25,0,0,22,15,15,15,15,15,15,15,29,45,44,13,9,19,37,41,36,29,44,31,20,4,3,43,41,47,28,21,0,0,8,16,38,51,50,49,50,49,31,28,38,51,31,15,1,0,3,13,27,45,48,46,46,52,47,29,21,0,0,0,1,37,
36,51,30,20,27,50,52,49,50,49,30,29,22,0,8,16,38,51,50,49,49,50,54,50,49,50,45,44,15,4,0,0,0,9,25,33,48,46,35,35,46,48,33,24,0,0,0,8,16,38,51,49,49,49,49,49,45,35,35,31,50,35,18,19,0,0,0,0,8,13,29,46,46,47,30,28,25,1,0,0,8,16,38,51,49,49,49,49,49,53,41,50,44,20,10,0,0,0,0,25,27,50,46,29,38,52,44,37,1,0,0,9,25,27,49,31,48,30,35,46,29,10,0,1,37,36,49,51,53,49,49,27,24,0,0,8,16,38,51,51,50,49,35,43,8,0,0,0,0,0,0,3,37,36,51,51,50,50,44,15,
5,0,0,0,8,16,38,51,50,49,49,50,51,31,15,1,0,0,0,0,0,6,37,44,48,49,50,49,52,49,33,22,0,0,17,39,45,50,51,54,54,50,50,27,24,0,0,0,21,32,31,48,50,49,53,52,44,13,9,0,0,9,13,44,52,46,34,12,12,20,36,51,48,40,24,7,0,0,0,0,9,13,35,53,30,27,49,33,46,41,41,52,38,20,4,0,8,16,38,51,51,50,49,35,43,13,21,0,0,0,0,11,33,49,45,39,36,52,36,37,32,32,14,26,0,0,9,13,35,53,30,27,49,33,46,41,41,52,31,20,32,32,14,26,0,8,16,38,51,50,49,49,30,14,5,0,21,29,47,53,
35,44,52,46,29,45,53,35,25,0,7,25,28,44,46,46,30,31,33,16,21,0,5,14,30,36,28,44,46,33,43,12,0,0,8,25,29,36,38,20,35,48,39,10,0,5,14,30,36,28,24,1,0,0,8,25,29,36,38,20,4,0,0,26,37,14,28,27,46,46,31,27,32,18,17,1,0,0,4,20,38,48,27,13,3,0,4,18,40,30,31,29,13,26,0,0,10,20,15,20,40,48,35,13,0,0,19,11,18,44,47,29,25,21,0,0,25,35,48,48,48,48,48,48,48,48,44,13,9,0,0,1,1,1,1,1,1,26,29,45,44,13,9,0,23,39,30,47,46,33,25,9,3,43,41,36,28,21,0,0,
8,16,38,51,31,33,27,47,50,27,38,51,31,15,1,0,0,12,15,41,52,38,38,52,47,29,21,0,0,3,21,20,31,51,31,15,27,50,48,33,27,46,53,35,13,9,8,16,38,51,31,33,33,38,51,31,33,44,50,47,29,21,0,0,0,12,14,31,53,48,50,50,48,53,31,14,12,0,0,8,16,38,51,31,35,35,41,52,48,49,53,48,50,30,28,17,0,0,0,7,25,40,30,50,51,52,50,38,39,11,0,0,8,16,38,51,31,35,35,35,53,52,51,52,48,41,20,12,0,0,0,26,29,46,50,27,45,48,33,23,0,0,0,9,13,39,36,49,53,44,32,39,43,1,0,1,37,35,35,53,
47,35,35,27,24,0,0,8,16,38,51,45,44,52,47,39,24,9,0,0,0,0,0,3,37,36,51,48,35,48,45,33,25,3,0,0,8,16,38,51,31,33,33,38,51,31,15,1,0,0,0,0,0,0,10,20,40,33,36,33,49,49,33,22,0,0,17,29,27,35,50,49,51,36,27,27,24,0,0,0,23,33,49,45,27,33,46,52,44,13,9,0,9,12,43,41,52,30,14,2,18,27,48,49,52,38,32,17,0,0,0,0,0,23,33,49,45,44,45,29,31,30,38,52,44,43,3,0,8,16,38,51,45,35,49,47,39,24,9,0,0,0,0,12,32,30,50,35,47,49,33,24,2,2,2,0,0,0,0,23,
33,49,45,44,45,29,31,30,38,52,44,18,2,2,2,0,0,8,16,38,51,31,33,33,33,14,5,0,21,28,47,52,44,44,52,46,40,48,50,27,24,0,19,37,44,38,33,30,27,44,47,39,22,0,4,15,27,47,31,39,38,47,27,15,4,7,13,39,38,46,27,41,48,41,32,26,0,4,15,27,47,31,39,25,0,7,13,39,38,46,27,37,6,0,6,37,44,36,38,35,29,29,35,30,30,35,29,18,3,0,26,29,47,36,20,12,0,0,22,40,46,30,44,44,35,13,0,0,11,27,27,33,44,50,35,25,0,0,12,28,35,44,47,35,35,13,0,0,25,35,36,36,36,36,36,36,36,36,44,13,
9,0,0,0,0,0,0,0,0,26,29,31,44,13,9,0,42,25,28,40,29,18,5,0,0,23,37,37,13,4,0,0,8,16,38,51,31,15,14,30,53,35,38,51,31,15,1,0,0,11,40,45,48,33,35,53,47,29,21,0,0,22,34,27,45,52,44,43,27,50,48,39,20,38,52,44,18,9,8,16,38,51,31,15,16,38,51,31,15,33,49,45,39,17,0,0,7,25,27,49,30,40,46,30,40,30,48,27,25,9,0,8,16,38,51,31,15,39,45,46,40,31,45,40,36,50,35,43,1,0,0,12,14,38,52,30,30,31,30,52,36,20,5,0,8,16,38,51,31,15,33,45,49,41,49,41,48,49,27,25,
0,0,0,6,37,41,52,31,53,30,14,12,0,0,0,0,8,13,27,49,45,50,38,32,12,0,0,0,22,13,35,53,47,28,25,25,12,0,0,8,16,38,51,31,44,31,52,31,32,17,0,0,0,0,0,3,37,36,51,30,20,41,53,30,28,22,0,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,0,0,0,0,12,22,37,36,33,49,49,33,22,0,0,1,11,39,47,50,35,45,49,35,43,2,0,0,0,11,27,50,46,28,32,46,52,44,13,9,0,26,14,33,46,51,36,15,23,29,46,50,44,47,50,44,37,4,0,0,0,0,21,28,46,50,38,30,14,44,45,47,49,27,11,0,0,
8,16,38,51,31,14,31,52,31,32,17,0,0,0,0,19,43,44,52,38,50,30,34,21,0,0,0,0,0,0,0,21,28,46,50,38,30,14,44,45,47,49,27,11,0,0,0,0,0,8,16,38,51,31,15,23,23,10,0,0,4,20,31,51,30,44,52,46,41,53,45,39,22,0,0,22,29,32,20,30,27,37,40,37,4,0,0,12,43,33,30,44,28,36,46,32,2,3,37,36,30,40,33,45,35,20,10,9,0,0,12,43,33,30,44,18,9,3,37,36,30,40,43,12,0,0,5,14,36,14,31,16,10,17,13,32,33,41,35,37,1,0,22,39,33,39,11,0,0,0,22,40,31,31,31,31,35,13,0,0,
11,27,31,30,30,44,32,17,0,0,12,28,31,31,31,31,35,13,0,0,10,37,37,37,37,37,37,37,37,37,37,22,0,0,0,0,0,0,0,0,0,2,37,15,15,22,0,0,0,3,21,22,26,1,0,0,0,0,3,3,7,0,0,0,8,16,38,51,30,31,38,50,48,33,38,51,31,15,1,0,12,20,36,52,36,20,35,53,47,29,21,0,0,11,27,49,52,46,39,11,27,50,48,31,38,49,49,27,25,0,8,16,38,51,31,15,16,38,51,30,31,30,52,30,32,12,0,0,21,32,38,49,27,14,46,30,14,27,49,38,14,21,0,8,16,38,51,31,16,36,53,44,15,31,45,29,40,45,46,29,
10,0,0,22,40,48,45,39,30,31,33,48,45,39,17,0,8,16,38,51,31,15,41,52,31,40,49,27,41,52,36,15,6,0,0,0,24,33,49,51,53,44,18,3,0,0,0,0,4,20,41,48,35,41,35,18,19,0,0,0,0,13,35,53,47,28,12,0,0,0,0,8,16,38,51,31,44,40,46,50,44,37,5,0,0,0,0,3,37,36,51,30,20,29,30,53,41,20,12,0,8,16,38,51,31,15,16,38,51,31,15,1,0,0,0,0,0,0,0,0,9,24,15,33,49,49,33,22,0,0,21,14,31,52,31,32,44,50,46,39,11,0,0,0,22,39,45,53,30,31,47,52,44,13,9,0,22,40,48,51,
45,33,25,20,36,53,30,28,35,50,47,40,24,9,0,0,0,4,20,31,51,51,36,37,27,50,54,45,29,17,0,0,8,16,38,51,31,15,39,46,50,44,37,5,0,0,0,0,23,40,48,51,52,41,37,1,0,0,0,0,0,0,0,4,20,31,51,51,36,37,27,50,54,47,29,17,0,0,0,0,0,8,16,38,51,31,15,1,0,0,0,0,9,25,27,45,53,47,51,48,49,50,41,20,5,0,0,1,22,26,20,44,27,11,22,21,0,0,0,0,4,13,39,44,18,14,44,32,2,3,37,36,29,25,33,27,37,21,0,0,0,0,0,4,13,39,44,18,9,3,37,36,29,25,4,0,0,0,1,43,
27,31,33,13,7,0,3,12,24,43,13,21,0,0,1,22,22,22,6,0,0,0,21,15,16,16,16,16,16,22,0,0,12,18,16,14,20,37,22,9,0,0,4,43,16,16,16,16,16,22,0,0,0,19,19,19,19,19,19,19,19,19,19,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,16,38,46,46,46,46,30,35,16,38,46,31,15,1,0,25,33,46,46,40,25,35,46,46,29,21,0,0,11,27,46,36,40,18,11,27,46,46,46,46,30,35,20,21,0,8,16,38,46,31,15,16,38,46,46,46,46,31,33,13,3,0,7,
13,27,46,38,14,14,46,30,14,14,38,46,27,13,9,8,16,38,46,31,33,46,46,29,15,31,46,29,16,36,46,44,37,1,0,11,27,46,30,14,30,31,28,46,46,33,22,0,8,16,38,46,31,15,31,46,44,40,46,27,27,46,30,14,5,0,0,0,21,34,30,46,46,39,22,0,0,0,0,0,0,22,16,33,32,20,32,26,0,0,0,0,0,13,35,46,46,28,12,0,0,0,0,8,16,38,46,31,27,39,27,46,46,33,25,0,0,0,0,3,37,36,46,30,20,13,33,46,46,27,18,7,8,16,38,46,31,15,16,38,46,31,15,1,0,0,0,0,0,0,0,0,0,7,22,33,46,46,33,22,
0,1,43,35,46,46,33,24,28,30,46,36,14,12,0,0,4,37,27,38,46,46,46,46,44,13,9,0,22,40,46,31,33,37,18,27,46,46,33,25,32,31,46,31,32,21,0,0,0,7,18,44,46,46,35,25,39,46,46,30,14,5,0,0,8,16,38,46,31,15,18,27,46,46,33,25,0,0,0,0,12,14,30,46,46,33,11,0,0,0,0,0,0,0,0,7,18,44,46,46,35,25,39,46,46,30,14,5,0,0,0,0,0,8,16,38,46,31,15,1,0,0,0,0,0,12,37,27,31,46,46,46,30,44,32,22,0,0,0,0,0,7,11,13,13,12,0,0,0,0,0,0,0,1,24,14,24,17,16,37,
4,9,25,14,11,17,14,18,2,0,0,0,0,0,0,0,1,24,14,24,0,9,25,14,11,19,0,0,0,0,0,12,13,15,13,5,0,0,0,0,0,7,9,0,0,0,0,0,0,0,0,0,0,0,0,42,6,6,6,6,6,0,0,0,0,7,8,5,4,19,0,0,0,0,0,1,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,43,34,34,34,34,34,14,18,43,34,34,34,18,3,0,24,34,34,34,15,11,34,34,34,20,12,0,0,22,32,32,
15,11,4,22,32,34,34,34,34,14,43,26,0,0,1,43,34,34,34,18,43,34,34,34,34,32,16,25,4,0,0,9,13,34,34,34,25,15,34,34,15,25,34,34,34,25,9,1,43,34,34,34,34,34,34,43,43,34,34,20,23,34,34,34,43,1,0,22,32,34,34,37,34,34,16,34,34,32,10,0,1,43,34,34,34,43,34,34,34,14,34,32,32,34,34,37,8,0,0,0,1,43,34,34,34,16,2,0,0,0,0,0,0,0,2,25,11,2,21,9,0,0,0,0,0,11,34,34,34,20,2,0,0,0,0,1,43,34,34,34,18,11,20,34,34,34,25,0,0,0,0,7,13,34,34,34,37,5,15,34,
34,34,13,7,1,43,34,34,34,18,43,34,34,34,18,19,0,0,0,0,0,0,0,0,0,0,10,32,34,34,14,10,0,1,43,34,34,34,37,5,25,34,34,34,20,12,0,0,0,12,13,20,34,34,34,34,34,24,0,0,26,14,32,16,25,5,13,34,34,34,16,2,24,34,34,34,14,21,0,0,0,0,23,34,34,34,32,22,20,34,34,34,18,3,0,0,1,43,34,34,34,18,12,20,34,34,34,25,0,0,0,0,3,18,34,34,34,20,12,0,0,0,0,0,0,0,0,0,23,34,34,34,32,22,20,34,34,34,18,3,0,0,0,0,0,1,43,34,34,34,18,3,0,0,0,0,0,0,2,25,
20,34,34,34,14,43,10,9,0,0,0,0,0,0,0,9,9,0,0,0,0,0,0,0,0,0,7,5,3,0,4,4,0,0,1,5,9,0,5,1,0,0,0,0,0,0,0,0,0,7,5,3,0,0,1,5,9,0,0,0,0,0,0,0,7,1,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,4,12,12,12,12,12,5,3,4,12,12,12,8,0,0,1,12,12,12,5,19,12,12,12,2,0,0,0,7,12,12,8,0,0,7,12,12,12,12,12,5,19,0,0,0,0,4,12,12,12,8,4,12,12,12,12,12,4,0,0,0,0,0,42,12,12,12,1,5,12,12,5,1,12,12,12,42,0,0,4,12,12,12,12,12,12,4,4,12,12,2,3,12,12,12,4,0,0,3,12,12,12,4,12,12,5,12,12,12,9,0,0,4,12,12,12,4,12,12,12,12,12,12,12,12,12,4,0,0,0,0,0,4,12,12,12,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,12,12,
12,2,0,0,0,0,0,0,4,12,12,12,8,0,2,12,12,12,1,0,0,0,0,0,6,12,12,12,4,0,5,12,12,12,8,0,0,4,12,12,12,8,4,12,12,12,4,0,0,0,0,0,0,0,0,0,0,0,9,12,12,12,12,9,0,0,4,12,12,12,5,0,1,12,12,12,2,0,0,0,0,0,7,4,12,12,12,12,12,1,0,0,0,12,12,4,0,0,8,12,12,12,5,0,19,12,12,12,12,0,0,0,0,0,3,12,12,12,12,7,2,12,12,12,8,0,0,0,0,4,12,12,12,8,0,2,12,12,12,1,0,0,0,0,0,8,12,12,12,2,0,0,0,0,0,0,0,0,0,0,
3,12,12,12,12,7,2,12,12,12,8,0,0,0,0,0,0,0,4,12,12,12,8,0,0,0,0,0,0,0,0,9,4,12,12,12,2,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,7,19,19,0,0,0,0,0,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,3,19,19,19,0,0,0,0,0,7,19,19,0,19,19,3,0,0,0,0,7,19,19,19,19,19,19,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,3,19,19,19,0,0,0,0,0,7,42,19,0,0,0,0,0,0,19,19,3,19,19,3,0,0,0,0,0,0,0,9,19,19,7,0,0,0,0,0,0,0,8,8,6,0,0,
0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,19,19,19,19,19,19,3,3,19,19,0,0,0,0,0,0,0,0,0,0,0,8,8,8,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,8,8,8,9,0,0,0,0,0,0,0,0,0,0,0,4,12,12,9,0,0,0,0,0,0,0,0,0,0,0,0,7,8,8,8,0,0,0,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,0,9,9,9,0,0,0,0,0,0,0,0,0,0,0,0,9,7,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,3,1,1,1,1,1,1,1,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,19,19,0,0,0,0,0,19,19,19,19,19,19,0,0,0,0,0,19,19,19,19,19,19,19,19,0,0,0,0,1,42,42,42,42,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1,1,1,1,1,1,1,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,11,20,20,20,22,0,0,0,1,13,37,37,22,0,0,0,17,37,37,37,13,8,0,7,12,21,21,12,0,0,0,5,18,37,37,37,21,0,0,0,1,13,37,37,22,37,37,43,5,0,0,6,13,37,37,37,37,37,37,13,6,0,7,12,21,21,12,21,21,21,4,0,0,11,37,37,37,43,37,37,37,12,0,0,0,26,18,15,43,22,3,0,
0,0,23,37,37,18,37,37,43,5,0,0,0,0,0,1,25,37,37,25,42,0,0,0,0,0,22,20,20,37,12,0,0,4,13,18,13,4,0,0,0,0,0,0,0,0,4,10,21,9,0,0,0,0,0,0,0,0,0,1,13,37,37,37,37,37,37,18,43,37,37,26,0,0,42,21,21,21,12,0,0,17,16,20,20,24,7,0,0,0,0,1,10,25,25,22,2,9,7,10,43,22,0,0,0,0,22,20,20,20,23,0,0,8,21,21,21,12,0,0,0,19,43,28,14,22,0,0,0,3,22,23,22,8,0,0,0,9,11,20,20,20,22,0,0,8,21,21,21,12,0,0,2,13,18,18,22,0,0,0,
0,0,0,0,2,13,18,18,22,0,0,0,0,0,0,0,0,0,5,23,13,43,13,22,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,18,37,37,37,37,37,37,37,37,12,0,0,5,21,21,21,4,12,21,21,21,1,12,21,21,12,7,0,0,0,22,37,37,37,10,0,0,0,22,37,37,37,37,37,37,10,0,0,0,22,37,37,37,37,37,37,37,37,10,0,0,12,37,15,15,15,15,37,12,0,0,21,24,24,23,24,24,10,0,0,5,10,10,10,10,10,12,0,0,0,6,22,25,18,25,17,7,0,0,0,0,0,0,0,4,11,24,24,24,24,24,24,24,24,11,4,
0,0,4,11,24,24,24,24,24,24,12,0,0,4,11,24,24,24,24,24,24,24,24,24,24,24,24,24,21,0,3,23,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,2,0,4,18,37,37,37,37,37,37,37,37,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,21,14,31,38,40,24,0,0,0,22,29,36,44,18,9,0,0,24,27,36,36,34,12,0,10,32,28,28,14,21,0,0,26,29,36,36,33,23,0,0,0,22,29,36,44,43,35,36,29,17,0,0,12,32,36,36,27,27,36,36,32,
12,0,10,32,28,28,14,28,28,28,18,3,19,37,36,36,36,29,36,36,33,22,0,0,17,14,35,31,44,28,23,0,0,3,43,41,36,28,44,36,39,17,0,0,0,0,7,11,29,36,36,39,24,3,0,0,0,0,25,35,38,29,26,0,0,17,39,44,39,17,0,0,0,5,21,21,21,21,37,39,32,22,0,0,0,0,0,0,0,0,0,2,14,36,36,36,36,35,36,40,33,36,27,11,0,0,25,28,28,28,20,2,0,23,39,38,38,32,17,0,0,0,2,13,29,27,35,40,20,13,13,32,44,13,9,0,0,12,20,41,38,27,13,0,0,25,29,29,29,20,2,0,19,22,43,35,41,15,4,0,
0,21,28,33,39,22,0,0,0,21,14,31,38,40,24,0,0,25,29,29,29,20,2,0,22,33,44,44,37,3,0,0,0,0,0,0,22,33,44,44,37,3,0,0,0,0,0,0,0,21,37,40,44,41,35,39,20,25,26,5,8,1,0,0,0,0,0,0,0,0,0,0,0,0,0,12,28,36,36,36,36,36,36,36,39,10,0,8,37,28,28,28,13,14,28,28,28,11,34,28,28,32,10,0,0,1,37,44,36,27,25,0,0,1,37,44,36,27,44,36,27,25,0,0,1,37,44,36,27,44,36,44,36,27,25,0,0,22,40,31,31,31,31,39,10,0,7,43,27,27,28,27,27,20,8,7,18,39,39,39,
39,39,15,6,3,11,37,29,35,44,35,34,23,0,0,0,0,0,0,0,22,40,27,27,27,27,27,27,27,27,40,22,0,0,22,40,27,27,27,27,27,27,25,0,0,22,40,27,27,27,27,27,27,27,27,27,27,27,27,27,43,7,12,34,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,24,0,12,28,36,36,36,36,36,36,36,39,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,37,35,45,27,18,4,0,0,4,15,41,49,33,23,0,0,0,24,27,53,46,34,12,0,22,33,47,47,39,
17,0,0,26,29,45,49,33,23,0,0,4,15,41,49,33,29,46,38,14,5,0,0,12,32,46,53,27,27,53,46,32,12,0,22,33,47,47,39,36,47,36,37,19,19,37,36,51,36,29,45,49,33,22,0,19,43,44,49,51,53,36,20,5,0,3,43,41,47,28,44,45,39,17,0,0,0,0,22,28,31,53,53,38,29,23,9,0,0,7,18,44,30,14,5,0,0,11,33,47,28,21,0,0,26,16,29,29,29,29,40,45,36,37,1,0,0,0,0,0,0,0,0,2,14,35,31,35,35,35,36,41,31,38,27,11,0,9,13,44,47,46,34,12,0,6,13,33,47,44,37,5,0,0,25,27,30,50,50,48,
31,35,35,38,44,13,9,0,1,13,27,48,35,37,2,0,9,13,44,45,46,34,12,0,17,29,29,39,46,28,12,0,0,21,29,45,33,23,0,0,4,37,35,45,27,18,4,0,9,13,44,45,46,34,12,0,22,40,41,35,37,3,0,0,0,0,0,0,22,33,35,36,37,3,0,0,0,0,0,0,4,37,35,45,31,36,30,45,31,27,29,14,16,43,5,0,0,0,0,0,0,0,0,0,0,0,0,12,28,31,31,31,31,31,31,31,39,10,0,4,14,30,47,36,37,29,47,47,35,25,33,47,47,33,22,0,0,26,28,46,41,16,12,0,0,26,28,46,41,28,46,41,16,12,0,0,26,28,46,41,
28,46,41,46,41,16,12,0,0,22,40,49,52,52,48,39,10,0,7,43,41,47,28,30,38,20,8,7,43,41,48,48,48,38,20,8,2,32,44,46,31,41,38,36,16,4,0,0,0,0,0,0,22,40,49,53,53,53,53,53,53,49,40,22,0,0,22,40,49,53,53,53,53,35,25,0,0,22,40,49,53,53,53,53,53,53,53,53,53,53,53,41,43,7,12,28,47,53,53,53,53,53,53,53,53,53,53,53,53,53,27,24,0,12,28,31,31,31,31,31,31,31,39,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,21,24,40,47,27,37,10,0,0,0,11,40,45,46,28,21,0,0,0,25,27,53,46,34,12,0,22,33,49,45,39,17,0,0,26,29,47,49,33,23,0,0,11,40,45,46,28,41,53,44,43,3,0,0,12,32,46,50,27,27,53,46,32,12,0,22,33,49,45,39,36,51,36,37,19,19,37,36,52,36,29,47,49,33,23,0,5,14,30,51,54,54,45,29,26,0,3,43,41,47,28,44,45,39,17,0,0,0,17,14,41,50,36,36,49,36,14,10,0,0,1,37,36,36,37,1,0,9,13,44,30,14,5,0,7,18,35,47,47,47,47,47,48,36,15,42,0,0,0,0,0,0,0,0,7,23,15,31,35,
13,35,36,31,36,38,27,11,0,9,13,44,52,46,34,12,0,0,4,43,27,45,33,25,9,0,25,35,31,27,27,36,45,50,50,30,27,13,9,0,22,29,30,44,16,12,0,0,9,13,44,52,46,34,12,0,17,39,47,45,31,14,2,0,0,21,29,47,27,15,6,0,24,40,47,27,37,2,0,0,9,13,44,52,46,34,12,0,26,29,46,35,37,3,0,0,0,0,0,0,22,33,36,35,13,9,0,0,0,0,0,0,23,39,45,35,20,37,32,33,36,46,47,30,31,29,21,0,0,0,0,0,0,0,0,0,0,0,0,12,28,36,36,36,36,36,36,36,39,10,0,4,14,30,51,36,37,29,45,53,
35,25,33,49,49,33,22,0,7,13,35,30,28,22,0,0,7,13,35,30,28,35,30,28,22,0,0,7,13,35,30,28,35,30,35,30,28,22,0,0,0,22,40,44,44,44,44,39,10,0,7,43,44,44,28,44,44,20,8,7,43,33,33,33,33,33,16,8,2,32,41,33,20,37,29,31,34,12,0,0,0,0,0,0,22,40,41,41,41,41,41,41,41,41,40,22,0,0,22,40,41,41,41,41,41,35,25,0,0,22,40,41,41,41,41,41,41,41,41,41,41,41,41,41,43,7,12,28,41,41,41,41,41,41,41,41,41,41,41,41,41,41,27,24,0,4,43,16,16,16,16,16,16,16,37,12,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,43,27,27,39,27,27,20,8,0,0,13,44,52,30,14,4,0,0,1,37,41,50,35,43,1,0,24,27,49,30,28,26,0,0,4,37,44,53,44,18,7,0,13,44,52,30,32,46,53,35,25,0,0,0,22,40,48,38,32,41,49,35,18,1,0,24,27,49,30,28,31,50,44,37,19,0,11,39,47,46,32,44,53,44,18,7,5,14,30,51,54,54,45,29,26,0,3,43,41,47,28,44,45,39,17,0,0,12,16,44,48,44,14,14,44,48,44,20,21,0,4,20,38,35,13,0,0,
42,15,36,31,16,8,0,3,43,44,49,27,39,39,39,39,32,23,0,0,0,0,0,0,0,0,0,0,1,15,31,35,13,35,36,33,40,38,27,11,0,9,13,44,46,46,34,12,0,0,0,2,37,39,39,18,7,0,25,35,32,25,25,16,39,27,27,29,13,2,0,0,11,39,39,20,26,0,0,0,9,13,44,46,46,34,12,0,21,20,29,39,14,11,9,0,0,4,15,33,33,16,8,0,13,39,39,15,12,0,0,0,9,13,44,46,46,34,12,0,1,18,40,35,37,3,0,0,0,0,0,0,22,33,27,14,26,0,0,0,0,0,0,0,24,39,39,16,12,1,12,24,15,28,40,27,35,29,21,0,
0,0,0,0,0,0,0,0,0,0,0,12,28,31,31,31,31,31,31,31,39,10,0,4,14,30,46,36,37,29,46,46,35,25,33,46,46,33,22,0,19,37,40,40,25,3,0,0,19,37,40,40,37,40,40,25,3,0,0,19,37,40,40,37,40,40,40,40,25,3,0,0,0,5,13,13,13,13,13,25,4,0,0,10,13,13,24,13,13,23,9,0,12,23,23,23,23,23,26,0,8,18,37,11,5,4,43,15,43,4,0,0,0,0,0,0,12,18,43,43,43,43,43,43,43,43,18,12,0,0,12,18,43,43,43,43,43,43,26,0,0,12,18,43,43,43,43,43,43,43,43,43,43,43,43,43,22,0,6,13,
43,43,43,43,43,43,43,43,43,43,43,43,43,43,43,21,0,0,1,6,6,6,6,6,6,6,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,43,41,47,28,30,38,20,8,0,0,13,44,50,30,14,4,0,0,2,14,30,38,32,26,0,3,43,41,49,27,13,19,0,0,0,10,29,30,31,20,4,0,13,44,50,30,32,46,50,35,25,0,0,9,13,35,48,33,14,30,38,32,21,0,3,43,41,49,27,29,47,30,28,10,0,0,4,37,44,48,40,29,30,31,20,4,1,37,44,50,51,53,31,20,5,
0,3,43,41,36,28,44,36,39,17,0,9,13,27,36,35,16,10,26,15,27,36,35,18,7,4,15,29,29,23,0,0,5,20,33,33,37,19,0,0,22,34,33,14,10,10,10,10,21,7,0,0,0,0,0,0,0,0,0,0,1,15,31,35,13,35,36,37,20,38,27,11,0,0,24,34,34,34,16,5,0,0,0,0,2,17,17,5,0,0,17,43,10,9,0,4,22,24,24,17,1,0,0,0,6,17,17,21,0,0,0,0,0,24,32,32,32,15,5,0,0,5,21,17,12,7,0,0,0,0,21,23,23,17,0,0,4,17,17,12,0,0,0,0,0,24,32,32,32,15,5,0,0,4,23,25,10,0,0,0,
0,0,0,0,5,25,24,21,0,0,0,0,0,0,0,0,4,10,10,21,0,0,0,0,8,21,22,24,25,24,42,0,0,0,0,0,0,0,0,0,0,0,0,4,43,16,16,16,16,16,16,16,37,12,0,8,37,34,34,34,13,20,34,34,34,11,32,34,34,14,10,0,0,12,22,22,4,0,0,0,0,12,22,22,12,22,22,4,0,0,0,0,12,22,22,12,22,22,22,22,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,1,0,0,0,19,42,19,0,0,0,0,0,0,0,0,7,7,7,7,7,7,7,7,7,7,0,
0,0,0,7,7,7,7,7,7,7,0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,9,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,43,44,44,28,44,44,20,8,0,0,13,33,33,33,20,4,0,0,21,29,33,33,25,7,0,2,14,30,31,14,12,0,0,0,0,19,18,33,33,32,12,0,13,33,33,33,14,33,33,33,24,0,0,19,37,33,33,20,29,33,33,25,7,
0,2,14,30,31,14,33,46,27,13,3,0,0,0,10,29,33,33,18,33,33,32,12,0,10,32,44,30,36,29,11,0,0,0,23,37,37,13,37,37,18,5,0,0,22,37,37,43,12,0,0,12,18,37,37,23,0,0,12,21,21,19,0,0,0,10,23,23,21,0,0,0,7,17,23,21,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,25,20,20,23,20,20,25,18,20,20,10,0,0,1,12,12,12,5,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,2,2,2,4,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,2,2,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,6,6,6,6,6,6,6,42,0,0,0,4,12,12,12,6,2,12,12,12,19,12,12,12,12,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,13,13,24,13,13,23,9,0,0,12,23,23,23,17,0,0,0,3,22,23,23,5,0,0,5,15,32,32,24,0,
0,0,0,0,0,2,23,23,22,7,0,12,23,23,23,10,23,23,23,5,0,0,0,21,23,23,17,22,23,23,4,0,0,5,15,32,32,24,32,32,20,12,0,0,0,0,3,22,23,23,12,23,23,10,9,0,9,10,37,20,15,23,19,0,0,0,0,3,3,7,3,3,3,0,0,0,0,19,19,3,0,0,0,0,3,19,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,4,4,9,4,4,3,1,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,2,3,2,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,4,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        wOut=hOut=512;
        SDF_ASSERT(wOut*hOut==sizeof(indices)/sizeof(indices[0]));
        rawRGBAOut.resize(4*sizeof(indices)/sizeof(indices[0]));
        int idx;unsigned int pal;const unsigned char *pPal;unsigned char *pRGBA=(unsigned char *) &rawRGBAOut[0];
        const int maxPaletteIdx = sizeof(palette)/sizeof(palette[0]);	// 42
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

