#include "shader.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace muon::rendering;

std::string textFromFile(const std::string& path) {
    std::ifstream input(path);
    return std::string((std::istreambuf_iterator<char>(input)), 
                        std::istreambuf_iterator<char>());

}

Shader::Shader() {
	program = 0;
	mFrag = 0;
	mVert = 0;
}


void Shader::load(std::string path)
{
	std::string vertexLocation = path + ".vert";
	load(vertexLocation, GL_VERTEX_SHADER);

	std::string fragmentLocation = path + ".frag";
	load(fragmentLocation, GL_FRAGMENT_SHADER);
}


void Shader::load(std::string path, GLenum type) {
	GLuint activeShader = 0;

	if (type == GL_VERTEX_SHADER)
		mVert = activeShader = glCreateShader(type);

	if (type == GL_FRAGMENT_SHADER)
		mFrag = activeShader = glCreateShader(type);

	std::string loadedShaderSource = textFromFile(path);
	const char* shaderSource = loadedShaderSource.c_str();
	int shaderSourceLength = loadedShaderSource.length();

	glShaderSource(activeShader, 1, &shaderSource, &shaderSourceLength);
}

void Shader::link() {
	if (mVert == 0 || mFrag == 0) {
		return;
	}

	glCompileShader(mVert);
	if (!mCheckShader(mVert)) {
        std::cout << "Vertex Shader broke" << std::endl;
	}

	glCompileShader(mFrag);
	if (!mCheckShader(mFrag)) {
       std::cout << "Fragment Shader broke" << std::endl;
	}

	program = glCreateProgram();

	glAttachShader(program, mVert);
	glAttachShader(program, mFrag);

	glLinkProgram(program);

	glDeleteShader(mVert);
	glDeleteShader(mFrag);
}

void Shader::use() {
	glUseProgram(program);
}


bool Shader::mCheckShader(GLuint uid) {
	GLint isCompiled = 0;
	glGetShaderiv(uid, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(uid, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(uid, maxLength, &maxLength, &errorLog[0]);

		for (int i = 0; i < errorLog.size(); i++) {
			std::cout << errorLog[i];
		}

		glDeleteShader(uid);
		return false;
	}

	return true;
}


Shader::~Shader() {
	glDeleteProgram(program);
	glDeleteShader(mVert);
	glDeleteShader(mFrag);

}