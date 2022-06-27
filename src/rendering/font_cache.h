#pragma once

#include "../gl.h"
#include "imgui/imstb_truetype.h"

#include <thread>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include <unordered_map>

namespace muon::rendering {

class Shader;

struct _FontChar {
    GLuint glTex = 0;
    float advance;
    int w, h, xoff, yoff;
    uint8_t* imgData;
};
typedef std::shared_ptr<_FontChar> FontChar;

struct _FontEntry {
    std::filesystem::path location;
    std::string name;
    stbtt_fontinfo info;
    std::unordered_map<char, FontChar> loadedCharset;
};
typedef std::shared_ptr<_FontEntry> FontEntry;

// The font cache is structured as follows
// In ./cache/fonts (by default)
// font caches are fontname_glyphnumber.png
//
// the font cache only attempts to hit the fs cache when a
// font is called to be loaded, and a cross refference will
// take place to load the minimum visible charset
//
// visible char set is 32-255
class FontCache {
public:
    FontCache();
    FontCache(std::string cacheLocation);
    ~FontCache();

    // Why would you need more than 255 fonts?
    uint8_t registerFont(std::string ttfLocation);

    std::filesystem::path defaultFont{ "DejaVu Sans Mono.ttf" };
    
    uint8_t getFontByName(std::string name);
    std::vector<std::string> getFonts();

    FontChar getFontChar(uint8_t font, char character);

private:
    // exclude is if cache is incomplete, only load non excluded
    void mLoadBasicCharset(FontEntry font, std::vector<char> exclude);
    // for individual, say a user typed an emoji, it is loaded
    // then cached, then a texture is allocated for it
    // UTF-8 SOON
    void mLoadFurtherChar(uint8_t font, char ch);

private:
    std::unordered_map<uint8_t, FontEntry> mFonts;
    uint8_t mNextFontID = 0;

    struct mCacheHit {
        std::filesystem::path location;
        std::string font;
        char glyph;
    };
    // disk cache information
    void mInitFSCache();
    std::vector<mCacheHit> mCacheCheck(std::string name);
    void mCacheWriteWorker();
    void mCacheReadWorker();

    std::vector<std::string> mCachedFonts;
    std::filesystem::path mCacheLocation{ "./.cache/fonts/" };
    std::filesystem::path mResourcesLocation{ "./res/" };

private:
    // SDF information
    float mGlyphScale;
    const float mSDFRes = 128.0f;
    const int mPixelDist = 64.0f;
};

}
