#ifdef _WIN32
#pragma once
#endif

#ifndef _MESH_COMPONENT_H
#define _MESH_COMPONENT_H

#include "Component.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ObjModel.h"
#include "tigl.h"

class MeshComponent 
	: Component
{
public:
	MeshComponent()
		: Component("MeshComponent")
	{}

	// Inherited via Component
	virtual void Initialize	(void*)				override;
	virtual void Update		(float a_DeltaTime) override;
	virtual void Draw		(float a_DeltaTime) override;

protected:
	float m_Scale = 1.0f;
	
private:
	std::vector<tigl::Vertex> m_Vertices;
	ObjModel* m_ObjectModel;

public:
	inline float	GetScale ()					{ return m_Scale; }
	inline void		SetScale (float a_Scale)	{ m_Scale = a_Scale; }
};

#endif