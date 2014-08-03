#include "PowerUpController.h"

PowerUpController::PowerUpController()
{
	m_powerUpEnable = false;
}

PowerUpController::~PowerUpController()
{
	for each(PowerUp * obj in m_powerUpList)
	{
		obj->UnloadContent();
		delete obj;
	}
}

void PowerUpController::Shutdown()
{
	for (unsigned int x = 0; x < (unsigned)m_powerUpList.size(); x++)
	{
		m_powerUpList[x]->UnloadContent();
	}
}

bool PowerUpController::Initialise(DxGraphics *dx, ResourceManager& resources, PlayerController *playerController, TerrainController &_terrainController)
{
	m_dynamicsWorld = resources.GetPhysicsManager()->GetWorld();

	m_powerUpList.push_back(new Shield("Shield.sbs", XMFLOAT4(255, 0, 0, 0)));
	m_powerUpList.push_back(new Obstacle("Obstacle.sbs", XMFLOAT4(255, 0, 0, 0)));
	m_powerUpList.push_back(new Boost("Boost.sbs", XMFLOAT4(0, 0, 255, 0)));

	m_powerUpList.push_back(new Push("PushField.sbs", XMFLOAT4(0, 50, 50, 0)));
	m_powerUpList.push_back(new Push("PushField.sbs", XMFLOAT4(0, 50, 50, 0)));
	m_powerUpList.push_back(new Steal("Steal.sbs", XMFLOAT4(0, 0, 50, 0)));
	m_powerUpList.push_back(new SteeringInverse("Inverse.sbs", XMFLOAT4(50, 0, 50, 0)));

	for (unsigned int x = 0; x < m_powerUpList.size(); x++)
	{
		//Initialize All PowerUps.
		if (!m_powerUpList[x]->LoadContent(dx,
			XMFLOAT3(50.0f * x, -2.0f, 100.0f),	//TEMPORARY Position
			resources, //resources
			0.0f,	//Yaw
			0.78f,	//Pitch
			0.78f,	//Roll
			1.0f))	//Scale
		{
			return false;
		}
		GenerateRandomRespawnTime(*m_powerUpList[x]); //time untio powerup respawns
	}
	void *obj;
	obj = m_powerUpList[0];
	((Shield *)obj)->GetShield().LoadContent(dx, XMFLOAT3(20, 4, 50), resources, 0, 0, 0, 4);
	obj = m_powerUpList[1];
	((Obstacle *)obj)->GetObstacle().LoadContent(dx, XMFLOAT3(20, 4, 50), resources, 0, 0, 0, 1);

	p_playerC = playerController;
	p_terrainC = &_terrainController;
	//Set up pointers for powerups
	for (unsigned int x = 0; x < m_powerUpList.size(); x++)
	{
		m_powerUpList[x]->SetPlayerControllerPointer(playerController);
		m_powerUpList[x]->m_rigidBody->setUserPointer(m_powerUpList[x]);
	}

	return true;
}

void PowerUpController::Update(float dt, HelicopterCamera& camera)
{

	//Variables used for powerUp Respawns
	m_playersAvgPos = p_playerC->GetAvgPlayerPosition();
	m_leaderPos = p_playerC->GetPlayer(p_playerC->GetRaceLeaderId())->GetCar()->GetPosition();


	for each(PowerUp* powerUpX in m_powerUpList)
	{
		///#### IF POWERUP IS ALIVE
		if (powerUpX->GetAlive())
		{

			//Set powerup alive to false if powerup is behind the camera
			btVector3 pos = powerUpX->m_rigidBody->getCenterOfMassTransform().getOrigin();
			if (pos.getZ() < camera.GetPosition().z - 65) //offset some powerups require
			{
				powerUpX->SetAlive(false);
			}
		}

		if (m_powerUpEnable)
		{
#pragma region POWERUP_RESPAWN
			///#### IF POWERUP IS DEAD
			if (!powerUpX->GetAlive() && powerUpX->GetPlayerPtr() == NULL)
			{
				//variable is used to access timers
				float timeUntilRespawn = powerUpX->GetRespawnTime();
				timeUntilRespawn -= dt;
				powerUpX->SetRespawnTime(timeUntilRespawn);

				if (timeUntilRespawn < 0)
				{
					//new powerupPosition
					XMFLOAT3 newPosition;

					//Z
					newPosition.z = m_leaderPos.z;
					float randomDistance = (float)(rand() % 500 + 100);
					float randomFlux = (float)(rand() % 50 - 25);
					newPosition.z += randomDistance + randomFlux;

					//X

					XMFLOAT3 terrainPosition = p_terrainC->GetTerrain()->GetPosition();
					vector<vector<int>> terrainRestrict = p_terrainC->GetTerrain()->GetTerrainRestrictions();
					float terrainScale = terrainRestrict[0][3];
					int powerUpChunkNumber = (int)(((newPosition.z / (float)terrainScale) / 32.0f));
					float terrainMinX = (float)(((terrainRestrict[powerUpChunkNumber][0] + 3) * terrainScale) + terrainPosition.x);
					float terrainMaxX = (float)(((terrainRestrict[powerUpChunkNumber][1] - 3) * terrainScale) + terrainPosition.x);
					float terrainClearWidth = terrainMaxX - terrainMinX;

					float randomXCoord = (float)(rand() % (int)(terrainMaxX - terrainMinX) + (int)terrainMinX);
					newPosition.x = randomXCoord;

					//Y
					newPosition.y = 0.3;
					powerUpX->SetPosition(newPosition);
					powerUpX->SetAlive(true);
					GenerateRandomRespawnTime(*powerUpX);
				}
				//update timer for a specific powerUp

				powerUpX->Update(dt);
			}
#pragma endregion
		}
		else
		{
			if (powerUpX->GetAlive())
			{
				powerUpX->SetAlive(false);
			}
		}

	}

	for each(PowerUp* powerUpX in m_powerUpList)
	{
		powerUpX->Update(dt);
	}
}

void PowerUpController::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	for each(PowerUp* powerUpX in m_powerUpList)
	{
		if (powerUpX->GetAlive()) //render only if alive
		{
			powerUpX->Render(dx, cam, lightManager);
		}
	}
}

///GET SET
vector <PowerUp *>& PowerUpController::GetPowerUpList()
{
	return m_powerUpList;
}

PowerUp* PowerUpController::GetPowerUp(unsigned int id)
{
	return m_powerUpList[id];
}

void PowerUpController::GenerateRandomRespawnTime(PowerUp &powerUpId)
{
	powerUpId.SetRespawnTime((float)(rand() % 6 + 1));
}

void PowerUpController::PowerUpControllerSwitch(bool _enable)
{
	m_powerUpEnable = _enable;
}


