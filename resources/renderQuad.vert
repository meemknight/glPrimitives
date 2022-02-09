#version 330 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec2 uvs;

out vec2 v_uvs;

void main()
{

	gl_Position.xyzw = vec4(pos,1);
	v_uvs = uvs;

}