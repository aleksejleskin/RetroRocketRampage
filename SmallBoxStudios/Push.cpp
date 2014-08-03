#include "Push.h"

Push::Push(string filename, XMFLOAT4 _colour) : PowerUp(filename, _colour)
{
	m_powerUpId = 1;
	m_currentObjectTime = 0;
	m_radius = 8;
}

Push::~Push()
{

}

void Push::Update(float dt)
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

				//how strong the force of push is.(multiplier)
				float mul = -5;
				//check if enemy players are within  the radius
				if ((player->GetVehiclePosition().z - player->GetVehiclePosition().z < m_radius)
					&&
					(player->GetVehiclePosition().z - player->GetVehiclePosition().z > -m_radius)
					&&
					((player->GetVehiclePosition().x - player->GetVehiclePosition().x) > -m_radius)
					&&
					((player->GetVehiclePosition().x - player->GetVehiclePosition().x) < m_radius))
				{
					//direction of the enemy player
					XMFLOAT3 dir = m_currentPlayer->GetVehiclePosition() - player->GetVehiclePosition();
					player->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(dir.x*mul, dir.y * mul, dir.z * mul));
					player->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(dir.x*mul, dir.y*mul, dir.z*mul));

				}
			}
		}
		
		//Remove the powerUp from the player , allow respawning.
			m_activate = false;
			m_isAlive = false;
			m_currentPlayer = NULL;
			m_timer = 0;
		

	}
	PowerUp::Update(dt);
}

void Push::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	PowerUp::Render(dx, cam, lightManager);
}

void Push::Deactivate()
{
	m_activate = false;
	m_isAlive = false;
	m_currentPlayer = NULL;
	m_timer = 0;
}


