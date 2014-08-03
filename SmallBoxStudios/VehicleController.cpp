#include "VehicleController.h"

VehicleController::VehicleController() : m_vehicleAmount(4)
{
	for (unsigned int v = 0; v < (unsigned)m_vehicleAmount; v++)
	{
		m_vehicles.push_back(new Vehicle());
	}
	m_newPartType = ALL_PARTS;
}

VehicleController::~VehicleController()
{
	//unload all the vehicles modules
}


Vehicle* VehicleController::GetVehicle(int p)
{
	return m_vehicles[p];
}

bool VehicleController::LoadComponents()
{
	if (!LoadParts("Parts.txt"))
	{
		return false;
	}
	return true;
}

void VehicleController::UnloadComponents()
{
	m_retroStructList.clear();
	m_retroStructList.shrink_to_fit();

	m_bodyStructList.clear();
	m_bodyStructList.shrink_to_fit();

	m_frontWheelStructList.clear();
	m_frontWheelStructList.shrink_to_fit();

	m_backWheelStructList.clear();
	m_backWheelStructList.shrink_to_fit();

	m_spoilerStructList.clear();
	m_spoilerStructList.shrink_to_fit();

	DeleteVector(m_vehicles);
}

bool VehicleController::CheckGenerate(DxGraphics* dx, ResourceManager& resources)
{
	for (unsigned int v = 0; v < 4; v++)
	{
		//check to see if the vehicle needs to generate
		if (m_vehicles[v]->GetGenerate())
		{
			int partType = m_newPartType;
			//Store component ids.
			int rndFrontWheels = 0;
			int rndBackWheels = 0;
			int rndSpoiler = 0;
			int rndRetros = 0;
			int rndBody = 0;

			//Check if car already exists , store the location to regenerate properly.
			btTransform previousT;
			btTransform wheels[4];
			XMFLOAT4X4 wheelsX[4];
			bool isLoaded = false;

			if (m_vehicles[v]->GetLoaded())
			{
				m_vehicles[v]->UnloadContent();
				//STORE NEW IDS
				rndBackWheels = m_vehicles[v]->GetCarPartList().backWheelsId;
				rndBody = m_vehicles[v]->GetCarPartList().bodyId;
				rndFrontWheels = m_vehicles[v]->GetCarPartList().frontWheelsId;
				rndRetros = m_vehicles[v]->GetCarPartList().retroId;
				rndSpoiler = m_vehicles[v]->GetCarPartList().spoilerId;

				if (partType == BACK_WHEELS || partType == ALL_PARTS)
				{
					if ((unsigned)rndBackWheels < m_backWheelStructList.size() - 1)
					{
						rndBackWheels++;
					}
					else
					{
						rndBackWheels = 0;
					}
				}

				if (partType == BODY || partType == ALL_PARTS)
				{
					if ((unsigned)rndBody < m_bodyStructList.size() - 1)
					{
						rndBody++;
					}
					else
					{
						rndBody = 0;
					}
				}

				if (partType == FRONT_WHEELS || partType == ALL_PARTS)
				{
					if ((unsigned)rndFrontWheels < m_frontWheelStructList.size() - 1)
					{
						rndFrontWheels++;
					}
					else
					{
						rndFrontWheels = 0;
					}
				}

				if (partType == RETROS || partType == ALL_PARTS)
				{
					if ((unsigned)rndRetros < m_retroStructList.size() - 1)
					{
						rndRetros++;
					}
					else
					{
						rndRetros = 0;
					}
				}

				if (partType == SPOILER || partType == ALL_PARTS)
				{
					if ((unsigned)rndSpoiler < m_spoilerStructList.size() - 1)
					{
						rndSpoiler++;
					}
					else
					{
						rndSpoiler = 0;
					}
				}

				isLoaded = true;
				previousT = m_vehicles[v]->GetRigidBody()->getWorldTransform();
				for (int x = 0; x < 4; x++)
				{
					wheels[x] = m_vehicles[v]->m_vehicle->getWheelInfo(v).m_worldTransform;
					wheelsX[x] = m_vehicles[v]->GetWheels()[x]->GetWorldMat();
				}
				//unload previous components
				m_vehicles[v]->UnloadContent();
			}
			//If the car does not exist(previously) set up random parts.
			else
			{
				//Safegaurd against loading incorrectly
				if( m_frontWheelStructList.size() == 0 ) return false;
				if( m_backWheelStructList.size() == 0 ) return false;
				if( m_spoilerStructList.size() == 0 ) return false;
				if( m_retroStructList.size() == 0 ) return false;
				if( m_bodyStructList.size() == 0 ) return false;

				//ASSIGN RANDOM IDS, IF NO CAR EXISTS
				rndFrontWheels = rand() % m_frontWheelStructList.size();
				rndBackWheels = rand() % m_backWheelStructList.size();
				//int rndSplitter = rand() % m_splitters.size();
				rndSpoiler = rand() % m_spoilerStructList.size();
				rndRetros = rand() % m_retroStructList.size();
				rndBody = rand() % m_bodyStructList.size();
			}


			//set the new components
			//WHEELS	
			for (int x = 2; x < 4; x++)
			{
				Wheel* wheel = new Wheel(
					m_backWheelStructList[rndBackWheels].modelFileName,
					m_backWheelStructList[rndBackWheels].mass,
					m_bodyStructList[rndBody].anchors[x],
					m_backWheelStructList[rndBackWheels].yaw,
					m_backWheelStructList[rndBackWheels].pitch,
					m_backWheelStructList[rndBackWheels].roll,
					m_backWheelStructList[rndBackWheels].scale);

				m_vehicles[v]->SetWheel(x, wheel);
			}

			for (int x = 0; x < 2; x++)
			{
				Wheel* wheel = new Wheel(
					m_frontWheelStructList[rndFrontWheels].modelFileName,
					m_frontWheelStructList[rndFrontWheels].mass,
					m_bodyStructList[rndBody].anchors[x],
					m_frontWheelStructList[rndFrontWheels].yaw,
					m_frontWheelStructList[rndFrontWheels].pitch,
					m_frontWheelStructList[rndFrontWheels].roll,
					m_frontWheelStructList[rndFrontWheels].scale);

				m_vehicles[v]->SetWheel(x, wheel);
			}

			for (int x = 0; x < 2; x++)
			{
				Retro* retro = new Retro(
					m_retroStructList[rndRetros].modelFileName,
					m_retroStructList[rndRetros].mass,
					//4 due to the offsets in file are 4 and 5 place
					m_bodyStructList[rndBody].anchors[x + 4],
					m_retroStructList[rndRetros].yaw,
					m_retroStructList[rndRetros].pitch,
					m_retroStructList[rndRetros].roll,
					m_retroStructList[rndRetros].scale,
					m_retroStructList[rndRetros].engineForce);
				m_vehicles[v]->SetRetro(x, retro);
			}

			Body* body = new Body(
				m_bodyStructList[rndBody].anchors,
				m_bodyStructList[rndBody].modelFileName,
				m_bodyStructList[rndBody].mass,
				m_bodyStructList[rndBody].yaw,
				m_bodyStructList[rndBody].pitch,
				m_bodyStructList[rndBody].roll,
				m_bodyStructList[rndBody].scale);
			m_vehicles[v]->SetBody(body);


			Spoiler* spoiler = new Spoiler(
				m_spoilerStructList[rndSpoiler].modelFileName, ///DOEASNT WORK
				m_spoilerStructList[rndSpoiler].mass,
				m_bodyStructList[rndBody].anchors[6],
				m_spoilerStructList[rndSpoiler].yaw,
				m_spoilerStructList[rndSpoiler].pitch,
				m_spoilerStructList[rndSpoiler].roll,
				m_spoilerStructList[rndSpoiler].scale);
			m_vehicles[v]->SetSpoiler(spoiler);
		
			//reload the components
			if (!m_vehicles[v]->LoadContent(dx, resources))
			{
				return false;
			}

			//STORE NEW IDS
			m_vehicles[v]->GetCarPartList().backWheelsId = rndBackWheels;
			m_vehicles[v]->GetCarPartList().bodyId = rndBody;
			m_vehicles[v]->GetCarPartList().frontWheelsId = rndFrontWheels;
			m_vehicles[v]->GetCarPartList().retroId = rndRetros;
			m_vehicles[v]->GetCarPartList().spoilerId = rndSpoiler;

			if (isLoaded)
			{
				m_vehicles[v]->GetRigidBody()->setWorldTransform(previousT);
				for (int x = 0; x < 4; x++)
				{
					m_vehicles[v]->m_vehicle->getWheelInfo(v).m_worldTransform = wheels[x];
					m_vehicles[v]->GetWheels()[x]->SetWorldMat(wheelsX[x]);
				}
			}
			//reset the generate message back to false;
			m_vehicles[v]->SetGenerate(false);
		}
	}
	return true;
}

