#include "text_buffer.h"

using namespace muon;

bool TextRectangle::includes(TextCharPos& pos) {
    return pos.c >= start.c && pos.c <= end.c && pos.l >= start.l && pos.l <= end.l;
}

bool TextRectangle::intersects(TextRectangle& rect) {
    TextCharPos thst = { end.c, start.l };
    TextCharPos thsb = { start.c, end.l };
    TextCharPos cmpt = { rect.end.c, rect.start.l };
    TextCharPos cmpb = { rect.start.c, rect.end.l };
    return includes(rect.start) || includes(rect.end) || includes(cmpt) || includes(cmpb) ||
           rect.includes(start) || rect.includes(end) || rect.includes(thst) || rect.includes(thsb);
}