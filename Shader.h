#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader {
public:
	GLuint ID;
	Shader(const char* _vert_path, const char* _frag_path);
	void use();

	void set_float(const std::string& name, float val) const;
	void set_mat4(const std::string& name, const glm::mat4& val) const;
	void set_vec4(const std::string& name, const glm::vec4& val) const;
	void set_vec3(const std::string& name, const glm::vec3& val) const;
	void set_bool(const std::string& name, bool val) const;
	void set_int(const std::string& name, int val) const;

	void build();
private:
	const char* vert_path;
	const char* frag_path;
};