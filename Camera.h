#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
	Camera();

	glm::vec3 pos{ 0.f, 0.f, 3.f };

	glm::vec3 target{};
	glm::vec3 dir{};

	glm::vec3 up{ 0.f,1.f,0.f };
	glm::vec3 camera_up{0.f,1.f,0.f};
	glm::vec3 front{ 0.f,0.f,-1.f };
	glm::vec3 right;

	glm::mat4 view;

	float yaw = -90.f;
	float pitch = 0.f;

	void update();

	// Used to prevent jump to initial mouse cursor pos
	bool first_mouse{ true };

	// Last mouse positions
	float last_x{ 400 };
	float last_y{ 300 };
};

