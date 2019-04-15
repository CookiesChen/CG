#include "Camera.h"

using namespace std;

Camera::Camera()
{
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	updateVector();
}


Camera::~Camera()
{
}

void Camera::moveForward(float const distance)
{
	position += front * distance;
}

void Camera::moveBack(float const distance)
{
	position -= front * distance;
}

void Camera::moveRight(float const distance)
{
	position += glm::normalize(glm::cross(front, up)) * distance;
}

void Camera::moveLeft(float const distance) 
{
	position -= glm::normalize(glm::cross(front, up)) * distance;
}

void Camera::rotate(float const xOffset, float const yOffset)
{
	yaw += xOffset * sensitivity;
	pitch += yOffset * sensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	updateVector();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void Camera::updateVector()
{
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	new_front.y = sin(glm::radians(pitch));
	new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(new_front);
	
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}