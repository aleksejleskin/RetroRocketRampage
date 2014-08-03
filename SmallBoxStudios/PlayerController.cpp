#include "PlayerController.h"

PlayerController::PlayerController()// : m_playerPrevDead( false )
{
	m_playersAlive = 0;
	m_raceLeaderId = 0;
	m_numOfPlayers = 4;
	m_timeUntilPlayerRespawn = 0.5f;
	m_timeUntilRespawnInAir = 1.0f;
	m_draggingEngineForce = 2500.0f;
	m_distanceForDragging = 20;
	m_startingPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_avgPlayerPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_playerPrevDead = false;
	m_activePlayers = 0;
	m_betweenCarSpace = 4.0f;

	//reset the timers to zero for each player
	for (int x = 0; x < 4; x++)
	{
		m_currentRespawnTime[x] = 0.0f;
		m_playerDeaths[x] = 0;
		m_stuckTimer[x] = 0.0f;
	}

	timer = 0.0f;

	//Drag start //delay between each player drag line
	m_dragTimer[0] = 3.0f;
	m_dragTimer[1] = 6.0f;
	m_dragTimer[2] = 7.0f;
	m_dragTimer[3] = 4.0f;

	m_startTitle = false;
	//remove players that are behind camera.
	m_behindCamera = true;
}

PlayerController::~PlayerController()
{
	for each (Player* player in m_playerList)
	{
		player->~Player();
		delete player;
	}
}

bool PlayerController::Initialise(DxGraphics* dx, VehicleController& vehicles, ResourceManager& resource, XMFLOAT3 initCarPosition)
{
	//start position assigned form the func param.
	m_startingPosition = initCarPosition;

	//BulletPhysics dynamicWorld used to detect collision
	m_dynamicsWorld = resource.GetPhysicsManager()->GetWorld();


	// Initialise the players, create
	for (int pID = 0; pID < m_numOfPlayers; pID++)
	{
		m_playerList.push_back(new Player(pID, true, pID + 1));
	}

	float carLength = 4.0f;
	float half = (carLength * 4 + m_betweenCarSpace * 4) / 2;
	float startX = 0;
	startX -= half;
	startX = initCarPosition.x + startX;
	XMFLOAT3 startPosition = XMFLOAT3(startX, initCarPosition.y, initCarPosition.z);
	// Assign a car to each player by pointers

	for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
	{

		//set up the cars.
		m_playerList[pID]->SetCar(vehicles.GetVehicle(pID));

		//All cars are offset from left to right , left being the starting point.
		m_playerList[pID]->SetInitialPosition(startPosition);
		m_playerList[pID]->SetVehiclePosition(startPosition);
		startPosition.x += carLength;
		startPosition.x += m_betweenCarSpace;
	}

	//Set up the colours for each player
	m_playerList[0]->GetCar()->SetVehicleColour(XMFLOAT4(178, 34, 34, 0));
	m_playerList[1]->GetCar()->SetVehicleColour(XMFLOAT4(0, 128, 0, 0));
	m_playerList[2]->GetCar()->SetVehicleColour(XMFLOAT4(30, 144, 225, 0));
	m_playerList[3]->GetCar()->SetVehicleColour(XMFLOAT4(225, 215, 0, 0));

	for (int x = 0; x < 4; x++)
	{
		m_initialPlayerX[x] = m_playerList[x]->GetVehiclePosition().x;
	}

	//generate a vehicle for each player
	if (!vehicles.CheckGenerate(dx, resource))
	{
		return false;
	}
	return true;
}

