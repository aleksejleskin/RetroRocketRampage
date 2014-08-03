#include "Retro.h"

Retro::Retro() : PhysicsObject(),
m_offset(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	rot = XMFLOAT3(0, 0, 0);
}

Retro::Retro(string filename, float mass,
	XMFLOAT3 offset, float yaw, float pitch, float roll, float scale, float _engineForce) :
	PhysicsObject(), m_offset(offset)
{
	engineForce = _engineForce;
	m_model->filename = filename;
	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;
	m_scale = scale;
	m_mass = mass;
	rot = XMFLOAT3(0, 0, 0);
}

Retro::~Retro()
{
}

bool Retro::LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
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

	//Load the shader constant buffers
	if(!m_cellShaderRenderer.LoadShader(dx->GetDevice(),
		m_shader->vShader, m_shader->pShader, false))
	{
		return false;
	}
	CalcualteBoxExtents();
	return true;
}

void Retro::UnloadContent()
{
	m_cellShaderRenderer.UnloadShader();
	GameObject::UnloadContent();
}

void Retro::Update(float dt, btTransform& Parent)
{
	btVector3 parentPos;
	parentPos = Parent.getOrigin();

	//Get Offset Rotation of a model(child)
	btQuaternion LocalRot;
	LocalRot = btQuaternion(m_yaw, m_pitch, m_roll);

	btQuaternion parentRotation;
	parentRotation = Parent.getRotation();

	//Construct Directx Matricies out of give Quaternion rotations, sets up proper transformation of a model, Global + local
	XMMATRIX matrixChildRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&XMFLOAT4(LocalRot.getX(), LocalRot.getY(), LocalRot.getZ(), LocalRot.getW())));
	XMMATRIX matrixParentRotation = XMMatrixRotationQuaternion(XMLoadFloat4(&XMFLOAT4(parentRotation.getX(), parentRotation.getY(), parentRotation.getZ(), parentRotation.getW())));

	//calcualte the final rotation by multiplying quaternions using dxmatrix
	XMMATRIX matrixFinalRotation = matrixParentRotation;

	XMMATRIX childOffset = XMMatrixTranslation(m_offset.x, m_offset.y, m_offset.z);

	
	XMMATRIX zeroWorld = XMMatrixTranslation(0, 0, 0);
	//TEMPORARY!!!
	XMMATRIX childRotation = XMMatrixRotationRollPitchYaw(m_roll, m_pitch, m_yaw);

	XMMATRIX translationMat = XMMatrixTranslation(parentPos.getX(), parentPos.getY(), parentPos.getZ());
	//setUp gameObject position variable
	m_position.x = parentPos.getX();
	m_position.y = parentPos.getY();
	m_position.z = parentPos.getZ();

	XMMATRIX scaleMat = XMMatrixScaling(m_scale,m_scale,m_scale);
	XMMATRIX worldMat = XMMatrixIdentity();
	worldMat =
		childRotation * 
		childOffset * 
		matrixFinalRotation * 
		zeroWorld * 
		scaleMat * 
		translationMat;

	XMStoreFloat4x4(&m_worldMat, worldMat);
}

void Retro::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	m_cellShaderRenderer.Render(dx->GetImmediateContext(), m_inputLayout,
		m_vBuffer, m_iBuffer, m_model->indexCount, cam, lightManager, m_worldMat);
}

XMFLOAT3 Retro::GetOffset()
{
	return m_offset;
}

void Retro::SetOffset(XMFLOAT3 off)
{
	 m_offset = off;
}