#ifndef STEERINGINVERSE_H
#define STEERINGINVERSE_H

#include "PowerUp.h"



class SteeringInverse : public PowerUp
{
public:
	SteeringInverse(string filename, XMFLOAT4 _colour);
	~SteeringInverse();

	//void UnloadContent();
	void Update(float dt);
	bool LoadContent(ResourceManager& resource);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Deactivate();
private:
	bool m_shutdown;
	//int m_powerUpId;
	string m_filename;
	XMFLOAT3 m_force;

	bool test;
	//TEST
	float m_currentObjectTime;
};

#endif 