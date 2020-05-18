#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "tigl.h"
#include <glm/gtc/matrix_transform.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>  

#include "MeshComponent.h"

using tigl::Vertex;

#include "stb_image.h" 
#include <chrono>
#include <ctime>
#include "MouseControl.h"
#include "TextControl.h"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

MeshComponent* meshComponent;
MeshComponent* meshComponentPincet;

cv::VideoCapture cap(1);
cv::Mat frame;

GLFWwindow* window;
GLuint textureId = -1;
double duration;
std::clock_t start;
static MouseControl* mouse;
TextControl* textWriter;
constexpr float sensivityScaler = 0.1;
constexpr float frameWidth = 1000 / 9;
constexpr float frameHeight = 1000 / 16;

void init();
void update();
void draw();
void cubes();

bool initCam();
void BindCVMat2GLTexture(cv::Mat& image);
void showCameraScreen();

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

    bool cameraAvailable = initCam();
    init();
    

	while (!glfwWindowShouldClose(window))
	{
        if (cameraAvailable) {
            cap.read(frame);
            cv::flip(frame, frame, 3);
            BindCVMat2GLTexture(frame);
        }

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
        if (key == GLFW_KEY_HOME)
            glfwSetCursorPos(window, 0, 0);       
        if (key == GLFW_KEY_N)        
            mouse->updateMouseMode(window, GLFW_CURSOR_NORMAL); // makes the mouse visible by pressing N
        if (key == GLFW_KEY_H)
            mouse->updateMouseMode(window, GLFW_CURSOR_DISABLED); // makes the mouse invisible by pressing H
    });

    glEnable(GL_DEPTH_TEST);

    //Loads model from file Pinguin
	//std::vector<Vertex> pinguinModel = ObjModel::load("E:\\Avans\\Proftaak AR\\DokterBibber_V0_1\\x64\\Debug\\PingquinBibber.obj");
    std::vector<Vertex> pinguinModel = ObjModel::load("D:\\Avans\\Leerjaar 2\\Periode 4\\Project\\Avans-Project-AR-A3\\PingquinBibberV1.obj");
    //std::vector<Vertex> pinguinModel = ObjModel::load("C:\\Users\\finni\\Documents\\Programming\\Project-Objects\\PingquinBibberV1.obj");
	meshComponent = Component::Instantiate<MeshComponent>(&pinguinModel);
	meshComponent->SetScale(1.0f);

    //Loads model from file Pincet
    std::vector<Vertex> pincetModel = ObjModel::load("D:\\Avans\\Leerjaar 2\\Periode 4\\Project\\Avans-Project-AR-A3\\PincetV1.obj");
    //std::vector<Vertex> pincetModel = ObjModel::load("C:\\Users\\finni\\Documents\\Programming\\Project-Objects\\PincetV1.obj");
    meshComponentPincet = Component::Instantiate<MeshComponent>(&pincetModel);
    meshComponentPincet->SetScale(0.2f);

    mouse = new MouseControl(window, frameHeight, frameWidth, sensivityScaler);
    textWriter = new TextControl("C:/Windows/Fonts/times.ttf", 20, 1920.0f, 1080.0f);
}

void update()
{    
    mouse->updateMousePos(window);
	meshComponent->Update(0);
    meshComponentPincet->Update(0);
    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

    //Check collision {
        //If collision -> Game.AddStrike();
    //}

    //If(Game.IsGameOver()) {
        //If game is done, do something.
        //To Reset, use Game.StartNewGame();
    //}
}

void draw()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // timer
    std::stringstream buffer;
    int seconds, hours, minutes;
    seconds = duration;
    minutes = seconds / 60;
    hours = minutes / 60;
    buffer << int(hours) << ":" << int(minutes % 60)
        << ":" << int(seconds % 60);

    textWriter->setScale(1.4f);
    textWriter->drawText(buffer.str(), 0, -800);
    std::string coordsXY("X: " + std::to_string(-mouse->getMouseXPos() * (sensivityScaler)) + ", coord Y: " + std::to_string(-mouse->getMouseYPos() * (sensivityScaler)));

    // coord text
    textWriter->setScale(1.1f);
    textWriter->drawText(coordsXY, -2020, -1080);
    tigl::shader->enableColor(true);

    // pincet movement
    meshComponentPincet->SetPosition(glm::vec3(-mouse->getMouseXPos() * (sensivityScaler), 0, -mouse->getMouseYPos() * (sensivityScaler)));

    glm::mat4 projection = glm::perspective(55.0f, width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 10.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, -1.0f));

    tigl::shader->setProjectionMatrix(projection);
    tigl::shader->setViewMatrix(view);

    tigl::shader->enableTexture(true);
    tigl::shader->enableColor(true);

    glBindTexture(GL_TEXTURE_2D, textureId);

    showCameraScreen();

    meshComponent->Draw(0);
    meshComponentPincet->Draw(0);
}

void cubes() {  
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1, 0, 0));
    model = glm::rotate(model, 0.f, glm::vec3(1, 1, 0));

    tigl::shader->setModelMatrix(model);

    tigl::begin(GL_QUADS);
    tigl::addVertex(Vertex::PC(glm::vec3(-5, 1, -5), glm::vec4(-5, 1, 5, 1)));
    tigl::addVertex(Vertex::PC(glm::vec3(-5, 1, 5), glm::vec4(5, 1, 5, 1)));
    tigl::addVertex(Vertex::PC(glm::vec3(5, 1, 5), glm::vec4(5, 1, -5, 1)));
    tigl::addVertex(Vertex::PC(glm::vec3(5, 1, -5), glm::vec4(-5, 1, -5, 1)));

    tigl::end();
}

bool initCam()
{
    // Controle of de camera wordt herkend.
    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video cam" << std::endl;
        return false;
    }
    return true;
}

void showCameraScreen() {
    tigl::begin(GL_QUADS);

    tigl::addVertex(Vertex::PT(glm::vec3(-17, 0, 2), glm::vec2(0, 0)));
    tigl::addVertex(Vertex::PT(glm::vec3(-17, 0, -8), glm::vec2(0, 1)));
    tigl::addVertex(Vertex::PT(glm::vec3(-8, 0, -8), glm::vec2(1, 1)));
    tigl::addVertex(Vertex::PT(glm::vec3(-8, 0, 2), glm::vec2(1, 0)));

    tigl::end();
}


void BindCVMat2GLTexture(cv::Mat& image)
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