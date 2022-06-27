#pragma once

#include "gl.h"
#include "font_cache.h"
#include "text/text_buffer.h"

#include <string>
#include <unordered_map>

namespace muon::rendering {

class Shader;

class TextRenderer {
public:
    TextRenderer();
    TextRenderer(TextBuffer buffer);
    ~TextRenderer();

private:
    FontCache mCache;

private:
    // rendering information
    std::shared_ptr<Shader> mFontShader;
};

class RenderCompositor {

};

}