void PlayerController::Update(float dt, ArcadeInput* pArcadeInput,
	Camera& camera, ParticleManager& particleManager,
	AudioEventManager& audioEvents, Terrain* terrain)
{
	for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
	{
		//Set the pointer for rigid bodies of the car to point to the player it is in.
		m_playerList[pID]->GetCar()->GetRigidBody()->setUserPointer(m_playerList[pID]);
	}

	//Average position is recalulated each step
	m_avgPlayerPosition = XMFLOAT3(0, 0, 0);
	//Recalcualte alive palyers each step.
	m_playersAlive = 0;
	//Calculate Race positions for each player (1,2,3,4)
	CalcualteRacePosition();

	switch (g_globalGameState)
	{

	case TITLE: // Pre start lobby screen wait for player input

#pragma region TITLE
	{
					int playersReady = 0;
					for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
					{
						if (!m_startTitle)
						{
							m_playerList[pID]->GetCar()->SetPosition(m_playerList[pID]->GetInitialPosition());
							btTransform transform;
							transform.setRotation(btQuaternion(0, 0, 0));
							transform.setOrigin(btVector3(m_playerList[pID]->GetInitialPosition().x, m_playerList[pID]->GetInitialPosition().y, m_playerList[pID]->GetInitialPosition().z));
							m_playerList[pID]->GetCar()->GetRigidBody()->setCenterOfMassTransform(transform);
							//make sure cars not moving
							m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
							m_playerList[pID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
							m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, 0, 0));

							//Check to see if any key has been pressed
							if (pArcadeInput->CheckIfAnyKeyPressed())
							{
								audioEvents.SetMiscEvents(AudioEventManager::PRESS_TO_CONTINUE_EVENT);
								m_startTitle = true;
							}

						}
						else
						{
							m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, -10, 0));
							m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));

							if (m_dragTimer[pID] > 0)
							{
								m_dragTimer[pID] -= dt;
							}
							else
							{
								XMFLOAT3 carPosition = m_playerList[pID]->GetCar()->GetPosition();
								if (MoveVehicle(pID, 0.25f, XMFLOAT3(carPosition.x, carPosition.y, 85.0f)))
								{
									playersReady++;
								}
							}
						}
					}
					//if all players reached destination, switch state
					if (playersReady >= m_playerList.size())
					{
						g_globalGameState = START_LOBBY;
					}

					break;
	}
#pragma endregion

	case START_LOBBY: // Initial player car selection lobby

#pragma region STARTLOBBY
		//start the timer
		timer += dt;
		for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
		{
			//Lift the car up
			if (timer >5 && timer < 7) //height is dependant on the time.
			{
				XMFLOAT3 carPosition = m_playerList[pID]->GetCar()->GetPosition();
				MoveVehicle(pID, 0.25f, XMFLOAT3(carPosition.x, 2, carPosition.z));

				RotateVehicle(pID, 0.25f, XMFLOAT3(3.14f, 0.52f, 0.0f));

				//do not allow the car to have any motion, lock in air.
				m_playerList[pID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, 0, 0));
			}

			//Allow players to lock in
			if (timer > 7 && timer < 25)
			{
				m_activePlayers = 0;
				//if player is not active, allow to activate
				if (!m_playerList[pID]->isActivePlayer())
				{
					m_playerList[pID]->SetPlayerState(3); // allow to lock in;
				}
				//if player locked in allow to sellect the car, and rotate the car.
				else
				{
					m_playerList[pID]->SetPlayerState(0);//allow players to rotate the car, and regenerate the car.
					m_playerList[pID]->GetCar()->SetFadeColour(true);
				}

				for each(Player* playerX in m_playerList)
				{
					if (playerX->isActivePlayer())
					{
						m_activePlayers += 1;
					}
				}

				//do not allow the car to have any motion, lock in air.
				m_playerList[pID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, 0, 0));
			}

			//rotate players back
			if (timer > 25 && timer < 28)
			{
				m_playerList[pID]->SetPlayerState(4); //
				RotateVehicle(pID, 0.25f, XMFLOAT3(0.0f, 0.0f, 0.0f));
				//do not allow the car to have any motion, lock in air.
				m_playerList[pID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
			}

			//Drop the cars
			if (timer >30 && timer < 31)
			{
				for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
				{
					//stop car colour fading.
					m_playerList[pID]->GetCar()->SetFadeColour(false);
					//allow players drive and controll the car.
					m_playerList[pID]->SetPlayerState(1);
					//apply gravity to cars body
					m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, -10, 0));
				}
				timer = 0;
			}
		}
