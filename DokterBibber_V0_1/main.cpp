#include "lib/imgui-1.76/imgui.h"
#include "lib/imgui-1.76/examples/imgui_impl_glfw.h"
#include "lib/imgui-1.76/examples/imgui_impl_opengl3.h"

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

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

#include <irrKlang.h>

#include "stb_image.h" 
#include <chrono>
#include <ctime>
#include "MouseControl.h"
#include "TextControl.h"
#include "FpsCam.h"

#include "Scene.h"
#include "BackgroundRemover.h"
#include "SkinDetector.h"
#include "FaceDetector.h"
#include "FingerCount.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll

irrklang::ISoundEngine* engine;
irrklang::ISound* backgroundMusic;

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
TextControl* textWriter;
static float sensivityScaler = 0.1;
constexpr float frameWidth = 1000 / 9;
constexpr float frameHeight = 1000 / 16;

float iLastX;
float iLastY;

float posX;
float posY;
float lastPosZ = 0.f;
float lastPosX = 0.f;

float xPosChange;
float zPosChange;

float yPos;

cv::Mat frame, frameOut, handMask, foreground, fingerCountDebug;

struct mat_texture_t
{
public:
    const std::string& texture_name;
    const GLuint& texture_id;
    const cv::Mat& cv_mat;
};

std::list<mat_texture_t> open_cv_material_textures;

BackgroundRemover backgroundRemover;
SkinDetector skinDetector;
FaceDetector faceDetector;
FingerCount fingerCount;

void init();
void update();
void draw();
void cubes();
void ongui();

bool initCam();
void BindCVMat2GLTexture(cv::Mat& image);
void showCameraScreen();

int main(void)
{
	// start the sound engine with default parameters
	engine = irrklang::createIrrKlangDevice();
	if (!engine)
		return 0; // error starting up the engine

    if (!glfwInit())
        throw "Could not initialize glwf";

#if __APPLE__
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    window = glfwCreateWindow(1920, 1080, "Dokter Bibber!", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        throw "Could not initialize glwf";
    }
    glfwMakeContextCurrent(window);

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        glfwTerminate();
        throw "Failed to initialize OpenGL loader!\n";
    }

	backgroundMusic = engine->play2D("backgroundHobbit.mp3", true, false, true);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    tigl::init();

    bool cameraAvailable = initCam();
    init();
    ImGui::StyleColorsDark();
    
    //open_cv_material_textures.push_back({"frame", BindCVMat2GLTexture()})

    // KalmanFilter for smoothing the handmovement
    KalmanFilter KF(4, 2, 0);
    Mat_<float> state(4, 1); /* (x, y, Vx, Vy) */
    Mat processNoise(4, 1, CV_32F);
    Mat_<float> measurement(2, 1); measurement.setTo(Scalar(0));
    char code = (char)-1;

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
            posX = fingerCount.getPosition().x;
            posY = fingerCount.getPosition().y;

            // KalmanFilter variable initilisation 
            KF.statePre.at<float>(0) = posX;
            KF.statePre.at<float>(1) = posY;
            KF.statePre.at<float>(2) = 0;
            KF.statePre.at<float>(3) = 0;
            KF.transitionMatrix = (Mat_<float>(4, 4) << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

            setIdentity(KF.measurementMatrix);
            setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
            setIdentity(KF.measurementNoiseCov, Scalar::all(1e-1));
            setIdentity(KF.errorCovPost, Scalar::all(.1));

            // KalmanFilter handmovement prediction and correction
            Mat prediction = KF.predict();
            Point predictPt(prediction.at<float>(0), prediction.at<float>(1));

            measurement(0) = posX;
            measurement(1) = posY;

            Point measPt(measurement(0), measurement(1));

            Mat estimated = KF.correct(measurement);
            Point statePt(estimated.at<float>(0), estimated.at<float>(1));

            posX = statePt.x;
            posY = statePt.y;

            if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
            {
                circle(frameOut, Point(posX, posY), 5, Scalar(255, 0, 255), 2, 8);
                //Draw a red line from the previous point to the current point
                //line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
            }

            imshow("output", frameOut);
            //imshow("foreground", foreground);
            //imshow("handMask", handMask);
            imshow("handDetection", fingerCountDebug);

            BindCVMat2GLTexture(frame);

            int key = waitKey(1);

            if (key == 98) // b
                backgroundRemover.calibrate(frame);
            else if (key == 115) // s            
                skinDetector.calibrate(frame);        
                
        }
		update();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ongui();
        ImGui::Render();

        draw();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwTerminate();

    return 0;
}

