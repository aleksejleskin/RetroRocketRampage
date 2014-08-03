#ifndef SPOILER_H
#define SPOILER_H

#include "PhysicsObject.h"
#include "CellShaderObject.h"

class Spoiler : public PhysicsObject
{
public:
	Spoiler();
	Spoiler(string filename, float mass,
		XMFLOAT3 offset, float yaw, float pitch, float roll, float scale);
	~Spoiler();

	bool LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
		float yaw, float pitch, float roll, float scale);
	void UnloadContent();

	void Update(float dt, btTransform& Parent);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);

	XMFLOAT3 GetOffset();

private:
	XMFLOAT3 m_offset;
	CellShaderObject m_cellShaderRenderer;
};

#endif