#pragma endregion 

		break;

	case RACE:
	{
				 //Check if player stopped or stuck , and respawn him 
				 PlayerStuckCheck(dt);
				 //drag players if they are behind someone
				 PlayerDragging();

				 int PlayersFinished = 0;
				 for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
				 {
					 if (m_playerList[pID]->GetCar()->GetPosition().z >= terrain->GetRaceFinishZPositions()[g_globalRaceNumber])
					 {
						 m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
						 m_playerList[pID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
						 m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, 0, 0));
						 m_playerList[pID]->m_finished = true;
					 }
				 }

				 for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
				 {
					 if (m_playerList[pID]->m_finished)
					 {
						 PlayersFinished++;
					 }
				 }

				 if (PlayersFinished >= 1)
				 {
					 g_globalGameState = END_RACE;

					 for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
					 {
						 m_playerList[pID]->m_finished = false;
					 }
				 }
				 PlayerRespawnCheck(audioEvents, terrain, dt, camera);
				 break;
	}


	case END_RACE: // After race lbby before next race
	{
					   timer += dt;
					   for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
					   {
						   if (m_playerList[pID]->isActivePlayer())
						   {
							   //Remove powerup if player has one
							   m_playerList[pID]->RemovePowerUp();

							   m_playerList[pID]->SetAlive(true);
							   float finishlineZPos = terrain->GetRaceFinishZPositions()[g_globalRaceNumber];
							   XMFLOAT3 initPos = m_playerList[pID]->GetInitialPosition();
							   // MoveVehicle(pID, 1.25f, XMFLOAT3(initPos.x, initPos.y, finishlineZPos));
							   m_playerList[pID]->GetCar()->SetPosition(XMFLOAT3(initPos.x, initPos.y, finishlineZPos));
							   m_playerList[pID]->GetCar()->SetRotation(XMFLOAT3(0, 0, 0));
							   m_playerList[pID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
							   m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
							   m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, 0, 0));
						   }
					   }
					   //Lift the car up
					   if (timer > 20 && timer < 30) //height is dependant on the time.
					   {
						   g_globalRaceNumber++;
						   if (g_globalRaceNumber == 2)
						   {
							   g_globalGameState = FINAL_LOBBY;
						   }
						   else
						   {
							   g_globalGameState = START_LOBBY;
							   timer = 0;
						   }
					   }
					   break;
	}



	case FINAL_LOBBY: // After final race
		for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
		{
			if (m_playerList[pID]->isActivePlayer())
			{
				//Remove powerup if player has one
				m_playerList[pID]->RemovePowerUp();

				m_playerList[pID]->SetAlive(true);
				float finishlineZPos = terrain->GetRaceFinishZPositions()[g_globalRaceNumber - 1];
				XMFLOAT3 initPos = m_playerList[pID]->GetInitialPosition();
				// MoveVehicle(pID, 1.25f, XMFLOAT3(initPos.x, initPos.y, finishlineZPos));
				m_playerList[pID]->GetCar()->SetPosition(XMFLOAT3(initPos.x, initPos.y, finishlineZPos));
				m_playerList[pID]->GetCar()->SetRotation(XMFLOAT3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
				m_playerList[pID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, 0, 0));
			}
		}
		break;

	case RESET: // Reset game
		int t = 0;
		break;
	}

	for each (Player *playerX in m_playerList)
	{
		int playerID = playerX->GetPlayerNumber();

		///###### IF PLAYER IS ALIVE
		if (playerX->GetAlive())
		{
			if (playerX->isActivePlayer())
			{

				if (playerX->GetCollisionInfo().playerPlayerCollision)
				{
					audioEvents.SetPlayerEvents(AudioEventManager::COLLISION_EVENT, playerID);
					playerX->GetCollisionInfo().playerPlayerCollision = false;
				}

				if (playerX->GetCollisionInfo().playerPowerUpCollision)
				{
					audioEvents.SetPlayerEvents(AudioEventManager::POWERUP_PICKUP_EVENT, playerID);
					playerX->GetCollisionInfo().playerPowerUpCollision = false;
				}

				if (m_behindCamera)
				{
					//Check if player falls back behind the camera
					if (playerX->GetCar()->GetPosition().z < camera.GetPosition().z)
					{
						playerX->SetAlive(false);
					}
				}

				//Count up the counter for each alive player.
				m_playersAlive += 1;
				//Calcualte the Total X,Y,Z positions for Alive players
				m_avgPlayerPosition = m_avgPlayerPosition + playerX->GetCar()->GetPosition();
			}
		}
	}

	if (m_activePlayers > 0)
	{
		//Calcualte the average position between all alive players
		m_avgPlayerPosition = m_avgPlayerPosition / m_playersAlive;

	}

	else
	{
		for (int x = 0; x < 4; x++)
		{
			m_avgPlayerPosition = m_avgPlayerPosition+ GetPlayerList()[x]->GetCar()->GetPosition();
		}
		m_avgPlayerPosition = m_avgPlayerPosition / 4;
	}

	for (int x = 0; x < 4; x++)
	{
		//Update the particles
		UpdateRetroParticles(particleManager);
		//Call before update to overrite all the other possible speeds, 
		CheckSpeedLimit();
		// Updates all the players assigning the right key inputs
		m_playerList[x]->Update(dt, pArcadeInput->GetKeysPressed(x));
	}

	//go through the different events and add them to stack ( will put in function when i have
	// access to everything i need )
	for (unsigned int p = 0; p < (unsigned int)m_numOfPlayers; p++)
	{
		if (m_playerList[p]->GetCar()->GetGenerate())
		{
			audioEvents.SetPlayerEvents(AudioEventManager::REBUILD_CAR_EVENT, p);
		}
		if (m_playerList[p]->GetPowerupActivated())
		{
			audioEvents.SetPlayerEvents(AudioEventManager::POWERUP_ACTIVATE_EVENT, p);
			m_playerList[p]->SetPowerupActivated(false);
		}
	}
}

