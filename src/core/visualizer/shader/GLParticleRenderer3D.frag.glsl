#version 330 core

in vec4 oColor;
in vec2 oTexcoord;

uniform vec3 lightDirection;
uniform mat4 viewMatrix;
uniform float ambientLightFactor;

out vec4 FragColor;

void main()
{
    const float blurredEdge = 0.025;

    vec2 moved = oTexcoord - vec2(0.5);
    float len = length(moved);
    float edgeMix = clamp(len - (0.5 - blurredEdge), 0.0, blurredEdge) / blurredEdge;

    vec4 lightDirCameraSpace = viewMatrix * vec4(lightDirection, 0.0);

    vec3 normal = normalize(vec3(moved.x, moved.y, 1.0 - len));

    float lightFactor = clamp(ambientLightFactor + clamp(dot(normal, lightDirCameraSpace.xyz), 0.0, 1.0), 0.0, 1.0); 

    FragColor = oColor * lightFactor;
    FragColor.a = mix(1.0, 0.0, edgeMix);	
    if(FragColor.a <= 0.02)
        discard;

    FragColor.a = 1.0;
}
