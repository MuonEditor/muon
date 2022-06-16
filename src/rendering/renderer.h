#pragma once

#include "../gl.h"

#include <string>
#include <unordered_map>

namespace muon::rendering {

class Shader;

struct FontChar {
    GLuint glTex;
    float advance;
    int w,h, xoff, yoff;
    uint8_t* data;
};

struct RenderableChar {
    FontChar* character;
    glm::vec4 colour;
    uint8_t selected        : 1;
    uint8_t sec_selected    : 1; // lighter overlay for ctrl f highlights or whatever
    uint8_t reserved        : 6; // NU, future useful flags for the happy GPU :(
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    void writeString();
    void putStr(std::string string, int x, int y, const glm::vec4& col);

    std::string ttfFontLocation;

private:
    std::unordered_map<char, FontChar> mFdata;
    
    float mGlyphScale;
    const float mSDFRes = 128.0f;
    const int mPixelDist = 92.0f;

private:
    Shader* mFontShader;

    GLuint mVAO;
    GLuint mVBO;
    
};

class RenderCompositor {

};

}

