#ifdef _WIN32
#pragma once
#endif

#ifndef _COMPONENT_H
#define _COMPONENT_H

#include <string>
#include <type_traits>

#include "IInitializableComponent.h"
#include "IUpdatableComponent.h"
#include "IDrawableComponent.h"

class Component
	: public IInitializableComponent, public IUpdatableComponent, public IDrawableComponent
{
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
	virtual void Draw(float a_DeltaTime) abstract;

	template <typename T>
	static T* Instantiate()
	{
		return Component::Instantiate<T>(nullptr);
	}

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
	inline void			SetComponentName(std::string a_ComponentName)	{	m_ComponentName = a_ComponentName;	}
	inline std::string	GetComponentName()								{	return m_ComponentName;				}
};

#endif