void PlayerController::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	for each(Player* player in m_playerList)
	{
		if (player->GetAlive())
		{
			player->Render(dx, cam, lightManager);
		}
	}
}

void PlayerController::CalcualteRacePosition()
{
	for each(Player* playerX in m_playerList)
	{
		//determine the leader
		if (playerX->GetRacePosition() == 1)
		{
			m_raceLeaderId = playerX->GetPlayerNumber();
		}

		//calcualte race positions for palyers
		for each(Player* playerZ in m_playerList)
		{
			if (playerX != playerZ)
			{
				if (playerX->GetCar()->GetPosition().z >
					playerZ->GetCar()->GetPosition().z)
				{
					if (playerX->GetRacePosition() > playerZ->GetRacePosition())
					{
						int tempPos = playerX->GetRacePosition();
						playerX->SetRacePosition(playerZ->GetRacePosition());
						playerZ->SetRacePosition(tempPos);
					}
				}
			}
		}
	}
}


void PlayerController::UpdateRetroParticles(ParticleManager& particleManager)
{
	//Update the retro particles
	unsigned int size = m_playerList.size();
	for (unsigned int p = 0; p < size; p++)
	{
		IPSEmitterObject* retro1, *retro2;

		//Get the current player retro emitters
		particleManager.GetPlayerRetroEmitters(p, &retro1, &retro2);

		//Start the emitters (TURN OFF WHEN THEY ARE RESPAWNED && if in wrong game state)
		if ((g_globalGameState == RACE) || (g_globalGameState == START_LOBBY))
		{
			//Get the retro position
			XMFLOAT3 retro1Pos = m_playerList[p]->GetCar()->GetRetro(RIGHT_RETRO)->GetPosition();
			XMFLOAT3 retro2Pos = m_playerList[p]->GetCar()->GetRetro(LEFT_RETRO)->GetPosition();

			//THIS IS SO BAD NEED TO EXTRACT TRANSLATION FROM WORLDMATRIX
			// set an origin
			XMFLOAT3 retro1Origin = XMFLOAT3(1.0f, 0.7f, 0.0f);
			XMFLOAT3 retro2Origin = XMFLOAT3(-1.0f, 0.7f, 0.0f);

			//Set position and direction
			XMFLOAT3 retro1Dir = RightVec(m_playerList[p]->GetCar()->GetRetro(RIGHT_RETRO)->GetWorldMat());
			XMFLOAT3 retro2Dir = RightVec(m_playerList[p]->GetCar()->GetRetro(LEFT_RETRO)->GetWorldMat());

			retro1Pos = retro2Pos + (retro1Dir * 3.7f) + retro1Origin;
			retro2Pos = retro2Pos + (retro2Dir * 3.7f) + retro2Origin;

			retro1->SetPosition(retro1Pos);
			retro2->SetPosition(retro2Pos);

			retro1->SetDirection(retro1Dir);
			retro2->SetDirection(retro2Dir);

			retro1->StartEmitter();
			retro2->StartEmitter();
		}
		else if (!m_playerList[p]->GetAlive())
		{
			retro1->StopEmitter();
			retro2->StopEmitter();
		}
		else
		{
			retro1->StopEmitter();
			retro2->StopEmitter();
		}

	}
}

