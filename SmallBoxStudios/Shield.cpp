#include "Shield.h"

Shield::Shield(string filename, XMFLOAT4 _colour) : PowerUp(filename, _colour), m_shieldObject("Shield.sbs", 1)
{
	m_powerUpId = 1;
	m_currentObjectTime = 0;
	m_timer = 0;
}

Shield::~Shield()
{

}

void Shield::Update(float dt)
{

	if (m_activate && (m_currentPlayer != NULL))
	{
		m_timer += dt;
		m_shieldObject.GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
		XMFLOAT3 pos = m_currentPlayer->GetCar()->GetPosition();
		btTransform currentT;
		currentT = m_rigidBody->getWorldTransform();
		btVector3 currentPos = btVector3(pos.x, pos.y, pos.z);
		currentT.setOrigin(currentPos);
		m_shieldObject.GetRigidBody()->setWorldTransform(currentT);

		m_currentPlayer->SetShieldStatus(true);
		if (m_timer > 15)
		{
			m_timer = 0;
			m_activate = false;
			m_isAlive = false;
			m_currentPlayer->SetShieldStatus(false);
			m_currentPlayer = NULL;

		}
	}
	m_shieldObject.Update(dt);
	PowerUp::Update(dt);
}

void Shield::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	m_shieldObject.Render(dx, cam, lightManager);
	PowerUp::Render(dx, cam, lightManager);
}

ShieldObject& Shield::GetShield()
{
	return m_shieldObject;
}

void Shield::Deactivate()
{
	if (m_currentPlayer != NULL)
	{
		m_timer = 0;
		m_activate = false;
		m_isAlive = false;
		m_currentPlayer->SetShieldStatus(false);
		m_currentPlayer = NULL;
	}
	}


