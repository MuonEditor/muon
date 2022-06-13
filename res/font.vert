#version 330 core

layout (location = 0) in vec4 vtx; // position and texture

out vec2 fTexCoord;

void main() {
    gl_Position = vec4(vtx.xy, 0.0, 1.0);
    fTexCoord = vtx.zw;
}
