#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "PowerUp.h"
#include "ObstacleObject.h"


class Obstacle : public PowerUp
{
public:
	Obstacle(string filename, XMFLOAT4 _colour);
	~Obstacle();

	void Update(float dt);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);
	ObstacleObject& GetObstacle();
	XMFLOAT3 GetOffset();
	void Deactivate();

private:
	float m_mass;
	XMFLOAT3 m_offset;

	CellShaderObject m_cellShaderRenderer;
	ObstacleObject m_obstacle;
};

#endif