#ifdef _WIN32
#pragma once
#endif

#ifndef SMALL_RET(obj)
#define SMALL_RET(obj) { return obj; }
#endif

#ifndef SMALL_SET(tgt, val)
#define SMALL_SET(tgt, val) { tgt = val; }
#endif

#ifndef SMALL_INST(line)
#define SMALL_INST(line) { line; }
#endif

#include "GameObject.h"

#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <string>
#include <type_traits>

#include "IInitializableComponent.h"
#include "IUpdatableComponent.h"
#include "IDrawableComponent.h"

class GameObject;

class Component
	: public IInitializableComponent, public IUpdatableComponent, public IDrawableComponent
{
protected:
	GameObject* m_Transform;

public:
	Component()
		: Component("DefaultComponent")
	{}

	Component(const Component& a_Component)
		: m_ComponentName(a_Component.m_ComponentName)
	{}

	Component(std::string a_ComponentName)
		: m_ComponentName(a_ComponentName)
	{}

	~Component()
	{
		m_Transform = nullptr;
		m_ComponentName.clear();
	}

protected:
	std::string m_ComponentName;

public:
	// Inherited via IInitializableComponent
	virtual void Initialize(void*) abstract;

	// Inherited via IUpdatableComponent
	virtual void Update(float a_DeltaTime) abstract;

	// Inherited via IDrawableComponent
	virtual void Draw() abstract;

	template <typename T>
	static T* Instantiate()
		SMALL_RET(Component::Instantiate<T>(nullptr))

	template <typename T>
	static T* Instantiate(void* a_Params)
	{
		static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
		Component* base = (Component*)new T();

		if (a_Params != nullptr)
			base->Initialize(a_Params);

		return (T*)base;
	}

public:
	inline void			SetComponentName(std::string a_ComponentName) SMALL_SET(m_ComponentName, a_ComponentName)
	inline std::string	GetComponentName()							  SMALL_RET(m_ComponentName)

	inline void			SetTransform	(GameObject* a_GameObject)	SMALL_SET(m_Transform, a_GameObject)
	inline GameObject*	GetTransform()								SMALL_RET(m_Transform)
};

#endif