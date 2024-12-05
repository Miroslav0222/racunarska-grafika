#ifndef SHADER_H
#define SHADER_H

#include<GL/glew.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void activate();

	std::string loadShaderSrc(const char* filepath);

	GLint compileShader(const char* filepath, GLenum type);

	void setMat4(const std::string& name, glm::mat4 val);

	void setVec3(const std::string& name, float f1, float f2, float f3);

	void setInt(const std::string& name, int value);

	void setFloat(const std::string& name, float value);
};

#endif