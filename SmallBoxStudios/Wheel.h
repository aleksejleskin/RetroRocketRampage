#ifndef WHEEL_H
#define WHEEL_H

#include "PhysicsObject.h"
#include "CellShaderObject.h"

class Wheel : public PhysicsObject
{
public:
	Wheel();
	Wheel(string filename, float mass, XMFLOAT3 offset,
		float yaw, float pitch, float roll, float scale);
	~Wheel();

	bool LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
		float yaw, float pitch, float roll, float scale);
	void UnloadContent();

	void Update(float dt, btTransform & parentTransform);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);

	XMFLOAT3 GetOffset();

private:
	XMFLOAT3 m_offset;

	CellShaderObject m_cellShaderRenderer;
};

#endif