#include "Steal.h"

Steal::Steal(string filename, XMFLOAT4 _colour) : PowerUp(filename, _colour)
{
	//m_powerUpId = 1;
	m_currentObjectTime = 0;
	m_timer = 0;
	Linear = btVector3(0, 0, 0);
}

Steal::~Steal()
{

}

void Steal::Update(float dt)
{
		if (m_activate && (m_currentPlayer != NULL))
		{
			m_timer += dt;
			float LinearZ = m_currentPlayer->GetCar()->GetRigidBody()->getLinearVelocity().getZ();
			if (m_timer <0.2)
			{
				m_currentPlayer->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 10, LinearZ));
			}
			if (m_timer>0.2 && m_timer <0.5)
			{
				m_currentPlayer->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, LinearZ + 10));
			}
			if (m_timer > 0.5)
			{
				m_currentPlayer->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, Linear.getZ() + 2));
				m_activate = false;
				m_isAlive = false;
				m_currentPlayer = NULL;
				m_timer = 0;
			}
		}	
		if (m_currentPlayer != NULL && !m_activate)
	{
		Linear = m_currentPlayer->GetCar()->GetRigidBody()->getLinearVelocity();
	}
	PowerUp::Update(dt);
}

void Steal::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	PowerUp::Render(dx, cam, lightManager);
}

void Steal::Deactivate()
{
	m_activate = false;
	m_isAlive = false;
	m_currentPlayer = NULL;
	m_timer = 0;
}


