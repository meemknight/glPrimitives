#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ctime>
#include "platformTools.h"
#include "config.h"
#include "platformInput.h"
#include "gameLayer.h"
#include <fstream>
#include <chrono>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#undef min
#undef max

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{

	bool state = 0;

	if(action == GLFW_PRESS)
	{
		state = 1;
	}else if(action == GLFW_RELEASE)
	{
		state = 0;
	}else
	{
		return;
	}

	if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
	{
		int index = key - GLFW_KEY_A;
		platform::internal::setButtonState(platform::Button::A + index, state);
	}else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
	{
		int index = key - GLFW_KEY_0;
		platform::internal::setButtonState(platform::Button::NR0 + index, state);
	}else
	{
	//special keys
		//GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE, GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT

		if (key == GLFW_KEY_SPACE)
		{
			platform::internal::setButtonState(platform::Button::Space, state);
		}
		else
		if (key == GLFW_KEY_ENTER)
		{
			platform::internal::setButtonState(platform::Button::Enter, state);
		}
		else
		if (key == GLFW_KEY_ESCAPE)
		{
			platform::internal::setButtonState(platform::Button::Escape, state);
		}
		else
		if (key == GLFW_KEY_UP)
		{
			platform::internal::setButtonState(platform::Button::Up, state);
		}
		else
		if (key == GLFW_KEY_DOWN)
		{
			platform::internal::setButtonState(platform::Button::Down, state);
		}
		else
		if (key == GLFW_KEY_LEFT)
		{
			platform::internal::setButtonState(platform::Button::Left, state);
		}
		else
		if (key == GLFW_KEY_RIGHT)
		{
			platform::internal::setButtonState(platform::Button::Right, state);
		}
	
	}
	
};

void mouseCallback(GLFWwindow *window, int key, int action, int mods)
{
	bool state = 0;

	if (action == GLFW_PRESS)
	{
		state = 1;
	}
	else if (action == GLFW_RELEASE)
	{
		state = 0;
	}
	else
	{
		return;
	}

	if(key == GLFW_MOUSE_BUTTON_LEFT)
	{
		platform::internal::setLeftMouseState(state);
	}else
	if (key == GLFW_MOUSE_BUTTON_RIGHT)
	{
		platform::internal::setRightMouseState(state);
	}
	

}

bool windowFocus = 0;

void windowFocusCallback(GLFWwindow *window, int focused)
{
	if (focused)
	{
		windowFocus = 1;
	}
	else
	{
		windowFocus = 0;
		//if you not capture the release event when the window loses focus,
		//the buttons will stay pressed
		platform::internal::resetInputsToZero();
	}
}

void windowSizeCallback(GLFWwindow *window, int x, int y)
{
	platform::internal::resetInputsToZero();
}

int mouseMovedFlag = 0;

void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
	mouseMovedFlag = 1;
}

#pragma region platform functions

GLFWwindow *wind = 0;

namespace platform
{

	//todo test
	void setRelMousePosition(int x, int y)
	{
		glfwSetCursorPos(wind, x, y);

	}

	//todo test
	glm::ivec2 getRelMousePosition()
	{
		double x = 0, y = 0;
		glfwGetCursorPos(wind, &x, &y);

		return { x, y };
	}

	//todo test
	glm::ivec2 getWindowSize()
	{
		int x = 0; int y = 0;

		glfwGetWindowSize(wind, &x, &y);

		return { x, y };
	}

	//todo test
	void showMouse(bool show)
	{
		if(show)
		{
			glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}else
		{
			glfwSetInputMode(wind, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

	}

	bool isFocused()
	{
		return windowFocus;
	}

	bool mouseMoved()
	{
		return mouseMovedFlag;
	}

	bool writeEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ofstream f(name, std::ios::binary);

		if(!f.is_open())
		{
			return 0;
		}

		f.write((char*)buffer, size);

		f.close();

		return 1;
	}


	bool readEntireFile(const char *name, void *buffer, size_t size)
	{
		std::ifstream f(name, std::ios::binary);

		if (!f.is_open())
		{
			return 0;
		}

		f.read((char *)buffer, size);

		f.close();

		return 1;
	}

};
#pragma endregion

void embraceTheDarkness()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(8.00f, 8.00f);
	style.FramePadding = ImVec2(5.00f, 2.00f);
	style.CellPadding = ImVec2(6.00f, 6.00f);
	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.IndentSpacing = 25;
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 7;
	style.ChildRounding = 4;
	style.FrameRounding = 3;
	style.PopupRounding = 4;
	style.ScrollbarRounding = 9;
	style.GrabRounding = 3;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 4;
}

int main()
{

#ifdef _WIN32
#ifdef _MSC_VER 
#if INTERNAL_BUILD
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
	std::cout.sync_with_stdio();
#endif
#endif
#endif

#pragma region window and opengl

	permaAssertComment(glfwInit(), "err initializing glfw");
	glfwWindowHint(GLFW_SAMPLES, 8);

	int w = 500;
	int h = 500;
	wind = glfwCreateWindow(w, h, "geam", nullptr, nullptr);
	glfwMakeContextCurrent(wind);
	glfwSwapInterval(1);

	glfwSetKeyCallback(wind, keyCallback);
	glfwSetMouseButtonCallback(wind, mouseCallback);
	glfwSetWindowFocusCallback(wind, windowFocusCallback);
	glfwSetWindowSizeCallback(wind, windowSizeCallback);
	glfwSetCursorPosCallback(wind, cursorPositionCallback);

	permaAssertComment(gladLoadGL(), "err initializing glad");

#pragma endregion

#pragma region imgui

	ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	embraceTheDarkness();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
		style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

	ImGui_ImplGlfw_InitForOpenGL(wind, true);
	ImGui_ImplOpenGL3_Init("#version 330");

#pragma endregion




#pragma region initGame
	if (!initGame())
	{
		return 0;
	}
#pragma endregion


	//long lastTime = clock();
	
	auto stop = std::chrono::high_resolution_clock::now();

	while (!glfwWindowShouldClose(wind))
	{

	#pragma region deltaTime

		//long newTime = clock();
		//float deltaTime = (float)(newTime - lastTime) / CLOCKS_PER_SEC;
		//lastTime = clock();
		auto start = std::chrono::high_resolution_clock::now();

		float deltaTime = (std::chrono::duration_cast<std::chrono::microseconds>(start - stop)).count() / 1000000.0f;
		stop = std::chrono::high_resolution_clock::now();

		float augmentedDeltaTime = deltaTime;
		if (augmentedDeltaTime > 1.f / 10) { augmentedDeltaTime = 1.f / 10; }
	
	#pragma endregion

	#pragma region imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
	#pragma endregion


	#pragma region game logic

		if (!gameLogic(augmentedDeltaTime))
		{
			return 0;
		}

	#pragma endregion

	#pragma region reset flags

		mouseMovedFlag = 0;
		platform::internal::updateAllButtons();

	#pragma endregion

	#pragma region window stuff

		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(wind, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}


		glfwSwapBuffers(wind);
		glfwPollEvents();


	#pragma endregion

	}

	closeGame();

	//if you want the console to stay after closing the window
	//std::cin.clear();
	//std::cin.get();
}