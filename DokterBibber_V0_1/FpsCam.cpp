#include "FpsCam.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


FpsCam::FpsCam(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

glm::mat4 FpsCam::getMatrix()
{
	glm::mat4 ret(1.0f);
	ret = glm::rotate(ret, rotation.x, glm::vec3(1, 0, 0));
	ret = glm::rotate(ret, rotation.y, glm::vec3(0, 1, 0));
	ret = glm::translate(ret, position);
	return ret;
}

void FpsCam::move(float angle, float fac)
{
	position.x += (float)cos(rotation.y + glm::radians(angle)) * fac;
	position.z += (float)sin(rotation.y + glm::radians(angle)) * fac;	
}

void FpsCam::setInit()
{
	setCamera(0, -15.0f, 0);
}

void FpsCam::setCamera(float x, float y, float z)
{
	position.x = x; 
	position.y = y; 
	position.z = z;
}

void FpsCam::update(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		setCamera(-1.5, -1, -0.3);
	//bolletje
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		setCamera(1.5, -1, 0);
	//hartje
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		setInit(); 
	//init
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		setCamera(2.2, -1, 0.4);
	//botinarm
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		setCamera(1.6, -1, 1.7);
	//bot
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		setCamera(0.4, -1, 1.6);
	//katapult
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		setCamera(1.2, -1, 3.0);
	//rondje
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		setCamera(-2.4, -1, 1.4);
	//driehoek
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		setCamera(0, -1, -3.2);
	//gatinhoofd
}