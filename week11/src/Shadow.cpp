#include "Shadow.h"

Shadow::Shadow()
{
}

Shadow::Shadow(Shader shader, Shader lightShader, Shader depthShader, const unsigned int height, const unsigned int width)
{
	this->shader = shader;
	this->lightShader = lightShader;
	this->depthShader = depthShader;

	this->height = height;
	this->width = width;
	this->camera = Camera();

	this->shader.setLi("shadowMap", 0);
	// 深度贴图
	// 缓冲对象
	glGenFramebuffers(1, &depthMapFBO);

	// 创建2D纹理
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 立方体初始化VAO
	unsigned int cubeVBO;
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// 平面初始化VAO
	unsigned int planeVBO;
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane), plane, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

}

Shadow::~Shadow()
{
}

void Shadow::render()
{
	widget();

	if (light_rotate) {
		float raduis = 3.0f;
		float x = sin((float)glfwGetTime()) * raduis;
		float z = cos((float)glfwGetTime()) * raduis;

		lightPos = glm::vec3(x, 1.0f, z);
	}
	else {
		lightPos = glm::vec3(-5.0f, 3.0f, -5.0f);
	}

	glEnable(GL_DEPTH_TEST);

	if (!enable_camera) {
		float raduis = 2.0f;
		camera.setPitch(-upOrDown * 45.0f);
		camera.setYaw(-leftOrRight * 45.0f);
		camera.setPosition(glm::vec3(leftOrRight, upOrDown, 3.0f - abs(upOrDown) - abs(leftOrRight)));
	}

	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	view = camera.GetViewMatrix();
	projection = glm::perspective(glm::radians(scroll_fov), (float)width / (float)height, 0.1f, 100.0f);

	//-- 从光源视角渲染保存深度值
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	GLfloat near_plane = 1.0f, far_plane = 10.f;
	if (projection_type == 0) {
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	}
	else {
		lightProjection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.5f, 200.0f);
	}
	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	depthShader.use();
	depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	renderScene(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//-- 正常渲染，携带深度信息
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	shader.setFloat("ambientStrength", ambientStrength);
	shader.setFloat("specularStrength", specularStrength);
	shader.setInt("Shininess", Shininess);

	shader.setVec3("lightPos", lightPos);
	shader.setVec3("lightColor", lightColor);
	shader.setVec3("objectColor", objectColor);
	shader.setVec3("viewPos", camera.getPostion());
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderScene(shader);

	//-- 光源绘制
	lightShader.use();
	lightShader.setMat4("model", model);
	lightShader.setMat4("view", view);
	lightShader.setMat4("projection", projection);

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Shadow::widget()
{
	ImGui::Begin("Lighting");

	ImGui::SliderFloat("view up or down", &upOrDown, -1.0f, 1.0f);
	ImGui::SliderFloat("view left or right", &leftOrRight, -1.0f, 1.0f);

	ImGui::Checkbox("Light Source rotate", &light_rotate);
	ImGui::SliderFloat("ambientStrength", &ambientStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("specularStrength", &specularStrength, 0.0f, 1.0f);
	ImGui::SliderInt("Shininess", &Shininess, 4, 256);

	ImGui::RadioButton("ortho", &projection_type, 0);
	ImGui::RadioButton("perspective", &projection_type, 1);

	ImGui::NextColumn();
	ImGui::End();
}

void Shadow::renderScene(Shader shader) {
	// 平面
	glm::mat4 model = glm::mat4(1.0f);

	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	// 立方体
	//-- 物体绘制
	shader.setMat4("model", model);
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, -1.5f));
	shader.setMat4("model", model);
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Shadow::processMove(int direction, float distance)
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


void Shadow::processRotate(double xPos, double yPos)
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

void Shadow::processScroll(double xOffset, double yOffset)
{
	if (!enable_camera) return;
	if (scroll_fov >= 60.0f && scroll_fov <= 80.0f)
		scroll_fov -= yOffset;
	if (scroll_fov <= 60.0f)
		scroll_fov = 60.0f;
	if (scroll_fov >= 80.0f)
		scroll_fov = 80.0f;
}