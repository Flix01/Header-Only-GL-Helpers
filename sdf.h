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
 * This file embeds a sdf-AngelFont version of DejaVuSerifCondensed-Bold.
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
#       ifdef YES_SdfSDF_MSDF_MODE    // never tested
            "float median(float r, float g, float b) {\n"
            "    return max(min(r, g), min(max(r, g), b));\n"
            "}\n"
#       endif //YES_SdfSDF_MSDF_MODE
            "void main(void) {\n"
#       ifndef YES_SdfSDF_MSDF_MODE
            "float dist = texture2D(Texture, Frag_UV.st).a; // retrieve distance from texture\n"
#       else //YES_SdfSDF_MSDF_MODE   // never tested
            "vec3 dist3 = texture2D(Texture, Frag_UV.st).rgb; // retrieve distance from texture\n"
            "float dist = median(dist3.r, dist3.g, dist3.b);// - 0.5;\n"
#       endif //YES_SdfSDF_MSDF_MODE
            "float width = fwidth(dist);"
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
#       ifdef YES_SdfSDF_MSDF_MODE    // never tested
            "float median(float r, float g, float b) {\n"
            "    return max(min(r, g), min(max(r, g), b));\n"
            "}\n"
#       endif //YES_SdfSDF_MSDF_MODE
            "void main(void) {\n"
#       ifndef YES_SdfSDF_MSDF_MODE
            "float dist = texture2D(Texture, Frag_UV.st).a; // retrieve distance from texture\n"
#       else //YES_SdfSDF_MSDF_MODE   // never tested
            "vec3 dist3 = texture2D(Texture, Frag_UV.st).rgb; // retrieve distance from texture\n"
            "float dist = median(dist3.r, dist3.g, dist3.b);// - 0.5;\n"
