#include "GameObject.h"

void GameObject::Update(float a_DeltaTime)
{
	for (auto& comp : m_Components)
		comp->Update(a_DeltaTime);

	for (auto& child : m_Children)
		child->Update(a_DeltaTime);

	//InternalUpdate(a_DeltaTime);
}

void GameObject::Draw()
{
	for (auto& comp : m_Components)
		comp->Draw();

	for (auto& child : m_Children)
		child->Draw();

	//InternalDraw();
}

void GameObject::AddComponent(Component * a_Component)
{
	a_Component->SetTransform(this);
	m_Components.push_back(a_Component);
}

void GameObject::RemoveComponent(Component * a_Component)
{
	m_Components.remove(a_Component);
	delete a_Component;
}

void GameObject::AddChild(GameObject* a_Child)
{
	bool contains = false;
	for (GameObject* obj : m_Children)
	{
		if (obj == a_Child)
		{
			contains = true;
			break;
		}
	}

	if (!contains)
	{
		m_Children.push_back(a_Child);
		a_Child->SetParent(this);
	}
}

void GameObject::RemoveChild(GameObject* a_Child)
{
	m_Children.remove(a_Child);
	delete a_Child;
}

template<typename T>
T* GameObject::GetComponent()
{
	for (auto& component : m_Components)
		if (typeid(component) == typeid(T))
			return component;

	return nullptr;
}