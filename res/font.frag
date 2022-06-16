#version 330 core

in vec2 fUV;

uniform sampler2D tSDF;
uniform vec4 glyphCol;

const float smoothing = 0.1;
const float buf = 0.1;

out vec4 colour;

void main() {
    float dist = texture(tSDF, fUV).r;
    float alpha = smoothstep(buf - smoothing, buf + smoothing, dist);
    colour = glyphCol * alpha;
}
