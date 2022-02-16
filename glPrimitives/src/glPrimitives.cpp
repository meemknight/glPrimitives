#include <glPrimitives/glPrimitives.h>
#include <iostream>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat3x3.hpp>
#include <glm/gtx/transform.hpp>

namespace glPrimitives
{

	namespace internal
	{

		GLint createShaderFromData(const char *data, GLenum shaderType)
		{
			GLuint shaderId = glCreateShader(shaderType);
			glShaderSource(shaderId, 1, &data, nullptr);
			glCompileShader(shaderId);

			GLint rezult = 0;
			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &rezult);

			if (!rezult)
			{
				char *message = 0;
				int   l = 0;

				glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &l);

				if (l)
				{
					message = new char[l];

					glGetShaderInfoLog(shaderId, l, &l, message);

					message[l - 1] = 0;

					std::cout << data << ":\n" << message << "\n";

					delete[] message;

				}
				else
				{
					std::cout << data << ":\n" << "unknown error" << "\n";
				}

				glDeleteShader(shaderId);

				shaderId = 0;
				return shaderId;
			}

			return shaderId;

		}

		GLint createShaderFromFile(const char *source, GLenum shaderType)
		{
			std::ifstream file;
			file.open(source);

			if (!file.is_open())
			{
				std::cout << "Error openning file: " << source << "\n";
				return 0;
			}

			GLint size = 0;
			file.seekg(0, file.end);
			size = file.tellg();
			file.seekg(0, file.beg);

			char *fileContent = new char[size + 1]{};

			file.read(fileContent, size);


			file.close();

			auto rez = createShaderFromData(fileContent, shaderType);

			delete[] fileContent;

			return rez;

		}

		bool Shader::loadShaderProgramFromMemory(const char *vertexShader, const char *fragmentShader)
		{
			auto vertexId = createShaderFromData(vertexShader, GL_VERTEX_SHADER);
			auto fragmentId = createShaderFromData(fragmentShader, GL_FRAGMENT_SHADER);

			if (vertexId == 0 || fragmentId == 0)
			{
				return 0;
			}

			id = glCreateProgram();

			glAttachShader(id, vertexId);
			glAttachShader(id, fragmentId);

			glLinkProgram(id);

			glDeleteShader(vertexId);
			glDeleteShader(fragmentId);

			GLint info = 0;
			glGetProgramiv(id, GL_LINK_STATUS, &info);

			if (info != GL_TRUE)
			{
				char *message = 0;
				int   l = 0;

				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &l);

				message = new char[l];

				glGetProgramInfoLog(id, l, &l, message);

				std::cout << "Link error: " << message << "\n";

				delete[] message;

				glDeleteProgram(id);
				id = 0;
				return 0;
			}

			glValidateProgram(id);

			return true;
		}

		void Shader::bind()
		{
			glUseProgram(id);
		}

		void Shader::clear()
		{
			glDeleteProgram(id);
			id = 0;
		}

		GLint getUniform(GLuint id, const char *name)
		{
			GLint uniform = glGetUniformLocation(id, name);
			if (uniform == -1)
			{
				std::cout << "uniform error " << name << "\n";
			}
			return uniform;
		};

	};



	glm::mat4x4 Camera::getProjectionMatrix()
	{
		auto mat = glm::perspective(this->fovRadians, this->aspectRatio, this->closePlane,
			this->farPlane);

		return mat;
	}

	glm::mat4x4 Camera::getWorldToViewMatrix()
	{
		glm::vec3 lookingAt = this->position;
		lookingAt += viewDirection;


		auto mat = glm::lookAt(this->position, lookingAt, this->up);
		return mat;
	}

	void Camera::rotateCamera(const glm::vec2 delta)
	{

		glm::vec3 rotateYaxe = glm::cross(viewDirection, up);

		viewDirection = glm::mat3(glm::rotate(delta.x, up)) * viewDirection;

		if (delta.y < 0)
		{	//down
			if (viewDirection.y < -0.99)
				goto noMove;
		}
		else
		{	//up
			if (viewDirection.y > 0.99)
				goto noMove;
		}

		viewDirection = glm::mat3(glm::rotate(delta.y, rotateYaxe)) * viewDirection;
noMove:

		viewDirection = glm::normalize(viewDirection);
	}


	void Camera::moveFPS(glm::vec3 direction)
	{
		viewDirection = glm::normalize(viewDirection);

		//forward
		float forward = -direction.z;
		float leftRight = direction.x;
		float upDown = direction.y;

		glm::vec3 move = {};

		move += up * upDown;
		move += glm::normalize(glm::cross(viewDirection, up)) * leftRight;
		move += viewDirection * forward;

		this->position += move;

	}

	const char *colorShaderVert =
		R"(
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
		
)";

	const char *colorShaderFrag =
		R"(
