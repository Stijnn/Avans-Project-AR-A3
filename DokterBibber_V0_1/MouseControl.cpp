#include "MouseControl.h"
#include <GLFW\glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

MouseControl::MouseControl(GLFWwindow* window, const int height, const int width, const float scaler)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
	{
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	this->xPos = 0.0;
	this->yPos = 0.0;
	this->screenHeigth = height * scaler;
	this->screenWidth = width * scaler; 
}

double MouseControl::getMouseXPos()
{
	return this->xPos;
}

double MouseControl::getMouseYPos()
{
	return this->yPos;
}

void MouseControl::updateMousePos(GLFWwindow* window)
{
	glfwGetCursorPos(window, &this->xPos, &this->yPos);
}

void MouseControl::updateMouseMode(GLFWwindow* window, int glMouseMode)
{
	glfwSetInputMode(window, GLFW_CURSOR, glMouseMode);
}
