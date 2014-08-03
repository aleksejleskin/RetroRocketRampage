#ifndef SCALE_H
#define SCALE_H


#include "PowerUp.h"


class Scale : public PowerUp
{
public:
	Scale(string filename, XMFLOAT4 _colour);
	~Scale();

	//void UnloadContent();
	void Update(float dt);
	void Render(DxGraphics* dx, Camera& cam);
	btRigidBody* GetRigidBody();
private:
	bool m_shutdown;
	int m_powerUpId;
	string m_filename;

	float m_scaleMultiplier;

	bool test;
	//TEST
	float m_currentObjectTime;
};

#endif 