#version 330 core

out layout(location = 0) vec4 outColor;
in vec4 v_color;

void main()
{
	outColor.rgba = v_color;
}
)";

	const char *depthPeelVert =
		R"(
#version 330 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec4 color;

uniform mat4 u_viewProjection;

out vec4 v_color;
out vec4 v_pos;

void main()
{

	gl_Position.xyzw = u_viewProjection * vec4(pos,1);
	v_color = color;
	v_pos = gl_Position;
}
)";

	const char *depthPeelFrag =
		R"(
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
)";

	const char *renderQuadVert = R"(
#version 330 core

in layout(location = 0) vec3 pos;
in layout(location = 1) vec2 uvs;

out vec2 v_uvs;

void main()
{

	gl_Position.xyzw = vec4(pos,1);
	v_uvs = uvs;

}
)";

	const char *renderQuadFrag = R"(
#version 330 core

out layout(location = 0) vec4 outColor;

in vec2 v_uvs;

uniform sampler2D u_texture;

void main()
{

	vec4 color = texture(u_texture, v_uvs.xy);
	outColor.rgba = color;

}
)";

	
	void PrimitiveRenderer::init(int w, int h)
	{
		colorShader.loadShaderProgramFromMemory(colorShaderVert, colorShaderFrag);


		depthPeelShader.loadShaderProgramFromMemory
		(
			depthPeelVert,
			depthPeelFrag
		);
	
		renderQuadShader.loadShaderProgramFromMemory
		(
			renderQuadVert,
			renderQuadFrag
		);
	
		u_viewProjection = getUniform(colorShader.id, "u_viewProjection");
	
		depthPeel_u_viewProjection = getUniform(depthPeelShader.id, "u_viewProjection");
		depthPeel_u_depthTexture = getUniform(depthPeelShader.id, "u_depthTexture");
		depthPeel_u_skip = getUniform(depthPeelShader.id, "u_skip");	
	
		renderQuad_u_texture = getUniform(renderQuadShader.id, "u_texture");
	
		glGenBuffers(1, &vertexData);
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vertexData);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void*)12);
		
		glGenBuffers(1, &vertexDataTransparent);
		glGenVertexArrays(1, &vaoTransparent);
		glBindVertexArray(vaoTransparent);
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataTransparent);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 32, (void*)16);
	
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
	
		glGenBuffers(1, &vertexDataQuad);
		glGenVertexArrays(1, &vaoQuad);
		glBindVertexArray(vaoQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	
		glBindVertexArray(0);
	
		glGenFramebuffers(DEPTH_PEEL_PASSES, fbos);
		glGenTextures(DEPTH_PEEL_PASSES, colorBuff);
		glGenTextures(DEPTH_PEEL_PASSES, depthBuff);
	
		updateWindowMetrics(w, h);
	
	
	}
	
	void PrimitiveRenderer::updateWindowMetrics(int w, int h)
	{
		if (this->w != w || this->h != h)
		{
			this->w = w;
			this->h = h;
			
			for (int i = 0; i < DEPTH_PEEL_PASSES; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
				glBindTexture(GL_TEXTURE_2D, colorBuff[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuff[i], 0);
	
				//glBindRenderbuffer(GL_RENDERBUFFER, depthBuff[i]);
				//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
				//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuff[i]);
	
				glBindTexture(GL_TEXTURE_2D, depthBuff[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuff[i], 0);
	
	
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					std::cout << "Errors on framebuffer: "<< i << "\n";
				}
	
	
			}
	
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
		}
	}
	
	void PrimitiveRenderer::flush(Camera& camera)
	{	
	
	#pragma region render solid geometry
	
		glEnable(GL_DEPTH_TEST);
	
		glBindFramebuffer(GL_FRAMEBUFFER, fbos[DEPTH_PEEL_PASSES - 1]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		auto projMat = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
	
		colorShader.bind();
		glUniformMatrix4fv(u_viewProjection, 1, GL_FALSE,
			&(projMat)[0][0]);
		glBindVertexArray(vao);
	
		glBindBuffer(GL_ARRAY_BUFFER, vertexData);
		glBufferData(GL_ARRAY_BUFFER, 
			sizeof(lineData[0]) * lineData.size(), lineData.data(),
			GL_STREAM_DRAW);
		
		glDrawArrays(GL_LINES, 0, lineData.size());
	
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(pointData[0]) * pointData.size(), pointData.data(),
			GL_STREAM_DRAW);
		glDrawArrays(GL_POINTS, 0, pointData.size());
	
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(triangleDataSolid[0]) * triangleDataSolid.size(), triangleDataSolid.data(),
			GL_STREAM_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, triangleDataSolid.size());
	
	#pragma endregion
	
	#pragma region render transparent geometry
	
		glBindVertexArray(vaoTransparent);
		depthPeelShader.bind();
		glUniformMatrix4fv(depthPeel_u_viewProjection, 1, GL_FALSE,
			&(projMat)[0][0]);
		glUniform1i(depthPeel_u_depthTexture, 0);
		glActiveTexture(GL_TEXTURE0);
	
		glBindBuffer(GL_ARRAY_BUFFER, vertexDataTransparent);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(triangleDataTransparent[0]) * triangleDataTransparent.size(), triangleDataTransparent.data(),
			GL_STREAM_DRAW);
	
		int peels = 0;
	
		for (int i = 0; i < DEPTH_PEEL_PASSES-1; i++)
		{
	
			glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fbos[DEPTH_PEEL_PASSES-1]);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbos[i]); // write to default framebuffer
			glBlitFramebuffer(
			  0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST
			);
	
			if (i == 0)
			{
	
				//glBindTexture(GL_TEXTURE_2D, depthBuff[DEPTH_PEEL_PASSES-1]);
				glUniform1i(depthPeel_u_skip, 1);
	
			}
			else
			{
				glUniform1i(depthPeel_u_skip, 0);
				glBindTexture(GL_TEXTURE_2D, depthBuff[i - 1]);
			}
			
			//todo opengl query fragments
			glDrawArrays(GL_TRIANGLES, 0, triangleDataTransparent.size());
			
		}
	
	
	#pragma endregion
	
	
	#pragma region render quads in reverse order
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		renderQuadShader.bind();
		glBindVertexArray(vaoQuad);
		glUniform1i(renderQuad_u_texture, 0);
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		for (int i = DEPTH_PEEL_PASSES - 1; i >= 0; i--)
		{
			glBindTexture(GL_TEXTURE_2D, colorBuff[i]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	
	#pragma endregion
	
		
		//glBindVertexArray(vaoTransparent);
		//glEnable(GL_BLEND);
		//glBlendEquation(GL_FUNC_ADD);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//
		//glBindBuffer(GL_ARRAY_BUFFER, vertexDataTransparent);
		//glBufferData(GL_ARRAY_BUFFER,
		//	sizeof(triangleDataTransparent[0]) * triangleDataTransparent.size(), triangleDataTransparent.data(),
		//	GL_STREAM_DRAW);
		//glDrawArrays(GL_TRIANGLES, 0, triangleDataTransparent.size());
		//
		glDisable(GL_BLEND);
	
	
		glBindVertexArray(0);
		lineData.clear();
		pointData.clear();
		triangleDataSolid.clear();
		triangleDataTransparent.clear();
	}
	
	void PrimitiveRenderer::drawTriangleMesh(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color)
	{
		drawLine(a, b, color);
		drawLine(b, c, color);
		drawLine(c, a, color);
	}
	
	void PrimitiveRenderer::drawTriangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color)
	{
		if (color.a >= 0.99999)
		{
			triangleDataSolid.push_back({a, color});
			triangleDataSolid.push_back({b, color});
			triangleDataSolid.push_back({c, color});
		}
		else
		{
			triangleDataTransparent.push_back({glm::vec4(a,1), color});
			triangleDataTransparent.push_back({glm::vec4(b,1), color});
			triangleDataTransparent.push_back({glm::vec4(c,1), color});
		}
	}

	
	void PrimitiveRenderer::drawTriangleWithBorder(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color)
	{
		drawTriangleMesh(a, b, c, color);
		drawTriangle(a, b, c, color);
	}
	
	void PrimitiveRenderer::drawLine(glm::vec3 a, glm::vec3 b, glm::vec3 color)
	{
		lineData.push_back({a, color});
		lineData.push_back({b, color});
	}
	
	void PrimitiveRenderer::drawPoint(glm::vec3 a, glm::vec3 color)
	{
		pointData.push_back({a, color});
	}

};