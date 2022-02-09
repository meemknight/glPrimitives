#version 330 core

out layout(location = 0) vec4 outColor;
in vec4 v_color;

void main()
{

	outColor.rgba = v_color;

}