#include "Component.h"
#include <vector>
#include "ObjModel.h"

#pragma once
class CollisionDetectionComponent 
	: public Component
{
public:
	CollisionDetectionComponent() : 
		Component("CollisionDetectionComponent")
	{}

	virtual void Initialize(void* a_Parameter)	override;
	virtual void Update(float a_DeltaTime)		override;
	virtual void Draw()							override;

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


private:
	std::vector<tigl::Vertex> m_Vertices;
	float meshScalingValue = 1.0f;
	ObjModel* m_ObjectModel;
};

