#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.h"
#include "Bresenham.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	const unsigned int WIDTH = 200;
	const unsigned int HEIGHT = 200;
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

	int show_type = 0;
	bool show_padding = false;

	// 三角形顶点
	int x1 = -50, y1 = -50;
	int x2 = 0, y2 = 50;
	int x3 = 50, y3 = -50;
	int precision = 100;

	// 半径、圆心
	int radius = 50;
	int center_X = 0, center_Y = 0;

	Bresenham bresenham;

	// 渲染循环
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		// 处理输入
		processInput(window);
		// 清屏
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 布兰森汉姆算法
		if (show_type == 0) {
			bresenham.triangle(x1, y1, x2, y2, x3, y3);
			if (show_padding) {
				bresenham.padding(x1, y1, x2, y2, x3, y3);
			}
		} else if (show_type == 1) {
			bresenham.circle(center_X, center_Y, radius);
		}

		// Imgui框架
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		// 自定义布局
		ImGui::Begin("CG Homework 2");
		ImGui::Text("Choose circle or triangle: ");

		ImGui::RadioButton("triangle", &show_type, 0);
		ImGui::RadioButton("circle", &show_type, 1);

		// 三角形布局imgui布局
		if (show_type == 0) {

			ImGui::Checkbox("padding", &show_padding);

			ImGui::PushItemWidth(50);
			
			ImGui::SliderInt("x1", &x1, -precision, precision);
			ImGui::SameLine();
			ImGui::SliderInt("y1", &y1, -precision, precision);

			ImGui::SliderInt("x2", &x2, -precision, precision);
			ImGui::SameLine();
			ImGui::SliderInt("y2", &y2, -precision, precision);

			ImGui::SliderInt("x3", &x3, -precision, precision);
			ImGui::SameLine();
			ImGui::SliderInt("y3", &y3, -precision, precision);
		}

		// 圆布局imgui布局
		if (show_type == 1) {
			ImGui::SliderInt("radius", &radius, 0, 100);
		}
		ImGui::NextColumn();
		ImGui::End();

		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		shaderProgram.use();

		// Imgui渲染
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
}

// 响应屏幕大小变化
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}