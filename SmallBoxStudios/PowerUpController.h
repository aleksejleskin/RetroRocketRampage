#ifndef POWERUPCONTROLLER_H
#define POWERUPCONTROLLER_H

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>

#include "PowerUp.h"
//POWERUP TYPES
#include "Scale.h"
#include "Boost.h"
#include "obstacle.h"
#include "Push.h"
#include "Shield.h"
#include "Steal.h"
#include "SteeringInverse.h"

#include "Timer.h"
#include "HelicopterCamera.h"
#include "GlobalGameState.h"

using namespace GlobalGameState;

class PowerUpController
{
public:
	PowerUpController();
	~PowerUpController();

	bool Initialise(DxGraphics *dx, ResourceManager& resources, PlayerController *playerController, TerrainController &_terrainController);
	void Update(float dt, HelicopterCamera& camera);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Shutdown();

	vector <PowerUp *>& GetPowerUpList();
	PowerUp* GetPowerUp(unsigned int id);
	//int GetPowerUpId(unsigned int id);

	void PowerUpControllerSwitch(bool _enable);
private:
	//PowerUp Position Generator for respawns
	void GenerateRandomRespawnPosition();
	//Generats a Respawn number for a powerUp
	void GenerateRandomRespawnTime(PowerUp &powerUpId);
	//Update Function for when powerUp is dead

	XMFLOAT3 m_playersAvgPos,
		m_leaderPos;

	bool m_shutdown;

	//Array of all the PowerUps
	vector <PowerUp*> m_powerUpList;

	PlayerController* p_playerC;
	TerrainController* p_terrainC;
	btDynamicsWorld* m_dynamicsWorld;

	bool	m_powerUpEnable;
};

#endif