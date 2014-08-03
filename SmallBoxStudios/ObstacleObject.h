#ifndef OBSTACLEOBJECT_H 
#define OBSTACLEOBJECT_H

#include "PhysicsObject.h"
#include "Camera.h"
#include "Timer.h"
#include "CellShaderObject.h"

class ObstacleObject : public PhysicsObject
{
public:
	ObstacleObject(string filename, float mass);
	~ObstacleObject();

	virtual bool LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
		float yaw, float pitch, float roll, float scale);

	void UnloadContent();
	virtual void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	virtual void Update(float dt);

	bool GetAlive();
	void SetAlive(bool isAlive);
	string m_filename;
	XMFLOAT4 Colour;

	//CellShading
	CellShaderObject	m_cellShader;
};

#endif 