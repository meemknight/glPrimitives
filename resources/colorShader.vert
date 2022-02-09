#version 330 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec4 color;

uniform mat4 u_viewProjection;

out vec4 v_color;

void main()
{

	gl_Position.xyzw = u_viewProjection * vec4(pos,1);
	v_color = color;

}