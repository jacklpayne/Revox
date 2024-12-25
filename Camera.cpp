#include "Camera.h"

Camera::Camera() {
	target = glm::vec3(0.f, 0.f, 0.f);
	dir = glm::normalize(pos - target);	

	right = glm::normalize(glm::cross(up, dir));
	
	dir.x = cos(glm::radians(yaw) * glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw) * glm::radians(pitch));
}

void Camera::update() {
	view = glm::lookAt(pos, pos + front, camera_up);
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	dir.y = sin(glm::radians(pitch));
	dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(dir);
}
