#version 330 core

out layout(location = 0) vec4 outColor;

in vec2 v_uvs;

uniform sampler2D u_texture;

void main()
{

	vec4 color = texture(u_texture, v_uvs.xy);
	outColor.rgba = color;

}