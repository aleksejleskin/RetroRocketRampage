#ifndef POWERUP_H 
#define POWERUP_H

#include "PhysicsObject.h"
#include "Camera.h"
#include "Timer.h"
#include "Player.h"
#include "PlayerController.h"
#include "CellShaderObject.h"
//class HelicopterCamera;
class Player;
class PlayerController;
//class PowerUpController;

class PowerUp : public PhysicsObject
{
public:
	PowerUp(string filename, XMFLOAT4 _colour);
	~PowerUp();

	virtual bool LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
		float yaw, float pitch, float roll, float scale);

	void UnloadContent();
	virtual void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	virtual void Update(float dt);
	virtual void Deactivate();
	bool GetAlive();
	void SetAlive(bool isAlive);

	int GetPowerUpID();
	void SetPowerUpID(int powerId);

	//Type SCALE BOOST BUSRT RESPAWN
	int GetPowerUpType();
	void SetPowerUpType(int powerType);

	//CURRENT PLAYER WHO IS HOLDING THE POWERUP
	void SetPlayerId(int player);
	void SetPlayerPtr(Player* _player);
	Player* GetPlayerPtr();

	//Colour For specific PowerUp.
	XMFLOAT4 getColour(); 

	void SetPlayerControllerPointer(PlayerController *_playerControllerPointer);
	void Activate(bool _isActive);

	float GetRespawnTime();
	void SetRespawnTime(float _timer);
protected:
	XMFLOAT4 Colour;

	AudioManager* m_audioManager;
	bool m_isAlive,
		m_shutdown;

	float m_timeTillRespawn;
	//int m_powerUpId;
	string m_filename;
	bool m_activate;
	float m_timer; 
	float m_durationTime; //how long until powerup is deactivated
	int m_powerUpType;
	Player* m_currentPlayer;
	PlayerController* ptr_playerController;

	float				f_dt;
	//CellShading
	CellShaderObject	m_cellShader;
};

#endif 