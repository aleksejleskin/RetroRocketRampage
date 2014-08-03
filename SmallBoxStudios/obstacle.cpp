#include "obstacle.h"

Obstacle::Obstacle(string filename, XMFLOAT4 _colour) : PowerUp(filename, _colour), m_obstacle("Shield.sbs", 1)
{

}

Obstacle::~Obstacle()
{
}


void Obstacle::Update(float dt)
{
	
	//if powerups is dead disable collision detection , ( not to have ghost object , that can collide)
	if (!m_isAlive)
	{
		//m_obstacle.GetRigidBody()->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	if (m_activate && (m_currentPlayer != NULL))
	{	
		m_timer += dt;
		//SET UP COLLISION OBJECT
		if (m_timer < 0.2)
		{
			m_obstacle.GetRigidBody()->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
			XMFLOAT3 pos = m_currentPlayer->GetCar()->GetPosition();
			btTransform currentT;
			currentT = m_rigidBody->getWorldTransform();
			btVector3 currentPos = btVector3(pos.x, pos.y + 4, pos.z + 75);
			currentT.setOrigin(currentPos);
			m_obstacle.GetRigidBody()->setWorldTransform(currentT);
		}

		//REMOVE POWERUP IF INACTIVE
		if (m_timer > 5)
		{
			m_timer = 0;
			m_activate = false;
			m_isAlive = false;
			m_currentPlayer = NULL;

		}
		
	}
	m_obstacle.Update(dt);
	PowerUp::Update(dt);
}

void Obstacle::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	m_obstacle.Render(dx, cam, lightManager);
	PowerUp::Render(dx, cam,  lightManager);
}

ObstacleObject& Obstacle::GetObstacle()
{
	return m_obstacle;
}

void Obstacle::Deactivate()
{
	if (m_currentPlayer != NULL)
	{
		m_timer = 0;
		m_activate = false;
		m_isAlive = false;
		m_currentPlayer = NULL;
	}
}