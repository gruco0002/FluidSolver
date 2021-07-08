#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 29) out;

uniform float pointSize;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec4 oColor;
out vec3 oNormal;

in VS_OUT {
    vec4 color;
    int discarded;
} gs_in[];

void main(){

    if(gs_in[0].discarded == 1) {
        return;
    }

    oColor = gs_in[0].color;
    vec3 position = gl_in[0].gl_Position.xyz;

    const float GOLDEN_RATIO = 1.61803;
    const vec3 ICOSAHEDRON_COORDINATES[12] = vec3[](
        vec3(0.000000, -1.000000, 1.618034),
        vec3(0.000000, 1.000000, 1.618034),
        vec3(-1.618034, 0.000000, 1.000000),
        vec3(1.618034, 0.000000, 1.000000),
        vec3(1.000000, 1.618034, 0.000000),
        vec3(-1.000000, 1.618034, 0.000000),
        vec3(-1.000000, -1.618034, 0.000000),
        vec3(0.000000, -1.000000, -1.618034),
        vec3(1.000000, -1.618034, 0.000000),
        vec3(1.618034, 0.000000, -1.000000),
        vec3(0.000000, 1.000000, -1.618034),
        vec3(-1.618034, 0.000000, -1.000000)
    );

    const float SCALE = 0.5 / 1.618034;

    const int ICOSAHEDRON_FACES[29] = int[](
        0, 1, 2, 0, 6, 2, 11, 5, 2, 1, 5, 4, 1, 3, 0, 8, 6, 7, 11, 10, 7, 9, 8, 3, 9, 4, 10, 5, 11
    );

    for(int i = 0; i < 29; i++){
        gl_Position = projectionMatrix * viewMatrix * vec4((position + SCALE * ICOSAHEDRON_COORDINATES[ICOSAHEDRON_FACES[i]]), 1.0);
        oNormal = normalize(ICOSAHEDRON_COORDINATES[ICOSAHEDRON_FACES[i]]);
        EmitVertex();     
           
    }
 EndPrimitive();

}
