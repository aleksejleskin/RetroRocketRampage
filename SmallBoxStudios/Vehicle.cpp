#include "Vehicle.h"

Vehicle::Vehicle() : PhysicsObject(), m_wheelType(0), m_bodyType(0),
m_splitterType(0), m_spoilerType(0), m_alive(true), m_shutdown(false), m_generate(true),
m_loaded(false)
{
	//Set Vehicle colour to white
	m_vehicleColour = XMFLOAT4(1, 1, 1, 1);
	m_retroColour = XMFLOAT4(112, 128, 144, 1);
	m_scale = 0;
	m_alive = true;
	m_activePlayer = false;
	m_fadeColour = false;
	//default colour of the car.
	SetColour(XMFLOAT4(1, 1, 1, 1));
	m_fadeRetroColour = false;
	m_defaultRetroColour = XMFLOAT4(128, 128, 128, 0);

	//populate vector of pointers with null pointers.
	m_retros.push_back(NULL);
	m_retros.push_back(NULL);
	m_wheels.push_back(NULL);
	m_wheels.push_back(NULL);
	m_wheels.push_back(NULL);
	m_wheels.push_back(NULL);
	m_body = NULL;
	m_splitter = NULL;
	m_spoiler = NULL;
}

Vehicle::~Vehicle()
{
	if (!m_shutdown)
	{
		UnloadContent();
	}
}

bool Vehicle::LoadContent(DxGraphics* dx, ResourceManager& resources)
{
	m_dynamicsWorld = resources.GetPhysicsManager()->GetWorld();

	//Loads the content of all the modules
	if (!m_body->LoadContent(dx, m_body->GetPosition(), resources, m_body->GetYaw(), m_body->GetPitch(),
		m_body->GetRoll(), m_scale + m_body->GetScale()))
	{
		return false;
	}
	for (unsigned int i = 0; i < 4; i++)
	{
		if (i == 0 || i == 1)
		{
			if (!m_retros[i]->LoadContent(dx, m_retros[i]->GetPosition(), resources, m_retros[i]->GetYaw(),
				m_retros[i]->GetPitch(), m_retros[i]->GetRoll(), m_scale + m_retros[i]->GetScale()))
			{
				return false;
			}
		}

		if (!m_wheels[i]->LoadContent(dx, m_wheels[i]->GetPosition(), resources, m_wheels[i]->GetYaw(),
			m_wheels[i]->GetPitch(), m_wheels[i]->GetRoll(), m_scale + m_wheels[i]->GetScale()))
		{
			return false;
		}
	}

	if (!m_spoiler->LoadContent(dx, m_spoiler->GetPosition(), resources, m_spoiler->GetYaw(),
		m_spoiler->GetPitch(), m_spoiler->GetRoll(), m_scale + m_spoiler->GetScale()))
	{
		return false;
	}


	//ASSIGN CAR INFORMATION
	m_vehicleInfo.m_engineForce = m_retros[0]->engineForce;
	m_vehicleInfo.m_defaultEngineForce = m_retros[0]->engineForce;
	m_vehicleInfo.m_frictionSlip = BT_LARGE_FLOAT;
	m_vehicleInfo.m_rollInfluence = 0.0001f;
	m_vehicleInfo.m_suspensionRestLength1 = 0.2f;
	m_vehicleInfo.m_suspensionStiffness = 40.0f;
	m_vehicleInfo.m_wheelsDampingCompression = 4.4f; //4.4
	m_vehicleInfo.m_wheelsDampingRelaxation = 10.2f; //20
	m_vehicleInfo.m_steeringForce = 0.05f;
	m_vehicleInfo.m_breakingForce = 100;
	m_vehicleInfo.m_maxSteeringAngle = 0.10f;

	//Calcualte mass of the car, based on components.
	m_mass = m_body->GetWeight() + m_spoiler->GetWeight() + (m_wheels[0]->GetWeight() * 4) + (m_retros[0]->GetWeight() * 2);

	//initialize the compound, start adding all the aprts together
	m_vehicleCompound = new btCompoundShape();

	//CREATE VEHICLE PARTS

	//BODY
	m_vehicleCompound->addChildShape(m_body->GetLocalTranslation(), m_body->CreateCollisionShape());

	//RETROS
	for (int x = 0; x < 2; x++)
	{
		m_vehicleCompound->addChildShape(m_retros[x]->GetLocalTranslation(), m_retros[x]->CreateCollisionShape());
	}
	//SPOILER
	m_vehicleCompound->addChildShape(m_spoiler->GetLocalTranslation(), m_spoiler->CreateCollisionShape());


	//set the car Transform 
	btTransform tr;
	tr.setIdentity();
	tr.setOrigin(btVector3(m_position.x, m_position.y, m_position.z));
	//Combine all the parts into a rigid body object.
	m_rigidBody = localCreateRigidBody(m_mass, tr, m_vehicleCompound);


	/// Create raycasting for the vechicle and attach it to the car
	{
		m_vehicleRayCaster = new btDefaultVehicleRaycaster(m_dynamicsWorld);
		m_vehicle = new btRaycastVehicle(m_tuning, m_rigidBody, m_vehicleRayCaster);
		///never deactivate the vehicle
		m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
		//add the vehicle to the simualtion.
		m_dynamicsWorld->addVehicle(m_vehicle);

		//Set up coordiante system , Y-up, Z-forward
		int rightIndex = 0;
		int upIndex = 1;
		int forwardIndex = 2;
		m_vehicle->setCoordinateSystem(rightIndex, upIndex, forwardIndex);

		bool isFrontWheel = true;
		btVector3 wheelDirectionCS0(0.0f, -1.0f, 0.0f);
		btVector3 wheelAxleCS(-1.0f, 0.0f, 0.0f);
		btScalar suspensionRestLength(0.2f);

		//initialize wheels
		for (int x = 0; x < 4; x++)
		{
			XMFLOAT3 colExt = m_wheels[x]->GetCollisionExtents();
			float	wheelRadius = colExt.y;
			float	wheelWidth = colExt.z;
			//determine which wheels are at the back
			if (x > 2) isFrontWheel = false;

			XMFLOAT3 offset = m_wheels[x]->GetOffset();
			btVector3 connectionPointCS0 = btVector3(offset.x, offset.y, offset.z);
			m_vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, m_tuning, isFrontWheel);
		}
		//Rotate the wheel for the other side.
		m_wheels[1]->SetYaw(m_wheels[2]->GetYaw() + XM_PI);
		m_wheels[3]->SetYaw(m_wheels[3]->GetYaw() + XM_PI);

		//Wheel parameters.
		for (int i = 0; i < m_vehicle->getNumWheels(); i++)
		{
			btWheelInfo& wheel = m_vehicle->getWheelInfo(i);
			wheel.m_suspensionStiffness = m_vehicleInfo.m_suspensionStiffness;
			wheel.m_wheelsDampingRelaxation = m_vehicleInfo.m_wheelsDampingRelaxation;
			wheel.m_wheelsDampingCompression = m_vehicleInfo.m_wheelsDampingCompression;
			wheel.m_frictionSlip = m_vehicleInfo.m_frictionSlip;
			wheel.m_rollInfluence = m_vehicleInfo.m_rollInfluence;
			wheel.m_suspensionRestLength1 = m_vehicleInfo.m_suspensionRestLength1;
		}
	}

	m_loaded = true;
	return true;
}

