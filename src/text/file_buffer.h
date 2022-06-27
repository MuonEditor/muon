#pragma once

#include "text_buffer.h"

#include <fstream>

namespace muon {
    class _FileBufferTreeNode;
    typedef std::shared_ptr<_FileBufferTreeNode> FileBufferTreeNode;

    class _FileBufferTreeNode {
    public:
        //size_t offset;
        //size_t length;
        TextRectangle rect;
        std::basic_string<TextChar> data;
        FileBufferTreeNode sub0;
        FileBufferTreeNode sub1;
    };

    struct _FileBuffer;
    typedef std::shared_ptr<_FileBuffer> FileBuffer;
    class _FileBuffer : public _TextBuffer {
    public:
        _FileBuffer(std::string path);
        ~_FileBuffer();

        std::vector<TextChunk> get(TextRectangle& rect);
        size_t getWidth();
        size_t getHeight();

    //private:
        void recursiveGet(std::vector<TextChunk>& chunks, FileBufferTreeNode node, TextRectangle& rect);
        std::vector<FileBufferTreeNode> createHigherLevel(std::vector<FileBufferTreeNode> nodes);

        std::ifstream file;
        FileBufferTreeNode root;
    };
}