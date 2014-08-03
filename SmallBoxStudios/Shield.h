#ifndef SHIELD_H
#define SHIELD_H

#include "PowerUp.h"
#include "ShieldObject.h"


class Shield : public PowerUp
{
public:
	Shield(string filename, XMFLOAT4 _colour);
	~Shield();

	//void UnloadContent();
	void Update(float dt);
	bool LoadContent(ResourceManager& resource);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Deactivate();
	ShieldObject& GetShield();
private:
	bool m_shutdown;
	int m_powerUpId;
	string m_filename;
	ShieldObject m_shieldObject;
	//TEST
	float m_currentObjectTime;
};

#endif 