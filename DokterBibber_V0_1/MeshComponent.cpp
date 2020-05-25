#include "MeshComponent.h"

void MeshComponent::Initialize(void* a_Parameters)
{
	this->m_Vertices = *((std::vector<tigl::Vertex>*)a_Parameters);
}

void MeshComponent::Update(float a_DeltaTime)
{

}

void MeshComponent::Draw()
{
	glm::mat4 modelMatrix(1);
	modelMatrix = glm::translate(modelMatrix, m_Transform->GetPosition());
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Transform->GetRotation()[0]), glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Transform->GetRotation()[1]), glm::vec3(0, 1, 0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(m_Transform->GetRotation()[2]), glm::vec3(0, 0, 1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_Transform->GetScale()[0], m_Transform->GetScale()[1], m_Transform->GetScale()[2]));

	tigl::shader->setModelMatrix(modelMatrix);

	tigl::begin(GL_TRIANGLES);
	for (auto& vert : m_Vertices)
		tigl::addVertex(vert);
	tigl::end();
}
