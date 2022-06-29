#pragma once

#include "../gl.h"

#include <string>
#include <memory>

namespace muon::rendering {

class _Shader {
public:
	_Shader();
	~_Shader();

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
typedef std::shared_ptr<_Shader> Shader;

}
