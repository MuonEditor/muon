#type vertex
#version 330 core

layout (location = 0) in vec4 vtx; // position and texture

out vec2 fUV;

void main() {
    gl_Position = vec4(vtx.xy, 0.0, 1.0);
    fUV = vtx.zw;
}


#type fragment
#version 330 core

in vec2 fUV;

uniform sampler2D tSDF;
uniform vec4 glyphCol;

// These should maybe be uniforms but for now
const float smoothing = 0.05;
const float buf = 0.2;

out vec4 colour;

void main() {
    float dist = texture(tSDF, fUV).r;
    float alpha = smoothstep(buf - smoothing, buf + smoothing, dist);
    colour = glyphCol * alpha;
}
