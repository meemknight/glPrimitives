#pragma once
#include "shader.h"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "camera.h"
#include <vector>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
};

struct VertexTransparent
{
	glm::vec4 pos; //not used last component
	glm::vec4 color;
};

struct ShapeDraw
{
	void init(int w, int h);

	void updateWindowMetrics(int w, int h);

	void flush(Camera &camera);

	int w = 0;
	int h = 0;

	constexpr static int DEPTH_PEEL_PASSES = 3;
	GLuint fbos[DEPTH_PEEL_PASSES];
	GLuint colorBuff[DEPTH_PEEL_PASSES];
	GLuint depthBuff[DEPTH_PEEL_PASSES];

	void drawTriangleMesh(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color);
	void drawTriangleSolid(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color);
	void drawTriangleTransparent(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);
	void drawTriangleFancy(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);
	void drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color);
	void drawPoint(glm::vec3 a, glm::vec3 color);

	std::vector<Vertex> triangleDataSolid;
	std::vector<VertexTransparent> triangleDataTransparent;
	std::vector<Vertex> lineData;
	std::vector<Vertex> pointData;

	Shader colorShader;
	GLint u_viewProjection = -1;

	Shader depthPeelShader;
	GLint depthPeel_u_viewProjection = -1;
	GLint depthPeel_u_depthTexture = -1;
	GLint depthPeel_u_skip = -1;

	Shader renderQuadShader;
	GLint renderQuad_u_texture = -1;

	GLuint vertexData;
	GLuint vertexDataTransparent;
	GLuint vertexDataQuad;
	GLuint vao;
	GLuint vaoTransparent;
	GLuint vaoQuad;
};