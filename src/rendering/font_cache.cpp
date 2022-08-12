#include "font_cache.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdio>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Modern C++ ?
void readBytes(uint8_t** buffer, std::string loc) {
    FILE* f = fopen(loc.c_str(), "rb");
    assert(f);

    size_t pos = ftell(f);
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, pos, SEEK_SET);

    *buffer = static_cast<uint8_t*>(malloc(len + 2)); // nul + extra

    fread(*buffer, 1, len, f);
    fclose(f);
}

using namespace muon::rendering;

FontCache::FontCache() {
    this->mInitFSCache();
    this->registerFont(defaultFont);
}

FontCache::FontCache(std::string cacheLocation) {
    mCacheLocation = std::filesystem::path(cacheLocation);
    this->mInitFSCache();
    this->registerFont(defaultFont);
}

FontCache::~FontCache() {
    // free FontChar imagedata
    // also stbtt_fontinfo has some data that needs to be cleared
}

FontEntry FontCache::registerFont(std::string ttfLocation) {
    std::filesystem::path location = mResourcesLocation;
    location += std::filesystem::path{ttfLocation};

    assert(std::filesystem::exists(location));

    uint8_t* fontBuffer;
    readBytes(&fontBuffer, location.string());

    FontEntry font = FontEntry(new _FontEntry);
    font->name = location.stem();
    font->location = location;
    font->info = (struct stbtt_fontinfo){};
    std::cout << "Loading font " << font->name << std::endl;

    stbtt_InitFont(&font->info, fontBuffer, 0);
    if (font->info.numGlyphs == 0) throw std::runtime_error("Font file error");

    mFonts.push_back(font);
    mGlyphScale = stbtt_ScaleForPixelHeight(&font->info, mSDFRes);
    std::cout << font->info.numGlyphs << " Glyphs Registered (Scale: " << mGlyphScale << ")" << std::endl;

    // Here we want to load the font's lineheight and other metrics useful for rendering

    // Check cache for existing font glyphs
    auto cacheHits = this->mCacheCheck(font->name);
    // font is not in cache, load range
    if (cacheHits.size() == 0) {
        this->mLoadBasicCharset(font, {});
        return font;
    }

    // build exclude list
    std::vector<mCacheHit> excludeList;
    for (const auto& hit : cacheHits) {
        excludeList.push_back(hit);
    }

    this->mLoadBasicCharset(font, excludeList);

    return font;
}

// TODO (Ben): This function is a little messy
void FontCache::mLoadBasicCharset(FontEntry font, std::vector<mCacheHit> exclude) {
    for (int i = 33; i < 127; i++) {
        // TODO (Ben): This doesn't do what i expect
        int glyph = stbtt_FindGlyphIndex(&font->info, i);

        int isGlyphEmpty = stbtt_IsGlyphEmpty(&font->info, glyph);
        if (isGlyphEmpty != 0) { 
            continue;
        }

        FontChar fc = FontChar(new _FontChar);

        // char is in the cache
        auto it = std::find_if(exclude.begin(), exclude.end(), [&i](const mCacheHit& obj) {return obj.glyph == i;});
        if (it != exclude.end()) {
            auto hitIndex = std::distance(exclude.begin(), it);

            // we are processing a cached SDF here
            std::string lookPath = exclude[hitIndex].location;
            // add to read queue
            // we want to load the cached font and use stbtt_GetCodepointBox to get glyph metadata
            int cacheX, cacheY;
            fc->imgData = static_cast<uint8_t*>(stbi_load(lookPath.c_str(), &cacheX, &cacheY, NULL, 1));

            // we want to load the metadata from the font, skipping the generation of the SDF
            int ix0, iy0, ix1, iy1;
            stbtt_GetGlyphBitmapBoxSubpixel(&font->info, glyph, mGlyphScale, mGlyphScale, 0.0f, 0.0f, &ix0, &iy0, &ix1, &iy1);
            ix0 -= mGlyphPadding;
            iy0 -= mGlyphPadding;
            ix1 += mGlyphPadding;
            iy1 += mGlyphPadding;

            fc->w = (ix1 - ix0);
            fc->h = (iy1 - iy0);
            fc->xoff = ix0;
            fc->yoff = iy0;

            assert((cacheX == fc->w) && (cacheY == fc->h));

            int advance;
            stbtt_GetCodepointHMetrics(&font->info, i, &advance, NULL);
            fc->advance = static_cast<float>(advance) * mGlyphScale;

            font->loadedCharset[static_cast<char>(i)] = fc;
            font->loadedChars++;
            continue;
        }

        // this char is not in the cache
        fc->imgData = stbtt_GetCodepointSDF(&font->info, mGlyphScale, i, 3, 128, mPixelDist, &fc->w, &fc->h, &fc->xoff, &fc->yoff);

        int advance;
        stbtt_GetCodepointHMetrics(&font->info, i, &advance, NULL);
        fc->advance = static_cast<float>(advance) * mGlyphScale;

        font->loadedCharset[static_cast<char>(i)] = fc;
        font->loadedChars++;

        // TOOD (Ben): add to write queue
        std::string s = font->name + "_"  + std::to_string(i) + ".png";
        std::filesystem::path cacheLocation = mCacheLocation;
        cacheLocation += s;
        stbi_write_png(cacheLocation.c_str(), fc->w, fc->h, 1, fc->imgData, fc->w * sizeof(uint8_t));
    }

    this->mCreateAtlasFromBasic(font);
}

