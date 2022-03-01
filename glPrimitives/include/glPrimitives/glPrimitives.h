#pragma once
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include <vector>
#include <glad/glad.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include <cmath>

namespace glPrimitives
{

	namespace internal 
	{
	
		struct Shader
		{
			GLuint id = 0;

			bool loadShaderProgramFromMemory(const char *vertexShader, const char *fragmentShader);

			void bind();

			void clear();
		};

		GLint getUniform(GLuint id, const char *name);
	
	};


	constexpr float PI = 3.1415926535897932384626433;

	struct Camera
	{
		Camera() = default;
		Camera(float aspectRatio, float fovRadians)
			:aspectRatio(aspectRatio),
			fovRadians(fovRadians)
		{
		}

		glm::vec3 up = {0.f,1.f,0.f};

		float aspectRatio = 1;
		float fovRadians = glm::radians(60.f);

		float closePlane = 0.01f;
		float farPlane = 200.f;


		glm::vec3 position = {};
		glm::vec3 viewDirection = {0,0,-1};

		glm::mat4x4 getProjectionMatrix();

		glm::mat4x4 getWorldToViewMatrix();

		void rotateCamera(const glm::vec2 delta);

		void moveFPS(glm::vec3 direction);
		
		void moveFly(glm::vec3 direction);

		bool operator==(const Camera &other)
		{
			return
				(up == other.up)
				&& (aspectRatio == other.aspectRatio)
				&& (fovRadians == other.fovRadians)
				&& (closePlane == other.closePlane)
				&& (farPlane == other.farPlane)
				&& (position == other.position)
				&& (viewDirection == other.viewDirection)
				;
		};

		bool operator!=(const Camera &other)
		{
			return !(*this == other);
		};


	};

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

	struct PrimitiveRenderer
	{
		//inits the renderer
		void init(int w, int h);

		//updates window size
		void updateWindowMetrics(int w, int h);

		//renderes the primitives
		void flush(Camera &camera);

		int w = 0;
		int h = 0;

		constexpr static int DEPTH_PEEL_PASSES = 3;
		GLuint fbos[DEPTH_PEEL_PASSES];
		GLuint colorBuff[DEPTH_PEEL_PASSES];
		GLuint depthBuff[DEPTH_PEEL_PASSES];

		//draws a wired triangle. 
		void drawTriangleMesh(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color);
		
		//draws a solid triangle
		void drawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);
		
		//draws a solid triangle and a border around it
		void drawTriangleWithBorder(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color);
		
		//draws a line
		void drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color);
		
		//draws a point
		void drawPoint(glm::vec3 a, glm::vec3 color);

		std::vector<Vertex> triangleDataSolid;
		std::vector<VertexTransparent> triangleDataTransparent;
		std::vector<Vertex> lineData;
		std::vector<Vertex> pointData;

		internal::Shader colorShader;
		GLint u_viewProjection = -1;

		internal::Shader depthPeelShader;
		GLint depthPeel_u_viewProjection = -1;
		GLint depthPeel_u_depthTexture = -1;
		GLint depthPeel_u_skip = -1;

		internal::Shader renderQuadShader;
		GLint renderQuad_u_texture = -1;

		GLuint vertexData;
		GLuint vertexDataTransparent;
		GLuint vertexDataQuad;
		GLuint vao;
		GLuint vaoTransparent;
		GLuint vaoQuad;
	};

};