#include "CollisionController.h"

CollisionController::CollisionController()
{
	p_playerC = NULL;
	p_powerUpC = NULL;
	p_terainC = NULL;
	p_dynamicsWorld = NULL;
}

CollisionController::~CollisionController()
{

}

void CollisionController::LoadHandlers(PlayerController &_playerController, PowerUpController &_powerUpController, TerrainController &_terraiController, btDynamicsWorld &_dynamicsWorld)
{
	p_playerC = &_playerController;
	p_powerUpC = &_powerUpController;
	p_terainC = &_terraiController;
	p_dynamicsWorld = &_dynamicsWorld;
}

void CollisionController::Update(float dt)
{
	//PLAYER PLAYER COLLISION
	for each (Player *playerX in p_playerC->GetPlayerList())
	{
		for each (Player *playerY in p_playerC->GetPlayerList())
		{
			if ((playerX != playerY) && playerY->GetAlive() && playerX->GetAlive())
			{
				p_dynamicsWorld->contactPairTest(playerX->GetCar()->GetRigidBody(),
					playerY->GetCar()->GetRigidBody(), PlayerPlayerCollisionHandler);
			}
		}
	}

	//PLAYER POERUP COLLISION
	for each(PowerUp* powerUpX in p_powerUpC->GetPowerUpList())
	{
		///#### IF POWERUP IS ALIVE
		if (powerUpX->GetAlive())
		{
			//Check if player intersects a powerup
			for each(Player* playerX in p_playerC->GetPlayerList())
			{
				if (playerX->GetAlive())
				{
					p_dynamicsWorld->contactPairTest(playerX->GetCar()->GetRigidBody(), powerUpX->m_rigidBody,
						 playerPowerUpCollisionCallback);
				}
			}

		}

		//PLAYER TERRAIN COLLISION
		btRigidBody* body = p_terainC->GetTerrain()->GetRigidBody();
		for each (Player *playerX in p_playerC->GetPlayerList())
		{
			p_dynamicsWorld->contactPairTest(playerX->GetCar()->GetRigidBody(),
				body, PlayerTerrainCollisionHandler);
		}
	}
}