FpsCam* camera;

void init()
{
	g_ptrMainScene = new Scene();
	g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Pinguin"));
	g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Pincet"));
    //g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Bone"));
    //g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Heart"));
    ////g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Gun"));
    //g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Circle"));
    //g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Triangle"));
    //g_ptrMainScene->GetRootObject()->AddChild(GameObject::Instantiate("Monkey"));

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        if (key == GLFW_KEY_HOME)
        {
            GameObject* pincetObj = (*g_ptrMainScene->GetRootObject())["Pincet"];
            pincetObj->SetPosition(glm::vec3(0, 3, 0));
        }          
		if (key == GLFW_KEY_SPACE)
		{
			engine->play2D("Buzzer.mp3", false);
		} 
        if (key == GLFW_KEY_Z)
		{
			backgroundMusic->setIsPaused(!backgroundMusic->getIsPaused());
		}
        if (key == GLFW_KEY_PAGE_UP)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // changes the cursor to normal
        }
        if (key == GLFW_KEY_PAGE_DOWN)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // changes the cursor to hidden
        }
        if (key == GLFW_KEY_UP)
        {
            yPos+=0.1; // incr. yPos
        }
        if (key == GLFW_KEY_DOWN)
        {
            yPos-=0.1; // decr. yPos
        }
    });

	camera = new FpsCam(window);

	//set camera to standard postion
	camera->setInit(); 

    glEnable(GL_DEPTH_TEST);

	//Add MeshComponent to Pinguin GameObject
	
	/// <summary>
	/// Nieuwe manier van inladen
	/// </summary>
	
	GameObject* pinguinObj = (*g_ptrMainScene->GetRootObject())["Pinguin"]; // Verkrijgen van het opgeslagen gameobject
    MeshComponent* penguinMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pinguin/PinguinBibber.obj"));
	pinguinObj->AddComponent(penguinMesh); // Het Toevoegen van een meshcomponent met data

    colDectPenguin = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    colDectPenguin->initializeCollisionFrame(penguinMesh->GetVertices()); // Het overgeven van de mesh data
	pinguinObj->AddComponent(colDectPenguin); // Het toevoegen van de collision component

    GameObject* pincetObj = (*g_ptrMainScene->GetRootObject())["Pincet"]; // Verkrijgen van het opgeslagen gameobject
    MeshComponent* pincetMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pincet/Pincet.obj"));
    pincetObj->AddComponent(pincetMesh); // Het Toevoegen van een meshcomponent met data
    pincetMesh->GetTransform()->SetScale(glm::vec3(0.01f));
    pincetMesh->GetTransform()->SetRotation(glm::vec3(20,0,-85)); 
     
    colDect = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    colDect->setMeshScalingValue(0.1f);
    colDect->initializeCollisionFrame(pincetMesh->GetVertices()); // Het overgeven van de mesh data
    pincetObj->AddComponent(colDect); // Het toevoegen van de collision component

    //Pick-up objecten

    ////Bone
    //GameObject* BoneObj = (*g_ptrMainScene->GetRootObject())["Bone"]; // Verkrijgen van het opgeslagen gameobject
    //MeshComponent* BoneMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pickup/PingquinBibberBoneobj.obj"));
    //BoneObj->AddComponent(BoneMesh); // Het Toevoegen van een meshcomponent met data

    //colDect = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    //colDect->initializeCollisionFrame(BoneMesh->GetVertices()); // Het overgeven van de mesh data
    //BoneObj->AddComponent(colDect); // Het toevoegen van de collision component

    ////Heart
    //GameObject* HeartObj = (*g_ptrMainScene->GetRootObject())["Heart"]; // Verkrijgen van het opgeslagen gameobject
    //MeshComponent* HeartMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pickup/PingquinBibberHeart.obj"));
    //HeartObj->AddComponent(HeartMesh); // Het Toevoegen van een meshcomponent met data

    //colDect = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    //colDect->initializeCollisionFrame(HeartMesh->GetVertices()); // Het overgeven van de mesh data
    //HeartObj->AddComponent(colDect); // Het toevoegen van de collision component

    ////Circle
    //GameObject* CircleObj = (*g_ptrMainScene->GetRootObject())["Circle"]; // Verkrijgen van het opgeslagen gameobject
    //MeshComponent* CircleMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pickup/PingquinBibberCircle.obj"));
    //CircleObj->AddComponent(CircleMesh); // Het Toevoegen van een meshcomponent met data

    //colDect = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    //colDect->initializeCollisionFrame(CircleMesh->GetVertices()); // Het overgeven van de mesh data
    //CircleObj->AddComponent(colDect); // Het toevoegen van de collision component

    ////Gun
    ////GameObject* GunObj = (*g_ptrMainScene->GetRootObject())["Gun"]; // Verkrijgen van het opgeslagen gameobject
    ////MeshComponent* GunMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pickup/PingquinBibberGun.obj"));
    ////GunObj->AddComponent(GunMesh); // Het Toevoegen van een meshcomponent met data

    ////colDect = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    ////colDect->initializeCollisionFrame(GunMesh->GetVertices()); // Het overgeven van de mesh data
    ////GunObj->AddComponent(colDect); // Het toevoegen van de collision component

    ////Monkey
    //GameObject* MonkeyObj = (*g_ptrMainScene->GetRootObject())["Monkey"]; // Verkrijgen van het opgeslagen gameobject
    //MeshComponent* MonkeyMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pickup/PingquinBibberMonkey.obj"));
    //MonkeyObj->AddComponent(MonkeyMesh); // Het Toevoegen van een meshcomponent met data

    //colDect = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    //colDect->initializeCollisionFrame(MonkeyMesh->GetVertices()); // Het overgeven van de mesh data
    //MonkeyObj->AddComponent(colDect); // Het toevoegen van de collision component

    ////triangle
    //GameObject* TriangleObj = (*g_ptrMainScene->GetRootObject())["Triangle"]; // Verkrijgen van het opgeslagen gameobject
    //MeshComponent* TriangleMesh = Component::Instantiate<MeshComponent>(&ObjModel::load("Data/Models/Pickup/PingquinBibberTriangle.obj"));
    //TriangleObj->AddComponent(TriangleMesh); // Het Toevoegen van een meshcomponent met data

    //colDect = Component::Instantiate<CollisionDetectionComponent>(); // Het aanmaken van een collision component
    //colDect->initializeCollisionFrame(TriangleMesh->GetVertices()); // Het overgeven van de mesh data
    //TriangleObj->AddComponent(colDect); // Het toevoegen van de collision component

    /*std::vector<Vertex> pincetModel = ObjModel::load("C:\\Users\\finni\\Documents\\Programming\\Project-Objects\\PincetV1.obj");
    meshComponentPincet = Component::Instantiate<MeshComponent>(&pincetModel);
    colDect = Component::Instantiate<CollisionDetectionComponent>();
    colDect->setMeshScalingValue(0.1f);
    colDect->initializeCollisionFrame(pincetModel);    */

    textWriter = new TextControl("C:/Windows/Fonts/times.ttf", 20, 1920.0f, 1080.0f);

    //inits the lighting 
    tigl::shader->enableLighting(true);
    tigl::shader->setLightCount(1);
    tigl::shader->setLightDirectional(0, false); 
    tigl::shader->setLightPosition(0, glm::vec3(2, 2, 3)); // position of the lighting
    tigl::shader->setLightAmbient(0, glm::vec3(0.1f, 0.1f, 0.15f)); // ambient color
    tigl::shader->setLightDiffuse(0, glm::vec3(0.8f, 0.8f, 0.8f)); // diffuse color factor
    tigl::shader->setLightSpecular(0, glm::vec3(0, 0, 0)); // spotlight effect, and color
    tigl::shader->setShinyness(32.0f); // reflective factor
}

