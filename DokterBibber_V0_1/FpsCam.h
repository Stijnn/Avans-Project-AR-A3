#pragma once
#include <glm/glm.hpp>
#include "GameObject.h"	

struct GLFWwindow;

class FpsCam
{
public:
	FpsCam(GLFWwindow*);

	glm::mat4 getMatrix();
	void update(GLFWwindow*, GameObject&);
	void move(float angle, float fac);
	void setInit(); 
	void setCamera(float x, float y, float z); 
	glm::mat4 getPincetCamera();
	bool cameraPincetTrackingOn();

private:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec2 rotation = glm::vec2(1.5, 0);

	bool isCameraTrackOn = false; // zooms out the camera on default (focussed on the penguin mesh)	
	glm::mat4 pincetView;
};



