#include "Renderer.h"

Renderer::Renderer() : shader("verts.vs", "frags.fs") {

	// Tell GLFW what OpenGL specs we'll be using
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);

	window = glfwCreateWindow(1920, 1080, "Revox", NULL, NULL);
	glfwMakeContextCurrent(window);
	// Associates this renderer with the window so that anything
	// that knows about the window can access it
	// this is useful for callbacks that only have the window
	glfwSetWindowUserPointer(window, this);

	// Initialize GLAD which manages OpenGL function ptrs on backend
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Tell OpenGL the rendering window (viewport) size
	glViewport(0, 0, 1920, 1080);

	// Register the callback for changing the OpenGL viewport size when the GLFW window is resized
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	// Init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	int fb_width, fb_height;
	glfwGetFramebufferSize(window, &fb_width, &fb_height);
	ImGui::GetIO().DisplaySize = ImVec2((float)fb_width, (float)fb_height);
	ImGui::GetIO().DisplayFramebufferScale = ImVec2(
		(float)fb_width / 1920.f,
		(float)fb_height / 1080.f
	);
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	

	// (VBO) Create and fill a buffer on the GPU for the vertex data
	glGenBuffers(1, &cube_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
	glBufferData(GL_ARRAY_BUFFER, cube_verts.size() * sizeof(Vertex), cube_verts.data(), GL_STATIC_DRAW);

	shader.build();

	// VAO setup
	glGenVertexArrays(1, &cube_VAO);
	glBindVertexArray(cube_VAO);

	// Specify vert buffer layout (attributes) & enable it
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(2);

	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 200.0f);


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	enqueue_world();
}

void Renderer::update() {
	if (glfwWindowShouldClose(window)) {
		glfwTerminate();
		return;
	}

	draw_debug_window();

	process_input();
	cam.update();
	glfwSwapBuffers(window);
	glfwPollEvents();

	// Update time
	float curr_frame = glfwGetTime();
	delta_time = curr_frame - last_frame;
	last_frame = curr_frame;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set uniforms
	shader.use();
	shader.set_mat4("view", cam.view);
	shader.set_mat4("projection", projection);

	glBindVertexArray(cube_VAO);
	draw_sun();
	draw_world();
}

void Renderer::process_input() {
	const float speed = 6.5 * delta_time;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.pos += speed * cam.front;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.pos -= speed * cam.front;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.pos -= glm::normalize(glm::cross(cam.front, cam.camera_up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.pos += glm::normalize(glm::cross(cam.front, cam.camera_up)) * speed;

	static bool escape_key_pressed = false;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		if (!escape_key_pressed) {
			if (mouse_freed) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			mouse_freed = !mouse_freed;
			escape_key_pressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		escape_key_pressed = false;
	}
}

void Renderer::mouse_callback(GLFWwindow* window, double x_pos, double y_pos) {
	// Get the renderer through the window to access its camera
	Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	if (renderer->mouse_freed) {
		return;
	}

	float& last_x = renderer->cam.last_x;
	float& last_y = renderer->cam.last_y;
	if (renderer->cam.first_mouse) {
		last_x = x_pos;
		last_y = y_pos;
		renderer->cam.first_mouse = false;
	}

	float x_offset = x_pos - last_x;
	float y_offset = last_y - y_pos;
	last_x = x_pos;
	last_y = y_pos;

	const float sensitivity = 0.3f;
	x_offset *= sensitivity;
	y_offset *= sensitivity;

	renderer->cam.yaw += x_offset;
	renderer->cam.pitch += y_offset;
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Renderer::enqueue_world() {
	chunk_queue.clear();
	const auto parsed_world = world.get_parsed_world();
	// For each chunk, generate vertex data and push it to the chunk queue
	for (const auto& parsed_chunk : parsed_world) {
		std::vector<Vertex> chunk_vertices;
		for (const auto& vox : parsed_chunk) {
			auto origin = glm::vec3{ vox.origin.x, vox.origin.y, vox.origin.z };
			auto color = glm::vec3{ vox.color.x, vox.color.y, vox.color.z };
			auto size = glm::vec3{ vox.size_mult };

			std::vector<Vertex> vox_verts;
			for (auto& vert : cube_verts) {
				vox_verts.push_back({ vert.position * size + origin, vert.normal, color });
			}
			chunk_vertices.insert(chunk_vertices.end(), vox_verts.begin(), vox_verts.end());
		}
		chunk_queue.push_back(chunk_vertices);
	}
}

void Renderer::draw_world() {
for (const auto& chunk : chunk_queue) {
		light_color = glm::vec3{ 1.f,1.f,1.f };
		model = glm::mat4(1.f);
		shader.set_mat4("model", model);
		//shader.set_vec3("color", color);
		glBufferData(GL_ARRAY_BUFFER, chunk.size() * sizeof(Vertex), chunk.data(), GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, chunk.size());
	}
}

void Renderer::draw_sun() {
	// Ensure that sun is white
	color = glm::vec3{ 1.f,1.f,1.f };
	light_color = glm::vec3{ 1.f,1.f,1.f };

	model = glm::mat4(1.0f);
	model = glm::translate(model, sun_pos);
	model = glm::scale(model, glm::vec3(0.5f));
	shader.set_mat4("model", model);

	shader.set_vec3("light_color", light_color);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::draw_debug_window() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();

	static int render_distance = 5;

	// Set the position and size of the debug window
	ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetIO().DisplaySize.y - 200), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_Always);

	// Begin the window
	ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	// Slider
	ImGui::SliderInt("Distance", &render_distance, 1, 5);

	// Button
	if (ImGui::Button("Regenerate")) {
		world.set_render_distance(render_distance);
		world.generate_world(5);
		enqueue_world();
	}

	// End the window
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
