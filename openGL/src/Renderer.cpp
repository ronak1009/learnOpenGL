#include "Renderer.h"
#include <iostream>

void GLClearError()
{
    //GL_NO_ERROR => guarantee to be 0
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, const int line)
{
    while(GLenum err = glGetError())
    {
        std::cout << "Error: "<< err << 
            " " << function << " : " << file << " : " <<
            line  << std::endl;
        return false;
    }
    return true;
}