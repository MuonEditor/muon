#include "renderer.h"

#include "shader.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "imgui/imstb_truetype.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <stdexcept>
#include <stdio.h>

using namespace muon::rendering;

TextRenderer::TextRenderer()
    : ttfFontLocation("../res/DejaVu Sans Mono.ttf") {

    FILE* f = fopen(ttfFontLocation.c_str(), "rb");
    if (!f) throw std::runtime_error("Cannot Open Font File");

    unsigned char* buffer;

    // bad way to test size lol
    size_t pos = ftell(f);
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, pos, SEEK_SET);

    buffer = static_cast<unsigned char*>(malloc(len+2)); // nul + extra

    fread(buffer, 1, len, f);
    fclose(f);

    stbtt_fontinfo font;
    stbtt_InitFont(&font, buffer, 0);

    mGlyphScale = stbtt_ScaleForPixelHeight(&font, mSDFRes);

    if (font.numGlyphs == 0) throw std::runtime_error("Font file error");
    std::cout << font.numGlyphs << " Glyphs Loaded (Scale: " << mGlyphScale << ")" << std::endl;

    // ok poggers that's the font loaded
    // and now we extract SDF glyphs
    // WE ONLY WANT RENDERABLE CHARACTERS!
    for (int i = 33; i < 127; i++) {
        FontChar fc;
        
        int xoffset, yoffset;
        fc.data = stbtt_GetCodepointSDF(
            &font,
            mGlyphScale,
            i, 3, 128, 
            mPixelDist, 
            &fc.w, &fc.h,
            &xoffset,
            &yoffset);

        fc.xoff = xoffset;
        fc.yoff = yoffset;

        int advance;
        stbtt_GetCodepointHMetrics(&font, i, &advance, NULL);

        fc.advance = advance * mGlyphScale;
        mFdata[i] = fc;

        // std::string s;
        // s += (char)i;
        // s += ".png";
        // stbi_write_png(s.c_str(), fc.w, fc.h, 1, fc.data, fc.w * sizeof(uint8_t));
    }


    // OK that's pog
    // now we can make opengl textures
    // from the SDF glyphs

    // so we don't have to align by the 4th byte
    // far more memory efficient for grayscale
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (auto& ch : mFdata) {
        glGenTextures(1, &ch.second.glTex);
        glBindTexture(GL_TEXTURE_2D, ch.second.glTex);
        // we use only the red channel because we are storing
        // grayscale so we can pack into less bytes
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ch.second.w, ch.second.h, 0, GL_RED, GL_UNSIGNED_BYTE, ch.second.data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        std::cout << ch.second.glTex << std::endl;
    }

    // Ok EPIC, now shader
    mFontShader = new Shader();
    mFontShader->load("../res/font");
    mFontShader->link();

    // OK SO EPIC, now buffers for this
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // Reserve a buffer but don't send data
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    // Buffer is interleaved xy = pos, zw = texCoord
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// we do NOT want to compute this every frame
void TextRenderer::putStr(std::string string, int x, int y, const glm::vec4& col) {
    mFontShader->use();

    glUniform4f(glGetUniformLocation(mFontShader->program, "glyphCol"), col.r, col.g, col.b, col.a);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(mVAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float xAdvance = 0;

    for (const char& character : string) {
        FontChar* ch = &mFdata[character];

        xAdvance += (static_cast<float>(ch->advance) * 0.002);
        if (character == ' ') continue;

        float xpos = static_cast<float>(x) * mGlyphScale + xAdvance;
        float ypos = static_cast<float>(y) * mGlyphScale;

        float w = static_cast<float>(ch->w) * mGlyphScale * 0.05;
        float h = static_cast<float>(ch->h) * mGlyphScale * 0.05;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch->glTex);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

TextRenderer::~TextRenderer() {
    delete mFontShader;
}
