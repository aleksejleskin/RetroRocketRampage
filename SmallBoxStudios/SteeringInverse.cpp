#include "SteeringInverse.h"

SteeringInverse::SteeringInverse(string filename, XMFLOAT4 _colour) : PowerUp(filename, _colour)
{
	test = false;
	//m_powerUpId = 1;
	m_currentObjectTime = 0;
	m_timer = 0;
}

SteeringInverse::~SteeringInverse()
{

}

void SteeringInverse::Update(float dt)
{
	if (m_isAlive)
	{
		//time of object being alive
		m_currentObjectTime += dt;
		//sin(0->1,22)(0->-2.44)
		//0.5 initial position that will be changed depending on sin;


		btTransform tran = m_rigidBody->getCenterOfMassTransform();
		tran.setOrigin(btVector3(36.0f, 0.5f * sinf(4.0f * m_currentObjectTime), 50.0f));
		//m_rigidBody->setCenterOfMassTransform(tran);

	}

	Colour = XMFLOAT4(0, 255, 0, 0);
	if (m_activate && (m_currentPlayer != NULL))
	{
		m_timer += dt;
		//For each player that is not the one with the powerUp Reverse Steering
		for each(Player* player in ptr_playerController->GetPlayerList())
		{
			if (player != m_currentPlayer && !player->GetShieldStatus())
			{
				player->SetReverseSteering(true);
			}
		}
		if (m_timer > 5)
		{
			//Reset to the original steering.
			for each(Player* player in ptr_playerController->GetPlayerList())
			{
				if (player != m_currentPlayer)
				{
					player->SetReverseSteering(false);
				}
			}

			m_activate = false;
			m_isAlive = false;
			m_currentPlayer = NULL;
			m_timer = 0;
		}

	}
	PowerUp::Update(dt);
}

void SteeringInverse::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	PowerUp::Render(dx, cam, lightManager);
}

void SteeringInverse::Deactivate()
{
	for each(Player* player in ptr_playerController->GetPlayerList())
	{
	if (player != m_currentPlayer)
		{
			player->SetReverseSteering(false);
		}
	}

	m_activate = false;
	m_isAlive = false;
	m_currentPlayer = NULL;
	m_timer = 0;
}



