#define FRONT_LEFT_WHEEL 0
#define FRONT_RIGHT_WHEEL 1

#define BACK_LEFT_WHEEL 2
#define BACK_RIGHT_WHEEL 3

#define RIGHT_RETRO 0
#define LEFT_RETRO 1

//FOR PARTS GENERATION
enum PartType
{
	BODY,
	FRONT_WHEELS,
	BACK_WHEELS,
	RETROS,
	SPOILER,
	ALL_PARTS = 10,
};

struct CurrentCarparts
{
	int frontWheelsId,
	backWheelsId,
	bodyId,
	spoilerId,
	retroId;

};

struct VehicleInfo
{
	float m_suspensionStiffness;
	float m_wheelsDampingRelaxation;
	float m_wheelsDampingCompression;
	float m_frictionSlip;
	float m_rollInfluence;
	float m_suspensionRestLength1;
	float m_steeringForce;
	float m_engineForce;
	float m_defaultEngineForce;
	float m_breakingForce;
	float m_maxSteeringAngle;
};

struct VehicleValues
{
	float m_engineForce;
	float m_steering;
	float m_breakingForce;
};