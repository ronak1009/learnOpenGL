#include <glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource {
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

    /* Create a windowed mode window and it's OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    //glewInit() uses the valid OpenGL context. without creating the valid context, the glew returns error
    //later you shall not be able to use the api(s) related to openGL

    if (glewInit() != GLEW_OK)
        std::cout << "Error" << std::endl;

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
    glGenBuffers(1, &buffer);
    //binding the buffer to openGL
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //adding the data to buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

    //binding the indicies to the buffer
    unsigned int indexBuffer;
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
    //setting up Vertex attribute
    int stride = sizeof(float) * 2; //amount of bytes for vertex
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, 0);
    //enabling the vertex
    glEnableVertexAttribArray(0);
    
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexShader, source.FragmentShader);
    glUseProgram(shader);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        //Render here
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
         
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}