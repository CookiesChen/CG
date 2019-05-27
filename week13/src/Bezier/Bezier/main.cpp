#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <math.h>

#include "Shader.h"

using namespace std;

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void onClick(GLFWwindow* window, int, int, int);
void processInput(GLFWwindow* window);
void cursor_position_callback(GLFWwindow* window, double x, double y);
long int factorial(int x);
void transform(vector<glm::vec2> vertex);

float deltaTime = 0.0f;
float lastTime = 0.0f;

int show_window = -1;
float xpos, ypos;

vector<glm::vec2> line;
float vertices[60];
float curve[10000];
float transformVertex[1000];
int curveCount = 0;
float animation = 0;
int checkAnimation = 0;

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
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, onClick);
	// GLAD指针管理
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "初始化GLAD失败" << std::endl;
		return -1;
	}

	Shader shader = Shader("line.vs", "line.fs");
	line = vector<glm::vec2>();

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

		for (int i = 0; i < line.size(); i++) {
			vertices[i * 2] = line[i].x;
			vertices[i * 2 + 1] = line[i].y;
		}

		// 原始直线
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * line.size(), vertices, GL_STATIC_DRAW);

		unsigned int VAO;
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		// 位置
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		if(line.size() >= 2) glDrawArrays(GL_LINE_STRIP, 0, line.size());

		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, line.size());
		
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		// 曲线
		curveCount = 0;
		if (line.size() >= 2) {
			for (float t = 0; t < 1; t += 0.001) {
				float cx = 0, cy = 0;
				for (int i = 0, n = line.size()-1; i <= n; i++) {
					float bernstein = factorial(n) / (factorial(i) * factorial(n - i)) * pow(t, i) * pow(1 - t, n - i);
					cx += line[i].x * bernstein;
					cy += line[i].y * bernstein;
				}
				curve[curveCount * 2] = cx;
				curve[curveCount * 2 + 1] = cy;
				curveCount++;
			}
		}
		unsigned int cVBO;
		glGenBuffers(1, &cVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cVBO);
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * curveCount, curve, GL_STATIC_DRAW);

		unsigned int cVAO;
		glBindBuffer(GL_ARRAY_BUFFER, cVBO);

		glGenVertexArrays(1, &cVAO);
		glBindVertexArray(cVAO);
		// 位置
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glPointSize(1.0f);
		glDrawArrays(GL_POINTS, 0, curveCount);

		glDeleteVertexArrays(1, &cVAO);
		glDeleteBuffers(1, &cVBO);

		// 动画
		if (line.size() >= 3) {
			checkAnimation++;
			if (checkAnimation >= 180) {
				transform(line);
				animation += 0.001;
				animation = animation > 1 ? 1 : animation;
			}
		}
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

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
		
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		
	}
}

// 响应屏幕大小变化
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void onClick(GLFWwindow* window, int button, int action, int mods) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		checkAnimation = 0;
		// 监听左键点击事件
		int flag = 0;
		for (int i = 0; i < line.size(); i++) {
			if (line[i].x == xpos && line[i].y == ypos) {
				flag = 1;
			}
		}
		if (flag == 0 && line.size() < 20) {
			line.push_back(glm::vec2(xpos, ypos));
		}
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		checkAnimation = 0;
		// 监听右键点击事件
		vector<glm::vec2>::iterator iter;
		for (iter = line.begin(); iter != line.end() ; iter++) {
			if (pow(iter->x - xpos, 2) + pow(iter->y - ypos, 2) <= 0.1*0.1) {
				line.erase(iter);
				break;
			}
		}
	}
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	xpos = (x-WIDTH/2)/(WIDTH/2);
	ypos = ((HEIGHT - y)-HEIGHT/2)/(HEIGHT/2);
}

// 计算阶乘
long int factorial(int x) {
	if (x == 0) return 1;
	int result = 1;
	for (int i = 1; i <= x; i++) {
		result *= i;
	}
	return result;
}

void transform(vector<glm::vec2> vertex) {
	int n = vertex.size();
	if (n == 1) return;

	vector<glm::vec2> nextVertexs = vector<glm::vec2>();
	for (int i = 0; i < n - 1; i++) {
		float tx = (1 - animation) * vertex[i].x + animation * vertex[i + 1].x;
		float ty = (1 - animation) * vertex[i].y + animation * vertex[i + 1].y;
		glm::vec2 nextVertex = glm::vec2(tx, ty);
		transformVertex[i * 2] = tx;
		transformVertex[i * 2 + 1] = ty;
		nextVertexs.push_back(nextVertex);
	}

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * nextVertexs.size(), transformVertex, GL_STATIC_DRAW);

	unsigned int VAO;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPointSize(10.0f);
	glDrawArrays(GL_POINTS, 0, nextVertexs.size());

	glPointSize(1.0f);
	glDrawArrays(GL_LINE_STRIP, 0, nextVertexs.size());

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	transform(nextVertexs);
}