#       endif //YES_SdfSDF_MSDF_MODE
            "float width = fwidth(dist);"
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
        CharDescriptor lf;bool lfFound = false;
        if ( (it=Chars.find((unsigned int)'\n'))==Chars.end() ) {
            lf = sp;lf.Id='\n';lf.XAdvance=lf.Width=lf.XOffset=0.f;
        }
        else {lf = it->second;lfFound=true;}
        if ( (it=Chars.find((unsigned int)'\r'))==Chars.end() ) {
            if (!lfFound) {lf = sp;lf.Id='\r';lf.XAdvance=lf.Width=lf.XOffset=0.f;}
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

    const float soc = 0.0625f;
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
        const char fntData[] = "info face=\"DejaVu Serif Condensed Bold\" size=25 bold=0 italic=0 charset=\"\" unicode=0 stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=-4,-4\n"	\
                "common lineHeight=30 base=24 scaleW=256 scaleH=256 pages=1 packed=0\n"	\
                "page id=0 file=\"DejaVuSerifCondensed-Bold.png\"\n"	\
                "chars count=155\n"	\
                "char id=0     x=138  y=30   width=16   height=27   xoffset=-3   yoffset=4    xadvance=14   page=0    chnl=0 \n"	\
                "char id=10    x=0    y=0    width=0    height=0    xoffset=-3   yoffset=0    xadvance=0    page=0    chnl=0 \n"	\
                "char id=32    x=0    y=0    width=0    height=0    xoffset=-3   yoffset=0    xadvance=8    page=0    chnl=0 \n"	\
                "char id=33    x=240  y=30   width=10   height=24   xoffset=-3   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                "char id=34    x=13   y=241  width=12   height=12   xoffset=-3   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                "char id=35    x=0    y=201  width=21   height=22   xoffset=-3   yoffset=4    xadvance=19   page=0    chnl=0 \n"	\
                "char id=36    x=187  y=0    width=16   height=28   xoffset=-3   yoffset=2    xadvance=16   page=0    chnl=0 \n"	\
                "char id=37    x=105  y=108  width=25   height=24   xoffset=-3   yoffset=3    xadvance=21   page=0    chnl=0 \n"	\
                "char id=38    x=130  y=108  width=24   height=24   xoffset=-3   yoffset=3    xadvance=20   page=0    chnl=0 \n"	\
                "char id=39    x=247  y=223  width=7    height=12   xoffset=-3   yoffset=3    xadvance=7    page=0    chnl=0 \n"	\
                "char id=40    x=67   y=30   width=12   height=27   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=41    x=79   y=30   width=12   height=27   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=42    x=99   y=223  width=16   height=17   xoffset=-3   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                "char id=43    x=21   y=201  width=19   height=20   xoffset=-3   yoffset=6    xadvance=19   page=0    chnl=0 \n"	\
                "char id=44    x=218  y=223  width=10   height=13   xoffset=-3   yoffset=18   xadvance=8    page=0    chnl=0 \n"	\
                "char id=45    x=203  y=241  width=12   height=8    xoffset=-3   yoffset=13   xadvance=9    page=0    chnl=0 \n"	\
                "char id=46    x=140  y=241  width=9    height=10   xoffset=-3   yoffset=17   xadvance=8    page=0    chnl=0 \n"	\
                "char id=47    x=154  y=30   width=13   height=26   xoffset=-3   yoffset=3    xadvance=8    page=0    chnl=0 \n"	\
                "char id=48    x=71   y=108  width=18   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=49    x=0    y=178  width=16   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=50    x=16   y=178  width=17   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=51    x=232  y=83   width=18   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=52    x=33   y=178  width=19   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=53    x=0    y=108  width=17   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=54    x=17   y=108  width=18   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=55    x=52   y=178  width=17   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=56    x=35   y=108  width=18   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=57    x=53   y=108  width=18   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=58    x=245  y=178  width=9    height=18   xoffset=-3   yoffset=9    xadvance=8    page=0    chnl=0 \n"	\
                "char id=59    x=234  y=178  width=11   height=22   xoffset=-3   yoffset=9    xadvance=8    page=0    chnl=0 \n"	\
                "char id=60    x=129  y=201  width=19   height=19   xoffset=-3   yoffset=7    xadvance=19   page=0    chnl=0 \n"	\
                "char id=61    x=228  y=223  width=19   height=13   xoffset=-3   yoffset=10   xadvance=19   page=0    chnl=0 \n"	\
                "char id=62    x=148  y=201  width=19   height=19   xoffset=-3   yoffset=7    xadvance=19   page=0    chnl=0 \n"	\
                "char id=63    x=89   y=108  width=16   height=24   xoffset=-3   yoffset=3    xadvance=13   page=0    chnl=0 \n"	\
                "char id=64    x=114  y=30   width=24   height=27   xoffset=-3   yoffset=4    xadvance=23   page=0    chnl=0 \n"	\
                "char id=65    x=0    y=132  width=23   height=23   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=66    x=23   y=132  width=21   height=23   xoffset=-3   yoffset=3    xadvance=19   page=0    chnl=0 \n"	\
                "char id=67    x=44   y=83   width=21   height=24   xoffset=-3   yoffset=3    xadvance=18   page=0    chnl=0 \n"	\
                "char id=68    x=44   y=132  width=22   height=23   xoffset=-3   yoffset=3    xadvance=20   page=0    chnl=0 \n"	\
                "char id=69    x=66   y=132  width=19   height=23   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=70    x=85   y=132  width=19   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=71    x=65   y=83   width=22   height=24   xoffset=-3   yoffset=3    xadvance=19   page=0    chnl=0 \n"	\
                "char id=72    x=104  y=132  width=24   height=23   xoffset=-3   yoffset=3    xadvance=21   page=0    chnl=0 \n"	\
                "char id=73    x=242  y=108  width=13   height=23   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=74    x=8    y=0    width=17   height=29   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=75    x=128  y=132  width=24   height=23   xoffset=-3   yoffset=3    xadvance=20   page=0    chnl=0 \n"	\
                "char id=76    x=152  y=132  width=19   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=77    x=171  y=132  width=28   height=23   xoffset=-3   yoffset=3    xadvance=25   page=0    chnl=0 \n"	\
                "char id=78    x=199  y=132  width=24   height=23   xoffset=-3   yoffset=3    xadvance=21   page=0    chnl=0 \n"	\
                "char id=79    x=87   y=83   width=23   height=24   xoffset=-3   yoffset=3    xadvance=20   page=0    chnl=0 \n"	\
                "char id=80    x=223  y=132  width=20   height=23   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=81    x=133  y=0    width=23   height=28   xoffset=-3   yoffset=3    xadvance=20   page=0    chnl=0 \n"	\
                "char id=82    x=0    y=155  width=22   height=23   xoffset=-3   yoffset=3    xadvance=19   page=0    chnl=0 \n"	\
                "char id=83    x=235  y=58   width=18   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=84    x=22   y=155  width=21   height=23   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=85    x=110  y=83   width=24   height=24   xoffset=-3   yoffset=3    xadvance=20   page=0    chnl=0 \n"	\
                "char id=86    x=43   y=155  width=23   height=23   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=87    x=66   y=155  width=31   height=23   xoffset=-3   yoffset=3    xadvance=25   page=0    chnl=0 \n"	\
                "char id=88    x=97   y=155  width=22   height=23   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=89    x=119  y=155  width=22   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=90    x=141  y=155  width=20   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=91    x=91   y=30   width=12   height=27   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=92    x=167  y=30   width=13   height=26   xoffset=-3   yoffset=3    xadvance=8    page=0    chnl=0 \n"	\
                "char id=93    x=103  y=30   width=11   height=27   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=94    x=25   y=241  width=19   height=12   xoffset=-3   yoffset=3    xadvance=19   page=0    chnl=0 \n"	\
                "char id=95    x=215  y=241  width=16   height=7    xoffset=-3   yoffset=25   xadvance=11   page=0    chnl=0 \n"	\
                "char id=96    x=192  y=241  width=11   height=9    xoffset=-3   yoffset=2    xadvance=11   page=0    chnl=0 \n"	\
                "char id=97    x=40   y=201  width=19   height=19   xoffset=-3   yoffset=8    xadvance=15   page=0    chnl=0 \n"	\
                "char id=98    x=134  y=83   width=19   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=99    x=59   y=201  width=17   height=19   xoffset=-3   yoffset=8    xadvance=14   page=0    chnl=0 \n"	\
                "char id=100   x=153  y=83   width=20   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=101   x=76   y=201  width=18   height=19   xoffset=-3   yoffset=8    xadvance=14   page=0    chnl=0 \n"	\
                "char id=102   x=161  y=155  width=15   height=23   xoffset=-3   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                "char id=103   x=173  y=83   width=20   height=24   xoffset=-3   yoffset=8    xadvance=16   page=0    chnl=0 \n"	\
                "char id=104   x=176  y=155  width=20   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=105   x=243  y=132  width=12   height=23   xoffset=-3   yoffset=3    xadvance=9    page=0    chnl=0 \n"	\
                "char id=106   x=25   y=0    width=14   height=29   xoffset=-3   yoffset=3    xadvance=8    page=0    chnl=0 \n"	\
                "char id=107   x=196  y=155  width=20   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=108   x=216  y=155  width=12   height=23   xoffset=-3   yoffset=3    xadvance=9    page=0    chnl=0 \n"	\
                "char id=109   x=189  y=201  width=28   height=18   xoffset=-3   yoffset=8    xadvance=24   page=0    chnl=0 \n"	\
                "char id=110   x=217  y=201  width=20   height=18   xoffset=-3   yoffset=8    xadvance=16   page=0    chnl=0 \n"	\
                "char id=111   x=94   y=201  width=19   height=19   xoffset=-3   yoffset=8    xadvance=15   page=0    chnl=0 \n"	\
                "char id=112   x=193  y=83   width=19   height=24   xoffset=-3   yoffset=8    xadvance=16   page=0    chnl=0 \n"	\
                "char id=113   x=212  y=83   width=20   height=24   xoffset=-3   yoffset=8    xadvance=16   page=0    chnl=0 \n"	\
                "char id=114   x=237  y=201  width=17   height=18   xoffset=-3   yoffset=8    xadvance=12   page=0    chnl=0 \n"	\
                "char id=115   x=113  y=201  width=16   height=19   xoffset=-3   yoffset=8    xadvance=13   page=0    chnl=0 \n"	\
                "char id=116   x=219  y=178  width=15   height=22   xoffset=-3   yoffset=5    xadvance=10   page=0    chnl=0 \n"	\
                "char id=117   x=0    y=223  width=20   height=18   xoffset=-3   yoffset=9    xadvance=16   page=0    chnl=0 \n"	\
                "char id=118   x=20   y=223  width=19   height=17   xoffset=-3   yoffset=9    xadvance=13   page=0    chnl=0 \n"	\
                "char id=119   x=39   y=223  width=25   height=17   xoffset=-3   yoffset=9    xadvance=19   page=0    chnl=0 \n"	\
                "char id=120   x=64   y=223  width=18   height=17   xoffset=-3   yoffset=9    xadvance=13   page=0    chnl=0 \n"	\
                "char id=121   x=228  y=155  width=19   height=23   xoffset=-3   yoffset=9    xadvance=13   page=0    chnl=0 \n"	\
                "char id=122   x=82   y=223  width=17   height=17   xoffset=-3   yoffset=9    xadvance=13   page=0    chnl=0 \n"	\
                "char id=123   x=156  y=0    width=16   height=28   xoffset=-3   yoffset=3    xadvance=14   page=0    chnl=0 \n"	\
                "char id=124   x=0    y=0    width=8    height=30   xoffset=-3   yoffset=2    xadvance=8    page=0    chnl=0 \n"	\
                "char id=125   x=172  y=0    width=15   height=28   xoffset=-3   yoffset=3    xadvance=14   page=0    chnl=0 \n"	\
                "char id=126   x=149  y=241  width=19   height=10   xoffset=-3   yoffset=11   xadvance=19   page=0    chnl=0 \n"	\
                "char id=161   x=167  y=178  width=10   height=23   xoffset=-3   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                "char id=162   x=160  y=58   width=17   height=25   xoffset=-3   yoffset=5    xadvance=16   page=0    chnl=0 \n"	\
                "char id=163   x=91   y=178  width=18   height=23   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=167   x=180  y=30   width=16   height=26   xoffset=-3   yoffset=3    xadvance=12   page=0    chnl=0 \n"	\
                "char id=169   x=177  y=178  width=21   height=23   xoffset=-3   yoffset=3    xadvance=23   page=0    chnl=0 \n"	\
                "char id=171   x=150  y=223  width=15   height=17   xoffset=-3   yoffset=8    xadvance=14   page=0    chnl=0 \n"	\
                "char id=174   x=198  y=178  width=21   height=23   xoffset=-3   yoffset=3    xadvance=23   page=0    chnl=0 \n"	\
                "char id=176   x=0    y=241  width=13   height=13   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=178   x=192  y=223  width=13   height=15   xoffset=-3   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                "char id=179   x=205  y=223  width=13   height=15   xoffset=-3   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                "char id=181   x=126  y=178  width=20   height=23   xoffset=-3   yoffset=9    xadvance=16   page=0    chnl=0 \n"	\
                "char id=185   x=180  y=223  width=12   height=15   xoffset=-3   yoffset=3    xadvance=10   page=0    chnl=0 \n"	\
                "char id=186   x=135  y=223  width=15   height=17   xoffset=-3   yoffset=3    xadvance=11   page=0    chnl=0 \n"	\
                "char id=187   x=165  y=223  width=15   height=17   xoffset=-3   yoffset=8    xadvance=14   page=0    chnl=0 \n"	\
                "char id=191   x=186  y=108  width=15   height=24   xoffset=-3   yoffset=3    xadvance=13   page=0    chnl=0 \n"	\
                "char id=192   x=21   y=30   width=23   height=28   xoffset=-3   yoffset=-2   xadvance=17   page=0    chnl=0 \n"	\
                "char id=196   x=44   y=30   width=23   height=28   xoffset=-3   yoffset=-2   xadvance=17   page=0    chnl=0 \n"	\
                "char id=199   x=0    y=30   width=21   height=28   xoffset=-3   yoffset=3    xadvance=18   page=0    chnl=0 \n"	\
                "char id=200   x=203  y=0    width=19   height=28   xoffset=-3   yoffset=-2   xadvance=17   page=0    chnl=0 \n"	\
                "char id=201   x=222  y=0    width=19   height=28   xoffset=-3   yoffset=-2   xadvance=17   page=0    chnl=0 \n"	\
                "char id=204   x=241  y=0    width=13   height=28   xoffset=-3   yoffset=-2   xadvance=11   page=0    chnl=0 \n"	\
                "char id=210   x=39   y=0    width=23   height=29   xoffset=-3   yoffset=-2   xadvance=20   page=0    chnl=0 \n"	\
                "char id=214   x=86   y=0    width=23   height=29   xoffset=-3   yoffset=-2   xadvance=20   page=0    chnl=0 \n"	\
                "char id=217   x=62   y=0    width=24   height=29   xoffset=-3   yoffset=-2   xadvance=20   page=0    chnl=0 \n"	\
                "char id=220   x=109  y=0    width=24   height=29   xoffset=-3   yoffset=-2   xadvance=20   page=0    chnl=0 \n"	\
                "char id=223   x=201  y=108  width=21   height=24   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=224   x=102  y=58   width=19   height=25   xoffset=-3   yoffset=2    xadvance=15   page=0    chnl=0 \n"	\
                "char id=228   x=177  y=58   width=19   height=25   xoffset=-3   yoffset=2    xadvance=15   page=0    chnl=0 \n"	\
                "char id=231   x=109  y=178  width=17   height=23   xoffset=-3   yoffset=8    xadvance=14   page=0    chnl=0 \n"	\
                "char id=232   x=66   y=58   width=18   height=25   xoffset=-3   yoffset=2    xadvance=14   page=0    chnl=0 \n"	\
                "char id=233   x=84   y=58   width=18   height=25   xoffset=-3   yoffset=2    xadvance=14   page=0    chnl=0 \n"	\
                "char id=236   x=174  y=108  width=12   height=24   xoffset=-3   yoffset=2    xadvance=9    page=0    chnl=0 \n"	\
                "char id=241   x=222  y=108  width=20   height=24   xoffset=-3   yoffset=2    xadvance=16   page=0    chnl=0 \n"	\
                "char id=242   x=121  y=58   width=19   height=25   xoffset=-3   yoffset=2    xadvance=15   page=0    chnl=0 \n"	\
                "char id=246   x=196  y=58   width=19   height=25   xoffset=-3   yoffset=2    xadvance=15   page=0    chnl=0 \n"	\
                "char id=249   x=140  y=58   width=20   height=25   xoffset=-3   yoffset=2    xadvance=16   page=0    chnl=0 \n"	\
                "char id=252   x=215  y=58   width=20   height=25   xoffset=-3   yoffset=2    xadvance=16   page=0    chnl=0 \n"	\
                "char id=8216  x=110  y=241  width=10   height=12   xoffset=-3   yoffset=3    xadvance=8    page=0    chnl=0 \n"	\
                "char id=8217  x=120  y=241  width=10   height=12   xoffset=-3   yoffset=3    xadvance=8    page=0    chnl=0 \n"	\
                "char id=8218  x=130  y=241  width=10   height=12   xoffset=-3   yoffset=18   xadvance=8    page=0    chnl=0 \n"	\
                "char id=8220  x=65   y=241  width=15   height=12   xoffset=-3   yoffset=3    xadvance=13   page=0    chnl=0 \n"	\
                "char id=8221  x=80   y=241  width=15   height=12   xoffset=-3   yoffset=3    xadvance=13   page=0    chnl=0 \n"	\
                "char id=8222  x=95   y=241  width=15   height=12   xoffset=-3   yoffset=18   xadvance=13   page=0    chnl=0 \n"	\
                "char id=8230  x=168  y=241  width=24   height=10   xoffset=-3   yoffset=17   xadvance=23   page=0    chnl=0 \n"	\
                "char id=8249  x=115  y=223  width=10   height=17   xoffset=-3   yoffset=8    xadvance=9    page=0    chnl=0 \n"	\
                "char id=8250  x=125  y=223  width=10   height=17   xoffset=-3   yoffset=8    xadvance=9    page=0    chnl=0 \n"	\
                "char id=8364  x=154  y=108  width=20   height=24   xoffset=-3   yoffset=3    xadvance=16   page=0    chnl=0 \n"	\
                "char id=8482  x=44   y=241  width=21   height=12   xoffset=-3   yoffset=3    xadvance=23   page=0    chnl=0 \n"	\
                "char id=8710  x=146  y=178  width=21   height=23   xoffset=-3   yoffset=3    xadvance=17   page=0    chnl=0 \n"	\
                "char id=9167  x=167  y=201  width=22   height=19   xoffset=-3   yoffset=7    xadvance=21   page=0    chnl=0 \n"	\
                "char id=9632  x=0    y=58   width=22   height=25   xoffset=-3   yoffset=5    xadvance=21   page=0    chnl=0 \n"	\
                "char id=9633  x=0    y=83   width=22   height=25   xoffset=-3   yoffset=5    xadvance=21   page=0    chnl=0 \n"	\
                "char id=9635  x=22   y=83   width=22   height=25   xoffset=-3   yoffset=5    xadvance=21   page=0    chnl=0 \n"	\
                "char id=9650  x=44   y=58   width=22   height=25   xoffset=-3   yoffset=5    xadvance=17   page=0    chnl=0 \n"	\
                "char id=9654  x=196  y=30   width=22   height=25   xoffset=-3   yoffset=5    xadvance=17   page=0    chnl=0 \n"	\
                "char id=9660  x=22   y=58   width=22   height=25   xoffset=-3   yoffset=5    xadvance=17   page=0    chnl=0 \n"	\
                "char id=9664  x=218  y=30   width=22   height=25   xoffset=-3   yoffset=5    xadvance=17   page=0    chnl=0 \n"	\
                "char id=9786  x=69   y=178  width=22   height=23   xoffset=-3   yoffset=3    xadvance=20   page=0    chnl=0 \n"	\
                "kernings count=320\n"	\
                "kerning first=85 second=44 amount=-2\n"	\
                "kerning first=70 second=45 amount=-1\n"	\
                "kerning first=8222 second=89 amount=-2\n"	\
                "kerning first=114 second=8221 amount=1\n"	\
                "kerning first=70 second=58 amount=-1\n"	\
                "kerning first=220 second=59 amount=-1\n"	\
                "kerning first=86 second=233 amount=-2\n"	\
                "kerning first=171 second=74 amount=-1\n"	\
                "kerning first=75 second=79 amount=-1\n"	\
                "kerning first=83 second=83 amount=-1\n"	\
                "kerning first=76 second=85 amount=-1\n"	\
                "kerning first=45 second=86 amount=-2\n"	\
                "kerning first=45 second=88 amount=-2\n"	\
                "kerning first=76 second=89 amount=-2\n"	\
                "kerning first=84 second=99 amount=-3\n"	\
                "kerning first=75 second=101 amount=-1\n"	\
                "kerning first=70 second=111 amount=-1\n"	\
                "kerning first=84 second=114 amount=-2\n"	\
                "kerning first=84 second=115 amount=-2\n"	\
                "kerning first=196 second=118 amount=-1\n"	\
                "kerning first=192 second=119 amount=-1\n"	\
                "kerning first=84 second=171 amount=-2\n"	\
                "kerning first=84 second=121 amount=-3\n"	\
                "kerning first=84 second=187 amount=-2\n"	\
                "kerning first=89 second=252 amount=-2\n"	\
                "kerning first=84 second=58 amount=-1\n"	\
                "kerning first=75 second=214 amount=-1\n"	\
                "kerning first=76 second=217 amount=-1\n"	\
                "kerning first=84 second=231 amount=-3\n"	\
                "kerning first=87 second=233 amount=-2\n"	\
                "kerning first=84 second=246 amount=-2\n"	\
                "kerning first=86 second=249 amount=-2\n"	\
                "kerning first=85 second=65 amount=-1\n"	\
                "kerning first=82 second=228 amount=1\n"	\
                "kerning first=75 second=233 amount=-1\n"	\
                "kerning first=84 second=196 amount=-1\n"	\
                "kerning first=65 second=119 amount=-1\n"	\
                "kerning first=192 second=121 amount=-1\n"	\
                "kerning first=87 second=121 amount=-1\n"	\
                "kerning first=80 second=101 amount=-1\n"	\
                "kerning first=8222 second=84 amount=-3\n"	\
                "kerning first=86 second=45 amount=-2\n"	\
                "kerning first=70 second=192 amount=-1\n"	\
                "kerning first=220 second=192 amount=-1\n"	\
                "kerning first=68 second=44 amount=-1\n"	\
                "kerning first=217 second=192 amount=-1\n"	\
                "kerning first=70 second=246 amount=-1\n"	\
                "kerning first=8220 second=89 amount=1\n"	\
                "kerning first=89 second=233 amount=-2\n"	\
                "kerning first=76 second=121 amount=-1\n"	\
                "kerning first=118 second=8220 amount=1\n"	\
                "kerning first=8250 second=84 amount=-2\n"	\
                "kerning first=75 second=210 amount=-1\n"	\
                "kerning first=80 second=97 amount=-1\n"	\
                "kerning first=220 second=196 amount=-1\n"	\
                "kerning first=88 second=65 amount=-1\n"	\
                "kerning first=84 second=117 amount=-2\n"	\
                "kerning first=8216 second=192 amount=-3\n"	\
                "kerning first=80 second=224 amount=-1\n"	\
                "kerning first=65 second=121 amount=-1\n"	\
                "kerning first=45 second=87 amount=-1\n"	\
                "kerning first=87 second=58 amount=-2\n"	\
                "kerning first=220 second=46 amount=-2\n"	\
                "kerning first=85 second=46 amount=-2\n"	\
                "kerning first=45 second=89 amount=-3\n"	\
                "kerning first=187 second=84 amount=-2\n"	\
                "kerning first=217 second=58 amount=-1\n"	\
                "kerning first=83 second=45 amount=1\n"	\
                "kerning first=76 second=8217 amount=-6\n"	\
                "kerning first=74 second=59 amount=-1\n"	\
                "kerning first=210 second=45 amount=1\n"	\
                "kerning first=86 second=252 amount=-2\n"	\
                "kerning first=121 second=44 amount=-2\n"	\
                "kerning first=217 second=196 amount=-1\n"	\
                "kerning first=87 second=246 amount=-2\n"	\
                "kerning first=82 second=86 amount=-1\n"	\
                "kerning first=71 second=46 amount=-1\n"	\
                "kerning first=192 second=8217 amount=-3\n"	\
                "kerning first=118 second=46 amount=-3\n"	\
                "kerning first=80 second=45 amount=-1\n"	\
                "kerning first=74 second=44 amount=-1\n"	\
                "kerning first=75 second=252 amount=-1\n"	\
                "kerning first=121 second=8216 amount=1\n"	\
                "kerning first=87 second=8222 amount=-3\n"	\
                "kerning first=79 second=46 amount=-1\n"	\
                "kerning first=86 second=101 amount=-2\n"	\
                "kerning first=70 second=233 amount=-1\n"	\
                "kerning first=80 second=65 amount=-2\n"	\
                "kerning first=87 second=196 amount=-2\n"	\
                "kerning first=187 second=74 amount=-1\n"	\
                "kerning first=86 second=58 amount=-3\n"	\
                "kerning first=102 second=46 amount=-1\n"	\
                "kerning first=89 second=101 amount=-2\n"	\
                "kerning first=192 second=8221 amount=-3\n"	\
                "kerning first=8222 second=119 amount=-1\n"	\
                "kerning first=8249 second=84 amount=-2\n"	\
                "kerning first=214 second=45 amount=1\n"	\
                "kerning first=76 second=84 amount=-2\n"	\
                "kerning first=214 second=44 amount=-1\n"	\
                "kerning first=65 second=84 amount=-1\n"	\
                "kerning first=84 second=111 amount=-3\n"	\
                "kerning first=80 second=228 amount=-1\n"	\
                "kerning first=75 second=121 amount=-1\n"	\
                "kerning first=119 second=8220 amount=1\n"	\
                "kerning first=87 second=44 amount=-4\n"	\
                "kerning first=84 second=45 amount=-4\n"	\
                "kerning first=8216 second=196 amount=-3\n"	\
                "kerning first=84 second=233 amount=-3\n"	\
                "kerning first=85 second=59 amount=-1\n"	\
                "kerning first=85 second=192 amount=-1\n"	\
                "kerning first=65 second=8217 amount=-3\n"	\
                "kerning first=86 second=224 amount=-2\n"	\
                "kerning first=196 second=89 amount=-1\n"	\
                "kerning first=87 second=117 amount=-1\n"	\
                "kerning first=118 second=44 amount=-2\n"	\
                "kerning first=76 second=87 amount=-2\n"	\
                "kerning first=87 second=111 amount=-2\n"	\
                "kerning first=86 second=59 amount=-3\n"	\
                "kerning first=89 second=58 amount=-3\n"	\
                "kerning first=192 second=87 amount=-1\n"	\
                "kerning first=74 second=46 amount=-2\n"	\
                "kerning first=84 second=44 amount=-3\n"	\
                "kerning first=84 second=84 amount=-1\n"	\
                "kerning first=88 second=196 amount=-1\n"	\
                "kerning first=89 second=224 amount=-2\n"	\
                "kerning first=118 second=8221 amount=1\n"	\
                "kerning first=102 second=8220 amount=1\n"	\
                "kerning first=78 second=58 amount=-1\n"	\
                "kerning first=84 second=8250 amount=-2\n"	\
                "kerning first=81 second=45 amount=1\n"	\
                "kerning first=119 second=46 amount=-3\n"	\
                "kerning first=86 second=65 amount=-2\n"	\
                "kerning first=75 second=85 amount=-1\n"	\
                "kerning first=118 second=8217 amount=1\n"	\
                "kerning first=65 second=87 amount=-1\n"	\
                "kerning first=87 second=8218 amount=-3\n"	\
                "kerning first=220 second=44 amount=-2\n"	\
                "kerning first=79 second=44 amount=-1\n"	\
                "kerning first=86 second=8218 amount=-3\n"	\
                "kerning first=80 second=59 amount=-1\n"	\
                "kerning first=75 second=65 amount=-1\n"	\
                "kerning first=119 second=8216 amount=1\n"	\
                "kerning first=121 second=8217 amount=1\n"	\
                "kerning first=65 second=8221 amount=-3\n"	\
                "kerning first=80 second=232 amount=-1\n"	\
                "kerning first=121 second=46 amount=-3\n"	\
                "kerning first=217 second=44 amount=-2\n"	\
                "kerning first=85 second=196 amount=-1\n"	\
                "kerning first=70 second=101 amount=-1\n"	\
                "kerning first=196 second=119 amount=-1\n"	\
                "kerning first=210 second=46 amount=-1\n"	\
                "kerning first=196 second=86 amount=-2\n"	\
                "kerning first=80 second=46 amount=-4\n"	\
                "kerning first=82 second=8217 amount=-1\n"	\
                "kerning first=121 second=8220 amount=1\n"	\
                "kerning first=76 second=8221 amount=-6\n"	\
                "kerning first=84 second=8218 amount=-2\n"	\
                "kerning first=80 second=192 amount=-2\n"	\
                "kerning first=192 second=84 amount=-1\n"	\
                "kerning first=76 second=220 amount=-1\n"	\
                "kerning first=89 second=65 amount=-2\n"	\
                "kerning first=87 second=97 amount=-2\n"	\
                "kerning first=8249 second=74 amount=-1\n"	\
                "kerning first=70 second=59 amount=-1\n"	\
                "kerning first=8220 second=88 amount=1\n"	\
                "kerning first=75 second=67 amount=-1\n"	\
                "kerning first=86 second=232 amount=-2\n"	\
                "kerning first=84 second=101 amount=-3\n"	\
                "kerning first=192 second=118 amount=-1\n"	\
                "kerning first=196 second=84 amount=-1\n"	\
                "kerning first=78 second=44 amount=-2\n"	\
                "kerning first=75 second=111 amount=-1\n"	\
                "kerning first=70 second=224 amount=-1\n"	\
                "kerning first=80 second=196 amount=-2\n"	\
                "kerning first=89 second=228 amount=-2\n"	\
                "kerning first=87 second=242 amount=-2\n"	\
                "kerning first=86 second=46 amount=-4\n"	\
                "kerning first=114 second=8220 amount=1\n"	\
                "kerning first=82 second=87 amount=-1\n"	\
                "kerning first=114 second=44 amount=-2\n"	\
                "kerning first=84 second=46 amount=-3\n"	\
                "kerning first=82 second=89 amount=-1\n"	\
                "kerning first=84 second=8222 amount=-2\n"	\
                "kerning first=80 second=242 amount=-1\n"	\
                "kerning first=89 second=44 amount=-3\n"	\
                "kerning first=45 second=84 amount=-4\n"	\
                "kerning first=196 second=121 amount=-1\n"	\
                "kerning first=102 second=8217 amount=2\n"	\
                "kerning first=89 second=111 amount=-2\n"	\
                "kerning first=84 second=224 amount=-1\n"	\
                "kerning first=75 second=217 amount=-1\n"	\
                "kerning first=89 second=249 amount=-2\n"	\
                "kerning first=82 second=8221 amount=-1\n"	\
                "kerning first=86 second=44 amount=-4\n"	\
                "kerning first=86 second=8222 amount=-3\n"	\
                "kerning first=87 second=101 amount=-2\n"	\
                "kerning first=80 second=58 amount=-1\n"	\
                "kerning first=75 second=249 amount=-1\n"	\
                "kerning first=83 second=46 amount=-1\n"	\
                "kerning first=79 second=45 amount=1\n"	\
                "kerning first=84 second=59 amount=-1\n"	\
                "kerning first=8222 second=86 amount=-4\n"	\
                "kerning first=81 second=44 amount=-1\n"	\
                "kerning first=114 second=8216 amount=1\n"	\
                "kerning first=70 second=65 amount=-1\n"	\
                "kerning first=70 second=97 amount=-1\n"	\
                "kerning first=70 second=228 amount=-1\n"	\
                "kerning first=87 second=114 amount=-1\n"	\
                "kerning first=78 second=59 amount=-1\n"	\
                "kerning first=75 second=192 amount=-1\n"	\
                "kerning first=89 second=45 amount=-3\n"	\
                "kerning first=196 second=8217 amount=-3\n"	\
                "kerning first=87 second=224 amount=-2\n"	\
                "kerning first=84 second=97 amount=-2\n"	\
                "kerning first=70 second=44 amount=-3\n"	\
                "kerning first=114 second=46 amount=-2\n"	\
                "kerning first=78 second=46 amount=-2\n"	\
                "kerning first=89 second=59 amount=-3\n"	\
                "kerning first=86 second=192 amount=-2\n"	\
                "kerning first=84 second=65 amount=-1\n"	\
                "kerning first=8220 second=65 amount=-3\n"	\
                "kerning first=87 second=45 amount=-1\n"	\
                "kerning first=75 second=89 amount=-1\n"	\
                "kerning first=75 second=196 amount=-1\n"	\
                "kerning first=89 second=232 amount=-2\n"	\
                "kerning first=84 second=228 amount=-1\n"	\
                "kerning first=65 second=118 amount=-1\n"	\
                "kerning first=89 second=46 amount=-3\n"	\
                "kerning first=86 second=242 amount=-2\n"	\
                "kerning first=121 second=8221 amount=1\n"	\
                "kerning first=8220 second=86 amount=1\n"	\
                "kerning first=89 second=192 amount=-2\n"	\
                "kerning first=75 second=232 amount=-1\n"	\
                "kerning first=80 second=246 amount=-1\n"	\
                "kerning first=8222 second=87 amount=-3\n"	\
                "kerning first=86 second=121 amount=-1\n"	\
                "kerning first=75 second=242 amount=-1\n"	\
                "kerning first=86 second=111 amount=-2\n"	\
                "kerning first=76 second=8220 amount=-6\n"	\
                "kerning first=192 second=86 amount=-2\n"	\
                "kerning first=75 second=199 amount=-1\n"	\
                "kerning first=80 second=8222 amount=-3\n"	\
                "kerning first=68 second=46 amount=-1\n"	\
                "kerning first=84 second=8249 amount=-2\n"	\
                "kerning first=87 second=65 amount=-2\n"	\
                "kerning first=88 second=192 amount=-1\n"	\
                "kerning first=89 second=196 amount=-2\n"	\
                "kerning first=220 second=58 amount=-1\n"	\
                "kerning first=8222 second=118 amount=-1\n"	\
                "kerning first=196 second=87 amount=-1\n"	\
                "kerning first=210 second=44 amount=-1\n"	\
                "kerning first=217 second=65 amount=-1\n"	\
                "kerning first=87 second=249 amount=-1\n"	\
                "kerning first=89 second=242 amount=-2\n"	\
                "kerning first=80 second=44 amount=-3\n"	\
                "kerning first=119 second=8217 amount=1\n"	\
                "kerning first=119 second=44 amount=-2\n"	\
                "kerning first=70 second=232 amount=-1\n"	\
                "kerning first=89 second=117 amount=-2\n"	\
                "kerning first=65 second=86 amount=-2\n"	\
                "kerning first=86 second=246 amount=-2\n"	\
                "kerning first=70 second=46 amount=-3\n"	\
                "kerning first=84 second=232 amount=-2\n"	\
                "kerning first=118 second=8216 amount=1\n"	\
                "kerning first=80 second=111 amount=-1\n"	\
                "kerning first=86 second=117 amount=-2\n"	\
                "kerning first=8220 second=192 amount=-3\n"	\
                "kerning first=196 second=8221 amount=-3\n"	\
                "kerning first=87 second=252 amount=-1\n"	\
                "kerning first=76 second=86 amount=-3\n"	\
                "kerning first=80 second=233 amount=-1\n"	\
                "kerning first=82 second=97 amount=1\n"	\
                "kerning first=86 second=228 amount=-2\n"	\
                "kerning first=8220 second=196 amount=-3\n"	\
                "kerning first=81 second=46 amount=-1\n"	\
                "kerning first=86 second=196 amount=-2\n"	\
                "kerning first=84 second=249 amount=-2\n"	\
                "kerning first=88 second=45 amount=-1\n"	\
                "kerning first=75 second=220 amount=-1\n"	\
                "kerning first=82 second=224 amount=1\n"	\
                "kerning first=102 second=8221 amount=2\n"	\
                "kerning first=89 second=97 amount=-2\n"	\
                "kerning first=102 second=44 amount=-1\n"	\
                "kerning first=8250 second=74 amount=-1\n"	\
                "kerning first=87 second=228 amount=-2\n"	\
                "kerning first=171 second=84 amount=-2\n"	\
                "kerning first=102 second=45 amount=-1\n"	\
                "kerning first=220 second=65 amount=-1\n"	\
                "kerning first=70 second=196 amount=-1\n"	\
                "kerning first=75 second=117 amount=-1\n"	\
                "kerning first=65 second=89 amount=-1\n"	\
                "kerning first=71 second=44 amount=-1\n"	\
                "kerning first=86 second=97 amount=-2\n"	\
                "kerning first=217 second=46 amount=-2\n"	\
                "kerning first=87 second=59 amount=-2\n"	\
                "kerning first=80 second=8218 amount=-3\n"	\
                "kerning first=75 second=45 amount=-2\n"	\
                "kerning first=84 second=192 amount=-1\n"	\
                "kerning first=119 second=8221 amount=1\n"	\
                "kerning first=75 second=246 amount=-1\n"	\
                "kerning first=87 second=232 amount=-2\n"	\
                "kerning first=84 second=119 amount=-3\n"	\
                "kerning first=85 second=58 amount=-1\n"	\
                "kerning first=87 second=46 amount=-4\n"	\
                "kerning first=70 second=242 amount=-1\n"	\
                "kerning first=75 second=87 amount=-1\n"	\
                "kerning first=87 second=192 amount=-2\n"	\
                "kerning first=8220 second=87 amount=1\n"	\
                "kerning first=74 second=58 amount=-1\n"	\
                "kerning first=114 second=8217 amount=1\n"	\
                "kerning first=214 second=46 amount=-1\n"	\
                "kerning first=107 second=45 amount=-1\n"	\
                "kerning first=83 second=44 amount=-1\n"	\
                "kerning first=84 second=252 amount=-2\n"	\
                "kerning first=8216 second=65 amount=-3\n"	\
                "kerning first=76 second=8216 amount=-6\n"	\
                "kerning first=89 second=246 amount=-2\n"	\
                "kerning first=192 second=89 amount=-1\n"	\
                "kerning first=84 second=242 amount=-2\n"	\
                "kerning first=217 second=59 amount=-1\n\n";

        fntFileOut.resize(sizeof(fntData)/sizeof(fntData[0]));
        memcpy(&fntFileOut[0],fntData,fntFileOut.size());
    }
    // "DejaVuSerifCondensed-Bold Image":
    {
        /*
         warning: integer literal is too large to be represented in
         type 'long' and is subject to undefined behavior under C++98, interpreting
         as 'unsigned long'; this literal will have type 'long long' in C++11
         onwards [-Wc++11-compat]
        */

        const unsigned int palette[42] = {	// each is a 4 bytes (RGBA) integer in little-endian encoding
                                            0U,402653183U,570425343U,150994943U,318767103U,503316479U,285212671U,184549375U,66582519U,587202559U,754974719U,805306367U,1358954495U,1593835519U,1493172223U,889192447U,1224736767U,1157627903U,1040187391U,956301311U,1308622847U,1728053247U,1845493759U,2315255807U,2717908991U,2449473535U,2013265919U,2214592511U,2550136831U,2030043135U,
                                            1879048191U,2147483647U,2885681151U,2952790015U,3120562175U,3875536895U,2734686207U,3623878655U,3472883711U,3271557119U,4294967295U,4160749567U};

        const int indices[256*256] = {	// [width x height]: each points to a palette entry
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,1,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,6,0,0,4,5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,6,0,0,4,5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,7,1,5,5,2,2,1,0,0,6,2,2,2,5,5,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,3,3,3,8,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,1,9,9,9,1,0,0,0,0,0,1,10,10,10,10,10,10,10,10,10,10,7,0,0,0,0,0,0,8,11,12,13,14,15,3,0,0,0,0,0,0,0,0,0,9,16,16,16,16,11,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,9,16,16,16,16,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,12,14,16,2,11,12,14,17,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,12,14,16,2,11,12,14,17,5,0,0,0,0,0,0,0,0,0,0,0,0,0,3,2,11,18,17,17,18,19,10,1,0,0,0,0,0,0,0,0,0,0,0,0,3,11,16,14,13,13,13,13,20,4,0,19,13,13,13,13,14,12,19,1,0,0,0,0,0,0,0,0,0,0,0,7,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,9,16,16,16,16,11,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,6,18,16,16,16,17,4,0,0,0,0,0,9,16,16,16,16,11,0,0,0,0,0,0,0,0,3,17,21,21,21,17,7,0,0,0,0,18,22,22,22,22,22,22,22,22,22,21,10,0,0,0,0,0,0,10,13,23,24,25,13,10,0,0,0,0,0,0,0,0,0,19,26,23,23,23,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,26,23,23,23,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,14,27,28,29,17,14,23,28,26,17,3,0,0,0,0,0,0,0,0,0,0,0,0,5,14,27,28,29,17,14,23,28,26,17,3,
                                        0,0,0,0,0,0,0,0,0,0,0,9,17,13,30,29,27,27,31,26,21,12,15,4,0,0,0,0,0,0,0,0,0,0,15,21,27,28,24,24,24,28,14,1,0,18,27,24,24,24,28,25,30,17,4,0,0,0,0,0,0,0,0,0,0,15,13,13,20,4,0,0,0,0,0,0,0,0,0,0,0,19,26,23,23,23,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,21,23,23,23,21,9,0,0,0,0,0,19,26,23,23,23,14,2,0,0,0,0,0,0,0,3,16,23,32,25,20,7,0,0,0,0,18,31,33,33,33,33,33,33,33,33,22,11,0,0,0,0,0,0,18,31,
                                        34,35,34,31,17,8,0,0,0,0,0,0,0,0,5,16,26,36,33,26,17,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,16,26,36,33,26,17,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,22,32,37,28,13,30,33,38,28,14,1,0,0,0,0,0,0,0,0,0,0,0,0,11,22,32,37,28,13,30,33,38,28,14,1,0,0,0,0,0,0,0,0,0,8,11,14,29,28,32,33,24,28,36,33,36,23,22,17,5,0,0,0,0,0,0,0,0,5,14,25,34,37,33,23,31,31,14,1,0,18,31,31,27,36,37,38,24,22,11,0,0,0,0,0,0,0,0,0,
                                        0,18,29,28,14,1,0,0,0,0,0,0,0,0,0,0,0,5,16,26,36,33,26,17,7,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,23,39,25,21,19,7,0,0,0,0,0,5,16,26,36,33,26,17,7,0,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,18,22,22,22,32,35,35,32,22,22,21,10,0,0,0,0,0,8,17,27,39,40,39,23,16,3,0,0,0,0,0,0,0,0,0,5,16,26,36,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,16,26,36,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,21,36,39,
                                        28,14,22,32,39,25,14,1,0,0,0,0,0,0,0,0,0,0,0,0,10,21,36,39,28,14,22,32,39,25,14,1,0,0,0,0,0,0,0,0,0,11,13,23,33,38,24,29,13,14,21,23,34,39,24,26,16,1,0,0,0,0,0,0,0,10,21,32,35,37,28,14,18,18,15,8,0,9,18,18,16,31,34,41,39,31,18,8,0,0,0,0,0,0,0,0,0,18,29,28,14,1,0,0,0,0,0,0,0,0,0,0,0,0,5,16,26,36,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,3,18,26,32,25,21,19,7,0,0,0,0,0,0,0,5,16,26,36,28,21,10,0,0,0,0,0,
                                        0,3,16,23,38,25,20,7,0,0,0,0,1,11,11,21,32,35,35,32,21,11,10,6,0,0,0,0,0,0,19,26,32,38,33,26,18,0,0,0,0,0,0,0,0,0,0,0,5,16,26,27,27,16,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,16,26,27,27,16,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,20,26,23,22,18,12,29,23,22,19,8,0,0,0,0,0,0,0,0,0,0,0,0,1,20,26,23,22,18,12,29,23,22,19,8,0,0,0,0,0,0,0,0,2,14,23,34,35,33,29,17,9,5,11,13,28,38,38,36,30,18,8,0,0,0,0,0,
                                        0,11,22,33,35,38,23,16,7,0,0,0,0,0,0,0,15,30,33,41,39,23,17,3,0,0,0,0,0,0,3,10,18,16,29,28,14,17,19,10,4,0,0,0,0,0,0,0,0,0,0,5,16,26,27,27,16,3,0,0,0,0,0,0,0,0,0,0,0,5,13,27,27,21,19,7,0,0,0,0,0,0,0,0,0,5,16,26,27,27,16,3,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,4,5,12,29,23,29,12,2,0,0,0,0,0,0,0,0,0,3,2,11,18,17,17,17,19,10,1,0,0,0,0,0,0,0,0,
                                        2,10,10,10,10,10,10,10,10,10,17,17,17,11,10,10,10,10,10,10,5,0,0,0,0,0,0,0,0,2,18,16,18,17,17,17,16,15,1,0,0,0,0,0,0,0,0,2,10,10,10,10,10,10,18,16,19,5,9,17,16,15,10,10,10,10,10,5,0,0,0,8,18,26,33,35,38,28,14,2,0,0,8,18,26,33,41,38,25,14,2,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,4,18,13,29,23,25,24,23,27,26,21,20,10,0,0,1,10,10,10,10,10,10,10,10,17,17,17,11,10,10,10,10,7,0,1,
                                        10,10,10,10,10,10,18,17,17,15,10,10,10,10,10,10,7,0,1,10,10,10,10,17,17,17,11,10,9,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,7,17,13,13,13,13,13,13,17,7,0,0,0,0,0,0,0,9,17,13,30,29,27,27,31,26,21,12,15,4,0,0,0,0,0,6,20,22,22,22,22,22,22,22,22,22,17,3,9,13,22,22,22,22,22,22,17,8,0,0,0,0,0,0,9,17,13,30,29,27,27,31,26,21,12,15,4,0,0,0,0,0,6,20,22,22,22,22,22,22,22,22,22,17,3,9,13,22,22,22,
                                        22,22,22,17,8,0,0,5,14,28,38,40,39,27,17,3,0,0,0,10,21,36,35,35,33,26,19,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,3,18,22,28,34,28,23,28,23,24,33,24,29,19,0,0,18,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21,10,0,18,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21,10,0,18,22,22,22,22,22,22,22,22,22,14,5,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,4,12,25,24,
                                        24,24,24,25,12,6,0,0,0,0,0,8,11,14,29,28,32,33,24,28,36,33,36,23,22,17,5,0,0,0,0,4,12,25,33,33,33,33,33,33,33,23,16,3,10,21,32,33,33,33,33,27,17,8,0,0,0,0,8,11,14,29,28,32,33,24,28,36,33,36,23,22,17,5,0,0,0,0,4,12,25,33,33,33,33,33,33,33,23,16,3,10,21,32,33,33,33,33,27,17,8,0,0,11,22,32,35,41,34,26,19,0,0,0,0,1,14,28,37,40,39,23,20,6,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,9,13,28,38,
                                        28,21,29,28,14,21,25,34,29,19,0,0,18,31,33,33,33,33,33,33,33,33,33,33,33,33,33,33,22,11,0,18,31,33,33,33,33,33,33,33,33,33,33,33,33,33,33,22,11,0,18,31,33,33,33,33,33,33,33,24,13,5,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,4,12,31,31,32,37,38,25,12,6,0,0,0,0,0,11,13,23,33,38,24,29,13,14,21,23,34,39,24,26,16,1,0,0,0,6,20,22,22,23,38,35,39,27,22,22,17,3,9,21,22,22,36,31,22,22,17,8,0,0,0,0,11,13,23,33,38,24,
                                        29,13,14,21,23,34,39,24,26,16,1,0,0,0,6,20,22,22,23,38,35,39,27,22,22,17,3,9,21,22,22,36,31,22,22,17,8,0,0,18,29,34,41,35,33,22,11,0,0,0,0,6,20,25,38,40,37,28,14,1,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,19,26,33,39,27,17,29,28,14,19,22,32,29,19,0,0,18,22,22,29,34,35,37,28,29,29,29,29,29,29,29,33,22,11,0,18,22,22,29,34,35,37,28,29,29,29,29,29,29,29,33,22,11,0,18,22,22,29,34,35,37,28,22,22,14,5,0,0,
                                        0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,11,18,21,32,35,38,25,12,6,0,0,0,0,2,14,23,34,35,33,29,17,9,5,11,13,28,38,38,36,30,18,8,0,0,0,9,11,20,23,38,40,39,27,17,11,5,0,3,10,11,21,36,31,18,11,5,0,0,0,0,2,14,23,34,35,33,29,17,9,5,11,13,28,38,38,36,30,18,8,0,0,0,9,11,20,23,38,40,39,27,17,11,5,0,3,10,11,21,36,31,18,11,5,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,35,24,13,2,0,0,0,0,
                                        0,15,26,33,41,39,27,17,3,0,0,0,0,0,0,0,11,22,33,35,38,23,20,6,0,0,0,8,17,27,39,38,25,14,29,28,14,5,14,23,29,19,0,0,1,11,19,29,34,41,37,28,14,18,18,18,18,18,30,33,22,11,0,1,11,19,29,34,41,37,28,14,18,18,18,18,18,30,33,22,11,0,1,11,19,29,34,41,37,28,14,11,10,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,8,18,26,33,35,38,28,14,2,0,0,8,18,26,33,41,38,25,14,2,0,0,0,
                                        0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,8,18,26,33,35,38,28,14,2,0,0,8,18,26,33,41,38,25,14,2,0,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,3,17,27,39,40,35,32,21,9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,0,5,10,11,16,27,39,35,34,29,18,0,0,0,0,0,0,0,0,9,21,24,37,37,28,13,15,10,10,8,8,17,27,39,35,33,23,29,28,14,19,18,16,16,9,0,0,0,0,19,29,34,41,37,28,14,4,0,1,2,11,30,36,22,11,0,0,
                                        0,19,29,34,41,37,28,14,4,0,1,2,11,30,36,22,11,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,5,14,28,38,40,39,27,17,3,0,0,0,10,21,36,35,35,33,26,19,0,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,5,14,28,38,40,39,27,17,3,0,0,0,10,21,36,35,35,33,26,19,0,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,
                                        31,18,0,0,0,0,8,17,27,39,40,35,32,21,9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,3,17,22,22,31,36,38,34,28,21,10,0,0,0,0,0,0,0,0,4,20,27,32,39,33,23,30,22,13,5,0,19,26,33,35,35,39,33,24,23,26,13,17,5,0,0,0,0,0,19,29,34,41,37,28,14,4,4,20,13,13,13,21,13,10,0,0,0,19,29,34,41,37,28,14,4,4,20,13,13,13,21,13,10,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,
                                        32,35,38,25,12,6,0,0,0,11,22,32,35,41,34,26,19,0,0,0,0,1,14,28,37,40,39,23,20,6,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,11,22,32,35,41,34,26,19,0,0,0,0,1,14,28,37,40,39,23,20,6,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,37,24,13,2,0,3,16,23,33,34,38,32,29,13,19,7,0,0,0,0,0,0,0,0,0,10,12,30,25,38,39,33,24,13,2,0,9,13,28,39,
                                        35,40,41,37,39,33,28,26,20,2,0,0,0,0,19,29,34,41,37,28,14,4,1,14,28,23,16,9,9,8,0,0,0,19,29,34,41,37,28,14,4,1,14,28,23,16,9,9,8,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,18,29,34,41,35,33,22,11,0,0,0,0,6,20,25,38,40,37,28,14,1,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,18,29,34,41,35,33,22,11,
                                        0,0,0,0,6,20,25,38,40,37,28,14,1,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,19,26,34,41,35,33,22,11,0,0,0,0,6,12,25,38,40,37,28,14,1,0,3,16,30,30,31,36,38,33,25,13,10,0,0,0,0,0,0,0,0,4,20,31,32,39,34,23,26,30,13,2,0,3,18,22,25,33,38,37,41,40,35,38,32,29,17,7,0,0,0,19,29,34,41,37,28,14,20,20,14,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,20,20,14,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,
                                        0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,35,24,13,2,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,35,24,13,2,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,10,21,36,35,41,34,29,19,0,0,0,0,5,14,28,37,40,39,27,16,7,0,0,2,11,
                                        15,16,27,39,35,34,29,18,0,0,0,0,0,0,0,0,9,21,24,37,37,28,13,19,11,10,8,0,0,6,19,13,26,25,24,39,39,37,40,38,28,13,2,0,0,0,19,29,34,41,37,28,25,25,25,25,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,25,25,25,25,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,3,17,27,39,40,35,32,21,9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,0,
                                        0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,3,17,27,39,40,35,32,21,9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,1,14,25,38,40,39,27,16,7,0,0,0,10,21,36,35,35,32,30,15,0,0,0,0,0,0,15,26,33,41,39,27,17,3,0,0,0,0,0,0,0,11,22,33,35,38,23,20,6,0,0,0,0,1,11,11,10,18,12,29,28,23,36,38,35,32,22,10,0,0,0,19,29,34,41,37,36,24,24,24,24,36,23,16,3,0,0,0,0,
                                        0,19,29,34,41,37,36,24,24,24,24,36,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,8,17,27,39,40,35,32,21,9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,8,17,27,39,40,35,32,21,9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,
                                        31,18,0,0,0,0,0,0,19,30,32,37,37,28,13,10,0,0,6,17,31,34,41,39,23,14,5,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,19,30,30,14,1,18,29,28,14,26,32,35,33,22,11,0,0,0,19,29,34,41,37,28,13,13,13,13,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,13,13,13,13,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,
                                        32,35,38,25,12,6,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,37,24,13,2,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,37,24,13,2,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,5,12,27,33,37,34,31,12,11,10,18,21,28,38,39,28,21,15,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,19,29,36,21,
                                        10,18,29,28,14,14,28,37,32,22,10,0,0,0,19,29,34,41,37,28,14,5,5,14,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,5,5,14,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,19,26,34,41,35,33,22,11,0,0,0,0,6,12,25,38,40,37,28,14,1,0,0,0,7,16,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,19,26,34,41,35,33,22,11,
                                        0,0,0,0,6,12,25,38,40,37,28,14,1,0,0,0,7,16,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,0,10,14,31,36,39,32,27,22,21,26,28,39,34,28,22,18,6,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,19,29,34,31,16,18,29,28,14,13,28,38,28,13,2,0,0,0,19,29,34,41,37,28,14,4,1,14,30,30,17,3,0,0,0,0,0,19,29,34,41,37,28,14,4,1,14,30,30,17,3,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,
                                        0,3,16,23,38,25,20,7,0,0,0,0,0,0,9,21,32,35,35,32,21,10,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,10,21,36,35,41,34,29,19,0,0,0,0,5,14,28,37,40,39,27,16,7,0,0,0,8,17,27,39,40,38,23,20,7,0,0,0,0,10,22,32,26,19,0,0,0,0,0,10,21,36,35,41,34,29,19,0,0,0,0,5,14,28,37,40,39,27,16,7,0,0,0,8,17,27,39,40,38,23,20,7,0,0,0,0,10,22,32,26,19,0,0,0,0,0,0,0,0,9,20,22,23,24,32,32,32,33,36,28,31,13,18,4,0,0,0,0,0,0,0,
                                        0,11,30,33,41,38,23,16,3,0,0,0,0,0,0,0,11,30,33,41,38,23,16,3,0,0,0,0,19,29,34,36,31,21,29,28,21,27,33,32,26,17,3,0,0,0,19,29,34,41,37,28,14,4,0,9,11,11,10,15,15,1,0,0,0,19,29,34,41,37,28,14,4,0,9,11,11,10,15,15,1,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,1,18,18,18,1,10,21,32,35,35,36,21,9,0,0,0,0,0,7,7,3,10,21,32,35,38,25,12,6,0,0,0,1,14,25,38,40,39,27,16,7,0,0,0,10,21,36,35,35,32,30,15,0,0,0,
                                        0,0,18,29,34,41,37,28,14,1,0,0,0,0,19,26,33,22,11,0,0,0,0,0,1,14,25,38,40,39,27,16,7,0,0,0,10,21,36,35,35,32,30,15,0,0,0,0,0,18,29,34,41,37,28,14,1,0,0,0,0,19,26,33,22,11,0,0,0,0,0,0,0,0,0,1,15,20,13,22,26,25,32,29,14,17,18,18,5,0,0,0,0,0,0,0,0,11,22,33,35,38,23,16,7,0,0,0,0,0,0,0,15,30,33,41,39,23,17,3,0,0,0,0,19,30,27,28,32,36,36,32,32,32,28,26,20,5,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,9,13,30,30,19,0,0,
                                        0,19,29,34,41,37,28,14,4,0,0,0,9,13,30,30,19,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,10,22,31,22,11,11,22,33,35,37,28,13,5,0,0,0,0,5,16,16,17,10,21,32,35,38,25,12,6,0,0,0,0,19,30,32,37,37,28,13,10,0,0,6,17,31,34,41,39,23,14,5,0,0,0,0,0,11,22,32,35,35,32,22,15,6,0,8,5,12,23,24,13,9,0,0,0,0,0,0,19,30,32,37,37,28,13,10,0,0,6,17,31,34,41,39,23,14,5,0,0,0,0,0,11,22,32,35,35,32,22,15,6,0,8,5,12,23,24,
                                        13,9,0,0,0,0,0,0,0,0,0,0,0,4,9,11,19,30,32,33,25,27,31,30,11,0,0,0,0,0,0,0,0,10,21,32,35,38,28,14,19,19,11,8,0,2,19,19,17,31,34,41,39,31,17,8,0,0,0,0,1,15,16,14,21,30,29,28,26,22,14,18,5,0,0,0,0,0,19,29,34,41,37,28,14,4,7,7,7,9,21,36,29,19,0,0,0,19,29,34,41,37,28,14,4,7,7,7,9,21,36,29,19,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,3,16,23,38,25,20,7,0,10,22,32,29,18,18,29,34,41,39,23,20,6,0,0,0,0,10,22,23,21,10,22,
                                        32,35,38,23,20,7,0,0,0,0,5,12,27,33,37,34,31,12,11,10,18,21,28,38,39,28,21,15,0,0,0,0,0,0,5,14,25,39,41,39,25,21,16,18,17,12,26,32,27,16,6,0,0,0,0,0,0,5,12,27,33,37,34,31,12,11,10,18,21,28,38,39,28,21,15,0,0,0,0,0,0,5,14,25,39,41,39,25,21,16,18,17,12,26,32,27,16,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,27,33,38,39,33,30,11,0,0,0,0,0,0,0,0,5,14,25,39,37,33,23,29,29,14,1,0,18,29,29,31,24,38,38,36,22,15,0,0,0,0,0,0,0,7,5,
                                        10,18,29,28,14,10,5,8,0,0,0,0,9,18,18,29,34,41,37,28,14,16,16,16,16,16,21,36,29,19,0,9,18,18,29,34,41,37,28,14,16,16,16,16,16,21,36,29,19,0,9,18,18,29,34,41,37,28,14,18,15,3,0,0,0,3,16,23,38,25,20,7,0,10,22,32,28,26,26,28,38,39,28,21,15,0,0,0,0,0,10,22,32,30,14,29,34,37,33,26,18,0,0,0,0,0,0,10,14,27,32,39,32,27,22,21,26,28,39,34,28,22,18,6,0,0,0,0,0,0,0,19,22,28,39,37,34,24,23,31,27,25,32,25,21,10,0,0,0,0,0,0,0,0,10,14,27,32,39,32,
                                        27,22,21,26,28,39,34,28,22,18,6,0,0,0,0,0,0,0,19,22,28,39,37,34,24,23,31,27,25,32,25,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,14,31,24,33,33,30,11,0,0,0,0,0,0,0,0,8,19,21,23,28,24,36,36,28,14,1,0,18,27,36,36,36,24,25,26,16,1,0,0,0,0,0,0,0,0,0,0,18,29,28,14,1,0,0,0,0,0,0,18,31,31,31,34,35,37,28,23,23,23,23,23,23,23,36,29,19,0,18,31,31,31,34,35,37,28,23,23,23,23,23,23,23,36,29,19,0,18,31,31,31,34,35,37,28,31,31,13,5,0,0,
                                        0,3,16,23,38,25,20,7,0,10,21,23,28,32,32,32,24,23,22,18,6,0,0,0,0,0,10,22,33,24,28,36,34,36,31,12,2,0,0,0,0,0,0,0,10,20,30,23,24,33,32,32,33,32,28,31,13,19,4,0,0,0,0,0,0,0,0,4,18,21,23,24,33,33,34,33,32,28,27,21,19,3,0,0,0,0,0,0,0,0,0,10,20,30,23,24,33,32,32,33,32,28,31,13,19,4,0,0,0,0,0,0,0,0,4,18,21,23,24,33,33,34,33,32,28,27,21,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,16,21,26,31,30,11,0,0,0,0,0,0,0,
                                        0,0,7,15,20,13,13,21,21,21,12,1,0,19,21,21,21,21,13,14,18,5,0,0,0,0,0,0,0,0,0,0,0,18,29,23,14,1,0,0,0,0,0,0,18,31,24,24,24,24,24,24,24,24,24,24,24,24,24,24,29,19,0,18,31,24,24,24,24,24,24,24,24,24,24,24,24,24,24,29,19,0,18,31,24,24,24,24,24,24,24,24,13,5,0,0,0,3,16,23,38,25,20,7,0,6,15,20,13,21,22,21,13,20,15,4,0,0,0,0,0,0,10,13,30,29,27,31,29,21,20,9,0,0,0,0,0,0,0,0,0,1,19,12,21,22,26,26,30,21,14,17,10,3,0,0,0,0,0,0,
                                        0,0,0,0,6,15,20,13,22,26,26,26,22,13,16,10,3,0,0,0,0,0,0,0,0,0,0,0,1,19,12,21,22,26,26,30,21,14,17,10,3,0,0,0,0,0,0,0,0,0,0,6,15,20,13,22,26,26,26,22,13,16,10,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,10,19,18,18,1,0,0,0,0,0,0,0,0,0,0,0,6,5,2,9,9,9,5,0,0,6,9,9,9,9,5,1,8,0,0,0,0,0,0,0,0,0,0,0,0,10,16,16,18,7,0,0,0,0,0,0,19,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,15,0,19,
                                        13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,15,0,19,13,13,13,13,13,13,13,13,13,20,1,0,0,0,3,16,23,28,25,20,7,0,0,0,6,5,10,10,10,2,4,0,0,0,0,0,0,0,0,8,9,11,18,17,18,19,10,4,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,15,10,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,15,10,2,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,15,10,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,15,10,2,6,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,8,0,0,0,0,0,0,0,7,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,0,7,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,0,7,5,5,5,5,5,5,5,5,5,1,0,0,0,0,8,18,14,14,14,18,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,1,1,1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,6,0,0,4,5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,5,1,0,0,3,2,6,0,0,0,0,0,0,0,0,0,0,4,2,2,2,2,2,2,2,7,0,0,0,1,2,2,2,2,2,2,5,8,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,5,11,18,17,17,18,19,10,5,3,0,0,0,0,0,0,0,0,0,0,9,16,16,16,16,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,14,16,2,11,12,14,17,5,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,5,18,14,20,4,0,15,13,16,10,6,0,0,0,0,0,0,0,7,17,13,13,13,13,13,13,13,19,0,0,1,12,13,13,13,13,13,13,13,10,0,0,0,0,0,0,0,0,4,9,15,18,17,17,18,15,9,6,0,0,0,0,0,0,0,9,16,16,16,16,16,16,16,16,16,16,16,16,19,8,0,0,0,0,0,0,0,2,10,10,10,5,0,0,1,10,10,10,9,0,0,0,0,0,0,0,0,0,0,0,3,9,19,18,17,18,19,10,2,8,0,0,0,6,9,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,7,9,4,0,0,0,5,10,10,10,10,10,1,0,0,0,0,0,0,0,0,0,0,0,0,0,2,18,14,22,29,27,27,31,26,21,14,17,10,4,0,0,0,0,0,0,0,0,19,26,23,23,23,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,14,27,28,29,16,14,23,28,26,17,7,0,0,0,0,0,0,0,0,0,0,7,11,14,26,25,14,1,0,19,29,27,21,17,2,0,0,0,0,0,0,6,20,25,24,24,24,24,24,31,18,0,0,5,13,24,24,24,24,24,24,30,11,0,0,0,0,0,0,6,11,20,21,26,31,27,27,
                                        31,26,21,16,10,7,0,0,0,0,0,19,29,23,23,23,23,23,23,23,23,23,23,23,12,4,0,0,0,0,0,0,6,20,22,22,22,17,3,8,18,22,22,22,12,1,0,0,0,0,0,0,0,0,0,4,18,13,26,31,27,31,26,22,13,10,0,0,0,19,21,16,10,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,9,17,13,17,3,0,3,16,22,22,22,22,22,19,0,0,0,0,0,0,0,0,0,0,0,8,10,14,26,28,32,34,36,28,24,32,32,28,31,21,17,7,0,0,0,0,0,0,0,5,16,26,36,
                                        33,29,17,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,22,32,37,28,13,30,33,38,28,14,1,0,0,0,0,0,0,0,0,0,6,18,21,23,32,25,14,1,0,19,29,24,24,29,12,10,0,0,0,0,0,6,20,25,38,35,32,31,31,31,18,0,0,5,13,31,31,23,39,35,33,30,11,0,0,0,0,0,1,17,21,27,24,33,32,24,24,24,32,24,27,21,18,1,0,0,0,0,19,29,28,25,25,25,25,25,25,25,25,25,25,12,4,0,0,0,0,0,0,2,13,24,33,31,18,8,0,19,26,33,36,21,10,0,0,0,0,0,0,0,0,8,19,22,28,33,36,28,28,33,32,
                                        30,11,0,0,0,18,31,31,13,17,9,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,18,13,29,23,16,7,0,3,17,27,33,33,33,26,19,0,0,0,0,0,0,0,0,0,0,0,10,13,23,33,38,32,27,21,14,13,22,25,34,34,25,12,6,0,0,0,0,0,0,0,0,5,16,26,24,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,21,36,39,28,14,22,32,39,25,14,1,0,0,0,0,0,0,0,0,7,18,22,28,39,23,13,15,3,0,5,16,26,36,32,27,14,9,0,0,0,0,6,20,25,38,
                                        35,32,21,18,18,9,0,0,3,15,18,17,23,39,41,33,30,11,0,0,0,0,1,16,26,28,34,28,31,22,13,13,21,22,27,24,28,30,17,4,0,0,0,19,29,28,12,12,12,12,12,12,12,12,29,25,12,4,0,0,0,0,0,0,15,30,33,32,22,11,0,0,10,21,36,33,26,19,0,0,0,0,0,0,0,0,5,14,25,39,36,22,14,14,31,33,30,11,0,0,0,18,31,34,28,29,14,18,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,15,14,26,25,33,23,16,7,0,0,18,31,34,35,33,22,11,0,0,0,0,0,0,0,
                                        0,0,0,5,12,27,34,35,34,31,12,10,5,5,15,13,25,39,25,12,6,0,0,0,0,0,0,0,0,0,1,17,26,27,27,16,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,20,26,23,22,18,20,29,23,22,19,8,0,0,0,0,0,0,0,0,11,21,28,39,24,21,15,8,0,0,0,1,20,27,34,33,31,20,1,0,0,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,0,6,17,26,36,33,27,13,17,10,2,5,9,11,16,21,25,24,22,18,3,0,0,19,29,28,12,4,6,6,6,6,6,19,29,25,12,4,0,0,0,0,0,3,
                                        17,27,39,28,14,5,0,0,1,12,25,39,23,16,6,0,0,0,0,0,0,0,9,21,36,37,28,14,1,9,21,28,30,11,0,0,0,18,31,39,38,33,25,26,14,15,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,11,20,22,23,32,38,38,23,16,7,0,0,15,26,33,35,36,21,10,0,0,0,0,0,0,0,0,0,8,18,26,32,35,37,28,13,10,0,0,0,8,19,30,32,25,12,6,0,0,0,0,0,0,0,0,0,7,10,18,17,17,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,18,16,19,10,10,18,16,19,6,0,0,0,
                                        0,0,0,0,0,5,12,23,34,39,27,16,6,0,0,0,0,0,10,21,24,37,36,30,19,0,0,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,0,11,21,28,34,27,14,10,1,9,10,2,7,1,1,15,13,25,28,13,10,0,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,0,0,1,14,25,39,27,16,7,0,0,8,17,31,34,28,13,5,0,0,0,0,0,0,0,9,21,36,37,24,22,17,9,20,14,14,10,0,0,0,18,31,39,40,35,38,32,23,22,20,11,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,7,10,16,21,27,36,39,35,40,38,23,16,7,0,0,10,22,32,37,24,13,5,0,0,0,0,0,0,0,0,0,5,14,25,38,40,39,27,16,7,0,0,0,0,5,14,28,25,12,6,0,0,0,0,0,0,0,0,0,10,21,22,22,22,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,10,21,22,22,22,19,0,0,0,0,0,0,0,0,0,0,15,30,32,35,33,30,15,0,0,0,0,0,0,4,12,25,38,39,25,14,1,0,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,1,12,23,39,25,13,10,19,12,21,21,13,
                                        17,14,14,14,17,21,24,31,17,3,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,0,0,10,21,36,33,26,19,0,0,0,0,11,22,32,32,22,11,0,0,0,0,0,0,0,5,14,25,38,38,24,31,13,18,5,1,0,0,0,0,18,31,39,40,40,40,35,39,36,27,21,16,10,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,9,17,13,31,24,39,37,40,40,40,38,23,16,7,0,0,9,21,36,37,28,14,1,0,0,0,0,0,0,0,0,0,10,22,32,35,41,34,26,19,0,0,0,0,0,7,16,31,31,12,6,0,0,0,0,0,0,0,0,8,17,31,
                                        33,33,23,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,17,31,33,33,23,12,4,0,0,0,0,0,0,0,0,6,20,23,39,35,24,21,9,0,0,0,0,0,0,8,17,27,39,35,32,22,11,0,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,11,21,36,32,30,19,18,22,25,36,32,28,29,27,28,28,12,17,31,28,14,5,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,0,0,19,26,34,24,21,9,0,0,0,0,2,13,28,34,31,17,8,0,0,0,0,0,0,8,18,30,24,39,37,33,25,30,12,
                                        10,0,0,0,0,18,31,39,40,40,40,40,41,37,34,24,31,13,17,9,8,0,0,0,0,0,0,0,0,0,0,0,5,18,14,29,28,34,37,41,40,40,40,40,38,23,16,7,0,0,5,13,24,38,25,20,6,0,0,0,0,0,0,0,0,0,19,29,34,41,35,33,22,11,0,0,0,0,0,0,10,18,18,11,0,0,0,0,0,0,0,0,0,5,14,25,38,37,24,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,14,25,38,37,24,21,10,0,0,0,0,0,0,0,0,2,13,28,37,37,28,14,1,0,0,0,0,0,0,0,19,26,34,41,34,31,17,8,0,6,20,25,38,
                                        35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,8,17,31,34,28,14,11,21,28,39,28,31,31,28,27,39,28,12,10,22,32,21,10,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,0,6,20,23,39,25,12,1,0,0,0,0,6,20,23,39,25,12,1,0,0,0,0,0,0,4,17,22,23,36,33,38,38,32,27,14,10,0,0,0,18,31,39,40,40,40,40,40,40,41,37,34,28,29,14,18,5,0,0,0,0,0,0,0,0,1,15,14,26,25,33,38,41,40,40,40,40,40,40,38,23,16,7,0,0,1,14,28,39,27,17,3,0,0,0,0,0,0,0,
                                        0,8,17,27,39,40,35,32,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,22,32,35,41,34,29,18,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,22,32,35,41,34,29,18,8,0,0,0,0,0,0,0,10,22,32,35,38,25,12,4,0,0,0,0,0,0,0,11,30,33,41,38,25,20,6,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,4,12,25,39,27,17,16,27,39,36,22,17,17,22,36,37,28,12,5,13,24,30,15,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,0,2,13,
                                        28,34,31,17,8,0,0,0,0,0,19,26,34,24,21,9,0,0,0,0,0,0,15,22,24,32,26,29,28,34,37,34,23,14,5,0,0,18,31,39,40,40,40,40,40,40,40,40,41,38,33,25,26,12,15,1,0,0,0,0,6,11,20,22,23,32,38,35,40,40,40,40,40,40,40,40,38,23,16,7,0,0,6,20,25,34,31,18,0,0,0,0,0,0,0,0,0,3,17,27,39,40,35,32,21,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,16,27,34,38,40,38,25,14,5,0,0,0,0,0,0,0,0,0,0,0,0,0,7,16,27,34,38,40,38,25,14,5,0,0,0,
                                        0,0,0,0,15,26,33,41,38,23,20,7,0,0,0,0,0,0,0,10,22,32,35,37,28,14,1,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,5,13,28,34,26,19,13,28,38,25,12,1,1,12,25,38,28,12,1,14,28,26,15,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,0,11,22,32,32,22,11,0,0,0,0,0,0,10,21,36,33,26,19,0,0,0,0,0,6,20,23,39,28,14,17,21,23,33,37,32,30,15,0,0,18,31,39,40,40,40,40,40,40,40,40,40,40,35,38,32,23,22,20,10,0,0,9,16,21,27,36,
                                        39,35,40,40,40,40,40,40,40,40,40,40,38,23,16,7,0,0,3,17,27,33,26,15,0,0,0,0,0,0,0,0,0,8,17,27,39,40,35,32,21,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,13,28,23,33,35,35,32,22,11,0,0,0,0,0,0,0,0,0,0,0,0,0,2,13,28,23,33,35,35,32,22,11,0,0,0,0,0,0,0,19,29,34,41,38,23,16,3,0,0,0,0,0,0,0,10,21,32,35,37,24,13,5,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,9,13,24,33,30,11,21,36,39,27,17,8,8,
                                        17,27,39,28,12,5,14,28,26,15,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,8,17,31,34,28,13,5,0,0,0,0,0,0,1,14,25,39,27,16,7,0,0,0,0,1,14,28,37,28,14,9,11,14,27,34,39,31,18,0,0,18,31,39,40,40,40,40,40,40,40,40,40,40,40,40,35,39,36,27,16,7,0,18,31,24,34,37,41,40,40,40,40,40,40,40,40,40,40,40,38,23,16,7,0,0,0,18,31,32,22,10,0,0,0,0,0,0,0,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,26,33,30,
                                        28,37,41,39,27,17,7,0,0,0,0,0,0,0,0,0,0,0,0,19,26,33,30,28,37,41,39,27,17,7,0,0,0,0,0,0,19,29,34,41,38,23,16,3,0,0,0,0,0,0,0,10,21,32,35,37,24,13,5,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,9,21,36,33,22,10,21,36,39,27,17,8,8,17,27,39,28,12,9,21,36,22,11,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,1,12,25,39,23,16,6,0,0,0,0,0,0,3,17,27,39,28,14,5,0,0,0,0,4,12,25,38,33,31,14,15,10,21,32,
                                        39,31,18,0,0,18,31,39,40,40,40,40,40,40,40,40,40,40,35,38,32,23,22,20,10,0,0,9,16,21,27,36,39,35,40,40,40,40,40,40,40,40,40,40,38,23,16,7,0,0,0,19,29,32,21,10,0,0,0,0,0,0,0,0,0,0,19,26,34,41,35,33,30,11,0,0,0,0,0,0,9,10,10,10,7,0,0,0,0,0,0,4,20,23,28,13,27,39,41,37,28,13,2,0,0,0,0,0,0,0,0,0,0,0,4,20,23,28,13,27,39,41,37,28,13,2,0,0,0,0,0,0,19,29,34,41,38,23,16,7,0,0,0,0,0,0,0,10,21,32,35,37,28,13,5,0,6,20,25,38,
                                        35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,2,13,24,33,30,11,21,36,39,27,16,3,3,17,27,39,28,12,18,26,24,13,9,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,10,21,36,33,26,19,0,0,0,0,0,0,0,0,15,30,33,32,22,11,0,0,0,0,3,17,31,34,37,33,23,21,16,30,33,32,30,15,0,0,18,31,39,40,40,40,40,40,40,40,40,41,38,33,25,30,12,15,1,0,0,0,0,6,11,20,22,23,32,38,35,40,40,40,40,40,40,40,40,38,23,16,7,0,0,0,19,29,32,21,10,0,0,0,0,0,0,0,0,
                                        0,0,10,21,36,35,41,34,29,18,0,0,0,0,0,5,14,22,22,21,10,0,0,0,0,0,0,10,21,24,27,17,22,32,35,35,33,30,19,0,0,0,0,0,0,0,0,0,0,0,10,21,24,27,17,22,32,35,35,33,30,19,0,0,0,0,0,0,15,30,33,41,38,25,20,6,0,0,0,0,0,0,0,10,22,32,35,37,28,14,1,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,5,14,28,34,29,19,14,28,38,28,14,2,5,14,25,38,28,12,13,25,27,20,6,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,0,19,26,33,
                                        36,21,10,0,0,0,0,0,0,0,0,2,13,24,34,31,18,8,0,0,0,0,10,13,25,34,37,39,24,27,28,33,23,14,5,0,0,18,31,39,40,40,40,40,40,40,41,37,34,28,29,14,19,5,0,0,0,0,0,0,0,0,1,15,12,26,25,33,38,41,40,40,40,40,40,40,38,23,16,7,0,0,0,19,26,26,21,10,0,0,0,0,0,0,0,0,0,0,1,12,23,39,40,39,23,20,6,0,0,0,0,11,22,32,24,21,9,0,0,0,0,0,8,18,29,32,22,11,14,25,38,40,39,23,20,4,0,0,0,0,0,0,0,0,0,8,18,29,32,22,11,14,25,38,40,39,23,20,4,0,
                                        0,0,0,0,10,21,32,35,38,25,12,4,0,0,0,0,0,0,0,11,30,33,41,38,23,20,6,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,6,20,23,39,23,16,17,31,34,33,29,12,12,29,36,38,28,21,23,28,21,11,0,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,7,16,23,39,25,14,1,0,0,0,0,0,0,0,0,6,20,23,38,25,12,4,0,0,0,0,8,15,13,27,32,38,37,34,36,29,14,10,0,0,0,18,31,39,40,40,40,40,41,37,34,24,31,13,17,9,8,0,0,0,0,0,0,0,0,0,0,0,
                                        5,18,14,29,28,34,37,41,40,40,40,40,38,23,16,7,0,0,1,17,21,30,13,19,7,0,0,0,0,0,0,0,0,0,0,19,22,36,37,37,24,21,11,0,0,0,4,16,27,34,23,20,4,0,0,0,0,0,1,14,25,25,14,1,18,31,34,41,37,24,21,10,0,0,0,0,0,0,0,0,0,1,14,25,25,14,1,18,31,34,41,37,24,21,10,0,0,0,0,0,5,13,28,37,37,28,14,1,0,0,0,0,0,0,0,19,26,34,41,34,31,18,8,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,18,29,34,28,13,10,13,25,34,32,25,25,
                                        24,27,39,24,24,28,22,18,6,0,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,5,13,28,34,31,17,8,0,0,0,0,0,0,0,0,0,18,29,34,24,21,9,0,0,0,0,0,8,10,12,30,25,33,37,39,28,21,11,0,0,0,18,31,39,40,40,40,35,39,36,27,21,16,10,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,9,17,13,31,24,39,37,40,40,40,38,23,16,7,0,0,19,30,24,33,28,21,10,0,0,0,0,0,0,0,0,0,0,1,20,31,33,37,34,23,14,19,10,10,17,22,24,36,22,15,0,0,0,0,0,0,11,22,36,29,16,16,
                                        16,21,36,35,41,34,29,18,8,0,0,0,0,0,0,0,0,11,22,36,29,16,16,16,21,36,35,41,34,29,18,8,0,0,0,0,7,16,27,39,35,36,21,9,0,0,0,0,0,0,8,17,27,39,35,36,21,10,0,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,10,21,24,33,29,17,15,13,31,25,28,23,22,27,28,23,29,13,18,4,0,0,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,0,11,22,32,32,22,11,0,0,0,0,0,0,0,0,0,0,10,21,36,33,26,19,0,0,0,0,0,5,9,9,18,13,31,24,38,39,
                                        27,16,3,0,0,18,31,39,40,35,38,32,23,22,20,11,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,10,16,21,27,36,39,35,40,38,23,16,7,0,7,16,23,39,41,34,29,18,0,0,0,0,0,0,0,0,0,0,0,9,12,31,36,39,33,25,26,21,22,31,24,36,29,16,4,0,0,0,0,0,3,17,27,32,23,23,23,23,23,28,37,40,38,25,12,1,0,0,0,0,0,0,0,3,17,27,32,23,23,23,23,23,28,37,40,38,25,12,1,0,0,0,0,0,11,22,36,37,33,30,15,0,0,0,0,0,0,1,12,25,38,39,23,12,1,0,0,6,20,25,38,
                                        35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,6,16,31,34,28,21,15,10,17,12,14,20,19,14,14,20,18,9,3,0,0,0,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,8,18,31,34,28,13,2,0,0,0,0,0,0,0,0,0,0,1,14,28,39,27,16,7,0,0,0,4,20,21,21,19,9,16,26,33,38,25,12,6,0,0,18,31,39,38,33,25,26,14,15,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,11,20,22,25,32,38,38,23,16,7,0,7,16,23,39,41,34,29,18,0,0,0,0,0,0,0,
                                        0,0,0,0,0,9,16,22,23,24,32,33,32,32,36,25,30,20,2,0,0,0,0,0,0,2,13,28,25,25,25,25,25,25,25,33,41,35,36,21,10,0,0,0,0,0,0,0,2,13,28,25,25,25,25,25,25,25,33,41,35,36,21,10,0,0,0,0,0,1,20,27,34,39,23,20,4,0,0,0,0,0,11,21,36,37,24,22,15,0,0,0,6,20,25,38,35,32,21,10,0,0,0,0,0,0,3,17,23,39,41,33,30,11,0,0,0,10,13,25,39,25,13,19,1,4,1,6,3,1,2,17,12,9,0,0,0,0,0,19,29,28,12,4,0,0,0,0,0,19,29,25,12,4,0,4,12,25,39,23,
                                        20,6,0,0,0,0,0,0,0,0,0,0,3,17,27,39,28,14,5,0,0,0,1,14,28,27,17,2,10,22,32,38,25,20,6,0,0,18,31,34,28,29,13,18,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,19,14,26,25,33,23,16,7,0,0,19,30,24,34,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,1,15,20,13,22,26,24,29,21,14,18,5,0,0,0,0,0,0,0,15,30,33,26,12,12,12,12,12,13,24,37,41,34,31,17,3,0,0,0,0,0,0,15,30,33,26,12,12,12,12,12,13,24,37,41,34,31,17,
                                        3,0,0,0,0,0,10,13,25,39,36,22,19,3,0,0,0,5,12,27,34,32,29,16,4,0,0,0,6,20,25,38,35,32,21,10,10,4,0,0,0,9,10,17,23,39,41,33,30,11,0,0,0,8,19,21,28,34,25,30,12,18,11,10,15,17,14,29,31,17,7,0,0,0,0,19,29,28,12,5,5,5,5,5,5,19,29,25,12,4,0,9,21,24,34,26,19,0,0,0,0,0,0,0,0,0,0,0,0,15,30,33,32,22,10,0,0,0,1,14,28,24,30,13,21,27,34,33,29,18,8,0,0,18,31,31,13,17,9,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,5,18,13,29,23,16,7,0,0,1,16,22,26,21,18,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,15,11,16,27,24,21,9,8,0,0,0,0,0,0,0,6,20,23,28,13,9,4,4,4,4,20,23,39,40,38,28,14,5,0,0,0,0,0,6,20,23,28,13,9,4,4,4,4,20,23,39,40,38,28,14,5,0,0,0,0,0,3,19,21,25,34,25,21,19,7,0,9,20,29,32,32,31,12,2,0,0,0,0,6,20,25,38,35,32,22,22,22,18,0,0,5,14,22,22,23,39,41,33,30,11,0,0,0,0,6,18,21,25,33,32,25,29,30,22,
                                        30,27,28,36,27,14,1,0,0,0,0,19,29,28,13,13,13,13,13,13,13,13,29,25,12,4,0,19,26,33,36,21,10,0,0,0,0,0,0,0,0,0,0,0,0,9,13,24,33,29,18,8,0,0,1,14,28,36,32,24,36,33,24,31,14,9,0,0,0,19,21,16,10,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,9,17,13,17,3,0,0,0,4,11,15,10,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,16,30,13,13,27,33,30,15,0,0,0,0,0,0,0,10,18,13,24,27,16,18,4,0,
                                        0,9,18,30,33,35,35,32,30,18,18,4,0,0,10,18,13,24,27,16,18,4,0,0,9,18,30,33,35,35,32,30,18,18,4,0,0,0,0,7,19,13,27,32,25,14,1,0,19,29,32,28,26,20,9,0,0,0,0,0,6,20,25,33,33,33,32,32,31,18,0,0,5,13,24,32,32,33,33,33,30,11,0,0,0,0,0,7,15,14,26,25,36,33,33,33,32,24,23,22,20,10,0,0,0,0,0,19,29,36,24,24,24,24,24,24,24,24,24,25,12,4,8,17,30,30,30,14,1,0,0,0,0,0,0,0,0,0,0,0,0,4,12,30,30,30,16,3,0,0,6,17,14,21,30,26,26,30,21,16,
                                        9,0,0,0,0,6,9,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,9,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,16,23,24,28,33,32,22,11,0,0,0,0,0,0,7,20,31,31,33,31,31,21,10,0,0,18,31,31,28,37,35,39,27,31,22,10,0,7,20,31,31,33,31,31,21,10,0,0,18,31,31,28,37,35,39,27,31,22,10,0,0,0,0,0,3,10,12,30,23,14,1,0,19,29,31,13,18,5,0,0,0,0,0,0,6,20,30,30,
                                        30,30,30,30,30,18,0,0,5,14,30,30,30,30,30,30,22,11,0,0,0,0,0,0,0,2,18,14,21,30,26,26,22,13,20,15,1,0,0,0,0,0,0,19,26,29,29,29,29,29,29,29,29,29,29,29,12,4,0,5,15,15,15,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,15,15,15,2,0,0,0,0,7,5,10,11,15,19,15,9,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,3,16,27,25,28,25,29,12,1,0,0,0,0,0,0,7,20,25,24,24,24,24,21,10,0,0,18,31,24,24,24,24,24,24,24,22,10,0,7,20,25,24,24,24,24,21,10,0,0,18,31,24,24,24,24,24,24,24,22,10,0,0,0,0,0,0,0,1,19,12,16,4,0,11,14,17,10,3,0,0,0,0,0,0,0,0,9,11,11,11,11,11,11,11,5,0,0,8,10,11,11,11,11,11,11,11,4,0,0,0,0,0,0,0,0,8,1,9,11,15,15,10,2,6,0,0,0,0,0,0,0,0,5,19,19,19,19,19,19,19,19,19,19,19,19,10,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,16,12,14,12,17,2,0,0,0,0,0,0,0,3,17,13,13,13,13,13,14,9,0,0,19,13,13,13,13,13,13,13,13,14,9,0,3,17,13,13,13,13,13,14,9,0,0,19,13,13,13,13,13,13,13,
                                        13,14,9,0,0,0,0,0,0,0,0,0,4,4,0,0,8,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,1,4,3,0,0,0,0,0,0,0,0,0,4,5,5,5,5,5,5,0,0,0,7,5,5,5,5,5,5,5,5,5,0,0,0,4,5,5,5,5,5,5,0,0,0,7,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,5,0,0,0,0,0,0,0,5,2,2,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,8,7,8,0,0,8,7,8,0,0,
                                        0,0,0,0,0,0,0,0,0,0,8,7,8,0,0,8,7,8,0,0,0,0,0,0,0,0,0,0,0,8,7,8,0,0,8,7,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,1,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,1,0,0,0,0,0,0,0,0,0,0,0,5,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,13,13,13,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,20,13,
                                        13,13,14,9,0,0,0,0,0,5,12,13,13,13,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,13,13,13,14,2,0,0,0,0,0,0,0,0,0,0,0,0,5,12,13,13,13,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,17,17,19,3,0,0,0,0,0,0,0,0,1,18,20,17,2,5,18,20,18,5,0,0,0,0,0,0,0,0,0,0,1,18,20,17,2,5,18,20,18,5,0,0,0,0,0,0,0,0,0,1,18,20,17,2,5,18,20,18,5,0,0,0,0,0,0,0,0,0,0,5,11,18,17,17,18,19,11,9,4,8,0,0,0,0,0,
                                        0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,0,0,0,0,0,0,0,0,1,12,14,9,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,27,24,24,29,17,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,19,30,24,24,23,14,9,0,0,0,0,0,5,12,27,24,24,29,17,7,0,0,0,0,0,0,0,0,0,0,0,0,5,12,27,24,24,29,17,7,0,0,0,0,0,0,0,0,0,0,0,5,12,27,24,24,29,17,7,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,19,29,27,14,1,0,0,0,0,0,0,0,7,17,30,23,29,20,17,26,25,26,16,4,0,0,0,0,0,0,0,0,7,17,30,23,29,20,17,26,25,26,16,4,0,0,0,0,0,0,0,7,17,30,23,29,20,17,26,25,26,16,4,0,0,0,0,0,0,0,6,15,14,22,29,27,27,31,29,22,13,12,18,5,0,0,0,0,0,18,27,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,23,16,3,0,15,30,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,29,17,8,0,0,0,0,0,0,0,0,0,11,22,26,18,8,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,10,14,23,34,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,13,25,39,25,13,11,0,0,0,0,0,0,0,10,14,23,34,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,10,14,23,34,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,10,14,23,34,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,28,14,1,0,0,0,0,0,0,0,5,13,28,38,36,21,13,24,38,24,21,9,0,0,0,0,0,0,0,0,5,13,28,38,36,21,13,24,38,24,21,9,0,0,0,0,0,0,0,5,13,28,38,36,
                                        21,13,24,38,24,21,9,0,0,0,0,0,0,7,18,21,25,32,32,24,28,24,32,32,24,25,26,15,0,0,0,0,0,18,27,39,35,35,35,35,35,35,35,35,35,35,35,35,35,35,38,23,16,3,0,5,14,25,38,35,35,35,35,35,35,35,35,35,35,35,35,37,28,13,10,0,0,0,0,0,0,0,0,0,4,20,27,25,14,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,13,23,34,27,12,1,0,0,0,0,0,0,0,0,0,0,0,0,6,16,31,33,25,21,15,8,0,0,0,0,0,0,0,0,10,13,23,34,27,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,10,
                                        13,23,34,27,12,1,0,0,0,0,0,0,0,0,0,0,0,0,10,13,23,34,27,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,4,9,19,29,28,14,2,6,0,0,0,0,0,0,5,13,28,38,32,22,21,24,37,36,21,10,0,0,0,0,0,0,0,0,5,13,28,38,32,22,21,24,37,36,21,10,0,0,0,0,0,0,0,5,13,28,38,32,22,21,24,37,36,21,10,0,0,0,0,0,0,10,21,28,39,28,30,13,14,13,30,25,34,33,26,15,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,8,18,26,33,35,40,40,40,40,
                                        40,40,40,40,40,40,41,34,27,17,7,0,0,0,0,0,0,0,0,0,10,21,24,32,30,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,11,13,25,32,30,19,8,0,0,0,0,0,0,0,0,0,0,0,15,21,24,28,21,19,3,0,0,0,0,0,0,0,0,0,8,11,13,25,32,30,19,8,0,0,0,0,0,0,0,0,0,0,0,0,8,11,13,25,32,30,19,8,0,0,0,0,0,0,0,0,0,0,0,8,11,13,25,32,30,19,8,0,0,0,0,0,0,0,0,0,0,6,15,20,21,22,29,28,22,13,20,19,2,0,0,0,0,6,17,29,28,27,12,20,31,28,31,20,1,
                                        0,0,0,0,0,0,0,0,6,17,29,28,27,12,20,31,28,31,20,1,0,0,0,0,0,0,0,6,17,29,28,27,12,20,31,28,31,20,1,0,0,0,0,0,7,16,27,34,33,26,18,2,5,2,15,13,25,33,26,15,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,2,13,28,38,40,40,40,40,40,40,40,40,40,40,37,24,21,10,0,0,0,0,0,0,0,0,0,7,17,31,34,39,23,12,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,19,21,27,27,14,1,0,0,0,0,0,0,0,0,0,0,6,20,27,27,22,18,
                                        6,0,0,0,0,0,0,0,0,0,0,0,3,19,21,27,27,14,1,0,0,0,0,0,0,0,0,0,0,0,0,0,3,19,21,27,27,14,1,0,0,0,0,0,0,0,0,0,0,0,0,3,19,21,27,27,14,1,0,0,0,0,0,0,0,0,0,4,18,21,23,24,33,32,32,32,24,23,26,17,8,0,0,0,0,5,17,14,20,10,9,16,14,16,9,0,0,0,0,0,0,0,0,0,0,5,17,14,20,10,9,16,14,16,9,0,0,0,0,0,0,0,0,0,5,17,14,20,10,9,16,14,16,9,0,0,0,0,0,0,5,14,28,37,32,22,10,0,0,0,8,18,26,33,26,15,0,0,0,0,
                                        0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,3,17,29,34,41,40,40,40,40,40,40,40,40,41,39,23,20,4,0,0,0,0,0,0,0,0,0,9,13,28,37,37,36,22,11,0,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,19,17,17,19,3,0,0,0,0,0,0,0,0,0,0,4,15,17,17,18,10,1,0,0,0,0,0,0,0,0,0,7,5,10,11,19,17,17,19,4,0,0,0,0,0,0,0,0,0,0,0,0,6,9,11,19,17,17,19,6,0,0,0,0,0,0,0,1,5,5,5,5,5,5,19,17,17,19,5,5,5,5,8,0,0,
                                        0,0,8,19,22,28,39,39,25,29,28,22,25,39,27,17,8,0,0,0,0,7,5,10,11,15,15,11,9,4,0,0,0,0,0,0,0,0,0,0,0,0,6,9,11,15,15,10,2,6,0,0,0,0,0,0,0,1,5,5,5,5,5,5,7,1,5,5,5,5,5,5,8,0,0,0,9,13,24,35,33,26,18,9,6,0,0,10,21,27,26,15,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,10,13,24,37,40,40,40,40,40,40,40,40,35,32,22,15,0,0,0,0,0,0,0,0,0,8,18,29,33,35,41,39,27,16,6,0,0,0,0,
                                        0,0,0,0,0,0,7,11,20,21,30,26,30,21,12,15,6,0,0,0,0,0,0,0,7,11,20,21,30,26,30,21,12,15,6,0,0,0,0,0,0,11,16,14,21,30,26,26,22,13,20,11,7,0,0,0,0,0,0,0,0,3,10,16,13,22,26,26,22,13,16,10,8,0,0,0,0,5,12,13,13,13,13,13,13,19,20,13,13,13,13,13,13,11,0,0,0,0,2,14,25,39,37,36,21,29,28,14,21,24,27,17,8,0,0,0,11,16,14,21,30,26,26,22,13,20,11,7,0,0,0,0,0,0,0,0,3,10,16,13,22,26,26,22,13,16,10,8,0,0,0,0,5,12,13,13,13,13,13,13,
                                        19,20,13,13,13,13,13,13,11,0,0,0,9,21,36,35,38,28,26,13,20,19,10,1,18,17,18,5,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,6,16,27,34,41,40,40,40,40,40,40,40,39,25,12,1,0,0,0,0,0,0,0,0,0,2,14,28,38,40,40,37,24,21,10,0,0,0,0,0,0,0,0,0,6,18,21,23,24,33,36,33,36,23,21,18,6,0,0,0,0,0,6,18,21,23,24,33,36,33,36,23,21,18,6,0,0,0,0,6,20,27,28,32,36,36,32,32,24,23,21,19,3,0,0,0,0,0,0,3,19,13,27,
                                        24,32,36,36,32,28,31,13,15,8,0,0,0,2,13,24,24,24,24,24,27,18,14,28,24,24,24,24,26,19,0,0,0,0,19,26,33,35,38,25,12,29,28,14,12,25,27,17,8,0,0,6,20,27,28,32,36,36,32,32,24,23,21,19,3,0,0,0,0,0,0,3,19,13,27,24,32,36,36,32,28,31,13,15,8,0,0,0,2,13,24,24,24,24,24,27,18,14,28,24,24,24,24,26,19,0,0,0,5,13,24,37,41,38,33,28,23,26,21,14,18,9,8,0,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,10,21,36,37,40,40,
                                        40,40,40,40,35,33,26,19,0,0,0,0,0,0,0,0,0,0,19,26,33,35,40,40,41,34,31,17,7,0,0,0,0,0,0,0,0,15,21,28,39,33,31,21,26,32,39,28,21,15,0,0,0,0,0,15,21,28,39,33,31,21,26,32,39,28,21,15,0,0,0,0,6,20,25,33,31,21,13,30,28,38,39,25,13,10,0,0,0,0,0,0,11,13,25,34,34,23,21,21,23,39,34,25,13,10,0,0,0,2,13,31,31,34,37,39,27,18,14,31,27,39,37,34,26,19,0,0,0,3,17,27,39,40,39,23,16,29,28,14,17,30,30,17,8,0,0,6,20,25,33,31,21,13,30,28,38,39,25,13,
                                        10,0,0,0,0,0,0,11,13,25,34,34,23,21,21,23,39,34,25,13,10,0,0,0,2,13,31,31,34,37,39,27,18,14,31,27,39,37,34,26,19,0,0,0,4,20,23,39,41,40,35,37,39,34,36,25,29,14,15,3,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,4,20,23,39,40,40,40,40,40,40,38,25,14,5,0,0,0,0,0,0,0,0,0,1,12,25,38,40,40,40,40,37,28,13,9,0,0,0,0,0,0,0,1,12,23,39,37,24,13,9,14,25,38,39,23,12,1,0,0,0,1,12,23,39,37,24,13,9,14,25,
                                        38,39,23,12,1,0,0,0,6,20,25,25,14,10,9,18,30,33,35,34,29,18,8,0,0,0,0,1,12,23,34,35,36,21,11,15,22,32,35,34,27,20,4,0,0,6,19,18,26,34,41,39,27,18,15,18,27,39,41,34,26,19,0,0,0,6,20,25,38,40,39,27,17,29,28,14,2,11,11,5,0,0,0,6,20,25,25,14,10,9,18,30,33,35,34,29,18,8,0,0,0,0,1,12,23,34,35,36,21,11,15,22,32,35,34,27,20,4,0,0,6,19,18,26,34,41,39,27,18,15,18,27,39,41,34,26,19,0,0,0,0,19,22,24,38,35,40,40,40,41,35,38,33,25,13,11,0,0,0,0,
                                        0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,15,22,32,35,40,40,40,40,35,33,29,18,8,0,0,0,0,0,0,0,0,0,15,22,32,35,40,40,40,40,35,33,29,18,8,0,0,0,0,0,0,11,22,32,35,37,28,14,1,16,23,39,35,32,22,11,0,0,0,11,22,32,35,37,28,14,1,16,23,39,35,32,22,11,0,0,0,6,20,30,30,18,9,9,10,21,36,35,38,23,20,6,0,0,0,0,10,21,36,37,37,28,14,5,2,13,24,37,37,24,21,10,0,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,
                                        0,4,12,25,38,40,39,27,18,29,28,14,1,0,0,0,0,0,0,6,20,30,30,18,9,9,10,21,36,35,38,23,20,6,0,0,0,0,10,21,36,37,37,28,14,5,2,13,24,37,37,24,21,10,0,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,1,17,30,25,33,39,37,35,41,40,40,35,34,23,20,4,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,1,12,25,38,40,40,40,40,37,28,13,9,0,0,0,0,0,0,0,0,0,4,20,23,39,40,40,40,40,40,40,38,25,14,5,0,0,
                                        0,0,0,0,18,29,34,41,38,25,20,15,17,27,39,41,34,29,18,0,0,0,18,29,34,41,38,25,20,15,17,27,39,41,34,29,18,0,0,0,0,9,11,17,14,21,21,21,21,36,35,37,28,12,4,0,0,0,0,18,29,34,41,38,25,12,4,1,14,28,37,41,34,26,19,0,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,6,20,25,38,40,39,27,17,29,28,14,1,9,9,5,0,0,0,0,9,11,17,14,21,21,21,21,36,35,37,28,12,4,0,0,0,0,18,29,34,41,38,25,12,4,1,14,28,37,41,34,26,19,0,0,0,0,19,26,34,41,39,27,
                                        18,0,18,27,39,41,34,26,19,0,0,0,0,0,1,18,14,30,27,28,36,34,38,35,40,37,24,21,9,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,0,19,26,33,35,40,40,41,34,31,17,7,0,0,0,0,0,0,0,0,0,10,21,36,37,40,40,40,40,40,40,35,33,30,19,0,0,0,0,0,8,17,27,39,40,38,25,26,26,26,27,39,35,39,23,16,3,0,8,17,27,39,40,38,25,26,26,26,27,39,35,39,23,16,3,0,0,8,15,13,31,28,36,32,32,32,34,35,37,28,14,4,0,0,0,8,17,27,39,40,38,
                                        25,20,6,4,14,28,37,40,39,31,18,8,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,3,17,27,39,40,39,23,16,29,28,14,20,21,21,12,5,0,0,8,15,13,31,28,36,32,32,32,34,35,37,28,14,4,0,0,0,8,17,27,39,40,38,25,20,6,4,14,28,37,40,39,31,18,8,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,2,10,10,2,15,16,14,21,29,25,33,37,35,33,30,11,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,0,2,14,28,38,
                                        40,40,37,24,21,10,0,0,0,0,0,0,0,0,0,6,16,27,34,41,40,40,40,40,40,40,40,39,25,12,1,0,0,0,0,3,16,23,39,40,37,32,32,32,32,32,32,32,32,25,20,6,0,3,16,23,39,40,37,32,32,32,32,32,32,32,32,25,20,6,0,0,9,14,25,34,38,36,31,26,26,36,35,37,28,14,4,0,0,0,3,16,23,39,40,38,25,20,7,4,12,28,37,40,39,27,17,8,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,19,26,33,35,38,25,12,29,28,14,14,28,28,14,5,0,0,9,14,25,34,38,36,31,26,26,36,35,37,28,
                                        14,4,0,0,0,3,16,23,39,40,38,25,20,7,4,12,28,37,40,39,27,17,8,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,4,12,22,22,17,8,3,5,10,18,14,27,34,41,34,26,15,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,0,8,18,29,33,35,41,39,27,16,6,0,0,0,0,0,0,0,0,0,10,13,24,37,40,40,40,40,40,40,40,40,35,32,22,15,0,0,0,0,8,17,27,39,40,38,25,21,21,21,21,21,21,21,21,16,7,0,8,17,27,39,40,38,25,21,21,21,21,
                                        21,21,21,21,16,7,0,0,19,26,33,35,34,31,16,15,13,36,35,37,28,14,4,0,0,0,8,17,27,39,40,38,25,20,6,4,14,28,37,40,39,31,18,8,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,2,14,25,39,37,24,21,29,28,14,30,32,27,17,7,0,0,19,26,33,35,34,31,16,15,13,36,35,37,28,14,4,0,0,0,8,17,27,39,40,38,25,20,6,4,14,28,37,40,39,31,18,8,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,4,12,28,25,12,4,0,0,0,8,10,22,32,35,33,30,15,0,0,0,
                                        0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,0,0,9,13,28,37,35,36,22,11,0,0,0,0,0,0,0,0,0,3,17,29,34,41,40,40,40,40,40,40,40,40,41,39,23,20,4,0,0,0,0,18,29,34,41,38,25,12,9,9,9,11,14,13,13,15,0,0,0,18,29,34,41,38,25,12,9,9,9,11,14,13,13,15,0,0,8,17,27,39,41,33,30,11,2,13,36,35,37,28,14,4,0,0,0,0,18,29,34,41,38,25,12,4,1,14,28,37,41,34,26,19,0,0,0,0,19,26,34,41,39,27,17,8,17,27,39,41,34,26,19,0,0,
                                        0,0,8,19,22,28,39,39,25,29,28,30,28,28,21,10,0,0,8,17,27,39,41,33,30,11,2,13,36,35,37,28,14,4,0,0,0,0,18,29,34,41,38,25,12,4,1,14,28,37,41,34,26,19,0,0,0,0,19,26,34,41,39,27,17,8,17,27,39,41,34,26,19,0,0,0,4,12,28,24,21,11,8,0,0,0,10,22,32,35,36,21,10,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,0,0,7,17,31,34,39,23,12,1,0,0,0,0,0,0,0,0,0,2,13,28,38,40,40,40,40,40,40,40,40,40,40,37,24,21,10,
                                        0,0,0,0,11,22,32,35,37,28,14,5,0,8,18,31,24,26,19,0,0,0,11,22,32,35,37,28,14,5,0,8,18,31,24,26,19,0,0,3,17,23,39,41,33,30,11,9,21,36,35,37,28,14,4,0,0,0,0,10,21,36,37,37,28,14,5,2,13,24,37,37,24,21,10,0,0,0,0,15,30,33,41,39,23,16,4,12,25,38,41,34,26,19,0,0,0,0,0,4,18,21,23,24,33,32,33,32,25,22,18,7,0,0,3,17,23,39,41,33,30,11,9,21,36,35,37,28,14,4,0,0,0,0,10,21,36,37,37,28,14,5,2,13,24,37,37,24,21,10,0,0,0,0,15,30,33,41,39,23,
                                        16,4,12,25,38,41,34,26,19,0,0,0,4,12,28,39,23,13,18,10,10,11,20,31,34,39,25,14,1,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,0,0,0,0,0,0,10,21,24,32,30,19,0,0,0,0,0,0,0,0,0,8,18,26,33,35,40,40,40,40,40,40,40,40,40,40,41,34,27,17,7,0,0,0,5,14,25,39,35,32,30,19,9,11,14,25,24,13,9,0,0,0,5,14,25,39,35,32,30,19,9,11,14,25,24,13,9,0,0,8,18,31,34,41,34,27,12,12,31,33,41,37,28,14,18,9,0,0,0,1,12,23,34,35,
                                        36,21,11,15,22,32,35,34,27,20,4,0,0,0,0,10,21,36,35,38,25,14,20,22,36,38,41,34,26,18,19,6,0,0,0,0,6,15,12,21,30,29,28,22,14,19,4,0,0,0,8,18,31,34,41,34,27,12,12,31,33,41,37,28,14,18,9,0,0,0,1,12,23,34,35,36,21,11,15,22,32,35,34,27,20,4,0,0,0,0,10,21,36,35,38,25,14,20,22,36,38,41,34,26,18,19,6,0,4,12,28,39,34,25,29,22,21,22,27,36,39,28,22,19,0,0,0,0,0,18,27,39,35,35,35,35,35,35,35,35,35,35,35,35,35,35,38,23,16,3,0,0,0,0,0,0,0,0,4,20,
                                        27,25,14,5,0,0,0,0,0,0,0,0,0,5,14,25,38,35,35,35,35,35,35,35,35,35,35,35,35,37,28,13,9,0,0,0,0,19,22,28,39,39,28,30,13,22,23,32,29,17,6,0,0,0,0,19,22,28,39,39,28,30,13,22,23,32,29,17,6,0,0,0,11,22,36,37,37,32,25,25,25,36,35,37,28,31,31,18,0,0,0,0,11,13,25,34,34,23,21,21,25,39,34,25,13,10,0,0,0,0,0,1,12,23,39,35,33,25,23,24,27,39,35,34,31,31,13,2,0,0,0,0,0,0,4,9,19,29,28,14,5,0,0,0,0,0,0,11,22,36,37,37,32,25,25,25,36,35,37,28,
                                        31,31,18,0,0,0,0,11,13,25,34,34,23,21,21,25,39,34,25,13,10,0,0,0,0,0,1,12,23,39,35,33,25,23,24,27,39,35,34,31,31,13,2,0,4,20,30,27,28,36,32,32,32,33,32,24,23,21,18,6,0,0,0,0,0,18,27,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,23,16,3,0,0,0,0,0,0,0,0,0,11,22,26,18,8,0,0,0,0,0,0,0,0,0,15,30,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,29,18,8,0,0,0,6,18,21,23,36,33,32,36,32,28,26,12,2,0,0,0,0,0,6,18,21,23,36,33,32,36,32,
                                        28,26,12,2,0,0,0,0,1,20,31,24,33,33,32,23,21,24,24,24,24,24,31,18,0,0,0,0,3,19,13,27,24,33,36,36,33,24,27,13,15,8,0,0,0,0,0,0,15,21,25,32,34,33,24,29,27,24,24,24,24,24,13,2,0,0,0,0,0,0,0,0,19,29,28,14,1,0,0,0,0,0,0,1,20,31,24,33,33,32,23,21,24,24,24,24,24,31,18,0,0,0,0,3,19,13,27,24,33,36,36,33,24,27,13,15,8,0,0,0,0,0,0,15,21,25,32,34,33,24,29,27,24,24,24,24,24,13,2,0,0,5,15,17,14,21,22,26,26,26,22,13,20,11,6,0,0,0,0,0,
                                        0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,0,0,0,0,0,0,0,0,1,12,14,9,0,0,0,0,0,0,0,0,0,0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,0,0,0,6,15,12,21,30,26,26,22,14,17,2,0,0,0,0,0,0,0,6,15,12,21,30,26,26,22,14,17,2,0,0,0,0,0,0,9,16,21,26,26,22,12,12,13,13,13,13,13,13,19,0,0,0,0,0,3,10,16,13,22,26,26,22,13,16,10,8,0,0,0,0,0,0,0,7,19,14,22,26,30,21,16,13,13,13,13,13,13,12,5,
                                        0,0,0,0,0,0,0,0,19,29,23,14,1,0,0,0,0,0,0,0,9,16,21,26,26,22,12,12,13,13,13,13,13,13,19,0,0,0,0,0,3,10,16,13,22,26,26,22,13,16,10,8,0,0,0,0,0,0,0,7,19,14,22,26,30,21,16,13,13,13,13,13,13,12,5,0,0,0,0,3,1,9,11,15,15,15,11,9,6,0,0,0,0,0,0,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,1,0,0,0,0,0,0,0,0,0,0,0,5,2,0,0,0,0,0,0,0,0,0,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
                                        1,0,0,0,0,0,0,0,1,10,15,15,15,10,5,3,0,0,0,0,0,0,0,0,0,0,1,10,15,15,15,10,5,3,0,0,0,0,0,0,0,0,4,10,15,15,10,1,5,5,5,5,5,5,5,7,0,0,0,0,0,0,0,6,9,11,15,15,11,9,6,0,0,0,0,0,0,0,0,0,0,8,5,11,15,15,9,6,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,10,16,16,18,7,0,0,0,0,0,0,0,0,4,10,15,15,10,1,5,5,5,5,5,5,5,7,0,0,0,0,0,0,0,6,9,11,15,15,11,9,6,0,0,0,0,0,0,0,0,0,0,8,5,11,15,
                                        15,9,6,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,3,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,2,2,2,2,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,2,2,2,2,2,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,1,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,1,0,0,0,0,0,0,0,0,3,5,11,18,17,17,18,19,10,5,3,0,0,0,0,0,0,0,0,0,0,3,5,11,18,17,17,17,18,11,9,4,0,0,0,0,0,0,0,0,0,0,0,3,2,
                                        11,18,17,17,18,19,10,1,0,0,0,0,0,0,0,0,2,10,10,10,10,10,10,10,10,10,5,0,3,10,10,10,10,10,10,10,5,0,0,5,12,13,13,13,13,13,13,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,13,13,13,13,13,13,12,1,0,0,0,0,0,0,0,3,2,11,15,15,9,1,5,5,5,5,5,5,6,0,0,0,1,5,5,5,5,5,5,1,10,15,15,10,1,0,0,0,0,0,0,0,0,0,3,2,11,15,15,9,1,5,5,5,5,5,5,6,0,0,0,0,4,9,11,19,18,17,17,18,15,2,3,0,0,0,0,0,0,0,0,0,0,
                                        0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,0,0,0,0,0,2,18,14,22,29,27,27,31,26,21,14,17,10,4,0,0,0,0,0,0,0,2,18,14,22,29,27,27,27,29,30,13,12,19,9,8,0,0,0,0,0,0,0,9,17,13,30,29,27,27,31,26,21,12,15,4,0,0,0,0,0,6,20,22,22,22,22,22,22,22,22,22,17,3,9,13,22,22,22,22,22,22,17,8,0,2,13,24,24,24,24,24,27,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,8,17,27,24,24,24,24,24,13,5,0,0,0,0,0,0,1,18,13,22,26,30,13,20,13,13,13,13,13,13,18,8,0,5,12,13,13,13,13,13,13,12,21,26,30,21,12,15,7,0,0,0,0,0,0,1,18,13,22,26,30,13,20,13,13,13,13,13,13,18,8,0,0,15,12,13,22,29,31,27,27,31,30,13,17,5,0,0,0,0,0,0,0,0,0,0,18,27,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,23,16,3,0,18,27,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,23,16,3,0,0,0,0,8,10,14,26,28,32,34,36,28,24,32,32,28,31,21,17,
                                        7,0,0,0,0,8,10,14,29,28,32,34,36,24,28,36,33,24,25,26,13,10,0,0,0,0,0,8,11,14,29,28,32,33,24,28,36,33,36,23,22,17,5,0,0,0,0,4,12,25,33,33,33,33,33,33,33,23,16,3,10,21,32,33,33,33,33,27,17,8,0,2,13,31,31,34,35,39,27,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,17,31,31,28,37,35,24,13,5,0,0,0,0,0,1,16,26,28,32,33,33,28,26,28,24,24,24,24,23,16,3,0,2,13,24,24,24,24,24,27,27,36,33,33,36,23,21,19,8,0,0,0,0,1,16,26,28,32,33,33,28,26,
                                        28,24,24,24,24,23,16,3,0,3,16,23,24,32,36,28,28,33,34,33,28,26,20,5,0,0,0,0,0,0,0,0,0,18,27,25,26,26,26,26,26,26,26,26,26,26,26,26,26,26,27,23,16,3,0,18,27,25,26,26,26,26,26,26,26,26,26,26,26,26,26,26,27,23,16,3,0,0,0,0,10,13,23,33,38,32,27,21,14,13,22,25,34,34,25,12,6,0,0,0,0,10,13,23,33,38,32,27,21,13,13,21,29,24,38,33,26,15,0,0,0,0,0,11,13,23,33,38,24,29,13,14,21,23,34,39,24,26,16,1,0,0,0,6,20,22,22,23,38,35,39,27,22,22,17,3,9,21,22,22,
                                        36,31,22,22,17,8,0,7,19,18,26,34,41,39,27,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,18,14,28,37,35,24,13,5,0,0,0,0,8,18,30,36,38,38,24,27,23,24,28,37,37,24,31,31,16,3,0,2,13,31,31,34,37,39,27,28,27,23,33,37,39,25,13,10,0,0,0,8,18,30,36,38,38,24,27,23,24,28,37,37,24,31,31,16,3,0,3,16,23,39,23,21,14,13,29,36,37,38,36,26,18,8,0,0,0,0,0,0,0,0,18,27,25,20,15,15,15,15,15,15,15,15,15,15,15,15,17,27,23,16,3,0,18,27,25,20,20,20,20,20,20,
                                        20,20,20,20,20,20,20,17,27,23,16,3,0,0,0,5,12,27,34,35,34,31,12,10,5,5,15,13,25,39,25,12,6,0,0,0,2,14,23,34,35,34,27,12,10,5,5,10,17,26,32,33,26,15,0,0,0,0,2,14,23,34,35,33,29,17,9,5,11,13,28,38,38,36,30,18,8,0,0,0,9,11,20,23,38,40,39,27,17,11,5,0,3,10,11,21,36,31,18,11,5,0,0,0,0,19,26,34,41,39,27,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,14,28,37,35,24,13,5,0,0,0,0,5,14,25,38,35,32,30,16,20,31,34,35,35,24,13,18,10,
                                        0,0,6,19,18,26,34,41,37,24,21,17,12,23,39,35,34,31,17,3,0,0,5,14,25,38,35,32,30,16,20,31,34,35,35,24,13,18,10,0,0,3,16,23,24,21,15,1,2,20,23,39,40,38,25,12,1,0,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,25,25,25,25,25,25,25,25,25,25,23,16,27,23,16,3,0,0,8,18,26,32,35,37,28,13,10,0,0,0,8,19,30,32,25,12,6,0,0,8,18,26,33,35,37,28,13,10,0,0,0,0,5,12,23,33,26,15,0,0,0,8,18,26,33,35,38,28,
                                        14,2,0,0,8,18,26,33,41,38,25,14,2,0,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,19,26,34,41,39,27,18,10,15,15,10,1,0,0,0,0,0,0,0,0,0,3,2,11,15,15,9,14,28,37,35,24,13,5,0,0,0,0,11,22,32,35,37,28,13,5,10,21,36,35,35,24,13,5,0,0,0,0,0,19,26,34,41,38,25,12,4,15,26,33,41,38,28,14,5,0,0,11,22,32,35,37,28,13,5,10,21,36,35,35,24,13,5,0,0,0,3,16,23,25,12,4,0,0,19,29,34,41,37,24,13,5,0,0,0,0,0,0,0,
                                        0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,38,38,38,38,38,38,38,38,38,23,16,27,23,16,3,0,0,5,14,25,38,40,39,27,16,7,0,0,0,0,5,14,28,25,12,6,0,0,5,14,25,38,40,39,27,16,7,0,0,0,0,0,19,26,33,26,15,0,0,0,5,14,28,38,40,39,27,17,3,0,0,0,10,21,36,35,35,33,26,19,0,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,19,26,34,41,39,27,12,21,26,30,21,12,15,7,0,0,0,0,0,0,1,18,13,
                                        22,26,30,13,14,28,37,35,24,13,5,0,0,0,0,18,31,34,41,38,25,12,4,5,13,24,37,35,24,13,5,0,0,0,0,0,19,26,34,41,39,27,17,3,10,21,32,35,35,36,21,10,0,0,18,31,34,41,38,25,12,4,5,13,24,37,35,24,13,5,0,0,0,8,18,13,13,17,3,0,0,19,26,34,41,37,24,13,5,0,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,40,40,40,40,40,38,23,16,27,23,16,3,0,0,10,22,32,35,41,34,26,19,0,0,0,0,0,7,16,31,31,12,
                                        6,0,0,10,22,32,35,41,34,26,19,0,0,0,0,0,0,9,21,31,26,15,0,0,0,11,22,32,35,41,34,26,19,0,0,0,0,1,14,28,37,40,39,23,20,6,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,19,26,34,41,39,27,27,36,33,33,36,23,21,19,8,0,0,0,0,1,16,26,28,32,33,33,28,26,28,37,35,24,13,5,0,0,0,8,17,27,39,40,38,25,20,6,1,14,28,37,35,24,13,5,0,0,0,0,0,19,26,34,41,39,27,17,8,9,21,36,35,35,33,22,11,0,8,17,27,39,40,38,25,20,6,1,14,
                                        28,37,35,24,13,5,0,0,0,0,6,5,5,6,3,3,1,16,27,39,41,38,25,12,4,0,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,40,40,40,40,40,38,23,16,27,23,16,3,0,0,19,29,34,41,35,33,22,11,0,0,0,0,0,0,10,18,18,11,0,0,0,19,29,34,41,35,33,22,11,0,0,0,0,0,0,6,19,18,18,5,0,0,0,18,29,34,41,35,33,22,11,0,0,0,0,6,20,25,38,40,37,28,14,1,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,
                                        36,31,18,0,0,0,0,0,0,19,26,34,41,39,27,28,27,23,33,37,39,25,13,10,0,0,0,8,18,30,36,38,38,24,27,23,24,28,37,35,24,13,5,0,0,0,3,16,23,39,40,38,25,20,7,1,14,28,37,35,24,13,5,0,0,0,0,0,19,26,34,41,39,27,18,0,9,21,36,35,41,33,30,11,0,3,16,23,39,40,38,25,20,7,1,14,28,37,35,24,13,5,0,0,0,0,0,0,0,5,17,16,12,30,24,37,39,36,30,19,8,0,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,
                                        40,40,40,40,40,38,23,16,27,23,16,3,0,8,17,27,39,40,35,32,21,10,0,0,0,0,0,0,0,0,0,0,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,35,24,13,2,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,19,26,34,41,37,24,21,17,12,23,39,35,34,31,17,3,0,0,5,14,25,38,35,32,30,16,20,31,34,35,35,24,13,5,0,0,0,8,17,27,39,40,38,25,20,6,1,14,28,37,35,24,13,5,0,
                                        0,0,0,0,19,26,34,41,39,27,17,8,9,21,36,35,35,33,22,11,0,8,17,27,39,40,38,25,20,6,1,14,28,37,35,24,13,5,0,0,0,0,0,0,0,10,22,23,25,36,39,24,25,30,16,1,0,0,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,40,40,40,40,40,38,23,16,27,23,16,3,0,3,17,27,39,40,35,32,21,9,0,0,0,0,0,0,0,0,0,0,0,0,3,17,27,39,40,35,32,21,9,0,0,6,19,18,18,18,18,18,18,15,3,0,3,17,27,39,40,35,32,21,
                                        9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,19,26,34,41,38,25,12,4,15,26,33,41,38,28,14,5,0,0,11,22,32,35,37,28,13,5,10,21,36,35,35,24,13,5,0,0,0,0,18,31,34,41,38,25,12,4,5,13,24,37,35,24,13,5,0,0,0,0,0,19,26,34,41,39,27,17,3,10,21,32,35,35,36,21,10,0,0,18,31,34,41,38,25,12,4,5,13,24,37,35,24,13,5,0,0,0,0,0,0,0,10,22,28,28,33,39,28,23,30,20,9,0,0,0,0,0,0,0,0,
                                        0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,40,40,40,40,40,38,23,16,27,23,16,3,0,8,17,27,39,40,35,32,21,9,0,0,0,0,0,0,0,0,0,0,0,0,8,17,27,39,40,35,32,21,9,0,0,2,13,27,27,27,27,27,27,14,1,0,8,17,27,39,40,35,32,21,9,0,0,0,0,3,16,23,38,40,35,36,21,9,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,19,26,34,41,39,27,17,3,10,21,32,35,35,36,21,10,0,0,18,31,34,41,38,
                                        25,12,4,5,13,24,37,35,24,13,5,0,0,0,0,11,22,32,35,37,28,13,2,10,21,36,35,35,24,13,5,0,0,0,0,0,19,26,34,41,38,25,12,4,15,26,33,41,38,28,14,5,0,0,11,22,32,35,37,28,13,2,10,21,36,35,35,24,13,5,0,0,0,0,0,0,0,2,12,14,13,26,24,38,39,32,31,20,1,0,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,40,40,40,40,40,38,23,16,27,23,16,3,0,8,17,27,39,40,35,32,21,10,0,0,0,0,0,0,0,0,0,0,
                                        0,0,8,17,27,39,40,35,32,21,10,0,0,2,13,28,28,32,39,39,28,14,1,0,8,17,27,39,40,35,32,21,10,0,0,0,0,7,16,23,38,40,37,24,13,2,0,0,0,7,20,23,38,40,39,27,17,8,0,0,0,0,9,21,36,31,18,0,0,0,0,0,0,19,26,34,41,39,27,17,8,9,21,36,35,35,33,22,11,0,8,17,27,39,40,38,25,20,6,1,14,28,37,35,24,13,5,0,0,0,0,5,14,25,38,35,33,26,16,20,31,34,35,35,24,13,5,0,0,0,0,0,19,26,34,41,37,24,21,17,12,23,39,35,34,31,17,3,0,0,5,14,25,38,35,33,26,16,20,31,
                                        34,35,35,24,13,5,0,0,0,0,0,0,0,0,4,1,2,17,26,33,35,37,36,22,11,0,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,40,40,40,40,40,38,23,16,27,23,16,3,0,0,19,26,34,41,35,33,30,11,0,0,0,0,0,0,9,10,10,10,7,0,0,19,26,34,41,35,33,30,11,0,0,1,12,14,21,36,35,37,28,14,1,0,0,19,26,34,41,35,33,22,11,0,0,0,0,6,12,25,38,40,37,28,14,1,0,0,0,7,16,23,38,40,39,27,17,8,0,0,0,0,9,21,
                                        36,31,18,0,0,0,0,0,0,19,26,34,41,39,27,18,0,9,21,36,35,41,33,30,11,0,3,16,23,39,40,38,25,20,7,1,14,28,37,35,24,13,5,0,0,0,0,8,18,30,36,38,38,24,27,23,24,28,37,35,24,13,5,0,0,0,0,0,19,26,34,41,39,27,28,27,23,33,37,39,25,13,10,0,0,0,8,18,30,36,38,38,24,27,23,24,28,37,35,24,13,5,0,0,0,0,8,3,3,0,0,0,0,9,13,24,37,41,34,31,17,8,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,
                                        40,40,40,40,40,38,23,16,27,23,16,3,0,0,10,21,36,35,41,34,29,18,0,0,0,0,0,5,14,22,22,21,10,0,0,10,21,36,35,41,34,29,18,0,0,0,1,9,21,36,35,37,28,14,1,0,0,10,21,36,35,41,34,29,19,0,0,0,0,5,14,28,37,40,39,27,16,7,0,0,0,8,17,27,39,40,38,23,20,7,0,0,0,0,10,22,32,26,19,0,0,0,0,0,0,19,26,34,41,39,27,17,8,9,21,36,35,35,33,22,11,0,8,17,27,39,40,38,25,20,6,1,14,28,37,35,24,13,5,0,0,0,0,0,1,16,26,28,33,34,33,24,26,28,37,35,24,13,5,0,
                                        0,0,0,0,19,26,34,41,39,27,27,36,33,33,36,23,21,19,8,0,0,0,0,1,16,26,28,33,34,33,24,26,28,37,35,24,13,5,0,0,0,6,18,16,17,9,0,0,0,1,14,28,37,40,38,23,16,3,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,40,40,40,40,40,40,40,40,38,23,16,27,23,16,3,0,0,1,12,23,39,40,39,23,20,6,0,0,0,0,11,22,32,24,21,9,0,0,1,12,25,39,40,39,23,20,6,0,0,0,9,21,36,35,37,28,14,1,0,0,1,14,25,38,40,39,27,
                                        16,7,0,0,0,10,21,36,35,35,32,30,15,0,0,0,0,0,18,29,34,41,37,28,14,1,0,0,0,0,19,26,33,22,11,0,0,0,0,0,0,19,26,34,41,39,27,17,3,10,21,32,35,35,36,21,10,0,0,18,31,34,41,38,25,12,4,5,13,24,37,35,24,13,5,0,0,0,0,0,2,19,18,13,22,26,30,21,14,28,37,37,24,13,5,0,0,0,0,0,19,26,34,41,39,27,12,21,26,26,21,12,15,7,0,0,0,0,0,0,1,18,13,22,26,30,21,14,28,37,35,24,13,5,0,0,0,5,13,23,26,19,0,0,0,1,14,28,37,40,38,23,16,3,0,0,0,0,0,0,
                                        0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,39,38,38,38,38,38,38,38,38,38,23,16,27,23,16,3,0,0,0,19,22,36,37,37,24,21,11,0,0,0,4,16,27,34,23,20,4,0,0,0,19,30,36,37,37,24,21,10,0,0,0,9,21,36,35,37,28,14,1,0,0,0,19,30,32,37,37,28,13,10,0,0,6,17,31,34,41,39,23,14,5,0,0,0,0,0,11,22,32,35,35,32,22,15,6,0,8,5,12,23,24,13,9,0,0,0,0,0,0,19,26,34,41,38,25,12,4,15,26,33,41,38,28,14,5,0,0,11,22,32,35,37,
                                        28,13,2,10,21,36,35,35,24,13,5,0,0,0,0,0,18,26,26,20,11,15,15,9,13,24,37,38,25,12,1,0,0,0,0,0,19,26,34,41,39,27,18,10,15,15,10,1,0,0,0,0,0,0,0,0,0,3,2,11,15,15,9,14,28,37,35,24,13,5,0,0,0,5,13,24,27,17,7,0,0,9,13,24,37,41,34,31,17,8,0,0,0,0,0,0,0,18,27,25,20,6,0,0,0,0,0,0,0,0,0,0,8,17,27,23,16,3,0,18,27,25,20,27,25,25,25,25,25,25,25,25,25,25,23,16,27,23,16,3,0,0,0,1,20,31,33,37,34,23,14,19,10,10,17,22,24,36,22,15,
                                        0,0,0,0,1,20,31,33,37,34,27,14,19,10,10,19,21,36,35,37,28,14,1,0,0,0,5,12,27,33,37,34,31,12,11,10,18,21,28,38,39,28,21,15,0,0,0,0,0,0,5,14,25,39,41,39,25,21,16,18,17,12,26,32,27,16,6,0,0,0,0,6,19,18,26,34,41,37,24,21,17,12,23,39,35,34,31,17,3,0,0,5,14,25,38,35,33,26,16,20,31,34,35,35,24,13,18,10,0,0,0,0,18,27,28,13,10,1,1,19,30,32,35,33,29,17,8,0,0,0,8,10,19,26,34,41,39,27,18,11,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,11,14,
                                        28,37,35,24,13,11,2,0,0,5,13,24,28,21,19,9,10,16,29,33,35,37,36,22,11,0,0,0,0,0,0,0,0,18,27,25,20,15,15,15,15,15,15,15,15,15,15,15,15,17,27,23,16,3,0,18,27,25,20,20,20,20,20,20,20,20,20,20,20,20,20,17,27,23,16,3,0,0,0,0,9,12,31,36,39,33,25,26,21,22,31,24,36,29,16,4,0,0,0,0,0,9,12,31,36,39,33,23,30,21,22,26,25,33,34,36,23,14,1,0,0,0,0,10,14,27,32,39,32,27,22,21,26,28,39,34,28,22,18,6,0,0,0,0,0,0,0,19,22,28,39,37,34,24,23,31,27,25,32,25,
                                        21,10,0,0,0,0,0,2,13,31,31,34,35,39,27,28,27,23,33,37,39,25,13,10,0,0,0,8,18,30,36,38,38,24,27,23,24,28,37,35,24,31,31,16,3,0,0,0,18,27,33,27,21,14,14,21,25,39,34,25,13,9,0,0,0,0,2,13,30,26,34,35,39,27,30,22,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,16,30,30,28,37,35,24,30,30,17,3,0,5,13,24,34,25,30,21,21,31,36,37,39,36,29,16,4,0,0,0,0,0,0,0,0,18,27,25,26,26,26,26,26,26,26,26,26,26,26,26,26,26,27,23,16,3,0,18,27,25,26,26,26,26,26,26,
                                        26,26,26,26,26,26,26,26,27,23,16,3,0,0,0,0,0,9,16,22,23,24,32,33,32,32,36,25,30,20,2,0,0,0,0,0,0,0,9,16,22,23,24,32,33,32,32,32,24,25,31,21,20,15,8,0,0,0,0,0,10,20,30,23,24,33,32,32,33,32,28,31,13,19,4,0,0,0,0,0,0,0,0,4,18,21,23,24,33,33,34,33,32,28,27,21,19,3,0,0,0,0,0,2,13,24,24,24,24,24,27,27,36,33,33,36,23,21,19,8,0,0,0,0,1,16,26,28,33,34,33,24,26,28,24,24,24,24,23,16,3,0,0,0,18,27,24,33,24,28,28,36,33,24,27,13,15,8,0,0,
                                        0,0,2,13,36,32,32,32,32,32,32,26,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,20,23,32,32,32,32,32,32,23,16,3,0,5,13,31,25,24,32,36,32,33,33,24,23,30,20,2,0,0,0,0,0,0,0,0,0,18,27,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,23,16,3,0,18,27,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,23,16,3,0,0,0,0,0,0,1,15,20,13,22,26,26,30,21,14,18,5,0,0,0,0,0,0,0,0,0,1,15,20,13,22,26,26,26,22,21,14,17,10,4,0,0,0,0,0,0,0,0,1,19,12,
                                        21,22,26,26,30,21,14,17,10,3,0,0,0,0,0,0,0,0,0,0,6,15,20,13,22,26,26,26,22,13,16,10,3,0,0,0,0,0,0,5,12,13,13,13,13,13,13,12,21,26,26,21,12,15,7,0,0,0,0,0,0,1,18,13,22,26,30,21,20,13,13,13,13,13,13,18,8,0,0,0,11,12,21,30,29,31,27,31,26,21,20,10,8,0,0,0,0,0,2,13,22,22,22,22,22,22,22,21,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,16,22,22,22,22,22,22,22,22,17,3,0,8,11,17,12,13,22,26,26,26,22,21,12,19,1,0,0,0,0,0,0,0,0,0,
                                        0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,19,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,0,0,0,0,0,0,0,6,9,11,15,15,15,10,1,8,0,0,0,0,0,0,0,0,0,0,0,0,6,9,10,15,15,15,11,9,1,8,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,15,10,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,15,10,2,6,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,1,10,15,15,10,1,0,0,0,0,0,0,0,0,0,3,2,
                                        11,15,15,9,1,5,5,5,5,5,5,6,0,0,0,0,0,4,9,11,18,18,17,18,15,10,4,0,0,0,0,0,0,0,8,9,10,10,10,10,10,10,10,10,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,10,10,10,10,10,10,10,10,5,0,0,0,0,8,1,9,10,15,15,15,11,9,4,0,0,0,0,0,0,0,0,0,0,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,1,0,0,6,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,5,2,2,2,5,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,8,4,5,5,5,1,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,4,7,0,0,6,4,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,10,10,10,10,10,10,10,10,10,10,10,3,0,0,0,0,0,0,0,4,10,19,17,17,18,19,11,2,6,0,0,0,0,0,0,8,5,11,18,17,17,18,19,10,4,0,0,0,0,0,0,0,0,8,5,15,18,17,17,18,10,1,0,0,0,0,0,0,0,0,0,0,6,10,19,17,17,18,15,5,8,0,0,0,0,0,0,7,5,10,19,18,
                                        17,17,18,11,1,0,0,0,0,0,0,0,0,5,15,18,17,18,10,4,0,0,0,0,0,11,17,17,18,1,0,0,0,0,0,0,0,0,0,0,0,3,9,15,18,17,17,18,15,10,5,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,9,15,18,17,18,15,9,6,0,0,0,0,5,12,13,13,13,14,2,0,0,0,0,0,0,0,0,0,0,6,11,17,18,9,0,0,0,0,0,0,0,0,0,8,9,18,12,14,13,13,14,20,19,5,0,0,0,0,0,0,0,0,0,0,8,10,16,12,17,10,2,20,12,16,1,0,0,0,0,0,1,10,10,10,10,10,10,10,10,10,9,0,0,
                                        0,0,5,14,22,22,22,22,22,22,22,22,22,22,21,10,0,0,0,0,0,8,10,20,21,26,27,27,27,29,22,13,20,1,0,0,0,0,1,18,14,22,29,27,27,31,26,21,20,10,0,0,0,0,0,0,4,19,14,30,31,27,27,29,21,12,10,3,0,0,0,0,0,0,0,2,17,21,26,27,27,31,30,14,19,6,0,0,0,0,1,17,14,21,26,31,27,27,29,22,12,11,3,0,0,0,0,0,10,14,30,31,27,29,21,16,5,0,0,0,5,12,27,27,22,10,0,0,0,0,0,0,0,0,0,0,5,17,13,30,31,27,27,31,26,21,14,10,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,5,18,13,26,31,27,31,26,13,16,11,4,0,0,5,12,27,24,24,29,17,7,0,0,0,0,0,0,0,0,3,18,21,27,29,14,10,0,0,0,0,0,0,0,3,15,13,29,25,28,24,24,28,23,30,12,9,0,0,0,0,0,0,0,0,0,10,13,27,25,27,13,19,22,25,13,2,0,0,0,0,0,18,22,22,22,22,22,22,22,22,22,14,5,0,0,0,5,13,24,33,33,33,33,33,33,33,33,32,21,10,0,0,0,0,8,15,13,27,24,34,24,28,28,33,32,24,13,2,0,0,0,6,17,22,25,32,34,24,28,32,33,24,27,14,10,0,0,0,0,4,17,22,25,32,34,24,28,33,
                                        36,27,13,15,8,0,0,0,0,0,9,12,29,24,33,24,28,32,32,25,21,18,6,0,0,0,9,21,28,32,36,28,28,32,34,32,23,13,15,0,0,0,0,9,14,23,32,28,23,32,24,26,17,6,0,8,19,30,32,23,20,1,0,0,0,0,0,0,0,0,0,1,16,26,28,33,33,24,28,32,33,32,30,11,0,0,0,0,0,0,0,0,0,0,0,0,2,20,26,28,33,33,28,28,32,24,27,21,17,7,0,0,10,14,23,34,28,21,10,0,0,0,0,0,0,0,0,9,13,28,39,33,27,16,7,0,0,0,0,0,0,10,13,25,33,39,25,29,29,23,33,32,31,12,5,0,0,0,0,0,
                                        0,0,8,17,31,34,34,34,25,30,27,24,13,5,0,0,0,0,0,18,31,33,33,33,33,33,33,33,24,13,5,0,0,0,5,13,24,37,37,37,37,37,37,37,37,32,21,10,0,0,0,0,10,13,25,34,33,27,21,14,13,29,36,36,13,2,0,0,0,11,21,24,38,38,28,21,14,26,33,37,33,27,20,4,0,0,0,15,22,24,39,39,25,13,14,27,34,34,25,13,9,0,0,0,0,1,16,31,32,38,25,21,14,26,32,39,28,21,15,0,0,0,9,21,36,24,22,14,14,29,32,35,34,25,14,5,0,0,8,18,29,33,33,26,12,23,39,24,21,10,0,2,14,25,24,21,11,0,0,0,
                                        0,0,0,0,0,0,0,19,30,36,38,33,31,13,13,30,24,33,30,11,0,0,0,0,0,0,0,0,0,0,0,4,16,29,32,38,33,31,13,14,26,24,39,25,20,6,0,0,0,10,13,23,34,27,12,1,0,0,0,0,0,0,0,11,30,33,35,37,28,14,1,0,0,0,0,0,6,16,27,34,35,36,21,18,19,13,28,37,32,30,19,0,0,0,0,0,0,0,6,12,25,25,29,28,39,32,33,25,12,4,0,0,0,0,0,18,22,22,29,34,35,37,28,22,22,14,5,0,0,0,5,13,24,24,24,24,24,24,24,24,24,24,21,10,0,0,0,4,20,27,34,38,25,14,10,1,2,20,27,36,
                                        13,2,0,0,3,16,27,39,41,39,27,17,9,13,24,35,37,36,21,10,0,0,4,12,23,39,41,33,26,19,10,21,36,35,33,31,17,7,0,0,0,15,22,36,37,34,29,18,5,14,25,38,39,23,12,1,0,0,9,21,36,27,17,1,5,12,25,38,35,33,30,15,0,0,1,12,25,38,36,21,18,31,34,34,29,18,3,18,26,33,27,16,4,0,0,0,0,0,0,0,0,0,6,20,23,39,37,28,14,9,5,17,31,33,30,11,0,0,0,0,0,0,0,0,0,0,0,15,22,36,37,39,25,14,9,5,16,31,34,25,20,6,0,0,0,8,11,13,25,32,30,19,8,0,0,0,0,0,0,10,
                                        22,32,37,38,25,12,4,0,0,0,0,0,2,13,28,37,37,28,14,1,15,20,23,38,39,23,20,6,0,0,0,0,0,0,1,14,28,29,18,21,23,24,28,22,19,0,0,0,0,0,0,1,11,19,29,34,41,37,28,14,11,10,0,0,0,0,5,13,24,26,13,13,13,13,13,13,13,13,14,9,0,0,0,10,21,36,37,34,29,18,8,0,0,15,30,29,13,2,0,0,4,12,25,38,41,34,29,18,5,14,28,37,41,33,26,15,0,0,9,21,24,37,35,32,21,10,1,14,28,37,37,28,13,2,0,0,6,20,23,39,35,33,22,11,7,16,23,38,35,32,22,11,0,0,9,21,24,30,15,0,
                                        8,17,27,39,40,39,27,17,8,0,5,13,24,37,24,13,19,26,34,39,27,17,9,13,28,24,21,10,0,0,0,0,0,0,0,0,0,0,4,14,28,37,37,28,14,5,0,11,22,28,30,11,0,0,0,0,0,0,0,0,0,0,1,12,23,39,41,33,26,19,0,0,10,21,24,25,20,6,0,0,0,0,3,19,21,27,27,14,1,0,0,0,0,0,0,1,12,27,36,28,30,18,8,0,0,0,0,0,9,21,36,35,38,25,12,17,22,23,28,38,34,28,14,5,0,0,0,0,0,0,4,16,13,13,11,11,12,13,13,18,4,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,
                                        0,0,5,13,24,26,15,10,11,11,10,5,5,5,5,0,0,0,8,18,31,34,35,32,22,10,2,9,2,1,19,19,15,7,0,0,4,14,28,37,41,34,29,19,1,14,28,37,41,34,26,15,0,0,11,30,33,35,35,36,21,9,4,12,25,37,35,33,30,15,0,0,2,13,24,37,35,32,21,10,8,17,27,39,41,34,31,17,8,0,2,14,13,13,10,0,0,18,31,39,40,39,23,17,3,0,2,13,24,35,24,13,19,26,34,38,23,16,17,29,33,31,17,6,0,0,0,0,0,0,0,0,0,0,6,12,25,38,35,32,26,18,7,2,20,12,12,9,0,0,0,0,0,0,0,0,0,7,10,21,
                                        36,37,35,36,21,10,6,6,4,12,25,25,20,6,0,0,4,5,5,5,19,17,17,19,3,0,0,0,0,0,0,0,10,12,21,13,18,1,0,0,0,0,0,0,10,21,32,35,38,25,12,30,24,39,33,25,31,26,13,2,0,0,0,0,4,5,5,5,2,2,5,4,2,11,15,11,9,7,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,5,13,24,26,14,21,30,30,21,14,17,9,0,0,0,0,4,12,25,38,35,36,21,12,13,21,13,12,18,5,0,0,0,0,6,20,23,39,40,34,31,18,5,13,24,37,35,32,22,11,0,0,15,26,33,41,35,24,13,2,4,12,
                                        25,38,41,34,31,18,0,0,11,22,32,35,35,36,21,9,0,18,31,39,40,38,25,12,4,0,0,5,2,5,8,0,8,17,27,39,40,39,27,17,8,0,5,13,24,37,24,13,19,29,34,39,27,17,13,28,28,13,10,0,0,0,0,0,0,0,0,0,0,0,8,18,29,33,35,38,28,21,15,3,4,4,10,10,10,10,10,10,10,3,0,0,1,17,20,26,34,41,37,28,13,20,20,20,20,17,31,31,20,6,0,7,17,13,13,13,13,13,13,17,7,0,0,0,0,0,0,0,5,13,28,27,17,8,0,0,0,0,0,0,10,21,32,35,38,25,14,28,38,39,23,14,18,19,11,3,0,0,0,7,
                                        17,13,13,13,13,13,13,17,13,22,26,30,13,18,1,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,5,13,24,26,25,36,33,33,32,28,29,14,11,8,0,0,5,13,28,37,35,24,30,25,24,36,24,25,26,20,2,0,0,0,0,19,30,36,38,39,23,20,18,21,36,35,34,23,14,5,0,0,15,30,33,41,35,36,21,9,4,12,25,38,40,39,27,17,3,0,19,26,34,41,35,24,13,2,0,18,31,34,40,37,28,14,5,0,0,0,0,0,0,0,5,14,25,38,41,33,26,19,0,0,4,12,25,38,32,21,18,31,39,33,26,16,31,33,29,18,3,8,8,0,0,0,
                                        0,0,0,0,0,0,1,18,13,25,38,41,39,25,13,11,0,11,21,22,22,22,22,22,13,10,0,0,11,22,25,25,39,40,37,28,25,25,25,25,25,13,18,18,11,0,0,4,12,25,24,24,24,24,25,12,6,0,0,0,0,0,8,1,10,13,24,27,17,8,0,0,0,0,0,0,10,21,32,35,38,25,21,36,35,34,26,19,8,0,0,0,0,0,0,4,12,25,24,24,24,24,25,22,28,33,33,33,28,30,17,7,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,5,13,24,36,31,22,30,23,33,38,33,23,13,10,0,0,9,21,36,35,35,36,36,27,26,23,34,38,32,29,
                                        16,4,0,0,0,1,16,26,28,33,32,27,29,25,34,36,23,13,15,0,0,0,10,22,32,35,35,36,21,10,1,14,28,37,40,38,23,16,3,0,18,31,34,41,35,24,13,2,0,18,29,34,41,37,24,13,2,0,0,0,0,0,3,9,17,26,32,35,38,28,13,9,0,0,0,18,26,32,33,29,14,25,38,28,13,21,24,25,14,10,19,17,17,19,9,3,0,0,0,0,0,1,17,30,28,34,36,38,35,34,23,14,10,15,26,32,32,32,32,32,21,10,0,8,17,27,25,25,38,40,38,28,25,25,25,25,31,17,3,0,0,0,0,4,12,31,31,32,37,38,25,12,6,0,0,0,0,1,19,14,
                                        21,26,24,27,17,8,0,0,0,0,0,0,10,21,32,35,38,25,21,36,35,39,27,14,10,8,0,0,0,0,0,4,12,31,31,32,37,38,25,28,25,25,32,37,38,28,13,9,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,5,13,25,29,16,11,15,14,25,38,35,33,31,16,6,0,9,21,36,35,40,38,25,14,19,13,28,37,37,24,21,11,0,0,0,6,19,13,29,23,34,36,36,33,24,27,22,20,9,0,0,0,5,13,28,38,35,33,30,15,9,13,24,37,40,38,23,16,7,0,18,31,39,40,35,24,13,2,0,19,29,34,41,35,36,13,2,0,0,0,0,0,15,13,
                                        29,24,38,39,24,30,18,7,0,0,0,5,12,31,24,36,28,33,28,22,20,27,32,26,18,13,29,27,27,26,13,19,6,0,0,0,3,18,30,24,38,36,22,24,38,35,33,27,12,15,22,30,26,32,30,30,21,10,0,0,15,20,20,25,38,40,38,25,20,20,20,20,20,10,0,0,0,0,0,0,11,18,21,32,35,38,25,12,6,0,0,0,5,16,30,25,36,33,32,27,17,8,0,0,0,0,0,0,10,21,32,35,38,25,13,28,37,35,33,27,13,15,7,0,0,0,0,0,11,18,21,32,35,37,33,29,12,12,27,39,41,33,26,19,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,
                                        0,0,4,17,12,20,5,0,0,19,26,34,41,37,24,13,9,0,10,21,32,35,40,39,27,17,7,16,23,38,41,39,27,17,3,0,0,19,21,25,33,39,25,21,13,26,36,39,36,31,12,5,0,0,3,17,29,33,35,39,27,12,16,26,33,37,40,38,23,16,3,0,18,31,39,40,35,24,13,2,0,19,29,34,41,35,24,13,2,0,0,0,0,0,18,31,33,36,28,27,21,17,1,0,0,0,0,0,9,16,21,26,26,30,13,18,22,36,25,14,22,28,34,25,25,33,25,21,15,0,0,0,9,13,28,38,39,23,20,26,36,37,37,32,29,20,11,18,31,36,21,11,11,7,0,0,5,12,14,25,
                                        38,40,38,25,14,14,14,14,15,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,7,17,26,36,38,38,28,30,14,15,0,0,0,0,0,0,0,10,21,32,35,38,25,12,27,34,35,37,34,25,21,18,6,0,0,0,0,0,10,21,32,35,37,24,13,9,15,26,33,41,39,27,17,8,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,7,4,6,0,0,0,10,22,32,35,35,33,30,15,0,9,21,36,35,41,34,29,19,8,17,27,39,40,38,25,12,4,0,5,14,25,39,35,33,30,15,9,12,25,38,37,32,26,19,0,0,0,9,14,27,32,39,32,23,23,24,
                                        27,34,41,39,27,17,8,0,18,29,34,41,35,24,13,2,0,18,29,34,41,37,24,13,5,0,0,0,0,0,18,31,36,21,13,16,11,6,0,0,0,0,0,0,0,4,10,15,19,11,2,12,23,32,30,14,25,39,28,13,21,24,39,23,12,1,0,0,19,26,33,35,33,26,19,16,29,32,37,37,36,26,17,12,25,28,14,1,0,0,0,0,15,22,28,24,38,40,37,24,28,28,28,26,19,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,9,13,28,38,35,32,30,18,5,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,13,25,34,35,35,39,28,22,18,3,0,0,0,
                                        0,10,21,32,35,37,28,14,1,11,30,33,41,39,23,17,3,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,3,3,0,0,0,0,9,21,36,35,41,34,26,19,0,2,13,24,35,41,34,26,15,0,18,31,39,40,37,28,14,1,0,11,22,32,35,35,36,21,9,8,17,27,39,40,38,23,20,6,0,0,0,10,14,26,23,28,28,23,26,29,34,41,34,29,18,0,0,15,26,33,41,35,24,13,2,0,18,31,34,40,37,28,14,1,0,0,0,0,0,18,31,36,21,9,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,30,32,23,12,21,32,38,25,12,12,25,38,36,
                                        21,9,0,8,17,27,39,35,33,22,11,2,12,27,33,35,38,24,22,21,36,27,17,3,0,0,0,8,17,27,27,27,39,40,37,28,27,27,27,13,2,5,10,10,10,8,0,0,0,10,21,32,35,38,25,12,6,0,0,19,26,33,41,38,25,14,5,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,15,13,23,33,37,41,39,28,21,10,0,0,0,0,10,21,32,35,38,25,12,4,11,30,33,41,39,23,17,3,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,15,16,16,11,0,0,0,9,21,36,35,41,34,26,19,0,1,14,28,37,41,34,26,15,0,18,31,39,40,37,
                                        28,14,1,0,19,26,34,41,35,24,13,2,0,18,31,39,40,37,28,14,1,0,0,0,8,5,19,20,14,14,20,17,27,39,35,33,30,11,0,0,10,22,32,35,35,36,21,9,0,18,31,39,40,38,23,20,6,0,0,0,0,0,18,31,23,21,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,14,25,36,22,15,26,33,38,23,16,20,25,38,33,22,11,0,3,16,23,38,41,33,30,15,0,10,14,23,34,35,38,28,23,24,21,10,0,0,0,0,0,10,18,18,26,33,41,37,24,13,18,18,15,7,16,22,22,13,2,0,0,0,10,21,32,35,38,25,12,6,0,0,18,31,39,40,39,
                                        23,16,3,0,3,10,10,10,3,0,0,0,10,21,32,35,38,25,12,6,10,14,31,32,37,41,34,31,17,8,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,7,20,23,23,16,3,0,0,10,21,32,35,35,33,30,15,0,7,16,23,39,41,34,26,19,0,18,31,39,40,38,25,12,4,0,19,26,34,41,35,24,13,2,0,18,31,39,40,37,28,14,1,0,0,10,17,17,19,6,1,1,6,20,23,38,37,28,13,2,0,0,5,13,28,37,35,32,21,10,8,17,27,39,41,34,31,18,8,0,0,0,0,5,16,21,
                                        30,13,19,3,0,0,0,0,0,0,0,0,0,0,0,0,8,18,26,32,27,20,19,26,34,38,23,16,20,25,38,33,30,11,0,8,17,27,39,41,34,31,17,7,0,10,13,25,39,35,39,33,31,16,4,0,0,0,0,0,0,0,9,13,24,37,35,32,21,10,0,0,1,14,28,28,13,5,0,0,0,10,21,32,35,38,25,12,6,0,0,18,27,39,40,39,27,17,8,0,10,21,22,21,10,0,0,0,10,21,32,35,38,25,12,17,19,9,20,31,33,35,38,25,20,6,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,
                                        0,7,20,25,25,12,4,0,0,19,26,33,41,37,24,21,9,0,0,19,26,33,35,34,31,18,3,17,27,39,41,39,27,17,3,0,11,30,33,35,35,36,21,9,8,17,27,39,40,38,25,12,4,0,0,18,31,27,14,5,0,0,2,13,28,37,34,27,16,6,0,0,7,16,27,34,35,33,30,11,7,20,23,38,37,36,21,10,0,0,0,0,8,18,26,36,33,25,13,9,0,0,0,0,0,0,0,0,0,0,0,0,9,14,25,24,21,10,15,30,33,38,23,16,20,25,38,32,22,10,0,0,19,26,33,35,38,28,21,15,6,8,15,21,28,39,41,34,23,13,10,0,0,0,0,0,0,0,6,16,
                                        27,34,41,34,29,18,8,0,10,21,36,23,20,6,0,0,0,10,21,32,35,38,25,12,6,0,0,19,29,34,41,38,23,16,7,0,11,22,32,21,10,0,0,0,10,21,32,35,38,25,26,27,13,2,9,13,28,37,38,25,12,6,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,7,20,25,24,21,19,9,10,14,23,39,35,34,27,20,4,0,0,2,14,25,39,38,23,20,9,14,25,38,37,36,22,11,0,0,2,13,28,38,35,33,30,19,9,12,25,38,35,33,29,18,8,0,0,18,31,32,30,18,9,10,16,29,
                                        33,38,28,21,10,0,0,0,0,10,21,28,38,34,31,17,9,14,28,37,34,27,20,4,0,0,0,0,6,12,25,38,35,33,26,19,0,0,0,0,0,0,0,0,0,0,0,3,17,29,33,31,16,6,10,21,36,38,25,12,14,28,37,24,13,9,0,0,9,13,28,39,41,34,25,21,20,17,20,21,23,34,38,35,34,27,14,18,18,5,0,0,0,0,0,11,21,28,38,38,28,13,11,10,20,27,32,30,19,0,0,0,0,10,21,32,35,38,25,12,6,0,0,10,21,36,37,37,28,13,10,9,16,31,32,21,10,0,0,11,18,21,32,35,38,25,26,32,22,15,9,13,28,37,39,27,17,3,0,0,
                                        0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,9,18,18,29,34,41,37,28,14,18,15,3,0,0,7,20,25,39,25,30,21,22,27,33,37,34,25,13,10,0,0,0,8,19,21,28,39,36,26,21,29,32,38,32,31,20,1,0,0,3,18,30,24,39,39,25,22,21,29,32,38,33,23,14,9,0,0,0,18,31,39,28,26,21,21,29,36,39,28,30,18,7,0,0,0,0,6,17,26,36,38,28,30,21,31,33,34,25,13,10,0,0,0,0,0,6,12,25,38,35,34,26,19,0,0,0,0,0,0,0,0,0,0,0,10,13,28,28,13,10,0,4,20,23,34,24,21,22,36,34,27,
                                        16,6,0,0,3,18,22,28,39,37,39,24,23,27,25,24,32,27,36,38,35,33,31,31,26,15,0,0,0,0,0,6,18,30,24,39,34,23,22,21,31,32,27,12,5,0,0,0,11,18,21,32,35,38,25,12,18,2,0,4,20,31,33,38,34,27,21,21,26,36,32,21,10,0,4,12,31,31,32,35,38,25,26,34,25,21,21,31,33,39,24,22,11,0,0,0,11,18,21,32,35,38,25,12,18,18,30,33,41,39,23,17,18,5,0,18,31,31,31,34,35,37,28,31,31,13,5,0,0,7,20,29,25,24,32,36,32,33,32,28,31,13,15,8,0,0,0,0,6,18,21,23,24,33,36,33,32,28,26,12,
                                        9,0,0,0,0,1,17,22,23,24,33,32,36,33,32,28,31,14,11,0,0,0,0,18,31,28,36,33,36,32,33,24,23,21,17,1,0,0,0,0,0,0,5,16,30,25,32,32,36,33,24,27,13,15,8,0,0,0,0,0,8,18,26,36,33,28,13,10,0,0,0,0,0,0,0,0,0,0,6,17,31,33,29,17,7,0,0,11,13,23,32,24,24,36,23,13,10,0,0,0,0,4,18,21,27,24,32,33,34,33,36,25,30,12,26,24,24,24,24,24,26,15,0,0,0,0,0,0,1,17,22,23,36,33,32,36,24,27,14,10,0,0,0,4,12,31,31,32,35,38,25,31,29,19,0,0,9,12,29,28,
                                        32,33,36,36,32,24,23,21,10,0,4,12,25,24,24,24,24,25,26,28,36,36,36,33,32,25,22,17,4,0,0,4,12,31,31,32,35,38,25,31,22,31,31,33,35,39,23,31,30,11,0,18,31,24,24,24,24,24,24,24,24,13,5,0,0,0,9,18,12,13,22,30,26,26,22,13,16,10,8,0,0,0,0,0,0,6,15,12,21,30,26,26,22,14,18,2,0,0,0,0,0,0,4,15,12,21,22,26,26,30,21,14,17,9,8,0,0,0,0,10,16,14,21,30,26,26,30,21,20,15,6,0,0,0,0,0,0,0,0,1,19,14,21,26,26,30,21,20,10,8,0,0,0,0,0,0,0,5,16,22,
                                        26,13,19,3,0,0,0,0,0,0,0,0,0,0,5,14,26,26,13,9,0,0,0,8,11,12,22,26,26,21,12,10,8,0,0,0,0,0,6,11,20,13,22,26,26,30,21,14,19,5,20,13,13,13,13,13,13,11,0,0,0,0,0,0,0,4,15,12,21,30,26,30,21,16,10,0,0,0,0,4,12,25,24,24,24,24,24,24,29,19,0,0,0,9,17,13,22,26,26,30,21,13,20,18,1,0,6,16,13,13,13,13,13,13,17,14,21,30,26,30,21,14,19,1,0,0,0,4,12,25,24,24,24,24,24,24,22,23,24,24,24,24,24,24,30,11,0,19,13,13,13,13,13,13,13,13,13,20,1,0,
                                        0,0,0,8,4,2,10,15,15,15,10,2,7,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,10,5,3,0,0,0,0,0,0,0,0,0,4,9,11,15,15,15,10,5,3,0,0,0,0,0,0,0,7,5,9,11,15,15,11,9,4,0,0,0,0,0,0,0,0,0,0,0,8,5,10,15,15,11,9,4,0,0,0,0,0,0,0,0,0,0,1,11,15,10,3,0,0,0,0,0,0,0,0,0,0,0,8,10,15,15,10,8,0,0,0,0,0,1,10,15,15,10,1,0,0,0,0,0,0,0,0,0,6,2,10,15,15,15,10,1,0,0,1,5,5,5,5,5,5,8,0,0,0,0,0,0,
                                        0,0,0,1,10,15,15,15,9,6,0,0,0,0,0,6,16,13,13,13,13,13,13,13,13,15,0,0,0,0,7,2,10,15,15,15,10,2,4,0,0,0,0,4,5,5,5,5,5,5,4,5,10,15,15,15,10,5,8,0,0,0,0,6,16,13,13,13,13,13,13,13,14,13,13,13,13,13,13,13,13,10,0,7,5,5,5,5,5,5,5,5,5,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,5,5,5,5,5,5,5,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,1,8,0,0,0,0,0,0,0,0,0,0,0,0,0,5,10,10,10,10,0,0,0,0,0,0,0,0,0,0,9,10,10,10,10,10,10,10,
                                        10,10,10,10,10,5,7,0,0,0,0,0,0,1,10,10,10,10,10,10,10,10,10,10,10,2,4,0,0,0,0,0,0,0,0,1,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,7,0,1,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,3,0,1,10,10,10,10,10,10,10,10,10,9,2,10,10,10,10,10,10,10,10,10,5,0,0,1,10,10,10,10,10,10,10,10,10,9,0,10,10,10,10,10,10,10,10,3,0,0,0,1,10,10,10,10,10,10,10,10,10,9,0,0,0,0,0,0,0,0,0,5,10,10,10,10,10,10,10,10,3,0,0,0,0,0,0,1,10,10,
                                        10,10,10,10,10,10,6,0,0,5,10,10,10,10,10,10,1,0,0,0,0,0,5,10,10,10,10,10,10,10,3,0,1,10,10,10,10,10,10,10,10,10,10,10,9,1,8,0,0,0,0,0,0,0,0,10,12,13,14,19,7,0,0,0,0,0,0,0,0,0,0,0,7,17,22,22,22,14,5,0,0,0,0,0,0,0,0,5,14,22,22,22,22,22,22,22,22,22,22,22,21,13,16,10,3,0,0,0,0,18,22,22,22,22,22,22,22,22,22,22,21,13,12,18,9,8,0,0,0,0,0,18,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,21,10,0,18,22,22,22,22,22,22,22,22,22,
                                        22,22,22,22,22,22,13,9,0,18,22,22,22,22,22,22,22,22,22,14,20,22,22,22,22,22,22,22,22,22,16,7,0,18,22,22,22,22,22,22,22,22,22,14,5,14,22,22,22,22,22,22,21,10,0,0,0,18,22,22,22,22,22,22,22,22,22,14,5,0,0,0,0,0,0,0,8,17,22,22,22,22,22,22,22,21,10,0,0,0,0,0,0,18,22,22,22,22,22,22,22,21,11,0,8,17,22,22,22,22,22,22,17,4,0,0,0,3,17,22,22,22,22,22,22,13,9,0,18,22,22,22,22,22,22,22,22,22,22,22,21,14,18,5,0,0,0,0,0,0,9,14,23,24,25,21,11,0,0,0,
                                        0,0,0,0,0,0,0,0,2,13,28,33,32,30,15,0,0,0,0,0,0,0,0,5,13,24,33,33,33,33,33,33,33,33,33,32,36,28,27,21,19,3,0,0,0,18,31,33,33,33,33,33,33,33,33,33,32,24,25,26,14,19,6,0,0,0,0,18,31,33,33,33,33,33,33,33,33,33,33,33,33,33,33,22,11,0,18,31,33,33,33,33,33,33,33,33,33,33,33,33,33,32,21,9,0,18,31,33,33,33,33,33,33,33,24,13,20,25,33,33,33,33,33,33,33,23,20,7,0,18,31,33,33,33,33,33,33,33,24,13,5,13,24,33,33,33,33,32,21,10,0,0,0,18,31,33,33,33,33,33,
                                        33,33,24,13,5,0,0,0,0,0,0,0,3,17,23,33,33,33,33,33,33,31,17,3,0,0,0,0,1,12,25,33,33,33,33,33,33,30,15,0,8,17,27,33,33,33,33,24,22,19,3,0,0,3,16,23,33,33,33,33,32,21,9,0,18,31,33,33,33,33,33,33,33,33,33,32,36,25,26,12,9,0,0,0,0,0,19,26,33,37,39,27,16,7,0,0,0,0,0,0,0,0,0,0,19,30,33,35,39,27,16,6,0,0,0,0,0,0,0,5,14,22,22,28,37,35,34,26,22,30,29,25,34,37,34,25,13,10,0,0,0,18,22,22,29,34,35,37,28,22,30,26,23,32,38,33,25,22,18,6,
                                        0,0,0,18,22,22,29,34,35,37,28,29,29,29,29,29,29,29,33,22,11,0,18,22,22,29,34,35,37,28,29,29,29,29,29,29,29,32,21,9,0,18,22,22,29,34,35,37,28,22,22,14,20,22,22,23,38,35,39,27,22,22,16,7,0,18,22,22,29,34,35,37,28,22,22,14,5,14,22,31,33,23,22,22,21,10,0,0,0,18,22,22,29,34,35,37,28,22,22,14,5,0,0,0,0,0,0,0,8,17,22,22,27,39,38,35,38,28,14,5,0,0,0,0,11,22,36,38,35,35,32,22,22,22,11,0,8,17,22,22,31,34,35,39,25,13,10,0,0,3,17,22,22,27,24,22,22,13,9,0,
                                        18,22,22,29,34,35,37,28,22,30,27,32,37,38,32,31,20,1,0,0,0,0,18,31,34,40,38,25,20,6,0,0,0,0,0,0,0,0,0,6,20,23,39,40,37,24,13,9,0,0,0,0,0,0,0,0,10,11,14,28,37,41,34,26,19,11,18,13,28,38,35,34,31,17,8,0,0,1,11,19,29,34,41,37,28,14,11,19,12,26,32,37,39,28,21,15,0,0,0,1,11,19,29,34,41,37,28,14,18,18,18,18,18,30,33,22,11,0,1,11,19,29,34,41,37,28,14,18,18,18,18,18,26,32,21,9,0,1,11,19,29,34,41,37,28,14,11,10,2,11,16,23,38,40,39,27,17,11,2,0,
                                        0,1,11,19,29,34,41,37,28,14,11,10,10,14,31,33,27,14,11,11,10,3,0,0,0,1,11,19,29,34,41,37,28,14,11,10,0,0,0,0,0,0,0,0,0,5,11,18,27,24,24,37,35,32,30,15,0,0,0,7,16,27,24,36,35,35,32,22,11,10,6,0,0,5,11,18,31,34,40,35,34,23,14,2,0,0,5,11,17,27,24,13,11,10,3,0,1,11,19,29,34,41,37,28,14,15,20,31,34,41,37,36,22,15,0,0,0,0,15,22,36,38,33,31,17,8,0,0,0,0,0,0,0,0,0,9,21,24,34,35,35,33,26,18,0,0,0,0,0,0,0,0,0,1,14,28,37,41,34,26,
                                        19,0,3,17,27,39,40,38,25,20,6,0,0,0,0,19,29,34,41,37,28,14,4,0,5,20,27,39,41,39,25,14,5,0,0,0,0,19,29,34,41,37,28,14,4,0,1,2,11,30,36,22,11,0,0,0,19,29,34,41,37,28,14,1,0,5,2,19,26,36,21,9,0,0,0,19,29,34,41,37,28,14,4,0,0,3,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,14,4,10,14,27,33,27,14,10,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,23,39,40,39,23,20,4,0,0,9,13,28,27,36,
                                        35,35,32,22,10,0,0,0,0,0,0,18,31,34,37,41,35,33,29,16,1,0,0,8,17,27,24,13,2,0,0,0,0,0,19,29,34,41,37,28,14,4,10,21,24,37,40,39,27,16,3,0,0,4,5,20,26,23,31,14,9,0,0,0,0,0,0,0,0,0,8,18,29,32,23,38,40,38,25,12,1,0,0,0,0,0,0,0,0,1,14,28,37,41,34,26,19,0,0,18,31,39,40,38,25,12,6,0,0,0,0,19,29,34,41,37,28,14,4,0,0,11,22,32,35,35,32,30,15,0,0,0,0,19,29,34,41,37,28,14,4,4,20,13,13,13,21,13,10,0,0,0,19,29,34,41,37,28,14,1,
                                        1,12,13,13,21,21,14,9,0,0,0,19,29,34,41,37,28,14,4,0,0,3,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,14,10,14,27,33,27,14,10,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,30,32,35,37,24,21,10,0,0,18,26,32,22,36,35,35,32,22,10,0,0,0,0,0,0,18,31,32,24,37,40,37,36,30,18,7,0,8,17,27,24,13,2,0,0,0,0,0,19,29,34,41,37,28,14,4,1,14,28,37,40,38,25,12,4,0,7,17,13,13,13,13,13,13,17,7,0,0,
                                        0,0,0,0,0,0,1,12,25,25,26,33,41,37,36,21,10,0,0,0,0,0,0,0,0,1,14,28,37,41,34,26,19,7,5,20,23,39,41,39,27,16,3,0,0,0,0,19,29,34,41,37,28,14,4,0,0,2,13,24,37,40,39,27,16,3,0,0,0,19,29,34,41,37,28,14,4,1,14,28,23,16,9,9,8,0,0,0,19,29,34,41,37,28,14,1,5,13,24,27,18,9,2,0,0,0,0,19,29,34,41,37,28,14,4,3,3,3,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,14,14,27,32,31,12,10,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,
                                        28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,13,28,38,41,34,31,17,3,1,12,25,25,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,29,32,35,41,38,28,21,11,0,8,17,27,24,13,2,0,0,0,0,0,19,29,34,41,37,28,14,4,4,12,25,37,40,38,25,12,4,0,4,12,25,24,24,24,24,25,12,6,0,0,0,0,0,0,0,0,10,21,36,29,13,24,37,41,34,31,17,8,0,0,0,0,0,0,0,1,14,28,37,41,34,26,16,16,14,26,36,37,39,28,21,11,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,4,12,25,38,40,38,25,
                                        14,1,0,0,0,19,29,34,41,37,28,14,20,20,14,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,20,20,13,24,27,18,0,0,0,0,0,0,19,29,34,41,37,28,14,16,16,16,16,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,13,23,32,31,12,9,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,13,29,34,41,38,28,14,5,11,22,36,29,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,27,34,35,41,34,23,14,9,8,17,27,24,13,2,0,0,0,
                                        0,0,19,29,34,41,37,28,14,4,2,13,24,37,40,39,23,16,7,0,4,12,31,31,32,37,38,25,12,6,0,0,0,0,0,0,0,3,17,31,24,21,20,23,39,40,38,25,14,5,0,0,0,0,0,0,0,1,14,28,37,41,34,23,23,23,28,32,36,28,27,21,18,6,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,6,20,25,38,40,37,24,13,5,0,0,0,19,29,34,41,37,28,25,25,25,25,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,25,25,25,25,24,27,18,0,0,0,0,0,0,19,29,34,41,37,28,23,23,23,23,23,23,23,38,40,39,27,17,8,0,0,
                                        0,0,0,19,29,34,41,37,28,23,34,34,27,12,9,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,13,21,24,37,35,32,30,15,16,27,24,13,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,13,25,39,41,35,33,31,20,1,17,27,24,13,2,0,0,0,0,0,19,29,34,41,37,28,14,9,17,26,33,35,35,32,26,19,0,0,0,11,18,21,32,35,38,25,12,6,0,0,0,0,0,0,0,5,14,28,23,12,15,30,33,35,35,32,22,15,0,0,0,0,0,0,0,1,14,28,37,41,34,28,
                                        28,28,24,33,24,28,27,22,16,2,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,7,16,23,38,40,35,24,13,2,0,0,0,19,29,34,41,37,36,24,24,24,24,36,23,16,3,0,0,0,0,0,19,29,34,41,37,36,24,24,24,24,32,27,18,0,0,0,0,0,0,19,29,34,41,37,32,36,36,36,36,36,36,36,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,32,34,35,37,33,29,16,1,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,13,20,23,39,40,39,23,20,13,28,27,16,21,36,
                                        35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,19,22,24,38,40,37,36,26,18,17,27,24,13,2,0,0,0,0,0,19,29,34,41,37,28,13,21,26,24,38,37,33,23,14,5,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,15,30,32,26,19,2,13,28,37,41,39,27,16,7,0,0,0,0,0,0,1,14,28,37,41,34,26,14,14,13,29,36,38,39,36,29,20,1,0,0,0,0,19,29,34,41,37,28,14,4,0,0,7,20,23,38,40,35,24,13,2,0,0,0,19,29,34,41,37,28,13,13,13,13,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,13,13,
                                        13,13,24,27,18,0,0,0,0,0,0,19,29,34,41,37,28,13,13,13,13,13,13,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,25,39,41,37,36,26,17,4,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,13,15,30,32,35,37,24,21,26,32,22,15,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,9,17,26,32,37,40,38,28,21,17,27,24,13,2,0,0,0,0,0,19,29,34,41,37,32,24,36,33,34,33,24,31,14,10,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,
                                        0,0,0,0,6,16,27,28,13,16,16,16,27,39,41,37,28,13,9,0,0,0,0,0,0,1,14,28,37,41,34,26,19,1,9,16,29,33,41,37,32,30,19,0,0,0,0,19,29,34,41,37,28,14,4,0,0,6,20,25,38,40,37,28,14,5,0,0,0,19,29,34,41,37,28,14,5,5,14,28,23,16,3,0,0,0,0,0,19,29,34,41,37,28,14,5,5,13,24,27,18,0,0,0,0,0,0,19,29,34,41,37,28,14,2,2,2,2,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,21,28,38,40,38,24,22,19,3,0,0,0,0,0,0,0,0,0,19,29,34,41,37,
                                        28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,13,2,14,28,38,41,34,31,25,25,14,9,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,9,5,12,27,34,35,41,39,25,13,27,24,13,2,0,0,0,0,0,19,29,34,41,37,28,29,29,29,29,30,13,16,10,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,9,13,24,25,23,23,23,23,23,33,41,35,33,26,19,0,0,0,0,0,0,1,14,28,37,41,34,26,19,0,0,9,21,36,35,40,39,23,16,7,0,0,0,19,29,34,41,37,28,14,4,0,0,1,14,28,37,40,38,25,
                                        12,4,0,0,0,19,29,34,41,37,28,14,4,1,14,30,30,17,3,0,0,0,0,0,19,29,34,41,37,28,14,1,5,14,30,30,18,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,3,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,14,30,36,37,41,39,25,21,11,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,8,8,0,0,0,0,0,0,18,27,24,13,2,17,31,34,41,38,28,32,29,18,9,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,9,0,10,13,25,39,41,35,34,27,27,24,13,2,0,0,0,
                                        0,0,19,29,34,41,37,28,14,18,18,19,15,9,6,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,19,26,32,25,25,25,25,25,25,28,38,40,39,23,20,4,0,0,0,0,0,1,14,28,37,41,34,26,19,0,0,5,14,28,37,40,38,25,12,6,0,0,0,19,29,34,41,37,28,14,4,0,0,9,13,24,37,41,34,31,17,8,0,0,0,19,29,34,41,37,28,14,4,0,9,11,11,10,15,15,1,0,0,0,19,29,34,41,37,28,14,1,8,10,11,11,5,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,3,16,23,38,40,39,27,17,8,0,0,
                                        0,0,0,19,29,34,41,37,28,14,16,29,32,37,35,34,23,14,10,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,11,17,17,15,8,0,0,0,0,18,27,24,13,2,10,21,24,37,35,39,24,13,9,9,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,9,0,8,19,21,28,38,40,37,32,27,24,13,2,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,4,12,23,28,14,12,12,12,12,12,31,34,41,37,24,21,10,0,0,0,0,0,1,14,28,37,41,34,26,
                                        19,0,0,2,13,24,37,40,38,25,20,6,0,0,0,19,29,34,41,37,28,14,4,0,0,19,26,33,35,37,36,22,11,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,9,13,30,30,19,0,0,0,19,29,34,41,37,28,14,1,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,3,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,14,2,12,27,33,35,35,33,27,12,2,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,7,20,27,27,12,6,0,0,0,0,18,27,24,13,2,4,20,23,39,41,39,27,16,6,9,21,36,
                                        35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,9,0,0,6,17,26,32,37,40,38,34,24,13,2,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,10,21,36,31,17,4,4,4,4,10,21,36,37,41,34,29,18,8,0,0,0,0,1,14,28,37,41,34,26,19,0,6,19,22,32,35,41,34,27,17,8,0,0,0,19,29,34,41,37,28,14,4,3,10,14,25,39,35,34,27,20,1,0,0,0,0,19,29,34,41,37,28,14,4,7,7,7,9,21,36,29,19,0,0,0,19,29,34,41,37,28,14,1,
                                        0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,3,16,23,38,40,39,27,17,8,0,0,0,0,0,19,29,34,41,37,28,14,4,10,14,23,34,35,37,32,29,16,1,0,0,0,0,0,0,19,29,34,41,37,28,14,4,7,7,7,20,25,25,12,6,0,0,0,0,18,27,24,13,2,0,15,30,32,33,32,22,15,0,9,21,36,35,35,32,22,10,0,0,0,0,0,0,18,31,36,21,9,0,0,0,5,20,27,33,35,40,35,24,13,2,0,0,0,0,0,19,29,34,41,37,28,14,4,0,0,0,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,
                                        0,4,18,17,31,36,21,18,10,0,0,3,15,12,23,38,40,38,25,14,18,10,0,3,15,18,14,28,37,41,34,26,18,18,16,21,25,39,41,38,24,21,11,0,0,9,18,18,29,34,41,37,28,14,18,17,13,27,33,37,34,25,13,10,0,0,0,9,18,18,29,34,41,37,28,14,16,16,16,16,16,21,36,29,19,0,9,18,18,29,34,41,37,28,14,18,18,2,0,0,0,0,0,0,0,9,18,18,29,34,41,37,28,14,18,15,11,18,16,23,38,40,39,27,17,18,10,0,0,9,18,18,29,34,41,37,28,14,18,15,11,21,28,39,41,37,36,30,17,18,10,0,0,9,18,18,29,34,41,37,
                                        28,14,16,16,16,16,20,25,25,12,6,0,0,10,18,18,27,24,13,18,19,5,14,26,26,26,14,5,19,18,21,36,35,35,32,22,18,18,5,0,0,10,18,18,31,36,21,18,18,4,0,0,10,14,25,39,41,35,24,13,2,0,0,0,9,18,18,29,34,41,37,28,14,18,18,9,0,0,0,0,0,0,0,0,0,11,18,21,32,35,38,25,12,18,2,0,0,10,21,31,25,25,31,31,20,7,0,5,13,31,31,34,35,35,32,31,31,20,6,5,13,31,31,28,37,35,34,31,31,31,23,24,39,38,34,28,26,17,4,0,0,18,31,31,31,34,35,37,28,31,31,27,28,34,34,36,27,13,15,8,
                                        0,0,0,18,31,31,31,34,35,37,28,23,23,23,23,23,23,23,36,29,19,0,18,31,31,31,34,35,37,28,31,31,29,19,0,0,0,0,0,0,0,18,31,31,31,34,35,37,28,31,31,13,20,31,31,23,38,35,39,27,31,31,20,7,0,18,31,31,31,34,35,37,28,31,31,13,5,19,22,24,38,35,38,28,31,31,20,7,0,18,31,31,31,34,35,37,28,23,23,23,23,23,23,25,25,12,6,0,3,17,31,31,27,24,31,31,21,9,10,15,15,15,10,10,21,31,31,36,35,35,32,31,31,30,15,0,8,17,31,31,31,36,31,31,21,10,0,0,0,15,21,28,38,35,24,13,2,0,0,0,
                                        18,31,31,31,34,35,37,28,31,31,31,18,0,0,0,0,0,0,0,0,4,12,31,31,32,35,38,25,31,29,19,0,0,10,21,24,24,24,24,25,20,7,0,5,13,24,24,24,24,24,24,24,25,20,6,5,13,24,24,24,24,24,24,24,24,24,24,24,28,23,29,21,17,1,0,0,0,18,31,24,24,24,24,24,24,24,24,24,28,25,31,21,20,10,8,0,0,0,0,18,31,24,24,24,24,24,24,24,24,24,24,24,24,24,24,29,19,0,18,31,24,24,24,24,24,24,24,24,29,19,0,0,0,0,0,0,0,18,31,24,24,24,24,24,24,24,24,13,20,25,24,24,24,24,24,24,24,23,20,7,
                                        0,18,31,24,24,24,24,24,24,24,24,13,5,4,17,26,24,24,24,24,24,23,20,7,0,18,31,24,24,24,24,24,24,24,24,24,24,24,24,24,25,12,6,0,3,17,23,24,24,24,24,24,21,9,0,0,0,0,0,10,21,24,24,24,24,24,24,24,24,30,15,0,8,17,27,24,24,24,24,24,21,10,0,0,0,7,18,30,24,24,24,13,2,0,0,0,18,31,24,24,24,24,24,24,24,24,31,18,0,0,0,0,0,0,0,0,4,12,25,24,24,24,24,24,24,29,19,0,0,9,14,13,13,13,13,13,17,3,0,1,12,13,13,13,13,13,13,13,13,17,3,1,12,13,13,13,13,13,13,13,
                                        13,13,13,13,14,20,18,10,7,0,0,0,0,19,13,13,13,13,13,13,13,13,13,13,14,12,17,11,1,0,0,0,0,0,0,19,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,15,0,19,13,13,13,13,13,13,13,13,13,13,15,0,0,0,0,0,0,0,19,13,13,13,13,13,13,13,13,13,20,17,13,13,13,13,13,13,13,13,13,17,3,0,19,13,13,13,13,13,13,13,13,13,20,1,0,5,20,13,13,13,13,13,13,17,3,0,19,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,17,7,0,8,18,13,13,13,13,13,13,14,2,0,0,0,0,0,2,14,13,13,13,
                                        13,13,13,13,13,13,10,0,0,18,13,13,13,13,13,13,14,9,0,0,0,0,1,20,13,13,13,12,5,0,0,0,19,13,13,13,13,13,13,13,13,13,13,15,0,0,0,0,0,0,0,0,6,16,13,13,13,13,13,13,13,13,15,0,0,0,5,5,5,5,5,5,4,0,0,0,1,5,5,5,5,5,5,5,5,4,0,0,1,5,5,5,5,5,5,5,5,5,5,5,1,6,0,0,0,0,0,0,0,7,5,5,5,5,5,5,5,5,5,5,5,4,8,0,0,0,0,0,0,0,0,7,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,0,7,5,5,5,5,5,5,5,5,5,
                                        5,3,0,0,0,0,0,0,0,7,5,5,5,5,5,5,5,5,5,1,4,5,5,5,5,5,5,5,5,5,4,0,0,7,5,5,5,5,5,5,5,5,5,1,0,0,0,1,5,5,5,5,5,5,4,0,0,7,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,0,0,0,6,5,5,5,5,5,5,5,0,0,0,0,0,0,0,5,5,5,5,5,5,5,5,5,5,8,0,0,6,5,5,5,5,5,5,5,0,0,0,0,0,0,1,5,5,5,1,0,0,0,0,7,5,5,5,5,5,5,5,5,5,5,7,0,0,0,0,0,0,0,0,0,4,5,5,5,5,5,5,5,5,3,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,6,5,2,5,1,6,8,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,1,0,0,0,0,4,5,5,5,5,5,5,5,1,8,5,5,5,5,5,1,0,0,0,0,0,0,0,0,0,0,0,1,10,10,10,10,10,10,10,10,10,10,10,10,5,3,0,0,0,0,0,0,0,6,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,9,0,0,0,5,10,10,10,10,10,10,10,10,10,8,0,0,5,10,10,10,10,10,
                                        10,7,0,0,2,10,10,10,10,10,10,10,10,10,8,0,8,10,10,10,10,10,8,0,8,10,10,10,10,10,10,10,0,0,1,10,10,10,10,10,10,10,10,10,2,5,10,10,10,10,10,10,10,8,0,0,0,2,10,10,10,10,10,10,10,10,10,3,0,9,10,10,10,10,10,2,0,0,0,6,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,9,0,0,0,0,0,0,1,19,20,14,13,13,14,12,17,5,0,4,16,13,13,13,13,13,13,17,7,0,0,0,0,0,0,0,0,0,0,6,16,13,13,13,13,13,13,17,7,0,0,0,0,0,0,0,0,0,0,6,16,13,13,13,13,13,
                                        13,17,7,0,0,6,16,13,13,13,13,13,13,13,20,10,13,13,13,13,13,12,5,0,0,0,0,0,0,0,0,0,0,18,22,22,22,22,22,22,22,22,22,22,22,21,14,17,9,8,0,0,0,0,0,11,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,14,5,0,3,16,22,22,22,22,22,22,22,22,13,2,0,3,17,22,22,22,22,22,21,10,0,6,20,22,22,22,22,22,22,22,22,13,2,0,2,13,22,22,22,13,9,0,9,13,22,22,22,22,22,14,5,0,19,22,22,22,22,22,22,22,22,22,16,16,22,22,22,22,22,22,13,2,0,0,6,20,22,22,22,22,22,22,
                                        22,22,13,9,5,14,22,22,22,22,22,16,7,0,0,11,21,22,22,22,22,22,22,22,22,22,22,22,22,22,22,14,5,0,0,0,0,1,16,22,23,28,24,24,28,25,26,15,0,1,14,28,24,24,24,24,25,12,6,0,0,0,0,0,0,0,0,0,0,4,12,25,24,24,24,24,25,12,6,0,0,0,0,0,0,0,0,0,0,4,12,25,24,24,24,24,25,12,6,0,0,4,14,28,24,24,24,24,24,28,14,15,30,24,24,24,24,21,9,0,0,0,0,0,0,0,0,0,0,18,31,33,33,33,33,33,33,33,33,33,32,36,28,31,13,11,0,0,0,0,0,15,30,33,33,33,33,33,33,33,
                                        33,33,33,33,33,33,33,33,24,13,5,0,7,16,23,33,33,33,33,33,33,24,13,2,0,3,16,23,33,33,33,33,22,11,0,6,20,25,33,33,33,33,33,33,36,13,2,0,15,30,33,33,33,30,15,0,9,21,36,33,33,33,24,13,5,0,18,29,33,33,33,33,33,33,33,25,20,20,23,33,33,33,33,24,13,2,0,0,6,20,25,33,33,33,33,33,33,32,21,10,5,13,24,33,33,33,25,20,6,0,0,11,30,33,33,33,33,33,33,33,33,33,33,33,33,33,24,13,5,0,0,0,8,18,30,36,39,34,23,26,23,34,26,15,0,1,14,31,31,32,35,38,25,12,6,0,0,0,0,0,
                                        0,0,0,0,0,4,12,31,31,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,0,4,12,31,31,32,35,38,25,12,6,0,0,4,14,31,27,39,37,39,31,31,14,15,30,31,25,23,31,21,9,0,0,0,0,0,0,0,0,0,0,18,22,22,29,34,35,37,28,22,30,29,28,39,38,33,23,14,9,0,0,0,0,15,30,33,29,29,29,29,29,33,35,37,24,29,29,29,29,27,24,13,5,0,3,16,22,22,24,37,35,34,27,22,13,2,0,3,17,22,22,25,25,22,21,10,0,6,20,22,22,36,35,35,33,26,22,13,2,3,17,27,39,35,39,27,17,3,9,13,22,22,32,30,22,14,
                                        5,0,19,22,22,25,39,35,37,24,22,22,16,16,22,22,28,25,22,22,13,2,0,0,6,20,22,22,32,35,35,34,29,22,21,9,5,14,22,31,32,22,22,16,7,0,0,11,30,33,29,29,29,29,29,29,29,29,28,37,35,38,28,13,5,0,0,0,1,14,25,38,37,24,13,19,21,24,26,15,0,8,11,18,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,11,18,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,11,18,21,32,35,38,25,12,6,0,0,8,11,18,22,32,35,38,28,13,15,5,18,21,36,30,18,19,6,0,0,0,0,0,0,0,0,0,
                                        0,1,11,19,29,34,41,37,28,14,11,18,21,24,37,35,33,29,18,8,0,0,0,15,30,33,22,18,18,18,30,33,41,37,24,13,18,18,18,27,24,13,5,0,0,2,11,20,23,39,40,38,28,14,10,8,0,0,5,11,21,36,29,18,10,6,0,0,2,11,12,25,38,40,39,23,16,10,8,1,14,25,38,40,38,28,14,1,3,10,18,31,24,13,11,10,8,0,1,11,19,22,36,37,41,39,23,12,2,2,20,27,36,30,19,11,10,8,0,0,0,2,11,12,23,39,41,38,28,13,10,3,0,10,21,28,23,12,11,2,0,0,0,11,30,33,22,18,18,18,18,18,12,27,34,41,35,32,26,18,
                                        3,0,0,0,9,21,36,35,37,25,14,1,14,22,22,15,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,1,14,25,38,35,33,30,19,8,17,31,28,13,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,4,20,23,38,40,38,25,20,6,0,0,0,15,30,33,22,10,0,15,30,33,41,37,24,13,5,0,18,27,24,13,5,0,0,0,0,19,30,33,35,35,33,30,15,0,0,0,3,17,31,24,21,10,
                                        0,0,0,0,0,8,17,31,34,41,37,28,13,5,0,10,21,36,37,41,35,36,21,10,0,4,12,25,23,20,6,0,0,0,0,0,1,20,27,34,35,35,32,26,18,19,22,36,27,20,1,0,0,0,0,0,0,0,0,15,22,36,37,41,34,31,16,6,6,16,31,36,22,15,0,0,0,0,0,11,30,32,22,10,0,0,3,18,26,32,37,41,34,23,12,5,0,0,0,1,10,21,32,35,38,25,12,5,9,11,11,4,0,0,0,10,21,32,35,38,25,12,2,11,15,11,9,7,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,1,5,5,5,5,5,5,3,0,0,0,0,10,21,32,35,38,
                                        25,12,6,0,0,0,0,8,18,29,34,41,39,23,20,5,14,28,27,16,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,8,17,27,39,40,37,28,12,4,0,0,0,11,22,26,21,10,0,15,30,33,41,37,24,13,5,0,18,26,26,14,5,0,0,0,0,2,13,28,37,40,39,23,16,6,0,0,5,14,28,23,20,4,0,0,0,0,0,0,11,30,32,35,35,32,22,11,0,19,26,32,24,37,41,34,29,18,0,9,21,24,29,18,0,0,0,0,0,0,0,10,13,28,38,40,38,28,13,14,25,28,21,10,0,0,0,0,0,0,0,0,0,4,20,27,39,41,
                                        37,24,21,10,10,21,24,27,20,1,0,0,0,0,0,11,21,21,21,10,0,0,10,13,28,38,40,38,28,21,11,0,0,0,6,16,13,21,32,35,38,25,13,13,13,17,3,0,0,0,0,10,21,32,35,38,25,12,13,22,26,30,13,18,1,0,0,0,0,0,0,10,21,32,35,38,25,12,1,20,13,13,13,13,13,13,11,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,10,21,24,37,37,24,21,11,22,32,22,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,8,17,27,39,40,38,25,12,4,0,0,0,4,15,15,11,6,0,15,30,33,
                                        41,37,24,13,5,0,5,15,15,10,8,0,0,0,0,7,17,27,34,41,37,24,13,9,0,0,15,30,32,26,19,0,0,0,0,0,0,0,2,13,28,37,41,34,31,18,6,16,23,28,23,38,40,39,23,20,6,19,26,36,21,10,0,0,0,0,0,0,0,3,18,26,32,35,35,34,27,29,33,26,17,7,0,0,0,0,0,0,0,0,0,0,11,21,24,37,41,34,27,16,20,27,24,21,11,0,0,0,0,0,0,7,10,10,10,3,0,5,12,27,34,41,35,32,26,17,7,0,0,0,1,14,28,24,33,35,37,36,24,24,23,20,7,0,0,0,0,10,21,32,35,38,25,22,28,33,33,33,28,30,
                                        17,7,0,0,0,0,0,10,21,32,35,38,25,12,5,14,28,24,24,24,24,29,19,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,6,20,23,39,41,34,31,17,27,28,14,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,2,14,25,38,35,34,31,17,3,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,11,22,32,35,35,33,26,18,0,6,16,27,28,13,2,0,0,0,0,0,0,0,6,20,23,39,40,38,25,12,5,13,28,27,29,34,41,37,28,13,2,16,27,28,14,5,0,0,
                                        0,0,0,0,0,0,5,12,23,39,41,37,36,28,25,14,2,0,0,0,0,0,0,0,0,0,0,0,4,16,27,34,41,37,24,21,21,24,27,16,4,0,0,0,0,0,0,0,0,0,0,0,3,18,30,32,37,41,39,23,12,5,0,0,0,0,1,14,31,31,32,35,38,25,31,31,31,20,7,0,0,0,0,10,21,32,35,38,25,28,25,25,32,37,38,28,13,9,0,0,0,0,0,10,21,32,35,38,25,12,5,14,31,27,24,31,31,29,19,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,15,30,32,35,38,25,13,28,31,17,3,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,19,29,34,41,37,28,14,16,14,29,32,38,34,25,13,10,0,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,1,14,25,38,40,38,25,12,1,9,13,24,27,16,7,0,0,0,0,0,0,0,0,18,29,34,41,37,24,21,11,22,32,26,21,36,35,35,32,30,15,14,28,27,17,3,0,0,0,0,0,0,0,0,0,11,21,24,37,41,38,24,22,15,0,0,0,0,0,0,0,0,0,0,0,0,0,10,21,24,37,41,39,27,23,24,21,10,0,0,0,0,0,0,0,0,0,0,0,0,10,13,28,38,40,38,24,21,11,0,0,0,
                                        0,0,8,15,18,21,32,35,38,25,12,18,18,10,0,0,0,0,0,10,21,32,35,37,33,29,12,12,27,39,41,33,26,19,0,0,0,0,0,10,21,32,35,38,25,12,9,12,31,24,26,17,18,18,2,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,5,14,28,38,35,32,30,36,22,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,23,23,25,33,36,28,31,13,19,3,0,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,8,18,29,34,41,37,36,21,10,18,26,32,22,11,0,0,
                                        0,0,0,0,0,0,0,10,21,36,35,41,33,26,18,31,24,21,14,28,38,41,39,31,17,22,32,30,15,0,0,0,0,0,0,0,0,0,0,6,17,31,33,35,35,33,29,17,7,0,0,0,0,0,0,0,0,0,0,0,0,6,17,31,34,41,37,36,36,31,17,6,0,0,0,0,0,0,0,0,0,0,0,1,20,27,34,35,35,33,29,17,6,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,32,35,37,24,13,9,15,26,33,41,39,27,17,8,0,0,0,0,10,21,32,35,38,25,12,12,31,24,26,17,1,0,0,0,0,0,0,0,10,21,32,35,38,
                                        25,12,6,0,0,0,0,0,0,3,17,31,34,41,39,28,25,14,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,24,28,24,34,39,28,27,13,11,3,0,0,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,0,10,21,24,37,41,34,31,18,12,25,25,14,5,0,0,0,0,0,0,0,0,0,1,14,28,38,40,39,27,12,25,25,12,17,27,39,40,38,25,14,29,24,13,9,0,0,0,0,0,0,0,0,0,0,0,9,14,25,37,40,38,28,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,10,13,
                                        28,38,40,37,28,13,10,0,0,0,0,0,0,0,0,0,0,0,3,18,30,32,37,41,39,23,14,2,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,32,35,37,28,14,1,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,38,25,12,31,33,26,17,1,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,24,37,35,34,29,18,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,21,25,39,37,34,23,12,5,0,0,0,0,0,0,0,0,0,0,0,15,30,33,
                                        41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,0,4,12,23,39,40,38,25,14,21,36,31,18,8,0,0,0,0,0,0,0,0,0,3,17,27,39,40,37,28,21,24,31,18,15,26,33,35,35,36,21,23,25,12,4,0,0,0,0,0,0,0,0,0,0,6,17,29,33,32,37,41,34,27,12,1,0,0,0,0,0,0,0,0,0,0,0,0,7,17,31,34,40,38,25,12,4,0,0,0,0,0,0,0,0,0,0,0,10,13,28,38,40,37,24,21,15,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,32,35,38,25,12,4,11,30,33,41,39,
                                        23,17,3,0,0,0,0,10,21,32,35,38,25,31,33,38,28,21,11,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,4,12,23,39,37,24,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,19,30,32,35,35,32,30,19,0,0,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,0,0,19,26,33,35,35,32,22,31,36,21,10,0,0,0,0,0,0,0,0,0,0,0,15,30,33,35,35,32,26,32,22,11,9,13,24,37,41,33,26,24,31,18,8,0,0,0,
                                        0,0,0,0,0,0,0,11,21,28,25,27,34,41,37,32,30,19,8,0,0,0,0,0,0,0,0,0,0,0,0,18,31,34,40,38,25,12,4,0,0,0,0,0,0,0,0,0,0,1,20,27,34,35,35,33,29,17,6,0,0,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,37,28,24,33,35,34,23,12,5,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,19,26,33,38,23,12,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,19,29,34,41,37,28,14,1,12,25,39,40,39,23,12,1,0,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,0,0,9,13,28,37,41,39,27,28,23,12,4,0,0,0,0,0,0,0,0,0,0,0,9,13,24,37,41,34,27,28,13,5,4,12,25,38,40,39,23,32,22,11,0,0,0,0,0,0,0,0,0,0,5,12,23,36,22,21,28,38,40,38,25,13,9,0,0,0,0,0,0,0,0,0,0,0,0,18,31,34,40,38,25,12,4,0,0,0,0,0,0,0,0,0,3,18,30,36,37,41,39,25,14,9,0,0,1,10,10,10,
                                        8,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,37,24,26,23,39,35,32,26,17,6,0,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,7,7,8,2,13,28,33,26,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,19,30,32,35,35,32,22,15,0,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,0,0,6,16,27,39,40,37,28,32,26,19,0,0,0,0,
                                        0,0,0,0,0,0,0,0,4,12,23,38,40,37,39,23,16,7,8,18,31,34,41,37,34,28,14,5,0,0,0,0,0,0,0,0,0,3,18,30,32,31,16,17,26,33,35,35,33,31,16,4,0,0,0,0,0,0,0,0,0,0,0,18,31,34,40,38,25,12,4,0,0,0,0,0,0,0,0,0,10,13,28,38,40,37,24,22,15,0,0,0,19,22,22,13,2,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,38,25,12,21,28,38,38,24,21,15,0,0,0,0,0,0,10,21,32,35,38,
                                        25,12,6,0,0,0,0,10,20,16,19,7,17,27,24,13,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,29,34,41,37,28,14,4,5,14,25,38,40,39,23,20,1,0,0,0,0,0,0,0,0,0,15,30,33,41,37,24,13,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,22,32,35,41,38,24,13,9,0,0,0,0,0,0,0,0,0,0,0,0,0,18,29,34,41,41,33,26,19,0,0,11,22,32,35,40,39,27,16,7,0,0,0,0,0,0,0,0,0,10,13,28,28,13,10,2,14,23,39,41,37,24,21,15,0,0,0,0,0,0,0,0,0,0,0,18,
                                        31,34,40,38,25,12,4,0,0,0,0,0,0,0,0,1,20,27,34,35,35,33,31,16,4,7,7,7,18,29,36,21,9,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,38,25,12,17,26,32,37,39,23,14,9,0,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,18,29,23,14,5,14,25,23,20,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,18,18,29,34,41,37,28,14,18,15,19,26,33,35,37,36,22,18,18,1,0,0,0,0,0,9,18,18,30,33,
                                        41,37,24,13,18,19,4,0,0,0,0,0,0,0,0,0,0,0,0,5,14,28,38,40,39,27,16,6,0,0,0,0,0,0,0,0,0,0,0,0,0,10,22,32,35,35,36,21,10,0,0,5,14,28,37,41,33,26,19,0,0,0,0,0,0,0,4,19,18,16,31,32,26,18,10,5,18,21,24,37,41,39,23,14,18,15,8,0,0,0,0,0,0,10,18,18,31,34,40,38,25,12,18,19,7,0,0,0,0,0,8,19,30,36,37,40,39,25,14,16,16,16,16,16,16,29,36,21,9,0,0,11,18,21,32,35,38,25,12,18,19,7,0,0,0,0,11,18,21,32,35,38,25,12,18,18,30,33,41,39,
                                        23,17,18,5,0,0,11,18,21,32,35,38,25,12,18,12,27,34,35,33,31,16,18,2,0,0,11,18,21,32,35,38,25,12,18,2,0,0,0,18,29,24,21,14,26,32,30,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,31,31,31,34,35,37,28,31,31,13,2,14,25,38,35,39,27,31,30,11,0,0,0,0,0,18,31,31,31,33,35,37,24,31,31,21,10,0,0,0,0,0,0,0,0,0,0,0,0,3,17,31,34,35,32,30,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,14,28,37,38,25,12,1,0,0,7,16,23,39,35,24,21,9,0,0,0,0,0,
                                        0,0,10,21,31,31,24,23,31,31,16,15,30,31,31,39,35,35,32,31,31,14,1,0,0,0,0,0,3,16,31,31,31,34,35,38,25,31,31,13,5,0,0,0,0,0,6,20,25,38,35,35,33,23,23,23,23,23,23,23,23,23,36,21,9,0,4,12,31,31,32,35,38,25,31,31,13,2,0,0,0,4,12,31,31,32,35,38,25,31,22,31,31,33,35,39,23,31,30,11,0,4,12,31,31,32,35,38,25,31,22,30,31,28,37,37,24,31,29,19,0,4,12,31,31,32,35,38,25,31,29,19,0,0,0,18,29,33,24,28,32,23,12,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,18,31,24,24,24,24,24,24,24,24,13,5,18,26,24,24,24,24,24,30,11,0,0,0,0,0,18,31,24,24,24,24,24,24,24,24,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,10,21,24,24,28,14,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,16,27,24,24,31,17,8,0,0,0,19,26,24,24,25,12,1,0,0,0,0,0,0,0,10,21,24,24,24,24,24,23,16,15,30,24,24,24,24,24,24,24,28,14,1,0,0,0,0,0,3,16,23,24,24,24,24,24,24,24,24,13,5,0,0,0,0,0,6,20,25,24,24,24,24,24,24,24,24,24,24,24,24,24,24,21,
                                        9,0,4,12,25,24,24,24,24,24,24,24,13,2,0,0,0,4,12,25,24,24,24,24,24,24,22,23,24,24,24,24,24,24,30,11,0,4,12,25,24,24,24,24,24,24,22,30,24,24,24,24,24,24,29,19,0,4,12,25,24,24,24,24,24,24,29,19,0,0,0,15,13,30,31,27,26,14,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,13,13,13,13,13,13,13,13,13,20,1,2,14,13,13,13,13,13,13,10,0,0,0,0,0,15,13,13,13,13,13,13,13,13,13,14,9,0,0,0,0,0,0,0,0,0,0,0,0,0,1,20,13,13,13,17,3,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,15,13,13,13,13,11,0,0,0,0,9,14,13,13,13,18,8,0,0,0,0,0,0,0,9,14,13,13,13,13,13,13,18,10,13,13,13,13,13,13,13,13,13,20,4,0,0,0,0,0,8,18,13,13,13,13,13,13,13,13,13,12,1,0,0,0,0,0,3,17,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,5,0,6,16,13,13,13,13,13,13,13,13,12,5,0,0,0,6,16,13,13,13,13,13,13,13,14,13,13,13,13,13,13,13,13,10,0,6,16,13,13,13,13,13,13,13,14,13,13,13,13,13,13,13,13,15,0,6,16,13,13,13,13,13,
                                        13,13,13,15,0,0,0,7,9,15,18,17,19,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,5,5,5,5,5,5,5,5,5,1,0,0,5,5,5,5,5,5,5,8,0,0,0,0,0,7,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,5,5,5,5,3,0,0,0,0,0,5,5,5,5,6,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,6,8,5,5,5,5,5,5,5,5,5,1,0,0,0,0,0,0,0,6,5,5,
                                        5,5,5,5,5,5,5,1,0,0,0,0,0,0,0,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,4,5,5,5,5,5,5,5,5,1,0,0,0,0,0,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,8,0,0,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,0,0,4,5,5,5,5,5,5,5,5,3,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,8,8,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,5,5,5,6,4,5,5,5,5,5,5,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,2,2,2,2,2,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,11,17,17,17,17,15,8,0,0,0,0,0,8,1,10,15,18,17,17,18,19,10,1,0,0,0,0,0,0,0,0,0,0,0,0,8,15,17,17,17,17,17,2,0,0,0,0,0,2,10,10,10,10,10,10,10,10,10,10,10,10,10,7,0,0,0,0,0,0,0,0,4,9,10,11,10,9,4,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,8,5,11,18,17,17,18,19,11,9,8,0,0,0,0,0,0,0,4,9,11,15,15,10,9,4,8,0,0,0,1,20,13,13,13,13,13,13,18,16,13,13,13,13,13,13,15,0,0,0,0,0,0,0,0,0,0,4,10,10,10,10,7,0,0,0,0,0,0,0,0,0,0,2,18,17,19,1,0,0,0,0,0,0,0,0,0,8,1,9,10,9,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,1,9,10,9,5,6,0,0,0,0,0,0,0,0,0,0,10,13,13,13,13,13,19,0,0,0,0,0,0,0,0,0,3,9,11,10,6,0,0,0,0,3,9,11,10,6,0,0,0,0,
                                        0,0,0,0,5,17,21,27,27,27,27,12,6,0,0,0,0,7,18,14,21,30,31,27,27,31,26,21,12,11,7,0,0,0,0,0,0,0,0,0,0,2,14,27,27,27,27,26,19,0,0,0,0,6,20,22,22,22,22,22,22,22,22,22,22,22,22,21,10,0,0,0,0,0,0,4,15,20,13,22,30,22,21,12,15,1,0,0,0,0,0,0,0,0,0,0,4,19,14,30,29,27,27,31,26,22,13,9,0,0,0,0,0,7,11,20,21,22,26,30,22,13,12,18,9,0,0,5,14,28,24,24,24,24,23,16,14,28,24,24,24,24,31,18,0,0,0,0,0,0,0,0,0,0,19,22,22,22,21,11,
                                        0,0,0,0,0,0,0,0,0,2,12,26,27,30,17,4,0,0,0,0,0,0,8,9,18,14,21,21,21,13,16,11,4,0,0,0,0,0,0,0,0,0,0,8,9,18,14,21,21,21,13,16,11,4,0,0,0,0,0,0,0,0,15,26,24,24,24,27,17,8,0,0,0,0,0,0,0,7,19,13,30,21,17,4,0,0,7,19,13,30,21,17,4,0,0,0,0,0,7,11,12,26,28,39,39,39,25,12,6,0,0,0,0,1,14,25,36,33,24,28,24,34,34,36,23,21,19,7,0,0,0,0,0,0,0,0,3,18,26,33,39,39,34,26,19,0,0,0,0,6,12,25,33,33,33,33,33,33,33,33,
                                        33,33,33,22,11,0,0,0,0,0,5,16,22,23,24,32,33,32,24,23,22,16,9,0,0,0,0,0,0,0,0,6,18,22,25,32,34,36,28,28,34,32,21,10,0,0,0,0,7,19,21,27,24,33,33,36,32,24,25,29,20,6,0,5,14,31,31,33,37,38,23,16,14,31,31,34,37,39,31,18,0,0,0,0,0,0,0,0,0,6,20,23,33,33,31,17,3,0,0,0,0,0,0,0,0,18,26,32,39,24,22,11,0,0,0,0,0,6,19,13,29,25,24,32,36,28,27,21,17,2,0,0,0,0,0,0,0,0,6,19,13,29,25,24,32,36,28,27,21,17,2,0,0,0,0,0,0,0,15,26,
                                        33,35,39,27,17,8,0,0,0,0,0,0,0,10,21,28,33,28,22,15,0,0,10,21,28,33,28,22,15,0,0,0,0,7,18,21,23,32,25,34,41,38,25,12,6,0,0,0,0,1,14,28,34,27,21,14,21,27,34,37,39,28,21,10,0,0,0,0,0,0,0,0,10,13,28,38,41,41,34,26,19,0,0,0,0,6,12,25,38,35,35,35,35,35,35,35,35,35,33,22,11,0,0,0,0,5,20,29,24,33,28,23,27,23,28,33,36,29,12,9,0,0,0,0,0,0,0,10,21,28,39,37,24,22,14,13,23,32,21,10,0,0,0,0,15,21,28,34,38,28,30,21,21,23,34,25,12,6,0,3,
                                        15,18,30,33,41,38,23,16,11,18,29,34,41,39,31,18,0,0,0,0,0,0,0,0,0,9,21,24,37,38,28,14,5,0,0,0,0,0,0,0,7,16,23,38,41,34,31,18,0,0,0,0,6,18,21,25,33,32,28,25,25,24,33,24,29,20,2,0,0,0,0,0,0,6,18,21,25,33,32,28,25,25,24,33,24,29,20,2,0,0,0,0,0,1,15,26,33,41,39,27,17,5,5,4,0,0,0,0,0,18,29,34,35,39,27,16,3,0,18,29,34,35,39,27,16,3,0,0,0,4,12,25,36,31,26,34,41,38,25,12,6,0,0,0,0,1,14,28,28,13,10,1,10,13,28,37,35,34,27,16,
                                        7,0,0,0,0,0,0,6,17,31,33,28,37,41,34,26,19,0,0,0,0,6,12,25,33,33,33,33,33,33,33,33,33,39,33,22,11,0,0,0,4,17,29,32,24,29,13,20,17,16,13,26,28,32,29,16,1,0,0,0,0,0,8,17,31,34,41,39,27,16,1,15,30,32,21,10,0,0,0,1,12,23,39,35,33,26,18,9,11,13,28,25,12,6,0,0,0,11,30,33,41,38,23,16,3,19,29,34,41,39,31,18,0,0,0,0,0,0,0,0,0,18,29,33,41,35,32,30,15,0,0,0,0,0,0,0,3,17,27,39,35,33,26,19,0,0,0,8,19,21,28,33,23,22,14,12,12,13,29,24,
                                        32,29,16,1,0,0,0,0,8,19,21,28,33,23,22,14,12,12,13,29,24,32,29,16,1,0,0,0,5,12,13,26,33,41,39,27,13,13,13,16,6,0,0,0,0,18,31,34,41,39,23,16,3,0,18,31,34,41,39,23,16,3,0,0,0,4,12,25,22,16,26,34,41,38,25,12,6,0,0,0,0,1,14,28,27,17,3,0,7,16,27,39,40,37,28,14,5,0,0,0,0,0,0,10,21,24,25,25,38,41,34,26,19,0,0,0,0,6,12,25,25,22,22,22,22,22,22,22,26,33,28,13,9,0,0,0,15,22,24,24,22,17,19,5,8,1,15,17,21,28,36,30,19,0,0,0,0,0,6,20,
                                        25,38,41,34,29,19,0,10,21,26,21,10,0,0,0,10,22,36,35,37,24,13,2,0,3,16,27,25,12,6,0,0,0,11,30,33,41,38,23,16,3,19,29,34,41,39,31,18,0,0,0,0,0,0,0,0,1,12,25,34,37,40,39,27,16,6,0,0,0,0,0,0,0,11,21,25,36,23,13,9,0,0,0,9,14,25,33,31,14,13,30,29,29,30,13,22,28,36,30,19,8,0,0,0,9,14,25,33,31,21,21,21,21,21,21,20,22,28,36,30,19,8,0,0,2,13,24,24,34,41,38,24,24,24,25,12,4,0,0,0,0,10,21,28,34,24,30,19,0,0,10,21,28,34,24,30,19,0,0,0,
                                        0,6,16,14,19,19,26,34,41,38,25,12,6,0,0,0,0,1,14,30,30,19,0,0,0,18,31,34,40,35,24,13,2,0,0,0,0,0,4,20,27,32,30,25,38,41,34,26,19,0,0,0,0,6,12,25,25,20,10,10,10,10,10,12,25,34,31,17,7,0,0,1,12,23,32,26,12,26,26,20,1,17,22,29,14,30,36,25,14,5,0,0,0,0,4,12,25,37,41,34,26,15,0,6,11,15,11,7,0,0,0,18,29,34,41,37,25,12,4,0,0,18,30,30,20,6,0,0,0,11,30,33,41,38,23,16,3,19,29,34,41,39,31,18,0,0,0,0,0,0,0,0,10,21,36,27,34,41,37,24,
                                        13,9,0,0,0,0,0,0,0,6,19,14,21,14,11,8,0,0,3,17,29,33,27,12,22,28,33,25,25,32,28,29,30,36,25,14,5,0,0,3,17,29,33,27,29,28,32,28,28,36,24,27,14,30,36,25,14,5,0,0,2,13,31,31,33,41,39,27,31,31,31,12,4,0,0,0,0,6,18,21,26,22,16,1,0,0,6,18,21,26,22,16,1,0,0,0,0,0,1,5,8,19,26,34,41,38,25,12,6,0,0,0,0,0,10,11,11,1,0,0,0,18,29,34,40,35,24,13,2,0,0,0,0,0,15,22,36,23,12,25,38,41,34,26,19,0,0,0,0,6,12,23,23,20,6,0,0,0,15,22,
                                        32,24,21,10,0,0,0,10,21,36,23,12,30,32,36,21,10,13,24,33,29,20,27,32,22,11,0,0,0,0,4,12,28,37,41,34,26,15,0,0,0,0,0,0,0,0,8,17,27,39,40,38,25,20,6,0,0,1,11,11,9,0,0,0,0,11,30,33,41,38,23,16,3,19,29,34,41,39,31,18,0,0,0,0,0,0,0,3,17,31,24,22,32,35,35,33,26,18,0,0,0,0,0,0,0,0,19,29,27,21,10,0,0,0,5,14,28,24,21,21,28,34,27,14,12,26,33,29,20,27,32,30,15,0,0,5,14,28,24,21,14,31,39,23,14,22,24,33,30,20,27,32,30,15,0,0,7,19,18,26,
                                        33,41,39,27,17,18,18,11,8,0,0,0,0,0,6,10,19,11,4,0,0,0,0,6,10,19,11,4,0,0,0,0,0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,8,17,27,39,40,38,25,14,1,0,0,0,0,5,12,23,36,22,12,25,38,41,34,26,19,0,0,0,0,8,19,16,16,19,8,0,0,4,20,23,39,27,20,4,0,0,0,18,29,33,26,19,26,33,36,21,10,13,24,34,29,18,30,32,31,18,8,0,0,1,5,12,28,37,41,34,26,15,5,5,5,0,0,0,0,0,3,16,23,39,40,38,25,20,7,0,0,0,0,0,0,0,0,0,
                                        0,11,30,33,41,38,23,16,3,19,29,34,41,39,31,18,0,0,0,0,0,0,0,5,14,28,23,14,25,38,40,38,25,12,1,0,0,0,0,0,0,0,19,29,32,21,10,0,0,0,10,22,32,27,16,31,34,24,13,10,5,14,28,29,19,22,32,27,17,3,0,10,22,32,27,16,18,31,39,23,16,12,25,34,26,19,22,32,27,17,3,0,0,0,15,26,33,41,39,27,17,8,0,0,0,0,0,0,0,0,4,2,2,2,2,3,0,0,0,7,10,15,10,6,0,0,0,0,0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,1,12,25,38,35,33,29,18,
                                        8,0,0,0,8,19,30,32,27,20,12,25,38,41,34,26,19,0,0,0,0,0,8,7,7,0,0,0,0,10,21,24,36,22,11,0,0,0,3,17,27,36,21,10,14,29,26,20,1,16,26,31,13,10,13,24,23,16,7,0,1,20,13,13,28,37,41,34,26,13,13,13,14,9,0,0,0,0,8,17,27,39,40,38,25,20,6,0,0,1,9,9,2,0,0,0,0,11,30,33,41,38,23,16,7,18,29,34,41,39,31,18,0,0,0,0,0,0,0,15,30,32,26,18,29,34,41,37,36,21,10,0,0,0,0,0,0,0,19,29,32,21,10,0,0,0,19,26,33,26,12,25,38,25,12,4,6,16,22,22,
                                        19,13,28,25,12,6,0,19,26,33,26,19,18,31,39,23,12,21,28,32,22,11,13,28,25,12,6,0,0,0,15,26,33,41,39,27,17,8,0,0,0,0,0,0,0,7,17,13,13,13,13,11,0,0,7,18,21,30,21,17,1,0,0,0,0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,15,22,36,35,34,23,14,9,0,0,0,0,2,14,25,24,21,11,12,25,38,41,34,26,19,0,0,0,0,0,0,0,0,0,0,0,7,17,27,34,23,12,1,0,0,0,7,16,23,24,13,11,13,13,19,2,0,5,19,13,21,19,14,28,25,20,6,0,5,13,24,24,
                                        36,37,41,34,24,24,24,24,22,10,0,0,0,0,0,18,29,34,41,37,25,14,4,0,3,17,21,21,14,9,0,0,0,11,30,33,41,38,25,12,6,16,27,39,40,39,31,18,0,0,0,0,0,0,6,16,27,28,13,10,21,24,37,41,34,31,17,8,0,0,0,0,0,0,18,31,33,22,11,0,0,0,19,29,33,30,14,28,38,23,16,3,0,5,10,10,1,14,28,28,14,4,0,19,29,33,30,11,18,31,39,28,25,24,25,29,20,1,14,28,28,14,4,0,0,0,15,26,33,41,39,27,17,8,0,0,0,0,0,0,0,6,20,25,24,24,26,19,0,0,10,21,28,33,24,22,19,0,0,0,
                                        0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,0,0,0,0,0,10,14,23,39,34,23,13,11,0,0,0,0,3,18,26,33,31,16,4,12,25,38,41,34,26,19,0,0,0,0,0,0,0,0,0,0,0,9,13,28,32,30,19,0,0,0,0,3,16,23,24,13,15,26,27,16,6,0,3,18,31,29,18,14,28,25,20,6,0,5,13,27,27,28,37,41,34,27,27,27,27,22,10,0,0,0,0,0,10,22,36,35,37,24,13,2,0,4,12,25,24,21,9,0,0,0,11,30,33,41,37,28,13,20,21,24,37,40,39,31,18,19,4,0,0,0,0,9,13,28,27,16,4,20,23,39,40,
                                        38,25,14,5,0,0,0,0,0,3,16,23,34,26,19,0,0,0,19,29,33,30,14,28,38,23,16,3,0,1,9,9,1,14,28,28,14,4,0,19,29,33,30,11,18,31,39,23,26,28,24,30,19,1,14,28,28,14,4,0,0,0,15,26,33,41,39,27,17,8,0,0,0,0,0,0,0,6,12,25,38,33,30,15,0,0,18,29,34,41,39,23,16,3,0,0,0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,0,0,0,0,10,14,27,33,32,27,13,15,3,0,0,0,0,9,13,28,28,13,19,19,12,25,38,41,34,26,19,19,15,6,0,0,0,0,0,0,0,3,18,29,34,25,
                                        14,5,0,0,0,0,8,17,27,32,21,10,21,24,21,19,2,11,13,28,30,11,21,24,23,16,3,0,3,19,18,12,28,37,41,34,26,18,18,18,18,1,0,0,0,0,0,1,12,23,39,35,33,26,18,9,15,21,24,23,20,4,0,0,0,11,30,33,41,35,34,28,23,28,27,34,35,39,31,31,21,10,0,0,0,0,19,26,32,22,15,0,19,26,33,35,35,32,22,11,0,0,0,0,0,6,12,25,39,31,18,0,0,0,15,26,33,26,12,25,38,25,12,4,6,16,21,21,17,13,24,25,20,6,0,15,26,33,26,19,18,31,39,23,16,31,34,25,12,2,13,24,25,20,6,0,0,0,15,26,
                                        33,41,39,27,17,8,0,0,0,0,0,0,0,1,14,28,37,32,21,10,0,0,18,29,34,41,39,23,16,3,0,0,0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,0,0,3,11,14,27,33,24,29,12,10,15,15,11,7,0,3,17,31,34,29,26,26,26,26,25,38,41,34,26,26,26,21,10,0,0,0,0,0,0,0,2,14,28,33,29,18,8,0,0,0,0,0,19,26,33,29,18,12,25,25,22,13,21,23,25,14,19,26,33,29,18,0,0,0,0,4,12,28,37,41,34,26,15,0,0,0,0,0,0,0,0,0,0,15,21,28,34,38,28,30,13,22,25,24,22,15,0,0,0,
                                        0,11,30,33,41,38,28,33,33,25,29,24,24,24,24,24,21,10,0,0,0,4,12,23,25,14,5,0,9,13,28,37,41,39,27,16,7,0,0,0,0,1,14,28,39,23,16,3,0,0,10,21,36,23,20,29,34,24,21,10,9,13,28,27,16,30,32,27,17,8,0,10,21,36,23,20,18,31,39,23,16,21,24,36,22,15,30,32,27,17,8,0,0,0,15,26,33,41,39,27,17,9,10,10,5,0,0,0,0,10,21,36,38,28,14,5,0,0,10,21,28,34,24,30,19,0,0,0,0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,0,1,18,21,23,33,25,22,17,2,19,26,26,21,
                                        9,0,3,16,23,32,32,32,32,32,32,33,37,40,39,32,32,32,22,10,0,0,0,0,0,0,0,19,26,33,28,13,9,0,0,0,0,0,0,9,21,24,25,13,15,21,25,32,24,36,28,22,19,14,25,36,21,10,0,0,0,0,4,12,28,37,41,34,26,15,0,7,19,18,18,2,0,0,0,0,7,19,21,27,24,33,32,36,32,28,30,17,4,0,0,0,0,11,30,33,41,38,23,30,30,13,13,13,13,13,13,13,14,9,0,0,0,10,21,24,31,17,8,0,6,16,27,39,40,37,28,13,2,0,0,0,0,2,13,24,38,25,12,6,0,0,1,14,25,24,21,13,25,34,23,13,13,31,36,22,
                                        12,23,32,22,11,0,0,1,14,25,24,21,30,31,39,23,30,20,23,39,27,22,23,32,22,11,0,0,0,0,15,26,33,41,39,27,17,14,22,22,16,7,0,0,1,16,31,34,34,31,17,3,0,0,6,18,21,26,22,17,1,0,0,0,0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,0,2,16,22,28,32,27,13,19,10,10,19,29,36,21,9,0,3,17,22,22,22,22,22,22,22,25,38,41,34,26,22,22,21,10,0,0,0,0,0,0,5,14,25,34,31,17,3,0,0,0,0,0,0,6,16,27,33,31,20,19,14,30,29,30,13,18,16,29,33,23,20,4,0,0,0,0,4,12,
                                        28,37,41,34,26,15,0,2,13,31,29,18,0,0,0,0,0,7,11,20,21,30,26,24,29,14,18,1,0,0,0,0,0,11,30,33,41,38,23,16,15,9,5,5,5,5,5,5,5,0,0,0,8,18,29,36,21,10,0,0,0,15,22,32,35,35,33,26,19,0,0,0,0,10,21,36,37,28,14,1,0,0,8,18,26,33,23,14,21,25,32,24,28,24,29,20,26,32,25,14,5,0,0,8,18,26,33,23,29,23,23,23,23,16,22,23,23,26,32,25,14,5,0,0,0,0,11,30,33,35,39,27,17,13,24,23,16,7,0,6,17,30,36,39,28,13,10,0,0,0,0,7,10,15,11,4,0,0,0,0,
                                        0,0,0,0,0,19,26,34,41,38,25,12,6,0,0,0,0,5,12,29,24,28,26,22,22,22,22,22,22,29,36,21,9,0,0,5,10,10,10,10,10,10,12,25,38,41,34,26,19,10,10,3,0,0,0,0,0,0,15,22,32,24,21,10,0,0,0,0,0,0,0,0,10,13,28,32,29,12,11,11,19,15,11,20,26,36,28,21,11,0,0,0,0,0,4,12,28,37,41,34,26,15,7,2,13,36,31,18,0,0,0,0,0,0,0,9,15,11,16,27,24,21,9,0,0,0,0,0,10,11,30,33,41,38,23,16,11,4,0,0,0,0,0,0,0,0,0,0,5,14,25,25,12,11,11,11,11,11,14,28,
                                        38,40,39,23,20,4,0,0,0,11,22,33,37,24,13,2,0,0,0,2,14,23,33,27,13,14,22,26,30,21,20,30,24,24,22,19,0,0,0,0,2,14,23,33,27,13,20,20,20,15,17,20,30,24,24,22,19,0,0,0,0,0,9,21,24,37,38,25,21,26,32,29,18,0,0,5,14,25,39,28,22,18,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,18,18,18,26,34,41,38,25,12,18,18,15,8,0,9,21,36,37,33,33,33,33,33,33,33,33,33,36,21,9,0,0,0,0,0,0,1,18,18,12,25,38,41,34,26,18,18,11,8,0,0,0,0,0,4,20,23,39,27,16,6,
                                        0,0,0,0,0,0,0,0,3,18,22,28,32,23,22,14,12,14,21,27,32,24,30,18,6,0,0,0,3,15,18,12,28,37,41,34,26,16,16,16,13,36,31,18,0,0,0,0,0,0,7,16,30,13,13,27,33,30,15,0,0,0,0,1,14,30,30,33,35,38,23,30,22,11,0,0,0,0,0,0,0,0,0,0,11,22,32,26,30,30,30,30,30,30,30,31,34,41,37,24,21,10,0,0,0,19,26,34,35,32,21,10,0,0,0,0,11,21,25,33,25,26,13,14,13,22,27,36,36,26,17,4,0,0,0,0,0,11,21,25,33,25,26,13,14,13,22,27,36,36,26,17,4,0,0,0,0,0,4,16,
                                        31,24,33,34,36,32,25,13,10,0,0,8,19,22,25,22,18,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,31,31,31,31,34,35,38,25,31,31,31,14,1,0,9,21,36,35,35,35,35,35,35,35,35,35,35,36,21,9,0,0,0,0,0,0,10,22,31,31,25,38,35,34,31,31,31,12,4,0,0,0,0,0,10,21,36,36,22,11,0,0,0,0,0,0,0,0,0,0,4,18,22,25,33,32,28,28,28,36,33,28,22,17,1,0,0,0,0,5,13,31,31,28,37,35,34,23,23,23,23,23,36,31,18,0,0,0,0,0,0,3,16,23,24,28,33,32,22,11,0,0,0,0,5,
                                        14,28,32,32,32,32,32,32,30,15,0,0,0,0,0,0,0,0,0,7,17,27,39,33,33,33,33,33,33,33,33,33,38,35,35,34,29,18,8,0,0,18,31,39,35,33,30,11,0,0,0,0,3,19,21,23,32,33,24,28,28,32,34,28,30,16,5,0,0,0,0,0,0,3,19,21,23,32,33,24,28,28,32,34,28,30,16,5,0,0,0,0,0,0,0,9,20,21,30,26,26,22,14,15,8,0,0,0,4,17,13,19,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,31,24,24,24,24,24,24,24,24,24,28,14,1,0,9,21,24,24,24,24,24,24,24,24,24,24,24,24,21,
                                        9,0,0,0,0,0,0,10,22,24,24,24,24,24,24,24,24,25,12,4,0,0,0,0,7,16,27,24,23,12,1,0,0,0,0,0,0,0,0,0,0,0,4,19,13,29,25,28,24,28,25,29,13,18,1,0,0,0,0,0,5,13,24,24,24,24,24,24,24,24,24,24,24,24,31,18,0,0,0,0,0,0,3,16,27,25,28,25,29,12,1,0,0,0,0,1,14,22,22,22,22,22,22,22,21,11,0,0,0,0,0,0,0,0,0,2,13,28,24,24,24,24,24,24,24,24,24,24,24,24,24,24,25,14,1,0,3,16,23,24,24,24,26,19,0,0,0,0,0,3,11,14,30,23,28,24,24,25,31,13,
                                        18,1,0,0,0,0,0,0,0,0,3,11,14,30,23,28,24,24,25,31,13,18,1,0,0,0,0,0,0,0,0,0,4,10,15,15,15,10,5,0,0,0,0,0,0,1,2,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,13,13,13,13,13,13,13,13,13,13,13,20,4,0,5,12,13,13,13,13,13,13,13,13,13,13,13,13,14,2,0,0,0,0,0,0,10,14,13,13,13,13,13,13,13,13,13,16,6,0,0,0,0,7,17,13,13,13,15,0,0,0,0,0,0,0,0,0,0,0,0,0,8,9,18,12,13,13,13,12,18,9,3,0,0,0,0,0,0,1,20,13,13,
                                        13,13,13,13,13,13,13,13,13,13,13,15,0,0,0,0,0,0,0,11,16,12,14,12,17,2,0,0,0,0,0,0,9,10,10,10,10,10,10,10,10,6,0,0,0,0,0,0,0,0,0,2,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,1,0,3,17,13,13,13,13,13,15,0,0,0,0,0,0,0,5,19,20,14,13,13,12,17,10,7,0,0,0,0,0,0,0,0,0,0,0,5,19,20,14,13,13,12,17,10,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,7,5,5,5,5,5,5,5,5,5,5,5,1,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,5,5,5,4,0,0,0,0,0,0,4,5,5,5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,4,5,5,5,4,8,0,0,0,0,0,0,0,0,0,1,5,5,5,5,5,5,5,5,5,5,5,5,5,7,0,0,0,0,0,0,0,0,7,4,1,4,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,
                                        5,5,5,5,5,1,0,0,0,4,5,5,5,5,5,3,0,0,0,0,0,0,0,0,0,6,5,5,5,4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,5,5,5,4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,3,6,5,5,5,4,0,0,0,0,0,0,0,0,0,0,0,7,7,7,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,6,0,0,0,1,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,20,13,13,13,15,18,13,13,13,17,7,0,0,0,0,0,0,0,0,0,9,16,16,16,17,1,0,0,0,0,0,0,0,0,0,0,7,5,10,11,15,15,11,9,4,0,0,0,0,0,0,0,0,0,0,0,0,4,9,11,15,15,10,9,4,8,0,0,0,0,0,0,0,0,4,9,11,15,15,10,1,0,0,0,0,0,0,0,0,0,0,0,6,9,11,15,15,10,2,6,0,0,0,0,0,0,0,0,0,0,1,10,11,15,15,11,9,1,3,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,2,19,12,16,4,0,2,12,16,15,5,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,16,12,10,8,0,0,0,0,0,0,0,0,0,0,4,5,5,5,5,5,5,4,2,11,15,11,2,8,0,4,10,15,15,10,1,0,0,0,0,0,0,0,4,5,5,5,5,5,5,4,2,11,15,11,9,7,0,0,0,0,0,0,1,5,5,5,5,5,5,4,5,11,15,11,10,4,0,0,0,0,0,0,0,0,0,0,10,21,24,24,26,19,12,25,24,23,20,6,0,0,0,0,0,0,0,0,0,19,26,23,23,
                                        21,10,0,0,0,0,0,0,0,0,0,11,16,14,21,30,26,26,22,13,20,11,7,0,0,0,0,0,0,0,0,7,11,20,21,22,26,30,22,13,12,18,9,0,0,0,0,0,7,11,20,21,30,26,30,21,12,15,6,0,0,0,0,0,0,0,3,10,16,13,22,26,26,22,13,16,10,8,0,0,0,0,0,0,4,19,12,21,30,26,30,22,21,14,17,10,0,0,0,0,0,0,0,0,0,0,0,0,1,10,17,13,26,23,14,1,0,10,22,27,22,14,18,10,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,11,14,31,27,13,15,6,0,0,0,0,0,0,0,0,7,17,
                                        13,13,13,13,13,13,17,13,30,26,22,13,18,9,16,21,30,26,21,12,10,0,0,0,0,0,7,17,13,13,13,13,13,13,17,13,22,26,30,13,18,1,0,0,0,0,4,20,13,13,13,13,13,13,17,14,22,26,30,21,18,0,0,0,0,0,0,0,0,0,0,15,26,33,32,21,10,13,24,34,31,18,0,0,0,0,0,0,0,0,0,0,19,26,34,32,21,10,0,0,0,0,0,0,0,0,6,20,27,28,32,36,36,32,32,24,23,21,19,3,0,0,0,0,0,0,7,19,21,27,24,33,33,36,32,24,25,29,20,6,0,0,0,6,18,21,23,24,33,36,33,36,23,21,18,6,0,0,0,0,
                                        0,3,19,13,27,24,32,36,36,32,28,31,13,15,8,0,0,0,0,7,18,22,25,36,33,36,36,32,36,28,27,17,8,0,0,0,0,0,0,0,0,3,2,19,12,21,31,28,33,28,14,1,0,10,22,32,32,25,29,21,16,15,5,8,0,0,0,0,0,0,0,0,0,0,0,0,0,4,19,13,23,33,34,25,21,18,1,0,0,0,0,0,0,0,4,12,25,24,24,24,24,25,22,28,33,33,33,28,22,12,31,24,33,33,36,27,14,9,0,0,0,0,4,12,25,24,24,24,24,25,22,28,33,33,33,28,30,17,7,0,0,0,1,14,28,24,24,24,24,25,21,25,32,33,33,27,18,0,0,0,
                                        0,0,0,0,0,0,8,17,27,39,28,14,10,22,32,33,30,11,0,0,0,0,0,0,0,0,0,0,19,26,34,32,21,10,0,0,0,0,0,0,0,0,6,20,25,33,31,21,13,30,28,38,39,25,13,10,0,0,0,0,0,0,15,21,28,34,38,28,30,21,21,23,34,25,12,6,0,0,0,15,21,28,39,33,31,21,26,32,39,28,21,15,0,0,0,0,0,11,13,25,34,34,23,21,21,23,39,34,25,13,10,0,0,0,0,10,21,28,39,33,31,21,21,22,23,34,27,17,8,0,0,0,0,0,0,1,10,17,13,26,23,36,34,37,39,28,14,1,0,10,22,32,39,38,34,24,27,22,14,18,
                                        10,6,0,0,0,0,0,0,0,0,0,0,5,17,22,25,34,35,35,39,28,30,16,9,0,0,0,0,0,0,4,12,31,31,32,37,38,25,28,25,25,33,35,39,25,31,24,23,28,39,37,33,29,17,3,0,0,0,4,12,31,31,32,37,38,25,28,25,25,32,37,38,28,13,9,0,0,0,1,14,31,31,32,37,38,25,25,28,23,28,39,27,18,0,0,0,0,0,0,9,11,11,11,12,25,38,23,20,18,29,34,24,13,11,11,9,0,0,0,0,0,0,0,0,19,26,34,32,21,10,0,0,0,0,0,0,0,0,6,20,25,25,14,10,9,18,30,33,35,34,29,18,8,0,0,0,0,1,12,23,
                                        39,35,33,26,18,9,11,13,28,25,12,6,0,0,1,12,23,39,37,24,13,9,14,25,38,39,23,12,1,0,0,0,1,12,23,34,35,36,21,11,15,22,32,35,34,27,20,4,0,0,0,18,29,34,37,24,13,10,9,11,13,28,27,17,8,0,0,0,3,2,19,12,21,31,28,33,39,38,33,28,27,22,12,1,0,9,13,26,23,24,34,38,39,32,25,29,21,16,15,5,8,0,0,0,0,0,8,10,12,26,24,39,35,40,40,41,38,36,31,14,11,3,0,0,0,0,0,11,18,21,32,35,37,33,29,12,14,25,38,35,33,24,21,20,21,36,35,38,25,14,1,0,0,0,0,11,18,21,32,35,
                                        37,33,29,12,12,27,39,41,33,26,19,0,0,0,3,15,18,21,32,35,38,28,25,13,20,21,24,27,18,0,0,0,0,0,6,20,30,30,30,30,24,34,31,30,30,23,39,25,30,30,30,20,6,0,0,0,0,0,0,0,19,26,34,32,21,10,0,0,0,0,0,0,0,0,6,20,30,30,18,9,9,10,21,36,35,38,23,20,6,0,0,0,0,10,22,36,35,37,24,13,2,0,3,16,27,25,12,6,0,0,11,22,32,35,37,28,14,1,16,23,39,35,32,22,11,0,0,0,10,21,36,37,37,28,14,5,2,13,24,37,37,24,21,10,0,0,8,17,27,39,35,36,22,20,19,10,16,23,27,17,
                                        8,0,7,10,17,13,26,23,36,34,38,34,36,25,26,13,16,15,5,0,0,8,2,19,20,21,31,25,32,39,38,34,24,27,22,14,18,9,0,0,0,7,15,13,27,32,38,41,40,40,40,40,40,37,33,23,13,19,4,0,0,0,0,0,10,21,32,35,37,24,13,9,17,27,39,40,39,27,16,1,14,28,37,37,24,13,2,0,0,0,0,0,10,21,32,35,37,24,13,9,15,26,33,41,39,27,17,8,0,0,0,0,10,21,32,35,35,32,30,19,6,20,25,27,18,0,0,0,0,0,6,12,25,33,33,33,34,38,33,33,33,33,37,33,33,33,25,12,6,0,7,10,11,11,11,11,19,26,34,32,
                                        21,11,11,11,11,11,10,0,0,0,0,9,11,17,14,21,21,21,21,36,35,37,28,12,4,0,0,0,0,18,29,34,41,37,25,12,4,0,0,18,30,30,20,6,0,0,18,29,34,41,38,25,20,15,17,27,39,41,34,29,18,0,0,0,18,29,34,41,38,25,12,4,1,14,28,37,41,34,26,19,0,0,8,17,27,39,41,39,24,23,26,21,12,20,20,11,0,0,10,21,31,28,33,39,39,33,28,31,21,12,18,9,6,0,0,0,0,0,0,0,4,10,18,14,22,27,24,33,38,39,32,25,29,14,1,0,1,18,21,25,33,38,38,38,38,38,38,38,38,38,38,34,25,22,17,5,0,0,0,0,
                                        10,21,32,35,38,25,12,4,18,31,39,40,34,31,18,4,14,28,37,35,36,21,9,0,0,0,0,0,10,21,32,35,37,28,14,1,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,37,28,13,2,8,19,20,20,11,0,0,0,0,0,6,12,25,34,34,34,37,34,34,34,34,38,39,34,34,34,25,12,6,0,10,21,30,30,30,30,30,26,34,32,30,30,30,30,30,30,14,1,0,0,8,15,13,31,28,36,32,32,32,34,35,37,28,14,4,0,0,0,8,17,27,39,40,38,25,20,6,0,0,1,11,11,9,0,0,8,17,27,39,40,38,25,26,26,26,27,39,35,39,23,16,3,0,8,
                                        17,27,39,40,38,25,20,6,4,14,28,37,40,39,31,18,8,0,0,19,30,33,35,41,37,39,33,36,23,30,20,2,0,0,10,22,32,38,34,24,23,30,14,17,10,1,0,0,0,0,0,0,0,0,0,0,0,0,8,5,15,16,13,26,25,36,38,38,28,14,1,0,18,30,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,26,17,3,0,0,0,10,21,32,35,38,25,12,6,18,31,39,41,34,29,19,4,12,28,37,35,36,21,9,0,0,0,0,0,10,21,32,35,38,25,12,4,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,37,25,12,4,0,8,6,6,0,0,0,0,
                                        0,0,6,20,26,26,26,23,38,25,26,26,26,33,36,26,26,26,26,12,6,0,10,22,32,33,33,33,33,33,38,39,33,33,33,33,33,28,14,1,0,0,9,14,25,34,38,36,31,26,26,36,35,37,28,14,4,0,0,0,3,16,23,39,40,38,25,20,7,0,0,0,0,0,0,0,0,3,16,23,39,40,37,32,32,32,32,32,32,32,32,25,20,6,0,3,16,23,39,40,38,25,20,7,4,12,28,37,40,39,27,17,8,0,0,5,14,27,32,39,37,35,41,37,39,36,29,17,7,0,10,22,32,35,33,23,26,13,16,11,1,8,0,0,0,0,0,0,0,0,0,0,0,0,0,3,2,19,20,21,
                                        29,25,34,37,28,14,1,0,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,19,0,0,0,0,10,21,32,35,38,25,12,6,18,31,39,41,34,29,19,4,12,28,37,35,36,21,9,0,0,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,0,0,10,15,15,13,28,39,27,17,17,27,39,28,14,15,15,15,10,0,0,10,22,32,39,39,39,39,39,37,38,39,39,39,39,39,28,14,1,0,0,19,26,33,35,34,31,16,15,13,36,35,37,28,14,4,0,0,0,8,17,27,39,
                                        40,38,25,20,6,0,0,1,9,9,2,0,0,8,17,27,39,40,38,25,21,21,21,21,21,21,21,21,16,7,0,8,17,27,39,40,38,25,20,6,4,14,28,37,40,39,31,18,8,0,0,7,10,12,26,23,24,33,39,37,40,37,28,13,2,0,10,22,25,32,39,39,33,28,27,22,14,18,10,4,0,0,0,0,0,0,0,0,0,1,10,17,14,30,23,24,34,38,34,36,23,14,1,0,18,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,17,3,0,0,0,10,21,32,35,38,25,12,6,18,31,39,41,34,29,19,4,12,28,37,35,36,21,9,0,0,0,0,0,10,21,32,35,
                                        38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,8,10,15,15,15,21,32,33,26,15,12,25,39,23,20,15,15,5,0,0,0,10,22,27,27,27,27,27,27,34,32,27,27,27,27,27,27,14,1,0,8,17,27,39,41,33,30,11,2,13,36,35,37,28,14,4,0,0,0,0,18,29,34,41,37,25,14,4,0,3,17,21,21,14,9,0,0,18,29,34,41,38,25,12,9,9,9,11,14,13,13,15,0,0,0,18,29,34,41,38,25,12,4,1,14,28,37,41,34,26,19,0,0,0,19,13,13,17,20,13,30,27,32,35,35,32,22,
                                        10,0,1,18,14,22,27,24,34,38,39,32,25,29,21,20,15,2,3,0,0,0,6,9,18,12,21,31,28,33,39,39,33,28,31,21,12,19,3,0,18,27,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,23,16,3,0,0,0,10,21,32,35,38,25,12,6,18,31,39,41,34,29,19,4,12,28,37,35,36,21,9,0,0,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,2,13,30,30,30,26,34,36,30,30,30,24,34,29,30,30,30,18,0,0,0,1,18,17,17,17,17,17,26,34,32,
                                        21,17,17,17,17,17,19,3,0,3,17,23,39,41,33,30,11,9,21,36,35,37,28,14,4,0,0,0,0,10,22,36,35,37,24,13,2,0,4,12,25,24,21,9,0,0,11,22,32,35,37,28,14,5,0,8,18,31,24,26,19,0,0,0,10,21,36,37,37,28,14,5,2,13,24,37,37,24,21,10,0,0,0,18,31,28,14,1,9,15,12,23,38,35,32,22,10,0,0,8,5,15,16,13,29,25,32,39,38,34,24,23,30,14,18,6,0,5,16,13,26,25,36,34,38,34,36,23,26,13,17,10,4,0,0,0,18,27,39,35,35,35,35,35,35,35,35,35,35,35,35,35,35,38,23,16,3,0,0,0,
                                        10,21,32,35,38,25,12,6,18,31,39,41,34,29,19,4,12,28,37,35,36,21,9,0,0,0,0,0,10,21,32,35,38,25,12,6,11,30,33,41,39,23,17,3,0,0,0,0,10,21,32,35,38,25,12,6,0,0,0,0,0,0,0,0,0,2,13,24,33,33,33,38,34,33,33,33,39,38,33,33,33,31,18,0,0,0,0,0,8,8,8,8,19,26,34,32,21,10,8,8,8,8,0,0,0,8,18,31,34,41,34,27,12,12,31,33,41,37,28,14,18,9,0,0,0,1,12,23,39,35,33,26,18,9,15,21,24,23,20,4,0,0,5,14,25,39,35,32,30,19,9,11,14,25,24,13,9,0,0,0,
                                        1,12,23,34,35,36,21,11,15,22,32,35,34,27,20,4,0,0,0,18,31,36,22,18,9,9,16,23,39,37,24,13,9,0,0,0,0,0,6,9,18,14,22,27,24,34,38,39,33,28,14,1,0,10,22,24,33,38,39,33,28,31,21,12,19,2,3,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,0,11,18,21,32,35,38,25,12,18,18,31,39,41,34,29,18,18,12,28,37,35,36,21,18,11,0,0,0,11,18,21,32,35,38,25,12,18,18,30,33,41,39,23,17,18,5,0,0,11,18,21,32,35,38,25,12,18,15,8,0,0,0,0,0,0,
                                        0,2,13,24,33,33,34,38,33,33,33,33,37,34,33,33,33,31,18,0,0,0,0,0,0,0,0,0,19,26,34,32,21,10,0,0,0,0,0,0,0,0,11,22,36,37,37,32,25,25,25,36,35,37,28,31,31,18,0,0,0,0,15,21,28,34,38,28,30,13,22,25,24,22,15,0,0,0,0,19,22,28,39,39,28,30,13,22,23,32,29,17,6,0,0,0,0,11,13,25,34,34,23,21,21,25,39,34,25,13,10,0,0,0,0,18,31,39,28,26,21,21,26,24,37,33,27,20,4,0,0,0,0,0,0,0,8,5,15,16,21,29,25,32,39,28,14,1,0,10,22,32,34,36,23,26,13,17,10,4,
                                        0,0,0,0,0,0,0,0,18,27,39,40,40,40,40,40,40,40,40,40,40,40,40,40,40,38,23,16,3,0,4,12,31,31,32,35,38,25,31,21,31,31,39,35,34,31,31,22,31,28,37,35,36,31,31,20,6,0,4,12,31,31,32,35,38,25,31,22,31,31,33,35,39,23,31,30,11,0,4,12,31,31,32,35,38,25,31,31,14,1,0,0,0,0,0,0,0,2,13,26,26,26,24,34,31,26,26,23,39,25,26,26,26,26,18,0,0,0,0,0,0,0,0,0,19,26,34,32,21,10,0,0,0,0,0,0,0,0,1,20,31,24,33,33,32,23,21,24,24,24,24,24,31,18,0,0,0,0,7,19,
                                        21,27,24,33,32,36,32,28,30,17,4,0,0,0,0,6,18,21,23,36,33,32,36,32,28,26,12,2,0,0,0,0,0,3,19,13,27,24,33,36,36,33,24,27,13,15,8,0,0,0,0,18,31,28,36,33,36,36,33,32,28,31,14,10,0,0,0,0,0,0,0,0,0,0,0,6,10,18,14,22,27,28,14,1,0,10,22,28,31,21,12,19,2,3,0,0,0,0,0,0,0,0,0,0,18,27,39,35,35,35,35,35,35,35,35,35,35,35,35,35,35,38,23,16,3,0,4,12,25,24,24,24,24,24,24,21,28,24,24,24,24,24,28,22,24,24,24,24,24,24,25,20,6,0,4,12,25,24,24,24,
                                        24,24,24,22,23,24,24,24,24,24,24,30,11,0,4,12,25,24,24,24,24,24,24,28,14,1,0,0,0,0,0,0,0,3,10,15,15,22,32,32,22,15,14,28,39,27,17,15,15,15,5,0,0,0,0,0,0,0,0,0,19,26,34,32,21,10,0,0,0,0,0,0,0,0,0,9,16,21,26,26,22,12,12,13,13,13,13,13,13,19,0,0,0,0,0,7,11,20,21,30,26,26,22,14,18,1,0,0,0,0,0,0,6,15,12,21,30,26,26,22,14,17,2,0,0,0,0,0,0,0,3,10,16,13,22,26,26,22,13,16,10,8,0,0,0,0,0,10,16,14,21,22,26,26,26,22,13,17,10,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,8,5,15,16,21,14,1,0,10,13,13,17,10,1,0,0,0,0,0,0,0,0,0,0,0,0,0,18,27,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,23,16,3,0,6,16,13,13,13,13,13,13,13,14,13,13,13,13,13,13,13,14,13,13,13,13,13,13,13,17,7,0,6,16,13,13,13,13,13,13,13,14,13,13,13,13,13,13,13,13,10,0,6,16,13,13,13,13,13,13,13,13,20,4,0,0,0,0,0,0,0,0,0,0,18,29,34,24,13,10,21,36,33,26,19,0,0,0,0,0,0,0,0,0,0,0,0,0,19,26,24,24,
                                        21,10,0,0,0,0,0,0,0,0,0,0,4,10,15,15,10,1,5,5,5,5,5,5,5,7,0,0,0,0,0,0,0,4,9,11,15,15,10,5,8,0,0,0,0,0,0,0,0,0,1,10,15,15,15,10,5,3,0,0,0,0,0,0,0,0,0,0,6,9,11,15,15,11,9,6,0,0,0,0,0,0,0,0,7,1,9,11,15,15,15,10,2,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,10,9,0,0,3,10,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,18,8,0,0,4,
                                        5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4,0,0,0,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,8,0,0,4,5,5,5,5,5,5,5,5,1,0,0,0,0,0,0,0,0,0,0,6,20,23,38,25,12,19,26,33,36,21,10,0,0,0,0,0,0,0,0,0,0,0,0,0,11,13,13,13,14,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,5,14,28,24,27,17,17,27,24,28,14,5,0,0,0,0,0,0,0,0,0,0,0,0,0,8,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,12,13,13,13,15,18,13,13,13,17,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,3,6,5,5,5,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,5,5,5,7,1,5,5,5,5,5,5,8,0,0,0,0,1,5,5,5,5,5,5,5,4,8,
                                        5,5,5,5,5,5,0,0,0,4,5,5,5,5,5,5,5,1,0,4,5,5,5,6,0,5,5,5,5,5,5,8,0,3,5,5,5,5,5,5,5,5,1,5,5,5,5,5,7,0,0,0,3,5,5,5,5,5,5,5,5,5,5,5,5,3,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,
                                        0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,4,1,1,1,1,0,0,0,5,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,0,0,0,0,0,0,0,0,0,0,0,5,12,13,13,13,13,13,13,19,20,13,13,13,13,13,13,11,0,0,0,1,20,13,13,13,13,13,13,13,17,10,14,13,13,13,13,14,2,0,3,17,13,13,13,13,13,13,13,20,4,17,13,13,13,18,5,14,13,13,13,13,14,10,0,15,13,13,13,13,13,13,13,13,12,13,13,13,13,13,19,0,0,0,11,13,13,13,13,13,13,13,13,13,13,13,
                                        13,15,0,0,0,0,0,0,0,11,17,17,17,2,0,0,0,0,0,0,0,0,0,0,0,0,4,2,3,0,7,9,4,0,0,0,0,0,0,0,0,0,0,6,10,19,17,17,18,10,4,0,0,0,0,0,0,0,0,0,0,4,2,3,0,0,4,2,3,0,7,9,4,0,0,7,9,4,0,0,0,0,0,0,0,0,0,7,10,17,17,17,19,7,0,0,0,0,6,10,19,18,17,17,19,10,6,0,0,0,0,7,9,15,18,17,17,18,11,1,0,0,0,0,0,5,20,14,14,14,12,5,0,9,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,20,1,0,4,10,10,10,10,7,0,0,
                                        0,2,13,24,24,24,24,24,27,18,14,28,24,24,24,24,26,19,0,0,0,5,13,24,24,24,24,24,24,25,12,11,30,24,24,24,24,21,9,0,6,20,25,24,24,24,24,24,28,14,5,14,28,24,25,12,9,21,24,24,24,24,30,11,0,18,29,24,24,24,24,24,24,23,13,24,24,24,24,27,18,0,0,0,19,29,24,24,24,24,24,24,24,24,24,24,31,18,0,0,0,0,3,7,3,16,27,27,26,15,0,7,0,0,0,0,0,0,0,0,0,5,17,13,11,0,15,21,17,1,0,0,0,0,0,0,0,0,5,17,21,29,27,27,29,21,16,9,0,0,0,0,0,0,0,0,5,17,13,11,
                                        0,1,17,13,11,0,15,21,17,1,0,15,13,17,1,0,0,0,0,0,0,0,5,18,21,27,27,27,13,5,0,0,0,0,19,21,26,31,27,27,29,21,17,1,0,0,0,11,21,30,29,27,27,29,22,20,9,0,0,0,0,2,13,28,28,28,21,9,0,10,22,24,24,24,24,24,24,24,24,24,24,24,24,24,28,14,1,0,19,22,22,22,21,10,0,0,0,2,13,31,31,34,37,39,27,18,14,31,27,39,37,34,26,19,0,0,0,5,13,31,23,39,37,34,31,31,12,11,30,31,25,25,31,21,9,0,6,20,31,31,34,37,39,31,31,14,11,22,32,37,24,21,10,21,31,27,28,31,30,11,
                                        0,18,29,31,24,37,37,33,31,31,13,31,27,28,31,31,18,0,0,0,19,29,28,29,29,29,29,29,36,37,37,33,31,18,0,0,0,4,17,17,5,17,27,33,30,11,10,20,15,0,0,0,0,0,0,0,2,16,26,26,15,0,19,29,30,17,5,0,0,0,0,0,0,1,16,26,24,33,27,27,32,36,31,12,5,0,0,0,0,0,0,2,16,26,26,15,5,16,30,26,19,0,19,29,30,17,5,19,26,26,16,5,0,0,0,0,0,1,12,26,28,33,39,24,13,5,0,0,0,0,18,27,33,23,31,28,34,24,26,18,3,0,0,15,26,33,23,31,25,34,36,31,16,6,0,0,0,9,13,36,
                                        37,24,21,9,0,10,22,32,38,38,38,38,38,38,38,38,38,38,38,38,28,14,1,0,19,29,33,32,22,10,0,0,0,6,19,18,26,34,41,39,27,18,15,18,27,39,41,34,26,19,0,0,0,3,15,18,26,33,35,38,23,12,11,5,18,21,36,29,18,19,6,0,0,11,18,21,32,35,38,25,14,15,17,31,34,41,33,26,19,19,14,28,27,16,18,5,0,2,18,16,31,33,35,38,28,13,18,30,36,26,18,18,9,0,0,0,19,29,28,14,18,18,14,25,39,41,39,25,14,9,0,0,0,11,21,26,20,17,27,33,22,15,13,27,14,5,0,0,0,0,0,9,20,29,36,26,15,0,19,29,
                                        24,26,16,2,0,0,0,0,0,15,22,36,38,25,14,12,23,39,32,26,18,8,0,0,0,0,9,20,29,36,26,15,20,26,36,26,19,0,19,29,24,26,16,19,26,36,26,20,2,0,0,0,0,5,13,28,26,24,37,24,13,5,0,0,0,0,18,27,25,14,18,30,32,38,28,14,5,0,0,15,26,28,14,18,21,24,37,24,13,5,0,0,0,10,21,32,37,28,14,5,0,10,22,25,25,25,25,25,25,25,25,25,25,25,25,25,25,14,1,0,19,29,34,32,22,10,0,0,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,0,2,13,28,37,37,36,21,10,8,
                                        17,31,24,21,9,0,0,0,0,0,5,14,28,38,35,36,21,10,14,25,38,41,39,23,20,11,22,32,30,15,0,0,0,0,0,9,13,25,39,35,34,27,13,25,23,12,5,0,0,0,0,0,19,29,23,14,1,17,29,33,35,37,24,22,19,8,0,0,1,12,23,32,31,14,31,32,21,21,25,33,26,18,8,0,0,0,10,12,31,32,25,13,10,0,10,13,28,36,29,20,9,0,0,0,7,16,23,39,39,27,17,18,29,34,38,25,12,1,0,0,0,10,12,31,32,25,13,12,29,36,28,13,10,0,10,13,28,36,29,20,13,25,36,29,12,9,0,0,0,5,14,13,13,24,37,24,13,5,0,0,
                                        0,0,18,26,26,18,9,21,36,37,24,13,5,0,0,11,21,21,16,15,13,24,38,28,13,5,0,0,0,19,26,33,39,23,16,6,0,5,20,12,12,12,12,12,12,12,12,12,12,12,12,12,12,17,6,0,19,29,34,32,22,10,0,0,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,0,7,16,27,39,41,34,29,18,5,14,25,23,20,4,0,0,0,0,0,3,17,27,39,41,34,29,18,21,36,24,37,37,24,13,18,31,24,13,9,0,0,0,0,0,8,19,22,36,37,37,36,31,24,21,11,0,0,0,0,0,0,10,16,16,18,15,21,24,37,35,33,31,16,
                                        4,0,0,0,2,13,31,24,33,23,29,36,22,28,33,25,26,16,7,0,0,10,14,27,32,36,22,19,8,0,3,18,30,32,32,31,12,10,0,0,1,14,28,37,39,27,18,19,26,34,37,24,13,2,0,0,10,14,27,32,36,22,14,31,32,36,30,18,3,0,3,18,30,32,32,31,12,30,36,32,31,14,10,0,0,0,10,10,13,24,37,24,13,5,0,0,0,0,5,15,15,5,10,21,36,39,25,14,1,0,0,6,10,10,21,26,23,33,28,26,17,7,0,0,2,14,25,38,32,30,19,0,0,4,19,18,18,18,18,18,18,18,18,18,18,18,18,18,18,15,8,0,19,29,34,32,22,10,0,0,
                                        0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,0,0,15,22,32,35,38,25,14,11,22,32,26,19,0,0,0,0,0,0,0,11,22,32,35,38,25,12,29,24,27,39,35,33,30,12,25,23,20,6,0,0,0,0,0,0,1,20,27,34,35,38,33,29,17,6,0,0,0,0,0,0,0,3,3,9,14,23,39,41,39,25,14,9,0,0,0,0,8,10,17,21,31,24,25,36,24,28,26,14,18,5,0,0,4,12,27,33,33,31,16,4,0,0,0,1,20,27,34,33,27,20,6,0,1,14,28,37,39,31,18,15,26,34,35,24,13,2,0,4,12,27,33,33,31,12,27,33,
                                        34,27,20,1,0,0,0,1,20,27,34,33,27,20,31,34,33,27,12,6,0,0,0,5,13,24,37,24,13,5,0,0,0,0,0,0,0,5,20,31,34,28,22,19,8,0,0,0,0,10,21,25,24,28,23,22,17,4,0,5,16,29,33,34,23,12,5,0,0,10,22,31,31,31,31,31,31,31,31,31,31,31,31,31,31,14,1,0,19,29,34,32,22,10,0,0,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,0,0,5,14,28,38,35,32,22,17,27,28,13,9,0,0,0,0,0,0,0,2,13,28,37,37,24,21,23,25,30,33,35,39,27,21,36,26,19,0,0,0,
                                        0,0,0,0,0,10,13,28,38,35,32,30,18,3,0,0,0,0,0,0,0,0,6,17,29,33,35,37,24,22,19,8,0,0,0,0,0,7,10,17,21,27,32,34,28,26,14,18,5,8,0,0,4,12,28,38,25,13,9,0,0,0,0,0,10,13,28,38,25,20,6,0,4,12,25,38,39,27,17,19,29,34,37,28,14,5,0,4,12,28,38,25,13,12,25,38,25,13,10,0,0,0,0,0,10,13,28,38,25,20,13,25,38,25,12,6,0,0,0,5,13,24,37,24,13,5,0,0,0,0,0,0,10,20,29,36,25,21,18,15,7,0,0,1,10,10,16,20,21,28,39,24,21,10,0,11,22,36,39,25,
                                        21,11,0,0,0,10,22,32,34,34,34,34,34,34,34,34,34,34,34,34,28,14,1,0,19,29,34,32,22,10,0,0,0,0,0,19,26,34,41,39,27,18,0,18,27,39,41,34,26,19,0,0,0,0,0,0,3,17,31,34,41,34,31,13,28,27,16,7,0,0,0,0,0,0,0,7,16,27,39,41,33,26,24,29,13,24,37,37,28,29,36,21,10,0,0,0,0,0,0,0,7,18,26,32,34,35,38,28,13,10,0,0,0,0,0,0,0,0,15,21,24,37,35,33,31,16,4,4,4,0,0,0,1,17,13,31,24,36,29,36,23,33,25,26,14,19,8,0,4,12,28,39,32,30,18,3,0,0,0,6,
                                        17,26,33,39,25,20,6,0,8,18,29,33,38,23,20,17,27,39,34,27,16,7,0,4,12,28,39,32,30,12,25,39,32,26,18,7,0,0,0,6,17,26,33,39,25,20,26,32,39,25,12,6,0,0,0,5,13,24,37,24,13,5,0,0,0,0,6,15,14,31,24,29,14,21,29,13,2,0,8,18,21,21,19,6,16,23,38,34,26,19,0,1,16,31,23,21,19,3,0,0,0,10,22,32,33,33,33,33,33,33,33,33,33,33,33,33,28,14,1,0,19,29,27,27,22,10,0,0,0,0,0,19,26,34,41,39,27,17,8,17,27,39,41,34,26,19,0,0,0,0,0,0,0,10,21,36,35,38,28,30,
                                        32,22,15,0,0,0,0,0,0,0,0,0,19,26,33,35,39,23,36,21,20,23,39,35,32,23,25,12,1,0,0,0,0,0,0,0,10,21,28,27,25,38,35,34,27,20,1,0,0,0,0,0,0,9,14,23,39,41,39,25,14,9,17,12,12,11,0,0,9,13,28,34,28,30,29,32,21,31,32,33,27,20,7,0,3,18,21,25,34,28,13,10,0,0,0,11,21,28,34,25,21,19,8,0,0,9,13,23,34,24,22,21,28,34,25,21,10,0,0,3,18,21,25,34,28,13,21,25,34,28,21,11,0,0,0,11,21,28,34,25,21,21,28,34,25,21,19,8,0,0,11,18,13,24,37,24,13,18,11,0,
                                        0,8,18,21,25,23,22,22,22,22,36,13,2,0,8,17,27,27,16,10,12,23,38,33,26,19,0,0,9,14,14,15,7,0,0,0,0,10,21,26,26,26,26,26,26,26,26,26,26,26,26,26,26,14,1,0,2,17,17,17,18,1,0,0,0,0,0,15,30,33,41,39,23,16,4,12,25,38,41,34,26,19,0,0,0,0,0,0,0,1,14,25,38,35,32,27,28,14,5,0,0,0,0,0,0,0,0,0,9,13,24,37,35,39,25,14,19,26,33,41,38,34,31,18,8,0,0,0,0,0,0,1,20,27,28,21,30,36,37,37,36,22,19,8,0,0,0,0,6,17,29,33,35,37,24,22,19,4,14,25,
                                        31,18,0,0,7,17,29,25,21,18,31,32,22,20,26,24,21,11,0,0,0,6,19,21,25,34,27,12,5,0,2,14,23,34,23,13,15,3,0,0,0,8,15,13,31,25,24,24,28,27,13,19,3,0,0,0,6,19,21,25,34,27,12,13,23,34,23,12,5,0,2,14,23,34,23,13,12,23,34,23,13,19,7,0,0,6,12,31,31,24,38,24,31,31,20,6,0,3,17,23,39,33,33,33,33,33,36,13,2,0,8,17,27,24,26,22,31,32,34,25,13,9,0,0,0,9,2,0,0,0,0,0,0,6,11,15,15,15,15,15,15,15,15,15,15,15,15,15,15,10,0,0,0,8,8,8,0,0,0,0,
                                        0,0,0,10,21,36,35,38,25,14,20,22,36,38,41,34,26,18,19,6,0,0,0,0,0,8,18,29,34,41,38,33,31,17,3,0,0,0,0,0,0,0,0,0,4,20,23,38,41,34,31,17,9,21,24,37,35,32,22,10,0,0,0,0,0,9,18,18,30,32,26,17,20,27,34,35,39,25,13,18,19,7,0,0,15,21,24,37,35,33,31,16,18,18,14,28,31,18,0,0,0,9,14,13,19,17,27,33,22,11,17,22,17,6,0,0,0,0,3,15,13,23,32,26,15,0,19,29,33,27,13,11,8,0,0,0,0,9,12,12,12,14,13,13,14,12,12,12,11,0,0,0,0,3,15,13,23,32,26,15,
                                        13,23,32,26,19,0,19,29,33,27,13,19,26,32,23,13,15,3,0,0,0,6,12,23,23,23,23,23,23,23,20,6,0,3,17,23,23,23,23,23,23,23,23,13,2,0,8,17,26,27,25,28,28,25,31,21,19,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,12,23,39,35,33,25,23,24,27,39,35,34,31,31,13,2,0,0,0,0,0,0,10,21,24,37,35,36,22,11,0,0,0,0,0,0,0,0,0,0,0,18,29,34,35,32,22,11,4,12,25,38,38,28,14,5,0,0,0,0,
                                        8,17,31,31,25,25,31,30,29,31,28,37,35,33,31,31,13,2,0,6,12,23,39,35,38,25,31,31,31,31,31,28,31,18,0,0,0,0,9,10,3,16,23,36,30,15,4,10,5,0,0,0,0,0,0,8,11,14,27,26,15,0,19,29,27,14,10,8,0,0,0,0,0,15,30,25,25,25,25,25,25,25,25,31,18,0,0,0,0,0,8,11,14,27,26,15,10,14,27,26,19,0,19,29,27,14,10,19,26,27,14,11,8,0,0,0,0,8,19,16,16,16,16,16,16,16,19,8,0,0,11,16,16,16,16,16,16,16,16,17,4,0,0,2,19,17,12,12,14,20,17,10,3,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,21,25,32,34,33,24,29,27,24,24,24,24,24,13,2,0,0,0,0,0,0,4,12,23,24,24,25,14,1,0,0,0,0,0,0,0,0,0,0,0,10,21,24,24,28,14,5,8,18,29,24,24,27,17,3,0,0,0,0,8,17,27,24,24,24,24,30,29,24,24,24,24,24,24,24,13,2,0,6,12,25,24,24,24,24,24,24,24,24,24,24,31,18,0,0,0,0,0,0,3,17,13,13,13,11,0,0,0,0,0,0,0,0,0,0,0,10,14,30,15,0,19,26,
                                        12,10,0,0,0,0,0,0,0,15,30,28,28,28,28,28,28,28,28,31,18,0,0,0,0,0,0,0,10,14,30,15,0,10,14,26,19,0,19,26,12,10,0,19,26,14,10,0,0,0,0,0,0,0,0,3,3,3,3,3,3,3,0,0,0,0,0,3,3,3,3,3,3,3,3,8,0,0,0,0,0,3,6,4,4,6,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,19,14,22,26,30,21,16,13,13,13,13,13,13,12,5,0,0,0,0,0,0,0,19,13,13,13,13,
                                        18,8,0,0,0,0,0,0,0,0,0,0,0,1,20,13,13,13,17,7,0,10,13,13,13,13,11,0,0,0,0,0,0,19,13,13,13,13,13,14,13,13,13,13,13,13,13,13,12,5,0,7,17,13,13,13,13,13,13,13,13,13,13,13,13,15,0,0,0,0,0,0,0,4,2,2,2,3,0,0,0,0,0,0,0,0,0,0,0,0,10,19,1,0,5,19,9,0,0,0,0,0,0,0,0,10,12,14,14,14,14,14,14,14,14,14,11,0,0,0,0,0,0,0,0,10,19,1,0,0,9,19,5,0,5,19,9,0,0,5,19,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,5,11,15,15,9,6,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,7,5,5,5,5,6,0,0,0,0,0,0,0,0,0,0,0,0,0,1,5,5,5,4,0,0,8,5,5,5,5,3,0,0,0,0,0,0,6,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,4,5,5,5,5,5,5,5,5,5,5,5,
                                        5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,4,4,4,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,1,1,1,1,6,1,1,1,1,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,2,2,2,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,18,20,20,18,9,0,0,0,0,4,10,10,10,10,5,10,10,10,10,8,0,0,0,0,0,0,0,6,10,10,10,10,3,0,0,0,0,0,0,0,0,9,10,10,10,10,10,10,10,10,10,10,10,8,6,10,10,10,10,6,0,0,0,0,0,2,18,15,8,0,5,18,19,7,0,0,0,0,1,10,10,10,10,1,10,10,10,10,5,0,0,0,0,6,16,14,14,14,12,17,14,14,14,12,9,0,0,0,0,0,2,18,15,8,0,0,0,0,1,10,10,10,10,
                                        1,0,0,0,0,6,16,14,14,14,12,5,0,0,0,4,10,15,9,3,0,0,0,0,0,3,5,9,9,1,3,0,0,0,0,0,8,9,2,0,0,0,0,4,10,15,9,3,0,0,3,9,15,10,4,0,0,0,5,11,11,5,0,0,0,5,12,13,13,13,14,2,0,0,0,0,1,15,15,15,15,15,15,15,15,2,0,0,9,19,19,19,19,19,19,19,19,19,19,19,19,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,14,26,23,23,29,14,11,0,0,0,19,22,22,22,21,17,22,22,22,13,2,0,0,0,0,0,0,7,
                                        18,21,22,22,13,15,8,0,0,0,0,0,0,5,14,22,22,22,22,22,22,21,22,22,22,13,9,15,22,22,22,22,15,0,0,0,0,10,12,26,13,10,9,20,26,21,11,0,0,0,0,19,22,22,22,22,18,22,22,22,22,17,8,0,0,0,1,14,28,28,28,21,12,25,28,28,22,10,0,0,0,0,10,12,26,13,10,0,0,0,0,19,22,22,22,22,18,0,0,0,0,1,14,28,28,28,21,9,0,0,1,17,21,30,13,19,3,0,0,0,2,17,14,21,21,14,17,10,4,0,0,4,15,13,12,1,0,0,1,17,21,30,13,19,3,3,19,13,30,21,16,5,0,10,12,22,30,14,10,0,
                                        0,5,12,27,24,24,29,17,7,0,0,0,15,30,26,26,26,26,26,26,26,17,8,8,17,29,29,29,29,29,29,29,29,29,29,29,29,20,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,12,27,33,33,33,33,23,14,2,0,0,19,29,33,32,22,17,27,33,24,13,2,0,0,0,0,0,8,15,21,28,33,33,23,13,10,0,0,0,0,0,0,5,13,24,25,28,36,25,25,30,25,36,33,29,18,12,23,33,25,26,15,0,0,0,9,14,27,32,27,20,12,31,32,23,12,5,0,0,0,18,31,33,33,31,18,26,33,33,27,17,8,0,
                                        0,0,1,14,28,37,24,13,12,25,38,36,21,9,0,0,0,9,14,27,32,27,20,1,0,0,0,18,31,33,33,31,18,0,0,0,0,1,14,28,37,24,13,2,0,0,19,30,24,33,28,21,10,0,8,10,14,29,28,36,36,28,31,21,20,18,19,20,22,25,14,1,0,0,19,30,24,33,28,21,10,9,13,25,33,36,26,18,4,20,27,32,33,27,12,1,0,0,10,14,23,34,28,21,10,0,0,0,15,26,34,34,34,34,34,34,27,17,8,8,17,27,34,34,34,34,34,34,34,34,34,34,23,20,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,15,30,32,28,26,30,25,33,26,18,0,0,19,29,34,32,22,17,27,39,24,13,2,0,0,0,0,0,11,13,25,39,38,37,34,27,14,9,0,0,0,0,0,5,13,28,14,23,24,13,27,30,13,24,24,28,13,30,23,33,29,12,10,0,0,4,16,31,33,33,31,14,26,32,34,27,14,2,0,0,0,18,31,39,33,26,18,29,34,34,29,18,0,0,0,0,9,13,24,38,25,12,13,28,37,28,14,5,0,0,4,16,31,33,33,31,14,5,0,0,0,18,31,39,33,26,19,0,0,0,0,9,13,24,38,25,12,4,0,6,20,23,38,41,34,29,19,0,9,13,23,33,38,38,38,38,34,24,
                                        23,29,29,23,24,28,14,1,0,6,20,23,38,41,34,29,19,15,26,33,35,38,25,12,2,13,24,37,37,36,21,9,0,0,0,10,13,23,34,27,12,1,0,0,15,26,34,37,37,37,37,39,27,17,8,8,17,27,28,28,28,28,28,28,28,28,28,28,23,20,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,17,27,33,26,18,19,22,32,23,16,7,0,19,29,34,32,22,17,27,39,24,13,2,0,0,0,0,10,14,23,34,39,28,36,38,33,31,20,5,0,0,0,0,1,20,13,16,23,24,13,13,14,13,24,27,33,29,25,30,33,29,19,
                                        0,0,0,10,21,24,39,23,14,13,28,38,25,13,10,0,0,0,6,20,23,38,36,21,16,27,39,32,22,11,0,0,0,8,19,26,33,34,31,17,22,32,39,27,16,7,0,0,10,21,24,39,23,14,9,0,0,0,6,20,23,38,36,21,10,0,0,0,8,19,26,33,34,31,17,8,0,6,20,23,38,41,34,29,19,0,10,22,32,32,28,23,25,36,34,38,38,34,34,39,34,23,14,1,0,6,20,23,38,41,34,29,19,15,26,33,35,38,25,12,2,13,24,37,35,36,21,9,0,0,0,8,11,13,25,32,30,19,8,0,15,26,24,24,24,24,24,24,27,17,8,0,19,14,14,14,14,14,14,14,
                                        14,14,14,14,14,18,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,17,27,32,22,10,9,21,36,25,20,6,0,19,29,34,32,22,17,27,39,24,13,2,0,0,0,9,12,27,33,34,25,21,30,28,39,32,29,16,1,0,0,0,0,1,5,16,23,24,13,5,5,13,24,21,24,28,23,30,33,29,19,0,0,8,17,31,34,33,30,18,29,34,34,29,18,8,0,0,0,10,21,24,39,25,12,13,28,38,28,14,5,0,0,8,11,13,25,38,28,21,14,23,39,24,21,11,0,0,8,17,31,34,33,30,19,0,0,0,0,10,21,24,39,25,12,
                                        1,0,0,8,11,13,25,38,28,21,10,0,0,0,19,30,36,34,28,21,10,0,10,22,28,26,14,20,14,21,31,28,32,34,34,24,27,13,11,8,0,0,19,30,36,34,28,21,10,9,13,25,33,36,26,18,4,20,27,33,33,23,12,1,0,0,0,0,3,19,21,27,27,14,1,0,10,14,13,13,13,13,13,13,13,19,0,0,3,1,1,1,1,1,1,1,1,1,1,1,1,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,31,34,31,12,16,26,33,27,17,3,0,19,29,34,32,22,17,27,39,24,13,2,0,0,5,20,31,33,33,
                                        27,13,19,18,21,25,34,36,30,17,4,0,0,0,0,1,16,23,24,13,2,2,13,24,13,31,24,22,30,33,29,19,6,0,4,12,25,38,36,21,20,23,38,32,21,10,0,0,0,10,14,23,34,36,30,12,27,34,32,26,18,8,0,0,1,14,23,34,36,26,12,27,33,32,29,16,4,0,0,4,12,25,38,36,21,9,0,0,0,10,14,23,34,36,30,19,0,0,0,1,14,23,34,36,26,17,7,0,0,0,1,16,22,26,21,18,7,0,10,22,21,18,5,7,1,10,17,13,22,29,26,21,20,10,8,0,0,0,1,16,22,26,21,18,7,3,19,13,26,22,20,5,0,10,14,30,26,14,10,0,
                                        0,0,0,0,0,7,19,17,17,19,3,0,8,5,5,5,5,5,5,5,5,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,21,24,32,23,23,36,36,22,15,0,0,19,29,34,32,22,17,27,39,24,13,2,0,1,16,26,32,36,29,12,10,8,6,19,13,27,33,24,22,18,7,0,0,8,17,21,23,24,21,14,14,21,24,21,14,13,13,30,33,29,21,11,0,5,14,28,39,24,13,14,28,39,36,21,9,0,0,0,18,26,32,32,29,20,30,36,33,27,12,5,0,0,0,
                                        3,18,30,24,26,20,19,22,24,29,12,2,0,0,0,5,14,28,39,24,13,5,0,0,0,18,26,32,32,29,20,1,0,0,0,3,18,30,24,26,20,5,0,0,0,0,0,1,11,15,10,7,0,0,4,19,11,3,0,0,0,0,3,2,11,19,19,10,4,0,0,0,0,0,0,1,11,15,10,7,0,0,3,10,15,11,1,0,0,0,2,15,15,2,0,0,0,0,0,0,0,0,0,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,6,17,26,24,34,34,24,29,20,1,0,0,19,29,27,27,22,17,27,27,27,13,2,0,11,30,27,27,30,16,9,0,0,0,8,10,14,29,27,27,13,2,0,0,3,17,23,28,28,28,13,14,28,28,28,14,10,22,28,28,28,26,15,0,2,13,27,27,27,13,14,27,27,27,21,9,0,0,0,5,12,27,26,20,9,20,31,29,12,10,0,0,0,0,0,1,20,21,17,5,4,16,22,16,9,0,0,0,0,2,13,27,27,27,13,5,0,0,0,5,12,27,26,20,9,0,0,0,0,0,1,20,21,17,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,17,21,26,26,21,16,2,0,0,0,2,17,17,17,18,10,17,17,17,19,6,0,5,17,17,17,18,1,0,0,0,0,0,0,9,17,17,17,18,6,0,0,0,19,12,12,12,12,17,17,12,12,12,17,2,12,12,12,12,12,
                                        10,0,6,18,17,17,17,19,19,17,17,17,18,4,0,0,0,0,10,14,17,5,0,9,14,16,9,0,0,0,0,0,0,0,2,10,4,0,0,5,11,1,0,0,0,0,0,6,18,17,17,17,19,7,0,0,0,0,10,14,17,5,0,0,0,0,0,0,0,2,10,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,10,19,19,10,4,0,0,0,0,0,8,8,8,0,0,8,8,8,0,0,0,0,8,8,8,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,8,4,4,4,4,6,6,4,4,4,6,0,4,4,4,4,4,0,0,0,0,8,8,8,0,0,8,8,8,8,0,0,0,0,0,0,1,6,0,0,0,1,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,8,8,0,0,0,0,0,0,0,1,6,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        wOut=hOut=256;
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

