#version 330 core

out layout(location = 0) vec4 outColor;
in vec4 v_color;
in vec4 v_pos;

uniform sampler2D u_depthTexture;
uniform int u_skip;

void main()
{

	if(u_skip==0)
	{
		vec2 p = v_pos.xy / v_pos.w;
		p += 1;
		p/=2;

		if (gl_FragCoord.z <= texture(u_depthTexture, p).x) 
			discard; //Manually performing the GL_GREATER depth test for each pixel
	}


	outColor.rgba = v_color;

}