#include "View.h"


View::View()
{
}

View::View(Shader shader, const unsigned int height, const unsigned int width)
{
	this->shader = shader;
	this->height = height;
	this->width = width;
	this->lastX = (double)width / 2.0;
	this->lastY = (double)height / 2.0;
	this->camera = Camera();
}


View::~View()
{
}

void View::render()
{

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Î»ÖÃ
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// ÑÕÉ«
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	widget();

	glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	if (view_type == 0) {
		model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
		if (projection_type == 0) {
			projection = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
			view = glm::translate(view, glm::vec3(0.0f, -1.0f, -3.0f));
			view = glm::rotate(view, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			view = glm::rotate(view, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else {
			projection = glm::ortho(left, right, bottom, top, nea, fa);
		}
	}
	else if (view_type == 1) {
		float radius = 3.0f;
		float cam1 = sin(glfwGetTime()) * radius;
		float cam2 = cos(glfwGetTime()) * radius;

		view = glm::lookAt(glm::vec3(cam1, 0.0, cam2), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
	}
	else if (view_type == 2) {
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(scroll_fov), (float)width / (float)height, 0.1f, 100.0f);
	}

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glEnable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	// Çå³ý
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void View::widget() {

	ImGui::Begin("CG Homework 4");
	ImGui::Text("Choose View Change: ");
	ImGui::NextColumn();

	ImGui::RadioButton("Projection", &view_type, 0);
	ImGui::RadioButton("View Changing", &view_type, 1);
	ImGui::RadioButton("FPS", &view_type, 2);

	if (view_type == 0) {
		ImGui::RadioButton("perspective", &projection_type, 0);
		ImGui::RadioButton("orthographic", &projection_type, 1);
		if (projection_type == 0) {
			ImGui::SliderFloat("fov", &fov, 30.0f, 90.0f);
			ImGui::SliderFloat("aspect", &aspect, 0.0f, 2.0f);
			ImGui::SliderFloat("zNear", &zNear, 0.0f, 50.0f);
			ImGui::SliderFloat("zFar", &zFar, 50.0f, 100.0f);
		}
		else if (projection_type == 1) {
			ImGui::SliderFloat("left", &left, -3.0f, -1.5f);
			ImGui::SliderFloat("right", &right, -1.5f, 0.0f);
			ImGui::SliderFloat("bottom", &bottom, -1.0f, 0.5f);
			ImGui::SliderFloat("top", &top, 0.5f, 2.0f);
			ImGui::SliderFloat("nea", &nea, 0.5f, 2.5f);
			ImGui::SliderFloat("fa", &fa, 0.5f, 2.5f);
		}
	}
	ImGui::End();
}

void View::processMove(int direction, float distance)
{
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


void View::processRotate(double xPos, double yPos)
{
	if (view_type == 2) {
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
}

void View::processScroll(double xOffset, double yOffset)
{
	if (view_type == 2) {
		if (scroll_fov >= 60.0f && scroll_fov <= 80.0f)
			scroll_fov -= yOffset;
		if (scroll_fov <= 60.0f)
			scroll_fov = 60.0f;
		if (scroll_fov >= 80.0f)
			scroll_fov = 80.0f;
	}
}