void update()
{      
	g_ptrMainScene->GetRootObject()->Update(0);

    duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

    //Check collision {
        //If collision -> Game.AddStrike();
    //}

    //If(Game.IsGameOver()) {
        //If game is done, do something.
        //To Reset, use Game.StartNewGame(); 
    //}

    // manages the position of the pincet
    xPosChange = posX - lastPosX;
    lastPosX = posX;
    if (xPosChange < 1 )
    {

    }
    else
    {
        
    }
    zPosChange = posY - lastPosZ;
    lastPosZ = posY;
    if (zPosChange < 1 )
    {

    }
    else
    {
       
    }    
    xPosChange *= sensivityScaler; // adds the sensivity scaling to the Xpos, for finer movement
    zPosChange *= sensivityScaler; // adds the sensivity scaling to the Zpos, for finer movement

    GameObject* pincetObj = (*g_ptrMainScene->GetRootObject())["Pincet"];

    camera->update(window, *pincetObj);
}

static bool wireframe_enabled = false;
void draw()
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if (height < 1)
    {
        return;
    }

    glViewport(0, 0, width, height);

    glClearColor(0.3f, 0.4f, 0.6f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (wireframe_enabled)
    {
        glEnable(GL_POLYGON_MODE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glDisable(GL_POLYGON_MODE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

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
    
    GameObject* pincetObj = (*g_ptrMainScene->GetRootObject())["Pincet"];
    
    glm::vec3 pos = pincetObj->GetPosition();
    std::string coordsXY("X: " + std::to_string((pos.x + xPosChange)) + ", Z: " + std::to_string((pos.z + zPosChange )));

    // coord text
    textWriter->setScale(1.1f);
    textWriter->drawText(coordsXY, -2020, -1080);

    // pincet movement

	/// <summary>
	/// Example pincet movement met gameobjects
	/// 1 - Retrieve actual object
	/// 2 - Get Pos
	/// 3 - Update Pos
	/// 4 - Set Pos
	/// </summary>
    
    float posX = (pos.x + xPosChange);
    float posZ = (pos.z + zPosChange);

    pos = glm::vec3( (posX), yPos, (posZ));
    pincetObj->SetPosition(pos);

    glm::mat4 projection = glm::perspective(glm::radians(75.0f), width / (float)height, 0.1f, 100.0f);

    tigl::shader->setProjectionMatrix(projection);
   // tigl::shader->setViewMatrix(view);

    if (camera->cameraPincetTrackingOn())
    {
        tigl::shader->setViewMatrix(camera->getPincetCamera());
        sensivityScaler = 0.02f;
    }
    else
    {
        tigl::shader->setViewMatrix(camera->getMatrix());
        sensivityScaler = 0.1f;
    }

    //tigl::shader->enableTexture(true);
    //glBindTexture(GL_TEXTURE_2D, textureId);
    //showCameraScreen();
    //tigl::shader->enableTexture(false);

	g_ptrMainScene->GetRootObject()->Draw();
	cubes(); 
}

void cubes() {  
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1, 0, 0));
    model = glm::rotate(model, 0.f, glm::vec3(1, 1, 0));

    tigl::shader->setModelMatrix(model);
	tigl::shader->enableAlphaTest(true); 

    tigl::begin(GL_QUADS);
	
	//rondje
	double x = 0;
	double z = 0;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
	//driehoek
	x = 1;
	z = -1.2;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
	//katapult
	x = -1.5;
	z = -1.6;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
	//bot
	x = -2.5;
	z = -1.8;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
	//cirkel
	x = -2.5;
	z = -3;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
	//hartje
	x = -2.5;
	z = -0.5;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
	//botinarm
	x = -3.2;
	z = -0.5;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
	//Gatinhoofd
	x = -1.3;
	z = 2.7;
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, -0.5 + z), glm::vec4(-5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(-0.5 + x, 1, 1 + z), glm::vec4(5, 1, 5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, 1 + z), glm::vec4(5, 1, -5, 0)));
	tigl::addVertex(Vertex::PC(glm::vec3(1 + x, 1, -0.5 + z), glm::vec4(-5, 1, -5, 0)));
    tigl::end();
}

