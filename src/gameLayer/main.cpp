#include "gameLayer.h"
#include "gl2d/gl2d.h"
#include "platformInput.h"
#include <glPrimitives/glPrimitives.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "camera.h"
#include "myMath.h"

gl2d::Renderer2D renderer;

gl2d::Font font;
gl2d::Texture texture;

struct SaveData
{
	glPrimitives::Camera camera;
	bool drawMesh = 0;
	glm::vec3 ta1;
	glm::vec3 ta2;
	glm::vec3 ta3;
	glm::vec4 triangleColor1;

	glm::vec3 tb1;
	glm::vec3 tb2;
	glm::vec3 tb3;
	glm::vec4 triangleColor2;

	glm::vec2 verticalLine;
	glm::vec3 verticalLineColor;

	glm::vec3 pointColor;

	glm::vec3 tc1;
	glm::vec3 tc2;
	glm::vec3 tc3;
	glm::vec4 triangleColor3;

}saveData;

glPrimitives::PrimitiveRenderer debugDraw;

bool initGame()
{
	renderer.create();
	font.createFromFile(RESOURCES_PATH "roboto_black.ttf");
	texture.loadFromFile(RESOURCES_PATH "test.jpg");
	
	saveData = SaveData();

	if(!platform::readEntireFile(RESOURCES_PATH "saveData.bin", &saveData, sizeof(SaveData)))
	{
	}

	debugDraw.init(1, 1);

	return true;
}

bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getWindowSizeX();
	h = platform::getWindowSizeY();
	glViewport(0, 0, w, h);
	saveData.camera.aspectRatio = (float)w / h;

	debugDraw.updateWindowMetrics(w, h);

	//renderer.updateWindowMetrics(w, h);
	//renderer.clearScreen();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_LINE_WIDTH);
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);
	
	glPointSize(20);
	glLineWidth(4);

#pragma endregion

#pragma region imgui

	ImGui::Begin("Editor");

	ImGui::ColorEdit4("Triangle color", &saveData.triangleColor1[0], ImGuiColorEditFlags_AlphaBar);
	ImGui::DragFloat3("Triangle1", &saveData.ta1[0], 0.01);
	ImGui::DragFloat3("Triangle2", &saveData.ta2[0], 0.01);
	ImGui::DragFloat3("Triangle3", &saveData.ta3[0], 0.01);
	ImGui::NewLine();

	ImGui::ColorEdit4("Triangle color 2", &saveData.triangleColor2[0], ImGuiColorEditFlags_AlphaBar);
	ImGui::DragFloat3("Triangle1 2", &saveData.tb1[0], 0.01);
	ImGui::DragFloat3("Triangle2 2", &saveData.tb2[0], 0.01);
	ImGui::DragFloat3("Triangle3 2", &saveData.tb3[0], 0.01);
	ImGui::NewLine();

	ImGui::ColorEdit3("Triangle color 3", &saveData.triangleColor3[0]);
	ImGui::DragFloat3("Triangle1 3", &saveData.tc1[0], 0.01);
	ImGui::DragFloat3("Triangle2 3", &saveData.tc2[0], 0.01);
	ImGui::DragFloat3("Triangle3 3", &saveData.tc3[0], 0.01);
	saveData.triangleColor3.a = 1.f;
	ImGui::NewLine();

	ImGui::ColorEdit3("Vertical line color", &saveData.verticalLineColor[0]);
	ImGui::DragFloat2("Vertical line", &saveData.verticalLine[0], 0.01);
	ImGui::NewLine();

	ImGui::ColorEdit3("Point color", &saveData.pointColor[0]);
	ImGui::NewLine();


	ImGui::Checkbox("Draw mesh", &saveData.drawMesh);


	ImGui::End();

#pragma endregion


#pragma region input

	float speed = 4;
	glm::vec3 dir = {};
	if (platform::isKeyHeld(platform::Button::W))
	{
		dir.z -= speed * deltaTime;
	}
	if (platform::isKeyHeld(platform::Button::S))
	{
		dir.z += speed * deltaTime;
	}

	if (platform::isKeyHeld(platform::Button::A))
	{
		dir.x -= speed * deltaTime;
	}
	if (platform::isKeyHeld(platform::Button::D))
	{
		dir.x += speed * deltaTime;
	}
	if (platform::isKeyHeld(platform::Button::Q))
	{
		dir.y -= speed * deltaTime;
	}
	if (platform::isKeyHeld(platform::Button::E))
	{
		dir.y += speed * deltaTime;
	}

	saveData.camera.moveFPS(dir);

	{
		static glm::vec2 lastMousePos = {};
		if (platform::isRMouseHeld())
		{
			glm::vec2 currentMousePos = platform::getRelMousePosition();
			float speed = 0.8f;

			glm::vec2 delta = lastMousePos - currentMousePos;
			delta *= speed * deltaTime;

			saveData.camera.rotateCamera(delta);

			lastMousePos = currentMousePos;
		}
		else
		{
			lastMousePos = platform::getRelMousePosition();
		}
	}

#pragma endregion

	glm::vec3 point{};
	float u = 0;
	float v = 0;

	glm::vec3 dir1 = saveData.ta2 - saveData.ta1;
	glm::vec3 dir2 = saveData.ta3 - saveData.ta1;

	barycentric(
		{saveData.verticalLine.x,saveData.verticalLine.y},
		{saveData.ta1.x,saveData.ta1.z},
		{saveData.ta2.x,saveData.ta2.z},
		{saveData.ta3.x,saveData.ta3.z},
		u, v
	);

	point = u * dir1 + v * dir2 + saveData.ta1;

#pragma region draw stuff


	debugDraw.drawTriangleWithBorder(saveData.ta1, saveData.ta2, saveData.ta3,
		saveData.triangleColor1);

	debugDraw.drawTriangleWithBorder(saveData.tb1, saveData.tb2, saveData.tb3,
		saveData.triangleColor2);

	debugDraw.drawTriangle(saveData.tc1, saveData.tc2, saveData.tc3,
		saveData.triangleColor3);

	debugDraw.drawLine(
		glm::vec3(saveData.verticalLine.x, 0, saveData.verticalLine.y),
		glm::vec3(saveData.verticalLine.x, 5, saveData.verticalLine.y),
		saveData.verticalLineColor
		);

	debugDraw.drawPoint(point, saveData.pointColor);


#pragma endregion


#pragma region draw mesh

	if (saveData.drawMesh)
	{
		float size = 1.f;
		int lineCount = 5;

		for (int i = -lineCount; i <= lineCount; i++)
		{
			debugDraw.drawLine(glm::vec3{i,0, -lineCount}*size,
				glm::vec3{i,0, lineCount}*size, {0.4, 0.4, 0.45});

			debugDraw.drawLine(glm::vec3{-lineCount,0, i}*size,
				glm::vec3{lineCount,0, i}*size, {0.4, 0.4, 0.45});

		}


	
	}

#pragma endregion

#pragma region set finishing stuff

	debugDraw.flush(saveData.camera);

	return true;
#pragma endregion

}

void closeGame()
{

	platform::writeEntireFile(RESOURCES_PATH "saveData.bin", &saveData, sizeof(SaveData));

}