bool VehicleController::LoadParts(string filename)
{
	ifstream file;
	string line = "";
	bool done = false;
	string modelFilename = "";
	int partType = -1; // holds the part type,
	//0 = BODY, 1 = WHEEL, 2 = SPOILER, 3 = SPLITTER, 4 = RETRO
	float mass = 0.0f,
		downForce = 0.0f,
		force = 0.0f,
		yaw = 0.0f,
		pitch = 0.0f,
		roll = 0.0f,
		scale = 0.0f,
		steeringForce = 0.0f,
		suspensionResting = 0.0f;
	XMFLOAT3 offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	vector<XMFLOAT3> anchors;

	XMFLOAT3 FLW = XMFLOAT3(0.0f, 0.0f, 0.0f)
		, FRW = XMFLOAT3(0.0f, 0.0f, 0.0f)
		, BLW = XMFLOAT3(0.0f, 0.0f, 0.0f)
		, BRW = XMFLOAT3(0.0f, 0.0f, 0.0f)
		, LR = XMFLOAT3(0.0f, 0.0f, 0.0f)
		, RR = XMFLOAT3(0.0f, 0.0f, 0.0f)
		, Spoiler = XMFLOAT3(0.0f, 0.0f, 0.0f);

	file.open(filename, ios::in);

	if (file.is_open())
	{
		while (!done)
		{
			getline(file, line);

			if (line == "#END")
			{
				done = true;
				break;
			}

			//Gets the part type from the file
			if (!line.empty())
			{
				if (line[0] == '0' || line[0] == '1' || line[0] == '2' || line[0] == '3' || line[0] == '4' || line[0] == '5')
				{
					stringstream ss;
					ss << line;
					ss >> partType;
					switch (partType)
					{
						//BODY
					case 0:
					{
							  // string filename;
							  stringstream ss;
							  string type;
							  ss << line;
							  ss >> type >> modelFilename >> mass >> scale >> yaw >> pitch >> roll
								  >> FLW.x >> FLW.y >> FLW.z
								  >> FRW.x >> FRW.y >> FRW.z
								  >> BLW.x >> BLW.y >> BLW.z
								  >> BRW.x >> BRW.y >> BRW.z
								  >> LR.x >> LR.y >> LR.z
								  >> RR.x >> RR.y >> RR.z
								  >> Spoiler.x >> Spoiler.y >> Spoiler.z
								  ;


							  anchors.push_back(FLW);
							  anchors.push_back(FRW);
							  anchors.push_back(BLW);
							  anchors.push_back(BRW);
							  anchors.push_back(LR);
							  anchors.push_back(RR);
							  anchors.push_back(Spoiler);

							  m_bodyStructList.push_back(BodyStruct(anchors, modelFilename, mass, yaw, pitch, roll, scale));

							  anchors.clear();

							  break;
					}

						//Front Wheel
					case 1:
					{
							  // string filename;
							  stringstream ss;
							  string type;
							  ss << line;
							  ss >> type >> modelFilename >> mass >> scale >> yaw >> pitch >> roll;

							  m_frontWheelStructList.push_back(WheelStruct(modelFilename, mass, yaw, pitch, roll, scale));

							  break;
					}

						//Back Wheel
					case 2:
					{
							  // string filename;
							  stringstream ss;
							  string type;
							  ss << line;
							  ss >> type >> modelFilename >> mass >> scale >> yaw >> pitch >> roll;

							  m_backWheelStructList.push_back(WheelStruct(modelFilename, mass, yaw, pitch, roll, scale));

							  break;
					}

						//Retro
					case 3:
					{
							  // string filename;
							  stringstream ss;
							  string type;
							  float engineForce = 0;
							  ss << line;
							  ss >> type >> modelFilename >> mass >> scale >> yaw >> pitch >> roll >> engineForce;

							  m_retroStructList.push_back(RetroStruct(modelFilename, mass, yaw, pitch, roll, scale, engineForce));

							  break;
					}

						//Spoiler
					case 4:
					{
							  // string filename;
							  stringstream ss;
							  string type;
							  ss << line;
							  ss >> type >> modelFilename >> mass >> scale >> yaw >> pitch >> roll;

							  m_spoilerStructList.push_back(SpoilerStruct(modelFilename, mass, yaw, pitch, roll, scale));

							  break;
					}
					}
				}
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void VehicleController::SetPartGenerateType(PartType type)
{
	m_newPartType = type;
}