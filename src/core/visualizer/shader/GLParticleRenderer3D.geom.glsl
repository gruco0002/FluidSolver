#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float pointSize;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec4 oColor;
out vec2 oTexcoord;

in VS_OUT {
    vec4 color;
    int discarded;
} gs_in[];

void main(){

    if(gs_in[0].discarded == 1) {
        return;
    }

    oColor = gs_in[0].color;
    vec4 position = viewMatrix * gl_in[0].gl_Position;

    gl_Position = projectionMatrix * (position + vec4(0.5 * pointSize, -0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(1.0, 1.0);
    EmitVertex();
    gl_Position = projectionMatrix * (position + vec4(-0.5 * pointSize, -0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position = projectionMatrix * (position + vec4(0.5 * pointSize, 0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(1.0, 0.0);
    EmitVertex();
    gl_Position = projectionMatrix * (position + vec4(-0.5 * pointSize, 0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(0.0, 0.0);
    EmitVertex();
    EndPrimitive();

}
