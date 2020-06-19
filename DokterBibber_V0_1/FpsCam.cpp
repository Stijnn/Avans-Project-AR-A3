#include "FpsCam.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "GameObject.h"


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
	setCamera(0, -10.0f, 0);
}

void FpsCam::setCamera(float x, float y, float z)
{
	position.x = x; 
	position.y = y; 
	position.z = z;
}

glm::mat4 FpsCam::getPincetCamera()
{
	return this->pincetView;
}

bool FpsCam::cameraPincetTrackingOn()
{
	return this->isCameraTrackOn;
}

void FpsCam::update(GLFWwindow* window, GameObject& pincet)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		this->isCameraTrackOn = true;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		this->isCameraTrackOn = false;
	}

	if (this->isCameraTrackOn) // follows the pincet on a zoomed in view
	{
		pincetView = glm::lookAt(glm::vec3(pincet.GetPosition().x + 0.1, pincet.GetPosition().y, pincet.GetPosition().z + 0.1), glm::vec3(pincet.GetPosition().x, -2 , pincet.GetPosition().z), glm::vec3(0, 1, -1.0f));
	}
	else // zooms out to the penguin
	{
		setCamera(0, -10.0f, 0); //  set camera to default position
	}

	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
	//{
	//	setCamera(-1.5, -1, -0.3);
	//	pincet.SetPosition(glm::vec3(-1.5, -1, -0.3));
	//}
	//	
	////bolletje
	//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//{
	//	setCamera(1.5, -1, 0);
	//	pincet.SetPosition(glm::vec3(-1.5, -1, -0.3));
	//}
	//	
	////hartje
	//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	//{
	//	setInit();
	//	pincet.SetPosition(glm::vec3(0, -2, 0));
	//}
	//	
	////init
	//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	//{
	//	setCamera(2.2, -1, 0.4);
	//	pincet.SetPosition(glm::vec3(2.2, -1, 0.4));
	//}
	//	
	////botinarm
	//if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	//	setCamera(1.6, -1, 1.7);
	//	//pincet.SetPosition(glm::vec3(1.6, -1, 1.7));
	////bot
	//if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	//	setCamera(0.4, -1, 1.6);
	//	//pincet.SetPosition(glm::vec3(-1.5, -1, -0.3));
	////katapult
	//if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	//	setCamera(1.2, -1, 3.0);
	//	//pincet.SetPosition(glm::vec3(-1.5, -1, -0.3));
	////rondje
	//if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	//	setCamera(-2.4, -1, 1.4);
	//	//pincet.SetPosition(glm::vec3(-1.5, -1, -0.3));
	////driehoek
	//if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	//	setCamera(0, -1, -3.2);
	//	//pincet.SetPosition(glm::vec3(-1.5, -1, -0.3));
	////gatinhoofd
}