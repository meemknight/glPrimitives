#pragma once

inline void barycentric(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float& u, float& v)
{
	float x = p.x;
	float y = p.y;

	float a = p0.x - p1.x;
	float b = p0.x - p2.x;
	float c = p0.x - x;
	float d = p0.y - p1.y;
	float e = p0.y - p2.y;
	float f = p0.y - y;

	float div = (a * e - b * d);

	if (div == 0)
	{
		u = 0;
		v = 0;
	}
	else
	{
		u = (c * e - b * f) / div;
		v = (f * a - d * c) / div;
	}

	//get the point back:
	//point = u * (p1-p0) + v * (p1-p0) + p0;
};
