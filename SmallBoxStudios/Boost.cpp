#include "Boost.h"

Boost::Boost(string filename, XMFLOAT4 _colour) : PowerUp(filename, _colour)
{

//	m_powerUpId = 1;
	m_currentObjectTime = 0;
	Colour = _colour;
	m_timer = 0;
}

Boost::~Boost()
{

}

void Boost::Update(float dt)
{

	if (m_activate && (m_currentPlayer != NULL))
	{
		m_timer += dt;
	//	m_currentPlayer->GetCar()->SetMaxEngineForce(3000);
		m_currentPlayer->SetSpeedLimitActive(false);
		if (m_timer > 5)
		{
			m_currentPlayer->SetSpeedLimitActive(true);
		//	m_currentPlayer->GetCar()->SetMaxEngineForce(1500);
			m_activate = false;
			m_currentPlayer = NULL;
			m_timer = 0;
		}

	}
	PowerUp::Update(dt);
}

void Boost::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	PowerUp::Render(dx, cam, lightManager);
}

void Boost::Deactivate()
{
	m_currentPlayer->SetSpeedLimitActive(true);
//	m_currentPlayer->GetCar()->SetMaxEngineForce(1500);
	m_activate = false;
	m_isAlive = false;
	m_currentPlayer = NULL;
	m_timer = 0;
}



