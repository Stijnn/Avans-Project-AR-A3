// 2.1_Camera.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define STB_IMAGE_IMPLEMENTATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp" 
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\ext\quaternion_trigonometric.hpp>
#include "stb_image.h" 
#include <string>
#include <Windows.h>

#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")

using namespace cv;
using namespace std;

GLFWwindow* window;
GLuint textureId = -1;

void init();
void update();
void draw();

int main(int argc, char* argv[])
{
	if (!glfwInit())
		throw "Could not initialize glwf";
	window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
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

	//// Open de camera met nummer 1 in lijst (red.: nr 0 was bij mij de camera in de klep van mijn laptop)  
	//VideoCapture cap(0);

	//cap.set(10, 170);			// # brightness		min : 0, max : 255, Increment : 1
	//cap.set(11, 50);		// # contrast       min : 0, max : 255, increment : 1
	//cap.set(12, 70);		// # saturation     min : 0, max : 255, increment : 1
	//cap.set(13, 13);		//# hue
	//cap.set(14, 1);			// # gain           min : 0, max : 127, increment : 1
	//cap.set(15, -3);		// # exposure       min : -7, max : -1, increment : 1
	//cap.set(17, 4000);		// # white_balance  min : 4000, max : 7000, increment : 1
	//cap.set(28, 0);			// # focus          min : 0, max : 255, increment : 5

	//// Controle of de camera wordt herkend.
	//if (!cap.isOpened())
	//{
	//	cout << "Cannot open the video cam" << endl;
	//	return -1;
	//}

	//// Breedte en hooogte van de frames die de camera genereert ophalen. 
	//double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	//double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	//cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	//// Window maken waarin de beelden "live" getoond worden
	//namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);

	//// Continue loop waarin een beeld wordt opgehaald en wordt getoond in het window
	//Mat frame;

	////int keyInput = waitKey(1);

	//while (1)
	//{

	//	// Lees een nieuw frame
	//	bool bSuccess = cap.read(frame);

	//	flip(frame, frame, 3);

	//	// Controlleer of het frame goed gelezen is.
	//	if (!bSuccess)
	//	{
	//		cout << "Cannot read a frame from video stream" << endl;
	//		break;
	//	}

	//	// Het tonen van grijswaarde beeld
	//	imshow("MyVideo", frame);

	//	//  Wacht 30 ms op ESC-toets. Als ESC-toets is ingedrukt verlaat dan de loop
	//	if (waitKey(1) == 27)
	//	{
	//		cout << "esc key is pressed by user" << endl;
	//		break;
	//	}
	//}
	//return 0;
}

void init()
{
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (key == GLFW_KEY_ESCAPE)
				glfwSetWindowShouldClose(window, true);
		});
}


void update()
{

}

void draw()
{

}

