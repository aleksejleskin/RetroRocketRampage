#include "Player.h"

Player::Player(int playerNum, bool controllerInput, int racePosition) : m_playerNumber(playerNum),
	m_controllerInput(controllerInput), m_shutdown(false), m_racePosition(racePosition), /*m_fire(false),*/
	m_playerState(4), m_powerupActivated(false), m_collisionInformation( CollisionInfo() )
{
	m_currentPowerUp = NULL;
	m_car = NULL;
	m_timer = 0;

	m_activePlayer = false;
	m_reverseSteering = false;
	//shield is disabled by deafult
	m_shield = false;
	m_enableSpeedLimit = true;
	m_finished = false;
	m_alive = true;
	m_initialPosition = XMFLOAT3(0, 0, 0);
	m_powerupActivated = false;

	//COLLISION INFO
	m_collisionInformation.playerPlayerCollision = false;
	m_collisionInformation.playerPowerUpCollision = false;
	m_collisionInformation.playerTerrainCollision = false;
	m_collisionInformation.counter = 0;

	m_renderDelay = 0;
}

Player::~Player()
{
	if (!m_shutdown)
	{
		Shutdown();
	}
}

void Player::Update(float dt, vector<bool> keys)
{
	switch (m_playerState)
	{
		//MENU CAR SELLECTION
	case 0:
		m_car->Update(dt);
		//
		m_timer += dt;
		if (keys[LEFT])
		{
			XMFLOAT3 rotCurrent = XMFLOAT3(m_car->GetYaw(), m_car->GetPitch(), m_car->GetRoll());
			btVector3 rot = btVector3(0, -1, 0);
			m_car->GetRigidBody()->setAngularVelocity(rot);
		}
		if (keys[MIDDLE])
		{
			if (m_timer > 0.3)
			{
				m_car->SetGenerate(true);
				m_timer = 0;
			}
		}
		if (keys[RIGHT])
		{
			XMFLOAT3 rotCurrent = XMFLOAT3(m_car->GetYaw(), m_car->GetPitch(), m_car->GetRoll());
			btVector3 rot = btVector3(0, 1, 0);
			m_car->GetRigidBody()->setAngularVelocity(rot);

		}

		break;

		//ALLOW PLAYER TO CONTROLL THE CAR, RACE
	case 1:
	{
			  if (m_activePlayer)
			  {
				  float vehicleSteering = m_car->GetVehicleValues().m_steering;
				  float steeringForce = m_car->GetVehicleInfo().m_steeringForce;
				  float maxSteeringAngle = m_car->GetVehicleInfo().m_maxSteeringAngle;
				  float maxEngineForce = m_car->GetVehicleInfo().m_engineForce;
				  float maxbreakingForce = m_car->GetVehicleInfo().m_breakingForce;
				  if (keys[LEFT])
				  {
					  //Reverse car steering if active
					  float steeringInverse = 1;
					  if (m_reverseSteering)
					  {
						  steeringInverse = -1;
					  }
					  m_car->GetVehicleValues().m_steering = (vehicleSteering - steeringForce * steeringInverse);
					  if (vehicleSteering < -maxSteeringAngle)
					  {
						  m_car->GetVehicleValues().m_steering = -maxSteeringAngle;
					  }
				  }
				  if (keys[MIDDLE])
				  {
					  if (m_currentPowerUp != NULL)
					  {
						  m_currentPowerUp->Activate(true);
						  m_currentPowerUp = NULL;
						  m_powerupActivated = true;
					  }
				  }
				  if (keys[RIGHT])
				  {
					  //Reverse car steering if active
					  float steeringInverse = 1;
					  if (m_reverseSteering)
					  {
						  steeringInverse = -1;
					  }
					  m_car->GetVehicleValues().m_steering = (vehicleSteering + steeringForce * steeringInverse);
					  if (vehicleSteering > maxSteeringAngle)
					  {
						  m_car->GetVehicleValues().m_steering = maxSteeringAngle;
					  }
				  }

				  m_car->GetVehicleValues().m_engineForce = maxEngineForce;
				  m_car->Update(dt);
			  }
			  break;
	}
		//WAIT FOR PLAYER ACTIVATE
	case 3:
	{
			  m_car->Update(dt);
			  if (keys[MIDDLE])
			  {
				  m_activePlayer = true;
			  }

			  break;
	}
	case 4:
	{
			  m_car->GetVehicleValues().m_engineForce = 0;
			  m_car->GetVehicleValues().m_breakingForce = 100;
			  m_car->Update(dt);
			  break;
	}

	}


	if (m_currentPowerUp != NULL)
	{
		m_car->SetRetroColour(m_currentPowerUp->getColour());
		m_car->FadeRetroColour(true);
	}
	else
	{
		m_car->SetRetroColour(m_car->GetDefaultRetroColour());
		m_car->FadeRetroColour(true);
	}
	if (!keys[LEFT] && !keys[MIDDLE] && !keys[RIGHT] && !keys[BACK])
	{
		//	m_car->gBreakingForce = m_car->maxBreakingForce;
		//m_car->gEngineForce = 0;
	}
	//rendere delay timer
	if (m_renderDelay < 5)
	{
		m_renderDelay += dt;
	}
}

