#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <iostream>
#include <vector>
#include "Player.h"
class Player;

#include "ArcadeInput.h"
#include "Input.h"
#include "VehicleController.h"
#include "AudioEventManager.h"
#include "ParticleManager.h"
#include "Timer.h"
#include "Camera.h"
#include "GlobalGameState.h"
#include <AntTweakBar.h>
#include "Terrain.h"

using std::vector;
using namespace GlobalGameState;
//Forward dec
class ArcadeInput;

class PlayerController
{
public:
	PlayerController();
	~PlayerController();

	bool Initialise(DxGraphics* dx, VehicleController& vehicles, ResourceManager& resource, XMFLOAT3 initPosition);
	//Has responsabillity of changing the game state
	void Update(float dt, ArcadeInput* pArcadeInput, Camera& camera,
		ParticleManager& particleManager, AudioEventManager& audioEvents,
		Terrain* terrain);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Shutdown();

	//Get average position of all alive and active players
	XMFLOAT3 GetAvgPlayerPosition();
	//Return player object dependant on Id
	Player* GetPlayer(int Id);
	//return the ID of the currrent leader of the race
	int GetRaceLeaderId();	
	//return the whole vector of players
	vector <Player* > GetPlayerList();	
	//Get current Leader Id
	int GetLeader();
	//How many players are currently alive
	int GetPlayersAlive();
	//how many players have locked in and are playing
	int GetNumActivePlayers();

	//Lift up cars to specific height and rotate to specific angle.
	bool MoveVehicle(int _playerID, float _liftSpeed, XMFLOAT3 _position);
	bool RotateVehicle(int _playerID, float _rotationSpeed, XMFLOAT3 _rotationAngle);
	//set a specific speed limit that players can not exceed.
	void SetSpeedLimit(float _speedLimit);
	// activate in order to respawn players that are behgind camera.
	void BehindCameraCheckSwitch(bool _behind);
	//Player Death Count, for score board.
	int						m_playerDeaths[4];

private:
	//Calcualte player positions in race
	void CalcualteRacePosition();

	void PlayerRespawnCheck(AudioEventManager &audioEvents, Terrain* terrain, float dt, Camera& cam);
	float timer;

	void UpdateRetroParticles(ParticleManager& particleManager);

	//Limit player speed
	void CheckSpeedLimit();

	//Limit car to certein speed value
	float					m_speedLimit;

	//Check if players needs to be respawned ( stuck )
	void PlayerStuckCheck(float dt);

	//dragging behind player
	void PlayerDragging();

	//engine force while dragging
	float					m_draggingEngineForce;

	//distance between players in order to start dragging
	int						m_distanceForDragging;

	//All players are stored in a vector
	vector<Player*>			m_playerList;

	//struct handles the collision sets up collision information
	//PlayerPlayerCallback	PlayerPlayerCollisionHandler;

	//BulletPhysics pointer to the world physics simualtion.
	btDynamicsWorld*		m_dynamicsWorld;

	//Average Position of all Alive players
	XMFLOAT3				m_avgPlayerPosition;

	//Starting position of all the players, (of first car)
	XMFLOAT3				m_startingPosition;

	//total number of players in the game
	int						m_numOfPlayers;

	//time until player respawns after death in seconds.
	float					m_timeUntilPlayerRespawn;

	//If player in air time before he respawns( longer for better visual effect) //adds up to the m_timeUntilPlayerRespawn
	float					m_timeUntilRespawnInAir;

	//number of players alive
	int						m_playersAlive;

	//current race leader id.<vector>
	int						m_raceLeaderId;

	//Current player respawn time, counts up for each player when he is dead until m_timeUntilPlayerRespawn is reached.
	//timer for each player
	float					m_currentRespawnTime[4];

	//Time until the cars start to drag, used to start each car at different time.
	float					m_dragTimer[4];

	float					m_stuckTimer[4];

	bool					m_playerPrevDead;

	int						m_activePlayers;

	float					m_initialPlayerX[4];

	bool					m_startTitle;

	//space between cars at start line.
	float					m_betweenCarSpace;

	bool					m_behindCamera;
};

#endif