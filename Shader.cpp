#include "Shader.h"
#include <glm/fwd.hpp>

Shader::Shader(const char* _vert_path, const char* _frag_path)
	: vert_path(_vert_path), frag_path(_frag_path) {}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::set_float(const std::string& name, float val) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)val);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& val) const {
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::set_vec4(const std::string& name, const glm::vec4& val) const {
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::set_vec3(const std::string& name, const glm::vec3& val) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(val));
}

void Shader::set_bool(const std::string& name, bool val) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::set_int(const std::string& name, int val) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), val);
}

void Shader::build() {
	// 1. Retrieve src
	std::string vert_src;
	std::string frag_src;
	std::ifstream vert_shader_file;
	std::ifstream frag_shader_file;

	// Ensure the objs can throw excepts
	vert_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	frag_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vert_shader_file.open(vert_path);
		std::stringstream vert_shader_stream;
		vert_shader_stream << vert_shader_file.rdbuf();
		vert_shader_file.close();
		vert_src = vert_shader_stream.str();


		frag_shader_file.open(frag_path);
		std::stringstream frag_shader_stream;
		frag_shader_stream << frag_shader_file.rdbuf();
		frag_shader_file.close();
		frag_src = frag_shader_stream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char* vert_shader_code = vert_src.c_str();
	const char* frag_shader_code = frag_src.c_str();

	// 2. Compile shaders
	GLuint vert, frag;
	int success;
	char info_log[512];

	vert = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert, 1, &vert_shader_code, NULL);
	glCompileShader(vert);

	// Print compile errors
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vert, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	};

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &frag_shader_code, NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(frag, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;
	};

	// Make program
	ID = glCreateProgram();
	glAttachShader(ID, vert);
	glAttachShader(ID, frag);
	glLinkProgram(ID);
	// print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}