void Vehicle::UnloadContent()
{

	//Unloads all the content of the modules
	if (m_loaded)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			m_wheels[i]->UnloadContent();
		}
		m_retros[0]->UnloadContent();
		m_retros[1]->UnloadContent();
		m_spoiler->UnloadContent();
		m_body->UnloadContent();


		btCollisionObject* obj = m_rigidBody;
		btRigidBody* body2 = btRigidBody::upcast(obj);

		m_dynamicsWorld->removeRigidBody(m_rigidBody);
		m_dynamicsWorld->removeVehicle(m_vehicle);

	}
}

void Vehicle::Update(float dt)
{

	//set the position variables to the position of the car.
	m_position.x = m_vehicle->getChassisWorldTransform().getOrigin().getX();
	m_position.y = m_vehicle->getChassisWorldTransform().getOrigin().getY();
	m_position.z = m_vehicle->getChassisWorldTransform().getOrigin().getZ();
	//update Vehicle with controlls
	int wheelIndex = 2;
	m_vehicle->applyEngineForce(m_vehicleValues.m_engineForce, wheelIndex);
	m_vehicle->setBrake(m_vehicleValues.m_breakingForce, wheelIndex);

	wheelIndex = 3;
	m_vehicle->applyEngineForce(m_vehicleValues.m_engineForce, wheelIndex);
	m_vehicle->setBrake(m_vehicleValues.m_breakingForce, wheelIndex);

	wheelIndex = 0;
	m_vehicle->setSteeringValue(m_vehicleValues.m_steering, wheelIndex);
	wheelIndex = 1;
	m_vehicle->setSteeringValue(m_vehicleValues.m_steering, wheelIndex);


	//Center the wheels if they are offset.
	if (m_vehicleValues.m_steering > 0)
	{
		m_vehicleValues.m_steering -= 1.6f *dt;
	}
	if (m_vehicleValues.m_steering < 0)
	{
		m_vehicleValues.m_steering += 1.6f *dt;
	}
	if (m_vehicleValues.m_steering < 0.1f && m_vehicleValues.m_steering > -0.1f)
	{
		m_vehicleValues.m_steering = 0.0f;
	}

	///UPDATE BODY
	btTransform parentMat = m_vehicle->getChassisWorldTransform();
	m_body->Update(dt, parentMat);


	//UPDATE WHEELS 
	for (unsigned int i = 0; i < 4; i++)
	{
		m_vehicle->updateWheelTransform(i, true);
		m_wheels[i]->Update(dt, m_vehicle->getWheelInfo(i).m_worldTransform);
	}

	//UPDATE RETROS 
	for (unsigned int i = 0; i < 2; i++)
	{
		m_retros[i]->Update(dt, parentMat);
	}

	//temp for changing colour
	dtt = dt;


	//	m_splitter.Update(dt,m_worldMat);
	m_spoiler->Update(dt, parentMat);

}