void PlayerController::Shutdown()
{
	for (unsigned int i = 0; i < m_playerList.size(); ++i)
	{
		m_playerList[i]->Shutdown();
	}

	DeleteVector(m_playerList);
}

int PlayerController::GetRaceLeaderId()
{
	return m_raceLeaderId;
}

vector <Player* > PlayerController::GetPlayerList()
{
	return m_playerList;
}

Player* PlayerController::GetPlayer(int Id)
{
	return m_playerList[Id];
}

XMFLOAT3 PlayerController::GetAvgPlayerPosition()
{
	return m_avgPlayerPosition;
}

int PlayerController::GetLeader()
{
	return m_raceLeaderId;
}

int PlayerController::GetPlayersAlive()
{
	return m_playersAlive;
}

void  PlayerController::SetSpeedLimit(float _speedLimit)
{
	m_speedLimit = _speedLimit;
}

void PlayerController::PlayerStuckCheck(float dt)
{
	for (unsigned int pID = 0; pID < m_playerList.size(); pID++)
	{

		if (m_playerList[pID]->isActivePlayer())
		{
			//If player speed is less than 1 then respawn.(kill player)
			float currentSpeed = m_playerList[pID]->GetCar()->m_vehicle->getCurrentSpeedKmHour();

			//if speed of the player is less than 1 km/h
			if (currentSpeed < 1)
			{

				//if player speed is not increasing update stuck timer.
				m_stuckTimer[pID] += dt;

				//respawn the players if he did not move for timeuntil respawn.
				//additional time until respawn if player is in the air.
				float extraTime = 0;
				if (m_playerList[pID]->GetCar()->GetPosition().y > 3)
				{
					extraTime = m_timeUntilRespawnInAir;
				}

				if (m_stuckTimer[pID] >= m_timeUntilPlayerRespawn + extraTime)
				{
					//kill the player
					m_playerList[pID]->SetAlive(false);
					//reset the timer
					m_stuckTimer[pID] = 0;
				}
			}
		}
	}
}

void PlayerController::PlayerDragging()
{
	for each (Player *playerX in m_playerList)
	{
		if (playerX->GetAlive() && !playerX->m_finished)
		{
			for each (Player *playerY in m_playerList)
			{

				if (playerY->GetAlive() && playerX != playerY && playerX->isActivePlayer() && playerY->isActivePlayer())
				{

					if (playerX->GetVehiclePosition().z < playerY->GetVehiclePosition().z)
					{
						if (((playerY->GetVehiclePosition().z - playerX->GetVehiclePosition().z) > 0.1f) &&
							(playerY->GetVehiclePosition().z - playerX->GetVehiclePosition().z) < m_distanceForDragging)
						{

							//area where player has do be behind in X coordinate
							float leftrightOffset = 0.5f;
							if (((playerY->GetVehiclePosition().x - playerX->GetVehiclePosition().x) > -leftrightOffset)
								&& ((playerY->GetVehiclePosition().x - playerX->GetVehiclePosition().x) < leftrightOffset))
							{
								playerX->SetSpeedLimitActive(false);
								playerX->GetCar()->GetVehicleInfo().m_engineForce = playerX->GetCar()->GetVehicleInfo().m_defaultEngineForce + 1000;
							}
						}
					}
					else
					{
						playerX->SetSpeedLimitActive(true); //delay by time
						playerX->GetCar()->GetVehicleInfo().m_engineForce = playerX->GetCar()->GetVehicleInfo().m_defaultEngineForce;
					}
				}
			}
		}

	}
}

void PlayerController::CheckSpeedLimit()
{
	for each (Player *playerX in m_playerList)
	{
		if (playerX->GetAlive())
		{
			if (playerX->GetSpeedLimitActive())
			{
				float currentSpeed = playerX->GetCar()->m_vehicle->getCurrentSpeedKmHour();
				if (currentSpeed > m_speedLimit)
				{
					playerX->GetCar()->GetVehicleInfo().m_engineForce = 0;
				}
				else
				{
					playerX->GetCar()->GetVehicleInfo().m_engineForce = playerX->GetCar()->GetVehicleInfo().m_defaultEngineForce;
				}
			}
			else
			{
				playerX->GetCar()->GetVehicleInfo().m_engineForce = playerX->GetCar()->GetVehicleInfo().m_defaultEngineForce + 1000;
			}
		}
	}
}

