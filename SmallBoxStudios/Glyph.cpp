#include "Glyph.h"

Glyph::Glyph()
{
	m_loadedGlyph = false;
}

Glyph::Glyph(string filename, float yaw,
	float pitch, float roll, float scale, bool physics)
{
	m_model->filename = filename;
	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;
	m_scale = scale;
	m_loaded = true;
	m_physicsObj = physics;
	m_colour = XMFLOAT4(0, 0, 0, 0);
	m_letterOffset = XMFLOAT3(0, 0, 0);
	m_dt = 0;
	m_colourFadeSwitch = false;
	m_loadedGlyph = false;
}

Glyph::~Glyph()
{
	if (m_loaded)
	{
		//m_dynamicsWorld->removeCollisionObject(m_rigidBody);
	}
}

bool Glyph::LoadContent(string filename, float yaw,
	float pitch, float roll, float scale, bool physics, DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource)
{
	m_model->filename = filename;
	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;
	m_scale = scale;
	m_loaded = true;
	m_physicsObj = physics;
	m_colour = XMFLOAT4(0, 0, 0, 0);
	m_letterOffset = XMFLOAT3(0, 0, 0);
	m_dt = 0;
	m_colourFadeSwitch = false;
	m_loadedGlyph = false;

	if (!GameObject::LoadContent(dx, position, m_yaw, m_pitch, m_roll, m_scale))
	{
		return false;
	}
	if (!GameObject::BuildModel(dx->GetDevice(), m_model->filename, resource))
	{
		return false;
	}
	if (!GameObject::BuildShader("cellShader.fx", resource))
	{
		return false;
	}

	if( !m_cellShader.LoadShader( dx->GetDevice(), false ) )
	{
		return false;
	}

	if (!BuildInputLayout(dx))
	{
		return false;
	}
	
	return true;
}

void Glyph::CreateRigidBody( btDynamicsWorld* dynamicsWorld, float mass )
{
	m_dynamicsWorld = dynamicsWorld;

	CalcualteBoxExtents();
	//set a specific size for collision box.
	m_collisionBoxExtents = XMFLOAT3(0.55f * m_scale, 0.85f * m_scale, 0.85f * m_scale);
	//SETTING UP A COLLISION OBEJCT(terrain).
	btBoxShape* letterShape = new btBoxShape(btVector3(btScalar(m_collisionBoxExtents.x), btScalar(m_collisionBoxExtents.y), btScalar(m_collisionBoxExtents.z)));
	//setupthe object and add to the world
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(m_position.x, m_position.y, m_position.z));
	tr.setRotation(btQuaternion(m_yaw, m_pitch, m_roll));
	m_rigidBody = localCreateRigidBody(mass, tr, letterShape);
	m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags()
		| btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );

}

void Glyph::UnloadContent()
{
	//m_dynamicsWorld->removeCollisionObject(m_rigidBody);
	PhysicsObject::UnloadContent();
}

void Glyph::Update(float dt)
{
	m_dt = dt;
	if(m_physicsObj)
	{
		PhysicsObject::Update(dt);
	}
	else GameObject::Update( dt );
}

void Glyph::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	if (m_colourFadeSwitch)
	{
		ColourFade(dx, 0.85f, m_colour, m_dt);
	}

	m_cellShader.Render(dx->GetImmediateContext(), m_inputLayout,
		m_vBuffer, m_iBuffer, m_model->indexCount, cam, lightManager, m_worldMat);
}

bool Glyph::GetLoaded()
{
	return m_loaded;
}

XMFLOAT4 Glyph::GetColour()
{
	return m_colour;
}

void Glyph::SetColour(XMFLOAT4 _colour)
{
	m_colour = _colour;
}

void Glyph::SetLetterOffset(XMFLOAT3 _letterOffset)
{
	m_letterOffset = _letterOffset;
}

XMFLOAT3 Glyph::GetLetterOffset()
{
	return m_letterOffset;
}

void Glyph::ColourFadeSwitch(bool _switch)
{
	m_colourFadeSwitch = _switch;
}

void Glyph::SetLoadedGlyph(bool _loaded)
{
	m_loadedGlyph = _loaded;
}

bool Glyph::GetLoadedGlyph()
{
	return m_loadedGlyph;
}