#include "PowerUp.h"

PowerUp::PowerUp(string filename,XMFLOAT4 _colour) : PhysicsObject(), 
	//m_powerUpType(powerUpType),
	//m_powerUpId(powerUpId),
	m_isAlive(false), 
	m_activate(false),
	m_durationTime(0),
	m_timer(0)
{
	m_shutdown = false;
	m_timeTillRespawn = 0;
	m_filename = filename;
	m_currentPlayer = NULL;
	Colour = _colour;
	f_dt = 0;
}

PowerUp::~PowerUp()
{

}

bool PowerUp::LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
	float yaw, float pitch, float roll, float scale)
{
	m_dynamicsWorld = resource.GetPhysicsManager()->GetWorld();

	if (!GameObject::LoadContent(dx, position, m_yaw, m_pitch, m_roll, m_scale))
	{
		return false;
	}
	if (!GameObject::BuildModel(dx->GetDevice(), m_filename, resource))
	{
		return false;
	}
	if (!GameObject::BuildShader("cellShader.fx", resource))
	{
		return false;
	}

	if (!m_cellShader.LoadShader(dx->GetDevice(), false))
	{
		return false;
	}

	if (!BuildInputLayout(dx))
	{
		return false;
	}

	if (!CreateRigidBody(100.0f,XMFLOAT3(0,0,0),btCollisionObject::CF_NO_CONTACT_RESPONSE))
	{
		return false;
	}
	m_audioManager = resource.GetAudioManager();




	return true;
}

void PowerUp::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	ColourFade(dx, 0.5, Colour, f_dt);
	//GameObject::Render(dx, cam);
	m_cellShader.Render(dx->GetImmediateContext(), m_inputLayout,
		m_vBuffer, m_iBuffer, m_model->indexCount, cam, lightManager, m_worldMat);
}

void PowerUp::Update(float dt)
{
	//dt used for colour fade in render
	f_dt = dt;


	if (m_currentPlayer != NULL)
	{
		XMFLOAT3 pPos;
		pPos = m_currentPlayer->GetVehiclePosition();
		pPos.y += 5.5f;
		SetPosition(pPos);
		
		SetRotation( XMFLOAT3( m_yaw += 5.5f * dt, 0.0f, 0.0f ) );
	}
	else
	{
	SetRotation(XMFLOAT3(m_yaw += 5.5f*dt, 0.0f, 0.0f));
	btVector3 pPos;
	pPos = m_rigidBody->getWorldTransform().getOrigin();
	pPos.setY(-1.0f);
	XMFLOAT3 pos = XMFLOAT3(pPos.getX(),pPos.getY(),pPos.getZ());
	SetPosition(pos);
	}

	

	PhysicsObject::Update(dt);
}

void PowerUp::UnloadContent()
{

	GameObject::UnloadContent();
}

///SET GET
bool PowerUp::GetAlive()
{
	return m_isAlive;
}

void PowerUp::SetAlive(bool isAlive)
{
	m_isAlive = isAlive;
}

int PowerUp::GetPowerUpType()
{
	return m_powerUpType;
}

void PowerUp::SetPowerUpType(int powerId)
{
	m_powerUpType = powerId;
}


void PowerUp::SetPlayerPtr(Player* _player)
{
	m_currentPlayer = _player;
}

XMFLOAT4 PowerUp::getColour()
{
	return Colour;
}

void PowerUp::Activate(bool _isActive)
{
	m_activate = _isActive;
}

Player* PowerUp:: GetPlayerPtr()
{
	return m_currentPlayer;
}

float PowerUp::GetRespawnTime()
{
	return m_timeTillRespawn;
}

void PowerUp::SetRespawnTime(float _timer)
{
	m_timeTillRespawn = _timer;
}

void PowerUp::SetPlayerControllerPointer(PlayerController *_playerControllerPointer)
{
	ptr_playerController = _playerControllerPointer;
}

void PowerUp::Deactivate()
{
	m_currentPlayer = NULL;
}