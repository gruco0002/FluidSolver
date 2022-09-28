#version 330 core

layout (location = 1) in vec3 aPosition;
layout (location = 0) in uint aType;
layout (location = 4) in uint aTag;

#define PARTICLE_TYPE_BOUNDARY 1u
#define PARTICLE_TYPE_DEAD 2u

uniform vec4 particleColor;
uniform vec4 boundaryColor;

uniform int showParticleMemoryLocation;
uniform float numberOfParticles;

uniform int selectedTag;


out VS_OUT {
    vec4 color;
    int discarded;
    int is_selected;
} vs_out;


vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{

    vs_out.discarded = 0;   
    vs_out.color = particleColor;
    vs_out.is_selected = 0;

    if(int(aTag) == selectedTag) {
        vs_out.is_selected = 1;
    }

    if(aType == PARTICLE_TYPE_DEAD) {
        vs_out.discarded = 1;
    } else if(aType == PARTICLE_TYPE_BOUNDARY) {
        vs_out.color = boundaryColor;
    }

    if(showParticleMemoryLocation == 1){
        float vertID = float(gl_VertexID);
        float partOfScale = vertID / numberOfParticles;      
        vs_out.color.rgb = hsv2rgb(vec3(partOfScale, 1.0, 1.0));
        vs_out.color.a = 1.0;
    }
    
    gl_Position =  vec4(aPosition, 1.0);

}
