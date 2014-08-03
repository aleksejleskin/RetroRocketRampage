// -----------------------------------------------------
//	Contains the vehicle class that will act as a parent
//	to all the modules of the car
// -----------------------------------------------------

#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <stdlib.h>
#include <ctime>

#include "DxGraphics.h"
#include "Body.h"
#include "Wheel.h"
#include "Retro.h"
#include "Spoiler.h"
#include "Splitter.h"
#include "PhysicsObject.h"
#include "LightManager.h"
#include "VehicleInfo.h"
class Vehicle : public PhysicsObject
{
public:
	Vehicle();
	~Vehicle();

	bool LoadContent(DxGraphics* dx, ResourceManager& resources);
	void UnloadContent();

	void Update(float dt);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);

	// Getters for Vehicle Components

	//change cars colour.

	Body	*GetBody();
	Retro	*GetRetro(int _retroId);
	void	GetWheels(Wheel * _wheels);


	void	SetBody(Body *body);
	void	SetWheel(int _wheelId, Wheel *wheel);
	void	SetSplitter(Splitter *splitter);
	void	SetSpoiler(Spoiler *spoiler);
	void	SetRetro(int _wheelId, Retro *retro);

	bool	GetGenerate();
	void	SetGenerate(bool generate);


	void		SetSteeringResetForce(float _steeringResetForce);
	void		SetVehicleColour(XMFLOAT4 _vehicleColour);
	void		SetFadeColour(bool fade);
	void		SetPlayerActive(bool active);
	bool		GetLoaded();

	XMFLOAT4 GetDefaultRetroColour();
	VehicleInfo &GetVehicleInfo();
	VehicleValues& GetVehicleValues();
	vector<Wheel*>	GetWheels();


	void FadeRetroColour(bool _fade);
	void SetRetroColour(XMFLOAT4 _colour);

	btRaycastVehicle*	m_vehicle;
	//the base of the vechicle, all parts are attached to this bt Obect;


	CurrentCarparts & GetCarPartList();

private:
	btRaycastVehicle::btVehicleTuning	m_tuning;
	btVehicleRaycaster*	m_vehicleRayCaster;
	btCompoundShape* m_vehicleCompound;

	bool m_fadeColour;
	bool m_fadeRetroColour;
	//gameobjects for all the modules
	Body *m_body;
	vector<Wheel*> m_wheels;
	vector<Retro*> m_retros;
	Splitter *m_splitter;
	Spoiler *m_spoiler;
	//
	CurrentCarparts m_vehiclePartIDs;


	int m_wheelType,
		m_bodyType,
		m_splitterType,
		m_spoilerType;

	bool m_alive,
		m_shutdown,
		m_activePlayer,
		m_generate,
		m_loaded;

	float	m_steeringResetForce; //force that will restore the steering to zero

	XMFLOAT4	m_vehicleColour;
	XMFLOAT4	m_retroColour;
	XMFLOAT4    m_defaultRetroColour;

	VehicleInfo m_vehicleInfo;
	VehicleValues m_vehicleValues;
protected:

	float dtt;
};

#endif

