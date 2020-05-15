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
	modelMatrix = glm::rotate(modelMatrix, glm::radians(180.f), glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Scale, m_Scale, m_Scale));

	tigl::shader->setModelMatrix(modelMatrix);

	tigl::begin(GL_TRIANGLES);
	for (auto& vert : m_Vertices)
		tigl::addVertex(vert);
	tigl::end();
}