void Vehicle::Render(DxGraphics* dx, Camera& cam, LightManager& lightManager)
{

	if (m_fadeColour)
	{
		m_body->ColourFade(dx, 1.5, m_vehicleColour, dtt);
	}
	if (m_fadeRetroColour)
	{
		m_retros[0]->ColourFade(dx, 0.5, m_retroColour, dtt);
		m_retros[1]->ColourFade(dx, 0.5, m_retroColour, dtt);
	}

	// call Render all the car parts
	m_body->Render(dx, cam, lightManager);
	for (unsigned int i = 0; i < 4; i++)
	{
		m_wheels[i]->Render(dx, cam, lightManager);
	}
	m_retros[0]->Render(dx, cam, lightManager);
	m_retros[1]->Render(dx, cam, lightManager);


	m_spoiler->Render(dx, cam, lightManager);
}

void Vehicle::SetBody(Body *body)
{
	m_body = body;
}

void Vehicle::SetWheel(int _wheelId, Wheel *wheel)
{
	m_wheels[_wheelId] = wheel;
}
void Vehicle::SetSplitter(Splitter *splitter)
{
	m_splitter = splitter;
}
void Vehicle::SetSpoiler(Spoiler *spoiler)
{
	m_spoiler = spoiler;
}
void Vehicle::SetRetro(int _retroId, Retro *retros)
{
	m_retros[_retroId] = retros;
}

bool Vehicle::GetGenerate()
{
	return m_generate;
}

void Vehicle::SetGenerate(bool generate)
{
	m_generate = generate;
}

vector<Wheel*> Vehicle::GetWheels()
{
	return m_wheels;
}

Body* Vehicle::GetBody()
{
	return m_body;
}
Retro* Vehicle::GetRetro(int _retroId)
{
	return m_retros[_retroId];
}

//Vehicle Getting parameters
void Vehicle::SetSteeringResetForce(float _steeringResetForce)
{
	m_steeringResetForce = _steeringResetForce;
}

void Vehicle::SetVehicleColour(XMFLOAT4 _vehicleColour)
{
	m_vehicleColour = _vehicleColour;
}

void Vehicle::SetFadeColour(bool fade)
{
	m_fadeColour = fade;
}

void Vehicle::SetPlayerActive(bool active)
{
	m_activePlayer = active;
}

bool Vehicle::GetLoaded()
{
	return m_loaded;
}

void Vehicle::FadeRetroColour(bool _fade)
{
	m_fadeRetroColour = _fade;
}
void Vehicle::SetRetroColour(XMFLOAT4 _colour)
{
	m_retroColour = _colour;
}

XMFLOAT4 Vehicle::GetDefaultRetroColour()
{
	return m_defaultRetroColour;
}

CurrentCarparts &Vehicle::GetCarPartList()
{
	return m_vehiclePartIDs;
}

VehicleInfo& Vehicle::GetVehicleInfo()
{
	return m_vehicleInfo;
}

VehicleValues& Vehicle::GetVehicleValues()
{
	return m_vehicleValues;
}