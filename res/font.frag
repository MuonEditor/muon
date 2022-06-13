#version 330 core

in vec2 fTexCoord;

uniform sampler2D text;
uniform vec4 glyphCol;

out vec4 colour;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, fTexCoord).r);
    colour = glyphCol * sampled;
}
