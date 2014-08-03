#ifndef PLAYER_H
#define PLAYER_H

#include "DxGraphics.h"
#include "Vehicle.h"
#include "Camera.h"
#include "Timer.h"
#include "PowerUp.h"
class PowerUp;
//class PlayerController;
//class PowerUpController;
#include "PPP.h"

class Player
{
public:
	Player(int playerNum, bool controllerInput, int racePosition);
	~Player();

	void Update(float dt, vector<bool> keys);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	void Shutdown();

	bool GetAlive();
	XMFLOAT3 GetVehiclePosition();
	
	int GetRacePosition();
	int GetPlayerNumber();
	bool EnableSpeedLimit();
	void SetAlive(bool isAlive);
	void SetVehiclePosition(XMFLOAT3 pos);
	void SetRacePosition(int racePos);

	Vehicle* GetCar();
	void SetCar(Vehicle* car);

	void SetSpeedLimitActive(bool _active);
	bool GetSpeedLimitActive();

	void SetShieldStatus(bool _shield);
	bool GetShieldStatus();

	bool GetPowerupActivated();
	void SetPowerupActivated( bool activated );

	void SetPowerUpPtr(PowerUp* _powerUp);
	PowerUp* GetPowerUpPtr();
	//FMS STATE
	void SetPlayerState(int state);
	int GetPlayerState();

	//Check if player locked in
	bool isActivePlayer();

	//Starting position of the car.
	void SetInitialPosition(XMFLOAT3 _initPos);
	XMFLOAT3 GetInitialPosition();

	void SetReverseSteering(bool _steerigReverse);
	CollisionInfo &GetCollisionInfo();
	bool			m_finished;

		//Remove powerup if player has one
	void RemovePowerUp();
private:

	unsigned short int m_playerNumber,
		m_racePosition;

	bool m_alive,
		m_shutdown,
		m_controllerInput,
		m_ready, 
		m_shield,
		m_powerupActivated;

	Vehicle* m_car; // pointer to an object in vehicle controller
	float m_timer;
	//TEMPORARY FOR DEMO
	enum m_keys {LEFT = 0, MIDDLE = 1, RIGHT = 2, BACK = 3};
	int m_playerState;

	//false/true allows to reverse the steering of the car
	bool			m_reverseSteering;
	//determine if the player needs to be updated.
	bool			m_activePlayer;

	//Pointer to the curren tpowerup that the player Holds, initialized when hit a powerup
	PowerUp*		m_currentPowerUp;

	//Permit or allow player to go past speed limit
	bool			m_enableSpeedLimit;

	//initial position of the car, used to reposition for each race(specific x,y for each car) z is from terrain
	XMFLOAT3		m_initialPosition;

	CollisionInfo m_collisionInformation;
	float m_renderDelay;
};

#endif