// TODO (Ben): This should be called when loading basic
void FontCache::mLoadFurtherChar(FontEntry font, char ch) {

}

// Loop over all chars and get the largest glyph w,h (and add 10px to each end)
// we do this so if a further char is loaded and it is marginaly bigger, the whole
// texture unit doesn't have to be resized
void FontCache::mCreateAtlasFromBasic(FontEntry font) {
    int w = 0; int h = 0;
    for (const auto& [index, fc] : font->loadedCharset) {
        if (fc->w > w) w = fc->w;
        if (fc->h > h) h = fc->h;
    }

    w += 10; h += 10;

    const GLsizei glyphs = static_cast<GLsizei>(font->loadedChars);
    std::vector<u_int32_t> nullData(w * h, 0);

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &font->glTex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, font->glTex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, w, h, glyphs, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    int i = 0;
    for (const auto& [index, fc] : font->loadedCharset) {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_RED, GL_UNSIGNED_BYTE, &nullData[0]);
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, fc->w, fc->h, 1, GL_RED, GL_UNSIGNED_BYTE, fc->imgData);

        i++;
    }

	// glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void FontCache::mAddGlpyhtoAtlas(FontEntry font, char ch) {

}

void FontCache::mInitFSCache() {
    // check if folder exists
    if (!std::filesystem::exists(mCacheLocation)) {
        std::cout << "Creating font cache" << std::endl;
        std::filesystem::create_directories(mCacheLocation);
        return;
    }

    // iterate over files in the cache
    for (const auto& cacheEntry : std::filesystem::directory_iterator(mCacheLocation)) {
        std::string cacheEntryLocation = cacheEntry.path().stem().string();
        std::string font = cacheEntryLocation.substr(0, cacheEntryLocation.find('_'));

        if (std::find(mCachedFonts.begin(), mCachedFonts.end(), font) != mCachedFonts.end()) {
            continue;
        }

        std::cout << "New cached font found " << font << std::endl;
        mCachedFonts.push_back(font);
    }
}

// all cache operations assume mInitFSCache has been called
std::vector<FontCache::mCacheHit> FontCache::mCacheCheck(std::string name) {
    std::vector<mCacheHit> ret;
    for (const auto& cacheEntry : std::filesystem::directory_iterator(mCacheLocation)) {
        std::string cacheEntryLocation = cacheEntry.path().stem().string();
        std::string font = cacheEntryLocation.substr(0, cacheEntryLocation.find('_'));
        std::string glyph = cacheEntryLocation.substr(cacheEntryLocation.find('_')+1, cacheEntryLocation.length());
        if (font == name) {
            ret.push_back({
                cacheEntry,
                font,
                static_cast<char>(std::atoi(glyph.c_str()))
            });
        }
    }
    return ret;
}

// TODO
void FontCache::mCacheWriteWorker() {

}

void FontCache::mCacheReadWorker() {

}
