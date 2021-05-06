#version 330 core

layout (location = 1) in vec3 aPosition;
layout (location = 0) in uint aType;

#define PARTICLE_TYPE_BOUNDARY 1u
#define PARTICLE_TYPE_DEAD 2u

uniform vec4 particleColor;
uniform vec4 boundaryColor;


out VS_OUT {
    vec4 color;
    int discarded;
} vs_out;


void main()
{

    vs_out.discarded = 0;	
   
    vs_out.color = particleColor;
    // vs_out.color.r = abs(aPosition.z) / 4.0f - 1.0f;
    // vs_out.color.g = abs(aPosition.x) / 4.0f;

    if(aType == PARTICLE_TYPE_DEAD) {
        vs_out.discarded = 1;
    } else if(aType == PARTICLE_TYPE_BOUNDARY) {
        vs_out.color = boundaryColor;
    }

    
    gl_Position =  vec4(aPosition, 1.0);

}
