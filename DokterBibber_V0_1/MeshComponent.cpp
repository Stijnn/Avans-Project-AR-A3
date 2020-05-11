#include "MeshComponent.h"

void MeshComponent::Initialize(void*)
{

}

void MeshComponent::Update(float a_DeltaTime)
{

}

void MeshComponent::Draw(float a_DeltaTime)
{
	tigl::begin(GL_TRIANGLES);

	for (size_t i = 0; i < m_Vertices.size(); i++)
		tigl::addVertex(m_Vertices[i]);

	tigl::end();
}
