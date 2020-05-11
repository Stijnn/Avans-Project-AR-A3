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

cv::VideoCapture cap(1);
cv::Mat frame;

void init();
void update();
void draw();
void cubes();

void initCam();
void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture);
void showCameraScreen();


int main(void)
{
    initCam();

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
        cap.read(frame);
        cv::flip(frame, frame, 3);
        cv::imshow("Dokter Bibber!", frame);

        BindCVMat2GLTexture(frame, textureId);

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

void drawTexture()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
}

void draw()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 projection = glm::perspective(55.0f, width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, -1.0f));
    

    tigl::shader->setProjectionMatrix(projection);
    tigl::shader->setViewMatrix(view);
    

    tigl::shader->enableColor(true);
    tigl::shader->enableTexture(true);
        
    cubes();
    showCameraScreen();
}

void showCameraScreen() {
    tigl::begin(GL_QUADS);

    tigl::addVertex(Vertex::PT(glm::vec3(-8.5, 0, 0), glm::vec2(0, 0)));
    tigl::addVertex(Vertex::PT(glm::vec3(-8.5, 4.2, 0), glm::vec2(0, 1)));
    tigl::addVertex(Vertex::PT(glm::vec3(-4, 4.2, 0), glm::vec2(1, 1)));
    tigl::addVertex(Vertex::PT(glm::vec3(-4, 0, 0), glm::vec2(1, 0)));

    tigl::end();
}

void cubes() {  

   
}


void initCam()
{
    // Controle of de camera wordt herkend.
    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video cam" << std::endl;
        return;
    }
}

void BindCVMat2GLTexture(cv::Mat& image, GLuint& imageTexture)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,       
        0,                 
        GL_RGB,             
        image.cols,          
        image.rows,          
        0,                   
        GL_RGB,              
        GL_UNSIGNED_BYTE,    
        image.ptr());        
}