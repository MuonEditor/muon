#pragma once
#include <vector>
#include <string>
#include <memory>

namespace muon {
    enum TextDecoration {
        NONE        = 0,
        UNDERLINE   = (1 << 0),
        OVERLINE    = (1 << 1),
        SQUIGGLE    = (1 << 2),
    };

    struct Color {
        float r, g, b, a;
    };

    struct TextChar {
        uint16_t code;
        TextDecoration deco;
        Color fore;
        Color back;
    };

    struct TextCharPos {
        size_t c;
        size_t l;
    };

    class TextRectangle {
    public:
        bool includes(TextCharPos& pos);
        bool intersects(TextRectangle& rect);
        TextCharPos start;
        TextCharPos end;
    };

    struct TextChunk {
        TextCharPos pos;
        std::basic_string<TextChar> data;
    };
    
    class _TextBuffer;
    typedef std::shared_ptr<_TextBuffer> TextBuffer;
    class _TextBuffer {
    public:
        virtual ~_TextBuffer() {}
        virtual std::vector<TextChunk> get(TextRectangle& rect) = 0;
        virtual size_t getWidth() = 0;
        virtual size_t getHeight() = 0;
    };
}
