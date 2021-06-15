#version 330 core
out vec4 FragColor;
in vec4 oColor;
in vec2 oTexcoord;
flat in int oSelected;
void main()
{
    const float blurredEdge = 0.025;

    vec2 moved = oTexcoord - vec2(0.5);
    float len = length(moved);
    float edgeMix = clamp(len - (0.5 - blurredEdge), 0.0, blurredEdge) / blurredEdge;

    FragColor = oColor;
    FragColor.a = mix(1.0, 0.0, edgeMix);

	if(oSelected == 1){
		FragColor.r = 1.0;		
	}

}
