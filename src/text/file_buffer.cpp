#include "file_buffer.h"

using namespace muon;
#define MAX_CHUNK_SIZE  256

_FileBuffer::_FileBuffer(std::string path) {
    // Open file
    file = std::ifstream(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return;
    }

    // Get file size
    size_t len = file.tellg();
    file.seekg(file.beg);

    // Load all file leafs
    char* buf = new char[MAX_CHUNK_SIZE];
    size_t c = 0;
    size_t lastC = 0;
    size_t l = 0;
    std::basic_string<TextChar> data;
    std::vector<FileBufferTreeNode> nodes;
    for (size_t offset = 0; offset < len; offset += MAX_CHUNK_SIZE) {
        // Calculate size and load
        size_t clen = std::min<size_t>(MAX_CHUNK_SIZE, len - offset);
        file.read(buf, clen);

        // Iterate over file to generate the correct 
        for (int i = 0; i < clen; i++) {
            char ch = buf[i];
            if (ch == '\n') {
                if (!data.empty()) {
                    FileBufferTreeNode n = FileBufferTreeNode(new _FileBufferTreeNode);
                    n->data = data;
                    n->rect.start.c = lastC;
                    n->rect.start.l = l;
                    n->rect.end.c = lastC + data.length() - 1;
                    n->rect.end.l = l;
                    n->sub0 = NULL;
                    n->sub1 = NULL;
                    nodes.push_back(n);
                }
                data.clear();
                c = 0;
                l++;
                continue;
            }

            data.push_back(TextChar { (uint16_t)ch, TextDecoration::NONE, Color { 1,1,1,1 }, Color { 0,0,0,0 } });
            c++;
        }

        if (!data.empty()) {
            FileBufferTreeNode n = FileBufferTreeNode(new _FileBufferTreeNode);
            n->data = data;
            n->rect.start.c = lastC;
            n->rect.start.l = l;
            n->rect.end.c = lastC + data.length() - 1;
            n->rect.end.l = l;
            n->sub0 = NULL;
            n->sub1 = NULL;
            nodes.push_back(n);
            lastC = c;
        }
    }
    delete[] buf;

    // Create tree
    while (nodes.size() > 1) {
        nodes = createHigherLevel(nodes);
    }

    root = nodes[0];
}

_FileBuffer::~_FileBuffer() {
    // Close file
    file.close();
}

std::vector<TextChunk> _FileBuffer::get(TextRectangle& rect) {
    std::vector<TextChunk> chunks;
    recursiveGet(chunks, root, rect);
    return chunks;
}

void _FileBuffer::recursiveGet(std::vector<TextChunk>& chunks, FileBufferTreeNode node, TextRectangle& rect) {
    // Return if null
    if (!node) { return; }
    
    // Make sure the node intersects the wanted area
    if (!node->rect.intersects(rect)) { return; }

    // If this is not a leaf, query deeper nodes
    if (node->data.empty()) {
        recursiveGet(chunks, node->sub0, rect);
        recursiveGet(chunks, node->sub1, rect);
        return;
    }

    // Get needed data depending on column
    size_t start = std::max<size_t>(rect.start.c - node->rect.start.c, 0);
    size_t end = std::min<size_t>(rect.end.c - node->rect.start.c, node->data.length() - 1);
    chunks.push_back(TextChunk { node->rect.start.l, node->rect.start.c + start, node->data.substr(start, end - start + 1) });
}

std::vector<FileBufferTreeNode> _FileBuffer::createHigherLevel(std::vector<FileBufferTreeNode> nodes) {
    std::vector<FileBufferTreeNode> h;

    int last = nodes.size() - 1;
    for (int i = 0; i < nodes.size(); i += 2) {
        if (i == last) {
            h.push_back(nodes[i]);
            continue;
        }
        FileBufferTreeNode n = FileBufferTreeNode(new _FileBufferTreeNode);
        n->data.clear();
        n->sub0 = nodes[i];
        n->sub1 = nodes[i+1];
        n->rect.start.c = std::min<size_t>(n->sub0->rect.start.c, n->sub1->rect.start.c);
        n->rect.start.l = std::min<size_t>(n->sub0->rect.start.l, n->sub1->rect.start.l);
        n->rect.end.c = std::max<size_t>(n->sub0->rect.start.c, n->sub1->rect.start.c);
        n->rect.end.l = std::max<size_t>(n->sub0->rect.start.l, n->sub1->rect.start.l);
        h.push_back(n);
    }

    return h;
}

size_t _FileBuffer::getWidth() {
    return root->rect.end.c + 1;
}

size_t _FileBuffer::getHeight() {
    return root->rect.end.l + 1;
}