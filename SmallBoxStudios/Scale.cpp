#include "Scale.h"

Scale::Scale(string filename, XMFLOAT4 _colour) : PowerUp(filename, _colour), m_scaleMultiplier(0)
{
//	m_powerUpId = powerUpType;
	test = false;
	m_powerUpId = 1;
}

Scale::~Scale()
{

}

void Scale::Update(float dt)
{
	//if(m_isAlive)
	//{
	//	//time of object being alive
	//	m_currentObjectTime += dt;
	//	//sin(0->1,22)(0->-2.44)
	//	//0.5 initial position that will be changed depending on sin;
	//	m_position.y = 0.5f * sinf( 2.0f * m_currentObjectTime );
	//}

	//if(!m_isAlive)
	//{
	//	if(m_playerId != -1 )
	//	{
	//		switch(m_state)
	//		{

	//		case CHECKACTIVATE:
	//			if(playerController.GetPlayer(m_playerId)->GetFire())
	//			{
	//				playerController.GetPlayer(m_playerId)->SetPowerUpId(-1);
	//				playerController.GetPlayer(m_playerId)->SetFire(false);
	//				m_state = ACTIVATE;

	//			}
	//			break;

	//		case ACTIVATE:
	//			//BREAKS IF THIS IS HERE AUDIO
	//			if(!test)
	//			{
	//			test = true;
	//			}

	//			m_scaleMultiplier = playerController.GetPlayer(m_playerId)->GetCar()->GetScale();
	//			if (m_scaleMultiplier < 1.2f)
	//			{
	//				m_scaleMultiplier += 0.05f *dt;
	//			}
	//			playerController.GetPlayer(m_playerId)->GetCar()->SetScale(m_scaleMultiplier);
	//			if (m_timer >= 20)
	//			{
	//				m_timer = 0.0f;
	//				m_state = DEACTIVATE;
	//			}
	//			m_timer += dt; // update timer
	//			break;

	//		case DEACTIVATE:
	//			m_scaleMultiplier = playerController.GetPlayer(m_playerId)->GetCar()->GetScale();
	//			if (m_scaleMultiplier > 1.0f)
	//			{
	//				m_scaleMultiplier -= 0.2f *dt;
	//			}
	//			playerController.GetPlayer(m_playerId)->GetCar()->SetScale(m_scaleMultiplier);

	//			if (m_timer >= 5.0f)
	//			{
	//				m_playerId = -1;
	//				m_timer = 0.0f;
	//				m_state = CHECKACTIVATE;
	//			}
	//			m_timer += dt; // update timer
	//			break;

	//		default:
	//			break;
	//		};
	//	}
	//}

	m_position = XMFLOAT3(36, 30, 80);
	PowerUp::Update(dt);
	//GameObject::Update(dt);
}

void Scale::Render(DxGraphics* dx, Camera& cam)
{
//	PowerUp::Render(dx, cam, lightManager);
}

btRigidBody* Scale::GetRigidBody()
{
	return m_rigidBody;
}

