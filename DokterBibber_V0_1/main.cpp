#define STB_IMAGE_IMPLEMENTATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tigl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"

using tigl::Vertex;

#include "stb_image.h" 

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

GLFWwindow* window;
GLuint textureId = -1;

void init();
void update();
void draw();
void cubes();

int main(void)
{
    if (!glfwInit())
        throw "Could not initialize glwf";
    window = glfwCreateWindow(1920, 1090, "Dokter Bibber!", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        throw "Could not initialize glwf";
    }
    glfwMakeContextCurrent(window);

    tigl::init();

    init();

	while (!glfwWindowShouldClose(window))
	{
		update();
		draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();


    return 0;
}


void init()
{
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
    });

    glEnable(GL_DEPTH_TEST);
}

float angle = 0.0f;

void update()
{

}

void draw()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 projection = glm::perspective(55.0f, width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 10.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, -1.0f));
    

    tigl::shader->setProjectionMatrix(projection);
    tigl::shader->setViewMatrix(view);
    

    tigl::shader->enableColor(true);
        
    cubes();
}

void cubes() {  
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1, 0, 0));
        model = glm::rotate(model, angle, glm::vec3(1, 1, 0));

        tigl::shader->setModelMatrix(model);

        tigl::begin(GL_QUADS);
        tigl::addVertex(Vertex::PC(glm::vec3(-5, 1, -5), glm::vec4(-5, 1, 5, 1)));
        tigl::addVertex(Vertex::PC(glm::vec3(-5, 1, 5), glm::vec4(5, 1, 5, 1)));
        tigl::addVertex(Vertex::PC(glm::vec3(5, 1, 5), glm::vec4(5, 1, -5, 1)));
        tigl::addVertex(Vertex::PC(glm::vec3(5, 1, -5), glm::vec4(-5, 1, -5, 1)));

        tigl::end();
}