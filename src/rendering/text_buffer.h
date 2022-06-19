#pragma once

#include "../gl.h"

#include <string>
#include <vector>

namespace muon::rendering {

enum TextDecorator {
    DECORATION_UNDERLINE,
    DECORATION_OVERLINE,
    DECORATION_SQUIGGLE,
    DECORATION_COLOURMOD
};

struct TextDecoration {
    int startX, endX;
    int startY, endY;
    TextDecorator textDecorator;
    glm::vec4 colourMod;
};

struct TextLine {
    std::vector<char> text;
    std::vector<TextDecoration> textDecorations;
    // unloaded line numbers will count so whole
    // file does not need to be in memory
    bool loaded;
};

struct TextBuffer {
    std::vector<TextLine> data;
    std::vector<glm::ivec2> cursors;
};

}
