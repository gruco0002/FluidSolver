#version 330 core

layout (location = 1) in vec3 aPosition;
layout (location = 0) in uint aType;

#define PARTICLE_TYPE_BOUNDARY 1u
#define PARTICLE_TYPE_DEAD 2u

uniform vec4 particleColor;
uniform vec4 boundaryColor;

uniform int showParticleMemoryLocation;
uniform float numberOfParticles;


out VS_OUT {
    vec4 color;
    int discarded;
} vs_out;


void main()
{

    vs_out.discarded = 0;   
    vs_out.color = particleColor;

    if(aType == PARTICLE_TYPE_DEAD) {
        vs_out.discarded = 1;
    } else if(aType == PARTICLE_TYPE_BOUNDARY) {
        vs_out.color = boundaryColor;
    }

    if(showParticleMemoryLocation == 1){
        float vertID = float(gl_VertexID);
        float third = vertID / (numberOfParticles / 3.0);
        vs_out.color.r = clamp(third, 0.0, 1.0);
        vs_out.color.g = clamp(third - 1.0f, 0.0, 1.0);
        vs_out.color.b = clamp(third - 2.0f, 0.0, 1.0);
        vs_out.color.a = 1.0;
    }
    
    gl_Position =  vec4(aPosition, 1.0);

}