void Player::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	
	if (m_car->GetGenerate())
	{
		m_renderDelay = 0;
		
	}
	else
	{
		if (m_renderDelay >= 0.1f)
		{
			m_car->Render(dx, cam, lightManager);
		}
	}
}

void Player::Shutdown()
{
	if (!m_shutdown)
	{
		m_car->UnloadContent();

		delete m_car; //dealocate memory 
		m_shutdown = true;
	}
}

///Get Set

int Player::GetPlayerNumber()
{
	return m_playerNumber;
}

bool Player::GetAlive()
{
	return m_alive;
}

void Player::SetAlive(bool isAlive)
{
	m_alive = isAlive;
}

XMFLOAT3 Player::GetVehiclePosition()
{
	return m_car->GetPosition();
}

int Player::GetRacePosition()
{
	return m_racePosition;
}

Vehicle* Player::GetCar()
{
	return m_car;
}

void Player::SetRacePosition(int racePosition)
{
	m_racePosition = racePosition;
}

void Player::SetVehiclePosition(XMFLOAT3 pos)
{
	m_car->GameObject::SetPosition(pos);
}



void Player::SetCar(Vehicle* car)
{
	m_car = car;
}


int Player::GetPlayerState()
{
	return m_playerState;
}

void Player::SetPlayerState(int state)
{
	m_playerState = state;
}

bool Player::isActivePlayer()
{
	return m_activePlayer;
}

void Player::SetPowerUpPtr(PowerUp* _powerUp)
{
	m_currentPowerUp = _powerUp;
}

PowerUp* Player::GetPowerUpPtr()
{
	return m_currentPowerUp;
}

void Player::SetReverseSteering(bool _steerigReverse)
{
	m_reverseSteering = _steerigReverse;
}

void Player::SetShieldStatus(bool _shield)
{
	m_shield = _shield;
}

bool Player::GetShieldStatus()
{
	return m_shield;
}

void Player::SetSpeedLimitActive(bool _active)
{

	bool current = m_enableSpeedLimit;
	bool newb = _active;
	m_enableSpeedLimit = _active;
}

bool Player::GetSpeedLimitActive()
{
	return m_enableSpeedLimit;
}

bool Player::GetPowerupActivated()
{
	return m_powerupActivated;
}

void Player::SetPowerupActivated(bool activated)
{
	m_powerupActivated = activated;
}

CollisionInfo &Player::GetCollisionInfo()
{
	return m_collisionInformation;
}


void Player::SetInitialPosition(XMFLOAT3 _initPos)
{
	m_initialPosition = _initPos;
}
XMFLOAT3 Player::GetInitialPosition()
{
	return m_initialPosition;
}

void Player::RemovePowerUp()
{
	if (m_currentPowerUp != NULL)
	{
		m_currentPowerUp->Deactivate();
		m_currentPowerUp->SetAlive(false);
		m_currentPowerUp->SetPlayerPtr(NULL);
		m_currentPowerUp = NULL;
	}
}