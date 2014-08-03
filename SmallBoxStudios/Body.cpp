#include "Body.h"

Body::Body() : PhysicsObject()
{
}

Body::~Body()
{
}

Body::Body(vector<XMFLOAT3> anchorPoints, string filename, float mass, float yaw, float pitch, float roll, float scale) : 
	PhysicsObject(), m_anchorPoints(anchorPoints)
{
	m_model->filename = filename;
	m_yaw = yaw;
	m_pitch = pitch;
	m_roll = roll;
	m_scale = scale;
	m_mass = mass;
}

bool Body::LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
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

	//Load the shaders buffers
	//if(!m_cellShadingRender.LoadShader(dx->GetDevice(), false))
	if(!m_cellShadingRender.LoadShader(dx->GetDevice(),
		m_shader->vShader, m_shader->pShader, false))
	{
		return false;
	}
	CalcualteBoxExtents();

	return true;
}

void Body::UnloadContent()
{
	m_cellShadingRender.UnloadShader();
	GameObject::UnloadContent();
}

void Body::Update(float dt, btTransform& Parent)
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
	XMMATRIX matrixFinalRotation = matrixChildRotation * matrixParentRotation;

	XMFLOAT3 m_offset = XMFLOAT3(0, 0, 0);
	XMMATRIX childOffset = XMMatrixTranslation(m_offset.x, m_offset.y, m_offset.z);

	XMMATRIX zeroWorld = XMMatrixTranslation(0, 0, 0);

	XMMATRIX translationMat = XMMatrixTranslation(parentPos.getX(), parentPos.getY(), parentPos.getZ());
	XMMATRIX scaleMat = XMMatrixScaling(m_scale, m_scale, m_scale);
	XMMATRIX worldMat = childOffset * matrixFinalRotation * zeroWorld * scaleMat * translationMat;

	XMStoreFloat4x4(&m_worldMat, worldMat);
}

void Body::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	//m_cellShadingRender.Render(dx->GetImmediateContext(),
		//m_inputLayout, m_model, cam, lightManager, m_worldMat);
	m_cellShadingRender.Render(dx->GetImmediateContext(), m_inputLayout,
		m_vBuffer, m_iBuffer, m_model->indexCount, cam, lightManager, m_worldMat);
}

XMFLOAT3 Body::GetAnchorPoint(int point)
{
	return m_anchorPoints[point];
}

