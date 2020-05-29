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

#include "GameObject.h"
#include "MeshComponent.h"
#include "CollisionDetectionComponent.h"

using tigl::Vertex;

#include "stb_image.h" 
#include <chrono>
#include <ctime>
#include "MouseControl.h"
#include "TextControl.h"

#include "Scene.h"
#include "BackgroundRemover.h"
#include "SkinDetector.h"
#include "FaceDetector.h"
#include "FingerCount.h"

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

MeshComponent* meshComponent;
MeshComponent* meshComponentPincet;
CollisionDetectionComponent* colDect; // a lose collision component
CollisionDetectionComponent* colDectPenguin; // a lose collision component

cv::VideoCapture cap(0);

Scene* g_ptrMainScene;

GLFWwindow* window;
GLuint textureId = -1;
double duration;
std::clock_t start;
static MouseControl* mouse;
TextControl* textWriter;
constexpr float sensivityScaler = 0.1;
constexpr float frameWidth = 1000 / 9;
constexpr float frameHeight = 1000 / 16;

int iLastX;
int iLastY;
int posX;
int posY;

cv::Mat frame, frameOut, handMask, foreground, fingerCountDebug;

BackgroundRemover backgroundRemover;
SkinDetector skinDetector;
FaceDetector faceDetector;
FingerCount fingerCount;

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

    window = glfwCreateWindow(1920, 1080, "Dokter Bibber!", NULL, NULL);

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

            frameOut = frame.clone();

            skinDetector.drawSkinColorSampler(frameOut);

            foreground = backgroundRemover.getForeground(frame);

            faceDetector.removeFaces(frame, foreground);
            handMask = skinDetector.getSkinMask(foreground);
            fingerCountDebug = fingerCount.findFingersCount(handMask, frameOut);

            Moments oMoments = moments(handMask);

            double dM01 = oMoments.m01;
            double dM10 = oMoments.m10;
            double dArea = oMoments.m00;

            //calculate the position of the ball
            posX = dM10 / dArea;
            posY = dM01 / dArea;

            if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
            {
                circle(frameOut, Point(posX, posY), 5, Scalar(255, 0, 255), 2, 8);
                //Draw a red line from the previous point to the current point
                //line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
            }

            imshow("output", frameOut);
            imshow("foreground", foreground);
            imshow("handMask", handMask);
            imshow("handDetection", fingerCountDebug);

            BindCVMat2GLTexture(frame);

            int key = waitKey(1);

            if (key == 98) // b
                backgroundRemover.calibrate(frame);
            else if (key == 115) // s
                skinDetector.calibrate(frame);
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
	g_ptrMainScene = new Scene();
	g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Pinguin"));
	g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Pincet"));

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

	//Add MeshComponent to Pinguin GameObject
	
	/// <summary>
	/// Nieuwe manier van inladen
	/// </summary>
	
	GameObject* pinguinObj = (*g_ptrMainScene->GetRootObject())["Pinguin"]; // Verkrijgen van het opgeslagen gameobject
    MeshComponent* c = Component::Instantiate<MeshComponent>(&ObjModel::load("D:\\Avans\\Leerjaar 2\\Periode 4\\Project\\Avans-Project-AR-A3\\PingquinBibberV1.obj"));
	pinguinObj->AddComponent((Component*)c); // Het Toevoegen van een meshcomponent met data

    colDectPenguin = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    colDectPenguin->initializeCollisionFrame(c->GetVertices()); // Het overgeven van de mesh data
	pinguinObj->AddComponent((Component*)colDectPenguin); // Het toevoegen van de collision component


    std::vector<Vertex> pincetModel = ObjModel::load("D:\\Avans\\Leerjaar 2\\Periode 4\\Project\\Avans-Project-AR-A3\\PincetV1.obj");
    meshComponentPincet = Component::Instantiate<MeshComponent>(&pincetModel);
    colDect = Component::Instantiate<CollisionDetectionComponent>();
    colDect->setMeshScalingValue(0.1f);
    colDect->initializeCollisionFrame(pincetModel);    

    mouse = new MouseControl(window, frameHeight, frameWidth, sensivityScaler);
    textWriter = new TextControl("C:/Windows/Fonts/times.ttf", 20, 1920.0f, 1080.0f);
}

void update()
{    
    mouse->updateMousePos(window);

	g_ptrMainScene->GetRootObject()->Update(0);

	/// <summary>
	/// TODO: Even uitwerken hoe dit gemakkelijker kan
	/// </summary>
	
    //colDect->SetPosition(meshComponentPincet->GetPosition());

    //if (colDect->checkComponentCollision(*colDect, *colDectPenguin))
    //{
    //    std::cout << "Collision!" << std::endl;
    //}

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
    //std::string coordsXY("X: " + std::to_string(-mouse->getMouseXPos() * (sensivityScaler)) + ", coord Y: " + std::to_string(-mouse->getMouseYPos() * (sensivityScaler)));
    std::string coordsXY("X: " + std::to_string(posX * (sensivityScaler)) + ", coord Y: " + std::to_string(posY * (sensivityScaler)));

    // coord text
    textWriter->setScale(1.1f);
    textWriter->drawText(coordsXY, -2020, -1080);
    tigl::shader->enableColor(true);

    // pincet movement

	/// <summary>
	/// Example pincet movement met gameobjects
	/// 1 - Retrieve actual object
	/// 2 - Get Pos
	/// 3 - Update Pos
	/// 4 - Set Pos
	/// </summary>
	GameObject* pincetObj = (*g_ptrMainScene->GetRootObject())["Pincet"];
	glm::vec3 pos = pincetObj->GetPosition();
    //pos = glm::vec3(-mouse->getMouseXPos() * (sensivityScaler), 0, -mouse->getMouseYPos() * (sensivityScaler));
    pos = glm::vec3(posX * (sensivityScaler), 0, posY * (sensivityScaler));
    pincetObj->SetPosition(pos);

    glm::mat4 projection = glm::perspective(55.0f, width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0, 10.0f, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, -1.0f));

    tigl::shader->setProjectionMatrix(projection);
    tigl::shader->setViewMatrix(view);

    tigl::shader->enableTexture(true);
    tigl::shader->enableColor(true);

    glBindTexture(GL_TEXTURE_2D, textureId);

    showCameraScreen();

	g_ptrMainScene->GetRootObject()->Draw();
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