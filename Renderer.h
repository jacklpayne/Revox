#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "World.h"

class Renderer {
public:
	Renderer();
	void update();
private:
	float delta_time{};
	float last_frame{};

	GLFWwindow* window;
	Camera cam{};
	void process_input();
	static void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	GLuint cube_VBO;
	GLuint cube_EBO;
	Shader shader;
	GLuint cube_VAO;
	GLuint light_VAO;

	glm::mat4 model{ glm::mat4(1.f) };
	glm::mat4 view{ glm::mat4(1.f) };
	glm::mat4 projection;
	glm::vec3 color{0.8f, 0.2f, 0.6f};

	glm::vec3 light_color{ 1.f, 1.f, 1.f };
	glm::vec3 sun_pos{ 12.f, 8.f, 4.f };

	std::vector<std::pair<glm::mat4, glm::vec3>> voxel_queue;

	World world{ 3 };
	void enqueue_world();
	void draw_world();
	void draw_sun();

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
	};

    std::vector<Vertex> cube_verts = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f, 0.0f}},

        // Left face
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},

        // Right face
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},

        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},

        // Top face
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f, 0.0f}}
    };

};

