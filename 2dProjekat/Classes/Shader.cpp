#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
	int success;
	char infoLog[512];

	unsigned int vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	unsigned int fragShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragShader);
	glLinkProgram(id);

	//error handling
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR WITH VERTEX SHADER COMP.: " << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

}

void Shader::activate() {
	glUseProgram(id);
}

GLint Shader::compileShader(const char* filepath, GLenum type) {
	int success;
	char infoLog[512];

	unsigned int ret = glCreateShader(type);
	std::string shaderSource = loadShaderSrc(filepath);
	const GLchar* shader = shaderSource.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);

	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ret, 512, NULL, infoLog);
		std::cout << "ERROR WITH SHADER COMP.: " << std::endl << infoLog << std::endl;
	}

	return ret;
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, float f1, float f2, float f3) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), f1, f2, f3);
}

void Shader::setMat4(const std::string& name, glm::mat4 val) { // name - name of the variable in glsl shader
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

std::string Shader::loadShaderSrc(const char* filename) {
	std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filename);
	if (file.is_open()) {
		buf << file.rdbuf();
		ret = buf.str();
		std::cout << "Uspjesno procitao fajl sa putanje \"" << filename << "\"!" << std::endl;
	}
	else {
		std::cout << "Could not open file";
	}
	file.close();
	return ret;
}