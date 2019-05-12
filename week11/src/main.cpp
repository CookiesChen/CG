#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.h"
#include "Bresenham.h"
#include "Transformation.h"
#include "View.h"
#include "Camera.h"
#include "Light.h"
#include "Shadow.h"

using namespace std;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float deltaTime = 0.0f;
float lastTime = 0.0f;

View view;
Light light;
Shadow shadow;

int show_window = -1;

int main()
{
	// GLFW初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GLFW窗口创建
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "创建窗口失败" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// GLAD指针管理
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "初始化GLAD失败" << std::endl;
		return -1;
	}

	// imgui初始化
	const char* glsl_version = "#version 330";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 tran_color = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);

	Shader shaderProgram = Shader("vertex.glsl", "fragment.glsl");
	Shader pShader = Shader("pvertex.glsl", "pfragment.glsl");
	Shader gShader = Shader("gvertex.glsl", "gfragment.glsl");
	Shader lightShader = Shader("lvertex.glsl", "lfragment.glsl");
	Shader depthShader = Shader("depthvertex.glsl", "depthfragment.glsl");
	Shader shadowShader = Shader("shadowvertex.glsl", "shadowfragment.glsl");

	Bresenham bresenham(shaderProgram);
	Transformation transformation(shaderProgram, HEIGHT, WIDTH);
	view = View(shaderProgram, HEIGHT, WIDTH);
	light = Light(pShader, gShader, lightShader, HEIGHT, WIDTH);
	shadow = Shadow(shadowShader, lightShader, depthShader, HEIGHT, WIDTH);

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		glfwPollEvents();
		// 处理输入
		processInput(window);
		// 清屏
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Imgui框架
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("CG Homework");
		ImGui::Text("Choose which to show: ");

		ImGui::RadioButton("Bresenham", &show_window, 0);
		ImGui::RadioButton("Transformation", &show_window, 1);
		ImGui::RadioButton("Projection and Camera", &show_window, 2);
		ImGui::RadioButton("Light", &show_window, 3);
		ImGui::RadioButton("Shadow", &show_window, 4);

		ImGui::End();

		if (show_window == 0) {
			bresenham.render();
		}
		else if (show_window == 1) {
			transformation.render();
		}
		else if (show_window == 2) {
			view.render();
		}
		else if (show_window == 3) {
			light.render();
		}
		else if (show_window == 4) {
			shadow.render();
		}

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

// 处理输入
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		switch (show_window)
		{
		case 2:
			view.processMove(0, deltaTime);
			break;
		case 3:
			light.processMove(0, deltaTime);
			break;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		switch (show_window)
		{
		case 2:
			view.processMove(1, deltaTime);
			break;
		case 3:
			light.processMove(1, deltaTime);
			break;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		switch (show_window)
		{
		case 2:
			view.processMove(2, deltaTime);
			break;
		case 3:
			light.processMove(2, deltaTime);
			break;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		switch (show_window)
		{
		case 2:
			view.processMove(3, deltaTime);
			break;
		case 3:
			light.processMove(3, deltaTime);
			break;
		}
	}
}

// 响应屏幕大小变化
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// 响应鼠标移动
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	switch (show_window)
	{
	case 2:
		view.processRotate(xpos, ypos);
		break;
	case 3:
		light.processRotate(xpos, ypos);
		break;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	switch (show_window)
	{
	case 2:
		view.processScroll(xoffset, yoffset);
		break;
	case 3:
		light.processScroll(xoffset, yoffset);
		break;
	}
}