#ifndef BOOST_H
#define BOOST_H

#include "PowerUp.h"



class Boost : public PowerUp
{
public:
	Boost(string filename, XMFLOAT4 _colour);
	~Boost();

	//void UnloadContent();
	void Update(float dt);
	bool LoadContent(ResourceManager& resource);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Deactivate();
private:
	bool m_shutdown;
	//int m_powerUpId;
	string m_filename;

	//TEST
	float m_currentObjectTime;
};

#endif 