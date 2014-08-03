#ifndef INLCUDE_GUARD_GLYPH_H
#define INLCUDE_GUARD_GLYPH_H


#include "CellShaderObject.h"
#include "PhysicsObject.h"

using namespace std;

class Glyph : public PhysicsObject
{
public:
	Glyph();
	Glyph( string filename, float yaw, float pitch, float roll, float scale, bool physics );
	~Glyph();

	bool LoadContent(string filename, float yaw,
		float pitch, float roll, float scale, bool physics, DxGraphics *dx, XMFLOAT3 position, ResourceManager& resource);
	void CreateRigidBody(btDynamicsWorld* dynamicsWorld, float mass );
	
	void UnloadContent();

	void Update(float dt);
	void Render(DxGraphics* dx, Camera& cam, LightManager& lightManager);

	bool GetLoaded();
	XMFLOAT4 GetColour();
	void SetColour(XMFLOAT4 _colour);

	void SetLetterOffset(XMFLOAT3 _letterOffset);
	XMFLOAT3 GetLetterOffset();
	void ColourFadeSwitch(bool _switch);

	void SetLoadedGlyph(bool _loaded);
	bool GetLoadedGlyph();
private:
	bool				m_loaded,
						m_physicsObj;

	CellShaderObject	m_cellShader;

	//Colour of the letter, used for colourfade
	XMFLOAT4			m_colour;
	XMFLOAT3			m_letterOffset;
	float				m_dt;
	bool			m_colourFadeSwitch;
	bool			m_loadedGlyph;
};

#endif