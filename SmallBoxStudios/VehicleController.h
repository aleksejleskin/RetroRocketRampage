#ifndef VEHICLE_CONTROLLER_H
#define VEHICLE_CONTROLLER_H

#include <iostream>
#include <vector>
#include <fstream>
//#include <sstream>
#include <sstream>
#include <string>
#include "Vehicle.h"
#include "Body.h"
#include "Wheel.h"
#include "Splitter.h"
#include "Spoiler.h"
#include "Retro.h"
#include "btBulletDynamicsCommon.h"

using std::vector;
using std::ifstream;
using std::ios;

struct BodyStruct
{
	vector<XMFLOAT3> anchors; 
	string modelFileName; 
	float mass;
	float yaw;
	float pitch;
	float roll;
	float scale;

	BodyStruct(vector<XMFLOAT3> _anchors, string _modelFileName, float _mass, float _yaw, float _pitch, float _roll, float _scale)
	{
		anchors = _anchors;
		modelFileName = _modelFileName;
		mass =_mass;
		yaw = _yaw;
		pitch = _pitch;
		roll = _roll;
		scale = _scale;
	}
};

struct WheelStruct
{
	string modelFileName;
	float mass;
	float yaw;
	float pitch;
	float roll;
	float scale;
	WheelStruct(string _modelFileName, float _mass, float _yaw, float _pitch, float _roll, float _scale)
	{
		modelFileName = _modelFileName;
		mass = _mass;
		yaw = _yaw;
		pitch = _pitch;
		roll = _roll;
		scale = _scale;
	}

};

struct RetroStruct
{
	string modelFileName; 
	float mass;
	float yaw;
	float pitch;
	float roll;
	float scale;
	float engineForce;
	RetroStruct(string _modelFileName, float _mass, float _yaw, float _pitch, float _roll, float _scale, float _engineForce)
	{
		modelFileName = _modelFileName;
		mass =_mass;
		yaw = _yaw;
		pitch = _pitch;
		roll = _roll;
		scale = _scale;
		engineForce = _engineForce;
	}
};

struct SpoilerStruct
{
	string modelFileName;
	float mass;
	float yaw;
	float pitch;
	float roll;
	float scale;

	SpoilerStruct(string _modelFileName, float _mass, float _yaw, float _pitch, float _roll, float _scale)
	{
		modelFileName = _modelFileName;
		mass = _mass;
		yaw = _yaw;
		pitch = _pitch;
		roll = _roll;
		scale = _scale;
	}
};

class VehicleController
{
public:
	VehicleController();
	~VehicleController();

	bool LoadComponents();
	void UnloadComponents();
	bool CheckGenerate(DxGraphics* dx, ResourceManager& resource);

	Vehicle* GetVehicle(int p);
	void SetPartGenerateType(PartType type);

private:
	bool LoadParts(string filename);

	int m_vehicleAmount;
	
	vector<RetroStruct> m_retroStructList;
	vector<BodyStruct> m_bodyStructList;
	vector<WheelStruct> m_frontWheelStructList;
	vector<WheelStruct> m_backWheelStructList;
	vector<SpoilerStruct> m_spoilerStructList;

	vector<Vehicle*> m_vehicles; 

	PartType m_newPartType;
};

#endif