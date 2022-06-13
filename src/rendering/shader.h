#pragma once

#include "../gl.h"

#include <string>

namespace muon::rendering {

class Shader {
public:
	Shader();
	~Shader();

	void load(std::string path);
	void load(std::string path, GLenum type);

	GLuint program;
	void link();

	void use();
private:
	bool mCheckShader(GLuint uid);

	GLuint mVert;
	GLuint mFrag;
};

}
