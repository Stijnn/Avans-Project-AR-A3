#ifdef _WIN32
#pragma once
#endif

#include "Component.h"

#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include <list>
#include <glm/vec3.hpp>
#include <string>

class Component;

class GameObject
{
private:
	GameObject() = default;
	GameObject(const GameObject& a_GameObject) :
		m_Name		(a_GameObject.m_Name),
		m_Children	(a_GameObject.m_Children),
		m_Parent	(a_GameObject.m_Parent),
		m_Position	(a_GameObject.m_Position),
		m_Rotation	(a_GameObject.m_Rotation),
		m_Scale		(a_GameObject.m_Scale)
	{};

public:
	~GameObject()
	{
		for (Component* c : m_Components)
			delete c;
		for (GameObject* c : m_Children)
			delete c;
	}

public:
	static GameObject* Instantiate(std::string a_Name, GameObject* a_Parent = nullptr)
	{
		GameObject* newObject = new GameObject();
		newObject->m_Name = a_Name;

		if (a_Parent != nullptr)
		{
			newObject->m_Parent = a_Parent;
			a_Parent->m_Children.push_back(newObject);
		}

		newObject->SetScale(glm::vec3(1, 1, 1));

		return newObject;
	}

	static GameObject* Instantiate(const std::list<Component*>& a_Components, std::string a_Name = "", GameObject* a_Parent = nullptr)
	{
		GameObject* newObject = GameObject::Instantiate(a_Name, a_Parent);
		newObject->m_Components = a_Components;
		return newObject;
	}


public:
	void Update				(float a_DeltaTime);
	void Draw				();

	template<typename T>
	T* GetComponent();

protected:
	std::string				m_Name;
	std::list<GameObject*>	m_Children;
	std::list<Component*>	m_Components;
	GameObject*				m_Parent;

	glm::vec3				m_Rotation;
	glm::vec3				m_Position;
	glm::vec3				m_Scale;

protected:
	//virtual void InternalUpdate	(float a_DeltaTime);
	//virtual void InternalDraw	();

public:
	void AddComponent		(Component* a_Component);
	void RemoveComponent	(Component* a_Component);

	void AddChild		(GameObject* a_Child);
	void RemoveChild	(GameObject* a_Child);

	GameObject* operator[](std::string name)
	{
		for (GameObject* go : m_Children)
			if (strcmp(go->GetName().c_str(), name.c_str()) == 0)
				return go;
		return nullptr;
	}

public:
	inline std::string	GetName		()	{ return m_Name; }
	inline GameObject*	GetParent	()	{ return m_Parent != nullptr ? m_Parent : nullptr; }

	inline const glm::vec3&	GetPosition	()	{ return m_Position; }
	inline const glm::vec3&	GetRotation	()	{ return m_Rotation; }
	inline const glm::vec3& GetScale	()	{ return m_Scale; }

	inline const std::list<	GameObject*	>&		GetChildren		()	const { return m_Children; }
	inline const std::list<	Component*	>&		GetComponents	()	const { return m_Components; }

	inline void SetParent(GameObject* a_Parent) 
		SMALL_SET(m_Parent, a_Parent)

	inline void SetRotation(const glm::vec3& newRotation) 
		SMALL_SET(m_Rotation, newRotation)

	inline void SetPosition(const glm::vec3& newPosition)
		SMALL_SET(m_Position, newPosition)

	inline void SetScale(const glm::vec3& newScale)
		SMALL_SET(m_Scale, newScale)
};

template<typename T>
inline T* GameObject::GetComponent()
{
	for (auto& component : m_Components)
		if (typeid(component) == typeid(T))
			return (T*)component;

	return nullptr;
}

#endif