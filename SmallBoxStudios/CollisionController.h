#ifndef COLLISION_CONTROLLER
#define COLLISION_CONTROLLER

#include "PowerUpController.h"
#include "PlayerController.h"

#pragma region COLLISION_HANDLERS

///PLAYER POWERUP COLLISION
struct PlayerPowerUpCallback : public btCollisionWorld::ContactResultCallback
{
	virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		if (((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetPowerUpPtr() != (PowerUp*)colObj1Wrap->getCollisionObject()->getUserPointer())
		{
			if (((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetPowerUpPtr() != NULL)
			{
				((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetPowerUpPtr()->SetAlive(false);
				((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetPowerUpPtr()->Deactivate();
				((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetPowerUpPtr()->SetPlayerPtr(NULL);
				((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->SetPowerUpPtr(NULL);
			}


			((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetCollisionInfo().playerPowerUpCollision = true;
			((PowerUp*)colObj1Wrap->getCollisionObject()->getUserPointer())->SetPlayerPtr(((Player*)colObj0Wrap->getCollisionObject()->getUserPointer()));
			((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->SetPowerUpPtr(((PowerUp*)colObj1Wrap->getCollisionObject()->getUserPointer()));
		}
		return 0;
	}
};

///PLAYER PLAYER COLLISION
struct PlayerPlayerCallback : public btCollisionWorld::ContactResultCallback
{
	virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		btVector3 ptA = cp.getPositionWorldOnA();
		btVector3 ptB = cp.getPositionWorldOnB();
		((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetCollisionInfo().playerPlayerCollision = true;

		return 0;
	}
};

///PLAYER TERRAIN COLLISION
struct PlayerTerrainCallback : public btCollisionWorld::ContactResultCallback
{
	virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{

		btVector3 ptA = cp.getPositionWorldOnA();
		btVector3 ptB = cp.getPositionWorldOnB();
		((Player*)colObj0Wrap->getCollisionObject()->getUserPointer())->GetCollisionInfo().playerTerrainCollision = true;
		return 0;
	}
};

#pragma endregion

class CollisionController
{
public:
	CollisionController();
	~CollisionController();

	void LoadHandlers(PlayerController &_playerController, PowerUpController &_powerUpController, TerrainController &_terraiController, btDynamicsWorld &_dynamicsWorld);
	void Update(float dt);
private:
	PlayerController*		p_playerC;
	PowerUpController*		p_powerUpC;
	TerrainController*		p_terainC;
	btDynamicsWorld*		p_dynamicsWorld;
	
	PlayerPowerUpCallback	playerPowerUpCollisionCallback;
	PlayerPlayerCallback	PlayerPlayerCollisionHandler;
	PlayerTerrainCallback	PlayerTerrainCollisionHandler;
};

#endif