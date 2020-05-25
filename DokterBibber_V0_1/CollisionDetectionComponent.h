#include "Component.h"
#include <vector>
#include "ObjModel.h"

#pragma once
class CollisionDetectionComponent : Component
{
public:
	CollisionDetectionComponent() : 
		Component("CollisionDetectionComponent") 
	{}

	virtual void Initialize(void* a_Parameter)	override;
	virtual void Update(float a_DeltaTime)		override;
	virtual void Draw(float a_DeltaTime)		override;

	bool checkComponentCollision(const CollisionDetectionComponent& main_object, const CollisionDetectionComponent& second_object);
	void initializeCollisionFrame(const std::vector<tigl::Vertex>& objectVerts);
	std::vector<tigl::Vertex> getMeshVertexVector();
	void setMeshScalingValue(const float scale);

private:	
	void showCollisionFrame(bool showFrame);
	float calculateExtrudeModifier(const float& x, const float& y, const float& z);
	void calculateMinMax(const std::vector<tigl::Vertex>& objectVerts);
	void buildCollisionFrame(const glm::vec3 dimensions);

private:
	std::vector<tigl::Vertex> collisionVerts; // collisionVerts
	std::vector<tigl::Vertex> meshVerts; // mesh components vertex

	bool debugWireFrames;
	const int extruderDivisionScaler = 3; // scaler for the collisionFrame
	float xMin;
	float xMax;
	float yMin;
	float yMax;
	float zMin;
	float zMax;

public:
	float xLength;
	float yLength;
	float zLength;

	float meshScalingValue = 1.0f;

	// mesh component variables
protected:
	float m_Scale = 1.0f;
	glm::vec3 m_Position;

private:
	std::vector<tigl::Vertex> m_Vertices;
	ObjModel* m_ObjectModel;

public:
	inline float		GetScale() { return m_Scale; }
	inline void			SetScale(float a_Scale) { m_Scale = a_Scale; }
	inline glm::vec3	GetPosition() { return m_Position; }
	inline void			SetPosition(glm::vec3 a_Position) { m_Position = a_Position; }
};

