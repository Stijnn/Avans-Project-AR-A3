#include "MeshComponent.h"

void MeshComponent::Initialize(void* a_Parameters)
{
	this->m_Vertices = *((std::vector<tigl::Vertex>*)a_Parameters);
}

void MeshComponent::Update(float a_DeltaTime)
{

}

void MeshComponent::Draw(float a_DeltaTime)
{
	glm::mat4 modelMatrix(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0,0,0));

	tigl::shader->setModelMatrix(modelMatrix);

	tigl::begin(GL_TRIANGLES);
	for (auto& vert : m_Vertices)
		tigl::addVertex(vert);
	tigl::end();
	//m_ObjectModel->draw();
}
