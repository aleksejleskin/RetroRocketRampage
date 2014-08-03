#include "Wheel.h"

Wheel::Wheel() : PhysicsObject(), m_offset(XMFLOAT3(0.0f, 0.0f, 0.f))
{
}

Wheel::Wheel(string filename, float mass, XMFLOAT3 offset,
		float yaw, float pitch, float roll, float scale) :
		PhysicsObject(), m_offset(offset)
{
	m_model->filename = filename;
	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;
	m_scale = scale;
	m_mass = mass;
}

Wheel::~Wheel()  
{
}

bool Wheel::LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
		float yaw, float pitch, float roll, float scale)
{
	if(!GameObject::LoadContent(dx, position, yaw, pitch, roll, scale))
	{
		return false;
	}
	if(!GameObject::BuildModel(dx->GetDevice(), m_model->filename, resource))
	{
		return false;
	}
	if(!GameObject::BuildShader("cellShader.fx", resource))
	{
		return false;
	}
	if(!BuildInputLayout(dx))
	{
		return false;
	}

	//Load shaders constant buffers
	//if(!m_cellShaderRenderer.LoadShader(dx->GetDevice(), false))
	if(!m_cellShaderRenderer.LoadShader(dx->GetDevice(),
		m_shader->vShader, m_shader->pShader, false))
	{
		return false;
	}
	CalcualteBoxExtents();

	return true;
}

void Wheel::UnloadContent()
{
	m_cellShaderRenderer.UnloadShader();
	GameObject::UnloadContent();
}

void Wheel::Update(float dt,btTransform & parentTransform)
{
	///ROTATION
	//get the quaternion rotation from euler angles using bt
	//offset rotation(initial rotation)
	btQuaternion btLocalRot = btQuaternion(m_yaw, m_pitch, m_roll);
	XMMATRIX localRot = XMMatrixRotationQuaternion(XMLoadFloat4(&XMFLOAT4(btLocalRot.getX(), btLocalRot.getY(), btLocalRot.getZ(), btLocalRot.getW())));
	//worldspace rotation
	btQuaternion btParentRot = parentTransform.getRotation();
	XMMATRIX ParentRot = XMMatrixRotationQuaternion(XMLoadFloat4(&XMFLOAT4(btParentRot.getX(), btParentRot.getY(), btParentRot.getZ(), btParentRot.getW())));
	//add rotations together
	XMMATRIX finalRot = localRot *  ParentRot;
	
	
	btVector3 btparentPos = parentTransform.getOrigin();
	XMMATRIX translationMat = XMMatrixTranslation(btparentPos.getX(), btparentPos.getY(), btparentPos.getZ());
	//XMMATRIX scaleMat = XMMatrixScaling(m_scale, m_scale, m_scale);
	XMMATRIX scaleMat = XMMatrixScaling(m_scale, m_scale, m_scale);
	XMMATRIX worldMat = finalRot * scaleMat * translationMat;
	XMStoreFloat4x4(&m_worldMat, worldMat);
}

void Wheel::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	//m_cellShaderRenderer.Render(dx->GetImmediateContext(), m_inputLayout,
		//m_model, cam, lightManager, m_worldMat);
	m_cellShaderRenderer.Render(dx->GetImmediateContext(), m_inputLayout,
		m_vBuffer, m_iBuffer, m_model->indexCount, cam, lightManager, m_worldMat);
}

XMFLOAT3 Wheel::GetOffset()
{
	return m_offset;
}