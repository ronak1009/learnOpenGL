#include <glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

struct ShaderProgramSource
{
  std::string VertexShader;
  std::string FragmentShader;
};

static ShaderProgramSource ParseShader(const std::string& filepath){

  enum class ShaderType
  {
    NONE = -1,
    VERTEX = 0,
    FRAGMENT = 1
  };

  ShaderType type = ShaderType::NONE;

  std::ifstream stream (filepath);
  std::string line;
  std::stringstream ss[2];

  while(getline(stream, line))
  {
    if(line.find("#shader") != std::string::npos)
    {
      if(line.find("vertex") != std::string::npos)
        type = ShaderType::VERTEX; //set mode to vertex
      else if(line.find("fragment") != std::string::npos)
        type = ShaderType::FRAGMENT; //set mode to fragment
    }
    else
      ss[(int)type] << line << '\n';
  }
  //returning the struct
  ShaderProgramSource obj = { ss[0].str(), ss[1].str() };
  return obj;
}

//strings are actual source code of the shaders

static unsigned int CompileShader(unsigned int type, const std::string& source )
{
  unsigned int id = glCreateShader(type);
  //returns the pointer to the string's starting
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  //error handling
  if(result==GL_FALSE)
  {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char* message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
    std::cout << message << std::endl;
    glDeleteShader(id);
    return 0;
  }

  return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);
  glValidateProgram(program);

  //glDeleteShader(vs);
  // glDeleteShader(fs);
  //glDeleteProgram(program);
  return program;
}


int main(void)
{
  GLFWwindow* window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



  /* Create a windowed mode window and it's OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }
  /* Make the window's context current */
  glfwMakeContextCurrent(window);
  //setting the frame-rate of your openGL context
  glfwSwapInterval(1);
  //glewInit() uses the valid OpenGL context. without creating the valid context, the glew returns error
  //later you shall not be able to use the api(s) related to openGL

  if (glewInit() != GLEW_OK)
    std::cout << "Error" << std::endl;
  {
    //building the buffer
    unsigned int buffer;
    float positions[] = {
      -0.5f, -0.5f, //0
      0.5f, -0.5f, //1
      0.5f, 0.5f, //2
      -0.5f, 0.5f, //3
    };
    //FOR CREATING THE INDEX BUFFER
    unsigned int indicies[] = {
      0, 1, 2,
      2, 3, 0
    };

    //creating vertex arrary object
   /* unsigned int vao;
    GLCALL(glGenVertexArrays(1, &vao));
    GLCALL(glBindVertexArray(vao));
*/
    VertexArray va;
    VertexBuffer vb (positions, sizeof(positions));
    //right now no need to call vb.Bind() as in the constructor we are binding it by default

    IndexBuffer ib (indicies, 6);

    //setting up Vertex attribute
    int stride = sizeof(float) * 2; //amount of bytes for vertex

    
    VertexBufferLayout layout;

    layout.Push<float>(2);
    va.AddBuffer(vb, layout);
    va.Bind();

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexShader, source.FragmentShader);
    GLCALL(glUseProgram(shader));


    GLCALL(
      int location = glGetUniformLocation(shader, "u_Color");
    ASSERT(location != -1); //does not have incorrect values.
    );

    //unbinding everything
    GLCALL(glBindVertexArray(0));
    GLCALL(glUseProgram(0));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


    //animating the colors
    float r = 0.0f;
    float increment = 0.05f;

    while (!glfwWindowShouldClose(window))
    {
      //Render here
      GLCALL(glClear(GL_COLOR_BUFFER_BIT));

      //before the draw call, you need to bind the data to buffer
      GLCALL(glUseProgram(shader));
      GLCALL(glUniform4f(location, r, 0.7f, 0.8f, 1.0f)); //can be set only after the shader is bound

      va.Bind();
      ib.Bind();
      GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

      if(r > 1.0f)
        increment = -0.5f;
      else if(r < 0.0f)
        increment = 0.05f;
      r += increment;

      GLCALL(glfwSwapBuffers(window));
      GLCALL(glfwPollEvents());
    }
  }
  GLCALL(glfwTerminate());
  return 0;
}