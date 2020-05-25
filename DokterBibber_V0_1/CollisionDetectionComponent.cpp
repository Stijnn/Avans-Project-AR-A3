#include "CollisionDetectionComponent.h"
#include <algorithm>
#include <glm\ext\matrix_transform.hpp>

void CollisionDetectionComponent::Initialize(void* a_Parameter)
{

}

void CollisionDetectionComponent::Update(float a_DeltaTime)
{

}

void CollisionDetectionComponent::Draw(float a_DeltaTime)
{
	glm::mat4 modelMatrix(1);
	modelMatrix = glm::translate(modelMatrix, m_Position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Scale, m_Scale, m_Scale));

	tigl::shader->setModelMatrix(modelMatrix);

	// DEBUG
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	tigl::shader->enableColor(true);
	tigl::drawVertices(GL_QUADS, this->collisionVerts);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool CollisionDetectionComponent::checkComponentCollision(const CollisionDetectionComponent& main_object, const CollisionDetectionComponent& second_object)
{
	/*bool collisionX = main_object.xMin <= second_object.xMax &&
		main_object.xMax >= second_object.xMin;

	bool collisionY = main_object.yMin <= second_object.yMax &&
		main_object.yMax >= second_object.yMin;

	bool collisionZ = main_object.zMin <= second_object.zMax &&
		main_object.zMax >= second_object.zMin;*/

	bool collisionX = main_object.m_Position.x + main_object.xLength >= second_object.m_Position.x &&
		second_object.m_Position.x + second_object.xLength >= main_object.m_Position.x;

	bool collisionY = main_object.m_Position.y + main_object.yLength >= second_object.m_Position.y &&
		second_object.m_Position.y + second_object.yLength >= main_object.m_Position.y;

	bool collisionZ = main_object.m_Position.z + main_object.zLength >= second_object.m_Position.z &&
		second_object.m_Position.z + second_object.zLength >= main_object.m_Position.z;

	return collisionX && collisionY && collisionZ; // if there is collision on the object detect correctly
}

std::vector<tigl::Vertex> CollisionDetectionComponent::getMeshVertexVector()
{
	return this->meshVerts;
}

void CollisionDetectionComponent::setMeshScalingValue(const float scale)
{
	this->meshScalingValue = scale;
}

void CollisionDetectionComponent::initializeCollisionFrame(const std::vector<tigl::Vertex>& objectVerts)
{
	this->xMin = objectVerts.at(0).position.x;
	this->xMax = objectVerts.at(0).position.x;
	this->yMin = objectVerts.at(0).position.y;
	this->yMax = objectVerts.at(0).position.y;
	this->zMin = objectVerts.at(0).position.z;
	this->zMax = objectVerts.at(0).position.z;

	calculateMinMax(objectVerts);

	this->xLength = this->xMax - this->xMin;
	this->yLength = this->yMax - this->yMin;
	this->zLength = this->zMax - this->zMin;

	float extrudeModifier = (calculateExtrudeModifier(this->xLength, this->yLength, this->zLength) / this->extruderDivisionScaler);

	this->xMin -= extrudeModifier;
	this->xMax += extrudeModifier;
	this->yMin -= extrudeModifier;
	this->yMax += extrudeModifier;
	this->zMin -= extrudeModifier;
	this->zMax += extrudeModifier;

	buildCollisionFrame(glm::vec3((this->xLength / 2), (this->yLength + extrudeModifier) / 2, (this->zLength + extrudeModifier) / 2));
}

void CollisionDetectionComponent::showCollisionFrame(bool showFrame)
{
	this->debugWireFrames = showFrame;
}

float CollisionDetectionComponent::calculateExtrudeModifier(const float& x, const float& y, const float& z)
{
	return std::min({ x, y, z });
}

void CollisionDetectionComponent::calculateMinMax(const std::vector<tigl::Vertex>& objectVerts)
{
	float temp;

	for (tigl::Vertex vert : objectVerts)
	{
		temp = vert.position.x;
		if (temp < this->xMin)
		{
			this->xMin = temp * meshScalingValue;
		}
		if (temp > this->xMax)
		{
			this->xMax = temp * meshScalingValue;
		}
		temp = vert.position.y;
		if (temp < this->yMin)
		{
			this->yMin = temp * meshScalingValue;
		}
		if (temp > this->yMax)
		{
			this->yMax = temp * meshScalingValue;
		}
		temp = vert.position.z;
		if (temp < this->zMin)
		{
			this->zMin = temp * meshScalingValue;
		}
		if (temp > this->zMax)
		{
			this->zMax = temp * meshScalingValue;
		}
	}
}

void CollisionDetectionComponent::buildCollisionFrame(const glm::vec3 dimensions)
{
	//bottom - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMin), glm::vec4(0, 1, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMax), glm::vec4(0, 0, 1, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));

	//top - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMin), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMin), glm::vec4(0, 1, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMax), glm::vec4(0, 1, 1, 1)));

	//left - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMin), glm::vec4(0, 1, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));

	//right - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMin), glm::vec4(1, 1, 1, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));

	//back - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMin), glm::vec4(0, 1, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMin), glm::vec4(0, 0, 1, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMin), glm::vec4(0, 1, 1, 1)));

	//front - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMax), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMax), glm::vec4(0, 1, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));
}
