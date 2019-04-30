#include "Light.h"

Light::Light()
{
}

Light::Light(Shader pShader, Shader gShader,Shader lightShader, const unsigned int height, const unsigned int width)
{
	this->pShader = pShader;
	this->gShader = gShader;
	this->lightShader = lightShader;

	this->height = height;
	this->width = width;
	this->camera = Camera();
}

Light::~Light()
{
}

void Light::render()
{
	widget();

	Shader shader;
	if (light_type == 0) {
		shader = pShader;
	}
	else {
		shader = gShader;
	}

	// 物体
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int cubeVAO;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	// 位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// 法向量
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 光源
	unsigned int lightVAO;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// 位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	if (light_rotate) {
		float raduis = 1.0f;
		float x = sin((float)glfwGetTime()) * raduis;
		float z = cos((float)glfwGetTime()) * raduis;

		lightPos = glm::vec3(x, 0.0f, z);
	}
	else {
		lightPos = glm::vec3(0.0f, 0.0f, 1.0f);
	}
	
	glEnable(GL_DEPTH_TEST);

	// 物体
	shader.use();
	glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	if (!enable_camera) {
		float raduis = 2.0f;
		camera.setPitch(-upOrDown * 45.0f);
		camera.setYaw(-leftOrRight * 45.0f);
		camera.setPosition(glm::vec3(leftOrRight, upOrDown, 3.0f-abs(upOrDown)-abs(leftOrRight)));
	}
	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(scroll_fov), (float)width / (float)height, 0.1f, 100.0f);

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	shader.setFloat("ambientStrength", ambientStrength);
	shader.setFloat("specularStrength", specularStrength);
	shader.setInt("Shininess", Shininess);

	shader.setVec3("lightPos", lightPos);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("objectColor", objectColor);
	shader.setVec3("viewPos", camera.getPostion());

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// 光源
	lightShader.use();
	model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	lightShader.setMat4("model", model);
	lightShader.setMat4("view", view);
	lightShader.setMat4("projection", projection);
	
	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	// 清除
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

}

void Light::widget()
{
	ImGui::Begin("Lighting");
	ImGui::Text("Choose transformation: ");

	ImGui::RadioButton("Phong Shading", &light_type, 0);
	ImGui::RadioButton("Gouraud Shading", &light_type, 1);

	ImGui::SliderFloat("view up or down", &upOrDown, -1.0f, 1.0f);
	ImGui::SliderFloat("view left or right", &leftOrRight, -1.0f, 1.0f);

	ImGui::Checkbox("Enable Camera", &enable_camera);
	ImGui::Checkbox("Bonus", &light_rotate);
	ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f);
	ImGui::SliderInt("Shininess", &Shininess, 0, 256);

	ImGui::NextColumn();
	ImGui::End();
}

void Light::processMove(int direction, float distance)
{
	if (!enable_camera) return;
	switch (direction)
	{
	case 0:
		camera.moveForward(distance);
		break;
	case 1:
		camera.moveBack(distance);
		break;
	case 2:
		camera.moveLeft(distance);
		break;
	case 3:
		camera.moveRight(distance);
		break;
	}
}


void Light::processRotate(double xPos, double yPos)
{
	if (!enable_camera) return;
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xoffset = xPos - lastX;
	float yoffset = lastY - yPos;

	lastX = xPos;
	lastY = yPos;

	camera.rotate(xoffset, yoffset);
}

void Light::processScroll(double xOffset, double yOffset)
{
	if (!enable_camera) return;
	if (scroll_fov >= 60.0f && scroll_fov <= 80.0f)
		scroll_fov -= yOffset;
	if (scroll_fov <= 60.0f)
		scroll_fov = 60.0f;
	if (scroll_fov >= 80.0f)
		scroll_fov = 80.0f;
}