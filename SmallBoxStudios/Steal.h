#ifndef STEAL_H
#define STEAL_H

#include "PowerUp.h"

class Steal : public PowerUp
{
public:
	Steal(string filename, XMFLOAT4 _colour);
	~Steal();

	//void UnloadContent();
	void Update(float dt);
	bool LoadContent(ResourceManager& resource);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Deactivate();
private:
	bool m_shutdown;
//	int m_powerUpId;
	string m_filename;
	//TEST
	float m_currentObjectTime;
	btVector3 Linear;
};

#endif 