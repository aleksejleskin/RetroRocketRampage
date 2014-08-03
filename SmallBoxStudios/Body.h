#ifndef BODY_H
#define BODY_H

#include "PhysicsObject.h"
#include "CellShaderObject.h"

class Body : public PhysicsObject
{
public:
	Body();
	~Body();

	Body(vector<XMFLOAT3> anchorPoints, string filename,
		float mass,	float yaw, float pitch, float row, float scale);

	bool LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
		float yaw, float pitch, float roll, float scale);
	void UnloadContent();

	void Update(float dt, btTransform& Parent);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);

	XMFLOAT3 GetAnchorPoint(int point);

	float GetSteeringForce();
	float GetSuspensionResting();

private:

	//1 frontleftwheel, 2 frontrightwheel, 3 backleftWheel, 4 backrightwheel,
	//5 leftretro, 6 rightretro,
	//7 splitter, 8 spoiler
	vector<XMFLOAT3> m_anchorPoints; 
	CellShaderObject m_cellShadingRender;

};

#endif