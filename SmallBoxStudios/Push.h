#ifndef PUSH_H
#define PUSH_H

#include "PowerUp.h"

class Push : public PowerUp
{
public:
	Push(string filename, XMFLOAT4 _colour);
	~Push();

	//void UnloadContent();
	void Update(float dt);
	bool LoadContent(ResourceManager& resource);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Deactivate();
private:
	bool m_shutdown;
	int m_powerUpId;
	string m_filename;
	XMFLOAT3 m_force;

	bool test;
	//TEST
	float m_currentObjectTime;

	//Radius of action for the powerup
	float m_radius;
};

#endif 