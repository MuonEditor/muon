#include "font_cache.h"

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdio>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

uint8_t FontCache::registerFont(std::string ttfLocation) {
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

    mGlyphScale = stbtt_ScaleForPixelHeight(&font->info, mSDFRes);
    std::cout << font->info.numGlyphs << " Glyphs Registered (Scale: " << mGlyphScale << ")" << std::endl;

    // Here we want to load the font's lineheight and other metrics useful for rendering

    // Check cache for existing font glyphs
    auto cacheHits = this->mCacheCheck(font->name);
    // font is not in cache, load range
    if (cacheHits.size() == 0) {
        this->mLoadBasicCharset(font, {});
        return mNextFontID++;
    }

    // build exclude list
    std::vector<mCacheHit> excludeList;
    for (const auto& hit : cacheHits) {
        excludeList.push_back(hit);
    }

    this->mLoadBasicCharset(font, excludeList);
    return mNextFontID++;
}

void FontCache::mLoadBasicCharset(FontEntry font, std::vector<mCacheHit> exclude) {
    // WE ONLY WANT RENDERABLE CHARACTERS
    for (int i = 33; i < 128; i++) {
        // TODO: This doesn't do what i expect
        int isGlyphEmpty = stbtt_IsGlyphEmpty(&font->info, i);
        if (isGlyphEmpty != 0) { 
            continue;
        }

        FontChar fc = FontChar(new _FontChar);

        // modern C++ moment
        auto it = std::find_if(exclude.begin(), exclude.end(), [&i](const mCacheHit& obj) {return obj.glyph == i;});
        if (it != exclude.end()) {
            auto hitIndex = std::distance(exclude.begin(), it);

            // we are processing a cached SDF here
            std::string lookPath = exclude[hitIndex].location;
            // add to read queue
            fc->imgData = static_cast<uint8_t*>(stbi_load(lookPath.c_str(), &fc->w, &fc->h, nullptr, 1));

            // we want to load the cached font and use stbtt_GetFontBoundingBox to get glyph metadata
            stbtt_GetFontBoundingBox(&font->info, &fc->w, &fc->h, &fc->xoff, &fc->yoff);
            int advance;
            stbtt_GetCodepointHMetrics(&font->info, i, &advance, NULL);
            fc->advance = advance * mGlyphScale;

            font->loadedCharset[static_cast<char>(i)] = fc;
            continue;
        }

        // this char is not in the cache, load it and put it in the cache
        fc->imgData = stbtt_GetCodepointSDF(&font->info, mGlyphScale, i, 3, 128, mPixelDist, &fc->w, &fc->h, &fc->xoff, &fc->yoff);
        int advance;
        stbtt_GetCodepointHMetrics(&font->info, i, &advance, NULL);
        fc->advance = advance * mGlyphScale;

        font->loadedCharset[static_cast<char>(i)] = fc;

        // add to write queue
        std::string s = font->name + "_"  + std::to_string(i) + ".png";
        std::filesystem::path cacheLocation = mCacheLocation;
        cacheLocation += s;
        stbi_write_png(cacheLocation.c_str(), fc->w, fc->h, 1, fc->imgData, fc->w * sizeof(uint8_t));
    }
}

void FontCache::mLoadFurtherChar(uint8_t font, char ch) {

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
    for (const auto cacheEntry : std::filesystem::directory_iterator(mCacheLocation)) {
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
