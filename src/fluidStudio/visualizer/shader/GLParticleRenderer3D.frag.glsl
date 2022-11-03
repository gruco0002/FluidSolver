#version 330 core

in vec4 oColor;
in vec3 oNormal;
flat in int o_is_selected;

uniform vec3 lightDirection;
uniform mat4 viewMatrix;
uniform float ambientLightFactor;

out vec4 FragColor;

void main()
{
    float lightFactor = clamp(ambientLightFactor + clamp(dot(oNormal, -lightDirection.xyz), 0.0, 1.0), 0.0, 1.0); 

    FragColor = oColor * lightFactor;
    FragColor.a = 1.0;

    if(o_is_selected == 1){
        FragColor.rgb = vec3(1.0, 0.0, 0.0);
    }
}