void ongui_tree_recursive(GameObject* go)
{
    if (ImGui::TreeNodeEx(go->GetName().c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed))
    {
        float positions[3] = { go->GetPosition().x, go->GetPosition().y, go->GetPosition().z };
        if (ImGui::SliderFloat3("Position", positions, -100, 100))
            go->SetPosition(glm::vec3(positions[0], positions[1], positions[2]));

        float rotations[3] = { go->GetRotation().x, go->GetRotation().y, go->GetRotation().z };
        if (ImGui::SliderFloat3("Rotation", rotations, -365, 365))
            go->SetRotation(glm::vec3(rotations[0], rotations[1], rotations[2]));

        float scaling[3] = { go->GetScale().x, go->GetScale().y, go->GetScale().z };
        if (ImGui::SliderFloat3("Scale", scaling, -5, 5))
            go->SetScale(glm::vec3(scaling[0], scaling[1], scaling[2]));

        for (Component* comp : go->GetComponents())
        {
            if (ImGui::TreeNodeEx(comp->GetComponentName().c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Framed))
            {
                ImGui::TreePop();
            }
        }

        for (GameObject* child : go->GetChildren())
        {
            ongui_tree_recursive(child);
        }
        ImGui::TreePop();
    }
}

void ongui()
{
    if (ImGui::Begin("Debug"))
    {
        ImGui::SliderFloat("Sensitivity", &sensivityScaler, 0.0f, 5.0f);
        ImGui::Checkbox("Wireframe Mode", &wireframe_enabled);

        ImGui::Text("Display: (%.0fx%.0f)", ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
        ImGui::Text("Framerate: %f", ImGui::GetIO().Framerate);
        ImGui::Separator();
        ImGui::BeginChild("Scrolling");
        if (ImGui::TreeNode("Scene"))
        {
            ongui_tree_recursive(g_ptrMainScene->GetRootObject());
            ImGui::TreePop();
        }
        ImGui::EndChild();
    }
    ImGui::End();

    if (ImGui::Begin("Camera"))
    {
        ImGui::Image((void*)(intptr_t)textureId, ImVec2(512, 512));
    }
    ImGui::End();
}

bool initCam()
{
    // Controle of de camera wordt herkend.
    if (!cap.isOpened())
    {
        std::cout << "Cannot open the video cam" << std::endl;
        return false;
    }
    glGenTextures(1, &textureId); // init
    glBindTexture(GL_TEXTURE_2D, textureId);// one-time needed for init

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // init
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // init
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
    glBindTexture(GL_TEXTURE_2D, textureId);// needed for every frame

    glTexImage2D(GL_TEXTURE_2D, // needed for every frame
        0,
        GL_RGB,
        image.cols,
        image.rows,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        image.ptr());
}