bool PlayerController::MoveVehicle(int _playerID, float _liftSpeed, XMFLOAT3 _position)
{
	bool state = false;

	//Get current position and rotataion
	btQuaternion currentCarRotation = m_playerList[_playerID]->GetCar()->GetRigidBody()->getCenterOfMassTransform().getRotation();
	btVector3 currentCarPosition = m_playerList[_playerID]->GetCar()->GetRigidBody()->getCenterOfMassTransform().getOrigin();

	//lerp current position to goal.
	btVector3 goalCarPosition = btVector3(_position.x, _position.y, _position.z);
	currentCarPosition = lerp(currentCarPosition, goalCarPosition, _liftSpeed);

	//create new transform
	btTransform transOriginal = btTransform(currentCarRotation, currentCarPosition);

	//apply the transformation to the car.
	m_playerList[_playerID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 200));

	if ((currentCarPosition.getX() >= _position.x - 0.1) &&
		(currentCarPosition.getY() >= _position.y - 0.1) &&
		(currentCarPosition.getZ() >= _position.z - 0.1))
	{
		m_playerList[_playerID]->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
		state = true;
	}
	else
	{
		state = false;
	}
	return state;
}

bool PlayerController::RotateVehicle(int _playerID, float _rotationSpeed, XMFLOAT3 _rotationAngle)
{
	//disable gravity, cars might fal if not.
	//m_playerList[_playerID]->GetCar()->GetRigidBody()->setGravity(btVector3(0, 0, 0));

	//Get current position and rotataion
	btQuaternion currentCarRotation = m_playerList[_playerID]->GetCar()->GetRigidBody()->getCenterOfMassTransform().getRotation();
	btVector3 currentCarPosition = m_playerList[_playerID]->GetCar()->GetRigidBody()->getCenterOfMassTransform().getOrigin();

	//lerp current position to goal.
	btQuaternion goalCarRotation = btQuaternion(_rotationAngle.x, _rotationAngle.y, _rotationAngle.z);
	currentCarRotation = slerp(currentCarRotation, goalCarRotation, _rotationSpeed);

	//create new transform
	btTransform transOriginal = btTransform(currentCarRotation, currentCarPosition);

	//apply the transformation to the car.
	m_playerList[_playerID]->GetCar()->GetRigidBody()->setWorldTransform(transOriginal);

	if (currentCarRotation.getX() >= goalCarRotation.getX() &&
		currentCarRotation.getY() >= goalCarRotation.getY() &&
		currentCarRotation.getZ() >= goalCarRotation.getZ() &&
		currentCarRotation.getW() >= goalCarRotation.getW()
		)
	{
		return true;
	}
	return false;
}

int PlayerController::GetNumActivePlayers()
{
	return m_activePlayers;
}

