#ifndef SHADOW_H
#define SHADOW_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Shader.h"
#include <math.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"


class Shadow
{
public:
	Shadow(Shader, Shader, Shader, const unsigned int, const unsigned int);
	Shadow();
	~Shadow();

	void render();
	void widget();

	void renderScene(Shader shader);

	void processMove(int direction, float distance);
	void processRotate(double xPos, double yPos);
	void processScroll(double xOffset, double yOffset);

private:

	Shader shader;
	Shader lightShader;
	Shader depthShader;
	Camera camera;

	GLuint depthMapFBO;
	GLuint depthMap;

	int height;
	int width;

	float upOrDown = 0.0;
	float leftOrRight = 0.0;

	bool enable_camera = false;
	bool light_rotate = false;

	bool firstMouse = true;
	double lastX;
	double lastY;
	float scroll_fov = 70.0f;

	float ambientStrength = 0.1;
	float specularStrength = 0.5;
	int Shininess = 32;
	int projection_type = 0;

	glm::vec3 lightPos;
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 objectColor = glm::vec3(0.0f, 1.0f, 1.0f);

	GLuint planeVAO;
	GLuint cubeVAO;
	GLuint lightVAO;

	float plane[48] = {
		25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f,

		25.0f, -0.5f, 25.0f, 0.0f, 1.0f, 0.0f,
		25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,
		-25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,
	};

	float vertices[216] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f

	};
};

#endif