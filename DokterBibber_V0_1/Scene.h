#ifdef _WIN32
#pragma once
#endif

#ifndef _SCENE_H
#define _SCENE_H

#include "GameObject.h"

class Scene
{
public:
	Scene()
	{
		m_RootObject = GameObject::Instantiate("Root");
	}

	Scene(const Scene& a_Scene)
		: m_RootObject(a_Scene.m_RootObject)
	{}

	~Scene()
	{
		delete m_RootObject;
	}

private:
	GameObject* m_RootObject;

public:
	inline GameObject* GetRootObject() { return m_RootObject; }
};

#endif