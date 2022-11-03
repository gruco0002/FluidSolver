#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aVelocity;
layout (location = 2) in vec2 aAcceleration;
layout (location = 3) in float aMass;
layout (location = 4) in float aPressure;
layout (location = 5) in float aDensity;
layout (location = 6) in uint aType;
// layout (location = 7) in int aIsSelected;


#define COLOR_SELECTION_VELOCITY 0
#define COLOR_SELECTION_ACCELERATION 1
#define COLOR_SELECTION_MASS 2
#define COLOR_SELECTION_PRESSURE 3
#define COLOR_SELECTION_DENSITY 4

#define PARTICLE_TYPE_BOUNDARY 1u
#define PARTICLE_TYPE_INACTIVE 2u


uniform int colorSelection;
uniform vec4 bottomColor;
uniform vec4 topColor;
uniform float bottomValue;
uniform float topValue;
uniform vec4 boundaryColor;
uniform int showParticleSelection;
uniform float numberOfParticles;
uniform int showParticleMemoryLocation;


out VS_OUT {
    vec4 color;
    int discarded;
	int selected;
} vs_out;


void main()
{

    vs_out.discarded = 0;
	vs_out.selected = 0;

    // determine selection
    //if(showParticleSelection + aIsSelected == 2){
    //    vs_out.selected = 1;
    //}


    float val = 0.0;
    if(colorSelection == COLOR_SELECTION_VELOCITY){
        val = length(aVelocity);
    } else if(colorSelection == COLOR_SELECTION_ACCELERATION){
        val = length(aAcceleration);
    } else if(colorSelection == COLOR_SELECTION_MASS){
        val = aMass;
    } else if(colorSelection == COLOR_SELECTION_PRESSURE){
        val = aPressure;
    } else if(colorSelection == COLOR_SELECTION_DENSITY){
        val = aDensity;
    }

    val = clamp(val - bottomValue, 0.0, (topValue - bottomValue)) / (topValue - bottomValue);
    vs_out.color = mix(bottomColor, topColor, val);

    if(aType == PARTICLE_TYPE_INACTIVE) {
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

    gl_Position =  vec4(aPosition, 0.0, 1.0);

}
