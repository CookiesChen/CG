#include "Transformation.h"



Transformation::Transformation(Shader shader, const unsigned int height, const unsigned int width)
{
	this->shader = shader;
	this->height = height;
	this->width = width;
}


Transformation::~Transformation()
{
}

void Transformation::render()
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

	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
	view = glm::rotate(view, glm::radians(30.0f), glm::vec3(1.0f, -1.0f, 0.0f));
	projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);

	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	if (!bonus) {
		if (rotation) {
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));
		}
		if (scaling) {
			model = glm::scale(model, (abs(sin((float)glfwGetTime())) + 0.1f) * glm::vec3(scale_x, scale_y, scale_z));
		}
		model = glm::translate(model, glm::vec3(translatio_x, translatio_y, 0.0f));
		model = glm::scale(model, glm::vec3(scale_x, scale_y, scale_z));

		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	} else {
		float radius = 2.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;

		model = glm::translate(model, glm::vec3(camX, 0.0f, camZ));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, 0.5f * glm::vec3(1.0f, 1.0f, 1.0f));

		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime()*0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		
		shader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	if (depth) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	// Çå³ý
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}


void Transformation::widget() {

	ImGui::Begin("CG Homework 3");
	ImGui::Text("Choose transformation: ");

	ImGui::PushItemWidth(50);
	ImGui::SliderFloat("TranslationX", &translatio_x, -1.0f, 1.0f);
	ImGui::SliderFloat("TranslationY", &translatio_y, -1.0f, 1.0f);
	ImGui::Checkbox("depth", &depth);
	ImGui::Checkbox("Rotation", &rotation);
	ImGui::Checkbox("Scaling", &scaling);
	ImGui::SliderFloat("ScaleX", &scale_x, 0.5f, 2.0f);
	ImGui::SliderFloat("ScaleY", &scale_y, 0.5f, 2.0f);
	ImGui::SliderFloat("ScaleZ", &scale_z, 0.5f, 2.0f);
	ImGui::Checkbox("Sun and Earth", &bonus);

	ImGui::NextColumn();
	ImGui::End();
}