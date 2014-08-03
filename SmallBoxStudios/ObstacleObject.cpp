#include "ObstacleObject.h"
ObstacleObject::ObstacleObject(string filename, float mass) : PhysicsObject()
{
	m_filename = filename;
	Colour = XMFLOAT4(218, 165, 32, 1);
	m_scale = 3;
}

ObstacleObject::~ObstacleObject()
{

}

bool ObstacleObject::LoadContent(DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource,
	float yaw, float pitch, float roll, float scale)
{
	m_dynamicsWorld = resource.GetPhysicsManager()->GetWorld();
	if (!GameObject::LoadContent(dx, position, m_yaw, m_pitch, m_roll, m_scale))
	{
		return false;
	}
	if (!GameObject::BuildModel(dx->GetDevice(), "CollisionObject.sbs", resource))
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
	//m_audioManager = resource.GetAudioManager();

	CalcualteBoxExtents();
	btBoxShape* groundShape = new btBoxShape(btVector3(m_collisionBoxExtents.x - 2, m_collisionBoxExtents.y-7, m_collisionBoxExtents.z - 1));
//	groundShape->initializePolyhedralFeatures();
	

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(position.x, position.y, position.z));
	btQuaternion quatRot;
	quatRot.setEuler(yaw, pitch, roll);
	groundTransform.setRotation(quatRot);
	m_rigidBody = localCreateRigidBody(100000.0f, groundTransform, groundShape);
	//setupthe object and add to the world
	//m_rigidBody = localCreateRigidBody(100., groundTransform, groundShape);
	m_rigidBody->setCollisionFlags(btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	m_rigidBody->setGravity(btVector3(0, -10, 0));
	return true;
}

void ObstacleObject::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{
	m_cellShader.Render(dx->GetImmediateContext(), m_inputLayout,
		m_vBuffer, m_iBuffer, m_model->indexCount, cam, lightManager, m_worldMat);
}

void ObstacleObject::Update(float dt)
{

	m_rigidBody->setLinearVelocity(btVector3(0, -10, 0));
	btTransform Parent;
	Parent = m_rigidBody->getWorldTransform();
	btVector3 parentPos;
	parentPos = Parent.getOrigin();
	m_position = XMFLOAT3(parentPos.getX(), parentPos.getY(), parentPos.getZ());

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


	XMMATRIX translationMat = XMMatrixTranslation(parentPos.getX(), parentPos.getY(), parentPos.getZ());
	XMMATRIX scaleMat = XMMatrixScaling(m_scale, m_scale, m_scale);
	XMMATRIX worldMat = matrixFinalRotation * scaleMat * translationMat;

	XMStoreFloat4x4(&m_worldMat, worldMat);
}

void ObstacleObject::UnloadContent()
{
	GameObject::UnloadContent();
}



