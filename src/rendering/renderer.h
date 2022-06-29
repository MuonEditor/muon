#pragma once

#include "gl.h"
#include "shader.h"
#include "font_cache.h"
#include "text/text_buffer.h"

#include <string>
#include <unordered_map>

namespace muon::rendering {

class TextRenderer {
public:
    TextRenderer(FontCache cache);
    ~TextRenderer();

    void renderBuffer(TextBuffer buffer);


private:
    FontCache mCache;

private:
    // rendering information
    Shader mFontShader;
};

class RenderCompositor {

};

}

