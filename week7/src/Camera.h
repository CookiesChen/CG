#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Camera
{
public:

	Camera();
	~Camera();

	void moveForward(float const distance);
	void moveBack(float const distance);
	void moveRight(float const distance);
	void moveLeft(float const distance);
	void rotate(float const, float const);

	glm::mat4 GetViewMatrix();

private:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;
	// Å·À­½Ç
	float yaw = -90.0f;
	float pitch = 0.0f;

	float sensitivity = 0.1;

	void updateVector();
};

#endif // !CAMERA_H