void PlayerController::PlayerRespawnCheck(AudioEventManager	&audioEvents, Terrain* terrain, float dt, Camera& camera)
{
	for each(Player* playerX in m_playerList)
	{
		///###### IF PLAYER IS DEAD
		if (!playerX->GetAlive() && playerX->isActivePlayer())
		{
			bool denyRespawn = false;

			//Get id of the current player
			int playerID = playerX->GetPlayerNumber();

			if (!m_playerPrevDead)
			{
				audioEvents.SetPlayerEvents(AudioEventManager::DESTROYED_EVENT, playerID);
				m_playerPrevDead = true;
			}

			//Remove Current powerUp
			playerX->RemovePowerUp();

#pragma region RESPAWN
			if (m_currentRespawnTime[playerID] >= m_timeUntilPlayerRespawn)
			{
				XMFLOAT3 newPosition = m_playerList[m_raceLeaderId]->GetCar()->GetPosition();
				newPosition.z -= 12.0f;
				newPosition.y = -1.0f;

				if (m_activePlayers == 1)
				{
					newPosition.z = camera.GetPosition().z + 15.0f;
				}

				XMFLOAT3 terrainPosition = terrain->GetPosition();
				vector<vector<int>> terrainRestrict = terrain->GetTerrainRestrictions();
				float terrainScale = terrainRestrict[0][3];
				int leaderChunkPos = (int)(((m_playerList[m_raceLeaderId]->GetCar()->GetPosition().z / (float)terrainScale) / 32.0f));
				float terrainMinX = (float)(((terrainRestrict[leaderChunkPos][0] + 3) * terrainScale) + terrainPosition.x);
				float terrainMaxX = (float)(((terrainRestrict[leaderChunkPos][1] - 3) * terrainScale) + terrainPosition.x);
				float terrainClearWidth = terrainMaxX - terrainMinX;

				vector<XMFLOAT3> playerPositions;

				for each(Player *playerY in m_playerList)
				{
					if (playerY != playerX &&
						(playerY->GetCar()->GetPosition().z > (newPosition.z - 4.0f) &&
						playerY->GetCar()->GetPosition().z < (newPosition.z + 4.0f)))
					{
						playerPositions.push_back(playerY->GetCar()->GetPosition());
					}
				}

				float carWidth = 6.0f;
				unsigned int carObstructionAmount = playerPositions.size();
				//where on the x to place the respawning car
				float randomXPos = 0.0f;

				if (carObstructionAmount > 0)
				{
					//work out position depending on the amount of cars restricting the respawn area
					switch (carObstructionAmount)
					{
						//1 car in placement area
					case 1:
					{
							  float obstructingCarPosX = playerPositions[0].x;

							  //left of car random pos								|| { terrain } |-w/2-| <---rand---> |-w/2-| { car } ...
							  //float lHandRandX = ( float )( rand() % ( int )( ( ( obstructingCarPosX - ( carWidth / 2.0f ) ) - ( terrainMinX + ( carWidth / 2.0f ) ) ) + ( terrainMinX + ( carWidth / 2.0f ) ) ) );
							  //right if car random pos								... { car } |-w/2-| <---rand---> |-w/2-| { terrain } ||
							  //float rHandRandX = ( float )( rand() % ( int )( ( ( terrainMaxX - ( carWidth / 2.0f ) ) - ( obstructingCarPosX + ( carWidth / 2.0f ) ) ) + ( obstructingCarPosX + ( carWidth / 2.0f ) ) ) );

							  int leftDist = (int)((obstructingCarPosX - (carWidth / 2.0f)) - (terrainMinX + (carWidth / 2)));
							  int rightDist = (int)((terrainMaxX - (carWidth / 2.0f)) - (obstructingCarPosX + (carWidth / 2.0f)));
							  if (leftDist == 0) leftDist = 1;
							  if (rightDist == 0) rightDist = 1;

							  float lHandRandX = (float)(rand() % leftDist + (int)(terrainMinX + (carWidth / 2.0f)));
							  float rHandRandX = (float)(rand() % rightDist + (int)(obstructingCarPosX + (carWidth / 2.0f)));

							  float terrainToCar = obstructingCarPosX - terrainMinX;
							  float carToTerrain = terrainMaxX - obstructingCarPosX;

							  //if theres enough room both sides
							  if ((terrainToCar > carWidth) && (carToTerrain > carWidth))
							  {
								  int leftRight = rand() % 2;
								  leftRight == 0 ? randomXPos = lHandRandX : randomXPos = rHandRandX;
							  }
							  //only room enough on one side
							  else if ((terrainToCar > carWidth) || (carToTerrain > carWidth))
							  {
								  terrainToCar > carWidth ? randomXPos = lHandRandX : randomXPos = rHandRandX;
							  }
							  // neither side free
							  else
							  {
								  denyRespawn = true;
							  }

							  break;
					}

						//2 cars in placement area
					case 2:
					{
							  float obstructingCarPosXLeft = playerPositions[0].x;
							  float obstructingCarPosXRight = playerPositions[1].x;

							  //swap if l > r
							  if (obstructingCarPosXLeft > obstructingCarPosXRight)
							  {
								  float tempSwap = obstructingCarPosXLeft;
								  obstructingCarPosXLeft = obstructingCarPosXRight;
								  obstructingCarPosXRight = tempSwap;
							  }

							  int leftDistance = (int)((obstructingCarPosXLeft - (carWidth / 2.0f)) - (terrainMinX + (carWidth / 2.0f)));
							  int middleDistance = (int)((obstructingCarPosXRight - (carWidth / 2.0f)) - (obstructingCarPosXLeft + (carWidth / 2.0f)));
							  int rightDistance = (int)((terrainMaxX - (carWidth / 2.0f)) - (obstructingCarPosXRight + (carWidth / 2.0f)));
							  if (leftDistance == 0) leftDistance = 1;
							  if (middleDistance == 0) middleDistance = 1;
							  if (rightDistance == 0) rightDistance = 1;

							  float leftHandRandX = (float)(rand() % leftDistance + (int)(terrainMinX + (carWidth / 2.0f)));
							  float middleRandX = (float)(rand() % middleDistance + (int)(obstructingCarPosXLeft + (carWidth / 2.0f)));
							  float rightHandRandX = (float)(rand() % rightDistance + (int)(obstructingCarPosXRight + (carWidth / 2.0f)));

							  float terrainToLHCar = obstructingCarPosXLeft - terrainMinX;
							  float leftToRightCar = obstructingCarPosXRight - obstructingCarPosXLeft;
							  float rhCarToTerrain = terrainMaxX - obstructingCarPosXRight;

							  bool t2L = false;
							  bool l2R = false;
							  bool r2T = false;

							  //if space in areas assign bools
							  if (terrainToLHCar > carWidth) t2L = true;
							  if (leftToRightCar > carWidth) l2R = true;
							  if (rhCarToTerrain > carWidth) r2T = true;

							  //if space in all 3 gaps
							  if (t2L && l2R && r2T)
							  {
								  //chose the gap to place the car and assign a random x position
								  int leftMiddleRight = rand() % 3;

								  if (leftMiddleRight == 0)
								  {
									  randomXPos = leftHandRandX;  // left
								  }
								  if (leftMiddleRight == 1)
								  {
									  randomXPos = middleRandX;    // middle
								  }
								  if (leftMiddleRight == 2)
								  {
									  randomXPos = rightHandRandX; // right
								  }
							  }
							  //if 1 or 2 spaces free
							  else if (t2L || l2R || r2T)
							  {
								  int leftRight = rand() % 2;

								  if ((t2L && l2R) && !r2T)
								  {
									  leftRight == 0 ? randomXPos = leftHandRandX : randomXPos = middleRandX; // l & m
								  }
								  if ((l2R && r2T) && !t2L)
								  {
									  leftRight == 0 ? randomXPos = middleRandX : randomXPos = rightHandRandX; // m & r
								  }
								  if ((t2L && r2T) && !l2R)
								  {
									  leftRight == 0 ? randomXPos = leftHandRandX : randomXPos = rightHandRandX; // l & r
								  }
								  if (t2L && !(l2R || r2T))
								  {
									  randomXPos = leftHandRandX;  // l
								  }
								  if (l2R && !(t2L || r2T))
								  {
									  randomXPos = middleRandX;    // m
								  }
								  if (r2T && !(t2L || l2R))
								  {
									  randomXPos = rightHandRandX; // r
								  }
							  }
							  //no spaces free
							  else
							  {
								  denyRespawn = true;
							  }

							  break;
					}

						// if no case met
					default:
					{
							   denyRespawn = true;
					}
					}
				}
				//if no cars random position between terrain params
				else
				{
					randomXPos = (float)(rand() % (int)terrainClearWidth + (int)terrainMinX); // carwidth
				}

				//assign newpositionx to the random position
				newPosition.x = randomXPos;


				//respawn the car
				if (!denyRespawn)
				{
					btTransform currentT;
					currentT = playerX->GetCar()->GetRigidBody()->getWorldTransform();
					btVector3 currentPos = currentT.getOrigin();
					currentPos.setZ(newPosition.z);
					currentPos.setX(newPosition.x);
					currentPos.setY(newPosition.y);
					currentT.setOrigin(currentPos);
					currentT.setRotation(btQuaternion(0, 0, 0));
					playerX->GetCar()->GetRigidBody()->setWorldTransform(currentT);
					playerX->GetCar()->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));

					btVector3 vel = m_playerList[m_raceLeaderId]->GetCar()->GetRigidBody()->getLinearVelocity();
					playerX->GetCar()->GetRigidBody()->setLinearVelocity(btVector3(0, 0, vel.getZ()));

					//Add to the death counter.

					m_playerDeaths[playerID] ++;
					playerX->SetAlive(true);
					m_currentRespawnTime[playerID] = 0;
					audioEvents.SetPlayerEvents(AudioEventManager::RESPAWN_EVENT, playerID);
					m_playerPrevDead = false;
				}
			}
#pragma endregion

			m_currentRespawnTime[playerID] += dt; // update timer
		}
	}
}

void PlayerController::BehindCameraCheckSwitch(bool _behind)
{
	m_behindCamera = _behind;
}