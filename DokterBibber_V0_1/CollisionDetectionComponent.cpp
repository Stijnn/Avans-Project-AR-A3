#include "CollisionDetectionComponent.h"
#include <algorithm>
#include <glm\ext\matrix_transform.hpp>
#include "MeshComponent.h"
#include <iostream>

void CollisionDetectionComponent::Initialize(void* a_Parameter) 
{

}

void CollisionDetectionComponent::Update(float a_DeltaTime)
{
	for (auto objectChild : m_Transform->GetParent()->GetChildren())
	{
		for (auto componentChild : objectChild->GetComponents())
		{
			CollisionDetectionComponent* colComp = componentChild->GetTransform()->GetComponent<CollisionDetectionComponent>();
			if (colComp != nullptr && colComp != this && colComp->GetTransform()->GetName() != "Pincet") // makes sure to filter and catch any occurances with the pincet mesh
			{
				// for debugging				
				float xMinGO = this->GetTransform()->GetPosition().x - (this->xLength / 2);
				float xMaxSO = colComp->GetTransform()->GetPosition().x + (colComp->xLength / 2);
				float xMaxGO = this->GetTransform()->GetPosition().x + (this->xLength / 2);
				float xMinSO = colComp->GetTransform()->GetPosition().x - (colComp->xLength / 2);

 				float yMinGO = this->GetTransform()->GetPosition().y - (this->yLength / 2);
				float yMaxSO = colComp->GetTransform()->GetPosition().y + (colComp->yLength / 2);
				float yMaxGO = this->GetTransform()->GetPosition().y + (this->yLength / 2);
				float yMinSO = colComp->GetTransform()->GetPosition().y - (colComp->yLength / 2);

				float zMinGO = this->GetTransform()->GetPosition().z - (this->zLength / 2);
				float zMaxSO = colComp->GetTransform()->GetPosition().z + (colComp->zLength / 2);
				float zMaxGO = this->GetTransform()->GetPosition().z + (this->zLength / 2);
				float zMinSO = colComp->GetTransform()->GetPosition().z - (colComp->zLength / 2);

				// 2nd method 
				/*float xMinGO = this->GetTransform()->GetPosition().x - (this->xMin);
				float xMaxSO = colComp->GetTransform()->GetPosition().x + (colComp->xMax);
				float xMaxGO = this->GetTransform()->GetPosition().x + (this->xMax);
				float xMinSO = colComp->GetTransform()->GetPosition().x - (colComp->xMin);

				float yMinGO = this->GetTransform()->GetPosition().y - (this->yMin);
				float yMaxSO = colComp->GetTransform()->GetPosition().y + (colComp->yMax);
				float yMaxGO = this->GetTransform()->GetPosition().y + (this->yMax);
				float yMinSO = colComp->GetTransform()->GetPosition().y - (colComp->yMin);

				float zMinGO = this->GetTransform()->GetPosition().z - (this->zMin);
				float zMaxSO = colComp->GetTransform()->GetPosition().z + (colComp->zMax);
				float zMaxGO = this->GetTransform()->GetPosition().z + (this->zMax);
				float zMinSO = colComp->GetTransform()->GetPosition().z - (colComp->zMin);*/

				// collision logic
				bool collisionX = xMinGO <= xMaxSO &&
					xMaxGO >= xMinSO;

				bool collisionY = yMinGO <= yMaxSO &&
					yMaxGO >= yMinSO;

				bool collisionZ = zMinGO <= zMaxSO &&
					zMaxGO >= zMinSO;

				// bool to check if sides x, y and z have collided
				bool CollisionOccured = collisionX && collisionZ && collisionY; // if there is collision on the object detect correctly
				if (CollisionOccured)
				{
					// debugging message for collision occurance
					std::cout << "Collision Occured! with: " << colComp->GetTransform()->GetName() << std::endl;

					
				}
			}
		}			
	}
}


void CollisionDetectionComponent::Draw()
{
	glm::mat4 modelMatrix(1);
	modelMatrix = glm::translate(modelMatrix, m_Transform->GetPosition());
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Transform->GetRotation()[0]), glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Transform->GetRotation()[1]), glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Transform->GetRotation()[2]), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Transform->GetScale()[0], m_Transform->GetScale()[1], m_Transform->GetScale()[2]));

	tigl::shader->setModelMatrix(modelMatrix);

	// DEBUG
	tigl::shader->enableTexture(false);
	tigl::shader->enableColor(true);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	tigl::drawVertices(GL_QUADS, this->collisionVerts);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	tigl::shader->enableTexture(true);
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

	this->xLength = (this->xMax - this->xMin);
	this->yLength = (this->yMax - this->yMin);
	this->zLength = (this->zMax - this->zMin);

	buildCollisionFrame();
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
			this->xMin = temp;
		}
		if (temp > this->xMax)
		{
			this->xMax = temp;
		}
		temp = vert.position.y;
		if (temp < this->yMin)
		{
			this->yMin = temp;
		}
		if (temp > this->yMax)
		{
			this->yMax = temp;
		}
		temp = vert.position.z;
		if (temp < this->zMin)
		{
			this->zMin = temp;
		}
		if (temp > this->zMax)
		{
			this->zMax = temp;
		}
	}
}

void CollisionDetectionComponent::buildCollisionFrame()
{
	////bottom - wireframe
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMin), glm::vec4(0, 1, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMax), glm::vec4(0, 0, 1, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));

	////top - wireframe
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMin), glm::vec4(1, 0, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMin), glm::vec4(0, 1, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMax), glm::vec4(0, 1, 1, 1)));

	////left - wireframe
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMin), glm::vec4(0, 1, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));

	////right - wireframe
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMin), glm::vec4(1, 1, 1, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));

	////back - wireframe
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMin), glm::vec4(1, 0, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMin), glm::vec4(0, 1, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMin), glm::vec4(0, 0, 1, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMin), glm::vec4(0, 1, 1, 1)));

	////front - wireframe
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMin, this->zMax), glm::vec4(1, 0, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMin, this->yMax, this->zMax), glm::vec4(0, 1, 0, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMax, this->zMax), glm::vec4(0, 0, 1, 1)));
	//this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xMax, this->yMin, this->zMax), glm::vec4(0, 1, 1, 1)));

	// REAL Wire Frame - used to check on the real wireframe check-up in the check on collision method
	//bottom - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));

	//top - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));

	//left - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));

	//right - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));

	//back - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, -this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));

	//front - 'REAL' wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, this->zLength / 2) + posOffSet, glm::vec4(1, 0, 0, 1)));
}

void CollisionDetectionComponent::buildPincetCollisionFrame(const glm::vec3 dimensions)
{
	this->xLength = dimensions.x;
	this->yLength = dimensions.y;
	this->zLength = dimensions.z;

	//bottom - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));

	//top - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));

	//left - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));

	//right - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));

	//back - wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, -this->zLength / 2), glm::vec4(1, 0, 0, 1)));

	//front - 'REAL' wireframe
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, -this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(-this->xLength / 2, this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
	this->collisionVerts.push_back(tigl::Vertex::PC(glm::vec3(this->xLength / 2, -this->yLength / 2, this->zLength / 2), glm::vec4(1, 0, 0, 1)));
}

