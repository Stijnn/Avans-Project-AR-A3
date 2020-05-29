#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;

class FpsCam
{
public:
	FpsCam(GLFWwindow*);

	glm::mat4 getMatrix();
	void update(GLFWwindow*);
	void move(float angle, float fac);
	void setInit(); 
	void setCamera(float x, float y, float z); 
private:
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec2 rotation = glm::vec2(1.5, 0);

	
};



