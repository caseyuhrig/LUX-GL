#pragma once


//#include <gl/GL.h>
#include <gl/glew.h>

class GLUtils {
public:
    GLUtils();
    ~GLUtils();

    static void loadShader(const char* shaderSource, GLenum shaderType, GLuint &programId);
};

